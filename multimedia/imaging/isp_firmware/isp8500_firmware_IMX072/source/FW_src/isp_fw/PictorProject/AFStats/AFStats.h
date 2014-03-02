/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \defgroup AFStats Manager Module
 * \brief It's Focus Stats Manager for the stats used by the Autofocus algo.
 */

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \file      AFStats.h
 * \brief     Header File containing external function declarations and
 *            defines for AutoFocus Stats Manager and Focus Stats Compiler.
 * \ingroup   AFStats
 * \endif
 */
#ifndef AFSTATS_H_
#   define AFSTATS_H_

/*Includes*/
#   include "Platform.h"
#   include "AFStats_IPInterface.h"
#   include "AFStats_OPInterface.h"

/*Defines*/
#define AFS_HIBYTE(w)               ((uint8_t) (((uint16_t) (w) >> 8) & 0xFF))
#define AFS_MAX_FSWM_PER_G_PIXELS   2
// Max Number of AF Stats Zones
#define AFS_HW_STATS_ZONE_NUMBER    10

/*global variables*/

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \enum      AFStats_AFWindowsSystem_te
 * \brief     Enum to give the AFWindow system chosen, 9zone system or 7zone
 *            system or some host defined system .
 * \details   it signifies the AF stats area in terms of the ZONE around the
 *            centre of the image, from where the AFstats are collected.
 * \ingroup   AFStats.
 * \endif
 */
typedef enum
{
    AFStats_WindowsSystem_e_AF_HOST_SYSTEM                  = 0,
    AFStats_WindowsSystem_e_AF_PRESET_DEFAULT_7_ZONES_SYSTEM= 1,
} AFStats_WindowsSystem_te;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \enum      AFStats_Error_te
 * \brief     Enum to give the Status of the ERROR(if any)occurs in bAFStats_Error
 *            of AFStats_Status PE .
 * \details   According to the status of the error, the the type of error and the
 *            cause of invalid AF stats can be known. Rules:
 *            No-error or warnings have enum values 0 and 1.
 *            All zone config related errors have enum values equal to or more than 8.
 * \ingroup   AFStats.
 * \endif
 */
typedef enum
{
    /// When there is no ERROR.
    AFStats_Error_e_AF_ERROR_OK                                                         = 0,

    /// Warning Message if host zone width and height is
    /// less then the standard width and height for a current WOI.
    /// This is only a warning message , host can work with this warning.
    AFStats_Error_e_AF_WARNING_HOST_ZONE_HEIGHT_WIDTH_LOW                               = 1,

    /// error if address for exporting the statistics is not specified
    AFStats_Error_e_AF_ERROR_HOST_ADDRESS_NOT_SPECIFIED_FOR_STATS_EXPORTING             = 2,

    /// Error shown when AF WOI Width is not valid (for eg, 0).
    AFStats_Error_e_AF_ERROR_WOI_INVALID                                                = 8,

    /// if bWindowSystem is neither of 9,7 or Host defined ZONE system then
    /// the Error is shown .
    AFStats_Error_e_AF_ERROR_WINDOWS_SYSTEM_NOT_VALID                                   = 9,

    /// If for any  AF ZONE Width and Height is less then a constant(fixed) value
    AFStats_Error_e_AF_ERROR_WINDOWSZONE_TOOMUCHLITTLE                                  = 10,

    /// if vertical/Horizontal  ratio(Num/Den) for a any one of zone becomes
    /// Num = 0 or Den = 0 or 3*num > Den , the Error is shown .
    AFStats_Error_e_AF_ERROR_RATIO_NOTVALID                                             = 11,

    /// Error shown when there aren't active zones, ActiveZoneCounter becomes 0 .
    AFStats_Error_e_AF_ERROR_HOST_NO_ZONE_ENABLED                                       = 12,

    /// If in AFStats_WindowsSystem_e_AF_HOST_SYSTEM zones are overlapped ,like shown below .
        ///  CASE 1                         //  CASE 2
        ///  +------+                       //      +------+
        ///  |i     |                       //      |     j|
        ///  |   +--+---+                   //  +---+--+   |
        ///  |   |  |  j|                   //  |i  |  |   |
        ///  +---+--+   |                   //  |   +--+---+
        ///      |      |                   //  |      |
        ///      +------+                   //  +---+--+

        ///  CASE 3                         //  CASE 4
        ///  +------+                       //      +------+
        ///  |j     |                       //      |     i|
        ///  |   +--+---+                   //  +---+--+   |
        ///  |   |  |  i|                   //  |j  |  |   |
        ///  +---+--+   |                   //  |   +--+---+
        ///      |      |                   //  |      |
        ///      +------+                   //  +---+--+
    AFStats_Error_e_AF_ERROR_HOST_ZONES_OVERLAPPED                                      = 13,

    /// host entered parameters are not correct.
    AFStats_Error_e_AF_ERROR_HOST_ZONES_INVALID_PARAMETERS                              = 14,

    /// host entered parameters are not within the current WOI.
    AFStats_Error_e_AF_ERROR_HOST_ZONES_PARAMETERS_OUT_OF_BOUNDARY                      = 15,

    /// error if host entered percentage for either height or width more then 100 .
    AFStats_Error_e_AF_ERROR_HOST_PER_VALUE_EXCEEDED_100                                = 16,

    /// error if the host set per value less than 0.
    AFStats_Error_e_AF_ERROR_HOST_PER_VALUE_NEGATIVE                                    = 17,

    /// error if the size of the zone exceeded 1M pixels.
    AFStats_Error_e_AF_ERROR_HOST_ZONE_SIZE_EXCEEDED_1M_PIX                             = 18,

    /// if the parameters that has been programmed by the host for the zone
    /// is not what has been read from hw , error will be shown .
    AFStats_Error_e_AF_ERROR_HOST_ENTERED_PARAMETER_NOT_EQUAL_TO_PROGRAMMED_PARAMETER   = 19,

} AFStats_Error_te;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \enum      AFStats_HostCmd_te
 * \brief
 *
 * \details
 *
 * \ingroup   AFStats.
 * \endif
 */
typedef enum
{
    /// no request for stats from host
    AFStats_HostCmd_e_NO_REQ                                                            = 0,

    /// request of stats only for once , doesn't matter valid or invalid .
    AFStats_HostCmd_e_REQ_STATS_ONCE                                                    = 1,

    /// request for contineously
    AFStats_HostCmd_e_REQ_STATS_CONTINUOUS_AND_VALID                                    = 2,

    ///request for the contineous but not with valid check.
    AFStats_HostCmd_e_REQ_STATS_CONTINUOUS_AND_WITHOUT_VALID_CHECK                      = 3
} AFStats_HostCmd_te;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \enum      AFStats_HostCmd_te
 * \brief
 *
 * \details
 *
 * \ingroup   AFStats.
 * \endif
 */
typedef enum
{
    /// status of the host request , when no request this is set .
    AFStats_StatusHostCmd_e_NO_REQ                                                      = 0,

    /// This signifies the Stats Copy has been to external memory.
    AFStats_StatusHostCmd_e_STATS_COPY_DONE                                             = 1,

    /// This variable shows that the stats are contineously exported to memory but only valid stats are copied .
    AFStats_StatusHostCmd_e_STATS_CONTINUOUS_AND_VALID                                  = 2,

    /// this shows the status of contineously exporting the stats to memory but without valid check.
    AFStats_StatusHostCmd_e_STATS_CONTINUOUS_AND_WITHOUT_VALID_CHECK                    = 3,

    /// This shows that the command has been issued by host for stats copy to external memory but
    /// the coin is not toggled , show this value  of enum shos the stats are still not copied to memory.
    AFStats_StatusHostCmd_e_STATS_COPY_NOT_YET_DONE                                     = 4,

    /// This shows that the command which has been issued by host for stats copy
    /// has been cancelled by the Firmware.
    AFStats_StatusHostCmd_e_STATS_CANCELLED                                             = 5
} AFStats_StatusHostCmd_te;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \struct    AFStats_Controls_ts
 * \brief     Elements used for setting various control parameters for Autofocus Stats.
 * \ingroup   AFStats
 * \endif
 */
typedef struct
{
    /// host specified address for the stats exporting to external memory .
    uint32_t    *pu32_HostAssignedAddr;

    /// Set the Coring value;
    uint8_t     u8_CoringValue;

    /// The required Horizontal ratio for a each zone
    /// Horizontal numerator value .
    uint8_t     u8_HRatioNum;

    /// Horizontal Denomerator value .
    uint8_t     u8_HRatioDen;

    /// The required vertical ratio for a each zone
        /// Vertical numerator value .
    uint8_t     u8_VRatioNum;

    /// Vertical Denomerator value .
    uint8_t     u8_VRatioDen;

    /// No of Enabled Zones
    uint8_t     u8_HostActiveZonesCounter;

    /// Set the Main System Windows control;
    uint8_t     e_AFStats_WindowsSystem_Control;

    ///  command coin to inform the host zone setup has been done.
    uint8_t     e_Coin_ZoneConfigCmd;

    ///  command coin to inform the host requestfor AF stats done.
    uint8_t     e_AFStats_HostCmd_Ctrl;

    /// Control the automatic update of af stats settings at the beginning of each frame
    uint8_t     e_Flag_AutoRefresh;

    /// Enable or disable the AbsSquare;
    uint8_t     e_Flag_AbsSquareEnabled;

    /// zone system reduced or not .
    uint8_t     e_Flag_ReducedZoneSetup;

    /// zone parameters in percentage term .
    uint8_t     e_Flag_HostZoneSetupInPercentage;

    /// AFStats exporting coin machanism .
    uint8_t     e_Coin_AFStatsExportCmd;

    /// Significant when there is a pending stats request, and the Host issues a STOP command. \n
    /// If TRUE, pending stats request will be cancelled before FW moves to STOPPED state. \n
    /// If FALSE, pending stats request will be completed, but notification will send to the Host in STOPPED state. \n
    /// [DEFAULT]: Flag_e_TRUE
    uint8_t     e_Flag_AFStatsCancel;
} AFStats_Controls_ts;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \struct    AFStats_Status_ts
 * \brief     Elements used for Knowing Status of parameters for Autofocus Stats.
 * \ingroup   AFStats
 * \endif
 */
typedef struct
{
    /// Theoretical max focus measure per pixel accumulated by the FSWM stats
    uint32_t    u32_MaxFocusMeasurePerPixel;

    /// The starting Line of the AFzone in WOI .
    uint16_t    u16_StartingAFZoneLine;

    /// current WOI Width
    uint16_t    u16_WOIWidth;

    /// current WOI Height
    uint16_t    u16_WOIHeight;

    /// zones width
    uint16_t    u16_AFZonesWidth;

    /// zones height
    uint16_t    u16_AFZonesHeight;

    /// The current coring value
    uint8_t     u8_CoringValue;

    /// How many zones are currently active
    uint8_t     u8_ActiveZonesCounter;

    /// The required horizontal ratio for a each zone
    uint8_t     u8_HRatioNum;

    ///required hor ratio for each zone .
    uint8_t     u8_HRatioDen;

    /// The required vertical ratio for a each zone
    uint8_t     u8_VRatioNum;

    ///required hor ratio for each zone .
    uint8_t     u8_VRatioDen;

    /// the no of zone interrupt cycles
    int8_t      s8_ZoneIntCycles;

    /// host config max zone no , for which the interrupt in a frame comes last .
    uint8_t     u8_IndexMax;

    /// host config min zone no , for which the interrupt in a frame comes first .
    uint8_t     u8_IndexMin;

    /// Window system , either the host define or the Default 7 zone eye shaped window at the center of the WOI.
    uint8_t     e_AFStats_WindowsSystem_Status;

    /// The current Error
    uint8_t     e_AFStats_Error_Status;

    ///  command coin status to inform the host zone setup .
    uint8_t     e_Coin_ZoneConfigStatus;

    ///  command coin req status of stats request from host .
    uint8_t     e_AFStats_StatusHostCmd_Status;

    /// Flag indicating that the current int has been forced externally, and not by any af stats
    uint8_t     e_Flag_ForcedAFStatsIrq;

    /// The current status of ABS Sqare
    uint8_t     e_Flag_AbsSquareEnabled;

    ///  command coin status to inform the stats exporting to memory  .
    uint8_t     e_Coin_AFStatsExportStatus;

    /// A global variable used for the AFStats Statistics Exporting to memory (specified by the host - Ext Memory)
    /// This variable is used only when the Command for stats gathering and the Lens Movement is given .
    /// this variable works as same as "e_Coin_AFStatsExportCmd" of g_AFStats_Controls. But here its not taken as control coin machanism ,
    /// instead as Flag is set(by FW) when the afstats and Lens movement is done together and the same is reset (by FW)
    ///when the copy of afstatistics are done .
    uint8_t     e_Flag_FW_LensWithStatsStatus;

    /// Frame ID , for which the values of this Structure corresponds ,
    /// this is also exported as a part of statistics on host request.
    uint8_t    u8_focus_stats_Frame_ID;
} AFStats_Status_ts;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \struct    AFStats_AFZoneInterrupt_ts
 * \brief
 * \ingroup   AFStats
 * \endif
 */
typedef struct
{
    uint16_t    u16_INT00_AUTOFOCUS;
    uint16_t    u16_INT01_AUTOFOCUS;
    uint16_t    u16_INT02_AUTOFOCUS;
    uint16_t    u16_INT03_AUTOFOCUS;
    uint16_t    u16_INT04_AUTOFOCUS;
    uint16_t    u16_INT05_AUTOFOCUS;
    uint16_t    u16_INT06_AUTOFOCUS;
    uint16_t    u16_INT07_AUTOFOCUS;
    uint16_t    u16_INT08_AUTOFOCUS;
    uint16_t    u16_INT09_AUTOFOCUS;
} AFStats_AFZoneInterrupt_ts;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \struct    AFStats_HostZoneConfigPercentage_ts
 * \brief        Host Programmable Zone System , where Host deciode the AF Zone Size in Percentage of the WOI.
 * \ingroup     AFStats
 * \endif
 */
typedef struct
{
    /// current Host Programmed Start X with in WOI X size .
    float_t f_HostAFZoneStartX_PER_wrt_WOIWidth;

    /// current Host Programmed Start Y with in WOI Y size .
    float_t f_HostAFZoneStartY_PER_wrt_WOIHeight;

    /// Width of the zone selected by the host .
    float_t f_HostAFZoneEndX_PER_wrt_WOIWidth;

    /// Height of the zone selected by the host .
    float_t f_HostAFZoneEndY_PER_wrt_WOIHeight;

    /// enable disable host zone
    uint8_t e_Flag_Enabled;
} AFStats_HostZoneConfigPercentage_ts;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \struct    AFStats_HostZoneConfig_ts
 * \brief           Host Programmable Zone System , where Host deciode the AF Zone Size .
 * \ingroup   AFStats
 * \endif
 */
typedef struct
{
    /// current Host Programmed Start X with in WOI X size .
    uint16_t    u16_HostAFZoneStartX;

    /// current Host Programmed Start Y with in WOI Y size .
    uint16_t    u16_HostAFZoneStartY;

    /// Width of the zone selected by the host .
    uint16_t    u16_HostAFZoneWidth;

    /// Height of the zone selected by the host .
    uint16_t    u16_HostAFZoneHeight;

    /// enable disable host zone
    uint8_t     e_Flag_Enabled;
} AFStats_HostZoneConfig_ts;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \struct    AFStats_HostZoneStatus_ts
 * \brief     Host Programmable Zone System , where Host deciode the AF Zone Size .
 * \ingroup   AFStats
 * \endif
 */
typedef struct
{
    /// focus Measure for each zone when valid for AF .
    uint32_t    u32_Focus;

    /// current Host Programmed Start X with in WOI X size .
    uint32_t    u32_AFZoneStartX;

    /// current Host Programmed Start Y with in WOI Y size .
    uint32_t    u32_AFZoneStartY;

    /// current Host Programmed End  X with in WOI X size .
    uint32_t    u32_AFZoneEndX;

    /// current Host Programmed End Y with in WOI Y size .
    uint32_t    u32_AFZoneEndY;

    /// Width of the zone
    uint32_t    u32_AFZonesWidth;

    /// Height of the zone
    uint32_t    u32_AFZonesHeight;

    /// Light Measure for each zone when valid for AF .
    uint32_t    u32_Light;

    /// weight assigned to zone .
    uint32_t    u32_WeightAssigned;

    /// enable disable host zone
    uint32_t    u32_Enabled;
} AFStats_HostZoneStatus_ts;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \struct    AFStats_ZoneHWStatus_ts
 * \brief     Actual HW Coordinates that is programmed in the HW registers for Zone .
 * \ingroup   AFStats
 * \endif
 */
typedef struct
{
    /// current HW Programmed Start X with in WOI X size .
    uint16_t    u16_AFStartX;

    /// current Host Programmed Start Y with in WOI Y size .
    uint16_t    u16_AFStartY;

    /// current Host Programmed End  X with in WOI X size .
    uint16_t    u16_AFEndX;

    /// current Host Programmed End Y with in WOI Y size .
    uint16_t    u16_AFEndY;
} AFStats_ZoneHWStatus_ts;

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \struct    AFStats_ZoneVectorBase_ts
 * \brief     Zone Size structure .
 * \ingroup   AFStats
 * \endif
 */
typedef struct
{
    uint32_t    u32_Focus;
    uint16_t    u32_Light;
    uint8_t     u8_FrameInterruptCycle;
} AFStats_ZoneVectorBase_ts;

/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \struct      AFStats_Debug_ts       .
 * \brief       For debugging and issue analysis.
 * \details
 * \ingroup     AFStats
 * \endif
*/
typedef struct
{
    /// Partha - TBD
    uint32_t    u32_dummy;
} AFStats_Debug_ts;

typedef struct
{
    AFStats_HostZoneStatus_ts        afStats[AFS_HW_STATS_ZONE_NUMBER];
    uint32_t                         afStatsValid;
    uint32_t                         afStatsLensPos;
    uint32_t                         afFrameId;
} AFStats_ExportFormat_ts;


/*Extern Declarations*/
extern AFStats_Status_ts                    g_AFStats_Status TO_EXT_DATA_MEM;
extern AFStats_Controls_ts                  g_AFStats_Controls TO_EXT_DATA_MEM;
extern AFStats_AFZoneInterrupt_ts           g_AFStats_AFZoneInt TO_EXT_DATA_MEM;
extern AFStats_ZoneVectorBase_ts            g_AFStats_HWZoneVector[AFS_HW_STATS_ZONE_NUMBER] TO_EXT_DATA_MEM;
extern AFStats_Debug_ts                     g_AFStats_Debug TO_EXT_DATA_MEM;
extern AFStats_HostZoneConfig_ts            g_AFStats_HostZoneConfig[AFS_HW_STATS_ZONE_NUMBER] TO_EXT_DATA_MEM;
extern AFStats_HostZoneConfigPercentage_ts  g_AFStats_HostZoneConfigPercentage[AFS_HW_STATS_ZONE_NUMBER] TO_EXT_DATA_MEM;
extern AFStats_HostZoneStatus_ts            g_AFStats_HostZoneStatus[AFS_HW_STATS_ZONE_NUMBER] TO_EXT_DATA_MEM;
extern AFStats_ZoneHWStatus_ts              g_AFStats_ZoneHWStatus[AFS_HW_STATS_ZONE_NUMBER] TO_EXT_DATA_MEM;

extern void AFStats_SingleZoneStatsReadyISR (uint8_t);

#      define Zone_Width                                                  0 /*AFSTATS_DEFAULT_AF_ZONES_WIDTH*/
#      define Zone_Height                                                 0 /*AFSTATS_DEFAULT_AF_ZONES_HEIGHT*/
#      define Zone0_StartY                                                0 /*(WOI_CenterY - (Zone_Height >> 1))*/

#      define AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_X(ZoneValueX)    0 /*((float_t)((float_t)(100 * ZoneValueX)/(float_t)AFSTATS_DEFAULT_WOI_WIDTH))*/
#      define AFSTATS_DEFAULT_HOST_ZONE_PER_WRT_WOI_SIZE_Y(ZoneValueY)    0 /*((float_t)((float_t)(100 * ZoneValueY)/(float_t)AFSTATS_DEFAULT_WOI_HEIGHT))*/

/*[PM]Reference repository, kept for future*/
#if 0
#      define Zone_Width                                                  0 /*AFSTATS_DEFAULT_AF_ZONES_WIDTH*/
#      define Zone_Height                                                 0 /*AFSTATS_DEFAULT_AF_ZONES_HEIGHT*/
#      define WOI_CenterX                                                 0 /*(AFSTATS_DEFAULT_WOI_WIDTH >> 1)*/
#      define WOI_CenterY                                                 0 /*(AFSTATS_DEFAULT_WOI_HEIGHT >> 1)*/
#      define Zone0_StartX                                                0 /*(WOI_CenterX - (Zone_Width >> 1))*/
#      define Zone0_StartY                                                0 /*(WOI_CenterY - (Zone_Height >> 1))*/
#endif
#endif //AFSTATS_H_

