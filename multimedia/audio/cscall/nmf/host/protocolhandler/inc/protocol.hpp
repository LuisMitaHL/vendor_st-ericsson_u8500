/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   protocol.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __protocolhpp
#define __protocolhpp

#include <cscall/nmf/host/protocolhandler/codecreq.idt>

#define MAX_NB_ANSWER 2

class Protocol : public TraceObject
{
public:
  Protocol(void){};
  virtual ~Protocol() {};

  /**
   * Get the modem message type from on OMX buffer. Depending on the message
   * type, protocolhandler will call specific process function.
   *
   * \param [IN] msg pointer on OMX buffer containing on Modem message
   * \return type of message contained in the buffer
   */
  virtual CsCallMessage_t getMsgType(OMX_BUFFERHEADERTYPE *msg) = 0;
  
  /**
   * Get the modem message length in bytes.
   *
   * \param [IN] msg pointer on OMX buffer containing on Modem message
   * \return length in bytes of the buffer
   */
  virtual t_uint16 getMsgLength(OMX_BUFFERHEADERTYPE *msg) = 0;

  
  /**
   * Get the number of answer to modem needed for this message.
   * Protocolhandler will only call the process function if enough "answer" (i.e. uplink)
   * buffers are available.
   * \attention : it MUST be lowe or equal to MAX_NB_ANSWER
   *
   * \param [IN] msg_type type of modem message
   * \return number of answer
   */
  virtual t_uint16 getNbAnswer(CsCallMessage_t msg_type) = 0;
  
  /**
   * Callback called when MSG_MODEM_INFORMATION_RESPONSE message is received.
   * The protocolhandler will call this function, with a table of OMX buffer as parameter.
   * This table contains, as many buffer as was return by getNbAnswer(MSG_MODEM_INFORMATION_RESPONSE).
   * After calling this function, will send back these buffer to modem, unless the process function
   * overwritte the addres with 0, which means that this buffer (whose address has been overwritte)
   * should not be send to modem.
   * Protocolhandler will not do anything after calling this function.
   *
   * \param [IN]     msg OMX buffer containing MSG_MODEM_INFORMATION_RESPONSE message
   * \param [IN/OUT] answer table of containing the number of buffer return by getNbAnswer
   * \return return false if message has not been processed correctly (may rarely happen), true otherwise
   */
  virtual bool process_modem_information_response(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer) = 0;
  
  /**
    * Callback called when MSG_TEST_MODEM_LOOP_REQUEST message is received.
    * The protocolhandler will call this function, with a table of OMX buffer as parameter.
    * This table contains, as many buffer as was return by getNbAnswer(MSG_TEST_AUDIO_LOOP_REQUEST).
    * After calling this function, will send back these buffer to modem, unless the process function
    * overwritte the addres with 0, which means that this buffer (whose address has been overwritte)
    * should not be send to modem.
    * Protocolhandler will not do anything after calling this function.
    *
    * \param [IN]     msg OMX buffer containing MSG_TEST_MODEM_LOOP_REQUEST message
    * \param [IN/OUT] answer table of containing the number of buffer return by getNbAnswer
    * \return return false if message has not been processed correctly (may rarely happen), true otherwise
    */
   virtual bool process_request_for_loop(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer) = 0;

  /**
   * Call by protocolhandler for each MSG_CODING_FORMAT_REQUEST, before
   * calling process_coding_format_request.
   * If true is returned, protocolhandler will send empty buffer with EOS flag
   * on downlink and stop ul timer.
   *
   * \param [IN] msg OMX buffer containing MSG_CODING_FORMAT_REQUEST message
   * \param [IN] codec_in_use current codec in use
   * \return true if the MSG_CODING_FORMAT_REQUEST indicates the end of call, false otherwise.
   */
  virtual bool is_end_of_call(OMX_BUFFERHEADERTYPE *msg) = 0;
  
  /**
   * Callback called when MSG_CODING_FORMAT_REQUEST message is received.
   * The protocolhandler will call this function, with a table of OMX buffer as parameter.
   * This table contains, as many buffer as was return by getNbAnswer(MSG_CODING_FORMAT_REQUEST).
   * After calling this function, protocolhandler will send back these buffer to modem, unless
   * the process function overwrited the addres with 0, which means that this buffer (whose address
   * has been overwritte) should not be send to modem.
   * After calling this function, protocolhandler will (is return status is true):
   *  - reconfigure codecs, if config_updated is set to true with the configuration passed in parameter
   *  - reconfigure the timer, if time_update is set to true with the time passed in parameter
   *
   *
   * \param [IN]     msg OMX buffer containing MSG_CODING_FORMAT_REQUEST message
   * \param [IN/OUT] answer table of containing the number of buffer return by getNbAnswer
   * \param [IN/OUT] codec_config pointer on current codec configuration.
   *                 To be updated with new codec configuration is necessary
   * \param [IN/OUT] codec_status pointer on current codec status.
   *                 To be updated if codec status evolves
   * \param [OUT]    pointer or structure that will be send back to proxy. To be updated if new codec and/or network
   * \param [IN/OUT] codec_in_use Current codec in used. To be updated if codec change
   * \param [IN/OUT] network_in_use Current netwrok in used. To be updated if network change
   * \param [OUT]    config_updated If set to true, protocolhandler will reconfigure codec with
   *                 the updated config in \param codec_config
   * \param [OUT]    time pointer on new timing to apply (if any)
   * \param [OUT]    time_update If set to true, protocolhandler will reconfigure the uplink timer
   *                 with the value set in \param time
   * \return return false if message has not been processed correctly (may rarely happen), true otherwise
   */
  virtual bool process_coding_format_request(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
                                             Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
                                             CsCallCodecReq_t *codec_req,
                                             CsCallCodec_t *codec_in_use, CsCallNetwork_t *network_in_use,
                                             bool *config_updated, t_uint32 *time, bool *time_updated) = 0;
  

   /**
   * Callback called when MSG_DOWNLINK_TIMING_CONFIGURATION message is received.
   * The protocolhandler will call this function, with a table of OMX buffer as parameter.
   * This table contains, as many buffer as was return by getNbAnswer(MSG_DOWNLINK_TIMING_CONFIGURATION).
   * After calling this function, protocolhandler will send back these buffer to modem, unless
   * the process function overwrited the addres with 0, which means that this buffer (whose address
   * has been overwritte) should not be send to modem.
   * After calling this function, protocolhandler will :
   *  - reconfigure the timer, if time_update is set to true with the time passed in parameter
   *
   *
   * \param [IN]     msg OMX buffer containing MSG_DOWNLINK_TIMING_CONFIGURATION message
   * \param [IN/OUT] answer table of containing the number of buffer return by getNbAnswer
   * \param [OUT]    time pointer on new timing to apply (if any)
   * \param [OUT]    time_update If set to true, protocolhandler will reconfigure the uplink timer
   *                 with the value set in \param time
   * \return return false if message has not been processed correctly (may rarely happen), true otherwise
   */
  virtual bool process_downlink_timing_configuration(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
                                                     t_uint32 *time, bool *time_updated) = 0;

  /**
   * Callback called when MSG_TIMING_REPORT message is received.
   * The protocolhandler will call this function, with a table of OMX buffer as parameter.
   * This table contains, as many buffer as was return by getNbAnswer(MSG_TIMING_REPORT).
   * After calling this function, protocolhandler will send back these buffer to modem, unless
   * the process function overwrited the addres with 0, which means that this buffer (whose address
   * has been overwritte) should not be send to modem.
   * After calling this function, protocolhandler will :
   *  - reconfigure the timer, if time_update is set to true with the time passed in parameter
   *
   *
   * \param [IN]     msg OMX buffer containing MSG_TIMING_REPORT message
   * \param [IN/OUT] answer table of containing the number of buffer return by getNbAnswer
   * \param [IN]     next_uplink_deliver_date is the theorical time when the next uplink frame is
   *                 supposed to be send.
   *                 Usually next_uplink_deliver_date = uplink_deliveryDate + 20 ms.
   * \param [OUT]    time pointer on new timing to apply (if any) for uplink
   * \param [OUT]    time_update If set to true, protocolhandler will reconfigure the uplink timer
   *                 with the value set in \param time
   * \return return false if message has not been processed correctly (may rarely happen), true otherwise
   */
  virtual bool process_timing_report(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer,
                                          unsigned long long next_uplink_deliver_date,
                                          t_uint32 *time, bool *time_updated) = 0;
  
  /**
   * Callback called when MSG_SPEECH_DATA_DOWNLINK message is received.
   * The protocolhandler will call this function, with one OMX buffer as parameter, to be filled
   * for the decoder.
   *
   * \param [IN]  msg OMX buffer containing MSG_SPEECH_DATA_DOWNLINK message.
   * \param [OUT] decoder OMX buffer to be filled for the decoder.
   * \return return false if decoder buffer has not been updated.
   */
  virtual bool process_speech_data_downlink(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE *decoder) = 0;
  

  /**
   * Callback called when when MSG_VOICE_CALL_STATUS message is received.
   * After calling this function, protocolhandler will mute downlink and uplink is connected status is false.
   *
   * \param [IN]     msg OMX buffer containing MSG_TIMING_REPORT message
   * \param [IN/OUT] answer table of containing the number of buffer return by getNbAnswer
   * \param [OUT]    connected to be set to true is voice call status is connected
   * \return return false if message has not been processed correctly (may rarely happen), true otherwise
   */
  virtual bool process_voice_call_status(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer, bool *connected) = 0;


  /**
   * Called when protocolhandler receives one buffer from the encoder.
   * After calling this function, protocolhandler will :
   *  - send answer buffer to modem if function returned true
   *  - reconfigure encoder, if config_updated is set to true with the configuration passed in parameter
   * 
   *
   * \param [IN]     encoder_buffer OMX buffer containing on encoded frame by the speech encoder.
   * \param [IN/OUT] msg OMX buffer to be filled with the correct MSG_SPEECH_DATA_UPLINK message
   * \param [IN/OUT] codec_config pointer on current codec configuration.
   *                 To be updated with new codec configuration is necessary.
   * \param [IN/OUT] codec_status pointer on current codec status.
   *                 To be updated if codec status evolves.
   * \param [OUT]    config_updated If set to true, protocolhandler will reconfigure codec with
   *                 the updated config in \param codec_config.
   * \return return false if modem buffer has not been updated.
   */
  virtual bool process_speech_data_uplink(OMX_BUFFERHEADERTYPE *encoder_buffer, OMX_BUFFERHEADERTYPE *msg,
                                          Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
                                          bool *config_updated) = 0;
  
  /**
   * Get the modem message length in bytes.
   *
   * \param [IN] msg pointer on OMX buffer containing on Modem message
   * \return length in bytes of the buffer
   */
  virtual bool set_modem_loop_mode(OMX_BUFFERHEADERTYPE *msg,
                                   Cscall_ModemLoop_Config_t *loop_config ) = 0;
  
  /**
   * allocate correct protocol
   */
  static Protocol * createProtocol(CsCallProtocol_t protocol);
  
  /**
   * allocate correct protocol, depending on msg
   */
  static Protocol * autoDetectProtocol(OMX_BUFFERHEADERTYPE *msg);
};

#endif
