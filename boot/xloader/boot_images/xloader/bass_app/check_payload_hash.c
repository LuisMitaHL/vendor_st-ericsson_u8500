/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <bass_app.h>
#include <tee_client_api.h>
#include "bass_app_intern.h"
#include "crtmem.h"
bass_return_code bass_check_payload_hash(bass_hash_type_t* hashtype,
					bass_payload_type_t* payloadtype,
					void* payload,
					size_t payload_size,
					void* ehash,
					size_t ehash_size)
{
	TEEC_Operation operation;
	t_uint32 buf[2];

	buf[0] = (t_uint32)*hashtype;
	buf[1] = (t_uint32)*payloadtype;

	memset((void*)(&operation), 0, sizeof(TEEC_Operation));

	/* We have only input, but uses all three struct-parts */
	operation.flags = TEEC_MEMREF_0_USED |
			TEEC_MEMREF_1_USED |
			TEEC_MEMREF_2_USED;
	/*
	 * Settings for input
	 * The first buffer takes two t_uint32 sequentially,
	 * the first is hash-type, the second payload-type.
	 * The size represents this.
	 */
	operation.memRefs[0].buffer = buf;
	operation.memRefs[0].size = 2 * sizeof(t_uint32);
	operation.memRefs[0].flags = TEEC_MEM_INPUT;

	operation.memRefs[1].buffer = payload;
	operation.memRefs[1].size = payload_size;
	operation.memRefs[1].flags = TEEC_MEM_INPUT;

	operation.memRefs[2].buffer = ehash;
	operation.memRefs[2].size = ehash_size;
	operation.memRefs[2].flags = TEEC_MEM_INPUT;

	return teec_invoke_secworld(&operation, BASS_APP_CHECK_PAYLOAD_HASH);
}