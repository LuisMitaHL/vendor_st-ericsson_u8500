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
#include <speech_proc/nmf/host/hybrid_buffer/arm2mpc.nmf>
#include <SpeechProcHybridBuffer.h>


void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)
{
  OMX_BUFFERHEADERTYPE	 *omx_buffer    = (OMX_BUFFERHEADERTYPE *)buffer;
  SpeechProcHybridBuffer *hybrid_buffer = (SpeechProcHybridBuffer *)(omx_buffer->pInputPortPrivate);
  
  hybrid_buffer->updateMPCHeader();
  mpc.emptyThisBuffer(hybrid_buffer->getMPCHeader());
}

void METH(fillThisBuffer)(t_uint32 buffer)
{
  SpeechProcHybridBuffer *hybrid_buffer = (SpeechProcHybridBuffer *)buffer;

  hybrid_buffer->updateOMXHeader();
  inputport.fillThisBuffer(hybrid_buffer->getOMXHeader());
}
