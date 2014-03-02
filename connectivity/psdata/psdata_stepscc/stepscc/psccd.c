/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control Daemon
 */

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>
#include "psccd.h"
#include "libstecom.h"
#include "pscc_msg.h"
#include "pscc_handler.h"
#include "pscc_bearer.h"
#include "pscc_sim.h"
#include "mpl_config.h"
#include "pscc_runscript.h"
#include "pscc_netdev.h"
#include "util_security.h"

/* Use Logcat for android, otherwise default log (syslog) */
#ifdef PSCC_SW_VARIANT_ANDROID
#define LOG_TAG "RIL PSCCD"
#include <cutils/log.h>
#include <time.h>
#include <sys/time.h>
#else
#include <syslog.h>
#endif //PSCC_SW_VARIANT_ANDROID
#define PRINT_BUF_SIZE  (512)


/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
enum transport_type_t {
  tt_unix,
  tt_ip
};

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/
static char req[PSCCD_BUFSIZ];
static int req_len;
static char resp[PSCCD_BUFSIZ];
static int resp_len;
static int err = 0;
static int daemon_mode = 1;
static int debug_mode = 0;
static struct stec_ctrl *ctrl_handle_p;
static struct stec_ctrl *event_handle_p;
//for config file and all parameters read from that
static char* config_path = "pscc.conf";
static bool pscc_initialisation_done = false;
static bool pscc_sim_init_done = false;
static int pscc_log_level =  (UTIL_LOG_TYPE_INFO | UTIL_LOG_TYPE_ERROR);

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static void print_usage(char *prog);
static void handle_terminate(int signo);
static void handle_hup(int signo);
static void close_handles(void);
static int initialize_bearer(void);
static int initialize_netdev(void);
static int initialize_sim(void);
static void deinitialize_sim(void);

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

mpl_config_t psccd_config;
unsigned short gprs_res_ctrl;

/**
 * main()
 **/
int main(int argc, char *argv[])
{
  int result;
  int nfds;
  fd_set rfds;
  // initialize to 0 to avoid warnings (will be overwritten when initialized)
  int modem_fd = 0;
  int netlnk_fd = -1;
  int netdev_fd = 0;
  int pscclog_fd = -1;
  int sim_fd = -1;
  struct sockaddr_un ctrl_addr_un;
  struct sockaddr_un event_addr_un;
  struct sockaddr_in ctrl_addr_in;
  struct sockaddr_in event_addr_in;
  enum transport_type_t transport_type = tt_unix;
  mpl_param_element_t* param_elem_p;

  /* optarg stuff */
  int c;
  extern char *optarg;
  extern int optind, optopt, opterr;
  /* optarg end */

  pscclog_fd = pscc_log_init();
    /* Set umask before anything else */
  util_set_restricted_umask();

  while ((c = getopt(argc, argv, "fd:t:r:c:")) != -1)
  {
    switch (c)
    {
    case 'f':
      daemon_mode = 0;
      break;
    case 'd':
      debug_mode = strtol(optarg, NULL, 10);
      break;
    case 'c':
      config_path = optarg;
      break;
    case 'r':
      pscc_runscript_path = optarg;
      break;
    case 't':
      if (!strncmp(optarg, "ip", 2))
      {
        transport_type = tt_ip;
      }
      else if (!strncmp(optarg, "unix", 4))
      {
        transport_type = tt_unix;
      }
      else
      {
        print_usage(argv[0]);
        exit(1);
      }
      break;
    case ':':
    case '?':
      print_usage(argv[0]);
      exit(1);
    }
  }

  if (daemon_mode)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "Deamon mode not supported. Will run foreground mode.");
  }

#ifndef PSCC_SW_VARIANT_ANDROID
  /* Open any logs here */
  openlog(argv[0], LOG_CONS | LOG_PID, LOG_DAEMON);
#endif //PSCC_SW_VARIANT_ANDROID

  /* Handle some signals */
  signal(SIGTERM, handle_terminate);
  signal(SIGHUP, handle_hup);
  signal(SIGINT, handle_terminate);

  /*Handle SIGPIPE to Avoid Crash*/
   if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
   PSCC_DBG_TRACE( LOG_ERR, "\n%d : SIG_ERR when ignoring SIGPIPE", __LINE__ );
   abort();
   }

  pscc_init(NULL, &psccd_log_func);
  mpl_config_init(NULL, &psccd_log_func, &psccd_config, PSCC_PARAM_SET_ID);

  if(0!=mpl_config_read_config(config_path, &psccd_config, PSCC_PARAM_SET_ID))
    PSCC_DBG_TRACE(LOG_WARNING, "Can't open config file %s\n",config_path);

  /* if debug mode is not enabled at startup, check if it is enabled in config */
  if (0 == debug_mode)
  {
    param_elem_p = mpl_config_get_para(pscc_paramid_debug_mode, &psccd_config);
    if ((NULL != param_elem_p) && (NULL != param_elem_p->value_p) && *((bool*) param_elem_p->value_p))
    {
      debug_mode = 1;
    }
  }

  if (0 != debug_mode)
  {
    PSCC_DBG_TRACE(LOG_INFO, "Debug mode is enabled\n");
  }

  memset(&param_elem_p, 0, sizeof(param_elem_p));
  if (0 == gprs_res_ctrl)
  {
    param_elem_p = mpl_config_get_para(pscc_paramid_gprs_res_ctrl, &psccd_config);
    if ((NULL != param_elem_p) && (NULL != param_elem_p->value_p) && *((bool*) param_elem_p->value_p))
    {
      gprs_res_ctrl = 1;
    }
  }
  if (gprs_res_ctrl)
  {
    PSCC_DBG_TRACE(LOG_INFO, "GPRS Resource Control is Enabled\n");
  }
  else
  {
    PSCC_DBG_TRACE(LOG_INFO, "GPRS Resource Control is Disabled\n");
  }

  if (transport_type == tt_unix)
  {
    memset(&ctrl_addr_un, 0, sizeof(ctrl_addr_un));
    ctrl_addr_un.sun_family = AF_UNIX;
    strncpy(ctrl_addr_un.sun_path, PSCC_CTRL_PATH, UNIX_PATH_MAX) ;
    unlink(ctrl_addr_un.sun_path);
    ctrl_handle_p = stec_opensrv((struct sockaddr*) &ctrl_addr_un, sizeof(ctrl_addr_un));
    if (ctrl_handle_p == NULL)
    {
      PSCC_DBG_TRACE( LOG_ERR, "\n%d: stec_opensrv failed\n", __LINE__ );
      exit(-1);
    }

    memset(&event_addr_un, 0, sizeof(event_addr_un));
    event_addr_un.sun_family = AF_UNIX;
    strncpy(event_addr_un.sun_path, PSCC_EVENT_PATH, UNIX_PATH_MAX) ;
    unlink(event_addr_un.sun_path);
    event_handle_p = stec_opensrv((struct sockaddr*) &event_addr_un, sizeof(event_addr_un));
    if (event_handle_p == NULL)
    {
      PSCC_DBG_TRACE( LOG_ERR, "\n%d: stec_opensrv failed\n", __LINE__ );
      exit(-1);
    }
  }
  else if (transport_type == tt_ip)
  {
    memset(&ctrl_addr_in, 0, sizeof(ctrl_addr_in));
    ctrl_addr_in.sin_family = AF_INET;
    ctrl_addr_in.sin_port = htons(PSCC_CTRL_PORT);
    ctrl_addr_in.sin_addr.s_addr = INADDR_ANY;
    ctrl_handle_p = stec_opensrv((struct sockaddr*) &ctrl_addr_in, sizeof(ctrl_addr_in));
    if (ctrl_handle_p == NULL)
    {
      PSCC_DBG_TRACE( LOG_ERR, "\n%d: stec_opensrv failed\n", __LINE__ );
      exit(-1);
    }

    memset(&event_addr_in, 0, sizeof(event_addr_in));
    event_addr_in.sin_family = AF_INET;
    event_addr_in.sin_port = htons(PSCC_EVENT_PORT);
    event_addr_in.sin_addr.s_addr = INADDR_ANY;
    event_handle_p = stec_opensrv((struct sockaddr*) &event_addr_in, sizeof(event_addr_in));
    if (event_handle_p == NULL)
    {
      PSCC_DBG_TRACE( LOG_ERR, "\n%d: stec_opensrv failed\n", __LINE__ );
      exit(-1);
    }
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "Entering loop\n");

  do
  {
    FD_ZERO(&rfds);
    nfds = 0;
    FD_SET(stec_get_fd(ctrl_handle_p), &rfds);
    nfds = stec_get_fd(ctrl_handle_p) > nfds ? stec_get_fd(ctrl_handle_p) : nfds;
    FD_SET( stec_get_fd(event_handle_p), &rfds );
    nfds = stec_get_fd(event_handle_p) > nfds ? stec_get_fd(event_handle_p) : nfds;
    FD_SET(pscclog_fd, &rfds);
    nfds = pscclog_fd > nfds ? pscclog_fd : nfds;
    if (pscc_initialisation_done)
    {
      FD_SET( modem_fd, &rfds );
      nfds = modem_fd > nfds ? modem_fd : nfds;
      FD_SET(netdev_fd, &rfds);
      nfds = netdev_fd > nfds ? netdev_fd : nfds;

      if (pscc_sim_init_done)
      {
        FD_SET(sim_fd, &rfds);
        nfds = sim_fd > nfds ? sim_fd : nfds;
      }

      if(netlnk_fd != -1)
      {
        FD_SET(netlnk_fd, &rfds);
        nfds = netlnk_fd > nfds ? netlnk_fd : nfds;
      }
    }

#ifdef PSCC_MODULE_TEST_ENABLED
    FD_SET( 0, &rfds );
#endif //PSCC_MODULE_TEST_ENABLED

    nfds += 1;

    //PSCC_DBG_TRACE(LOG_DEBUG, "Calling select()\n");
    result = select(nfds, &rfds, NULL, NULL, 0);

    if (result < 0)
    {
      if (errno == EINTR)
      {
        PSCC_DBG_TRACE(LOG_INFO, "select returned because of a signal\n");
        continue;
      }
      else
      {
        PSCC_DBG_TRACE(LOG_ERR, "select returned error: %s\n", strerror(errno));
        break;
      }
    }

    if (FD_ISSET(stec_get_fd(ctrl_handle_p), &rfds))
    {
      req_len = stec_recv(ctrl_handle_p, req, PSCCD_BUFSIZ,-1);
      if (req_len < 0)
      {
        PSCC_DBG_TRACE(LOG_ERR, "stec_recv: %s\n", strerror(errno));
        return -1;
      }

      if (!pscc_initialisation_done)
      {
        /* PSCC initialisation */
        modem_fd = initialize_bearer();
        netdev_fd = initialize_netdev();
        netlnk_fd = pscc_bearer_netlnk_init();
        pscc_initialisation_done = true;
        PSCC_DBG_TRACE(LOG_INFO, "bearer and netdev initialised\n");
        util_continue_as_privileged_user(UTIL_SECURITY_CAP_NET_ADMIN |
                                         UTIL_SECURITY_CAP_SYS_ADMIN);
      }

      /* in case sim is not initialized */
      if (!pscc_sim_init_done)
      {
        PSCC_DBG_TRACE(LOG_INFO, "Trying to initialize SIM\n");
        sim_fd = initialize_sim();
      }

      resp_len = PSCCD_BUFSIZ;
      result = pscc_handler(req, (size_t)req_len, resp, (size_t*)&resp_len);
      if (result < 0)
      {
        PSCC_DBG_TRACE(LOG_ERR, "psconnctrl_handle_client_request failed\n");
        /* Ignore and continue loop */
      }
      else if (resp_len > 0)
      {
        if (stec_send(ctrl_handle_p, resp, resp_len) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR, "stec_send: %s\n", strerror(errno));
          return -1;
        }
      }
    }

    if (FD_ISSET(stec_get_fd(event_handle_p), &rfds))
    {
      if(stec_process_recv(event_handle_p) < 0)
      {
        PSCC_DBG_TRACE(LOG_ERR, "stec_process_recv failed\n");
      }
    }

    if (pscc_initialisation_done)
    {
      // In case the netlink socket creates async events:
      if (FD_ISSET(netdev_fd, &rfds))
      {
        if (pscc_netdev_handle_message(netdev_fd) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR,"pscc_netdev_handle_message failed\n");
        }
      }

      if (FD_ISSET(modem_fd, &rfds))
      {
        if (pscc_bearer_handler(modem_fd) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR,"pscc_bearer_handle_message failed\n");
        }
      }
      if (FD_ISSET(netlnk_fd, &rfds))
      {
        if (pscc_bearer_netlnk_handle_message(netlnk_fd) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_netlnk_handle_message failed\n");
        }
        else
        {
          PSCC_DBG_TRACE(LOG_WARNING, "received modem reset indication, \
                                  reinitializing the pscc server!!\n");
          err = 1;
          break;
         }
       }

      if (FD_ISSET(sim_fd, &rfds))
      {
        if (pscc_sim_handler(sim_fd) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR,"pscc_sim_handler failed\n");
          deinitialize_sim();
          sim_fd = -1;
        }
      }
    }

      if (FD_ISSET(pscclog_fd, &rfds))
      {
        if (pscc_log_select_callback(pscclog_fd, NULL) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR,"pscc_log_select_callback failed\n");
        }
      }

#ifdef PSCC_MODULE_TEST_ENABLED
    if (FD_ISSET(0, &rfds))
    {
      read(0,req, PSCCD_BUFSIZ);
      if(0 == strncmp(req,"quit",strlen("quit")))
        break;

      if(0 == strncmp(req,"sync",strlen("sync")))
        printf("psccd: sync\n");
    }
#endif //PSCC_MODULE_TEST_ENABLED
  } while(!err);
  close_handles();
  return(err);
}

/**
 * psccd_log_func()
 **/
void psccd_log_func(void* user_p, int level, const char* file, int line, const char *format, ...)
{
  PSCC_IDENTIFIER_NOT_USED(user_p);

#ifdef PSCC_SW_VARIANT_ANDROID
  PSCC_IDENTIFIER_NOT_USED(file);
  PSCC_IDENTIFIER_NOT_USED(line);
  char *buf = malloc(PRINT_BUF_SIZE);
  int log_level = (pscc_log_level & level);
  va_list arg;
  va_start(arg, format);

  vsnprintf(buf, PRINT_BUF_SIZE, format, arg);
  buf[PRINT_BUF_SIZE - 1] = '\0';
  switch ( log_level ) {
  case UTIL_LOG_TYPE_ERROR:
    PSCC_LOG_E("%s",buf);
    break;
  case UTIL_LOG_TYPE_WARNING:
    PSCC_LOG_W("%s",buf);
    break;
  case UTIL_LOG_TYPE_INFO:
    PSCC_LOG_I("%s",buf);
    break;
  case UTIL_LOG_TYPE_DEBUG:
    PSCC_LOG_D("%s",buf);
    break;
  case UTIL_LOG_TYPE_VERBOSE:
    PSCC_LOG_V("%s",buf);
    break;
  default:
    break;
  }
  free(buf);
  va_end(arg);
  #else
  va_list ap;
  if (debug_mode || level != LOG_DEBUG)
  {
    char *buf = malloc(PRINT_BUF_SIZE);
    va_start(ap, format);
    vsnprintf(buf, PRINT_BUF_SIZE, format, ap);
    buf[PRINT_BUF_SIZE - 1] = '\0';
    switch (level)
    {

    case LOG_EMERG:
    case LOG_ALERT:
    case LOG_CRIT:
    case LOG_ERR:
      fprintf( stderr, "ERROR [%s:%d]: ", file, line);
      break;

    case LOG_WARNING:
      fprintf( stderr, "WARN  [%s:%d]: ", file, line);
      break;

    case LOG_NOTICE:
    case LOG_INFO:
      fprintf( stderr, "INFO  [%s:%d]: ", file, line);
      break;

    case LOG_DEBUG:
    default:
      fprintf( stderr, "DEBUG [%s:%d]: ", file, line);
      break;
    }

    fprintf( stderr, "%s",buf );
    /* flush printf */
    fflush(stderr);
    free(buf);
    va_end(ap);
  }
#endif //PSCC_SW_VARIANT_ANDROID
}

/**
 * psccd_log_func_no_info()
 **/
void psccd_log_func_no_info(void* user_p, const char *format, ...)
{
  PSCC_IDENTIFIER_NOT_USED(user_p);
/* Use Logcat for android, otherwise default log (syslog) */
#ifdef PSCC_SW_VARIANT_ANDROID
  if (debug_mode >= LOG_INFO)
  {
    ALOGI("%s", format);
  }
#else
  va_list ap;
  if (debug_mode)
  {
    va_start(ap, format);
    vsyslog(LOG_INFO, format, ap);
    va_end(ap);
  }
#endif //PSCC_SW_VARIANT_ANDROID
}

/**
 * psccd_send_event()
 **/
int psccd_send_event(char *event_p, int event_len)
{
  // send to all
  return stec_sendevent(event_handle_p, event_p, event_len);
}

/**
 * psccd_sendto_event()
 **/
int psccd_sendto_event(char *event_p, int event_len, struct sockaddr *to, socklen_t to_len)
{
  // send to specific
  return stec_sendeventto(event_handle_p, event_p, event_len, to, to_len);
}


/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
/**
 * close_handles()
 **/
static void close_handles(void)
{
  PSCC_DBG_TRACE(LOG_DEBUG, "Closing handles\n");
  pscc_log_close();
  pscc_bearer_deinit();
  deinitialize_sim();
  stec_close(ctrl_handle_p);
  stec_close(event_handle_p);
  mpl_config_reset(&psccd_config);
  (void)pscc_netdev_shutdown();
  PSCC_DBG_TRACE(LOG_DEBUG, "Closed\n");
}

/**
 * handle_terminate()
 **/
static void handle_terminate(int signo)
{
  PSCC_DBG_TRACE(LOG_INFO, "Terminated by signal %d.\n", signo);
  close_handles();
  exit(0);
}

/**
 * handle_hup()
 **/
static void handle_hup(int signo)
{
  PSCC_DBG_TRACE(LOG_INFO, "Load default parameters by signal %d.\n", signo);
  mpl_config_reset(&psccd_config);
  if(0!=mpl_config_read_config(config_path, &psccd_config, PSCC_PARAM_SET_ID))
    PSCC_DBG_TRACE(LOG_ERR, "Can't open config file %s\n",config_path);
}

/**
 * print_usage()
 **/
static void print_usage(char *prog)
{
#ifdef PSCC_SW_VARIANT_ANDROID
  PSCC_DBG_TRACE(LOG_INFO,
#else
  fprintf(stderr,
#endif
    "Usage: %s [-f] [-d level] [-t unix/ip] [-c <config file>] [-r <run-script>]\n"
    "       -f : foreground mode (no daemon)\n"
    "       -d : debug\n"
    "       -t : transport type\n"
    "       -c : specify the config file\n"
    "       -r : specify the run-script file\n", prog);
}

/**
 * initialize_bearer()
 **/
static int initialize_bearer(void)
{
  /* Bearer initialisation */
  int fd = pscc_bearer_init();
  if (fd < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Could not initialise modem\n");
    exit(-1);
  }

  return fd;
}

/**
 * initialize_netdev()
 **/
static int initialize_netdev(void)
{
  int fd = pscc_netdev_init();
  if (fd < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Could not initialise net device\n");
    exit(-1);
  }

  return fd;
}

/**
 * initialize_sim()
 **/
static int initialize_sim(void)
{
  int fd = pscc_sim_init();
  if (fd < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Could not initialise sim\n");
    return (-1);
  }

  pscc_sim_init_done = true;
  return fd;
}

/**
 * deinitialize_sim()
 **/
static void deinitialize_sim(void)
{
  pscc_sim_deinit();
  pscc_sim_init_done = false;
}

/**
 * pscc_set_log_level()
 **/
void pscc_set_log_level(util_log_type_t log_level)
{
  if (log_level > 0) {
  /* It should not be possible to turn of ERROR logs. */
    pscc_log_level = (log_level | UTIL_LOG_TYPE_ERROR);
    PSCC_DBG_TRACE(LOG_INFO, "pscc_log_level is set to : %d",pscc_log_level);
  }
}

/**
 * pscc_get_log_level()
 **/
util_log_type_t pscc_get_log_level()
{
  return pscc_log_level;
}
