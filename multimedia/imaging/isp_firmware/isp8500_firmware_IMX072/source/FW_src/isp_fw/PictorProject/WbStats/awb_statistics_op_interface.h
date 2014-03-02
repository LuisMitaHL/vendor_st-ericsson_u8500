/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
\file awb_statistics_op_interface.h
\brief The file export all the structures and functions needed by any external module to access white balance stats.
        The module will need to include this file. The file is part of release code.
\ingroup WBStats
*/
#ifndef _AWB_STATISTICS_OP_INTERFACE_H_
#   define _AWB_STATISTICS_OP_INTERFACE_H_

#   include "Platform.h"
#   include "PictorhwReg.h"

#   include "whitebalance_opinterface.h"




#if WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR



/********************    Exported API's  ************************************/
extern float_t              CalculateNormalisedStatistics (
                            uint32_t    u32_Acc,
                            uint32_t    u32_NoOfPixelPerZone,
                            uint16_t    u16_SumOfZoneGains,
                            uint8_t     u8_NoOfZones);

#endif       // WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR

#endif //_AWB_STATISTICS_OP_INTERFACE_H_

