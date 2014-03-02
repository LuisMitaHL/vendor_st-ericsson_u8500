/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include <bass_app.h>
#include <shm_netlnk.h>
#include "modem-sup.h"
#include "fwhandler.h"
#include "t_stamps.h"
#include <libmlr.h>
#include "msup-server.h"

#define BINNAME "MSUP"

#ifdef HAVE_ANDROID_OS
#define LOG_TAG BINNAME
#include <cutils/log.h>
#define OPENLOG(facility) ((void)0)
#else
#include <syslog.h>
#define OPENLOG(facility) openlog(BINNAME, LOG_PID | LOG_CONS, facility)
#define LOG(priority, format, ...) syslog(priority, format, ##__VA_ARGS__)
#define ALOGV(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGD(format, ...)   LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#define ALOGI(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGW(format, ...)   LOG(LOG_WARNING, format, ##__VA_ARGS__)
#define ALOGE(format, ...)   LOG(LOG_ERR, format, ##__VA_ARGS__)
#endif


info_reset_s * p_reset_hdl;

static int daemonize(void)
{
    pid_t pid, sid;

    ALOGE("%s\n", __func__);

    /* MODEM_INIT_LOG; */
    ALOGI("starting the daemonizing process\n");

    pid = fork();

    switch (pid) {
    case 0:
        break;
    case -1:
        ALOGE("%s\n", strerror(errno));
        return errno;
    default:
        exit(EXIT_SUCCESS);
    }

    umask(0);
    sid = setsid();

    if (sid < 0) {
        ALOGE("%s\n", strerror(errno));
        return errno;
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return 0;
}


static int modem_debug(struct modem_sup_info *modem_sup)
{
    int ret;
    (void)modem_sup;
    printf("%s\n", __func__);

    ret = smcl_unlock_dbg_modem_mem();

    if (ret != SMCL_OK) {
        ALOGE("failed: unlock modem dbg\n");
        goto out;
    }

    ret = mlr_send_coredump_gen(MLR_TIMEOUT_SEC);

    if (ret != 1) {
        ALOGE("failed: CoreDump req\n");
        goto out;
    }

out:
    return ret;
}

static int modem_stop()
{
    int ret;

    printf("%s\n", __func__);
    ALOGI("modem stop\n");

    ret = smcl_reset_unlock_modem_mem();

    if (ret != SMCL_OK) {
        ALOGE("failed: halt modem\n");
        goto out;
    }

out:
    return ret;
}

static int modem_start(struct modem_sup_info *modem_sup)
{
    int ret = -1;
    int msg;
    int fd_max;
    fd_set readfds;
    struct timeval time_out;

    /* Set Modem time out  */
    time_out.tv_sec = MODEM_READY_TIMEOUT_SEC;
    time_out.tv_usec = 0;


    printf("%s\n", __func__);
    ALOGI("modem start abx500 id:%#x\n", modem_sup->abx500_id);

    ret = smcl_restart_modem(modem_sup->abx500_id);

    if (ret != SMCL_OK) {
        printf("error: restart modem\n");
        ALOGE("failed: restart modem\n");
        goto out;
    }

    do {
        FD_ZERO(&readfds);
        fd_max = modem_sup->fd_netlnk;
        FD_SET(modem_sup->fd_netlnk, &readfds);

        ret =  select(fd_max + 1, &readfds, NULL, NULL, \
                                &time_out);

        if (ret > 0) {
            if (FD_ISSET(modem_sup->fd_netlnk, &readfds)) {
                ret = netlnk_socket_recv(modem_sup->fd_netlnk, &msg);
            } else {
                ALOGI("No MODEM Answer : TimeOut expired \n");
                return -1;
            }
        } else {
            ALOGI("No SHM link acces \n");
            return -1;
        }
    } while (msg != MODEM_STATUS_ONLINE);

#ifdef MODEM_LOG_RELAY
	mlr_send_modem_ready();
#endif

    ret = 0;
    ALOGI("MODEM_RESTARTED\n");


out:
    return ret;
}

static int modem_status(struct modem_sup_info *modem_sup)
{
    int ret;
    int msg;

    printf("%s\n", __func__);
    ALOGI("modem status\n");

    ret = netlnk_socket_send(modem_sup->fd_netlnk, MODEM_QUERY_STATUS_REQ);

    if (ret) {
        ALOGE("failed: netlnk send MODEM_QUERY_STATUS_REQ\n");
        goto err;
    }

    ret = netlnk_socket_recv(modem_sup->fd_netlnk, &msg);

    if (ret) {
        ALOGE("failed: netlnk recv\n");
        goto err;
    }

    ALOGI("modem status = %d\n", msg);
    return msg;

err:
    return -1;
}

static int modem_load(struct modem_sup_info *modem_sup)
{
    int ret;

    printf("%s\n", __func__);
    ALOGI("modem loading\n");
    ret = firmware_upload(modem_sup->mloader_path);
    ALOGI("modem loaded\n");
    return ret;
}


static int catch_evt_modem(struct modem_sup_info *modem_sup)
{
    static int prev_msg = -1;
    int msg = -1, fd_max, fd, ret;
    msup_flags_t flags;
    struct timeval reset_timeout;
    struct timeval *tvp = NULL;
    fd_set readfds;

    printf("%s\n", __func__);
    ALOGI("wait catch modem event\n");

    modem_sup->force_restart = 0;
    modem_sup->dump_core = 1;

    /* Wait for a MODEM_RESET_IND message from PRCMU,
     * or a notification from an APE component. */
    do {
        FD_ZERO(&readfds);
        fd_max = modem_sup->fd_netlnk;
        FD_SET(modem_sup->fd_netlnk, &readfds);
        if (modem_sup->fd_msup >= 0) {
            fd_max = modem_sup->fd_msup > fd_max ? modem_sup->fd_msup : fd_max;
            FD_SET(modem_sup->fd_msup, &readfds);
        }

        ret =  select(fd_max + 1, &readfds, NULL, NULL, tvp);

        if (ret > 0) {

            if (FD_ISSET(modem_sup->fd_netlnk, &readfds)) {
                netlnk_socket_recv(modem_sup->fd_netlnk, &msg);
                ALOGI("receive netlnk msg = %02X\n", msg);
                if (prev_msg == MODEM_STATUS_ONLINE && msg == MODEM_STATUS_ONLINE)
                    goto err;
                prev_msg = msg;

            } else if (FD_ISSET(modem_sup->fd_msup, &readfds)) {
                if (msup_server_receive(modem_sup->msup_server_p, &flags) == 0) {
                    ALOGI("receive msup_server flags = %02X\n", flags);

                    if (flags == MSUP_FLAGS_RESET) {
                        modem_sup->force_restart = 0;
                        modem_sup->dump_core = 1;
                        ALOGI("Modem force restart Disabled, and core dump Enabled.\n");
                    } else {

                        if (flags & MSUP_FLAG_RESET_IMMINENT) {
                            reset_timeout.tv_sec = MSUP_RESET_TIMEOUT_MS / 1000;
                            reset_timeout.tv_usec = (MSUP_RESET_TIMEOUT_MS % 1000) * 1000;
                            tvp = &reset_timeout;
                            ALOGW("Modem reset is IMMINENT, timeout for reset indication is %d ms!\n", MSUP_RESET_TIMEOUT_MS);
                        }

                        if (flags & MSUP_FLAG_FORCE_MSR_AFTER_NEXT_RESET) {
                            modem_sup->force_restart = 1;
                            ALOGI("Modem force restart Enabled for next reset.\n");
                        }

                        if (flags & MSUP_FLAG_NO_COREDUMP_AFTER_NEXT_RESET) {
                            modem_sup->dump_core = 0;
                            ALOGI("Modem core dump Disabled for next reset.\n");
                        }
                    }
                }
            }

        } else if (ret < 0) {
            if (errno != EAGAIN) {
                ALOGE("%s: select() %d: %s\n", __func__, errno, strerror(errno));
                return -1;
            }

        } else {
            /* select timed out, reset flags */
            modem_sup->force_restart = 0;
            modem_sup->dump_core = 1;
            tvp = NULL;
            ALOGW("Modem reset indication not received within time limit, resetting restart & core dump flags to defult values.");
        }

    } while (msg != MODEM_RESET_IND);

    fd = open(ACQUIRE_WAKE_LOCK_PATH, O_RDWR);
    if (fd < 0) {
        ALOGE("Cannot open %s", ACQUIRE_WAKE_LOCK_PATH);
    } else {
        write(fd, WAKE_LOCK_STRING, sizeof(WAKE_LOCK_STRING) - 1);
        close(fd);
    }

    ALOGI("receive msg : MODEM_RESET_IND \n");

    return 0;

err:
    return -1;
}


/*
   Force target to restart
*/
int target_restart(void)
{
    ALOGI("\n Send Command to Shutdown and reboot the target ");
    ALOGI("TARGET_RESTARTED\n");

    return system("reboot");
};


static int modem_supervisor(struct modem_sup_info *modem_sup)
{

    int try = 0;
    int modem_reboot_valid = 1;
    int mstate = MODEM_CATCH_EVT;
    int ret = 0, fd;

    printf("%s\n", __func__);

    modem_sup->msup_server_p = msup_server_open(&modem_sup->fd_msup);

    if (modem_sup->msup_server_p == NULL) {
        ALOGW("failed: msup_server_open\n");
    }

    do {

        switch (mstate) {
        case MODEM_CATCH_EVT:
            /* Set to 0 and increase if modem restart fails */
            try = 0;

            ret = catch_evt_modem(modem_sup);

            if (ret) {
                ALOGE("err: catch modem event failed ret=%d\n", ret);
                break;
            }

            /* Store time stamp in modem reset list */
            ret = store_reset_ind(p_reset_hdl, (int)modem_sup->max_occurency,
                                  (int)modem_sup->time_reset_window);

            if (ret) {
                modem_reboot_valid = 0;
                break;
            }

            mstate = MODEM_DEBUG;
        case MODEM_DEBUG:
            if (modem_sup->dump_core) {
                ret = modem_debug(modem_sup);
            }
            mstate = MODEM_STOP;

            if(!modem_sup->try_restart) {
                ALOGI("Restart set to %d so reboot APE\n", modem_sup->try_restart);
                break;
            }
        case MODEM_STOP:
            ret = modem_stop();

            if (ret) {
                ALOGE(" ****** Modem Stop error =%d\n", ret);
                mstate = MODEM_ERR;
                break;
            }

            mstate = MODEM_LOAD;
        case MODEM_LOAD:
            ret = modem_load(modem_sup);

            if (ret) {
                ALOGE(" ****** Modem Load error =%d\n", ret);
                mstate = MODEM_ERR;
                break;
            }

            mstate = MODEM_START;
        case MODEM_START:
            ret = modem_start(modem_sup);

            if (ret) {
                ALOGE(" ****** Modem Start error =%d\n", ret);
                mstate = MODEM_ERR;
                break;
            }

            mstate = MODEM_CATCH_EVT;

            fd = open(ACQUIRE_WAKE_UNLOCK_PATH, O_RDWR);
            if (fd < 0) {
                ALOGE("Cannot open %s", ACQUIRE_WAKE_UNLOCK_PATH);
            } else {
                write(fd, WAKE_LOCK_STRING, sizeof(WAKE_LOCK_STRING) - 1);
                close(fd);
            }

            break;

        case MODEM_ERR:
            ALOGE("err: modem failed, previous state %d\n", mstate);
            mstate = MODEM_STOP;
            /* Reset the force flag to prevent an endless restart loop in case of error */
            modem_sup->force_restart = 0;

            /*
             * Failed to restart modem so try again and increase
             * try++. If fail to many times restart APE.
             */
            try++;
            ALOGI("Modem failed to restart try:%d of %d\n", try, modem_sup->try_restart);
            break;
        }

    } while ((try < modem_sup->try_restart || modem_sup->force_restart) && modem_reboot_valid);

    if (!(modem_sup->try_restart))
        ALOGI("Modem reboot not supported, force target restart\n");
    else if (try >= modem_sup->try_restart)
        ALOGE("fatal: try to restart %d modem\n", modem_sup->try_restart);
    else
        ALOGE("err: Reset occurency too High \n");

    msup_server_close(modem_sup->msup_server_p);

    return -1;
}

uint get_abx500_id(char * abx500_id_path)
{
    int ret, id;
    FILE *abx500_id;
    char *pch;
    char *buffer;
    int size = 0;

    ALOGI("abx500 path is %s\n",abx500_id_path);

    do {
        pch = strstr((abx500_id_path+size),ABX50X_CHIP_ID_STRING);

        if(pch == NULL) {
            id = -1;
            goto error;
        }

        buffer = strndup(abx500_id_path+size,
            (pch + strlen(ABX50X_CHIP_ID_STRING)) - (abx500_id_path+size));

        if(buffer == NULL) {
            ALOGE("MSUP: Failed to allocate memory\n");
            id = -1;
            goto error;
        }

        size += (strlen(buffer) + 1);

        abx500_id = fopen(buffer, "r");
        if (!abx500_id) {
            ALOGI("abx500 id failed %s at path %s\n", strerror(errno), buffer);
        } else {
            ALOGI("Get abx50x chip ID at %s\n",buffer);
        }
        free(buffer);

    } while(!abx500_id);

    ret = fscanf(abx500_id, "0x%x", &id);

    if (ret != 1) {
        ALOGE("get abx500 id failed\n");
        id = -1;
        goto out;
    }

out:
    fclose(abx500_id);
error:
    return id;
}


void usage(int argc, char *argv[])
{
    if (argc >= 1) {
        printf("Usage: %s -f <dev firmware path>\n"
               "          -i <sys abx500 id path>,<sys abx500 id path>    -c <command> [options]\n"
               "\n"
               " --firmware-dev, -f device path of modem firmware loader\n"
               " --id, -i           sysfs path of abx500 id\n"
               " --cmd, -c          command \n"
               "\n"
               " command can be:\n"
               "      monitor          listen modem and relaunch\n"
               "      stop             stop modem\n"
               "      start            start modem\n"
               "      status           return modem status\n"
               "\n"
               " options can be:\n"
               "      --try, -t        number try to restarted modem\n"
               "      --daemonize, -d  daemonize\n"
               "      --reboot, -r     reboot target in case of fatal error\n"
               "      -w <time reset monitoring window>\n"
               "      -o <max reset occurency>\n"
               "\n"
               , argv[0]);
    }

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{

    int ret = 0;
    int daemon = 0;
    int command = CMD_NONE;
    struct modem_sup_info modem_sup;

    static const struct option options[] = {
        { "firmware-dev", required_argument, NULL, 'f' },
        { "id", required_argument, NULL, 'i' },
        { "cmd", required_argument, NULL, 'c' },
        { "try", required_argument, NULL, 't' },
        { "daemon", no_argument, NULL, 'd' },
        { "reboot", no_argument, NULL, 'r' },
        { "help", no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'V' },
        { "time-reset-window", required_argument, NULL, 'w' },
        { "reset-occurency", required_argument, NULL, 'o' },
    };

    modem_sup.try_restart = 1;
    modem_sup.fd_netlnk = -1;
    modem_sup.fd_msup = -1;
    modem_sup.target_reboot = 0;
	modem_sup.max_occurency = DEFAULT_MAX_RESET_NUMBER;
	modem_sup.time_reset_window  = DEFAULT_MIN_RESET_DELAY;
    modem_sup.force_restart = 0;
    modem_sup.dump_core = 1;

    if (argc <= 1)
        usage(argc, argv);

    for (;;) {
        int option;

        option = getopt_long(argc, argv, "f:i:c:t:w:o:drhV", options, NULL);
        ALOGI("Option  %d ", option);

        if (option == -1)
            break;

        switch (option) {
        case 'd':
            daemon = 1;
            break;
        case 'f':
            modem_sup.mloader_path = optarg;
            break;
        case 't':
            modem_sup.try_restart = strtoul(optarg, NULL, 0);
            break;
        case 'i':
            modem_sup.abx500_id_path = optarg;
            break;
        case 'c':

            if (strcmp(optarg, "monitor") == 0)
                command = CMD_MONITOR;
            else if (strcmp(optarg, "stop") == 0)
                command = CMD_STOP;
            else if (strcmp(optarg, "start") == 0)
                command = CMD_START;
            else if (strcmp(optarg, "status") == 0)
                command = CMD_STATUS;
            else
                usage(argc, argv);

            break;
        case 'o':
            modem_sup.max_occurency = strtoul(optarg, NULL, 0);
            break;
        case 'w':
            modem_sup.time_reset_window = strtoul(optarg, NULL, 0);
            break;
        case 'r':
            modem_sup.target_reboot = 1;
            break;
        default:
            usage(argc, argv);
        }
    }

    modem_sup.abx500_id = get_abx500_id(modem_sup.abx500_id_path);

    if (modem_sup.abx500_id < 0) {
        ALOGE("failed: get abx500 id\n");
        goto out;
    }

    if (daemon) {
        OPENLOG(LOG_DAEMON);
        ret = daemonize();

        if (ret) {
            ALOGE("daemonize failed ret:%d\n", ret);
            goto out;
        }
    } else
        OPENLOG(LOG_USER);


    ret = netlnk_socket_create(&modem_sup.fd_netlnk);

    if (ret < 0) {
        ALOGE("failed: netlink socket create\n");
        goto out;
    }

    ret = init_firmware_upload_handler();

    if (ret) {
        ALOGE("failed: firmware handler init\n");
        goto out;
    }

#ifdef MODEM_LOG_RELAY
    /* Init data socket with modem_log_relay */
    mlr_sup_init_sk(SK_MODEM_SUPVISOR);
#endif

    ALOGE("Max Occurrency %d \n", modem_sup.max_occurency);

    if (modem_sup.max_occurency == 0)
        modem_sup.max_occurency = DEFAULT_MAX_RESET_NUMBER;

    ALOGE("Time Reset %d \n", modem_sup.time_reset_window);

    if (modem_sup.time_reset_window == 0)
        modem_sup.time_reset_window = DEFAULT_MIN_RESET_DELAY;

    /* Init list for Modem Reset Indication Time stamp storage */
    p_reset_hdl = list_create_reset();

    if (p_reset_hdl == NULL) {
        ALOGE("failed: list for modem reset time stamp handler init\n");
        goto out;
    }

    switch (command) {

    case CMD_MONITOR:
        ret = modem_supervisor(&modem_sup);

        if ((ret < 0) && modem_sup.target_reboot)
            target_restart();
        else
            ALOGE(" Modem Supervisor Failed ret=%d target_to_restart =%d \n", \
                 ret, modem_sup.target_reboot);

        break;

    case CMD_STATUS:
        ret = modem_status(&modem_sup);
        break;

    case CMD_STOP:
        ret = modem_stop();
        break;

    case CMD_START:
        ret = modem_start(&modem_sup);
    }

out:
    return ret;
}
