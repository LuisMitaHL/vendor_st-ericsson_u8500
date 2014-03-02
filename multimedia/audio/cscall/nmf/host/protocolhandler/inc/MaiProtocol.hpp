/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MaiProtocol.hpp
 * \brief  implementation of Protocol class for STE protocol.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __MaiProtocolhpp
#define __MaiProtocolhpp

#include "cscall/nmf/host/protocolhandler/inc/protocol.hpp"
#include "cscall/nmf/host/protocolhandler/inc/t_mai.h"

class MaiProtocol : public Protocol
{
public:
  MaiProtocol(void);
  virtual ~MaiProtocol();
  
  virtual CsCallMessage_t getMsgType(OMX_BUFFERHEADERTYPE *msg);
  
  virtual t_uint16 getMsgLength(OMX_BUFFERHEADERTYPE *msg);

  virtual t_uint16 getNbAnswer(CsCallMessage_t msg_type);
  
  virtual bool process_modem_information_response(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer);

  virtual bool process_request_for_loop(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer);

  virtual bool is_end_of_call(OMX_BUFFERHEADERTYPE *msg);

  virtual bool process_coding_format_request(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
											 Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
											 CsCallCodecReq_t *codec_req,
											 CsCallCodec_t *codec_in_use, CsCallNetwork_t *network_in_use,
											 bool *config_updated, t_uint32 *time, bool *time_updated);
  
  virtual bool process_downlink_timing_configuration(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
													 t_uint32 *time, bool *time_updated);

  virtual bool process_timing_report(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
                                          unsigned long long next_uplink_deliver_date,
                                          t_uint32 *time, bool *time_updated);

  virtual bool process_speech_data_downlink(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE *decoder);

  virtual bool process_voice_call_status(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer, bool *connected);
  
  virtual bool process_speech_data_uplink(OMX_BUFFERHEADERTYPE *encoder_buffer, OMX_BUFFERHEADERTYPE *msg,
										  Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
										  bool *config_updated);

  virtual bool set_modem_loop_mode(OMX_BUFFERHEADERTYPE *msg,
  									  Cscall_ModemLoop_Config_t *loop_config );
private:
  MAI_SpeechCodec_t mCodecInUse;
  MAI_System_t      mNetwork;
  CsCallCodec_t   mai2CscallCodec(MAI_SpeechCodec_t codec);
  CsCallNetwork_t mai2CscallNetwork(MAI_System_t network);

  t_uint32 DL_MaxProcTime;    
  t_uint32 DL_CurrentProcTime;

  const t_uint32 UL_TimingWM_Low;
  const t_uint32 UL_TimingWM_High;
  const t_uint32 UL_TIMING_WM_ADJUST;
  
#ifdef USE_SAMC
  bool                    SAMC_Opened;
#endif
};

#endif // __MaiProtocolhpp
