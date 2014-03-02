/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "GSMFREnc_BitStreamPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "AFM_PcmPort.h"

GSMFREnc_BitstreamPort::GSMFREnc_BitstreamPort(
	OMX_U32 nIndex, 
        OMX_DIRTYPE eDir,
        OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_PARAM_GSMFRTYPE &defaultGsmfrSettings,
	ENS_Component &enscomp)
        : AFM_Port(
            nIndex, eDir, nBufferSizeMin, OMX_AUDIO_CodingGSMFR, enscomp),
          mGsmfrParams(defaultGsmfrSettings)
{
}

OMX_ERRORTYPE 
GSMFREnc_BitstreamPort::checkFormatCompatibility(
        OMX_HANDLETYPE hTunneledComp, 
        OMX_U32 nTunneledPort) const
{
    return OMX_ErrorNone;
    
}
void GSMFREnc_BitstreamPort::checkInformationGivenByPcmPort(OMX_AUDIO_PARAM_PCMMODETYPE pcmformat)
{
    /*OMX_BOOL InfoModified = OMX_FALSE;


    if (InfoModified == OMX_TRUE){
        getENSComponent().eventHandlerCB(
                OMX_EventPortSettingsChanged, 0, 0, 0);
    }*/
}

OMX_ERRORTYPE GSMFREnc_BitstreamPort::CheckOMXParams(OMX_AUDIO_PARAM_GSMFRTYPE *gsmfrParams)
{
      if (gsmfrParams->bHiPassFilter !=OMX_TRUE ) return OMX_ErrorBadParameter;
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE GSMFREnc_BitstreamPort::setParameter(
	OMX_INDEXTYPE nParamIndex,
	OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE error;
	switch (nParamIndex) {
	case OMX_IndexParamAudioGsm_FR:
	{
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_GSMFRTYPE);
		OMX_AUDIO_PARAM_GSMFRTYPE *gsmfrParams =
			(OMX_AUDIO_PARAM_GSMFRTYPE *)pComponentParameterStructure;

        error = CheckOMXParams(gsmfrParams);
        if (error != OMX_ErrorNone) return error;
        mGsmfrParams      = *gsmfrParams;
 
		/*mGsmFrParams.nPortIndex      = gsmfrparams->nPortIndex;
		mGsmFrParams.bDTX            = gsmfrparams->bDTX;
		mGsmFrParams.bHiPassFilter   = gsmfrparams->bHiPassFilter;*/
                return OMX_ErrorNone;
	}

	default:
            return AFM_Port::setParameter(
				   nParamIndex, pComponentParameterStructure);
	}
}

OMX_ERRORTYPE GSMFREnc_BitstreamPort::getParameter(
	OMX_INDEXTYPE nParamIndex,
	OMX_PTR pComponentParameterStructure) const
{
	switch (nParamIndex) {
	case OMX_IndexParamAudioGsm_FR:
	{
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_GSMFRTYPE);
		OMX_AUDIO_PARAM_GSMFRTYPE *gsmfrparams =
			(OMX_AUDIO_PARAM_GSMFRTYPE *)pComponentParameterStructure;
        
        *gsmfrparams      = mGsmfrParams;

		/*gsmfrparams->nPortIndex       = mGsmFrParams.nPortIndex;
		gsmfrparams->bDTX             = mGsmFrParams.bDTX;
		gsmfrparams->bHiPassFilter    = mGsmFrParams.bHiPassFilter;*/

                return OMX_ErrorNone;
	}

	default:
            return AFM_Port::getParameter(
				   nParamIndex, pComponentParameterStructure);
	}
}


