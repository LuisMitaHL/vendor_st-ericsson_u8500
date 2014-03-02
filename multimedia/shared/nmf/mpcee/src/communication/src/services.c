/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* communication/src/services.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#include <communication.nmf>

#include <share/communication/inc/initializer.h>
#include <rtos/common/inc/lowscheduler.h>
#include <inc/mmdsp_mapping.h>
#include <inc/archi-wrapper.h>
#include <communication/inc/communication.h>

struct TEvent* ATTR(TOP);
const t_nmf_fifo ATTR(FIFOcmd);
const t_nmf_fifo ATTR(FIFOack);
extern t_uint24 forceWakeup; // Not beautiful, but i'm tired

typedef void (*t_init_fct)(void);

#pragma noprefix
extern void* THIS;

static void callMethodAndAck(t_remote_event *event, int index) {
    t_event_params_handle _xyuv_data = event->data;

    /* Free event */
     event->event.next = ATTR(TOP);
     ATTR(TOP) = event;

    /* Call method with interface switching */
    {
        void *oldTHIS = THIS;
        THIS = (void *)(int)ext16to32((int)_xyuv_data[INIT_COMPONENT_CMD_THIS_INDEX+1], (int)_xyuv_data[INIT_COMPONENT_CMD_THIS_INDEX]);

	UNMASK_ALL_ITS();

	nmfTraceActivity(TRACE_ACTIVITY_START, (t_uint24)THIS, index);
        ((t_init_fct)(int)ext16to32((int)_xyuv_data[INIT_COMPONENT_CMD_METHOD_INDEX+1], (int)_xyuv_data[INIT_COMPONENT_CMD_METHOD_INDEX]))();
	nmfTraceActivity(TRACE_ACTIVITY_END, (t_uint24)THIS, index);

	MASK_ALL_ITS();

        THIS = oldTHIS;
    }
    
    /* Acknowledge to Host */
    {
        t_event_params_handle _xyuv_data_ack;
        
        do {
            /*
             * Infinite loop until host has pop some element.
             * This potentially will drive MPC to dead, if Host is dead. But we assume, this is not a issue.
             */
            _xyuv_data_ack = AllocEvent(ATTR(FIFOack));
        } while(_xyuv_data_ack == 0);
        
        _xyuv_data_ack[INIT_COMPONENT_ACK_HANDLE_INDEX] = _xyuv_data[INIT_COMPONENT_CMD_HANDLE_INDEX];
        _xyuv_data_ack[INIT_COMPONENT_ACK_HANDLE_INDEX+1] = _xyuv_data[INIT_COMPONENT_CMD_HANDLE_INDEX+1];

        AcknowledgeEvent(ATTR(FIFOcmd));
        PushEventTace(ATTR(FIFOack), _xyuv_data_ack, index, 0);
    }
}

static void Contructer_Run_Common(t_remote_event *event, int index)
{
#if 0
    int i;

    MMDSP_FLUSH_DATABUFFER();
    for (i = 0; i < pParam->componentPgmFlushNumber; i++)
    {
        MMDSP_FLUSH_ICACHE_BY_SERVICE((t_uint24)pParam->componentFlushRequest[i].begin, (t_uint24)pParam->componentFlushRequest[i].end);
    }

    for (i = 0; i < pParam->componentDataFlushNumber; i++)
    {
        MMDSP_FLUSH_DCACHE_BY_SERVICE(
                (t_uint24)pParam->componentFlushRequest[pParam->componentPgmFlushNumber + i].begin,
                (t_uint24)pParam->componentFlushRequest[pParam->componentPgmFlushNumber + i].end
                );
    }
#else
    MMDSP_FLUSH_ICACHE_UNLOCK_ONLY();
    MMDSP_FLUSH_DCACHE();
#endif

    callMethodAndAck(event, index);
}

t_uint32 isServiceCall(t_remote_event *pEvent)
{
    return (pEvent == ATTR(TOP))?1:0;
}

void Constructer_Run(t_remote_event *event)
{
    Contructer_Run_Common(event, NMF_CONSTRUCT_INDEX);
}

void Constructer_Run_Sync(t_remote_event *event)
{
    Contructer_Run_Common(event, NMF_CONSTRUCT_SYNC_INDEX);
}

void Starter_Run(t_remote_event *event)
{
    callMethodAndAck(event, NMF_START_INDEX);
}

void Starter_Run_Sync(t_remote_event *event)
{
    callMethodAndAck(event, NMF_START_SYNC_INDEX);
}

void Stopper_Run(t_remote_event *event)
{
    callMethodAndAck(event, NMF_STOP_INDEX);
}

void Stopper_Run_Sync(t_remote_event *event)
{
    callMethodAndAck(event, NMF_STOP_SYNC_INDEX);
}

void Destroyer_Run(t_remote_event *event)
{
    callMethodAndAck(event, NMF_DESTROY_INDEX);
}

void Update_Stack(t_remote_event *event)
{
    t_event_params_handle _xyuv_data = event->data;
    unsigned int currentStackSize;
    
    /* Free event */
    event->event.next = ATTR(TOP);
    ATTR(TOP) = event;
    
    /* update stack */
    currentStackSize = (int)ext16to32((int)_xyuv_data[1], (int)_xyuv_data[0]);
    sched_updateStack(currentStackSize);
    
    /* acknowledge fifo */
    AcknowledgeEvent(ATTR(FIFOcmd));

    /* Acknowledge to Host */
    {
        t_event_params_handle _xyuv_data_ack;

        _xyuv_data_ack = AllocEvent(ATTR(FIFOack));

        PushEventTace(ATTR(FIFOack), _xyuv_data_ack, NMF_UPDATE_STACK, 0);
    }
}

void Lock_Cache(t_remote_event *event)
{
    t_event_params_handle _xyuv_data = event->data;
    t_uint24 addr;
    t_uint24 way;

    /* Free event */
    event->event.next = ATTR(TOP);
    ATTR(TOP) = event;

    /* update parameters */
    addr = (int)ext16to32((int)_xyuv_data[1], (int)_xyuv_data[0]);
    way = (int)ext16to32((int)_xyuv_data[3], (int)_xyuv_data[2]);

    /* call api */
    lock.lock(addr, way);

    /* acknowledge fifo */
    AcknowledgeEvent(ATTR(FIFOcmd));

    /* Acknowledge to Host */
    {
        t_event_params_handle _xyuv_data_ack;

        _xyuv_data_ack = AllocEvent(ATTR(FIFOack));

        PushEventTace(ATTR(FIFOack), _xyuv_data_ack, NMF_LOCK_CACHE, 0);
    }
}

void Unlock_Cache(t_remote_event *event)
{
    t_event_params_handle _xyuv_data = event->data;
    t_uint24 way;

    /* Free event */
    event->event.next = ATTR(TOP);
    ATTR(TOP) = event;

    /* update parameters */
    way = (int)ext16to32((int)_xyuv_data[1], (int)_xyuv_data[0]);

    /* call api */
    lock.unlock(way);

    /* acknowledge fifo */
    AcknowledgeEvent(ATTR(FIFOcmd));

    /* Acknowledge to Host */
    {
        t_event_params_handle _xyuv_data_ack;

        _xyuv_data_ack = AllocEvent(ATTR(FIFOack));

        PushEventTace(ATTR(FIFOack), _xyuv_data_ack, NMF_UNLOCK_CACHE, 0);
    }
}

void NmfPrint0(int level, char* str);

void ULP_ForceWakeUp(t_remote_event *event)
{
    /* Free event */
    event->event.next = ATTR(TOP);
    ATTR(TOP) = event;

    /* update parameters */
    forceWakeup = 1;

    // NmfPrint0(0, "MMDSP: ForceWakeUp\n");

    /* acknowledge fifo */
    AcknowledgeEvent(ATTR(FIFOcmd));

    /* Acknowledge to Host */
    {
        t_event_params_handle _xyuv_data_ack;

        _xyuv_data_ack = AllocEvent(ATTR(FIFOack));

        PushEventTace(ATTR(FIFOack), _xyuv_data_ack, NMF_ULP_FORCEWAKEUP, 0);
    }
}

void ULP_AllowSleep(t_remote_event *event)
{
    /* Free event */
    event->event.next = ATTR(TOP);
    ATTR(TOP) = event;

    /* update parameters */
    forceWakeup = 0;

    // NmfPrint0(0, "MMDSP: AllowSleep\n");

    /* acknowledge fifo */
    AcknowledgeEvent(ATTR(FIFOcmd));

    /* Acknowledge to Host */
    {
        t_event_params_handle _xyuv_data_ack;

        _xyuv_data_ack = AllocEvent(ATTR(FIFOack));

        PushEventTace(ATTR(FIFOack), _xyuv_data_ack, NMF_ULP_ALLOWSLEEP, 0);
    }
}
