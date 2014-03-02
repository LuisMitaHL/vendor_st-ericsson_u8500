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
vector_scale_add(Float **data_ptr, Float *result_ptr, int block_len,
				 Float scale1, Float scale2)
{
	Float          *data1_ptr;
	Float          *data2_ptr;
	int             i;

	data1_ptr = data_ptr[0];
	data2_ptr = data_ptr[1];

	for (i = 0; i < block_len; i++) {
		*result_ptr++ = (*data1_ptr++ * scale1) + (*data2_ptr++ * scale2);
	}

}

#else

void
vector_scale_add(Float **data_ptr, Float *result_ptr, int block_len,
				 Float scale1, Float scale2)
{
	Float          *data1_ptr;
	Float          *data2_ptr;
	WORD56          accu;
	WORD56          accu1;
	Float           data1;
	Float           data2;
	Float           res;
	int             local_len;
	
	register int scare2 AT_REG(R0L);
	register int scare1 AT_REG(R1L);

	scare1 = scale1;
	scare2 = scale2;

	data1_ptr = data_ptr[0];
	data2_ptr = data_ptr[1];

	while ( block_len > 0 ) {
		local_len = wmin(block_len, 1024);
		
		data1 = *data1_ptr++;
		accu = wX_fmul(data1, scare1);
		data2 = *data2_ptr++;
		accu1 = wX_fmul(data2, scare2);
	
		HWLOOP(local_len - 1) {
			data1 = *data1_ptr++;
			res = waddr(accu, accu1);
			accu = wX_fmul(data1, scare1);
			data2 = *data2_ptr++;
			accu1 = wX_fmul(data2, scare2);
			*result_ptr++ = res;
		} END_HWLOOP;
		*result_ptr = waddr(accu, accu1);

		block_len -= local_len;
	}
}


void
vector_scale_add_interleaved(Float *data_ptr, Float *result_ptr, int block_len,
				 Float scale1, Float scale2)
{
	
	WORD56          accu;
	WORD56          accu1;
	Float           data1;
	Float           data2;
	Float           res;
	int             local_len;
	
	register int scare2 AT_REG(R0L);
	register int scare1 AT_REG(R1L);

	scare1 = scale1;
	scare2 = scale2;

	while ( block_len > 0 ) {
		local_len = wmin(block_len, 1024);
		
		data1 = *data_ptr++;
		accu = wX_fmul(data1, scare1);
		data2 = *data_ptr++;
		accu1 = wX_fmul(data2, scare2);
	
		HWLOOP(local_len - 1) {
			data1 = *data_ptr++;
			res = waddr(accu, accu1);
			accu = wX_fmul(data1, scare1);
			data2 = *data_ptr++;
			accu1 = wX_fmul(data2, scare2);
			*result_ptr= res;
			result_ptr+=2;
		} END_HWLOOP;
		*result_ptr = waddr(accu, accu1);

		block_len -= local_len;
	}
}

#endif


