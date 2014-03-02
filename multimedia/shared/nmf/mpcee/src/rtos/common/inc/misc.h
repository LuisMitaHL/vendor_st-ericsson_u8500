/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/common/inc/misc.h - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
/*
 * Misc internal function 
 */
 
#ifndef RTOS_MISC_H
#define RTOS_MISC_H

/*
 * sp0 stack management
 */
asm void * get_sp0(void) {
    xmv sp0, @{}
}
asm void  _init_sp0(int * val) {
    xmv @{val}, sp0
}
asm void *get_other_sp0() {
    get_other_sp @{}
}
asm void set_other_sp0(void* val) {
    set_other_sp @{val}
}

// Memory range
extern int _max_ram1_;

// Stack range defined by linker
extern int C_stack_, C_stack_size, C_stack_end;
extern int S_stack_, S_stack_end;


// Set stack checker range
extern void init_stackcheck_U(unsigned int stack_size, int * stack_address);
extern void init_stackcheck_S(unsigned int stack_size, int * stack_address);

#endif /* RTOS_MISC_H */
