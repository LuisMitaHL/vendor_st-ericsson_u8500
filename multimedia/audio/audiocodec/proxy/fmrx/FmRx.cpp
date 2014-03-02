/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Implementation of FmRx Source
 * \author  ST-Ericsson
 */
/*****************************************************************************/


#include "FmRx.h"
#include "audio_chipset_api_index.h"
#include "audiocodec_debug.h"

//-----------------------------------------------------------------------------
//                  Factory
//-----------------------------------------------------------------------------
OMX_ERRORTYPE FmRxFactoryMethod(ENS_Component_p * ppENSComponent)
{
    LOG_I("Enter.\n");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    FmRx *FmRxSource = new FmRx;
    if (FmRxSource == 0) {
        return OMX_ErrorInsufficientResources;
    }
    error = FmRxSource->construct();
    if (error != OMX_ErrorNone) {
        delete FmRxSource;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = FmRxSource;
    }

    FmRxNmfMpc *FmRxSource_ProcessingComp = new FmRxNmfMpc(*FmRxSource, AUDIO_HAL_CHANNEL_FMRX);
    if (FmRxSource_ProcessingComp == 0) {
        delete FmRxSource_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    FmRxSource->setProcessingComponent(FmRxSource_ProcessingComp);
    return error;
}

//-----------------------------------------------------------------------------
//                  Class FmRxNmfMpc implementation
//-----------------------------------------------------------------------------
FmRxNmfMpc * FmRxNmfMpc::myself = 0;

OMX_ERRORTYPE FmRxNmfMpc::construct(void) {
	myself = this;
	RETURN_IF_OMX_ERROR( AudioCapturerAb8500NmfMpc::construct() );
    return OMX_ErrorNone;	
}

OMX_ERRORTYPE FmRxNmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioCapturerAb8500NmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<FmRxNmfMpc *>(NULL); 
    return OMX_ErrorNone;
}

OMX_ERRORTYPE FmRxNmfMpc::configureMain(void) {
    // Here is righ place to put specific Apply config calls
    return AudioCapturerAb8500NmfMpc::configureMain();
}

OMX_ERRORTYPE FmRxNmfMpc::applyConfig(OMX_INDEXTYPE nConfigIndex,  OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioCapturerAb8500NmfMpc::applyConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE FmRxNmfMpc::retrieveConfig( OMX_INDEXTYPE nConfigIndex,  OMX_PTR pComponentConfigStructure) {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioCapturerAb8500NmfMpc::retrieveConfig(
                    nConfigIndex, pComponentConfigStructure);
    }
}

void FmRxNmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself){
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}


//-----------------------------------------------------------------------------
//                  Class FmRx implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE FmRx::construct(void) {
	RETURN_IF_OMX_ERROR(AudioCapturerAb8500::construct(FMRX_NB_CHANNEL, DO_NOT_SUPPORT_VOLUME_CONFIG) );

    ENS_String<30> role = "audio_capturer.pcm.fm";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    return OMX_ErrorNone;    
}

OMX_ERRORTYPE FmRx::getConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
    switch (nParamIndex) {
        //Here is right place to put specific indexes
		default:
			return AudioCapturerAb8500::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE FmRx::setConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
	switch (nParamIndex) {
        //Here is right place to put specific indexes
        default:
			return AudioCapturerAb8500::setConfig(nParamIndex,pComponentConfigStructure);
	}
}


