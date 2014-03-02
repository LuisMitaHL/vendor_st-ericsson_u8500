/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     speechcontroller.cpp
 * \brief    controller for Speech Proc Composite
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include <speech_proc/nmf/host/speechcontroller.nmf>
#include <OMX_Core.h>

/*
  For Hybrid composite
           +-------------------------------------------------------------+
           |                       OMX                                   |
           |      +------------+           +----------------+            |
cscall --> |1 --> |1  DL wrp  0|  ------>  |1   Dsp Port   0|  ------>  4| --> speaker output
output     |      +------------+           +----------------+            |
           |                                                             |
           |      +------------+     +--------------+                    |                  
cscall <-- |3 <-- |0  UL wrp  1| <-- |2    time    1| <---------------- 2| <-- reference
input      |      +------------+     |  alignment   |                    |
           |                         |             0| <---------------- 0| <-- micro input
           |                         +--------------+                    |
           |                                                             |
           +-------------------------------------------------------------+

  For Host composite
           +-------------------------------------------------------------+
           |                       OMX                                   |
           |                      +------------+                         |
cscall --> |1 ------------------> |1  DL wrp  0| -------------------->  4| --> speaker output
output     |                      +------------+                         |
           |                                                             |
           |      +------------+     +--------------+                    |                  
cscall <-- |3 <-- |0  UL wrp  1| <-- |2    time    1| <---------------- 2| <-- reference
input      |      +------------+     |  alignment   |                    |
           |                         |             0| <---------------- 0| <-- micro input
           |                         +--------------+                    |
           |                                                             |
           +-------------------------------------------------------------+
*/


void METH(fsmInit)(fsmInit_t initFsm)
{
  fsmInit_t initSalveFsm = {0,0,0,0};

  // trace init (mandatory before port init)
  if (initFsm.traceInfoAddr){
    setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
  }
  
  init(NB_SLAVE, &proxy) ;
  
  setSlave(DOWNLINK_INDEX, &downlinkcomponent, false) ;
  initSalveFsm.traceInfoAddr = initFsm.traceInfoAddr;
  initSalveFsm.portsDisabled = omx2localPortMask(initFsm.portsDisabled, DOWNLINK_INDEX);
  initSalveFsm.portsTunneled = ALL_PORTS_TUNNELED;
#ifndef USE_HYBRID_COMPONENT
  initSalveFsm.portsDisabled |= omx2localPortMask(initFsm.portsDisabled, DSP_PORT_INDEX);
#endif
  initSalveFsm.id1 = initFsm.id1 + 1;
  downlinkfsminit.fsmInit(initSalveFsm);

  setSlave(UPLINK_INDEX, &uplinkcomponent, false) ;
  initSalveFsm.traceInfoAddr = initFsm.traceInfoAddr;
  initSalveFsm.portsDisabled = omx2localPortMask(initFsm.portsDisabled, UPLINK_INDEX);
  initSalveFsm.portsTunneled = ALL_PORTS_TUNNELED;
  initSalveFsm.id1++;
  uplinkfsminit.fsmInit(initSalveFsm);

  setSlave(TIME_ALIGN_INDEX, &timealignmentcomponent, true) ;
#ifdef USE_HYBRID_COMPONENT
  if (initFsm.traceInfoAddr) initSalveFsm.traceInfoAddr = (TraceInfo_t *)initFsm.traceInfoAddr->dspAddr;
#else
  initSalveFsm.traceInfoAddr = initFsm.traceInfoAddr;
#endif
  initSalveFsm.portsDisabled = omx2localPortMask(initFsm.portsDisabled, TIME_ALIGN_INDEX);
  initSalveFsm.portsTunneled = ALL_PORTS_TUNNELED;
  initSalveFsm.id1++;
  timealignmentfsminit.fsmInit(initSalveFsm);

#ifdef USE_HYBRID_COMPONENT
  setSlave(DSP_PORT_INDEX, &dspportcomponent, true);
  if (initFsm.traceInfoAddr) initSalveFsm.traceInfoAddr = (TraceInfo_t *)initFsm.traceInfoAddr->dspAddr;
  initSalveFsm.portsDisabled = omx2localPortMask(initFsm.portsDisabled, DSP_PORT_INDEX);
  initSalveFsm.portsTunneled = ALL_PORTS_TUNNELED;
  initSalveFsm.id1++;
  dspportfsminit.fsmInit(initSalveFsm);
#endif
} /* fsmInit */


void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
  // subcomponant are always tunneled.
  // We should ever enter here .
  NMF_PANIC("speechproc_controller :: setTunnelStatus");
}


void speech_proc_nmf_host_speechcontroller::forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword portIdx) {
  switch(cmd) {
    case OMX_CommandPortDisable:        
    case OMX_CommandPortEnable:
      {
        switch (portIdx)
        {
          case DOWNLINK_OMX_PORT_INDEX :
            downlinkcomponent.sendCommand(cmd, DOWNLINK_LOCAL_IN_PORT_INDEX);
            waitCommandFromSlave(DOWNLINK_INDEX, cmd, 1);
            return;
			
          case UPLINK_OMX_PORT_INDEX:
            uplinkcomponent.sendCommand(cmd, UPLINK_LOCAL_OUT_PORT_INDEX);
            waitCommandFromSlave(UPLINK_INDEX, cmd, 1);
            return;
			
          case TIME_ALIGN_REF_OMX_PORT_INDEX :
			timealignmentcomponent.sendCommand(cmd, TIME_ALIGN_LOCAL_REF_PORT_INDEX);
			waitCommandFromSlave(TIME_ALIGN_INDEX, cmd, 1);
            return;
			
          case TIME_ALIGN_OMX_PORT_INDEX :
            timealignmentcomponent.sendCommand(cmd, TIME_ALIGN_LOCAL_IN_PORT_INDEX);
            waitCommandFromSlave(TIME_ALIGN_INDEX, cmd, 1);
            return;
			
          case DSP_PORT_OMX_PORT_INDEX :
#ifdef USE_HYBRID_COMPONENT
            dspportcomponent.sendCommand(cmd, DSP_PORT_LOCAL_OUT_PORT_INDEX);
            waitCommandFromSlave(DSP_PORT_INDEX, cmd, 1);
#else
            downlinkcomponent.sendCommand(cmd, DOWNLINK_LOCAL_OUT_PORT_INDEX);
            waitCommandFromSlave(DOWNLINK_INDEX, cmd, 1);
#endif
            return;
			
		  case CTRL_PORT_INDEX:
			proxy.eventHandler(OMX_EventCmdComplete, cmd, CTRL_PORT_INDEX);
			return;
			  
          case OMX_ALL:
#ifdef USE_HYBRID_COMPONENT
            downlinkcomponent.sendCommand(cmd, DOWNLINK_LOCAL_IN_PORT_INDEX);
            waitCommandFromSlave(DOWNLINK_INDEX, cmd, 1);
			
            dspportcomponent.sendCommand(cmd, DSP_PORT_LOCAL_OUT_PORT_INDEX);
            waitCommandFromSlave(DSP_PORT_INDEX, cmd, 1);
#else
            downlinkcomponent.sendCommand(cmd, DOWNLINK_LOCAL_OUT_PORT_INDEX);
            downlinkcomponent.sendCommand(cmd, DOWNLINK_LOCAL_IN_PORT_INDEX);
            waitCommandFromSlave(DOWNLINK_INDEX, cmd, 2);
#endif
            uplinkcomponent.sendCommand(cmd, UPLINK_LOCAL_OUT_PORT_INDEX);
            waitCommandFromSlave(UPLINK_INDEX, cmd, 1);
			
			timealignmentcomponent.sendCommand(cmd, TIME_ALIGN_LOCAL_IN_PORT_INDEX);
			timealignmentcomponent.sendCommand(cmd, TIME_ALIGN_LOCAL_REF_PORT_INDEX);
			waitCommandFromSlave(TIME_ALIGN_INDEX, cmd, 2);
			// no need to response for CTRL port
            return;
			
          default:
            NMF_PANIC("Speechcontroller:forwardPortCommand: wrong port id [%x]", portIdx); return ; 
        }
      }
      
    case OMX_CommandFlush:
      //TODO handle flush command ??
    default:
      break ;
      
  }
} /* forwardPortCommand */


////////////////////////////////////////////////////////////
// Provided Interfaces 
////////////////////////////////////////////////////////////

void METH(downlink_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
    hst_misc_audiocontroller::slavesEventHandler(event, data1, data2, DOWNLINK_INDEX) ;
  } else {

    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
#ifndef USE_HYBRID_COMPONENT
      if(data1 == 0)
        proxy.eventHandler(event, DSP_PORT_OMX_PORT_INDEX, data2);
      else
#endif
        proxy.eventHandler(event, DOWNLINK_OMX_PORT_INDEX, data2);
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
}


void METH(uplink_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
    hst_misc_audiocontroller::slavesEventHandler(event, data1, data2, UPLINK_INDEX) ;
  } else {

    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
      proxy.eventHandler(event, UPLINK_OMX_PORT_INDEX, data2);
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
}


void METH(timealignment_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
    hst_misc_audiocontroller::slavesEventHandler(event, data1, data2, TIME_ALIGN_INDEX) ;
  } else {

    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
	  if(data1 == TIME_ALIGN_LOCAL_IN_PORT_INDEX)
		proxy.eventHandler(event, TIME_ALIGN_OMX_PORT_INDEX, data2);
	  else if (data1 == TIME_ALIGN_LOCAL_REF_PORT_INDEX)
		proxy.eventHandler(event, TIME_ALIGN_REF_OMX_PORT_INDEX, data2);
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
}


void METH(dspport_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
  if ((event == OMX_EventCmdReceived) || (event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
    hst_misc_audiocontroller::slavesEventHandler(event, data1, data2, DSP_PORT_INDEX) ;
  } else {

    /* Manage other events localy */
    if (event == OMX_EventBufferFlag) {
	  proxy.eventHandler(event, DSP_PORT_OMX_PORT_INDEX, data2);
    } else if (event == OMX_EventError) {
      proxy.eventHandler(event, data1, data2);
    } else {
      ARMNMF_DBC_ASSERT(0);
    }
  } /* if ((ev == OMX_EventCmdComplete) ... */
}


/////////////////////////////////////////
//  Helper functions
/////////////////////////////////////////
t_uint16 speech_proc_nmf_host_speechcontroller::omx2localPortMask(t_uint16 mask, ComponentIndex compo)
{
  t_uint16 new_mask = 0;
  
  switch (compo)
  {
    case DOWNLINK_INDEX:
      if(mask & (1 << DOWNLINK_OMX_PORT_INDEX)) new_mask |= (1 << DOWNLINK_LOCAL_IN_PORT_INDEX);
      break;
    case UPLINK_INDEX:
      if(mask & (1 << UPLINK_OMX_PORT_INDEX))   new_mask |= (1 << UPLINK_LOCAL_OUT_PORT_INDEX);
      break;
    case TIME_ALIGN_INDEX:
      if(mask & (1 << TIME_ALIGN_OMX_PORT_INDEX))     new_mask |= (1 << TIME_ALIGN_LOCAL_IN_PORT_INDEX);
	  if(mask & (1 << TIME_ALIGN_REF_OMX_PORT_INDEX)) new_mask |= (1 << TIME_ALIGN_LOCAL_REF_PORT_INDEX);
      break;
    case DSP_PORT_INDEX:
#ifdef USE_HYBRID_COMPONENT	  
      if(mask & (1 << DSP_PORT_OMX_PORT_INDEX)) new_mask |= (1 << DSP_PORT_LOCAL_OUT_PORT_INDEX);
#else
	  if(mask & (1 << DSP_PORT_OMX_PORT_INDEX)) new_mask |= (1 << DOWNLINK_LOCAL_OUT_PORT_INDEX);
#endif
      break;
    default:
      NMF_PANIC("Speechcontroller wrong component index");
  }
  
  return new_mask;
}
