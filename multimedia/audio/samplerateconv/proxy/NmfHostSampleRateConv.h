/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfHostSampleRateConv.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _NMFHOSTSAMPLERATECONV_H_
#define _NMFHOSTSAMPLERATECONV_H_

#include "NmfHostPcmProcessing.h"

#include "samplerateconv/nmfil/host/effect/configure.hpp"

class NmfHostSampleRateConv: public NmfHostPcmProcessing{
public:
  NmfHostSampleRateConv(t_host_pcmprocessing_config &config);
    
    virtual ~NmfHostSampleRateConv(void);
    virtual OMX_ERRORTYPE instantiateAlgo(void);
    virtual OMX_ERRORTYPE startAlgo(void);
    virtual OMX_ERRORTYPE stopAlgo(void);
    virtual OMX_ERRORTYPE configureAlgo(void);
    virtual OMX_ERRORTYPE deInstantiateAlgo(void);

    OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex,  
                              OMX_PTR pComponentConfigStructure);
private:
    /***********************************************************/
    // TODO FIXME: understand what this member is used for ;-)
    /***********************************************************/
    OMX_U32                                 mLowMipsToOut48;
    
    //    OMX_U32                                 mSampleRateIn;
    //    OMX_U32                                 mSampleRateOut;
    //    OMX_U32                                 mBlockSize;
    //    OMX_U32                                 mChannels;

    Isamplerateconv_nmfil_host_effect_configure mIconfigure;
};

#endif //_NMFHOSTSAMPLERATECONV_H_
