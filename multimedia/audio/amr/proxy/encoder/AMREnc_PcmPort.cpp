/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AMREnc_PcmPort.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include "AMREnc_PcmPort.h"

AMREnc_PcmPort::AMREnc_PcmPort(OMX_U32         nIndex,
               OMX_DIRTYPE     eDir,
               OMX_U32         nChannels,
               OMX_U32         nSampleRate,
               OMX_U32         nBitPerSample,
               bool            isHost,
               ENS_Component & enscomp)
:AFM_PcmPort(nIndex,
             eDir,
             nChannels,
             nSampleRate,
             nBitPerSample,
             enscomp)
{
    setProperties(isHost);
};

AMREnc_PcmPort::AMREnc_PcmPort(OMX_U32 nIndex,
            OMX_DIRTYPE eDir,
            OMX_U32 nBufferSizeMin,
            const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
            bool isHost,
            ENS_Component &enscomp)
:AFM_PcmPort(nIndex,
             eDir,
             nBufferSizeMin,
             defaultPcmSettings,
             enscomp)
{
    setProperties(isHost);
};


OMX_ERRORTYPE   AMREnc_PcmPort::checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const 
{
    if (pcmSettings.nChannels != 1) return OMX_ErrorUnsupportedSetting;
    if (pcmSettings.nSamplingRate != 8000) return OMX_ErrorUnsupportedSetting;
    return OMX_ErrorNone;
};

void AMREnc_PcmPort::setProperties(bool isHost) {
    setSupportedProprietaryComm(isHost? AFM_HOST_PROPRIETARY_COMMUNICATION:AFM_MPC_PROPRIETARY_COMMUNICATION);
    //Now already done in AFM
    //setMpc(!isHost);
    //setPortPriorityLevel(OMX_PriorityBackground);
}

