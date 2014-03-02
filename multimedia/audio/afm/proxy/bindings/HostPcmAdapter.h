/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostPcmAdapter.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _HOSTPCMADAPTER_H_
#define _HOSTPCMADAPTER_H_

#include "HostBindings.h"
#include "hst/bindings/pcmadapter/configure.hpp"

//FIXME: code duplicated with PcmAdapter binding class!!!!!
//-> should be updated later!!!
class HostPcmAdapter: public HostBindings { 
    public:
        HostPcmAdapter(AFM_Port &port, AFM_Controller &controller, OMX_HANDLETYPE omxhandle, OMX_BOOL isOutputPort);

        //virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);
        //virtual OMX_ERRORTYPE start(void);
        //virtual OMX_ERRORTYPE stop(void);
        //virtual OMX_ERRORTYPE deInstantiate(void);

        virtual OMX_ERRORTYPE instantiateBindingComponent(void);
        virtual OMX_ERRORTYPE deInstantiateBindingComponent(void);

        OMX_ERRORTYPE configure(void);

    private:
        OMX_PTR                  mBufferPcm;
       
        Ihst_bindings_pcmadapter_configure  mIconfigure;
        HostPcmAdapterParam_t                   mParams;
		
        // Private methods
        OMX_ERRORTYPE       allocatePcmBuffer(void);
        OMX_ERRORTYPE       freePcmBuffer(void);
        int                 computeBufferSize(void);
        int                 gcd(int a, int b);
};

#endif //_HOSTPCMADAPTER_H__
