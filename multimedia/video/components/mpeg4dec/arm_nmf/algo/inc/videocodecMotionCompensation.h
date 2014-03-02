/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECMOTIONCOMPENSATION_H
#define INCLUSION_GUARD_VIDEOCODECMOTIONCOMPENSATION_H

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
typedef void (*MC_Block8_Func) (PEL* Src_p, PEL* Dst_p, int SrcWidth, int DstWidth);
	
typedef void (*MC_Block8_Qpel_Func) (PEL* Src_p, PEL* Dst_p, int SrcWidth, int DstWidth, int rounding_control, int BlockSize8x8, int FieldPrediction);

/*************************************************************************
* Declarations of functions
**************************************************************************/
#ifndef _CB_CR_INTERLEAVE_	
extern MC_Block8_Func MC_Block8_FuncTab[8];
#else
extern MC_Block8_Func MC_Block8_FuncTab[16];
#endif
extern MC_Block8_Qpel_Func MC_Block8_Qpel_FuncTab[16];


/*************************************************************************
*
* Name:         mp4d_SetMotionParams
*
* Parameters:   LayerData_p  [IN]  
*               MC_Params_p  [OUT]
*               
*
* Returns:      Nothing
*
* Description:  Sets dst for the motion compensation. 
*
**************************************************************************/

void mp4d_SetMotionParams(LayerData_t* LayerData_p, MC_Source_t* MC_Params_p);


/*************************************************************************
*
* Name:         mp4d_SetMotionSource
*
* Parameters:   LayerData_p     [IN]  
*               MB_Data_p       [IN]
*               PredictionMode  [IN]
*               MC_Params_p     [OUT]
*
* Returns:      Nothing
*
* Description:  Setup the correct source depandant on the mb predictionmode
*
**************************************************************************/
void mp4d_SetMotionSource(LayerData_t* LayerData_p, MB_Data_t* MB_Data_p, uint8 PredictionMode, MC_Source_t* MC_Params_p)  __attribute__ ((section ("Reconstruct")));


/*************************************************************************
*
* Name:         MotionCompensateMB
*
* Parameters:   LayerData_p         [IN/OUT]  
*               MB_Nr               [IN]  
*               
*
* Returns:      Nothing
*
* Description:	Motion-compensate a macroblock.
*			          Get pixels from prediction image and put the
*               the result in the current reconstructed image.
*
**************************************************************************/

void mp4d_MotionCompensate(LayerData_t* LayerData_p, 
                                     int MB_Nr)  __attribute__ ((section ("Reconstruct")));

/*************************************************************************
*
* Name:         mp4d_CopySkippedMB
*
* Parameters:   LayerData_p         [IN/OUT]  
*               MB_Nr               [IN]  
*               
*
* Returns:      Nothing
*
* Description:  Copies MB that are not motion compensated from previous image
*
**************************************************************************/

void mp4d_CopySkippedMB(LayerData_t* LayerData_p, 
                                int MB_Nr)  __attribute__ ((section ("Reconstruct")));      


/*************************************************************************
*
* Name:         mp4d_MotionCompensateBiDirectional
*
* Parameters:   LayerData_p         [IN/OUT]  
*               MB_Data_p               [IN]  
*               
*
* Returns:      Nothing
*
* Description:  Motion-compensate a macroblock. 
*               Get pixels from prediction image and put the
*               the result in the Dst_p. Used only for B-VOP and predictionmode DIRECT and INTERPOLATE
*
**************************************************************************/
void mp4d_MotionCompensateBiDirectional(LayerData_t* LayerData_p, MB_Data_t* MB_Data_p);

/*************************************************************************
*
* Name:         PadBlock
*
* Parameters:   Src_p [IN]
*               Xpos  [IN]
*               Ypos  [IN]
*               Xmax  [IN]
*               Ymax  [IN]
*
* Returns:
*
* Description:  The returned block is a word aligned 9x9 block
*               to allow half pel interpolation.
*
**************************************************************************/
uint8* mp4d_PadBlock(uint8* Src_p, int Xpos, int Ypos, int Xmax, int Ymax, int MBColorType, int Width, int Height, int BlockSize);

/*************************************************************************/                                  
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECMOTIONCOMPENSATION_H
