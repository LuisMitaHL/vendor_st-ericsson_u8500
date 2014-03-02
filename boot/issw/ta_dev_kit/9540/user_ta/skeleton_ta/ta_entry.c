/**
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <tee_ta_api.h>
#include <ta_skeleton.h>

#include "skeleton.h"

/*
 * Trusted Application Entry Points
 */

/* Called each time a new instance is created */
TEE_Result TA_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

/* Called each time an instance is destroyed */
void TA_DestroyEntryPoint(void)
{
}

/* Called each time a session is opened */
TEE_Result TA_OpenSessionEntryPoint(uint32_t nParamTypes,
                TEE_Param  pParams[4], void **ppSessionContext)
{
    return TEE_SUCCESS;
}

/* Called each time a session is closed */
void TA_CloseSessionEntryPoint(void *pSessionContext)
{
}

/* Called when a command is invoked */
TEE_Result TA_InvokeCommandEntryPoint(
    void *pSessionContext, uint32_t nCommandID, uint32_t nParamTypes,
    TEE_Param pParams[4])
{
    switch (nCommandID) {
    case TA_SKELETON_CMD_1:
        return ta_entry_cmd_1(nParamTypes, pParams);
    case TA_SKELETON_CMD_2:
        return ta_entry_cmd_2(nParamTypes, pParams);
    case TA_SKELETON_CMD_3:
        return ta_entry_cmd_3(nParamTypes, pParams);
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}

