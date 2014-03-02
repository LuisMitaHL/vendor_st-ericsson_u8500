/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   IsiMessage.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __isimessagehpp
#define __isimessagehpp

#include "cscall/nmf/host/protocolhandler/inc/modem_audio_isi.h"

typedef struct
{
  t_uint8   media;
  t_uint8   sender_dev;
  t_uint8   sender_obj;
  t_uint8   transsaction_id;
}IsiTopHeader;


typedef struct
{
  t_uint8   media;
  t_uint8   receiver_dev;
  t_uint8   sender_dev;
  t_uint8   res;
  t_uint16  length;
  t_uint8   receiver_obj;
  t_uint8   sender_obj;
  t_uint8   TransactionId;
  t_uint8   MessageId;
}PHONET_HEADER_T;


typedef struct
{
  PHONET_HEADER_T   phonet_header;
  t_uint16      spare;
  t_uint16      SpeechCodecUse;
  t_uint16      SpeechCodec;
  t_uint16              SpeechEncoderConfiguration;
  t_uint16      SpeechDecoderConfiguration;
  t_uint16      SpeechCodecInit;
}SPEECH_CODEC_REQ_T;


typedef struct
{
    PHONET_HEADER_T phonet_header;
    t_uint16        spare;
    t_uint16        Reason;
} SPEECH_CODEC_RESP_T;


// #define for TimingReq Message
typedef struct
{
    PHONET_HEADER_T phonet_header;
    t_uint16        timing_high;
    t_uint16        timing_low;
    t_uint16        processing_time;
    t_uint16        subsystem;
    t_uint16        fill1;
} SPC_TIMING_REQ_T;


// #define for TimingResp message
typedef struct
{
    PHONET_HEADER_T phonet_header;
    t_uint16        status;
} SPC_TIMING_RESP_T;



// #define for SPC_DECODER_DATA_REQ/SPC_ENCODER_DATA_NTF messages
typedef struct
{
    PHONET_HEADER_T phonet_header;
    t_uint16        data_format;
    t_uint16        fill1;
    t_uint16        num_sb;
    t_uint16        sb_id;
    t_uint16        sb_len;
} SPC_DECODER_DATA_REQ_T;


typedef struct
{
    PHONET_HEADER_T phonet_header;
    t_uint16        data_format;
    t_uint16        fill1;
    t_uint16        num_sb;
    t_uint16        sb_id;
    t_uint16        sb_len;
} SPC_ENCODER_DATA_NTF_T;


typedef struct
{
  t_uint16  reserved;
  t_uint16  Filler1:8;
  t_uint16  BFI:1;
  t_uint16  TAF:1;
  t_uint16  SID:2;
  t_uint16  SP:1;
  t_uint16  Reserved1:1;
  t_uint16  Filler2:2;
  t_uint16  data[17];
}MODEM_AUDIO_GSM_FR_FRAME_T;

typedef struct
{
  t_uint16  reserved;
  t_uint16  Filler1:8;
  t_uint16  BFI:1;
  t_uint16  UFI:1;
  t_uint16  TAF:1;
  t_uint16  SID:2;
  t_uint16  SP:1;
  t_uint16  Reserved1:1;
  t_uint16  Filler2:1;
  t_uint16  data[9];
}MODEM_AUDIO_GSM_HR_FRAME_T;

typedef struct
{
    t_uint16  reserved;
    t_uint16  Filler1:8;
    t_uint16  BFI:1;
    t_uint16  TAF:1;
    t_uint16  SID:2;
    t_uint16  SP:1;
    t_uint16  Filler2:3;
    t_uint16  data[17];
}MODEM_AUDIO_GSM_EFR_FRAME_T;

typedef struct
{
  t_uint16  reserved;
  t_uint16  TxType:2;
  t_uint16  SFN:3;
  t_uint16  RxType:3;
  t_uint16  padding3:1;
  t_uint16  padding2:1;
  t_uint16  quality:1;
  t_uint16  FrameType:4;
  t_uint16  padding1:1;
  t_uint16  data[16];
}MODEM_AUDIO_AMR_FRAME_T;


typedef struct
{
  t_uint16  reserved;
  t_uint16  TxType:2;
  t_uint16  SFN:3;
  t_uint16  RxType:3;
  t_uint16  padding3:1;
  t_uint16  padding2:1;
  t_uint16  quality:1;
  t_uint16  FrameType:4;
  t_uint16  padding1:1;
  t_uint16  data[36];
} MODEM_AUDIO_AMR_WB_FRAME_T;


typedef enum {
  ISI_CODEC_NONE = SPC_NO,
  ISI_CODEC_GSM_FR = SPC_UNI_GSM_FR,
  ISI_CODEC_GSM_HR = SPC_UNI_GSM_HR,
  ISI_CODEC_EFR    = SPC_UNI_GSM_EFR,
  ISI_CODEC_AMR_NB = SPC_UNI_AMR,
  ISI_CODEC_AMR_WB = SPC_UNI_WB_AMR,
  ISI_CODEC_PCM8   = 0x27,
  ISI_CODEC_PCM16  = 0x28
} ISI_CODEC_NAME_T;


typedef enum {
  ISI_NETWORK_NONE  = SPC_USE_NO,
  ISI_NETWORK_GSM   = SPC_USE_GSM_MODEM,
  ISI_NETWORK_WCDMA = SPC_USE_WCDMA_MODEM 
} ISI_NETWORK_NAME_T;

#define PN_DSP_AUDIO            0xF3
#define SENDER_DEVICE_AUDIO_DSP 0x00
#define GETCODEC_REQField(f,v) (((v) & MODEM_AUDIO_SPEECH_CODEC_REQ_##f##_MASK) >> MODEM_AUDIO_SPEECH_CODEC_REQ_##f##_OFFSET)
#define SETCODEC_REQField(f,v,i) (((v) & (~MODEM_AUDIO_SPEECH_CODEC_REQ_##f##_MASK))|((i)<< MODEM_AUDIO_SPEECH_CODEC_REQ_##f##_OFFSET))



class IsiMessage{
public:
  IsiMessage(t_uint8 *bufPtr):phonetHeader((PHONET_HEADER_T *)bufPtr),
                  codecReq((SPEECH_CODEC_REQ_T *)bufPtr),
                  codecResp((SPEECH_CODEC_RESP_T *)bufPtr),
                  timingReq((SPC_TIMING_REQ_T*)bufPtr),
                  timingResp((SPC_TIMING_RESP_T *)bufPtr),
                  decoderDataReq((SPC_DECODER_DATA_REQ_T *)bufPtr),
                  encoderDataNtf((SPC_ENCODER_DATA_NTF_T *)bufPtr)
  {}
  IsiMessage(t_uint8 *bufPtr, t_uint8 type, IsiTopHeader &isi_info, t_uint16 param, OMX_BUFFERHEADERTYPE *omx_header);
  ~IsiMessage(void){}

  t_uint16 swapEndianess16(t_uint16 data) {return (((data & 0xFF)<<8) | ((data & 0xFF00) >> 8));}

  // common to all messages
  t_uint8  getType(void)                           {return phonetHeader->MessageId;}
  void     setType(t_uint8 type)                   {phonetHeader->MessageId = type;}
#ifdef USE_LITTLE_ENDIAN_MODEM
  // use little endian modem
  t_uint16 getLength(void)                         {return phonetHeader->length  + 6;}
  void     setLength(t_uint16 length)              {phonetHeader->length = (length - 6);}
#else
  // use big endian modem
  t_uint16 getLength(void)                         {return (swapEndianess16(phonetHeader->length)  + 6);}
  void     setLength(t_uint16 length)              {phonetHeader->length = swapEndianess16((length - 6));}
#endif
  t_uint8  getMedia(void)                          {return phonetHeader->media;}
  void     setMedia(t_uint8 media)                 {phonetHeader->media = media;}
  t_uint8  getSenderDev(void)                      {return phonetHeader->sender_dev;}
  void     setSenderDev(t_uint8 sender_dev)        {phonetHeader->sender_dev = sender_dev;}
  void     setReceiverDev(t_uint8 receiver_dev)    {phonetHeader->receiver_dev = receiver_dev;}
  t_uint8  getSenderObj(void)                      {return phonetHeader->sender_obj;}
  void     setSenderObj(t_uint8 sender_obj)        {phonetHeader->sender_obj = sender_obj;}
  void     setReceiverObj(t_uint8 receiver_obj)    {phonetHeader->receiver_obj = receiver_obj;}
  t_uint8  getTransactionId(void)                  {return phonetHeader->TransactionId;}
  void     setTransactionId(t_uint8 TransactionId) {phonetHeader->TransactionId = TransactionId;}
  void     setRes(t_uint8 res)                     {phonetHeader->res = res;}

  // specific to CODEC_REQ
  ISI_NETWORK_NAME_T getNetwork(void) {return (ISI_NETWORK_NAME_T)swapEndianess16(codecReq->SpeechCodecUse);}
  ISI_CODEC_NAME_T   getCodec(void)   {return (ISI_CODEC_NAME_T)swapEndianess16(codecReq->SpeechCodec);}
  t_uint8  getAACFlag(void)        {return GETCODEC_REQField(audio_activity_control, swapEndianess16(codecReq->SpeechEncoderConfiguration));}
  t_uint8  getNsync(void)          {return GETCODEC_REQField(nsync,                  swapEndianess16(codecReq->SpeechEncoderConfiguration));}
  t_uint8  getSFN(void)            {return GETCODEC_REQField(SpeechFrameNumber,      swapEndianess16(codecReq->SpeechEncoderConfiguration));}
  t_uint8  getAmrEncoder(void)     {return GETCODEC_REQField(AMR_encoder,            swapEndianess16(codecReq->SpeechEncoderConfiguration));}
  t_uint8  getDTX(void)            {return GETCODEC_REQField(DTX_use,                swapEndianess16(codecReq->SpeechEncoderConfiguration));}
  t_uint8  getAmrDecoder(void)     {return GETCODEC_REQField(AMR_decoder,            swapEndianess16(codecReq->SpeechDecoderConfiguration));}
  t_uint16 getCodecInit(void)      {return codecReq->SpeechCodecInit;}
//setters
  void  setNetwork(ISI_NETWORK_NAME_T aSpeechCodecUse) {codecReq->SpeechCodecUse = swapEndianess16(aSpeechCodecUse);}
  void  setCodec(ISI_CODEC_NAME_T aSpeechCodec)   {codecReq->SpeechCodec  = swapEndianess16(aSpeechCodec);}
  void  setAACFlag(t_uint8 aAACflag){codecReq->SpeechEncoderConfiguration = swapEndianess16(SETCODEC_REQField(audio_activity_control, swapEndianess16(codecReq->SpeechEncoderConfiguration),aAACflag));}
  void  setNsync(t_uint8 aNsynch)   {codecReq->SpeechEncoderConfiguration = swapEndianess16(SETCODEC_REQField(nsync, swapEndianess16(codecReq->SpeechEncoderConfiguration),aNsynch));}
  void  setSFN(t_uint8 aSFN)        {codecReq->SpeechEncoderConfiguration = swapEndianess16(SETCODEC_REQField(SpeechFrameNumber, swapEndianess16(codecReq->SpeechEncoderConfiguration),aSFN));}
  void  setAmrEncoder(t_uint8 aAMREncoder) {codecReq->SpeechEncoderConfiguration = swapEndianess16(SETCODEC_REQField(AMR_encoder, swapEndianess16(codecReq->SpeechEncoderConfiguration),aAMREncoder));}
  void  setDTX(t_uint8 aDTX)               {codecReq->SpeechEncoderConfiguration = swapEndianess16(SETCODEC_REQField(DTX_use, swapEndianess16(codecReq->SpeechEncoderConfiguration),aDTX));}
  void  setAmrDecoder(t_uint8 aAMRDecoder)     {codecReq->SpeechDecoderConfiguration = swapEndianess16(SETCODEC_REQField(AMR_decoder, swapEndianess16(codecReq->SpeechDecoderConfiguration),aAMRDecoder));}
  void  setCodecInit(t_uint16 aCodecInit)      {codecReq->SpeechCodecInit = aCodecInit;}


  // specific to CODEC_RESP
  void setReason(t_uint16 reason) {codecResp->Reason = swapEndianess16(reason);}

  // specific to TIMING_REQ
  t_uint32 getTimingHigh(void)     {return swapEndianess16(timingReq->timing_high);}
  t_uint32 getTimingLow(void)      {return swapEndianess16(timingReq->timing_low);}
  t_uint16 getProcessingtime(void) {return swapEndianess16(timingReq->processing_time);}
  t_uint16 getSubSystem(void)      {return swapEndianess16(timingReq->subsystem);}
  //setters
  void setTimingHigh(t_uint16 aTimingHigh)     {timingReq->timing_high = swapEndianess16(aTimingHigh);}
  void setTimingLow(t_uint16 aTimingLow)      {timingReq->timing_low = swapEndianess16(aTimingLow);}
  void setProcessingtime(t_uint16 aProcessingTime) {timingReq->processing_time = swapEndianess16(aProcessingTime);}
  void setSubSystem(t_uint16 aSubsystem)      {timingReq->subsystem = swapEndianess16(aSubsystem);}


  // specific to TIMING_RESP
  void setStatus(t_uint16 status) {timingResp->status = swapEndianess16(status);};

  // specific to DECODER_DATA_REQ
  t_uint16 getDataFormat(void) {return swapEndianess16(decoderDataReq->data_format);}
  t_uint16 getNumSB(void)      {return swapEndianess16(decoderDataReq->num_sb);}
  t_uint16 getSBId(void)       {return swapEndianess16(decoderDataReq->sb_id);}
  t_uint16 getSBLength(void)   {return swapEndianess16(decoderDataReq->sb_len);}
  t_uint16 getFrameLength(void){return swapEndianess16(decoderDataReq->sb_len) - 4;}
  t_uint8 *getFramePtr(void)   {return (t_uint8 *)((int)phonetHeader + sizeof(SPC_DECODER_DATA_REQ_T));}

  void setDecDataFormat(t_uint16 data_format) {decoderDataReq->data_format = swapEndianess16(data_format);}
  void setDecNumSb(t_uint16 num_sb)           {decoderDataReq->num_sb = swapEndianess16(num_sb);}
  void setDecSBId(t_uint16 id)                {decoderDataReq->sb_id = swapEndianess16(id);}
  void setDecFrameLength(t_uint16 len)        {decoderDataReq->sb_len = swapEndianess16(len + 4);}


  // specific to ENCODER_DATA_NTF
  void setDataFormat(t_uint16 data_format) {encoderDataNtf->data_format = swapEndianess16(data_format);}
  void setNumSb(t_uint16 num_sb)           {encoderDataNtf->num_sb = swapEndianess16(num_sb);}
  void setSBId(t_uint16 id)                {encoderDataNtf->sb_id = swapEndianess16(id);}
  void setSBLength(t_uint16 len)           {encoderDataNtf->sb_len = swapEndianess16(len);}
  void setFrameLength(t_uint16 len)        {encoderDataNtf->sb_len = swapEndianess16(len + 4);}
  t_uint16 getDataNtfHeaderSize(void)      {return sizeof(SPC_ENCODER_DATA_NTF_T);}


  t_uint16 getEncFrameLength(void){return swapEndianess16(encoderDataNtf->sb_len) - 4;}
  t_uint8 *getEncFramePtr(void)   {return (t_uint8 *)((int)phonetHeader + sizeof(SPC_ENCODER_DATA_NTF_T));}
  t_uint16 getEncDataFormat(void) {return swapEndianess16(encoderDataNtf->data_format);}
  t_uint16 getEncNumSB(void)      {return swapEndianess16(encoderDataNtf->num_sb);}
  t_uint16 getEncSBId(void)       {return swapEndianess16(encoderDataNtf->sb_id);}



private:
  PHONET_HEADER_T         *phonetHeader;
  SPEECH_CODEC_REQ_T      *codecReq;
  SPEECH_CODEC_RESP_T     *codecResp;
  SPC_TIMING_REQ_T        *timingReq;
  SPC_TIMING_RESP_T       *timingResp;
  SPC_DECODER_DATA_REQ_T  *decoderDataReq;
  SPC_ENCODER_DATA_NTF_T  *encoderDataNtf;

};


#endif //__isimessagehpp
