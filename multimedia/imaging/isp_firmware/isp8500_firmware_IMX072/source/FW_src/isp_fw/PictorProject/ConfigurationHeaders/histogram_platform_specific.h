/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
\file    histogram_platform_specific.h
\brief   Defines device specific values
\ingroup HStats
*/
#ifndef _HISTOGRAM_PLATFORM_SPECiFIC_H_
#   define _HISTOGRAM_PLATFORM_SPECiFIC_H_

#   include "PictorhwReg.h"
#   include "histogram_op_interface.h"

#   define Histogram_GetGStatsAddr()   (0x90000000 + 0x02c804 - 0x4000)
#   define Histogram_GetRStatsAddr()   (0x90000000 + 0x02cc04 - 0x4000)
#   define Histogram_GetBStatsAddr()   (0x90000000 + 0x02d004 - 0x4000)
#   define DEFAULT_HIST_R_ADDR         0
#   define DEFAULT_HIST_G_ADDR         0
#   define DEFAULT_HIST_B_ADDR         0
#   define DEFAULT_HIST_REL_SIZE_X     1.0
#   define DEFAULT_HIST_REL_SIZE_Y     1.0
#   define DEFAULT_HIST_REL_OFFSET_X   0.0
#   define DEFAULT_HIST_REL_OFFSET_Y   0.0
#   define DEFAULT_HIST_PIXEL_SHIFT    0
#   define DEFAULT_HIST_INPUT_SRC      HistInputSrc_e_PostChannelGains
#   define DEFAULT_HIST_IP_ENABLE      Flag_e_TRUE
#   define DEFAULT_HIST_IP_SOFT_ENABLE Flag_e_FALSE
#   define DEFAULT_HIST_CMD            HistCmd_e_STILL
#   define DEFAULT_HIST_COIN_CTRL      Coin_e_Tails
#   define DEFAULT_HIST_MODE           HistogramMode_e_IDLE
#   define DEFAULT_HIST_GEOMETRY_MODE           (StatisticsFov_e_Master_Pipe)

#   define NUM_OF_BITS_PER_PIXEL       8

#   define DEFAULT_HIST_SIZE_X         0
#   define DEFAULT_HIST_SIZE_Y         0
#   define DEFAULT_HIST_OFFEST_X       0
#   define DEFAULT_HIST_OFFEST_Y       0
#   define DEFAULT_HIST_DARKEST_R      0
#   define DEFAULT_HIST_BRIGHTEST_R    0
#   define DEFAULT_HIST_HIGHEST_R      0
#   define DEFAULT_HIST_DARKEST_G      0
#   define DEFAULT_HIST_BRIGHTEST_G    0
#   define DEFAULT_HIST_HIGHEST_G      0
#   define DEFAULT_HIST_DARKEST_B      0
#   define DEFAULT_HIST_BRIGHTEST_B    0
#   define DEFAULT_HIST_HIGHEST_B      0
#   define DEFAULT_HIST_COIN_STATUS    Coin_e_Tails
#   define DEFAULT_HIST_EXPORT_STATUS  ExportStatus_e_COMPLETE
#endif //_HISTOGRAM_PLATFORM_SPECiFIC_H_

