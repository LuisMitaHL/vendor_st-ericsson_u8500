/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Host Encoder nmf processing class headers
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _AMR_HOSTENCNMFPROCESSINGCOMP_H_
#define _AMR_HOSTENCNMFPROCESSINGCOMP_H_

#include "AFMNmfHost_Encoder.h"
#include "AMRHostEncAlgo.h"

/// Concrete class implementing a AMR encoder processing component
/// Derived from AFM_EncNmfProcessingComp.
class AMRNmfHost_Encoder: public AFMNmfHost_Encoder{
public:

    AMRNmfHost_Encoder(ENS_Component &enscomp) 
            : AFMNmfHost_Encoder(enscomp),mAlgo(0){}

    virtual ~AMRNmfHost_Encoder () {};

    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);

    OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

protected:

    virtual OMX_ERRORTYPE instantiateAlgo(void);
    virtual OMX_ERRORTYPE startAlgo(void);
    virtual OMX_ERRORTYPE stopAlgo(void);
    virtual OMX_ERRORTYPE configureAlgo(void);
    virtual OMX_ERRORTYPE deInstantiateAlgo(void);

private:

    AMRHostEncAlgo * mAlgo;
};

#endif // _AMR_HOSTENCNMFPROCESSINGCOMP_H_
