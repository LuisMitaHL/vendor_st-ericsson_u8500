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
        It is the main file which implements conversion matrix, manual contrast and saturation.
 \ingroup RgbToYuvCoder
 \endif
*/
#include "RgbToYuvCoder.h"
#   include "osttrace.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_RgbToYuvCoder_RgbToYuvCoderTraces.h"
#endif

static void                         FadeToBlackUpdate (uint8_t u8_PipeNo)TO_EXT_DDR_PRGM_MEM;
static void                         SetSignalRangesAndStockMatrix (uint8_t u8_PipeNo, uint8_t e_Transform_Type)TO_EXT_DDR_PRGM_MEM;
static void                         BloatYCbCrMatrix (float_t f_Contrast, float_t f_Saturation)TO_EXT_DDR_PRGM_MEM;
static void                         YCbCrToRgb (float_t f_Contrast, float_t f_Saturation)TO_EXT_DDR_PRGM_MEM;
static void                         FinaliseYCbCrMatrix (float_t f_Contrast, float_t f_Saturation)TO_EXT_DDR_PRGM_MEM;
#if 0
static int16_t                      GetRounded (float_t f_Input);
#endif
static void                         GenerateFixedPointVersion (void)TO_EXT_DDR_PRGM_MEM;
static void                         GetBloatedOffsetVector (uint8_t u8_PipeNo)TO_EXT_DDR_PRGM_MEM;

/// ITU-T Recomendation REC.601
/// Refer: http://en.wikipedia.org/wiki/YCbCr
/// u16_LumaExcursion = 235 - 16 = 219
/// u16_LumaMidpointTimes2 = (235 + 16 )  = 251
/// u16_ChromaExcursion    = 240 - 16 = 224
/// u16_ChromaMidpointTimes2 = (240 + 16 ) = 256
const RgbToYuvOutputSignalRange_ts  YuvRec601OutputSignalRange = { 219, 251, 224, 256 };

/// ITU-T Recomendation REC.709
/// Refer: http://en.wikipedia.org/wiki/YCbCr
/// u16_LumaExcursion = 235 - 16 = 219
/// u16_LumaMidpointTimes2 = (235 + 16 )  = 251
/// u16_ChromaExcursion    = 240 - 16 = 224
/// u16_ChromaMidpointTimes2 = (240 + 16 ) = 256
const RgbToYuvOutputSignalRange_ts  YuvRec709OutputSignalRange = { 219, 251, 224, 256 };

/// Full Range JFIF YUV format
const RgbToYuvOutputSignalRange_ts  YuvJfifOutputSignalRange = { 255, 256, 255, 256 };

/// Full Range BT Rec709 format
const RgbToYuvOutputSignalRange_ts  YuvRec709OutputSignalRangeFull = { 255, 256, 255, 256 };

/// ISP8500 give 10 bit RGB data, so maximum data will be 1023 range
const RgbToYuvOutputSignalRange_ts  RgbOutputSignalRange =
{
    RGBTOYUVCODER_MAX_INPUT_FROM_PIPE,
    RGBTOYUVCODER_MAX_INPUT_FROM_PIPE,
    RGBTOYUVCODER_MAX_INPUT_FROM_PIPE,
    RGBTOYUVCODER_MAX_INPUT_FROM_PIPE
};

/// Standard YCbCr Conversion matrix for REC601
const float_t    f_RgbToYuvStockMatrix_REC601[9] = { 0.299, 0.587, 0.114, -0.169, -0.331, 0.5, 0.5, -0.419, -0.081 };

/// Standard YCbCr Conversion matrix for REC709
const float_t    f_RgbToYuvStockMatrix_REC709[9] = { 0.2126, 0.7152, 0.0722, -0.1146, -0.3854, 0.5, 0.5, -0.4542, -0.0458 };

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
float_t                             f_OutputMatrixView[6];

// Global pointers
// Pointer to Signal matrix based on Output format
RgbToYuvOutputSignalRange_ts        *ptr_PipeRgbToYUVSignalRange = ( RgbToYuvOutputSignalRange_ts * ) NULL;

// Global pointers
// Pointer to correct YUV stock matrix based on selected standard
float *ptr_YuvStockMatrix = ( float * ) NULL;

// Pointer to Current Pipe Offset Matrix
int16_t                             *ptrs16_OffsetVector = ( int16_t * ) NULL;

// Pointer to Current Pipe output YUV matrix
int16_t                             *ptrs16_RgbToYuvMatrix = ( int16_t * ) NULL;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void RgbToYuvCoder_Run (uint8_t u8_PipeNo)
 \brief Calculate Matrices to be programmed in the hardware
 \details called from VID and in first frame
 \param u8_PipeNo: Pipe No in ISP
 \return Flag_e_TRUE(Success), Flag_e_FALSE(Failure)
 \callgraph
 \callergraph
 \ingroup RgbToYuvCoder
 \endif
*/
void
RgbToYuvCoder_Run(
uint8_t u8_PipeNo)
{
#if ENABLE_YUVCODER_TRACES
    uint32_t count = 0;
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder> >> RgbToYuvCoder_Run : pipe no. = %u\n", u8_PipeNo);

    OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  g_CE_YUVCoderControls[%u].e_Transform_Type = %u\n", u8_PipeNo, g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type);
    OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  g_CE_YUVCoderControls[%u].u8_Contrast = %u\n", u8_PipeNo, g_CE_YUVCoderControls[u8_PipeNo].u8_Contrast);
    OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  g_CE_YUVCoderControls[%u].u8_MaxContrast = %u\n", u8_PipeNo, g_CE_YUVCoderControls[u8_PipeNo].u8_MaxContrast);
    OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  g_CE_YUVCoderControls[%u].u8_ColourSaturation = %u\n", u8_PipeNo, g_CE_YUVCoderControls[u8_PipeNo].u8_ColourSaturation);
    OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  g_CE_YUVCoderControls[%u].u8_MaxColourSaturation = %u\n", u8_PipeNo, g_CE_YUVCoderControls[u8_PipeNo].u8_MaxColourSaturation);
#endif

    FadeToBlackUpdate(u8_PipeNo);

    // Update the signal range based on transform type
    SetSignalRangesAndStockMatrix(u8_PipeNo, g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type);

    ptrs16_RgbToYuvMatrix = ( int16_t * ) &g_CE_OutputCoderMatrix[u8_PipeNo];

    g_YUVCoderStatus[u8_PipeNo].f_Contrast = ((float_t) g_CE_YUVCoderControls[u8_PipeNo].u8_Contrast)/100;

    // reduce the user contrast setting depending on the fade to black damper
    g_YUVCoderStatus[u8_PipeNo].f_Contrast *= g_CE_FadeToBlack[u8_PipeNo].f_DamperOutput;

    g_YUVCoderStatus[u8_PipeNo].f_Saturation = ((float_t) g_CE_YUVCoderControls[u8_PipeNo].u8_ColourSaturation) /100;

    // Clip contrast against the maximum contrast
    // FIXME comment : <Hem> why are we comparing f_Contrast which is a float value against u8_MaxContrast which is X100 times of contrast
    if (g_YUVCoderStatus[u8_PipeNo].f_Contrast > g_CE_YUVCoderControls[u8_PipeNo].u8_MaxContrast)
    {
        g_YUVCoderStatus[u8_PipeNo].f_Contrast = (float_t) g_CE_YUVCoderControls[u8_PipeNo].u8_MaxContrast;
    }

    // Clip saturation against the maximum saturation
    // FIXME comment : <Hem> same comment as above for saturation also
    if (g_YUVCoderStatus[u8_PipeNo].f_Saturation > g_CE_YUVCoderControls[u8_PipeNo].u8_MaxColourSaturation)
    {
        g_YUVCoderStatus[u8_PipeNo].f_Saturation = (float_t) g_CE_YUVCoderControls[u8_PipeNo].u8_MaxColourSaturation;
    }

#if ENABLE_YUVCODER_TRACES
    OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  g_YUVCoderStatus[%u].f_Contrast = %u\n", u8_PipeNo, g_YUVCoderStatus[u8_PipeNo].f_Contrast);
    OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  g_YUVCoderStatus[%u].f_Saturation = %u\n", u8_PipeNo, g_YUVCoderStatus[u8_PipeNo].f_Saturation);
    OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  (Damper value should be 1)  g_CE_FadeToBlack[%u].f_DamperOutput = %u\n", u8_PipeNo, g_CE_FadeToBlack[u8_PipeNo].f_DamperOutput);
#endif


    if (g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type >= Transform_e_Rgb_Standard)
    {
        // this case will not be envountered if output format is YUV
#if ENABLE_YUVCODER_TRACES
        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  output format is RGB\n");
#endif
        YCbCrToRgb(g_YUVCoderStatus[u8_PipeNo].f_Contrast, g_YUVCoderStatus[u8_PipeNo].f_Saturation);
    }
    else
    {
        // RGB to YCbCr
        // The function generate only first 2 columns of the output matrix. It apply the final contrast and
        // saturation to the output matrix
        // row in floating format for contrast and saturation.
        BloatYCbCrMatrix(g_YUVCoderStatus[u8_PipeNo].f_Contrast, g_YUVCoderStatus[u8_PipeNo].f_Saturation);

        // create 3rd element of each op row. Each rwo of YCbCr matrix has some finite sum. So 3rd element
        // can be computed by substracting first two element sum from total sum.
        FinaliseYCbCrMatrix(g_YUVCoderStatus[u8_PipeNo].f_Contrast, g_YUVCoderStatus[u8_PipeNo].f_Saturation);
    }


    ptrs16_OffsetVector = ( int16_t * ) &g_CE_OutputCoderOffsetVector[u8_PipeNo];

    GetBloatedOffsetVector(u8_PipeNo);

#if ENABLE_YUVCODER_TRACES
        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  << RgbToYuvCoder_Run\n");
#endif

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void FadeToBlackUpdate(uint8_t u8_PipeNo)
 \brief Calculate Damper value if Fade to black feature is enabled
 \details
 \param u8_PipeNo: Pipe No in ISP
 \return void
 \callgraph
 \callergraph
 \ingroup RgbToYuvCoder
 \endif
*/
void
FadeToBlackUpdate(
uint8_t u8_PipeNo)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    float_t f_DamperValue = 1.0;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    // determine the fade to black damper value at this point to allow
    // a sensible decision to be made wrt running the entire rgbToYuv
    // algorithm.
    //
    // if fDisable is set the op from the damper will prob be different
    // the the previous pass for one more pass of the sytem. then both
    // f_DamperOutput and .fpDamperOutput will ==. this should not be
    // a problem....
#ifdef RGBTOYUV_FADETOBLACK
    f_DamperValue = Damper_Make(
        g_CE_FadeToBlack[u8_PipeNo].f_DamperLowThreshold,
        DefaultMaximumDamperOutput,
        g_CE_FadeToBlack[u8_PipeNo].f_DamperHighThreshold,
        g_CE_FadeToBlack[u8_PipeNo].f_BlackValue,
        g_CE_FadeToBlack[u8_PipeNo].e_Flag_Disable);
#endif // RGBTOYUV_FADETOBLACK

    // update the page with the new damper value.
    g_CE_FadeToBlack[u8_PipeNo].f_DamperOutput = f_DamperValue;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void SetSignalRangesAndStockMatrix (uint8_t u8_PipeNo, uint8_t e_Transform_Type)
 \brief Update signal range based on output required from pipe
 \details   Excursion and midpoint of output parameters are effected by the selection of YUV/RGB transform and
            flavour of corresponding transform. Update the pointer to right signal range.
 \param u8_PipeNo: Pipe No in ISP
 \param e_Transform_Type: Output format requested in the PIPE
 \return void
 \callgraph
 \callergraph
 \ingroup RgbToYuvCoder
 \endif
*/
void
SetSignalRangesAndStockMatrix(
uint8_t u8_PipeNo,
uint8_t e_Transform_Type)
{
#if ENABLE_YUVCODER_TRACES
        uint32_t count = 0;
        OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  >> SetSignalRangesAndStockMatrix >> u8_PipeNo = %u, e_Transform_Type = %u\n", u8_PipeNo, e_Transform_Type);
#endif

    // By default keep standard as REC601 for all the cases
    ptr_YuvStockMatrix = (float *) f_RgbToYuvStockMatrix_REC601;

    if (Transform_e_YCbCr_JFIF == e_Transform_Type)
    {
        ptr_PipeRgbToYUVSignalRange = ( RgbToYuvOutputSignalRange_ts * ) &YuvJfifOutputSignalRange;
    }
    else if (Transform_e_YCbCr_Rec601 == e_Transform_Type)
    {
        ptr_PipeRgbToYUVSignalRange = ( RgbToYuvOutputSignalRange_ts * ) &YuvRec601OutputSignalRange;
    }
    else if(Transform_e_YCbCr_Rec709_FULL_RANGE == e_Transform_Type)
    {
        OstTraceInt0(TRACE_DEBUG, "Transform_e_YCbCr_Rec709_FULL_RANGE \n");
        ptr_PipeRgbToYUVSignalRange =  ( RgbToYuvOutputSignalRange_ts * ) &YuvRec709OutputSignalRangeFull;
        ptr_YuvStockMatrix = (float *) f_RgbToYuvStockMatrix_REC709;
    }
    else if (Transform_e_YCbCr_Rec709 == e_Transform_Type)
    {
        OstTraceInt0(TRACE_DEBUG, "Transform_e_YCbCr_Rec709 \n");
        ptr_PipeRgbToYUVSignalRange = ( RgbToYuvOutputSignalRange_ts * ) &YuvRec709OutputSignalRange;
        ptr_YuvStockMatrix = (float *) f_RgbToYuvStockMatrix_REC709;
    }
    else if (Transform_e_YCbCr_Custom == e_Transform_Type)
    {
        OstTraceInt0(TRACE_DEBUG, "Transform_e_YCbCr_Custom \n");
        ptr_PipeRgbToYUVSignalRange = ( RgbToYuvOutputSignalRange_ts * ) &g_CE_CustomTransformOutputSignalRange[u8_PipeNo];
        ptr_YuvStockMatrix = (float *)&g_CustomStockMatrix[u8_PipeNo].f_StockMatrix[0];
    }
    else if (Transform_e_Rgb_Standard == e_Transform_Type)
    {
        ptr_PipeRgbToYUVSignalRange = ( RgbToYuvOutputSignalRange_ts * ) &RgbOutputSignalRange;
    }
    else
    {
        ptr_PipeRgbToYUVSignalRange = ( RgbToYuvOutputSignalRange_ts * ) &g_CE_CustomTransformOutputSignalRange[u8_PipeNo];
    }

#if ENABLE_YUVCODER_TRACES
        for(count=0; count<9; count++)
        {
            OstTraceInt4(TRACE_DEBUG, "ptr_YuvStockMatrix[%u] = %f, f_RgbToYuvStockMatrix_REC709[%u] = %f \n", count, ptr_YuvStockMatrix[count], count, f_RgbToYuvStockMatrix_REC709[count]);
        }
        OstTraceInt4(TRACE_DEBUG, "Signal range: %u %u %u %u\n", g_CE_CustomTransformOutputSignalRange[u8_PipeNo].u16_LumaExcursion, g_CE_CustomTransformOutputSignalRange[u8_PipeNo].u16_LumaMidpointTimes2, g_CE_CustomTransformOutputSignalRange[u8_PipeNo].u16_ChromaExcursion, g_CE_CustomTransformOutputSignalRange[u8_PipeNo].u16_ChromaMidpointTimes2);
        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  << SetSignalRangesAndStockMatrix \n");
#endif
    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void BloatYCbCrMatrix(float_t f_Contrast, float_t f_Saturation)
 \brief The function calculate first 2 columns of each row from 'stock' matrix by appling contrast
        and saturation to it.
 \details:
        [x x 0] = [x x x] * f_Contrast
        [x x 0] = [x x x] * f_Contrast * f_Saturation
        [x x 0] = [x x x] * f_Contrast * f_Saturation
        The above calculated values are stored in 6 element matrix

 \param f_Contrast: Contrast value in floating point
 \param f_Saturation: Saturation value in floating point
 \return void
 \callgraph
 \callergraph
 \ingroup RgbToYuvCoder
 \endif
*/
void
BloatYCbCrMatrix(
float_t f_Contrast,
float_t f_Saturation)
{
    /*~~~~~~~~~~~~~~~~~~~~*/
    // this function implements the actual bloating function, it
    // makes no checks for the validity of the input variables
    // to the bloating process and is normally called via a
    // wrapper function
    // the basic bloating function is to process an RGB to YCbCr
    // matrix according to the contrast and saturation controls
    // the if the output is RGB multiply this by the inverse of
    // the original function (e.g. YCbCr to RGB)...
    // we start with a 'stock' RGB to YCbCr conversion matrix, which
    // we 'bloat' in order to alter the conversion in such a way as
    // to implement our required contrast & colour saturation
    // adjustments...
    float_t f_lumaScaleFactor = 0;
    float_t f_chromaScaleFactor = 0;
    //f_ChromaScaler;
    /*~~~~~~~~~~~~~~~~~~~~*/

    // Luminance
    // ---------
    // dDesiredLumaRange = (m_adOutputMax[0] - m_adOutputMin[0]) * dLumaScaler;
    // dLumaElementScaler = dDesiredLumaRange / dLumaRange;
#if ENABLE_YUVCODER_TRACES
    uint32_t count = 0;
    OstTraceInt0(TRACE_DEBUG, "<yuvcoder> >> BloatYCbCrMatrix\n");

    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_Contrast = %f\n", f_Contrast);
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_Saturation = %f\n", f_Saturation);
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  u16_LumaExcursion = %f\n", ptr_PipeRgbToYUVSignalRange->u16_LumaExcursion);
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  u16_ChromaExcursion = %f\n", ptr_PipeRgbToYUVSignalRange->u16_ChromaExcursion);
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  luma_coef = %f\n", ((float_t)F_LUMA_RANGE)/(((float_t) (ptr_PipeRgbToYUVSignalRange->u16_LumaExcursion)) * f_Contrast));
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  chroma_coef = %f\n", ((float_t)F_CHROMA_RANGE)/(((float_t) (ptr_PipeRgbToYUVSignalRange->u16_ChromaExcursion)) * f_Contrast * f_Saturation));
 #endif
#if 0
    f_lumaScaleFactor = ((float_t)F_LUMA_RANGE)/(((float_t) (ptr_PipeRgbToYUVSignalRange->u16_LumaExcursion)) * f_Contrast);
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_lumaScaleFactor_1 = %f\n", f_lumaScaleFactor);
    f_lumaScaleFactor = 1.0f/f_lumaScaleFactor;
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_lumaScaleFactor_2 = %f\n", f_lumaScaleFactor);

    // Output matrix = stock matrix * Scalar
    f_OutputMatrixView[0] = ptr_YuvStockMatrix[0] * f_lumaScaleFactor;
    f_OutputMatrixView[1] = ptr_YuvStockMatrix[1] * f_lumaScaleFactor;

    // Chrominance
    // -----------
    // double dChromaScaler = dLumaScaler * ((double)iSaturation / 100.0);
    // f_ChromaScaler = f_Contrast * f_Saturation;

    // dDesiredChromaRange = (m_adOutputMax[1] - m_adOutputMin[1]) * dChromaScaler;
    // dChromaElementScaler = dDesiredChromaRange / dBluChromaRange;
    f_chromaScaleFactor = ((float_t)F_CHROMA_RANGE)/(((float_t) (ptr_PipeRgbToYUVSignalRange->u16_ChromaExcursion)) * f_Contrast * f_Saturation);
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_chromaScaleFactor_1 = %f\n", f_chromaScaleFactor);
    f_chromaScaleFactor = 1.0f/f_chromaScaleFactor;
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_chromaScaleFactor_2 = %f\n", f_chromaScaleFactor);

    // Cb
    // --
    f_OutputMatrixView[2] = ptr_YuvStockMatrix[3] * f_chromaScaleFactor;
    f_OutputMatrixView[3] = ptr_YuvStockMatrix[4] * f_chromaScaleFactor;

    // Cr
    // --
    f_OutputMatrixView[4] = ptr_YuvStockMatrix[6] * f_chromaScaleFactor;
    f_OutputMatrixView[5] = ptr_YuvStockMatrix[7] * f_chromaScaleFactor;
#else
    f_lumaScaleFactor = ((float_t)F_LUMA_RANGE)/(((float_t) (ptr_PipeRgbToYUVSignalRange->u16_LumaExcursion)) * f_Contrast);
#if ENABLE_YUVCODER_TRACES
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_lumaScaleFactor_1 = %f\n", f_lumaScaleFactor);
   // f_lumaScaleFactor = 1.0f/f_lumaScaleFactor;
    //OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_lumaScaleFactor_2 = %f\n", f_lumaScaleFactor);
#endif
    // Output matrix = stock matrix * Scalar
    f_OutputMatrixView[0] = ptr_YuvStockMatrix[0] / f_lumaScaleFactor;
    f_OutputMatrixView[1] = ptr_YuvStockMatrix[1] / f_lumaScaleFactor;

    // Chrominance
    // -----------
    // double dChromaScaler = dLumaScaler * ((double)iSaturation / 100.0);
    // f_ChromaScaler = f_Contrast * f_Saturation;

    // dDesiredChromaRange = (m_adOutputMax[1] - m_adOutputMin[1]) * dChromaScaler;
    // dChromaElementScaler = dDesiredChromaRange / dBluChromaRange;
    f_chromaScaleFactor = ((float_t)F_CHROMA_RANGE)/(((float_t) (ptr_PipeRgbToYUVSignalRange->u16_ChromaExcursion)) * f_Contrast * f_Saturation);
#if ENABLE_YUVCODER_TRACES
    OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_chromaScaleFactor_1 = %f\n", f_chromaScaleFactor);
    //f_chromaScaleFactor = 1.0f/f_chromaScaleFactor;
    //OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_chromaScaleFactor_2 = %f\n", f_chromaScaleFactor);
#endif
    // Cb
    // --
    f_OutputMatrixView[2] = ptr_YuvStockMatrix[3] / f_chromaScaleFactor;
    f_OutputMatrixView[3] = ptr_YuvStockMatrix[4] / f_chromaScaleFactor;

    // Cr
    // --
    f_OutputMatrixView[4] = ptr_YuvStockMatrix[6] / f_chromaScaleFactor;
    f_OutputMatrixView[5] = ptr_YuvStockMatrix[7] / f_chromaScaleFactor;
#endif
#if ENABLE_YUVCODER_TRACES
    OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  f_lumaScaleFactor = %f, f_chromaScaleFactor = %f\n", f_lumaScaleFactor, f_chromaScaleFactor);
    for(count = 0; count <6; count++)
    {
        OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  f_OutputMatrixView[%u] = %f\n", count, f_OutputMatrixView[count]);
    }

    for(count = 0; count <9; count++)
    {
        OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  ptr_YuvStockMatrix[%u] = %f\n", count, ptr_YuvStockMatrix[count]);
    }

    OstTraceInt0(TRACE_DEBUG, "<yuvcoder> << BloatYCbCrMatrix\n");
#endif

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void FinaliseYCbCrMatrix(float_t f_Contrast, float_t f_Saturation)
 \brief The function calculate last element of each row by subtracting sum of first two element from ideal sum.
 \details:
 \param f_Contrast: Contrast value in floating point
 \param f_Saturation: Saturation value in floating point
 \return void
 \callgraph
 \callergraph
 \ingroup RgbToYuvCoder
 \endif
*/
void
FinaliseYCbCrMatrix(
float_t f_Contrast,
float_t f_Saturation)
{
    float_t f_OutputExcursionDividedByInputExcursion;
    float_t f_Scaler;
    float_t f_RowSumLuma;
    int16_t s16_IdealLastElement;
    int16_t s16_IdealRowSums[3];
    int16_t s16_RowSum;
    uint8_t u8_Element;
    uint8_t u8_Row;

#if ENABLE_YUVCODER_TRACES
        uint32_t count = 0;

        OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  >> FinaliseYCbCrMatrix : v = %f, f_Saturation = %f\n", f_Contrast, f_Saturation);
#endif

    s16_IdealRowSums[0] = s16_IdealRowSums[1] = s16_IdealRowSums[2] = 0;

    // Convert Martix in floating point to Fixed point format
    GenerateFixedPointVersion();

    // double dOutputExcursionDividedByInputExcursion = dDesiredLumaExcursion / dInputMax;
    // <Hem> We are assuming that excursion for luma and chroma will be same. Is it a good asumption ?
    f_OutputExcursionDividedByInputExcursion = (float_t) (ptr_PipeRgbToYUVSignalRange->u16_LumaExcursion) / RGBTOYUVCODER_MAX_INPUT_FROM_PIPE;

    // all calculations are in Fixed point format, so multiply by Matrix scaler
    // double dScaler = MatrixScaler * ((double)m_iContrast/100.0);
    f_Scaler = F_MATRIX_SCALER * f_Contrast;

    // double dRowSum = dOutputExcursionDividedByInputExcursion * dScaler;
    f_RowSumLuma = f_OutputExcursionDividedByInputExcursion * f_Scaler;

    // updating only element 0 as in YCbCr matrix, only Row[0] sum has finite value
    // Row[1] and Row[2] sum are always zero as they are difference signals
    s16_IdealRowSums[0] = GetRounded(f_RowSumLuma);

    for (u8_Row = 0; u8_Row < 3; u8_Row++)
    {
        s16_RowSum = 0;

        // the ideal 'last' element is simply the ideal row sum
        // minus the other two elements...
        //
        s16_IdealLastElement = s16_IdealRowSums[u8_Row];

        for (u8_Element = 0; u8_Element < 2; ++u8_Element)
        {
            s16_RowSum += ptrs16_RgbToYuvMatrix[(u8_Row * 3) + u8_Element];
        }

        ptrs16_RgbToYuvMatrix[(u8_Row * 3) + 2] = s16_IdealRowSums[u8_Row] - s16_RowSum;

#if ENABLE_YUVCODER_TRACES
        OstTraceInt3(TRACE_DEBUG,"<yuvcoder> s16_RowSum = %+d, s16_IdealRowSums[%d] = %+d\n", s16_RowSum, u8_Row, s16_IdealRowSums[u8_Row]);
        OstTraceInt2(TRACE_DEBUG,"<yuvcoder> ptrs16_RgbToYuvMatrix[%d] = %+d\n", (u8_Row * 3) + 2, ptrs16_RgbToYuvMatrix[(u8_Row * 3) + 2]);
#endif
    }

#if ENABLE_YUVCODER_TRACES
        OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  f_OutputExcursionDividedByInputExcursion = %f\n", f_OutputExcursionDividedByInputExcursion);
        for (count =0; count <3; count++)
        {
            OstTraceInt1(TRACE_DEBUG, "<yuvcoder>  s16_IdealRowSums[count] = %u\n", s16_IdealRowSums[count]);
        }

        for (count =0; count <9; count++)
        {
            OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  ptrs16_RgbToYuvMatrix[%u] = %d",count, ptrs16_RgbToYuvMatrix[count]);
        }

        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  << FinaliseYCbCrMatrix\n");
#endif

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void GenerateFixedPointVersion(void)
 \brief The function calculate Hardware output to be programmed from contrast applied matrix
 \details: The function convert floating values into fixed point format
 \param void
 \return void
 \callgraph
 \callergraph
 \ingroup RgbToYuvCoder
 \endif
*/
void
GenerateFixedPointVersion(void)
{
    float_t fpScaled;
    uint8_t u8_TargetCount,
    u8_SourceCount = 0;

#if ENABLE_YUVCODER_TRACES
    OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  >> GenerateFixedPointVersion\n");
#endif

    // copy correct parts of 6 item array into 9 element array
    for (u8_TargetCount = 0; u8_TargetCount < 9; u8_TargetCount++)
    {
#if ENABLE_YUVCODER_TRACES
        OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  f_OutputMatrixView[%u] = %f\n", u8_SourceCount, f_OutputMatrixView[u8_SourceCount]);
#endif
        // f_OutputMatrixView has only 6 elements so output matrix column 3 is invalid
        if (!(2 == u8_TargetCount || 5 == u8_TargetCount || 8 == u8_TargetCount))
        {
            // round and convert to int16 every array element
            fpScaled = F_MATRIX_SCALER * f_OutputMatrixView[u8_SourceCount++];
            ptrs16_RgbToYuvMatrix[u8_TargetCount] = GetRounded(fpScaled);
#if ENABLE_YUVCODER_TRACES
            OstTraceInt3(TRACE_DEBUG, "<yuvcoder>  fpScaled = %f, ptrs16_RgbToYuvMatrix[%u] = %+d\n", fpScaled, u8_TargetCount, ptrs16_RgbToYuvMatrix[u8_TargetCount]);
#endif
        }
    }

#if ENABLE_YUVCODER_TRACES
    OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  << GenerateFixedPointVersion\n");
#endif
    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void YCbCrToRgb(float_t f_Contrast,float_t f_Saturation)
 \brief Used to convert fpOutputMatrix[][] to Rgb format
             by multiplying it by the inverse of the stock
             rgbToYuv.  This will result in the identity matrix
             if contrast = saturation = 100%.
 \details
 \param f_Contrast: Contrast value in floating point
 \param f_Saturation: Saturation value in floating point
 \return void
 \callgraph
 \callergraph
 \ingroup RgbToYuvCoder
 \endif
*/
void
YCbCrToRgb(
float_t f_Contrast,
float_t f_Saturation)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float_t f_CommonFactor;
    float_t f_OnDiagonalByCol[3];
    float_t f_OffDiagonalByCol[3];
    float_t f_OutputExcursion;
    float_t f_Tmp;
    int16_t s16_OffsetIntegerOffDiagonalsByCol[3];
    int16_t s16_OffsetIntegerExcursions[3];
    int16_t s16_OffsetIntegerOnDiagonal[3];
    int16_t s16_Element;
    uint8_t u8_Col,
            u8_Row;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if ENABLE_YUVCODER_TRACES
        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  >> YCbCrToRgb\n");
#endif

    s16_OffsetIntegerOnDiagonal[0] = s16_OffsetIntegerOnDiagonal[1] = s16_OffsetIntegerOnDiagonal[2] = 0;
    for (u8_Col = 0; u8_Col < 3; ++u8_Col)
    {
        //  double dCommonFactor = dContrast * (1-dSaturation) * Luma.m_adElements[iCol];
        f_CommonFactor = f_Contrast * (1 - f_Saturation) * ptr_YuvStockMatrix[u8_Col];

        //  adOffDiagonalByCol[iCol] = dCommonFactor;
        f_OffDiagonalByCol[u8_Col] = f_CommonFactor;

        //  adOnDiagonal[iCol] = dCommonFactor + dContrast * dSaturation;
        f_OnDiagonalByCol[u8_Col] = f_CommonFactor + f_Contrast * f_Saturation;
    }


    // we can now determine the expected row midpoints
    // (or rather the excursions = midpoint * 2)...
    // as the f_Excursion is the same for each row we just do one calculation.
    // f_OutputExcursion = f_OnDiagonalByCol[0] + f_OffDiagonalByCol[1] + f_OffDiagonalByCol[2];
    //
    f_OutputExcursion = f_OnDiagonalByCol[0] + f_OffDiagonalByCol[1] + f_OffDiagonalByCol[2];

    // we now loop through each col/row generating reduced precision versions
    // of the off-diagonal elements and the excursions (by scaling according
    // to the desired precision then a rounding cast to integer)...
    //
    for (u8_Col = 0; u8_Col < 3; u8_Col++)
    {
        //s16_OffsetIntegerOffDiagonalsByCol[i] = iGetRounded( MatrixScaler * adOffDiagonalByCol[i] );
        f_Tmp = F_MATRIX_SCALER * f_OffDiagonalByCol[u8_Col];
        s16_OffsetIntegerOffDiagonalsByCol[u8_Col] = GetRounded(f_Tmp);

        //s16_OffsetIntegerExcursions[i] = iGetRounded( m_dMatrixScaler * adOutputExcursions[i] )
        f_Tmp = F_MATRIX_SCALER * f_OutputExcursion;
        s16_OffsetIntegerExcursions[u8_Col] = GetRounded(f_Tmp);
    }


    // loop through each row generating reduced precision on-diagonal
    // elements that satisfy the desired reduced precision excursions...
    for (u8_Row = 0; u8_Row < 3; u8_Row++)
    {
        for (u8_Col = 0; u8_Col < 3; u8_Col++)
        {
            s16_OffsetIntegerOnDiagonal[u8_Row] += (u8_Row == u8_Col) ? s16_OffsetIntegerExcursions[u8_Col] : -s16_OffsetIntegerOffDiagonalsByCol[u8_Col];
        }
    }


    // using the observed mirror image about the diagonal for
    // the op rgb matrix create the matrix.
    for (u8_Row = 0; u8_Row < 3; u8_Row++)
    {
        for (u8_Col = 0; u8_Col < 3; u8_Col++)
        {
            s16_Element = (u8_Row == u8_Col) ? s16_OffsetIntegerOnDiagonal[u8_Col] : s16_OffsetIntegerOffDiagonalsByCol[u8_Col];
            ptrs16_RgbToYuvMatrix[(u8_Row * 3) + u8_Col] = s16_Element;
        }
    }

#if ENABLE_YUVCODER_TRACES
        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  << YCbCrToRgb\n");
#endif

    return;
}

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void GetBloatedOffsetVector(uint8_t u8_PipeNo)
 \brief determine the offset vector using the user defined
             midpointtimes2 and the already calculated matrix
             coefficients....
 \details
 \param u8_PipeNo: Pipe No in ISP
 \return void
 \callgraph
 \callergraph
 \ingroup RgbToYuvCoder
 \endif
*/
void
GetBloatedOffsetVector(
uint8_t u8_PipeNo)
{
    // formula:
    // for luma row    (luma_midpointx2 - sum(row0))/2
    // for chroma rows (chroma_midpointx2 - sum(row1|2))/2
    uint16_t    u16_MidpointTimes2;
    int16_t     s16_RowSum;
    int16_t     s16_Tmp;
    uint8_t     u8_Row;
    uint8_t     u8_Col;

#if ENABLE_YUVCODER_TRACES
        uint32_t count = 0;
        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  >> GetBloatedOffsetVector\n");
#endif

    for (u8_Row = 0; u8_Row < 3; ++u8_Row)
    {
        // determine the midpoint to be used for this row
        if (0 == u8_Row)    // Luma Row or Y component in YCbCr
        {
            u16_MidpointTimes2 = ptr_PipeRgbToYUVSignalRange->u16_LumaMidpointTimes2;
        }
        else                // Chroma row are 1 and 2
        {
            u16_MidpointTimes2 = ptr_PipeRgbToYUVSignalRange->u16_ChromaMidpointTimes2;
        }

        // now determine the sum of the current row.
        s16_RowSum = 0;
        for (u8_Col = 0; u8_Col < 3; ++u8_Col)
        {
            s16_RowSum += ptrs16_RgbToYuvMatrix[(u8_Row * 3) + u8_Col];
        }

        // adjust midpoint depending on fade to black damper.
        // RowSum has all the gains in it.
        s16_Tmp = ((int16_t) u16_MidpointTimes2 - s16_RowSum) / 2;

        // in case of yuv out the 'Y' must be damped for fade 2 black.
        // for rgb out all rows must be damped.
        // if rgb out all rows must be damped.
        if ((g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type >= Transform_e_Rgb_Standard) || (0 == u8_Row))   // if yuv & 'Y' or rgb & 'Y' we have to do the work
        {
#if ENABLE_YUVCODER_TRACES
        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  g_CE_YUVCoderControls[u8_PipeNo].e_Transform_Type >= Transform_e_Rgb_Standard\n");
#endif
            ptrs16_OffsetVector[u8_Row] = (int16_t) (g_CE_FadeToBlack[u8_PipeNo].f_DamperOutput * s16_Tmp);
        }
        else
        {
            ptrs16_OffsetVector[u8_Row] = s16_Tmp;
        }
    }

#if ENABLE_YUVCODER_TRACES
        for(count = 0; count <3; count++)
        {
            OstTraceInt2(TRACE_DEBUG, "<yuvcoder>  ptrs16_OffsetVector[%d] = %d\n", count, ptrs16_OffsetVector[count]);
        }

        OstTraceInt0(TRACE_DEBUG, "<yuvcoder>  << GetBloatedOffsetVector\n");
#endif

    return;
}

