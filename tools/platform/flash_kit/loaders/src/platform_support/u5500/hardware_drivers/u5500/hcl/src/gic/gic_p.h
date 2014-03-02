/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of Generic Interrupt Controller (GIC) module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _GICP_H_
#define _GICP_H_

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "gic.h"

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/


#define GIC_P_ID0   0x90
#define GIC_P_ID1   0xB3
#define GIC_P_ID2   0x1B
#define GIC_P_ID3   0x00


/* GIC_P_ID4-7 TBD */
#define GIC_CELL_ID0    0x0D
#define GIC_CELL_ID1    0xF0
#define GIC_CELL_ID2    0x05
#define GIC_CELL_ID3    0xB1

/* Interrupts numbers */
#define GIC_IT_MAX_NUM  (GIC_MAX_IT_LINES)

/* Priority limit values */
/* For 8500 */
#define GIC_FIRST_PRIORITY          0x00
#define GIC_LAST_PRIORITY           0x1F
#define GIC_PRIORITY_NOT_CONFIGURED 0xFF

/* Default information linked to each interrupt */
#define GIC_NO_DATUM    0xFFFFFFFF

/* MASK VALUE TO ENABLE THE ALL GIC LINES */
/* __STN_8500 */
#define GIC_EN_ALL_LINES_MASK_1 0xFFFFFFFF
#define GIC_EN_ALL_LINES_MASK_2 0xFFFFFFFF
#define GIC_EN_ALL_LINES_MASK_3 0xFFFFFFFF
#define GIC_EN_ALL_LINES_MASK_4 0xFFFFFFFF

/* MASK VALUE FOR TEST */
#define GIC_EN_TEST_MASK1 	0x80000810
#define GIC_EN_TEST_MASK2	0xFFFFFFFF
#define GIC_EN_TEST_MASK3	0x00000000


#define ACK_TIMECOUNT		300

/*--------------------------------------------------------------------------*
 * Local Functions															*
 *--------------------------------------------------------------------------*/
/* PRIVATE t_uint8 gic_LineToPriority(t_gic_line);*/

/*--------------------------------------------------------------------------*
 * New internal types														*
 *--------------------------------------------------------------------------*/
#define GIC_SET_BIT(reg_name, mask)                     HCL_SET_BITS(reg_name, mask)
#define GIC_CLR_BIT(reg_name, mask)                     HCL_CLEAR_BITS(reg_name, mask)
#define GIC_WRITE_BIT(reg_name, val, mask)              HCL_WRITE_BITS(reg_name, val, mask)
#define GIC_TEST_BIT(reg_name, val)                     HCL_READ_BITS(reg_name, val)
#define GIC_WRITE_REG(reg_name, val)                    HCL_WRITE_REG(reg_name, val)
#define GIC_READ_REG(reg_name)                          HCL_READ_REG(reg_name)
#define GIC_CLEAR                                       MASK_NULL32

#define GIC_WRITE_FIELD(reg_name, mask, shift, value)   (reg_name = ((reg_name &~mask) | (value << shift)))
#define GIC_READ_FIELD(reg_name, mask, shift)           ((reg_name & mask) >> shift)

/* Interface Control Register */
/* Mask values for control register mask */
#define GIC_ICR_ENS     MASK_BIT0
#define GIC_ICR_ENNS    MASK_BIT1
#define GIC_ICR_ACKTL   MASK_BIT2
#define GIC_ICR_FIQEN   MASK_BIT3
#define GIC_ICR_SBPR    MASK_BIT4

/*  shift valus for control register bit fields */
#define GIC_ICR_SHIFT_ENNS  1
#define GIC_ICR_SHIFT_ACKTL 2
#define GIC_ICR_SHIFT_FIQEN 3
#define GIC_ICR_SHIFT_SBPR  4

/* Distributor Control Register Type */
/* Mask values for control register mask */
#define GIC_ICT_SPI_NUM 0x1F
#define GIC_ICT_CPU_NUM 0xE0

/* Mask values for Interrupt acknowledge register bit fields */
#define GIC_IT_ACK_ID       0x000003FF
#define GIC_IT_ACK_EOI_MASK 0x00001FFF

/*  shift valus for control register bit fields */
#define GIC_ICT_SHIFT_CPU_NUM   5

/*  shift valus for interrupt acknowledge register bit fields */
#define GIC_IT_ACK_SHIFT_SRC_CPU    10

/* STI trigger register */
#define TARG_LIST_BIT  16
#define TARG_LIST_FILTER_BIT 24

/* OFFSET values for the distributor register parameters */
/*  For interrupt Security Register */
/* For Interrupt Config Register */
/* For Interrupt SPI Target Register */
/* For Enable Set Register */
/* For Disable Clear Register */
/* GIC registers */
/* Hardware registers description */
typedef volatile struct
{
    /*Type	 Name								 Description						Offset */
    /* CPU Interface registers */
    t_uint32    cpu_icr;
    t_uint32    prio_mask;
    t_uint32    bin_point;
    t_uint32    it_ack;
    t_uint32    eoi;
    t_uint32    run_prio;
    t_uint32    hi_pend;
    t_uint32    alias_bin_pt_ns;
    t_uint32    reserved_0[(0xFB - 0x1C) >> 2];
    t_uint32    cpu_ident;
    t_uint32    reserved[(0xFFF - 0x1FC) >> 2];
    /* Distributor registers */
    //t_uint32    enable_ns;                      /* Interrupt enable						0x000 */
    t_uint32    it_dis_ctr_type;                /* FIQ Status	    				0x004 */
    t_uint32    it_ctr_type;                    /* Raw Interrupt status				0x008 */
    t_uint32    it_dis_imp_id;
    t_uint32    reserved_1[(0x80 - 0x0C) >> 2];
    t_uint32    it_sec[32];
    t_uint32    it_en_set[32];
    t_uint32    it_en_clr[32];
    t_uint32    it_pen_set[32];
    t_uint32    it_pen_clr[32];
    t_uint32    it_active_status[32];
    t_uint32    reserved_2[(0x3FF - 0x37C) >> 2];
    t_uint32    it_prio_level[32];
    t_uint32    reserved_3[(0x7FF - 0x47C) >> 2];
    t_uint32    it_spi_target[32];
    t_uint32    reserved_4[(0xBFF - 0x87F) >> 2];
    t_uint32    it_config[64];
    t_uint32    it_ppi_status;
    t_uint32    it_spi_status[31];
    t_uint32    reserved_5[(0xEFF - 0xD7C) >> 2];
    t_uint32    it_sti_trig;
    t_uint32    reserved_6[(0xFDF - 0xF00) >> 2];
    t_uint32    periph_id_0;
    t_uint32    periph_id_1;
    t_uint32    periph_id_2;
    t_uint32    periph_id_3;
    t_uint32    pcell_id_0;
    t_uint32    pcell_id_1;
    t_uint32    pcell_id_2;
    t_uint32    pcell_id_3;
} t_gic_register;

typedef struct
{
    t_uint32    it_cpu_ens;
    t_uint32    it_cpu_enns;
    t_uint32    acktl;
    t_uint32    fiqen;
    t_uint32    sbpr;
    t_uint32    spi_num;
    t_uint32    cpu_num;
    t_uint32    lspi_num;
    t_uint32    en_set_1;
    t_uint32    pen_set_1;
} t_gic_device_context;

typedef struct
{
    t_gic_device_context    gic_device_context;
    t_gic_register          *p_gic_register;    /* Pointer to GIC registers structure */
} t_gic_system_context;


#endif

/*_GICP_H_ */
/* End of file - gicp.h */

