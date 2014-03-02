/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECDEBLOCKINGFILTER_P3_H
#define INCLUSION_GUARD_VIDEOCODECDEBLOCKINGFILTER_P3_H
#include "algo_deblock.hpp"
#ifdef __cplusplus
extern "C" {
#endif
#define MABS(a)      (((a) < 0) ? -(a) : (a))
#define MAX(a,b)     ((a) > (b) ? (a) : (b))
#define MIN(a,b)     ((a) < (b) ? (a) : (b))
#define MMAX(a,b)  (((a)>(b)) ? (a) : (b))
#define MSIGN(a)              ((a) < 0 ? -1 : 1)
#define ABS(a)              ((a) < 0 ? -a : a)

// Clipping to an interval
#define MCLIP(var, min, max)  ((var) < (min) ? (min) : ((var) > (max) ? (max) : (var)))

// Clipping for positive values 
#define PCLIP(var,max)        ((var)>(max) ? (max) : (var))

#define TCLIP(var, min, max)    tclip[(var)]
#define Div2Round(X)     (((X) >> 1) | ((X) & 1))
#define GET_Y_PTR(img, x, y)  ((img)->Y_p  + (x) + (y) * (img)->Y_LineWidth)
#define GET_CB_PTR(img, x, y) ((img)->Cb_p + (x) + (y) * (img)->C_LineWidth)
#define GET_CR_PTR(img, x, y) ((img)->Cr_p + (x) + (y) * (img)->C_LineWidth)
const t_uint8 ChrominanceQuantTable_P3[32] =
{
  0, 1, 2, 3, 4, 5, 6,
  6, 7, 8,
  9, 9,
  10, 10,
  11, 11,
  12, 12, 12,
  13, 13, 13,
  14, 14, 14, 14, 14,
  15, 15, 15, 15, 15
};
/*************************************************************************
* Types, constants and external variables
**************************************************************************/

/*************************************************************************
* Declarations of functions
**************************************************************************/
/*************************************************************************
*
* Name:         mp4d_DeblockingFilter
*
* Parameters:   Image_p      [OUT]  
*               MB_Data_p    [IN]  
*               LayerData_p  [IN]  
*               
*
* Returns:      Nothing
*
* Description:  Deblocking filter Annex J, H.263
*               Block edges are smoothed
*
**************************************************************************/

void mp4d_DeblockingFilter_P3(YUV_Image_dblk* Image_p, 
					  PostFilter_State_t *State_p);

/*************************************************************************/

#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECDEBLOCKINGFILTER_P3_H
