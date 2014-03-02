/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Encoder nmf processing class headers
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _AMR_ENCNMFPROCESSINGCOMP_H_
#define _AMR_ENCNMFPROCESSINGCOMP_H_

#include "AFMNmfMpc_Encoder.h"
#include "AMREncAlgo.h"

/// Concrete class impl`ementing a AMR decoder processing component
/// Derived from AFM_EncNmfProcessingComp.
class AMRNmfMpc_Encoder: public AFMNmfMpc_Encoder{
public:

    AMRNmfMpc_Encoder(ENS_Component &enscomp):
        AFMNmfMpc_Encoder(enscomp) {};

    virtual ~AMRNmfMpc_Encoder() {};

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

    AMREncAlgo * mAlgo;

    void registerStubsAndSkels(void);
    void unregisterStubsAndSkels(void);

};

#endif // _AMR_HOSTENCNMFPROCESSINGCOMP_H_
