/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* semaphores/system/src/semaphores.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#include <semaphores/system.nmf>

#include <semaphores/inc/semaphores.h>
#include <share/semaphores/inc/hwsem_hwp.h>
#include <inc/mmdsp_mapping.h>
#include <inc/archi-wrapper.h>

const t_uint16 ATTR(myCoreId) = 0;
/* stuff to save before sleeping */
static t_uword saveUsedIntLevel;
static t_sleepNotifyCallback sleepCb;
static t_shared_reg imsc[HSEM_MAX_INTR];

#define EXTSEMAPHORE_BASE  ((0x40000>>1) + MMIO_DSP_BASE_ADDR) // see mmdsp_mapping.h: to know the base of EXTMMIO. EXTMMIO is equivalent to data16 for address generation

static __EXTMMIO t_hw_semaphore_regs *pHwSemRegs = (__EXTMMIO t_hw_semaphore_regs *)EXTSEMAPHORE_BASE;
struct local_reg {
    unsigned int value;
    unsigned int dummy;
};
static volatile struct local_reg __MMIO *it_remap_reg = (volatile struct local_reg __MMIO *)MMDSP_IT_REMAP_BASE;

/* implement ee.api.sleepNotify.itf */
void METH(enter)()
{
    int i;
    
    // save hsem setup since after wake up we will restore hsem setup
    for(i=HSEM_FIRST_INTR; i<HSEM_MAX_INTR;i++) {
        imsc[i] = pHwSemRegs->it[i].imsc;
    }
}

void METH(leave)()
{
    int i;

    // restore hsem save setup
    for(i=HSEM_FIRST_INTR; i<HSEM_MAX_INTR;i++) {
        pHwSemRegs->it[i].imsc = imsc[i];
    }
    // restore irq
    it_remap_reg[saveUsedIntLevel].value = EXT_IT_9_INT_NUMBER;
    ITREMAP_interf_itmskl &= ~(1UL<<saveUsedIntLevel);
}

void sem_Init(t_uword usedIntLevel)
{
    saveUsedIntLevel = usedIntLevel;
    /* TODO : move in starter. register for ulp */
    sleepCb.interfaceCallback = sleepNotify;
    registerForSleep(&sleepCb);
    /* Remap hwsemaphore int (muxed) on given interrupt level */
    /* From STn8815_B0.pdf pp13.11-12: Async_IT[9] */
    it_remap_reg[usedIntLevel].value = EXT_IT_9_INT_NUMBER;
    /* Unmask usedIntLevel interrupt */
    ITREMAP_interf_itmskl &= ~(1UL<<usedIntLevel);
}

t_nmf_core_id sem_GetFromCoreIdFromIrqSrc(void)
{
    t_uword misValue = pHwSemRegs->it[ATTR(myCoreId)+1].mis;
    t_uint32 mask = 1 << FIRST_NEIGHBOR_SEMID(ATTR(myCoreId));
    t_nmf_core_id coreId = ARM_CORE_ID;

    while ((misValue & mask) == 0)
    {
        mask <<= 1;

        coreId++;
        if(coreId == ATTR(myCoreId))
            coreId++;
        if(coreId > LAST_CORE_ID)
            return coreId;
    }

    /* Acknowledge Hsem interrupt */
    pHwSemRegs->it[ATTR(myCoreId)+1].icr = mask;

    return coreId;
}

t_nmf_core_id sem_GetToCoreIdFromSemId(t_semaphore_id semId)
{
    return (t_nmf_core_id)(semId / NB_USED_HSEM_PER_CORE);
}

void sem_GenerateIrq(t_semaphore_id semId)
{
    pHwSemRegs->sem[semId] = CORE_ID_2_HW_CORE_ID(ATTR(myCoreId)+1); //TODO : Could be value one ?
    MMDSP_FLUSH_DATABUFFER(); //TODO : really need ?
    pHwSemRegs->sem[semId] = HSEM_INTRA_MASK|HSEM_INTRB_MASK|HSEM_INTRC_MASK|HSEM_INTRD_MASK; /* all interrupt requested (filtering done by mask programmation at CM level) */
}
