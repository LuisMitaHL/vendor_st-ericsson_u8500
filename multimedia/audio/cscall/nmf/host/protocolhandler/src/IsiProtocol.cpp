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
#include "cscall/nmf/host/protocolhandler/inc/IsiProtocol.hpp"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_protocolhandler_src_IsiProtocolTraces.h"
#endif
#ifdef USE_LOGI
#define LOG_TAG "CSCALL_PH"
#include "linux_utils.h"
#endif

#define CSCALL_MINIMAL_TIMING_VALUE 10000   // correspond to 10ms in us
#define CSCALL_MAXIMAL_TIMING_VALUE 1000000 // correspond to 1s in us
#define CSCALL_DEFAULT_TIMING_VALUE 25000   // default value for timing


////////////////////////
// constructor
////////////////////////
IsiProtocol::IsiProtocol(void)
{
  for (int i=0; i<IDX_NB ;i++)
  {
	mIsiTopHeader[i].media	         = 0;
	mIsiTopHeader[i].sender_dev	     = 0;
	mIsiTopHeader[i].sender_obj	     = 0;
	mIsiTopHeader[i].transsaction_id = 0;
  }

  mCodecInUse = ISI_CODEC_NONE;
  mNetwork    = ISI_NETWORK_NONE;
}


////////////////////////
// Protocol interface
////////////////////////
CsCallMessage_t IsiProtocol::getMsgType(OMX_BUFFERHEADERTYPE *msg)
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

t_uint16 IsiProtocol::getMsgLength(OMX_BUFFERHEADERTYPE *msg)
{
  IsiMessage  isi_msg(msg->pBuffer);

  return isi_msg.getLength();
}

t_uint16 IsiProtocol::getNbAnswer(CsCallMessage_t msg_type)
{
  switch(msg_type)
  {
	case MSG_MODEM_INFORMATION_REQUEST:     return 0;
	case MSG_MODEM_INFORMATION_RESPONSE:    return 0;
	case MSG_CODING_FORMAT_REQUEST:         return 1;
	case MSG_CODING_FORMAT_RESPONSE:        return 0;
	case MSG_UPLINK_TIMING_CONFIGURATION:   return 0;
	case MSG_DOWNLINK_TIMING_CONFIGURATION: return 0;
	case MSG_TIMING_REPORT:                 return 1;
	case MSG_TIMING_REPORT_RESPONSE:        return 0;
	case MSG_SPEECH_DATA_DOWNLINK:          return 0;
	case MSG_SPEECH_DATA_UPLINK:            return 0;
	case MSG_VOICE_CALL_STATUS:             return 0;
	case MSG_TEST_MODEM_LOOP_REQUEST:       return 0;
	case MSG_TEST_MODEM_LOOP_RESPONSE:      return 0;
	case MSG_TEST_AUDIO_LOOP_REQUEST:       return 0;
	case MSG_TEST_AUDIO_LOOP_RESPONSE:      return 0;
	default:                                return 0;
  }
}

bool IsiProtocol::process_modem_information_response(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer)
{
  // no such message in Isi protocol
  return false;
}


bool IsiProtocol::is_end_of_call(OMX_BUFFERHEADERTYPE *msg)
{
  IsiMessage       isi_msg(msg->pBuffer);
  ISI_CODEC_NAME_T newcodec = isi_msg.getCodec();

  if((newcodec != mCodecInUse) &&
	 newcodec == ISI_CODEC_NONE)
  {
	return true;
  }

  return false;
}

void IsiProtocol::getCodecAndNetwork(IsiMessage &isi_msg, ISI_CODEC_NAME_T &codec, ISI_NETWORK_NAME_T &network)
{
  //coverity[check_return]
  codec    = isi_msg.getCodec();
  network  = isi_msg.getNetwork();
}

bool IsiProtocol::process_coding_format_request(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
												Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
												CsCallCodecReq_t *codec_req,
												CsCallCodec_t *codec_in_use, CsCallNetwork_t *network_in_use,
												bool *config_updated, t_uint32 *time, bool *time_updated)
{
  IsiMessage         isi_msg(msg->pBuffer);
  //coverity[check_return]
  ISI_CODEC_NAME_T	 newcodec;
  ISI_NETWORK_NAME_T newnetwork;

  getCodecAndNetwork(isi_msg, newcodec, newnetwork);

  OstTraceFiltInst4 (TRACE_ALWAYS, "Cscall/IsiProtocol: process_coding_format_request() new config received (codec=%d, network=%d, Dtx=%d, BitRate=%d)",newcodec,newnetwork,isi_msg.getDTX(),isi_msg.getAmrEncoder());

  storeIsiTopHeader(CODEC_IDX, isi_msg);

  // default value
  *config_updated = true;
  *time_updated   = false;

  // Always update current DTX mode
  codec_status->encDtx = isi_msg.getDTX();

  if((mCodecInUse != newcodec) || (mNetwork != newnetwork))
  {
	// Set New Codec Config
	mCodecInUse     = newcodec;
	*codec_in_use   = isi2CscallCodec(newcodec);
	mNetwork        = newnetwork;
	*network_in_use = isi2CscallNetwork(newnetwork);

	codec_req->speech_codec     = *codec_in_use;
	codec_req->network          = *network_in_use;
	codec_req->init             = isi_msg.getCodecInit();
	codec_req->encoder.AAC      = isi_msg.getAACFlag();
	codec_req->encoder.nsync    = isi_msg.getNsync();
	codec_req->encoder.SFN      = isi_msg.getSFN();
	codec_req->encoder.amr_rate = isi_msg.getAmrEncoder();
	codec_req->encoder.dtx      = isi_msg.getDTX();
	codec_req->encoder.nodata   = 0;
	codec_req->encoder.NSCB     = 0;
	codec_req->decoder.amr_rate = isi_msg.getAmrEncoder();

	switch(mCodecInUse)
	{
	  case ISI_CODEC_AMR_NB:
	  case ISI_CODEC_EFR:
		{
		  t_uint8 dtx = codec_req->encoder.dtx;

		  codec_config->amrnbEncParam.bNoHeader         = 0;
		  codec_config->amrnbEncParam.memory_preset     = MEM_PRESET_DEFAULT;

		  codec_config->amrnbDecParam.bNoHeader         = 0;
		  codec_config->amrnbDecParam.bErrorConcealment = 1;
		  codec_config->amrnbDecParam.memory_preset     = MEM_PRESET_DEFAULT;

		  if(mNetwork == ISI_NETWORK_GSM)
		  {
			codec_config->amrnbDecConfig.ePayloadFormat = AMR_MB_MODEM_GSM_Payload;
			codec_config->amrnbEncConfig.ePayloadFormat = AMR_MB_MODEM_GSM_Payload;
			codec_status->NextSFN         = 0;
			codec_status->RequestedSFN    = -1;
			if(codec_req->encoder.nsync && codec_req->encoder.dtx)
			{
			  codec_status->Nsync   = 12;
			  dtx = 0;
			}
			else
			{
			  codec_status->Nsync   = -1;
			}
		  }
		  else
		  {
			codec_config->amrnbDecConfig.ePayloadFormat = AMR_MB_MODEM_3G_Payload;
			codec_config->amrnbEncConfig.ePayloadFormat = AMR_MB_MODEM_3G_Payload;
		  }
		  codec_config->amrnbEncConfig.nBitRate   = codec_req->encoder.amr_rate;
		  codec_config->amrnbEncConfig.bDtxEnable = dtx;

		  if(mCodecInUse == ISI_CODEC_EFR)
		  {
			codec_config->amrnbEncConfig.bEfr_on = 1;
			codec_config->amrnbDecConfig.bEfr_on = 1;
		  }
          else
          {
			codec_config->amrnbEncConfig.bEfr_on = 0;
			codec_config->amrnbDecConfig.bEfr_on = 0;
          }
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs AMR-NB/EFR (dec payload=%d)  (enc payload=%d)",codec_config->amrnbDecConfig.ePayloadFormat,codec_config->amrnbEncConfig.ePayloadFormat);
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs AMR-NB/EFR (enc nBitRate=%d) (enc bDtxEnable=%d)",codec_config->amrnbEncConfig.nBitRate, codec_config->amrnbEncConfig.bDtxEnable);
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs AMR-NB/EFR (dec bEfr_on=%d)  (enc bEfr_on=%d)",codec_config->amrnbDecConfig.bEfr_on, codec_config->amrnbEncConfig.bEfr_on);

		  break;
		}
	  case ISI_CODEC_GSM_FR:
		{
		  codec_config->gsmfrDecParam.memory_preset    = MEM_PRESET_DEFAULT;
		  codec_config->gsmfrDecConfig.epayload_format = FR_MB_MODEM_PAYLOAD;
		  codec_config->gsmfrDecConfig.bDtx            = 1; // ?

		  codec_config->gsmfrEncParam.memory_preset    = MEM_PRESET_DEFAULT;
		  codec_config->gsmfrEncConfig.epayload_format = FR_MB_MODEM_PAYLOAD;
		  codec_config->gsmfrEncConfig.bDtx            = codec_req->encoder.dtx;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs FR (enc bDtx=%d)",codec_config->gsmfrEncConfig.bDtx);
		  break;
		}
	  case ISI_CODEC_GSM_HR:
		{
		  codec_config->gsmhrDecParam.memory_preset     = MEM_PRESET_DEFAULT;
		  codec_config->gsmhrDecConfig.epayload_format  = HR_MB_MODEM_PAYLOAD;

		  codec_config->gsmhrEncParam.memory_preset     = MEM_PRESET_DEFAULT;
		  codec_config->gsmhrEncConfig.epayload_format  = HR_MB_MODEM_PAYLOAD;
		  codec_config->gsmhrEncConfig.bDtx             = codec_req->encoder.dtx;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs HR (enc bDtx=%d)",codec_config->gsmhrEncConfig.bDtx);
		  break;
		}
	  case ISI_CODEC_AMR_WB:
		{
		  t_uint8 dtx = codec_req->encoder.dtx;

		  codec_config->amrwbDecParam.bNoHeader   = 0;
		  codec_config->amrwbEncParam.bNoHeader   = 0;

          if(mNetwork == ISI_NETWORK_GSM)
		  {
			codec_config->amrwbDecConfig.ePayloadFormat = AMRWB_MB_MODEM_GSM_Payload;
			codec_config->amrwbEncConfig.ePayloadFormat = AMRWB_MB_MODEM_GSM_Payload;
			codec_status->NextSFN         = 0;
			codec_status->RequestedSFN    = -1;
			if(codec_req->encoder.nsync && codec_req->encoder.dtx)
			{
			  codec_status->Nsync  = 12;
			  dtx = 0;
			}
			else
			{
			  codec_status->Nsync = -1;
			}
		  }
		  else
		  {
			codec_config->amrwbDecConfig.ePayloadFormat = AMRWB_MB_MODEM_3G_Payload;
			codec_config->amrwbEncConfig.ePayloadFormat = AMRWB_MB_MODEM_3G_Payload;
		  }

		  codec_config->amrwbEncConfig.nBitRate   = codec_req->encoder.amr_rate;
		  codec_config->amrwbEncConfig.bDtxEnable = dtx;

		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs AMR-WB (dec payload=%d)  (enc payload=%d)",codec_config->amrwbDecConfig.ePayloadFormat,codec_config->amrwbEncConfig.ePayloadFormat);
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs AMR-WB (enc nBitRate=%d) (enc bDtxEnable=%d)",codec_config->amrwbEncConfig.nBitRate, codec_config->amrwbEncConfig.bDtxEnable);

		  break;
		}
	  case ISI_CODEC_PCM8:
		{
		  codec_config->pcmEncConfig.sampling_rate = PCM_SamplingRate_8k;
		  codec_config->pcmDecConfig.sampling_rate = PCM_SamplingRate_8k;
		  codec_config->pcmDecConfig.big_endian_flag = false;
		  codec_config->pcmEncConfig.big_endian_flag = false;
		  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs PCM8 ");
		  break;
		}
	  case ISI_CODEC_PCM16:
		{
		  codec_config->pcmEncConfig.sampling_rate = PCM_SamplingRate_16k;
		  codec_config->pcmDecConfig.sampling_rate = PCM_SamplingRate_16k;
		  codec_config->pcmDecConfig.big_endian_flag = false;
		  codec_config->pcmEncConfig.big_endian_flag = false;
		  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs PCM16 ");
		  break;
		}
	  case ISI_CODEC_NONE:
		{
		  break;
		}
	  default:
		{
		  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/IsiProtocol: configureCodec() Error in codec: reset mNewCodecReqReceived");
		  ARMNMF_DBC_ASSERT(0);
		  return false;
		}
	}
  }
  else
  {
	// reconfigure codec
	switch(mCodecInUse)
	{
	  case ISI_CODEC_AMR_NB:
		{
		  if(mNetwork == ISI_NETWORK_GSM)
		  {
			if(isi_msg.getNsync() && isi_msg.getDTX())
			{
			  //disable DTX for 12 frames
			  codec_config->amrnbEncConfig.bDtxEnable = 0;
			  codec_status->Nsync   = 12;
			}
			else
			{
			  codec_config->amrnbEncConfig.bDtxEnable = isi_msg.getDTX();
			}

			codec_config->amrnbEncConfig.nBitRate = isi_msg.getAmrEncoder();

			if(isi_msg.getSFN() == codec_status->NextSFN)
			  codec_status->RequestedSFN = -1;
			else
            {
              *config_updated = false;
			  codec_status->RequestedSFN = isi_msg.getSFN();
            }
		  }
		  else if (mNetwork == ISI_NETWORK_WCDMA)
		  {
			codec_config->amrnbEncConfig.bDtxEnable = isi_msg.getDTX();
			codec_config->amrnbEncConfig.nBitRate   = isi_msg.getAmrEncoder();
		  }
		  OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/IsiProtocol: Reconfigure codecs AMR-NB (enc nBitRate=%d) (enc bDtxEnable=%d) (SFN=%d)",codec_config->amrnbEncConfig.nBitRate, codec_config->amrnbEncConfig.bDtxEnable, isi_msg.getSFN());
		  break;
		}
	  case ISI_CODEC_GSM_FR:
		{
		  codec_config->gsmfrEncConfig.bDtx = isi_msg.getDTX();
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/IsiProtocol: Reconfigure codecs FR (enc bDtx=%d)",codec_config->gsmfrEncConfig.bDtx);
		  break;
		}
	  case ISI_CODEC_GSM_HR:
		{
		  codec_config->gsmhrEncConfig.bDtx = isi_msg.getDTX();
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/IsiProtocol: Reconfigure codecs HR (enc bDtx=%d)",codec_config->gsmhrEncConfig.bDtx);
		  break;
		}
	  case ISI_CODEC_EFR:
		{
		  codec_config->amrnbEncConfig.bDtxEnable = isi_msg.getDTX();
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/IsiProtocol: Reconfigure codecs EFR (enc bDtx=%d)",codec_config->amrnbEncConfig.bDtxEnable);
		  break;
		}
	  case ISI_CODEC_PCM8:
	  case ISI_CODEC_PCM16:
		{
		  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/IsiProtocol: Reconfigure codecs PCM");
		  *config_updated = false;
		  break;
		}
	  case ISI_CODEC_AMR_WB:
		{
		  if(mNetwork == ISI_NETWORK_GSM)
		  {
			if(isi_msg.getNsync() && isi_msg.getDTX())
			{
			  //disable DTX for 12 frames
			  codec_config->amrwbEncConfig.bDtxEnable = 0;
			  codec_status->Nsync                     = 12;
			}
			else
			{
			  codec_config->amrwbEncConfig.bDtxEnable = isi_msg.getDTX();
			}

			codec_config->amrwbEncConfig.nBitRate = isi_msg.getAmrEncoder();

			if(isi_msg.getSFN() == codec_status->NextSFN)
			  codec_status->RequestedSFN = -1;
			else {
              *config_updated = false;
			  codec_status->RequestedSFN = isi_msg.getSFN();
            }
		  }
		  else if (mNetwork == ISI_NETWORK_WCDMA)
		  {
			codec_config->amrwbEncConfig.bDtxEnable = isi_msg.getDTX();
			codec_config->amrwbEncConfig.nBitRate   = isi_msg.getAmrEncoder();
		  }
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: Reconfigure codecs AMR-WB (enc nBitRate=%d) (enc bDtxEnable=%d)",codec_config->amrwbEncConfig.nBitRate, codec_config->amrwbEncConfig.bDtxEnable);
		  break;
		}
	  case ISI_CODEC_NONE:
		{
		   OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/IsiProtocol: Reconfigure codec NONE");
		  *config_updated = false;
		  break;
		}
	  default:
		{
		  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/IsiProtocol: configureCodec() Error in codec: reset mNewCodecReqReceived");
		  ARMNMF_DBC_ASSERT(0);
		  return false;
		}
	}
  }



  if(mCodecInUse == ISI_CODEC_AMR_WB || mCodecInUse == ISI_CODEC_AMR_NB)
  {
	OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs  (Nsync=%d) (encDtx=%d)",codec_status->Nsync, codec_status->encDtx);
	OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: Configure codecs  (RequestedSFN=%d) (NextSFN=%d)",codec_status->RequestedSFN, codec_status->NextSFN);
  }

  // answer to modem
  IsiMessage answer_msg(answer[0]->pBuffer,MODEM_AUDIO_SPEECH_CODEC_RESP,
						mIsiTopHeader[CODEC_IDX], SPC_RESP_OK, answer[0]);
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/IsiProtocol: send MODEM_AUDIO_SPEECH_CODEC_RESP SPC_RESP_OK to Modem");

  return true;
}


bool IsiProtocol::process_downlink_timing_configuration(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
														t_uint32 *time, bool *time_updated)
{
  // no such message in Isi protocol
  return false;
}


t_uint32 IsiProtocol::getTimingValue(IsiMessage &isi_msg, t_uint16 &response)
{
  t_uint32 timing;

  timing  = ((t_uint32)isi_msg.getTimingHigh())<<16;
  timing += (t_uint32)(isi_msg.getTimingLow());
  OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/IsiProtocol: MODEM_AUDIO_SPC_TIMING_REQ (timing = %dus) (timing_high = %d , timing_low = %d )", timing,isi_msg.getTimingHigh(),isi_msg.getTimingLow());

  if(timing < CSCALL_MINIMAL_TIMING_VALUE) //10ms
  {
	timing += 20000;
  }

  if (timing >= CSCALL_MAXIMAL_TIMING_VALUE) //1s
  {
	OstTraceFiltInst2 (TRACE_ERROR, "Cscall/IsiProtocol:ERROR!! timing (%d us) >= (%d) send MODEM_AUDIO_SPC_TIMING_RESP with SPC_TIMING_FAIL !!!!!!!!!!!!!", timing, CSCALL_MAXIMAL_TIMING_VALUE);
	timing = CSCALL_DEFAULT_TIMING_VALUE;
	response = SPC_TIMING_FAIL;
  }

  return timing;
}

bool IsiProtocol::process_timing_report(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
                                        unsigned long long next_uplink_deliver_date,
                                        t_uint32 *time, bool *time_updated)


{
  IsiMessage isi_msg(msg->pBuffer);
  t_uint16   response = SPC_TIMING_OK;
  storeIsiTopHeader(TIMING_IDX, isi_msg);

  *time_updated = true;
  *time = getTimingValue(isi_msg, response);
  if(*time == 0)
  {
	*time_updated = false;
  }
  else
  {
	*time_updated = true;
	OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/IsiProtocol: send new timing (%d us) to uplink", *time);
  }

  // answer Modem
  IsiMessage  answer_isi(answer[0]->pBuffer,MODEM_AUDIO_SPC_TIMING_RESP,
						 mIsiTopHeader[TIMING_IDX], response, answer[0]);
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/IsiProtocol: send MODEM_AUDIO_SPC_TIMING_RESP to Modem");

  return true;
}


bool IsiProtocol::process_speech_data_downlink(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE *decoder)
{
  IsiMessage  isi_msg(msg->pBuffer);
  t_uint32   *frame_ptr  = (t_uint32 *)isi_msg.getFramePtr();
  t_uint16    frame_size = isi_msg.getFrameLength();
  t_uint16    i;

  for(i=0;i<frame_size+3;i+=4)
  {
	t_uint32 tmp = *frame_ptr++;
	decoder->pBuffer[i]   = (tmp & 0xFF000000) >> 24;
	decoder->pBuffer[i+1] = (tmp & 0xFF0000)   >> 16;
	decoder->pBuffer[i+2] = (tmp & 0xFF00)     >> 8;
	decoder->pBuffer[i+3] = (tmp & 0xFF);
  }


  decoder->nFilledLen = frame_size;
  decoder->nOffset    = 0;

  return true;
}

bool IsiProtocol::set_modem_loop_mode(OMX_BUFFERHEADERTYPE *msg,
										Cscall_ModemLoop_Config_t *loop_config )
{
  return false;
}



bool IsiProtocol::process_request_for_loop(OMX_BUFFERHEADERTYPE *msg,OMX_BUFFERHEADERTYPE **answer)
{
   // no such message in Isi protocol
  return false;
}

bool IsiProtocol::process_voice_call_status(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer, bool *connected)
{
  // no such message in Isi protocol
  return false;
}


bool IsiProtocol::process_speech_data_uplink(OMX_BUFFERHEADERTYPE *encoder_buffer, OMX_BUFFERHEADERTYPE *msg,
											 Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
											 bool *config_updated)
{
  IsiMessage  isi_msg(msg->pBuffer,MODEM_AUDIO_SPC_ENCODER_DATA_NTF, mIsiTopHeader[CODEC_IDX],
					  encoder_buffer->nFilledLen, msg);
  t_uint32   *frame_ptr = (t_uint32 *)isi_msg.getFramePtr();

  isi_msg.setDataFormat(mCodecInUse);

  for(t_uint16 i=encoder_buffer->nOffset;i<(encoder_buffer->nFilledLen+encoder_buffer->nOffset)+3;i+=4)
  {
	t_uint32 tmp;
	tmp  = (t_uint32)(encoder_buffer->pBuffer[i]     << 24 |
					  encoder_buffer->pBuffer[i + 1] << 16 |
					  encoder_buffer->pBuffer[i + 2] << 8  |
					  encoder_buffer->pBuffer[i + 3]);
	*frame_ptr++ = tmp;
  }

  // Check AMR/AMR-WB config
  *config_updated = false;

  if (mNetwork == ISI_NETWORK_GSM) {
	if(mCodecInUse == ISI_CODEC_AMR_NB) {
	  MODEM_AUDIO_AMR_FRAME_T    *amrnb_frame = (MODEM_AUDIO_AMR_FRAME_T *)isi_msg.getFramePtr();
	  codec_status->NextSFN = (amrnb_frame->SFN + 1 ) & 7;
	  OstTraceFiltInst4 (TRACE_FLOW, "Cscall/IsiProtocol: sendCodecFrame() frame(NB): FrameType=%d TxType=%d SFN=%d NextSFN=%d",amrnb_frame->FrameType,amrnb_frame->TxType,amrnb_frame->SFN, codec_status->NextSFN);
    }
	else if (mCodecInUse == ISI_CODEC_AMR_WB) {
	  MODEM_AUDIO_AMR_WB_FRAME_T *amrwb_frame = (MODEM_AUDIO_AMR_WB_FRAME_T *)isi_msg.getFramePtr();
	  codec_status->NextSFN = (amrwb_frame->SFN + 1 ) & 7;
	  OstTraceFiltInst4 (TRACE_FLOW, "Cscall/IsiProtocol: sendCodecFrame() frame(WB): FrameType=%d TxType=%d SFN=%d NextSFN=%d",amrwb_frame->FrameType,amrwb_frame->TxType,amrwb_frame->SFN, codec_status->NextSFN);

    }
  }

  // can be put in generic part ?
  if(((codec_status->Nsync != -1)   || (codec_status->RequestedSFN != -1)) &&
	 (((mCodecInUse == ISI_CODEC_AMR_NB) || (mCodecInUse == ISI_CODEC_AMR_WB)) && (mNetwork == ISI_NETWORK_GSM)))
  {
	if(codec_status->Nsync > 0)
	{
	  codec_status->Nsync--;
	  if(codec_status->Nsync == 0)
	  {
		codec_status->Nsync = -1;
		if(mCodecInUse == ISI_CODEC_AMR_NB)
		  codec_config->amrnbEncConfig.bDtxEnable = codec_status->encDtx;
		else
		  codec_config->amrwbEncConfig.bDtxEnable = codec_status->encDtx;

		*config_updated = true;
	  }
	}

	if(codec_status->RequestedSFN == codec_status->NextSFN)
	{
	  *config_updated = true;
	  codec_status->RequestedSFN = -1;
    }

	OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: sendCodecFrame() pending reconfiguration (Nsync=%d) (encDtx=%d)",codec_status->Nsync, codec_status->encDtx);
	OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/IsiProtocol: sendCodecFrame() pending reconfiguration (RequestedSFN=%d) (NextSFN=%d)",codec_status->RequestedSFN, codec_status->NextSFN);
  }

  return true;
}


////////////////////////
// Helper function
////////////////////////
void IsiProtocol::storeIsiTopHeader(indexname idx, IsiMessage &msg)
{
  mIsiTopHeader[idx].media	         = msg.getMedia();
  mIsiTopHeader[idx].sender_dev	     = msg.getSenderDev();
  mIsiTopHeader[idx].sender_obj	     = msg.getSenderObj();
  mIsiTopHeader[idx].transsaction_id = msg.getTransactionId();
}


CsCallCodec_t IsiProtocol::isi2CscallCodec(ISI_CODEC_NAME_T codec)
{
  switch(codec)
  {
	case ISI_CODEC_NONE   : return CODEC_NONE;
    case ISI_CODEC_GSM_FR : return CODEC_GSM_FR;
    case ISI_CODEC_GSM_HR : return CODEC_GSM_HR;
    case ISI_CODEC_EFR    : return CODEC_EFR;
    case ISI_CODEC_AMR_NB : return CODEC_AMR_NB;
    case ISI_CODEC_AMR_WB : return CODEC_AMR_WB;
    case ISI_CODEC_PCM8   : return CODEC_PCM8;
    case ISI_CODEC_PCM16  : return CODEC_PCM16;
	default               : return CODEC_NONE;
  }
}

CsCallNetwork_t IsiProtocol::isi2CscallNetwork(ISI_NETWORK_NAME_T network)
{
  switch(network)
  {
	case ISI_NETWORK_NONE  : return NETWORK_NONE;
	case ISI_NETWORK_GSM   : return NETWORK_GSM;
	case ISI_NETWORK_WCDMA : return NETWORK_WCDMA;
	default                : return NETWORK_NONE;
  }
}



