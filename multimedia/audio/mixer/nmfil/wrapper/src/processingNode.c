/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   processingNode.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <mixer/nmfil/wrapper.nmf>
#include <processingNode.h>
#include <dbc.h>
#include <buffer.idt>

#ifndef _SIMU_

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mixer_nmfil_wrapper_src_processingNodeTraces.h"
#endif

#else // _SIMU_
#include "libeffects/mpc/include/audiolibs_trace.h"
#endif // _SIMU_

//pcmdump config
static PcmDump_t    mPcmDumpConfig[MIXER_INPUT_PORT_COUNT+1];
#ifndef _SIMU_
#define DO_PROBE(buf, pPcmDump) pcmdump.probe((void*) buf->data, (t_uint24) buf->filledLen, pPcmDump)
#else // _SIMU_
#define DO_PROBE(port_idx, buf)
#endif //_SIMU_

static const int    mFreq[FREQ_LAST_IN_LIST] = {0, 192000, 176400, 128000, 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7200};

static void fillBufferWithZeroes(Buffer_p buf)
{
    int i;

    for (i = buf->filledLen; i < buf->allocLen; i++) {
        buf->data[i] = 0;
    }
}

//---------------------------------------------------------------------------------

PcmDump_t * getOMXPortPcmDumpStruct(t_uint24 omx_port_idx) {
    return &mPcmDumpConfig[omx_port_idx];
}

void ProcessingNode_configureProbe(PcmDumpConfigure_t sConfigure, TRACE_t *pTraceObject, PcmDump_t *pPcmDump) {
#ifndef _SIMU_
	// configure the pcmdump singleton
    pcmdump.configure(sConfigure, pTraceObject, &complete, pPcmDump);
#endif
}

//---------------------------------------------------------------------------------

static t_ErrorNodeId InplaceProcessingNode_getFilledBuffer(PcmProcessingNode_t * pPcmProcessingNode, Buffer_p *buf) {
    TRACE_t * this = pPcmProcessingNode->sProcessingNode.pTraceObject;
    ProcessingNode_t *pUpstreamNode = pPcmProcessingNode->upstreamNode;
    bool needInputBuf = false, filledOutputBuf = false;
    t_EffectError error = ERROR_NODE_NONE;

    error = pUpstreamNode->getFilledBuffer(pUpstreamNode, buf);
    if(error != ERROR_NODE_NONE || *buf == NULL) {
        return error;
    }

    OstTraceFiltInst2(TRACE_DEBUG,"MixerMpc::pNode=0x%x process Inplace buf=0x%x", (unsigned int)pPcmProcessingNode, (unsigned int)*buf);

    error = pPcmProcessingNode->pEffectDescription->effect_processBuffer(pPcmProcessingNode->pEffectCtx, *buf, *buf, &needInputBuf, &filledOutputBuf); //process Inplace
    ASSERT(error == EFFECT_ERROR_NONE);

    // pcmdump!!!
    OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::InplaceProcessingNode_getFilledBuffer pcm probe pNode=0x%x", (unsigned int)pPcmProcessingNode);
    DO_PROBE((*buf), &pPcmProcessingNode->sPcmDumpConfig);

    return ERROR_NODE_NONE;
}

static void InplaceProcessingNode_releaseBuffer(PcmProcessingNode_t * pPcmProcessingNode, Buffer_p buf) {
    ProcessingNode_t *pUpstreamNode = pPcmProcessingNode->upstreamNode;
    pUpstreamNode->releaseBuffer(pUpstreamNode, buf);
}

//---------------------------------------------------------------------------------

static t_ErrorNodeId NotInplaceProcessingNode_getFilledBuffer(PcmProcessingNode_t * pPcmProcessingNode, Buffer_p *buf) {
    TRACE_t * this = pPcmProcessingNode->sProcessingNode.pTraceObject;
    ProcessingNode_t *pUpstreamNode = pPcmProcessingNode->upstreamNode;
    t_EffectError error = EFFECT_ERROR_NONE;
    bool needInputBuf = false, filledOutputBuf = false;

    if(pPcmProcessingNode->inputbuf == NULL) {
        error = pUpstreamNode->getFilledBuffer(pUpstreamNode, &pPcmProcessingNode->inputbuf);
        if(error != ERROR_NODE_NONE || pPcmProcessingNode->inputbuf == NULL) {
            *buf = NULL;
            return error;
        }
    }

    if(pPcmProcessingNode->outputbuf == NULL) {
        pPcmProcessingNode->outputbuf = pPcmProcessingNode->pEffectDescription->effect_getOutputBuffer(pPcmProcessingNode->pEffectCtx); // TODO: to be replaced by a buffer pool shared by all the effects
    }

    OstTraceFiltInst3(TRACE_DEBUG,"MixerMpc::pNode=0x%x start process Not Inplace (inputbuf=0x%x outputbuf=0x%x)", (unsigned int)pPcmProcessingNode, (unsigned int)pPcmProcessingNode->inputbuf, (unsigned int)pPcmProcessingNode->outputbuf);

    error = pPcmProcessingNode->pEffectDescription->effect_processBuffer(pPcmProcessingNode->pEffectCtx, pPcmProcessingNode->inputbuf, pPcmProcessingNode->outputbuf, &needInputBuf, &filledOutputBuf); //process NOT Inplace
    ASSERT(error == EFFECT_ERROR_NONE);

    OstTraceFiltInst3(TRACE_DEBUG,"MixerMpc::pNode=0x%x end process Not Inplace needInputBuf=%d filledOutputBuf=%d", (unsigned int)pPcmProcessingNode, needInputBuf, filledOutputBuf);

    if(needInputBuf) {
        pUpstreamNode->releaseBuffer(pUpstreamNode, pPcmProcessingNode->inputbuf);
        pPcmProcessingNode->inputbuf = NULL;
    }

    if(filledOutputBuf) {

        // pcmdump!!!
        OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::NotInplaceProcessingNode_getFilledBuffer pcm probe pNode=0x%x", (unsigned int)pPcmProcessingNode);
        DO_PROBE(pPcmProcessingNode->outputbuf, &pPcmProcessingNode->sPcmDumpConfig);

        *buf = pPcmProcessingNode->outputbuf;
        return ERROR_NODE_NONE;
    } else {
        *buf = NULL;
        return ERROR_NODE_NOT_READY;
    }
}

static void NotInplaceProcessingNode_releaseBuffer(PcmProcessingNode_t * pPcmProcessingNode, Buffer_p buf) {
    TRACE_t * this = pPcmProcessingNode->sProcessingNode.pTraceObject;

    OstTraceFiltInst2(TRACE_DEBUG,"MixerMpc::pNode=0x%x release internal buffer (0x%x)", (unsigned int)pPcmProcessingNode, (unsigned int)buf);

    ASSERT(buf == pPcmProcessingNode->pEffectDescription->effect_getOutputBuffer(pPcmProcessingNode->pEffectCtx));
    pPcmProcessingNode->outputbuf = NULL;
}

//---------------------------------------------------------------------------------

PcmProcessingNode_t * PcmProcessingNode_construct(EffectInit_t *pEffectInitParams, t_uint24 nPosition, const char* sEffectName) {
	TRACE_t * this = pEffectInitParams->pTraceObject;
    EffectDescription_t * pEffectDescription = NULL;
    PcmProcessingNode_t * pPcmProcessingNode = NULL;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = pEffectInitParams->nMemoryBank;
    pAllocParams.trace_p = this;

    pEffectDescription = createEffect(sEffectName);

    pPcmProcessingNode = (PcmProcessingNode_t *) vmalloc(sizeof(PcmProcessingNode_t), &pAllocParams);

    pPcmProcessingNode->sProcessingNode.pTraceObject    = this;
    pPcmProcessingNode->sProcessingNode.nPosition       = nPosition;
    pPcmProcessingNode->pEffectDescription              = pEffectDescription;

    pPcmProcessingNode->sProcessingNode.getFilledBuffer = InplaceProcessingNode_getFilledBuffer; //default mode is inplace, will be changed at open
    pPcmProcessingNode->sProcessingNode.releaseBuffer   = InplaceProcessingNode_releaseBuffer; //default mode is inplace, will be changed at open
    pPcmProcessingNode->inputbuf                        = NULL;
    pPcmProcessingNode->outputbuf                       = NULL;

    ASSERT(pEffectDescription->effect_construct(pEffectInitParams, &pPcmProcessingNode->pEffectCtx) == EFFECT_ERROR_NONE);

    OstTraceFiltInst3(TRACE_FLOW,"MixerMpc::init PcmProcessingNode (0x%x) EffectCtx=0x%x position=%d", (unsigned int)pPcmProcessingNode, (unsigned int)pPcmProcessingNode->pEffectCtx, nPosition);

    return pPcmProcessingNode;
}

void PcmProcessingNode_open(PcmProcessingNode_t *pPcmProcessingNode, EffectOpen_t* pOpenParameters) {
    TRACE_t * this = pPcmProcessingNode->sProcessingNode.pTraceObject;
    t_EffectProcessMode effectMode = EFFECT_INPLACE;

    ASSERT(pPcmProcessingNode->pEffectDescription->effect_open(pPcmProcessingNode->pEffectCtx, pOpenParameters, &effectMode) == EFFECT_ERROR_NONE);

    if(effectMode == EFFECT_INPLACE) {
        pPcmProcessingNode->sProcessingNode.getFilledBuffer = InplaceProcessingNode_getFilledBuffer;
        pPcmProcessingNode->sProcessingNode.releaseBuffer   = InplaceProcessingNode_releaseBuffer;
        OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::open PcmProcessingNode (0x%x) inplace mode is using", (unsigned int)pPcmProcessingNode);
    } else {
        pPcmProcessingNode->sProcessingNode.getFilledBuffer = NotInplaceProcessingNode_getFilledBuffer;
        pPcmProcessingNode->sProcessingNode.releaseBuffer   = NotInplaceProcessingNode_releaseBuffer;
        OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::open PcmProcessingNode (0x%x) Not inplace mode is using", (unsigned int)pPcmProcessingNode);
    }
}

void PcmProcessingNode_destroy(PcmProcessingNode_t *pPcmProcessingNode) {
    TRACE_t * this = pPcmProcessingNode->sProcessingNode.pTraceObject;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = 0;
    pAllocParams.trace_p = this;

    OstTraceFiltInst2(TRACE_DEBUG,"MixerMpc::close and destroy ProcessingNode (0x%x) at position %d", (unsigned int)pPcmProcessingNode, pPcmProcessingNode->sProcessingNode.nPosition);

    pPcmProcessingNode->pEffectDescription->effect_close(pPcmProcessingNode->pEffectCtx);
    pPcmProcessingNode->pEffectDescription->effect_destroy(pPcmProcessingNode->pEffectCtx);
    vfree(pPcmProcessingNode, &pAllocParams);
}

//---------------------------------------------------------------------------------

static t_ErrorNodeId InputPort_getFilledBuffer(InputPortProcessingNode_t * pPortNode, Buffer_p *buf) {
    TRACE_t * this = pPortNode->sProcessingNode.pTraceObject;
    Buffer_p buftmp = NULL;
    Port *portCtx = (Port *) pPortNode->pPortCtx;

    if(((!Port_queuedBufferCount(portCtx)) && (InputPort_isEos(pPortNode))) || InputPort_isPaused(pPortNode) || (!InputPort_isReady(pPortNode))) {
        //EOS input or port paused or disabled: no need to wait buffer on this port
        *buf = NULL;
        return ERROR_NODE_NONE;
    }

    if(!Port_queuedBufferCount(portCtx)) {
        *buf = NULL;
        OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::no buffer available on InputPortNode(0x%x) : return ERROR_NODE_NOT_READY", (unsigned int)pPortNode);
        return ERROR_NODE_NOT_READY;
    }

    buftmp = Port_getBuffer(portCtx, 0);

    OstTraceFiltInst2(TRACE_DEBUG,"MixerMpc::InputPort_getFilledBuffer get buf (0x%x) on InputPortNode(0x%x)", (unsigned int)(buftmp), (unsigned int)pPortNode);

    ASSERT(buftmp->filledLen == pPortNode->blockSize * InputPort_getNbChannels(pPortNode) ||
                (buftmp->filledLen < pPortNode->blockSize * InputPort_getNbChannels(pPortNode) &&
                 buftmp->flags & BUFFERFLAG_EOS));
    ASSERT(buftmp->allocLen >= pPortNode->blockSize * InputPort_getNbChannels(pPortNode));

    // pcmdump!!!
    OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::InputPort_getFilledBuffer pcm probe portIdx %d", portCtx->portIdx);
    DO_PROBE(buftmp, &mPcmDumpConfig[portCtx->portIdx]);

    if(buftmp->flags & BUFFERFLAG_EOS) {
        OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::InputPort_getFilledBuffer send OMX_EventBufferFlag for portId %d", portCtx->portIdx);
        SET_BITS(pPortNode->properties, EOS_MASK);
        fillBufferWithZeroes(buftmp);
        proxy.eventHandler(OMX_EventBufferFlag, portCtx->portIdx, buftmp->flags);
    } else {
        CLEAR_BITS(pPortNode->properties, EOS_MASK);
    }

    *buf = buftmp;
    return ERROR_NODE_NONE;
}

static void InputPort_releaseBuffer(InputPortProcessingNode_t * pPortNode, Buffer_p buf) {
    TRACE_t * this = pPortNode->sProcessingNode.pTraceObject;
    Port *portCtx = (Port *) pPortNode->pPortCtx;

    ASSERT(buf == (Port_getBuffer(portCtx, 0)));

    OstTraceFiltInst2(TRACE_DEBUG,"MixerMpc::InputPort_releaseBuffer return buf (0x%x) on InputPortNode(0x%x)", (unsigned int)buf, (unsigned int)pPortNode);

    Port_dequeueAndReturnBuffer(portCtx);
}

InputPortProcessingNode_t * InputPort_construct(t_memory_bank nMemoryBank, TRACE_t *pTraceObject, Port *pPort, InputPortParam_t *pInputPortParams) {
    TRACE_t * this = pTraceObject;
    InputPortProcessingNode_t * pPortNode = NULL;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = nMemoryBank;
    pAllocParams.trace_p = this;

    pPortNode = (InputPortProcessingNode_t *) vmalloc(sizeof(InputPortProcessingNode_t), &pAllocParams);
    ASSERT(pPortNode!=NULL);

    //Initialize PortProcessingNode_t fields
    pPortNode->nSamplingRate    = pInputPortParams->nSamplingRate;

    //Set InputPort properties bitmask (isPaused/isEos/isReady/nbCh)
    pPortNode->properties = 0;
    InputPort_setNbChannels(pPortNode, pInputPortParams->nChannels);
    InputPort_setPaused(pPortNode, pInputPortParams->bPaused);
    SET_BITS(pPortNode->properties, EOS_MASK); //port in EOS state at startup. Will be in EOS state till one buffer is received on this port.
    CLEAR_BITS(pPortNode->properties, READY_MASK); //port will be ready after InputPort_open() call.

    pPortNode->blockSize        = pInputPortParams->nBlockSize;
    pPortNode->pPortCtx         = pPort;

    pPortNode->sProcessingNode.pTraceObject     = pTraceObject;
    pPortNode->sProcessingNode.nPosition        = 0;
    pPortNode->sProcessingNode.getFilledBuffer  = InputPort_getFilledBuffer;
    pPortNode->sProcessingNode.releaseBuffer    = InputPort_releaseBuffer;

    OstTraceFiltInst4(TRACE_FLOW,"MixerMpc::init InputPortNode (0x%x) isPaused=%d freq=%d ch=%d", (unsigned int)pPortNode, pInputPortParams->bPaused, pInputPortParams->nSamplingRate, pInputPortParams->nChannels);

    return pPortNode;
}

void InputPort_open(InputPortProcessingNode_t *pNode) {
    TRACE_t * this = pNode->sProcessingNode.pTraceObject;
    SET_BITS(pNode->properties, READY_MASK);
}

void InputPort_destroy(InputPortProcessingNode_t *pNode) {
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = 0;
    pAllocParams.trace_p = pNode->sProcessingNode.pTraceObject;

    CLEAR_BITS(pNode->properties, READY_MASK);
    vfree(pNode, &pAllocParams);
}

//---------------------------------------------------------------------------------

static void MixerProcessingNode_setAlgoDefaultSettings(MixerProcessingNode_t *pMixerNode, t_uint24 blockSize) {
    pMixerNode->sAlgoStruct.alpha_dw  = ALPHA_DW_DEFAULT;
    pMixerNode->sAlgoStruct.alpha_up  = ALPHA_UP_DEFAULT;
    pMixerNode->sAlgoStruct.one_minus_alpha_dw = (0x800000U - ALPHA_DW_DEFAULT);
    pMixerNode->sAlgoStruct.one_minus_alpha_up = (0x800000U - ALPHA_UP_DEFAULT);
    pMixerNode->sAlgoStruct.threshold = THRESHOLD_DEFAULT;

    pMixerNode->sAlgoStruct.coef_poly[0] = COEF_1;
    pMixerNode->sAlgoStruct.coef_poly[1] = (MMlong)COEF_2;
    pMixerNode->sAlgoStruct.coef_poly[2] = COEF_3;
    pMixerNode->sAlgoStruct.coef_poly[3] = COEF_4;

    pMixerNode->sAlgoStruct.env_st[0].abs_x0  = 0;
    pMixerNode->sAlgoStruct.env_st[0].abs_x1  = 0;
    pMixerNode->sAlgoStruct.env_st[0].abs_in  = 0;
    pMixerNode->sAlgoStruct.env_st[0].envelop = 0;

    pMixerNode->sAlgoStruct.env_st[1].abs_x0  = 0;
    pMixerNode->sAlgoStruct.env_st[1].abs_x1  = 0;
    pMixerNode->sAlgoStruct.env_st[1].abs_in  = 0;
    pMixerNode->sAlgoStruct.env_st[1].envelop = 0;
    pMixerNode->sAlgoStruct.data_block_size = blockSize;
}

static inline void MixerProcessingNode_doMixing(MixerProcessingNode_t *pMixerNode, MMshort *pIn_list, Buffer_p pBuffer, t_uint24 nbInput) {
    TRACE_t * this = (TRACE_t *)pMixerNode->sProcessingNode.pTraceObject;
    int i;

    OstTraceFiltInst2(TRACE_FLOW,"MixerMpc::MixerProcessingNode (0x%x) is mixing %d inputs", (unsigned int)pMixerNode, nbInput);

    if (nbInput == 1) {
        //do nothing : Mixer is working in inplace mode!
        return;
    }
    else if (pMixerNode->nbChannels == 2) {
        mixer_stereo(pIn_list, nbInput, (MMshort *)pBuffer->data, (int)pBuffer->filledLen, &pMixerNode->sAlgoStruct);
    }
    else {
        mixer_mono(pIn_list, nbInput, (MMshort *)pBuffer->data, (int)pBuffer->filledLen, 1, &pMixerNode->sAlgoStruct);
    }
}

static inline void samplesPlayedComp(
        t_uint48 * SPValue,
        t_uint48 *NbSamples,
        t_uint24 SampleFreq,
        t_uint24 NbChannel,
        t_uint48 BufferSampleSize)
{
    signed long time;

    if(SPValue != NULL) {
        *NbSamples += BufferSampleSize;

        if (mFreq[SampleFreq] && NbChannel) {
            *SPValue = (*NbSamples * 10000)/(mFreq[SampleFreq] * NbChannel) * 100;
        } else {
            *SPValue = 0;
        }
    }
}

static inline void MixerProcessingNode_releaseInputBuffer(MixerProcessingNode_t *pMixerNode, t_uint24 portIdx)
{
    TRACE_t * this = (TRACE_t *)pMixerNode->sProcessingNode.pTraceObject;
    ProcessingNode_t *pUpstreamNode = pMixerNode->upstreamNode[portIdx-IN];

    OstTraceFiltInst4(TRACE_DEBUG,"MixerMpc::MixerProcessingNode release Buffer (0x%x) on portId %d on upstreamNode 0x%x flags=0x%x", (unsigned int)pMixerNode->inputbuf[portIdx-IN], portIdx, (unsigned int)pUpstreamNode, pMixerNode->inputbuf[portIdx-IN]->flags);

    // Samples Played input port update
    samplesPlayedComp(&(pMixerNode->pSamplesPlayedPtr->port[portIdx]), &pMixerNode->sampleCounter[portIdx], pMixerNode->nSamplingRate, pMixerNode->nbChannels, pMixerNode->inputbuf[portIdx-IN]->filledLen);

    pUpstreamNode->releaseBuffer(pUpstreamNode, pMixerNode->inputbuf[portIdx-IN]);
    pMixerNode->inputbuf[portIdx-IN] = NULL;
}

static t_ErrorNodeId MixerProcessingNode_getFilledBuffer(MixerProcessingNode_t * pMixerNode, Buffer_p *buf) {
    TRACE_t * this = (TRACE_t *)pMixerNode->sProcessingNode.pTraceObject;
    MMshort *pIn_list[MIXER_INPUT_PORT_COUNT];
    t_ErrorNodeId error = ERROR_NODE_NONE;
    t_uint24 portIdx = 0;
    Buffer_p pOutputBuf = NULL;
    t_uint24 nbInputWithoutData = 0, nbInputWithData = 0, nbEosInputs = 0;
    bool waitInput = false;
    bool transmitEos = false;

    ASSERT(pMixerNode->inplacePortIdx == 0);

    for (portIdx = 1; portIdx <= MIXER_INPUT_PORT_COUNT ; portIdx++)
    {
        ProcessingNode_t *pUpstreamNode = pMixerNode->upstreamNode[portIdx-IN];

        if(pUpstreamNode == NULL) { //Input Port is disabled
            nbInputWithoutData ++;
            continue;
        }

        if(pMixerNode->inputbuf[portIdx-IN] == NULL) {
            error = pUpstreamNode->getFilledBuffer(pUpstreamNode, &pMixerNode->inputbuf[portIdx-IN]);
            if((pMixerNode->inputbuf[portIdx-IN] == NULL) && (error == ERROR_NODE_NOT_READY)) {
                waitInput = true;
                continue;
            } else if ((pMixerNode->inputbuf[portIdx-IN] == NULL) && (error == ERROR_NODE_NONE)) {
                nbInputWithoutData ++;
                continue;
            } else {
                ASSERT((pMixerNode->inputbuf[portIdx-IN] != NULL) && (error == ERROR_NODE_NONE));
            }
        }

        pIn_list[nbInputWithData] = pMixerNode->inputbuf[portIdx-IN]->data;
        if(pMixerNode->inplacePortIdx == 0) {
            //Mixer is working in inplace mode on one of its port!
            pMixerNode->inplacePortIdx = portIdx;
            pOutputBuf = pMixerNode->inputbuf[portIdx-IN];
        }
        nbInputWithData ++;
    }

    if (waitInput == true) {
        OstTraceFiltInst0(TRACE_FLOW,"MixerMpc::MixerProcessingNode -> one input is missing to mix");
        *buf = NULL;
        pMixerNode->inplacePortIdx = 0;
        return ERROR_NODE_NOT_READY;
    }

    if (nbInputWithoutData == MIXER_INPUT_PORT_COUNT) {
        OstTraceFiltInst0(TRACE_FLOW,"MixerMpc::MixerProcessingNode -> no input to mix");
        *buf = NULL;
        pMixerNode->inplacePortIdx = 0;
        return ERROR_NODE_NOT_READY;
    }

    MixerProcessingNode_doMixing(pMixerNode, pIn_list, pOutputBuf, nbInputWithData);

    for (portIdx = 1; portIdx <= MIXER_INPUT_PORT_COUNT ; portIdx++) {
        if (pMixerNode->inputbuf[portIdx-IN] != NULL) {
            //Check if inputBuf has EOS flag
            if (pMixerNode->inputbuf[portIdx-IN]->flags & BUFFERFLAG_EOS) {
                nbEosInputs ++;
            }

            // propagate Drain ID
            pOutputBuf->flags |= (pMixerNode->inputbuf[portIdx-IN]->flags & BUFFERFLAG_MASK_FOR_DRAIN);
            //Check if inputBuf can be released
            if (pMixerNode->inputbuf[portIdx-IN] != pOutputBuf) {
                MixerProcessingNode_releaseInputBuffer(pMixerNode, portIdx);
            }
        }
    }

    //Check if EOS flag must be propagated
    if((nbEosInputs + nbInputWithoutData) == MIXER_INPUT_PORT_COUNT) {
        OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::MixerProcessingNode transmit OMX_EventBufferFlag for portId %d",OUT);
        pOutputBuf->flags |= BUFFERFLAG_EOS;
    } else if(nbEosInputs != 0) {
        pOutputBuf->flags &= ~BUFFERFLAG_EOS;  //Reset EOS flag in output buffer
    }

    // SamplesPlayed output port update
    samplesPlayedComp(&(pMixerNode->pSamplesPlayedPtr->port[OUT]), &pMixerNode->sampleCounter[OUT], pMixerNode->nSamplingRate, pMixerNode->nbChannels, pOutputBuf->filledLen);

    *buf = pOutputBuf;

    return ERROR_NODE_NONE;
}

static void MixerProcessingNode_releaseBuffer(MixerProcessingNode_t * pMixerNode, Buffer_p buf) {
    TRACE_t * this = (TRACE_t *)pMixerNode->sProcessingNode.pTraceObject;

    ASSERT(pMixerNode->inputbuf[pMixerNode->inplacePortIdx - IN] == buf);

    MixerProcessingNode_releaseInputBuffer(pMixerNode, pMixerNode->inplacePortIdx);
    pMixerNode->inplacePortIdx = 0;
}

MixerProcessingNode_t * MixerProcessingNode_construct(t_memory_bank nMemoryBank, TRACE_t *pTraceObject, t_uint24 nbChannels, t_uint24 blockSize, t_sample_freq freq) {
    TRACE_t * this = pTraceObject;
    MixerProcessingNode_t *pMixerNode = NULL;
    t_uint24 portIdx;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = nMemoryBank;
    pAllocParams.trace_p = this;

    pMixerNode = (MixerProcessingNode_t *) vmalloc(sizeof(MixerProcessingNode_t), &pAllocParams);
    ASSERT(pMixerNode!=NULL);

    pMixerNode->sProcessingNode.pTraceObject    = pTraceObject;
    pMixerNode->sProcessingNode.nPosition       = 0;
    pMixerNode->sProcessingNode.getFilledBuffer = MixerProcessingNode_getFilledBuffer;
    pMixerNode->sProcessingNode.releaseBuffer   = MixerProcessingNode_releaseBuffer;
    pMixerNode->nbChannels                      = nbChannels;
    pMixerNode->nSamplingRate                   = freq;
    pMixerNode->pSamplesPlayedPtr               = NULL;
    pMixerNode->inplacePortIdx                  = 0;
    pMixerNode->sampleCounter[OUT]              = 0;

    for (portIdx = 1; portIdx <= MIXER_INPUT_PORT_COUNT ; portIdx++) {
        pMixerNode->inputbuf[portIdx-IN] = NULL;
        pMixerNode->upstreamNode[portIdx-IN] = NULL;
        pMixerNode->sampleCounter[portIdx-IN] = 0;
    }

    MixerProcessingNode_setAlgoDefaultSettings(pMixerNode, blockSize);

    OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::init MixerProcessingNode (0x%x)", (unsigned int)pMixerNode);

    return pMixerNode;
}

void MixerProcessingNode_destroy(MixerProcessingNode_t *pMixerNode) {
    TRACE_t * this = pMixerNode->sProcessingNode.pTraceObject;
    t_ExtendedAllocParams pAllocParams;

    OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::close MixerProcessingNode (0x%x)", (unsigned int)pMixerNode);

    pAllocParams.bank = 0;
    pAllocParams.trace_p = this;

    vfree(pMixerNode, &pAllocParams);
}

//---------------------------------------------------------------------------------

static inline void copyBuffer(Buffer_p pInputBuf, Buffer_p pOutputBuf) {
    int i;
    if(((pInputBuf->filledLen % 2)==0) && (((int)pOutputBuf->data % 2) == 0) && (((int)pInputBuf->data % 2) == 0)) {
        //Copy optimization in case where datas address and filledLen are pairs
        long * outbuf = pOutputBuf->data;
        long * inbuf = pInputBuf->data;
        int size = pInputBuf->filledLen >> 1;
        for (i = 0; i < size; i++) {
            outbuf[i] = inbuf[i];
        }
    } else {
        int * outbuf = pOutputBuf->data;
        int * inbuf = pInputBuf->data;
        int size = pInputBuf->filledLen;
        for (i = 0; i < size; i++) {
            outbuf[i] = inbuf[i];
        }
    }
}

void OutputPort_process(OutputPortNode_t *pOutputPortNode) {
    TRACE_t * this = (TRACE_t *)pOutputPortNode->pTraceObject;
    ProcessingNode_t *pUpstreamNode = pOutputPortNode->upstreamNode;
    Buffer_p pInputBuf = NULL, pOutputBuf = NULL;
    t_ErrorNodeId error = ERROR_NODE_NONE;

    //Don't do anything if no buffer available on output port fifo.
    if(!Port_queuedBufferCount(pOutputPortNode->pPortCtx)) return;

    error = pUpstreamNode->getFilledBuffer(pUpstreamNode, &pInputBuf);
    if(error == ERROR_NODE_NOT_READY) return;
    ASSERT((error == ERROR_NODE_NONE) && (pInputBuf!=NULL));

    if(pInputBuf->flags & BUFFERFLAG_EOS) {
        //Send event to proxy
        OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::OutputPort_process : send OMX_EventBufferFlag for portId %d", OUT);
        proxy.eventHandler(OMX_EventBufferFlag, OUT, pInputBuf->flags);
    }

    if(pOutputPortNode->isBufferSupplier == false) {

        //Since output port is not buffer supplier we need to copy input to output
        pOutputBuf = Port_getBuffer(pOutputPortNode->pPortCtx, 0);

        ASSERT(pOutputBuf->allocLen >= pOutputPortNode->blockSize * pOutputPortNode->nChannels);

        copyBuffer(pInputBuf, pOutputBuf);

        //Propagate flags and timestamps
        pOutputBuf->byteInLastWord  = pInputBuf->byteInLastWord;
        pOutputBuf->filledLen       = pInputBuf->filledLen;
        pOutputBuf->flags           = pInputBuf->flags;
        pOutputBuf->nTimeStamph     = pInputBuf->nTimeStamph;
        pOutputBuf->nTimeStampl     = pInputBuf->nTimeStampl;

        //Release inputBuffer
        pUpstreamNode->releaseBuffer(pUpstreamNode, pInputBuf);

    } else {
        OstTraceFiltInst0(TRACE_ERROR,"MixerMpc::OutputPort_process outputport is buffer supplier : NOT supported YET");
        ASSERT(0); //TODO: Could be supported later to allow complete inplace mode in case of 1 mixer input port!
    }

    // conditionnally probe output port
    OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::OutputPort_process pcm probe portIdx %d", OUT);
    DO_PROBE(pOutputBuf, &mPcmDumpConfig[OUT]);

    OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::OutputPort_process dequeued and return buf (0x%x) from output port fifo", (unsigned int)pOutputBuf);
    Port_dequeueAndReturnBuffer(pOutputPortNode->pPortCtx);
}

OutputPortNode_t * OutputPort_construct(t_memory_bank nMemoryBank, TRACE_t *pTraceObject, bool isBufferSupplier, Port *pPort, OutputPortParam_t * pOutputPortParam) {
    TRACE_t * this = pTraceObject;
    OutputPortNode_t * pPortNode = NULL;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = nMemoryBank;
    pAllocParams.trace_p = this;

    pPortNode = (OutputPortNode_t *) vmalloc(sizeof(OutputPortNode_t), &pAllocParams);
    ASSERT(pPortNode!=NULL);

    //Initialize OutputPortNode_t fields
    pPortNode->pTraceObject     = pTraceObject;
    pPortNode->isBufferSupplier = isBufferSupplier;
    pPortNode->nSamplingRate    = pOutputPortParam->nSamplingRate;
    pPortNode->nChannels        = pOutputPortParam->nChannels;
    pPortNode->blockSize        = pOutputPortParam->nBlockSize;
    pPortNode->pPortCtx         = pPort;
    pPortNode->isOpened           = false;

    OstTraceFiltInst4(TRACE_FLOW,"MixerMpc::init OutputPortNode (0x%x) isBufferSupplier=%d freq=%d ch=%d", (unsigned int)pPortNode, pPortNode->isBufferSupplier, pPortNode->nSamplingRate, pPortNode->nChannels);

    return pPortNode;
}

void OutputPort_open(OutputPortNode_t *pPortNode) {
    pPortNode->isOpened = true;
}

void OutputPort_destroy(OutputPortNode_t *pPortNode) {
    TRACE_t * this = (TRACE_t *)pPortNode->pTraceObject;
    t_ExtendedAllocParams pAllocParams;
    OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::close OutputPortNode (0x%x)", (unsigned int)pPortNode);

    pAllocParams.bank = 0;
    pAllocParams.trace_p = this;

    vfree(pPortNode, &pAllocParams);
}

