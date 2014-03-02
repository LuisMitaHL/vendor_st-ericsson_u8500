/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _DEFEREREDEVENTMGR_H_
#define _DEFEREREDEVENTMGR_H_

#include "OMX_Index.h"
#include "qevent.h"
#include "OMX_Core.h"
#include "error.h"

#define NB_MAX_DIFFERED_EVENTS (16+1)	//since circular queue has been implemented with 1 slot vacant

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CDefferedEventMgr);
#endif


class CDefferedEventMgr
{

	public :

		CDefferedEventMgr();
		OMX_ERRORTYPE queueNewEvent(s_scf_event* aEvent);
		OMX_ERRORTYPE queuePriorEvent(s_scf_event* aEvent);
		void dequeuePendingEvent(s_scf_event* aEvent);
		void dequeuePriorEvent(s_scf_event* aEvent);
		OMX_ERRORTYPE requeueEventAtSameLocation(s_scf_event* aEvent);
		OMX_ERRORTYPE requeuePriorEventAtSameLocation(s_scf_event* aEvent);
		t_bool areQueuesEmpty (void);
		void onlyDequeuePriorEvents (bool aEnable);
		t_uint32 totalNoOfPendingEvents();
		t_uint32 totalNoOfPriorPendingEvents();
		void listPendingEvents();
		void listPriorPendingEvents();
		OMX_ERRORTYPE queueZoomEvent(s_scf_event * aEvent, t_bool * aNewEventQueued);

	protected:

		struct s_scf_event mEventTable[NB_MAX_DIFFERED_EVENTS];		//circular buffer for non priority events
		struct s_scf_event mPriorEventTable[NB_MAX_DIFFERED_EVENTS];	//circular buffer for priority events
		t_uint32 mEventTableReadIndex;						//Index to read for mEventTable
		t_uint32 mEventTableWriteIndex;						//Index to write for mEventTable
		t_uint32 mPriorEventTableReadIndex;					//Index to read for mPriorEventTable
		t_uint32 mPriorEventTableWriteIndex;					//Index to write for mPriorEventTable
		bool 	 mIgnoreNonPriorEvents;							//ignore non-prior events when true

	private:
		t_bool isEventTableFull();
		t_bool isPriorEventTableFull();
		t_bool isEventTableEmpty();
		t_bool isPriorEventTableEmpty();
		t_uint32 getNumberOfElements();
		t_uint32 getPriorNumberOfElements();
};

#endif
