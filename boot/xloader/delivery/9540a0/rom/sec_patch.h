/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef SEC_PATCHES_H
#define SEC_PATCHES_H

#include <stdarg.h>

  /* Constants */

/* Patch ID's. Note that these ID's shall f i t into one byte */


/* DMA patches */
#define SEC_PATCH_DMA_INIT                        1
#define SEC_PATCH_DMA_PROGRAM_TRANSFER            2
#define SEC_PATCH_DMA_TRANSFER_RUNNING            3
#define SEC_PATCH_DMA_COMPUTE_LOG_ELEMENT_REG     4 /* NU */
#define SEC_PATCH_DMA_FIQ_HANDLER                 5
#define SEC_PATCH_DMA_COMPUTE_EVEN_PARAM_REG      6
#define SEC_PATCH_DMA_COMPUTE_ODD_PARAM_REG       7
#define SEC_PATCH_DMA_CREATE_LLI                  8 /* NU */
#define SEC_PATCH_DMA_SECURIZE_EVENT_LINE         9
#define SEC_PATCH_HW_HASH_DMA_INIT               10
#define SEC_PATCH_HW_HASH_DMA_UPDATE             11
#define SEC_PATCH_HW_HASH_DMA_FINAL              12

#define SEC_PATCH_L2CC_ENABLE                    13
#define SEC_PATCH_L2CC_DISABLE                   14

#define SEC_PATCH_PKA_HW_SET_KEY                 15
#define SEC_PATCH_PKA_HW_COMPUTE                 16
#define SEC_PATCH_PKA_HW_SAVE                    17
#define SEC_PATCH_PKA_HW_RESTORE                 18
#define SEC_PATCH_PKA_HW_RESUME	                 19

#define SEC_PATCH_HW_RNG_RANDOM_DATA_GET         20
#define SEC_PATCH_RANDOM_DATA_GET                21
#define SEC_PATCH_RANDOM_GEN_INIT                22

#define SEC_PATCH_TZPC_HW_CONFIG_SEC_MEM         23
#define SEC_PATCH_TZPC_HW_READ_MEM_CONFIG        24
#define SEC_PATCH_TZPC_HW_CONFIG_SEC_API         25
#define SEC_PATCH_TZPC_HW_READ_API_CONFIG        26

#define SEC_PATCH_SHA512_INIT                    27

#define SEC_PATCH_IS_NON_SEC_ADDRESS             28
#define SEC_PATCH_IS_NON_SEC_AREA                29

#define SEC_PATCH_INITIALIZE_COUNTER             30
#define SEC_PATCH_GET_COUNTER_TIME               31

#define SEC_PATCH_PROCESS_TIMER_IT               32

#define SEC_PATCH_CRYPTO_ACCEL_CONFIG            33
#define SEC_PATCH_CRYPTO_ACCEL_ENTRY             34
#define SEC_PATCH_CRYPTO_ACCEL_EXIT              35

#define SEC_PATCH_DEEP_ENTRY                     36
#define SEC_PATCH_DEEP_PERSONAL_CONTEXT_SAVE     37
#define SEC_PATCH_DEEP_COMMON_CONTEXT_SAVE       38
#define SEC_PATCH_DEEP_SLEEP_GO                  39
#define SEC_PATCH_DEEP_DMA_SAVE                  40
#define SEC_PATCH_DEEP_DMA_RESTORE               41
#define SEC_PATCH_DEEP_ADDITIONNAL_SAVE          42
#define SEC_PATCH_DEEP_ADDITIONNAL_RESTORE       43
#define SEC_PATCH_DEEP_SAVE_MEMORY_PAGE          44
#define SEC_PATCH_DEEP_SAVE_MEMORY               45
#define SEC_PATCH_DEEP_EXIT_SLAVE                46
#define SEC_PATCH_DEEP_EXIT_MASTER               47

#define SEC_PATCH_NON_VALID_ADDRESS              48

#define SEC_PATCH_PA2VA                          49
#define SEC_PATCH_ENABLE_SEC_MODE                50
#define SEC_PATCH_DISABLE_SEC_MODE               51

#define SEC_PATCH_EVENT_WAIT                     52
#define SEC_PATCH_EVENT_SYNCHRONIZE              53

#define SEC_PATCH_DMA_INIT_LLI_AREA              54
#define SEC_PATCH_DMA_ALLOCATE_LLI               55
#define SEC_PATCH_DMA_FILL_LLI                   56
#define SEC_PATCH_DMA_FREE_LLI                   57
#define SEC_PATCH_DMA_FREE_LLI_ENTRY             58

/* Used only in assembly file ! */
#define SEC_PATCH_PROCESS_UNKNOWN_IT             59

#define SEC_PATCH_XP70_WRITE_MAILBOX             60
#define SEC_PATCH_XP70_WAIT_MAILBOX              61
#define SEC_PATCH_XP70_CLEAR_IT                  62

#define SEC_PATCH_L2CC_MAINTENANCE_BY_AREA       63

#define SEC_PATCH_DEEP_REGISTER_STACK_AREA       64

#define SEC_PATCH_XP70_NOTIFY_SEC_STATUS         65

#define SEC_PATCH_IT_WAIT                        66
#define SEC_PATCH_IT_SYNCHRONIZE                 67
#define SEC_PATCH_SLAVE_CPU_WAKE_UP              68
#define SEC_PATCH_IT_SYNC_EXECUTION              69

#define SEC_PATCH_HW_CRYP_INIT                   70
#define SEC_PATCH_HW_CRYP_UPDATE                 71
#define SEC_PATCH_HW_CRYP_UPDATE_BASIC           72
#define SEC_PATCH_HW_CRYP_UPDATE_XTS             73

#define SEC_PATCH_DEEP_ENCRYPT_PAGE              74
#define SEC_PATCH_DEEP_DECRYPT_PAGE              75

#define SEC_PATCH_FIQ_SGI_HANDLE                 76

#define SEC_PATCH_DMA_SET_DMA_REGS               77

#define SEC_PATCH_XP70_WRITE_DMA_HASH_ONGOING    78
#define SEC_PATCH_XP70_WRITE_DMA_CRYP_ONGOING    79
#define SEC_PATCH_XP70_WRITE_SEC_MEM_SAVED       80

#define SEC_PATCH_DMA_RESTORE_CONFIG             81

/* Max patch ID must be less or equal to 255 */

#endif /* End of sec_patch.h */
