/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shared_bufin.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <nmf/host/shared_bufin.nmf>
#include <SharedBuffer.h>


void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)
{
  OMX_BUFFERHEADERTYPE	*omx_buffer   = (OMX_BUFFERHEADERTYPE *)buffer;
  SharedBuffer		*sharedbuffer = (SharedBuffer *)(omx_buffer->pInputPortPrivate);

  sharedbuffer->updateMPCHeader();
  mpc.emptyThisBuffer(sharedbuffer->getMPCHeader());
}

void METH(fillThisBuffer)(t_uint32 buffer)
{
  ARMSharedBuffer_t	*arm_shared_buffer = (ARMSharedBuffer_t *)buffer;
  SharedBuffer		*sharedbuffer      = (SharedBuffer *)(arm_shared_buffer->hostClassAddr);

  sharedbuffer->updateOMXHeader(false);
  host.fillThisBuffer(sharedbuffer->getOMXHeader());
}
