/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file BinningRepair.h
 */
#ifndef BINNINGREPAIR_H_
#   define BINNINGREPAIR_H_

#   include "Platform.h"

/// enum defining modes used in the ip
typedef enum
{
    /// automatic mode, binning factor is calculated by fw (either 2 or 4)
    BinningRepairMode_e_Auto,

    /// custom, coefficient are programmed by host
    BinningRepairMode_e_Custom
} BinningRepairMode_te;

/// struct for ip configuation
typedef struct
{
    /// enable, disable the IP
    uint8_t e_Flag_BinningRepairEnable;

    /// enable, disable the horizontal jog of the filter coefficients
    uint8_t e_Flag_H_Jog_Enable;

    /// enable, disable the vertical jog of the filter coefficients
    uint8_t e_Flag_V_Jog_Enable;

    /// choose among the different modes available for the IP
    uint8_t e_BinningRepairMode;

    /// custom mode, coefficient top left
    uint8_t u8_Coeff_00;

    /// custom mode, coefficient top right
    uint8_t u8_Coeff_01;

    /// custom mode, coefficient bottom left
    uint8_t u8_Coeff_10;

    /// custom mode, coefficient bottom right
    uint8_t u8_Coeff_11;

    /// custom mode, down shift value
    uint8_t u8_Coeff_shift;

    /// binning repair factor, either 2 or 4, 1 indicates binning repair disabled
    /// it is a status page, used internally by firmware
    uint8_t u8_BinningRepair_factor;
} BinningRepair_Ctrl_ts;

/// ctrl page for ip configuration
extern BinningRepair_Ctrl_ts    g_BinningRepair_Ctrl;

/// function that updates the coefficients
extern void                     BinningRepair_Update (void);

/// function that commits the coefficients on to hw
extern void                     BinningRepair_Commit (void);
#endif /* BINNINGREPAIR_H_ */

