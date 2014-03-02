/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cn_log.h"
#include "cn_macros.h"
#include "util_log.h"

static util_log_type_t cn_log_level = (UTIL_LOG_TYPE_INFO | UTIL_LOG_TYPE_ERROR);

void cn_set_log_level(util_log_type_t log_level)
{
    if (log_level > 0) {
        /* It should not be possible to turn of ERROR logs. */
        cn_log_level = (log_level | UTIL_LOG_TYPE_ERROR);
    }
}

util_log_type_t cn_get_log_level()
{
    /* NOTE: CN_LOG macros (ENTER/EXIT) cannot be used here since that will lead to inifinite recursion */
    return cn_log_level;
}


