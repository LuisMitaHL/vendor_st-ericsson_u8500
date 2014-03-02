/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Charger/Linux/ab8500_Charger.c
 * 
 *
 * Generated on the 04/04/2011 14:09 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : STw4500 Registers Specification Rev 2.61 5 January 2011
 *
 ***************************************************************************/
#include "ab8500_Charger.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_main_current_limitation
 *
 * RET  : Return the value of register MainChargerStatus
 *
 * Notes : From register 0x0B00, bits 7:4
 *
 **************************************************************************/
unsigned char get_main_current_limitation()
  {
    unsigned char value;


    /* 
     * Result of automatic input current limitation (to avoid 
     * Main voltage to drop). Input current limitation can be 
     * from 90mA to 1.3A by about 200mA steps. 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & CURRENT_LIMITATION_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_current_regulation
 *
 * RET  : Return the value of register MainChargerStatus
 *
 * Notes : From register 0x0B00, bits 3:3
 *
 **************************************************************************/
unsigned char get_main_current_regulation()
  {
    unsigned char value;


    /* 
     * 0: Charger is not in input current regulation 
     * 1: Charger is in input current regulation 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & CURRENT_REGULATION_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_voltage_regulation
 *
 * RET  : Return the value of register MainChargerStatus
 *
 * Notes : From register 0x0B00, bits 2:2
 *
 **************************************************************************/
unsigned char get_main_voltage_regulation()
  {
    unsigned char value;


    /* 
     * 0: charger is not in voltage regulation 
     * 1: charger is in voltage regulation 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & VOLTAGE_REGULATION_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charge_on
 *
 * RET  : Return the value of register MainChargerStatus
 *
 * Notes : From register 0x0B00, bits 1:1
 *
 **************************************************************************/
unsigned char get_main_charge_on()
  {
    unsigned char value;


    /* 
     * 0: Main charger is off 
     * 1: Main charger is on 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & MAIN_CHARGE_ON_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_level
 *
 * RET  : Return the value of register MainChargerStatus
 *
 * Notes : From register 0x0B00, bits 0:0
 *
 **************************************************************************/
unsigned char get_main_charger_level()
  {
    unsigned char value;


    /* 
     * 0: MainCharger voltage below MainChDet threshold 
     * 1: MainCharger voltage above MainChDet threshold 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & VOLTAGE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_event
 *
 * RET  : Return the value of register MainChargerEvent
 *
 * Notes : From register 0x0B01, bits 7:0
 *
 **************************************************************************/
unsigned char get_main_charger_event()
  {
    unsigned char value;


    /* 
     * List of protection events which disable main charging 
     */ 
   value = I2CRead(MAIN_CHARGER_EVENT_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_voltage_drop
 *
 * RET  : Return the value of register MainChargerEvent
 *
 * Notes : From register 0x0B01, bits 7:7
 *
 **************************************************************************/
unsigned char get_voltage_drop()
  {
    unsigned char value;


    /* 
     * MainCh voltage drops when MainCharger input current is 
     * above Wall Charger current capability 
     */ 
    value = (I2CRead(MAIN_CHARGER_EVENT_REG) & VOLTAGE_DROP_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_detected
 *
 * RET  : Return the value of register MainChargerEvent
 *
 * Notes : From register 0x0B01, bits 6:6
 *
 **************************************************************************/
unsigned char get_main_charger_detected()
  {
    unsigned char value;


    /* 
     * 0: MainCh is detected after debounce (mean's above 
     * ChOVVLow threshold) 
     * 1: MainCh is detected after debounce (mean's above 
     * ChOVVLow threshold) 
     */ 
    value = (I2CRead(MAIN_CHARGER_EVENT_REG) & MAIN_CHARGER_DETECTED_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_input_high_voltage
 *
 * RET  : Return the value of register MainChargerEvent
 *
 * Notes : From register 0x0B01, bits 5:5
 *
 **************************************************************************/
unsigned char get_main_charger_input_high_voltage()
  {
    unsigned char value;


    /* 
     * 0: MainCh input voltage is below ChOVVHigh threshold 
     * 1: MainCh input voltage is above ChOVVHigh threshold 
     */ 
    value = (I2CRead(MAIN_CHARGER_EVENT_REG) & INPUT_VOLTAGE_HIGH_THRESHOLD_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_input_low_voltage
 *
 * RET  : Return the value of register MainChargerEvent
 *
 * Notes : From register 0x0B01, bits 4:4
 *
 **************************************************************************/
unsigned char get_main_charger_input_low_voltage()
  {
    unsigned char value;


    /* 
     * 0: MainCh input voltage is below ChOVVLow threshold 
     * 1: MainCh input voltage is above ChOVVLow threshold 
     */ 
    value = (I2CRead(MAIN_CHARGER_EVENT_REG) & INPUT_VOLTAGE_LOW_THRESHOLD_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_thermal_protection
 *
 * RET  : Return the value of register MainChargerEvent
 *
 * Notes : From register 0x0B01, bits 1:1
 *
 **************************************************************************/
unsigned char get_thermal_protection()
  {
    unsigned char value;


    /* 
     * 0: Die temperature below thermal threshold. 
     * 1: Die temperature above thermal threshold. Charging is 
     * stopped. 
     */ 
    value = (I2CRead(MAIN_CHARGER_EVENT_REG) & TERMAL_STATUS_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_voltage_status
 *
 * RET  : Return the value of register MainChargerEvent
 *
 * Notes : From register 0x0B01, bits 0:0
 *
 **************************************************************************/
unsigned char get_voltage_status()
  {
    unsigned char value;


    /* 
     * 0: MainCharger voltage below MainChOVV threshold (10v). 
     * 1: MainCharger voltage above MainChOVV threshold (10v). 
     * Overvoltage or sine wave charger detected. Charger is 
     * disabled. 
     */ 
    value = (I2CRead(MAIN_CHARGER_EVENT_REG) & VOLTAGE_STATUS_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_status
 *
 * RET  : Return the value of register UsbChargerStatus
 *
 * Notes : From register 0x0B02, bits 7:0
 *
 **************************************************************************/
unsigned char get_usb_charger_status()
  {
    unsigned char value;


    /* 
     * USB charger status. 
     */ 
   value = I2CRead(USB_CHARGER_STATUS_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_voltage_close_battery_voltage
 *
 * RET  : Return the value of register UsbChargerStatus
 *
 * Notes : From register 0x0B02, bits 6:6
 *
 **************************************************************************/
unsigned char get_voltage_close_battery_voltage()
  {
    unsigned char value;


    /* 
     * 0: Vbus doesn't drops close to battery voltage 
     * 1: Vbus drops close to battery voltage 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_REG) & CLOSE_BATTERY_VOLTAGE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_voltage_regulation
 *
 * RET  : Return the value of register UsbChargerStatus
 *
 * Notes : From register 0x0B02, bits 3:3
 *
 **************************************************************************/
unsigned char get_voltage_regulation()
  {
    unsigned char value;


    /* 
     * 0: charger is not in voltage regulation 
     * 1: charger is in voltage regulation 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_REG) & VOLTAGE_REGULATION_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_on
 *
 * RET  : Return the value of register UsbChargerStatus
 *
 * Notes : From register 0x0B02, bits 2:2
 *
 **************************************************************************/
unsigned char get_usb_charger_on()
  {
    unsigned char value;


    /* 
     * 0: UsbCharger is off 
     * 1: Usbcharger is on 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_REG) & USB_CHARGER_ON_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_vbus_detection_debouce100_ms
 *
 * RET  : Return the value of register UsbChargerStatus
 *
 * Notes : From register 0x0B02, bits 1:1
 *
 **************************************************************************/
unsigned char get_vbus_detection_debouce100_ms()
  {
    unsigned char value;


    /* 
     * 0: VbusDetDbnc is low more than 100ms or VbusChNOK is low 
     * 1: VbusDetDbnc is high more than 100ms (usb charger can 
     * be enabled) 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_REG) & USB_CHARGER_CAN_BE_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_vbus_detection_debouce1_ms
 *
 * RET  : Return the value of register UsbChargerStatus
 *
 * Notes : From register 0x0B02, bits 0:0
 *
 **************************************************************************/
unsigned char get_vbus_detection_debouce1_ms()
  {
    unsigned char value;


    /* 
     * 0: Vbus voltage is below UsbChDetLow threshold more than 
     * 1ms (usb charger is disabled) 
     * 1: Vbus voltage is above UsbChDetHigh threshold more than 
     * 1ms 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_REG) & VBUS_DETECTION_DEBOUCE_1_MS_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_status2
 *
 * RET  : Return the value of register UsbChargerStatus2
 *
 * Notes : From register 0x0B03, bits 7:0
 *
 **************************************************************************/
unsigned char get_usb_charger_status2()
  {
    unsigned char value;


    /* 
     * USB charger status. 
     */ 
   value = I2CRead(USB_CHARGER_STATUS_2_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_automatic_usb_charger_current
 *
 * RET  : Return the value of register UsbChargerStatus2
 *
 * Notes : From register 0x0B03, bits 7:4
 *
 **************************************************************************/
unsigned char get_automatic_usb_charger_current()
  {
    unsigned char value;


    /* 
     * Result of automatic input current limitation (to avoid to 
     * Vbus voltage to drop). Input current limitation can be 
     * from 90mA to 1.3A by about 200mA steps 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_2_REG) & CURRENT_LIMITATION_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_device_illegal
 *
 * RET  : Return the value of register UsbChargerStatus2
 *
 * Notes : From register 0x0B03, bits 3:3
 *
 **************************************************************************/
unsigned char get_usb_device_illegal()
  {
    unsigned char value;


    /* 
     * 0: USB device connected to STw4500 is not "illegal" (does 
     * not include VbusOVV) 
     * 1: USB device connected to STw4500 is "illegal" (does not 
     * include VbusOVV) 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_2_REG) & USB_DEVICE_ILLEGAL_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_thermal_protection
 *
 * RET  : Return the value of register UsbChargerStatus2
 *
 * Notes : From register 0x0B03, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_thermal_protection()
  {
    unsigned char value;


    /* 
     * 0: Die temperature is below thermal threshold. 
     * 1: Die temperature is above thermal threshold. Charger is 
     * disabled. 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_2_REG) & TERMAL_STATUS_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_vbus_protection
 *
 * RET  : Return the value of register UsbChargerStatus2
 *
 * Notes : From register 0x0B03, bits 0:0
 *
 **************************************************************************/
unsigned char get_vbus_protection()
  {
    unsigned char value;


    /* 
     * 0: Vbus voltage below VbusOVV threshold (6.3v). 
     * 1: Vbus voltage above VbusOVV threshold (6.3v). Charger 
     * is disabled 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_2_REG) & VOLTAGE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_status3
 *
 * RET  : Return the value of register UsbChargerStatus3
 *
 * Notes : From register 0x0B05, bits 7:0
 *
 **************************************************************************/
unsigned char get_usb_charger_status3()
  {
    unsigned char value;


    /* 
     * USB charger status. 
     */ 
   value = I2CRead(USB_CHARGER_STATUS_3_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_charging_stopped
 *
 * RET  : Return the value of register UsbChargerStatus3
 *
 * Notes : From register 0x0B05, bits 3:3
 *
 **************************************************************************/
unsigned char get_charging_stopped()
  {
    unsigned char value;


    /* 
     * 0: indicated that Charging is not stopped by Btemp 
     * comparator 
     * 1: indicated that Charging is stopped by Btemp comparator 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_3_REG) & CHARGING_STOPPED_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature
 *
 * RET  : Return the value of register UsbChargerStatus3
 *
 * Notes : From register 0x0B05, bits 2:1
 *
 **************************************************************************/
unsigned char get_battery_temperature()
  {
    unsigned char value;


    /* 
     * Btemp comparator status (BtempLow threshold = typical 
     * -7°C, BtempMedium 
     * typical 3°C, BtempHigh threshold = typical 52°C or 57°C 
     * or 62°C) 
     * 00: Btemp < BtempLow 
     * 01: BtempLow < Btemp < BtempMedium 
     * 10: BtempMedium < Btemp < BtempHigh 
     * 11: BtempHigh < Btemp 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_3_REG) & BATTERY_TEMPERATURE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_voltage_below_threshold
 *
 * RET  : Return the value of register UsbChargerStatus3
 *
 * Notes : From register 0x0B05, bits 0:0
 *
 **************************************************************************/
unsigned char get_voltage_below_threshold()
  {
    unsigned char value;


    /* 
     * 0: Battery voltage is below BatOVV threshold. 
     * 1: Battery voltage is above BatOVV threshold. Charger 
     * (Main or USB) is disabled when BattOVV occurs. BattOVV 
     * default threshold is 3.7v. 4.75v threshold is selected at 
     * first primary watchdog kick 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_3_REG) & VOLTAGE_BELOW_THRESHOLD_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ch_volt_level
 *
 * IN   : param, a value to write to the regiter ChVoltLevel
 * OUT  : 
 *
 * RET  : Return the value of register ChVoltLevel
 *
 * Notes : From register 0x0B40, bits 6:0
 *
 **************************************************************************/
unsigned char set_ch_volt_level( enum volt_level param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CH_VOLT_LEVEL_REG);

    /* 
     * Common to Main and USB charger 
     */ 
  
    value =  old_value & ~VOLT_LEVEL_PARAM_MASK;


     value |=  param ;  
    I2CWrite(CH_VOLT_LEVEL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ch_volt_level
 *
 * RET  : Return the value of register ChVoltLevel
 *
 * Notes : From register 0x0B40, bits 6:0
 *
 **************************************************************************/
unsigned char get_ch_volt_level()
  {
    unsigned char value;


    /* 
     * Common to Main and USB charger 
     */ 
    value = (I2CRead(CH_VOLT_LEVEL_REG) & VOLT_LEVEL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_maximum_charging_voltage_level
 *
 * IN   : param, a value to write to the regiter ChargerVoltLevelMax
 * OUT  : 
 *
 * RET  : Return the value of register ChargerVoltLevelMax
 *
 * Notes : From register 0x0B41, bits 6:0
 *
 **************************************************************************/
unsigned char set_maximum_charging_voltage_level( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_VOLT_LEVEL_MAX_REG);

    /* 
     * Defines maximum charging voltage level. Can be written 
     * only once 
     */ 
  
    value =  old_value & ~CHARGING_VOLTAGE_LEVEL_PARAM_MASK;


    value |=  param ;  

    I2CWrite(CHARGER_VOLT_LEVEL_MAX_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_maximum_charging_voltage_level
 *
 * RET  : Return the value of register ChargerVoltLevelMax
 *
 * Notes : From register 0x0B41, bits 6:0
 *
 **************************************************************************/
unsigned char get_maximum_charging_voltage_level()
  {
    unsigned char value;


    /* 
     * Defines maximum charging voltage level. Can be written 
     * only once 
     */ 
    value = (I2CRead(CHARGER_VOLT_LEVEL_MAX_REG) & CHARGING_VOLTAGE_LEVEL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_channel_output_current_level
 *
 * IN   : param, a value to write to the regiter ChannelOutputCurrentLevel
 * OUT  : 
 *
 * RET  : Return the value of register ChannelOutputCurrentLevel
 *
 * Notes : From register 0x0B42, bits 3:0
 *
 **************************************************************************/
unsigned char set_channel_output_current_level( enum charger_current_level param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHANNEL_OUTPUT_CURRENT_LEVEL_REG);

    /* 
     * MainChCurrLevel 
     */ 
  
    value =  old_value & ~CHANNEL_OUTPUT_CURRENT_LEVEL_PARAM_MASK;


     value |=  param ;  
    I2CWrite(CHANNEL_OUTPUT_CURRENT_LEVEL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_channel_output_current_level
 *
 * RET  : Return the value of register ChannelOutputCurrentLevel
 *
 * Notes : From register 0x0B42, bits 3:0
 *
 **************************************************************************/
unsigned char get_channel_output_current_level()
  {
    unsigned char value;


    /* 
     * MainChCurrLevel 
     */ 
    value = (I2CRead(CHANNEL_OUTPUT_CURRENT_LEVEL_REG) & CHANNEL_OUTPUT_CURRENT_LEVEL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_maximum_charging_current
 *
 * IN   : param, a value to write to the regiter MaximumChargingCurrent
 * OUT  : 
 *
 * RET  : Return the value of register MaximumChargingCurrent
 *
 * Notes : From register 0x0B43, bits 3:0
 *
 **************************************************************************/
unsigned char set_maximum_charging_current( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAXIMUM_CHARGING_CURRENT_REG);

    /* 
     * Defines maximum charging current level. Can be written 
     * only once 
     */ 
  
    value =  old_value & ~MAXIMUM_CHARGING_CURRENT_PARAM_MASK;


    value |=  param ;  

    I2CWrite(MAXIMUM_CHARGING_CURRENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_maximum_charging_current
 *
 * RET  : Return the value of register MaximumChargingCurrent
 *
 * Notes : From register 0x0B43, bits 3:0
 *
 **************************************************************************/
unsigned char get_maximum_charging_current()
  {
    unsigned char value;


    /* 
     * Defines maximum charging current level. Can be written 
     * only once 
     */ 
    value = (I2CRead(MAXIMUM_CHARGING_CURRENT_REG) & MAXIMUM_CHARGING_CURRENT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_charging_current_behavor
 *
 * IN   : param, a value to write to the regiter ChannelOutputCurrentLevelBehavor
 * OUT  : 
 *
 * RET  : Return the value of register ChannelOutputCurrentLevelBehavor
 *
 * Notes : From register 0x0B44, bits 0:0
 *
 **************************************************************************/
unsigned char set_charging_current_behavor( enum charging_current param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHANNEL_OUTPUT_CURRENT_LEVEL_BEHAVOR_REG);

    /* 
     * When Btemp is above Low and below Medium temperature 
     * threshold: 
     * 0: Charging current is defined by OTP bits. 
     * 1: Charging current is set at 300mA (typ) regardless OTP 
     * bits 
     */ 
    switch( param ){
        case CHARGING_CURRENT_300_MA_E: 
           value = old_value | CHARGING_CURRENT_PARAM_MASK; 
           break;
        case CHARGING_CURRENT_DEFINED_BY_OTP_E: 
           value = old_value & ~ CHARGING_CURRENT_PARAM_MASK;
           break;
    }
  

    I2CWrite(CHANNEL_OUTPUT_CURRENT_LEVEL_BEHAVOR_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charging_current_behavor
 *
 * RET  : Return the value of register ChannelOutputCurrentLevelBehavor
 *
 * Notes : From register 0x0B44, bits 0:0
 *
 **************************************************************************/
unsigned char get_charging_current_behavor()
  {
    unsigned char value;


    /* 
     * When Btemp is above Low and below Medium temperature 
     * threshold: 
     * 0: Charging current is defined by OTP bits. 
     * 1: Charging current is set at 300mA (typ) regardless OTP 
     * bits 
     */ 
    value = (I2CRead(CHANNEL_OUTPUT_CURRENT_LEVEL_BEHAVOR_REG) & CHARGING_CURRENT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_watch_dog_timer
 *
 * IN   : param, a value to write to the regiter ChargerWatchDogTimer
 * OUT  : 
 *
 * RET  : Return the value of register ChargerWatchDogTimer
 *
 * Notes : From register 0x0B50, bits 7:0
 *
 **************************************************************************/
unsigned char set_charger_watch_dog_timer( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(CHARGER_WATCH_DOG_TIMER_REG);

    /* 
     * Defined watchdog timer. Programmable from 0s to 255 x 5s. 
     * Max is about 42 minutes. 
     */ 
    I2CWrite(CHARGER_WATCH_DOG_TIMER_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_charger_watch_dog_timer
 *
 * RET  : Return the value of register ChargerWatchDogTimer
 *
 * Notes : From register 0x0B50, bits 7:0
 *
 **************************************************************************/
unsigned char get_charger_watch_dog_timer()
  {
    unsigned char value;


    /* 
     * Defined watchdog timer. Programmable from 0s to 255 x 5s. 
     * Max is about 42 minutes. 
     */ 
   value = I2CRead(CHARGER_WATCH_DOG_TIMER_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_watch_dog_timer9
 *
 * IN   : param, a value to write to the regiter ChargerWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register ChargerWatchDogControl
 *
 * Notes : From register 0x0B51, bits 1:1
 *
 **************************************************************************/
unsigned char set_charger_watch_dog_timer9( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_WATCH_DOG_CONTROL_REG);

    /* 
     * 9° bit of ChargerWatchDogTimer 0-7 at 0xB50 
     */ 
  
    value =  old_value & ~CHARGER_WATCH_DOG_TIMER_9_PARAM_MASK;


    value |= ( param << 0x1);  

    I2CWrite(CHARGER_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_watch_dog_timer9
 *
 * RET  : Return the value of register ChargerWatchDogControl
 *
 * Notes : From register 0x0B51, bits 1:1
 *
 **************************************************************************/
unsigned char get_charger_watch_dog_timer9()
  {
    unsigned char value;


    /* 
     * 9° bit of ChargerWatchDogTimer 0-7 at 0xB50 
     */ 
    value = (I2CRead(CHARGER_WATCH_DOG_CONTROL_REG) & CHARGER_WATCH_DOG_TIMER_9_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_kick_watchdog_timer_enable
 *
 * IN   : param, a value to write to the regiter ChargerWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register ChargerWatchDogControl
 *
 * Notes : From register 0x0B51, bits 0:0
 *
 **************************************************************************/
unsigned char set_kick_watchdog_timer_enable( enum kick_watchdog_timer_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_WATCH_DOG_CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: Kick Watchdog timer (re-start timer). Auto-cleared 
     * (cleared once timer setting done). 
     */ 
    switch( param ){
        case KICK_WATCHDOG_TIMER_ENABLE_E: 
           value = old_value | KICK_WATCHDOG_TIMER_ENABLE_PARAM_MASK; 
           break;
        case KICK_WATCHDOG_TIMER_DISABLE_E: 
           value = old_value & ~ KICK_WATCHDOG_TIMER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CHARGER_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_kick_watchdog_timer_enable
 *
 * RET  : Return the value of register ChargerWatchDogControl
 *
 * Notes : From register 0x0B51, bits 0:0
 *
 **************************************************************************/
unsigned char get_kick_watchdog_timer_enable()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Kick Watchdog timer (re-start timer). Auto-cleared 
     * (cleared once timer setting done). 
     */ 
    value = (I2CRead(CHARGER_WATCH_DOG_CONTROL_REG) & KICK_WATCHDOG_TIMER_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_temperature_high
 *
 * IN   : param, a value to write to the regiter ChargerTemperatureHigh
 * OUT  : 
 *
 * RET  : Return the value of register ChargerTemperatureHigh
 *
 * Notes : From register 0x0B52, bits 1:0
 *
 **************************************************************************/
unsigned char set_charger_temperature_high( enum temperature_high_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_TEMPERATURE_HIGH_REG);

    /* 
     * Defined Btemp high threshold: 
     * 00: 57°c (typ) 
     * 01: 52°c (typ) 
     * 10: 57°c (typ) 
     * 11: 62°c (typ) 
     */ 
  

     value =  old_value & ~TEMPERATURE_HIGH_THRESHOLD_PARAM_MASK ;

    switch(  param ){

           case TEMPERATURE_HIGH_57_CELSIUS_E:
                value =  value | TEMPERATURE_HIGH_57_CELSIUS ;
                break;
           case TEMPERATURE_HIGH_52_CELSIUS_E:
                value =  value | TEMPERATURE_HIGH_52_CELSIUS ;
                break;
           case TEMPERATURE_HIGH_57_CELSIUS_1_E:
                value =  value | TEMPERATURE_HIGH_57_CELSIUS_1 ;
                break;
           case TEMPERATURE_HIGH_62_CELSIUS_E:
                value =  value | TEMPERATURE_HIGH_62_CELSIUS ;
                break;
    }
  

    I2CWrite(CHARGER_TEMPERATURE_HIGH_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_temperature_high
 *
 * RET  : Return the value of register ChargerTemperatureHigh
 *
 * Notes : From register 0x0B52, bits 1:0
 *
 **************************************************************************/
unsigned char get_charger_temperature_high()
  {
    unsigned char value;


    /* 
     * Defined Btemp high threshold: 
     * 00: 57°c (typ) 
     * 01: 52°c (typ) 
     * 10: 57°c (typ) 
     * 11: 62°c (typ) 
     */ 
    value = (I2CRead(CHARGER_TEMPERATURE_HIGH_REG) & TEMPERATURE_HIGH_THRESHOLD_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_led_current
 *
 * IN   : param, a value to write to the regiter LedIndicatorPwmControl
 * OUT  : 
 *
 * RET  : Return the value of register LedIndicatorPwmControl
 *
 * Notes : From register 0x0B53, bits 2:1
 *
 **************************************************************************/
unsigned char set_charger_led_current( enum led_current param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LED_INDICATOR_PWM_CONTROL_REG);

    /* 
     */ 
  

     value =  old_value & ~LED_CURRENT_PARAM_MASK ;

    switch(  param ){

           case LED_CURRENT_2_5_MA_E:
                value  = value  | (LED_CURRENT_2_5_MA << 0x1);
                break;
           case LED_CURRENT_1_MA_E:
                value  = value  | (LED_CURRENT_1_MA << 0x1);
                break;
           case LED_CURRENT_5_MA_E:
                value  = value  | (LED_CURRENT_5_MA << 0x1);
                break;
           case LED_CURRENT_10_MA_E:
                value  = value  | (LED_CURRENT_10_MA << 0x1);
                break;
    }
  

    I2CWrite(LED_INDICATOR_PWM_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_led_current
 *
 * RET  : Return the value of register LedIndicatorPwmControl
 *
 * Notes : From register 0x0B53, bits 2:1
 *
 **************************************************************************/
unsigned char get_charger_led_current()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(LED_INDICATOR_PWM_CONTROL_REG) & LED_CURRENT_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_led_indicator_enable
 *
 * IN   : param, a value to write to the regiter LedIndicatorPwmControl
 * OUT  : 
 *
 * RET  : Return the value of register LedIndicatorPwmControl
 *
 * Notes : From register 0x0B53, bits 0:0
 *
 **************************************************************************/
unsigned char set_led_indicator_enable( enum led_indicator_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LED_INDICATOR_PWM_CONTROL_REG);

    /* 
     * 0: disable LedIndicator PWM generator 
     * 1: enable LedIndicator PWM generator 
     */ 
    switch( param ){
        case LED_INDICATOR_ENABLE_E: 
           value = old_value | LED_INDICATOR_ENABLE_PARAM_MASK; 
           break;
        case LED_INDICATOR_DISABLE_E: 
           value = old_value & ~ LED_INDICATOR_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(LED_INDICATOR_PWM_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_led_indicator_enable
 *
 * RET  : Return the value of register LedIndicatorPwmControl
 *
 * Notes : From register 0x0B53, bits 0:0
 *
 **************************************************************************/
unsigned char get_led_indicator_enable()
  {
    unsigned char value;


    /* 
     * 0: disable LedIndicator PWM generator 
     * 1: enable LedIndicator PWM generator 
     */ 
    value = (I2CRead(LED_INDICATOR_PWM_CONTROL_REG) & LED_INDICATOR_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_led_indicator_duty
 *
 * IN   : param, a value to write to the regiter LedIndicatorDuty
 * OUT  : 
 *
 * RET  : Return the value of register LedIndicatorDuty
 *
 * Notes : From register 0x0B54, bits 7:0
 *
 **************************************************************************/
unsigned char set_led_indicator_duty( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(LED_INDICATOR_DUTY_REG);

    /* 
     * Defined LedIndicator PWM duty cycle from 1/256 to 100% 
     * with a 1/256 step 
     */ 
    I2CWrite(LED_INDICATOR_DUTY_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_led_indicator_duty
 *
 * RET  : Return the value of register LedIndicatorDuty
 *
 * Notes : From register 0x0B54, bits 7:0
 *
 **************************************************************************/
unsigned char get_led_indicator_duty()
  {
    unsigned char value;


    /* 
     * Defined LedIndicator PWM duty cycle from 1/256 to 100% 
     * with a 1/256 step 
     */ 
   value = I2CRead(LED_INDICATOR_DUTY_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_dithering_enable
 *
 * IN   : param, a value to write to the regiter BatteryThresholdControl
 * OUT  : 
 *
 * RET  : Return the value of register BatteryThresholdControl
 *
 * Notes : From register 0x0B55, bits 2:2
 *
 **************************************************************************/
unsigned char set_charger_dithering_enable( enum clock_charger_dithering_enabled param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BATTERY_THRESHOLD_CONTROL_REG);

    /* 
     * 0: clock charger dithering disabled 
     * 1: clock charger dithering enabled 
     */ 
    switch( param ){
        case CLOCK_CHARGER_DITHERING_ENABLE_E: 
           value = old_value | CLOCK_CHARGER_DITHERING_ENABLED_PARAM_MASK; 
           break;
        case CLOCK_CHARGER_DITHERING_DISABLE_E: 
           value = old_value & ~ CLOCK_CHARGER_DITHERING_ENABLED_PARAM_MASK;
           break;
    }
  

    I2CWrite(BATTERY_THRESHOLD_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_dithering_enable
 *
 * RET  : Return the value of register BatteryThresholdControl
 *
 * Notes : From register 0x0B55, bits 2:2
 *
 **************************************************************************/
unsigned char get_charger_dithering_enable()
  {
    unsigned char value;


    /* 
     * 0: clock charger dithering disabled 
     * 1: clock charger dithering enabled 
     */ 
    value = (I2CRead(BATTERY_THRESHOLD_CONTROL_REG) & CLOCK_CHARGER_DITHERING_ENABLED_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_o_v_v_enable
 *
 * IN   : param, a value to write to the regiter BatteryThresholdControl
 * OUT  : 
 *
 * RET  : Return the value of register BatteryThresholdControl
 *
 * Notes : From register 0x0B55, bits 1:1
 *
 **************************************************************************/
unsigned char set_battery_o_v_v_enable( enum battery_o_v_v_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BATTERY_THRESHOLD_CONTROL_REG);

    /* 
     * can be enabled by charger state machine or by Sw, default 
     * value depends if a charger is plug in 
     */ 
    switch( param ){
        case BATTERY_OVV_ENABLE_E: 
           value = old_value | BATTERY_OVV_ENABLE_PARAM_MASK; 
           break;
        case BATTERY_OVV_DISABLE_E: 
           value = old_value & ~ BATTERY_OVV_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(BATTERY_THRESHOLD_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_o_v_v_enable
 *
 * RET  : Return the value of register BatteryThresholdControl
 *
 * Notes : From register 0x0B55, bits 1:1
 *
 **************************************************************************/
unsigned char get_battery_o_v_v_enable()
  {
    unsigned char value;


    /* 
     * can be enabled by charger state machine or by Sw, default 
     * value depends if a charger is plug in 
     */ 
    value = (I2CRead(BATTERY_THRESHOLD_CONTROL_REG) & BATTERY_OVV_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_threshold
 *
 * IN   : param, a value to write to the regiter BatteryThresholdControl
 * OUT  : 
 *
 * RET  : Return the value of register BatteryThresholdControl
 *
 * Notes : From register 0x0B55, bits 0:0
 *
 **************************************************************************/
unsigned char set_battery_threshold( enum battery_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BATTERY_THRESHOLD_CONTROL_REG);

    /* 
     * 0: BattOVV threshold = 3.7v (typ) 
     * 1: BattOVV threshold = 4.75v (typ) 
     */ 
    switch( param ){
        case BATTERY_THRESHOLD_4_75_E: 
           value = old_value | BATTERY_THRESHOLD_PARAM_MASK; 
           break;
        case BATTERY_THRESHOLD_3_70_E: 
           value = old_value & ~ BATTERY_THRESHOLD_PARAM_MASK;
           break;
    }
  

    I2CWrite(BATTERY_THRESHOLD_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_threshold
 *
 * RET  : Return the value of register BatteryThresholdControl
 *
 * Notes : From register 0x0B55, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_threshold()
  {
    unsigned char value;


    /* 
     * 0: BattOVV threshold = 3.7v (typ) 
     * 1: BattOVV threshold = 4.75v (typ) 
     */ 
    value = (I2CRead(BATTERY_THRESHOLD_CONTROL_REG) & BATTERY_THRESHOLD_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_charger
 *
 * IN   : param, a value to write to the regiter ChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register ChargerControl
 *
 * Notes : From register 0x0B56, bits 0:0
 *
 **************************************************************************/
unsigned char set_reset_charger( enum reset_charger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_CONTROL_REG);

    /* 
     * 0: reset following registers (#) 
     * 1: inactive 
     */ 
    switch( param ){
        case UN_RESET_E: 
           value = old_value | RESET_CHARGER_ENABLE_PARAM_MASK; 
           break;
        case RESET_E: 
           value = old_value & ~ RESET_CHARGER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_charger
 *
 * RET  : Return the value of register ChargerControl
 *
 * Notes : From register 0x0B56, bits 0:0
 *
 **************************************************************************/
unsigned char get_reset_charger()
  {
    unsigned char value;


    /* 
     * 0: reset following registers (#) 
     * 1: inactive 
     */ 
    value = (I2CRead(CHARGER_CONTROL_REG) & RESET_CHARGER_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbat_overshoot_control
 *
 * IN   : param, a value to write to the regiter MainChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register MainChargerControl
 *
 * Notes : From register 0x0B80, bits 1:1
 *
 **************************************************************************/
unsigned char set_vbat_overshoot_control( enum vbat_overshoot_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_CHARGER_CONTROL_REG);

    /* 
     * 0: enable Vbat overshoot control 
     * 1: disable Vbat overshoot control 
     */ 
    switch( param ){
        case VBAT_OVERSHOOT_DISABLE_E: 
           value = old_value | VBAT_OVERSHOOT_ENABLE_PARAM_MASK; 
           break;
        case VBAT_OVERSHOOT_ENABLE_E: 
           value = old_value & ~ VBAT_OVERSHOOT_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(MAIN_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbat_overshoot_control
 *
 * RET  : Return the value of register MainChargerControl
 *
 * Notes : From register 0x0B80, bits 1:1
 *
 **************************************************************************/
unsigned char get_vbat_overshoot_control()
  {
    unsigned char value;


    /* 
     * 0: enable Vbat overshoot control 
     * 1: disable Vbat overshoot control 
     */ 
    value = (I2CRead(MAIN_CHARGER_CONTROL_REG) & VBAT_OVERSHOOT_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_enable
 *
 * IN   : param, a value to write to the regiter MainChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register MainChargerControl
 *
 * Notes : From register 0x0B80, bits 0:0
 *
 **************************************************************************/
unsigned char set_main_charger_enable( enum main_charger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_CHARGER_CONTROL_REG);

    /* 
     * 0: Disable MainCharger 
     * 1: Enable MainCharger 
     */ 
    switch( param ){
        case MAIN_CHARGER_ENABLE_E: 
           value = old_value | MAIN_CHARGER_ENABLE_PARAM_MASK; 
           break;
        case MAIN_CHARGER_DISABLE_E: 
           value = old_value & ~ MAIN_CHARGER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(MAIN_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_enable
 *
 * RET  : Return the value of register MainChargerControl
 *
 * Notes : From register 0x0B80, bits 0:0
 *
 **************************************************************************/
unsigned char get_main_charger_enable()
  {
    unsigned char value;


    /* 
     * 0: Disable MainCharger 
     * 1: Enable MainCharger 
     */ 
    value = (I2CRead(MAIN_CHARGER_CONTROL_REG) & MAIN_CHARGER_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_channel_max
 *
 * IN   : param, a value to write to the regiter MainChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register MainChargerControl2
 *
 * Notes : From register 0x0B81, bits 2:2
 *
 **************************************************************************/
unsigned char set_main_channel_max( enum max_current param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_CHARGER_CONTROL_2_REG);

    /* 
     * 0: I charge Max = 900 mA 
     * 1: I charge Max = 1.5 A 
     */ 
    switch( param ){
        case CHARGE_MAX_900MA_E: 
           value = old_value | MAX_CURRENT_PARAM_MASK; 
           break;
        case CHARGE_MAX_1A5_E: 
           value = old_value & ~ MAX_CURRENT_PARAM_MASK;
           break;
    }
  

    I2CWrite(MAIN_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_channel_max
 *
 * RET  : Return the value of register MainChargerControl2
 *
 * Notes : From register 0x0B81, bits 2:2
 *
 **************************************************************************/
unsigned char get_main_channel_max()
  {
    unsigned char value;


    /* 
     * 0: I charge Max = 900 mA 
     * 1: I charge Max = 1.5 A 
     */ 
    value = (I2CRead(MAIN_CHARGER_CONTROL_2_REG) & MAX_CURRENT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_thermal_threshold
 *
 * IN   : param, a value to write to the regiter MainChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register MainChargerControl2
 *
 * Notes : From register 0x0B81, bits 1:0
 *
 **************************************************************************/
unsigned char set_thermal_threshold( enum thermal_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_CHARGER_CONTROL_2_REG);

    /* 
     * 00: Positive thermal threshold = 150 °C 
     * 01: Positive thermal threshold = 159 °C 
     * 10: Positive thermal threshold = 168 °C 
     * 11: Positive thermal threshold = 140 °C 
     */ 
  

     value =  old_value & ~THERMAL_THRESHOLD_PARAM_MASK ;

    switch(  param ){

           case THERMAL_THRESHOLD_150_E:
                value =  value | THERMAL_THRESHOLD_150 ;
                break;
           case THERMAL_THRESHOLD_159_E:
                value =  value | THERMAL_THRESHOLD_159 ;
                break;
           case THERMAL_THRESHOLD_168_E:
                value =  value | THERMAL_THRESHOLD_168 ;
                break;
           case THERMAL_THRESHOLD_140_E:
                value =  value | THERMAL_THRESHOLD_140 ;
                break;
    }
  

    I2CWrite(MAIN_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_thermal_threshold
 *
 * RET  : Return the value of register MainChargerControl2
 *
 * Notes : From register 0x0B81, bits 1:0
 *
 **************************************************************************/
unsigned char get_thermal_threshold()
  {
    unsigned char value;


    /* 
     * 00: Positive thermal threshold = 150 °C 
     * 01: Positive thermal threshold = 159 °C 
     * 10: Positive thermal threshold = 168 °C 
     * 11: Positive thermal threshold = 140 °C 
     */ 
    value = (I2CRead(MAIN_CHARGER_CONTROL_2_REG) & THERMAL_THRESHOLD_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_ch_input_curr
 *
 * IN   : param, a value to write to the regiter MainChargerCurrentControl
 * OUT  : 
 *
 * RET  : Return the value of register MainChargerCurrentControl
 *
 * Notes : From register 0x0B82, bits 7:4
 *
 **************************************************************************/
unsigned char set_main_ch_input_curr( enum charger_current_level param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_CHARGER_CURRENT_CONTROL_REG);

    /* 
     * MainChInputCurr 
     */ 
  
    value =  old_value & ~MAIN_CH_INPUT_CURR_PARAM_MASK;


     value |= ( param << 0x4);  
    I2CWrite(MAIN_CHARGER_CURRENT_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_ch_input_curr
 *
 * RET  : Return the value of register MainChargerCurrentControl
 *
 * Notes : From register 0x0B82, bits 7:4
 *
 **************************************************************************/
unsigned char get_main_ch_input_curr()
  {
    unsigned char value;


    /* 
     * MainChInputCurr 
     */ 
    value = (I2CRead(MAIN_CHARGER_CURRENT_CONTROL_REG) & MAIN_CH_INPUT_CURR_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_overshoot_control
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 4:4
 *
 **************************************************************************/
unsigned char set_usb_overshoot_control( enum usb_overshoot_control_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * 0: enable Vbat overshoot control 
     * 1: disable Vbat overshoot control 
     */ 
    switch( param ){
        case USB_OVERSHOOT_CONTROL_DISABLE_E: 
           value = old_value | USB_OVERSHOOT_CONTROL_ENABLE_PARAM_MASK; 
           break;
        case USB_OVERSHOOT_CONTROL_ENABLE_E: 
           value = old_value & ~ USB_OVERSHOOT_CONTROL_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_overshoot_control
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 4:4
 *
 **************************************************************************/
unsigned char get_usb_overshoot_control()
  {
    unsigned char value;


    /* 
     * 0: enable Vbat overshoot control 
     * 1: disable Vbat overshoot control 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & USB_OVERSHOOT_CONTROL_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_current_limit_disable
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 3:3
 *
 **************************************************************************/
unsigned char set_ubs_current_limit_disable( enum current_limit_disable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * 0: inactive (current limitation: 900mA in HS mode, 500mA 
     * in Chirp mode) 
     * 1: disable current limitation (current limitation from 
     * ULPI path disable, current max can be up to 1.5A) 
     */ 
    switch( param ){
        case CURRENT_LIMIT_DISABLE_DISABLE_E: 
           value = old_value | CURRENT_LIMIT_DISABLE_PARAM_MASK; 
           break;
        case CURRENT_LIMIT_DISABLE_INACTIVE_E: 
           value = old_value & ~ CURRENT_LIMIT_DISABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_current_limit_disable
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 3:3
 *
 **************************************************************************/
unsigned char get_ubs_current_limit_disable()
  {
    unsigned char value;


    /* 
     * 0: inactive (current limitation: 900mA in HS mode, 500mA 
     * in Chirp mode) 
     * 1: disable current limitation (current limitation from 
     * ULPI path disable, current max can be up to 1.5A) 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & CURRENT_LIMIT_DISABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mask_usb_chirp
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 2:2
 *
 **************************************************************************/
unsigned char set_mask_usb_chirp( enum mask_usb_chirp_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * [0]: no effect 
     * [1]: mask the signal chirp (forcing it to 0) to disable 
     * current limitation when in USB Chirp signaling ` 
     */ 
    switch( param ){
        case MASK_USB_CHIRP_ENABLE_E: 
           value = old_value | MASK_USB_CHIRP_ENABLE_PARAM_MASK; 
           break;
        case MASK_USB_CHIRP_DISABLE_E: 
           value = old_value & ~ MASK_USB_CHIRP_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mask_usb_chirp
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 2:2
 *
 **************************************************************************/
unsigned char get_mask_usb_chirp()
  {
    unsigned char value;


    /* 
     * [0]: no effect 
     * [1]: mask the signal chirp (forcing it to 0) to disable 
     * current limitation when in USB Chirp signaling ` 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & MASK_USB_CHIRP_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbat_overshoot_control_enable
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 1:1
 *
 **************************************************************************/
unsigned char set_vbat_overshoot_control_enable( enum vbat_overshoot_control_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * [0]: enable Vbat overshoot control 
     * [1]: disable Vbat overshoot control (default) 
     */ 
    switch( param ){
        case VBAT_OVERSHOOT_CONTROL_DISABLE_E: 
           value = old_value | VBAT_OVERSHOOT_CONTROL_ENABLE_PARAM_MASK; 
           break;
        case VBAT_OVERSHOOT_CONTROL_ENABLE_E: 
           value = old_value & ~ VBAT_OVERSHOOT_CONTROL_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbat_overshoot_control_enable
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 1:1
 *
 **************************************************************************/
unsigned char get_vbat_overshoot_control_enable()
  {
    unsigned char value;


    /* 
     * [0]: enable Vbat overshoot control 
     * [1]: disable Vbat overshoot control (default) 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & VBAT_OVERSHOOT_CONTROL_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_charger_enable
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 0:0
 *
 **************************************************************************/
unsigned char set_ubs_charger_enable( enum ubs_charger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * 0: Disable USB charge 
     * 1: Enable USB charger 
     */ 
    switch( param ){
        case UBS_CHARGER_ENABLE_E: 
           value = old_value | UBS_CHARGER_ENABLE_PARAM_MASK; 
           break;
        case UBS_CHARGER_DISABLE_E: 
           value = old_value & ~ UBS_CHARGER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_charger_enable
 *
 * RET  : Return the value of register UsbChargerControl
 *
 * Notes : From register 0x0BC0, bits 0:0
 *
 **************************************************************************/
unsigned char get_ubs_charger_enable()
  {
    unsigned char value;


    /* 
     * 0: Disable USB charge 
     * 1: Enable USB charger 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & UBS_CHARGER_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_monitor
 *
 * IN   : param, a value to write to the regiter UsbChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl2
 *
 * Notes : From register 0x0BC1, bits 7:7
 *
 **************************************************************************/
unsigned char set_vbus_monitor( enum vbus_monitor_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_2_REG);

    /* 
     * 0: Inactive 
     * 1: when Vbus drops below VbusValid (4.4V), Usb charging 
     * will stop. SW needs to restart charger by setting 
     * 'UsbChEna' bit low then high 
     */ 
    switch( param ){
        case VBUS_MONITOR_ENABLE_E: 
           value = old_value | VBUS_MONITOR_ENABLE_PARAM_MASK; 
           break;
        case VBUS_MONITOR_DISABLE_E: 
           value = old_value & ~ VBUS_MONITOR_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_monitor
 *
 * RET  : Return the value of register UsbChargerControl2
 *
 * Notes : From register 0x0BC1, bits 7:7
 *
 **************************************************************************/
unsigned char get_vbus_monitor()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: when Vbus drops below VbusValid (4.4V), Usb charging 
     * will stop. SW needs to restart charger by setting 
     * 'UsbChEna' bit low then high 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_2_REG) & VBUS_MONITOR_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_voltage_stop
 *
 * IN   : param, a value to write to the regiter UsbChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl2
 *
 * Notes : From register 0x0BC1, bits 6:3
 *
 **************************************************************************/
unsigned char set_ubs_voltage_stop( enum vbus_voltage_stop param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_2_REG);

    /* 
     * Charging stops is Vbus voltage is higher than 
     */ 
  
    value =  old_value & ~VBUS_VOLTAGE_STOP_PARAM_MASK;


     value |= ( param << 0x3);  
    I2CWrite(USB_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_voltage_stop
 *
 * RET  : Return the value of register UsbChargerControl2
 *
 * Notes : From register 0x0BC1, bits 6:3
 *
 **************************************************************************/
unsigned char get_ubs_voltage_stop()
  {
    unsigned char value;


    /* 
     * Charging stops is Vbus voltage is higher than 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_2_REG) & VBUS_VOLTAGE_STOP_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_charger_max_current
 *
 * IN   : param, a value to write to the regiter UsbChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl2
 *
 * Notes : From register 0x0BC1, bits 2:2
 *
 **************************************************************************/
unsigned char set_ubs_charger_max_current( enum max_current param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_2_REG);

    /* 
     * 0: I charge Max = 900 mA 
     * 1: I charge Max = 1.5 A 
     */ 
    switch( param ){
        case CHARGE_MAX_900MA_E: 
           value = old_value | MAX_CURRENT_PARAM_MASK; 
           break;
        case CHARGE_MAX_1A5_E: 
           value = old_value & ~ MAX_CURRENT_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_charger_max_current
 *
 * RET  : Return the value of register UsbChargerControl2
 *
 * Notes : From register 0x0BC1, bits 2:2
 *
 **************************************************************************/
unsigned char get_ubs_charger_max_current()
  {
    unsigned char value;


    /* 
     * 0: I charge Max = 900 mA 
     * 1: I charge Max = 1.5 A 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_2_REG) & MAX_CURRENT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_charger_thermal_threshold
 *
 * IN   : param, a value to write to the regiter UsbChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl2
 *
 * Notes : From register 0x0BC1, bits 1:0
 *
 **************************************************************************/
unsigned char set_ubs_charger_thermal_threshold( enum thermal_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_2_REG);

    /* 
     * 00: Positive thermal threshold = 150 °C 
     * 01: Positive thermal threshold = 159 °C 
     * 10: Positive thermal threshold = 168 °C 
     * 11: Positive thermal threshold = 140 °C 
     */ 
  

     value =  old_value & ~UBS_CHARGER_THERMAL_THRESHOLD_PARAM_MASK ;

    switch(  param ){

           case THERMAL_THRESHOLD_150_E:
                value =  value | THERMAL_THRESHOLD_150 ;
                break;
           case THERMAL_THRESHOLD_159_E:
                value =  value | THERMAL_THRESHOLD_159 ;
                break;
           case THERMAL_THRESHOLD_168_E:
                value =  value | THERMAL_THRESHOLD_168 ;
                break;
           case THERMAL_THRESHOLD_140_E:
                value =  value | THERMAL_THRESHOLD_140 ;
                break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_charger_thermal_threshold
 *
 * RET  : Return the value of register UsbChargerControl2
 *
 * Notes : From register 0x0BC1, bits 1:0
 *
 **************************************************************************/
unsigned char get_ubs_charger_thermal_threshold()
  {
    unsigned char value;


    /* 
     * 00: Positive thermal threshold = 150 °C 
     * 01: Positive thermal threshold = 159 °C 
     * 10: Positive thermal threshold = 168 °C 
     * 11: Positive thermal threshold = 140 °C 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_2_REG) & THERMAL_THRESHOLD_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_charger_current_limit
 *
 * IN   : param, a value to write to the regiter UsbChargerControl3
 * OUT  : 
 *
 * RET  : Return the value of register UsbChargerControl3
 *
 * Notes : From register 0x0BC2, bits 6:4
 *
 **************************************************************************/
unsigned char set_ubs_charger_current_limit( enum usb_input_current_charge param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_3_REG);

    /* 
     */ 
  

     value =  old_value & ~UBS_CHARGER_CURRENT_LIMIT_PARAM_MASK ;

    switch(  param ){

           case USB_CURRENT_CHARGE_50MA_E:
                value  = value  | (USB_CURRENT_CHARGE_50MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_98MA_E:
                value  = value  | (USB_CURRENT_CHARGE_98MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_193MA_E:
                value  = value  | (USB_CURRENT_CHARGE_193MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_290MA_E:
                value  = value  | (USB_CURRENT_CHARGE_290MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_380MA_E:
                value  = value  | (USB_CURRENT_CHARGE_380MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_450MA_E:
                value  = value  | (USB_CURRENT_CHARGE_450MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_500MA_E:
                value  = value  | (USB_CURRENT_CHARGE_500MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_600MA_E:
                value  = value  | (USB_CURRENT_CHARGE_600MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_700MA_E:
                value  = value  | (USB_CURRENT_CHARGE_700MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_800MA_E:
                value  = value  | (USB_CURRENT_CHARGE_800MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_900MA_E:
                value  = value  | (USB_CURRENT_CHARGE_900MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1000MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1000MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1100MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1100MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1300MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1300MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1400MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1400MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1500MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1500MA << 0x4);
                break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_charger_current_limit
 *
 * RET  : Return the value of register UsbChargerControl3
 *
 * Notes : From register 0x0BC2, bits 6:4
 *
 **************************************************************************/
unsigned char get_ubs_charger_current_limit()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_3_REG) & UBS_CHARGER_CURRENT_LIMIT_PARAM_MASK) >> 4;
    return value;
  }


