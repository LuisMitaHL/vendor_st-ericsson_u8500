/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/

#include <bass_app.h>
#include <bass_app_intern.h>
#include "tee_client_api.h"
#include "issw_printf.h"
#include "crtmem.h"

/* The static UUID */
static TEEC_UUID ta_static_uuid = {
    0xBC765EDE,
    0x6724,
    0x11DF,
    {0x8E, 0x12, 0xEC, 0xDB, 0xDF, 0xD7, 0x20, 0x85} };


bool bass_sha256_init(void** context)
{
	(void) context;

	return true;
}

bool bass_sha256_update(void** context,
			const uint8_t* const data,
			const uint32_t length)
{
	(void) context;
	(void) data;
	(void) length;

	return true;
}


bool bass_sha256_final(void** context,
		       bass_hash_t* const hash)
{
	(void) context;
	(void) hash;

	return true;
}

static bass_return_code bass_decode_teec_result(TEEC_Result result)
{
		bass_return_code rc;

		PRINTF("bass_decode_teec_result\n");
		switch (result)
		{
			case TEEC_SUCCESS:
				PRINTF("TEEC_SUCCESS -> BASS_RC_SUCCESS");
				rc = BASS_RC_SUCCESS;
				break;
			case TEEC_ERROR_GENERIC:
				PRINTF("TEEC_ERROR_GENERIC -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_ACCESS_DENIED:
				PRINTF("TEEC_ERROR_ACCESS_DENIED -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_CANCEL:
				PRINTF("TEEC_ERROR_CANCEL -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_ACCESS_CONFLICT:
				PRINTF("TEEC_ERROR_ACCESS_CONFLICT -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_EXCESS_DATA:
				PRINTF("TEEC_ERROR_EXCESS_DATA -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_BAD_FORMAT:
				PRINTF("TEEC_ERROR_BAD_FORMAT -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_BAD_PARAMETERS:
				PRINTF("TEEC_ERROR_BAD_PARAMETERS -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_BAD_STATE:
				PRINTF("TEEC_ERROR_BAD_STATE -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_ITEM_NOT_FOUND:
				PRINTF("TEEC_ERROR_ITEM_NOT_FOUND -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_NOT_IMPLEMENTED:
				PRINTF("TEEC_ERROR_NOT_IMPLEMENTED -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_NOT_SUPPORTED:
				PRINTF("TEEC_ERROR_NOT_SUPPORTED -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_NO_DATA:
				PRINTF("TEEC_ERROR_NO_DATA -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_OUT_OF_MEMORY:
				PRINTF("TEEC_ERROR_OUT_OF_MEMORY -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_BUSY:
				PRINTF("TEEC_ERROR_BUSY -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_COMMUNICATION:
				PRINTF("TEEC_ERROR_COMMUNICATION -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_SECURITY:
				PRINTF("TEEC_ERROR_SECURITY -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			case TEEC_ERROR_SHORT_BUFFER:
				PRINTF("TEEC_ERROR_SHORT_BUFFER -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
			default:
				PRINTF("Unknown TEEC_Result -> BASS_RC_ERROR_INTERNAL\n");
				rc = BASS_RC_ERROR_INTERNAL;
				break;
		}

		return rc;
}

bass_return_code bass_calcdigest(const t_bass_hash_type ht,
				 const uint8_t* data,
				 const size_t data_length,
				 bass_hash_t* const hash)
{
	TEEC_Session session;
	TEEC_Operation operation;
	TEEC_Result result;
	TEEC_Context context;
	TEEC_ErrorOrigin errorOrigin;
	//TEEC_UUID destination;
	bass_return_code rc;

	memset((void*)(&operation), 0, sizeof(TEEC_Operation));

	//destination = ta_static_uuid;
	rc = BASS_RC_SUCCESS;

	PRINTF("bass_calcdigest\n");

	result = TEEC_InitializeContext(NULL, &context);
	if (result != TEEC_SUCCESS)
	{
		PRINTF("TEEC_InitializeContext result != TEEC_SUCCESS\n");
		rc = bass_decode_teec_result(result);
		goto exit;
	}

	result = TEEC_OpenSession(&context,
							  &session,
							  &ta_static_uuid,
							  TEEC_LOGIN_PUBLIC,
							  NULL,
							  NULL,
						      &errorOrigin);

	if(result != TEEC_SUCCESS)
	{
		PRINTF("TEEC_OpenSession result != TEEC_SUCCESS\n");
		rc = BASS_RC_ERROR_INTERNAL;
		goto close;
	}


	operation.memRefs[0].buffer = (void *)(&ht);
	operation.memRefs[0].size = sizeof(t_bass_hash_type);
	operation.memRefs[0].flags = TEEC_MEM_INPUT;
	operation.flags = TEEC_MEMREF_0_USED;

	operation.memRefs[1].buffer = (void *)data;
	operation.memRefs[1].size = data_length;
	operation.memRefs[1].flags = TEEC_MEM_INPUT;
	operation.flags |= TEEC_MEMREF_1_USED;

	operation.memRefs[2].buffer = hash;
	operation.memRefs[2].size = BASS_HASH_LENGTH;
	operation.memRefs[2].flags = TEEC_MEM_OUTPUT;
	operation.flags |= TEEC_MEMREF_2_USED;

	result = TEEC_InvokeCommand(&session,
				    BASS_APP_CALC_DIGEST_ID,
				    &operation,
				    &errorOrigin);


	if (result != TEEC_SUCCESS)
	{
		PRINTF("TEEC_InvokeCommand result != TEEC_SUCCESS\n");
		rc = bass_decode_teec_result(result);
	}

close:
	result =  TEEC_CloseSession(&session);

	if(result != TEEC_SUCCESS)
	{
		PRINTF("TEEC_CloseSession result != TEEC_SUCCESS\n");
		rc = bass_decode_teec_result(result);
	}

	result = TEEC_FinalizeContext(&context);

	if (result != TEEC_SUCCESS) {
		PRINTF("TEEC_FinalizeContext result != TEEC_SUCCESS\n");
		rc = bass_decode_teec_result(result);
	}

exit:
	PRINTF("bass_calcdigest done\n");
	return rc;
}


