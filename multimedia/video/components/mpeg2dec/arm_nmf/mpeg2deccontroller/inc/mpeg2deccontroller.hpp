/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __mpeg2deccontrollerhpp
#define __mpeg2deccontrollerhpp

#include "Controller.h"

typedef enum {
  PARSER_INDEX = 0,
  MPEG2DECMPC_INDEX  = 1
} ComponentIndex;

class mpeg2dec_arm_nmf_mpeg2deccontroller : public Controller, public mpeg2dec_arm_nmf_mpeg2deccontrollerTemplate
{
public:
	virtual void fsmInit(fsmInit_t init);
	virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled) {return;}
	virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
	virtual void controllers_eventHandler(OMX_EVENTTYPE ev,t_uint32 data1,
										  t_uint32 data2,t_uint8 collectionIndex);
  
	virtual void forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) ;
	virtual void forwardPortEvent(OMX_COMMANDTYPE cmd, t_uword id) ;

	virtual void parser_eventHandler (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
	virtual void mpeg2decMPC_eventHandler (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
};

#endif
