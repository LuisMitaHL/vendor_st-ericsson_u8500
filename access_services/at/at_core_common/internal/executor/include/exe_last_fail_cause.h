/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_LAST_FAIL_CAUSE_H_
#define EXE_LAST_FAIL_CAUSE_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    EXE_LAST_FAIL_CAUSE_CLASS_NONE = 0,
    EXE_LAST_FAIL_CAUSE_CLASS_CN_NETWORK,
    EXE_LAST_FAIL_CAUSE_CLASS_CN_SERVER,
    EXE_LAST_FAIL_CAUSE_CLASS_NET_GSM,
    EXE_LAST_FAIL_CAUSE_CLASS_NET_PSCC,
    EXE_LAST_FAIL_CAUSE_CLASS_NET_SS
} exe_last_fail_cause_class_t;

typedef struct {
    exe_last_fail_cause_class_t     class;
    int32_t                         cause;
} exe_last_fail_cause_t;


void exe_last_fail_cause_init(exe_last_fail_cause_t *last_fail_p);
void exe_last_fail_cause_set(exe_last_fail_cause_t *last_fail_info_p, exe_last_fail_cause_t *last_fail_stored_p);
bool exe_last_fail_cause_get(exe_last_fail_cause_t *last_fail_p, char *error_report_p);

#endif /* EXE_LAST_FAIL_CAUSE_H_ */
