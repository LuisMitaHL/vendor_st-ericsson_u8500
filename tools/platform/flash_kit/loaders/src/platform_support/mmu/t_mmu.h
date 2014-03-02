/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef T_MMU_H_
#define T_MMU_H_

/**
 * @file t_mmu.h
 * @brief Types and constants definitions for mmu.c file
 * @addtogroup prologue
 * @{
 *
 *    @addtogroup mmu_init
 *    @{
 *
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/**
 *  Maximum size of the MMU table (always 16kB for 4GB address range).
 */
#define MMU_TABLE_SIZE 0x4000

/**
 *  Alignment of MMU table
 */
#define MMU_TABLE_ALIGN 0x4000

/**
 *  Number of MMU table entries.
 */
#define NB_MMU_TABLE_ENTRIES (MMU_TABLE_SIZE / sizeof(uint32))

/**
 *  1MB is the default section size.
 */
#define MMU_SECTION_SIZE 0x100000

/** Value for minimum Section Base Address */
#define MINIMUM_SECTION_BA 0x000

/** Value for maximum Section Base Address */
#define MAXIMUM_SECTION_BA 0xFFF

/** Offset for Section Base Address */
#define SECTION_BA_OFFSET  20

/**
 *  Access control register value. 0x55555555 equ ( Domains D0-D15 client ).
 */
#define DEFAULT_MMU_CONTROL_REGISTER 0x55555555

/**********************************************************************
 *                                                                    *
 *  MMU Table descriptor format                                       *
 *                                                                    *
 *  Bit position:   15   14:12    11:10     8:5    4   3   2   1:0    *
 *                AP[2]   TEX    AP[1:0]  Domain  Xn   C   B  Section *
 *                                                                    *
 **********************************************************************/

#define DEFAULT_MMU_ACCESS      0x0C02

/**
 *  Value of the access mask for MMU table.
 */
#define ACCESS_MASK             0xFFFF0003

/*
 * Cache policy.
 * I - Inner (L1) cache policy
 * O - Outer (L2) cache policy
 */
/** Inner (L1) No cachable*/
#define I_NON_CACHABLE            0x0000

/** Outer (L2) No cachable*/
#define O_NON_CACHABLE            0x0000

/** Inner (L1) Write-Back cached, Write allocate*/
#define I_WB_CACHE_WR_ALLOCATE    0x0006

/** Outer (L2) Write-Back cached, Write allocate*/
#define O_WB_CACHE_WR_ALLOCATE    0x1000

/** Inner (L1) Write-Through cached, No allocate on write*/
#define I_WT_CACHE_NO_ALLOCATE_WR 0x0008

/** Outer (L2) Write-Through cached, No allocate on write*/
#define O_WT_CACHE_NO_ALLOCATE_WR 0x2000

/** Inner (L1) Write-Back cached, No allocate on write*/
#define I_WB_CACHE_NO_ALLOCATE_WR 0x000C

/** Outer (L2) Write-Back cached, No allocate on write*/
#define O_WB_CACHE_NO_ALLOCATE_WR 0x3000

/**
 * TEX Field
 */
#define TEX                       0x4000

/*
 * Memory region attributes.
 */
/** Normal memory, Inner cache enabled, Outer cache enabled*/
#define NORMAL_I_WT_ON_O_WT_ON   (TEX | I_WT_CACHE_NO_ALLOCATE_WR | O_WT_CACHE_NO_ALLOCATE_WR)

/** Normal memory, Inner cache enabled, Outer cache enabled*/
#define NORMAL_I_WB_ON_O_WB_ON   (TEX | I_WB_CACHE_NO_ALLOCATE_WR | O_WB_CACHE_NO_ALLOCATE_WR)

/** Normal memory, Inner cache enabled, Outer cache disabled*/
#define NORMAL_I_WT_ON_O_OFF  (TEX | I_WT_CACHE_NO_ALLOCATE_WR | O_NON_CACHABLE)

/** Normal memory, Inner cache enabled, Outer cache disabled*/
#define NORMAL_I_WB_ON_O_OFF  (TEX | I_WB_CACHE_NO_ALLOCATE_WR | O_NON_CACHABLE)

/** Normal memory, Inner cache disabled, Outer cache disabled*/
#define NORMAL_I_OFF_O_OFF (TEX | I_NON_CACHABLE | O_NON_CACHABLE)

/*******************************************
 *                                         *
 * Access permissions for memory sections. *
 *                                         *
 *******************************************/
/** Privileged permissions: No Access, User Permissions: No Access */
#define NO_ACCESS 0x0000

/** Privileged permissions: Read/Write, User Permissions: No Access */
#define RW_ACCESS 0x0400

/** Privileged permissions: Read-only, User Permissions: Read-only */
#define RO_ACCESS 0x8400


/** @} */
/** @} */
#endif /* T_MMU_H_ */
