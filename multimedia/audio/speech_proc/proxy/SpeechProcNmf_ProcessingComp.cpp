/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     SpeechProcNmf_ProcessingComp.cpp
 * \brief    NMF part of Speech Proc proxy
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "SpeechProcNmf_ProcessingComp.h"
#include "speech_proc_config.h"
#include "share/inc/macros.h"
#include "ENS_Nmf.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "speech_proc_proxy_SpeechProcNmf_ProcessingCompTraces.h"
#endif

SpeechProcNmf_ProcessingComp::SpeechProcNmf_ProcessingComp(ENS_Component &enscomp):
#ifdef USE_HYBRID_COMPONENT
  AFMNmfHostMpc_ProcessingComp(enscomp),
#else
  AFMNmfHost_ProcessingComp(enscomp),
#endif
  mLibrary(NULL),
  mConfigTable(NULL),
  mIndexTable(NULL),
#ifdef USE_HYBRID_COMPONENT
  mMemorySampleRate(NULL),
  mMemoryTimeAlign(NULL),
  mDownlinkHybridbuffer(NULL),
  mUplinkHybridbuffer(NULL),
#endif
  mLibraryVersion(0)
{
}


OMX_ERRORTYPE SpeechProcNmf_ProcessingComp::construct()
{
  OMX_ERRORTYPE error;

#ifdef USE_HYBRID_COMPONENT
  CM_REGISTER_STUBS_SKELS(speech_proc_cpp);
  error = AFMNmfHostMpc_ProcessingComp::construct();
#else
  error = AFMNmfHost_ProcessingComp::construct();
#endif
  if (error != OMX_ErrorNone) return error;

  return OMX_ErrorNone;
}


OMX_ERRORTYPE SpeechProcNmf_ProcessingComp::destroy(void)
{
  OMX_ERRORTYPE error;

#ifdef USE_HYBRID_COMPONENT
  CM_UNREGISTER_STUBS_SKELS(speech_proc_cpp);
  error = AFMNmfHostMpc_ProcessingComp::destroy();
#else
  error = AFMNmfHost_ProcessingComp::destroy();
#endif
  if (error != OMX_ErrorNone) return error;

  return OMX_ErrorNone;
}


OMX_ERRORTYPE SpeechProcNmf_ProcessingComp::applyConfig(OMX_INDEXTYPE nParamIndex,
                                                        OMX_PTR       pComponentConfigStructure)
{
  switch(nParamIndex)
  {
    default :
      {
        OMX_ERRORTYPE error;
        OMX_U32 nb_config;
        bool index_handled = false;

        error = mLibrary->getConfig(UPLINK_ALGORITHMS, nParamIndex, mConfigTable, mIndexTable, &nb_config);
        if(error != OMX_ErrorUnsupportedIndex)
        {
          index_handled = true;
          for(unsigned int i=0; i<nb_config; i++)
          {
            mIconfigureUL.setConfig(mIndexTable[i], mConfigTable[i]);
          }
        }

        error = mLibrary->getConfig(DOWNLINK_ALGORITHMS, nParamIndex, mConfigTable, mIndexTable, &nb_config);
        if(error != OMX_ErrorUnsupportedIndex)
        {
          index_handled = true;
          for(unsigned int i=0; i<nb_config; i++)
          {
            mIconfigureDL.setConfig(mIndexTable[i], mConfigTable[i]);
          }
        }

        if(!index_handled) return OMX_ErrorUnsupportedIndex;
      }
  }

  return OMX_ErrorNone;
}


OMX_ERRORTYPE SpeechProcNmf_ProcessingComp::instantiateMain() {

  t_nmf_error error;

  t_uint32 priority_ul = getPortPriorityLevel(INPUT_UPLINK_PORT);
  t_uint32 priority_dl = getPortPriorityLevel(INPUT_DOWNLINK_PORT);

  if (getPortPriorityLevel(INPUT_REFERENCE_PORT) > priority_ul) {
    priority_ul = getPortPriorityLevel(INPUT_REFERENCE_PORT);
  }
  if (getPortPriorityLevel(OUTPUT_UPLINK_PORT) > priority_ul) {
    priority_ul = getPortPriorityLevel(OUTPUT_UPLINK_PORT);
  }
  if (getPortPriorityLevel(OUTPUT_DOWNLINK_PORT) > priority_dl) {
    priority_dl=getPortPriorityLevel(OUTPUT_DOWNLINK_PORT);
  }

  OstTraceFiltInst2(TRACE_FLOW,"SpeechProc/proxy: instantiateMain, ul priority = %d, dl priority = %d", priority_ul, priority_dl);

#ifdef USE_HYBRID_COMPONENT
  mNmfMain = speech_proc_nmf_host_composite_speech_proc_hybridCreate();
  ((speech_proc_nmf_host_composite_speech_proc_hybrid *)mNmfMain)->domain          = mENSComponent.getNMFDomainHandle();
  ((speech_proc_nmf_host_composite_speech_proc_hybrid *)mNmfMain)->priority_ul     = priority_ul;
  ((speech_proc_nmf_host_composite_speech_proc_hybrid *)mNmfMain)->priority_dl     = priority_dl;
  ((speech_proc_nmf_host_composite_speech_proc_hybrid *)mNmfMain)->priority_dsp_ul = (priority_ul > NMF_SCHED_URGENT)? NMF_SCHED_URGENT:priority_ul;
  ((speech_proc_nmf_host_composite_speech_proc_hybrid *)mNmfMain)->priority_dsp_dl = (priority_dl > NMF_SCHED_URGENT)? NMF_SCHED_URGENT:priority_dl;
  ((speech_proc_nmf_host_composite_speech_proc_hybrid *)mNmfMain)->priority_ctrl   = priority_ul;
#else
  mNmfMain = speech_proc_nmf_host_composite_speech_procCreate();
  ((speech_proc_nmf_host_composite_speech_proc *)mNmfMain)->priority_ul = priority_ul;
  ((speech_proc_nmf_host_composite_speech_proc *)mNmfMain)->priority_dl = priority_dl;
#endif
  if (mNmfMain->construct() != NMF_OK) {
    OstTraceFiltInst0(TRACE_ERROR,"PANIC - SPEECH PROC Construct Error");
    return OMX_ErrorInsufficientResources;
  }

  // Binds configuration interfaces
  OMX_U32 nb_dl_config = mLibrary->getNbParamAndConfig(DOWNLINK_ALGORITHMS);
  error = mNmfMain->bindFromUser("configure_downlink", (nb_dl_config + 3), &mIconfigureDL);
  if ( error != NMF_OK )
  {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to bind downlink configuration!...[%d]", error);
    return OMX_ErrorUndefined;
  }

  OMX_U32 nb_ul_config = mLibrary->getNbParamAndConfig(UPLINK_ALGORITHMS);
  error = mNmfMain->bindFromUser("configure_uplink", (nb_ul_config + 3), &mIconfigureUL);
  if ( error != NMF_OK )
  {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to bind uplink configuration!...[%d]", error);
    return OMX_ErrorUndefined;
  }

  if(nb_ul_config > nb_dl_config) nb_dl_config = nb_ul_config;

  mConfigTable = new OMX_PTR[nb_dl_config];
  if(mConfigTable == NULL) return OMX_ErrorInsufficientResources;

  mIndexTable  = new OMX_U32[nb_dl_config];
  if(mIndexTable == NULL) return OMX_ErrorInsufficientResources;

  error = mNmfMain->bindFromUser("configure_time_align", 3, &mIconfigureTimealign);
  if ( error != NMF_OK )
  {
	OstTraceFiltInst1(TRACE_ERROR,"Error: unable to bind time align configuration!...[%d]", error);
    return OMX_ErrorUndefined;
  }

#ifdef USE_HYBRID_COMPONENT
  error = mNmfMain->bindFromUser("configure_dsp_port", 1, &mIconfigureDspPort);
  if ( error != NMF_OK )
  {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to bind dsp port configuration!...[%d]", error);
    return OMX_ErrorUndefined;
  }
#endif // USE_HYBRID_COMPONENT

  // sidetone port
  error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(),mNmfMain,"sidetone",static_cast<speech_proc_nmf_host_common_sidetoneDescriptor*>(this),1);
  if (error != NMF_OK)
  {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to bind sidetone interface!...[%d]", error);
    return OMX_ErrorUndefined;
  }

  // context return
  error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(),mNmfMain,"context",static_cast<speech_proc_nmf_host_common_contextDescriptor*>(this),2);
  if (error != NMF_OK)
  {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to bind context interface!...[%d]", error);
    return OMX_ErrorUndefined;
  }

  return OMX_ErrorNone;
}

OMX_ERRORTYPE SpeechProcNmf_ProcessingComp::startMain()
{
  OstTraceFiltInst0(TRACE_FLOW,"SpeechProc/proxy: startMain");
  mNmfMain->start();
  return OMX_ErrorNone;
}

OMX_ERRORTYPE SpeechProcNmf_ProcessingComp::stopMain()
{
  OstTraceFiltInst0(TRACE_FLOW,"SpeechProc/proxy: stopMain");
  mNmfMain->stop_flush();
  mLibrary->setLibCtx((void *)0,UPLINK_ALGORITHMS);
  mLibrary->setLibCtx((void *)0,DOWNLINK_ALGORITHMS);
  return OMX_ErrorNone;
}


OMX_ERRORTYPE SpeechProcNmf_ProcessingComp::configureMain()
{
  OMX_AUDIO_PARAM_PCMMODETYPE   *pcm_settings[SPEECH_PROC_NB_AUDIO_PORT];
  OMX_U32       port_status    = 0;
  t_uint16      port_enable    = 0;
  OMX_U32       previous_index = 0;
  OMX_U32       current_index  = 0;
  OMX_PTR       param          = NULL;
  OMX_ERRORTYPE error;
  t_uint16      nb_ref_channels;

  OstTraceFiltInst0(TRACE_FLOW,"SpeechProc/proxy: configureMain");

  // Check Port settings
  for(int i=0;i<SPEECH_PROC_NB_AUDIO_PORT;i++)
  {
    AFM_PcmPort *port = static_cast<AFM_PcmPort *>(mENSComponent.getPort(i));
    pcm_settings[i] = &(port->getPcmSettings());
    if(port->isEnabled()) port_status |= (1<<i);
  }
  nb_ref_channels = (t_uint16)pcm_settings[INPUT_REFERENCE_PORT]->nChannels;

#ifdef FORCE_MONO_REFERENCE
  // at NMF level we force SRC input as mono
  // (conversion is done if necessary by binding)
  nb_ref_channels = 1;
#endif

  error = mLibrary->checkPortSettings(pcm_settings, port_status);
  if(error != OMX_ErrorNone) return error;

  ////////////////////////////
  // Uplink configuration
  ////////////////////////////

#ifdef USE_HYBRID_COMPONENT
  // Hybrid Buffer
  // for now allocate worst case : 16kHz + stero reference signal and 6 channle on uplink
  // as we do not reallocate the buffer if port config is changed
  t_cm_size buf_size = (MAX_SAMPLE_FREQ * 2 * PROCESSING_BUFFER_DURATION);    /* PROCESSING_BUFFER_DURATION ms in Bytes for 1 channel at max frequency*/
  buf_size = buf_size * (MAX_NB_REF_CHANNEL + MAX_NB_INPUT_CHANNEL);
  mUplinkHybridbuffer = new SpeechProcHybridBuffer(mENSComponent.getNMFDomainHandle(INPUT_UPLINK_PORT),
												   buf_size, UL_HYBRID_BUFFER_MEMORY, error);
  if(error != OMX_ErrorNone)
  {
	cleanMpcMemory();
	return error;
  }

  // time alignment config
  t_uint32 src_dsp_addr;
  t_uint32 ring_dsp_addr;
  t_nmf_error nmf_error;

  error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(INPUT_REFERENCE_PORT),
                              SRC_MEMORY, SRC_HEAP_SIZE, CM_MM_ALIGN_WORD, &mMemorySampleRate);
  if(error != OMX_ErrorNone)
  {
	cleanMpcMemory();
	return error;
  }

  nmf_error = CM_GetMpcMemoryMpcAddress(mMemorySampleRate, &src_dsp_addr);
  if(nmf_error != CM_OK)
  {
	error = OMX_ErrorUndefined;
	cleanMpcMemory();
	return error;
  }

  error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(INPUT_REFERENCE_PORT),
                              RING_BUFFER_MEMORY, RING_BUFFER_SIZE, CM_MM_ALIGN_WORD, &mMemoryTimeAlign);
  if(error != OMX_ErrorNone)
  {
	cleanMpcMemory();
	return error;
  }

  nmf_error = CM_GetMpcMemoryMpcAddress(mMemoryTimeAlign, &ring_dsp_addr);
  if(nmf_error != CM_OK)
  {
	error = OMX_ErrorUndefined;
	cleanMpcMemory();
	return error;
  }


  mIconfigureTimealign.setParameter(mUplinkHybridbuffer->getMPCHeader(), ring_dsp_addr,
                                    src_dsp_addr, SRC_HEAP_SIZE, (t_uint16)realtimeReference(),
									RING_BUFFER_SIZE);
#else
  // only configure real_time_ref
  mIconfigureTimealign.setParameter(0, 0, 0, 0, isReferenceRealTime());
#endif

  // time alignement
  if(port_status & (1<<INPUT_UPLINK_PORT))
  {
    mIconfigureTimealign.setConfig((t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nSamplingRate,
                                   (t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nChannels);
  }

  if(port_status & (1<<INPUT_REFERENCE_PORT))
  {
    mIconfigureTimealign.setConfigRef((t_uint16)pcm_settings[INPUT_REFERENCE_PORT]->nSamplingRate,
                                      (t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nSamplingRate,
                                      nb_ref_channels);
  }

  // UL wrapper
  t_uint16 bypass = 0;
  if(outputReference()) { bypass = 0x100; }
  if(uplinkBypass() | mLibrary->isBypass(UPLINK_ALGORITHMS)) { bypass += 1; }
  mIconfigureUL.initLib(bypass);

  if((port_status & (1<<OUTPUT_UPLINK_PORT)) &&
     (port_status & (1<<INPUT_UPLINK_PORT)))
  {

    port_enable = 0x5;
    if(port_status & (1<<INPUT_REFERENCE_PORT)) port_enable |= 0x2;

    mIconfigureUL.configure((t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nSamplingRate,
                            (t_uint16)pcm_settings[OUTPUT_UPLINK_PORT]->nSamplingRate,
                            (t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nChannels,
                            nb_ref_channels,
                            port_enable);
  }

  // UL processing
  param = mLibrary->getNextParameter(UPLINK_ALGORITHMS, 0, &current_index);
  while(param)
  {
    mIconfigureUL.setParameter(current_index, param);
    previous_index = current_index;
    param = mLibrary->getNextParameter(UPLINK_ALGORITHMS, previous_index, &current_index);
  }

  param = mLibrary->getNextConfig(UPLINK_ALGORITHMS, 0, &current_index);
  while(param)
  {
    mIconfigureUL.setConfig(current_index, param);
    previous_index = current_index;
    param = mLibrary->getNextConfig(UPLINK_ALGORITHMS, previous_index, &current_index);
  }

  ////////////////////////////
  // Downlink configuration
  ////////////////////////////
#ifdef USE_HYBRID_COMPONENT
  // Shared Buffer
  buf_size = (MAX_SAMPLE_FREQ * 2 * PROCESSING_BUFFER_DURATION); /* PROCESSING_BUFFER_DURATION ms in Bytes for 1 channel at 16kHz (worst case) */
  mDownlinkHybridbuffer = new SpeechProcHybridBuffer(mENSComponent.getNMFDomainHandle(OUTPUT_DOWNLINK_PORT),
													 buf_size, DL_HYBRID_BUFFER_MEMORY, error);
  if(error != OMX_ErrorNone)
  {
	cleanMpcMemory();
	return error;
  }

  mIconfigureDspPort.setParameter(mDownlinkHybridbuffer->getMPCHeader());
#endif

  // DL wrapper
  mIconfigureDL.initLib((t_uint16)(downlinkBypass() | mLibrary->isBypass(DOWNLINK_ALGORITHMS)));

  if((port_status & (1<<OUTPUT_DOWNLINK_PORT)) &&
     (port_status & (1<<INPUT_DOWNLINK_PORT)))
  {
    port_enable = 0x3;

    mIconfigureDL.configure((t_uint16)pcm_settings[INPUT_DOWNLINK_PORT]->nSamplingRate,
                            (t_uint16)pcm_settings[OUTPUT_DOWNLINK_PORT]->nSamplingRate,
                            (t_uint16)pcm_settings[INPUT_DOWNLINK_PORT]->nChannels,
                            0, port_enable);
  }

  // DL processing
  param = mLibrary->getNextParameter(DOWNLINK_ALGORITHMS, 0, &current_index);
  while(param)
  {
    mIconfigureDL.setParameter(current_index, param);
    previous_index = current_index;
    param = mLibrary->getNextParameter(DOWNLINK_ALGORITHMS, previous_index, &current_index);
  }

  param = mLibrary->getNextConfig(DOWNLINK_ALGORITHMS, 0, &current_index);
  while(param)
  {
    mIconfigureDL.setConfig(current_index, param);
    previous_index = current_index;
    param = mLibrary->getNextConfig(DOWNLINK_ALGORITHMS, previous_index, &current_index);
  }

  return OMX_ErrorNone;
}


OMX_ERRORTYPE SpeechProcNmf_ProcessingComp::deInstantiateMain() {
  t_nmf_error   nmf_error;

  OstTraceFiltInst0(TRACE_FLOW,"SpeechProc/proxy: deInstantiateMain");

  delete [] mConfigTable;
  delete [] mIndexTable;

  // sidetone
  nmf_error = EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mNmfMain, "sidetone");
  if (nmf_error != NMF_OK) {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to unbind sidetone!...[%d]", nmf_error);
    return OMX_ErrorUndefined;
  }

  // context
  nmf_error = EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mNmfMain, "context");
  if (nmf_error != NMF_OK) {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to unbind context!...[%d]", nmf_error);
    return OMX_ErrorUndefined;
  }

  // Unbinds configuration interfaces
  nmf_error = mNmfMain->unbindFromUser("configure_downlink");
  if ( nmf_error != NMF_OK )
  {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to unbind downlink configuration!...[%d]", nmf_error);
    return OMX_ErrorUndefined;
  }

  nmf_error = mNmfMain->unbindFromUser("configure_uplink");
  if ( nmf_error != NMF_OK )
  {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to unbind uplink configuration!...[%d]", nmf_error);
    return OMX_ErrorUndefined;
  }

  nmf_error = mNmfMain->unbindFromUser("configure_time_align");
  if ( nmf_error != NMF_OK )
  {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to unbind time align configuration!...[%d]", nmf_error);
    return OMX_ErrorUndefined;
  }

#ifdef USE_HYBRID_COMPONENT
  nmf_error = mNmfMain->unbindFromUser("configure_dsp_port");
  if ( nmf_error != NMF_OK )
  {
    OstTraceFiltInst1(TRACE_ERROR,"Error: unable to unbind dsp port configuration!...[%d]", nmf_error);
    return OMX_ErrorUndefined;
  }

  cleanMpcMemory();
#endif // USE_HYBRID_COMPONENT

  // destroy the main component
  nmf_error = mNmfMain->destroy();
  if(nmf_error != NMF_OK) return OMX_ErrorUndefined;

#ifdef USE_HYBRID_COMPONENT
  speech_proc_nmf_host_composite_speech_proc_hybridDestroy((speech_proc_nmf_host_composite_speech_proc_hybrid*&)mNmfMain);
#else
  speech_proc_nmf_host_composite_speech_procDestroy((speech_proc_nmf_host_composite_speech_proc*&)mNmfMain);
#endif

  return OMX_ErrorNone;
}


OMX_ERRORTYPE SpeechProcNmf_ProcessingComp::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
  if((eCmd  != OMX_CommandStateSet) &&
     (nData == OUTPUT_CONTROL_PORT)) return OMX_ErrorNone; // Do not reach AFM layer for control port commands
                                                           // except for OMX_ALL

  if(eCmd == OMX_CommandPortEnable)
  {
    OMX_AUDIO_PARAM_PCMMODETYPE *pcm_settings[SPEECH_PROC_NB_AUDIO_PORT];
    t_uint32 port_status = 0;
    t_uint16 nb_ref_channels;


    // Check Port settings
    for(int i=0;i<SPEECH_PROC_NB_AUDIO_PORT;i++)
    {
      AFM_PcmPort   *port = static_cast<AFM_PcmPort *>(mENSComponent.getPort(i));
      pcm_settings[i] = &(port->getPcmSettings());
      if(port->isEnabled()) port_status |= (1<<i);
    }
    nb_ref_channels = (t_uint16)pcm_settings[INPUT_REFERENCE_PORT]->nChannels;

#ifdef FORCE_MONO_REFERENCE
    // at NMF level we force SRC input as mono
    // (conversion is done if necessary by binding)
    nb_ref_channels = 1;
#endif

    if(nData != OMX_ALL) port_status |= (1<<nData);
    else                 port_status  = OMX_ALL;

    OMX_ERRORTYPE error;
    error = mLibrary->checkPortSettings(pcm_settings, port_status);
    if(error != OMX_ErrorNone) return error;

    ///////////////////////////
    // reconfigure downlink
    ///////////////////////////
    if((nData == OMX_ALL) ||
       ((nData == INPUT_DOWNLINK_PORT)  && (port_status & (1<<OUTPUT_DOWNLINK_PORT))) ||
       ((nData == OUTPUT_DOWNLINK_PORT) && (port_status & (1<<INPUT_DOWNLINK_PORT))))
    {
      t_uint16 port_enable = 0x3;
      mIconfigureDL.configure((t_uint16)pcm_settings[INPUT_DOWNLINK_PORT]->nSamplingRate,
                              (t_uint16)pcm_settings[OUTPUT_DOWNLINK_PORT]->nSamplingRate,
                              (t_uint16)pcm_settings[INPUT_DOWNLINK_PORT]->nChannels,
                              0,port_enable);

    }

    ///////////////////////////
    // reconfigure uplink
    ///////////////////////////
    if((nData == OMX_ALL) ||
       ((nData == INPUT_REFERENCE_PORT) && (port_status & (1<<INPUT_UPLINK_PORT))) ||
       ((nData == INPUT_UPLINK_PORT)    && (port_status & (1<<INPUT_REFERENCE_PORT))))
    {
      mIconfigureTimealign.setConfigRef((t_uint16)pcm_settings[INPUT_REFERENCE_PORT]->nSamplingRate,
                                        (t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nSamplingRate,
                                        nb_ref_channels);
    }

    if((nData == OMX_ALL) ||
       ((nData == INPUT_UPLINK_PORT)  && (port_status & (1<<OUTPUT_UPLINK_PORT))) ||
       ((nData == OUTPUT_UPLINK_PORT) && (port_status & (1<< INPUT_UPLINK_PORT))) ||
       ((nData == INPUT_REFERENCE_PORT)        &&
        (port_status & (1<<INPUT_UPLINK_PORT)) &&
        (port_status & (1<<OUTPUT_UPLINK_PORT))))
    {
      mIconfigureTimealign.setConfig((t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nSamplingRate,
                                     (t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nChannels);

      t_uint16 port_enable = 0x5;
      if(port_status & (1<<INPUT_REFERENCE_PORT)) port_enable |= 0x2;

      mIconfigureUL.configure((t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nSamplingRate,
                              (t_uint16)pcm_settings[OUTPUT_UPLINK_PORT]->nSamplingRate,
                              (t_uint16)pcm_settings[INPUT_UPLINK_PORT]->nChannels,
                              nb_ref_channels,
                              port_enable);

    }
  }

#ifdef USE_HYBRID_COMPONENT
  return AFMNmfHostMpc_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
#else
  return AFMNmfHost_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
#endif
}


const char * SpeechProcNmf_ProcessingComp::getNmfSuffix(OMX_U32 portidx)const{
  speech_proc_port_name_t port = (speech_proc_port_name_t)portidx;

#ifdef USE_HYBRID_COMPONENT
  switch(port)
  {
    case INPUT_UPLINK_PORT:     return "[0]";
    case INPUT_DOWNLINK_PORT:   return "";
    case INPUT_REFERENCE_PORT:  return "[1]";
    case OUTPUT_UPLINK_PORT:    return "";
    case OUTPUT_DOWNLINK_PORT:  return "";
    default: return "";
  }
#else
  switch(port)
  {
    case INPUT_UPLINK_PORT:     return "[0]";
    case INPUT_DOWNLINK_PORT:   return "[1]";
    case INPUT_REFERENCE_PORT:  return "[2]";
    case OUTPUT_UPLINK_PORT:    return "[0]";
    case OUTPUT_DOWNLINK_PORT:  return "[1]";
    default: return "";
  }
#endif
}


void SpeechProcNmf_ProcessingComp::newSidetoneGain(t_sint32 gain)
{
#ifdef USE_CONTROL_PORT
  SpeechProc_ControlPort * controlPort = static_cast<SpeechProc_ControlPort *>(mENSComponent.getPort(OUTPUT_CONTROL_PORT));
  controlPort->applyNewSidetoneGain(gain);
#endif
}


void SpeechProcNmf_ProcessingComp::newLibraryContext(void *ctx, t_uint16 algo, t_uint32 version)
{
  OstTraceFiltInst1(TRACE_FLOW, "Speech Proc : processing library version is 0x%x", version);
  mLibraryVersion = version;
  mLibrary->setLibCtx(ctx,(speech_proc_algo_type_t)algo);
}


#ifdef USE_HYBRID_COMPONENT

t_cm_instance_handle SpeechProcNmf_ProcessingComp::getNmfMpcHandle(OMX_U32 portidx) const
{
  speech_proc_port_name_t port = (speech_proc_port_name_t)portidx;

  switch(port)
  {
    case INPUT_UPLINK_PORT:     return mNmfMain->getMPCComponentHandle("time_align");
    case INPUT_DOWNLINK_PORT:   return 0;
    case INPUT_REFERENCE_PORT:  return mNmfMain->getMPCComponentHandle("time_align");
    case OUTPUT_UPLINK_PORT:    return 0;
    case OUTPUT_DOWNLINK_PORT:  return mNmfMain->getMPCComponentHandle("dsp_port");
    default: return 0;
  }
}


void SpeechProcNmf_ProcessingComp::cleanMpcMemory(void)
{
  OMX_ERRORTYPE error = OMX_ErrorNone;

  if(mMemorySampleRate != 0)
  {
	error = ENS::freeMpcMemory(mMemorySampleRate);
	DBC_ASSERT(error == OMX_ErrorNone);
	mMemorySampleRate = 0;
  }

  if(mMemoryTimeAlign != 0)
  {
	error = ENS::freeMpcMemory(mMemoryTimeAlign);
	DBC_ASSERT(error == OMX_ErrorNone);
	mMemoryTimeAlign = 0;
  }

  if(mDownlinkHybridbuffer != NULL)
  {
	delete mDownlinkHybridbuffer;
	mDownlinkHybridbuffer = NULL;
  }

  if(mUplinkHybridbuffer != NULL)
  {
	delete mUplinkHybridbuffer;
	mUplinkHybridbuffer = NULL;
  }
}

#endif //USE_HYBRID_COMPONENT


