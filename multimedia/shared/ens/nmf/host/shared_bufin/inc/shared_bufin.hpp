/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shared_bufin.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __shared_bufinhpp
#define __shared_bufinhpp

#include "ENS_Redefine_Class.h"

class nmf_host_shared_bufin : public nmf_host_shared_bufinTemplate
{
public:
  
  virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
  virtual void fillThisBuffer(t_uint32 buffer);
};

#endif // __shared_bufinhpp
