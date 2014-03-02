/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file RgbToYuvMatrix.h
 */
#ifndef _RGBTOYUVMATRIX_H_
#define _RGBTOYUVMATRIX_H_

#include "Platform.h"


#define FLOATING_TO_6_10_FIXED_POINT_SCALER 1024.0

typedef struct
{
    /// On-diagonal Matrix coefficient (0,0)
    float_t f_RgbToYuv_Matrix_RCOF00;

    /// Off-diagonal Matrix coefficient (0,1)
    float_t f_RgbToYuv_Matrix_RCOF01;

    /// Off-diagonal Matrix coefficient (0,2)
    float_t f_RgbToYuv_Matrix_RCOF02;

    /// Off-diagonal Matrix coefficient (1,0)
    float_t f_RgbToYuv_Matrix_RCOF10;

    /// Ondiagonal Matrix coefficient (1,1)
    float_t f_RgbToYuv_Matrix_RCOF11;

    /// Off diagonal Matrix coefficient (1,2)
    float_t f_RgbToYuv_Matrix_RCOF12;

    /// Off diagonal Matrix coefficient (2,0)
    float_t f_RgbToYuv_Matrix_RCOF20;

    /// Off diagonal Matrix coefficient (2,1)
    float_t f_RgbToYuv_Matrix_RCOF21;

    /// On diagonal Matrix coefficient (2,0)
    float_t f_RgbToYuv_Matrix_RCOF22;

    /// Offset for R data
    int16_t s16_RgbToYuv_Matrix_R_OFFSET;

    /// Offset for R data
    int16_t s16_RgbToYuv_Matrix_G_OFFSET;

    /// Offset for R data
    int16_t s16_RgbToYuv_Matrix_B_OFFSET;

} RgbToYuvMatrix_Control_ts;


typedef struct
{
    /// On-diagonal Matrix coefficient (0,0)
    int16_t s16_RgbToYuv_Matrix_RCOF00;

    /// Off-diagonal Matrix coefficient (0,1)
    int16_t s16_RgbToYuv_Matrix_RCOF01;

    /// Off-diagonal Matrix coefficient (0,2)
    int16_t s16_RgbToYuv_Matrix_RCOF02;

    /// Off-diagonal Matrix coefficient (1,0)
    int16_t s16_RgbToYuv_Matrix_RCOF10;

    /// Ondiagonal Matrix coefficient (1,1)
    int16_t s16_RgbToYuv_Matrix_RCOF11;

    /// Off diagonal Matrix coefficient (1,2)
    int16_t s16_RgbToYuv_Matrix_RCOF12;

    /// Off diagonal Matrix coefficient (2,0)
    int16_t s16_RgbToYuv_Matrix_RCOF20;

    /// Off diagonal Matrix coefficient (2,1)
    int16_t s16_RgbToYuv_Matrix_RCOF21;

    /// On diagonal Matrix coefficient (2,0)
    int16_t s16_RgbToYuv_Matrix_RCOF22;

    /// Offset for R data
    int16_t s16_RgbToYuv_Matrix_R_OFFSET;

    /// Offset for R data
    int16_t s16_RgbToYuv_Matrix_G_OFFSET;

    /// Offset for R data
    int16_t s16_RgbToYuv_Matrix_B_OFFSET;

} RgbToYuvMatrix_Status_ts;


// Control page of RgbToYuv_Matrix
extern RgbToYuvMatrix_Control_ts  g_RgbToYuv_Matrix_Ctrl[];

// Status page of RgbToYuv_Matrix
extern RgbToYuvMatrix_Status_ts  g_RgbToYuv_Matrix_Status[];


// Function to commit RgbToYuv_Matrix parameters
void RgbToYuvMatrix_Commit(uint8_t u8_PipeNo);

// Function to Update RgbToYuv_Matrix parameters
void RgbToYuvMatrix_Update(uint8_t u8_PipeNo);


#endif /* _RGBTOYUVMATRIX_H_ */


