/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "server.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "tuningServer_utils.h"
#include "ste_adm_client.h"

#define TUNING_SERVER_VERSION "1.0.7"
#define TUNING_SERVER_DEFAULT_PORT 30001

static void init_tables(int portNumber)
{
    int chid = 0;
    char portString[7];

    sprintf(portString, "%5d", portNumber);
    portString[6] = '\0';

    // Ugly but all the '(const *)' are just to get rid of the compiler warnings :-(
    char *const cmd[][14] = {{(char *)"iptables", (char *)"--t", (char *)"filter", (char *)"-A", (char *)"INPUT", (char *)"-p", (char *)"tcp", (char *)"--dport", (char *)portString, (char *)"-i", (char *)"usb0", (char *)"-j", (char *)"ACCEPT", (char *)0},
        {(char *)"iptables", (char *)"--t", (char *)"filter", (char *)"-A", (char *)"INPUT", (char *)"-p", (char *)"tcp", (char *)"--dport", (char *)portString, (char *)"-j", (char *)"DROP", (char *)0},
        {(char *)"iptables", (char *)"--t", (char *)"filter", (char *)"-A", (char *)"OUTPUT", (char *)"-p", (char *)"tcp", (char *)"--sport", (char *)portString, (char *)"-o", (char *)"usb0", (char *)"-j", (char *)"ACCEPT", (char *)0},
        {(char *)"iptables", (char *)"--t", (char *)"filter", (char *)"-A", (char *)"OUTPUT", (char *)"-p", (char *)"tcp", (char *)"--sport", (char *)portString, (char *)"-j", (char *)"DROP", (char *)0},
        {(char *)"iptables", (char *)"--list", (char *)"--verbose", (char *)0}
    };

    int i = 0;

    for (i = 0; i < 4; i++) {
        chid = fork();

        if (0 == chid) {
            execvp("iptables", cmd[i]);
        } else if (chid > 0) {
            int status, ret;
            ret = waitpid(chid, &status, 0);
        } else {
            printf("fork() returned %d\n", chid);
            i--;
        }
    }

    FILE *fd = fopen("/tmp/ats", "w");

    if (fd != 0) {
        fprintf(fd, "%d\n", portNumber);
        fclose(fd);
    }
}

static void clear_tables()
{
    int chid = 0;
    int portNumber = 0;
    char portString[7];

    FILE *fd = fopen("/tmp/ats", "r");

    if (fd != 0) {
        fscanf(fd, "%d", &portNumber);
        fclose(fd);
    }

    if (portNumber == 0) {
        printf("No saved TCP port number!! \n");
        return;
    }

    sprintf(portString, "%5d", portNumber);
    portString[6] = '\0';

    // Ugly but all the '(const *)' are just to get rid of the compiler warnings :-(
    char *const cmd[][14] = {{(char *)"iptables", (char *)"--t", (char *)"filter", (char *)"-D", (char *)"INPUT", (char *)"-p", (char *)"tcp", (char *)"--dport", (char *)portString, (char *)"-i", (char *)"usb0", (char *)"-j", (char *)"ACCEPT", (char *)0},
        {(char *)"iptables", (char *)"--t", (char *)"filter", (char *)"-D", (char *)"INPUT", (char *)"-p", (char *)"tcp", (char *)"--dport", (char *)portString, (char *)"-j", (char *)"DROP", (char *)0},
        {(char *)"iptables", (char *)"--t", (char *)"filter", (char *)"-D", (char *)"OUTPUT", (char *)"-p", (char *)"tcp", (char *)"--sport", (char *)portString, (char *)"-o", (char *)"usb0", (char *)"-j", (char *)"ACCEPT", (char *)0},
        {(char *)"iptables", (char *)"--t", (char *)"filter", (char *)"-D", (char *)"OUTPUT", (char *)"-p", (char *)"tcp", (char *)"--sport", (char *)portString, (char *)"-j", (char *)"DROP", (char *)0},
        {(char *)"iptables", (char *)"--list", (char *)"--verbose", (char *)0}
    };


    for (int i = 0; i < 4; i++) {
        chid = fork();

        if (0 == chid) {
            execvp("iptables", cmd[i]);
        } else if (chid > 0) {
            int status, ret;
            ret = waitpid(chid, &status, 0);
        } else {
            printf("fork() returned %d\n", chid);
            i--;
        }
    }
}

static void start_srv(char *appname, int port)
{
    int i = 0;
    pid_t pid = -1;
    pid_t sid = -1;

    trim(appname, "./");

    if ((pid = pidof(appname, getpid())) != -1) {
        fprintf(stderr, "%s seems to be already running.\n", appname);
    } else {
        int pid = fork();

        if (pid == 0) {
            // Create a new SID for the child process
            sid = setsid();

            if (sid < 0) {
                exit(EXIT_FAILURE);
            }

            // Close all descriptors
            for (i = getdtablesize(); i >= 0; --i) {
                close(i);
            }

            // Handle standard I/O
            i = open("/dev/null", O_RDWR);
            if(i < 0){
                exit(EXIT_FAILURE);
            }
            dup(i);
            dup(i);

            // Handle some signals
            signal(SIGCHLD, SIG_IGN); /* ignore child */
            signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
            signal(SIGTTOU, SIG_IGN);
            signal(SIGTTIN, SIG_IGN);

            //init_tables(port);
            Server tuning_server;
            tuning_server.startServer(port);
        } else {
            exit(EXIT_SUCCESS);
        }
    }
}

static void stop_srv(char *appname)
{
    pid_t pid = -1;

    ste_adm_res_t res = ste_adm_client_set_tuning_mode(0);
    if (res != STE_ADM_RES_OK) {
        ALOGE("stop_srv: ste_adm_client_set_tuning_mode failed res=%d", res);
    }

    trim(appname, "./");

    if ((pid = pidof(appname, getpid())) != -1) {
        if (kill(pid, SIGTERM) != 0) {
            fprintf(stderr, "Failed to terminate %s %d.", appname, errno);
            exit(EXIT_FAILURE);
        }

        //clear_tables();
    } else {
        fprintf(stderr, "%s does not seem to be running.\n", appname);
    }
}

void showVersion()
{
    printf("tuning_server version: %s\n", TUNING_SERVER_VERSION);
}

void shortHelp()
{
    printf("Usage: tuning_server [start [port]] | [stop] | -v\n");
}

int main(int argc, char **argv)
{
    /*
     * give the user the possibility to not start as a daemon if wanted
     */
    int port;
    char *username;

    if (argc == 1) {
        printf("%s: Nothing to do\n", *argv);
        shortHelp();
        exit(0);
    }

    username = getlogin();

    // If we're not root then exit
    if (0 != strcmp(username, "root")) {
        fprintf(stderr, "tuning_server: User '%s' is not allowed to use this service\n", username);
        exit(EXIT_FAILURE);
    }

    if (argc > 1) {
        if (strcmp("start", argv[1]) == 0) {
            if (argc > 2) {
                port = atoi(argv[2]);
            } else {
                port = TUNING_SERVER_DEFAULT_PORT ;
            }

            start_srv(*argv, port);
        } else if (strcmp("stop", argv[1]) == 0) {
            stop_srv(*argv);
        } else if (strcmp("-v", argv[1]) == 0) {
            showVersion();
        } else {
            fprintf(stderr, "%s: Unknown command `%s'.\n", argv[0], argv[1]);
            shortHelp();
        }
    }

    return 0;
}
