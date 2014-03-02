/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _AAC_HOSTENCNMFPROCESSINGCOMP_H_
#define _AAC_HOSTENCNMFPROCESSINGCOMP_H_

#include "AFMNmfHost_Encoder.h"
#include "AACHostEncAlgo.h"

/// Concrete class implementing a AAC encoder processing component
/// Derived from AFM_EncNmfProcessingComp.
class AACNmfHost_Encoder: public AFMNmfHost_Encoder{
public:

    AACNmfHost_Encoder(ENS_Component &enscomp) 
            : AFMNmfHost_Encoder(enscomp),mAlgo(0){}

    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);

protected:

    virtual OMX_ERRORTYPE instantiateAlgo(void);
    virtual OMX_ERRORTYPE startAlgo(void);
    virtual OMX_ERRORTYPE stopAlgo(void);
    virtual OMX_ERRORTYPE configureAlgo(void);
    virtual OMX_ERRORTYPE deInstantiateAlgo(void);

private:

    AACHostEncAlgo * mAlgo;
};

#endif // _AAC_HOSTENCNMFPROCESSINGCOMP_H_
