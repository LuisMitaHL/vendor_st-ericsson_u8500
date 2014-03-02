/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _DSPLIB_TOOLS_H_
#define _DSPLIB_TOOLS_H_

typedef enum DSPLIB_STATUS{		
	DSPLIB_PASS,
	DSPLIB_FAIL,	
	DSPLIB_OTHERS
}DSPLIB_STATUS;


/* 
    
	Test two 16 Bit Buffers for Relative Error and Absolute error. 
	Test Fails if Relative Error > maxRelDiff and Absolute error > 3

    pBufferRef: i/p : pointer to 16 bit Reference buffer 
	pBufferLib: i/p : pointer to 16 bit Library buffer
	maxRelDiff : i/p : user defined Maximum Allowed % Relative Difference
	BuffSize  : i/p : size of buffers
	frame     : i/p : frame number being processed

	return : enum DSPLIB_STATUS

*/

extern DSPLIB_STATUS Test_RelDiff_16Bit_Buffers(short *pBufferRef, short *pBufferLib, float maxRelDiff,long BuffSize, long frame);

/* 

	Test two 16 Bit Buffers for Max Bit Diff
	Test Fails if Absolute error > (1<<maxBitDiff)

    pBufferRef: i/p : pointer to 16 bit Reference buffer 
	pBufferLib: i/p : pointer to 16 bit Library buffer 
	maxBitDiff : i/p : user defined Maximum Allowed Bit Difference
	BuffSize  : i/p : size of buffers
	frame     : i/p : frame number being processed
	

	return : enum DSPLIB_STATUS

*/

extern DSPLIB_STATUS Test_BitDiff_16Bit_Buffers(short *pBufferRef, short *pBufferLib,  long maxBitDiff, long BuffSize, long frame);


/* 

	Test two 16 Bit Buffers for Bit Exactness. useful for testing speech related libraries
    Test Fails if samples are not Bit Exact

    pBufferRef: i/p : pointer to 16 bit Reference buffer 
	pBufferLib: i/p : pointer to 16 bit Library buffer 
	BuffSize  : i/p : size of buffers
	frame     : i/p : frame number being processed
	
	return : enum DSPLIB_STATUS

*/

extern DSPLIB_STATUS Test_BitExact_16Bit_Buffers(short *pBufferRef, short *pBufferLib, long BuffSize, long frame);

/* 

	Test two 32 Bit Buffers for Relative Error and Absolute error. 
	Test Fails if Relative Error > maxRelDiff and Absolute error > 256

    pBufferRef: i/p : pointer to 32 bit Reference buffer 
	pBufferLib: i/p : pointer to 32 bit Library buffer
	maxRelDiff : i/p : user defined Maximum Allowed % Relative Difference
	BuffSize  : i/p : size of buffers
	frame     : i/p : frame number being processed

	return : enum DSPLIB_STATUS

*/

extern DSPLIB_STATUS Test_RelDiff_32Bit_Buffers(long *pBufferRef, long *pBufferLib, float maxRelDiff, long BuffSize, long frame);


/* 

	Test two 32 Bit Buffers for Max Bit Diff
	Test Fails if Absolute error > (1<<maxBitDiff)


    pBufferRef: i/p : pointer to 32 bit Reference buffer 
	pBufferLib: i/p : pointer to 32 bit Library buffer 
	maxBitDiff: i/p : user defined Maximum Allowed Bit Difference
	BuffSize  : i/p : size of buffers
	frame     : i/p : frame number being processed


	return : enum DSPLIB_STATUS

*/

extern DSPLIB_STATUS Test_BitDiff_32Bit_Buffers(long *pBufferRef, long *pBufferLib, long maxBitDiff, long BuffSize, long frame);


/* 

	Test two 32 Bit Buffers for Bit Exactness. 
    Test Fails if samples are not Bit Exact

    pBufferRef: i/p : pointer to 32 bit Reference buffer 
	pBufferLib: i/p : pointer to 32 bit Library buffer 
	BuffSize  : i/p : size of buffers
	frame     : i/p : frame number being processed

	return : enum DSPLIB_STATUS

*/

extern DSPLIB_STATUS Test_BitExact_32Bit_Buffers(long *pBufferRef, long *pBufferLib, long BuffSize, long frame);


#endif

