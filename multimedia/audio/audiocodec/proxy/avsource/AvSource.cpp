/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Implementation of AvSource Source
 * \author  ST-Ericsson
 */
/*****************************************************************************/

#include "AvSource.h"
#include "audio_chipset_api_index.h"
#include "audiocodec_debug.h"

//-----------------------------------------------------------------------------
//                  Factory
//-----------------------------------------------------------------------------
//TODO remove nokia word
OMX_ERRORTYPE AvSourceFactoryMethod(ENS_Component_p * ppENSComponent)
{
    LOG_I("Enter.\n");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    AvSource * avsource = new AvSource;
    if (avsource == 0) {
        return OMX_ErrorInsufficientResources;
    }
    error = avsource->construct();
    if (error != OMX_ErrorNone) {
        delete avsource;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = avsource;
    }

    AvSourceNmfMpc *AvSource_ProcessingComp = new AvSourceNmfMpc(*avsource, AUDIO_HAL_CHANNEL_HSETIN);
    if (AvSource_ProcessingComp == 0) {
        delete AvSource_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    avsource->setProcessingComponent(AvSource_ProcessingComp);

    return error;
}
//-----------------------------------------------------------------------------
//                  Class AvSourceNmfMpc implementation
//-----------------------------------------------------------------------------
AvSourceNmfMpc * AvSourceNmfMpc::myself = 0;

OMX_ERRORTYPE AvSourceNmfMpc::construct() {
	myself = this; 
	RETURN_IF_OMX_ERROR( AudioCapturerAb8500NmfMpc::construct() );
    return OMX_ErrorNone;	
}

OMX_ERRORTYPE AvSourceNmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioCapturerAb8500NmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<AvSourceNmfMpc *>(NULL); 
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AvSourceNmfMpc::configureMain(void) {
    // Here is righ place to put specific Apply config calls
    return AudioCapturerAb8500NmfMpc::configureMain();
}

OMX_ERRORTYPE AvSourceNmfMpc::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioCapturerAb8500NmfMpc::applyConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AvSourceNmfMpc::retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) { 
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

void AvSourceNmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself){
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}

//-----------------------------------------------------------------------------
//                  Class AvSource implementation
//-----------------------------------------------------------------------------

OMX_ERRORTYPE AvSource::construct(void) {
	RETURN_IF_OMX_ERROR( AudioCapturerAb8500::construct(AVSOURCE_NB_CHANNEL, SUPPORT_VOLUME_CONFIG) );

    ENS_String<30> role = "audio_source.pcm.nokiaav";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AvSource::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
    switch (nParamIndex) {
        //Here is right place to put specific indexes
		default:
			return AudioCapturerAb8500::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AvSource::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
	switch (nParamIndex) {
        //Here is right place to put specific indexes
        default:
			return AudioCapturerAb8500::setConfig(nParamIndex,pComponentConfigStructure);
	}
}



