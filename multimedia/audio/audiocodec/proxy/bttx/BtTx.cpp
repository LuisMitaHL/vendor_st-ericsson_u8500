/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Implementation of BtTx Sink
 * \author  ST-Ericsson
 */
/*****************************************************************************/

#include "BtTx.h"
#include "NmfMpcInOut.h"
#include "audio_chipset_api_index.h"
#include "audiocodec_debug.h"

//-----------------------------------------------------------------------------
//                  Factory
//-----------------------------------------------------------------------------
OMX_ERRORTYPE BtTxFactoryMethod(ENS_Component_p * ppENSComponent)
{
    LOG_I("Enter.\n");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    BtTx *BtTxSink= new BtTx;
    if (BtTxSink == 0) {
        return OMX_ErrorInsufficientResources;
    }
    error = BtTxSink->construct();
    if (error != OMX_ErrorNone) {
        delete BtTxSink;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = BtTxSink;
    }
    
    BtTxNmfMpc *BtTxSink_ProcessingComp = new BtTxNmfMpc(*BtTxSink, AUDIO_HAL_CHANNEL_BTOUT);
    if (BtTxSink_ProcessingComp == 0) {
        delete BtTxSink_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    BtTxSink->setProcessingComponent(BtTxSink_ProcessingComp);

    return error;
}

//-----------------------------------------------------------------------------
//                  Class BtTxNmfMpc implementation
//-----------------------------------------------------------------------------
BtTxNmfMpc * BtTxNmfMpc::myself = 0;

NmfMpcBase * BtTxNmfMpc::getNmfMpcDmaHandle(const AudiocodecBase & audiocodec){
    OMX_U32 monoBlockSizeInSamples = audiocodec.getDMABufferSize();

    mDmaBTOut = NmfMpcOutBT::getHandle(monoBlockSizeInSamples);
    return mDmaBTOut;
}

void BtTxNmfMpc::freeDmaHandle(){
    mDmaBTOut->freeHandle();
    mDmaBTOut = static_cast<NmfMpcOutBT*>(NULL);
}

OMX_ERRORTYPE BtTxNmfMpc::construct() {
	myself = this; 
	RETURN_IF_OMX_ERROR( AudioRendererNmfMpc::construct() );
    return OMX_ErrorNone;
}

OMX_ERRORTYPE BtTxNmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioRendererNmfMpc::destroy() );
	myself = static_cast<BtTxNmfMpc *>(NULL); 
    return OMX_ErrorNone;
}
    
OMX_ERRORTYPE BtTxNmfMpc::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioRendererNmfMpc::applyConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE BtTxNmfMpc::retrieveConfig( OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }
   
    switch (nConfigIndex) {
        //Here is right place to put specific indexes
        default:
            return AudioRendererNmfMpc::retrieveConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

ENS_String<32> BtTxNmfMpc::addSuffixPerPortNb(const char * str){
    //Note that we have only one port for BT
    ENS_String<32> ret(str);
    return ret;
}

void BtTxNmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself){
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}


//-----------------------------------------------------------------------------
//                  Class BtTx implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE BtTx::construct(void) {
    //TODO not use direct figures
    RETURN_IF_OMX_ERROR(AudioRenderer::construct(160, BT_BLOCKSIZE_5MS_NB, NB_CHANNEL_MONO, 8000 /*SamplingRate*/, DO_NOT_SUPPORT_VOLUME_CONFIG, HAVE_FEEDBACK_PORT)) ;

    ENS_String<30> role = "audio_sink.pcm.bt";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE BtTx::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        //Here is right place to put specific indexes
    	default:
    		return AudioRenderer::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE BtTx::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
	if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

	switch (nParamIndex) {
        //Here is right place to put specific indexes
		default:
			return AudioRenderer::setConfig(nParamIndex,pComponentConfigStructure);
    }
}

