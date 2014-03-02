/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   IsiProtocol.hpp
 * \brief  implementation of Protocol class for ISI protocol.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __IsiProtocolhpp
#define __IsiProtocolhpp

#include "cscall/nmf/host/protocolhandler/inc/protocol.hpp"
#include "cscall/nmf/host/protocolhandler/inc/IsiMessage.hpp"

class IsiProtocol : public Protocol
{
public:
  IsiProtocol(void);
  virtual ~IsiProtocol() {};
  
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

protected:
  typedef enum {CODEC_IDX, TIMING_IDX, IDX_NB} indexname; 
  IsiTopHeader mIsiTopHeader[IDX_NB];
  void storeIsiTopHeader(indexname idx, IsiMessage &msg);
  
  ISI_CODEC_NAME_T   mCodecInUse;
  ISI_NETWORK_NAME_T mNetwork;
  
  CsCallCodec_t   isi2CscallCodec(ISI_CODEC_NAME_T codec);
  CsCallNetwork_t isi2CscallNetwork(ISI_NETWORK_NAME_T network);

  // extract codec and netwrok to use from ISI message.
  // Virtual as the codec/network to be use inisde cscall may be different from the one contained in the message
  virtual void getCodecAndNetwork(IsiMessage &isi_msg, ISI_CODEC_NAME_T &codec, ISI_NETWORK_NAME_T &network);

  // extract the uplink timing from isi message and return the answer to use in TIMING_RESP message.
  // a timing of zero means no UL reconfiguration.
  // Virtual as we may not always want to reconfigure UL timing.
  virtual t_uint32 getTimingValue(IsiMessage &isi_msg, t_uint16 &response);
};

#endif // __IsiProtocolhpp
