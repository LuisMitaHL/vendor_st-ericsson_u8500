/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   IsiProtocol.cpp
 * \brief  implementation of Protocol class for ISI protocol.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/protocolhandler.nmf>
#include "cscall/nmf/host/protocolhandler/inc/IsiTDProtocol.hpp"
#include <string.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_protocolhandler_src_IsiTDProtocolTraces.h"
#endif

////////////////////////
// Constructor
////////////////////////
IsiTDProtocol::IsiTDProtocol(void) : IsiProtocol()
{
  mIsFirstTimingReq = true;
  mRealCodecInUse   = ISI_CODEC_NONE;
}


////////////////////////
// Protocol interface
// only reimplements functions that need a specific behaviour compared to standard ISI protocol
////////////////////////

bool IsiTDProtocol::process_speech_data_downlink(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE *decoder)
{
  IsiMessage  isi_msg(msg->pBuffer);
  t_uint32   *frame_ptr  = (t_uint32 *)isi_msg.getFramePtr();
  t_uint16    frame_size = isi_msg.getFrameLength();

  // skip the frame header
  // size of the header is NOT included, so do not update frame_size
  frame_ptr++;

  // For the Half-Slim modem architecture we do not need to perform any byte re-ordering
  // (providing the modem handles PCM data in correct order)
  memcpy(decoder->pBuffer, frame_ptr, frame_size);

  decoder->nFilledLen = frame_size;
  decoder->nOffset    = 0;

  return true;
}


bool IsiTDProtocol::process_speech_data_uplink(OMX_BUFFERHEADERTYPE *encoder_buffer, OMX_BUFFERHEADERTYPE *msg,
											   Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
											   bool *config_updated)
{
  IsiMessage  isi_msg(msg->pBuffer,MODEM_AUDIO_SPC_ENCODER_DATA_NTF, mIsiTopHeader[CODEC_IDX],
					  (encoder_buffer->nFilledLen + 4), msg); // +4 for the header
  t_uint32   *frame_ptr = (t_uint32 *)isi_msg.getFramePtr();

  isi_msg.setDataFormat(mRealCodecInUse);

  // Set the frame pointer to four bytes ahead of current position - skipping over the
  // reserved field (2 bytes) and header (2 bytes) for the frame before writing the PCM data
  *frame_ptr = 0;
  frame_ptr++;

  // For the Half-Slim modem architecture we do not need to perform any byte re-ordering
  // (providing the modem handles PCM data in correct order)
  memcpy(frame_ptr, &(encoder_buffer->pBuffer[encoder_buffer->nOffset]), encoder_buffer->nFilledLen);

  *config_updated = false;
  return true;
}

////////////////////////
// ISIProtocol function override
////////////////////////
void IsiTDProtocol::getCodecAndNetwork(IsiMessage &isi_msg, ISI_CODEC_NAME_T &codec, ISI_NETWORK_NAME_T &network)
{
  // TD use half slim architecture. Codec information in the message is the one use for the communication
  // need to overwritte it by ISI_CODEC_PCM8/ISI_CODEC_PCM16. Choice between 8 and 16 khz is done at compilation time
  // Save codec use for communication in mRealCodecInUse
  mRealCodecInUse = isi_msg.getCodec();
  network         = isi_msg.getNetwork();

  if(mRealCodecInUse != ISI_CODEC_NONE)
  {
	OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiTDProtocol: codec = %d, network = %d", mRealCodecInUse, isi_msg.getNetwork());

#if CSCALL_DEFAULT_SAMPLING_RATE == 16000
	codec = ISI_CODEC_PCM16;
#else
	codec = ISI_CODEC_PCM8;
#endif
  }
  else
  {
	mIsFirstTimingReq = true;
	codec = ISI_CODEC_NONE;
  }
}


t_uint32 IsiTDProtocol::getTimingValue(IsiMessage &isi_msg, t_uint16 &response)
{
  if(mIsFirstTimingReq)
  {
	mIsFirstTimingReq = false;
	return IsiProtocol::getTimingValue(isi_msg, response);
  }
  else
  {
	response = SPC_TIMING_OK;
	return 0;
  }
}
