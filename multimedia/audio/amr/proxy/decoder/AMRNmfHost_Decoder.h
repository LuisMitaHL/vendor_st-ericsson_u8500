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

#ifndef _AMR_HOSTDECNMFPROCESSINGCOMP_H_
#define _AMR_HOSTDECNMFPROCESSINGCOMP_H_

#include "AFMNmfHost_Decoder.h"
#include "AMRHostDecAlgo.h"

/// Concrete class impl`ementing a AMR decoder processing component
/// Derived from AFM_DecNmfProcessingComp.
class AMRNmfHost_Decoder: public AFMNmfHost_Decoder{
public:

    AMRNmfHost_Decoder(ENS_Component &enscomp) 
            : AFMNmfHost_Decoder(enscomp),mAlgo(0){}

    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);

protected:

    virtual OMX_ERRORTYPE instantiateAlgo(void);
    virtual OMX_ERRORTYPE startAlgo(void);
    virtual OMX_ERRORTYPE stopAlgo(void);
    virtual OMX_ERRORTYPE configureAlgo(void);
    virtual OMX_ERRORTYPE deInstantiateAlgo(void);

private:

    AMRHostDecAlgo * mAlgo;
};

#endif // _AMR_HOSTDECNMFPROCESSINGCOMP_H_
