/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef MODEMLOGRELAY_H
#define MODEMLOGRELAY_H

// This define will be used to add a prefix in response
// string for command to get the modem software version
#define MODEM_VERSION_PREFIX_STR    "ModemVersion:"

/*
 *
 *
 * Structure to manage socket server
 *
 *
 */

typedef void (*func_t)(int);


enum {
    SK_SERV_CTRL ,
    SK_SERV_DATA ,
    SK_SERV_INV,
    SK_SERV_DBG,
    SK_SERV_RPT,
    SK_SERV_MAX
};

struct socket_server_info {
    int port;
    func_t func;
    char name[128];
    int fd;
    int pipe[2];
};
void command_handler(int command_hdl_fd);
void datalink_handler(int fd);
void sck_server_inv_trace(int fd);
void debug_handler(int dbg_hdl_fd);
void report_handler(int fd);
void ril_connection_handler(int port);
int request_reset_modem_with_dump(void);
int trace_trigger(int source);



/*
 *
 *
 * structure to manage argument of modemlogrelay
 *
 *
 */

enum {
    ARG_NOTYPE,
    ARG_INT
};

typedef struct {
    char  cmd[40]; /* cmd string to compare */
    char  fmt[4];  /* %d %s ... */
    char  val[16]; /* defautl value */
    int   manda;   /* 1 for mandatory */
    void  *adr;    /* adress of variable to set */
    int   found;   /* 1 if argument is found */
    int   type;    /* ARG_INT ... */
} arg_t;


/*
 *
 *
 * parsing
 *
 *
 *
 */


#define MAL_MON_RESPONSE_OK 0x00
#define MAX_CONFIG_RESP_LEN 1024



/*
 * Evacuation Structure
 *
 *
*/
#define IMEI_SIZE 20
#define MAX_PATH_LEN 128
#define MAX_TRACE_NAME_LEN 128
#define MAX_RESP_MSG_LEN 1024
#define MAX_VALUE_LENGTH 128
#define MOUNT_LINE_LENGTH 256
#define MAX_SYNC_RETRIES 15

#define AUTOCONF_FILENAME "/system/etc/trace_auto.conf"
#define MODEM_PATH "/dev/db8500-modem-trace"
#define BASEBAND_UID_PATH "/sys/socinfo/soc_id"

#define UEVENT_USB_STR "usb_connected"
#define USB_CONNECTION_PATH "/sys/class/switch/usb_connected/state"

#define MAX_SOCKET_RECONNECT_TRIES 5

// Discard trace from internal buffer if threshold is reached 1 = Enabled, 0 = Disabled
#define TRACE_DISCARD 1

// Threshold in Bytes (100 MB default)
#define TRACE_DISCARD_THRESHOLD 200*1024*1024


char dummy_file_content[] = "\n" \
                            "The modem log is not completed successfully. "
                            "Parts of the modem trace and log are not flushed to the trace files.\n"
                            "\n"
                            "Reason:\n"
                            "==========\n"
                            "Before collecting modem logs, trigger report or stop trace was not performed.\n"
                            "\n"
                            "How to get complete log:\n"
                            "===========================\n"
                            "1) Some phones have inbuilt android app called \"Modem Trace Control\".\n"
                            "   Use this android application on phone to stop trace or create a trigger report manually.\n"
                            "\n"
                            "2) Send a trigger report or stop trace command to Modem Log Relay. "
                            "Refer to document \"Collecting U8500 modem traces and modem dumps\".\n"
                            "\n";


enum {
    MSUP_GEN_MODEM_COREDUMP,
    MSUP_IND_MODEM_REBOOT,
    MSUP_UNKNOWN_CMD
};

struct event_s {
    int flag;
    pthread_mutex_t lock;
    pthread_cond_t cond;
};

typedef struct {
    int enabled;
    unsigned long long max_size[MLR_MAX_TYPE_BUF_TYPE];
} rol_file_feature_t;

typedef struct {
    int enabled;
    unsigned long long cur_size;
    unsigned long long max_size;
    unsigned long long threshold;
} rol_file_t;

struct evac_buffer {
    int  size;
    char *buffer;
    struct evac_buffer *next;
};

typedef struct {
    sem_t           data_ready;
    pthread_mutex_t mutex;
    int fd;

    rol_file_t rol_file;

    struct evac_buffer *first;
    struct evac_buffer *last;

    int count;
    int countwr;
    int countrd;
    int missed;
} evac_fifo_t ;

struct evac_info_s {
    struct event_s dlink_evt;
    struct event_s trace_evt;
    struct event_s flush_evt;
    struct event_s sdcard_evt;
    struct event_s modem_evt;
    struct event_s trigger_evt;
    struct event_s reportlink_evt;
    struct event_s storage_evt;

    int nb_fifo; /* 1 for usb use case, else 5 for sdcard use case */
    int modem_fd;

    rol_file_feature_t rol_ff;

    int type;
    char imei[IMEI_SIZE];
    char dest_path[MAX_PATH_LEN];
    char filename[MAX_PATH_LEN];

    int dump_type;
    char dump_path[MAX_PATH_LEN];

    evac_fifo_t fifo[MLR_MAX_EVAC_FILE];

    int trace_route_type;

    // This is used for detecting trace file auto
    // trigger is enabled for roll-off case
    int is_autotrigger;

};

enum {
    OFF,
    STP,
    BUFFER,
    BOTH,
};


enum {
    MAL_SK_CMD_REQ,
    MAL_SK_CMD_RSP,
    MAL_SK_IND,
    MAL_SK_IND_INV,
    MAL_SK_MAX
};

#define DGRAM_SK_LENGTH_MSG_MAX 0x100

struct mal_dgram_sk_s {
    char name[30];
    int type;
    struct dgram_conn_t *conn;
};

struct mal_dgram_sk_s mal_dgram_sk[] = {
    {.name = "/tmp/mal_sk_req", .type = DGRAM_SK_CLIENT},
    {.name = "/tmp/mal_sk_rsp", .type = DGRAM_SK_SERVER},
    {.name = "/tmp/mal_sk_ind", .type = DGRAM_SK_SERVER},
    {.name = "/tmp/mal_sk_inv", .type = DGRAM_SK_SERVER},
};

struct dgram_conn_t *my_dgram_sk[MAL_SK_MAX];

enum {
    DEST_UNKNOWN,
    DEST_USB,
    DEST_FS,
    DEST_SDCARD,
};

enum {
    EVAC_STOP,
    EVAC_START,
};

enum {
    DLINK_DISCONNECTED,
    DLINK_CONNECTED,
};

enum {
    SD_UNMOUNTED,
    SD_MOUNTED,
};

enum {
    EVAC_FLUSHED,
    EVAC_FLUSHING,
};

enum {
    MODEM_NOT_READY,
    MODEM_READY,
};

enum {
    BOOT_SYNCHRO,
    REBOOT_SYNCHRO,
};

enum {
    EVAC_UNLOCKED,
    EVAC_LOCKED,
};

enum {
    STORAGE_AVAIL,
    STORAGE_FULL,
};

enum {
    HOST_TRIGGER,
    MODEM_TRIGGER,
    STOP_TRIGGER,
    RIL_TRIGGER
};

static const char trigger_type[][8] = {
    {"host"},
    {"modem"},
    {"stop"},
    {"ril"}
};

/* Variable for switch to hats mode */
int hatsmode;

/* file descriptor to evacuate inv traces */
int sck_server_Inv_trace_fd = -1;

/* Number of days to keep old traces (0 = never delete files) */
long trace_history = 0;

struct socket_server_info sk_serv[] = {
    {.name = "command_handler",      .func = command_handler,      .fd = -1, .pipe = { -1, -1}},
    {.name = "datalink_handler",     .func = datalink_handler,     .fd = -1, .pipe = { -1, -1}},
    {.name = "sck_server_inv_trace", .func = sck_server_inv_trace, .fd = -1, .pipe = { -1, -1}},
    {.name = "debug_handler",        .func = debug_handler,        .fd = -1, .pipe = { -1, -1}},
    {.name = "report_handler",       .func = report_handler,       .fd = -1, .pipe = { -1, -1}},
};

typedef struct {
    int32_t  header;
    uint32_t time_stamp;
    uint8_t  trace_entity;    /**< trace entity value */
    uint8_t  trace_grp;       /**< trace group value */
    uint8_t  trace_id;        /**< trace id value */
    uint8_t  filler;
    uint16_t trace_data_len; /**< length of trace data */
} trace_inv_t;


enum {
    _TRACE,
    _INV_TRACE,
    _DUMP,
    _TRIGGER
};

#endif // #ifndef MODEMLOGRELAY_H
