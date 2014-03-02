/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* inc/archi-wrapper.h - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#ifndef MMDSP_ARCHI_WRAPPER_H_
#define MMDSP_ARCHI_WRAPPER_H_

#ifdef __flexcc2__
#include <inc/type.h>
#include <archi.h>

/*
 * Switch to idle mode and implictly unmask IT 
 * (See Page 167 of Hamac_8815_cut20.pdf)
 */
#define SWITCH_IDLE_MODE()                          \
{                                                   \
    wnop();wnop();                                  \
    EMU_unit_stop = 1;                              \
    wnop();wnop(); wnop(); wnop();                          \
}

/*
 * Mask & unmask Irq
 */
#define MASK_ALL_ITS()                              \
{                                                   \
    do {                                            \
        EMU_unit_maskit = 1;                        \
        wnop(); wnop(); wnop();                     \
    } while(EMU_unit_maskit == 0);                  \
}

#define UNMASK_ALL_ITS()                            \
{                                                   \
    EMU_unit_maskit = 0;                            \
}

/*
 * IrqSafe with state restoring
 */
#define ENTER_CRITICAL_SECTION                      \
{                                                   \
    t_uint16 sav_nk_maskit = EMU_unit_maskit;       \
    do {                                            \
        EMU_unit_maskit = 1;                        \
        wnop(); wnop(); wnop();                     \
    } while(EMU_unit_maskit == 0);

#define EXIT_CRITICAL_SECTION                       \
    EMU_unit_maskit = sav_nk_maskit;                \
}

/*
 * Data cache flush
 */

typedef volatile __MMIO struct {
    t_uword mode;
    t_uword control;
    t_uword way;
    t_uword line;
    t_uword command;
    t_uword status;
    t_uword cptr1l;
    t_uword cptr1h;
    t_uword cptr2l;
    t_uword cptr2h;
    t_uword cptr3l;
    t_uword cptr3h;
    t_uword cptrsel;
} t_dcache_regs;

#define MMDSP_FLUSH_DATABUFFER() {*(volatile __MMIO t_uword *)0xF804 = 1;}

#define CACHE_FLUSH_WAIT_CYCLE 140

#define MMDSP_FLUSH_DCACHE()                           \
{                                                      \
    int i;                                             \
    t_dcache_regs * pDcache = (t_dcache_regs *)0xEC05; \
    ENTER_CRITICAL_SECTION;                            \
    pDcache->command = (t_uint16)0x7U;                 \
    for(i = 0; i < CACHE_FLUSH_WAIT_CYCLE; i++) {      \
        wnop();                                        \
    }                                                  \
    MMDSP_FLUSH_DATABUFFER();                          \
    EXIT_CRITICAL_SECTION;                             \
}

#ifdef __mode16__

asm long ext16to32(int msp, int lsp) {
   mv @{msp}, @{}.1
   mv @{lsp}, @{}.0
}

#else /* __mode16__ -> __mode24__ */

asm long ext16to32(int msp, int lsp) {
   mv @{msp}, @{}.1
   mv @{lsp}, @{}.0
   asli @{}.0, #8, @{}.0
   L_msri @{}, #8, @{}
} 

#endif /* mode24 */

#else /* MMDSP */

#define SWITCH_IDLE_MODE()
#define MASK_ALL_ITS()
#define UNMASK_ALL_ITS()

#define ENTER_CRITICAL_SECTION 
#define EXIT_CRITICAL_SECTION

// Of course that don't run but that compile ;-)
static unsigned long EMU_unit_maskit, ITREMAP_interf_itmskl, ITREMAP_interf_itmskh;
static unsigned short TIMER_mod0lsb, TIMER_mod0msb;
static unsigned short TIMER_mod1lsb, TIMER_mod1msb;
static unsigned long EMU_unit_stop;
	
//static void* THIS;

#define _INTERRUPT

#endif /* MMDSP */

#endif /* MMDSP_ARCHI_WRAPPER_H_ */
