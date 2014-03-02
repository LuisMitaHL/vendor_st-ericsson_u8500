/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file YuvToRgbMatrix.h
 */
#ifndef _YUVTORGBMATRIX_H_
#define _YUVTORGBMATRIX_H_

#include "Platform.h"

#define FLOATING_TO_6_10_FIXED_POINT_SCALER 1024.0

typedef struct
{
    /// On-diagonal Matrix coefficient (0,0)
    float_t f_YuvToRgb_Matrix_RCOF00;

    /// Off-diagonal Matrix coefficient (0,1)
    float_t f_YuvToRgb_Matrix_RCOF01;

    /// Off-diagonal Matrix coefficient (0,2)
    float_t f_YuvToRgb_Matrix_RCOF02;

    /// Off-diagonal Matrix coefficient (1,0)
    float_t f_YuvToRgb_Matrix_RCOF10;

    /// Ondiagonal Matrix coefficient (1,1)
    float_t f_YuvToRgb_Matrix_RCOF11;

    /// Off diagonal Matrix coefficient (1,2)
    float_t f_YuvToRgb_Matrix_RCOF12;

    /// Off diagonal Matrix coefficient (2,0)
    float_t f_YuvToRgb_Matrix_RCOF20;

    /// Off diagonal Matrix coefficient (2,1)
    float_t f_YuvToRgb_Matrix_RCOF21;

    /// On diagonal Matrix coefficient (2,0)
    float_t f_YuvToRgb_Matrix_RCOF22;

    /// Offset for R data
    int16_t s16_YuvToRgb_Matrix_R_OFFSET;

    /// Offset for R data
    int16_t s16_YuvToRgb_Matrix_G_OFFSET;

    /// Offset for R data
    int16_t s16_YuvToRgb_Matrix_B_OFFSET;

} YuvToRgbMatrix_Control_ts;


typedef struct
{
    /// On-diagonal Matrix coefficient (0,0)
    int16_t s16_YuvToRgb_Matrix_RCOF00;

    /// Off-diagonal Matrix coefficient (0,1)
    int16_t s16_YuvToRgb_Matrix_RCOF01;

    /// Off-diagonal Matrix coefficient (0,2)
    int16_t s16_YuvToRgb_Matrix_RCOF02;

    /// Off-diagonal Matrix coefficient (1,0)
    int16_t s16_YuvToRgb_Matrix_RCOF10;

    /// Ondiagonal Matrix coefficient (1,1)
    int16_t s16_YuvToRgb_Matrix_RCOF11;

    /// Off diagonal Matrix coefficient (1,2)
    int16_t s16_YuvToRgb_Matrix_RCOF12;

    /// Off diagonal Matrix coefficient (2,0)
    int16_t s16_YuvToRgb_Matrix_RCOF20;

    /// Off diagonal Matrix coefficient (2,1)
    int16_t s16_YuvToRgb_Matrix_RCOF21;

    /// On diagonal Matrix coefficient (2,0)
    int16_t s16_YuvToRgb_Matrix_RCOF22;

    /// Offset for R data
    int16_t s16_YuvToRgb_Matrix_R_OFFSET;

    /// Offset for R data
    int16_t s16_YuvToRgb_Matrix_G_OFFSET;

    /// Offset for R data
    int16_t s16_YuvToRgb_Matrix_B_OFFSET;

} YuvToRgbMatrix_Status_ts;


// Control page of YuvToRgb_Matrix
extern YuvToRgbMatrix_Control_ts  g_YuvToRgb_Matrix_Ctrl[];

// Status page of YuvToRgb_Matrix
extern YuvToRgbMatrix_Status_ts  g_YuvToRgb_Matrix_Status[];


// Function to commit YuvToRgb_Matrix parameters
void YuvToRgbMatrix_Commit(uint8_t u8_PipeNo);

// Function to Update YuvToRgb_Matrix parameters
void YuvToRgbMatrix_Update(uint8_t u8_PipeNo);


#endif /* _YUVTORGBMATRIX_H_ */



