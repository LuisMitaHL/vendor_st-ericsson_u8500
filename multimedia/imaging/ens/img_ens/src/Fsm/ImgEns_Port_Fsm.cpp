/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#include "ImgEns_Shared.h"
#include "osi_toolbox_lib.h"
#include "ImgEns_Fsm.h"
#include "ImgEns_Trace.h"
#include "ImgEns_Port.h"
#include "ImgEns_Port_Fsm.h"
#include "ImgEns_ProcessingComponent.h"

#define OMXCOMPONENT "IMGENS_PORT_FSM"

#ifndef _MSC_VER
	#include "osi_trace.h"
	//#include "OstTraceDefinitions_ste.h"
	#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
		#include "ens_proxy_ENS_Fsm_ENS_Port_FsmTraces.h"
	#endif //OST_TRACE_COMPILER_IN_USE
#else
#endif

#undef MAX
#define MAX(a,b) (((a)<(b))?(b):(a))

const s_FsmStateDescription FsmStateDescription_Invalid={ NULL, "Unknow" , OMX_StateInvalid, OMX_StateInvalid};

const s_FsmStateDescription ImgEns_Port_Fsm::TableFsmStatDescription[]=
{
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateLoaded                                   , "fsmStateLoaded"                                   , OMX_StateLoaded                  , OMX_StateLoaded                  },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateLoadedToIdleHOST                         , "fsmStateLoadedToIdleHOST"                         , OMX_StateLoadedToIdleHOST        , OMX_StateLoadedToIdleHOST        },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateLoadedToIdleDSP                          , "fsmStateLoadedToIdleDSP"                          , OMX_StateLoadedToIdleDSP         , OMX_StateLoadedToIdleDSP         },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateCheckRscAvailability                     , "fsmStateCheckRscAvailability"                     , OMX_StateCheckRscAvailability    , OMX_StateCheckRscAvailability    },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateLoadedToTestForResources                 , "fsmStateLoadedToTestForResources"                 , OMX_StateLoadedToTestForResources, OMX_StateLoadedToTestForResources},
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateTestForResources                         , "fsmStateTestForResources"                         , OMX_StateTestForResources        , OMX_StateTestForResources        },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateWaitForResources                         , "fsmStateWaitForResources"                         , OMX_StateWaitForResources        , OMX_StateWaitForResources        },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateIdle                                     , "fsmStateIdle"                                     , OMX_StateIdle                    , OMX_StateIdle                    },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateIdleToLoaded                             , "fsmStateIdleToLoaded"                             , OMX_StateIdleToLoaded            , OMX_StateIdleToLoaded            },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateExecuting                                , "fsmStateExecuting"                                , OMX_StateExecuting               , OMX_StateExecuting               },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStatePause                                    , "fsmStatePause"                                    , OMX_StatePause                   , OMX_StatePause                   },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateTransient                                , "fsmStateTransient"                                , OMX_StateTransient               , OMX_StateTransient               },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateTransientToIdle                          , "fsmStateTransientToIdle"                          , OMX_StateTransient               , OMX_StateTransient               },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateTransientToDisableWaitBuffersFromNeighbor, "fsmStateTransientToDisableWaitBuffersFromNeighbor", eStateDisabled                   , eStateDisabled                   },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateTransientToDisableWaitForFreeBuffers     , "fsmStateTransientToDisableWaitForFreeBuffers"     , eStateDisabled                   , eStateDisabled                   },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP                    , "fsmStateTransientToDisableDSP"                    , eStateDisabled                   , eStateDisabled                   },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateTransientToEnableHOST                    , "fsmStateTransientToEnableHOST"                    , eStateDisabled                   , eStateDisabled                   },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateTransientToEnableDSP                     , "fsmStateTransientToEnableDSP"                     , eStateDisabled                   , eStateDisabled                   },
	{ (ImgEns_Fsm::_fnState)&ImgEns_Port_Fsm::fsmStateDisabled                                 , "fsmStateDisabled"                                 , eStateDisabled                   , eStateDisabled                   },
};


ImgEns_Port_Fsm::ImgEns_Port_Fsm(ImgEns_Port &port)
: mPort(port)
{
	mDisabledState            = OMX_StateInvalid;
	mWaitBuffersFromNeighbor  = OMX_FALSE;
	mWaitBuffersToNeighbor    = OMX_FALSE;
	mEventAppliedOnEachPorts  = OMX_FALSE;
	init(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
	m_BufferSharingEnabled = true;
}


OMX_ERRORTYPE ImgEns_Port_Fsm::eventHandlerCB(OMX_EVENTTYPE ev, OMX_U32 data1, OMX_U32 data2)
{
	ImgEns_Component &enscomp = mPort.getENSComponent();
	return enscomp.eventHandlerCB(ev, data1, data2, 0);
}


int ImgEns_Port_Fsm::NotifyChangeState(const _fnState currentState, const _fnState nextState) const
//*************************************************************************************************************
{
	const s_FsmStateDescription &_current= GetStateDescription(currentState);
	const s_FsmStateDescription &_next   = GetStateDescription(nextState);
	OstTraceFiltInst4(TRACE_FLOW, "%s.port[%d]: fsm transition from  '%s' to '%s'", mPort.getENSComponent().getName(), mPort.getPortIndex(), _current.Name, _next.Name);
	return(0);
}


OMX_STATETYPE ImgEns_Port_Fsm::getState(void) const
{
	/*const*/ _fnStatePort state =(_fnStatePort)getFsmState();
	if (state == &ImgEns_Port_Fsm::fsmStateLoaded) 
	{
		return OMX_StateLoaded;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStateLoadedToIdleHOST)
	{
		return (OMX_STATETYPE)OMX_StateLoadedToIdleHOST;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStateLoadedToIdleDSP)
	{
		return  (OMX_STATETYPE)OMX_StateLoadedToIdleDSP;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStateCheckRscAvailability)
	{
		return  (OMX_STATETYPE)OMX_StateCheckRscAvailability;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStateLoadedToTestForResources)
	{
		return  (OMX_STATETYPE)OMX_StateLoadedToTestForResources;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStateTestForResources)
	{
		return  OMX_StateTestForResources;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStateWaitForResources)
	{
		return OMX_StateWaitForResources;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStateIdle)
	{
		return OMX_StateIdle;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStateIdleToLoaded)
	{
		return  (OMX_STATETYPE)OMX_StateIdleToLoaded;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStateExecuting)
	{
		return OMX_StateExecuting;
	} 
	else if (state== &ImgEns_Port_Fsm::fsmStatePause)
	{
		return OMX_StatePause;
	} 
	else if (  state== &ImgEns_Port_Fsm::fsmStateTransient
			|| state== &ImgEns_Port_Fsm::fsmStateTransientToIdle)
	{
		return  (OMX_STATETYPE)OMX_StateTransient;
	} else if (state== &ImgEns_Port_Fsm::fsmStateTransientToDisableWaitBuffersFromNeighbor
			|| state== &ImgEns_Port_Fsm::fsmStateTransientToDisableWaitForFreeBuffers
			|| state== &ImgEns_Port_Fsm::fsmStateTransientToDisableDSP
			|| state== &ImgEns_Port_Fsm::fsmStateTransientToEnableHOST
			|| state== &ImgEns_Port_Fsm::fsmStateTransientToEnableDSP
			|| state== &ImgEns_Port_Fsm::fsmStateDisabled)
	{
		return mDisabledState;
	} 
	else 
	{
		return OMX_StateInvalid;
	}
}

ImgEns_Fsm::_fnState ImgEns_Port_Fsm::getFsmParentState(OMX_STATETYPE parentState)
{
	if (parentState == OMX_StateLoaded) 
	{
		return static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded);
	} 
	else if (parentState == OMX_StateIdle) 
	{
		return static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle);
	} 
	else if (parentState == OMX_StateExecuting) 
	{
		return static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateExecuting);
	} 
	else if (parentState == OMX_StatePause) 
	{
		return static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStatePause);
	} 
	else {
		return static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateInvalid);
	}
}

void ImgEns_Port_Fsm::emptybufferdone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	MSG3("emptybufferdone_cb received from Img_ProcessingComponent : h=0x%p portIdx=%lu pBuffer=0x%p\n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex(), pBuffer);
	ImgEns_CmdBufferHdrEvt evt(OMX_EMPTY_BUFFER_DONE_SIG, pBuffer);
	dispatch(&evt);
}

void ImgEns_Port_Fsm::fillbufferdone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	MSG3("fillbufferdone_cb received from Img_ProcessingComponent : h=0x%p portIdx=%lu pBuffer=0x%p\n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex(), pBuffer);
	OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: fillbufferdone_cb received pBuffer=0x%p", (unsigned int)pBuffer);
	ImgEns_CmdBufferHdrEvt evt(OMX_FILL_BUFFER_DONE_SIG, pBuffer);
	dispatch(&evt);
}

Img_ProcessingComponent & ImgEns_Port_Fsm::getProcessingComponent() const
{
	return mPort.getENSComponent().getProcessingComponent();
}

OMX_ERRORTYPE ImgEns_Port_Fsm::checkStandardTunnelingInit(void)
{
	if (!m_BufferSharingEnabled) 
	{
		if (!mPort.getTunneledComponent() || !mPort.useStandardTunneling() || !mPort.isBufferSupplier())
		{
			return OMX_ErrorNone;
		}
	} 
	else 
	{
		if (!mPort.getTunneledComponent() || !mPort.useStandardTunneling() || !mPort.isBufferSupplier() || !mPort.isAllocatorPort())
		{
			return OMX_ErrorNone;
		}
	}
	return mPort.standardTunnelingInit();
}

OMX_ERRORTYPE ImgEns_Port_Fsm::checkStandardTunnelingDeInit(void)
{
	if (!mPort.getTunneledComponent() || !mPort.useStandardTunneling() || !mPort.isBufferSupplier())
	{
		return OMX_ErrorNone;
	}

	if (!m_BufferSharingEnabled)
	{
		return mPort.standardTunnelingDeInit();
	}
	else
	{
		// Case of a Standard tunneling buffer supplier
		if (mPort.isAllocatorPort()) 
		{
			return mPort.standardTunnelingDeInit();
		} 
		else 
		{
			return mPort.bufferSharingDeinit();
		}
	}
}

OMX_ERRORTYPE ImgEns_Port_Fsm::goLoadedToIdle(void)
{
	// Case proprietary tunneling or std tunneling buffer supplier
	if (mPort.useProprietaryCommunication() || (mPort.getTunneledComponent() && mPort.useStandardTunneling() && mPort.isBufferSupplier())) 
	{
		stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateCheckRscAvailability));
	} 
	else 
	{
		if (mPort.isPopulated()) 
		{
			if (mPort.getTunneledComponent() != (OMX_HANDLETYPE) NULL) 
			{
				stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateCheckRscAvailability));
			} 
			else 
			{
				return OMX_ErrorIncorrectStateOperation;
			}
		} 
		else 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoadedToIdleHOST));
		}
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port_Fsm::goIdleToLoaded(void) 
{
	OMX_ERRORTYPE error;

	//Check if we need to reset port for standard tunneling
	error = checkStandardTunnelingDeInit();
	if (error != OMX_ErrorNone) 
	{
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: goIdleToLoaded error: %d", error);
		return error;
	}
	if (mPort.useProprietaryCommunication() || !mPort.isPopulated()) 
	{
		stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
	} 
	else 
	{
		if (mPort.getBufferCountCurrent() == 0) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
		} 
		else 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdleToLoaded));
		}
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port_Fsm::goToDisable(OMX_STATETYPE currentState, OMX_U32 portIndex) 
{
	mDisabledState = currentState;
	if (mDisabledState == OMX_StateLoaded) 
	{
		mPort.setEnabledFlag(OMX_FALSE);
		//send eIMGOMX_EventCmdReceived to ENSWrapper : in loaded state -> processingComponent not created yet.
		eventHandlerCB((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandPortDisable, mPort.getPortIndex());
		stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateDisabled));
		return OMX_ErrorNone;
	}

	//if standard/proprietary tunneling, check that non-buffer supplier port of tunnel is
	//disabled after buffer supplier port.
	if (mPort.getTunneledComponent() && !mPort.isBufferSupplier()) 
	{
		OMX_ERRORTYPE error;
		OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
		tunneledPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		tunneledPortDef.nPortIndex = mPort.getTunneledPort();
		getImgOmxIlSpecVersion(&tunneledPortDef.nVersion);
		error = OMX_GetParameter(mPort.getTunneledComponent(), OMX_IndexParamPortDefinition, &tunneledPortDef);
		if (error != OMX_ErrorNone) 
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: goToDisable error: %d", error);
			return error;
		}
		if (tunneledPortDef.bEnabled == OMX_TRUE) 
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: goToDisable error: incorrectStateOperation");
			return OMX_ErrorIncorrectStateOperation;
		}
	}
	mPort.setEnabledFlag(OMX_FALSE);

	if (portIndex == OMX_ALL) 
	{
		mEventAppliedOnEachPorts = OMX_TRUE;
	}

	//If standard tunneling or non-tunneled communication : send eIMGOMX_EventCmdReceived to ENSWrapper
	//--> OMX_SendCommand can not be locked. In current implementation, processingcomponent will not
	//received the sendCommand before all buffers returned to supplier OMX component.
	if (!mPort.useProprietaryCommunication())
	{
		eventHandlerCB((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandPortDisable, mPort.getPortIndex());
	}

	if (!m_BufferSharingEnabled)
	{
		if (mPort.useStandardTunneling() && mPort.isBufferSupplier() && !mPort.isAllBuffersReturnedFromNeighbor())
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableWaitBuffersFromNeighbor));
		}
		else
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
		}
	}
	else
	{
		if (mPort.useStandardTunneling() && mPort.isBufferSupplier())
		{
			if (mPort.isAllocatorPort())
			{
				if (!mPort.isAllBuffersReturnedFromNeighbor()
						|| !mPort.isAllBuffersReturnedFromSharingPorts())
				{
					stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableWaitBuffersFromNeighbor));
				} else
					stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
			}

			if (mPort.isSharingPort())
			{
				if (!mPort.isAllBuffersReturnedFromNeighbor())
				{
					stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableWaitBuffersFromNeighbor));
				}
				else
					stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
			}
		}
		else
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
		}
	}

	// Check if eSRAM estimation has changed after disabling
	if (mPort.getENSComponent().omxState() == OMX_StateLoaded)
	{
		return OMX_ErrorNone;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port_Fsm::goToEnable(OMX_U32 portIndex)
{
	OMX_ERRORTYPE error;

	if (mDisabledState == OMX_StateLoaded)
	{
		mPort.setEnabledFlag(OMX_TRUE);
		//send eIMGOMX_EventCmdReceived to ENSWrapper : in loaded state -> processingComponent not created yet.
		eventHandlerCB((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandPortEnable, mPort.getPortIndex());
		stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
		mDisabledState = OMX_StateInvalid;
		return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandPortEnable, mPort.getPortIndex());
	}

	//if standard/proprietary tunneling, check that buffer supplier port of tunnel is
	//enabled after buffer supplier port.
	if (mPort.getTunneledComponent() && mPort.isBufferSupplier())
	{
		OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
		tunneledPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		tunneledPortDef.nPortIndex = mPort.getTunneledPort();
		getImgOmxIlSpecVersion(&tunneledPortDef.nVersion);
		error = OMX_GetParameter(mPort.getTunneledComponent(), OMX_IndexParamPortDefinition, &tunneledPortDef);
		if (error != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: goToEnable error: %d", error);
			return error;
		}
		if (tunneledPortDef.bEnabled == OMX_FALSE)
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: goToEnable error: incorrectStateOperation");
			return OMX_ErrorIncorrectStateOperation;
		}
	}

	mPort.setEnabledFlag(OMX_TRUE);

	if (portIndex == OMX_ALL)
	{
		mEventAppliedOnEachPorts = OMX_TRUE;
	}

	//Check if we need to initialize port for standard tunneling
	error = checkStandardTunnelingInit();
	if (error != OMX_ErrorNone)
	{
		stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateInvalid));
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: goToDisable error: bad standard tunneling init %d", error);
		return OMX_ErrorNone; // do not return the error, otherwise the transition is not taken into account
	}

	//If standard tunneling or non-tunneled communication : send eIMGOMX_EventCmdReceived to ENSWrapper
	//--> OMX_SendCommand can not be locked. In current implementation, processingcomponent will not
	//received the sendCommand before reception of OMX_UseBuffer/OMX_AllocateBuffer.
	if (!mPort.useProprietaryCommunication())
	{
		eventHandlerCB((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandPortEnable, mPort.getPortIndex());
	}

	if (mPort.useProprietaryCommunication() || mPort.isPopulated())
	{
		stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToEnableDSP));
	} 
	else 
	{
		stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToEnableHOST));
	}

	// Check if eSRAM estimation has changed after enabling
	if (mPort.getENSComponent().omxState() == OMX_StateLoaded)
	{
		return OMX_ErrorNone;
	}
	return OMX_ErrorNone;
}

//-----------------------------------------------------------------------------
//! \brief Default Behavior for all states of the port FSM
//!
//! This method implements default behavior for all states of the port FSM.
//! It is intended to be called in the default case of each state handler method.
//! State handlers should handle events only when the behavior is different.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------

OMX_ERRORTYPE ImgEns_Port_Fsm::defaultBehavior(const ImgEns_FsmEvent *evt)
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
		return mPort.getParameter(getParamEvt->getIndex(), getParamEvt->getStructPtr());
	}

	case OMX_UPDATE_PORT_SETTINGS_SIG:
	{
		OMX_PARAM_PORTDEFINITIONTYPE old;

		ImgEns_CmdUpdateSettingsEvt *updateSettingsEvt = (ImgEns_CmdUpdateSettingsEvt *) evt;
		if (updateSettingsEvt->getIndex() == OMX_IndexParamPortDefinition)
		{
			old = mPort.getValueParamPortDefinition();
		}
		OMX_ERRORTYPE error = mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
		if (error != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: defaultBehavior UPDATE_PORT_SETTING_SIG error: %d", error);
			return error;
		}

		if ((updateSettingsEvt->getIndex() != OMX_IndexParamPortDefinition) || mPort.mustSendEventHandler(&old))
		{
			return eventHandlerCB(OMX_EventPortSettingsChanged, mPort.getPortIndex(), updateSettingsEvt->getIndex());
		}
		else
		{
			return OMX_ErrorNone;
		}
	}

	case OMX_EMPTY_THIS_BUFFER_SIG:
	{
		ImgEns_CmdEmptyThisBufferEvt *emptyThisBufferEvt =(ImgEns_CmdEmptyThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =emptyThisBufferEvt->getOMXBufferHdrPtr();
		OMX_ERRORTYPE err = OMX_ErrorNone;

		mPort.bufferReturnedFromNeighbor(pOMXBuffer);
		mPort.bufferSentToProcessingComponent(pOMXBuffer);

		err = getProcessingComponent().emptyThisBuffer(pOMXBuffer);
		if (err != OMX_ErrorNone)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: defaultBehavior EMPTY_THIS_BUFFER_SIG error: %d", err);
			mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
			mPort.bufferSentToNeighbor(pOMXBuffer);
		}
		return err;
	}
	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
	{
		ImgEns_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt =(ImgEns_CmdEmptyThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;

		IMGENS_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
		copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);

		mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
		mPort.bufferSentToNeighbor(pMappedOMXBuffer);

		if (!mPort.getTunneledComponent())
		{
			mPort.getENSComponent().fillBufferDoneCB(pMappedOMXBuffer);
		}
		else
		{
			OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
		}
		return OMX_ErrorNone;
	}
	case OMX_FILL_THIS_BUFFER_SIG:
	{
		ImgEns_CmdFillThisBufferEvt *fillThisBufferEvt = (ImgEns_CmdFillThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisBufferEvt->getOMXBufferHdrPtr();
		OMX_ERRORTYPE err = OMX_ErrorNone;

		mPort.bufferReturnedFromNeighbor(pOMXBuffer);
		OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: defaultBehavior OMX_FILL_THIS_BUFFER_SIG 0x%x", (unsigned int)pOMXBuffer);
		if (!m_BufferSharingEnabled) 
		{
			mPort.bufferSentToProcessingComponent(pOMXBuffer);

			err = getProcessingComponent().fillThisBuffer(pOMXBuffer);
			if (err != OMX_ErrorNone) 
			{
				OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: defaultBehavior FILL_THIS_BUFFER_SIG error: %d", err);
				mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
				mPort.bufferSentToNeighbor(pOMXBuffer);
			}
		} 
		else 
		{
			if (mPort.getNumberInterConnectedPorts())
			{
				err = mPort.forwardOutputBuffer(pOMXBuffer);
				if (err != OMX_ErrorNone)
					return err;
				mPort.bufferSentToSharingPort(pOMXBuffer);
			} 
			else 
			{
				mPort.bufferSentToProcessingComponent(pOMXBuffer);
				err = getProcessingComponent().fillThisBuffer(pOMXBuffer);
				if (err != OMX_ErrorNone) 
				{
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: defaultBehavior FILL_THIS_BUFFER_SIG error: %d", err);
					mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
				}
			}
		}
		return err;
	}
	case OMX_FILL_THIS_SHARED_BUFFER_SIG: 
	{
		ImgEns_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt = (ImgEns_CmdFillThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
		OMX_BOOL sendBufferToNeighbor;
		IMGENS_ASSERT(OMX_ErrorNone== mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
		sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);

		if (sendBufferToNeighbor) 
		{
			copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);
			mPort.bufferSentToNeighbor(pMappedOMXBuffer);
			if (!mPort.getTunneledComponent()) 
			{
				mPort.getENSComponent().emptyBufferDoneCB(pMappedOMXBuffer);
			} 
			else 
			{
				OMX_FillThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
			}
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_BUFFER_DONE_SIG:
	{
		ImgEns_CmdBufferHdrEvt *emptyBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();

		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);

		if (!m_BufferSharingEnabled) 
		{
			mPort.bufferSentToNeighbor(pOMXBuffer);

			if (!mPort.getTunneledComponent()) 
			{
				mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
			} 
			else 
			{
				OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
			}
		} 
		else 
		{
			if (mPort.getNumberInterConnectedPorts()) 
			{
				OMX_ERRORTYPE error = mPort.forwardInputBuffer(pOMXBuffer);
				if (error != OMX_ErrorNone)
					return error;
				mPort.bufferSentToSharingPort(pOMXBuffer);
			} 
			else 
			{
				mPort.bufferSentToNeighbor(pOMXBuffer);
				if (!mPort.getTunneledComponent()) 
				{
					mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
				} 
				else 
				{
					OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
				}
			}
		}
		return OMX_ErrorNone;
	}
	case OMX_FILL_BUFFER_DONE_SIG: 
	{
		ImgEns_CmdBufferHdrEvt *fillBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();

		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
		mPort.bufferSentToNeighbor(pOMXBuffer);

		if (!mPort.getTunneledComponent()) 
		{
			mPort.getENSComponent().fillBufferDoneCB(pOMXBuffer);
		} 
		else 
		{
			OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
		}
		return OMX_ErrorNone;
	}
	case OMX_FREE_BUFFER_SIG: 
	{
		ImgEns_CmdFreeBufferEvt *freeBufferEvt = (ImgEns_CmdFreeBufferEvt *) evt;
		OMX_ERRORTYPE error = mPort.freeBuffer(freeBufferEvt->getOMXBufferHdrPtr());
		if (error != OMX_ErrorNone) 
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: defaultBehavior FREE_BUFFER_SIG error: %d", error);
			return error;
		}
		return eventHandlerCB(OMX_EventError, (t_uint32) OMX_ErrorPortUnpopulated, mPort.getPortIndex());
	}
	case OMX_ENABLE_PORT_SIG:
	case OMX_DISABLE_PORT_SIG:
	case OMX_FLUSH_PORT_SIG:
		OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: Enable / Disable / Flush: incorrect state operation");
		return eventHandlerCB(OMX_EventError, (t_uint32) OMX_ErrorSameState, mPort.getPortIndex());
	default:
		OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: defaultBehavior error: incorrect state operation");
		return OMX_ErrorIncorrectStateOperation;
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is in OMX Loaded state
//!
//! This method implements the behavior of the port FSM when it is in Loaded state.
//! When the port FSM is in this state, it means that all tunnel buffers have been
//! freed.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateLoaded(const ImgEns_FsmEvent *evt) {
	switch (evt->signal()) {

	case eSignal_Entry: 
	{
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG: {
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		OMX_STATETYPE targetState = setStateEvt->targetState();

		if (targetState == OMX_StateIdle) {
			OMX_ERRORTYPE err = goLoadedToIdle();
			return err;
		} else if (targetState == OMX_StateTestForResources) {
			stateTransition(
					static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoadedToTestForResources));
			return OMX_ErrorNone;
		} else if (targetState == OMX_StateWaitForResources) {
			stateTransition(
					static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateWaitForResources));
			return OMX_ErrorNone;
		} else if (targetState == OMX_StateLoaded) {
			OstTraceFiltInst0(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoaded SETSTATE_SIG error: same state");
			return OMX_ErrorSameState;
		} else {
			OstTraceFiltInst0(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoaded SETSTATE_SIG error: incorrect state transition");
			return OMX_ErrorIncorrectStateTransition;
		}
	}

	case OMX_SETPARAMETER_SIG: {
		ImgEns_CmdSetParamEvt *setParamEvt = (ImgEns_CmdSetParamEvt *) evt;
		return mPort.setParameter(setParamEvt->getIndex(),
				setParamEvt->getStructPtr());
	}

	case OMX_UPDATE_PORT_SETTINGS_SIG: {
		// no callback
		ImgEns_CmdUpdateSettingsEvt *updateSettingsEvt =
				(ImgEns_CmdUpdateSettingsEvt *) evt;
		return mPort.setParameter(updateSettingsEvt->getIndex(),
				updateSettingsEvt->getStructPtr());
	}

	case OMX_DISABLE_PORT_SIG:
	{
		ImgEns_CmdPortEvt *portEvt = (ImgEns_CmdPortEvt *) evt;
		return goToDisable(OMX_StateLoaded, portEvt->getPortIdx());
	}

	case OMX_COMP_TUNNEL_REQUEST_SIG:
	{
		ImgEns_CmdTunnelRequestEvt *tunnelReqEvt = (ImgEns_CmdTunnelRequestEvt *) evt;
		OMX_ERRORTYPE err = mPort.tunnelRequest(
				tunnelReqEvt->getTunneledCompHdl(),
				tunnelReqEvt->getTunneledPortIdx(),
				tunnelReqEvt->getTunnelStructPtr());
		if (OMX_ErrorNone != err)
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoaded COMP_TUNNEL_REQUEST_SIG error: %d", err);
			return err;
		}
#ifdef USE_RME
		// Notify the RM if port is an output port OR standard Tunneled
		if (mPort.getDirection() == OMX_DirOutput)
		{
			ImgEns_HwRm::HWRM_Notify_Tunnel(mPort.getComponent(),
					mPort.getPortIndex(), tunnelReqEvt->getTunneledCompHdl(),
					tunnelReqEvt->getTunneledPortIdx());
		}
		else if (mPort.useStandardTunneling())
		{
			ImgEns_HwRm::HWRM_Notify_Tunnel(tunnelReqEvt->getTunneledCompHdl(),
					tunnelReqEvt->getTunneledPortIdx(), mPort.getComponent(),
					mPort.getPortIndex());
		}
#endif
		return OMX_ErrorNone;
	}

	case OMX_FLUSH_PORT_SIG:
		return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandFlush, mPort.getPortIndex());

	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_FILL_THIS_SHARED_BUFFER_SIG:
	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoaded error: incorrect state operation %d",
				(unsigned int)evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default: {
		return defaultBehavior(evt);
	}
	}

}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is in OMX Idle state
//!
//! This method implements the behavior of the port FSM when it is in Idle state.
//! When the port FSM is in this state, it means that the port is populated and
//! the processing component has been instantiated.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateIdle(const ImgEns_FsmEvent *evt) 
{

	switch (evt->signal()) 
	{

	case eSignal_Entry:
		{
		OstTraceFiltInst1(TRACE_FLOW,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateIdle eSignal_Entry: isAllBuffersAtOwner = %d",
				mPort.isAllBuffersAtOwner());
		IMGENS_ASSERT(mPort.isAllBuffersAtOwner());

		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG: 
	{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		OMX_STATETYPE targetState = setStateEvt->targetState();

		if (targetState == OMX_StateLoaded) {
			return goIdleToLoaded();
		} else if (targetState == OMX_StateExecuting
				|| targetState == OMX_StatePause) {
			stateTransition(
					static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransient));
			return OMX_ErrorNone;
		} else if (targetState == OMX_StateIdle) {
			OstTraceFiltInst0(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateIdle SETSTATE_SIG error: same state");
			return OMX_ErrorSameState;
		} else {
			OstTraceFiltInst0(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateIdle SETSTATE_SIG error: incorrect state transition");
			return OMX_ErrorIncorrectStateTransition;
		}
	}

	case OMX_DISABLE_PORT_SIG: {
		ImgEns_CmdPortEvt *portEvt = (ImgEns_CmdPortEvt *) evt;
		return goToDisable(OMX_StateIdle, portEvt->getPortIdx());
	}

	case OMX_FLUSH_PORT_SIG: {
		ImgEns_CmdPortEvt *portEvt = (ImgEns_CmdPortEvt *) evt;
		if (portEvt->getPortIdx() != OMX_ALL) {
			return getProcessingComponent().sendCommand(OMX_CommandFlush,
					mPort.getPortIndex());
		} else if (mPort.getPortIndex()
				== (mPort.getENSComponent().getPortCount() - 1)) {
			return getProcessingComponent().sendCommand(OMX_CommandFlush,
					OMX_ALL);
		}
		return OMX_ErrorNone;
	}

	case OMX_FLUSHPORTCOMPLETE_SIG:
		return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandFlush,
				mPort.getPortIndex());

	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FILL_THIS_SHARED_BUFFER_SIG:
	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateIdle error: incorrect state operation %d",
				(unsigned int)evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default: {
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is in OMX Executing state
//!
//! This method implements the behavior of the port FSM when it is in Executing state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateExecuting(const ImgEns_FsmEvent *evt) {

	switch (evt->signal()) 
	{

	case eSignal_Entry: 
	{
		if (!mPort.useStandardTunneling())
			return OMX_ErrorNone;
		return mPort.standardTunnelingTransfertInit();
	}

	case OMX_SETSTATE_SIG: 
	{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		OMX_STATETYPE targetState = setStateEvt->targetState();
		if (targetState == OMX_StateIdle) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToIdle));
			return OMX_ErrorNone;
		} 
		else if (targetState == OMX_StatePause) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransient));
			return OMX_ErrorNone;
		} 
		else if (targetState == OMX_StateExecuting) 
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateExecuting SETSTATE_SIG error: same state");
			return OMX_ErrorSameState;
		} 
		else 
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateExecuting SETSTATE_SIG error: incorrect state transition");
			return OMX_ErrorIncorrectStateTransition;
		}
	}
	case OMX_DISABLE_PORT_SIG: 
	{
		ImgEns_CmdPortEvt *portEvt = (ImgEns_CmdPortEvt *) evt;
		return goToDisable(OMX_StateExecuting, portEvt->getPortIdx());
	}
	case OMX_FLUSH_PORT_SIG: 
	{
		ImgEns_CmdPortEvt *portEvt = (ImgEns_CmdPortEvt *) evt;
		if (portEvt->getPortIdx() != OMX_ALL) 
		{
			return getProcessingComponent().sendCommand(OMX_CommandFlush, mPort.getPortIndex());
		} 
		else if (mPort.getPortIndex()== (mPort.getENSComponent().getPortCount() - 1)) 
		{
			return getProcessingComponent().sendCommand(OMX_CommandFlush, OMX_ALL);
		}
		return OMX_ErrorNone;
	}
	case OMX_FLUSHPORTCOMPLETE_SIG:
		return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandFlush, mPort.getPortIndex());
	default: 
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is in OMX Pause state
//!
//! This method implements the behavior of the port FSM when it is in Pause state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStatePause(const ImgEns_FsmEvent *evt) {

	switch (evt->signal()) {

	case eSignal_Entry: {
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG: {
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		OMX_STATETYPE targetState = setStateEvt->targetState();

		if (targetState == OMX_StateIdle) {
			stateTransition(
					static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToIdle));
			return OMX_ErrorNone;
		} else if (targetState == OMX_StateExecuting) {
			stateTransition(
					static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransient));
			return OMX_ErrorNone;
		} else if (targetState == OMX_StatePause) {
			OstTraceFiltInst0(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStatePause SETSTATE_SIG error: same state");
			return OMX_ErrorSameState;
		} else {
			OstTraceFiltInst0(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStatePause SETSTATE_SIG error: incorrect state transition");
			return OMX_ErrorIncorrectStateTransition;
		}
	}

	case OMX_DISABLE_PORT_SIG: {
		ImgEns_CmdPortEvt *portEvt = (ImgEns_CmdPortEvt *) evt;
		return goToDisable(OMX_StatePause, portEvt->getPortIdx());
	}

	case OMX_FLUSH_PORT_SIG: {
		ImgEns_CmdPortEvt *portEvt = (ImgEns_CmdPortEvt *) evt;
		if (portEvt->getPortIdx() != OMX_ALL) {
			return getProcessingComponent().sendCommand(OMX_CommandFlush,
					mPort.getPortIndex());
		} else if (mPort.getPortIndex()
				== (mPort.getENSComponent().getPortCount() - 1)) {
			return getProcessingComponent().sendCommand(OMX_CommandFlush,
					OMX_ALL);
		}
		return OMX_ErrorNone;
	}

	case OMX_FLUSHPORTCOMPLETE_SIG:
		return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandFlush,
				mPort.getPortIndex());

	default: {
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is transitionning from Loaded to Idle
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! from Loaded to Idle. This transient state waits for calls
//! to OMX_AllocateBuffer or OMX_UseBuffer until the port is populated.
//! It then transitions to OmxStateLoadedToIdleDsp.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateLoadedToIdleHOST(const ImgEns_FsmEvent *evt) {

	OMX_ERRORTYPE error;
	switch (evt->signal()) 
	{
	case eSignal_Entry: 
	{
		// From ImgEns_Port_Fsm::goLoadedToIdle(), this state is not used when we are in standard tunneling and Buffer supplier
		// This is because the component generates itself the allocation of buffers, and does not receive
		// OMX_AllocateBuffer() from the application
		DBC_PRECONDITION(!mPort.useProprietaryCommunication() && !mPort.isBufferSupplier());
		return OMX_ErrorNone;
	}

	case OMX_SETPARAMETER_SIG: 
		{
		if (m_BufferSharingEnabled) 
		{
			OMX_ERRORTYPE error;
			ImgEns_CmdSetParamEvt *setParamEvt = (ImgEns_CmdSetParamEvt *) evt;
			// Only OMX_IndexParamPortDefinition index is supported in this state
			if (setParamEvt->getIndex() != OMX_IndexParamPortDefinition)
				return OMX_ErrorIncorrectStateOperation;

			error = mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
			if (error != OMX_ErrorNone)
				return error;

			error = mPort.sendBufferReqs(OMX_IndexParamPortDefinition, setParamEvt->getStructPtr());
			if (error != OMX_ErrorNone)
				return error;
			return OMX_ErrorNone;
		} 
		else 
		{
			ImgEns_CmdSetParamEvt *setParamEvt = (ImgEns_CmdSetParamEvt *) evt;
			// Only OMX_IndexParamPortDefinition index is supported in this state
			if (setParamEvt->getIndex() != OMX_IndexParamPortDefinition)
				return OMX_ErrorIncorrectStateOperation;
			return mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
		}
	}
	case OMX_USE_BUFFER_SIG: 
		{
		ImgEns_CmdUseBufferEvt *useBufferEvt = (ImgEns_CmdUseBufferEvt *) evt;

		error = mPort.useBuffer(useBufferEvt->getOMXBufferHdrPtr(), useBufferEvt->getAppPrivate(), useBufferEvt->getSizeBytes(), useBufferEvt->getBufferPtr());
		if (error != OMX_ErrorNone) 
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoadedToIdleHOST USE_BUFFER_SIG error: %d", error);
			return error;
		}
		if (mPort.isPopulated()) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateCheckRscAvailability));
		}
		return OMX_ErrorNone;
	}

	case OMX_ALLOCATE_BUFFER_SIG: 
	{
		ImgEns_CmdAllocBufferEvt *allocBufferEvt = (ImgEns_CmdAllocBufferEvt *) evt;

		error = mPort.allocateBuffer(allocBufferEvt->getOMXBufferHdrPtr(), allocBufferEvt->getAppPrivate(), allocBufferEvt->getSizeBytes());
		if (error != OMX_ErrorNone) 
		{
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoadedToIdleHOST ALLOCATE_BUFFER_SIG error: %d", error);
			return error;
		}
		if (mPort.isPopulated()) 
		{
			stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateCheckRscAvailability));
		}
		return OMX_ErrorNone;
	}

	case OMX_GETPARAMETER_SIG: 
		if (m_BufferSharingEnabled) 
		{
			OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
			OMX_ERRORTYPE error;
			ImgEns_CmdGetParamEvt *getParamEvt = (ImgEns_CmdGetParamEvt *) evt;
			OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
			error = mPort.getParameter(getParamEvt->getIndex(), pComponentParameterStructure);
			if (error != OMX_ErrorNone)
				return error;

			error = mPort.calculateBufferReqs(&maxBufferSize, &maxBufferCountActual);
			if (error != OMX_ErrorNone)
				return error;

			OMX_PARAM_PORTDEFINITIONTYPE *portdef = static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(pComponentParameterStructure);

			// Calculate self requirements
			maxBufferSize = MAX(portdef->nBufferSize, maxBufferSize);
			maxBufferCountActual = MAX(portdef->nBufferCountActual,maxBufferCountActual);

			portdef->nBufferSize = maxBufferSize;
			portdef->nBufferCountActual = maxBufferCountActual;

			return OMX_ErrorNone;
		} 
		else 
		{
			return defaultBehavior(evt);
		}
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_FILL_THIS_SHARED_BUFFER_SIG:
	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoadedToIdleHOST error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;
	default: 
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is transitionning from Loaded to Idle
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! from Loaded to Idle. This transient state waits for the command complete event
//! from the processing component. It then transitions to the OmxStateIdle state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateLoadedToIdleDSP(const ImgEns_FsmEvent *evt) 
{

	switch (evt->signal())
	{
	case eSignal_Entry:
	{
		return OMX_ErrorNone;
	}

	case OMX_SETSTATECOMPLETE_SIG:
	{
		ImgEns_CmdSetStateCompleteEvt *setStateCompleteEvt = (ImgEns_CmdSetStateCompleteEvt *) evt;
		OMX_STATETYPE newState = setStateCompleteEvt->newState();

		if (newState == OMX_StateIdle)
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}
		else
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateInvalid));
		}
		return OMX_ErrorNone;
	}

	case OMX_USE_SHARED_BUFFER_SIG:
	{
		IMGENS_ASSERT(mPort.getPortRole() == ImgEns_SHARING_PORT);
		// Sharing ports receives buffer from non supplier/allocator
		// Forward this buffer to tunneled port
		ImgEns_CmdUseSharedBufferEvt *useSharedBufferevt = (ImgEns_CmdUseSharedBufferEvt *) evt;
		return mPort.useSharedBuffer(useSharedBufferevt->getAppPrivate(),
				useSharedBufferevt->getSizeBytes(),
				useSharedBufferevt->getBufferPtr(),
				useSharedBufferevt->getSharedChunk());
	}

	case OMX_SETPARAMETER_SIG:
	{
		if (m_BufferSharingEnabled)
		{
			if (mPort.getPortRole() == ImgEns_SHARING_PORT)
			{
				// Its a sharing port
				OMX_ERRORTYPE error;
				ImgEns_CmdSetParamEvt *setParamEvt = (ImgEns_CmdSetParamEvt *) evt;
				// Only OMX_IndexParamPortDefinition index is supported in this state
				if (setParamEvt->getIndex() != OMX_IndexParamPortDefinition)
					return OMX_ErrorIncorrectStateOperation;

				error = mPort.setParameter(setParamEvt->getIndex(),
						setParamEvt->getStructPtr());
				if (error != OMX_ErrorNone)
					return error;

				error = mPort.sendBufferReqs(OMX_IndexParamPortDefinition,
						setParamEvt->getStructPtr());
				if (error != OMX_ErrorNone)
					return error;

				return OMX_ErrorNone;
			} else
				return OMX_ErrorIncorrectStateOperation;
		} else {
			return OMX_ErrorIncorrectStateOperation;
		}
	}

	case OMX_GETPARAMETER_SIG: {
		if (m_BufferSharingEnabled) {
			OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
			OMX_ERRORTYPE error;
			ImgEns_CmdGetParamEvt *getParamEvt = (ImgEns_CmdGetParamEvt *) evt;
			OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
			error = mPort.getParameter(getParamEvt->getIndex(),
					pComponentParameterStructure);
			if (error != OMX_ErrorNone)
				return error;

			error = mPort.calculateBufferReqs(&maxBufferSize,
					&maxBufferCountActual);
			if (error != OMX_ErrorNone)
				return error;

			OMX_PARAM_PORTDEFINITIONTYPE *portdef =
					static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(pComponentParameterStructure);

			// Calculate self requirements
			maxBufferSize = MAX(portdef->nBufferSize, maxBufferSize);
			maxBufferCountActual =
					MAX(portdef->nBufferCountActual,maxBufferCountActual);

			portdef->nBufferSize = maxBufferSize;
			portdef->nBufferCountActual = maxBufferCountActual;

			return OMX_ErrorNone;
		} else {
			return defaultBehavior(evt);
		}
	}

	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoadedToIdleDSP error: incorrect state operation %d",
				evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default: {
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is transitionning from Idle to Loaded
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! from Idle to Loaded. This transient states entered once the processing component
//! has been de-instantiated. It waits for calls to OMX_FreeBuffer for all buffer.
//! it then transitions to the OmxStateLoaded state
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateIdleToLoaded(const ImgEns_FsmEvent *evt) {

	OMX_ERRORTYPE error;

	switch (evt->signal()) {

	case eSignal_Entry: {
		return OMX_ErrorNone;
	}

	case OMX_UPDATE_PORT_SETTINGS_SIG: {
		// no callback
		ImgEns_CmdUpdateSettingsEvt *updateSettingsEvt =
				(ImgEns_CmdUpdateSettingsEvt *) evt;
		return mPort.setParameter(updateSettingsEvt->getIndex(),
				updateSettingsEvt->getStructPtr());
	}

	case OMX_FREE_BUFFER_SIG: {
		ImgEns_CmdFreeBufferEvt *freeBufferEvt = (ImgEns_CmdFreeBufferEvt *) evt;
		error = mPort.freeBuffer(freeBufferEvt->getOMXBufferHdrPtr());
		if (error != OMX_ErrorNone) {
			OstTraceFiltInst1(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateIdleToLoaded FREE_BUFFER_SIG error: %d",
					error);
			return error;
		}

		if (mPort.getBufferCountCurrent() == 0) {
			stateTransition(
					static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FILL_THIS_SHARED_BUFFER_SIG:
	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateIdleToLoaded error: incorrect state operation %d",
				evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default: {
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief transient state used when transitionning from Executing/Pause to Idle
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! from Executing or Pause to Idle.
//! In this state, the port waits for buffers to come back to their owner. it
//! then transitions to the OmxStateIdle state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateTransientToIdle(const ImgEns_FsmEvent *evt) 
{
	switch (evt->signal()) 
	{
	case eSignal_Entry: 
		return OMX_ErrorNone;
	case OMX_SETSTATECOMPLETE_SIG: 
	{
		ImgEns_CmdSetStateCompleteEvt *setStateCompleteEvt = (ImgEns_CmdSetStateCompleteEvt *) evt;
		OMX_STATETYPE newState = setStateCompleteEvt->newState();
		OstTraceFiltInst0(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToIdle OMX_SETSTATECOMPLETE_SIG");

		if (newState != OMX_StateIdle) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateInvalid));
		} 
		else if (!mPort.isAllBuffersAtOwner()) 
		{
			if (mPort.useStandardTunneling() && mPort.isBufferSupplier()) 
			{
				//Standard Tunneling case : Buffer supplier port is still waiting
				//for buffers from non-supplier port.
				mWaitBuffersFromNeighbor = OMX_TRUE;
			} 
			else 
			{
				OstTraceFiltInst0(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToIdle OMX_SETSTATECOMPLETE_SIG mWaitBuffersToNeighbor");
				mWaitBuffersToNeighbor = OMX_TRUE;
			}
		} 
		else 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}

		return OMX_ErrorNone;
	}

	case OMX_EMPTY_THIS_BUFFER_SIG: 
	{
		ImgEns_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ImgEns_CmdEmptyThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = emptyThisBufferEvt->getOMXBufferHdrPtr();

		if (!mPort.useStandardTunneling() || !mPort.isBufferSupplier()) 
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToIdle OMX_EMPTY_THIS_BUFFER_SIG incorrect state operation");
			return OMX_ErrorIncorrectStateOperation;
		}

		mPort.bufferReturnedFromNeighbor(pBuffer);
		if (m_BufferSharingEnabled) 
		{
			if (mPort.isSharingPort()) 
			{
				OMX_ERRORTYPE error = mPort.forwardInputBuffer(pBuffer);
				if (error != OMX_ErrorNone)
					return error;
				mPort.bufferSentToSharingPort(pBuffer);
			}
		}
		OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToIdle OMX_EMPTY_THIS_BUFFER_SIG 0x%x", (unsigned int)(emptyThisBufferEvt->getOMXBufferHdrPtr()));

		if (mWaitBuffersFromNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) 
		{
			mWaitBuffersFromNeighbor = OMX_FALSE;
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}
		return OMX_ErrorNone;
	}
	case OMX_FILL_THIS_BUFFER_SIG: 
	{

		ImgEns_CmdFillThisBufferEvt *fillThisBufferEvt = (ImgEns_CmdFillThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pBuffer = fillThisBufferEvt->getOMXBufferHdrPtr();

		if (!mPort.useStandardTunneling() || !mPort.isBufferSupplier()) 
		{
			return OMX_ErrorIncorrectStateOperation;
		}

		mPort.bufferReturnedFromNeighbor(pBuffer);
		OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToIdle OMX_FILL_THIS_BUFFER_SIG 0x%x", (unsigned int)(fillThisBufferEvt->getOMXBufferHdrPtr()));
		if (m_BufferSharingEnabled) 
		{
			if (mPort.isSharingPort()) 
			{
				OMX_ERRORTYPE error = mPort.forwardOutputBuffer(pBuffer);
				if (error != OMX_ErrorNone)
					return error;
				mPort.bufferSentToSharingPort(pBuffer);
			}
		}

		if (mWaitBuffersFromNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) 
		{
			mWaitBuffersFromNeighbor = OMX_FALSE;
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_BUFFER_DONE_SIG: 
		{
		ImgEns_CmdBufferHdrEvt *emptyBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();

		if (mWaitBuffersFromNeighbor == OMX_TRUE) 
		{
			stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateInvalid) );
			return OMX_ErrorNone;
		}

		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
		if (m_BufferSharingEnabled) 
		{
			if (mPort.isSharingPort()) 
			{
				OMX_ERRORTYPE error = mPort.forwardInputBuffer(pOMXBuffer);
				if (error != OMX_ErrorNone)
					return error;
				mPort.bufferSentToSharingPort(pOMXBuffer);
			}
		}
		OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToIdle OMX_EMPTY_BUFFER_DONE_SIG 0x%x", (unsigned int)(pOMXBuffer));
		if (!mPort.getTunneledComponent()) 
		{
			mPort.bufferSentToNeighbor(pOMXBuffer);
			mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
		} 
		else if (!mPort.isBufferSupplier()) 
		{
			mPort.bufferSentToNeighbor(pOMXBuffer);
			OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
		}

		if (mWaitBuffersToNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) 
		{
			mWaitBuffersToNeighbor = OMX_FALSE;
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}

		return OMX_ErrorNone;
	}

	case OMX_FILL_BUFFER_DONE_SIG: 
	{

		ImgEns_CmdBufferHdrEvt *fillBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();

		if (mWaitBuffersFromNeighbor == OMX_TRUE) 
		{
			stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateInvalid));
			return OMX_ErrorNone;
		}

		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
		if (m_BufferSharingEnabled) 
		{
			IMGENS_ASSERT(mPort.isSharingPort() == OMX_FALSE);
		}
		OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToIdle OMX_FILL_BUFFER_DONE_SIG 0x%x", (unsigned int)(pOMXBuffer));
		if (!mPort.getTunneledComponent()) 
		{
			mPort.bufferSentToNeighbor(pOMXBuffer);
			mPort.getENSComponent().fillBufferDoneCB(pOMXBuffer);
		} 
		else if (!mPort.isBufferSupplier()) 
		{
			mPort.bufferSentToNeighbor(pOMXBuffer);
			OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
		}

		if (mWaitBuffersToNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) 
		{
			mWaitBuffersToNeighbor = OMX_FALSE;
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG: 
		{
		ImgEns_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt = (ImgEns_CmdEmptyThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;

		IMGENS_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer, &pMappedOMXBuffer));
		copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);
		mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);

		if (!mPort.isSharingPort())
		{
			if (!mPort.isAllocatorPort())
			{
				mPort.bufferSentToNeighbor(pMappedOMXBuffer);

				if (!mPort.getTunneledComponent()) 
				{
					mPort.getENSComponent().fillBufferDoneCB(pMappedOMXBuffer);
				} 
				else 
				{
					OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
				}
			}
		} 
		else
			return OMX_ErrorIncorrectStateOperation;

		if (mWaitBuffersToNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner())
		{
			mWaitBuffersToNeighbor = OMX_FALSE;
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}

		if (mWaitBuffersFromNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner())
		{
			mWaitBuffersFromNeighbor = OMX_FALSE;
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}
		return OMX_ErrorNone;
	}

	case OMX_FILL_THIS_SHARED_BUFFER_SIG:
	{
		ImgEns_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt = (ImgEns_CmdFillThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =fillThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
		OMX_BOOL sendBufferToNeighbor;
		IMGENS_ASSERT(OMX_ErrorNone== mPort.mapSharedBufferHeader(pOMXBuffer, &pMappedOMXBuffer));
		sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
		if (sendBufferToNeighbor) 
		{
			if (!mPort.isSharingPort()) 
			{
				if (!mPort.isAllocatorPort()) 
				{
					copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);
					mPort.bufferSentToNeighbor(pMappedOMXBuffer);
					if (!mPort.getTunneledComponent()) 
					{
						mPort.getENSComponent().emptyBufferDoneCB(pMappedOMXBuffer);
					} 
					else 
					{
						OMX_FillThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
					}
				}
			} 
			else
				return OMX_ErrorIncorrectStateOperation;
		}

		if (mWaitBuffersToNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) 
		{
			mWaitBuffersToNeighbor = OMX_FALSE;
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}

		if (mWaitBuffersFromNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) 
		{
			mWaitBuffersFromNeighbor = OMX_FALSE;
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		}
		return OMX_ErrorNone;
	}

	case OMX_FREE_BUFFER_SIG:
		OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToIdle FREE_BUFFER_SIG error: incorrect state operation (need to be in IDLE state)");
		return OMX_ErrorIncorrectStateOperation;

	default: 
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief transient state used when a command has been sent to processing component
//!
//! This method implements the behavior of the port FSM when  a SendCommand(setState)
//! has been sent to the processing component. It waits for the command complete event
//! to transition to the target state
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateTransient(const ImgEns_FsmEvent *evt) 
{
	switch (evt->signal()) 
	{
	case eSignal_Entry: 
	{
		return OMX_ErrorNone;
	}

	case OMX_SETSTATECOMPLETE_SIG: 
	{
		ImgEns_CmdSetStateCompleteEvt *setStateCompleteEvt =(ImgEns_CmdSetStateCompleteEvt *) evt;
		OMX_STATETYPE newState = setStateCompleteEvt->newState();
		if (newState == OMX_StateExecuting) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateExecuting));
		} 
		else if (newState == OMX_StatePause) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStatePause));
		} 
		else if (newState == OMX_StateIdle) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdle));
		} 
		else 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateInvalid));
		}
		return OMX_ErrorNone;
	}
	case OMX_FREE_BUFFER_SIG:
		OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransient FREE_BUFFER_SIG error: incorrect state operation");
		return OMX_ErrorIncorrectStateOperation;
	default: 
	{
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief state handler when the port is in invalid state
//!
//! This method implements the behavior of the port FSM when it is in
//! the invalid state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateInvalid(const ImgEns_FsmEvent *evt) 
{
	switch (evt->signal()) 
	{
	case eSignal_Entry: 
	{
		return OMX_ErrorNone;
	}

	default:
		OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateInvalid error");
		return OMX_ErrorInvalidState;
	}
}

//-----------------------------------------------------------------------------
//! \brief transient state when disabling a port
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! to disabled state. This transient state can be entered only if the port is tunneled
//! and buffer supplier. In this case it is the first transient state and it waits
//! for all buffers to be returned from tunneled component. When it is the case,
//! the port FSM transitions to the OmxStateTransientToDisableDSP state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateTransientToDisableWaitBuffersFromNeighbor(const ImgEns_FsmEvent *evt) 
{

	switch (evt->signal()) 
	{

	case eSignal_Entry: 
	{
		MSG2("OmxStateTransientToDisableWaitBuffersFromNeighbor eSignal_Entry h=0x%p portIdx=%lu \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableWaitBuffersFromNeighbor eSignal_Entry");
		DBC_PRECONDITION(mPort.useStandardTunneling() && mPort.isBufferSupplier());
		return OMX_ErrorNone;
	}
	case OMX_UPDATE_PORT_SETTINGS_SIG: 
	{
		// no callback
		ImgEns_CmdUpdateSettingsEvt *updateSettingsEvt =(ImgEns_CmdUpdateSettingsEvt *) evt;
		return mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
	}

	case OMX_EMPTY_THIS_BUFFER_SIG: 
		{
		ImgEns_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ImgEns_CmdEmptyThisBufferEvt *) evt;
		mPort.bufferReturnedFromNeighbor( emptyThisBufferEvt->getOMXBufferHdrPtr());
		OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableWaitBuffersFromNeighbor OMX_EMPTY_THIS_BUFFER_SIG 0x%x", (unsigned int)(emptyThisBufferEvt->getOMXBufferHdrPtr()));
		if (!m_BufferSharingEnabled)
		{
			if (mPort.isAllBuffersReturnedFromNeighbor()) 
			{
				stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
			}
		} 
		else 
		{
			if (mPort.isSharingPort()) 
			{
				OMX_ERRORTYPE err;
				err = mPort.forwardInputBuffer(emptyThisBufferEvt->getOMXBufferHdrPtr());
				if (err != OMX_ErrorNone)
					return err;

				mPort.bufferSentToSharingPort(emptyThisBufferEvt->getOMXBufferHdrPtr());

				if (mPort.isAllBuffersReturnedFromNeighbor()) 
				{
					stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
				}
			}

			if (mPort.isAllocatorPort()) 
			{
				if (mPort.isAllBuffersReturnedFromNeighbor() && mPort.isAllBuffersReturnedFromSharingPorts()) 
				{
					stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
				}
			}
		}
		return OMX_ErrorNone;
	}

	case OMX_FILL_THIS_BUFFER_SIG: 
	{
		ImgEns_CmdFillThisBufferEvt *fillThisBufferEvt =(ImgEns_CmdFillThisBufferEvt *) evt;

		mPort.bufferReturnedFromNeighbor( fillThisBufferEvt->getOMXBufferHdrPtr());
		OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableWaitBuffersFromNeighbor OMX_FILL_THIS_BUFFER_SIG 0x%x", (unsigned int)(fillThisBufferEvt->getOMXBufferHdrPtr()));
		if (!m_BufferSharingEnabled) 
		{
			if (mPort.isAllBuffersReturnedFromNeighbor()) 
			{
				stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
			}
		} 
		else 
		{
			if (mPort.isSharingPort()) 
			{
				OMX_ERRORTYPE err;
				err = mPort.forwardOutputBuffer(fillThisBufferEvt->getOMXBufferHdrPtr());
				if (err != OMX_ErrorNone)
					return err;

				mPort.bufferSentToSharingPort(fillThisBufferEvt->getOMXBufferHdrPtr());

				if (mPort.isAllBuffersReturnedFromNeighbor()) 
				{
					stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
				}
			}

			if (mPort.isAllocatorPort()) 
			{
				if (mPort.isAllBuffersReturnedFromNeighbor() && mPort.isAllBuffersReturnedFromSharingPorts()) 
				{
					stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
				}
			}
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_BUFFER_DONE_SIG: 
		{
		ImgEns_CmdBufferHdrEvt *emptyBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =emptyBufferDoneEvt->getOMXBufferHdrPtr();

		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
		if (m_BufferSharingEnabled) 
		{
			if (mPort.isSharingPort()) 
			{
				OMX_ERRORTYPE err;
				err = mPort.forwardInputBuffer(pOMXBuffer);
				if (err != OMX_ErrorNone)
					return err;
				mPort.bufferSentToSharingPort(pOMXBuffer);
			}
		}
		return OMX_ErrorNone;
	}

	case OMX_FILL_BUFFER_DONE_SIG: 
	{
		ImgEns_CmdBufferHdrEvt *fillBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();
		if (m_BufferSharingEnabled) 
		{
			IMGENS_ASSERT(mPort.isSharingPort() == OMX_FALSE);
		}
		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
		return OMX_ErrorNone;
	}

	case OMX_FREE_BUFFER_SIG:
		OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableWaitBuffersFromNeighbor FREE_BUFFER_SIG error: incorrect state operation");
		return OMX_ErrorIncorrectStateOperation;

	case OMX_FILL_THIS_SHARED_BUFFER_SIG: 
		{
		ImgEns_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt = (ImgEns_CmdFillThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
		OMX_BOOL sendBufferToNeighbor;

		if (mPort.isAllocatorPort()) 
		{
			IMGENS_ASSERT( OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer, &pMappedOMXBuffer));
			sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort( pMappedOMXBuffer);

			if (sendBufferToNeighbor) 
			{
				copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);
				if (mPort.isAllBuffersReturnedFromNeighbor() && mPort.isAllBuffersReturnedFromSharingPorts()) 
				{
					stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
				}
			}
		} 
		else 
		{
			return OMX_ErrorIncorrectStateOperation;
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG: 
		{
		ImgEns_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt =(ImgEns_CmdEmptyThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;

		if (mPort.isAllocatorPort()) 
		{
			IMGENS_ASSERT( OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer, &pMappedOMXBuffer));
			mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
			copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);
			if (mPort.isAllBuffersReturnedFromNeighbor() && mPort.isAllBuffersReturnedFromSharingPorts()) 
			{
				stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP));
			}
		} 
		else 
		{
			return OMX_ErrorIncorrectStateOperation;
		}
		return OMX_ErrorNone;
	}

	default: {
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief transient state when disabling a port
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! to disabled state. This transient state can be either the second or first transient
//! state when disabling a port, depending on wheter the
//! OmxStateTransientToDisableWaitBuffersFromNeighbor state is entered or not.
//! In this state, the port FSM sends the disable port command
//! to the processing component as entry action, and then waits for the disable port
//! complete and all buffers to be returned by the processing component.
//! When this is the case, the port FSM transitions to the
//! OmxStateTransientToDisableWaitForFreeBuffers state if the port is port is not supplier,
//! or it frees all the buffers if it is supplier
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateTransientToDisableDSP(const ImgEns_FsmEvent *evt) 
{
	switch (evt->signal()) 
	{
	case eSignal_Entry: 
	{
		MSG2("OmxStateTransientToDisableDSP eSignal_Entry h=0x%p portIdx=%lu \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableDSP eSignal_Entry");
		if (mEventAppliedOnEachPorts == OMX_FALSE) 
		{
			return getProcessingComponent().sendCommand(OMX_CommandPortDisable, mPort.getPortIndex());
		} 
		else if (mPort.getENSComponent().allPortsInSameFsmState(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableDSP))== OMX_TRUE) 
		{
			mEventAppliedOnEachPorts = OMX_FALSE;
			return getProcessingComponent().sendCommand(OMX_CommandPortDisable, OMX_ALL);
		} 
		else 
		{
			mEventAppliedOnEachPorts = OMX_FALSE;
		}
		return OMX_ErrorNone;
	}

	case OMX_UPDATE_PORT_SETTINGS_SIG: 
	{
		// no callback
		ImgEns_CmdUpdateSettingsEvt *updateSettingsEvt =(ImgEns_CmdUpdateSettingsEvt *) evt;
		return mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
	}
	case OMX_DISABLEPORTCOMPLETE_SIG: 
	{
		if (mPort.getBufferCountCurrent() == 0) 
		{
			stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateDisabled));
		} 
		else if (mPort.useStandardTunneling() && mPort.isBufferSupplier()) 
		{
			OMX_ERRORTYPE error;
			if (!m_BufferSharingEnabled) 
			{
				error = mPort.standardTunnelingDeInit();
				if (error != OMX_ErrorNone) 
				{
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableDSP DISABLEPORTCOMPLETE_SIG error: %d", error);
					return error;
				}
				stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateDisabled));
			} 
			else 
			{
				if (mPort.isAllocatorPort()) 
				{
					error = mPort.standardTunnelingDeInit();
					if (error != OMX_ErrorNone) 
					{
						OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableDSP DISABLEPORTCOMPLETE_SIG error: %d", error);
						return error;
					}
					stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateDisabled));
				}

				if (mPort.isSharingPort()) 
				{
					error = mPort.bufferSharingDeinit();
					if (error != OMX_ErrorNone) 
					{
						OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableDSP DISABLEPORTCOMPLETE_SIG error: %d", error);
						return error;
					}
					stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateDisabled));
				}
			}
		} 
		else 
		{
			if (!m_BufferSharingEnabled) 
			{
				stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableWaitForFreeBuffers));
			} 
			else 
			{
				if (mPort.isAllBuffersReturnedFromSharingPorts()) 
				{
					stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableWaitForFreeBuffers));
				}
			}
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_BUFFER_DONE_SIG: 
		{
		ImgEns_CmdBufferHdrEvt *emptyBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =emptyBufferDoneEvt->getOMXBufferHdrPtr();
		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
		if (!mPort.getTunneledComponent()) 
		{
			mPort.bufferSentToNeighbor(pOMXBuffer);
			mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
		} 
		else 
		{
			if (!m_BufferSharingEnabled) 
			{
				if (mPort.isBufferSupplier())
					return OMX_ErrorNone;
			} 
			else {
				if (mPort.isAllocatorPort())
					return OMX_ErrorNone;

				if (mPort.isSharingPort())
				{
					OMX_ERRORTYPE err;
					err = mPort.forwardInputBuffer(pOMXBuffer);
					if (err != OMX_ErrorNone)
						return err;
					mPort.bufferSentToSharingPort(pOMXBuffer);
					return OMX_ErrorNone;
				}

			}
			mPort.bufferSentToNeighbor(pOMXBuffer);
			OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
		}
		return OMX_ErrorNone;
	}

	case OMX_FILL_BUFFER_DONE_SIG: 
		{
		ImgEns_CmdBufferHdrEvt *fillBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();

		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);

		if (!mPort.getTunneledComponent()) 
		{
			mPort.bufferSentToNeighbor(pOMXBuffer);
			mPort.getENSComponent().fillBufferDoneCB(pOMXBuffer);
		} 
		else 
		{
			if (!m_BufferSharingEnabled) 
			{
				if (mPort.isBufferSupplier())
					return OMX_ErrorNone;
			} 
			else 
			{
				if (mPort.isAllocatorPort())
					return OMX_ErrorNone;
				if (mPort.isSharingPort())
					IMGENS_ASSERT(0);
			}
			mPort.bufferSentToNeighbor(pOMXBuffer);
			OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
		}
		return OMX_ErrorNone;
	}

	case OMX_FREE_BUFFER_SIG: 
		{
		ImgEns_CmdFreeBufferEvt *freeBufferEvt = (ImgEns_CmdFreeBufferEvt *) evt;

		if (mPort.getTunneledComponent() && mPort.isBufferSupplier()) 
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableDSP FREE_BUFFER_SIG error: incorrect state operation");
			return OMX_ErrorIncorrectStateOperation;
		}
		return mPort.freeBuffer(freeBufferEvt->getOMXBufferHdrPtr());
	}

	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG: 
		{
		ImgEns_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt =(ImgEns_CmdEmptyThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
		IMGENS_ASSERT(mPort.isBufferSupplier() == OMX_FALSE);
		IMGENS_ASSERT(OMX_ErrorNone== mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
		copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);
		mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
		mPort.bufferSentToNeighbor(pMappedOMXBuffer);
		if (!mPort.getTunneledComponent()) 
		{
			mPort.getENSComponent().fillBufferDoneCB(pMappedOMXBuffer);
		} 
		else 
		{
			OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
		}
		if (mPort.isAllBuffersReturnedFromSharingPorts()) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableWaitForFreeBuffers));
		}
		return OMX_ErrorNone;
	}

	case OMX_FILL_THIS_SHARED_BUFFER_SIG: 
		{
		ImgEns_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt =(ImgEns_CmdFillThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =fillThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
		OMX_BOOL sendBufferToNeighbor;

		IMGENS_ASSERT(mPort.isBufferSupplier() == OMX_FALSE);
		IMGENS_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer, &pMappedOMXBuffer));
		sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);

		if (sendBufferToNeighbor) 
		{
			copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);
			mPort.bufferSentToNeighbor(pMappedOMXBuffer);
			if (!mPort.getTunneledComponent()) 
			{
				mPort.getENSComponent().emptyBufferDoneCB(pMappedOMXBuffer);
			} 
			else 
			{
				OMX_FillThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
			}
		}

		if (mPort.isAllBuffersReturnedFromSharingPorts()) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToDisableWaitForFreeBuffers));
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableDSP error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default: 
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief transient state when disabling a port
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! to disabled state. This transient state can be entered only if the port is not supplier.
//! In this state, the port waits for calls to OMX_FreeBuffer for all buffers.
//! When it is the case, the port FSM transitions to the OmxStateDisabled state.
//! For hybrid component (having both ARm and DSP port), buffers can be received (fill/empty_buffer_done)
//! in this state because we cannot ensure nmf message order. For example buffer can be received
//! NMF-DSP callback thread, and disable port complete by HOST-EE callback thread
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateTransientToDisableWaitForFreeBuffers(const ImgEns_FsmEvent *evt) 
{

	OMX_ERRORTYPE error;

	switch (evt->signal()) 
	{
	case eSignal_Entry:
		MSG2("OmxStateTransientToDisableWaitForFreeBuffers eSignal_Entry h=0x%p portIdx=%lu \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableWaitForFreeBuffers  eSignal_Entry");
		DBC_PRECONDITION(!mPort.useProprietaryCommunication() && !mPort.isBufferSupplier());
		return OMX_ErrorNone;

	case OMX_UPDATE_PORT_SETTINGS_SIG: 
	{
		// no callback
		ImgEns_CmdUpdateSettingsEvt *updateSettingsEvt =(ImgEns_CmdUpdateSettingsEvt *) evt;
		return mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
	}

	case OMX_FREE_BUFFER_SIG: 
	{
		ImgEns_CmdFreeBufferEvt *freeBufferEvt = (ImgEns_CmdFreeBufferEvt *) evt;

		error = mPort.freeBuffer(freeBufferEvt->getOMXBufferHdrPtr());
		if (error != OMX_ErrorNone) 
		{
			OstTraceFiltInst1(TRACE_ERROR,"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableWaitForFreeBuffers FREE_BUFFER_SIG error: %d",  error);
			return error;
		}
		if (mPort.getBufferCountCurrent() == 0) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateDisabled));
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_BUFFER_DONE_SIG: 
		{
		ImgEns_CmdBufferHdrEvt *emptyBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();
		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
		if (!mPort.getTunneledComponent()) 
		{
			mPort.bufferSentToNeighbor(pOMXBuffer);
			mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
		} 
		else 
		{
			if (mPort.isBufferSupplier())
				return OMX_ErrorNone;
			mPort.bufferSentToNeighbor(pOMXBuffer);
			OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
		}
		return OMX_ErrorNone;
	}

	case OMX_FILL_BUFFER_DONE_SIG: 
	{
		ImgEns_CmdBufferHdrEvt *fillBufferDoneEvt = (ImgEns_CmdBufferHdrEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();
		mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
		if (!mPort.getTunneledComponent()) 
		{
			mPort.bufferSentToNeighbor(pOMXBuffer);
			mPort.getENSComponent().fillBufferDoneCB(pOMXBuffer);
		} 
		else 
		{
			if (mPort.isBufferSupplier())
				return OMX_ErrorNone;
			mPort.bufferSentToNeighbor(pOMXBuffer);
			OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
		}
		return OMX_ErrorNone;
	}
	case OMX_FILL_THIS_SHARED_BUFFER_SIG:
	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToDisableWaitForFreeBuffers error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default:
		return defaultBehavior(evt);
	}
}

//-----------------------------------------------------------------------------
//! \brief  state when a port is disabled
//!
//! This method implements the behavior of the port FSM when the port is disabled
//! The mDisabledState member variable stores the state to which the port should
//! transition if the port would be enabled
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateDisabled(const ImgEns_FsmEvent *evt) {

	switch (evt->signal()) {

	case eSignal_Entry: {
		MSG2("OmxStateDisabled eSignal_Entry h=0x%p portIdx=%lu \n",
				mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateDisabled  eSignal_Entry");
		return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandPortDisable,
				mPort.getPortIndex());
	}

	case OMX_SETSTATE_SIG: {
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		OMX_STATETYPE targetState = setStateEvt->targetState();

		if (mDisabledState == OMX_StateLoaded && targetState == OMX_StateIdle) {
			mDisabledState =  (OMX_STATETYPE)OMX_StateCheckRscAvailability;
		} else if (targetState == OMX_StateLoaded) {
			mDisabledState = OMX_StateLoaded;
		} else {
			mDisabledState =  (OMX_STATETYPE)OMX_StateTransient;
		}
		return OMX_ErrorNone;
	}

	case OMX_SETSTATECOMPLETE_SIG: {
		ImgEns_CmdSetStateCompleteEvt *setStateCompleteEvt =
				(ImgEns_CmdSetStateCompleteEvt *) evt;
		mDisabledState = setStateCompleteEvt->newState();
		return OMX_ErrorNone;
	}

	case OMX_SETPARAMETER_SIG: {
		ImgEns_CmdSetParamEvt *setParamEvt = (ImgEns_CmdSetParamEvt *) evt;
		return mPort.setParameter(setParamEvt->getIndex(),
				setParamEvt->getStructPtr());
	}

	case OMX_UPDATE_PORT_SETTINGS_SIG: {
		// no callback
		ImgEns_CmdUpdateSettingsEvt *updateSettingsEvt =
				(ImgEns_CmdUpdateSettingsEvt *) evt;
		return mPort.setParameter(updateSettingsEvt->getIndex(),
				updateSettingsEvt->getStructPtr());
	}

	case OMX_ENABLE_PORT_SIG: {
		ImgEns_CmdPortEvt *portEvt = (ImgEns_CmdPortEvt *) evt;
		return goToEnable(portEvt->getPortIdx());
	}

	case OMX_COMP_TUNNEL_REQUEST_SIG:
	{
		ImgEns_CmdTunnelRequestEvt *tunnelReqEvt = (ImgEns_CmdTunnelRequestEvt *) evt;
		OMX_ERRORTYPE err = mPort.tunnelRequest(tunnelReqEvt->getTunneledCompHdl(), tunnelReqEvt->getTunneledPortIdx(), tunnelReqEvt->getTunnelStructPtr());
		if (OMX_ErrorNone != err)
		{
			OstTraceFiltInst1(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateDisabled COMP_TUNNEL_REQUEST_SIG error: %d", err);
			return err;
		}
#ifdef USE_RME
		// Notify the RM if port is an output port OR standard Tunneled
		if (mPort.getDirection() == OMX_DirOutput)
		{
			ImgEns_HwRm::HWRM_Notify_Tunnel(mPort.getComponent(),
					mPort.getPortIndex(), tunnelReqEvt->getTunneledCompHdl(),
					tunnelReqEvt->getTunneledPortIdx());
		}
		else if (mPort.useStandardTunneling())
		{
			ImgEns_HwRm::HWRM_Notify_Tunnel(tunnelReqEvt->getTunneledCompHdl(),
					tunnelReqEvt->getTunneledPortIdx(), mPort.getComponent(),
					mPort.getPortIndex());
		}
#endif
		return OMX_ErrorNone;
	}

	case OMX_RESRESERVATIONPROCESSED_SIG: {
		ImgEns_CmdResReservationProcessedEvt *ackRscEvt =
				(ImgEns_CmdResReservationProcessedEvt *) evt;
		OMX_BOOL rscGranted = ackRscEvt->resGranted();

		// Not enough resources
		if (rscGranted == OMX_FALSE)
			mDisabledState = OMX_StateLoaded;
		else
			mDisabledState =  (OMX_STATETYPE)OMX_StateLoadedToIdleDSP;
		return OMX_ErrorNone;
	}

	case OMX_FILL_THIS_SHARED_BUFFER_SIG:
	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateDisabled error: incorrect state operation %d",
				evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default: {
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief  Transient state when a port is being enabled
//!
//! This method implements the behavior of the port FSM when the port is transitionning
//! to enabled state. This state is entered only if the port is not-supplier.
//! In this state, the port waits for calls to OMX_AllocateBuffer or OMX_UseBuffer until
//! the port is populated. When it is the case, the port transitions to
//! the OmxStateTransientToEnableDSP state.
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateTransientToEnableHOST(
		const ImgEns_FsmEvent *evt) {

	OMX_ERRORTYPE error;

	switch (evt->signal()) {
	case eSignal_Entry: {
		MSG2("OmxStateTransientToEnableHOST eSignal_Entry h=0x%p portIdx=%lu \n",
				mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableHOST eSignal_Entry");

		if (!m_BufferSharingEnabled) {
			DBC_PRECONDITION(
					!mPort.useProprietaryCommunication() && !mPort.isBufferSupplier());
		} else {
			DBC_PRECONDITION(
					!mPort.useProprietaryCommunication() && !mPort.isAllocatorPort());
		}
		return OMX_ErrorNone;
	}

	case OMX_SETPARAMETER_SIG: {
		if (m_BufferSharingEnabled) {
			OMX_ERRORTYPE error;
			ImgEns_CmdSetParamEvt *setParamEvt = (ImgEns_CmdSetParamEvt *) evt;
			// Only OMX_IndexParamPortDefinition index is supported in this state
			if (setParamEvt->getIndex() != OMX_IndexParamPortDefinition)
				return OMX_ErrorIncorrectStateOperation;

			error = mPort.setParameter(setParamEvt->getIndex(),
					setParamEvt->getStructPtr());
			if (error != OMX_ErrorNone)
				return error;

			error = mPort.sendBufferReqs(OMX_IndexParamPortDefinition,
					setParamEvt->getStructPtr());
			if (error != OMX_ErrorNone)
				return error;

			return OMX_ErrorNone;
		} else {
			ImgEns_CmdSetParamEvt *setParamEvt = (ImgEns_CmdSetParamEvt *) evt;
			//Only OMX_IndexParamPortDefinition index is supported in this state
			if (setParamEvt->getIndex() != OMX_IndexParamPortDefinition)
				return OMX_ErrorIncorrectStateOperation;
			return mPort.setParameter(setParamEvt->getIndex(),
					setParamEvt->getStructPtr());
		}
	}
	case OMX_USE_BUFFER_SIG: {
		ImgEns_CmdUseBufferEvt *useBufferEvt = (ImgEns_CmdUseBufferEvt *) evt;
		error = mPort.useBuffer(useBufferEvt->getOMXBufferHdrPtr(),
				useBufferEvt->getAppPrivate(), useBufferEvt->getSizeBytes(),
				useBufferEvt->getBufferPtr());
		if (error != OMX_ErrorNone) {
			OstTraceFiltInst1(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableHOST USE_BUFFER_SIG error: %d",
					error);
			return error;
		}

		if (mPort.isPopulated()) {
			if (mPort.getDomain() != OMX_PortDomainOther) {
				stateTransition(
						static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToEnableDSP));
			} else {
				OMX_ERRORTYPE err;
				err = getProcessingComponent().sendCommand(
						OMX_CommandPortEnable, mPort.getPortIndex());
				if (err != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR,
							"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableHOST USE_BUFFER_SIG Populated port error: %d",
							err);
					return err;
				}
				stateTransition(getFsmParentState(mDisabledState));
				mDisabledState = OMX_StateInvalid;
				return eventHandlerCB(OMX_EventCmdComplete,
						OMX_CommandPortEnable, mPort.getPortIndex());
			}
		}

		return OMX_ErrorNone;
	}

	case OMX_ALLOCATE_BUFFER_SIG: {
		ImgEns_CmdAllocBufferEvt *allocBufferEvt = (ImgEns_CmdAllocBufferEvt *) evt;
		error = mPort.allocateBuffer(allocBufferEvt->getOMXBufferHdrPtr(),
				allocBufferEvt->getAppPrivate(),
				allocBufferEvt->getSizeBytes());
		if (error != OMX_ErrorNone) {
			OstTraceFiltInst1(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableHOST ALLOCATE_BUFFER_SIG error: %d",
					error);
			return error;
		}

		if (mPort.isPopulated()) {
			if (mPort.getDomain() != OMX_PortDomainOther) {
				stateTransition(
						static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToEnableDSP));
			} else {
				OMX_ERRORTYPE err;
				err = getProcessingComponent().sendCommand(
						OMX_CommandPortEnable, mPort.getPortIndex());
				if (err != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR,
							"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableHOST ALLOCATE_BUFFER_SIG Populated port error: %d",
							err);
					return err;
				}
				stateTransition(getFsmParentState(mDisabledState));
				mDisabledState = OMX_StateInvalid;
				return eventHandlerCB(OMX_EventCmdComplete,
						OMX_CommandPortEnable, mPort.getPortIndex());
			}
		}

		return OMX_ErrorNone;
	}

	case OMX_USE_SHARED_BUFFER_SIG: {
		OMX_ERRORTYPE error;
		IMGENS_ASSERT(mPort.getPortRole() == ImgEns_SHARING_PORT);
		// Sharing ports receives buffer from non supplier/allocator
		// Forward this buffer to tunneled port
		ImgEns_CmdUseSharedBufferEvt *useSharedBufferevt =
				(ImgEns_CmdUseSharedBufferEvt *) evt;
		error = mPort.useSharedBuffer(useSharedBufferevt->getAppPrivate(),
				useSharedBufferevt->getSizeBytes(),
				useSharedBufferevt->getBufferPtr(),
				useSharedBufferevt->getSharedChunk());

		if (error != OMX_ErrorNone)
			return error;

		if (mPort.isPopulated()) {
			if (mPort.getDomain() != OMX_PortDomainOther) {
				stateTransition(
						static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToEnableDSP));
			} else {
				OMX_ERRORTYPE err;
				err = getProcessingComponent().sendCommand(
						OMX_CommandPortEnable, mPort.getPortIndex());
				if (err != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR,
							"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableHOST USE_BUFFER_SIG Populated port error: %d",
							err);
					return err;
				}
				stateTransition(getFsmParentState(mDisabledState));
				mDisabledState = OMX_StateInvalid;
				return eventHandlerCB(OMX_EventCmdComplete,
						OMX_CommandPortEnable, mPort.getPortIndex());
			}
		}

		return OMX_ErrorNone;

	}

	case OMX_GETPARAMETER_SIG: {
		if (m_BufferSharingEnabled) {
			OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
			OMX_ERRORTYPE error;
			ImgEns_CmdGetParamEvt *getParamEvt = (ImgEns_CmdGetParamEvt *) evt;
			OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
			error = mPort.getParameter(getParamEvt->getIndex(),
					pComponentParameterStructure);
			if (error != OMX_ErrorNone)
				return error;

			error = mPort.calculateBufferReqs(&maxBufferSize,
					&maxBufferCountActual);
			if (error != OMX_ErrorNone)
				return error;

			OMX_PARAM_PORTDEFINITIONTYPE *portdef =
					static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(pComponentParameterStructure);

			// Calculate self requirements
			maxBufferSize = MAX(portdef->nBufferSize, maxBufferSize);
			maxBufferCountActual =
					MAX(portdef->nBufferCountActual,maxBufferCountActual);

			portdef->nBufferSize = maxBufferSize;
			portdef->nBufferCountActual = maxBufferCountActual;

			return OMX_ErrorNone;
		} else {
			return defaultBehavior(evt);
		}
	}
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableHOST error: incorrect state operation %d",
				evt->signal());
		return OMX_ErrorIncorrectStateOperation;

	default: {
		return defaultBehavior(evt);
	}
	}
}

//-----------------------------------------------------------------------------
//! \brief  Transient state when a port is being enabled
//!
//! This method implements the behavior of the port FSM when the port is transitionning
//! to enabled state. This state is entered once the port is populated.
//! In this state, the port FSM sends the enable port command to the processing component
//! as entry action, then waits for the command complete event.
//! It then transition to the state stored in the mDisabledState member variable
//!
//! \param evt the event to process
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateTransientToEnableDSP(
		const ImgEns_FsmEvent *evt) {

	switch (evt->signal()) {

	case eSignal_Entry: {
		MSG2("OmxStateTransientToEnableDSP eSignal_Entry h=0x%p portIdx=%lu \n",
				mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableDSP eSignal_Entry");
		if (mEventAppliedOnEachPorts == OMX_FALSE) {
			return getProcessingComponent().sendCommand(OMX_CommandPortEnable,
					mPort.getPortIndex());
		} else if (mPort.getENSComponent().allPortsInSameFsmState(
				static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTransientToEnableDSP))
				== OMX_TRUE) {
			mEventAppliedOnEachPorts = OMX_FALSE;
			return getProcessingComponent().sendCommand(OMX_CommandPortEnable, OMX_ALL);
		}
		else
		{
			mEventAppliedOnEachPorts = OMX_FALSE;
		}
		return OMX_ErrorNone;
	}

	case eSignal_Exit:
	{
		return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandPortEnable, mPort.getPortIndex());
	}
	case OMX_ENABLEPORTCOMPLETE_SIG:
	{
		stateTransition(getFsmParentState(mDisabledState));
		mDisabledState = OMX_StateInvalid;
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_THIS_BUFFER_SIG: {
		ImgEns_CmdEmptyThisBufferEvt *emptyThisBufferEvt =
				(ImgEns_CmdEmptyThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =
				emptyThisBufferEvt->getOMXBufferHdrPtr();
		OMX_ERRORTYPE err = OMX_ErrorNone;

		if (!mPort.getTunneledComponent() || mPort.isBufferSupplier()) {
			return OMX_ErrorIncorrectStateOperation;
		}

		mPort.bufferReturnedFromNeighbor(pOMXBuffer);
		OstTraceFiltInst1(TRACE_DEBUG,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableDSP OMX_EMPTY_THIS_BUFFER_SIG 0x%x",
				(unsigned int)pOMXBuffer);
		mPort.bufferSentToProcessingComponent(pOMXBuffer);

		err = getProcessingComponent().emptyThisBuffer(pOMXBuffer);
		if (err != OMX_ErrorNone) {
			mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
			mPort.bufferSentToNeighbor(pOMXBuffer);
		}

		OstTraceFiltInst1(TRACE_ERROR,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableDSP EMPTY_THIS_BUFFER error: %d",
				err);
		return err;
	}

	case OMX_FILL_THIS_BUFFER_SIG: {
		ImgEns_CmdFillThisBufferEvt *fillThisBufferEvt =
				(ImgEns_CmdFillThisBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =
				fillThisBufferEvt->getOMXBufferHdrPtr();
		OMX_ERRORTYPE err = OMX_ErrorNone;

		if (!mPort.getTunneledComponent() || mPort.isBufferSupplier()) {
			return OMX_ErrorIncorrectStateOperation;
		}

		mPort.bufferReturnedFromNeighbor(pOMXBuffer);
		OstTraceFiltInst1(TRACE_DEBUG,
				"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableDSP OMX_FILL_THIS_BUFFER_SIG 0x%x",
				(unsigned int)pOMXBuffer);
		if (!m_BufferSharingEnabled) {
			mPort.bufferSentToProcessingComponent(pOMXBuffer);

			err = getProcessingComponent().fillThisBuffer(pOMXBuffer);
			if (err != OMX_ErrorNone) {
				mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
				mPort.bufferSentToNeighbor(pOMXBuffer);
			}

			OstTraceFiltInst1(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableDSP FILL_THIS_BUFFER error: %d",
					err);
		} else {
			if (mPort.getNumberInterConnectedPorts()) {
				err = mPort.forwardOutputBuffer(pOMXBuffer);
				if (err != OMX_ErrorNone)
					return err;

				mPort.bufferSentToSharingPort(pOMXBuffer);
			} else {
				mPort.bufferSentToProcessingComponent(pOMXBuffer);

				err = getProcessingComponent().fillThisBuffer(pOMXBuffer);
				if (err != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR,
							"ENS_PROXY: ImgEns_Port_Fsm: defaultBehavior FILL_THIS_BUFFER_SIG error: %d",
							err);
					mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
				}
			}
		}

		return err;
	}

	case OMX_EMPTY_THIS_SHARED_BUFFER_SIG: {
		ImgEns_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt =
				(ImgEns_CmdEmptyThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =
				emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;

		IMGENS_ASSERT(
				OMX_ErrorNone
						== mPort.mapSharedBufferHeader(pOMXBuffer,
								&pMappedOMXBuffer));
		copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);

		mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
		mPort.bufferSentToNeighbor(pMappedOMXBuffer);

		if (!mPort.getTunneledComponent()) {
			mPort.getENSComponent().fillBufferDoneCB(pMappedOMXBuffer);
		} else {
			OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
		}

		return OMX_ErrorNone;
	}

	case OMX_FILL_THIS_SHARED_BUFFER_SIG: {
		ImgEns_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt =
				(ImgEns_CmdFillThisSharedBufferEvt *) evt;
		OMX_BUFFERHEADERTYPE *pOMXBuffer =
				fillThisSharedBufferEvt->getOMXBufferHdrPtr();
		OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
		OMX_BOOL sendBufferToNeighbor;

		IMGENS_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer, &pMappedOMXBuffer));
		sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);

		if (sendBufferToNeighbor) {
			mPort.bufferSentToNeighbor(pMappedOMXBuffer);
			copyOMXBufferHeaderFields(pOMXBuffer, pMappedOMXBuffer);
			if (!mPort.getTunneledComponent()) {
				mPort.getENSComponent().emptyBufferDoneCB(pMappedOMXBuffer);
			} else {
				OMX_FillThisBuffer(mPort.getTunneledComponent(),
						pMappedOMXBuffer);
			}
		}

		return OMX_ErrorNone;
	}

	case OMX_USE_BUFFER_SIG: {
		ImgEns_CmdUseBufferEvt *useBufferEvt = (ImgEns_CmdUseBufferEvt *) evt;
		return mPort.useBuffer(useBufferEvt->getOMXBufferHdrPtr(),
				useBufferEvt->getAppPrivate(), useBufferEvt->getSizeBytes(),
				useBufferEvt->getBufferPtr());
	}

	case OMX_ALLOCATE_BUFFER_SIG: {
		ImgEns_CmdAllocBufferEvt *allocBufferEvt = (ImgEns_CmdAllocBufferEvt *) evt;
		return mPort.allocateBuffer(allocBufferEvt->getOMXBufferHdrPtr(),
				allocBufferEvt->getAppPrivate(), allocBufferEvt->getSizeBytes());
	}

	case OMX_USE_SHARED_BUFFER_SIG: {
		IMGENS_ASSERT(mPort.getPortRole() == ImgEns_SHARING_PORT);
		// Sharing ports receives buffer from non supplier/allocator
		// Forward this buffer to tunneled port
		ImgEns_CmdUseSharedBufferEvt *useSharedBufferevt =
				(ImgEns_CmdUseSharedBufferEvt *) evt;
		return mPort.useSharedBuffer(useSharedBufferevt->getAppPrivate(),
				useSharedBufferevt->getSizeBytes(),
				useSharedBufferevt->getBufferPtr(),
				useSharedBufferevt->getSharedChunk());

	}

	case OMX_GETPARAMETER_SIG: 
		{
		if (m_BufferSharingEnabled) 
		{
			OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
			OMX_ERRORTYPE error;
			ImgEns_CmdGetParamEvt *getParamEvt = (ImgEns_CmdGetParamEvt *) evt;
			OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
			error = mPort.getParameter(getParamEvt->getIndex(), pComponentParameterStructure);
			if (error != OMX_ErrorNone)
				return error;

			error = mPort.calculateBufferReqs(&maxBufferSize, &maxBufferCountActual);
			if (error != OMX_ErrorNone)
				return error;

			OMX_PARAM_PORTDEFINITIONTYPE *portdef = static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(pComponentParameterStructure);

			// Calculate self requirements
			maxBufferSize = MAX(portdef->nBufferSize, maxBufferSize);
			maxBufferCountActual = MAX(portdef->nBufferCountActual,maxBufferCountActual);

			portdef->nBufferSize = maxBufferSize;
			portdef->nBufferCountActual = maxBufferCountActual;
			return OMX_ErrorNone;
		} 
		else 
		{
			return defaultBehavior(evt);
		}
	}
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTransientToEnableDSP error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;
	default: 
	{
		return defaultBehavior(evt);
	}
	}
}

OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateTestForResources(const ImgEns_FsmEvent *evt) 
{
	switch (evt->signal()) 
	{

	case eSignal_Entry: 
		{
		MSG2("OmxStateTestForResources eSignal_Entry h=0x%p portIdx=%lu \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTestForResources  eSignal_Entry");
		return OMX_ErrorNone;
	}

	case OMX_SETSTATE_SIG: 
		{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		OMX_STATETYPE targetState = setStateEvt->targetState();

		if (targetState == OMX_StateIdle) 
		{
			return goLoadedToIdle();
		} 
		else if (targetState == OMX_StateLoaded) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
			return OMX_ErrorNone;
		} 
		else if (targetState == OMX_StateTestForResources) 
		{
			return OMX_ErrorSameState;
		} 
		else 
		{
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTestForResources SETSTATE_SIG error: incorrect state transition");
			return OMX_ErrorIncorrectStateTransition;
		}
	}
	case OMX_UPDATE_PORT_SETTINGS_SIG:
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_FLUSH_PORT_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateTestForResources error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;
	default: {
		return defaultBehavior(evt);
	}
	}
}

OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateWaitForResources(const ImgEns_FsmEvent *evt) 
{
	switch (evt->signal()) 
	{
	case eSignal_Entry: 
		{
		MSG2("OmxStateWaitForResources eSignal_Entry h=0x%p portIdx=%lu \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateWaitForResources  eSignal_Entry");
		return OMX_ErrorNone;
	}
	case OMX_SETSTATE_SIG: 
		{
		ImgEns_CmdSetStateEvt *setStateEvt = (ImgEns_CmdSetStateEvt *) evt;
		OMX_STATETYPE targetState = setStateEvt->targetState();
		if (targetState == OMX_StateIdle) 
		{
			return goLoadedToIdle();
		} else if (targetState == OMX_StateLoaded) 
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
			return OMX_ErrorNone;
		}
		else if (targetState == OMX_StateWaitForResources) 
		{
			return OMX_ErrorSameState;
		} else {
			OstTraceFiltInst0(TRACE_ERROR,
					"ENS_PROXY: ImgEns_Port_Fsm: OmxStateWaitForResources SETSTATE_SIG error: incorrect state transition");
			return OMX_ErrorIncorrectStateTransition;
		}
	}
	case OMX_UPDATE_PORT_SETTINGS_SIG:
	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_FLUSH_PORT_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateWaitForResources error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;
	default: 
		return defaultBehavior(evt);
	}
}

OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateLoadedToTestForResources(const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{

	case eSignal_Entry:
	{
		MSG2("OmxStateLoadedToTestForResources eSignal_Entry h=0x%p portIdx=%lu \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoadedToTestForResources  eSignal_Entry");
		return OMX_ErrorNone;
	}

	case OMX_RESRESERVATIONPROCESSED_SIG:
	{
		ImgEns_CmdResReservationProcessedEvt *ackRscEvt = (ImgEns_CmdResReservationProcessedEvt *) evt;
		OMX_BOOL rscGranted = ackRscEvt->resGranted();
		if (rscGranted == OMX_FALSE)
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
		}
		else
		{
			stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateTestForResources));
		}
		return OMX_ErrorNone;
	}

	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_FLUSH_PORT_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateLoadedToTestForResources error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;
	default:
		return defaultBehavior(evt);
	}
}

OMX_ERRORTYPE ImgEns_Port_Fsm::fsmStateCheckRscAvailability( const ImgEns_FsmEvent *evt)
{
	switch (evt->signal())
	{
	case eSignal_Entry:
		MSG2("OmxStateCheckRscAvailability eSignal_Entry h=0x%p portIdx=%lu \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
		OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateCheckRscAvailability eSignal_Entry");
		return OMX_ErrorNone;
	case OMX_RESRESERVATIONPROCESSED_SIG:
	{
		ImgEns_CmdResReservationProcessedEvt *ackRscEvt = (ImgEns_CmdResReservationProcessedEvt *) evt;
		OMX_BOOL rscGranted = ackRscEvt->resGranted();
		
		if (rscGranted == OMX_FALSE) 
		{ // Not enough resources
			// Case non-tunneled or std-tunneled not supplier
			if (!mPort.getTunneledComponent() || ( mPort.useStandardTunneling()  && !mPort.isBufferSupplier() )) 
			{
				if (mPort.getBufferCountCurrent() == 0) 
				{
					stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
				} 
				else {
					stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateIdleToLoaded));
				}
			} 
			else 
			{
				stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoaded));
			}
			return OMX_ErrorNone;
		}
		// Resources Granted
		// Allocate buffers in case std-tunneling supplier
		if (!m_BufferSharingEnabled)
		{
			if (mPort.getTunneledComponent() && mPort.useStandardTunneling()
					&& mPort.isBufferSupplier()) {
				OMX_ERRORTYPE error = mPort.standardTunnelingInit();
				if (error != OMX_ErrorNone)
				{
					stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateInvalid));
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateCheckRscAvailability RESRESERVATIONPROCESSED_SIG error: %d", error);
					return OMX_ErrorNone; // do not return the error, otherwise the transition is not taken into account
				}
			}
		} 
		else 
		{
			if (mPort.getTunneledComponent() && mPort.useStandardTunneling() && mPort.isBufferSupplier() && mPort.isAllocatorPort()) 
			{
				OMX_ERRORTYPE error = mPort.standardTunnelingInit();
				if (error != OMX_ErrorNone) 
				{
					stateTransition( static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateInvalid));
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateCheckRscAvailability RESRESERVATIONPROCESSED_SIG error: %d", error);
					return OMX_ErrorNone; // do not return the error, otherwise the transition is not taken into account
				}
			}
		}
		stateTransition(static_cast<_fnState>(&ImgEns_Port_Fsm::fsmStateLoadedToIdleDSP));
		return OMX_ErrorNone;
	}
	case OMX_USE_SHARED_BUFFER_SIG:
	{
		IMGENS_ASSERT(mPort.getPortRole() == ImgEns_SHARING_PORT);
		// Sharing ports receives buffer from non supplier/allocator Forward this buffer to tunneled port
		ImgEns_CmdUseSharedBufferEvt *useSharedBufferevt = (ImgEns_CmdUseSharedBufferEvt *) evt;
		return mPort.useSharedBuffer(useSharedBufferevt->getAppPrivate(), useSharedBufferevt->getSizeBytes(), useSharedBufferevt->getBufferPtr(), useSharedBufferevt->getSharedChunk());
	}

	case OMX_SETPARAMETER_SIG:
	{
		if (m_BufferSharingEnabled) 
		{
			if (mPort.getPortRole() == ImgEns_SHARING_PORT)
			{
				// Its a sharing port
				OMX_ERRORTYPE error;
				ImgEns_CmdSetParamEvt *setParamEvt = (ImgEns_CmdSetParamEvt *) evt;
				// Only OMX_IndexParamPortDefinition index is supported in this state
				if (setParamEvt->getIndex() != OMX_IndexParamPortDefinition)
					return OMX_ErrorIncorrectStateOperation;
				error = mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
				if (error != OMX_ErrorNone)
					return error;
				error = mPort.sendBufferReqs(OMX_IndexParamPortDefinition, setParamEvt->getStructPtr());
				if (error != OMX_ErrorNone)
					return error;
				return OMX_ErrorNone;
			}
			else
				return OMX_ErrorIncorrectStateOperation;
		}
		else
		{
			return OMX_ErrorIncorrectStateOperation;
		}
	}

	case OMX_GETPARAMETER_SIG:
	{
		if (m_BufferSharingEnabled)
		{
			OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
			OMX_ERRORTYPE error;
			ImgEns_CmdGetParamEvt *getParamEvt = (ImgEns_CmdGetParamEvt *) evt;
			OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
			error = mPort.getParameter(getParamEvt->getIndex(), pComponentParameterStructure);
			if (error != OMX_ErrorNone)
				return error;

			error = mPort.calculateBufferReqs(&maxBufferSize, &maxBufferCountActual);
			if (error != OMX_ErrorNone)
				return error;

			OMX_PARAM_PORTDEFINITIONTYPE *portdef = static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(pComponentParameterStructure);

			// Calculate self requirements
			maxBufferSize        = MAX(portdef->nBufferSize, maxBufferSize);
			maxBufferCountActual = MAX(portdef->nBufferCountActual,maxBufferCountActual);

			portdef->nBufferSize        = maxBufferSize;
			portdef->nBufferCountActual = maxBufferCountActual;

			return OMX_ErrorNone;
		}
		else
		{
			return defaultBehavior(evt);
		}
	}

	case OMX_EMPTY_THIS_BUFFER_SIG:
	case OMX_FILL_THIS_BUFFER_SIG:
	case OMX_EMPTY_BUFFER_DONE_SIG:
	case OMX_FILL_BUFFER_DONE_SIG:
	case OMX_FREE_BUFFER_SIG:
	case OMX_FLUSH_PORT_SIG:
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port_Fsm: OmxStateCheckRscAvailability error: incorrect state operation %d", evt->signal());
		return OMX_ErrorIncorrectStateOperation;
	default:
		return defaultBehavior(evt);
	}
}

void ImgEns_Port_Fsm::copyOMXBufferHeaderFields(const  OMX_BUFFERHEADERTYPE *pSrcBuffer, OMX_BUFFERHEADERTYPE *pDstBuffer) const
{
	pDstBuffer->nFilledLen           = pSrcBuffer->nFilledLen;
	pDstBuffer->nOffset              = pSrcBuffer->nOffset;
	pDstBuffer->nFlags               = pSrcBuffer->nFlags;
	pDstBuffer->nTickCount           = pSrcBuffer->nTickCount;
	pDstBuffer->nTimeStamp           = pSrcBuffer->nTimeStamp;
	pDstBuffer->hMarkTargetComponent = pSrcBuffer->hMarkTargetComponent;
	pDstBuffer->pMarkData            = pSrcBuffer->pMarkData;
}

const s_FsmStateDescription &ImgEns_Port_Fsm::GetStateDescription(const ImgEns_Fsm::_fnState state)
//*************************************************************************************************************
{ //Retreive state description
	const s_FsmStateDescription *pTable=ImgEns_Port_Fsm::TableFsmStatDescription;
	size_t nbr=SIZE_OF_ARRAY(TableFsmStatDescription);
	while(nbr > 0)
	{
		if (state== pTable->fn_state)
			return(*pTable);
		--nbr;
		++pTable;
	}
	OTB_ASSERT(0);
	return(FsmStateDescription_Invalid);

}

