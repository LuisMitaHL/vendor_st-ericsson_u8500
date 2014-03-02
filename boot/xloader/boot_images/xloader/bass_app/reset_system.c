/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <bass_app.h>
#include <tee_client_api.h>
#include <bass_app_intern.h>
#include "issw_printf.h"
#include "crtmem.h"

/* The static UUID */
static TEEC_UUID ta_static_uuid = {
    0xBC765EDE,
    0x6724,
    0x11DF,
    {0x8E, 0x12, 0xEC, 0xDB, 0xDF, 0xD7, 0x20, 0x85} };

bass_return_code issw_api_reset(void)
{
	TEEC_Session session;
	TEEC_Operation operation;
	TEEC_Result result;
	TEEC_Context context;
	TEEC_ErrorOrigin errorOrigin;

	memset((void *)(&operation), 0, sizeof(TEEC_Operation));
	PRINTF("issw_api_reset\n");
	result = TEEC_InitializeContext(NULL, &context);
	if (TEEC_SUCCESS != result) {
		PRINTF("TEEC_InitializeContext TEEC_SUCCESS != result\n");
		return BASS_RC_FAILURE;
	}

	result = TEEC_OpenSession(&context,
				  &session,
				  &ta_static_uuid,
				  TEEC_LOGIN_PUBLIC,
				  NULL,
				  NULL,
			      	  &errorOrigin);
	if (TEEC_SUCCESS != result) {
		PRINTF("TEEC_OpenSession TEEC_SUCCESS != result\n");
		return BASS_RC_FAILURE;
	}

	result = TEEC_InvokeCommand(&session,
				BASS_APP_SYSTEM_RESET,
				&operation,
			    	&errorOrigin);
	if (TEEC_SUCCESS != result) {
		PRINTF("TEEC_InvokeCommand FAIL result (%d)\n", result);
		return BASS_RC_FAILURE;
	}

	result =  TEEC_CloseSession(&session);
	if (TEEC_SUCCESS != result) {
		PRINTF("TEEC_CloseSession FAIL result (%d)\n", result);
		return BASS_RC_FAILURE;
	}

	result = TEEC_FinalizeContext(&context);
	if (TEEC_SUCCESS != result) {
		PRINTF("TEEC_FinalizeContext TEEC_SUCCESS != result\n");
		return BASS_RC_FAILURE;
	}

	PRINTF("issw_api_reset done\n");
	return BASS_RC_SUCCESS;
}
