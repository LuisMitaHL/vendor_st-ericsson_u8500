/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   loopback.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __loopback_hpp
#define __loopback_hpp

#include <cscall/nmf/host/protocolhandler/codecreq.idt>

class LoopBack
{
public:
  virtual ~LoopBack(){};
  
  virtual CsCallMessage_t getMsgType(OMX_BUFFERHEADERTYPE *msg) = 0;
  
  virtual void generate_coding_format_request(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer) = 0;

  virtual void generate_timing_report(t_uint32 timing, t_uint16 cause, OMX_BUFFERHEADERTYPE *buffer) = 0;
  
  virtual void generate_voice_call_status(bool connected, OMX_BUFFERHEADERTYPE *buffer) = 0;
  
  virtual void generate_fake_downlink_frame(CsCallCodecReq_t *config, OMX_BUFFERHEADERTYPE *buffer) = 0;

  virtual void copy_speech_uplink_to_downlink(OMX_BUFFERHEADERTYPE *uplink, OMX_BUFFERHEADERTYPE *downlink) = 0;

  static LoopBack * getLoopBack(CsCallProtocol_t protocolID);
};

#endif // __loopback_hpp
