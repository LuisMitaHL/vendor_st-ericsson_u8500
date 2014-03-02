/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _NMF_GSMFR_HOST_DEC_ALGO_H_
#define _NMF_GSMFR_HOST_DEC_ALGO_H_

#include "NmfComponent.h"

#include "gsmfr/nmfil/host/decoder/configure.hpp"

class GSMFRHostDecAlgo : public NmfComponent {
    public:
        GSMFRHostDecAlgo() {};
        virtual ~GSMFRHostDecAlgo() {};

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
        OMX_ERRORTYPE setParameter(NmfGsmfrDecParams_t gsmfrNmfParams);
        OMX_ERRORTYPE setConfig(NmfGsmfrDecConfig_t gsmfrNmfConfig);
        
    private :

        Igsmfr_nmfil_host_decoder_configure      mIConfig;


};


#endif /* _NMF_GSMFR_HOST_DEC_ALGO_H_ */

