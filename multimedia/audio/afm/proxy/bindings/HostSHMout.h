/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostSHMout.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _HOST_SHM_OUT_H_
#define _HOST_SHM_OUT_H_

#include "HostBindings.h"
#include "hst/composite/shmout.hpp"

class HostSHMout : public HostBindings {
public:

  HostSHMout(AFM_Port &port, AFM_Controller &controller,
	     OMX_HANDLETYPE omxhandle,
	     Iarmnmf_fillthisbuffer *fillthisbuffer , 
	     armnmf_emptythisbufferDescriptor  *emptythisbuffer)
    : HostBindings(port, controller, omxhandle),
      mIfillThisBuffer(fillthisbuffer),
      mDemptyThisBuffer(emptythisbuffer)  
  {};
  
  virtual OMX_ERRORTYPE instantiateBindingComponent(void);
  virtual OMX_ERRORTYPE deInstantiateBindingComponent(void);

  virtual OMX_ERRORTYPE configure(void);

private:

  // NMF
  Ihst_bindings_shm_configure		 mIconfigure;
  Iarmnmf_fillthisbuffer		*mIfillThisBuffer;
  armnmf_emptythisbufferDescriptor	*mDemptyThisBuffer;
};

/// @}

#endif  // _HOST_SHM_OUT_H_
