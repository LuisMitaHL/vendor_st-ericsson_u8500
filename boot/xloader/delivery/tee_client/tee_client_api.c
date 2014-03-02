/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: martin.xm.hovang@stericsson.com
 */

#include "tee_client_api.h"
#include "security.h"
#include "isswapi.h"

#include "issw_printf.h"

TEEC_Result TEEC_InitializeContext(
    const char*   name,
    TEEC_Context* context)
{
    /* not used in OS free context */
    return TEEC_SUCCESS;
}

TEEC_Result TEEC_FinalizeContext(
    TEEC_Context* context)
{
    /* not used in OS free context */
    return TEEC_SUCCESS;
}

TEEC_Result TEEC_OpenSession(
    TEEC_Context*     context,
    TEEC_Session*     session,
    const TEEC_UUID*  destination,
    t_uint32          connectionMethod,
    void*             connectionData,
    TEEC_Operation*   operation,
    TEEC_ErrorOrigin* errorOrigin)
{
    if(errorOrigin == NULL)
    {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if(session == NULL)
    {
        *errorOrigin = TEEC_ORIGIN_API;
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if(destination != NULL)
    {
        /* Open a session towards an application inside
           the TEE (located inside the ISSW binary) */
        session->uuid = *destination;
        session->ta = NULL;
    }
    else
    {
        /* Open a session towards a TA defined by operation */
        if(operation == NULL || operation->memRefs[0].buffer == NULL)
        {
            *errorOrigin = TEEC_ORIGIN_API;
            return TEEC_ERROR_BAD_PARAMETERS;
        }
        session->ta = (void*)SECURITY_GetPhysicalAddress(
            (t_logical_address)operation->memRefs[0].buffer);
    }

    session->id = 0;
    return TEEC_SUCCESS;
}

TEEC_Result TEEC_CloseSession(
    TEEC_Session* session)
{
    void* uuid = NULL;
    TEEC_Result res;

    if(session == NULL)
    {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if(session->ta == NULL)
    {
        uuid = (void*)SECURITY_GetPhysicalAddress(
            (t_logical_address)(&(session->uuid)));
    }

    /* Clean context in secure world (hashes etc) */
    SECURITY_CallSecureService(
        ISSWAPI_CLOSE_TA,
        SEC_ROM_FORCE_CLEAN_MASK,
        session->id, uuid, session->ta,
        //session->id, 0, session->ta,
        SECURITY_GetPhysicalAddress((t_logical_address)(&res)));

    return res;
}

TEEC_Result TEEC_InvokeCommand(
    TEEC_Session*      session,
    t_uint32           commandID,
    TEEC_Operation*    operation,
    TEEC_ErrorOrigin*  errorOrigin)
{
    TEEC_Operation op;
    t_uint32 sel = 1;
    t_uint32 i = 0;
    void* uuid = NULL; /* TA UUID */
    TEEC_Result res;

    if(errorOrigin == NULL)
    {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if(operation == NULL || session == NULL || errorOrigin == NULL)
    {
        *errorOrigin = TEEC_ORIGIN_API;
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    op = *operation;

    for(; i < 4; i++)
    {
        if(sel & op.flags)
        {
            op.memRefs[i].buffer = (void*)SECURITY_GetPhysicalAddress(
                (t_logical_address)(op.memRefs[i].buffer));
        }
        sel = sel << 1;
    }

    if(session->ta == NULL)
    {
        uuid = (void*)SECURITY_GetPhysicalAddress(
            (t_logical_address)(&(session->uuid)));
    }

    SECURITY_CallSecureService(
        ISSWAPI_EXECUTE_TA,
        SEC_ROM_FORCE_CLEAN_MASK,
        SECURITY_GetPhysicalAddress((t_logical_address)(&(session->id))),
        uuid, session->ta, commandID,
        SECURITY_GetPhysicalAddress((t_logical_address)(&op)),
        SECURITY_GetPhysicalAddress((t_logical_address)(&res)),
        SECURITY_GetPhysicalAddress((t_logical_address)errorOrigin));

    if (res == TEEC_SUCCESS) {
        for (i = 0; i < 4; i++) {
            if ((1 << i) & op.flags) {
                operation->memRefs[i].size = op.memRefs[i].size;
                operation->memRefs[i].flags = op.memRefs[i].flags;
            }
        }
    }


    return res;
}

