/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _NMF_AAC_HOST_ENC_ALGO_H_
#define _NMF_AAC_HOST_ENC_ALGO_H_

#include "NmfComponent.h"
#include "aacenc/nmfil/host/encoder/configure.hpp"

class AACHostEncAlgo : public NmfComponent {
    public:
        AACHostEncAlgo() {};
        virtual ~AACHostEncAlgo() {};

        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);
        virtual OMX_ERRORTYPE deInstantiate();
        virtual OMX_ERRORTYPE start();
        virtual OMX_ERRORTYPE stop();

        virtual OMX_ERRORTYPE configure() {return OMX_ErrorNone;}

        OMX_ERRORTYPE setParameter(AacEncParam_t aacNmfParams);
        //OMX_ERRORTYPE setConfig(AacEncConfig_t aacNmfconfig);

    private :
	    Iaacenc_nmfil_host_encoder_configure  mIConfig;
};


#endif /* _NMF_AAC_HOST_ENC_ALGO_H_ */
