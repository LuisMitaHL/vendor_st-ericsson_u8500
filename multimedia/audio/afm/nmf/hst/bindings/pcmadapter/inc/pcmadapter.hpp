/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   pcmadapter.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _pcmadapter_hpp_
#define _pcmadapter_hpp_

#include "Component.h"
#include "common_interface.h"


class hst_bindings_pcmadapter : public Component, public hst_bindings_pcmadapterTemplate
{
private:
  typedef enum {INPUT_PORT, OUTPUT_PORT} portname;

public:
  //Component virtual functions
  virtual void process() ;
  virtual void reset() ;
  virtual void disablePortIndication(t_uint32 portIdx) ;
  virtual void enablePortIndication(t_uint32 portIdx) ;
  virtual void flushPortIndication(t_uint32 portIdx) ;

  virtual void fsmInit(fsmInit_t init);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
  virtual void processEvent(void)  			       { Component::processEvent() ; }
  virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
  virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)  { Component::deliverBuffer(OUTPUT_PORT, buffer); }

  virtual void setParameter(HostPcmAdapterParam_t params);

  virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size);

private:
  void stereo2mono_16(OMX_BUFFERHEADERTYPE * buf);
  void stereo2mono_32(OMX_BUFFERHEADERTYPE * buf);
  void mono2stereo_16(OMX_BUFFERHEADERTYPE * buf);
  void mono2stereo_32(OMX_BUFFERHEADERTYPE * buf);
  void c16to32(OMX_BUFFERHEADERTYPE * buf);
  void c32to16(OMX_BUFFERHEADERTYPE * buf);
  void move_chunk();
  void propagate_timestamp();
  void handleNewInputBuffer();
  void handleNewOutputBuffer();
  t_uint16 computeBufferSize(HostPcmAdapterParam_t params);
  OMX_S64  computeTimeStamp(t_uint16 nbChannels, OMX_S64 *nbSamples, t_uint16 sampleSize);
  void initTimeStampComputation();
  void returnOutputBuffer();
  int gcd(int a, int b);

   Port         mPorts[2];

   OMX_BUFFERHEADERTYPE     mBufIn;
   OMX_BUFFERHEADERTYPE     mBufOut;

   OMX_U8  *       mBuffer;
   t_uint16     mBufferSize;

   OMX_BUFFERHEADERTYPE *     mInputBuf, *mOutputBuf;
   OMX_U8          *mReadPtr, *mWritePtr;

   t_uint16     mInputSize;
   t_uint16     mChannelsIn;
   t_uint16     mChannelsOut;
   t_uint16     mBitPerSampleIn;
   t_uint16     mBitPerSampleOut;
   t_uint32     mSampleFreq;
   t_uint32     mSizeSentToOutput;

   bool         mSetStartTime;
   OMX_S64      mInitialTS;  
   OMX_S64      mNbSamplesForTSComputationInput;
   OMX_S64      mNbSamplesForTSComputationOutput;
   bool         bPropagateTS;

};

#endif // _pcmadapter_hpp_
