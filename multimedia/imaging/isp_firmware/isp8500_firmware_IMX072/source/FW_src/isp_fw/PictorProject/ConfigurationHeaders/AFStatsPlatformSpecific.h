/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \file 		  AFStatsPlatformSpecific.h
 * \brief 		  Header File containing external function declarations and defines for AFStasts Manager and AFStats Compiler
 * \ingroup 	  AFStats
*/
#ifndef AFSTATSPLATFORMSPECIFIC_H_
#   define AFSTATSPLATFORMSPECIFIC_H_

#   include "AFStats.h"
#   include "AFStats_IPInterface.h"

/** \addtogroup AFStats */

/*\@{*/

/// Initialization of the AFStats_Controls Page.
#   define AFSTATS_DEFAULT_CTRL_CORING_VALUE               (0)
#   define AFSTATS_DEFAULT_CTRL_H_RATIO_NUM                (1)
#   define AFSTATS_DEFAULT_CTRL_H_RATIO_DEN                (6)
#   define AFSTATS_DEFAULT_CTRL_V_RATIO_NUM                (1)
#   define AFSTATS_DEFAULT_CTRL_V_RATIO_DEN                (7)
#   define AFSTATS_DEFAULT_CTRL_HOST_ACTIVE_ZONES_COUNTER  (0)
#   define AFSTATS_DEFAULT_CTRL_WINDOWS_SYSTEM             (AFStats_WindowsSystem_e_AF_PRESET_DEFAULT_7_ZONES_SYSTEM)
#   define AFSTATS_DEFAULT_ZONE_CONFIG_COMMAND_COIN        (Coin_e_Heads)
#   define AFSTATS_DEFAULT_ZONE_STATS_REQ_CMD              (AFStats_HostCmd_e_NO_REQ)

/// host specified address for the stats exporting to external memory .
#   define AFSTATS_DEFAULT_HOST_STATS_EXPORT_ADDR          (0x00000000)
#   define AFSTATS_DEFAULT_AUTO_REFRESH                    (Flag_e_TRUE)
#   define AFSTATS_DEFAULT_ABS_SQUARE_ENABLED              (Flag_e_FALSE)
#   define AFSTATS_DEFAULT_REDUCED_ZONE_SETUP              (Flag_e_FALSE)
#   define AFSTATS_DEFAULT_HOST_ZONE_SETUP_IN_PERCENTAGE   (Flag_e_FALSE)
#   define AFSTATS_DEFAULT_STATS_EXPORT_COMMAND_COIN       (Coin_e_Heads)
#   define AFSTATS_DEFAULT_STATS_CANCEL                    (Flag_e_TRUE)

/// Initialization of the AFStats_Status Page.
#   define AFSTATS_DEFAULT_MAX_FOCUS_MEASURE_PER_PIXEL (0)
#   define AFSTATS_DEFAULT_STARTING_AF_ZONE_LINE       (0)
/* [PM]For debugging*/
#if 0
#   define AFSTATS_DEFAULT_ENDING_AF_ZONE_LINE         (0)
#endif
#   define AFSTATS_DEFAULT_WOI_WIDTH                   (0)
#   define AFSTATS_DEFAULT_WOI_HEIGHT                  (0)
#   define AFSTATS_DEFAULT_AF_ZONES_WIDTH              (0)
#   define AFSTATS_DEFAULT_AF_ZONES_HEIGHT             (0)
#   define AFSTATS_DEFAULT_STATUS_CORING_VALUE         (0)
#   define AFSTATS_DEFAULT_STATUS_ACTIVE_ZONES_COUNTER (0)
#   define AFSTATS_DEFAULT_STATUS_H_RATIO_NUM          (1)
#   define AFSTATS_DEFAULT_STATUS_H_RATIO_DEN          (6)
#   define AFSTATS_DEFAULT_STATUS_V_RATIO_NUM          (1)
#   define AFSTATS_DEFAULT_STATUS_V_RATIO_DEN          (7)

/// the no of zone interrupt cycles
#   define AFSTATS_DEFAULT_ZONE_INT_CYCLES (0)

/// host config max zone no , for which the interrupt in a frame comes last .
#   define AFSTATS_DEFAULT_ZONE_INT_LAST_INDEX (AFS_HW_STATS_ZONE_NUMBER - 1)

/// host config min zone no , for which the interrupt in a frame comes first .
#   define AFSTATS_DEFAULT_ZONE_INT_FIRST_INDEX        (0)
#   define AFSTATS_DEFAULT_STATUS_WINDOWS_SYSTEM       (AFStats_WindowsSystem_e_AF_PRESET_DEFAULT_7_ZONES_SYSTEM)
#   define AFSTATS_DEFAULT_ZONE_CONFIG_STATUS_COIN     (Coin_e_Heads)
#   define AFSTATS_DEFAULT_ZONE_STATS_REQ_STATUS_COIN  (AFStats_StatusHostCmd_e_NO_REQ)
#   define AFSTATS_DEFAULT_AF_ERROR                    (AFStats_Error_e_AF_ERROR_OK)
#   define AFSTATS_DEFAULT_FORCED_IRQ                  (Flag_e_FALSE)
#   define AFSTATS_DEFAULT_ABS_SQUARE_ENABLED          (Flag_e_FALSE)
#   define AFSTATS_DEFAULT_STATS_EXPORT_STATUS_COIN    (Coin_e_Heads)
#   define AFSTATS_DEFAULT_STATS_WITH_LENSMOVE_FW_STATUS	(Flag_e_FALSE)
#   define AFSTATS_DEFAULT_FRAME_ID					   (0)


/// Initialization of the AFStats_FocusStats Page.
#   define AFSTATS_DEFAULT_STATS_VALUE_0   (0)
#   define AFSTATS_DEFAULT_STATS_VALUE_1   (0)
#   define AFSTATS_DEFAULT_STATS_VALUE_2   (0)
#   define AFSTATS_DEFAULT_STATS_VALUE_3   (0)
#   define AFSTATS_DEFAULT_STATS_VALUE_4   (0)
#   define AFSTATS_DEFAULT_STATS_VALUE_5   (0)
#   define AFSTATS_DEFAULT_STATS_VALUE_6   (0)
#   define AFSTATS_DEFAULT_STATS_VALUE_7   (0)
#   define AFSTATS_DEFAULT_STATS_VALUE_8   (0)
#   define AFSTATS_DEFAULT_STATS_VALUE_9   (0)


#      define AFSTATS_DEFAULT_HOST_ZONE0_START_X                          0 /*Zone0_StartX*/
#      define AFSTATS_DEFAULT_HOST_ZONE0_START_Y                          0 /*Zone0_StartY*/
#      define AFSTATS_DEFAULT_HOST_ZONE0_WIDTH                            0 /*Zone_Width*/
#      define AFSTATS_DEFAULT_HOST_ZONE0_HEIGHT                           0 /*Zone_Height*/

#      define AFSTATS_DEFAULT_HOST_ZONE0_END_X                            (AFSTATS_DEFAULT_HOST_ZONE0_START_X + Zone_Width)
#      define AFSTATS_DEFAULT_HOST_ZONE0_END_Y                            (AFSTATS_DEFAULT_HOST_ZONE0_START_Y + Zone_Height)
#      define AFSTATS_DEFAULT_HOST_ZONE0_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE0_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE0_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE0_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE0_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE0_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE0_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE0_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE1_START_X                          0 /*(Zone0_StartX - 1 - Zone_Width)*/
#      define AFSTATS_DEFAULT_HOST_ZONE1_START_Y                          (Zone0_StartY)
#      define AFSTATS_DEFAULT_HOST_ZONE1_WIDTH                            Zone_Width
#      define AFSTATS_DEFAULT_HOST_ZONE1_HEIGHT                           Zone_Height

#      define AFSTATS_DEFAULT_HOST_ZONE1_END_X                            (AFSTATS_DEFAULT_HOST_ZONE1_START_X + Zone_Width)
#      define AFSTATS_DEFAULT_HOST_ZONE1_END_Y                            (AFSTATS_DEFAULT_HOST_ZONE1_START_Y + Zone_Height)
#      define AFSTATS_DEFAULT_HOST_ZONE1_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE1_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE1_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE1_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE1_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE1_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE1_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE1_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE2_START_X                          0 /*(Zone0_StartX + 1 + Zone_Width)*/
#      define AFSTATS_DEFAULT_HOST_ZONE2_START_Y                          (Zone0_StartY)
#      define AFSTATS_DEFAULT_HOST_ZONE2_WIDTH                            Zone_Width
#      define AFSTATS_DEFAULT_HOST_ZONE2_HEIGHT                           Zone_Height

#      define AFSTATS_DEFAULT_HOST_ZONE2_END_X                            (AFSTATS_DEFAULT_HOST_ZONE2_START_X + Zone_Width)
#      define AFSTATS_DEFAULT_HOST_ZONE2_END_Y                            (AFSTATS_DEFAULT_HOST_ZONE2_START_Y + Zone_Height)
#      define AFSTATS_DEFAULT_HOST_ZONE2_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE2_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE2_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE2_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE2_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE2_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE2_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE2_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE3_START_X                          0 /*(WOI_CenterX - Zone_Width -1)*/
#      define AFSTATS_DEFAULT_HOST_ZONE3_START_Y                          0 /*(Zone0_StartY - Zone_Height -1)*/
#      define AFSTATS_DEFAULT_HOST_ZONE3_WIDTH                            Zone_Width
#      define AFSTATS_DEFAULT_HOST_ZONE3_HEIGHT                           Zone_Height

#      define AFSTATS_DEFAULT_HOST_ZONE3_END_X                            (AFSTATS_DEFAULT_HOST_ZONE3_START_X + Zone_Width)
#      define AFSTATS_DEFAULT_HOST_ZONE3_END_Y                            (AFSTATS_DEFAULT_HOST_ZONE3_START_Y + Zone_Height)
#      define AFSTATS_DEFAULT_HOST_ZONE3_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE3_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE3_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE3_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE3_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE3_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE3_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE3_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE4_START_X                          0 /*(WOI_CenterX +1)*/
#      define AFSTATS_DEFAULT_HOST_ZONE4_START_Y                          0 /*(Zone0_StartY - Zone_Height -1)*/
#      define AFSTATS_DEFAULT_HOST_ZONE4_WIDTH                            Zone_Width
#      define AFSTATS_DEFAULT_HOST_ZONE4_HEIGHT                           Zone_Height

#      define AFSTATS_DEFAULT_HOST_ZONE4_END_X                            (AFSTATS_DEFAULT_HOST_ZONE4_START_X + Zone_Width)
#      define AFSTATS_DEFAULT_HOST_ZONE4_END_Y                            (AFSTATS_DEFAULT_HOST_ZONE4_START_Y + Zone_Height)
#      define AFSTATS_DEFAULT_HOST_ZONE4_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE4_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE4_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE4_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE4_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE4_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE4_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE4_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE5_START_X                          0 /*(WOI_CenterX - Zone_Width -1)*/
#      define AFSTATS_DEFAULT_HOST_ZONE5_START_Y                          0 /*(Zone0_StartY + Zone_Height +1)*/
#      define AFSTATS_DEFAULT_HOST_ZONE5_WIDTH                            Zone_Width
#      define AFSTATS_DEFAULT_HOST_ZONE5_HEIGHT                           Zone_Height

#      define AFSTATS_DEFAULT_HOST_ZONE5_END_X                            (AFSTATS_DEFAULT_HOST_ZONE5_START_X + Zone_Width)
#      define AFSTATS_DEFAULT_HOST_ZONE5_END_Y                            (AFSTATS_DEFAULT_HOST_ZONE5_START_Y + Zone_Height)
#      define AFSTATS_DEFAULT_HOST_ZONE5_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE5_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE5_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE5_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE5_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE5_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE5_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE5_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE6_START_X                          0 /*(WOI_CenterX +1)*/
#      define AFSTATS_DEFAULT_HOST_ZONE6_START_Y                          0 /*(Zone0_StartY + Zone_Height +1)*/
#      define AFSTATS_DEFAULT_HOST_ZONE6_WIDTH                            Zone_Width
#      define AFSTATS_DEFAULT_HOST_ZONE6_HEIGHT                           Zone_Height

#      define AFSTATS_DEFAULT_HOST_ZONE6_END_X                            (AFSTATS_DEFAULT_HOST_ZONE6_START_X + Zone_Width)
#      define AFSTATS_DEFAULT_HOST_ZONE6_END_Y                            (AFSTATS_DEFAULT_HOST_ZONE6_START_Y + Zone_Height)
#      define AFSTATS_DEFAULT_HOST_ZONE6_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE6_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE6_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE6_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE6_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE6_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE6_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE6_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE7_START_X                          0
#      define AFSTATS_DEFAULT_HOST_ZONE7_START_Y                          0
#      define AFSTATS_DEFAULT_HOST_ZONE7_WIDTH                            0
#      define AFSTATS_DEFAULT_HOST_ZONE7_HEIGHT                           0

#      define AFSTATS_DEFAULT_HOST_ZONE7_END_X                            0
#      define AFSTATS_DEFAULT_HOST_ZONE7_END_Y                            0

#      define AFSTATS_DEFAULT_HOST_ZONE7_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE7_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE7_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE7_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE7_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE7_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE7_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE7_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE8_START_X                          0
#      define AFSTATS_DEFAULT_HOST_ZONE8_START_Y                          0
#      define AFSTATS_DEFAULT_HOST_ZONE8_WIDTH                            0
#      define AFSTATS_DEFAULT_HOST_ZONE8_HEIGHT                           0

#      define AFSTATS_DEFAULT_HOST_ZONE8_END_X                            0
#      define AFSTATS_DEFAULT_HOST_ZONE8_END_Y                            0

#      define AFSTATS_DEFAULT_HOST_ZONE8_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE8_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE8_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE8_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE8_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE8_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE8_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE8_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE9_START_X                          0
#      define AFSTATS_DEFAULT_HOST_ZONE9_START_Y                          0
#      define AFSTATS_DEFAULT_HOST_ZONE9_WIDTH                            0
#      define AFSTATS_DEFAULT_HOST_ZONE9_HEIGHT                           0

#      define AFSTATS_DEFAULT_HOST_ZONE9_END_X                            0
#      define AFSTATS_DEFAULT_HOST_ZONE9_END_Y                            0

#      define AFSTATS_DEFAULT_HOST_ZONE9_START_X_PER_WRT_WOI_SIZE_X       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE9_START_X)
#      define AFSTATS_DEFAULT_HOST_ZONE9_START_Y_PER_WRT_WOI_SIZE_Y       AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE9_START_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE9_END_X_PER_WRT_WOI_SIZE_X         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X( \
        AFSTATS_DEFAULT_HOST_ZONE9_END_X)
#      define AFSTATS_DEFAULT_HOST_ZONE9_END_Y_PER_WRT_WOI_SIZE_Y         AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y( \
        AFSTATS_DEFAULT_HOST_ZONE9_END_Y)
#      define AFSTATS_DEFAULT_HOST_ZONE0_ENABLED                          Flag_e_FALSE
#      define AFSTATS_DEFAULT_HOST_ZONE1_ENABLED                          Flag_e_FALSE
#      define AFSTATS_DEFAULT_HOST_ZONE2_ENABLED                          Flag_e_FALSE
#      define AFSTATS_DEFAULT_HOST_ZONE3_ENABLED                          Flag_e_FALSE
#      define AFSTATS_DEFAULT_HOST_ZONE4_ENABLED                          Flag_e_FALSE
#      define AFSTATS_DEFAULT_HOST_ZONE5_ENABLED                          Flag_e_FALSE
#      define AFSTATS_DEFAULT_HOST_ZONE6_ENABLED                          Flag_e_FALSE
#      define AFSTATS_DEFAULT_HOST_ZONE7_ENABLED                          Flag_e_FALSE
#      define AFSTATS_DEFAULT_HOST_ZONE8_ENABLED                          Flag_e_FALSE
#      define AFSTATS_DEFAULT_HOST_ZONE9_ENABLED                          Flag_e_FALSE

/*\@}*/
#endif /*AFSTATSPLATFORMSPECIFIC_H_*/

