/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   IsiLoopBack.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __isiloopback_hpp
#define __isiloopback_hpp

#include "cscall/nmf/host/loopbackmodem/inc/LoopBack.hpp"
#include "cscall/nmf/host/protocolhandler/inc/IsiMessage.hpp"
#include "OMX_Audio.h"

class IsiLoopBack : public LoopBack
{
public:
  virtual CsCallMessage_t getMsgType(OMX_BUFFERHEADERTYPE *msg);

  virtual void generate_coding_format_request(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer);
  
  virtual void generate_timing_report(t_uint32 timing, t_uint16 cause, OMX_BUFFERHEADERTYPE *buffer);
  
  virtual void generate_voice_call_status(bool connected, OMX_BUFFERHEADERTYPE *buffer);

  virtual void generate_fake_downlink_frame(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer);

  virtual void copy_speech_uplink_to_downlink(OMX_BUFFERHEADERTYPE *uplink, OMX_BUFFERHEADERTYPE *downlink);

private:
  ISI_NETWORK_NAME_T mNetwork;
  ISI_CODEC_NAME_T   cscall2IsiCodec(CsCallCodec_t codec);
  ISI_NETWORK_NAME_T cscall2IsiNetwork(CsCallNetwork_t network);
  t_uint8 get_amr_rate(OMX_AUDIO_AMRBANDMODETYPE aAmrBandMode);
};
  
#endif // __isiloopback_hpp
