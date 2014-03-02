/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \if INCLUDE_IN_HTML_ONLY
 \file  RgbToYuvCoder.c

 \brief This file is a part of the release code of the RGB to YUV convertor module.
        The file export user interface functions.
 \ingroup RgbToYuvCoder
 \endif
*/
#include "RgbToYuvCoder.h"
#include "RgbToYuvCoder_PlatformSpecific.h"
#include "osttrace.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_RgbToYuvCoder_RGB2YUVTraces.h"
#endif







/// Control page elements for RGB to YUV coder block
volatile RgbToYuvControl_ts              g_CE_YUVCoderControls[RGBTOYUVCODER_NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { RGBTOYUVCODER_DEFAULT_YCBCR_TRANSFORM, RGBTOYUVCODER_DEFAULT_VALUE_100_PERCENT, RGBTOYUVCODER_DEFAULT_MAX_CONTRAST, RGBTOYUVCODER_DEFAULT_VALUE_100_PERCENT, RGBTOYUVCODER_DEFAULT_MAX_SATURATION },
    { RGBTOYUVCODER_DEFAULT_YCBCR_TRANSFORM, RGBTOYUVCODER_DEFAULT_VALUE_100_PERCENT, RGBTOYUVCODER_DEFAULT_MAX_CONTRAST, RGBTOYUVCODER_DEFAULT_VALUE_100_PERCENT, RGBTOYUVCODER_DEFAULT_MAX_SATURATION }
};

/// Signal range for YUV and RGB
/// The values will be used only in Transform_e_YCbCr_Custom and Transform_e_Rgb_Custom custom transformation mode
/// Standard modes has Matrices in the code
///
volatile RgbToYuvOutputSignalRange_ts    g_CE_CustomTransformOutputSignalRange[RGBTOYUVCODER_NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { 255, 255, 256, 256 },
    { 255, 255, 256, 256 }
};

volatile FadeToBlack_ts                  g_CE_FadeToBlack[RGBTOYUVCODER_NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { RGBTOYUVCODER_DEFAULT_BLACK_VALUE, RGBTOYUVCODER_DEFAULT_LOW_THRESHOLD, RGBTOYUVCODER_DEFAULT_HIGH_THRESHOLD, 1.0, Flag_e_TRUE },
    { RGBTOYUVCODER_DEFAULT_BLACK_VALUE, RGBTOYUVCODER_DEFAULT_LOW_THRESHOLD, RGBTOYUVCODER_DEFAULT_HIGH_THRESHOLD, 1.0, Flag_e_TRUE }
};

volatile CustomStockMatrixControl_ts g_CustomStockMatrix[RGBTOYUVCODER_NO_OF_HARDWARE_PIPE_IN_ISP] = 
{ 
        { {0.299, 0.587, 0.114, -0.169, -0.331, 0.5, 0.5, -0.419, -0.081 }},
        { {0.299, 0.587, 0.114, -0.169, -0.331, 0.5, 0.5, -0.419, -0.081 }}
};


/// Output to be programmed in YUV coder block
volatile RgbToYuvMatrix_ts               g_CE_OutputCoderMatrix[RGBTOYUVCODER_NO_OF_HARDWARE_PIPE_IN_ISP];
volatile RgbToYuvOffsetVector_ts         g_CE_OutputCoderOffsetVector[RGBTOYUVCODER_NO_OF_HARDWARE_PIPE_IN_ISP];

volatile YUVCoderStatus_ts               g_YUVCoderStatus[RGBTOYUVCODER_NO_OF_HARDWARE_PIPE_IN_ISP] = { { 1.0, 1.0 }, { 1.0, 1.0 } };
volatile LumaOffset_ts                   g_CE_LumaOffset[RGBTOYUVCODER_NO_OF_HARDWARE_PIPE_IN_ISP] = {{0,Flag_e_FALSE},{0,Flag_e_FALSE}};
/**
 \fn void RgbToYuvCoder_Update(uint8_t u8_PipeNo)
 \brief The procedure computes the RGB2YUV coder elements for colour engine.
 \details The procedure compute YUV conversion matrix coefficient after applying constrast
          and saturation values on standard matrix. The procedure can be called at any point of
          time as it just calculate the values.
 \param u8_PipeNo        : Pipe No in ISP
 \return void
 \ingroup RgbToYuvCoder
 \callgraph
 \callergraph
*/
void
RgbToYuvCoder_Update(
uint8_t u8_PipeNo)
{
#if ENABLE_YUVCODER_TRACES
        OstTraceInt3(TRACE_DEBUG, "<yuvcoder>  RgbToYuvCoder_Update >>  Pipe number = %u, g_Pipe[%u].e_OutputFormat_Pipe = %u\n", u8_PipeNo, u8_PipeNo, g_Pipe[u8_PipeNo].e_OutputFormat_Pipe); 
#endif
    // Ensure that the transform type programmed by the host is in line with the pipe output format...
    // If Coder transform is different then change it to default
    if (RgbToYuvCoder_IsCurrentPipeOutputFormat_YUV422(u8_PipeNo))
    {
        // ensure that the transform type selected by the host in sync
        // with the pipe output format...
        if (g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type > Transform_e_YCbCr_Custom)
        {
            // the host setting is not inline with the pipe output format...
            // default to TransformType_YCbCr_JFIF...
            g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type = Transform_e_YCbCr_JFIF;
        }
    }
    else
    {
        // ensure that the transform type selected by the host in sync
        // with the pipe output format...
        if (g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type < Transform_e_Rgb_Standard)
        {
            // the host setting is not inline with the pipe output format...
            // default to TransformType_Rgb...
            g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type = Transform_e_Rgb_Standard;
        }
    }

#if ENABLE_YUVCODER_TRACES
        OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  g_CE_YUVCoderControls[%u].e_Transform_Type = %u\n", u8_PipeNo, g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type); 
#endif

    RgbToYuvCoder_Run(u8_PipeNo);

#if ENABLE_YUVCODER_TRACES        
        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  << RgbToYuvCoder_Update\n");        
#endif   

    return;
}


/**
 \fn void RGBtoYUVCoder_Commit(uint8_t u8_PipeNo)
 \brief Program the values of RGB2YUV elements on the hardware registers.
 \details The procedure program YUV conversion matrix, YCbCr floor and
          output type to the ISP pipe. Yuv coder module do not have shadow register,
          so the function should be called on frame boundary i.e. video complete interrupt or
          Statistic interrupt. This will ensure there is no distorion effect on image.
 \param u8_PipeNo        : Pipe No in ISP
 \return void
 \ingroup RgbToYuvCoder
 \callgraph
 \callergraph
*/
void
RGBtoYUVCoder_Commit(
uint8_t u8_PipeNo)
{
    int16_t s16_YFloor,s16_CBFloor,s16_CRFloor;

    if (0 == u8_PipeNo)
    {
        Set_ISP_yuvCoder_ce0_ENABLE(1, 0);  //enable,soft_reset
        if (RgbToYuvCoder_IsCurrentPipeOutputFormat_YUV422(u8_PipeNo))
        {
            // pipe0 streaming YUV
            SET_PIPE0_YUV_CODER_FORMAT_TO_YUV422();
            SET_PIPE0_YUV_CODER_CH0_MAX_CLIP(255);
            SET_PIPE0_YUV_CODER_CH1_MAX_CLIP(255);
            SET_PIPE0_YUV_CODER_CH2_MAX_CLIP(255);
            
            SET_PIPE0_YUV_CODER_CB_FLOOR(g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i1);
            SET_PIPE0_YUV_CODER_CR_FLOOR(g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i2);
        }
        else
        {
            // pipe0 streaming RGB
            if (RgbToYuvCoder_IsCurrentPipeInputSource_RGB_UNPEAKED(u8_PipeNo))
            {
                SET_PIPE0_YUV_CODER_FORMAT_TO_RGB_UNPEAKED();
            }
            else
            {
                SET_PIPE0_YUV_CODER_FORMAT_TO_RGB_PEAKED();
            }


            SET_PIPE0_YUV_CODER_CH0_MAX_CLIP(RGBTOYUVCODER_MAX_INPUT_FROM_PIPE);
            SET_PIPE0_YUV_CODER_CH1_MAX_CLIP(RGBTOYUVCODER_MAX_INPUT_FROM_PIPE);
            SET_PIPE0_YUV_CODER_CH2_MAX_CLIP(RGBTOYUVCODER_MAX_INPUT_FROM_PIPE);

             // update offset vector...
          
            s16_CBFloor = g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i1 + g_CE_LumaOffset[u8_PipeNo].s16_LumaOffset;
            if(s16_CBFloor <= CE0_CBFLOOR_MIN)
            {
                s16_CBFloor = CE0_CBFLOOR_MIN;
                g_CE_LumaOffset[u8_PipeNo].u8_CBFloor_Saturated = Flag_e_TRUE;
            }
            else if(s16_CBFloor >= CE0_CBFLOOR_MAX)
            {
                s16_CBFloor = CE0_CBFLOOR_MAX;
                g_CE_LumaOffset[u8_PipeNo].u8_CBFloor_Saturated = Flag_e_TRUE;
            }
            else
            {
                s16_CRFloor = s16_CRFloor;
                g_CE_LumaOffset[u8_PipeNo].u8_CBFloor_Saturated = Flag_e_FALSE;
            }
            

            s16_CRFloor = g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i2 + g_CE_LumaOffset[u8_PipeNo].s16_LumaOffset;
            if(s16_CRFloor <= CE0_CRFLOOR_MIN)
            {
                s16_CRFloor = CE0_CRFLOOR_MIN;
                g_CE_LumaOffset[u8_PipeNo].u8_CRFloor_Saturated = Flag_e_TRUE;
            }
            else if(s16_CRFloor >= CE0_CRFLOOR_MAX)
            {
                s16_CRFloor = CE0_CRFLOOR_MAX;
                g_CE_LumaOffset[u8_PipeNo].u8_CRFloor_Saturated = Flag_e_TRUE;
            }
            else
            {
                s16_CRFloor = s16_CRFloor;
                g_CE_LumaOffset[u8_PipeNo].u8_CRFloor_Saturated = Flag_e_FALSE;
            }
            
            SET_PIPE0_YUV_CODER_CB_FLOOR(s16_CBFloor);
            SET_PIPE0_YUV_CODER_CR_FLOOR(s16_CRFloor);
        }


        // update matrix elements...
        SET_PIPE0_YUV_CODER_YCBCR_00(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w0_0);
        SET_PIPE0_YUV_CODER_YCBCR_01(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w0_1);
        SET_PIPE0_YUV_CODER_YCBCR_02(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w0_2);
        SET_PIPE0_YUV_CODER_YCBCR_10(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w1_0);
        SET_PIPE0_YUV_CODER_YCBCR_11(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w1_1);
        SET_PIPE0_YUV_CODER_YCBCR_12(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w1_2);
        SET_PIPE0_YUV_CODER_YCBCR_20(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w2_0);
        SET_PIPE0_YUV_CODER_YCBCR_21(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w2_1);
        SET_PIPE0_YUV_CODER_YCBCR_22(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w2_2);

        // update offset vector...
        s16_YFloor = g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i0 + g_CE_LumaOffset[u8_PipeNo].s16_LumaOffset;
        if(s16_YFloor <= CE0_YFLOOR_MIN){
            s16_YFloor = CE0_YFLOOR_MIN;
            g_CE_LumaOffset[u8_PipeNo].u8_YFloor_Saturated = Flag_e_TRUE;
        }
        else if(s16_YFloor >= CE0_YFLOOR_MAX){
            s16_YFloor = CE0_YFLOOR_MAX;
            g_CE_LumaOffset[u8_PipeNo].u8_YFloor_Saturated = Flag_e_TRUE;
        }
        else {
            s16_YFloor = s16_YFloor;
            g_CE_LumaOffset[u8_PipeNo].u8_YFloor_Saturated = Flag_e_FALSE;
        }

        SET_PIPE0_YUV_CODER_Y_FLOOR(s16_YFloor);
       // SET_PIPE0_YUV_CODER_CB_FLOOR(g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i1);
       // SET_PIPE0_YUV_CODER_CR_FLOOR(g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i2);
    }
    else
    {
        Set_ISP_yuvCoder_ce1_ENABLE(1, 0);  // enable,soft_reset
        if (RgbToYuvCoder_IsCurrentPipeOutputFormat_YUV422(u8_PipeNo))
        {
            // pipe1 streaming YUV
            SET_PIPE1_YUV_CODER_FORMAT_TO_YUV422();
            SET_PIPE1_YUV_CODER_CH0_MAX_CLIP(255);
            SET_PIPE1_YUV_CODER_CH1_MAX_CLIP(255);
            SET_PIPE1_YUV_CODER_CH2_MAX_CLIP(255);

            SET_PIPE1_YUV_CODER_CB_FLOOR(g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i1);
            SET_PIPE1_YUV_CODER_CR_FLOOR(g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i2);
        }
        else
        {
            // pipe1 streaming RGB
            if (RgbToYuvCoder_IsCurrentPipeInputSource_RGB_UNPEAKED(u8_PipeNo))
            {
                SET_PIPE1_YUV_CODER_FORMAT_TO_RGB_UNPEAKED();
            }
            else
            {
                SET_PIPE1_YUV_CODER_FORMAT_TO_RGB_PEAKED();
            }

            SET_PIPE1_YUV_CODER_CH0_MAX_CLIP(RGBTOYUVCODER_MAX_INPUT_FROM_PIPE);
            SET_PIPE1_YUV_CODER_CH1_MAX_CLIP(RGBTOYUVCODER_MAX_INPUT_FROM_PIPE);
            SET_PIPE1_YUV_CODER_CH2_MAX_CLIP(RGBTOYUVCODER_MAX_INPUT_FROM_PIPE);

             // update offset vector...
           
            s16_CBFloor = g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i1 + g_CE_LumaOffset[u8_PipeNo].s16_LumaOffset;
            if(s16_CBFloor <= CE1_CBFLOOR_MIN)
            {
                s16_CBFloor = CE1_CBFLOOR_MIN;
                g_CE_LumaOffset[u8_PipeNo].u8_CBFloor_Saturated = Flag_e_TRUE;
            }
            else if(s16_CBFloor >= CE1_CBFLOOR_MAX)
            {
                s16_CBFloor = CE1_CBFLOOR_MAX;
                g_CE_LumaOffset[u8_PipeNo].u8_CBFloor_Saturated = Flag_e_TRUE;
            }
            else
            {
                s16_CRFloor = s16_CRFloor;
                g_CE_LumaOffset[u8_PipeNo].u8_CBFloor_Saturated = Flag_e_FALSE;
            }
            

            s16_CRFloor = g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i2 + g_CE_LumaOffset[u8_PipeNo].s16_LumaOffset;
            if(s16_CRFloor <= CE1_CRFLOOR_MIN)
            {
                s16_CRFloor = CE1_CRFLOOR_MIN;
                g_CE_LumaOffset[u8_PipeNo].u8_CRFloor_Saturated = Flag_e_TRUE;
            }
            else if(s16_CRFloor >= CE1_CRFLOOR_MAX)
            {
                s16_CRFloor = CE1_CRFLOOR_MAX;
                g_CE_LumaOffset[u8_PipeNo].u8_CRFloor_Saturated = Flag_e_TRUE;
            }
            else
            {
                s16_CRFloor = s16_CRFloor;
                g_CE_LumaOffset[u8_PipeNo].u8_CRFloor_Saturated = Flag_e_FALSE;
            }
            
            SET_PIPE1_YUV_CODER_CB_FLOOR(s16_CBFloor);
            SET_PIPE1_YUV_CODER_CR_FLOOR(s16_CRFloor);
        }


        // update matrix elements...
        SET_PIPE1_YUV_CODER_YCBCR_00(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w0_0);
        SET_PIPE1_YUV_CODER_YCBCR_01(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w0_1);
        SET_PIPE1_YUV_CODER_YCBCR_02(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w0_2);
        SET_PIPE1_YUV_CODER_YCBCR_10(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w1_0);
        SET_PIPE1_YUV_CODER_YCBCR_11(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w1_1);
        SET_PIPE1_YUV_CODER_YCBCR_12(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w1_2);
        SET_PIPE1_YUV_CODER_YCBCR_20(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w2_0);
        SET_PIPE1_YUV_CODER_YCBCR_21(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w2_1);
        SET_PIPE1_YUV_CODER_YCBCR_22(g_CE_OutputCoderMatrix[u8_PipeNo].s16_w2_2);

        // update offset vector...
        s16_YFloor = g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i0 + g_CE_LumaOffset[u8_PipeNo].s16_LumaOffset;
        if(s16_YFloor <= CE1_YFLOOR_MIN)
        {
            s16_YFloor = CE1_YFLOOR_MIN;
            g_CE_LumaOffset[u8_PipeNo].u8_YFloor_Saturated = Flag_e_TRUE;
        }
        else if(s16_YFloor >= CE1_YFLOOR_MAX)
        {
            s16_YFloor = CE1_YFLOOR_MAX;
            g_CE_LumaOffset[u8_PipeNo].u8_YFloor_Saturated = Flag_e_TRUE;
        }
        else
        {
            s16_YFloor = s16_YFloor;
            g_CE_LumaOffset[u8_PipeNo].u8_YFloor_Saturated = Flag_e_FALSE;
        }

        SET_PIPE1_YUV_CODER_Y_FLOOR(s16_YFloor);
        //SET_PIPE1_YUV_CODER_CB_FLOOR(g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i1);
        //SET_PIPE1_YUV_CODER_CR_FLOOR(g_CE_OutputCoderOffsetVector[u8_PipeNo].s16_i2);
    }

    return;
}

