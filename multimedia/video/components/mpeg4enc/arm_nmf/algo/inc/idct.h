/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef IDCT_H
#define IDCT_H


#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
* Includes
**************************************************************************/
#include "ast_types.h"
/*************************************************************************
* Types, constants and external variables
**************************************************************************/

/*************************************************************************
* Declarations of functions
**************************************************************************/

/*************************************************************************
*
* Name:         mp4e_IDCT_Intra
*
* Parameters:   Width               [IN]
*               Block_p             [IN]
*               Image_p             [IN/OUT]
*         LastCoeff       [IN]
*
*
* Returns:      void   [RET]
*
* Description:  idct for one intra macroblock
*
**************************************************************************/
void mp4e_IDCT_Chroma_Inter(int Width, sint16* Block_U, sint16* Block_V, PEL* Image_p);
void mp4e_IDCT_Luma_Inter(int Width, sint16* Block_p, PEL* Image_p);
void mp4e_IDCT_Chroma_Intra(int Width, sint16* Block_U, sint16* Block_V, PEL* Image_p);
void mp4e_IDCT_Luma_Intra(int Width, sint16* Block_p, PEL* Image_p);


/*************************************************************************
*
* Name:         mp4e_IDCT_Inter
*
* Parameters:   Width               [IN]
*               Block_p             [IN]
*               Image_p             [IN/OUT]
*         LastCoeff     [IN]
*
*
* Returns:      void   [RET]
*
* Description:  idct for one inter macroblock
*
**************************************************************************/

void mp4e_IDCT_Inter(int Width,
                             sint16* Block_p,
                             PEL* Image_p,
                             int LastCoeff,
                             int BlockNr,
                             int InterlaceField);
#if 1//def CHROMA_INTERLEAVE
void mp4e_IDCT_Inter_Chroma(int Width,
                             sint16* Block_p,
                             PEL* Image_p,
                             int LastCoeff,
                             int BlockNr,
                             int InterlaceField,
							 int V);
#endif
/*************************************************************************/

#ifdef __cplusplus
}
#endif



#endif
