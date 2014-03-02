/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "../include/logger.h"
#ifndef ANDROID
//#define MODULE_TEST

static boolean logstarted = false;
static boolean logsystem = true;
static FILE* file = NULL;

/*********************************************************************************/
boolean start_log(const char* appname, const char* logfile) {
  if (!logstarted) {
    if (logfile == NULL) {
      logsystem = true;
      openlog(appname, LOG_CONS | LOG_NOWAIT | LOG_PID, LOG_DAEMON);
    }
    else {
      logsystem = false;
      if ((file = fopen(logfile,"a+")) == NULL) {
        fprintf(stderr, "Could not open logfile (%s) for writing: %s.\n",
            logfile, strerror(errno));
        return false;
      }
    }
    logstarted = true;
  }
  return logstarted;
}

/*********************************************************************************/
void log_message(loglevel_t level, const char* logstring, ...) {
  if (logstarted) {
    va_list ap;
    int d = 0;
    char *s, c;

    va_start(ap, logstring);

    d = va_arg(ap, int);
    s = va_arg(ap, char*);
    c = (char) va_arg(ap, int);

    if (logsystem) {
      syslog(level, logstring, d, s, c);
    }
    else {
      if (file != NULL) {
        char outstr[20];
        time_t t = time(NULL);
        struct tm *tmp = localtime(&t);

        if (tmp  != NULL) {
          strftime(outstr, sizeof(outstr), "%b %d %T", tmp);
          fprintf(file, "%s ", outstr);
        }
        fprintf(file, logstring, d, s, c);
        fprintf(file, "\n");
        fflush(file); // needed to force output directly
      }
    }
    va_end(ap);
  }
}

/*********************************************************************************/
void stop_log(void) {
  if (logstarted) {
    if (logsystem) {
      closelog();
    }
    else {
      if (file != NULL) {
        fclose(file);
        file = NULL;
      }
    }
    logstarted = false;
  }
}

/*********************************************************************************/
#ifdef MODULE_TEST
void message(const int variable) {
  const char* s = "brain";
  char c = 'B';
  log_message(L_INFO, "Pure informational log");
  log_message(L_INFO, "Using some variable: %d", variable);
  log_message(L_WARN, "You forgot your: %s",s);
  log_message(L_ERR, "Error: %c, is a stupid letter", c);
}

int main(int argc, char **argv) {
  const char* logfile = {"./eprobed.log"};

  start_log(*argv, logfile);
  message(argc);
  stop_log();
  start_log(*argv, NULL);
  message(argc);
  stop_log();
}
#endif /* ANDROID*/

#endif
