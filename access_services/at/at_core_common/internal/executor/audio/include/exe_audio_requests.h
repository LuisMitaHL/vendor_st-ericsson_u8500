/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_AUDIO_REQUESTS_H
#define EXE_AUDIO_REQUESTS_H 1

#include "exe_request_record.h"

exe_request_result_t request_mute_set(exe_request_record_t *record_p);
exe_request_result_t request_mute_read(exe_request_record_t *record_p);
exe_request_result_t request_set_tty_mode(exe_request_record_t *record_p);
exe_request_result_t request_get_tty_mode(exe_request_record_t *record_p);

#endif
