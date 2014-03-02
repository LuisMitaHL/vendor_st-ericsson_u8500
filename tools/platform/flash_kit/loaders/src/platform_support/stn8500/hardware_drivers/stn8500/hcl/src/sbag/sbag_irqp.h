/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file for MSP
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SBAG_IRQP_H_
#define _SBAG_IRQP_H_

#ifdef __cplusplus
extern "C"
{
#endif

    /*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "sbag_irq.h"

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
#define SBAG_TEST_BIT(reg_name, val)                    HCL_READ_BITS( reg_name, val )
#define SBAG_WRITE_FIELD(reg_name, value, mask, shift)  (reg_name |= ((reg_name &~mask) | (value << shift)))
#define SBAG_READ_FIELD(reg_name, mask, shift)          ((reg_name & mask) >> shift)
#define SBAG_ENABLE_DEVICE                              0x1
#define SBAG_MAX_OPERATING_MODE                         0x2

#define SBAG_SHIFT_BY_SIXTEEN                           16
#define SBAG_GET_INTERRUPTS                             0x0000FFFF
#define SBAG_IRQ_SRC_ALL                                0x0000FFFF

#define SBAG_OPERATING_MODE_MASK                        ( MASK_BIT0 | MASK_BIT1 | MASK_BIT2 )
#define SBAG_ENABLE                                     0x1
#define SBAG_DISABLE                                    0x0

#define SBAG_WPSAT_CLR_PGM_MODE                         MASK_BIT0
#define SBAG_TMSAT_CLR_PGM_MODE                         MASK_BIT0

#define SBAG_WPSAT_TRACE_TIME_MASK                      MASK_BIT24
#define SBAG_WPSAT_TRACE_DATA_MASK                      MASK_BIT8
#define SBAG_WPSAT_TRACE_CELL2_MASK                     MASK_BIT1
#define SBAG_WPSAT_TRACE_CELL1_MASK                     MASK_BIT0
#define SBAG_WPSAT_TRACE_TIME_SHIFT                     24
#define SBAG_WPSAT_TRACE_DATA_SHIFT                     8
#define SBAG_WPSAT_TRACE_CELL2_SHIFT                    1
#define SBAG_WPSAT_TRACE_CELL1_SHIFT                    0

#define SBAG_TMSAT_TRACE_LMETRIC1_MASK                  MASK_BIT0
#define SBAG_TMSAT_TRACE_LMETRIC2_MASK                  MASK_BIT1
#define SBAG_TMSAT_TRACE_LMETRIC3_MASK                  MASK_BIT2
#define SBAG_TMSAT_TRACE_LERR_MASK                      MASK_BIT3
#define SBAG_TMSAT_TRACE_LMETRIC1_SHIFT                 0
#define SBAG_TMSAT_TRACE_LMETRIC2_SHIFT                 1
#define SBAG_TMSAT_TRACE_LMETRIC3_SHIFT                 2
#define SBAG_TMSAT_TRACE_LERR_SHIFT                     3
#define SBAG_GET_TMSAT_ID                               0xF7

#define SBAG_MSG_REF_PACE_MIN                           150
#define SBAG_MSG_REF_PACE_MASK                          0x7FFFFFFF

#define SBAG_FPF_MSG_MASK                               MASK_BIT31
#define SBAG_FPF_TRACE_DATA_MASK                        MASK_BIT0
#define SBAG_FPF_TRACE_TIME_MASK                        MASK_BIT24
#define SBAG_FPF_TRACE_DATA_SHIFT                       0
#define SBAG_FPF_TRACE_TIME_SHIFT                       24

#define SBAG_PI_MSG_MASK                                MASK_BIT31

#define SBAG_REF_MSG_MASK                               MASK_BIT31

#define SBAG_ERR_MSG_MASK                               MASK_BIT31

#define SBAG_ERR_TRACE_FLAGS0_MASK                      MASK_BIT0
#define SBAG_ERR_TRACE_FLAGS1_MASK                      MASK_BIT8
#define SBAG_ERR_TRACE_FLAGS2_MASK                      MASK_BIT9
#define SBAG_ERR_TRACE_FLAGS3_MASK                      MASK_BIT16
#define SBAG_ERR_TRACE_FLAGS0_SHIFT                     0
#define SBAG_ERR_TRACE_FLAGS1_SHIFT                     8
#define SBAG_ERR_TRACE_FLAGS2_SHIFT                     9
#define SBAG_ERR_TRACE_FLAGS3_SHIFT                     16

#define SBAG_WPSAT_ADD_OUT_MASK                         MASK_BIT0
#define SBAG_WPSAT_START_ADD_MASK                       0xFFFFFFFC
#define SBAG_WPSAT_END_ADD_MASK                         SBAG_WPSAT_START_ADD_MASK
#define SBAG_WPSAT_REF_OPC_MASK                         MASK_BYTE0
#define SBAG_WPSAT_REF_SRC_MASK                         ( MASK_BYTE1 | MASK_BIT16 | MASK_BIT17 )
#define SBAG_WPSAT_REF_ROPC_MASK                        ( MASK_BIT18 | MASK_BIT19 | MASK_QUARTET5 | MASK_BIT24 | MASK_BIT25 )
#define SBAG_WPSAT_OPC_MASK                             SBAG_WPSAT_REF_OPC_MASK
#define SBAG_WPSAT_SRC_MASK                             SBAG_WPSAT_REF_SRC_MASK
#define SBAG_WPSAT_ROPC_MASK                            SBAG_WPSAT_REF_ROPC_MASK
#define SBAG_WPSAT_REQ_ENB                              MASK_BIT26
#define SBAG_WPSAT_RES_ENB                              MASK_BIT27
#define SBAG_WPSAT_OPC_OUT                              MASK_BIT29
#define SBAG_WPSAT_SRC_OUT                              MASK_BIT30
#define SBAG_WPSAT_ROPC_OUT                             MASK_BIT31
#define SBAG_WPSAT_ADD_OUT_SHIFT                        0
#define SBAG_WPSAT_START_ADD_SHIFT                      2
#define SBAG_WPSAT_END_ADD_SHIFT                        2
#define SBAG_WPSAT_REF_OPC_SHIFT                        0
#define SBAG_WPSAT_REF_SRC_SHIFT                        8
#define SBAG_WPSAT_REF_ROPC_SHIFT                       18
#define SBAG_WPSAT_OPC_MASK_SHIFT                       0
#define SBAG_WPSAT_SRC_MASK_SHIFT                       8
#define SBAG_WPSAT_ROPC_MASK_SHIFT                      18
#define SBAG_WPSAT_REQ_ENB_SHIFT                        26
#define SBAG_WPSAT_RES_ENB_SHIFT                        27
#define SBAG_WPSAT_OPC_OUT_SHIFT                        29
#define SBAG_WPSAT_SRC_OUT_SHIFT                        30
#define SBAG_WPSAT_ROPC_OUT_SHIFT                       31

#define SBAG_TMSAT_SRC_VAL_MASK                         ( MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 )
#define SBAG_TMSAT_OPC_VAL_MASK                         ( MASK_BIT10 | MASK_BIT11 | MASK_QUARTET3 | MASK_BIT16 | MASK_BIT17 )
#define SBAG_TMSAT_SRC_MASK                             SBAG_TMSAT_SRC_VAL_MASK
#define SBAG_TMSAT_OPC_MASK                             SBAG_TMSAT_OPC_VAL_MASK
#define SBAG_TMSAT_OPC_EN_MASK                          MASK_BIT19
#define SBAG_TMSAT_SRC_EN_MASK                          MASK_BIT18
#define SBAG_TMSAT_SRC_VAL_SHIFT                        0
#define SBAG_TMSAT_OPC_VAL_SHIFT                        10
#define SBAG_TMSAT_SRC_SHIFT                            0
#define SBAG_TMSAT_OPC_SHIFT                            10
#define SBAG_TMSAT_OPC_EN_SHIFT                         19
#define SBAG_TMSAT_SRC_EN_SHIFT                         18

#define SBAG_WPSAT_READ_SRC_MASK                        MASK_BYTE0 | MASK_BIT8 | MASK_BIT9
#define SBAG_WPSAT_READ_SRC_SHIFT                       0
#define SBAG_WPSAT_READ_OPC_MASK                        MASK_BIT10 | MASK_BIT11 | MASK_QUARTET3 | MASK_BIT16 | MASK_BIT17
#define SBAG_WPSAT_READ_OPC_SHIFT                       10
#define SBAG_WPSAT_READ_BE_MASK                         MASK_BIT18 | MASK_BIT19 | MASK_QUARTET5 | MASK_BIT24 | MASK_BIT25
#define SBAG_WPSAT_READ_BE_SHIFT                        18
#define SBAG_WPSAT_READ_TID_MASK                        0xFC000000
#define SBAG_WPSAT_READ_TID_SHIFT                       26
#define SBAG_WPSAT_READ_TID_BITS                        0x3
#define SBAG_WPSAT_READ_LSB_ADD_MASK                    0xFFFFFFFC
#define SBAG_WPSAT_READ_LSB_ADD_SHIFT                   2
#define SBAG_WPSAT_READ_REQ_NOT_RESP                    MASK_BIT0

#define TMSAT_METRIC1_ERROR                             MASK_BIT0
#define TMSAT_METRIC2_ERROR                             MASK_BIT1
#define TMSAT_METRIC3_ERROR                             MASK_BIT2
#define TMSAT_ERROR_ACT_MCT                             MASK_BIT3
#define TMSAT_LOST_ERROR                                MASK_BIT0

#define SBAG_PI_TRACE_TIME_MASK                         MASK_BIT24
#define SBAG_PI_TRACE_TIME_SHIFT                        24
#define SBAG_PI_TRACE_INCDATA_MASK                      MASK_BYTE0 | MASK_BYTE1
#define SBAG_PI_TRACE_INCDATA_SHIFT                     0
#define SBAG_PI_TRIGGER_RAISE_MASK                      MASK_BYTE0 | MASK_BYTE1
#define SBAG_PI_TRIGGER_FALL_MASK                       MASK_BYTE0 | MASK_BYTE1
#define SBAG_PI_TRIGGER_RAISE_SHIFT                     0
#define SBAG_PI_TRIGGER_FALL_SHIFT                      0

#define SBAG_WPSAT_DEVICE                               0x00000;
#define SBAG_PI_DEVICE                                  0x10000;
#define SBAG_TMSAT_DEVICE                               0x20000;

    /*-----------------SBAG Register Description--------------------------------*/
    typedef volatile struct
    {
        t_uint32    wpsat_reg1;
        t_uint32    wpsat_reg2;
        t_uint32    wpsat_reg3;
        t_uint32    wpsat_reg4;
        t_uint32    wpsat_req_not_rsp;
    } t_watchpoint_register;

    typedef volatile struct
    {
        t_uint32    tmsat_reg1;
        t_uint32    tmsat_reg2;
        t_uint32    tmsat_reg3;
    } t_tmsat_register;

    typedef volatile struct
    {
        /*type		name							description						offset*/
        /*General Purpose Registers*/
        t_uint32                ba_enable;                      /*To enable and disable SBAG		    12h'000 */
        t_uint32                ba_mode;                        /*To define working mode of SBAG	    12h'004 */
        t_uint32                ba_msg_port_pace;               /*To define pace for output data		12h'008 */
        t_uint32                ba_status;                      /*Gives the fifo status					12h'00C */
        t_uint32                padding0[4];                    /*Reserved upto 						12h'01F */

        /*Watch Point Satellite control Registers */
        t_uint32                wpsat_enable;                   /*To enable the WP Sat					12h'020	*/
        t_uint32                wpsat_prog_mode;                /*To Enter Prog Mode for each WP Sat	12h'024 */
        t_uint32                wpsat_prog_status;              /*To know if WPSAT Prog has succeeded	12h'028 */
        t_uint32                wpsat_prog_clear;               /*To exit WPSAT programming mode		12h'02C	*/
        t_uint32                padding1[4];                    /*Reserved upto							12h'03F	*/

        /*Traffic Management Satellite control Registers */
        t_uint32                tmsat_enable;                   /*To enable the TM Sat					12h'040 */
        t_uint32                tmsat_prog_mode;                /*To enter Prog mode for each TM Sat	12h'044 */
        t_uint32                tmsat_prog_status;              /*To know if TM Sat Prog is succeeded	12h'048 */
        t_uint32                tmsat_prog_clear;               /*To exit TM Sat programming mode		12h'04C */
        t_uint32                tmsat_read_status;              /*To know if TM Sat Metrices are valid	12h'050 */
        t_uint32                padding2[3];                    /*Reserved upto 						12h'05E	*/

        /*Interrupt Registers*/
        t_uint32                wpsat_its;                      /*To know which WP Sat flag is high		12h'060 */
        t_uint32                wpsat_its_clr;                  /*To clear WP Sat [i]					12h'064	*/
        t_uint32                wpsat_its_set;                  /*To Set the WP Sat interrupts			12h'068 */
        t_uint32                wpsat_itm;                      /*To know which Int Mask is high		12h'06C	*/
        t_uint32                wpsat_itm_clr;                  /*To clear wpsat_itm[i]					12h'070 */
        t_uint32                wpsat_itm_set;                  /*To set wpsat_itm[i]					12h'074	*/
        t_uint32                pi_its;                         /*To know which PI flag is high			12h'078 */
        t_uint32                pi_its_clr;                     /*To clear pi its[i]					12h'07c	*/
        t_uint32                pi_its_set;                     /*To set the pi its[i]					12h'080 */
        t_uint32                pi_itm;                         /*To know which PI int mask is high		12h'084	*/
        t_uint32                pi_itm_clr;                     /*To clear pi_itm[i]					12h'088	*/
        t_uint32                pi_itm_set;                     /*To set the pi_itm[i]					12h'08C	*/
        t_uint32                tmsat_its;                      /*To know which TM Sat flag is high		12h'090 */
        t_uint32                tmsat_its_clr;                  /*To clear TM Sat [i]					12h'094	*/
        t_uint32                tmsat_its_set;                  /*To Set the TM Sat interrupts			12h'098 */
        t_uint32                tmsat_itm;                      /*To know which Int Mask is high		12h'09C	*/
        t_uint32                tmsat_itm_clr;                  /*To clear tmsat_itm[i]					12h'0A0 */
        t_uint32                tmsat_itm_set;                  /*To set tmsat_itm[i]					12h'0A4	*/
        t_uint32                pi_error;                       /*Error on Pi Request Lost				12h'0A8 */
        t_uint32                pi_error_clr;                   /*To clear Pi error register			12h'0AC */
        t_uint32                tmsat0_7_error;                 /*Error in tmsat metric computation		12h'0B0 */
        t_uint32                tmsat8_15_error;                /*Error in tmsat metric computation		12h'0B4	*/
        t_uint32                tm_lost_error;                  /*Indicates that Tmsat result is lost	12h'0B8 */
        t_uint32                tm_error_clr;                   /*Clears all TM Error					12h'0BC */
        t_uint32                sbag_dvf0;                      /*Permit to enable design for verify	12h'0C0 */
        t_uint32                time_absolute_lsb;              /*Lsb counter Value						12h'0C4 */
        t_uint32                time_absolute_msb;              /*Msb counter Value						12h'0C8 */
        t_uint32                padding3[(0x200 - 0xCC) >> 2];  /*Reserved till 					12h'200	*/

        /*Watch Points*/
        t_watchpoint_register   watchpoint[16];                 /* Registers till offset of 			12h'33C	*/
        t_uint32                padding4[(0x400 - 0x340) >> 2]; /* Reserved Till 	        12h'400 */
        t_uint32                msg_ref_pace;                   /* Message Ref Pace					12h'400 */
        t_uint32                msg_fpf_data;                   /* 32 Bit Data to put in FPF message  	12h'404	*/
        t_uint32                msg_fpf_trace_control;          /* Select fields to include in FPF		12h'408 */
        t_uint32                msg_pi_trace_control;           /* Select fields to include in PI		12h'40C */
        t_uint32                msg_ref_trace_control;          /* To enable/disable Ref Gen Messages 	12h'410 */
        t_uint32                msg_err_trace_control;          /* Select fields to include in Err		12h'414	*/
        t_uint32                msg_wp_enable;                  /* To enable WP Message				12h'418 */
        t_uint32                msg_wp_trace_control;           /* Select fields to include in WP 		12h'41C */
        t_uint32                msg_tm_enable;                  /* To enable TM Message				12h'420 */
        t_uint32                msg_tm_trace_control_l;         /* Select fields to include in TM 0-7	12h'424 */
        t_uint32                msg_tm_trace_control_m;         /* Select fields to include in TM 8-15	12h'428 */
        t_uint32                padding5[(0x600 - 0x42C) >> 2]; /*Reserved uptill				12h'600 */
        t_tmsat_register        tmsat_register[16];             /* Register Offset till				12h'6BC	*/
        t_uint32                padding6[(0x800 - 0x6C0) >> 2]; /*Reserved uptil 				12h'800 */
        t_uint32                pi_trigger_raise;               /* Enable rising edge detection in PI	12h'800 */
        t_uint32                pi_trigger_fall;                /* Falling Edge detection in PI		12h'804 */
    } t_sbag_register;

#ifdef __cplusplus
}   /* allow C++ to use these headers*/
#endif /* __cplusplus*/
#endif /* _SBAG_IRQP_H_*/

/*End of sbag_irqp.h*/
