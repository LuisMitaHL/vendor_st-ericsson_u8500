/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostSHMin.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _HOST_SHM_IN_H_
#define _HOST_SHM_IN_H_

#include "HostBindings.h"
#include "hst/composite/shmin.hpp"


class HostSHMin : public HostBindings {
public:

  HostSHMin(AFM_Port &port, AFM_Controller &controller,
	    OMX_HANDLETYPE omxhandle,
	    Iarmnmf_emptythisbuffer *emptythisbuffer ,
	    armnmf_fillthisbufferDescriptor *fillthisbuffer)
    : HostBindings(port, controller, omxhandle),
      mIemptyThisBuffer(emptythisbuffer),
      mDfillThisBuffer(fillthisbuffer)
  {};

  virtual OMX_ERRORTYPE instantiateBindingComponent(void);
  virtual OMX_ERRORTYPE deInstantiateBindingComponent(void);

  virtual OMX_ERRORTYPE configure(void);
        
private:

  // NMF
  Ihst_bindings_shm_configure	 mIconfigure;
  Ihst_common_pcmsettings		 mIpcmsettings;
  Iarmnmf_emptythisbuffer		*mIemptyThisBuffer;
  armnmf_fillthisbufferDescriptor	*mDfillThisBuffer;
};

/// @}

#endif  // _HOST_SHM_IN_H_
