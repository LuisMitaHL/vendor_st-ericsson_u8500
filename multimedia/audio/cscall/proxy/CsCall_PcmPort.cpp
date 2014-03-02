/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CsCall_PcmPort.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "CsCall_PcmPort.h"
#include "CsCall_defines.h"
#include "cscall_config.h"

#if 0
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "audio_cscall_proxy_CsCall_PcmPortTraces.h"
#endif
#endif

CsCall_PcmPort::CsCall_PcmPort(OMX_U32 nIndex, OMX_DIRTYPE eDir, OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
        ENS_Component &enscomp)
: AFM_PcmPort(nIndex, eDir, nBufferSizeMin, defaultPcmSettings, enscomp)
{
    setSupportedProprietaryComm(AFM_HOST_PROPRIETARY_COMMUNICATION);
    setMpc(false);
}


OMX_ERRORTYPE CsCall_PcmPort::setParameter(OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) 
{
    switch (nParamIndex)
    {
        case OMX_IndexParamAudioPcm:
            {
                OMX_AUDIO_PARAM_PCMMODETYPE *pcmformat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
                OMX_ERRORTYPE error;

                // check nb channels
                if (pcmformat->nChannels != 1)
                    return OMX_ErrorUnsupportedSetting;

                // check sample frequency
                if((pcmformat->nSamplingRate != 8000) && (pcmformat->nSamplingRate != 16000))
                    return OMX_ErrorUnsupportedSetting;

                // check sample size
                if(pcmformat->nBitPerSample != 16)
                    return OMX_ErrorUnsupportedSetting;

                // call AFM implementation is NEEDED
                error = AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
                if (error == OMX_ErrorNone)
				{
                    // If no error, we change PCM buffer size 
                    if ((pcmformat->nSamplingRate == 8000))
					{
					  mParamPortDefinition.nBufferSize = 8 * 2;
					}
					else
					{
					  mParamPortDefinition.nBufferSize = 16 * 2;
                    }

					if ((pcmformat->nPortIndex == CSCALL_OUTPUT_PORT_INDEX))
					{
					  mParamPortDefinition.nBufferSize = mParamPortDefinition.nBufferSize * OMX_OUTPUT_BUFFER_DURATION;
					}
					else
					{
					  mParamPortDefinition.nBufferSize = mParamPortDefinition.nBufferSize * OMX_INPUT_BUFFER_DURATION;
                    }
                };
                return error;
            }

        default:
            return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
    }
}

