/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   TransducerEqualizer.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _TRANSDUCER_EQUALIZER_H_
#define _TRANSDUCER_EQUALIZER_H_

#include "AFM_PcmProcessing.h"
#include "audio_chipset_api_index.h"
#include "audio_transducer_chipset_api.h"

#ifndef HOST_ONLY
#include "host/transducer_equalizer/nmfil/effect/configure.hpp"
#endif // !HOST_ONLY
#include "host/transducer_equalizer/nmfil/host/effect/configure.hpp"

#include "TEQ_SHAI.h"

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
#include <los/api/los_api.h>
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0


typedef enum {USE_PROPRIETARY_CONFIG, USE_SHAI_CONFIG, USE_TUNING_DATA_CONFIG} t_config_use;

typedef enum {TEQ_BIQUAD_CELL_RESET, TEQ_BIQUAD_CELL_CONFIG, TEQ_BIQUAD_CELL_VALID, TEQ_BIQUAD_CELL_COMPUTED} t_ConfigAudioTransEqualizer_iireq_status;

#define TREQ_IN_PORT                    0
#define TREQ_OUT_PORT                   1
#define TREQ_DEFAULT_NB_CHANNEL         2
#define TREQ_DEFAULT_SAMPLING_FREQUENCY 48000
#define TREQ_DEFAULT_NB_BITS_PER_SAMPLE 16


/// @defgroup transducerequalizer
/// @ingroup  afmpcmprocessing
/// @{

/// Transducer Equalizer factory method used when registering the TransducerEqualizer
/// component to ENS Core
#ifndef HOST_ONLY
OMX_ERRORTYPE transducer_equalizerFactoryMethod      (ENS_Component_p *ppENSComponent);
#endif /* !HOST_ONLY */
OMX_ERRORTYPE transducer_equalizer_host_FactoryMethod(ENS_Component_p *ppENSComponent);

class TransducerEqualizer_RDB : public ENS_ResourcesDB
{
    public:
        TransducerEqualizer_RDB(OMX_U32 nbOfDomains);
};

/// Concrete class implementing a TransducerEqualizer component
/// Derived from AFM_PcmProcessing
class TransducerEqualizer : public AFM_PcmProcessing
{
    public:
        /// Constructor
        TransducerEqualizer(bool isHost) {mIsHost = isHost; defaultParams = true; nb_bits_quantif = (isHost ? 32 : 24); instance++;};
        OMX_ERRORTYPE construct(void);

        /// Destructor
        ~TransducerEqualizer(void) {instance--;};

        virtual OMX_ERRORTYPE createResourcesDB(void);
        virtual OMX_ERRORTYPE setParameter     (OMX_INDEXTYPE nParamIndex,  OMX_PTR pComponentParameterStructure);
        virtual OMX_ERRORTYPE getParameter     (OMX_INDEXTYPE nParamIndex,  OMX_PTR pComponentParameterStructure) const;
        virtual OMX_ERRORTYPE setConfig        (OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
        virtual OMX_ERRORTYPE getConfig        (OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const;

        virtual OMX_U32 getUidTopDictionnary   (void);

        bool          getDefaultParams                                              (void) {return defaultParams;};
        OMX_ERRORTYPE getSamplingRateChannels                                       (AFM_PcmPort *p_pcmport_in, AFM_PcmPort *p_pcmport_out, int *p_nSamplingRate, int *p_nChannels);
        void          checkSHAIFilterUpdate                                         (int nChannels);
        OMX_ERRORTYPE getEffectParams                                               (int nChannels, void *p_equalizerParams_void);
        OMX_ERRORTYPE getEffectConfig_IndexConfigTransducerEqualizer                (int nChannels, int nSamplingRate, void *p_equalizerConfig_void);
        OMX_ERRORTYPE getEffectConfig_OMX_IndexConfigAudioTransEqualizer            (int nChannels);
        OMX_ERRORTYPE getEffectConfig_OMX_IndexConfigAudioTransEqualizer_iireq      (int nChannels, int nSamplingRate);
        OMX_ERRORTYPE getEffectConfig_OMX_IndexConfigAudioTransEqualizer_firresponse(int nChannels, int nSamplingRate);

        inline AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE * getTransducerParams(void) const
        {
            return (AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE *) &mTransducerEqualizerParams;
        }

        inline AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE * getTransducerConfig(void) const
        {
            return (AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE *) &mTransducerEqualizerConfig;
        }

        inline t_config_use getConfigUse(void) const
        {
            return mConfigUse;
        }

        inline int getInstance(void) const
        {
            return instance;
        }


    private:
        static int                               instance;
        bool                                     mIsHost;
        bool                                     defaultParams;
        int                                      nb_bits_quantif;
        AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE  mTransducerEqualizerParams;
        OMX_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE mOMX_ConfigAudioTransEqualizer[2];
        OMX_AUDIO_CONFIG_IIR_EQUALIZER           mOMX_ConfigAudioTransEqualizer_iireq[2][NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS];
        OMX_AUDIO_CONFIG_FIR_RESPONSE            mOMX_ConfigAudioTransEqualizer_firresponse[2];
        AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE mTransducerEqualizerConfig;
        t_config_use                             mConfigUse;
        t_ConfigAudioTransEqualizer_iireq_status ConfigAudioTransEqualizer_iireq_status[2][NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS];

        t_IIR_filter_float                       IIR_filter_float[2];

        t_IIR_filter_quantified                  IIR_filter_quantified[2];
        t_FIR_filter_quantified                  FIR_filter_quantified[2];
};
/// @}
#endif //  _TRANSDUCER_EQUALIZER_H_
