/*****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "ImgEns_Shared.h"
#include "osi_toolbox_lib.h"
#include "ImgEns_Fsm.h"
#include "ImgEns_Trace.h"
#include "ImgEns_Port.h"
#include "ImgEns_Port_Fsm.h"
#include "ImgEns_Index.h"
#include "ImgEns_Component_Fsm.h"
#include "ImgEns_ProcessingComponent.h"

#define OMXCOMPONENT "ImgEns_COMPONENT_FSM"
#include "osi_trace.h"

#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "ens_proxy_ENS_Fsm_ENS_Component_FsmTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE

#define TRACE_OMX_API "ImgEns_Component_Fsm"

static const s_FsmStateDescription FsmStateDescription_Unknow={ NULL, "Unknow" , OMX_StateInvalid, OMX_StateInvalid};

const s_FsmStateDescription ImgEns_Component_Fsm::TableFsmStatDescription[] =
{     // State                                                                                          Name                                                 Omx state                         Parent state
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_Loaded                                   , "FsmState_Loaded"                                   , OMX_StateLoaded                  , OMX_StateLoaded                  },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_Idle                                     , "FsmState_Idle"                                     , OMX_StateIdle                    , OMX_StateIdle                    },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_Executing                                , "FsmState_Executing"                                , OMX_StateExecuting               , OMX_StateExecuting               },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_Pause                                    , "FsmState_Pause"                                    , OMX_StatePause                   , OMX_StatePause                   },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_Invalid                                  , "FsmState_Invalid"                                  , OMX_StateInvalid                 , OMX_StateInvalid                 },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_TestForResources                         , "FsmState_TestForResources"                         , OMX_StateTestForResources        , OMX_StateTestForResources        },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_WaitForResources                         , "FsmState_WaitForResources"                         , OMX_StateWaitForResources        , OMX_StateWaitForResources        },

	// OMX IL RM Sub-States
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_IdleSuspended                            , "FsmState_IdleSuspended"                            , OMX_StateIdle                    , OMX_StateIdle                    },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_PauseSuspended                           , "FsmState_PauseSuspended"                           , OMX_StatePause                   , OMX_StatePause                   },

	// Transient States
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_LoadedToIdleHOST                         , "FsmState_LoadedToIdleHOST"                         , OMX_StateLoaded                   , OMX_StateLoaded                 },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_LoadedToIdleDSP                          , "FsmState_LoadedToIdleDSP"                          , OMX_StateLoaded                   , OMX_StateLoaded                 },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_IdleToLoaded                             , "FsmState_IdleToLoaded"                             , OMX_StateIdle                     , OMX_StateIdle                   },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_Transient                                , "FsmState_Transient"                                , OMX_StateMax                      , OMX_StateMax                    },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_LoadedToTestForResources                 , "FsmState_LoadedToTestForResources"                 , OMX_StateLoaded                   , OMX_StateLoaded                 },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Component_Fsm::FsmState_CheckRscAvailability                     , "FsmState_CheckRscAvailability"                     , OMX_StateLoaded                   , OMX_StateLoaded                 },
};

const s_FsmStateDescription &ImgEns_Component_Fsm::GetStateDescription(const ImgEns_Fsm::_fnState state)
//*************************************************************************************************************
{
	const s_FsmStateDescription *pTable=ImgEns_Component_Fsm::TableFsmStatDescription;
	size_t nbr=SIZE_OF_ARRAY(ImgEns_Component_Fsm::TableFsmStatDescription);
	while(nbr > 0)
	{
		if (state== pTable->fn_state)
			return(*pTable);
		++pTable;
		--nbr;
	}
	OTB_ASSERT(0);
	return(FsmStateDescription_Unknow);

}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************


static const unsigned int OMX_ErrorPreemptResourcesRequest = (unsigned int) OMX_ErrorVendorStartUnused;

ImgEns_Component_Fsm::ImgEns_Component_Fsm(ImgEns_Component &ensComp)
: mEnsComponent(ensComp)
, mTargetState(OMX_StateLoaded)
{
	init(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Loaded));
	mTransientState        = OMX_StateInvalid;
	m_BufferSharingEnabled = true;
}

int ImgEns_Component_Fsm::NotifyChangeState(const _fnState currentState, const _fnState nextState) const
//*************************************************************************************************************
{
	const s_FsmStateDescription &_current= GetStateDescription(currentState);
	const s_FsmStateDescription &_next   = GetStateDescription(nextState);
	OstTraceFiltInst3(TRACE_FLOW, "%s: fsm transition from  '%s' to '%s'", mEnsComponent.getName(), _current.Name, _next.Name);
	return(0);
}

OMX_ERRORTYPE ImgEns_Component_Fsm::suspendResources(void)
{
	if (mEnsComponent.resourcesSuspended() == true)
		return OMX_ErrorNone;
	OMX_ERRORTYPE err = mEnsComponent.suspendResources();
	if (err == OMX_ErrorNone)
	{
		mEnsComponent.resourcesSuspended(true);
	}
	return err;
}

OMX_ERRORTYPE ImgEns_Component_Fsm::unsuspendResources(void)
{
	if (mEnsComponent.resourcesSuspended() == false)
		return OMX_ErrorNone;
	OMX_ERRORTYPE err = mEnsComponent.unsuspendResources();
	if (err == OMX_ErrorNone)
	{
		mEnsComponent.resourcesSuspended(false);
	}
	return err;
}

OMX_STATETYPE ImgEns_Component_Fsm::getOMXState() const
{
	const _fnState state=getFsmState();
	if (   state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Loaded) 
		|| state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleHOST)
		|| state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleDSP)
		|| state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToTestForResources)
		|| state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_CheckRscAvailability))
	{
		return OMX_StateLoaded;
	}
	else if ((state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle))
		  || (state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_IdleSuspended)))
	{
		return OMX_StateIdle;
	}
	else if (state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_IdleToLoaded))
	{		// Check what is the target state
		if (mTargetState == OMX_StateIdle)
		{// Means the transition to Idle as failed maybe due to RM issue
			return OMX_StateLoaded;
		}
		return OMX_StateIdle;
	}
	else if (state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Executing))
	{
		return OMX_StateExecuting;
	}
	else if ((state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause))
		  || (state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_PauseSuspended)))
	{
		return OMX_StatePause;
	}
	else if (state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_TestForResources))
	{
		return OMX_StateTestForResources;
	}
	else if (state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_WaitForResources))
	{
		return OMX_StateWaitForResources;
	}
	else if (state == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Transient))
	{
		OMX_STATETYPE previousState, targetState;
		translateTransientState(previousState, targetState);
		return previousState;
	}
	return OMX_StateInvalid;
}

OMX_STATETYPE ImgEns_Component_Fsm::getState() const
{
	return this->getState(this->getFsmState());
}

OMX_STATETYPE ImgEns_Component_Fsm::getState(const _fnState& st) const
{
	if (st == &ImgEns_Component_Fsm::FsmState_Loaded)
	{
		return OMX_StateLoaded;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleHOST))
	{
		return (OMX_STATETYPE)OMX_StateLoadedToIdleHOST;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleDSP))
	{
		return (OMX_STATETYPE)OMX_StateLoadedToIdleDSP;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_CheckRscAvailability))
	{
		return (OMX_STATETYPE)OMX_StateCheckRscAvailability;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToTestForResources))
	{
		return (OMX_STATETYPE)OMX_StateLoadedToTestForResources;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_TestForResources))
	{
		return OMX_StateTestForResources;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_WaitForResources))
	{
		return OMX_StateWaitForResources;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle))
	{
		return OMX_StateIdle;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_IdleSuspended))
	{
		return (OMX_STATETYPE)OMX_StateIdleSuspended;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_IdleToLoaded))
	{
		return (OMX_STATETYPE)OMX_StateIdleToLoaded;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Executing))
	{
		return OMX_StateExecuting;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause))
	{
		return OMX_StatePause;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_PauseSuspended))
	{
		return (OMX_STATETYPE)OMX_StatePauseSuspended;
	}
	else if (st == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Transient))
	{
		return (OMX_STATETYPE)OMX_StateTransient;
	}
	else
	{
		return OMX_StateInvalid;
	}
}

//-----------------------------------------------------------------------------
//! \brief method that computes the state of the component from its ports' states
//!
//! This method updates the state of the component FSM from the current states of
//! the component's ports
//!
//! \param currentState the current state that the component FSM is in
//!
//-----------------------------------------------------------------------------
void ImgEns_Component_Fsm::updateState(_fnState currentState)
{
	if (currentState == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Invalid))
	{ // we are already in invalid state. We stay in this state this could be the case if the component is in invalid state, but none port is
		return;
	}
	_fnState newState                 = currentState;
	OMX_U32 loadedPorts               = 0;
	OMX_U32 idlePorts                 = 0;
	OMX_U32 executingPorts            = 0;
	OMX_U32 pausedPorts               = 0;
	OMX_U32 loadedToIdleHOSTPorts     = 0;
	OMX_U32 loadedToIdleDSPPorts      = 0;
	OMX_U32 loadedToTestForResources  = 0;
	OMX_U32 testForResources          = 0;
	OMX_U32 checkRscAvailabilityPorts = 0;
	OMX_U32 waitForResources          = 0;
	OMX_U32 idleToLoadedPorts         = 0;
	OMX_U32 transientPorts            = 0;
	OMX_U32 invalidPorts              = 0;
	OMX_U32 portCount                 = mEnsComponent.getPortCount();
	for (unsigned int i = 0; i < portCount; i++)
	{
		switch ((int)portGetState(i))
		{
		case OMX_StateLoaded:
			loadedPorts++;
			break;
		case OMX_StateLoadedToIdleHOST:
			loadedToIdleHOSTPorts++;
			break;
		case OMX_StateLoadedToIdleDSP:
			loadedToIdleDSPPorts++;
			break;
		case OMX_StateCheckRscAvailability:
			checkRscAvailabilityPorts++;
			break;
		case OMX_StateIdleToLoaded:
			idleToLoadedPorts++;
			break;
		case OMX_StateIdle:
			idlePorts++;
			break;
		case OMX_StateExecuting:
			executingPorts++;
			break;
		case OMX_StatePause:
			pausedPorts++;
			break;
		case OMX_StateTransient:
			transientPorts++;
			break;
		case OMX_StateInvalid:
			invalidPorts++;
			break;
		case OMX_StateLoadedToTestForResources:
			loadedToTestForResources++;
			break;
		case OMX_StateTestForResources:
			testForResources++;
			break;
		case OMX_StateWaitForResources:
			waitForResources++;
			break;
		default:
			IMGENS_ASSERT(0);
			break;
		}
	}
	if (executingPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Executing);
	}
	else if (pausedPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause);
	}
	else if (idlePorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle);
	}
	else if (testForResources == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_TestForResources);
	}
	else if (waitForResources == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_WaitForResources);
	}
	else if (loadedToTestForResources == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToTestForResources);
	}
	else if (loadedPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Loaded);
	}
	else if (checkRscAvailabilityPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_CheckRscAvailability);
	}
	else if (loadedPorts + loadedToTestForResources == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Loaded);
	}
	else if (loadedToTestForResources + testForResources == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToTestForResources);
	}
	else if (loadedToIdleDSPPorts == portCount || idlePorts + loadedToIdleDSPPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleDSP);
	}
	else if (loadedToIdleDSPPorts + loadedToIdleHOSTPorts == portCount || idlePorts + loadedToIdleHOSTPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleHOST);
	}
	else if (checkRscAvailabilityPorts + loadedToIdleHOSTPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleHOST);
	}
	else if (checkRscAvailabilityPorts + loadedToIdleDSPPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleDSP);
	}
	else if (loadedPorts + idleToLoadedPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_IdleToLoaded);
	}
	else if (transientPorts == portCount)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Transient);
	}
	else if (invalidPorts != 0)
	{
		newState = static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Invalid);
	}
	if (newState != currentState)
	{
		stateTransition(newState);
	}
}

OMX_ERRORTYPE ImgEns_Component_Fsm::dispatchEvtOnPorts(const ImgEns_FsmEvent *evt)
{
	OMX_ERRORTYPE error;
	for (unsigned int i = 0; i < mEnsComponent.getPortCount(); i++)
	{
		error = DispatchToPort(i, evt);
		if (error != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: dispatchEvtOnPorts error: %d", error);
			// OMX_ErrorSameState and OMX_ErrorIncorrectStateTransition must be returned using an EventHandler
			if (error == OMX_ErrorSameState || error == OMX_ErrorIncorrectStateTransition)
			{
				mEnsComponent.eventHandlerCB(OMX_EventError, error, 0);
				return OMX_ErrorNone;
			}
			else
			{
				return error;
			}
		}
	}
	return OMX_ErrorNone;
}

void ImgEns_Component_Fsm::dspEventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2)
{
	switch (ev)
	{
	case OMX_EventCmdComplete:
	{
		MSG1("EventCmdComplete from Img_ProcessingComponent : h=0x%p\n", mEnsComponent.getOMXHandle());
		OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ImgEns_Component_Fsm: EventCmdComplete");

		switch (data1)
		{
		case OMX_CommandStateSet:
		{
			MSG1("  ---> OMX_CommandStateSet, state=%lu\n", data2);
			OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ImgEns_Component_Fsm:  ---> OMX_CommandStateSet, state=%lu", data2);
			ImgEns_CmdSetStateCompleteEvt evt((OMX_STATETYPE) data2);
			//Dispatch SetStateComplete event on each port.
			dispatchEvtOnPorts(&evt);
			//Dispatch SetStateComplete event on component.
			dispatch(&evt);
			return;
		}
		case OMX_CommandPortDisable:
		{
			if (data2 == OMX_ALL)
			{
				for (OMX_U32 i = 0; i < mEnsComponent.getPortCount(); i++)
				{
					//Dispatch disablePortComplete event on each port.
					MSG1("  ---> OMX_CommandPortDisable, port=%lu\n", i);
					OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ImgEns_Component_Fsm:  ---> OMX_CommandPortDisable, port=%lu", i);
					ImgEns_FsmEvent evtPort(OMX_DISABLEPORTCOMPLETE_SIG);
					getPortFsm(i)->dispatch(&evtPort);
				}
			}
			else
			{
				MSG1("  ---> OMX_CommandPortDisable, port=%lu\n", data2);
				OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ImgEns_Component_Fsm:  ---> OMX_CommandPortDisable, port=%lu", data2);
				ImgEns_FsmEvent evtPort(OMX_DISABLEPORTCOMPLETE_SIG);
				DispatchToPort(data2, &evtPort);
			}
			return;
		}
		case OMX_CommandPortEnable:
		{
			if (data2 == OMX_ALL)
			{
				for (OMX_U32 i = 0; i < mEnsComponent.getPortCount(); i++)
				{	//Dispatch enablePortComplete event on each port.
					MSG1("  ---> OMX_CommandPortEnable, port=%lu\n", i);
					OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ImgEns_Component_Fsm:  ---> OMX_CommandPortEnable, port=%lu", i);
					ImgEns_FsmEvent evt(OMX_ENABLEPORTCOMPLETE_SIG);
					getPortFsm(i)->dispatch(&evt);
				}
			}
			else
			{
				MSG1("  ---> OMX_CommandPortEnable, port=%lu\n", data2);
				OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ImgEns_Component_Fsm:  ---> OMX_CommandPortEnable, port=%lu", data2);
				ImgEns_FsmEvent evt(OMX_ENABLEPORTCOMPLETE_SIG);
				DispatchToPort(data2, &evt);
			}
			return;
		}
		case OMX_CommandFlush:
		{
			if (data2 == OMX_ALL)
			{
				for (OMX_U32 i = 0; i < mEnsComponent.getPortCount(); i++)
				{//Dispatch flushPortComplete event on each port.
					MSG1("  ---> OMX_CommandFlush, port=%lu\n", i);
					OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ImgEns_Component_Fsm:  ---> OMX_CommandFlush, port=%lu", i);
					ImgEns_FsmEvent evt(OMX_FLUSHPORTCOMPLETE_SIG);
					DispatchToPort(i, &evt);
				}
			}
			else
			{
				MSG1("  ---> OMX_CommandFlush, port=%lu\n", data2);
				OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ImgEns_Component_Fsm:  ---> OMX_CommandFlush, port=%lu", data2);
				ImgEns_FsmEvent evt(OMX_FLUSHPORTCOMPLETE_SIG);
				DispatchToPort(data2, &evt);
			}
			return;
		}
		default:
			break;
		}
		break;
	}
	case eIMGOMX_EventCmdReceived:
	{
		OstTraceFiltInst2(TRACE_OMX_API, "ENS_PROXY: ImgEns_Component_Fsm:  ---> eIMGOMX_EventCmdReceived data1=%d data2=%d", data1, data2);
		if ((data1 == OMX_CommandPortDisable) || (data1 == OMX_CommandPortEnable))
		{
			//Special case for disablePort/enablePort to manage OMX_ALL + standardTunneling/nonTunneling cases.
			if (data2 == OMX_ALL)
			{ //Send one CmdReceived event for each port
				for (OMX_U32 i = 0; i < mEnsComponent.getPortCount(); i++)
				{
					if (mEnsComponent.getPort(i)->useProprietaryCommunication())
					{	//Only forward event to ENSWrapper in case of proprietary tunneling.
						//Otherwise this event has already been sent by proxy during OMX_SendCommand itself.
						mEnsComponent.eventHandlerCB(ev, data1, i);
					}
				}
			}
			else if (mEnsComponent.getPort(data2)->useProprietaryCommunication())
			{//Only forward event to ENSWrapper in case of proprietary tunneling.
				//Otherwise this event has already been sent by proxy during OMX_SendCommand itself.
				mEnsComponent.eventHandlerCB(ev, data1, data2);
			}
		}
		else if (data1 == OMX_CommandFlush)
		{	//Don't forward eIMGOMX_EventCmdReceived event for OMX_CommandFlush to ENSWrapper
			return;
		}
		else
		{
			mEnsComponent.eventHandlerCB(ev, data1, data2);
		}
		return;
	}
	case OMX_EventError:
	{
		MSG1("  ---> OMX_EventError, error=0x%lx\n", data1);
		OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ImgEns_Component_Fsm:  ---> OMX_EventError, error=0x%lx", data1);
		data1 |= 0x80000000;
		break;
	}
	default:
		break;
	}
	mEnsComponent.eventHandlerCB(ev, data1, data2);
}

void ImgEns_Component_Fsm::setTransientState(_fnState currentState, OMX_STATETYPE targetState)
{
	if (currentState == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle))
	{
		if (targetState == OMX_StateExecuting)
		{
			mTransientState = (OMX_STATETYPE)OMX_StateIdleToExecuting;
		}
		else if (targetState == OMX_StatePause)
		{
			mTransientState = (OMX_STATETYPE)OMX_StateIdleToPause;
		}
		else
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: setTransientState from Idle state assert");
			IMGENS_ASSERT(0);
		}
	}
	else if (currentState == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Executing))
	{
		if (targetState == OMX_StateIdle)
		{
			mTransientState = (OMX_STATETYPE)OMX_StateExecutingToIdle;
		}
		else if (targetState == OMX_StatePause)
		{
			mTransientState = (OMX_STATETYPE)OMX_StateExecutingToPause;
		}
		else
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: setTransientState from Executing state assert");
			IMGENS_ASSERT(0);
		}
	}
	else if (currentState == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause))
	{
		if (targetState == OMX_StateIdle)
		{
			mTransientState = (OMX_STATETYPE)OMX_StatePauseToIdle;
		}
		else if (targetState == OMX_StateExecuting)
		{
			mTransientState = (OMX_STATETYPE)OMX_StatePauseToExecuting;
		}
		else
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: setTransientState from Pause state assert");
			IMGENS_ASSERT(0);
		}
	}
	else
	{
		OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: setTransientState assert");
		IMGENS_ASSERT(0);
	}
}

void ImgEns_Component_Fsm::translateTransientState(OMX_STATETYPE &previousState, OMX_STATETYPE &targetState) const
{
	previousState = OMX_StateInvalid;
	targetState   = OMX_StateInvalid;
	switch ((int)mTransientState)
	{
	case OMX_StateIdleToExecuting:
		previousState = OMX_StateIdle;
		targetState   = OMX_StateExecuting;
		break;
	case OMX_StateIdleToPause:
		previousState = OMX_StateIdle;
		targetState   = OMX_StatePause;
		break;
	case OMX_StateExecutingToIdle:
		previousState = OMX_StateExecuting;
		targetState   = OMX_StateIdle;
		break;
	case OMX_StateExecutingToPause:
		previousState = OMX_StateExecuting;
		targetState   = OMX_StatePause;
		break;
	case OMX_StatePauseToIdle:
		previousState = OMX_StatePause;
		targetState   = OMX_StateIdle;
		break;
	case OMX_StatePauseToExecuting:
		previousState = OMX_StatePause;
		targetState   = OMX_StateExecuting;
		break;
	default:
		previousState = OMX_StateInvalid;
		targetState   = OMX_StateInvalid;
		break;
	}
}

OMX_ERRORTYPE ImgEns_Component_Fsm::setState(const ImgEns_FsmEvent *evt, _fnState currentState)
{ // Store the target state
	ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
	mTargetState = setStateEvt->targetState();
	return dispatchEvtAndUpdateState(evt, currentState);
}

OMX_ERRORTYPE ImgEns_Component_Fsm::dispatchEvtAndUpdateState(const ImgEns_FsmEvent *evt, _fnState currentState)
{
	OMX_ERRORTYPE error;
	// Dispatch the evt
	error = dispatchEvtOnPorts(evt);
	if (error != OMX_ErrorNone)
	{
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: dispatchEvtAndUpdateState error: %d", error);
		return error;
	}

	// Update the state and the transient state
	updateState(currentState);

	if (getFsmState() == static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Transient))
	{ //store transient State
		setTransientState(currentState, mTargetState);
	}
	else
	{
		mTransientState = OMX_StateInvalid;
	}
	return OMX_ErrorNone;
}

//-----------------------------------------------------------------------------
//! \brief Default Behavior for all states of the component FSM
//!
//! This method implements default behavior for all states of the component FSM.
//! It is intended to be called in the default case of each state handler method.
//! State handlers should handle events only when the behavior is different.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::defaultBehavior(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{
	case eSignal_Entry:
		return OMX_ErrorNone;

	case eSignal_Exit:
		return OMX_ErrorNone;

	case OMX_GETPARAMETER_SIG:
	{
		ImgEns_CmdGetParamEvt *getParamEvt = (ImgEns_CmdGetParamEvt *) evt;
		return mEnsComponent.getParameter(getParamEvt->getIndex(), getParamEvt->getStructPtr());
	}

	case OMX_SETCONFIG_SIG:
	{
		ImgEns_CmdSetConfigEvt *setConfigEvt = (ImgEns_CmdSetConfigEvt *) evt;
		OMX_ERRORTYPE err;
		err = mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
		if (err != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: defaultBehavior SETCONFIG_SIG error: %d", err);
			return err;
		}
		if ((unsigned long) setConfigEvt->getIndex() == (unsigned long) eEnsIndex_ConfigTraceSetting)
		{
			return OMX_ErrorNone; // do not propagate such a config
		}
		return mEnsComponent.getProcessingComponent().applyConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
	}

	case OMX_GETCONFIG_SIG:
	{
		ImgEns_CmdGetConfigEvt *getConfigEvt = (ImgEns_CmdGetConfigEvt *) evt;
		OMX_ERRORTYPE err;
		err = mEnsComponent.getProcessingComponent().retrieveConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
		if (err == OMX_ErrorUnsupportedIndex)
		{
			return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
		}
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: defaultBehavior GETCONFIG_SIG error: %d", err);
		return err;
	}

	case OMX_GETEXTENSIONIDX_SIG:
	{
		ImgEns_CmdGetExtIdxEvt *getExtIdxEvt = (ImgEns_CmdGetExtIdxEvt *) evt;
		return mEnsComponent.getExtensionIndex(getExtIdxEvt->getParamName(), getExtIdxEvt->getIndexTypePtr());
	}

	case OMX_USE_BUFFER_SIG:
	{
		ImgEns_CmdUseBufferEvt *useBufferEvt = (ImgEns_CmdUseBufferEvt *) evt;
		return (DispatchToPort(useBufferEvt->getPortIdx(), evt));
	}

	case OMX_ALLOCATE_BUFFER_SIG:
	{
		ImgEns_CmdAllocBufferEvt *allocBufferEvt = (ImgEns_CmdAllocBufferEvt *) evt;
		return (DispatchToPort(allocBufferEvt->getPortIdx(), evt));
	}

	case OMX_FREE_BUFFER_SIG:
	{
		ImgEns_CmdFreeBufferEvt *freeBufferEvt = (ImgEns_CmdFreeBufferEvt *) evt;
		return (DispatchToPort(freeBufferEvt->getPortIdx(), evt));
	}

	case OMX_EMPTY_THIS_BUFFER_SIG:
	{
		ImgEns_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ImgEns_CmdEmptyThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = emptyThisBufferEvt->getOMXBufferHdrPtr();
		return (DispatchToPort(pBuffer->nInputPortIndex, evt));
	}

	case OMX_FILL_THIS_BUFFER_SIG:
	{
		ImgEns_CmdFillThisBufferEvt *fillThisBufferEvt = (ImgEns_CmdFillThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = fillThisBufferEvt->getOMXBufferHdrPtr();
		return (DispatchToPort(pBuffer->nOutputPortIndex, evt));
	}

	case OMX_EMPTY_BUFFER_DONE_SIG:
	{
		ImgEns_CmdEmptyBufferDoneEvt *emptyBufferDoneEvt = (ImgEns_CmdEmptyBufferDoneEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();
		return (DispatchToPort(pBuffer->nInputPortIndex, evt));
	}

	case OMX_FILL_BUFFER_DONE_SIG:
	{
		ImgEns_CmdFillBufferDoneEvt *fillBufferDoneEvt = (ImgEns_CmdFillBufferDoneEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();
		return (DispatchToPort(pBuffer->nOutputPortIndex, evt));
	}

	case OMX_RESRESERVATIONPROCESSED_SIG:
	{
#ifdef USE_RME
		ImgEns_CmdResReservationProcessedEvt *ackRscEvt = (ImgEns_CmdResReservationProcessedEvt *) evt;
		// Can occur if component resources were suspended by disabling/enabling ports
		if (mEnsComponent.resourcesSuspended() == true)
		{
			if (OMX_TRUE == ackRscEvt->resGranted())
			{
				// Inform RM
				ImgEns_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), mEnsComponent.omxState(), RM_RS_SATISFIED, 0);
				// Let component restore suspended resources
				return this->unsuspendResources();
			}
			else
			{
				mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorInsufficientResources, 0);
				return OMX_ErrorInsufficientResources;
			}
		}
		else
		{
			// ?? unexpected event
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: defaultBehavior error: incorrect event received");
			return OMX_ErrorIncorrectStateOperation;
		}
#endif
	}
	default:
	{
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: defaultBehavior error for cmd %d: incorrect state operation", evt->signal());
		IMGENS_ASSERT(0);
		return OMX_ErrorIncorrectStateOperation;
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Loaded state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Loaded state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_Loaded(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{
	case eSignal_Entry:
	{
		// TODO: use previousState info instead
		if (mTargetState != OMX_StateIdle)
		{
			// No notification if target state was Idle and never reached
			mEnsComponent.eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateLoaded);
		}
		return OMX_ErrorNone;
	}

	case eSignal_Exit:
	{
		if (m_BufferSharingEnabled)
		{
			if (mEnsComponent.isBufferSharingEnabled())
			{
				mEnsComponent.createBufferSharingGraph();
			}
			else
			{
				mEnsComponent.disableBufferSharing();
			}
		}
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG:
	{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		if (setStateEvt->targetState() == OMX_StateIdle)
		{//send eIMGOMX_EventCmdReceived to ENSWrapper : will not be sent by processingComponent when transitionning from loaded to idle state.
			mEnsComponent.eventHandlerCB((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandStateSet, OMX_StateIdle);
		}
		return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Loaded));
	}

	case OMX_SETPARAMETER_SIG:
	{
		ImgEns_CmdSetParamEvt *setParamEvt = (ImgEns_CmdSetParamEvt *) evt;
		return mEnsComponent.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
	}

	case OMX_SETCONFIG_SIG:
	{
		ImgEns_CmdSetConfigEvt *setConfigEvt = (ImgEns_CmdSetConfigEvt *) evt;
		return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
	}

	case OMX_GETCONFIG_SIG:
	{
		ImgEns_CmdGetConfigEvt *getConfigEvt = (ImgEns_CmdGetConfigEvt *) evt;
		return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
	}

	case OMX_SET_CALLBACKS_SIG:
	{
		ImgEns_CmdSetCbEvt * setCbEvt = (ImgEns_CmdSetCbEvt *) evt;
		return mEnsComponent.registerILClientCallbacks(setCbEvt->getCbStructPtr(), setCbEvt->getAppDataPtr());
	}

	default:
	{
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Idle state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Idle state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_Idle(const ImgEns_FsmEvent *evt)
{
	OMX_ERRORTYPE error;

	switch (evt->signal())
	{

	case eSignal_Entry:
	{// Case Idle preempted
		if (mEnsComponent.getPreemptionState() == ImgEns_Component::Preempt_Total)
		{
			mEnsComponent.eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle);
			// The component has been totally preempted by RM, it shall move in loaded state
			// Send the evt OMX_ErrorResourcesLost to the client
			mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorResourcesLost, 0);

			// Move  component to loaded
			ImgEns_CmdSetStateEvt setStateEvt(OMX_StateLoaded);
			return setState(&setStateEvt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle));
		}

		// Notify the client
		ImgEns_EntryEvt* entryEvt = (ImgEns_EntryEvt *) evt;
		int prevState = this->getState(entryEvt->previousState());
		if (prevState != OMX_StateIdleSuspended)
		{
			mEnsComponent.eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle);
		}
		return OMX_ErrorNone;
	}

	case eSignal_Exit:
	{
		int fsmState = this->getState();
		if (fsmState == OMX_StateLoaded || fsmState == OMX_StateIdleToLoaded)
		{
#if 0
			if (mEnsComponent.getEnsWrapperCB() != 0)
			{
				// Delegate processing to wrapper if possible
				if (mEnsComponent.getEnsWrapperCB()->DestroyENSProcessingPartAsync != 0)
				{
					// Delegate destruction processing
					mEnsComponent.getEnsWrapperCB()->DestroyENSProcessingPartAsync(mEnsComponent.getOMXHandle());
				}
			}
			else
#endif
			{
				error = mEnsComponent.getProcessingComponent().destroy();
				if (error != OMX_ErrorNone)
				{
					mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) error, 0);
				}
			}
		}
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG:
	{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		if (OMX_StateIdleSuspended == (int)setStateEvt->targetState())
		{
			// Move to sub state idle-suspended
			stateTransition(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_IdleSuspended));
			return OMX_ErrorNone;
		}

		if (mEnsComponent.getPreemptionState() == ImgEns_Component::Preempt_Total)
		{
			// Component can go in loaded state only
			if (OMX_StateLoaded == setStateEvt->targetState())
			{
				return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle));
			}
			else
			{
				return OMX_ErrorComponentSuspended;
			}
		}
		else
		{
			return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle));
		}
	}

	case OMX_RELEASERESOURCE_SIG:
	{
		// The component is preempted (fully or partially) by RME
		ImgEns_CmdReleaseResourceEvt *releaseResourceEvt = (ImgEns_CmdReleaseResourceEvt *) evt;
		if (OMX_FALSE == releaseResourceEvt->resSuspend())
		{
			// Full Preemption
			// Send evt to the client
			mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorResourcesLost, 0);
			// Change state to Loaded
			ImgEns_CmdSetStateEvt setStateEvt(OMX_StateLoaded);
			return setState(&setStateEvt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle));
		}
		else
		{
			// Suspension
			// Send evt to the client
			mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorComponentSuspended, 0);
			// Synchronous call to release resources
			OMX_ERRORTYPE err = this->suspendResources();
			if (err != OMX_ErrorNone)
			{
				return err;
			}
			// Move state to idle-suspended
			ImgEns_CmdSetStateEvt setStateEvt((OMX_STATETYPE)OMX_StateIdleSuspended);
			// return this->dispatch(&setStateEvt);
			return Dispatch(&setStateEvt);

		}
	}
	default:
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Executing state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Executing state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_Executing(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{

	case eSignal_Entry:
	{// Notify the client
		mEnsComponent.eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateExecuting);
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG:
	{
		return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Executing));
	}

	case OMX_RELEASERESOURCE_SIG:
	{
		ImgEns_CmdReleaseResourceEvt *releaseResourceEvt = (ImgEns_CmdReleaseResourceEvt *) evt;
		if (OMX_FALSE == releaseResourceEvt->resSuspend())
		{
			// Full Preemption
			if (mEnsComponent.getPreemptionPolicy())
			{
				// Send preemption request event to the client
				mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorPreemptResourcesRequest, 0);
				return OMX_ErrorNone;
			}
			else
			{
				// Send evt to the client
				mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorResourcesPreempted, 0);
				// Change state to idle preempted
				ImgEns_CmdSetStateEvt setStateEvt(OMX_StateIdle);
				return setState(&setStateEvt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Executing));
			}
		}
		else
		{
			// Partial suspension
			// Send evt to the client
			mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorComponentSuspended, 0);
			// Synchronous call to release resources
			OMX_ERRORTYPE err = this->suspendResources();
			if (err != OMX_ErrorNone)
			{
				return err;
			}
			// Move state to pause then pause-suspended
			ImgEns_CmdSetStateEvt setStateEvt(OMX_StatePause);
			return setState(&setStateEvt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Executing));
		}
	}

	default:
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Pause state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Pause state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_Pause(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{

	case eSignal_Entry:
	{// Notify the client
		ImgEns_EntryEvt* entryEvt = (ImgEns_EntryEvt *) evt;
		int prevState = this->getState(entryEvt->previousState());
		if (prevState != OMX_StatePauseSuspended && prevState != OMX_StatePause)
		{
			mEnsComponent.eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StatePause);
		}

		if (mEnsComponent.getPreemptionState() == ImgEns_Component::Preempt_Partial)
		{
			// Move state to pause-suspended
			ImgEns_CmdSetStateEvt setStateEvt((OMX_STATETYPE)OMX_StatePauseSuspended);
			//return this->dispatch(&setStateEvt);
			return Dispatch(&setStateEvt);
		}
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG:
	{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		if (mEnsComponent.getPreemptionState() == ImgEns_Component::Preempt_Total)
		{
			// Can only move to state Idle
			if (OMX_StateIdle == setStateEvt->targetState())
			{
				return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause));
			}
			else
			{
				return OMX_ErrorComponentSuspended;
			}
		}

		if (OMX_StatePauseSuspended == (int)setStateEvt->targetState())
		{
			// Move to sub state pause-suspended
			stateTransition(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_PauseSuspended));
			return OMX_ErrorNone;
		}
		return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause));
	}

	case OMX_RELEASERESOURCE_SIG:
	{
		// The component is suspended or preempted
		ImgEns_CmdReleaseResourceEvt *releaseResourceEvt = (ImgEns_CmdReleaseResourceEvt *) evt;
		if (OMX_FALSE == releaseResourceEvt->resSuspend())
		{
			// Full suspension
			if (mEnsComponent.getPreemptionPolicy())
			{
				// Send preemption request event to the client
				mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorPreemptResourcesRequest, 0);
				return OMX_ErrorNone;
			}
			else
			{
				// Send evt to the client
				mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorResourcesPreempted, 0);
				// Change state to idle preempted
				ImgEns_CmdSetStateEvt setStateEvt(OMX_StateIdle);
				return setState(&setStateEvt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause));
			}
		}
		else
		{
			// Partial suspension
			// Send evt to the client
			mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorComponentSuspended, 0);
			// Synchronous call to release resources
			OMX_ERRORTYPE err = this->suspendResources();
			if (err != OMX_ErrorNone)
			{
				return err;
			}
			// Move state to pause-suspended
			ImgEns_CmdSetStateEvt setStateEvt((OMX_STATETYPE)OMX_StatePauseSuspended);
			return Dispatch(&setStateEvt);
		}
	}
	default:
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Invalid state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Invalid state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_Invalid(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{

	case eSignal_Entry:
	{
		return mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorInvalidState, 0);
	}
	case OMX_FREE_BUFFER_SIG:
	{
		ImgEns_CmdFreeBufferEvt *freeBufferEvt = (ImgEns_CmdFreeBufferEvt *) evt;
		return DispatchToPort(freeBufferEvt->getPortIdx(), evt);
	}

	default:
		OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateInvalid error");
		return OMX_ErrorInvalidState;
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for the first transient state from Loaded to Idle
//!
//! This method implements the behavior of the component FSM when the component
//! is transitionning from Loaded to Idle. This transient state waits for calls
//! to OMX_AllocateBuffer or OMX_UseBuffer on all non-supplier ports of the component.
//! It then transitions to OmxStateLoadedToIdleDsp.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_LoadedToIdleHOST(const ImgEns_FsmEvent *evt)
{

	OMX_ERRORTYPE error;

	switch (evt->signal())
	{
	case eSignal_Entry:
	{
		return OMX_ErrorNone;
	}

	case OMX_SETCONFIG_SIG:
	{
		ImgEns_CmdSetConfigEvt *setConfigEvt = (ImgEns_CmdSetConfigEvt *) evt;
		return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
	}

	case OMX_GETCONFIG_SIG:
	{
		ImgEns_CmdGetConfigEvt *getConfigEvt = (ImgEns_CmdGetConfigEvt *) evt;
		return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
	}

	case OMX_USE_BUFFER_SIG:
	{
		ImgEns_CmdUseBufferEvt *useBufferEvt = (ImgEns_CmdUseBufferEvt *) evt;

		/* ImgEns_Port_Fsm *portfsm = getPortFsm(useBufferEvt->getPortIdx());
		 if (!portfsm) {
		 return OMX_ErrorBadPortIndex;
		 }
		 error = portfsm->dispatch(evt);*/
		error = DispatchToPort(useBufferEvt->getPortIdx(), evt);
		if (error != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateLoadedToIdleHOST USE_BUFFER_SIG error: %d", error);
			return error;
		}

		updateState(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleHOST));

		return OMX_ErrorNone;
	}

	case OMX_ALLOCATE_BUFFER_SIG:
	{
		ImgEns_CmdAllocBufferEvt *allocBufferEvt = (ImgEns_CmdAllocBufferEvt *) evt;
		/* ImgEns_Port_Fsm *portfsm = getPortFsm(allocBufferEvt->getPortIdx());
		 if (!portfsm) {
		 return OMX_ErrorBadPortIndex;
		 }
		 error = portfsm->dispatch(evt); */
		error = DispatchToPort(allocBufferEvt->getPortIdx(), evt);
		if (error != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateLoadedToIdleHOST ALLOCATE_BUFFER_SIG error: %d", error);
			return error;
		}
		updateState(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleHOST));
		return OMX_ErrorNone;
	}

	default:
	{
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for the second transient state from Loaded to Idle
//!
//! This method implements the behavior of the component FSM when the component
//! is transitionning from Loaded to Idle. The component will transition to this
//! state when all its ports are populated. On entry to this state, the processing
//! component's construct method is called. When the callback from the processing
//! component is called to tell that the processing component is in Idle state, the
//! component FSM transitions to OmxStateIdle.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_LoadedToIdleDSP(const ImgEns_FsmEvent *evt)
{

	OMX_ERRORTYPE error;

	switch (evt->signal())
	{

	case eSignal_Entry:
	{
		error = mEnsComponent.getProcessingComponent().construct();
		if (error != OMX_ErrorNone)
		{
			mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) error, 0);
		}
		return OMX_ErrorNone;
	}
	case OMX_SETSTATECOMPLETE_SIG:
	{
		updateState(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToIdleDSP));
		return OMX_ErrorNone;
	}
	default:
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for transient state from Idle to Loaded
//!
//! This method implements the behavior of the component FSM when the component
//! is transitionning from Idle to Loaded and the component has some non-supplier ports.
//! When the component FSM transitions to this state, the processing component's
//! destroy method has already been called. This state then waits for calls to OMX_FreeBuffer
//! on all non-supplier ports. It then transitions to Loaded state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_IdleToLoaded(const ImgEns_FsmEvent *evt)
{

	OMX_ERRORTYPE error;

	switch (evt->signal())
	{

	case eSignal_Entry:
	{
		return OMX_ErrorNone;
	}

	case OMX_FREE_BUFFER_SIG:
	{
		ImgEns_CmdFreeBufferEvt *freeBufferEvt = (ImgEns_CmdFreeBufferEvt *) evt;

		/* ImgEns_Port_Fsm *portfsm = getPortFsm(freeBufferEvt->getPortIdx());
		 if (!portfsm) {
		 return OMX_ErrorBadPortIndex;
		 }
		 error = portfsm->dispatch(evt); */
		error = DispatchToPort(freeBufferEvt->getPortIdx(), evt);
		if (error != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateIdleToLoaded FREE_BUFFER_SIG error: %d", error);
			return error;
		}
		updateState(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_IdleToLoaded));
		return OMX_ErrorNone;
	}

	default:
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for all other transient states
//!
//! This method implements the behavior of the component FSM when the component
//! is transitionning from one state to another and this transition only implies
//! forwarding the command to the processing component and waiting for the CmdComplete
//! callback
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_Transient(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{
	case eSignal_Entry:
	{
		OMX_STATETYPE previousState, targetState;
		translateTransientState(previousState, targetState);
		// TODO: eSignal_Entry shall not return error
		return mEnsComponent.getProcessingComponent().sendCommand(OMX_CommandStateSet, targetState);
	}

	case OMX_SETSTATECOMPLETE_SIG:
	{
		updateState(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Transient));
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_THIS_BUFFER_SIG:
	{
		ImgEns_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ImgEns_CmdEmptyThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = emptyThisBufferEvt->getOMXBufferHdrPtr();
		OMX_ERRORTYPE error;
		error = DispatchToPort(pBuffer->nInputPortIndex, evt);
		if (error != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateTransient EMPTY_THIS_BUFFER_SIG error: %d", error);
			return error;
		}

		updateState(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Transient));
		return OMX_ErrorNone;
	}

	case OMX_FILL_THIS_BUFFER_SIG:
	{
		ImgEns_CmdFillThisBufferEvt *fillThisBufferEvt = (ImgEns_CmdFillThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = fillThisBufferEvt->getOMXBufferHdrPtr();
		OMX_ERRORTYPE error;
		error = DispatchToPort(pBuffer->nOutputPortIndex, evt);
		if (error != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateTransient FILL_THIS_BUFFER_SIG error: %d", error);
			return error;
		}
		updateState(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Transient));
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_BUFFER_DONE_SIG:
	{
		ImgEns_CmdEmptyBufferDoneEvt *emptyBufferDoneEvt = (ImgEns_CmdEmptyBufferDoneEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();
		OMX_ERRORTYPE error;
		error = DispatchToPort(pBuffer->nInputPortIndex, evt);
		if (error != OMX_ErrorNone)
			return error;
		updateState(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Transient));
		return OMX_ErrorNone;
	}

	case OMX_FILL_BUFFER_DONE_SIG:
	{
		ImgEns_CmdFillBufferDoneEvt *fillBufferDoneEvt = (ImgEns_CmdFillBufferDoneEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();
		OMX_ERRORTYPE error;
		error = DispatchToPort(pBuffer->nOutputPortIndex, evt);
		if (error != OMX_ErrorNone)
			return error;
		updateState(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Transient));
		return OMX_ErrorNone;
	}

	default:
		return defaultBehavior(evt);
	}
}

OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_TestForResources(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{
	case eSignal_Entry:
	{
		// Notify the client
		mEnsComponent.eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateTestForResources);
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG:
	{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		if (setStateEvt->targetState() == OMX_StateIdle)
		{
			//send eIMGOMX_EventCmdReceived to ENSWrapper : will not be sent by processingComponent when transitionning from TestForResources to idle state.
			mEnsComponent.eventHandlerCB((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandStateSet, OMX_StateIdle);
		}
		return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_TestForResources));
	}

	case OMX_SETCONFIG_SIG:
	{
		ImgEns_CmdSetConfigEvt *setConfigEvt = (ImgEns_CmdSetConfigEvt *) evt;
		return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
	}

	case OMX_RELEASERESOURCE_SIG:
	{
		ImgEns_CmdReleaseResourceEvt *releaseResourceEvt = (ImgEns_CmdReleaseResourceEvt *) evt;
		if (OMX_FALSE == releaseResourceEvt->resSuspend())
		{
			// Full suspension
			// Send evt to the client
			mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorResourcesLost, 0);
			// Change state to loaded
			ImgEns_CmdSetStateEvt setStateEvt(OMX_StateLoaded);
			return setState(&setStateEvt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_TestForResources));
		}
		return OMX_ErrorNone;
	}

	case OMX_GETCONFIG_SIG:
	{
		ImgEns_CmdGetConfigEvt *getConfigEvt = (ImgEns_CmdGetConfigEvt *) evt;
		return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
	}

	case OMX_USE_BUFFER_SIG:
	case OMX_ALLOCATE_BUFFER_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateTransient error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default:
	{
		return defaultBehavior(evt);
	}
	}
}

OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_WaitForResources(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{
	case eSignal_Entry:
	{
		// Notify the client
		mEnsComponent.eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateWaitForResources);
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG:
	{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		if (setStateEvt->targetState() == OMX_StateIdle)
		{ //send eIMGOMX_EventCmdReceived to ENSWrapper : will not be sent by processingComponent when transitionning from OmxStateWaitForResources to idle state.
			mEnsComponent.eventHandlerCB((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandStateSet, OMX_StateIdle);
		}
		return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_WaitForResources));
	}

	case OMX_SETCONFIG_SIG:
	{
		ImgEns_CmdSetConfigEvt *setConfigEvt = (ImgEns_CmdSetConfigEvt *) evt;
		return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
	}

	case OMX_GETCONFIG_SIG:
	{
		ImgEns_CmdGetConfigEvt *getConfigEvt = (ImgEns_CmdGetConfigEvt *) evt;
		return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
	}

	case OMX_USE_BUFFER_SIG:
	case OMX_ALLOCATE_BUFFER_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateWaitForResources error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default:
		return defaultBehavior(evt);
	}
}

OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_LoadedToTestForResources(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{
	case eSignal_Entry:
	{
		const ImgEns_CmdResReservationProcessedEvt Evt(OMX_TRUE);
		Dispatch(&Evt);
		return OMX_ErrorNone;
	}

	case OMX_RESRESERVATIONPROCESSED_SIG:
	{
		OMX_ERRORTYPE err = dispatchEvtAndUpdateState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_LoadedToTestForResources));
		if (err != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateLoadedToTestForResources RESRESERVATIONPROCESSED_SIG error: %d", err);
			return err;
		}

		ImgEns_CmdResReservationProcessedEvt *ackRscEvt = (ImgEns_CmdResReservationProcessedEvt *) evt;
		if (OMX_FALSE == ackRscEvt->resGranted())
		{
			return mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorInsufficientResources, 0);
		}
		return OMX_ErrorNone;
	}

	case OMX_SETCONFIG_SIG:
	{
		ImgEns_CmdSetConfigEvt *setConfigEvt = (ImgEns_CmdSetConfigEvt *) evt;
		return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
	}

	case OMX_GETCONFIG_SIG:
	{
		ImgEns_CmdGetConfigEvt *getConfigEvt = (ImgEns_CmdGetConfigEvt *) evt;
		return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
	}

	case OMX_USE_BUFFER_SIG:
	case OMX_ALLOCATE_BUFFER_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateLoadedToTestForResources error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default:
	{
		return defaultBehavior(evt);
	}
	}
}

OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_CheckRscAvailability(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{
	case eSignal_Entry:
	{
		const ImgEns_CmdResReservationProcessedEvt Evt(OMX_TRUE);
		Dispatch(&Evt);
		return OMX_ErrorNone;
	}

	case OMX_RESRESERVATIONPROCESSED_SIG:
	{
		OMX_ERRORTYPE err = dispatchEvtAndUpdateState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_CheckRscAvailability));
		if (err != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateCheckRscAvailability RESRESERVATIONPROCESSED_SIG error: %d", err);
			return err;
		}
		ImgEns_CmdResReservationProcessedEvt *ackRscEvt = (ImgEns_CmdResReservationProcessedEvt *) evt;
		if (OMX_FALSE == ackRscEvt->resGranted())
		{
			mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorInsufficientResources, 0);
		}
		return OMX_ErrorNone;
	}

	case OMX_SETCONFIG_SIG:
	{
		ImgEns_CmdSetConfigEvt *setConfigEvt = (ImgEns_CmdSetConfigEvt *) evt;
		return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
	}

	case OMX_GETCONFIG_SIG:
	{
		ImgEns_CmdGetConfigEvt *getConfigEvt = (ImgEns_CmdGetConfigEvt *) evt;
		return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
	}

	case OMX_USE_BUFFER_SIG:
	case OMX_ALLOCATE_BUFFER_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateCheckRscAvailability error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default:
	{
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Idle-Suspended state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Idle Suspended state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_IdleSuspended(const ImgEns_FsmEvent *evt)
{

	switch (evt->signal())
	{

	case eSignal_Entry:
	{// This state is reachable from Idle state only. No need to notify client
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG:
	{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		if (OMX_StateIdle == setStateEvt->targetState())
		{// Move back to idle
			stateTransition(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle));
			return OMX_ErrorNone;
		}
		if (OMX_StateExecuting != setStateEvt->targetState())
		{// Component not allowed to go in executing state if suspended
			return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle));
		}
		else
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStateIdleSuspended error: Component suspended");
			return OMX_ErrorComponentSuspended;
		}
	}

	case OMX_RELEASERESOURCE_SIG:
	{
		ImgEns_CmdReleaseResourceEvt *releaseResourceEvt = (ImgEns_CmdReleaseResourceEvt *) evt;
		if (OMX_FALSE == releaseResourceEvt->resSuspend())
		{// Full suspension. The suspended component is now preempted by RME
			// Send evt to the client
			mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorResourcesLost, 0);
			// Change state to loaded
			ImgEns_CmdSetStateEvt setStateEvt(OMX_StateLoaded);
			return setState(&setStateEvt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Idle));
		}
		else
		{// Component already suspended. Should not occur
			return OMX_ErrorComponentSuspended;
		}
	}

	case OMX_RESRESERVATIONPROCESSED_SIG:
	{// The component is unsuspended by RME
		ImgEns_CmdResReservationProcessedEvt *ackRscEvt = (ImgEns_CmdResReservationProcessedEvt *) evt;
		if (OMX_TRUE == ackRscEvt->resGranted())
		{ // Reset the suspension flag
			mEnsComponent.setPreemptionState(ImgEns_Component::Preempt_None);
			OMX_ERRORTYPE err = this->unsuspendResources();
			if (err != OMX_ErrorNone)
			{
				return err;
			}
			// Notify the client and send OMX_EventComponentResumed event
			mEnsComponent.eventHandlerCB(OMX_EventComponentResumed, 0, 0);
			ImgEns_CmdSetStateEvt setStateEvt(OMX_StateIdle);
			return Dispatch(&setStateEvt);
		}
		return OMX_ErrorNone;
	}

	default:
	{
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Pause-Suspended state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Pause-Suspended state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Component_Fsm::FsmState_PauseSuspended(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{
	case eSignal_Entry:
	{// This state is reachable from Pause state only.No need to notify client
		return OMX_ErrorNone;
	}
	case OMX_SETSTATE_SIG:
	{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		if (OMX_StatePause == setStateEvt->targetState())
		{ // Move back to pause
			stateTransition(static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause));
			return OMX_ErrorNone;
		}
		// Component not allowed to move from suspended to executing
		if (OMX_StateExecuting == setStateEvt->targetState())
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Component_Fsm: OmxStatePauseSuspended error: Component suspended");
			return OMX_ErrorComponentSuspended;
		}
		else
		{
			return setState(evt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause));
		}
	}

	case OMX_RELEASERESOURCE_SIG:
	{
		ImgEns_CmdReleaseResourceEvt *releaseResourceEvt = (ImgEns_CmdReleaseResourceEvt *) evt;
		if (OMX_FALSE == releaseResourceEvt->resSuspend())
		{
			// Full suspension. The suspended component is now preempted by RME
			if (mEnsComponent.getPreemptionPolicy())
			{
				// Send preemption request event to the client
				mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorPreemptResourcesRequest, 0);
				return OMX_ErrorNone;
			}
			else
			{
				// Change state to idle preempted
				mEnsComponent.eventHandlerCB(OMX_EventError, (OMX_U32) OMX_ErrorResourcesPreempted, 0);
				ImgEns_CmdSetStateEvt setStateEvt(OMX_StateIdle);
				return setState(&setStateEvt, static_cast<_fnState>(&ImgEns_Component_Fsm::FsmState_Pause));
			}
		}
		else
		{// Component already suspended. Should not occur
			return OMX_ErrorComponentSuspended;
		}
	}

	case OMX_RESRESERVATIONPROCESSED_SIG:
	{// The component is unsuspended by RME
		ImgEns_CmdResReservationProcessedEvt *ackRscEvt = (ImgEns_CmdResReservationProcessedEvt *) evt;
		if (OMX_TRUE == ackRscEvt->resGranted())
		{
			// Reset the suspension flag
			mEnsComponent.setPreemptionState(ImgEns_Component::Preempt_None);
			// Re-acquire lost resources
			OMX_ERRORTYPE err = this->unsuspendResources();
			if (err != OMX_ErrorNone)
			{
				return err;
			}
			// Notify the client and send OMX_EventComponentResumed event
			mEnsComponent.eventHandlerCB(OMX_EventComponentResumed, 0, 0);
			ImgEns_CmdSetStateEvt setStateEvt(OMX_StatePause);
			return Dispatch(&setStateEvt);
		}
		return OMX_ErrorNone;
	}

	default:
	{
		return defaultBehavior(evt);
	}
	}
}

ImgEns_Port_Fsm * ImgEns_Component_Fsm::getPortFsm(const OMX_U32 portIdx) const
{
	if (mEnsComponent.getPortCount() <= portIdx || mEnsComponent.getPort(portIdx) == 0)
	{
		return 0;
	}
	return mEnsComponent.getPort(portIdx)->getPortFsm();
}

ImgEns_Port* ImgEns_Component_Fsm::getPort(const OMX_U32 portIdx) const
{
	if (mEnsComponent.getPortCount() <= portIdx || mEnsComponent.getPort(portIdx) == 0)
	{
		return NULL;
	}
	return mEnsComponent.getPort(portIdx);
}

OMX_ERRORTYPE ImgEns_Component_Fsm::DispatchToPort(const OMX_U32 portIdx, const ImgEns_FsmEvent*evt) const
{
	ImgEns_Port *pPort = getPort(portIdx);
	if (pPort != NULL)
	{
		return (pPort->Dispatch(evt));
	}
	else
	{
		return OMX_ErrorBadPortIndex;
	}
}

OMX_ERRORTYPE ImgEns_Component_Fsm::Dispatch(const ImgEns_FsmEvent*evt)
{ //Override with omx return type
	int status = this->dispatch(evt);
	if (status == ImgEns_Fsm::eError_IncorrectStateOperation)
		status = OMX_ErrorIncorrectStateOperation; //translate to right OMX error index
	return (OMX_ERRORTYPE) status;
}

OMX_BOOL ImgEns_Component_Fsm::isPortEnabled(const OMX_U32 portidx) const
{
	return mEnsComponent.getPort(portidx)->isEnabled();
}


inline OMX_STATETYPE ImgEns_Component_Fsm::portGetState(const OMX_U32 portIdx) const
{
	if (mEnsComponent.getPortCount() <= portIdx || mEnsComponent.getPort(portIdx) == 0)
	{
		return OMX_StateInvalid;
	}
	ImgEns_Port_Fsm *portFsm = (mEnsComponent.getPort(portIdx))->getPortFsm();
	return portFsm->getState();
}
