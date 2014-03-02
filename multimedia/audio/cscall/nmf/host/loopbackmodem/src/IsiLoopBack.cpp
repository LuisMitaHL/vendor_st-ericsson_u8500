/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   IsiLoopBack.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cscall/nmf/host/loopbackmodem.nmf"
#include "cscall/nmf/host/loopbackmodem/inc/IsiLoopBack.hpp"
#include <string.h>

CsCallMessage_t IsiLoopBack::getMsgType(OMX_BUFFERHEADERTYPE *msg)
{
  IsiMessage  isi_msg(msg->pBuffer);
  t_uint32    type = isi_msg.getType();

  switch(type)
  {
	case MODEM_AUDIO_SPEECH_CODEC_REQ:      return MSG_CODING_FORMAT_REQUEST;
	case MODEM_AUDIO_SPEECH_CODEC_RESP:     return MSG_CODING_FORMAT_RESPONSE;
	case MODEM_AUDIO_SPC_TIMING_REQ:        return MSG_TIMING_REPORT;
	case MODEM_AUDIO_SPC_TIMING_RESP:       return MSG_TIMING_REPORT_RESPONSE;
	case MODEM_AUDIO_SPC_TIMING_NEEDED_NTF: return MSG_UPLINK_TIMING_CONFIGURATION;
	case MODEM_AUDIO_SPC_DECODER_DATA_REQ:  return MSG_SPEECH_DATA_DOWNLINK;
	case MODEM_AUDIO_SPC_ENCODER_DATA_NTF:  return MSG_SPEECH_DATA_UPLINK;
	default:                                return MSG_UPLINK_TIMING_CONFIGURATION;
  }
}


void IsiLoopBack::generate_coding_format_request(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer)
{
  IsiMessage isiMessage(buffer->pBuffer);

  //set various fields
  isiMessage.setType(MODEM_AUDIO_SPEECH_CODEC_REQ);
  isiMessage.setLength(sizeof(ModemAudioSpeechCodecReq));
  isiMessage.setMedia(0x26);//that is what modem sends
  isiMessage.setSenderDev(0x64);//dir of msg Modem ->APE sender = 0x64,rec = 0x0;
  isiMessage.setReceiverDev(0x0);
  isiMessage.setSenderObj(0x0);
  isiMessage.setReceiverObj(0x0);
  isiMessage.setTransactionId(1);
  isiMessage.setRes(PN_DSP_AUDIO);
  mNetwork = cscall2IsiNetwork(config->network); //remember this to be used for SpeechTimingReq
  isiMessage.setNetwork(mNetwork);
  if(config->speech_codec == CODEC_NONE)
  {
	NMF_LOG("Ending CS Call\n");
  }
  isiMessage.setCodec(cscall2IsiCodec(config->speech_codec));
  isiMessage.setAACFlag(config->encoder.AAC);
  isiMessage.setNsync(config->encoder.nsync);
  isiMessage.setSFN(config->encoder.SFN);
  isiMessage.setAmrEncoder(get_amr_rate((OMX_AUDIO_AMRBANDMODETYPE)config->encoder.amr_rate));
  isiMessage.setDTX(config->encoder.dtx);
  isiMessage.setAmrDecoder(get_amr_rate((OMX_AUDIO_AMRBANDMODETYPE)config->decoder.amr_rate));
  isiMessage.setCodecInit(config->init);
}



void IsiLoopBack::generate_timing_report(t_uint32 timing, t_uint16 cause, OMX_BUFFERHEADERTYPE *buffer)
{
  IsiMessage isiMessage(buffer->pBuffer);
  
  //set various fields
  isiMessage.setType(MODEM_AUDIO_SPC_TIMING_REQ);
  isiMessage.setLength(sizeof(ModemAudioSpcTimingReq));
  isiMessage.setMedia(0x26);//that is what modem sends
  isiMessage.setSenderDev(0x64);//dir of msg Modem ->APE sender = 0x64,rec = 0x0;
  isiMessage.setReceiverDev(0x0);
  isiMessage.setSenderObj(0x0);
  isiMessage.setReceiverObj(0x0);
  isiMessage.setTransactionId(1);
  isiMessage.setRes(PN_DSP_AUDIO);
  //set specific fields
  isiMessage.setTimingHigh((timing & 0xFFFF0000)>>16);
  isiMessage.setTimingLow(timing & 0xFFFF);
  isiMessage.setProcessingtime(0);
  isiMessage.setSubSystem(mNetwork);//for the time being hard coded, shall change it to the value we get in codec_req
}

void IsiLoopBack::generate_voice_call_status(bool connected, OMX_BUFFERHEADERTYPE *buffer)
{
  ARMNMF_DBC_ASSERT(0);
}


#ifdef LOOPBACK_SEND_DUMMY_FRAME
// dummy frame
t_uint8 amr_frame  []={0,0,0,52,198,30,8,39,128,5,40,62,64,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
t_uint8 amrwb_frame[]={0,0,0,20,80,70,0,119,255,222,5,241,91,103,143,140,247,112,7,218,130,202,209,90,66,222,90,192,68,238};
t_uint8 efr_frame  []={0,0,0,0,245,85,228,53,1,58,7,127,160,76,0,1,94,0,0,15,0,0,224,0,0,1,0,16,0,0};
t_uint8 fr_frame   []={0,0,0,0,0,193,135,160,16,7,0,8,128,68,255,255,255,255,255,255,0,7,0,0,0,0,0,0,1,152};
t_uint8 hr_frame   []={0,0,0,32,145,130,0,2,0,32,0,0,66,65,0,161,0,0,0,0,0,0};
#endif

void IsiLoopBack::generate_fake_downlink_frame(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer)
{
  IsiMessage isiMessageDL(buffer->pBuffer);
#ifdef LOOPBACK_SEND_DUMMY_FRAME
  t_uint8    *bitstream = isiMessageDL.getFramePtr();
#endif
  
  //set specfic fields
  isiMessageDL.setDecNumSb(1);
  isiMessageDL.setDecSBId(MODEM_AUDIO_SB_SPC_CODEC_FRAME);
  isiMessageDL.setDecFrameLength(0);
  
  switch(config->speech_codec)
  {
	case CODEC_GSM_FR:
	  {
		isiMessageDL.setDecDataFormat(ISI_CODEC_GSM_FR);
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		isiMessageDL.setDecFrameLength(sizeof(fr_frame));
		memcpy(bitstream, fr_frame, sizeof(fr_frame));
#endif
		break;
	  }
	case CODEC_GSM_HR:
	  {
		isiMessageDL.setDecDataFormat(ISI_CODEC_GSM_HR);
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		isiMessageDL.setDecFrameLength(sizeof(hr_frame));
		memcpy(bitstream, hr_frame, sizeof(hr_frame));
#endif
		break;
	  }
	case CODEC_EFR:
	  {
		isiMessageDL.setDecDataFormat(ISI_CODEC_EFR);
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		isiMessageDL.setDecFrameLength(sizeof(efr_frame));
		memcpy(bitstream, efr_frame, sizeof(efr_frame));
#endif
		break;
	  }
	case CODEC_AMR_NB:
	  {
		isiMessageDL.setDecDataFormat(ISI_CODEC_AMR_NB);
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		isiMessageDL.setDecFrameLength(sizeof(amr_frame));
		memcpy(bitstream, amr_frame, sizeof(amr_frame));
#endif
		break;
	  }
	case CODEC_AMR_WB:
	  {
		isiMessageDL.setDecDataFormat(ISI_CODEC_AMR_WB);
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		isiMessageDL.setDecFrameLength(sizeof(amrwb_frame));
		memcpy(bitstream, amrwb_frame, sizeof(amrwb_frame));
#endif
		break;
	  }
	case CODEC_PCM8:
	  {
		isiMessageDL.setDecDataFormat(ISI_CODEC_PCM8);
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		isiMessageDL.setDecFrameLength(320);
		memset((void *)bitstream, 0, 320);
#endif
		break;
	  }
	case CODEC_PCM16:
	  {
		isiMessageDL.setDecDataFormat(ISI_CODEC_PCM16);
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		isiMessageDL.setDecFrameLength(640);
		memset((void *)bitstream, 0, 640);
#endif
		break;
	  }
	default:
	  {
		ARMNMF_DBC_ASSERT(0);
		break;
	  }
  }

  //set command fields
  isiMessageDL.setType(MODEM_AUDIO_SPC_DECODER_DATA_REQ);
  isiMessageDL.setLength(sizeof(SPC_DECODER_DATA_REQ_T) + isiMessageDL.getEncFrameLength());
  isiMessageDL.setMedia(0x26);//that is what modem sends
  isiMessageDL.setSenderDev(0x64);//dir of msg Modem ->APE sender = 0x64,rec = 0x0;
  isiMessageDL.setReceiverDev(0x0);
  isiMessageDL.setSenderObj(0x0);
  isiMessageDL.setReceiverObj(0x0);
  isiMessageDL.setTransactionId(1);
  isiMessageDL.setRes(PN_DSP_AUDIO);

  buffer->nFilledLen = sizeof(SPC_DECODER_DATA_REQ_T) + isiMessageDL.getEncFrameLength();
  buffer->nOffset    = 0;
}


void IsiLoopBack::copy_speech_uplink_to_downlink(OMX_BUFFERHEADERTYPE *uplink, OMX_BUFFERHEADERTYPE *downlink)
{
  IsiMessage isiMessageUL(uplink->pBuffer);
  IsiMessage isiMessageDL(downlink->pBuffer);

  //copy the encoder frame to the decoder frame,
  memcpy( (void *)isiMessageDL.getFramePtr(), (const void *)isiMessageUL.getEncFramePtr(),isiMessageUL.getEncFrameLength());
  
  //set various fields
  //set command fields
  isiMessageDL.setType(MODEM_AUDIO_SPC_DECODER_DATA_REQ);
  isiMessageDL.setLength(sizeof(SPC_DECODER_DATA_REQ_T) + isiMessageUL.getEncFrameLength()/*its the same frame we copied*/);
  isiMessageDL.setMedia(0x26);//that is what modem sends
  isiMessageDL.setSenderDev(0x64);//dir of msg Modem ->APE sender = 0x64,rec = 0x0;
  isiMessageDL.setReceiverDev(0x0);
  isiMessageDL.setSenderObj(0x0);
  isiMessageDL.setReceiverObj(0x0);
  isiMessageDL.setTransactionId(1);
  isiMessageDL.setRes(PN_DSP_AUDIO);

  downlink->nFilledLen = sizeof(SPC_DECODER_DATA_REQ_T) + isiMessageUL.getEncFrameLength();
  downlink->nOffset    = 0;
  
  //set specfic fields
  isiMessageDL.setDecDataFormat(isiMessageUL.getEncDataFormat());
  isiMessageDL.setDecNumSb(isiMessageUL.getEncNumSB());
  isiMessageDL.setDecSBId(isiMessageUL.getEncSBId());
  isiMessageDL.setDecFrameLength(isiMessageUL.getEncFrameLength());

  // not needed in HR format, the TX header containing relevent info for RX.
  if (isiMessageDL.getDataFormat()==ISI_CODEC_GSM_FR)
  {
	//assuming the frame is a GSM FR frame for now
	MODEM_AUDIO_GSM_FR_FRAME_T* gsmfr_dl_frame;
	gsmfr_dl_frame = (MODEM_AUDIO_GSM_FR_FRAME_T* )isiMessageDL.getFramePtr();//frame already copied to DL buffer, so can use this
    
	//check the SP flag. 1 means speech, 0 means SID
	if (gsmfr_dl_frame->SP == MODEM_AUDIO_SPC_SP_OFF)
	{
	  gsmfr_dl_frame->SID = MODEM_AUDIO_SPC_SID_2;    // Good SID frame
	}
	gsmfr_dl_frame->SP = MODEM_AUDIO_SPC_SP_OFF; // SP flag is not relevent in DL
  }
  
  if (isiMessageDL.getDataFormat()==ISI_CODEC_EFR)
  {
	//assuming the frame is a GSM HR frame for now
	MODEM_AUDIO_GSM_EFR_FRAME_T* efr_dl_frame;
	efr_dl_frame = (MODEM_AUDIO_GSM_EFR_FRAME_T* )isiMessageDL.getFramePtr();//frame already copied to DL buffer, so can use this
    
	//check the SP flag. 1 means speech, 0 means SID
	if (efr_dl_frame->SP == MODEM_AUDIO_SPC_SP_OFF)
	{
	  efr_dl_frame->SID = MODEM_AUDIO_SPC_SID_2;    // Good SID frame
	}
	efr_dl_frame->SP = MODEM_AUDIO_SPC_SP_OFF; // SP flag is not relevent in DL
  }
  
  if(isiMessageDL.getDataFormat()==ISI_CODEC_AMR_NB)
  {
	//assuming the frame is a AMR NB frame for now
	MODEM_AUDIO_AMR_FRAME_T* amr_dl_frame;
	amr_dl_frame = (MODEM_AUDIO_AMR_FRAME_T* )isiMessageDL.getFramePtr();//frame already copied to DL buffer, so can use this
	switch(amr_dl_frame->TxType)
	{
	  case AMR_TX_SPEECH:
		amr_dl_frame->RxType = AMR_RX_SPEECH_GOOD;
		//in case it is a 3G frame, we set WCDMA quality default as GOOD
		amr_dl_frame->quality = 1;
		break;
	  case AMR_TX_SID_FIRST:
		amr_dl_frame->RxType = AMR_RX_SID_FIRST;
		break;
	  case AMR_TX_SID_UPDATE:
		amr_dl_frame->RxType = AMR_RX_SID_UPDATE;
		break;
	  case AMR_TX_NO_DATA:
		amr_dl_frame->RxType = AMR_RX_NO_DATA;
		break;
	  default:
		NMF_LOG("Tx Type has no valid data");
		break;
	}
	//not relevant for decoder, set to 0
	amr_dl_frame->TxType = 0;
	//frame type remains same
	//amr_dl_frame->FrameType = amr_dl_frame->FrameType;
  }
  
  if(isiMessageDL.getDataFormat()==ISI_CODEC_AMR_WB)
  {
	//assuming the frame is a AMR NB frame for now
	MODEM_AUDIO_AMR_WB_FRAME_T* amrwb_dl_frame;
	amrwb_dl_frame = (MODEM_AUDIO_AMR_WB_FRAME_T* )isiMessageDL.getFramePtr();//frame already copied to DL buffer, so can use this
    
	switch(amrwb_dl_frame->TxType)
	{
	  case AMR_TX_SPEECH:
		amrwb_dl_frame->RxType = AMR_RX_SPEECH_GOOD;
		//in case it is a 3G frame, we set WCDMA quality default as GOOD
		amrwb_dl_frame->quality = 1;
		break;
	  case AMR_TX_SID_FIRST:
		amrwb_dl_frame->RxType = AMR_RX_SID_FIRST;
		break;
	  case AMR_TX_SID_UPDATE:
		amrwb_dl_frame->RxType = AMR_RX_SID_UPDATE;
		break;
	  case AMR_TX_NO_DATA:
		amrwb_dl_frame->RxType = AMR_RX_NO_DATA;
		break;
	  default:
		NMF_LOG("Tx Type has no valid data");
		break;
	}
	//not relevant for decoder, set to 0
	amrwb_dl_frame->TxType = 0;
	//frame type remains same
	//amr_dl_frame->FrameType = amr_dl_frame->FrameType;
  }
}

ISI_CODEC_NAME_T IsiLoopBack::cscall2IsiCodec(CsCallCodec_t codec)
{
  switch(codec)
  {
	case CODEC_NONE   : return ISI_CODEC_NONE;
    case CODEC_GSM_FR : return ISI_CODEC_GSM_FR;
    case CODEC_GSM_HR : return ISI_CODEC_GSM_HR;
    case CODEC_EFR    : return ISI_CODEC_EFR;
    case CODEC_AMR_NB : return ISI_CODEC_AMR_NB;
    case CODEC_AMR_WB : return ISI_CODEC_AMR_WB;
    case CODEC_PCM8   : return ISI_CODEC_PCM8;
    case CODEC_PCM16  : return ISI_CODEC_PCM16;
	default           : return ISI_CODEC_NONE;
  }
}

ISI_NETWORK_NAME_T IsiLoopBack::cscall2IsiNetwork(CsCallNetwork_t network)
{
  switch(network)
  {
	case NETWORK_NONE  : return ISI_NETWORK_NONE;
	case NETWORK_GSM   : return ISI_NETWORK_GSM;
	case NETWORK_WCDMA : return ISI_NETWORK_WCDMA;
	default            : return ISI_NETWORK_NONE;
  }
}


t_uint8 IsiLoopBack::get_amr_rate(OMX_AUDIO_AMRBANDMODETYPE aAmrBandMode)
{
  switch(aAmrBandMode)
  {
	case OMX_AUDIO_AMRBandModeUnused: return  0;
	case OMX_AUDIO_AMRBandModeNB0:    return  MODEM_AUDIO_AMR_475;
	case OMX_AUDIO_AMRBandModeNB1:    return  MODEM_AUDIO_AMR_515;
	case OMX_AUDIO_AMRBandModeNB2:    return  MODEM_AUDIO_AMR_590;
	case OMX_AUDIO_AMRBandModeNB3:    return  MODEM_AUDIO_AMR_670;
	case OMX_AUDIO_AMRBandModeNB4:    return  MODEM_AUDIO_AMR_740;
	case OMX_AUDIO_AMRBandModeNB5:    return  MODEM_AUDIO_AMR_795;
	case OMX_AUDIO_AMRBandModeNB6:    return  MODEM_AUDIO_AMR_102;
	case OMX_AUDIO_AMRBandModeNB7:    return  MODEM_AUDIO_AMR_122;
	case OMX_AUDIO_AMRBandModeWB0:    return  MODEM_AUDIO_WB_AMR_660;
	case OMX_AUDIO_AMRBandModeWB1:    return  MODEM_AUDIO_WB_AMR_885;
	case OMX_AUDIO_AMRBandModeWB2:    return  MODEM_AUDIO_WB_AMR_1265;
	case OMX_AUDIO_AMRBandModeWB3:    return  MODEM_AUDIO_WB_AMR_1425;
	case OMX_AUDIO_AMRBandModeWB4:    return  MODEM_AUDIO_WB_AMR_1585;
	case OMX_AUDIO_AMRBandModeWB5:    return  MODEM_AUDIO_WB_AMR_1825;
	case OMX_AUDIO_AMRBandModeWB6:    return  MODEM_AUDIO_WB_AMR_1985;
	case OMX_AUDIO_AMRBandModeWB7:    return  MODEM_AUDIO_WB_AMR_2305;
	case OMX_AUDIO_AMRBandModeWB8:    return  MODEM_AUDIO_WB_AMR_2385;
	default:                          return  0;
  }
}
