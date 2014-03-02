/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 */

#ifndef USER_TA_HEADER_H
#define USER_TA_HEADER_H

#include <tee_api_types.h>

/*
   signed_header
   ta_head_t
   ta_func_head_t (1)
   ta_func_head_t (2)
   ...
   ta_func_head_t (N) N = ta_head(_t).nbr_func
   func_1
   func_1
   ...
   func_N
   hash_1
   hash_2
   ...
   hash_M
*/

struct user_ta_head {
  TEE_UUID uuid;
  uint32_t nbr_func;
  uint32_t ro_size;
  uint32_t rw_size;
  uint32_t zi_size;
  uint32_t got_size;
  uint32_t hash_type;
};

#define USER_TA_HEAD_FLAG_USER_MODE 0x80000000UL
#define USER_TA_HEAD_FLAG_DDR_EXEC  0x40000000UL

struct user_ta_func_head {
  uint32_t cmd_id;
  uint32_t start; /* offset to start func */
};

struct user_ta_sub_head {
    uint32_t flags;
    uint32_t spare;
    uint32_t heap_size;
    uint32_t stack_size;
};

#define TA_FLAG_USER_MODE           (1 << 0)
#define TA_FLAG_EXEC_DDR            (1 << 1)
#define TA_FLAG_SINGLE_INSTANCE     (1 << 2)
#define TA_FLAG_MULTI_SESSION       (1 << 3)
#define TA_FLAG_INSTANCE_KEEP_ALIVE (1 << 4)
/*
 * TEE Core will allow memrefs in some firewalled memory if this flag is
 * set for a User TA.
 */
#define TA_FLAG_UNSAFE_NW_PARAMS    (1 << 5)

enum user_ta_prop_type {
    USER_TA_PROP_TYPE_BOOL,     /* bool */
    USER_TA_PROP_TYPE_U32,      /* uint32_t */
    USER_TA_PROP_TYPE_UUID,     /* TEE_UUID */
    USER_TA_PROP_TYPE_IDENTITY, /* TEE_Identity */
    USER_TA_PROP_TYPE_STRING,   /* zero terminated string of char */
};

struct user_ta_property {
    const char *name;
    enum user_ta_prop_type type;
    const void *value;
};

extern const struct user_ta_property ta_props[];
extern const size_t ta_num_props;

/* Needed by TEE_CheckMemoryAccessRights() */
extern uint32_t ta_param_types;
extern TEE_Param ta_params[4];

#endif /*USER_TA_HEADER_H*/
