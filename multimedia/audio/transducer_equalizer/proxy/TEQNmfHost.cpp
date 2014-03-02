/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   TEQNmfHost.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <string.h>
#include "TEQNmfHost.h"
#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "transducer_equalizer/nmfil/host/composite/effect.hpp"


OMX_ERRORTYPE TEQNmfHost::construct(void)
{
    return AFMNmfHost_PcmProcessing::construct();
}


OMX_ERRORTYPE TEQNmfHost::destroy(void)
{
    return AFMNmfHost_PcmProcessing::destroy();
}


OMX_ERRORTYPE TEQNmfHost::instantiateAlgo(void)
{
    t_nmf_error nmf_error;


    mNmfil = (NMF::Composite *) transducer_equalizer_nmfil_host_composite_effectCreate();

    if(((NMF::Composite *) mNmfil) == NULL)
    {
       return OMX_ErrorInsufficientResources;
    }

    if(((NMF::Composite *) mNmfil)->construct() != NMF_OK)
    {
       return OMX_ErrorInsufficientResources;
    }


    nmf_error = ((NMF::Composite *) mNmfil)->bindFromUser("configure", 2, &mIconfigureHost);

    if(nmf_error != NMF_OK)
    {
       return OMX_ErrorInsufficientResources;
    }

    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfHost::deInstantiateAlgo(void)
{
#if defined(MATLAB_TEQ_SHAI_CONFIG) || (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0)
    TransducerEqualizer *proxy = static_cast<TransducerEqualizer *>(&mENSComponent);
#endif // MATLAB_TEQ_SHAI_CONFIG || (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0)
    t_nmf_error         nmf_error;


    nmf_error = ((NMF::Composite *) mNmfil)->unbindFromUser("configure");

    if(nmf_error != NMF_OK)
    {
       return OMX_ErrorInsufficientResources;
    }

    if(((NMF::Composite *) mNmfil)->destroy() != NMF_OK)
    {
       return OMX_ErrorInsufficientResources;
    }

    transducer_equalizer_nmfil_host_composite_effectDestroy((transducer_equalizer_nmfil_host_composite_effect *&) mNmfil);

#ifdef MATLAB_TEQ_SHAI_CONFIG
    close_matlab_TEQ_SHAI_file(proxy->getInstance());
#endif // MATLAB_TEQ_SHAI_CONFIG

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
    close_debug_TEQ_SHAI_file(proxy->getInstance());
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0

    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfHost::startAlgo(void)
{
    ((NMF::Composite *) mNmfil)->start();

    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfHost::stopAlgo(void)
{
    ((NMF::Composite *) mNmfil)->stop();

    return OMX_ErrorNone;
}


OMX_U32 TEQNmfHost::nbBitPerSampleProcessed(void)
{
    return 32;
}


OMX_ERRORTYPE TEQNmfHost::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
    if(configureAlgoRequested)
    {
        TransducerEqualizer *proxy         = static_cast<TransducerEqualizer *>(&mENSComponent);
        AFM_PcmPort         *p_pcmport_in  = static_cast<AFM_PcmPort         *>(proxy->getPort(TREQ_IN_PORT));
        AFM_PcmPort         *p_pcmport_out = static_cast<AFM_PcmPort         *>(proxy->getPort(TREQ_OUT_PORT));


#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
        fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "TEQNmfHost::doSpecificSendCommand\n");
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

    return AFMNmfHost_PcmProcessing::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}


OMX_ERRORTYPE TEQNmfHost::setProxyParams(TransducerEqualizer *proxy)
{
    TransducerEqualizerParams_t TransducerEqualizerParamsHost;
    OMX_ERRORTYPE               error;


    error = proxy->getEffectParams(nChannels, &TransducerEqualizerParamsHost);
    if(error != OMX_ErrorNone)
    {
        return error;
    }

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "mIconfigureHost.setParameter(TransducerEqualizerParamsHost, %d)\n", proxy->getInstance());
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "--------------------------------------------------\n");
    fflush(debug_TEQ_SHAI_file[proxy->getInstance()]);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    mIconfigureHost.setParameter(TransducerEqualizerParamsHost, proxy->getInstance());
    paramsNbChannels = (TransducerEqualizerParamsHost.istereo ? 2 : 1);
    setParamDone     = true;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE TEQNmfHost::configureAlgo_port_enabled(TransducerEqualizer *proxy)
{
    OMX_ERRORTYPE error;


#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "TEQNmfHost::configureAlgo_port_enabled\n");
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


OMX_ERRORTYPE TEQNmfHost::configureAlgo(void)
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
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "TEQNmfHost::configureAlgo\n");
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


OMX_ERRORTYPE TEQNmfHost::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
    TransducerEqualizer *proxy         = static_cast<TransducerEqualizer *>(&mENSComponent);
    AFM_PcmPort         *p_pcmport_in  = static_cast<AFM_PcmPort *>(proxy->getPort(TREQ_IN_PORT));
    AFM_PcmPort         *p_pcmport_out = static_cast<AFM_PcmPort *>(proxy->getPort(TREQ_OUT_PORT));


#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "TEQNmfHost::applyConfig\n");
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
                TransducerEqualizerConfig_t TransducerEqualizerConfigHost;


                error = proxy->getEffectConfig_IndexConfigTransducerEqualizer(nChannels, nSamplingRate, &TransducerEqualizerConfigHost);
                if(error == OMX_ErrorNone)
                {
#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
                    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "mIconfigureHost.setConfig(TransducerEqualizerConfigHost)\n");
                    fprintf(debug_TEQ_SHAI_file[proxy->getInstance()], "--------------------------------------------------\n");
                    fflush(debug_TEQ_SHAI_file[proxy->getInstance()]);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
                    mIconfigureHost.setConfig(TransducerEqualizerConfigHost);
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
                error = AFMNmfHost_PcmProcessing::applyConfig(nConfigIndex, pComponentConfigStructure);
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
                return AFMNmfHost_PcmProcessing::applyConfig(nConfigIndex, pComponentConfigStructure);
        }
    }
}
