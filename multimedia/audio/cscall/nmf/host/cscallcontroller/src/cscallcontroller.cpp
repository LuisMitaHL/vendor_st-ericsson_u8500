/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   cscallcontroller.cpp
 * \brief  The controller handles state transition propagation to all of the sub-components within the composite component. It also handles call-back messages from sub-components. 
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/cscallcontroller.nmf>
#include <OMX_Core.h>
#include <armnmf_dbc.h>
#include "cscall_config.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_cscallcontroller_src_cscallcontrollerTraces.h"
#endif

#define ALL_PORTS_TUNNELED      (0xFFFF)

#define CTRL_PORT               (2)
#define UPLINK_PORT             (1)
#define DOWNLINK_PORT           (0)
#define UPLINK_INPUTPORT        (0)
#define DOWNLINK_OUTPUTPORT     (1)



void METH(fsmInit)(fsmInit_t initFsm) {
  // trace init
#ifdef FORCE_TRACE
  initFsm.traceInfoAddr->traceEnable = ~0;
#endif
  initFsm.id1 = CSCONTROLLER_ID;
  setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/controller:: fsmInit() called (id=%d)",initFsm.id1);

  init(NB_NMF_COMPONENT, &proxy) ;

  fsmInit_t init;
  init.portsDisabled = 0;
  init.portsTunneled = ALL_PORTS_TUNNELED;
  init.traceInfoAddr = initFsm.traceInfoAddr;
  init.id1 = PH_ID; 
  setSlave(PROTOCOL_INDEX, &protocolcomponent, true) ;
  protocolfsminit.fsmInit(init) ;

  setSlave(LOOPBACKMODEM_INDEX, &loopbackmodemcomponent, false);
  init.id1 = LBMODEM_ID;
  if (!loopbackmodemcomponent.IsNullInterface()) loopbackmodemfsminit.fsmInit(init);

  /* Cscall Input Port (1) = src_uplink input port (0)
   *  portsDisabled/portsTunneled(CSCALL) = xy
   *  portsDisabled(ULRATE) = 0x
   *  portsTunneled(ULRATE) = 1x
   */
  setSlave(UPLINK_INDEX, &uplinkcomponent, false);
  init.id1 = UPLINK_ID;
  init.portsDisabled = 0;
  init.portsDisabled |= (initFsm.portsDisabled & (0x2)) >> 1;
  init.portsTunneled = ALL_PORTS_TUNNELED;
  init.portsTunneled ^= ( (~initFsm.portsTunneled) & (0x2)) >> 1;
  if (!uplinkcomponent.IsNullInterface()) uplinkfsminit.fsmInit(init) ;

  /* Cscall Output Port (0) = downlink output port (1)
   *  portsDisabled/portsTunneled(CSCALL) = xy
   *  portsDisabled(DLRATE) = y0
   *  portsTunneled(DLRATE) = y1
   */
  setSlave(DOWNLINK_INDEX, &downlinkcomponent, false);
  init.id1 = DOWNLINK_ID;
  init.portsDisabled = 0;
  init.portsDisabled |= (initFsm.portsDisabled & (0x1)) << 1;
  init.portsTunneled = ALL_PORTS_TUNNELED;
  init.portsTunneled ^= ( (~initFsm.portsTunneled) & (0x1)) << 1;
  if (!downlinkcomponent.IsNullInterface()) downlinkfsminit.fsmInit(init) ;
    
} /* fsmInit */


void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/controller:: setTunnelStatus() called (portIdx=%d) (isTunneled=%d)",portIdx,isTunneled);
  if(portIdx == UPLINK_PORT)
  {
    if (!uplinkcomponent.IsNullInterface())  uplinkfsminit.setTunnelStatus(UPLINK_INPUTPORT, isTunneled);
    return;
  }
  else if (portIdx == DOWNLINK_PORT)
  {
    if (!downlinkcomponent.IsNullInterface()) downlinkfsminit.setTunnelStatus(DOWNLINK_OUTPUTPORT, isTunneled);
    return;
  }

  NMF_PANIC("CscallController : setTunnelStatus() wrong Port Id\n");
}

void cscall_nmf_host_cscallcontroller::forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) {

  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/controller:: forwardPortCommand() called (cmd=%d) (id=%d)",cmd,id);

  switch(cmd) {
    case OMX_CommandPortDisable:        
    case OMX_CommandPortEnable: 
      if (id == CTRL_PORT)
      {
        proxy.eventHandler(OMX_EventCmdComplete, cmd, CTRL_PORT) ;
      }
      else if (id == UPLINK_PORT)
      {
        if (!uplinkcomponent.IsNullInterface())
        {
          uplinkcomponent.sendCommand(cmd, UPLINK_INPUTPORT);
          waitCommandFromSlave(UPLINK_INDEX, cmd, 1) ;
        }
      }
      else if (id == DOWNLINK_PORT)
      {
        if (!downlinkcomponent.IsNullInterface())
        {
          downlinkcomponent.sendCommand(cmd, DOWNLINK_OUTPUTPORT);
          waitCommandFromSlave(DOWNLINK_INDEX, cmd, 1);
        }
      }
      else if (id == OMX_ALL)
      {
        if (!uplinkcomponent.IsNullInterface()){
          uplinkcomponent.sendCommand(cmd, UPLINK_INPUTPORT);
          waitCommandFromSlave(UPLINK_INDEX, cmd, 1) ;
        }
        if (!downlinkcomponent.IsNullInterface())
        {
          downlinkcomponent.sendCommand(cmd, DOWNLINK_OUTPUTPORT);
          waitCommandFromSlave(DOWNLINK_INDEX, cmd, 1) ;
        }
      }
      else
        NMF_PANIC("CscallController : forwardPortCommand() wrong Port Id\n");

      break;

    case OMX_CommandFlush:
      OstTraceFiltInst0 (TRACE_WARNING, "Cscall/controller:: WARNING!! forwardPortCommand() called OMX_CommandFlush not supported yet !!!");

      //TODO handle flush command
      break;

    default:
      break;
  }

} /* forwardPortCommand */

////////////////////////////////////////////////////////////
// Provided Interfaces 
////////////////////////////////////////////////////////////

void METH(protocol_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/controller: protocol_eventHandler event=%d", event);
  OstTraceFiltInst2 (TRACE_ALWAYS, "                   Datas are data1=%d, data2=%d",data1,data2);
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
    hst_misc_audiocontroller::slavesEventHandler(event, data1, data2, PROTOCOL_INDEX) ;
  } else {

    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
      /* @TODO */
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
}


void METH(loopbackmodem_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/controller: loopbackmodem_eventHandler event=%d",event);
  OstTraceFiltInst2 (TRACE_ALWAYS, "                   Datas are data1=%d, data2=%d",data1,data2);
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
    hst_misc_audiocontroller::slavesEventHandler(event, data1, data2, LOOPBACKMODEM_INDEX) ;
  } else {
    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
      /* @TODO */
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
}


void METH(uplink_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/controller: uplink_eventHandler event=%d",event);
  OstTraceFiltInst2 (TRACE_ALWAYS, "                   Datas are data1=%d, data2=%d",data1,data2);
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
    hst_misc_audiocontroller::slavesEventHandler(event, data1, data2, UPLINK_INDEX) ;
  } else {
    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
      proxy.eventHandler(event, UPLINK_PORT, data2);
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
}

void METH(downlink_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/controller: downlink_eventHandler event=%d",event);
  OstTraceFiltInst2 (TRACE_ALWAYS, "                   Datas are data1=%d, data2=%d",data1,data2);
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
    hst_misc_audiocontroller::slavesEventHandler(event, data1, data2, DOWNLINK_INDEX) ;
  } else {
    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
      proxy.eventHandler(event, DOWNLINK_PORT, data2);
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
}


void METH(newCodecReq)(CsCallCodecReq_t config)
{
  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/controller: new Codec req (network=%d) (codec=%d)", config.network, config.speech_codec);
  proxy_modem_info.newCodecReq(config);
}

