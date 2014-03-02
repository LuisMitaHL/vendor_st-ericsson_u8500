/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public header file for ST-Bus Analyser SBAG
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SBAG_H_
#define _SBAG_H_

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "debug.h"
#include "sbag_irq.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*--------------------------------------------------------------------------*
 * Constants and new types													*
 * Values defined with an enum item have a real hardware signification.
 * They represent a real hardware register value. Be carefull when changing !
 * Except for the t_msp_error type.
 *--------------------------------------------------------------------------*/
    /* Errors related values
-----------------------*/
    typedef enum
    {
        SBAG_TRANSMIT_FIFO_TIMEOUT          = HCL_MAX_ERROR_VALUE - 3,
        SBAG_NON_AUTHORIZED_MODE            = HCL_MAX_ERROR_VALUE - 2,
        SBAG_NO_ACTIVE_IT_ERROR             = HCL_MAX_ERROR_VALUE - 1,
        SBAG_TRANSMISSION_ON_GOING          = HCL_MAX_ERROR_VALUE,
        SBAG_INTERNAL_ERROR                 = HCL_INTERNAL_ERROR,                   /*(-8)*/
        SBAG_NOT_CONFIGURED                 = HCL_NOT_CONFIGURED,                   /*(-7)*/
        SBAG_REQUEST_PENDING                = HCL_REQUEST_PENDING,                  /*(-6)*/
        SBAG_REQUEST_NOT_APPLICABLE         = HCL_REQUEST_NOT_APPLICABLE,           /*(-5)*/
        SBAG_INVALID_PARAMETER              = HCL_INVALID_PARAMETER,                /*(-4)*/
        SBAG_UNSUPPORTED_FEATURE            = HCL_UNSUPPORTED_FEATURE,              /*(-3)*/
        SBAG_UNSUPPORTED_HW                 = HCL_UNSUPPORTED_HW,                   /*(-2)*/
        SBAG_ERROR                          = HCL_ERROR,                            /*(-1)*/
        SBAG_OK                             = HCL_OK,                               /*( 0)*/
        SBAG_INTERNAL_EVENT                 = HCL_INTERNAL_EVENT,                   /*( 1)*/
        SBAG_REMAINING_PENDING_EVENTS       = HCL_REMAINING_PENDING_EVENTS,         /*( 2)*/
        SBAG_REMAINING_FILTER_PENDING_EVENTS= HCL_REMAINING_FILTER_PENDING_EVENTS,  /*( 3)*/
        SBAG_NO_MORE_PENDING_EVENT          = HCL_NO_MORE_PENDING_EVENT,            /*( 4)*/
        SBAG_NO_MORE_FILTER_PENDING_EVENT   = HCL_NO_MORE_FILTER_PENDING_EVENT,     /*( 5)*/
        SBAG_NO_PENDING_EVENT_ERROR         = HCL_NO_PENDING_EVENT_ERROR,           /*( 7)*/

        SBAG_TMSAT_METRIC1_ERROR            = HCL_MAX_ERROR_VALUE - 300,
        SBAG_TMSAT_METRIC2_ERROR            = HCL_MAX_ERROR_VALUE - 301,
        SBAG_TMSAT_METRIC3_ERROR            = HCL_MAX_ERROR_VALUE - 302,
        SBAG_TMSAT_ERROR_ACT_MCT            = HCL_MAX_ERROR_VALUE - 303,
        SBAG_TMSAT_LOST_ERROR               = HCL_MAX_ERROR_VALUE - 304,
        SBAG_DATA_UNAVILABLE                = HCL_MAX_ERROR_VALUE - 305
    } t_sbag_error;

    /*-------SBAG Interrupt Management ----------*/
    typedef enum
    {
        SBAG_IRQ_STATE_NEW                  = 0,
        SBAG_IRQ_STATE_OLD                  = 1
    } t_sbag_irq_state;

    typedef struct
    {
        t_sbag_irq_state    interrupt_state;
        t_sbag_irq_src      initial_irq;    /* To be done */
        t_sbag_irq_src      pending_irq;
    } t_sbag_irq_status;

    /*----------SBAG Operating Mode---------*/
    typedef enum
    {
        SBAG_INTERRUPT_MODE                 = 0,
        SBAG_TRACE_MODE                     = 1,
        SBAG_LINK_MODE                      = 2
    } t_sbag_mode;

    /*--------- SBAG WPSAT Watchpoint Satellite Ids---------*/
    typedef enum
    {
        SBAG_WPSAT_ID0,
        SBAG_WPSAT_ID1,
        SBAG_WPSAT_ID2,
        SBAG_WPSAT_ID3,
        SBAG_WPSAT_ID4,
        SBAG_WPSAT_ID5,
        SBAG_WPSAT_ID6,
        SBAG_WPSAT_ID7,
        SBAG_WPSAT_ID8,
        SBAG_WPSAT_ID9,
        SBAG_WPSAT_ID10,
        SBAG_WPSAT_ID11,
        SBAG_WPSAT_ID12,
        SBAG_WPSAT_ID13,
        SBAG_WPSAT_ID14,
        SBAG_WPSAT_ID15
    } t_sbag_wpsat_id;

    /*----------SBAG TMSAT Traffic Management Satellite IDs---------*/
    typedef enum
    {
        SBAG_TMSAT_ID0,
        SBAG_TMSAT_ID1,
        SBAG_TMSAT_ID2,
        SBAG_TMSAT_ID3,
        SBAG_TMSAT_ID4,
        SBAG_TMSAT_ID5,
        SBAG_TMSAT_ID6,
        SBAG_TMSAT_ID7,
        SBAG_TMSAT_ID8,
        SBAG_TMSAT_ID9,
        SBAG_TMSAT_ID10,
        SBAG_TMSAT_ID11,
        SBAG_TMSAT_ID12,
        SBAG_TMSAT_ID13,
        SBAG_TMSAT_ID14,
        SBAG_TMSAT_ID15
    } t_sbag_tmsat_id;

    /*---------SBAG PI ids---------*/
    typedef enum
    {
        SBAG_PI_ID0,
        SBAG_PI_ID1,
        SBAG_PI_ID2,
        SBAG_PI_ID3,
        SBAG_PI_ID4,
        SBAG_PI_ID5,
        SBAG_PI_ID6,
        SBAG_PI_ID7,
        SBAG_PI_ID8,
        SBAG_PI_ID9,
        SBAG_PI_ID10,
        SBAG_PI_ID11,
        SBAG_PI_ID12,
        SBAG_PI_ID13,
        SBAG_PI_ID14,
        SBAG_PI_ID15
    } t_sbag_pi_id;

    /*----------Data Type to include or exclude Absoulte Time Value --------*/
    typedef enum
    {
        SBAG_EXCLUDE_TIME_ABSOLUTE          = 0,
        SBAG_INCLUDE_TIME_ABSOLUTE          = 1
    } t_sbag_wpsat_time_msg;

    /*-------- Data Type to include or exclude the Data Message-------*/
    typedef enum
    {
        SBAG_EXCLUDE_DATA_MSG               = 0,
        SBAG_INCLUDE_DATA_MSG               = 1
    } t_sbag_wpsat_data_msg;

    /*--------Data Type to Include or Exclude the Cell Message---------*/
    typedef enum
    {
        SBAG_EXCLUDE_CELL_MSG               = 0,
        SBAG_INCLUDE_CELL_MSG               = 1
    } t_sbag_wpsat_cell_msg;

    /*--------- Structure to Capture the Msg Configuration --------*/
    typedef struct
    {
        t_sbag_wpsat_time_msg   sbag_wpsat_time_msg;
        t_sbag_wpsat_data_msg   sbag_wpsat_data_msg;
        t_sbag_wpsat_cell_msg   sbag_wpsat_cell1_msg;
        t_sbag_wpsat_cell_msg   sbag_wpsat_cell2_msg;
    } t_sbag_wpsat_msg_config;

    /*----------Data type to include or exclude the Error Messages--------*/
    typedef enum
    {
        SBAG_EXCLUDE_LERR_MSG               = 0,
        SBAG_INCLUDE_LERR_MSG               = 1
    } t_sbag_tmsat_lerr_msg;

    /*---------Data Type to Include or exclude the Metric 3 Computation---------*/
    typedef enum
    {
        SBAG_EXCLUDE_LMETRIC3_MSG           = 0,
        SBAG_INCLUDE_LMETRIC3_MSG           = 1
    } t_sbag_tmsat_lmetric3_msg;

    /*---------Data Type to Include or exclude the Metric 2 Computation---------*/
    typedef enum
    {
        SBAG_EXCLUDE_LMETRIC2_MSG           = 0,
        SBAG_INCLUDE_LMETRIC2_MSG           = 1
    } t_sbag_tmsat_lmetric2_msg;

    /*--------Date type to Include or exclude the metric 1 Computation----------*/
    typedef enum
    {
        SBAG_EXCLUDE_LMETRIC1_MSG           = 0,
        SBAG_INCLUDE_LMETRIC1_MSG           = 1
    } t_sbag_tmsat_lmetric1_msg;

    /*---------Structure to configure the TMSAT Messages ---------*/
    typedef struct
    {
        t_sbag_tmsat_lerr_msg       tmsat_lerr_msg;
        t_sbag_tmsat_lmetric3_msg   tmsat_lmetric3_msg;
        t_sbag_tmsat_lmetric2_msg   tmsat_lmetric2_msg;
        t_sbag_tmsat_lmetric1_msg   tmsat_lmetric1_msg;
    } t_sbag_tmsati_message;

    /*---------TMSAT Message Configuration Structure---------*/
    typedef struct
    {
        t_sbag_tmsati_message   sbag_tmsati_message[16];
    } t_sbag_tmsat_msg_config;

    /* -----------Referece Message  Pace---------*/
    typedef t_uint32    t_sbag_msg_ref_pace;

    /*------------Data Type to include or exclude fpf Data Message---------*/
    typedef enum
    {
        SBAG_EXCLUDE_FPF_DATA_MSG           = 0,
        SBAG_INCLUDE_FPF_DATA_MSG           = 1
    } t_sbag_fpf_data_msg;

    /*---------- Data Type to Include or Exclude the Time Message----------*/
    typedef enum
    {
        SBAG_EXCLUDE_TIME_MSG               = 0,
        SBAG_INCLUDE_TIME_MSG               = 1
    } t_sbag_fpf_time_msg;

    /*---------- Structure to Configure the Message of FPF-----------*/
    typedef struct
    {
        t_sbag_fpf_data_msg sbag_fpf_data_msg;
        t_sbag_fpf_time_msg sbag_fpf_time_msg;
    } t_sbag_fpf_msg_config;

    /*-----------Data Type to Include or Exclude the Error Flag---------*/
    typedef enum
    {
        SBAG_EXCLUDE_ERROR_FLAG             = 0,
        SBAG_INCLUDE_ERROR_FLAG             = 1
    } t_sbag_err_flag_msg;

    /*----------Structure to Configure the Error Message Structure---------*/
    typedef struct
    {
        t_sbag_err_flag_msg sbag_err_flag0_msg;
        t_sbag_err_flag_msg sbag_err_flag1_msg;
        t_sbag_err_flag_msg sbag_err_flag2_msg;
        t_sbag_err_flag_msg sbag_err_flag3_msg;
    } t_sbag_err_msg_config;

    /*Data Types  for programming or writing the WPSAT*/
    /*---------Data Type to Specify whether the address provided is in the address range or out of range-------*/
    typedef enum
    {
        SBAG_ADD_IN                         = 0,
        SBAG_ADD_OUT                        = 1
    } t_sbag_wpsat_add_out;

    /*---------Data Type to Perform Inverted ROPC comparision--------*/
    typedef enum
    {
        SBAG_ROPC_COMP                      = 0,
        SBAG_INV_ROPC_COMP                  = 1
    } t_sbag_wpsat_ropc_out;

    /*-------Data Type to Perform Inverted Src Comparision ----------*/
    typedef enum
    {
        SBAG_SRC_COMP                       = 0,
        SBAG_INV_SRC_COMP                   = 1
    } t_sbag_wpsat_src_out;

    /*---------Data Type To Perform the Inverted OPC Comparision---------*/
    typedef enum
    {
        SBAG_OPC_COMP                       = 0,
        SBAG_INV_OPC_COMP                   = 1
    } t_sbag_wpsat_opc_out;

    /*-----------Data Type to Specify Request Enable or Disable ---------*/
    typedef enum
    {
        SBAG_REQ_DISABLE                    = 0,
        SBAG_REQ_ENABLE                     = 1
    } t_sbag_wpsat_req_enb;

    /*---------Data Type to Specify Response Enable or Disable----------*/
    typedef enum
    {
        SBAG_RES_DISABLE                    = 0,
        SBAG_RES_ENABLE                     = 1
    } t_sbag_wpsat_res_enb;

    /*----------Structure used for the purpose of Programming a WPSAT---------*/
    typedef struct
    {
        t_sbag_wpsat_add_out    sbag_wpsat_add_out;     /*Address Range is In or Out*/
        t_uint32                sbag_wpsat_start_add;   /*Start Address*/
        t_uint32                sbag_wpsat_end_add;     /*End Address*/
        t_uint8                 sbag_ref_op_code;       /*Reference Opcode*/
        t_uint16                sbag_ref_source;        /*Reference Source - Use 10 bits only*/
        t_uint8                 sbag_ref_ropcode;       /*Reference Ropcode*/
        t_uint8                 sbag_ref_op_code_mask;  /*Reference Opcode Mask*/
        t_uint16                sbag_ref_source_mask;   /*Reference Source - Use 10 bits only Mask*/
        t_uint8                 sbag_ref_ropcode_mask;  /*Reference Ropcode Mask*/
        t_sbag_wpsat_ropc_out   sbag_wpsat_ropc_out;    /*Inverse Operation Setting for Ropc*/
        t_sbag_wpsat_src_out    sbag_wpsat_src_out;     /*Inverse Operation Setting for Src*/
        t_sbag_wpsat_opc_out    sbag_wpsat_opc_out;     /*Inverse Operation Setting for opc*/
        t_sbag_wpsat_req_enb    sbag_wpsat_req_enb;     /*Request Enable*/
        t_sbag_wpsat_res_enb    sbag_wpsat_res_enb;     /*Response Enable*/
    } t_sbag_wpsat_write_data;

    /*-------------Data for Programming the TMSATs-----------*/
    /*----------- Data Type for Enabling or Disabling the Opcode Filter---------*/
    typedef enum
    {
        SBAG_DISABLE_OPCODE_FILTER          = 0,
        SBAG_ENABLE_OPCODE_FILTER           = 1
    } t_sbag_tmsat_opc_en;

    /*----------- Data Type for Enabling or Disabling the Src Filter----------*/
    typedef enum
    {
        SBAG_DISABLE_SRC_FILTER             = 0,
        SBAG_ENABLE_SRC_FILTER              = 1
    } t_sbag_tmsat_src_en;

    /*------------ Structure for Programming the TMSATs-----------*/
    typedef struct
    {
        t_uint16            sbag_tmsat_src_value;       /*The Source Value- use only 10 bits */
        t_uint8             sbag_tmsat_opc_value;       /*The Opcode Value*/
        t_uint16            sbag_tmsat_src_mask;        /*Mask for the source*/
        t_uint8             sbag_tmsat_op_code_mask;    /*Mask for the op-code*/

        /*The following Metrices are only applicable if BOT is enabled in RTL*/
        t_sbag_tmsat_opc_en sbag_tmsat_opc_en;          /*To Enable Opcode filtering*/
        t_sbag_tmsat_src_en sbag_tmsat_src_en;          /*To Enable Source Filtering*/
    } t_sbag_tmsat_write_data;

    /*----------Data Types for the Purpose of Programming the PI---------*/
    /*--------- Data Type to include or exclude the PI Time message ---------*/
    typedef enum
    {
        SBAG_EXCLUDE_PI_TIME                = 0,
        SBAG_INCLUDE_PI_TIME                = 1
    } t_sbag_pi_time_msg;

    /*--------Data Type to set the Triggering mechanism of PI----------*/
    typedef enum
    {
        SBAG_PI_TRIGGER_FALLING             = 0,
        SBAG_PI_TRIGGER_RAISING             = 1
    } t_sbag_pi_trigger;

    /*--------- Structure to configure the PI message Structure---------*/
    typedef struct
    {
        t_sbag_pi_time_msg  sbag_pi_time_msg;
        t_uint16            sbag_pi_incdata;
        t_sbag_pi_trigger   sbag_pi_trigger;
    } t_sbag_pi_msg_config;

    /*---------------------------------------------------------------------------------------------------*/
    /*					FUNCTIONS DECLARATIONS															 */
    /*---------------------------------------------------------------------------------------------------*/
    PUBLIC t_sbag_error     SBAG_Init(IN t_logical_address sbag_base_address);
    PUBLIC t_sbag_error     SBAG_Enable(void);
    PUBLIC t_sbag_error     SBAG_Disable(void);
    PUBLIC t_sbag_error     SBAG_SetOperatingMode(IN t_sbag_mode sbag_operating_mode);
    PUBLIC t_sbag_error     SBAG_EnableWPSAT(IN t_sbag_wpsat_id sbag_wpsat_id);
    PUBLIC t_sbag_error     SBAG_DisableWPSAT(IN t_sbag_wpsat_id sbag_wpsat_id);
    PUBLIC t_sbag_error     SBAG_WPSATConfigMsgSetting
                            (
                                IN t_sbag_wpsat_id          sbag_wpsat_id,
                                IN t_sbag_wpsat_msg_config  *p_sbag_wpsat_msg_config
                            );
    PUBLIC t_sbag_error     SBAG_EnableTMSAT(IN t_sbag_tmsat_id sbag_tmsat_id);
    PUBLIC t_sbag_error     SBAG_DisableTMSAT(IN t_sbag_tmsat_id sbag_tmsat_id);
    PUBLIC t_sbag_error     SBAG_TMSATConfigMsgSetting
                            (
                                t_sbag_tmsat_id         sbag_tmsat_id,
                                t_sbag_tmsat_msg_config *p_sbag_tmsat_msg_config
                            );
    PUBLIC t_sbag_error     SBAG_SetDbgLevel(IN t_dbg_level dbg_level);
    PUBLIC t_sbag_error     SBAG_GetDbgLevel(OUT t_dbg_level *p_dbg_level);
    PUBLIC t_sbag_error     SBAG_GetVersion(OUT t_version *p_version);
    PUBLIC t_sbag_error     SBAG_SetRefMsgPace(IN t_sbag_msg_ref_pace sbag_msg_ref_pace);
    PUBLIC t_sbag_error     SBAG_EnableFPFMessage(void);
    PUBLIC t_sbag_error     SBAG_DisableFPFMessage(void);
    PUBLIC t_sbag_error     SBAG_FPFConfigMsgSetting(IN t_sbag_fpf_msg_config *p_sbag_fpf_msg_config);
    PUBLIC t_sbag_error     SBAG_EnablePIMessage(void);
    PUBLIC t_sbag_error     SBAG_DisablePIMessage(void);
    PUBLIC t_sbag_error     SBAG_EnableRefMessage(void);
    PUBLIC t_sbag_error     SBAG_DisableRefMessage(void);
    PUBLIC t_sbag_error     SBAG_EnableErrMessage(void);
    PUBLIC t_sbag_error     SBAG_DisableErrMessage(void);
    PUBLIC t_sbag_error     SBAG_ERRConfigMsgSetting(IN t_sbag_err_msg_config *p_sbag_err_msg_config);
    PUBLIC t_sbag_error     SBAG_WriteWPSAT
                            (
                                IN t_sbag_wpsat_id          sbag_wpsat_id,
                                IN t_sbag_wpsat_write_data  *p_sbag_wpsat_write_data
                            );
    PUBLIC t_sbag_error     SBAG_WriteTMSAT
                            (
                                IN t_sbag_tmsat_id          sbag_tmsat_id,
                                IN t_sbag_tmsat_write_data  *p_sbag_tmsat_write_data
                            );
    PUBLIC t_sbag_error     SBAG_ReadAbsouluteTime(void);
    PUBLIC t_sbag_error     SBAG_ReadWPSAT(t_sbag_wpsat_id sbag_wpsat_id);
    PUBLIC t_sbag_error     SBAG_ReadTMSAT(t_sbag_tmsat_id sbag_tmsat_id);
    PUBLIC t_sbag_error     SBAG_ProcessIRQSrc(IN t_sbag_irq_status *p_status);
    PUBLIC void             SBAG_GetIRQSrcStatus(IN t_sbag_irq_src sbag_irq_src, OUT t_sbag_irq_status *p_status);
    PUBLIC t_sbag_error     SBAG_PIConfigMsgSetting
                            (
                                IN t_sbag_pi_id         sbag_pi_id,
                                IN t_sbag_pi_msg_config *p_sbag_pi_msg_config
                            );
    PUBLIC t_sbag_error     SBAG_WriteFPFData(IN t_uint32 sbag_data);
    PUBLIC t_sbag_error     SBAG_ReadFPFData(INOUT t_uint32 *sbag_data);
    PUBLIC t_sbag_error     SBAG_WPSATEnableMessage(IN t_sbag_wpsat_id sbag_wpsat_id);
    PUBLIC t_sbag_error     SBAG_WPSATDisableMessage(IN t_sbag_wpsat_id sbag_wpsat_id);
    PUBLIC t_sbag_error     SBAG_WPSATEnterPgmMode(IN t_sbag_wpsat_id sbag_wpsat_id);
    PUBLIC t_sbag_error     SBAG_WPSATClearPgmMode(void);
    PUBLIC t_sbag_error     SBAG_TMSATClearPgmMode(void);
    PUBLIC t_sbag_error     SBAG_TMSATEnterPgmMode(IN t_sbag_tmsat_id sbag_tmsat_id);
    PUBLIC t_sbag_device    SBAG_GetSBAGCurrentDevice(void);
    PUBLIC t_sbag_error     SBAG_TMSATEnableMessage(t_sbag_tmsat_id sbag_tmsat_id);
    PUBLIC t_sbag_error     SBAG_TMSATDisableMessage(IN t_sbag_tmsat_id sbag_tmsat_id);
#ifdef __cplusplus
}   /* allow C++ to use these headers*/
#endif /* __cplusplus*/
#endif /* _SBAG_H_*/

/*End of sbag.h*/
