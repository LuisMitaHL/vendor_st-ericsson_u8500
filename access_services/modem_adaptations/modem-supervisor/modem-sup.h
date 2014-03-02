/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef modem_sup_h
#define modem_sup_h

#define MLR_TIMEOUT_SEC  60
#define MODEM_READY_TIMEOUT_SEC  10
#define MLR_TIMEOUT_SYNCHRO  30

#define DEFAULT_MAX_RESET_NUMBER 3
#define DEFAULT_MIN_RESET_DELAY 100
#define TEST_RESET_LIST 10

#define ACQUIRE_WAKE_LOCK_PATH "/sys/power/wake_lock"
#define ACQUIRE_WAKE_UNLOCK_PATH "/sys/power/wake_unlock"
#define WAKE_LOCK_STRING "msup_msr"
#define ABX50X_CHIP_ID_STRING "chip_id"

enum {
    CMD_MONITOR = 0,
    CMD_STOP,
    CMD_START,
    CMD_STATUS,
    CMD_NONE
};

enum {
    MODEM_CATCH_EVT = 0,
    MODEM_DEBUG,
    MODEM_STOP,
    MODEM_LOAD,
    MODEM_START,
    MODEM_ERR,
};

/* Modem-supervisor error messages */
enum {
    SUCCESS = 0
    , MLOADER_DEVICE_ERROR
    , MLOADER_REQ_ERROR
    , MLOADER_WRITE_ERROR
    , MLR_SOCKET_OPEN_ERROR
    , MLR_SOCKET_WR_ERROR
    , MLR_SOCKET_RD_ERROR
    , MLR_SOCKET_TIMEOUT
    , BAM_ERROR
    , MODEM_SUP_IDLE_ERROR
    , MODEM_UNLOCK_FOR_DEBUG_ERROR
    , MODEM_UNLOCK_FOR_RELOAD_ERROR
    , MODEM_FMW_INTEGRITY_ERROR
    , MODEM_SUP_RESTART_ERROR
    , FAILED = 100
};

struct modem_sup_info {
    char *mloader_path;
    char *abx500_id_path;
    void *msup_server_p;
    int fd_netlnk;
    int fd_msup;

    int try_restart;
    int abx500_id;
    int max_occurency;
    int time_reset_window;
    int mlr_socket_fd;
    int target_reboot;
    int force_restart;
    int dump_core;
};

#endif /* modem_sup_h */
