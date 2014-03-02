/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <tonegen/nmfil/host/wrapper.nmf>
#include <string.h>
#include <verbose.h>
#include <armnmf_dbc.h>

//#define PCMPROC_TRACE_CONTROL

#ifdef PCMPROC_TRACE_CONTROL
#include <stdio.h>
#endif


t_nmf_error METH(construct)(void)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::construct()\n");
#endif
  return NMF_OK;
}

void METH(destroy)(void)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::destroy()\n");
  printf("\tmCount = %d\n", mCount);
#endif
}

void METH(start)(void)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::start()\n");
#endif
  mResetCount = 0;
  mStartTime = true;
  mTimeStamp = 0;
  mCount = 0;
}

void METH(stop)(void)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::stop()\n");
#endif
  // Close the effect...
  source.close();
}


void METH(fsmInit)(fsmInit_t initFsm)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::fsmInit()\n");
#endif

  // trace init (mandatory before port init)
  if (initFsm.traceInfoAddr){
    setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
  }

  mPort.init(OutputPort, false, false, 0, 0, 1, &outputport, OUTPUT_PORT_IDX,
          (initFsm.portsDisabled & (1 << (OUTPUT_PORT_IDX)))!=0,
          (initFsm.portsTunneled & (1 << (OUTPUT_PORT_IDX)))!=0, this);

  init(1, &mPort, &proxy, &me, false);

  // Open source...
  bool result = source.open(&m_source_config);
  // TODO: Investigate what to do if open returns with failure!
  // fsmInit should have return value to block transition to Idle in case of failure!?
  ARMNMF_DBC_ASSERT(result);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::setTunnelStatus()\n");
#endif
    ARMNMF_DBC_ASSERT(0);
    mPort.setTunnelStatus(isTunneled);
}


void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)
{
  Component::deliverBuffer(OUTPUT_PORT_IDX, buffer);
}

// Interface for fsm
// reset is called when entering Idle state, inside Component::init called from fsmInit
void METH(reset)()
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::reset()\n");
#endif

  if (mResetCount++ > 0) {
    // If NOT first reset (Init -> Idle transition) then reset effect...
    source.reset(SOURCE_RESET_REASON_STOP);
  }
}

void METH(disablePortIndication)(t_uint32 portIdx)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::disablePortIndication()\n");
#endif

  // TODO: Implement
  ARMNMF_DBC_ASSERT(0);
}

void METH(enablePortIndication)(t_uint32 portIdx)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::disablePortIndication()\n");
#endif

  // TODO: Implement
  ARMNMF_DBC_ASSERT(0);
}

void METH(flushPortIndication)(t_uint32 portIdx)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::flushPortIndication()\n");
#endif

  source.reset(SOURCE_RESET_REASON_FLUSH);
  // TODO: Check if any pending buffers, and that in case return them
}


void METH(process)()
{
  t_source_process_params params;
  OMX_BUFFERHEADERTYPE *omxBufHdr = NULL;

  // If no buffers on port then return...
  if(mPort.queuedBufferCount() == 0) {
    return;
  }

  // Point to out-buffer (do not de-queue until processed)...
  omxBufHdr = mPort.getBuffer(0);
  params.buf_hdr = omxBufHdr;

  // process
  source.process(&params);

  omxBufHdr->nFilledLen = omxBufHdr->nAllocLen;
  omxBufHdr->nOffset = 0;
  omxBufHdr->nOutputPortIndex = 0;
  omxBufHdr->nTimeStamp = mTimeStamp;
  omxBufHdr->nFlags = 0;

  // Time stamp in microseconds
  mTimeStamp += 1000000 * omxBufHdr->nFilledLen /
                (m_source_config.freq * m_source_config.nof_channels *
                 m_source_config.nof_bits_per_sample / 8);

  if (mStartTime) {
    omxBufHdr->nFlags |= OMX_BUFFERFLAG_STARTTIME;
    mStartTime = false;
  }

  mPort.dequeueAndReturnBuffer();
  mCount++;
}


void METH(setParameter)(t_tonegen_wrapper_config config)
{
#ifdef PCMPROC_TRACE_CONTROL
  printf("tonegen_nmfil_host_wrapper::setParameter()\n");
#endif

  // Store input format...
  m_source_config.freq = config.freq;
  m_source_config.nof_channels = config.nof_channels;
  m_source_config.nof_bits_per_sample = config.nof_bits_per_sample;
}
