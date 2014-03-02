/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Host Decoder Algo class headers
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _NMF_AMR_HOST_DEC_ALGO_H_
#define _NMF_AMR_HOST_DEC_ALGO_H_

#include "NmfComponent.h"

#include "amr/nmfil/host/decoder/configure.hpp"

class AMRHostDecAlgo : public NmfComponent {
   public:
        AMRHostDecAlgo() {};
        virtual ~AMRHostDecAlgo() {};

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
        OMX_ERRORTYPE setParameter(AmrDecParams_t amrNmfParams);
        OMX_ERRORTYPE setConfig(AmrDecConfig_t amrNmfConfig);

    private :
	    Iamr_nmfil_host_decoder_configure  mIConfig;
};


#endif /* _NMF_AMR_HOST_DEC_ALGO_H_ */
