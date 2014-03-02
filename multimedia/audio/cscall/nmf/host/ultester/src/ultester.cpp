/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ultester.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/ultester.nmf>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "cscall/nmf/host/protocolhandler/inc/IsiMessage.hpp"

#define OUT 1
#define IN  0

////////////////////////////////////////////////////////////////////////
//                  Global Variables
////////////////////////////////////////////////////////////////////////
#define _DEBUG_VAR_
////////////////////////////////////////////////////////////////////////
void METH(fsmInit)(fsmInit_t initFsm)
{
    // trace init (mandatory before port init)
    if (initFsm.traceInfoAddr){
        setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    }

    mPorts[IN].init (InputPort,  false, false, 0, 0, 1, &inputport, IN, false, true, this);
    mPorts[OUT].init(OutputPort, false, false, 0, 0, 1, &outputport, OUT, false, true, this);

    init(2, mPorts, &proxy, &me, false);

    speech_codec = CODEC_AMR_NB;
}

void METH(reset)() {}
void METH(disablePortIndication)(t_uint32 portIdx) {} ;
void METH(enablePortIndication)(t_uint32 portIdx) {} ;
void METH(flushPortIndication)(t_uint32 portIdx) {} ;

void METH(process)() {}

////////////////////////////////////////////////////////////////////////
//                  Provided Interfaces
////////////////////////////////////////////////////////////////////////
void cscall_nmf_host_ultester::signal(){
    // It's time to send a new frame on uplink 
    OMX_BUFFERHEADERTYPE* bufIn;
    OMX_BUFFERHEADERTYPE* bufOut;
    unsigned int i;

    if(mPorts[IN].queuedBufferCount() == 0)
        //	ASSERT(0);
        return;

    if (mPorts[OUT].queuedBufferCount() == 0)
        //	ASSERT(0);
        return;

    bufIn   = mPorts[IN].dequeueBuffer();
    bufOut  = mPorts[OUT].dequeueBuffer();

    if(speech_codec == CODEC_AMR_NB)
    {
        MODEM_AUDIO_AMR_FRAME_T * amr_frame = (MODEM_AUDIO_AMR_FRAME_T *)bufOut->pBuffer;

        amr_frame->reserved   = 0;
        amr_frame->RxType     = AMR_RX_SPEECH_GOOD;
        amr_frame->SFN        = SFN;
        amr_frame->TxType     = AMR_TX_SPEECH;
        amr_frame->FrameType  = MODEM_AUDIO_AMR_475;
        amr_frame->quality    = MODEM_AUDIO_SPC_AMR_QUALITY_OK;
        amr_frame->padding3   = 0;
        amr_frame->padding2   = 0;
        amr_frame->padding1   = 0;

        SFN = (SFN + 1) % 8;

        for(i=1; i<=bufIn->nFilledLen; i+=4 )      
        {
            bufOut->pBuffer[i+3] = bufIn->pBuffer[i+3];
            bufOut->pBuffer[i+4] = bufIn->pBuffer[i+2];
            bufOut->pBuffer[i+5] = bufIn->pBuffer[i+1];
            bufOut->pBuffer[i+6] = bufIn->pBuffer[i];
        }

        // a revoir a taille du header
        bufIn->nFilledLen+=3;
    }
    else
    {
        for(i=0; i<=(bufIn->nFilledLen + 3); i+=4 )      
        {
            bufOut->pBuffer[i]   = bufIn->pBuffer[i+3];
            bufOut->pBuffer[i+1] = bufIn->pBuffer[i+2];
            bufOut->pBuffer[i+2] = bufIn->pBuffer[i+1];
            bufOut->pBuffer[i+3] = bufIn->pBuffer[i];
        }
    }

    bufOut->nFilledLen = bufIn->nFilledLen;
    bufOut->nOffset    = 0;
    bufOut->nFlags     = 0;

    returnBufferAsync(IN, bufIn);
    returnBufferAsync(OUT, bufOut);

    return;
}


void METH(newCodecReq)(CsCallCodecReq_t config)
{
    speech_codec = config.speech_codec;
}

