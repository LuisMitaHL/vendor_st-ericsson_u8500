/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _NMF_GSMFR_HOST_ENC_ALGO_H_
#define _NMF_GSMFR_HOST_ENC_ALGO_H_

#include "NmfComponent.h"
#include "gsmfr/nmfil/host/encoder/configure.hpp"

class GSMFRHostEncAlgo : public NmfComponent {
public:
        GSMFRHostEncAlgo() {};
        virtual ~GSMFRHostEncAlgo() {};

        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);

        // deInstantiate the NMF components
        virtual OMX_ERRORTYPE deInstantiate();

        // start the NMF components
        virtual OMX_ERRORTYPE start();

        // stop the NMF components
        virtual OMX_ERRORTYPE stop();

        virtual OMX_ERRORTYPE configure() {return OMX_ErrorNone;}

        OMX_ERRORTYPE setParameter(NmfGsmfrEncParams_t gsmfrNmfParams);
        OMX_ERRORTYPE setConfig(NmfGsmfrEncConfig_t gsmfrNmfConfig);
        
private:

        Igsmfr_nmfil_host_encoder_configure	mIConfig;

};

/// @}

#endif /* _NMF_GSMFR_ENC_ALGO_H_ */


