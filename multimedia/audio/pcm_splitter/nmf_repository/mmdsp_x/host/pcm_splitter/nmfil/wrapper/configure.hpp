/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* 'pcm_splitter.nmfil.wrapper.configure' interface */
#if !defined(pcm_splitter_nmfil_wrapper_configure_IDL)
#define pcm_splitter_nmfil_wrapper_configure_IDL

#include <cpp.hpp>
#include <host/bool.idt.h>
#include <host/samplefreq.idt.h>

#define NB_OUTPUT 2

#define NMF_AUDIO_MAXCHANNELS 4

typedef struct t_xyuv_OutputPortParam_t {
  t_sample_freq nSamplingRate;
  t_uint16 nChannels;
  t_uint16 nBlockSize;
  void* pHeap;
  t_uint16 nHeapSize;
  t_uint16 nChannelSwitchBoard[NMF_AUDIO_MAXCHANNELS];
  BOOL bPaused;
  BOOL bMute;
  t_sint16 nBalance;
  t_sint16 nVolume;
  t_uint16 nRampChannels;
  t_sint16 nRampEndVolume;
  t_uint24 nRampDuration;
  BOOL bRampTerminate;
} OutputPortParam_t;

typedef struct t_xyuv_InputPortParam_t {
  t_uint16 nBlockSize;
  t_sample_freq nSamplingRate;
  t_uint16 nChannels;
} InputPortParam_t;

typedef struct t_xyuv_SplitterParam_t {
  InputPortParam_t inputPortParam;
  OutputPortParam_t outputPortParam[NB_OUTPUT];
  t_uint32 traceAddr;
} SplitterParam_t;


class pcm_splitter_nmfil_wrapper_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(SplitterParam_t splitterParam) = 0;
    virtual void setInputPortParameter(InputPortParam_t inputPortParam) = 0;
    virtual void setOutputPortParameter(t_uint16 idx, OutputPortParam_t outputPortParam) = 0;
    virtual void setChannelSwitchBoardOutputPort(t_uint16 portIdx, t_uint16 channelSwitchBoard[NMF_AUDIO_MAXCHANNELS]) = 0;
    virtual void setPausedOutputPort(t_uint16 portIdx, BOOL bIsPaused) = 0;
    virtual void setMuteOutputPort(t_uint16 portIdx, BOOL bIsPaused) = 0;
    virtual void setBalanceOutputPort(t_uint16 portIdx, t_sint16 nBalance) = 0;
    virtual void setVolumeOutputPort(t_uint16 portIdx, t_sint16 nVolume) = 0;
    virtual void setVolumeRampOutputPort(t_uint16 portIdx, t_sint16 nStartVolume, t_sint16 nEndVolume, t_uint16 nChannels, t_uint24 nDuration, BOOL bTerminate) = 0;
};

class Ipcm_splitter_nmfil_wrapper_configure: public NMF::InterfaceReference {
  public:
    Ipcm_splitter_nmfil_wrapper_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(SplitterParam_t splitterParam) {
      ((pcm_splitter_nmfil_wrapper_configureDescriptor*)reference)->setParameter(splitterParam);
    }
    void setInputPortParameter(InputPortParam_t inputPortParam) {
      ((pcm_splitter_nmfil_wrapper_configureDescriptor*)reference)->setInputPortParameter(inputPortParam);
    }
    void setOutputPortParameter(t_uint16 idx, OutputPortParam_t outputPortParam) {
      ((pcm_splitter_nmfil_wrapper_configureDescriptor*)reference)->setOutputPortParameter(idx, outputPortParam);
    }
    void setChannelSwitchBoardOutputPort(t_uint16 portIdx, t_uint16 channelSwitchBoard[NMF_AUDIO_MAXCHANNELS]) {
      ((pcm_splitter_nmfil_wrapper_configureDescriptor*)reference)->setChannelSwitchBoardOutputPort(portIdx, channelSwitchBoard);
    }
    void setPausedOutputPort(t_uint16 portIdx, BOOL bIsPaused) {
      ((pcm_splitter_nmfil_wrapper_configureDescriptor*)reference)->setPausedOutputPort(portIdx, bIsPaused);
    }
    void setMuteOutputPort(t_uint16 portIdx, BOOL bIsPaused) {
      ((pcm_splitter_nmfil_wrapper_configureDescriptor*)reference)->setMuteOutputPort(portIdx, bIsPaused);
    }
    void setBalanceOutputPort(t_uint16 portIdx, t_sint16 nBalance) {
      ((pcm_splitter_nmfil_wrapper_configureDescriptor*)reference)->setBalanceOutputPort(portIdx, nBalance);
    }
    void setVolumeOutputPort(t_uint16 portIdx, t_sint16 nVolume) {
      ((pcm_splitter_nmfil_wrapper_configureDescriptor*)reference)->setVolumeOutputPort(portIdx, nVolume);
    }
    void setVolumeRampOutputPort(t_uint16 portIdx, t_sint16 nStartVolume, t_sint16 nEndVolume, t_uint16 nChannels, t_uint24 nDuration, BOOL bTerminate) {
      ((pcm_splitter_nmfil_wrapper_configureDescriptor*)reference)->setVolumeRampOutputPort(portIdx, nStartVolume, nEndVolume, nChannels, nDuration, bTerminate);
    }
};

#endif
