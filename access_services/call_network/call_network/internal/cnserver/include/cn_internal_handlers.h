/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_internal_handlers_h__
#define __cn_internal_handlers_h__ (1)

#include "cn_data_types.h"
#include "cn_log.h"
#include "request_handling.h"
#include "message_handler.h"

#ifdef ENABLE_MODULE_TEST
#define NVD_INDICATOR_PATH "/tmp/cns.nvd"
#else
#define NVD_INDICATOR_PATH "/data/misc/cns.nvd"
#endif

void set_default_non_volatile_modem_data();

#endif /* __cn_internal_handlers_h__ */
