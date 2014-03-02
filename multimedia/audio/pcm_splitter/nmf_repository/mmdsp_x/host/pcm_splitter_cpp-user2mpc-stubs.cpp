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

#include <cm/inc/cm.h>
#include <cm/proxy/api/private/stub-requiredapi.h>

/* Generated C++ stub for 'pcm_splitter.nmfil.wrapper.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/pcm_splitter/nmfil/wrapper/configure.hpp>

class pcm_splitter_nmfil_wrapper_configure: public pcm_splitter_nmfil_wrapper_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(SplitterParam_t splitterParam) {
      t_uint16 _xyuv_data[46];
      int ___i0, ___j0;
      int ___i1, ___j1;
      /* splitterParam <SplitterParam_t> marshalling */
      _xyuv_data[0+0+0] = (t_uint16)splitterParam.inputPortParam.nBlockSize;
      _xyuv_data[0+0+1] = (t_uint16)((unsigned int)splitterParam.inputPortParam.nSamplingRate & 0xFFFFU);
      _xyuv_data[0+0+1+1] = (t_uint16)((unsigned int)splitterParam.inputPortParam.nSamplingRate >> 16);
      _xyuv_data[0+0+3] = (t_uint16)splitterParam.inputPortParam.nChannels;
      for(___i0 = ___j0 = 0; ___i0 < 2; ___i0++) {
        _xyuv_data[0+4+___j0+0] = (t_uint16)((unsigned int)splitterParam.outputPortParam[___i0].nSamplingRate & 0xFFFFU);
        _xyuv_data[0+4+___j0+0+1] = (t_uint16)((unsigned int)splitterParam.outputPortParam[___i0].nSamplingRate >> 16);
        _xyuv_data[0+4+___j0+2] = (t_uint16)splitterParam.outputPortParam[___i0].nChannels;
        _xyuv_data[0+4+___j0+3] = (t_uint16)splitterParam.outputPortParam[___i0].nBlockSize;
        _xyuv_data[0+4+___j0+4] = (t_uint16)((unsigned int)splitterParam.outputPortParam[___i0].pHeap & 0xFFFFU);
        _xyuv_data[0+4+___j0+4+1] = (t_uint16)((unsigned int)splitterParam.outputPortParam[___i0].pHeap >> 16);
        _xyuv_data[0+4+___j0+6] = (t_uint16)splitterParam.outputPortParam[___i0].nHeapSize;
        for(___i1 = ___j1 = 0; ___i1 < 4; ___i1++) {
          _xyuv_data[0+4+___j0+7+___j1] = (t_uint16)splitterParam.outputPortParam[___i0].nChannelSwitchBoard[___i1];
          ___j1 += 1;
        }
        _xyuv_data[0+4+___j0+11] = (t_uint16)splitterParam.outputPortParam[___i0].bPaused;
        _xyuv_data[0+4+___j0+12] = (t_uint16)splitterParam.outputPortParam[___i0].bMute;
        _xyuv_data[0+4+___j0+13] = (t_uint16)splitterParam.outputPortParam[___i0].nBalance;
        _xyuv_data[0+4+___j0+14] = (t_uint16)splitterParam.outputPortParam[___i0].nVolume;
        _xyuv_data[0+4+___j0+15] = (t_uint16)splitterParam.outputPortParam[___i0].nRampChannels;
        _xyuv_data[0+4+___j0+16] = (t_uint16)splitterParam.outputPortParam[___i0].nRampEndVolume;
        _xyuv_data[0+4+___j0+17] = (t_uint16)splitterParam.outputPortParam[___i0].nRampDuration;
        _xyuv_data[0+4+___j0+17+1] = (t_uint16)(splitterParam.outputPortParam[___i0].nRampDuration >> 16);
        _xyuv_data[0+4+___j0+19] = (t_uint16)splitterParam.outputPortParam[___i0].bRampTerminate;
        ___j0 += 20;
      }
      _xyuv_data[0+44] = (t_uint16)splitterParam.traceAddr;
      _xyuv_data[0+44+1] = (t_uint16)(splitterParam.traceAddr >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 46*2, 0);
    }
    virtual void setInputPortParameter(InputPortParam_t inputPortParam) {
      t_uint16 _xyuv_data[4];
      /* inputPortParam <InputPortParam_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)inputPortParam.nBlockSize;
      _xyuv_data[0+1] = (t_uint16)((unsigned int)inputPortParam.nSamplingRate & 0xFFFFU);
      _xyuv_data[0+1+1] = (t_uint16)((unsigned int)inputPortParam.nSamplingRate >> 16);
      _xyuv_data[0+3] = (t_uint16)inputPortParam.nChannels;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 1);
    }
    virtual void setOutputPortParameter(t_uint16 idx, OutputPortParam_t outputPortParam) {
      t_uint16 _xyuv_data[21];
      int ___i0, ___j0;
      /* idx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)idx;
      /* outputPortParam <OutputPortParam_t> marshalling */
      _xyuv_data[1+0] = (t_uint16)((unsigned int)outputPortParam.nSamplingRate & 0xFFFFU);
      _xyuv_data[1+0+1] = (t_uint16)((unsigned int)outputPortParam.nSamplingRate >> 16);
      _xyuv_data[1+2] = (t_uint16)outputPortParam.nChannels;
      _xyuv_data[1+3] = (t_uint16)outputPortParam.nBlockSize;
      _xyuv_data[1+4] = (t_uint16)((unsigned int)outputPortParam.pHeap & 0xFFFFU);
      _xyuv_data[1+4+1] = (t_uint16)((unsigned int)outputPortParam.pHeap >> 16);
      _xyuv_data[1+6] = (t_uint16)outputPortParam.nHeapSize;
      for(___i0 = ___j0 = 0; ___i0 < 4; ___i0++) {
        _xyuv_data[1+7+___j0] = (t_uint16)outputPortParam.nChannelSwitchBoard[___i0];
        ___j0 += 1;
      }
      _xyuv_data[1+11] = (t_uint16)outputPortParam.bPaused;
      _xyuv_data[1+12] = (t_uint16)outputPortParam.bMute;
      _xyuv_data[1+13] = (t_uint16)outputPortParam.nBalance;
      _xyuv_data[1+14] = (t_uint16)outputPortParam.nVolume;
      _xyuv_data[1+15] = (t_uint16)outputPortParam.nRampChannels;
      _xyuv_data[1+16] = (t_uint16)outputPortParam.nRampEndVolume;
      _xyuv_data[1+17] = (t_uint16)outputPortParam.nRampDuration;
      _xyuv_data[1+17+1] = (t_uint16)(outputPortParam.nRampDuration >> 16);
      _xyuv_data[1+19] = (t_uint16)outputPortParam.bRampTerminate;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 21*2, 2);
    }
    virtual void setChannelSwitchBoardOutputPort(t_uint16 portIdx, t_uint16 channelSwitchBoard[NMF_AUDIO_MAXCHANNELS]) {
      t_uint16 _xyuv_data[5];
      int ___i0, ___j0;
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* channelSwitchBoard <t_uint16*> marshalling */
      for(___i0 = ___j0 = 0; ___i0 < 4; ___i0++) {
        _xyuv_data[1+___j0] = (t_uint16)channelSwitchBoard[___i0];
        ___j0 += 1;
      }
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 3);
    }
    virtual void setPausedOutputPort(t_uint16 portIdx, BOOL bIsPaused) {
      t_uint16 _xyuv_data[2];
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* bIsPaused <BOOL> marshalling */
      _xyuv_data[1] = (t_uint16)bIsPaused;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 4);
    }
    virtual void setMuteOutputPort(t_uint16 portIdx, BOOL bIsPaused) {
      t_uint16 _xyuv_data[2];
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* bIsPaused <BOOL> marshalling */
      _xyuv_data[1] = (t_uint16)bIsPaused;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 5);
    }
    virtual void setBalanceOutputPort(t_uint16 portIdx, t_sint16 nBalance) {
      t_uint16 _xyuv_data[2];
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* nBalance <t_sint16> marshalling */
      _xyuv_data[1] = (t_uint16)nBalance;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 6);
    }
    virtual void setVolumeOutputPort(t_uint16 portIdx, t_sint16 nVolume) {
      t_uint16 _xyuv_data[2];
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* nVolume <t_sint16> marshalling */
      _xyuv_data[1] = (t_uint16)nVolume;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 7);
    }
    virtual void setVolumeRampOutputPort(t_uint16 portIdx, t_sint16 nStartVolume, t_sint16 nEndVolume, t_uint16 nChannels, t_uint24 nDuration, BOOL bTerminate) {
      t_uint16 _xyuv_data[7];
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* nStartVolume <t_sint16> marshalling */
      _xyuv_data[1] = (t_uint16)nStartVolume;
      /* nEndVolume <t_sint16> marshalling */
      _xyuv_data[2] = (t_uint16)nEndVolume;
      /* nChannels <t_uint16> marshalling */
      _xyuv_data[3] = (t_uint16)nChannels;
      /* nDuration <t_uint24> marshalling */
      _xyuv_data[4] = (t_uint16)nDuration;
      _xyuv_data[4+1] = (t_uint16)(nDuration >> 16);
      /* bTerminate <BOOL> marshalling */
      _xyuv_data[6] = (t_uint16)bTerminate;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 7*2, 8);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_pcm_splitter_nmfil_wrapper_configure(t_nmf_interface_desc* itf) {
  pcm_splitter_nmfil_wrapper_configure *stub = new pcm_splitter_nmfil_wrapper_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function pcm_splitter_cpp_stub_functions[] = {
  {"pcm_splitter.nmfil.wrapper.configure", &INITstub_pcm_splitter_nmfil_wrapper_configure},
};

/*const */t_nmf_stub_register pcm_splitter_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(pcm_splitter_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    pcm_splitter_cpp_stub_functions
};
