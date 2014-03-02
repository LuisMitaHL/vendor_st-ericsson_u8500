/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : cat_internal.h
 * Description     : Internal definitions.
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */

#ifndef __cat_internal_h__
#define __cat_internal_h__ (1)

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>


#define STE_CAT_RANGE_MASK                      (0xff00)
#define STE_CAT_RANGE_REQ                       (0x1000)
#define STE_CAT_RANGE_RSP                       (0x1100)
#define STE_CAT_RANGE_IND                       (0x1200)

// request
typedef enum {
    STE_CAT_REQ_BASE = (STE_CAT_RANGE_REQ | 0x0001),
    STE_CAT_REQ_REGISTER,
    STE_CAT_REQ_DEREGISTER,
    STE_CAT_REQ_SIM_PC_DISPLAYTEXT,
    STE_CAT_REQ_SIM_PC_SETUPMENU,
    STE_CAT_REQ_TR,
    STE_CAT_REQ_EC,
    STE_CAT_REQ_GET_TERMINAL_PROFILE,
    STE_CAT_REQ_SET_TERMINAL_PROFILE,
    STE_CAT_REQ_RAW_APDU,
    STE_CAT_REQ_CALL_CONTROL,
    STE_CAT_REQ_GET_CAT_STATUS,
    STE_CAT_REQ_ANSWER_CALL,
    STE_CAT_REQ_EVENT_DOWNLOAD,
    STE_CAT_REQ_SMS_CONTROL,
    STE_CAT_REQ_ENABLE,
} ste_cat_req_t;

// response
typedef enum {
    STE_CAT_RSP_BASE = (STE_CAT_RANGE_RSP | 0x0001),
    STE_CAT_RSP_REGISTER,
    STE_CAT_RSP_DEREGISTER,
    STE_CAT_RSP_SIM_PC_DISPLAYTEXT,
    STE_CAT_RSP_SIM_PC_SETUPMENU,
    STE_CAT_RSP_TR,
    STE_CAT_RSP_EC,
    STE_CAT_RSP_GET_TERMINAL_PROFILE,
    STE_CAT_RSP_SET_TERMINAL_PROFILE,
    STE_CAT_RSP_RAW_APDU,
    STE_CAT_RSP_CALL_CONTROL,
    STE_CAT_RSP_GET_CAT_STATUS,
    STE_CAT_RSP_ANSWER_CALL,
    STE_CAT_RSP_EVENT_DOWNLOAD,
    STE_CAT_RSP_SMS_CONTROL,
    STE_CAT_RSP_ENABLE,
} ste_cat_rsp_t;

// indications
typedef enum {
    STE_CAT_IND_PC = (STE_CAT_RANGE_IND | 0x0001),
    STE_CAT_IND_PC_NOTIFICATION,
    STE_CAT_IND_CAT_STATUS,
    STE_CAT_IND_PC_SETUP_CALL,
    STE_CAT_IND_PC_REFRESH,
    STE_CAT_IND_PC_REFRESH_FILE,
    STE_CAT_IND_SESSION_END,
    STE_CAT_IND_CAT_INFO,
    STE_CAT_IND_RAT_SETTING_UPDATED
} ste_cat_int_t;

#endif
