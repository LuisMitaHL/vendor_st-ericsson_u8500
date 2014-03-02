/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     time_align.hpp
 * \brief    NMF time alignment header file
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef _speech_proc_nmf_host_time_align_hpp_
#define _speech_proc_nmf_host_time_align_hpp_

#include "Component.h"
#include "time_align_common.h"
#include "resample.h"


#define SAMPLE2BYTE(a) ((a)<<2) // samples are 32bits
#define BYTE2SAMPLE(a) ((a)>>2)

class speech_proc_nmf_host_time_align : public Component, public speech_proc_nmf_host_time_alignTemplate
{
public:
  //Component virtual functions
  virtual void process();
  virtual void reset();
  virtual void disablePortIndication(t_uint32 portIdx);
  virtual void enablePortIndication(t_uint32 portIdx);
  virtual void flushPortIndication(t_uint32 portIdx);

  // provided interfaces
  // lifecycle.starter
  virtual void  start(void);

  // lifecycle.stopper
  virtual void  stop(void);

  // fsminit
  virtual void  fsmInit(fsmInit_t initFsm);
  virtual void  setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);

  // sendcommand
  virtual void  sendCommand(OMX_COMMANDTYPE cmd, t_uword param);

  // postevent
  virtual void  processEvent(void) { Component::processEvent(); }

  // armnmf_fillthisbuffer
  virtual void  fillThisBuffer (OMX_BUFFERHEADERTYPE_p buffer) { ((OMX_BUFFERHEADERTYPE *)buffer)->nOffset = 0;
                                                                 Component::deliverBuffer(OUTPUT_PORT,  buffer); }
  // armnmf_emptythisbuffer
  virtual void  emptyThisBuffer (OMX_BUFFERHEADERTYPE_p buffer, t_uint8 idx);

  // hst.common.pcmsettings
  virtual void  newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size, t_uint8 idx);

  // speech_proc.nmf.common.configure_time_align
  virtual void  setParameter(t_uint24 shared_buffer_header, t_uint24 ring_buffer, t_uint24 heap, t_uint24 heap_size, t_uint16 real_time_ref);
  virtual void  setConfigRef(t_uint16 in_freq, t_uint16 out_freq, t_uint16 nb_channel_ref);
  virtual void  setConfig(t_uint16 freq, t_uint16 nb_channel);

private:
  void  reconfigureSrc(void);
  bool  processRefBuffer(OMX_BUFFERHEADERTYPE *buffer);
  bool  referenceAvailable(void);
  void  packOutputBuffer(OMX_BUFFERHEADERTYPE *pInputBuf, OMX_BUFFERHEADERTYPE *pOutputBuf);
  void  readFromRingBuffer(t_sint16 *dst, t_uint32 size, OMX_TICKS timestamp);
  void  completeOutputBufferWithZeroPadding(t_sint16 * ptr, t_uint16 offset);
  void  resetReferencePort(void);
  void  resetTimeAlignment(void);
  void  resetInputPort(void);
  void  resetRingBuffer(void);
  void  ringModuloWrite(t_sint32 *src, t_uint16 nb_sample);
  void  ringModuloRead(t_sint16 *dst, t_uint16 nb_sample);
  void  ringModuloSet(t_uint16 data, t_uint16 nb_sample);
  void  adaptRingUsedSize(t_uint16 delta);

  // ports
  Port                   mPorts[3];

  // src context
  ResampleContext        mResampleContext;
  char                  *mHeap;
  int                    mHeapSize;

  // uplink port
  bool                   mIsConfigured;
  t_sample_freq          mSampleFreq;
  t_uint16               mChansNb;
  t_uint16               mOutputPart;

  // Reference input port
  bool                   mIsRefConfigured;
  t_sample_freq          mRefSampleFreq;
  t_uint16               mRefChansNb;
  bool                   mRealTimeReference;
  t_ref_status           mRefStatus;
  OMX_TICKS              mTimeStamp;     // timestamp of the last sample received on reference port in us. (i.e. time stamp of the last buffer + buffer duration)
  t_align_status         mAlignmentStatus;

  // Ring Buffer (32 bits samples as SRC lib works on 32bits)
  t_sint32               mRingBuffer[RING_BUFFER_SIZE];
  t_uint32               mRingBufferEnd;
  t_sint32              *mRingWritePtr;
  t_sint32              *mRingReadPtr;
  t_uint32               mRingUsedSize; // in samples
  t_uint32               mRingFreeSize; // in samples

  // buffer sizes
  t_uint16               mSingleInputBufferSize; // in samples
  t_uint16               mOutputBufferSize;      // in bytes

  //output buffer
  OMX_BUFFERHEADERTYPE   mOutputBufferHeader;
  t_uint16               mOutputBuffer[OUTPUT_BUFFER_SIZE];
};



#endif //_speech_proc_nmf_host_time_align_hpp_

