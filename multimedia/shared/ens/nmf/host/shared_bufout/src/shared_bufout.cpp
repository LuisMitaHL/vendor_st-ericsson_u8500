/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shared_bufout.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <nmf/host/shared_bufout.nmf>
#include <SharedBuffer.h>


void METH(emptyThisBuffer)(t_uint32 buffer)
{
  ARMSharedBuffer_t	*arm_shared_buffer = (ARMSharedBuffer_t *)buffer;
  SharedBuffer		*sharedbuffer      = (SharedBuffer *)(arm_shared_buffer->hostClassAddr);
  
  sharedbuffer->updateOMXHeader(true);
  host.emptyThisBuffer(sharedbuffer->getOMXHeader());
}

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)
{
  OMX_BUFFERHEADERTYPE	*omx_buffer   = (OMX_BUFFERHEADERTYPE *)buffer;
  SharedBuffer		*sharedbuffer = (SharedBuffer *)(omx_buffer->pOutputPortPrivate);
  
  sharedbuffer->updateMPCHeader();
  mpc.fillThisBuffer(sharedbuffer->getMPCHeader());
}
