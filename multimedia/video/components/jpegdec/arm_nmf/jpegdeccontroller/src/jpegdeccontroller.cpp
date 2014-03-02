/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <jpegdec/arm_nmf/jpegdeccontroller.nmf>
#include "OMX_Core.h"
#include <stdio.h>
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegdec_arm_nmf_jpegdeccontroller_src_jpegdeccontrollerTraces.h"
#endif
#define INPUT_PORT    (0)
#define OUTPUT_PORT   (1)

//void METH(fsmInit)(t_uint16 portsDisabled, t_uint16 portsTunneled)
void METH(fsmInit)(fsmInit_t fsm_init)
{
	fsmInit_t initSalveFsm = {0,0,0,0};
    OstTraceInt0(TRACE_API, "jpegdeccontroller::FSMINIT\n");
	if (fsm_init.traceInfoAddr)
	{
         setTraceInfo(fsm_init.traceInfoAddr, fsm_init.id1);
	}

  init(2, &proxy, &controllers_output[0], &controllers_output[1], false) ;
  ARMNMF_DBC_ASSERT(!parserSendcommand.IsNullInterface() || !jpegdecMPCSendcommand.IsNullInterface());

  initSalveFsm.traceInfoAddr = fsm_init.traceInfoAddr;
  initSalveFsm.portsDisabled = fsm_init.portsDisabled;
  initSalveFsm.portsTunneled = fsm_init.portsTunneled;
  initSalveFsm.id1 = fsm_init.id1 + 1;
  setSlave(PARSER_INDEX, &parserSendcommand) ;
  if(!parserSendcommand.IsNullInterface()) parserFSMinit.fsmInit(initSalveFsm);//.portsDisabled, initFSM.portsTunneled) ;

  //setDSPSlave(JPEGDECMPC_INDEX, &jpegdecMPCSendcommand) ;
  if (fsm_init.traceInfoAddr)
	initSalveFsm.traceInfoAddr = (TraceInfo_t *)fsm_init.traceInfoAddr->dspAddr;
  initSalveFsm.portsDisabled = fsm_init.portsDisabled;
  initSalveFsm.portsTunneled = fsm_init.portsTunneled;
  initSalveFsm.id1++;
  setSlave(JPEGDECMPC_INDEX, &jpegdecMPCSendcommand) ;
  if(!jpegdecMPCSendcommand.IsNullInterface()) jpegdecMPCFSMinit.fsmInit(initSalveFsm);//.portsDisabled, initFSM.portsTunneled) ;
}

void jpegdec_arm_nmf_jpegdeccontroller::sendCommand(OMX_COMMANDTYPE cmd, t_uword param){
	OstTraceFiltInst0(TRACE_FLOW, "jpegdeccontroller::sendCommand\n");
  //printf("\nController::sendCommand Enter cmd=%d,param=%d",cmd,param);
  Controller::sendCommand(cmd, param) ;
  //printf("\nController::sendCommand Exit");
}

void jpegdec_arm_nmf_jpegdeccontroller::controllers_eventHandler(OMX_EVENTTYPE ev,t_uint32 data1,
                                                                 t_uint32 data2,t_uint8 collectionIndex){
   //printf("\ncontrollers_eventHandler Exit");
	OstTraceFiltInst0(TRACE_FLOW, "jpegdeccontroller::controllers_eventHandler\n");
   Controller::controllersEventHandler(ev, data1, data2, collectionIndex);
   //printf("\ncontrollers_eventHandler Exit");
}


void jpegdec_arm_nmf_jpegdeccontroller::forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) {
	OstTraceFiltInst2(TRACE_API, "jpegdeccontroller::forwardPortCommand - cmd -%d , id -%d\n",cmd,id);
  //printf("\nforwardPortCommand Enter cmd=%d,id=%d",cmd,id);
	switch(cmd) {
  	case OMX_CommandPortDisable:
  	case OMX_CommandPortEnable:
          if(id == INPUT_PORT){
             parserSendcommand.sendCommand(cmd, id);
             waitCommandFromSlave(PARSER_INDEX, cmd, 1) ;

             jpegdecMPCSendcommand.sendCommand(cmd, id);
             waitCommandFromSlave(JPEGDECMPC_INDEX, cmd, 1) ;
          }
          else if (id == OUTPUT_PORT){
            parserSendcommand.sendCommand(cmd, id);
            waitCommandFromSlave(PARSER_INDEX, cmd, 1) ;

            jpegdecMPCSendcommand.sendCommand(cmd, id);
            waitCommandFromSlave(JPEGDECMPC_INDEX, cmd, 1) ;
          }
          else{
		  OstTraceFiltInst0(TRACE_API, "jpegdeccontroller:: command port dis en forwardPortCommand (cmd port flush)CscallController : wrong Port Id");
              NMF_PANIC("CscallController : wrong Port Id\n");
				}
  		break ;

  	case OMX_CommandFlush:
          if(id == INPUT_PORT){
              parserSendcommand.sendCommand(cmd, id);
              waitCommandFromSlave(PARSER_INDEX, cmd, 1) ;

              jpegdecMPCSendcommand.sendCommand(cmd, id);
              waitCommandFromSlave(JPEGDECMPC_INDEX, cmd, 1) ;
          }
          else if (id == OUTPUT_PORT){
              parserSendcommand.sendCommand(cmd, id);
              waitCommandFromSlave(PARSER_INDEX, cmd, 1) ;

              jpegdecMPCSendcommand.sendCommand(cmd, id);
              waitCommandFromSlave(JPEGDECMPC_INDEX, cmd, 1) ;
          }
          else{
              OstTraceFiltInst0(TRACE_API, "jpegdeccontroller:: command port dis en forwardPortCommand (cmd port flush)CscallController : wrong Port Id");
              NMF_PANIC("CscallController : wrong Port Id\n");
          }

          break ;

    default:
      //To remove warnings
        break;



	}
  ////printf("\nforwardPortCommand Exit.");
}

void jpegdec_arm_nmf_jpegdeccontroller::forwardPortEvent(OMX_COMMANDTYPE cmd, t_uword id) {
    OstTraceFiltInst2(TRACE_API, "jpegdeccontroller::forwardPortEvent - cmd -%d , id -%d\n",cmd,id);
  // Nothing to do yet
  proxy.eventHandler(OMX_EventCmdComplete, cmd, id) ;
}

////////////////////////////////////////////////////////////
// Provided Interfaces
////////////////////////////////////////////////////////////

void METH(parser_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
	OstTraceFiltInst3(TRACE_API, "jpegdeccontroller::parser_eventHandler - event -%d , data1 -%d ,data2 -%d\n",event,data1,data2);
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
    Controller::slavesEventHandler(event, data1, data2, PARSER_INDEX) ;
  } else {

    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
      /* @TODO */
      proxy.eventHandler(event, data1, data2);
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
        OstTraceFiltInst0(TRACE_API, "jpegdeccontroller::parser_eventHandler event unknown");
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
  ////printf("\nparser_eventHandler Exit.");
}


void METH(jpegdecMPC_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
    OstTraceFiltInst3(TRACE_API, "jpegdeccontroller::jpegdecMPC_eventHandler - event -%d , data1 -%d ,data2 -%d\n",event,data1,data2);
  if((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))){
    Controller::slavesEventHandler(event, data1, data2, JPEGDECMPC_INDEX) ;
  } else {

    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
      /* @TODO */
      proxy.eventHandler(event, data1, data2);
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
        OstTraceFiltInst0(TRACE_API, "jpegdeccontroller::jpegdecMPC_eventHandler event unknown");
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
  ////printf("\njpegdecMPC_eventHandler Exit.");
  ////printf("\njpegdecMPC_eventHandler Push.");
}

