/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcBase.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "NmfMpcBase.h"
#include "wrapping_macros.h"

//-----------------------------------------------------------------------------
//                  Class NmfMpcBase implementation
//-----------------------------------------------------------------------------
NmfMpcBase::NmfMpcBase(const char * inOutMpcName, const char * mspDmaCtrlItfName, unsigned int nbNmfPort)
    : mDmaMsp(0), mInstantiateCount(0), mStartCount(0), mStartDmaCount(0),
    mIsConfigured(false), mInOutMpcName(inOutMpcName), mMspDmaCtrInterfaceName(mspDmaCtrlItfName), mNbNmfPort(nbNmfPort),
    mSPHandle(0), mSParmAddr(0)         
{
    mDoubleBuffer       = (t_cm_memory_handle)0;
   
    // Reset param structure
    mMmdspConfig.buffer                   = 0;
    mMmdspConfig.buffer_size              = 0;
    mMmdspConfig.buffer_ring              = 0;
    mMmdspConfig.sample_count_buffer      = 0;
    mMmdspConfig.sample_count_buffer_size = 0;
    mMmdspConfig.nb_msp_channel_enabled   = 0;
    mMmdspConfig.samplerate               = 0;
};

OMX_ERRORTYPE NmfMpcBase::instantiateLibraries(OMX_U32 domainId, OMX_U32 priority) {

    RETURN_IF_OMX_ERROR( bindComponent( getNmfMpcHandle(), "osttrace"           , mDmaMsp->getNmfOstTrace()        , "osttrace") );
    RETURN_IF_OMX_ERROR( bindComponent( getNmfMpcHandle(), "hostReg"            , mDmaMsp->getNmfHostReg()         , "hostReg") );
    RETURN_IF_OMX_ERROR( bindComponent( getNmfMpcHandle(), "mspDmaControl"      , mDmaMsp->getNmfMspDmaController(), "mspDmaControl") );
    
    RETURN_IF_OMX_ERROR( bindComponent( getNmfMpcHandle(), mMspDmaCtrInterfaceName, (t_cm_instance_handle)mDmaMsp->getNmfHandle(), mMspDmaCtrInterfaceName) );   
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcBase::deInstantiateLibraries(void) {
    RETURN_IF_OMX_ERROR( unbindComponent(getNmfMpcHandle(), mMspDmaCtrInterfaceName) );

    RETURN_IF_OMX_ERROR( unbindComponent(getNmfMpcHandle(), "mspDmaControl") );
    RETURN_IF_OMX_ERROR( unbindComponent(getNmfMpcHandle(), "osttrace") );
    RETURN_IF_OMX_ERROR( unbindComponent(getNmfMpcHandle(), "hostReg") );

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcBase::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    if (mInstantiateCount++) {
        return OMX_ErrorNone;
    }

    ENS_String<32> compName("audiocodec.mpc.");
    compName.concat(mInOutMpcName);

    t_cm_instance_handle & nmfMain = getNmfMpcHandle();

    RETURN_IF_OMX_ERROR( instantiateNMFComponent( domainId, compName, mInOutMpcName, &nmfMain, priority) );
    RETURN_IF_OMX_ERROR( mDmaMsp->instantiate(domainId, priority) );
    RETURN_IF_OMX_ERROR( allocateDoubleBuffer(domainId) );
    RETURN_IF_OMX_ERROR( instantiateLibraries(domainId, priority) );
    RETURN_IF_OMX_ERROR( bindComponentAsynchronous( getNmfMpcHandle(), "me", getNmfMpcHandle(), "postevent", FIFO_DEPTH_ONE) );
    RETURN_IF_OMX_ERROR( bindComponentFromHost(getNmfMpcHandle(), "configure", &mIconfigure, FIFO_DEPTH_ONE) );
    
	//--------------------------------------------------------------------
    // Allocation of shared memory between mmdsp and arm for SamplesPlayed
	//--------------------------------------------------------------------
   	t_cm_system_address SYSmem;
    RETURN_IF_OMX_ERROR( ENS::allocMpcMemory(domainId, CM_MM_MPC_SDRAM24, mNbNmfPort * (sizeof(SamplesPlayed_t)/4), CM_MM_ALIGN_2WORDS, &mSPHandle) );
	t_cm_error cm_error = CM_GetMpcMemorySystemAddress(mSPHandle, &SYSmem);
	if (cm_error != CM_OK) {
		return OMX_ErrorUndefined;
	}
	mSParmAddr = (SamplesPlayed_t *)(SYSmem.logical);

    OMX_U32             SPdspAddr;
	cm_error = CM_GetMpcMemoryMpcAddress(mSPHandle, &SPdspAddr);
   	if (cm_error != CM_OK) {
		return OMX_ErrorUndefined;
	}

    mMmdspConfig.sample_count_buffer      = (void *)SPdspAddr;
    mMmdspConfig.sample_count_buffer_size = mNbNmfPort * 2; // 2 == SamplesPlayed_t size in Mmdsp MAU 
    
    // initial value
    for(unsigned int i = 0; i< mNbNmfPort; i++){
        mSParmAddr[i].samplesPlayedLH = 0;
        mSParmAddr[i].samplesPlayedLL = 0;
    }
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcBase::deInstantiate(void) {
    if (--mInstantiateCount) {
        return OMX_ErrorNone;
    }
	
    if (mSPHandle) {
	    ENS::freeMpcMemory(mSPHandle); 
    }
    
    RETURN_IF_OMX_ERROR( freeDoubleBuffer() );
    RETURN_IF_OMX_ERROR( unbindComponentAsynchronous(getNmfMpcHandle(), "me") );
    RETURN_IF_OMX_ERROR( unbindComponentFromHost(&mIconfigure) );
    RETURN_IF_OMX_ERROR( deInstantiateLibraries() );
    RETURN_IF_OMX_ERROR( destroyNMFComponent(getNmfMpcHandle()) );
    RETURN_IF_OMX_ERROR( mDmaMsp->deInstantiate() );
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcBase::configure(OMX_U16 samplerate) {
    OMX_ERRORTYPE error(OMX_ErrorNone);
    t_uint16 target;

#ifdef AUDIOCODEC_U8500
    target = 0;
#else
    target = 1;
#endif

    if (mIsConfigured) {
        return OMX_ErrorNone;
    }
    mIsConfigured = true;
    
    mMmdspConfig.samplerate = samplerate;

    error = mDmaMsp->configure();
    if (error != OMX_ErrorNone) return error;

    mMmdspConfig.nb_msp_channel_enabled = mDmaMsp->getNbMspEnabledChannels();

    mIconfigure.setParameter(mMmdspConfig,target);

    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfMpcBase::start() {
    OMX_ERRORTYPE error(OMX_ErrorNone );
    
    if (mStartCount++) {
        return OMX_ErrorNone;
    }

    error = mDmaMsp->start();
    if (error != OMX_ErrorNone) return error;

    // Start DmaIn or DmaOut
    return startNMFComponent(getNmfMpcHandle());
}

OMX_ERRORTYPE NmfMpcBase::stop() {
    OMX_ERRORTYPE error(OMX_ErrorNone);
    
    if (--mStartCount) {
        return OMX_ErrorNone;
    }

    error = mDmaMsp->stop();
    if (error != OMX_ErrorNone) return error;
    
    // Stop DmaIn or DmaOut
    return stopNMFComponent(getNmfMpcHandle());
    

}

OMX_ERRORTYPE NmfMpcBase::freeDoubleBuffer(void) {
    return ENS::freeMpcMemory(mDoubleBuffer);
}

OMX_S64 NmfMpcBase::getTimeInUs(unsigned int portIndex){
    DBC_ASSERT(portIndex < mNbNmfPort );
    if(mSParmAddr){
        return ((OMX_S64)(mSParmAddr[portIndex].samplesPlayedLH << 24) | (OMX_S64)(mSParmAddr[portIndex].samplesPlayedLL & 0xFFFFFF)) * NB_US_IN_ONE_MS;
    }
    return 0;
}

//-----------------------------------------------------------------------------
//                  Class NmfMpcBaseAb8500 implementation
//-----------------------------------------------------------------------------
NmfMpcBaseAb8500::NmfMpcBaseAb8500(const char * inOutMpcName, unsigned int nbNmfPort, OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels)
    : NmfMpcBase(inOutMpcName, "mspDmaControlAb8500", nbNmfPort)
{
    mDmaMsp = mDmaMspAb8500 = NmfMpcDmaMspAb8500::getHandle(monoBlockSizeInSamples, nbMspChannels);
};


//-----------------------------------------------------------------------------
//                  Class NmfMpcBaseAbBT implementation
//-----------------------------------------------------------------------------
NmfMpcBaseBT::NmfMpcBaseBT(const char * inOutMpcName, OMX_U32 monoBlockSizeInSamples)
    : NmfMpcBase(inOutMpcName, "mspDmaControlBT", BT_DMA_NB_MNF_PORTS), mMonoBlockSizeInSamples(monoBlockSizeInSamples) {
    mDmaMsp = mDmaMspBT = NmfMpcDmaMspBT::getHandle();
};

OMX_ERRORTYPE NmfMpcBaseBT::allocateDoubleBuffer(OMX_U32 domainId) {
    unsigned int buffer_size_in_mmdsp_MAU = mMonoBlockSizeInSamples * BT_TOTAL_NB_CHANNELS * 2;

    // *2 means Double Buffer
    RETURN_IF_OMX_ERROR( ENS::allocMpcMemory( domainId, CM_MM_MPC_SDRAM24 , buffer_size_in_mmdsp_MAU,  CM_MM_ALIGN_WORD, &mDoubleBuffer) );

    OMX_U32 dmaBufferDspAddr;
    CM_GetMpcMemoryMpcAddress(mDoubleBuffer, &dmaBufferDspAddr);
    mMmdspConfig.buffer      = (void *) dmaBufferDspAddr;
    mMmdspConfig.buffer_size = buffer_size_in_mmdsp_MAU;
 
    return OMX_ErrorNone;
}

