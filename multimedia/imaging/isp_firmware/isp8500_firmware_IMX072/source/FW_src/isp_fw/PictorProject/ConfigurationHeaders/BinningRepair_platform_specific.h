/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file BinningRepair_platform_specific.h
 */

#ifndef BINNINGREPAIR_PLATFORM_SPECIFIC_H_
#define BINNINGREPAIR_PLATFORM_SPECIFIC_H_

#include "Platform.h"
#include "BinningRepair.h"

#define DEFAULT_BINNING_REPAIR_ENABLE       Flag_e_FALSE
#define DEFAULT_BINNING_REPAIR_H_JOG_ENABLE Flag_e_FALSE
#define DEFAULT_BINNING_REPAIR_V_JOG_ENABLE Flag_e_FALSE
#define DEFAULT_BINNING_REPAIR_MODE         BinningRepairMode_e_Auto
#define DEFAULT_BINNING_REPAIR_COEFF_00     1
#define DEFAULT_BINNING_REPAIR_COEFF_01     9
#define DEFAULT_BINNING_REPAIR_COEFF_10     7
#define DEFAULT_BINNING_REPAIR_COEFF_11     49
#define DEFAULT_BINNING_REPAIR_COEFF_SHIFT  5
#define DEFAULT_BINNING_REPAIR_FACTOR       1

#endif /* BINNINGREPAIR_PLATFORM_SPECIFIC_H_ */
