/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   cscallcontroller.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __cscallcontrollerhpp
#define __cscallcontrollerhpp

#include "audiocontroller.hpp"
#include <cscall/nmf/host/protocolhandler/codecreq.idt>

typedef enum {
  PROTOCOL_INDEX      = 0,
  DOWNLINK_INDEX      = 1,
  UPLINK_INDEX        = 2,
  LOOPBACKMODEM_INDEX = 3,
  NB_NMF_COMPONENT
} ComponentIndex;

class cscall_nmf_host_cscallcontroller : public hst_misc_audiocontroller, public cscall_nmf_host_cscallcontrollerTemplate
{
public:
  cscall_nmf_host_cscallcontroller()
  {}
  virtual void fsmInit(fsmInit_t init);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { hst_misc_audiocontroller::sendCommand(cmd, param) ; }

  virtual void forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) ;

  virtual void protocol_eventHandler     (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
  virtual void downlink_eventHandler     (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
  virtual void uplink_eventHandler       (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
  virtual void loopbackmodem_eventHandler(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
  
  virtual void newCodecReq(CsCallCodecReq_t config);

private:
  void sendNewCodecReq();
};

#endif
