/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "GSMFRDec_BitStreamPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "AFM_PcmPort.h"

GSMFRDec_BitstreamPort::GSMFRDec_BitstreamPort(
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
GSMFRDec_BitstreamPort::checkFormatCompatibility(
        OMX_HANDLETYPE hTunneledComp, 
        OMX_U32 nTunneledPort) const
{
    return OMX_ErrorNone;
    
}

OMX_ERRORTYPE GSMFRDec_BitstreamPort::CheckOMXParams(OMX_AUDIO_PARAM_GSMFRTYPE *gsmfrParams)
{
      if (gsmfrParams->bHiPassFilter !=OMX_TRUE ) return OMX_ErrorBadParameter;
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE GSMFRDec_BitstreamPort::setParameter(
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
                AFM_PcmPort *outputPort = (AFM_PcmPort *) getENSComponent().getPort(1);

        error = CheckOMXParams(gsmfrParams);
        if (error != OMX_ErrorNone) return error;
        mGsmfrParams      = *gsmfrParams;
        return outputPort->updatePcmSettings(1, 8000);
 
		/*mGsmFrParams.nPortIndex      = gsmfrparams->nPortIndex;
		mGsmFrParams.bDTX            = gsmfrparams->bDTX;
		mGsmFrParams.bHiPassFilter   = gsmfrparams->bHiPassFilter;
		return OMX_ErrorNone;*/
	}

	default:
            return AFM_Port::setParameter(
				   nParamIndex, pComponentParameterStructure);
	}
}

OMX_ERRORTYPE GSMFRDec_BitstreamPort::getParameter(
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
OMX_ERRORTYPE GSMFRDec_BitstreamPort::updateGsmfrSettings(
        OMX_AUDIO_PARAM_GSMFRTYPE &gsmfrParams)
{
    OMX_ERRORTYPE error;

    if ( (mGsmfrParams.bDTX            == gsmfrParams.bDTX) &&
         (mGsmfrParams.bHiPassFilter   == gsmfrParams.bHiPassFilter))
         {
          return OMX_ErrorNone;
       }

    mGsmfrParams = gsmfrParams;

    error = updateSettings(OMX_IndexParamAudioGsm_FR, &mGsmfrParams);
    if (error != OMX_ErrorNone) return error; 

    return OMX_ErrorNone;        
}




