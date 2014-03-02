/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_nmf_ndfilter_functions.h"

#include "ite_testenv_utils.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_init.h"
#include "ite_sia_init.h"

//#include "ite_alloc.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_sia_buffer.h"
#include "ite_host2sensor.h"
#include "ite_sensorinfo.h"

//#include "ite_nmf_tuning_functions.h"
#include <cm/inc/cm_macros.h>
#include <test/api/test.h>
#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"
extern ts_sia_usecase   usecase;

/* static functions */
static void             Apply_NDFilter (void);
static void             Remove_NDFilter (void);

/* globals */
t_cmd_list              ite_cmd_list_ndfilter[] =
{
    { "NDFilterhelp", C_ite_dbg_NDFilter_help_cmd, "NDFilterhelp: NDFilterhelp\n" },
    { "TestNDFilterSupport", C_ite_dbg_TestNDFilterSupport_cmd, "TestNDFilterSupport: TestNDFilterSupport \n" },
    { "EnableNDFilter", C_ite_dbg_Enable_Disable_NDFilter_cmd, "EnableNDFilter: EnableNDFilter\n" },
    { "DisableNDFilter", C_ite_dbg_Enable_Disable_NDFilter_cmd, "DisableNDFilter: DisableNDFilter\n" }
};

static t_uint16         u16_NDFilterTransparency = 0;

/* -----------------------------------------------------------------------
   FUNCTION : Apply_NDFilter
   PURPOSE  :
   ------------------------------------------------------------------------ */
static void
Apply_NDFilter(void)
{
    t_uint32    control_coin,
                status_coin;

    LOS_Log("Applying Filter now..\n");

    ITE_writePE(Exposure_DriverControls_e_Flag_NDFilter_Byte0, Flag_e_TRUE);

    //toggle coin
    control_coin = ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
    ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !control_coin);

    do
    {
        control_coin = ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
        status_coin = ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
    } while (control_coin != status_coin);

    if (Flag_e_TRUE == ITE_readPE(FrameParamStatus_u32_Flag_NDFilter_Byte0))
    {
        LOS_Log("Filter Applied!\n");
        LOS_Log(
        "Transparency = %.2f%%\n",
        ( float ) (ITE_readPE(FrameParamStatus_u32_NDFilter_Transparency_x100_Byte0) / 100));
    }
    else
    {
        LOS_Log("Could not apply filter.\n");
    }
}


/* -----------------------------------------------------------------------
   FUNCTION : Remove_NDFilter
   PURPOSE  :
   ------------------------------------------------------------------------ */
static void
Remove_NDFilter(void)
{
    t_uint32    control_coin,
                status_coin;

    LOS_Log("Removing Filter now..\n");

    ITE_writePE(Exposure_DriverControls_e_Flag_NDFilter_Byte0, Flag_e_FALSE);

    //toggle coin
    control_coin = ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
    ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !control_coin);

    do
    {
        control_coin = ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
        status_coin = ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
    } while (control_coin != status_coin);

    if (Flag_e_FALSE == ITE_readPE(FrameParamStatus_u32_Flag_NDFilter_Byte0))
    {
        LOS_Log("Filter Removed!\n");
    }
    else
    {
        LOS_Log("Could not remove filter.\n");
    }
}


/* -----------------------------------------------------------------------
   FUNCTION : Init_NDFilter_ITECmdList
   PURPOSE  :
   ------------------------------------------------------------------------ */
void
Init_NDFilter_ITECmdList(void)
{
    CLI_register_interface(
    "NDFilter_ITE_CMB",
    sizeof(ite_cmd_list_ndfilter) / (sizeof(ite_cmd_list_ndfilter[0])),
    ite_cmd_list_ndfilter,
    1);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_NDFilter_help_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_NDFilter_help_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    UNUSED(ap_args);

    MMTE_TEST_START(
    "NDFilter_help",
    "/ite_nmf/test_results_nreg/NDFilter_tests",
    "Testing for NDFilter_help");

    sprintf(mess, "Testing for NDFilter_help");
    MMTE_TEST_NEXT(mess);

    if (a_nb_args == 1)
    {
        CLI_disp_error("\n");

        CLI_disp_error("TestNDFilterSupport: Checks whether NDFilter_ is suppported by the firmware \n");
        CLI_disp_error("TestNDFilterSupport[Syntax]: TestNDFilterSupport \n");

        CLI_disp_error("EnableNDFilter: Turns NDFilter ON\n");
        CLI_disp_error("EnableNDFilter[Syntax]: EnableNDFilter\n");

        CLI_disp_error("DisableNDFilter: Turns NDFilter OFF\n");
        CLI_disp_error("DisableNDFilter[Syntax]: DisableNDFilter\n");
    }
    else
    {
        MMTE_TEST_COMMENT("Not correct command arguments\n");
        MMTE_TEST_FAILED();

        return (CMD_ERR_ARGS);
    }


    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_TestNDFilterSupport_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_TestNDFilterSupport_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    MMTE_TEST_START(
    "Test NDFilterSupport",
    "/ite_nmf/test_results_nreg/NDFilter_tests",
    "Testing for NDFilterSupport");

    sprintf(mess, "Testing for NDFilterSupport");
    mmte_testNext(mess);

    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("TestNDFilterSupport: Checks whether NDFilter_ is suppported by the firmware \n");
            CLI_disp_error("TestNDFilterSupport[Syntax]: TestNDFilterSupport \n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            u16_NDFilterTransparency = (t_uint16) ITE_readPE(FrameParamStatus_u32_NDFilter_Transparency_x100_Byte0);
            if (u16_NDFilterTransparency > 0)
            {
                MMTE_TEST_COMMENT("*** NDFilter Supported! ***\n");
                MMTE_TEST_PASSED();
                return (CMD_COMPLETE);
            }
            else    /*not supported*/
            {
                MMTE_TEST_COMMENT("*** NDFilter not Supported. ***\n");
                MMTE_TEST_SKIPPED();
                return (CMD_ERR_ARGS);
            }
        }
        else
        {
            MMTE_TEST_COMMENT("Not correct command arguments\n");
            MMTE_TEST_FAILED();

            return (CMD_ERR_ARGS);
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Enable_Disable_NDFilter_cmd
   PURPOSE  : Function to apply as well as remove ND Filter
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Enable_Disable_NDFilter_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint32    control_coin,
                status_coin;
    char        mess[256];

    MMTE_TEST_START(
    "Enable_Disable_NDFilter",
    "/ite_nmf/test_results_nreg/NDFilter_tests",
    "Testing for Enable_Disable_NDFilter");

    sprintf(mess, "Testing for Enable_Disable_NDFilter");
    mmte_testNext(mess);
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("EnableNDFilter: Turns NDFilter ON\n");
            CLI_disp_error("EnableNDFilter[Syntax]: EnableNDFilter\n");

            CLI_disp_error("DisableNDFilter: Turns NDFilter OFF\n");
            CLI_disp_error("DisableNDFilter[Syntax]: DisableNDFilter\n");
        }
    }
    else if (a_nb_args == 1)
    {
        if (u16_NDFilterTransparency > 0)
        {
            control_coin = ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
            status_coin = ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);

            if (control_coin != status_coin)
            {
                MMTE_TEST_COMMENT("Coins not in proper state to execute the command.\n");
                LOS_Log("Status and Control Coins should be equal before executing the command.\n");
                MMTE_TEST_FAILED();
                return (CMD_ERR_GENERAL);
            }


            if (Flag_e_TRUE == ITE_readPE(FrameParamStatus_u32_Flag_NDFilter_Byte0))
            {
                MMTE_TEST_COMMENT("ND Filter already set. So remove it first and then re-apply it!\n");
                Remove_NDFilter();
                Apply_NDFilter();
                MMTE_TEST_PASSED();
                return (CMD_COMPLETE);
            }
            else
            {
                MMTE_TEST_COMMENT("ND Filter not yet applied. So first apply it and then remove it again!\n");
                Apply_NDFilter();
                Remove_NDFilter();
                MMTE_TEST_PASSED();
                return (CMD_COMPLETE);
            }
        }
        else    /*not supported*/
        {
            MMTE_TEST_COMMENT("*** NDFilter not Supported. ***\n");
            MMTE_TEST_SKIPPED();
            return (CMD_ERR_ARGS);
        }
    }
    else
    {
        MMTE_TEST_COMMENT("Not correct command arguments\n");
        MMTE_TEST_FAILED();
        return (CMD_ERR_ARGS);
    }


    return (CMD_COMPLETE);
}

