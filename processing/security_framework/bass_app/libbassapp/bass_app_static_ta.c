/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>
#include <stdint.h>

#include <bass_app.h>
#include <debug.h>
#include <teec_handler.h>
#include <tee_client_api.h>
#include <uuid.h>

bass_return_code bass_set_drm_state(bool drm_state_locked)
{
    TEEC_Operation operation;

    memset(&operation, 0, sizeof(TEEC_Operation));

    operation.memRefs[0].buffer = &drm_state_locked;
    operation.memRefs[0].size = sizeof(bool);
    operation.memRefs[0].flags = TEEC_MEM_INPUT;

    operation.flags = TEEC_MEMREF_0_USED;

    return teec_invoke_secworld(&operation, BASS_APP_SET_DRM_STATE);
}

bass_return_code bass_auth_dnt_magic(void *inbuffer, size_t inbuffer_size)
{
	TEEC_Operation operation;
	TEEC_Result return_result = TEEC_SUCCESS;
	bass_return_code bass_result = BASS_RC_SUCCESS;

	memset(&operation, 0, sizeof(TEEC_Operation));

	/* We have both input and output */
	operation.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;
	/* Settings for input */
	operation.memRefs[0].buffer = inbuffer;
	operation.memRefs[0].size = inbuffer_size;
	operation.memRefs[0].flags = TEEC_MEM_INPUT;
	/* Settings for output (want a TEEC_Result) */
	operation.memRefs[1].buffer = &return_result;
	operation.memRefs[1].size = sizeof(TEEC_Result);
	operation.memRefs[1].flags = TEEC_MEM_OUTPUT;

	bass_result = teec_invoke_secworld(&operation,
					BASS_APP_AUTH_DNT_MAGIC);

	/* If the result was a failure we need to report it */
	if (BASS_RC_SUCCESS != bass_result) {
		dprintf(ERROR, "bass_auth_dnt_magic failed\n");
		bass_result = BASS_RC_FAILURE;
	}

	if (TEEC_SUCCESS != return_result) {
		dprintf(INFO, "Incorrect magic\n");
	}

	return bass_result;
}

