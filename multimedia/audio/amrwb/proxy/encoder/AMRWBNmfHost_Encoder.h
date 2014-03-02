/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _AMRWB_HOSTENCNMFPROCESSINGCOMP_H_
#define _AMRWB_HOSTENCNMFPROCESSINGCOMP_H_

#include "AFMNmfHost_Encoder.h"
#include "AMRWBHostEncAlgo.h"

/// Concrete class implementing a AMR encoder processing component
/// Derived from AFM_EncNmfProcessingComp.
class AMRWBNmfHost_Encoder: public AFMNmfHost_Encoder{
public:

    AMRWBNmfHost_Encoder(ENS_Component &enscomp) 
            : AFMNmfHost_Encoder(enscomp),mAlgo(0){}

    virtual ~AMRWBNmfHost_Encoder () {};
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

    AMRWBHostEncAlgo * mAlgo;
};

#endif // _AMRWB_HOSTENCNMFPROCESSINGCOMP_H_
