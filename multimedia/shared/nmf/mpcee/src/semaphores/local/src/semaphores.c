/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* semaphores/local/src/semaphores.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#include <semaphores/local.nmf>

#include <semaphores/inc/semaphores.h>
#include <inc/mmdsp_mapping.h>
#include <inc/archi-wrapper.h>


const t_uint16 ATTR(myCoreId) = 0;

struct local_reg {
    unsigned int value;
    unsigned int dummy;
};
static volatile struct local_reg __MMIO *hw_sem = (volatile struct local_reg __MMIO *)MMDSP_SEMAPHORE_BASE;
static volatile struct local_reg __MMIO *it_remap_reg = (volatile struct local_reg __MMIO *)MMDSP_IT_REMAP_BASE;

void sem_Init(t_uword usedIntLevel)
{
    /* Remap ARM -> DSP interrupt (HOST_cmd0: 11) on given level */
    it_remap_reg[usedIntLevel].value = HOST_0_INT_NUMBER;
    /* Unmask usedIntLevel interrupt */
    ITREMAP_interf_itmskl &= ~(1UL<<usedIntLevel);

    /* Unmask DSP -> ARM interrupt */
    HOST[HOST_IntPol]= 0U; /* active high */
    HOST[HOST_Inte0] = 0x1U;
}

t_nmf_core_id sem_GetFromCoreIdFromIrqSrc(void)
{
    static int emulIrqSrc = 1;

    if (emulIrqSrc != 0)
    {
        emulIrqSrc = 0;
        return 0;
    }
    else
    {
        emulIrqSrc = 1;
        return LAST_CORE_ID + 1;
    }
}

t_nmf_core_id sem_GetToCoreIdFromSemId(t_semaphore_id semId)
{
    return 0; /* With local semaphore, we only support communication between host CPU and the given dsp */
}

void sem_GenerateIrq(t_semaphore_id semId)
{
    /* Assert ARM communication interrupt */
    HOST[HOST_Int0] = 1U;
}

