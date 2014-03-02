#ifndef INCLUSION_GUARD_CPU_SUPPORT_H
#define INCLUSION_GUARD_CPU_SUPPORT_H
// * ---------------------------------------------------------------------------
// * $Copyright ST-Ericsson 2009 $
// * The copyright to the document(s) herein is the property of Ericsson AB,
// * Sweden
// *
// * The document(s) may be used and/or copied only with the written
// * permission from Ericsson AB or in accordance with the terms and
// * conditions stipulated in the agreement/contract under which the
// * document(s) have been supplied.
// * ---------------------------------------------------------------------------
// * File name  : cpu_support.h
// * File type  : C header
// * ---------------------------------------------------------------------------
// * Description:
// *   CPU supporting functions
// * ---------------------------------------------------------------------------
// * References:
// *
// * ---------------------------------------------------------------------------
// * Language:    ANSI C (C++ comments allowed)
// * ---------------------------------------------------------------------------
/**  \file cpu_support
     \brief CPU supporting functions
*/
#ifdef MACH_TYPE_DB5700
#include "t_asicdefinitions.h"
#endif

/*!
 \enum CPU_Irq_State_t
 \brief This enum defines the interrupt state
*/
typedef enum {
    CPU_IRQ_ENABLE = 0,   /*!< IRQ Enabled */
    CPU_IRQ_DISABLE       /*!< IRQ Disabled */
} CPU_Irq_State_t;

/*!
 \enum CPU_Fiq_State_t
 \brief This enum defines the fast interrupt state
*/
typedef enum {
    CPU_FIQ_ENABLE = 0,   /*!< FIQ Enabled */
    CPU_FIQ_DISABLE       /*!< FIQ Disabled */
} CPU_Fiq_State_t;


//******************************************************************************
/*!
     \brief Enables IRQ interrupts in the CPU status register. Works only in
            privileged mode.
*/
//******************************************************************************
void CPU_IrqEnable(void);


//******************************************************************************
/*!
     \brief Disables IRQ interrupts in the CPU status register. Works only in
            privileged mode.

     \return      CPU_Irq_State_t                The previous state
*/
//******************************************************************************
CPU_Irq_State_t CPU_IrqDisable(void);


//******************************************************************************
/*!
     \brief Enables FIQ interrupts in the CPU status register. Works only in
            privileged mode.
*/
//******************************************************************************
void CPU_FiqEnable(void);


//******************************************************************************
/*!
     \brief Disables FIQ interrupts in the CPU status register. Works only in
            privileged mode.

     \return      CPU_Irq_State_t                The previous state
*/
//******************************************************************************
CPU_Fiq_State_t CPU_FiqDisable(void);


//******************************************************************************
/*!
     \brief Enables the instruction cache. Works only in privileged mode.
*/
//******************************************************************************
void CPU_ICacheEnable(void);


//******************************************************************************
/*!
     \brief Disables the instruction cache. Works only in privileged mode.
*/
//******************************************************************************
void CPU_ICacheDisable(void);


//******************************************************************************
/*!
     \brief Enables branch prediction. Works only in privileged mode.
*/
//******************************************************************************
void CPU_BranchPredictionEnable(void);


//******************************************************************************
/*!
     \brief Disables branch prediction. Works only in privileged mode.
*/
//******************************************************************************
void CPU_BranchPredictionDisable(void);


//******************************************************************************
/*!
     \brief Enables unaligned data access. Works only in privileged mode.
*/
//******************************************************************************
void CPU_UnalignAccessEnable(void);


//******************************************************************************
/*!
     \brief Disables unaligned data access. Works only in privileged mode.
*/
//******************************************************************************
void CPU_UnalignAccessDisable(void);


//******************************************************************************
/*!
     \brief Enable data cache. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_DCacheEnable(void);


//******************************************************************************
/*!
     \brief Disables data cache. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_DCacheDisable(void);


//******************************************************************************
/*!
     \brief Enable Instruction cache. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_ICacheEnable(void);

//******************************************************************************
/*!
     \brief Disable Instruction cache. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_ICacheDisable(void);


//******************************************************************************
/*!
     \brief Stall execution until all outstanding accesses in the write buffer
            are completed. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_DataSynchBarrier(void);


//******************************************************************************
/*!
     \brief Flush data cache. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_DCacheFlush(void);

//******************************************************************************
/*!
     \brief Flush specified address data cache. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_DCacheInvalidateMVA(uint32 MVA);

//******************************************************************************
/*!
     \brief Flush instruction cache. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_ICacheFlush(void);


//******************************************************************************
/*!
     \brief Test and clean the whole data cache. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_DCacheTestClean(void);


//******************************************************************************
/*!
     \brief Clean and invalidate the whole data cache. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_DCacheCleanInvalidate(void);


//******************************************************************************
/*!
     \brief Returns cache details. Works only in privileged mode.
*/
//******************************************************************************
void PD_CPU_CacheDetails(void);


//******************************************************************************
/*!
     \brief Returns cache details. Works only in privileged mode.
*/
//******************************************************************************
void PD_MMU_InvalidateTLB(void);

/*************************************************************************
*        Name: PD_CPU_MMUEnable
*  Parameters: -
*     Returns: -
* Description: Enable MMU.
*        CPUs: ARM1176
**************************************************************************/
void PD_CPU_MMUEnable(void);

/*************************************************************************
*        Name: PD_CPU_MMUDisable
*  Parameters: -
*     Returns: -
* Description: Disable MMU.
*        CPUs: ARM1176
**************************************************************************/
void PD_CPU_MMUDisable(void);

/*************************************************************************
*        Name: PD_CPU_SetNew_TTB_Address
*  Parameters: [in]  TTB_Address          New MMU table base address.
*     Returns: -
* Description: Register new TTB addres after change. It must be called
*              after generation of the MMU table contest. This function
*              writes new MMU base address in CP15.
*        CPUs: ARM1176
**************************************************************************/
void PD_CPU_SetNew_TTB_Address(uint32 TTB_Address);

/*************************************************************************
*        Name: PD_CPU_Read_TTB_Address
*  Parameters: [out]  TTB_Address        MMU table base address.
*     Returns: -
* Description: Reads MMU base address in CP15.
*        CPUs: ARM1176
**************************************************************************/
uint32 PD_CPU_Read_TTB_Address();

/*************************************************************************
*        Name: PD_CPU_ChangeDomainAccess
*  Parameters: [in]  DomainAssessControl    New Domain Access Control value
*                                           for all domains.
*     Returns: -
* Description: ARM11 has 16 memory domains(D0-D15).The purpose of the fields
*              D15-D0 is to define the access permissions for each of the 16
*               domains. These domains can be either sections, large pages
*               or small pages of memory.
*
*              Domain Access Control Register format(cp15 reg3):
*
*              31 30|29 28|27 26| ... | 5 4 | 3 2 | 1 0 |
*               D15 | D14 | D13 | ... | D2  | D1  | D0  |
*
*               Value |Access type| Description
*             -------------------------------------------------------------
*               b00   |No access  |Any access generates a domain fault.
*             -------------------------------------------------------------
*               b01   |Client     |Accesses are checked against the access
*                     |           |permission bits in the TLB entry.
*             -------------------------------------------------------------
*               b10   |Reserved   |Any access generates a domain fault.
*             -------------------------------------------------------------
*               b11   |Manager    |Accesses are not checked against the
*                     |           |access permission bits in the TLB entry,
*                     |           |so a permission fault cannot be generated.
*             -------------------------------------------------------------
*        CPUs: ARM1176
**************************************************************************/
void PD_CPU_ChangeDomainAccess(uint32 DomainAssessControl);

#endif //INCLUSION_GUARD_CPU_SUPPORT_H
