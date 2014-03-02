/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* +CR337836 */
#define DECLARE_AUTOVAR
/* -CR337836 */

#define DBGT_PREFIX "CDefferedEventMgr"


#include "deferredeventmgr.h"
#include <stdio.h>
#include "osi_trace.h"


/* constructor */
CDefferedEventMgr::CDefferedEventMgr()
{
/* +CR337836 */
	GET_AND_SET_TRACE_LEVEL(imgcommon);
/* -CR337836 */

	mIgnoreNonPriorEvents = false;
	mPriorEventTableReadIndex=0;
	mPriorEventTableWriteIndex=0;
	mEventTableReadIndex=0;
	mEventTableWriteIndex=0;

	/*For loop to initialise table entries with null event*/
	for(int i=0;i<NB_MAX_DIFFERED_EVENTS;i++)
	{
		mEventTable[i].sig = NONE_SIG;
		mPriorEventTable[i].sig = NONE_SIG;
	}
}


/**
* \brief 	queues the event it receives in a non-priority queue
* \in  		aEvent : a pointer to s_scf_event containing info about the event to be enqueued
* \out
* \return 	error if the queue is full 
*
*/
OMX_ERRORTYPE CDefferedEventMgr::queueNewEvent(s_scf_event* aEvent)
{
		if(!isEventTableFull())
		{
			mEventTable[mEventTableWriteIndex]=*aEvent;
			mEventTableWriteIndex++;
			mEventTableWriteIndex%=NB_MAX_DIFFERED_EVENTS;
			MSG2("queueNewEvent   signal = %d (%s)\n",aEvent->sig, CError::stringSMSig(aEvent->sig));
			MSG1("queueNewEvent Nb elements in normal queue = %lu\n",getNumberOfElements());
			return OMX_ErrorNone;
		}
		else
		{
			DBGT_ERROR ("queueNewEvent is FULL PLEASE CHECK \n" );
			return OMX_ErrorInsufficientResources;
		}
}


/**
* \brief 	queues the event it receives in a priority queue
* \in  		aEvent : a pointer to s_scf_event containing info about the event to be enqueued
* \out
* \return 	error if the queue is full 
*
*/
OMX_ERRORTYPE CDefferedEventMgr::queuePriorEvent(s_scf_event* aEvent)
{
		//MSG0("Queue Prior Event\n");

		if(!isPriorEventTableFull())
		{
			mPriorEventTable[mPriorEventTableWriteIndex]=*aEvent;
			mPriorEventTableWriteIndex++;
			mPriorEventTableWriteIndex%=NB_MAX_DIFFERED_EVENTS;
			MSG2("queuePriorEvent signal = %d (%s)\n",aEvent->sig, CError::stringSMSig(aEvent->sig));
			MSG1("queuePriorEvent Nb elements in Prior= %lu\n",getPriorNumberOfElements());
			return OMX_ErrorNone;
		}
		else
		{
			DBGT_ERROR ("queuePriorEvent is FULL PLEASE CHECK \n" );
			return OMX_ErrorInsufficientResources;
		}
}


/**
*\brief 	removes a pending event from the non-priority queue
*\in
*\out 		the event dequeued
*\return 	error if the queue is empty
*
*/
void CDefferedEventMgr::dequeuePendingEvent(s_scf_event* aEvent)
{
	MSG1("Nb elements = %lu\n",getNumberOfElements());
	if(!isEventTableEmpty() && (false == mIgnoreNonPriorEvents))
	{
		*aEvent = mEventTable[mEventTableReadIndex];
		DBGT_PDEBUG("Found item in normal queue requests : signal = %d (%s)\n",aEvent->sig, CError::stringSMSig(aEvent->sig));
		DBGT_PDEBUG("New nb elements for normal queue request = %lu\n",getNumberOfElements());
		mEventTableReadIndex++;
		mEventTableReadIndex%=NB_MAX_DIFFERED_EVENTS;
	}
	else
	{
		MSG0("No pending event has been found\n");
		aEvent->sig = SHARED_RETURN_SIG;
	}
}


/**
*\brief 	removes a pending event from the priority queue
*\in
*\out 		the event dequeued
*\return 	error if the queue is empty
*
*/
void CDefferedEventMgr::dequeuePriorEvent(s_scf_event* aEvent)
{
	MSG1("Nb elements = %lu\n",getPriorNumberOfElements());
	if(!isPriorEventTableEmpty())
	{
		*aEvent = mPriorEventTable[mPriorEventTableReadIndex];
		DBGT_PDEBUG("Found item in queued requests : signal = %d (%s)\n",aEvent->sig, CError::stringSMSig(aEvent->sig));
		DBGT_PDEBUG("New nb elements in prior queue = %lu\n",getPriorNumberOfElements());
		mPriorEventTableReadIndex++;
		mPriorEventTableReadIndex%=NB_MAX_DIFFERED_EVENTS;
	}
	else
	{
		MSG0("No PriorEvent pending event has been found\n");
		aEvent->sig = SHARED_RETURN_SIG;
	}
}


/**
* \brief 	queues the event it receives in the non-priority queue before the element to which the current read index points in the buffer
* \in  		aEvent : a pointer to s_scf_event containing info about the event to be enqueued
* \out
* \return 	error if the queue is full 
*
*/
OMX_ERRORTYPE CDefferedEventMgr::requeueEventAtSameLocation(s_scf_event* aEvent)
{
		MSG0("requeueEventAtSameLocation\n");
		if(!isEventTableFull())
		{
			if(mEventTableReadIndex!=0)
			{
				mEventTableReadIndex-=1;
				mEventTable[mEventTableReadIndex]=*aEvent;
			}
			else
			{
				mEventTableReadIndex=NB_MAX_DIFFERED_EVENTS-1;
				mEventTable[mEventTableReadIndex]=*aEvent;
			}
			MSG2("requeueEventAtSameLocation signal = %d (%s)\n",aEvent->sig, CError::stringSMSig(aEvent->sig));
			return OMX_ErrorNone;
		}
		else
		{
			DBGT_ERROR ("requeueEventAtSameLocation is FULL PLEASE CHECK \n" );
			return OMX_ErrorInsufficientResources;
		}
}


/**
* \brief 	queues the event it receives in the non-priority queue before the element to which the current read index points in the buffer
* \in  		aEvent : a pointer to s_scf_event containing info about the event to be enqueued
* \out
* \return 	error if the queue is full 
*
*/
OMX_ERRORTYPE CDefferedEventMgr::requeuePriorEventAtSameLocation(s_scf_event* aEvent)
{
	MSG0("requeuePriorEventAtSameLocation\n");
	if(!isPriorEventTableFull())
	{
		if(mPriorEventTableReadIndex!=0)
		{
			mPriorEventTableReadIndex-=1;
			mPriorEventTable[mPriorEventTableReadIndex]=*aEvent;
		}
		else
		{
			mPriorEventTableReadIndex=NB_MAX_DIFFERED_EVENTS-1;
			mPriorEventTable[mPriorEventTableReadIndex]=*aEvent;
		}
		MSG2("requeuePriorEventAtSameLocation signal = %d (%s)\n",aEvent->sig, CError::stringSMSig(aEvent->sig));
		return OMX_ErrorNone;
	}
	else
	{
		DBGT_ERROR ("requeuePriorEventAtSameLocation is FULL PLEASE CHECK \n" );
		return OMX_ErrorInsufficientResources;
	}
}


/**
*\brief 	checks if both the non-priority and priority queues are empty
*\in
*\out
*\return 	boolean : true if both the queues are empty else false
*
*/
t_bool CDefferedEventMgr::areQueuesEmpty (void)
{
	t_bool bQueuesEmpty = (0 == (getNumberOfElements() + getPriorNumberOfElements()))? true:false;
	return (bQueuesEmpty);
}


/**
*\brief 	sets a boolean to deque the priority queue only and ignore non-prior events
*\in  		aEnable : boolean to enable/disable ignoring of non-prior events
*\out
*\return
*
*/
void CDefferedEventMgr::onlyDequeuePriorEvents (bool aEnable)
{
	mIgnoreNonPriorEvents = aEnable;

}


/**
*\brief 	returns the total number of elements in the non-priority
*\in
*\out
*\return 	the total number of pending elements
*
*/
t_uint32 CDefferedEventMgr::totalNoOfPendingEvents()
{
	MSG1("totalNoOfPendingEvents = %lu\n",getNumberOfElements());
	return (getNumberOfElements());
}


/**
*\brief 	returns the total number of elements in the non-priority and priority queues
*\in
*\out
*\return 	the total number of pending elements in both the queues
*
*/
t_uint32 CDefferedEventMgr::totalNoOfPriorPendingEvents()
{
	MSG1("totalNoOfPriorPendingEvents = %lu\n",getPriorNumberOfElements());
	return (getPriorNumberOfElements());
}


/**
*\brief 	lists the pending events in the non-priority queue
*\in
*\out 		sig of pending events
*\return
*
*/
void CDefferedEventMgr::listPendingEvents()
{
	s_scf_event event;
	t_uint32 mTempEventTableReadIndex=mEventTableReadIndex;
	t_uint32 mTempEventTableWriteIndex=mEventTableWriteIndex;
	if(!isEventTableEmpty())
	{
		while((mTempEventTableReadIndex%NB_MAX_DIFFERED_EVENTS)!=mTempEventTableWriteIndex)
		{
			event=mEventTable[mTempEventTableReadIndex];
			MSG2("signal = %d (%s)\n",event.sig, CError::stringSMSig(event.sig));
			mTempEventTableReadIndex++;
			mTempEventTableReadIndex%=NB_MAX_DIFFERED_EVENTS;
		}
	}
	else
	{
		MSG0("No pending event has been found!!!\n");
	}
}


/**
*\brief 	lists the pending events in the priority queue
*\in
*\out 		sig of pending events
*\return
*
*/
void CDefferedEventMgr::listPriorPendingEvents()
{
	s_scf_event event;
	t_uint32 mTempPriorEventTableReadIndex=mPriorEventTableReadIndex;
	t_uint32 mTempPriorEventTableWriteIndex=mPriorEventTableWriteIndex;

	if(!isPriorEventTableEmpty())
	{
		while((mTempPriorEventTableReadIndex%NB_MAX_DIFFERED_EVENTS)!=mTempPriorEventTableWriteIndex)
		{
			event=mPriorEventTable[mTempPriorEventTableReadIndex];
			MSG2("signal = %d (%s)\n",event.sig, CError::stringSMSig(event.sig));
			mTempPriorEventTableReadIndex++;
			mTempPriorEventTableReadIndex%=NB_MAX_DIFFERED_EVENTS;
		}
	}
	else
	{
		MSG0("No prior pending event has been found!!!\n");
	}
}


/**
*\brief 	checks if the non-priority queue is full
*\in
*\out
*\return 	returns 1 if the queue is full
*
*/
t_bool CDefferedEventMgr::isEventTableFull()
{
		if(((mEventTableWriteIndex+1)%NB_MAX_DIFFERED_EVENTS)==mEventTableReadIndex)
			return true;
		else
			return false;
}


/**
*\brief 	checks if the priority queue is full
*\in
*\out
*\return 	returns 1 if the queue is full
*
*/
t_bool CDefferedEventMgr::isPriorEventTableFull()
{
		if(((mPriorEventTableWriteIndex+1)%NB_MAX_DIFFERED_EVENTS)==mPriorEventTableReadIndex)
			return true;
		else
			return false;
}


/**
*\brief 	checks if the non-priority queue is empty
*\in
*\out
*\return 	returns 1 if the table is empty else 0
*
*/
t_bool CDefferedEventMgr::isEventTableEmpty()
{
	if(((mEventTableReadIndex)%NB_MAX_DIFFERED_EVENTS)==mEventTableWriteIndex)
			return true;
	else
			return false;
}


/**
*\brief 	checks if the priority queue is empty
*\in
*\out
*\return 	returns 1 if the table is empty else 0
*
*/
t_bool CDefferedEventMgr::isPriorEventTableEmpty()
{
	if(((mPriorEventTableReadIndex)%NB_MAX_DIFFERED_EVENTS)==mPriorEventTableWriteIndex)
		return true;
	else
		return false;
}


/**
*\brief 	returns the number of elements in the non-priority queue
*\in
*\out
*\return 	returns the number of elements
*
*/
t_uint32 CDefferedEventMgr::getNumberOfElements()
{
	t_uint32 count=0;
	t_uint32 mTempEventTableReadIndex=mEventTableReadIndex;
	t_uint32 mTempEventTableWriteIndex=mEventTableWriteIndex;
	while(mTempEventTableReadIndex != mTempEventTableWriteIndex)
	{
		count++;
		mTempEventTableReadIndex++;
		mTempEventTableReadIndex%=NB_MAX_DIFFERED_EVENTS;
	}
	return count;
}


/**
*\brief 	returns the number of elements in the priority queue
*\in
*\out
*\return 	returns the number of elements
*
*/
t_uint32 CDefferedEventMgr::getPriorNumberOfElements()
{
	t_uint32 count=0;
	t_uint32 mTempPriorEventTableReadIndex=mPriorEventTableReadIndex;
	t_uint32 mTempPriorEventTableWriteIndex=mPriorEventTableWriteIndex;

	while((mTempPriorEventTableReadIndex%NB_MAX_DIFFERED_EVENTS)!=mTempPriorEventTableWriteIndex)
	{
		count++;
		mTempPriorEventTableReadIndex%=NB_MAX_DIFFERED_EVENTS;
		mTempPriorEventTableReadIndex++;
	}
	return count;
}


OMX_ERRORTYPE CDefferedEventMgr::queueZoomEvent(s_scf_event * aEvent, t_bool * aNewEventQueued)
{
    s_scf_event event;
    t_uint32 tempEventTableReadIndex = mEventTableReadIndex;
    t_uint32 tempEventTableWriteIndex = mEventTableWriteIndex;
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    bool bSimilarEventFound = false;

   *aNewEventQueued = false;

    if(!isEventTableEmpty())
    {
        while(tempEventTableReadIndex != tempEventTableWriteIndex)
        {
            event = mEventTable[tempEventTableReadIndex];
            if((event.sig == aEvent->sig) && (event.args.omxsetConfigIndex == aEvent->args.omxsetConfigIndex))
            {
                bSimilarEventFound = true;
                MSG1("Similar zoom event found at index %lu, Not queuing the Event\n", tempEventTableReadIndex);
                break;
            }
            tempEventTableReadIndex++;
            tempEventTableReadIndex %= NB_MAX_DIFFERED_EVENTS;
        }
    }

    if(bSimilarEventFound == false)
    {
        MSG0("Similar Zoom event not found, so queuing new event\n");
        ret = queueNewEvent(aEvent);
        *aNewEventQueued = true;
    }

    return ret;
}

