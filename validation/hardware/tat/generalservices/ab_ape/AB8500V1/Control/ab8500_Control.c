/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Control/Linux/ab8500_Control.c
 * 
 *
 * Generated on the 23/06/2010 14:50 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2010
 *
 *  Reference document : AB8500-Registers-cut1_1-cut2_0-rev1.xls
 *
 ***************************************************************************/
#include "ab8500_Control.h"

extern unsigned char SPIRead( unsigned short register_address);
extern void SPIWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_usb_id_detect
 *
 * IN   : param, a value to write to the regiter SystemContolStatus
 * OUT  : 
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 6:6
 *
 **************************************************************************/
unsigned char set_usb_id_detect( enum usb_detect_on param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CONTOL_STATUS_REG);

    /* 
     * 0: UsbIDDetect is not a turn on event 
     * 1: UsbIDDetect is a turn on event 
     */ 
    switch( param )
      {
        case USB_DETECT_OFF_E: 
           value = old_value | USB_DETECT_ON_PARAM_MASK; 
           break;
        case USB_DETECT_ON_E: 
           value = old_value & ~ USB_DETECT_ON_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_CONTOL_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_id_detect
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 6:6
 *
 **************************************************************************/
unsigned char get_usb_id_detect()
  {
    unsigned char value;


    /* 
     * 0: UsbIDDetect is not a turn on event 
     * 1: UsbIDDetect is a turn on event 
     */ 
    value = (SPIRead(SYSTEM_CONTOL_STATUS_REG) & USB_DETECT_ON_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_dectect
 *
 * IN   : param, a value to write to the regiter SystemContolStatus
 * OUT  : 
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 5:5
 *
 **************************************************************************/
unsigned char set_vbus_dectect( enum vbus_detect_on param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CONTOL_STATUS_REG);

    /* 
     * 0: VbusDetect is not a turn on event 
     * 1: VbusDetect is a turn on event 
     */ 
    switch( param )
      {
        case VBUS_DETECT_OFF_E: 
           value = old_value | VBUS_DETECT_ON_PARAM_MASK; 
           break;
        case VBUS_DETECT_ON_E: 
           value = old_value & ~ VBUS_DETECT_ON_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_CONTOL_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_dectect
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 5:5
 *
 **************************************************************************/
unsigned char get_vbus_dectect()
  {
    unsigned char value;


    /* 
     * 0: VbusDetect is not a turn on event 
     * 1: VbusDetect is a turn on event 
     */ 
    value = (SPIRead(SYSTEM_CONTOL_STATUS_REG) & VBUS_DETECT_ON_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_ch_detect
 *
 * IN   : param, a value to write to the regiter SystemContolStatus
 * OUT  : 
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 4:4
 *
 **************************************************************************/
unsigned char set_main_ch_detect( enum main_ch_detect_on param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CONTOL_STATUS_REG);

    /* 
     * 0: MainChDetect is not a turn on event 
     * 1: MainChDetect is a turn on event 
     */ 
    switch( param )
      {
        case MAIN_CH_DETECT_OFF_E: 
           value = old_value | MAIN_CH_DETECT_ON_PARAM_MASK; 
           break;
        case MAIN_CH_DETECT_ON_E: 
           value = old_value & ~ MAIN_CH_DETECT_ON_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_CONTOL_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_ch_detect
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 4:4
 *
 **************************************************************************/
unsigned char get_main_ch_detect()
  {
    unsigned char value;


    /* 
     * 0: MainChDetect is not a turn on event 
     * 1: MainChDetect is a turn on event 
     */ 
    value = (SPIRead(SYSTEM_CONTOL_STATUS_REG) & MAIN_CH_DETECT_ON_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_alarm_detect
 *
 * IN   : param, a value to write to the regiter SystemContolStatus
 * OUT  : 
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 3:3
 *
 **************************************************************************/
unsigned char set_rtc_alarm_detect( enum rtc_alarm_detect_on param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CONTOL_STATUS_REG);

    /* 
     * 0: RtcAlarmDetect is not a turn on event 
     * 1: RtcAlarmDetect is a turn on event 
     */ 
    switch( param )
      {
        case RTC_ALARM_DETECT_OFF_E: 
           value = old_value | RTC_ALARM_DETECT_ON_PARAM_MASK; 
           break;
        case RTC_ALARM_DETECT_ON_E: 
           value = old_value & ~ RTC_ALARM_DETECT_ON_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_CONTOL_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_alarm_detect
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 3:3
 *
 **************************************************************************/
unsigned char get_rtc_alarm_detect()
  {
    unsigned char value;


    /* 
     * 0: RtcAlarmDetect is not a turn on event 
     * 1: RtcAlarmDetect is a turn on event 
     */ 
    value = (SPIRead(SYSTEM_CONTOL_STATUS_REG) & RTC_ALARM_DETECT_ON_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_power_on_key2_detect
 *
 * IN   : param, a value to write to the regiter SystemContolStatus
 * OUT  : 
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 2:2
 *
 **************************************************************************/
unsigned char set_power_on_key2_detect( enum key_detected param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CONTOL_STATUS_REG);

    /* 
     * 0: PowerOnKey2Detect is not a turn on event 
     * 1: PowerOnKey2Detect is a turn on event 
     */ 
    switch( param )
      {
        case TURN_ON_E: 
           value = old_value | POWER_ON_KEY_2_DETECT_PARAM_MASK; 
           break;
        case NOT_TURN_ON_E: 
           value = old_value & ~ POWER_ON_KEY_2_DETECT_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_CONTOL_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_power_on_key2_detect
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 2:2
 *
 **************************************************************************/
unsigned char get_power_on_key2_detect()
  {
    unsigned char value;


    /* 
     * 0: PowerOnKey2Detect is not a turn on event 
     * 1: PowerOnKey2Detect is a turn on event 
     */ 
    value = (SPIRead(SYSTEM_CONTOL_STATUS_REG) & POWER_ON_KEY_2_DETECT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_power_on_key1_detect
 *
 * IN   : param, a value to write to the regiter SystemContolStatus
 * OUT  : 
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 1:1
 *
 **************************************************************************/
unsigned char set_power_on_key1_detect( enum key_detected param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CONTOL_STATUS_REG);

    /* 
     * 0: PowerOnKey1Detect is not a turn on event 
     * 1: PowerOnKey1Detect is a turn on event 
     */ 
    switch( param )
      {
        case TURN_ON_E: 
           value = old_value | POWER_ON_KEY_1_DETECT_PARAM_MASK; 
           break;
        case NOT_TURN_ON_E: 
           value = old_value & ~ POWER_ON_KEY_1_DETECT_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_CONTOL_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_power_on_key1_detect
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 1:1
 *
 **************************************************************************/
unsigned char get_power_on_key1_detect()
  {
    unsigned char value;


    /* 
     * 0: PowerOnKey1Detect is not a turn on event 
     * 1: PowerOnKey1Detect is a turn on event 
     */ 
    value = (SPIRead(SYSTEM_CONTOL_STATUS_REG) & POWER_ON_KEY_1_DETECT_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_detect
 *
 * IN   : param, a value to write to the regiter SystemContolStatus
 * OUT  : 
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 0:0
 *
 **************************************************************************/
unsigned char set_battery_detect( enum battery_detect_on param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CONTOL_STATUS_REG);

    /* 
     * 0: PowerOnKey1Detect is not a turn on event 
     * 1: PowerOnKey1Detect is a turn on event 
     */ 
    switch( param )
      {
        case BATTERY_DETECT_OFF_E: 
           value = old_value | BATTERY_DETECT_ON_PARAM_MASK; 
           break;
        case BATTERY_DETECT_ON_E: 
           value = old_value & ~ BATTERY_DETECT_ON_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_CONTOL_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_detect
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0100, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_detect()
  {
    unsigned char value;


    /* 
     * 0: PowerOnKey1Detect is not a turn on event 
     * 1: PowerOnKey1Detect is a turn on event 
     */ 
    value = (SPIRead(SYSTEM_CONTOL_STATUS_REG) & BATTERY_DETECT_ON_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_software_reset_flag
 *
 * RET  : Return the value of register ResetStatus
 *
 * Notes : From register 0x0101, bits 2:2
 *
 **************************************************************************/
unsigned char get_software_reset_flag()
  {
    unsigned char value;


    /* 
     * Software Reset status 
     */ 
    value = (SPIRead(RESET_STATUS_REG) & SOFTWARE_RESET_STATUS_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_reset4500
 *
 * RET  : Return the value of register ResetStatus
 *
 * Notes : From register 0x0101, bits 0:0
 *
 **************************************************************************/
unsigned char get_reset4500()
  {
    unsigned char value;


    /* 
     * Reset 4500 status 
     */ 
    value = (SPIRead(RESET_STATUS_REG) & RESET_4500_STATUS_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_power_on_key1_press_time
 *
 * IN   : param, a value to write to the regiter SystemContolStatus
 * OUT  : 
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0102, bits 6:0
 *
 **************************************************************************/
unsigned char set_power_on_key1_press_time( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CONTOL_STATUS_REG);

    /* 
     * A 7 bit counter is used to measure PonKey1db pressure 
     * duration with steps 
     * of 125 msec and a range of 10 sec. PonKey1PressTime[6:0] 
     * indicates number of 125ms steps. 
     */ 
  
    value =  old_value & ~PRESS_TIME_PARAM_MASK;


    value |=  param ;  

    SPIWrite(SYSTEM_CONTOL_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_power_on_key1_press_time
 *
 * RET  : Return the value of register SystemContolStatus
 *
 * Notes : From register 0x0102, bits 6:0
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
    value = (SPIRead(SYSTEM_CONTOL_STATUS_REG) & PRESS_TIME_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req
 *
 * IN   : param, a value to write to the regiter SysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register SysClkReq
 *
 * Notes : From register 0x0142, bits 7:0
 *
 **************************************************************************/
unsigned char set_sys_clk_req( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(SYS_CLK_REQ_REG);

    /* 
     * SysClkReq ball status (for production testing). 
     */ 
    SPIWrite(SYS_CLK_REQ_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_sys_clk_req
 *
 * RET  : Return the value of register SysClkReq
 *
 * Notes : From register 0x0142, bits 7:0
 *
 **************************************************************************/
unsigned char get_sys_clk_req()
  {
    unsigned char value;


    /* 
     * SysClkReq ball status (for production testing). 
     */ 
    value = SPIRead(SYS_CLK_REQ_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_thermal_software_off
 *
 * IN   : param, a value to write to the regiter Control
 * OUT  : 
 *
 * RET  : Return the value of register Control
 *
 * Notes : From register 0x0180, bits 2:2
 *
 **************************************************************************/
unsigned char set_thermal_software_off( enum thermal_software_off param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_REG);

    /* 
     * Thermal DB8500 Software Off 
     */ 
    switch( param )
      {
        case THERMAL_SOFTWARE_OFF_E: 
           value = old_value | THERMAL_SOFTWARE_OFF_PARAM_MASK; 
           break;
        case THERMAL_SOFTWARE_ON_E: 
           value = old_value & ~ THERMAL_SOFTWARE_OFF_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_thermal_software_off
 *
 * RET  : Return the value of register Control
 *
 * Notes : From register 0x0180, bits 2:2
 *
 **************************************************************************/
unsigned char get_thermal_software_off()
  {
    unsigned char value;


    /* 
     * Thermal DB8500 Software Off 
     */ 
    value = (SPIRead(CONTROL_REG) & THERMAL_SOFTWARE_OFF_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_software_reset
 *
 * IN   : param, a value to write to the regiter Control
 * OUT  : 
 *
 * RET  : Return the value of register Control
 *
 * Notes : From register 0x0180, bits 1:1
 *
 **************************************************************************/
unsigned char set_software_reset( enum software_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_REG);

    /* 
     * 0: Reset STw4500 registers 
     * 1: inactive 
     */ 
    switch( param )
      {
        case SOFTWARE_RESET_E: 
           value = old_value | SOFTWARE_RESET_PARAM_MASK; 
           break;
        case SOFTWARE_UN_RESET_E: 
           value = old_value & ~ SOFTWARE_RESET_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_software_reset
 *
 * RET  : Return the value of register Control
 *
 * Notes : From register 0x0180, bits 1:1
 *
 **************************************************************************/
unsigned char get_software_reset()
  {
    unsigned char value;


    /* 
     * 0: Reset STw4500 registers 
     * 1: inactive 
     */ 
    value = (SPIRead(CONTROL_REG) & SOFTWARE_RESET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_software_off
 *
 * IN   : param, a value to write to the regiter Control
 * OUT  : 
 *
 * RET  : Return the value of register Control
 *
 * Notes : From register 0x0180, bits 0:0
 *
 **************************************************************************/
unsigned char set_software_off( enum software_off param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: Turn off STw4500 
     */ 
    switch( param )
      {
        case TURN_OFF_STW4500_INACTIVE_E: 
           value = old_value | SOFTWARE_OFF_PARAM_MASK; 
           break;
        case TURN_OFF_STW4500_E: 
           value = old_value & ~ SOFTWARE_OFF_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_software_off
 *
 * RET  : Return the value of register Control
 *
 * Notes : From register 0x0180, bits 0:0
 *
 **************************************************************************/
unsigned char get_software_off()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Turn off STw4500 
     */ 
    value = (SPIRead(CONTROL_REG) & SOFTWARE_OFF_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vsmps1_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 7:7
 *
 **************************************************************************/
unsigned char set_reset_vsmps1_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_2_REG);

    /* 
     * 0: Doesn't valid reset of 'Vsmps1Regu[1:0]', 
     * 'Vsmps1RequestCtrl[1:0]' and Vsmps1Sel(i)[5:0] bits when 
     * STW4500 is reset 
     * 1: Allow to valid reset of 'Vsmps1Regu[1:0]', 
     * 'Vsmps1RequestCtrl[1:0]' and 'Vsmps1Sel(i)[5:0]' bits 
     * when STW4500 is reset 
     */ 
    switch( param )
      {
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VSMPS_1_VALID_PARAM_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VSMPS_1_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vsmps1_valid
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 7:7
 *
 **************************************************************************/
unsigned char get_reset_vsmps1_valid()
  {
    unsigned char value;


    /* 
     * 0: Doesn't valid reset of 'Vsmps1Regu[1:0]', 
     * 'Vsmps1RequestCtrl[1:0]' and Vsmps1Sel(i)[5:0] bits when 
     * STW4500 is reset 
     * 1: Allow to valid reset of 'Vsmps1Regu[1:0]', 
     * 'Vsmps1RequestCtrl[1:0]' and 'Vsmps1Sel(i)[5:0]' bits 
     * when STW4500 is reset 
     */ 
    value = (SPIRead(CONTROL_2_REG) & RESET_VSMPS_1_VALID_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vext_supply3_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 6:6
 *
 **************************************************************************/
unsigned char set_reset_vext_supply3_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_2_REG);

    /* 
     * 0: Doesn't valid reset of 'VextSupply3Regu[1:0]', 
     * 'VextSupply3RequestCtrl[1:0] ExtSup13LPnClkSel and 
     * ExtSupply13LP bits when STW4500 is reset 
     * 1: Allow to valid reset of 'VextSupply3Regu[1:0]', 
     * 'VextSupply3RequestCtrl[1:'ExtSup13LPnClkSel' and 
     * 'ExtSupply13LP' bits when STW4500 is reset 
     */ 
    switch( param )
      {
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VEXT_SUPPLY_3_VALID_PARAM_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VEXT_SUPPLY_3_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vext_supply3_valid
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 6:6
 *
 **************************************************************************/
unsigned char get_reset_vext_supply3_valid()
  {
    unsigned char value;


    /* 
     * 0: Doesn't valid reset of 'VextSupply3Regu[1:0]', 
     * 'VextSupply3RequestCtrl[1:0] ExtSup13LPnClkSel and 
     * ExtSupply13LP bits when STW4500 is reset 
     * 1: Allow to valid reset of 'VextSupply3Regu[1:0]', 
     * 'VextSupply3RequestCtrl[1:'ExtSup13LPnClkSel' and 
     * 'ExtSupply13LP' bits when STW4500 is reset 
     */ 
    value = (SPIRead(CONTROL_2_REG) & RESET_VEXT_SUPPLY_3_VALID_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vext_supply2_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 5:5
 *
 **************************************************************************/
unsigned char set_reset_vext_supply2_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_2_REG);

    /* 
     * 0: Doesn't valid reset of 'VextSupply2Regu[1:0]', 
     * 'VextSupply2RequestCtrl[1:0] ExtSup12LPnClkSel and 
     * ExtSupply12LP bits when STW4500 is reset 
     * 1: Allow to valid reset of 'VextSupply2Regu[1:0]', 
     * 'VextSupply2RequestCtrl[1:'ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when STW4500 is reset 
     */ 
    switch( param )
      {
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VEXT_SUPPLY_2_VALID_PARAM_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VEXT_SUPPLY_2_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vext_supply2_valid
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 5:5
 *
 **************************************************************************/
unsigned char get_reset_vext_supply2_valid()
  {
    unsigned char value;


    /* 
     * 0: Doesn't valid reset of 'VextSupply2Regu[1:0]', 
     * 'VextSupply2RequestCtrl[1:0] ExtSup12LPnClkSel and 
     * ExtSupply12LP bits when STW4500 is reset 
     * 1: Allow to valid reset of 'VextSupply2Regu[1:0]', 
     * 'VextSupply2RequestCtrl[1:'ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when STW4500 is reset 
     */ 
    value = (SPIRead(CONTROL_2_REG) & RESET_VEXT_SUPPLY_2_VALID_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vext_supply_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 4:4
 *
 **************************************************************************/
unsigned char set_reset_vext_supply_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_2_REG);

    /* 
     * 0: Doesn't valid reset of 'VextSupply1Regu[1:0]', 
     * 'VextSupply1RequestCtrl[1:0]','ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when STW4500 is reset 
     * 1: Allow to valid reset of 'VextSupply1Regu[1:0]', 
     * 'VextSupply1RequestCtrl[1:0]', 'ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when STW4500 is reset 
     */ 
    switch( param )
      {
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VEXT_SUPPLY_VALID_PARAM_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VEXT_SUPPLY_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vext_supply_valid
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 4:4
 *
 **************************************************************************/
unsigned char get_reset_vext_supply_valid()
  {
    unsigned char value;


    /* 
     * 0: Doesn't valid reset of 'VextSupply1Regu[1:0]', 
     * 'VextSupply1RequestCtrl[1:0]','ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when STW4500 is reset 
     * 1: Allow to valid reset of 'VextSupply1Regu[1:0]', 
     * 'VextSupply1RequestCtrl[1:0]', 'ExtSup12LPnClkSel' and 
     * 'ExtSupply12LP' bits when STW4500 is reset 
     */ 
    value = (SPIRead(CONTROL_2_REG) & RESET_VEXT_SUPPLY_VALID_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vmod_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 3:3
 *
 **************************************************************************/
unsigned char set_reset_vmod_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_2_REG);

    /* 
     * 0: VmodRegu, VmodRequestCtrl and VmodSel bits stay at 
     * their programmed value when STw4500 is resetted 
     * 1: VmodRegu, VmodRequestCtrl and Vaux3Sel bits are reset 
     * to their default values when STw4500 is resetted 
     */ 
    switch( param )
      {
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VMOD_VALID_PARAM_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VMOD_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vmod_valid
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 3:3
 *
 **************************************************************************/
unsigned char get_reset_vmod_valid()
  {
    unsigned char value;


    /* 
     * 0: VmodRegu, VmodRequestCtrl and VmodSel bits stay at 
     * their programmed value when STw4500 is resetted 
     * 1: VmodRegu, VmodRequestCtrl and Vaux3Sel bits are reset 
     * to their default values when STw4500 is resetted 
     */ 
    value = (SPIRead(CONTROL_2_REG) & RESET_VMOD_VALID_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vaux3_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 2:2
 *
 **************************************************************************/
unsigned char set_reset_vaux3_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_2_REG);

    /* 
     * 0: Vaux3Regu, Vaux3RequestCtrl and Vaux3Sel bits stay at 
     * their programmed value when STw4500 is resetted 
     * 1: Vaux3Regu, Vaux3RequestCtrl and Vaux3Sel bits are 
     * reset to their default values when STw4500 is resetted 
     */ 
    switch( param )
      {
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VAUX_3_VALID_PARAM_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VAUX_3_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vaux3_valid
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 2:2
 *
 **************************************************************************/
unsigned char get_reset_vaux3_valid()
  {
    unsigned char value;


    /* 
     * 0: Vaux3Regu, Vaux3RequestCtrl and Vaux3Sel bits stay at 
     * their programmed value when STw4500 is resetted 
     * 1: Vaux3Regu, Vaux3RequestCtrl and Vaux3Sel bits are 
     * reset to their default values when STw4500 is resetted 
     */ 
    value = (SPIRead(CONTROL_2_REG) & RESET_VAUX_3_VALID_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vaux2_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 1:1
 *
 **************************************************************************/
unsigned char set_reset_vaux2_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_2_REG);

    /* 
     * 0: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits stay at 
     * their programmed value when STw4500 is resetted 
     * 1: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits are 
     * reset to their default values when STw4500 is resetted 
     */ 
    switch( param )
      {
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VAUX_2_VALID_PARAM_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VAUX_2_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vaux2_valid
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 1:1
 *
 **************************************************************************/
unsigned char get_reset_vaux2_valid()
  {
    unsigned char value;


    /* 
     * 0: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits stay at 
     * their programmed value when STw4500 is resetted 
     * 1: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits are 
     * reset to their default values when STw4500 is resetted 
     */ 
    value = (SPIRead(CONTROL_2_REG) & RESET_VAUX_2_VALID_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_vaux1_valid
 *
 * IN   : param, a value to write to the regiter Control2
 * OUT  : 
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 0:0
 *
 **************************************************************************/
unsigned char set_reset_vaux1_valid( enum reset_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_2_REG);

    /* 
     * 0: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits stay at 
     * their programmed value when STw4500 is resetted 
     * 1: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits are 
     * reset to their default values when STw4500 is resetted 
     */ 
    switch( param )
      {
        case RESET_TO_DEFAULT_ON_RESET_E: 
           value = old_value | RESET_VAUX_1_VALID_PARAM_MASK; 
           break;
        case STAY_PROGRAMMED_ON_RESET_E: 
           value = old_value & ~ RESET_VAUX_1_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_vaux1_valid
 *
 * RET  : Return the value of register Control2
 *
 * Notes : From register 0x0181, bits 0:0
 *
 **************************************************************************/
unsigned char get_reset_vaux1_valid()
  {
    unsigned char value;


    /* 
     * 0: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits stay at 
     * their programmed value when STw4500 is resetted 
     * 1: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits are 
     * reset to their default values when STw4500 is resetted 
     */ 
    value = (SPIRead(CONTROL_2_REG) & RESET_VAUX_1_VALID_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_thermal_shutdown
 *
 * IN   : param, a value to write to the regiter Control3
 * OUT  : 
 *
 * RET  : Return the value of register Control3
 *
 * Notes : From register 0x0200, bits 3:3
 *
 **************************************************************************/
unsigned char set_thermal_shutdown( enum thermal_shutdown_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_3_REG);

    /* 
     * 0: disable thermal shutdown 
     * 1: enable thermal shutdown 
     */ 
    switch( param )
      {
        case THERMAL_SHUTDOWN_DISABLE_E: 
           value = old_value | THERMAL_SHUTDOWN_ENABLE_PARAM_MASK; 
           break;
        case THERMAL_SHUTDOWN_ENABLE_E: 
           value = old_value & ~ THERMAL_SHUTDOWN_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_thermal_shutdown
 *
 * RET  : Return the value of register Control3
 *
 * Notes : From register 0x0200, bits 3:3
 *
 **************************************************************************/
unsigned char get_thermal_shutdown()
  {
    unsigned char value;


    /* 
     * 0: disable thermal shutdown 
     * 1: enable thermal shutdown 
     */ 
    value = (SPIRead(CONTROL_3_REG) & THERMAL_SHUTDOWN_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_video_denc_reset
 *
 * IN   : param, a value to write to the regiter Control3
 * OUT  : 
 *
 * RET  : Return the value of register Control3
 *
 * Notes : From register 0x0200, bits 2:2
 *
 **************************************************************************/
unsigned char set_video_denc_reset( enum video_denc_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_3_REG);

    /* 
     * 0: reset VideoDenc registers 
     * 1: inactive 
     */ 
    switch( param )
      {
        case VIDEO_DENC_RESET_E: 
           value = old_value | VIDEO_DENC_RESET_PARAM_MASK; 
           break;
        case VIDEO_DENC_UN_RESET_E: 
           value = old_value & ~ VIDEO_DENC_RESET_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_video_denc_reset
 *
 * RET  : Return the value of register Control3
 *
 * Notes : From register 0x0200, bits 2:2
 *
 **************************************************************************/
unsigned char get_video_denc_reset()
  {
    unsigned char value;


    /* 
     * 0: reset VideoDenc registers 
     * 1: inactive 
     */ 
    value = (SPIRead(CONTROL_3_REG) & VIDEO_DENC_RESET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_reset
 *
 * IN   : param, a value to write to the regiter Control3
 * OUT  : 
 *
 * RET  : Return the value of register Control3
 *
 * Notes : From register 0x0200, bits 1:1
 *
 **************************************************************************/
unsigned char set_audio_reset( enum audio_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_3_REG);

    /* 
     * 0: reset Audio registers 
     * 1: inactive 
     */ 
    switch( param )
      {
        case AUDIO_RESET_E: 
           value = old_value | AUDIO_RESET_PARAM_MASK; 
           break;
        case AUDIO_UN_RESET_E: 
           value = old_value & ~ AUDIO_RESET_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_reset
 *
 * RET  : Return the value of register Control3
 *
 * Notes : From register 0x0200, bits 1:1
 *
 **************************************************************************/
unsigned char get_audio_reset()
  {
    unsigned char value;


    /* 
     * 0: reset Audio registers 
     * 1: inactive 
     */ 
    value = (SPIRead(CONTROL_3_REG) & AUDIO_RESET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_clock32
 *
 * IN   : param, a value to write to the regiter Control3
 * OUT  : 
 *
 * RET  : Return the value of register Control3
 *
 * Notes : From register 0x0200, bits 0:0
 *
 **************************************************************************/
unsigned char set_clock32( enum clock32_output_buffer_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_3_REG);

    /* 
     * 0: inactive 1: disable Clk32KOut2 output buffer 
     */ 
    switch( param )
      {
        case CLOCK_32_OUTPUT_BUFFER_ENABLE_E: 
           value = old_value | CLOCK_32_OUTPUT_BUFFER_ENABLE_PARAM_MASK; 
           break;
        case CLOCK_32_OUTPUT_BUFFER_DISABLE_E: 
           value = old_value & ~ CLOCK_32_OUTPUT_BUFFER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_clock32
 *
 * RET  : Return the value of register Control3
 *
 * Notes : From register 0x0200, bits 0:0
 *
 **************************************************************************/
unsigned char get_clock32()
  {
    unsigned char value;


    /* 
     * 0: inactive 1: disable Clk32KOut2 output buffer 
     */ 
    value = (SPIRead(CONTROL_3_REG) & CLOCK_32_OUTPUT_BUFFER_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_turn_on_valid_on_expire
 *
 * IN   : param, a value to write to the regiter MainWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register MainWatchDogControl
 *
 * Notes : From register 0x0201, bits 4:4
 *
 **************************************************************************/
unsigned char set_turn_on_valid_on_expire( enum main_watch_dog_write_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(MAIN_WATCH_DOG_CONTROL_REG);

    /* 
     * 0: when Watchdog expires STW4500 is turned off 
     * 1: when Watchdog expires STW4500 is turned off and then 
     * turned On running start up sequence. 
     */ 
    switch( param )
      {
        case STAY_TURNED_OFF_E: 
           value = old_value | MAIN_WATCH_DOG_WRITE_ENABLE_PARAM_MASK; 
           break;
        case TURN_OFF_AND_STARTUP_E: 
           value = old_value & ~ MAIN_WATCH_DOG_WRITE_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(MAIN_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_turn_on_valid_on_expire
 *
 * RET  : Return the value of register MainWatchDogControl
 *
 * Notes : From register 0x0201, bits 4:4
 *
 **************************************************************************/
unsigned char get_turn_on_valid_on_expire()
  {
    unsigned char value;


    /* 
     * 0: when Watchdog expires STW4500 is turned off 
     * 1: when Watchdog expires STW4500 is turned off and then 
     * turned On running start up sequence. 
     */ 
    value = (SPIRead(MAIN_WATCH_DOG_CONTROL_REG) & MAIN_WATCH_DOG_WRITE_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_watchdog_kick
 *
 * IN   : param, a value to write to the regiter MainWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register MainWatchDogControl
 *
 * Notes : From register 0x0201, bits 1:1
 *
 **************************************************************************/
unsigned char set_main_watchdog_kick( enum kick_main_watchdog_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(MAIN_WATCH_DOG_CONTROL_REG);

    /* 
     * Kick Main watchdog. Auto-cleared 
     */ 
    switch( param )
      {
        case KICK_MAIN_WATCHDOG_DISABLE_E: 
           value = old_value | KICK_MAIN_WATCHDOG_ENABLE_PARAM_MASK; 
           break;
        case KICK_MAIN_WATCHDOG_ENABLE_E: 
           value = old_value & ~ KICK_MAIN_WATCHDOG_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(MAIN_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_watchdog_kick
 *
 * RET  : Return the value of register MainWatchDogControl
 *
 * Notes : From register 0x0201, bits 1:1
 *
 **************************************************************************/
unsigned char get_main_watchdog_kick()
  {
    unsigned char value;


    /* 
     * Kick Main watchdog. Auto-cleared 
     */ 
    value = (SPIRead(MAIN_WATCH_DOG_CONTROL_REG) & KICK_MAIN_WATCHDOG_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_watchdog_enable
 *
 * IN   : param, a value to write to the regiter MainWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register MainWatchDogControl
 *
 * Notes : From register 0x0201, bits 0:0
 *
 **************************************************************************/
unsigned char set_main_watchdog_enable( enum main_watchdog_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(MAIN_WATCH_DOG_CONTROL_REG);

    /* 
     * Enable Main watchdog 
     */ 
    switch( param )
      {
        case MAIN_WATCHDOG_DISABLE_E: 
           value = old_value | MAIN_WATCHDOG_ENABLE_PARAM_MASK; 
           break;
        case MAIN_WATCHDOG_ENABLE_E: 
           value = old_value & ~ MAIN_WATCHDOG_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(MAIN_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_watchdog_enable
 *
 * RET  : Return the value of register MainWatchDogControl
 *
 * Notes : From register 0x0201, bits 0:0
 *
 **************************************************************************/
unsigned char get_main_watchdog_enable()
  {
    unsigned char value;


    /* 
     * Enable Main watchdog 
     */ 
    value = (SPIRead(MAIN_WATCH_DOG_CONTROL_REG) & MAIN_WATCHDOG_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_watchdog_timer
 *
 * IN   : param, a value to write to the regiter MainWatchDogTimer
 * OUT  : 
 *
 * RET  : Return the value of register MainWatchDogTimer
 *
 * Notes : From register 0x0202, bits 6:0
 *
 **************************************************************************/
unsigned char set_main_watchdog_timer( enum main_watchdog_timer param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(MAIN_WATCH_DOG_TIMER_REG);

    /* 
     * Defined Main watchdog time from 1s to 128s by 1s steps, 
     * default value is 32s 
     */ 
  
    value =  old_value & ~MAIN_WATCHDOG_TIMER_PARAM_MASK;


     value |=  param ;  
    SPIWrite(MAIN_WATCH_DOG_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_watchdog_timer
 *
 * RET  : Return the value of register MainWatchDogTimer
 *
 * Notes : From register 0x0202, bits 6:0
 *
 **************************************************************************/
unsigned char get_main_watchdog_timer()
  {
    unsigned char value;


    /* 
     * Defined Main watchdog time from 1s to 128s by 1s steps, 
     * default value is 32s 
     */ 
    value = (SPIRead(MAIN_WATCH_DOG_TIMER_REG) & MAIN_WATCHDOG_TIMER_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_bat_threshold
 *
 * IN   : param, a value to write to the regiter MainWatchDogTimer
 * OUT  : 
 *
 * RET  : Return the value of register MainWatchDogTimer
 *
 * Notes : From register 0x0203, bits 6:1
 *
 **************************************************************************/
unsigned char set_low_bat_threshold( enum low_bat_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(MAIN_WATCH_DOG_TIMER_REG);

    /* 
     * Defined LowBat threshold from 2.3v to 3.85v 25mv steps 
     */ 
  
    value =  old_value & ~LOW_BAT_THRESHOLD_PARAM_MASK;


     value |= ( param << 0x1);  
    SPIWrite(MAIN_WATCH_DOG_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_bat_threshold
 *
 * RET  : Return the value of register MainWatchDogTimer
 *
 * Notes : From register 0x0203, bits 6:1
 *
 **************************************************************************/
unsigned char get_low_bat_threshold()
  {
    unsigned char value;


    /* 
     * Defined LowBat threshold from 2.3v to 3.85v 25mv steps 
     */ 
    value = (SPIRead(MAIN_WATCH_DOG_TIMER_REG) & LOW_BAT_THRESHOLD_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_battery_enable
 *
 * IN   : param, a value to write to the regiter MainWatchDogTimer
 * OUT  : 
 *
 * RET  : Return the value of register MainWatchDogTimer
 *
 * Notes : From register 0x0203, bits 0:0
 *
 **************************************************************************/
unsigned char set_low_battery_enable( enum low_battery_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(MAIN_WATCH_DOG_TIMER_REG);

    /* 
     * 0: disable LowBat function 
     * 1: enable LowBat function 
     */ 
    switch( param )
      {
        case LOW_BATTERY_DISABLE_E: 
           value = old_value | LOW_BATTERY_ENABLE_PARAM_MASK; 
           break;
        case LOW_BATTERY_ENABLE_E: 
           value = old_value & ~ LOW_BATTERY_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(MAIN_WATCH_DOG_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_battery_enable
 *
 * RET  : Return the value of register MainWatchDogTimer
 *
 * Notes : From register 0x0203, bits 0:0
 *
 **************************************************************************/
unsigned char get_low_battery_enable()
  {
    unsigned char value;


    /* 
     * 0: disable LowBat function 
     * 1: enable LowBat function 
     */ 
    value = (SPIRead(MAIN_WATCH_DOG_TIMER_REG) & LOW_BATTERY_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_ok2_threshold
 *
 * IN   : param, a value to write to the regiter BatteryOkThreshold
 * OUT  : 
 *
 * RET  : Return the value of register BatteryOkThreshold
 *
 * Notes : From register 0x0204, bits 7:4
 *
 **************************************************************************/
unsigned char set_battery_ok2_threshold( enum battery_ok2_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(BATTERY_OK_THRESHOLD_REG);

    /* 
     * BattOk2 falling thresholds management 
     */ 
  
    value =  old_value & ~BATTERY_OK_2_THRESHOLD_PARAM_MASK;


     value |= ( param << 0x4);  
    SPIWrite(BATTERY_OK_THRESHOLD_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_ok2_threshold
 *
 * RET  : Return the value of register BatteryOkThreshold
 *
 * Notes : From register 0x0204, bits 7:4
 *
 **************************************************************************/
unsigned char get_battery_ok2_threshold()
  {
    unsigned char value;


    /* 
     * BattOk2 falling thresholds management 
     */ 
    value = (SPIRead(BATTERY_OK_THRESHOLD_REG) & BATTERY_OK_2_THRESHOLD_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_ok1_threshold
 *
 * IN   : param, a value to write to the regiter BatteryOkThreshold
 * OUT  : 
 *
 * RET  : Return the value of register BatteryOkThreshold
 *
 * Notes : From register 0x0204, bits 3:0
 *
 **************************************************************************/
unsigned char set_battery_ok1_threshold( enum battery_ok1_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(BATTERY_OK_THRESHOLD_REG);

    /* 
     * BattOk1 falling thresholds management 
     */ 
  
    value =  old_value & ~BATTERY_OK_1_THRESHOLD_PARAM_MASK;


     value |=  param ;  
    SPIWrite(BATTERY_OK_THRESHOLD_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_ok1_threshold
 *
 * RET  : Return the value of register BatteryOkThreshold
 *
 * Notes : From register 0x0204, bits 3:0
 *
 **************************************************************************/
unsigned char get_battery_ok1_threshold()
  {
    unsigned char value;


    /* 
     * BattOk1 falling thresholds management 
     */ 
    value = (SPIRead(BATTERY_OK_THRESHOLD_REG) & BATTERY_OK_1_THRESHOLD_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_system_clock_timer_adjustement
 *
 * IN   : param, a value to write to the regiter SystemClockTimer
 * OUT  : 
 *
 * RET  : Return the value of register SystemClockTimer
 *
 * Notes : From register 0x205, bits 7:4
 *
 **************************************************************************/
unsigned char set_system_clock_timer_adjustement( enum timer_adjustement param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CLOCK_TIMER_REG);

    /* 
     * System clock timer management. Adjustement value multiple 
     * of 32 Khz 
     */ 
  
    value =  old_value & ~TIMER_ADJUSTEMENT_PARAM_MASK;


     value |= ( param << 0x4);  
    SPIWrite(SYSTEM_CLOCK_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_system_clock_timer_adjustement
 *
 * RET  : Return the value of register SystemClockTimer
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
    value = (SPIRead(SYSTEM_CLOCK_TIMER_REG) & TIMER_ADJUSTEMENT_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_system_clock_timer
 *
 * IN   : param, a value to write to the regiter SystemClockTimer
 * OUT  : 
 *
 * RET  : Return the value of register SystemClockTimer
 *
 * Notes : From register 0x205, bits 3:0
 *
 **************************************************************************/
unsigned char set_system_clock_timer( enum clock_timer param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_CLOCK_TIMER_REG);

    /* 
     * System clock timer management. value multiple of 32 Khz 
     */ 
  
    value =  old_value & ~CLOCK_TIMER_PARAM_MASK;


     value |=  param ;  
    SPIWrite(SYSTEM_CLOCK_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_system_clock_timer
 *
 * RET  : Return the value of register SystemClockTimer
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
    value = (SPIRead(SYSTEM_CLOCK_TIMER_REG) & CLOCK_TIMER_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_force_smps_clock
 *
 * IN   : param, a value to write to the regiter SmpsClockControl
 * OUT  : 
 *
 * RET  : Return the value of register SmpsClockControl
 *
 * Notes : From register 0x206, bits 2:2
 *
 **************************************************************************/
unsigned char set_force_smps_clock( enum force_smps_clock_on param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SMPS_CLOCK_CONTROL_REG);

    /* 
     * Force internal 3.2MHz oscillator ON 
     */ 
    switch( param )
      {
        case FORCE_SMPS_CLOCK_OFF_E: 
           value = old_value | FORCE_SMPS_CLOCK_ON_PARAM_MASK; 
           break;
        case FORCE_SMPS_CLOCK_ON_E: 
           value = old_value & ~ FORCE_SMPS_CLOCK_ON_PARAM_MASK;
           break;
      }
  

    SPIWrite(SMPS_CLOCK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_force_smps_clock
 *
 * RET  : Return the value of register SmpsClockControl
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
    value = (SPIRead(SMPS_CLOCK_CONTROL_REG) & FORCE_SMPS_CLOCK_ON_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_smps_clock_selection
 *
 * IN   : param, a value to write to the regiter SmpsClockControl
 * OUT  : 
 *
 * RET  : Return the value of register SmpsClockControl
 *
 * Notes : From register 0x206, bits 1:0
 *
 **************************************************************************/
unsigned char set_smps_clock_selection( enum smps_clock_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SMPS_CLOCK_CONTROL_REG);

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
  

     value =  old_value & ~SMPS_CLOCK_SELECTION_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(SMPS_CLOCK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_smps_clock_selection
 *
 * RET  : Return the value of register SmpsClockControl
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
    value = (SPIRead(SMPS_CLOCK_CONTROL_REG) & SMPS_CLOCK_SELECTION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase
 * OUT  : 
 *
 * RET  : Return the value of register SmpsClockPhase
 *
 * Notes : From register 0x207, bits 5:3
 *
 **************************************************************************/
unsigned char set_vape_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SMPS_CLOCK_PHASE_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VAPE_CLOCK_SEL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(SMPS_CLOCK_PHASE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_clock_sel
 *
 * RET  : Return the value of register SmpsClockPhase
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
    value = (SPIRead(SMPS_CLOCK_PHASE_REG) & VAPE_CLOCK_SEL_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase
 * OUT  : 
 *
 * RET  : Return the value of register SmpsClockPhase
 *
 * Notes : From register 0x207, bits 2:0
 *
 **************************************************************************/
unsigned char set_varm_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SMPS_CLOCK_PHASE_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VARM_CLOCK_SEL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(SMPS_CLOCK_PHASE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_clock_sel
 *
 * RET  : Return the value of register SmpsClockPhase
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
    value = (SPIRead(SMPS_CLOCK_PHASE_REG) & VARM_CLOCK_SEL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase
 * OUT  : 
 *
 * RET  : Return the value of register SmpsClockPhase
 *
 * Notes : From register 0x208, bits 5:3
 *
 **************************************************************************/
unsigned char set_vsmps1_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SMPS_CLOCK_PHASE_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VSMPS_1_CLOCK_SEL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(SMPS_CLOCK_PHASE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_clock_sel
 *
 * RET  : Return the value of register SmpsClockPhase
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
    value = (SPIRead(SMPS_CLOCK_PHASE_REG) & VSMPS_1_CLOCK_SEL_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmod_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase
 * OUT  : 
 *
 * RET  : Return the value of register SmpsClockPhase
 *
 * Notes : From register 0x208, bits 2:0
 *
 **************************************************************************/
unsigned char set_vsmod_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SMPS_CLOCK_PHASE_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VSMOD_CLOCK_SEL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(SMPS_CLOCK_PHASE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmod_clock_sel
 *
 * RET  : Return the value of register SmpsClockPhase
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
    value = (SPIRead(SMPS_CLOCK_PHASE_REG) & VSMOD_CLOCK_SEL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase
 * OUT  : 
 *
 * RET  : Return the value of register SmpsClockPhase
 *
 * Notes : From register 0x209, bits 5:3
 *
 **************************************************************************/
unsigned char set_vsmps2_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SMPS_CLOCK_PHASE_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VSMPS_2_CLOCK_SEL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(SMPS_CLOCK_PHASE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_clock_sel
 *
 * RET  : Return the value of register SmpsClockPhase
 *
 * Notes : From register 0x209, bits 5:3
 *
 **************************************************************************/
unsigned char get_vsmps2_clock_sel()
  {
    unsigned char value;


    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
    value = (SPIRead(SMPS_CLOCK_PHASE_REG) & VSMPS_2_CLOCK_SEL_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_clock_sel
 *
 * IN   : param, a value to write to the regiter SmpsClockPhase
 * OUT  : 
 *
 * RET  : Return the value of register SmpsClockPhase
 *
 * Notes : From register 0x209, bits 2:0
 *
 **************************************************************************/
unsigned char set_vsmps3_clock_sel( enum smps_ckock_phase param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SMPS_CLOCK_PHASE_REG);

    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
  

     value =  old_value & ~VSMPS_3_CLOCK_SEL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(SMPS_CLOCK_PHASE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_clock_sel
 *
 * RET  : Return the value of register SmpsClockPhase
 *
 * Notes : From register 0x209, bits 2:0
 *
 **************************************************************************/
unsigned char get_vsmps3_clock_sel()
  {
    unsigned char value;


    /* 
     * Smps clock phase management versus IntClk clock 
     */ 
    value = (SPIRead(SMPS_CLOCK_PHASE_REG) & VSMPS_3_CLOCK_SEL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_clock27_ouput_buffer_d_p
 *
 * IN   : param, a value to write to the regiter SystemUlpClockConf
 * OUT  : 
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 7:7
 *
 **************************************************************************/
unsigned char set_clock27_ouput_buffer_d_p( enum ouput_buffer_p_d_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_ULP_CLOCK_CONF_REG);

    /* 
     * enable output buffer pulldown 
     */ 
    switch( param )
      {
        case OUPUT_BUFFER_PD_DISABLE_E: 
           value = old_value | OUPUT_BUFFER_PD_ENABLE_PARAM_MASK; 
           break;
        case OUPUT_BUFFER_PD_ENABLE_E: 
           value = old_value & ~ OUPUT_BUFFER_PD_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_ULP_CLOCK_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_clock27_ouput_buffer_d_p
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 7:7
 *
 **************************************************************************/
unsigned char get_clock27_ouput_buffer_d_p()
  {
    unsigned char value;


    /* 
     * enable output buffer pulldown 
     */ 
    value = (SPIRead(SYSTEM_ULP_CLOCK_CONF_REG) & OUPUT_BUFFER_PD_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_clock27_ouput_buffer
 *
 * IN   : param, a value to write to the regiter SystemUlpClockConf
 * OUT  : 
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 6:6
 *
 **************************************************************************/
unsigned char set_clock27_ouput_buffer( enum ouput_buffer_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_ULP_CLOCK_CONF_REG);

    /* 
     * enable output buffer 
     */ 
    switch( param )
      {
        case OUPUT_BUFFER_DISABLE_E: 
           value = old_value | OUPUT_BUFFER_ENABLE_PARAM_MASK; 
           break;
        case OUPUT_BUFFER_ENABLE_E: 
           value = old_value & ~ OUPUT_BUFFER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_ULP_CLOCK_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_clock27_ouput_buffer
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 6:6
 *
 **************************************************************************/
unsigned char get_clock27_ouput_buffer()
  {
    unsigned char value;


    /* 
     * enable output buffer 
     */ 
    value = (SPIRead(SYSTEM_ULP_CLOCK_CONF_REG) & OUPUT_BUFFER_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ulta_low_power_clock_strength
 *
 * IN   : param, a value to write to the regiter SystemUlpClockConf
 * OUT  : 
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 5:5
 *
 **************************************************************************/
unsigned char set_ulta_low_power_clock_strength( enum strength param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_ULP_CLOCK_CONF_REG);

    /* 
     * High strength 
     */ 
    switch( param )
      {
        case HIGH_STRENGTH_E: 
           value = old_value | ULTA_LOW_POWER_CLOCK_STRENGTH_PARAM_MASK; 
           break;
        case LOW_STRENGTH_E: 
           value = old_value & ~ ULTA_LOW_POWER_CLOCK_STRENGTH_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_ULP_CLOCK_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ulta_low_power_clock_strength
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 5:5
 *
 **************************************************************************/
unsigned char get_ulta_low_power_clock_strength()
  {
    unsigned char value;


    /* 
     * High strength 
     */ 
    value = (SPIRead(SYSTEM_ULP_CLOCK_CONF_REG) & ULTA_LOW_POWER_CLOCK_STRENGTH_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_tvout_clock_inverted
 *
 * IN   : param, a value to write to the regiter SystemUlpClockConf
 * OUT  : 
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 4:4
 *
 **************************************************************************/
unsigned char set_tvout_clock_inverted( enum tvout_clock_inverted param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_ULP_CLOCK_CONF_REG);

    /* 
     * internal TVout clock is inverted in order to add internal 
     * delay 
     */ 
    switch( param )
      {
        case NON_INVERTED_E: 
           value = old_value | TVOUT_CLOCK_INVERTED_PARAM_MASK; 
           break;
        case CLOCK_INVERTED_E: 
           value = old_value & ~ TVOUT_CLOCK_INVERTED_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_ULP_CLOCK_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_tvout_clock_inverted
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 4:4
 *
 **************************************************************************/
unsigned char get_tvout_clock_inverted()
  {
    unsigned char value;


    /* 
     * internal TVout clock is inverted in order to add internal 
     * delay 
     */ 
    value = (SPIRead(SYSTEM_ULP_CLOCK_CONF_REG) & TVOUT_CLOCK_INVERTED_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_tvout_clock_input
 *
 * IN   : param, a value to write to the regiter SystemUlpClockConf
 * OUT  : 
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 3:3
 *
 **************************************************************************/
unsigned char set_tvout_clock_input( enum tvout_clock_input_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_ULP_CLOCK_CONF_REG);

    /* 
     * internal input buffer is enabled in order to add 
     * internally delay to TVout clock 
     */ 
    switch( param )
      {
        case TVOUT_CLOCK_INPUT_ENABLE_E: 
           value = old_value | TVOUT_CLOCK_INPUT_ENABLE_PARAM_MASK; 
           break;
        case TVOUT_CLOCK_INPUT_DISABLE_E: 
           value = old_value & ~ TVOUT_CLOCK_INPUT_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_ULP_CLOCK_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_tvout_clock_input
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 3:3
 *
 **************************************************************************/
unsigned char get_tvout_clock_input()
  {
    unsigned char value;


    /* 
     * internal input buffer is enabled in order to add 
     * internally delay to TVout clock 
     */ 
    value = (SPIRead(SYSTEM_ULP_CLOCK_CONF_REG) & TVOUT_CLOCK_INPUT_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_clock27_strength
 *
 * IN   : param, a value to write to the regiter SystemUlpClockConf
 * OUT  : 
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 2:2
 *
 **************************************************************************/
unsigned char set_clock27_strength( enum strength param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_ULP_CLOCK_CONF_REG);

    /* 
     * High strength 
     */ 
    switch( param )
      {
        case HIGH_STRENGTH_E: 
           value = old_value | CLOCK_27_STRENGTH_PARAM_MASK; 
           break;
        case LOW_STRENGTH_E: 
           value = old_value & ~ CLOCK_27_STRENGTH_PARAM_MASK;
           break;
      }
  

    SPIWrite(SYSTEM_ULP_CLOCK_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_clock27_strength
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 2:2
 *
 **************************************************************************/
unsigned char get_clock27_strength()
  {
    unsigned char value;


    /* 
     * High strength 
     */ 
    value = (SPIRead(SYSTEM_ULP_CLOCK_CONF_REG) & CLOCK_27_STRENGTH_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ulta_low_power_pad
 *
 * IN   : param, a value to write to the regiter SystemUlpClockConf
 * OUT  : 
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 1:0
 *
 **************************************************************************/
unsigned char set_ulta_low_power_pad( enum ulta_low_power_padonnf param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SYSTEM_ULP_CLOCK_CONF_REG);

    /* 
     * UlpClk pad configurations. stw4500 centric 
     */ 
  

     value =  old_value & ~ULTA_LOW_POWER_PADONNF_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(SYSTEM_ULP_CLOCK_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ulta_low_power_pad
 *
 * RET  : Return the value of register SystemUlpClockConf
 *
 * Notes : From register 0x20A, bits 1:0
 *
 **************************************************************************/
unsigned char get_ulta_low_power_pad()
  {
    unsigned char value;


    /* 
     * UlpClk pad configurations. stw4500 centric 
     */ 
    value = (SPIRead(SYSTEM_ULP_CLOCK_CONF_REG) & ULTA_LOW_POWER_PADONNF_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_buf4_req
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 7:7
 *
 **************************************************************************/
unsigned char set_sys_clk_buf4_req( enum activate_sys_clock_buffer4 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_MANAGEMENT_REG);

    /* 
     * Activate click buffer 4 of U8500 RF device 
     */ 
    switch( param )
      {
        case INACTIVE_SYS_CLOCK_BUFFER_4_E: 
           value = old_value | ACTIVATE_SYS_CLOCK_BUFFER_4_PARAM_MASK; 
           break;
        case ACTIVATE_SYS_CLOCK_BUFFER_4_E: 
           value = old_value & ~ ACTIVATE_SYS_CLOCK_BUFFER_4_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_buf4_req
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 7:7
 *
 **************************************************************************/
unsigned char get_sys_clk_buf4_req()
  {
    unsigned char value;


    /* 
     * Activate click buffer 4 of U8500 RF device 
     */ 
    value = (SPIRead(CLOCK_MANAGEMENT_REG) & ACTIVATE_SYS_CLOCK_BUFFER_4_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_buf3_req
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 6:6
 *
 **************************************************************************/
unsigned char set_sys_clk_buf3_req( enum activate_sys_clock_buffer3 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_MANAGEMENT_REG);

    /* 
     * Activate click buffer 3 of U8500 RF device 
     */ 
    switch( param )
      {
        case INACTIVE_SYS_CLOCK_BUFFER_3_E: 
           value = old_value | ACTIVATE_SYS_CLOCK_BUFFER_3_PARAM_MASK; 
           break;
        case ACTIVATE_SYS_CLOCK_BUFFER_3_E: 
           value = old_value & ~ ACTIVATE_SYS_CLOCK_BUFFER_3_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_buf3_req
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 6:6
 *
 **************************************************************************/
unsigned char get_sys_clk_buf3_req()
  {
    unsigned char value;


    /* 
     * Activate click buffer 3 of U8500 RF device 
     */ 
    value = (SPIRead(CLOCK_MANAGEMENT_REG) & ACTIVATE_SYS_CLOCK_BUFFER_3_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_buf2_req
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 5:5
 *
 **************************************************************************/
unsigned char set_sys_clk_buf2_req( enum activate_sys_clock_buffer2 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_MANAGEMENT_REG);

    /* 
     * Activate click buffer 2 of U8500 RF device 
     */ 
    switch( param )
      {
        case INACTIVE_SYS_CLOCK_BUFFER_2_E: 
           value = old_value | ACTIVATE_SYS_CLOCK_BUFFER_2_PARAM_MASK; 
           break;
        case ACTIVATE_SYS_CLOCK_BUFFER_2_E: 
           value = old_value & ~ ACTIVATE_SYS_CLOCK_BUFFER_2_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_buf2_req
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 5:5
 *
 **************************************************************************/
unsigned char get_sys_clk_buf2_req()
  {
    unsigned char value;


    /* 
     * Activate click buffer 2 of U8500 RF device 
     */ 
    value = (SPIRead(CLOCK_MANAGEMENT_REG) & ACTIVATE_SYS_CLOCK_BUFFER_2_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_clock
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 4:4
 *
 **************************************************************************/
unsigned char set_audio_clock( enum audio_clock_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_MANAGEMENT_REG);

    /* 
     * enable audio clock to audio block 
     */ 
    switch( param )
      {
        case AUDIO_CLOCK_DISABLE_E: 
           value = old_value | AUDIO_CLOCK_ENABLE_PARAM_MASK; 
           break;
        case AUDIO_CLOCK_ENABLE_E: 
           value = old_value & ~ AUDIO_CLOCK_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_clock
 *
 * RET  : Return the value of register ClockManagement
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
    value = (SPIRead(CLOCK_MANAGEMENT_REG) & AUDIO_CLOCK_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_system_clock_request
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 3:3
 *
 **************************************************************************/
unsigned char set_system_clock_request( enum system_clock_request_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_MANAGEMENT_REG);

    /* 
     * STw4500 request system clock 
     */ 
    switch( param )
      {
        case SYSTEM_CLOCK_REQUEST_DISABLE_E: 
           value = old_value | SYSTEM_CLOCK_REQUEST_ENABLE_PARAM_MASK; 
           break;
        case SYSTEM_CLOCK_REQUEST_ENABLE_E: 
           value = old_value & ~ SYSTEM_CLOCK_REQUEST_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_system_clock_request
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 3:3
 *
 **************************************************************************/
unsigned char get_system_clock_request()
  {
    unsigned char value;


    /* 
     * STw4500 request system clock 
     */ 
    value = (SPIRead(CLOCK_MANAGEMENT_REG) & SYSTEM_CLOCK_REQUEST_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ulp_request
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 2:2
 *
 **************************************************************************/
unsigned char set_ulp_request( enum ulp_request_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_MANAGEMENT_REG);

    /* 
     * STw4500 request ulp clock 
     */ 
    switch( param )
      {
        case ULP_REQUEST_DISABLE_E: 
           value = old_value | ULP_REQUEST_ENABLE_PARAM_MASK; 
           break;
        case ULP_REQUEST_ENABLE_E: 
           value = old_value & ~ ULP_REQUEST_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ulp_request
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 2:2
 *
 **************************************************************************/
unsigned char get_ulp_request()
  {
    unsigned char value;


    /* 
     * STw4500 request ulp clock 
     */ 
    value = (SPIRead(CLOCK_MANAGEMENT_REG) & ULP_REQUEST_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_ultra_low_power_selection
 *
 * IN   : param, a value to write to the regiter ClockManagement
 * OUT  : 
 *
 * RET  : Return the value of register ClockManagement
 *
 * Notes : From register 0x20B, bits 1:0
 *
 **************************************************************************/
unsigned char set_sys_ultra_low_power_selection( enum sys_ultra_low_power_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_MANAGEMENT_REG);

    /* 
     * internal clock switching 
     */ 
  

     value =  old_value & ~SYS_ULTRA_LOW_POWER_SELECTION_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(CLOCK_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_ultra_low_power_selection
 *
 * RET  : Return the value of register ClockManagement
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
    value = (SPIRead(CLOCK_MANAGEMENT_REG) & SYS_ULTRA_LOW_POWER_SELECTION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_clock_enable
 *
 * IN   : param, a value to write to the regiter ClockValid
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid
 *
 * Notes : From register 0x20C, bits 2:2
 *
 **************************************************************************/
unsigned char set_usb_clock_enable( enum usb_clock_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_REG);

    /* 
     * 0: disable SysClk path to USB block 
     * 1: enable SysClk path to USB block 
     */ 
    switch( param )
      {
        case USB_CLOCK_DISABLE_E: 
           value = old_value | USB_CLOCK_ENABLE_PARAM_MASK; 
           break;
        case USB_CLOCK_ENABLE_E: 
           value = old_value & ~ USB_CLOCK_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_clock_enable
 *
 * RET  : Return the value of register ClockValid
 *
 * Notes : From register 0x20C, bits 2:2
 *
 **************************************************************************/
unsigned char get_usb_clock_enable()
  {
    unsigned char value;


    /* 
     * 0: disable SysClk path to USB block 
     * 1: enable SysClk path to USB block 
     */ 
    value = (SPIRead(CLOCK_VALID_REG) & USB_CLOCK_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_t_vout_clock_enable
 *
 * IN   : param, a value to write to the regiter ClockValid
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid
 *
 * Notes : From register 0x20C, bits 1:1
 *
 **************************************************************************/
unsigned char set_t_vout_clock_enable( enum t_vout_clk_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_REG);

    /* 
     * 0: disable 27Mhz clock to TVout block 
     * 1: enable 27Mhz clock to TVout block 
     */ 
    switch( param )
      {
        case T_VOUT_CLK_DISABLE_E: 
           value = old_value | T_VOUT_CLK_ENABLE_PARAM_MASK; 
           break;
        case T_VOUT_CLK_ENABLE_E: 
           value = old_value & ~ T_VOUT_CLK_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_t_vout_clock_enable
 *
 * RET  : Return the value of register ClockValid
 *
 * Notes : From register 0x20C, bits 1:1
 *
 **************************************************************************/
unsigned char get_t_vout_clock_enable()
  {
    unsigned char value;


    /* 
     * 0: disable 27Mhz clock to TVout block 
     * 1: enable 27Mhz clock to TVout block 
     */ 
    value = (SPIRead(CLOCK_VALID_REG) & T_VOUT_CLK_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_tv_out_pll_enable
 *
 * IN   : param, a value to write to the regiter ClockValid
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid
 *
 * Notes : From register 0x20C, bits 0:0
 *
 **************************************************************************/
unsigned char set_tv_out_pll_enable( enum tv_out_pll_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_REG);

    /* 
     * enable TVout pll (generate 27Mhz from SysClk) 
     */ 
    switch( param )
      {
        case TV_OUT_PLL_DISABLE_E: 
           value = old_value | TV_OUT_PLL_ENABLE_PARAM_MASK; 
           break;
        case TV_OUT_PLL_ENABLE_E: 
           value = old_value & ~ TV_OUT_PLL_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_tv_out_pll_enable
 *
 * RET  : Return the value of register ClockValid
 *
 * Notes : From register 0x20C, bits 0:0
 *
 **************************************************************************/
unsigned char get_tv_out_pll_enable()
  {
    unsigned char value;


    /* 
     * enable TVout pll (generate 27Mhz from SysClk) 
     */ 
    value = (SPIRead(CLOCK_VALID_REG) & TV_OUT_PLL_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ultra_low_power_request1_valid
 *
 * IN   : param, a value to write to the regiter ClockValid2
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid2
 *
 * Notes : From register 0x20D, bits 1:1
 *
 **************************************************************************/
unsigned char set_ultra_low_power_request1_valid( enum ultra_low_power_request1_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_2_REG);

    /* 
     * valid ulp clock request from SysClkReq1 input 
     */ 
    switch( param )
      {
        case ULTRA_LOW_POWER_DOWN_E: 
           value = old_value | ULTRA_LOW_POWER_REQUEST_1_VALID_PARAM_MASK; 
           break;
        case ULTRA_LOW_POWER_UP_E: 
           value = old_value & ~ ULTRA_LOW_POWER_REQUEST_1_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ultra_low_power_request1_valid
 *
 * RET  : Return the value of register ClockValid2
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
    value = (SPIRead(CLOCK_VALID_2_REG) & ULTRA_LOW_POWER_REQUEST_1_VALID_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_request1_valid
 *
 * IN   : param, a value to write to the regiter ClockValid2
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid2
 *
 * Notes : From register 0x20D, bits 0:0
 *
 **************************************************************************/
unsigned char set_sys_clock_request1_valid( enum sys_clock_request1_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_2_REG);

    /* 
     * Sys valid system clock request from SysClkReq1input 
     */ 
    switch( param )
      {
        case APE_DONT_VALID_ULP_RESQUEST_E: 
           value = old_value | SYS_CLOCK_REQUEST_1_VALID_PARAM_MASK; 
           break;
        case APE_VALID_ULP_RESQUEST_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_1_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_request1_valid
 *
 * RET  : Return the value of register ClockValid2
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
    value = (SPIRead(CLOCK_VALID_2_REG) & SYS_CLOCK_REQUEST_1_VALID_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt8500_open_drain
 *
 * IN   : param, a value to write to the regiter ControlBall
 * OUT  : 
 *
 * RET  : Return the value of register ControlBall
 *
 * Notes : From register 0x20F, bits 4:4
 *
 **************************************************************************/
unsigned char set_interrupt8500_open_drain( enum ball_configuration param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_BALL_REG);

    /* 
     * 0: Int8500n is a push/pull output 
     * 1: Int8500n is an OpenDrain output 
     */ 
    switch( param )
      {
        case PUSH_PULL_E: 
           value = old_value | BALL_CONFIGURATION_PARAM_MASK; 
           break;
        case OPEN_DRAIN_E: 
           value = old_value & ~ BALL_CONFIGURATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(CONTROL_BALL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt8500_open_drain
 *
 * RET  : Return the value of register ControlBall
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
    value = (SPIRead(CONTROL_BALL_REG) & BALL_CONFIGURATION_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ext_sup3_l_pn_clk_sel
 *
 * IN   : param, a value to write to the regiter ControlBall
 * OUT  : 
 *
 * RET  : Return the value of register ControlBall
 *
 * Notes : From register 0x20F, bits 3:2
 *
 **************************************************************************/
unsigned char set_ext_sup3_l_pn_clk_sel( enum configuration3 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_BALL_REG);

    /* 
     * 00: select ExtSup3LPn function = ExtSup3LP OR ExtSup3LP 
     * 01: select ExtSupply3Clk function. ExtSupply3Clk can be 
     * internal oscillator, Sysclk or UlpClk. 
     * 10 or 11: select (ExtSupply3LPn AND ExtSupply3Clk) 
     */ 
  

     value =  old_value & ~CONFIGURATION_3_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(CONTROL_BALL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ext_sup3_l_pn_clk_sel
 *
 * RET  : Return the value of register ControlBall
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
    value = (SPIRead(CONTROL_BALL_REG) & CONFIGURATION_3_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ext_sup12_l_pn_clk_sel
 *
 * IN   : param, a value to write to the regiter ControlBall
 * OUT  : 
 *
 * RET  : Return the value of register ControlBall
 *
 * Notes : From register 0x20F, bits 1:0
 *
 **************************************************************************/
unsigned char set_ext_sup12_l_pn_clk_sel( enum configuration12 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CONTROL_BALL_REG);

    /* 
     * 00: select ExtSup12LPn function = ExtSup12LP OR 
     * ExtSup12LP 
     * 01: select ExtSupply12Clk function. ExtSupply12Clk can be 
     * internal oscillator, Sysclk or UlpClk. 
     * 10 or 11: select (ExtSupply12LPn AND ExtSupply12Clk) 
     */ 
  

     value =  old_value & ~CONFIGURATION_12_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(CONTROL_BALL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ext_sup12_l_pn_clk_sel
 *
 * RET  : Return the value of register ControlBall
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
    value = (SPIRead(CONTROL_BALL_REG) & CONFIGURATION_12_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req1
 *
 * IN   : param, a value to write to the regiter ClockValid3
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid3
 *
 * Notes : From register 0x210, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req1( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_3_REG);

    /* 
     * 0: SysClkReq1 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq1 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req1
 *
 * RET  : Return the value of register ClockValid3
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
    value = (SPIRead(CLOCK_VALID_3_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req1
 *
 * IN   : param, a value to write to the regiter ClockValid3
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid3
 *
 * Notes : From register 0x210, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req1( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_3_REG);

    /* 
     * 0: SysClkReq1 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq1 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req1
 *
 * RET  : Return the value of register ClockValid3
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
    value = (SPIRead(CLOCK_VALID_3_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req1
 *
 * IN   : param, a value to write to the regiter ClockValid3
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid3
 *
 * Notes : From register 0x210, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req1( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_3_REG);

    /* 
     * 0: SysClkReq1 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq1 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req1
 *
 * RET  : Return the value of register ClockValid3
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
    value = (SPIRead(CLOCK_VALID_3_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req2
 *
 * IN   : param, a value to write to the regiter ClockValid4
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid4
 *
 * Notes : From register 0x211, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req2( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_4_REG);

    /* 
     * 0: SysClkReq2 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq2 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req2
 *
 * RET  : Return the value of register ClockValid4
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
    value = (SPIRead(CLOCK_VALID_4_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req2
 *
 * IN   : param, a value to write to the regiter ClockValid4
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid4
 *
 * Notes : From register 0x211, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req2( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_4_REG);

    /* 
     * 0: SysClkReq2 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq2 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req2
 *
 * RET  : Return the value of register ClockValid4
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
    value = (SPIRead(CLOCK_VALID_4_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req2
 *
 * IN   : param, a value to write to the regiter ClockValid4
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid4
 *
 * Notes : From register 0x211, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req2( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_4_REG);

    /* 
     * 0: SysClkReq2 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq2 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req2
 *
 * RET  : Return the value of register ClockValid4
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
    value = (SPIRead(CLOCK_VALID_4_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req3
 *
 * IN   : param, a value to write to the regiter ClockValid5
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid5
 *
 * Notes : From register 0x212, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req3( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_5_REG);

    /* 
     * 0: SysClkReq3 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq3 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req3
 *
 * RET  : Return the value of register ClockValid5
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
    value = (SPIRead(CLOCK_VALID_5_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req3
 *
 * IN   : param, a value to write to the regiter ClockValid5
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid5
 *
 * Notes : From register 0x212, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req3( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_5_REG);

    /* 
     * 0: SysClkReq3 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq3 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req3
 *
 * RET  : Return the value of register ClockValid5
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
    value = (SPIRead(CLOCK_VALID_5_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req3
 *
 * IN   : param, a value to write to the regiter ClockValid5
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid5
 *
 * Notes : From register 0x212, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req3( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_5_REG);

    /* 
     * 0: SysClkReq3 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq3 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req3
 *
 * RET  : Return the value of register ClockValid5
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
    value = (SPIRead(CLOCK_VALID_5_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req4
 *
 * IN   : param, a value to write to the regiter ClockValid6
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid6
 *
 * Notes : From register 0x213, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req4( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_6_REG);

    /* 
     * 0: SysClkReq4 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq4 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req4
 *
 * RET  : Return the value of register ClockValid6
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
    value = (SPIRead(CLOCK_VALID_6_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req4
 *
 * IN   : param, a value to write to the regiter ClockValid6
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid6
 *
 * Notes : From register 0x213, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req4( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_6_REG);

    /* 
     * 0: SysClkReq4 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq4 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req4
 *
 * RET  : Return the value of register ClockValid6
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
    value = (SPIRead(CLOCK_VALID_6_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req4
 *
 * IN   : param, a value to write to the regiter ClockValid6
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid6
 *
 * Notes : From register 0x213, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req4( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_6_REG);

    /* 
     * 0: SysClkReq4 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq4 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req4
 *
 * RET  : Return the value of register ClockValid6
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
    value = (SPIRead(CLOCK_VALID_6_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req5
 *
 * IN   : param, a value to write to the regiter ClockValid7
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid7
 *
 * Notes : From register 0x214, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req5( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_7_REG);

    /* 
     * 0: SysClkReq5 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq5 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req5
 *
 * RET  : Return the value of register ClockValid7
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
    value = (SPIRead(CLOCK_VALID_7_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req5
 *
 * IN   : param, a value to write to the regiter ClockValid7
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid7
 *
 * Notes : From register 0x214, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req5( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_7_REG);

    /* 
     * 0: SysClkReq5 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq5 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req5
 *
 * RET  : Return the value of register ClockValid7
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
    value = (SPIRead(CLOCK_VALID_7_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req5
 *
 * IN   : param, a value to write to the regiter ClockValid7
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid7
 *
 * Notes : From register 0x214, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req5( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_7_REG);

    /* 
     * 0: SysClkReq5 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq5 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req5
 *
 * RET  : Return the value of register ClockValid7
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
    value = (SPIRead(CLOCK_VALID_7_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req6
 *
 * IN   : param, a value to write to the regiter ClockValid8
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid8
 *
 * Notes : From register 0x215, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req6( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_8_REG);

    /* 
     * 0: SysClkReq6 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq6 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req6
 *
 * RET  : Return the value of register ClockValid8
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
    value = (SPIRead(CLOCK_VALID_8_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req6
 *
 * IN   : param, a value to write to the regiter ClockValid8
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid8
 *
 * Notes : From register 0x215, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req6( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_8_REG);

    /* 
     * 0: SysClkReq6 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq6 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req6
 *
 * RET  : Return the value of register ClockValid8
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
    value = (SPIRead(CLOCK_VALID_8_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req6
 *
 * IN   : param, a value to write to the regiter ClockValid8
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid8
 *
 * Notes : From register 0x215, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req6( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_8_REG);

    /* 
     * 0: SysClkReq6 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq6 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req6
 *
 * RET  : Return the value of register ClockValid8
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
    value = (SPIRead(CLOCK_VALID_8_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req7
 *
 * IN   : param, a value to write to the regiter ClockValid9
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid9
 *
 * Notes : From register 0x216, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req7( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_9_REG);

    /* 
     * 0: SysClkReq7 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq7 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req7
 *
 * RET  : Return the value of register ClockValid9
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
    value = (SPIRead(CLOCK_VALID_9_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req7
 *
 * IN   : param, a value to write to the regiter ClockValid9
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid9
 *
 * Notes : From register 0x216, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req7( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_9_REG);

    /* 
     * 0: SysClkReq7 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq7 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req7
 *
 * RET  : Return the value of register ClockValid9
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
    value = (SPIRead(CLOCK_VALID_9_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req7
 *
 * IN   : param, a value to write to the regiter ClockValid9
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid9
 *
 * Notes : From register 0x216, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req7( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_9_REG);

    /* 
     * 0: SysClkReq7 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq7 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req7
 *
 * RET  : Return the value of register ClockValid9
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
    value = (SPIRead(CLOCK_VALID_9_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf4_req8
 *
 * IN   : param, a value to write to the regiter ClockValid10
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid10
 *
 * Notes : From register 0x217, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clock_buf4_req8( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_10_REG);

    /* 
     * 0: SysClkReq8 is not associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq8 is associated to clock buffer 4 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf4_req8
 *
 * RET  : Return the value of register ClockValid10
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
    value = (SPIRead(CLOCK_VALID_10_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf3_req8
 *
 * IN   : param, a value to write to the regiter ClockValid10
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid10
 *
 * Notes : From register 0x217, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clock_buf3_req8( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_10_REG);

    /* 
     * 0: SysClkReq8 is not associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq8 is associated to clock buffer 3 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf3_req8
 *
 * RET  : Return the value of register ClockValid10
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
    value = (SPIRead(CLOCK_VALID_10_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clock_buf2_req8
 *
 * IN   : param, a value to write to the regiter ClockValid10
 * OUT  : 
 *
 * RET  : Return the value of register ClockValid10
 *
 * Notes : From register 0x217, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clock_buf2_req8( enum sys_clock_resquest param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(CLOCK_VALID_10_REG);

    /* 
     * 0: SysClkReq8 is not associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     * 1: SysClkReq8 is associated to clock buffer 2 of 
     * Levi/Yllas RF I.C. 
     */ 
    switch( param )
      {
        case SYS_CLOCK_RESQUET_E: 
           value = old_value | SYS_CLOCK_RESQUEST_PARAM_MASK; 
           break;
        case SYS_CLOCK_NOT_VALID_E: 
           value = old_value & ~ SYS_CLOCK_RESQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(CLOCK_VALID_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clock_buf2_req8
 *
 * RET  : Return the value of register ClockValid10
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
    value = (SPIRead(CLOCK_VALID_10_REG) & SYS_CLOCK_RESQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dithering_delay
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 7:6
 *
 **************************************************************************/
unsigned char set_dithering_delay( enum dithering_delay param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(DITHER_CLK_CTRL_REG);

    /* 
     * Dithering delay 2.5ns, 3ns 3.5ns 4ns 
     */ 
  

     value =  old_value & ~DITHERING_DELAY_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dithering_delay
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 7:6
 *
 **************************************************************************/
unsigned char get_dithering_delay()
  {
    unsigned char value;


    /* 
     * Dithering delay 2.5ns, 3ns 3.5ns 4ns 
     */ 
    value = (SPIRead(DITHER_CLK_CTRL_REG) & DITHERING_DELAY_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 5:5
 *
 **************************************************************************/
unsigned char set_vape_dither( enum vape_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(DITHER_CLK_CTRL_REG);

    /* 
     * VapeDitherEna 
     */ 
    switch( param )
      {
        case VAPE_DITHER_DISABLE_E: 
           value = old_value | VAPE_DITHER_ENABLE_PARAM_MASK; 
           break;
        case VAPE_DITHER_ENABLE_E: 
           value = old_value & ~ VAPE_DITHER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_dither
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 5:5
 *
 **************************************************************************/
unsigned char get_vape_dither()
  {
    unsigned char value;


    /* 
     * VapeDitherEna 
     */ 
    value = (SPIRead(DITHER_CLK_CTRL_REG) & VAPE_DITHER_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 4:4
 *
 **************************************************************************/
unsigned char set_vmod_dither( enum vmod_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(DITHER_CLK_CTRL_REG);

    /* 
     * VmodDitherEna 
     */ 
    switch( param )
      {
        case VMOD_DITHER_DISABLE_E: 
           value = old_value | VMOD_DITHER_ENABLE_PARAM_MASK; 
           break;
        case VMOD_DITHER_ENABLE_E: 
           value = old_value & ~ VMOD_DITHER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_dither
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 4:4
 *
 **************************************************************************/
unsigned char get_vmod_dither()
  {
    unsigned char value;


    /* 
     * VmodDitherEna 
     */ 
    value = (SPIRead(DITHER_CLK_CTRL_REG) & VMOD_DITHER_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 3:3
 *
 **************************************************************************/
unsigned char set_vsmps2_dither( enum vsmps2_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(DITHER_CLK_CTRL_REG);

    /* 
     * Vsmps2DitherEna 
     */ 
    switch( param )
      {
        case VSMPS_2_DITHER_DISABLE_E: 
           value = old_value | VSMPS_2_DITHER_ENABLE_PARAM_MASK; 
           break;
        case VSMPS_2_DITHER_ENABLE_E: 
           value = old_value & ~ VSMPS_2_DITHER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_dither
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 3:3
 *
 **************************************************************************/
unsigned char get_vsmps2_dither()
  {
    unsigned char value;


    /* 
     * Vsmps2DitherEna 
     */ 
    value = (SPIRead(DITHER_CLK_CTRL_REG) & VSMPS_2_DITHER_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps1_dither( enum vsmps1_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(DITHER_CLK_CTRL_REG);

    /* 
     * Vsmps1DitherEna 
     */ 
    switch( param )
      {
        case VSMPS_1_DITHER_DISABLE_E: 
           value = old_value | VSMPS_1_DITHER_ENABLE_PARAM_MASK; 
           break;
        case VSMPS_1_DITHER_ENABLE_E: 
           value = old_value & ~ VSMPS_1_DITHER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_dither
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 2:2
 *
 **************************************************************************/
unsigned char get_vsmps1_dither()
  {
    unsigned char value;


    /* 
     * Vsmps1DitherEna 
     */ 
    value = (SPIRead(DITHER_CLK_CTRL_REG) & VSMPS_1_DITHER_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsmps3_dither( enum vsmps3_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(DITHER_CLK_CTRL_REG);

    /* 
     * Vsmps3DitherEna 
     */ 
    switch( param )
      {
        case VSMPS_3_DITHER_DISABLE_E: 
           value = old_value | VSMPS_3_DITHER_ENABLE_PARAM_MASK; 
           break;
        case VSMPS_3_DITHER_ENABLE_E: 
           value = old_value & ~ VSMPS_3_DITHER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_dither
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 1:1
 *
 **************************************************************************/
unsigned char get_vsmps3_dither()
  {
    unsigned char value;


    /* 
     * Vsmps3DitherEna 
     */ 
    value = (SPIRead(DITHER_CLK_CTRL_REG) & VSMPS_3_DITHER_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_dither
 *
 * IN   : param, a value to write to the regiter DitherClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 0:0
 *
 **************************************************************************/
unsigned char set_varm_dither( enum varm_dither_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(DITHER_CLK_CTRL_REG);

    /* 
     * VarmDitherEna 
     */ 
    switch( param )
      {
        case VARM_DITHER_DISABLE_E: 
           value = old_value | VARM_DITHER_ENABLE_PARAM_MASK; 
           break;
        case VARM_DITHER_ENABLE_E: 
           value = old_value & ~ VARM_DITHER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(DITHER_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_dither
 *
 * RET  : Return the value of register DitherClkCtrl
 *
 * Notes : From register 0x0220, bits 0:0
 *
 **************************************************************************/
unsigned char get_varm_dither()
  {
    unsigned char value;


    /* 
     * VarmDitherEna 
     */ 
    value = (SPIRead(DITHER_CLK_CTRL_REG) & VARM_DITHER_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_swat_bit5_set
 *
 * IN   : param, a value to write to the regiter SwatCtrl
 * OUT  : 
 *
 * RET  : Return the value of register SwatCtrl
 *
 * Notes : From register 0x0230, bits 5:5
 *
 **************************************************************************/
unsigned char set_swat_bit5_set( enum swat_bit5_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SWAT_CTRL_REG);

    /* 
     * Set 'Swat[5]' (each updated value will be sent through 
     * Swat interface) 
     */ 
    switch( param )
      {
        case SWAT_BIT_5_RESET_SET_E: 
           value = old_value | SWAT_BIT_5_SET_PARAM_MASK; 
           break;
        case SWAT_BIT_5_SET_E: 
           value = old_value & ~ SWAT_BIT_5_SET_PARAM_MASK;
           break;
      }
  

    SPIWrite(SWAT_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_swat_bit5_set
 *
 * RET  : Return the value of register SwatCtrl
 *
 * Notes : From register 0x0230, bits 5:5
 *
 **************************************************************************/
unsigned char get_swat_bit5_set()
  {
    unsigned char value;


    /* 
     * Set 'Swat[5]' (each updated value will be sent through 
     * Swat interface) 
     */ 
    value = (SPIRead(SWAT_CTRL_REG) & SWAT_BIT_5_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rf_off_timer
 *
 * IN   : param, a value to write to the regiter SwatCtrl
 * OUT  : 
 *
 * RET  : Return the value of register SwatCtrl
 *
 * Notes : From register 0x0230, bits 4:2
 *
 **************************************************************************/
unsigned char set_rf_off_timer( enum off_timer param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SWAT_CTRL_REG);

    /* 
     * Defined timer of RF turn off sequence, Timer = 
     * (RfOffTimer[2:0] +1) x periods of 32kHz 
     */ 
  

     value =  old_value & ~OFF_TIMER_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(SWAT_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rf_off_timer
 *
 * RET  : Return the value of register SwatCtrl
 *
 * Notes : From register 0x0230, bits 4:2
 *
 **************************************************************************/
unsigned char get_rf_off_timer()
  {
    unsigned char value;


    /* 
     * Defined timer of RF turn off sequence, Timer = 
     * (RfOffTimer[2:0] +1) x periods of 32kHz 
     */ 
    value = (SPIRead(SWAT_CTRL_REG) & OFF_TIMER_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_swat_enable
 *
 * IN   : param, a value to write to the regiter SwatCtrl
 * OUT  : 
 *
 * RET  : Return the value of register SwatCtrl
 *
 * Notes : From register 0x0230, bits 1:1
 *
 **************************************************************************/
unsigned char set_swat_enable( enum swat_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SWAT_CTRL_REG);

    /* 
     * 0: Disable Swat block 
     * 1: Enable Swat block 
     */ 
    switch( param )
      {
        case SWAT_DISABLE_E: 
           value = old_value | SWAT_ENABLE_PARAM_MASK; 
           break;
        case SWAT_ENABLE_E: 
           value = old_value & ~ SWAT_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(SWAT_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_swat_enable
 *
 * RET  : Return the value of register SwatCtrl
 *
 * Notes : From register 0x0230, bits 1:1
 *
 **************************************************************************/
unsigned char get_swat_enable()
  {
    unsigned char value;


    /* 
     * 0: Disable Swat block 
     * 1: Enable Swat block 
     */ 
    value = (SPIRead(SWAT_CTRL_REG) & SWAT_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_levi_or_yllas
 *
 * IN   : param, a value to write to the regiter SwatCtrl
 * OUT  : 
 *
 * RET  : Return the value of register SwatCtrl
 *
 * Notes : From register 0x0230, bits 0:0
 *
 **************************************************************************/
unsigned char set_levi_or_yllas( enum levi_or_yllas param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SWAT_CTRL_REG);

    /* 
     * 0: RF I.C. is Levi (allow to configure Swat message) 
     * 1: RF I.C. is Yllas (allow to configure Swat message) 
     */ 
    switch( param )
      {
        case RF_IC_LEVI_E: 
           value = old_value | LEVI_OR_YLLAS_PARAM_MASK; 
           break;
        case RF_IC_YLLAS_E: 
           value = old_value & ~ LEVI_OR_YLLAS_PARAM_MASK;
           break;
      }
  

    SPIWrite(SWAT_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_levi_or_yllas
 *
 * RET  : Return the value of register SwatCtrl
 *
 * Notes : From register 0x0230, bits 0:0
 *
 **************************************************************************/
unsigned char get_levi_or_yllas()
  {
    unsigned char value;


    /* 
     * 0: RF I.C. is Levi (allow to configure Swat message) 
     * 1: RF I.C. is Yllas (allow to configure Swat message) 
     */ 
    value = (SPIRead(SWAT_CTRL_REG) & LEVI_OR_YLLAS_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req8_hiq_enable
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 7:7
 *
 **************************************************************************/
unsigned char set_sys_clk_req8_hiq_enable( enum sys_clk_req8_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq8 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq8 input to control HiqClkEna ball 
     */ 
    switch( param )
      {
        case SYS_CLK_REQ_8_HIQ_DISABLE_E: 
           value = old_value | SYS_CLK_REQ_8_HIQ_ENABLE_PARAM_MASK; 
           break;
        case SYS_CLK_REQ_8_HIQ_ENABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_8_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req8_hiq_enable
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 7:7
 *
 **************************************************************************/
unsigned char get_sys_clk_req8_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq8 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq8 input to control HiqClkEna ball 
     */ 
    value = (SPIRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_8_HIQ_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req7_hiq_enable
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 6:6
 *
 **************************************************************************/
unsigned char set_sys_clk_req7_hiq_enable( enum sys_clk_req7_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq7 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq7 input to control HiqClkEna ball 
     */ 
    switch( param )
      {
        case SYS_CLK_REQ_7_HIQ_DISABLE_E: 
           value = old_value | SYS_CLK_REQ_7_HIQ_ENABLE_PARAM_MASK; 
           break;
        case SYS_CLK_REQ_7_HIQ_ENABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_7_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req7_hiq_enable
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 6:6
 *
 **************************************************************************/
unsigned char get_sys_clk_req7_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq7 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq7 input to control HiqClkEna ball 
     */ 
    value = (SPIRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_7_HIQ_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req6_hiq_enable
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 5:5
 *
 **************************************************************************/
unsigned char set_sys_clk_req6_hiq_enable( enum sys_clk_req6_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq6 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq6 input to control HiqClkEna ball 
     */ 
    switch( param )
      {
        case SYS_CLK_REQ_6_HIQ_DISABLE_E: 
           value = old_value | SYS_CLK_REQ_6_HIQ_ENABLE_PARAM_MASK; 
           break;
        case SYS_CLK_REQ_6_HIQ_ENABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_6_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req6_hiq_enable
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 5:5
 *
 **************************************************************************/
unsigned char get_sys_clk_req6_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq6 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq6 input to control HiqClkEna ball 
     */ 
    value = (SPIRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_6_HIQ_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req5_hiq_enable
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clk_req5_hiq_enable( enum sys_clk_req5_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq5 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq5 input to control HiqClkEna ball 
     */ 
    switch( param )
      {
        case SYS_CLK_REQ_5_HIQ_DISABLE_E: 
           value = old_value | SYS_CLK_REQ_5_HIQ_ENABLE_PARAM_MASK; 
           break;
        case SYS_CLK_REQ_5_HIQ_ENABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_5_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req5_hiq_enable
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clk_req5_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq5 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq5 input to control HiqClkEna ball 
     */ 
    value = (SPIRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_5_HIQ_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req4_hiq_enable
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clk_req4_hiq_enable( enum sys_clk_req4_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq4 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq4 input to control HiqClkEna ball 
     */ 
    switch( param )
      {
        case SYS_CLK_REQ_4_HIQ_DISABLE_E: 
           value = old_value | SYS_CLK_REQ_4_HIQ_ENABLE_PARAM_MASK; 
           break;
        case SYS_CLK_REQ_4_HIQ_ENABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_4_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req4_hiq_enable
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clk_req4_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq4 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq4 input to control HiqClkEna ball 
     */ 
    value = (SPIRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_4_HIQ_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req3_hiq_enable
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clk_req3_hiq_enable( enum sys_clk_req3_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq3 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq3 input to control HiqClkEna ball 
     */ 
    switch( param )
      {
        case SYS_CLK_REQ_3_HIQ_DISABLE_E: 
           value = old_value | SYS_CLK_REQ_3_HIQ_ENABLE_PARAM_MASK; 
           break;
        case SYS_CLK_REQ_3_HIQ_ENABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_3_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req3_hiq_enable
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clk_req3_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq3 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq3 input to control HiqClkEna ball 
     */ 
    value = (SPIRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_3_HIQ_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req2_hiq_enable
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 1:1
 *
 **************************************************************************/
unsigned char set_sys_clk_req2_hiq_enable( enum sys_clk_req2_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq2 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq2 input to control HiqClkEna ball 
     */ 
    switch( param )
      {
        case SYS_CLK_REQ_2_HIQ_DISABLE_E: 
           value = old_value | SYS_CLK_REQ_2_HIQ_ENABLE_PARAM_MASK; 
           break;
        case SYS_CLK_REQ_2_HIQ_ENABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_2_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req2_hiq_enable
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 1:1
 *
 **************************************************************************/
unsigned char get_sys_clk_req2_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq2 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq2 input to control HiqClkEna ball 
     */ 
    value = (SPIRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_2_HIQ_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req1_hiq_enable
 *
 * IN   : param, a value to write to the regiter HiqClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 0:0
 *
 **************************************************************************/
unsigned char set_sys_clk_req1_hiq_enable( enum sys_clk_req1_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(HIQ_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq1 input to control HiqClkEna ball 
     */ 
    switch( param )
      {
        case SYS_CLK_REQ_1_HIQ_DISABLE_E: 
           value = old_value | SYS_CLK_REQ_1_HIQ_ENABLE_PARAM_MASK; 
           break;
        case SYS_CLK_REQ_1_HIQ_ENABLE_E: 
           value = old_value & ~ SYS_CLK_REQ_1_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(HIQ_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req1_hiq_enable
 *
 * RET  : Return the value of register HiqClkCtrl
 *
 * Notes : From register 0x0232, bits 0:0
 *
 **************************************************************************/
unsigned char get_sys_clk_req1_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to control HiqClkEna 
     * ball 
     * 1: valid SysClkReq1 input to control HiqClkEna ball 
     */ 
    value = (SPIRead(HIQ_CLK_CTRL_REG) & SYS_CLK_REQ_1_HIQ_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req8_hiq_enable
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 7:7
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req8_hiq_enable( enum vsim_sys_clk_req8_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq8 input to request Vsim ldo 
     * 1: valid SysClkReq8 inputs to request Vsim ldo 
     */ 
    switch( param )
      {
        case VSIM_SYS_CLK_REQ_8_HIQ_DISABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_PARAM_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req8_hiq_enable
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 7:7
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req8_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq8 input to request Vsim ldo 
     * 1: valid SysClkReq8 inputs to request Vsim ldo 
     */ 
    value = (SPIRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req7_hiq_enable
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 6:6
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req7_hiq_enable( enum vsim_sys_clk_req7_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq7 input to request Vsim ldo 
     * 1: valid SysClkReq7 inputs to request Vsim ldo 
     */ 
    switch( param )
      {
        case VSIM_SYS_CLK_REQ_7_HIQ_DISABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_PARAM_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req7_hiq_enable
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 6:6
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req7_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq7 input to request Vsim ldo 
     * 1: valid SysClkReq7 inputs to request Vsim ldo 
     */ 
    value = (SPIRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req6_hiq_enable
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 5:5
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req6_hiq_enable( enum vsim_sys_clk_req6_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq6 input to request Vsim ldo 
     * 1: valid SysClkReq6 inputs to request Vsim ldo 
     */ 
    switch( param )
      {
        case VSIM_SYS_CLK_REQ_6_HIQ_DISABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_PARAM_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req6_hiq_enable
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 5:5
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req6_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq6 input to request Vsim ldo 
     * 1: valid SysClkReq6 inputs to request Vsim ldo 
     */ 
    value = (SPIRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req5_hiq_enable
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 4:4
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req5_hiq_enable( enum vsim_sys_clk_req5_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq5 input to request Vsim ldo 
     * 1: valid SysClkReq5 inputs to request Vsim ldo 
     */ 
    switch( param )
      {
        case VSIM_SYS_CLK_REQ_5_HIQ_DISABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_PARAM_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req5_hiq_enable
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 4:4
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req5_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq5 input to request Vsim ldo 
     * 1: valid SysClkReq5 inputs to request Vsim ldo 
     */ 
    value = (SPIRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req4_hiq_enable
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 3:3
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req4_hiq_enable( enum vsim_sys_clk_req4_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq4 input to request Vsim ldo 
     * 1: valid SysClkReq4 inputs to request Vsim ldo 
     */ 
    switch( param )
      {
        case VSIM_SYS_CLK_REQ_4_HIQ_DISABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_PARAM_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req4_hiq_enable
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 3:3
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req4_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq4 input to request Vsim ldo 
     * 1: valid SysClkReq4 inputs to request Vsim ldo 
     */ 
    value = (SPIRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req3_hiq_enable
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req3_hiq_enable( enum vsim_sys_clk_req3_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq3 input to request Vsim ldo 
     * 1: valid SysClkReq2 inputs to request Vsim ldo 
     */ 
    switch( param )
      {
        case VSIM_SYS_CLK_REQ_3_HIQ_DISABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_PARAM_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req3_hiq_enable
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 2:2
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req3_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq3 input to request Vsim ldo 
     * 1: valid SysClkReq2 inputs to request Vsim ldo 
     */ 
    value = (SPIRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req2_hiq_enable
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req2_hiq_enable( enum vsim_sys_clk_req2_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq2 input to request Vsim ldo 
     * 1: valid SysClkReq2 inputs to request Vsim ldo 
     */ 
    switch( param )
      {
        case VSIM_SYS_CLK_REQ_2_HIQ_DISABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_PARAM_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req2_hiq_enable
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 1:1
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req2_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq2 input to request Vsim ldo 
     * 1: valid SysClkReq2 inputs to request Vsim ldo 
     */ 
    value = (SPIRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_sys_clk_req1_hiq_enable
 *
 * IN   : param, a value to write to the regiter VsimSysClkCtrl
 * OUT  : 
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 0:0
 *
 **************************************************************************/
unsigned char set_vsim_sys_clk_req1_hiq_enable( enum vsim_sys_clk_req1_hiq_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(VSIM_SYS_CLK_CTRL_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsim ldo 
     * 1: valid SysClkReq1 inputs to request Vsim ldo 
     */ 
    switch( param )
      {
        case VSIM_SYS_CLK_REQ_1_HIQ_DISABLE_E: 
           value = old_value | VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_PARAM_MASK; 
           break;
        case VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_E: 
           value = old_value & ~ VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(VSIM_SYS_CLK_CTRL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_sys_clk_req1_hiq_enable
 *
 * RET  : Return the value of register VsimSysClkCtrl
 *
 * Notes : From register 0x0233, bits 0:0
 *
 **************************************************************************/
unsigned char get_vsim_sys_clk_req1_hiq_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsim ldo 
     * 1: valid SysClkReq1 inputs to request Vsim ldo 
     */ 
    value = (SPIRead(VSIM_SYS_CLK_CTRL_REG) & VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_PARAM_MASK);
    return value;
  }


