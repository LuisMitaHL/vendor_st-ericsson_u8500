/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostSHM_PcmOut.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _HOST_SHM_PCM_OUT_H_
#define _HOST_SHM_PCM_OUT_H_

#include "HostBindings.h"
#include "hst/composite/shmpcmout.hpp"

class HostSHM_PcmOut : public HostBindings {
public:

  HostSHM_PcmOut(AFM_Port &port, AFM_Controller &controller,
		 OMX_HANDLETYPE omxhandle,
		 Iarmnmf_fillthisbuffer *fillthisbuffer ,
		 armnmf_emptythisbufferDescriptor  *emptythisbuffer)
    : HostBindings(port, controller, omxhandle),
      mIfillThisBuffer(fillthisbuffer),
      mDemptyThisBuffer(emptythisbuffer){
    memset(&mBuffer, 0, sizeof(OMX_BUFFERHEADERTYPE));
    mBufferSize = 0;
  };

  virtual OMX_ERRORTYPE instantiateBindingComponent(void);
  virtual OMX_ERRORTYPE deInstantiateBindingComponent(void);

  virtual OMX_ERRORTYPE configure(void);

private:

  // NMF
  Ihst_bindings_shmpcm_configure     mIconfigure;
  Iarmnmf_fillthisbuffer            *mIfillThisBuffer;
  armnmf_emptythisbufferDescriptor  *mDemptyThisBuffer;
	
  //Internal buffer of ShmPcm component
  OMX_BUFFERHEADERTYPE        mBuffer;
  int                         mBufferSize;


  // Private methods
  OMX_ERRORTYPE           allocateBuffer();
  OMX_ERRORTYPE           freeBuffer();
};

/// @}

#endif  // _SHM_PCM_OUT_H_
