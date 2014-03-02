/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file YuvToRgbMatrix.c
 */
#include "YuvToRgbMatrix.h"
#include "YuvToRgbMatrix_platform_specific.h"


#if 1
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_YuvToRgbMatrix_YuvToRgbMatrixTraces.h"
#endif
#endif

YuvToRgbMatrix_Control_ts  g_YuvToRgb_Matrix_Ctrl[NO_OF_HARDWARE_PIPE_IN_ISP]=
{
    {
        DEFAULT_YUV2RGB_MATRIX_RCOF00,
        DEFAULT_YUV2RGB_MATRIX_RCOF01,
        DEFAULT_YUV2RGB_MATRIX_RCOF02,
        DEFAULT_YUV2RGB_MATRIX_RCOF10,
        DEFAULT_YUV2RGB_MATRIX_RCOF11,
        DEFAULT_YUV2RGB_MATRIX_RCOF12,
        DEFAULT_YUV2RGB_MATRIX_RCOF20,
        DEFAULT_YUV2RGB_MATRIX_RCOF21,
        DEFAULT_YUV2RGB_MATRIX_RCOF22,
        DEFAULT_YUV2RGB_MATRIX_R_OFFSET,
        DEFAULT_YUV2RGB_MATRIX_G_OFFSET,
        DEFAULT_YUV2RGB_MATRIX_B_OFFSET
    },
    {

        DEFAULT_YUV2RGB_MATRIX_RCOF00,
        DEFAULT_YUV2RGB_MATRIX_RCOF01,
        DEFAULT_YUV2RGB_MATRIX_RCOF02,
        DEFAULT_YUV2RGB_MATRIX_RCOF10,
        DEFAULT_YUV2RGB_MATRIX_RCOF11,
        DEFAULT_YUV2RGB_MATRIX_RCOF12,
        DEFAULT_YUV2RGB_MATRIX_RCOF20,
        DEFAULT_YUV2RGB_MATRIX_RCOF21,
        DEFAULT_YUV2RGB_MATRIX_RCOF22,
        DEFAULT_YUV2RGB_MATRIX_R_OFFSET,
        DEFAULT_YUV2RGB_MATRIX_G_OFFSET,
        DEFAULT_YUV2RGB_MATRIX_B_OFFSET
    }
};

YuvToRgbMatrix_Status_ts  g_YuvToRgb_Matrix_Status[NO_OF_HARDWARE_PIPE_IN_ISP]=
{
    {
        DEFAULT_YUV2RGB_MATRIX_RCOF00,
        DEFAULT_YUV2RGB_MATRIX_RCOF01,
        DEFAULT_YUV2RGB_MATRIX_RCOF02,
        DEFAULT_YUV2RGB_MATRIX_RCOF10,
        DEFAULT_YUV2RGB_MATRIX_RCOF11,
        DEFAULT_YUV2RGB_MATRIX_RCOF12,
        DEFAULT_YUV2RGB_MATRIX_RCOF20,
        DEFAULT_YUV2RGB_MATRIX_RCOF21,
        DEFAULT_YUV2RGB_MATRIX_RCOF22,
        DEFAULT_YUV2RGB_MATRIX_R_OFFSET,
        DEFAULT_YUV2RGB_MATRIX_G_OFFSET,
        DEFAULT_YUV2RGB_MATRIX_B_OFFSET
    },
    {

        DEFAULT_YUV2RGB_MATRIX_RCOF00,
        DEFAULT_YUV2RGB_MATRIX_RCOF01,
        DEFAULT_YUV2RGB_MATRIX_RCOF02,
        DEFAULT_YUV2RGB_MATRIX_RCOF10,
        DEFAULT_YUV2RGB_MATRIX_RCOF11,
        DEFAULT_YUV2RGB_MATRIX_RCOF12,
        DEFAULT_YUV2RGB_MATRIX_RCOF20,
        DEFAULT_YUV2RGB_MATRIX_RCOF21,
        DEFAULT_YUV2RGB_MATRIX_RCOF22,
        DEFAULT_YUV2RGB_MATRIX_R_OFFSET,
        DEFAULT_YUV2RGB_MATRIX_G_OFFSET,
        DEFAULT_YUV2RGB_MATRIX_B_OFFSET
    }
};


void
YuvToRgbMatrix_Commit(uint8_t u8_PipeNo)
{
    switch (u8_PipeNo)
    {
        case 0:
        {
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_RCOF00_rcof00(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF00);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_RCOF01_rcof01(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF01);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_RCOF02_rcof02(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF02);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_RCOF10_rcof10(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF10);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_RCOF11_rcof11(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF11);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_RCOF12_rcof12(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF12);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_RCOF20_rcof20(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF20);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_RCOF21_rcof21(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF21);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_RCOF22_rcof22(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF22);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_R_OFFSET_r_offset(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_R_OFFSET);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_G_OFFSET_g_offset(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_G_OFFSET);
            Set_ISP_CE0_YUV2RGB_MATRIX_ISP_CE0_YUV2RGB_MATRIX_B_OFFSET_b_offset(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_B_OFFSET);
            break;
        }
        case 1:
        {
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_RCOF00_rcof00(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF00);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_RCOF01_rcof01(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF01);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_RCOF02_rcof02(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF02);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_RCOF10_rcof10(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF10);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_RCOF11_rcof11(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF11);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_RCOF12_rcof12(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF12);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_RCOF20_rcof20(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF20);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_RCOF21_rcof21(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF21);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_RCOF22_rcof22(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF22);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_R_OFFSET_r_offset(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_R_OFFSET);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_G_OFFSET_g_offset(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_G_OFFSET);
            Set_ISP_CE1_YUV2RGB_MATRIX_ISP_CE1_YUV2RGB_MATRIX_B_OFFSET_b_offset(g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_B_OFFSET);
            break;
        }
        default :
        {
#if USE_TRACE_ERROR
            OstTraceInt1(TRACE_ERROR, "<ERROR>!!YuvToRgb_Matrix_Commit() pipe index passed is %d",u8_PipeNo);
#endif
            // ideally code should not come here
            ASSERT_XP70();
        }
    }
}

void
YuvToRgbMatrix_Update(uint8_t u8_PipeNo)
{
    if ((u8_PipeNo & 0xFE) == 0) // If the condition is true it means u8_PipeNo is either 1 or 0
    {
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF00 = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].f_YuvToRgb_Matrix_RCOF00 * FLOATING_TO_6_10_FIXED_POINT_SCALER;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF01 = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].f_YuvToRgb_Matrix_RCOF01 * FLOATING_TO_6_10_FIXED_POINT_SCALER;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF02 = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].f_YuvToRgb_Matrix_RCOF02 * FLOATING_TO_6_10_FIXED_POINT_SCALER;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF10 = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].f_YuvToRgb_Matrix_RCOF10 * FLOATING_TO_6_10_FIXED_POINT_SCALER;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF11 = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].f_YuvToRgb_Matrix_RCOF11 * FLOATING_TO_6_10_FIXED_POINT_SCALER;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF12 = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].f_YuvToRgb_Matrix_RCOF12 * FLOATING_TO_6_10_FIXED_POINT_SCALER;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF20 = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].f_YuvToRgb_Matrix_RCOF20 * FLOATING_TO_6_10_FIXED_POINT_SCALER;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF21 = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].f_YuvToRgb_Matrix_RCOF21 * FLOATING_TO_6_10_FIXED_POINT_SCALER;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_RCOF22 = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].f_YuvToRgb_Matrix_RCOF22 * FLOATING_TO_6_10_FIXED_POINT_SCALER;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_R_OFFSET = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].s16_YuvToRgb_Matrix_R_OFFSET;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_G_OFFSET = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].s16_YuvToRgb_Matrix_G_OFFSET;
        g_YuvToRgb_Matrix_Status[u8_PipeNo].s16_YuvToRgb_Matrix_B_OFFSET = g_YuvToRgb_Matrix_Ctrl[u8_PipeNo].s16_YuvToRgb_Matrix_B_OFFSET;
    }
    else
    {
#if USE_TRACE_ERROR
        OstTraceInt1(TRACE_ERROR, "<ERROR>!!YuvToRgb_Matrix_Update() pipe index passed is %d",u8_PipeNo);
#endif
        // ideally code should not come here
        ASSERT_XP70();
    }
}
