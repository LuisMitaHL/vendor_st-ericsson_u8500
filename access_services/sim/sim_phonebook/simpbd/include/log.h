/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 *  Description: Log handling.
 *
 *  Author: Johan Norberg <johan.xx.norberg@stericsson.com>
 *          Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */

#ifndef __simpb_log_h__
#define __simpb_log_h__ (1)

typedef enum {
    SIMPB_LOG_TYPE_UNKNOWN,
    SIMPB_LOG_TYPE_ERROR,
    SIMPB_LOG_TYPE_DEBUG,
    SIMPB_LOG_TYPE_INFO,
    SIMPB_LOG_TYPE_VERBOSE
} simpb_log_type_t;

#ifdef CFG_USE_ANDROID_LOG

#ifndef CFG_ANDROID_LOG_TAG
#define CFG_ANDROID_LOG_TAG "RIL SIMPB"
#endif

/* In Android use their logging*/
#include <utils/Log.h>

#endif /* CFG_USE_ANDROID_LOG */

void simpb_print_log(simpb_log_type_t log_type, const char *func_name_p, const char *format_p, ...);
#define  SIMPB_LOG_E(...) simpb_print_log(SIMPB_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define  SIMPB_LOG_D(...) simpb_print_log(SIMPB_LOG_TYPE_DEBUG, __func__, __VA_ARGS__)
#define  SIMPB_LOG_I(...) simpb_print_log(SIMPB_LOG_TYPE_INFO, __func__, __VA_ARGS__)
#define  SIMPB_LOG_V(...) simpb_print_log(SIMPB_LOG_TYPE_VERBOSE, __func__, __VA_ARGS__)

#endif /* __simpb_log_h__ */
