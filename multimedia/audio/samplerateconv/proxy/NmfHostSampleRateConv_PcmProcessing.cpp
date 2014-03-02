/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Host Decoder nmf processing class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "NmfHostSampleRateConv_PcmProcessing.h"
#include "samplerateconv/nmfil/host/effectWrapped.hpp"


inline OMX_ERRORTYPE
SRCNmfHost_PcmProcessing::construct(void)
{
    return (AFMNmfHost_PcmProcessing::construct());
}
#ifdef NEWAB
void SRCNmfHost_PcmProcessing::setParam(OMX_U32 nMode) 
{
    mMode = nMode;
}
#endif

inline OMX_ERRORTYPE 
SRCNmfHost_PcmProcessing::destroy(void)
{
    return (AFMNmfHost_PcmProcessing::destroy());
}

OMX_ERRORTYPE 
SRCNmfHost_PcmProcessing::instantiateAlgo(void) 
{
    t_nmf_error   nmf_error;

    mNmfil = (NMF::Composite*)samplerateconv_nmfil_host_effectWrappedCreate();
    if (((NMF::Composite*)mNmfil) == NULL) { return OMX_ErrorInsufficientResources; };
    ((samplerateconv_nmfil_host_effectWrapped *)(mNmfil))->priority=getPriorityLevel();
    if (((NMF::Composite*)mNmfil)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = ((NMF::Composite*)mNmfil)->bindFromUser("configure", 1, &mIConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
SRCNmfHost_PcmProcessing::deInstantiateAlgo(void) 
{

    t_nmf_error   nmf_error;
  
    nmf_error = ((NMF::Composite*)mNmfil)->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
    
    if (((NMF::Composite*)mNmfil)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    samplerateconv_nmfil_host_effectWrappedDestroy((samplerateconv_nmfil_host_effectWrapped*&)mNmfil);

    return OMX_ErrorNone;
}

OMX_U32       
SRCNmfHost_PcmProcessing::nbBitPerSampleProcessed(void)
{
	return 16;
}

OMX_ERRORTYPE 
SRCNmfHost_PcmProcessing::startAlgo(void) 
{
    ((NMF::Composite*)mNmfil)->start();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
SRCNmfHost_PcmProcessing::stopAlgo(void) 
{
    ((NMF::Composite*)mNmfil)->stop();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
SRCNmfHost_PcmProcessing::configureAlgo(void) 
{
#ifdef NEWAB
    mIConfig.setParameter(mMode); // complexity mode normal
#else
 	mIConfig.setParameter(0); // complexity mode normal
#endif
    return OMX_ErrorNone;
}

