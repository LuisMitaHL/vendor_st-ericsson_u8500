/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shared_bufout.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __shared_bufouthpp
#define __shared_bufouthpp


#include "ENS_Redefine_Class.h"

class nmf_host_shared_bufout : public nmf_host_shared_bufoutTemplate
{
public:
  
  virtual void fillThisBuffer (OMX_BUFFERHEADERTYPE_p buffer);
  virtual void emptyThisBuffer(t_uint32 buffer);
};

#endif // __shared_bufouthpp
