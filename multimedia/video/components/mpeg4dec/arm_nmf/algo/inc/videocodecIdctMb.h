/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#ifndef INCLUSION_GUARD_VIDEOCODECIDCTMB_H
#define INCLUSION_GUARD_VIDEOCODECIDCTMB_H

#ifdef __cplusplus
extern "C" {
#endif

	/*************************************************************************
	* Includes
	**************************************************************************/

#include "videocodecDecInternal.h"
	/*************************************************************************
	* Types, constants and external variables
	**************************************************************************/

	/*************************************************************************
	* Declarations of functions
	**************************************************************************/

	/*************************************************************************
	*
	* Name:         mp4d_IDCT_Intra
	*
	* Parameters:   Width               [IN]
	*               Block_p             [IN]
	*               Image_p             [IN/OUT]
	* 				LastCoeff    		[IN]
	*
	*
	* Returns:      void   [RET]
	*
	* Description:	idct for one intra macroblock
	*
	**************************************************************************/
#ifndef IDCT_HW
#ifdef __ARM_NEON_IDCT__
#ifdef __ARM_NEON_FAST_IDCT__
	void mp4d_IDCT_Intra_1x1(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_2x2(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_3x3(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_4x4(int Width, sint16* Block_p, PEL* Image_p);
#endif //__ARM_NEON_FAST_IDCT__
	void mp4d_IDCT_Intra_1x1(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_4x4(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra(int Width, sint16* Block_p, PEL* Image_p);
#ifdef _CB_CR_INTERLEAVE_
	void mp4d_IDCT_Intra_1x1_cbcr(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_4x4_cbcr(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_cbcr(int Width, sint16* Block_p, PEL* Image_p);
#endif //#ifdef _CB_CR_INTERLEAVE_
#else                     
#ifdef _CB_CR_INTERLEAVE_
	void mp4d_IDCT_Intra_Full(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_0(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_Full_cbcr(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_0_cbcr(int Width, sint16* Block_p, PEL* Image_p);
#else
	void mp4d_IDCT_Intra_Full(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Intra_0(int Width, sint16* Block_p, PEL* Image_p);
#endif 
#endif

	/*************************************************************************
	*
	* Name:         mp4d_IDCT_Inter
	*
	* Parameters:   Width               [IN]
	*               Block_p             [IN]
	*               Image_p             [IN/OUT]
	* 				LastCoeff			[IN]
	*
	*
	* Returns:      void   [RET]
	*
	* Description:	idct for one inter macroblock
	*
	**************************************************************************/
#ifdef __ARM_NEON_IDCT__
#ifdef __ARM_NEON_FAST_IDCT__
	void mp4d_IDCT_Inter_1x1(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_2x2(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_3x3(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_4x4(int Width, sint16* Block_p, PEL* Image_p);
#endif //__ARM_NEON_FAST_IDCT__
	void mp4d_IDCT_Inter(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_4x4(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_1x1(int Width, sint16* Block_p, PEL* Image_p);
#ifdef _CB_CR_INTERLEAVE_
	void mp4d_IDCT_Inter_cbcr(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_1x1_cbcr(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_4x4_cbcr(int Width, sint16* Block_p, PEL* Image_p);
#endif //#ifdef _CB_CR_INTERLEAVE_
#else              
#ifdef _CB_CR_INTERLEAVE_
	void mp4d_IDCT_Inter_Full(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_0(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_Full_cbcr(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_0_cbcr(int Width, sint16* Block_p, PEL* Image_p);
#else
	void mp4d_IDCT_Inter_Full(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_0(int Width, sint16* Block_p, PEL* Image_p);
#endif
#endif
#else //IDCT_HW
	void mp4d_IDCT_Intra_Full_hw(int Width, sint16* Block_p, PEL* Image_p);
	void mp4d_IDCT_Inter_Full_hw(int Width, sint16* Block_p, PEL* Image_p);
#endif  //IDCT_HW

	/*************************************************************************/

#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECIDCTMB_H
