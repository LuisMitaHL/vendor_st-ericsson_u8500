/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_PcmProcessing.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AFM_PcmProcessing.h"
#include "AFM_PcmProcessingInputPort.h"

#ifndef BC_BREAK
EXPORT_C OMX_ERRORTYPE
AFM_PcmProcessing::construct(
    const OMX_AUDIO_PARAM_PCMMODETYPE & defaultPcmSettingsIn,
    const OMX_AUDIO_PARAM_PCMMODETYPE & defaultPcmSettingsOut, 
    OMX_U32 nRoleCount,
    bool isArmOnly)
{
    OMX_ERRORTYPE error;

    error = AFM_Component::construct(2, nRoleCount, isArmOnly);
    if (error != OMX_ErrorNone) return error;

    //create i/p PCM port
//  input port needs to be specific because of slaving behavior of PCM settings
    ENS_Port * port = new AFM_PcmProcessingInputPort(
                0, getBlockSize() * defaultPcmSettingsIn.nChannels * defaultPcmSettingsIn.nBitPerSample/8, defaultPcmSettingsIn, *this);
    if (port == 0) {
        return OMX_ErrorInsufficientResources;
    }
    addPort(port);

    //create o/p PCM port
    error = createPcmPort(1, OMX_DirOutput, getBlockSize() * defaultPcmSettingsOut.nChannels * defaultPcmSettingsOut.nBitPerSample/8, defaultPcmSettingsOut);
    if (error != OMX_ErrorNone) return error;
    
    return OMX_ErrorNone;
}
#endif

EXPORT_C OMX_ERRORTYPE
AFM_PcmProcessing::construct(OMX_U32 nRoleCount, bool isArmOnly)
{
    OMX_ERRORTYPE error;

    error = AFM_Component::construct(2, nRoleCount, isArmOnly);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

EXPORT_C AFM_PcmProcessing::~AFM_PcmProcessing() {}


EXPORT_C OMX_ERRORTYPE AFM_PcmProcessing::getParameter(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure) const
{
    switch (nParamIndex) {
    case AFM_IndexParamPcmLayout:
    {
        AFM_PARAM_PCMLAYOUTTYPE *pcmlayout
        = (AFM_PARAM_PCMLAYOUTTYPE *) pComponentParameterStructure;

        OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsIn =
            ((AFM_PcmPort *) getPort(0))->getPcmSettings();

        OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsOut =
            ((AFM_PcmPort *) getPort(1))->getPcmSettings();

        if (pcmlayout->nPortIndex == 0)
        {
            pcmlayout->nBlockSize     = getBlockDurationInMs() * (pcmSettingsIn.nSamplingRate /1000);
            pcmlayout->nChannels      = pcmSettingsIn.nChannels;
            pcmlayout->nMaxChannels   = 2;
            pcmlayout->nBitsPerSample = getSampleBitSize();
            pcmlayout->nNbBuffers     = 2;
        }
        else if (pcmlayout->nPortIndex == 1)
        {
            pcmlayout->nBlockSize     =  getBlockDurationInMs() * (pcmSettingsOut.nSamplingRate /1000);
            pcmlayout->nChannels      = pcmSettingsOut.nChannels;
            pcmlayout->nMaxChannels   = 2;
            pcmlayout->nBitsPerSample = getSampleBitSize();
            pcmlayout->nNbBuffers     = 2;
        } else return OMX_ErrorBadPortIndex;

        return OMX_ErrorNone;
    }

    default:
        return AFM_Component::getParameter(
                   nParamIndex, pComponentParameterStructure);
    }
}

