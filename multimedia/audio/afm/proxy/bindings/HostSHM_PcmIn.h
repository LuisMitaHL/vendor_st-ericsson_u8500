/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostSHM_PcmIn.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _HOST_SHM_PCM_IN_H_
#define _HOST_SHM_PCM_IN_H_

#include "HostBindings.h"
#include "hst/composite/shmpcmin.hpp"


class HostSHM_PcmIn : public HostBindings {
public:

  HostSHM_PcmIn(AFM_Port &port, AFM_Controller &controller,
		OMX_HANDLETYPE omxhandle, 
		Iarmnmf_emptythisbuffer *emptythisbuffer ,
		armnmf_fillthisbufferDescriptor *fillthisbuffer)
    : HostBindings(port, controller, omxhandle),
      mIemptyThisBuffer(emptythisbuffer),
      mDfillThisBuffer(fillthisbuffer){
    memset(&mBuffer, 0, sizeof(OMX_BUFFERHEADERTYPE));
    mBufferSize = 0;
  };

  virtual OMX_ERRORTYPE instantiateBindingComponent(void);
  virtual OMX_ERRORTYPE deInstantiateBindingComponent(void);

  virtual OMX_ERRORTYPE configure(void);
        
private:

  // NMF
  Ihst_bindings_shmpcm_configure   mIconfigure;
  Iarmnmf_emptythisbuffer         *mIemptyThisBuffer;
  armnmf_fillthisbufferDescriptor *mDfillThisBuffer;
	  
  // Internal buffer of ShmPcm component
  OMX_BUFFERHEADERTYPE        mBuffer;
  int                         mBufferSize;

  // Private methods
  OMX_ERRORTYPE           allocateBuffer();
  OMX_ERRORTYPE           freeBuffer();

        
};

/// @}

#endif  // _HOST_SHM_PCM_IN_H_
