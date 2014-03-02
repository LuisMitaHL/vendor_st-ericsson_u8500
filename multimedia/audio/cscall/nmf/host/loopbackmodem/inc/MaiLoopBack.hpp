/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MaiLoopBack.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __mailoopback_hpp
#define __mailoopback_hpp

#include "cscall/nmf/host/loopbackmodem/inc/LoopBack.hpp"
#include "cscall/nmf/host/protocolhandler/inc/t_mai.h"
#include "OMX_Audio.h"

class MaiLoopBack : public LoopBack
{
public:
  virtual CsCallMessage_t getMsgType(OMX_BUFFERHEADERTYPE *msg);

  virtual void generate_coding_format_request(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer);
  
  virtual void generate_timing_report(t_uint32 timing, t_uint16 cause, OMX_BUFFERHEADERTYPE *buffer);
  
  virtual void generate_voice_call_status(bool connected, OMX_BUFFERHEADERTYPE *buffer);

  virtual void generate_fake_downlink_frame(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer);

  virtual void copy_speech_uplink_to_downlink(OMX_BUFFERHEADERTYPE *uplink, OMX_BUFFERHEADERTYPE *downlink);

private:
  MAI_SpeechCodec_t cscall2MaiCodec(CsCallCodec_t codec);
  MAI_System_t      cscall2MaiNetwork(CsCallNetwork_t network);
  MAI_AMR_BitRate_t get_amr_rate(OMX_AUDIO_AMRBANDMODETYPE aAmrBandMode);
};
  
#endif // __mailoopback_hpp
