/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef _NMF_MPC_
#include <libeffects/mpc/libvector.nmf>
#endif //#ifdef _NMF_MPC_
#include "vector.h"

#ifndef MMDSP

void
vector_scale_copy_to_buffers(Float *data_ptr, Float **result_ptr,
							 int block_len, Float scale)
{
	int             i;
	Float          *result1;
	Float          *result2;
	Float           res;

	result1 = result_ptr[0];
	result2 = result_ptr[1];

	for (i = 0; i < block_len; i++) {
		res = *data_ptr++ * scale;
		*result1++ = res;
		*result2++ = res;
	}
}

#else

void
vector_scale_copy_to_buffers(Float *data_ptr, Float **result_ptr,
							 int block_len, Float scale)
{
	Float          *result1;
	Float          *result2;
	Float           res;
	Float           in_data;
	int             local_len;

	result1 = result_ptr[0];
	result2 = result_ptr[1];

	 in_data = *data_ptr++;
	 res = wfmulr(in_data, scale);
	 block_len--;
	 
	 while (block_len>0) {
		 local_len = wmin(block_len, 1023);
		 
		 HWLOOP(local_len) {
			 in_data = *data_ptr++;
			 *result1++ = res;
			 *result2++ = res;
			 res = wfmulr(in_data, scale);
		 } END_HWLOOP;
		 block_len -= local_len;
	 } 
	 *result1++ = res;
	 *result2++ = res;   
}


void
vector_scale_copy_to_buffers_interleaved(Float *data_ptr, Float *result_ptr,
							 int block_len, Float scale)
{
	
	Float           res;
	Float           in_data;
	int             local_len;

	
	 in_data = *data_ptr;
	 data_ptr+=2;
	 res = wfmulr(in_data, scale);
	 block_len--;
	 
	 while (block_len>0) {
		 local_len = wmin(block_len, 1023);
		 
		 HWLOOP(local_len) {
			 in_data = *data_ptr;
			 in_data+=2;
			 *result_ptr++ = res;
			 *result_ptr++ = res;
			 res = wfmulr(in_data, scale);
		 } END_HWLOOP;
		 block_len -= local_len;
	 } 
	 *result_ptr++ = res;
	 *result_ptr++ = res;   
}

#endif
