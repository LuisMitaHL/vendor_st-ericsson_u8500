/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file BinningRepair.c
 */
#include "BinningRepair.h"
#include "BinningRepair_ip_interface.h"


BinningRepair_Ctrl_ts   g_BinningRepair_Ctrl =
{
    DEFAULT_BINNING_REPAIR_ENABLE,
    DEFAULT_BINNING_REPAIR_H_JOG_ENABLE,
    DEFAULT_BINNING_REPAIR_V_JOG_ENABLE,
    DEFAULT_BINNING_REPAIR_MODE,
    DEFAULT_BINNING_REPAIR_COEFF_00,
    DEFAULT_BINNING_REPAIR_COEFF_01,
    DEFAULT_BINNING_REPAIR_COEFF_10,
    DEFAULT_BINNING_REPAIR_COEFF_11,
    DEFAULT_BINNING_REPAIR_COEFF_SHIFT,
    DEFAULT_BINNING_REPAIR_FACTOR
};

uint8_t e_Flag_BinningRepairEnable = Flag_e_FALSE;

void
BinningRepair_Update(void)
{
    e_Flag_BinningRepairEnable = Flag_e_FALSE;

    if (BinningRepairMode_e_Auto == g_BinningRepair_Ctrl.e_BinningRepairMode)
    {
        g_BinningRepair_Ctrl.u8_BinningRepair_factor = BinningRepair_GetBinningFactor();

        if (2 == g_BinningRepair_Ctrl.u8_BinningRepair_factor)
        {
            g_BinningRepair_Ctrl.u8_Coeff_00 = 1;
            g_BinningRepair_Ctrl.u8_Coeff_01 = 9;
            g_BinningRepair_Ctrl.u8_Coeff_10 = 7;
            g_BinningRepair_Ctrl.u8_Coeff_11 = 49;
            g_BinningRepair_Ctrl.u8_Coeff_shift = 5;
            e_Flag_BinningRepairEnable = Flag_e_TRUE;
        }
        else if (4 == g_BinningRepair_Ctrl.u8_BinningRepair_factor)
        {
            g_BinningRepair_Ctrl.u8_Coeff_00 = 9;
            g_BinningRepair_Ctrl.u8_Coeff_01 = 39;
            g_BinningRepair_Ctrl.u8_Coeff_10 = 39;
            g_BinningRepair_Ctrl.u8_Coeff_11 = 169;
            g_BinningRepair_Ctrl.u8_Coeff_shift = 7;

            e_Flag_BinningRepairEnable = Flag_e_TRUE;
        }
        else if (1 == g_BinningRepair_Ctrl.u8_BinningRepair_factor)
        {
            // disable IP
            e_Flag_BinningRepairEnable = Flag_e_FALSE;
        }
    }
    else
    {
        e_Flag_BinningRepairEnable = Flag_e_TRUE;
        // use the control page elements to program into hardware block.
        // nothing to be done here
    }
}


void
BinningRepair_Commit(void)
{
    /// soft_reset,br_coef_hjog,br_coef_vjog
    Set_BINNING_REPAIR_CONTROL(
    0,
    g_BinningRepair_Ctrl.e_Flag_H_Jog_Enable,
    g_BinningRepair_Ctrl.e_Flag_V_Jog_Enable);

    /// coeff00
    Set_BINNING_REPAIR_COEF00(g_BinningRepair_Ctrl.u8_Coeff_00);

    /// coeff01
    Set_BINNING_REPAIR_COEF01(g_BinningRepair_Ctrl.u8_Coeff_01);

    /// coeff10
    Set_BINNING_REPAIR_COEF10(g_BinningRepair_Ctrl.u8_Coeff_10);

    /// coeff11
    Set_BINNING_REPAIR_COEF11(g_BinningRepair_Ctrl.u8_Coeff_11);

    /// coeff_shift
    Set_BINNING_REPAIR_COEFF_SFT(g_BinningRepair_Ctrl.u8_Coeff_shift);

    /// enable/disable
    Set_BINNING_REPAIR_ENABLE( (g_BinningRepair_Ctrl.e_Flag_BinningRepairEnable) & e_Flag_BinningRepairEnable);
}

