/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "GSMHRDec_BitstreamPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "AFM_PcmPort.h"

GSMHRDec_BitstreamPort::GSMHRDec_BitstreamPort(
        OMX_U32 nIndex, 
        OMX_DIRTYPE eDir,
        OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_PARAM_GSMHRTYPE &defaultGsmhrSettings,
        ENS_Component &enscomp)
        : AFM_Port(
            nIndex, eDir, nBufferSizeMin, OMX_AUDIO_CodingGSMHR, enscomp),
          mGsmhrParams(defaultGsmhrSettings)
{
}

OMX_ERRORTYPE 
GSMHRDec_BitstreamPort::checkFormatCompatibility(
        OMX_HANDLETYPE hTunneledComp, 
        OMX_U32 nTunneledPort) const
{
    return OMX_ErrorNone;
    
}

OMX_ERRORTYPE GSMHRDec_BitstreamPort::CheckOMXParams(OMX_AUDIO_PARAM_GSMHRTYPE *gsmhrParams)
{
    if (gsmhrParams->bHiPassFilter !=OMX_TRUE ) return OMX_ErrorBadParameter;
    else {
    	return OMX_ErrorNone;
    }
}

OMX_ERRORTYPE GSMHRDec_BitstreamPort::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE error;
    switch (nParamIndex) {
        case OMX_IndexParamAudioGsm_HR:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_GSMHRTYPE);
                OMX_AUDIO_PARAM_GSMHRTYPE *gsmhrParams =
                    (OMX_AUDIO_PARAM_GSMHRTYPE *)pComponentParameterStructure;
                AFM_PcmPort *outputPort = (AFM_PcmPort *) getENSComponent().getPort(1);

                error = CheckOMXParams(gsmhrParams);
                if (error != OMX_ErrorNone) return error;
 
                mGsmhrParams      = *gsmhrParams;
                return outputPort->updatePcmSettings(1, 8000);
            }

        default:
            return AFM_Port::setParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE GSMHRDec_BitstreamPort::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const
{
    switch (nParamIndex) {
        case OMX_IndexParamAudioGsm_HR:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_GSMHRTYPE);
                OMX_AUDIO_PARAM_GSMHRTYPE *gsmhrparams =
                    (OMX_AUDIO_PARAM_GSMHRTYPE *)pComponentParameterStructure;

                *gsmhrparams      = mGsmhrParams;

                return OMX_ErrorNone;
            }

        default:
            return AFM_Port::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE GSMHRDec_BitstreamPort::updateGsmhrSettings(
        OMX_AUDIO_PARAM_GSMHRTYPE &gsmhrParams)
{
    OMX_ERRORTYPE error;

    if ( (mGsmhrParams.bDTX            == gsmhrParams.bDTX) &&
         (mGsmhrParams.bHiPassFilter   == gsmhrParams.bHiPassFilter))
         {
          return OMX_ErrorNone;
       }

    mGsmhrParams = gsmhrParams;

    error = updateSettings(OMX_IndexParamAudioGsm_HR, &mGsmhrParams);
    if (error != OMX_ErrorNone) return error; 

    return OMX_ErrorNone;        
}


