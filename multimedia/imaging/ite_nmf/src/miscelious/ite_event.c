/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


 /***************************************************************
 * Queueing mechanism. The Event Queue was created to solve the problem
 * of many NMF Callback functions setting various globals for use in the
 * non-callback code.
 * The Event Queue centralises information reported by the callbacks,
 * when appropriate.
 * There is an important assumption regarding locking the queue:
 * only callbacks can add events, and only non-callback code
 * can remove events. Callbacks are called in interrupt context and
 * will interrupt non-callback code, but callbacks won't interrupt
 * each other. Hence, when non-callback code wants to remove an event,
 * it must lock the queue. This is done simply by masking interrupts.
 * It was inspired by the SDL event queue.
 ***************************************************************/
#include "ite_event.h"
#include "cm/inc/cm_type.h" /* provided by nmf. Used to t_uint* types */


/*!!!  WARNING !!! Class ce define ;) */
#define VIC_IMAGING_IT_0_LINE 42

#include <los/api/los_api.h>
#include <stdio.h>
#ifdef __ARM_LINUX
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#endif //__ARM_LINUX
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ite_vpip.h"
#include <test/api/test.h>
#include "ite_nmf_standard_functions.h"
#include "ite_testenv_utils.h"
#include "ite_recordsystem_time.h"


static t_uint16 g_Event0_Count[32],g_Event1_Count[32],g_Event2_Count[32],g_Event3_Count[32];


static struct s_ITE_EventQ{
    int head; /* first valid event (except if queue empty) */
    int tail; /* next empty space (except if queue is full) */
	t_los_sem_id ite_event_semid;	/* semaphore identifier to provide sync */
    union u_ITE_Event events[ITE_EVENT_QUEUE_SIZE];
} ITE_EventQ;

static void ITE_DelEvent(int index);
//static int ITE_FindEvent(t_uint16 event_type);

//t_uint16  g_NumberEventsRecv=0;
//t_uint16* g_pNumberEventsRecv = &g_NumberEventsRecv;
t_uint8 ITE_Event_Log_flag=TRUE;
t_uint8 g_timeout=FALSE;


/************************************************/
/* void ITE_Log_Event_enable(void)  */
/*                      */
/* set flag to tree             */
/************************************************/
void ITE_Log_Event_enable(void)
{
   ITE_Event_Log_flag=TRUE;
}
/************************************************/
/* void ITE_Log_Event_disable(void) */
/*                      */
/* set flag to tree             */
/************************************************/
void ITE_Log_Event_disable(void)
{
   ITE_Event_Log_flag=FALSE;
}
/* Display all event from the queue.
 * Returns 0/FALSE if fail
 */

int ITE_DisplayEvent()
{
    int start;
    //LOS_Log(" STE ITE_DisplayEvent - \n");

    for (start = ITE_EventQ.head; start != ITE_EventQ.tail; start = ((start+1)%ITE_EVENT_QUEUE_SIZE))
    {
        LOS_Log(" **** Events in queue **** %d \n",ITE_EventQ.events[start].type);
        ITE_traceISPCTLevent(ITE_EventQ.events[start]);
    }
    return 0;

}

/* add an event from the queue. It is assumed this is always done
 * from (uninterruptible) callback context, hence no locking
 * is needed.
 * Returns 0/FALSE if fail
 */

int ITE_AddEvent(union u_ITE_Event event)
{
    int tail;
	LOS_SemaphoreWait(ITE_EventQ.ite_event_semid);
    tail = (ITE_EventQ.tail+1)%ITE_EVENT_QUEUE_SIZE;

    if (tail == ITE_EventQ.head) {
        /* overflow */
		LOS_SemaphoreNotify(ITE_EventQ.ite_event_semid);
        return FALSE;
    } else {
        ITE_EventQ.events[ITE_EventQ.tail] = event;
        ITE_EventQ.tail = tail;
		LOS_SemaphoreNotify(ITE_EventQ.ite_event_semid);
        return TRUE;
    }
}

/* remove an event from the queue. It is assumed that this is always done
 * from outside callback context, hence is interruptible, and hence
 * must disable interrupts.
 */
static void ITE_DelEvent(int index)
{

    if (index == ITE_EventQ.head) {
        ITE_EventQ.head = (ITE_EventQ.head+1)%ITE_EVENT_QUEUE_SIZE;
        return;
    } else if (((index+1)%ITE_EVENT_QUEUE_SIZE) == ITE_EventQ.tail) {
        ITE_EventQ.events[index] = ITE_EventQ.events[ITE_EventQ.tail];
        ITE_EventQ.tail = index;
        return;
    } else {
        /* we're in the middle. shift subsequent events (between index and tail) down */
        int here, next;
        if (--ITE_EventQ.tail < 0) ITE_EventQ.tail = ITE_EVENT_QUEUE_SIZE-1;

        for (here = index; here != ITE_EventQ.tail; here = next) {
            next = (here+1)%ITE_EVENT_QUEUE_SIZE;
            ITE_EventQ.events[here] = ITE_EventQ.events[next];
        }
    }
}



int ITE_FindEvent(t_uint16 event_type)
{
    int i;

    for (i = ITE_EventQ.head; i != ITE_EventQ.tail; i = (i+1)%ITE_EVENT_QUEUE_SIZE) {
        /* Note: yes, I could've made event_type into bit flags, but
         * non-bit values are more useful when debugging, at the expense
               * of a bit of ugly code :p
               */
        if (
             (ITE_EventQ.events[i].type == event_type)

            || ((event_type == ITE_EVT_ALL_EVENTS) &&
             (ITE_EventQ.events[i].type != ITE_EVT_NONE))
			|| ((event_type == ITE_EVT_SIARM_EVENTS)
               && ((ITE_EventQ.events[i].type == ITE_EVT_SIARM_INFO)
                   || (ITE_EventQ.events[i].type == ITE_EVT_SIARM_ERROR)
                   || (ITE_EventQ.events[i].type == ITE_EVT_SIARM_DEBUG)))
			|| ((event_type == ITE_EVT_ISPCTL_EVENTS)
               && ((ITE_EventQ.events[i].type == ITE_EVT_ISPCTL_INFO)
                   || (ITE_EventQ.events[i].type == ITE_EVT_ISPCTL_ERROR)
                   || (ITE_EventQ.events[i].type == ITE_EVT_ISPCTL_DEBUG)))
			|| ((event_type == ITE_EVT_GRABLR_EVENTS)
               && ((ITE_EventQ.events[i].type == ITE_EVT_GRABLR_INFO)
                   || (ITE_EventQ.events[i].type == ITE_EVT_GRABLR_ERROR)
                   || (ITE_EventQ.events[i].type == ITE_EVT_GRABLR_DEBUG)))
			|| ((event_type == ITE_EVT_GRABHR_EVENTS)
               && ((ITE_EventQ.events[i].type == ITE_EVT_GRABHR_INFO)
                   || (ITE_EventQ.events[i].type == ITE_EVT_GRABHR_ERROR)
                   || (ITE_EventQ.events[i].type == ITE_EVT_GRABHR_DEBUG)))
			|| ((event_type == ITE_EVT_GRABBMS_EVENTS)
               && ((ITE_EventQ.events[i].type == ITE_EVT_GRABBMS_INFO)
                   || (ITE_EventQ.events[i].type == ITE_EVT_GRABBMS_ERROR)
                   || (ITE_EventQ.events[i].type == ITE_EVT_GRABBMS_DEBUG)))
			|| ((event_type == ITE_EVT_GRABBML_EVENTS)
               && ((ITE_EventQ.events[i].type == ITE_EVT_GRABBML_INFO)
                   || (ITE_EventQ.events[i].type == ITE_EVT_GRABBML_ERROR)
                   || (ITE_EventQ.events[i].type == ITE_EVT_GRABBML_DEBUG)))
                   )
         {
                return i;
    }
    }

    return -1;
}

union u_ITE_Event ITE_GetEvent(t_uint16 event_type)
{
    union u_ITE_Event event;
    int index;
    event.type = ITE_EVT_NONE;
	   LOS_SemaphoreWait(ITE_EventQ.ite_event_semid);

    index = ITE_FindEvent(event_type);

    if (index != -1)
    {
     event = ITE_EventQ.events[index];
     ITE_DelEvent(index);
    }

	   LOS_SemaphoreNotify(ITE_EventQ.ite_event_semid);
    return event;
}

union u_ITE_Event ITE_GetZoomEvent(void)
{
    union u_ITE_Event event;
    int index;

    event.type = ITE_EVT_NONE;
   // event.ispctlInfo.info_id = ISP_INF_NONE;

    LOS_SemaphoreWait(ITE_EventQ.ite_event_semid);

    index = ITE_FindEvent(ITE_EVT_ISPCTL_ZOOM_EVENTS);

    if (index != -1)
    {
        event = ITE_EventQ.events[index];
        ITE_DelEvent(index);
    }

	   LOS_SemaphoreNotify(ITE_EventQ.ite_event_semid);

    return event;
}


union u_ITE_Event ITE_GetStatsEvent()
{
    union u_ITE_Event event;
    int index;

    event.type = ITE_EVT_NONE;
    event.ispctlInfo.info_id = ISP_INF_NONE;

	LOS_SemaphoreWait(ITE_EventQ.ite_event_semid);

    index = ITE_FindEvent(ITE_EVT_ISPCTL_STATS_EVENTS);

    if (index != -1)
    {
        event = ITE_EventQ.events[index];
        ITE_DelEvent(index);
    }

	LOS_SemaphoreNotify(ITE_EventQ.ite_event_semid);

    return event;
}



void ITE_CleanEvent(t_uint16 event_type)
{
    int index;
	LOS_SemaphoreWait(ITE_EventQ.ite_event_semid);

    do {
        index = ITE_FindEvent(event_type);
    if (index!=-1) ITE_DelEvent(index);
    } while (index!=-1);
	LOS_SemaphoreNotify(ITE_EventQ.ite_event_semid);
}

union u_ITE_Event ITE_WaitEvent(t_uint16 event_type)
{
    union u_ITE_Event event;

    do {
        event = ITE_GetEvent(event_type);
      		// Adding a 1 ms delay so as not to have a tight loop; this would let other threads process the event queue, if needed
      		LOS_Sleep(1);
    } while (event.type == ITE_EVT_NONE);

    return event;
}



t_uint8 ITE_ManagEvent(union u_ITE_Event event)
{

    t_uint16 error;

    switch (event.type)
        {
            case ITE_EVT_GRABHR_INFO:
                error = FALSE;
            break;

            case ITE_EVT_GRABHR_ERROR:
                LOS_Log("|EVT: !!!!!! ITE_EVT_GRABHR_ERROR : %d !!!!!  \n", event.grabHRError.error_id, NULL, NULL, NULL,NULL,NULL);
                error = TRUE;
            break;

            case ITE_EVT_GRABHR_DEBUG:
                LOS_Log("|EVT: ITE_EVT_GRABHR_DEBUG : %d \n",  event.grabHRDebug.debug_id, NULL,NULL,NULL,NULL,NULL);
                error = TRUE;
            break;

            case ITE_EVT_GRABLR_INFO:
                error = FALSE;
            break;

            case ITE_EVT_GRABLR_ERROR:
                LOS_Log("|EVT: !!!!!! ITE_EVT_GRABLR_ERROR: %d !!!!!\n", event.grabLRError.error_id, NULL, NULL, NULL,NULL,NULL);
                error = TRUE;
            break;

            case ITE_EVT_GRABLR_DEBUG:
                LOS_Log("|EVT: ITE_EVT_GRABLR_DEBUG\n",  event.grabLRDebug.debug_id, NULL,NULL,NULL,NULL,NULL);
                error = TRUE;
            break;

        case ITE_EVT_GRABBMS_INFO:
                error = FALSE;
            break;

            case ITE_EVT_GRABBMS_ERROR:
                LOS_Log("|EVT: !!!!!! ITE_EVT_GRABBMS_ERROR: %d !!!!!\n", event.grabBMSError.error_id, NULL, NULL, NULL,NULL,NULL);
                error = TRUE;
            break;

            case ITE_EVT_GRABBMS_DEBUG:
                LOS_Log("|EVT: ITE_EVT_GRABBMS_DEBUG\n",  event.grabBMSDebug.debug_id, NULL,NULL,NULL,NULL,NULL);
                error = TRUE;
            break;

        case ITE_EVT_GRABBML_INFO:
                error = FALSE;
            break;

            case ITE_EVT_GRABBML_ERROR:
                LOS_Log("|EVT: !!!!!! ITE_EVT_GRABBML_ERROR: %d !!!!!\n", event.grabBMLError.error_id, NULL, NULL, NULL,NULL,NULL);
                error = TRUE;
            break;

            case ITE_EVT_GRABBML_DEBUG:
                LOS_Log("|EVT: ITE_EVT_GRABBML_DEBUG\n",  event.grabBMLDebug.debug_id, NULL,NULL,NULL,NULL,NULL);
                error = TRUE;
            break;

            case ITE_EVT_ISPCTL_INFO:
                error = FALSE;
            break;

            case ITE_EVT_ISPCTL_ERROR:
                LOS_Log("|EVT: !!!!!! ITE_EVT_ISPCTL_ERROR !!!!!\n", NULL, NULL, NULL, NULL,NULL,NULL);
                error = TRUE;
            break;

            case ITE_EVT_ISPCTL_DEBUG:
                LOS_Log("|EVT: ITE_EVT_ISPCTL_DEBUG\n",  NULL, NULL,NULL,NULL,NULL,NULL);
                error = TRUE;
            break;

            case ITE_EVT_NONE:
                LOS_Log("|EVT: !!!!! TIMEOUT !!!!\n",  NULL, NULL,NULL,NULL,NULL,NULL);
                error = TRUE;
            break;

            default:
                LOS_Log("Unexpected event type %i\n",event.type,
                                                        NULL,NULL,NULL,NULL,NULL);
            error = TRUE;
            break;
        }


    return error;
}
/* time out handler  */
void ITE_timouthandler(t_uint32 param)
{
    UNUSED(param);
    g_timeout=TRUE;
}


t_uint8 ITE_WaitEventTimeout(t_uint16 event_type, int timeout)
{
    t_uint8 error=TRUE;
    t_uint32 param = 0;
    t_uint32 handle;
    union u_ITE_Event event;

    g_timeout=FALSE;
    UNUSED(handle);
    UNUSED(param);


    fprintf(stderr, "Waiting %d seconds for alarm...",timeout/1000);
    do
    {
    event = ITE_GetEvent(event_type);
    timeout--;
	// Adding a 1 ms delay so as not to have a tight loop; this would let other threads process the event queue, if needed
	LOS_Sleep(1);
    } while ((event.type == ITE_EVT_NONE));

   // if here event.type == ITE_EVT_NONE, that's means TIMEOUT ERROR (no event)
    error = ITE_ManagEvent(event);

    return error;
}


//Fucntion to be called after a ITE_WaitEvent() call to analyse which ispctl event has been received
 //EVENT_GET_EVENTCOUNT used to find no of events received. comment at the top of function
void ITE_traceISPCTLevent(union u_ITE_Event event)
{

     switch (event.type)
     {
        case ITE_EVT_ISPCTL_INFO:

            switch(event.ispctlInfo.info_id)
            {
                case ISP_INIT_DONE:
                    LOS_Log("ISP_INIT_DONE event received \n");
                    break;
                case ISP_HOST_COMMS_READY:
                    LOS_Log("ISP_HOST_COMMS_READY event received \n");
                    break;
                case ISP_BOOT_COMPLETE:
                    LOS_Log("ISP_BOOT_COMPLETE event received \n");
                    break;
                case ISP_SLEEPING:
                    LOS_Log("ISP_SLEEPING event received \n");
                    break;
                case ISP_WOKEN_UP:
                    LOS_Log("ISP_WOKEN_UP event received \n");
                    break;
                case ISP_STREAMING:
                     #ifdef TEST_PERFORMANCE
                     RECORD_SYSTEM_TIME("STOP", "Event0_Count_u16_EVENT0_6_ISP_STOP","ISP_START_STREAM",MAX_ISP_START_STREAM);
                     #endif
                    LOS_Log("ISP_STREAMING event received \n");
                    break;
                case ISP_STOP_STREAMING:
                     #ifdef TEST_PERFORMANCE
                     RECORD_SYSTEM_TIME("STOP", "Event0_Count_u16_EVENT0_6_ISP_STOP","ISP_STOP_STREAM",MAX_ISP_STOP_STREAM);
                     #endif
                    LOS_Log("ISP_STOP_STREAMING event received \n");
                    break;
                case ISP_SENSOR_START_STREAMING:
                    #ifdef TEST_PERFORMANCE
                    RECORD_SYSTEM_TIME("STOP", "Event0_Count_u16_EVENT0_8_SENSOR_START","SENSOR_START_STREAM",MAX_SENSOR_START_STREAM);
                    #endif
                    LOS_Log("ISP_SENSOR_START_STREAMING event received \n");
                    break;
                case ISP_SENSOR_STOP_STREAMING:
                    #ifdef TEST_PERFORMANCE
                    RECORD_SYSTEM_TIME("STOP", "Event0_Count_u16_EVENT0_7_SENSOR_STOP","SENSOR_STOP_STREAM",MAX_SENSOR_STOP_STREAM);
                    #endif
                    LOS_Log("ISP_SENSOR_STOP_STREAMING event received \n");
                    break;
                case ISP_LOAD_READY:
                    LOS_Log("ISP_LOAD_READY event received \n");
                    break;
                case ISP_READLIST_DONE:
                    LOS_Log("ISP_READLIST_DONE event received \n");
                    break;
                case ISP_WRITELIST_DONE:
                    LOS_Log("ISP_WRITELIST_DONE event received \n");
                    break;
                case ISP_POLLING_PE_VALUE_DONE:
                    LOS_Log("ISP_POLLING_PE_VALUE_DONE event received \n");
                                break;
                case ISP_INF_NONE:
                    break;
                   case UNKNOWN_INFO:
                    LOS_Log("UNKNOWN_INFO event received \n");
                    break;
                   default :
                    LOS_Log("WARNING - Unknown ITE_EVT_ISPCTL_INFO event received - Not Expected:0x%x\n",event.ispctlInfo.info_id);
                    break;
            }
            break;

        case ITE_EVT_ISPCTL_ERROR:
            switch(event.ispctlError.error_id)
            {
                case ISP_POLLING_TIMEOUT_ERROR:
                    LOS_Log("ISP_POLLING_TIMEOUT_ERROR event received \n");
                    break;
                case ISP_CHECK_VALUE_ERROR:
                    LOS_Log("ISP_CHECK_VALUE_ERROR event received \n");
                    break;
                case ISP_FORBIDDEN_STATE_TRANSITION_ERROR:
                    LOS_Log("ISP_FORBIDDEN_STATE_TRANSITION_ERROR event received \n");
                    break;
                case ISP_READ_ONLY_ERROR:
                    LOS_Log("ISP_READ_ONLY_ERROR event received \n");
                    break;
                case ISP_READ_ONLY_IN_LIST_ERROR:
                    LOS_Log("ISP_READ_ONLY_IN_LIST_ERROR event received \n");
                    break;
                case ISP_WRITELIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR:
                    LOS_Log("ISP_WRITELIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR event received \n");
                    break;
                case ISP_READLIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR:
                    LOS_Log("ISP_READLIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR event received \n");
                    break;
                case ISP_STREAMING_ERROR:
                    LOS_Log("ISP_STREAMING_ERROR event received \n");
                    break;
                case ISP_DOES_NOT_RESPOND_ERROR:
                    LOS_Log("ISP_DOES_NOT_RESPOND_ERROR event received \n");
                    break;
                case ISP_MASTER_I2C_ACCESS_FAILURE:
                    LOS_Log("ISP_MASTER_I2C_ACCESS_FAILURE event received \n");
                    break;
                default:
                    LOS_Log("UNKNOWN ITE_EVT_ISPCTL_ERROR event received \n");
                    break;
            }
            break;

       case ISP_HOST_TO_SENSOR_ACCESS_COMPLETE_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_HOST_TO_SENSOR_ACCESS_COMPLETE:
                    LOS_Log("ISP_HOST_TO_SENSOR_ACCESS_COMPLETE event received \n");
                    break;
                default :
                    LOS_Log("UNKNOWN ISP_HOST_TO_SENSOR_ACCESS_COMPLETE_INFO event received\n");
                    break;
                }
            break;

        case ITE_EVT_ISPCTL_ZOOM_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_ZOOM_CONFIG_REQUEST_DENIED:
                    LOS_Log("ISP_ZOOM_CONFIG_REQUEST_DENIED event received \n");
                    break;
                case ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED:
                    LOS_Log("ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED event received \n");
                    break;
                case ISP_ZOOM_STEP_COMPLETE:
                    LOS_Log("ISP_ZOOM_STEP_COMPLETE event received \n");
                    break;
                case ISP_ZOOM_SET_OUT_OF_RANGE:
                    LOS_Log("ISP_ZOOM_SET_OUT_OF_RANGE event received \n");
                    break;
                    default :
                    LOS_Log("UNKNOWN ITE_EVT_ISPCTL_ZOOM_INFO event received\n");
                    break;
                }
            break;

        case ITE_EVT_ISPCTL_FOCUS_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_FLADRIVER_LENS_STOP:
                    LOS_Log("ISP_FLADRIVER_LENS_STOP event received \n");
                    break;
                case ISP_AUTOFOCUS_STATS_READY:
                    LOS_Log("ISP_AUTOFOCUS_STATS_READY event received \n");
                    break;
                    default :
                    LOS_Log("UNKNOWN ITE_EVT_ISPCTL_FOCUS_INFO event received\n");
                    break;
                }
            break;

        case ITE_EVT_ISPCTL_STATS_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_GLACE_STATS_READY:
                    LOS_Log("ISP_GLACE_STATS_READY event received \n");
                    break;
                case ISP_HISTOGRAM_STATS_READY:
                    LOS_Log("ISP_HISTOGRAM_STATS_READY event received \n");
                    break;
                case ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED:
                    LOS_Log("ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED event received \n");
                    break;
                case ISP_VALID_BMS_FRAME_NOTIFICATION:
                    LOS_Log("ISP_VALID_BMS_FRAME_NOTIFICATION event received \n");
                    break;
                default :
                    LOS_Log("UNKNOWN ITE_EVT_ISPCTL_STATS_EVENTS event received\n");
                    break;
                }
            break;

          case ISP_NVM_EXPORT_DONE_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_NVM_EXPORT_DONE:
                    LOS_Log("ISP_NVM_EXPORT_DONE event received \n");
                    break;
                default :
                    LOS_Log("UNKNOWN ISP_NVM_EXPORT_DONE_INFO event received\n");
                    break;
                }
            break;

          case ISP_CDCC_AVAILABLE_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_CDCC_AVAILABLE:
                    LOS_Log("ISP_CDCC_AVAILABLE event received \n");
                    break;
                default :
                    LOS_Log("UNKNOWN ISP_CDCC_AVAILABLE_INFO event received\n");
                    break;
                }
            break;

          case ISP_POWER_NOTIFICATION_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_POWER_NOTIFICATION:
                    LOS_Log("ISP_POWER_NOTIFICATION event received \n");
                    break;
                default :
                    LOS_Log("UNKNOWN ISP_POWER_NOTIFICATION_INFO event received\n");
                    break;
                }
            break;

          case ISP_SMS_NOTIFICATION_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_SMS_NOTIFICATION:
                    LOS_Log("ISP_SMS_NOTIFICATION event received \n");
                    break;
                default :
                    LOS_Log("UNKNOWN ISP_SMS_NOTIFICATION_INFO event received\n");
                    break;
                }
            break;

          case ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION:
                    LOS_Log("ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION event received \n");
                    break;
                    default :
                    LOS_Log("UNKNOWN ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION_INFO event received\n");
                    break;
                }
            break;

          case ISP_SENSOR_COMMIT_NOTIFICATION_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_SENSOR_COMMIT_NOTIFICATION:
                    LOS_Log("ISP_SENSOR_COMMIT_NOTIFICATION event received \n");
                default :
                    LOS_Log("UNKNOWN ISP_SENSOR_COMMIT_NOTIFICATION_INFO event received\n");
                    break;
                }
            break;

          case ISP_ISP_COMMIT_NOTIFICATION_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                case ISP_READ_DONE:
                    LOS_Log("ISP_ISP_COMMIT_NOTIFICATION event received \n");
                    break;
               default:
                    LOS_Log("UNKNOWN ISP_ISP_COMMIT_NOTIFICATION_INFO event received\n");
                    break;
                }
            break;



          case ITE_EVT_ISPCTL_GAMMA_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                 case ISP_LR_GAMMA_UPDATE_COMPLETE:
                    LOS_Log("ISP_LR_GAMMA_UPDATE_COMPLETE event received \n");
                    break;
                case ISP_HR_GAMMA_UPDATE_COMPLETE:
                    LOS_Log("ISP_HR_GAMMA_UPDATE_COMPLETE event received \n");
                    break;
                default:
                    LOS_Log("UNKNOWN ITE_EVT_ISPCTL_GAMMA_EVENTS event received\n");
                    break;
                }
            break;


          case ITE_EVT_ISPCTL_RW_EVENTS:
            switch(event.ispctlInfo.info_id)
                {
                 case ISP_READ_DONE:
                    LOS_Log("ISP_READ_DONE event received \n");
                    break;
                case ISP_WRITE_DONE:
                    LOS_Log("ISP_WRITE_DONE event received \n");
                    break;
        default:
                    LOS_Log("UNKNOWN ITE_EVT_ISPCTL_RW_INFO event received\n");
                    break;
                }
            break;

           case ITE_EVT_NONE:
                 //do nothing
                break;
        default:
            LOS_Log("UNKNOWN UNKNOWN ISPCTL Event received. Not an ispctl event:0x%x\n",event.ispctlInfo.info_id);

    }
}

void ITE_RefreshEventCount(t_uint16 addr)
{
volatile t_uint16 count;

    if ((addr > (EVENT_COUNT_3_BASE + EVENT_PAGE_SIZE)) || ( addr < (EVENT_COUNT_0_BASE)))
    {
      LOS_Log("EVENT IS OUT OF BOUNDS");

    }

    else if ( addr >= EVENT_COUNT_3_BASE)
    {
      //check for the page element in  EVENT_COUNT_3
      if(addr == EVENT_COUNT_3_BASE )
        {
             count = 0;
        }

      else
       {
         count = (addr - EVENT_COUNT_3_BASE)/(2);
       }

       g_Event3_Count[count]= ITE_readPE(addr);


    }

    else if ( addr >= EVENT_COUNT_2_BASE)

    {
       //check for the page element in  EVENT_COUNT_2
      if(addr == EVENT_COUNT_2_BASE )
        {
             count = 0;
        }

      else
       {
         count = (addr - EVENT_COUNT_2_BASE)/(2);
       }

       g_Event2_Count[count]= ITE_readPE(addr);


    }


    else if ( addr >= EVENT_COUNT_1_BASE)

    {

       //check for the page element in  EVENT_COUNT_1
      if(addr == EVENT_COUNT_1_BASE )
       {
            count = 0;
       }

      else
       {
         count = (addr - EVENT_COUNT_1_BASE)/(2);
       }

      g_Event1_Count[count]= ITE_readPE(addr);



    }

    else if ( addr >= EVENT_COUNT_0_BASE)

    {

        //check for the page element in  EVENT_COUNT_0
      if(addr == EVENT_COUNT_0_BASE )
       {
            count = 0;
       }

      else
       {
         count = (addr - EVENT_COUNT_0_BASE)/(2);
       }

        g_Event0_Count[count]= ITE_readPE(addr);



    }

}



int ITE_GetEventCount(t_uint16 addr)
{
  volatile t_uint16 count,old_value,no_of_events_recv = 0;
  volatile t_sint16 err;



    if ((addr > (EVENT_COUNT_3_BASE + EVENT_PAGE_SIZE)) || ( addr < (EVENT_COUNT_0_BASE)))
    {
      LOS_Log("EVENT IS OUT OF BOUNDS");

    }

    else if ( addr >= EVENT_COUNT_3_BASE)
    {

        //check for the page element in  EVENT_COUNT_3
       if(addr == EVENT_COUNT_3_BASE )
        {
             count = 0;
        }

       else
        {
          count = (addr - EVENT_COUNT_3_BASE)/(2);
        }

        //lies in EVENT_COUNT_3
        //Reads old value from the global variable
       old_value = g_Event3_Count[count];





        //wait till atleast 1 event arrives.
       do {
             g_Event3_Count[count]= ITE_readPE(addr);
            //stores the number of events occured after previous read
           no_of_events_recv = (g_Event3_Count[count]-old_value);//boundary condition check to be added
          } while (no_of_events_recv == 0);


    }

    else if ( addr >= EVENT_COUNT_2_BASE)

    {

       //check for the page element in  EVENT_COUNT_2
       if(addr == EVENT_COUNT_2_BASE )
        {
             count = 0;
        }

       else
        {
           count = (addr - EVENT_COUNT_2_BASE)/(2);
        }

        //lies in EVENT_COUNT_2
        //Reads old value from the global variable
       old_value = g_Event2_Count[count];


       //wait till atleast 1 event arrives.
      do {

            g_Event2_Count[count]= ITE_readPE(addr);
            //stores the number of events occured after previous read
           no_of_events_recv = (g_Event2_Count[count]-old_value);//boundary condition check to be added
          } while (no_of_events_recv == 0);
       }


    else if ( addr >= EVENT_COUNT_1_BASE)

    {
       //check for the page element in  EVENT_COUNT_1
       if(addr == EVENT_COUNT_1_BASE )
        {
             count = 0;
        }

       else
        {
            count = (addr - EVENT_COUNT_1_BASE)/(2);
        }
        //lies in EVENT_COUNT_1
        //Reads old value from the global variable
       old_value = g_Event1_Count[count];





        //wait till atleast 1 event arrives.
      do {
              g_Event1_Count[count]= ITE_readPE(addr);
            //stores the number of events occured after previous read
           no_of_events_recv = (g_Event1_Count[count]-old_value);//boundary condition check to be added
          } while (no_of_events_recv == 0);

     }

else if ( addr >= EVENT_COUNT_0_BASE)

     {
          //check for the page element in  EVENT_COUNT_0
          if(addr == EVENT_COUNT_0_BASE )
           {
                count = 0;
           }

          else
           {
              count = (addr - EVENT_COUNT_0_BASE)/(2);
           }

           //lies in EVENT_COUNT_0
           //Reads old value from the global variable

          old_value = g_Event0_Count[count];





         //wait till atleast 1 event arrives.
      do {
            g_Event0_Count[count]= ITE_readPE(addr);
            //stores the number of events occured after previous read
           no_of_events_recv = (g_Event0_Count[count]-old_value);//boundary condition check to be added
          } while (no_of_events_recv == 0);

     }

     err = (no_of_events_recv == 1)? (0):(-1);
     return err;

}

int ITE_Check_Event_Count(t_uint16 no_of_event_recv)

{


if(1 == no_of_event_recv)

  {

     LOS_Log("SUCCESS!!! 1 EVENT RECEIVED\n\n");
     return 0;

  }

else
  {
    LOS_Log("Event Count Received Is Not 1 TEST FAILED\n\n");
    return (-1);
  }
}



/************************************************/
/* void ITE_initEvent(void)                                             */
/*                                                                                */
/* initialize Event Array's to zero                                     */
/************************************************/
void ITE_initEvent(void)
{
  t_uint16 i;

  for (i=0;i<32;i++)
  {
      g_Event0_Count[i]=0;
      g_Event1_Count[i]=0;
      g_Event2_Count[i]=0;
      g_Event3_Count[i]=0;
  }

  // Create the semaphore to protect the event queue
  ITE_EventQ.ite_event_semid  = LOS_SemaphoreCreate(1);
  return;
}


void ITE_ExitEvent(void)
{
  // Delete the semaphore that is used to protect the event queue
  LOS_SemaphoreDestroy(ITE_EventQ.ite_event_semid);

  return;
}

