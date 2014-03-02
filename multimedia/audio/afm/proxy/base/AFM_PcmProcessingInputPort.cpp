/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_PcmProcessingInputPort.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef BC_BREAK
#include "AFM_PcmProcessingInputPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"

AFM_PcmProcessingInputPort::AFM_PcmProcessingInputPort(
        OMX_U32 nIndex,
        OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
        ENS_Component &enscomp) 	 
    : AFM_PcmPort(nIndex, OMX_DirInput, nBufferSizeMin, defaultPcmSettings, enscomp)
{ 	 
}


OMX_ERRORTYPE AFM_PcmProcessingInputPort::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) 
{
    switch (nParamIndex) {
        case OMX_IndexParamAudioPcm:
        {
            OMX_ERRORTYPE error = AFM_PcmPort::setParameter(
                nParamIndex, pComponentParameterStructure);
            if (error != OMX_ErrorNone) return error;

            // propagate PCM settings to output port (slaving behavior)
            AFM_PcmPort *outputPort = (AFM_PcmPort *) getENSComponent().getPort(1);
            error = outputPort->updatePcmSettings(getPcmSettings().nChannels, getPcmSettings().nSamplingRate);

            if (error != OMX_ErrorNone) return error;

            return OMX_ErrorNone;
        }

        default:
            return AFM_PcmPort::setParameter(
                nParamIndex, pComponentParameterStructure);
    }
}

#endif
