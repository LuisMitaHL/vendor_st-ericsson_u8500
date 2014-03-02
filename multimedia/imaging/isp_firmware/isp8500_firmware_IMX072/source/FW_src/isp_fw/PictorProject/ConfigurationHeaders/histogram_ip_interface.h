/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file    histogram_ip_interface.h
\brief   Provides an input interface for control of the histogram block.
\ingroup HStats
*/
#ifndef _HISTOGRAM_IP_INTERFACE_H_
#   define _HISTOGRAM_IP_INTERFACE_H_

#   include "PictorhwReg.h"
#   include "histogram_platform_specific.h"
#   include "FrameDimension_op_interface.h"
#   include "Zoom_OPInterface.h"
#   include "SystemConfig.h"
#   include "EventManager.h"

/// The stats will always be valid except in the case of Flash.
/// When Flash is also being used, then the Valid Stats will be those
/// which has the pre flash or main flash fired in them.
#if 0
#define Histogram_AreStatsValid()                      (        \
                                                          ( \
                                                             (SYSTEM_CONFIG_PARAMS_ABSORBED())  \
                                                             || (g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug != g_SystemConfig_Status.e_Coin_Glace_Histogram_Status)  \
                                                             || (g_HistStats_Status.e_CoinStatus != g_HistStats_Ctrl.e_CoinCtrl_debug)   \
                                                           ) \
                                                           &&  \
                                                           (!(GlaceHistogramStatus_HistogramDone & g_GlaceHistogramStatsFrozen))   \
                                                        )
#endif

/// Interface to program the mux for data input
#   define Hist_SetDataInput(IP_ENABLE, IP_SOFT_ENABLE) \
    Set_ISP_STATS_MUX_HIST_ISP_STATS_MUX_HIST_ENABLE(   \
    IP_ENABLE,                                          \
    IP_SOFT_ENABLE)

/// Interface for triggering an internal interrupt in the stxp70
#   define Histogram_ScheduleExport()  STXP70_TriggerInterrupt(IT_NO_HISTOGRAM_SCHEDULE)

/// Interface to notify the host that the Histogram statistics are available
#   define Histogram_StatsReadyNotify()        EventManager_HistogramStatsReady_Notify()

/// Interface to return the minimum of x or y
#   define Histogram_Min(x, y) min(x, y)

/// Interface to query the current prescale factor applied in the sensor
#   define Histogram_GetPreScale() FrameDimension_GetCurrentPreScaleFactor()

/// Interface to selsct the input src to the block
#   define Set_HIST_MUX_SELECT(HistInputSrc, ShadowEn) \
    Set_ISP_STATS_MUX_HIST_ISP_STATS_MUX_HIST_SELECT(  \
    HistInputSrc,                                      \
    ShadowEn)

/// Interface to enable the IP
#   define Set_HIST_ENABLE(Enable, SoftResest) Set_ISP_STATS_HIST_ISP_STATS_HIST_ENABLE(Enable, SoftResest)

/// Interface to give command to the hw block
#   define Set_HIST_CMD(HistCmd)   Set_ISP_STATS_HIST_ISP_STATS_HIST_CMD(HistCmd)

/// Interface to set the X size of Region Of Interest
#   define Set_HIST_SIZE_X(HistSizeX)  Set_ISP_STATS_HIST_ISP_STATS_HIST_X_SIZE(HistSizeX)

/// Interface to set the Y size of Region Of Interest
#   define Set_HIST_SIZE_Y(HistSizeY)  Set_ISP_STATS_HIST_ISP_STATS_HIST_Y_SIZE(HistSizeY)

/// Interface to set the offset in X dimension for Region Of Interest
#   define Set_HIST_OFFSET_X(HistOffsetX)  Set_ISP_STATS_HIST_ISP_STATS_HIST_X_OFFSET(HistOffsetX)

/// Interface to set the offset in Y dimension for Region Of Interest
#   define Set_HIST_OFFSET_Y(HistOffsetY)  Set_ISP_STATS_HIST_ISP_STATS_HIST_Y_OFFSET(HistOffsetY)

/// Interface to set the shift in the input pixel
/// Ths control is used to control the number of bins to be used in the hw block
#   define Set_HIST_PIXEL_SHIFT(HistPixelInputShift)   Set_ISP_STATS_HIST_ISP_STATS_HIST_PIXELIN_SHIFT(HistPixelInputShift)

/// Interface to get the Darkest Bin for G channel
#   define Get_HIST_DARKEST_G()    Get_ISP_STATS_HIST_ISP_STATS_HIST_DARKEST_G()

/// Interface to get the Brighest Bin for G channel
#   define Get_HIST_BRIGHTEST_G()  Get_ISP_STATS_HIST_ISP_STATS_HIST_BRIGHTEST_G()

/// Interface to get the Highest Bin for G channel
#   define Get_HIST_HIGHEST_G()    Get_ISP_STATS_HIST_ISP_STATS_HIST_HIGHEST_G()

/// Interface to get the Darkest Bin for R channel
#   define Get_HIST_DARKEST_R()    Get_ISP_STATS_HIST_ISP_STATS_HIST_DARKEST_R()

/// Interface to get the Brighest Bin for R channel
#   define Get_HIST_BRIGHTEST_R()  Get_ISP_STATS_HIST_ISP_STATS_HIST_BRIGHTEST_R()

/// Interface to get the Highest Bin for R channel
#   define Get_HIST_HIGHEST_R()    Get_ISP_STATS_HIST_ISP_STATS_HIST_HIGHEST_R()

/// Interface to get the Darkest Bin for B channel
#   define Get_HIST_DARKEST_B()    Get_ISP_STATS_HIST_ISP_STATS_HIST_DARKEST_B()

/// Interface to get the Brighest Bin for B channel
#   define Get_HIST_BRIGHTEST_B()  Get_ISP_STATS_HIST_ISP_STATS_HIST_BRIGHTEST_B()

/// Interface to get the Highest Bin for B channel
#   define Get_HIST_HIGHEST_B()    Get_ISP_STATS_HIST_ISP_STATS_HIST_HIGHEST_B()

#if 1
/// Interface to query the current horizontal user field of view
#   define Histogram_Get_FOV_X()   (g_Zoom_Status.f_FOVX)

/// Interface to query the current vertical user field of view
#   define Histogram_Get_FOV_Y()   (g_Zoom_Status.f_FOVY)

#else

/// Interface to query the current horizontal user field of view
//#   define Histogram_Get_FOV_X()   Zoom_GetCurrentFOVX()
#   define Histogram_Get_FOV_X()   \
        (SystemConfig_IsPipe0Active() ? Zoom_GetPipe0FOVX() : Zoom_GetPipe1FOVX())

/// Interface to query the current vertical user field of view
//#   define Histogram_Get_FOV_Y()   Zoom_GetCurrentFOVY()
#   define Histogram_Get_FOV_Y()   \
        (SystemConfig_IsPipe0Active() ? Zoom_GetPipe0FOVY() : Zoom_GetPipe1FOVY())

#endif

#if 1
/// Interface to query the X start of the user visible FOV within the frame being output by the sensor
/// Basically this should be the value of crop_h_start programmed in the ISP
#define Histogram_GetFOV_OffsetX() ((g_Zoom_Status_LLA.u16_woi_resX - g_Zoom_Status.f_FOVX)/(FrameDimension_GetCurrentPreScaleFactor()*2.0))


/// Interface to query the Y start of the user visible FOV within the frame being output by the sensor
/// Basically this should be the value of crop_v_start programmed in the ISP
#   define Histogram_GetFOV_OffsetY() ((g_Zoom_Status_LLA.u16_woi_resY - g_Zoom_Status.f_FOVY)/(FrameDimension_GetCurrentPreScaleFactor()*2.0))

/// Interface to query the actual X size of frame coming from the sensor
#   define Histogram_GetInputSizeX()   (g_Zoom_Status_LLA.u16_output_resX)

/// Interface to query the actual Y size of frame coming from the sensor
#   define Histogram_GetInputSizeY()   (g_Zoom_Status_LLA.u16_output_resY)
#else
/// Interface to query the X start of the user visible FOV within the frame being output by the sensor
/// Basically this should be the value of crop_h_start programmed in the ISP
#   define Histogram_GetFOV_OffsetX() \
        (SystemConfig_IsPipe0Active() ? Get_ISP_CE0_CROP_ISP_CE0_CROP_H_START() : (SystemConfig_IsPipe1Active()?Get_ISP_CE1_CROP_ISP_CE1_CROP_H_START(): BMS_CROP_XSTART()))

/// Interface to query the Y start of the user visible FOV within the frame being output by the sensor
/// Basically this should be the value of crop_v_start programmed in the ISP
#   define Histogram_GetFOV_OffsetY() \
        (SystemConfig_IsPipe0Active() ? Get_ISP_CE0_CROP_ISP_CE0_CROP_V_START() : (SystemConfig_IsPipe1Active() ? Get_ISP_CE1_CROP_ISP_CE1_CROP_V_START(): BMS_CROP_YSTART()))   

/// Interface to query the actual X size of frame coming from the sensor
#   define Histogram_GetInputSizeX()   FrameDimension_GetCurrentOPXOutputSize()

/// Interface to query the actual Y size of frame coming from the sensor
#   define Histogram_GetInputSizeY()   FrameDimension_GetCurrentOPYOutputSize()

#endif     
#endif // _HISTOGRAM_IP_INTERFACE_H_

