/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   arm2mpc.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __arm2mpc_hpp_
#define __arm2mpc_hpp_


class speech_proc_nmf_host_hybrid_buffer_arm2mpc : public speech_proc_nmf_host_hybrid_buffer_arm2mpcTemplate
{
public:

  // armnmf_emptythisbuffer
  virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);

  // speech_proc_nmf_common_hybrid_fillthisbuffer
  virtual void fillThisBuffer(t_uint32 buffer);
};

#endif // __arm2mpc_hpp_
