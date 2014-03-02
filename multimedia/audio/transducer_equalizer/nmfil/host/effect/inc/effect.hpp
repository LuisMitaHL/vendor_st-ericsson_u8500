/*****************************************************************************/
/**
 *  (c) ST-Ericsson, 2009 - All rights reserved
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson
 *
 * \file   wrapper.hpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _transducer_equalizer_effect_hpp_
#define _transducer_equalizer_effect_hpp_
#include "common_interface.h"
#include "libeffects/libtransducer_equalizer/include/transducer_equalizer_CA9.h"
#include "libeffects/libtransducer_equalizer/include/effect_common.h"


class transducer_equalizer_nmfil_host_effect : public transducer_equalizer_nmfil_host_effectTemplate
{
public:
    transducer_equalizer_nmfil_host_effect();
    virtual ~transducer_equalizer_nmfil_host_effect();

    // Methods provided
    virtual void   start(void);
    virtual void   stop(void);
    virtual void   close(void);
    virtual t_bool open(const t_host_effect_config *config, t_effect_caps *caps);
    virtual void   reset(t_effect_reset_reason reason);
    virtual void   setParameter(TransducerEqualizerParams_t &StaticParams, t_uint16 instance_uint16);
    virtual void   setConfig(TransducerEqualizerConfig_t &DynamicParams);
    virtual void   process(t_effect_process_params *params);
    virtual void   newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size, t_uint8 idx);


private:
    MMshort fract_ratio(MMshort num, MMshort den);
    void    transition1_processBuffer(void **p_p_inbuf, void **p_p_outbuf, int nb_samples, int offset_shift);
    void    transition2_processBuffer(void **p_p_inbuf, void **p_p_outbuf, int nb_samples, int offset_shift);

    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T  mTrEqStruct;
    TransducerEqualizerConfig_t          mTrEqConfig;
    t_effect_process_type                effect_process_type_cap;
    t_host_effect_config                 host_effect_config;
    transducer_equalizer_smooth_config_t smooth_config;
    int                                  instance;
    int                                  buf1_sample32_size;
    int                                  buf2_sample32_size;
    int                                  *p_buf1_sample32;
    int                                  *p_buf2_sample32;
};

#endif // _transducer_equalizer_effect_hpp_
