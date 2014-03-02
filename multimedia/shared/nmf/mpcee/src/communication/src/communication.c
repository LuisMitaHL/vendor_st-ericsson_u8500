/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* communication/src/communication.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 *
 * Written by NMF team.
 */
#include <communication.nmf>

#include <share/communication/inc/communication_fifo.h>
#include <share/inc/macros.h>
#include <communication/fifo/inc/nmf_fifo_dsp.h>
#include <rtos/common/inc/lowscheduler.h>

#include <semaphores/inc/semaphores.h>

#include <inc/mmdsp_mapping.h>
#include <inc/archi-wrapper.h>
#include <communication/inc/services.h>

const t_nmf_fifo ATTR(toNeighborsComsFifoId)[NB_CORE_IDS];
volatile const t_nmf_fifo ATTR(fromNeighborsComsFifoId)[NB_CORE_IDS];

#ifdef __DEBUG
static volatile t_uint24 armdspCounter;
static volatile t_uint24 discardedArmdspCounter;
static volatile t_uint24 armdspIrqCounter;
static volatile t_uint24 armdspSpuriousIrqCounter;
static volatile t_uint24 dsparmCounter;
static volatile t_uint24 dsparmIrqCounter;
static volatile t_uint24 dspPollingCounter;
#endif /* __DEBUG */

/*
   PUBLIC void InitCommunication(void)
   */
t_nmf_error METH(construct)(void)
{
    t_uint16 val;

#ifdef __DEBUG
    armdspCounter = 0;
    discardedArmdspCounter = 0;
    armdspIrqCounter = 0;
    dsparmCounter = 0;
    dsparmIrqCounter = 0;
    dspPollingCounter = 0;
    armdspSpuriousIrqCounter = 0;
#endif /* __DEBUG */

    sem_Init(11); /* 11 => _INTERRUPT void	IT11(void) {...} */

    return NMF_OK;
}

#pragma force_dcumode
PUBLIC t_event_params_handle AllocEvent(t_nmf_fifo fifoId)
{
    t_shared_addr retValue;
    
    retValue = fifo_getAndAckNextElemToWritePointer(fifoId);

    if(retValue == 0)
        Panic(PARAM_FIFO_OVERFLOW, 0);
    
    return (t_event_params_handle)retValue;
}

asm long ext24to32(/*int*/long p) {
    L_msli @{p}, #8, @{p}
    asri @{p}.0, #8, @{}.1
    mv @{p}.1, @{}.0
}

asm /*int*/long ext32to24(long value) {
    asli @{value}.1, #8,  @{}.0
    zero16 @{value}.0, @{}.1
    L_lsri @{}, #8, @{}
/*
    mv @{p}.1, @{}.0
    mv @{p}.0, @{}.1
    asli @{}.0, #8, @{}.0
    L_msri @{}, #8, @{}
    */
}

void PushEventTace(t_nmf_fifo fifoId, t_event_params_handle h, t_uword id, t_uint32 isTrace)
{
    t_nmf_fifo_desc __SHARED16 *pFifoDesc = (t_nmf_fifo_desc __SHARED16 *)fifoId;
    t_uint32 __SHARED16 *pEventElem;
    t_nmf_core_id coreId = sem_GetToCoreIdFromSemId(pFifoDesc->semId);

    do {
        /*
         * Infinite loop until host has pop some element.
         * This potentially will drive MPC to dead, if Host is dead. But we assume, this is not a issue.
         */
        pEventElem = (t_uint32 __SHARED16 *)fifo_getNextElemToWritePointer(ATTR(toNeighborsComsFifoId)[coreId]);

        /*
         * Enable interruption in order to allow flush of incoming FIFO COMS in order to not
         * fall in such error => Of course such infinite loop is not beautiful !
         */
        if(pEventElem == 0)
        {
            UNMASK_ALL_ITS();
            MASK_ALL_ITS();
        }
    }
    while (pEventElem == 0);

    pEventElem[EVENT_ELEM_METHOD_IDX] = ext24to32(id);
    pEventElem[EVENT_ELEM_PARAM_IDX] = ext24to32(h);
    pEventElem[EVENT_ELEM_EXTFIELD_IDX] = pFifoDesc->extendedField;

#ifdef __DEBUG
    dsparmCounter++;
#endif /* __DEBUG */

    if (isTrace)
        nmfTraceCommunication(TRACE_COMMUNICATION_COMMAND_SEND, coreId);
    fifo_coms_acknowledgeWriteAndInterruptGeneration(ATTR(toNeighborsComsFifoId)[coreId]);

    return;
}

#pragma force_dcumode
PUBLIC void PushEvent(t_nmf_fifo fifoId, t_event_params_handle h, t_uword id)
{
    PushEventTace(fifoId, h, id, 1);
}

#pragma force_dcumode
PUBLIC void AcknowledgeEvent(t_nmf_fifo fifoId) { fifo_acknowledgeRead(fifoId); }

#pragma lock
#pragma force_dcumode
_INTERRUPT void	IT11(void)
{
    t_nmf_core_id codeId;

#ifdef __DEBUG
    armdspIrqCounter++;
#endif /* __DEBUG */
    while ((codeId = sem_GetFromCoreIdFromIrqSrc()) <= LAST_CORE_ID)
    {
        do {
            t_uint32 __SHARED16 *pEventElem;
            t_uint24 *pFifoSharedFieldDesc;

            t_remote_event **ppEventFifo;
            t_remote_event *pEvent;

            pEventElem = fifo_getNextElemToReadPointer(ATTR(fromNeighborsComsFifoId)[codeId]);
            if (pEventElem == 0)
            {
                /* we reveice a spurious interrupt */
#ifdef __DEBUG
                armdspSpuriousIrqCounter++;
#endif          
                break;
            }

#ifdef __DEBUG
            armdspCounter++;
#endif /* __DEBUG */

            pFifoSharedFieldDesc = (t_uint24*)ext32to24(pEventElem[EVENT_ELEM_EXTFIELD_IDX]);

            ppEventFifo = (t_remote_event **)pFifoSharedFieldDesc[EXTENDED_FIELD_BCTHIS_OR_TOP];
            pEvent = *ppEventFifo;
            if ((void * )0 != (void *)pEvent)
            {
                /*
                 * Since we can be interrupt by more priority interrupt that can post event,
                 * then we have to block them in order to solve reentrancy issue and lack
                 * of event in scheduler
                 */
                t_uint32 isService = isServiceCall(pEvent);
                int idx = ext32to24(pEventElem[EVENT_ELEM_METHOD_IDX]);

                MASK_ALL_ITS();

                pEvent->data = (t_event_params_handle)ext32to24(pEventElem[EVENT_ELEM_PARAM_IDX]);
                pEvent->event.reaction = (t_reaction)pFifoSharedFieldDesc[EXTENDED_FIELD_BCDESC + idx];
                (*ppEventFifo) = pEvent->event.next;

                if (!isService)
                {
                    nmfTraceCommunication(TRACE_COMMUNICATION_COMMAND_RECEIVE, codeId);
                }

                Sched_Event(&pEvent->event);

                UNMASK_ALL_ITS();
            }
            else
            {
#ifdef __DEBUG
                discardedArmdspCounter++;
#endif
                Panic(INTERNAL_PANIC, 0);
            }
        } while (fifo_acknowledgeRead(ATTR(fromNeighborsComsFifoId)[codeId]) > 0);
    }
}

/* End of communication.c file */
