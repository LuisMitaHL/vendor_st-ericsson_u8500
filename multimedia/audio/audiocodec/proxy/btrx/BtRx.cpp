/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Implementation of BtRx Sink
 * \author  ST-Ericsson
 */
/*****************************************************************************/

#include "BtRx.h"
#include "NmfMpcInOut.h"
#include "audio_chipset_api_index.h"
#include "audiocodec_debug.h"

//-----------------------------------------------------------------------------
//                  Factory
//-----------------------------------------------------------------------------
OMX_ERRORTYPE BtRxFactoryMethod(ENS_Component_p * ppENSComponent)
{
    LOG_I("Enter.\n");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    BtRx *BtRxSource= new BtRx;
    if (BtRxSource == 0) {
        return OMX_ErrorInsufficientResources;
    }
    error = BtRxSource->construct();
    if (error != OMX_ErrorNone) {
        delete BtRxSource;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = BtRxSource;
    }

    BtRxNmfMpc *BtRxSource_ProcessingComp = new BtRxNmfMpc(*BtRxSource, AUDIO_HAL_CHANNEL_BTIN);
    if (BtRxSource_ProcessingComp == 0) {
        delete BtRxSource_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    BtRxSource->setProcessingComponent(BtRxSource_ProcessingComp);

    return error;
}

//-----------------------------------------------------------------------------
//                  Class BtRxNmfMpc implementation
//-----------------------------------------------------------------------------
BtRxNmfMpc * BtRxNmfMpc::myself = 0;

NmfMpcBase * BtRxNmfMpc::getNmfMpcDmaHandle(const AudiocodecBase & audiocodec){
    OMX_U32 monoBlockSizeInSamples = audiocodec.getDMABufferSize();

    mDmaBTIn = NmfMpcInBT::getHandle(monoBlockSizeInSamples);
    return mDmaBTIn;
}

void BtRxNmfMpc::freeDmaHandle(){
    mDmaBTIn->freeHandle();
    mDmaBTIn = static_cast<NmfMpcInBT*>(NULL);
}

OMX_ERRORTYPE BtRxNmfMpc::construct() {
	myself = this; 
	RETURN_IF_OMX_ERROR( AudioCapturerNmfMpc::construct() );
    return OMX_ErrorNone;
}
   
OMX_ERRORTYPE BtRxNmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioCapturerNmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<BtRxNmfMpc *>(NULL); 
    return OMX_ErrorNone;
}

OMX_ERRORTYPE BtRxNmfMpc::applyConfig(
    OMX_INDEXTYPE nConfigIndex,  
    OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioCapturerNmfMpc::applyConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE BtRxNmfMpc::retrieveConfig(
    OMX_INDEXTYPE nConfigIndex,  
    OMX_PTR pComponentConfigStructure) {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
   
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioCapturerNmfMpc::retrieveConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

ENS_String<32> BtRxNmfMpc::addSuffixPerPortNb(const char * str){
    //Note that we have only one port for BT, do not add index
    ENS_String<32> ret(str);
    return ret;
}

void BtRxNmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself){
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}



//-----------------------------------------------------------------------------
//                  Class BtRx implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE BtRx::construct(void) {
    //TODO: don t use direct figures, use define or ....
    RETURN_IF_OMX_ERROR( AudioCapturer::construct(160, BT_BLOCKSIZE_5MS_NB, NB_CHANNEL_MONO, 8000 /*SamplingRate*/, DO_NOT_SUPPORT_VOLUME_CONFIG) );

    ENS_String<30> role = "audio_source.pcm.bt";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    return OMX_ErrorNone;	
}

OMX_ERRORTYPE BtRx::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        //Here is right place to put specific indexes
    	default:
    		return AudioCapturer::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE BtRx::setConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

	switch (nParamIndex) {
        //Here is right place to put specific indexes
		default:
			return AudioCapturer::setConfig(nParamIndex,pComponentConfigStructure);
    }
}

