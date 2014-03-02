/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef GLACE_IPINTERFACE_H_
#define GLACE_IPINTERFACE_H_

/**
 \file      Glace_IPInterface.h
  \brief    This file is not a part of the zoom module release.
            All inputs needed by the Glace module that can only
            be resolved at the project level (at integration time)
            are met through this file.
            It is the responsibility of the integrator to generate
            this file at integration time and meet all the input
            dependencies.
 \ingroup   Glace
*/


#include "Glace_PlatformSpecific.h"
#include "Stream.h"
#include "Platform.h"
#include "PictorhwReg.h"
#include "GenericFunctions.h"
#include "FrameDimension_op_interface.h"
#include "Zoom_OPInterface.h"
#include "EventManager.h"
#include "STXP70_OPInterface.h"
#include "InterruptManager.h"

#if 0
/// Interface to allow the module to know if the statistics are to be exported
#define Glace_IsStatsValid()                      (        \
                                                    ( \
                                                      (SYSTEM_CONFIG_PARAMS_ABSORBED())  \
                                                      || (g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug != g_SystemConfig_Status.e_Coin_Glace_Histogram_Status)  \
                                                      || (g_Glace_Status.u8_ControlUpdateCount != g_Glace_Control.u8_ControlUpdateCount_debug)                                   \
                                                      || (GlaceOperationMode_e_Continuous == g_Glace_Control.e_GlaceOperationMode_Control)                    \
                                                    ) \
                                                    &&  \
                                                    (!(GlaceHistogramStatus_GlaceDone & g_GlaceHistogramStatsFrozen))   \
                                                  )

#endif

/// Interface to allow the module to know if the device is streaming
#define Glace_IsStreaming()         Stream_IsISPInputStreaming()

/// Interface to query the actual X size of frame coming from the sensor
/// If any pipe is active use Framedimension, otherwise it is case of BMS. use g_zoom_status_LLA in this case
#define Glace_XInputSize()          (g_Zoom_Status_LLA.u16_output_resX)

/// Interface to query the actual Y size of frame coming from the sensor
/// If any pipe is active use Framedimension, otherwise it is case of BMS. use g_zoom_status_LLA in this case
#define Glace_YInputSize()          (g_Zoom_Status_LLA.u16_output_resY)

/// Interface to return log(to base y) of x
#define Glace_Log(x, y)             GenericFunctions_Log(x, y)

/// Interface to return the ceiling of fractional value x
#define Glace_Ceiling(x)            GenericFunctions_Ceiling(x)

#if 0
/// Interface to return the minimum of x or y
#define Glace_Min(x, y)             (((x)<(y)) ? (float)(x) : (float)(y))
#else
/// Interface to return the minimum of x or y
#define Glace_Min(x, y)             min(x, y)
#endif

/// Interface to program register ISP_STATS_GLACE_H_ROI_START
#define Glace_Set_H_ROI_START(x)    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_H_ROI_START(x)

/// Interface to program register ISP_STATS_GLACE_V_ROI_START
#define Glace_Set_V_ROI_START(x)    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_V_ROI_START(x)

/// Interface to program register ISP_STATS_GLACE_H_BLK_SIZE
#define Glace_Set_H_BLK_SIZE(x)     Set_ISP_STATS_GLACE_ISP_STATS_GLACE_H_BLK_SIZE(x)

/// Interface to program register ISP_STATS_GLACE_V_BLK_SIZE
#define Glace_Set_V_BLK_SIZE(x)     Set_ISP_STATS_GLACE_ISP_STATS_GLACE_V_BLK_SIZE(x)

/// Interface to program register ISP_STATS_GLACE_H_GRID_SIZE
#define Glace_Set_H_GRID_SIZE(x)    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_H_GRID_SIZE(x)

/// Interface to program register ISP_STATS_GLACE_V_GRID_SIZE
#define Glace_Set_V_GRID_SIZE(x)    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_V_GRID_SIZE(x)

/// Interface to program register ISP_STATS_GLACE_SAT_LEVEL_R
#define Glace_Set_SAT_LEVEL_R(x)    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_SAT_LEVEL_R(x)

/// Interface to program register ISP_STATS_GLACE_SAT_LEVEL_G
#define Glace_Set_SAT_LEVEL_G(x)    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_SAT_LEVEL_G(x)

/// Interface to program register ISP_STATS_GLACE_SAT_LEVEL_B
#define Glace_Set_SAT_LEVEL_B(x)    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_SAT_LEVEL_B(x)

/// Interface to program register ISP_STATS_GLACE_AV_MULT
#define Glace_Set_AV_MULT(x)        Set_ISP_STATS_GLACE_ISP_STATS_GLACE_AV_MULT(x)

/// Interface to program register ISP_STATS_GLACE_AV_SHIFT
#define Glace_Set_AV_SHIFT(x)       Set_ISP_STATS_GLACE_ISP_STATS_GLACE_AV_SHIFT(x)

/// Interface to enable Glace accumulation (ISP_STATS_GLACE_ACC_ENABLE.GLACE_ACC_ENABLE)
#define Glace_Enable()              Set_ISP_STATS_GLACE_ISP_STATS_GLACE_ACC_ENABLE(GLACE_ACC_ENABLE_ENABLE, SOFT_RESET_B_0x0)   // GLACE_ACC_ENABLE, SOFT_RESET

/// Interface to disable Glace accumulation (ISP_STATS_GLACE_ACC_ENABLE.GLACE_ACC_ENABLE)
#define Glace_Disable()             Set_ISP_STATS_GLACE_ISP_STATS_GLACE_ACC_ENABLE(GLACE_ACC_ENABLE_DISABLE, SOFT_RESET_B_0x0)  // GLACE_ACC_ENABLE, SOFT_RESET

/// Interface to reset the Glace IP
#define Glace_Reset()               Set_ISP_STATS_GLACE_ISP_STATS_GLACE_ACC_ENABLE_SOFT_RESET__B_0x1(); \
                                    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_ACC_ENABLE_SOFT_RESET__B_0x0();

/// Interface to disable data intput into the Glace IP
#define Glace_DisableDataInput()    Set_ISP_STATS_MUX_GLACE_ISP_STATS_MUX_GLACE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0)    // mux2to1_enable, mux2to1_soft_reset

/// Interface to enable data intput into the Glace IP
#define Glace_EnableDataInput()     Set_ISP_STATS_MUX_GLACE_ISP_STATS_MUX_GLACE_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0) // mux2to1_enable, mux2to1_soft_reset

/// Interface to select the Glace data source as output of Gridiron
#define Glace_Set_MUX_GLACE_SELECT_POST_LENS_SHADING()  Set_ISP_STATS_MUX_GLACE_ISP_STATS_MUX_GLACE_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1)    // mux2to1_select,mux2to1_shadow_en

/// Interface to select the Glace data source as output of Channel Gain
#define Glace_Set_MUX_GLACE_SELECT_POST_CHANNEL_GAINS() Set_ISP_STATS_MUX_GLACE_ISP_STATS_MUX_GLACE_SELECT(mux2to1_select_IDP1, mux2to1_shadow_en_B_0x1)    // mux2to1_select,mux2to1_shadow_en

/// Interface to allow the module to schedule the Glace export at a lower priority than the Glace Interrupt
#define Glace_ScheduleExport()      STXP70_TriggerInterrupt(IT_NO_GLACE_SCHEDULE)

/// Interface to notify the host that the glace statistics are available
#define Glace_StatsReadyNotify()    EventManager_GlaceStatsReady_Notify()

// Enable MPSS
#define Glace_Enable_MPSS()    Set_ISP_STATS_MPSS_GLACE_ISP_STATS_MPSS_GLACE_ENABLE_enable__B_0x1()

#define Glace_Get_FOV_X()  (g_Zoom_Status.f_FOVX)

#define Glace_Get_FOV_Y()  (g_Zoom_Status.f_FOVY)

/// Interface to query the current prescale factor applied in the sensor
#   define Glace_GetPreScale() FrameDimension_GetCurrentPreScaleFactor()

/// Interface to query the X start of the user visible FOV within the frame being output by the sensor
/// Basically this should be the value of crop_h_start programmed in the ISP
#if 1
#   define Glace_GetFOV_OffsetX() \
        ((g_Zoom_Status_LLA.u16_woi_resX - g_Zoom_Status.f_FOVX)/(FrameDimension_GetCurrentPreScaleFactor()*2.0))

/// Interface to query the Y start of the user visible FOV within the frame being output by the sensor
/// Basically this should be the value of crop_v_start programmed in the ISP
#   define Glace_GetFOV_OffsetY() \
        ((g_Zoom_Status_LLA.u16_woi_resY - g_Zoom_Status.f_FOVY)/(FrameDimension_GetCurrentPreScaleFactor()*2.0))
#else
#   define Glace_GetFOV_OffsetX() \
        (SystemConfig_IsPipe0Active() ? Get_ISP_CE0_CROP_ISP_CE0_CROP_H_START() : (SystemConfig_IsPipe1Active()?Get_ISP_CE1_CROP_ISP_CE1_CROP_H_START(): BMS_CROP_XSTART()))

/// Interface to query the Y start of the user visible FOV within the frame being output by the sensor
/// Basically this should be the value of crop_v_start programmed in the ISP
#   define Glace_GetFOV_OffsetY() \
        (SystemConfig_IsPipe0Active() ? Get_ISP_CE0_CROP_ISP_CE0_CROP_V_START() : (SystemConfig_IsPipe1Active() ? Get_ISP_CE1_CROP_ISP_CE1_CROP_V_START(): BMS_CROP_YSTART()))

#endif

#endif /*GLACE_IPINTERFACE_H_*/
