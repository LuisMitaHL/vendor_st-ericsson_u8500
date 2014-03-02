/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "../include/utils.h"
#include "../include/server.h"
#include "../include/logger.h"
#include "../include/common.h"
#include "../api/t_mm_probe.h"

static int sockfd = -1;
static char * runningdir = RUNNING_DIR;

/*********************************************************************************/
static void longhelp(const char* appname) {
  printf("Usage : %s [<option> <value>] <command>\n", appname);

  printf("\nAvailable options: \n");
  printf("\t -l <file>\t\t Log into a file instead of syslog.\n");
  printf("\t -p <port no> \t\t Sets server listen port. Default is %s.\n",
      itoa(PORT, 10));
  printf("\t -d <dir>\t\t Specifies the working directory of mmprobed. Default is %s.\n",
      RUNNING_DIR);
  printf("\t -e <ProbeID1,ProbeID2>\t List of probes to be enabled when probing locally to phone\n");
  printf("\t\t\t\t seperated by comma.\n");
  printf("\t -s <path>\t\t Store location for .bin/.raw files when logging probing locally to phone.\n");
  printf("\t -c \t\t\t Combined file format. All probes in one .bin file. Needs LogDecoder for decoding.\n");
  printf("\t\t\t\t Omit -c => each probe in a seperate .raw file.\n");
  printf("\t ------------------------------------------------------ \n");
  printf("\nAvailable commands: \n");
  printf("\t start \t Starts the server.\n");
  printf("\t stop \t Stops the server.\n");
  printf("\t ------------------------------------------------------ \n");
  printf("\t example \t store probeID 26 and 27 locally to phone in a combined pcm file:\n");
  printf("\t mmprobed -e 26,27 -s /sdcard/mmprobe/ -c start\n");
}

/*********************************************************************************/
static void shorthelp(void) {
  fprintf(stderr, "Use -h for help.\n");
}

/*********************************************************************************/
void signal_handler(int sig) {
  switch(sig) {
  case SIGHUP:
    log_message(L_INFO, "hang-up signal catched");
    break;
  case SIGTERM:
    log_message(L_INFO, "terminate signal catched");

    if ((chdir(runningdir)) < 0) {
      log_message(L_WARN, "Could not change to working directory %s.",
          runningdir);
    }

    if (stop_server(sockfd) != 0) {
      log_message(L_ERR, "Could not stop the server.");
    }

    stop_log();

    exit(EXIT_SUCCESS);
    break;
  default:
    log_message(L_WARN, "unhandled signal catched %d", sig);
    break;
  }
}

/*********************************************************************************/
void daemonize() {
  int i = 0;
  pid_t pid, sid;

  if (getppid() == 1) {
    return; // Already a daemon
  }

  // Fork off the parent process
  pid = fork();
  if (pid < 0) {
    log_message(L_ERR, "Could not fork.");
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    // If we got a good PID, then we can exit the parent process.
    exit(EXIT_SUCCESS);
  }

  // Create a new SID for the child process
  sid = setsid();
  if (sid < 0) {
    log_message(L_ERR,"Could not set the session ID.");
    exit(EXIT_FAILURE);
  }

  // Close all descriptors
  for (i = getdtablesize(); i >= 0; --i) {
    close(i);
  }

  // Handle standard I/O
  i = open("/dev/null",O_RDWR);
  dup(i);
  dup(i);

  // Set newly created file permissions
  umask(027);

  // Change the current working directory
  if ((chdir(runningdir)) < 0) {
    log_message(L_WARN,"Could not change to working directory %s.",
        runningdir);
  }

  // Handle some signals
  signal(SIGCHLD, SIG_IGN); /* ignore child */
  signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
  signal(SIGTTOU, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGHUP, signal_handler); /* catch hang-up signal */
  signal(SIGTERM, signal_handler); /* catch kill signal */
}

/*********************************************************************************/
void start_srv(const int port, char* appname, char* logfile, char* enabledProbes, int logDecoderFormat) {
  pid_t pid = -1;

  // Check if mmprobe is already running.
  trim(appname, "./");

  if ((pid = pidof(appname, getpid())) != -1) {
    fprintf(stderr, "%s seems to be already running.\n", appname);
  } else {

    // Enter daemon mode
    daemonize();
    // Start the logging engine. Need to be done after daemonize since after fork
    // child closes all opened file descriptors
    start_log(appname, logfile);
    // Start the server
    if (init_server(port, &sockfd) != 0) {
      log_message(L_ERR,"Failed to create the server.");
      exit(EXIT_FAILURE);
    }
    if (start_server(sockfd, enabledProbes, logDecoderFormat) != 0) {
      log_message(L_ERR,"Failed to start the server.");
      exit(EXIT_FAILURE);
    }
  }
}

/*********************************************************************************/
void stop_srv(char* appname) {
  pid_t pid = -1;

  trim(appname, "./");

  if ((pid = pidof(appname, getpid())) != -1) {
    if (kill(pid, SIGTERM) != 0) {
      log_message(L_ERR, "Failed to terminate server %d.", errno);
      exit(EXIT_FAILURE);
    }
  }
  else {
    fprintf(stderr, "%s does not seem to be running.\n", appname);
  }
}

/*********************************************************************************/
int main(int argc, char **argv) {
  int port = PORT;
  int c = 0;
  char* logfile = NULL;
  char* enabledProbes = NULL;
  char* storePath = NULL;
  int logDecoderFormat = 0; // 1=>LogDecoder format single file, 0=>multiple files in .raw (pcm) format
  opterr = 0;
#ifndef X86
  //check if user is allowed to start mmprobed
  char *username = NULL;
  username = getlogin();
  if (0 != strcmp(username, "root")) {
    fprintf(stderr, "mmprobed %s is not allowed to start this server.\n", username);
    exit(1);
  }
#endif

  //Create directory if not already exists the "/data/tmp" direcorty
  if (mkdir(MMPROBE_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
    if (errno != EEXIST) {
      log_message(L_ERR, "server: could not create dir:%s, error=%s" , MMPROBE_DIR, strerror(errno));
      exit(1);
    }
  }

  // Parses the command line options
  while ((c = getopt(argc, argv, "hl:p:d:e:s:c")) != -1)
  switch (c) {
    case 'l':
      logfile = optarg;
      break;
    case 'd':
      runningdir = optarg;
      break;
    case 'p':
      port = atoi(optarg);
      break;
    case 'h':
      longhelp(*argv);
      exit(EXIT_SUCCESS);
    case 'e':
      enabledProbes = optarg;
      break;
    case 's':
      {
        FILE *file;
        storePath = optarg;
        //can already write filepath here
        file = fopen(MMPROBETXT , "w");
        if (file != NULL) {
          fwrite(storePath, 1, strlen(storePath), file);
          fclose(file);
        } else {
          log_message(L_ERR, "server: could not open file:%s for writing", MMPROBETXT);
        }
      }
      break;
    case 'c':
      logDecoderFormat = 1;
      break;
    case '?':
      if (optopt == 'p' || optopt == 'l' || optopt =='d' || optopt == 'e' || optopt == 's') {
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      }
      else if (isprint(optopt)) {
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      }
      else {
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      }
      shorthelp();
      exit(EXIT_FAILURE);
    default:
      abort();
    }

  // Parse other command line arguments
  if (optind == argc) {
    fprintf(stderr, "No command specified.\n");
    shorthelp();
    exit(EXIT_FAILURE);
  }
  else {
    // For loop is kept if for possibility of command stacking.
    for (c = optind; c < argc; c++) {
      // I generally dislike this, but for only few commands, this is sufficient.
      if (strcmp("start", argv[c]) == 0) {
        start_srv(port, *argv, logfile, enabledProbes, logDecoderFormat);
      } else if (strcmp("stop", argv[c]) == 0) {
        stop_srv(*argv);
      } else {
        fprintf(stderr, "Unknown command `%s'.\n", argv[c]);
        shorthelp();
      }
      break;
    }
  }

  exit(EXIT_SUCCESS);
}
