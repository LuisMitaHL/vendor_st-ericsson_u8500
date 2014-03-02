/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _NMF_GSMHR_HOST_DEC_ALGO_H_
#define _NMF_GSMHR_HOST_DEC_ALGO_H_

#include "NmfComponent.h"

#include "gsmhr/nmfil/host/decoder/configure.hpp"

class GSMHRHostDecAlgo : public NmfComponent {
   public:
        GSMHRHostDecAlgo() {};
        virtual ~GSMHRHostDecAlgo() {};

        // instantiate the NMF components
        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);

        // deInstantiate the NMF components
        virtual OMX_ERRORTYPE deInstantiate();

        // start the NMF components
        virtual OMX_ERRORTYPE start();

        // Configure NMF component: already done in setParameter
        virtual OMX_ERRORTYPE configure() {return OMX_ErrorNone;};

        // stop the NMF components
        virtual OMX_ERRORTYPE stop();
        OMX_ERRORTYPE setParameter(GsmhrDecParams_t gsmhrNmfParams);
        OMX_ERRORTYPE setConfig(GsmhrDecConfig_t gsmfrNmfConfig);

    private :
	    Igsmhr_nmfil_host_decoder_configure  mIConfig;
};


#endif /* _NMF_GSMHR_HOST_DEC_ALGO_H_ */
