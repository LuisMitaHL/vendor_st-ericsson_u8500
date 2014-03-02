/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_FSM_H_
#define _WOM_FSM_H_


typedef struct
{
	int       signal;
	//void *    pUser;
} WomFsmEvent;

typedef struct
{
	WomFsmEvent fsmEvent;
	union
	{
		struct
		{
			OMX_BUFFERHEADERTYPE *buffer;
			unsigned int          portIdx;
		} returnBuffer;
		struct
		{
			OMX_STATETYPE   state;
		} setState;
		struct
		{
			unsigned  int  portIdx;
		} portCmd;
	} args;
} WomOmxEvent;


class WOM_API WomFsm
//*************************************************************************************************************
{
public:
	WomFsm();
	virtual ~WomFsm() = 0;

	// Definition of the state function pointer
	typedef void (WomFsm::*t_FsmState)(const WomFsmEvent *);

	void init(t_FsmState initialState);
	void dispatch(const WomFsmEvent *evt);

	t_FsmState getState() const { return(m_StateHandlerFunction); } /* getState */

	bool isDispatching() const { return (m_bDispatching); }
	void StateHandler_Null(const WomFsmEvent *) { WOM_ASSERT(0); }  // The default state handler du nothing

	enum FsmSignal
	{
		// specific signals
		eSignal_Entry,
		eSignal_Exit,
		// start of user defined signals
		eSignal_User,
		eSignal_LastEnum, // First user extension
	};

protected:
	t_FsmState m_StateHandlerFunction;
	bool       m_bDispatching;
}; /* WomFsm */

#endif //_WOM_FSM_H_
