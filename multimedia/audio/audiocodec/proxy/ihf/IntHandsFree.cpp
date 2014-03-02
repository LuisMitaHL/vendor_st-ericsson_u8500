/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Implementation of IHF Sink
 * \author  ST-Ericsson
 */
/*****************************************************************************/

#include "IntHandsFree.h"
#include "NmfMpcInOut.h"
#include "audio_chipset_api_index.h"
#include "audiocodec_debug.h"
#include "OMX_CoreExt.h"

//-----------------------------------------------------------------------------
//                  Factory
//-----------------------------------------------------------------------------
OMX_ERRORTYPE intHandsFreeFactoryMethod(ENS_Component_p * ppENSComponent)
{
    LOG_I("IHF: Enter.\n");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    IntHandsFree *intHandsFree = new IntHandsFree;
    if (intHandsFree == 0) {
        return OMX_ErrorInsufficientResources;
    }
    
    error = intHandsFree->construct();
    if (error != OMX_ErrorNone) {
        delete intHandsFree;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = intHandsFree;
    }

    IntHandsFreeNmfMpc *intHandsFree_ProcessingComp = new IntHandsFreeNmfMpc(*intHandsFree, AUDIO_HAL_CHANNEL_SPEAKER);
    if (intHandsFree_ProcessingComp == 0) {
        delete intHandsFree_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    intHandsFree->setProcessingComponent(intHandsFree_ProcessingComp);
    return error;
}


//-----------------------------------------------------------------------------
//                  Class IntHandsFreeNmfMpc implementation
//-----------------------------------------------------------------------------
IntHandsFreeNmfMpc * IntHandsFreeNmfMpc::myself = 0;

OMX_ERRORTYPE IntHandsFreeNmfMpc::construct() {
	myself = this; 
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::construct() );
    return OMX_ErrorNone;	
}

OMX_ERRORTYPE IntHandsFreeNmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<IntHandsFreeNmfMpc *>(NULL); 
    return OMX_ErrorNone;
}

OMX_ERRORTYPE IntHandsFreeNmfMpc::configureMain(void) {
    // Here is righ place to put specific Apply config calls
    return AudioRendererAb8500NmfMpc::configureMain();
}

OMX_ERRORTYPE IntHandsFreeNmfMpc::applyConfig( OMX_INDEXTYPE nConfigIndex,  OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioRendererAb8500NmfMpc::applyConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE IntHandsFreeNmfMpc::retrieveConfig( OMX_INDEXTYPE nConfigIndex,  OMX_PTR pComponentConfigStructure) {
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

void IntHandsFreeNmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself){
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}

//-----------------------------------------------------------------------------
//                  Class IntHandsFree implementation
//-----------------------------------------------------------------------------

OMX_ERRORTYPE IntHandsFree::construct(void) {
	RETURN_IF_OMX_ERROR( AudioRendererAb8500::construct(IHF_NB_CHANNEL, SUPPORT_VOLUME_CONFIG, HAVE_FEEDBACK_PORT) );

    ENS_String<30> role = "audio_sink.pcm.ihf";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE IntHandsFree::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        //Here is right place to put specific indexes
    	default:
    		return AudioRendererAb8500::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE IntHandsFree::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
	
    switch (nParamIndex) {
        //Here is right place to put specific indexes
		default:
			return AudioRendererAb8500::setConfig(nParamIndex,pComponentConfigStructure);
	}
}

OMX_ERRORTYPE IntHandsFree::createResourcesDB() {
    
    LOG_I("IHF: create DB \n");
    
    mRMP = new IntHandsFree_RDB(getPortCount());
 
    if (mRMP == 0) {
        return OMX_ErrorInsufficientResources;
    }
    else {
        return OMX_ErrorNone;
    }
}

OMX_ERRORTYPE IntHandsFree::StartLowPower(OMX_U32 LPADomainID)
{
    LOG_I("IHF: Start Low Power.\n");

    AudioRendererAb8500NmfMpc *processingComponent = static_cast<AudioRendererAb8500NmfMpc*>(&getProcessingComponent());
    NmfMpcOutAb8500 *dmaOut = processingComponent->getNmfMpcOutAb8500();

    if(dmaOut != NULL){
            eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, (OMX_U32)OMX_ALL, (OMX_U32)AFM_IndexLowPowerRenderer, (OMX_PTR) 0);
            dmaOut->startLowPowerTransition(LPADomainID,getOMXHandle());
            mLowPowerEnabled = OMX_TRUE;
    }
    else{
        return OMX_ErrorUndefined;
    }
    return OMX_ErrorNone;

}

OMX_ERRORTYPE IntHandsFree::StartLowPower()
{
    LOG_I("IHF: Start Low Power No domain.\n");

    AudioRendererAb8500NmfMpc *processingComponent = static_cast<AudioRendererAb8500NmfMpc*>(&getProcessingComponent());
    NmfMpcOutAb8500 *dmaOut = processingComponent->getNmfMpcOutAb8500();

    if(dmaOut != NULL){
            eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, (OMX_U32)OMX_ALL, (OMX_U32)AFM_IndexLowPowerRenderer, (OMX_PTR) 0);
            dmaOut->startLowPowerTransition(getOMXHandle());
            mLowPowerEnabled = OMX_TRUE;
    }
    else{
        return OMX_ErrorUndefined;
    }
    return OMX_ErrorNone;

}

OMX_ERRORTYPE IntHandsFree::StopLowPower(void)
{
    AudioRendererAb8500NmfMpc *processingComponent = static_cast<AudioRendererAb8500NmfMpc*>(&getProcessingComponent());
    NmfMpcOutAb8500 *dmaOut = processingComponent->getNmfMpcOutAb8500();

    LOG_I("IHF: Stop Low Power.\n");

    if(dmaOut != NULL){
        dmaOut->startNormalModeTransition(getOMXHandle());
        mLowPowerEnabled = OMX_FALSE;
        eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, (OMX_U32)OMX_ALL, (OMX_U32)AFM_IndexLowPowerRenderer, (OMX_PTR) 0);
    }
    else{
        return OMX_ErrorUndefined;
    }
    return OMX_ErrorNone;
}

IntHandsFree_RDB::IntHandsFree_RDB(OMX_U32 nbOfDomains) : ENS_ResourcesDB(nbOfDomains) {
    for (unsigned int i=0;i<nbOfDomains;i++) {
        setDefaultNMFDomainType(RM_NMFD_PROCSIA,i);
    }
}
