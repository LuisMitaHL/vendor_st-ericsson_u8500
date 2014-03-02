/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \brief   imgcontroller ARM-NMF component class implementation
*/
#include <imgcontroller.nmf>
#include <OMX_Core.h>
#include <armnmf_dbc.h>
#include "grabctl_types.idt.h"

#define UPLINK_PORT    (0)
#define DOWNLINK_PORT  (1)

void METH(fsmInit)(fsmInit_t inits) {
setTraceInfo(inits.traceInfoAddr, inits.id1);

  init(2, &proxy, &controllers_output[0], &controllers_output[1], false) ;

  setSlave(STAB_INDEX, &stab);
  setSlave(GRABCTL_INDEX, &grabctl);

  /* ignore port_clock */
  inits.portsDisabled &= 0x7; inits.portsTunneled &= 0x7;
  stab_fsminit.fsmInit(inits);

  /* to pass trace informations to DSP, need to convert ARM pointer to the DSP address space */
	inits.traceInfoAddr = (TraceInfo_t *)getDspAddr();
  grabctl_fsminit.fsmInit(inits);
}

void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
    if (portIdx == 3) return; /* ignore port_clock */

    stab_fsminit.setTunnelStatus(portIdx, isTunneled);
    grabctl_fsminit.setTunnelStatus(portIdx, isTunneled);
}

////////////////////////////////////////////////////////////
// Provided Interfaces
////////////////////////////////////////////////////////////

void METH(forwardPortCommand)(OMX_COMMANDTYPE cmd, t_uword id)
{
    /* port enable/disable commands must be sent to both components or their state machines won't
       be in the right state */
    if ((cmd == OMX_CommandPortDisable) || (cmd == OMX_CommandPortEnable) || (OMX_CommandFlush == cmd))
    {
        grabctl.sendCommand(cmd, id);
        waitCommandFromSlave(GRABCTL_INDEX, cmd, 1);
        if(id!=OMX_ALL)id*=2; //adapt portId to Id of stab
    }

    /* all other commands that come through here an be sent just to stab (StateSet doesn't come
       through here, and Flush MarkBuffer can be sent just to Stab */
    stab.sendCommand(cmd, id);
    waitCommandFromSlave(STAB_INDEX, cmd, 1);

}

void METH(forwardPortEvent)(OMX_COMMANDTYPE cmd, t_uword id)
{
    proxy.eventHandler(OMX_EventCmdComplete, cmd, id) ;
}

void METH(grabctl_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
#ifdef ENS_2011_35_0_PRIOR
  if ((event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
#else
  if ((event == OMX_EventCmdComplete) || (event == OMX_EventCmdReceived) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
#endif
    Controller::slavesEventHandler(event, data1, data2, GRABCTL_INDEX) ;
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


void METH(stab_eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
#ifdef ENS_2011_35_0_PRIOR
  if ((event == OMX_EventCmdComplete) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
#else
  if ((event == OMX_EventCmdComplete) || (event == OMX_EventCmdReceived) || ((event == OMX_EventError) && (data1 == (t_uint32)OMX_ErrorInvalidState))) {
#endif

		if((event == OMX_EventCmdComplete) &&((data1 == OMX_CommandPortDisable	)||(data1 == OMX_CommandPortEnable)) &&(data2%2==0))
		{
			stab.sendCommand((OMX_COMMANDTYPE)data1, data2+1);/* forward to the shared port in that case */
			waitCommandFromSlave(STAB_INDEX, (OMX_COMMANDTYPE)data1, 1);
		}
		else {

			/* the port Id is modified in forwardPortCommand, to match stab ports definitions.
			   Hence here we need to revert it back to the client-provided value, in order to
			   avoid issues in ENS in the event Handlers */ 
			if((event == OMX_EventCmdReceived)&& (data2!=OMX_ALL) && ((data1 == OMX_CommandPortDisable)||(data1 == OMX_CommandPortEnable)||(data1 == OMX_CommandFlush)))
			 	data2/=2;   

			Controller::slavesEventHandler(event, data1, data2, STAB_INDEX) ;
		}

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

