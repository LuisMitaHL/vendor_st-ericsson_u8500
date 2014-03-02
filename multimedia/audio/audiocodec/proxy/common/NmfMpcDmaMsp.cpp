/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcDmaMsp.cpp
 * \brief  MspDma (Ab8500 and BT) Nmf Mpc proxy classes implementation
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "NmfMpcDmaMsp.h"
#include "wrapping_macros.h"
#include "alsactrl_audiocodec.h"
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_proxy_common_NmfMpcDmaMspTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE
#include <cm/inc/cm.hpp>
#include <cm/proxy/api/dma_proxy.h>

//-----------------------------------------------------------------------------
//                  Class NmfMpcDmaMspCtrl implementation
//-----------------------------------------------------------------------------

int NmfMpcDmaMspCtrl::mRefCount = 0;
NmfMpcDmaMspCtrl * NmfMpcDmaMspCtrl::mDmaMspCtrl = 0;

NmfMpcDmaMspCtrl * NmfMpcDmaMspCtrl::getHandle(void) {
    if (mRefCount++) {
        return mDmaMspCtrl;
    }
    mDmaMspCtrl = new NmfMpcDmaMspCtrl();

    return mDmaMspCtrl;
}

void NmfMpcDmaMspCtrl::freeHandle() {
    if (--mRefCount) {
        return;
    }
    delete mDmaMspCtrl;
    mDmaMspCtrl = 0;
}

OMX_ERRORTYPE NmfMpcDmaMspCtrl::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    t_cm_instance_handle & nmfMain = getNmfMpcHandle();

    if (mInstantiateCount++) {
        return OMX_ErrorNone;
    }

    RETURN_IF_OMX_ERROR ( instantiateNMFComponent( domainId, "osttrace.mmdsp", "OSTTRACE", &mOstTrace, priority) );
    RETURN_IF_OMX_ERROR ( instantiateNMFComponent( domainId, "audiocodec.mpc.host_reg", "host_reg", &mNmfHostReg, priority) );
    RETURN_IF_OMX_ERROR ( instantiateNMFComponent( domainId, "audiocodec.mpc.msp_dma_controller", "msp_dma_control", & nmfMain, priority) );

    RETURN_IF_OMX_ERROR ( bindComponent( nmfMain, "osttrace", mOstTrace, "osttrace") ); 
    RETURN_IF_OMX_ERROR ( bindComponent( nmfMain, "hostReg", mNmfHostReg, "hostReg") );

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcDmaMspCtrl::deInstantiate() {

    if (--mInstantiateCount) {
        return OMX_ErrorNone;
    }
  
    RETURN_IF_OMX_ERROR ( unbindComponent(getNmfMpcHandle(), "osttrace") );
    RETURN_IF_OMX_ERROR ( unbindComponent(getNmfMpcHandle(), "hostReg") );
    
	RETURN_IF_OMX_ERROR ( destroyNMFComponent(mOstTrace) );
    RETURN_IF_OMX_ERROR ( destroyNMFComponent(mNmfHostReg) );
    RETURN_IF_OMX_ERROR ( destroyNMFComponent(getNmfMpcHandle()) );

    return OMX_ErrorNone;
}
  
OMX_ERRORTYPE NmfMpcDmaMspCtrl::start() {

    if (mStartCount++) {
        //OstTraceInt1(TRACE_DEBUG, "NmfMpcDmaMspCtrl::start: mStartCount=%d",mStartCount);
        return OMX_ErrorNone;
    }
    
    RETURN_IF_OMX_ERROR ( startNMFComponent(mOstTrace) );
    RETURN_IF_OMX_ERROR ( startNMFComponent(mNmfHostReg) );
    RETURN_IF_OMX_ERROR ( startNMFComponent(getNmfMpcHandle()) );

    // OstTraceInt0(TRACE_DEBUG, "NmfMpcDmaMspCtrl::start  return");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcDmaMspCtrl::stop() {

    if (--mStartCount) {
        //OstTraceInt1(TRACE_DEBUG, "NmfMpcDmaMspCtrl::stop: mStartCount=%d",mStartCount);
        return OMX_ErrorNone;
    }

    RETURN_IF_OMX_ERROR ( stopNMFComponent(mOstTrace) );
    RETURN_IF_OMX_ERROR ( stopNMFComponent(mNmfHostReg) );
    RETURN_IF_OMX_ERROR ( stopNMFComponent(getNmfMpcHandle()) );
    //OstTraceInt0(TRACE_DEBUG, "NmfMpcDmaMspCtrl::stop  return");
    return OMX_ErrorNone;
}

//-----------------------------------------------------------------------------
//                  Class NmfMpcDmaMspBase implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE NmfMpcDmaMspBase::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    if (mInstantiateCount++) {
        return OMX_ErrorNone;
    }

    instantiateMain(domainId, priority);

    RETURN_IF_OMX_ERROR ( mDmaMspCtrl->instantiate(domainId, priority) );

    RETURN_IF_OMX_ERROR ( bindComponent( getNmfMpcHandle(), "osttrace"     , mDmaMspCtrl->getNmfOstTrace()                    , "osttrace"     ) );
    RETURN_IF_OMX_ERROR ( bindComponent( getNmfMpcHandle(), "hostReg"      , mDmaMspCtrl->getNmfHostReg()                     , "hostReg"      ) );
    RETURN_IF_OMX_ERROR ( bindComponent( getNmfMpcHandle(), "mspDmaControl", mDmaMspCtrl->getNmfMpcHandle()                   , "mspDmaControl") );

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcDmaMspBase::deInstantiate() {
    if (--mInstantiateCount) {
        return OMX_ErrorNone;
    }
  
    RETURN_IF_OMX_ERROR ( unbindComponent(getNmfMpcHandle(), "osttrace") );
    RETURN_IF_OMX_ERROR ( unbindComponent(getNmfMpcHandle(), "hostReg") );  
    RETURN_IF_OMX_ERROR ( unbindComponent(getNmfMpcHandle(), "mspDmaControl") );

    deInstantiateMain();

    RETURN_IF_OMX_ERROR ( destroyNMFComponent(getNmfMpcHandle()) );

    mDmaMspCtrl->deInstantiate();

    return OMX_ErrorNone;
}
  
OMX_ERRORTYPE NmfMpcDmaMspBase::start() {
    if (mStartCount++) {
        return OMX_ErrorNone;
    }
    
    RETURN_IF_OMX_ERROR ( mDmaMspCtrl->start() );

    RETURN_IF_OMX_ERROR ( startNMFComponent(getNmfMpcHandle()) );

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcDmaMspBase::stop() {
    if (--mStartCount) {
        return OMX_ErrorNone;
    }
    
    RETURN_IF_OMX_ERROR ( stopNMFComponent(getNmfMpcHandle()) );
    
    RETURN_IF_OMX_ERROR ( mDmaMspCtrl->stop() );

    return OMX_ErrorNone;
}

//-----------------------------------------------------------------------------
//                  Class NmfMpcDmaMspAb8500 implementation
//-----------------------------------------------------------------------------
int                  NmfMpcDmaMspAb8500::mRefCount = 0;
NmfMpcDmaMspAb8500 * NmfMpcDmaMspAb8500::mDma      = 0;

NmfMpcDmaMspAb8500 * NmfMpcDmaMspAb8500::getHandle(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels) {
    if (mRefCount++) {
        return mDma;
    }
    mDma = new NmfMpcDmaMspAb8500(monoBlockSizeInSamples, nbMspChannels);

    return mDma;
}

void NmfMpcDmaMspAb8500::freeHandle() {
    if (--mRefCount) {
        return;
    }
    delete mDma;
    mDma = 0;
}
       
NmfMpcDmaMspAb8500::NmfMpcDmaMspAb8500(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels)
    : NmfMpcDmaMspBase(nbMspChannels), mMonoBlockSizeInSamples(monoBlockSizeInSamples) {
        mBindContext    = BindContext::getHandle();
        mDmaMspCtrl     = NmfMpcDmaMspCtrl::getHandle();
        mDoubleBufferTx = (t_cm_memory_handle)0;
    };

NmfMpcDmaMspAb8500::~NmfMpcDmaMspAb8500(){
        mBindContext->freeHandle();
	mDmaMspCtrl->freeHandle();
}

OMX_U32 NmfMpcDmaMspAb8500::getBufferSizeInSample(void){
    return mMonoBlockSizeInSamples * AB_8500_TX_MSP_SLOTS_MAX;
}

OMX_ERRORTYPE NmfMpcDmaMspAb8500::instantiateMain(OMX_U32 domainId, OMX_U32 priority) {
    t_cm_instance_handle & nmfMain = getNmfMpcHandle();

    RETURN_IF_OMX_ERROR ( instantiateNMFComponent( domainId, "audiocodec.mpc.msp_dma_ctrl_ab8500", "msp_dma_ctrl_ab8500", & nmfMain, priority) );
    RETURN_IF_OMX_ERROR ( bindComponentFromHost(nmfMain, "configure", &mIconfigure, FIFO_DEPTH_ONE) );
   
    RETURN_IF_OMX_ERROR ( ENS::localBindComponentToUser(mBindContext->getBindContext(), nmfMain, "bitclock", (msp_dma_ctrl_ab8500_bitclockDescriptor*)this, FIFO_DEPTH_ONE) );
#ifdef AUDIOCODEC_U8500
    RETURN_IF_OMX_ERROR ( ENS::allocMpcMemory( domainId, CM_MM_MPC_ESRAM24, getBufferSizeInSample() * JITTERBUFFER_TX_MS, CM_MM_ALIGN_WORD, &mDoubleBufferTx) );
#else
    RETURN_IF_OMX_ERROR ( ENS::allocMpcMemory( domainId, CM_MM_MPC_SDRAM24, getBufferSizeInSample() * JITTERBUFFER_TX_MS, CM_MM_ALIGN_WORD, &mDoubleBufferTx) );
#endif
    CM_SetupRelinkAreaMem2Per(mDoubleBufferTx,0x80124000,JITTERBUFFER_TX_MS,192*AB_8500_TX_MSP_SLOTS_MAX,1);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcDmaMspAb8500::deInstantiateMain() {

    RETURN_IF_OMX_ERROR ( unbindComponentFromHost( &mIconfigure) );

    RETURN_IF_OMX_ERROR ( ENS::localUnbindComponentToUser(mBindContext->getBindContext(), getNmfMpcHandle(), "bitclock") );
    
    RETURN_IF_OMX_ERROR ( ENS::freeMpcMemory(mDoubleBufferTx) );
    
    return OMX_ErrorNone;
}
  
OMX_U32 NmfMpcDmaMspAb8500::getTxDoubleBufferDspAddr(void){
    t_uint32 bufferDspAddr;
    CM_GetMpcMemoryMpcAddress(mDoubleBufferTx, &bufferDspAddr);
    return bufferDspAddr;
}

void NmfMpcDmaMspAb8500::enable(t_bool on_off) {
    if (on_off) {
        audio_hal_start_fsbitclk();
        //OstTraceInt0(TRACE_DEBUG, "NmfMpcDmaMspAb8500::enable  audio_start_fsbitclk");
    } else {
        audio_hal_set_burst_device(AUDIO_HAL_CHANNEL_HSETOUT);
        audio_hal_stop_fsbitclk();
        //OstTraceInt0(TRACE_DEBUG, "NmfMpcDmaMspAb8500::enable  audio_stop_fsbitclk");
    }
}

OMX_ERRORTYPE NmfMpcDmaMspAb8500::configure() {
    t_uint16 target;

#ifdef AUDIOCODEC_U8500
    target = 0;
#elif defined(AUDIOCODEC_U9540)
    target = 1;
#else
    target = 2;
#endif

    if (mConfigured) {
        return OMX_ErrorNone;
    }
    mConfigured = true;
    
    mIconfigure.setParameter((void *)getTxDoubleBufferDspAddr(), getBufferSizeInSample(), mNbMspChannelEnabled, target);
    return OMX_ErrorNone;
}



//-----------------------------------------------------------------------------
//                  Class NmfMpcDmaMspBT implementation
//-----------------------------------------------------------------------------
int              NmfMpcDmaMspBT::mRefCount = 0;
NmfMpcDmaMspBT * NmfMpcDmaMspBT::mDmaBT    = 0;

NmfMpcDmaMspBT * NmfMpcDmaMspBT::getHandle() {
    if (mRefCount++) {
        return mDmaBT;
    }
    mDmaBT = new NmfMpcDmaMspBT();

    return mDmaBT;
}

void NmfMpcDmaMspBT::freeHandle() {
    if (--mRefCount) {
        return;
    }
    delete mDmaBT;
    mDmaBT = 0;
}

NmfMpcDmaMspBT::NmfMpcDmaMspBT() : NmfMpcDmaMspBase(BT_TOTAL_NB_CHANNELS){ 
        mDmaMspCtrl = NmfMpcDmaMspCtrl::getHandle();
    };


OMX_ERRORTYPE NmfMpcDmaMspBT::instantiateMain(OMX_U32 domainId, OMX_U32 priority) {
    t_cm_instance_handle & nmfMain = getNmfMpcHandle();
    RETURN_IF_OMX_ERROR ( instantiateNMFComponent( domainId, "audiocodec.mpc.msp_dma_ctrl_bt", "msp_dma_ctrl_bt", &nmfMain, priority) );
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcDmaMspBT::deInstantiateMain() {
    return OMX_ErrorNone;
}


