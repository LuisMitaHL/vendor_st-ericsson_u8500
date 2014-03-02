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
vector_scale_mix(Float **data_ptr, Float **result_ptr, int block_len,
				 Float *scale)
{
	Float          *data1_ptr;
	Float          *data2_ptr;
	Float          *result1_ptr;
	Float          *result2_ptr;
	int             i;
	Float           scale11, scale21, scale12, scale22;

	scale11 = *scale++;
	scale12 = *scale++;
	scale21 = *scale++;
	scale22 = *scale;

	data1_ptr = data_ptr[0];
	data2_ptr = data_ptr[1];
	result1_ptr = result_ptr[0];
	result2_ptr = result_ptr[1];

	for (i = 0; i < block_len; i++) {
		*result1_ptr++ = (*data1_ptr * scale11) + (*data2_ptr * scale21);
		*result2_ptr++ = (*data2_ptr * scale22) + (*data1_ptr * scale12);
		data1_ptr++;
		data2_ptr++;
	}

}

#else
void
vector_scale_mix(Float **data_ptr, Float **result_ptr, int block_len,
				 Float *scale)
{
#if 1
	Float          *data1_ptr;
	Float          *data2_ptr;
	Float          *result1_ptr;
	Float          *result2_ptr;
	WORD56          accu;
	WORD56          accu1;

	Float           res1, res2;
	Float           scale11, scale12, scale21, scale22;

	register Float scare2 AT_REG(R0L);
	register Float scare1 AT_REG(R0H);
	register Float data1 AT_REG(R1L);
	register Float data2 AT_REG(R1H);
	int      local_len;
	scale11 = *scale++;
	scale12 = *scale++;
	scale21 = *scale++;
	scale22 = *scale++;

	data1_ptr = data_ptr[0];
	data2_ptr = data_ptr[1];
	result1_ptr = result_ptr[0];
	result2_ptr = result_ptr[1];

	data1 = *data1_ptr++;
	scare1 = scale11;
	accu = wX_fmul(data1, scare1);

	while(block_len>0) {
		local_len = wmin(block_len, 1024);
		HWLOOP(local_len - 1) {
			data2 = *data2_ptr++;
			scare2 = scale21;
			accu1 = wX_fmul(data2, scare2);
			res1 = waddr(accu, accu1);
			*result1_ptr++ = res1;
			
			scare1 = scale12;
			accu = wX_fmul(data1, scare1);
			scare2 = scale22;
			accu1 = wX_fmul(data2, scare2);
			data1 = *data1_ptr++;
			res2 = waddr(accu, accu1);
			scare1 = scale11;
			*result2_ptr++ = res2;
			accu = wX_fmul(data1, scare1);
		} END_HWLOOP;
		data2 = *data2_ptr;
		scare2 = scale21;
		accu1 = wX_fmul(data2, scare2);
		res1 = waddr(accu, accu1);
		*result1_ptr = res1;
		
		scare1 = scale12;
		accu = wX_fmul(data1, scare1);
		scare2 = scale22;
		accu1 = wX_fmul(data2, scare2);
		res2 = waddr(accu, accu1);
		*result2_ptr = res2;

		block_len -= local_len;
	}
#else
	Float          *data1_ptr;
	Float          *data2_ptr;
	Float          *result1_ptr;
	Float          *result2_ptr;
	WORD56          accu;
	WORD56          accu1;
	WORD56          t_accu;

	Float           res1;
	
	register Float data1 AT_REG(R1L);
	register Float data2 AT_REG(R1H);
	register Float vscale AT_REG(R0H);
	
	data1_ptr = data_ptr[0];
	data2_ptr = data_ptr[1];
	result1_ptr = result_ptr[0];
	result2_ptr = result_ptr[1];

	HWLOOP ( block_len ) {

		data1  = *data1_ptr++;

		data2 = *data2_ptr++;
		
		vscale = *scale++; // scale 11

		accu  = wX_fmul(data1, vscale);
		vscale = *scale++; // scale 12
		
		accu1 = wX_fmul(data1, vscale);
		vscale = *scale++; // scale 21
		
		t_accu = wX_fmul(data2, vscale);
		vscale = *scale--; // scale 22

		res1 = waddr(accu, t_accu);
		scale --;

		*result1_ptr++ = res1;
		t_accu = wX_fmul(data2, vscale);

		res1 = waddr(accu1, t_accu);
		scale --;

		*result2_ptr++ = res1;
	} END_HWLOOP;


#endif	
	
}

void vector_scale_mix_interleaved(Float *data_ptr, Float *result_ptr, int block_len,
				 Float *scale)
{

	WORD56          accu;
	WORD56          accu1;
	Float          *data1_ptr;
	Float          *data2_ptr;
	Float           res1, res2;
	Float           scale11, scale12, scale21, scale22;

	register Float scare2 AT_REG(R0L);
	register Float scare1 AT_REG(R0H);
	register Float data1 AT_REG(R1L);
	register Float data2 AT_REG(R1H);
	int      local_len;
	scale11 = *scale++;
	scale12 = *scale++;
	scale21 = *scale++;
	scale22 = *scale++;
	data1_ptr=data_ptr;
	data2_ptr=data_ptr+1;

	data1 = *data_ptr++;
	scare1 = scale11;
	accu = wX_fmul(data1, scare1);

	while(block_len>0) {
		local_len = wmin(block_len, 1024);
		HWLOOP(local_len - 1) {
			data2 = *data2_ptr++;
			scare2 = scale21;
			accu1 = wX_fmul(data2, scare2);
			res1 = waddr(accu, accu1);
			*result_ptr++ = res1;
			
			scare1 = scale12;
			accu = wX_fmul(data1, scare1);
			scare2 = scale22;
			accu1 = wX_fmul(data2, scare2);
			data1 = *data1_ptr++;
			res2 = waddr(accu, accu1);
			scare1 = scale11;
			*result_ptr++ = res2;
			accu = wX_fmul(data1, scare1);
		} END_HWLOOP;
		data2 = *data_ptr;
		scare2 = scale21;
		accu1 = wX_fmul(data2, scare2);
		res1 = waddr(accu, accu1);
		*result_ptr = res1;
		
		scare1 = scale12;
		accu = wX_fmul(data1, scare1);
		scare2 = scale22;
		accu1 = wX_fmul(data2, scare2);
		res2 = waddr(accu, accu1);
		*result_ptr = res2;

		block_len -= local_len;
	}

	
}

#endif
