/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#ifndef _IMG_ENS_COMPONENT_FSM_H
#define _IMG_ENS_COMPONENT_FSM_H

#include <omxil/OMX_Component.h>
#include <omxil/OMX_Core.h>
#include "ImgEns_Component.h"
#include "ImgEns_Port_Fsm.h"
#include "ImgEns_Fsm.h"
#include "ImgEns_FsmEvent.h"

class IMGENS_API ImgEns_Component_Fsm: public ImgEns_Fsm, public Img_TraceObject
{
public:
	ImgEns_Component_Fsm(ImgEns_Component &ensComp);

	void dspEventHandler(OMX_EVENTTYPE ev, OMX_U32 data1, OMX_U32 data2);
	OMX_STATETYPE getOMXState() const;
	OMX_STATETYPE getState() const;

	virtual int NotifyChangeState(const _fnState currentState, const _fnState nextState)  const;
protected:
	ImgEns_Component & mEnsComponent;
	OMX_STATETYPE      mTransientState;
	OMX_STATETYPE      mTargetState;

	// default behavior for all states
	OMX_ERRORTYPE defaultBehavior(const ImgEns_FsmEvent *);

	// Main OMX IL States
	OMX_ERRORTYPE FsmState_Loaded(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_Idle(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_Executing(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_Pause(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_Invalid(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_TestForResources(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_WaitForResources(const ImgEns_FsmEvent *);

	// OMX IL RM Sub-States
	OMX_ERRORTYPE FsmState_IdleSuspended(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_PauseSuspended(const ImgEns_FsmEvent *);

	// Transient States
	OMX_ERRORTYPE FsmState_LoadedToIdleHOST(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_LoadedToIdleDSP(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_IdleToLoaded(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_Transient(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_LoadedToTestForResources(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE FsmState_CheckRscAvailability(const ImgEns_FsmEvent *);

	OMX_ERRORTYPE setState(const ImgEns_FsmEvent *, _fnState currentState);
	OMX_ERRORTYPE dispatchEvtAndUpdateState(const ImgEns_FsmEvent *, _fnState currentState);
	void updateState(_fnState currentState);
	void setTransientState(_fnState currentState, OMX_STATETYPE targetState);
	void translateTransientState(OMX_STATETYPE &previousState, OMX_STATETYPE &targetState) const;
	OMX_ERRORTYPE goIdleToLoaded(void);
	OMX_ERRORTYPE dispatchEvtOnPorts(const ImgEns_FsmEvent *evt);

	OMX_BOOL isPortEnabled(const OMX_U32 portidx) const;
	OMX_STATETYPE portGetState(const OMX_U32 portIdx) const;

	ImgEns_Port* getPort(const OMX_U32 portIdx) const;
	ImgEns_Port_Fsm * getPortFsm(const OMX_U32 portIdx) const;
	OMX_ERRORTYPE DispatchToPort(const OMX_U32 portIdx, const  ImgEns_FsmEvent*evt) const;
	OMX_ERRORTYPE Dispatch(const  ImgEns_FsmEvent*evt); //Override with omx return type

	OMX_STATETYPE getState(const _fnState& st) const;
public:
	//Note: This flag will be removed in future
	bool m_BufferSharingEnabled;
	void bufferSharingEnabled(bool enable = true) { m_BufferSharingEnabled = enable;}
	
	static const s_FsmStateDescription &GetStateDescription(const _fnState state);
	static const s_FsmStateDescription TableFsmStatDescription[];

protected:
	OMX_ERRORTYPE suspendResources(void);
	OMX_ERRORTYPE unsuspendResources(void);
};


#endif // _IMG_ENS_COMPONENT_FSM_H
