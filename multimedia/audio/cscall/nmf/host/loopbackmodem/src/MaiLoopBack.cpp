/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MaiLoopBack.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cscall/nmf/host/loopbackmodem.nmf"
#include "cscall/nmf/host/loopbackmodem/inc/MaiLoopBack.hpp"
#include <string.h>


CsCallMessage_t MaiLoopBack::getMsgType(OMX_BUFFERHEADERTYPE *msg)
{
  MAI_MessageHeader_t *mai_msg = (MAI_MessageHeader_t *)msg->pBuffer;
  
  switch(mai_msg->MessageId)
  {
	case MAI_MESSAGE_INFORMATION_REQUEST:    return MSG_MODEM_INFORMATION_REQUEST;
	case MAI_MESSAGE_INFORMATION_RESPONSE:   return MSG_MODEM_INFORMATION_RESPONSE;
	case MAI_MESSAGE_CODING_FORMAT:          return MSG_CODING_FORMAT_REQUEST;
	case MAI_MESSAGE_CODING_FORMAT_RESPONSE: return MSG_CODING_FORMAT_RESPONSE;
	case MAI_MESSAGE_VOICE_CALL_STATUS:      return MSG_VOICE_CALL_STATUS;
	case MAI_MESSAGE_TIMING_REPORT:          return MSG_TIMING_REPORT;
	case MAI_MESSAGE_UPLINK_TIMING_CONFIG:   return MSG_UPLINK_TIMING_CONFIGURATION;
	case MAI_MESSAGE_DOWNLINK_TIMING_CONFIG: return MSG_DOWNLINK_TIMING_CONFIGURATION;
	case MAI_MESSAGE_DOWNLINK_SPEECH_DATA:   return MSG_SPEECH_DATA_DOWNLINK;
	case MAI_MESSAGE_UPLINK_SPEECH_DATA:     return MSG_SPEECH_DATA_UPLINK;
    case MAI_MESSAGE_TEST_MODEM_LOOPBACK :       return MSG_TEST_MODEM_LOOP_REQUEST;
    case MAI_MESSAGE_TEST_MODEM_LOOPBACK_RESPONSE :  return MSG_TEST_MODEM_LOOP_RESPONSE;
    case MAI_MESSAGE_TEST_AUDIO_LOOPBACK :       return MSG_TEST_AUDIO_LOOP_REQUEST;
    case MAI_MESSAGE_TEST_AUDIO_LOOPBACK_RESPONSE :  return MSG_TEST_AUDIO_LOOP_RESPONSE;

	default:                                 return MSG_UPLINK_TIMING_CONFIGURATION;
  }
}


void MaiLoopBack::generate_coding_format_request(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer)
{
  MAI_CodingFormat_t *coding_format = (MAI_CodingFormat_t *)buffer->pBuffer;

  // common header
  coding_format->MessageHeader.MessageSize[0] = 0;
  coding_format->MessageHeader.MessageSize[1] = sizeof(MAI_CodingFormat_t);
  coding_format->MessageHeader.MessageId      = MAI_MESSAGE_CODING_FORMAT;
  
  // specific part 
  coding_format->System                    = cscall2MaiNetwork(config->network);
  coding_format->SpeechCodec               = cscall2MaiCodec(config->speech_codec);
  coding_format->NoiseSupressionControlBit = (MAI_NSCB_t)config->encoder.NSCB;
  coding_format->NSYNC                     = (MAI_AMR_NSYNC_t)config->encoder.nsync;
  coding_format->FrameNumber               = (MAI_FrameNumber_t)config->encoder.SFN;
  coding_format->DTX                       = (MAI_DTX_t)config->encoder.dtx;
  coding_format->NoDataUsage               = (MAI_NoDataUsage_t)config->encoder.nodata;
  coding_format->AMR_Rate                  = get_amr_rate((OMX_AUDIO_AMRBANDMODETYPE)config->encoder.amr_rate);
}



void MaiLoopBack::generate_timing_report(t_uint32 timing, t_uint16 cause, OMX_BUFFERHEADERTYPE *buffer)
{
  MAI_TimingReport_t *timing_report = (MAI_TimingReport_t *)buffer->pBuffer;

  // common header
  timing_report->MessageHeader.MessageSize[0] = 0;
  timing_report->MessageHeader.MessageSize[1] = sizeof(MAI_TimingReport_t);
  timing_report->MessageHeader.MessageId      = MAI_MESSAGE_TIMING_REPORT;

  // specific part
  timing_report->TimeDiff[0] = (uint8)(timing & 0xFF);
  timing_report->TimeDiff[1] = (uint8)(timing>>8 & 0xFF);
  timing_report->TimeDiff[2] = (uint8)(timing>>16 & 0xFF);
  timing_report->TimeDiff[3] = (uint8)(timing>>24 & 0xFF);
  timing_report->Cause       = (MAI_TimingCause_t)cause;
}


void MaiLoopBack::generate_voice_call_status(bool connected, OMX_BUFFERHEADERTYPE *buffer)
{
  MAI_VoiceCallStatus_t *voice_status = (MAI_VoiceCallStatus_t *)buffer->pBuffer;

  // common header
  voice_status->MessageHeader.MessageSize[0] = 0;
  voice_status->MessageHeader.MessageSize[1] = sizeof(MAI_VoiceCallStatus_t);
  voice_status->MessageHeader.MessageId      = MAI_MESSAGE_VOICE_CALL_STATUS;

  // specific part
  voice_status->CallStatus = (MAI_CallStatus_t)connected;
}

void MaiLoopBack::generate_fake_downlink_frame(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer)
{
  MAI_SpeechDataDL_t * dl_data = (MAI_SpeechDataDL_t *)buffer->pBuffer;
  int dl_size = sizeof(MAI_SpeechDataDL_t) - sizeof(MAI_DL_Buffers_t);

  // specific part
  dl_data->ProcessingTime[0] = 0;  // TODO : put real value
  dl_data->ProcessingTime[1] = 0;
  dl_data->ProcessingTime[2] = 0;
  dl_data->ProcessingTime[3] = 0;

  if(config->network == NETWORK_GSM)
  {
	dl_data->System = MAI_SYSTEM_2G;
  }
  else if(config->network == NETWORK_WCDMA)
  {
	dl_data->System = MAI_SYSTEM_3G;
  }
  else if(config->network == NETWORK_CS_O_HS)
  {
	dl_data->System = MAI_SYSTEM_CS_O_HS;
  }
  else
  {
	ARMNMF_DBC_ASSERT(0);
  }
  
  switch(config->speech_codec)
  {
	case CODEC_GSM_FR:
	  {
		dl_data->SpeechCodec  = MAI_SPEECH_CODEC_FR;
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		dl_size               = MAI_FR_DL_DATA_MSG_SIZE;
		NMF_PANIC("loopback mode for MAI FR is not up to date");
#endif
		break;
	  }
	case CODEC_GSM_HR:
	  {
		dl_data->SpeechCodec  = MAI_SPEECH_CODEC_HR;
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		dl_size               = MAI_HR_DL_DATA_MSG_SIZE;
		NMF_PANIC("loopback mode for MAI HR is not up to date");
#endif
		break;
	  }
	case CODEC_EFR:
	  {
		dl_data->SpeechCodec  = MAI_SPEECH_CODEC_EFR;
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		dl_size               = MAI_EFR_DL_DATA_MSG_SIZE;
		NMF_PANIC("loopback mode for MAI EFR is not up to date");
#endif
		break;
	  }
	case CODEC_AMR_NB:
	  {
		dl_data->SpeechCodec  = MAI_SPEECH_CODEC_AMR_NB;
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		dl_size               = MAI_AMR_NB_DL_DATA_MSG_SIZE;
		NMF_PANIC("loopback mode for MAI AMR is not up to date");
#endif
		break;
	  }
	case CODEC_AMR_WB:
	  {
		dl_data->SpeechCodec  = MAI_SPEECH_CODEC_AMR_WB;
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		dl_size               = MAI_AMR_WB_DL_DATA_MSG_SIZE;
		NMF_PANIC("loopback mode for MAI AMRWB is not up to date");
#endif
		break;
	  }
	case CODEC_PCM8:
	  {
		dl_data->SpeechCodec  = MAI_SPEECH_CODEC_PCM8;
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		dl_size               = MAI_PCM8_DL_DATA_MSG_SIZE;
		memset((void *)dl_data->DL_Data.PCM8.PCM8_Data, 0, MAI_PCM_FRAME_SIZE_8KHZ);
#endif
		break;
	  }
	case CODEC_PCM16:
	  {
		dl_data->SpeechCodec  = MAI_SPEECH_CODEC_PCM16;
#ifdef LOOPBACK_SEND_DUMMY_FRAME
		dl_size               = MAI_PCM16_DL_DATA_MSG_SIZE;
		memset((void *)dl_data->DL_Data.PCM16.PCM16_Data, 0, MAI_PCM_FRAME_SIZE_16KHZ);
#endif
		break;
	  }
	default:
	  {
		ARMNMF_DBC_ASSERT(0);
		break;
	  }
  }
  
  // common header
  dl_data->MessageHeader.MessageSize[0] = (dl_size>>8) & 0xFF;
  dl_data->MessageHeader.MessageSize[1] = dl_size & 0xFF;
  dl_data->MessageHeader.MessageId      = MAI_MESSAGE_DOWNLINK_SPEECH_DATA;
  
  buffer->nFilledLen = dl_size;
  buffer->nOffset    = 0;
}

void MaiLoopBack::copy_speech_uplink_to_downlink(OMX_BUFFERHEADERTYPE *uplink, OMX_BUFFERHEADERTYPE *downlink)
{
  MAI_SpeechDataUL_t * ul_data = (MAI_SpeechDataUL_t *)uplink->pBuffer;
  MAI_SpeechDataDL_t * dl_data = (MAI_SpeechDataDL_t *)downlink->pBuffer;
  int dl_size = ul_data->MessageHeader.MessageSize[1] + (ul_data->MessageHeader.MessageSize[0] << 8) + 8; //DL header has 2 more int
  
  // common header
  dl_data->MessageHeader.MessageSize[0] = (dl_size>>8) & 0xFF;
  dl_data->MessageHeader.MessageSize[1] = dl_size & 0xFF;
  dl_data->MessageHeader.MessageId      = MAI_MESSAGE_DOWNLINK_SPEECH_DATA;
  downlink->nFilledLen = dl_size;
  downlink->nOffset    = 0;
  
  // specific part
  dl_data->System            = ul_data->System;
  dl_data->SpeechCodec       = ul_data->SpeechCodec;
  dl_data->ProcessingTime[0] = 0;  // TODO : put real value
  dl_data->ProcessingTime[1] = 0;
  dl_data->ProcessingTime[2] = 0;
  dl_data->ProcessingTime[3] = 0;
  
  // codec specific
  switch(dl_data->SpeechCodec)
  {
	case MAI_SPEECH_CODEC_FR:
	  {
		if(ul_data->UL_Data.FR.SP == MAI_SP_SID_FRAME)
		{
		  dl_data->DL_Data.FR.SID = MAI_FRAME_TYPE_VALID_SID;
		}
		else
		{
		  dl_data->DL_Data.FR.SID = MAI_FRAME_TYPE_SPEECH;
		}
		
		dl_data->DL_Data.FR.TAF = MAI_TAF_NO_UPDATE_COMFORT_NOISE;
		dl_data->DL_Data.FR.BFI = MAI_BFI_FALSE;
		dl_data->DL_Data.FR.DFI = MAI_FR_DFI_FALSE ;
		
		memcpy((void *)(dl_data->DL_Data.FR.SpeechData), (const void *)(ul_data->UL_Data.FR.SpeechData), MAI_FR_FRAME_SIZE);
		break;
	  }
	case MAI_SPEECH_CODEC_HR:
	  {
		if(ul_data->UL_Data.HR.SP == MAI_SP_SID_FRAME)
		{
		  dl_data->DL_Data.HR.SID = MAI_FRAME_TYPE_VALID_SID;
		}
		else
		{
		  dl_data->DL_Data.HR.SID = MAI_FRAME_TYPE_SPEECH;
		}
		
		dl_data->DL_Data.HR.TAF = MAI_TAF_NO_UPDATE_COMFORT_NOISE;
		dl_data->DL_Data.HR.BFI = MAI_BFI_FALSE;
		dl_data->DL_Data.HR.UFI = MAI_UFI_FALSE;
		
		memcpy((void *)(dl_data->DL_Data.HR.SpeechData), (const void *)(ul_data->UL_Data.HR.SpeechData), MAI_HR_FRAME_SIZE);
		break; 
	  }
	case MAI_SPEECH_CODEC_EFR:
	  {
		if(ul_data->UL_Data.EFR.SP == MAI_SP_SID_FRAME)
		{
		  dl_data->DL_Data.EFR.SID = MAI_FRAME_TYPE_VALID_SID;
		}
		else
		{
		  dl_data->DL_Data.EFR.SID = MAI_FRAME_TYPE_SPEECH;
		}
		
		dl_data->DL_Data.EFR.TAF = MAI_TAF_NO_UPDATE_COMFORT_NOISE;
		dl_data->DL_Data.EFR.BFI = MAI_BFI_FALSE;
		
		memcpy((void *)(dl_data->DL_Data.EFR.SpeechData), (const void *)(ul_data->UL_Data.EFR.SpeechData), MAI_EFR_FRAME_SIZE);
		break;
	  }
	case MAI_SPEECH_CODEC_AMR_NB:
	  {
		switch(ul_data->UL_Data.AMR_NB.TX_Type)
		{
		  case MAI_AMR_TX_TYPE_SPEECH :
			dl_data->DL_Data.AMR_NB.RX_Type = MAI_AMR_NB_RX_TYPE_SPEECH;
			break;
		  case MAI_AMR_TX_TYPE_SID_FIRST :
			dl_data->DL_Data.AMR_NB.RX_Type = MAI_AMR_NB_RX_TYPE_SID_FIRST;
			break;
		  case MAI_AMR_TX_TYPE_SID_UPDATE :
			dl_data->DL_Data.AMR_NB.RX_Type = MAI_AMR_NB_RX_TYPE_SID_UPDATE;
			break;
		  case MAI_AMR_TX_TYPE_NO_DATA :
			dl_data->DL_Data.AMR_NB.RX_Type = MAI_AMR_NB_RX_TYPE_NO_DATA;
			break;
		}
		
		dl_data->DL_Data.AMR_NB.Rate = ul_data->UL_Data.AMR_NB.Rate;
		dl_data->DL_Data.AMR_NB.FQI  = MAI_FQI_FRAME_GOOD;
		
		memcpy((void *)(dl_data->DL_Data.AMR_NB.SpeechData), (const void *)(ul_data->UL_Data.AMR_NB.SpeechData), MAI_AMR_NB_FRAME_SIZE);
		break;
	  }
	case MAI_SPEECH_CODEC_AMR_WB:
	  {
		switch(ul_data->UL_Data.AMR_WB.TX_Type)
		{
		  case MAI_AMR_TX_TYPE_SPEECH :
			dl_data->DL_Data.AMR_WB.RX_Type = MAI_AMR_WB_RX_TYPE_SPEECH;
			break;
		  case MAI_AMR_TX_TYPE_SID_FIRST :
			dl_data->DL_Data.AMR_WB.RX_Type = MAI_AMR_WB_RX_TYPE_SID_FIRST;
			break;
		  case MAI_AMR_TX_TYPE_SID_UPDATE :
			dl_data->DL_Data.AMR_WB.RX_Type = MAI_AMR_WB_RX_TYPE_SID_UPDATE;
			break;
		  case MAI_AMR_TX_TYPE_NO_DATA :
			dl_data->DL_Data.AMR_WB.RX_Type = MAI_AMR_WB_RX_TYPE_NO_DATA;
			break;
		}
		
		dl_data->DL_Data.AMR_WB.Rate = ul_data->UL_Data.AMR_WB.Rate;
		dl_data->DL_Data.AMR_WB.FQI  = MAI_FQI_FRAME_GOOD;
		
		memcpy((void *)(dl_data->DL_Data.AMR_WB.SpeechData), (const void *)(ul_data->UL_Data.AMR_WB.SpeechData), MAI_AMR_WB_FRAME_SIZE);
		break;
	  }
	case MAI_SPEECH_CODEC_PCM8:
	  {
		dl_data->DL_Data.PCM8.SpeechCodec = MAI_SPEECH_CODEC_PCM8;
		memcpy((void *)&(dl_data->DL_Data.PCM8.PCM8_Data), (const void *)&(ul_data->UL_Data.PCM8.PCM8_Data), MAI_PCM_FRAME_SIZE_8KHZ);
		break;
	  }
	case MAI_SPEECH_CODEC_PCM16:
	  {
		dl_data->DL_Data.PCM8.SpeechCodec = MAI_SPEECH_CODEC_PCM16;
		memcpy((void *)&(dl_data->DL_Data.PCM16.PCM16_Data), (const void *)&(ul_data->UL_Data.PCM16.PCM16_Data), MAI_PCM_FRAME_SIZE_16KHZ);
		break;
	  }
	default :
	  {
		break;
	  }
  }
}

MAI_SpeechCodec_t MaiLoopBack::cscall2MaiCodec(CsCallCodec_t codec)
{
  switch(codec)
  {
	case CODEC_NONE   : return MAI_SPEECH_CODEC_NONE;
    case CODEC_GSM_FR : return MAI_SPEECH_CODEC_FR;
    case CODEC_GSM_HR : return MAI_SPEECH_CODEC_HR;
    case CODEC_EFR    : return MAI_SPEECH_CODEC_EFR;
    case CODEC_AMR_NB : return MAI_SPEECH_CODEC_AMR_NB;
    case CODEC_AMR_WB : return MAI_SPEECH_CODEC_AMR_WB;
    case CODEC_PCM8   : return MAI_SPEECH_CODEC_PCM8;
    case CODEC_PCM16  : return MAI_SPEECH_CODEC_PCM16;
	default           : return MAI_SPEECH_CODEC_NONE;
  }
}

MAI_System_t MaiLoopBack::cscall2MaiNetwork(CsCallNetwork_t network)
{
  switch(network)
  {
	case NETWORK_NONE    : return MAI_SYSTEM_NONE;
	case NETWORK_GSM     : return MAI_SYSTEM_2G;
	case NETWORK_WCDMA   : return MAI_SYSTEM_3G;
	case NETWORK_CS_O_HS : return MAI_SYSTEM_CS_O_HS;
	default              : return MAI_SYSTEM_NONE;
  }
}


MAI_AMR_BitRate_t MaiLoopBack::get_amr_rate(OMX_AUDIO_AMRBANDMODETYPE aAmrBandMode)
{
  MAI_AMR_BitRate_t bitrate;

  bitrate.AMR_NB = MAI_AMR_NB_RATE_NO_DATA;
  
  switch(aAmrBandMode)
  {
	case OMX_AUDIO_AMRBandModeNB0: bitrate.AMR_NB = MAI_AMR_NB_RATE_4_75;  break;
	case OMX_AUDIO_AMRBandModeNB1: bitrate.AMR_NB = MAI_AMR_NB_RATE_5_15;  break;
	case OMX_AUDIO_AMRBandModeNB2: bitrate.AMR_NB = MAI_AMR_NB_RATE_5_90;  break;
	case OMX_AUDIO_AMRBandModeNB3: bitrate.AMR_NB = MAI_AMR_NB_RATE_6_70;  break;
	case OMX_AUDIO_AMRBandModeNB4: bitrate.AMR_NB = MAI_AMR_NB_RATE_7_40;  break;
	case OMX_AUDIO_AMRBandModeNB5: bitrate.AMR_NB = MAI_AMR_NB_RATE_7_95;  break;
	case OMX_AUDIO_AMRBandModeNB6: bitrate.AMR_NB = MAI_AMR_NB_RATE_10_2;  break;
	case OMX_AUDIO_AMRBandModeNB7: bitrate.AMR_NB = MAI_AMR_NB_RATE_12_2;  break;
	case OMX_AUDIO_AMRBandModeWB0: bitrate.AMR_WB = MAI_AMR_WB_RATE_6_60;  break;
	case OMX_AUDIO_AMRBandModeWB1: bitrate.AMR_WB = MAI_AMR_WB_RATE_8_85;  break;
	case OMX_AUDIO_AMRBandModeWB2: bitrate.AMR_WB = MAI_AMR_WB_RATE_12_65; break;
	case OMX_AUDIO_AMRBandModeWB3: bitrate.AMR_WB = MAI_AMR_WB_RATE_14_25; break;
	case OMX_AUDIO_AMRBandModeWB4: bitrate.AMR_WB = MAI_AMR_WB_RATE_15_85; break;
	case OMX_AUDIO_AMRBandModeWB5: bitrate.AMR_WB = MAI_AMR_WB_RATE_18_25; break;
	case OMX_AUDIO_AMRBandModeWB6: bitrate.AMR_WB = MAI_AMR_WB_RATE_19_85; break;
	case OMX_AUDIO_AMRBandModeWB7: bitrate.AMR_WB = MAI_AMR_WB_RATE_23_05; break;
	case OMX_AUDIO_AMRBandModeWB8: bitrate.AMR_WB = MAI_AMR_WB_RATE_23_85; break;
	default:                       break;
  }
  
  return bitrate;
}
