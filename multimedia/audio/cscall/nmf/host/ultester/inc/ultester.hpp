/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ultester.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __ultesterhpp
#define __ultesterhpp

#include "Component.h"

class cscall_nmf_host_ultester : public Component, public cscall_nmf_host_ultesterTemplate
{
public:

  //Component virtual functions
  virtual void process() ;
  virtual void reset() ;
  virtual void disablePortIndication(t_uint32 portIdx) ;
  virtual void enablePortIndication(t_uint32 portIdx) ;
  virtual void flushPortIndication(t_uint32 portIdx) ;

  virtual void fsmInit(fsmInit_t initFsm);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled) {mPorts[portIdx].setTunnelStatus(isTunneled);}

  virtual void	signal();
  virtual void	sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
  virtual void	processEvent(void)  			  { Component::processEvent() ; }
  virtual void	emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) { Component::deliverBuffer(0, buffer); } 
  virtual void	fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)  { Component::deliverBuffer(1, buffer); }
  virtual void  encoder_configureCodec(CsCallCodec_t codecType, void* pCodecInfo) {;}
  virtual void  decoder_configureCodec(CsCallCodec_t codecType, void* pCodecInfo) {;}
  virtual void	newCodecReq(CsCallCodecReq_t config);

private:
  OMX_BUFFERHEADERTYPE	mBufIn, mBufOut;
  Port			mPorts[2];
  int			mNewTiming;
  int			SFN;
  CsCallCodec_t		speech_codec;
};

#endif
