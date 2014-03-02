/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file AFStats_IPInterface.h
 \brief

 \note
 \ingroup AFStats
 */
#ifndef AFSTATS_IPINTERFACE_H_
#   define AFSTATS_IPINTERFACE_H_

// Files to be included at the Time of integration.
#   include "PictorhwReg.h"
#   include "videotiming_op_interface.h"
#   include "FocusControl_OPInterface.h"
#   include "FLADriver_OPInterface.h"
#   include "EventManager.h"
#   include "ITM.h"
#   include "SystemConfig.h"

#   define AFStats_GetCsiRawFormat()   VideoTiming_GetCsiRawFormat()

// ITM AF STATUS
#   define GET_AFSTATS_ITM_AFZONE_0_STATUS()       Get_ITM_ITM_AF_STATUS_AF_0_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_1_STATUS()       Get_ITM_ITM_AF_STATUS_AF_1_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_2_STATUS()       Get_ITM_ITM_AF_STATUS_AF_2_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_3_STATUS()       Get_ITM_ITM_AF_STATUS_AF_3_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_4_STATUS()       Get_ITM_ITM_AF_STATUS_AF_4_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_5_STATUS()       Get_ITM_ITM_AF_STATUS_AF_5_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_6_STATUS()       Get_ITM_ITM_AF_STATUS_AF_6_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_7_STATUS()       Get_ITM_ITM_AF_STATUS_AF_7_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_8_STATUS()       Get_ITM_ITM_AF_STATUS_AF_8_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_9_STATUS()       Get_ITM_ITM_AF_STATUS_AF_9_STATUS()
#   define GET_AFSTATS_ITM_AFZONE_STATUS()         Get_ITM_ITM_AF_STATUS()
#   define SET_AFSTATS_ITM_AFZONE_0_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_0_STATUS_BCLR(x)
#   define SET_AFSTATS_ITM_AFZONE_1_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_1_STATUS_BCLR(x)
#   define SET_AFSTATS_ITM_AFZONE_2_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_2_STATUS_BCLR(x)
#   define SET_AFSTATS_ITM_AFZONE_3_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_3_STATUS_BCLR(x)
#   define SET_AFSTATS_ITM_AFZONE_4_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_4_STATUS_BCLR(x)
#   define SET_AFSTATS_ITM_AFZONE_5_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_5_STATUS_BCLR(x)
#   define SET_AFSTATS_ITM_AFZONE_6_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_6_STATUS_BCLR(x)
#   define SET_AFSTATS_ITM_AFZONE_7_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_7_STATUS_BCLR(x)
#   define SET_AFSTATS_ITM_AFZONE_8_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_8_STATUS_BCLR(x)
#   define SET_AFSTATS_ITM_AFZONE_9_STATUS_BCLR(x) Set_ITM_ITM_AF_STATUS_BCLR_AF_9_STATUS_BCLR(x)
#   define GET_AFSTATS_ITM_AF_EN_STATUS()          Get_ITM_ITM_AF_EN_STATUS()
#   define SET_AFSTATS_ITM_AF_STATUS_BSET                                                                              \
        (af_0_bset, af_1_bset, af_2_bset, af_3_bset, af_4_bset, af_5_bset, af_6_bset, af_7_bset, af_8_bset, af_9_bset) \
        Set_ITM_ITM_AF_STATUS_BSET(                                                                                    \
        af_0_bset,                                                                                                     \
        af_1_bset,                                                                                                     \
        af_2_bset,                                                                                                     \
        af_3_bset,                                                                                                     \
        af_4_bset,                                                                                                     \
        af_5_bset,                                                                                                     \
        af_6_bset,                                                                                                     \
        af_7_bset,                                                                                                     \
        af_8_bset,                                                                                                     \
        af_9_bset)
#   define GET_AFSTATS_ITM_AF_STATUS_BSET()    Get_ITM_ITM_AF_STATUS_BSET()
#   define SET_AFSTATS_ITM_AF_STATUS_BCLR                                                                              \
        (af_0_bclr, af_1_bclr, af_2_bclr, af_3_bclr, af_4_bclr, af_5_bclr, af_6_bclr, af_7_bclr, af_8_bclr, af_9_bclr) \
        Set_ITM_ITM_AF_STATUS_BCLR(                                                                                    \
        af_0_bclr,                                                                                                     \
        af_1_bclr,                                                                                                     \
        af_2_bclr,                                                                                                     \
        af_3_bclr,                                                                                                     \
        af_4_bclr,                                                                                                     \
        af_5_bclr,                                                                                                     \
        af_6_bclr,                                                                                                     \
        af_7_bclr,                                                                                                     \
        af_8_bclr,                                                                                                     \
        af_9_bclr)
#   define SET_AFSTATS_ITM_AF_7_ZONE_INTERRUPT()       SET_AFSTATS_ITM_AF_STATUS_BSET(1, 1, 1, 1, 1, 1, 1, 0, 0, 0)
#   define SET_AFSTATS_ITM_AF_CLEAR_7_ZONE_INTERRUPT() SET_AFSTATS_ITM_AF_STATUS_BCLR(1, 1, 1, 1, 1, 1, 1, 0, 0, 0)

//AF_ENABLE
#   define SET_ISP_AF_STATS_CTRL_Enable()                  Set_ISP_STATS_AF_ISP_STATS_AF_ENABLE_enable__ENABLE()
#   define SET_ISP_AF_STATS_CTRL_Disable()                 Set_ISP_STATS_AF_ISP_STATS_AF_ENABLE_enable__DISABLE()
#   define SET_ISP_AF_STATS_ZONE_CTRL_CoringDisable()      Set_ISP_STATS_AF_ISP_STATS_AF_STATS_ZONE_CTRL_coring_en__B_0x0()
#   define SET_ISP_AF_STATS_ZONE_CTRL_CoringEnable()       Set_ISP_STATS_AF_ISP_STATS_AF_STATS_ZONE_CTRL_coring_en__B_0x1()
#   define SET_ISP_AF_STATS_CORING_value(x)                Set_ISP_STATS_AF_ISP_STATS_AF_CORING_coring_value(x)
#   define SET_ISP_AF_STATS_ZONE_CTRL_AbsSquareDisable()   Set_ISP_STATS_AF_ISP_STATS_AF_STATS_ZONE_CTRL_abs_square_en__B_0x0()
#   define SET_ISP_AF_STATS_ZONE_CTRL_AbsSquareEnable()    Set_ISP_STATS_AF_ISP_STATS_AF_STATS_ZONE_CTRL_coring_en__B_0x1()
#   define SET_ISP_AF_STATS_ZONE_CTRL_LightStatsEnable() \
        Set_ISP_STATS_AF_ISP_STATS_AF_STATS_ZONE_CTRL_light_change_stats_en__Enable()
#   define SET_ISP_AF_STATS_ZONE_IntEnable(zone_enable)    Set_ISP_STATS_AF_ISP_STATS_AF_STATS_ZONE_ENABLE(zone_enable)
#   define SET_ISP_AF_STATS_ZONE_IntEnable_ENABLE()        Set_ISP_STATS_AF_ISP_STATS_AF_STATS_ZONE_ENABLE_zone_enable__Enable()
#   define SET_ISP_AF_STATS_ZONE_ENABLE_Word(x)            Set_ISP_STATS_AF_ISP_STATS_AF_STATS_ZONE_ENABLE_word(x)
#   define SET_ISP_AF_STATS_DFV_Dfv_Enable()               Set_ISP_STATS_AF_ISP_STATS_AF_DFV_dfv_enable__ENABLE()
#   define SET_ISP_AF_STATS_DFV_Dfv_Disable()              Set_ISP_STATS_AF_ISP_STATS_AF_DFV_dfv_enable__DISABLE()
#   define SET_ISP_AF_STATS_DFV_6_InterruptCtrl(x)         Set_ISP_STATS_AF_ISP_STATS_AF_DFV_dfv_6_interrupt_ctrl(x)
#   define GET_ISP_AF_STATS_DFV_6_InterruptCtrl()          Get_ISP_STATS_AF_ISP_STATS_AF_DFV_dfv_6_interrupt_ctrl()
#   define SET_ISP_AF_STATS_DFV_8_InterruptCtrl(x)         Set_ISP_STATS_AF_ISP_STATS_AF_DFV_dfv_8_interrupt_ctrl(x)
#   define GET_ISP_AF_STATS_DFV_InterruptCtrl()            Get_ISP_STATS_AF_ISP_STATS_AF_DFV()
#   define SET_ISP_AF_STATS_DFV_InterruptCtrl                                                                   \
        (                                                                                                       \
            dfv_enable, dfv_0_interrupt_ctrl, dfv_1_interrupt_ctrl, dfv_2_interrupt_ctrl, dfv_3_interrupt_ctrl, \
                dfv_4_interrupt_ctrl, dfv_5_interrupt_ctrl, dfv_6_interrupt_ctrl, dfv_7_interrupt_ctrl,         \
                    dfv_8_interrupt_ctrl, dfv_9_interrupt_ctrl                                                  \
        )                                                                                                       \
        Set_ISP_STATS_AF_ISP_STATS_AF_DFV(                                                                      \
        dfv_enable,                                                                                             \
        dfv_0_interrupt_ctrl,                                                                                   \
        dfv_1_interrupt_ctrl,                                                                                   \
        dfv_2_interrupt_ctrl,                                                                                   \
        dfv_3_interrupt_ctrl,                                                                                   \
        dfv_4_interrupt_ctrl,                                                                                   \
        dfv_5_interrupt_ctrl,                                                                                   \
        dfv_6_interrupt_ctrl,                                                                                   \
        dfv_7_interrupt_ctrl,                                                                                   \
        dfv_8_interrupt_ctrl,                                                                                   \
        dfv_9_interrupt_ctrl)
#   define SET_ISP_AF_STATS_HIST_DFV_word(x)               Set_ISP_STATS_HIST_ISP_STATS_HIST_DFV_word(x)
#   define SET_ISP_AF_STATS_CLEAR_INTR_STATS_AutoFocus()   ITM_Clear_Interrupt_STAT2_Auto_Focus()

// Zone configuration Definesl

//ISP_STATS_AF_ZONE_X_START_OFFSET_0
#   define GET_AF_STATS_ZONE_X_START_OFFSET_0_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_0_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_0_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_0_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_0_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_0_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_0_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_0_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_0()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_0_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_0() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_0_acc_value_light_change()

//ISP_STATS_AF_ZONE_X_START_OFFSET_1
#   define GET_AF_STATS_ZONE_X_START_OFFSET_1_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_1_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_1_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_1_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_1_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_1_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_1_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_1_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_1()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_1_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_1() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_1_acc_value_light_change()

//ISP_STATS_AF_ZONE_X_START_OFFSET_2
#   define GET_AF_STATS_ZONE_X_START_OFFSET_2_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_2_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_2_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_2_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_2_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_2_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_2_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_2_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_2()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_2_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_2() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_2_acc_value_light_change()

//ISP_STATS_AF_ZONE_X_START_OFFSET_3
#   define GET_AF_STATS_ZONE_X_START_OFFSET_3_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_3_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_3_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_3_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_3_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_3_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_3_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_3_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_3()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_3_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_3() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_3_acc_value_light_change()

//ISP_STATS_AF_ZONE_X_START_OFFSET_4
#   define GET_AF_STATS_ZONE_X_START_OFFSET_4_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_4_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_4_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_4_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_4_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_4_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_4_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_4_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_4()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_4_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_4() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_4_acc_value_light_change()

//ISP_STATS_AF_ZONE_X_START_OFFSET_5
#   define GET_AF_STATS_ZONE_X_START_OFFSET_5_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_5_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_5_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_5_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_5_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_5_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_5_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_5_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_5()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_5_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_5() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_5_acc_value_light_change()

//ISP_STATS_AF_ZONE_X_START_OFFSET_6
#   define GET_AF_STATS_ZONE_X_START_OFFSET_6_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_6_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_6_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_6_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_6_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_6_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_6_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_6_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_6()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_6_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_6() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_6_acc_value_light_change()
#   define GET_AF_STATS_ZONE_X_START_OFFSET_7_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_7_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_7_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_7_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_7_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_7_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_7_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_7_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_7()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_7_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_7() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_7_acc_value_light_change()
#   define GET_AF_STATS_ZONE_X_START_OFFSET_8_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_8_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_8_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_8_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_8_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_8_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_8_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_8_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_8()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_8_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_8() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_8_acc_value_light_change()
#   define GET_AF_STATS_ZONE_X_START_OFFSET_9_x_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_9_x_start()
#   define GET_AF_STATS_ZONE_X_END_OFFSET_9_x_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_9_x_end()
#   define SET_AF_STATS_ZONE_X_START_OFFSET_9_x_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_START_OFFSET_9_x_start(x)
#   define SET_AF_STATS_ZONE_X_END_OFFSET_9_x_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_X_END_OFFSET_9_x_end(x)
#   define GET_AF_STATS_ZONE_ACC_VALUE_FOCUS_9()           Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_FOCUS_9_acc_value_focus()
#   define GET_AF_STATS_ZONE_ACC_VALUE_LIGHT_CHANGE_9() \
        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_ACC_VALUE_LIGHT_CHANGE_9_acc_value_light_change()

//ISP_STATS_AF_ZONE_Y_START_OFFSET_0
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_0_y_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_0_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_0_y_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_0_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_0_y_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_0_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_0_y_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_0_y_end(x)

//ISP_STATS_AF_ZONE_Y_START_OFFSET_1
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_1_y_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_1_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_1_y_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_1_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_1_y_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_1_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_1_y_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_1_y_end(x)

//ISP_STATS_AF_ZONE_Y_START_OFFSET_2
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_2_y_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_2_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_2_y_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_2_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_2_y_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_2_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_2_y_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_2_y_end(x)

//ISP_STATS_AF_ZONE_Y_START_OFFSET_3
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_3_y_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_3_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_3_y_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_3_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_3_y_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_3_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_3_y_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_3_y_end(x)

//ISP_STATS_AF_ZONE_Y_START_OFFSET_4
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_4_y_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_4_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_4_y_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_4_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_4_y_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_4_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_4_y_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_4_y_end(x)

//ISP_STATS_AF_ZONE_Y_START_OFFSET_5
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_5_y_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_5_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_5_y_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_5_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_5_y_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_5_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_5_y_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_5_y_end(x)

//ISP_STATS_AF_ZONE_Y_START_OFFSET_6
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_6_y_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_6_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_6_y_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_6_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_6_y_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_6_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_6_y_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_6_y_end(x)
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_7_y_start()    Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_7_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_7_y_end()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_7_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_7_y_start(x)   Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_7_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_7_y_end(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_7_y_end(x)

//ISP_STATS_AF_ZONE_Y_START_OFFSET_8
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_8_y_start()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_8_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_8_y_end()            Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_8_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_8_y_start(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_8_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_8_y_end(x)           Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_8_y_end(x)
#   define GET_AF_STATS_ZONE_Y_START_OFFSET_9_y_start()        Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_9_y_start()
#   define GET_AF_STATS_ZONE_Y_END_OFFSET_9_y_end()            Get_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_9_y_end()
#   define SET_AF_STATS_ZONE_Y_START_OFFSET_9_y_start(x)       Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_START_OFFSET_9_y_start(x)
#   define SET_AF_STATS_ZONE_Y_END_OFFSET_9_y_end(x)           Set_ISP_STATS_AF_ISP_STATS_AF_ZONE_Y_END_OFFSET_9_y_end(x)
#   define AFStats_FocusControlAFStatsReadyISR()               FocusControl_AFStatsReadyISR()

#   define AFStats_StatsReadyNotify()                          EventManager_AFStatsReady_Notify()
#   define AFStats_FLADriverGetLowLevelLensPosition()          FLADriver_GetLowLevelLensPosition()
#   define AFStats_FLADriverGetLensIsMoving()                  FLADriver_GetLensISMoving()



// the following macro define where i must read the image size
#   define _CROP_BLOCK_BEFORE_AFSTATS_ (0)


#if _CROP_BLOCK_BEFORE_AFSTATS_

	#    define GET_ISP_AF_STATS_CURRENT_WOI_H_Offset()            (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_H_START_crop_h_start():Get_ISP_CE1_CROP_ISP_CE1_CROP_H_START_crop_h_start())
	#    define GET_ISP_AF_STATS_CURRENT_WOI_V_Offset()            (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_V_START_crop_v_start():Get_ISP_CE1_CROP_ISP_CE1_CROP_V_START_crop_v_start())

	#    define GET_ISP_AF_STATS_CURRENT_WOI_V_Size()              (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_V_SIZE_crop_v_size():Get_ISP_CE1_CROP_ISP_CE1_CROP_V_SIZE_crop_v_size())
	#    define GET_ISP_AF_STATS_CURRENT_WOI_H_Size()              (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_H_SIZE_crop_h_size():Get_ISP_CE1_CROP_ISP_CE1_CROP_H_SIZE_crop_h_size())

#else

	#    define GET_ISP_AF_STATS_CURRENT_WOI_H_Offset()            (((g_Zoom_Status_LLA.u16_woi_resX - g_Zoom_Status.f_FOVX)/(FrameDimension_GetCurrentPreScaleFactor()*2.0)))
	#    define GET_ISP_AF_STATS_CURRENT_WOI_V_Offset()            (((g_Zoom_Status_LLA.u16_woi_resY - g_Zoom_Status.f_FOVY)/(FrameDimension_GetCurrentPreScaleFactor()*2.0)))

	#    define GET_ISP_AF_STATS_CURRENT_WOI_V_Size()              (g_Zoom_Status.f_FOVY/FrameDimension_GetCurrentPreScaleFactor())
	#    define GET_ISP_AF_STATS_CURRENT_WOI_H_Size()              (g_Zoom_Status.f_FOVX/FrameDimension_GetCurrentPreScaleFactor())

#endif


#   define GET_ISP_AF_STATS_CURRENT_WOI_H_BOUNDARY_check(Cord_X) \
        ((Cord_X < (g_AFStats_Status.u16_WOIWidth + GET_ISP_AF_STATS_CURRENT_WOI_H_Offset())) ? 1 : 0)
#   define GET_ISP_AF_STATS_CURRENT_WOI_V_BOUNDARY_check(Cord_Y) \
        ((Cord_Y < (g_AFStats_Status.u16_WOIHeight + GET_ISP_AF_STATS_CURRENT_WOI_V_Offset())) ? 1 : 0)

#   define AFStats_ForceStatsUpdate()      SystemConfig_IsZoomUpdateDone()
#   define AFStats_ZoomUpdateAbsorbed()    SystemConfig_ResetZoomUpdateDone()
#   define AFStats_IsFDMRequestPending()   Zoom_IsFDMRequestPending()

#   define AFStats_RequestedWithLensCommand() \
             (Flag_e_TRUE == FLADriver_AFStatsGetStatsWithLensMoveFWStatus())

#   define AFStats_RequestedWithoutLensCommand() \
          (g_AFStats_Status.e_Coin_AFStatsExportStatus != g_AFStats_Controls.e_Coin_AFStatsExportCmd)

#   define AFStats_Requested()  \
          ( AFStats_RequestedWithoutLensCommand() || AFStats_RequestedWithLensCommand() )

#   define Complete_AFStats_And_Notify_After_Stop() \
          ( \
           AFStats_Requested() \
           && \
          (Flag_e_FALSE == g_AFStats_Controls.e_Flag_AFStatsCancel) \
           && \
          (Flag_e_FALSE == g_AFStatsNotifyPending) \
          )


#endif /*AFSTATS_IPINTERFACE_H_*/

