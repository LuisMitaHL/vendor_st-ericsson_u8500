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
#include <mixer/nmfil/wrapper.nmf>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dbc.h>
#include <verbose.h>
#include "fsm/component/include/Component.inl"

#include "processingNode.h"

#include "libeffects/include/effects_description.h"
#include "libeffects/include/effects_dsp_index.h"
#include "libeffects/include/effects_dsp_types.h"

#ifndef _SIMU_

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mixer_nmfil_wrapper_src_wrapperTraces.h"
#endif

#else // _SIMU_*/
#include "libeffects/mpc/include/audiolibs_trace.h"
#endif // _SIMU_

#include "samplesplayed.idt"

#define MS_GRANULARITY  5

#define ALL_INPUTS          0x1FE   // bitmask with all input port idxs set
#define SET_BIT(a, i)       ((a) |= (1 << (i)))
#define CLEAR_BIT(a, i)     ((a) &= ~(1 << (i)))
#define IS_BIT_SET(a, i)    ((a) & (1 << (i)))

#define MAXREGISTERED   20

////////////////////////////////////////////////////////////
//			Global Variables
////////////////////////////////////////////////////////////

//State machine variable
static void *       mFifoIn[MIXER_INPUT_PORT_COUNT][1];
static void *       mFifoOut[1];
static Port         mPorts[MIXER_INPUT_PORT_COUNT+1];
static Component    mMixer;
static bool         mFsmInitialized;

//Processing Nodes variables
static InputPortProcessingNode_t *  mInputPortNode[MIXER_INPUT_PORT_COUNT];
static OutputPortNode_t *           mOutputPortNode;
static MixerProcessingNode_t *      mMixerProcessingNode;

////////////////////////////////////////////////////////////
//			Inline functions
////////////////////////////////////////////////////////////

static inline bool isPortNodeOpened(t_uint24 portIdx) {
    if(portIdx == OUT) {
        return mOutputPortNode->isOpened;
    } else {
        return InputPort_isReady(mInputPortNode[portIdx-IN]);
    }
}

static inline ProcessingNode_t * getFirstUpstreamNode(t_uint24 portIdx) {
    if(portIdx == OUT) {
        return mOutputPortNode->upstreamNode;
    } else {
        return mMixerProcessingNode->upstreamNode[portIdx-IN];
    }
}

static inline PcmProcessingNode_t * getPcmProcessingNode(t_uint24 portIdx, t_uint24 position) {
    PcmProcessingNode_t *pNode = getFirstUpstreamNode(portIdx);

    while ((pNode->sProcessingNode.nPosition!=0) && (pNode->sProcessingNode.nPosition != position))
    {
        pNode = pNode->upstreamNode;
    }

    ASSERT(pNode!=NULL);
    return pNode;
}

static inline int getPortIdxFromEffectCtx(void *pEffectCtx) {
    PcmProcessingNode_t *pNode = NULL;
    t_uint24 portIdx;

    for (portIdx = 1; portIdx <= MIXER_INPUT_PORT_COUNT; portIdx++) {
        pNode = mMixerProcessingNode->upstreamNode[portIdx-IN];;
        while ((pNode!=NULL) && (pNode->pEffectCtx != pEffectCtx))
        {
            pNode = pNode->upstreamNode;
        }
        if(pNode != NULL) return portIdx;
    }

    ASSERT(portIdx<=MIXER_INPUT_PORT_COUNT);
    return portIdx;
}

static inline void openPcmProcessingNodeOnOutputPort(PcmProcessingNode_t *pPcmProcessingNode) {
    EffectOpen_t sEffectopen;

    sEffectopen.sInputPcmSettings.nBlockSize        = mOutputPortNode->blockSize;
    sEffectopen.sOutputPcmSettings.nBlockSize       = mOutputPortNode->blockSize;
    sEffectopen.sInputPcmSettings.nSamplingRate     = mOutputPortNode->nSamplingRate;
    sEffectopen.sOutputPcmSettings.nSamplingRate    = mOutputPortNode->nSamplingRate;
    sEffectopen.sInputPcmSettings.nChannels         = mOutputPortNode->nChannels;
    sEffectopen.sOutputPcmSettings.nChannels        = mOutputPortNode->nChannels;
    PcmProcessingNode_open(pPcmProcessingNode, &sEffectopen);
}

static inline void openPcmProcessingNodeOnInputPort(PcmProcessingNode_t *pPcmProcessingNode, t_uint24 portIdx) {
    EffectOpen_t sEffectopen;

    sEffectopen.sInputPcmSettings.nBlockSize            = mInputPortNode[portIdx-IN]->blockSize;
    sEffectopen.sOutputPcmSettings.nBlockSize           = mOutputPortNode->blockSize;
    if(pPcmProcessingNode->sProcessingNode.nPosition == INPUT_SRC_POSITION) {
        sEffectopen.sInputPcmSettings.nSamplingRate     = mInputPortNode[portIdx-IN]->nSamplingRate;
        sEffectopen.sOutputPcmSettings.nSamplingRate    = mOutputPortNode->nSamplingRate;
    } else {
        sEffectopen.sInputPcmSettings.nSamplingRate     = mInputPortNode[portIdx-IN]->nSamplingRate;
        sEffectopen.sOutputPcmSettings.nSamplingRate    = mInputPortNode[portIdx-IN]->nSamplingRate;
    }
    if(InputPort_getNbChannels(mInputPortNode[portIdx-IN]) != mOutputPortNode->nChannels) {
        if ((InputPort_getNbChannels(mInputPortNode[portIdx-IN]) == 2) && (mOutputPortNode->nChannels == 1)) { //Downmix
            if(pPcmProcessingNode->sProcessingNode.nPosition == INPUT_DOWNMIX_POSITION) {
                sEffectopen.sInputPcmSettings.nChannels     = 2;
                sEffectopen.sOutputPcmSettings.nChannels    = 1;
            } else {
                sEffectopen.sInputPcmSettings.nChannels     = 1;
                sEffectopen.sOutputPcmSettings.nChannels    = 1;
            }
        } else { //Upmix
            if(pPcmProcessingNode->sProcessingNode.nPosition == INPUT_UPMIX_POSITION) {
                sEffectopen.sInputPcmSettings.nChannels     = 1;
                sEffectopen.sOutputPcmSettings.nChannels    = 2;
            } else if(pPcmProcessingNode->sProcessingNode.nPosition > INPUT_UPMIX_POSITION) {
                sEffectopen.sInputPcmSettings.nChannels     = 2;
                sEffectopen.sOutputPcmSettings.nChannels    = 2;
            } else {
                sEffectopen.sInputPcmSettings.nChannels     = 1;
                sEffectopen.sOutputPcmSettings.nChannels    = 1;
            }
        }
    } else {
        sEffectopen.sInputPcmSettings.nChannels     = mOutputPortNode->nChannels;
        sEffectopen.sOutputPcmSettings.nChannels    = mOutputPortNode->nChannels;
    }

    PcmProcessingNode_open(pPcmProcessingNode, &sEffectopen);
}

static inline void closeAllPcmProcessingNodesOnPort(t_uint24 portIdx) {
    ProcessingNode_t *pNode = getFirstUpstreamNode(portIdx);
    while (pNode->nPosition!=0)
    {
        PcmProcessingNode_t *pPcmProcessingNode = (PcmProcessingNode_t *) pNode;
        PcmProcessingNode_t *pUstreamNode = pPcmProcessingNode->upstreamNode;
        PcmProcessingNode_destroy(pPcmProcessingNode);
        pNode = (ProcessingNode_t *)pUstreamNode;
    }
}

////////////////////////////////////////////////////////////
//			EVENTS FROM EFFECTS
////////////////////////////////////////////////////////////

void Mixer_effectEventHandler(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2, void *pEffectCtx) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    t_uint24 portIdx = getPortIdxFromEffectCtx(pEffectCtx);
    OstTraceFiltInst3(TRACE_DEBUG,"MixerMpc::Mixer_effectEventHandler : send event to proxy ev=%d, portId=%d, data2=%d", event, portIdx, (unsigned int)data2);
    proxy.eventHandler(event, portIdx, data2);
}

////////////////////////////////////////////////////////////
//			static functions
////////////////////////////////////////////////////////////

static void initAllProcessingNodesOnInputPort(t_uint24 portIdx, InputPortParam_t * pInputPortParam) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    PcmProcessingNode_t * pSrcEffect = NULL;
    PcmProcessingNode_t * pVolctrlEffect = NULL;
    PcmProcessingNode_t * pDownmixEffect = NULL;
    PcmProcessingNode_t * pUpmixEffect = NULL;
    EffectInit_t sEffectinit;

    OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::initAllProcessingNodesOnInputPort %d", portIdx);

    sEffectinit.pTraceObject = this;
    sEffectinit.nMemoryBank = pInputPortParam->nMemoryBank;
    sEffectinit.eventHandler = Mixer_effectEventHandler;

    //InputPort
    mInputPortNode[portIdx-IN] = InputPort_construct(MEM_XTCM, this, &mPorts[portIdx], pInputPortParam);

    //Init Volctrl
    pVolctrlEffect = PcmProcessingNode_construct(&sEffectinit, INPUT_VOLCTRL_POSITION, "volctrl_effect");

    //Init Updownmix if needed
    if(pInputPortParam->nChannels != mOutputPortNode->nChannels) {
        if (pInputPortParam->nChannels == 2 && mOutputPortNode->nChannels == 1) {
            pDownmixEffect = PcmProcessingNode_construct(&sEffectinit, INPUT_DOWNMIX_POSITION, "updownmix_effect");
        } else {
            pUpmixEffect = PcmProcessingNode_construct(&sEffectinit, INPUT_UPMIX_POSITION, "updownmix_effect");
        }
    }

    //Init SRC if needed
    if(pInputPortParam->nSamplingRate != mOutputPortNode->nSamplingRate) {
        pSrcEffect = PcmProcessingNode_construct(&sEffectinit, INPUT_SRC_POSITION, "resampling_effect");
    }

    //Connect PcmProcessingNodes : order is always like that downmix/SRC/Volctrl or SRC/Upmix/Volctrl
    //This allows PcmProcessingNodes to always work in mono in updownmix cases
    //And Upmix must be before Volctrl to manage volctrl balance configurations
    mMixerProcessingNode->upstreamNode[portIdx-IN] = pVolctrlEffect;
    if(pUpmixEffect != NULL) {
        //Order is <SRC>/Upmix/Volctrl
        pVolctrlEffect->upstreamNode = pUpmixEffect;
        if(pSrcEffect != NULL) {
            pUpmixEffect->upstreamNode  = pSrcEffect;
            pSrcEffect->upstreamNode    = mInputPortNode[portIdx-IN];
        } else {
            pUpmixEffect->upstreamNode  = mInputPortNode[portIdx-IN];
        }
    } else if (pDownmixEffect != NULL) {
        //Order is Downmix/<SRC>/Volctrl
        if(pSrcEffect != NULL) {
            pVolctrlEffect->upstreamNode    = pSrcEffect;
            pSrcEffect->upstreamNode        = pDownmixEffect;
        } else {
            pVolctrlEffect->upstreamNode    = pDownmixEffect;
        }
        pDownmixEffect->upstreamNode = mInputPortNode[portIdx-IN];
    } else {
        //Order is <SRC>/Volctrl
        if(pSrcEffect != NULL) {
            pVolctrlEffect->upstreamNode    = pSrcEffect;
            pSrcEffect->upstreamNode        = mInputPortNode[portIdx-IN];
        } else {
            pVolctrlEffect->upstreamNode    = mInputPortNode[portIdx-IN];
        }
    }
}

static void openAllProcessingNodesOnInputPort(t_uint24 portIdx) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    ProcessingNode_t *pNode = mMixerProcessingNode->upstreamNode[portIdx-IN];

    OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::openAllProcessingNodesOnInputPort %d", (unsigned int)portIdx);

    while (pNode->nPosition!=0)
    {
        PcmProcessingNode_t *pPcmProcessingNode = (PcmProcessingNode_t *) pNode;
        openPcmProcessingNodeOnInputPort(pPcmProcessingNode, portIdx);
        pNode = pPcmProcessingNode->upstreamNode;
    }

    //Now that PcmProcessing nodes have been opened, we can open inputPortNode
    InputPort_open(mInputPortNode[portIdx-IN]);
}

static void closeAllProcessingNodesOnInputPort(t_uint24 portIdx) {
    TRACE_t * this = (TRACE_t *)&mMixer;

    OstTraceFiltInst1(TRACE_DEBUG,"MixerMpc::closeAllProcessingNodesOnInputPort %d", (unsigned int)portIdx);

    closeAllPcmProcessingNodesOnPort(portIdx);

    mMixerProcessingNode->upstreamNode[portIdx-IN] = NULL;

    //close input Port
    InputPort_destroy(mInputPortNode[portIdx-IN]);
    mInputPortNode[portIdx-IN] = NULL;
}

static void openAllEnabledInputPorts(void) {
    t_uint24 portIdx;
    TRACE_t * this = (TRACE_t *)&mMixer;

    OstTraceFiltInst0(TRACE_FLOW,"MixerMpc::openAllEnabledInputPorts");

    for (portIdx = 1 ; portIdx <= MIXER_INPUT_PORT_COUNT; portIdx++) {
        if (mInputPortNode[portIdx-IN] == NULL) continue; //input port is disabled!
        openAllProcessingNodesOnInputPort(portIdx);
    }
}

static void closeAllEnabledInputPorts(void) {
    t_uint24 portIdx;
    TRACE_t * this = (TRACE_t *)&mMixer;

    OstTraceFiltInst0(TRACE_FLOW,"MixerMpc::closeAllEnabledInputPorts");

    for (portIdx = 1 ; portIdx <= MIXER_INPUT_PORT_COUNT; portIdx++) {
        if (mInputPortNode[portIdx-IN] == NULL) continue; //input port is disabled!
        closeAllProcessingNodesOnInputPort(portIdx);
    }
}

static void initAllProcessingNodesOnOutputPort(OutputPortParam_t *pOutputPortParam) {
    TRACE_t * this = (TRACE_t *)&mMixer;

    OstTraceFiltInst3(TRACE_DEBUG,"MixerMpc::initAllProcessingNodesOnOutputPort Freq=%d Channels=%d Blocksize=%d", pOutputPortParam->nSamplingRate, pOutputPortParam->nChannels, pOutputPortParam->nBlockSize);

    //Init Mixer component
    mMixerProcessingNode = MixerProcessingNode_construct(MEM_XTCM, this, pOutputPortParam->nChannels, pOutputPortParam->nBlockSize, pOutputPortParam->nSamplingRate);

    //Output Port
    mOutputPortNode = OutputPort_construct(MEM_XTCM, this, false, &mPorts[OUT], pOutputPortParam);
    mOutputPortNode->upstreamNode = mMixerProcessingNode;
}

static void closeAllProcessingNodesOnOutputPort(void) {
    TRACE_t * this = (TRACE_t *)&mMixer;

    OstTraceFiltInst0(TRACE_DEBUG,"MixerMpc::closeAllProcessingNodesOnOutputPort");

    closeAllPcmProcessingNodesOnPort(OUT);

    //close Mixer component
    MixerProcessingNode_destroy(mMixerProcessingNode);
    mMixerProcessingNode = NULL;

    //close output Port
    OutputPort_destroy(mOutputPortNode);
    mOutputPortNode = NULL;
}

static void openAllProcessingNodesOnOutputPort(void) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    ProcessingNode_t *pNode = mOutputPortNode->upstreamNode;

    OstTraceFiltInst0(TRACE_DEBUG,"MixerMpc::openAllProcessingNodesOnOutputPort");

    while (pNode->nPosition!=0)
    {
        PcmProcessingNode_t *pPcmProcessingNode = (PcmProcessingNode_t *) pNode;
        openPcmProcessingNodeOnOutputPort(pPcmProcessingNode);
        pNode = pPcmProcessingNode->upstreamNode;
    }

    //Now that PcmProcessing nodes have been opened, we can open outputPortNode
    OutputPort_open(mOutputPortNode);
}

static void initAndInsertPcmProcessingNode(t_uint24 portIdx, t_uint24 position, const char* sEffectName) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    PcmProcessingNode_t * pNewNode = NULL;
    PcmProcessingNode_t * pPrevNode   = NULL;
    PcmProcessingNode_t * pNextNode   = NULL;
    EffectInit_t sEffectinit;

    OstTraceFiltInst2(TRACE_DEBUG,"MixerMpc::initAndInsertPcmProcessingNode portIdx=%d effectPosition=%d", portIdx, position);

    sEffectinit.pTraceObject = this;
    sEffectinit.nMemoryBank = MEM_XTCM;
    sEffectinit.eventHandler = Mixer_effectEventHandler;

    pNewNode = PcmProcessingNode_construct(&sEffectinit, position , sEffectName);

    pPrevNode = getFirstUpstreamNode(portIdx);
    if((portIdx == OUT) && (position > pPrevNode->sProcessingNode.nPosition)) {
        //1st effect to be added in mixer output port
        pNewNode->upstreamNode = mOutputPortNode->upstreamNode;
        mOutputPortNode->upstreamNode = pNewNode;
    } else {
        while ((pPrevNode->sProcessingNode.nPosition!=0) && (position <= pPrevNode->sProcessingNode.nPosition))
        {
            pNextNode = pPrevNode;
            pPrevNode = pPrevNode->upstreamNode;
        }
        //An effect can not be added at same position of another effect
        //Previous effect at same position must first be removed.
        ASSERT(position!=pPrevNode->sProcessingNode.nPosition);

        pNewNode->upstreamNode  = pNextNode->upstreamNode;
        pNextNode->upstreamNode = pNewNode;
    }
}

static void openPcmProcessingNode(t_uint24 portIdx, t_uint24 position) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    PcmProcessingNode_t *pPcmProcessingNode = NULL;

    OstTraceFiltInst2(TRACE_DEBUG,"MixerMpc::openPcmProcessingNode on portId %d at position %d", portIdx, position);

    pPcmProcessingNode = getPcmProcessingNode(portIdx, position);

    if(portIdx == OUT) {
        openPcmProcessingNodeOnOutputPort(pPcmProcessingNode);
    } else {
        openPcmProcessingNodeOnInputPort(pPcmProcessingNode, portIdx);
    }
}

static void closeAndRemovePcmProcessingNode(t_uint24 portIdx, t_uint24 position) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    PcmProcessingNode_t * pNodetoRemove = NULL;
    PcmProcessingNode_t * pPrevNode   = NULL;
    PcmProcessingNode_t * pNextNode   = NULL;
    EffectInit_t sEffectinit;

    OstTraceFiltInst2(TRACE_DEBUG,"MixerMpc::closeAndRemovePcmProcessingNode portIdx=%d effectPosition=%d", portIdx, position);

    pNodetoRemove = getFirstUpstreamNode(portIdx);
    while (position < pNodetoRemove->sProcessingNode.nPosition)
    {
        pNextNode = pNodetoRemove;
        pNodetoRemove = pNodetoRemove->upstreamNode;
    }
    ASSERT(pNodetoRemove->sProcessingNode.nPosition == position);

    if(pNodetoRemove == mOutputPortNode->upstreamNode){
        //We want to remove 1st upstream node : can only possible for output port.
        //Indeed for input port we always have one volctrl at position 65535
        mOutputPortNode->upstreamNode = pNodetoRemove->upstreamNode;
    } else {
        pNextNode->upstreamNode = pNodetoRemove->upstreamNode;
    }

    PcmProcessingNode_destroy(pNodetoRemove);
}

////////////////////////////////////////////////////////////
//			Virtual methods
////////////////////////////////////////////////////////////

void Mixer_process(Component *this) {
    if(mOutputPortNode == NULL) return; //output port is disabled
    OutputPort_process(mOutputPortNode);
}

void Mixer_reset(Component *this) {
    t_uint24 portIdx;

    OstTraceFiltInst0(TRACE_FLOW,"MixerMpc::Mixer_reset");

    if(mMixerProcessingNode != NULL) {
        for (portIdx = 0 ; portIdx <= MIXER_INPUT_PORT_COUNT ; portIdx++) {
            //Reset MixerProcessingNode variables on this input port
            MixerProcessingNode_resetInputPort(mMixerProcessingNode, portIdx);
        }
    }
}

void Mixer_disablePortIndication(t_uint32 portIdx) {
    TRACE_t * this = (TRACE_t *)&mMixer;

    OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::disablePortIndication  on port %d", (t_uint24)portIdx);

    if (portIdx != OUT) {
        if (mOutputPortNode != NULL) {
            //Reset MixerProcessingNode variables on this input port
            MixerProcessingNode_resetInputPort(mMixerProcessingNode, (t_uint24)portIdx);
            //Close all pcm processing nodes on this input port
            closeAllProcessingNodesOnInputPort((t_uint24)portIdx);
            if(OutputPort_hasBufferQueued(mOutputPortNode)) {
                // mixer may be able to generate output buf now
                // that this input port is disabled
                Component_scheduleProcessEvent(&mMixer);
            }
        }
    } else {
        closeAllEnabledInputPorts();
        closeAllProcessingNodesOnOutputPort();
    }
}

void Mixer_enablePortIndication(t_uint32 portIdx) {
    TRACE_t * this = (TRACE_t *)&mMixer;

    OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::enablePortIndication  on port %d", (t_uint24)portIdx);

    if (portIdx != OUT) {
        if (mOutputPortNode != NULL) {
            openAllProcessingNodesOnInputPort((t_uint24)portIdx);
        }
    } else {
        openAllProcessingNodesOnOutputPort();
        openAllEnabledInputPorts();
    }
}

void Mixer_flushPortIndication(t_uint32 portIdx) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::flushPortIndication  on port %d", (t_uint24)portIdx);
}

////////////////////////////////////////////////////////////
//			Provided Interfaces
////////////////////////////////////////////////////////////

void METH(start)(void){
    t_uint24 portIdx;
    //Global variables initialization
    for (portIdx = 1 ; portIdx <= MIXER_INPUT_PORT_COUNT; portIdx++) {
        mInputPortNode[portIdx-IN] = NULL;
    }
    mOutputPortNode = NULL;
    mMixerProcessingNode = NULL;
    mFsmInitialized = false;
}

void METH(stop)(void){
    //Memory deallocation
    closeAllEnabledInputPorts();
    if(mOutputPortNode != NULL) {
        closeAllProcessingNodesOnOutputPort();
    }
}

void METH(configureEffects)(t_uint32 configDSPAddress) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    MixerConfig_t * pMixerConfig = (MixerConfig_t *) configDSPAddress;
    MixerEffectConfig_t * pEffectConfig = NULL;
    PcmProcessingNode_t *pProcessingNode = NULL;
    t_EffectError err = EFFECT_ERROR_NONE;
    t_uint24 effectconfigDSPAddress;
    int index;

    PRECONDITION(mOutputPortNode != NULL);

    //Flush data cache for making structure fields readable by mpc
    MMDSP_FLUSH_DCACHE();

    effectconfigDSPAddress = (t_uint32)pMixerConfig->pEffectConfigTable;

    for(index=0; index<pMixerConfig->nbConfigs; index++) {
        pEffectConfig = (MixerEffectConfig_t *)effectconfigDSPAddress;
        ASSERT(pEffectConfig!=NULL);
        ASSERT(pEffectConfig->portIdx <= MIXER_INPUT_PORT_COUNT);

        switch(pEffectConfig->configIndex) {
            case AFM_DSP_IndexConfigNone:
                {
                    OstTraceFiltInst3(TRACE_DEBUG,"MixerMpc::configureEffects with AFM_DSP_IndexConfigNone for pProcessingNode=0x%x portIdx=%d effectPosition=%d (config ignored - already applied)", (unsigned int)pProcessingNode, pEffectConfig->portIdx, pEffectConfig->effectPosition);
                    break;
                }
            case OMX_DSP_IndexConfigAudioEffectAddPosition:
                {
                    AudioConfigAddEffect_t * pConfig = (AudioConfigAddEffect_t *)pEffectConfig->pSpecificConfigStruct;
                    initAndInsertPcmProcessingNode(pEffectConfig->portIdx, pEffectConfig->effectPosition, (char*)pConfig->cEffectName);
                    if((mFsmInitialized == true)&&(isPortNodeOpened(pEffectConfig->portIdx))){
                        //Open PcmProcessingNode only when mixer is in executing or pause state and port is enabled
                        openPcmProcessingNode(pEffectConfig->portIdx, pEffectConfig->effectPosition);
                    }
                    break;
                }
            case OMX_DSP_IndexConfigAudioEffectRemovePosition:
                {
                    closeAndRemovePcmProcessingNode(pEffectConfig->portIdx, pEffectConfig->effectPosition);
                    break;
                }
            case AFM_DSP_IndexConfigPcmProbe:
                {
                    AudioConfigPcmProbe_t * pConfig = (AudioConfigPcmProbe_t *)pEffectConfig->pSpecificConfigStruct;
                    PcmDumpConfigure_t sPcmDumpConfig;
                    sPcmDumpConfig.nmf_port_idx         = pEffectConfig->portIdx;
                    sPcmDumpConfig.omx_port_idx         = pEffectConfig->portIdx;
                    sPcmDumpConfig.effect_position_idx  = pEffectConfig->effectPosition;
                    sPcmDumpConfig.enable               = pConfig->bEnable;
                    sPcmDumpConfig.buffer               = (void*)pConfig->buffer;
                    sPcmDumpConfig.buffer_size          = pConfig->buffer_size;

                    pProcessingNode = getPcmProcessingNode(pEffectConfig->portIdx, pEffectConfig->effectPosition);

                    OstTraceFiltInst3(TRACE_DEBUG,"MixerMpc::configure_pcmprobe for pProcessingNode=0x%x portIdx=%d effectPosition=%d", (unsigned int)pProcessingNode, pEffectConfig->portIdx, pEffectConfig->effectPosition);
                    OstTraceFiltInst3(TRACE_DEBUG,"MixerMpc::configure_pcmprobe bEnable=%d buffer=0x%x buffer_size=%d", pConfig->bEnable, pConfig->buffer, pConfig->buffer_size);

                    ProcessingNode_configureProbe(sPcmDumpConfig, (TRACE_t*)&mMixer, &pProcessingNode->sPcmDumpConfig);
                    break;
                }
            default:
                {
                    pProcessingNode = getPcmProcessingNode(pEffectConfig->portIdx, pEffectConfig->effectPosition);

                    OstTraceFiltInst4(TRACE_DEBUG,"MixerMpc::configure pProcessingNode=0x%x portIdx=%d effectPosition=%d Index=%d", (unsigned int)pProcessingNode, pEffectConfig->portIdx, pEffectConfig->effectPosition, pEffectConfig->configIndex);

                    if(pEffectConfig->isStaticConfiguration) {
                        ASSERT(pProcessingNode->pEffectDescription->effect_set_parameter(pProcessingNode->pEffectCtx, pEffectConfig->configIndex, pEffectConfig->pSpecificConfigStruct) == EFFECT_ERROR_NONE);
                    } else {
                        ASSERT(pProcessingNode->pEffectDescription->effect_set_config(pProcessingNode->pEffectCtx, pEffectConfig->configIndex, pEffectConfig->pSpecificConfigStruct) == EFFECT_ERROR_NONE);
                    }
                    break;
                }
        }

        effectconfigDSPAddress += sizeof(MixerEffectConfig_t);
    }

    setConfigApplied.newConfigApplied(pMixerConfig->configStructArmAddress);
}

void METH(setPausedInputPort)(t_uint16 portIdx, BOOL bIsPaused) {
    TRACE_t * this = (TRACE_t *)&mMixer;

    PRECONDITION(portIdx <= MIXER_INPUT_PORT_COUNT && portIdx >= 1);
    PRECONDITION(mOutputPortNode != NULL); //SetConfig called only when output port is enabled
    PRECONDITION(mInputPortNode[portIdx-IN] != NULL);

    OstTraceFiltInst2(TRACE_FLOW,"MixerMpc::setPausedInputPort portIdx=%d Paused=%d", portIdx, bIsPaused);

    InputPort_setPaused(mInputPortNode[portIdx-IN], bIsPaused);

    if(bIsPaused == true) {
        if (OutputPort_hasBufferQueued(mOutputPortNode)) {
            // mixer may be able to generate output buf now
            // that this input port is paused
            Component_scheduleProcessEvent(&mMixer);
        }
    }
}

void METH(setInputPortParameter)(t_uint16 portIdx, InputPortParam_t inputPortParam) {

    int *p;
    TRACE_t * this = (TRACE_t *)&mMixer;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = inputPortParam.nMemoryBank;
    pAllocParams.trace_p = this;

    PRECONDITION(portIdx <= MIXER_INPUT_PORT_COUNT && portIdx >= 1);
    PRECONDITION(mOutputPortNode != NULL);

    if (inputPortParam.bDisabled) {
        mInputPortNode[portIdx-IN] = NULL;
        OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::setInputPortParameter portIdx=%d is disabled", portIdx);
        return;
    }

    OstTraceFiltInst4(TRACE_FLOW,"MixerMpc::setInputPortParameter portIdx=%d Freq=%d Channels=%d Blocksize=%d", portIdx, inputPortParam.nSamplingRate, inputPortParam.nChannels, inputPortParam.nBlockSize);

    initAllProcessingNodesOnInputPort(portIdx, &inputPortParam);
}

void METH(setParameter)(MixerParam_t mixerParam)
{
    t_uint24 portIdx;
    TRACE_t * this = (TRACE_t *)&mMixer;

    mMixer.fsm.traceObject.mTraceInfoPtr = (TraceInfo_t *)mixerParam.traceAddr;

    OstTraceFiltInst0(TRACE_FLOW,"MixerMpc::setParameter");

    //setParameter must not be called if output port is disabled
    ASSERT(mixerParam.outputPortParam.bDisabled == false);

    initAllProcessingNodesOnOutputPort(&mixerParam.outputPortParam);

    for (portIdx = 1; portIdx <= MIXER_INPUT_PORT_COUNT; portIdx++) {
       METH(setInputPortParameter)(portIdx, mixerParam.inputPortParam[portIdx-IN]);
    }
}

void METH(fsmInit) (fsmInit_t initFsm) {
    t_uint24 idx;
    TRACE_t * this = (TRACE_t *)&mMixer;
    int needs = 0;

    // trace init (mandatory before port init)
    FSM_traceInit(&mMixer, initFsm.traceInfoAddr, initFsm.id1);

    OstTraceFiltInst1(TRACE_FLOW,"MixerMpc::fsmInit portsDisabled=%d", initFsm.portsDisabled);

    for (idx = 0 ; idx < MIXER_INPUT_PORT_COUNT ; idx++) {
        Port_init(&mPorts[IN+idx], InputPort, false, false, 0, &mFifoIn[idx],
                1, &inputport[idx], IN+idx, (initFsm.portsDisabled & (1 << (IN+idx))),
                (initFsm.portsTunneled & (1 << (IN+idx))), &mMixer);
    }

    Port_init(&mPorts[OUT], OutputPort, false, false, 0, &mFifoOut, 1, &outputport, OUT,
            (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mMixer);

    mMixer.reset                    = Mixer_reset;
    mMixer.process                  = Mixer_process;
    mMixer.disablePortIndication    = Mixer_disablePortIndication;
    mMixer.enablePortIndication     = Mixer_enablePortIndication;
    mMixer.flushPortIndication      = Mixer_flushPortIndication;

    if (mOutputPortNode != NULL) { //check if output port is enabled
        openAllProcessingNodesOnOutputPort();
        openAllEnabledInputPorts();
    }

    Component_init(&mMixer, MIXER_INPUT_PORT_COUNT + 1, mPorts, &proxy);

    mFsmInitialized = true;
}


void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}


void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize, t_uint8 idx)
{
    TRACE_t * this = (TRACE_t *)&mMixer;
    OstTraceFiltInst3(TRACE_FLOW,"MixerMpc::newFormat received on portIdx=%d freq=%d chans_nb=%d", idx+1, freq, chans_nb);
}

void METH(processEvent)() {
    Component_processEvent(&mMixer);
}

void METH(emptyThisBuffer)(Buffer_p buf, t_uint8 idx) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    OstTraceFiltInst3(TRACE_FLOW,"MixerMpc::emptyThisBuffer portIdx=%d  filledLen=%d flags=%d", idx+1, (unsigned int)buf->filledLen, buf->flags);

    Component_deliverBuffer(&mMixer, IN+idx, buf);
}

void METH(fillThisBuffer)(Buffer_p buf) {
    TRACE_t * this = (TRACE_t *)&mMixer;
    OstTraceFiltInst0(TRACE_FLOW,"MixerMpc::fillThisBuffer");

    Component_deliverBuffer(&mMixer, OUT, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    Component_sendCommand(&mMixer, cmd, param);
}

void METH(SetSamplesPlayedAddr)(void * ptr){
    PRECONDITION(mMixerProcessingNode!=NULL);
    MixerProcessingNode_setSamplesPlayedPtr(mMixerProcessingNode, ptr);
}

void METH(setSamplesPlayedCounter)(t_uint16 portIdx, t_uint32 value){
    PRECONDITION(mMixerProcessingNode!=NULL);
    PRECONDITION(value == 0);
    MixerProcessingNode_resetSamplesCounter(mMixerProcessingNode, portIdx);
}

void METH(setEffectGetInfoPtr)(t_uint16 portIdx, t_uint16 positionId, void * ptr){
    PcmProcessingNode_t *pProcessingNode = getPcmProcessingNode(portIdx, positionId);
    ASSERT(pProcessingNode->pEffectDescription->effect_set_parameter(pProcessingNode->pEffectCtx, OMX_DSP_ParamSetEffectInfoPtr, ptr) == EFFECT_ERROR_NONE);
}


void METH(configure_pcmprobe)(PcmDumpConfigure_t sConfigure)
{
    t_sint24 tab_idx;
    TRACE_t * this = (TRACE_t*)&mMixer;
	ASSERT(sConfigure.nmf_port_idx == sConfigure.omx_port_idx);
    ASSERT(sConfigure.nmf_port_idx < (MIXER_INPUT_PORT_COUNT + 1));

    // configure the pcmdump singleton
    OstTraceFiltInst3(TRACE_ALWAYS, "MixerMpc::configure_pcmprobe: nmf port idx: %d, enable: %d, omx port idx: %d", sConfigure.nmf_port_idx, sConfigure.enable, sConfigure.omx_port_idx);
    OstTraceFiltInst2(TRACE_ALWAYS, "MixerMpc::configure_pcmprobe: buffer: 0x%x, buffer size: %d", (t_uint24)sConfigure.buffer, sConfigure.buffer_size);

    ProcessingNode_configureProbe(sConfigure, this, getOMXPortPcmDumpStruct(sConfigure.omx_port_idx));
}
