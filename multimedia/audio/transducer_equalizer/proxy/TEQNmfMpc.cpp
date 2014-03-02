/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   TEQNmfMpc.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <string.h>
#include "TEQNmfMpc.h"
#include "ENS_Nmf.h"
#include "AFM_nmf.h"


#define MAX_RAM_X (NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS * 6 + NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS)
#define MAX_RAM_Y (4 * (NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS / 2 + 1) + NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS)

// TransducerEqualizer Memory need
static t_afm_mempreset memory_presets[]=
{
    //                                        TCM        YTCM       DDR24 DDR16       ESR24 ESR16
    { MEM_PRESET_ALL_TCM,            {{ MAX_RAM_X,  MAX_RAM_Y,          0,    0,          0,    0 }} },
    { MEM_PRESET_ALL_ESRAM,          {{         0,  MAX_RAM_Y,          0,    0,  MAX_RAM_X,    0 }} },
    { MEM_PRESET_ALL_DDR,            {{         0,  MAX_RAM_Y,  MAX_RAM_X,    0,          0,    0 }} },
    { MEM_PRESET_MIX_DDR_TCM_1,      {{ MAX_RAM_X,  MAX_RAM_Y,          0,    0,          0,    0 }} },
    { MEM_PRESET_MIX_ESRAM_DDR,      {{         0,  MAX_RAM_Y,          0,    0,  MAX_RAM_X,    0 }} },
    { MEM_PRESET_MIX_ESRAM_TCM,      {{ MAX_RAM_X,  MAX_RAM_Y,          0,    0,          0,    0 }} }
};


OMX_ERRORTYPE TEQNmfMpc::construct(void)
{
    ConfigStructHandle                    = 0;
    ConfigStructDspAddress                = 0;
    p_TransducerEqualizerConfigStructFifo = NULL;

    registerStubsAndSkels();

    return AFMNmfMpc_PcmProcessing::construct();
}


void TEQNmfMpc::registerStubsAndSkels(void)
{
    CM_REGISTER_STUBS(transducer_equalizer_cpp);
}


void TEQNmfMpc::unregisterStubsAndSkels(void)
{
    CM_UNREGISTER_STUBS(transducer_equalizer_cpp);
}


OMX_ERRORTYPE TEQNmfMpc::destroy(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfMpc_PcmProcessing::destroy();
    if (error != OMX_ErrorNone) return error;

    unregisterStubsAndSkels();
    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfMpc::instantiateAlgo(void)
{
    OMX_ERRORTYPE error;

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(),"transducer_equalizer.nmfil.effect",
                                         "transducer_equalizer_nmfil",&mNmfil,getPriorityLevel());
    if(error != OMX_ErrorNone){return error;}

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(),"libeffects.mpc.libeffects",
                                         "libeffects",&mNmfEffectsLib,getPriorityLevel());
    if(error != OMX_ErrorNone){return error;}

    error = ENS::bindComponent(mNmfEffectsLib, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfil, "libtransducer_equalizer", mNmfEffectsLib, "libtransducer_equalizer");
    if(error != OMX_ErrorNone){return error;}

    error = ENS::bindComponentFromHostEx(mNmfil, "setheap",  &mISetHeap, 6);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::bindComponentFromHostEx(mNmfil, "configure",  &mIconfigure, TEQ_FIFO_CONFIG_SIZE - 1);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(), "malloc", "malloc", &mNmfMalloc, getPriorityLevel());
    if(error != OMX_ErrorNone){return error;}

    error = ENS::bindComponent(mNmfil, "malloc", mNmfMalloc, "malloc");
    if(error != OMX_ErrorNone){return error;}

    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfMpc::deInstantiateAlgo(void)
{
#if defined(MATLAB_TEQ_SHAI_CONFIG) || (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0)
    TransducerEqualizer *proxy = static_cast<TransducerEqualizer *>(&mENSComponent);
#endif // MATLAB_TEQ_SHAI_CONFIG || (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0)
    OMX_ERRORTYPE       error;
    t_uint16            memory_bank;

    for(memory_bank = 0; memory_bank < MEM_BANK_COUNT; memory_bank++)
    {
        if(mHeap[memory_bank])
        {
            ENS::freeMpcMemory(mHeap[memory_bank]);
            mHeap[memory_bank]            = 0;
            mMemoryNeed.size[memory_bank] = 0;
        }
    }
    if(p_TransducerEqualizerConfigStructFifo != NULL)
    {
        ENS::freeMpcMemory(ConfigStructHandle);
        p_TransducerEqualizerConfigStructFifo = NULL;
    }

    error = ENS::unbindComponent(mNmfil, "libtransducer_equalizer");
    if(error != OMX_ErrorNone){return error;}

    error = ENS::unbindComponent(mNmfEffectsLib, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfEffectsLib);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::unbindComponentFromHost( &mISetHeap);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::unbindComponentFromHost( &mIconfigure);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::unbindComponent(mNmfil, "malloc");
    if(error != OMX_ErrorNone){return error;}

    error = ENS::destroyNMFComponent(mNmfil);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::destroyNMFComponent(mNmfMalloc);
    if(error != OMX_ErrorNone){return error;}

#ifdef MATLAB_TEQ_SHAI_CONFIG
    close_matlab_TEQ_SHAI_file(proxy->getInstance());
#endif // MATLAB_TEQ_SHAI_CONFIG

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
    close_debug_TEQ_SHAI_file(proxy->getInstance());
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0

    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfMpc::startAlgo(void)
{
    OMX_ERRORTYPE error;

    configureAlgoRequested = false;

    error = ENS::startNMFComponent(mNmfil);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::startNMFComponent(mNmfEffectsLib);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::startNMFComponent(mNmfMalloc);
    if(error != OMX_ErrorNone){return error;}

    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfMpc::stopAlgo(void)
{
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfil);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::stopNMFComponent(mNmfEffectsLib);
    if(error != OMX_ErrorNone){return error;}

    error = ENS::stopNMFComponent(mNmfMalloc);
    if(error != OMX_ErrorNone){return error;}

    configureAlgoRequested = false;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfMpc::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
    if(configureAlgoRequested)
    {
        TransducerEqualizer *proxy         = static_cast<TransducerEqualizer *>(&mENSComponent);
        AFM_PcmPort         *p_pcmport_in  = static_cast<AFM_PcmPort         *>(proxy->getPort(TREQ_IN_PORT));
        AFM_PcmPort         *p_pcmport_out = static_cast<AFM_PcmPort         *>(proxy->getPort(TREQ_OUT_PORT));


#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "TEQNmfMpc::doSpecificSendCommand\n");
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "eCmd                       = ");
        if(eCmd == OMX_CommandPortEnable)
        {
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "OMX_CommandPortEnable\n");
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "port                       = %d\n", (int) nData);
        }
        else if(eCmd == OMX_CommandPortDisable)
        {
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "OMX_CommandPortDisable\n");
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "port                       = %d\n", (int) nData);
        }
        else
        {
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "%d\n", (int) eCmd);
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "nData                      = %d\n", (int) nData);
        }
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "p_pcmport_in->isEnabled()  = %d\n", (int) p_pcmport_in->isEnabled());
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "p_pcmport_out->isEnabled() = %d\n", (int) p_pcmport_out->isEnabled());
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "setParamDone               = %d\n", (int) setParamDone);
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "defaultParams              = %d\n", (int) proxy->getDefaultParams());
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "--------------------------------------------------\n");
        fflush(debug_TEQ_SHAI_file[proxy->getInstance()]);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1

        if(   (eCmd == OMX_CommandPortEnable)
           && ((nData == TREQ_IN_PORT) || ((nData == TREQ_OUT_PORT) || (nData == OMX_ALL))))
        {
            // a port has been enabled
            if(p_pcmport_in->isEnabled() && p_pcmport_out->isEnabled())
            {
                OMX_ERRORTYPE error;
                bool          doConfigureAlgo_port_enabled = false;

                error = proxy->getSamplingRateChannels(p_pcmport_in, p_pcmport_out, &nSamplingRate, &nChannels);
                if(error != OMX_ErrorNone)
                {
                    return error;
                }

                if(nChannels != last_nChannels)
                {
                    if(proxy->getDefaultParams() || (last_nChannels == -1))
                    {
                        last_nChannels = nChannels;
                    }
                    else
                    {
                        return OMX_ErrorPortsNotCompatible;
                    }
                }

                if(setParamDone)
                {
                    // check that port settings are compliant with parameters
                    if(nChannels != paramsNbChannels)
                    {
                        return OMX_ErrorPortsNotCompatible;
                    }
                }
                else
                {
                    // configureAlgo hasn't been done because port wasn't enable when configureAlgo has been called
                    // so number of channels & sampling rate weren't certain
                    // now they are certain (because port is enabled) so we can do the configureAlgo
                    doConfigureAlgo_port_enabled = true;
                }

                if(nSamplingRate != last_nSamplingRate)
                {
                    // if sampling rate has changed and if we use SHAI interface,
                    // we must recompute already computed biquad cells
                    // and verify that validated biquad cells are still valid
                    proxy->checkSHAIFilterUpdate(nChannels);

                    last_nSamplingRate = nSamplingRate;

                    // sampling rate is valid (because output port is enabled),
                    // thus configure algo with last SHAI config
                    doConfigureAlgo_port_enabled = true;
                }

                if(doConfigureAlgo_port_enabled)
                {
                    error = configureAlgo_port_enabled(proxy);
                    if(error != OMX_ErrorNone)
                    {
                        return error;
                    }
                }
            }
        }
    }

    bDeferredCmd = OMX_FALSE;

    return AFMNmfMpc_PcmProcessing::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}


OMX_ERRORTYPE TEQNmfMpc::setProxyParams(TransducerEqualizer *proxy)
{
    TransducerEqualizerParams_t equalizerParamsMpc;
    int                         nb_alloc_biquad_cells_per_channel, nb_alloc_FIR_coefs_per_channel;
    bool                        stereo = (nChannels > 1);
    t_uint16                    memory_bank;
    t_uint32                    memory_size;
    t_uint32                    i, RAM_X_size, RAM_Y_size;
    OMX_ERRORTYPE               error;


    error = proxy->getEffectParams(nChannels, &equalizerParamsMpc);
    if(error != OMX_ErrorNone)
    {
        return error;
    }
    nb_alloc_biquad_cells_per_channel = (int) equalizerParamsMpc.inb_alloc_biquad_cells_per_channel;
    nb_alloc_FIR_coefs_per_channel    = (int) equalizerParamsMpc.inb_alloc_FIR_coefs_per_channel;
    equalizerParamsMpc.iMemoryPreset  = memory_presets[mENSComponent.getMemoryPreset()].mempreset;

    // RAM heap optimization
    // WARNING : must be aligned with nmfil/effect/src/effect.c allocations
    if(stereo)
    {
        RAM_X_size = 12 *  nb_alloc_biquad_cells_per_channel      + 2 *   nb_alloc_FIR_coefs_per_channel;
        RAM_Y_size =  4 * (nb_alloc_biquad_cells_per_channel + 1) + 2 *   nb_alloc_FIR_coefs_per_channel;
    }
    else
    {
        RAM_X_size =  6 *  nb_alloc_biquad_cells_per_channel      + 2 * ((nb_alloc_FIR_coefs_per_channel + 1) / 2);
        RAM_Y_size =  2 * (nb_alloc_biquad_cells_per_channel + 1) + 2 * ((nb_alloc_FIR_coefs_per_channel + 1) / 2);
    }
    for(i = 0; i < sizeof(memory_presets) / sizeof(t_afm_mempreset); i++)
    {
        for(memory_bank = 0; memory_bank < MEM_BANK_COUNT; memory_bank++)
        {
            memory_presets[i].memory_needs.size[memory_bank] = 0;
        }
        switch(memory_presets[i].mempreset)
        {
            case MEM_PRESET_ALL_TCM:
            case MEM_PRESET_MIX_ESRAM_TCM:
            case MEM_PRESET_MIX_DDR_TCM_1:
            case MEM_PRESET_MIX_DDR_TCM_2:
            case MEM_PRESET_MIX_DDR_TCM_3:
            case MEM_PRESET_MIX_DDR_TCM_4:
            case MEM_PRESET_MIX_DDR_TCM_5:
                memory_presets[i].memory_needs.size[MEM_XTCM] = RAM_X_size;
                break;
            case MEM_PRESET_ALL_ESRAM:
            case MEM_PRESET_MIX_ESRAM_DDR:
            case MEM_PRESET_MIX_ESRAM_OTHER_1:
            case MEM_PRESET_MIX_ESRAM_OTHER_2:
            case MEM_PRESET_MIX_ESRAM_OTHER_3:
            case MEM_PRESET_MIX_ESRAM_OTHER_4:
            case MEM_PRESET_MIX_ESRAM_OTHER_5:
                memory_presets[i].memory_needs.size[MEM_ESR24] = RAM_X_size;
                break;
            case MEM_PRESET_ALL_DDR:
            default:
                memory_presets[i].memory_needs.size[MEM_DDR24] = RAM_X_size;
                break;
        }
        memory_presets[i].memory_needs.size[MEM_YTCM] = RAM_Y_size;
    }


    mMemoryNeed = memory_presets[mENSComponent.getMemoryPreset()].memory_needs;

    for(memory_bank = 0; memory_bank < MEM_BANK_COUNT; memory_bank++)
    {
        if(mMemoryNeed.size[memory_bank])
        {
            t_uint32 dsp_address;

            error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(),
                                        AFM::memoryBank((t_memory_bank) memory_bank),
                                        mMemoryNeed.size[memory_bank],
                                        CM_MM_ALIGN_2WORDS,
                                        &mHeap[memory_bank]);
            if(error != OMX_ErrorNone)
            {
                return error;
            }

            CM_GetMpcMemoryMpcAddress(mHeap[memory_bank], &dsp_address);
            memory_size = mMemoryNeed.size[memory_bank];

            mISetHeap.setHeap((t_memory_bank) memory_bank, (void *) dsp_address, memory_size);
        }
        else
        {
            mHeap[memory_bank] = 0;
        }
    }

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "mIconfigure.setParameter(equalizerParamsMpc)\n");
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "--------------------------------------------------\n");
    fflush(debug_TEQ_SHAI_file[proxy->getInstance()]);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    mIconfigure.setParameter(equalizerParamsMpc);
    paramsNbChannels = (equalizerParamsMpc.istereo ? 2 : 1);
    setParamDone     = true;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfMpc::configureAlgo_port_enabled(TransducerEqualizer *proxy)
{
    OMX_ERRORTYPE error;


#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "TEQNmfMpc::configureAlgo_port_enabled\n");
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "setParamDone = %d\n", (int) setParamDone);
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "--------------------------------------------------\n");
    fflush(debug_TEQ_SHAI_file[proxy->getInstance()]);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1

    if(!setParamDone)
    {
        error = setProxyParams(proxy);
        if(error != OMX_ErrorNone)
        {
            return error;
        }
    }

    if(proxy->getConfigUse() == USE_SHAI_CONFIG)
    {
        doConfigureAlgo = true;

        error = applyConfig((OMX_INDEXTYPE) OMX_Symbian_IndexConfigAudioTransEqualizer_iireq, NULL);
        if(error != OMX_ErrorNone)
        {
            return error;
        }

        error = applyConfig((OMX_INDEXTYPE) OMX_Symbian_IndexConfigAudioTransEqualizer_firresponse, NULL);
        if(error != OMX_ErrorNone)
        {
            return error;
        }

        error = applyConfig((OMX_INDEXTYPE) OMX_Symbian_IndexConfigAudioTransEqualizer, NULL);
        if(error != OMX_ErrorNone)
        {
            return error;
        }

        doConfigureAlgo = false;
    }

    return applyConfig((OMX_INDEXTYPE) AFM_IndexConfigTransducerEqualizer, NULL);
}


OMX_ERRORTYPE TEQNmfMpc::configureAlgo(void)
{
    TransducerEqualizer *proxy         = static_cast<TransducerEqualizer *>(&mENSComponent);
    AFM_PcmPort         *p_pcmport_in  = static_cast<AFM_PcmPort         *>(proxy->getPort(TREQ_IN_PORT));
    AFM_PcmPort         *p_pcmport_out = static_cast<AFM_PcmPort         *>(proxy->getPort(TREQ_OUT_PORT));


#ifdef MATLAB_TEQ_SHAI_CONFIG
    open_matlab_TEQ_SHAI_file(proxy->getInstance());
#endif // MATLAB_TEQ_SHAI_CONFIG

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
    open_debug_TEQ_SHAI_file(proxy->getInstance());
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "TEQNmfMpc::configureAlgo\n");
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "p_pcmport_in->isEnabled()  = %d\n", (int) p_pcmport_in->isEnabled());
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "p_pcmport_out->isEnabled() = %d\n", (int) p_pcmport_out->isEnabled());
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "--------------------------------------------------\n");
    fflush(debug_TEQ_SHAI_file[proxy->getInstance()]);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1

    configureAlgoRequested = true;
    setParamDone           = false;
    doConfigureAlgo        = false;
    last_nSamplingRate     = -1;
    last_nChannels         = -1;
    if(p_pcmport_in->isEnabled() && p_pcmport_out->isEnabled())
    {
        OMX_ERRORTYPE error;

        error = proxy->getSamplingRateChannels(p_pcmport_in, p_pcmport_out, &nSamplingRate, &nChannels);
        if(error != OMX_ErrorNone)
        {
            return error;
        }

        if(nChannels != last_nChannels)
        {
            if(proxy->getDefaultParams() || (last_nChannels == -1))
            {
                last_nChannels = nChannels;
            }
            else
            {
                return OMX_ErrorPortsNotCompatible;
            }
        }

        return configureAlgo_port_enabled(proxy);
    }
    else
    {
        AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE *proxyParams = proxy->getTransducerParams();

        // set params with current params which may be default parameters !!!

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "TEQNmfMpc::configureAlgo with ports disabled => no verification that parameters are compliant with port settings\n");
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "--------------------------------------------------\n");
        fflush(debug_TEQ_SHAI_file[proxy->getInstance()]);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1

        nChannels = (proxyParams->stereo ? 2 : 1);

        return setProxyParams(proxy);
    }
}


OMX_ERRORTYPE TEQNmfMpc::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
    TransducerEqualizer *proxy         = static_cast<TransducerEqualizer *>(&mENSComponent);
    AFM_PcmPort         *p_pcmport_in  = static_cast<AFM_PcmPort *>(proxy->getPort(TREQ_IN_PORT));
    AFM_PcmPort         *p_pcmport_out = static_cast<AFM_PcmPort *>(proxy->getPort(TREQ_OUT_PORT));


#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "TEQNmfMpc::applyConfig\n");
    switch(nConfigIndex)
    {
        case AFM_IndexConfigTransducerEqualizer:
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "nConfigIndex = AFM_IndexConfigTransducerEqualizer\n");
            break;
        case OMX_Symbian_IndexConfigAudioTransEqualizer:
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "nConfigIndex = OMX_Symbian_IndexConfigAudioTransEqualizer\n");
            break;
        case OMX_Symbian_IndexConfigAudioTransEqualizer_iireq:
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "nConfigIndex = OMX_Symbian_IndexConfigAudioTransEqualizer_iireq\n");
            break;
        case OMX_Symbian_IndexConfigAudioTransEqualizer_firresponse:
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "nConfigIndex = OMX_Symbian_IndexConfigAudioTransEqualizer_firresponse\n");
            break;
        default:
            fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "nConfigIndex = %d\n", (int) nConfigIndex);
            break;
    }
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "p_pcmport_in->isEnabled()  = %d\n", (int) p_pcmport_in->isEnabled());
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "p_pcmport_out->isEnabled() = %d\n", (int) p_pcmport_out->isEnabled());
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "--------------------------------------------------\n");
    fflush(debug_TEQ_SHAI_file[proxy->getInstance()]);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1

    if(p_pcmport_in->isEnabled() && p_pcmport_out->isEnabled())
    {
        OMX_ERRORTYPE error;

        error = proxy->getSamplingRateChannels(p_pcmport_in, p_pcmport_out, &nSamplingRate, &nChannels);
        if(error != OMX_ErrorNone)
        {
            return error;
        }

        if(nChannels != last_nChannels)
        {
            // check if channel change is authorized
            if(proxy->getDefaultParams() || (last_nChannels == -1))
            {
                last_nChannels = nChannels;
            }
            else
            {
                return OMX_ErrorPortsNotCompatible;
            }
        }

        switch(nConfigIndex)
        {
            case AFM_IndexConfigTransducerEqualizer:
            {
                TransducerEqualizerConfig_t *p_configMpc;
                t_uint16 volatile           *p_read_index;


                if(p_TransducerEqualizerConfigStructFifo == NULL)
                {
                    t_cm_system_address ConfigStructAddress;
                    t_cm_error          cmErr;

                    error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(),
                                                AFM::memoryBank(MEM_DDR16),
                                                sizeof(TransducerEqualizerConfigStructFifo_t),
                                                CM_MM_ALIGN_WORD,
                                                &ConfigStructHandle);
                    if(error != OMX_ErrorNone)
                    {
                        return error;
                    }

                    cmErr = CM_GetMpcMemorySystemAddress(ConfigStructHandle, &ConfigStructAddress);
                    if(cmErr != CM_OK)
                    {
                        ENS::freeMpcMemory(ConfigStructHandle);
                        p_TransducerEqualizerConfigStructFifo = NULL;
                        return OMX_ErrorInsufficientResources;
                    }

                    cmErr = CM_GetMpcMemoryMpcAddress(ConfigStructHandle, &ConfigStructDspAddress);
                    if(cmErr != CM_OK)
                    {
                        ENS::freeMpcMemory(ConfigStructHandle);
                        p_TransducerEqualizerConfigStructFifo = NULL;
                        return OMX_ErrorInsufficientResources;
                    }

                    p_TransducerEqualizerConfigStructFifo              = (TransducerEqualizerConfigStructFifo_t *) ConfigStructAddress.logical;
                    p_TransducerEqualizerConfigStructFifo->write_index = TEQ_FIFO_CONFIG_SIZE - 1;
                    p_TransducerEqualizerConfigStructFifo->read_index  = 0;
                    // remark : TransducerEqualizerConfigStructFifo is not cached on host side
                }

                // when FIFO is full : wait for mpc to read at least one config
                p_read_index = (t_uint16 volatile *) (&p_TransducerEqualizerConfigStructFifo->read_index);
                while(*p_read_index == p_TransducerEqualizerConfigStructFifo->write_index)
                    ;

                // increment write index
                if(p_TransducerEqualizerConfigStructFifo->write_index == TEQ_FIFO_CONFIG_SIZE - 1)
                {
                    p_TransducerEqualizerConfigStructFifo->write_index = 0;
                }
                else
                {
                    p_TransducerEqualizerConfigStructFifo->write_index++;
                }

                p_configMpc = &(p_TransducerEqualizerConfigStructFifo->config[p_TransducerEqualizerConfigStructFifo->write_index]);
                error = proxy->getEffectConfig_IndexConfigTransducerEqualizer(nChannels, nSamplingRate, p_configMpc);
                if(error == OMX_ErrorNone)
                {
#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
                    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "mIconfigure.setConfig(configMpc)\n");
                    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "--------------------------------------------------\n");
                    fflush(debug_TEQ_SHAI_file[proxy->getInstance()]);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
                    mIconfigure.setConfig(ConfigStructDspAddress);
                }
                break;
            }


            case OMX_Symbian_IndexConfigAudioTransEqualizer:
                error = proxy->getEffectConfig_OMX_IndexConfigAudioTransEqualizer(nChannels);
                if(error == OMX_ErrorNone)
                {
                    if(!doConfigureAlgo)
                    {
                        error = applyConfig((OMX_INDEXTYPE) AFM_IndexConfigTransducerEqualizer, pComponentConfigStructure);
                    }
                }
                break;


            case OMX_Symbian_IndexConfigAudioTransEqualizer_iireq:
                error = proxy->getEffectConfig_OMX_IndexConfigAudioTransEqualizer_iireq(nChannels, nSamplingRate);
                if(error == OMX_ErrorNone)
                {
                    if(!doConfigureAlgo)
                    {
                        error = applyConfig((OMX_INDEXTYPE) OMX_Symbian_IndexConfigAudioTransEqualizer, pComponentConfigStructure);
                    }
                }
                break;


            case OMX_Symbian_IndexConfigAudioTransEqualizer_firresponse:
                error = proxy->getEffectConfig_OMX_IndexConfigAudioTransEqualizer_firresponse(nChannels, nSamplingRate);
                if(error == OMX_ErrorNone)
                {
                    if(!doConfigureAlgo)
                    {
                        error = applyConfig((OMX_INDEXTYPE) OMX_Symbian_IndexConfigAudioTransEqualizer, pComponentConfigStructure);
                    }
                }
                break;


            default:
                error = AFMNmfMpc_PcmProcessing::applyConfig(nConfigIndex, pComponentConfigStructure);
                break;
        }

        return error;
    }
    else
    {
        switch(nConfigIndex)
        {
            case AFM_IndexConfigTransducerEqualizer:
            case OMX_Symbian_IndexConfigAudioTransEqualizer:
            case OMX_Symbian_IndexConfigAudioTransEqualizer_iireq:
            case OMX_Symbian_IndexConfigAudioTransEqualizer_firresponse:
                return OMX_ErrorNone;

            default:
                return AFMNmfMpc_PcmProcessing::applyConfig(nConfigIndex, pComponentConfigStructure);
        }
    }
}
