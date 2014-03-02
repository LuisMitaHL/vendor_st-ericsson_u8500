/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#ifndef _samplerateconv_nmfil_host_effect_hpp_
#define _samplerateconv_nmfil_host_effect_hpp_

extern "C" {
#include "libeffects/libresampling/include/resample.h"
}

class samplerateconv_nmfil_host_effect : public samplerateconv_nmfil_host_effectTemplate
{
public:
    samplerateconv_nmfil_host_effect();
    virtual ~samplerateconv_nmfil_host_effect();

    // Effect interface...
    virtual t_bool open(const t_host_effect_config* config, t_effect_caps* caps);
    virtual void reset(t_effect_reset_reason reason);
    virtual void process(t_effect_process_params* params);
    virtual void close(void);
    virtual void getSampleSize(t_uint32 *sampleIn,t_uint32 *sampleOut);

    // Methods provided by interface samplerateconv.nmfil.host.effect.configure:
    virtual void setParameter(t_uint16 lowmips_to_out48);

private:

    ResampleContext  mResampleContext;
    t_host_effect_config  m_config;
    t_uint16         mLowMipsToOut48;
    char *           mHeap;
    int              mHeapSize;
    int*           mInBuf_p;
    int*           mOutBuf_p;

};

#endif // _samplerateconv_nmfil_host_effect_hpp_
