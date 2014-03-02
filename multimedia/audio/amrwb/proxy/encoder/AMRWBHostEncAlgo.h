/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _NMF_AMRWB_HOST_ENC_ALGO_H_
#define _NMF_AMRWB_HOST_ENC_ALGO_H_

#include "NmfComponent.h"
#include "amrwb/nmfil/host/encoder/configure.hpp"

class AMRWBHostEncAlgo : public NmfComponent {
    public:
        AMRWBHostEncAlgo() {};
        virtual ~AMRWBHostEncAlgo() {};

        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);
        virtual OMX_ERRORTYPE deInstantiate();
        virtual OMX_ERRORTYPE start();
        virtual OMX_ERRORTYPE stop();

        virtual OMX_ERRORTYPE configure() {return OMX_ErrorNone;}

        OMX_ERRORTYPE setParameter(AmrwbEncParam_t amrwbNmfParams);
        OMX_ERRORTYPE setConfig(AmrwbEncConfig_t amrwbNmfconfig);

    private :
	    Iamrwb_nmfil_host_encoder_configure  mIConfig;
};


#endif /* _NMF_AMRWB_HOST_ENC_ALGO_H_ */
