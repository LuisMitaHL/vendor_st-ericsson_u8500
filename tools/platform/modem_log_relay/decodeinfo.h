/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef DECODEINFO_H
#define DECODEINFO_H

#include <stdint.h>


enum ID {
    MODEM_INFO = 2, ERROR_INFO = 3, OS_INFO = 7, ABORT_DATA = 8,
    ASSERT_INFO = 11, TASK_SWAP_HISTORY = 13
};

/* OS info section */
#define OS_INFO_SECTION__TASK        "enum OS_TASK_NAME"
#define OS_INFO_SECTION__FUNCTION_ID "enum MONITOR_HOOK_IDS"
#define OS_INFO_SECTION__STATUS      "enum OS_RESPONSES"

typedef struct {
    uint8_t wgm_a;
    uint8_t wgm_b;
    uint8_t host_rev;
    uint8_t host;
} hw_version;

/*
 * Defines modem info data structure
 */
typedef struct {
    uint8_t cpu_id;
    uint8_t reset_reason;
    uint16_t filler;
    uint32_t system_time;
    char sw_version_data[64];
    hw_version hw_vers;
    char build_id[64];
    char company_id[64];
} modem_info;

/*
 * Defines assert info data structure
 */
#define ASSERT_INFO_SRC_NAME_SIZE 20
typedef struct {
    char source_name[ASSERT_INFO_SRC_NAME_SIZE];
    uint32_t line_number;
} assert_info;

/*
 * RIL reset information.
 */
typedef struct {
    uint8_t reason;
    assert_info assert;
} ril_reset_info_t;

typedef struct {
    uint32_t time;
    uint8_t event;
    uint8_t id;
    uint8_t priority;
    uint8_t state;
} swap_info;

/*
 * Defines task swap history data structure
 */
typedef struct {
    uint32_t index;
    swap_info info[128];
} task_swap_history;

/*
 * Defines error info data structure
 */
typedef struct {
    uint32_t x_error_code;
    uint32_t x_error_info;
} error_info;

/*
 * Defines Last OS info data structure
 */
typedef struct {
    uint8_t error_info_task;
    uint8_t error_info_function_id;
    uint8_t error_info_status_code;
    uint8_t error_info_filler;
    int32_t error_info_param1;
    int32_t error_info_param2;
    int32_t error_info_param3;
    uint8_t warning_info_task;
    uint8_t warning_info_function_id;
    uint8_t warning_info_status_code;
    uint8_t warning_info_filler;
    int32_t warning_info_param1;
    int32_t warning_info_param2;
    int32_t warning_info_param3;
    uint8_t info_info_task;
    uint8_t info_info_function_id;
    uint8_t info_info_status_code;
    uint8_t info_info_filler;
    int32_t info_info_param1;
    int32_t info_info_param2;
    int32_t info_info_param3;
} os_info;

/*
 * Defines Abort data info data structure
 */
typedef struct {
    uint32_t r8_fiq_mode;
    uint32_t r9_fiq_mode;
    uint32_t r10_fiq_mode;
    uint32_t r11_fiq_mode;
    uint32_t r12_fiq_mode;
    uint32_t spsr_fiq_mode;
    uint32_t sp_fiq_mode;
    uint32_t lr_fiq_mode;
    uint32_t spsr_irq_mode;
    uint32_t sp_irq_mode;
    uint32_t lr_irq_mode;
    uint32_t spsr_undefined_mode;
    uint32_t sp_undefined_mode;
    uint32_t lr_undefined_mode;
    uint32_t spsr_abort_mode;
    uint32_t sp_abort_mode;
    uint32_t lr_abort_mode;
    uint32_t spsr_supervisor_mode;
    uint32_t sp_supervisor_mode;
    uint32_t lr_supervisor_mode;
    uint32_t filler;
    uint32_t cpsr;
    uint32_t sp_system_and_user_mode;
    uint32_t lr_system_and_user_mode;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t lr_current;
} abort_data;

char *endianess[] = {
    "Little endian",
    "Big endian"
};

char *reset_reason[] = {
    "HW_RESET_RESERVED", "HW_RESET_UNKNOWN",
    "HW_RESET_ASSERTION_FAILED", "HW_RESET_SW_RESET_REQUESTED",
    "HW_RESET_SECURE_VIOLATION", "HW_RESET_PRODUCTION_RESET",
    "HW_RESET_ASIC_WD_RESET", "HW_RESET_SECURE_WD_RESET",
    "HW_RESET_DIV_BY_ZERO", "HW_RESET_ALIGNMENT_ERROR",
    "HW_RESET_ABORT_PREFETCH", "HW_RESET_ABORT_DATA",
    "HW_RESET_UNDEFINED_INSTRUCTION", "HW_RESET_UNHANDLED_ABORT",
    "HW_RESET_OS_STACK_OVERFLOW", "HW_RESET_OS_FATAL_HANDLER",
    "HW_RESET_OS_WARNING_HANDLER", "HW_RESET_IPC_HOST_ERROR",
    "HW_RESET_IPC_SLAVE_ERROR", "HW_RESET_FATAL", "HW_RESET_ERROR",
    "HW_RESET_WARNING", "HW_RESET_HIDDEN"
};

char *wgm_host_rev[] = {
    "WGM_HOST_8500_REVISION_ED",
    "WGM_HOST_8500_REVISION_V1", "WGM_HOST_8500_REVISION_V2",
    "WGM_HOST_8500_REVISION_V2_1"
};

#endif



