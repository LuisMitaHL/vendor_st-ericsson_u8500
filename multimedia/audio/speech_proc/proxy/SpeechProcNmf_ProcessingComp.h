/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     SpeechProcNmf_ProcessingComp.h
 * \brief    NMF part of Speech Proc proxy
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef _SPEECHPROCNMFHOSTMPC_PROCESSINGCOMP_H_
#define _SPEECHPROCNMFHOSTMPC_PROCESSINGCOMP_H_

#ifdef USE_HYBRID_COMPONENT
#include "AFMNmfHostMpc_ProcessingComp.h"
#include "SpeechProcHybridBuffer.h"
#include "speech_proc/nmf/host/composite/speech_proc_hybrid.hpp"

#else
#include "AFMNmfHost_ProcessingComp.h"
#include "speech_proc/nmf/host/composite/speech_proc.hpp"
#endif

#include "SpeechProcComponent.h"

// must be in line with fixed_id.txt
#define TRACE_PCM_PORT_START_BIT 8
#define TRACE_PCM_PORT_MASK      0x0700
#define TRACE_PCM_LIB_START_BIT  11
#define TRACE_PCM_LIB_MASK       0xf800

#ifdef USE_HYBRID_COMPONENT
class SpeechProcNmf_ProcessingComp:public AFMNmfHostMpc_ProcessingComp,
								   public speech_proc_nmf_host_common_sidetoneDescriptor,
								   public speech_proc_nmf_host_common_contextDescriptor
#else
class SpeechProcNmf_ProcessingComp:public AFMNmfHost_ProcessingComp,
								   public speech_proc_nmf_host_common_sidetoneDescriptor,
								   public speech_proc_nmf_host_common_contextDescriptor
#endif
{
public:
  SpeechProcNmf_ProcessingComp(ENS_Component &enscomp);

  virtual OMX_ERRORTYPE construct();
  virtual OMX_ERRORTYPE destroy();
  virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure);
  virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);
  void setLibrary(SpeechProcLibrary *lib){mLibrary = lib;};

protected:

  virtual OMX_ERRORTYPE instantiateMain(void);
  virtual OMX_ERRORTYPE startMain(void);
  virtual OMX_ERRORTYPE configureMain(void);
  virtual OMX_ERRORTYPE stopMain(void);
  virtual OMX_ERRORTYPE deInstantiateMain(void);

  virtual const char *getNmfSuffix(OMX_U32 portidx) const;

  // inherited from speech_proc_nmf_host_common_sidetoneDescriptor
  virtual void newSidetoneGain(t_sint32 gain);

  // inherited from speech_proc_nmf_host_common_conextDescriptor
  virtual void newLibraryContext(void *ctx, t_uint16 algo, t_uint32 version);

private:
  bool realtimeReference(void) const {
	return ((static_cast<SpeechProcComponent *>(&mENSComponent))->realtimeReference());
  }
  bool downlinkBypass(void) const {
	return ((static_cast<SpeechProcComponent *>(&mENSComponent))->downlinkBypass());
  }
  bool uplinkBypass(void) const {
  	return ((static_cast<SpeechProcComponent *>(&mENSComponent))->uplinkBypass());
  }
  bool outputReference(void) const {
  	return ((static_cast<SpeechProcComponent *>(&mENSComponent))->outputReference());
  }

  Ispeech_proc_nmf_host_common_configure         mIconfigureDL;
  Ispeech_proc_nmf_host_common_configure         mIconfigureUL;
  Ispeech_proc_nmf_common_configure_time_align   mIconfigureTimealign;
  SpeechProcLibrary *                            mLibrary;
  OMX_PTR*                                       mConfigTable;
  OMX_U32*                                       mIndexTable;

#ifdef USE_HYBRID_COMPONENT
  // specific stuff hybrid component
protected :
  virtual t_cm_instance_handle getNmfMpcHandle(OMX_U32 portidx) const;

private :
  void cleanMpcMemory(void);

  Ispeech_proc_nmf_mpc_dsp_port_configure        mIconfigureDspPort;
  t_cm_memory_handle                             mMemorySampleRate;
  t_cm_memory_handle                             mMemoryTimeAlign;
  SpeechProcHybridBuffer *                       mDownlinkHybridbuffer;
  SpeechProcHybridBuffer *                       mUplinkHybridbuffer;
#endif

  t_uint32                                       mLibraryVersion;
};

#endif // _SPEECHPROCNMF_PROCESSINGCOMP_H_
