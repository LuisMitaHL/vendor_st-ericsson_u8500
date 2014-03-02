/*******************************************************************************
* $Copyright ST-Ericsson 2009 $
********************************************************************************
*
* FILE: 'mmu.c'
*
* DESCRIPTION:
* Target HW specific settings for MMU are made in this file
*
********************************************************************************
*
* REVISION HISTORY
*
* $Log: \LD_PlatformSec_Pub\cnh1606560_ldr_prologue\source\mmu.c $
 *
 *    Version: ...\cnh1606560\cnh1606560_r2\4 20 Nov 2009 14:02 (GMT) xvsalra
 * Code review remarks resolved
 *
 *    Version: ...\cnh1606560\cnh1606560_r2\3 30 Oct 2009 12:48 (GMT) XMARDRN
 * Optimization code rollback
 *
 *    Version: ...\cnh1606560\cnh1606560_r2\1 24 Sep 2009 16:57 (GMT) XVSZOAN
 * Changes for P570.
 * New CABS and NAND PD labels.
 *
 *    Version: ...\cnh1606560\1 13 Aug 2009 06:05 (GMT) XVSVLPI
 * ewa porting
 *
 *    Version: ...\cnh1606560\cnh1606560_r1\4 03 Jul 2009 15:34 (GMT) xvsalra
 * Code Review remarks solved
 *
 *    Version: ...\cnh1606560\cnh1606560_r1\3 16 Apr 2009 06:54 (GMT) XVSVLPI
 * update merge from main branch replica
*
*******************************************************************************/

/*
 * @addtogroup mmu_init
 * @file mmu.c
 * @brief Target HW specific settings for MMU are made in this file
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include <malloc.h>
#include "r_mmu.h"
#include "l2cc_regs.h"
#include "r_basicdefinitions.h"
#include "error_codes.h"
#include "cpu_support.h"
#include "memory_mapping.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#if defined(STE_AP9540) || defined(STE_AP8540)
static uint32 *COPYTable_p = NULL;
#else
static uint32 *TranslationTable_p = NULL;
#endif

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
ErrorCode_e Start_MMU(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = PD_MMU_setup();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

#if !defined(STE_AP9540) && !defined(STE_AP8540)
    ReturnValue = PD_MMU_SetupRegionAccessRights(SDRAM_BANK_0_BASE_ADDR, SDRAM_BANK_1_END_ADDR, NORMAL_I_WT_ON_O_WT_ON | RW_ACCESS);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = PD_MMU_SetupRegionAccessRights(EMBEDDED_SRAM_BASE_ADDR, EMBEDDED_SRAM_END_ADDR, NORMAL_I_OFF_O_OFF | RW_ACCESS);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#endif

    MMU_Enable();

ErrorExit:
    return ReturnValue;
}

ErrorCode_e PD_MMU_setup(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

#if defined(STE_AP9540) || defined(STE_AP8540)
    uint32 *XLoaderTBL_p = NULL;
    uint32 TBLADDR = 0;
#else
    uint32 i = 0;
#endif

    /* Disable MMU before changing content of the MMU Table */
    MMU_Disable();

#if defined(STE_AP9540) || defined(STE_AP8540)
    TBLADDR = PD_CPU_Read_TTB_Address();
    TBLADDR = TBLADDR & 0xFFFFFF00;
    XLoaderTBL_p = (uint32 *)TBLADDR;

    if (NULL == COPYTable_p) {
        COPYTable_p = (uint32 *)memalign(MMU_TABLE_ALIGN, MMU_TABLE_SIZE);
        VERIFY(NULL != COPYTable_p, E_ALLOCATE_FAILED);
    }

    memset(COPYTable_p, 0x00, MMU_TABLE_SIZE);
    memcpy(COPYTable_p, XLoaderTBL_p, MMU_TABLE_SIZE);

    /* Registering new Translation Table */
    PD_CPU_SetNew_TTB_Address((uint32)COPYTable_p);
#else

    if (NULL == TranslationTable_p) {
        TranslationTable_p = (uint32 *)memalign(MMU_TABLE_ALIGN, MMU_TABLE_SIZE);
        VERIFY(NULL != TranslationTable_p, E_ALLOCATE_FAILED);
    }

    memset(TranslationTable_p, 0x00, MMU_TABLE_SIZE);

    /* Writing default Access rights */
    for (i = 0; i < NB_MMU_TABLE_ENTRIES; i++) {
        TranslationTable_p[i] = DEFAULT_MMU_ACCESS + MMU_SECTION_SIZE * i;
    }

    /* Registering new Translation Table */
    PD_CPU_SetNew_TTB_Address((uint32)TranslationTable_p);
#endif

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

#if !defined(STE_AP9540) && !defined(STE_AP8540)
ErrorCode_e PD_MMU_SetupRegionAccessRights(uint32 VA_Start, uint32 VA_End, uint32 AccessRights)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    sint32 i;
    uint32 *MMU_Table_p = NULL;
    sint32  MMU_Table_offset = VA_Start >> SECTION_BA_OFFSET;
    sint32  MMU_Table_End = VA_End >> SECTION_BA_OFFSET;
    sint32  MMU_Table_offset_Size = MMU_Table_End - MMU_Table_offset;

    /* Input parameter check */
    VERIFY((MMU_Table_offset_Size >= MINIMUM_SECTION_BA) && (MMU_Table_offset_Size <= MAXIMUM_SECTION_BA), E_INVALID_INPUT_PARAMETERS);

    /* Finding MMU Table section start */
    MMU_Table_p = &(TranslationTable_p[MMU_Table_offset]);

    /* Disable MMU before changing content of the MMU Table */
    MMU_Disable();

    /* Changing Access rights of the requested region */
    for (i = 0; i < (MMU_Table_offset_Size + 1); i++) {
        MMU_Table_p[i] = ((MMU_Table_p[i] & ACCESS_MASK) | AccessRights);
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}
#endif

void MMU_Disable(void)
{
    PD_CPU_DataSynchBarrier();      // Drain write buffer
    //PD_CPU_DCacheCleanInvalidate(); // Flush DCACHE
    PD_CPU_DCacheDisable();         // Disable L1  DCACHE
    PD_CPU_ICacheFlush();           // Invalidate  ICACHE
    PD_CPU_ICacheDisable();         // Disable L1  ICACHE

    //Disable in CP15!
    PD_CPU_MMUDisable();
    PD_MMU_InvalidateTLB();

}

void MMU_Enable(void)
{
    PD_CPU_DCacheFlush();          // Flush DCACHE
    PD_CPU_ICacheFlush();          // Flush ICACHE
    PD_CPU_DataSynchBarrier();     // Drain write buffer
    PD_MMU_InvalidateTLB();
    PD_CPU_ChangeDomainAccess(DEFAULT_MMU_CONTROL_REGISTER);

    /* New MMU Table is ready to use */
    CPU_BranchPredictionEnable();
    PD_CPU_DCacheEnable();         // Enable Data Cache
    PD_CPU_ICacheEnable();         // Enable Instruction Cache

    PD_CPU_MMUEnable();
}

void MMU_InvalidateL2(uint32 Physical_Address, uint32 Data_Size)
{
    volatile t_l2cc_registers *const p_L2ccRegs = (t_l2cc_registers *)L2CC_REG_START_ADDR;
    uint32 Invalidate_Address = Physical_Address & (~31);

    if (p_L2ccRegs->Control.Enable == 1) {
        do {
            p_L2ccRegs->InvLineByPhysAddr = Invalidate_Address;
            Invalidate_Address += 32;
        } while (Invalidate_Address < (Physical_Address + Data_Size));
    }

    return;
}

/* @} */
