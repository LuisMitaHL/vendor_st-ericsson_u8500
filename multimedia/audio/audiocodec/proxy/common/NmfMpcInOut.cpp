/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcInOut.cpp
 * \brief Input / Output for Ab8500 and BT Nmf Mpc proxies implementation
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "NmfMpcDmaMsp.h"
#include "NmfMpcInOut.h"
#include "alsactrl_audiocodec.h"

#include "wrapping_macros.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_proxy_common_NmfMpcInOutTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE

#include "audiocodec_debug.h"
#include <cm/inc/cm.hpp>
#include <cm/proxy/api/dma_proxy.h>
#include <linux_utils.h>

//-----------------------------------------------------------------------------
//                  Class NmfMpcOutAb8500 implementation
//-----------------------------------------------------------------------------
//-------------------------------------------------
// Singleton implementation
//-------------------------------------------------
int               NmfMpcOutAb8500::mRefCount = 0;
NmfMpcOutAb8500 * NmfMpcOutAb8500::mDmaOut   = 0;

NmfMpcOutAb8500 * NmfMpcOutAb8500::getHandle(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels) {
    if (mRefCount++) {
        return mDmaOut;
    }
    mDmaOut = new NmfMpcOutAb8500(monoBlockSizeInSamples, nbMspChannels);

    return mDmaOut;

}

void NmfMpcOutAb8500::freeHandle() {
    if (--mRefCount) {
        return;
    }
    delete mDmaOut;
    mDmaOut = 0;
}

NmfMpcOutAb8500::NmfMpcOutAb8500(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels) : NmfMpcBaseAb8500("dmaout", AB_8500_DMAOUT_NB_MNF_PORTS, monoBlockSizeInSamples, nbMspChannels){
    mBindContext     = BindContext::getHandle();
    mRingBufferSDRAM = (t_cm_memory_handle)0;
    mRingBufferESRAM = (t_cm_memory_handle)0;
    mHdlToBeAcknowledged = NULL;
    mDomainId = 0;
    
    memset(clients,0,sizeof(clients));
};

NmfMpcOutAb8500::~NmfMpcOutAb8500(){
   mBindContext->freeHandle();
}


//-------------------------------------------------
//  Instantiation / Deinstantiation
//-------------------------------------------------
OMX_ERRORTYPE NmfMpcOutAb8500::instantiateLibraries(OMX_U32 domainId, OMX_U32 priority){
    mDomainId = domainId;

    RETURN_IF_OMX_ERROR( NmfMpcBaseAb8500::instantiateLibraries(domainId, priority) );

    RETURN_IF_OMX_ERROR( bindComponentAsynchronous( getNmfMpcHandle(), "feedback_me", getNmfMpcHandle(), "feedback_postevent", FIFO_DEPTH_ONE) );
    RETURN_IF_OMX_ERROR( bindComponentFromHost( getNmfMpcHandle(), "low_power",  &mLowPower, FIFO_DEPTH_ONE) );
    RETURN_IF_OMX_ERROR(  ENS::localBindComponentToUser(mBindContext->getBindContext(),getNmfMpcHandle(), "low_signal", static_cast<dmaout_low_signalDescriptor*>(this), 3) );

    RETURN_IF_OMX_ERROR(allocateRingBuffer(domainId));
  
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcOutAb8500::deInstantiateLibraries() { 

    RETURN_IF_OMX_ERROR( unbindComponentFromHost( &mLowPower) );   
    RETURN_IF_OMX_ERROR( ENS::localUnbindComponentToUser(mBindContext->getBindContext(), getNmfMpcHandle(), "low_signal") );
    RETURN_IF_OMX_ERROR( unbindComponentAsynchronous( getNmfMpcHandle(), "feedback_me") );
    RETURN_IF_OMX_ERROR( freeRingBuffer() );

    if (mRingBufferESRAM != (t_cm_memory_handle) 0) {
        ENS::freeMpcMemory(mRingBufferESRAM);
        mRingBufferESRAM = (t_cm_memory_handle)0;
    }
    
    RETURN_IF_OMX_ERROR( NmfMpcBaseAb8500::deInstantiateLibraries() );

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcOutAb8500::allocateRingBuffer(OMX_U32 domainId) {
    mDomainId = domainId;
    // + 8 for workaround of frame counter in 4500
    unsigned int buffer_size_in_mmdsp_MAU = mDmaMspAb8500->getMonoBlockSizeInSamples() * AB_8500_RING_BUFFER_SLOTS * 2;

    RETURN_IF_OMX_ERROR( ENS::allocMpcMemory( domainId,
                                              CM_MM_MPC_SDRAM24,
                                              buffer_size_in_mmdsp_MAU + 8 ,
                                              CM_MM_ALIGN_WORD,
                                              &mRingBufferSDRAM) );
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcOutAb8500::freeRingBuffer() {
    return ENS::freeMpcMemory(mRingBufferSDRAM);
}

// For Ab8500 Tx double buffer is already allocated by msp_dma_ctrl_ab8500
OMX_ERRORTYPE NmfMpcOutAb8500::allocateDoubleBuffer(OMX_U32 domainId){

    mMmdspConfig.buffer      = (void *) mDmaMspAb8500->getTxDoubleBufferDspAddr();
    mMmdspConfig.buffer_size = mDmaMspAb8500->getMonoBlockSizeInSamples() * AB_8500_TX_MSP_SLOTS_MAX * JITTERBUFFER_TX_MS;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcOutAb8500::freeDoubleBuffer(void) {
    return OMX_ErrorNone;
}

//-------------------------------------------------
//  Low Power 
//-------------------------------------------------
void  NmfMpcOutAb8500::acknowledgeRmForLowPowerMode(void) {
    LOG_I("Acknowledge low power enter with handle %d.", (int)mHdlToBeAcknowledged);
    if (mHdlToBeAcknowledged != NULL) {
        // Notify RM that we have entered burst mode
        ENS_HwRm::HWRM_Notify_RMConfigApplied(mHdlToBeAcknowledged, RM_RC_AUDIOLOWPOWER_ENTER, 0, RM_E_NONE);
    }
}

void  NmfMpcOutAb8500::lowPowerModeSetup( t_uint16 port_idx)
{
    audio_hal_channel ch;
    switch (port_idx) {
        case AVSINK_INPUT_PORT_IDX:
            ch = AUDIO_HAL_CHANNEL_HSETOUT;
            break;
        case  IHF_INPUT_PORT_IDX:
            ch = AUDIO_HAL_CHANNEL_SPEAKER;
            break;
        default:
            ch = AUDIO_HAL_CHANNEL_HSETOUT;
            break;
    }

    LOG_I("Setup burst mode port_idx = %u\n", port_idx);
    int ret = audio_hal_set_burst_device(ch);
    if (ret != 0)
         LOG_E("ERROR: Failure to set burst device. (%u)!",(unsigned int) port_idx);
}

void  NmfMpcOutAb8500::lowPowerMode( t_uint16 framecount)
{
    LOG_I("Switching to burst-mode at framecount = %d\n", (int)framecount);

    int ret = audio_hal_switch_to_burst((unsigned int)framecount);
    if (ret != 0)
        LOG_E("ERROR: Switching to burst-mode failed (%d)!", ret);
}

void  NmfMpcOutAb8500::normalMode()
{
    LOG_I("Switching to normal-mode.");

    int ret = audio_hal_switch_to_normal();
    if (ret != 0)
        LOG_E("ERROR: Switching to normal-mode failed (%d)!", ret);

    if (mRingBufferESRAM != (t_cm_memory_handle) 0) {
        ENS::freeMpcMemory(mRingBufferESRAM);
        mRingBufferESRAM = (t_cm_memory_handle)0;
    }

    LOG_I("Acknowledge low power exit with handle %d.", (int)mHdlToBeAcknowledged);
    if (mHdlToBeAcknowledged != NULL) {
        // Notify RM that we have entered normal mode
        ENS_HwRm::HWRM_Notify_RMConfigApplied(mHdlToBeAcknowledged, RM_RC_AUDIOLOWPOWER_EXIT, 0, RM_E_NONE);
    }
    //OstTraceInt0(TRACE_DEBUG, "NmfMpcOutAb8500::normalMode");
}

void NmfMpcOutAb8500::fifoEmptied() {
    LOG_I("FIFO emptied!");
    int ret = audio_hal_set_burst_device(AUDIO_HAL_CHANNEL_HSETOUT);
    if (ret != 0)
         LOG_E("ERROR: Failure to reset burst device!");
}

void NmfMpcOutAb8500::portMuted(t_uint16 port_idx) {

    LOG_I("Port muted: %u",port_idx);

    if (clients[port_idx])
        clients[port_idx]->invoke_mute(OMX_TRUE);
}

void NmfMpcOutAb8500::digitalMute(t_uint16 mute) {

    LOG_I("Digital mute: %u\n", mute);

    if (mute) {
        audio_hal_digital_mute(1);
    } else {
        audio_hal_digital_mute(0);
    }
}

void NmfMpcOutAb8500::startLowPowerTransition(OMX_HANDLETYPE hdlToBeAcknowledged)
{
    LOG_I("Enter.");

    mHdlToBeAcknowledged = hdlToBeAcknowledged;
    OMX_ERRORTYPE error;

    LOG_I("startLowPower LPA");

    unsigned int buffer_size_in_mmdsp_MAU = mDmaMspAb8500->getMonoBlockSizeInSamples() * AB_8500_RING_BUFFER_SLOTS * 2;
    error = ENS::allocMpcMemory(mDomainId, CM_MM_MPC_ESRAM24,
                                buffer_size_in_mmdsp_MAU + 8,  CM_MM_ALIGN_WORD,
                                &mRingBufferESRAM);

    OMX_U32 dmaRingBufferDspAddr = 0;
    if (error == OMX_ErrorNone) {
        CM_GetMpcMemoryMpcAddress(mRingBufferESRAM, &dmaRingBufferDspAddr);
    } else {
        LOGE("LPA: No ESRAM available!\n");
	OstTraceInt0(TRACE_DEBUG, "NmfMpcOutAb8500::startLowPowerTransition, no ESRAM, will fall back to pre-allocated SDRAM");
        CM_GetMpcMemoryMpcAddress(mRingBufferSDRAM, &dmaRingBufferDspAddr);
    }
    
    mLowPower.startLowPowerTransition((void*) dmaRingBufferDspAddr, buffer_size_in_mmdsp_MAU);
    LOGI("Low power audio activated\n");
}

void NmfMpcOutAb8500::startLowPowerTransition(OMX_U32 LPADomainID, OMX_HANDLETYPE hdlToBeAcknowledged)
{
    LOG_I("Enter.");

    OMX_ERRORTYPE error;

    LOG_I("startLowPower LPA Domain ID : %u",(unsigned int)LPADomainID);

    mHdlToBeAcknowledged = hdlToBeAcknowledged;

    unsigned int buffer_size_in_mmdsp_MAU = mDmaMspAb8500->getMonoBlockSizeInSamples() * AB_8500_RING_BUFFER_SLOTS * 2;
    error = ENS::allocMpcMemory(LPADomainID, CM_MM_MPC_ESRAM24,
                                buffer_size_in_mmdsp_MAU + 8,  CM_MM_ALIGN_WORD,
                                &mRingBufferESRAM);

    OMX_U32 dmaRingBufferDspAddr = 0;
    if (error == OMX_ErrorNone) {
        CM_GetMpcMemoryMpcAddress(mRingBufferESRAM, &dmaRingBufferDspAddr);
    } else {
        LOGE("LPA: No ESRAM available!\n");    
	OstTraceInt0(TRACE_DEBUG, "NmfMpcOutAb8500::startLowPowerTransition, no ESRAM, will fall back to pre-allocated SDRAM");
        CM_GetMpcMemoryMpcAddress(mRingBufferSDRAM, &dmaRingBufferDspAddr);
    }

    mLowPower.startLowPowerTransition((void*) dmaRingBufferDspAddr, buffer_size_in_mmdsp_MAU);
    LOGI("Low power audio activated\n");
}

void NmfMpcOutAb8500::startNormalModeTransition(OMX_HANDLETYPE hdlToBeAcknowledged)
{
    LOG_I("Enter.");

    mHdlToBeAcknowledged = hdlToBeAcknowledged;

    mLowPower.startNormalModeTransition();
    //OstTraceInt0(TRACE_DEBUG, "NmfMpcOutAb8500::startNormalModeTransition");
    LOGI("Low power audio de-activated\n");
}

void NmfMpcOutAb8500::mutePort(t_uint16 port_idx, AudioRendererAb8500NmfMpc *client)
{
    LOG_I("Enter.");
    clients[port_idx] = client;
    mLowPower.mutePort(port_idx);
}

//-----------------------------------------------------------------------------
//                  Class NmfMpcInAb8500 implementation
//-----------------------------------------------------------------------------
int              NmfMpcInAb8500::mRefCount = 0;
NmfMpcInAb8500 * NmfMpcInAb8500::mDmaIn    = 0;
     
NmfMpcInAb8500::NmfMpcInAb8500(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels): NmfMpcBaseAb8500( "dmain", AB_8500_DMAIN_NB_MNF_PORTS, monoBlockSizeInSamples, nbMspChannels){
};

NmfMpcInAb8500 * NmfMpcInAb8500::getHandle(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels) {
    if (mRefCount++) {
        return mDmaIn;
    }
    mDmaIn = new NmfMpcInAb8500(monoBlockSizeInSamples, nbMspChannels);

    return mDmaIn;
}

void NmfMpcInAb8500::freeHandle() {
    if (--mRefCount) {
        return;
    }
    delete mDmaIn;
    mDmaIn = 0;
}

OMX_ERRORTYPE NmfMpcInAb8500::allocateDoubleBuffer(OMX_U32 domainId) {

    //  TODO: Make this configurable
    unsigned int buffer_size_in_mmdsp_MAU = mDmaMspAb8500->getMonoBlockSizeInSamples() * AB_8500_RX_MSP_SLOTS_MAX * JITTERBUFFER_RX_MS;

#ifdef AUDIOCODEC_U8500
    RETURN_IF_OMX_ERROR( ENS::allocMpcMemory( domainId, CM_MM_MPC_ESRAM24, buffer_size_in_mmdsp_MAU,  CM_MM_ALIGN_WORD, &mDoubleBuffer) );
#else
    RETURN_IF_OMX_ERROR( ENS::allocMpcMemory( domainId, CM_MM_MPC_SDRAM24, buffer_size_in_mmdsp_MAU,  CM_MM_ALIGN_WORD, &mDoubleBuffer) );
#endif

    CM_SetupRelinkAreaPer2Mem(mDoubleBuffer,0x80125000,10, 192 * 4, LLI_RX_TDM_SLOTS_4);
    CM_SetupRelinkAreaPer2Mem(mDoubleBuffer,0x80125000,10, 192 * 2, LLI_RX_TDM_SLOTS_2);
    CM_SetupRelinkAreaPer2Mem(mDoubleBuffer,0x80125000,10, 192 * 1, LLI_RX_TDM_SLOTS_1);

    OMX_U32 dmaBufferDspAddr;
    CM_GetMpcMemoryMpcAddress(mDoubleBuffer, &dmaBufferDspAddr);
    
    mMmdspConfig.buffer      = (void *) dmaBufferDspAddr;
    mMmdspConfig.buffer_size = buffer_size_in_mmdsp_MAU;
    
    return OMX_ErrorNone;
}

//-----------------------------------------------------------------------------
//                  Class NmfMpcOutBT implementation
//-----------------------------------------------------------------------------
int           NmfMpcOutBT::mRefCount = 0;
NmfMpcOutBT * NmfMpcOutBT::mDmaBTOut = 0;

NmfMpcOutBT * NmfMpcOutBT::getHandle(OMX_U32 monoBlockSizeInSamples) {
    if (mRefCount++) {
        return mDmaBTOut;
    }
    mDmaBTOut = new NmfMpcOutBT(monoBlockSizeInSamples);

    return mDmaBTOut;

}

void NmfMpcOutBT::freeHandle() {
    if (--mRefCount) {
        return;
    }
    delete mDmaBTOut;
    mDmaBTOut = 0;
}

NmfMpcOutBT::NmfMpcOutBT(OMX_U32 monoBlockSizeInSamples) : NmfMpcBaseBT("dmaout_bt", monoBlockSizeInSamples){
};


OMX_ERRORTYPE NmfMpcOutBT::instantiateLibraries(OMX_U32 domainId, OMX_U32 priority){
    RETURN_IF_OMX_ERROR( NmfMpcBase::instantiateLibraries(domainId, priority) );

    RETURN_IF_OMX_ERROR( bindComponentAsynchronous( getNmfMpcHandle(), "feedback_me", getNmfMpcHandle(), "feedback_postevent", FIFO_DEPTH_ONE) );    
    return OMX_ErrorNone ; 
}

OMX_ERRORTYPE NmfMpcOutBT::deInstantiateLibraries(void) { 
    RETURN_IF_OMX_ERROR( unbindComponentAsynchronous( getNmfMpcHandle(), "feedback_me") );
    RETURN_IF_OMX_ERROR( NmfMpcBase::deInstantiateLibraries() );
    return OMX_ErrorNone ; 
}

//-----------------------------------------------------------------------------
//                  Class NmfMpcOutBT implementation
//-----------------------------------------------------------------------------
int          NmfMpcInBT::mRefCount = 0;
NmfMpcInBT * NmfMpcInBT::mDmaBTIn  = 0;

NmfMpcInBT * NmfMpcInBT::getHandle(OMX_U32 monoBlockSizeInSamples) {
    if (mRefCount++) {
        return mDmaBTIn;
    }
    mDmaBTIn = new NmfMpcInBT(monoBlockSizeInSamples);

    return mDmaBTIn;

}

void NmfMpcInBT::freeHandle() {
    if (--mRefCount) {
        return;
    }
    delete mDmaBTIn;
    mDmaBTIn = 0;
}

NmfMpcInBT::NmfMpcInBT(OMX_U32 monoBlockSizeInSamples) : NmfMpcBaseBT("dmain_bt", monoBlockSizeInSamples){
};


