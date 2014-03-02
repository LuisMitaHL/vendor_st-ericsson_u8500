/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mpc2arm.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <speech_proc/nmf/host/hybrid_buffer/mpc2arm.nmf>
#include <SpeechProcHybridBuffer.h>


void METH(emptyThisBuffer)(t_uint32 buffer)
{
  SpeechProcHybridBuffer *hybrid_buffer = (SpeechProcHybridBuffer *)buffer;
  
  hybrid_buffer->updateOMXHeader();
  outputport.emptyThisBuffer(hybrid_buffer->getOMXHeader());
}

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)
{
  OMX_BUFFERHEADERTYPE	 *omx_buffer    = (OMX_BUFFERHEADERTYPE *)buffer;
  SpeechProcHybridBuffer *hybrid_buffer = (SpeechProcHybridBuffer *)(omx_buffer->pOutputPortPrivate);
  
  hybrid_buffer->updateMPCHeader();
  mpc.fillThisBuffer(hybrid_buffer->getMPCHeader());
}
