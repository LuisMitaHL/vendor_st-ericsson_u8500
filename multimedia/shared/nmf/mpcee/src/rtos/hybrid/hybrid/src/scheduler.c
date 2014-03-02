/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/hybrid/hybrid/src/scheduler.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
/*
 * Event scheduling 
 */
#include "rtos/hybrid/hybrid.nmf"
#include <rtos/common/inc/lowscheduler.h>
#include <rtos/perfmeter/inc/mpcload.h>
#include <rtos/common/inc/misc.h>
#include <inc/archi-wrapper.h>
#include <share/inc/nmf.h>

/*
 * In order to use EE outside board with simulator, define topOfStack
 * as end of C_stack_end.
 */
const int ATTR(topOfStack) = (int)&C_stack_end;
static unsigned int saveCurrentStackSize = 3 * MIN_STACK_SIZE;

void clearClampRegister(void );

void sched_updateStack(unsigned int currentStackSize)
{
    saveCurrentStackSize = currentStackSize;
    init_stackcheck_U(currentStackSize,(int *)(ATTR(topOfStack) - currentStackSize));
}

/*
 *  Priority management
 */
static volatile int activePriority = -1;
static int switchOnGoing = 0;


/*
 * Raw event scheduler 
 * Note: must be called with interrupt disable
 */
static void RawSchedule() {
    struct TEvent* event;

    /*
     * Schedule each event at this priority
     */
    while((event = FirstEvent[activePriority]) != 0)
    {
        FirstEvent[activePriority] = event->next;
        ScheduleEvent(event);
    } 
}

/*
 * Event scheduler
 * Note: must be called with interrupt disable and must return with interrupt disable
 */
void Schedule(void) {
    int preemptedPriority = activePriority;

    switchOnGoing = 0;
    mpcload_SetLoadState();

    /*
     * Travel whole priority up to current one in order to be compliant with potential
     * double IT2 (one for priority=1, thus another for priority=2). Of course, this could
     * drive to spurious IT2.
     */
    for(activePriority = 2; activePriority > preemptedPriority; activePriority--)
    {
        RawSchedule();
    }
}

/*
 * Sched_Event method implementations
 * Note: must be called with interrupt disable
 */
#pragma force_dcumode
void Sched_Event(struct TEvent *event)
{
    int priority = event->priority;

    LowSchedEvent(event, priority);

    if(activePriority < priority)
    {
        // Switch to higher priority, if necessary Schedule can switch to high higher priority
         if(! switchOnGoing)
         {
             /*
              * Trigger method to go in high level scheduler
              * IT2 is the less priority then will be call only when leave other IT
              * and when returning to user code.
              * Note: must be called with interrupt disable
              */
             switchOnGoing = 1;
             HOST[0x16] = 1;
         }

    }
}

void METH(enter)()
{
    int saveActivePriority;

    MASK_ALL_ITS();
    saveActivePriority = activePriority;
    activePriority = -1;

    while(1) {
        /*
         * Mask IT, require in loop since SWITCH_IDLE_MODE unmask IT.
         */
        MASK_ALL_ITS();
        mpcload_SetLoadState();

        /* Jump into the scheduler
         * Note: here we can come back from the Scheduler
         */
        Schedule();

        /*
         * Go to sleep, which implicitly unmask all IT through EMU_unit_maskit
         * Before and after, change perfmeter state in order to trp idle mips.
         */
        mpcload_SetIdleState();
        ulp.enter();
        SWITCH_IDLE_MODE();
    }
    MASK_ALL_ITS();
    activePriority = saveActivePriority;
    UNMASK_ALL_ITS();
}

void METH(enterAfterSleep)()
{
    ITREMAP_interf_reg2 = 14;   // IT14 (HOST IT) remappe en IT2
    ITREMAP_interf_reg14 = 2;   // IT2 (Fast IT2) remappe en IT14
    ITREMAP_interf_itmskl &= ~(1U << 2); // UnMask IT2
    STACK_CHECK_CMD &= 0xE;//disactivate the stack checker
    _init_sp0((int *) ATTR(topOfStack));
    sched_updateStack(saveCurrentStackSize);
    
    // Unmask interruption in order to allow IT to be taken into account before going in scheduler
    // elsewhere, we can go again in sleep without take into account interruption and thus schedule incoming message
    UNMASK_ALL_ITS();

    METH(enter)();
    
    MASK_ALL_ITS();
    while(1);
}

t_sint32 getRunningComponentPriority()
{
    return (t_sint32) activePriority;
}

/*
 * Program stack checker
 */
void main() {   
    ITREMAP_interf_reg2 = 14;	// IT14 (HOST IT) remappe en IT2
    ITREMAP_interf_reg14 = 2;	// IT2 (Fast IT2) remappe en IT14
	 
    ITREMAP_interf_itmskl	= 0xFFFFU & ~(1U << 2); // Mask all IT except IT2
	ITREMAP_interf_itmskh	= 0xFFFFU;
    
    /* register interupt when they are masked */
    ITREMAP_interf_itmemoh = 0xFFFFU;
    ITREMAP_interf_itmemol = 0xFFFFU;
    
#if defined(EXECUTIVE_ENGINE)
    /* 
     * Init stack pointer
     */
	STACK_CHECK_CMD &= 0xE;//deactivate the stack checker
	
	_init_sp0((int *) ATTR(topOfStack));

    sched_updateStack(3 * MIN_STACK_SIZE);
#endif

    // The first action of the scheduler was to call
    // component initialization method
    construct();
    start();

    clearClampRegister();
    METH(enter)();

    MASK_ALL_ITS();
    while(1);
}

