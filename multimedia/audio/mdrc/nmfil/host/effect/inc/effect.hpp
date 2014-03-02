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
#ifndef _mdrc_effect_hpp_
#define _mdrc_effect_hpp_
#include "common_interface.h"
#include "libeffects/libmdrc5b/include/mdrc5b.h"


class mdrc_nmfil_host_effect : public mdrc_nmfil_host_effectTemplate
{
public:
    mdrc_nmfil_host_effect();
    virtual ~mdrc_nmfil_host_effect();

    // Methods provided
    virtual void   start(void);
    virtual void   stop(void);
    virtual void   close(void);
    virtual t_bool open(const t_host_effect_config *config, t_effect_caps *caps);
    virtual void   reset(t_effect_reset_reason reason);
    virtual void   setParameter(MdrcParams_t &StaticParams);
    virtual void   setConfig(MdrcConfig_t &DynamicParams);
    virtual void   process(t_effect_process_params *params);
    virtual void   newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size, t_uint8 idx);


private:
    void memoryAlloc(t_mdrc_filter_kind mdrc_filter_kind,
                     t_biquad_kind      biquad_kind,
                     t_FIR_kind         FIR_kind,
                     int                NumMainCh,
                     int                SampleFreq);

    MDRC5B_LOCAL_STRUCT_T mMdrcLocalStruct;
    MdrcConfig_t          mMdrc5bConfig;
    Mdrc5bAlgoConfig_t    mMdrc5bAlgoConfig;
    t_effect_process_type effect_process_type_cap;
    t_host_effect_config  host_effect_config;
    int                   param_received;
    int                   config_received;
    int                   open_received;
    int                   alloc_done;
};

#endif // _mdrc_effect_hpp_
