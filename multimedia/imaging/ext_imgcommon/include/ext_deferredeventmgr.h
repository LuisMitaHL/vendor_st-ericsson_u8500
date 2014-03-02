/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _DEFEREREDEVENTMGR_H_
#define _DEFEREREDEVENTMGR_H_

#undef LOG_TAG
#include "OMX_Index.h"
#include "ENS_Component.h"
#include "ENS_List.h"
#include "ext_qevent.h"
#include "ext_error.h"

#undef   LOG_TAG
#define  LOG_TAG DBGT_TAG
#undef DBGT_LAYER
#define DBGT_LAYER 3
#undef DBGT_PREFIX
#define DBGT_PREFIX "GRBC"
#include "debug_trace.h"

#define NB_MAX_DIFFERED_EVENTS 16


/* for traces */

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CDefferedEventMgr);
#endif
class CDefferedEventMgr
{
	public :

		/* constructor */
		CDefferedEventMgr(){
			eventTableIndex=-1;
			priorEventCounter=0;
			eventCounter=0;
		}

		s_scf_event nullEvent;
		ENS_List eventsQueue;
		ENS_List priorityEventsQueue;


		void queueNewEvent(s_scf_event* event){
				DBGT_PTRACE("Queue Event\n");
				eventsQueue.pushBack(event);
				DBGT_PTRACE("signal = %d (%s)\n",event->sig, CError::stringSMSig(event->sig));
				DBGT_PTRACE("Nb elements = %d\n",eventsQueue.getSize());
				eventCounter++;

		}

		void queuePriorEvent(s_scf_event* event){
				DBGT_PTRACE("Queue Prior Event\n");
				priorityEventsQueue.pushBack(event);
				DBGT_PTRACE("signal = %d (%s)\n",event->sig, CError::stringSMSig(event->sig));
				DBGT_PTRACE("Nb elements = %d\n",priorityEventsQueue.getSize());
				priorEventCounter++;
		}

		s_scf_event * dequeuePendingEvent(void){
			DBGT_PTRACE("Nb elements = %d\n",eventsQueue.getSize());
			if (eventsQueue.getSize()!=0)
			{
				DBGT_PTRACE("Size is not empty\n");
				s_scf_event* event = (s_scf_event *)eventsQueue.popFront();
				DBC_ASSERT((int)event);
				DBGT_PTRACE("Found item in queued requests : signal = %d (%s)\n",event->sig, CError::stringSMSig(event->sig));
				DBGT_PTRACE("New nb elements = %d\n",eventsQueue.getSize());
				eventCounter --;
				return event;
			}
			else
			{
				/* checks that there is indeed no more events in the queue */
				DBC_ASSERT(eventCounter==0);
				DBGT_PTRACE("No pending event has been found\n");
				nullEvent.sig = SHARED_RETURN_SIG;
				return &nullEvent;
			}
		}

		void requeueEventAtSameLocation(s_scf_event* event){
				DBGT_PTRACE("requeueEventAtSameLocation\n");
				eventsQueue.pushFront(event);
				DBGT_PTRACE("signal = %d (%s)\n",event->sig, CError::stringSMSig(event->sig));
				DBGT_PTRACE("Nb elements = %d\n",eventsQueue.getSize());
				eventCounter++;
		}

		void requeuePriorEventAtSameLocation(s_scf_event* event){
				DBGT_PTRACE("requeueEventAtSameLocation\n");
				priorityEventsQueue.pushFront(event);
				DBGT_PTRACE("signal = %d (%s)\n",event->sig, CError::stringSMSig(event->sig));
				DBGT_PTRACE("Nb elements = %d\n",priorityEventsQueue.getSize());
				priorEventCounter++;
		}

		s_scf_event * dequeuePriorEvent(void){
			DBGT_PTRACE("Nb elements = %d\n",priorityEventsQueue.getSize());
			if (priorityEventsQueue.getSize()!=0)
			{
				s_scf_event* event = (s_scf_event *)priorityEventsQueue.popFront();
				DBGT_PTRACE("Found item in prior queued requests : signal = %d (%s)\n",event->sig,CError::stringSMSig(event->sig));
				DBGT_PTRACE("New nb elements  = %d\n",priorityEventsQueue.getSize());
				priorEventCounter --;
				return event;
			}
			else
			{
				/* checks that there is indeed no more events in the queue */
				DBC_ASSERT(priorEventCounter==0);
				DBGT_PTRACE("No prior pending event has been found\n");
				nullEvent.sig = SHARED_RETURN_SIG;
				return &nullEvent;
			}
		}


		s_scf_event * getEmptyEvent(void) {
			eventTableIndex = (eventTableIndex+1)%NB_MAX_DIFFERED_EVENTS;
			DBGT_PTRACE("eventTableIndex = %ld\n",eventTableIndex);
			eventTable[eventTableIndex].sig =NONE_SIG;
			eventTable[eventTableIndex].type.other = 0;
			DBGT_PTRACE("@eventTable[eventTableIndex] = 0x%lx\n",(OMX_U32)&eventTable[eventTableIndex]);
			return(&eventTable[eventTableIndex]);
			}

		t_bool areQueuesEmpty (void) {
			t_bool bQueuesEmpty = (0 == (eventCounter + priorEventCounter))? true:false;
			return (bQueuesEmpty);
		}

	protected:

		s_scf_event 	eventTable[NB_MAX_DIFFERED_EVENTS];
		t_sint32 eventTableIndex;

		/* these counters are used for checks */
		t_uint32 priorEventCounter;
		t_uint32 eventCounter;


};


#endif
