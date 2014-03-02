/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/common/src/lowscheduler.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
/*
 * 
 */
#include "rtos/common.nmf"

#include <rtos/common/inc/lowscheduler.h>
#include <inc/archi-wrapper.h>

struct TEvent *FirstEvent[MAX_SCHED_PRIORITY] = {0, 0, 0};
struct TEvent *LastEvent[MAX_SCHED_PRIORITY] = {0, 0, 0};

#pragma noprefix
extern void* THIS;

/*
 * This method must be called under interrupt masked.
 * The reaction will demask interrupt and we remask it before leaving this method.
 */ 
void ScheduleEvent(struct TEvent* event) {
	t_reaction reaction = (t_reaction)event->reaction;
	
 	// Set the new THIS
    THIS = (void*)(event->THIS);
 
    // IT are unmasked in the reaction
 	reaction(event); 

	// Mask it, in order to recoming in the scheduler
 	MASK_ALL_ITS();
}
