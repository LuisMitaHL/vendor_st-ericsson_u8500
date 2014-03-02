/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "wom_shared.h"
#include <stdio.h>
#include <string.h>

#include <omxil/OMX_Types.h>
#include <omxil/OMX_Core.h>
#include "wom_fsm.h"

WOM_API WomFsm::WomFsm()
//*************************************************************************************************************
{
	m_StateHandlerFunction = &WomFsm::StateHandler_Null;
	m_bDispatching         = false;
}

WOM_API WomFsm::~WomFsm()
//*************************************************************************************************************
{
}

WOM_API void WomFsm::init(t_FsmState initialState)
//*************************************************************************************************************
{
	// check that the initial m_StateHandlerFunction is entered only once
	//WOM_ASSERT(m_StateHandlerFunction == 0);
	m_StateHandlerFunction = initialState;

	WomFsmEvent localEvt;
	localEvt.signal = eSignal_Entry;

	// enter initial m_StateHandlerFunction
	(this->*m_StateHandlerFunction)(&localEvt);
} /* init */


WOM_API void WomFsm::dispatch(const WomFsmEvent *evt)
//*************************************************************************************************************
{
	WomFsmEvent localEvt;
	t_FsmState currentState;

	// check that we don't dispatch an event while already processing an event
	WOM_ASSERT(!m_bDispatching); //"WomFsm is already m_bDispatching an event"
	m_bDispatching = true;

	WOM_ASSERT(m_StateHandlerFunction != 0);

	currentState            = m_StateHandlerFunction;
	m_StateHandlerFunction  = 0;

	(this->*currentState)(evt);

	if (m_StateHandlerFunction != 0)
	{
		// m_StateHandlerFunction transition taken so exit the source m_StateHandlerFunction
		localEvt.signal = eSignal_Exit;
		(this->*currentState)(&localEvt);

		// enter target m_StateHandlerFunction
		localEvt.signal = eSignal_Entry;
		(this->*m_StateHandlerFunction)(&localEvt);
	}
	else
	{
		m_StateHandlerFunction = currentState;
	}

	m_bDispatching = false;
}
