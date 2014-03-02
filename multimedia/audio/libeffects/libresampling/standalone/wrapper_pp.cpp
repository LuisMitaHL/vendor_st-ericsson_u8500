/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   wrapper.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <hst/pcmprocessings/wrapper.nmf>
#include <string.h>
#include <verbose.h>
#include <armnmf_dbc.h>

//#define PCMPROC_TRACE_CONTROL
#ifdef PCMPROC_TRACE_CONTROL
#include <stdio.h>
#endif

hst_pcmprocessings_wrapper::hst_pcmprocessings_wrapper(): mResetCount(0) 
{
    mConfig.mode = PCMPROCESSING_MODE_COUNT;
    mConfig.block_size = 0;

    m_effect_config.block_size = 0;

    m_effect_config.infmt.freq = FREQ_UNKNOWN;
    m_effect_config.infmt.nof_channels = 0;
    m_effect_config.infmt.nof_bits_per_sample = 0;
    m_effect_config.infmt.headroom = 0;
    m_effect_config.infmt.interleaved = true;

    m_effect_config.outfmt.freq = FREQ_UNKNOWN;
    m_effect_config.outfmt.nof_channels = 0;
    m_effect_config.outfmt.nof_bits_per_sample = 0;
    m_effect_config.outfmt.headroom = 0;
    m_effect_config.outfmt.interleaved = true;

    m_effect_caps.proc_type = EFFECT_PROCESS_TYPE_COUNT;
}

hst_pcmprocessings_wrapper::~hst_pcmprocessings_wrapper() 
{
}

t_nmf_error METH(construct)(void) {
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper::construct\n");
#endif
    useLocalOutputBuffer = false;
    useLocalInputBuffer  = false;
    
    return NMF_OK;
}

void METH(destroy)(void) {
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper::destroy\n");
#endif
}

void METH(start)(void) {
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper::start\n");
#endif
}

void METH(stop)(void) {
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper::stop\n");
#endif
    // Close the effect...
    effect.close();
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize) {
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper::newFormat\n");
    printf("\tfreq=%u\n", (unsigned int)freq);
    printf("\tchannels=%u\n", (unsigned int)chans_nb);
    printf("\tbits/sample=%u\n", (unsigned int)sample_bitsize);
#endif
    // Store input format...
    m_effect_config.infmt.freq = freq;
    m_effect_config.infmt.nof_channels = chans_nb;
    m_effect_config.infmt.nof_bits_per_sample = sample_bitsize;

    // Calculate output format...
    // Note: assumes that setParameter has been called before this method
    if (m_effect_config.outfmt.freq == FREQ_UNKNOWN) {
        // If out freq undefined then forward in freq...
        m_effect_config.outfmt.freq = m_effect_config.infmt.freq;
    }
    if (m_effect_config.outfmt.nof_channels == 0) {
        // If out #channels undefined then forward in #channels...
        m_effect_config.outfmt.nof_channels = m_effect_config.infmt.nof_channels;
    }
    if (m_effect_config.outfmt.nof_bits_per_sample == 0) {
        // If out #bits/sample undefined then forward in #bits/sample...
        m_effect_config.outfmt.nof_bits_per_sample = m_effect_config.infmt.nof_bits_per_sample;
    }

    // Forward new settings to out-port...
    // TODO: Check if interface bound, it is optional!
    outputsettings.newFormat(m_effect_config.outfmt.freq, m_effect_config.outfmt.nof_channels, m_effect_config.outfmt.nof_bits_per_sample);
}

void METH(run_inplace)() {
    int i;
    OMX_U32 flags;
    OMX_BUFFERHEADERTYPE * pInputBuf; 
    OMX_BUFFERHEADERTYPE * pOutputBuf;

    ARMNMF_DBC_PRECONDITION(m_effect_caps.proc_type == EFFECT_PROCESS_TYPE_INPLACE);

    // In- and out-ports share buffers. Return all buffers rxed on out-port to in-port.
    int outPortBufferCount = mPorts[OUTPUT_PORT_IDX].queuedBufferCount();

    for (i = 0; i < outPortBufferCount; i++) {
        // For each out-port buffer...
        // De-queue from out-port and return to in-port...
        pOutputBuf = mPorts[OUTPUT_PORT_IDX].dequeueBuffer();
        mPorts[INPUT_PORT_IDX].returnBuffer(pOutputBuf);
    }

    // If no pending in-port buffers then return...
    if (mPorts[INPUT_PORT_IDX].queuedBufferCount() == 0){
        return;
    }

    // De-queue buffer to process from in-port...    
    pInputBuf = mPorts[INPUT_PORT_IDX].dequeueBuffer();
    
    // Process the buffer in-place...    
    t_effect_process_inplace_params params;
    params.base.size      = sizeof(t_effect_process_inplace_params);
    params.base.proc_type = m_effect_caps.proc_type;
    params.buf_hdr        = pInputBuf;
    effect.process((t_effect_process_params*)&params);
    
    // Remember flags...    
    flags = pInputBuf->nFlags;

    // Return in-buffer on out-port (remember it is shared)...
    mPorts[OUTPUT_PORT_IDX].returnBuffer(pInputBuf);

    // Fire buffer flag event if EOS...
    if (flags & OMX_BUFFERFLAG_EOS) {
        proxy.eventHandler(OMX_EventBufferFlag, 1, flags);
        effect.reset(EFFECT_RESET_REASON_EOS);
    }
}

void METH(run_not_inplace)() {
    OMX_BUFFERHEADERTYPE * pInputBuf; 
    OMX_BUFFERHEADERTYPE * pOutputBuf;
    
    unsigned int i;
    
    ARMNMF_DBC_PRECONDITION((m_effect_caps.proc_type == EFFECT_PROCESS_TYPE_INPLACE) || (m_effect_caps.proc_type == EFFECT_PROCESS_TYPE_NOT_INPLACE));

    // If not buffers on both ports then return...    
    if( !mPorts[INPUT_PORT_IDX].queuedBufferCount() || !mPorts[OUTPUT_PORT_IDX].queuedBufferCount()) {
        return;
    }

    // Point to in- and out-buffers (do not de-queue until processed)...
    pInputBuf  = mPorts[INPUT_PORT_IDX].getBuffer(0);
    pOutputBuf = mPorts[OUTPUT_PORT_IDX].getBuffer(0);

    bool inBufEmptied = false;
    bool outBufFilled = false;

    //Check that output buffer contains enough space
    ARMNMF_DBC_ASSERT(pInputBuf->nFilledLen <= pOutputBuf->nAllocLen);

    // Process...
    if (m_effect_caps.proc_type == EFFECT_PROCESS_TYPE_INPLACE) {
        // If effect has implemented in-place processing...
        // Copy in- to out-buffer...
        ARMNMF_DBC_ASSERT(pInputBuf->nOffset == 0);
        memcpy(pOutputBuf->pBuffer, pInputBuf->pBuffer, pInputBuf->nFilledLen);
        // Forward applicable parts of buffer header...
        pOutputBuf->nFilledLen = pInputBuf->nFilledLen;
        pOutputBuf->nOffset    = pInputBuf->nOffset;
        pOutputBuf->nTimeStamp = pInputBuf->nTimeStamp;
        pOutputBuf->nFlags     = pInputBuf->nFlags;
        // Process out-buffer in-place...
        t_effect_process_inplace_params params;
        params.base.size      = sizeof(t_effect_process_inplace_params);
        params.base.proc_type = m_effect_caps.proc_type;
        params.buf_hdr        = pOutputBuf;
        effect.process((t_effect_process_params*)&params);

        // Buffers always completely processed in case of in-place processing...
        inBufEmptied = true;
        outBufFilled = true;
    }
    else {
        // Else effect has implemented not in-place processing, ask it to process...
        t_effect_process_not_inplace_params params;
        params.base.size      = sizeof(t_effect_process_not_inplace_params);
        params.base.proc_type = m_effect_caps.proc_type;
        params.inbuf_hdr      = pInputBuf;
        params.inbuf_emptied  = false;
        params.outbuf_hdr     = pOutputBuf;
        params.outbuf_filled  = false;
        // Call process function
        effect.process((t_effect_process_params*)&params);
        // Check if buffers are processed...
        inBufEmptied = params.inbuf_emptied;
        outBufFilled = params.outbuf_filled;
    }

    OMX_U32 outFlags = pOutputBuf->nFlags;
    bool outEos      = (outFlags & OMX_BUFFERFLAG_EOS);

    // outBufFilled indicates that out buffer is filled and shall be returned on output port
    //   Effect MAY update pOutputBuf->nFilledLen incrementally
    // outEos indicates that out buffer is LAST. It implies that the buffer is filled!
    // outBufFilled=0 AND outEos=0 : the buffer is not filled and is kept on output port
    // outBufFilled=1 AND outEos=0 : the buffer is filled and is returned on output port
    // outBufFilled=X AND outEos=1 : the buffer is filled AND last and is returned on output port

    if (outBufFilled || outEos) {
#ifdef PCMPROC_TRACE_CONTROL
        printf("hst_pcmprocessings_wrapper::return OUTPUT_PORT_IDX \n");
#endif
        mPorts[OUTPUT_PORT_IDX].dequeueAndReturnBuffer();
    }

    // inBufEmptied indicates that in buf is emptied and shall be returned on input port
    //   Effect MUST NOT update pInputBuf->nFilledLen
    // inEos indicates that in buffer is LAST.
    // The last in buffer may produce several out buffers (e.g. because of internal effect delay) =>
    // If inEos then effect shall set inBufEmptied only when last out buffer is filled
    // ((Keep last in buffer while outBufHdr.eos=0))
    // inBufEmptied=0 AND inEos=X : the buffer is not emptied and is kept on input port
    // inBufEmptied=1 AND inEos=X : the buffer is emptied and is returned on input port
    
    if (inBufEmptied) {
#ifdef PCMPROC_TRACE_CONTROL
        printf("hst_pcmprocessings_wrapper::return INPUT_PORT_IDX \n");
#endif
        mPorts[INPUT_PORT_IDX].dequeueAndReturnBuffer();
    }

    // Fire buffer flag event if EOS...
    if (outEos) {   
#ifdef PCMPROC_TRACE_CONTROL
        printf("hst_pcmprocessings_wrapper:: EOS sent to proxy \n");
#endif 
        proxy.eventHandler(OMX_EventBufferFlag, 1, outFlags);
        effect.reset(EFFECT_RESET_REASON_EOS);
    }
}

void METH(fsmInit)(fsmInit_t initFsm)
{
    t_uint32 bitSizeIn=0,bitSizeOut=0;
    
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper::fsmInit \n");
#endif
    // trace init (mandatory before port init)
    if (initFsm.traceInfoAddr){
        setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    }

    // Init ports depending on inplace mode...
    switch (mConfig.mode) {
        case PCMPROCESSING_MODE_INPLACE_PUSH:
            // in this case out-port is buffer supplier, triggered by ETB on in-port
            mPorts[INPUT_PORT_IDX].init(InputPort, false, false, &mPorts[OUTPUT_PORT_IDX], 0, 1, &inputport, INPUT_PORT_IDX, false, true, this);
            mPorts[OUTPUT_PORT_IDX].init(OutputPort, true, false, &mPorts[INPUT_PORT_IDX], 0, 1, &outputport, OUTPUT_PORT_IDX, false, true, this);
            break;
            
        case PCMPROCESSING_MODE_INPLACE_PULL:
            // in this case in-port is buffer supplier, triggered by FTB on out-port
            mPorts[INPUT_PORT_IDX].init(InputPort, true, false, &mPorts[OUTPUT_PORT_IDX], 0, 1, &inputport, INPUT_PORT_IDX, false, true, this);
            mPorts[OUTPUT_PORT_IDX].init(OutputPort, false, false, &mPorts[INPUT_PORT_IDX], 0, 1, &outputport, OUTPUT_PORT_IDX, false, true, this);
            break;

        case PCMPROCESSING_MODE_NOT_INPLACE:
            mPorts[INPUT_PORT_IDX].init(InputPort, false, false, 0, 0, 1, &inputport, INPUT_PORT_IDX, false, true, this);
            mPorts[OUTPUT_PORT_IDX].init(OutputPort, false, false, 0, 0, 1, &outputport, OUTPUT_PORT_IDX, false, true, this);
            break;

        default:
            ARMNMF_DBC_ASSERT(0);
            break;
    }

    // Init this component...
    init(2, mPorts, &proxy, &me, false);

    // Get information about size of sample In & Out
    effect.getSampleSize(&bitSizeIn,&bitSizeOut);
#ifdef PCMPROC_TRACE_CONTROL
        printf("hst_pcmprocessings_wrapper::fsmInit Algo needs %d bits in input\n",bitSizeIn);
        printf("hst_pcmprocessings_wrapper::fsmInit Algo needs %d bits in output\n",bitSizeOut);
#endif

    // Open effect...
    bool result = effect.open(&m_effect_config, &m_effect_caps);
    // TODO: Investigate what to do if open returns with failure!
    // fsmInit should have return value to block transition to Idle in case of failure!? 
    ARMNMF_DBC_ASSERT(result);
    ARMNMF_DBC_ASSERT(m_effect_caps.proc_type != EFFECT_PROCESS_TYPE_COUNT);
}

void METH(setTunnelStatus) (t_uint16 portIdx, t_uint16 isTunneled) {
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper:: setTunnelStatus\n");
    printf("\tportIdx=%u\n",portIdx);
    printf("\tisTunneled=%u\n",isTunneled);
#endif
    mPorts[portIdx].setTunnelStatus(isTunneled);
}

void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)  
{ 
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper::emptyThisBuffer \n");
    printf("\tbuffer->nAllocLen=%u\n",(unsigned int)buffer->nAllocLen);
    printf("\tbuffer->nFilledLen=%u\n",(unsigned int)buffer->nFilledLen);
    printf("\tbuffer->nFlags=%u\n",(unsigned int)buffer->nFlags);
#endif
    Component::deliverBuffer(INPUT_PORT_IDX, buffer); 
} 

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)   
{
    OMX_BUFFERHEADERTYPE* omxBuffer = (OMX_BUFFERHEADERTYPE*)buffer;
    omxBuffer->nFilledLen = 0;
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper::fillThisBuffer \n");
    printf("\tbuffer->nAllocLen=%u\n",(unsigned int)buffer->nAllocLen);
    printf("\tbuffer->nFilledLen=%u\n",(unsigned int)buffer->nFilledLen);
    printf("\tbuffer->nFlags=%u\n",(unsigned int)buffer->nFlags);
#endif
    Component::deliverBuffer(OUTPUT_PORT_IDX, buffer); 
}

// Interface for fsm
// reset is called when entering Idle state, inside Component::init called from fsmInit
void METH(reset)()
{
    if (mResetCount++ > 0) {
        // If NOT first reset (Init -> Idle transition) then reset effect...
        effect.reset(EFFECT_RESET_REASON_STOP);
    }
}

void METH(disablePortIndication)(t_uint32 portIdx)
{
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper:: disablePortIndication\n");
    printf("\tportIdx=%u\n",(unsigned int)portIdx);
#endif
    // TODO: Implement
    ARMNMF_DBC_ASSERT(0);
}

void METH(enablePortIndication)(t_uint32 portIdx)
{
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper:: enablePortIndication\n");
    printf("\tportIdx=%u\n",(unsigned int)portIdx);
#endif
    // TODO: Implement
    ARMNMF_DBC_ASSERT(0);
}

void METH(flushPortIndication)(t_uint32 portIdx)
{
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper:: flushPortIndication\n");
    printf("\tportIdx=%u\n",(unsigned int)portIdx);
#endif
    effect.reset(EFFECT_RESET_REASON_FLUSH);
    // TODO: Check if any pending buffers, and that in case return them
}

void METH(process)()
{
    switch (mConfig.mode) {
        case PCMPROCESSING_MODE_INPLACE_PUSH:
        case PCMPROCESSING_MODE_INPLACE_PULL:
#ifdef PCMPROC_TRACE_CONTROL
            printf("hst_pcmprocessings_wrapper::process (INPLACE)\n");
#endif
            run_inplace();
            break;
        case PCMPROCESSING_MODE_NOT_INPLACE:
#ifdef PCMPROC_TRACE_CONTROL
            printf("hst_pcmprocessings_wrapper::process (INPLACE_NOT_INPLACE)\n");
#endif
            run_not_inplace();
            break;
        default:
            ARMNMF_DBC_ASSERT(0);
            break;
    }
#ifdef PCMPROC_TRACE_CONTROL
            printf("hst_pcmprocessings_wrapper::process (EXIT)\n");
#endif
}

void METH(setParameter)(t_pcmprocessing_config config)
{
#ifdef PCMPROC_TRACE_CONTROL
    printf("hst_pcmprocessings_wrapper::setParameter\n");
    printf("\tmode=%u\n", (unsigned int)config.mode);
    printf("\tblock_size=%u\n", (unsigned int)config.block_size);
    printf("\tout_freq=%u\n", (unsigned int)config.out_freq);
    printf("\tout_nof_channels=%u\n", (unsigned int)config.out_nof_channels);
    printf("\tout_nof_bits_per_sample=%u\n", (unsigned int)config.out_nof_bits_per_sample);
#endif
    ARMNMF_DBC_PRECONDITION(config.block_size > 0);
    ARMNMF_DBC_PRECONDITION(m_effect_config.outfmt.freq == FREQ_UNKNOWN);
    ARMNMF_DBC_PRECONDITION(m_effect_config.outfmt.nof_channels == 0);
    ARMNMF_DBC_PRECONDITION(m_effect_config.outfmt.nof_bits_per_sample == 0);

    // Store PCM proc config for later use...
    mConfig = config;

    // Update effect config to use when opening the effect...
    m_effect_config.block_size                 = mConfig.block_size;
    m_effect_config.outfmt.freq                = mConfig.out_freq;
    m_effect_config.outfmt.nof_channels        = mConfig.out_nof_channels;
    m_effect_config.outfmt.nof_bits_per_sample = mConfig.out_nof_bits_per_sample;
}

