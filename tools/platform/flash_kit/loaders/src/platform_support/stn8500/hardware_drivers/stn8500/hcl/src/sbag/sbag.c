/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the MSP
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "sbag_irqp.h"
#include "sbag_p.h"

/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Local Functions															*
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * debug stuff                                                              *
 *--------------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_SBAG
#define MY_DEBUG_ID             myDebugID_SBAG
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = MSP_HCL_DBG_ID;
#endif

/*--------------------------------------------------------------------------*
 * Global data for interrupt mode management                                *
 *--------------------------------------------------------------------------*/
PRIVATE t_sbag_system_context   g_sbag_system_context;

/*--------------------------------------------------------------------------*
 * Public functions                                                         *
 *--------------------------------------------------------------------------*/
extern void                     sbag_ReadRegister(t_uint32 register_name);

/****************************************************************************/
/* NAME			:  	 SBAG_Init	                               				*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine initializes SBAG register base address.	*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	sbag_base_address     :  SBAG registers base address    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_INVALID_PARAMETER:	This values is returned if base	*/
/*                             			    address passed is zero			*/
/*					SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/
/*			1)		Global variable p_sbag_register(register base address)	*/
/*					is being modified										*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_Init(IN t_logical_address sbag_base_address)
{
    if (NULL != sbag_base_address)
    {
        g_sbag_system_context.p_sbag_register = (t_sbag_register *)sbag_base_address;

        DBGEXIT0( SBAG_OK );
        return (SBAG_OK);
    }
    else
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }
}

/****************************************************************************/
/* NAME			:  	 SBAG_Enable			                            	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is used to Enable the SBAG.						*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	none												    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_Enable(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    HCL_SET_BITS( p_sbag_register->ba_enable, SBAG_ENABLE_DEVICE );

    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_Disable                            				*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is used to Disable the SBAG.						*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	none												    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_Disable(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    HCL_CLEAR_BITS( p_sbag_register->ba_enable, SBAG_ENABLE );

    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_SetOperatingMode  			                	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is used to set the operating mode of SBAG			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_mode: The Operating Mode for SBAG			    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the Mode is not	*/
/*											valid							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_SetOperatingMode(IN t_sbag_mode sbag_operating_mode)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    DBGENTER1( " (%d)", sbag_operating_mode );

    if (sbag_operating_mode > SBAG_MAX_OPERATING_MODE)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }
    else
    {
        HCL_WRITE_BITS( p_sbag_register->ba_mode, sbag_operating_mode, SBAG_OPERATING_MODE_MASK );
        g_sbag_system_context.sbag_mode = sbag_operating_mode;
    }

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_EnableWPSAT				                  		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is used to enable the particular WPSAT				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_wpsat_id: The Wp Satellite Id				    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the Mode is not	*/
/*											valid							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_EnableWPSAT(IN t_sbag_wpsat_id sbag_wpsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         wpsat_id_shift;
    t_uint32        wpsat_enable_mask = 0x1;    /*Enable Mask*/

    DBGENTER1( " (%d)", sbag_wpsat_id );

    if (sbag_wpsat_id > SBAG_WPSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    wpsat_id_shift = (t_uint8) sbag_wpsat_id;

    wpsat_enable_mask = wpsat_enable_mask << wpsat_id_shift;

    HCL_SET_BITS( p_sbag_register->wpsat_enable, wpsat_enable_mask );

    g_sbag_system_context.sbag_device = SBAG_WPSAT;

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_DisableWPSAT 				                 		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is used to disable the particular WPSAT			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_wpsat_id: The Wp Satellite Id				    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the Mode is not	*/
/*											valid							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_DisableWPSAT(IN t_sbag_wpsat_id sbag_wpsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         wpsat_id_shift;
    t_uint32        wpsat_disable_mask = 0x1;   /*Disable mask*/

    DBGENTER1( " (%d)", sbag_wpsat_id );

    if (sbag_wpsat_id > SBAG_WPSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    wpsat_id_shift = (t_uint8) sbag_wpsat_id;

    /* Shift the bit to be disabled*/
    wpsat_disable_mask = wpsat_disable_mask << wpsat_id_shift;

    HCL_CLEAR_BITS( p_sbag_register->wpsat_enable, wpsat_disable_mask );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_WPSATConfigMsgSetting            					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This configures the message Settings to get the trace	*/
/*																			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_wpsat_msg_config:Pointer to message settings	    */
/*				:	t_sbag_wpsat_id: The Wp Satellite Id					*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the parameters 	*/
/*											are wrong						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_WPSATConfigMsgSetting(IN t_sbag_wpsat_id sbag_wpsat_id, IN t_sbag_wpsat_msg_config *p_sbag_wpsat_msg_config)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        msg_wp_trace_control = 0x0;

    msg_wp_trace_control ^= msg_wp_trace_control;   /*Clear the register*/

    DBGENTER2( " (%lx %d)", p_sbag_wpsat_msg_config, sbag_wpsat_id );

    if ((NULL == p_sbag_wpsat_msg_config) || (sbag_wpsat_id > SBAG_WPSAT_ID15))
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    /*Set the Appropriate Messages you want*/
    SBAG_WRITE_FIELD
    (
        msg_wp_trace_control,
        p_sbag_wpsat_msg_config->sbag_wpsat_time_msg,
        SBAG_WPSAT_TRACE_TIME_MASK,
        SBAG_WPSAT_TRACE_TIME_SHIFT
    );
    SBAG_WRITE_FIELD
    (
        msg_wp_trace_control,
        p_sbag_wpsat_msg_config->sbag_wpsat_data_msg,
        SBAG_WPSAT_TRACE_DATA_MASK,
        SBAG_WPSAT_TRACE_DATA_SHIFT
    );
    SBAG_WRITE_FIELD
    (
        msg_wp_trace_control,
        p_sbag_wpsat_msg_config->sbag_wpsat_cell1_msg,
        SBAG_WPSAT_TRACE_CELL1_MASK,
        SBAG_WPSAT_TRACE_CELL1_SHIFT
    );
    SBAG_WRITE_FIELD
    (
        msg_wp_trace_control,
        p_sbag_wpsat_msg_config->sbag_wpsat_cell2_msg,
        SBAG_WPSAT_TRACE_CELL2_MASK,
        SBAG_WPSAT_TRACE_CELL2_SHIFT
    );

    /*Write the Value into the register*/
    HCL_WRITE_REG( p_sbag_register->msg_wp_trace_control, msg_wp_trace_control );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_WPSATEnableMessage			            		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This enables the WPSAT message generation				*/
/*																			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/*		IN		:	t_sbag_wpsat_id:The WP Satellite Id						*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the parameters 	*/
/*											are wrong						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_WPSATEnableMessage(IN t_sbag_wpsat_id sbag_wpsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         wpsat_id_shift = 0x0;
    t_uint32        msg_wp_enable = 0x1;

    DBGENTER1( " (%d)", sbag_wpsat_id );
    if ((sbag_wpsat_id > SBAG_WPSAT_ID15))
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    /*Enable WpSAT message Generation*/
    wpsat_id_shift = (t_uint8) sbag_wpsat_id;
    msg_wp_enable = msg_wp_enable << wpsat_id_shift;
    HCL_SET_BITS( p_sbag_register->msg_wp_enable, msg_wp_enable );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_WPSATDisableMessage			            		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This disables the WPSAT message generation				*/
/*																			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/*		IN		:	t_sbag_wpsat_id:The WP Satellite Id						*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the parameters 	*/
/*											are wrong						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_WPSATDisableMessage(IN t_sbag_wpsat_id sbag_wpsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         wpsat_id_shift = 0x0;
    t_uint32        msg_wp_disable = 0x1;

    DBGENTER1( " (%d)", sbag_wpsat_id );

    if ((sbag_wpsat_id > SBAG_WPSAT_ID15))
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    /*Enable WpSAT message Generation*/
    wpsat_id_shift = (t_uint8) sbag_wpsat_id;
    msg_wp_disable = msg_wp_disable << wpsat_id_shift;
    HCL_CLEAR_BITS( p_sbag_register->msg_wp_enable, msg_wp_disable );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_EnableTMSAT				                  		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is used to enable the particular TMSAT				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_tmsat_id: The Tm Satellite Id				    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the Mode is not	*/
/*											valid							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_EnableTMSAT(IN t_sbag_tmsat_id sbag_tmsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         tmsat_id_shift;
    t_uint32        tmsat_enable_mask = 0x1;

    DBGENTER1( " (%d)", sbag_tmsat_id );
    if (sbag_tmsat_id > SBAG_TMSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    tmsat_id_shift = (t_uint8) sbag_tmsat_id;

    /*Shift the bit to be enabled*/
    tmsat_enable_mask = tmsat_enable_mask << tmsat_id_shift;

    HCL_SET_BITS( p_sbag_register->tmsat_enable, tmsat_enable_mask );

    g_sbag_system_context.sbag_device = SBAG_TMSAT;

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_DisableTMSAT                  					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is used to disable the particular TMSAT			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_tmsat_id: The Tm Satellite Id				    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the Mode is not	*/
/*											valid							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_DisableTMSAT(IN t_sbag_tmsat_id sbag_tmsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         tmsat_id_shift;
    t_uint32        tmsat_disable_mask = 0x1;

    DBGENTER1( " (%d)", sbag_tmsat_id );
    if (sbag_tmsat_id > SBAG_TMSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    tmsat_id_shift = (t_uint8) sbag_tmsat_id;

    /*Shift the bit to be enabled*/
    tmsat_disable_mask = tmsat_disable_mask << tmsat_id_shift;

    HCL_CLEAR_BITS( p_sbag_register->tmsat_enable, tmsat_disable_mask );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_TMSATConfigMsgSetting			            		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This configures the message Settings to get the taces	*/
/*																			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_tmsat_msg_config:Pointer to message settings	    */
/*		IN		:	t_sbag_tmsat_id:The TM Satellite Id						*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the parameters 	*/
/*											are wrong						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_TMSATConfigMsgSetting(t_sbag_tmsat_id sbag_tmsat_id, t_sbag_tmsat_msg_config *p_sbag_tmsat_msg_config)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        msg_tm_trace_control = 0x0;
    t_uint8         tmsat_id, reg_shift, temp_id;

    msg_tm_trace_control ^= msg_tm_trace_control;   /*clear the register*/

    tmsat_id = (t_uint8) sbag_tmsat_id;

    DBGENTER2( " (%d), (%lx)", sbag_tmsat_id, p_sbag_tmsat_msg_config );

    if ((NULL == p_sbag_tmsat_msg_config) || sbag_tmsat_id > SBAG_TMSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    SBAG_WRITE_FIELD
    (
        msg_tm_trace_control,
        p_sbag_tmsat_msg_config->sbag_tmsati_message[tmsat_id].tmsat_lmetric1_msg,
        SBAG_TMSAT_TRACE_LMETRIC1_MASK,
        SBAG_TMSAT_TRACE_LMETRIC1_SHIFT
    );
    SBAG_WRITE_FIELD
    (
        msg_tm_trace_control,
        p_sbag_tmsat_msg_config->sbag_tmsati_message[tmsat_id].tmsat_lmetric2_msg,
        SBAG_TMSAT_TRACE_LMETRIC2_MASK,
        SBAG_TMSAT_TRACE_LMETRIC2_SHIFT
    );
    SBAG_WRITE_FIELD
    (
        msg_tm_trace_control,
        p_sbag_tmsat_msg_config->sbag_tmsati_message[tmsat_id].tmsat_lmetric3_msg,
        SBAG_TMSAT_TRACE_LMETRIC3_MASK,
        SBAG_TMSAT_TRACE_LMETRIC3_SHIFT
    );
    SBAG_WRITE_FIELD
    (
        msg_tm_trace_control,
        p_sbag_tmsat_msg_config->sbag_tmsati_message[tmsat_id].tmsat_lerr_msg,
        SBAG_TMSAT_TRACE_LERR_MASK,
        SBAG_TMSAT_TRACE_LERR_SHIFT
    );

    /*The Data has to entered into the register now*/
    /*Depending on the TMSAT id determine the corresponding shift required to write into register*/
    if (sbag_tmsat_id <= SBAG_TMSAT_ID7)
    {
        reg_shift = ((3 * tmsat_id) + tmsat_id);

        /*After getting the register shift write into the register*/
        p_sbag_register->msg_tm_trace_control_l |= (msg_tm_trace_control << reg_shift);
    }
    else
    {
        temp_id = tmsat_id & SBAG_GET_TMSAT_ID;
        reg_shift = ((3 * temp_id) + temp_id);

        /*After getting the register shift write into the register*/
        p_sbag_register->msg_tm_trace_control_m |= (msg_tm_trace_control << reg_shift);
    }

    DBGEXIT( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_TMSATEnableMessage			            		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This enables the TMSAT message generation				*/
/*																			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/*		IN		:	t_sbag_tmsat_id:The TM Satellite Id						*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the parameters 	*/
/*											are wrong						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_TMSATEnableMessage(IN t_sbag_tmsat_id sbag_tmsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        msg_tm_enable = 0x1;
    t_uint8         tmsat_id_shift;

    tmsat_id_shift = (t_uint8) sbag_tmsat_id;

    DBGENTER1( " (%d)", sbag_tmsat_id );

    if (sbag_tmsat_id > SBAG_TMSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    /*Enable TmSAT message Generation*/
    msg_tm_enable = msg_tm_enable << tmsat_id_shift;
    HCL_SET_BITS( p_sbag_register->msg_tm_enable, msg_tm_enable );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_TMSATDisableMessage			            		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This disables the TMSAT message generation				*/
/*																			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/*		IN		:	t_sbag_tmsat_id:The TM Satellite Id						*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the parameters 	*/
/*											are wrong						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_TMSATDisableMessage(IN t_sbag_tmsat_id sbag_tmsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        msg_tm_disable = 0x1;
    t_uint8         tmsat_id_shift;

    tmsat_id_shift = (t_uint8) sbag_tmsat_id;

    DBGENTER1( "(%d)", sbag_tmsat_id );

    if (sbag_tmsat_id > SBAG_TMSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    /*Enable TmSAT message Generation*/
    msg_tm_disable = msg_tm_disable << tmsat_id_shift;
    HCL_SET_BITS( p_sbag_register->msg_tm_enable, msg_tm_disable );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_SetDbgLevel	                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine sets the debug level for the SBAG HCL.       */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   t_dbg_level:dbg_level from DEBUG_LEVEL0 to DEBUG_LEVEL4 */
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_SetDbgLevel(IN t_dbg_level dbg_level)
{
    DBGENTER1( " (%d)", dbg_level );
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = dbg_level;
#endif
    DBGEXIT( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_GetDbgLevel                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine gets the debug level for the SBAG HCL.       */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   														*/
/* 		OUT		:   p_dbg_level from DEBUG_LEVEL0 to DEBUG_LEVEL4	        */
/*                                                                          */
/* RETURN		:   SBAG_OK : If there is no error                          */
/*				:	SBAG_INVALID_PARAMETER: If the debug level is NULL		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_GetDbgLevel(OUT t_dbg_level *p_dbg_level)
{
    DBGENTER1( " (%lx)", p_dbg_level );

    if (NULL == p_dbg_level)
    {
        DBGEXIT( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

#ifdef __DEBUG
    * p_dbg_level = MY_DEBUG_LEVEL_VAR_NAME;
#endif
    DBGEXIT( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_GetVersion	                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine gets the debug level for the SBAG HCL.       */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   														*/
/* 		OUT		:   p_dbg_level from DEBUG_LEVEL0 to DEBUG_LEVEL4	        */
/*                                                                          */
/* RETURN		:   SBAG_OK : If there is no error                          */
/*				:	SBAG_INVALID_PARAMETER: If the debug level is NULL		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_GetVersion(OUT t_version *p_version)
{
    DBGENTER1( " (%lx)", p_version );
    if (NULL != p_version)
    {
        p_version->minor = SBAG_HCL_MINOR_ID;
        p_version->major = SBAG_HCL_MAJOR_ID;
        p_version->version = SBAG_HCL_VERSION_ID;
        DBGEXIT0( SBAG_OK );
        return (SBAG_OK);
    }
    else
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }
}

/****************************************************************************/
/* NAME:    SBAG_SetRefMsgPace	                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine sets the reference Message Pace.			    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   t_sbag_msg_ref_pace:Reference Message Pace Value to set */
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_INVALID_PARAMETER: If the Value is Less than or    */
/*							equal to 150									*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_SetRefMsgPace(IN t_sbag_msg_ref_pace sbag_msg_ref_pace)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    DBGENTER1( " (%d)", sbag_msg_ref_pace );

    if (SBAG_MSG_REF_PACE_MIN >= (t_uint32) sbag_msg_ref_pace)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    sbag_msg_ref_pace = sbag_msg_ref_pace & SBAG_MSG_REF_PACE_MASK;

    /*Write the value into the register*/
    HCL_WRITE_REG( p_sbag_register->msg_ref_pace, sbag_msg_ref_pace );
	DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_EnableFPFMessage	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine enables the FPF Message Generation.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   none													*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_REQUEST_NOT_APPLICABLE: If the modes are other than*/
/*							trace mode										*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_EnableFPFMessage(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    if (SBAG_TRACE_MODE != g_sbag_system_context.sbag_mode)
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }

    HCL_SET_BITS( p_sbag_register->msg_fpf_trace_control, SBAG_FPF_MSG_MASK );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_DisableFPFMessage	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine disables the FPF Message Generation.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   none													*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_REQUEST_NOT_APPLICABLE: If the modes are other than*/
/*							trace mode										*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_DisableFPFMessage(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    if (SBAG_TRACE_MODE != g_sbag_system_context.sbag_mode)
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }

    HCL_CLEAR_BITS( p_sbag_register->msg_fpf_trace_control, SBAG_FPF_MSG_MASK );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_FPFConfigMsgSetting                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine configures the FPF Message Settings.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:  t_sbag_fpf_msg_config : FPF Message Configuration	    */
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_INVALID_PARAMETER: If the pointer is NULL		    */
/*				:	SBAG_REQUEST_NOT_APPLICABLE: If it is not Trace mode	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_FPFConfigMsgSetting(IN t_sbag_fpf_msg_config *p_sbag_fpf_msg_config)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        msg_fpf_trace_control = 0x0;

    msg_fpf_trace_control = msg_fpf_trace_control ^ msg_fpf_trace_control;  /*Clear the contents of the register*/

    DBGENTER1( " (%lx)", p_sbag_fpf_msg_config );

    if (NULL == p_sbag_fpf_msg_config)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    if (SBAG_TRACE_MODE == g_sbag_system_context.sbag_mode || SBAG_LINK_MODE == g_sbag_system_context.sbag_mode)
    {
        SBAG_WRITE_FIELD
        (
            msg_fpf_trace_control,
            p_sbag_fpf_msg_config->sbag_fpf_data_msg,
            SBAG_FPF_TRACE_DATA_MASK,
            SBAG_FPF_TRACE_DATA_SHIFT
        );
        SBAG_WRITE_FIELD
        (
            msg_fpf_trace_control,
            p_sbag_fpf_msg_config->sbag_fpf_time_msg,
            SBAG_FPF_TRACE_TIME_MASK,
            SBAG_FPF_TRACE_TIME_SHIFT
        );

        /*Write the Value into the register Map*/
        p_sbag_register->msg_fpf_trace_control |= msg_fpf_trace_control;
    }
    else
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_EnablePIMessage	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine enables the PI Message Generation.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   none													*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_REQUEST_NOT_APPLICABLE: If the modes are other than*/
/*							trace mode										*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_EnablePIMessage(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    if (SBAG_TRACE_MODE != g_sbag_system_context.sbag_mode)
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }

    HCL_SET_BITS( p_sbag_register->msg_pi_trace_control, SBAG_PI_MSG_MASK );

    g_sbag_system_context.sbag_device = SBAG_PI;

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_DisablePIMessage	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine enables the PI Message Generation.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   none													*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_REQUEST_NOT_APPLICABLE: If the modes are other than*/
/*							trace mode										*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_DisablePIMessage(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    if (SBAG_TRACE_MODE != g_sbag_system_context.sbag_mode)
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }

    HCL_CLEAR_BITS( p_sbag_register->msg_pi_trace_control, SBAG_PI_MSG_MASK );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_EnableRefMessage	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine enables the REF Message Generation.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   none													*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_EnableRefMessage(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    HCL_SET_BITS( p_sbag_register->msg_ref_trace_control, SBAG_REF_MSG_MASK );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_DisableRefMessage	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine disables the REF Message Generation.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   none													*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_REQUEST_NOT_APPLICABLE: If the modes are other than*/
/*							trace mode										*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_DisableRefMessage(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    if (SBAG_TRACE_MODE != g_sbag_system_context.sbag_mode)
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }

    HCL_CLEAR_BITS( p_sbag_register->msg_ref_trace_control, SBAG_REF_MSG_MASK );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_EnableErrMessage	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine enables the ERR Message Generation.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   none													*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_REQUEST_NOT_APPLICABLE: If the modes are other than*/
/*							trace mode										*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_EnableErrMessage(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    HCL_SET_BITS( p_sbag_register->msg_err_trace_control, SBAG_ERR_MSG_MASK );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_DisableErrMessage	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine disables the ERR Message Generation.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:   none													*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_REQUEST_NOT_APPLICABLE: If the modes are other than*/
/*							trace mode										*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_DisableErrMessage(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    if (SBAG_TRACE_MODE != g_sbag_system_context.sbag_mode)
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }

    HCL_CLEAR_BITS( p_sbag_register->msg_err_trace_control, SBAG_ERR_MSG_MASK );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_ERRConfigMsgSetting                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine configures the ERR Message Settings.		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:  t_sbag_err_msg_config : Pointer ERR Message Configuration*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_INVALID_PARAMETER: If pointer is NULL			    */
/*				:	SBAG_REQUEST_NOT_APPLICABLE: If mode is not Trace Mode	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_ERRConfigMsgSetting(IN t_sbag_err_msg_config *p_sbag_err_msg_config)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        msg_err_trace_control = 0x0;

    msg_err_trace_control = msg_err_trace_control ^ msg_err_trace_control;  /*Clear the contents of the register*/

    DBGENTER1( " (%lx)", p_sbag_err_msg_config );

    if (NULL == p_sbag_err_msg_config)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    if (SBAG_TRACE_MODE == g_sbag_system_context.sbag_mode)
    {
        SBAG_WRITE_FIELD
        (
            msg_err_trace_control,
            p_sbag_err_msg_config->sbag_err_flag0_msg,
            SBAG_ERR_TRACE_FLAGS0_MASK,
            SBAG_ERR_TRACE_FLAGS0_SHIFT
        );
        SBAG_WRITE_FIELD
        (
            msg_err_trace_control,
            p_sbag_err_msg_config->sbag_err_flag1_msg,
            SBAG_ERR_TRACE_FLAGS1_MASK,
            SBAG_ERR_TRACE_FLAGS1_SHIFT
        );
        SBAG_WRITE_FIELD
        (
            msg_err_trace_control,
            p_sbag_err_msg_config->sbag_err_flag2_msg,
            SBAG_ERR_TRACE_FLAGS2_MASK,
            SBAG_ERR_TRACE_FLAGS2_SHIFT
        );
        SBAG_WRITE_FIELD
        (
            msg_err_trace_control,
            p_sbag_err_msg_config->sbag_err_flag3_msg,
            SBAG_ERR_TRACE_FLAGS3_MASK,
            SBAG_ERR_TRACE_FLAGS3_SHIFT
        );

        /*Write the Value in the register*/
        p_sbag_register->msg_err_trace_control |= msg_err_trace_control;
    }
    else
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_WriteWPSAT			                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This Routine is Used to Write the Data into WPSAT for     */
/*				  performing the Matching									*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:  	t_sbag_wpsat_id: WPSAT to be used to perform comparision*/
/*				:	t_sbag_wpsat_write_data: Pointer to Write Data			*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_INVALID_PARAMETER: If pointer is NULL			    */
/*				:	SBAG_REQUEST_PENDING: If program status is one			*/
/*				:	SBAG_INTERNAL_ERROR: If ids do not match				*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_WriteWPSAT(IN t_sbag_wpsat_id sbag_wpsat_id, IN t_sbag_wpsat_write_data *p_sbag_wpsat_write_data)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         wpsat_id_shift, wpsat_id;
    t_sbag_error    sbag_error = SBAG_OK;
    t_uint32        test_wpsat_prog_status = 0x1;
    t_uint32        wpsat_reg1 = 0x0, wpsat_reg2 = 0x0, wpsat_reg3 = 0x0, wpsat_reg4 = 0x0, temp_val = 0x0;
    t_uint32        interrupt_src = 0x1;

    temp_val ^= temp_val;   //clear the registers
    wpsat_reg1 = wpsat_reg1 ^ wpsat_reg1;
    wpsat_reg2 = wpsat_reg2 ^ wpsat_reg2;
    wpsat_reg3 = wpsat_reg3 ^ wpsat_reg3;
    wpsat_reg4 = wpsat_reg4 ^ wpsat_reg4;

    DBGENTER2( "%d (%lx)", sbag_wpsat_id, p_sbag_wpsat_write_data );

    if ((NULL == p_sbag_wpsat_write_data) || (sbag_wpsat_id > SBAG_WPSAT_ID15))
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    wpsat_id = wpsat_id_shift = (t_uint8) sbag_wpsat_id;
    test_wpsat_prog_status = test_wpsat_prog_status << wpsat_id_shift;
    if (SBAG_TEST_BIT( p_sbag_register->wpsat_prog_status, test_wpsat_prog_status ))
    {
        /*This means that it has already been programmed or in the process of programming*/
        DBGEXIT0( SBAG_REQUEST_PENDING );
        return (SBAG_REQUEST_PENDING);
    }
    else
    {
        /*clear any previously available data of WPSAT*/
        sbag_error = sbag_WPSATClearDataAvailability( sbag_wpsat_id );
        if ((wpsat_id > 15) || SBAG_OK != sbag_error)
        {
            DBGEXIT0( SBAG_INTERNAL_ERROR );
            return (SBAG_INTERNAL_ERROR);
        }

        SBAG_WRITE_FIELD
        (
            wpsat_reg1,
            p_sbag_wpsat_write_data->sbag_wpsat_add_out,
            SBAG_WPSAT_ADD_OUT_MASK,
            SBAG_WPSAT_ADD_OUT_SHIFT
        );

        HCL_WRITE_BITS( wpsat_reg1, p_sbag_wpsat_write_data->sbag_wpsat_start_add, SBAG_WPSAT_START_ADD_MASK );

        HCL_WRITE_REG( p_sbag_register->watchpoint[wpsat_id].wpsat_reg1, wpsat_reg1 );

        /* Only 30 Bits are significant in end address */
        p_sbag_wpsat_write_data->sbag_wpsat_end_add = p_sbag_wpsat_write_data->sbag_wpsat_end_add & SBAG_WPSAT_END_ADD_MASK;

        HCL_WRITE_BITS( wpsat_reg2, p_sbag_wpsat_write_data->sbag_wpsat_end_add, SBAG_WPSAT_END_ADD_MASK );

        HCL_WRITE_REG( p_sbag_register->watchpoint[wpsat_id].wpsat_reg2, wpsat_reg2 );

        /*Write the Opcode , Src, and ropcode*/
        SBAG_WRITE_FIELD
        (
            wpsat_reg3,
            p_sbag_wpsat_write_data->sbag_ref_ropcode,
            SBAG_WPSAT_REF_OPC_MASK,
            SBAG_WPSAT_REF_OPC_SHIFT
        );

        temp_val = p_sbag_wpsat_write_data->sbag_ref_source;
        temp_val = temp_val << SBAG_WPSAT_REF_SRC_SHIFT;
        wpsat_reg3 |= temp_val;

        temp_val ^= temp_val;
        temp_val = p_sbag_wpsat_write_data->sbag_ref_ropcode;
        temp_val = temp_val << SBAG_WPSAT_REF_ROPC_SHIFT;
        wpsat_reg3 |= temp_val;

        HCL_WRITE_REG( p_sbag_register->watchpoint[wpsat_id].wpsat_reg3, wpsat_reg3 );

        /*Write the Mask for Opcode, Src, Ropcode*/
        SBAG_WRITE_FIELD
        (
            wpsat_reg4,
            p_sbag_wpsat_write_data->sbag_ref_op_code_mask,
            SBAG_WPSAT_OPC_MASK,
            SBAG_WPSAT_OPC_MASK_SHIFT
        );

        temp_val ^= temp_val;
        temp_val = p_sbag_wpsat_write_data->sbag_ref_source_mask;
        temp_val = temp_val << SBAG_WPSAT_SRC_MASK_SHIFT;
        wpsat_reg4 |= temp_val;

        temp_val ^= temp_val;
        temp_val = p_sbag_wpsat_write_data->sbag_ref_ropcode_mask;
        temp_val = temp_val << SBAG_WPSAT_ROPC_MASK_SHIFT;
        wpsat_reg4 |= temp_val;

        /*Write the Request and Response enable Bits*/
        temp_val ^= temp_val;
        temp_val = p_sbag_wpsat_write_data->sbag_wpsat_req_enb;
        temp_val = temp_val << SBAG_WPSAT_REQ_ENB_SHIFT;
        wpsat_reg4 |= temp_val;

        temp_val ^= temp_val;
        temp_val = p_sbag_wpsat_write_data->sbag_wpsat_res_enb;
        temp_val = temp_val << SBAG_WPSAT_RES_ENB_SHIFT;
        wpsat_reg4 |= temp_val;

        /*Negation Operation Bits Setting*/
        temp_val ^= temp_val;
        temp_val = p_sbag_wpsat_write_data->sbag_wpsat_opc_out;
        temp_val = temp_val << SBAG_WPSAT_OPC_OUT_SHIFT;
        wpsat_reg4 |= temp_val;

        temp_val ^= temp_val;
        temp_val = p_sbag_wpsat_write_data->sbag_wpsat_src_out;
        temp_val = temp_val << SBAG_WPSAT_SRC_OUT_SHIFT;
        wpsat_reg4 |= temp_val;

        temp_val ^= temp_val;
        temp_val = p_sbag_wpsat_write_data->sbag_wpsat_ropc_out;
        temp_val = temp_val << SBAG_WPSAT_ROPC_OUT_SHIFT;
        wpsat_reg4 |= temp_val;

        HCL_WRITE_REG( p_sbag_register->watchpoint[wpsat_id].wpsat_reg4, wpsat_reg4 );

        if (SBAG_INTERRUPT_MODE == g_sbag_system_context.sbag_mode || SBAG_LINK_MODE == g_sbag_system_context.sbag_mode)
        {
            interrupt_src = interrupt_src << wpsat_id_shift;    /*Enable only that IT bit which is required*/
            interrupt_src = interrupt_src | (t_uint32) SBAG_WPSAT_DEVICE;
            SBAG_EnableIRQSrc( (t_sbag_irq_src) interrupt_src );

            sbag_error = sbag_WPSATSetDataAvailability( sbag_wpsat_id );
        }
    }

    DBGEXIT0( sbag_error );
    return (sbag_error);
}

/****************************************************************************/
/* NAME:    SBAG_WriteTMSAT			                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This Routine is Used to Write the Data into TMSAT for     */
/*				  performing the Matching									*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:  	t_sbag_tmsat_id: TMSAT to be used to perform comparision*/
/*				:	t_sbag_tmsat_write_data: Pointer to Write Data			*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             	:	SBAG_INVALID_PARAMETER: If pointer is NULL			    */
/*				:	SBAG_REQUEST_PENDING: If program status is one			*/
/*				:	SBAG_INTERNAL_ERROR: If ids do not match				*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_WriteTMSAT(IN t_sbag_tmsat_id sbag_tmsat_id, IN t_sbag_tmsat_write_data *p_sbag_tmsat_write_data)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         tmsat_id_shift, tmsat_id;
    t_sbag_error    sbag_error = SBAG_OK;
    t_uint32        test_tmsat_prog_status = 0x1;
    t_uint32        tmsat_reg1 = 0x0, tmsat_reg2 = 0x0, temp_val = 0x0, interrupt_src = 0x1;

    /*clear the registers*/
    tmsat_reg1 = tmsat_reg1 ^ tmsat_reg1;
    tmsat_reg2 = tmsat_reg2 ^ tmsat_reg2;
    temp_val = temp_val ^ temp_val;

    DBGENTER2( "%d (%lx)", sbag_tmsat_id, p_sbag_tmsat_write_data );

    if ((NULL == p_sbag_tmsat_write_data) || (sbag_tmsat_id > SBAG_TMSAT_ID15))
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    tmsat_id = tmsat_id_shift = (t_uint8) sbag_tmsat_id;
    test_tmsat_prog_status = test_tmsat_prog_status << tmsat_id_shift;
    if (SBAG_TEST_BIT( p_sbag_register->tmsat_prog_status, test_tmsat_prog_status ))
    {
        /*This means that it has already been programmed or in the process of programming*/
        DBGEXIT0( SBAG_REQUEST_PENDING );
        return (SBAG_REQUEST_PENDING);
    }
    else
    {
        /*clear any previously available data of previous WPSAT*/
        sbag_error = sbag_TMSATClearDataAvailability( sbag_tmsat_id );

        if ((tmsat_id > 15) || (SBAG_OK != sbag_error))
        {
            DBGEXIT0( SBAG_INTERNAL_ERROR );
            return (SBAG_INTERNAL_ERROR);
        }

        p_sbag_tmsat_write_data->sbag_tmsat_src_value &= SBAG_TMSAT_SRC_VAL_MASK;
        SBAG_WRITE_FIELD
        (
            tmsat_reg1,
            p_sbag_tmsat_write_data->sbag_tmsat_src_value,
            SBAG_TMSAT_SRC_VAL_MASK,
            SBAG_TMSAT_SRC_VAL_SHIFT
        );

        temp_val = p_sbag_tmsat_write_data->sbag_tmsat_opc_value;
        temp_val = temp_val << SBAG_TMSAT_OPC_VAL_SHIFT;
        tmsat_reg1 |= temp_val;
        temp_val = temp_val ^ temp_val;

        p_sbag_tmsat_write_data->sbag_tmsat_src_mask &= SBAG_TMSAT_SRC_MASK;

        SBAG_WRITE_FIELD
        (
            tmsat_reg2,
            p_sbag_tmsat_write_data->sbag_tmsat_src_mask,
            SBAG_TMSAT_SRC_MASK,
            SBAG_TMSAT_SRC_SHIFT
        );

        temp_val = p_sbag_tmsat_write_data->sbag_tmsat_op_code_mask;
        temp_val = temp_val << SBAG_TMSAT_OPC_SHIFT;
        tmsat_reg2 |= temp_val;

        /*Enable the Opcode and Src Filtering*/
        temp_val ^= temp_val;
        temp_val = p_sbag_tmsat_write_data->sbag_tmsat_opc_en;
        temp_val = temp_val << SBAG_TMSAT_OPC_EN_SHIFT;
        tmsat_reg2 |= temp_val;

        temp_val ^= temp_val;
        temp_val = p_sbag_tmsat_write_data->sbag_tmsat_src_en;
        temp_val = temp_val << SBAG_TMSAT_SRC_EN_SHIFT;
        tmsat_reg2 |= temp_val;

        HCL_WRITE_REG( p_sbag_register->tmsat_register[tmsat_id].tmsat_reg2, tmsat_reg2 );
        HCL_WRITE_REG( p_sbag_register->tmsat_register[tmsat_id].tmsat_reg1, tmsat_reg1 );
        p_sbag_register->tmsat_prog_clear = 0x1;    /*Put back to Read Mode HW Bug work around*/

        /*Enable the Interrupts for the Interrupt and Link Mode*/
        if (SBAG_INTERRUPT_MODE == g_sbag_system_context.sbag_mode || SBAG_LINK_MODE == g_sbag_system_context.sbag_mode)
        {
            interrupt_src = interrupt_src << tmsat_id_shift;
            interrupt_src = interrupt_src | (t_uint32) SBAG_TMSAT_DEVICE;
            SBAG_EnableIRQSrc( (t_sbag_irq_src) interrupt_src );

            /*Enable the Data Availability Bit*/
            sbag_error = sbag_TMSATSetDataAvailability( sbag_tmsat_id );
        }
    }

    DBGEXIT0( sbag_error );
    return (sbag_error);
}

/****************************************************************************/
/* NAME:    SBAG_ReadAbsoluteTime	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine is used to get the absoulte Time				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:  	none													*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*             																*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_ReadAbsouluteTime(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    /*Store the Value of the time in the system context*/
    g_sbag_system_context.time_absolute_lsb = p_sbag_register->time_absolute_lsb;

    sbag_ReadRegister( g_sbag_system_context.time_absolute_lsb );

    g_sbag_system_context.time_absolute_msb = p_sbag_register->time_absolute_msb;

    sbag_ReadRegister( g_sbag_system_context.time_absolute_lsb );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_ReadWPSAT			                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine is used to read the data from the WPSAT		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:  	t_sbag_wpsat_id: The Wp Satellite Id					*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*				:	SBAG_INTERNAL_ERROR : In case clear doesn't happen		*/
/*				:	SBAG_INVALID_PARAMETER: Incase the Ids is invalid		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_ReadWPSAT(t_sbag_wpsat_id sbag_wpsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_sbag_error    sbag_error = SBAG_OK;
    t_uint32        wpsat_its = 0x1;
    t_uint8         wpsat_id, wpsat_id_shift, temp_var;

    DBGENTER1( "%d ", sbag_wpsat_id );
    if (sbag_wpsat_id > SBAG_WPSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    /*Before reading the data check if the data is available to read*/
    wpsat_id = wpsat_id_shift = (t_uint8) sbag_wpsat_id;
    wpsat_its = wpsat_its << wpsat_id_shift;

    /*Perform the reading of the registers*/
    g_sbag_system_context.wpsat_src = SBAG_READ_FIELD
        (
            p_sbag_register->watchpoint[wpsat_id].wpsat_reg1,
            SBAG_WPSAT_READ_SRC_MASK,
            SBAG_WPSAT_READ_SRC_SHIFT
        );
    g_sbag_system_context.wpsat_opc = SBAG_READ_FIELD
        (
            p_sbag_register->watchpoint[wpsat_id].wpsat_reg1,
            SBAG_WPSAT_READ_OPC_MASK,
            SBAG_WPSAT_READ_OPC_SHIFT
        );
    g_sbag_system_context.wpsat_be = SBAG_READ_FIELD
        (
            p_sbag_register->watchpoint[wpsat_id].wpsat_reg1,
            SBAG_WPSAT_READ_BE_MASK,
            SBAG_WPSAT_READ_BE_SHIFT
        );
    g_sbag_system_context.wpsat_tid = SBAG_READ_FIELD
        (
            p_sbag_register->watchpoint[wpsat_id].wpsat_reg1,
            SBAG_WPSAT_READ_TID_MASK,
            SBAG_WPSAT_READ_TID_SHIFT
        );

    /*Store the remaining bits of wpsat_tid*/
    temp_var = SBAG_READ_FIELD( p_sbag_register->watchpoint[wpsat_id].wpsat_reg2, SBAG_WPSAT_READ_TID_BITS, 0x0 );

    g_sbag_system_context.wpsat_tid |= temp_var;
    g_sbag_system_context.wpsat_lsb_address = SBAG_READ_FIELD
        (
            p_sbag_register->watchpoint[wpsat_id].wpsat_reg2,
            SBAG_WPSAT_READ_LSB_ADD_MASK,
            SBAG_WPSAT_READ_LSB_ADD_SHIFT
        );
    g_sbag_system_context.wpsat_data_lsb = HCL_READ_REG( p_sbag_register->watchpoint[wpsat_id].wpsat_reg3 );
    g_sbag_system_context.wpsat_data_msb = HCL_READ_REG( p_sbag_register->watchpoint[wpsat_id].wpsat_reg4 );
    g_sbag_system_context.wpsat_req_resp = SBAG_READ_FIELD
        (
            p_sbag_register->watchpoint[wpsat_id].wpsat_req_not_rsp,
            SBAG_WPSAT_READ_REQ_NOT_RESP,
            0x0
        );

    /*Add Print functions if needed*/
    sbag_ReadRegister( g_sbag_system_context.wpsat_src );
    sbag_ReadRegister( g_sbag_system_context.wpsat_opc );
    sbag_ReadRegister( g_sbag_system_context.wpsat_be );
    sbag_ReadRegister( g_sbag_system_context.wpsat_tid );
    sbag_ReadRegister( g_sbag_system_context.wpsat_lsb_address );
    sbag_ReadRegister( g_sbag_system_context.wpsat_data_lsb );
    sbag_ReadRegister( g_sbag_system_context.wpsat_data_msb );
    sbag_ReadRegister( g_sbag_system_context.wpsat_req_resp );

    /*After reading the data from the wpsat clear the data availability bit*/
    /*for next configuration*/
    //sbag_error = sbag_WPSATClearDataAvailability(sbag_wpsat_id);
    DBGEXIT0( sbag_error );
    return (sbag_error);
}

/****************************************************************************/
/* NAME:    SBAG_ReadTMSAT			                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine is used to read the TMSAT data				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN		:  	t_sbag_tmsat_id: The Tm Satellite Id					*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   SBAG_OK: If there is no error                           */
/*				:	SBAG_INTERNAL_ERROR : In case clear doesn't happen		*/
/*				:	SBAG_INVALID_PARAMETER: Incase the Ids is invalid		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_ReadTMSAT(t_sbag_tmsat_id sbag_tmsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_sbag_error    sbag_error = SBAG_OK;
    t_uint8         tmsat_id, tmsat_id_shift;
    t_uint32        tmsat_its = 0x1;

    DBGENTER1( "%d ", sbag_tmsat_id );

    if (sbag_tmsat_id > SBAG_TMSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    /*Before reading the data check if the data is available to read*/
    /*Also Check if the data is valid */
    tmsat_id = tmsat_id_shift = (t_uint8) sbag_tmsat_id;
    tmsat_its = tmsat_its << tmsat_id_shift;

    g_sbag_system_context.tmsat_metric1 = HCL_READ_REG( p_sbag_register->tmsat_register[tmsat_id].tmsat_reg1 );
    g_sbag_system_context.tmsat_metric2 = HCL_READ_REG( p_sbag_register->tmsat_register[tmsat_id].tmsat_reg2 );
    g_sbag_system_context.tmsat_metric3 = HCL_READ_REG( p_sbag_register->tmsat_register[tmsat_id].tmsat_reg3 );

    /*Add the external Print Function to get the value*/
    sbag_ReadRegister( g_sbag_system_context.tmsat_metric1 );
    sbag_ReadRegister( g_sbag_system_context.tmsat_metric2 );
    sbag_ReadRegister( g_sbag_system_context.tmsat_metric3 );

    /*After reading the data from the wpsat clear the data availability bit*/
    /*for next configuration*/
    DBGEXIT0( sbag_error );
    return (sbag_error);
}

/****************************************************************************/
/* NAME:    SBAG_ProcessIRQSrc		                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine is used to process all the given interrupt   */
/*                sources identified. It also updates the interrupt status  */
/*                for the pending interrupts.								*/
/* PARAMETERS	:                                                           */
/* 		IN		:  	t_sbag_irq_status: The Status of SBAG Source to be 		*/
/*					processed												*/
/* 		OUT		:   none                                                    */
/*                                                                          */
/* RETURN		:   														*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_ProcessIRQSrc(IN t_sbag_irq_status *p_status)
{
    t_sbag_error    sbag_error = SBAG_OK;

    DBGENTER1( "%lx ", p_status );

    if (NULL == p_status)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    if (SBAG_IRQ_STATE_OLD == p_status->interrupt_state || 0 == (p_status->pending_irq & SBAG_GET_INTERRUPTS))
    {
        sbag_error = SBAG_NO_PENDING_EVENT_ERROR;
        DBGEXIT0( sbag_error );
        return (sbag_error);
    }

    p_status->interrupt_state = SBAG_IRQ_STATE_OLD;

    /*Process the interrupts received*/
    sbag_error = sbag_ProcessIt( p_status->pending_irq );

    DBGEXIT0( sbag_error );
    return (sbag_error);
}

/****************************************************************************/
/* NAME:    SBAG_GetIRQSrcStatus	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: Updates the interrupt status Variables					*/
/* PARAMETERS	:                                                           */
/* 		IN		:  	t_sbag_irq_src: id of the interrupt for which structure	*/
/*					has to be updated 										*/
/* 		OUT		:   p_status : Status Structure                             */
/*                                                                          */
/* RETURN		:   void													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SBAG_GetIRQSrcStatus(IN t_sbag_irq_src sbag_irq_src, OUT t_sbag_irq_status *p_status)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_sbag_device   sbag_device = (t_sbag_device) (sbag_irq_src >> SBAG_SHIFT_BY_SIXTEEN);

    DBGENTER2( " %d %lx ", sbag_irq_src, p_status );

    if ((sbag_device > SBAG_TMSAT) || NULL == p_status)
    {
        return;
    }

    if ((t_uint32) SBAG_IRQ_SRC_ALL == (sbag_irq_src & SBAG_GET_INTERRUPTS))
    {
        switch (sbag_device)
        {
            case SBAG_WPSAT:
                p_status->pending_irq = p_sbag_register->wpsat_itm | (((t_uint32) sbag_device) << SBAG_SHIFT_BY_SIXTEEN);
                break;

            case SBAG_PI:
                p_status->pending_irq = p_sbag_register->pi_itm | (((t_uint32) sbag_device) << SBAG_SHIFT_BY_SIXTEEN);
                break;

            case SBAG_TMSAT:
                p_status->pending_irq = p_sbag_register->tmsat_itm | (((t_uint32) sbag_device) << SBAG_SHIFT_BY_SIXTEEN);
                break;

            default:
                return;
        }

        p_status->initial_irq = p_status->pending_irq;
    }
    else
    {
        switch (sbag_device)
        {
            case SBAG_WPSAT:
                p_status->pending_irq = ((p_sbag_register->wpsat_itm) & (sbag_irq_src & SBAG_GET_INTERRUPTS)) | (((t_uint32) sbag_device) << SBAG_SHIFT_BY_SIXTEEN);
                break;

            case SBAG_PI:
                p_status->pending_irq = ((p_sbag_register->pi_itm) & (sbag_irq_src & SBAG_GET_INTERRUPTS)) | (((t_uint32) sbag_device) << SBAG_SHIFT_BY_SIXTEEN);
                break;

            case SBAG_TMSAT:
                p_status->pending_irq = ((p_sbag_register->tmsat_itm) & (sbag_irq_src & SBAG_GET_INTERRUPTS)) | (((t_uint32) sbag_device) << SBAG_SHIFT_BY_SIXTEEN);
                break;

            default:
                return;
        }

        p_status->initial_irq = p_status->pending_irq;
    }

    DBGEXIT( SBAG_OK );
}

/****************************************************************************/
/* NAME:    SBAG_GetSBAGCurrentDevice                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Gets the Device in Use. This is needed for the ISR		*/
/* PARAMETERS	:                                                           */
/* 		IN		:  none														*/
/* 		OUT		:  none							                            */
/*                                                                          */
/* RETURN		:   t_sbag_device: The device which is in use				*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_device SBAG_GetSBAGCurrentDevice(void)
{
    t_sbag_device   sbag_device;

    sbag_device = g_sbag_system_context.sbag_device;

    return (sbag_device);
}

/****************************************************************************/
/* NAME			:  	 SBAG_PIConfigMsgSetting            					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This configures the message Settings for PI in Trace 	*/
/*					mode													*/
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_pi_msg_config:Pointer to message settings	    */
/*				:	t_sbag_pi_id: The PI Id	whose message to be included	*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the parameters 	*/
/*											are wrong						*/
/*				:	SBAG_REQUEST_NOT_APPLICABLE: When SBAG operating mode is*/
/*											not Trace mode					*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_PIConfigMsgSetting(IN t_sbag_pi_id sbag_pi_id, IN t_sbag_pi_msg_config *p_sbag_pi_msg_config)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         pi_id;
    t_uint16        pi_incdata, pi_msg_to_include = 0x1;
    t_uint32        msg_pi_trace_control = 0x0;

    DBGENTER2( " (%d), (%lx)", sbag_pi_id, p_sbag_pi_msg_config );

    if ((NULL == p_sbag_pi_msg_config) || (sbag_pi_id > SBAG_PI_ID15))
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    msg_pi_trace_control ^= msg_pi_trace_control;
    pi_id = (t_uint8) sbag_pi_id;
    pi_msg_to_include = pi_msg_to_include << pi_id;

    if (SBAG_TRACE_MODE == g_sbag_system_context.sbag_mode || SBAG_LINK_MODE == g_sbag_system_context.sbag_mode)
    {
        /*Enable the Time Message in the control register*/
        SBAG_WRITE_FIELD
        (
            msg_pi_trace_control,
            p_sbag_pi_msg_config->sbag_pi_time_msg,
            SBAG_PI_TRACE_TIME_MASK,
            SBAG_PI_TRACE_TIME_SHIFT
        );

        pi_incdata = p_sbag_pi_msg_config->sbag_pi_incdata;
        if (pi_incdata != pi_msg_to_include)
        {
            pi_incdata = pi_msg_to_include;
        }

        SBAG_WRITE_FIELD( msg_pi_trace_control, pi_incdata, SBAG_PI_TRACE_INCDATA_MASK, SBAG_PI_TRACE_INCDATA_SHIFT );

        /*Write the data into the register*/
        HCL_WRITE_REG( p_sbag_register->msg_pi_trace_control, msg_pi_trace_control );

        /*PI trigger setting*/
        if (SBAG_PI_TRIGGER_RAISING == p_sbag_pi_msg_config->sbag_pi_trigger)
        {
            SBAG_WRITE_FIELD
            (
                p_sbag_register->pi_trigger_raise,
                pi_incdata,
                SBAG_PI_TRIGGER_RAISE_MASK,
                SBAG_PI_TRIGGER_RAISE_SHIFT
            );
        }
        else
        {
            /*Falling Edge Trigger*/
            SBAG_WRITE_FIELD
            (
                p_sbag_register->pi_trigger_fall,
                pi_incdata,
                SBAG_PI_TRIGGER_FALL_MASK,
                SBAG_PI_TRIGGER_FALL_SHIFT
            );
        }
    }
    else
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_WriteFPFData		                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: Writes the FPF Data A special case of achieving s/w reset */
/*				   is by writing 0xFFFFFFFF									*/
/* PARAMETERS	:                                                           */
/* 		IN		:  	t_uint32: Data to be written							*/
/*																			*/
/* 		OUT		:   							                            */
/*                                                                          */
/* RETURN		:   SBAG_OK: if there is no error							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_WriteFPFData(IN t_uint32 sbag_data)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    DBGENTER1( "%d", sbag_data );

    /*Store the Value in System context*/
    g_sbag_system_context.fpf_message = sbag_data;
    p_sbag_register->msg_fpf_data = sbag_data;

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME:    SBAG_ReadFPFData		                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: Reads the FPF Data										*/
/* PARAMETERS	:                                                           */
/* 		INOUT	:  	t_uint32: Data Read										*/
/*																			*/
/*                                                                          */
/* RETURN		:   SBAG_OK: if there is no error							*/
/*				:	SBAG_INVALID_PARAMETER: If address is NULL				*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sbag_error SBAG_ReadFPFData(INOUT t_uint32 *sbag_data)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    DBGENTER1( "%d", sbag_data );
    if (NULL == sbag_data)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    *sbag_data = p_sbag_register->msg_fpf_data;

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_WPSATEnterPgmMode             					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is used to make a WPSAT in Programming Mode		*/
/*					Each WPSAT Has to be in Programming mode for writing 	*/
/*					the data to be matched									*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_wpsat_id: The Wp Satellite Id				    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the Mode is not	*/
/*											valid							*/
/*				:	SBAG_REQUEST_NOT_APPLICABLE : when it is already 		*/
/*											programmed						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_WPSATEnterPgmMode(IN t_sbag_wpsat_id sbag_wpsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        wpsat_prog_status_test = 0x1;
    t_uint8         wpsat_id_shift;

    DBGENTER1( " (%d)", sbag_wpsat_id );

    if (sbag_wpsat_id > SBAG_WPSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    wpsat_id_shift = (t_uint8) sbag_wpsat_id;

    /*Before Entering into Programming mode for any WPSAT check if it is*/
    /*Not in the programming mode*/
    wpsat_prog_status_test = wpsat_prog_status_test << wpsat_id_shift;
    if (HCL_READ_BITS( p_sbag_register->wpsat_prog_status, wpsat_prog_status_test ))
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }
    else
    {
        HCL_SET_BITS( p_sbag_register->wpsat_prog_mode, wpsat_prog_status_test );
    }

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:   SBAG_WPSATClearPgmMode             						*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This has the effect of clearing the programming mode	*/
/*					for all the WPSAT										*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	none												    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_WPSATClearPgmMode(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    HCL_SET_BITS( p_sbag_register->wpsat_prog_clear, SBAG_WPSAT_CLR_PGM_MODE );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:   SBAG_TMSATEnterPgmMode             						*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Enter in Programming mode for each TM Satellite			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_tmsat_id: The Tm Satellite Id				    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*				:	SBAG_INVALID_PARAMETER: Returns this if the Mode is not	*/
/*											valid							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_TMSATEnterPgmMode(IN t_sbag_tmsat_id sbag_tmsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        tmsat_prog_status_test = 0x1;
    t_uint8         tmsat_id_shift;

    DBGENTER1( " (%d)", sbag_tmsat_id );

    if (sbag_tmsat_id > SBAG_TMSAT_ID15)
    {
        DBGEXIT0( SBAG_INVALID_PARAMETER );
        return (SBAG_INVALID_PARAMETER);
    }

    tmsat_id_shift = (t_uint8) sbag_tmsat_id;

    /*Before Entering into Programming mode for any WPSAT check if it is*/
    /*Not in the programming mode*/
    tmsat_prog_status_test = tmsat_prog_status_test << tmsat_id_shift;
    if (HCL_READ_BITS( p_sbag_register->tmsat_prog_status, tmsat_prog_status_test ))
    {
        DBGEXIT0( SBAG_REQUEST_NOT_APPLICABLE );
        return (SBAG_REQUEST_NOT_APPLICABLE);
    }
    else
    {
        HCL_SET_BITS( p_sbag_register->tmsat_prog_mode, tmsat_prog_status_test );
    }

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 SBAG_TMSATClearPgmMode             					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This has the effect of clearing the programming mode	*/
/*					for all the TMSAT										*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	none												    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PUBLIC t_sbag_error SBAG_TMSATClearPgmMode(void)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;

    HCL_SET_BITS( p_sbag_register->tmsat_prog_clear, SBAG_TMSAT_CLR_PGM_MODE );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/*--------------------------------------------------------------------------*/
/*						PRIVATE API's 										*/
/*--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:  	 sbag_WPSATSetDataAvailability        					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This enables the setting of Data Availability Flag 		*/
/*					whenever a match occurs									*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_wpsat_id: The Wp Satellite Id					*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PRIVATE t_sbag_error sbag_WPSATSetDataAvailability(IN t_sbag_wpsat_id sbag_wpsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        wpsat_its_set = 0x1;
    t_uint8         wpsat_id_shift;

    DBGENTER1( " (%d)", sbag_wpsat_id );

    wpsat_id_shift = (t_uint8) sbag_wpsat_id;
    wpsat_its_set = wpsat_its_set << wpsat_id_shift;

    /*Set the Data Availability Indication*/
    HCL_SET_BITS( p_sbag_register->wpsat_its_set, wpsat_its_set );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 sbag_TMSATSetDataAvailability        					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This enables the setting of Data Availability Flag 		*/
/*					whenever a match occurs									*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_tmsat_id: The Tm Satellite Id					*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PRIVATE t_sbag_error sbag_TMSATSetDataAvailability(IN t_sbag_tmsat_id sbag_tmsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        tmsat_its_set = 0x1;
    t_uint8         tmsat_id_shift;

    DBGENTER1( " (%d)", sbag_tmsat_id );

    tmsat_id_shift = (t_uint8) sbag_tmsat_id;
    tmsat_its_set = tmsat_its_set << tmsat_id_shift;

    /*Set the Data Availability Indication*/
    HCL_SET_BITS( p_sbag_register->tmsat_its_set, tmsat_its_set );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 sbag_WPSATClearDataAvailability       					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This clears the data Availability flag after a match	*/
/*                  occurs                                                  */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_wpsat_id: The Wp Satellite Id					*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PRIVATE t_sbag_error sbag_WPSATClearDataAvailability(IN t_sbag_wpsat_id sbag_wpsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        wpsat_its_clr = 0x1;
    t_uint8         wpsat_id_shift;

    DBGENTER1( " (%d)", sbag_wpsat_id );

    wpsat_id_shift = (t_uint8) sbag_wpsat_id;
    wpsat_its_clr = wpsat_its_clr << wpsat_id_shift;

    /*Clear the Data Availability Indication*/
    HCL_CLEAR_BITS( p_sbag_register->wpsat_its_clr, wpsat_its_clr );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 sbag_TMSATClearDataAvailability        				*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This clears the data availability flag after the match	*/
/*					occurs                                                  */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_tmsat_id: The Tm Satellite Id					*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PRIVATE t_sbag_error sbag_TMSATClearDataAvailability(IN t_sbag_tmsat_id sbag_tmsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint32        tmsat_its_clr = 0x1;

    DBGENTER1( " (%d)", sbag_tmsat_id );

    /*Clear the Data Availability Indication*/
    HCL_CLEAR_BITS( p_sbag_register->tmsat_its_clr, tmsat_its_clr );

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 sbag_GetTMSATErrorStatus        						*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This gets the Errors in the TMSAT metric computation    */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_tmsat_id: The Tm Satellite Id					*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_TMSAT_METRIC1_ERROR: Error in Metric1 computation  */
/*				:	SBAG_TMSAT_METRIC2_ERROR: Error in Metric2 Computation	*/
/*				:	SBAG_TMSAT_METRIC3_ERROR: Error in Metric3 Computation	*/
/*				:	SBAG_TMSAT_LOST_ERROR:	If the errors are lost 			*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PRIVATE t_sbag_error sbag_GetTMSATErrorStatus(IN t_sbag_tmsat_id sbag_tmsat_id)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_sbag_error    sbag_error = SBAG_OK;
    t_uint32        error_mask = 0xF;
    t_uint32        error_result;
    t_uint8         shift, temp_id;
    t_uint8         tmsat_id = (t_uint8) sbag_tmsat_id;

    DBGENTER1( " (%d)", sbag_tmsat_id );

    if (sbag_tmsat_id <= SBAG_TMSAT_ID7)
    {
        shift = ((3 * tmsat_id) + tmsat_id);
        error_result = p_sbag_register->tmsat0_7_error & (error_mask << shift);
    }
    else
    {
        temp_id = tmsat_id & SBAG_GET_TMSAT_ID;
        shift = ((3 * temp_id) + temp_id);
        error_result = p_sbag_register->tmsat8_15_error & (error_mask << shift);
    }

    if (SBAG_TEST_BIT( error_result, TMSAT_METRIC1_ERROR ))
    {
        sbag_error = SBAG_TMSAT_METRIC1_ERROR;
        return (sbag_error);
    }
    else if (SBAG_TEST_BIT( error_result, TMSAT_METRIC2_ERROR ))
    {
        sbag_error = SBAG_TMSAT_METRIC2_ERROR;
        return (sbag_error);
    }
    else if (SBAG_TEST_BIT( error_result, TMSAT_METRIC3_ERROR ))
    {
        sbag_error = SBAG_TMSAT_METRIC3_ERROR;
        return (sbag_error);
    }
    else if (SBAG_TEST_BIT( error_result, TMSAT_ERROR_ACT_MCT ))
    {
        sbag_error = SBAG_TMSAT_ERROR_ACT_MCT;
        return (sbag_error);
    }
    else if (p_sbag_register->tm_lost_error != 0x0)
    {
        sbag_error = SBAG_TMSAT_LOST_ERROR;
    }

    return (sbag_error);
}

/****************************************************************************/
/* NAME			:  	 sbag_ProcessIt					        				*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This clears the data availability flag after the match	*/
/*					occurs                                                  */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_tmsat_id: The Tm Satellite Id					*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PRIVATE t_sbag_error sbag_ProcessIt(IN t_sbag_irq_src sbag_irq_src)
{
    t_sbag_device   sbag_device = (t_sbag_device) (sbag_irq_src >> SBAG_SHIFT_BY_SIXTEEN);
    t_uint32        interrupt_src;
    t_sbag_error    sbag_error = SBAG_NO_MORE_PENDING_EVENT;

    /*Get the interrupts*/
    interrupt_src = sbag_irq_src & SBAG_GET_INTERRUPTS;
    switch (sbag_device)
    {
        case SBAG_WPSAT:
            sbag_error = sbag_WPSATProcessIt( interrupt_src );
            break;

        case SBAG_PI:
            sbag_error = sbag_PIProcessIt( interrupt_src );
            break;

        case SBAG_TMSAT:
            sbag_error = sbag_TMSATProcessIt( interrupt_src );
            break;
    }

    DBGEXIT0( sbag_error );
    return (sbag_error);
}

/****************************************************************************/
/* NAME			:  	 sbag_WPSATProcessIt			        				*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is the interrupt service routine for handling WPSAT*/
/*					interrupt                                               */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_uint32: Interrupt Source								*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PRIVATE t_sbag_error sbag_WPSATProcessIt(t_uint32 interrupt_src)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         count = 0x0;
    t_uint32        interrupt = 0x1;
    t_uint32        mask;
    t_sbag_wpsat_id sbag_wpsat_id;

    while (count <= 15)
    {
        mask = interrupt << count;
        if (SBAG_TEST_BIT( interrupt_src, mask ))
        {
            sbag_wpsat_id = (t_sbag_wpsat_id) count;
            if (SBAG_TEST_BIT( p_sbag_register->wpsat_its, mask ))
            {
                SBAG_ReadWPSAT( sbag_wpsat_id );
            }
            else
            {
                DBGEXIT0( SBAG_DATA_UNAVILABLE );
                return (SBAG_DATA_UNAVILABLE);
            }

            /*After Reading clear the interrupt*/
            HCL_SET_BITS( p_sbag_register->wpsat_itm_clr, mask );

            /*After Reading also clear the data Available Flag*/
            HCL_SET_BITS( p_sbag_register->wpsat_its_clr, mask );
            break;
        }
        else
        {
            count = count + 1;
        }
    }

    DBGEXIT0( SBAG_OK );
    return (SBAG_OK);
}

/****************************************************************************/
/* NAME			:  	 sbag_TMSATProcessIt			        				*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is the interrupt service routine for handling TMSAT*/
/*					interrupt												*/
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_uint32: Interrupt Source								*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PRIVATE t_sbag_error sbag_TMSATProcessIt(t_uint32 interrupt_src)
{
    t_sbag_register *p_sbag_register = g_sbag_system_context.p_sbag_register;
    t_uint8         count = 0x0;
    t_uint32        mask, interrupt = 0x1;
    t_sbag_tmsat_id sbag_tmsat_id;
    t_sbag_error    sbag_error = SBAG_OK;

    while (count <= 15)
    {
        mask = interrupt << count;
        if (SBAG_TEST_BIT( interrupt_src, mask ))
        {
            sbag_tmsat_id = (t_sbag_tmsat_id) count;
            if (SBAG_TEST_BIT( p_sbag_register->tmsat_its, mask ))
            {
                SBAG_ReadTMSAT( sbag_tmsat_id );
            }
            else
            {
                sbag_error = sbag_GetTMSATErrorStatus( sbag_tmsat_id );
				DBGEXIT0( sbag_error );
                return (sbag_error);
            }

            /*After Reading clear the interrupts*/
            HCL_SET_BITS( p_sbag_register->tmsat_itm_clr, mask );

            /*After Reading clear the data availability bit*/
            HCL_SET_BITS( p_sbag_register->wpsat_its_clr, 0x1 );
            break;
        }
        else
        {
            count = count + 1;
        }
    }

    DBGEXIT0( sbag_error );
    return (sbag_error);
}

/****************************************************************************/
/* NAME			:  	 sbag_PIProcessIt			        					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This is the interrupt service routine for handling PI	*/
/*					interrupt												*/
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_uint32: Interrupt Source								*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	SBAG_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/

/****************************************************************************/
PRIVATE t_sbag_error sbag_PIProcessIt(t_uint32 interrupt_src)
{
    /*To be Done*/
    return (SBAG_OK);
}

/*end of sbag.c*/
