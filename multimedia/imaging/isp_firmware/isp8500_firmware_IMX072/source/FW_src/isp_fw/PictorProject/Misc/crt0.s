//******************************************************************************
//*                                                                            *
//*        Copyright 2004, STMicroelectronics, Incorporated.                   *
//*        All rights reserved                                                 *
//*                                                                            *
//*          STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION          *
//* This software is supplied under the terms of a license agreement or        *
//* nondisclosure agreement with STMicroelectronics and may not be copied or   *
//* disclosed except in accordance with the terms of that agreement.           *
//*                                                                            *
//******************************************************************************
	
// ****************************************************************************
// Macro to define a vector (jump to the label)
// ****************************************************************************
.macro VECTOR label
    jr %rel2to23(label)
.endm	

// ****************************************************************************
//  Interrupt vector table definition
// ****************************************************************************
    .file "boot.s"
    .section ".startup", .text
    .globl _start

_start:
    VECTOR _reset_handler           // Reset jumps to _reset_handler
    VECTOR _nmie_handler            // NMI to _nmie_handler
    VECTOR _core_hwtrap_handler     // Core trap to _core_hwtrap_handler
    VECTOR _system_hwtrap_handler   // System trap to _system_hwtrap_handler
    VECTOR _swtrap0_handler         // Software trap 0 to _swtrap0_handler
    VECTOR _swtrap1_handler         // Software trap 1 to _swtrap1_handler
    VECTOR _swtrap2_handler         // Software trap 2 to _swtrap2_handler
    VECTOR _swtrap3_handler         // Software trap 3 to _swtrap3_handler

    .type _start,@function
    .size _start,.-_start

// ***************************************
// * Reset Handler (boot code)           *
// ***************************************
    .section ".thandlers", .text
    .entry
    .globl   _reset_handler

_reset_handler:
    // *******************
    // Start cycle counter
    // *******************
    clrcc

    // ****************************
    // Stack pointer initialization
    // ****************************
        
    // __syscallbkp1 is handled by a potential debugger if connected.
    // nop is replaced by a bkp. When the debugger reaches bkp associated to
    // __syscallbkp1, it sets r0 to argc value.
    make       r0, 0    // resets potential value of argc
__syscallbkp1:
    nop
    cmpeq      g0, r0, 0
g0? make       sp, %abs16to31(__stm_end_stack1)
g0? more       sp, %abs0to15(__stm_end_stack1)

    // *******************************************************************
    // Data pointer initialization: GP to the beginning of .gpbase section
    // *******************************************************************
    make       gp, %abs16to31(__GP_BASE)
    more       gp, %abs0to15(__GP_BASE)

    // *********************************
    // Set safe values for loop counters
    // *********************************
    setls      l0, ____start0
    setle      l0, ____end0
    setls      l1, ____start1
    setle      l1, ____end1

    // *********************************
    // PCS initialization
    // *********************************
    movesfr2r  r3, PCS
    bset       r3, r3, 0    // Enable traps
//    bset       r3, r3, 1    // Enable arithmetic traps (disable by default)
    mover2sfr  PCS, r3
    barrier

    // *********************************
    // SR initialization
    // *********************************
    movesfr2r  r3, SR
    bset       r3, r3, 6   // Enable HW loop L0
    bset       r3, r3, 7   // Enable HW loop L1
    mover2sfr  SR, r3
    barrier

    // **************************************
    // Function __stm_ind_main is supposed to
    // never return but currently it does...
    // **************************************
    callr	__stm_ind_crt0

    // *****************************************
    // Breakpoint and infinite loop for security
    // *****************************************
    bkp
    jr	0

    .type _reset_handler,@function
    .size _reset_handler,.-_reset_handler
	
____start0:
____end0:
    nop
____start1:
____end1:
    nop
