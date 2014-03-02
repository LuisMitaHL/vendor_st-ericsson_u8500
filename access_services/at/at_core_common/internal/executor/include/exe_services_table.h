/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_SERVICES_TABLE_H
#define EXE_SERVICES_TABLE_H 1

#include "exe.h"

typedef struct {
    exe_service_t       service;
    const char         *description;
} exe_service_table_t;

/* NOTE: aligned with exe_service_t */
exe_service_table_t exe_service_table[] = {
    {EXE_SERVICE_EXE,   "EXE"},
#ifdef EXE_USE_AUDIO_SERVICE
    {EXE_SERVICE_AUDIO, "AUDIO"},
#endif
#ifdef EXE_USE_CN_SERVICE
    {EXE_SERVICE_CN,    "C&N"},
#endif
#ifdef EXE_USE_COPS_SERVICE
    {EXE_SERVICE_COPS,  "COPS"},
#endif
#ifdef EXE_USE_CSPSA_SERVICE
    {EXE_SERVICE_CSPSA, "CSPSA"},
#endif
#ifdef EXE_USE_PSCC_SERVICE
    {EXE_SERVICE_PSCC,  "PSCC"},
#endif
#ifdef EXE_USE_SIM_SERVICE
    {EXE_SERVICE_SIM,   "SIM"},
#endif
#ifdef EXE_USE_SIMPB_SERVICE
    {EXE_SERVICE_SIMPB, "SIMPB"},
#endif
#ifdef EXE_USE_SMS_SERVICE
    {EXE_SERVICE_SMS,   "SMS"},
#endif
    {EXE_SERVICE_LAST,  "DUMMY"},
};

#endif
