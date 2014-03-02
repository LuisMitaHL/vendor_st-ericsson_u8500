/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef R_PD_MMU_H_
#define R_PD_MMU_H_

/**
 *  @file r_mmu.h
 *  @brief Extern function definitions for mmu.c file
 *  @addtogroup prologue Prologue
 *  @brief Prologue module is used to initialize MMU and SDRAM.
 *
 *  In special cases this is the first loader executable code downloaded
 *  to the ME. Even though PROTROM commands can be used to setup SDRAM available
 *  on platform after ME is booted, in some special cases prologue is needed to
 *  initialize types of not very common DRAM or other types of memories that can
 *  not be initialized by PROTROM commands. Prologue is also used to initialize
 *  the basic memory layout (Read-Only, Read-Write and No Access area). Final
 *  memory lock down is done in the production loader. After the execution of
 *  prologue, control over platform is returned to BootROM code that can accept
 *  next loader. Currently there are no restrictions what type of
 *  initializations can be made by the prologue. This means that in prologue
 *  functions for initialization of specific customer hardware can exist that
 *  will initialize them prior loaders are loaded and started.
 *
 *  @{
 *
 *    @addtogroup mmu_init Memory Management Unit Initialization
 *    @{
 *
 */
/*******************************************************************************
* Includes
*******************************************************************************/
#include "t_mmu.h"
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 *  Starts the memory management unit
 *
 *  @retval E_GENERAL_FATAL_ERROR      If execution failed.
 *  @retval E_SUCCESS                  After successful execution.
 *
 */
ErrorCode_e Start_MMU(void);

/**
 *  This function creates initial first lavel MMU Page Table. Begining of the
 *  first lavel page table is defined in scatter file.
 *
 *  After executing of this function memory configuration will have
 *  Virtual address = Physical Address and all regions will be set
 *  according to DEFAULT_MMU_ACCESS.
 *  This mapping shall only be done by the prologue.
 *
 *
 *  @retval E_GENERAL_FATAL_ERROR      If execution failed.
 *  @retval E_SUCCESS                  After successful execution.
 *
 */
ErrorCode_e PD_MMU_setup(void);


#if !defined(STE_AP9540) && !defined(STE_AP8540)
/**
 *  This function change access rights of any memory region from Virtual
 *  address start (VA_Start) to Virtual Address End (VA_End) into AccessRights.
 *  Possible combinations of AccessRights are defined in t_mmu.h file.
 *  VA_Start and VA_End addresses are 1M byte align.
 *  If you set VA_Start=VA_End, then you will change settings for just one
 *  1M byte section.
 *
 *  This function must be executed after PD_MMU_setup function.
 *
 *  @param [in]  VA_Start           The virtual address start location.
 *  @param [in]  VA_End             The virtual address end location.
 *  @param [in]  AccessRights       Access rights for region.
 *
 *  @retval E_GENERAL_FATAL_ERROR   If execution failed.
 *  @retval E_SUCCESS               After successful execution.
 *
 */
ErrorCode_e PD_MMU_SetupRegionAccessRights(uint32 VA_Start,
        uint32 VA_End,
        uint32 AccessRights);
#endif


/**
 *  This function enables usage of MMU.
 *
 *
 * @return None.
 */
void MMU_Enable(void);

/**
 * This function disables usage of MMU.
 *
 *
 * @return None.
 */
void MMU_Disable(void);

/**
 * This function invalidates data in L2 cache.
 *
 *
 * @return None.
 */
void MMU_InvalidateL2(uint32 Physical_Address, uint32 Data_Size);

/** @} */
/** @} */
#endif /* R_MMU_H_ */
