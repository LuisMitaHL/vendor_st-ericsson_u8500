/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \file      AFStats.c
 * \brief     This is Focus Stats Manager for the stats used by the Autofocus algo .
 * \ingroup   Setting up the zone size ,shape, interrupt for the stats , the type of
 *            Window system used , and fetching the stats from HW registers to FW structure .
 * \author    Ashwani Chauhan
 * \date      28 Mar 2009
 * \endif
 */
#include "AFStats.h"
#include "AFStatsPlatformSpecific.h"
#include "AFStats_IPInterface.h"
#include "GenericFunctions.h"
#include "GPIOManager_OpInterface.h"
#include "lla_abstraction.h"
#include "ErrorHandler.h"
#include "string.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#   include "PictorProject_AFStats_AFStatsTraces.h"
#endif

// include Auto Focus 3 Modules (AFstats+FocusControl+FLADriver)
#if INCLUDE_FOCUS_MODULES

#define    AFSTATS_ERROR()             (2 <= g_AFStats_Status.e_AFStats_Error_Status)

#define    AFSTATS_CONFIG_ERROR()       (8 <= g_AFStats_Status.e_AFStats_Error_Status)

#define    AFSTATS_EXPORT_ERROR()       (2 == g_AFStats_Status.e_AFStats_Error_Status)

#define    AFSTATS_CONFIG_ASSERT()     \
           do { \
            if (AFSTATS_CONFIG_ERROR()) \
            { \
             return; \
            } \
           } while (0)

#define    AFSTATS_EXPORT_ASSERT()     \
           do { \
            if (AFSTATS_EXPORT_ERROR()) \
            { \
             return; \
            } \
           } while (0)

#define    AFZONE_START_X(index)    (g_AFStats_HostZoneConfig[index].u16_HostAFZoneStartX)
#define    AFZONE_WIDTH(index)      (g_AFStats_HostZoneConfig[index].u16_HostAFZoneWidth)
#define    AFZONE_END_X(index)      (AFZONE_START_X(index) + AFZONE_WIDTH(index))

#define    AFZONE_START_Y(index)    (g_AFStats_HostZoneConfig[index].u16_HostAFZoneStartY)
#define    AFZONE_HEIGHT(index)     (g_AFStats_HostZoneConfig[index].u16_HostAFZoneHeight)
#define    AFZONE_END_Y(index)      (AFZONE_START_Y(index) + AFZONE_HEIGHT(index))

void                                AFStats_7ZonesEyeShapedDefaultSetup (void);
void                                AFStats_HostZonesConfigurationCheck (void) TO_EXT_DDR_PRGM_MEM;
void                                AFStats_HostZonesConfigurationCheckInPercentage (void) TO_EXT_DDR_PRGM_MEM;
void                                AFStats_HostZonesConfigurationCheckInAbsolute (void) TO_EXT_DDR_PRGM_MEM;
void                                AFStats_SeperateCommonZoneBoundaries(void) TO_EXT_DDR_PRGM_MEM;
void                                AFStats_HostZonesConfigurationStatus (void);
void                                AFStats_MinMaxHostWindowsSystem (void) TO_EXT_DDR_PRGM_MEM;
void                                AFStats_GetZoneHWDimensions (void) TO_EXT_DDR_PRGM_MEM;
void                                AFStats_HostZonesConfigurationSetup (void);
Flag_te                             AFStats_GetStatsValidity (void) TO_EXT_DDR_PRGM_MEM;
void                                AFStats_ExportToHostMemory (uint32_t);
void                                AFStats_ErrorCheck (AFStats_Error_te) TO_EXT_DDR_PRGM_MEM;
Flag_te                             AFStats_ErrorCheck_ZoneOverlap (void) TO_EXT_DDR_PRGM_MEM;
Flag_te                             AFStats_ErrorCheck_HostData_Vs_RegisterData (void) TO_EXT_DDR_PRGM_MEM;


/// Global Structs Definition
AFStats_Controls_ts                 g_AFStats_Controls =
{
    AFSTATS_DEFAULT_HOST_STATS_EXPORT_ADDR,
    AFSTATS_DEFAULT_CTRL_CORING_VALUE,
    AFSTATS_DEFAULT_CTRL_H_RATIO_NUM,
    AFSTATS_DEFAULT_CTRL_H_RATIO_DEN,
    AFSTATS_DEFAULT_CTRL_V_RATIO_NUM,
    AFSTATS_DEFAULT_CTRL_V_RATIO_DEN,
    AFSTATS_DEFAULT_CTRL_HOST_ACTIVE_ZONES_COUNTER,
    AFSTATS_DEFAULT_CTRL_WINDOWS_SYSTEM,
    AFSTATS_DEFAULT_ZONE_CONFIG_COMMAND_COIN,
    AFSTATS_DEFAULT_ZONE_STATS_REQ_CMD,
    AFSTATS_DEFAULT_AUTO_REFRESH,
    AFSTATS_DEFAULT_ABS_SQUARE_ENABLED,
    AFSTATS_DEFAULT_REDUCED_ZONE_SETUP,
    AFSTATS_DEFAULT_HOST_ZONE_SETUP_IN_PERCENTAGE,
    AFSTATS_DEFAULT_STATS_EXPORT_COMMAND_COIN,
    AFSTATS_DEFAULT_STATS_CANCEL
};

AFStats_Status_ts                   g_AFStats_Status =
{
    AFSTATS_DEFAULT_MAX_FOCUS_MEASURE_PER_PIXEL,
    AFSTATS_DEFAULT_STARTING_AF_ZONE_LINE,
    AFSTATS_DEFAULT_WOI_WIDTH,
    AFSTATS_DEFAULT_WOI_HEIGHT,
    AFSTATS_DEFAULT_AF_ZONES_WIDTH,
    AFSTATS_DEFAULT_AF_ZONES_HEIGHT,
    AFSTATS_DEFAULT_STATUS_CORING_VALUE,
    AFSTATS_DEFAULT_STATUS_ACTIVE_ZONES_COUNTER,
    AFSTATS_DEFAULT_STATUS_H_RATIO_NUM,
    AFSTATS_DEFAULT_STATUS_H_RATIO_DEN,
    AFSTATS_DEFAULT_STATUS_V_RATIO_NUM,
    AFSTATS_DEFAULT_STATUS_V_RATIO_DEN,
    AFSTATS_DEFAULT_ZONE_INT_CYCLES,
    AFSTATS_DEFAULT_ZONE_INT_LAST_INDEX,
    AFSTATS_DEFAULT_ZONE_INT_FIRST_INDEX,
    AFSTATS_DEFAULT_STATUS_WINDOWS_SYSTEM,
    AFSTATS_DEFAULT_AF_ERROR,
    AFSTATS_DEFAULT_ZONE_CONFIG_STATUS_COIN,
    AFSTATS_DEFAULT_ZONE_STATS_REQ_STATUS_COIN,
    AFSTATS_DEFAULT_FORCED_IRQ,
    AFSTATS_DEFAULT_ABS_SQUARE_ENABLED,
    AFSTATS_DEFAULT_STATS_EXPORT_STATUS_COIN,
    AFSTATS_DEFAULT_STATS_WITH_LENSMOVE_FW_STATUS,
    AFSTATS_DEFAULT_FRAME_ID,
};

AFStats_AFZoneInterrupt_ts          g_AFStats_AFZoneInt =
{
    AFSTATS_DEFAULT_STATS_VALUE_0,
    AFSTATS_DEFAULT_STATS_VALUE_1,
    AFSTATS_DEFAULT_STATS_VALUE_2,
    AFSTATS_DEFAULT_STATS_VALUE_3,
    AFSTATS_DEFAULT_STATS_VALUE_4,
    AFSTATS_DEFAULT_STATS_VALUE_5,
    AFSTATS_DEFAULT_STATS_VALUE_6,
    AFSTATS_DEFAULT_STATS_VALUE_7,
    AFSTATS_DEFAULT_STATS_VALUE_8,
    AFSTATS_DEFAULT_STATS_VALUE_9
};

AFStats_HostZoneStatus_ts           g_AFStats_HostZoneStatus[AFS_HW_STATS_ZONE_NUMBER] =
{
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED },
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED },
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED },
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED },
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED },
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED },
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED },
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED },
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED },
    { 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED, 0xDEADFEED }
};

AFStats_HostZoneConfigPercentage_ts g_AFStats_HostZoneConfigPercentage[AFS_HW_STATS_ZONE_NUMBER] =
{
    { AFSTATS_DEFAULT_HOST_ZONE0_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE0_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE0_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE0_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE0_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE1_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE1_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE1_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE1_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE1_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE2_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE2_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE2_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE2_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE2_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE3_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE3_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE3_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE3_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE3_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE4_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE4_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE4_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE4_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE4_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE5_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE5_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE5_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE5_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE5_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE6_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE6_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE6_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE6_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE6_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE7_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE7_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE7_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE7_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE7_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE8_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE8_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE8_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE8_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE8_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE9_START_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE9_START_Y_PER_WRT_WOI_SIZE_Y,
            AFSTATS_DEFAULT_HOST_ZONE9_END_X_PER_WRT_WOI_SIZE_X, AFSTATS_DEFAULT_HOST_ZONE9_END_Y_PER_WRT_WOI_SIZE_Y,
                AFSTATS_DEFAULT_HOST_ZONE9_ENABLED, }
};

AFStats_HostZoneConfig_ts           g_AFStats_HostZoneConfig[AFS_HW_STATS_ZONE_NUMBER] =
{
    { AFSTATS_DEFAULT_HOST_ZONE0_START_X, AFSTATS_DEFAULT_HOST_ZONE0_START_Y, AFSTATS_DEFAULT_HOST_ZONE0_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE0_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE0_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE1_START_X, AFSTATS_DEFAULT_HOST_ZONE1_START_Y, AFSTATS_DEFAULT_HOST_ZONE1_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE1_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE1_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE2_START_X, AFSTATS_DEFAULT_HOST_ZONE2_START_Y, AFSTATS_DEFAULT_HOST_ZONE2_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE2_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE2_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE3_START_X, AFSTATS_DEFAULT_HOST_ZONE3_START_Y, AFSTATS_DEFAULT_HOST_ZONE3_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE3_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE3_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE4_START_X, AFSTATS_DEFAULT_HOST_ZONE4_START_Y, AFSTATS_DEFAULT_HOST_ZONE4_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE4_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE4_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE5_START_X, AFSTATS_DEFAULT_HOST_ZONE5_START_Y, AFSTATS_DEFAULT_HOST_ZONE5_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE5_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE5_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE6_START_X, AFSTATS_DEFAULT_HOST_ZONE6_START_Y, AFSTATS_DEFAULT_HOST_ZONE6_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE6_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE6_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE7_START_X, AFSTATS_DEFAULT_HOST_ZONE7_START_Y, AFSTATS_DEFAULT_HOST_ZONE7_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE7_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE7_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE8_START_X, AFSTATS_DEFAULT_HOST_ZONE8_START_Y, AFSTATS_DEFAULT_HOST_ZONE8_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE8_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE8_ENABLED, },
    { AFSTATS_DEFAULT_HOST_ZONE9_START_X, AFSTATS_DEFAULT_HOST_ZONE9_START_Y, AFSTATS_DEFAULT_HOST_ZONE9_WIDTH,
            AFSTATS_DEFAULT_HOST_ZONE9_HEIGHT, AFSTATS_DEFAULT_HOST_ZONE9_ENABLED, }
};

/// The following array of struct is very useful
/// it is used to access on the AF Stats Block like an array
/// with this i can use the for/loop instead the single variable
/// this solution generates some warning but there isn't problem
AFStats_ZoneVectorBase_ts           g_AFStats_HWZoneVector[AFS_HW_STATS_ZONE_NUMBER] =
{
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
};

AFStats_ZoneHWStatus_ts             g_AFStats_ZoneHWStatus[AFS_HW_STATS_ZONE_NUMBER] =
{
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
};

AFStats_Debug_ts                    g_AFStats_Debug = {0};

uint8_t                             g_u8_AFStatsFrameID       = 0xFF;
Flag_te                             g_isAFStatsCancelled      = Flag_e_TRUE;
volatile Flag_te                    g_AFStatsNotifyPending    = Flag_e_FALSE;
volatile uint32_t                   g_u32_AFStatsReceivedTime = 0;

#ifdef AF_DEBUG
extern volatile Flag_te            g_request_time_marked;
#endif //AF_DEBUG
//------------------------------------------------------------------/
//------------------------------------------------------------------/

/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_SetupStats(void)
 * \brief       Main Function of AFState Engine,Update all the Hardware Setting with AFStatsState.
 * \details     this function basically enable the control for the afstats , light stats , coring .
 *              Set up the WOI for the Af stats ,Zone width and height , the type of window system
 *              used.
 *
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_SetupStats(void)
{
   /* [PM]
    * Two (safe?) assumptions in AFStats Implementation:
    * 1. Stats accumulation for AFZones starts after Zone configuration
    *
    * Zone configuration is done in SMIA Rx interrupt context. So even if there is
    * a simultaneous zone config and stats request, Zone configuration should be
    * complete before stats accumulation of the first zone begins.
    *
    * 2. Interrupts for all zones arrive before SMIA Rx of next frame.
    *
    * Typically, an AFZone interrupt arrives when the zone end is reached.
    */

    // AC: TO be reviewed with Atul -(AFStats_ForceStatsUpdate()).
    if
    (
        (g_AFStats_Status.e_Coin_ZoneConfigStatus != g_AFStats_Controls.e_Coin_ZoneConfigCmd)
    ||  (AFStats_ForceStatsUpdate())
    )
    {
        OstTraceInt0(TRACE_DEBUG,"[Focus]Zone Config command received.");

        g_AFStats_Status.u16_WOIWidth  = GET_ISP_AF_STATS_CURRENT_WOI_H_Size();
        g_AFStats_Status.u16_WOIHeight = GET_ISP_AF_STATS_CURRENT_WOI_V_Size();

        // Don't assert, just update the status.
        AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_WOI_INVALID);

        // Validate and set the correct windowing system; no assert in this case too.
        AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_WINDOWS_SYSTEM_NOT_VALID);

        switch (g_AFStats_Status.e_AFStats_WindowsSystem_Status)
        {
            case AFStats_WindowsSystem_e_AF_HOST_SYSTEM:

                if (Flag_e_FALSE == AFSTATS_CONFIG_ERROR())
                {
                    AFStats_HostZonesConfigurationSetup();
                }

                if (AFSTATS_CONFIG_ERROR())
                {
                    OstTraceInt1(TRACE_DEBUG,"[Focus]g_AFStats_Status.e_AFStats_Error_Status = %d",g_AFStats_Status.e_AFStats_Error_Status);

                    // Reset Status PEs in view of Error
                    g_AFStats_Status.u8_IndexMax = g_AFStats_Status.u8_IndexMin = 0;
                    g_AFStats_Status.u8_ActiveZonesCounter = 0;

                    OstTraceInt0(TRACE_DEBUG,"[Focus]Error case, so configuring Default Zone System.");
                    /* no break, to configure default zones */

                }
                else  //[PM]FOCUS: Host Zone System configured.
                {
                    g_AFStats_Status.e_AFStats_WindowsSystem_Status = AFStats_WindowsSystem_e_AF_HOST_SYSTEM;
                    g_AFStats_Status.e_Coin_ZoneConfigStatus = g_AFStats_Controls.e_Coin_ZoneConfigCmd;

                    OstTraceInt0(TRACE_DEBUG,"[Focus]AF_HOST_SYSTEM configured!");
                    break;
                }

            case AFStats_WindowsSystem_e_AF_PRESET_DEFAULT_7_ZONES_SYSTEM:

                AFStats_7ZonesEyeShapedDefaultSetup();

                g_AFStats_Status.e_AFStats_WindowsSystem_Status = AFStats_WindowsSystem_e_AF_PRESET_DEFAULT_7_ZONES_SYSTEM;
                g_AFStats_Status.e_Coin_ZoneConfigStatus = g_AFStats_Controls.e_Coin_ZoneConfigCmd;

                OstTraceInt0(TRACE_DEBUG,"[Focus]Default Zone System requested and configured.");
                break;

            default: //[PM]FOCUS: Should not reach here!

                OstTraceInt0(TRACE_DEBUG,"[Focus]Should never hit the default case!");
                g_AFStats_Status.e_Coin_ZoneConfigStatus = g_AFStats_Controls.e_Coin_ZoneConfigCmd;
                break;
        }

        // afzones updated according to the new zoom fov set.
        if (AFStats_ForceStatsUpdate())
        {
            AFStats_ZoomUpdateAbsorbed();
        }
    }

    //AF block enable
    SET_ISP_AF_STATS_CTRL_Enable();

    //AF light stats enable
    SET_ISP_AF_STATS_ZONE_CTRL_LightStatsEnable();

    // check Coring
    if (g_AFStats_Controls.u8_CoringValue == 0)
    {
        SET_ISP_AF_STATS_ZONE_CTRL_CoringDisable();
        g_AFStats_Status.u8_CoringValue = 0;
    }
    else
    {
        SET_ISP_AF_STATS_ZONE_CTRL_CoringEnable();

        g_AFStats_Status.u8_CoringValue = g_AFStats_Controls.u8_CoringValue;

        SET_ISP_AF_STATS_CORING_value(g_AFStats_Status.u8_CoringValue);
    }

    // Check AbsSquare
    g_AFStats_Status.e_Flag_AbsSquareEnabled = g_AFStats_Controls.e_Flag_AbsSquareEnabled;
    if (g_AFStats_Status.e_Flag_AbsSquareEnabled)
    {
        SET_ISP_AF_STATS_ZONE_CTRL_AbsSquareEnable();
    }
    else
    {
        SET_ISP_AF_STATS_ZONE_CTRL_AbsSquareDisable();
    }

    //Clear cancel stats flag
    AFStats_SetStatsCancelStatus(Flag_e_FALSE);
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_7ZonesEyeshaped_Setup(void)
 * \brief       setups af stats zones for 7 zones, eye shaped, configuration
 * \details     Setting up the shape and size for each zone , usually the shape is build from the centre of the WOI.
 *              enabling the interrupt for the 6th zone , as this is the last zone for the Af stats .
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_7ZonesEyeShapedDefaultSetup(void)
{
    uint16_t    u16_XStart,
                u16_YStart;
    uint16_t    u16_XCenter;
    uint16_t    u16_YCenter;
    uint8_t     u8_i;

    u16_XCenter = GET_ISP_AF_STATS_CURRENT_WOI_H_Offset() + (g_AFStats_Status.u16_WOIWidth >> 1);
    u16_YCenter = GET_ISP_AF_STATS_CURRENT_WOI_V_Offset() + (g_AFStats_Status.u16_WOIHeight >> 1);

    //[PM]Adding 2 pixel correction for babylon
    u16_XCenter += 2;
    u16_YCenter += 2;

    u16_XStart = u16_XCenter - (g_AFStats_Status.u16_AFZonesWidth >> 1);
    u16_YStart = u16_YCenter - (g_AFStats_Status.u16_AFZonesHeight >> 1);

    // Disable AF Stats and clear all interrupts
    AFStats_StatsIntDisableAll();
    AFStats_StatsIntClearAll();

    // zone 0.X
    g_AFStats_ZoneHWStatus[0].u16_AFStartX = u16_XStart;
    SET_AF_STATS_ZONE_X_START_OFFSET_0_x_start(g_AFStats_ZoneHWStatus[0].u16_AFStartX);
    g_AFStats_ZoneHWStatus[0].u16_AFEndX = u16_XStart + g_AFStats_Status.u16_AFZonesWidth;
    SET_AF_STATS_ZONE_X_END_OFFSET_0_x_end(g_AFStats_ZoneHWStatus[0].u16_AFEndX);

    // zone 1.X
    g_AFStats_ZoneHWStatus[1].u16_AFStartX = u16_XStart - 1 - g_AFStats_Status.u16_AFZonesWidth;
    SET_AF_STATS_ZONE_X_START_OFFSET_1_x_start(g_AFStats_ZoneHWStatus[1].u16_AFStartX);
    g_AFStats_ZoneHWStatus[1].u16_AFEndX = u16_XStart - 1;
    SET_AF_STATS_ZONE_X_END_OFFSET_1_x_end(g_AFStats_ZoneHWStatus[1].u16_AFEndX);

    // zone 2.X
    g_AFStats_ZoneHWStatus[2].u16_AFStartX = u16_XStart + 1 + g_AFStats_Status.u16_AFZonesWidth;
    SET_AF_STATS_ZONE_X_START_OFFSET_2_x_start(g_AFStats_ZoneHWStatus[2].u16_AFStartX);
    g_AFStats_ZoneHWStatus[2].u16_AFEndX = u16_XStart + 1 + g_AFStats_Status.u16_AFZonesWidth * 2;
    SET_AF_STATS_ZONE_X_END_OFFSET_2_x_end(g_AFStats_ZoneHWStatus[2].u16_AFEndX);

    // the zone 0, 1, 2 have the same Y offset
    // 0.Y   1.Y   2.Y
    for (u8_i = 0; u8_i < 3; ++u8_i)
    {
        g_AFStats_ZoneHWStatus[u8_i].u16_AFStartY = u16_YStart;
        g_AFStats_ZoneHWStatus[u8_i].u16_AFEndY = u16_YStart + g_AFStats_Status.u16_AFZonesHeight;
    }


    //for Y-0
    SET_AF_STATS_ZONE_Y_START_OFFSET_0_y_start(g_AFStats_ZoneHWStatus[0].u16_AFStartY);
    SET_AF_STATS_ZONE_Y_END_OFFSET_0_y_end(g_AFStats_ZoneHWStatus[0].u16_AFEndY);

    //for Y-1
    SET_AF_STATS_ZONE_Y_START_OFFSET_1_y_start(g_AFStats_ZoneHWStatus[1].u16_AFStartY);
    SET_AF_STATS_ZONE_Y_END_OFFSET_1_y_end(g_AFStats_ZoneHWStatus[1].u16_AFEndY);

    //for Y-2
    SET_AF_STATS_ZONE_Y_START_OFFSET_2_y_start(g_AFStats_ZoneHWStatus[2].u16_AFStartY);
    SET_AF_STATS_ZONE_Y_END_OFFSET_2_y_end(g_AFStats_ZoneHWStatus[2].u16_AFEndY);

    //       +-----------------------+
    //       |      +---+ +---+      |
    //       |      | 3 | | 4 |      |
    //       |      +---+ +---+      |
    //       |   +---+ +---+ +---+   |
    //       |   | 1 | | 0 | | 2 |   |
    //       |   +---+ +---+ +---+   |
    //       |      +---+ +---+      |
    //       |      | 5 | | 6 |      |
    //       |      +---+ +---+      |
    //       +-----------------------+
    // zone 3 and 5 have the same X offset
    // 3.X - 5.X
    for (u8_i = 3; u8_i < 6; u8_i += 2)
    {
        g_AFStats_ZoneHWStatus[u8_i].u16_AFStartX = u16_XCenter - g_AFStats_Status.u16_AFZonesWidth - 1;
        g_AFStats_ZoneHWStatus[u8_i].u16_AFEndX = u16_XCenter - 1;
    }


    //for X-3
    SET_AF_STATS_ZONE_X_START_OFFSET_3_x_start(g_AFStats_ZoneHWStatus[3].u16_AFStartX);
    SET_AF_STATS_ZONE_X_END_OFFSET_3_x_end(g_AFStats_ZoneHWStatus[3].u16_AFEndX);

    //for X-5
    SET_AF_STATS_ZONE_X_START_OFFSET_5_x_start(g_AFStats_ZoneHWStatus[5].u16_AFStartX);
    SET_AF_STATS_ZONE_X_END_OFFSET_5_x_end(g_AFStats_ZoneHWStatus[5].u16_AFEndX);

    // zone 4 and 6 have the same X offset
    // 4.X - 6.X
    for (u8_i = 4; u8_i < 7; u8_i += 2)
    {
        g_AFStats_ZoneHWStatus[u8_i].u16_AFStartX = u16_XCenter + 1;
        g_AFStats_ZoneHWStatus[u8_i].u16_AFEndX = u16_XCenter + 1 + g_AFStats_Status.u16_AFZonesWidth;
    }


    //for X-4
    SET_AF_STATS_ZONE_X_START_OFFSET_4_x_start(g_AFStats_ZoneHWStatus[4].u16_AFStartX);
    SET_AF_STATS_ZONE_X_END_OFFSET_4_x_end(g_AFStats_ZoneHWStatus[4].u16_AFEndX);

    //for X-6
    SET_AF_STATS_ZONE_X_START_OFFSET_6_x_start(g_AFStats_ZoneHWStatus[6].u16_AFStartX);
    SET_AF_STATS_ZONE_X_END_OFFSET_6_x_end(g_AFStats_ZoneHWStatus[6].u16_AFEndX);

    // the zone 3, 4 have the same Y offset
    // 3.Y - 4.Y
    for (u8_i = 3; u8_i < 5; ++u8_i)
    {
        g_AFStats_ZoneHWStatus[u8_i].u16_AFStartY = u16_YStart - 1 - g_AFStats_Status.u16_AFZonesHeight;
        g_AFStats_ZoneHWStatus[u8_i].u16_AFEndY = u16_YStart - 1;
    }


    //for Y-3
    SET_AF_STATS_ZONE_Y_START_OFFSET_3_y_start(g_AFStats_ZoneHWStatus[3].u16_AFStartY);
    SET_AF_STATS_ZONE_Y_END_OFFSET_3_y_end(g_AFStats_ZoneHWStatus[3].u16_AFEndY);

    //for Y-4
    SET_AF_STATS_ZONE_Y_START_OFFSET_4_y_start(g_AFStats_ZoneHWStatus[4].u16_AFStartY);
    SET_AF_STATS_ZONE_Y_END_OFFSET_4_y_end(g_AFStats_ZoneHWStatus[4].u16_AFEndY);

    // store this information to autoskip frame af feature
    g_AFStats_Status.u16_StartingAFZoneLine = u16_YStart;

    // the zone 5, 6 have the same Y offset
    // 5.Y - 6.Y
    for (u8_i = 5; u8_i < 7; ++u8_i)
    {
        g_AFStats_ZoneHWStatus[u8_i].u16_AFStartY = u16_YStart + 1 + g_AFStats_Status.u16_AFZonesHeight;
        g_AFStats_ZoneHWStatus[u8_i].u16_AFEndY = u16_YStart + 1 + g_AFStats_Status.u16_AFZonesHeight * 2;
    }


    //for Y-5
    SET_AF_STATS_ZONE_Y_START_OFFSET_5_y_start(g_AFStats_ZoneHWStatus[5].u16_AFStartY);
    SET_AF_STATS_ZONE_Y_END_OFFSET_5_y_end(g_AFStats_ZoneHWStatus[5].u16_AFEndY);

    //for Y-6
    SET_AF_STATS_ZONE_Y_START_OFFSET_6_y_start(g_AFStats_ZoneHWStatus[6].u16_AFStartY);
    SET_AF_STATS_ZONE_Y_END_OFFSET_6_y_end(g_AFStats_ZoneHWStatus[6].u16_AFEndY);

    // Enable the Status of all the 7 zones, rest all 0 .
    g_AFStats_Status.u8_ActiveZonesCounter = 7;
    for (u8_i = 0; u8_i < AFStats_GetMaxNoOfZoneSupport(); u8_i++)
    {
        if (u8_i < 7)
        {
            g_AFStats_HostZoneStatus[u8_i].u32_Enabled = Flag_e_TRUE;
        }
        else
        {
            g_AFStats_HostZoneStatus[u8_i].u32_Enabled = Flag_e_FALSE;
        }
    }

    // Read the status of the host zones (get the real status)
    AFStats_HostZonesConfigurationStatus();

    // No need to check user request as configured default zone systtem.
    // So now calculating min max zone indices
    AFStats_MinMaxHostWindowsSystem();

    // Store this information to autoskip frame af feature
    g_AFStats_Status.u16_StartingAFZoneLine = g_AFStats_HostZoneStatus[g_AFStats_Status.u8_IndexMin].u32_AFZoneStartY;
    //g_AFStats_Status.u16_EndingAFZoneLine   = g_AFStats_HostZoneStatus[g_AFStats_Status.u8_IndexMax].u32_AFZoneEndY;

    // Enable AF Stats
    AFStats_StatsIntEnable();
}   //AFStats_7ZonesEyeShaped_Setup


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_HostZonesConfigurationSetup(void)
 * \brief
 * \details
 *
 * \return      uint8_t
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_HostZonesConfigurationSetup(void)
{
    //configuring zone parameters
    AFStats_HostZonesConfigurationCheck();

    /// if error then return
    AFSTATS_CONFIG_ASSERT();

    // Get host zone configuration  setup
    AFStats_GetZoneHWDimensions();

    // Partha: TBC - why configuring zones when stats interrupt is enabled?

    // Disable AF Stats and clear all interrupts
    AFStats_StatsIntDisableAll();
    AFStats_StatsIntClearAll();

    if (g_AFStats_HostZoneConfig[0].e_Flag_Enabled)
    {
        SET_AF_STATS_ZONE_X_START_OFFSET_0_x_start(g_AFStats_ZoneHWStatus[0].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_0_x_end(g_AFStats_ZoneHWStatus[0].u16_AFEndX);

        //for Y-0
        SET_AF_STATS_ZONE_Y_START_OFFSET_0_y_start(g_AFStats_ZoneHWStatus[0].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_0_y_end(g_AFStats_ZoneHWStatus[0].u16_AFEndY);
    }


    g_AFStats_HostZoneStatus[0].u32_Enabled = g_AFStats_HostZoneConfig[0].e_Flag_Enabled;

    if (g_AFStats_HostZoneConfig[1].e_Flag_Enabled)
    {
        // zone 1.X
        SET_AF_STATS_ZONE_X_START_OFFSET_1_x_start(g_AFStats_ZoneHWStatus[1].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_1_x_end(g_AFStats_ZoneHWStatus[1].u16_AFEndX);

        //for Y-1
        SET_AF_STATS_ZONE_Y_START_OFFSET_1_y_start(g_AFStats_ZoneHWStatus[1].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_1_y_end(g_AFStats_ZoneHWStatus[1].u16_AFEndY);
    }

    g_AFStats_HostZoneStatus[1].u32_Enabled = g_AFStats_HostZoneConfig[1].e_Flag_Enabled;

    if (g_AFStats_HostZoneConfig[2].e_Flag_Enabled)
    {
        // zone 2.X
        SET_AF_STATS_ZONE_X_START_OFFSET_2_x_start(g_AFStats_ZoneHWStatus[2].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_2_x_end(g_AFStats_ZoneHWStatus[2].u16_AFEndX);

        //for Y-2
        SET_AF_STATS_ZONE_Y_START_OFFSET_2_y_start(g_AFStats_ZoneHWStatus[2].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_2_y_end(g_AFStats_ZoneHWStatus[2].u16_AFEndY);
    }

    g_AFStats_HostZoneStatus[2].u32_Enabled = g_AFStats_HostZoneConfig[2].e_Flag_Enabled;

    // zone 3 and 5 have the same X offset
    // 3.X - 5.X
    if (g_AFStats_HostZoneConfig[3].e_Flag_Enabled)
    {
        //for X-3
        SET_AF_STATS_ZONE_X_START_OFFSET_3_x_start(g_AFStats_ZoneHWStatus[3].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_3_x_end(g_AFStats_ZoneHWStatus[3].u16_AFEndX);

        // the zone 3, 4 have the same Y offset
        // 3.Y - 4.Y
        //for Y-3
        SET_AF_STATS_ZONE_Y_START_OFFSET_3_y_start(g_AFStats_ZoneHWStatus[3].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_3_y_end(g_AFStats_ZoneHWStatus[3].u16_AFEndY);
    }

    g_AFStats_HostZoneStatus[3].u32_Enabled = g_AFStats_HostZoneConfig[3].e_Flag_Enabled;

    if (g_AFStats_HostZoneConfig[4].e_Flag_Enabled)
    {
        //for X-4
        SET_AF_STATS_ZONE_X_START_OFFSET_4_x_start(g_AFStats_ZoneHWStatus[4].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_4_x_end(g_AFStats_ZoneHWStatus[4].u16_AFEndX);

        //for Y-4
        SET_AF_STATS_ZONE_Y_START_OFFSET_4_y_start(g_AFStats_ZoneHWStatus[4].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_4_y_end(g_AFStats_ZoneHWStatus[4].u16_AFEndY);
    }

    g_AFStats_HostZoneStatus[4].u32_Enabled = g_AFStats_HostZoneConfig[4].e_Flag_Enabled;

    if (g_AFStats_HostZoneConfig[5].e_Flag_Enabled)
    {
        //for X-5
        SET_AF_STATS_ZONE_X_START_OFFSET_5_x_start(g_AFStats_ZoneHWStatus[5].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_5_x_end(g_AFStats_ZoneHWStatus[5].u16_AFEndX);

        // the zone 5, 6 have the same Y offset
        // 5.Y - 6.Y
        //for Y-5
        SET_AF_STATS_ZONE_Y_START_OFFSET_5_y_start(g_AFStats_ZoneHWStatus[5].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_5_y_end(g_AFStats_ZoneHWStatus[5].u16_AFEndY);
    }

    g_AFStats_HostZoneStatus[5].u32_Enabled = g_AFStats_HostZoneConfig[5].e_Flag_Enabled;

    //for X-6
    if (g_AFStats_HostZoneConfig[6].e_Flag_Enabled)
    {
        SET_AF_STATS_ZONE_X_START_OFFSET_6_x_start(g_AFStats_ZoneHWStatus[6].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_6_x_end(g_AFStats_ZoneHWStatus[6].u16_AFEndX);

        //for Y-6
        SET_AF_STATS_ZONE_Y_START_OFFSET_6_y_start(g_AFStats_ZoneHWStatus[6].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_6_y_end(g_AFStats_ZoneHWStatus[6].u16_AFEndY);
    }

    g_AFStats_HostZoneStatus[6].u32_Enabled = g_AFStats_HostZoneConfig[6].e_Flag_Enabled;

    if (g_AFStats_HostZoneConfig[7].e_Flag_Enabled)
    {
        //for X-7
        SET_AF_STATS_ZONE_X_START_OFFSET_7_x_start(g_AFStats_ZoneHWStatus[7].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_7_x_end(g_AFStats_ZoneHWStatus[7].u16_AFEndX);

        //for Y-7
        SET_AF_STATS_ZONE_Y_START_OFFSET_7_y_start(g_AFStats_ZoneHWStatus[7].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_7_y_end(g_AFStats_ZoneHWStatus[7].u16_AFEndY);
    }

    g_AFStats_HostZoneStatus[7].u32_Enabled = g_AFStats_HostZoneConfig[7].e_Flag_Enabled;

    if (g_AFStats_HostZoneConfig[8].e_Flag_Enabled)
    {
        //for X-8
        SET_AF_STATS_ZONE_X_START_OFFSET_8_x_start(g_AFStats_ZoneHWStatus[8].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_8_x_end(g_AFStats_ZoneHWStatus[8].u16_AFEndX);

        //for Y-8
        SET_AF_STATS_ZONE_Y_START_OFFSET_8_y_start(g_AFStats_ZoneHWStatus[8].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_8_y_end(g_AFStats_ZoneHWStatus[8].u16_AFEndY);
    }

    g_AFStats_HostZoneStatus[8].u32_Enabled = g_AFStats_HostZoneConfig[8].e_Flag_Enabled;

    if (g_AFStats_HostZoneConfig[9].e_Flag_Enabled)
    {
        //for X-9
        SET_AF_STATS_ZONE_X_START_OFFSET_9_x_start(g_AFStats_ZoneHWStatus[9].u16_AFStartX);
        SET_AF_STATS_ZONE_X_END_OFFSET_9_x_end(g_AFStats_ZoneHWStatus[9].u16_AFEndX);

        //for Y-9
        SET_AF_STATS_ZONE_Y_START_OFFSET_9_y_start(g_AFStats_ZoneHWStatus[9].u16_AFStartY);
        SET_AF_STATS_ZONE_Y_END_OFFSET_9_y_end(g_AFStats_ZoneHWStatus[9].u16_AFEndY);
    }

    g_AFStats_HostZoneStatus[9].u32_Enabled = g_AFStats_HostZoneConfig[9].e_Flag_Enabled;

    // Read the status of the host zones (get the real status)
    AFStats_HostZonesConfigurationStatus();

    // check the status against the configured.
    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_HOST_ENTERED_PARAMETER_NOT_EQUAL_TO_PROGRAMMED_PARAMETER);
    AFSTATS_CONFIG_ASSERT();

    // calculating min max zone indices
    AFStats_MinMaxHostWindowsSystem();

    // store this information to autoskip frame af feature
    g_AFStats_Status.u16_StartingAFZoneLine = g_AFStats_HostZoneStatus[g_AFStats_Status.u8_IndexMin].u32_AFZoneStartY;
    //g_AFStats_Status.u16_EndingAFZoneLine   = g_AFStats_HostZoneStatus[g_AFStats_Status.u8_IndexMax].u32_AFZoneEndY;

    // enable AF stats
    AFStats_StatsIntEnable();
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_HostZonesConfigurationCheck(void)
 * \brief
 * \details
 *
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_HostZonesConfigurationCheck(void)
{
    // Check generic zone parameters
    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_RATIO_NOTVALID);
    AFSTATS_CONFIG_ASSERT();

    g_AFStats_Status.u8_VRatioNum = g_AFStats_Controls.u8_VRatioNum;
    g_AFStats_Status.u8_HRatioNum = g_AFStats_Controls.u8_HRatioNum;
    g_AFStats_Status.u8_HRatioDen = g_AFStats_Controls.u8_HRatioDen;
    g_AFStats_Status.u8_VRatioDen = g_AFStats_Controls.u8_VRatioDen;

    g_AFStats_Status.u16_AFZonesWidth  = (g_AFStats_Status.u16_WOIWidth * g_AFStats_Status.u8_HRatioNum) / g_AFStats_Status.u8_HRatioDen;
    g_AFStats_Status.u16_AFZonesHeight = (g_AFStats_Status.u16_WOIHeight * g_AFStats_Status.u8_VRatioNum) / g_AFStats_Status.u8_VRatioDen;

    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_WINDOWSZONE_TOOMUCHLITTLE);
    AFSTATS_CONFIG_ASSERT();

    // Check if atleast one zone enabled
    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_HOST_NO_ZONE_ENABLED);
    AFSTATS_CONFIG_ASSERT();

    // Compute absolute values, if zone dimensions provided in percentage
    if (Flag_e_TRUE == g_AFStats_Controls.e_Flag_HostZoneSetupInPercentage)
    {
        AFStats_HostZonesConfigurationCheckInPercentage();
    }

    // Once percentage values are converted to absolute,
    // now check config for absolute..
    AFStats_HostZonesConfigurationCheckInAbsolute();

    // Fine Tune common Zone Boundaries now
    AFStats_SeperateCommonZoneBoundaries();
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_HostZonesConfigurationCheckInPercentage(void)
 * \brief
 * \details
 *
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_HostZonesConfigurationCheckInPercentage(void)
{
    uint8_t     u8_Index_I = 0;

    float_t     f_Start_X_Per = 0.0;
    float_t     f_Start_Y_Per = 0.0;
    float_t     f_End_X_Per = 0.0;
    float_t     f_End_Y_Per = 0.0;

    uint16_t    u16_AFStatsHostZoneEndX[AFS_HW_STATS_ZONE_NUMBER];
    uint16_t    u16_AFStatsHostZoneEndY[AFS_HW_STATS_ZONE_NUMBER];


    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_HOST_PER_VALUE_EXCEEDED_100);
    AFSTATS_CONFIG_ASSERT();

    // [PM]Even if the Host provides any zone start value = 0, it won't be an issue
    // as we are adding 2 pixels for babylon correction. So ideally, we will be
    // exporting the stats beginning from 3rd pixel. So just need to check whether any value
    // is peovided negative by mistake.
    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_HOST_PER_VALUE_NEGATIVE);
    AFSTATS_CONFIG_ASSERT();

    // calculating actual width and height for each zone , using % value entered by the host wrt WOI width & height.
    for (u8_Index_I = 0; u8_Index_I < AFStats_GetMaxNoOfZoneSupport(); u8_Index_I++)
    {
        // check if the value entered by the host for width or height % is greater then 100
        if (g_AFStats_HostZoneConfigPercentage[u8_Index_I].e_Flag_Enabled == Flag_e_TRUE)
        {
            f_Start_X_Per = g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneStartX_PER_wrt_WOIWidth;
            f_Start_Y_Per = g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneStartY_PER_wrt_WOIHeight;
            f_End_X_Per = g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneEndX_PER_wrt_WOIWidth;
            f_End_Y_Per = g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneEndY_PER_wrt_WOIHeight;

            g_AFStats_HostZoneConfig[u8_Index_I].u16_HostAFZoneStartX = (uint16_t) GenericFunctions_Ceiling(((float_t) (f_Start_X_Per * (float_t) g_AFStats_Status.u16_WOIWidth) / (float_t) 100.0));
            g_AFStats_HostZoneConfig[u8_Index_I].u16_HostAFZoneStartY = (uint16_t) GenericFunctions_Ceiling(((float_t) (f_Start_Y_Per * (float_t) g_AFStats_Status.u16_WOIHeight) / (float_t) 100.0));

            u16_AFStatsHostZoneEndX[u8_Index_I] = (uint16_t) GenericFunctions_Ceiling(((float_t) (f_End_X_Per * (float_t) g_AFStats_Status.u16_WOIWidth) / (float_t) 100.0));
            u16_AFStatsHostZoneEndY[u8_Index_I] = (uint16_t) GenericFunctions_Ceiling(((float_t) (f_End_Y_Per * (float_t) g_AFStats_Status.u16_WOIHeight) / (float_t) 100.0));

            g_AFStats_HostZoneConfig[u8_Index_I].u16_HostAFZoneWidth =
                (
                    u16_AFStatsHostZoneEndX[u8_Index_I] -
                    g_AFStats_HostZoneConfig[u8_Index_I].u16_HostAFZoneStartX
                );
            g_AFStats_HostZoneConfig[u8_Index_I].u16_HostAFZoneHeight =
                (
                    u16_AFStatsHostZoneEndY[u8_Index_I] -
                    g_AFStats_HostZoneConfig[u8_Index_I].u16_HostAFZoneStartY
                );

        }   // end-if

        // copy zone enable/disable status to g_AFStats_HostZoneConfig
        g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled = g_AFStats_HostZoneConfigPercentage[u8_Index_I].e_Flag_Enabled;
    }      // end-for

}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_HostZonesConfigurationCheckInAbsolute(void)
 * \brief
 * \details
 *
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_HostZonesConfigurationCheckInAbsolute(void)
{
    uint8_t u8_Index_I = 0;

    //update active zones counter
    for (
          u8_Index_I = 0, g_AFStats_Status.u8_ActiveZonesCounter = 0;
          (u8_Index_I < AFStats_GetMaxNoOfZoneSupport());
          ++u8_Index_I
        )
    {
        if (Flag_e_TRUE == g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled)
        {
            g_AFStats_Status.u8_ActiveZonesCounter++;
        }
    }

    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_HOST_ZONES_OVERLAPPED);
    AFSTATS_CONFIG_ASSERT();

    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_HOST_ZONES_INVALID_PARAMETERS);
    AFSTATS_CONFIG_ASSERT();

    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_HOST_ZONES_PARAMETERS_OUT_OF_BOUNDARY);
    AFSTATS_CONFIG_ASSERT();

    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_HOST_ZONE_SIZE_EXCEEDED_1M_PIX);
    AFSTATS_CONFIG_ASSERT();

    //Handling Small zone width or height warning here: compare with "ideal" zone width and height
    AFStats_ErrorCheck(AFStats_Error_e_AF_WARNING_HOST_ZONE_HEIGHT_WIDTH_LOW);
    AFSTATS_CONFIG_ASSERT();

}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_SeperateCommonZoneBoundaries(void)
 * \brief
 * \details
 *
 * \return      uint8_t
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_SeperateCommonZoneBoundaries(void)
{
    uint8_t u8_Index_I=0;
    uint8_t u8_Index_J=0;

    /* Seperating shared edges of zones */
    for (u8_Index_I = 0; (u8_Index_I < (AFStats_GetMaxNoOfZoneSupport() - 1)); ++u8_Index_I)
    {
        if (g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled)
        {
            for (u8_Index_J = u8_Index_I + 1; (u8_Index_J < AFStats_GetMaxNoOfZoneSupport()); ++u8_Index_J)
            {
                if (g_AFStats_HostZoneConfig[u8_Index_J].e_Flag_Enabled)
                {
                    /* Common vertical edge */
                    if (
                         (AFZONE_END_X(u8_Index_I) == AFZONE_START_X(u8_Index_J))
                         &&
                         (
                           (
                             (AFZONE_START_Y(u8_Index_I) >= AFZONE_START_Y(u8_Index_J))
                             &&
                             (AFZONE_START_Y(u8_Index_I) <= AFZONE_END_Y(u8_Index_J))
                           )
                           ||
                           (
                             (AFZONE_END_Y(u8_Index_I) >= AFZONE_START_Y(u8_Index_J))
                              &&
                             (AFZONE_END_Y(u8_Index_I) <= AFZONE_END_Y(u8_Index_J))
                           )
                         )
                       )
                       {
                           AFZONE_START_X(u8_Index_J)++;
                           AFZONE_WIDTH(u8_Index_J)--;
                       }

                    /* Common horizontal edge */
                    if (
                         (AFZONE_END_Y(u8_Index_I) == AFZONE_START_Y(u8_Index_J))
                         &&
                         (
                           (
                             (AFZONE_START_X(u8_Index_I) >= AFZONE_START_X(u8_Index_J))
                             &&
                             (AFZONE_START_X(u8_Index_I) <= AFZONE_END_X(u8_Index_J))
                           )
                           ||
                           (
                             (AFZONE_END_X(u8_Index_I) >= AFZONE_START_X(u8_Index_J))
                              &&
                             (AFZONE_END_X(u8_Index_I) <= AFZONE_END_X(u8_Index_J))
                           )
                         )
                       )
                       {
                           AFZONE_START_Y(u8_Index_J)++;
                           AFZONE_HEIGHT(u8_Index_J)--;
                       }
                }
            }
        }
     }
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_GetZoneHWDimensions(void)
 * \brief
 * \details
 *
 * \return      uint8_t
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_GetZoneHWDimensions(void)
{
    uint8_t     u8_ZoneNo;

    uint16_t    u16_VOffset;
    uint16_t    u16_HOffset;

    u16_HOffset = GET_ISP_AF_STATS_CURRENT_WOI_H_Offset();
    u16_VOffset = GET_ISP_AF_STATS_CURRENT_WOI_V_Offset();

    //[PM]Adding 2 pixel correction for babylon
    u16_HOffset += 2;
    u16_VOffset += 2;

    for (u8_ZoneNo = 0; u8_ZoneNo < AFStats_GetMaxNoOfZoneSupport(); u8_ZoneNo++)
    {
        g_AFStats_ZoneHWStatus[u8_ZoneNo].u16_AFStartX = u16_HOffset + g_AFStats_HostZoneConfig[u8_ZoneNo].u16_HostAFZoneStartX;
        g_AFStats_ZoneHWStatus[u8_ZoneNo].u16_AFStartY = u16_VOffset + g_AFStats_HostZoneConfig[u8_ZoneNo].u16_HostAFZoneStartY;
        g_AFStats_ZoneHWStatus[u8_ZoneNo].u16_AFEndX = g_AFStats_ZoneHWStatus[u8_ZoneNo].u16_AFStartX + g_AFStats_HostZoneConfig[u8_ZoneNo].u16_HostAFZoneWidth;
        g_AFStats_ZoneHWStatus[u8_ZoneNo].u16_AFEndY = g_AFStats_ZoneHWStatus[u8_ZoneNo].u16_AFStartY + g_AFStats_HostZoneConfig[u8_ZoneNo].u16_HostAFZoneHeight;
    }
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_HostZonesConfigurationStatus(void)
 * \brief       This function will populate the values in
 *              g_AFStats_HostZoneStatus directly from the
 *              hardware.
 * \return      uint8_t
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_HostZonesConfigurationStatus(void)
{
    uint16_t    u16_HOffset;
    uint16_t    u16_VOffset;

    u16_HOffset = GET_ISP_AF_STATS_CURRENT_WOI_H_Offset();
    u16_VOffset = GET_ISP_AF_STATS_CURRENT_WOI_V_Offset();

    //[PM]Adding 2 pixel correction for babylon
    u16_HOffset += 2;
    u16_VOffset += 2;

    // status for the Host zone 0 .
    if (g_AFStats_HostZoneStatus[0].u32_Enabled)
    {
        g_AFStats_HostZoneStatus[0].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_0_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[0].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_0_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[0].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_0_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[0].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_0_y_end() - u16_VOffset;

        g_AFStats_HostZoneStatus[0].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[0].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[0].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[0].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[0].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[0].u32_AFZoneStartY
            );
    }


    // status for the Host zone 1 .
    if (g_AFStats_HostZoneStatus[1].u32_Enabled)
    {
        g_AFStats_HostZoneStatus[1].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_1_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[1].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_1_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[1].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_1_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[1].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_1_y_end() - u16_VOffset;
        g_AFStats_HostZoneStatus[1].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[1].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[1].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[1].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[1].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[1].u32_AFZoneStartY
            );
    }


    // status for the Host zone 2 .
    if (g_AFStats_HostZoneStatus[2].u32_Enabled)
    {
        g_AFStats_HostZoneStatus[2].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_2_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[2].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_2_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[2].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_2_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[2].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_2_y_end() - u16_VOffset;
        g_AFStats_HostZoneStatus[2].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[2].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[2].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[2].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[2].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[2].u32_AFZoneStartY
            );
    }


    // status for the Host zone 3 .
    if (g_AFStats_HostZoneStatus[3].u32_Enabled)
    {
        g_AFStats_HostZoneStatus[3].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_3_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[3].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_3_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[3].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_3_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[3].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_3_y_end() - u16_VOffset;

        g_AFStats_HostZoneStatus[3].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[3].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[3].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[3].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[3].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[3].u32_AFZoneStartY
            );
    }


    // status for the Host zone 4 .
    if (g_AFStats_HostZoneStatus[4].u32_Enabled)
    {
        g_AFStats_HostZoneStatus[4].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_4_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[4].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_4_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[4].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_4_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[4].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_4_y_end() - u16_VOffset;

        g_AFStats_HostZoneStatus[4].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[4].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[4].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[4].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[4].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[4].u32_AFZoneStartY
            );
    }


    if (g_AFStats_HostZoneStatus[5].u32_Enabled)
    {   // status for the Host zone 5 .
        g_AFStats_HostZoneStatus[5].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_5_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[5].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_5_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[5].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_5_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[5].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_5_y_end() - u16_VOffset;

        g_AFStats_HostZoneStatus[5].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[5].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[5].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[5].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[5].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[5].u32_AFZoneStartY
            );
    }


    // status for the Host zone 6 .
    if (g_AFStats_HostZoneStatus[6].u32_Enabled)
    {
        g_AFStats_HostZoneStatus[6].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_6_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[6].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_6_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[6].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_6_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[6].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_6_y_end() - u16_VOffset;

        g_AFStats_HostZoneStatus[6].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[6].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[6].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[6].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[6].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[6].u32_AFZoneStartY
            );
    }


    // status for the Host zone 7 .
    if (g_AFStats_HostZoneStatus[7].u32_Enabled)
    {
        g_AFStats_HostZoneStatus[7].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_7_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[7].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_7_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[7].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_7_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[7].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_7_y_end() - u16_VOffset;

        g_AFStats_HostZoneStatus[7].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[7].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[7].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[7].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[7].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[7].u32_AFZoneStartY
            );
    }


    // status for the Host zone 8 .
    if (g_AFStats_HostZoneStatus[8].u32_Enabled)
    {
        g_AFStats_HostZoneStatus[8].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_8_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[8].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_8_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[8].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_8_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[8].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_8_y_end() - u16_VOffset;

        g_AFStats_HostZoneStatus[8].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[8].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[8].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[8].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[8].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[8].u32_AFZoneStartY
            );
    }


    // status for the Host zone 9 .
    if (g_AFStats_HostZoneStatus[9].u32_Enabled)
    {
        g_AFStats_HostZoneStatus[9].u32_AFZoneStartX = GET_AF_STATS_ZONE_X_START_OFFSET_9_x_start() - u16_HOffset;
        g_AFStats_HostZoneStatus[9].u32_AFZoneStartY = GET_AF_STATS_ZONE_Y_START_OFFSET_9_y_start() - u16_VOffset;
        g_AFStats_HostZoneStatus[9].u32_AFZoneEndX = GET_AF_STATS_ZONE_X_END_OFFSET_9_x_end() - u16_HOffset;
        g_AFStats_HostZoneStatus[9].u32_AFZoneEndY = GET_AF_STATS_ZONE_Y_END_OFFSET_9_y_end() - u16_VOffset;

        g_AFStats_HostZoneStatus[9].u32_AFZonesWidth =
            (
                g_AFStats_HostZoneStatus[9].u32_AFZoneEndX -
                g_AFStats_HostZoneStatus[9].u32_AFZoneStartX
            );
        g_AFStats_HostZoneStatus[9].u32_AFZonesHeight =
            (
                g_AFStats_HostZoneStatus[9].u32_AFZoneEndY -
                g_AFStats_HostZoneStatus[9].u32_AFZoneStartY
            );
    }
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_MinMaxHostWindowsSystem(void)
 * \brief
 * \details
 *
 * \return      uint8_t
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_MinMaxHostWindowsSystem(void)
{
    uint16_t    u16_MaxValueY;
    uint16_t    u16_MaxValueX;
    uint16_t    u16_MinValueY;
    uint16_t    u16_MinValueX;
    uint16_t    u16_TempY;
    uint16_t    u16_TempX;
    uint8_t     u8_Index = 0;  //initialized to zero.

    /* When code hits here, atleast one zone is enabled check has passed.*/
    while (!(g_AFStats_HostZoneStatus[u8_Index].u32_Enabled))
    {
        u8_Index++;
    };

    /* Initialize with the value of the "1st" enabled zone.*/
    u16_MinValueY = u16_MaxValueY = g_AFStats_HostZoneStatus[u8_Index].u32_AFZoneEndY;
    u16_MinValueX = u16_MaxValueX = g_AFStats_HostZoneStatus[u8_Index].u32_AFZoneEndX;
    g_AFStats_Status.u8_IndexMin = g_AFStats_Status.u8_IndexMax = u8_Index;

    /* Search for a minimum (Y). Lets say Y(min) = Y1.
     * If another minimum Y2 reached, replace it: Y(min) = Y2.
     * Else, if (Y2 = Y1), compare X2 with X1. Let X(min) = min(X2, X1).
     * X(min) is the g_AFStats_Status.u8_IndexMin index.
     */
    for (u8_Index = u8_Index + 1; u8_Index < AFStats_GetMaxNoOfZoneSupport(); u8_Index++)
    {
        if (g_AFStats_HostZoneStatus[u8_Index].u32_Enabled)
        {
            u16_TempX = g_AFStats_HostZoneStatus[u8_Index].u32_AFZoneEndX;
            u16_TempY = g_AFStats_HostZoneStatus[u8_Index].u32_AFZoneEndY;

            ///  search the lowest zone.
            if ((u16_TempY > u16_MaxValueY) || ((u16_TempY == u16_MaxValueY) && (u16_TempX > u16_MaxValueX)))
            {
                // a new max
                g_AFStats_Status.u8_IndexMax = u8_Index;
                u16_MaxValueX = u16_TempX;
                u16_MaxValueY = u16_TempY;
            }


            /// search for the highest zone.
            else if ((u16_TempY < u16_MinValueY) || ((u16_TempY == u16_MinValueY) && (u16_TempX < u16_MinValueX)))
            {
                // a new Lowest
                g_AFStats_Status.u8_IndexMin = u8_Index;
                u16_MinValueX = u16_TempX;
                u16_MinValueY = u16_TempY;
            }
        }   //end-if
    }       //end-for
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_SingleZoneStatsReadyISR(uint8_t u8_AFZoneIntCount)
 * \brief
 * \details     .
 *
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_SingleZoneStatsReadyISR(
uint8_t u8_AFZoneIntCount)
{
    switch (u8_AFZoneIntCount)
    {
        case 0:
            g_AFStats_HWZoneVector[0].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_0();
            g_AFStats_HWZoneVector[0].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_0();
            g_AFStats_AFZoneInt.u16_INT00_AUTOFOCUS++;
            break;

        case 1:
            g_AFStats_HWZoneVector[1].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_1();
            g_AFStats_HWZoneVector[1].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_1();
            g_AFStats_AFZoneInt.u16_INT01_AUTOFOCUS++;
            break;

        case 2:
            g_AFStats_HWZoneVector[2].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_2();
            g_AFStats_HWZoneVector[2].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_2();
            g_AFStats_AFZoneInt.u16_INT02_AUTOFOCUS++;
            break;

        case 3:
            g_AFStats_HWZoneVector[3].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_3();
            g_AFStats_HWZoneVector[3].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_3();
            g_AFStats_AFZoneInt.u16_INT03_AUTOFOCUS++;
            break;

        case 4:
            g_AFStats_HWZoneVector[4].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_4();
            g_AFStats_HWZoneVector[4].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_4();
            g_AFStats_AFZoneInt.u16_INT04_AUTOFOCUS++;
            break;

        case 5:
            g_AFStats_HWZoneVector[5].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_5();
            g_AFStats_HWZoneVector[5].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_5();
            g_AFStats_AFZoneInt.u16_INT05_AUTOFOCUS++;
            break;

        case 6:
            g_AFStats_HWZoneVector[6].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_6();
            g_AFStats_HWZoneVector[6].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_6();
            g_AFStats_AFZoneInt.u16_INT06_AUTOFOCUS++;
            break;

        case 7:
            g_AFStats_HWZoneVector[7].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_7();
            g_AFStats_HWZoneVector[7].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_7();
            g_AFStats_AFZoneInt.u16_INT07_AUTOFOCUS++;
            break;

        case 8:
            g_AFStats_HWZoneVector[8].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_8();
            g_AFStats_HWZoneVector[8].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_8();
            g_AFStats_AFZoneInt.u16_INT08_AUTOFOCUS++;
            break;

        case 9:
            g_AFStats_HWZoneVector[9].u32_Focus = GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_9();
            g_AFStats_HWZoneVector[9].u32_Light = GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_9();
            g_AFStats_AFZoneInt.u16_INT09_AUTOFOCUS++;
            break;
    }

    if (g_AFStats_Status.u8_IndexMax == u8_AFZoneIntCount)
    {
        OstTraceInt0(TRACE_DEBUG, "<INT> Focus ALL zones interrupt received");

        // note the time to check validity of stats later.
        g_u32_AFStatsReceivedTime = g_gpio_debug.u32_GlobalRefCounter;

        g_AFStats_Status.s8_ZoneIntCycles++;

        if (SystemConfig_IsInputImageSourceSensor())
        {
            // Store the value of the Frame for which the stats are stored .
            g_u8_AFStatsFrameID = Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter();
        }
        else
        {
            g_u8_AFStatsFrameID = 0xFF;
        }

        AFStats_FocusControlAFStatsReadyISR();
    }

}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_ProcessingHostCmd(void)
 * \brief
 * \details
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_ProcessingHostCmd(void)
{
    switch (g_AFStats_Controls.e_AFStats_HostCmd_Ctrl)
    {
        case AFStats_HostCmd_e_NO_REQ:
            OstTraceInt0(TRACE_DEBUG, "AFStats_HostCmd_e_NO_REQ");

            g_AFStats_Status.e_AFStats_StatusHostCmd_Status = AFStats_StatusHostCmd_e_NO_REQ;
            g_AFStats_Status.e_Coin_AFStatsExportStatus = g_AFStats_Controls.e_Coin_AFStatsExportCmd;
            break;

        case AFStats_HostCmd_e_REQ_STATS_ONCE:
            if (AFStats_Requested())
            {
                /* [PM]AFZone Config Error Recovery: if Host requested Stats without configuring Zones,
                 * or in case of any zone configuration error, configure default zones. */
                if ( (0 == AFStats_GetActiveZonesNumber()) || (AFSTATS_CONFIG_ERROR()) )
                {
                    AFStats_7ZonesEyeShapedDefaultSetup();
                    g_AFStats_Status.e_AFStats_WindowsSystem_Status = AFStats_WindowsSystem_e_AF_PRESET_DEFAULT_7_ZONES_SYSTEM;
                    g_AFStats_Status.e_Coin_ZoneConfigStatus = g_AFStats_Controls.e_Coin_ZoneConfigCmd;
                }

                OstTraceInt0(TRACE_DEBUG, "AFStats_HostCmd_e_REQ_STATS_ONCE - command sampled");

                AFStats_ExportToHostMemory((uint32_t) AFStats_GetStatsValidity());

                // Hold stats notification in the following case
                if (Complete_AFStats_And_Notify_After_Stop())
                {
                    g_AFStatsNotifyPending = Flag_e_TRUE;
                    break;
                }

                // Notify Stats
                AFStats_CompleteStatsNotify();
                OstTraceInt1(TRACE_DEBUG,"[AF Optimization]AF Stats Export Complete in Frame = %u", Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter());

              #ifdef AF_DEBUG
                g_request_time_marked = Flag_e_FALSE;
              #endif //AF_DEBUG
            }
            else
            {
                g_AFStats_Status.e_AFStats_StatusHostCmd_Status = AFStats_StatusHostCmd_e_STATS_COPY_NOT_YET_DONE;
            }

            break;

        case AFStats_HostCmd_e_REQ_STATS_CONTINUOUS_AND_VALID:

            OstTraceInt0(TRACE_DEBUG, "AFStats_HostCmd_e_REQ_STATS_CONTINUOUS_AND_VALID");

            if (Flag_e_TRUE == AFStats_GetStatsValidity())
            {
                // Send ONLY valid stats
                AFStats_ExportToHostMemory(1);

                // Notify Stats
                AFStats_CompleteStatsNotify();
            }

            break;

        case AFStats_HostCmd_e_REQ_STATS_CONTINUOUS_AND_WITHOUT_VALID_CHECK:

            OstTraceInt0(TRACE_DEBUG, "AFStats_HostCmd_e_REQ_STATS_CONTINUOUS");

            // Always send stats
            AFStats_ExportToHostMemory((uint32_t) AFStats_GetStatsValidity());

            // Notify Stats
            AFStats_CompleteStatsNotify();

            break;
    }
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          Flag_te AFStats_GetStatsValidity()
 * \brief       Get Validity of AF Stats.
 * \details
 * \return      stats validity: returns 0[Invalid] or 1[Valid]
 * \ingroup     AFstats
 * \endif
 **************************************************************/
Flag_te
AFStats_GetStatsValidity()
{
    Flag_te  validity;
    uint16_t u16_EndingAFZoneLine;
    float_t  f_afZonesTime_ms;
    float_t  f_afZonesIntStartTime_ms;

    /* Stats will be valid when (CONDITON 1) || (CONDITON 2) is true:
     * CONDITON 1: (stats_accumulation_start_time > last_movement_stop_time)
     * CONDITON 2: (stats_accumulation_stop_time < last_movement_start_time)
     * Ideally, we need to check ONLY CONDITION 1, as CONDITION 2 will be
     * automatically checked in the stats interrupt of the next frame.
     */

    //if lens is moving, return validity as FALSE.
    if (AFStats_FLADriverGetLensIsMoving())
    {
        OstTraceInt0(TRACE_USER1,"[AF Optimization]AFStats Validity = 0 (Lens Still Moving)");
        return Flag_e_FALSE;
    }

    //If g_u32_LensMoveStopTime = 0, then following scenarios are possible:
    //1. There is no actuator.
    //2. Actuator is present but no lens movement request.
    //3. Lens moved and stopped, and a 'valid' stats was already exported corresponding to that request.
    //In all these scenarios, return validity as TRUE.
    if (0 == g_u32_LensMoveStopTime)
    {
        OstTraceInt0(TRACE_USER1,"[AF Optimization]AFStats Validity = 1 (No Lens Movement request)");
        return Flag_e_TRUE;
    }


   /* Lens movement has taken place. So now compute f_afZonesIntStartTime_ms.
    * To compute f_afZonesIntStartTime_ms (t),
    * step 1: take stats received time (t1)
    * step 2: subtract zone readout time  (t2)
    * step 3: subtract integration time (t3).
    * t = t1 - t2 - t3;
    * No need to take care of overflow/underflow in calculations as min/max range
    * of the global counter is a subset of the values allowed by the datatype (uint32).
    */

    //[PM]The following calculations to be removed from interrupt context
    u16_EndingAFZoneLine = g_AFStats_HostZoneStatus[g_AFStats_Status.u8_IndexMax].u32_AFZoneEndY;

    //[PM] macro from lla_abstraction.h used to avoid float
    //First assign readout time (t2).
    f_afZonesTime_ms = (
            (CURRENT_SENSOR_LINE_READOUT_TIME_US() / 1000.0) *
            (u16_EndingAFZoneLine - g_AFStats_Status.u16_StartingAFZoneLine));


    //Add integration time (t2 + t3)
    f_afZonesTime_ms += (g_FrameParamStatus.u32_ExposureTime_us / 1000.0);

    //Calculate (t) now.
    //Check if (t1) would become "-ve" if (t2+t3) is subtracted from it.
    //On a safer side, always check '<' instead of '<=' (or '>' instead of '>=')
    if (f_afZonesTime_ms < (float_t)g_u32_AFStatsReceivedTime)
    {
        //More probability: counter did not rollover between the two timing measurements.
        f_afZonesIntStartTime_ms = g_u32_AFStatsReceivedTime - f_afZonesTime_ms;
        OstTraceInt0(TRACE_USER1,"[AF Optimization]afZonesIntStartTime Calculation Usual case.");
    }
    else
    {
        //Less probability: counter got rollover between the two timing measurements
        f_afZonesIntStartTime_ms = (GLOBAL_COUNTER_MAX_VALUE - f_afZonesTime_ms) + g_u32_AFStatsReceivedTime;
        OstTraceInt0(TRACE_USER1,"[AF Optimization]afZonesIntStartTime Calculation Counter ROLLOVER case.");
    }

    //Now check if the global counter got rollover between the two timings
    if (GET_ABS_DIFF(g_u32_LensMoveStopTime, f_afZonesIntStartTime_ms) < GLOBAL_COUNTER_MAX_VALUE/2)
    {
        //More probability: counter did not rollover between the two timing measurements.
        if ((float_t)g_u32_LensMoveStopTime < f_afZonesIntStartTime_ms)
        {
            validity = Flag_e_TRUE;
        }
        else
        {
            validity = Flag_e_FALSE;
        }
        OstTraceInt0(TRACE_USER1,"[AF Optimization]Validity computation Counter Usual case.");
    }
    else
    {
        //Less probability: counter got rollover between the two timing measurements
        if ((float_t)g_u32_LensMoveStopTime < f_afZonesIntStartTime_ms)
        {
            validity = Flag_e_FALSE;
        }
        else
        {
            validity = Flag_e_TRUE;
        }
        OstTraceInt0(TRACE_USER1,"[AF Optimization]Validity computation Counter ROLLOVER case.");
    }

    //OstTraceInt1(TRACE_USER1,"[AF Optimization]Lens Move Start Time = %u",g_u32_LensMoveStartTime);
    OstTraceInt1(TRACE_USER1,"[AF Optimization]Lens Move Stop Time = %u",g_u32_LensMoveStopTime);
    OstTraceInt1(TRACE_USER1,"[AF Optimization]AFStats Received Time = %u",g_u32_AFStatsReceivedTime);
    OstTraceInt1(TRACE_USER1,"[AF Optimization]Ending AF Zone Line = %u",u16_EndingAFZoneLine);
    OstTraceInt1(TRACE_USER1,"[AF Optimization]AF Zones (Readout + Exposure) Time = %u",f_afZonesTime_ms);
    OstTraceInt1(TRACE_USER1,"[AF Optimization]AF Zones Integration Start Time = %u",f_afZonesIntStartTime_ms);
    OstTraceInt1(TRACE_USER1,"[AF Optimization]AFStats Validity = %u",(uint32_t)validity);

    //Reset lens move time counts, iff stats validity is TRUE. Otherwise, retain history information.
    if (Flag_e_TRUE == validity)
    {
        //g_u32_LensMoveStartTime = 0;
        g_u32_LensMoveStopTime = 0;
    }
    g_u32_AFStatsReceivedTime = 0;

    return validity;
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_ExportToHostMemory(uint32_t)
 * \brief       Copy the focus measure  values to host address.
 * \details
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_ExportToHostMemory(
uint32_t    u32_valid)
{
    uint16_t    u16_Index_I;

    OstTraceInt1(TRACE_FLOW, "<AF>AFStats_ExportToHostMemory: u32_valid: %d", u32_valid);

    //if no address then error is shown and the stats are not exported
    AFStats_ErrorCheck(AFStats_Error_e_AF_ERROR_HOST_ADDRESS_NOT_SPECIFIED_FOR_STATS_EXPORTING);
    AFSTATS_EXPORT_ASSERT();

    AFStats_ExportFormat_ts *p_AFStats_ExportFormat = (AFStats_ExportFormat_ts *) g_AFStats_Controls.pu32_HostAssignedAddr;

    // Store the Value of the Frame to the Expoting statistics structure , so that the right frame ID can be exported.
    g_AFStats_Status.u8_focus_stats_Frame_ID = g_u8_AFStatsFrameID;

    for (u16_Index_I = 0; u16_Index_I < AFStats_GetMaxNoOfZoneSupport(); ++u16_Index_I)
    {
        if (g_AFStats_HostZoneStatus[u16_Index_I].u32_Enabled)
        {
            g_AFStats_HostZoneStatus[u16_Index_I].u32_Focus = g_AFStats_HWZoneVector[u16_Index_I].u32_Focus;
            g_AFStats_HostZoneStatus[u16_Index_I].u32_Light = g_AFStats_HWZoneVector[u16_Index_I].u32_Light;

            //[PM]: legacy PE - Weight is 1 by default
            g_AFStats_HostZoneStatus[u16_Index_I].u32_WeightAssigned = 1;
        }
        else
        {
            g_AFStats_HostZoneStatus[u16_Index_I].u32_Focus = 0xDEADFEED;
            g_AFStats_HostZoneStatus[u16_Index_I].u32_Light = 0xDEADFEED;
            g_AFStats_HostZoneStatus[u16_Index_I].u32_WeightAssigned = 0xDEADFEED;
        }
    }

    memcpy(p_AFStats_ExportFormat->afStats, g_AFStats_HostZoneStatus, sizeof(AFStats_HostZoneStatus_ts) * AFStats_GetMaxNoOfZoneSupport());

    p_AFStats_ExportFormat->afStatsValid = u32_valid;
    p_AFStats_ExportFormat->afStatsLensPos = (uint32_t) AFStats_FLADriverGetLowLevelLensPosition();
    p_AFStats_ExportFormat->afFrameId = (uint32_t) g_AFStats_Status.u8_focus_stats_Frame_ID;

    // Copying FrameParamStatus_Af paramaters for export.
    g_FrameParamStatus_Af.u32_AfStatsValid = u32_valid;
    g_FrameParamStatus_Af.u32_AfStatsLensPos = (uint32_t) (AFStats_FLADriverGetLowLevelLensPosition());
    g_FrameParamStatus_Af.u32_AfStatsFrameId = (uint32_t) (g_AFStats_Status.u8_focus_stats_Frame_ID);

    if (0 != g_FrameParamStatus_Af.pu32_HostAssignedFrameStatusAddr)
    {
        memcpy(g_FrameParamStatus_Af.pu32_HostAssignedFrameStatusAddr, &g_FrameParamStatus_Af, sizeof(FrameParamStatus_Af_ts));
    }

}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_ErrorCheck(AFStats_Error_te)
 * \brief
 * \details     Function to handle AFStats Errors.
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_ErrorCheck(AFStats_Error_te err_no)
{
    Flag_te isError = Flag_e_FALSE;
    uint8_t u8_Index_I = 0;

    switch (err_no)
    {
        case AFStats_Error_e_AF_ERROR_OK:
            break;


        case AFStats_Error_e_AF_WARNING_HOST_ZONE_HEIGHT_WIDTH_LOW:
            for (u8_Index_I = 0; u8_Index_I < AFStats_GetMaxNoOfZoneSupport(); u8_Index_I++)
            {
                if (g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled)
                {
                    if
                    (
                        (AFZONE_WIDTH(u8_Index_I) < g_AFStats_Status.u16_AFZonesWidth)
                        ||
                        (AFZONE_HEIGHT(u8_Index_I) < g_AFStats_Status.u16_AFZonesHeight)
                    )
                    {
                        isError = Flag_e_TRUE;
                        break;
                    }
                }
            }
            break;


        case AFStats_Error_e_AF_ERROR_HOST_ADDRESS_NOT_SPECIFIED_FOR_STATS_EXPORTING:
            if (0 == g_AFStats_Controls.pu32_HostAssignedAddr)
            {
                isError = Flag_e_TRUE;
            }
            break;


        case AFStats_Error_e_AF_ERROR_WOI_INVALID:
            if ((0 == g_AFStats_Status.u16_WOIWidth) || (0 == g_AFStats_Status.u16_WOIHeight))
            {
                isError = Flag_e_TRUE;
            }
            break;


        case AFStats_Error_e_AF_ERROR_WINDOWS_SYSTEM_NOT_VALID:
            if (g_AFStats_Controls.e_AFStats_WindowsSystem_Control > AFStats_WindowsSystem_e_AF_PRESET_DEFAULT_7_ZONES_SYSTEM)
            {
                // [PM]Assign the default system and flag error
                g_AFStats_Status.e_AFStats_WindowsSystem_Status = AFStats_WindowsSystem_e_AF_PRESET_DEFAULT_7_ZONES_SYSTEM;
                isError = Flag_e_TRUE;
            }
            else
            {
                g_AFStats_Status.e_AFStats_WindowsSystem_Status = g_AFStats_Controls.e_AFStats_WindowsSystem_Control;
            }
            break;


        case AFStats_Error_e_AF_ERROR_WINDOWSZONE_TOOMUCHLITTLE:
            if (g_AFStats_Status.u16_AFZonesHeight < 6 || g_AFStats_Status.u16_AFZonesWidth < 12)
            {
               isError = Flag_e_TRUE;
            }
            break;


        case AFStats_Error_e_AF_ERROR_RATIO_NOTVALID:
            if
            (
                g_AFStats_Controls.u8_VRatioNum == 0
            ||  g_AFStats_Controls.u8_HRatioNum == 0
            ||  g_AFStats_Controls.u8_VRatioDen == 0
            ||  g_AFStats_Controls.u8_HRatioDen == 0
            ||  (3 * g_AFStats_Controls.u8_VRatioNum) > g_AFStats_Controls.u8_VRatioDen // we have to reduce not increase
            ||  (3 * g_AFStats_Controls.u8_HRatioNum) > g_AFStats_Controls.u8_HRatioDen // we have to reduce not increase
            )
            {
                isError = Flag_e_TRUE;
            }
            break;


        case AFStats_Error_e_AF_ERROR_HOST_NO_ZONE_ENABLED:
            for (u8_Index_I = 0, isError = Flag_e_TRUE; u8_Index_I < AFStats_GetMaxNoOfZoneSupport(); u8_Index_I++)
            {
                if
                 (
                  ((Flag_e_TRUE == g_AFStats_Controls.e_Flag_HostZoneSetupInPercentage) &&
                   (Flag_e_TRUE == g_AFStats_HostZoneConfigPercentage[u8_Index_I].e_Flag_Enabled))
                   ||
                  ((Flag_e_FALSE == g_AFStats_Controls.e_Flag_HostZoneSetupInPercentage) &&
                   (Flag_e_TRUE == g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled))
                  )
                {
                    isError = Flag_e_FALSE;
                    break;
                }
            }
            break;


        case AFStats_Error_e_AF_ERROR_HOST_ZONES_OVERLAPPED:
            isError = AFStats_ErrorCheck_ZoneOverlap();
            break;


        case AFStats_Error_e_AF_ERROR_HOST_ZONES_INVALID_PARAMETERS:
            for (u8_Index_I = 0; u8_Index_I < AFStats_GetMaxNoOfZoneSupport(); u8_Index_I++)
            {
                if (g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled)
                {
                    if
                    (
                        (AFZONE_START_X(u8_Index_I) >= AFZONE_END_X(u8_Index_I))
                        ||
                        (AFZONE_START_Y(u8_Index_I) >= AFZONE_END_Y(u8_Index_I))
                    )
                    {
                        isError = Flag_e_TRUE;
                        break;
                    }
                }
            }
            break;


        case AFStats_Error_e_AF_ERROR_HOST_ZONES_PARAMETERS_OUT_OF_BOUNDARY:
            for (u8_Index_I = 0; u8_Index_I < AFStats_GetMaxNoOfZoneSupport(); u8_Index_I++)
            {
                if (g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled)
                {
                    if
                    (
                        !(
                            GET_ISP_AF_STATS_CURRENT_WOI_H_BOUNDARY_check(AFZONE_START_X(u8_Index_I))
                            &&
                            GET_ISP_AF_STATS_CURRENT_WOI_V_BOUNDARY_check(AFZONE_START_Y(u8_Index_I))
                            &&
                            GET_ISP_AF_STATS_CURRENT_WOI_H_BOUNDARY_check(AFZONE_END_X(u8_Index_I))
                            &&
                            GET_ISP_AF_STATS_CURRENT_WOI_V_BOUNDARY_check(AFZONE_END_Y(u8_Index_I))
                         )
                    )
                    {
                        isError = Flag_e_TRUE;
                        break;
                    }

                 }
            }
            break;


        case AFStats_Error_e_AF_ERROR_HOST_PER_VALUE_EXCEEDED_100:
           for (u8_Index_I = 0; u8_Index_I < AFStats_GetMaxNoOfZoneSupport(); u8_Index_I++)
           {
               if (g_AFStats_HostZoneConfigPercentage[u8_Index_I].e_Flag_Enabled)
               {
                   if
                   (
                       (g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneStartX_PER_wrt_WOIWidth > 100.0)
                   ||  (g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneStartY_PER_wrt_WOIHeight > 100.0)
                   ||  (g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneEndX_PER_wrt_WOIWidth > 100.0)
                   ||  (g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneEndY_PER_wrt_WOIHeight > 100.0)
                   )
                   {
                       isError = Flag_e_TRUE;
                       break;
                   }
                }
            }
            break;


        case AFStats_Error_e_AF_ERROR_HOST_PER_VALUE_NEGATIVE:
           for (u8_Index_I = 0; u8_Index_I < AFStats_GetMaxNoOfZoneSupport(); u8_Index_I++)
           {
               if (g_AFStats_HostZoneConfigPercentage[u8_Index_I].e_Flag_Enabled)
               {
                   if
                   (
                       (g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneStartX_PER_wrt_WOIWidth < 0.0)
                   ||  (g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneStartY_PER_wrt_WOIHeight < 0.0)
                   ||  (g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneEndX_PER_wrt_WOIWidth < 0.0)
                   ||  (g_AFStats_HostZoneConfigPercentage[u8_Index_I].f_HostAFZoneEndY_PER_wrt_WOIHeight < 0.0)
                   )
                   {
                       isError = Flag_e_TRUE;
                       break;
                   }
                }
            }
            break;


        case AFStats_Error_e_AF_ERROR_HOST_ZONE_SIZE_EXCEEDED_1M_PIX:
            for (u8_Index_I = 0; u8_Index_I < AFStats_GetMaxNoOfZoneSupport(); u8_Index_I++)
            {
                if (g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled)
                {
                    if ((AFZONE_WIDTH(u8_Index_I) * AFZONE_HEIGHT(u8_Index_I)) > 1000000)
                    {
                        isError = Flag_e_TRUE;
                        break;
                    }
                }
            }
            break;


        case AFStats_Error_e_AF_ERROR_HOST_ENTERED_PARAMETER_NOT_EQUAL_TO_PROGRAMMED_PARAMETER:
            isError = AFStats_ErrorCheck_HostData_Vs_RegisterData();
            break;


        default:
            break;
    }

    //Assigning the value of Error to Error_Status
    if (Flag_e_TRUE == isError)
    {
        g_AFStats_Status.e_AFStats_Error_Status = err_no;
    }

    return;
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_ErrorCheck_ZoneOverlap(void)
 * \brief
 * \return      TRUE, if error; FALSE if no error
 * \ingroup     AFstats
 * \endif
 **************************************************************/
Flag_te
AFStats_ErrorCheck_ZoneOverlap(void)
{
    uint8_t u8_Index_I =0;
    uint8_t u8_Index_J =0;

    for (u8_Index_I = 0; (u8_Index_I < (AFStats_GetMaxNoOfZoneSupport() - 1)); ++u8_Index_I)
    {
        if (g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled)
        {
            for (u8_Index_J = u8_Index_I + 1; (u8_Index_J < AFStats_GetMaxNoOfZoneSupport()); ++u8_Index_J)
            {
                if (g_AFStats_HostZoneConfig[u8_Index_J].e_Flag_Enabled)
                {
                    //    CASE 1                             CASE 2
                    //  +------+                               +------+
                    //  |i     |                               |     i|
                    //  |   +--+---+                       +---+--+   |
                    //  |   |  |  j|                       |j  |  |   |
                    //  +---+--+   |                       |   +--+---+
                    //      |      |                       |      |
                    //      +------+                       +---+--+
                    //
                    //
                    //    CASE 3                            CASE 4
                    //  +------+                               +------+
                    //  |j     |                               |     j|
                    //  |   +--+---+                       +---+--+   |
                    //  |   |  |  i|                       |i  |  |   |
                    //  +---+--+   |                       |   +--+---+
                    //      |      |                       |      |
                    //      +------+                       +---+--+
                    //

                    //Checking Overlap in X
                    if
                      (
                       (
                         (AFZONE_START_X(u8_Index_I) > AFZONE_START_X(u8_Index_J))
                         &&
                         (AFZONE_START_X(u8_Index_I) < AFZONE_END_X(u8_Index_J))
                       )
                       ||
                       (
                         (AFZONE_END_X(u8_Index_I) > AFZONE_START_X(u8_Index_J))
                         &&
                         (AFZONE_END_X(u8_Index_I) < AFZONE_END_X(u8_Index_J))
                       )
                      )
                    {
                       //X is Overlapping; so now checking Overlap in Y
                        if
                          (
                           (
                            (AFZONE_START_Y(u8_Index_I) > AFZONE_START_Y(u8_Index_J))
                            &&
                            (AFZONE_START_Y(u8_Index_I) < AFZONE_END_Y(u8_Index_J))
                           )
                           ||
                           (
                             (AFZONE_END_Y(u8_Index_I) > AFZONE_START_Y(u8_Index_J))
                             &&
                             (AFZONE_END_Y(u8_Index_I) < AFZONE_END_Y(u8_Index_J))
                           )
                          )
                        {
                            return (Flag_e_TRUE);
                        }
                    }
                }  // end-if( g_AFStats_HostZoneConfig[u8_Index_J].e_Flag_Enabled )
            }     // end-for j
        }          // end-if( g_AFStats_HostZoneConfig[u8_Index_I].e_Flag_Enabled )
    }             // end-for I

    return Flag_e_FALSE;
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          Flag_te AFStats_ErrorCheck_HostData_Vs_RegisterData(void)
 * \brief       Validates AF register data
 * \details     Validates values programmed in the registers to
 *              those which were intended to be programmed.
 * \return      TRUE, if error; FALSE if no error
 * \ingroup     AFstats
 * \endif
 **************************************************************/
Flag_te
AFStats_ErrorCheck_HostData_Vs_RegisterData(void)
{
    uint8_t u8_Index_I;

    for (u8_Index_I = 0; u8_Index_I < AFStats_GetMaxNoOfZoneSupport(); u8_Index_I++)
    {
        if (g_AFStats_HostZoneStatus[u8_Index_I].u32_Enabled)
        {
            if
            (
              !(
                  (g_AFStats_HostZoneStatus[u8_Index_I].u32_AFZoneStartX == (uint32_t)AFZONE_START_X(u8_Index_I))
                  &&
                  (g_AFStats_HostZoneStatus[u8_Index_I].u32_AFZoneStartY == (uint32_t)AFZONE_START_Y(u8_Index_I))
                  &&
                  (g_AFStats_HostZoneStatus[u8_Index_I].u32_AFZoneEndX == (uint32_t)AFZONE_END_X(u8_Index_I))
                  &&
                  (g_AFStats_HostZoneStatus[u8_Index_I].u32_AFZoneEndY == (uint32_t)AFZONE_END_Y(u8_Index_I))
                  &&
                  (g_AFStats_HostZoneStatus[u8_Index_I].u32_AFZonesWidth == (uint32_t)AFZONE_WIDTH(u8_Index_I))
                  &&
                  (g_AFStats_HostZoneStatus[u8_Index_I].u32_AFZonesHeight == (uint32_t)AFZONE_HEIGHT(u8_Index_I))
               )
            )
            {
                return (Flag_e_TRUE);
            }
        }
    }

    return Flag_e_FALSE;
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void HandlePendingAFRequest(void)
 * \brief
 * \details     Function to handle pending stats request, if any.
                Handling depends on the value of g_AFStats_Controls.e_Flag_AFStatsCancel.
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
HandlePendingAFRequest(void)
{
    // Cancel AF pending stats request, if any
    if
    (
        (Flag_e_TRUE == g_AFStats_Controls.e_Flag_AFStatsCancel)
    ||  (Flag_e_TRUE == g_SystemSetup.e_Flag_abortRx_OnStop)
    ||  (Flag_e_TRUE == g_ErrorHandler.e_Flag_Error_Abort)
    )
    {
        AFStats_Cancel();
        g_AFStatsNotifyPending = Flag_e_FALSE;
    }
    else
    {   //Complete AF pending stats notification, if any.
        if (Flag_e_TRUE == g_AFStatsNotifyPending)
        {
            AFStats_CompleteStatsNotify();
            g_AFStatsNotifyPending = Flag_e_FALSE;
        }
    }


    return;
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_Cancel(void)
 * \brief
 * \details
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/

void
AFStats_Cancel(void)
{
    uint16_t    u16_Index_I;

    //If its sensor changover usecase, don't do anything with stats.
    if (AFStats_IsFDMRequestPending())
    {
        return;
    }

    if (AFStats_Requested())
    {
        //Disable stats
        SET_ISP_AF_STATS_CTRL_Disable();

        //Disable all zones
        AFStats_StatsIntDisableAll();

        //Clear all interrupts
        AFStats_StatsIntClearAll();

        //Copy Dummy Stats
        AFStats_ExportFormat_ts *p_AFStats_CancelledStats = (AFStats_ExportFormat_ts *) g_AFStats_Controls.pu32_HostAssignedAddr;

        // Store the Value of the Frame to the Expoting statistics structure , so that the right frame ID can be exported.
        g_AFStats_Status.u8_focus_stats_Frame_ID = 0xFF;

        for (u16_Index_I = 0; u16_Index_I < AFStats_GetMaxNoOfZoneSupport(); ++u16_Index_I)
        {
            if (g_AFStats_HostZoneStatus[u16_Index_I].u32_Enabled)
            {
                g_AFStats_HostZoneStatus[u16_Index_I].u32_Focus = 0xDEADFEED;
                g_AFStats_HostZoneStatus[u16_Index_I].u32_Light = 0xDEADFEED;
                g_AFStats_HostZoneStatus[u16_Index_I].u32_WeightAssigned = 0xDEADFEED;
            }
        }

        memcpy(p_AFStats_CancelledStats->afStats, g_AFStats_HostZoneStatus, sizeof(AFStats_HostZoneStatus_ts) * AFStats_GetMaxNoOfZoneSupport());

        //mark stats as INVALID.
        p_AFStats_CancelledStats->afStatsValid = 0;

        p_AFStats_CancelledStats->afStatsLensPos = (uint32_t) AFStats_FLADriverGetLowLevelLensPosition();
        p_AFStats_CancelledStats->afFrameId = (uint32_t) g_AFStats_Status.u8_focus_stats_Frame_ID;

        //Cleanup now..
        //Reset stats PEs
        if (AFStats_RequestedWithoutLensCommand())
        {
            g_AFStats_Status.e_Coin_AFStatsExportStatus = g_AFStats_Controls.e_Coin_AFStatsExportCmd;
            g_AFStats_Status.e_AFStats_StatusHostCmd_Status = AFStats_StatusHostCmd_e_STATS_CANCELLED;
        }

        //[PM]'else' not used here to handle the usecase in which statistics requested via both
        //afstats coin as well as fladriver flag. Ideally, the host should reset this flag
        //when it requests for stats through afstats control coin.
        if (AFStats_RequestedWithLensCommand())
        {
            //[PM]Since pending lens movement is already handled: stop command is not taken
            //until lens movement is pending, so lens must be stopped when this code is hit.

            FLADriver_AFStatsSetStatsWithLensMoveFWStatus(Flag_e_FALSE);
        }

        //Set cancel stats flag
        AFStats_SetStatsCancelStatus(Flag_e_TRUE);

        //Send Notification
        AFStats_StatsReadyNotify();
    }

}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_CompleteStatsNotify(void)
 * \brief
 * \details     Complete pending stats notification.
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_CompleteStatsNotify(void)
{
    // Resetting the Flag to FALSE when the stats exporting is done
    if (AFStats_RequestedWithLensCommand())
    {
        FLADriver_AFStatsSetStatsWithLensMoveFWStatus(Flag_e_FALSE);
    }

    //Reset Status - this assignment is ok, as the status enums are an extension of the control enums.
    g_AFStats_Status.e_AFStats_StatusHostCmd_Status = g_AFStats_Controls.e_AFStats_HostCmd_Ctrl;

    //Equal export coin
    g_AFStats_Status.e_Coin_AFStatsExportStatus = g_AFStats_Controls.e_Coin_AFStatsExportCmd;

    //Send Notification
    AFStats_StatsReadyNotify();
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          Avoid FStats_StatsIntEnable(void)
 * \brief
 * \details
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_StatsIntEnable(void)
{
    uint16_t    u16_InterruptMask;

    u16_InterruptMask =
        (
            g_AFStats_HostZoneStatus[9].u32_Enabled <<
            9 |
            g_AFStats_HostZoneStatus[8].u32_Enabled <<
            8 |
            g_AFStats_HostZoneStatus[7].u32_Enabled <<
            7 |
            g_AFStats_HostZoneStatus[6].u32_Enabled <<
            6 |
            g_AFStats_HostZoneStatus[5].u32_Enabled <<
            5 |
            g_AFStats_HostZoneStatus[4].u32_Enabled <<
            4 |
            g_AFStats_HostZoneStatus[3].u32_Enabled <<
            3 |
            g_AFStats_HostZoneStatus[2].u32_Enabled <<
            2 |
            g_AFStats_HostZoneStatus[1].u32_Enabled <<
            1 |
            g_AFStats_HostZoneStatus[0].u32_Enabled <<
            0
        );

    SET_ISP_AF_STATS_ZONE_ENABLE_Word(u16_InterruptMask);

    SET_ISP_AF_STATS_ZONE_IntEnable(u16_InterruptMask);
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_StatsIntDisableAll(void)
 * \brief
 * \details
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_StatsIntDisableAll(void)
{
    SET_ISP_AF_STATS_ZONE_ENABLE_Word(0x00);
    SET_ISP_AF_STATS_ZONE_IntEnable(0x00);
}


/*************************************************************
 *
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void AFStats_StatsIntClearAll(void)
 * \brief
 * \details
 * \return      void
 * \ingroup     AFstats
 * \endif
 **************************************************************/
void
AFStats_StatsIntClearAll(void)
{
    SET_ISP_AF_STATS_CLEAR_INTR_STATS_AutoFocus();
    SET_ISP_AF_STATS_DFV_Dfv_Disable();
}

#endif // INCLUDE_FOCUS_MODULES

