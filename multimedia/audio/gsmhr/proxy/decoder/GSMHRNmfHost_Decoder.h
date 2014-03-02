/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _GSMHR_HOSTDECNMFPROCESSINGCOMP_H_
#define _GSMHR_HOSTDECNMFPROCESSINGCOMP_H_

#include "AFMNmfHost_Decoder.h"
#include "GSMHRHostDecAlgo.h"

/// Concrete class impl`ementing a GSMHR decoder processing component
/// Derived from AFM_DecNmfProcessingComp.
class GSMHRNmfHost_Decoder: public AFMNmfHost_Decoder{
public:

    GSMHRNmfHost_Decoder(ENS_Component &enscomp) 
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

    GSMHRHostDecAlgo * mAlgo;
};

#endif // _GSMHR_HOSTDECNMFPROCESSINGCOMP_H_
