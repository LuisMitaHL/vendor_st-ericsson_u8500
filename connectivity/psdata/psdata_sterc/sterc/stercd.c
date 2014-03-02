/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
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
#include <getopt.h>
#include <signal.h>
#include <stdarg.h>
#include <assert.h>
#include "stercd.h"
#include "libstecom.h"
#include "sterc_msg.h"
#include "sterc_handler.h"
#include "sterc_pscc.h"
#include "sterc_runscript.h"
#include "util_security.h"

/* Use Logcat for android, otherwise default log (syslog) */
#ifdef STERC_SW_VARIANT_ANDROID
#define LOG_TAG "RIL STERCD"
#include <cutils/log.h>
#include <time.h>
#include <sys/time.h>
#else
#include <syslog.h>
#endif //STERC_SW_VARIANT_ANDROID

#define PRINT_BUF_SIZE  (512)
/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/
static char req[STERCD_BUFSIZ];
static int req_len;
static char resp[STERCD_BUFSIZ];
static int resp_len;
static int err = 0;
static int daemon_mode = 1;
static int debug_mode = 0;
static struct stec_ctrl *ctrl_handle_p=NULL;
static struct stec_ctrl *event_handle_p=NULL;
//for config file and all parameters read from that
char* config_path = "sterc.conf";
static const char pscc_ctrl[] = "/dev/socket/pscc_ctrl_server";
static const char pscc_event[] = "/dev/socket/pscc_event_server";
static int sterc_log_level =  (UTIL_LOG_TYPE_INFO | UTIL_LOG_TYPE_ERROR);

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static void print_usage(char *prog);
static sterc_transport_type_t get_transport_type(char *str_p);
static void handle_terminate(int signo);
static void handle_hup(int signo);
static void close_handles(void);

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

mpl_config_t stercd_config;

/**
 * main()
 **/
int main(int argc, char *argv[])
{

    int result;
    int nfds;
    fd_set rfds;
    int pscc_event_fd;
    int sterclog_fd = -1;

    struct sockaddr_un ctrl_addr_un;
    struct sockaddr_un event_addr_un;
    struct sockaddr_in ctrl_addr_in;
    struct sockaddr_in event_addr_in;
    pid_t pid;
    pid_t sid;
    sterc_transport_type_t transport_type = tt_unix;
    sterc_transport_type_t pscc_transport_type = tt_unix;
    char *pscc_ctrl_p = (char*)pscc_ctrl,*pscc_event_p=(char*)pscc_event;
    int pscc_reconnect_tries = 5;
    mpl_param_element_t* param_elem_p;

    /* optarg stuff */
    int c;
    extern char *optarg;
    extern int optind, optopt, opterr;
    int option_index = 0;
    static struct option long_options[] = {
      {"pt", 1, 0, 0},
      {"pc", 1, 0, 0},
      {"pe", 1, 0, 0},
      {0, 0, 0, 0}
    };
    /* optarg end */

    /* Set umask before anything else */
    util_set_restricted_umask();
    sterclog_fd = sterc_log_init();

    while ((c = getopt(argc, argv, "fd:t:r:c:s:")) != -1)
    {
      switch (c)
      {
      case 0:
        if(strncmp(long_options[option_index].name,"pt",2) == 0)
        {
          pscc_transport_type = get_transport_type(optarg);
          if (pscc_transport_type == tt_unknown)
          {
            print_usage(argv[0]);
            exit(1);
          }
        }
        else if(strncmp(long_options[option_index].name,"pc",2) == 0)
          pscc_ctrl_p = optarg;
        else if(strncmp(long_options[option_index].name,"pe",2) == 0)
          pscc_event_p = optarg;
        else
        {
          print_usage(argv[0]);
          exit(1);
        }
        break;
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
        sterc_runscript_path_connect = optarg;
        break;
      case 's':
        sterc_runscript_path_disconnect = optarg;
        break;
      case 't':
        transport_type = get_transport_type(optarg);
        break;
      case ':':
      case '?':
          print_usage(argv[0]);
          exit(1);
      }
    }

    if (daemon_mode)
    {
      /* Daemonize */
      int fd;
      /* Become child of init */
      pid = fork();
      if (pid < 0)
      {
        exit(1);
      }
      if (pid > 0)
      {
        exit(0);
      }

      /* Override the umask */
      umask(0);

      /* Start a new session */
      sid = setsid();
      if (sid < 0)
      {
        exit(1);
      }

      /* Change working directory to something that surely exists */
      if (chdir("/") < 0)
      {
        exit(1);
      }

      /* re-direct standard file descriptors to /dev/null */
      fd = open("/dev/null", O_RDWR);
      if(fd<0)
      {
        close(0);
        close(1);
        close(2);
      }
      else
      {
        if(dup2(fd, STDIN_FILENO)<0)
          close(0);
        if(dup2(fd, STDOUT_FILENO)<0)
          close(1);
        if(dup2(fd, STDERR_FILENO)<0)
          close(2);
        close(fd);
      }

      util_set_restricted_umask();

      /* We are now daemonized */
    }

#ifndef STERC_SW_VARIANT_ANDROID
    /* Open any logs here */
    openlog(argv[0], LOG_CONS | LOG_PID, LOG_DAEMON);
#endif //STERC_SW_VARIANT_ANDROID

    /* Handle some signals */
    signal(SIGTERM, handle_terminate);
    signal(SIGHUP, handle_hup);

   /*Handle SIGPIPE to Avoid Crash*/
   if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
   STERC_DBG_TRACE( LOG_ERR, "\n%d: SIG_ERR when ignoring SIGPIPE", __LINE__ );
   abort();
   }

    if (!daemon_mode)
      signal(SIGINT, handle_terminate);

    sterc_init(NULL, &stercd_log_func);

    mpl_config_init(NULL, &stercd_log_func, &stercd_config, STERC_PARAM_SET_ID);

    if(0!=mpl_config_read_config(config_path, &stercd_config, STERC_PARAM_SET_ID))
      STERC_DBG_TRACE(LOG_WARNING, "Can't open config file %s\n",config_path);
    if (sterc_runscript_path_connect == NULL)
      STERC_DBG_TRACE(LOG_WARNING, "sterc_runscript_path_connect is NULL!\n");
    if (sterc_runscript_path_disconnect == NULL)
      STERC_DBG_TRACE(LOG_WARNING, "sterc_runscript_path_disconnect is NULL!\n");

    /* if debug mode is not enabled at startup, check if it is enabled in config */
    if (0 == debug_mode)
    {
      param_elem_p = mpl_config_get_para(sterc_paramid_debug_mode, &stercd_config);
      if ((NULL != param_elem_p) && (NULL != param_elem_p->value_p) && *((bool*) param_elem_p->value_p))
      {
        debug_mode = 1;
      }
    }
    if (0 != debug_mode)
    {
      STERC_DBG_TRACE(LOG_INFO, "Debug mode is enabled\n");
    }

    if (transport_type == tt_unix)
    {
      memset(&ctrl_addr_un, 0, sizeof(ctrl_addr_un));
      ctrl_addr_un.sun_family = AF_UNIX;
      strncpy(ctrl_addr_un.sun_path, STERC_CTRL_PATH, UNIX_PATH_MAX) ;
      unlink(ctrl_addr_un.sun_path);
      ctrl_handle_p = stec_opensrv((struct sockaddr*) &ctrl_addr_un, sizeof(ctrl_addr_un));
      if (ctrl_handle_p == NULL)
      {
        STERC_DBG_TRACE( LOG_ERR, "\n%d: stec_opensrv failed\n", __LINE__ );
        close_handles();
        exit(-1);
      }

      memset(&event_addr_un, 0, sizeof(event_addr_un));
      event_addr_un.sun_family = AF_UNIX;
      strncpy(event_addr_un.sun_path, STERC_EVENT_PATH, UNIX_PATH_MAX) ;
      unlink(event_addr_un.sun_path);
      event_handle_p = stec_opensrv((struct sockaddr*) &event_addr_un, sizeof(event_addr_un));
      if (event_handle_p == NULL)
      {
        STERC_DBG_TRACE( LOG_ERR, "\n%d: stec_opensrv failed\n", __LINE__ );
        close_handles();
        exit(-1);
      }
    }
    else if (transport_type == tt_ip)
    {
      memset(&ctrl_addr_in, 0, sizeof(ctrl_addr_in));
      ctrl_addr_in.sin_family = AF_INET;
      ctrl_addr_in.sin_port = htons(STERC_CTRL_PORT);
      ctrl_addr_in.sin_addr.s_addr = INADDR_ANY;
      ctrl_handle_p = stec_opensrv((struct sockaddr*) &ctrl_addr_in, sizeof(ctrl_addr_in));
      if (ctrl_handle_p == NULL)
      {
        STERC_DBG_TRACE( LOG_ERR, "\n%d: stec_opensrv failed\n", __LINE__ );
        close_handles();
        exit(-1);
      }

      memset(&event_addr_in, 0, sizeof(event_addr_in));
      event_addr_in.sin_family = AF_INET;
      event_addr_in.sin_port = htons(STERC_EVENT_PORT);
      event_addr_in.sin_addr.s_addr = INADDR_ANY;
      event_handle_p = stec_opensrv((struct sockaddr*) &event_addr_in, sizeof(event_addr_in));
      if (event_handle_p == NULL)
      {
        STERC_DBG_TRACE( LOG_ERR, "\nstec_opensrv failed\n");
        close_handles();
        exit(-1);
      }
    }

    /* initialise pscc, if pscc has not started yet wait one second and try again */
    while (pscc_reconnect_tries>0) {
      if((pscc_event_fd = sterc_pscc_init(pscc_transport_type,pscc_ctrl_p,pscc_event_p)) < 0)
      {
        STERC_DBG_TRACE( LOG_WARNING, "sterc_pscc_init failed, %d retries left.\n", pscc_reconnect_tries);
        pscc_reconnect_tries--;
        sleep(1);
      }
      else
        break;
    }

    /* failed to initialize pscc */
    if (pscc_event_fd < 0) {
      STERC_DBG_TRACE( LOG_ERR, "failed to initialize pscc from sterc");
      close_handles();
      exit(-1);
    }

    do
    {
      FD_ZERO(&rfds);
      nfds = 0;
      FD_SET(stec_get_fd(ctrl_handle_p), &rfds);
      nfds = stec_get_fd(ctrl_handle_p) > nfds ? stec_get_fd(ctrl_handle_p) : nfds;
      FD_SET( stec_get_fd(event_handle_p), &rfds );
      nfds = stec_get_fd(event_handle_p) > nfds ? stec_get_fd(event_handle_p) : nfds;
      FD_SET( pscc_event_fd, &rfds );
      nfds = pscc_event_fd > nfds ? pscc_event_fd : nfds;
      FD_SET(sterclog_fd, &rfds);
      nfds = sterclog_fd > nfds ? sterclog_fd : nfds;
#ifdef STERC_MODULE_TEST_ENABLED
      if(!daemon_mode)
        FD_SET( 0, &rfds );
#endif //STERC_MODULE_TEST_ENABLED
      nfds += 1;

      result = select(nfds, &rfds, NULL, NULL, 0);

      if (result < 0)
      {
        if (errno == EINTR)
        {
          STERC_DBG_TRACE(LOG_INFO, "select returned because of a signal\n");
          continue;
        }
        else
        {
          STERC_DBG_TRACE(LOG_ERR, "select returned error: %s\n", strerror(errno));
          break;
        }
      }

      if (FD_ISSET(stec_get_fd(ctrl_handle_p), &rfds))
      {
        req_len = stec_recv(ctrl_handle_p, req, STERCD_BUFSIZ,-1);
        if (req_len < 0)
        {
          STERC_DBG_TRACE(LOG_ERR, "stec_recv: %s\n", strerror(errno));
          return -1;
        }
        resp_len = STERCD_BUFSIZ;
        result = sterc_handler(ctrl_handle_p,req, (size_t)req_len, resp, (size_t*)&resp_len);
        if (result < 0)
        {
          STERC_DBG_TRACE(LOG_ERR, "sterc_handler failed\n");
          /* Ignore and continue loop */
        }
        else if (resp_len > 0)
        {
          if (stec_send(ctrl_handle_p, resp, resp_len) < 0)
          {
            STERC_DBG_TRACE(LOG_ERR, "stec_send: %s\n", strerror(errno));
          }
          STERC_DBG_TRACE(LOG_DEBUG, "Sending response message: %s\n", resp);
        }
      }

      if (FD_ISSET(stec_get_fd(event_handle_p), &rfds))
      {
        if(stec_process_recv(event_handle_p) < 0)
        {
          STERC_DBG_TRACE(LOG_ERR, "stec_process_recv failed\n");
        }
      }

      if (FD_ISSET(pscc_event_fd, &rfds))
      {
        if (sterc_pscc_handler(pscc_event_fd) < 0)
        {
          STERC_DBG_TRACE(LOG_ERR, "sterc_pscc_handler failed\n");
        }
      }
      if (FD_ISSET(sterclog_fd, &rfds))
      {
        if (sterc_log_select_callback(sterclog_fd, NULL) < 0)
        {
          STERC_DBG_TRACE(LOG_ERR,"pscc_log_select_callback failed\n");
        }
      }

#ifdef STERC_MODULE_TEST_ENABLED
      if (!daemon_mode && FD_ISSET(0, &rfds))
      {
        read(0,req, STERCD_BUFSIZ);
        if(0 == strncmp(req,"quit",strlen("quit")))
          break;

        if(0 == strncmp(req,"sync",strlen("sync")))
          printf("stercd: sync\n");
      }
#endif //STERC_MODULE_TEST_ENABLED
    } while(!err);

    close_handles();
    return(err);
}

/**
 * stercd_log_func()
 **/
void stercd_log_func(void* user_p, int level, const char* file, int line, const char *format, ...)
{

  STERC_IDENTIFIER_NOT_USED(user_p);

/* Use Logcat for android, otherwise default log (syslog) */
#ifdef STERC_SW_VARIANT_ANDROID
  STERC_IDENTIFIER_NOT_USED(file);
  STERC_IDENTIFIER_NOT_USED(line);

  char *buf = malloc(PRINT_BUF_SIZE);
  int log_level = (sterc_log_level & level);
  va_list arg;
  va_start(arg, format);

  vsnprintf(buf, PRINT_BUF_SIZE, format, arg);
  buf[PRINT_BUF_SIZE - 1] = '\0';
  switch ( log_level ) {
  case UTIL_LOG_TYPE_ERROR:
    STERC_LOG_E("%s",buf);
    break;
  case UTIL_LOG_TYPE_WARNING:
    STERC_LOG_W("%s",buf);
    break;
  case UTIL_LOG_TYPE_INFO:
    STERC_LOG_I("%s",buf);
    break;
  case UTIL_LOG_TYPE_DEBUG:
    STERC_LOG_D("%s",buf);
    break;
  case UTIL_LOG_TYPE_VERBOSE:
    STERC_LOG_V("%s",buf);
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
#endif //STERC_SW_VARIANT_ANDROID
}

/**
 * stercd_send_event()
 **/
int stercd_send_event(char *event_p, int event_len)
{
  // send to all
  return stec_sendevent(event_handle_p, event_p, event_len);
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
  STERC_DBG_TRACE(LOG_DEBUG, "Closing handles\n");
  sterc_log_close();
  sterc_pscc_deinit();
  if(ctrl_handle_p != NULL)
    stec_close(ctrl_handle_p);
  if(event_handle_p != NULL)
    stec_close(event_handle_p);
  mpl_config_reset(&stercd_config);
}

/**
 * handle_terminate()
 **/
static void handle_terminate(int signo)
{
  STERC_DBG_TRACE(LOG_INFO, "Terminated by signal %d.\n", signo);
  close_handles();
  exit(0);
}

/**
 * handle_hup()
 **/
static void handle_hup(int signo)
{
  STERC_DBG_TRACE(LOG_INFO, "Load default parameters by signal %d.\n", signo);
  mpl_config_reset(&stercd_config);
  if(0!=mpl_config_read_config(config_path, &stercd_config, STERC_PARAM_SET_ID))
    STERC_DBG_TRACE(LOG_ERR, "Can't open config file %s\n",config_path);
}

/**
 * print_usage()
 **/
static void print_usage(char *prog)
{
#ifdef STERC_SW_VARIANT_ANDROID
  STERC_DBG_TRACE(LOG_INFO,
#else
  fprintf(stderr,
#endif
    "Usage: %s [-f] [-d level] [-t unix/ip] [-c <config file>]  [-r <run-script>] [--pt uinx/ip] [--pc ctrl] [--pe event]\n"
    "       -f   : foreground mode (no daemon)\n"
    "       -d   : debug\n"
    "       -t   : transport type\n"
    "       -c   : specify the config file\n"
    "       -r   : specify the connect run-script file\n"
    "       -s   : specify the disconnect run-script file\n"
    "       --pt : pscc transport type\n"
    "       --pc : pscc control socket address\n"
    "       --pe : pscc event socket address\n", prog);
}

static sterc_transport_type_t get_transport_type(char *str_p)
{
  assert(str_p != NULL);

  if (!strncmp(str_p, "ip", 2))
    return (tt_ip);


  if (!strncmp(str_p, "unix", 4))
    return (tt_unix);

  return (tt_unknown);
}

/**
 * sterc_set_log_level()
 **/
void sterc_set_log_level(util_log_type_t log_level)
{
  if (log_level > 0) {
  /* It should not be possible to turn of ERROR logs. */
    sterc_log_level = (log_level | UTIL_LOG_TYPE_ERROR);
    STERC_DBG_TRACE(LOG_INFO, "sterc_log_level is set to  : %d",sterc_log_level);
  }
}

/**
 * sterc_get_log_level()
 **/
util_log_type_t sterc_get_log_level()
{
  return sterc_log_level;
}
