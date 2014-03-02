/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Control/Linux/ab9540_Control.c
 * 
 *
 * Generated on the 29/02/2012 09:14 by the 'super-cool' code generator 
 *
 * Generator written in Common Lisp, created by  Remi PRUD'HOMME - STEricsson 
 * with the help of : 
 *
 ***************************************************************************
 *  Copyright STEricsson  2012
 *
 *  Reference document : User Manual ref : CD00291561 Rev 3, May 2011
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_CONTROL_LINUX_AB9540_CONTROL_C
#define C__LISP_PRODUCTION_AB9540_AB9540_CONTROL_LINUX_AB9540_CONTROL_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Control.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_battery_detect
 *
 * RET  : Return the value of register 0xSystemContolStatus
 *
 * Notes : From register 0x100, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_detect()
  {
    unsigned char value;


    /* 
     * 0: PowerOnKey1Detect is not a turn on event 
     * 1: PowerOnKey1Detect is a turn on event 
     */ 
    value = (I2CRead(SYSTEM_CONTOL_STATUS_REG) & BATTERY_DETECT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_power_on_key1_detect
 *
 * RET  : Return the value of register 0xSystemContolStatus
 *
 * Notes : From register 0x100, bits 1:1
 *
 **************************************************************************/
unsigned char get_power_on_key1_detect()
  {
    unsigned char value;


    /* 
     * 0: PowerOnKey1Detect is not a turn on event 
     * 1: PowerOnKey1Detect is a turn on event 
     */ 
    value = (I2CRead(SYSTEM_CONTOL_STATUS_REG) & POWER_ON_KEY_1_DETECT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_power_on_key2_detect
 *
 * RET  : Return the value of register 0xSystemContolStatus
 *
 * Notes : From register 0x100, bits 2:2
 *
 **************************************************************************/
unsigned char get_power_on_key2_detect()
  {
    unsigned char value;


    /* 
     * 0: PowerOnKey2Detect is not a turn on event 
     * 1: PowerOnKey2Detect is a turn on event 
     */ 
    value = (I2CRead(SYSTEM_CONTOL_STATUS_REG) & POWER_ON_KEY_2_DETECT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_rtc_alarm_detect
 *
 * RET  : Return the value of register 0xSystemContolStatus
 *
 * Notes : From register 0x100, bits 3:3
 *
 **************************************************************************/
unsigned char get_rtc_alarm_detect()
  {
    unsigned char value;


    /* 
     * 0: RtcAlarmDetect is not a turn on event 
     * 1: RtcAlarmDetect is a turn on event 
     */ 
    value = (I2CRead(SYSTEM_CONTOL_STATUS_REG) & RTC_ALARM_DETECT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_vbus_dectect
 *
 * RET  : Return the value of register 0xSystemContolStatus
 *
 * Notes : From register 0x100, bits 5:5
 *
 **************************************************************************/
unsigned char get_vbus_dectect()
  {
    unsigned char value;


    /* 
     * 0: VbusDetect is not a turn on event 
     * 1: VbusDetect is a turn on event 
     */ 
    value = (I2CRead(SYSTEM_CONTOL_STATUS_REG) & VBUS_DECTECT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_id_detect
 *
 * RET  : Return the value of register 0xSystemContolStatus
 *
 * Notes : From register 0x100, bits 6:6
 *
 **************************************************************************/
unsigned char get_usb_id_detect()
  {
    unsigned char value;


    /* 
     * 0: UsbIDDetect is not a turn on event 
     * 1: UsbIDDetect is a turn on event 
     */ 
    value = (I2CRead(SYSTEM_CONTOL_STATUS_REG) & USB_ID_DETECT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_uart_factory_mode
 *
 * RET  : Return the value of register 0xSystemContolStatus
 *
 * Notes : From register 0x100, bits 7:7
 *
 **************************************************************************/
unsigned char get_uart_factory_mode()
  {
    unsigned char value;


    /* 
     * 0: Uart Factory Mode Detected is not a turn on event 
     * 1: Uart Factory Mode Detected is a turn on event 
     */ 
    value = (I2CRead(SYSTEM_CONTOL_STATUS_REG) & UART_FACTORY_MODE_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_power_on_key1_press_time
 *
 * RET  : Return the value of register 0xSystemControlStatus2
 *
 * Notes : From register 0x102, bits 6:0
 *
 **************************************************************************/
unsigned char get_power_on_key1_press_time()
  {
    unsigned char value;


    /* 
     * A 7 bit counter is used to measure PonKey1db pressure 
     * duration with steps 
     * of 125 msec and a range of 10 sec. PonKey1PressTime[6:0] 
     * indicates number of 125ms steps. 
     */ 
    value = (I2CRead(SYSTEM_CONTROL_STATUS_2_REG) & POWER_ON_KEY_1_PRESS_TIME_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_power_on_key1_press_time2
 *
 * IN   : param, a value to write to the regiter SoftwareControlFallback
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareControlFallback
 *
 * Notes : From register 0x103, bits 6:0
 *
 **************************************************************************/
unsigned char set_power_on_key1_press_time2( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_CONTROL_FALLBACK_REG);

    /* 
     * enable resetart when VBAT fallback when charging in SW 
     * mode 
     */ 
  
    value =  old_value & ~POWER_ON_KEY_1_PRESS_TIME_2_MASK;


    value |=  param ;  

    I2CWrite(SOFTWARE_CONTROL_FALLBACK_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_power_on_key1_press_time2
 *
 * RET  : Return the value of register 0xSoftwareControlFallback
 *
 * Notes : From register 0x103, bits 6:0
 *
 **************************************************************************/
unsigned char get_power_on_key1_press_time2()
  {
    unsigned char value;


    /* 
     * enable resetart when VBAT fallback when charging in SW 
     * mode 
     */ 
    value = (I2CRead(SOFTWARE_CONTROL_FALLBACK_REG) & POWER_ON_KEY_1_PRESS_TIME_2_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req
 *
 * RET  : Return the value of register 0xSysClkReq
 *
 * Notes : From register 0x142, bits 7:0
 *
 **************************************************************************/
unsigned char get_sys_clk_req()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(SYS_CLK_REQ_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_ctrl_def0_spare0
 *
 * RET  : Return the value of register 0xCtrlDef0Spare0
 *
 * Notes : From register 0x150, bits 7:0
 *
 **************************************************************************/
unsigned char get_ctrl_def0_spare0()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(CTRL_DEF_0_SPARE_0_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ctrl_def0_spare0
 *
 * IN   : param, a value to write to the regiter CtrlDef0Spare0
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtrlDef0Spare0
 *
 * Notes : From register 0x150, bits 7:0
 *
 **************************************************************************/
unsigned char set_ctrl_def0_spare0( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(CTRL_DEF_0_SPARE_0_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(CTRL_DEF_0_SPARE_0_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_ctrl_def0_spare1
 *
 * RET  : Return the value of register 0xCtrlDef0Spare1
 *
 * Notes : From register 0x151, bits 7:0
 *
 **************************************************************************/
unsigned char get_ctrl_def0_spare1()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(CTRL_DEF_0_SPARE_1_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ctrl_def0_spare1
 *
 * IN   : param, a value to write to the regiter CtrlDef0Spare1
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtrlDef0Spare1
 *
 * Notes : From register 0x151, bits 7:0
 *
 **************************************************************************/
unsigned char set_ctrl_def0_spare1( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(CTRL_DEF_0_SPARE_1_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(CTRL_DEF_0_SPARE_1_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_ctrl_def0_spare2
 *
 * RET  : Return the value of register 0xCtrlDef0Spare2
 *
 * Notes : From register 0x152, bits 7:0
 *
 **************************************************************************/
unsigned char get_ctrl_def0_spare2()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(CTRL_DEF_0_SPARE_2_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ctrl_def0_spare2
 *
 * IN   : param, a value to write to the regiter CtrlDef0Spare2
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtrlDef0Spare2
 *
 * Notes : From register 0x152, bits 7:0
 *
 **************************************************************************/
unsigned char set_ctrl_def0_spare2( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(CTRL_DEF_0_SPARE_2_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(CTRL_DEF_0_SPARE_2_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_ctrl_def1_spare3
 *
 * RET  : Return the value of register 0xCtrlDef1Spare3
 *
 * Notes : From register 0x153, bits 7:0
 *
 **************************************************************************/
unsigned char get_ctrl_def1_spare3()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(CTRL_DEF_1_SPARE_3_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ctrl_def1_spare3
 *
 * IN   : param, a value to write to the regiter CtrlDef1Spare3
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtrlDef1Spare3
 *
 * Notes : From register 0x153, bits 7:0
 *
 **************************************************************************/
unsigned char set_ctrl_def1_spare3( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(CTRL_DEF_1_SPARE_3_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(CTRL_DEF_1_SPARE_3_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_pull_down_usbu_icc
 *
 * IN   : param, a value to write to the regiter CtlSpare5Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare5Fun
 *
 * Notes : From register 0x154, bits 0:0
 *
 **************************************************************************/
unsigned char set_pull_down_usbu_icc( enum pull_down_usbu_icc param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_5_FUN_REG);

    /* 
     * Pull Down of pads usbuicc 
     */ 
    switch( param ){
        case PULLDOWN_USBU_ICC_ON_E: 
           value = old_value | PULL_DOWN_USBU_ICC_MASK; 
           break;
        case PULLDOWN_USBU_ICC_OFF_E: 
           value = old_value & ~ PULL_DOWN_USBU_ICC_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_5_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pull_down_usbu_icc
 *
 * RET  : Return the value of register 0xCtlSpare5Fun
 *
 * Notes : From register 0x154, bits 0:0
 *
 **************************************************************************/
unsigned char get_pull_down_usbu_icc()
  {
    unsigned char value;


    /* 
     * Pull Down of pads usbuicc 
     */ 
    value = (I2CRead(CTL_SPARE_5_FUN_REG) & PULL_DOWN_USBU_ICC_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux5_selection
 *
 * IN   : param, a value to write to the regiter CtlSpare6Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 2:0
 *
 **************************************************************************/
unsigned char set_vaux5_selection( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_6_FUN_REG);

    /* 
     * Vaux5Selection 
     */ 
  
    value =  old_value & ~VAUX_5_SELECTION_MASK;


    value |=  param ;  

    I2CWrite(CTL_SPARE_6_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux5_selection
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 2:0
 *
 **************************************************************************/
unsigned char get_vaux5_selection()
  {
    unsigned char value;


    /* 
     * Vaux5Selection 
     */ 
    value = (I2CRead(CTL_SPARE_6_FUN_REG) & VAUX_5_SELECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux5_lowper
 *
 * IN   : param, a value to write to the regiter CtlSpare6Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 3:3
 *
 **************************************************************************/
unsigned char set_vaux5_lowper( enum vaux5_lowper_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_6_FUN_REG);

    /* 
     * Vaux5Lowper 
     */ 
    switch( param ){
        case VAUX_5_LOWPER_ENABLE_E: 
           value = old_value | VAUX_5_LOWPER_MASK; 
           break;
        case VAUX_5_LOWPER_NO_ACTION_E: 
           value = old_value & ~ VAUX_5_LOWPER_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_6_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux5_lowper
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 3:3
 *
 **************************************************************************/
unsigned char get_vaux5_lowper()
  {
    unsigned char value;


    /* 
     * Vaux5Lowper 
     */ 
    value = (I2CRead(CTL_SPARE_6_FUN_REG) & VAUX_5_LOWPER_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux5
 *
 * IN   : param, a value to write to the regiter CtlSpare6Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 4:4
 *
 **************************************************************************/
unsigned char set_vaux5( enum vaux5_enable_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_6_FUN_REG);

    /* 
     * Vaux5Enable 
     */ 
    switch( param ){
        case VAUX_5_ENABLE_E: 
           value = old_value | VAUX_5_MASK; 
           break;
        case VAUX_5_DISABLE_E: 
           value = old_value & ~ VAUX_5_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_6_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux5
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 4:4
 *
 **************************************************************************/
unsigned char get_vaux5()
  {
    unsigned char value;


    /* 
     * Vaux5Enable 
     */ 
    value = (I2CRead(CTL_SPARE_6_FUN_REG) & VAUX_5_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux5_discharge
 *
 * IN   : param, a value to write to the regiter CtlSpare6Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 5:5
 *
 **************************************************************************/
unsigned char set_vaux5_discharge( enum vaux5_discharge_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_6_FUN_REG);

    /* 
     * Vaux5Discharge 
     */ 
    switch( param ){
        case VAUX_5_DISCHARGE_ENABLE_E: 
           value = old_value | VAUX_5_DISCHARGE_MASK; 
           break;
        case VAUX_5_DISCHARGE_NO_ACTION_E: 
           value = old_value & ~ VAUX_5_DISCHARGE_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_6_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux5_discharge
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 5:5
 *
 **************************************************************************/
unsigned char get_vaux5_discharge()
  {
    unsigned char value;


    /* 
     * Vaux5Discharge 
     */ 
    value = (I2CRead(CTL_SPARE_6_FUN_REG) & VAUX_5_DISCHARGE_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux5_dis_sfst
 *
 * IN   : param, a value to write to the regiter CtlSpare6Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 6:6
 *
 **************************************************************************/
unsigned char set_vaux5_dis_sfst( enum vaux5_dis_sfst_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_6_FUN_REG);

    /* 
     * Vaux5DisSfstSet 
     */ 
    switch( param ){
        case VAUX_5_DIS_SFST_ENABLE_E: 
           value = old_value | VAUX_5_DIS_SFST_MASK; 
           break;
        case VAUX_5_DIS_SFST_NO_ACTION_E: 
           value = old_value & ~ VAUX_5_DIS_SFST_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_6_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux5_dis_sfst
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 6:6
 *
 **************************************************************************/
unsigned char get_vaux5_dis_sfst()
  {
    unsigned char value;


    /* 
     * Vaux5DisSfstSet 
     */ 
    value = (I2CRead(CTL_SPARE_6_FUN_REG) & VAUX_5_DIS_SFST_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux5_dis_pull_down
 *
 * IN   : param, a value to write to the regiter CtlSpare6Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux5_dis_pull_down( enum vaux5_dis_pull_down param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_6_FUN_REG);

    /* 
     * Vaux5DisPullDown 
     */ 
    switch( param ){
        case VAUX_5_DIS_PULL_UP_E: 
           value = old_value | VAUX_5_DIS_PULL_DOWN_MASK; 
           break;
        case VAUX_5_DIS_PULL_DOWN_E: 
           value = old_value & ~ VAUX_5_DIS_PULL_DOWN_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_6_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux5_dis_pull_down
 *
 * RET  : Return the value of register 0xCtlSpare6Fun
 *
 * Notes : From register 0x155, bits 7:7
 *
 **************************************************************************/
unsigned char get_vaux5_dis_pull_down()
  {
    unsigned char value;


    /* 
     * Vaux5DisPullDown 
     */ 
    value = (I2CRead(CTL_SPARE_6_FUN_REG) & VAUX_5_DIS_PULL_DOWN_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux6_selection
 *
 * IN   : param, a value to write to the regiter CtlSpare7Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 2:0
 *
 **************************************************************************/
unsigned char set_vaux6_selection( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_7_FUN_REG);

    /* 
     * Vaux6Selection 
     */ 
  
    value =  old_value & ~VAUX_6_SELECTION_MASK;


    value |=  param ;  

    I2CWrite(CTL_SPARE_7_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux6_selection
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 2:0
 *
 **************************************************************************/
unsigned char get_vaux6_selection()
  {
    unsigned char value;


    /* 
     * Vaux6Selection 
     */ 
    value = (I2CRead(CTL_SPARE_7_FUN_REG) & VAUX_6_SELECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux6_lowper
 *
 * IN   : param, a value to write to the regiter CtlSpare7Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 3:3
 *
 **************************************************************************/
unsigned char set_vaux6_lowper( enum vaux6_lowper_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_7_FUN_REG);

    /* 
     * Vaux6Lowper 
     */ 
    switch( param ){
        case VAUX_6_LOWPER_ENABLE_E: 
           value = old_value | VAUX_6_LOWPER_MASK; 
           break;
        case VAUX_6_LOWPER_NO_ACTION_E: 
           value = old_value & ~ VAUX_6_LOWPER_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_7_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux6_lowper
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 3:3
 *
 **************************************************************************/
unsigned char get_vaux6_lowper()
  {
    unsigned char value;


    /* 
     * Vaux6Lowper 
     */ 
    value = (I2CRead(CTL_SPARE_7_FUN_REG) & VAUX_6_LOWPER_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux6
 *
 * IN   : param, a value to write to the regiter CtlSpare7Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 4:4
 *
 **************************************************************************/
unsigned char set_vaux6( enum vaux6_enable_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_7_FUN_REG);

    /* 
     * Vaux6Enable 
     */ 
    switch( param ){
        case VAUX_6_ENABLE_E: 
           value = old_value | VAUX_6_MASK; 
           break;
        case VAUX_6_DISABLE_E: 
           value = old_value & ~ VAUX_6_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_7_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux6
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 4:4
 *
 **************************************************************************/
unsigned char get_vaux6()
  {
    unsigned char value;


    /* 
     * Vaux6Enable 
     */ 
    value = (I2CRead(CTL_SPARE_7_FUN_REG) & VAUX_6_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux6_discharge
 *
 * IN   : param, a value to write to the regiter CtlSpare7Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 5:5
 *
 **************************************************************************/
unsigned char set_vaux6_discharge( enum vaux6_discharge_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_7_FUN_REG);

    /* 
     * Vaux6Discharge 
     */ 
    switch( param ){
        case VAUX_6_DISCHARGE_ENABLE_E: 
           value = old_value | VAUX_6_DISCHARGE_MASK; 
           break;
        case VAUX_6_DISCHARGE_NO_ACTION_E: 
           value = old_value & ~ VAUX_6_DISCHARGE_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_7_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux6_discharge
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 5:5
 *
 **************************************************************************/
unsigned char get_vaux6_discharge()
  {
    unsigned char value;


    /* 
     * Vaux6Discharge 
     */ 
    value = (I2CRead(CTL_SPARE_7_FUN_REG) & VAUX_6_DISCHARGE_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux6_dis_sfst
 *
 * IN   : param, a value to write to the regiter CtlSpare7Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 6:6
 *
 **************************************************************************/
unsigned char set_vaux6_dis_sfst( enum vaux6_dis_sfst_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_7_FUN_REG);

    /* 
     * Vaux6DisSfstSet 
     */ 
    switch( param ){
        case VAUX_6_DIS_SFST_ENABLE_E: 
           value = old_value | VAUX_6_DIS_SFST_MASK; 
           break;
        case VAUX_6_DIS_SFST_NO_ACTION_E: 
           value = old_value & ~ VAUX_6_DIS_SFST_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_7_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux6_dis_sfst
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 6:6
 *
 **************************************************************************/
unsigned char get_vaux6_dis_sfst()
  {
    unsigned char value;


    /* 
     * Vaux6DisSfstSet 
     */ 
    value = (I2CRead(CTL_SPARE_7_FUN_REG) & VAUX_6_DIS_SFST_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux6_dis_pull_down
 *
 * IN   : param, a value to write to the regiter CtlSpare7Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux6_dis_pull_down( enum vaux6_dis_pull_down param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_7_FUN_REG);

    /* 
     * Vaux6DisPullDown 
     */ 
    switch( param ){
        case VAUX_6_DIS_PULL_UP_E: 
           value = old_value | VAUX_6_DIS_PULL_DOWN_MASK; 
           break;
        case VAUX_6_DIS_PULL_DOWN_E: 
           value = old_value & ~ VAUX_6_DIS_PULL_DOWN_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_7_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux6_dis_pull_down
 *
 * RET  : Return the value of register 0xCtlSpare7Fun
 *
 * Notes : From register 0x156, bits 7:7
 *
 **************************************************************************/
unsigned char get_vaux6_dis_pull_down()
  {
    unsigned char value;


    /* 
     * Vaux6DisPullDown 
     */ 
    value = (I2CRead(CTL_SPARE_7_FUN_REG) & VAUX_6_DIS_PULL_DOWN_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ab_reset_pull_down
 *
 * IN   : param, a value to write to the regiter CtlSpare8Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 0:0
 *
 **************************************************************************/
unsigned char set_ab_reset_pull_down( enum ab_reset_pull_down param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_8_FUN_REG);

    /* 
     * AbResetPullDown 
     */ 
    switch( param ){
        case AB_RESET_E: 
           value = old_value | AB_RESET_PULL_DOWN_MASK; 
           break;
        case AB_UN_RESET_E: 
           value = old_value & ~ AB_RESET_PULL_DOWN_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_8_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ab_reset_pull_down
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 0:0
 *
 **************************************************************************/
unsigned char get_ab_reset_pull_down()
  {
    unsigned char value;


    /* 
     * AbResetPullDown 
     */ 
    value = (I2CRead(CTL_SPARE_8_FUN_REG) & AB_RESET_PULL_DOWN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_iso_uicc_data_pull_down
 *
 * IN   : param, a value to write to the regiter CtlSpare8Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 1:1
 *
 **************************************************************************/
unsigned char set_iso_uicc_data_pull_down( enum iso_uicc_data_pull_down param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_8_FUN_REG);

    /* 
     * IsoUiccData pull down active 
     */ 
    switch( param ){
        case ISO_UICC_DATA_PULL_UP_E: 
           value = old_value | ISO_UICC_DATA_PULL_DOWN_MASK; 
           break;
        case ISO_UICC_DATA_PULL_DOWN_E: 
           value = old_value & ~ ISO_UICC_DATA_PULL_DOWN_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_8_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_iso_uicc_data_pull_down
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 1:1
 *
 **************************************************************************/
unsigned char get_iso_uicc_data_pull_down()
  {
    unsigned char value;


    /* 
     * IsoUiccData pull down active 
     */ 
    value = (I2CRead(CTL_SPARE_8_FUN_REG) & ISO_UICC_DATA_PULL_DOWN_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_iso_uicc_clk_pull_down
 *
 * IN   : param, a value to write to the regiter CtlSpare8Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 2:2
 *
 **************************************************************************/
unsigned char set_iso_uicc_clk_pull_down( enum iso_uicc_clock_pull_down param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_8_FUN_REG);

    /* 
     * IsoUiccClk pull down active 
     */ 
    switch( param ){
        case ISO_UICC_CLOCK_PULL_UP_E: 
           value = old_value | ISO_UICC_CLK_PULL_DOWN_MASK; 
           break;
        case ISO_UICC_CLOCK_PULL_DOWN_E: 
           value = old_value & ~ ISO_UICC_CLK_PULL_DOWN_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_8_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_iso_uicc_clk_pull_down
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 2:2
 *
 **************************************************************************/
unsigned char get_iso_uicc_clk_pull_down()
  {
    unsigned char value;


    /* 
     * IsoUiccClk pull down active 
     */ 
    value = (I2CRead(CTL_SPARE_8_FUN_REG) & ISO_UICC_CLK_PULL_DOWN_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_iso_uicc_io_control
 *
 * IN   : param, a value to write to the regiter CtlSpare8Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 3:3
 *
 **************************************************************************/
unsigned char set_iso_uicc_io_control( enum iso_uicc_io_control_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_8_FUN_REG);

    /* 
     * IsoUiccIoControl 
     */ 
    switch( param ){
        case ISO_UICC_IO_CONTROL_ENABLE_E: 
           value = old_value | ISO_UICC_IO_CONTROL_MASK; 
           break;
        case ISO_UICC_IO_CONTROL_NO_ACTION_E: 
           value = old_value & ~ ISO_UICC_IO_CONTROL_MASK;
           break;
    }
  

    I2CWrite(CTL_SPARE_8_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_iso_uicc_io_control
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 3:3
 *
 **************************************************************************/
unsigned char get_iso_uicc_io_control()
  {
    unsigned char value;


    /* 
     * IsoUiccIoControl 
     */ 
    value = (I2CRead(CTL_SPARE_8_FUN_REG) & ISO_UICC_IO_CONTROL_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaudio_selection
 *
 * IN   : param, a value to write to the regiter CtlSpare8Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 6:4
 *
 **************************************************************************/
unsigned char set_vaudio_selection( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_8_FUN_REG);

    /* 
     * VaudioSelection 
     */ 
  
    value =  old_value & ~VAUDIO_SELECTION_MASK;


    value |= ( param << 0x4);  

    I2CWrite(CTL_SPARE_8_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaudio_selection
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 6:4
 *
 **************************************************************************/
unsigned char get_vaudio_selection()
  {
    unsigned char value;


    /* 
     * VaudioSelection 
     */ 
    value = (I2CRead(CTL_SPARE_8_FUN_REG) & VAUDIO_SELECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_debug_for_internal_use
 *
 * IN   : param, a value to write to the regiter CtlSpare8Fun
 * OUT  : 
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 7:7
 *
 **************************************************************************/
unsigned char set_debug_for_internal_use( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CTL_SPARE_8_FUN_REG);

    /* 
     * Debug For Internal Use 
     */ 
  
    value =  old_value & ~DEBUG_FOR_INTERNAL_USE_MASK;


    value |= ( param << 0x7);  

    I2CWrite(CTL_SPARE_8_FUN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_debug_for_internal_use
 *
 * RET  : Return the value of register 0xCtlSpare8Fun
 *
 * Notes : From register 0x157, bits 7:7
 *
 **************************************************************************/
unsigned char get_debug_for_internal_use()
  {
    unsigned char value;


    /* 
     * Debug For Internal Use 
     */ 
    value = (I2CRead(CTL_SPARE_8_FUN_REG) & DEBUG_FOR_INTERNAL_USE_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_software_off
 *
 * IN   : param, a value to write to the regiter Control
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl
 *
 * Notes : From register 0x180, bits 0:0
 *
 **************************************************************************/
unsigned char set_software_off( enum software_off param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: Turn off Ab8500 
     */ 
    switch( param ){
        case TURN_OFF_STW8500_E: 
           value = old_value | SOFTWARE_OFF_MASK; 
           break;
        case TURN_OFF_STW8500_INACTIVE_E: 
           value = old_value & ~ SOFTWARE_OFF_MASK;
           break;
    }
  

    I2CWrite(CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_software_off
 *
 * RET  : Return the value of register 0xControl
 *
 * Notes : From register 0x180, bits 0:0
 *
 **************************************************************************/
unsigned char get_software_off()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Turn off Ab8500 
     */ 
    value = (I2CRead(CONTROL_REG) & SOFTWARE_OFF_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_software_reset
 *
 * IN   : param, a value to write to the regiter Control
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl
 *
 * Notes : From register 0x180, bits 1:1
 *
 **************************************************************************/
unsigned char set_software_reset( enum software_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_REG);

    /* 
     * 0: Reset Ab8500 registers 
     * 1: inactive 
     */ 
    switch( param ){
        case SOFTWARE_RESET_E: 
           value = old_value | SOFTWARE_RESET_MASK; 
           break;
        case SOFTWARE_UN_RESET_E: 
           value = old_value & ~ SOFTWARE_RESET_MASK;
           break;
    }
  

    I2CWrite(CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_software_reset
 *
 * RET  : Return the value of register 0xControl
 *
 * Notes : From register 0x180, bits 1:1
 *
 **************************************************************************/
unsigned char get_software_reset()
  {
    unsigned char value;


    /* 
     * 0: Reset Ab8500 registers 
     * 1: inactive 
     */ 
    value = (I2CRead(CONTROL_REG) & SOFTWARE_RESET_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_thermal_software_off
 *
 * IN   : param, a value to write to the regiter Control
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl
 *
 * Notes : From register 0x180, bits 2:2
 *
 **************************************************************************/
unsigned char set_thermal_software_off( enum thermal_software_off param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_REG);

    /* 
     * Thermal DB8500 Software Off 
     */ 
    switch( param ){
        case THERMAL_SOFTWARE_ON_E: 
           value = old_value | THERMAL_SOFTWARE_OFF_MASK; 
           break;
        case THERMAL_SOFTWARE_OFF_E: 
           value = old_value & ~ THERMAL_SOFTWARE_OFF_MASK;
           break;
    }
  

    I2CWrite(CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_thermal_software_off
 *
 * RET  : Return the value of register 0xControl
 *
 * Notes : From register 0x180, bits 2:2
 *
 **************************************************************************/
unsigned char get_thermal_software_off()
  {
    unsigned char value;


    /* 
     * Thermal DB8500 Software Off 
     */ 
    value = (I2CRead(CONTROL_REG) & THERMAL_SOFTWARE_OFF_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vaux1_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 0:0
 *
 **************************************************************************/
unsigned char set_reset_vaux1_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_2_REG);

    /* 
     * 0: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits stay at 
     * their programmed value when Ab8500 is resetted 
     * 1: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits are 
     * reset to their default values when Ab8500 is resetted 
     */ 
    switch( param ){
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VAUX_1_VALID_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VAUX_1_VALID_MASK;
           break;
    }
  

    I2CWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vaux1_valid
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 0:0
 *
 **************************************************************************/
unsigned char get_reset_vaux1_valid()
  {
    unsigned char value;


    /* 
     * 0: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits stay at 
     * their programmed value when Ab8500 is resetted 
     * 1: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits are 
     * reset to their default values when Ab8500 is resetted 
     */ 
    value = (I2CRead(CONTROL_2_REG) & RESET_VAUX_1_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vaux2_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 1:1
 *
 **************************************************************************/
unsigned char set_reset_vaux2_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_2_REG);

    /* 
     * 0: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits stay at 
     * their programmed value when Ab8500 is resetted 
     * 1: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits are 
     * reset to their default values when Ab8500 is resetted 
     */ 
    switch( param ){
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VAUX_2_VALID_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VAUX_2_VALID_MASK;
           break;
    }
  

    I2CWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vaux2_valid
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 1:1
 *
 **************************************************************************/
unsigned char get_reset_vaux2_valid()
  {
    unsigned char value;


    /* 
     * 0: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits stay at 
     * their programmed value when Ab8500 is resetted 
     * 1: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits are 
     * reset to their default values when Ab8500 is resetted 
     */ 
    value = (I2CRead(CONTROL_2_REG) & RESET_VAUX_2_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vaux3_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 2:2
 *
 **************************************************************************/
unsigned char set_reset_vaux3_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_2_REG);

    /* 
     * 0: Doesn't valid reset of Vaux3Regu[1:0], 
     * Vaux3RequestCtrl[1:0] and Vaux3Sel[2:0] bits when AB8500 
     * is reset 
     * 1: allow to valid reset of Vaux3Regu[1:0], 
     * Vaux3RequestCtrl[1:0] and Vaux3Sel[2:0] bits when AB8500 
     * is reset 
     */ 
    switch( param ){
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VAUX_3_VALID_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VAUX_3_VALID_MASK;
           break;
    }
  

    I2CWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vaux3_valid
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 2:2
 *
 **************************************************************************/
unsigned char get_reset_vaux3_valid()
  {
    unsigned char value;


    /* 
     * 0: Doesn't valid reset of Vaux3Regu[1:0], 
     * Vaux3RequestCtrl[1:0] and Vaux3Sel[2:0] bits when AB8500 
     * is reset 
     * 1: allow to valid reset of Vaux3Regu[1:0], 
     * Vaux3RequestCtrl[1:0] and Vaux3Sel[2:0] bits when AB8500 
     * is reset 
     */ 
    value = (I2CRead(CONTROL_2_REG) & RESET_VAUX_3_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vmod_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 3:3
 *
 **************************************************************************/
unsigned char set_reset_vmod_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_2_REG);

    /* 
     * 0: VmodRegu, VmodRequestCtrl and VmodSel bits stay at 
     * their programmed value when Ab8500 is resetted 
     * 1: VmodRegu, VmodRequestCtrl and Vaux3Sel bits are reset 
     * to their default values when Ab8500 is resetted 
     */ 
    switch( param ){
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VMOD_VALID_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VMOD_VALID_MASK;
           break;
    }
  

    I2CWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vmod_valid
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 3:3
 *
 **************************************************************************/
unsigned char get_reset_vmod_valid()
  {
    unsigned char value;


    /* 
     * 0: VmodRegu, VmodRequestCtrl and VmodSel bits stay at 
     * their programmed value when Ab8500 is resetted 
     * 1: VmodRegu, VmodRequestCtrl and Vaux3Sel bits are reset 
     * to their default values when Ab8500 is resetted 
     */ 
    value = (I2CRead(CONTROL_2_REG) & RESET_VMOD_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vext_supply_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 4:4
 *
 **************************************************************************/
unsigned char set_reset_vext_supply_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_2_REG);

    /* 
     * 0: Doesn't valid reset of 'VextSupply1Regu[1:0]', 
     * 'VextSupply1RequestCtrl[1:0]','ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when AB8500 is reset 
     * 1: Allow to valid reset of 'VextSupply1Regu[1:0]', 
     * 'VextSupply1RequestCtrl[1:0]', 'ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when AB8500 is reset 
     */ 
    switch( param ){
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VEXT_SUPPLY_VALID_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VEXT_SUPPLY_VALID_MASK;
           break;
    }
  

    I2CWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vext_supply_valid
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 4:4
 *
 **************************************************************************/
unsigned char get_reset_vext_supply_valid()
  {
    unsigned char value;


    /* 
     * 0: Doesn't valid reset of 'VextSupply1Regu[1:0]', 
     * 'VextSupply1RequestCtrl[1:0]','ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when AB8500 is reset 
     * 1: Allow to valid reset of 'VextSupply1Regu[1:0]', 
     * 'VextSupply1RequestCtrl[1:0]', 'ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when AB8500 is reset 
     */ 
    value = (I2CRead(CONTROL_2_REG) & RESET_VEXT_SUPPLY_VALID_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vext_supply2_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 5:5
 *
 **************************************************************************/
unsigned char set_reset_vext_supply2_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_2_REG);

    /* 
     * 0: Doesn't valid reset of 'VextSupply2Regu[1:0]', 
     * 'VextSupply2RequestCtrl[1:0] ExtSup12LPnClkSel and 
     * ExtSupply12LP bits when AB8500 is reset 
     * 1: Allow to valid reset of 'VextSupply2Regu[1:0]', 
     * 'VextSupply2RequestCtrl[1:'ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when AB8500 is reset 
     */ 
    switch( param ){
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VEXT_SUPPLY_2_VALID_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VEXT_SUPPLY_2_VALID_MASK;
           break;
    }
  

    I2CWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vext_supply2_valid
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 5:5
 *
 **************************************************************************/
unsigned char get_reset_vext_supply2_valid()
  {
    unsigned char value;


    /* 
     * 0: Doesn't valid reset of 'VextSupply2Regu[1:0]', 
     * 'VextSupply2RequestCtrl[1:0] ExtSup12LPnClkSel and 
     * ExtSupply12LP bits when AB8500 is reset 
     * 1: Allow to valid reset of 'VextSupply2Regu[1:0]', 
     * 'VextSupply2RequestCtrl[1:'ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when AB8500 is reset 
     */ 
    value = (I2CRead(CONTROL_2_REG) & RESET_VEXT_SUPPLY_2_VALID_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vext_supply3_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 6:6
 *
 **************************************************************************/
unsigned char set_reset_vext_supply3_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_2_REG);

    /* 
     * 0: Doesn't valid reset of 'VextSupply3Regu[1:0]', 
     * 'VextSupply3RequestCtrl[1:0] ExtSup13LPnClkSel and 
     * ExtSupply13LP bits when AB8500 is reset 
     * 1: Allow to valid reset of 'VextSupply3Regu[1:0]', 
     * 'VextSupply3RequestCtrl[1:'ExtSup13LPnClkSel' and 
     * 'ExtSupply13LP' bits when AB8500 is reset 
     */ 
    switch( param ){
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VEXT_SUPPLY_3_VALID_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VEXT_SUPPLY_3_VALID_MASK;
           break;
    }
  

    I2CWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vext_supply3_valid
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 6:6
 *
 **************************************************************************/
unsigned char get_reset_vext_supply3_valid()
  {
    unsigned char value;


    /* 
     * 0: Doesn't valid reset of 'VextSupply3Regu[1:0]', 
     * 'VextSupply3RequestCtrl[1:0] ExtSup13LPnClkSel and 
     * ExtSupply13LP bits when AB8500 is reset 
     * 1: Allow to valid reset of 'VextSupply3Regu[1:0]', 
     * 'VextSupply3RequestCtrl[1:'ExtSup13LPnClkSel' and 
     * 'ExtSupply13LP' bits when AB8500 is reset 
     */ 
    value = (I2CRead(CONTROL_2_REG) & RESET_VEXT_SUPPLY_3_VALID_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vsmps1_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 7:7
 *
 **************************************************************************/
unsigned char set_reset_vsmps1_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_2_REG);

    /* 
     * 0: Doesn't valid reset of 'Vsmps1Regu[1:0]', 
     * 'Vsmps1RequestCtrl[1:0]' and Vsmps1Sel(i)[5:0] bits when 
     * AB8500 is reset 
     * 1: Allow to valid reset of 'Vsmps1Regu[1:0]', 
     * 'Vsmps1RequestCtrl[1:0]' and 'Vsmps1Sel(i)[5:0]' bits 
     * when AB8500 is reset 
     */ 
    switch( param ){
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VSMPS_1_VALID_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VSMPS_1_VALID_MASK;
           break;
    }
  

    I2CWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vsmps1_valid
 *
 * RET  : Return the value of register 0xControl2
 *
 * Notes : From register 0x181, bits 7:7
 *
 **************************************************************************/
unsigned char get_reset_vsmps1_valid()
  {
    unsigned char value;


    /* 
     * 0: Doesn't valid reset of 'Vsmps1Regu[1:0]', 
     * 'Vsmps1RequestCtrl[1:0]' and Vsmps1Sel(i)[5:0] bits when 
     * AB8500 is reset 
     * 1: Allow to valid reset of 'Vsmps1Regu[1:0]', 
     * 'Vsmps1RequestCtrl[1:0]' and 'Vsmps1Sel(i)[5:0]' bits 
     * when AB8500 is reset 
     */ 
    value = (I2CRead(CONTROL_2_REG) & RESET_VSMPS_1_VALID_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_force_mode_power_on
 *
 * IN   : param, a value to write to the regiter SystemCtrl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSystemCtrl4
 *
 * Notes : From register 0x182, bits 0:0
 *
 **************************************************************************/
unsigned char set_force_mode_power_on( enum valid_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYSTEM_CTRL_4_REG);

    /* 
     * allow to valid reset of SWMODPWRON bits when AB9540 is 
     * resetted 
     */ 
  

     value =  old_value & ~FORCE_MODE_POWER_ON_MASK ;

    switch(  param ){

           case DO_NOT_VALID_RESET_E:
                value =  value | DO_NOT_VALID_RESET ;
                break;
           case VALID_RESET_E:
                value =  value | VALID_RESET ;
                break;
    }
  

    I2CWrite(SYSTEM_CTRL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_force_mode_power_on
 *
 * RET  : Return the value of register 0xSystemCtrl4
 *
 * Notes : From register 0x182, bits 0:0
 *
 **************************************************************************/
unsigned char get_force_mode_power_on()
  {
    unsigned char value;


    /* 
     * allow to valid reset of SWMODPWRON bits when AB9540 is 
     * resetted 
     */ 
    value = (I2CRead(SYSTEM_CTRL_4_REG) & FORCE_MODE_POWER_ON_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_force_mode_power_status
 *
 * IN   : param, a value to write to the regiter SystemCtrl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSystemCtrl4
 *
 * Notes : From register 0x182, bits 1:1
 *
 **************************************************************************/
unsigned char set_force_mode_power_status( enum valid_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYSTEM_CTRL_4_REG);

    /* 
     * allow to valid reset of FORCEMODPWRSTATUS bits when 
     * AB9540 is resetted 
     */ 
  

     value =  old_value & ~FORCE_MODE_POWER_STATUS_MASK ;

    switch(  param ){

           case DO_NOT_VALID_RESET_E:
                value  = value  | (DO_NOT_VALID_RESET << 0x1);
                break;
           case VALID_RESET_E:
                value  = value  | (VALID_RESET << 0x1);
                break;
    }
  

    I2CWrite(SYSTEM_CTRL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_force_mode_power_status
 *
 * RET  : Return the value of register 0xSystemCtrl4
 *
 * Notes : From register 0x182, bits 1:1
 *
 **************************************************************************/
unsigned char get_force_mode_power_status()
  {
    unsigned char value;


    /* 
     * allow to valid reset of FORCEMODPWRSTATUS bits when 
     * AB9540 is resetted 
     */ 
    value = (I2CRead(SYSTEM_CTRL_4_REG) & FORCE_MODE_POWER_STATUS_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_software_dbb_reset
 *
 * IN   : param, a value to write to the regiter SystemCtrl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSystemCtrl4
 *
 * Notes : From register 0x182, bits 2:2
 *
 **************************************************************************/
unsigned char set_software_dbb_reset( enum valid_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYSTEM_CTRL_4_REG);

    /* 
     * allow to valid reset of SWDBBRSTN bits when AB9540 is 
     * resetted 
     */ 
  

     value =  old_value & ~SOFTWARE_DBB_RESET_MASK ;

    switch(  param ){

           case DO_NOT_VALID_RESET_E:
                value  = value  | (DO_NOT_VALID_RESET << 0x2);
                break;
           case VALID_RESET_E:
                value  = value  | (VALID_RESET << 0x2);
                break;
    }
  

    I2CWrite(SYSTEM_CTRL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_software_dbb_reset
 *
 * RET  : Return the value of register 0xSystemCtrl4
 *
 * Notes : From register 0x182, bits 2:2
 *
 **************************************************************************/
unsigned char get_software_dbb_reset()
  {
    unsigned char value;


    /* 
     * allow to valid reset of SWDBBRSTN bits when AB9540 is 
     * resetted 
     */ 
    value = (I2CRead(SYSTEM_CTRL_4_REG) & SOFTWARE_DBB_RESET_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vaux4_valid
 *
 * IN   : param, a value to write to the regiter SystemCtrl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSystemCtrl4
 *
 * Notes : From register 0x182, bits 3:3
 *
 **************************************************************************/
unsigned char set_reset_vaux4_valid( enum valid_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYSTEM_CTRL_4_REG);

    /* 
     * allow to valid reset of Vaux4Regu, Vaux4RequestCtrl and 
     * Vaux4Sel bits when AB9540 is resetted 
     */ 
  

     value =  old_value & ~RESET_VAUX_4_VALID_MASK ;

    switch(  param ){

           case DO_NOT_VALID_RESET_E:
                value  = value  | (DO_NOT_VALID_RESET << 0x3);
                break;
           case VALID_RESET_E:
                value  = value  | (VALID_RESET << 0x3);
                break;
    }
  

    I2CWrite(SYSTEM_CTRL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vaux4_valid
 *
 * RET  : Return the value of register 0xSystemCtrl4
 *
 * Notes : From register 0x182, bits 3:3
 *
 **************************************************************************/
unsigned char get_reset_vaux4_valid()
  {
    unsigned char value;


    /* 
     * allow to valid reset of Vaux4Regu, Vaux4RequestCtrl and 
     * Vaux4Sel bits when AB9540 is resetted 
     */ 
    value = (I2CRead(SYSTEM_CTRL_4_REG) & RESET_VAUX_4_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_clock32
 *
 * IN   : param, a value to write to the regiter Control3
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl3
 *
 * Notes : From register 0x200, bits 0:0
 *
 **************************************************************************/
unsigned char set_clock32( enum clock32_output_buffer_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_3_REG);

    /* 
     * 0: inactive 1: disable Clk32KOut2 output buffer 
     */ 
    switch( param ){
        case CLOCK_32_OUTPUT_BUFFER_ENABLE_E: 
           value = old_value | CLOCK_32_MASK; 
           break;
        case CLOCK_32_OUTPUT_BUFFER_DISABLE_E: 
           value = old_value & ~ CLOCK_32_MASK;
           break;
    }
  

    I2CWrite(CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_clock32
 *
 * RET  : Return the value of register 0xControl3
 *
 * Notes : From register 0x200, bits 0:0
 *
 **************************************************************************/
unsigned char get_clock32()
  {
    unsigned char value;


    /* 
     * 0: inactive 1: disable Clk32KOut2 output buffer 
     */ 
    value = (I2CRead(CONTROL_3_REG) & CLOCK_32_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_reset
 *
 * IN   : param, a value to write to the regiter Control3
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl3
 *
 * Notes : From register 0x200, bits 1:1
 *
 **************************************************************************/
unsigned char set_audio_reset( enum audio_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_3_REG);

    /* 
     * 0: reset Audio registers 
     * 1: inactive 
     */ 
    switch( param ){
        case AUDIO_RESET_E: 
           value = old_value | AUDIO_RESET_MASK; 
           break;
        case AUDIO_UN_RESET_E: 
           value = old_value & ~ AUDIO_RESET_MASK;
           break;
    }
  

    I2CWrite(CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_reset
 *
 * RET  : Return the value of register 0xControl3
 *
 * Notes : From register 0x200, bits 1:1
 *
 **************************************************************************/
unsigned char get_audio_reset()
  {
    unsigned char value;


    /* 
     * 0: reset Audio registers 
     * 1: inactive 
     */ 
    value = (I2CRead(CONTROL_3_REG) & AUDIO_RESET_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_thermal_shutdown
 *
 * IN   : param, a value to write to the regiter Control3
 * OUT  : 
 *
 * RET  : Return the value of register 0xControl3
 *
 * Notes : From register 0x200, bits 3:3
 *
 **************************************************************************/
unsigned char set_thermal_shutdown( enum thermal_shutdown_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_3_REG);

    /* 
     * 0: disable thermal shutdown 
     * 1: enable thermal shutdown 
     */ 
    switch( param ){
        case THERMAL_SHUTDOWN_ENABLE_E: 
           value = old_value | THERMAL_SHUTDOWN_MASK; 
           break;
        case THERMAL_SHUTDOWN_DISABLE_E: 
           value = old_value & ~ THERMAL_SHUTDOWN_MASK;
           break;
    }
  

    I2CWrite(CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_thermal_shutdown
 *
 * RET  : Return the value of register 0xControl3
 *
 * Notes : From register 0x200, bits 3:3
 *
 **************************************************************************/
unsigned char get_thermal_shutdown()
  {
    unsigned char value;


    /* 
     * 0: disable thermal shutdown 
     * 1: enable thermal shutdown 
     */ 
    value = (I2CRead(CONTROL_3_REG) & THERMAL_SHUTDOWN_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_watchdog
 *
 * IN   : param, a value to write to the regiter MainWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainWatchDogControl
 *
 * Notes : From register 0x201, bits 0:0
 *
 **************************************************************************/
unsigned char set_main_watchdog( enum main_watchdog_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_WATCH_DOG_CONTROL_REG);

    /* 
     * Enable Main watchdog 
     */ 
    switch( param ){
        case MAIN_WATCHDOG_ENABLE_E: 
           value = old_value | MAIN_WATCHDOG_MASK; 
           break;
        case MAIN_WATCHDOG_DISABLE_E: 
           value = old_value & ~ MAIN_WATCHDOG_MASK;
           break;
    }
  

    I2CWrite(MAIN_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_watchdog
 *
 * RET  : Return the value of register 0xMainWatchDogControl
 *
 * Notes : From register 0x201, bits 0:0
 *
 **************************************************************************/
unsigned char get_main_watchdog()
  {
    unsigned char value;


    /* 
     * Enable Main watchdog 
     */ 
    value = (I2CRead(MAIN_WATCH_DOG_CONTROL_REG) & MAIN_WATCHDOG_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_watchdog_kick
 *
 * IN   : param, a value to write to the regiter MainWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainWatchDogControl
 *
 * Notes : From register 0x201, bits 1:1
 *
 **************************************************************************/
unsigned char set_main_watchdog_kick( enum kick_main_watchdog_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_WATCH_DOG_CONTROL_REG);

    /* 
     * Kick Main watchdog. Auto-cleared 
     */ 
    switch( param ){
        case KICK_MAIN_WATCHDOG_ENABLE_E: 
           value = old_value | MAIN_WATCHDOG_KICK_MASK; 
           break;
        case KICK_MAIN_WATCHDOG_DISABLE_E: 
           value = old_value & ~ MAIN_WATCHDOG_KICK_MASK;
           break;
    }
  

    I2CWrite(MAIN_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_watchdog_kick
 *
 * RET  : Return the value of register 0xMainWatchDogControl
 *
 * Notes : From register 0x201, bits 1:1
 *
 **************************************************************************/
unsigned char get_main_watchdog_kick()
  {
    unsigned char value;


    /* 
     * Kick Main watchdog. Auto-cleared 
     */ 
    value = (I2CRead(MAIN_WATCH_DOG_CONTROL_REG) & MAIN_WATCHDOG_KICK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_turn_on_valid_on_expire
 *
 * IN   : param, a value to write to the regiter MainWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainWatchDogControl
 *
 * Notes : From register 0x201, bits 4:4
 *
 **************************************************************************/
unsigned char set_turn_on_valid_on_expire( enum main_watch_dog_write_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_WATCH_DOG_CONTROL_REG);

    /* 
     * 0: when Watchdog expires AB8500 is turned off 
     * 1: when Watchdog expires AB8500 is turned off and then 
     * turned On running start up sequence. 
     */ 
    switch( param ){
        case TURN_OFF_AND_STARTUP_E: 
           value = old_value | TURN_ON_VALID_ON_EXPIRE_MASK; 
           break;
        case STAY_TURNED_OFF_E: 
           value = old_value & ~ TURN_ON_VALID_ON_EXPIRE_MASK;
           break;
    }
  

    I2CWrite(MAIN_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_turn_on_valid_on_expire
 *
 * RET  : Return the value of register 0xMainWatchDogControl
 *
 * Notes : From register 0x201, bits 4:4
 *
 **************************************************************************/
unsigned char get_turn_on_valid_on_expire()
  {
    unsigned char value;


    /* 
     * 0: when Watchdog expires AB8500 is turned off 
     * 1: when Watchdog expires AB8500 is turned off and then 
     * turned On running start up sequence. 
     */ 
    value = (I2CRead(MAIN_WATCH_DOG_CONTROL_REG) & TURN_ON_VALID_ON_EXPIRE_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_watchdog_timer
 *
 * IN   : param, a value to write to the regiter MainWatchDogTimer
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainWatchDogTimer
 *
 * Notes : From register 0x202, bits 6:0
 *
 **************************************************************************/
unsigned char set_main_watchdog_timer( enum main_watchdog_timer param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_WATCH_DOG_TIMER_REG);

    /* 
     * Defined Main watchdog time from 1s to 128s by 1s steps, 
     * default value is 32s 
     */ 
  
    value =  old_value & ~MAIN_WATCHDOG_TIMER_MASK;


    value |=  param ;  
    I2CWrite(MAIN_WATCH_DOG_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_watchdog_timer
 *
 * RET  : Return the value of register 0xMainWatchDogTimer
 *
 * Notes : From register 0x202, bits 6:0
 *
 **************************************************************************/
unsigned char get_main_watchdog_timer()
  {
    unsigned char value;


    /* 
     * Defined Main watchdog time from 1s to 128s by 1s steps, 
     * default value is 32s 
     */ 
    value = (I2CRead(MAIN_WATCH_DOG_TIMER_REG) & MAIN_WATCHDOG_TIMER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_battery_function
 *
 * IN   : param, a value to write to the regiter MainWatchDogTimer1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainWatchDogTimer1
 *
 * Notes : From register 0x203, bits 0:0
 *
 **************************************************************************/
unsigned char set_low_battery_function( enum low_battery_function_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_WATCH_DOG_TIMER_1_REG);

    /* 
     * 0: disable LowBat function 
     * 1: enable LowBat function 
     */ 
    switch( param ){
        case LOW_BATTERY_FUNCTION_ENABLE_E: 
           value = old_value | LOW_BATTERY_FUNCTION_MASK; 
           break;
        case LOW_BATTERY_FUNCTION_DISABLE_E: 
           value = old_value & ~ LOW_BATTERY_FUNCTION_MASK;
           break;
    }
  

    I2CWrite(MAIN_WATCH_DOG_TIMER_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_battery_function
 *
 * RET  : Return the value of register 0xMainWatchDogTimer1
 *
 * Notes : From register 0x203, bits 0:0
 *
 **************************************************************************/
unsigned char get_low_battery_function()
  {
    unsigned char value;


    /* 
     * 0: disable LowBat function 
     * 1: enable LowBat function 
     */ 
    value = (I2CRead(MAIN_WATCH_DOG_TIMER_1_REG) & LOW_BATTERY_FUNCTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_bat_threshold
 *
 * IN   : param, a value to write to the regiter MainWatchDogTimer1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainWatchDogTimer1
 *
 * Notes : From register 0x203, bits 6:1
 *
 **************************************************************************/
unsigned char set_low_bat_threshold( enum low_bat_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_WATCH_DOG_TIMER_1_REG);

    /* 
     * Defined LowBat threshold from 2.3v to 3.85v 25mv steps 
     */ 
  
    value =  old_value & ~LOW_BAT_THRESHOLD_MASK;


    value |= ( param << 0x1);  
    I2CWrite(MAIN_WATCH_DOG_TIMER_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_bat_threshold
 *
 * RET  : Return the value of register 0xMainWatchDogTimer1
 *
 * Notes : From register 0x203, bits 6:1
 *
 **************************************************************************/
unsigned char get_low_bat_threshold()
  {
    unsigned char value;


    /* 
     * Defined LowBat threshold from 2.3v to 3.85v 25mv steps 
     */ 
    value = (I2CRead(MAIN_WATCH_DOG_TIMER_1_REG) & LOW_BAT_THRESHOLD_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_ok1_threshold
 *
 * IN   : param, a value to write to the regiter BatteryOkThreshold
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatteryOkThreshold
 *
 * Notes : From register 0x204, bits 3:0
 *
 **************************************************************************/
unsigned char set_battery_ok1_threshold( enum battery_ok1_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BATTERY_OK_THRESHOLD_REG);

    /* 
     * BattOk1 falling thresholds management 
     */ 
  
    value =  old_value & ~BATTERY_OK_1_THRESHOLD_MASK;


    value |=  param ;  
    I2CWrite(BATTERY_OK_THRESHOLD_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_ok1_threshold
 *
 * RET  : Return the value of register 0xBatteryOkThreshold
 *
 * Notes : From register 0x204, bits 3:0
 *
 **************************************************************************/
unsigned char get_battery_ok1_threshold()
  {
    unsigned char value;


    /* 
     * BattOk1 falling thresholds management 
     */ 
    value = (I2CRead(BATTERY_OK_THRESHOLD_REG) & BATTERY_OK_1_THRESHOLD_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_ok2_threshold
 *
 * IN   : param, a value to write to the regiter BatteryOkThreshold
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatteryOkThreshold
 *
 * Notes : From register 0x204, bits 7:4
 *
 **************************************************************************/
unsigned char set_battery_ok2_threshold( enum battery_ok2_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BATTERY_OK_THRESHOLD_REG);

    /* 
     * BattOk2 falling thresholds management 
     */ 
  
    value =  old_value & ~BATTERY_OK_2_THRESHOLD_MASK;


    value |= ( param << 0x4);  
    I2CWrite(BATTERY_OK_THRESHOLD_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_ok2_threshold
 *
 * RET  : Return the value of register 0xBatteryOkThreshold
 *
 * Notes : From register 0x204, bits 7:4
 *
 **************************************************************************/
unsigned char get_battery_ok2_threshold()
  {
    unsigned char value;


    /* 
     * BattOk2 falling thresholds management 
     */ 
    value = (I2CRead(BATTERY_OK_THRESHOLD_REG) & BATTERY_OK_2_THRESHOLD_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_system_clock_timer
 *
 * IN   : param, a value to write to the regiter SystemClockTimer
 * OUT  : 
 *
 * RET  : Return the value of register 0xSystemClockTimer
 *
 * Notes : From register 0x205, bits 3:0
 *
 **************************************************************************/
unsigned char set_system_clock_timer( enum clock_timer param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYSTEM_CLOCK_TIMER_REG);

    /* 
     * System clock timer management. value multiple of 32 Khz 
     */ 
  

     value =  old_value & ~SYSTEM_CLOCK_TIMER_MASK ;

    switch(  param ){

           case PERIOD_3_38_MS_E:
                value =  value | PERIOD_3_38_MS ;
                break;
           case PERIOD_5_34_MS_E:
                value =  value | PERIOD_5_34_MS ;
                break;
           case PERIOD_7_78_MS_E:
                value =  value | PERIOD_7_78_MS ;
                break;
           case PERIOD_9_73_MS_E:
                value =  value | PERIOD_9_73_MS ;
                break;
    }
  

    I2CWrite(SYSTEM_CLOCK_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_system_clock_timer
 *
 * RET  : Return the value of register 0xSystemClockTimer
 *
 * Notes : From register 0x205, bits 3:0
 *
 **************************************************************************/
unsigned char get_system_clock_timer()
  {
    unsigned char value;


    /* 
     * System clock timer management. value multiple of 32 Khz 
     */ 
    value = (I2CRead(SYSTEM_CLOCK_TIMER_REG) & SYSTEM_CLOCK_TIMER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_system_clock_timer_adjustement
 *
 * IN   : param, a value to write to the regiter SystemClockTimer
 * OUT  : 
 *
 * RET  : Return the value of register 0xSystemClockTimer
 *
 * Notes : From register 0x205, bits 7:4
 *
 **************************************************************************/
unsigned char set_system_clock_timer_adjustement( enum timer_adjustement param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYSTEM_CLOCK_TIMER_REG);

    /* 
     * System clock timer management. Adjustement value multiple 
     * of 32 Khz 
     */ 
  
    value =  old_value & ~SYSTEM_CLOCK_TIMER_ADJUSTEMENT_MASK;


    value |= ( param << 0x4);  
    I2CWrite(SYSTEM_CLOCK_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_system_clock_timer_adjustement
 *
 * RET  : Return the value of register 0xSystemClockTimer
 *
 * Notes : From register 0x205, bits 7:4
 *
 **************************************************************************/
unsigned char get_system_clock_timer_adjustement()
  {
    unsigned char value;


    /* 
     * System clock timer management. Adjustement value multiple 
     * of 32 Khz 
     */ 
    value = (I2CRead(SYSTEM_CLOCK_TIMER_REG) & SYSTEM_CLOCK_TIMER_ADJUSTEMENT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_smps_clock_selection
 *
 * IN   : param, a value to write to the regiter SmpsClockControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSmpsClockControl
 *
 * Notes : From register 0x206, bits 1:0
 *
 **************************************************************************/
unsigned char set_smps_clock_selection( enum smps_clock_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SMPS_CLOCK_CONTROL_REG);

    /* 
     * 00: smps internal clock is SysClk (for Early Drop to 
     * change in inactive) 
     * 01: smps internal clock is internal 3.2Mhz oscillator 
     * 10: smps internal is first SysClk (if present ) or 
     * secondly internal 3.2Mhz oscillator 
     * 11: smps internal is first SysClk (if present) or 
     * secondly ulp clock (if present) or third internal 3.2Mhz 
     * oscillator 
     */ 
  

     value =  old_value & ~SMPS_CLOCK_SELECTION_MASK ;

    switch(  param ){

           case SYS_CLOCK_E:
                value =  value | SYS_CLOCK ;
                break;
           case INTERNAL_OSCILLATOR_E:
                value =  value | INTERNAL_OSCILLATOR ;
                break;
           case SYS_CLOCK_FISRT_OR_INTERNAL_OSCILLATOR_E:
                value =  value | SYS_CLOCK_FISRT_OR_INTERNAL_OSCILLATOR ;
                break;
           case SYS_CLOCK_FISRT_OR_ULP_CLOKC_OR_INTERNAL_OSCILLATOR_E:
                value =  value | SYS_CLOCK_FISRT_OR_ULP_CLOKC_OR_INTERNAL_OSCILLATOR ;
                break;
    }
  

    I2CWrite(SMPS_CLOCK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_smps_clock_selection
 *
 * RET  : Return the value of register 0xSmpsClockControl
 *
 * Notes : From register 0x206, bits 1:0
 *
 **************************************************************************/
unsigned char get_smps_clock_selection()
  {
    unsigned char value;


    /* 
     * 00: smps internal clock is SysClk (for Early Drop to 
     * change in inactive) 
     * 01: smps internal clock is internal 3.2Mhz oscillator 
     * 10: smps internal is first SysClk (if present ) or 
     * secondly internal 3.2Mhz oscillator 
     * 11: smps internal is first SysClk (if present) or 
     * secondly ulp clock (if present) or third internal 3.2Mhz 
     * oscillator 
     */ 
    value = (I2CRead(SMPS_CLOCK_CONTROL_REG) & SMPS_CLOCK_SELECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_force_smps_clock
 *
 * IN   : param, a value to write to the regiter SmpsClockControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSmpsClockControl
 *
 * Notes : From register 0x206, bits 2:2
 *
 **************************************************************************/
unsigned char set_force_smps_clock( enum force_smps_clock_on param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SMPS_CLOCK_CONTROL_REG);

    /* 
     * Force internal 3.2MHz oscillator ON 
     */ 
    switch( param ){
        case FORCE_SMPS_CLOCK_ON_E: 
           value = old_value | FORCE_SMPS_CLOCK_MASK; 
           break;
        case FORCE_SMPS_CLOCK_OFF_E: 
           value = old_value & ~ FORCE_SMPS_CLOCK_MASK;
           break;
    }
  

    I2CWrite(SMPS_CLOCK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_force_smps_clock
 *
 * RET  : Return the value of register 0xSmpsClockControl
 *
 * Notes : From register 0x206, bits 2:2
 *
 **************************************************************************/
unsigned char get_force_smps_clock()
  {
    unsigned char value;


    /* 
     * Force internal 3.2MHz oscillator ON 
     */ 
    value = (I2CRead(SMPS_CLOCK_CONTROL_REG) & FORCE_SMPS_CLOCK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSmpsClockPhase1
 *
 * Notes : From register 0x207, bits 2:0
 *
 **************************************************************************/
unsigned char set_varm_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SMPS_CLOCK_PHASE_1_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VARM_CLOCK_SEL_MASK ;

    switch(  param ){

           case PHASE_0_E:
                value =  value | PHASE_0 ;
                break;
           case PHASE_60_E:
                value =  value | PHASE_60 ;
                break;
           case PHASE_120_E:
                value =  value | PHASE_120 ;
                break;
           case PHASE_180_E:
                value =  value | PHASE_180 ;
                break;
           case PHASE_240_E:
                value =  value | PHASE_240 ;
                break;
           case PHASE_300_E:
                value =  value | PHASE_300 ;
                break;
    }
  

    I2CWrite(SMPS_CLOCK_PHASE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_clock_sel
 *
 * RET  : Return the value of register 0xSmpsClockPhase1
 *
 * Notes : From register 0x207, bits 2:0
 *
 **************************************************************************/
unsigned char get_varm_clock_sel()
  {
    unsigned char value;


    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
    value = (I2CRead(SMPS_CLOCK_PHASE_1_REG) & VARM_CLOCK_SEL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSmpsClockPhase1
 *
 * Notes : From register 0x207, bits 5:3
 *
 **************************************************************************/
unsigned char set_vape_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SMPS_CLOCK_PHASE_1_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VAPE_CLOCK_SEL_MASK ;

    switch(  param ){

           case PHASE_0_E:
                value  = value  | (PHASE_0 << 0x3);
                break;
           case PHASE_60_E:
                value  = value  | (PHASE_60 << 0x3);
                break;
           case PHASE_120_E:
                value  = value  | (PHASE_120 << 0x3);
                break;
           case PHASE_180_E:
                value  = value  | (PHASE_180 << 0x3);
                break;
           case PHASE_240_E:
                value  = value  | (PHASE_240 << 0x3);
                break;
           case PHASE_300_E:
                value  = value  | (PHASE_300 << 0x3);
                break;
    }
  

    I2CWrite(SMPS_CLOCK_PHASE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_clock_sel
 *
 * RET  : Return the value of register 0xSmpsClockPhase1
 *
 * Notes : From register 0x207, bits 5:3
 *
 **************************************************************************/
unsigned char get_vape_clock_sel()
  {
    unsigned char value;


    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
    value = (I2CRead(SMPS_CLOCK_PHASE_1_REG) & VAPE_CLOCK_SEL_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmod_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSmpsClockPhase2
 *
 * Notes : From register 0x208, bits 2:0
 *
 **************************************************************************/
unsigned char set_vsmod_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SMPS_CLOCK_PHASE_2_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VSMOD_CLOCK_SEL_MASK ;

    switch(  param ){

           case PHASE_0_E:
                value =  value | PHASE_0 ;
                break;
           case PHASE_60_E:
                value =  value | PHASE_60 ;
                break;
           case PHASE_120_E:
                value =  value | PHASE_120 ;
                break;
           case PHASE_180_E:
                value =  value | PHASE_180 ;
                break;
           case PHASE_240_E:
                value =  value | PHASE_240 ;
                break;
           case PHASE_300_E:
                value =  value | PHASE_300 ;
                break;
    }
  

    I2CWrite(SMPS_CLOCK_PHASE_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmod_clock_sel
 *
 * RET  : Return the value of register 0xSmpsClockPhase2
 *
 * Notes : From register 0x208, bits 2:0
 *
 **************************************************************************/
unsigned char get_vsmod_clock_sel()
  {
    unsigned char value;


    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
    value = (I2CRead(SMPS_CLOCK_PHASE_2_REG) & VSMOD_CLOCK_SEL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSmpsClockPhase2
 *
 * Notes : From register 0x208, bits 5:3
 *
 **************************************************************************/
unsigned char set_vsmps1_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SMPS_CLOCK_PHASE_2_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VSMPS_1_CLOCK_SEL_MASK ;

    switch(  param ){

           case PHASE_0_E:
                value  = value  | (PHASE_0 << 0x3);
                break;
           case PHASE_60_E:
                value  = value  | (PHASE_60 << 0x3);
                break;
           case PHASE_120_E:
                value  = value  | (PHASE_120 << 0x3);
                break;
           case PHASE_180_E:
                value  = value  | (PHASE_180 << 0x3);
                break;
           case PHASE_240_E:
                value  = value  | (PHASE_240 << 0x3);
                break;
           case PHASE_300_E:
                value  = value  | (PHASE_300 << 0x3);
                break;
    }
  

    I2CWrite(SMPS_CLOCK_PHASE_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_clock_sel
 *
 * RET  : Return the value of register 0xSmpsClockPhase2
 *
 * Notes : From register 0x208, bits 5:3
 *
 **************************************************************************/
unsigned char get_vsmps1_clock_sel()
  {
    unsigned char value;


    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
    value = (I2CRead(SMPS_CLOCK_PHASE_2_REG) & VSMPS_1_CLOCK_SEL_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSmpsClockPhase3
 *
 * Notes : From register 0x209, bits 2:0
 *
 **************************************************************************/
unsigned char set_vsmps2_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SMPS_CLOCK_PHASE_3_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VSMPS_2_CLOCK_SEL_MASK ;

    switch(  param ){

           case PHASE_0_E:
                value =  value | PHASE_0 ;
                break;
           case PHASE_60_E:
                value =  value | PHASE_60 ;
                break;
           case PHASE_120_E:
                value =  value | PHASE_120 ;
                break;
           case PHASE_180_E:
                value =  value | PHASE_180 ;
                break;
           case PHASE_240_E:
                value =  value | PHASE_240 ;
                break;
           case PHASE_300_E:
                value =  value | PHASE_300 ;
                break;
    }
  

    I2CWrite(SMPS_CLOCK_PHASE_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_clock_sel
 *
 * RET  : Return the value of register 0xSmpsClockPhase3
 *
 * Notes : From register 0x209, bits 2:0
 *
 **************************************************************************/
unsigned char get_vsmps2_clock_sel()
  {
    unsigned char value;


    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
    value = (I2CRead(SMPS_CLOCK_PHASE_3_REG) & VSMPS_2_CLOCK_SEL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSmpsClockPhase3
 *
 * Notes : From register 0x209, bits 5:3
 *
 **************************************************************************/
unsigned char set_vsmps3_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SMPS_CLOCK_PHASE_3_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VSMPS_3_CLOCK_SEL_MASK ;

    switch(  param ){

           case PHASE_0_E:
                value  = value  | (PHASE_0 << 0x3);
                break;
           case PHASE_60_E:
                value  = value  | (PHASE_60 << 0x3);
                break;
           case PHASE_120_E:
                value  = value  | (PHASE_120 << 0x3);
                break;
           case PHASE_180_E:
                value  = value  | (PHASE_180 << 0x3);
                break;
           case PHASE_240_E:
                value  = value  | (PHASE_240 << 0x3);
                break;
           case PHASE_300_E:
                value  = value  | (PHASE_300 << 0x3);
                break;
    }
  

    I2CWrite(SMPS_CLOCK_PHASE_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_clock_sel
 *
 * RET  : Return the value of register 0xSmpsClockPhase3
 *
 * Notes : From register 0x209, bits 5:3
 *
 **************************************************************************/
unsigned char get_vsmps3_clock_sel()
  {
    unsigned char value;


    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
    value = (I2CRead(SMPS_CLOCK_PHASE_3_REG) & VSMPS_3_CLOCK_SEL_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ulta_low_power_pad
 *
 * IN   : param, a value to write to the regiter SystemUlpClockConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xSystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 1:0
 *
 **************************************************************************/
unsigned char set_ulta_low_power_pad( enum ulta_low_power_pad param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYSTEM_ULP_CLOCK_CONF_REG);

    /* 
     * UlpClk pad configurations. ab8500 centric 
     */ 
  

     value =  old_value & ~ULTA_LOW_POWER_PAD_MASK ;

    switch(  param ){

           case ULTRA_LOW_POWER_PAD_NO_FUNTION_E:
                value =  value | ULTRA_LOW_POWER_PAD_NO_FUNTION ;
                break;
           case ULTRA_LOW_POWER_PAD_OUTPUT_E:
                value =  value | ULTRA_LOW_POWER_PAD_OUTPUT ;
                break;
           case ULTRA_LOW_POWER_PAD_INPUT_E:
                value =  value | ULTRA_LOW_POWER_PAD_INPUT ;
                break;
    }
  

    I2CWrite(SYSTEM_ULP_CLOCK_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ulta_low_power_pad
 *
 * RET  : Return the value of register 0xSystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 1:0
 *
 **************************************************************************/
unsigned char get_ulta_low_power_pad()
  {
    unsigned char value;


    /* 
     * UlpClk pad configurations. ab8500 centric 
     */ 
    value = (I2CRead(SYSTEM_ULP_CLOCK_CONF_REG) & ULTA_LOW_POWER_PAD_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ulta_low_power_clock_strength
 *
 * IN   : param, a value to write to the regiter SystemUlpClockConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xSystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 5:5
 *
 **************************************************************************/
unsigned char set_ulta_low_power_clock_strength( enum strength param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYSTEM_ULP_CLOCK_CONF_REG);

    /* 
     * 0: Low strength (able to drive a capacitor load up to 
     * 20pF) 
     * 1: High strength (able to drive a capacitor load up to 
     * 40pF) 
     */ 
    switch( param ){
        case HIGH_STRENGTH_E: 
           value = old_value | ULTA_LOW_POWER_CLOCK_STRENGTH_MASK; 
           break;
        case LOW_STRENGTH_E: 
           value = old_value & ~ ULTA_LOW_POWER_CLOCK_STRENGTH_MASK;
           break;
    }
  

    I2CWrite(SYSTEM_ULP_CLOCK_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ulta_low_power_clock_strength
 *
 * RET  : Return the value of register 0xSystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 5:5
 *
 **************************************************************************/
unsigned char get_ulta_low_power_clock_strength()
  {
    unsigned char value;


    /* 
     * 0: Low strength (able to drive a capacitor load up to 
     * 20pF) 
     * 1: High strength (able to drive a capacitor load up to 
     * 40pF) 
     */ 
    value = (I2CRead(SYSTEM_ULP_CLOCK_CONF_REG) & ULTA_LOW_POWER_CLOCK_STRENGTH_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_ultra_low_power_selection
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 1:0
 *
 **************************************************************************/
unsigned char set_sys_ultra_low_power_selection( enum sys_ultra_low_power_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_MANAGEMENT_REG);

    /* 
     * internal clock switching 
     */ 
  

     value =  old_value & ~SYS_ULTRA_LOW_POWER_SELECTION_MASK ;

    switch(  param ){

           case INTERNAL_CLOCK_SYSTEM_CLOCK_E:
                value =  value | INTERNAL_CLOCK_SYSTEM_CLOCK ;
                break;
           case INTERNAL_CLOCK_ULP_CLOCK_E:
                value =  value | INTERNAL_CLOCK_ULP_CLOCK ;
                break;
           case INTERNAL_CLOCK_SWITCHING_SYS_CLOCK_FISRT_E:
                value =  value | INTERNAL_CLOCK_SWITCHING_SYS_CLOCK_FISRT ;
                break;
    }
  

    I2CWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_ultra_low_power_selection
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 1:0
 *
 **************************************************************************/
unsigned char get_sys_ultra_low_power_selection()
  {
    unsigned char value;


    /* 
     * internal clock switching 
     */ 
    value = (I2CRead(CLOCK_MANAGEMENT_REG) & SYS_ULTRA_LOW_POWER_SELECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ulp_request
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 2:2
 *
 **************************************************************************/
unsigned char set_ulp_request( enum ulp_request_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_MANAGEMENT_REG);

    /* 
     * Ab8500 request ulp clock 
     */ 
    switch( param ){
        case ULP_REQUEST_ENABLE_E: 
           value = old_value | ULP_REQUEST_MASK; 
           break;
        case ULP_REQUEST_DISABLE_E: 
           value = old_value & ~ ULP_REQUEST_MASK;
           break;
    }
  

    I2CWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ulp_request
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 2:2
 *
 **************************************************************************/
unsigned char get_ulp_request()
  {
    unsigned char value;


    /* 
     * Ab8500 request ulp clock 
     */ 
    value = (I2CRead(CLOCK_MANAGEMENT_REG) & ULP_REQUEST_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_system_clock_request
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 3:3
 *
 **************************************************************************/
unsigned char set_system_clock_request( enum system_clock_request_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_MANAGEMENT_REG);

    /* 
     * Ab8500 request system clock 
     */ 
    switch( param ){
        case SYSTEM_CLOCK_REQUEST_ENABLE_E: 
           value = old_value | SYSTEM_CLOCK_REQUEST_MASK; 
           break;
        case SYSTEM_CLOCK_REQUEST_DISABLE_E: 
           value = old_value & ~ SYSTEM_CLOCK_REQUEST_MASK;
           break;
    }
  

    I2CWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_system_clock_request
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 3:3
 *
 **************************************************************************/
unsigned char get_system_clock_request()
  {
    unsigned char value;


    /* 
     * Ab8500 request system clock 
     */ 
    value = (I2CRead(CLOCK_MANAGEMENT_REG) & SYSTEM_CLOCK_REQUEST_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_clock
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 4:4
 *
 **************************************************************************/
unsigned char set_audio_clock( enum audio_clock_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_MANAGEMENT_REG);

    /* 
     * enable audio clock to audio block 
     */ 
    switch( param ){
        case AUDIO_CLOCK_ENABLE_E: 
           value = old_value | AUDIO_CLOCK_MASK; 
           break;
        case AUDIO_CLOCK_DISABLE_E: 
           value = old_value & ~ AUDIO_CLOCK_MASK;
           break;
    }
  

    I2CWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_clock
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 4:4
 *
 **************************************************************************/
unsigned char get_audio_clock()
  {
    unsigned char value;


    /* 
     * enable audio clock to audio block 
     */ 
    value = (I2CRead(CLOCK_MANAGEMENT_REG) & AUDIO_CLOCK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_buf2_req
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 5:5
 *
 **************************************************************************/
unsigned char set_sys_clk_buf2_req( enum activate_sys_clock_buffer2 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_MANAGEMENT_REG);

    /* 
     * 1: enable clock buffer 2 of U8500 RF / U8500 RF update 
     * device 
     */ 
    switch( param ){
        case ACTIVATE_SYS_CLOCK_BUFFER_2_E: 
           value = old_value | SYS_CLK_BUF_2_REQ_MASK; 
           break;
        case INACTIVE_SYS_CLOCK_BUFFER_2_E: 
           value = old_value & ~ SYS_CLK_BUF_2_REQ_MASK;
           break;
    }
  

    I2CWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_buf2_req
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 5:5
 *
 **************************************************************************/
unsigned char get_sys_clk_buf2_req()
  {
    unsigned char value;


    /* 
     * 1: enable clock buffer 2 of U8500 RF / U8500 RF update 
     * device 
     */ 
    value = (I2CRead(CLOCK_MANAGEMENT_REG) & SYS_CLK_BUF_2_REQ_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_buf3_req
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 6:6
 *
 **************************************************************************/
unsigned char set_sys_clk_buf3_req( enum activate_sys_clock_buffer3 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_MANAGEMENT_REG);

    /* 
     * 1: enable clock buffer 3 of U8500 RF / U8500 RF update 
     * device 
     */ 
    switch( param ){
        case ACTIVATE_SYS_CLOCK_BUFFER_3_E: 
           value = old_value | SYS_CLK_BUF_3_REQ_MASK; 
           break;
        case INACTIVE_SYS_CLOCK_BUFFER_3_E: 
           value = old_value & ~ SYS_CLK_BUF_3_REQ_MASK;
           break;
    }
  

    I2CWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_buf3_req
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 6:6
 *
 **************************************************************************/
unsigned char get_sys_clk_buf3_req()
  {
    unsigned char value;


    /* 
     * 1: enable clock buffer 3 of U8500 RF / U8500 RF update 
     * device 
     */ 
    value = (I2CRead(CLOCK_MANAGEMENT_REG) & SYS_CLK_BUF_3_REQ_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_buf4_req
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 7:7
 *
 **************************************************************************/
unsigned char set_sys_clk_buf4_req( enum activate_sys_clock_buffer4 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_MANAGEMENT_REG);

    /* 
     * 1: enable clock buffer 4 of U8500 RF / U8500 RF update 
     * device 
     */ 
    switch( param ){
        case ACTIVATE_SYS_CLOCK_BUFFER_4_E: 
           value = old_value | SYS_CLK_BUF_4_REQ_MASK; 
           break;
        case INACTIVE_SYS_CLOCK_BUFFER_4_E: 
           value = old_value & ~ SYS_CLK_BUF_4_REQ_MASK;
           break;
    }
  

    I2CWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_buf4_req
 *
 * RET  : Return the value of register 0xClockManagement
 *
 * Notes : From register 0x20B, bits 7:7
 *
 **************************************************************************/
unsigned char get_sys_clk_buf4_req()
  {
    unsigned char value;


    /* 
     * 1: enable clock buffer 4 of U8500 RF / U8500 RF update 
     * device 
     */ 
    value = (I2CRead(CLOCK_MANAGEMENT_REG) & SYS_CLK_BUF_4_REQ_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_clock
 *
 * IN   : param, a value to write to the regiter ClockValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid
 *
 * Notes : From register 0x20C, bits 2:2
 *
 **************************************************************************/
unsigned char set_usb_clock( enum usb_clock_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_REG);

    /* 
     * 0: disable SysClk path to USB block 
     * 1: enable SysClk path to USB block 
     */ 
    switch( param ){
        case USB_CLOCK_ENABLE_E: 
           value = old_value | USB_CLOCK_MASK; 
           break;
        case USB_CLOCK_DISABLE_E: 
           value = old_value & ~ USB_CLOCK_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_clock
 *
 * RET  : Return the value of register 0xClockValid
 *
 * Notes : From register 0x20C, bits 2:2
 *
 **************************************************************************/
unsigned char get_usb_clock()
  {
    unsigned char value;


    /* 
     * 0: disable SysClk path to USB block 
     * 1: enable SysClk path to USB block 
     */ 
    value = (I2CRead(CLOCK_VALID_REG) & USB_CLOCK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_request1_valid
 *
 * IN   : param, a value to write to the regiter ClockValid2
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid2
 *
 * Notes : From register 0x20D, bits 0:0
 *
 **************************************************************************/
unsigned char set_sys_clock_request1_valid( enum sys_clock_request1_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_2_REG);

    /* 
     * Sys valid system clock request from SysClkReq1input 
     */ 
    switch( param ){
        case APE_VALID_ULP_RESQUEST_E: 
           value = old_value | SYS_CLOCK_REQUEST_1_VALID_MASK; 
           break;
        case APE_DONT_VALID_ULP_RESQUEST_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_1_VALID_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_request1_valid
 *
 * RET  : Return the value of register 0xClockValid2
 *
 * Notes : From register 0x20D, bits 0:0
 *
 **************************************************************************/
unsigned char get_sys_clock_request1_valid()
  {
    unsigned char value;


    /* 
     * Sys valid system clock request from SysClkReq1input 
     */ 
    value = (I2CRead(CLOCK_VALID_2_REG) & SYS_CLOCK_REQUEST_1_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ultra_low_power_request1_valid
 *
 * IN   : param, a value to write to the regiter ClockValid2
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid2
 *
 * Notes : From register 0x20D, bits 1:1
 *
 **************************************************************************/
unsigned char set_ultra_low_power_request1_valid( enum ultra_low_power_request1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_2_REG);

    /* 
     * valid ulp clock request from SysClkReq1 input 
     */ 
    switch( param ){
        case ULTRA_LOW_POWER_REQUEST_1_ENABLE_E: 
           value = old_value | ULTRA_LOW_POWER_REQUEST_1_VALID_MASK; 
           break;
        case ULTRA_LOW_POWER_REQUEST_1_DISABLE_E: 
           value = old_value & ~ ULTRA_LOW_POWER_REQUEST_1_VALID_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ultra_low_power_request1_valid
 *
 * RET  : Return the value of register 0xClockValid2
 *
 * Notes : From register 0x20D, bits 1:1
 *
 **************************************************************************/
unsigned char get_ultra_low_power_request1_valid()
  {
    unsigned char value;


    /* 
     * valid ulp clock request from SysClkReq1 input 
     */ 
    value = (I2CRead(CLOCK_VALID_2_REG) & ULTRA_LOW_POWER_REQUEST_1_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_sys_clockr_request1_valid
 *
 * IN   : param, a value to write to the regiter ClockValid2
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid2
 *
 * Notes : From register 0x20D, bits 2:2
 *
 **************************************************************************/
unsigned char set_usb_sys_clockr_request1_valid( enum usb_sys_clockr_request1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_2_REG);

    /* 
     * 0: doesn't valid Usb block to request System 
     * 1: valid Usb block to request System clock 
     */ 
    switch( param ){
        case USB_SYS_CLOCKR_REQUEST_1_ENABLE_E: 
           value = old_value | USB_SYS_CLOCKR_REQUEST_1_VALID_MASK; 
           break;
        case USB_SYS_CLOCKR_REQUEST_1_DISABLE_E: 
           value = old_value & ~ USB_SYS_CLOCKR_REQUEST_1_VALID_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_sys_clockr_request1_valid
 *
 * RET  : Return the value of register 0xClockValid2
 *
 * Notes : From register 0x20D, bits 2:2
 *
 **************************************************************************/
unsigned char get_usb_sys_clockr_request1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid Usb block to request System 
     * 1: valid Usb block to request System clock 
     */ 
    value = (I2CRead(CLOCK_VALID_2_REG) & USB_SYS_CLOCKR_REQUEST_1_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ext_sup12_l_pn_clk_sel
 *
 * IN   : param, a value to write to the regiter ControlBall
 * OUT  : 
 *
 * RET  : Return the value of register 0xControlBall
 *
 * Notes : From register 0x20F, bits 1:0
 *
 **************************************************************************/
unsigned char set_ext_sup12_l_pn_clk_sel( enum configuration12 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_BALL_REG);

    /* 
     * 00: select ExtSup12LPn function = ExtSup12LP OR 
     * ExtSup12LP 
     * 01: select ExtSupply12Clk function. ExtSupply12Clk can be 
     * internal oscillator, Sysclk or UlpClk. 
     * 10 or 11: select (ExtSupply12LPn AND ExtSupply12Clk) 
     */ 
  

     value =  old_value & ~EXT_SUP_12L_PN_CLK_SEL_MASK ;

    switch(  param ){

           case EXT_SUP12_LPN_E:
                value =  value | EXT_SUP12_LPN ;
                break;
           case EXT_SUPPLY_12CLK_E:
                value =  value | EXT_SUPPLY_12CLK ;
                break;
           case EXT_SUPPLY12_LPN_AND_EXT_SUPPLY12_CLK_E:
                value =  value | EXT_SUPPLY12_LPN_AND_EXT_SUPPLY12_CLK ;
                break;
    }
  

    I2CWrite(CONTROL_BALL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ext_sup12_l_pn_clk_sel
 *
 * RET  : Return the value of register 0xControlBall
 *
 * Notes : From register 0x20F, bits 1:0
 *
 **************************************************************************/
unsigned char get_ext_sup12_l_pn_clk_sel()
  {
    unsigned char value;


    /* 
     * 00: select ExtSup12LPn function = ExtSup12LP OR 
     * ExtSup12LP 
     * 01: select ExtSupply12Clk function. ExtSupply12Clk can be 
     * internal oscillator, Sysclk or UlpClk. 
     * 10 or 11: select (ExtSupply12LPn AND ExtSupply12Clk) 
     */ 
    value = (I2CRead(CONTROL_BALL_REG) & EXT_SUP_12L_PN_CLK_SEL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ext_sup3_l_pn_clk_sel
 *
 * IN   : param, a value to write to the regiter ControlBall
 * OUT  : 
 *
 * RET  : Return the value of register 0xControlBall
 *
 * Notes : From register 0x20F, bits 3:2
 *
 **************************************************************************/
unsigned char set_ext_sup3_l_pn_clk_sel( enum configuration3 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_BALL_REG);

    /* 
     * 00: select ExtSup3LPn function = ExtSup3LP OR ExtSup3LP 
     * 01: select ExtSupply3Clk function. ExtSupply3Clk can be 
     * internal oscillator, Sysclk or UlpClk. 
     * 10 or 11: select (ExtSupply3LPn AND ExtSupply3Clk) 
     */ 
  

     value =  old_value & ~EXT_SUP_3L_PN_CLK_SEL_MASK ;

    switch(  param ){

           case EXT_SUP3_LPN_E:
                value  = value  | (EXT_SUP3_LPN << 0x2);
                break;
           case EXT_SUP_3LPN_E:
                value  = value  | (EXT_SUP_3LPN << 0x2);
                break;
           case EXT_SUPPLY3_LPN_AND_EXT_SUPPLY3_CLK_E:
                value  = value  | (EXT_SUPPLY3_LPN_AND_EXT_SUPPLY3_CLK << 0x2);
                break;
    }
  

    I2CWrite(CONTROL_BALL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ext_sup3_l_pn_clk_sel
 *
 * RET  : Return the value of register 0xControlBall
 *
 * Notes : From register 0x20F, bits 3:2
 *
 **************************************************************************/
unsigned char get_ext_sup3_l_pn_clk_sel()
  {
    unsigned char value;


    /* 
     * 00: select ExtSup3LPn function = ExtSup3LP OR ExtSup3LP 
     * 01: select ExtSupply3Clk function. ExtSupply3Clk can be 
     * internal oscillator, Sysclk or UlpClk. 
     * 10 or 11: select (ExtSupply3LPn AND ExtSupply3Clk) 
     */ 
    value = (I2CRead(CONTROL_BALL_REG) & EXT_SUP_3L_PN_CLK_SEL_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt8500_open_drain
 *
 * IN   : param, a value to write to the regiter ControlBall
 * OUT  : 
 *
 * RET  : Return the value of register 0xControlBall
 *
 * Notes : From register 0x20F, bits 4:4
 *
 **************************************************************************/
unsigned char set_interrupt8500_open_drain( enum ball_configuration param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONTROL_BALL_REG);

    /* 
     * 0: Int8500n is a push/pull output 
     * 1: Int8500n is an OpenDrain output 
     */ 
    switch( param ){
        case OPEN_DRAIN_E: 
           value = old_value | INTERRUPT_8500_OPEN_DRAIN_MASK; 
           break;
        case PUSH_PULL_E: 
           value = old_value & ~ INTERRUPT_8500_OPEN_DRAIN_MASK;
           break;
    }
  

    I2CWrite(CONTROL_BALL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt8500_open_drain
 *
 * RET  : Return the value of register 0xControlBall
 *
 * Notes : From register 0x20F, bits 4:4
 *
 **************************************************************************/
unsigned char get_interrupt8500_open_drain()
  {
    unsigned char value;


    /* 
     * 0: Int8500n is a push/pull output 
     * 1: Int8500n is an OpenDrain output 
     */ 
    value = (I2CRead(CONTROL_BALL_REG) & INTERRUPT_8500_OPEN_DRAIN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req1
 *
 * IN   : param, a value to write to the regiter ClockValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid3
 *
 * Notes : From register 0x210, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req1( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_3_REG);

    /* 
     * 0: SysClkReq1 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq1 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_2_REQ_1_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_2_REQ_1_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req1
 *
 * RET  : Return the value of register 0xClockValid3
 *
 * Notes : From register 0x210, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clock_buf2_req1()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq1 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq1 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_3_REG) & SYS_CLOCK_BUF_2_REQ_1_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req1
 *
 * IN   : param, a value to write to the regiter ClockValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid3
 *
 * Notes : From register 0x210, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req1( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_3_REG);

    /* 
     * 0: SysClkReq1 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq1 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_3_REQ_1_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_3_REQ_1_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req1
 *
 * RET  : Return the value of register 0xClockValid3
 *
 * Notes : From register 0x210, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clock_buf3_req1()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq1 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq1 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_3_REG) & SYS_CLOCK_BUF_3_REQ_1_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req1
 *
 * IN   : param, a value to write to the regiter ClockValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid3
 *
 * Notes : From register 0x210, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req1( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_3_REG);

    /* 
     * 0: SysClkReq1 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq1 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_4_REQ_1_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_4_REQ_1_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req1
 *
 * RET  : Return the value of register 0xClockValid3
 *
 * Notes : From register 0x210, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clock_buf4_req1()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq1 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq1 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_3_REG) & SYS_CLOCK_BUF_4_REQ_1_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req2
 *
 * IN   : param, a value to write to the regiter ClockValid4
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid4
 *
 * Notes : From register 0x211, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req2( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_4_REG);

    /* 
     * 0: SysClkReq2 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq2 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_2_REQ_2_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_2_REQ_2_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req2
 *
 * RET  : Return the value of register 0xClockValid4
 *
 * Notes : From register 0x211, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clock_buf2_req2()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq2 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq2 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_4_REG) & SYS_CLOCK_BUF_2_REQ_2_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req2
 *
 * IN   : param, a value to write to the regiter ClockValid4
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid4
 *
 * Notes : From register 0x211, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req2( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_4_REG);

    /* 
     * 0: SysClkReq2 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq2 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_3_REQ_2_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_3_REQ_2_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req2
 *
 * RET  : Return the value of register 0xClockValid4
 *
 * Notes : From register 0x211, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clock_buf3_req2()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq2 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq2 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_4_REG) & SYS_CLOCK_BUF_3_REQ_2_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req2
 *
 * IN   : param, a value to write to the regiter ClockValid4
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid4
 *
 * Notes : From register 0x211, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req2( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_4_REG);

    /* 
     * 0: SysClkReq2 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq2 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_4_REQ_2_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_4_REQ_2_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req2
 *
 * RET  : Return the value of register 0xClockValid4
 *
 * Notes : From register 0x211, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clock_buf4_req2()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq2 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq2 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_4_REG) & SYS_CLOCK_BUF_4_REQ_2_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req3
 *
 * IN   : param, a value to write to the regiter ClockValid5
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid5
 *
 * Notes : From register 0x212, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req3( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_5_REG);

    /* 
     * 0: SysClkReq3 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq3 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_2_REQ_3_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_2_REQ_3_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req3
 *
 * RET  : Return the value of register 0xClockValid5
 *
 * Notes : From register 0x212, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clock_buf2_req3()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq3 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq3 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_5_REG) & SYS_CLOCK_BUF_2_REQ_3_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req3
 *
 * IN   : param, a value to write to the regiter ClockValid5
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid5
 *
 * Notes : From register 0x212, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req3( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_5_REG);

    /* 
     * 0: SysClkReq3 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq3 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_3_REQ_3_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_3_REQ_3_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req3
 *
 * RET  : Return the value of register 0xClockValid5
 *
 * Notes : From register 0x212, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clock_buf3_req3()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq3 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq3 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_5_REG) & SYS_CLOCK_BUF_3_REQ_3_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req3
 *
 * IN   : param, a value to write to the regiter ClockValid5
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid5
 *
 * Notes : From register 0x212, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req3( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_5_REG);

    /* 
     * 0: SysClkReq3 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq3 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_4_REQ_3_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_4_REQ_3_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req3
 *
 * RET  : Return the value of register 0xClockValid5
 *
 * Notes : From register 0x212, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clock_buf4_req3()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq3 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq3 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_5_REG) & SYS_CLOCK_BUF_4_REQ_3_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req4
 *
 * IN   : param, a value to write to the regiter ClockValid6
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid6
 *
 * Notes : From register 0x213, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req4( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_6_REG);

    /* 
     * 0: SysClkReq4 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq4 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_2_REQ_4_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_2_REQ_4_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req4
 *
 * RET  : Return the value of register 0xClockValid6
 *
 * Notes : From register 0x213, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clock_buf2_req4()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq4 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq4 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_6_REG) & SYS_CLOCK_BUF_2_REQ_4_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req4
 *
 * IN   : param, a value to write to the regiter ClockValid6
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid6
 *
 * Notes : From register 0x213, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req4( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_6_REG);

    /* 
     * 0: SysClkReq4 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq4 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_3_REQ_4_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_3_REQ_4_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req4
 *
 * RET  : Return the value of register 0xClockValid6
 *
 * Notes : From register 0x213, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clock_buf3_req4()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq4 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq4 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_6_REG) & SYS_CLOCK_BUF_3_REQ_4_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req4
 *
 * IN   : param, a value to write to the regiter ClockValid6
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid6
 *
 * Notes : From register 0x213, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req4( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_6_REG);

    /* 
     * 0: SysClkReq4 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq4 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_4_REQ_4_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_4_REQ_4_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req4
 *
 * RET  : Return the value of register 0xClockValid6
 *
 * Notes : From register 0x213, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clock_buf4_req4()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq4 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq4 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_6_REG) & SYS_CLOCK_BUF_4_REQ_4_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req5
 *
 * IN   : param, a value to write to the regiter ClockValid7
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid7
 *
 * Notes : From register 0x214, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req5( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_7_REG);

    /* 
     * 0: SysClkReq5 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq5 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_2_REQ_5_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_2_REQ_5_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req5
 *
 * RET  : Return the value of register 0xClockValid7
 *
 * Notes : From register 0x214, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clock_buf2_req5()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq5 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq5 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_7_REG) & SYS_CLOCK_BUF_2_REQ_5_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req5
 *
 * IN   : param, a value to write to the regiter ClockValid7
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid7
 *
 * Notes : From register 0x214, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req5( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_7_REG);

    /* 
     * 0: SysClkReq5 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq5 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_3_REQ_5_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_3_REQ_5_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req5
 *
 * RET  : Return the value of register 0xClockValid7
 *
 * Notes : From register 0x214, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clock_buf3_req5()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq5 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq5 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_7_REG) & SYS_CLOCK_BUF_3_REQ_5_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req5
 *
 * IN   : param, a value to write to the regiter ClockValid7
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid7
 *
 * Notes : From register 0x214, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req5( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_7_REG);

    /* 
     * 0: SysClkReq5 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq5 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_4_REQ_5_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_4_REQ_5_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req5
 *
 * RET  : Return the value of register 0xClockValid7
 *
 * Notes : From register 0x214, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clock_buf4_req5()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq5 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq5 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_7_REG) & SYS_CLOCK_BUF_4_REQ_5_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req6
 *
 * IN   : param, a value to write to the regiter ClockValid8
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid8
 *
 * Notes : From register 0x215, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req6( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_8_REG);

    /* 
     * 0: SysClkReq6 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq6 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_2_REQ_6_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_2_REQ_6_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req6
 *
 * RET  : Return the value of register 0xClockValid8
 *
 * Notes : From register 0x215, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clock_buf2_req6()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq6 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq6 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_8_REG) & SYS_CLOCK_BUF_2_REQ_6_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req6
 *
 * IN   : param, a value to write to the regiter ClockValid8
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid8
 *
 * Notes : From register 0x215, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req6( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_8_REG);

    /* 
     * 0: SysClkReq6 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq6 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_3_REQ_6_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_3_REQ_6_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req6
 *
 * RET  : Return the value of register 0xClockValid8
 *
 * Notes : From register 0x215, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clock_buf3_req6()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq6 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq6 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_8_REG) & SYS_CLOCK_BUF_3_REQ_6_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req6
 *
 * IN   : param, a value to write to the regiter ClockValid8
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid8
 *
 * Notes : From register 0x215, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req6( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_8_REG);

    /* 
     * 0: SysClkReq6 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq6 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_4_REQ_6_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_4_REQ_6_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req6
 *
 * RET  : Return the value of register 0xClockValid8
 *
 * Notes : From register 0x215, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clock_buf4_req6()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq6 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq6 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_8_REG) & SYS_CLOCK_BUF_4_REQ_6_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req7
 *
 * IN   : param, a value to write to the regiter ClockValid9
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid9
 *
 * Notes : From register 0x216, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req7( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_9_REG);

    /* 
     * 0: SysClkReq7 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq7 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_2_REQ_7_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_2_REQ_7_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req7
 *
 * RET  : Return the value of register 0xClockValid9
 *
 * Notes : From register 0x216, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clock_buf2_req7()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq7 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq7 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_9_REG) & SYS_CLOCK_BUF_2_REQ_7_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req7
 *
 * IN   : param, a value to write to the regiter ClockValid9
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid9
 *
 * Notes : From register 0x216, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req7( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_9_REG);

    /* 
     * 0: SysClkReq7 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq7 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_3_REQ_7_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_3_REQ_7_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req7
 *
 * RET  : Return the value of register 0xClockValid9
 *
 * Notes : From register 0x216, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clock_buf3_req7()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq7 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq7 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_9_REG) & SYS_CLOCK_BUF_3_REQ_7_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req7
 *
 * IN   : param, a value to write to the regiter ClockValid9
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid9
 *
 * Notes : From register 0x216, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req7( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_9_REG);

    /* 
     * 0: SysClkReq7 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq7 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_4_REQ_7_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_4_REQ_7_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req7
 *
 * RET  : Return the value of register 0xClockValid9
 *
 * Notes : From register 0x216, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clock_buf4_req7()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq7 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq7 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_9_REG) & SYS_CLOCK_BUF_4_REQ_7_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req8
 *
 * IN   : param, a value to write to the regiter ClockValid10
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid10
 *
 * Notes : From register 0x217, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req8( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_10_REG);

    /* 
     * 0: SysClkReq8 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq8 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_2_REQ_8_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_2_REQ_8_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req8
 *
 * RET  : Return the value of register 0xClockValid10
 *
 * Notes : From register 0x217, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clock_buf2_req8()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq8 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq8 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_10_REG) & SYS_CLOCK_BUF_2_REQ_8_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req8
 *
 * IN   : param, a value to write to the regiter ClockValid10
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid10
 *
 * Notes : From register 0x217, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req8( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_10_REG);

    /* 
     * 0: SysClkReq8 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq8 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_3_REQ_8_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_3_REQ_8_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req8
 *
 * RET  : Return the value of register 0xClockValid10
 *
 * Notes : From register 0x217, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clock_buf3_req8()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq8 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq8 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_10_REG) & SYS_CLOCK_BUF_3_REQ_8_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req8
 *
 * IN   : param, a value to write to the regiter ClockValid10
 * OUT  : 
 *
 * RET  : Return the value of register 0xClockValid10
 *
 * Notes : From register 0x217, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req8( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLOCK_VALID_10_REG);

    /* 
     * 0: SysClkReq8 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq8 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param ){
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_BUF_4_REQ_8_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_BUF_4_REQ_8_MASK;
           break;
    }
  

    I2CWrite(CLOCK_VALID_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req8
 *
 * RET  : Return the value of register 0xClockValid10
 *
 * Notes : From register 0x217, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clock_buf4_req8()
  {
    unsigned char value;


    /* 
     * 0: SysClkReq8 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq8 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    value = (I2CRead(CLOCK_VALID_10_REG) & SYS_CLOCK_BUF_4_REQ_8_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 0:0
 *
 **************************************************************************/
unsigned char set_varm_dither( enum varm_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DITHER_CLK_CTRL_REG);

    /* 
     * VarmDitherEna 
     */ 
    switch( param ){
        case VARM_DITHER_ENABLE_E: 
           value = old_value | VARM_DITHER_MASK; 
           break;
        case VARM_DITHER_DISABLE_E: 
           value = old_value & ~ VARM_DITHER_MASK;
           break;
    }
  

    I2CWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_dither
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 0:0
 *
 **************************************************************************/
unsigned char get_varm_dither()
  {
    unsigned char value;


    /* 
     * VarmDitherEna 
     */ 
    value = (I2CRead(DITHER_CLK_CTRL_REG) & VARM_DITHER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsmps3_dither( enum vsmps3_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DITHER_CLK_CTRL_REG);

    /* 
     * Vsmps3DitherEna 
     */ 
    switch( param ){
        case VSMPS_3_DITHER_ENABLE_E: 
           value = old_value | VSMPS_3_DITHER_MASK; 
           break;
        case VSMPS_3_DITHER_DISABLE_E: 
           value = old_value & ~ VSMPS_3_DITHER_MASK;
           break;
    }
  

    I2CWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_dither
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 1:1
 *
 **************************************************************************/
unsigned char get_vsmps3_dither()
  {
    unsigned char value;


    /* 
     * Vsmps3DitherEna 
     */ 
    value = (I2CRead(DITHER_CLK_CTRL_REG) & VSMPS_3_DITHER_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps1_dither( enum vsmps1_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DITHER_CLK_CTRL_REG);

    /* 
     * Vsmps1DitherEna 
     */ 
    switch( param ){
        case VSMPS_1_DITHER_ENABLE_E: 
           value = old_value | VSMPS_1_DITHER_MASK; 
           break;
        case VSMPS_1_DITHER_DISABLE_E: 
           value = old_value & ~ VSMPS_1_DITHER_MASK;
           break;
    }
  

    I2CWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_dither
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 2:2
 *
 **************************************************************************/
unsigned char get_vsmps1_dither()
  {
    unsigned char value;


    /* 
     * Vsmps1DitherEna 
     */ 
    value = (I2CRead(DITHER_CLK_CTRL_REG) & VSMPS_1_DITHER_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 3:3
 *
 **************************************************************************/
unsigned char set_vsmps2_dither( enum vsmps2_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DITHER_CLK_CTRL_REG);

    /* 
     * Vsmps2DitherEna 
     */ 
    switch( param ){
        case VSMPS_2_DITHER_ENABLE_E: 
           value = old_value | VSMPS_2_DITHER_MASK; 
           break;
        case VSMPS_2_DITHER_DISABLE_E: 
           value = old_value & ~ VSMPS_2_DITHER_MASK;
           break;
    }
  

    I2CWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_dither
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 3:3
 *
 **************************************************************************/
unsigned char get_vsmps2_dither()
  {
    unsigned char value;


    /* 
     * Vsmps2DitherEna 
     */ 
    value = (I2CRead(DITHER_CLK_CTRL_REG) & VSMPS_2_DITHER_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 4:4
 *
 **************************************************************************/
unsigned char set_vmod_dither( enum vmod_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DITHER_CLK_CTRL_REG);

    /* 
     * VmodDitherEna 
     */ 
    switch( param ){
        case VMOD_DITHER_ENABLE_E: 
           value = old_value | VMOD_DITHER_MASK; 
           break;
        case VMOD_DITHER_DISABLE_E: 
           value = old_value & ~ VMOD_DITHER_MASK;
           break;
    }
  

    I2CWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_dither
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 4:4
 *
 **************************************************************************/
unsigned char get_vmod_dither()
  {
    unsigned char value;


    /* 
     * VmodDitherEna 
     */ 
    value = (I2CRead(DITHER_CLK_CTRL_REG) & VMOD_DITHER_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 5:5
 *
 **************************************************************************/
unsigned char set_vape_dither( enum vape_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DITHER_CLK_CTRL_REG);

    /* 
     * VapeDitherEna 
     */ 
    switch( param ){
        case VAPE_DITHER_ENABLE_E: 
           value = old_value | VAPE_DITHER_MASK; 
           break;
        case VAPE_DITHER_DISABLE_E: 
           value = old_value & ~ VAPE_DITHER_MASK;
           break;
    }
  

    I2CWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_dither
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 5:5
 *
 **************************************************************************/
unsigned char get_vape_dither()
  {
    unsigned char value;


    /* 
     * VapeDitherEna 
     */ 
    value = (I2CRead(DITHER_CLK_CTRL_REG) & VAPE_DITHER_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dithering_delay
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 7:6
 *
 **************************************************************************/
unsigned char set_dithering_delay( enum dithering_delay param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DITHER_CLK_CTRL_REG);

    /* 
     * Dithering delay 2.5ns, 3ns 3.5ns 4ns 
     */ 
  

     value =  old_value & ~DITHERING_DELAY_MASK ;

    switch(  param ){

           case DITHERING_DELAY_2_5_NS_E:
                value  = value  | (DITHERING_DELAY_2_5_NS << 0x6);
                break;
           case DITHERING_DELAY_3_NS_E:
                value  = value  | (DITHERING_DELAY_3_NS << 0x6);
                break;
           case DITHERING_DELAY_3_5_NS_E:
                value  = value  | (DITHERING_DELAY_3_5_NS << 0x6);
                break;
           case DITHERING_DELAY_4_NS_E:
                value  = value  | (DITHERING_DELAY_4_NS << 0x6);
                break;
    }
  

    I2CWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dithering_delay
 *
 * RET  : Return the value of register 0xDitherClkCtrl
 *
 * Notes : From register 0x220, bits 7:6
 *
 **************************************************************************/
unsigned char get_dithering_delay()
  {
    unsigned char value;


    /* 
     * Dithering delay 2.5ns, 3ns 3.5ns 4ns 
     */ 
    value = (I2CRead(DITHER_CLK_CTRL_REG) & DITHERING_DELAY_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_levi_or_yllas
 *
 * IN   : param, a value to write to the regiter SwatCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSwatCtrl
 *
 * Notes : From register 0x230, bits 0:0
 *
 **************************************************************************/
unsigned char set_levi_or_yllas( enum levi_or_yllas param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SWAT_CTRL_REG);

    /* 
     * 0: RF I.C. is Levi (allow to configure Swat message) 
     * 1: RF I.C. is Yllas (allow to configure Swat message) 
     */ 
    switch( param ){
        case RF_IC_YLLAS_E: 
           value = old_value | LEVI_OR_YLLAS_MASK; 
           break;
        case RF_IC_LEVI_E: 
           value = old_value & ~ LEVI_OR_YLLAS_MASK;
           break;
    }
  

    I2CWrite(SWAT_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_levi_or_yllas
 *
 * RET  : Return the value of register 0xSwatCtrl
 *
 * Notes : From register 0x230, bits 0:0
 *
 **************************************************************************/
unsigned char get_levi_or_yllas()
  {
    unsigned char value;


    /* 
     * 0: RF I.C. is Levi (allow to configure Swat message) 
     * 1: RF I.C. is Yllas (allow to configure Swat message) 
     */ 
    value = (I2CRead(SWAT_CTRL_REG) & LEVI_OR_YLLAS_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_swat
 *
 * IN   : param, a value to write to the regiter SwatCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSwatCtrl
 *
 * Notes : From register 0x230, bits 1:1
 *
 **************************************************************************/
unsigned char set_swat( enum swat_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SWAT_CTRL_REG);

    /* 
     * 0: Disable Swat block 
     * 1: Enable Swat block 
     */ 
    switch( param ){
        case SWAT_ENABLE_E: 
           value = old_value | SWAT_MASK; 
           break;
        case SWAT_DISABLE_E: 
           value = old_value & ~ SWAT_MASK;
           break;
    }
  

    I2CWrite(SWAT_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_swat
 *
 * RET  : Return the value of register 0xSwatCtrl
 *
 * Notes : From register 0x230, bits 1:1
 *
 **************************************************************************/
unsigned char get_swat()
  {
    unsigned char value;


    /* 
     * 0: Disable Swat block 
     * 1: Enable Swat block 
     */ 
    value = (I2CRead(SWAT_CTRL_REG) & SWAT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rf_off_timer
 *
 * IN   : param, a value to write to the regiter SwatCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSwatCtrl
 *
 * Notes : From register 0x230, bits 4:2
 *
 **************************************************************************/
unsigned char set_rf_off_timer( enum off_timer param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SWAT_CTRL_REG);

    /* 
     * Defined timer of RF turn off sequence, Timer = 
     * (RfOffTimer[2:0] +1) x periods of 32kHz 
     */ 
  

     value =  old_value & ~RF_OFF_TIMER_MASK ;

    switch(  param ){

           case TURN_OFF_1_PERIOD_OF_32KHZ_E:
                value  = value  | (TURN_OFF_1_PERIOD_OF_32KHZ << 0x2);
                break;
           case TURN_OFF_2_PERIOD2_OF_32KHZ_E:
                value  = value  | (TURN_OFF_2_PERIOD2_OF_32KHZ << 0x2);
                break;
           case TURN_OFF_3_PERIOD2_OF_32KHZ_E:
                value  = value  | (TURN_OFF_3_PERIOD2_OF_32KHZ << 0x2);
                break;
           case TURN_OFF_4_PERIOD2_OF_32KHZ_E:
                value  = value  | (TURN_OFF_4_PERIOD2_OF_32KHZ << 0x2);
                break;
           case TURN_OFF_5_PERIOD2_OF_32KHZ_E:
                value  = value  | (TURN_OFF_5_PERIOD2_OF_32KHZ << 0x2);
                break;
           case TURN_OFF_6_PERIOD2_OF_32KHZ_E:
                value  = value  | (TURN_OFF_6_PERIOD2_OF_32KHZ << 0x2);
                break;
           case TURN_OFF_7_PERIOD2_OF_32KHZ_E:
                value  = value  | (TURN_OFF_7_PERIOD2_OF_32KHZ << 0x2);
                break;
           case TURN_OFF_8_PERIOD2_OF_32KHZ_E:
                value  = value  | (TURN_OFF_8_PERIOD2_OF_32KHZ << 0x2);
                break;
    }
  

    I2CWrite(SWAT_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rf_off_timer
 *
 * RET  : Return the value of register 0xSwatCtrl
 *
 * Notes : From register 0x230, bits 4:2
 *
 **************************************************************************/
unsigned char get_rf_off_timer()
  {
    unsigned char value;


    /* 
     * Defined timer of RF turn off sequence, Timer = 
     * (RfOffTimer[2:0] +1) x periods of 32kHz 
     */ 
    value = (I2CRead(SWAT_CTRL_REG) & RF_OFF_TIMER_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_swat_bit5_set
 *
 * IN   : param, a value to write to the regiter SwatCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSwatCtrl
 *
 * Notes : From register 0x230, bits 6:6
 *
 **************************************************************************/
unsigned char set_swat_bit5_set( enum swat_bit5_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SWAT_CTRL_REG);

    /* 
     * Set 'Swat[5]' (each updated value will be sent through 
     * Swat interface) 
     */ 
    switch( param ){
        case SWAT_BIT_5_ENABLE_E: 
           value = old_value | SWAT_BIT_5_SET_MASK; 
           break;
        case SWAT_BIT_5_NO_ACTION_E: 
           value = old_value & ~ SWAT_BIT_5_SET_MASK;
           break;
    }
  

    I2CWrite(SWAT_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_swat_bit5_set
 *
 * RET  : Return the value of register 0xSwatCtrl
 *
 * Notes : From register 0x230, bits 6:6
 *
 **************************************************************************/
unsigned char get_swat_bit5_set()
  {
    unsigned char value;


    /* 
     * Set 'Swat[5]' (each updated value will be sent through 
     * Swat interface) 
     */ 
    value = (I2CRead(SWAT_CTRL_REG) & SWAT_BIT_5_SET_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req1_hiq
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 0:0
 *
 **************************************************************************/
unsigned char set_sys_clk_req1_hiq( enum sys_clk_req1_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq1 input to control HiqClkEna ball 
     */ 
    switch( param ){
        case SYS_CLK_REQ_1_HIQ_ENABLE_E: 
           value = old_value | SYS_CLK_REQ_1_HIQ_MASK; 
           break;
        case SYS_CLK_REQ_1_HIQ_DISABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_1_HIQ_MASK;
           break;
    }
  

    I2CWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req1_hiq
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 0:0
 *
 **************************************************************************/
unsigned char get_sys_clk_req1_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq1 input to control HiqClkEna ball 
     */ 
    value = (I2CRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_1_HIQ_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req2_hiq
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 1:1
 *
 **************************************************************************/
unsigned char set_sys_clk_req2_hiq( enum sys_clk_req2_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq2 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq2 input to control HiqClkEna ball 
     */ 
    switch( param ){
        case SYS_CLK_REQ_2_HIQ_ENABLE_E: 
           value = old_value | SYS_CLK_REQ_2_HIQ_MASK; 
           break;
        case SYS_CLK_REQ_2_HIQ_DISABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_2_HIQ_MASK;
           break;
    }
  

    I2CWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req2_hiq
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 1:1
 *
 **************************************************************************/
unsigned char get_sys_clk_req2_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq2 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq2 input to control HiqClkEna ball 
     */ 
    value = (I2CRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_2_HIQ_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req3_hiq
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clk_req3_hiq( enum sys_clk_req3_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq3 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq3 input to control HiqClkEna ball 
     */ 
    switch( param ){
        case SYS_CLK_REQ_3_HIQ_ENABLE_E: 
           value = old_value | SYS_CLK_REQ_3_HIQ_MASK; 
           break;
        case SYS_CLK_REQ_3_HIQ_DISABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_3_HIQ_MASK;
           break;
    }
  

    I2CWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req3_hiq
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clk_req3_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq3 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq3 input to control HiqClkEna ball 
     */ 
    value = (I2CRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_3_HIQ_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req4_hiq
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clk_req4_hiq( enum sys_clk_req4_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq4 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq4 input to control HiqClkEna ball 
     */ 
    switch( param ){
        case SYS_CLK_REQ_4_HIQ_ENABLE_E: 
           value = old_value | SYS_CLK_REQ_4_HIQ_MASK; 
           break;
        case SYS_CLK_REQ_4_HIQ_DISABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_4_HIQ_MASK;
           break;
    }
  

    I2CWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req4_hiq
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clk_req4_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq4 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq4 input to control HiqClkEna ball 
     */ 
    value = (I2CRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_4_HIQ_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req5_hiq
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clk_req5_hiq( enum sys_clk_req5_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq5 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq5 input to control HiqClkEna ball 
     */ 
    switch( param ){
        case SYS_CLK_REQ_5_HIQ_ENABLE_E: 
           value = old_value | SYS_CLK_REQ_5_HIQ_MASK; 
           break;
        case SYS_CLK_REQ_5_HIQ_DISABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_5_HIQ_MASK;
           break;
    }
  

    I2CWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req5_hiq
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clk_req5_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq5 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq5 input to control HiqClkEna ball 
     */ 
    value = (I2CRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_5_HIQ_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req6_hiq
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 5:5
 *
 **************************************************************************/
unsigned char set_sys_clk_req6_hiq( enum sys_clk_req6_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq6 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq6 input to control HiqClkEna ball 
     */ 
    switch( param ){
        case SYS_CLK_REQ_6_HIQ_ENABLE_E: 
           value = old_value | SYS_CLK_REQ_6_HIQ_MASK; 
           break;
        case SYS_CLK_REQ_6_HIQ_DISABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_6_HIQ_MASK;
           break;
    }
  

    I2CWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req6_hiq
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 5:5
 *
 **************************************************************************/
unsigned char get_sys_clk_req6_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq6 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq6 input to control HiqClkEna ball 
     */ 
    value = (I2CRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_6_HIQ_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req7_hiq
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 6:6
 *
 **************************************************************************/
unsigned char set_sys_clk_req7_hiq( enum sys_clk_req7_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq7 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq7 input to control HiqClkEna ball 
     */ 
    switch( param ){
        case SYS_CLK_REQ_7_HIQ_ENABLE_E: 
           value = old_value | SYS_CLK_REQ_7_HIQ_MASK; 
           break;
        case SYS_CLK_REQ_7_HIQ_DISABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_7_HIQ_MASK;
           break;
    }
  

    I2CWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req7_hiq
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 6:6
 *
 **************************************************************************/
unsigned char get_sys_clk_req7_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq7 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq7 input to control HiqClkEna ball 
     */ 
    value = (I2CRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_7_HIQ_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req8_hiq
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 7:7
 *
 **************************************************************************/
unsigned char set_sys_clk_req8_hiq( enum sys_clk_req8_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq8 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq8 input to control HiqClkEna ball 
     */ 
    switch( param ){
        case SYS_CLK_REQ_8_HIQ_ENABLE_E: 
           value = old_value | SYS_CLK_REQ_8_HIQ_MASK; 
           break;
        case SYS_CLK_REQ_8_HIQ_DISABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_8_HIQ_MASK;
           break;
    }
  

    I2CWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req8_hiq
 *
 * RET  : Return the value of register 0xHiqClkCtrl
 *
 * Notes : From register 0x232, bits 7:7
 *
 **************************************************************************/
unsigned char get_sys_clk_req8_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq8 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq8 input to control HiqClkEna ball 
     */ 
    value = (I2CRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_8_HIQ_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req1_hiq
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 0:0
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req1_hiq( enum vsim_sys_clk_req1_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsim ldo 
     * 1: valid SysClkReq1 inputs to request Vsim ldo 
     */ 
    switch( param ){
        case VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_1_HIQ_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_1_HIQ_DISABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_1_HIQ_MASK;
           break;
    }
  

    I2CWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req1_hiq
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 0:0
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req1_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsim ldo 
     * 1: valid SysClkReq1 inputs to request Vsim ldo 
     */ 
    value = (I2CRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_1_HIQ_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req2_hiq
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req2_hiq( enum vsim_sys_clk_req2_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq2 input to request Vsim ldo 
     * 1: valid SysClkReq2 inputs to request Vsim ldo 
     */ 
    switch( param ){
        case VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_2_HIQ_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_2_HIQ_DISABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_2_HIQ_MASK;
           break;
    }
  

    I2CWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req2_hiq
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 1:1
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req2_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq2 input to request Vsim ldo 
     * 1: valid SysClkReq2 inputs to request Vsim ldo 
     */ 
    value = (I2CRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_2_HIQ_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req3_hiq
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req3_hiq( enum vsim_sys_clk_req3_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq3 input to request Vsim ldo 
     * 1: valid SysClkReq2 inputs to request Vsim ldo 
     */ 
    switch( param ){
        case VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_3_HIQ_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_3_HIQ_DISABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_3_HIQ_MASK;
           break;
    }
  

    I2CWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req3_hiq
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 2:2
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req3_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq3 input to request Vsim ldo 
     * 1: valid SysClkReq2 inputs to request Vsim ldo 
     */ 
    value = (I2CRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_3_HIQ_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req4_hiq
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 3:3
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req4_hiq( enum vsim_sys_clk_req4_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq4 input to request Vsim ldo 
     * 1: valid SysClkReq4 inputs to request Vsim ldo 
     */ 
    switch( param ){
        case VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_4_HIQ_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_4_HIQ_DISABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_4_HIQ_MASK;
           break;
    }
  

    I2CWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req4_hiq
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 3:3
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req4_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq4 input to request Vsim ldo 
     * 1: valid SysClkReq4 inputs to request Vsim ldo 
     */ 
    value = (I2CRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_4_HIQ_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req5_hiq
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 4:4
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req5_hiq( enum vsim_sys_clk_req5_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq5 input to request Vsim ldo 
     * 1: valid SysClkReq5 inputs to request Vsim ldo 
     */ 
    switch( param ){
        case VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_5_HIQ_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_5_HIQ_DISABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_5_HIQ_MASK;
           break;
    }
  

    I2CWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req5_hiq
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 4:4
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req5_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq5 input to request Vsim ldo 
     * 1: valid SysClkReq5 inputs to request Vsim ldo 
     */ 
    value = (I2CRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_5_HIQ_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req6_hiq
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 5:5
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req6_hiq( enum vsim_sys_clk_req6_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq6 input to request Vsim ldo 
     * 1: valid SysClkReq6 inputs to request Vsim ldo 
     */ 
    switch( param ){
        case VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_6_HIQ_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_6_HIQ_DISABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_6_HIQ_MASK;
           break;
    }
  

    I2CWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req6_hiq
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 5:5
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req6_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq6 input to request Vsim ldo 
     * 1: valid SysClkReq6 inputs to request Vsim ldo 
     */ 
    value = (I2CRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_6_HIQ_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req7_hiq
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 6:6
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req7_hiq( enum vsim_sys_clk_req7_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq7 input to request Vsim ldo 
     * 1: valid SysClkReq7 inputs to request Vsim ldo 
     */ 
    switch( param ){
        case VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_7_HIQ_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_7_HIQ_DISABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_7_HIQ_MASK;
           break;
    }
  

    I2CWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req7_hiq
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 6:6
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req7_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq7 input to request Vsim ldo 
     * 1: valid SysClkReq7 inputs to request Vsim ldo 
     */ 
    value = (I2CRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_7_HIQ_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req8_hiq
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 7:7
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req8_hiq( enum vsim_sys_clk_req8_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq8 input to request Vsim ldo 
     * 1: valid SysClkReq8 inputs to request Vsim ldo 
     */ 
    switch( param ){
        case VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_8_HIQ_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_8_HIQ_DISABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_8_HIQ_MASK;
           break;
    }
  

    I2CWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req8_hiq
 *
 * RET  : Return the value of register 0xVsimSysClkCtrl
 *
 * Notes : From register 0x233, bits 7:7
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req8_hiq()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq8 input to request Vsim ldo 
     * 1: valid SysClkReq8 inputs to request Vsim ldo 
     */ 
    value = (I2CRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_8_HIQ_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sysclk12_buf1
 *
 * IN   : param, a value to write to the regiter SysClk12BufCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 0:0
 *
 **************************************************************************/
unsigned char set_sysclk12_buf1( enum sysclk12_buf1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYS_CLK_12_BUF_CTRL_REG);

    /* 
     * 0: Sysclk12Buf1 desabled (default) 
     * 1: Sysclk12Buf1 is enabled 
     */ 
    switch( param ){
        case SYSCLK_12_BUF_1_ENABLE_E: 
           value = old_value | SYSCLK_12_BUF_1_MASK; 
           break;
        case SYSCLK_12_BUF_1_DISABLE_E: 
           value = old_value & ~ SYSCLK_12_BUF_1_MASK;
           break;
    }
  

    I2CWrite(SYS_CLK_12_BUF_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sysclk12_buf1
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 0:0
 *
 **************************************************************************/
unsigned char get_sysclk12_buf1()
  {
    unsigned char value;


    /* 
     * 0: Sysclk12Buf1 desabled (default) 
     * 1: Sysclk12Buf1 is enabled 
     */ 
    value = (I2CRead(SYS_CLK_12_BUF_CTRL_REG) & SYSCLK_12_BUF_1_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sysclk12_buf2
 *
 * IN   : param, a value to write to the regiter SysClk12BufCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 1:1
 *
 **************************************************************************/
unsigned char set_sysclk12_buf2( enum sysclk12_buf2_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYS_CLK_12_BUF_CTRL_REG);

    /* 
     * 0: Sysclk12Buf1 desabled (default) 
     * 1: Sysclk12Buf1 is enabled 
     */ 
    switch( param ){
        case SYSCLK_12_BUF_2_ENABLE_E: 
           value = old_value | SYSCLK_12_BUF_2_MASK; 
           break;
        case SYSCLK_12_BUF_2_DISABLE_E: 
           value = old_value & ~ SYSCLK_12_BUF_2_MASK;
           break;
    }
  

    I2CWrite(SYS_CLK_12_BUF_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sysclk12_buf2
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 1:1
 *
 **************************************************************************/
unsigned char get_sysclk12_buf2()
  {
    unsigned char value;


    /* 
     * 0: Sysclk12Buf1 desabled (default) 
     * 1: Sysclk12Buf1 is enabled 
     */ 
    value = (I2CRead(SYS_CLK_12_BUF_CTRL_REG) & SYSCLK_12_BUF_2_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sysclk12_buf3
 *
 * IN   : param, a value to write to the regiter SysClk12BufCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 2:2
 *
 **************************************************************************/
unsigned char set_sysclk12_buf3( enum sysclk12_buf3_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYS_CLK_12_BUF_CTRL_REG);

    /* 
     * 0: Sysclk12Buf1 desabled (default) 
     * 1: Sysclk12Buf1 is enabled 
     */ 
    switch( param ){
        case SYSCLK_12_BUF_3_ENABLE_E: 
           value = old_value | SYSCLK_12_BUF_3_MASK; 
           break;
        case SYSCLK_12_BUF_3_DISABLE_E: 
           value = old_value & ~ SYSCLK_12_BUF_3_MASK;
           break;
    }
  

    I2CWrite(SYS_CLK_12_BUF_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sysclk12_buf3
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 2:2
 *
 **************************************************************************/
unsigned char get_sysclk12_buf3()
  {
    unsigned char value;


    /* 
     * 0: Sysclk12Buf1 desabled (default) 
     * 1: Sysclk12Buf1 is enabled 
     */ 
    value = (I2CRead(SYS_CLK_12_BUF_CTRL_REG) & SYSCLK_12_BUF_3_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sysclk12_buf4
 *
 * IN   : param, a value to write to the regiter SysClk12BufCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 3:3
 *
 **************************************************************************/
unsigned char set_sysclk12_buf4( enum sysclk12_buf4_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYS_CLK_12_BUF_CTRL_REG);

    /* 
     * 0: Sysclk12Buf1 desabled (default) 
     * 1: Sysclk12Buf1 is enabled 
     */ 
    switch( param ){
        case SYSCLK_12_BUF_4_ENABLE_E: 
           value = old_value | SYSCLK_12_BUF_4_MASK; 
           break;
        case SYSCLK_12_BUF_4_DISABLE_E: 
           value = old_value & ~ SYSCLK_12_BUF_4_MASK;
           break;
    }
  

    I2CWrite(SYS_CLK_12_BUF_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sysclk12_buf4
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 3:3
 *
 **************************************************************************/
unsigned char get_sysclk12_buf4()
  {
    unsigned char value;


    /* 
     * 0: Sysclk12Buf1 desabled (default) 
     * 1: Sysclk12Buf1 is enabled 
     */ 
    value = (I2CRead(SYS_CLK_12_BUF_CTRL_REG) & SYSCLK_12_BUF_4_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk12_buf_x_stre0
 *
 * IN   : param, a value to write to the regiter SysClk12BufCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clk12_buf_x_stre0( enum buffer_state param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYS_CLK_12_BUF_CTRL_REG);

    /* 
     * 0: Buffer strength low(default) 
     * 1: Buffer strength high 
     */ 
  

     value =  old_value & ~SYS_CLK_12_BUF_X_STRE_0_MASK ;

    switch(  param ){

           case BUFFER_STRENGTH_LOW_E:
                value  = value  | (BUFFER_STRENGTH_LOW << 0x4);
                break;
           case BUFFER_STRENGTH_HIGH_E:
                value  = value  | (BUFFER_STRENGTH_HIGH << 0x4);
                break;
    }
  

    I2CWrite(SYS_CLK_12_BUF_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk12_buf_x_stre0
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clk12_buf_x_stre0()
  {
    unsigned char value;


    /* 
     * 0: Buffer strength low(default) 
     * 1: Buffer strength high 
     */ 
    value = (I2CRead(SYS_CLK_12_BUF_CTRL_REG) & SYS_CLK_12_BUF_X_STRE_0_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk12_buf_x_stre1
 *
 * IN   : param, a value to write to the regiter SysClk12BufCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 5:5
 *
 **************************************************************************/
unsigned char set_sys_clk12_buf_x_stre1( enum buffer_state param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYS_CLK_12_BUF_CTRL_REG);

    /* 
     * 0: Buffer strength low(default) 
     * 1: Buffer strength high 
     */ 
  

     value =  old_value & ~SYS_CLK_12_BUF_X_STRE_1_MASK ;

    switch(  param ){

           case BUFFER_STRENGTH_LOW_E:
                value  = value  | (BUFFER_STRENGTH_LOW << 0x5);
                break;
           case BUFFER_STRENGTH_HIGH_E:
                value  = value  | (BUFFER_STRENGTH_HIGH << 0x5);
                break;
    }
  

    I2CWrite(SYS_CLK_12_BUF_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk12_buf_x_stre1
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 5:5
 *
 **************************************************************************/
unsigned char get_sys_clk12_buf_x_stre1()
  {
    unsigned char value;


    /* 
     * 0: Buffer strength low(default) 
     * 1: Buffer strength high 
     */ 
    value = (I2CRead(SYS_CLK_12_BUF_CTRL_REG) & SYS_CLK_12_BUF_X_STRE_1_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk12_buf_x_stre2
 *
 * IN   : param, a value to write to the regiter SysClk12BufCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 6:6
 *
 **************************************************************************/
unsigned char set_sys_clk12_buf_x_stre2( enum buffer_state param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYS_CLK_12_BUF_CTRL_REG);

    /* 
     * 0: Buffer strength low(default) 
     * 1: Buffer strength high 
     */ 
  

     value =  old_value & ~SYS_CLK_12_BUF_X_STRE_2_MASK ;

    switch(  param ){

           case BUFFER_STRENGTH_LOW_E:
                value  = value  | (BUFFER_STRENGTH_LOW << 0x6);
                break;
           case BUFFER_STRENGTH_HIGH_E:
                value  = value  | (BUFFER_STRENGTH_HIGH << 0x6);
                break;
    }
  

    I2CWrite(SYS_CLK_12_BUF_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk12_buf_x_stre2
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 6:6
 *
 **************************************************************************/
unsigned char get_sys_clk12_buf_x_stre2()
  {
    unsigned char value;


    /* 
     * 0: Buffer strength low(default) 
     * 1: Buffer strength high 
     */ 
    value = (I2CRead(SYS_CLK_12_BUF_CTRL_REG) & SYS_CLK_12_BUF_X_STRE_2_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk12_buf_x_stre3
 *
 * IN   : param, a value to write to the regiter SysClk12BufCtrl
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 7:7
 *
 **************************************************************************/
unsigned char set_sys_clk12_buf_x_stre3( enum buffer_state param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SYS_CLK_12_BUF_CTRL_REG);

    /* 
     * 0: Buffer strength low(default) 
     * 1: Buffer strength high 
     */ 
  

     value =  old_value & ~SYS_CLK_12_BUF_X_STRE_3_MASK ;

    switch(  param ){

           case BUFFER_STRENGTH_LOW_E:
                value  = value  | (BUFFER_STRENGTH_LOW << 0x7);
                break;
           case BUFFER_STRENGTH_HIGH_E:
                value  = value  | (BUFFER_STRENGTH_HIGH << 0x7);
                break;
    }
  

    I2CWrite(SYS_CLK_12_BUF_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk12_buf_x_stre3
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl
 *
 * Notes : From register 0x234, bits 7:7
 *
 **************************************************************************/
unsigned char get_sys_clk12_buf_x_stre3()
  {
    unsigned char value;


    /* 
     * 0: Buffer strength low(default) 
     * 1: Buffer strength high 
     */ 
    value = (I2CRead(SYS_CLK_12_BUF_CTRL_REG) & SYS_CLK_12_BUF_X_STRE_3_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk12_conf_ctr
 *
 * RET  : Return the value of register 0xSysClk12ConfCtr
 *
 * Notes : From register 0x235, bits 7:0
 *
 **************************************************************************/
unsigned char get_sys_clk12_conf_ctr()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(SYS_CLK_12_CONF_CTR_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk12_conf_ctr
 *
 * IN   : param, a value to write to the regiter SysClk12ConfCtr
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12ConfCtr
 *
 * Notes : From register 0x235, bits 7:0
 *
 **************************************************************************/
unsigned char set_sys_clk12_conf_ctr( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(SYS_CLK_12_CONF_CTR_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(SYS_CLK_12_CONF_CTR_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_sys_clk12_buf_ctrl2
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl2
 *
 * Notes : From register 0x236, bits 7:0
 *
 **************************************************************************/
unsigned char get_sys_clk12_buf_ctrl2()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(SYS_CLK_12_BUF_CTRL_2_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk12_buf_ctrl2
 *
 * IN   : param, a value to write to the regiter SysClk12BufCtrl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12BufCtrl2
 *
 * Notes : From register 0x236, bits 7:0
 *
 **************************************************************************/
unsigned char set_sys_clk12_buf_ctrl2( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(SYS_CLK_12_BUF_CTRL_2_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(SYS_CLK_12_BUF_CTRL_2_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_sys_clk12_buf1_valid
 *
 * RET  : Return the value of register 0xSysClk12Buf1Valid
 *
 * Notes : From register 0x237, bits 7:0
 *
 **************************************************************************/
unsigned char get_sys_clk12_buf1_valid()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(SYS_CLK_12_BUF_1_VALID_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk12_buf1_valid
 *
 * IN   : param, a value to write to the regiter SysClk12Buf1Valid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12Buf1Valid
 *
 * Notes : From register 0x237, bits 7:0
 *
 **************************************************************************/
unsigned char set_sys_clk12_buf1_valid( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(SYS_CLK_12_BUF_1_VALID_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(SYS_CLK_12_BUF_1_VALID_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_sys_clk12_buf2valid
 *
 * RET  : Return the value of register 0xSysClk12Buf2valid
 *
 * Notes : From register 0x238, bits 7:0
 *
 **************************************************************************/
unsigned char get_sys_clk12_buf2valid()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(SYS_CLK_12_BUF_2VALID_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk12_buf2valid
 *
 * IN   : param, a value to write to the regiter SysClk12Buf2valid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12Buf2valid
 *
 * Notes : From register 0x238, bits 7:0
 *
 **************************************************************************/
unsigned char set_sys_clk12_buf2valid( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(SYS_CLK_12_BUF_2VALID_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(SYS_CLK_12_BUF_2VALID_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_sys_clk12_buf3valid
 *
 * RET  : Return the value of register 0xSysClk12Buf3valid
 *
 * Notes : From register 0x239, bits 7:0
 *
 **************************************************************************/
unsigned char get_sys_clk12_buf3valid()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(SYS_CLK_12_BUF_3VALID_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk12_buf3valid
 *
 * IN   : param, a value to write to the regiter SysClk12Buf3valid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSysClk12Buf3valid
 *
 * Notes : From register 0x239, bits 7:0
 *
 **************************************************************************/
unsigned char set_sys_clk12_buf3valid( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(SYS_CLK_12_BUF_3VALID_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(SYS_CLK_12_BUF_3VALID_REG, param);
    return value;

  }




#ifdef __cplusplus
}
#endif

#endif
