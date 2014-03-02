/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   loopbackmodem.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __loopbackmodemhpp
#define __loopbackmodemhpp

#include "Component.h"
#include <cscall/nmf/host/protocolhandler/inc/IsiMessage.hpp>
#include "cscall_config.h"
#include "LoopBack.hpp"

class cscall_nmf_host_loopbackmodem : public Component, public cscall_nmf_host_loopbackmodemTemplate
{

public:
  virtual t_nmf_error construct();
  virtual void        destroy();

  //Component virtual functions
  virtual void process() ;
  virtual void reset() ;
  virtual void disablePortIndication(t_uint32 portIdx) ;
  virtual void enablePortIndication(t_uint32 portIdx) ;
  virtual void flushPortIndication(t_uint32 portIdx) ;

  // fsminit interface (fsminit)
  virtual void fsmInit(fsmInit_t initFsm);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled) {mPorts[portIdx].setTunnelStatus(isTunneled);}

  // sendcommand interface (sendcommand)
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) ;
  
  // postevent interface (postevent)
  virtual void processEvent(void)  			                   { Component::processEvent() ; }
  
  // armnmf_emptythisbuffer interface (emptythisbuffer)
  virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)  { Component::deliverBuffer(UPLINK_PORT, buffer); }

  // armnmf_fillthisbuffer interface (fillthisbuffer)
  virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)   { if (!toIdle) Component::deliverBuffer(DOWNLINK_PORT, buffer); }
  
  // timer.api.alarm (alarmMReP)
  virtual void signal(void);

  // cscall.nmf.host.loopbackmodem.configure (configure)
  virtual void SetProtocol(CsCallProtocol_t protocol);
  virtual void SpeechCodecReq(CsCallCodecReq_t config);
  virtual void SpeechTimingRequest(t_uint32 timing, t_uint32 cause);
  virtual void VoiceCallStatus(t_uint32 connected);
  virtual void MRePInit(t_uint32 timing, t_uint16 *pBuffer, t_uint32 outBufferSize, t_uint16 *pBufferOutput);

private:
  // Port and buffer
  typedef enum {UPLINK_PORT, DOWNLINK_PORT} portname;

  OMX_BUFFERHEADERTYPE mBufIn[NB_UL_MODEM_BUFFER], mBufOut[NB_DL_MODEM_BUFFER] ;
  Port                 mPorts[2]  ;

  // loopback mode
  typedef enum {WAIT_CODEC_REQ, WAIT_CODEC_RESP, WAIT_UL_FRAME1, WAIT_UL_FRAME2, ON_GOING, WAIT_DL_BUFFER, WAIT_UL_FRAME_NOW} loopback_status;

  loopback_status  mLoopBackStatus;
  t_uint32         mGSM445pattern;
  CsCallCodecReq_t mCodecConfig;

  void signalProcess(bool from_process, t_uint32 * timing);
  
  // Modem replay mode 
  t_uint16 		 nMRePActivation;
  t_uint32 		 nMRePTiming;
  t_uint16      *pMRePBuffer;
  t_uint32 		 nMRePOutputSize;
  t_uint16      *pMRePBufferOutput;
  t_uint16      *pMRePBufferOutputInitial;

  void			setNextDLProcessDate(t_uint32 timing, t_uint32 period);
  t_uint32		MRePProcess(void);
  
  // protocol specific
  LoopBack     *mLoopBack;
  void createLoopBack(CsCallProtocol_t protocol);

  // componant status
  int  toIdle ;
};

#endif /* __cscall_nmf_host_loopbackmodemhpp */
