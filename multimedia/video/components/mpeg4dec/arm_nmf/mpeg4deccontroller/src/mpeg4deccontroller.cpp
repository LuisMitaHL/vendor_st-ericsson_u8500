/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <mpeg4dec/arm_nmf/mpeg4deccontroller.nmf>
#include "OMX_Core.h"
//#include <stdio.h>
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4dec_arm_nmf_mpeg4deccontroller_src_mpeg4deccontrollerTraces.h"
#endif
void METH(fsmInit)(fsmInit_t fsm_init)
{
	fsmInit_t initSalveFsm = {0,0,0,0};
  OstTraceInt0(TRACE_API, "At start of mpeg4deccontroller::fsmInit\n");
  //printf("\nMPEG4dec_controller:: fsmInit enter portsDisabled=%d portsTunneled=%d ",fsm_init.portsDisabled,fsm_init.portsTunneled);
  if (fsm_init.traceInfoAddr)
  {
         setTraceInfo(fsm_init.traceInfoAddr, fsm_init.id1);
  }
  init(2, &proxy, &controllers_output[0], &controllers_output[1], false) ;
  ARMNMF_DBC_ASSERT(!parserSendcommand.IsNullInterface() || !mpeg4decMPCSendcommand.IsNullInterface());

  initSalveFsm.traceInfoAddr = fsm_init.traceInfoAddr;
  initSalveFsm.portsDisabled = fsm_init.portsDisabled;
  initSalveFsm.portsTunneled = fsm_init.portsTunneled;
  initSalveFsm.id1 = fsm_init.id1 + 1;
  setSlave(PARSER_INDEX, &parserSendcommand) ;
  if(!parserSendcommand.IsNullInterface()) parserFSMinit.fsmInit(initSalveFsm) ;

  if (fsm_init.traceInfoAddr)
  {
#ifdef SOFT_DECODER
	initSalveFsm.traceInfoAddr = fsm_init.traceInfoAddr;
//+ER406303
#elif __MPEG4DEC_ARM_NMF
	initSalveFsm.traceInfoAddr = fsm_init.traceInfoAddr;
#else 
	initSalveFsm.traceInfoAddr = (TraceInfo_t *)fsm_init.traceInfoAddr->dspAddr;
#endif
//-ER406303  
  }
  initSalveFsm.portsDisabled = fsm_init.portsDisabled;
  initSalveFsm.portsTunneled = fsm_init.portsTunneled;
  initSalveFsm.id1++;
  setSlave(MPEG4DECMPC_INDEX, &mpeg4decMPCSendcommand) ;
  if(!mpeg4decMPCSendcommand.IsNullInterface()) mpeg4decMPCFSMinit.fsmInit(initSalveFsm) ;
   //printf("\nMPEG4dec_controller::fsmInit Exit");
}

void mpeg4dec_arm_nmf_mpeg4deccontroller::sendCommand(OMX_COMMANDTYPE cmd, t_uword param)
{
  //printf("\nMPEG4dec_controller::sendCommand Enter cmd=%d,param=%d",cmd,param);
  Controller::sendCommand(cmd, param) ; 
  //printf("\nMPEG4dec_controller::sendCommand Exit");
}

void mpeg4dec_arm_nmf_mpeg4deccontroller::controllers_eventHandler(OMX_EVENTTYPE ev,t_uint32 data1,t_uint32 data2,t_uint8 collectionIndex)
{
   //printf("\nMPEG4dec_controller::controllers_eventHandler data1=%d,data2=%d collectionIndex=%d",data1,data2,collectionIndex);
   Controller::controllersEventHandler(ev, data1, data2, collectionIndex);
   //printf("\nMPEG4dec_controller::controllers_eventHandler Exit");
}


void mpeg4dec_arm_nmf_mpeg4deccontroller::forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) 
{
    //printf("\nMPEG4dec_controller::forwardPortCommand Enter cmd=%d,id=%d",cmd,id);
	switch(cmd) {
  	case OMX_CommandPortDisable:		
	case OMX_CommandPortEnable:	
    case OMX_CommandFlush:
			  parserSendcommand.sendCommand(cmd, id);
              waitCommandFromSlave(PARSER_INDEX, cmd, 1) ;
      
              mpeg4decMPCSendcommand.sendCommand(cmd, id);
              waitCommandFromSlave(MPEG4DECMPC_INDEX, cmd, 1);
              break ;        
    default:
			 break;



	}
  //printf("\nMPEG4dec_controller::forwardPortCommand Exit.");
} 

void mpeg4dec_arm_nmf_mpeg4deccontroller::forwardPortEvent(OMX_COMMANDTYPE cmd, t_uword id) 
{
  //printf("\nMPEG4dec_controller::forwardPortEvent_to_proxy Enter cmd=%d,id=%d",cmd,id);
  proxy.eventHandler(OMX_EventCmdComplete, cmd, id) ;
  //printf("\nMPEG4dec_controller::forwardPortEvent_to_proxy  Exit.");
}

void METH(parser_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
   //printf("\nMPEG4dec_controller::parser_eventHandler Enter event=%d,data1=%d,data2=%d",event,data1,data2);
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState)))
   {
	   Controller::slavesEventHandler(event, data1, data2, PARSER_INDEX) ;
   } 
   else 
   {
	   /* Manage other events localy */
	   if (event == OMX_EventBufferFlag) 
	   {
		   /* @TODO */
		   proxy.eventHandler(event, data1, data2);
	   } 
	   else if (event == OMX_EventError) 
	   {
		   proxy.eventHandler(event, data1, data2);
	   } 
	   else 
	   {
		   ARMNMF_DBC_ASSERT(0);
	   }
   } 
  //printf("\nMPEG4dec_controller::parser_eventHandler Exit.");
}


void METH(mpeg4decMPC_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
	//printf("\nMPEG4dec_controller::mpeg4decMPC_eventHandler Enter event=%d,data1=%d,data2=%d",event,data1,data2);

	if(((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete)) && data2 ==2 && (data1 != (t_uint32)OMX_CommandStateSet))
	{
		//printf("\nMPEG4dec_controller::ignore event for port 2 ");
	}
	else
	{
	    if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState)))
		{
			Controller::slavesEventHandler(event, data1, data2, MPEG4DECMPC_INDEX) ;
		} 
		else 
		{
			/* Manage other events localy */
			if (event == OMX_EventBufferFlag) 
			{
				//printf("\nMPEG4dec_controller::OMX_EventBufferFlag %X",data2);
				proxy.eventHandler(event, data1, data2);
			} 
			else if (event == OMX_EventError) 
			{
				proxy.eventHandler(event, data1, data2);
			} 
			else 
			{
				ARMNMF_DBC_ASSERT(0);
			}
		} 
	}
    //printf("\nMPEG4dec_controller::mpeg4decMPC_eventHandler Exit.");
}

