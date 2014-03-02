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

bass_return_code verify_signedheader(const uint8_t* const hdr,
						 enum issw_payload_type pl_type,
                         bass_vsh_t* const hinfo)
{
	TEEC_Session session;
	TEEC_Operation operation;
	TEEC_Result result;
	TEEC_Context context;
	TEEC_ErrorOrigin errorOrigin;
	uint32_t hashSize = 0;
	//TEEC_UUID destination;

	memset((void*)(&operation), 0, sizeof(TEEC_Operation));

	PRINTF("verify_signedheader\n");
	if(NULL == hinfo)
	{
		//Error
		PRINTF("NULL == hinfo\n");
		return BASS_RC_FAILURE;
	}

	//destination = ta_static_uuid;

	result = TEEC_InitializeContext(NULL, //name
								   &context);
	if(TEEC_SUCCESS != result) {
		PRINTF("TEEC_InitializeContext TEEC_SUCCESS != result\n");
		return BASS_RC_FAILURE;
	}

	result = TEEC_OpenSession(&context,
							  &session,
							  &ta_static_uuid,
							  TEEC_LOGIN_PUBLIC,
							  NULL, //connectionData
							  NULL, //operation
						      &errorOrigin);

	if(TEEC_SUCCESS != result) {
		PRINTF("TEEC_OpenSession TEEC_SUCCESS != result\n");
		return BASS_RC_FAILURE;
	}

	operation.memRefs[0].buffer = (void *)hdr;
	operation.memRefs[0].size = 0;
	operation.memRefs[0].flags = TEEC_MEM_INPUT;

	if (((bass_app_signed_header_t *)hdr)->magic == ISSW_SIGNED_HEADER_MAGIC) {
		operation.memRefs[0].size = ((bass_app_signed_header_t *)hdr)->size_of_signed_header;
	}

	operation.flags |= TEEC_MEMREF_0_USED;


	operation.memRefs[1].buffer = &pl_type;
	operation.memRefs[1].size = (size_t)(sizeof(enum issw_payload_type));
	operation.memRefs[1].flags |= TEEC_MEM_INPUT;

	operation.flags |= TEEC_MEMREF_1_USED;

	result = TEEC_InvokeCommand(&session,
								BASS_APP_VERIFY_SIGNED_HEADER_ID,
								&operation,
							    &errorOrigin);


	if(TEEC_SUCCESS != result)
	{
		PRINTF("TEEC_InvokeCommand TEEC_SUCCESS != result (%d)\n", result);
		return BASS_RC_FAILURE;
	}

	result =  TEEC_CloseSession(&session);

	if(TEEC_SUCCESS != result)
	{
		PRINTF("TEEC_CloseSession TEEC_SUCCESS != result (%d)\n", result);
		return BASS_RC_FAILURE;
	}

    result = TEEC_FinalizeContext(&context);


	if(TEEC_SUCCESS != result)
	{
		PRINTF("TEEC_FinalizeContext TEEC_SUCCESS != result\n");
		return BASS_RC_FAILURE;
	}


	if (((bass_app_signed_header_t *)hdr)->magic == ISSW_SIGNED_HEADER_MAGIC) {
		//set the output parameters
		hinfo->pl_size = ((bass_app_signed_header_t *)hdr)->size_of_payload;
		hinfo->hdr_size = ((bass_app_signed_header_t *)hdr)->
													size_of_signed_header;
		hinfo->ht = ((bass_app_signed_header_t *)hdr)->hash_type;
		hinfo->plt = ((bass_app_signed_header_t *)hdr)->payload_type;

		switch(hinfo->ht)
		{
			case BASS_APP_SHA1_HASH:
				hashSize = 20;
				break;
			case BASS_APP_SHA256_HASH:
				hashSize = 32;
				break;
			case BASS_APP_SHA384_HASH:
				hashSize = 48;
				break;
			case BASS_APP_SHA512_HASH:
				hashSize = 64;
				break;
			default:
			PRINTF("hinfo->ht default/unknown case hinfo->ht %d\n", hinfo->ht);
				 //Error
				return BASS_RC_FAILURE;
		}
		hinfo->hash_size = hashSize;

		memcpy(hinfo->ehash.Value, hdr+sizeof(bass_app_signed_header_t), hashSize);

	} else {
		hinfo->pl_size = 0;
		hinfo->hdr_size = 0;
		hinfo->ht = BASS_APP_SHA1_HASH;
		hinfo->plt = pl_type;
		hinfo->hash_size = 0;
		memset(hinfo->ehash.Value, 0, BASS_HASH_LENGTH);
	}

	PRINTF("verify_signedheader done\n");
	return BASS_RC_SUCCESS;
}


