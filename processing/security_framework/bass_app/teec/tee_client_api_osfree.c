/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdbool.h>

#include <debug.h>
#include <tee_client_api.h>
#include <teec_hcl.h>

TEEC_Result TEEC_InitializeContext(const char   *name, TEEC_Context *context)
{
    (void)name;
    (void)context;
    /* Not used in OS free context. */
    dprintf(INFO, "TEEC_InitializeContext done\n");
    return TEEC_SUCCESS;
}

TEEC_Result TEEC_FinalizeContext(TEEC_Context *context)
{
    (void)context;
    /* Not used in OS free context. */
    dprintf(INFO, "TEEC_Result TEEC_FinalizeContext done\n");
    return TEEC_SUCCESS;
}

TEEC_Result TEEC_OpenSession(TEEC_Context     *context,
                             TEEC_Session     *session,
                             const TEEC_UUID  *destination,
                             t_uint32          connectionMethod,
                             void             *connectionData,
                             TEEC_Operation   *operation,
                             TEEC_ErrorOrigin *errorOrigin)
{
    TEEC_Result res = TEEC_SUCCESS;
    (void)context;
    (void)connectionMethod;
    (void)connectionData;
    t_bass_app_physical_address physical_addr = 0;

    if (session == NULL) {
        *errorOrigin = TEEC_ORIGIN_API;
        dprintf(ERROR, "session == NULL\n");
        res = TEEC_ERROR_BAD_PARAMETERS;
        goto exit;
    }

    TEEC_Init();

    if (destination != NULL) {
        /*
         * Open a session towards an application inside the TEE (located inside
         * the ISSW binary).
        */
        session->uuid = *destination;
        session->ta = NULL;
    } else {
        /* Open a session towards a TA defined by operation */
        if (operation == NULL || operation->memRefs[0].buffer == NULL) {
            *errorOrigin = TEEC_ORIGIN_API;
            return TEEC_ERROR_BAD_PARAMETERS;
        }

        physical_addr = SECURITY_GetPhysicalAddress(
                    (t_bass_app_logical_address)operation->memRefs[0].buffer);
        session->ta = (void *)physical_addr;
    }

    session->id = 0;

exit:
    return res;
}

static void TEEC_CleanSecureWorldContext(TEEC_Session *session, void *uuid,
                                         TEEC_Result *res)
{
    t_bass_app_logical_address res_la = (t_bass_app_logical_address)res;
    t_bass_app_physical_address res_pa;

    res_pa = SECURITY_GetPhysicalAddress(res_la);

    /* Clean context in secure world (hashes etc) */
    SECURITY_CallSecureService(BASS_APP_ISSWAPI_CLOSE_TA,
                               SEC_ROM_FORCE_CLEAN_MASK,
                               session->id,
                               uuid,
                               session->ta,
                               res_pa);

}

TEEC_Result TEEC_CloseSession(TEEC_Session *session)
{
    void *uuid = NULL;
    TEEC_Result res = TEEC_SUCCESS;
    t_bass_app_physical_address physical_addr = 0;

    if (session == NULL) {
        dprintf(ERROR, "TEEC_CloseSession session == NULL\n");
        res = TEEC_ERROR_BAD_PARAMETERS;
        goto exit;
    }

    if (session->ta == NULL) {
        t_bass_app_logical_address address =
            ((t_bass_app_logical_address)(&(session->uuid)));
        physical_addr = SECURITY_GetPhysicalAddress(address);
        uuid = (void *)physical_addr;
    }

    TEEC_CleanSecureWorldContext(session, uuid, &res);

exit:
    return res;
}

static bool TEEC_Invoke_Parameters_Are_Valid(TEEC_Session *session,
        t_uint32 commandID,
        TEEC_Operation *operation,
        TEEC_ErrorOrigin *errorOrigin)
{
    bool result = false;
    (void)commandID;

    if (operation == NULL) {
        dprintf(ERROR, "operation == NULL\n");
        goto function_exit;
    }

    if (session == NULL) {
        dprintf(ERROR, "session == NULL\n");
        goto function_exit;
    }

    if (errorOrigin == NULL) {
        dprintf(ERROR, "errorOrigin == NULL\n");
        goto function_exit;
    }

    result = true;

function_exit:
    return result;
}

TEEC_Result TEEC_InvokeCommand(TEEC_Session *session,
                               t_uint32 commandID,
                               TEEC_Operation *operation,
                               TEEC_ErrorOrigin *errorOrigin)
{
    TEEC_Operation op;
    t_uint32 sel = 1;  /* Magic number? */
    t_uint32 i = 0;
    void *uuid = NULL; /* TA UUID */
    TEEC_Result res = TEEC_SUCCESS;
    t_bass_app_physical_address physical_addr = 0;

    if (!TEEC_Invoke_Parameters_Are_Valid(session, commandID, operation,
                                          errorOrigin)) {
        *errorOrigin = TEEC_ORIGIN_API;
        res = TEEC_ERROR_BAD_PARAMETERS;
        goto exit;
    }

    op = *operation;

    dprintf(INFO, "op.flags : %u\n", (unsigned int)op.flags);

    /*
     * For each of the memRef, assign the physical address to buffer if it is
     * used.
     */
    for (i = 0; i < 4; i++) {
        if (sel & op.flags) {
            /* Assign the physical address to buffer. */

            physical_addr = SECURITY_GetPhysicalAddress(
                           (t_bass_app_logical_address)(op.memRefs[i].buffer));
            op.memRefs[i].buffer = (void *)physical_addr;
        }

        sel = sel << 1;
    }

    if (session->ta == NULL) {
        physical_addr = SECURITY_GetPhysicalAddress(
                   (t_bass_app_logical_address)(&(session->uuid)));
        uuid = (void *)physical_addr;
    }

    SECURITY_CallSecureService(
        BASS_APP_ISSWAPI_EXECUTE_TA,
        SEC_ROM_FORCE_CLEAN_MASK,
        SECURITY_GetPhysicalAddress(
            (t_bass_app_logical_address)(&(session->id))),
        uuid,
        session->ta,
        commandID,
        SECURITY_GetPhysicalAddress((t_bass_app_logical_address)(&op)),
        SECURITY_GetPhysicalAddress((t_bass_app_logical_address)(&res)),
        SECURITY_GetPhysicalAddress((t_bass_app_logical_address)errorOrigin));

    dprintf(INFO, "SECURITY_CallSecureService done: result = %u\n",
              (unsigned int) res);

    operation->memRefs[0].flags = op.memRefs[0].flags;
    operation->memRefs[1].flags = op.memRefs[1].flags;
    operation->memRefs[2].flags = op.memRefs[2].flags;
    operation->memRefs[3].flags = op.memRefs[3].flags;

exit:
    return res;
}

