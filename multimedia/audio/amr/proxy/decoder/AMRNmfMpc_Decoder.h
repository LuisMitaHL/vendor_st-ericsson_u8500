/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Decoder nmf processing class headers
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _AMR_DECNMFPROCESSINGCOMP_H_
#define _AMR_DECNMFPROCESSINGCOMP_H_

#include "AFMNmfMpc_Decoder.h"
#include "AMRDecAlgo.h"

/// Concrete class impl`ementing a AMR decoder processing component
/// Derived from AFM_DecNmfProcessingComp.
class AMRNmfMpc_Decoder: public AFMNmfMpc_Decoder {
public:

    AMRNmfMpc_Decoder(ENS_Component &enscomp):
        AFMNmfMpc_Decoder(enscomp) {}

    virtual ~AMRNmfMpc_Decoder() {};

    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);

protected:

    virtual OMX_ERRORTYPE instantiateAlgo(void);
    virtual OMX_ERRORTYPE startAlgo(void);
    virtual OMX_ERRORTYPE stopAlgo(void);
    virtual OMX_ERRORTYPE configureAlgo(void);
    virtual OMX_ERRORTYPE deInstantiateAlgo(void);

private:

    AMRDecAlgo * mAlgo;
    void registerStubsAndSkels(void);
    void unregisterStubsAndSkels(void);

};

#endif // _AMR_DECNMFPROCESSINGCOMP_H_
