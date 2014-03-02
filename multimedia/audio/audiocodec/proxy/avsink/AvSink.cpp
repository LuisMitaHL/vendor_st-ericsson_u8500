/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Implementation of AV Sink
 * \author  ST-Ericsson
 */
/*****************************************************************************/
#include "AvSink.h"
#include "audio_chipset_api_index.h"
#include "NmfMpcInOut.h"
#include "audiocodec_debug.h"
#include "OMX_CoreExt.h"

//-----------------------------------------------------------------------------
//                  Factory
//-----------------------------------------------------------------------------
OMX_ERRORTYPE AvSinkFactoryMethod(ENS_Component_p * ppENSComponent) {
    OMX_ERRORTYPE error = OMX_ErrorNone;

    LOG_I("Enter.\n");

    AVSink *nokiaAVSink = new AVSink;
    if (nokiaAVSink == 0) {
        return OMX_ErrorInsufficientResources;
    }

    error = nokiaAVSink->construct();
    if (error != OMX_ErrorNone) {
        delete nokiaAVSink;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = nokiaAVSink;
    }

    AvSinkNmfMpc *nokiaAvSink_ProcessingComp = new AvSinkNmfMpc(*nokiaAVSink, AUDIO_HAL_CHANNEL_HSETOUT);
    if (nokiaAvSink_ProcessingComp == 0) {
        return OMX_ErrorInsufficientResources;
    }

    nokiaAVSink->setProcessingComponent(nokiaAvSink_ProcessingComp);

    return error;
}

//-----------------------------------------------------------------------------
//                  Class AvSinkNmfMpc implementation
//-----------------------------------------------------------------------------
AvSinkNmfMpc * AvSinkNmfMpc::myself = 0;

OMX_ERRORTYPE AvSinkNmfMpc::construct() {
	myself = this;
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::construct() );
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AvSinkNmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<AvSinkNmfMpc *>(NULL);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AvSinkNmfMpc::configureMain(void) {
    // Here is righ place to put specific Apply config calls
    return AudioRendererAb8500NmfMpc::configureMain();
}

OMX_ERRORTYPE AvSinkNmfMpc::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioRendererAb8500NmfMpc::applyConfig(
                    nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AvSinkNmfMpc::retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
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

void AvSinkNmfMpc::statusCB(AUDIO_HAL_STATUS status) {

    LOG_I("Enter (status = %d).\n", (int)status);

    if(myself){
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}

//-----------------------------------------------------------------------------
//                  Class AVSink_RDB implementation
//-----------------------------------------------------------------------------
AVSink_RDB::AVSink_RDB(OMX_U32 nbOfDomains) : ENS_ResourcesDB(nbOfDomains) {
    //TODO: Why not having a function setDefaultNMFDomainType without second parameter
    for (unsigned int i=0;i<nbOfDomains;i++) {
        setDefaultNMFDomainType(RM_NMFD_PROCSIA,i);
    }
}

OMX_ERRORTYPE AVSink::createResourcesDB() {
    mRMP = new AVSink_RDB(getPortCount());
    if (mRMP == 0) {
        return OMX_ErrorInsufficientResources;
    }
    else {
        return OMX_ErrorNone;
    }
}

//-----------------------------------------------------------------------------
//                  Class AVSink implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE AVSink::construct(void) {
    RETURN_IF_OMX_ERROR(AudioRendererAb8500::construct(AVSINK_NB_CHANNEL, SUPPORT_VOLUME_CONFIG, HAVE_FEEDBACK_PORT) );

    ENS_String<30> role = "audio_sink.pcm.nokiaav";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AVSink::getConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        //Here is right place to put specific indexes
		default:
			return AudioRendererAb8500::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AVSink::setConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

	switch (nParamIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioRendererAb8500::setConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AVSink::StartLowPower(OMX_U32 LPADomainID)
{
    LOG_I("Enter.\n");

    AudioRendererAb8500NmfMpc *processingComponent = static_cast<AudioRendererAb8500NmfMpc*>(&getProcessingComponent());
    NmfMpcOutAb8500 *dmaOut = processingComponent->getNmfMpcOutAb8500();

    if(dmaOut != NULL){
            eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, (OMX_U32)OMX_ALL, (OMX_U32)AFM_IndexLowPowerRenderer, (OMX_PTR) 0);
            dmaOut->startLowPowerTransition(LPADomainID, getOMXHandle());
            mLowPowerEnabled = OMX_TRUE;
    }
    else{
        return OMX_ErrorUndefined;
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AVSink::StartLowPower(void)
{
    LOG_I("Enter.\n");

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

OMX_ERRORTYPE AVSink::StopLowPower(void)
{
    AudioRendererAb8500NmfMpc *processingComponent = static_cast<AudioRendererAb8500NmfMpc*>(&getProcessingComponent());
    NmfMpcOutAb8500 *dmaOut = processingComponent->getNmfMpcOutAb8500();

    LOG_I("Enter.\n");

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

