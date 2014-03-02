/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file ColourMatrix.c
 \brief This file is a part of the release code of the colour matrix module.
        The file implements damper update and pipe update functionality.\n
        ASSUMPTION: The sum of each row should always be unity.
 \ingroup ColourMatrix
 \endif
*/
#include "ColourMatrix.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_ColourMatrix_ColourMatrixTraces.h"
#endif

// Colour matrix page elements for PIPE0 and PIPE1
// g_CE_ColourMatrixFloat[0-1] store the values from Sensor0
// The values can be modified after reading them from sensor.
ColourMatrixFloat_ts    g_CE_ColourMatrixFloat[COLOURMATRIX_NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, SwapColour_e_RedSwap, SwapColour_e_GreenSwap,
            SwapColour_e_BlueSwap },
    { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, SwapColour_e_RedSwap, SwapColour_e_GreenSwap, SwapColour_e_BlueSwap }
};

/// Control Page element for colour matrix (Both PIPE0 and PIPE1)
/// g_CE_ColourMatrixCtrl[0-1] for programming the offset vector for PIPES
ColourMatrixCtrl_ts     g_CE_ColourMatrixCtrl[COLOURMATRIX_NO_OF_HARDWARE_PIPE_IN_ISP] = { { 0, 0, 0 }, { 0, 0, 0 } };

// Status page element holds the damped values for Colour matrix for PIPE[0/1]
ColourMatrixFixed_ts    g_CE_ColourMatrixDamped[COLOURMATRIX_NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { COLOURMATRIX_DEFAULT_VALUE_UNITY, COLOURMATRIX_DEFAULT_VALUE_ZERO, COLOURMATRIX_DEFAULT_VALUE_ZERO,
            COLOURMATRIX_DEFAULT_VALUE_ZERO, COLOURMATRIX_DEFAULT_VALUE_UNITY, COLOURMATRIX_DEFAULT_VALUE_ZERO,
                COLOURMATRIX_DEFAULT_VALUE_ZERO, COLOURMATRIX_DEFAULT_VALUE_ZERO,
                    COLOURMATRIX_DEFAULT_VALUE_UNITY, 0, 0, 0 },
    { COLOURMATRIX_DEFAULT_VALUE_UNITY, COLOURMATRIX_DEFAULT_VALUE_ZERO, COLOURMATRIX_DEFAULT_VALUE_ZERO,
            COLOURMATRIX_DEFAULT_VALUE_ZERO, COLOURMATRIX_DEFAULT_VALUE_UNITY, COLOURMATRIX_DEFAULT_VALUE_ZERO,
                COLOURMATRIX_DEFAULT_VALUE_ZERO, COLOURMATRIX_DEFAULT_VALUE_ZERO,
                    COLOURMATRIX_DEFAULT_VALUE_UNITY, 0, 0, 0 }
};

/**
 \fn void ColourMatrix_DamperUpdate(uint8_t u8_PipeNo)
 \brief Calculate fixed point colour matrix after applying damper
 \details The procedure calculate the colour matrix from the floating point matrix read from the sensor.
          It apply damber on the matrix and convert floating point matrix into 6.10 floating format.
          The procedure should be called from status line interrupt as user is given flexibility to
          change colour matrix and damper parameters while streaming.
 \param u8_PipeNo        : Pipe No in ISP
 \return void
 \ingroup ColourMatrix
 \callgraph
 \callergraph
*/
void
ColourMatrix_DamperUpdate(
uint8_t u8_PipeNo)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Pointer to Floating point colour matrix (read from sensor) for current sensor and Pipe number passed
    ColourMatrixFloat_ts    *ptr_CurrentSrcFloatMatrix = ( ColourMatrixFloat_ts * ) NULL;

    // Pointer to Target/Status colour matrix for current pipe. The matrix will be programmed in H/W
    ColourMatrixFixed_ts    *ptr_CurrentTrgtFixedMatrix = ( ColourMatrixFixed_ts * ) NULL;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ptr_CurrentSrcFloatMatrix = &g_CE_ColourMatrixFloat[u8_PipeNo];
    ptr_CurrentTrgtFixedMatrix = &g_CE_ColourMatrixDamped[u8_PipeNo];

    // 1st row...
    ptr_CurrentTrgtFixedMatrix->s16_GreenInRed = COLOURMATRIX_CreateDampedMatrixElement(
        ptr_CurrentSrcFloatMatrix->f_GreenInRed,
        1.0);
    ptr_CurrentTrgtFixedMatrix->s16_BlueInRed = COLOURMATRIX_CreateDampedMatrixElement(
        ptr_CurrentSrcFloatMatrix->f_BlueInRed,
        1.0);
    ptr_CurrentTrgtFixedMatrix->s16_RedInRed = COLOURMATRIX_CreateDampedMatrixElement(
        ptr_CurrentSrcFloatMatrix->f_RedInRed,
        1.0);

    // 2nd row...
    ptr_CurrentTrgtFixedMatrix->s16_RedInGreen = COLOURMATRIX_CreateDampedMatrixElement(
        ptr_CurrentSrcFloatMatrix->f_RedInGreen,
        1.0);
    ptr_CurrentTrgtFixedMatrix->s16_BlueInGreen = COLOURMATRIX_CreateDampedMatrixElement(
        ptr_CurrentSrcFloatMatrix->f_BlueInGreen,
        1.0);
    ptr_CurrentTrgtFixedMatrix->s16_GreenInGreen = COLOURMATRIX_CreateDampedMatrixElement(
        ptr_CurrentSrcFloatMatrix->f_GreenInGreen,
        1.0);

    // 3rd row...
    ptr_CurrentTrgtFixedMatrix->s16_RedInBlue = COLOURMATRIX_CreateDampedMatrixElement(
        ptr_CurrentSrcFloatMatrix->f_RedInBlue,
        1.0);
    ptr_CurrentTrgtFixedMatrix->s16_GreenInBlue = COLOURMATRIX_CreateDampedMatrixElement(
        ptr_CurrentSrcFloatMatrix->f_GreenInBlue,
        1.0);
    ptr_CurrentTrgtFixedMatrix->s16_BlueInBlue = COLOURMATRIX_CreateDampedMatrixElement(
        ptr_CurrentSrcFloatMatrix->f_BlueInBlue,
        1.0);

    ptr_CurrentTrgtFixedMatrix->s16_Offset_R = g_CE_ColourMatrixCtrl[u8_PipeNo].s16_Offset_R;
    ptr_CurrentTrgtFixedMatrix->s16_Offset_G = g_CE_ColourMatrixCtrl[u8_PipeNo].s16_Offset_G;
    ptr_CurrentTrgtFixedMatrix->s16_Offset_B = g_CE_ColourMatrixCtrl[u8_PipeNo].s16_Offset_B;
    return;
}


/**
 \fn void ColourMatrix_Commit(uint8_t u8_PipeNo)
 \brief Update ISP hardware with colour matrix values in g_CE_ColourMatrixDamped[]
 \details Update the hardware pipe( 0 or 1) with matrix values calculated in ColourMatrix_DamperUpdate()
          The procedure should be called from Video complete interrupt or end of frame as the values are
          applied directly to ISP Pixel Pipe, so frame bountary should be maintained.
 \param u8_PipeNo        : Pipe No in ISP
 \return void
 \ingroup ColourMatrix
 \callgraph
 \callergraph
*/
void
ColourMatrix_Commit(
uint8_t u8_PipeNo)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Pointer to Target/Status colour matrix for current pipe. The matrix will be programmed in H/W
    ColourMatrixFixed_ts    *ptr_CurrentTrgtFixedMatrix = &g_CE_ColourMatrixDamped[u8_PipeNo];

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if (0 == u8_PipeNo)
    {
        if (SwapColour_e_RedSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Red)
        {
            // Row0
            SET_PIPE0_COLOUR_MATRIX_RCOF00_RedInRed(ptr_CurrentTrgtFixedMatrix->s16_RedInRed);
            SET_PIPE0_COLOUR_MATRIX_RCOF01_GreenInRed(ptr_CurrentTrgtFixedMatrix->s16_GreenInRed);
            SET_PIPE0_COLOUR_MATRIX_RCOF02_BlueInRed(ptr_CurrentTrgtFixedMatrix->s16_BlueInRed);
        }
        else if (SwapColour_e_GreenSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Red)
        {
            // Row0
            SET_PIPE0_COLOUR_MATRIX_RCOF00_RedInRed(ptr_CurrentTrgtFixedMatrix->s16_RedInGreen);
            SET_PIPE0_COLOUR_MATRIX_RCOF01_GreenInRed(ptr_CurrentTrgtFixedMatrix->s16_GreenInGreen);
            SET_PIPE0_COLOUR_MATRIX_RCOF02_BlueInRed(ptr_CurrentTrgtFixedMatrix->s16_BlueInGreen);
        }
        else if (SwapColour_e_BlueSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Red)
        {
            // Row0
            SET_PIPE0_COLOUR_MATRIX_RCOF00_RedInRed(ptr_CurrentTrgtFixedMatrix->s16_RedInBlue);
            SET_PIPE0_COLOUR_MATRIX_RCOF01_GreenInRed(ptr_CurrentTrgtFixedMatrix->s16_GreenInBlue);
            SET_PIPE0_COLOUR_MATRIX_RCOF02_BlueInRed(ptr_CurrentTrgtFixedMatrix->s16_BlueInBlue);
        }
        else
        {
#if USE_TRACE_ERROR        
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!ColourMatrix_Commit() - wrong swap channel!!");
#endif
            ASSERT_XP70();
        }


        if (SwapColour_e_GreenSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Green)
        {
            // Row1
            SET_PIPE0_COLOUR_MATRIX_RCOF10_RedInGreen(ptr_CurrentTrgtFixedMatrix->s16_RedInGreen);
            SET_PIPE0_COLOUR_MATRIX_RCOF11_GreenInGreen(ptr_CurrentTrgtFixedMatrix->s16_GreenInGreen);
            SET_PIPE0_COLOUR_MATRIX_RCOF12_BlueInGreen(ptr_CurrentTrgtFixedMatrix->s16_BlueInGreen);
        }
        else if (SwapColour_e_RedSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Green)
        {
            // Row1
            SET_PIPE0_COLOUR_MATRIX_RCOF10_RedInGreen(ptr_CurrentTrgtFixedMatrix->s16_RedInRed);
            SET_PIPE0_COLOUR_MATRIX_RCOF11_GreenInGreen(ptr_CurrentTrgtFixedMatrix->s16_GreenInRed);
            SET_PIPE0_COLOUR_MATRIX_RCOF12_BlueInGreen(ptr_CurrentTrgtFixedMatrix->s16_BlueInRed);
        }
        else if (SwapColour_e_BlueSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Green)
        {
            // Row1
            SET_PIPE0_COLOUR_MATRIX_RCOF10_RedInGreen(ptr_CurrentTrgtFixedMatrix->s16_RedInBlue);
            SET_PIPE0_COLOUR_MATRIX_RCOF11_GreenInGreen(ptr_CurrentTrgtFixedMatrix->s16_GreenInBlue);
            SET_PIPE0_COLOUR_MATRIX_RCOF12_BlueInGreen(ptr_CurrentTrgtFixedMatrix->s16_BlueInBlue);
        }
        else
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!ColourMatrix_Commit() - wrong swap channel!!");
            ASSERT_XP70();
        }


        if (SwapColour_e_BlueSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Blue)
        {
            // Row2
            SET_PIPE0_COLOUR_MATRIX_RCOF20_RedInBlue(ptr_CurrentTrgtFixedMatrix->s16_RedInBlue);
            SET_PIPE0_COLOUR_MATRIX_RCOF21_GreenInBlue(ptr_CurrentTrgtFixedMatrix->s16_GreenInBlue);
            SET_PIPE0_COLOUR_MATRIX_RCOF22_BlueInBlue(ptr_CurrentTrgtFixedMatrix->s16_BlueInBlue);
        }
        else if (SwapColour_e_RedSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Blue)
        {
            // Row2
            SET_PIPE0_COLOUR_MATRIX_RCOF20_RedInBlue(ptr_CurrentTrgtFixedMatrix->s16_RedInRed);
            SET_PIPE0_COLOUR_MATRIX_RCOF21_GreenInBlue(ptr_CurrentTrgtFixedMatrix->s16_GreenInRed);
            SET_PIPE0_COLOUR_MATRIX_RCOF22_BlueInBlue(ptr_CurrentTrgtFixedMatrix->s16_BlueInRed);
        }
        else if (SwapColour_e_GreenSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Blue)
        {
            // Row2
            SET_PIPE0_COLOUR_MATRIX_RCOF20_RedInBlue(ptr_CurrentTrgtFixedMatrix->s16_RedInGreen);
            SET_PIPE0_COLOUR_MATRIX_RCOF21_GreenInBlue(ptr_CurrentTrgtFixedMatrix->s16_GreenInGreen);
            SET_PIPE0_COLOUR_MATRIX_RCOF22_BlueInBlue(ptr_CurrentTrgtFixedMatrix->s16_BlueInGreen);
        }
        else
        {
#if USE_TRACE_ERROR        
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!ColourMatrix_Commit() - wrong swap channel!!");
#endif
            ASSERT_XP70();
        }


        SET_PIPE0_COLOUR_MATRIX_R_OFFSET(ptr_CurrentTrgtFixedMatrix->s16_Offset_R);
        SET_PIPE0_COLOUR_MATRIX_G_OFFSET(ptr_CurrentTrgtFixedMatrix->s16_Offset_G);
        SET_PIPE0_COLOUR_MATRIX_B_OFFSET(ptr_CurrentTrgtFixedMatrix->s16_Offset_B);
    }
    else    // PIPE1
    {
        if (SwapColour_e_RedSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Red)
        {
            // Row0
            SET_PIPE1_COLOUR_MATRIX_RCOF00_RedInRed(ptr_CurrentTrgtFixedMatrix->s16_RedInRed);
            SET_PIPE1_COLOUR_MATRIX_RCOF01_GreenInRed(ptr_CurrentTrgtFixedMatrix->s16_GreenInRed);
            SET_PIPE1_COLOUR_MATRIX_RCOF02_BlueInRed(ptr_CurrentTrgtFixedMatrix->s16_BlueInRed);
        }
        else if (SwapColour_e_GreenSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Red)
        {
            // Row0
            SET_PIPE1_COLOUR_MATRIX_RCOF00_RedInRed(ptr_CurrentTrgtFixedMatrix->s16_RedInGreen);
            SET_PIPE1_COLOUR_MATRIX_RCOF01_GreenInRed(ptr_CurrentTrgtFixedMatrix->s16_GreenInGreen);
            SET_PIPE1_COLOUR_MATRIX_RCOF02_BlueInRed(ptr_CurrentTrgtFixedMatrix->s16_BlueInGreen);
        }
        else if (SwapColour_e_BlueSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Red)
        {
            // Row0
            SET_PIPE1_COLOUR_MATRIX_RCOF00_RedInRed(ptr_CurrentTrgtFixedMatrix->s16_RedInBlue);
            SET_PIPE1_COLOUR_MATRIX_RCOF01_GreenInRed(ptr_CurrentTrgtFixedMatrix->s16_GreenInBlue);
            SET_PIPE1_COLOUR_MATRIX_RCOF02_BlueInRed(ptr_CurrentTrgtFixedMatrix->s16_BlueInBlue);
        }
        else
        {
#if USE_TRACE_ERROR        
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!ColourMatrix_Commit() - wrong swap channel!!");
#endif
            ASSERT_XP70();
        }


        if (SwapColour_e_GreenSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Green)
        {
            // Row1
            SET_PIPE1_COLOUR_MATRIX_RCOF10_RedInGreen(ptr_CurrentTrgtFixedMatrix->s16_RedInGreen);
            SET_PIPE1_COLOUR_MATRIX_RCOF11_GreenInGreen(ptr_CurrentTrgtFixedMatrix->s16_GreenInGreen);
            SET_PIPE1_COLOUR_MATRIX_RCOF12_BlueInGreen(ptr_CurrentTrgtFixedMatrix->s16_BlueInGreen);
        }
        else if (SwapColour_e_RedSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Green)
        {
            // Row1
            SET_PIPE1_COLOUR_MATRIX_RCOF10_RedInGreen(ptr_CurrentTrgtFixedMatrix->s16_RedInRed);
            SET_PIPE1_COLOUR_MATRIX_RCOF11_GreenInGreen(ptr_CurrentTrgtFixedMatrix->s16_GreenInRed);
            SET_PIPE1_COLOUR_MATRIX_RCOF12_BlueInGreen(ptr_CurrentTrgtFixedMatrix->s16_BlueInRed);
        }
        else if (SwapColour_e_BlueSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Green)
        {
            // Row1
            SET_PIPE1_COLOUR_MATRIX_RCOF10_RedInGreen(ptr_CurrentTrgtFixedMatrix->s16_RedInBlue);
            SET_PIPE1_COLOUR_MATRIX_RCOF11_GreenInGreen(ptr_CurrentTrgtFixedMatrix->s16_GreenInBlue);
            SET_PIPE1_COLOUR_MATRIX_RCOF12_BlueInGreen(ptr_CurrentTrgtFixedMatrix->s16_BlueInBlue);
        }
        else
        {
#if USE_TRACE_ERROR        
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!ColourMatrix_Commit() - wrong swap channel!!");
#endif
            ASSERT_XP70();
        }


        if (SwapColour_e_BlueSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Blue)
        {
            // Row2
            SET_PIPE1_COLOUR_MATRIX_RCOF20_RedInBlue(ptr_CurrentTrgtFixedMatrix->s16_RedInBlue);
            SET_PIPE1_COLOUR_MATRIX_RCOF21_GreenInBlue(ptr_CurrentTrgtFixedMatrix->s16_GreenInBlue);
            SET_PIPE1_COLOUR_MATRIX_RCOF22_BlueInBlue(ptr_CurrentTrgtFixedMatrix->s16_BlueInBlue);
        }
        else if (SwapColour_e_RedSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Blue)
        {
            // Row2
            SET_PIPE1_COLOUR_MATRIX_RCOF20_RedInBlue(ptr_CurrentTrgtFixedMatrix->s16_RedInRed);
            SET_PIPE1_COLOUR_MATRIX_RCOF21_GreenInBlue(ptr_CurrentTrgtFixedMatrix->s16_GreenInRed);
            SET_PIPE1_COLOUR_MATRIX_RCOF22_BlueInBlue(ptr_CurrentTrgtFixedMatrix->s16_BlueInRed);
        }
        else if (SwapColour_e_GreenSwap == g_CE_ColourMatrixFloat[u8_PipeNo].e_SwapColour_Blue)
        {
            // Row2
            SET_PIPE1_COLOUR_MATRIX_RCOF20_RedInBlue(ptr_CurrentTrgtFixedMatrix->s16_RedInGreen);
            SET_PIPE1_COLOUR_MATRIX_RCOF21_GreenInBlue(ptr_CurrentTrgtFixedMatrix->s16_GreenInGreen);
            SET_PIPE1_COLOUR_MATRIX_RCOF22_BlueInBlue(ptr_CurrentTrgtFixedMatrix->s16_BlueInGreen);
        }
        else
        {
#if USE_TRACE_ERROR        
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!ColourMatrix_Commit() - wrong swap channel!!");
#endif
            ASSERT_XP70();
        }


        SET_PIPE1_COLOUR_MATRIX_R_OFFSET(ptr_CurrentTrgtFixedMatrix->s16_Offset_R);
        SET_PIPE1_COLOUR_MATRIX_G_OFFSET(ptr_CurrentTrgtFixedMatrix->s16_Offset_G);
        SET_PIPE1_COLOUR_MATRIX_B_OFFSET(ptr_CurrentTrgtFixedMatrix->s16_Offset_B);
    }


    return;
}

