/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Host Encoder Proxy Port class headers
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _NMF_AMR_HOST_ENC_ALGO_H_
#define _NMF_AMR_HOST_ENC_ALGO_H_

#include "NmfComponent.h"
#include "amr/nmfil/host/encoder/configure.hpp"

class AMRHostEncAlgo : public NmfComponent {
    public:
        AMRHostEncAlgo() {};
        virtual ~AMRHostEncAlgo() {};

        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);
        virtual OMX_ERRORTYPE deInstantiate();
        virtual OMX_ERRORTYPE start();
        virtual OMX_ERRORTYPE stop();

        virtual OMX_ERRORTYPE configure() {return OMX_ErrorNone;}

        OMX_ERRORTYPE setParameter(AmrEncParam_t amrNmfParams);
        OMX_ERRORTYPE setConfig(AmrEncConfig_t amrNmfconfig);

    private :
	    Iamr_nmfil_host_encoder_configure  mIConfig;
};


#endif /* _NMF_AMR_HOST_ENC_ALGO_H_ */
