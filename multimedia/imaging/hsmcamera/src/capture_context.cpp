/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "capture_context.h"

CCapture_context::CCapture_context()
{
    nOmxBufHdrReadCount = 0;
    nOmxBufHdrWriteCount = 0;
    
    for (int i=0 ; i<MAX_ALLOCATED_BUFFER_NB ; i++)
    {
        ppOmxBufHdr[i] = 0;
    }

    nBMS_capture_context.Zoom_Control_s16_CenterOffsetX = 0;
    nBMS_capture_context.Zoom_Control_s16_CenterOffsetY = 0;
    nBMS_capture_context.Zoom_Control_f_SetFOVX = 0;
    nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVX = 0;
    nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVY = 0;
    nBMS_capture_context.SensorFrameConstraints_u16_MaxOPXOutputSize = 0;
    nBMS_capture_context.SensorFrameConstraints_u16_MaxOPYOutputSize = 0;
    nBMS_capture_context.SensorFrameConstraints_u16_VTXAddrMin = 0;
    nBMS_capture_context.SensorFrameConstraints_u16_VTYAddrMin = 0;
    nBMS_capture_context.SensorFrameConstraints_u16_VTXAddrMax = 0;
    nBMS_capture_context.SensorFrameConstraints_u16_VTYAddrMax = 0;
    nBMS_capture_context.CurrentFrameDimension_f_PreScaleFactor = 0;
    nBMS_capture_context.CurrentFrameDimension_u16_VTXAddrStart = 0;
    nBMS_capture_context.CurrentFrameDimension_u16_VTYAddrStart = 0;
    nBMS_capture_context.CurrentFrameDimension_u16_VTXAddrEnd = 0;
    nBMS_capture_context.CurrentFrameDimension_u16_VTYAddrEnd = 0;
    nBMS_capture_context.PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun = 0;
    nBMS_capture_context.PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun = 0;
    nBMS_capture_context.CurrentFrameDimension_u8_NumberOfStatusLines = 0;
    nBMS_capture_context.SystemConfig_Status_e_PixelOrder = 0;
    nBMS_capture_context.TimeNudgeEnabled = OMX_FALSE;
}


OMX_ERRORTYPE CCapture_context::pushOmxBufHdr(OMX_BUFFERHEADERTYPE* pOmxBuf)
{
    if ((nOmxBufHdrWriteCount + 1)%MAX_ALLOCATED_BUFFER_NB == nOmxBufHdrReadCount)
    {
        return (OMX_ErrorInsufficientResources);
    }

    ppOmxBufHdr[nOmxBufHdrWriteCount] = pOmxBuf;

    nOmxBufHdrWriteCount++;
    nOmxBufHdrWriteCount %= MAX_ALLOCATED_BUFFER_NB;

    return(OMX_ErrorNone);
}


OMX_ERRORTYPE CCapture_context::popOmxBufHdr(OMX_BUFFERHEADERTYPE** pOmxBuf)
{
    if (nOmxBufHdrReadCount == nOmxBufHdrWriteCount)
    {
        return (OMX_ErrorInsufficientResources);
    }
    
    *pOmxBuf = ppOmxBufHdr[nOmxBufHdrReadCount];
    ppOmxBufHdr[nOmxBufHdrReadCount] = (OMX_BUFFERHEADERTYPE *) 0;

    nOmxBufHdrReadCount++;
    nOmxBufHdrReadCount %= MAX_ALLOCATED_BUFFER_NB;

    return(OMX_ErrorNone);
}
