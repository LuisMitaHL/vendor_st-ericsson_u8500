/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _GSMFR_HOSTENC_NMFPROCESSINGCOMP_H_
#define _GSMFR_HOSTENC_NMFPROCESSINGCOMP_H_

#include "AFMNmfHost_Encoder.h"
#include "GSMFRHostEncAlgo.h"

/// Concrete class implementing a GSMFR decoder processing component
/// Derived from AFM_DecNmfProcessingComp.
class GSMFRNmfHost_Encoder: public AFMNmfHost_Encoder{

public:

    GSMFRNmfHost_Encoder(ENS_Component &enscomp) 
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

    GSMFRHostEncAlgo * mAlgo;
};

#endif // _GSMFR_HOSTENC_NMFPROCESSINGCOMP_H_
