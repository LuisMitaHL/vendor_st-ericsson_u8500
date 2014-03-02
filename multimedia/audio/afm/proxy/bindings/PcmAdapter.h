/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   PcmAdapter.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _PCMADAPTER_H_
#define _PCMADAPTER_H_

#include "MpcBindings.h"
#include "AFM_PcmPort.h"

// NMF interfaces
#include "host/bindings/pcmadapter/configure.hpp"

class PcmAdapter: public MpcBindings {
    public:
  PcmAdapter(AFM_PcmPort &port, AFM_Controller &controller, OMX_U32 priority);
            
        virtual OMX_ERRORTYPE instantiateBindingComponent(void);
        virtual OMX_ERRORTYPE deInstantiateBindingComponent(void);

        virtual OMX_ERRORTYPE configure(void);

    private:
 	    t_cm_instance_handle                mNmfSyncLib;
 
		t_cm_memory_handle                  mBufferPcm;
        
        Ibindings_pcmadapter_configure      mIconfigure;
        PcmAdapterParam_t                   mParams;
		
        OMX_ERRORTYPE       allocatePcmBuffer(void);
        OMX_ERRORTYPE       freePcmBuffer(void);
        int                 computeBufferSize(void);

        static int          gcd(int a, int b);
};

#endif // _PCMADAPTER_H_

