/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <string.h>

#include <bass_app.h>
#include <debug.h>
#include <tee_client_api.h>
#include <tee_ta.h>
#include <uuid.h>

#define ELF_ENTRY_OFFSET 24

int verify_start_modem(void *access_mem_start,
                       size_t shared_mem_size,
                       size_t access_private_mem_size,
                       struct access_image_descr *access_image_descr)
{
    int ret = 0;

    TEEC_Result teec_ret = TEEC_ERROR_GENERIC;
    TEEC_Context ctx;
    TEEC_SharedMemory shm;
    TEEC_ErrorOrigin org;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_UUID ta_static_uuid = STATIC_TA_UUID;

    (void)access_mem_start;
    (void)shared_mem_size;
    (void)access_private_mem_size;

    if (NULL == access_image_descr ||
            NULL == access_image_descr->elf_hdr) {
        ret = -1;
        dprintf(ERROR, "NULL pointer error");
        goto out;
    }

    teec_ret = TEEC_InitializeContext(NULL, &ctx);

    if (teec_ret != TEEC_SUCCESS) {
        goto out;
    }

    teec_ret = TEEC_OpenSession(&ctx, &session, &ta_static_uuid,
                                TEEC_LOGIN_PUBLIC, NULL, NULL, &org);

    if (teec_ret != TEEC_SUCCESS) {
        goto out_finalize;
    }

    shm.size = 4;
    shm.flags = TEEC_MEM_INPUT;
    teec_ret = TEEC_AllocateSharedMemory(&ctx, &shm);

    if (teec_ret != TEEC_SUCCESS) {
        goto out_close;
    }

    operation.memRefs[0] = shm;
    operation.flags = TEEC_MEMREF_0_USED;

    /* Point to entry. */
    *((unsigned int *)shm.buffer) =
        (uintptr_t)((uint8_t *) access_image_descr->elf_hdr +
                       ELF_ENTRY_OFFSET);

    teec_ret = TEEC_InvokeCommand(&session, BASS_APP_VERITY_START_MODEM,
                                  &operation, &org);

    if (teec_ret != TEEC_SUCCESS) {
        goto out_release;
    }

out_release:
    TEEC_ReleaseSharedMemory(&shm);

out_close:
    TEEC_CloseSession(&session);

out_finalize:
    TEEC_FinalizeContext(&ctx);

out:

    if (teec_ret != TEEC_SUCCESS) {
        ret = -1;
    }

    return ret;
}
