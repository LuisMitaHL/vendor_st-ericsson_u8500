/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "cm/inc/cm_macros.h"
#include "AudioVirtualizerNmfMpc.h"
#include "ENS_Nmf.h"
#include "AudioVirtualizer.h"
#include "AFM_nmf.h"


#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "audio_stereowidener_proxy_AudioVirtualizerNmfMpcTraces.h"
#endif

// AudioVirtualizer Memory need
static t_afm_mempreset memory_presets[3]=
{
    //                                   TCM  YTCM DDR24 DDR16 ESR24 ESR16
    { MEM_PRESET_ALL_DDR,            {{    0,    0, 6400,    0,    0,    0 }} },
    { MEM_PRESET_ALL_TCM,            {{ 6400,    0,    0,    0,    0,    0 }} },
    { MEM_PRESET_ALL_ESRAM,          {{    0,    0,    0,    0, 6400,    0 }} }

};


OMX_ERRORTYPE
AudioVirtualizerNmfMpc::construct(void)
{
    OMX_ERRORTYPE error;

    registerStubsAndSkels();

    error = AFMNmfMpc_PcmProcessing::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


void AudioVirtualizerNmfMpc::registerStubsAndSkels(void)
{
    CM_REGISTER_STUBS(stereowidener_cpp);
}


void AudioVirtualizerNmfMpc::unregisterStubsAndSkels(void)
{
    CM_UNREGISTER_STUBS(stereowidener_cpp);
}


OMX_ERRORTYPE  
AudioVirtualizerNmfMpc::destroy(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfMpc_PcmProcessing::destroy();
    if (error != OMX_ErrorNone) return error;

    unregisterStubsAndSkels();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioVirtualizerNmfMpc::instantiateStereoWidener(void)
{
    OMX_ERRORTYPE               error;
            
    error = ENS::instantiateNMFComponent(
                mENSComponent.getNMFDomainHandle(), "stereowidener.nmfil.effect",
                "stereowidener_nmfil", &mNmfil, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
                mENSComponent.getNMFDomainHandle(), "stereowidener.effect.libstereoenhancer",
                "libstereoenhancer", &mNmfStereoWidenerLib, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
                mNmfil, "libstereoenhancer", mNmfStereoWidenerLib, "libstereoenhancer");
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(), "audiolibs.libaudiotables",
			          "libaudiotables", &mNmfAudiotablesLib,getPortPriorityLevel(0));
	  if (error != OMX_ErrorNone) return error;	
    
    error = ENS::bindComponent(mNmfil, "libaudiotables", 
    	                         mNmfAudiotablesLib, "libaudiotables");
	  if (error != OMX_ErrorNone) return error;
	  
	  error = ENS::bindComponent(mNmfil, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;
	  
    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(), "malloc",
                                         "malloc", &mNmfMalloc, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfStereoWidenerLib, "malloc", mNmfMalloc, "malloc");
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::bindComponentFromHostEx(mNmfil, "setheap",
                                        &mISetHeap, 6);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(
                mNmfil, "configure",  &mIconfigure, 2);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE AudioVirtualizerNmfMpc::instantiateAlgo(void)
{
    OMX_ERRORTYPE               error;
    
    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsIn =
        ((AFM_PcmPort *)mENSComponent.getPort(0))->getPcmSettings();
        
    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsOut =
        ((AFM_PcmPort *)mENSComponent.getPort(1))->getPcmSettings();

    //if number of channel at i/p port is 1 or 2 AND number of channels at o/p port is 2, initialize stereowidener
    if ((pcmSettingsIn.nChannels==2 || pcmSettingsIn.nChannels==1 ) && pcmSettingsOut.nChannels==2) {
        virtualizerEffect = 1;
    }
    if (virtualizerEffect == 1)
    {
        error = instantiateStereoWidener();
        OstTraceFiltInst0(TRACE_DEBUG, "STW_PROXY:instantiated Stereowidener algo\n");
        if (error != OMX_ErrorNone) return error;
    }
    return OMX_ErrorNone;
}


OMX_ERRORTYPE AudioVirtualizerNmfMpc::deInstantiateAlgo() {    
OMX_ERRORTYPE error;

    if (virtualizerEffect == 1)
    {
        error = deInstantiateStereoWidener();
        if (error != OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}    
    
OMX_ERRORTYPE AudioVirtualizerNmfMpc::deInstantiateStereoWidener(void)
{
    OMX_ERRORTYPE error;
    t_uint16 memory_bank;

    for (memory_bank=0;memory_bank<MEM_BANK_COUNT;memory_bank++)
    {
        if (mHeap[memory_bank])
        {
            ENS::freeMpcMemory(mHeap[memory_bank]);
            mHeap[memory_bank] = 0;
            mMemoryNeed.size[memory_bank] = 0;
        }
    }

    error = ENS::unbindComponent(mNmfil, "libstereoenhancer");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfil, "libaudiotables");
    if (error != OMX_ErrorNone) return error;
    	
    error = ENS::unbindComponent(mNmfStereoWidenerLib, "malloc");
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::unbindComponent(mNmfil, "osttrace");
    if (error != OMX_ErrorNone) return error;
    	
    error = ENS::destroyNMFComponent(mNmfStereoWidenerLib);
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::destroyNMFComponent(mNmfAudiotablesLib);
	  if (error != OMX_ErrorNone) return error;
	  
    error = ENS::destroyNMFComponent(mNmfMalloc);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost( &mISetHeap);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost( &mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfil);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioVirtualizerNmfMpc::startAlgo() {
    OMX_ERRORTYPE error;

    if (virtualizerEffect == 1)
    {
        error = startStereoWidener();
        if (error != OMX_ErrorNone) return error;
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioVirtualizerNmfMpc::startStereoWidener()
{
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfil);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mNmfStereoWidenerLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mNmfAudiotablesLib);
	if (error != OMX_ErrorNone) return error;
    
    error = ENS::startNMFComponent(mNmfMalloc);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE AudioVirtualizerNmfMpc::stopAlgo() {
    OMX_ERRORTYPE error;

    if (virtualizerEffect == 1)
    {
        error = stopStereoWidener();
        if (error != OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioVirtualizerNmfMpc::stopStereoWidener(void)
{
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfil);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mNmfStereoWidenerLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mNmfAudiotablesLib);
	if (error != OMX_ErrorNone) return error;
    
    error = ENS::stopNMFComponent(mNmfMalloc);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioVirtualizerNmfMpc::configureAlgo(void)
{
    OMX_ERRORTYPE error;
    t_uint16      memory_bank;
    t_uint32      memory_size;
    //StereowidenerParams_t stereowidenerParamsMpc;
    AudioVirtualizer *proxy = static_cast<AudioVirtualizer * >(&mENSComponent);
    

    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsIn =
        ((AFM_PcmPort *)mENSComponent.getPort(0))->getPcmSettings();
        
    mStereowidenerParamsMpc.iMemoryPreset = memory_presets[mENSComponent.getMemoryPreset()].mempreset;
    
    if(virtualizerEffect == 1){
        
       mStereowidenerParamsMpc.iSamplingFreq = pcmSettingsIn.nSamplingRate;
       mStereowidenerParamsMpc.iChannelsIn   = pcmSettingsIn.nChannels;
    }   
      
    mMemoryNeed = memory_presets[mENSComponent.getMemoryPreset()].memory_needs;

    for(memory_bank = 0; memory_bank < MEM_BANK_COUNT; memory_bank++){
        if(mMemoryNeed.size[memory_bank]){
            t_uint32 dsp_address;

            error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(),
                    AFM::memoryBank((t_memory_bank) memory_bank),
                    mMemoryNeed.size[memory_bank],
                    CM_MM_ALIGN_2WORDS,
                    &mHeap[memory_bank]);
            if(error != OMX_ErrorNone){return error;}

            CM_GetMpcMemoryMpcAddress(mHeap[memory_bank], &dsp_address);
            memory_size = mMemoryNeed.size[memory_bank];

            mISetHeap.setHeap((t_memory_bank) memory_bank, (void *) dsp_address, memory_size);
        }
        else{
            mHeap[memory_bank] = 0;
        }
    }

    if (virtualizerEffect == 1){
    mIconfigure.setParameter(mStereowidenerParamsMpc);
    }

    applyConfig((OMX_INDEXTYPE)OMX_IndexConfigAudioVirtualizer, proxy->getOMX_Symbian_AudioVirtualizerConfig());

    return OMX_ErrorNone;

}

OMX_ERRORTYPE AudioVirtualizerNmfMpc::applyConfig(OMX_INDEXTYPE nConfigIndex,OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {
               
        case OMX_IndexConfigAudioVirtualizer:
        {   
            AudioVirtualizer *proxy = static_cast<AudioVirtualizer * >(&mENSComponent);
            OMX_SYMBIAN_AUDIO_CONFIG_VIRTUALIZERTYPE *proxyConfig = proxy->getOMX_Symbian_AudioVirtualizerConfig();
            //StereowidenerConfig_t configMpc;
                

            mConfigMpc.iEnable          = (t_uint16)proxyConfig->bEnable;
            mConfigMpc.iOutputMode      = 0; 
            mConfigMpc.iHeadphoneMode   = proxyConfig->eVirtualizationType;
            mConfigMpc.iInternalExternalSpeaker = 0; //iInternalExternalSpeaker =1 for external speaker mode else it is 0,not supported by SHAI till now

                /* As iStrength varies from 0 to 32768 and nVirtualizationStrength varies from 0 to 100
                   so one step size of nVirtualizationStrength equals to 32768/100 ~327.68 times of iStrength value */
            mConfigMpc.iStrength        = (t_uint16)((proxyConfig->nVirtualizationStrength*32768)/100);

            mIconfigure.setConfig(mConfigMpc);
            
            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioVirtualizerLoudspeaker:
        case OMX_IndexConfigAudioVirtualizerUpmix:
            return OMX_ErrorUnsupportedIndex; // TODO
        
        default:
            return AFMNmfMpc_PcmProcessing::applyConfig(
                       nConfigIndex, pComponentConfigStructure);
    }
}

