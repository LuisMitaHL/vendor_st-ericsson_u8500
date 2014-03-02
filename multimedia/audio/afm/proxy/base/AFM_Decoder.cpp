/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_Decoder.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AFM_Decoder.h"

#ifndef BC_BREAK

AFM_API_EXPORT OMX_ERRORTYPE  
AFM_Decoder::construct(
        OMX_AUDIO_CODINGTYPE eEncoding, 
        OMX_U32 nSampleFrameSize, 
        const OMX_AUDIO_PARAM_PCMMODETYPE & defaultPcmSettings, 
        OMX_U32 nRoleCount)
{
    OMX_ERRORTYPE error;

    error = AFM_Component::construct(2, nRoleCount);
    if (error != OMX_ErrorNone) return error;

    error = createPcmPort(1, OMX_DirOutput, nSampleFrameSize * defaultPcmSettings.nBitPerSample/8, defaultPcmSettings);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}
#endif

AFM_API_EXPORT OMX_ERRORTYPE  
AFM_Decoder::construct(OMX_U32 nRoleCount)
{
    OMX_ERRORTYPE error;

    error = AFM_Component::construct(2, nRoleCount);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

AFM_API_EXPORT AFM_Decoder::~AFM_Decoder() {}


AFM_API_EXPORT OMX_ERRORTYPE AFM_Decoder::getParameter(
        OMX_INDEXTYPE nParamIndex,  
        OMX_PTR pComponentParameterStructure) const 
{
    OMX_ERRORTYPE error = OMX_ErrorNone;

    switch (nParamIndex) {
        case AFM_IndexParamPcmLayout:
            {
                AFM_PARAM_PCMLAYOUTTYPE *pcmlayoutIn 
                    = (AFM_PARAM_PCMLAYOUTTYPE *) pComponentParameterStructure;

                if (pcmlayoutIn->nPortIndex != 1) {
                    return OMX_ErrorBadPortIndex;
                }

                pcmlayoutIn->nBlockSize      = getSampleFrameSize();
                pcmlayoutIn->nChannels       = 0;
                pcmlayoutIn->nMaxChannels    = getMaxChannels();
                pcmlayoutIn->nBitsPerSample  = getSampleBitSize();
                pcmlayoutIn->nNbBuffers      = 1;

                return error;
            }

        default:
            return AFM_Component::getParameter(
                nParamIndex, pComponentParameterStructure);
    }
}

