/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <bass_app.h>
#include <tee_client_api.h>
#include "issw_printf.h"

static TEEC_UUID ta_static_uuid = {
	0xBC765EDE,
	0x6724,
	0x11DF,
	{0x8E, 0x12, 0xEC, 0xDB, 0xDF, 0xD7, 0x20, 0x85}
};

bass_return_code teec_invoke_secworld(TEEC_Operation *const operation,
				const uint32_t command_id)
{
	bass_return_code ret_code = BASS_RC_SUCCESS;
	TEEC_Result result = TEEC_ERROR_GENERIC;
	TEEC_Session session;
	TEEC_Context context;
	TEEC_ErrorOrigin errorOrigin;

	result = TEEC_InitializeContext(NULL, &context);

	if (TEEC_SUCCESS != result) {
		PRINTF("%s(%d) TEEC_InitializeContext error = %d\n",
			__FILE__, __LINE__, result);
		ret_code = BASS_RC_FAILURE;
		goto function_exit;
	}
	result = TEEC_OpenSession(&context, &session, &ta_static_uuid,
				TEEC_LOGIN_PUBLIC, NULL, NULL, &errorOrigin);

	if (TEEC_SUCCESS != result) {
		PRINTF("%s(%d) TEEC_OpenSession error = %d"
			" errorOrigin = %d\n", __FILE__, __LINE__, result,
			errorOrigin);
		ret_code = BASS_RC_FAILURE;
		goto finalize;
	}

	result = TEEC_InvokeCommand(&session, command_id, operation,
		&errorOrigin);

	if (TEEC_SUCCESS != result) {
		PRINTF("%s(%d) TEEC_InvokeCommand error = %d"
			" errorOrigin = %d\n", __FILE__, __LINE__, result,
			errorOrigin);
		ret_code = BASS_RC_FAILURE;
		goto close;
	}

close:
	result = TEEC_CloseSession(&session);

	if (result != TEEC_SUCCESS) {
		PRINTF("%s(%d) TEEC_CloseSession error = %d\n", __FILE__,
			__LINE__, result);
		ret_code = BASS_RC_FAILURE;
	}

finalize:
	result = TEEC_FinalizeContext(&context);

	if (result != TEEC_SUCCESS) {
		PRINTF("%s(%d) TEEC_FinalizeContext error = %d\n", __FILE__,
			__LINE__, result);
		ret_code = BASS_RC_FAILURE;
	}

function_exit:
	return ret_code;
}
