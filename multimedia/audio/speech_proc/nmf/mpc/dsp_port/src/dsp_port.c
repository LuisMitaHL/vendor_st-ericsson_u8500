/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dsp_port.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <speech_proc/nmf/mpc/dsp_port.nmf>
#include "fsm/component/include/Component.inl"
#include <stdbool.h>
#include <dbc.h>


#define OUTPUT_PORT 0
#define INPUT_PORT  1

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "speech_proc_nmf_mpc_dsp_port_src_dsp_portTraces.h"
#endif

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////
static void *       mFifoIn[1];
static void *       mFifoOut[1];
static Port         mPorts[2];
static Component    mComponent;

static t_uint16      mSendNewFormat = 0;
static t_uint16      mInExecutingState = 0;
static t_uint16      mIsDisabled = 0;
static t_sample_freq mSampleFreq;
static t_uint16      mChansNb;

void propagateNewFormatIfNeeded(void);
////////////////////////////////////////////////////////////
//   COMPONENT methods
////////////////////////////////////////////////////////////
static void process(Component *this)
{
  Buffer_p pInputBuf = 0;
  Buffer_p pOutputBuf = 0;
  t_sword *data_in;
  t_sword *data_out;
  int i;

  if ( !Port_queuedBufferCount(&mPorts[INPUT_PORT]) ||
       !Port_queuedBufferCount(&mPorts[OUTPUT_PORT])) {
    return;
  }

  OstTraceFiltInst0(TRACE_DEBUG, "One buffer processed by dsp_port");

  pInputBuf   = Port_dequeueBuffer(&mPorts[INPUT_PORT]);
  pOutputBuf  = Port_dequeueBuffer(&mPorts[OUTPUT_PORT]);
  PRECONDITION(pInputBuf->filledLen <= pOutputBuf->allocLen);

  data_in = pInputBuf->data;
  data_out = pOutputBuf->data;

  for (i=0;i< pInputBuf->filledLen;i++)
  {
    *data_out++ = ((*data_in++)<<8);
  }

  pOutputBuf->filledLen       = pInputBuf->filledLen ;
  pOutputBuf->byteInLastWord  = pInputBuf->byteInLastWord;
  pOutputBuf->flags           = pInputBuf->flags;
  pOutputBuf->nTimeStamph     = pInputBuf->nTimeStamph;
  pOutputBuf->nTimeStampl     = pInputBuf->nTimeStampl;

  OstTraceFiltInst3 (TRACE_DEBUG, "Speech_proc/dsp_port: latency control (dsp_port, output) (timestamp = 0x%x 0x%x 0x%x us)", pOutputBuf->nTimeStamph, (unsigned int)(pOutputBuf->nTimeStampl >> 24), (unsigned int)(pOutputBuf->nTimeStampl & 0xffffffu));
  Port_returnBuffer(&mPorts[OUTPUT_PORT], pOutputBuf);

  if (pInputBuf->flags & BUFFERFLAG_EOS){
    proxy.eventHandler(OMX_EventBufferFlag, 1, pInputBuf->flags);
  }

  Port_returnBuffer(&mPorts[INPUT_PORT], pInputBuf);
}


static void reset(Component *this)
{
}

static void disablePortIndication(t_uint32 portIdx)
{
}

static void enablePortIndication(t_uint32 portIdx)
{
  propagateNewFormatIfNeeded();
}

static void flushPortIndication(t_uint32 portIdx)
{
}


////////////////////////////////////////////////////////////
// Provided Interfaces
////////////////////////////////////////////////////////////
void METH(setParameter)(t_uint24 shared_buffer_header)
{
  mFifoIn[0] = (void *)shared_buffer_header;
}

void METH(fsmInit)(fsmInit_t initFsm) {

  // trace init (mandatory before trace init)
  FSM_traceInit(&mComponent, initFsm.traceInfoAddr, initFsm.id1);

  Port_init(&mPorts[INPUT_PORT], InputPort, true, false, 0, &mFifoIn, 1, &inputport, INPUT_PORT,
            (initFsm.portsDisabled & (1 << (INPUT_PORT))), (initFsm.portsTunneled & (1 << (INPUT_PORT))), &mComponent);
  Port_init(&mPorts[OUTPUT_PORT], OutputPort, false, false, 0, &mFifoOut, 1, &outputport, OUTPUT_PORT,
            (initFsm.portsDisabled & (1 << (OUTPUT_PORT))), (initFsm.portsTunneled & (1 << (OUTPUT_PORT))), &mComponent);

  mComponent.process               = process;
  mComponent.reset                 = reset;
  mComponent.disablePortIndication = disablePortIndication;
  mComponent.enablePortIndication  = enablePortIndication;
  mComponent.flushPortIndication   = flushPortIndication;

  Component_init(&mComponent, 2, mPorts, &proxy);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
  Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(processEvent)() {
  Component_processEvent(&mComponent);
}

void METH(emptyThisBuffer)(Buffer_p buf)
{
  TRACE_t * this = (TRACE_t *)&mComponent;
  OstTraceFiltInst0(TRACE_FLOW, "Empty This buffer received in dsp_port");
  Component_deliverBuffer(&mComponent, INPUT_PORT, buf);
}

void METH(fillThisBuffer)(Buffer_p buf)
{
  TRACE_t * this = (TRACE_t *)&mComponent;
  OstTraceFiltInst0(TRACE_FLOW, "Fill This buffer received in dsp_port");
  Component_deliverBuffer(&mComponent, OUTPUT_PORT, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    TRACE_t * this = (TRACE_t *)&mComponent;
    OstTraceFiltInst0(TRACE_DEBUG, "Processing sendCommand received in dsp_port");

  Component_sendCommand(&mComponent, cmd, param);

  if (cmd == OMX_CommandStateSet)
  {
    if((OMX_STATETYPE)param == OMX_StateExecuting)
    {
      mInExecutingState = 1;
      propagateNewFormatIfNeeded();
    }
    else
    {
        if((OMX_STATETYPE)param == OMX_StateIdle) {
            OstTraceFiltInst0(TRACE_DEBUG, "OMX_CommandStateSet OMX_StateIdle received in dsp_port");
        }


      mInExecutingState = 0;
    }
  }
  else if (cmd == OMX_CommandPortDisable) {
      OstTraceFiltInst0(TRACE_DEBUG, "OMX_CommandPortDisable received in dsp_port");
      mIsDisabled = 1;
  }
    else if (cmd == OMX_CommandPortEnable) {
      OstTraceFiltInst0(TRACE_DEBUG, "OMX_CommandPortEnable received in dsp_port");
      mIsDisabled = 0;
  }
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize)
{
  PRECONDITION(sample_bitsize == 16);
  mSampleFreq = freq;
  mChansNb    = chans_nb;

  mSendNewFormat = 1;
  propagateNewFormatIfNeeded();
}


////////////////////////////////////////////////////////////
// Helper function
////////////////////////////////////////////////////////////
void propagateNewFormatIfNeeded(void)
{
  if((mSendNewFormat) && (mInExecutingState) && (mPorts[OUTPUT_PORT].bufferQueue.enabled))
  {
    outputsettings.newFormat(mSampleFreq, mChansNb, 24);
    mSendNewFormat = 0;
  }
}
