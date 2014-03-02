/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <ee/scheduler.nmf>

/* structure that group all variable to handle one level of priority */
static struct t_context {
    /* variable to handle dynamic creation/destruction of threads */
    t_uint32 bindingRefCounter;
    hSem refCounterLock;
    hSem rendezVousSem;
    t_sint32 threadNumber;
    /* executive thread variables */
    hSem semNotifier;
    t_sint32 wakeUpCounter;
    hMutex priorityQueuesMutex;
    t_queue FirstEvent[MAX_SCHEDULER_SUBPRIORITY_NUMBER];
    t_bool isActive;
} context[MAX_SCHEDULER_PRIORITY_NUMBER];

/* typedef for call back */
typedef void (*reacCb)(t_sched_event *);

/* private methods */
static void schedLoop(void * args);
static void wakeUpIfNeeded(t_queue_link *pRes);
static void controlSleep(t_queue_link *pRes, void *args);
static t_uint32 initPriority(struct t_context *pContext);
static void destroyPriority(struct t_context *pContext, t_uint32 priority);
static t_uint32 startPriority(struct t_context *pContext, t_uint32 priority);
static void stopPriority(struct t_context *pContext, t_uint32 priority);
static t_sched_event *getNextEvent(t_uint32 priority);

/* implement ee.api.scheduler.itf */
EXPORT_SHARED void nmfSchedulerSchedEvent(t_sched_event *pEvent)
{
    /* sanity check */
    if(pEvent->priority >= MAX_SCHEDULER_PRIORITY_NUMBER) {
        NMF_PANIC("nmfSchedulerSchedEvent : Illegal priority %d (max = %d)\n", pEvent->priority, MAX_SCHEDULER_PRIORITY_NUMBER);
    }
    if(pEvent->subpriority >= MAX_SCHEDULER_SUBPRIORITY_NUMBER) {
        NMF_PANIC("nmfSchedulerSchedEvent : Illegal subpriority %d (max = %d)\n", pEvent->subpriority, MAX_SCHEDULER_SUBPRIORITY_NUMBER);
    }

    eeMutexLock(context[pEvent->priority].priorityQueuesMutex);
    queueNoLock.pushAndExecute(context[pEvent->priority].FirstEvent[pEvent->subpriority], (t_queue_link *)pEvent, (void *)wakeUpIfNeeded);
    eeMutexUnlock(context[pEvent->priority].priorityQueuesMutex);
}

EXPORT_SHARED void* EEgetDistributionChannel(t_uint32 priority) {
    if (!IS_NULL_INTERFACE(distribution, getDistributionChannel))
        return (void *) distribution.getDistributionChannel(priority);
    else
    {
        NMF_PANIC("EEgetDistributionChannel : Illegal call on a not distributed system\n");
        return (void *) 0;
    }
}

EXPORT_SHARED t_uint32 signalComponentCreation(t_uint32 priority)
{
    struct t_context *pContext = (struct t_context *) &context[priority];
    
    /* sanity check */
    if (priority >= MAX_SCHEDULER_PRIORITY_NUMBER) {
        NMF_PANIC("signalComponentCreation : Illegal priority %d (max = %d)\n", priority, MAX_SCHEDULER_PRIORITY_NUMBER);
    }
    
    /* start priority stuff if first for priority */
    sem.P(pContext->refCounterLock);
    if (++pContext->bindingRefCounter == 1) {
        if (startPriority(pContext, priority))
            goto error;
    }
    sem.V(pContext->refCounterLock);
        
    return 0;

error:
    pContext->bindingRefCounter--;
    sem.V(pContext->refCounterLock);
    
    return 1;
}

EXPORT_SHARED t_uint32 signalComponentDestruction(t_uint32 priority)
{
    struct t_context *pContext = (struct t_context *) &context[priority];
    
    /* sanity check */
    if (priority >= MAX_SCHEDULER_PRIORITY_NUMBER) {
        NMF_PANIC("signalComponentCreation : Illegal priority %d (max = %d)\n", priority, MAX_SCHEDULER_PRIORITY_NUMBER);
    }
    
    /* stop priority stuff if needed */
    sem.P(pContext->refCounterLock);
    if (--pContext->bindingRefCounter == 0)
        stopPriority(pContext, priority);
    sem.V(pContext->refCounterLock);
    
    return 0;
}

/* implement ee.api.threadStartRoutine.itf */
void METH(routine)(void *args)
{
    schedLoop(args);
}

/* implement ee.api.init */
t_sint32 METH(init)()
{
    t_uint32 i;
    
    for(i=0;i<MAX_SCHEDULER_PRIORITY_NUMBER;i++) {
        if (initPriority(&context[i]))
            goto error;
    }

    return 0;
    
error:
    while(i--)
        destroyPriority(&context[i-1], i-1);

    return NMF_NO_MORE_MEMORY;
}

void METH(destroy)()
{
    t_uint32 i;

    for(i=0;i<MAX_SCHEDULER_PRIORITY_NUMBER;i++)
        destroyPriority(&context[i], i);
}

/* privates method */
static t_bool notFoundInRunning(t_queue_link *pRes, void* Args) {
    t_sched_event *pEvent = (t_sched_event*) pRes;
    
    if (eeMutexLockTry((hMutex) pEvent->mutexHandle))
        return 0;//lock try fail
    else
        return 1;//lock try successfull
}

static void schedLoop(void * args)
{
    t_uint32 priority = (t_uint32)args & 0xff;
    struct t_context *pContext = (struct t_context *) &context[priority];
    //t_uint32 core = ((t_uint32)args >> 8) & 0xff;

    while(pContext->isActive)
    {
        t_sched_event *pEvent;

        //TODO ;
        while((pEvent = getNextEvent(priority)) != 0x0) {
            reacCb react = (reacCb)pEvent->pReaction;
            hMutex targetMutex = (hMutex) pEvent->mutexHandle;

            //NMF_LOG("Thread 0x%08x:%d / Pop 0x%08x\n",0, core, pEvent);

            (*react)(pEvent);
            eeMutexLock(pContext->priorityQueuesMutex);
            eeMutexUnlock(targetMutex);
            eeMutexUnlock(pContext->priorityQueuesMutex);
        }

        //NMF_LOG("Thread 0x%08x:%d:%d waiting\n",0,priority, core);
        sem.P(pContext->semNotifier);
        //NMF_LOG("Thread 0x%08x:%d:%d wakeup\n",0,priority, core);
    }
    
    sem.V(pContext->rendezVousSem);
}

static t_sched_event *getNextEvent(t_uint32 priority)
{
	struct t_context *pContext = (struct t_context *) &context[priority];
	t_sint16 subPriority = MAX_SCHEDULER_SUBPRIORITY_NUMBER;
	t_sched_event *pRes = 0;

	eeMutexLock(pContext->priorityQueuesMutex);
	while(--subPriority >= 0) {
	    if (subPriority)
	        pRes = (t_sched_event *) queueNoLock.popMatchingAndExecute(pContext->FirstEvent[subPriority], (void *) notFoundInRunning,
                                                     0x0, 0, 0);
	    else
	        pRes = (t_sched_event *) queueNoLock.popMatchingAndExecute(pContext->FirstEvent[subPriority], (void *) notFoundInRunning,
                                                     0x0, (void *) controlSleep, (void *) priority);
		if (pRes)
			break;
	}
	eeMutexUnlock(pContext->priorityQueuesMutex);

	return pRes;
}

static void wakeUpIfNeeded(t_queue_link *pRes)
{
    t_sched_event *pEvent = (t_sched_event*) pRes;

    /* if some thread are sleeping then wake up one to see if it can handle event */
    if (context[pEvent->priority].wakeUpCounter < 0)
    {
        context[pEvent->priority].wakeUpCounter++;
        sem.V(context[pEvent->priority].semNotifier);
    }
}

static void controlSleep(t_queue_link *pRes, void *args)
{
    t_uint32 priority = (t_uint32)args;

    /* if no event return after queue parsing then thread will go to sleep */
    if (pRes == 0) {context[priority].wakeUpCounter--;}
}

/* privates to handle context */
t_uint32 initPriority(struct t_context *pContext)
{
    pContext->bindingRefCounter = 0;
    pContext->refCounterLock = sem.create(1);
    if (!pContext->refCounterLock)
        return 1;
    
    return 0;
}

void destroyPriority(struct t_context *pContext, t_uint32 priority)
{
    if (pContext->bindingRefCounter) {
        NMF_LOG("destroyPriority : priority %d destruction whereas there are still loaded components\n", priority);
    }
    if (pContext->refCounterLock)
        sem.destroy(pContext->refCounterLock);
    pContext->refCounterLock = 0;
    pContext->bindingRefCounter = 0;
}

t_uint32 startPriority(struct t_context *pContext, t_uint32 priority)
{
    t_sint32 i;

    if (!IS_NULL_INTERFACE(distribution, notifyStart)
	&& osd.isDistributionThread(priority))
        if (!distribution.notifyStart(priority))
            return 1;
    pContext->wakeUpCounter = 0;
    pContext->isActive = TRUE;
    pContext->rendezVousSem = sem.create(0);
    if (!pContext->rendezVousSem)
        goto errorRendezVousSem;
    pContext->semNotifier = sem.create(0);
    if (!pContext->semNotifier)
        goto errorSemNotifier;
    pContext->priorityQueuesMutex = eeMutexCreate();
    if (!pContext->priorityQueuesMutex)
    	goto errorPriorityMutex;
    for(i=0;i<MAX_SCHEDULER_SUBPRIORITY_NUMBER;i++) {
    	pContext->FirstEvent[i] = queueNoLock.create();
    	if (!pContext->FirstEvent[i])
    	        goto errorFirstEvent;
    }
    if (IS_NULL_INTERFACE(distribution, getDistributionChannel))
        pContext->threadNumber = osd.notifyStart(priority, 0);
    else
        pContext->threadNumber = osd.notifyStart(priority, distribution.getDistributionChannel(priority));
    if (pContext->threadNumber <= 0)
        goto errorThreadNumber;

    return 0;

errorThreadNumber:
    {
        pContext->threadNumber = -pContext->threadNumber;
        // Exit from distribution thread if distribution active and
        // thread started
        if (!IS_NULL_INTERFACE(distribution, notifyStop)
	    && osd.isDistributionThread(priority)) {
            if (pContext->threadNumber) {
                distribution.notifyStop(priority);
                pContext->threadNumber--;
            }
        }
        // Exit from worker thread already started
        pContext->isActive = FALSE;
        // Awake worker thread
        for(i=0;i<pContext->threadNumber;i++)
            sem.V(pContext->semNotifier);
        // Wait for thread leaving loop
        for(i=0;i<pContext->threadNumber;i++)
            sem.P(pContext->rendezVousSem);
        pContext->threadNumber = 0;
    }
errorFirstEvent:
	for(i=0;i<MAX_SCHEDULER_SUBPRIORITY_NUMBER;i++) {
		if (pContext->FirstEvent[i]) {
			queueNoLock.destroy(pContext->FirstEvent[i]);
			pContext->FirstEvent[i] = 0;
		}
	}
	eeMutexDestroy(pContext->priorityQueuesMutex);
errorPriorityMutex:
    sem.destroy(pContext->semNotifier);
errorSemNotifier:
    sem.destroy(pContext->rendezVousSem);
errorRendezVousSem:
    pContext->isActive = FALSE;
    
    return 1;
}

void stopPriority(struct t_context *pContext, t_uint32 priority)
{
    t_sint32 i;
    
    // Close distribution thread
    if (!IS_NULL_INTERFACE(distribution, notifyStop)
	&& osd.isDistributionThread(priority))
        distribution.notifyStop(priority);
    // Unblock executive threads
    pContext->isActive = FALSE;
    for(i=0;i<pContext->threadNumber;i++)
        sem.V(pContext->semNotifier);
    // Wait for thread leaving loop
    for(i=0;i<pContext->threadNumber;i++)
        sem.P(pContext->rendezVousSem);
    // We notify osi code that executive threads are leaving
    if (IS_NULL_INTERFACE(distribution, getDistributionChannel))
        osd.notifyStop(priority, 0);
    else
        osd.notifyStop(priority, distribution.getDistributionChannel(priority));
    // Now we can clean priority stuff
    pContext->wakeUpCounter = 0;
    for(i=0;i<MAX_SCHEDULER_SUBPRIORITY_NUMBER;i++) {
		queueNoLock.destroy(pContext->FirstEvent[i]);
		pContext->FirstEvent[i] = 0;
    }
    eeMutexDestroy(pContext->priorityQueuesMutex);
    sem.destroy(pContext->semNotifier);
    sem.destroy(pContext->rendezVousSem);
}

EXPORT_SHARED t_uint32 nmfSchedulerWait()
{
    hSem waitSem;
    t_uint32 result = (t_uint32)-1;

    waitSem = (hSem) sem.create(0);
    if (waitSem == 0x0)
        goto error;

    result = sem.PTimeout(waitSem, 100);

    sem.destroy(waitSem);
error:
    return result;
}

static t_bool matchTargetTHIS(t_queue_link *pRes, void* Args) {
    t_sched_event *pEvent = (t_sched_event*) pRes;

    if (pEvent->THIS == Args)
        return 1;// THIS do match
    else
        return 0;
}

EXPORT_SHARED t_queue_link *nmfSchedulerRemove(void* targetTHIS, t_uint16 priority, t_uint16 subPriority)
{
    struct t_context *pContext = (struct t_context *) &context[priority];
    t_queue_link *pRes = 0;

    eeMutexLock(pContext->priorityQueuesMutex);
    pRes = queueNoLock.popMatchingAndExecute(pContext->FirstEvent[subPriority], (void *) matchTargetTHIS, targetTHIS, 0, 0);
    eeMutexUnlock(pContext->priorityQueuesMutex);

    return pRes;
}
