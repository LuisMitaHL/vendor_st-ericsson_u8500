/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   SHM_PcmIn.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _SHM_PCM_IN_H_
#define _SHM_PCM_IN_H_

#include "MpcBindings.h"

// NMF interfaces
#include "host/pcmsettings.hpp"
#include "host/bindings/shmpcm/configure.hpp"


class SHM_PcmIn : public MpcBindings {
    public:

        SHM_PcmIn(AFM_Port &port, AFM_Controller &controller, OMX_U32 priority, t_cm_instance_handle sharedbuffer)
	  : MpcBindings(port, controller, priority), mNmfSyncLib(0), mNmfSharedBuffer(sharedbuffer), mBuffer(0), mBufferSize(0) {
        };

        virtual OMX_ERRORTYPE instantiateBindingComponent(void);
        virtual OMX_ERRORTYPE deInstantiateBindingComponent(void);

        virtual OMX_ERRORTYPE configure(void);
        
    private:
	t_cm_instance_handle        mNmfSyncLib;
        t_cm_instance_handle        mNmfSharedBuffer;
	  
	// NMF
        Ibindings_shmpcm_configure  mIconfigure;

        // Internal buffer of ShmPcm component
        t_cm_memory_handle          mBuffer;
        int                         mBufferSize;

        // Private methods
        OMX_ERRORTYPE               allocateBuffer();
        OMX_ERRORTYPE               freeBuffer();

        
};

/// @}

#endif  // _SHM_PCM_IN_H_
