/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/synchronous/synchronous/src/scheduler.c - EE.
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
#include "rtos/synchronous/synchronous.nmf"
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
static unsigned int saveCurrentStackSize = MIN_STACK_SIZE;
static volatile int activePriority = -1;

void sched_updateStack(unsigned int currentStackSize)
{
    saveCurrentStackSize = currentStackSize;
    init_stackcheck_U(currentStackSize,(int *)(ATTR(topOfStack) - currentStackSize + 1));
}

/*
 * Sched_Event_xxx method implementations
 */
#pragma force_dcumode
void Sched_Event(struct TEvent *event) {
    LowSchedEvent(event, event->priority);
}

/*
 * 
 */
void METH(enter)() {
     //printf("Restart %x %x\n", get_sp0(), get_other_sp0());
    
    wnop();
    while(1) {
        struct TEvent* event;
        int priority;

        MASK_ALL_ITS();

        for(priority = MAX_SCHED_PRIORITY - 1; priority >= 0; ) {
            activePriority = priority;
            if((event = FirstEvent[priority]) != 0) {   
                FirstEvent[priority] = event->next;
                ScheduleEvent(event);
                /* 
                 * Restart scheduler loop at beginning since protentially, reaction can have post event.
                 */
                priority = MAX_SCHED_PRIORITY - 1; 
            } else {
                /*
                 * No more priority, go to less one 
                 */
                priority --;
            }
        }
        activePriority = -1;
        
        /*
         * Go to sleep, which implicitly unmask all IT through EMU_unit_maskit
         * Before and after, change perfmeter state in order to trp idle mips.
         */
        mpcload_SetIdleState();
        ulp.enter();
        SWITCH_IDLE_MODE();
        mpcload_SetLoadState();
    }
}

void METH(enterAfterSleep)()
{
    STACK_CHECK_CMD &= 0xE;//disactivate the stack checker
    _init_sp0((int *) ATTR(topOfStack));
    sched_updateStack(saveCurrentStackSize);
    
    METH(enter)();
}

#if !defined(BLOCKING_PANIC)
/*
 * Abort current component execution and jump to next reaction
 */
void abortCurrentTask() {
	// Reset stack
    _init_sp0((int *) ATTR(topOfStack));
    
    // Restart scheduler
    METH(enter)();
}
#endif

t_sint32 getRunningComponentPriority()
{
    return (t_sint32) activePriority;
}

/*
 * Sched runs infinitly and dispatch events that have been previously posted.
 */
void main() {
	ITREMAP_interf_itmskl	= 0xFFFF;
	ITREMAP_interf_itmskh	= 0xFFFF;
    
    /* register interupt when they are masked */
    ITREMAP_interf_itmemoh = 0xFFFFU;
    ITREMAP_interf_itmemol = 0xFFFFU;
    
#if defined(EXECUTIVE_ENGINE)
    /* 
     * Init stack pointer 
     */
	STACK_CHECK_CMD &= 0xE;//disactivate the stack checker

	_init_sp0((int *) ATTR(topOfStack));

    sched_updateStack(MIN_STACK_SIZE);
#endif
    
    // The first action of the scheduler was to call
    // component initialization method
    construct();
    start();

    METH(enter)();
}
