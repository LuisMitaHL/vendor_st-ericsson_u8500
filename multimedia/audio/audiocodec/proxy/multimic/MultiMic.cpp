/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Implementation of MultiMic Source
 * \author  ST-Ericsson
 */
/*****************************************************************************/
#include "MultiMic.h"
#include "audio_chipset_api_index.h"
#include "audiocodec_debug.h"

//-----------------------------------------------------------------------------
//                  Factory
//-----------------------------------------------------------------------------
OMX_ERRORTYPE multiMicFactoryMethod(ENS_Component_p * ppENSComponent)
{
    LOG_I("Enter.\n");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    MultiMic *multiMic = new MultiMic;
    if (multiMic == 0) {
        return OMX_ErrorInsufficientResources;
    }
    error = multiMic->construct();
    if (error != OMX_ErrorNone) {
        delete multiMic;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = multiMic;
    }

    MultiMicNmfMpc *multiMic_ProcessingComp = new MultiMicNmfMpc(*multiMic, AUDIO_HAL_CHANNEL_MIC);
    if (multiMic_ProcessingComp == 0) {
        delete multiMic_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    multiMic->setProcessingComponent(multiMic_ProcessingComp);
    return error;
}

//-----------------------------------------------------------------------------
//                  Class MultiMic implementation
//-----------------------------------------------------------------------------
MultiMicNmfMpc * MultiMicNmfMpc::myself = 0;

OMX_ERRORTYPE MultiMicNmfMpc::construct() {
	myself = this; 
	RETURN_IF_OMX_ERROR( AudioCapturerAb8500NmfMpc::construct() );
    return OMX_ErrorNone;	
}

OMX_ERRORTYPE MultiMicNmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioCapturerAb8500NmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<MultiMicNmfMpc *>(NULL); 
    return OMX_ErrorNone;
}
    
OMX_ERRORTYPE MultiMicNmfMpc::configureMain(void) {
    // Here is righ place to put specific Apply config calls
    return AudioCapturerAb8500NmfMpc::configureMain();
}

OMX_ERRORTYPE MultiMicNmfMpc::applyConfig( OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioCapturerAb8500NmfMpc::applyConfig(
                    nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE MultiMicNmfMpc::retrieveConfig(
    OMX_INDEXTYPE nConfigIndex,  
    OMX_PTR pComponentConfigStructure)
{
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

void MultiMicNmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself){
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}


//-----------------------------------------------------------------------------
//                  Class MultiMic implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE MultiMic::construct(void) {

    mConfigHwLoop.nSize      = sizeof(OMX_AUDIO_CONFIG_HWLOOPCONTROLTYPE);
    getOmxIlSpecVersion(& mConfigHwLoop.nVersion);
    mConfigHwLoop.nPortIndex = 0;
    mConfigHwLoop.eLoopIndex = OMX_AUDIO_SIDETONE_LOOP;
    mConfigHwLoop.nChannel   = 0;
    mConfigHwLoop.bControlSwitch = OMX_FALSE;
    mConfigHwLoop.bLinear    = OMX_FALSE;
    mConfigHwLoop.sLoopVolume.nValue = 0;
    mConfigHwLoop.sLoopVolume.nMin   = -3000;
    mConfigHwLoop.sLoopVolume.nMax   = 0;

	RETURN_IF_OMX_ERROR( AudioCapturerAb8500::construct(MULTIMIC_NB_CHANNEL, SUPPORT_VOLUME_CONFIG) );

    ENS_String<30> role = "audio_source.pcm.multimic";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MultiMic::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
    switch (nParamIndex) {
        //Here is right place to put specific indexes
		default:
			return AudioCapturerAb8500::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE MultiMic::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
	switch (nParamIndex) {
        //Here is right place to put specific indexes
        default:
			return AudioCapturerAb8500::setConfig(nParamIndex,pComponentConfigStructure);
	}
}


