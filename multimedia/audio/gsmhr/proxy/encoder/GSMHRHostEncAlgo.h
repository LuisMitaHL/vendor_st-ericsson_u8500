/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _NMF_GSMHR_HOST_ENC_ALGO_H_
#define _NMF_GSMHR_HOST_ENC_ALGO_H_

#include "NmfComponent.h"
#include "gsmhr/nmfil/host/encoder/configure.hpp"

class GSMHRHostEncAlgo : public NmfComponent {
public:
        GSMHRHostEncAlgo() {};
        virtual ~GSMHRHostEncAlgo() {};

        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);

        virtual OMX_ERRORTYPE deInstantiate();
        virtual OMX_ERRORTYPE start();
        virtual OMX_ERRORTYPE stop();

        virtual OMX_ERRORTYPE configure() {return OMX_ErrorNone;}

        OMX_ERRORTYPE setParameter(GsmhrEncParams_t gsmhrNmfParams);
        OMX_ERRORTYPE setConfig(GsmhrEncConfig_t gsmhrNmfConfig);

    private :
	    Igsmhr_nmfil_host_encoder_configure  mIConfig;
};


#endif /* _NMF_GSMHR_HOST_ENC_ALGO_H_ */
