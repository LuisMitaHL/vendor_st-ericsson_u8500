/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     dl_wrapper.hpp
 * \brief    NMF wrapper for downlink algorithms
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef _speech_proc_nmf_host_dl_wrapper_hpp_
#define _speech_proc_nmf_host_dl_wrapper_hpp_

#include "Component.h"
#include "speech_proc_interface.h"
#include "speech_proc_config.h"

#define NB_MAX_BUFFER 5

class speech_proc_nmf_host_dl_wrapper : public Component, public speech_proc_nmf_host_dl_wrapperTemplate
{
public:
  //Component virtual functions
  virtual void process();
  virtual void reset();
  virtual void disablePortIndication(t_uint32 portIdx);
  virtual void enablePortIndication(t_uint32 portIdx);
  virtual void flushPortIndication(t_uint32 portIdx);

  // Component interface
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
  virtual void  processEvent(void) {Component::processEvent();}

  // armnmf_fillthisbuffer
  virtual void  fillThisBuffer  (OMX_BUFFERHEADERTYPE_p buffer);

  // armnmf_emptythisbuffer
  virtual void  emptyThisBuffer (OMX_BUFFERHEADERTYPE_p buffer);

  // hst.common.pcmsettings
  virtual void  newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size);

  // speech_proc.nmf.host.common.configure
  virtual void  initLib(t_uint16 bypass);
  virtual void  configure(t_uint16 freq_in, t_uint16 freq_out, t_uint16 nb_channel_main, t_uint16 nb_channel_ref, t_uint16 port_enable);
  virtual void  setParameter(t_uint32 index, void *config_struct);
  virtual void  setConfig(t_uint32 index, void *config_struct);

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

  Port                   mPorts[2];
  void                  *mProcessingCtx;
  speech_proc_algo_t     mAlgo;
  t_sample_freq          mSampleFreqIn;
  t_sample_freq          mSampleFreqOut;
  unsigned short         mInputChannelSampleSize;  //size in samples per channel
  unsigned short         mOutputChannelSampleSize; //size in samples per channel
  t_sint16              *mInputBuffer;
  t_sint16              *mOutputBuffer;
  bool                   mInExecutingState;
  bool                   mIsConfigured;
  bool                   mGetSidetoneGain;
  void                  *mInfoPtr[NB_INFO_EXCHANGE];
  t_uint16               mNextInfoWrite;
  t_uint16               mNextInfoRead;
  bool                   mInfoFifoFull;
};

#endif //_speech_proc_nmf_host_ul_wrapper_hpp_
