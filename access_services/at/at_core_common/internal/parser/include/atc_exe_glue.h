/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ATC_EXE_GLUE_H_
#define ATC_EXE_GLUE_H_

#include <exe.h>

exe_t *atc_get_exe_handle(void);
void atc_setup_exe_glue(exe_t *exe_p);

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS

bool atc_exe_event_customer_extension_callback(exe_event_t event, void *response_p);
#endif

#endif /* ATC_EXE_GLUE_H_ */
