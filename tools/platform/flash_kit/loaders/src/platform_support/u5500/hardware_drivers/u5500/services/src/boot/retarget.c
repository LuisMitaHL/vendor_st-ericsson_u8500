/* Copyright ARM Ltd 2001. All rights reserved.*/


#include <stdio.h>
#include <rt_misc.h>
/*
Extern declarations 
*/

extern unsigned int bottom_of_heap;
extern unsigned int top_of_stacks;

unsigned int dummyTmp;

/* 
Function declarations
*/
void Reset_Handler(void);
void Vector_Init(void);

__value_in_regs struct __initial_stackheap __user_initial_stackheap(
        unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
    struct __initial_stackheap config;
    
    config.heap_base = (unsigned int)&bottom_of_heap; /*  defined in heap.s */
                                                      /* placed by scatterfile    */
    config.stack_base = SP;   /* inherit SP from the execution environment */

	config.heap_limit = (unsigned int)(&bottom_of_heap + ((1024*1024*16)/sizeof(unsigned int))); /* Heap limit */

	config.stack_limit = config.heap_limit + 8; /* Stack limit */

    return config;
}

void dummy_Init(void)
{
Vector_Init();  /* vector_table */
Reset_Handler();
dummyTmp = top_of_stacks;
}

