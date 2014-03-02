/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/hybrid/hybrid/src/trampoline.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#include <inc/archi-wrapper.h>
#include <rtos/common/inc/misc.h>

extern void Schedule(void);

/*
 * IT2 management
 */
void* info1 = 0;
void* info2 = 0;

asm void push_registers()
{
    .extern THIS
    
    push rh2
    push rl2
    push rh3
    push rl3
    push rh4
    push rl4
    push rh5
    push rl5
    
    xsave2 ext23, rl5
    push rl5
    xsave2 ext45, rl5
    push rl5    
    
    push sp1
    
    ldx_f       #_info2, ax1 // Restore and push it ax1
    push ax1 
    push ax2
    push ax3
    
    push Max1
    push Min1
    
    pushlb
    pushls
    pushlelc
    pushlb
    pushls
    pushlelc
    pushlb
    pushls
    pushlelc
    
    push Flag
    mode24
    push Adctl
    
    ldx_f       #THIS, ax1
    push ax1
    xmvi #8192,adctl
}

asm void pop_registers()
{
    .extern THIS
    .extern _EMU_unit_maskit
    
    pop ax1
    stx_f       ax1, #THIS

    pop Adctl
    pop Flag
    
    poplelc
    popls
    poplb
    poplelc
    popls
    poplb
    poplelc
    popls
    poplb

    pop Min1
    pop Max1
 
    pop ax3
    pop ax2
    pop ax1
    
    pop rl5
    xmv rl5, sp1
    
    pop rl5 
    xload2 rl5, ext45
    pop rl5
    xload2 rl5, ext23
 
    pop rl5
    pop rh5    
    pop rl4
    pop rh4
    pop rl3
    pop rh3
    pop rl2
    /* reactivate interruption before popping last elem of context */
    dmvi #0, rh2
    stmmio_f rh2, #_EMU_unit_maskit
    pop rh2
}

#pragma nosaveregs
void trampoline_1() {
    push_registers();
    Schedule();
    pop_registers();
}

asm void change_return()
{
    ldx_f       #_info1, ax1
    stxi_f      ax1, sp0
}

#pragma nosaveregs
void trampoline_0() {
    change_return();
    trampoline_1();
}

void trampoline() {
    trampoline_0();
}

void* trampolineptr = &trampoline;

void deactivateIt()
{
    MASK_ALL_ITS();
}

asm void pushrl2()
{
    push rl2
    push Flag
}

asm void poprl2()
{
    pop Flag
    pop rl2
}

asm void switch_return()
{
    .extern _EMU_unit_maskit
    .extern _deactivateIt
    /*
     * Since the purpose of this method is to switch to system code. Mask ITs
     * before going.
     */
/*    push rl2
myloop:
    dmvi #1, rl2
    stmmio_f rl2, #_EMU_unit_maskit
    nop
    nop
    nop
    ldmmio_f #_EMU_unit_maskit, rl2
    or rl2, rl2, rl2
    beq myloop
    pop rl2*/

    
    stx_f       ax1, #_info2           // Memorise ax1

    /*
     * Change return address in system stack
     */
    ldxi_f      sp0, ax1                // read return address of IT2()
    stx_f       ax1, #_info1            // Memorise it
    ldx_f       #_trampolineptr, ax1    // Load new return address
    stxi_f      ax1, sp0                // Set new return address of IT2()    
}

#pragma nosaveregs
_INTERRUPT  void    IT2(void) {
    pushrl2();
    deactivateIt();
    poprl2();
    switch_return();
}
