/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Host Decoder nmf processing class headers
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _SRC_NMF_HOST_PCM_PROCESSING_H_
#define _SRC_NMF_HOST_PCM_PROCESSING_H_
//#define NEWAB
#include "AFMNmfHost_PcmProcessing.h"
#include "samplerateconv/nmfil/host/effect/configure.hpp"

/// Concrete class implementing SRC processing component
class SRCNmfHost_PcmProcessing: public AFMNmfHost_PcmProcessing {
public:

    SRCNmfHost_PcmProcessing(ENS_Component &enscomp) 
            : AFMNmfHost_PcmProcessing(enscomp) {}

    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);
#ifdef NEWAB
	void setParam(OMX_U32 nMode);
#endif
protected:

    virtual OMX_ERRORTYPE instantiateAlgo(void);
    virtual OMX_ERRORTYPE startAlgo(void);
    virtual OMX_ERRORTYPE stopAlgo(void);
    virtual OMX_ERRORTYPE configureAlgo(void);
    virtual OMX_ERRORTYPE deInstantiateAlgo(void);
    virtual OMX_U32       nbBitPerSampleProcessed(); // Should be 16 or 32

private:
#ifdef NEWAB
    OMX_U32                                 mMode;
#endif

    Isamplerateconv_nmfil_host_effect_configure  mIConfig;
};

#endif // _SRC_NMF_HOST_PCM_PROCESSING_H_
