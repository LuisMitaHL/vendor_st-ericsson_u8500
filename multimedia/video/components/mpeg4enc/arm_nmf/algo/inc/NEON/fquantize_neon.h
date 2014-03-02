/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef FQUANTIZE_NEON_H_
#define FQUANTIZE_NEON_H_

/* 
   Meaning of the different labels:
   
   * H263_QUANT : the H.263 quantization (called mode 0 in the standard), without any
                  DC prediction and luma_/chroma_scaler makes it different from the 
		  normal H.263 quantization process;


   * SHDR_QUANT : the H.263 quantization process as is defined in the H.263
	 	  standard: the DC coefficient is simply divided by 8 without
		  any prediction from the surrounding DC coefficients.

*/

void mp4e_fquantize_inter_NEON(short *coeff, short *qcoeff, int QP, int shortheader);
void mp4e_fquantize_intra_NEON(short *coeff, short *qcoeff, int QP, int shortheader);

#ifdef H263_P3
t_sint32 mp4e_H263P3_FwdQuantize_NEON(t_sint32 mquant, t_sint16 *block, t_sint32 inter, mp4_parameters *mp4_par);
#endif

#endif /*FQUANTIZE_NEON_H_*/
