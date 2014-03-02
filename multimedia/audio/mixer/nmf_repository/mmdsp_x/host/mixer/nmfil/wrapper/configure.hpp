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

/* 'mixer.nmfil.wrapper.configure' interface */
#if !defined(mixer_nmfil_wrapper_configure_IDL)
#define mixer_nmfil_wrapper_configure_IDL

#include <cpp.hpp>
#include <host/bool.idt.h>
#include <host/samplefreq.idt.h>
#include <host/memorybank.idt.h>
#include <host/mixer/nmfil/common/mixer_defines.idt.h>

#define INPUT_UPMIX_POSITION 65534

#define INPUT_DOWNMIX_POSITION 65532

#define INPUT_VOLCTRL_POSITION 65535

#define INPUT_SRC_POSITION 65533

typedef struct t_xyuv_InputPortParam_t {
  t_sample_freq nSamplingRate;
  t_uint16 nChannels;
  t_uint16 nBlockSize;
  t_memory_bank nMemoryBank;
  BOOL bDisabled;
  BOOL bPaused;
} InputPortParam_t;

typedef struct t_xyuv_OutputPortParam_t {
  t_uint16 nBlockSize;
  t_sample_freq nSamplingRate;
  t_uint16 nChannels;
  BOOL bDisabled;
} OutputPortParam_t;

typedef struct t_xyuv_MixerParam_t {
  InputPortParam_t inputPortParam[MIXER_INPUT_PORT_COUNT];
  OutputPortParam_t outputPortParam;
  t_uint32 traceAddr;
} MixerParam_t;

typedef struct t_xyuv_MixerConfig_t {
  t_uint24 nbConfigs;
  void* pEffectConfigTable;
  t_uint48 configStructArmAddress;
} MixerConfig_t;

typedef struct t_xyuv_MixerEffectConfig_t {
  t_uint24 portIdx;
  t_uint24 effectPosition;
  t_uint24 configIndex;
  t_uint24 isStaticConfiguration;
  void* pSpecificConfigStruct;
  t_uint24 dummyfield;
  t_uint48 specificConfigStructHandle;
} MixerEffectConfig_t;


class mixer_nmfil_wrapper_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(MixerParam_t mixerParam) = 0;
    virtual void setInputPortParameter(t_uint16 portIdx, InputPortParam_t inputPortParam) = 0;
    virtual void configureEffects(t_uint32 paramDspAddress) = 0;
    virtual void setPausedInputPort(t_uint16 portIdx, BOOL bIsPaused) = 0;
    virtual void setSamplesPlayedCounter(t_uint16 portIdx, t_uint32 value) = 0;
    virtual void setEffectGetInfoPtr(t_uint16 portIdx, t_uint16 positionId, void* ptr) = 0;
};

class Imixer_nmfil_wrapper_configure: public NMF::InterfaceReference {
  public:
    Imixer_nmfil_wrapper_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(MixerParam_t mixerParam) {
      ((mixer_nmfil_wrapper_configureDescriptor*)reference)->setParameter(mixerParam);
    }
    void setInputPortParameter(t_uint16 portIdx, InputPortParam_t inputPortParam) {
      ((mixer_nmfil_wrapper_configureDescriptor*)reference)->setInputPortParameter(portIdx, inputPortParam);
    }
    void configureEffects(t_uint32 paramDspAddress) {
      ((mixer_nmfil_wrapper_configureDescriptor*)reference)->configureEffects(paramDspAddress);
    }
    void setPausedInputPort(t_uint16 portIdx, BOOL bIsPaused) {
      ((mixer_nmfil_wrapper_configureDescriptor*)reference)->setPausedInputPort(portIdx, bIsPaused);
    }
    void setSamplesPlayedCounter(t_uint16 portIdx, t_uint32 value) {
      ((mixer_nmfil_wrapper_configureDescriptor*)reference)->setSamplesPlayedCounter(portIdx, value);
    }
    void setEffectGetInfoPtr(t_uint16 portIdx, t_uint16 positionId, void* ptr) {
      ((mixer_nmfil_wrapper_configureDescriptor*)reference)->setEffectGetInfoPtr(portIdx, positionId, ptr);
    }
};

#endif
