/*****************************************************************************/
/**
*  � ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* This module provides some support routines for the MultiMedia Card
* SD-card host Interface.
*
* author : ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/

#include <string.h>
#include "mmc_p.h"

/*--------------------------------------------------------------------------*
 * Debug stuff                                                              *
 *--------------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_MMC
#define MY_DEBUG_ID             myDebugID_MMC

PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = MMCSD_HCL_DBG_ID;
#endif

/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_mmc_system_context    g_mmc_system_context[NUM_MMC_INSTANCES];
PRIVATE t_mmc_error mmc_SwitchResp1Error(IN t_uint8 cmd, IN t_mmc_device_id mmc_device_id);

/********************************************************************************/
/*       NAME : MMC_Init()                                                      */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the MMC registers, checks              */
/*              Peripheral and PCell Id.                                        */
/* PARAMETERS :                                                                 */
/*         IN : t_logical_address mmc_base_address:MMC registers base address   */
/*         IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used   */
/*        OUT : NONE                                                            */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PUBLIC t_mmc_error MMC_Init(IN t_logical_address mmc_base_address, t_mmc_device_id mmc_device_id)
{
    t_mmc_error error;

    DBGENTER1("In MMC_Init MMCI logical address set at (%lx)\n", mmc_base_address);
#ifdef __DEBUG
    if (NULL == mmc_base_address)
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }
#endif
    g_mmc_system_context[mmc_device_id].p_mmc_register = (t_mmc_register *) mmc_base_address;

    /* Check Peripheral and Pcell Id Register for MMC */
    if
    (
        (MMC_PERIPHERAL_ID0 == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_periph_id0)
    &&  (MMC_PERIPHERAL_ID1 == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_periph_id1)
    &&  (MMC_PERIPHERAL_ID2 == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_periph_id2)
    &&  (MMC_PERIPHERAL_ID3 == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_periph_id3)
    &&  (MMC_PCELL_ID0 == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_pcell_id0)
    &&  (MMC_PCELL_ID1 == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_pcell_id1)
    &&  (MMC_PCELL_ID2 == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_pcell_id2)
    &&  (MMC_PCELL_ID3 == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_pcell_id3)
    )
    {
        error = MMC_OK;

#ifdef __MMCI_HCL_ENHANCED
        /*ASSIGN DEFAULT VALUES TO GLOBAL VARIABLE */
        mmc_ResetGV(mmc_device_id);
#endif /* __MMCI_HCL_ENHANCED */
    }
    else
    {
        error = MMC_UNSUPPORTED_HW;
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_SetDbgLevel()                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different            */
/*              debug comment levels                                        */
/* PARAMETERS :                                                             */
/*         IN : t_dbg_level mmc_debug_level:identify MMC debug level        */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error : MMC error code                                */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetDbgLevel(IN t_dbg_level mmc_debug_level)
{
    t_mmc_error error;

    DBGENTER1("In MMC_SetDbgLevel Setting Debug Level to %d\n", mmc_debug_level);
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = mmc_debug_level;
#endif
    error = MMC_OK;

    DBGEXIT0(error);
    return(error);
}

/**********************************************************************************************/
/* NAME:    t_mmc_error MMC_GetVersion()                                                      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns version for MMC HCL.                                     */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :         NONE                                                                          */
/* OUT :        t_version*   :          Version for MMC HCL                                   */
/*                                                                                            */
/* RETURN:      t_mmc_error :          MMC error code                                         */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant                                                                     */

/**********************************************************************************************/
PUBLIC t_mmc_error MMC_GetVersion(OUT t_version *p_version)
{
    DBGENTER0();
#ifdef __DEBUG
    if (NULL == p_version)
    {
        DBGEXIT0(MMC_INVALID_PARAMETER);
        return(MMC_INVALID_PARAMETER);
    }
#endif
    p_version->minor = MMC_HCL_MINOR_ID;
    p_version->major = MMC_HCL_MAJOR_ID;
    p_version->version = MMC_HCL_VERSION_ID;

    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_SetClockFrequency                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets clock frequency for MMCI controller.      */
/*                                                                          */
/* PARAMETERS :                                                             */
/*       IN :   t_uint8     Clock divider value for desired frequency.      */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */
/*     COMMENT:  Clock frequency is calculated by this formula:             */
/*      MCICLK = MCLK/(2 * [Clock_div +1])                                  */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetClockFrequency(IN t_uint8 clock_div, t_mmc_device_id mmc_device_id)
{
    DBGENTER1("In MMC_SetClockFrequency,Setting clock div as %d\n", clock_div);

    MMC_SET_CLKDIV(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, (t_uint32) clock_div);

    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_SetFeedBackClock                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables feedback clock.            */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state     feedback clock to enable or disable.        */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetFeedBackClock(IN t_mmc_state feedback_clock, t_mmc_device_id mmc_device_id)
{
    DBGENTER1("Setting feedback clock as %d\n", feedback_clock);
    MMC_SET_FBCLK(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power, feedback_clock);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

#ifndef ST_8500_ED
/****************************************************************************/
/*       NAME : MMC_SetInvClock                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables ofr disables SDICLK invert feature     */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state     invert_clock to enable or disable.          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetInvClock(IN t_mmc_state invert_clock, t_mmc_device_id mmc_device_id)
{
    DBGENTER1("Setting clockout invert as %d\n", invert_clock);
    MMC_SET_INVCLK(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, invert_clock);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}


/****************************************************************************/
/*       NAME : MMC_EnableNegEdgeClock                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables Negative edge clock.       */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state     edge_status clock to enable or disable.     */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_EnableNegEdgeClock(IN t_mmc_state edge_status, t_mmc_device_id mmc_device_id)
{
    DBGENTER1("Setting Negative Edge as %d\n", edge_status);
    MMC_SET_NEGEDGE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, edge_status);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}


#endif
/****************************************************************************/
/*       NAME : MMC_EnableDirSignals                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to CMDDIR and DATDIR signals to control */
/*              MCCMD and MCDAT signals.                                    */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state     MCCMD and MCDAT signal's control to be      */
/*                               enabled or disabled.                       */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_EnableDirSignals(IN t_mmc_state state, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    DBGENTER1("Setting enable dir signals as %d\n", state);

    switch (state)
    {
        case MMC_DISABLE:
            HCL_CLEAR_BITS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power, MMC_POWER_MASK_DIREN);
            break;

        case MMC_ENABLE:
            HCL_SET_BITS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power, MMC_POWER_MASK_DIREN);
            break;

        default:
            error = MMC_INVALID_PARAMETER;
            break;
    }

    DBGEXIT0(error);
    return(error);
}


/***********************************************************************************/
/* NAME:    t_mmc_error MMC_SaveDeviceContext()                                    */
/*---------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine saves current of MMC hardware for power management.   */
/*                                                                                 */
/* PARAMETERS:                                                                     */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used        */
/*      OUT : NONE                                                                 */
/* RETURN:      void                                                               */
/*                                                                                 */
/*---------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                       */
/* Global array MMC_DCMASK0 is being modified.                                     */

/***********************************************************************************/
PUBLIC t_mmc_error MMC_SaveDeviceContext(t_mmc_device_id mmc_device_id)
{
    DBGENTER0();
    g_mmc_system_context[mmc_device_id].mmc_dvcontext[0] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power;
    g_mmc_system_context[mmc_device_id].mmc_dvcontext[1] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock;
    g_mmc_system_context[mmc_device_id].mmc_dvcontext[2] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument;
    g_mmc_system_context[mmc_device_id].mmc_dvcontext[3] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command;
    g_mmc_system_context[mmc_device_id].mmc_dvcontext[4] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer;
    g_mmc_system_context[mmc_device_id].mmc_dvcontext[5] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length;
    g_mmc_system_context[mmc_device_id].mmc_dvcontext[6] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl;
    g_mmc_system_context[mmc_device_id].mmc_dvcontext[7] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0;
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/**********************************************************************************************/
/* NAME:    t_mmc_error MMC_RestoreDeviceContext()                                                    */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine restore current of MMCSD hardware.                               */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT          NONE                                                                          */
/* RETURN:      void                                                                          */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY:NA                                                                              */

/**********************************************************************************************/
PUBLIC t_mmc_error MMC_RestoreDeviceContext(t_mmc_device_id mmc_device_id)
{
    DBGENTER0();
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power = g_mmc_system_context[mmc_device_id].mmc_dvcontext[0];
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock = g_mmc_system_context[mmc_device_id].mmc_dvcontext[1];
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = g_mmc_system_context[mmc_device_id].mmc_dvcontext[2];
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = (g_mmc_system_context[mmc_device_id].mmc_dvcontext[3] & (~MMCCMDPATHENABLE));
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = g_mmc_system_context[mmc_device_id].mmc_dvcontext[4];
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = g_mmc_system_context[mmc_device_id].mmc_dvcontext[5];
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (g_mmc_system_context[mmc_device_id].mmc_dvcontext[6] & (~MMCDATAPATHENABLE));
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = g_mmc_system_context[mmc_device_id].mmc_dvcontext[7];
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/*********************************************************************************/
/*              FUNCTIONS SPECIFIC TO ELEMENTARY LAYER                           */
/*********************************************************************************/
#if defined(__MMCI_HCL_ELEMENTARY)

/****************************************************************************/
/*       NAME : MMC_SetWideBus()                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables wide bus operation bits as */
/*              requested.                                                  */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_power_state : wide bus operation state to be enabled  */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                  or disabled.                            */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error : MMC error code                                */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetWideBus(IN t_mmc_wide_mode widebusstate, t_mmc_device_id mmc_device_id)
{
    DBGENTER1("Setting wide bus operaion as %d\n", widebusstate);
    MMC_SET_WIDEBUS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, widebusstate);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_SetPowerState()                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the power status of the controller.       */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_power_state Power state to set                        */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error : MMC error code                                */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetPowerState(IN t_mmc_power_state powerstate, t_mmc_device_id mmc_device_id)
{
    DBGENTER1("Setting Power state to %d\n", powerstate);
    MMC_SET_CTRL(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power, (t_uint32) powerstate);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_GetPowerState()                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the power status of the controller.    */
/*                                                                          */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error : MMC error code                                */

/****************************************************************************/
PUBLIC t_mmc_power_state MMC_GetPowerState(t_mmc_device_id mmc_device_id)
{
    t_mmc_power_state   state;
    DBGENTER0();

    state = (t_mmc_power_state) HCL_READ_BITS
        (
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power,
            MMC_POWER_MASK_CTRL
        );

    DBGEXIT0(state);
    return(state);
}

/****************************************************************************/
/*       NAME : MMC_ConfigBus                                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the bus in open drain or .          */
/*              push-pull mode                                              */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_bus_configuration     Bus configuration to set        */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_ConfigBus(IN t_mmc_bus_mode bus_config, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    DBGENTER1("bus mode(%d) \n", bus_config);
    MMC_SET_OPEND(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power, bus_config);
    DBGEXIT0(error);
    return(error);
}

#ifndef ST_8500_ED
/****************************************************************************/
/*       NAME : MMC_EnPowerReset                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables the SDI power state        */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state     Bus reset_state to enable or disable        */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/

PUBLIC t_mmc_error MMC_EnPowerReset(IN t_mmc_state reset_state, t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    DBGENTER1("bus mode(%d) \n", reset_state);
    MMC_EN_RESET(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power, reset_state);

    DBGEXIT0(error);
    return(error);
}
#endif

/****************************************************************************/
/*       NAME : MMC_SetClock                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the clock state as enabled or disabled.   */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state     clock state to set.                         */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : void                                                */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetClockState(IN t_mmc_state clock_state, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1("Setting bus state to %d\n", clock_state);
    MMC_SET_CENABLE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, clock_state);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_ConfigClockControl                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets configurations for clock and bus mode.    */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_clock_control     clock control state to set.         */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : void                                                */

/****************************************************************************/
PUBLIC t_mmc_error MMC_ConfigClockControl(IN t_mmc_clock_control clock_control, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER3
    (
        "powersave(%d) bypass(%d) hs_enable(%d)\n",
        clock_control.pwr_save,
        clock_control.bypass,
        clock_control.hs_enable
    );
    MMC_SET_PWRSAVE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, clock_control.pwr_save);
    MMC_SET_BYPASS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, clock_control.bypass);
    MMC_SET_HSENABLE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock,clock_control.hs_enable);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}


/****************************************************************************/
/*       NAME : MMC_EnableHWFlowControl                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to Enable/Disable the HW Flow Control   */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state     Hardware Flow control to be                 */
/*                               enabled or disabled.                       */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */
/****************************************************************************/
PUBLIC t_mmc_error MMC_EnableHWFlowControl(IN t_mmc_state state, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    DBGENTER1("Setting enable Hw Flow Control as %d\n", state);

    switch (state)
    {
        case MMC_DISABLE:
            HCL_CLEAR_BITS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, MMC_CLOCK_MASK_HWFC);
            break;

        case MMC_ENABLE:
            HCL_SET_BITS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, MMC_CLOCK_MASK_HWFC);
            break;

        default:
            error = MMC_INVALID_PARAMETER;
            break;
    }
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/*       NAME : MMC_SendCommand                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sends command and enable Command path          */
/*              state machine.                                              */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_command_index: Command to send.                       */
/*              t_uint32: argument to send.                                 */
/*              t_mmc_command_control: Command control parameters to set.   */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SendCommand
(
    IN t_mmc_command_index      cmd_index,
    IN t_uint32                 argument,
    IN t_mmc_command_control    ctrl_parameters,
    IN t_mmc_device_id          mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    reg;

    DBGENTER1("Command to send %d\n", cmd_index);
    if (MMC_NO_CMD != cmd_index)
    {
        reg = (t_uint32) cmd_index;

        DBGENTER1("argument to set %d\n", argument);
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;

        DBGENTER1("response expected %d\n", ctrl_parameters.is_resp_expected);
        reg |= ((t_uint32) ctrl_parameters.is_resp_expected << 6);

        DBGENTER1("Long response expected %d\n", ctrl_parameters.is_long_resp);
        reg |= ((t_uint32) ctrl_parameters.is_long_resp << 7);

        DBGENTER1("interrupt mode as %d\n", ctrl_parameters.is_interrupt_mode);
        reg |= ((t_uint32) ctrl_parameters.is_interrupt_mode << 8);

        DBGENTER1("pending bit as %d\n", ctrl_parameters.is_pending);
        reg |= ((t_uint32) ctrl_parameters.is_pending << 9);

        DBGENTER1("cmd_path as %d\n", ctrl_parameters.cmd_path);
        reg |= ((t_uint32) ctrl_parameters.cmd_path << 10);

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = reg;
    }
    else
    {
        error = MMC_INVALID_PARAMETER;
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_GetCommandResponse                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns command index of last command for      */
/*              which response received.                                    */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_command_index command index of last command           */
/*                                                                          */

/****************************************************************************/
PUBLIC t_mmc_command_index MMC_GetCommandResponse(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_command_index respcommand;

    DBGENTER0();
    respcommand = (t_mmc_command_index) g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_resp_command;
    DBGEXIT0(respcommand);
    return(respcommand);
}

/****************************************************************************/
/*       NAME : MMC_GetResponse                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns response received from the card for    */
/*              the last command.                                           */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_response_type: Expected response type                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*         OUT: t_uint32 *         : p_respbuff pointer to store response.  */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */
/*                                                                          */

/****************************************************************************/
PUBLIC t_mmc_error MMC_GetResponse
(
    IN t_mmc_response_type  resp_type,
    IN t_mmc_device_id      mmc_device_id,
    OUT t_uint32            *p_respbuff
)
{
    t_mmc_error error = MMC_OK;
    DBGENTER0();

#ifdef __DEBUG
    if (NULL == p_respbuff)
    {
        DBGEXIT0(MMC_INVALID_PARAMETER);
        return(MMC_INVALID_PARAMETER);
    }
#endif
    if (MMC_SHORT_RESP == resp_type)
    {
        *p_respbuff = (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0);
    }
    else
    {
        *p_respbuff = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;
        *(p_respbuff + 1) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response1;
        *(p_respbuff + 2) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response2;
        *(p_respbuff + 3) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response3;
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_SetDataPath                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables/disables the data path for data        */
/*              transfer.                                                   */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state: Specifies the state of the                     */
/*               data_path_state, whether to enabled or disabled.           */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : void                                                    */
/*                                                                          */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetDataPath(IN t_mmc_state data_path_state, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER0();
    MMC_SET_DATAPATH(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, (t_uint32) data_path_state);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_SetDataTimeOut                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the data timeout period in card bus       */
/*              clock periods                                               */
/* PARAMETERS :                                                             */
/*         IN : t_uint32: Specifies the timeout value of the data path.     */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetDataTimeOut(IN t_uint32 timeout, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1("setting data timeout as(%lx)\n", timeout);
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = timeout;
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/******************************************************************************/
/*       NAME : MMC_SetDataLength                                             */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the data length (in bytes) for the data     */
/*              transfer.                                                     */
/* PARAMETERS :                                                               */
/*       IN : t_uint32 Specifies the number of data bytes to be transferred.*/
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used   */
/*        OUT : NONE                                                          */
/*                                                                            */
/*     RETURN : t_mmc_error                                                       */

/******************************************************************************/
PUBLIC t_mmc_error MMC_SetDataLength(IN t_uint32 data_length, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1("no of bytes to be transferred %d\n", data_length);
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = data_length;
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_SetDataBlockLength                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the data block size (in an encoded fashion)*/
/*              for block data transfer.                                    */
/* PARAMETERS :                                                             */
/*         IN : t_uint8 Specifies the data block size for block transfer.   */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
#if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
PUBLIC t_mmc_error MMC_SetDataBlockLength(IN t_uint8 blocksize, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error;
    DBGENTER1("block size in encoded form%d\n", blocksize);
    if (MAXBSIZEPOWER >= blocksize)
    {
        /*coverity[result_independent_of_operands]*/
        MMC_SET_BLOCKSIZE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, blocksize);
        error = MMC_OK;
    }
    else
    {
        error = MMC_INVALID_PARAMETER;
    }

    DBGEXIT0(error);
    return(error);
}

#else

PUBLIC t_mmc_error MMC_SetDataBlockLength(IN t_uint16 blocksize, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error;
    DBGENTER1("block size in encoded form%d\n", blocksize);
    if (MAX_MMC_BLKSIZE >= blocksize)
    {
        /*coverity[result_independent_of_operands]*/
        MMC_SET_BLOCKSIZE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, blocksize);
        error = MMC_OK;
    }
    else
    {
        error = MMC_INVALID_PARAMETER;
    }

    DBGEXIT0(error);
    return(error);
}
#endif

/****************************************************************************/
/*       NAME : MMC_SetTransferDirection                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: routine sets direction for data transfer, whether           */
/*               the transfer is a read or write.                           */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_transfer_direction  the direction for data transfer.  */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetTransferDirection(IN t_mmc_transfer_direction trans_direc, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1("tansfer direction to set as %d\n", trans_direc);
    MMC_SET_DATADIR(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, trans_direc);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/***********************************************************************************/
/*       NAME : MMC_SetTransferType                                                */
/*---------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets whether data transfer is                         */
/*               in stream mode or block mode.                                     */
/* PARAMETERS :                                                                    */
/*         IN : t_mmc_transfer_type  Specifies the transfer type for data transfer.*/
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used        */
/*        OUT : NONE                                                               */
/*                                                                                 */
/*     RETURN : t_mmc_error                                                        */

/***********************************************************************************/
PUBLIC t_mmc_error MMC_SetTransferType(IN t_mmc_transfer_type trans_mode, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1("tansfer type to set as %d\n", trans_mode);
    MMC_SET_MODE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, trans_mode);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_HandleDMA                                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables data transfer through DMA. */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state  Specifies whether to enable/disable DMA for    */
/*                           data transfer.                                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : void                                                    */

/****************************************************************************/
PUBLIC t_mmc_error MMC_HandleDMA(IN t_mmc_state dma_state, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1("dma state to set as %d\n", dma_state);
    MMC_SET_DMA(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, dma_state);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_GetDataCounter                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns number of data elements (in bytes)     */
/*              yet to be transferred.                                      */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_uint16                                                    */

/****************************************************************************/
PUBLIC t_uint16 MMC_GetDataCounter(IN t_mmc_device_id mmc_device_id)
{
    t_uint16    no_of_elements;
    DBGENTER0();
    no_of_elements = (t_uint16) g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_count;
    DBGEXIT0(no_of_elements);
    return(no_of_elements);
}

/****************************************************************************/
/*       NAME : MMC_ReadFIFO                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to read one data word from RX FIFO.     */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : t_uint32 *  pointer to 32-bit data word.                    */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_ReadFIFO(IN t_mmc_device_id mmc_device_id, OUT t_uint32 *p_data)
{
    t_mmc_error error = MMC_OK;
    DBGENTER0();
#ifdef __DEBUG
    if (NULL == p_data)
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }
#endif
    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & (t_uint32) MMC_IRQ_RX_DATA_AVLBL)
    {
        *p_data = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
    }

    /* cmd14 - only for HSMMC */
    else if ((t_uint32) MMC_HS_BUSTEST_READ == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_resp_command)
    {
        *p_data = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
    }
    else
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_WriteFIFO                                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to write one data word to TX FIFO.      */
/* PARAMETERS :                                                             */
/*         IN : t_uint32     32-bit data word to write.                     */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_WriteFIFO(IN t_uint32 data, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    DBGENTER1("data(%d)", data);
    if (!(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & (t_uint32) MMC_IRQ_TX_FIFO_FULL))
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = data;
    }
    else if ((t_uint32) MMC_HS_BUSTEST_WRITE == g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_resp_command)   /* cmd19 - only for HSMMC */
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = data;
    }
    else
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_GetFIFOCount                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the no. of words left to be written    */
/*              to or read from FIFO.                                       */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_uint16    remaining no of words to be transmitted         */

/****************************************************************************/
PUBLIC t_uint16 MMC_GetFIFOCount(IN t_mmc_device_id mmc_device_id)
{
    t_uint16    fifocnt;
    DBGENTER0();
    fifocnt = (t_uint16) g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo_count;
    DBGEXIT0(fifocnt);
    return(fifocnt);
}

/*********************************************************************************/
/*              FUNCTIONS SPECIFIC TO SDIO CARD SUPPORT                          */
/*********************************************************************************/
/**/

/**/
/****************************************************************************/
/*       NAME : MMC_StartSdioReadWait                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine starts read wait interval for SDIO card.       */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */
/*              MMC_REQUEST_NOT_APPLICABLE: if SDIO specific operation is not   */
/*              enabled and direction for data path not from card to host.  */
/*              MMC_OK: No error                                            */

/****************************************************************************/
PUBLIC t_mmc_error MMC_StartSdioReadWait(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    DBGENTER0();

    if
    (
        (
            NULL !=
                (
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl &
                        (t_uint32) MMC_DATAPATH_MASK_DIRECTION
                )
        )
    &&  (
            NULL !=
                (
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl &
                        (t_uint32) MMC_DATAPATH_MASK_SDIO_ENABLE
                )
        )
    )
    {
        MMC_START_READ_WAIT
        (
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl,
            MMC_DATAPATH_MASK_RWSTART
        );
    }
    else
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(error);
    return(error);
}

/********************************************************************************/
/*       NAME : MMC_StopSdioReadWait                                            */
/*--------------------------------------------------------------------------    */
/* DESCRIPTION: This routine starts read wait interval for SDIO card.           */
/* PARAMETERS :                                                                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : NONE                                                            */
/*                                                                              */
/*     RETURN : t_mmc_error                                                     */
/*              MMC_REQUEST_NOT_APPLICABLE: if SDIO specific operation is not   */
/*              enabled and direction for data path not from card to host.      */
/*              MMC_OK: No error                                                */

/********************************************************************************/
PUBLIC t_mmc_error MMC_StopSdioReadWait(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    DBGENTER0();

    if
    (
        (
            NULL !=
                (
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl &
                        (t_uint32) MMC_DATAPATH_MASK_RWSTART
                )
        )
    &&  (
            NULL ==
                (
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl &
                        (t_uint32) MMC_DATAPATH_MASK_RWSTOP
                )
        )
    )
    {
        MMC_STOP_READ_WAIT(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, MMC_DATAPATH_MASK_RWSTOP);
    }
    else
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(error);
    return(error);
}

/***************************************************************************************************/
/*       NAME : MMC_SetSdioReadWaitMode                                                            */
/*----------------------------------------------------------------------------------------------   */
/* DESCRIPTION: This routine sets one of the two options of inserting read wait interval.          */
/* PARAMETERS :                                                                                    */
/*         IN : Read_wait_mode : MMC_READ_WAIT_CONTROLLING_CLK, If SDIO card doesn't support       */
/*                               read wait feature by controlling DAT2 line.                       */
/*                              (i.e. by stopping clock) MMC_READ_WAIT_CONTROLLING_DAT2, otherwise.*/
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                        */
/*                                                                                                 */
/*        OUT : NONE                                                                               */
/*                                                                                                 */
/*     RETURN : t_mmc_error MMC_OK: No error                                                       */

/***************************************************************************************************/
PUBLIC t_mmc_error MMC_SetSdioReadWaitMode(IN t_mmc_read_wait_mode read_wait_mode, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1("Read wait mode passed - %x\n", read_wait_mode);

    if (MMC_READ_WAIT_CONTROLLING_CLK != read_wait_mode)
    {
        HCL_CLEAR_BITS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, MMC_DATAPATH_MASK_RWMODE);
    }
    else
    {
        HCL_SET_BITS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, MMC_DATAPATH_MASK_RWMODE);
    }

    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/***************************************************************************************************/
/*       NAME : MMC_ResetSdioReadWait                                                              */
/*----------------------------------------------------------------------------------------------   */
/* DESCRIPTION: This routine Resets start and stop bits for read wait.                             */
/* PARAMETERS :                                                                                    */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                        */
/*                                                                                                 */
/*        OUT : NONE                                                                               */
/*                                                                                                 */
/*     RETURN : t_mmc_error MMC_OK: No error                                                       */

/***************************************************************************************************/
PUBLIC t_mmc_error MMC_ResetSdioReadWait(IN t_mmc_device_id mmc_device_id)
{
    DBGENTER0();

    if
    (
        NULL !=
            (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl & (t_uint32) MMC_DATAPATH_MASK_RWSTOP)
    )
    {
        HCL_CLEAR_BITS
        (
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl,
            (MMC_DATAPATH_MASK_RWSTART | MMC_DATAPATH_MASK_RWSTOP)
        );
    }
    else
    {
        DBGEXIT0(MMC_REQUEST_NOT_APPLICABLE);
        return(MMC_REQUEST_NOT_APPLICABLE);
    }

    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/***************************************************************************************************/
/*       NAME : MMC_SetSdioOperation                                                               */
/*-------------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine informs data path unit that SDIO operation is to be performed or not. */
/* PARAMETERS :                                                                                    */
/*         IN : Read_wait_mode : MMC_ENABLE, If SDIO specific operation is to start.               */
/*         IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                      */
/*                               MMC_DISABLE, otherwise.                                           */
/*        OUT : NONE                                                                               */
/*                                                                                                 */
/*     RETURN : t_mmc_error MMC_OK: No error                                                       */

/***************************************************************************************************/
PUBLIC t_mmc_error MMC_SetSdioOperation(IN t_mmc_state sdio_operation, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1("Sdio operation state passed - %x\n", sdio_operation);

    if (MMC_DISABLE == sdio_operation)
    {
        HCL_CLEAR_BITS
        (
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl,
            MMC_DATAPATH_MASK_SDIO_ENABLE
        );
    }
    else
    {
        HCL_SET_BITS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, MMC_DATAPATH_MASK_SDIO_ENABLE);
    }

    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/***********************************************************************************************/
/*       NAME : MMC_SendSdioSuspendCmd                                                         */
/*---------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine informs command path unit that next command to be sent is suspend */
/*                command or not.                                                              */
/* PARAMETERS :                                                                                */
/*         IN : Read_wait_mode : MMC_ENABLE, If next command to be sent is suspend command.    */
/*                               MMC_DISABLE, otherwise.                                       */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                    */
/*        OUT : NONE                                                                           */
/*                                                                                             */
/*     RETURN : t_mmc_error MMC_OK: No error                                                   */

/***********************************************************************************************/
PUBLIC t_mmc_error MMC_SendSdioSuspendCmd(IN t_mmc_state suspend_cmd, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1("Sdio Suspend command state eneterd - %x\n", suspend_cmd);

    if (MMC_DISABLE == suspend_cmd)
    {
        HCL_CLEAR_BITS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command, MMC_CMD_MASK_SDIO_SUSPEND);
    }
    else
    {
        HCL_SET_BITS(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command, MMC_CMD_MASK_SDIO_SUSPEND);
    }

    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_SetBOOTMode                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables                            */
/*              Data path State Machine BOOTMODE .                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state  Specifies whether to enable/disable BOOT MODE  */
/*                                                                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*       OUT : NONE                                                         */
/*                                                                          */
/*     RETURN : MMC_OK                                                      */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetBOOTMode(IN t_mmc_state boot_mode_state, t_mmc_device_id mmc_device_id)
{
    DBGENTER0();
    MMC_SET_BOOTMODE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, (t_uint32) boot_mode_state);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_SetBUSYTMode                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables                            */
/*              Data path State Machine BUSYMODE .                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state  Specifies whether to enable/disable BUSY MODE  */
/*                                                                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*       OUT : NONE                                                         */
/*                                                                          */
/*     RETURN : MMC_OK                                                      */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetBUSYMode(IN t_mmc_state busy_mode_state, t_mmc_device_id mmc_device_id)
{
    DBGENTER0();
    MMC_SET_BUSYMODE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, (t_uint32) busy_mode_state);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

#ifndef ST_8500_ED

/****************************************************************************/
/*       NAME : MMC_EnableDDRMode                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables                            */
/*              Data path State DDR mode .                          */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state  Specifies whether to enable/disable BUSY MODE  */
/*                                                                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*       OUT : NONE                                                         */
/*                                                                          */
/*     RETURN : MMC_OK                                                      */

/****************************************************************************/
PUBLIC t_mmc_error MMC_EnableDDRMode(IN t_mmc_state ddr_mode_state, t_mmc_device_id mmc_device_id)
{
    DBGENTER0();
    MMC_SET_DDRMODE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, (t_uint32) ddr_mode_state);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}


#endif


#endif /*__MMCI_HCL_ELEMENTARY elementary layer*/

/*********************************************************************************/
/*              FUNCTIONS SPECIFIC TO ENHANCED LAYER                             */
/*********************************************************************************/
#if defined(__MMCI_HCL_ENHANCED)

/********************************************************************************/
/*       NAME : MMC_EnableEraseGroup()                                          */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables HC Erase group definition.                 */
/* PARAMETERS :                                                                 */
/*         IN : t_mmc_device_id: Device Id for MMC Instance Used                */
/*     RETURN : t_mmc_error : MMC error code                                    */
/********************************************************************************/
PUBLIC t_mmc_error MMC_EnableEraseGroup(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_ERASE_GROUPDEF;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command  = MMCHSSWITCH | MMCRESPEXPECTED | MMCCMDPATHENABLE;

    error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

/********************************************************************************/
/*       NAME : MMC_SetEnhancedConfig()                                             */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine Configures the enhanced Data Area for eMMC.            */
/* PARAMETERS :                                                                 */
/*         IN : t_uint32 startaddr: Address of the enhanced area in form of      */
/*                  write protect groups. Should be write protected              */
/*        IN : t_uint32 enh_size: Size of the partition (in byte units)         */
/*        IN : t_uint8 patition_attribute: enhanced or not                      */
/*        IN : t_mmc_device_id: Device Id for MMC Instance Used                 */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PUBLIC t_mmc_error MMC_SetEnhancedConfig(IN t_uint32 enh_startaddr, IN t_uint32 enh_size, IN t_uint8 partition_attribute, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32 csd_buff[128];
    t_uint32 protectstatus = 0x0;
    t_uint8 enh_mult0, enh_mult1, enh_mult2, enh_mult3;
    t_uint8 enh_size_mult0, enh_size_mult1, enh_size_mult2;
    volatile t_uint32 i;

    t_uint8 cardno = 1;



    /* Enhanced start address should be write protect grp aligned*/

    enh_mult0 = (enh_startaddr & 0xFF);
    enh_mult1 = ((enh_startaddr & 0xFF00)>>8);
    enh_mult2 = ((enh_startaddr & 0xFF0000)>>16);
    enh_mult3 = ((enh_startaddr & 0xFF000000)>>24);

    enh_size_mult0 = (enh_size & 0xFF);
    enh_size_mult1 = ((enh_size & 0xFF00)>>8);
    enh_size_mult2 = ((enh_size & 0xFF0000)>>16);

    error  = MMC_SendWriteProtectStatus(cardno, enh_startaddr, &protectstatus, mmc_device_id);
    if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

    error = MMC_GetExtCSD(cardno, csd_buff, mmc_device_id);
    if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

    if(NULL != (((t_uint8*)csd_buff)[156]) & (0x1))
        {
            DBGEXIT0(MMC_PARTITION_ALREADY_DONE);
            return(MMC_PARTITION_ALREADY_DONE);
        }


    if(NULL != (((t_uint8*)csd_buff)[160]) & (0x3))  /* Partition support enable */ /* Enhanced support enable */
        {
            /*Enable Erase group def */
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_ERASE_GROUPDEF;
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
              MMCRESPEXPECTED |
              MMCCMDPATHENABLE;
        
             error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
             if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }


            /* Set ENH_user data area SIZE : Multiple of write protect group size */

                /* Set ENH_SIZE 0 */
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (MMC_ENH_SIZE0 | (enh_size_mult0<<8)) ;
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
              MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
        
             error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
             if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

            /* Set ENH_SIZE 1 */
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (MMC_ENH_SIZE1 | (enh_size_mult1<<8)) ;
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
              MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
        
             error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
             if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

                /* Set ENH_SIZE 2 */
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (MMC_ENH_SIZE2 | (enh_size_mult2<<8)) ;
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
              MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
        
            error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

            /* Set ENH_start address0 */
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (MMC_ENH_STARTADDR0 | (enh_mult0<<8)) ;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
             MMCRESPEXPECTED |
                MMCCMDPATHENABLE;

            error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

            /* Set ENH_start address1 */
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (MMC_ENH_STARTADDR1 | (enh_mult1<<8)) ;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
             MMCRESPEXPECTED |
                MMCCMDPATHENABLE;

            error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

            /* Set ENH_start address2 */
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (MMC_ENH_STARTADDR2 | (enh_mult2<<8)) ;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
              MMCRESPEXPECTED |
                MMCCMDPATHENABLE;

            error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

                /* Set ENH_start address3 */
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (MMC_ENH_STARTADDR3 | (enh_mult3<<8)) ;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
             MMCRESPEXPECTED |
                MMCCMDPATHENABLE;

            error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }


            /* Enable partition attribute */
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_PARTITION_ATT;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
             MMCRESPEXPECTED |
               MMCCMDPATHENABLE;

            error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);

            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        /****************** PARTITIONING COMPLETED *************************/

                /* Set PARTITION COMPLETE  */
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_PARTTITON_COMPLETED;
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
              MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
        
             error = mmc_SwitchResp1Error(MMCHSSWITCH, mmc_device_id);
             if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

       /* Power On Cycle */
       
       for(i=0; i<=1000; i++); /* Delay provided before sending CMD0 */

           /*Send CMD0 GO_IDLE_STATE*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0xFFFFFFFA;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCGO_IDLE_STATE &~MMCRESPEXPECTED | MMCCMDPATHENABLE;

        error = mmc_CmdError(mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }


    /* XVSZOAN: Card must be completely reinitialized after GO_IDLE_STATE */

        /*Enable Erase group def */
/*       g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_ERASE_GROUPDEF;
         g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH |
          MMCRESPEXPECTED |
          MMCCMDPATHENABLE;

         error = mmc_CmdResp1Error(MMCHSSWITCH, mmc_device_id);
         if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
*/
           
       /* Check the status of the partitions configure */

/*               g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x0;
             g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMC_SEND_STATUS |
              MMCRESPEXPECTED |
                MMCCMDPATHENABLE;

             error = mmc_CmdResp1Error(MMC_SEND_STATUS, mmc_device_id);
             if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
*/
      /* Calculation of Enhanced User Data Area */ 
       
       
       }
       
       else
       {
            DBGEXIT0(MMC_PARTITION_NOT_SUPPORTED);
            return(MMC_PARTITION_NOT_SUPPORTED);

       }


   return(error);

}







/****************************************************************************/
/*       NAME : MMC_PowerON                                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: For MuPoC Lite this routine enquires cards about their      */
/*              operating voltage and sets optimal value to supply output   */
/*              voltage. Sends out of range cards to inactive states. Also  */
/*              configures clock controls.                                  */
/*              MuPoC Full supports only one data port, so one sd-card or   */
/*              multimedia card can be supported.                           */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/

PRIVATE t_mmc_error mmc_PowerON_SDIO_COMBO(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    response;
    //t_uint32  delay, temp = 0;
    t_bool      validvoltage = FALSE, flag = FALSE;
    t_uint32    count = 0;
    t_uint32    address_mode = MMC_BYTE_MODE;

    /*Send CMD0 GO_IDLE_STATE*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCGO_IDLE_STATE &~MMCRESPEXPECTED | MMCCMDPATHENABLE;

    error = mmc_CmdError(mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*SEND ACMD41*/
    /*SEND CMD55 APP_CMD with RCA as 0*/
    /*delay */

    //for (delay = 0; delay < 100; delay++)
   // {
    //    temp = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_periph_id0;
   // }

    //temp = temp;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_CHECK_PATTERN;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSD_SEND_IF_COND |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;

    error = mmc_CmdResp7Error(mmc_device_id);
    if (MMC_OK == error)
    {
        address_mode = MMC_SECTOR_MODE;
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD | MMCRESPEXPECTED | MMCCMDPATHENABLE;

    error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);

    if (MMC_OK == error)
    {
        /*Send CMD41 SD_APP_APP_OP_COND WITH ARGUMENT 0x00FFC000 */
        while ((!validvoltage) && (MMC_MAXVOLTTRIAL > count))
        {
            if (flag)
            {
                /*SEND CMD55 APP_CMD with RCA as 0*/
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD |
                    MMCRESPEXPECTED |
                    MMCCMDPATHENABLE;

                error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);
                if (MMC_OK != error)
                {
                    DBGEXIT0(error);
                    return(error);
                }
            }

            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_VOLTAGEWINDOWSD | address_mode;                      //voltage window
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSD_APP_OP_COND |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;

            error = mmc_CmdResp3Error(mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

            response = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;
            validvoltage = (t_bool) (((response >> MMC_SHIFT31) == 1) ? 1 : 0);
            flag = TRUE;
            count++;

            /*NO VOLTAGE SETTING IN MuPoC-FULL */
        }

        if (MMC_MAXVOLTTRIAL <= count)
        {
            error = MMC_INVALID_VOLTRANGE;
            DBGEXIT0(error);
            return(error);
        }

        if (0 < g_mmc_system_context[mmc_device_id].current_card.sdio_info.no_of_io_funcs)
        {
            g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_SECURE_DIGITAL_IO_COMBO_CARD;
        }
        else
        {
            if (MMC_SECTOR_MODE == address_mode)
            {
                g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_HIGH_CAPACITY_SD_CARD;
            }
            else
            {
                g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_SECURE_DIGITAL_CARD;
            }
        }
    }
    else
    {
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_PowerON_SDIO(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error         error = MMC_OK;
    //t_uint32            delay, temp = 0;
    t_bool              validvoltage = FALSE;
    t_uint32            count = 0;
    t_uint32            valid_ocr = 0;
    t_mmc_sdio_resp4    sdio_resp;

    if (0 < g_mmc_system_context[mmc_device_id].current_card.sdio_info.no_of_io_funcs)
    {
        if (NULL != (g_mmc_system_context[mmc_device_id].current_card.sdio_info.op_cond_register & MMCSDIO_OCR_VALUE))
        {
            count = 0;
            while (count < 24)
            {
                if ((g_mmc_system_context[mmc_device_id].current_card.sdio_info.op_cond_register & MMCSDIO_OCR_VALUE) & (MASK_BIT0 << count))
                {
                    valid_ocr = (MASK_BIT0 << count);
                    break;
                }

                count++;
            }

            count = 0;
            while ((!validvoltage) && (MMC_MAXVOLTTRIAL > count))
            {
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = valid_ocr;
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_SEND_OP_COND |
                    MMCRESPEXPECTED |
                    MMCCMDPATHENABLE;

                error = mmc_CmdResp4Error(MMCIO_SEND_OP_COND, &sdio_resp, mmc_device_id);
                if (MMC_OK != error)
                {
                    DBGEXIT0(error);
                    return(error);
                }
                /*
                for (delay = 0; delay < 1; delay++)
                {
                    temp = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_periph_id0;
                }

                temp = temp;
                */
                validvoltage = (t_bool) g_mmc_system_context[mmc_device_id].current_card.sdio_info.card_ready_after_init;

                /*NO VOLTAGE SETTING IN MuPoC-FULL */
                count++;
            }

            if (TRUE != (t_bool) g_mmc_system_context[mmc_device_id].current_card.sdio_info.card_ready_after_init)
            {
                DBGEXIT0(MMC_INVALID_VOLTRANGE);
                return(MMC_INVALID_VOLTRANGE);
            }
            else
            {
                g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_SECURE_DIGITAL_IO_CARD;
            }
        }
        else
        {
            DBGEXIT0(MMC_INVALID_VOLTRANGE);
            return(MMC_INVALID_VOLTRANGE);
        }
    }

    if (TRUE == (t_bool) g_mmc_system_context[mmc_device_id].current_card.sdio_info.memory_present)
    {
        error = mmc_PowerON_SDIO_COMBO(mmc_device_id);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_PowerON_SD(IN t_mmc_device_id mmc_device_id,t_uint32 address_mode)
{
    t_mmc_error error = MMC_OK;
    t_uint32    response;
    t_bool      validvoltage = FALSE, flag = FALSE;
    t_uint32    count = 0;
    

    /*SD CARD */
    /*Send CMD41 SD_APP_APP_OP_COND WITH ARGUMENT 0x00FFC000 */
    while ((!validvoltage) && (MMC_MAXVOLTTRIAL > count))
    {
        if (flag)
        {
            /*SEND CMD55 APP_CMD with RCA as 0*/
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;

            error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        }

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_VOLTAGEWINDOWSD | address_mode;                          /* voltage window */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSD_APP_OP_COND |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

        error = mmc_CmdResp3Error(mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        response = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;
        validvoltage = (t_bool) (((response >> MMC_SHIFT31) == 1) ? 1 : 0);
        flag = TRUE;
        count++;

        /*NO VOLTAGE SETTING IN MuPoC-FULL */
    }

    if (MMC_MAXVOLTTRIAL <= count)
    {
        error = MMC_INVALID_VOLTRANGE;
        DBGEXIT0(error);
        return(error);
    }

    if (MMC_SECTOR_MODE == address_mode)
    {
        g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_HIGH_CAPACITY_SD_CARD;
    }
    else
    {
        g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_SECURE_DIGITAL_CARD;
    }

    return(error);
}

PRIVATE t_mmc_error mmc_PowerON_MMC(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    //t_uint32    response, delay, temp = 0;
    t_bool      validvoltage = FALSE;
    t_uint32    count = 0;
    t_uint32    response;
    /*Send CMD0 GO_IDLE_STATE*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCGO_IDLE_STATE &~MMCRESPEXPECTED | MMCCMDPATHENABLE;

    error = mmc_CmdError(mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power |= MMC_OPENDRAIN;

    /* while (!validvoltage) */
    /*{
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEND_OP_COND | MMCRESPEXPECTED | MMCCMDPATHENABLE;
        error = mmc_CmdResp3Error();

        response = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;

        if (MMC_SECTOR_MODE & response)
        {
            address_mode = MMC_SECTOR_MODE;
        }

        validvoltage = (t_bool) (((response >> MMC_SHIFT31) == 1) ? 1 : 0);
    }   */
    validvoltage = FALSE;
    response = 0;

    /* MMC_CARD */
    while ((!validvoltage) && (MMC_MAXVOLTTRIAL > count))
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_VOLTAGEWINDOWMMC | MMC_SECTOR_MODE /*address_mode*/ ;    /* voltage window */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEND_OP_COND |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

        error = mmc_CmdResp3Error(mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        /*
        for (delay = 0; delay < 1; delay++)
        {
            temp = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_periph_id0;
        }

        temp = temp;
        */
        response = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;
        validvoltage = (t_bool) (((response >> MMC_SHIFT31) == 1) ? 1 : 0);

        /*NO VOLTAGE SETTING IN MuPoC-FULL */
        count++;
    }

    if (MMC_MAXVOLTTRIAL <= count)
    {
        error = MMC_INVALID_VOLTRANGE;
        DBGEXIT0(error);
        return(error);
    }

    if (response & MMC_SECTOR_MODE)
    {
        g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_HIGH_CAPACITY_MMC_CARD;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.address_mode = MMC_SECT_ADDRESSABLE;
    }
    else
    {
        g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_MULTIMEDIA_CARD;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.address_mode = MMC_BYTE_ADDRESSABLE;
    }

    return(error);
}

PUBLIC  t_mmc_address_mode  MMC_GetAddressingMode(t_mmc_device_id mmc_device_id)
{
    t_mmc_address_mode AddressingMode;
    DBGENTER0();
    AddressingMode = g_mmc_system_context[mmc_device_id].current_card.mmc_context.address_mode;

    DBGEXIT0(AddressingMode); 
    return(AddressingMode);
}

PUBLIC t_mmc_error MMC_PowerON(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error         error = MMC_OK;
  /*  t_uint32            temp = 0, delay;
    t_uint32            one_msec = 0;*/
    t_mmc_sdio_resp4    sdio_resp;
    t_uint32            address_mode = MMC_BYTE_MODE;

    DBGENTER0();

    /*ASSIGN DEFAULT VALUES TO GLOBAL VARIABLE */
    mmc_ResetGV(mmc_device_id);
    g_mmc_system_context[mmc_device_id].no_of_cards = 0;

    /*Power ON, OPEN_DRAIN, CMD AND DIR ENABLE SIGNAL */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power = MMC_POWERON | MMC_OPENDRAIN;

    /* CLOCK FREQ 400 KHZ, CLK ENABLE*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock = MMC_CLKDIVINIT | MMC_CLKENABLE;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 &= ~MMCALLINTERRUPTS;

 /*   one_msec = 52000 / ((g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock & 0xFF) + 2); */

    /* SDIO Support*/
    /* BUS MODE PUSH_PULL MODE */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power &= ~MMC_OPENDRAIN;

    sdio_resp.card_ready_after_init = MMC_ALLZERO;
    sdio_resp.no_of_io_funcs = MMC_ALLZERO;
    sdio_resp.memory_present = MMC_ALLZERO;
    sdio_resp.op_cond_register = MMC_ALLZERO;

    /* ASSUME SDIO CARD, send CMD5 */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_SEND_OP_COND |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;

    error = mmc_CmdResp4Error(MMCIO_SEND_OP_COND, &sdio_resp, mmc_device_id);

    if (MMC_OK == error)
    {
        error = mmc_PowerON_SDIO(mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else
    {
        /*Send CMD0 GO_IDLE_STATE*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCGO_IDLE_STATE &~MMCRESPEXPECTED | MMCCMDPATHENABLE;

        error = mmc_CmdError(mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        /*ASSUME IT IS A SD-CARD. SEND CMD41. IF IT IS A MULTIMEDIA CARD, IT WILL NOT RESPOND TO CMD.*/
        /* BUS MODE PUSH_PULL MODE */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power &= ~MMC_OPENDRAIN;

        /* send CMD8 to verify SD card interface operating condition */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = MMC_CHECK_PATTERN;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSD_SEND_IF_COND |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

        error = mmc_CmdResp7Error(mmc_device_id);
        if (MMC_OK == error)
        {
            address_mode = MMC_SECTOR_MODE;
        }
        else
        {
            /*Send CMD0 GO_IDLE_STATE*/
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCGO_IDLE_STATE &~MMCRESPEXPECTED | MMCCMDPATHENABLE;

            error = mmc_CmdError(mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        }

        address_mode = MMC_SECTOR_MODE;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

/*        for (delay = 0; delay < (one_msec * 27); delay++);*/
            

        error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);

        /* IF ERROR IS COMMAND TIMEOUT IT IS MMC CARD */
        if (MMC_OK == error)
        {
            error = mmc_PowerON_SD(mmc_device_id,address_mode);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        }
        else if (MMC_CMD_RSP_TIMEOUT == error)
        {
            error = mmc_PowerON_MMC(mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        }

       /* temp = temp;*/
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_PowerOFF                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine turns the supply output voltage off.           */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_PowerOFF(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

#if defined(SDIO_SUPPORT)
    t_uint8     card_index = 1, io_direct_data;
    t_uint32    argument = 0;
#endif
    DBGENTER0();

#if defined(SDIO_SUPPORT)
    while (card_index <= g_mmc_system_context[mmc_device_id].no_of_cards)
    {
        if
        (
            MMC_SECURE_DIGITAL_IO_CARD == g_mmc_system_context[mmc_device_id].card_array[card_index - 1].card_type
            ||  MMC_SECURE_DIGITAL_IO_COMBO_CARD == g_mmc_system_context[mmc_device_id].card_array[card_index - 1].
                card_type
        )
        {
            if (card_index != g_mmc_system_context[mmc_device_id].current_card.selected_card)
            {
                /*SEND CMD7 MMCSEL_DESEL_CARD */
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) (g_mmc_system_context[mmc_device_id].card_array[card_index - 1].rca) << MMC_SHIFT16;
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
                    MMCRESPEXPECTED |
                    MMCCMDPATHENABLE;
                error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
                if (MMC_OK != error)
                {
                    DBGEXIT0(error);
                    return(error);
                }
            }

            /* Reset by writing to I/O Abort register */
            argument = (t_uint32) MASK_BIT0 | (t_uint32) 0x06 << MMC_SHIFT9 |   /* Address of I/O Abort register CCCR */
            (t_uint32) 0x0 << MMC_SHIFT28 | /* Select function 0, CIA */
            (t_uint32) 0x0 << MMC_SHIFT27 | /* No read after write */
            (t_uint32) MMC_SDIO_WRITE << MMC_SHIFT31;

            /* SEND IO DIRECT COMMAND */
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
            error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        }

        card_index++;
    }
#endif
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power &= ~MMC_POWERON;

    /*ASSIGN DEFAULT VALUES TO GLOBAL VARIABLE */
    mmc_ResetGV(mmc_device_id);
    g_mmc_system_context[mmc_device_id].no_of_cards = 0;

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_InitializeCards                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine intialises all cards or single card as the     */
/*              case may be. Card(s) come into standby state.               */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PRIVATE t_mmc_error mmc_initializeSDIOcard(t_uint16 index, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint16    rca;
    t_uint32    delay, argument;
    t_uint8     io_direct_data;

    //temp = temp;
    rca = g_mmc_system_context[mmc_device_id].card_array[index - 1].rca;

    // get rca from system context !!
    /* CLOCK FREQ 400 KHZ*/
    MMC_SET_CLKDIV(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock, (t_uint32) MMC_SDIOCLKDIV);

    /*SEND CMD7 MMCSEL_DESEL_CARD */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) rca << MMC_SHIFT16;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    
    error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }
    else
    {
        g_mmc_system_context[mmc_device_id].current_card.selected_card = (t_uint8) index;
    }

    /* Read SDIO SDIO/CCCR revision Information*/
    argument = (t_uint32) 0x00 << MMC_SHIFT9 |                      /* Address of SDIO/CCCR revision register of CCCR */
    (t_uint32) 0x0 << MMC_SHIFT28 |                                 /* Select function 0, CIA */
    (t_uint32) 0x0 << MMC_SHIFT27 |                                 /* No read after write */
    (t_uint32) MMC_SDIO_READ << MMC_SHIFT31;

    /* SEND IO DIRECT COMMAND */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error
        (
            MMCIO_RW_DIRECT,
            &g_mmc_system_context[mmc_device_id].card_array[index - 1].sdio_cccr[1],
            mmc_device_id
        );
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* Read SDIO Specification Information */
    argument = (t_uint32) 0x01 << MMC_SHIFT9 |                      /* Address of SDIO Specification register of CCCR */
    (t_uint32) 0x0 << MMC_SHIFT28 |                                 /* Select function 0, CIA */
    (t_uint32) 0x0 << MMC_SHIFT27 |                                 /* No read after write */
    (t_uint32) MMC_SDIO_READ << MMC_SHIFT31;

    /* SEND IO DIRECT COMMAND */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error
        (
            MMCIO_RW_DIRECT,
            &g_mmc_system_context[mmc_device_id].card_array[index - 1].sdio_cccr[2],
            mmc_device_id
        );
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* Read SDIO Card Capability Information*/
    argument = (t_uint32) 0x07 << MMC_SHIFT9 |                      /* Address of Card Capability register of CCCR */
    (t_uint32) 0x0 << MMC_SHIFT28 |                                 /* Select function 0, CIA */
    (t_uint32) 0x0 << MMC_SHIFT27 |                                 /* No read after write */
    (t_uint32) MMC_SDIO_READ << MMC_SHIFT31;

    /* SEND IO DIRECT COMMAND */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error
        (
            MMCIO_RW_DIRECT,
            &g_mmc_system_context[mmc_device_id].card_array[index - 1].sdio_cccr[3],
            mmc_device_id
        );
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* IO ENABLE for all functions */
    argument = (t_uint32) 0xFE | (t_uint32) 0x02 << MMC_SHIFT9 |    /* Address of I/O Enable register CCCR */
    (t_uint32) 0x0 << MMC_SHIFT28 |                 /* Select function 0, CIA */
    (t_uint32) 0x1 << MMC_SHIFT27 |                 /* No read after write */
    (t_uint32) MMC_SDIO_WRITE << MMC_SHIFT31;

    /* SEND IO DIRECT COMMAND */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* Delay for IO functions to be enabled */
    for (delay = 0; delay < 100; delay++)
    {
        /* Read IO Ready register */
        argument = (t_uint32) 0x03 << MMC_SHIFT9 |  /* Address of I/O Enable register CCCR */
        (t_uint32) 0x0 << MMC_SHIFT28 |             /* Select function 0, CIA */
        (t_uint32) 0x0 << MMC_SHIFT27 |             /* No read after write */
        (t_uint32) MMC_SDIO_READ << MMC_SHIFT31;

        /* SEND IO DIRECT COMMAND */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        if (io_direct_data == (t_uint8) (MASK_BYTE & (~MASK_BIT0)))
        {
            break;
        }

       // temp = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_periph_id0;
    }

    if (0 == io_direct_data)
    {
        error = MMC_SDIO_FUNCTION_FAILED;
        DBGEXIT0(error);
        return(error);
    }
    else
    {
        g_mmc_system_context[mmc_device_id].card_array[index - 1].sdio_cccr[0] = io_direct_data;
    }

    return(error);
}

PUBLIC t_mmc_error MMC_InitializeCards(IN t_mmc_device_id mmc_device_id)
{
    PRIVATE t_uint16 rca;
    t_mmc_error     error = MMC_OK;
    t_uint16        index = 1;
    t_uint32        temp_cid[4];

    /*CHECK FOR POWER ON */
    DBGENTER0();

    /* INITIALIZATION TO REMOVE WARNINGS */
    temp_cid[0] = 0x0;
    temp_cid[1] = 0x0;
    temp_cid[2] = 0x0;
    temp_cid[3] = 0x0;
    rca = 0x1;
    if (0x00 == (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power & MMC_POWERON))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    if (MMC_SECURE_DIGITAL_IO_CARD != g_mmc_system_context[mmc_device_id].current_card.cardtype)
    {
        /* SEND CMD2 ALL_SEND_CID */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x00000000;

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCALL_SEND_CID |
            MMCRESPEXPECTED |
            MMCLONGRESPONSE |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp2Error(mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        temp_cid[0] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response3;
        temp_cid[1] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response2;
        temp_cid[2] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response1;
        temp_cid[3] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;
    }

    if
    (
        MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype
    ||  MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype
    )
    {
        /*SEND CMD3 SET_REL_ADDR with RCA defines as 1 */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) rca << MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_REL_ADDR |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

        error = mmc_CmdResp1Error(MMCSET_REL_ADDR, mmc_device_id);
        if (!((MMC_OK == error) || (MMC_ILLEGAL_CMD == error)))
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if
        (
            MMC_SECURE_DIGITAL_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype
        ||  MMC_SECURE_DIGITAL_IO_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype
        ||  MMC_SECURE_DIGITAL_IO_COMBO_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype
        ||  MMC_HIGH_CAPACITY_SD_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype
        )
    {
        /*SEND CMD3 SET_REL_ADDR with argument 0 */
        /*SD-CARD PUBLISHES ITS RCA. */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_REL_ADDR |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp6Error(MMCSET_REL_ADDR, &rca, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].card_array[index - 1].rca = (t_uint16) rca;
    }

    if (MMC_SECURE_DIGITAL_IO_CARD != g_mmc_system_context[mmc_device_id].current_card.cardtype)
    {
        g_mmc_system_context[mmc_device_id].card_array[index - 1].cid[0] = temp_cid[0];
        g_mmc_system_context[mmc_device_id].card_array[index - 1].cid[1] = temp_cid[1];
        g_mmc_system_context[mmc_device_id].card_array[index - 1].cid[2] = temp_cid[2];
        g_mmc_system_context[mmc_device_id].card_array[index - 1].cid[3] = temp_cid[3];
        g_mmc_system_context[mmc_device_id].card_array[index - 1].rca = (t_uint16) rca;

        /*SEND CMD9  SEND_CSD with argument as card's RCA*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) (g_mmc_system_context[mmc_device_id].card_array[index - 1].rca) << MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEND_CSD |
            MMCRESPEXPECTED |
            MMCLONGRESPONSE |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp2Error(mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].card_array[index - 1].csd[0] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response3;
        g_mmc_system_context[mmc_device_id].card_array[index - 1].csd[1] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response2;
        g_mmc_system_context[mmc_device_id].card_array[index - 1].csd[2] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response1;
        g_mmc_system_context[mmc_device_id].card_array[index - 1].csd[3] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;
    }

    if (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype)
    {
        if
        (
            MMC_SPECHIGHSPEED ==
                ((g_mmc_system_context[mmc_device_id].card_array[index - 1].csd[3] & MMC_SPECVERSION) >> MMC_SHIFT26)
        )
        {
            g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_HIGH_SPEED_MULTIMEDIA_CARD;
        }
    }

#if defined(SDIO_SUPPORT)
    if (MMC_SECURE_DIGITAL_IO_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype)
    {
        error = mmc_initializeSDIOcard(index, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
#endif
    g_mmc_system_context[mmc_device_id].card_array[index - 1].card_type = g_mmc_system_context[mmc_device_id].current_card.cardtype;
    g_mmc_system_context[mmc_device_id].no_of_cards++;
    error = MMC_OK; /*All cards get intialized */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power &= ~MMC_OPENDRAIN;    /* PUSHPULL mode after intialization */

#ifndef ST_8500_ED
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock |= MMC_CLOCK_MASK_PWRSAVE;    /* Put card in Power Save mode */
#endif

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_GetNumOfActivecards                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns no of cards currently active and       */
/*               can actually serve requests.                               */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_uint8                                                     */

/****************************************************************************/
PUBLIC t_uint8 MMC_GetNumOfActivecards(IN t_mmc_device_id mmc_device_id)
{
    DBGENTER0();
    DBGEXIT0(g_mmc_system_context[mmc_device_id].no_of_cards);
    return(g_mmc_system_context[mmc_device_id].no_of_cards);
}

/****************************************************************************/
/*       NAME : MMC_GetCardInfo                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns information about specific card.       */
/* PARAMETERS :                                                             */
/*         IN : t_uint8          Cardno for which information is needed.    */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : t_mmc_card_info *p_cardinfo contains cid, csd, rca, cccr    */
/*                               registers depending upon the card type     */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_GetCardInfo(IN t_uint8 cardno, IN t_mmc_device_id mmc_device_id, OUT t_mmc_card_info *p_cardinfo)
{
    t_mmc_error error = MMC_OK;
    DBGENTER1("accessing info for card %d\n", cardno);
#ifdef __DEBUG
    if (NULL == p_cardinfo)
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }
#endif

    if ((cardno <= g_mmc_system_context[mmc_device_id].no_of_cards) && (cardno > 0))
    {
        p_cardinfo->cid[0] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].cid[0];
        p_cardinfo->cid[1] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].cid[1];
        p_cardinfo->cid[2] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].cid[2];
        p_cardinfo->cid[3] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].cid[3];
        p_cardinfo->csd[0] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[0];
        p_cardinfo->csd[1] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[1];
        p_cardinfo->csd[2] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2];
        p_cardinfo->csd[3] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[3];
        p_cardinfo->rca = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].rca;
        p_cardinfo->card_type = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type;
#if defined(SDIO_SUPPORT)
        p_cardinfo->sdio_cccr[0] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].sdio_cccr[0];
        p_cardinfo->sdio_cccr[1] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].sdio_cccr[1];
        p_cardinfo->sdio_cccr[2] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].sdio_cccr[2];
        p_cardinfo->sdio_cccr[3] = g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].sdio_cccr[3];
#endif
    }
    else
    {
        error = MMC_INVALID_PARAMETER;
    }

    DBGEXIT0(error);
    return(error);
}

PRIVATE t_mmc_error mmc_SDEnableWideBusOperation
(
    IN t_uint8          cardno,
    IN t_mmc_wide_mode  wide_mode,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    if (MMC_8_BIT_WIDE == wide_mode)
    {
        error = MMC_UNSUPPORTED_FEATURE;
        DBGEXIT0(error);
        return(error);
    }
    else if (MMC_4_BIT_WIDE == wide_mode)
    {
        error = mmc_SDEnWideBus(cardno, MMC_ENABLE, mmc_device_id);
        g_mmc_system_context[mmc_device_id].current_card.wide_mode = MMC_ENABLE;
    }
    else
    {
        error = mmc_SDEnWideBus(cardno, MMC_DISABLE, mmc_device_id);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_MMCEnableWideBusOperation
(
    IN t_uint8          cardno,
    IN t_mmc_wide_mode  wide_mode,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    if (MMC_1_BIT_WIDE == wide_mode)
    {
        error = mmc_TestFunctionalPins(cardno, MMC_1_BIT_WIDE, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        error = mmc_HSEnWideBus(cardno, MMC_1_BIT_WIDE, mmc_device_id);
        if (MMC_OK == error)
        {
            g_mmc_system_context[mmc_device_id].current_card.wide_mode = MMC_DISABLE;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock &= ~MMC_CLOCK_WIDEBUS_MASK;
        }
    }

    /*ADDED SUPPORT TO 8-BIT*/
    else if (MMC_8_BIT_WIDE == wide_mode)
    {
        error = mmc_TestFunctionalPins(cardno, MMC_8_BIT_WIDE, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        error = mmc_HSEnWideBus(cardno, MMC_8_BIT_WIDE, mmc_device_id);
        if (MMC_OK == error)
        {
            g_mmc_system_context[mmc_device_id].current_card.wide_mode = MMC_ENABLE;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock &= ~MMC_CLOCK_WIDEBUS_MASK;
        }
    }
    else
    {
        error = mmc_TestFunctionalPins(cardno, wide_mode, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        error = mmc_HSEnWideBus(cardno, wide_mode, mmc_device_id);
        if (MMC_OK == error)
        {
            g_mmc_system_context[mmc_device_id].current_card.wide_mode = MMC_ENABLE;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock |= (MMC_CLOCK_WIDEBUS_MASK & ((t_uint32) wide_mode << MMC_SHIFT11));
        }
    }

    return(error);
}

/****************************************************************************/
/*       NAME : MMC_EnableWideBusOperation                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables wide bus opeartion for the requeseted  */
/*              card if supported by card                                   */
/* PARAMETERS :                                                             */
/*         IN : t_uint8 cardno: cardno for which this request is intended.  */
/*         IN : t_mmc_wide_mode : Kind of wide operation to be enabled      */
/*                              (i.e. 4 bit wide or 8 bit wide or           */
/*                                      reset to 1 bit mode ).              */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_EnableWideBusOperation
(
    IN t_uint8          cardno,
    IN t_mmc_wide_mode  wide_mode,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    DBGENTER2("cardno(%d) widen (%d)", cardno, wide_mode);
#ifdef __DEBUG
    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }
#endif

    /*MMC CARD DOESN'T SUPPORT THIS FEATURE */
    if (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    {
        error = MMC_UNSUPPORTED_FEATURE;
        DBGEXIT0(error);
        return(error);
    }
    else if
        (
            (MMC_SECURE_DIGITAL_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        ||  (MMC_HIGH_CAPACITY_SD_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        )
    {
        error = mmc_SDEnableWideBusOperation(cardno, wide_mode, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if
        (
            (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        )
    {
        error = mmc_MMCEnableWideBusOperation(cardno, wide_mode, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

#if defined SDIO_SUPPORT
    else if (MMC_SECURE_DIGITAL_IO_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    {
        error = mmc_SDIOEnWideBus(cardno, wide_mode, mmc_device_id);
        if (MMC_OK == error)
        {
            g_mmc_system_context[mmc_device_id].current_card.wide_mode = MMC_ENABLE;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock |= (MMC_CLOCK_WIDEBUS_MASK & ((t_uint32) wide_mode << MMC_SHIFT11));
        }
    }
    else if (MMC_SECURE_DIGITAL_IO_COMBO_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    {
        error = mmc_SDIOEnWideBus(cardno, wide_mode, mmc_device_id);
        if (MMC_OK == error)
        {
            if (MMC_4_BIT_WIDE == wide_mode)
            {
                error = mmc_SDEnWideBus(cardno, MMC_ENABLE, mmc_device_id);
            }
            else
            {
                error = mmc_SDEnWideBus(cardno, MMC_DISABLE, mmc_device_id);
            }
        }

        if (MMC_OK == error)
        {
            g_mmc_system_context[mmc_device_id].current_card.wide_mode = MMC_ENABLE;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock |= (MMC_CLOCK_WIDEBUS_MASK & ((t_uint32) wide_mode << MMC_SHIFT11));
        }
    }
#endif
    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_SetIRQMode                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine set the card in interrupt mode.                */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_state     IRQ mode to set to reset.                   */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetIRQMode(IN t_mmc_state irqstate, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    DBGENTER1("irqstate(%d)", irqstate);
#ifdef __DEBUG
    if
    (
        (MMC_MULTIMEDIA_CARD != g_mmc_system_context[mmc_device_id].current_card.cardtype)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD != g_mmc_system_context[mmc_device_id].current_card.cardtype)
    ||  (MMC_HIGH_CAPACITY_MMC_CARD != g_mmc_system_context[mmc_device_id].current_card.cardtype)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }
#endif

    /*IRQ REQUEST IS APPLICABLE ONLY IN STAND_BY_STATE */
    /*SO IF A CARD IS SELECTED, DESELECT IT */
    /*CARD WILL GIVE CMDRESP TIMEOUT */
    if (0 != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_CMD_RSP_TIMEOUT != error)
        {
            error = MMC_CC_ERROR;
            DBGEXIT0(error);
            return(error);
        }

        /*CARD IS DESELECTED. */
        error = MMC_OK;
        g_mmc_system_context[mmc_device_id].current_card.selected_card = 0;
    }

    /*SEND CMD40 GO_IRQ_STATE */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x00000000;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCGO_IRQ_STATE |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;

    error = mmc_CmdResp1Error(MMCGO_IRQ_STATE, mmc_device_id);
    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_SetDeviceMode                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets device mode whether to operate in Polling,*/
/*              Interrupt, dma mode.                                        */
/* PARAMETERS :                                                             */
/*         IN : t_mmc_device_mode        mode to for further transmission.  */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetDeviceMode(IN t_mmc_device_mode devicemode, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    DBGENTER1("device mode(%d)", devicemode);
    switch (devicemode)
    {
        case MMC_POLLING_MODE:
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode = MMC_POLLING_MODE;
            break;

        case MMC_INTERRUPT_MODE:
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode = MMC_INTERRUPT_MODE;
            break;

        case MMC_DMA_MODE:
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode = MMC_DMA_MODE;
            break;

        default:
            error = MMC_INVALID_PARAMETER;
            break;
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_ReadBytes                                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to read bytes from specified address    */
/*              in a card                                                   */
/* PARAMETERS :                                                             */
/*         IN : t_uint8 cardno: card to access                              */
/*              t_uint32 addr : address from where to start reading         */
/*              t_uint16 no_of_bytes: no. of bytes to read                  */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : t_uint32* p_readbuff: buffer to store data                  */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PRIVATE t_mmc_error mmc_ReadBytes_openend
(
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint32         no_of_bytes,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    /*MAXIMUM NO OF BYTES CAN BE TRANSFFERED IS ((0xFFFF/4)*4)  or ((0x1FFFFFF/4)*4) */
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = MMC_MAXBYTES;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = MMC_MAXBYTES;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMCREADDIR | MMCSTREAMMODE | MMCDATAPATHENABLE;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_readbuff;

    /*SEND CMD11 READ_DAT_UNTIL_STOP*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCREAD_DAT_UNTIL_STOP |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCREAD_DAT_UNTIL_STOP, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
        MMCDATATIMEOUT |
        MMCDATAEND |
        MMCRXFIFOHALFFULL |
        MMCRXOVERRUN;

    return(error);
}

PRIVATE t_mmc_error mmc_ReadBytes
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint32         no_of_bytes,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    count;
    t_uint32    *p_tempbuff = p_readbuff;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = no_of_bytes;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = no_of_bytes;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMCREADDIR | MMCSTREAMMODE | MMCDATAPATHENABLE;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_readbuff;

    /*SEND CMD11 READ_DAT_UNTIL_STOP*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCREAD_DAT_UNTIL_STOP |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCREAD_DAT_UNTIL_STOP, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    if (MMC_POLLING_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;

        while
        (
            !(
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                    (MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATAEND)
            )
        )
        {
            if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXFIFOHALFFULL)
            {
                for (count = 0; count < MMC_HALFFIFO; count++)
                {
                    *(p_tempbuff + count) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
                }

                p_tempbuff += MMC_HALFFIFO;
            }
        }

        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
            error = MMC_DATA_TIMEOUT;
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_TIMEOUT;
            DBGEXIT0(error);
            return(error);
        }
        else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
            error = MMC_DATA_CRC_FAIL;
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_CRC_FAIL;
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;  /* clear all the static status flags */
        while (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXDATAAVLBL)
        {
            *p_tempbuff = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
            p_tempbuff++;
        }

        /* SEND CMD12  STOP_TRANSMISSION */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x00000000;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSTOP_TRANSMISSION |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSTOP_TRANSMISSION, mmc_device_id);
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error;
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if (MMC_INTERRUPT_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCRXFIFOHALFFULL |
            MMCRXOVERRUN;
    }
    else if (MMC_DMA_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCRXOVERRUN;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDMAENABLE;
    }

    return(error);
}

PUBLIC t_mmc_error MMC_ReadBytes
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint32         no_of_bytes,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    DBGENTER3("cardno(%d) addr(%d) no_of_bytes(%d)", cardno, addr, no_of_bytes);

    /*SD-Card DOESN'T SUPPORT R/W IN STREAM MODE */
    if
    (
        (MMC_SECURE_DIGITAL_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_SECURE_DIGITAL_IO_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_CAPACITY_SD_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_UNSUPPORTED_FEATURE;
        DBGEXIT0(error);
        return(error);
    }
    else if
        (
            (
                (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
            ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
            )
        &&  (MMC_ENABLE == g_mmc_system_context[mmc_device_id].current_card.wide_mode)
        )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

#ifdef __DEBUG
    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }
#endif
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = 0;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMC_ALLZERO;

    /* SEND COMMAND TO SELECT THE CARD. */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    /* now depending on parameter no_of_bytes, send command READ_DAT_UNTIL_STOP */
    /* no_of_bytes == 0, OPEN-ENDED TRANSFER. SUPPORTED ONLY IN INTERRUPT MODE */
    if (0 == no_of_bytes)
    {
        if (MMC_INTERRUPT_MODE != g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
        {
            error = MMC_REQUEST_NOT_APPLICABLE;
            DBGEXIT0(error);
            return(error);
        }

        error = mmc_ReadBytes_openend(addr, p_readbuff, no_of_bytes, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    /* no_of_bytes defined */
    else if (no_of_bytes > 0)
    {
        error = mmc_ReadBytes(cardno, addr, p_readbuff, no_of_bytes, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_WriteBytes                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to write bytes starting from a specified*/
/*              address in a card                                           */
/* PARAMETERS :                                                             */
/*         IN : t_uint8 cardno: card to access                              */
/*              t_uint32 addr : address where to start writing              */
/*              t_uint32* p_writebuff: source buffer                        */
/*              t_uint16 no_of_bytes: no. of bytes to write                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PRIVATE t_mmc_error mmc_WriteBytes_poll_mode
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint32         no_of_bytes,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    t_uint32    *p_tempbuff = p_writebuff;
    t_uint32    count, rest_words, bytes_transferred = 0;
    t_uint8     cardstate;

    g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATAEND)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
        {
            if
            (
                MMC_HALFFIFOBYTES >
                    (
                        g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                        bytes_transferred
                    )
            )
            {
                rest_words =
                    (
                        (
                            (
                                g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                bytes_transferred
                            ) % MMC_WORDDIV
                        ) == 0
                    ) ?
                        (
                            (
                                g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                bytes_transferred
                            ) / MMC_WORDDIV
                        ) :
                            (
                                (
                                    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                    bytes_transferred
                                ) / MMC_WORDDIV + 1
                            );
                for (count = 0; count < rest_words; count++, p_tempbuff++, bytes_transferred += MMC_BYTEINC)
                {
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *p_tempbuff;
                }
            }
            else
            {
                for (count = 0; count < MMC_HALFFIFO; count++)
                {
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *(p_tempbuff + count);
                }

                p_tempbuff += MMC_HALFFIFO;
                bytes_transferred += MMC_HALFFIFOBYTES;
            }
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;  /* clear all the static status flags */

    /*SEND CMD12  STOP_TRANSMISSION*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x00000000;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSTOP_TRANSMISSION |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = mmc_CmdResp1Error
        (
            MMCSTOP_TRANSMISSION,
            mmc_device_id
        );
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_WriteBytes_mode_sel
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint32         no_of_bytes,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    if (MMC_POLLING_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        error = mmc_WriteBytes_poll_mode(cardno, addr, p_writebuff, no_of_bytes, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if (MMC_INTERRUPT_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXFIFOHALFEMPTY |
            MMCTXUNDERRUN;
    }
    else if (MMC_DMA_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXUNDERRUN;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDMAENABLE;
    }

    return(error);
}

PRIVATE t_mmc_error mmc_WriteBytes_err_chk
(
    IN t_uint8          cardno,
    IN t_uint32         *p_writebuff,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    if
    (
        (
            (MMC_SECURE_DIGITAL_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        ||  (MMC_SECURE_DIGITAL_IO_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        ||  (MMC_HIGH_CAPACITY_SD_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        )
    &&  (
            (
                (
                    MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].
                        card_type
                )
            ||  (
                        MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].
                            card_type
                    )
            ) && (MMC_ENABLE == g_mmc_system_context[mmc_device_id].current_card.wide_mode)
        )
    )
    {
        error = MMC_UNSUPPORTED_FEATURE;
        DBGEXIT0(error);
        return(error);
    }

    if ((NULL == p_writebuff) && ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno)))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = 0;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMC_ALLZERO;

    /* SEND COMMAND TO SELECT THE CARD. */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

PUBLIC t_mmc_error MMC_WriteBytes
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint32         no_of_bytes,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    DBGENTER3("cardno(%d) addr(%d) no_of_bytes(%d)", cardno, addr, no_of_bytes);

    error = mmc_WriteBytes_err_chk(cardno, p_writebuff, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*SD-Card DOESN'T SUPPORT R/W IN STREAM MODE */
    /* now depending on parameter no_of_bytes, send command WRITE_DAT_UNTIL_STOP */
    /* no_of_bytes == 0, OPEN-ENDED TRANSFER. SUPPORTED ONLY IN INTERRUPT MODE */
    if (0 == no_of_bytes)
    {
        if (MMC_INTERRUPT_MODE != g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
        {
            return(MMC_REQUEST_NOT_APPLICABLE);
        }

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

        g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = MMC_MAXBYTES;

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = MMC_MAXBYTES;

        g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer = p_writebuff;

        /*SEND CMD20 WRITE_DAT_UNTIL_STOP*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCWRITE_DAT_UNTIL_STOP |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCWRITE_DAT_UNTIL_STOP, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMCSTREAMMODE &~MMCREADDIR | MMCDATAPATHENABLE;

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCTXFIFOHALFEMPTY |
            MMCDATAEND |
            MMCTXUNDERRUN;
    }
    else
    {
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = no_of_bytes;

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = no_of_bytes;

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

        g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer = p_writebuff;

        /*SEND CMD20 WRITE_DAT_UNTIL_STOP*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCWRITE_DAT_UNTIL_STOP |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCWRITE_DAT_UNTIL_STOP, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMCSTREAMMODE &~MMCREADDIR | MMCDATAPATHENABLE;

        error = mmc_WriteBytes_mode_sel(cardno, addr, p_writebuff, no_of_bytes, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_ReadBlocks                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to read blocks from a specified         */
/*              address in a card                                           */
/* PARAMETERS :                                                             */
/*         IN : t_uint8 cardno: card to access                              */
/*              t_uint32 addr : address from where to start reading         */
/*              t_uint16 blocksize : size of block in bytes                 */
/*              t_uint16 no_of_blocks: no. of blocks to read                */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : t_uint32* p_readbuff: source buffer                         */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/

PRIVATE t_mmc_error mmc_ReadBlock_mode_handler
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    volatile t_uint32 status=0x0;
    
    if (MMC_POLLING_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        /* POLLING MODE */
        do
                {
            status=g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
                    if (status & MMCRXDATAAVLBL)
            {
                *p_readbuff = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
                p_readbuff++;
            }
                }
                while( (status & MMCRXACTIVE) && !(status & (MMCRXOVERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCSTARTBITERROR)) );
                    
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_TIMEOUT;
            DBGEXIT0(MMC_DATA_TIMEOUT);
            return(MMC_DATA_TIMEOUT);
        }
        else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_CRC_FAIL;
            DBGEXIT0(MMC_DATA_CRC_FAIL);
            return(MMC_DATA_CRC_FAIL);
        }
        else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXOVERRUN)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCRXOVERRUN;
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_RX_OVERRUN;
            DBGEXIT0(MMC_RX_OVERRUN);
            return(MMC_RX_OVERRUN);
        }
        else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_START_BIT_ERR;
            DBGEXIT0(MMC_START_BIT_ERR);
            return(MMC_START_BIT_ERR);
        }

        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_OK;

        /*clear all the static status flags*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;
    }
    else if (MMC_INTERRUPT_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCRXFIFOHALFFULL |
            MMCRXOVERRUN |
            MMCSTARTBITERROR;
    }
    else if (MMC_DMA_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCRXOVERRUN |
            MMCSTARTBITERROR;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDMAENABLE;
    }

    return(MMC_OK);
}

PRIVATE t_mmc_error mmc_ReadBlocks
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id,
    IN t_bool           force_multiblock_read
)
{
    t_mmc_error error = MMC_OK;

    /*COMMON TO ALL MODES */
    if (MMC_MAXDATALENGTH < (no_of_blocks * blocksize))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*COMMON TO ALL MODES */
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = (no_of_blocks * blocksize);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    /* ENABLE DATA PATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCREADDIR &~MMCSTREAMMODE | MMCDATAPATHENABLE;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_readbuff;

    if ((1 == no_of_blocks) && (!force_multiblock_read))
    {
        /*SEND CMD17 READ_SINGLE_BLOCK */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCREAD_SINGLE_BLOCK |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCREAD_SINGLE_BLOCK, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        /*IN CASE OF SINGLE BLOCK TRANSFER, NO NEED OF STOP TRANSFER AT ALL.*/
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
    }
    else
    {
        if
        (
            (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        )
        {
            /*SEND CMD23 SET_BLOCK_COUNT, argument = no_of_blocks */
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) no_of_blocks;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCK_COUNT |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
            error = mmc_CmdResp1Error(MMCSET_BLOCK_COUNT, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

            g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;
        }

        /*SEND CMD18 READ_MULT_BLOCK with argument data address*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCREAD_MULT_BLOCK |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCREAD_MULT_BLOCK, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    error = mmc_ReadBlock_mode_handler(cardno, addr, p_readbuff, blocksize, no_of_blocks, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_ReadBlocks_openend
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = (MMC_MAXDATALENGTH / (t_uint32) blocksize) * (t_uint32) blocksize;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = (t_uint32) ((MMC_MAXDATALENGTH / blocksize) * blocksize);

    if
    (
        (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        /*SET MAX NO OF BLOCKS TO BE TRANSFERRED IN CASE OF MULTIMEDIA CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (MMC_MAXDATALENGTH / (t_uint32) blocksize);
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCK_COUNT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCK_COUNT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
    }
    else
    {   /*FOR SD-CARD STOP COMMAND TO BE SENT */
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;
    }

    /*FILL UP DATA CTRL REGISTER. */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCREADDIR &~MMCSTREAMMODE | MMCDATAPATHENABLE;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_readbuff;

    /*SEND CMD18 READ_MULT_BLOCK WITH ARGUMENT AS ADDRESS */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCREAD_MULT_BLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCREAD_MULT_BLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
        MMCDATATIMEOUT |
        MMCRXFIFOHALFFULL |
        MMCRXOVERRUN |
        MMCDATAEND |
        MMCSTARTBITERROR;

    return(error);
}

PUBLIC t_mmc_error MMC_ReadBlocks
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    
    #if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
    t_uint8    power;
    #else
    t_uint32    power;
    #endif

    DBGENTER4("cardno(%d) addr(%d)blocksize(%d)no_of_blocks(%d)", cardno, addr, blocksize, no_of_blocks);

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = 0;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMC_ALLZERO;

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

#if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
    if ((blocksize > 0) && (blocksize <= MMC_MAXBLSIZE) && (0 == (blocksize & (blocksize - 1))))
    {
        power = mmc_convert_from_bytes_to_power_of_two(blocksize);

        /*SET BLOCK SIZE FOR CONTROLLER.*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) power << MMC_SHIFT4;

#else
        if ((blocksize > 0) && (0 == (blocksize & (blocksize - 1))))
        {
         power = blocksize;   
        /*SET BLOCK SIZE FOR CONTROLLER.*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) power << MMC_SHIFT16;
#endif

        /*SET BLOCK SIZE FOR CARD IF IT IS NOT EQUAL TO CURRENT BLOCK SIZE FOR CARD. */
        if (power != g_mmc_system_context[mmc_device_id].current_card.blk_length)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) blocksize;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
            error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

            g_mmc_system_context[mmc_device_id].current_card.blk_length = power;
        }
    }
    else
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /* no_of_blocks = 0, OPEN_ENDED TRANSMISSION */
    /*FOR OPEN_ENDED TRANSFER.SUPPORTED ONLY IN INTERRUPT MODE */
    if (0 == no_of_blocks)
    {
        if (MMC_INTERRUPT_MODE != g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
        {
            return(MMC_REQUEST_NOT_APPLICABLE);
        }

        error = mmc_ReadBlocks_openend(cardno, addr, p_readbuff, blocksize, no_of_blocks, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else
    {
        error = mmc_ReadBlocks(cardno, addr, p_readbuff, blocksize, no_of_blocks, mmc_device_id, FALSE);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    DBGEXIT0(error);
    return(error);
}

PRIVATE t_mmc_error mmc_WriteBlocks_openend
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = (MMC_MAXDATALENGTH / (t_uint32) blocksize) * (t_uint32) blocksize;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = (t_uint32) ((MMC_MAXDATALENGTH / blocksize) * blocksize);

    if
    (
        (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        /*SET MAX NO OF BLOCKS TO BE TRANSFERRED IN CASE OF MULTIMEDIA CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (MMC_MAXDATALENGTH / (t_uint32) blocksize);
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCK_COUNT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCK_COUNT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
    }
    else
    {
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;
    }

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer = p_writebuff;

    /*SEND CMD25 WRITE_MULT_BLOCK WITH ARGUMENT AS ADDRESS */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCWRITE_MULT_BLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCWRITE_MULT_BLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ENABLE DATAPATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
        MMCDATATIMEOUT |
        MMCTXFIFOHALFEMPTY |
        MMCTXUNDERRUN |
        MMCDATAEND |
        MMCSTARTBITERROR;

    return(error);
}

PRIVATE t_mmc_error mmc_WriteBlock_poll_mode_err_chk(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXUNDERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCTXUNDERRUN;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_TX_UNDERRUN;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_START_BIT_ERR;
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_WriteBlock_poll_mode
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    count, rest_words;
    t_uint8     cardstate;
    t_uint32    *p_tempbuff = p_writebuff;
    t_uint32    bytes_transferred = 0;

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCTXUNDERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATAEND | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
        {
            if
            (
                MMC_HALFFIFOBYTES >
                    (
                        g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                        bytes_transferred
                    )
            )
            {

                rest_words =
                    (
                        (
                            g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                            bytes_transferred
                        ) %
                        4 == 0
                    ) ?
                        (
                            (
                                g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                bytes_transferred
                            ) / 4
                        ) :
                            (
                                (
                                    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                    bytes_transferred
                                ) / 4 + 1
                            );

                for (count = 0; count < rest_words; count++, p_tempbuff++, bytes_transferred += 4)
                {
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *p_tempbuff;
                }
            }
            else
            {
                for (count = 0; count < MMC_HALFFIFO; count++)
                {
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *(p_tempbuff + count);
                }

                p_tempbuff += MMC_HALFFIFO;
                bytes_transferred += MMC_HALFFIFOBYTES;
            }
        }
    }

    error = mmc_WriteBlock_poll_mode_err_chk(mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATAEND)
    {
        if
        (
            (
                (no_of_blocks > 1)
            &&  (
                    (MMC_SECURE_DIGITAL_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
                ||  (MMC_HIGH_CAPACITY_SD_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
                )
            )
        )
        {
            /*SEND CMD12 STOP_TRANSMISSION */
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSTOP_TRANSMISSION |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
            error = mmc_CmdResp1Error(MMCSTOP_TRANSMISSION, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        }
    }

    /*clear all the static status flags */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error;

    /*WAIT TILL THE CARD IS IN PROGRAMMING STATE */
    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_WriteSingleBlock
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    timeout = 0;
    t_uint32    card_status;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = blocksize;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = blocksize;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer = p_writebuff;

    /*Wait till card is ready for data Added*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
            (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
        MMC_SHIFT16;        /* rca */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEND_STATUS |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;

    error = mmc_CmdResp1Error(MMCSEND_STATUS, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    card_status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;
    timeout = MMC_DATATIMEOUT;
    while ((0 == (card_status & 0x00000100)) && (timeout > 0))
    {
        timeout--;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;    /* rca */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEND_STATUS |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

        error = mmc_CmdResp1Error(MMCSEND_STATUS, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        card_status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;
    }

    if (0 == timeout)
    {
        return(MMC_ERROR);
    }

    /*Till here*/
    /*SEND CMD24 WRITE_SINGLE_BLOCK */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCWRITE_SINGLE_BLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCWRITE_SINGLE_BLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* ENABLE DATA PATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);

    /*IN CASE OF SINGLE DATA BLOCK TRANSFER NO NEED OF STOP COMMAND AT ALL */
    g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;

    if (MMC_POLLING_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        error = mmc_WriteBlock_poll_mode(cardno, addr, p_writebuff, blocksize, no_of_blocks, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if (MMC_INTERRUPT_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXFIFOHALFEMPTY |
            MMCTXUNDERRUN |
            MMCSTARTBITERROR;
    }
    else if (MMC_DMA_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        /*  for(timeout =0; timeout < 5000; timeout++); */ //in permanent label this will be removed
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXUNDERRUN |
            MMCSTARTBITERROR;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDMAENABLE;
    }

    return(error);
}

PRIVATE t_mmc_error mmc_WriteMultiBlocks
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    //t_uint32  timeout;
    /*COMMON TO ALL MODES */
    if (MMC_MAXDATALENGTH < (no_of_blocks * blocksize))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = (no_of_blocks * blocksize);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = (t_uint32) (no_of_blocks * blocksize);

    if
    (
        (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        /*SEND CMD23 SET_BLOCK_COUNT, argument = no_of_blocks */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = ((t_uint32) no_of_blocks);
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCK_COUNT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCK_COUNT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
    }
    else
    {
        /*To improve performance*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = g_mmc_system_context[mmc_device_id].
                card_array[cardno - 1].rca <<
            16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

        error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) no_of_blocks;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCK_COUNT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCK_COUNT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;
    }
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer = p_writebuff;

    /*SEND CMD25 WRITE_MULT_BLOCK with argument data address*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCWRITE_MULT_BLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCWRITE_MULT_BLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ENABLE DATAPATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);

    if (MMC_POLLING_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {

        error = mmc_WriteBlock_poll_mode(cardno, addr, p_writebuff, blocksize, no_of_blocks, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if (MMC_INTERRUPT_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXFIFOHALFEMPTY |
            MMCTXUNDERRUN |
            MMCSTARTBITERROR;
    }
    else if (MMC_DMA_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        /* for(timeout =0; timeout < 5000; timeout++); */ // in parmanent label this will be removed
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXUNDERRUN |
            MMCSTARTBITERROR;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDMAENABLE;
    }
    return(error);
}


PRIVATE t_mmc_error mmc_WriteBlock_setting
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    #if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
    t_uint8    power;
    #else
    t_uint32    power;
    #endif

    if (NULL == p_writebuff)
    {
        DBGEXIT0(MMC_INVALID_PARAMETER);
        return(MMC_INVALID_PARAMETER);
    }

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = 0;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMC_ALLZERO;

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        DBGEXIT0(MMC_INVALID_PARAMETER);
        return(MMC_INVALID_PARAMETER);
    }

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        DBGEXIT0(MMC_LOCK_UNLOCK_FAILED);
        return(MMC_LOCK_UNLOCK_FAILED);
    }

    /* set the block size,both on controller and card */

#if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
    if ((blocksize > 0) && (blocksize <= MMC_MAXBLSIZE) && (0 == (blocksize & (blocksize - 1))))
    {
        power = mmc_convert_from_bytes_to_power_of_two(blocksize);

        /*SET BLOCK SIZE FOR CONTROLLER.*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) power << MMC_SHIFT4;

#else
        if ((blocksize > 0) && (0 == (blocksize & (blocksize - 1))))
        {
         power = blocksize;   
        /*SET BLOCK SIZE FOR CONTROLLER.*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) power << MMC_SHIFT16;
#endif

      

        /*SET BLOCK SIZE FOR CARD IF IT IS NOT EQUAL TO CURRENT BLOCK SIZE FOR CARD. */
        if (power != g_mmc_system_context[mmc_device_id].current_card.blk_length)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) blocksize;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
            error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

            g_mmc_system_context[mmc_device_id].current_card.blk_length = power;
        }
    }
    else
    {
        DBGEXIT0(MMC_INVALID_PARAMETER);
        return(MMC_INVALID_PARAMETER);
    }

    return(error);
}





/****************************************************************************/
/*       NAME : MMC_ReliableWriteMultiBlocks                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to write blocks starting from a         */
/*              specified address in a card                                 */
/* PARAMETERS :                                                             */
/*         IN : t_uint8 cardno: card to access                              */
/*              t_uint32 addr : address from where to start writing         */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*              t_uint16 blocksize : size of block in bytes                 */
/*              t_uint16 no_of_blocks: no. of blocks to write               */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/


PUBLIC t_mmc_error MMC_ReliableWriteMultiBlocks
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    /*COMMON TO ALL MODES */
    if (MMC_MAXDATALENGTH < (no_of_blocks * blocksize))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    error = mmc_WriteBlock_setting(cardno, addr, p_writebuff, blocksize, no_of_blocks, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }
    
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = (no_of_blocks * blocksize);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = (t_uint32) (no_of_blocks * blocksize);

    if
    (
        (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        /*SEND CMD23 SET_BLOCK_COUNT, argument = no_of_blocks */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (EN_REL_WR | (t_uint32) no_of_blocks);
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCK_COUNT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCK_COUNT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
    }
    else
    {
        /*  SD card does not support SECURE WRITE FEATURE */
        DBGEXIT0(MMC_UNSUPPORTED_HW);
        return(MMC_UNSUPPORTED_HW);

    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer = p_writebuff;

    /*SEND CMD25 WRITE_MULT_BLOCK with argument data address*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCWRITE_MULT_BLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCWRITE_MULT_BLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ENABLE DATAPATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);

    if (MMC_POLLING_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        error = mmc_WriteBlock_poll_mode(cardno, addr, p_writebuff, blocksize, no_of_blocks, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if (MMC_INTERRUPT_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXFIFOHALFEMPTY |
            MMCTXUNDERRUN |
            MMCSTARTBITERROR;
    }
    else if (MMC_DMA_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXUNDERRUN |
            MMCSTARTBITERROR;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDMAENABLE;
    }

    return(error);
}


/****************************************************************************/
/*       NAME : MMC_WriteBlocks                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to write blocks starting from a         */
/*              specified address in a card                                 */
/* PARAMETERS :                                                             */
/*         IN : t_uint8 cardno: card to access                              */
/*              t_uint32 addr : address from where to start writing         */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*              t_uint16 blocksize : size of block in bytes                 */
/*              t_uint16 no_of_blocks: no. of blocks to write               */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*       IN : t_bool force_multiblock_write: Force multiblock write command */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/


PUBLIC t_mmc_error MMC_WriteBlocks
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id,
    IN t_bool           force_multiblock_write
)
{
    t_mmc_error error = MMC_OK;

    DBGENTER4("cardno(%d) addr(%d)blocksize(%d)no_of_blocks(%d)", cardno, addr, blocksize, no_of_blocks);

    error = mmc_WriteBlock_setting(cardno, addr, p_writebuff, blocksize, no_of_blocks, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* no_of_blocks = 0, OPEN_ENDED TRANSMISSION */
    /*FOR OPEN_ENDED TRANSFER.SUPPORTED ONLY IN INTERRUPT MODE */
    if (0 == no_of_blocks)
    {
        if (MMC_INTERRUPT_MODE != g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
        {
            error = MMC_REQUEST_NOT_APPLICABLE;
            DBGEXIT0(error);
            return(error);
        }

        error = mmc_WriteBlocks_openend(cardno, addr, p_writebuff, blocksize, no_of_blocks, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if ((1 == no_of_blocks) && (!force_multiblock_write))
    {
        error = mmc_WriteSingleBlock(cardno, addr, p_writebuff, blocksize, no_of_blocks, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    /* IF NO OF BLOCKS ARE MORE THAN 1, BUT DEFINED */
    else if ((1 < no_of_blocks) || force_multiblock_write)
    {
        error = mmc_WriteMultiBlocks(cardno, addr, p_writebuff, blocksize, no_of_blocks, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_GetTransferState()                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the current data transfer state           */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                                                          */
/*     RETURN : t_mmc_transfer_state                                        */

/****************************************************************************/
PUBLIC t_mmc_transfer_state MMC_GetTransferState(IN t_mmc_device_id mmc_device_id)
{
    DBGENTER0();
    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & (MMCTXACTIVE | MMCRXACTIVE))
    {
        DBGEXIT0(MMC_TRANSFER_IN_PROGRESS);
        return(MMC_TRANSFER_IN_PROGRESS);
    }
    else
    {
        DBGEXIT0(MMC_NO_TRANSFER);
        return(MMC_NO_TRANSFER);
    }
}

/****************************************************************************/
/*       NAME : MMC_LastTransferInfo()                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the last data transfer information        */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                                                          */
/*     RETURN : t_mmc_last_transfer_info                                    */

/****************************************************************************/
PUBLIC t_mmc_last_transfer_info MMC_LastTransferInfo(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_last_transfer_info    info = { 0 };
    DBGENTER0();
    info.transfer_error = g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error;
    info.transferred_bytes = (t_uint16)
        (
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_count
        );
    DBGEXIT0(MMC_OK);
    return(info);
}

/****************************************************************************/
/*       NAME : MMC_StopTransfer()                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine aborts an ongoing data transfer                */
/* PARAMETERS :                                                             */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_StopTransfer(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

#if defined(SDIO_SUPPORT)
    t_uint8     io_direct_data;
    t_uint32    argument;
#endif
    DBGENTER0();

#if defined(SDIO_SUPPORT)
    if
    (
        MMC_SECURE_DIGITAL_IO_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype
    ||  MMC_SECURE_DIGITAL_IO_COMBO_CARD == g_mmc_system_context[mmc_device_id].current_card.cardtype
    )
    {
        /* Abort by writing to I/O Abort register */
        argument = (t_uint32) g_mmc_system_context[mmc_device_id].current_card.sdio_function |  /* Active Function */
        (t_uint32) 0x06 << MMC_SHIFT9 | /* Address of I/O Abort register CCCR */
        (t_uint32) 0x0 << MMC_SHIFT28 | /* Select function 0, CIA */
        (t_uint32) 0x0 << MMC_SHIFT27 | /* No read after write */
        (t_uint32) MMC_SDIO_WRITE << MMC_SHIFT31;

        /* SEND IO DIRECT COMMAND */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else
    {
#endif

        /*SEND CMD12 STOP_TRANSMISSION  */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x00000000;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSTOP_TRANSMISSION |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

        error = mmc_CmdResp1Error(MMCSTOP_TRANSMISSION, mmc_device_id);

#if defined(SDIO_SUPPORT)
    }
#endif
    DBGEXIT0(error);
    return(error);
}

/***********************************************************************************************/
/*       NAME : MMC_SendSdioOpCond                                                             */
/*---------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine implements cmd5 of SDIO card and retrieves its response.          */
/*                                                                                             */
/* PARAMETERS :                                                                                */
/*         IN : NONE                                                                           */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                    */
/*        OUT : t_mmc_sdio_resp4 : response of the cmd 5 of SDIO                               */
/*                                                                                             */
/*     RETURN : t_mmc_error MMC_OK: No error                                                   */

/***********************************************************************************************/
PUBLIC t_mmc_error MMC_SendSdioOpCond(OUT t_mmc_sdio_resp4 *response, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error         error = MMC_OK;
    t_mmc_sdio_resp4    temp;

    DBGENTER0();

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_SEND_OP_COND |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;

    error = mmc_CmdResp4Error(MMCIO_SEND_OP_COND, &temp, mmc_device_id);

    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    *response = temp;

    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/*************************************************************************************************/
/*       NAME : MMC_SDIOReadWrite()                                                              */
/*--------------------------------------------------------------------------                     */
/* DESCRIPTION: This routine reads/writes data from/to SDIO card                                 */
/* PARAMETERS :IN cardno: Card to access.                                                        */
/*             IN transfer_param:                                                                */
/*              transfer_param. card_addr: address from where to start reading.                  */
/*              transfer_param. no_of_bytes: number of bytes of data to be                       */
/*                              transferred.                                                     */
/*              transfer_param. no_of_blocks: number of blocks of data to be                     */
/*                              transferred, only relevant if block_mode is enabled.             */
/*              transfer_param. block_size: size of a single block to be transferred,            */
/*                              only relevant if block_mode is enabled.                          */
/*              transfer_param. function_number: numeric value of requested function.            */
/*              transfer_param. read_after_write: If enabled the byte written is                 */
/*                              immediately read back, only relevant in command transfer mode,   */
/*                              ie no_of_bytes is one.                                           */
/*              transfer_param. block_mode: Enable/Disable block mode.                           */
/*              transfer_param. incremental_transfer: If Disable source/destinalion card         */
/*                              address is not incremented, it is read repeatedly read,          */
/*                              useful when reading a FIFO register in card.                     */
/*              transfer_param. read_write: Direction of Transfer read or                        */
/*                              writes to card                                                   */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                      */
/*                                                                                               */
/*              INOUT p_readwritebuff: buffer to store information                               */
/*                                                                                               */
/*     RETURN : t_mmc_error                                                                      */

/*************************************************************************************************/
#if defined(SDIO_SUPPORT)
/* additional APIs for code complexity reduction  */
PRIVATE t_mmc_error mmc_SDIOReadWriteData_errorCHK
(
    IN t_uint8                          cardno,
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    argument;
    t_uint8     io_direct_data;

    if
    (
        (MMC_SECURE_DIGITAL_IO_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    &&  (MMC_SECURE_DIGITAL_IO_COMBO_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    if (NULL == p_readwritebuff)
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    if (MMCSDIO_ADDR_RANGE < p_transfer_param->card_addr)
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /* BLOCK MODE SUPPORT CHECK */
    if (MMC_ENABLE == p_transfer_param->block_mode)
    {
        if (NULL == (g_mmc_system_context[mmc_device_id].card_array[cardno - 1].sdio_cccr[3] & MASK_BIT1))
        {
            error = MMC_UNSUPPORTED_FEATURE;
            DBGEXIT0(error);
            return(error);
        }
    }

    /* CHECK FOR IO FUNCTION READY */
    if
    (
        (
            NULL ==
                (
                    g_mmc_system_context[mmc_device_id].card_array[cardno - 1].sdio_cccr[0] &
                        (MASK_BIT0 << p_transfer_param->function_number)
                )
        )
    &&  (p_transfer_param->function_number > 0)
    )
    {
        error = MMC_SDIO_DISABLED;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD. */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    /* CHECK IF FUNCTION BUSY, Exec FLAG of CCCR */
    argument = ((t_uint32) 0x0E << MMC_SHIFT9) |            /* Address of Exec flag register of CCCR */
    ((t_uint32) 0x0 << MMC_SHIFT28) |                       /* Select function 0, CIA */
    ((t_uint32) 0x0 << MMC_SHIFT27) |                       /* No read after write */
    ((t_uint32) MMC_SDIO_READ << MMC_SHIFT31);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    if
    (
        (NULL == (io_direct_data & (MASK_BIT0 << p_transfer_param->function_number)))
    &&  (p_transfer_param->function_number > 0)
    )
    {
        error = MMC_SDIO_FUNCTION_BUSY;
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

PRIVATE void mmc_SDIOReadWriteData_blkpollMODE_Read(INOUT t_uint32 *p_readwritebuff, IN t_mmc_device_id mmc_device_id)
{
    /* SDIO SUPPORT */
    t_uint32    count;

    t_uint32    *p_tempbuff = p_readwritebuff;

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCRXOVERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATAEND | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXFIFOHALFFULL)
        {
            for (count = 0; count < MMC_HALFFIFO; count++)
            {
                *(p_tempbuff + count) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
            }

            p_tempbuff += MMC_HALFFIFO;
        }
    }
    return;
}

PRIVATE void mmc_SDIOReadWriteData_blkpollMODE_Write(INOUT t_uint32 *p_readwritebuff, IN t_mmc_device_id mmc_device_id)
{
    /* SDIO SUPPORT */
    t_uint32    count, rest_words, bytes_transferred = 0;

    t_uint32    *p_tempbuff = p_readwritebuff;

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCTXUNDERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATAEND | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
        {
            if
            (
                MMC_HALFFIFOBYTES >
                    (
                        g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                        bytes_transferred
                    )
            )
            {
                rest_words =
                    (
                        (
                            g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                            bytes_transferred
                        ) %
                        4 == 0
                    ) ?
                        (
                            (
                                g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                bytes_transferred
                            ) / 4
                        ) :
                            (
                                (
                                    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                    bytes_transferred
                                ) / 4 + 1
                            );

                for (count = 0; count < rest_words; count++, p_tempbuff++, bytes_transferred += 4)
                {
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *p_tempbuff;
                }
            }
            else
            {
                for (count = 0; count < MMC_HALFFIFO; count++)
                {
                    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *(p_tempbuff + count);
                }

                p_tempbuff += MMC_HALFFIFO;
                bytes_transferred += MMC_HALFFIFOBYTES;
            }
        }
    }
    return;
}

PRIVATE t_mmc_error mmc_SDIOReadWriteData_blkpollMODE
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    argument;
    t_uint8     io_direct_data;

    t_uint32    *p_tempbuff = p_readwritebuff;

    /* READ */
    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        mmc_SDIOReadWriteData_blkpollMODE_Read(p_readwritebuff, mmc_device_id);
    }

    /* WRITE */
    else
    {
        mmc_SDIOReadWriteData_blkpollMODE_Write(p_readwritebuff, mmc_device_id);
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }
    else if
        (
            (NULL != (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXOVERRUN))
        &&  (MMC_SDIO_READ == p_transfer_param->read_write)
        )
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCRXOVERRUN;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_RX_OVERRUN;
        DBGEXIT0(error);
        return(error);
    }
    else if
        (
            (NULL != (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXUNDERRUN))
        &&  (MMC_SDIO_WRITE == p_transfer_param->read_write)
        )
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCTXUNDERRUN;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_TX_UNDERRUN;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_START_BIT_ERR;
        DBGEXIT0(error);
        return(error);
    }

    /* READ LAST TRANSFER DATA */
    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        while (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXDATAAVLBL)
        {
            *p_tempbuff = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
            p_tempbuff++;
        }
    }

    if (0 < p_transfer_param->function_number)
    {
        do
        {
            /* CHECK IF FUNCTION BUSY, Ready FLAG of CCCR */
            argument = ((t_uint32) 0x0F << MMC_SHIFT9) |    /* Address of Exec flag register of CCCR */
            ((t_uint32) 0x0 << MMC_SHIFT28) |               /* Select function 0, CIA */
            ((t_uint32) 0x0 << MMC_SHIFT27) |               /* No read after write */
            ((t_uint32) MMC_SDIO_READ << MMC_SHIFT31);

            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
            error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        } while (NULL == (io_direct_data & (MASK_BIT0 << p_transfer_param->function_number)));

        /* MASK_BIT1 */
    }

    /*clear all the static status flags */
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;
    return(error);
}

PRIVATE t_mmc_error mmc_SDIOReadWriteData_blkMODE_OpenXfer
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    argument;
    t_uint8     io_direct_data;
    t_uint16    blocksize = MMC_SDIO_BLOCK_SIZE;

    /* Open ended transfer is supported only in Interrupt Mode */
    if (MMC_INTERRUPT_MODE != g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        return(MMC_REQUEST_NOT_APPLICABLE);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = (MMC_MAXDATALENGTH / (t_uint32) blocksize) * (t_uint32) blocksize;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = (t_uint32) ((MMC_MAXDATALENGTH / blocksize) * blocksize);

    /* STOP BY WRITING TO ASx BITS IN CCR */
    g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = TRUE;

    /* FILL UP DATA CTRL REGISTER. */
    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCREADDIR &~MMCSTREAMMODE | MMCDATAPATHENABLE;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_readwritebuff;
    }
    else
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer = p_readwritebuff;
    }

    /* SEND IO EXTENDED COMMAND */
    argument = (t_uint32) p_transfer_param->no_of_blocks | ((t_uint32) p_transfer_param->card_addr << MMC_SHIFT9) |
        ((t_uint32) p_transfer_param->incremental_transfer << MMC_SHIFT26) |
            ((t_uint32) p_transfer_param->block_mode << MMC_SHIFT27) |
                ((t_uint32) p_transfer_param->function_number << MMC_SHIFT28) |
                    ((t_uint32) p_transfer_param->read_write << MMC_SHIFT31);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_EXTENDED |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error(MMCIO_RW_EXTENDED, &io_direct_data, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* READ */
    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCRXFIFOHALFFULL |
            MMCRXOVERRUN |
            MMCDATAEND |
            MMCSTARTBITERROR;
    }

    /* WRITE */
    else
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCTXFIFOHALFEMPTY |
            MMCTXUNDERRUN |
            MMCDATAEND |
            MMCSTARTBITERROR;
    }

    return(error);
}

PRIVATE void mmc_SDIOReadWriteData_blkINTMODE
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    IN t_mmc_device_id                  mmc_device_id
)
{
    /* READ */
    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCRXFIFOHALFFULL |
            MMCRXOVERRUN |
            MMCSTARTBITERROR;
    }

    /* WRITE */
    else
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXFIFOHALFEMPTY |
            MMCTXUNDERRUN |
            MMCSTARTBITERROR;
    }
    return;
}

PRIVATE void mmc_SDIOReadWriteData_blkDMAMODE
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    IN t_mmc_device_id                  mmc_device_id
)
{
    /* READ */
    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCRXOVERRUN |
            MMCSTARTBITERROR;
    }

    /* WRITE */
    else
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXUNDERRUN |
            MMCSTARTBITERROR;
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDMAENABLE;
    return;
}

PRIVATE t_mmc_error mmc_SDIOReadWriteData_blkMode
(
    IN t_uint8                          cardno,
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    argument;
    t_uint8     io_direct_data;
    t_uint16    blocksize = MMC_SDIO_BLOCK_SIZE;
    t_uint8     power = MMC_SDIO_BLOCK_POWER;

    if (MMC_ENABLE == p_transfer_param->block_mode)
    {
        blocksize = p_transfer_param->block_size;

#if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
    if ((blocksize > 0) && (blocksize <= MMC_MAXBLSIZE) && (0 == (blocksize & (blocksize - 1))))
    {
        power = mmc_convert_from_bytes_to_power_of_two(blocksize);

        /*SET BLOCK SIZE FOR CONTROLLER.*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) power << MMC_SHIFT4;

#else
        if ((blocksize > 0) && (0 == (blocksize & (blocksize - 1))))
        {
         power = blocksize;   
        /*SET BLOCK SIZE FOR CONTROLLER.*/
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) power << MMC_SHIFT16;
#endif


            /*SET BLOCK SIZE FOR CARD IF IT IS NOT EQUAL TO CURRENT BLOCK SIZE FOR CARD. */
            if (power != g_mmc_system_context[mmc_device_id].current_card.blk_length)
            {
                /*SET BLOCK SIZE FOR I/O FUNCTION*/
                argument = (t_uint32) p_transfer_param->block_size | ((((t_uint32) p_transfer_param->function_number << MMC_SHIFT8)) << MMC_SHIFT9) |    /* Address FBR I/O block size*/
                ((t_uint32) 0x0 << MMC_SHIFT28) |   /* Select function 0, CIA */
                ((t_uint32) 0x0 << MMC_SHIFT27) |   /* No read after write */
                ((t_uint32) MMC_SDIO_READ << MMC_SHIFT31);

                /* SEND IO DIRECT COMMAND */
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
                    MMCRESPEXPECTED |
                    MMCCMDPATHENABLE;
                error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
                if (MMC_OK != error)
                {
                    return(error);
                }

                g_mmc_system_context[mmc_device_id].current_card.blk_length = power;
            }
        }
        else
        {
            error = MMC_INVALID_PARAMETER;
            DBGEXIT0(error);
            return(error);
        }

        /* OPEN ENDED TRANSFER */
        if (0 == p_transfer_param->no_of_bytes)
        {
            error = mmc_SDIOReadWriteData_blkMODE_OpenXfer(p_transfer_param, p_readwritebuff, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        }

        /* FIXED LENGTH TRANSFER */
        else
        {
            /*COMMON TO ALL MODES */
            if (MMC_MAXDATALENGTH < p_transfer_param->no_of_blocks * blocksize)
            {
                error = MMC_INVALID_PARAMETER;
                DBGEXIT0(error);
                return(error);
            }

            g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = (t_uint32) (p_transfer_param->no_of_blocks * blocksize);

            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = (t_uint32) g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes;

            g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;

            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

            if (MMC_SDIO_READ == p_transfer_param->read_write)
            {
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCREADDIR &~MMCSTREAMMODE | MMCDATAPATHENABLE;
                g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_readwritebuff;
            }
            else
            {
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
                g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer = p_readwritebuff;
            }

            /* SEND IO EXTENDED COMMAND */
            argument = (t_uint32) p_transfer_param->no_of_blocks |
                ((t_uint32) p_transfer_param->card_addr << MMC_SHIFT9) |
                    ((t_uint32) p_transfer_param->incremental_transfer << MMC_SHIFT26) |
                        ((t_uint32) p_transfer_param->block_mode << MMC_SHIFT27) |
                            ((t_uint32) p_transfer_param->function_number << MMC_SHIFT28) |
                                ((t_uint32) p_transfer_param->read_write << MMC_SHIFT31);

            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_EXTENDED |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
            error = mmc_CmdResp5Error(MMCIO_RW_EXTENDED, &io_direct_data, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }

            if (MMC_POLLING_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
            {
                error = mmc_SDIOReadWriteData_blkpollMODE(p_transfer_param, p_readwritebuff, mmc_device_id);
                if (MMC_OK != error)
                {
                    DBGEXIT0(error);
                    return(error);
                }
            }
            else if (MMC_INTERRUPT_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
            {
                mmc_SDIOReadWriteData_blkINTMODE(p_transfer_param, mmc_device_id);
            }
            else if (MMC_DMA_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
            {
                mmc_SDIOReadWriteData_blkDMAMODE(p_transfer_param, mmc_device_id);
            }
        }
    }

    return(error);
}

PRIVATE t_mmc_error mmc_SDIOReadWriteData_bytePoll_Mode
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
)
{
    /* SDIO SUPPORT */
    t_mmc_error error = MMC_OK;
    t_uint32    argument;
    t_uint8     io_direct_data;
    t_uint32    count, rest_words, bytes_transferred = 0;

    t_uint32    *p_tempbuff = p_readwritebuff;

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATAEND)
        )
    )
    {
        /* READ */
        if (MMC_SDIO_READ == p_transfer_param->read_write)
        {
            if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXFIFOHALFFULL)
            {
                for (count = 0; count < MMC_HALFFIFO; count++)
                {
                    *(p_tempbuff + count) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
                }

                p_tempbuff += MMC_HALFFIFO;
            }
        }

        /* WRITE */
        else
        {
            if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
            {
                if
                (
                    MMC_HALFFIFOBYTES >
                        (
                            g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                            bytes_transferred
                        )
                )
                {
                    rest_words =
                        (
                            (
                                g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                bytes_transferred
                            ) %
                            MMC_WORDDIV == 0
                        ) ?
                            (
                                (
                                    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                    bytes_transferred
                                ) / MMC_WORDDIV
                            ) :
                                (
                                    (
                                        g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                                        bytes_transferred
                                    ) / MMC_WORDDIV + 1
                                );
                    for (count = 0; count < rest_words; count++, p_tempbuff++, bytes_transferred += MMC_BYTEINC)
                    {
                        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *p_tempbuff;
                    }
                }
                else
                {
                    for (count = 0; count < MMC_HALFFIFO; count++)
                    {
                        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *(p_tempbuff + count);
                    }

                    p_tempbuff += MMC_HALFFIFO;
                    bytes_transferred += MMC_HALFFIFOBYTES;
                }
            }
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        error = MMC_DATA_CRC_FAIL;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;  /* clear all the static status flags */

    /* READ LAST TRANSFER DATA */
    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        while (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXDATAAVLBL)
        {
            *p_tempbuff = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
            p_tempbuff++;
        }
    }

    /* WAIT FOR WRITE/READ TO COMPLETE */
    if (0 < p_transfer_param->function_number)
    {
        do
        {
            /* CHECK IF FUNCTION BUSY, Ready FLAG of CCCR */
            argument = ((t_uint32) 0x0F << MMC_SHIFT9) |                                /* Address of Exec flag register of CCCR */
            ((t_uint32) 0x0 << MMC_SHIFT28) |   /* Select function 0, CIA */
            ((t_uint32) 0x0 << MMC_SHIFT27) |   /* No read after write */
            ((t_uint32) MMC_SDIO_READ << MMC_SHIFT31);

            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
            error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        } while (NULL == (io_direct_data & (MASK_BIT0 << p_transfer_param->function_number)));
    }

    return(error);
}

PRIVATE void mmc_SDIOReadWriteData_byteINT_Mode
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    IN t_mmc_device_id                  mmc_device_id
)
{
    /* READ */
    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCRXFIFOHALFFULL |
            MMCRXOVERRUN;
    }

    /* WRITE */
    else
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXFIFOHALFEMPTY |
            MMCTXUNDERRUN;
    }
return;
}

PRIVATE void mmc_SDIOReadWriteData_byteDMA_Mode
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    IN t_mmc_device_id                  mmc_device_id
)
{
    /* READ */
    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCRXOVERRUN;
    }

    /* WRITE */
    else
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 = MMCDATACRCFAIL |
            MMCDATATIMEOUT |
            MMCDATAEND |
            MMCTXUNDERRUN;
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCDMAENABLE;
    return;
}

PRIVATE t_mmc_error mmc_SDIOReadWriteData_byteMode
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    argument;
    t_uint8     io_direct_data;
    t_uint32    tranfer_size;

    /* INITIALIZE HOST DATA PATH */
    if (0 == p_transfer_param->no_of_bytes)
    {
        /* For tranfer in bytes if tranfer size is set to zero , 512 bytes are tranferred */
        tranfer_size = 512;
    }
    else
    {
        tranfer_size = p_transfer_param->no_of_bytes;
    }

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = (t_uint32) tranfer_size;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = tranfer_size;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    if (MMC_SDIO_READ == p_transfer_param->read_write)
    {
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_readwritebuff;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMCSDIOENABLE |
            MMCREADDIR |
            MMCSTREAMMODE |
            MMCDATAPATHENABLE;
    }
    else
    {
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer = p_readwritebuff;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMCSDIOENABLE |
            MMCSTREAMMODE &~MMCREADDIR |
            MMCDATAPATHENABLE;
    }

    /* SEND IO EXTENDED COMMAND */
    argument = (t_uint32) p_transfer_param->no_of_bytes | ((t_uint32) p_transfer_param->card_addr << MMC_SHIFT9) |
        ((t_uint32) p_transfer_param->incremental_transfer << MMC_SHIFT26) |
            ((t_uint32) p_transfer_param->block_mode << MMC_SHIFT27) |
                ((t_uint32) p_transfer_param->function_number << MMC_SHIFT28) |
                    ((t_uint32) p_transfer_param->read_write << MMC_SHIFT31);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_EXTENDED |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;

    error = mmc_CmdResp5Error(MMCIO_RW_EXTENDED, &io_direct_data, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    if (MMC_POLLING_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
        error = mmc_SDIOReadWriteData_bytePoll_Mode(p_transfer_param, p_readwritebuff, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if (MMC_INTERRUPT_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
        mmc_SDIOReadWriteData_byteINT_Mode(p_transfer_param, mmc_device_id);
    }
    else if (MMC_DMA_MODE == g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
    {
        g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
        mmc_SDIOReadWriteData_byteDMA_Mode(p_transfer_param, mmc_device_id);
    }

    return(error);
}
#endif

PUBLIC t_mmc_error MMC_SDIOReadWriteData
(
    IN t_uint8                          cardno,
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
)
{
    /* SDIO SUPPORT */
    t_mmc_error error = MMC_OK;
    t_uint32    argument;
    t_uint8     io_direct_data;

    /* ERROR CONDINTIONS CHECK */
    error = mmc_SDIOReadWriteData_errorCHK(cardno, p_transfer_param, p_readwritebuff, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* UPDATE GLOBAL CARD CONTEXT */
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = 0;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMC_ALLZERO;

    g_mmc_system_context[mmc_device_id].current_card.sdio_function = p_transfer_param->function_number;

    /* SDIO_RW_DIRECT */
    if (1 == p_transfer_param->no_of_bytes)
    {
        argument = ((t_uint32) p_transfer_param->card_addr << MMC_SHIFT9) |
            ((t_uint32) p_transfer_param->read_after_write << MMC_SHIFT27) |
                ((t_uint32) p_transfer_param->function_number << MMC_SHIFT28) |
                    ((t_uint32) p_transfer_param->read_write << MMC_SHIFT31);

        if (MMC_SDIO_WRITE == p_transfer_param->read_write)
        {
            argument |= *p_readwritebuff & MASK_BYTE;
        }

        /* SEND IO DIRECT COMMAND */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        if ((MMC_SDIO_READ == p_transfer_param->read_write) || (MMC_ENABLE == p_transfer_param->read_after_write))
        {
            *p_readwritebuff = (t_uint32) io_direct_data;
        }
    }

    /* SDIO_RW_EXTENDED */
    else
    {
        if (MMC_ENABLE == p_transfer_param->block_mode)
        {
            /* BLOCK MODE */
            error = mmc_SDIOReadWriteData_blkMode(cardno, p_transfer_param, p_readwritebuff, mmc_device_id);
            if (MMC_OK != error)
            {
                DBGEXIT0(error);
                return(error);
            }
        }

        /* BYTE TRANSFER */
        error = mmc_SDIOReadWriteData_byteMode(p_transfer_param, p_readwritebuff, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_SDIOSuspendFunction()                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine suspends the currently active SDIO function    */
/* PARAMETERS : IN t_uint8 cardno: card to access                           */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SDIOSuspendFunction(IN t_uint8 cardno, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    argument, timeout = 0xFFFF;
    t_uint8     io_direct_data;

    /* ERROR CONDINTIONS CHECK */
    if
    (
        (MMC_SECURE_DIGITAL_IO_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    &&  (MMC_SECURE_DIGITAL_IO_COMBO_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* CHECK FOR SUSPEND/RESUME SUPPORT IN CARD */
    if (NULL == (g_mmc_system_context[mmc_device_id].card_array[cardno - 1].sdio_cccr[3] & MASK_BIT3))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD. */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    /* Request Bus Release */
    argument = (t_uint32) MASK_BIT1 |                   /* Set BR */
    ((t_uint32) 0x0C << MMC_SHIFT9) |                   /* Address of I/O Enable register CCCR */
    ((t_uint32) 0x0 << MMC_SHIFT27) |                   /* Select function 0, CIA */
    ((t_uint32) 0x1 << MMC_SHIFT28) |                   /* Read after write */
    ((t_uint32) MMC_SDIO_WRITE << MMC_SHIFT31);

    /* SEND IO DIRECT COMMAND */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSDIOSUSPEND |
        MMCIO_RW_DIRECT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    while ((io_direct_data & (MASK_BIT0 | MASK_BIT1)) || (timeout-- == NULL))
    {
        /* IO ENABLE for all functions */
        argument = ((t_uint32) 0x0C << MMC_SHIFT9) |    /* Address of Bus suspend register CCCR */
        ((t_uint32) 0x0 << MMC_SHIFT27) |               /* Select function 0, CIA */
        ((t_uint32) 0x0 << MMC_SHIFT28) |               /* No read after write */
        ((t_uint32) MMC_SDIO_READ << MMC_SHIFT31);

        /* SEND IO DIRECT COMMAND */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSDIOSUSPEND |
            MMCIO_RW_DIRECT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    if (0 == timeout)
    {
        DBGEXIT0(MMC_CMD_RSP_TIMEOUT);
        return(MMC_CMD_RSP_TIMEOUT);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command &= ~MMCSDIOSUSPEND;
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}

/****************************************************************************/
/*       NAME : MMC_SDIOResumeFunction()                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine resumes the specific suspended SDIO function   */
/* PARAMETERS : IN t_uint8 cardno: card to access                           */
/*              IN t_uint8 function_number: Function to be resumed.         */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*              OUT t_uint8 *data_flag: if set then indicates more data     */
/*                                      will be transferred.                */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SDIOResumeFunction
(
    IN t_uint8          cardno,
    IN t_uint8          function_number,
    OUT t_uint8         *data_flag,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    argument;
    t_uint8     io_direct_data;

    /* ERROR CONDINTIONS CHECK */
    if
    (
        (MMC_SECURE_DIGITAL_IO_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    &&  (MMC_SECURE_DIGITAL_IO_COMBO_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* CHECK FOR SUSPEND/RESUME SUPPORT IN CARD */
    if (NULL == (g_mmc_system_context[mmc_device_id].card_array[cardno - 1].sdio_cccr[3] & MASK_BIT3))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD. */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    /* Select given function */
    argument = (t_uint32) function_number | /* Set BR */
    ((t_uint32) 0x0D << MMC_SHIFT9) |       /* Address of function select register CCCR */
    ((t_uint32) 0x0 << MMC_SHIFT27) |       /* Select function 0, CIA */
    ((t_uint32) 0x1 << MMC_SHIFT28) |       /* Read after write */
    ((t_uint32) MMC_SDIO_WRITE << MMC_SHIFT31);

    /* SEND IO DIRECT COMMAND */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* Return DATA FLAG */
    *data_flag = (t_uint8) ((io_direct_data & MASK_BIT7) >> MMC_SHIFT7);
    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}



/****************************************************************************/
/*       NAME : MMC_Erase                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to erase memory area specified  for the */
/*              given card.                                                 */
/* PARAMETERS :                                                             */
/*         IN : t_uint8     Cardno. to access                               */
/*              t_uint32    start address for erase.                        */
/*              t_uint32    Last address for erase.                         */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PRIVATE t_mmc_error mmc_Erase_set(IN t_uint32 startaddr, IN t_uint32 endaddr, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    /*SEND CMD35 ERASE_GRP_START with argument as addr  */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = startaddr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCERASE_GRP_START |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCERASE_GRP_START, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*SEND CMD36 ERASE_GRP_END with argument as addr  */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = endaddr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCERASE_GRP_END |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCERASE_GRP_END, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_sdErase_set(IN t_uint32 startaddr, IN t_uint32 endaddr, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    /*SEND CMD32 SD_ERASE_GRP_START with argument as addr  */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = startaddr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSD_ERASE_GRP_START |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSD_ERASE_GRP_START, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*SEND CMD33 SD_ERASE_GRP_END with argument as addr  */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = endaddr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSD_ERASE_GRP_END |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSD_ERASE_GRP_END, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_erase_errorChk(IN t_uint8 cardno, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /* CHECK IF THE CARD COMND CLASS SUPPORTS ERASE COMMAND */
    if (0 == ((g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2] >> 20) & MMC_CCCC_ERASE))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}


PUBLIC t_mmc_error MMC_Erase
(
    IN t_uint8          cardno,
    IN t_uint32         startaddr,
    IN t_uint32         endaddr,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint8     cardstate;

    DBGENTER3("cardno(%d)startaddr(%d)endaddr(%d)", cardno, startaddr, endaddr);

    error = mmc_erase_errorChk(cardno, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ACCORDING TO MultiMedia Card Spec 4.41 ERASE_GROUP_START (CMD35) AND ERASE_GROUP_END(CMD36)*/
    if
    (
        (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = mmc_Erase_set(startaddr, endaddr, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    /*ACCORDING TO SD-CARD SPEC 2.0 ERASE_GROUP_START (CMD32) AND ERASE_GROUP_END(CMD33)*/
    else if
        (
            (MMC_SECURE_DIGITAL_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        ||  (MMC_HIGH_CAPACITY_SD_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
        )
    {
        error = mmc_sdErase_set(startaddr, endaddr, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    /* SEND CMD38 ERASE */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCERASE | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCERASE, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* WAIT TILL THE CARD IS IN PROGRAMMING STATE */
    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    DBGEXIT0(error);
    return(error);
}




PUBLIC t_mmc_error MMC_Trim
(
    IN t_uint8          cardno,
    IN t_uint32         startaddr,
    IN t_uint32         endaddr,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint8     cardstate;

    DBGENTER3("cardno(%d)startaddr(%d)endaddr(%d)", cardno, startaddr, endaddr);

    error = mmc_erase_errorChk(cardno, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ACCORDING TO MultiMedia Card Spec 4.41 ERASE_GROUP_START (CMD35) AND ERASE_GROUP_END(CMD36)*/
    if
    (
        (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = mmc_Erase_set(startaddr, endaddr, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }


    /* SEND CMD38 ERASE */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = EN_TRIM;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCERASE | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCERASE, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* WAIT TILL THE CARD IS IN PROGRAMMING STATE */
    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_SetWriteProtect                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to protect data against erase or write. */
/* PARAMETERS :                                                             */
/*         IN : t_uint8     Cardno. to access                               */
/*              t_mmc_write_protect_type    protection type.                */
/*              t_uint32    Address of write protect group.                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SetWriteProtect
(
    IN t_uint8                  cardno,
    IN t_mmc_write_protect_type type,
    IN t_uint32                 addr,
    IN t_mmc_device_id          mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint8     cardstate;

    DBGENTER3("cardno(%d),type(%d)addr(%d)", cardno, type, addr);
    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD COOMND CLASS SUPPORTS WRITE PROTECTION */
    if (0 == ((g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2] >> 20) & MMC_CCCC_WRITE_PROT))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    if (MMC_WRITE_PROT_WHOLE_CARD_PERM == type)
    {
        error = mmc_ProgramCSD(cardno, MMC_PERMWP, TRUE, mmc_device_id);                        /* Set bit13 of csd register high */
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else if (MMC_WRITE_PROT_WHOLE_CARD_TEMP == type)
    {
#if defined(__MMC_SD_SOFTWARE_WORKAROUND)
        error = mmc_TempProtect(cardno, TRUE, mmc_device_id);                                   // Set bit12 of csd register high
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
#endif
    }
    else
    {
        /*CHECK IF THE SUPPORTS GROUP WRITE PROTECTION */
        if (!(g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[0] & BIT31))
        {
            error = MMC_REQUEST_NOT_APPLICABLE;
            DBGEXIT0(error);
            return(error);
        }

        /*SEND CMD28 SET_WRITE_PROT WITH ARGUMENT AS addr */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_WRITE_PROT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_WRITE_PROT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        /*WAIT TILL THE CARD IS IN PROGRAMMING STATE */
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
        while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
        {
            error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
        }
    }

    DBGEXIT0(error);
    return(error);
}

#if defined(__MMC_SD_SOFTWARE_WORKAROUND)
PRIVATE t_mmc_error mmc_TempProtect_SetReset
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         cmd,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    wp_grp_size = 0, sector_size = 0, blk_len_factor = 0, num_wp_grp = 0;
    t_uint32    erase_group_mult = 0, erase_group_size = 0;
    t_uint32    i = 0;
    t_uint8     cardstate;

    blk_len_factor = g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.blk_len_factor;
    erase_group_mult = g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.erase_group_mult;
    erase_group_size = g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.erase_group_size;
    num_wp_grp = g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.num_wp_grp;
    sector_size = g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.sector_size;
    wp_grp_size = g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.wp_grp_size;

    for (i = 0; i < num_wp_grp; i++)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = cmd | MMCRESPEXPECTED | MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_WRITE_PROT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        /*WAIT TILL THE CARD IS IN PROGRAMMING STATE */
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
        while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
        {
            error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
        }

        if
        (
            (MMC_SECURE_DIGITAL_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type)
        ||  (
                MMC_SECURE_DIGITAL_IO_COMBO_CARD == g_mmc_system_context[mmc_device_id].card_array[
                    (cardno - 1)].card_type
            )
        )
        {
            addr = addr + ((wp_grp_size + 1) * (sector_size + 1) * (0x1 << blk_len_factor));    /* Address of 2nd WP group */
        }

        if
        (
            (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type)
        ||  (
                MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[
                    (cardno - 1)].card_type
            )
        )
        {
            addr = addr + ((wp_grp_size + 1) * (erase_group_size + 1) * (erase_group_mult + 1) * (0x1 << blk_len_factor));          /* Address of 2nd WP group */
        }

        if (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type)
        {
            addr = addr + ((wp_grp_size + 1) * (erase_group_size + 1) * (erase_group_mult + 1) * (0x1 << (blk_len_factor - 9)));    /* Address of 2nd WP group */
        }
    }

    return(error);
}

PRIVATE t_mmc_error mmc_TempProtect(IN t_uint8 cardno, IN t_bool status, IN t_mmc_device_id mmc_device_id)
{
    t_uint32    c_size, c_size_mult, num_blk, wp_grp_size = 0, sector_size = 0, blk_len_factor = 0, num_wp_grp = 0;
    t_uint32    erase_group_mult = 0, erase_group_size = 0, addr = 0;
    t_mmc_error error = MMC_OK;

    if
    (
        (MMC_SECURE_DIGITAL_IO_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type)
    ||  (MMC_HIGH_CAPACITY_SD_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type)
    )
    {
        DBGEXIT0(MMC_UNSUPPORTED_FEATURE);
        return(MMC_UNSUPPORTED_FEATURE);
    }

    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    c_size =
        (
            ((g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[1] & 0xc0000000) >> MMC_SHIFT30) |
            ((g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[2] & 0xc00003ff) << MMC_SHIFT2)
        );
    c_size_mult = ((g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[1] & 0x00038000) >> MMC_SHIFT15);

    num_blk = 1;    /* 1 has been assigned to get power of 2 in next line. */
    num_blk = ((num_blk << (c_size_mult + 2)) * (c_size + 1));  /*according spec 1 & 2 ad   */

    if
    (
        MMC_SECURE_DIGITAL_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type
    ||  MMC_SECURE_DIGITAL_IO_COMBO_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type
    )
    {
        wp_grp_size = (g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[1] & 0x0000007F);
        sector_size = ((g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[1] & 0x00003f80) >> MMC_SHIFT7);
        blk_len_factor =
            (
                (g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[0] & 0x03c00000) >>
                MMC_SHIFT22
            );

        num_wp_grp = num_blk / ((wp_grp_size + 1) * (sector_size + 1));
    }

    if
    (
        (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type)
    ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type)
    )
    {
        wp_grp_size = (g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[1] & 0x0000001F);
        erase_group_mult =
            (
                (g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[1] & 0x000003E0) >>
                MMC_SHIFT5
            );
        erase_group_size =
            (
                (g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[1] & 0x00007C00) >>
                MMC_SHIFT10
            );
        blk_len_factor =
            (
                (g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[0] & 0x03c00000) >>
                MMC_SHIFT22
            );

        num_wp_grp = num_blk / ((wp_grp_size + 1) * (erase_group_size + 1) * (erase_group_mult + 1));
    }

    /*CHECK IF THE CARD SUPPORTS GROUP WRITE PROTECTION */
    if (!(g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[0] & BIT31))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.blk_len_factor = blk_len_factor;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.erase_group_mult = erase_group_mult;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.erase_group_size = erase_group_size;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.num_wp_grp = num_wp_grp;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.sector_size = sector_size;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.local_temp_protect.wp_grp_size = wp_grp_size;

    if (TRUE == status)
    {
        error = mmc_TempProtect_SetReset(cardno, addr, MMCSET_WRITE_PROT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }
    else
    {
        error = mmc_TempProtect_SetReset(cardno, addr, MMCCLR_WRITE_PROT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_ClearFullWriteProtect                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to clear write protection for a         */
/*              write protect group specified.                              */
/* PARAMETERS :                                                             */
/*         IN : t_uint8     Cardno. to access                               */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_ClearFullWriteProtect(IN t_uint8 cardno, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    DBGENTER1("cardno(%d)", cardno);

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD COOMND CLASS SUPPORTS WRITE PROTECTION */
    if (0 == ((g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2] >> 20) & MMC_CCCC_WRITE_PROT))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    error = mmc_TempProtect(cardno, FALSE, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    DBGEXIT0(MMC_OK);
    return(MMC_OK);
}
#endif /* __MMC_SD_SOFTWARE_WORKAROUND */

/****************************************************************************/
/*       NAME : MMC_ClearWriteProtect                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to clear write protection for a         */
/*              write protect group specified.                              */
/* PARAMETERS :                                                             */
/*         IN : t_uint8     Cardno. to access                               */
/*              t_uint32    Address of write protect group.                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_ClearWriteProtect(IN t_uint8 cardno, IN t_uint32 addr, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint8     cardstate;

    DBGENTER2("cardno(%d) addr(%d)", cardno, addr);

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD COOMND CLASS SUPPORTS WRITE PROTECTION */
    if (0 == ((g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2] >> 20) & MMC_CCCC_WRITE_PROT))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD IS PERMENENTLY WRITE PROTECT */
    if (g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[0] & MMC_PERMWP)
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD IS TEMPORARY WRITE PROTECT */
    else if (g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[0] & MMC_TEMPWP)
    {
#if defined(__MMC_SD_SOFTWARE_WORKAROUND)
        error = mmc_TempProtect(cardno, FALSE, mmc_device_id);  /*  Set bit12 of csd register low */
#endif
    }
    else
    {
        /*CHECK IF THE CARD SUPPORTS GROUP WRITE PROTECTION */
        if (!(g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[0] & BIT31))
        {
            error = MMC_REQUEST_NOT_APPLICABLE;
            DBGEXIT0(error);
            return(error);
        }

        /*SEND CMD29 CLR_WRITE_PROT WITH ARGUMENT AS addr */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCCLR_WRITE_PROT |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCCLR_WRITE_PROT, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        /*WAIT TILL THE CARD IS IN PROGRAMMING STATE */
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
        while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
        {
            error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
        }
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_SendWriteProtectStatus                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to retrieve write protect status for 32 */
/*              write protect groups starting from address specified.       */
/*              write protect group specified.                              */
/* PARAMETERS :                                                             */
/*         IN : t_uint8     Cardno. to access                               */
/*              t_uint32    Address of write protect group.                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT : t_uint32 *  Staus of 32 WP groups from the specified location*/
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_SendWriteProtectStatus
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_protectstatus,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    t_uint32    write_protect_status = 0;

    DBGENTER2("cardno(%d) addr (%d)", cardno, addr);

    if (NULL == p_protectstatus)
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /* High Capacity SD cards does not support Write Protection */
    if (MMC_HIGH_CAPACITY_SD_CARD == g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].card_type)
    {
        error = MMC_UNSUPPORTED_FEATURE;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD COOMND CLASS SUPPORTS WRITE PROTECTION */
    if (0 == ((g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2] >> 20) & MMC_CCCC_WRITE_PROT))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    /*SEND CMD16 SET_BLOCKLEN */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) 512;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*update global variable current_RblkLength.*/
    g_mmc_system_context[mmc_device_id].current_card.blk_length = 512;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 4;
    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl =
            (
                (4 << MMC_SHIFT16) |
                MMCREADDIR &~MMCSTREAMMODE
            ) |
            MMCDATAPATHENABLE;
    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl =
            (
                (2 << MMC_SHIFT4) |
                MMCREADDIR &~MMCSTREAMMODE
            ) |
            MMCDATAPATHENABLE;
    #endif
    /* SEND CMD30 SEND_WRITE_PROT */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = addr;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEND_WRITE_PROT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSEND_WRITE_PROT, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCRXOVERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATABLOCKEND | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXDATAAVLBL)
        {
            write_protect_status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXOVERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCRXOVERRUN;
        error = MMC_RX_OVERRUN;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        error = MMC_START_BIT_ERR;
        DBGEXIT0(error);
        return(error);
    }

    *p_protectstatus = ((write_protect_status & MMC_0TO7BITS) << MMC_SHIFT24) |
        (
            (write_protect_status & MMC_8TO15BITS) <<
            MMC_SHIFT8
        ) |
                ((write_protect_status & MMC_16TO23BITS) >> MMC_SHIFT8) |
                ((write_protect_status & MMC_24TO31BITS) >> MMC_SHIFT24);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;  /* clear all the static status flags */

    DBGEXIT0(error);

    return(error);
}

PRIVATE t_mmc_error mmc_Lock_UnlockCard_error_chk
(
    IN t_uint8          cardno,
    IN t_uint8          lock_set_chk,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_uint8     cardstate = 0;
    t_mmc_error error = MMC_OK;

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXUNDERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCTXUNDERRUN;
        error = MMC_TX_UNDERRUN;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        error = MMC_START_BIT_ERR;
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;  //clear all the static status flags
    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    if (TRUE == lock_set_chk)
    {
        /*CHECK IF CARD IS LOCKED OR NOT */
        if (!(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED))
        {
            error = MMC_LOCK_UNLOCK_FAILED;
        }
    }

    return(error);
}

/****************************************************************************/
/*       NAME : MMC_SetPassword                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to set password for the card specified. */
/* PARAMETERS :                                                             */
/*         IN : t_uint8 Card to access.                                     */
/*              t_uint8 password length in bytes.                           */
/*              t_uint8* buff containing password.                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PRIVATE t_mmc_error mmc_SetClearPassword_init_setting
(
    IN t_uint8          cardno,
    IN t_uint8          passleng,
    IN t_uint8          *p_password,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    if (NULL == p_password)
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    if
    (
        (cardno > g_mmc_system_context[mmc_device_id].no_of_cards)
    ||  (0 == cardno)
    ||  (passleng < MMC_MINPASSLENG)
    ||  (passleng > MMC_MAXPASSLENG)
    )
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD COOMND CLASS SUPPORTS LOCK UNLOCK */
    if
    (
        0 ==
            ((g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2] >> MMC_SHIFT20) & MMC_CCCC_LOCK_UNLOCK)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    DBGEXIT0(error);
    return(error);
}

PUBLIC t_mmc_error MMC_SetPassword
(
    IN t_uint8          cardno,
    IN t_uint8          passleng,
    IN t_uint8          *p_password,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    count;
    t_uint16    blocklen;
    t_uint8     power, count1, count2, shift, pwcount;
    t_uint32    pwddata[MMC_PWLENGTH];
    error = MMC_OK;

    pwcount = passleng;
    DBGENTER2("cardno(%d),passleng(%d)", cardno, passleng);
    error = mmc_SetClearPassword_init_setting(cardno, passleng, p_password, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* Block to be sent is have structure  */
    /* 1byte : Lock/Unlockmode             */
    /* 1byte : containg passwordlength     */
    /* 1byte : containing password itself  */
    /* So block size is passleng +2        */
    blocklen = (t_uint16) (passleng + MMC_PWEXTRABYTES);

    #if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
    error = mmc_FindBlocklen(blocklen, &power);
    #else
    power = blocklen;
    #endif
    /*SEND CMD16  SET_BLOCKLEN */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) (blocklen);
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* update global variable current_WblkLength */
    g_mmc_system_context[mmc_device_id].current_card.blk_length = power;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = (t_uint32) (blocklen);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    for (count2 = 0; count2 < MMC_PWLENGTH; count2++)
    {
        pwddata[count2] = 0;
    }

    if (MMC_PWLEN2 >= passleng)
    {
        passleng = MMC_PWLEN2;
    }
    else if (MMC_PWLEN6 >= passleng)
    {
        passleng = MMC_PWLEN6;
    }
    else
    {
        passleng = MMC_PWLEN14;
    }

    pwddata[0] |= ((t_uint32) passleng << MMC_SHIFT8) | 0x01;
    for (count1 = 0, count2 = 0, shift = 16; count1 < pwcount; count1++, shift += 8)
    {
        pwddata[count2] |= ((t_uint32) p_password[count1] << (shift % 32));
        if ((shift % 32) == 24)
        {
            count2++;
        }
    }

    /*SEND CMD42 LOCK_UNLOCK  */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCLOCK_UNLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCLOCK_UNLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ENABLE DATA PATH */
    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = ((t_uint32) power << MMC_SHIFT16) | (t_uint32) MMCDATAPATHENABLE &~((t_uint32) MMCREADDIR | (t_uint32) MMCSTREAMMODE);
    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = ((t_uint32) power << MMC_SHIFT4) | (t_uint32) MMCDATAPATHENABLE &~((t_uint32) MMCREADDIR | (t_uint32) MMCSTREAMMODE);
    #endif

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCDATABLOCKEND | MMCTXUNDERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
        {
            for (count = 0; count < (t_uint32) (1 << power) / MMC_WORDDIV; count++)
            {
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = pwddata[count];
            }
        }
    }

    error = mmc_Lock_UnlockCard_error_chk(cardno, FALSE, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_ClearPassword                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to Clear password for the card specified.*/
/* PARAMETERS :                                                             */
/*         IN : t_uint8 Card to access.                                     */
/*              t_uint8 password length in bytes.                           */
/*              t_uint8* buff containing password.                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_ClearPassword
(
    IN t_uint8          cardno,
    IN t_uint8          passleng,
    IN t_uint8          *p_password,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    t_uint32    count;
    t_uint16    blocklen;
    t_uint8     power, count1, count2, shift, pwcount;
    t_uint32    pwddata[MMC_PWLENGTH];

    pwcount = passleng;

    DBGENTER2("cardno(%d),passleng(%d)", cardno, passleng);

    error = mmc_SetClearPassword_init_setting(cardno, passleng, p_password, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* Block to be sent is have structure  */
    /* 1byte : Lock/Unlockmode             */
    /* 1byte : containg passwordlength     */
    /* 1byte : containing password itself  */
    /* So block size is passleng +2        */
    blocklen = (t_uint16) (passleng + MMC_PWEXTRABYTES);

    #if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
    error = mmc_FindBlocklen(blocklen, &power);
    #else
    power = blocklen;
    #endif

    /* SEND CMD16 SET_BLOCKLEN */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) (blocklen);
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    //update global variable current_WblkLength
    g_mmc_system_context[mmc_device_id].current_card.blk_length = (t_uint32)power;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = (t_uint32) (blocklen);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    for (count2 = 0; count2 < MMC_PWLENGTH; count2++)
    {
        pwddata[count2] = 0;
    }

    if (passleng <= MMC_PWLEN2)
    {
        passleng = MMC_PWLEN2;
    }
    else if (passleng <= MMC_PWLEN6)
    {
        passleng = MMC_PWLEN6;
    }
    else
    {
        passleng = MMC_PWLEN14;
    }

    pwddata[0] |= ((t_uint32) passleng << 8) | 0x02;
    for (count1 = 0, count2 = 0, shift = 16; count1 < pwcount; count1++, shift += 8)
    {
        pwddata[count2] |= ((t_uint32) p_password[count1] << (shift % 32));
        if (24 == (shift % 32))
        {
            count2++;
        }
    }

    /*SEND CMD42 LOCK_UNLOCK */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCLOCK_UNLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCLOCK_UNLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ENABLE DATAPATH */
    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = ((t_uint32) power << MMC_SHIFT16) | MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = ((t_uint32) power << MMC_SHIFT4) | MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
    #endif

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCDATABLOCKEND | MMCTXUNDERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
        {
            for (count = 0; count < (t_uint32) (1 << power) / MMC_WORDDIV; count++)
            {
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = pwddata[count];
            }
        }
    }

    error = mmc_Lock_UnlockCard_error_chk(cardno, FALSE, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_ChangePassword                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to Change password for the card specified.*/
/* PARAMETERS :                                                             */
/*         IN :     t_uint8  Card to access.                                 */
/*         IN :     t_uint8  Old password length in bytes.                   */
/*         IN :     t_uint8* buff containing old password.                   */
/*         IN :     t_uint8  new password length in bytes.                   */
/*         IN :     t_uint8* buff containing new password.                   */
/*         IN :t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_ChangePassword
(
    IN t_uint8          cardno,
    IN t_uint8          oldpassleng,
    IN t_uint8          *p_oldpassword,
    IN t_uint8          newpassleng,
    IN t_uint8          *p_newpassword,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    DBGENTER3("cardno(%d),oldpassleng(%d)newpassleng(%d)", cardno, oldpassleng, newpassleng);
    if ((NULL == p_oldpassword) || (NULL == p_newpassword))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    if
    (
        (cardno > g_mmc_system_context[mmc_device_id].no_of_cards)
    ||  (0 == cardno)
    ||  (oldpassleng < MMC_MINPASSLENG)
    ||  (oldpassleng > MMC_MAXPASSLENG)
    ||  (newpassleng < MMC_MINPASSLENG)
    ||  (newpassleng > MMC_MAXPASSLENG)
    )
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD COOMND CLASS SUPPORTS LOCK UNLOCK */
    if
    (
        0 ==
            ((g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2] >> MMC_SHIFT20) & MMC_CCCC_LOCK_UNLOCK)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    error = MMC_ClearPassword(cardno, oldpassleng, p_oldpassword, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    error = MMC_SetPassword(cardno, newpassleng, p_newpassword, mmc_device_id);

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_LockCard                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to lock the card specified.             */
/* PARAMETERS :                                                             */
/*         IN : t_uint8  Card to access.                                    */
/*              t_uint8  password length in bytes.                          */
/*              t_uint8* buff containing password.                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PRIVATE t_mmc_error mmc_Lock_UnlockCard_setting
(
    IN t_uint8          cardno,
    IN t_uint8          passleng,
    IN t_uint8          *p_password,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint16    blocklen;
    t_uint8     power;

    if (NULL == p_password)
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    if
    (
        (cardno > g_mmc_system_context[mmc_device_id].no_of_cards)
    ||  (0 == cardno)
    ||  (passleng < MMC_MINPASSLENG)
    ||  (passleng > MMC_MAXPASSLENG)
    )
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD COOMND CLASS SUPPORTS LOCK UNLOCK */
    if
    (
        0 ==
            ((g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2] >> MMC_SHIFT20) & MMC_CCCC_LOCK_UNLOCK)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /* Block to be sent is have structure  */
    /* 1byte : Lock/Unlockmode             */
    /* 1byte : containg passwordlength     */
    /* 1byte : containing password itself  */
    /* So block size is passleng +2        */
    blocklen = (t_uint16) (passleng + MMC_PWEXTRABYTES);

    #if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
    error = mmc_FindBlocklen(blocklen, &power);
    #else
    power = blocklen;
    #endif

    /*SEND CMD16 SET_BLOCKLEN */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) (blocklen);
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /* update global variable current_WblkLength */
    g_mmc_system_context[mmc_device_id].current_card.blk_length = (t_uint32)power;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = (t_uint32) (blocklen);

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    return(error);
}

PUBLIC t_mmc_error MMC_LockCard
(
    IN t_uint8          cardno,
    IN t_uint8          passleng,
    IN t_uint8          *p_password,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_uint8     count2 = 0, count1 = 0;
    t_mmc_error error = MMC_OK;
    t_uint32    count, power;
    t_uint32    pwddata[MMC_PWLENGTH];
    t_uint8     pwcount = 1, shift;

    DBGENTER2("cardno(%d)passleng(%d)", cardno, passleng);

    error = mmc_Lock_UnlockCard_setting(cardno, passleng, p_password, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    power = g_mmc_system_context[mmc_device_id].current_card.blk_length;

    /* SEND CMD42 LOCK_UNLOCK */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCLOCK_UNLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCLOCK_UNLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = ((t_uint32) power << MMC_SHIFT16) | MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = ((t_uint32) power << MMC_SHIFT4) | MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
    #endif


    for (count2 = 0; count2 < MMC_PWLENGTH; count2++)
    {
        pwddata[count2] = 0;
    }

    if (MMC_PWLEN2 >= passleng)
    {
        passleng = MMC_PWLEN2;
    }
    else if (MMC_PWLEN6 >= passleng)
    {
        passleng = MMC_PWLEN6;
    }
    else
    {
        passleng = MMC_PWLEN14;
    }

    pwddata[0] |= ((t_uint32) passleng << 8) | 0x04;
    for (count1 = 0, count2 = 0, shift = 16; count1 < pwcount; count1++, shift += 8)
    {
        pwddata[count2] |= ((t_uint32) p_password[count1] << (shift % 32));
        if (24 == (shift % 32))
        {
            count2++;
        }
    }

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCDATABLOCKEND | MMCTXUNDERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
        {
            for (count = 0; count < ((t_uint32) (1 << power) / MMC_WORDDIV); count++)
            {
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = pwddata[count];
            }
        }
    }

    error = mmc_Lock_UnlockCard_error_chk(cardno, TRUE, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/*       NAME : MMC_UnLockCard                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to Unlock the card specified.           */
/* PARAMETERS :                                                             */
/*         IN : t_uint8  Card to access.                                    */
/*              t_uint8  password length in bytes.                          */
/*              t_uint8* buff containing password.                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_UnlockCard
(
    IN t_uint8          cardno,
    IN t_uint8          passleng,
    IN t_uint8          *p_password,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    count, power;
    t_uint8     count1, count2, shift, pwcount;
    t_uint32    pwddata[MMC_PWLENGTH];

    pwcount = passleng;
    DBGENTER2("cardno(%d)passleng(%d)", cardno, passleng);

    error = mmc_Lock_UnlockCard_setting(cardno, passleng, p_password, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    power = g_mmc_system_context[mmc_device_id].current_card.blk_length;

    for (count2 = 0; count2 < MMC_PWLENGTH; count2++)
    {
        pwddata[count2] = 0;
    }

    if (MMC_PWLEN2 >= passleng)
    {
        passleng = MMC_PWLEN2;
    }
    else if (MMC_PWLEN6 >= passleng)
    {
        passleng = MMC_PWLEN6;
    }
    else
    {
        passleng = MMC_PWLEN14;
    }

    pwddata[0] |= ((t_uint32) passleng << 8) | 0x0;
    for (count1 = 0, count2 = 0, shift = 16; count1 < pwcount; count1++, shift += 8)
    {
        pwddata[count2] |= ((t_uint32) p_password[count1] << (shift % 32));
        if (24 == (shift % 32))
        {
            count2++;
        }
    }

    /*SEND CMD42 LOCK_UNLOCK */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCLOCK_UNLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCLOCK_UNLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ENABLE DATAPATH */
    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = ((t_uint32) power << MMC_SHIFT16) | MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = ((t_uint32) power << MMC_SHIFT4) | MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
    #endif
    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCDATABLOCKEND | MMCTXUNDERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
        {
            for (count = 0; count < ((t_uint32) (1 << power) / MMC_WORDDIV); count++)
            {
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = pwddata[count];
            }
        }
    }

    error = mmc_Lock_UnlockCard_error_chk(cardno, TRUE, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*       NAME : MMC_ForcedErase                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to erase all the card content along     */
/*              with the password set.                                      */
/* PARAMETERS :                                                             */
/*         IN : t_uint8  Card to access.                                    */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PUBLIC t_mmc_error MMC_ForceErase(IN t_uint8 cardno, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    blocklen;
    t_uint8     cardstate;

    DBGENTER1("cardno(%d)", cardno);

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*CHECK IF THE CARD COOMND CLASS SUPPORTS LOCK UNLOCK */
    if (0 == ((g_mmc_system_context[mmc_device_id].card_array[(cardno - 1)].csd[2] >> 20) & MMC_CCCC_LOCK_UNLOCK))
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    /* SEND COMMAND TO SELECT THE CARD, IF NEEDED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /* Block to be sent is have structure  */
    /* 1byte : Lock/Unlockmode             */
    /* 1byte : containg passwordlength     */
    /* 1byte : containing password itself  */
    /* So block size is passleng +2        */
    blocklen = 1;

    /*SEND CMD16 SET_BLOCKLEN */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) blocklen;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    //update global variable current_WblkLength
    g_mmc_system_context[mmc_device_id].current_card.blk_length = 0;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 1;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    /*SEND COMMAND 42 LOCK_UNLOCK */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCLOCK_UNLOCK |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCLOCK_UNLOCK, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ENABLE DATA PATH */
    //devarsh
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCDATABLOCKEND | MMCTXUNDERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = 0x00000008;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXUNDERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCTXUNDERRUN;
        error = MMC_TX_UNDERRUN;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        error = MMC_START_BIT_ERR;
        DBGEXIT0(error);
        return(error);
    }

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    DBGEXIT0(error);
    return(error);
}

/**********************************************************************************************/
/* NAME:    void  MMC_GetIRQSrcStatus()                                                       */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Status register and updates the status.        */
/* PARAMETERS:                                                                                */
/* IN :         t_mmc_irq_src      irqSrc:             identify the interrupt source          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT :        t_mmc_irq_status   *p_status:          status of the interrupt                */
/*                                                                                            */
/* RETURN:      none                                                                          */

/**********************************************************************************************/
PUBLIC void MMC_GetIRQSrcStatus
(
    IN t_mmc_irq_src        irqsrc,
    OUT t_mmc_irq_status    *p_status,
    IN t_mmc_device_id      mmc_device_id
)
{
    DBGENTER0();

    if ((t_uint32) MMC_IRQ_ALL == irqsrc)
    {
        p_status->initial_irq = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    }
    else
    {
        p_status->initial_irq = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & irqsrc;
    }

    p_status->irq_state = MMC_IRQ_STATE_NEW;
    p_status->pending_irq = p_status->initial_irq;

    DBGEXIT0(MMC_OK);
}

/**********************************************************************************************/
/* NAME:    void  MMC_IsIRQSrcActive()                                                        */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read Status register and checks whether the given       */
/*             interrupt source is active or not. It also updates the status(if not NULL) if  */
/*             it is active.                                                                  */
/* PARAMETERS:                                                                                */
/* IN :         t_mmc_irq_src      irqsrc:             identify the interrupt source          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT :        t_mmc_irq_status   *p_status:          status of the interrupt                */
/*                                                                                            */
/* RETURN:      t_bool        TRUE if the source is active.                                   */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                                             */

/**********************************************************************************************/
PUBLIC t_bool MMC_IsIRQSrcActive
(
    IN t_mmc_irq_src        irqsrc,
    OUT t_mmc_irq_status    *p_status,
    IN t_mmc_device_id      mmc_device_id
)
{
    t_mmc_irq_src   irqstatus;

    DBGENTER1(" (%d)", irqsrc);

    irqstatus = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;

    if (MMC_ALLZERO != (irqstatus & irqsrc))    /*ie IRQSrc(s) active*/
    {
        if (NULL != p_status)
        {
            if ((t_uint32) MMC_IRQ_ALL == irqsrc)
            {
                p_status->initial_irq = irqstatus;
            }
            else
            {
                p_status->initial_irq = irqstatus & irqsrc;
            }

            p_status->irq_state = MMC_IRQ_STATE_NEW;
            p_status->pending_irq = p_status->initial_irq;
        }

        DBGEXIT0(MMC_OK);
        return(TRUE);
    }
    else
    {
        DBGEXIT0(MMC_OK);
        return(FALSE);
    }
}

/**********************************************************************************************/
/* NAME:    t_mmc_error  MMC_ProcessIRQSrc()                                                  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process all the interrupts that are high.               */
/*             The status of the interrupt is provided by t_mmc_irq_status.                   */
/* PARAMETERS:                                                                                */
/* IN :        t_mmc_irq_status    *p_status:          status of the interrupt                */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT :       none                                                                           */
/*                                                                                            */
/* RETURN:      t_mmc_error      Error status                                                 */
/*              MMC_NO_MORE_PENDING_EVENT: when all the interrupts are processed successfully */
/*              MMC_NO_PENDING_EVENT_ERROR: when there is no interrupt to process             */

/**********************************************************************************************/
PRIVATE void mmc_ProcessIRQSrc_TX_FIFO_HALF_EMPTY(IN t_mmc_device_id mmc_device_id)
{
    t_uint32        count, rest_words;
    static t_uint32 bytes_transferred = 0;

    if
    (
        MMC_HALFFIFOBYTES >
            (g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes - bytes_transferred)
    )
    {
        rest_words =
            (
                (g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes - bytes_transferred) %
                MMC_WORDDIV == 0
            ) ?
                (
                    (g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes - bytes_transferred) /
                    MMC_WORDDIV
                ) :
                    (
                        (
                            g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes -
                            bytes_transferred
                        ) / MMC_WORDDIV + 1
                    );

        for
        (
            count = 0;
            count < rest_words;
            count++,
                (g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer)
                    ++, bytes_transferred += MMC_BYTEINC
        )
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *(g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer);
        }
    }
    else
    {
        for (count = 0; count < MMC_HALFFIFO; count++)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = *((g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer) + count);
        }

        g_mmc_system_context[mmc_device_id].current_card.mmc_context.source_buffer += MMC_HALFFIFO;
        bytes_transferred += MMC_HALFFIFOBYTES;
    }
    return;
}

PUBLIC t_mmc_error MMC_ProcessIRQSrc(IN t_mmc_irq_status *p_status, IN t_mmc_device_id mmc_device_id)
{
    t_uint32        count;
    static t_uint32 bytes_transferred = 0;
    t_uint32        it_status;

    it_status = p_status->pending_irq;

    if (0 == it_status)
    {
        return(MMC_NO_PENDING_EVENT_ERROR);
    }

    if (it_status & (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL)
    {
        for (count = 0; count < MMC_HALFFIFO; count++)
        {
            *(
                (g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer) +
                count
                    ) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
        }

        g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer += MMC_HALFFIFO;
        bytes_transferred += MMC_HALFFIFOBYTES;
    }
    else if (it_status & (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY)
    {
        mmc_ProcessIRQSrc_TX_FIFO_HALF_EMPTY(mmc_device_id);
    }

    if (it_status & (t_uint32) MMC_IRQ_DATA_END)
    {
        if (MMC_DMA_MODE != g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
        {
            while
            (
                (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & (t_uint32) MMC_IRQ_RX_DATA_AVLBL)
            &&  (bytes_transferred < g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes)
            )
            {
                *(g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer)
                        = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
                g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer++;
                bytes_transferred += MMC_BYTEINC;
            }
        }

        if (g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed)
        {
            /*SEND CMD12 STOP_TRANSMISSION */
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSTOP_TRANSMISSION |
                MMCRESPEXPECTED |
                MMCCMDPATHENABLE;
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = mmc_CmdResp1Error
                (
                    MMCSTOP_TRANSMISSION,
                    mmc_device_id
                );
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_OK;
        }

        bytes_transferred = 0;
        MMC_ClearIRQSrc((t_uint32) MMC_IRQ_DATA_END, mmc_device_id);
        MMC_DisableIRQSrc
        (
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            ),
            mmc_device_id
        );
        return(g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error);
    }

    if (it_status & (t_uint32) MMC_IRQ_DATA_CRC_FAIL)
    {
        MMC_ClearIRQSrc((t_uint32) MMC_IRQ_DATA_CRC_FAIL, mmc_device_id);
        MMC_DisableIRQSrc
        (
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            ),
            mmc_device_id
        );
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_CRC_FAIL;
        bytes_transferred = 0;
        return(MMC_DATA_CRC_FAIL);
    }

    if (it_status & (t_uint32) MMC_IRQ_DATA_TIMEOUT)
    {
        MMC_ClearIRQSrc((t_uint32) MMC_IRQ_DATA_TIMEOUT, mmc_device_id);
        MMC_DisableIRQSrc
        (
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            ),
            mmc_device_id
        );
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_TIMEOUT;
        bytes_transferred = 0;
        return(MMC_DATA_TIMEOUT);
    }

    if (it_status & (t_uint32) MMC_IRQ_RX_OVERRUN)
    {
        MMC_ClearIRQSrc((t_uint32) MMC_IRQ_RX_OVERRUN, mmc_device_id);
        MMC_DisableIRQSrc
        (
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            ),
            mmc_device_id
        );

        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_RX_OVERRUN;
        bytes_transferred = 0;
        return(MMC_RX_OVERRUN);
    }

    if (it_status & (t_uint32) MMC_IRQ_TX_UNDERRUN)
    {
        MMC_ClearIRQSrc((t_uint32) MMC_IRQ_TX_UNDERRUN, mmc_device_id);
        MMC_DisableIRQSrc
        (
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            ),
            mmc_device_id
        );
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_TX_UNDERRUN;
        bytes_transferred = 0;
        return(MMC_TX_UNDERRUN);
    }

    if (it_status & (t_uint32) MMC_IRQ_START_BIT_ERR)
    {
        MMC_ClearIRQSrc((t_uint32) MMC_IRQ_START_BIT_ERR, mmc_device_id);
        MMC_DisableIRQSrc
        (
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            ),
            mmc_device_id
        );
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_START_BIT_ERR;
        bytes_transferred = 0;
        return(MMC_START_BIT_ERR);
    }

    return(MMC_OK);
}

/**********************************************************************************************/
/* NAME:    t_mmc_error  MMC_FilterProcessIRQSrc()                                            */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process the active interrupts one by one.               */
/*             The status of the interrupt is provided by t_mmc_irq_status. After each status */
/*             is processed the t_mmc_event, global HCL event information and p_event are     */
/*             updated.                                                                       */
/* PARAMETERS:                                                                                */
/* IN :        t_mmc_irq_status   *p_status:       status of the interrupt                    */
/*             t_mmc_filter_mode  filter_mode:  filter to selectively process the interrupt   */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT :       t_mmc_event        *p_event:        current event that is handled by           */
/*                                                 the routine.                               */
/*                                                                                            */
/* RETURN:      t_mmc_error      Error status as follows                                      */
/*              MMC_NO_MORE_PENDING_EVENT: when all the interrupts are processed successfully */
/*              MMC_NO_MORE_FILTER_PENDING_EVENT: when all the interrupts of the              */
/*                                   given filter are processed successfully.                 */
/*              MMC_REMAINING_PENDING_EVENTS: When extended processing is required by the     */
/*                                   upper layer. HCL updates the event parameter passed      */
/*                                   which is then used by the upper layer to                 */
/*                                   extend the process. (return value by default)            */
/*              MMC_NO_PENDING_EVENT_ERROR: when there is no interrupt to process             */
/*              MMC_INTERNAL_EVENT: When all the status' are processed but the events         */
/*                                   are not processed.                                       */
/*                                                                                            */

/**********************************************************************************************/
PRIVATE void mmc_FilterProcessIRQSrc_sel(t_uint32 *temp_filter, t_mmc_irq_status *temp_status)
{
    if (*temp_filter & (t_uint32) MMC_IRQ_TX_UNDERRUN)
    {
        temp_status->pending_irq = (t_uint32) MMC_IRQ_TX_UNDERRUN;
    }
    else if (*temp_filter & (t_uint32) MMC_IRQ_RX_OVERRUN)
    {
        temp_status->pending_irq = (t_uint32) MMC_IRQ_RX_OVERRUN;
    }
    else if (*temp_filter & (t_uint32) MMC_IRQ_DATA_TIMEOUT)
    {
        temp_status->pending_irq = (t_uint32) MMC_IRQ_DATA_TIMEOUT;
    }
    else if (*temp_filter & (t_uint32) MMC_IRQ_DATA_CRC_FAIL)
    {
        temp_status->pending_irq = (t_uint32) MMC_IRQ_DATA_CRC_FAIL;
    }
    else if (*temp_filter & (t_uint32) MMC_IRQ_START_BIT_ERR)
    {
        temp_status->pending_irq = (t_uint32) MMC_IRQ_START_BIT_ERR;
    }
    else if (*temp_filter & (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY)
    {
        temp_status->pending_irq = (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY;
    }
    else if (*temp_filter & (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL)
    {
        temp_status->pending_irq = (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL;
    }
    else if (*temp_filter & (t_uint32) MMC_IRQ_DATA_END)
    {
        temp_status->pending_irq = (t_uint32) MMC_IRQ_DATA_END;
    }
return;
}

PUBLIC t_mmc_error MMC_FilterProcessIRQSrc
(
    IN t_mmc_irq_status     *p_status,
    OUT t_mmc_event         *p_event,
    IN t_mmc_filter_mode    filter_mode,
    IN t_mmc_device_id      mmc_device_id
)
{
    t_mmc_irq_status    temp_status;
    t_uint32            *temp_event;
    t_uint32            temp_filter;

    if ((MMC_IRQ_STATE_NEW == p_status->irq_state) && (0 == p_status->initial_irq))
    {
        return(MMC_NO_PENDING_EVENT_ERROR);
    }

    p_status->irq_state = MMC_IRQ_STATE_OLD;

    temp_event = &(g_mmc_system_context[mmc_device_id].current_card.mmc_context.mmc_event);

    if (NO_FILTER_MODE == filter_mode)
    {
        temp_filter = p_status->pending_irq;
        if (0 == temp_filter)
        {
            if (0 == (*temp_event))
            {
                return(MMC_NO_MORE_PENDING_EVENT);
            }
            else
            {
                return(MMC_INTERNAL_EVENT);
            }
        }
    }
    else
    {
        temp_filter = filter_mode & p_status->pending_irq;
        if (0 == temp_filter)
        {
            if (0 == ((*temp_event) & filter_mode))
            {
                if (0 == (*temp_event))
                {
                    return(MMC_NO_MORE_PENDING_EVENT);
                }
                else
                {
                    return(MMC_NO_MORE_FILTER_PENDING_EVENT);
                }
            }
            else
            {
                return(MMC_INTERNAL_EVENT);
            }
        }
    }

    temp_status.pending_irq = 0;
    mmc_FilterProcessIRQSrc_sel(&temp_filter, &temp_status);
    (*temp_event) |= temp_status.pending_irq;
    p_status->pending_irq &= ~(temp_status.pending_irq);
    *p_event = temp_status.pending_irq;

    mmc_ProcessIt(&temp_status, mmc_device_id);

    return(MMC_REMAINING_PENDING_EVENTS);
}

/**********************************************************************************************/
/* NAME:    t_bool  MMC_IsEventActive()                                                       */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to check whether the given event is active or not by       */
/*             checking the global event variable of the HCL.                                 */
/* PARAMETERS:                                                                                */
/* IN :        t_mmc_event        *p_event:      the event for which its status has to be     */
/*                                               checked.                                     */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT :       none                                                                           */
/*                                                                                            */
/* RETURN:      t_bool       TRUE if the event is active.                                     */

/**********************************************************************************************/
PUBLIC t_bool MMC_IsEventActive(IN t_mmc_event *p_event, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1(" (%d)", *p_event);
    if (((*p_event) & g_mmc_system_context[mmc_device_id].current_card.mmc_context.mmc_event) != 0)
    {
        DBGEXIT0(TRUE);
        return(TRUE);
    }

    DBGEXIT0(FALSE);
    return(FALSE);
}

/**********************************************************************************************/
/* NAME:    t_bool  MMC_AcknowledgeEvent()                                                    */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: AcknowledgeEvent() is called by the upper layer, after processing the given   */
/*              event. This routine resets the event bit of the global event variable.        */
/* PARAMETERS:                                                                                */
/* IN :        t_mmc_event        *p_event:      the event that has to be acknowledged        */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT :       none                                                                           */
/*                                                                                            */
/* RETURN:      none                                                                          */

/**********************************************************************************************/
PUBLIC void MMC_AcknowledgeEvent(IN t_mmc_event *p_event, IN t_mmc_device_id mmc_device_id)
{
    DBGENTER1(" (%d)", *p_event);
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.mmc_event &= ~(*p_event);
    DBGEXIT0(MMC_OK);
}

/**********************************************************************************************/
/* NAME:    t_mmc_error  MMC_SendStatus()                                                     */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Function current card's status.                                          */
/* PARAMETERS:                                                                                */
/* IN :        t_uint8        cardno: cardno whose status is desired.                         */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT :      t_uint32*       status: card's status                                           */
/*                                                                                            */
/* RETURN:      t_mmc_error                                                                   */

/**********************************************************************************************/
PUBLIC t_mmc_error MMC_SendStatus(IN t_uint8 cardno, OUT t_uint32 *p_cardstatus, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    DBGENTER0();

    if (NULL == p_cardstatus)
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
            (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
        MMC_SHIFT16;    /* rca */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEND_STATUS |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;

    error = mmc_CmdResp1Error(MMCSEND_STATUS, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    *p_cardstatus = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;

    DBGEXIT0(error);
    return(error);
}

/**********************************************************************************************/
/* NAME:    t_mmc_error  MMC_SendSDStatus()                                                   */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Function current SD card's status.                                       */
/* PARAMETERS:                                                                                */
/* IN :        t_uint8        cardno: cardno whose status is desired.                         */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT :      t_uint32*       status: card's status                                           */
/*                                                                                            */
/* RETURN:      t_mmc_error                                                                   */

/**********************************************************************************************/
PRIVATE t_mmc_error mmc_SendSDStatus_err_chk(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXOVERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCRXOVERRUN;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_RX_OVERRUN;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error = MMC_START_BIT_ERR;
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

PUBLIC t_mmc_error MMC_SendSDStatus(IN t_uint8 cardno, OUT t_uint32 *p_sdstatus, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    count;
    t_uint32    temp_sdstatus[16];
    t_uint32    *tempbuff = temp_sdstatus;

    DBGENTER1("cardno(%d)", cardno);

    /* REQUEST IS NOT APPLICABLE TO MMC CARD. */
    if
    (
        (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_UNSUPPORTED_FEATURE;
        DBGEXIT0(error);
        return(error);
    }

    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    /*SET BLOCK SIZE FOR CONTROLLER.*/
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) 64 << MMC_SHIFT16;


    /*SET BLOCK SIZE FOR CARD IF IT IS NOT EQUAL TO CURRENT BLOCK SIZE FOR CARD. */
    if (64 != g_mmc_system_context[mmc_device_id].current_card.blk_length)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) 64;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.blk_length = 64;
    }

    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) 6 << MMC_SHIFT4;

    /*SET BLOCK SIZE FOR CARD IF IT IS NOT EQUAL TO CURRENT BLOCK SIZE FOR CARD. */
    if (6 != g_mmc_system_context[mmc_device_id].current_card.blk_length)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) 6;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.blk_length = 6;
    }

    #endif

    /*SEND CMD55 APP_CMD with argument as card's RCA.*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
            (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
        MMC_SHIFT16;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = 64;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 64;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    /* ENABLE DATA PATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCREADDIR &~MMCSTREAMMODE | MMCDATAPATHENABLE;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_sdstatus;

    /*SEND ACMD13 SD_APP_STAUS   with argument as card's RCA.*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSD_APP_STAUS |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSD_APP_STAUS, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCRXOVERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATABLOCKEND | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXFIFOHALFFULL)
        {
            for (count = 0; count < MMC_HALFFIFO; count++)
            {
                *(tempbuff + count) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
            }

            tempbuff += MMC_HALFFIFO;
        }
    }

    error = mmc_SendSDStatus_err_chk(mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    while (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXDATAAVLBL)
    {
        *tempbuff = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
        tempbuff++;
    }

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = error;

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    for (count = 0; count < 16; count++)
    {
        p_sdstatus[count] = ((temp_sdstatus[count] & MMC_0TO7BITS) << MMC_SHIFT24) |
            ((temp_sdstatus[count] & MMC_8TO15BITS) << MMC_SHIFT8) |
                ((temp_sdstatus[count] & MMC_16TO23BITS) >> MMC_SHIFT8) |
                    ((temp_sdstatus[count] & MMC_24TO31BITS) >> MMC_SHIFT24);
    }

    DBGEXIT0(error);
    return(error);
}

/**********************************************************************************************/
/* NAME:    PRIVATE void  mmc_ProcessIt()                                                     */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process all the interrupts that are high.               */
/*             The status of the interrupt is provided by t_mmc_irq_status.                   */
/* PARAMETERS:                                                                                */
/* IN :        t_mmc_irq_status        *p_status:          status of the interrupt            */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used                   */
/* OUT :       none                                                                           */
/*                                                                                            */
/* RETURN:      none                                                                          */

/**********************************************************************************************/
PRIVATE void mmc_ProcessIt(OUT t_mmc_irq_status *p_status, IN t_mmc_device_id mmc_device_id)
{
    t_uint32        count;
    static t_uint32 bytes_transferred = 0;
    t_uint32        it_status;

#if defined SDIO_SUPPORT
    t_uint32        argument;
    t_uint8         io_direct_data;
#endif
    it_status = p_status->pending_irq;

    if (it_status & (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL)
    {
        for (count = 0; count < MMC_HALFFIFO; count++)
        {
            *(
                (g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer) +
                count
                    ) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
        }

        g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer += MMC_HALFFIFO;
        bytes_transferred += MMC_HALFFIFOBYTES;
    }
    else if (it_status & (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY)
    {
        mmc_ProcessIRQSrc_TX_FIFO_HALF_EMPTY(mmc_device_id);
    }
    else if (it_status & (t_uint32) MMC_IRQ_DATA_END)
    {
        if (MMC_DMA_MODE != g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode)
        {
            while
            (
                (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & (t_uint32) MMC_IRQ_RX_DATA_AVLBL)
            &&  (bytes_transferred < g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes)
            )
            {
                *(g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer)
                        = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
                g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer++;
                bytes_transferred += MMC_BYTEINC;
            }
        }

        if (g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed)
        {
#if defined SDIO_SUPPORT
            if
            (
                (
                    MMC_SECURE_DIGITAL_IO_CARD == g_mmc_system_context[mmc_device_id].card_array[g_mmc_system_context[
                        mmc_device_id].current_card.selected_card - 1].card_type
                )
            ||  (
                    MMC_SECURE_DIGITAL_IO_COMBO_CARD == g_mmc_system_context[mmc_device_id].card_array[
                        g_mmc_system_context[mmc_device_id].current_card.selected_card - 1].card_type
                )
            )
            {
                argument = (t_uint32) g_mmc_system_context[mmc_device_id].current_card.sdio_function | ((t_uint32) 0x06 << MMC_SHIFT9) |    /* Address of I/O Abort register of CCCR */
                ((t_uint32) 0x0 << MMC_SHIFT28) |   /* Select function 0, CIA */
                ((t_uint32) 0x0 << MMC_SHIFT27) |   /* No read after write */
                ((t_uint32) MMC_SDIO_WRITE << MMC_SHIFT31);

                /* SEND IO DIRECT COMMAND */
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
                    MMCRESPEXPECTED |
                    MMCCMDPATHENABLE;
                g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = mmc_CmdResp5Error
                    (
                        MMCIO_RW_DIRECT,
                        &io_direct_data,
                        mmc_device_id
                    );
            }
            else
#endif
            {
                /*SEND CMD12 STOP_TRANSMISSION */
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSTOP_TRANSMISSION |
                    MMCRESPEXPECTED |
                    MMCCMDPATHENABLE;
                g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = mmc_CmdResp1Error
                    (
                        MMCSTOP_TRANSMISSION,
                        mmc_device_id
                    );
            }
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_OK;
        }

        bytes_transferred = 0;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= (t_uint32) MMC_IRQ_DATA_END;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 &= ~
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            );
    }
    else if (it_status & (t_uint32) MMC_IRQ_DATA_CRC_FAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= (t_uint32) MMC_IRQ_DATA_CRC_FAIL;
        MMC_DisableIRQSrc
        (
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            ),
            mmc_device_id
        );
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_CRC_FAIL;
        bytes_transferred = 0;
    }
    else if (it_status & (t_uint32) MMC_IRQ_DATA_TIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= (t_uint32) MMC_IRQ_DATA_TIMEOUT;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 &= ~
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN
            );
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_DATA_TIMEOUT;
        bytes_transferred = 0;
    }
    else if (it_status & (t_uint32) MMC_IRQ_RX_OVERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= (t_uint32) MMC_IRQ_RX_OVERRUN;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 &= ~
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            );

        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_RX_OVERRUN;
        bytes_transferred = 0;
    }
    else if (it_status & (t_uint32) MMC_IRQ_TX_UNDERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= (t_uint32) MMC_IRQ_TX_UNDERRUN;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 &= ~
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            );
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_TX_UNDERRUN;
        bytes_transferred = 0;
    }
    else if (it_status & (t_uint32) MMC_IRQ_START_BIT_ERR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= (t_uint32) MMC_IRQ_START_BIT_ERR;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_mask0 &= ~
            (
                (t_uint32) MMC_IRQ_DATA_CRC_FAIL | (t_uint32) MMC_IRQ_DATA_TIMEOUT | (t_uint32) MMC_IRQ_DATA_END |
                    (t_uint32) MMC_IRQ_RX_FIFO_HALF_FULL |
                        (t_uint32) MMC_IRQ_TX_FIFO_HALF_EMPTY |
                        (t_uint32) MMC_IRQ_RX_OVERRUN |
                        (t_uint32) MMC_IRQ_TX_UNDERRUN |
                        (t_uint32) MMC_IRQ_START_BIT_ERR
            );
        g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_START_BIT_ERR;
        bytes_transferred = 0;
    }
}

/****************************************************************************/
/*       NAME : MMC_ProgramCSD                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This private routine allows to program CSD register of      */
/*              selected card                                               */
/* PARAMETERS :                                                             */
/*         IN : t_uint32        argument                                    */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */

/****************************************************************************/
PRIVATE t_mmc_error mmc_ProgramCSD
(
    IN t_uint32         card,
    IN t_uint32         bitfield,
    IN t_bool           status,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error;
    t_uint8     cardstate;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 16;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    /*SEND CMD27 PROG_CSD */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCPROG_CSD | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCPROG_CSD, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    /*ENABLE DATAPATH */
    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (16 << MMC_SHIFT16) | MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (16 << MMC_SHIFT4) | MMCDATAPATHENABLE &~(MMCREADDIR | MMCSTREAMMODE);
    #endif
    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCDATABLOCKEND | MMCTXUNDERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOHALFEMPTY)
        {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = (g_mmc_system_context[mmc_device_id].card_array[card - 1].csd[3]);
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = (g_mmc_system_context[mmc_device_id].card_array[card - 1].csd[2]);
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = (g_mmc_system_context[mmc_device_id].card_array[card - 1].csd[1]);
            if (TRUE == status)
            {
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo =
                    (
                        g_mmc_system_context[mmc_device_id].card_array[(card - 1)].csd[0] |
                        bitfield
                    );
            }
            else
            {
                g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = (g_mmc_system_context[mmc_device_id].card_array[(card - 1)].csd[0] &~bitfield);
            }
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        error = MMC_DATA_CRC_FAIL;
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXUNDERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCTXUNDERRUN;
        error = MMC_TX_UNDERRUN;
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        error = MMC_START_BIT_ERR;
        return(error);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;  /* clear all the static status flags */
    error = mmc_IsCardProgramming(card, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(card, &cardstate, mmc_device_id);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEND_CSD |
        MMCRESPEXPECTED |
        MMCLONGRESPONSE |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp2Error(mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    g_mmc_system_context[mmc_device_id].card_array[card - 1].csd[0] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response3;
    g_mmc_system_context[mmc_device_id].card_array[card - 1].csd[1] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response2;
    g_mmc_system_context[mmc_device_id].card_array[card - 1].csd[2] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response1;
    g_mmc_system_context[mmc_device_id].card_array[card - 1].csd[3] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;

    /*if (error == MMC_OK)
    {
        if (status == TRUE)
        g_mmc_system_context[mmc_device_id].card_array[(card-1)].csd[0] |= bitfield;
        else
        g_mmc_system_context[mmc_device_id].card_array[(card-1)].csd[0] &= ~bitfield;
    }*/
    return(error);
}
#if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
/****************************************************************************/
/*       NAME : mmc_FindBlocklen                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This private routine allows to find block length if no. of  */
/*              bytes transferred is given.                                 */
/* PARAMETERS :                                                             */
/*         IN : t_uint8     cardno                                          */
/*              t_uint8     blocklen                                        */
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_mmc_error                                                 */
/*     COMMENT: suppose 1024 bytes given then 2^10 so return 10. If 13 bytes*/
/*              then 13->16 so 2^4 , will return 4                          */

/****************************************************************************/
PRIVATE t_mmc_error mmc_FindBlocklen(IN t_uint16 nobytes, OUT t_uint8 *p_power)
{
    t_mmc_error error = MMC_OK;
    t_uint16    shift;
    t_uint8     req_power;

    if (nobytes > MMC_MAXBLSIZE)
    {
        return(MMC_INVALID_PARAMETER);
    }

    if (!(nobytes & (nobytes - 1)))
    {
        shift = (t_uint16) (nobytes << (15 - MAXBSIZEPOWER));
        req_power = MAXBSIZEPOWER;
        while (shift > 0)
        {
            shift <<= 1;
            --req_power;
            error = MMC_OK;
        }

        *p_power = (t_uint8) (req_power + 1);
    }
    else
    {
        shift = (t_uint16) (nobytes << (15 - MAXBSIZEPOWER));
        req_power = MAXBSIZEPOWER;

        while (!(shift & 0x8000))
        {
            shift <<= 1;
            --req_power;
            error = MMC_OK;
        }

        *p_power = (t_uint8) (req_power + 1);
    }

    return(error);
}


PRIVATE t_uint8 mmc_convert_from_bytes_to_power_of_two(t_uint16 no_of_bytes)
{
    t_uint8 count = 0;

    while (no_of_bytes != 1)
    {
        no_of_bytes >>= 1;
        count++;
    }

    return(count);
}
#endif
/********************************************************************************/
/*       NAME : MMC_CmdError()                                                  */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine checks for error conditions for CMD0       */
/* PARAMETERS :                                                                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : NONE                                                            */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PRIVATE t_mmc_error mmc_CmdError(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    timeout;

#if defined __MMCI_HCL_PRIVATE_DEBUG
    timeout = (t_uint32) ((float) (64 / (float) clockfreq) / ((float) (1 / (float) MCLK) + 5 * (float) (1 / (float) PROCESSOR_CLK)));
#endif
    timeout = MMC_CMD0TIMEOUT;
    while ((timeout > 0) && !(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCCMDSENT))
    {
        timeout--;
    }

    if (0 == timeout)
    {
        error = MMC_CMD_RSP_TIMEOUT;
        return(error);
    }

    /*clear all the static flags */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    return(error);
}

/********************************************************************************/
/*       NAME : mmc_CmdResp5Error()                                             */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine checks for error conditions for R5 response*/
/* PARAMETERS :                                                                 */
/*         IN : t_uint8 cmd :command sent                                       */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : t_uint8 data:byte read from card                                */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PRIVATE t_mmc_error mmc_CmdResp5Error(IN t_uint8 cmd, t_uint8 *data, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    status;
    t_uint32    response_r5;

    status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    while (!(status & (MMCCMDCRCFAIL | MMCCMDRESPEND | MMCCMDTIMEOUT)))
    {
        status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    }

    if (status & MMCCMDTIMEOUT)
    {
        error = MMC_CMD_RSP_TIMEOUT;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDTIMEOUT;
        return(error);
    }
    else if (status & MMCCMDCRCFAIL)
    {
        error = MMC_CMD_CRC_FAIL;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDCRCFAIL;
        return(error);
    }

    /* CHECK RESPONSE RECEIVED IS OF DESIRED COMMAND */
    if (cmd != g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_resp_command)
    {
        error = MMC_ILLEGAL_CMD;
        return(error);
    }

    /*Clear Static flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    /* WE HAVE RECEIVED RESPONSE, RETRIEVE IT FOR ANALYSIS  */
    response_r5 = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;

    *data = (t_uint8) (response_r5 & MASK_BYTE);

    if (response_r5 & MMCSDIO_OUT_OF_RANGE)
    {
        return(MMC_ADDR_OUT_OF_RANGE);
    }

    if (response_r5 & MMCSDIO_FUNCTION_NUMBER)
    {
        return(MMC_SDIO_UNKNOWN_FUNCTION);
    }

    if (response_r5 & MMCSDIO_COM_CRC_ERR)
    {
        return(MMC_COM_CRC_FAILED);
    }

    if (response_r5 & MMCSDIO_ILLEGAL_COMM)
    {
        return(MMC_ILLEGAL_CMD);
    }

    if (response_r5 & MMCSDIO_UNKNOWN_ERROR)
    {
        return(MMC_GENERAL_UNKNOWN_ERROR);
    }

    if (0x0 == ((response_r5 & MMCSDIO_CURRENT_STATE) >> MMC_SHIFT12))
    {
        return(MMC_SDIO_DISABLED);
    }

    return(error);
}

/*************************************************************************************/
/*       NAME : mmc_CmdResp4Error()                                                  */
/*-----------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine checks for error conditions for R4 SDIO response*/
/* PARAMETERS :                                                                      */
/*         IN : t_uint8 cmd :command sent                                            */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used          */
/*        OUT : NONE                                                                 */
/*                                                                                   */
/*     RETURN : t_mmc_error : MMC error code                                         */

/*************************************************************************************/
PRIVATE t_mmc_error mmc_CmdResp4Error(IN t_uint8 cmd, OUT t_mmc_sdio_resp4 *resp, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    status, timeout;
    t_uint32    response_r4;
    timeout = MMC_CMD0TIMEOUT;

    status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    while ((timeout > 0) && !(status & MMCCMDCRCFAIL))
    {
        timeout--;
        status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    }

    /*Clear Static flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    /* WE HAVE RECEIVED RESPONSE, RETRIEVE IT FOR ANALYSIS  */
    response_r4 = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;

    if ((0 == timeout) || (0 == response_r4))
    {
        error = MMC_CMD_RSP_TIMEOUT;
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.sdio_info.card_ready_after_init =
        (response_r4 & MMCR4CARD_READY_AFTER_INIT) >>
        MMCR4CARD_READY_AFTER_INIT_sb;
    g_mmc_system_context[mmc_device_id].current_card.sdio_info.memory_present = (response_r4 & MMCR4MEMORY_PRESENT) >> MMCR4MEMORY_PRESENT_sb;
    g_mmc_system_context[mmc_device_id].current_card.sdio_info.no_of_io_funcs = (response_r4 & MMCR4NO_OF_IO_FUNCS) >> MMCR4NO_OF_IO_FUNCS_sb;
    g_mmc_system_context[mmc_device_id].current_card.sdio_info.op_cond_register = (response_r4 & MMCR4OP_COND_REG);

    resp->card_ready_after_init = (response_r4 & MMCR4CARD_READY_AFTER_INIT) >> MMCR4CARD_READY_AFTER_INIT_sb;
    resp->no_of_io_funcs = (response_r4 & MMCR4NO_OF_IO_FUNCS) >> MMCR4NO_OF_IO_FUNCS_sb;
    resp->memory_present = (response_r4 & MMCR4MEMORY_PRESENT) >> MMCR4MEMORY_PRESENT_sb;
    resp->op_cond_register = (response_r4 & MMCR4OP_COND_REG);

    return(error);
}

/********************************************************************************/
/*       NAME : mmc_CmdResp7Error()                                             */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine checks for error conditions for R7 response*/
/* PARAMETERS :                                                                 */
/*         IN : t_uint8 cmd :command sent                                       */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : NONE                                                            */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PRIVATE t_mmc_error mmc_CmdResp7Error(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    status;
    t_uint32    timeout = MMC_CMD0TIMEOUT;

    status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    while (!(status & (MMCCMDCRCFAIL | MMCCMDRESPEND | MMCCMDTIMEOUT)) && (timeout > 0))
    {
        timeout--;
        status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    }

    if ((0 == timeout) || (status & MMCCMDTIMEOUT))
    {
        /* Card is not V2.0 complient or card does not support the set voltage range */
        error = MMC_CMD_RSP_TIMEOUT;
        return(error);
    }

    if (status & MMCCMDRESPEND)
    {
        /* Card is V2.0 complient */
        error = MMC_OK;
        return(error);
    }

    return(error);
}

/********************************************************************************/
/*       NAME : mmc_CmdResp1Error()                                             */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine checks for error conditions for R1 response*/
/* PARAMETERS :                                                                 */
/*         IN : t_uint8 cmd :command sent                                       */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : NONE                                                            */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PRIVATE t_mmc_error mmc_CmdResp1Error_sel(t_uint32 response_r1)
{
    t_mmc_error error = MMC_OK;

    if (response_r1 & MMCADDR_OUT_OF_RANGE)
    {
        return(MMC_ADDR_OUT_OF_RANGE);
    }

    if (response_r1 & MMCADDR_MISALIGNED)
    {
        return(MMC_ADDR_MISALIGNED);
    }

    if (response_r1 & MMCBLOCK_LEN_ERR)
    {
        return(MMC_BLOCK_LEN_ERR);
    }

    if (response_r1 & MMCERASE_SEQ_ERR)
    {
        return(MMC_ERASE_SEQ_ERR);
    }

    if (response_r1 & MMCBAD_ERASE_PARAM)
    {
        return(MMC_BAD_ERASE_PARAM);
    }

    if (response_r1 & MMCWRITE_PROT_VIOLATION)
    {
        return(MMC_WRITE_PROT_VIOLATION);
    }

    if (response_r1 & MMCLOCK_UNLOCK_FAILED)
    {
        return(MMC_LOCK_UNLOCK_FAILED);
    }

    if (response_r1 & MMCCOM_CRC_FAILED)
    {
        return(MMC_COM_CRC_FAILED);
    }

    if (response_r1 & MMCILLEGAL_CMD)
    {
        return(MMC_ILLEGAL_CMD);
    }

    if (response_r1 & MMCCARD_ECC_FAILED)
    {
        return(MMC_CARD_ECC_FAILED);
    }

    if (response_r1 & MMCCC_ERROR)
    {
        return(MMC_CC_ERROR);
    }

    if (response_r1 & MMCSTREAM_READ_UNDERRUN)
    {
        return(MMC_STREAM_READ_UNDERRUN);
    }

    if (response_r1 & MMCSTREAM_WRITE_OVERRUN)
    {
        return(MMC_STREAM_WRITE_OVERRUN);
    }

    if (response_r1 & MMCCID_CSD_OVERWRIETE)
    {
        return(MMC_CID_CSD_OVERWRITE);
    }

    if (response_r1 & MMCWP_ERASE_SKIP)
    {
        return(MMC_WP_ERASE_SKIP);
    }

    if (response_r1 & MMCCARD_ECC_DISABLED)
    {
        return(MMC_CARD_ECC_DISABLED);
    }

    if (response_r1 & MMCERASE_RESET)
    {
        return(MMC_ERASE_RESET);
    }

    if (response_r1 & MMCAKE_SEQ_ERROR)
    {
        return(MMC_AKE_SEQ_ERROR);
    }

    return(error);
}

PRIVATE t_mmc_error mmc_CmdResp1Error(IN t_uint8 cmd, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    status;
    t_uint32    response_r1;

    status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    while (!(status & (MMCCMDCRCFAIL | MMCCMDRESPEND | MMCCMDTIMEOUT)))
    {
        status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    }

    if (status & MMCCMDTIMEOUT)
    {
        error = MMC_CMD_RSP_TIMEOUT;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDTIMEOUT;
        return(error);
    }
    else if (status & MMCCMDCRCFAIL)
    {
        error = MMC_CMD_CRC_FAIL;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDCRCFAIL;
        return(error);
    }

    /* CHECK RESPONSE RECEIVED IS OF DESIRED COMMAND */
    if (cmd != g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_resp_command)
    {
        error = MMC_ILLEGAL_CMD;
        return(error);
    }

    /*Clear Static flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    /* WE HAVE RECEIVED RESPONSE, RETRIEVE IT FOR ANALYSIS  */
    response_r1 = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;
    error = mmc_CmdResp1Error_sel(response_r1);

    return(error);
}

PRIVATE t_mmc_error mmc_SwitchResp1Error(IN t_uint8 cmd, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint8 cardstate;
    t_uint8 cardno = 1;

    error = mmc_CmdResp1Error( cmd, mmc_device_id);

    if (MMC_OK == error) {
        error = mmc_IsCardProgramming( cardno, &cardstate, mmc_device_id);
        while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
        {
            error = mmc_IsCardProgramming( cardno, &cardstate, mmc_device_id);
        }
    }

    return(error);
}


/********************************************************************************/
/*       NAME : MMC_CmdResp6Error()                                             */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine checks for error conditions for R6 response*/
/* PARAMETERS :                                                                 */
/*         IN : t_uint8 cmd :command sent                                       */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : NONE                                                            */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PRIVATE t_mmc_error mmc_CmdResp6Error(IN t_uint8 cmd, OUT t_uint16 *p_rca, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    status;
    t_uint32    response_r1;

    status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    while (!(status & (MMCCMDCRCFAIL | MMCCMDRESPEND | MMCCMDTIMEOUT)))
    {
        status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    }

    if (status & MMCCMDTIMEOUT)
    {
        error = MMC_CMD_RSP_TIMEOUT;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDTIMEOUT;
        return(error);
    }
    else if (status & MMCCMDCRCFAIL)
    {
        error = MMC_CMD_CRC_FAIL;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDCRCFAIL;
        return(error);
    }

    /* CHECK RESPONSE RECEIVED IS OF DESIRED COMMAND */
    if (cmd != g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_resp_command)
    {
        error = MMC_ILLEGAL_CMD;
        return(error);
    }

    /*Clear Static flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    /* WE HAVE RECEIVED RESPONSE, RETRIEVE IT.  */
    response_r1 = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;

    if (MMC_ALLZERO == (response_r1 & (MMCR6GENERAL_UNKNOWN_ERROR | MMCR6ILLEGAL_CMD | MMCR6COM_CRC_FAILED)))
    {
        *p_rca = (t_uint16) (response_r1 >> MMC_SHIFT16);
        return(error);
    }

    if (response_r1 & MMCR6GENERAL_UNKNOWN_ERROR)
    {
        return(MMC_GENERAL_UNKNOWN_ERROR);
    }

    if (response_r1 & MMCR6ILLEGAL_CMD)
    {
        return(MMC_ILLEGAL_CMD);
    }

    if (response_r1 & MMCR6COM_CRC_FAILED)
    {
        return(MMC_COM_CRC_FAILED);
    }

    return(error);
}

/********************************************************************************/
/*       NAME : MMC_CmdResp2Error()                                             */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine checks for error conditions for R2 response*/
/* PARAMETERS :                                                                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : NONE                                                            */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PRIVATE t_mmc_error mmc_CmdResp2Error(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    status;

#if defined __MMCI_HCL_PRIVATE_DEBUG
    t_uint32    timeout;
    timeout = (t_uint32) ((float) (64 / (float) clockfreq) / ((float) (1 / (float) MCLK) + 8 * (float) (1 / (float) PROCESSOR_CLK)));
#endif
    status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    while (!(status & (MMCCMDCRCFAIL | MMCCMDTIMEOUT | MMCCMDRESPEND)))
    {
        status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    }

    if (status & MMCCMDTIMEOUT)
    {
        error = MMC_CMD_RSP_TIMEOUT;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDTIMEOUT;
        return(error);
    }
    else if (status & MMCCMDCRCFAIL)
    {
        error = MMC_CMD_CRC_FAIL;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDCRCFAIL;
        return(error);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;  /*Clear Static flags*/

    return(error);
}

/********************************************************************************/
/*       NAME : MMC_CmdResp3Error()                                             */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine checks for error conditions for R3 response*/
/* PARAMETERS :                                                                 */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : NONE                                                            */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PRIVATE t_mmc_error mmc_CmdResp3Error(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    status;

#if defined __MMCI_HCL_PRIVATE_DEBUG
    t_uint32    timeout;
    timeout = (t_uint32) ((float) (1 / (float) clockfreq) / ((float) (1 / (float) MCLK) + 8 * (float) (1 / (float) PROCESSOR_CLK)));
#endif
    status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    while (!(status & (MMCCMDRESPEND | MMCCMDTIMEOUT | MMCCMDCRCFAIL)))
    {
        status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    }

    if (status & MMCCMDTIMEOUT)
    {
        error = MMC_CMD_RSP_TIMEOUT;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDTIMEOUT;
        return(error);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    return(error);
}

PRIVATE t_mmc_error mmc_IsCardProgramming(t_uint32 cardno, t_uint8 *p_status, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    response_r1, err_status;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
            (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
        MMC_SHIFT16;    /* RCA */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEND_STATUS |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;

    err_status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;

    while (!(err_status & (MMCCMDCRCFAIL | MMCCMDRESPEND | MMCCMDTIMEOUT)))
    {
        err_status = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status;
    }

    if (err_status & MMCCMDTIMEOUT)
    {
        error = MMC_CMD_RSP_TIMEOUT;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDTIMEOUT;
        return(error);
    }
    else if (err_status & MMCCMDCRCFAIL)
    {
        error = MMC_CMD_CRC_FAIL;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCCMDCRCFAIL;
        return(error);
    }

    /* CHECK RESPONSE RECEIVED IS OF DESIRED COMMAND */
    if (MMCSEND_STATUS != g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_resp_command)
    {
        error = MMC_ILLEGAL_CMD;
        return(error);
    }

    /*Clear Static flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    /* WE HAVE RECEIVED RESPONSE, RETRIEVE IT FOR ANALYSIS  */
    response_r1 = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0;

    /*FIND OUT CARD STATUS */
    *p_status = (t_uint8) ((response_r1 >> 9) & 0x0000000f);

    error = mmc_CmdResp1Error_sel(response_r1);
    if (MMC_OK != error)
    {
        return(error);
    }

    return(error);
}

/*PRIVATE FUNCTION TO GET SCR REGISTER OF SD-CARD */
PRIVATE t_mmc_error mmc_FINDSCR(IN t_uint16 rca, OUT t_uint32 *p_scr, IN t_mmc_device_id mmc_device_id)
{
    t_uint32    index = 0;
    t_mmc_error error = MMC_OK;
    t_uint32    temp_scr[2] = { 0, 0 };

    /*SET BLOCK SIZE TO 8 BYTES */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) 8;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.blk_length = 8;

    /*SEND CMD55 APP_CMD with agument as card's rca.*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) rca << MMC_SHIFT16;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    /*ENABLE DATAPATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 8;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    /* ENABLE DATA PATH */
    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (8 << MMC_SHIFT16) |
        MMCREADDIR &~MMCSTREAMMODE |
        MMCDATAPATHENABLE;

    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (3 << MMC_SHIFT4) |
        MMCREADDIR &~MMCSTREAMMODE |
        MMCDATAPATHENABLE;
    #endif
    /*SEND ACMD51 SD_APP_SEND_SCR with argument as 0*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSD_APP_SEND_SCR |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSD_APP_SEND_SCR, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCRXOVERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATABLOCKEND | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXDATAAVLBL)
        {
            *(temp_scr + index) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
            index++;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        error = MMC_DATA_CRC_FAIL;
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXOVERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCRXOVERRUN;
        error = MMC_RX_OVERRUN;
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        error = MMC_START_BIT_ERR;
        return(error);
    }

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    *(p_scr + 1) = ((temp_scr[0] & MMC_0TO7BITS) << MMC_SHIFT24) | ((temp_scr[0] & MMC_8TO15BITS) << MMC_SHIFT8) | ((temp_scr[0] & MMC_16TO23BITS) >> MMC_SHIFT8) | ((temp_scr[0] & MMC_24TO31BITS) >> MMC_SHIFT24);

    *(p_scr) = ((temp_scr[1] & MMC_0TO7BITS) << MMC_SHIFT24) | ((temp_scr[1] & MMC_8TO15BITS) << MMC_SHIFT8) | ((temp_scr[1] & MMC_16TO23BITS) >> MMC_SHIFT8) | ((temp_scr[1] & MMC_24TO31BITS) >> MMC_SHIFT24);

    return(error);
}

/********************************************************************************/
/*       NAME : MMC_GETSCR()                                                    */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This Public routine finds scr register for sd card.             */
/* PARAMETERS :                                                                 */
/*         IN : t_uint8 cardno: cardno                                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : t_uint32* scr register                                          */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PRIVATE t_mmc_error MMC_GetSCR_errorChk(IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXOVERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCRXOVERRUN;
        error = MMC_RX_OVERRUN;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        error = MMC_START_BIT_ERR;
        DBGEXIT0(error);
        return(error);
    }

    return(error);
}

PUBLIC t_mmc_error MMC_GetSCR(IN t_uint8 cardno, OUT t_uint32 *p_scr, IN t_mmc_device_id mmc_device_id)
{
    t_uint32    index = 0;
    t_mmc_error error = MMC_OK;
    t_uint32    temp_scr[2] = { 0, 0 };

    DBGENTER1("cardno(%d)", cardno);
    if
    (
        (MMC_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_SPEED_MULTIMEDIA_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    ||  (MMC_HIGH_CAPACITY_MMC_CARD == g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_UNSUPPORTED_FEATURE;
        DBGEXIT0(error);
        return(error);
    }

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno) || (NULL == p_scr))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*SELECT CARD,IF NOT SELCTED ALREADY */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        DBGEXIT0(error);
        return(error);
    }

    /*SET BLOCK SIZE TO 8 BYTES */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) 8;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.blk_length = 8;

    /*SEND CMD55 APP_CMD with agument as card's RCA.*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
            (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
        MMC_SHIFT16;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*ENABLE DATAPATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 8;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    /* ENABLE DATA PATH */
    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (3 << MMC_SHIFT16) |
        MMCREADDIR &~MMCSTREAMMODE |
        MMCDATAPATHENABLE;
    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (3 << MMC_SHIFT4) |
        MMCREADDIR &~MMCSTREAMMODE |
        MMCDATAPATHENABLE;
    #endif
    /*SEND ACMD51 SD_APP_SEND_SCR with argument as 0*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSD_APP_SEND_SCR |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCSD_APP_SEND_SCR, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCRXOVERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATABLOCKEND | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXDATAAVLBL)
        {
            *(temp_scr + index) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
            index++;
        }
    }

    error = MMC_GetSCR_errorChk(mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    *(p_scr + 1) = ((temp_scr[0] & MMC_0TO7BITS) << MMC_SHIFT24) | ((temp_scr[0] & MMC_8TO15BITS) << MMC_SHIFT8) | ((temp_scr[0] & MMC_16TO23BITS) >> MMC_SHIFT8) | ((temp_scr[0] & MMC_24TO31BITS) >> MMC_SHIFT24);

    *(p_scr) = ((temp_scr[1] & MMC_0TO7BITS) << MMC_SHIFT24) | ((temp_scr[1] & MMC_8TO15BITS) << MMC_SHIFT8) | ((temp_scr[1] & MMC_16TO23BITS) >> MMC_SHIFT8) | ((temp_scr[1] & MMC_24TO31BITS) >> MMC_SHIFT24);

    DBGEXIT0(error);
    return(error);
}

/********************************************************************************/
/*       NAME : MMC_GetExtCSD()                                                 */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This Public routine finds Ext-CSD register for HS-MMC card.     */
/* PARAMETERS :                                                                 */
/*         IN : t_uint8 cardno: cardno                                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : t_uint32* pointer to ext csd register.                          */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PUBLIC t_mmc_error MMC_GetExtCSD(IN t_uint8 cardno, OUT t_uint32 *p_extcsd, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    count;
    t_uint32    *p_tempbuff = p_extcsd;

    DBGENTER1("cardno(%d)", cardno);
    if
    (
        (MMC_HIGH_SPEED_MULTIMEDIA_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    &&  (MMC_HIGH_CAPACITY_MMC_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno) || (NULL == p_extcsd))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*SELECT CARD,IF NOT SELCTED ALREADY */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /*SET BLOCK SIZE FOR CONTROLLER.*/
    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) 512 << MMC_SHIFT16;

    if (512 != g_mmc_system_context[mmc_device_id].current_card.blk_length)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) 512;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.blk_length = 512;
    }

    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) 9 << MMC_SHIFT4;

    if (9 != g_mmc_system_context[mmc_device_id].current_card.blk_length)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) 512;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.blk_length = 9;
    }
    #endif
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = 512;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 512;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    /* ENABLE DATA PATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCREADDIR &~MMCSTREAMMODE | MMCDATAPATHENABLE;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_extcsd;

    /*SEND CMD8 MMC_HS_SEND_EXT_CSD with argument as 0*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSENDEXTCSD |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCHSSENDEXTCSD, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCRXOVERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATABLOCKEND | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXFIFOHALFFULL)
        {
            for (count = 0; count < MMC_HALFFIFO; count++)
            {
                *(p_tempbuff + count) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
            }

            p_tempbuff += MMC_HALFFIFO;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        error = MMC_DATA_CRC_FAIL;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXOVERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCRXOVERRUN;
        error = MMC_RX_OVERRUN;
        DBGEXIT0(error);
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        error = MMC_START_BIT_ERR;
        DBGEXIT0(error);
        return(error);
    }

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    DBGEXIT0(error);
    return(error);
}

/********************************************************************************/
/*       NAME : MMC_SetHighSpeedModeTiming()                                    */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables high speed mode timing bit for */
/*              the  HS MMC card.                                               */
/* PARAMETERS :                                                                 */
/*         IN : t_uint8 cardno: cardno                                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : t_mmc_state hsmodetiming: to be enabled or to be disabled.      */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PUBLIC t_mmc_error MMC_SetHighSpeedModeTiming
(
    IN t_uint8          cardno,
    IN t_mmc_state      hsmodetiming,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint8     cardstate;

    DBGENTER1("cardno(%d)", cardno);
    if
    (
        (MMC_HIGH_SPEED_MULTIMEDIA_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    &&  (MMC_HIGH_CAPACITY_MMC_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*SELECT CARD,IF NOT SELCTED ALREADY */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /*SEND CMD6 MMCHSSWITCH with argument desired.*/
    if (MMC_ENABLE == hsmodetiming)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x03B90100;
    }
    else
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x03B90000;
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCHSSWITCH, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    DBGEXIT(error);
    return(error);
}

/********************************************************************************/
/*       NAME : MMC_SetPowerClass()                                             */
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets power class of the HS-Speed MMC card.         */
/* PARAMETERS :                                                                 */
/*         IN : t_uint8 cardno: cardno                                          */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used     */
/*        OUT : t_uint8 powerclass : value for power class                      */
/*                                                                              */
/*     RETURN : t_mmc_error : MMC error code                                    */

/********************************************************************************/
PUBLIC t_mmc_error MMC_SetPowerClass(IN t_uint8 cardno, IN t_uint8 powerclass, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint8     cardstate;

    DBGENTER1("cardno(%d)", cardno);
    if
    (
        (MMC_HIGH_SPEED_MULTIMEDIA_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    &&  (MMC_HIGH_CAPACITY_MMC_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error);
        return(error);
    }

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno) || (powerclass > 10))
    {
        error = MMC_INVALID_PARAMETER;
        DBGEXIT0(error);
        return(error);
    }

    /*SELECT CARD,IF NOT SELCTED ALREADY */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            DBGEXIT0(error);
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /*SEND CMD6 MMCHSSWITCH with argument desired.*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x03BB0000 | ((t_uint32) powerclass << MMC_SHIFT8);
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCHSSWITCH, mmc_device_id);
    if (MMC_OK != error)
    {
        DBGEXIT0(error);
        return(error);
    }

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    DBGEXIT(error);
    return(error);
}

PRIVATE void mmc_ResetGV(IN t_mmc_device_id mmc_device_id)
{
    g_mmc_system_context[mmc_device_id].current_card.selected_card = 0;
    g_mmc_system_context[mmc_device_id].current_card.cardtype = MMC_MULTIMEDIA_CARD;
    g_mmc_system_context[mmc_device_id].current_card.blk_length = 0;
    g_mmc_system_context[mmc_device_id].current_card.blk_length = 0;
    g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;
    g_mmc_system_context[mmc_device_id].current_card.wide_mode = MMC_DISABLE;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.device_mode = MMC_POLLING_MODE;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.mmc_event = 0;
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.transfer_error = MMC_CMD_CRC_FAIL;
    g_mmc_system_context[mmc_device_id].current_card.sdio_function = 0;

    g_mmc_system_context[mmc_device_id].no_of_cards = 0;
}

PRIVATE t_mmc_error mmc_SDEnableWideBus_onebit
(
    IN t_uint8          cardno,
    IN t_mmc_state      widen,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    /*IF REQUESTED CARD SUPPORTS 1 BIT MODE OPERATION */
    /*SELECT CARD IF ALREADY NOT SELECTED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /*SEND CMD55 APP_CMD with argument as card's RCA.*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
            (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
        MMC_SHIFT16;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    /*SEND ACMD6 APP_CMD with argument as 0 for wide bus mode*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x00;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_SD_SET_BUSWIDTH |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCAPP_SD_SET_BUSWIDTH, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    /*SET WIDE BUS OPERATION FOR THE CONTROLLER */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock &= ~MMC_CLOCK_WIDEBUS_MASK;
    g_mmc_system_context[mmc_device_id].current_card.wide_mode = MMC_DISABLE;
    return(error);
}

PRIVATE t_mmc_error mmc_SDEnableWidebus_multibit
(
    IN t_uint8          cardno,
    IN t_mmc_state      widen,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;

    /*SELECT CARD IF ALREADY NOT SELECTED */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /*SEND CMD55 APP_CMD with argument as card's RCA.*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
            (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
        MMC_SHIFT16;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_CMD | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCAPP_CMD, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    /*SEND ACMD6 APP_CMD with argument as 2 for wide bus mode*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x02;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCAPP_SD_SET_BUSWIDTH |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCAPP_SD_SET_BUSWIDTH, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    /*SET WIDE BUS OPERATION FOR THE CONTROLLER */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock |= (MMC_CLOCK_WIDEBUS_MASK & ((t_uint32) MMC_4_BIT_WIDE << MMC_SHIFT11));
    g_mmc_system_context[mmc_device_id].current_card.wide_mode = MMC_ENABLE;
    return(error);
}

PRIVATE t_mmc_error mmc_SDEnWideBus(IN t_uint8 cardno, IN t_mmc_state widen, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    scr[16];

    /* REMOVE WARNING */
    scr[1] = 0;

    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_response0 & MMC_CARDLOCKED)
    {
        error = MMC_LOCK_UNLOCK_FAILED;
        return(error);
    }

    /*GET SCR REGISTER */
    error = mmc_FINDSCR(g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca, scr, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    /*IF WIDE BUS OPERATION TO BE ENABLED*/
    if (MMC_ENABLE == widen)
    {
        /*IF REQUESTED CARD SUPPORTS WIDE BUS OPERATION */
        if ((scr[1] & MMC_WIDEBUSSUPPORT) != MMC_ALLZERO)
        {
            error = mmc_SDEnableWidebus_multibit(cardno, widen, mmc_device_id);
            if (MMC_OK != error)
            {
                return(error);
            }
        }
        else
        {
            error = MMC_REQUEST_NOT_APPLICABLE;
            return(error);
        }
    }   /*IF WIDE BUS OPERATION TO BE DISABLED*/
    else
    {
        if ((scr[1] & MMC_SINGLEBUSSUPPORT) != MMC_ALLZERO)
        {
            error = mmc_SDEnableWideBus_onebit(cardno, widen, mmc_device_id);
            if (MMC_OK != error)
            {
                return(error);
            }
        }
        else
        {
            error = MMC_REQUEST_NOT_APPLICABLE;
            return(error);
        }
    }

    return(error);
}

PRIVATE t_mmc_error mmc_HSEnWideBus(t_uint8 cardno, t_mmc_wide_mode wide_mode, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint8     cardstate;

    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /*SEND CMD6 MMCHSSWITCH with argument desired.(acess bits set to 11, index set to B7 (bus_width), value set to wide_mode)*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x03B70000 | ((t_uint32) wide_mode << MMC_SHIFT8);
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH | MMCRESPEXPECTED | MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCHSSWITCH, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;
    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    return(error);
}

#if defined SDIO_SUPPORT
PRIVATE t_mmc_error mmc_SDIOEnWideBus(IN t_uint8 cardno, IN t_mmc_wide_mode wide_mode, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint8     io_direct_data;
    t_uint32    argument;

    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    argument = (t_uint32) 0x0 | ((t_uint32) 0x08 << MMC_SHIFT9) |               /* Address of Card Capability register of CCCR */
    ((t_uint32) 0x0 << MMC_SHIFT28) |                                           /* Select function 0, CIA */
    ((t_uint32) 0x0 << MMC_SHIFT27) |                                           /* No read after write */
    ((t_uint32) MMC_SDIO_READ << MMC_SHIFT31);

    /* SEND IO DIRECT COMMAND */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    if ((NULL != (io_direct_data & MASK_BIT6)) && (NULL == (io_direct_data & MASK_BIT7)))
    {
        return(MMC_UNSUPPORTED_FEATURE);
    }

    if (MMC_1_BIT_WIDE == wide_mode)
    {
        io_direct_data = 0x0;
    }
    else if (MMC_4_BIT_WIDE == wide_mode)
    {
        io_direct_data = 0x2;
    }
    else
    {
        return(MMC_UNSUPPORTED_FEATURE);
    }

    argument = (t_uint32) io_direct_data | ((t_uint32) 0x07 << MMC_SHIFT9) |    /* Address of Bus Interface Control register of CCCR */
    ((t_uint32) 0x0 << MMC_SHIFT28) |   /* Select function 0, CIA */
    ((t_uint32) 0x0 << MMC_SHIFT27) |   /* No read after write */
    ((t_uint32) MMC_SDIO_WRITE << MMC_SHIFT31);

    /* SEND IO DIRECT COMMAND */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = argument;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCIO_RW_DIRECT |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp5Error(MMCIO_RW_DIRECT, &io_direct_data, mmc_device_id);

    return(error);
}
#endif /* defined SDIO_SUPPORT */
PRIVATE t_mmc_error mmc_testpin_write_fifo(t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = 0x0000AA55;
    switch (mmc_device_id)
    {
        case MMC_DEVICE_ID_0:
            * (volatile t_uint32 *) (0x80126084) = 0x0000AA55;
            break;

        case MMC_DEVICE_ID_1:
            * (volatile t_uint32 *) (0x80118084) = 0x0000AA55;
            break;

        case MMC_DEVICE_ID_2:
            * (volatile t_uint32 *) (0x80005084) = 0x0000AA55;
            break;

        case MMC_DEVICE_ID_3:
            * (volatile t_uint32 *) (0x80119084) = 0x0000AA55;
            break;

        case MMC_DEVICE_ID_4:
            * (volatile t_uint32 *) (0x80114084) = 0x0000AA55;
            break;

        case MMC_DEVICE_ID_5:
            * (volatile t_uint32 *) (0x80008084) = 0x0000AA55;
            break;
    }
    return(error);

}
PRIVATE t_mmc_error mmc_TestFunctionalPins_write
(
    IN t_uint8          cardno,
    IN t_mmc_wide_mode  mode,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;


    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |=
        ((MMCDATAPATHENABLE | MMCSTREAMMODE) & ~(MMCREADDIR)) |
        (0x0 << MMC_SHIFT16);            /* MMCSTREAMMODE &~MMCREADDIR | MMCDATAPATHENABLE; */
    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |=
        ((MMCDATAPATHENABLE | MMCSTREAMMODE) & ~(MMCREADDIR)) |
        (0x0 << MMC_SHIFT4);            /* MMCSTREAMMODE &~MMCREADDIR | MMCDATAPATHENABLE; */
    #endif



    /*SEND CMD 19 MMCHS_BUSTEST_WRITE */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHS_BUSTEST_WRITE |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCHS_BUSTEST_WRITE, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = 0;




    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCDATATIMEOUT | MMCDATAEND | MMCTXUNDERRUN)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCTXFIFOEMPTY)
        {
            /*  Added to add 8-bit support*/
            if (MMC_8_BIT_WIDE == mode)
            {
                error = mmc_testpin_write_fifo(mmc_device_id);
            }

            else {
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo = 0x0000005A;
            switch (mmc_device_id)
            {
                case MMC_DEVICE_ID_0:
                    * (volatile t_uint32 *) (0x80126084) = 0x0000005A;
                    break;

                case MMC_DEVICE_ID_1:
                    * (volatile t_uint32 *) (0x80118084) = 0x0000005A;
                    break;

                case MMC_DEVICE_ID_2:
                    * (volatile t_uint32 *) (0x80005084) = 0x0000005A;
                    break;

                case MMC_DEVICE_ID_3:
                    * (volatile t_uint32 *) (0x80119084) = 0x0000005A;
                    break;

                case MMC_DEVICE_ID_4:
                    * (volatile t_uint32 *) (0x80114084) = 0x0000005A;
                    break;

                case MMC_DEVICE_ID_5:
                    * (volatile t_uint32 *) (0x80008084) = 0x0000005A;
                    break;
            }

            }
        }
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;
    return(error);
}

PRIVATE t_mmc_error mmc_TestFunctionalPins_read
(
    IN t_uint8          cardno,
    IN t_mmc_wide_mode  mode,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_mmc_error error = MMC_OK;
    t_uint32    reversed_pattern[2];
    t_uint32    i = 0;
    t_uint32    timeout;


    /*RECEIVE 8 BYTES */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMC_ALLZERO;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 8;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCREADDIR | MMCDATAPATHENABLE | (8 << MMC_SHIFT16);
    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCREADDIR | MMCDATAPATHENABLE | (0x3 << MMC_SHIFT4);
    #endif

    /*SEND CMD 14 MMCHS_BUSTEST_READ */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHS_BUSTEST_READ |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCHS_BUSTEST_READ, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    timeout = 10000;
    while
    (
        (!(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCRXFIFOHALFFULL | MMCDATATIMEOUT | MMCDATAEND | MMCDATACRCFAIL)
        )) && (timeout)  
   )
    {
        timeout--;
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        return(error);
    }

    for (i = 0; i < 2; i++)
    {
        reversed_pattern[i] = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    if (MMC_1_BIT_WIDE == mode)
    {
        if (!((reversed_pattern[0] & 0x000000C0) == 0x00000080))
        {
            error = MMC_GENERAL_UNKNOWN_ERROR;
        }
    }
    else if (MMC_4_BIT_WIDE == mode)
    {
        if (!((reversed_pattern[0] & 0x000000FF) == 0x000000A5))
        {
            error = MMC_GENERAL_UNKNOWN_ERROR;
        }
    }
    else if (MMC_8_BIT_WIDE == mode)
    {
        if (!((reversed_pattern[0] & 0x0000FFFF) == 0x000055AA))
        {
            error = MMC_GENERAL_UNKNOWN_ERROR;
        }
    }

    return(error);
}

PRIVATE t_mmc_error mmc_TestFunctionalPins(IN t_uint8 cardno, IN t_mmc_wide_mode mode, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;

    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /*SEND FOUR BYTES */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = MMC_ALLZERO;
    if (MMC_8_BIT_WIDE == mode)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock |= 0x1000;
    }
    else
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock |= 0x0800;
    }
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 2;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = 0; /* MMC_DATATIMEOUT; */

    error = mmc_TestFunctionalPins_write(cardno, mode, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    error = mmc_TestFunctionalPins_read(cardno, mode, mmc_device_id);
    if (MMC_OK != error)
    {
        return(error);
    }

    return(error);
}

/*
 * MMC_getCID : returns the CID of the mmc deice.
 * The 16 Bytes data is read in 4 unsigned integers, and because of the BIG -> LITTLE endian transfer,
 * the each of the 4 uints are rotated, i.e. first byte is the forth one and vice versa.
 */
PUBLIC t_mmc_error MMC_getCID(IN t_uint8 cardno, OUT t_uint8 *p_cid, IN t_uint32 len, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    int i = 0, j = 0;
    t_uint32 *temp_;
    void *cast_cid;
    cast_cid = (void *) p_cid;

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno)) {
        error = MMC_INVALID_PARAMETER;
        return(error);
    }

    temp_ = (t_uint32 *)cast_cid;
    for(i = 4 - 1; i >= 0; i--)
    {
        temp_[j] = g_mmc_system_context[mmc_device_id].card_array[cardno - 1].cid[i];
        j++;
    }

    return error;
}

/**********************
 *    RPMB Support    *
 **********************/

/* RPMB Data packet
 *
 * Packet should be packet as you see it now
 *
|-------------|----------|-----------|----------|---------------|---------|-------------|---------|----------|
| Stuff Bytes | Key/MAC  |    Data   |   Nonce  | Write Counter | Address | Block Count | Result  | Req/Resp |
|  196 Bytes  | 32 Bytes | 256 Bytes | 16 Bytes |    4 Bytes    | 2 Bytes |   2 Bytes   | 2 Bytes | 2 Bytes  |
|-------------|----------|-----------|----------|---------------|---------|-------------|---------|----------|
|   511:316   | 315:284  |   283:28  |   27:12  |     11:8      |   7:6   |     5:4     |   3:2   |   1:0    |
|-------------|----------|-----------|----------|---------------|---------|-------------|---------|----------|
 *
 * MAC is calculated on bytes 283:0
 * Nonce is random data for improved security
 * Write counter must be read from the card before MAC calculation
 */

/****************************************************************************/
/*       NAME : MMC_SetdeviceRPMBMode                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Activate or deactivate RPMB partition                       */
/*                                                                          */
/* PARAMETERS :                                                             */
/*       IN : t_uint8 cardno: card to access                                */
/*       IN : t_mmc_state mode: MMC_ENABLE to activate partition            */
/*                              MMC_DISABLE to deactivate partition         */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                                                          */
/*   RETURN : t_emmc_error                                                  */
/****************************************************************************/
PUBLIC t_emmc_error MMC_SetdeviceRPMBMode
(
    IN t_uint8          cardno,
    IN t_mmc_state      mode,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_emmc_error error = {0};
    t_uint8      cardstate;

    if ((MMC_HIGH_SPEED_MULTIMEDIA_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type) &&
       (MMC_HIGH_CAPACITY_MMC_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type))
    {
        error.mmc_error = MMC_REQUEST_NOT_APPLICABLE;
        return(error);
    }

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno)) {
        error.mmc_error = MMC_INVALID_PARAMETER;
        return(error);
    }

    /* SELECT CARD, IF NOT SELCTED ALREADY */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card) {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca << MMC_SHIFT16;

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

        error.mmc_error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);

        if (MMC_OK != error.mmc_error) {
            return(error);
        } else {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

//    error.mmc_error = MMC_EnableEraseGroup(mmc_device_id);
//    if (MMC_OK != error.mmc_error) {
//        return(error);
//    }

    /*SEND CMD6 MMCHSSWITCH with argument desired.*/
    if (MMC_ENABLE == mode) {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = ENABLE_RPMB;
    } else {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = DISABLE_RPMB;
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH | MMCRESPEXPECTED | MMCCMDPATHENABLE;

    error.mmc_error = mmc_CmdResp1Error(MMCHSSWITCH, mmc_device_id);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    error.mmc_error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    while ((MMC_OK == error.mmc_error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error.mmc_error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }

    return(error);
}

/****************************************************************************/
/*       NAME : MMC_RPMB_ProgramAuthenticationKey                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Program authentication key used for SHA256-HMAC calculation */
/*                                                                          */
/* PARAMETERS :                                                             */
/*       IN : t_uint8  cardno: card to access                               */
/*       IN : t_uint8* auth_key: 32 bytes authentication key                */
/*       IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                                                          */
/*   RETURN : t_emmc_error                                                  */
/****************************************************************************/
PUBLIC t_emmc_error MMC_RPMB_ProgramAuthenticationKey
(
    IN t_uint8          cardno,
    IN t_uint8         *auth_key,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_emmc_error error       = {0};
    t_RPMB_data  RPMB_packet = {0};
    void *TempData_p         = &RPMB_packet;

    RPMB_packet.request_response[0] = (t_uint8)(AUTH_KEY_PRG_REQ>>8);
    RPMB_packet.request_response[1] = (t_uint8)(AUTH_KEY_PRG_REQ);
    memcpy(RPMB_packet.key_MAC, auth_key, RPMB_KEY_MAC_SIZE);

    error.mmc_error = MMC_ReliableWriteMultiBlocks( cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, 1, mmc_device_id);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    /* Check RPMB return value */
    memset((t_uint8*)(&RPMB_packet), 0x00, sizeof(t_RPMB_data));
    RPMB_packet.request_response[0] = (t_uint8)(RES_READ_REQ>>8);
    RPMB_packet.request_response[1] = (t_uint8)(RES_READ_REQ);

    error.mmc_error = mmc_WriteMultiBlocks( cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, 1, mmc_device_id);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    error.mmc_error = mmc_ReadBlocks(cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, 1, mmc_device_id, TRUE);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    error.rpmb_error = (t_uint16)(RPMB_packet.result[0])<<8 | (t_uint16)(RPMB_packet.result[1]);

    if (error.rpmb_error >= WRITE_CNT_EXPIRED) {
        /* Write counter expired */
        error.rpmb_error -= WRITE_CNT_EXPIRED;
    }

    if (error.rpmb_error != OP_OK) {
        return(error);
    }

    return(error);
}

/*****************************************************************************/
/*       NAME : MMC_RPMB_ReadCounterValue                                    */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: Returns write counter value                                  */
/*              The counter value is used for MAC calculations               */
/* PARAMETERS :                                                              */
/*       IN  : t_uint8   cardno: card to access                              */
/*       OUT : t_uint32* counter_val: Write counter value                    */
/*       IN  : t_uint8*  nonce: 16 bytes random data for improved security   */
/*       IN  : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used */
/*                                                                           */
/*   RETURN : t_emmc_error                                                   */
/*****************************************************************************/
PUBLIC t_emmc_error MMC_RPMB_ReadCounterValue
(
    IN  t_uint8          cardno,
    OUT t_uint32        *counter_val,
    IN  t_uint8         *nonce,
    IN  t_mmc_device_id  mmc_device_id
)
{
    t_emmc_error error      = {0};
    t_RPMB_data RPMB_packet = {0};
    void *TempData_p        = &RPMB_packet;

    RPMB_packet.request_response[0] = (t_uint8)(READ_WRITE_CNT_REQ>>8);
    RPMB_packet.request_response[1] = (t_uint8)(READ_WRITE_CNT_REQ);

    if (nonce != NULL) {
        memcpy(RPMB_packet.nonce, nonce, RPMB_NONCE_SIZE);
    }

    unsigned char *pb = (unsigned char *)TempData_p;

    error.mmc_error = MMC_WriteBlocks( cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, 1, mmc_device_id, TRUE);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    error.mmc_error = mmc_ReadBlocks(cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, 1, mmc_device_id, TRUE);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    error.rpmb_error = (t_uint16)(RPMB_packet.result[0])<<8 | (t_uint16)(RPMB_packet.result[1]);
    if (error.rpmb_error >= WRITE_CNT_EXPIRED) {
        /* Write counter expired */
        error.rpmb_error -= WRITE_CNT_EXPIRED;
    }

    if (error.rpmb_error != OP_OK) {
        return(error);
    }

    *counter_val = (t_uint32)(RPMB_packet.write_counter[0])<<24 | (t_uint32)(RPMB_packet.write_counter[1])<<16 | (t_uint32)(RPMB_packet.write_counter[2])<<8 | (t_uint32)(RPMB_packet.write_counter[3]);

    return(error);
}

/******************************************************************************/
/*       NAME : MMC_RPMB_WriteMultiBlocks                                     */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: Write data into RPMB area                                     */
/*              Data should be formated in t_RPMB_data chunks, with           */
/*              calculated valid MAC and all parameters set                   */
/* PARAMETERS :                                                               */
/*       IN  : t_uint8      cardno: card to access                            */
/*       OUT : t_RPMB_data* p_writebuff: Write data buffer                    */
/*       IN  : t_uint32     no_of_blocks: Number of blocks to write           */
/*       IN  : t_mmc_device_id mmc_device_id: Device Id for MMC Instance Used */
/*                                                                            */
/*   RETURN : t_emmc_error                                                    */
/******************************************************************************/
PUBLIC t_emmc_error MMC_RPMB_WriteMultiBlocks
(
    IN t_uint8          cardno,
    IN t_RPMB_data     *p_writebuff,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_emmc_error error       = {0};
    t_RPMB_data  RPMB_packet = {0};
    void *TempData_p         = p_writebuff;

    if ((p_writebuff == NULL) || (no_of_blocks == 0)) {
        error.mmc_error = MMC_INVALID_PARAMETER;
        return(error);
    }

    error.mmc_error = MMC_ReliableWriteMultiBlocks( cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, no_of_blocks, mmc_device_id);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    TempData_p = &RPMB_packet;

    /* Check RPMB return value */
    RPMB_packet.request_response[0] = (t_uint8)(RES_READ_REQ>>8);
    RPMB_packet.request_response[1] = (t_uint8)(RES_READ_REQ);

    error.mmc_error = mmc_WriteMultiBlocks( cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, 1, mmc_device_id);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    error.mmc_error = mmc_ReadBlocks(cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, 1, mmc_device_id, TRUE);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    error.rpmb_error = (t_uint16)(RPMB_packet.result[0])<<8 | (t_uint16)(RPMB_packet.result[1]);
    if (error.rpmb_error >= WRITE_CNT_EXPIRED) {
        /* Write counter expired */
        error.rpmb_error -= WRITE_CNT_EXPIRED;
    }

    if (error.rpmb_error != OP_OK) {
        return(error);
    }

    return(error);
}

/******************************************************************************/
/*       NAME : MMC_RPMB_ReadMultiBlocks                                      */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: Read data from RPMB area                                      */
/*              Data will be formated in t_RPMB_data chunks                   */
/* PARAMETERS :                                                               */
/*       IN  : t_uint8      cardno: card to access                            */
/*       IN  : t_uint16     addr: Address to read from                        */
/*       OUT : t_RPMB_data* p_readbuff: Read data buffer                      */
/*       IN  : t_uint32     no_of_blocks: Number of blocks to read            */
/*       IN  : t_uint8*     nonce: 16 bytes random data for improved security */
/*       IN  : t_mmc_device_id mmc_device_id: Device Id for MMC Instance Used */
/*                                                                            */
/*   RETURN : t_emmc_error                                                    */
/******************************************************************************/
PUBLIC t_emmc_error MMC_RPMB_ReadMultiBlocks
(
    IN  t_uint8          cardno,
    IN  t_uint16         addr,
    OUT t_RPMB_data     *p_readbuff,
    IN  t_uint32         no_of_blocks,
    IN  t_uint8         *nonce,
    IN  t_mmc_device_id  mmc_device_id
)
{
    t_emmc_error error       = {0};
    t_RPMB_data  RPMB_packet = {0};
    t_uint32     Counter     = 0;
    void        *TempData_p  = &RPMB_packet;

    if ((p_readbuff == NULL) || (no_of_blocks == 0)) {
        error.mmc_error = MMC_INVALID_PARAMETER;
        return(error);
    }

    RPMB_packet.request_response[0] = (t_uint8)(AUTH_DATA_READ_REQ>>8);
    RPMB_packet.request_response[1] = (t_uint8)(AUTH_DATA_READ_REQ);

    RPMB_packet.address[0] = (t_uint8)(addr>>8);
    RPMB_packet.address[1] = (t_uint8)(addr);

    if (nonce != NULL) {
        memcpy(RPMB_packet.nonce, nonce, RPMB_NONCE_SIZE);
    }

    error.mmc_error = MMC_WriteBlocks( cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, 1, mmc_device_id, TRUE);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    TempData_p = p_readbuff;

    error.mmc_error = mmc_ReadBlocks(cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, no_of_blocks, mmc_device_id, TRUE);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    /*
     * Check result values from all blocks
     */
    do {
        error.rpmb_error = (t_uint16)((p_readbuff + Counter)->result[0])<<8 | (t_uint16)((p_readbuff + Counter)->result[1]);
        if ((error.rpmb_error != OP_OK) && (error.rpmb_error != WRITE_CNT_EXPIRED)) {
            return(error);
        }
        Counter++;
    } while (Counter < no_of_blocks);

    return(error);
}

/******************************************************************************/
/*       NAME : MMC_RPMB_Raw_Write                                            */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: Transfer write packet to eMMC card                            */
/*                                                                            */
/* PARAMETERS :                                                               */
/*       IN  : t_uint8      cardno: card to access                            */
/*       OUT : t_RPMB_data* p_writebuff: Write data buffer                    */
/*       IN  : t_uint32     no_of_blocks: Number of blocks to write           */
/*       IN  : t_mmc_device_id mmc_device_id: Device Id for MMC Instance Used */
/*                                                                            */
/*   RETURN : t_emmc_error                                                    */
/******************************************************************************/
PUBLIC t_emmc_error MMC_RPMB_Raw_Write
(
    IN t_uint8          cardno,
    IN t_uint8         *p_writebuff,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
)
{
    t_emmc_error error = {0};
    void *TempData_p   = p_writebuff;

    if ((p_writebuff == NULL) || (no_of_blocks == 0)) {
        error.mmc_error = MMC_INVALID_PARAMETER;
        return(error);
    }

    if ((uint32)p_writebuff & 3) {
        error.mmc_error = MMC_INVALID_PARAMETER;
        return(error);
    }

    /* Check if reliable write is needed */
    if (do_reliable_write(p_writebuff)) {
        error.mmc_error = MMC_ReliableWriteMultiBlocks( cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, no_of_blocks, mmc_device_id);
    }
    else {
        error.mmc_error = MMC_WriteBlocks( cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, no_of_blocks, mmc_device_id, TRUE);
    }

    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    return(error);
}

/******************************************************************************/
/*       NAME : MMC_RPMB_Raw_Read                                             */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: Receive read packet from eMMC card                            */
/*                                                                            */
/* PARAMETERS :                                                               */
/*       IN  : t_uint8      cardno: card to access                            */
/*       OUT : t_RPMB_data* p_readbuff: Read data buffer                      */
/*       IN  : t_uint32     no_of_blocks: Number of blocks to read            */
/*       IN  : t_mmc_device_id mmc_device_id: Device Id for MMC Instance Used */
/*                                                                            */
/*   RETURN : t_emmc_error                                                    */
/******************************************************************************/
PUBLIC t_emmc_error MMC_RPMB_Raw_Read
(
    IN  t_uint8          cardno,
    OUT t_uint8         *p_readbuff,
    IN  t_uint32         no_of_blocks,
    IN  t_mmc_device_id  mmc_device_id
)
{
    t_emmc_error error = {0};
    void *TempData_p   = p_readbuff;

    if ((p_readbuff == NULL) || (no_of_blocks == 0)) {
        error.mmc_error = MMC_INVALID_PARAMETER;
        return(error);
    }

    if ((uint32)p_readbuff & 3) {
        error.mmc_error = MMC_INVALID_PARAMETER;
        return(error);
    }

    error.mmc_error = mmc_ReadBlocks(cardno, 0, (t_uint32*)TempData_p, MMC_SDIO_BLOCK_SIZE, no_of_blocks, mmc_device_id, TRUE);
    if (MMC_OK != error.mmc_error) {
        return(error);
    }

    return(error);
}

/*
 * Utility function to determine if the "write" to the RPMB partition should
 * be "reliable", or not.
 */
PRIVATE int do_reliable_write(t_uint8 *data)
{
    t_uint8 hi = data[REQUEST_HI_BYTE];
    t_uint8 lo = data[REQUEST_LO_BYTE];

    /*
     * Reliable Write used for Writing Authenticated data and Writing Key
     * to RPMB partition.
     * The high byte will be 0x00 for both request types.
     */
    if ((hi == 0x00) &&
        ((lo == AUTH_DATA_WRITE_REQ) ||(lo == AUTH_KEY_PRG_REQ))) {
        return 1;
    } else {
        return 0;
    }
}

/*************************
 *    RPMB Support END   *
 *************************/

/* Used for debugging purposes */
#if defined __MMCI_HCL_PRIVATE_DEBUG
PRIVATE PRIVATE float GetDataTimeoutValue_sel(float timeunit_in_sec, t_uint8 taac)
{
    float   time_value = 1.0f, switch ((taac & 0x78) >> 3)
    {
        case 0x01:
            time_value = 1.0f;
            break;

        case 0x02:
            time_value = 1.2f;
            break;

        case 0x03:
            time_value = 1.3f;
            break;

        case 0x04:
            time_value = 1.5f;
            break;

        case 0x05:
            time_value = 2.0f;
            break;

        case 0x06:
            time_value = 2.5f;
            break;

        case 0x07:
            time_value = 3.0f;
            break;

        case 0x08:
            time_value = 3.5f;
            break;

        case 0x09:
            time_value = 4.0f;
            break;

        case 0x0A:
            time_value = 4.5f;
            break;

        case 0x0B:
            time_value = 5.0f;
            break;

        case 0x0C:
            time_value = 5.5f;
            break;

        case 0x0D:
            time_value = 6.0f;
            break;

        case 0x0E:
            time_value = 7.0f;
            break;

        case 0x0F:
            time_value = 8.0f;
            break;

        default:    /* never reach here. */
            break;
    }

    return(timeunit_in_sec * time_value);
}

PRIVATE t_uint32 GetDataTimeoutValue(t_uint8 cardno, t_bool dir)
{
    t_uint32    count;
    t_uint8     taac, nsac, r2w_factor;
    float       timeunit_in_sec = 1.0f;
    float       taac_in_sec;

    taac = (t_uint8) ((g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[3] & 0x00FF0000) >> 16);
    nsac = (t_uint8) ((g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[3] & 0x0000FF00) >> 8);

    if (0 == (taac & 0x07))
    {
        timeunit_in_sec = (float) 1 / 1000000000;
    }
    else
    {
        for (count = 0; count < ((t_uint32) taac & 0x07); count++)
        {
            timeunit_in_sec = (timeunit_in_sec * 10) / 1000000000;
        }
    }

    taac_in_sec = GetDataTimeoutValue_sel(timeunit_in_sec, taac);
    if (TRUE == dir)
    {
        return((t_uint32) (10 * ((taac_in_sec * clockfreq) + 100 * nsac)));
    }
    else
    {
        r2w_factor = (t_uint8) ((g_mmc_system_context[mmc_device_id].card_array[cardno - 1].csd[0] & 0x0E000000) >> 26);
        return((t_uint32) (10 * r2w_factor * ((taac_in_sec * clockfreq) + 100 * nsac)));
    }
}



PRIVATE t_mmc_error mmc_SetdeviceBootMode
(
    IN t_uint8          cardno,
    IN t_mmc_state      bootmode,
    IN t_mmc_device_id  mmc_device_id
)

{

    t_mmc_error error = MMC_OK;
    t_uint8     cardstate;

    if
    (
        (MMC_HIGH_SPEED_MULTIMEDIA_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    &&  (MMC_HIGH_CAPACITY_MMC_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        return(error);

    }

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno))
    {
        error = MMC_INVALID_PARAMETER;
        return(error);
    }

    /*SELECT CARD,IF NOT SELCTED ALREADY */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;

        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;

        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);

        if (MMC_OK != error)
        {
            return(error);
        }

        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }

    }
 

    /*SEND CMD6 MMCHSSWITCH with argument desired.*/

    if (MMC_ENABLE == bootmode)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x03B34900;
    }

    else
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0x03B30000;
    }

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSWITCH | MMCRESPEXPECTED | MMCCMDPATHENABLE;

    error = mmc_CmdResp1Error(MMCHSSWITCH, mmc_device_id);

    if (MMC_OK != error)
    {
        return(error);
    }

 

    /*clear all the static status flags*/

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);

    while ((MMC_OK == error) && ((MMC_CARDPROGRAMMING == cardstate) || (MMC_CARDRECEIVING == cardstate)))
    {
        error = mmc_IsCardProgramming(cardno, &cardstate, mmc_device_id);
    }
    return(error);

}


PRIVATE t_mmc_error mmc_SetBOOTMode_CMD_SDI(t_mmc_device_id mmc_device_id)
{
    t_mmc_error         error = MMC_OK;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_power = MMC_POWERON  ;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clock = MMC_CLKDIVINIT | MMC_CLKENABLE | MMC_HWFC_EN | MMC_NEGEDGE;

    MMC_SET_BOOTMODE(g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl, (t_uint32) 1);/*Set this bit to enable boot ack*/

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0xFFFFFFFA;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCGO_IDLE_STATE &~MMCRESPEXPECTED | MMCCMDPATHENABLE;

    error = mmc_CmdError(mmc_device_id);
    if (MMC_OK != error)
    {
      return(error);
    }
    return(error);
}


PRIVATE t_mmc_error mmc_GetExtCSD(IN t_uint8 cardno, OUT t_uint32 *p_extcsd, IN t_mmc_device_id mmc_device_id)
{
    t_mmc_error error = MMC_OK;
    t_uint32    count;
    t_uint32    *p_tempbuff = p_extcsd;

    if
    (
        (MMC_HIGH_SPEED_MULTIMEDIA_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    &&  (MMC_HIGH_CAPACITY_MMC_CARD != g_mmc_system_context[mmc_device_id].card_array[cardno - 1].card_type)
    )
    {
        error = MMC_REQUEST_NOT_APPLICABLE;
        return(error);
    }

    if ((cardno > g_mmc_system_context[mmc_device_id].no_of_cards) || (0 == cardno) || (NULL == p_extcsd))
    {
        error = MMC_INVALID_PARAMETER;
        
        return(error);
    }

    /*SELECT CARD,IF NOT SELCTED ALREADY */
    if (cardno != g_mmc_system_context[mmc_device_id].current_card.selected_card)
    {
        /*SEND CMD7 MMCSEL_DESEL_CARD */
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument =
                (t_uint32) g_mmc_system_context[mmc_device_id].card_array[cardno - 1].rca <<
            MMC_SHIFT16;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSEL_DESEL_CARD |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSEL_DESEL_CARD, mmc_device_id);
        if (MMC_OK != error)
        {
            
            return(error);
        }
        else
        {
            g_mmc_system_context[mmc_device_id].current_card.selected_card = cardno;
        }
    }

    /*SET BLOCK SIZE FOR CONTROLLER.*/
    //devarsh
    #if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) 512 << MMC_SHIFT16;

    if (g_mmc_system_context[mmc_device_id].current_card.blk_length != 512)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) 512;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
        if (MMC_OK != error)
        {

            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.blk_length = 512;
    }

    #else
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl = (t_uint32) 9 << MMC_SHIFT4;

    if (g_mmc_system_context[mmc_device_id].current_card.blk_length != 9)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = (t_uint32) 512;
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCSET_BLOCKLEN |
            MMCRESPEXPECTED |
            MMCCMDPATHENABLE;
        error = mmc_CmdResp1Error(MMCSET_BLOCKLEN, mmc_device_id);
        if (MMC_OK != error)
        {
            
            return(error);
        }

        g_mmc_system_context[mmc_device_id].current_card.blk_length = 9;
    }
    #endif
    g_mmc_system_context[mmc_device_id].current_card.mmc_context.total_no_of_bytes = 512;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_length = 512;

    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_timer = MMC_DATATIMEOUT;

    /* ENABLE DATA PATH */
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_data_ctrl |= MMCREADDIR &~MMCSTREAMMODE | MMCDATAPATHENABLE;

    g_mmc_system_context[mmc_device_id].current_card.mmc_context.dest_buffer = p_extcsd;

    /*SEND CMD8 MMC_HS_SEND_EXT_CSD with argument as 0*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_argument = 0;
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_command = MMCHSSENDEXTCSD |
        MMCRESPEXPECTED |
        MMCCMDPATHENABLE;
    error = mmc_CmdResp1Error(MMCHSSENDEXTCSD, mmc_device_id);
    if (MMC_OK != error)
    {
        
        return(error);
    }

    g_mmc_system_context[mmc_device_id].current_card.stopcmd_needed = FALSE;

    while
    (
        !(
            g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status &
                (MMCRXOVERRUN | MMCDATACRCFAIL | MMCDATATIMEOUT | MMCDATABLOCKEND | MMCSTARTBITERROR)
        )
    )
    {
        if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXFIFOHALFFULL)
        {
            for (count = 0; count < MMC_HALFFIFO; count++)
            {
                *(p_tempbuff + count) = g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_fifo;
            }

            p_tempbuff += MMC_HALFFIFO;
        }
    }

    if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATATIMEOUT)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATATIMEOUT;
        error = MMC_DATA_TIMEOUT;
        
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCDATACRCFAIL)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCDATACRCFAIL;
        error = MMC_DATA_CRC_FAIL;
        
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCRXOVERRUN)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCRXOVERRUN;
        error = MMC_RX_OVERRUN;
        
        return(error);
    }
    else if (g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_status & MMCSTARTBITERROR)
    {
        g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear |= MMCSTARTBITERROR;
        error = MMC_START_BIT_ERR;
        
        return(error);
    }

    /*clear all the static status flags*/
    g_mmc_system_context[mmc_device_id].p_mmc_register->mmc_clear = MMCCLRSTATICFLAGS;

    
    return(error);
}

#endif /*__MMC_HCL_ENHANCED*/
#endif




