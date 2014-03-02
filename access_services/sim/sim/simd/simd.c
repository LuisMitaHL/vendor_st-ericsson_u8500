/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * File name       : simd.c
 * Description     : SIM Card daemon
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */



#include "catd.h"
#include "uiccd.h"
#include "msgq.h"
#include "sim_internal.h"
#include "cat_barrier.h"
#include "catd_reader.h"
#include "simd.h"
#include "sim_unused.h"
#include "catd_modem.h"
#include "client_data.h"
#include "func_trace.h"
#include "util_security.h"
#include "catd_cn_client.h"
#include <signal.h>

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>

#include <sys/stat.h>

#include "simd_timer.h"
#include "sim_log.h"

#ifdef CFG_USE_ANDROID_LOG
// If this is an Android build, then use the Android logging mechanism.
#define LOG_TAG "RIL_SIMD"
#include <cutils/log.h>
#endif

#ifndef HOST_FS_PATH
#define HOST_FS_PATH ""
#endif

#ifndef HAVE_ANDROID_OS
#define SIM_INIT_DEBUG_LEVEL    HOST_FS_PATH "/dev/null"
#define MODEM_INIT_DEBUG_LEVEL  HOST_FS_PATH "/dev/null"
#else
#define DEBUG_LEVEL_ROOT        "/etc"
#define SIM_INIT_DEBUG_LEVEL    DEBUG_LEVEL_ROOT "/simd_debug_level.ini"
#define MODEM_INIT_DEBUG_LEVEL  DEBUG_LEVEL_ROOT "/modem_debug_level.ini"
#endif

// -----------------------------------------------------------------------------
// Static variables

#define SIMD_NUM_TIMERS (16)

#define STARTUP_UNDEFINED 0
#define STARTUP_NONE 1
#define STARTUP_IN_PROGRESS 2
#define STARTUP_DONE 3
#define STARTUP_FAILED 4
static int startup_state;
// Count for responses. Set to 2, and then decremented when uiccd/catd
// did their init stuff. When zero, startup is done.
static int startup_count;

static int              opt_daemonise = 1;
static sim_logging_t    opt_v = SIM_LOGGING_E;
static sim_logging_t    modem_v = SIM_LOGGING_E;

static ste_msgq_t      *simd_mq = 0;    /* simd main message queue */
static uint8_t          pin_required_flag;


ste_catd_reader_t      *catd_rdr = 0;

int stdin_fd = -1;
int stderr_fd = -1;
int stdout_fd = -1;
int modem_reset_status = 0;

// Config options to be read from file
#define SIM_CONFIG_FILE HOST_FS_PATH "/system/etc/sim_config"

uint8_t ENABLE_VIVO_ROAMING_BROKER = 0;
typedef struct {
    char* opt_name;
    uint8_t *opt_val;
} config_options_t;
static config_options_t config_options[] = {
    { "ENABLE_VIVO_ROAMING_BROKER", &ENABLE_VIVO_ROAMING_BROKER },
    { NULL, NULL } // Last Parameter
};

static void catd_flush_log_messages();

// -----------------------------------------------------------------------------
// Modem abstraction access.

static ste_modem_t     *static_modem;

ste_modem_t            *catd_get_modem()
{
    assert(static_modem);
    return static_modem;
}


static void catd_set_modem(ste_modem_t * m)
{
    static_modem = m;
}


// -----------------------------------------------------------------------------
// Command handling
// .. WARNING: executed by the reader thread!

static void simd_do_startup(int fd, uintptr_t client_tag);
static void simd_sig_set_sim_debug_level(int fd, uintptr_t client_tag, int level);
static void simd_sig_set_modem_debug_level(int fd, uintptr_t client_tag, int level);
static void simd_read_config_options(void);

void
ste_simd_handle_command(uint16_t cmd, uintptr_t client_tag,
                        const char *buf,uint16_t len,
                        ste_sim_client_context_t * cc)
{
    const char             *bb = buf;
    buf = bb;
    switch (cmd) {

    case STE_SIM_REQ_PING:
        simd_sig_ping(cc->fd, client_tag);
        break;
    case STE_SIM_REQ_STARTUP:
        // simd_sig_startup(cc->fd, client_tag);
        simd_do_startup(cc->fd, client_tag);
        break;
    case STE_SIM_REQ_SHUTDOWN:
        simd_sig_shutdown(cc->fd, client_tag);
        break;
    case STE_SIM_REQ_SET_SIM_DEBUG_LEVEL:
        {
            int level;
            memcpy(&level,buf,sizeof(level));
            simd_sig_set_sim_debug_level(cc->fd, client_tag, level);
        }
        break;
    case STE_SIM_REQ_SET_MODEM_DEBUG_LEVEL:
        {
            int level;
            memcpy(&level,buf,sizeof(level));
            simd_sig_set_modem_debug_level(cc->fd, client_tag, level);
        }
        break;

    default:
        catd_log_f(SIM_LOGGING_E, "read : BAD COMMAND: %d len=%d", cmd, len);
    }
}

static void simd_do_startup(int fd, uintptr_t client_tag)
{
    ste_modem_t *m;
    int i;
    catd_log_f(SIM_LOGGING_I, "simd : STARTUP message received");
    if (startup_state == STARTUP_NONE) {
      startup_state = STARTUP_IN_PROGRESS;
      startup_count = 2;
      m = catd_get_modem();
      if (!m) {
        catd_log_f(SIM_LOGGING_E, "simd : ste_catd_modem_new failed");
        catd_flush_log_messages();
        abort();
      }
      i = ste_catd_modem_connect(m, 1);
      if (i) {
        catd_log_f(SIM_LOGGING_E,"simd : failed to connect to modem");
        catd_flush_log_messages();
        abort();
      }

#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
      uiccd_msr_init_pin_caching();
#endif
      // Super user access rights not needed after modem_connect
      util_continue_as_non_privileged_user();
    }
    uiccd_sig_startup(fd, client_tag);
    catd_sig_startup(fd, client_tag);
}


// -----------------------------------------------------------------------------
// Registering event stream with reader thread.

int catd_add_es(ste_es_t * es)
{
    if (!es || !catd_rdr)
        return -1;
    return ste_catd_reader_add_es(catd_rdr, es);
}

int catd_rem_es(int fd)
{
    if (!catd_rdr)
        return -1;
    return ste_catd_reader_rem_es(catd_rdr, fd);
}


// -----------------------------------------------------------------------------
// Messages

typedef enum {
    SIMD_MSG_FIRST = STE_MSG_RANGE_SIMD,
#ifdef CFG_USE_ANDROID_LOG
    SIMD_MSG_UNUSED_IN_ANDROID,
#else
    SIMD_MSG_LOG,
#endif
    SIMD_MSG_PING,
    SIMD_MSG_SHUTDOWN,
    SIMD_MSG_CONNECT,
    SIMD_MSG_DISCONNECT,
    SIMD_MSG_STARTUP,
    SIMD_MSG_STARTUP_COMPLETED,
    SIMD_MSG_SET_SIM_DEBUG_LEVEL,
    SIMD_MSG_SET_MODEM_DEBUG_LEVEL,
    SIMD_MSG__MAX                              /* highest msg id no */
} SIMD_MSG_TYPES_t;


#ifndef CFG_USE_ANDROID_LOG
/* min size for log messages: 64-4-4-8 (common, level, alen, malloc overhead) */
#define SIMD_LOG_MIN_SIZE       (40)
typedef struct {
    STE_MSG_COMMON;
    sim_logging_t           level;      /* log level */
    size_t                  alen;       /* Allocated size */
    char                    buf[SIMD_LOG_MIN_SIZE];     /* NUL terminated! */
} catd_msg_log_t;

catd_msg_log_t         *catd_msg_log_create(size_t n, sim_logging_t level, uintptr_t client_tag);
void                    catd_msg_log_cpy(catd_msg_log_t * msg,
                                         const char *str, size_t n);
#endif





// -----------------------------------------------------------------------------
// Generic file descriptor messages
typedef struct {
    STE_MSG_COMMON;
    int                     fd;
} simd_msg_fd_t;

typedef simd_msg_fd_t  simd_msg_ping_t;
typedef simd_msg_fd_t  simd_msg_connect_t;
typedef simd_msg_fd_t  simd_msg_disconnect_t;
typedef simd_msg_fd_t  simd_msg_shutdown_t;
typedef simd_msg_fd_t  simd_msg_startup_t;

typedef struct {
  STE_MSG_COMMON;
  int fd;
  int daemon;
  int status;
} simd_msg_startup_completed_t;

typedef struct {
    STE_MSG_COMMON;
    int fd;
    int level;
} simd_msg_debug_level_t;

simd_msg_ping_t       *simd_msg_ping_create(int fd, uintptr_t client_tag);
simd_msg_connect_t    *simd_msg_connect_create(int fd, uintptr_t client_tag);
simd_msg_disconnect_t *simd_msg_disconnect_create(int fd, uintptr_t client_tag);
simd_msg_shutdown_t   *simd_msg_shutdown_create(int fd, uintptr_t client_tag);
simd_msg_startup_t   *simd_msg_startup_create(int fd, uintptr_t client_tag);
simd_msg_startup_completed_t   *simd_msg_startup_completed_create(int fd, int daemon, int status, uintptr_t client_tag);
simd_msg_debug_level_t  *simd_msg_set_sim_debug_level_create(int fd, uintptr_t client_tag, int level);
simd_msg_debug_level_t  *simd_msg_set_modem_debug_level_create(int fd, uintptr_t client_tag, int level);

// -----------------------------------------------------------------------------
// file descriptor messages

static simd_msg_fd_t  *simd_msg_fd_create(int type, int fd, uintptr_t client_tag)
{
    simd_msg_fd_t         *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = type;
        p->delete_func = 0;
        p->client_tag = client_tag;
        p->fd = fd;
    }
    return p;
}



simd_msg_ping_t       *simd_msg_ping_create(int fd, uintptr_t client_tag)
{
    simd_msg_ping_t       *p = simd_msg_fd_create(SIMD_MSG_PING, fd, client_tag);
    return p;
}



simd_msg_connect_t    *simd_msg_connect_create(int fd, uintptr_t client_tag)
{
    simd_msg_connect_t    *p = simd_msg_fd_create(SIMD_MSG_CONNECT, fd, client_tag);
    return p;
}



simd_msg_disconnect_t *simd_msg_disconnect_create(int fd, uintptr_t client_tag)
{
    simd_msg_disconnect_t *p =
        simd_msg_fd_create(SIMD_MSG_DISCONNECT, fd, client_tag);
    return p;
}


simd_msg_startup_t    *simd_msg_startup_create(int fd, uintptr_t client_tag)
{
    return simd_msg_fd_create(SIMD_MSG_STARTUP, fd, client_tag);
}

simd_msg_startup_completed_t   *simd_msg_startup_completed_create(int fd, int daemon, int status, uintptr_t client_tag)
{
  simd_msg_startup_completed_t *p;
  p = malloc(sizeof(*p));
  if (p) {
    p->type = SIMD_MSG_STARTUP_COMPLETED;
    p->delete_func = 0;
    p->client_tag = client_tag;
    p->fd = fd;
    p->daemon = daemon;
    p->status = status;
  }
  return p;
}

simd_msg_shutdown_t    *simd_msg_shutdown_create(int fd, uintptr_t client_tag)
{
    return simd_msg_fd_create(SIMD_MSG_SHUTDOWN, fd, client_tag);
}

simd_msg_debug_level_t  *simd_msg_set_sim_debug_level_create(int fd, uintptr_t client_tag, int level)
{
    simd_msg_debug_level_t  *p = NULL;
    p = malloc(sizeof(*p));
    if (p) {
      p->type = SIMD_MSG_SET_SIM_DEBUG_LEVEL;
      p->delete_func = 0;
      p->client_tag = client_tag;
      p->fd = fd;
      p->level = level;
    }
    return p;
}

simd_msg_debug_level_t  *simd_msg_set_modem_debug_level_create(int fd, uintptr_t client_tag, int level)
{
    simd_msg_debug_level_t  *p = NULL;
    p = malloc(sizeof(*p));
    if (p) {
      p->type = SIMD_MSG_SET_MODEM_DEBUG_LEVEL;
      p->delete_func = 0;
      p->client_tag = client_tag;
      p->fd = fd;
      p->level = level;
    }
    return p;
}

static void simd_read_config_options(void)
{
    FILE *fp;
    char line[80];
    char*param_pos_p = NULL;
    uint8_t i;

    fp = fopen(SIM_CONFIG_FILE,"r");
    if ( fp == NULL ) {
        /* File Does Not exist. Use already set default values */
        return;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        /* Ignore empty or comment lines */
        if (1 > strlen(line) || '#' == line[0]) {
            continue;
        }
        param_pos_p = NULL;
        for ( i = 0; config_options[i].opt_name != NULL; i++ ) {
            param_pos_p = strstr( line, config_options[i].opt_name );
            if ( param_pos_p != NULL ) {
                *config_options[i].opt_val = atoi(param_pos_p + strlen(config_options[i].opt_name) + 1);
                break;
            }
        }

    }
    fclose(fp);
}


#ifndef CFG_USE_ANDROID_LOG
// -----------------------------------------------------------------------------
// Log messages
catd_msg_log_t         *catd_msg_log_create(size_t n, sim_logging_t level, uintptr_t client_tag)
{
    static size_t           overhead = (sizeof(catd_msg_log_t) -
                                        sizeof(char) * SIMD_LOG_MIN_SIZE);
    size_t                  size;
    catd_msg_log_t         *p;
    if (n < SIMD_LOG_MIN_SIZE)
        n = SIMD_LOG_MIN_SIZE;
    size = n + overhead;
    p = malloc(size);
    if (p) {
        p->type = SIMD_MSG_LOG;
        p->delete_func = 0;
        p->client_tag = client_tag;
        p->level = level;
        p->alen = n;
        p->buf[0] = '\0';
    }
    return p;
}



void catd_msg_log_cpy(catd_msg_log_t * msg, const char *str, size_t n)
{
    static const char       bad_str[] = "bad arg to catd_msg_log_cpy";
    if (!str || !*str)
        str = bad_str;
    if (!n)
        n = strlen(str);
    if (n >= msg->alen)
        n = msg->alen;
    strncpy(msg->buf, str, n);
    msg->buf[n - 1] = '\0';
}






// -----------------------------------------------------------------------------
// Log printing

static void mprint(const char *str)
{
    static const char       bad_str[] = "BAD arg to mprint";

    if (!str)
        str = bad_str;

    if (opt_daemonise) {
      printf("D:%s\n",str);
    } else {
        fputs(str, stdout);
        fputs("\n", stdout);
    }
}


static void catd_log_msg_send(catd_msg_log_t * msg)
{
    ste_msgq_add(simd_mq, (ste_msg_t *) msg);
}
#endif


void catd_log_s(sim_logging_t level, const char *str, uintptr_t client_tag)
{
#ifdef CFG_USE_ANDROID_LOG

    if(sim_get_log_level() < level)
    return;

    switch ( level ) {

    case SIM_LOGGING_E:
        SIM_LOG_E("%s",str);
        break;
    case SIM_LOGGING_I:
        SIM_LOG_I("%s",str);
        break;
    case SIM_LOGGING_D:
        SIM_LOG_D("%s",str);
        break;
    case SIM_LOGGING_V:
        SIM_LOG_V("%s",str);
        break;
    default:
        SIM_LOG_E("%s",str);
        break;
    }

#else
    catd_msg_log_t         *msg;
    size_t n = strlen(str) + 1;
    msg = catd_msg_log_create(n, level, client_tag);
    if (msg) {
        catd_msg_log_cpy(msg, str, n);
        catd_log_msg_send(msg);
    }
#endif
}

#define PRINT_BUF_SIZE  (512)

void catd_log_f(sim_logging_t level, const char *fmt, ...)
{
    char                   *buf = malloc(PRINT_BUF_SIZE);
    va_list                 arg;
    va_start(arg, fmt);
    vsnprintf(buf, PRINT_BUF_SIZE, fmt, arg);
    buf[PRINT_BUF_SIZE - 1] = '\0';
    catd_log_s(level, buf, 0);
    va_end(arg);
    free(buf);
}

void catd_log_b(sim_logging_t level, const char *str, const void *buf, size_t n)
{
    static const char       hex_digit[] = "0123456789abcdef";
    char*                   dst_buf = 0;
    char*                   src = 0;
    char*                   src_end = 0;
    char*                   dst = (char *)buf;
    size_t                  len = n*2 + 1; // 2 chars each for hex string + null terminator
    size_t                  str_len = 0;

    if (str) {
        str_len = strlen(str);
    }
    len += str_len;

    dst = dst_buf = malloc(len);
    if (!dst_buf) {
        return;
    }
    if (str) {
        strcpy(dst, str);
        dst += str_len;
    }

    src_end = (char *)buf + n; // reusing len
    for (src = (char *)buf; src < src_end; src++) {
        uint8_t msb = (*src >> 4) & 0xf; // right shift signed entity shifts in sign bit (compiler dependant)
        uint8_t lsb = *src & 0xf;
        *dst++ = hex_digit[msb];
        *dst++ = hex_digit[lsb];
    }
    *dst = '\0';

    catd_log_s(level, dst_buf, 0);

    free(dst_buf);
}

void catd_log_a(sim_logging_t level, const char *str, const ste_apdu_t * apdu)
{
    size_t                alen;
    const uint8_t         *praw;

    praw = ste_apdu_get_raw(apdu);
    alen = ste_apdu_get_raw_length(apdu);

    catd_log_b(level, str, praw, alen);
}

#ifndef CFG_USE_ANDROID_LOG
void catd_main_log(ste_msg_t * ste_msg)
{
    catd_msg_log_t         *msg = (catd_msg_log_t *) ste_msg;
    if (msg->level <= opt_v)
        mprint(msg->buf);
    ste_msg_delete(ste_msg);
}
#endif



static void catd_flush_log_messages()
{
#ifndef CFG_USE_ANDROID_LOG
    ste_msg_t              *msg;
    for (;;) {
        msg = ste_msgq_try_pop(simd_mq);
        if (!msg)
            break;
        if (msg->type == SIMD_MSG_LOG) {
            catd_main_log(msg);
        } else {
            ste_msg_delete(msg);
        }
    }
#endif
}


// -----------------------------------------------------------------------------
// Ping

void simd_sig_ping(int fd, uintptr_t client_tag)
{
    simd_msg_ping_t        *msg;
    msg = simd_msg_ping_create(fd, client_tag);
    if (msg) {
        ste_msgq_add(simd_mq, (ste_msg_t *) msg);
    }
}



void simd_main_ping(ste_msg_t * ste_msg)
{
    simd_msg_ping_t        *msg = (simd_msg_ping_t *) ste_msg;
    catd_log_f(SIM_LOGGING_I, "simd : PING message received fd=%d", msg->fd);
    sim_send_generic(msg->fd, STE_SIM_RSP_PING, 0, 0, msg->client_tag);

    ste_msg_delete(ste_msg);
}



// -----------------------------------------------------------------------------
// Connect


void simd_sig_connect(int fd, uintptr_t client_tag)
{
    simd_msg_connect_t     *msg;
    msg = simd_msg_connect_create(fd, client_tag);
    if (msg) {
        ste_msgq_add(simd_mq, (ste_msg_t *) msg);
    }
}



void simd_main_connect(ste_msg_t * ste_msg)
{
    simd_msg_connect_t     *msg = (simd_msg_connect_t *) ste_msg;
    ste_sim_client_data_t   cd;
    int                     rv;

    catd_log_f(SIM_LOGGING_I, "simd : CONNECT message received fd=%d", msg->fd);

    cd.fd = msg->fd;
    rv = ste_sim_client_data_register(&cd);
    if ( rv != 0 ) {
        catd_log_f(SIM_LOGGING_E, "simd : ste_sim_client_data_register failed=%d", rv);
    }

    // No reply as this is generated from the reader thread
    ste_msg_delete(ste_msg);
}


// -----------------------------------------------------------------------------
// Disconnect

void simd_sig_disconnect(int fd, uintptr_t client_tag)
{
    simd_msg_disconnect_t  *msg;
    msg = simd_msg_disconnect_create(fd, client_tag);
    if (msg) {
        ste_msgq_add(simd_mq, (ste_msg_t *) msg);
    }

    // Also send the signals to cat and uicc to allow them to deregister
    // any client that is registered for proactive commands and
    // pin needed notifications.
    uiccd_sig_disconnect(fd, client_tag);
}



void simd_main_disconnect(ste_msg_t * ste_msg)
{
    simd_msg_disconnect_t  *msg = (simd_msg_disconnect_t *) ste_msg;
    catd_log_f(SIM_LOGGING_I, "simd : DISCONNECT message received fd=%d", msg->fd);

    // No reply as this is generated from the reader thread
    ste_sim_client_data_deregister(msg->fd);
    ste_msg_delete(ste_msg);
}

// -----------------------------------------------------------------------------
// Startup

void simd_sig_startup(int fd, uintptr_t client_tag)
{
    simd_msg_startup_t    *msg;
    msg = simd_msg_startup_create(fd, client_tag);
    if (msg) {
        ste_msgq_add(simd_mq, (ste_msg_t *) msg);
    }
}


void simd_main_startup(ste_msg_t * ste_msg)
{
    simd_msg_startup_t    *msg = (simd_msg_startup_t *) ste_msg;

    catd_log_f(SIM_LOGGING_E,"simd : simd_main_startup SHOULD NOT EXECUTE!");
    simd_do_startup(msg->fd, msg->client_tag);
    ste_msg_delete(ste_msg);
}

void simd_sig_startup_completed(int fd, int daemon, int status, uintptr_t client_tag)
{
    simd_msg_startup_completed_t    *msg;
    msg = simd_msg_startup_completed_create(fd, daemon, status, client_tag);
    if (msg) {
        ste_msgq_add(simd_mq, (ste_msg_t *) msg);
    }
}
void simd_main_startup_completed(ste_msg_t * ste_msg)
{
  simd_msg_startup_completed_t *msg = (simd_msg_startup_completed_t *)ste_msg;

  catd_log_f(SIM_LOGGING_I, "simd : STARTUP COMPLETED. daemon=%d status=%d startup_count=%d", msg->daemon, msg->status, startup_count);
  if (startup_state != STARTUP_IN_PROGRESS) {
    ste_msg_delete(ste_msg);
    return;
  }
  --startup_count;
  if (msg->status != 0) {
    startup_state = STARTUP_FAILED;
    // sim_send_generic(msg->fd, STE_SIM_RSP_STARTUP, 0, 0, msg->client_tag);
    ste_msg_delete(ste_msg);
    return;
  }
  if (startup_count == 0) {
    startup_state = STARTUP_DONE;
    catd_log_f(SIM_LOGGING_I, "simd : STARTUP COMPLETED");
    sim_send_generic(msg->fd, STE_SIM_RSP_STARTUP, 0, 0, msg->client_tag);
    //catd_sig_startup_done(msg->fd, msg->client_tag);
    //uiccd_sig_startup_done(msg->fd, msg->client_tag);
  }
  ste_msg_delete(ste_msg);
}

// -----------------------------------------------------------------------------
// Shutdown

void simd_sig_shutdown(int fd, uintptr_t client_tag)
{
    simd_msg_shutdown_t    *msg;
    msg = simd_msg_shutdown_create(fd, client_tag);
    if (msg) {
        ste_msgq_add(simd_mq, (ste_msg_t *) msg);
    }
}


void simd_main_shutdown(ste_msg_t * ste_msg)
{
    simd_msg_shutdown_t    *msg = (simd_msg_shutdown_t *) ste_msg;

    catd_log_s(SIM_LOGGING_I, "simd : SHUTDOWN message received", msg->client_tag);

    if(msg->fd != -1) {
      sim_send_generic(msg->fd, STE_SIM_RSP_SHUTDOWN, 0, 0, msg->client_tag);
    } else {
      modem_reset_status = 1;
      catd_log_s(SIM_LOGGING_I, "simd : MODEM SILENT RESET message received", msg->client_tag);
      uiccd_sig_modem_silent_reset(msg->fd, msg->client_tag);
    }
    ste_msg_delete(ste_msg);
}


// -----------------------------------------------------------------------------
// Debug Levels

static void simd_sig_set_sim_debug_level(int fd, uintptr_t client_tag, int level)
{
    simd_msg_debug_level_t        *msg;
    msg = simd_msg_set_sim_debug_level_create(fd, client_tag, level);
    if (msg) {
        ste_msgq_add(simd_mq, (ste_msg_t *) msg);
    }
}
static void simd_main_set_sim_debug_level(ste_msg_t * ste_msg)
{
    simd_msg_debug_level_t        *msg = (simd_msg_debug_level_t *) ste_msg;
    {
        sim_logging_t   keep = opt_v;
        opt_v = SIM_LOGGING_I;
        // This should always be visible, regardless of current or new level
        catd_log_f(SIM_LOGGING_I, "simd : SIM DEBUG LEVEL set to %d", msg->level);
        opt_v = keep;
    }
    opt_v = (sim_logging_t)msg->level;
    sim_send_generic(msg->fd, STE_SIM_RSP_SET_SIM_DEBUG_LEVEL, 0, 0, msg->client_tag);
    ste_msg_delete(ste_msg);
}


static void simd_sig_set_modem_debug_level(int fd, uintptr_t client_tag, int level)
{
    simd_msg_debug_level_t        *msg;
    msg = simd_msg_set_modem_debug_level_create(fd, client_tag, level);
    if (msg) {
        ste_msgq_add(simd_mq, (ste_msg_t *) msg);
    }
}
static void simd_main_set_modem_debug_level(ste_msg_t * ste_msg)
{
    simd_msg_debug_level_t        *msg = (simd_msg_debug_level_t *) ste_msg;
    {
        sim_logging_t   keep = opt_v;
        opt_v = SIM_LOGGING_I;
        // This should always be visible, regardless of current or new level
        catd_log_f(SIM_LOGGING_I, "simd : MODEM DEBUG LEVEL set to %d", msg->level);
        opt_v = keep;
    }
    ste_modem_set_debug_level(catd_get_modem(),0,msg->level);
    sim_send_generic(msg->fd, STE_SIM_RSP_SET_MODEM_DEBUG_LEVEL, 0, 0, msg->client_tag);
    ste_msg_delete(ste_msg);
}


// -----------------------------------------------------------------------------
//

void simd_main_loop()
{
    ste_msg_t              *msg;

    catd_log_s(SIM_LOGGING_I, "simd : Entering main loop", 0);
    for (;;) {
        msg = ste_msgq_pop(simd_mq);
        if (!msg) {
            catd_log_s(SIM_LOGGING_E, "catd : NULL message received", 0);
            continue;
        }

        switch (msg->type) {
#ifndef CFG_USE_ANDROID_LOG
        case SIMD_MSG_LOG:
            catd_main_log(msg);
            break;
#endif

        case SIMD_MSG_SHUTDOWN:
            simd_main_shutdown(msg);
            goto Done;

        case SIMD_MSG_PING:
            simd_main_ping(msg);
            break;

        case SIMD_MSG_DISCONNECT:
            simd_main_disconnect(msg);
            break;

        case SIMD_MSG_CONNECT:
            simd_main_connect(msg);
            break;

        case SIMD_MSG_STARTUP:
            catd_log_f(SIM_LOGGING_E, "simd : SIMD_MSG_STARTUP shall NOT print this");
            //simd_main_startup(msg);
            break;

        case SIMD_MSG_STARTUP_COMPLETED:
            simd_main_startup_completed(msg);
            break;

        case SIMD_MSG_SET_SIM_DEBUG_LEVEL:
            simd_main_set_sim_debug_level(msg);
            break;

        case SIMD_MSG_SET_MODEM_DEBUG_LEVEL:
            simd_main_set_modem_debug_level(msg);
            break;

        default:
            catd_log_f(SIM_LOGGING_E, "simd : unknown message received=%x", msg->type);
            ste_msg_delete(msg);
            break;
        }
    }
  Done:
    catd_log_s(SIM_LOGGING_I, "simd : Exiting main loop", 0);

}



// -----------------------------------------------------------------------------
// simd main.

static void init_simd()
{
    int rv;

    simd_mq = ste_msgq_new();   /// Main input message queue.
    startup_state = STARTUP_NONE;

    rv = ste_sim_client_data_init();
    if (rv) {
        catd_log_f(SIM_LOGGING_E, "catd : ste_sim_client_data_init failed (%d)", rv);
    }

}


static void fini_simd()
{
    ste_sim_client_data_fini();
    ste_msgq_delete(simd_mq);
    simd_mq = 0;
}

#ifdef CFG_USE_ANDROID_LOG
#define mprint  ALOGI
#endif

void simd()
{
    int                     i;
    ste_modem_t            *modem;

    if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
        mprint("catd : SIG_ERR when ignoring SIGPIPE");
        abort();
    }

    mprint("simd : Entering catd");
    init_simd();

    modem = ste_catd_modem_new();

    if (!modem) {
        catd_log_s(SIM_LOGGING_E, "simd : ste_catd_modem_new failed", 0);
        catd_flush_log_messages();
        abort();
    }
    catd_set_modem(modem);
    ste_modem_set_debug_level(catd_get_modem(),0,modem_v);
    // Connect to modem and modem init moved to reader thread
    // and invoked from a client command

    catd_rdr = ste_catd_reader_new(simd_mq);
    if (!catd_rdr) {
        mprint("simd : ste_catd_reader_new failed");
        abort();
    }

    i = ste_catd_reader_start(catd_rdr);
    if (i) {
        catd_log_s(SIM_LOGGING_E, "simd : ste_catd_reader_start failed", 0);
        catd_flush_log_messages();
        abort();
    }

    /*
     * Timers must be initiated after catd_reader
     */
    if (simd_timer_initiate_timers(SIMD_NUM_TIMERS) < 0) {
        catd_log_s(SIM_LOGGING_E, "simd : simd_timer_initiate_timers failed", 0);
        catd_flush_log_messages();
        abort();
    }

    i = catd_start(0);
    if (i) {
        catd_log_s(SIM_LOGGING_E, "simd : catd_start failed", 0);
        abort();
    }

    i = uiccd_start(0);
    if (i) {
        catd_log_s(SIM_LOGGING_E, "simd : uiccd_start failed", 0);
        abort();
    }
    sim_log_init(catd_rdr); /*initializing logging mechanism*/
    simd_read_config_options();
    catd_log_f(SIM_LOGGING_I, "simd: ENABLE_VIVO_ROAMING_BROKER = %d", ENABLE_VIVO_ROAMING_BROKER);
    simd_main_loop();
    sim_log_close(catd_rdr); /*logging mechanism terminated*/
    catd_flush_log_messages();

    i = uiccd_stop(0);
    if ( i != 0 ) {
        catd_log_s(SIM_LOGGING_E, "simd : uiccd_stop failed", 0);
        abort();
    }

    i = catd_stop(0);
    if ( i != 0 ) {
        catd_log_s(SIM_LOGGING_E, "simd : catd_stop failed", 0);
        catd_flush_log_messages();
        abort();
    }

    /*
     * Timers must be destroyed before catd_reader
     */
    if (simd_timer_shutdown_timers() < 0) {
        catd_log_s(SIM_LOGGING_E, "simd : simd_timer_shutdown_timers failed", 0);
        catd_flush_log_messages();
    }

    catd_flush_log_messages();
    if (modem_reset_status == 0) {
    if (modem) {
      i = ste_catd_modem_disconnect(modem, 0);
      if (i) {
        catd_log_s(SIM_LOGGING_E, "simd : ste_catd_modem_disconnect failed", 0);
        catd_flush_log_messages();
        abort();
      }
      ste_catd_modem_delete(modem, 0);
      catd_set_modem(0);
    }
    i = ste_catd_reader_stop(catd_rdr);
    if (i) {
        catd_log_s(SIM_LOGGING_E, "simd : ste_catd_reader_stop failed", 0);
        catd_flush_log_messages();
        abort();
    }
    catd_flush_log_messages();

    ste_catd_reader_delete(catd_rdr);
    catd_rdr = 0;
    catd_flush_log_messages();
    }
    fini_simd();
}

// -----------------------------------------------------------------------------

static void eval_level ( int level, sim_logging_t *result ) {
  switch ( level ) {
    case '0':
      *result = SIM_LOGGING_E;
      break;
    case '1':
      *result = SIM_LOGGING_I;
      break;
    case '2':
      *result = SIM_LOGGING_D;
      break;
    case '3':
      *result = SIM_LOGGING_V;
      break;
    default:
      // Ignore any other character, or EOF
      break;
  }
}

static void check_for_log_level_files ( void ) {
  FILE *fp;

  fp = fopen(SIM_INIT_DEBUG_LEVEL,"r");
  if ( fp != NULL ) {
    eval_level(fgetc(fp), &opt_v);
    fclose(fp);
  }
  fp = fopen(MODEM_INIT_DEBUG_LEVEL,"r");
  if ( fp != NULL ) {
    eval_level(fgetc(fp), &modem_v);
    fclose(fp);
  }
}

// -----------------------------------------------------------------------------

static int match(const char *a, const char *b)
{
    size_t                  n = strlen(b);
    size_t                  m = strlen(a);

    return (n == m ? !strncmp(a, b, n) : 0);
}


void parse_options(int argc, const char **argv)
{

    --argc;
    ++argv;

    while (argc) {
        const char             *opt = *argv;
        --argc;
        ++argv;

        if (!opt)
            continue;

        if (match(opt, "-fg")) {
            opt_daemonise = 0;
        }

        else if (match(opt, "-v")) {
            opt_v = SIM_LOGGING_E;
            modem_v = opt_v;
        }

        else if (match(opt, "-vv")) {
            opt_v = SIM_LOGGING_I;
            modem_v = opt_v;
        }

        else if (match(opt, "-vvv")) {
            opt_v = SIM_LOGGING_D;
            modem_v = opt_v;
        }

        else if (match(opt, "-vvvv")) {
            opt_v = SIM_LOGGING_V;
            modem_v = opt_v;
        }

        else if (match(opt, "--need-pin")) {
            pin_required_flag = 1;
            printf("simd : pin_required_flag set to %d\n", pin_required_flag);
        }

        else {
            fprintf(stderr, "Bad argument: %s\n", opt);
            exit(EXIT_FAILURE);
        }

    };

}


void daemonise()
{
    pid_t                   pid;
    int                     i;
    int                     null_fd;

    for(i = 0; i< sysconf(_SC_OPEN_MAX); i++) {
        close(i); // Close all the open file descriptors that was inherited from the parent
    }


    i = chdir("/");             /* set CWD to / to unblock other fs */
    if (i < 0) {
        perror("daemonise chdir failed");
        abort();
    }

    /* redirect all I/O to /dev/null */
    if ( (null_fd = open("/dev/null", O_RDONLY)) >= 0)
        stdin_fd = dup2(null_fd, 0);
    if ( (null_fd = open("/dev/null", O_WRONLY)) >= 0)
        stdout_fd = dup2(null_fd, 1);
    if ( (null_fd = open("/dev/null", O_WRONLY)) >= 0)
        stderr_fd = dup2(null_fd, 2);

    util_set_restricted_umask();
}




int main(int argc, const char **argv)
{
#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
    umask(0);
#endif
    pin_required_flag = 0;
    setProcName(argv[0]);
    setbuf(stdout, 0);
    parse_options(argc, argv);
    check_for_log_level_files();

    if (opt_daemonise) {
        printf("Daemonizing\n");
        daemonise();
    }
    simd();

    if (stdin_fd >= 0)
        close(stdin_fd);
    if (stdout_fd >= 0)
        close(stdout_fd);
    if (stderr_fd >= 0)
        close(stderr_fd);

    exit(EXIT_SUCCESS);
}
