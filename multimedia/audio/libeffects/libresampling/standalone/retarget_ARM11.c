/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   retarget_ARM11.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
/*
** Copyright (C) ARM Limited, 2001. All rights reserved.
*/


#include <stdio.h>
#include <rt_misc.h>

extern unsigned int bottom_of_heap;
extern unsigned int top_of_stacks;

unsigned int dummyTmp;
void Reset_Handler(void);
void Vector_Init(void);

__value_in_regs struct __initial_stackheap __user_initial_stackheap(
        unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
    struct __initial_stackheap config;
    
    config.heap_base = (unsigned int)&bottom_of_heap; // defined in heap.s
                                                      // placed by scatterfile   
    config.stack_base = SP;   // inherit SP from the execution environment

    return config;
}

void dummy_Init(void)
{
Vector_Init();
Reset_Handler();
dummyTmp = top_of_stacks;
}
