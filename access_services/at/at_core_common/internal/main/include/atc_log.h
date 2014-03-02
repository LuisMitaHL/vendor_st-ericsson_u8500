/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ATC_LOG_H
#define ATC_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util_log.h"


#define  ATC_LOG_E(...) util_print_log(UTIL_LOG_MODULE_AT, atc_get_log_level(), UTIL_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define  ATC_LOG_W(...) util_print_log(UTIL_LOG_MODULE_AT, atc_get_log_level(), UTIL_LOG_TYPE_WARNING, __func__, __VA_ARGS__)
#define  ATC_LOG_I(...) util_print_log(UTIL_LOG_MODULE_AT, atc_get_log_level(), UTIL_LOG_TYPE_INFO, __func__, __VA_ARGS__)
#define  ATC_LOG_D(...) util_print_log(UTIL_LOG_MODULE_AT, atc_get_log_level(), UTIL_LOG_TYPE_DEBUG, __func__, __VA_ARGS__)
#define  ATC_LOG_V(...) util_print_log(UTIL_LOG_MODULE_AT, atc_get_log_level(), UTIL_LOG_TYPE_VERBOSE, __func__, __VA_ARGS__)


void atc_log_init();
void atc_log_close();
void atc_set_log_level(util_log_type_t log_level);
util_log_type_t atc_get_log_level(void);

#ifdef __cplusplus
}
#endif

#endif /* ATC_LOG_H */
