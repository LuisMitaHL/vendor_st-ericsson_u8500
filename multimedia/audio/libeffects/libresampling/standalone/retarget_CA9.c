/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   retarget_CA9.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
/*
** Copyright (C) ARM Limited, 2005. All rights reserved.
*/

#include <stdio.h>
#include <rt_misc.h>
 
// Heap base from scatter file
extern int Image$$HEAP$$ZI$$Base;
//#pragma import(__use_two_region_memory)

extern void core_init(void);

/*
The functions below are patched onto main.
*/

extern void $Super$$main(int argc, char *argv[]);

void $Sub$$main(int argc, char *argv[])
{
  core_init();                    // does some extra setup work
  
  $Super$$main(argc,argv);                 // calls the original function
}


/*
This function re-implements the C Library semihosted function. The stack pointer
has aready been set and is passed back to the function, The base of the heap is
set from the scatter file
*/
__value_in_regs struct __initial_stackheap __user_initial_stackheap(
        unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
    struct __initial_stackheap config;

    config.heap_base = (unsigned int)&Image$$HEAP$$ZI$$Base; // placed by scatterfile
    config.stack_base = SP;   // inherit SP from the execution environment

    return config;
}
