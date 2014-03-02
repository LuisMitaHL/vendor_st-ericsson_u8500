/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <mpeg2dec/arm_nmf/mpeg2deccontroller.nmf>
#include "OMX_Core.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg2dec_arm_nmf_mpeg2deccontroller_src_mpeg2deccontrollerTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

#define INPUT_PORT    (0)
#define OUTPUT_PORT   (1)
#define BOTH_PORTS    (0xFFFFFFFF)

void METH(fsmInit)(fsmInit_t fsm_init){
  OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::fsmInit");
	if (fsm_init.traceInfoAddr){
         setTraceInfo(fsm_init.traceInfoAddr, fsm_init.id1);
  }

  init(2, &proxy, &controllers_output[0], &controllers_output[1], false) ;
  ARMNMF_DBC_ASSERT(!parserSendcommand.IsNullInterface() || !mpeg2decMPCSendcommand.IsNullInterface());

  setSlave(PARSER_INDEX, &parserSendcommand) ;
  if(!parserSendcommand.IsNullInterface()) parserFSMinit.fsmInit(fsm_init) ;

  setSlave(MPEG2DECMPC_INDEX, &mpeg2decMPCSendcommand) ;
  if(!mpeg2decMPCSendcommand.IsNullInterface()) mpeg2decMPCFSMinit.fsmInit(fsm_init) ;
  OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_mpeg2deccontroller::fsmInit");
}

void mpeg2dec_arm_nmf_mpeg2deccontroller::sendCommand(OMX_COMMANDTYPE cmd, t_uword param){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::sendCommand");
	OstTraceFiltInst2(TRACE_FLOW,"cmd:%d,param:%d",cmd,param);
  Controller::sendCommand(cmd, param) ;
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_mpeg2deccontroller::sendCommand");
}

void mpeg2dec_arm_nmf_mpeg2deccontroller::controllers_eventHandler(OMX_EVENTTYPE ev,t_uint32 data1,
                                                                 t_uint32 data2,t_uint8 collectionIndex){
   OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::controllers_eventHandler");
   OstTraceFiltInst3(TRACE_FLOW,"event:%d,data1:%d,data2:%d",ev,data1,data2);
   Controller::controllersEventHandler(ev, data1, data2, collectionIndex);
   OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_mpeg2deccontroller::controllers_eventHandler");
}


void mpeg2dec_arm_nmf_mpeg2deccontroller::forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) {
  OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::forwardPortCommand");
  OstTraceFiltInst2(TRACE_FLOW,"cmd:%d,id:%d",cmd,id);
	switch(cmd) {
  	case OMX_CommandPortDisable:
	case OMX_CommandPortEnable:
    case OMX_CommandFlush:
			  parserSendcommand.sendCommand(cmd, id);
              waitCommandFromSlave(PARSER_INDEX, cmd, 1) ;

              mpeg2decMPCSendcommand.sendCommand(cmd, id);
              waitCommandFromSlave(MPEG2DECMPC_INDEX, cmd, 1);
			 /* if(((cmd == OMX_CommandPortDisable )||(cmd == OMX_CommandPortEnable )||(cmd == OMX_CommandFlush ))&&(id==0))
				  {
				 mpeg2decMPCSendcommand.sendCommand(cmd, id+2);
				 waitCommandFromSlave(MPEG2DECMPC_INDEX, cmd, 1);
			  }*/
             break ;

    default:
      //To remove warnings
     break;



	}
  OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_mpeg2deccontroller::forwardPortCommand");
}

void mpeg2dec_arm_nmf_mpeg2deccontroller::forwardPortEvent(OMX_COMMANDTYPE cmd, t_uword id) {
  OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::forwardPortEvent");
  OstTraceFiltInst2(TRACE_FLOW,"cmd:%d,id:%d",cmd,id);
  //if(!(((cmd == OMX_CommandPortDisable )||(cmd == OMX_CommandPortEnable )||(cmd == OMX_CommandFlush ))&&(id==2)))
  proxy.eventHandler(OMX_EventCmdComplete, cmd, id) ;
  OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::forwardPortEvent");
}

////////////////////////////////////////////////////////////
// Provided Interfaces
////////////////////////////////////////////////////////////

void METH(parser_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
  OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::parser_eventHandler");
  OstTraceFiltInst3(TRACE_FLOW,"event:%d,data1:%d,data2:%d",event,data1,data2);

  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState)))
  {
    Controller::slavesEventHandler(event, data1, data2, PARSER_INDEX) ;
  } else {

    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
      /* @TODO */
      proxy.eventHandler(event, data1, data2);
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
  OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::parser_eventHandler");
}


void METH(mpeg2decMPC_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::mpeg2decMPC_eventHandler");
	OstTraceFiltInst3(TRACE_FLOW,"event:%d,data1:%d,data2:%d",event,data1,data2);

	if(((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete)) && data2 ==2 && (data1 != (t_uint32)OMX_CommandStateSet))
	{
		//printf("\nMPEG4dec_controller::ignore event for port 2 ");
	}
	else{
	  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState)))
	  {
	    Controller::slavesEventHandler(event, data1, data2, MPEG2DECMPC_INDEX) ;
	  } else {

	    /* Manage other events localy */
	    if (event == OMX_EventBufferFlag) {
	      /* @TODO */
				//printf("\nMPEG2dec_controller::mpeg2decMPC_eventHandler Enter event=%d,data1=%d,data2=%d",event,data1,data2);
				proxy.eventHandler(event, data1, data2);
	    } else if (event == OMX_EventError) {
	      proxy.eventHandler(event, data1, data2);
	    } else {
	      ARMNMF_DBC_ASSERT(0);
	    }
	  } /* if ((ev == OMX_EventCmdComplete) ... */
}
  OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_mpeg2deccontroller::mpeg2decMPC_eventHandler");
}

