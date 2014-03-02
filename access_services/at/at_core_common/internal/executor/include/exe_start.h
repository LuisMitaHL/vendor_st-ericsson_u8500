/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __exe_start_h__
#define __exe_start_h__ (1)

#include <stdint.h>

#include "exe.h"

typedef enum {
    EXE_START_INVALID = -2,
    /* Critical services listed in the order they need to be connected */
    EXE_START_CRITICAL = -1,
#ifdef EXE_USE_CN_SERVICE
    EXE_START_CN,
#endif
#ifdef EXE_USE_SIM_SERVICE
    EXE_START_SIM,
#endif
#ifdef EXE_USE_COPS_SERVICE
    EXE_START_COPS,
#endif

    /* Non-critical services listed below are connected to in any order,
     * when all critical services listed above have been connected to.
     */
    EXE_START_REST,
#ifdef EXE_USE_SMS_SERVICE
    EXE_START_SMS,
#endif
#ifdef EXE_USE_PSCC_SERVICE
    EXE_START_PSCC,
#endif
#ifdef EXE_USE_SIMPB_SERVICE
    EXE_START_SIMPB,
#endif

    /* All services above this line */
    EXE_START_LAST
} exe_start_services_t;

typedef enum {
    EXE_START_UNKNOWN,
    EXE_START_CONNECTED_WAITING,
    EXE_START_CONNECTED_READY,      /* Connected and ready to accept commands */
    EXE_START_NOT_CONNECTED
} exe_start_status_t;


/*
 * Initiate the startup functionality.
 */
bool exe_start_init(exe_t *exe_p);

/*
 * This is called to get the current connection status for a specific service.
 */
exe_start_status_t exe_start_get_status(exe_start_services_t service);

/*
 * This is called to set a connection status for a specific service.
 * If a attempt to open as session to a specific service fails this method should be called with
 * the given service and status = EXE_START_NOT_CONNECTED.
 * If a session was established then status = EXE_START_CONNECTED;
 *
 * The status will effect what exe_start will do. If status is set to EXE_START_NOT_CONNECTED
 * it will try to initiate a new session.
 *
 */
void exe_start_set_status(exe_start_services_t service, exe_start_status_t status);

#endif /* __exe_start_h__ */
