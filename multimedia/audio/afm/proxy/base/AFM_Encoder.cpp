/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_Encoder.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AFM_Encoder.h"

#ifndef BC_BREAK
AFM_API_EXPORT OMX_ERRORTYPE
AFM_Encoder::construct(
		OMX_AUDIO_CODINGTYPE eEncoding,
		OMX_U32 nSampleFrameSize,
		const OMX_AUDIO_PARAM_PCMMODETYPE & defaultPcmSettings, 
        OMX_U32 nRoleCount)
{
	OMX_ERRORTYPE error;

	error = AFM_Component::construct(2, nRoleCount);
	if (error != OMX_ErrorNone) return error;

	error = createPcmPort(0, OMX_DirInput, nSampleFrameSize * defaultPcmSettings.nBitPerSample/8, defaultPcmSettings);
	if (error != OMX_ErrorNone) return error;

	return OMX_ErrorNone;
}
#endif

AFM_API_EXPORT OMX_ERRORTYPE
AFM_Encoder::construct(OMX_U32 nRoleCount)
{
    OMX_ERRORTYPE error;

    error = AFM_Component::construct(2, nRoleCount);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

AFM_API_EXPORT AFM_Encoder::~AFM_Encoder() {}


AFM_API_EXPORT OMX_ERRORTYPE AFM_Encoder::getParameter(
		OMX_INDEXTYPE nParamIndex,
		OMX_PTR pComponentParameterStructure) const
{
	OMX_ERRORTYPE error = OMX_ErrorNone;

	switch (nParamIndex) {
		case AFM_IndexParamPcmLayout:
			{
				AFM_PARAM_PCMLAYOUTTYPE *pcmlayoutIn
					= (AFM_PARAM_PCMLAYOUTTYPE *) pComponentParameterStructure;


				if (pcmlayoutIn->nPortIndex != 0) {
					return OMX_ErrorBadPortIndex;
				}


				pcmlayoutIn->nBlockSize      = getSampleFrameSize();
				pcmlayoutIn->nChannels       = getChannels();
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

