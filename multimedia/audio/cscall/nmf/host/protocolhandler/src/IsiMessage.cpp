/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   IsiMessage.cpp
 * \brief  ISI is the protocol agreed by the modem and APE.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/protocolhandler.nmf>
#include "cscall/nmf/host/protocolhandler/inc/IsiMessage.hpp"

IsiMessage::IsiMessage(t_uint8 *bufPtr, t_uint8 type, IsiTopHeader &isi_info, t_uint16 param, OMX_BUFFERHEADERTYPE *omx_header):
  phonetHeader((PHONET_HEADER_T *)bufPtr),
  codecReq((SPEECH_CODEC_REQ_T *)bufPtr),
  codecResp((SPEECH_CODEC_RESP_T *)bufPtr),
  timingReq((SPC_TIMING_REQ_T*)bufPtr),
  timingResp((SPC_TIMING_RESP_T *)bufPtr),
  decoderDataReq((SPC_DECODER_DATA_REQ_T *)bufPtr),
  encoderDataNtf((SPC_ENCODER_DATA_NTF_T *)bufPtr)
{
  setType(type);
  setMedia(isi_info.media);
  setReceiverDev(isi_info.sender_dev);
  setReceiverObj(isi_info.sender_obj);
  setTransactionId(isi_info.transsaction_id);
  setSenderDev(SENDER_DEVICE_AUDIO_DSP);
  setSenderObj(0);
  setRes(PN_DSP_AUDIO);
  
  switch (type)
  {
    case MODEM_AUDIO_SPEECH_CODEC_RESP:
      // param = reason
      setLength(sizeof(SPEECH_CODEC_RESP_T));
      setReason(param);
      break;
    case MODEM_AUDIO_SPC_TIMING_RESP:
      // param = status
      setLength(sizeof(SPC_TIMING_RESP_T));
      setStatus(param);
      break;
    case MODEM_AUDIO_SPC_ENCODER_DATA_NTF:
      // param = frame size in bytes
      setLength(param + getDataNtfHeaderSize());
      setNumSb(1);
      setSBId(MODEM_AUDIO_SB_SPC_CODEC_FRAME);
      setFrameLength(param);
      break;
    default:
      ARMNMF_DBC_ASSERT(0);
      break;
  }

  
  omx_header->nFilledLen = getLength();
  omx_header->nOffset	 = 0;
  omx_header->nFlags	 = 0;
}
