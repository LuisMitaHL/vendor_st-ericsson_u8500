/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_log_h__
#define __cn_log_h__ (1)

#include "cn_general.h"
#include "util_log.h"

void cn_log_init();
void cn_log_close();
int cn_log_select_callback(const int fd, const void *data_p);
void cn_set_log_level(util_log_type_t log_level);
util_log_type_t cn_get_log_level();

#define  CN_LOG_E(...) util_print_log(UTIL_LOG_MODULE_CN, cn_get_log_level(), UTIL_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define  CN_LOG_W(...) util_print_log(UTIL_LOG_MODULE_CN, cn_get_log_level(), UTIL_LOG_TYPE_WARNING, __func__, __VA_ARGS__)
#define  CN_LOG_I(...) util_print_log(UTIL_LOG_MODULE_CN, cn_get_log_level(), UTIL_LOG_TYPE_INFO, __func__, __VA_ARGS__)
#define  CN_LOG_D(...) util_print_log(UTIL_LOG_MODULE_CN, cn_get_log_level(), UTIL_LOG_TYPE_DEBUG, __func__, __VA_ARGS__)
#define  CN_LOG_V(...) util_print_log(UTIL_LOG_MODULE_CN, cn_get_log_level(), UTIL_LOG_TYPE_VERBOSE, __func__, __VA_ARGS__)

#endif /* __cn_log_h__ */
