/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Implementation of FmTx
 * \author  ST-Ericsson
 */
/*****************************************************************************/


#include "FmTx.h"
#include "audio_chipset_api_index.h"
#include "audiocodec_debug.h"

//-----------------------------------------------------------------------------
//                  Factory
//-----------------------------------------------------------------------------
OMX_ERRORTYPE FmTxFactoryMethod(ENS_Component_p * ppENSComponent)
{
    LOG_I("Enter.\n");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    FmTx * FmTxSink = new FmTx;
    if (FmTxSink == 0) {
        return OMX_ErrorInsufficientResources;
    }
    error = FmTxSink->construct();
    if (error != OMX_ErrorNone) {
        delete FmTxSink;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = FmTxSink;
    }

    FmTxNmfMpc *FmTxSink_ProcessingComp = new FmTxNmfMpc(*FmTxSink, AUDIO_HAL_CHANNEL_FMTX);
    if (FmTxSink_ProcessingComp == 0) {
        delete FmTxSink_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    FmTxSink->setProcessingComponent(FmTxSink_ProcessingComp);

    return error;
}

//-----------------------------------------------------------------------------
//                  Class FmTxNmfMpc implementation
//-----------------------------------------------------------------------------
FmTxNmfMpc * FmTxNmfMpc::myself = 0;

OMX_ERRORTYPE FmTxNmfMpc::construct() {
	myself = this; 
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::construct() );
    return OMX_ErrorNone;	
}

OMX_ERRORTYPE FmTxNmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<FmTxNmfMpc *>(NULL); 
    return OMX_ErrorNone;
}

OMX_ERRORTYPE FmTxNmfMpc::configureMain(void) {
    OMX_ERRORTYPE error(OMX_ErrorNone);

    FmTx *proxy = static_cast<FmTx *>(&mENSComponent);
    error =  applyConfig((OMX_INDEXTYPE )OMX_IndexConfigAudioPower, proxy->getPowerConfig());
    if(error != OMX_ErrorNone){
        return error;
    }
    return AudioRendererAb8500NmfMpc::configureMain();
}

OMX_ERRORTYPE FmTxNmfMpc::applyConfig(
    OMX_INDEXTYPE nConfigIndex,  
    OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
   	
    	// Below ndexs are not yet supported !!
        case OMX_IndexConfigAudioVolume:
        case OMX_IndexConfigAudioMute:
        case OMX_IndexConfigAudioChannelVolume:
    	case OMX_IndexConfigAudioChannelMute:
	    
        case OMX_IndexConfigAudioVolumeRamp:
	    case OMX_IndexConfigAudioBurstControl:
        case OMX_IndexConfigAudioRegBankQuery:
        case OMX_IndexConfigAudioRegBankDescriptorQuery:
	    case OMX_IndexConfigAudioCustomHwControl:
	    case OMX_IndexConfigAudioLoopControl:
            return OMX_ErrorUnsupportedIndex;
        default:
            return AudioRendererAb8500NmfMpc::applyConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE FmTxNmfMpc::retrieveConfig(
    OMX_INDEXTYPE nConfigIndex,  
    OMX_PTR pComponentConfigStructure)
{
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nConfigIndex) {
    	// Below indexs are not yet supported !!
        case OMX_IndexConfigAudioVolume:
        case OMX_IndexConfigAudioMute:
    	case OMX_IndexConfigAudioChannelVolume:
    	case OMX_IndexConfigAudioChannelMute:
        
        case OMX_IndexConfigAudioVolumeRamp:
        case OMX_IndexConfigAudioBurstControl:
        case OMX_IndexConfigAudioRegBankQuery:
        case OMX_IndexConfigAudioRegBankDescriptorQuery:
        case OMX_IndexConfigAudioCustomHwControl:
        case OMX_IndexConfigAudioLoopControl:
            return OMX_ErrorUnsupportedIndex;
        default:
         	return AudioRendererAb8500NmfMpc::retrieveConfig(
                       nConfigIndex, pComponentConfigStructure);
    }
}

void FmTxNmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself){
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}


//-----------------------------------------------------------------------------
//                  Class FmTx implementation
//-----------------------------------------------------------------------------

OMX_ERRORTYPE FmTx::construct(void) {
    RETURN_IF_OMX_ERROR(AudioRendererAb8500::construct(FMTX_NB_CHANNEL, DO_NOT_SUPPORT_VOLUME_CONFIG, DO_NOT_HAVE_FEEDBACK_PORT) );

    ENS_String<30> role = "audio_renderer.pcm.fm";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE FmTx::getConfig(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentConfigStructure) const {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        //Here is right place to put specific indexes
		default:
			return AudioRendererAb8500::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE FmTx::setConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

	switch (nParamIndex) {
        //Here is right place to put specific indexes
        default:
			return AudioRendererAb8500::setConfig(nParamIndex,pComponentConfigStructure);
    }
}

