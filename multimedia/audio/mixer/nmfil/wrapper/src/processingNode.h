/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file  processingNode.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _processingNode_h_
#define _processingNode_h_

#include "mixer_defines.idt"
#include "libeffects/libmixer/include/mixer.h"

typedef enum {
    ERROR_NODE_NONE,
    ERROR_NODE_NOT_READY
} t_ErrorNodeId;

PcmDump_t * getOMXPortPcmDumpStruct(t_uint24 omx_port_idx);

//---------------------------------------------------------------------------------

//ProcessingNode_t : base class
typedef struct ProcessingNode_t {
    TRACE_t *  pTraceObject;
    t_uint24   nPosition;
    //getFilledBuffer return values:
    //  - Normal case: return ERROR_NODE_NONE with *buf!=NULL
    //  - *buf can be NULL with ERROR_NODE_NONE if EOS input or port paused or port disabled. In that case
    //  mixer does not need to wait buffer on this port.
    //  - return ERROR_NODE_NOT_READY if no buffer available on this port. In that case mixer must wait buffer
    //  on this port before mixing
    t_ErrorNodeId (*getFilledBuffer)(struct ProcessingNode_t * pNode, Buffer_p *buf);
    void (*releaseBuffer)(struct ProcessingNode_t * pNode, Buffer_p buf);
} ProcessingNode_t;

void ProcessingNode_configureProbe(PcmDumpConfigure_t sConfigure, TRACE_t *pTraceObject, PcmDump_t *pPcmDump);

//---------------------------------------------------------------------------------

//PcmProcessingNode_t : Derived class from ProcessingNode_t (used for Inplace/NotInplace PcmProcessing nodes)
typedef struct PcmProcessingNode_t {
    ProcessingNode_t                sProcessingNode;
    PcmDump_t                       sPcmDumpConfig;
    EffectDescription_t *           pEffectDescription;
    void *                          pEffectCtx;
    Buffer_p                        inputbuf;   //only used for Not inplace PcmProcessing nodes
    Buffer_p                        outputbuf;  //only used for Not inplace PcmProcessing nodes
    ProcessingNode_t *              upstreamNode;
} PcmProcessingNode_t;

PcmProcessingNode_t * PcmProcessingNode_construct(EffectInit_t *pEffectInitParams, t_uint24 nPosition, const char* sEffectName);
void PcmProcessingNode_open(PcmProcessingNode_t *pNode, EffectOpen_t* pOpenParameters);
void PcmProcessingNode_destroy(PcmProcessingNode_t *pNode);

//---------------------------------------------------------------------------------

//InputPortProcessingNode_t : Derived class from ProcessingNode_t (used by input ports nodes)
typedef struct InputPortProcessingNode_t {
    ProcessingNode_t                sProcessingNode;
    t_sample_freq                   nSamplingRate;
    t_uint24                        blockSize;
    t_uint24                        properties; //bitmask isPaused/isEos/isReady/nbCh
    Port *                          pPortCtx;
} InputPortProcessingNode_t;

InputPortProcessingNode_t * InputPort_construct(t_memory_bank nMemoryBank, TRACE_t *pTraceObject, Port *pPort, InputPortParam_t *pInputPortParams);
void InputPort_open(InputPortProcessingNode_t *pNode);
void InputPort_destroy(InputPortProcessingNode_t *pNode);

#define NB_CHANNELS_MASK 3      //0000 0011
#define READY_MASK      (1<<2)  //0000 0100
#define EOS_MASK        (1<<3)  //0000 1000
#define PAUSED_MASK     (1<<4)  //0001 0000

#define SET_BITS(a, mask)       ((a) |= mask)
#define CLEAR_BITS(a, mask)     ((a) &= ~(mask))
#define READ_BITS(a, mask)      ((a) & (mask))

static inline void InputPort_setNbChannels(InputPortProcessingNode_t *pNode, t_uint24 nbChannels) {
    CLEAR_BITS(pNode->properties, NB_CHANNELS_MASK); //Clear NbChannels
    pNode->properties |= nbChannels; //Set NbChannels
}

static inline void InputPort_setPaused(InputPortProcessingNode_t *pNode, bool bPaused) {
    if(bPaused == true) SET_BITS(pNode->properties,PAUSED_MASK);
    else CLEAR_BITS(pNode->properties, PAUSED_MASK);
}

static inline bool InputPort_getNbChannels(InputPortProcessingNode_t *pNode) { return READ_BITS(pNode->properties,NB_CHANNELS_MASK); }
static inline bool InputPort_isPaused(InputPortProcessingNode_t *pNode) { return READ_BITS(pNode->properties,PAUSED_MASK); }
static inline bool InputPort_isEos(InputPortProcessingNode_t *pNode) { return READ_BITS(pNode->properties,EOS_MASK); }
static inline bool InputPort_isReady(InputPortProcessingNode_t *pNode) { return READ_BITS(pNode->properties,READY_MASK); }

//---------------------------------------------------------------------------------

//MixerProcessingNode_t : Derived class from ProcessingNode_t (only used by Mixer processing node)
typedef struct MixerProcessingNode_t {
    ProcessingNode_t                sProcessingNode;
    MIXER_LOCAL_STRUCT_T            sAlgoStruct;
    t_sample_freq                   nSamplingRate;
    t_uint24		                nbChannels;
    t_uint24                        inplacePortIdx;
    Buffer_p                        inputbuf[MIXER_INPUT_PORT_COUNT];
    t_uint48                        sampleCounter[MIXER_INPUT_PORT_COUNT+1];
    MixerUseSamplesPlayed_t *       pSamplesPlayedPtr;
    ProcessingNode_t *              upstreamNode[MIXER_INPUT_PORT_COUNT];
} MixerProcessingNode_t;

MixerProcessingNode_t * MixerProcessingNode_construct(t_memory_bank nMemoryBank, TRACE_t *pTraceObject, t_uint24 nbChannels, t_uint24 blockSize, t_sample_freq freq);
void MixerProcessingNode_destroy(MixerProcessingNode_t *pMixerNode);

static inline void MixerProcessingNode_setSamplesPlayedPtr(MixerProcessingNode_t *pMixerNode, void *ptr) {
    pMixerNode->pSamplesPlayedPtr = (MixerUseSamplesPlayed_t *)ptr;
}

static inline void MixerProcessingNode_resetSamplesCounter(MixerProcessingNode_t *pMixerNode, t_uint24 portIdx) {
    pMixerNode->sampleCounter[portIdx] = 0;
    pMixerNode->pSamplesPlayedPtr->port[portIdx] = 0;
}

static inline void MixerProcessingNode_resetInputPort(MixerProcessingNode_t *pMixerNode, t_uint24 portIdx) {
    pMixerNode->sampleCounter[portIdx] = 0;
    pMixerNode->pSamplesPlayedPtr->port[portIdx] = 0;
    pMixerNode->inputbuf[portIdx-IN] = NULL;
    if(pMixerNode->inplacePortIdx == portIdx) {
        pMixerNode->inplacePortIdx = 0;
    }
}

//---------------------------------------------------------------------------------

//OutputPortNode_t : specific class (used by output port node)
typedef struct OutputPortNode_t {
    TRACE_t *                       pTraceObject;
    bool                            isBufferSupplier;
    bool                            isOpened;
    t_sample_freq                   nSamplingRate;
    t_uint24                        nChannels;
    t_uint24                        blockSize;
    Port *                          pPortCtx;
    ProcessingNode_t *              upstreamNode;
} OutputPortNode_t;

OutputPortNode_t * OutputPort_construct(t_memory_bank nMemoryBank, TRACE_t *pTraceObject, bool isBufferSupplier, Port *pPort, OutputPortParam_t * pOutputPortParam);
void OutputPort_open(OutputPortNode_t *pPortNode);
void OutputPort_destroy(OutputPortNode_t *pPortNode);
void OutputPort_process(OutputPortNode_t *pOutputPortNode);

static inline int OutputPort_hasBufferQueued(OutputPortNode_t *pOutputPortNode) {
    return Port_queuedBufferCount(pOutputPortNode->pPortCtx);
}

#endif //_processingNode_h_

