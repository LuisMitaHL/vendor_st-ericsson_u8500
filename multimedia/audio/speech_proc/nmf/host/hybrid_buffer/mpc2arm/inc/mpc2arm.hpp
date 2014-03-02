/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mpc2arm.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __mpc2arm_hpp_
#define __mpc2arm_hpp_


class speech_proc_nmf_host_hybrid_buffer_mpc2arm : public speech_proc_nmf_host_hybrid_buffer_mpc2armTemplate
{
public:

  // armnmf_fillthisbuffer
  virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);

  // speech_proc_nmf_common_hybrid_emptythisbuffer
  virtual void emptyThisBuffer(t_uint32 buffer);
};

#endif // __mpc2arm_hpp_
