/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   wrapper.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <pcmprocessings/wrapper.nmf>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <dbc.h>
#include "fsm/component/include/Component.inl"

#ifndef _SIMU_
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_mpc_pcmprocessings_wrapper_src_wrapperTraces.h"
#endif
#else // _SIMU_*/
#include "libeffects/mpc/include/audiolibs_trace.h"
#endif // _SIMU_

#define IN  0
#define OUT 1
#define NB_PORTS 2

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////

static void *       mFifoIn[1];
static void *       mFifoOut[1];
static Port         mPorts[NB_PORTS];
static Component    mWrapper;
static PcmDump_t    mPcmDumpConfig[NB_PORTS];

static t_uint16     mProcessingMode = PCMPROCESSING_MODE_NOT_INPLACE;

static t_effect_config  mEffectConfig;
static t_uint16         mSendNewFormat = 0;

// AV synchronisation
// Note: this simple implementation assumes:
// - timestamp values are continue
// - there is no data outputed before starttime is received (no "decode only" frame as it is not yet implemented)
static  bool         mTimeStampUpdateFlag;
static  AVSynchro_t  mComputedTimeStamp;

#define DO_PROBE(port_idx, buf) pcmdump.probe((void*) buf->data, (t_uint24) buf->filledLen, &mPcmDumpConfig[port_idx])

////////////////////////////////////////////////////////////

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize) {
    PRECONDITION(chans_nb == mEffectConfig.infmt.nof_channels);
    // TODO FIXME: the wrapper should be informed of any new settings from elswhere than the newFormat
    // Do this ugly hack to avoid the assert, BUT this only works if the effect does not rely on the sampling frequency
    mEffectConfig.infmt.freq = freq;
    PRECONDITION(sample_bitsize == mEffectConfig.infmt.nof_bits_per_sample);

    if(Port_isEnabled(&mWrapper.ports[OUT])) {
        ASSERT(outputsettings.THIS);
        outputsettings.newFormat(freq, mEffectConfig.outfmt.nof_channels, sample_bitsize);
        mSendNewFormat = 0;
    } else {
        mSendNewFormat = 1;
    }
}


static void
run_inplace(Component *this) {
    int i;
    int eosReached = 0;
    Buffer_p pInputBuf = 0;
    Buffer_p pOutputBuf = 0;

    if (!Port_isEnabled(&this->ports[IN]) || !Port_isEnabled(&this->ports[OUT])) {
        return;
    }

    for (i = 0; i < Port_queuedBufferCount(&this->ports[OUT]); i++) {
        pOutputBuf = Port_dequeueBuffer(&this->ports[OUT]);
        Port_returnBuffer(&this->ports[IN], pOutputBuf);
    }

    if (!Port_queuedBufferCount(&this->ports[IN])){
        return;
    }

    pInputBuf = Port_dequeueBuffer(&this->ports[IN]);

    if ( pInputBuf->flags & BUFFERFLAG_STARTTIME) {
        // then save initial TimeStamp value
        OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: pcmprocessings wrapper: run_inplace BUFFERFLAG_STARTTIME");
        TSinit(&mComputedTimeStamp, pInputBuf->nTimeStamph, pInputBuf->nTimeStampl);
        mTimeStampUpdateFlag = true;
    }

    if (pInputBuf->flags & BUFFERFLAG_EOS){
        eosReached = 1;
    }

    // pcmdump!
    DO_PROBE(IN, pInputBuf);

    effect.processBuffer(
        pInputBuf->data, pInputBuf->data, (t_uint16)pInputBuf->filledLen);

    pInputBuf->filledLen = (pInputBuf->filledLen / mEffectConfig.infmt.nof_channels) * mEffectConfig.outfmt.nof_channels;

    // compute TimeStamp value (and update sample counter for next TimeStamp computation)
    if (mTimeStampUpdateFlag) {
        TScomputation(&mComputedTimeStamp, pInputBuf,  pInputBuf->filledLen, mEffectConfig.infmt.freq, mEffectConfig.outfmt.nof_channels);
    }

    // pcmdump!
    DO_PROBE(OUT, pInputBuf);

    Port_returnBuffer(&this->ports[OUT],pInputBuf);

    if (eosReached){
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_MPC: pcmprocessings wrapper: run_inplace EventBufferFlag %x",pInputBuf->flags);
        proxy.eventHandler(OMX_EventBufferFlag, 1, pInputBuf->flags);
        effect.reset();
    }
}


static void
run_not_inplace(Component *this) {
    Buffer_p pInputBuf = 0;
    Buffer_p pOutputBuf = 0;

    if ( !Port_queuedBufferCount(&this->ports[IN]) ||
            !Port_queuedBufferCount(&this->ports[OUT])) {
        return;
    }

    pInputBuf   = Port_dequeueBuffer(&this->ports[IN]);
    pOutputBuf  = Port_dequeueBuffer(&this->ports[OUT]);

    if ( pInputBuf->flags & BUFFERFLAG_STARTTIME) {
        OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: pcmprocessings wrapper: run_not_inplace BUFFERFLAG_STARTTIME");
        // then save initial TimeStamp value
        TSinit(&mComputedTimeStamp, pInputBuf->nTimeStamph, pInputBuf->nTimeStampl);
        mTimeStampUpdateFlag = true;
    }

    DO_PROBE(IN, pInputBuf);

    effect.processBuffer(pInputBuf->data, pOutputBuf->data, (t_uint16)pInputBuf->filledLen);

    pOutputBuf->filledLen   = (pInputBuf->filledLen / mEffectConfig.infmt.nof_channels) * mEffectConfig.outfmt.nof_channels;
    pOutputBuf->byteInLastWord  = pInputBuf->byteInLastWord;
    pOutputBuf->flags           = pInputBuf->flags;

    DO_PROBE(OUT, pOutputBuf);

    // compute TimeStamp value (and update sampel counter for next TimeStamp computation)
    if (mTimeStampUpdateFlag) {
        TScomputation(&mComputedTimeStamp, pOutputBuf, pOutputBuf->filledLen, mEffectConfig.infmt.freq, mEffectConfig.outfmt.nof_channels);
    }

    Port_returnBuffer(&this->ports[OUT], pOutputBuf);

    if (pInputBuf->flags & BUFFERFLAG_EOS){
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_MPC: pcmprocessings wrapper: run_not_inplace EventBufferFlag %x",pInputBuf->flags);
        proxy.eventHandler(OMX_EventBufferFlag, 1, pInputBuf->flags);
        effect.reset();
    }

    Port_returnBuffer(&this->ports[IN], pInputBuf);
}

static void
reset(Component *this){
    effect.reset();

    TSreset(&mComputedTimeStamp);
    mTimeStampUpdateFlag = false;
}

static void
disablePortIndication(t_uint32 portIdx) {
    if(portIdx == IN) {
        effect.reset();
    }
}

static void
enablePortIndication(t_uint32 portIdx) {
    if(portIdx ==OUT)
    {
        if (mSendNewFormat){
            outputsettings.newFormat(mEffectConfig.infmt.freq, mEffectConfig.outfmt.nof_channels, mEffectConfig.infmt.nof_bits_per_sample);
            mSendNewFormat = 0;
        }
    }
}

static void
flushPortIndication(t_uint32 portIdx) {
    if(portIdx == IN) {
        effect.reset();
    }
}

////////////////////////////////////////////////////////////
// Provided Interfaces
////////////////////////////////////////////////////////////

void METH(setParameter)(t_pcmprocessing_config config)
{
    // DSP framework does not support effects doing SRC
    PRECONDITION(config.effectConfig.infmt.freq == config.effectConfig.outfmt.freq);
    // dont see why an effect would use different sample size on input and output
    PRECONDITION(config.effectConfig.infmt.nof_bits_per_sample == config.effectConfig.outfmt.nof_bits_per_sample);

    mProcessingMode     = config.processingMode;
    mEffectConfig       = config.effectConfig;

    effect.open();

    effect.newFormat(
            &config.effectConfig.infmt.freq,
            &config.effectConfig.infmt.nof_channels,
            &config.effectConfig.infmt.nof_bits_per_sample);

    POSTCONDITION(config.effectConfig.infmt.freq == mEffectConfig.outfmt.freq);
    // WRONG POSTCONDITION ! some effects could do 2->6, or actually do 2->1, like volctrl in mixers
    // POSTCONDITION(config.effectConfig.infmt.nof_channels == mEffectConfig.outfmt.nof_channels);
    POSTCONDITION(config.effectConfig.infmt.nof_bits_per_sample == mEffectConfig.outfmt.nof_bits_per_sample);
}

void METH(fsmInit)(fsmInit_t initFsm) {

    // trace init (mandatory before trace init)
    FSM_traceInit(&mWrapper, initFsm.traceInfoAddr, initFsm.id1);

    switch (mProcessingMode) {
    case PCMPROCESSING_MODE_INPLACE_PUSH:
        mWrapper.process = run_inplace;
        Port_init(&mPorts[IN], InputPort, false, false, &mPorts[OUT], &mFifoIn, 1, &inputport, IN,
                  (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mWrapper);
        Port_init(&mPorts[OUT], OutputPort, true, false, &mPorts[IN], &mFifoOut, 1, &outputport, OUT,
                  (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mWrapper);
        break;

    case PCMPROCESSING_MODE_INPLACE_PULL:
        mWrapper.process = run_inplace;
        Port_init(&mPorts[IN], InputPort, true, false, &mPorts[OUT], &mFifoIn, 1, &inputport, IN,
                  (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mWrapper);
        Port_init(&mPorts[OUT], OutputPort, false, false, &mPorts[IN], &mFifoOut, 1, &outputport, OUT,
                  (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mWrapper);
        break;

    case PCMPROCESSING_MODE_NOT_INPLACE:
        mWrapper.process = run_not_inplace;
        Port_init(&mPorts[IN], InputPort, false, false, 0, &mFifoIn, 1, &inputport, IN,
                  (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mWrapper);
        Port_init(&mPorts[OUT], OutputPort, false, false, 0, &mFifoOut, 1, &outputport, OUT,
                  (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mWrapper);
        break;

    default:
        ASSERT(0);
        break;
    }

    mWrapper.reset                  = reset;
    mWrapper.disablePortIndication  = disablePortIndication;
    mWrapper.enablePortIndication   = enablePortIndication;
    mWrapper.flushPortIndication    = flushPortIndication;

    Component_init(&mWrapper, NB_PORTS, mPorts, &proxy);

}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(processEvent)() {
    Component_processEvent(&mWrapper);
}

void METH(emptyThisBuffer)(Buffer_p buf) {
    Component_deliverBufferCheck(&mWrapper, 0, buf);
}

void METH(fillThisBuffer)(Buffer_p buf) {
    Component_deliverBufferCheck(&mWrapper, 1, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    if (cmd == OMX_CommandStateSet && ((OMX_STATETYPE)param) == OMX_StateExecuting){
        if (mSendNewFormat){
            if(Port_isEnabled(&mWrapper.ports[OUT])) {
                outputsettings.newFormat(mEffectConfig.infmt.freq, mEffectConfig.outfmt.nof_channels, mEffectConfig.infmt.nof_bits_per_sample);
                mSendNewFormat = 0;
            }
        }
    }
    Component_sendCommand(&mWrapper, cmd, param);
}

void METH(eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
    proxy.eventHandler(event, data1, data2);
}

void METH(configure_pcmprobe)(PcmDumpConfigure_t sConfigure)
{
    t_sint24 tab_idx;
    t_sint24 write_idx, offset;
    TRACE_t * this = (TRACE_t*)&mWrapper;
    ASSERT(sConfigure.nmf_port_idx < NB_PORTS);

    OstTraceFiltInst3(TRACE_ALWAYS, "AFM_MPC: pcmprocessings wrapper: configure_pcmprobe: nmf port idx: %d, enable: %d, omx port idx: %d", sConfigure.nmf_port_idx, sConfigure.enable, sConfigure.omx_port_idx);
    OstTraceFiltInst2(TRACE_ALWAYS, "AFM_MPC: pcmprocessings wrapper: configure_pcmprobe: buffer: 0x%x, buffer size: %d", (t_uint24)sConfigure.buffer, sConfigure.buffer_size);

    pcmdump.configure(sConfigure, this, &complete, &mPcmDumpConfig[sConfigure.nmf_port_idx]);
}
