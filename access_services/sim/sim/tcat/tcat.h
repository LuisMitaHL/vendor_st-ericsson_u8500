/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : tcat.h
 * Description     : Header file for SIM tcat
 *
 * Author          : Teddie Stenvi <teddie.xx.stenvi@stericsson.com>
 *
 */
#ifndef TCAT_H
#define TCAT_H

#include "sim.h"

typedef enum {
    TCAT_CT_STE_SIM_CAUSE_NOOP = 1,
    TCAT_CT_STE_SIM_CAUSE_CONNECT,
    TCAT_CT_STE_SIM_CAUSE_DISCONNECT,
    TCAT_CT_STE_SIM_CAUSE_SHUTDOWN,
    TCAT_CT_STE_SIM_CAUSE_PING,
    TCAT_CT_STE_CAT_CAUSE_NOOP,
    TCAT_CT_STE_CAT_CAUSE_REGISTER,
    TCAT_CT_STE_SIM_CAUSE_STARTUP,
    TCAT_CT_STE_CAT_CAUSE_PC,
    TCAT_CT_STE_CAT_CAUSE_SIM_EC_CALL_CONTROL,
    TCAT_CT_STE_CAT_CAUSE_EC,
    TCAT_CT_STE_CAT_CAUSE_GET_CAT_STATUS,
    TCAT_CT_STE_CAT_CAUSE_GET_TERMINAL_PROFILE,
    TCAT_CT_STE_CAT_CAUSE_SET_TERMINAL_PROFILE,
    TCAT_CT_STE_SIM_CAUSE_DEREGISTER,
    TCAT_CT_STE_CAT_CAUSE_SIM_EC_SMS_CONTROL,
    TCAT_CT_STE_CAT_CAUSE_ANSWER_CALL,
    TCAT_CT_STE_CAT_CAUSE_EVENT_DOWNLOAD,
    TCAT_CT_STE_CAT_CAUSE_RAW_APDU,
    TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE,
} tcat_ct_ste_cat_cause_t;

#define TCAT_CT_STE_SIM_CAUSE_HANGUP                  0
#define TCAT_CT_UNKNOWN                               0
#define TCAT_CT_NOT_USED                              0


#endif
