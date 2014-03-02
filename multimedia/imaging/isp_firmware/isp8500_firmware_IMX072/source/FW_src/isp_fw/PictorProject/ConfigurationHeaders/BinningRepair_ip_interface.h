/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file BinningRepair_ip_interface.h
 */

#ifndef BINNINGREPAIR_IP_INTERFACE_H_
#define BINNINGREPAIR_IP_INTERFACE_H_

#include "BinningRepair_platform_specific.h"
#include "PictorhwReg.h"

#include "lla_abstraction.h"

/// interface to enable/disable the binning repair hardware block
#define Set_BINNING_REPAIR_ENABLE(binning_repair_enable) Set_ISP_BINNING_REPAIR_ISP_BINNING_REPAIR_ENABLE(binning_repair_enable)

/// interface to set control for binning repair ip
#define Set_BINNING_REPAIR_CONTROL(soft_reset,br_coef_hjog,br_coef_vjog) Set_ISP_BINNING_REPAIR_ISP_BINNING_REPAIR_CONTROL(soft_reset,br_coef_hjog,br_coef_vjog)

/// interface to set coefficient 00
#define Set_BINNING_REPAIR_COEF00(br_coef00) Set_ISP_BINNING_REPAIR_ISP_BINNING_REPAIR_COEF00(br_coef00)

/// interface to set coefficient 01
#define Set_BINNING_REPAIR_COEF01(br_coef01) Set_ISP_BINNING_REPAIR_ISP_BINNING_REPAIR_COEF01(br_coef01)

/// interface to set coefficient 10
#define Set_BINNING_REPAIR_COEF10(br_coef10) Set_ISP_BINNING_REPAIR_ISP_BINNING_REPAIR_COEF10(br_coef10)

/// interface to set coefficient 11
#define Set_BINNING_REPAIR_COEF11(br_coef11) Set_ISP_BINNING_REPAIR_ISP_BINNING_REPAIR_COEF11(br_coef11)

/// interface to set coefficient_shift
#define Set_BINNING_REPAIR_COEFF_SFT(br_coef_sft) Set_ISP_BINNING_REPAIR_ISP_BINNING_REPAIR_COEF_SFT(br_coef_sft)

/// interface for knowing the binning factor used in the firmware
/// <TODO:MS>
#define BinningRepair_GetBinningFactor()    LLA_BINNING_FACTOR()

#endif /* BINNINGREPAIR_IP_INTERFACE_H_ */


