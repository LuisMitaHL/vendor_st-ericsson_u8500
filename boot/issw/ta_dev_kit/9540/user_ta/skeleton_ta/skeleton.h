/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef SKELETON_H
#define SKELETON_H

#include <tee_api.h>

TEE_Result ta_entry_cmd_1(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_entry_cmd_2(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_entry_cmd_3(uint32_t param_types, TEE_Param params[4]);

#endif /*SKELETON_H*/
