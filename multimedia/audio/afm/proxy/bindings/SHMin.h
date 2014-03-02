/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   SHMin.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _SHM_IN_H_
#define _SHM_IN_H_

#include "MpcBindings.h"

// NMF interfaces
#include "host/pcmsettings.hpp"
#include "host/bindings/shm/configure.hpp"


class SHMin : public MpcBindings {
    public:

        SHMin(AFM_Port &port, AFM_Controller &controller, OMX_U32 priority, t_cm_instance_handle sharedbuffer)
	  : MpcBindings(port, controller, priority), mNmfSharedBuffer(sharedbuffer) {
        };

        virtual OMX_ERRORTYPE instantiateBindingComponent(void);
        virtual OMX_ERRORTYPE deInstantiateBindingComponent(void);

        virtual OMX_ERRORTYPE configure(void);
        
    private:

        // NMF
        Ibindings_shm_configure     mIconfigure;
        Ipcmsettings                mIpcmsettings;

	t_cm_instance_handle        mNmfSharedBuffer;
};

/// @}

#endif  // _SHM_IN_H_

