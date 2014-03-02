/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Implementation of Earpiece Sink
 * \author  ST-Ericsson
 */
/*****************************************************************************/


#include "Earpiece.h"
#include "audio_chipset_api_index.h"
#include "audiocodec_debug.h"

//-----------------------------------------------------------------------------
//                  Factory
//-----------------------------------------------------------------------------
OMX_ERRORTYPE earpieceFactoryMethod(ENS_Component_p * ppENSComponent)
{
    LOG_I("Enter.\n");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    Earpiece *earpiece = new Earpiece;
    if (earpiece == 0) {
        return OMX_ErrorInsufficientResources;
    }
    error = earpiece->construct();
    if (error != OMX_ErrorNone) {
        delete earpiece;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = earpiece;
    }

    EarpieceNmfMpc *earpiece_ProcessingComp = new EarpieceNmfMpc(*earpiece, AUDIO_HAL_CHANNEL_EARPIECE);
    if (earpiece_ProcessingComp == 0) {
        delete earpiece_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    earpiece->setProcessingComponent(earpiece_ProcessingComp);
    return error;
}

//-----------------------------------------------------------------------------
//                  Class EarpieceNmfMpc implementation
//-----------------------------------------------------------------------------
EarpieceNmfMpc * EarpieceNmfMpc::myself = 0;


OMX_ERRORTYPE EarpieceNmfMpc::construct() {
	myself = this; 
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::construct() );
    return OMX_ErrorNone;	
}

OMX_ERRORTYPE EarpieceNmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<EarpieceNmfMpc *>(NULL); 
    return OMX_ErrorNone;
}

OMX_ERRORTYPE EarpieceNmfMpc::configureMain(void) {
    // Here is righ place to put specific Apply config calls
    return AudioRendererAb8500NmfMpc::configureMain();
}

OMX_ERRORTYPE EarpieceNmfMpc::applyConfig( OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioRendererAb8500NmfMpc::applyConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE EarpieceNmfMpc::retrieveConfig( OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
   
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
         	return AudioRendererAb8500NmfMpc::retrieveConfig(
                       nConfigIndex, pComponentConfigStructure);
    }
}

void EarpieceNmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself){
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}

//-----------------------------------------------------------------------------
//                  Class Earpiece implementation
//-----------------------------------------------------------------------------

OMX_ERRORTYPE Earpiece::construct(void) {
	OMX_ERRORTYPE error = AudioRendererAb8500::construct(EAR_NB_CHANNEL, SUPPORT_VOLUME_CONFIG, HAVE_FEEDBACK_PORT);
    if (error != OMX_ErrorNone) return error;
    
    ENS_String<30> role = "audio_sink.pcm.earpiece";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE Earpiece::getConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        //Here is right place to put specific indexes
    	default:
    		return AudioRendererAb8500::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE Earpiece::setConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

	switch (nParamIndex) {
        //Here is right place to put specific indexes
		default:
			return AudioRendererAb8500::setConfig(nParamIndex,pComponentConfigStructure);
    }
}

