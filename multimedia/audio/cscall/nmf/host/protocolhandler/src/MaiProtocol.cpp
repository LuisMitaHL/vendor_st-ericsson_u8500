/*****************************************************************************/
/*
 *  (c) ST-Ericsson, 2010 - All rights reserved
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson
 *
 */

/**
 * \file   MaiProtocol.cpp
 * \brief  implementation of Protocol class for ISI protocol.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/protocolhandler.nmf>
#include "cscall/nmf/host/protocolhandler/inc/MaiProtocol.hpp"
#include <string.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_protocolhandler_src_MaiProtocolTraces.h"
#endif

#ifdef USE_SAMC
#include "samc_client.h"
#endif

/* Defines for Conversion functions */
#define PROT_AMR_RATE_SID        8
#define PROT_AMR_RATE_NO_DATA   15
#define PROT_AMRWB_RATE_SID      9
#define PROT_AMRWB_RATE_NO_DATA 15

#define ISI_HR_FRAME_SIZE (MAI_HR_FRAME_SIZE + 4)

/* Typedefs  */
typedef struct 
{ 
  t_uint16 Reserved;
  t_uint16 Control;
  t_uint8  SpeechData[MAI_AMR_WB_FRAME_SIZE]; // Max frame size
} ISI_SpeechFrame_t;

/* Static funtions */

/* AMR Functions */
static MAI_AMR_TX_Type_t AMR_ConvertTX_Type(t_uint8 *const Rate, const t_uint8 RateByte);
static t_uint16 AMR_ConvertRX_Type(const MAI_AMR_NB_Rate_t    Rate,    /* In  */
                                   const MAI_AMR_NB_RX_Type_t RX_Type); /* In  */

static t_uint16 AMRWB_ConvertRX_Type(const MAI_AMR_WB_Rate_t    Rate,     /* In  */
                                     const MAI_AMR_WB_RX_Type_t RX_Type); /* In  */

static t_uint16 swapEndianess16(t_uint16 data) {return (((data & 0xFF)<<8) | ((data & 0xFF00) >> 8));}


////////////////////////
// constructor
////////////////////////
MaiProtocol::MaiProtocol(void):
  UL_TimingWM_Low(4000), /* Low WaterMark in ms  */
  UL_TimingWM_High(12000),/* High WaterMark in ms */
  UL_TIMING_WM_ADJUST(UL_TimingWM_Low + ((UL_TimingWM_High - UL_TimingWM_Low) / 2)) /* AdjustTime in ms */
{

  mCodecInUse = MAI_SPEECH_CODEC_NONE;
  mNetwork    = MAI_SYSTEM_NONE;

  DL_MaxProcTime     = 0;
  DL_CurrentProcTime = 0;

#ifdef USE_SAMC
  if (SAMC_CLIENT_OK != SAMC_Client_Start())
  {
    SAMC_Opened = false;
    OstTraceFiltInst0(TRACE_ERROR,"Cscall/MaiProtocol: SAMC Client Start... FAILED\n");
  }
  else
  {
    SAMC_Opened = true;
    OstTraceFiltInst0(TRACE_ALWAYS,"Cscall/MaiProtocol: SAMC Client Start... OK\n");
  }
#endif
}


////////////////////////
// destructor
////////////////////////
MaiProtocol::~MaiProtocol(void)
{
#ifdef USE_SAMC
  if (SAMC_Opened)
  {
    if (SAMC_CLIENT_OK != SAMC_Client_Stop())
      OstTraceFiltInst0(TRACE_ERROR,"Cscall/MaiProtocol: SAMC Client Stop... FAILED\n");
    else
      OstTraceFiltInst0(TRACE_ALWAYS,"Cscall/MaiProtocol: SAMC Client Stop... OK\n");
  }
#endif
}


////////////////////////
// Protocol interface 
////////////////////////
CsCallMessage_t MaiProtocol::getMsgType(OMX_BUFFERHEADERTYPE *msg)
{
  MAI_MessageHeader_t *messageHeader = (MAI_MessageHeader_t *) msg->pBuffer;

  switch(messageHeader->MessageId)
  {
    case MAI_MESSAGE_INFORMATION_REQUEST :    return MSG_MODEM_INFORMATION_REQUEST;
    case MAI_MESSAGE_INFORMATION_RESPONSE :   return MSG_MODEM_INFORMATION_RESPONSE;
    case MAI_MESSAGE_CODING_FORMAT :          return MSG_CODING_FORMAT_REQUEST;
    case MAI_MESSAGE_CODING_FORMAT_RESPONSE : return MSG_CODING_FORMAT_RESPONSE;
    case MAI_MESSAGE_VOICE_CALL_STATUS :      return MSG_VOICE_CALL_STATUS;
    case MAI_MESSAGE_UPLINK_TIMING_CONFIG :   return MSG_UPLINK_TIMING_CONFIGURATION;
    case MAI_MESSAGE_DOWNLINK_TIMING_CONFIG : return MSG_DOWNLINK_TIMING_CONFIGURATION;
    case MAI_MESSAGE_TIMING_REPORT :          return MSG_TIMING_REPORT;
    case MAI_MESSAGE_DOWNLINK_SPEECH_DATA :   return MSG_SPEECH_DATA_DOWNLINK;
    case MAI_MESSAGE_UPLINK_SPEECH_DATA :     return MSG_SPEECH_DATA_UPLINK;
    case MAI_MESSAGE_TEST_MODEM_LOOPBACK :       return MSG_TEST_MODEM_LOOP_REQUEST;
    case MAI_MESSAGE_TEST_MODEM_LOOPBACK_RESPONSE : return MSG_TEST_MODEM_LOOP_RESPONSE;
    case MAI_MESSAGE_TEST_AUDIO_LOOPBACK :        return MSG_TEST_AUDIO_LOOP_REQUEST;
    case MAI_MESSAGE_TEST_AUDIO_LOOPBACK_RESPONSE :   return MSG_TEST_AUDIO_LOOP_RESPONSE;

    default :                                 return MSG_TIMING_REPORT_RESPONSE;
  }
}

t_uint16 MaiProtocol::getMsgLength(OMX_BUFFERHEADERTYPE *msg)
{
  MAI_MessageHeader_t *mai_msg = (MAI_MessageHeader_t *) msg->pBuffer;

  return (((t_uint16)(mai_msg->MessageSize[0]<<8)) | (t_uint16)(mai_msg->MessageSize[1]));
}

t_uint16 MaiProtocol::getNbAnswer(CsCallMessage_t msg_type)
{
  switch(msg_type)
  {
    case MSG_MODEM_INFORMATION_REQUEST:     return 0;    
    case MSG_MODEM_INFORMATION_RESPONSE:    return 0;     
    case MSG_CODING_FORMAT_REQUEST:         return 2;          
    case MSG_CODING_FORMAT_RESPONSE:        return 0;         
    case MSG_UPLINK_TIMING_CONFIGURATION:   return 0;    
    case MSG_DOWNLINK_TIMING_CONFIGURATION: return 0;  
    case MSG_TIMING_REPORT:                 return 0;                  
    case MSG_TIMING_REPORT_RESPONSE:        return 0;         
    case MSG_SPEECH_DATA_DOWNLINK:          return 0;           
    case MSG_SPEECH_DATA_UPLINK:            return 0;
    case MSG_VOICE_CALL_STATUS:             return 0;
    case MSG_TEST_MODEM_LOOP_REQUEST:       return 0;
    case MSG_TEST_MODEM_LOOP_RESPONSE:      return 0;
    case MSG_TEST_AUDIO_LOOP_REQUEST:       return 1;
    case MSG_TEST_AUDIO_LOOP_RESPONSE:      return 0;
    default:                                return 0;
  }
}


bool MaiProtocol::process_modem_information_response(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer)
{
  MAI_InformationResponse_t *InfoModemResp_p = (MAI_InformationResponse_t*) msg->pBuffer;

/*
  OstTraceFiltInst4(TRACE_ALWAYS,"Cscall/MaiProtocol: Modem information request : ModemId = %s ModemVersion = %s VersionMajor = %d VersionMinor = %d\n",
		  InfoModemResp_p ->ModemId,
		  InfoModemResp_p ->ModemVersionString,
		  InfoModemResp_p ->VersionMajor,
		  InfoModemResp_p ->VersionMinor);
*/
  OstTraceFiltInst1(TRACE_ALWAYS,"Cscall/MaiProtocol: Modem version = %d\n",InfoModemResp_p ->VersionMajor);

  return true; 
}


bool MaiProtocol::is_end_of_call(OMX_BUFFERHEADERTYPE *msg)
{

  MAI_CodingFormat_t *ste_msg = (MAI_CodingFormat_t *) msg->pBuffer;
  MAI_SpeechCodec_t  newcodec = ste_msg->SpeechCodec;

  if((newcodec != mCodecInUse) &&
     newcodec == MAI_SPEECH_CODEC_NONE)
  {
    return true;
  }

  return false;
}


bool MaiProtocol::process_request_for_loop(OMX_BUFFERHEADERTYPE *msg,OMX_BUFFERHEADERTYPE **answer)
{
  // Answer to modem
   OMX_BUFFERHEADERTYPE * answer_buffer       = answer[0];

   /* Coding Format response */
   MAI_TestModemLoopBackResponse_t * loopSupportStatus  = (MAI_TestModemLoopBackResponse_t *)answer_buffer->pBuffer;
   loopSupportStatus->MessageHeader.MessageId      = MAI_MESSAGE_TEST_AUDIO_LOOPBACK_RESPONSE;
   loopSupportStatus->MessageHeader.MessageSize[1] = sizeof(MAI_TestAudioLoopBackResponse_t);
   loopSupportStatus->MessageHeader.MessageSize[0] = 0;
   loopSupportStatus->Result    = MAI_LOOPBACK_NOT_SUPPORTED;
   
  return true;
}


bool MaiProtocol::process_coding_format_request(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
                                                Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
                                                CsCallCodecReq_t *codec_req,
                                                CsCallCodec_t *codec_in_use, CsCallNetwork_t *network_in_use,
                                                bool *config_updated, t_uint32 *time, bool *time_updated)
{
  MAI_CodingFormat_t *codingFormat = (MAI_CodingFormat_t*) msg->pBuffer;
  MAI_SpeechCodec_t   newCodec     = codingFormat->SpeechCodec;
  MAI_System_t        newNetwork   = codingFormat->System;
  
  /* Start TX timer */
  *time_updated = false;
  if (mCodecInUse == MAI_SPEECH_CODEC_NONE && newCodec != MAI_SPEECH_CODEC_NONE)
  {
    *time = 20000;
    *time_updated = true;
  }

  *config_updated = true;
  // If New Codec,
  if((mCodecInUse != newCodec) || (mNetwork != newNetwork))
  {
    /* Save new codec and system */
    mCodecInUse     = newCodec;
    *codec_in_use   = mai2CscallCodec(newCodec);
    mNetwork        = newNetwork;
    *network_in_use = mai2CscallNetwork(newNetwork);
    
    
    // forward config to proxy
    codec_req->speech_codec     = *codec_in_use;
    codec_req->network          = *network_in_use;
    codec_req->init             = 1;
    codec_req->encoder.AAC      = codingFormat->NoiseSupressionControlBit;
    codec_req->encoder.nsync    = codingFormat->NSYNC;
    codec_req->encoder.SFN      = codingFormat->FrameNumber;
	codec_req->encoder.amr_rate = codingFormat->AMR_Rate.AMR_NB;
    codec_req->encoder.dtx      = codingFormat->DTX;
	codec_req->encoder.nodata   = codingFormat->NoDataUsage;
	codec_req->encoder.NSCB     = codingFormat->NoiseSupressionControlBit;
	codec_req->decoder.amr_rate = codingFormat->AMR_Rate.AMR_NB;
	
    switch(mCodecInUse)
    {
      case MAI_SPEECH_CODEC_AMR_NB:
	  case MAI_SPEECH_CODEC_EFR:
		{
		  codec_config->amrnbEncParam.bNoHeader         = 0;
		  codec_config->amrnbEncParam.memory_preset     = MEM_PRESET_DEFAULT;
		  
		  codec_config->amrnbDecParam.bNoHeader         = 0;
		  codec_config->amrnbDecParam.bErrorConcealment = 1;
		  codec_config->amrnbDecParam.memory_preset     = MEM_PRESET_DEFAULT;
		  
		  if(newNetwork == MAI_SYSTEM_2G)
		  {
			codec_config->amrnbDecConfig.ePayloadFormat = AMR_MB_MODEM_GSM_Payload;
			codec_config->amrnbEncConfig.ePayloadFormat = AMR_MB_MODEM_GSM_Payload;
		  }
		  else // if (newNetwork == MAI_SYSTEM_3G)
		  { 
			codec_config->amrnbDecConfig.ePayloadFormat = AMR_MB_MODEM_3G_Payload;
			codec_config->amrnbEncConfig.ePayloadFormat = AMR_MB_MODEM_3G_Payload;
		  }
		  
		  codec_config->amrnbEncConfig.nBitRate   = codec_req->encoder.amr_rate;
		  codec_config->amrnbEncConfig.bDtxEnable = codec_req->encoder.dtx;
		  
		  if(mCodecInUse == MAI_SPEECH_CODEC_EFR)
		  {
			codec_config->amrnbEncConfig.bEfr_on = 1;
			codec_config->amrnbDecConfig.bEfr_on = 1;
		  }
		  else
		  {
			codec_config->amrnbEncConfig.bEfr_on = 0;
			codec_config->amrnbDecConfig.bEfr_on = 0;
		  }
		  break;
		}
      case MAI_SPEECH_CODEC_FR:
		{
		  codec_config->gsmfrDecParam.memory_preset     = MEM_PRESET_DEFAULT;
		  codec_config->gsmfrDecConfig.epayload_format  = FR_FB_MODEM_PAYLOAD;  
		  codec_config->gsmfrDecConfig.bDtx             = codingFormat->DTX; // ?

		  codec_config->gsmfrEncParam.memory_preset     = MEM_PRESET_DEFAULT;
		  codec_config->gsmfrEncConfig.epayload_format  = FR_FB_MODEM_PAYLOAD;  
		  codec_config->gsmfrEncConfig.bDtx             = codingFormat->DTX;
		  break;
		}
      case MAI_SPEECH_CODEC_HR:
		{
		  codec_config->gsmhrDecParam.memory_preset     = MEM_PRESET_DEFAULT;
		  codec_config->gsmhrDecConfig.epayload_format  = HR_FB_MODEM_PAYLOAD;

		  codec_config->gsmhrEncParam.memory_preset     = MEM_PRESET_DEFAULT;
		  codec_config->gsmhrEncConfig.epayload_format  = HR_FB_MODEM_PAYLOAD;
		  codec_config->gsmhrEncConfig.bDtx             = codingFormat->DTX;
		  break;
		}
      case MAI_SPEECH_CODEC_AMR_WB:
		{
		  codec_config->amrwbDecParam.bNoHeader   = 0;
		  codec_config->amrwbEncParam.bNoHeader   = 0;
		  
		  if(mNetwork == MAI_SYSTEM_2G)
		  {
			codec_config->amrwbDecConfig.ePayloadFormat = AMRWB_MB_MODEM_GSM_Payload;
			codec_config->amrwbEncConfig.ePayloadFormat = AMRWB_MB_MODEM_GSM_Payload;
		  }
		  else
		  {
			codec_config->amrwbDecConfig.ePayloadFormat = AMRWB_MB_MODEM_3G_Payload;
			codec_config->amrwbEncConfig.ePayloadFormat = AMRWB_MB_MODEM_3G_Payload;
		  }

		  codec_config->amrwbEncConfig.nBitRate   = codec_req->encoder.amr_rate;
		  codec_config->amrwbEncConfig.bDtxEnable = codec_req->encoder.dtx;
        break;
		}
      case MAI_SPEECH_CODEC_PCM8:
		{
		  codec_config->pcmDecConfig.sampling_rate = PCM_SamplingRate_8k;
		  codec_config->pcmEncConfig.sampling_rate = PCM_SamplingRate_8k;
		  codec_config->pcmDecConfig.big_endian_flag = true;
		  codec_config->pcmEncConfig.big_endian_flag = true;
		  break;
		}
      case MAI_SPEECH_CODEC_PCM16:
		{
		  codec_config->pcmDecConfig.sampling_rate = PCM_SamplingRate_16k;
		  codec_config->pcmEncConfig.sampling_rate = PCM_SamplingRate_16k;
		  codec_config->pcmDecConfig.big_endian_flag = true;
		  codec_config->pcmEncConfig.big_endian_flag = true;
		  break;
		}
      case MAI_SPEECH_CODEC_NONE:
        {
          break;
        }
      default:
        {
          OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/MaiProtocol: configureCodec() Error in codec: reset mNewCodecReqReceived");
          ARMNMF_DBC_ASSERT(0);
          return false;
        }
    }
  }
  else // Else only config Bitrate and/or DTX mode
  {
    // reconfigure codec
    switch(mCodecInUse)
    {
      case MAI_SPEECH_CODEC_AMR_NB:
      case MAI_SPEECH_CODEC_EFR:
		{
		  codec_config->amrnbEncConfig.bDtxEnable = codingFormat->DTX;
		  codec_config->amrnbEncConfig.nBitRate   = codingFormat->AMR_Rate.AMR_NB;
		  break;
		}
      case MAI_SPEECH_CODEC_FR:
		{
		  codec_config->gsmfrEncConfig.bDtx = codingFormat->DTX;
		  break;
		}
      case MAI_SPEECH_CODEC_HR:
		{
		  codec_config->gsmhrEncConfig.bDtx = codingFormat->DTX;
		  break;
		}
      case MAI_SPEECH_CODEC_AMR_WB:
		{
		  codec_config->amrwbEncConfig.nBitRate   = codingFormat->DTX;
		  codec_config->amrwbEncConfig.bDtxEnable = codingFormat->AMR_Rate.AMR_WB;
		  break;
		}
      case MAI_SPEECH_CODEC_PCM16:
      case MAI_SPEECH_CODEC_PCM8:
      case MAI_SPEECH_CODEC_NONE:
		{
		  *config_updated = false;
		  break;
		}
      default:
		{
		  return false;
		}
    }
  }

  // answer to modem (Codec Config & UL Timing Config)
  OMX_BUFFERHEADERTYPE * answer_buffer       = answer[0];
  OMX_BUFFERHEADERTYPE * answer_bufferTiming = answer[1];

  /* Coding Format response */   
  MAI_CodingFormatResponse_t *codingFormatResponse = (MAI_CodingFormatResponse_t *)answer_buffer->pBuffer;
  codingFormatResponse->MessageHeader.MessageId   = MAI_MESSAGE_CODING_FORMAT_RESPONSE;
  codingFormatResponse->MessageHeader.MessageSize[1]     = sizeof(MAI_CodingFormatResponse_t);
  codingFormatResponse->MessageHeader.MessageSize[0]     = 0;
  codingFormatResponse->CodingConfigResponse    = MAI_CODING_CONFIG_OK;
  codingFormatResponse->NoDataUsage = MAI_NO_DATA_USAGE_TRUE;

  /* Uplink Timing Command */   
  MAI_UplinkTiming_t  *uplinkTimingCommand = (MAI_UplinkTiming_t  *)answer_bufferTiming->pBuffer;
  uplinkTimingCommand->MessageHeader.MessageId        = MAI_MESSAGE_UPLINK_TIMING_CONFIG;
  uplinkTimingCommand->MessageHeader.MessageSize[1]          = sizeof(MAI_UplinkTiming_t);
  uplinkTimingCommand->MessageHeader.MessageSize[0]          = 0;
  uplinkTimingCommand->WaterMarkLow[3]  = ( (UL_TimingWM_Low & 0x000000ff) );
  uplinkTimingCommand->WaterMarkLow[2]  = ( (UL_TimingWM_Low & 0x0000ff00) >> 8);
  uplinkTimingCommand->WaterMarkLow[1]  = ( (UL_TimingWM_Low & 0x00ff0000) >> 16);
  uplinkTimingCommand->WaterMarkLow[0]  = 0;
  uplinkTimingCommand->WaterMarkHigh[3] = ( (UL_TimingWM_High & 0x000000ff) );
  uplinkTimingCommand->WaterMarkHigh[2] = ( (UL_TimingWM_High & 0x0000ff00) >> 8);
  uplinkTimingCommand->WaterMarkHigh[1] = ( (UL_TimingWM_High & 0x00ff0000) >> 16);
  uplinkTimingCommand->WaterMarkHigh[0] = 0;

  /* Set Aux Buffer info */
  answer_buffer->nFilledLen = sizeof(MAI_CodingFormatResponse_t);
  answer_buffer->nOffset    = 0;
  answer_buffer->nFlags     = 0;

  answer_bufferTiming->nFilledLen = sizeof(MAI_UplinkTiming_t);
  answer_bufferTiming->nOffset    = 0;
  answer_bufferTiming->nFlags     = 0;

  return true;
}


bool MaiProtocol::process_downlink_timing_configuration(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
                                                        t_uint32 *time, bool *time_updated)
{
  MAI_DownlinkTiming_t *DL_TmingCmd_p = (MAI_DownlinkTiming_t*) msg->pBuffer;
  t_uint32 b1,b2,b3,b4;

  /* Get processing time from Message */
  b1 = DL_TmingCmd_p->ProcessingTime[0];
  b2 = DL_TmingCmd_p->ProcessingTime[1] << 8;
  b3 = DL_TmingCmd_p->ProcessingTime[2] << 16;
  b4 = DL_TmingCmd_p->ProcessingTime[3] << 24;
  DL_MaxProcTime = b1 | b2 | b3 | b4;
  
  OstTraceFiltInst1(TRACE_ALWAYS,"Cscall/MaiProtocol: MAI_MESSAGE_DOWNLINK_TIMING_CONFIG, DL_MaxProcTime: %d\n", DL_MaxProcTime);
  return true; 
}

bool MaiProtocol::process_timing_report(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
                                        unsigned long long next_uplink_deliver_date,
                                        t_uint32 *time, bool *time_updated)
{
  t_uint32 TimeDiff = 0;
  int NewsyncTime = 0;
  unsigned long long actualTr = 0;

  MAI_TimingReport_t *TimingReportData_p = (MAI_TimingReport_t*) msg->pBuffer;

  //nTimeStamp represents the timeStamp linked to the reception of TimeReport. This field is by default a signed long long value
  unsigned long long timereport_delivery_date = msg->nTimeStamp;

  TimeDiff = ( (TimingReportData_p->TimeDiff[3])       | (TimingReportData_p->TimeDiff[2] << 8 ) |
               (TimingReportData_p->TimeDiff[1] << 16) | (TimingReportData_p->TimeDiff[0] << 24) );

  /* If too late or early, then resync */
  if ( (TimingReportData_p->Cause == MAI_TIMING_CAUSE_TOO_LATE) ||
       (TimingReportData_p->Cause == MAI_TIMING_CAUSE_TOO_EARLY) )
  {

	/* Note : ULDeliveryTime = nextULDeliveryDate - 2000 */
	 if ((next_uplink_deliver_date - 20000) > timereport_delivery_date)
	 {
	    	 OstTraceFiltInst2(TRACE_ERROR, "Cscall/MaiProtocol: TimeReport arrive before sending UL frame. TimeReportDeliverydate : %d,nextULDeliveryDate : %d\n",(unsigned int) timereport_delivery_date,(unsigned int) next_uplink_deliver_date);
	 }

     // NewsyncTime : represents the delta time which must separate the sending of the new uplink frame versus the previous one.
	 // TimeDiff : represents the value associated to the TimeReport message returned by the Modem.
	 // UL_TIMING_WM_ADJUST : represents the offset needed to reach the middle of the acceptance window.
	 // next_uplink_deliver_date  : represents the standard time when the next uplink frame is supposed to be send by the APE.
	 //                       usually, nextDeliveryDate is equal to ULDeliveryDate + 20.
	 // timereport_delivery_date : represents the time when the TimeReport has been received by the APE.
	 //
	 //   Modem                                  APE
	 //    |                                      |
	 //    |<-------------Data_UL-----------------|
	 //    |--------------Report(timeDiff)------->|
	 //    |                                      |
	 //    || t_high                              |
	 //    ||                                     |
	 //    || t_low                               |
	 //    |                                      |
	 //    |-----Data DL------------------------->|
	 //    |                                      |
	 //    |                                      |
	 //    || t_high                              |
	 //    ||<-------------Data_UL----------------|
	 //    || t_low                               |
	 //    |                                      |
	 //
	 // NOTE :
	 // The parameter UL_TIMING_WM_ADJUST is a t_uint32
	 // The parameter TimeDiff is a t_uint32
	 // The parameter timereport_delivery_date is a signed long long which has been casted already in unsigned long long
	 // The parameter next_uplink_deliver_date is an unsigned long long


	 actualTr = next_uplink_deliver_date - timereport_delivery_date; // Note : Value which should be always positive

	 NewsyncTime = (int) actualTr + (int) TimeDiff - (int) UL_TIMING_WM_ADJUST;

	 OstTraceFiltInst4(TRACE_ALWAYS, "Cscall/MaiProtocol: TimeReportCompValues: %d,timeReportDeliverydate : %d,next_uplink_deliver_date : %d,TimeDiff : %d,Window : %d\n",(unsigned int) timereport_delivery_date,(unsigned int) next_uplink_deliver_date,(unsigned int) TimeDiff,(unsigned int) UL_TIMING_WM_ADJUST);

	 // We check that the result is somehow realist... Usually it should be around 20000 ms +/- correction...
	 if ((NewsyncTime < 0) || (NewsyncTime > 80000))
	 {
		 OstTraceFiltInst1(TRACE_ERROR, "Cscall/MaiProtocol: Wrong computation since NewsyncTime is out of range : %d \n",(unsigned int) NewsyncTime);
		 *time_updated = false;
		 *time = 20000; // We need at least to put a value...20 is not ideal but not worse than 0...Anyway this value is not used since
		             // time_update is equal to false...
	 }
	 else
	 {
		 OstTraceFiltInst1(TRACE_ALWAYS, "Cscall/MaiProtocol: NewsyncTime computation : %d \n",(unsigned int) NewsyncTime);
		 *time = (t_uint32) NewsyncTime;
		 *time_updated = true;
	 }
  }
  else if (TimingReportData_p->Cause == MAI_TIMING_CAUSE_PACKAGE_DISCARD)
  {
    /* Do Nothing */
    *time_updated = false;
  } 

  OstTraceFiltInst4(TRACE_ALWAYS, "Cscall/MaiProtocol: MAI_MESSAGE_TIMING_REPORT: TimeDiff: %d, Cause: %d ULAdjust:%d, (NewSync: %d)\n", (unsigned int)TimeDiff, (int)TimingReportData_p->Cause, UL_TIMING_WM_ADJUST, (unsigned int)NewsyncTime);
  return true;
}

bool MaiProtocol::process_speech_data_downlink(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE *decoder)
{
  MAI_SpeechDataDL_t *speechData = (MAI_SpeechDataDL_t*) msg->pBuffer;
  
  /* struct to temporay hold the STE Modem Data in ISI format (for AMR and EFR)*/
  ISI_SpeechFrame_t *speechFrame = (ISI_SpeechFrame_t *)decoder->pBuffer;
  t_uint16           frameSize   = (speechData->MessageHeader.MessageSize[1] + (speechData->MessageHeader.MessageSize[0] <<8));

  frameSize = frameSize - (sizeof(MAI_SpeechDataDL_t) - sizeof(MAI_DL_Buffers_t));
  
  /* Always reset Buffer */
  memset(speechFrame, 0, sizeof(ISI_SpeechFrame_t));

  /* Get processing time from Message */
  DL_CurrentProcTime = ( (speechData->ProcessingTime[3])       | (speechData->ProcessingTime[2] << 8 ) | 
                         (speechData->ProcessingTime[1] << 16) | (speechData->ProcessingTime[0] << 24) );
    
  /* Check if DL proctime is within limits */ //TODO: check max DL proctime, 2010-06-03, not working for GSM
  //if(DL_CurrentProcTime > DL_MaxProcTime)
  //  LOGD("transmitCodecFrame: DL Proctime is to large: MaxTime: %5.1u: CurrTime: %5.1u\n", DL_MaxProcTime, DL_CurrentProcTime);

  if(frameSize)
  {
	/* AMR */
	if (mCodecInUse == MAI_SPEECH_CODEC_AMR_NB)
	{
	  t_uint16 Rate = speechData->DL_Data.AMR_NB.Rate;
	  t_uint16 FQI  = speechData->DL_Data.AMR_NB.FQI;
	  
	  if (mNetwork == MAI_SYSTEM_3G)
	  {
		Rate = AMR_ConvertRX_Type(speechData->DL_Data.AMR_NB.Rate,
								  speechData->DL_Data.AMR_NB.RX_Type);
	  }                     
	  
	  /* Set Flags & Copy speechData */ 
	  speechFrame->Control |= (FQI << 2);  // FQI
	  speechFrame->Control |= (Rate << 3); // Bitrate
	  speechFrame->Control |= (speechData->DL_Data.AMR_NB.RX_Type << 13); // RX Type
	  speechFrame->Control  = swapEndianess16(speechFrame->Control);
	  
	  /* Copy Speech Data */ 
	  memcpy(speechFrame->SpeechData, speechData->DL_Data.AMR_NB.SpeechData, MAI_AMR_NB_FRAME_SIZE); 
	  frameSize = MAI_AMR_NB_FRAME_SIZE + 4;
	}
	
	/* EFR */
	else if (mCodecInUse == MAI_SPEECH_CODEC_EFR)
	{
	  /* Set Flags */ 
	  speechFrame->Control |= (speechData->DL_Data.EFR.SID << 2);
	  speechFrame->Control |= (speechData->DL_Data.EFR.TAF << 1);
	  speechFrame->Control |= (speechData->DL_Data.EFR.BFI);
	  speechFrame->Control  = swapEndianess16(speechFrame->Control);
	  
	  /* Copy Speech Data */ 
	  memcpy(speechFrame->SpeechData, speechData->DL_Data.EFR.SpeechData, MAI_EFR_FRAME_SIZE);
	  frameSize = MAI_EFR_FRAME_SIZE + 4;
	}
	
	/* FR */
	else if (mCodecInUse == MAI_SPEECH_CODEC_FR)
	{
	  memcpy(decoder->pBuffer, &speechData->DL_Data.FR, sizeof(MAI_FR_DL_Buffer_t));
	  frameSize = sizeof(MAI_FR_DL_Buffer_t);
	}
	
	/* HR */
	else if (mCodecInUse ==MAI_SPEECH_CODEC_HR)
	{
	  memcpy(decoder->pBuffer, &speechData->DL_Data.HR, sizeof(MAI_HR_DL_Buffer_t));
	  frameSize = sizeof(MAI_HR_DL_Buffer_t);
	}
	
	/* PCM8 */
	else if (mCodecInUse ==MAI_SPEECH_CODEC_PCM8)
	{
	  /*
		The Data format coming from the Modem has the following structure.
		Data [0..319] Data
		Data [320] Codec
		Data [321] Rx type
		Data [322] Bit rate or TAF
		Data [323] FQI or BFI
	  */
	  memcpy(decoder->pBuffer, speechData->DL_Data.PCM8.PCM8_Data, MAI_PCM_FRAME_SIZE_8KHZ);
	  frameSize = MAI_PCM_FRAME_SIZE_8KHZ;
	}
	
	/* PCM16 */
	else if (mCodecInUse ==MAI_SPEECH_CODEC_PCM16)
	{
	  memcpy(decoder->pBuffer, speechData->DL_Data.PCM16.PCM16_Data, MAI_PCM_FRAME_SIZE_16KHZ);
	  frameSize = MAI_PCM_FRAME_SIZE_16KHZ;
	}
	
	/* AMR WB */
	else if (mCodecInUse == MAI_SPEECH_CODEC_AMR_WB)
	{
	  t_uint16 Rate = speechData->DL_Data.AMR_WB.Rate;
	  t_uint16 FQI  = speechData->DL_Data.AMR_WB.FQI;
	  
	  if (mNetwork == MAI_SYSTEM_3G)
	  {
		Rate = AMRWB_ConvertRX_Type(speechData->DL_Data.AMR_WB.Rate,
									speechData->DL_Data.AMR_WB.RX_Type);
	  }                     
	  
	  /* Set Flags & Copy speechData */ 
	  speechFrame->Control |= (FQI << 2);  // FQI
	  speechFrame->Control |= (Rate << 3); // Bitrate
	  speechFrame->Control |= (speechData->DL_Data.AMR_WB.RX_Type << 13); // RX Type
	  speechFrame->Control  = swapEndianess16(speechFrame->Control);
	  memcpy(speechFrame->SpeechData, speechData->DL_Data.AMR_WB.SpeechData, (MAI_AMR_WB_FRAME_SIZE) );
	  frameSize = MAI_AMR_WB_FRAME_SIZE + 4;
	}
	
	/* Swap all to fit Decoder */
	//  t_uint32 *frame_ptr = (t_uint32*)decoder->pBuffer;
	//  t_uint16 i;
	//  for(i = 0;i < frameSize; i+=4)
	//  {
	//    t_uint32 tmp = *frame_ptr++;
	//    decoder->pBuffer[i]   = (tmp & 0xFF000000) >> 24;
	//    decoder->pBuffer[i+1] = (tmp & 0xFF0000)   >> 16;
	//    decoder->pBuffer[i+2] = (tmp & 0xFF00)     >> 8;
	//    decoder->pBuffer[i+3] = (tmp & 0xFF);
	//  }
  } // frameSize != 0
  
  decoder->nFilledLen = frameSize;
  decoder->nOffset    = 0;
  
  return true;
}

bool MaiProtocol::set_modem_loop_mode(OMX_BUFFERHEADERTYPE *msg,
									  Cscall_ModemLoop_Config_t *loop_config )
{
  MAI_TestModemLoopBack_t *loopMsg = (MAI_TestModemLoopBack_t *)msg->pBuffer;
  t_uint16 message_size = 0;

  // Prepare the message
  message_size = sizeof(MAI_TestModemLoopBack_t);
  loopMsg->MessageHeader.MessageId   = MAI_MESSAGE_TEST_MODEM_LOOPBACK;
  loopMsg->System      = mNetwork;

  switch (loop_config -> modem_loop_type)
  {
  	  case MODEM_LOOP_OFF			:loopMsg->LoopBackMode = MAI_MODEM_LOOPBACK_MODE_OFF;
  	  	  	  	  	  	  	  	  	 break;
  	  case MODEM_LOOP_RAW			:loopMsg->LoopBackMode = MAI_MODEM_LOOPBACK_MODE_RAW;
  	  	  	  	  	  	  	  	  	 break;
  	  case MODEM_LOOP_SPEECH_CODEC	:loopMsg->LoopBackMode = MAI_MODEM_LOOPBACK_MODE_SPEECH_CODEC;
  	  	  	  	  	  	  	  	  	 break;
  	  default 						:loopMsg->LoopBackMode = MAI_MODEM_LOOPBACK_MODE_OFF;
  	  	  	  	  	  	  	  	     break;
  }

  switch (loop_config -> codec_on_modem_side)
  {
  	  case CODEC_GSM_FR : loopMsg->SpeechCodec = MAI_SPEECH_CODEC_FR;
  			  	  	  	  break;
  	  case CODEC_GSM_HR : loopMsg->SpeechCodec = MAI_SPEECH_CODEC_HR;
	  	  	  	  	  	  break;
  	  case CODEC_EFR    : loopMsg->SpeechCodec = MAI_SPEECH_CODEC_EFR;
	  	  	  	  	  	  break;
  	  case CODEC_AMR_NB : loopMsg->SpeechCodec = MAI_SPEECH_CODEC_AMR_NB;
	  	  	  	  	  	  break;
  	  case CODEC_AMR_WB : loopMsg->SpeechCodec = MAI_SPEECH_CODEC_AMR_WB;
	  	  	  	  	  	  break;
  	  default			: loopMsg->SpeechCodec = MAI_SPEECH_CODEC_AMR_NB;
  	  	  	  	  	  	  break;
  }

  loopMsg->SpeechCodec = loop_config -> codec_on_modem_side;
  loopMsg->DTX = MAI_DTX_OFF;
  loopMsg->AMR_Rate.AMR_NB = loop_config -> amr_rate;

  /* Set Tail for Modem message */
  msg->nFilledLen = message_size;
  loopMsg->MessageHeader.MessageSize[1] = (t_uint8)(message_size & 0xFF);
  loopMsg->MessageHeader.MessageSize[0] = (t_uint8)((message_size >> 8)&0xFF);

  msg->nOffset    = 0;
  msg->nFlags     = 0;

  return true;
}


bool MaiProtocol::process_speech_data_uplink(OMX_BUFFERHEADERTYPE *encoder_buffer, OMX_BUFFERHEADERTYPE *msg,
                                             Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
                                             bool *config_updated)
{

  t_uint16 message_size = 0;
  MAI_SpeechDataUL_t *speechData = (MAI_SpeechDataUL_t *)msg->pBuffer;
  speechData->MessageHeader.MessageId   = MAI_MESSAGE_UPLINK_SPEECH_DATA;
  speechData->System      = mNetwork;
  speechData->SpeechCodec = mCodecInUse;
  
  /* AMR */
  if (mCodecInUse == MAI_SPEECH_CODEC_AMR_NB)
  {
    MAI_AMR_TX_Type_t Tx_Type = (encoder_buffer->pBuffer[2] & 0x3);
    MAI_AMR_NB_Rate_t    Rate = (encoder_buffer->pBuffer[3] >> 3);
    
    /* If WCDMA, Convert TX Type */
    if (mNetwork == MAI_SYSTEM_3G)
    {
      Tx_Type = AMR_ConvertTX_Type(&Rate, encoder_buffer->pBuffer[8]); /* Arg 2 is Rate &  STI byte */
    }
    /* Setup the frame number */
    speechData->FrameNumber = ( (encoder_buffer->pBuffer[2] >> 2) & 0x7);
    /* Set up Speech data in Message */
    speechData->UL_Data.AMR_NB.Rate    = Rate;
    speechData->UL_Data.AMR_NB.TX_Type = Tx_Type;
    memcpy(speechData->UL_Data.AMR_NB.SpeechData, &encoder_buffer->pBuffer[4], MAI_AMR_NB_FRAME_SIZE);
    message_size = MAI_AMR_NB_UL_DATA_MSG_SIZE;
  }
  
  /* EFR */
  else if (mCodecInUse == MAI_SPEECH_CODEC_EFR)
  {
	/* Setup the frame number */
	speechData->FrameNumber = ( (encoder_buffer->pBuffer[2] >> 2) & 0x7);
	/* Set up Speech data in Message */
    speechData->UL_Data.EFR.SP = (encoder_buffer->pBuffer[3] >> 4);
    memcpy(speechData->UL_Data.EFR.SpeechData, &encoder_buffer->pBuffer[4], MAI_EFR_FRAME_SIZE);
    message_size = MAI_EFR_UL_DATA_MSG_SIZE;
  }
  
  /* FR */
  else if (mCodecInUse == MAI_SPEECH_CODEC_FR)
  {
    memcpy(&speechData->UL_Data.FR, encoder_buffer->pBuffer, sizeof(MAI_FR_UL_Buffer_t));
    message_size = MAI_FR_UL_DATA_MSG_SIZE;
  }
  
  /* HR */
  else if (mCodecInUse == MAI_SPEECH_CODEC_HR)
  { 
    memcpy(&speechData->UL_Data.HR, encoder_buffer->pBuffer, sizeof(MAI_HR_UL_Buffer_t));
    message_size = MAI_HR_UL_DATA_MSG_SIZE;
  }

  /* PCM8 */
  else if (mCodecInUse == MAI_SPEECH_CODEC_PCM8)
  {
	/* Set up Speech data in Message */
    memcpy(speechData->UL_Data.PCM8.PCM8_Data, encoder_buffer->pBuffer, MAI_PCM_FRAME_SIZE_8KHZ);
    message_size = MAI_PCM8_UL_DATA_MSG_SIZE;
  }

  /* PCM16 */
  else if (mCodecInUse == MAI_SPEECH_CODEC_PCM16)
  {
    /* Set up Speech data in Message */
    memcpy(speechData->UL_Data.PCM16.PCM16_Data, encoder_buffer->pBuffer, MAI_PCM_FRAME_SIZE_16KHZ);
    message_size = MAI_PCM16_UL_DATA_MSG_SIZE;
  }

  /* AMR-WB */
  else if (mCodecInUse == MAI_SPEECH_CODEC_AMR_WB)
  {
    MAI_AMR_TX_Type_t Tx_Type = (encoder_buffer->pBuffer[2] & 0x3);
    MAI_AMR_WB_Rate_t  Rate   = (encoder_buffer->pBuffer[3] >> 3);

    /* If WCDMA, Convert TX Type */
    if (mNetwork == MAI_SYSTEM_3G)
    {
      Tx_Type = AMR_ConvertTX_Type(&Rate, encoder_buffer->pBuffer[8]); /* Arg 2 is Rate & STI byte */
    }

	/* Setup the frame number */
	speechData->FrameNumber = ( (encoder_buffer->pBuffer[2] >> 2) & 0x7);

    /* Set up Speech data in Message */
    speechData->UL_Data.AMR_WB.Rate    = Rate;
    speechData->UL_Data.AMR_WB.TX_Type = Tx_Type;
    memcpy(speechData->UL_Data.AMR_WB.SpeechData, &encoder_buffer->pBuffer[4], MAI_AMR_WB_FRAME_SIZE);
	message_size = MAI_AMR_WB_UL_DATA_MSG_SIZE;
  }
     
  /* Set Tail for Modem message */
  msg->nFilledLen = message_size;
  speechData->MessageHeader.MessageSize[1] = (t_uint8)(message_size & 0xFF);
  speechData->MessageHeader.MessageSize[0] = (t_uint8)((message_size >> 8)&0xFF);
  msg->nOffset    = 0;
  msg->nFlags     = 0;
        
  return true;
}


bool MaiProtocol::process_voice_call_status(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer, bool *connected)
{
  MAI_VoiceCallStatus_t  *voicestatus = (MAI_VoiceCallStatus_t  *)msg->pBuffer;

  if(voicestatus->CallStatus == MAI_CALL_STATUS_CONNECTED)
  {
    *connected = true;
  }
  else
  {
    *connected = false;
  }
  
  return true;
}


///////////////////////////////////
// helper functions
//////////////////////////////////
CsCallCodec_t MaiProtocol::mai2CscallCodec(MAI_SpeechCodec_t codec)
{
  switch(codec)
  {
    case MAI_SPEECH_CODEC_HR:     return CODEC_GSM_HR;
    case MAI_SPEECH_CODEC_FR:     return CODEC_GSM_FR;
    case MAI_SPEECH_CODEC_EFR:    return CODEC_EFR;
    case MAI_SPEECH_CODEC_AMR_NB: return CODEC_AMR_NB;
    case MAI_SPEECH_CODEC_AMR_WB: return CODEC_AMR_WB;
    case MAI_SPEECH_CODEC_PCM8:   return CODEC_PCM8;
    case MAI_SPEECH_CODEC_PCM16:  return CODEC_PCM16;
    case MAI_SPEECH_CODEC_NONE:   return CODEC_NONE;
    default:                      return CODEC_NONE;
  }
}

CsCallNetwork_t MaiProtocol::mai2CscallNetwork(MAI_System_t network)
{
  switch(network)
  {
    case MAI_SYSTEM_NONE:    return NETWORK_NONE;
    case MAI_SYSTEM_2G:      return NETWORK_GSM;
    case MAI_SYSTEM_3G:      return NETWORK_WCDMA;
    case MAI_SYSTEM_CS_O_HS: return NETWORK_CS_O_HS;
    default:                 return NETWORK_NONE;
  }
}


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
/* Static functions                                    */

/****************************************************************
 * Function    : AMRWB_ConvertRX_Type
 * Description : Converts AMRWB GSM RX Type and rate 2 WCDMA RX Rate
 ****************************************************************/
static t_uint16 AMRWB_ConvertRX_Type(const MAI_AMR_WB_Rate_t    Rate,    /* In  */
                                     const MAI_AMR_WB_RX_Type_t RX_Type) /* In  */
{
  t_uint16 RetVal;

  /*********************************************************/
  /* Add Sid Information in SID Frames, only used by WCDMA */
  if ( (RX_Type == MAI_AMR_TX_TYPE_SID_FIRST) ||
       (RX_Type == MAI_AMR_WB_RX_TYPE_SID_BAD)   ||
       (RX_Type == MAI_AMR_WB_RX_TYPE_SID_UPDATE) )
  {
    RetVal = PROT_AMRWB_RATE_SID; /* 9 */
  }

  else if ( (RX_Type == MAI_AMR_WB_RX_TYPE_SPEECH)                   ||
            (RX_Type == MAI_AMR_WB_RX_TYPE_SPEECH_DEGRADED) ||
            (RX_Type == MAI_AMR_WB_RX_TYPE_SPEECH_LOST)              ||
            (RX_Type == MAI_AMR_WB_RX_TYPE_SPEECH_BAD) )
  {
    RetVal = Rate;
  }

  else  /*  if(Rx_Type == MAI_AMR_WB_RX_TYPE_NO_DATA) */
    RetVal = PROT_AMRWB_RATE_NO_DATA; /* 15 */

  return RetVal;
}

/****************************************************************
 * Function    : AMR_ConvertRX_Type
 * Description : Converts AMR GSM RX Type and rate 2 WCDMA RX Rate
 ****************************************************************/
static t_uint16 AMR_ConvertRX_Type(const MAI_AMR_NB_Rate_t    Rate,    /* In  */
                                   const MAI_AMR_NB_RX_Type_t RX_Type) /* In  */
{
  t_uint16 RetVal;

  /*********************************************************/
  /* Add Sid Information in SID Frames, only used by WCDMA */
  if ( (RX_Type == MAI_AMR_NB_RX_TYPE_SID_FIRST) ||
       (RX_Type == MAI_AMR_NB_RX_TYPE_SID_BAD)   ||
       (RX_Type == MAI_AMR_NB_RX_TYPE_SID_UPDATE) )
  {
    RetVal = PROT_AMR_RATE_SID; /* 8 */
  }

  else if ( (RX_Type == MAI_AMR_NB_RX_TYPE_SPEECH)          ||
            (RX_Type == MAI_AMR_NB_RX_TYPE_SPEECH_DEGRADED) ||
            (RX_Type == MAI_AMR_NB_RX_TYPE_ONSET)           ||
            (RX_Type == MAI_AMR_NB_RX_TYPE_SPEECH_BAD) )
  {
    RetVal = Rate;
  }

  else  /*  if(Rx_Type == MAI_AMR_NB_RX_TYPE_NO_DATA) */
    RetVal = PROT_AMR_RATE_NO_DATA; /* 15 */

  return RetVal;
}

/****************************************************************
 * Function    : AMR_ConvertTX_Type
 * Description : Converts WCDMA RX Rate 2 GSM TX Type and rate
 ****************************************************************/
static MAI_AMR_TX_Type_t AMR_ConvertTX_Type(t_uint8 *const Rate,const t_uint8 RateByte)
{
  MAI_AMR_TX_Type_t RetType = MAI_AMR_TX_TYPE_SPEECH;
  
  t_uint8 ExtractedRate;
  t_uint8 ExtractedSTI;

  /* Get bitRate */
  ExtractedRate = ((RateByte & 0x000f) >> 1);

  /* else we need to grab Bitrate and SidType from last byte */
  if (*Rate == PROT_AMR_RATE_SID) /* 8 */
  {
    /* Get STI bit */
    ExtractedSTI = ((RateByte & 0x0010) >> 4);

    *Rate = ExtractedRate;

    if(ExtractedSTI == 0)    
      RetType = MAI_AMR_TX_TYPE_SID_FIRST;
    else
      RetType = MAI_AMR_TX_TYPE_SID_UPDATE;
  }
  else if (*Rate ==  PROT_AMR_RATE_NO_DATA) /* 15 */
  {
    RetType = MAI_AMR_TX_TYPE_NO_DATA;
    *Rate   = ExtractedRate;
  }

  return RetType;
}

