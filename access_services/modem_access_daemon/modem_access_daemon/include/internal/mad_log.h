/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef MAD_LOG_H
#define MAD_LOG_H (1)

#include "util_log.h"

void mad_log_init();
void mad_log_close();
int mad_log_select_callback(const int fd, const void *data_p);

void mad_set_log_level(util_log_type_t log_level);
util_log_type_t mad_get_log_level();


#define  MAD_LOG_E(...) util_print_log(UTIL_LOG_MODULE_MAD, mad_get_log_level(), UTIL_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define  MAD_LOG_W(...) util_print_log(UTIL_LOG_MODULE_MAD, mad_get_log_level(), UTIL_LOG_TYPE_WARNING, __func__, __VA_ARGS__)
#define  MAD_LOG_I(...) util_print_log(UTIL_LOG_MODULE_MAD, mad_get_log_level(), UTIL_LOG_TYPE_INFO, __func__, __VA_ARGS__)
#define  MAD_LOG_D(...) util_print_log(UTIL_LOG_MODULE_MAD, mad_get_log_level(), UTIL_LOG_TYPE_DEBUG, __func__, __VA_ARGS__)
#define  MAD_LOG_V(...) util_print_log(UTIL_LOG_MODULE_MAD, mad_get_log_level(), UTIL_LOG_TYPE_VERBOSE, __func__, __VA_ARGS__)

#endif        /* #ifndef MAD_LOG_H */

