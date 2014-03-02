/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private header file for the PWL Pulse width light modulator module
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _HCL_PWL_HWP_H_
#define _HCL_PWL_HWP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#ifndef _HCL_DEFS_H
#include "hcl_defs.h"
#endif

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

/* HW access macros */
#define GET_PWL_LEVEL(c)    ((c) = HCL_READ_BITS(g_pwl_system_context.p_pwl_register->pwl_lvr, PWLVL_MASK))
#define SET_PWL_LEVEL(c)    HCL_WRITE_BITS(g_pwl_system_context.p_pwl_register->pwl_lvr, c, PWLVL_MASK)
#define ENABLE_PWL          HCL_SET_BITS(g_pwl_system_context.p_pwl_register->pwl_cr, PWLEN_MASK)
#define DISABLE_PWL         HCL_CLEAR_BITS(g_pwl_system_context.p_pwl_register->pwl_cr, PWLEN_MASK)

#define PWL_WRITE_BITS(reg, val, bit_nb, pos)   ((reg) = (((reg) &~(bit_nb << pos)) | (((val) & bit_nb) << pos)))

/* MASK defines */
#define PWLVL_MASK      (0x000000FFUL)  /* MASK_ALL8 */
#define PWLEN_MASK      MASK_BIT0
#define PWL_MIN_LEVEL   0x00UL
#define PWL_MAX_LEVEL   0xFFUL
#define ONE_BIT         0x1UL

/* PWL Integration Test Control Register*/
#define PWL_TEST_MODE_ENABLE 0

/* PWL Integration Test Output Register*/
#define PWL_OUTPUT_PWLOUT    0

/*--------------------------------------------------------------------------*
 * Peripheral Ids                                                           *
 *--------------------------------------------------------------------------*/
#define PWL_P_ID0  0x38
#define PWL_P_ID1  0x00
#define PWL_P_ID2  0x08
#define PWL_P_ID3  0x00

/*--------------------------------------------------------------------------*
 * P-Cell Ids                                                               *
 *--------------------------------------------------------------------------*/
#define PWL_CELL_ID0 0x0D
#define PWL_CELL_ID1 0xF0
#define PWL_CELL_ID2 0x05
#define PWL_CELL_ID3 0xB1

/* PWL registers */
typedef volatile struct
{
   /* Type      Name                           Description                         Offset                */
    t_uint32    pwl_cr;                     /* PWL Control Register                0x00        -- 1      */
    t_uint32    pwl_lvr;                    /* PWL Level Register                  0x04        -- 2      */
    t_uint32    reserved1[30];              /* RESERVED                            0x08-0x7C             */
    t_uint32    pwl_itcr;                   /* PWL Integration Test Control Reg    0x80        -- 3      */
    t_uint32    reserved2;                  /* RESERVED                            0x84                  */
    t_uint32    pwl_itop;                   /* PWL Integration test Output Reg.    0x88        -- 4      */
    t_uint32    reserved3[981];             /* RESERVED                            0x8C-0xFE0            */
    t_uint32    pwl_periphid0;              /* PWL Peripheral ID Reg 0             0xFE0                 */
    t_uint32    pwl_periphid1;              /* PWL Peripheral ID Reg 1             0xFE4                 */
    t_uint32    pwl_periphid2;              /* PWL Peripheral ID Reg 2             0xFE8                 */
    t_uint32    pwl_periphid3;              /* PWL Peripheral ID Reg 3             0xFEC                 */
    t_uint32    pwl_cellid0;                /* PWL PCell ID Reg 0                  0xFF0                 */ 
    t_uint32    pwl_cellid1;                /* PWL PCell ID Reg 1                  0xFF4                 */ 
    t_uint32    pwl_cellid2;                /* PWL PCell ID Reg 2                  0xFF8                 */ 
    t_uint32    pwl_cellid3;                /* PWL PCell ID Reg 3                  0xFFC                 */ 
} t_pwl_register;

typedef volatile struct
{
    t_pwl_register  *p_pwl_register;
} t_pwl_system_context;

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _HCL_PWL_HWP_H_ */

