/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#ifndef _IMG_ENS_PORT_FSM_H
#define _IMG_ENS_PORT_FSM_H

#include "ImgEns_Fsm.h"
#include "ImgEns_FsmEvent.h"
#include "ImgEns_Port.h"

class ImgEns_Port;
class Img_ProcessingComponent;

class IMGENS_API ImgEns_Port_Fsm: public ImgEns_Fsm, public Img_TraceObject
{
public:
	ImgEns_Port_Fsm(ImgEns_Port &port);
	virtual int NotifyChangeState(const _fnState currentState, const _fnState nextState) const;

	OMX_STATETYPE getState(void) const;

	void emptybufferdone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
	void fillbufferdone_cb(OMX_BUFFERHEADERTYPE* pBuffer);

	enum
	{
		eStateDisabled=-1,
	};
	static const s_FsmStateDescription &GetStateDescription(const _fnState state);
	typedef OMX_ERRORTYPE (ImgEns_Port_Fsm::* _fnStatePort)(const ImgEns_FsmEvent *);
protected:
	ImgEns_Port   &mPort;
	OMX_STATETYPE mDisabledState;
	OMX_BOOL      mWaitBuffersFromNeighbor;
	OMX_BOOL      mWaitBuffersToNeighbor;
	OMX_BOOL      mEventAppliedOnEachPorts;

	// default behavior for all states of the FSM
	OMX_ERRORTYPE defaultBehavior(const ImgEns_FsmEvent *);

	// Main OMX IL States
	OMX_ERRORTYPE fsmStateLoaded   (const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateIdle     (const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateExecuting(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStatePause    (const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateInvalid  (const ImgEns_FsmEvent *);

	// Transient States
	OMX_ERRORTYPE fsmStateLoadedToIdleHOST(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateLoadedToIdleDSP(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateIdleToLoaded(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateTransientToIdle(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateTransient(const ImgEns_FsmEvent *);

	OMX_ERRORTYPE fsmStateTransientToDisableWaitBuffersFromNeighbor(const ImgEns_FsmEvent *evt);
	OMX_ERRORTYPE fsmStateTransientToDisableWaitForFreeBuffers(const ImgEns_FsmEvent *evt);
	OMX_ERRORTYPE fsmStateTransientToDisableDSP(const ImgEns_FsmEvent *evt);
	OMX_ERRORTYPE fsmStateTransientToEnableHOST(const ImgEns_FsmEvent *evt);
	OMX_ERRORTYPE fsmStateTransientToEnableDSP(const ImgEns_FsmEvent *evt);
	OMX_ERRORTYPE fsmStateDisabled(const ImgEns_FsmEvent *evt);

	OMX_ERRORTYPE fsmStateLoadedToTestForResources(const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateTestForResources    (const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateWaitForResources    (const ImgEns_FsmEvent *);
	OMX_ERRORTYPE fsmStateCheckRscAvailability(const ImgEns_FsmEvent *);

	OMX_ERRORTYPE goLoadedToIdle(void);
	OMX_ERRORTYPE goIdleToLoaded(void);
	OMX_ERRORTYPE goToEnable(OMX_U32 portIndex);
	OMX_ERRORTYPE goToDisable(OMX_STATETYPE currentState, OMX_U32 portIndex);
	_fnState getFsmParentState(OMX_STATETYPE parentState);

	/// Standard Tunneling Initialization :
	/// Each supplier port shall allocate and pass its buffers to the non-
	/// supplier port it is tunneling.
	/// @return OMX_ERRORTYPE
	OMX_ERRORTYPE checkStandardTunnelingInit(void);

	/// Standard Tunneling De-Initialization :
	/// Each supplier port shall call FreeBuffer method on its tunneled Component
	/// and frees its buffers.
	/// @return OMX_ERRORTYPE
	OMX_ERRORTYPE checkStandardTunnelingDeInit(void);

	/// Copies OMX_BUFFERHEADERTYPE fields from source buffer to destination buffer.
	/// It is used in buffer sharing in which we need to map buffers between two connected ports.
	void copyOMXBufferHeaderFields(const OMX_BUFFERHEADERTYPE *pSrcBuffer, OMX_BUFFERHEADERTYPE *pDstBuffer) const ;

	OMX_ERRORTYPE eventHandlerCB(OMX_EVENTTYPE ev, OMX_U32 data1, OMX_U32 data2);

	Img_ProcessingComponent & getProcessingComponent() const;

public:
	//Note: This flag will be removed in future
	void bufferSharingEnabled(bool enable=true) { m_BufferSharingEnabled = enable;}
protected:
	bool m_BufferSharingEnabled;

	static const s_FsmStateDescription TableFsmStatDescription[];
};

#endif // _IMG_ENS_PORT_FSM_H
