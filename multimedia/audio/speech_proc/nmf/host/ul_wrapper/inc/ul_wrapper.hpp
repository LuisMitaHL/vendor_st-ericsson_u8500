/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     ul_wrapper.hpp
 * \brief    NMF wrapper for uplink algorithms
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef _speech_proc_nmf_host_ul_wrapper_hpp_
#define _speech_proc_nmf_host_ul_wrapper_hpp_

#include "Component.h"
#include "speech_proc_interface.h"
#include "speech_proc_config.h"

#define MAX_NB_CHANNEL (MAX_NB_REF_CHANNEL + MAX_NB_INPUT_CHANNEL)

class speech_proc_nmf_host_ul_wrapper : public Component, public speech_proc_nmf_host_ul_wrapperTemplate
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
  virtual void  fillThisBuffer (OMX_BUFFERHEADERTYPE_p buffer);

  // armnmf_emptythisbuffer
  virtual void  emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) { Component::deliverBuffer(INPUT_PORT,  buffer); }

  // speech_proc.nmf.host.common.configure
  virtual void  initLib(t_uint16 bypass);
  virtual void  configure(t_uint16 freq_in, t_uint16 freq_out, t_uint16 nb_channel_main,
                          t_uint16 nb_channel_ref, t_uint16 port_enable);
  virtual void  setConfig(t_uint32 index, void *config_struct);
  virtual void  setParameter(t_uint32 index, void *config_struct);

  // speech_proc.nmf.host.common.processing_info
  virtual void  resetProcessingInfo(void);
  virtual t_bool setProcessingInfo(void *info);

  // speech_proc.nmf.host.common.ack_processing_info
  virtual void  ackProcessingInfo(void);

private:
  typedef enum {OUTPUT_PORT, INPUT_PORT} portname;
  void	traceInputBuffer(OMX_BUFFERHEADERTYPE *input);
  void	returnInputBuffer(OMX_BUFFERHEADERTYPE *input);
  void	returnOutputBuffer(OMX_BUFFERHEADERTYPE *output, OMX_U32 flags, OMX_TICKS timestamp);
  void  copyRefernceToOutput(OMX_U8* input, OMX_U32 input_offset, OMX_U8 *output);

  Port					 mPorts[2];
  void                  *mProcessingCtx;
  speech_proc_algo_t     mAlgo;
  bool					 mInExecutingState;
  t_uint32				 mNbFrame;
  t_uint16               mChansNb;
  t_uint16               mRefChansNb;
  t_sample_freq          mSampleFreqIn;
  t_sample_freq          mSampleFreqOut;
  unsigned short         mInputChannelSampleSize;		//size in samples per channel
  unsigned short         mOutputChannelSampleSize;		//size in samples per channel
  t_sint16              *mInputBuffers[MAX_NB_CHANNEL];
  t_sint16              *mOutputBuffer;
  bool                   mIsConfigured;
  bool                   mGetSidetoneGain;
  bool                   mOutputReference;
  void                  *mInfoPtr[NB_INFO_EXCHANGE];
  t_uint16               mNextInfoWrite;
  t_uint16               mNextInfoRead;
  bool                   mInfoFifoFull;

};



#endif //_speech_proc_nmf_host_ul_wrapper_hpp_
