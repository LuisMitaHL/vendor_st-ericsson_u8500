/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   SHM_PcmOut.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _SHM_PCM_OUT_H_
#define _SHM_PCM_OUT_H_

#include "MpcBindings.h"

// NMF interfaces
#include "host/bindings/shmpcm/configure.hpp"

class SHM_PcmOut : public MpcBindings {
    public:

       SHM_PcmOut(AFM_Port &port, AFM_Controller &controller, OMX_U32 priority, t_cm_instance_handle sharedbuffer)
         : MpcBindings(port, controller,priority), mBuffer(0), mBufferSize(0), mNmfSyncLib(0), mNmfSharedBuffer(sharedbuffer) {
        };

        virtual OMX_ERRORTYPE instantiateBindingComponent(void);
        virtual OMX_ERRORTYPE deInstantiateBindingComponent(void);

        virtual OMX_ERRORTYPE configure(void);

    private:

        // NMF
        Ibindings_shmpcm_configure 	mIconfigure;

        //Internal buffer of ShmPcm component
        t_cm_memory_handle          mBuffer;
        int                         mBufferSize;

	t_cm_instance_handle        mNmfSyncLib;
	t_cm_instance_handle        mNmfSharedBuffer;
	
        // Private methods
        OMX_ERRORTYPE           allocateBuffer();
        OMX_ERRORTYPE           freeBuffer();
};

/// @}

#endif  // _SHM_PCM_OUT_H_
