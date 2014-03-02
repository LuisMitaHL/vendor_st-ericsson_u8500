
/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   MCDE Driver
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "mcde.h"
#include "mcde_p.h"
#include "hcl_defs.h"
#include "debug.h"

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
PRIVATE volatile t_mcde_system_context  g_mcde_system_context;

/*-----------------------------------------------------------------------------
 * DEBUG STUFF
 *---------------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_MCDE
#define MY_DEBUG_ID             myDebugID_MCDE

t_dbg_level                             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
t_dbg_id                                MY_DEBUG_ID = MCDE_HCL_DBG_ID;
#endif

/*---------------------------------------------------------------------------
*  Public Functions                                                     
*---------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	MCDE_Init()												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine initializes the MCDE registers, checks 	*/
/*				   Peripheral and PCell Id	and clears all interrupts.		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_logical_address : mcde_base_address:MCDE registers base  */
/*                                    address	                            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_Init(IN t_logical_address mcde_base_address)
{
    t_mcde_error    mcde_error = MCDE_OK;
/*    t_uint32        mcde_pid = 0x0;*/

    DBGENTER1("Setting Base Address for registers to %lx", mcde_base_address);

    if (NULL == mcde_base_address)
    {
        return(MCDE_INVALID_PARAMETER);
    }

 /*   mcde_pid = ((MCDE_PID_MAJOR_VERSION << SHIFT_BYTE3) || (MCDE_PID_MINOR_VERSION << SHIFT_BYTE2) || (MCDE_PID_DEV_VERSION << SHIFT_BYTE1) || MCDE_PID_METALFIX_VERSION);*/
    g_mcde_system_context.p_mcde_register = (t_mcde_register *) mcde_base_address;

/*    if
    (
        (mcde_pid != g_mcde_system_context.p_mcde_register->mcde_pid)
    )
    {
        return(MCDE_UNSUPPORTED_HW);
    }*/
    DBGEXIT0(mcde_error);
    return(mcde_error);
}

/****************************************************************************/
/* NAME			:	MCDE_SetDbgLevel()										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine enables to choose between different	  		*/
/*				  debug comment levels										*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dbg_level mcde_dbg_level:identify MCDE debug level		*/
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK         : if it is ok                               */
/*               MCDE_INVALID_PARAMETER  if debug level exceed the certain   */
/*                                      level                               */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetDbgLevel(IN t_dbg_level mcde_dbg_level)
{
    t_mcde_error    error = MCDE_OK;

    DBGENTER1("Setting Debug Level to %d", mcde_dbg_level);

#ifdef __DEBUG
    if (mcde_dbg_level < 0xFFFFFFFF)    /*Debug level should not exceed */
    {
        MY_DEBUG_LEVEL_VAR_NAME = mcde_dbg_level;
        error = MCDE_OK;
    }
    else
    {
        error = MCDE_INVALID_PARAMETER;
    }
#endif
    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_GetVersion()											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This function is Panel Dependent so function will return	*/
/*				the HCL Software Version, if such data is not provided		*/
/*				by the Hardware.											*/
/*																			*/
/* PARAMETERS :																*/
/*         IN :																*/
/*     InOut  :None                                                         */
/*		  OUT :	pt_version: Contains the Major, Minor & Build Version		*/
/*							of the software.								*/
/*                                                                          */
/* RETURN	  :  t_mcde_error  mcde_error            				 	    */
/*             MCDE_OK                                                      */
/*             MCDE_INVALID_PARAMETER: if input argument is invalid         */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetVersion(OUT t_version *pt_version)
{
    DBGENTER0();
    if (NULL == pt_version)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    pt_version->version = MCDE_HCL_VERSION_ID;
    pt_version->major = MCDE_MAJOR_ID;
    pt_version->minor = MCDE_MINOR_ID;
    DBGEXIT3(MCDE_OK, "%u %u %u", pt_version->major, pt_version->minor, pt_version->version);
    return(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetState()											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables/disables the MCDE 	                */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_state state:Enable/Disable bit                      */
/*                                    address	                            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetState(IN t_mcde_state state)
{

    DBGENTER0();

    g_mcde_system_context.p_mcde_register->mcde_cr =
        (
            (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_MCDEEN_MASK) |
            (((t_uint32) state << MCDE_CTRL_MCDEEN_SHIFT) & MCDE_CTRL_MCDEEN_MASK)
        );

    DBGEXIT0(MCDE_OK);

}

/****************************************************************************/
/* NAME			:	MCDE_GetState()											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine reads the enable/disable state of  the MCDE*/
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :t_mcde_state *state:Pointer to MCDE state                  */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetState(OUT t_mcde_state *p_state)
{
    t_mcde_error    error = MCDE_OK;

    DBGENTER0();

    if (NULL == p_state)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    *p_state = (t_mcde_state) ((g_mcde_system_context.p_mcde_register->mcde_cr & MCDE_CTRL_MCDEEN_MASK) >> MCDE_CTRL_MCDEEN_SHIFT);

    DBGEXIT0(error);

    return(error);
}

PRIVATE t_mcde_error mcde_SetFIFOAControl(t_mcde_fifo_output fifoa)
{
    t_mcde_error error = MCDE_OK;

     /*FIFO A Output Selection*/
    switch (fifoa)
    {
        case MCDE_DPI_A:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIA_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_DPIA_EN_SHIFT) & MCDE_CTRL_DPIA_EN_MASK)
                );
            break;

        case MCDE_DSI_VID0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIA_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIA_EN_SHIFT) & MCDE_CTRL_DPIA_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID0] = MCDE_CH_A;

            break;

        case MCDE_DSI_VID1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIA_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIA_EN_SHIFT) & MCDE_CTRL_DPIA_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID1_EN_SHIFT) & MCDE_DSIVID1_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID1] = MCDE_CH_A;
            break;

        case MCDE_DSI_CMD2:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIA_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIA_EN_SHIFT) & MCDE_CTRL_DPIA_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID1_EN_SHIFT) & MCDE_DSIVID1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD2_EN_MASK) |
                    ((t_uint32) MCDE_SET_BIT & MCDE_DSICMD2_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD2] = MCDE_CH_A;
            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }
    return(error);
}


PRIVATE t_mcde_error mcde_SetFIFOBControl(t_mcde_fifo_output fifob)
{
    t_mcde_error error = MCDE_OK;

    /*FIFO B Output Selection*/
    switch (fifob)
    {
        case MCDE_DPI_B:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIB_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_DPIB_EN_SHIFT) & MCDE_CTRL_DPIB_EN_MASK)
                );
            break;

        case MCDE_DSI_VID0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIB_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIB_EN_SHIFT) & MCDE_CTRL_DPIB_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );
            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID0] = MCDE_CH_B;
            break;

        case MCDE_DSI_VID1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIB_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIB_EN_SHIFT) & MCDE_CTRL_DPIB_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID1_EN_SHIFT) & MCDE_DSIVID1_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID1] = MCDE_CH_B;
            break;

        case MCDE_DSI_CMD2:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIB_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIB_EN_SHIFT) & MCDE_CTRL_DPIB_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID1_EN_SHIFT) & MCDE_DSIVID1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD2_EN_MASK) |
                    ((t_uint32) MCDE_SET_BIT & MCDE_DSICMD2_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD2] = MCDE_CH_B;

            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }
    return(error);
}

PRIVATE t_mcde_error mcde_SetFIFO0Control(t_mcde_fifo_output fifo0)
{
    t_mcde_error error = MCDE_OK;

    switch (fifo0)
    {
        case MCDE_DBI_C0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_DBIC0_EN_SHIFT) & MCDE_CTRL_DBIC0_EN_MASK)
                );
            break;

        case MCDE_DSI_CMD0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC0_EN_SHIFT) & MCDE_CTRL_DBIC0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD0] = MCDE_CH_C0;

            break;

        case MCDE_DSI_CMD1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC0_EN_SHIFT) & MCDE_CTRL_DBIC0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSICMD1_EN_SHIFT) & MCDE_DSICMD1_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD1] = MCDE_CH_C0;
            break;

        case MCDE_DSI_VID2:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC0_EN_SHIFT) & MCDE_CTRL_DBIC0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD1_EN_SHIFT) & MCDE_DSICMD1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID2_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID2_EN_SHIFT) & MCDE_DSIVID2_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID2] = MCDE_CH_C0;

            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }
    return(error);
}


PRIVATE t_mcde_error mcde_SetFIFO1Control(t_mcde_fifo_output fifo1)
{
    t_mcde_error error = MCDE_OK;

    switch (fifo1)
    {
        case MCDE_DBI_C1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_DBIC1_EN_SHIFT) & MCDE_CTRL_DBIC1_EN_MASK)
                );
            break;

        case MCDE_DSI_CMD0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC1_EN_SHIFT) & MCDE_CTRL_DBIC1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD0] = MCDE_CH_C1;

            break;

        case MCDE_DSI_CMD1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC1_EN_SHIFT) & MCDE_CTRL_DBIC1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSICMD1_EN_SHIFT) & MCDE_DSICMD1_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD1] = MCDE_CH_C1;

            break;

        case MCDE_DSI_CMD2:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC1_EN_SHIFT) & MCDE_CTRL_DBIC1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD1_EN_SHIFT) & MCDE_DSICMD1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID2_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID2_EN_SHIFT) & MCDE_DSIVID2_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD2] = MCDE_CH_C1;

            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_SetFIFOControl()					     			*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the formatter selection for output FIFOs*/
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_fifo_ctrl : FIFO selection control structure        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetFIFOControl(IN t_mcde_fifo_ctrl *fifo_ctrl)
{
    t_mcde_error    error = MCDE_OK;

    DBGENTER0();

    /*FIFO A Output Selection*/
    error = mcde_SetFIFOAControl(fifo_ctrl->out_fifoa);
 /*   
    switch (fifo_ctrl.out_fifoa)
    {
        case MCDE_DPI_A:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIA_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_DPIA_EN_SHIFT) & MCDE_CTRL_DPIA_EN_MASK)
                );
            break;

        case MCDE_DSI_VID0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIA_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIA_EN_SHIFT) & MCDE_CTRL_DPIA_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID0] = MCDE_CH_A;

            break;

        case MCDE_DSI_VID1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIA_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIA_EN_SHIFT) & MCDE_CTRL_DPIA_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID1_EN_SHIFT) & MCDE_DSIVID1_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID1] = MCDE_CH_A;
            break;

        case MCDE_DSI_CMD2:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIA_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIA_EN_SHIFT) & MCDE_CTRL_DPIA_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID1_EN_SHIFT) & MCDE_DSIVID1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD2_EN_MASK) |
                    ((t_uint32) MCDE_SET_BIT & MCDE_DSICMD2_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD2] = MCDE_CH_A;
            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }*/

    /*FIFO B Output Selection*/
    error = mcde_SetFIFOBControl(fifo_ctrl->out_fifob);
/*    switch (fifo_ctrl.out_fifob)
    {
        case MCDE_DPI_B:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIB_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_DPIB_EN_SHIFT) & MCDE_CTRL_DPIB_EN_MASK)
                );
            break;

        case MCDE_DSI_VID0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIB_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIB_EN_SHIFT) & MCDE_CTRL_DPIB_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );
            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID0] = MCDE_CH_B;
            break;

        case MCDE_DSI_VID1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIB_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIB_EN_SHIFT) & MCDE_CTRL_DPIB_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID1_EN_SHIFT) & MCDE_DSIVID1_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID1] = MCDE_CH_B;
            break;

        case MCDE_DSI_CMD2:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DPIB_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DPIB_EN_SHIFT) & MCDE_CTRL_DPIB_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_FABMUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_FABMUX_SHIFT) & MCDE_CTRL_FABMUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID0_EN_SHIFT) & MCDE_DSIVID0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSIVID1_EN_SHIFT) & MCDE_DSIVID1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD2_EN_MASK) |
                    ((t_uint32) MCDE_SET_BIT & MCDE_DSICMD2_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD2] = MCDE_CH_B;

            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }*/

    /*FIFO 0 Output Selection*/
    error = mcde_SetFIFO0Control(fifo_ctrl->out_fifo0);
 /*   switch (fifo_ctrl.out_fifo0)
    {
        case MCDE_DBI_C0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_DBIC0_EN_SHIFT) & MCDE_CTRL_DBIC0_EN_MASK)
                );
            break;

        case MCDE_DSI_CMD0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC0_EN_SHIFT) & MCDE_CTRL_DBIC0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD0] = MCDE_CH_C0;

            break;

        case MCDE_DSI_CMD1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC0_EN_SHIFT) & MCDE_CTRL_DBIC0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSICMD1_EN_SHIFT) & MCDE_DSICMD1_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD1] = MCDE_CH_C0;
            break;

        case MCDE_DSI_VID2:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC0_EN_SHIFT) & MCDE_CTRL_DBIC0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD1_EN_SHIFT) & MCDE_DSICMD1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID2_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID2_EN_SHIFT) & MCDE_DSIVID2_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_VID2] = MCDE_CH_C0;

            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }*/

    /*FIFO 1 Output Selection*/
    error = mcde_SetFIFO1Control(fifo_ctrl->out_fifo1);
 /*   switch (fifo_ctrl.out_fifo1)
    {
        case MCDE_DBI_C1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_DBIC1_EN_SHIFT) & MCDE_CTRL_DBIC1_EN_MASK)
                );
            break;

        case MCDE_DSI_CMD0:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC1_EN_SHIFT) & MCDE_CTRL_DBIC1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD0] = MCDE_CH_C1;

            break;

        case MCDE_DSI_CMD1:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC1_EN_SHIFT) & MCDE_CTRL_DBIC1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD1_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSICMD1_EN_SHIFT) & MCDE_DSICMD1_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD1] = MCDE_CH_C1;

            break;

        case MCDE_DSI_CMD2:
            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_DBIC1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_CTRL_DBIC1_EN_SHIFT) & MCDE_CTRL_DBIC1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_CTRL_F01MUX_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_CTRL_F01MUX_SHIFT) & MCDE_CTRL_F01MUX_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD0_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD0_EN_SHIFT) & MCDE_DSICMD0_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSICMD1_EN_MASK) |
                    (((t_uint32) MCDE_CLEAR_BIT << MCDE_DSICMD1_EN_SHIFT) & MCDE_DSICMD1_EN_MASK)
                );

            g_mcde_system_context.p_mcde_register->mcde_cr =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_cr &~MCDE_DSIVID2_EN_MASK) |
                    (((t_uint32) MCDE_SET_BIT << MCDE_DSIVID2_EN_SHIFT) & MCDE_DSIVID2_EN_MASK)
                );

            g_mcde_system_context.dsi_formatter_plugged_channel[MCDE_DSI_CH_CMD2] = MCDE_CH_C1;

            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }*/

    DBGEXIT0(error);

    return(error);
}
/****************************************************************************/
/* NAME			:	MCDE_SetControlConfig()									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets Control Structure for the MCDE 	    */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_control  config:Config information                  */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetControlConfig(IN t_mcde_control *config)
{

    DBGENTER0();

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_OUTMUX4_MASK) |
            (((t_uint32) config->data_msb1 << MCDE_CTRL_OUTMUX4_SHIFT) & MCDE_CTRL_OUTMUX4_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_OUTMUX3_MASK) |
            (((t_uint32) config->data_msb0 << MCDE_CTRL_OUTMUX3_SHIFT) & MCDE_CTRL_OUTMUX3_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_OUTMUX2_MASK) |
            (((t_uint32) config->data_mid << MCDE_CTRL_OUTMUX2_SHIFT) & MCDE_CTRL_OUTMUX2_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_OUTMUX1_MASK) |
            (((t_uint32) config->data_lsb1 << MCDE_CTRL_OUTMUX1_SHIFT) & MCDE_CTRL_OUTMUX1_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_OUTMUX0_MASK) |
            (((t_uint32) config->data_lsb0 << MCDE_CTRL_OUTMUX0_SHIFT) & MCDE_CTRL_OUTMUX0_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_IFIFO_WATERMARK_MASK) |
            (((t_uint32) config->ififo_watermark << MCDE_IFIFO_WATERMARK_SHIFT) & MCDE_IFIFO_WATERMARK_MASK)
        );

    DBGEXIT0(MCDE_OK);

}

/****************************************************************************/
/* NAME			:	MCDE_SetChFlowControl()									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine configures channels' flow parameters       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_mode_ctrl mode: Frame sync and Flow control      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetChFlowControl(IN t_mcde_ch_mode_ctrl *mode)
{
    DBGENTER0();

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_FSYNCTRLB_MASK) |
            (((t_uint32) mode->sync_ctrl_chB << MCDE_CTRL_FSYNCTRLB_SHIFT) & MCDE_CTRL_FSYNCTRLB_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_FSYNCTRLA_MASK) |
            (((t_uint32) mode->sync_ctrl_chA << MCDE_CTRL_FSYNCTRLA_SHIFT) & MCDE_CTRL_FSYNCTRLA_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_CTRLB_MASK) |
            (((t_uint32) mode->flow_ctrl_chB << MCDE_CTRL_CTRLB_SHIFT) & MCDE_CTRL_CTRLB_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_conf0 =
        (
            (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_CTRLA_MASK) |
            (((t_uint32) mode->flow_ctrl_chA << MCDE_CTRL_CTRLA_SHIFT) & MCDE_CTRL_CTRLA_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetChFSyncCtrl()									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine configures channels' frame sync control    */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel:Channel Type(A/B)                     */
/*	             t_mcde_ch_ctrl fsync_ctrl: Muxing mode  or Normal mode     */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetChFSyncCtrl(IN t_mcde_ch_id channel, IN t_mcde_ch_ctrl fsync_ctrl)
{
    t_mcde_error    error = MCDE_OK;

    DBGENTER0();

    switch (channel)
    {
        case MCDE_CH_A:
            g_mcde_system_context.p_mcde_register->mcde_conf0 =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_FSYNCTRLA_MASK) |
                    (((t_uint32) fsync_ctrl << MCDE_CTRL_FSYNCTRLA_SHIFT) & MCDE_CTRL_FSYNCTRLA_MASK)
                );
            break;

        case MCDE_CH_B:
            g_mcde_system_context.p_mcde_register->mcde_conf0 =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_CTRL_FSYNCTRLB_MASK) |
                    (((t_uint32) fsync_ctrl << MCDE_CTRL_FSYNCTRLB_SHIFT) & MCDE_CTRL_FSYNCTRLB_MASK)
                );
            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }

    DBGEXIT0(error);

    return(error);
}
/****************************************************************************/
/* NAME			:	MCDE_SetOutputConfiguration()	    					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine configures channels' Output config         */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_output_conf output_conf:Output Config               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetOutputConfiguration(IN t_mcde_output_conf output_conf)
{
    t_mcde_error    error = MCDE_OK;

    DBGENTER0();
    switch (output_conf)
    {
        case MCDE_CONF_TVA_DPIC0_LCDB:
            g_mcde_system_context.p_mcde_register->mcde_conf0 =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_SYNCMUX_MASK) |
                    ((t_uint32) MCDE_TVA_DPIC0_LCDB_MASK & MCDE_SYNCMUX_MASK)
                );
            break;

        case MCDE_CONF_TVB_DPIC1_LCDA:
            g_mcde_system_context.p_mcde_register->mcde_conf0 =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_SYNCMUX_MASK) |
                    ((t_uint32) MCDE_TVB_DPIC1_LCDA_MASK & MCDE_SYNCMUX_MASK)
                );
            break;

        case MCDE_CONF_DPIC1_LCDA:
            g_mcde_system_context.p_mcde_register->mcde_conf0 =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_SYNCMUX_MASK) |
                    ((t_uint32) MCDE_DPIC1_LCDA_MASK & MCDE_SYNCMUX_MASK)
                );
            break;

        case MCDE_CONF_DPIC0_LCDB:
            g_mcde_system_context.p_mcde_register->mcde_conf0 =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_SYNCMUX_MASK) |
                    ((t_uint32) MCDE_DPIC0_LCDB_MASK & MCDE_SYNCMUX_MASK)
                );
            break;

        case MCDE_CONF_LCDA_LCDB:
            g_mcde_system_context.p_mcde_register->mcde_conf0 =
                (
                    (g_mcde_system_context.p_mcde_register->mcde_conf0 &~MCDE_SYNCMUX_MASK) |
                    ((t_uint32) MCDE_LCDA_LCDB_MASK & MCDE_SYNCMUX_MASK)
                );
            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_GetControlConfig()									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets Control Structure for the MCDE 	    */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_mcde_control  *config:Config information                 */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetControlConfig(OUT t_mcde_control *p_config)
{
    DBGENTER0();
    
    if ((NULL == p_config))
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_config->data_msb1 = (t_mcde_out_mux_ctrl) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_CTRL_OUTMUX4_MASK) >> MCDE_CTRL_OUTMUX4_SHIFT);

    p_config->data_msb0 = (t_mcde_out_mux_ctrl) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_CTRL_OUTMUX3_MASK) >> MCDE_CTRL_OUTMUX3_SHIFT);

    p_config->data_mid = (t_mcde_out_mux_ctrl) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_CTRL_OUTMUX2_MASK) >> MCDE_CTRL_OUTMUX2_SHIFT);

    p_config->data_lsb1 = (t_mcde_out_mux_ctrl) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_CTRL_OUTMUX1_MASK) >> MCDE_CTRL_OUTMUX1_SHIFT);

    p_config->data_lsb0 = (t_mcde_out_mux_ctrl) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_CTRL_OUTMUX0_MASK) >> MCDE_CTRL_OUTMUX0_SHIFT);

    p_config->ififo_watermark = (t_uint8) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_IFIFO_WATERMARK_MASK) >> MCDE_IFIFO_WATERMARK_SHIFT);

    DBGEXIT0(MCDE_OK);
    
    return(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_GetChFlowCtrl()									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets channels' normal or muxing mode info  */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*	             None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_mcde_ch_mode_ctrl *mode: Muxing mode  or Normal mode      */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetChFlowControl(OUT t_mcde_ch_mode_ctrl *p_mode)
{
    DBGENTER0();
    
    if ((NULL == p_mode))
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_mode->sync_ctrl_chB = (t_mcde_ch_ctrl) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_CTRL_FSYNCTRLB_MASK) >> MCDE_CTRL_FSYNCTRLB_SHIFT);

    p_mode->sync_ctrl_chA = (t_mcde_ch_ctrl) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_CTRL_FSYNCTRLA_MASK) >> MCDE_CTRL_FSYNCTRLA_SHIFT);

    p_mode->flow_ctrl_chB = (t_mcde_swap_b_c1_ctrl) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_CTRL_CTRLB_MASK) >> MCDE_CTRL_CTRLB_SHIFT);

    p_mode->flow_ctrl_chA = (t_mcde_swap_a_c0_ctrl) ((g_mcde_system_context.p_mcde_register->mcde_conf0 & MCDE_CTRL_CTRLA_MASK) >> MCDE_CTRL_CTRLA_SHIFT);

    DBGEXIT0(MCDE_OK);
    
    return(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetBufferBaseAddress()								*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the base address of the buffer. */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_buffer_id buffer_id : Buffer id whose address is    */
/*                                              to be configured            */
/*               t_uint32 address   : Address of the buffer                 */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetBufferBaseAddress
(
    IN t_mcde_ext_src   src_id,
    IN t_mcde_buffer_id buffer_id,
    IN t_uint32         address
)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    switch (buffer_id)
    {
	case MCDE_BUFFER_ID_0:
		/*coverity[bit_and_with_zero]*/
            ext_src->mcde_extsrc_a0 =
                (
                    (ext_src->mcde_extsrc_a0 &~MCDE_EXT_BUFFER_MASK) |
                    ((address << MCDE_EXT_BUFFER_SHIFT) & MCDE_EXT_BUFFER_MASK)
                );

            break;

	case MCDE_BUFFER_ID_1:
		/*coverity[bit_and_with_zero]*/
            ext_src->mcde_extsrc_a1 =
                (
                    (ext_src->mcde_extsrc_a1 &~MCDE_EXT_BUFFER_MASK) |
                    ((address << MCDE_EXT_BUFFER_SHIFT) & MCDE_EXT_BUFFER_MASK)
                );

            break;

	case MCDE_BUFFER_ID_2:
        /*coverity[bit_and_with_zero]*/
            ext_src->mcde_extsrc_a2 =
                (
                    (ext_src->mcde_extsrc_a2 &~MCDE_EXT_BUFFER_MASK) |
                    ((address << MCDE_EXT_BUFFER_SHIFT) & MCDE_EXT_BUFFER_MASK)
                );

            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_SetExtSrcConfig()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the External Source       */
/*                  Color Format,Endianity and buffer configuration         */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_ext_conf config : Configuration structure           */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetExtSrcConfig(IN t_mcde_ext_src src_id, IN t_mcde_ext_conf *config)
{
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BEPO_MASK) |
            (((t_uint32) config->ovr_pxlorder << MCDE_EXT_BEPO_SHIFT) & MCDE_EXT_BEPO_MASK)
        );

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BEBO_MASK) |
            (((t_uint32) config->endianity << MCDE_EXT_BEBO_SHIFT) & MCDE_EXT_BEBO_MASK)
        );

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BGR_MASK) |
            (((t_uint32) config->rgb_format << MCDE_EXT_BGR_SHIFT) & MCDE_EXT_BGR_MASK)
        );

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BPP_MASK) |
            (((t_uint32) config->bpp << MCDE_EXT_BPP_SHIFT) & MCDE_EXT_BPP_MASK)
        );

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_PRI_OVR_MASK) |
            (((t_uint32) config->ovr_id << MCDE_EXT_PRI_OVR_SHIFT) & MCDE_EXT_PRI_OVR_MASK)
        );

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BUFFER_NUM_MASK) |
            (((t_uint32) config->buf_num << MCDE_EXT_BUFFER_NUM_SHIFT) & MCDE_EXT_BUFFER_NUM_MASK)
        );

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BUFFER_ID_MASK) |
            ((t_uint32) config->buf_id & MCDE_EXT_BUFFER_ID_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetExtSrcControl()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the control of            */
/*                  External Source                                         */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_ext_conf config : Control structure                 */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetExtSrcControl(IN t_mcde_ext_src src_id, IN t_mcde_ext_src_ctrl *control)
{
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    ext_src->mcde_extsrc_cr =
        (
            (ext_src->mcde_extsrc_cr &~MCDE_EXT_FORCEFSDIV_MASK) |
            (((t_uint32) control->fs_div << MCDE_EXT_FORCEFSDIV_SHIFT) & MCDE_EXT_FORCEFSDIV_MASK)
        );

    ext_src->mcde_extsrc_cr =
        (
            (ext_src->mcde_extsrc_cr &~MCDE_EXT_FSDISABLE_MASK) |
            (((t_uint32) control->fs_ctrl << MCDE_EXT_FSDISABLE_SHIFT) & MCDE_EXT_FSDISABLE_MASK)
        );

    ext_src->mcde_extsrc_cr =
        (
            (ext_src->mcde_extsrc_cr &~MCDE_EXT_OVR_CTRL_MASK) |
            (((t_uint32) control->ovr_ctrl << MCDE_EXT_OVR_CTRL_SHIFT) & MCDE_EXT_OVR_CTRL_MASK)
        );

    ext_src->mcde_extsrc_cr =
        (
            (ext_src->mcde_extsrc_cr &~MCDE_EXT_BUF_MODE_MASK) |
            ((t_uint32) control->sel_mode & MCDE_EXT_BUF_MODE_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SelectCurrentBuffer()								*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to select the buffer id for            */
/*                  software bypass                                         */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_buffer_id buffer_id : buffer_id enum                */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SelectCurrentBuffer(IN t_mcde_ext_src src_id, IN t_mcde_buffer_id buffer_id)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    if ( buffer_id > MCDE_BUFFER_ID_2)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BUFFER_ID_MASK) |
            ((t_uint32) buffer_id & MCDE_EXT_BUFFER_ID_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_SetNumBufferUsed()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to select the number of buffers        */
/*                  used by external source                                 */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_num_buffer_used buffer_num : number of buffers      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetNumBufferUsed(IN t_mcde_ext_src src_id, IN t_mcde_num_buffer_used buffer_num)
{
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BUFFER_NUM_MASK) |
            (((t_uint32) buffer_num << MCDE_EXT_BUFFER_NUM_SHIFT) & MCDE_EXT_BUFFER_NUM_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetPriOverlay()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the primary overlay for         */
/*                  external source                                         */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_overlay_id ovr_id : overlay id                      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetPriOverlay(IN t_mcde_ext_src src_id, IN t_mcde_overlay_id ovr_id)
{
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_PRI_OVR_MASK) |
            (((t_uint32) ovr_id << MCDE_EXT_PRI_OVR_SHIFT) & MCDE_EXT_PRI_OVR_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetExtSrcBpp()								        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the bits per pixel of the data  */
/*                                                                          */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_bpp_ctrl bpp : bpp enum                             */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetExtSrcBpp(IN t_mcde_ext_src src_id, IN t_mcde_bpp_ctrl bpp)
{
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BPP_MASK) |
            (((t_uint32) bpp << MCDE_EXT_BPP_SHIFT) & MCDE_EXT_BPP_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetRGBFormat()								        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the RGB or BGR format of data   */
/*                                                                          */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_rgb_format_sel rgb_format : RGB format/BGR format   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetRGBFormat(IN t_mcde_ext_src src_id, IN t_mcde_rgb_format_sel rgb_format)
{
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    ext_src->mcde_extsrc_conf =
        (
            (ext_src->mcde_extsrc_conf &~MCDE_EXT_BGR_MASK) |
            (((t_uint32) rgb_format << MCDE_EXT_BGR_SHIFT) & MCDE_EXT_BGR_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetBufferSelMode()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the mode of selection for the   */
/*                  current buffer used for external source                 */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_buffer_sel_mode mode : buffer slection mode         */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetBufferSelMode(IN t_mcde_ext_src src_id, IN t_mcde_buffer_sel_mode mode)
{
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    ext_src->mcde_extsrc_cr =
        (
            (ext_src->mcde_extsrc_cr &~MCDE_EXT_BUF_MODE_MASK) |
            ((t_uint32) mode & MCDE_EXT_BUF_MODE_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetBufferReleaseMode()							    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the release mode of the         */
/*                  current buffer used for external source                 */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_multi_ovr_ctrl ovr_ctrl : Release mode              */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetBufferReleaseMode(IN t_mcde_ext_src src_id, IN t_mcde_multi_ovr_ctrl ovr_ctrl)
{
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    ext_src->mcde_extsrc_cr =
        (
            (ext_src->mcde_extsrc_cr &~MCDE_EXT_OVR_CTRL_MASK) |
            (((t_uint32) ovr_ctrl << MCDE_EXT_OVR_CTRL_SHIFT) & MCDE_EXT_OVR_CTRL_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_GetBufferBaseAddress()								*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the base address of the buffer. */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_buffer_id buffer_id : Buffer id whose address is    */
/*                                              to be configured            */
/*               t_uint32 *address   : pointer to Address of the buffer     */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetBufferBaseAddress
(
    IN t_mcde_ext_src   src_id,
    IN t_mcde_buffer_id buffer_id,
    OUT t_uint32        *p_addr
)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    if ((NULL == p_addr) || (MCDE_BUFFER_ID_RESERVED <= buffer_id))
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    switch (buffer_id)
    {
        case MCDE_BUFFER_ID_0:
            *p_addr = (t_uint32) ((ext_src->mcde_extsrc_a0 & MCDE_EXT_BUFFER_MASK) >> MCDE_EXT_BUFFER_SHIFT);

            break;

        case MCDE_BUFFER_ID_1:
            *p_addr = (t_uint32) ((ext_src->mcde_extsrc_a1 & MCDE_EXT_BUFFER_MASK) >> MCDE_EXT_BUFFER_SHIFT);

            break;

        case MCDE_BUFFER_ID_2:
            *p_addr = (t_uint32) ((ext_src->mcde_extsrc_a2 & MCDE_EXT_BUFFER_MASK) >> MCDE_EXT_BUFFER_SHIFT);

            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_GetExtSrcConfig()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the External Source            */
/*                  Color Format,Endianity and buffer configuration         */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_ext_conf *config : Configuration structure pointer  */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetExtSrcConfig(IN t_mcde_ext_src src_id, OUT t_mcde_ext_conf *p_config)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    if (NULL == p_config)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_config->ovr_pxlorder = (t_mcde_pixel_order_in_byte) ((ext_src->mcde_extsrc_conf & MCDE_EXT_BEPO_MASK) >> MCDE_EXT_BEPO_SHIFT);

    p_config->endianity = (t_mcde_byte_endianity) ((ext_src->mcde_extsrc_conf & MCDE_EXT_BEBO_MASK) >> MCDE_EXT_BEBO_SHIFT);

    p_config->rgb_format = (t_mcde_rgb_format_sel) ((ext_src->mcde_extsrc_conf & MCDE_EXT_BGR_MASK) >> MCDE_EXT_BGR_SHIFT);

    p_config->bpp = (t_mcde_bpp_ctrl) ((ext_src->mcde_extsrc_conf & MCDE_EXT_BPP_MASK) >> MCDE_EXT_BPP_SHIFT);

    p_config->ovr_id = (t_mcde_overlay_id) ((ext_src->mcde_extsrc_conf & MCDE_EXT_PRI_OVR_MASK) >> MCDE_EXT_PRI_OVR_SHIFT);

    p_config->buf_num = (t_mcde_num_buffer_used) ((ext_src->mcde_extsrc_conf & MCDE_EXT_BUFFER_NUM_MASK) >> MCDE_EXT_BUFFER_NUM_SHIFT);

    p_config->buf_id = (t_mcde_buffer_id) (ext_src->mcde_extsrc_conf & MCDE_EXT_BUFFER_ID_MASK);

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_GetExtSrcControl()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the control information of     */
/*                  External Source                                         */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ext_src src_id: External source id number to be     */
/*                                      configured                          */
/*               t_mcde_ext_src_ctrl *control : Control structure pointer   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetExtSrcControl(IN t_mcde_ext_src src_id, OUT t_mcde_ext_src_ctrl *p_control)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ext_src_reg  *ext_src;

    ext_src = (t_mcde_ext_src_reg *) &(g_mcde_system_context.p_mcde_register->ext_src[src_id]);

    DBGENTER0();

    if (NULL == p_control)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_control->fs_div = (t_mcde_fs_div) ((ext_src->mcde_extsrc_cr & MCDE_EXT_FORCEFSDIV_MASK) >> MCDE_EXT_FORCEFSDIV_SHIFT);

    p_control->fs_ctrl = (t_mcde_fs_ctrl) ((ext_src->mcde_extsrc_cr & MCDE_EXT_FSDISABLE_MASK) >> MCDE_EXT_FSDISABLE_SHIFT);

    p_control->ovr_ctrl = (t_mcde_multi_ovr_ctrl) ((ext_src->mcde_extsrc_cr & MCDE_EXT_OVR_CTRL_MASK) >> MCDE_EXT_OVR_CTRL_SHIFT);

    p_control->sel_mode = (t_mcde_buffer_sel_mode) (ext_src->mcde_extsrc_cr &~MCDE_EXT_BUF_MODE_MASK);

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOverlayCtrl()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the control of            */
/*                  the overlays                                            */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_control ovr_cr : Control structure              */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOverlayCtrl(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_control *ovr_cr)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_OVLEN_MASK) |
            ((t_uint32) ovr_cr->ovr_state & MCDE_OVR_OVLEN_MASK)
        );

    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_COLCTRL_MASK) |
            (((t_uint32) ovr_cr->col_ctrl << MCDE_OVR_COLCTRL_SHIFT) & MCDE_OVR_COLCTRL_MASK)
        );

    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_CKEYEN_MASK) |
            (((t_uint32) ovr_cr->color_key << MCDE_OVR_CKEYEN_SHIFT) & MCDE_OVR_CKEYEN_MASK)
        );

    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_STBPRIO_MASK) |
            (((t_uint32) ovr_cr->priority << MCDE_OVR_STBPRIO_SHIFT) & MCDE_OVR_STBPRIO_MASK)
        );

    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_BURSTSZ_MASK) |
            (((t_uint32) ovr_cr->burst_req << MCDE_OVR_BURSTSZ_SHIFT) & MCDE_OVR_BURSTSZ_MASK)
        );

    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_MAXREQ_MASK) |
            (((t_uint32) ovr_cr->outstnd_req << MCDE_OVR_MAXREQ_SHIFT) & MCDE_OVR_MAXREQ_MASK)
        );

    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_ROTBURSTSIZE_MASK) |
            (((t_uint32) ovr_cr->rot_burst_req << MCDE_OVR_ROTBURSTSIZE_SHIFT) & MCDE_OVR_ROTBURSTSIZE_MASK)
        );

    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_ALPHAPMEN_MASK) |
            (((t_uint32) ovr_cr->alpha << MCDE_OVR_ALPHAPMEN_SHIFT) & MCDE_OVR_ALPHAPMEN_MASK)
        );
        
    #ifdef ST_8500ED
    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_PALCTRL_MASK) |
            (((t_uint32) ovr_cr->pal_control << MCDE_OVR_PALCTRL_SHIFT) & MCDE_OVR_PALCTRL_MASK)
        );
        
    ovr_config->mcde_ovl_cr =
        (
            (ovr_config->mcde_ovl_cr &~MCDE_OVR_CLIPEN_MASK) |
            (((t_uint32) ovr_cr->clip << MCDE_OVR_CLIPEN_SHIFT) & MCDE_OVR_CLIPEN_MASK)
        );
    
    #endif


    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOverlayConfig()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the lines per pixel,pixels*/
/*                   per line and associate the overlay to external source  */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_config ovr_conf : Config structure              */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOverlayConfig(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_config *ovr_conf)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_conf =
        (
            (ovr_config->mcde_ovl_conf &~MCDE_OVR_LPF_MASK) |
            (((t_uint32) ovr_conf->line_per_frame << MCDE_OVR_LPF_SHIFT) & MCDE_OVR_LPF_MASK)
        );

    ovr_config->mcde_ovl_conf =
        (
            (ovr_config->mcde_ovl_conf &~MCDE_EXT_SRCID_MASK) |
            (((t_uint32) ovr_conf->src_id << MCDE_EXT_SRCID_SHIFT) & MCDE_EXT_SRCID_MASK)
        );

    ovr_config->mcde_ovl_conf =
        (
            (ovr_config->mcde_ovl_conf &~MCDE_OVR_PPL_MASK) |
            ((t_uint32) ovr_conf->ovr_ppl & MCDE_OVR_PPL_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOverlayConf2()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the watermark level,      */
/*                   overlay opaque settings,alpha value,blend parameter and*/
/*                   pixel offset in 64 bit word                            */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_config ovr_conf : Configuration structure       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOverlayConf2(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_conf2 *ovr_conf2)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_conf2 =
        (
            (ovr_config->mcde_ovl_conf2 &~MCDE_WATERMARK_MASK) |
            (((t_uint32) ovr_conf2->watermark_level << MCDE_WATERMARK_SHIFT) & MCDE_WATERMARK_MASK)
        );

    ovr_config->mcde_ovl_conf2 =
        (
            (ovr_config->mcde_ovl_conf2 &~MCDE_OVR_OPQ_MASK) |
            (((t_uint32) ovr_conf2->ovr_opaq << MCDE_OVR_OPQ_SHIFT) & MCDE_OVR_OPQ_MASK)
        );

    ovr_config->mcde_ovl_conf2 =
        (
            (ovr_config->mcde_ovl_conf2 &~MCDE_ALPHAVALUE_MASK) |
            (((t_uint32) ovr_conf2->alpha_value << MCDE_ALPHAVALUE_SHIFT) & MCDE_ALPHAVALUE_MASK)
        );

    ovr_config->mcde_ovl_conf2 =
        (
            (ovr_config->mcde_ovl_conf2 &~MCDE_PIXOFF_MASK) |
            (((t_uint32) ovr_conf2->pixoff << MCDE_PIXOFF_SHIFT) & MCDE_PIXOFF_MASK)
        );

    ovr_config->mcde_ovl_conf2 =
        (
            (ovr_config->mcde_ovl_conf2 &~MCDE_OVR_BLEND_MASK) |
            ((t_uint32) ovr_conf2->ovr_blend & MCDE_OVR_BLEND_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOvrLineIncrement()								*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the line increment        */
/*                  parameter for the overlay                               */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_uint32 ovr_ljinc : line increment                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOvrLineIncrement(IN t_mcde_overlay_id overlay, IN t_uint32 ovr_ljinc)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_ljinc = ((ovr_ljinc << MCDE_LINEINCREMENT_SHIFT) & MCDE_LINEINCREMENT_MASK);

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOvrTopMargin()							    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the top margin            */
/*                  parameter for the overlay compared to input buffer      */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_uint32 ovr_topmargin : top margin                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOvrTopMargin(IN t_mcde_overlay_id overlay, IN t_uint32 ovr_topmargin)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_crop =
        (
            (ovr_config->mcde_ovl_crop &~MCDE_YCLIP_MASK) |
            ((ovr_topmargin << MCDE_YCLIP_SHIFT) & MCDE_YCLIP_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOvrLeftMargin()							    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the left margin           */
/*                  parameter for the overlay compared to input buffer      */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_uint32 ovr_leftmargin : left margin                      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOvrLeftMargin(IN t_mcde_overlay_id overlay, IN t_uint16 ovr_leftmargin)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_crop =
        (
            (ovr_config->mcde_ovl_crop &~MCDE_XCLIP_MASK) |
            (((t_uint32) ovr_leftmargin << MCDE_XCLIP_SHIFT) & MCDE_XCLIP_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOvrComposition()							  	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the channle id,x and y    */
/*                  position and z level of the overlay                     */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_comp ovr_comp : overlay composition structure   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOvrComposition(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_comp *ovr_comp)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_comp =
        (
            (ovr_config->mcde_ovl_comp &~MCDE_OVR_ZLEVEL_MASK) |
            (((t_uint32) ovr_comp->ovr_zlevel << MCDE_OVR_ZLEVEL_SHIFT) & MCDE_OVR_ZLEVEL_MASK)
        );

    ovr_config->mcde_ovl_comp =
        (
            (ovr_config->mcde_ovl_comp &~MCDE_OVR_YPOS_MASK) |
            (((t_uint32) ovr_comp->ovr_ypos << MCDE_OVR_YPOS_SHIFT) & MCDE_OVR_YPOS_MASK)
        );

    ovr_config->mcde_ovl_comp =
        (
            (ovr_config->mcde_ovl_comp &~MCDE_OVR_CHID_MASK) |
            (((t_uint32) ovr_comp->ch_id << MCDE_OVR_CHID_SHIFT) & MCDE_OVR_CHID_MASK)
        );

    ovr_config->mcde_ovl_comp =
        (
            (ovr_config->mcde_ovl_comp &~MCDE_OVR_XPOS_MASK) |
            ((t_uint32) ovr_comp->ovr_xpos & MCDE_OVR_XPOS_MASK)
        );

    DBGEXIT0(MCDE_OK);
}
#if 0
/****************************************************************************/
/* NAME			:	MCDE_SetOvrClip()							        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the overlay clipping      */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_clip ovr_clip : overlay clipping structure      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOvrClip(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_clip *ovr_clip)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_brclip =
        (
            (ovr_config->mcde_ovl_brclip &~MCDE_YBRCOOR_MASK) |
            (((t_uint32) ovr_clip->ybrcoor << MCDE_YBRCOOR_SHIFT) & MCDE_YBRCOOR_MASK)
        );

    ovr_config->mcde_ovl_tlclip =
        (
            (ovr_config->mcde_ovl_tlclip &~MCDE_YBRCOOR_MASK) |
            (((t_uint32) ovr_clip->ytlcoor << MCDE_YBRCOOR_SHIFT) & MCDE_YBRCOOR_MASK)
        );

    ovr_config->mcde_ovl_brclip =
        (
            (ovr_config->mcde_ovl_brclip &~MCDE_XBRCOOR_MASK) |
            ((t_uint32) ovr_clip->xbrcoor & MCDE_XBRCOOR_MASK)
        );

    ovr_config->mcde_ovl_tlclip =
        (
            (ovr_config->mcde_ovl_tlclip &~MCDE_XBRCOOR_MASK) |
            ((t_uint32) ovr_clip->xtlcoor & MCDE_XBRCOOR_MASK)
        );

    DBGEXIT0(MCDE_OK);
}
#endif
/****************************************************************************/
/* NAME			:	MCDE_ReadOvrStatus()							    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the overlay status             */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*     InOut    :t_mcde_ovr_status *ovr_status :pointer to status structure */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_ReadOvrStatus(IN t_mcde_overlay_id overlay, OUT t_mcde_ovr_status *p_ovr_status)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    p_ovr_status->ovrb_status = (t_mcde_ovr_blocked_status) ((ovr_config->mcde_ovl_cr & MCDE_OVR_BLOCKED_MASK) >> MCDE_OVR_BLOCKED_SHIFT);

    p_ovr_status->ovr_read = (t_mcde_ovr_read_status) ((ovr_config->mcde_ovl_cr & MCDE_OVR_READ_MASK) >> MCDE_OVR_READ_SHIFT);

    p_ovr_status->ovr_status = (t_mcde_ovr_fetch_status) ((ovr_config->mcde_ovl_cr & MCDE_OVR_FETCH_MASK) >> MCDE_OVR_FETCH_SHIFT);

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_OverlayEnable()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to enable the overlay                  */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          enabled                         */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_OverlayEnable(IN t_mcde_overlay_id overlay)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_cr = ((ovr_config->mcde_ovl_cr &~MCDE_OVR_OVLEN_MASK) | (1 & MCDE_OVR_OVLEN_MASK));

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_OverlayDisable()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to disable the overlay                 */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          disabled                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_OverlayDisable(IN t_mcde_overlay_id overlay)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_cr = ((ovr_config->mcde_ovl_cr &~MCDE_OVR_OVLEN_MASK) | (0 & MCDE_OVR_OVLEN_MASK));

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOvrPPL()							  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the pixels per line for the     */
/*				    overlay                                           		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_uint16 ppl:pixels per line                               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOvrPPL(IN t_mcde_overlay_id overlay, IN t_uint16 ppl)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_conf =
        (
            (ovr_config->mcde_ovl_conf &~MCDE_OVR_PPL_MASK) |
            ((t_uint32) ppl & MCDE_OVR_PPL_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOvrLPF()							  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the lines per frame for the     */
/*				    overlay                                           		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_uint16 lpf: lines per frame                              */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOvrLPF(IN t_mcde_overlay_id overlay, IN t_uint16 lpf)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_conf =
        (
            (ovr_config->mcde_ovl_conf &~MCDE_OVR_LPF_MASK) |
            (((t_uint32) lpf << MCDE_OVR_LPF_SHIFT) & MCDE_OVR_LPF_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOvrXYPOS()							  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the x and y positions for the   */
/*				    overlay                                           		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_xy xy_pos: x and y position structure           */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOvrXYPOS(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_xy *xy_pos)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_comp =
        (
            (ovr_config->mcde_ovl_comp &~MCDE_OVR_YPOS_MASK) |
            (((t_uint32) xy_pos->ovr_ypos << MCDE_OVR_YPOS_SHIFT) & MCDE_OVR_YPOS_MASK)
        );

    ovr_config->mcde_ovl_comp =
        (
            (ovr_config->mcde_ovl_comp &~MCDE_OVR_XPOS_MASK) |
            ((t_uint32) xy_pos->ovr_xpos & MCDE_OVR_XPOS_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOvrZPOS()							  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the z position for the          */
/*				    overlay                                           		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_uint8 z_pos: z position                                  */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetOvrZPOS(IN t_mcde_overlay_id overlay, IN t_uint8 z_pos)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_comp =
        (
            (ovr_config->mcde_ovl_comp &~MCDE_OVR_ZLEVEL_MASK) |
            (((t_uint32) z_pos << MCDE_OVR_ZLEVEL_SHIFT) & MCDE_OVR_ZLEVEL_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_OvrAssociateChannel()							  	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to associate the overlay to the        */
/*				    channel(A/B/C)                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ch_id ch_id:Channel to which overlay is associated  */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_OvrAssociateChannel(IN t_mcde_overlay_id overlay, IN t_mcde_ch_id ch_id)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_comp =
        (
            (ovr_config->mcde_ovl_comp &~MCDE_OVR_CHID_MASK) |
            (((t_uint32) ch_id << MCDE_OVR_CHID_SHIFT) & MCDE_OVR_CHID_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_OvrAssociateExtSrc()							  	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to associate the overlay to the        */
/*				    external source                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ext_src ext_src:Src to which overlay is associated  */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_OvrAssociateExtSrc(IN t_mcde_overlay_id overlay, IN t_mcde_ext_src ext_src)
{
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    ovr_config->mcde_ovl_conf =
        (
            (ovr_config->mcde_ovl_conf &~MCDE_EXT_SRCID_MASK) |
            (((t_uint32) ext_src << MCDE_EXT_SRCID_SHIFT) & MCDE_EXT_SRCID_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_GetOverlayCtrl()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the control info of            */
/*                  the overlays                                            */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_control *ovr_cr : Control structure pointer     */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetOverlayCtrl(IN t_mcde_overlay_id overlay, OUT t_mcde_ovr_control *p_ovr_cr)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    if (NULL == p_ovr_cr)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_ovr_cr->ovr_state = (t_mcde_overlay_ctrl) (ovr_config->mcde_ovl_cr & MCDE_OVR_OVLEN_MASK);

    p_ovr_cr->col_ctrl = (t_mcde_col_conv_ctrl) ((ovr_config->mcde_ovl_cr & MCDE_OVR_COLCTRL_MASK) >> MCDE_OVR_COLCTRL_SHIFT);

    p_ovr_cr->color_key = (t_mcde_color_key_ctrl) ((ovr_config->mcde_ovl_cr & MCDE_OVR_CKEYEN_MASK) >> MCDE_OVR_CKEYEN_SHIFT);

    p_ovr_cr->priority = (t_uint8) ((ovr_config->mcde_ovl_cr & MCDE_OVR_STBPRIO_MASK) >> MCDE_OVR_STBPRIO_SHIFT);

    p_ovr_cr->burst_req = (t_mcde_burst_req) ((ovr_config->mcde_ovl_cr & MCDE_OVR_BURSTSZ_MASK) >> MCDE_OVR_BURSTSZ_SHIFT);

    p_ovr_cr->outstnd_req = (t_mcde_outsnd_req) ((ovr_config->mcde_ovl_cr & MCDE_OVR_MAXREQ_MASK) >> MCDE_OVR_MAXREQ_SHIFT);

    p_ovr_cr->rot_burst_req = (t_mcde_rotate_req) ((ovr_config->mcde_ovl_cr & MCDE_OVR_ROTBURSTSIZE_MASK) >> MCDE_OVR_ROTBURSTSIZE_SHIFT);
    
    #ifdef ST_8500ED
    
    p_ovr_cr->pal_control = (t_mcde_pal_ctrl) ((ovr_config->mcde_ovl_cr & MCDE_OVR_PALCTRL_MASK) >> MCDE_OVR_PALCTRL_SHIFT);
	
	#endif

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_GetOverlayConfig()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the lines per pixel,pixels     */
/*                   per line and associate the overlay to external source  */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_config *ovr_conf : Config structure pointer     */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetOverlayConfig(IN t_mcde_overlay_id overlay, OUT t_mcde_ovr_config *p_ovr_conf)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    if (NULL == p_ovr_conf)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_ovr_conf->line_per_frame = (t_uint32) ((ovr_config->mcde_ovl_conf & MCDE_OVR_LPF_MASK) >> MCDE_OVR_LPF_SHIFT);

    p_ovr_conf->src_id = (t_mcde_ext_src) ((ovr_config->mcde_ovl_conf & MCDE_EXT_SRCID_MASK) >> MCDE_EXT_SRCID_SHIFT);

    p_ovr_conf->ovr_ppl = (t_uint16) (ovr_config->mcde_ovl_conf & MCDE_OVR_PPL_MASK);

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_GetOverlayConf2()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the watermark level,           */
/*                   overlay opaque settings,alpha value,blend parameter and*/
/*                   pixel offset in 64 bit word                            */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_conf2 *ovr_conf2 : Configuration structure      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetOverlayConf2(IN t_mcde_overlay_id overlay, OUT t_mcde_ovr_conf2 *p_ovr_conf2)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    if (NULL == p_ovr_conf2)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_ovr_conf2->watermark_level = (t_uint32) ((ovr_config->mcde_ovl_conf2 & MCDE_WATERMARK_MASK) >> MCDE_WATERMARK_SHIFT);

    p_ovr_conf2->ovr_opaq = (t_mcde_ovr_opq_ctrl) ((ovr_config->mcde_ovl_conf2 & MCDE_OVR_OPQ_MASK) >> MCDE_OVR_OPQ_SHIFT);

    p_ovr_conf2->alpha_value = (t_uint8) ((ovr_config->mcde_ovl_conf2 & MCDE_ALPHAVALUE_MASK) >> MCDE_ALPHAVALUE_SHIFT);

    p_ovr_conf2->pixoff = (t_uint8) ((ovr_config->mcde_ovl_conf2 & MCDE_PIXOFF_MASK) >> MCDE_PIXOFF_SHIFT);

    p_ovr_conf2->ovr_blend = (t_mcde_blend_ctrl) (ovr_config->mcde_ovl_conf2 & MCDE_OVR_BLEND_MASK);

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_GetOvrLineIncrement()								*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the line increment             */
/*                  parameter for the overlay                               */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_uint32 *ovr_ljinc : line increment pointer               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetOvrLineIncrement(IN t_mcde_overlay_id overlay, OUT t_uint32 *p_ovr_ljinc)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    if (NULL == p_ovr_ljinc)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    *p_ovr_ljinc = (t_uint32) ((ovr_config->mcde_ovl_ljinc & MCDE_LINEINCREMENT_MASK) >> MCDE_LINEINCREMENT_SHIFT);

    DBGEXIT0(error);

    return(error);
}

#if 0

/****************************************************************************/
/* NAME			:	MCDE_GetOvrTopMargin()							    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the top margin                 */
/*                  parameter for the overlay compared to input buffer      */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_uint32 *ovr_topmargin : top margin pointer               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetOvrTopMargin(IN t_mcde_overlay_id overlay, OUT t_uint32 *ovr_topmargin)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    if (NULL == ovr_topmargin)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    *ovr_topmargin = ((ovr_config->mcde_ovl_yclip & MCDE_YCLIP_MASK) >> MCDE_YCLIP_SHIFT);

    DBGEXIT0(error);

    return(error);
}
#endif

/****************************************************************************/
/* NAME			:	MCDE_GetOvrComposition()							  	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the channle id,x and y         */
/*                  position and z level of the overlay                     */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_comp *ovr_comp : overlay composition structure  */
/*     InOut    :None                        pointer                        */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetOvrComposition(IN t_mcde_overlay_id overlay, OUT t_mcde_ovr_comp *p_ovr_comp)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ovl_reg  *ovr_config;

    ovr_config = (t_mcde_ovl_reg *) &(g_mcde_system_context.p_mcde_register->ovr[overlay]);

    DBGENTER0();

    if (NULL == p_ovr_comp)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_ovr_comp->ovr_zlevel = (t_uint8) ((ovr_config->mcde_ovl_comp & MCDE_OVR_ZLEVEL_MASK) >> MCDE_OVR_ZLEVEL_SHIFT);

    p_ovr_comp->ovr_ypos = (t_uint16) ((ovr_config->mcde_ovl_comp & MCDE_OVR_YPOS_MASK) >> MCDE_OVR_YPOS_SHIFT);

    p_ovr_comp->ch_id = (t_mcde_ch_id) ((ovr_config->mcde_ovl_comp & MCDE_OVR_CHID_MASK) >> MCDE_OVR_CHID_SHIFT);

    p_ovr_comp->ovr_xpos = (t_uint16) (ovr_config->mcde_ovl_comp & MCDE_OVR_XPOS_MASK);

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_SetChXConfig()							  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the channle X lines per   */
/*                  frame and pixels per line for the channel X             */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_mcde_chx_config config :channel x configuration structure*/
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetChXConfig(IN t_mcde_ch_id channel, IN t_mcde_chx_config *config)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_ch_conf =
        (
            (ch_syncreg->mcde_ch_conf &~MCDE_CHXLPF_MASK) |
            (((t_uint32) config->chx_lpf << MCDE_CHXLPF_SHIFT) & MCDE_CHXLPF_MASK)
        );

    ch_syncreg->mcde_ch_conf =
        (
            (ch_syncreg->mcde_ch_conf &~MCDE_CHXPPL_MASK) |
            ((t_uint32) config->chx_ppl & MCDE_CHXPPL_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

#if 0

/****************************************************************************/
/* NAME			:	MCDE_SetChSyncConf()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the timings                     */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_mcde_chsyncconf conf: structure containing timing info   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetChSyncConf(IN t_mcde_ch_id channel, IN t_mcde_chsyncconf *conf)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_con =
        (
            (ch_syncreg->mcde_chsyn_con &~MCDE_SWINTVCNT_MASK) |
            (((t_uint32) conf->swframe_intrdelay << MCDE_SWINTVCNT_SHIFT) & MCDE_SWINTVCNT_MASK)
        );

    ch_syncreg->mcde_chsyn_con =
        (
            (ch_syncreg->mcde_chsyn_con &~MCDE_SWINTVEVENT_MASK) |
            (((t_uint32) conf->frame_intrevent << MCDE_SWINTVEVENT_SHIFT) & MCDE_SWINTVEVENT_MASK)
        );

    ch_syncreg->mcde_chsyn_con =
        (
            (ch_syncreg->mcde_chsyn_con &~MCDE_HWREQVCNT_MASK) |
            (((t_uint32) conf->autoframe_delay << MCDE_HWREQVCNT_SHIFT) & MCDE_HWREQVCNT_MASK)
        );

    ch_syncreg->mcde_chsyn_con =
        (
            (ch_syncreg->mcde_chsyn_con &~MCDE_HWREQVEVENT_MASK) |
            ((t_uint32) conf->frame_synchroevnt & MCDE_HWREQVEVENT_MASK)
        );

    DBGEXIT0(error);

    return(error);
}
#endif
/****************************************************************************/
/* NAME			:	MCDE_GetChannelState()						     	  	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the channle state              */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_overlay_id overlay: Overlay id number to be         */
/*                                          configured                      */
/*               t_mcde_ovr_comp *ovr_comp : overlay composition structure  */
/*     InOut    :None                        pointer                        */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_GetChannelState(IN t_mcde_ch_id channel, OUT t_mcde_chnl_state *p_chnl_state)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    p_chnl_state->abort_state = (t_mcde_chnl_abort_state) ((ch_syncreg->mcde_ch_conf & MCDE_CHX_ABORT_MASK) >> MCDE_CHX_ABORT_SHIFT);

    p_chnl_state->read_state = (t_mcde_chnl_read_status) (ch_syncreg->mcde_ch_conf & MCDE_CHX_READ_MASK);

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetChSyncSource()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the synchro generation source   */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_mcde_chsyncmod sync_mod: structure containing synchro    */
/*                                            source info                   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetChSyncSource(IN t_mcde_ch_id channel, IN t_mcde_chsyncmod *sync_mod)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_mod =
        (
            (ch_syncreg->mcde_chsyn_mod &~MCDE_OUTINTERFACE_MASK) |
            (((t_uint32) sync_mod->out_synch_interface << MCDE_OUTINTERFACE_SHIFT) & MCDE_OUTINTERFACE_MASK)
        );

    ch_syncreg->mcde_chsyn_mod =
        (
            (ch_syncreg->mcde_chsyn_mod &~MCDE_SRCSYNCH_MASK) |
            ((t_uint32) sync_mod->ch_synch_src & MCDE_SRCSYNCH_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetSwSync()							  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to send the software trigger for frame */
/*                  geneartion for the channel X                            */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_mcde_sw_trigger sw_trig : enable/disable                 */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetSwSync(IN t_mcde_ch_id channel, IN t_mcde_sw_trigger sw_trig)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_sw =
        (
            (ch_syncreg->mcde_chsyn_sw &~MCDE_SW_TRIG_MASK) |
            ((t_uint32) sw_trig & MCDE_SW_TRIG_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetChBckGndCol()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the background color      */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_mcde_ch_bckgrnd_col color : structure containing RGB val */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_SetChBckGndCol(IN t_mcde_ch_id channel, IN t_mcde_ch_bckgrnd_col *color)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_bck =
        (
            (ch_syncreg->mcde_chsyn_bck &~MCDE_REDCOLOR_MASK) |
            ((color->red << MCDE_REDCOLOR_SHIFT) & MCDE_REDCOLOR_MASK)
        );

    ch_syncreg->mcde_chsyn_bck =
        (
            (ch_syncreg->mcde_chsyn_bck &~MCDE_GREENCOLOR_MASK) |
            ((color->green << MCDE_GREENCOLOR_SHIFT) & MCDE_GREENCOLOR_MASK)
        );

    ch_syncreg->mcde_chsyn_bck =
        (
            (ch_syncreg->mcde_chsyn_bck &~MCDE_BLUECOLOR_MASK) |
            (color->blue & MCDE_BLUECOLOR_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetChPriority()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the priority              */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_uint32 priority : priority value                         */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`								*/

/****************************************************************************/
PUBLIC void MCDE_SetChPriority(IN t_mcde_ch_id channel, IN t_uint32 priority)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_prio =
        (
            (ch_syncreg->mcde_chsyn_prio &~MCDE_CHPRIORITY_MASK) |
            (priority & MCDE_CHPRIORITY_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOutDeviceLPF()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the lines per frame       */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_uint16 lpf : lpf value                                   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`							*/

/****************************************************************************/
PUBLIC void MCDE_SetOutDeviceLPF(IN t_mcde_ch_id channel, IN t_uint16 lpf)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_ch_conf =
        (
            (ch_syncreg->mcde_ch_conf &~MCDE_CHXLPF_MASK) |
            ((lpf << MCDE_CHXLPF_SHIFT) & MCDE_CHXLPF_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOutDevicePPL()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the pixels per line       */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_uint16 ppl : ppl value                                   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`							*/

/****************************************************************************/
PUBLIC void MCDE_SetOutDevicePPL(IN t_mcde_ch_id channel, IN t_uint16 ppl)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_ch_conf = ((ch_syncreg->mcde_ch_conf &~MCDE_CHXPPL_MASK) | (ppl & MCDE_CHXLPF_MASK));

    DBGEXIT0(MCDE_OK);
}

#if 0

/****************************************************************************/
/* NAME			:	MCDE_SetSwFrameIntrDelay()							    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the delay to software frame*/
/*                  interrupt                                               */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_uint16 delay : delay value                               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`							*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetSwFrameIntrDelay(IN t_mcde_ch_id channel, IN t_uint16 delay)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_con =
        (
            (ch_syncreg->mcde_chsyn_con &~MCDE_SWINTVCNT_MASK) |
            ((delay << MCDE_SWINTVCNT_SHIFT) & MCDE_SWINTVCNT_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_SetFrameIntrGenEvent()							    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the Frame Interrupt       */
/*                  generation events                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_mcde_frame_events event : event type enum                */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`							*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetFrameIntrGenEvent(IN t_mcde_ch_id channel, IN t_mcde_frame_events event)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_con =
        (
            (ch_syncreg->mcde_chsyn_con &~MCDE_SWINTVEVENT_MASK) |
            (((t_uint32) event << MCDE_SWINTVEVENT_SHIFT) & MCDE_SWINTVEVENT_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_SetFrameSyncDelay()							    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the time between frame    */
/*                  synchro                                                 */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_uint16 delay : delay value                               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`							*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetFrameSyncDelay(IN t_mcde_ch_id channel, IN t_uint16 delay)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_con =
        (
            (ch_syncreg->mcde_chsyn_con &~MCDE_HWREQVCNT_MASK) |
            ((delay << MCDE_HWREQVCNT_SHIFT) & MCDE_HWREQVCNT_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_SetFrameSyncGenEvent()							    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the Frame Synchro         */
/*                  generation events                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_mcde_frame_events event : event type                     */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`							*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetFrameSyncGenEvent(IN t_mcde_ch_id channel, IN t_mcde_frame_events event)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_con =
        (
            (ch_syncreg->mcde_chsyn_con &~MCDE_HWREQVEVENT_MASK) |
            ((t_uint32) event & MCDE_HWREQVEVENT_MASK)
        );

    DBGEXIT0(error);

    return(error);
}
#endif

/****************************************************************************/
/* NAME			:	MCDE_SetChSyncSrc()							            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the Source of channel X   */
/*                  synchronization                                         */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_mcde_synchro_source sync_src : sync source               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`							*/

/****************************************************************************/
PUBLIC void MCDE_SetChSyncSrc(IN t_mcde_ch_id channel, IN t_mcde_synchro_source sync_src)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_mod =
        (
            (ch_syncreg->mcde_chsyn_mod &~MCDE_SRCSYNCH_MASK) |
            ((t_uint32) sync_src & MCDE_SRCSYNCH_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_SetOutSyncSrc()							        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the Output interface      */
/*                  synchronization source                                  */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*               t_mcde_synchro_out_interface sync : sync source            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`							*/

/****************************************************************************/
PUBLIC void MCDE_SetOutSyncSrc(IN t_mcde_ch_id channel, IN t_mcde_synchro_out_interface sync)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    ch_syncreg->mcde_chsyn_mod =
        (
            (ch_syncreg->mcde_chsyn_mod &~MCDE_OUTINTERFACE_MASK) |
            (((t_uint32) sync << MCDE_OUTINTERFACE_SHIFT) & MCDE_OUTINTERFACE_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_GetChXConfig()							  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the channle X lines per        */
/*                  frame and pixels per line for the channel X             */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          read(A/B/C)                     */
/*     InOut    :None                                                       */
/* 		OUT 	: t_mcde_chx_config config :channel x configuration structure*/
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_GetChXConfig(IN t_mcde_ch_id channel, OUT t_mcde_chx_config *p_config)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    p_config->chx_lpf = (t_uint16) ((ch_syncreg->mcde_ch_conf & MCDE_CHXLPF_MASK) >> MCDE_CHXLPF_SHIFT);

    p_config->chx_ppl = (t_uint16) (ch_syncreg->mcde_ch_conf & MCDE_CHXPPL_MASK);

    DBGEXIT0(MCDE_OK);
}

#if 0

/****************************************************************************/
/* NAME			:	MCDE_GetChSyncConf()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the timings                    */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          read(A/B/C)                     */
/*     InOut    :None                                                       */
/* 		OUT 	:t_mcde_chsyncconf *conf: structure containing timing info  */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetChSyncConf(IN t_mcde_ch_id channel, OUT t_mcde_chsyncconf *p_conf)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    p_conf->swframe_intrdelay = (t_uint16) ((ch_syncreg->mcde_chsyn_con & MCDE_SWINTVCNT_MASK) >> MCDE_SWINTVCNT_SHIFT);

    p_conf->frame_intrevent = (t_mcde_frame_events) ((ch_syncreg->mcde_chsyn_con & MCDE_SWINTVEVENT_MASK) >> MCDE_SWINTVEVENT_SHIFT);

    p_conf->autoframe_delay = (t_uint16) ((ch_syncreg->mcde_chsyn_con & MCDE_HWREQVCNT_MASK) >> MCDE_HWREQVCNT_SHIFT);

    p_conf->frame_synchroevnt = (t_mcde_frame_events) (ch_syncreg->mcde_chsyn_con & MCDE_HWREQVEVENT_MASK);

    DBGEXIT0(error);

    return(error);
}
#endif

/****************************************************************************/
/* NAME			:	MCDE_GetChSyncSource()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the synchro generation source  */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          configured(A/B/C)               */
/*     InOut    :None                                                       */
/* 		OUT 	:t_mcde_chsyncmod *sync_mod: structure containing synchro   */
/*                                          source info pointer             */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_GetChSyncSource(IN t_mcde_ch_id channel, OUT t_mcde_chsyncmod *p_sync_mod)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    p_sync_mod->out_synch_interface = (t_mcde_synchro_out_interface) ((ch_syncreg->mcde_chsyn_mod & MCDE_OUTINTERFACE_MASK) >> MCDE_OUTINTERFACE_SHIFT);

    p_sync_mod->ch_synch_src = (t_mcde_synchro_source) (ch_syncreg->mcde_chsyn_mod & MCDE_SRCSYNCH_MASK);

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_GetSwSync()							  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the software trigger for frame */
/*                  generation for the channel X                            */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          read(A/B/C)                     */
/*     InOut    :None                                                       */
/* 		OUT 	: t_mcde_sw_trigger sw_trig : enable/disable                */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetSwSync(IN t_mcde_ch_id channel, OUT t_mcde_sw_trigger *p_sw_trig)
{
    t_mcde_error        error = MCDE_OK;
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    if (NULL == p_sw_trig)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    *p_sw_trig = (t_mcde_sw_trigger) (ch_syncreg->mcde_chsyn_sw & MCDE_SW_TRIG_MASK);

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/* NAME			:	MCDE_GetChBckGndCol()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the background color           */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          read(A/B/C)                     */
/*     InOut    :None                                                       */
/* 		OUT 	:t_mcde_ch_bckgrnd_col *color : structure containing RGB val*/
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void MCDE_GetChBckGndCol(IN t_mcde_ch_id channel, OUT t_mcde_ch_bckgrnd_col *p_color)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    p_color->red = (t_uint8) ((ch_syncreg->mcde_chsyn_bck & MCDE_REDCOLOR_MASK) >> MCDE_REDCOLOR_SHIFT);

    p_color->green = (t_uint8) ((ch_syncreg->mcde_chsyn_bck & MCDE_GREENCOLOR_MASK) >> MCDE_GREENCOLOR_SHIFT);

    p_color->red = (t_uint8) (ch_syncreg->mcde_chsyn_bck & MCDE_BLUECOLOR_MASK);

    DBGEXIT0(MCDE_OK);
}

/****************************************************************************/
/* NAME			:	MCDE_GetChPriority()							  	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the priority                   */
/*                  for the channel X                                       */
/*				                                                    		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_mcde_ch_id channel: channel id number to be              */
/*                                          read(A/B/C)                     */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint32 *priority : priority value pointer          	    */
/*                                                                          */
/* RETURN		:t_mcde_error	: MCDE error code						   	*/
/*               MCDE_OK                                                    */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :							`							*/

/****************************************************************************/
PUBLIC void MCDE_GetChPriority(IN t_mcde_ch_id channel, OUT t_uint32 *p_priority)
{
    t_mcde_ch_synch_reg *ch_syncreg;

    ch_syncreg = (t_mcde_ch_synch_reg *) &(g_mcde_system_context.p_mcde_register->chnsynch[channel]);

    DBGENTER0();

    *p_priority = (t_uint32) (ch_syncreg->mcde_chsyn_prio & MCDE_CHPRIORITY_MASK);

    DBGEXIT0(MCDE_OK);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetChCConfig()							  	         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the channel C configuration*/
/*                  parameters                                               */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: ID of the panel, C0/C1           */
/*               t_mcde_chc_config config : config structure                 */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetChCConfig(IN t_mcde_chc_panel panel_id, IN t_mcde_chc_config *config)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_RES1_MASK) |
                    (((t_uint32) config->res_pol << MCDE_RES1_SHIFT) & MCDE_RES1_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_RD1_MASK) |
                    (((t_uint32) config->rd_pol << MCDE_RD1_SHIFT) & MCDE_RD1_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_WR1_MASK) |
                    (((t_uint32) config->wr_pol << MCDE_WR1_SHIFT) & MCDE_WR1_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_CD1_MASK) |
                    (((t_uint32) config->cd_pol << MCDE_CD1_SHIFT) & MCDE_CD1_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_CS1_MASK) |
                    (((t_uint32) config->cs_pol << MCDE_CS1_SHIFT) & MCDE_CS1_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_CS1EN_MASK) |
                    (((t_uint32) config->csen << MCDE_CS1EN_SHIFT) & MCDE_CS1EN_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_INBAND1_MASK) |
                    (((t_uint32) config->inband_mode << MCDE_INBAND1_SHIFT) & MCDE_INBAND1_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_BUSSIZE1_MASK) |
                    (((t_uint32) config->bus_size << MCDE_BUSSIZE1_SHIFT) & MCDE_BUSSIZE1_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_SYNCEN1_MASK) |
                    (((t_uint32) config->syncen << MCDE_SYNCEN1_SHIFT) & MCDE_SYNCEN1_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_WMLVL1_MASK) |
                    (((t_uint32) config->fifo_watermark << MCDE_WMLVL1_SHIFT) & MCDE_WMLVL1_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_C1EN_MASK) |
                    (((t_uint32) config->chcen << MCDE_C1EN_SHIFT) & MCDE_C1EN_MASK)
                );

            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_RES2_MASK) |
                    (((t_uint32) config->res_pol << MCDE_RES2_SHIFT) & MCDE_RES2_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_RD2_MASK) |
                    (((t_uint32) config->rd_pol << MCDE_RD2_SHIFT) & MCDE_RD2_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_WR2_MASK) |
                    (((t_uint32) config->wr_pol << MCDE_WR2_SHIFT) & MCDE_WR2_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_CD2_MASK) |
                    (((t_uint32) config->cd_pol << MCDE_CD2_SHIFT) & MCDE_CD2_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_CS2_MASK) |
                    (((t_uint32) config->cs_pol << MCDE_CS2_SHIFT) & MCDE_CS2_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_CS2EN_MASK) |
                    (((t_uint32) config->csen << MCDE_CS2EN_SHIFT) & MCDE_CS2EN_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_INBAND2_MASK) |
                    (((t_uint32) config->inband_mode << MCDE_INBAND2_SHIFT) & MCDE_INBAND2_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_BUSSIZE2_MASK) |
                    (((t_uint32) config->bus_size << MCDE_BUSSIZE2_SHIFT) & MCDE_BUSSIZE2_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_SYNCEN2_MASK) |
                    (((t_uint32) config->syncen << MCDE_SYNCEN2_SHIFT) & MCDE_SYNCEN2_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_WMLVL2_MASK) |
                    (((t_uint32) config->fifo_watermark << MCDE_WMLVL2_SHIFT) & MCDE_WMLVL2_MASK)
                );

            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_C2EN_MASK) |
                    (((t_uint32) config->chcen << MCDE_C2EN_SHIFT) & MCDE_C2EN_MASK)
                );

            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetChCCtrl()							  	         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the channel C control      */
/*                  parameters(Sync control and select,reset enable,clock)   */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_ctrl control : control structure                 */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC void MCDE_SetChCCtrl(IN t_mcde_chc_ctrl *control)
{
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    ch_c_reg->mcde_chc_crc =
        (
            (ch_c_reg->mcde_chc_crc &~MCDE_SYNCCTRL_MASK) |
            (((t_uint32) control->sync << MCDE_SYNCCTRL_SHIFT) & MCDE_SYNCCTRL_MASK)
        );

    ch_c_reg->mcde_chc_crc =
        (
            (ch_c_reg->mcde_chc_crc &~MCDE_RESEN_MASK) |
            (((t_uint32) control->resen << MCDE_RESEN_SHIFT) & MCDE_RESEN_MASK)
        );

    ch_c_reg->mcde_chc_crc =
        (
            (ch_c_reg->mcde_chc_crc &~MCDE_CLKSEL_MASK) |
            (((t_uint32) control->clksel << MCDE_CLKSEL_SHIFT) & MCDE_CLKSEL_MASK)
        );

    ch_c_reg->mcde_chc_crc =
        (
            (ch_c_reg->mcde_chc_crc &~MCDE_SYNCSEL_MASK) |
            (((t_uint32) control->synsel << MCDE_SYNCSEL_SHIFT) & MCDE_SYNCSEL_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/*****************************************************************************/
/* NAME			:	MCDE_ChC_PowerEnable()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to enable/disable power to channel C    */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_powen_select power: Enable/Disable                   */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC void MCDE_ChC_PowerEnable(IN t_mcde_powen_select power)
{
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    ch_c_reg->mcde_chc_crc =
        (
            (ch_c_reg->mcde_chc_crc &~MCDE_POWEREN_MASK) |
            (((t_uint32) power << MCDE_POWEREN_SHIFT) & MCDE_POWEREN_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/*****************************************************************************/
/* NAME			:	MCDE_ChC_FlowEnable()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to enable/disable Channel C flow        */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_flow_select flow: Enable/Disable                     */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC void MCDE_ChC_FlowEnable(IN t_mcde_flow_select flow)
{
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    ch_c_reg->mcde_chc_crc = ((ch_c_reg->mcde_chc_crc &~MCDE_FLOEN_MASK) | ((t_uint32) flow & MCDE_FLOEN_MASK));

    DBGEXIT0(MCDE_OK);
}

/*****************************************************************************/
/* NAME			:	MCDE_ConfigurePBCUnit()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the channel C PBC unit     */
/*                  parameters                                               */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: ID of the panel, C0/C1           */
/*               t_mcde_pbc_config config : PBC configuration structure      */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_ConfigurePBCUnit(IN t_mcde_chc_panel panel_id, IN t_mcde_pbc_config *config)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
	case MCDE_PANEL_C0:
        /*coverity[result_independent_of_operands]*/
			ch_c_reg->mcde_chc_pbcrc0 =
                (
                    (ch_c_reg->mcde_chc_pbcrc0 &~MCDE_PDCTRL_SHIFT) |
                    (((t_uint32) config->duplex_mode << MCDE_PDCTRL_SHIFT) & MCDE_PDCTRL_SHIFT)
                );

            ch_c_reg->mcde_chc_pbcrc0 =
                (
                    (ch_c_reg->mcde_chc_pbcrc0 &~MCDE_DUPLEXER_MASK) |
                    (((t_uint32) config->duplex_mode << MCDE_DUPLEXER_SHIFT) & MCDE_DUPLEXER_MASK)
                );

            ch_c_reg->mcde_chc_pbcrc0 =
                (
                    (ch_c_reg->mcde_chc_pbcrc0 &~MCDE_BSDM_MASK) |
                    (((t_uint32) config->data_segment << MCDE_BSDM_SHIFT) & MCDE_BSDM_MASK)
                );

            ch_c_reg->mcde_chc_pbcrc0 =
                (
                    (ch_c_reg->mcde_chc_pbcrc0 &~MCDE_BSCM_MASK) |
                    ((t_uint32) config->cmd_segment & MCDE_BSCM_MASK)
                );
            break;

	case MCDE_PANEL_C1:
        /*coverity[result_independent_of_operands]*/
            ch_c_reg->mcde_chc_pbcrc1 =
                (
                    (ch_c_reg->mcde_chc_pbcrc1 &~MCDE_PDCTRL_SHIFT) |
                    (((t_uint32) config->duplex_mode << MCDE_PDCTRL_SHIFT) & MCDE_PDCTRL_SHIFT)
                );

            ch_c_reg->mcde_chc_pbcrc1 =
                (
                    (ch_c_reg->mcde_chc_pbcrc1 &~MCDE_DUPLEXER_MASK) |
                    (((t_uint32) config->duplex_mode << MCDE_DUPLEXER_SHIFT) & MCDE_DUPLEXER_MASK)
                );

            ch_c_reg->mcde_chc_pbcrc1 =
                (
                    (ch_c_reg->mcde_chc_pbcrc1 &~MCDE_BSDM_MASK) |
                    (((t_uint32) config->data_segment << MCDE_BSDM_SHIFT) & MCDE_BSDM_MASK)
                );

            ch_c_reg->mcde_chc_pbcrc1 =
                (
                    (ch_c_reg->mcde_chc_pbcrc1 &~MCDE_BSCM_MASK) |
                    ((t_uint32) config->cmd_segment & MCDE_BSCM_MASK)
                );
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetPBCMUX()							  	         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the channel C PBC          */
/*                  multiplexer unit                                         */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: ID of the panel, C0/C1           */
/*               t_mcde_pbc_mux mux : PBC multiplexer structure              */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetPBCMUX(IN t_mcde_chc_panel panel_id, IN t_mcde_pbc_mux *mux)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_pbcbmrc0[0] = mux->imux0;

            ch_c_reg->mcde_chc_pbcbmrc0[1] = mux->imux1;

            ch_c_reg->mcde_chc_pbcbmrc0[2] = mux->imux2;

            ch_c_reg->mcde_chc_pbcbmrc0[3] = mux->imux3;

            ch_c_reg->mcde_chc_pbcbmrc0[4] = mux->imux4;
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_pbcbmrc1[0] = mux->imux0;

            ch_c_reg->mcde_chc_pbcbmrc1[1] = mux->imux1;

            ch_c_reg->mcde_chc_pbcbmrc1[2] = mux->imux2;

            ch_c_reg->mcde_chc_pbcbmrc1[3] = mux->imux3;

            ch_c_reg->mcde_chc_pbcbmrc1[4] = mux->imux4;
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetPBCBit_Ctrl()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the channel C PBC          */
/*                  Bit controller                                           */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: ID of the panel, C0/C1           */
/*               t_mcde_pbc_bitctrl bit_control: PBC bit control structure   */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetPBCBit_Ctrl(IN t_mcde_chc_panel panel_id, IN t_mcde_pbc_bitctrl *bit_control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_pbcbcrc0[0] = bit_control->bit_ctrl0;

            ch_c_reg->mcde_chc_pbcbcrc0[1] = bit_control->bit_ctrl1;
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_pbcbcrc1[0] = bit_control->bit_ctrl0;

            ch_c_reg->mcde_chc_pbcbcrc1[1] = bit_control->bit_ctrl1;
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetChCSynCaptureConf()							  	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the vertical SynCapture    */
/*                  Configuration                                            */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: ID of the panel, C0/C1           */
/*               t_mcde_sync_conf config: Vertical SynCapture configuration  */
/*                                                          structure        */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetChCSynCaptureConf(IN t_mcde_chc_panel panel_id, IN t_mcde_sync_conf *config)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_vscrc[0] =
                (
                    (ch_c_reg->mcde_chc_vscrc[0] &~MCDE_VSDBL_MASK) |
                    ((config->debounce_length << MCDE_VSDBL_SHIFT) & MCDE_VSDBL_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[0] =
                (
                    (ch_c_reg->mcde_chc_vscrc[0] &~MCDE_VSSEL_MASK) |
                    (((t_uint32) config->sync_sel << MCDE_VSSEL_SHIFT) & MCDE_VSSEL_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[0] =
                (
                    (ch_c_reg->mcde_chc_vscrc[0] &~MCDE_VSPOL_MASK) |
                    (((t_uint32) config->sync_pol << MCDE_VSPOL_SHIFT) & MCDE_VSPOL_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[0] =
                (
                    (ch_c_reg->mcde_chc_vscrc[0] &~MCDE_VSPDIV_MASK) |
                    (((t_uint32) config->clk_div << MCDE_VSPDIV_SHIFT) & MCDE_VSPDIV_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[0] =
                (
                    (ch_c_reg->mcde_chc_vscrc[0] &~MCDE_VSPMAX_MASK) |
                    (((t_uint32) config->vsp_max << MCDE_VSPMAX_SHIFT) & MCDE_VSPMAX_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[0] =
                (
                    (ch_c_reg->mcde_chc_vscrc[0] &~MCDE_VSPMIN_MASK) |
                    ((t_uint32) config->vsp_min & MCDE_VSPMIN_MASK)
                );
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_vscrc[1] =
                (
                    (ch_c_reg->mcde_chc_vscrc[1] &~MCDE_VSDBL_MASK) |
                    ((config->debounce_length << MCDE_VSDBL_SHIFT) & MCDE_VSDBL_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[1] =
                (
                    (ch_c_reg->mcde_chc_vscrc[1] &~MCDE_VSSEL_MASK) |
                    (((t_uint32) config->sync_sel << MCDE_VSSEL_SHIFT) & MCDE_VSSEL_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[1] =
                (
                    (ch_c_reg->mcde_chc_vscrc[1] &~MCDE_VSPOL_MASK) |
                    (((t_uint32) config->sync_pol << MCDE_VSPOL_SHIFT) & MCDE_VSPOL_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[1] =
                (
                    (ch_c_reg->mcde_chc_vscrc[1] &~MCDE_VSPDIV_MASK) |
                    (((t_uint32) config->clk_div << MCDE_VSPDIV_SHIFT) & MCDE_VSPDIV_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[1] =
                (
                    (ch_c_reg->mcde_chc_vscrc[1] &~MCDE_VSPMAX_MASK) |
                    (((t_uint32) config->vsp_max << MCDE_VSPMAX_SHIFT) & MCDE_VSPMAX_MASK)
                );

            ch_c_reg->mcde_chc_vscrc[1] =
                (
                    (ch_c_reg->mcde_chc_vscrc[1] &~MCDE_VSPMIN_MASK) |
                    ((t_uint32) config->vsp_min & MCDE_VSPMIN_MASK)
                );
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetChCSynCaptureTrigger()							 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the Channel C Trigger Event*/
/*                  Configuration                                            */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_sync_trigger trigger_event: trigger event structure  */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetChCSynCTriggerDelay(IN t_uint8 delay)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    ch_c_reg->mcde_chc_sctrc =
        (
            (ch_c_reg->mcde_chc_sctrc &~MCDE_TRDELC_MASK) |
            ((delay << MCDE_TRDELC_SHIFT) & MCDE_TRDELC_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetChCVSYNCDelay()							         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the Channel C delay after  */
/*                  Vertical synchro  to C0/C1                               */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: Panel C0/C1                      */
/*				 t_uint8 delay: delay                                        */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetChCVSYNCDelay(IN t_mcde_chc_panel panel_id, IN t_uint8 delay)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_sctrc = ((ch_c_reg->mcde_chc_sctrc &~MCDE_SYNCDELC0_MASK) | (delay & MCDE_SYNCDELC0_MASK));

            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_sctrc =
                (
                    (ch_c_reg->mcde_chc_sctrc &~MCDE_SYNCDELC1_MASK) |
                    ((delay << MCDE_SYNCDELC1_SHIFT) & MCDE_SYNCDELC1_MASK)
                );

            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_GetSyncCaptureStatus()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine reads the Synchro Capture Status for the panel */
/*																			*/
/* PARAMETERS :																*/
/*          IN: t_mcde_chc_panel panel_id  : ID of the panel		        */
/*     InOut  :t_bool *p_sync_status : variable to hold status              */
/* 		OUT   :None                                                         */
/*                                                                          */
/* RETURN	  :t_mcde_error	: MCDE error code						   	    */
/*             MCDE_OK                                                      */
/*             MCDE_INVALID_PARAMETER   if input argument is not valid      */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_GetSyncCaptureStatus(IN t_mcde_chc_panel panel_id, OUT t_bool *p_sync_status)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    if (NULL == p_sync_status)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            *p_sync_status = (t_bool) (ch_c_reg->mcde_chc_scsr & MCDE_VSTAC0_MASK);

            break;

        case MCDE_PANEL_C1:
            *p_sync_status = (t_bool) ((ch_c_reg->mcde_chc_scsr & MCDE_VSTAC1_MASK) >> MCDE_VSTAC1_SHIFT);

            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_SetBusAccessNum()								        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the BUS Access Number for the panel       */
/*																			*/
/* PARAMETERS :																*/
/*          IN: t_mcde_chc_panel panel_id  : ID of the panel		        */
/*            : t_uint8 bcn: activation/deactivation bus access number      */
/*     InOut  :None                                                         */
/* 		OUT   :None                                                         */
/*                                                                          */
/* RETURN	  :t_mcde_error	: MCDE error code						   	    */
/*             MCDE_OK                                                      */
/*             MCDE_INVALID_PARAMETER   if input argument is not valid      */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetBusAccessNum(IN t_mcde_chc_panel panel_id, IN t_uint8 bcn)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_bcnr[0] = ((ch_c_reg->mcde_chc_bcnr[0] &~MCDE_BCN_MASK) | (bcn & MCDE_BCN_MASK));
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_bcnr[1] = ((ch_c_reg->mcde_chc_bcnr[1] &~MCDE_BCN_MASK) | (bcn & MCDE_BCN_MASK));
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_SetChipSelectTiming()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the activate/deactivate timing of         */
/*              Chip Select(CS) signal                                      */
/*																			*/
/* PARAMETERS :																*/
/*          IN: t_mcde_chc_panel panel_id  : ID of the panel		        */
/*            : t_mcde_cd_timing_activate active: activation/deactivation   */
/*                                                  time                    */
/*     InOut  :None                                                         */
/* 		OUT   :None                                                         */
/*                                                                          */
/* RETURN	  :t_mcde_error	: MCDE error code						   	    */
/*             MCDE_OK                                                      */
/*             MCDE_INVALID_PARAMETER   if input argument is not valid      */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetChipSelectTiming(IN t_mcde_chc_panel panel_id, IN t_mcde_cd_timing_activate *active)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_cscdtr[0] =
                (
                    (ch_c_reg->mcde_chc_cscdtr[0] &~MCDE_CSCDDEACT_MASK) |
                    ((active->cs_cd_deactivate << MCDE_CSCDDEACT_SHIFT) & MCDE_CSCDDEACT_MASK)
                );

            ch_c_reg->mcde_chc_cscdtr[0] =
                (
                    (ch_c_reg->mcde_chc_cscdtr[0] &~MCDE_CSCDACT_MASK) |
                    (active->cs_cd_activate & MCDE_CSCDACT_MASK)
                );
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_cscdtr[1] =
                (
                    (ch_c_reg->mcde_chc_cscdtr[1] &~MCDE_CSCDDEACT_MASK) |
                    ((active->cs_cd_deactivate << MCDE_CSCDDEACT_SHIFT) & MCDE_CSCDDEACT_MASK)
                );

            ch_c_reg->mcde_chc_cscdtr[1] =
                (
                    (ch_c_reg->mcde_chc_cscdtr[1] &~MCDE_CSCDACT_MASK) |
                    (active->cs_cd_activate & MCDE_CSCDACT_MASK)
                );
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_SetReadWriteTiming()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the activate/deactivate timing of         */
/*              READ/WRITE signal                                           */
/*																			*/
/* PARAMETERS :																*/
/*          IN: t_mcde_chc_panel panel_id  : ID of the panel		        */
/*            : t_mcde_rw_timing rw_time: activation/deactivation           */
/*                                                  time                    */
/*     InOut  :None                                                         */
/* 		OUT   :None                                                         */
/*                                                                          */
/* RETURN	  :t_mcde_error	: MCDE error code						   	    */
/*             MCDE_OK                                                      */
/*             MCDE_INVALID_PARAMETER   if input argument is not valid      */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetReadWriteTiming(IN t_mcde_chc_panel panel_id, IN t_mcde_rw_timing *rw_time)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_rdwrtr[0] =
                (
                    (ch_c_reg->mcde_chc_rdwrtr[0] &~MCDE_MOTINT_MASK) |
                    (((t_uint32) rw_time->panel_type << MCDE_MOTINT_SHIFT) & MCDE_MOTINT_MASK)
                );

            ch_c_reg->mcde_chc_rdwrtr[0] =
                (
                    (ch_c_reg->mcde_chc_rdwrtr[0] &~MCDE_RWDEACT_MASK) |
                    ((rw_time->readwrite_deactivate << MCDE_RWDEACT_SHIFT) & MCDE_RWDEACT_MASK)
                );

            ch_c_reg->mcde_chc_rdwrtr[0] =
                (
                    (ch_c_reg->mcde_chc_rdwrtr[0] &~MCDE_RWACT_MASK) |
                    (rw_time->readwrite_activate & MCDE_RWACT_MASK)
                );
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_rdwrtr[1] =
                (
                    (ch_c_reg->mcde_chc_rdwrtr[1] &~MCDE_MOTINT_MASK) |
                    (((t_uint32) rw_time->panel_type << MCDE_MOTINT_SHIFT) & MCDE_MOTINT_MASK)
                );

            ch_c_reg->mcde_chc_rdwrtr[1] =
                (
                    (ch_c_reg->mcde_chc_rdwrtr[1] &~MCDE_RWDEACT_MASK) |
                    ((rw_time->readwrite_deactivate << MCDE_RWDEACT_SHIFT) & MCDE_RWDEACT_MASK)
                );

            ch_c_reg->mcde_chc_rdwrtr[1] =
                (
                    (ch_c_reg->mcde_chc_rdwrtr[1] &~MCDE_RWACT_MASK) |
                    (rw_time->readwrite_activate & MCDE_RWACT_MASK)
                );
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_SetDataOutTiming()								        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the activate/deactivate timing of dataout */
/*																			*/
/* PARAMETERS :																*/
/*          IN: t_mcde_chc_panel panel_id  : ID of the panel		        */
/*            : t_mcde_data_out_timing data_time: activation/deactivation   */
/*                                                  time                    */
/*     InOut  :None                                                         */
/* 		OUT   :None                                                         */
/*                                                                          */
/* RETURN	  :t_mcde_error	: MCDE error code						   	    */
/*             MCDE_OK                                                      */
/*             MCDE_INVALID_PARAMETER   if input argument is not valid      */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_SetDataOutTiming(IN t_mcde_chc_panel panel_id, IN t_mcde_data_out_timing *data_time)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_dotr[0] =
                (
                    (ch_c_reg->mcde_chc_dotr[0] &~MCDE_DODEACT_MASK) |
                    ((data_time->data_out_deactivate << MCDE_DODEACT_SHIFT) & MCDE_DODEACT_MASK)
                );

            ch_c_reg->mcde_chc_dotr[0] =
                (
                    (ch_c_reg->mcde_chc_dotr[0] &~MCDE_DOACT_MASK) |
                    (data_time->data_out_activate & MCDE_DOACT_MASK)
                );
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_dotr[1] =
                (
                    (ch_c_reg->mcde_chc_dotr[1] &~MCDE_DODEACT_MASK) |
                    ((data_time->data_out_deactivate << MCDE_DODEACT_SHIFT) & MCDE_DODEACT_MASK)
                );

            ch_c_reg->mcde_chc_dotr[1] =
                (
                    (ch_c_reg->mcde_chc_dotr[1] &~MCDE_DOACT_MASK) |
                    (data_time->data_out_activate & MCDE_DOACT_MASK)
                );
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_WriteCommand()										    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine writes command in the command register of MCDE */
/*																			*/
/* PARAMETERS :																*/
/*          IN: t_mcde_chc_panel panel_id  : ID of the panel		        */
/*            : command : command to be written                             */
/*     InOut  :None                                                         */
/* 		OUT   :None                                                         */
/*                                                                          */
/* RETURN	  :t_mcde_error	: MCDE error code						   	    */
/*             MCDE_OK                                                      */
/*             MCDE_INVALID_PARAMETER   if input argument is not valid      */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_WriteCommand(IN t_mcde_chc_panel panel_id, IN t_uint32 command)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();
    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_wcmd[0] = (command & MCDE_DATACOMMANDMASK);

            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_wcmd[1] = (command & MCDE_DATACOMMANDMASK);

            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_WriteData()										    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine writes data in the Data register of MCDE       */
/*																			*/
/* PARAMETERS :																*/
/*          IN: t_mcde_chc_panel panel_id  : ID of the panel		        */
/*            : data : data to be written                                   */
/*     InOut  :None                                                         */
/* 		OUT   :None                                                         */
/*                                                                          */
/* RETURN	  :t_mcde_error	: MCDE error code						   	    */
/*             MCDE_OK                                                      */
/*             MCDE_INVALID_PARAMETER   if input argument is not valid      */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_WriteData(IN t_mcde_chc_panel panel_id, IN t_uint32 data)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_wd[0] = (data & MCDE_DATACOMMANDMASK);
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_wd[1] = (data & MCDE_DATACOMMANDMASK);
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_ReadData()										        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine reads data from the Rx FIFO                	*/
/*																			*/
/* PARAMETERS :																*/
/*          IN: t_mcde_chc_panel panel_id  : ID of the panel		        */
/*     InOut  :None                                                         */
/* 		OUT   :t_uint16 *pt_data : pointer to the data read value           */
/*                                                                          */
/* RETURN	  :t_mcde_error	: MCDE error code						   	    */
/*             MCDE_OK                                                      */
/*             MCDE_INVALID_PARAMETER   if input argument is not valid      */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_ReadData(IN t_mcde_chc_panel panel_id, OUT t_uint16 *pt_data)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_rdata[0] = 0x00010000;

            *pt_data = (t_uint16) (ch_c_reg->mcde_chc_rdata[0] & 0x0000FFFF);
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_rdata[1] = 0x00010000;

            *pt_data = (t_uint16) (ch_c_reg->mcde_chc_rdata[1] & 0x0000FFFF);
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/****************************************************************************/
/*		 NAME :	MCDE_WriteTxFIFO()										    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine writes data to one of the four Tx FIFO         */
/*              registers                	                                */
/*																			*/
/* PARAMETERS :                                                             */
/*          IN: t_mcde_chc_panel panel_id  : ID of the panel		        */
/*            :	t_dif_txfifo_request_type type : specifies the request type */
/*                                               for writing into one of the*/
/*                                               four registers             */
/*              t_uint32 data : contains the 24 bit data to be written      */
/*     InOut  :None                                                         */
/* 		OUT   :None                                                	        */
/*                                                                          */
/* RETURN	  :t_mcde_error	: MCDE error code						   	    */
/*             DIF_OK                                                       */
/*             DIF_INVALID_PARAMETER: if input argument is invalid          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_mcde_error MCDE_WriteTxFIFO(IN t_mcde_chc_panel panel_id, IN t_mcde_txfifo_request_type type, IN t_uint32 data)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (type)
    {
        case MCDE_TXFIFO_WRITE_DATA:
            ch_c_reg->mcde_chc_wd[panel_id] = (data & MCDE_DATACOMMANDMASK);

            break;

        case MCDE_TXFIFO_WRITE_COMMAND:
            ch_c_reg->mcde_chc_wcmd[panel_id] = (data & MCDE_DATACOMMANDMASK);

            break;

        default:
            error = MCDE_INVALID_PARAMETER;
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_EnableSynchro()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to enable synchro for  Channel C0/C1    */
/*                                                                           */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: Panel C0/C1                      */
/*               t_mcde_synchro_capture sync_enable:synchro enable           */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_EnableSynchro(IN t_mcde_chc_panel panel_id, IN t_mcde_synchro_capture sync_enable)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_SYNCEN1_MASK) |
                    (((t_uint32) sync_enable << MCDE_SYNCEN1_SHIFT) & MCDE_SYNCEN1_MASK)
                );
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_SYNCEN2_MASK) |
                    (((t_uint32) sync_enable << MCDE_SYNCEN2_SHIFT) & MCDE_SYNCEN2_MASK)
                );
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_EnableChannelC()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to enable  Channel C0/C1                */
/*                                                                           */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: Panel C0/C1                      */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_EnableChannelC(IN t_mcde_chc_panel panel_id)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_C1EN_MASK) |
                    ((1 << MCDE_C1EN_SHIFT) & MCDE_C1EN_MASK)
                );
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_C2EN_MASK) |
                    ((1 << MCDE_C2EN_SHIFT) & MCDE_C2EN_MASK)
                );
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_DisableChannelC()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to disable Channel C0/C1                */
/*                                                                           */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: Panel C0/C1                      */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_DisableChannelC(IN t_mcde_chc_panel panel_id)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_C1EN_MASK) |
                    ((0 << MCDE_C1EN_SHIFT) & MCDE_C1EN_MASK)
                );
            break;

        case MCDE_PANEL_C1:
            ch_c_reg->mcde_chc_crc =
                (
                    (ch_c_reg->mcde_chc_crc &~MCDE_C2EN_MASK) |
                    ((0 << MCDE_C2EN_SHIFT) & MCDE_C2EN_MASK)
                );
            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_ChCSyncSelect()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to select the synchro for Channel C     */
/*                                                                           */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_synchro_select sync_select: Synchro selection        */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC void MCDE_ChCSyncSelect(IN t_mcde_synchro_select sync_select)
{
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    ch_c_reg->mcde_chc_crc =
        (
            (ch_c_reg->mcde_chc_crc &~MCDE_SYNCSEL_MASK) |
            (((t_uint32) sync_select << MCDE_SYNCSEL_SHIFT) & MCDE_SYNCSEL_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/*****************************************************************************/
/* NAME			:	MCDE_ChCClkSelect()							  	         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the clock for Channel C          */
/*                                                                           */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_clk_sel clk_sel: Clock type                          */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC void MCDE_ChCClkSelect(IN t_mcde_clk_sel clk_sel)
{
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    ch_c_reg->mcde_chc_crc =
        (
            (ch_c_reg->mcde_chc_crc &~MCDE_CLKSEL_MASK) |
            (((t_uint32) clk_sel << MCDE_CLKSEL_SHIFT) & MCDE_CLKSEL_MASK)
        );

    DBGEXIT0(MCDE_OK);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetChCConfig()							  	         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the channel C configuration     */
/*                  parameters                                               */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_chc_panel panel_id: ID of the panel, C0/C1           */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_chc_config *config : config structure pointer        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetChCConfig(IN t_mcde_chc_panel panel_id, OUT t_mcde_chc_config *p_config)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    if (NULL == p_config)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    switch (panel_id)
    {
        case MCDE_PANEL_C0:
            p_config->res_pol = (t_mcde_sig_pol) ((ch_c_reg->mcde_chc_crc & MCDE_RES1_MASK) >> MCDE_RES1_SHIFT);

            p_config->rd_pol = (t_mcde_sig_pol) ((ch_c_reg->mcde_chc_crc & MCDE_RD1_MASK) >> MCDE_RD1_SHIFT);

            p_config->wr_pol = (t_mcde_sig_pol) ((ch_c_reg->mcde_chc_crc & MCDE_WR1_MASK) >> MCDE_WR1_SHIFT);

            p_config->cd_pol = (t_mcde_cd_polarity) ((ch_c_reg->mcde_chc_crc & MCDE_CD1_MASK) >> MCDE_CD1_SHIFT);

            p_config->cs_pol = (t_mcde_sig_pol) ((ch_c_reg->mcde_chc_crc & MCDE_CS1_MASK) >> MCDE_CS1_SHIFT);

            p_config->csen = (t_mcde_cs_enable_rw) ((ch_c_reg->mcde_chc_crc & MCDE_CS1EN_MASK) >> MCDE_CS1EN_SHIFT);

            p_config->inband_mode = (t_mcde_inband_select) ((ch_c_reg->mcde_chc_crc & MCDE_INBAND1_MASK) >> MCDE_INBAND1_SHIFT);

            p_config->bus_size = (t_mcde_bus_size) ((ch_c_reg->mcde_chc_crc & MCDE_BUSSIZE1_MASK) >> MCDE_BUSSIZE1_SHIFT);

            p_config->syncen = (t_mcde_synchro_capture) ((ch_c_reg->mcde_chc_crc & MCDE_SYNCEN1_MASK) >> MCDE_SYNCEN1_SHIFT);

            p_config->fifo_watermark = (t_mcde_fifo_wmlvl_sel) ((ch_c_reg->mcde_chc_crc & MCDE_WMLVL1_MASK) >> MCDE_WMLVL1_SHIFT);

            p_config->chcen = (t_mcde_chc_enable) ((ch_c_reg->mcde_chc_crc & MCDE_C1EN_MASK) >> MCDE_C1EN_SHIFT);

            break;

        case MCDE_PANEL_C1:
            p_config->res_pol = (t_mcde_sig_pol) ((ch_c_reg->mcde_chc_crc & MCDE_RES2_MASK) >> MCDE_RES2_SHIFT);

            p_config->rd_pol = (t_mcde_sig_pol) ((ch_c_reg->mcde_chc_crc & MCDE_RD2_MASK) >> MCDE_RD2_SHIFT);

            p_config->wr_pol = (t_mcde_sig_pol) ((ch_c_reg->mcde_chc_crc & MCDE_WR2_MASK) >> MCDE_WR2_SHIFT);

            p_config->cd_pol = (t_mcde_cd_polarity) ((ch_c_reg->mcde_chc_crc & MCDE_CD2_MASK) >> MCDE_CD2_SHIFT);

            p_config->cs_pol = (t_mcde_sig_pol) ((ch_c_reg->mcde_chc_crc & MCDE_CS2_MASK) >> MCDE_CS2_SHIFT);

            p_config->csen = (t_mcde_cs_enable_rw) ((ch_c_reg->mcde_chc_crc & MCDE_CS2EN_MASK) >> MCDE_CS2EN_SHIFT);

            p_config->inband_mode = (t_mcde_inband_select) ((ch_c_reg->mcde_chc_crc & MCDE_INBAND2_MASK) >> MCDE_INBAND2_SHIFT);

            p_config->bus_size = (t_mcde_bus_size) ((ch_c_reg->mcde_chc_crc & MCDE_BUSSIZE2_MASK) >> MCDE_BUSSIZE2_SHIFT);

            p_config->syncen = (t_mcde_synchro_capture) ((ch_c_reg->mcde_chc_crc & MCDE_SYNCEN2_MASK) >> MCDE_SYNCEN2_SHIFT);

            p_config->fifo_watermark = (t_mcde_fifo_wmlvl_sel) ((ch_c_reg->mcde_chc_crc & MCDE_WMLVL2_MASK) >> MCDE_WMLVL2_SHIFT);

            p_config->chcen = (t_mcde_chc_enable) ((ch_c_reg->mcde_chc_crc & MCDE_C2EN_MASK) >> MCDE_C2EN_SHIFT);

            break;

        default:
            DBGEXIT0(MCDE_INVALID_PARAMETER);
            return(MCDE_INVALID_PARAMETER);
    }

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetChCCtrl()							  	         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read  the channel C control          */
/*                  parameters(Sync control and select,reset enable,clock)   */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:None                                                        */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_chc_ctrl *control : control structure pointer        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetChCCtrl(OUT t_mcde_chc_ctrl *p_control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_chc_reg  *ch_c_reg;

    ch_c_reg = (t_mcde_chc_reg *) &(g_mcde_system_context.p_mcde_register->ch_c_reg);
    DBGENTER0();

    if (NULL == p_control)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_control->sync = (t_mcde_sync_ctrl) ((ch_c_reg->mcde_chc_crc & MCDE_SYNCCTRL_MASK) >> MCDE_SYNCCTRL_SHIFT);

    p_control->resen = (t_mcde_resen) ((ch_c_reg->mcde_chc_crc & MCDE_RESEN_MASK) >> MCDE_RESEN_SHIFT);

    p_control->clksel = (t_mcde_clk_sel) ((ch_c_reg->mcde_chc_crc & MCDE_CLKSEL_MASK) >> MCDE_CLKSEL_SHIFT);

    p_control->synsel = (t_mcde_synchro_select) ((ch_c_reg->mcde_chc_crc & MCDE_SYNCSEL_MASK) >> MCDE_SYNCSEL_SHIFT);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************
*  Channel A/B APIs                                                          *
******************************************************************************/
/*****************************************************************************/
/* NAME			:	MCDE_SetChannelXControl()							  	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to read the channel A/B control         */
/*                  parameters                                               */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*               t_mcde_chx_control0 control : control structure             */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetChannelXControl(IN t_mcde_ch_id channel, IN t_mcde_chx_control0 *control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_CHX_BURSTSIZE_MASK) |
            (((t_uint32) control->chx_read_request << MCDE_CHX_BURSTSIZE_SHIFT) & MCDE_CHX_BURSTSIZE_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_CHX_ALPHA_MASK) |
            (((t_uint32) control->alpha_blend << MCDE_CHX_ALPHA_SHIFT) & MCDE_CHX_ALPHA_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_CHX_ROTDIR_MASK) |
            (((t_uint32) control->rot_dir << MCDE_CHX_ROTDIR_SHIFT) & MCDE_CHX_ROTDIR_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_CHX_GAMAEN_MASK) |
            (((t_uint32) control->gamma_ctrl << MCDE_CHX_GAMAEN_SHIFT) & MCDE_CHX_GAMAEN_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_FLICKFORMAT_MASK) |
            (((t_uint32) control->flicker_format << MCDE_FLICKFORMAT_SHIFT) & MCDE_FLICKFORMAT_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_FLICKMODE_MASK) |
            (((t_uint32) control->filter_mode << MCDE_FLICKMODE_SHIFT) & MCDE_FLICKMODE_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_BLENDCONTROL_MASK) |
            (((t_uint32) control->blend << MCDE_BLENDCONTROL_SHIFT) & MCDE_BLENDCONTROL_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_KEYCTRL_MASK) |
            (((t_uint32) control->key_ctrl << MCDE_KEYCTRL_SHIFT) & MCDE_KEYCTRL_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_ROTEN_MASK) |
            (((t_uint32) control->rot_enable << MCDE_ROTEN_SHIFT) & MCDE_ROTEN_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_DITHEN_MASK) |
            (((t_uint32) control->dither_ctrl << MCDE_DITHEN_SHIFT) & MCDE_DITHEN_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_CEAEN_MASK) |
            (((t_uint32) control->color_enhance << MCDE_CEAEN_SHIFT) & MCDE_CEAEN_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_AFLICKEN_MASK) |
            (((t_uint32) control->anti_flicker << MCDE_AFLICKEN_SHIFT) & MCDE_AFLICKEN_MASK)
        );

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_BLENDEN_MASK) |
            (((t_uint32) control->blend_ctrl << MCDE_BLENDEN_SHIFT) & MCDE_BLENDEN_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetChannelXControl()							  	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to configure the channel A/B control    */
/*                  parameters                                               */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_chx_control0 *control : control structure pointer    */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetChannelXControl(IN t_mcde_ch_id channel, IN t_mcde_chx_control0 *p_control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    p_control->chx_read_request = (t_mcde_rotate_req) ((ch_x_reg->mcde_ch_cr0 & MCDE_CHX_BURSTSIZE_MASK) >> MCDE_CHX_BURSTSIZE_SHIFT);

    p_control->alpha_blend = (t_uint8) ((ch_x_reg->mcde_ch_cr0 & MCDE_CHX_ALPHA_MASK) >> MCDE_CHX_ALPHA_SHIFT);

    p_control->rot_dir = (t_mcde_rot_dir) ((ch_x_reg->mcde_ch_cr0 & MCDE_CHX_ROTDIR_MASK) >> MCDE_CHX_ROTDIR_SHIFT);

    p_control->gamma_ctrl = (t_mcde_gamma_ctrl) ((ch_x_reg->mcde_ch_cr0 & MCDE_CHX_GAMAEN_MASK) >> MCDE_CHX_GAMAEN_SHIFT);

    p_control->flicker_format = (t_mcde_flicker_format) ((ch_x_reg->mcde_ch_cr0 & MCDE_FLICKFORMAT_MASK) >> MCDE_FLICKFORMAT_SHIFT);

    p_control->filter_mode = (t_mcde_flicker_filter_mode) ((ch_x_reg->mcde_ch_cr0 & MCDE_FLICKMODE_MASK) >> MCDE_FLICKMODE_SHIFT);

    p_control->blend = (t_mcde_blend_control) ((ch_x_reg->mcde_ch_cr0 & MCDE_BLENDCONTROL_MASK) >> MCDE_BLENDCONTROL_SHIFT);

    p_control->key_ctrl = (t_mcde_key_ctrl) ((ch_x_reg->mcde_ch_cr0 & MCDE_KEYCTRL_MASK) >> MCDE_KEYCTRL_SHIFT);

    p_control->rot_enable = (t_mcde_roten) ((ch_x_reg->mcde_ch_cr0 & MCDE_ROTEN_MASK) >> MCDE_ROTEN_SHIFT);

    p_control->dither_ctrl = (t_mcde_dithering_control) ((ch_x_reg->mcde_ch_cr0 & MCDE_DITHEN_MASK) >> MCDE_DITHEN_SHIFT);

    p_control->color_enhance = (t_mcde_clr_enhance_ctrl) ((ch_x_reg->mcde_ch_cr0 & MCDE_CEAEN_MASK) >> MCDE_CEAEN_SHIFT);

    p_control->anti_flicker = (t_mcde_antiflicker_ctrl) ((ch_x_reg->mcde_ch_cr0 & MCDE_AFLICKEN_MASK) >> MCDE_AFLICKEN_SHIFT);

    p_control->blend_ctrl = (t_mcde_blend_status) ((ch_x_reg->mcde_ch_cr0 & MCDE_BLENDEN_MASK) >> MCDE_BLENDEN_SHIFT);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_ChannelX_PowerEnable()							  	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to enable power to the channel A/B      */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_ChannelX_PowerEnable(IN t_mcde_ch_id channel)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_POWEREN_MASK) |
            ((0x1 << MCDE_POWEREN_SHIFT) & MCDE_POWEREN_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_ChannelX_PowerDisable()							 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to disable power to the channel A/B     */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_ChannelX_PowerDisable(IN t_mcde_ch_id channel)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_cr0 =
        (
            (ch_x_reg->mcde_ch_cr0 &~MCDE_POWEREN_MASK) |
            ((0x0 << MCDE_POWEREN_SHIFT) & MCDE_POWEREN_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_ChannelX_FlowEnable()							  	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to enable Channel A/B flow              */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_ChannelX_FlowEnable(IN t_mcde_ch_id channel)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_cr0 = ((ch_x_reg->mcde_ch_cr0 &~MCDE_FLOEN_MASK) | (0x1 & MCDE_FLOEN_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_ChannelX_FlowDisable()							  	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to disable Channel A/B flow             */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_ChannelX_FlowDisable(IN t_mcde_ch_id channel)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_cr0 = ((ch_x_reg->mcde_ch_cr0 &~MCDE_FLOEN_MASK) | (0x0 & MCDE_FLOEN_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetPanelControl()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: This API is used to configure Channel A/B panel parameters */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_chx_control1 control:control structure               */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetPanelControl(IN t_mcde_ch_id channel, IN t_mcde_chx_control1 *control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_cr1 =
        (
            (ch_x_reg->mcde_ch_cr1 &~MCDE_CLK_MASK) |
            (((t_uint32) control->tv_clk << MCDE_CLK_SHIFT) & MCDE_CLK_MASK)
        );

    ch_x_reg->mcde_ch_cr1 =
        (
            (ch_x_reg->mcde_ch_cr1 &~MCDE_BCD_MASK) |
            (((t_uint32) control->bcd_ctrl << MCDE_BCD_SHIFT) & MCDE_BCD_MASK)
        );

    ch_x_reg->mcde_ch_cr1 =
        (
            (ch_x_reg->mcde_ch_cr1 &~MCDE_OUTBPP_MASK) |
            (((t_uint32) control->out_bpp << MCDE_OUTBPP_SHIFT) & MCDE_OUTBPP_MASK)
        );

    ch_x_reg->mcde_ch_cr1 =
        (
            (ch_x_reg->mcde_ch_cr1 &~MCDE_CLP_MASK) |
            (((t_uint32) control->clk_per_line << MCDE_CLP_SHIFT) & MCDE_CLP_MASK)
        );

    ch_x_reg->mcde_ch_cr1 =
        (
            (ch_x_reg->mcde_ch_cr1 &~MCDE_CDWIN_MASK) |
            (((t_uint32) control->lcd_bus << MCDE_CDWIN_SHIFT) & MCDE_CDWIN_MASK)
        );

    ch_x_reg->mcde_ch_cr1 =
        (
            (ch_x_reg->mcde_ch_cr1 &~MCDE_CLOCKSEL_MASK) |
            (((t_uint32) control->dpi2_clk << MCDE_CLOCKSEL_SHIFT) & MCDE_CLOCKSEL_MASK)
        );

    ch_x_reg->mcde_ch_cr1 = ((ch_x_reg->mcde_ch_cr1 &~MCDE_PCD_MASK) | ((t_uint32) control->pcd & MCDE_PCD_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetPanelControl()							  	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: This API is used to read Channel A/B panel parameters      */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_chx_control1 *control:control structure              */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetPanelControl(IN t_mcde_ch_id channel, IN t_mcde_chx_control1 *p_control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_control)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_control->tv_clk = (t_mcde_tv_clk) ((ch_x_reg->mcde_ch_cr1 & MCDE_CLK_MASK) >> MCDE_CLK_SHIFT);

    p_control->bcd_ctrl = (t_mcde_bcd_ctrl) ((ch_x_reg->mcde_ch_cr1 & MCDE_BCD_MASK) >> MCDE_BCD_SHIFT);

    p_control->out_bpp = (t_mcde_out_bpp) ((ch_x_reg->mcde_ch_cr1 & MCDE_OUTBPP_MASK) >> MCDE_OUTBPP_SHIFT);

    p_control->clk_per_line = (t_uint16) ((ch_x_reg->mcde_ch_cr1 & MCDE_CLP_MASK) >> MCDE_CLP_SHIFT);

    p_control->lcd_bus = (t_mcde_lcd_bus) ((ch_x_reg->mcde_ch_cr1 & MCDE_CDWIN_MASK) >> MCDE_CDWIN_SHIFT);

    p_control->dpi2_clk = (t_mcde_dpi2_clksel) ((ch_x_reg->mcde_ch_cr1 & MCDE_CLOCKSEL_MASK) >> MCDE_CLOCKSEL_SHIFT);

    p_control->pcd = (t_uint16) (ch_x_reg->mcde_ch_cr1 & MCDE_PCD_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetColorKey()							  	         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the value of color key of panel  */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_color_key key:color key structure                    */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetColorKey(IN t_mcde_ch_id channel, IN t_mcde_color_key *key)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_colkey =
        (
            (ch_x_reg->mcde_ch_colkey &~MCDE_KEYA_MASK) |
            ((key->alpha << MCDE_KEYA_SHIFT) & MCDE_KEYA_MASK)
        );

    ch_x_reg->mcde_ch_colkey =
        (
            (ch_x_reg->mcde_ch_colkey &~MCDE_KEYR_MASK) |
            ((key->red << MCDE_KEYR_SHIFT) & MCDE_KEYR_MASK)
        );

    ch_x_reg->mcde_ch_colkey =
        (
            (ch_x_reg->mcde_ch_colkey &~MCDE_KEYG_MASK) |
            ((key->green << MCDE_KEYG_SHIFT) & MCDE_KEYG_MASK)
        );

    ch_x_reg->mcde_ch_colkey = ((ch_x_reg->mcde_ch_colkey &~MCDE_KEYB_MASK) | (key->blue & MCDE_KEYB_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetColorKey()							  	         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the value of color key of panel  */
/*				                                                    		 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_color_key *key:color key structure pointer           */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetColorKey(IN t_mcde_ch_id channel, IN t_mcde_color_key *p_key)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_key)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_key->alpha = (t_uint8) ((ch_x_reg->mcde_ch_colkey & MCDE_KEYA_MASK) >> MCDE_KEYA_SHIFT);

    p_key->red = (t_uint8) ((ch_x_reg->mcde_ch_colkey & MCDE_KEYR_MASK) >> MCDE_KEYR_SHIFT);

    p_key->green = (t_uint8) ((ch_x_reg->mcde_ch_colkey & MCDE_KEYG_MASK) >> MCDE_KEYG_SHIFT);

    p_key->blue = (t_uint8) (ch_x_reg->mcde_ch_colkey & MCDE_KEYB_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetForcedColorKey()							  	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the forced value of color key of */
/*				    panel                                                    */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_color_key key:color key structure                    */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetForcedColorKey(IN t_mcde_ch_id channel, IN t_mcde_forced_color_key *key)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_fcolkey =
        (
            (ch_x_reg->mcde_ch_fcolkey &~MCDE_KEYA_MASK) |
            ((key->f_alpha << MCDE_KEYA_SHIFT) & MCDE_KEYA_MASK)
        );

    ch_x_reg->mcde_ch_fcolkey =
        (
            (ch_x_reg->mcde_ch_fcolkey &~MCDE_KEYR_MASK) |
            ((key->f_red << MCDE_KEYR_SHIFT) & MCDE_KEYR_MASK)
        );

    ch_x_reg->mcde_ch_fcolkey =
        (
            (ch_x_reg->mcde_ch_fcolkey &~MCDE_KEYG_MASK) |
            ((key->f_green << MCDE_KEYG_SHIFT) & MCDE_KEYG_MASK)
        );

    ch_x_reg->mcde_ch_fcolkey = ((ch_x_reg->mcde_ch_fcolkey &~MCDE_KEYB_MASK) | (key->f_blue & MCDE_KEYB_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetForcedColorKey()							  	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the forced value of color key of */
/*				    panel                                                    */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_color_key *key:color key structure pointer           */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetForcedColorKey(IN t_mcde_ch_id channel, IN t_mcde_forced_color_key *p_key)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_key)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_key->f_alpha = (t_uint8) ((ch_x_reg->mcde_ch_fcolkey & MCDE_KEYA_MASK) >> MCDE_KEYA_SHIFT);

    p_key->f_red = (t_uint8) ((ch_x_reg->mcde_ch_fcolkey & MCDE_KEYR_MASK) >> MCDE_KEYR_SHIFT);

    p_key->f_green = (t_uint8) ((ch_x_reg->mcde_ch_fcolkey & MCDE_KEYG_MASK) >> MCDE_KEYG_SHIFT);

    p_key->f_blue = (t_uint8) (ch_x_reg->mcde_ch_fcolkey & MCDE_KEYB_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetColorConversionMatrix()							 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the value of coefficients of     */
/*				    color conversion matrix                                  */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_rgb_conv_coef coef:color conversion matrix structure */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetColorConversionMatrix(IN t_mcde_ch_id channel, IN t_mcde_rgb_conv_coef *coef)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_rgbconv1 =
        (
            (ch_x_reg->mcde_ch_rgbconv1 &~MCDE_RGB_MASK1) |
            ((coef->Yr_red << MCDE_RGB_SHIFT) & MCDE_RGB_MASK1)
        );

    ch_x_reg->mcde_ch_rgbconv1 = ((ch_x_reg->mcde_ch_rgbconv1 &~MCDE_RGB_MASK2) | (coef->Yr_green & MCDE_RGB_MASK2));

    ch_x_reg->mcde_ch_rgbconv2 =
        (
            (ch_x_reg->mcde_ch_rgbconv2 &~MCDE_RGB_MASK1) |
            ((coef->Yr_blue << MCDE_RGB_SHIFT) & MCDE_RGB_MASK1)
        );

    ch_x_reg->mcde_ch_rgbconv2 = ((ch_x_reg->mcde_ch_rgbconv2 &~MCDE_RGB_MASK2) | (coef->Cr_red & MCDE_RGB_MASK2));

    ch_x_reg->mcde_ch_rgbconv3 =
        (
            (ch_x_reg->mcde_ch_rgbconv3 &~MCDE_RGB_MASK1) |
            ((coef->Cr_green << MCDE_RGB_SHIFT) & MCDE_RGB_MASK1)
        );

    ch_x_reg->mcde_ch_rgbconv3 = ((ch_x_reg->mcde_ch_rgbconv3 &~MCDE_RGB_MASK2) | (coef->Cr_blue & MCDE_RGB_MASK2));

    ch_x_reg->mcde_ch_rgbconv4 =
        (
            (ch_x_reg->mcde_ch_rgbconv4 &~MCDE_RGB_MASK1) |
            ((coef->Cb_red << MCDE_RGB_SHIFT) & MCDE_RGB_MASK1)
        );

    ch_x_reg->mcde_ch_rgbconv4 = ((ch_x_reg->mcde_ch_rgbconv4 &~MCDE_RGB_MASK2) | (coef->Cb_green & MCDE_RGB_MASK2));

    ch_x_reg->mcde_ch_rgbconv5 =
        (
            (ch_x_reg->mcde_ch_rgbconv5 &~MCDE_RGB_MASK1) |
            ((coef->Cb_blue << MCDE_RGB_SHIFT) & MCDE_RGB_MASK1)
        );

    ch_x_reg->mcde_ch_rgbconv5 = ((ch_x_reg->mcde_ch_rgbconv5 &~MCDE_RGB_MASK2) | (coef->Off_red & MCDE_RGB_MASK2));

    ch_x_reg->mcde_ch_rgbconv6 =
        (
            (ch_x_reg->mcde_ch_rgbconv6 &~MCDE_RGB_MASK1) |
            ((coef->Off_green << MCDE_RGB_SHIFT) & MCDE_RGB_MASK1)
        );

    ch_x_reg->mcde_ch_rgbconv6 = ((ch_x_reg->mcde_ch_rgbconv6 &~MCDE_RGB_MASK2) | (coef->Off_blue & MCDE_RGB_MASK2));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetColorConversionMatrix()							 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the value of coefficients of     */
/*				    color conversion matrix                                  */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_rgb_conv_coef *coef:color conversion matrix structure*/
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetColorConversionMatrix(IN t_mcde_ch_id channel, IN t_mcde_rgb_conv_coef *p_coef)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_coef)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_coef->Yr_red = (t_uint16) ((ch_x_reg->mcde_ch_rgbconv1 & MCDE_RGB_MASK1) >> MCDE_RGB_SHIFT);

    p_coef->Yr_green = (t_uint16) (ch_x_reg->mcde_ch_rgbconv1 & MCDE_RGB_MASK2);

    p_coef->Yr_blue = (t_uint16) ((ch_x_reg->mcde_ch_rgbconv2 & MCDE_RGB_MASK1) >> MCDE_RGB_SHIFT);

    p_coef->Cr_red = (t_uint16) (ch_x_reg->mcde_ch_rgbconv2 & MCDE_RGB_MASK2);

    p_coef->Cr_green = (t_uint16) ((ch_x_reg->mcde_ch_rgbconv3 & MCDE_RGB_MASK1) >> MCDE_RGB_SHIFT);

    p_coef->Cr_blue = (t_uint16) (ch_x_reg->mcde_ch_rgbconv3 & MCDE_RGB_MASK2);

    p_coef->Cb_red = (t_uint16) ((ch_x_reg->mcde_ch_rgbconv4 & MCDE_RGB_MASK1) >> MCDE_RGB_SHIFT);

    p_coef->Cb_green = (t_uint16) (ch_x_reg->mcde_ch_rgbconv4 & MCDE_RGB_MASK2);

    p_coef->Cb_blue = (t_uint16) ((ch_x_reg->mcde_ch_rgbconv5 & MCDE_RGB_MASK1) >> MCDE_RGB_SHIFT);

    p_coef->Off_red = (t_uint16) (ch_x_reg->mcde_ch_rgbconv5 & MCDE_RGB_MASK2);

    p_coef->Off_green = (t_uint16) ((ch_x_reg->mcde_ch_rgbconv6 & MCDE_RGB_MASK1) >> MCDE_RGB_SHIFT);

    p_coef->Off_blue = (t_uint16) (ch_x_reg->mcde_ch_rgbconv6 & MCDE_RGB_MASK2);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetFlickerFilterCoefficient()					     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the value of coefficients of     */
/*				    flicker filter along with the threshold                  */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_filter_coef coef:flicker filter coef. structure      */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetFlickerFilterCoefficient(IN t_mcde_ch_id channel, IN t_mcde_filter_coef *coef)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_ffcoef0 =
        (
            (ch_x_reg->mcde_ch_ffcoef0 &~MCDE_THRESHOLD_MASK) |
            ((coef->threshold_ctrl0 << MCDE_THRESHOLD_SHIFT) & MCDE_THRESHOLD_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef0 =
        (
            (ch_x_reg->mcde_ch_ffcoef0 &~MCDE_COEFFN3_MASK) |
            ((coef->Coeff0_N3 << MCDE_COEFFN3_SHIFT) & MCDE_COEFFN3_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef0 =
        (
            (ch_x_reg->mcde_ch_ffcoef0 &~MCDE_COEFFN2_MASK) |
            ((coef->Coeff0_N2 << MCDE_COEFFN2_SHIFT) & MCDE_COEFFN2_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef0 =
        (
            (ch_x_reg->mcde_ch_ffcoef0 &~MCDE_COEFFN1_MASK) |
            (coef->Coeff0_N1 & MCDE_COEFFN1_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef1 =
        (
            (ch_x_reg->mcde_ch_ffcoef1 &~MCDE_THRESHOLD_MASK) |
            ((coef->threshold_ctrl1 << MCDE_THRESHOLD_SHIFT) & MCDE_THRESHOLD_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef1 =
        (
            (ch_x_reg->mcde_ch_ffcoef1 &~MCDE_COEFFN3_MASK) |
            ((coef->Coeff1_N3 << MCDE_COEFFN3_SHIFT) & MCDE_COEFFN3_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef1 =
        (
            (ch_x_reg->mcde_ch_ffcoef1 &~MCDE_COEFFN2_MASK) |
            ((coef->Coeff1_N2 << MCDE_COEFFN2_SHIFT) & MCDE_COEFFN2_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef1 =
        (
            (ch_x_reg->mcde_ch_ffcoef1 &~MCDE_COEFFN1_MASK) |
            (coef->Coeff1_N1 & MCDE_COEFFN1_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef2 =
        (
            (ch_x_reg->mcde_ch_ffcoef2 &~MCDE_THRESHOLD_MASK) |
            ((coef->threshold_ctrl2 << MCDE_THRESHOLD_SHIFT) & MCDE_THRESHOLD_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef2 =
        (
            (ch_x_reg->mcde_ch_ffcoef2 &~MCDE_COEFFN3_MASK) |
            ((coef->Coeff2_N3 << MCDE_COEFFN3_SHIFT) & MCDE_COEFFN3_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef2 =
        (
            (ch_x_reg->mcde_ch_ffcoef2 &~MCDE_COEFFN2_MASK) |
            ((coef->Coeff2_N2 << MCDE_COEFFN2_SHIFT) & MCDE_COEFFN2_MASK)
        );

    ch_x_reg->mcde_ch_ffcoef2 =
        (
            (ch_x_reg->mcde_ch_ffcoef2 &~MCDE_COEFFN1_MASK) |
            (coef->Coeff2_N1 & MCDE_COEFFN1_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetFlickerFilterCoefficient()					     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the value of coefficients of     */
/*				    flicker filter along with the threshold                  */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_filter_coef coef:flicker filter coef. structure      */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetFlickerFilterCoefficient(IN t_mcde_ch_id channel, IN t_mcde_filter_coef *p_coef)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_coef)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_coef->threshold_ctrl0 = (t_uint8) ((ch_x_reg->mcde_ch_ffcoef0 & MCDE_THRESHOLD_MASK) >> MCDE_THRESHOLD_SHIFT);

    p_coef->Coeff0_N3 = (t_uint8) ((ch_x_reg->mcde_ch_ffcoef0 & MCDE_COEFFN3_MASK) >> MCDE_COEFFN3_SHIFT);

    p_coef->Coeff0_N2 = (t_uint8) ((ch_x_reg->mcde_ch_ffcoef0 & MCDE_COEFFN2_MASK) >> MCDE_COEFFN2_SHIFT);

    p_coef->Coeff0_N1 = (t_uint8) (ch_x_reg->mcde_ch_ffcoef0 & MCDE_COEFFN1_MASK);

    p_coef->threshold_ctrl1 = (t_uint8) ((ch_x_reg->mcde_ch_ffcoef1 & MCDE_THRESHOLD_MASK) >> MCDE_THRESHOLD_SHIFT);

    p_coef->Coeff1_N3 = (t_uint8) ((ch_x_reg->mcde_ch_ffcoef1 & MCDE_COEFFN3_MASK) >> MCDE_COEFFN3_SHIFT);

    p_coef->Coeff1_N2 = (t_uint8) ((ch_x_reg->mcde_ch_ffcoef1 & MCDE_COEFFN2_MASK) >> MCDE_COEFFN2_SHIFT);

    p_coef->Coeff1_N1 = (t_uint8) (ch_x_reg->mcde_ch_ffcoef1 & MCDE_COEFFN1_MASK);

    p_coef->threshold_ctrl2 = (t_uint8) ((ch_x_reg->mcde_ch_ffcoef2 & MCDE_THRESHOLD_MASK) >> MCDE_THRESHOLD_SHIFT);

    p_coef->Coeff2_N3 = (t_uint8) ((ch_x_reg->mcde_ch_ffcoef2 & MCDE_COEFFN3_MASK) >> MCDE_COEFFN3_SHIFT);

    p_coef->Coeff2_N2 = (t_uint8) ((ch_x_reg->mcde_ch_ffcoef2 & MCDE_COEFFN2_MASK) >> MCDE_COEFFN2_SHIFT);

    p_coef->Coeff2_N1 = (t_uint8) (ch_x_reg->mcde_ch_ffcoef2 & MCDE_COEFFN1_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetTVControl()					    				 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the line number and type of input*/
/*				    (progressive/interlaced) in TV mode.                     */
/*                  IN LCD mode ,it sets the lines per panel                 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_control control:TV control structure              */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetTVControl(IN t_mcde_ch_id channel, IN t_mcde_tv_control *control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_tvcr =
        (
            (ch_x_reg->mcde_ch_tvcr &~MCDE_TV_LINES_MASK) |
            ((control->num_lines << MCDE_TV_LINES_SHIFT) & MCDE_TV_LINES_MASK)
        );

    ch_x_reg->mcde_ch_tvcr =
        (
            (ch_x_reg->mcde_ch_tvcr &~MCDE_TVMODE_MASK) |
            (((t_uint32) control->tv_mode << MCDE_TVMODE_SHIFT) & MCDE_TVMODE_MASK)
        );

    ch_x_reg->mcde_ch_tvcr =
        (
            (ch_x_reg->mcde_ch_tvcr &~MCDE_IFIELD_MASK) |
            (((t_uint32) control->ifield << MCDE_IFIELD_SHIFT) & MCDE_IFIELD_MASK)
        );

    ch_x_reg->mcde_ch_tvcr =
        (
            (ch_x_reg->mcde_ch_tvcr &~MCDE_INTEREN_MASK) |
            (((t_uint32) control->interlace_enable << MCDE_INTEREN_SHIFT) & MCDE_INTEREN_MASK)
        );

    ch_x_reg->mcde_ch_tvcr =
        (
            (ch_x_reg->mcde_ch_tvcr &~MCDE_SELMODE_MASK) |
            ((t_uint32) control->sel_mode & MCDE_SELMODE_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetTVControl()					    				 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the line number and type of input*/
/*				    (progressive/interlaced) in TV mode.                     */
/*                  IN LCD mode ,it sets the lines per panel                 */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_control control:TV control structure              */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetTVControl(IN t_mcde_ch_id channel, IN t_mcde_tv_control *p_control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_control)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_control->num_lines = (t_uint16) ((ch_x_reg->mcde_ch_tvcr & MCDE_TV_LINES_MASK) >> MCDE_TV_LINES_SHIFT);

    p_control->tv_mode = (t_mcde_tvmode) ((ch_x_reg->mcde_ch_tvcr & MCDE_TVMODE_MASK) >> MCDE_TVMODE_SHIFT);

    p_control->ifield = (t_mcde_signal_level) ((ch_x_reg->mcde_ch_tvcr & MCDE_IFIELD_MASK) >> MCDE_IFIELD_SHIFT);

    p_control->interlace_enable = (t_mcde_state) ((ch_x_reg->mcde_ch_tvcr & MCDE_INTEREN_MASK) >> MCDE_INTEREN_SHIFT);

    p_control->sel_mode = (t_mcde_display_mode) (ch_x_reg->mcde_ch_tvcr & MCDE_SELMODE_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetField1Blanking()					             */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the blanking of field1 in TV mode*/
/*                  IN LCD mode ,it sets the vertical timing parameters      */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_blanking_field:blanking control structure         */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetField1Blanking(IN t_mcde_ch_id channel, IN t_mcde_tv_blanking_field *blanking)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_tvbl1 =
        (
            (ch_x_reg->mcde_ch_tvbl1 &~MCDE_BSL_MASK) |
            ((blanking->blanking_start << MCDE_BSL_SHIFT) & MCDE_BSL_MASK)
        );

    ch_x_reg->mcde_ch_tvbl1 = ((ch_x_reg->mcde_ch_tvbl1 &~MCDE_BEL_MASK) | (blanking->blanking_end & MCDE_BEL_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetField1Blanking()					             */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the blanking of field1 in TV mode*/
/*                  IN LCD mode ,it sets the vertical timing parameters      */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_tv_blanking_field *:blanking control structure       */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetField1Blanking(IN t_mcde_ch_id channel, IN t_mcde_tv_blanking_field *p_blanking)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_blanking)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_blanking->blanking_start = (t_uint16) ((ch_x_reg->mcde_ch_tvbl1 & MCDE_BSL_MASK) >> MCDE_BSL_SHIFT);

    p_blanking->blanking_end = (t_uint16) (ch_x_reg->mcde_ch_tvbl1 & MCDE_BEL_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetField2Blanking()					             */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the blanking of field2 in TV mode*/
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_blanking_field:blanking control structure         */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetField2Blanking(IN t_mcde_ch_id channel, IN t_mcde_tv_blanking_field *blanking)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_tvbl2 =
        (
            (ch_x_reg->mcde_ch_tvbl2 &~MCDE_BSL_MASK) |
            ((blanking->blanking_start << MCDE_BSL_SHIFT) & MCDE_BSL_MASK)
        );

    ch_x_reg->mcde_ch_tvbl2 = ((ch_x_reg->mcde_ch_tvbl2 &~MCDE_BEL_MASK) | (blanking->blanking_end & MCDE_BEL_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetField2Blanking()					             */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the blanking of field2 in TV mode*/
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_tv_blanking_field *:blanking control structure       */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetField2Blanking(IN t_mcde_ch_id channel, IN t_mcde_tv_blanking_field *p_blanking)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_blanking)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_blanking->blanking_start = (t_uint16) ((ch_x_reg->mcde_ch_tvbl2 & MCDE_BSL_MASK) >> MCDE_BSL_SHIFT);

    p_blanking->blanking_end = (t_uint16) (ch_x_reg->mcde_ch_tvbl2 & MCDE_BEL_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetTVStartLine()					                 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the start line for channelX in TV*/
/*                  mode.This controls vertical timing parameter             */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_start_line:start line structure                   */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetTVStartLine(IN t_mcde_ch_id channel, IN t_mcde_tv_start_line *start_line)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_tvisl =
        (
            (ch_x_reg->mcde_ch_tvisl &~MCDE_FSL2_MASK) |
            ((start_line->field2_start_line << MCDE_FSL2_SHIFT) & MCDE_FSL2_MASK)
        );

    ch_x_reg->mcde_ch_tvisl =
        (
            (ch_x_reg->mcde_ch_tvisl &~MCDE_FSL1_MASK) |
            (start_line->field1_start_line & MCDE_FSL1_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetTVStartLine()					                 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the start line for channelX in TV*/
/*                  mode.This controls vertical timing parameter             */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_tv_start_line *:start line structure                 */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetTVStartLine(IN t_mcde_ch_id channel, IN t_mcde_tv_start_line *p_start_line)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_start_line)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_start_line->field2_start_line = (t_uint16) ((ch_x_reg->mcde_ch_tvisl & MCDE_FSL2_MASK) >> MCDE_FSL2_SHIFT);

    p_start_line->field1_start_line = (t_uint16) (ch_x_reg->mcde_ch_tvisl & MCDE_FSL1_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetTVVerticalOffset()					             */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the vertical offset for filed1   */
/*                  and field2 for channelX                                  */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_dvo_offset:vertical offset structure              */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetTVVerticalOffset(IN t_mcde_ch_id channel, IN t_mcde_tv_dvo_offset *offset)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_tvdvo =
        (
            (ch_x_reg->mcde_ch_tvdvo &~MCDE_DVO2_MASK) |
            ((offset->field2_window_offset << MCDE_DVO2_SHIFT) & MCDE_DVO2_MASK)
        );

    ch_x_reg->mcde_ch_tvdvo =
        (
            (ch_x_reg->mcde_ch_tvdvo &~MCDE_DVO1_MASK) |
            (offset->field1_window_offset & MCDE_DVO1_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetTVVerticalOffset()					             */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the vertical offset for filed1   */
/*                  and field2 for channelX                                  */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_tv_dvo_offset *:vertical offset structure            */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetTVVerticalOffset(IN t_mcde_ch_id channel, IN t_mcde_tv_dvo_offset *p_offset)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_offset)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_offset->field2_window_offset = (t_uint16) ((ch_x_reg->mcde_ch_tvdvo & MCDE_DVO2_MASK) >> MCDE_DVO2_SHIFT);

    p_offset->field1_window_offset = (t_uint16) (ch_x_reg->mcde_ch_tvdvo & MCDE_DVO1_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetTVSourceWindowHeight()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to set the source window height for     */
/*                  bottom field for channelX                                */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_swh_time:window height structure                  */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetTVSourceWindowHeight(IN t_mcde_ch_id channel, IN t_mcde_tv_swh_time *swh)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_tvswh =
        (
            (ch_x_reg->mcde_ch_tvswh &~MCDE_DVO2_MASK) |
            ((swh->tv_swh2 << MCDE_DVO2_SHIFT) & MCDE_DVO2_MASK)
        );

    ch_x_reg->mcde_ch_tvswh = ((ch_x_reg->mcde_ch_tvswh &~MCDE_DVO1_MASK) | (swh->tv_swh1 & MCDE_DVO1_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetTVSourceWindowHeight()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This API is used to get the source window height for     */
/*                  bottom field for channelX                                */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_tv_swh_time:window height structure                  */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetTVSourceWindowHeight(IN t_mcde_ch_id channel, IN t_mcde_tv_swh_time *p_swh)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_swh)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_swh->tv_swh2 = (t_uint16) ((ch_x_reg->mcde_ch_tvswh & MCDE_DVO2_MASK) >> MCDE_DVO2_SHIFT);

    p_swh->tv_swh1 = (t_uint16) (ch_x_reg->mcde_ch_tvswh & MCDE_DVO1_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetTVSourceWindowWidth()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the source window width and       */
/*                 horizontal offset in TV mode                              */
/*                 In LCD mode,it is used to set horizontal timing parameters*/
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_timing1:control structure                         */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetTVSourceWindowWidth(IN t_mcde_ch_id channel, IN t_mcde_tv_timing1 *sww)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_tvtim1 =
        (
            (ch_x_reg->mcde_ch_tvtim1 &~MCDE_SWW_MASK) |
            ((sww->src_window_width << MCDE_SWW_SHIFT) & MCDE_SWW_MASK)
        );

    ch_x_reg->mcde_ch_tvtim1 =
        (
            (ch_x_reg->mcde_ch_tvtim1 &~MCDE_DHO_MASK) |
            (sww->destination_hor_offset & MCDE_DHO_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetTVSourceWindowWidth()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the source window width and       */
/*                 horizontal offset in TV mode                              */
/*                 In LCD mode,it is used to get horizontal timing parameters*/
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_tv_timing1 *:control structure                       */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetTVSourceWindowWidth(IN t_mcde_ch_id channel, IN t_mcde_tv_timing1 *p_sww)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_sww)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_sww->src_window_width = (t_uint16) ((ch_x_reg->mcde_ch_tvtim1 & MCDE_SWW_MASK) >> MCDE_SWW_SHIFT);

    p_sww->destination_hor_offset = (t_uint16) (ch_x_reg->mcde_ch_tvtim1 & MCDE_DHO_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetTVModeLineBlanking()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the line blanking and active line */
/*                 width in TV mode                                          */
/*                 In LCD mode,it is used to set horizontal timing parameters*/
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_lbalw_timing:control structure                    */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetTVModeLineBlanking(IN t_mcde_ch_id channel, IN t_mcde_tv_lbalw_timing *balw)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_tvbalw =
        (
            (ch_x_reg->mcde_ch_tvbalw &~MCDE_ALW_MASK) |
            ((balw->active_line_width << MCDE_ALW_SHIFT) & MCDE_ALW_MASK)
        );

    ch_x_reg->mcde_ch_tvbalw =
        (
            (ch_x_reg->mcde_ch_tvbalw &~MCDE_LBW_MASK) |
            (balw->line_blanking_width & MCDE_LBW_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetTVModeLineBlanking()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the line blanking and active line */
/*                 width in TV mode                                          */
/*                 In LCD mode,it is used to get horizontal timing parameters*/
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_tv_lbalw_timing *:control structure                  */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetTVModeLineBlanking(IN t_mcde_ch_id channel, IN t_mcde_tv_lbalw_timing *p_balw)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    p_balw->active_line_width = (t_uint16) ((ch_x_reg->mcde_ch_tvbalw & MCDE_ALW_MASK) >> MCDE_ALW_SHIFT);

    ch_x_reg->mcde_ch_tvbalw = (t_uint16) (ch_x_reg->mcde_ch_tvbalw & MCDE_LBW_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetChannelXBackgroundColor()					     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the color background for channel X*/
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_tv_background_time:background color structure        */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetChannelXBackgroundColor(IN t_mcde_ch_id channel, IN t_mcde_tv_background_time *color)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_tvblu =
        (
            (ch_x_reg->mcde_ch_tvblu &~MCDE_TVBCR_MASK) |
            ((color->background_cr << MCDE_TVBCR_SHIFT) & MCDE_TVBCR_MASK)
        );

    ch_x_reg->mcde_ch_tvblu =
        (
            (ch_x_reg->mcde_ch_tvblu &~MCDE_TVBCB_MASK) |
            ((color->background_cb << MCDE_TVBCB_SHIFT) & MCDE_TVBCB_MASK)
        );

    ch_x_reg->mcde_ch_tvblu = ((ch_x_reg->mcde_ch_tvblu &~MCDE_TVBLU_MASK) | (color->background_lu & MCDE_TVBLU_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetChannelXBackgroundColor()					     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the color background for channel X*/
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_tv_background_time *:background color structure      */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetChannelXBackgroundColor(IN t_mcde_ch_id channel, IN t_mcde_tv_background_time *p_color)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_color)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_color->background_cr = (t_uint8) ((ch_x_reg->mcde_ch_tvblu & MCDE_TVBCR_MASK) >> MCDE_TVBCR_SHIFT);

    p_color->background_cb = (t_uint8) ((ch_x_reg->mcde_ch_tvblu & MCDE_TVBCB_MASK) >> MCDE_TVBCB_SHIFT);

    p_color->background_lu = (t_uint8) (ch_x_reg->mcde_ch_tvblu & MCDE_TVBLU_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetLCDControl()					                 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the timing parameters specific to */
/*                 HR-TFT LCDs                                               */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_lcd_timing0:timing control parameters                */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetLCDControl(IN t_mcde_ch_id channel, IN t_mcde_lcd_timing0 *control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_REVVAEN_MASK) |
            (((t_uint32) control->rev_va_enable << MCDE_REVVAEN_SHIFT) & MCDE_REVVAEN_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_REVTGEN_MASK) |
            (((t_uint32) control->rev_toggle_enable << MCDE_REVTGEN_SHIFT) & MCDE_REVTGEN_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_REVLOADSEL_MASK) |
            (((t_uint32) control->rev_sync_sel << MCDE_REVLOADSEL_SHIFT) & MCDE_REVLOADSEL_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_REVDEL1_MASK) |
            (((t_uint32) control->rev_delay1 << MCDE_REVDEL1_SHIFT) & MCDE_REVDEL1_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_REVDEL0_MASK) |
            (((t_uint32) control->rev_delay0 << MCDE_REVDEL0_SHIFT) & MCDE_REVDEL0_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_PSVAEN_MASK) |
            (((t_uint32) control->ps_va_enable << MCDE_PSVAEN_SHIFT) & MCDE_PSVAEN_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_PSTGEN_MASK) |
            (((t_uint32) control->ps_toggle_enable << MCDE_PSTGEN_SHIFT) & MCDE_PSTGEN_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_PSLOADSEL_MASK) |
            (((t_uint32) control->ps_sync_sel << MCDE_PSLOADSEL_SHIFT) & MCDE_PSLOADSEL_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_PSDEL1_MASK) |
            (((t_uint32) control->ps_delay1 << MCDE_PSDEL1_SHIFT) & MCDE_PSDEL1_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim0 =
        (
            (ch_x_reg->mcde_ch_lcdtim0 &~MCDE_PSDEL0_MASK) |
            ((t_uint32) control->ps_delay0 & MCDE_PSDEL0_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetLCDControl()					                 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the timing parameters specific to */
/*                 HR-TFT LCDs                                               */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_lcd_timing0:timing control parameters                */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetLCDControl(IN t_mcde_ch_id channel, IN t_mcde_lcd_timing0 *p_control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_control)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_control->rev_va_enable = (t_mcde_va_enable) ((ch_x_reg->mcde_ch_lcdtim0 & MCDE_REVVAEN_MASK) >> MCDE_REVVAEN_SHIFT);

    p_control->rev_toggle_enable = (t_mcde_toggle_enable) ((ch_x_reg->mcde_ch_lcdtim0 & MCDE_REVTGEN_MASK) >> MCDE_REVTGEN_SHIFT);

    p_control->rev_sync_sel = (t_mcde_loadsel) ((ch_x_reg->mcde_ch_lcdtim0 & MCDE_REVLOADSEL_MASK) >> MCDE_REVLOADSEL_SHIFT);

    p_control->rev_delay1 = (t_uint8) ((ch_x_reg->mcde_ch_lcdtim0 & MCDE_REVDEL1_MASK) >> MCDE_REVDEL1_SHIFT);

    p_control->rev_delay0 = (t_uint8) ((ch_x_reg->mcde_ch_lcdtim0 & MCDE_REVDEL0_MASK) >> MCDE_REVDEL0_SHIFT);

    p_control->ps_va_enable = (t_mcde_va_enable) ((ch_x_reg->mcde_ch_lcdtim0 & MCDE_PSVAEN_MASK) >> MCDE_PSVAEN_SHIFT);

    p_control->ps_toggle_enable = (t_mcde_toggle_enable) ((ch_x_reg->mcde_ch_lcdtim0 & MCDE_PSTGEN_MASK) >> MCDE_PSTGEN_SHIFT);

    p_control->ps_sync_sel = (t_mcde_loadsel) ((ch_x_reg->mcde_ch_lcdtim0 & MCDE_PSLOADSEL_MASK) >> MCDE_PSLOADSEL_SHIFT);

    p_control->ps_delay1 = (t_uint8) ((ch_x_reg->mcde_ch_lcdtim0 & MCDE_PSDEL1_MASK) >> MCDE_PSDEL1_SHIFT);

    p_control->ps_delay0 = (t_uint8) (ch_x_reg->mcde_ch_lcdtim0 & MCDE_PSDEL0_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetLCDTimingControl()					             */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the timing parameters like SPL    */
/*                 delay,LCDGEN1 and LCDGEN2 outputs for Channel A/B         */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_lcd_timing1:timing control parameters                */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetLCDTimingControl(IN t_mcde_ch_id channel, IN t_mcde_lcd_timing1 *control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_IOE_MASK) |
            (((t_uint32) control->io_enable << MCDE_IOE_SHIFT) & MCDE_IOE_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_IPC_MASK) |
            (((t_uint32) control->ipc << MCDE_IPC_SHIFT) & MCDE_IPC_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_IHS_MASK) |
            (((t_uint32) control->ihs << MCDE_IHS_SHIFT) & MCDE_IPC_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_IVS_MASK) |
            (((t_uint32) control->ivs << MCDE_IVS_SHIFT) & MCDE_IVS_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_IVP_MASK) |
            (((t_uint32) control->ivp << MCDE_IVP_SHIFT) & MCDE_IPC_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_ICLSPL_MASK) |
            (((t_uint32) control->iclspl << MCDE_ICLSPL_SHIFT) & MCDE_ICLSPL_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_ICLREV_MASK) |
            (((t_uint32) control->iclrev << MCDE_ICLREV_SHIFT) & MCDE_ICLREV_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_ICLSP_MASK) |
            (((t_uint32) control->iclsp << MCDE_ICLSP_SHIFT) & MCDE_ICLSP_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_SPLVAEN_MASK) |
            (((t_uint32) control->mcde_spl << MCDE_SPLVAEN_SHIFT) & MCDE_SPLVAEN_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_SPLTGEN_MASK) |
            (((t_uint32) control->spltgen << MCDE_SPLTGEN_SHIFT) & MCDE_SPLTGEN_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_SPLLOADSEL_MASK) |
            (((t_uint32) control->spl_sync_sel << MCDE_SPLLOADSEL_SHIFT) & MCDE_SPLLOADSEL_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_SPLDEL1_MASK) |
            (((t_uint32) control->spl_delay1 << MCDE_SPLDEL1_SHIFT) & MCDE_SPLDEL1_MASK)
        );

    ch_x_reg->mcde_ch_lcdtim1 =
        (
            (ch_x_reg->mcde_ch_lcdtim1 &~MCDE_SPLDEL0_MASK) |
            ((t_uint32) control->spl_delay0 & MCDE_SPLDEL0_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetLCDTimingControl()					             */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the timing parameters like SPL    */
/*                 delay,LCDGEN1 and LCDGEN2 outputs for Channel A/B         */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_lcd_timing1 *:timing control parameters              */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetLCDTimingControl(IN t_mcde_ch_id channel, IN t_mcde_lcd_timing1 *p_control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_control)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_control->io_enable = (t_mcde_signal_level) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_IOE_MASK) >> MCDE_IOE_SHIFT);

    p_control->ipc = (t_mcde_data_lines) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_IPC_MASK) >> MCDE_IPC_SHIFT);

    p_control->ihs = (t_mcde_signal_level) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_IHS_MASK) >> MCDE_IHS_SHIFT);

    p_control->ivs = (t_mcde_signal_level) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_IVS_MASK) >> MCDE_IVS_SHIFT);

    p_control->ivp = (t_mcde_signal_level) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_IVP_MASK) >> MCDE_IVP_SHIFT);

    p_control->iclspl = (t_mcde_signal_level) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_ICLSPL_MASK) >> MCDE_ICLSPL_SHIFT);

    p_control->iclrev = (t_mcde_signal_level) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_ICLREV_MASK) >> MCDE_ICLREV_SHIFT);

    p_control->iclsp = (t_mcde_signal_level) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_ICLSP_MASK) >> MCDE_ICLSP_SHIFT);

    p_control->mcde_spl = (t_mcde_va_enable) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_SPLVAEN_MASK) >> MCDE_SPLVAEN_SHIFT);

    p_control->spltgen = (t_mcde_toggle_enable) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_SPLTGEN_MASK) >> MCDE_SPLTGEN_SHIFT);

    p_control->spl_sync_sel = (t_mcde_loadsel) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_SPLLOADSEL_MASK) >> MCDE_SPLLOADSEL_SHIFT);

    p_control->spl_delay1 = (t_uint8) (t_mcde_loadsel) ((ch_x_reg->mcde_ch_lcdtim1 & MCDE_SPLDEL1_MASK) >> MCDE_SPLDEL1_SHIFT);

    p_control->spl_delay0 = (t_uint8) (ch_x_reg->mcde_ch_lcdtim1 & MCDE_SPLDEL0_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetDitherControl()					                 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the dithering control parameters  */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_dither_ctrl:dither control parameters                */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetDitherControl(IN t_mcde_ch_id channel, IN t_mcde_dither_ctrl *control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_ditctrl =
        (
            (ch_x_reg->mcde_ch_ditctrl &~MCDE_FOFFY_MASK) |
            ((control->y_offset << MCDE_FOFFY_SHIFT) & MCDE_FOFFY_MASK)
        );

    ch_x_reg->mcde_ch_ditctrl =
        (
            (ch_x_reg->mcde_ch_ditctrl &~MCDE_FOFFX_MASK) |
            ((control->x_offset << MCDE_FOFFX_SHIFT) & MCDE_FOFFX_MASK)
        );

    ch_x_reg->mcde_ch_ditctrl =
        (
            (ch_x_reg->mcde_ch_ditctrl &~MCDE_MASK_BITCTRL_MASK) |
            (((t_uint32) control->masking_ctrl << MCDE_MASK_BITCTRL_SHIFT) & MCDE_MASK_BITCTRL_MASK)
        );

    ch_x_reg->mcde_ch_ditctrl =
        (
            (ch_x_reg->mcde_ch_ditctrl &~MCDE_MODE_MASK) |
            ((control->mode << MCDE_MODE_SHIFT) & MCDE_MODE_MASK)
        );

    ch_x_reg->mcde_ch_ditctrl =
        (
            (ch_x_reg->mcde_ch_ditctrl &~MCDE_COMP_MASK) |
            (((t_uint32) control->comp_dithering << MCDE_COMP_SHIFT) & MCDE_COMP_MASK)
        );

    ch_x_reg->mcde_ch_ditctrl =
        (
            (ch_x_reg->mcde_ch_ditctrl &~MCDE_TEMP_MASK) |
            ((t_uint32) control->temp_dithering & MCDE_TEMP_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetDitherControl()					                 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the dithering control parameters  */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_dither_ctrl *:dither control parameters              */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetDitherControl(IN t_mcde_ch_id channel, IN t_mcde_dither_ctrl *p_control)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_control)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_control->y_offset = (t_uint8) ((ch_x_reg->mcde_ch_ditctrl & MCDE_FOFFY_MASK) >> MCDE_FOFFY_SHIFT);

    p_control->x_offset = (t_uint8) ((ch_x_reg->mcde_ch_ditctrl & MCDE_FOFFX_MASK) >> MCDE_FOFFX_SHIFT);

    p_control->masking_ctrl = (t_mcde_masking_bit_ctrl) ((ch_x_reg->mcde_ch_ditctrl & MCDE_MASK_BITCTRL_MASK) >> MCDE_MASK_BITCTRL_SHIFT);

    p_control->mode = (t_uint8) ((ch_x_reg->mcde_ch_ditctrl & MCDE_MODE_MASK) >> MCDE_MODE_SHIFT);

    p_control->comp_dithering = (t_mcde_dithering_ctrl) ((ch_x_reg->mcde_ch_ditctrl & MCDE_COMP_MASK) >> MCDE_COMP_SHIFT);

    p_control->temp_dithering = (t_mcde_dithering_ctrl) (ch_x_reg->mcde_ch_ditctrl & MCDE_TEMP_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetDitherOffset()					                 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the dithering offset  parameters  */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_dithering_offset:dither offset parameters            */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetDitherOffset(IN t_mcde_ch_id channel, IN t_mcde_dithering_offset *offset)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    ch_x_reg->mcde_ch_ditctrl =
        (
            (ch_x_reg->mcde_ch_ditctrl &~MCDE_YB_MASK) |
            ((offset->y_offset_rb << MCDE_YB_SHIFT) & MCDE_YB_MASK)
        );

    ch_x_reg->mcde_ch_ditctrl =
        (
            (ch_x_reg->mcde_ch_ditctrl &~MCDE_XB_MASK) |
            ((offset->x_offset_rb << MCDE_XB_SHIFT) & MCDE_XB_MASK)
        );

    ch_x_reg->mcde_ch_ditctrl =
        (
            (ch_x_reg->mcde_ch_ditctrl &~MCDE_YG_MASK) |
            ((offset->y_offset_rg << MCDE_YG_SHIFT) & MCDE_YG_MASK)
        );

    ch_x_reg->mcde_ch_ditctrl = ((ch_x_reg->mcde_ch_ditctrl &~MCDE_XG_MASK) | (offset->x_offset_rg & MCDE_XG_MASK));

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetDitherOffset()					                 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the dithering offset  parameters  */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_dithering_offset *:dither offset parameters          */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetDitherOffset(IN t_mcde_ch_id channel, IN t_mcde_dithering_offset *p_offset)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_offset)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    p_offset->y_offset_rb = (t_uint8) ((ch_x_reg->mcde_ch_ditctrl & MCDE_YB_MASK) >> MCDE_YB_SHIFT);

    p_offset->x_offset_rb = (t_uint8) ((ch_x_reg->mcde_ch_ditctrl & MCDE_XB_MASK) >> MCDE_XB_SHIFT);

    p_offset->y_offset_rg = (t_uint8) ((ch_x_reg->mcde_ch_ditctrl & MCDE_YG_MASK) >> MCDE_YG_SHIFT);

    p_offset->x_offset_rg = (t_uint8) (ch_x_reg->mcde_ch_ditctrl & MCDE_XG_MASK);

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetPalette()					                     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the palette coefficients          */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_palette:palette coefficients parameters              */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetPalette(IN t_mcde_ch_id channel, IN t_mcde_palette *palette)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }
    
    #ifdef ST_8500ED
    
        ch_x_reg->mcde_ch_pal =
        (
            (ch_x_reg->mcde_ch_pal &~MCDE_ARED_MASK) |
            ((palette->alphared << MCDE_ARED_SHIFT) & MCDE_ARED_MASK)
        );

/*    ch_x_reg->mcde_ch_pal =
        (
            (ch_x_reg->mcde_ch_pal &~MCDE_GREEN_MASK) |
            ((palette->green << MCDE_GREEN_SHIFT) & MCDE_GREEN_MASK)
        );
*/
	ch_x_reg->mcde_ch_pal =
		(
			(ch_x_reg->mcde_ch_pal &~MCDE_GREEN_MASK) |
		);


    ch_x_reg->mcde_ch_pal = ((ch_x_reg->mcde_ch_pal &~MCDE_BLUE_MASK) | (palette->blue & MCDE_BLUE_MASK));
    
    #else

    ch_x_reg->mcde_ch_pal1 =
        (
            (ch_x_reg->mcde_ch_pal1 &~MCDE_ARED_MASK) |
            ((palette->alphared << MCDE_ARED_SHIFT) & MCDE_ARED_MASK)
        );

    ch_x_reg->mcde_ch_pal0 =
        (
            (ch_x_reg->mcde_ch_pal0 &~MCDE_GREEN_MASK) |
            ((palette->green << MCDE_GREEN_SHIFT) & MCDE_GREEN_MASK)
        );

    ch_x_reg->mcde_ch_pal0 = ((ch_x_reg->mcde_ch_pal0 &~MCDE_BLUE_MASK) | (palette->blue & MCDE_BLUE_MASK));
    
    #endif
    
    DBGEXIT0(error);

    return(error);
}
/*****************************************************************************/
/* NAME			:	MCDE_GetPalette()					                     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the palette coefficients          */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_palette *:palette coefficients parameters            */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetPalette(IN t_mcde_ch_id channel, IN t_mcde_palette *p_palette)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_palette)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
	
	#ifdef ST_8500ED
	
    p_palette->alphared = (t_uint16) ((ch_x_reg->mcde_ch_pal & MCDE_ARED_MASK) >> MCDE_ARED_SHIFT);

    p_palette->green = (t_uint8) ((ch_x_reg->mcde_ch_pal & MCDE_GREEN_MASK) >> MCDE_GREEN_SHIFT);

    p_palette->blue = (t_uint8) (ch_x_reg->mcde_ch_pal & MCDE_BLUE_MASK);
    
    #else
    
    p_palette->alphared = (t_uint16) ((ch_x_reg->mcde_ch_pal1 & MCDE_ARED_MASK) >> MCDE_ARED_SHIFT);

    p_palette->green = (t_uint16) ((ch_x_reg->mcde_ch_pal0 & MCDE_GREEN_MASK) >> MCDE_GREEN_SHIFT);

    p_palette->blue = (t_uint16) (ch_x_reg->mcde_ch_pal0 & MCDE_BLUE_MASK);
    
    #endif

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetGammaCoef()					                     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the gamma coefficients            */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_mcde_gamma:gamma coefficients parameters                  */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetGammaCoef(IN t_mcde_ch_id channel, IN t_mcde_gamma *gamma)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }
	#ifdef ST_8500ED
	
	    ch_x_reg->mcde_ch_gam =
        (
            (ch_x_reg->mcde_ch_gam &~MCDE_RED_MASK) |
            ((gamma->red << MCDE_ARED_SHIFT) & MCDE_RED_MASK)
        );

    ch_x_reg->mcde_ch_gam =
        (
            (ch_x_reg->mcde_ch_gam &~MCDE_GREEN_MASK) |
            ((gamma->green << MCDE_GREEN_SHIFT) & MCDE_GREEN_MASK)
        );

    ch_x_reg->mcde_ch_gam = ((ch_x_reg->mcde_ch_gam &~MCDE_BLUE_MASK) | (gamma->blue & MCDE_BLUE_MASK));
    
    #else
    
    ch_x_reg->mcde_gam0 = ((ch_x_reg->mcde_gam0 &~MCDE_GAM_MASK) | (gamma->blue & MCDE_GAM_MASK));
    
    ch_x_reg->mcde_gam1 = ((ch_x_reg->mcde_gam1 &~MCDE_GAM_MASK) | (gamma->green & MCDE_GAM_MASK));
    
    ch_x_reg->mcde_gam2 = ((ch_x_reg->mcde_gam2 &~MCDE_GAM_MASK) | (gamma->red & MCDE_GAM_MASK));
    
    #endif

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetGammaCoef()					                     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the gamma coefficients            */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_mcde_gamma *:gamma coefficients parameters                */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetGammaCoef(IN t_mcde_ch_id channel, IN t_mcde_gamma *p_gamma)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_gamma)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

	#ifdef ST_8500ED
	
	p_gamma->red = (t_uint8) ((ch_x_reg->mcde_ch_gam & MCDE_RED_MASK) >> MCDE_ARED_SHIFT);

    p_gamma->green = (t_uint8) ((ch_x_reg->mcde_ch_gam & MCDE_GREEN_MASK) >> MCDE_GREEN_SHIFT);

    p_gamma->blue = (t_uint8) (ch_x_reg->mcde_ch_gam & MCDE_BLUE_MASK);
    
    #else
    
    p_gamma->blue = (t_uint8) (ch_x_reg->mcde_gam0 & MCDE_GAM_MASK) ;

    p_gamma->green = (t_uint8) (ch_x_reg->mcde_gam1 & MCDE_GAM_MASK) ;

    p_gamma->red = (t_uint8) (ch_x_reg->mcde_gam2 & MCDE_GAM_MASK);
    
    #endif

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetRotation0BaseAddress()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the rotation 0 base address       */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_uint32 address:address                                    */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetRotation0BaseAddress(IN t_mcde_ch_id channel, IN t_uint32 address)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == address)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    ch_x_reg->mcde_rotadd0 = address;

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetRotation0BaseAddress()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the rotation 0 base address       */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_uint32 address *:address                                  */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetRotation0BaseAddress(IN t_mcde_ch_id channel, IN t_uint32 *p_address)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_address)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    *p_address = (t_uint32) ch_x_reg->mcde_rotadd0;

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetRotation1BaseAddress()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the rotation 1 base address       */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*              :t_uint32 address:address                                    */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                	     */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetRotation1BaseAddress(IN t_mcde_ch_id channel, IN t_uint32 address)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == address)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    ch_x_reg->mcde_rotadd1 = address;

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_GetRotation1BaseAddress()					         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to get the rotation 1 base address       */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_ch_id channel:Channel A/B                            */
/*     InOut    :None                                                        */
/* 		OUT 	:t_uint32 *address:address                                    */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*               MCDE_INVALID_PARAMETER :if input argument is invalid        */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_GetRotation1BaseAddress(IN t_mcde_ch_id channel, IN t_uint32 *p_address)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_ch_reg   *ch_x_reg;

    DBGENTER0();

    if (2 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        ch_x_reg = (t_mcde_ch_reg *) &(g_mcde_system_context.p_mcde_register->ch_x_reg[channel]);
    }

    if (NULL == p_address)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }

    *p_address = (t_uint32) ch_x_reg->mcde_rotadd1;

    DBGEXIT0(error);

    return(error);
}

/******************************************************************************
DSIX APIs
******************************************************************************/

#ifdef ST_8500ED
/*****************************************************************************/
/* NAME			:	MCDE_SetDSIClock()					                     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	:  This API is used to set the DSI clock                     */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_dsi_clk_config clk_config : clock config structure   */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC void MCDE_SetDSIClock(IN t_mcde_dsi_clk_config *clk_config)
{

    DBGENTER0();

    g_mcde_system_context.p_mcde_register->mcde_clkdsi =
        (
            (g_mcde_system_context.p_mcde_register->mcde_clkdsi &~MCDE_PLLOUT_DIVSEL1_MASK) |
            (((t_uint32) clk_config->pllout_divsel1 << MCDE_PLLOUT_DIVSEL1_SHIFT) & MCDE_PLLOUT_DIVSEL1_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_clkdsi =
        (
            (g_mcde_system_context.p_mcde_register->mcde_clkdsi &~MCDE_PLLOUT_DIVSEL0_MASK) |
            (((t_uint32) clk_config->pllout_divsel0 << MCDE_PLLOUT_DIVSEL0_SHIFT) & MCDE_PLLOUT_DIVSEL0_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_clkdsi =
        (
            (g_mcde_system_context.p_mcde_register->mcde_clkdsi &~MCDE_PLL4IN_SEL_MASK) |
            (((t_uint32) clk_config->pll4in_sel << MCDE_PLL4IN_SEL_SHIFT) & MCDE_PLL4IN_SEL_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_clkdsi =
        (
            (g_mcde_system_context.p_mcde_register->mcde_clkdsi &~MCDE_TXESCDIV_SEL_MASK) |
            (((t_uint32) clk_config->txescdiv_sel << MCDE_TXESCDIV_SEL_SHIFT) & MCDE_TXESCDIV_SEL_MASK)
        );

    g_mcde_system_context.p_mcde_register->mcde_clkdsi =
        (
            (g_mcde_system_context.p_mcde_register->mcde_clkdsi &~MCDE_TXESCDIV_MASK) |
            ((t_uint32) clk_config->txescdiv & MCDE_TXESCDIV_MASK)
        );

    DBGEXIT0(MCDE_OK);
}
#endif
/*****************************************************************************/
/* NAME			:	MCDE_SetDSICommandWord()			                     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: This API is used to set the DSI command byte               */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_dsi_channel channel : DSI channel                    */
/*               t_uint8 cmdbyte_lsb        : LSB of commnad byte            */
/*               t_uint8 cmdbyte_msb        : MSB of command byte            */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetDSICommandWord
(
    IN t_mcde_dsi_channel   channel,
    IN t_uint16              cmdbyte_lsb,
    IN t_uint16              cmdbyte_msb
)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_dsi_reg  *dsi_reg;

    DBGENTER0();

    if (6 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        dsi_reg = (t_mcde_dsi_reg *) &(g_mcde_system_context.p_mcde_register->mcde_dsi_reg[channel]);
    }

    dsi_reg->mcde_dsi_cmd =
        (
            (dsi_reg->mcde_dsi_cmd &~MCDE_CMDBYTE_LSB_MASK) |
            ((t_uint32) cmdbyte_lsb & MCDE_CMDBYTE_LSB_MASK)
        );

    dsi_reg->mcde_dsi_cmd =
        (
            (dsi_reg->mcde_dsi_cmd &~MCDE_CMDBYTE_MSB_MASK) |
            (((t_uint32) cmdbyte_msb << MCDE_CMDBYTE_MSB_SHIFT) & MCDE_CMDBYTE_MSB_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetDSISyncPulse()			                         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: This API is used to set the sync pulse sent to the software and DMA*/
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_dsi_channel channel : DSI channel                    */
/*               t_uint16 sync_dma:                                          */
/*               t_uint16 sync_sw:                                           */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetDSISyncPulse(IN t_mcde_dsi_channel channel, IN t_uint16 sync_dma, IN t_uint16 sync_sw)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_dsi_reg  *dsi_reg;

    DBGENTER0();

    if (6 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        dsi_reg = (t_mcde_dsi_reg *) &(g_mcde_system_context.p_mcde_register->mcde_dsi_reg[channel]);
    }

    dsi_reg->mcde_dsi_sync = ((dsi_reg->mcde_dsi_sync &~MCDE_DSI_DMA_MASK) | ((t_uint32) sync_dma & MCDE_DSI_DMA_MASK));

    dsi_reg->mcde_dsi_sync =
        (
            (dsi_reg->mcde_dsi_sync &~MCDE_DSI_SW_MASK) |
            (((t_uint32) sync_dma << MCDE_DSI_SW_SHIFT) & MCDE_DSI_SW_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

/*****************************************************************************/
/* NAME			:	MCDE_SetDSIConfig()		       	                         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION	: This API is used to set the configuration of DSI channel   */
/*                                                                           */
/*                                                                           */
/* PARAMETERS	:                                                            */
/* 		IN  	:t_mcde_dsi_channel channel : DSI channel                    */
/*               t_mcde_dsi_conf dsi_conf   : config structure               */
/*     InOut    :None                                                        */
/* 		OUT 	:None                                                        */
/*                                                                           */
/* RETURN		:t_mcde_error	: MCDE error code						   	 */
/*               MCDE_OK                                                     */
/*---------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                               */
/* REENTRANCY 	     :	Non Re-entrant                                       */
/* REENTRANCY ISSUES :														 */

/*****************************************************************************/
PUBLIC t_mcde_error MCDE_SetDSIConfig(IN t_mcde_dsi_channel channel, IN t_mcde_dsi_conf *dsi_conf)
{
    t_mcde_error    error = MCDE_OK;
    t_mcde_dsi_reg  *dsi_reg;

    DBGENTER0();

    if (6 <= (t_uint32) channel)
    {
        DBGEXIT0(MCDE_INVALID_PARAMETER);
        return(MCDE_INVALID_PARAMETER);
    }
    else
    {
        dsi_reg = (t_mcde_dsi_reg *) &(g_mcde_system_context.p_mcde_register->mcde_dsi_reg[channel]);
    }

    dsi_reg->mcde_dsi_conf0 =
        (
            (dsi_reg->mcde_dsi_conf0 &~MCDE_DSI_PACK_MASK) |
            (((t_uint32) dsi_conf->packing << MCDE_DSI_PACK_SHIFT) & MCDE_DSI_PACK_MASK)
        );

    dsi_reg->mcde_dsi_conf0 =
        (
            (dsi_reg->mcde_dsi_conf0 &~MCDE_DSI_DCSVID_MASK) |
            (((t_uint32) dsi_conf->synchro << MCDE_DSI_DCSVID_SHIFT) & MCDE_DSI_DCSVID_MASK)
        );

    dsi_reg->mcde_dsi_conf0 =
        (
            (dsi_reg->mcde_dsi_conf0 &~MCDE_DSI_BYTE_SWAP_MASK) |
            (((t_uint32) dsi_conf->byte_swap << MCDE_DSI_BYTE_SWAP_SHIFT) & MCDE_DSI_BYTE_SWAP_MASK)
        );

    dsi_reg->mcde_dsi_conf0 =
        (
            (dsi_reg->mcde_dsi_conf0 &~MCDE_DSI_BIT_SWAP_MASK) |
            (((t_uint32) dsi_conf->bit_swap << MCDE_DSI_BIT_SWAP_SHIFT) & MCDE_DSI_BIT_SWAP_MASK)
        );

    dsi_reg->mcde_dsi_conf0 =
        (
            (dsi_reg->mcde_dsi_conf0 &~MCDE_DSI_CMD8_MASK) |
            (((t_uint32) dsi_conf->cmd_mode << MCDE_DSI_CMD8_SHIFT) & MCDE_DSI_CMD8_MASK)
        );

    dsi_reg->mcde_dsi_conf0 =
        (
            (dsi_reg->mcde_dsi_conf0 &~MCDE_DSI_VID_MODE_MASK) |
            (((t_uint32) dsi_conf->vid_mode << MCDE_DSI_VID_MODE_SHIFT) & MCDE_DSI_VID_MODE_MASK)
        );

    dsi_reg->mcde_dsi_conf0 =
        (
            (dsi_reg->mcde_dsi_conf0 &~MCDE_BLANKING_MASK) |
            ((t_uint32) dsi_conf->blanking & MCDE_BLANKING_MASK)
        );

    dsi_reg->mcde_dsi_frame =
        (
            (dsi_reg->mcde_dsi_frame &~MCDE_DSI_FRAME_MASK) |
            ((t_uint32) dsi_conf->words_per_frame & MCDE_DSI_FRAME_MASK)
        );

    dsi_reg->mcde_dsi_pkt =
        (
            (dsi_reg->mcde_dsi_pkt &~MCDE_DSI_PACKET_MASK) |
            ((t_uint32) dsi_conf->words_per_packet & MCDE_DSI_PACKET_MASK)
        );

    DBGEXIT0(error);

    return(error);
}

