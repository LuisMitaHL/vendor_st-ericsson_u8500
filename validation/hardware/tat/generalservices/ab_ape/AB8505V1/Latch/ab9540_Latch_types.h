/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Latch/Linux/ab9540_Latch_types.h
 * 
 *
 * Generated on the 13/01/2012 09:19 by the 'super-cool' code generator 
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_LATCH_LINUX_AB9540_LATCH_TYPES_H
#define C__LISP_PRODUCTION_AB9540_AB9540_LATCH_LINUX_AB9540_LATCH_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


//  
//  Rising edge on PonKey1 ball detected.
//  Register LatchedInterrupt1 0x0E20, Bits 7:7, typedef 
//  rising_pon_key1_latched_interrupt
//   
//  
#define RISING_PON_KEY_1_LATCHED_INTERRUPT_MASK 0x80U
#define RISING_PON_KEY_1_LATCHED_INTERRUPT_OFFSET 7
#define RISING_PON_KEY_1_LATCHED_INTERRUPT_STOP_BIT 7
#define RISING_PON_KEY_1_LATCHED_INTERRUPT_START_BIT 7
#define RISING_PON_KEY_1_LATCHED_INTERRUPT_WIDTH 1

#define LATCHED_INTERRUPT_1_REG 0xE20



//  
//  Falling edge on PonKey1 ball detected.
//  Register LatchedInterrupt1 0x0E20, Bits 6:6, typedef 
//  falling_pon_key1_latched_interrupt
//   
//  
#define FALLING_PON_KEY_1_LATCHED_INTERRUPT_MASK 0x40U
#define FALLING_PON_KEY_1_LATCHED_INTERRUPT_OFFSET 6
#define FALLING_PON_KEY_1_LATCHED_INTERRUPT_STOP_BIT 6
#define FALLING_PON_KEY_1_LATCHED_INTERRUPT_START_BIT 6
#define FALLING_PON_KEY_1_LATCHED_INTERRUPT_WIDTH 1




//  
//  Rising edge on PonKey2 ball detected.
//  Register LatchedInterrupt1 0x0E20, Bits 5:5, typedef 
//  rising_pon_key2_latched_interrupt
//   
//  
#define RISING_PON_KEY_2_LATCHED_INTERRUPT_MASK 0x20U
#define RISING_PON_KEY_2_LATCHED_INTERRUPT_OFFSET 5
#define RISING_PON_KEY_2_LATCHED_INTERRUPT_STOP_BIT 5
#define RISING_PON_KEY_2_LATCHED_INTERRUPT_START_BIT 5
#define RISING_PON_KEY_2_LATCHED_INTERRUPT_WIDTH 1




//  
//  Falling edge on PonKey2 ball detected.
//  Register LatchedInterrupt1 0x0E20, Bits 4:4, typedef 
//  falling_pon_key2_latched_interrupt
//   
//  
#define FALLING_PON_KEY_2_LATCHED_INTERRUPT_MASK 0x10U
#define FALLING_PON_KEY_2_LATCHED_INTERRUPT_OFFSET 4
#define FALLING_PON_KEY_2_LATCHED_INTERRUPT_STOP_BIT 4
#define FALLING_PON_KEY_2_LATCHED_INTERRUPT_START_BIT 4
#define FALLING_PON_KEY_2_LATCHED_INTERRUPT_WIDTH 1




//  
//  Thermal warning.
//  Register LatchedInterrupt1 0x0E20, Bits 3:3, typedef 
//  thermal_warning_latched_interrupt
//   
//  
#define THERMAL_WARNING_LATCHED_INTERRUPT_MASK 0x8U
#define THERMAL_WARNING_LATCHED_INTERRUPT_OFFSET 3
#define THERMAL_WARNING_LATCHED_INTERRUPT_STOP_BIT 3
#define THERMAL_WARNING_LATCHED_INTERRUPT_START_BIT 3
#define THERMAL_WARNING_LATCHED_INTERRUPT_WIDTH 1




//  
//  Tvset (75 Ohms) connection on CVBS ball detected.
//  Register LatchedInterrupt1 0x0E20, Bits 2:2, typedef plug_tv_latched_interrupt
//   
//  
#define PLUG_TV_LATCHED_INTERRUPT_MASK 0x4U
#define PLUG_TV_LATCHED_INTERRUPT_OFFSET 2
#define PLUG_TV_LATCHED_INTERRUPT_STOP_BIT 2
#define PLUG_TV_LATCHED_INTERRUPT_START_BIT 2
#define PLUG_TV_LATCHED_INTERRUPT_WIDTH 1




//  
//  Tvset (75 Ohms) disconnection on CVBS ball detected.
//  Register LatchedInterrupt1 0x0E20, Bits 1:1, typedef un_plug_tv_latched_interrupt
//   
//  
#define UN_PLUG_TV_LATCHED_INTERRUPT_MASK 0x2U
#define UN_PLUG_TV_LATCHED_INTERRUPT_OFFSET 1
#define UN_PLUG_TV_LATCHED_INTERRUPT_STOP_BIT 1
#define UN_PLUG_TV_LATCHED_INTERRUPT_START_BIT 1
#define UN_PLUG_TV_LATCHED_INTERRUPT_WIDTH 1




//  
//  Rising edge on Vbus ball detected.
//  Register LatchedInterrupt2 0x0E21, Bits 7:7, typedef vbus_rising_edge_latch
//   
//  
#define VBUS_RISING_EDGE_LATCH_MASK 0x80U
#define VBUS_RISING_EDGE_LATCH_OFFSET 7
#define VBUS_RISING_EDGE_LATCH_STOP_BIT 7
#define VBUS_RISING_EDGE_LATCH_START_BIT 7
#define VBUS_RISING_EDGE_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_2_REG 0xE21



//  
//  Falling edge on Vbus ball detected.
//  Register LatchedInterrupt2 0x0E21, Bits 6:6, typedef vbus_falling_edge_latch
//   
//  
#define VBUS_FALLING_EDGE_LATCH_MASK 0x40U
#define VBUS_FALLING_EDGE_LATCH_OFFSET 6
#define VBUS_FALLING_EDGE_LATCH_STOP_BIT 6
#define VBUS_FALLING_EDGE_LATCH_START_BIT 6
#define VBUS_FALLING_EDGE_LATCH_WIDTH 1




//  
//  Main charger plug detected.
//  Register LatchedInterrupt2 0x0E21, Bits 3:3, typedef main_charger_plug_latch
//   
//  
#define MAIN_CHARGER_PLUG_LATCH_MASK 0x8U
#define MAIN_CHARGER_PLUG_LATCH_OFFSET 3
#define MAIN_CHARGER_PLUG_LATCH_STOP_BIT 3
#define MAIN_CHARGER_PLUG_LATCH_START_BIT 3
#define MAIN_CHARGER_PLUG_LATCH_WIDTH 1




//  
//  Main charger unplug detected.
//  Register LatchedInterrupt2 0x0E21, Bits 2:2, typedef main_charger_un_plug_latch
//   
//  
#define MAIN_CHARGER_UN_PLUG_LATCH_MASK 0x4U
#define MAIN_CHARGER_UN_PLUG_LATCH_OFFSET 2
#define MAIN_CHARGER_UN_PLUG_LATCH_STOP_BIT 2
#define MAIN_CHARGER_UN_PLUG_LATCH_START_BIT 2
#define MAIN_CHARGER_UN_PLUG_LATCH_WIDTH 1




//  
//  TVbatA ball goes upper over voltage threshold (charge stopped).
//  Register LatchedInterrupt2 0x0E21, Bits 0:0, typedef battery_over_voltage_latch
//   
//  
#define BATTERY_OVER_VOLTAGE_LATCH_MASK 0x1U
#define BATTERY_OVER_VOLTAGE_LATCH_OFFSET 0
#define BATTERY_OVER_VOLTAGE_LATCH_STOP_BIT 0
#define BATTERY_OVER_VOLTAGE_LATCH_START_BIT 0
#define BATTERY_OVER_VOLTAGE_LATCH_WIDTH 1




//  
//  Icharge greater Icharge max programmed detected.
//  Register LatchedInterrupt3 0x0E22, Bits 7:7, typedef main_charger_over_current_latch
//   
//  
#define MAIN_CHARGER_OVER_CURRENT_LATCH_MASK 0x80U
#define MAIN_CHARGER_OVER_CURRENT_LATCH_OFFSET 7
#define MAIN_CHARGER_OVER_CURRENT_LATCH_STOP_BIT 7
#define MAIN_CHARGER_OVER_CURRENT_LATCH_START_BIT 7
#define MAIN_CHARGER_OVER_CURRENT_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_3_REG 0xE22



//  
//  Overvoltage on Vbus ball detected.
//  Register LatchedInterrupt3 0x0E22, Bits 6:6, typedef overvoltage_on_vbus_latch
//   
//  
#define OVERVOLTAGE_ON_VBUS_LATCH_MASK 0x40U
#define OVERVOLTAGE_ON_VBUS_LATCH_OFFSET 6
#define OVERVOLTAGE_ON_VBUS_LATCH_STOP_BIT 6
#define OVERVOLTAGE_ON_VBUS_LATCH_START_BIT 6
#define OVERVOLTAGE_ON_VBUS_LATCH_WIDTH 1




//  
//  Watchdog charger expiration detected.
//  Register LatchedInterrupt3 0x0E22, Bits 5:5, typedef 
//  watchdog_charger_expiration_latch
//   
//  
#define WATCHDOG_CHARGER_EXPIRATION_LATCH_MASK 0x20U
#define WATCHDOG_CHARGER_EXPIRATION_LATCH_OFFSET 5
#define WATCHDOG_CHARGER_EXPIRATION_LATCH_STOP_BIT 5
#define WATCHDOG_CHARGER_EXPIRATION_LATCH_START_BIT 5
#define WATCHDOG_CHARGER_EXPIRATION_LATCH_WIDTH 1




//  
//  Battery removal.
//  Register LatchedInterrupt3 0x0E22, Bits 4:4, typedef battery_removal_latch
//   
//  
#define BATTERY_REMOVAL_LATCH_MASK 0x10U
#define BATTERY_REMOVAL_LATCH_OFFSET 4
#define BATTERY_REMOVAL_LATCH_STOP_BIT 4
#define BATTERY_REMOVAL_LATCH_START_BIT 4
#define BATTERY_REMOVAL_LATCH_WIDTH 1




//  
//  RTC timer reaches alarm time.
//  Register LatchedInterrupt3 0x0E22, Bits 2:2, typedef reach_alarm_time_latch
//   
//  
#define RTC_ALARM_LATCH_MASK 0x4U
#define RTC_ALARM_LATCH_OFFSET 2
#define RTC_ALARM_LATCH_STOP_BIT 2
#define RTC_ALARM_LATCH_START_BIT 2
#define RTC_ALARM_LATCH_WIDTH 1




//  
//  RTC timer reaches a 60s period.
//  Register LatchedInterrupt3 0x0E22, Bits 1:1, typedef period60s_latch
//   
//  
#define RTC_6_0S_LATCH_MASK 0x2U
#define RTC_6_0S_LATCH_OFFSET 1
#define RTC_6_0S_LATCH_STOP_BIT 1
#define RTC_6_0S_LATCH_START_BIT 1
#define RTC_6_0S_LATCH_WIDTH 1




//  
//  Usb charger was in dropout and internal input current loop update current regulation 
//  to avoid usb charger to drop.
//  Register LatchedInterrupt3 0x0E22, Bits 0:0, typedef usb_drop_out_latch
//   
//  
#define USB_CHARGER_DROPOUT_LATCH_MASK 0x1U
#define USB_CHARGER_DROPOUT_LATCH_OFFSET 0
#define USB_CHARGER_DROPOUT_LATCH_STOP_BIT 0
#define USB_CHARGER_DROPOUT_LATCH_START_BIT 0
#define USB_CHARGER_DROPOUT_LATCH_WIDTH 1




//  
//  BackUpBat ball voltage goes above RtcBackupChg register.
//  Register LatchedInterrupt4 0x0E23, Bits 7:7, typedef rtc_battery_high_latch
//   
//  
#define RTC_BATTERY_HIGH_LATCH_MASK 0x80U
#define RTC_BATTERY_HIGH_LATCH_OFFSET 7
#define RTC_BATTERY_HIGH_LATCH_STOP_BIT 7
#define RTC_BATTERY_HIGH_LATCH_START_BIT 7
#define RTC_BATTERY_HIGH_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_4_REG 0xE23



//  
//  BackUpBat ball voltage goes below RtcBackupChg register.
//  Register LatchedInterrupt4 0x0E23, Bits 6:6, typedef rtc_battery_low_latch
//   
//  
#define RTC_BATTERY_LOW_LATCH_MASK 0x40U
#define RTC_BATTERY_LOW_LATCH_OFFSET 6
#define RTC_BATTERY_LOW_LATCH_STOP_BIT 6
#define RTC_BATTERY_LOW_LATCH_START_BIT 6
#define RTC_BATTERY_LOW_LATCH_WIDTH 1




//  
//  VbatA voltage goes above LowBat register programmed threshold.
//  Register LatchedInterrupt4 0x0E23, Bits 5:5, typedef high_battery_latch
//   
//  
#define HIGH_BATTERY_LATCH_MASK 0x20U
#define HIGH_BATTERY_LATCH_OFFSET 5
#define HIGH_BATTERY_LATCH_STOP_BIT 5
#define HIGH_BATTERY_LATCH_START_BIT 5
#define HIGH_BATTERY_LATCH_WIDTH 1




//  
//  VbatA voltage goes below LowBat register programmed threshold.
//  Register LatchedInterrupt4 0x0E23, Bits 4:4, typedef low_battery_latch
//   
//  
#define LOW_BATTERY_LATCH_MASK 0x10U
#define LOW_BATTERY_LATCH_OFFSET 4
#define LOW_BATTERY_LATCH_STOP_BIT 4
#define LOW_BATTERY_LATCH_START_BIT 4
#define LOW_BATTERY_LATCH_WIDTH 1




//  
//  Coulomb Counter has ended its calibration.
//  Register LatchedInterrupt4 0x0E23, Bits 3:3, typedef 
//  coulomb_calibration_endedd_latch
//   
//  
#define COULOMB_CALIBRATION_ENDEDD_LATCH_MASK 0x8U
#define COULOMB_CALIBRATION_ENDEDD_LATCH_OFFSET 3
#define COULOMB_CALIBRATION_ENDEDD_LATCH_STOP_BIT 3
#define COULOMB_CALIBRATION_ENDEDD_LATCH_START_BIT 3
#define COULOMB_CALIBRATION_ENDEDD_LATCH_WIDTH 1




//  
//  Coulomb Counter has ended data convertion.
//  Register LatchedInterrupt4 0x0E23, Bits 2:2, typedef coulomb_counter_ended_latch
//   
//  
#define COULOMB_ENDED_LATCH_MASK 0x4U
#define COULOMB_ENDED_LATCH_OFFSET 2
#define COULOMB_ENDED_LATCH_STOP_BIT 2
#define COULOMB_ENDED_LATCH_START_BIT 2
#define COULOMB_ENDED_LATCH_WIDTH 1




//  
//  Audio interrupt form audio digital part detected.
//  Register LatchedInterrupt4 0x0E23, Bits 1:1, typedef audio_latch
//   
//  
#define AUDIO_LATCH_MASK 0x2U
#define AUDIO_LATCH_OFFSET 1
#define AUDIO_LATCH_STOP_BIT 1
#define AUDIO_LATCH_START_BIT 1
#define AUDIO_LATCH_WIDTH 1




//  
//  Accumulation of N sample conversion is detected.
//  Register LatchedInterrupt4 0x0E23, Bits 0:0, typedef accumulation_conversion_latch
//   
//  
#define ACCUMULATION_CONVERSION_LATCH_MASK 0x1U
#define ACCUMULATION_CONVERSION_LATCH_OFFSET 0
#define ACCUMULATION_CONVERSION_LATCH_STOP_BIT 0
#define ACCUMULATION_CONVERSION_LATCH_START_BIT 0
#define ACCUMULATION_CONVERSION_LATCH_WIDTH 1




//  
//  GP ADC conversion requested thru Hardware control.
//  Register LatchedInterrupt5 0x0E24, Bits 7:7, typedef adc_hardware_control
//   
//  
#define ADC_HARDWARE_CONTROL_LATCH_MASK 0x80U
#define ADC_HARDWARE_CONTROL_LATCH_OFFSET 7
#define ADC_HARDWARE_CONTROL_LATCH_STOP_BIT 7
#define ADC_HARDWARE_CONTROL_LATCH_START_BIT 7
#define ADC_HARDWARE_CONTROL_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_5_REG 0xE24



//  
//  AccDetect2 ball voltage level goes above AccDetect21Th.
//  Register LatchedInterrupt5 0x0E24, Bits 6:6, typedef accessory21_high_voltage
//   
//  
#define ACCESSORY_21_HIGH_VOLTAGE_LATCH_MASK 0x40U
#define ACCESSORY_21_HIGH_VOLTAGE_LATCH_OFFSET 6
#define ACCESSORY_21_HIGH_VOLTAGE_LATCH_STOP_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_LATCH_START_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_LATCH_WIDTH 1




//  
//  AccDetect2 ball voltage level goes below AccDetect21Th.
//  Register LatchedInterrupt5 0x0E24, Bits 5:5, typedef accessory21_low_voltage_latch
//   
//  
#define ACCESSORY_21_LOW_VOLTAGE_LATCH_MASK 0x20U
#define ACCESSORY_21_LOW_VOLTAGE_LATCH_OFFSET 5
#define ACCESSORY_21_LOW_VOLTAGE_LATCH_STOP_BIT 5
#define ACCESSORY_21_LOW_VOLTAGE_LATCH_START_BIT 5
#define ACCESSORY_21_LOW_VOLTAGE_LATCH_WIDTH 1




//  
//  AccDetect2 ball voltage level goes above AccDetect22Th.
//  Register LatchedInterrupt5 0x0E24, Bits 4:4, typedef accessory22_high_voltage_latch
//   
//  
#define ACCESSORY_22_HIGH_VOLTAGE_LATCH_MASK 0x10U
#define ACCESSORY_22_HIGH_VOLTAGE_LATCH_OFFSET 4
#define ACCESSORY_22_HIGH_VOLTAGE_LATCH_STOP_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_LATCH_START_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_LATCH_WIDTH 1




//  
//  AccDetect2 ball voltage level goes below AccDetect22Th.
//  Register LatchedInterrupt5 0x0E24, Bits 3:3, typedef accessory22_low_voltage
//   
//  
#define ACCESSORY_22_LOW_VOLTAGE_LATCH_MASK 0x8U
#define ACCESSORY_22_LOW_VOLTAGE_LATCH_OFFSET 3
#define ACCESSORY_22_LOW_VOLTAGE_LATCH_STOP_BIT 3
#define ACCESSORY_22_LOW_VOLTAGE_LATCH_START_BIT 3
#define ACCESSORY_22_LOW_VOLTAGE_LATCH_WIDTH 1




//  
//  AccDetect1 ball voltage level goes above AccDetect1Th.
//  Register LatchedInterrupt5 0x0E24, Bits 2:2, typedef accessory1_high_voltage
//   
//  
#define ACCESSORY_1_HIGH_VOLTAGE_LATCH_MASK 0x4U
#define ACCESSORY_1_HIGH_VOLTAGE_LATCH_OFFSET 2
#define ACCESSORY_1_HIGH_VOLTAGE_LATCH_STOP_BIT 2
#define ACCESSORY_1_HIGH_VOLTAGE_LATCH_START_BIT 2
#define ACCESSORY_1_HIGH_VOLTAGE_LATCH_WIDTH 1




//  
//  AccDetect1 ball voltage level goes below AccDetect1Th.
//  Register LatchedInterrupt5 0x0E24, Bits 1:1, typedef accessory1_low_voltage_latch
//   
//  
#define ACCESSORY_1_LOW_VOLTAGE_LATCH_MASK 0x2U
#define ACCESSORY_1_LOW_VOLTAGE_LATCH_OFFSET 1
#define ACCESSORY_1_LOW_VOLTAGE_LATCH_STOP_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_LATCH_START_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_LATCH_WIDTH 1




//  
//  GP ADC conversion requested thru software control is ended (data are 
//  available).
//  Register LatchedInterrupt5 0x0E24, Bits 0:0, typedef adc_software_control
//   
//  
#define ADC_SOFTWARE_CONTROL_LATCH_MASK 0x1U
#define ADC_SOFTWARE_CONTROL_LATCH_OFFSET 0
#define ADC_SOFTWARE_CONTROL_LATCH_STOP_BIT 0
#define ADC_SOFTWARE_CONTROL_LATCH_START_BIT 0
#define ADC_SOFTWARE_CONTROL_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO6 ball.
//  Register LatchedInterrupt7 0x0E26, Bits 0:0, typedef gpio6_rising_latch_event
//   
//  
#define GPIO_6_RISING_LATCH_MASK 0x1U
#define GPIO_6_RISING_LATCH_OFFSET 0
#define GPIO_6_RISING_LATCH_STOP_BIT 0
#define GPIO_6_RISING_LATCH_START_BIT 0
#define GPIO_6_RISING_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_7_REG 0xE26



//  
//  Rising edge detected on GPIO7 ball.
//  Register LatchedInterrupt7 0x0E26, Bits 1:1, typedef gpio7_rising_latch_event
//   
//  
#define GPIO_7_RISING_LATCH_MASK 0x2U
#define GPIO_7_RISING_LATCH_OFFSET 1
#define GPIO_7_RISING_LATCH_STOP_BIT 1
#define GPIO_7_RISING_LATCH_START_BIT 1
#define GPIO_7_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO8 ball.
//  Register LatchedInterrupt7 0x0E26, Bits 2:2, typedef gpio8_rising_latch_event
//   
//  
#define GPIO_8_RISING_LATCH_MASK 0x4U
#define GPIO_8_RISING_LATCH_OFFSET 2
#define GPIO_8_RISING_LATCH_STOP_BIT 2
#define GPIO_8_RISING_LATCH_START_BIT 2
#define GPIO_8_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO9 ball.
//  Register LatchedInterrupt7 0x0E26, Bits 3:3, typedef gpio9_rising_latch_event
//   
//  
#define GPIO_9_RISING_LATCH_MASK 0x8U
#define GPIO_9_RISING_LATCH_OFFSET 3
#define GPIO_9_RISING_LATCH_STOP_BIT 3
#define GPIO_9_RISING_LATCH_START_BIT 3
#define GPIO_9_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO10 ball.
//  Register LatchedInterrupt7 0x0E26, Bits 4:4, typedef gpio10_rising_latch_event
//   
//  
#define GPIO_10_RISING_LATCH_MASK 0x10U
#define GPIO_10_RISING_LATCH_OFFSET 4
#define GPIO_10_RISING_LATCH_STOP_BIT 4
#define GPIO_10_RISING_LATCH_START_BIT 4
#define GPIO_10_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO11 ball.
//  Register LatchedInterrupt7 0x0E26, Bits 5:5, typedef gpio11_rising_latch_event
//   
//  
#define GPIO_11_RISING_LATCH_MASK 0x20U
#define GPIO_11_RISING_LATCH_OFFSET 5
#define GPIO_11_RISING_LATCH_STOP_BIT 5
#define GPIO_11_RISING_LATCH_START_BIT 5
#define GPIO_11_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO12 ball.
//  Register LatchedInterrupt7 0x0E26, Bits 6:6, typedef gpio12_rising_latch_event
//   
//  
#define GPIO_12_RISING_LATCH_MASK 0x40U
#define GPIO_12_RISING_LATCH_OFFSET 6
#define GPIO_12_RISING_LATCH_STOP_BIT 6
#define GPIO_12_RISING_LATCH_START_BIT 6
#define GPIO_12_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO13 ball.
//  Register LatchedInterrupt7 0x0E26, Bits 7:7, typedef gpio13_rising_latch_event
//   
//  
#define GPIO_13_RISING_LATCH_MASK 0x80U
#define GPIO_13_RISING_LATCH_OFFSET 7
#define GPIO_13_RISING_LATCH_STOP_BIT 7
#define GPIO_13_RISING_LATCH_START_BIT 7
#define GPIO_13_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO24 ball.
//  Register LatchedInterrupt8 0x0E27, Bits 0:0, typedef gpio24_rising_latch_event
//   
//  
#define GPIO_24_RISING_LATCH_MASK 0x1U
#define GPIO_24_RISING_LATCH_OFFSET 0
#define GPIO_24_RISING_LATCH_STOP_BIT 0
#define GPIO_24_RISING_LATCH_START_BIT 0
#define GPIO_24_RISING_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_8_REG 0xE27



//  
//  Rising edge detected on GPIO25 ball.
//  Register LatchedInterrupt8 0x0E27, Bits 1:1, typedef gpio25_rising_latch_event
//   
//  
#define GPIO_25_RISING_LATCH_MASK 0x2U
#define GPIO_25_RISING_LATCH_OFFSET 1
#define GPIO_25_RISING_LATCH_STOP_BIT 1
#define GPIO_25_RISING_LATCH_START_BIT 1
#define GPIO_25_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO36 ball.
//  Register LatchedInterrupt8 0x0E27, Bits 2:2, typedef gpio36_rising_latch_event
//   
//  
#define GPIO_36_RISING_LATCH_MASK 0x4U
#define GPIO_36_RISING_LATCH_OFFSET 2
#define GPIO_36_RISING_LATCH_STOP_BIT 2
#define GPIO_36_RISING_LATCH_START_BIT 2
#define GPIO_36_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO37 ball.
//  Register LatchedInterrupt8 0x0E27, Bits 3:3, typedef gpio37_rising_latch_event
//   
//  
#define GPIO_37_RISING_LATCH_MASK 0x8U
#define GPIO_37_RISING_LATCH_OFFSET 3
#define GPIO_37_RISING_LATCH_STOP_BIT 3
#define GPIO_37_RISING_LATCH_START_BIT 3
#define GPIO_37_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO38 ball.
//  Register LatchedInterrupt8 0x0E27, Bits 4:4, typedef gpio38_rising_latch_event
//   
//  
#define GPIO_38_RISING_LATCH_MASK 0x10U
#define GPIO_38_RISING_LATCH_OFFSET 4
#define GPIO_38_RISING_LATCH_STOP_BIT 4
#define GPIO_38_RISING_LATCH_START_BIT 4
#define GPIO_38_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO39 ball.
//  Register LatchedInterrupt8 0x0E27, Bits 5:5, typedef gpio39_rising_latch_event
//   
//  
#define GPIO_39_RISING_LATCH_MASK 0x20U
#define GPIO_39_RISING_LATCH_OFFSET 5
#define GPIO_39_RISING_LATCH_STOP_BIT 5
#define GPIO_39_RISING_LATCH_START_BIT 5
#define GPIO_39_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO40 ball.
//  Register LatchedInterrupt8 0x0E27, Bits 6:6, typedef gpio40_rising_latch_event
//   
//  
#define GPIO_40_RISING_LATCH_MASK 0x40U
#define GPIO_40_RISING_LATCH_OFFSET 6
#define GPIO_40_RISING_LATCH_STOP_BIT 6
#define GPIO_40_RISING_LATCH_START_BIT 6
#define GPIO_40_RISING_LATCH_WIDTH 1




//  
//  Rising edge detected on GPIO41 ball.
//  Register LatchedInterrupt8 0x0E27, Bits 7:7, typedef gpio41_rising_latch_event
//   
//  
#define GPIO_41_RISING_LATCH_MASK 0x80U
#define GPIO_41_RISING_LATCH_OFFSET 7
#define GPIO_41_RISING_LATCH_STOP_BIT 7
#define GPIO_41_RISING_LATCH_START_BIT 7
#define GPIO_41_RISING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO6 ball.
//  Register LatchedInterrupt9 0x0E28, Bits 0:0, typedef gpio6_falling_latch_event
//   
//  
#define GPIO_6_FALLING_LATCH_MASK 0x1U
#define GPIO_6_FALLING_LATCH_OFFSET 0
#define GPIO_6_FALLING_LATCH_STOP_BIT 0
#define GPIO_6_FALLING_LATCH_START_BIT 0
#define GPIO_6_FALLING_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_9_REG 0xE28



//  
//  Falling edge detected on GPIO7 ball.
//  Register LatchedInterrupt9 0x0E28, Bits 1:1, typedef gpio7_falling_latch_event
//   
//  
#define GPIO_7_FALLING_LATCH_MASK 0x2U
#define GPIO_7_FALLING_LATCH_OFFSET 1
#define GPIO_7_FALLING_LATCH_STOP_BIT 1
#define GPIO_7_FALLING_LATCH_START_BIT 1
#define GPIO_7_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO8 ball.
//  Register LatchedInterrupt9 0x0E28, Bits 2:2, typedef gpio8_falling_latch_event
//   
//  
#define GPIO_8_FALLING_LATCH_MASK 0x4U
#define GPIO_8_FALLING_LATCH_OFFSET 2
#define GPIO_8_FALLING_LATCH_STOP_BIT 2
#define GPIO_8_FALLING_LATCH_START_BIT 2
#define GPIO_8_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO9 ball.
//  Register LatchedInterrupt9 0x0E28, Bits 3:3, typedef gpio9_falling_latch_event
//   
//  
#define GPIO_9_FALLING_LATCH_MASK 0x8U
#define GPIO_9_FALLING_LATCH_OFFSET 3
#define GPIO_9_FALLING_LATCH_STOP_BIT 3
#define GPIO_9_FALLING_LATCH_START_BIT 3
#define GPIO_9_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO10 ball.
//  Register LatchedInterrupt9 0x0E28, Bits 4:4, typedef gpio10_falling_latch_event
//   
//  
#define GPIO_10_FALLING_LATCH_MASK 0x10U
#define GPIO_10_FALLING_LATCH_OFFSET 4
#define GPIO_10_FALLING_LATCH_STOP_BIT 4
#define GPIO_10_FALLING_LATCH_START_BIT 4
#define GPIO_10_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO11 ball.
//  Register LatchedInterrupt9 0x0E28, Bits 5:5, typedef gpio11_falling_latch_event
//   
//  
#define GPIO_11_FALLING_LATCH_MASK 0x20U
#define GPIO_11_FALLING_LATCH_OFFSET 5
#define GPIO_11_FALLING_LATCH_STOP_BIT 5
#define GPIO_11_FALLING_LATCH_START_BIT 5
#define GPIO_11_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO12 ball.
//  Register LatchedInterrupt9 0x0E28, Bits 6:6, typedef gpio12_falling_latch_event
//   
//  
#define GPIO_12_FALLING_LATCH_MASK 0x40U
#define GPIO_12_FALLING_LATCH_OFFSET 6
#define GPIO_12_FALLING_LATCH_STOP_BIT 6
#define GPIO_12_FALLING_LATCH_START_BIT 6
#define GPIO_12_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO13 ball.
//  Register LatchedInterrupt9 0x0E28, Bits 7:7, typedef gpio13_falling_latch_event
//   
//  
#define GPIO_13_FALLING_LATCH_MASK 0x80U
#define GPIO_13_FALLING_LATCH_OFFSET 7
#define GPIO_13_FALLING_LATCH_STOP_BIT 7
#define GPIO_13_FALLING_LATCH_START_BIT 7
#define GPIO_13_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO24 ball.
//  Register LatchedInterrupt10 0x0E29, Bits 0:0, typedef gpio24_falling_latch_event
//   
//  
#define GPIO_24_FALLING_LATCH_MASK 0x1U
#define GPIO_24_FALLING_LATCH_OFFSET 0
#define GPIO_24_FALLING_LATCH_STOP_BIT 0
#define GPIO_24_FALLING_LATCH_START_BIT 0
#define GPIO_24_FALLING_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_10_REG 0xE29



//  
//  Falling edge detected on GPIO25 ball.
//  Register LatchedInterrupt10 0x0E29, Bits 1:1, typedef gpio25_falling_latch_event
//   
//  
#define GPIO_25_FALLING_LATCH_MASK 0x2U
#define GPIO_25_FALLING_LATCH_OFFSET 1
#define GPIO_25_FALLING_LATCH_STOP_BIT 1
#define GPIO_25_FALLING_LATCH_START_BIT 1
#define GPIO_25_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO36 ball.
//  Register LatchedInterrupt10 0x0E29, Bits 2:2, typedef gpio36_falling_latch_event
//   
//  
#define GPIO_36_FALLING_LATCH_MASK 0x4U
#define GPIO_36_FALLING_LATCH_OFFSET 2
#define GPIO_36_FALLING_LATCH_STOP_BIT 2
#define GPIO_36_FALLING_LATCH_START_BIT 2
#define GPIO_36_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO37 ball.
//  Register LatchedInterrupt10 0x0E29, Bits 3:3, typedef gpio37_falling_latch_event
//   
//  
#define GPIO_37_FALLING_LATCH_MASK 0x8U
#define GPIO_37_FALLING_LATCH_OFFSET 3
#define GPIO_37_FALLING_LATCH_STOP_BIT 3
#define GPIO_37_FALLING_LATCH_START_BIT 3
#define GPIO_37_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO38 ball.
//  Register LatchedInterrupt10 0x0E29, Bits 4:4, typedef gpio38_falling_latch_event
//   
//  
#define GPIO_38_FALLING_LATCH_MASK 0x10U
#define GPIO_38_FALLING_LATCH_OFFSET 4
#define GPIO_38_FALLING_LATCH_STOP_BIT 4
#define GPIO_38_FALLING_LATCH_START_BIT 4
#define GPIO_38_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO39 ball.
//  Register LatchedInterrupt10 0x0E29, Bits 5:5, typedef gpio39_falling_latch_event
//   
//  
#define GPIO_39_FALLING_LATCH_MASK 0x20U
#define GPIO_39_FALLING_LATCH_OFFSET 5
#define GPIO_39_FALLING_LATCH_STOP_BIT 5
#define GPIO_39_FALLING_LATCH_START_BIT 5
#define GPIO_39_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO40 ball.
//  Register LatchedInterrupt10 0x0E29, Bits 6:6, typedef gpio40_falling_latch_event
//   
//  
#define GPIO_40_FALLING_LATCH_MASK 0x40U
#define GPIO_40_FALLING_LATCH_OFFSET 6
#define GPIO_40_FALLING_LATCH_STOP_BIT 6
#define GPIO_40_FALLING_LATCH_START_BIT 6
#define GPIO_40_FALLING_LATCH_WIDTH 1




//  
//  Falling edge detected on GPIO41 ball.
//  Register LatchedInterrupt10 0x0E29, Bits 7:7, typedef gpio41_falling_latch_event
//   
//  
#define GPIO_41_FALLING_LATCH_MASK 0x80U
#define GPIO_41_FALLING_LATCH_OFFSET 7
#define GPIO_41_FALLING_LATCH_STOP_BIT 7
#define GPIO_41_FALLING_LATCH_START_BIT 7
#define GPIO_41_FALLING_LATCH_WIDTH 1




//  
//  0: inactive 
//  1: no detection after inactive no detection after 2046*32Khz clock cycles (about 
//  62ms).
//  Register LatchedInterrupt12 0x0E2B, Bits 0:0, typedef adp_source_error_latch_event
//   
//  
#define ADP_SOURCE_ERROR_LATCH_MASK 0x1U
#define ADP_SOURCE_ERROR_LATCH_OFFSET 0
#define ADP_SOURCE_ERROR_LATCH_STOP_BIT 0
#define ADP_SOURCE_ERROR_LATCH_START_BIT 0
#define ADP_SOURCE_ERROR_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_12_REG 0xE2B



//  
//  0: inactive 
//  1: no detection after 2046*32Khz clock cycles (about 62ms).
//  Register LatchedInterrupt12 0x0E2B, Bits 1:1, typedef adp_sink_error_latch_event
//   
//  
#define ADP_SINK_ERROR_LATCH_MASK 0x2U
#define ADP_SINK_ERROR_LATCH_OFFSET 1
#define ADP_SINK_ERROR_LATCH_STOP_BIT 1
#define ADP_SINK_ERROR_LATCH_START_BIT 1
#define ADP_SINK_ERROR_LATCH_WIDTH 1




//  
//  0: previous state 
//  1: accessory plug on Vbus.
//  Register LatchedInterrupt12 0x0E2B, Bits 2:2, typedef adp_probe_plug_latch_event
//   
//  
#define ADP_PROBE_PLUG_LATCH_MASK 0x4U
#define ADP_PROBE_PLUG_LATCH_OFFSET 2
#define ADP_PROBE_PLUG_LATCH_STOP_BIT 2
#define ADP_PROBE_PLUG_LATCH_START_BIT 2
#define ADP_PROBE_PLUG_LATCH_WIDTH 1




//  
//  0: previous state 
//  1: accessory unplug on Vbus.
//  Register LatchedInterrupt12 0x0E2B, Bits 3:3, typedef adp_probe_un_plug_latch_event
//   
//  
#define ADP_PROBE_UN_PLUG_LATCH_MASK 0x8U
#define ADP_PROBE_UN_PLUG_LATCH_OFFSET 3
#define ADP_PROBE_UN_PLUG_LATCH_STOP_BIT 3
#define ADP_PROBE_UN_PLUG_LATCH_START_BIT 3
#define ADP_PROBE_UN_PLUG_LATCH_WIDTH 1




//  
//  0: Current source Off 
//  1: Current source On.
//  Register LatchedInterrupt12 0x0E2B, Bits 4:4, typedef current_source_on_latch_event
//   
//  
#define ADP_SENSE_OFF_LATCH_MASK 0x10U
#define ADP_SENSE_OFF_LATCH_OFFSET 4
#define ADP_SENSE_OFF_LATCH_STOP_BIT 4
#define ADP_SENSE_OFF_LATCH_START_BIT 4
#define ADP_SENSE_OFF_LATCH_WIDTH 1




//  
//  0: inactive 
//  1: USB PHY has been reset due to a power loss.
//  Register LatchedInterrupt12 0x0E2B, Bits 6:6, typedef usb_phy_power_error
//   
//  
#define USB_PHY_POWER_ERROR_MASK 0x40U
#define USB_PHY_POWER_ERROR_OFFSET 6
#define USB_PHY_POWER_ERROR_STOP_BIT 6
#define USB_PHY_POWER_ERROR_START_BIT 6
#define USB_PHY_POWER_ERROR_WIDTH 1




//  
//  0: UsbLink status unchanged 
//  1: UsbLink status changed.
//  Register LatchedInterrupt12 0x0E2B, Bits 7:7, typedef 
//  usb_link_status_changed_latch_event
//   
//  
#define USB_LINK_STATUS_LATCH_MASK 0x80U
#define USB_LINK_STATUS_LATCH_OFFSET 7
#define USB_LINK_STATUS_LATCH_STOP_BIT 7
#define USB_LINK_STATUS_LATCH_START_BIT 7
#define USB_LINK_STATUS_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Rising edge detected on GPIO50 ball.
//  Register LatchedInterrupt13 0x0E2C, Bits 1:1, typedef gpio50_rising_latch_event
//   
//  
#define GPIO_50_RISING_LATCH_MASK 0x2U
#define GPIO_50_RISING_LATCH_OFFSET 1
#define GPIO_50_RISING_LATCH_STOP_BIT 1
#define GPIO_50_RISING_LATCH_START_BIT 1
#define GPIO_50_RISING_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_13_REG 0xE2C



//  
//  0: Inactive 
//  1: Rising edge detected on GPIO51 ball.
//  Register LatchedInterrupt13 0x0E2C, Bits 2:2, typedef gpio51_rising_latch_event
//   
//  
#define GPIO_51_RISING_LATCH_MASK 0x4U
#define GPIO_51_RISING_LATCH_OFFSET 2
#define GPIO_51_RISING_LATCH_STOP_BIT 2
#define GPIO_51_RISING_LATCH_START_BIT 2
#define GPIO_51_RISING_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Rising edge detected on GPIO52 ball.
//  Register LatchedInterrupt13 0x0E2C, Bits 3:3, typedef gpio52_rising_latch_event
//   
//  
#define GPIO_52_RISING_LATCH_MASK 0x8U
#define GPIO_52_RISING_LATCH_OFFSET 3
#define GPIO_52_RISING_LATCH_STOP_BIT 3
#define GPIO_52_RISING_LATCH_START_BIT 3
#define GPIO_52_RISING_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Rising edge detected on GPIO53 ball.
//  Register LatchedInterrupt13 0x0E2C, Bits 4:4, typedef gpio53_rising_latch_event
//   
//  
#define GPIO_53_RISING_LATCH_MASK 0x10U
#define GPIO_53_RISING_LATCH_OFFSET 4
#define GPIO_53_RISING_LATCH_STOP_BIT 4
#define GPIO_53_RISING_LATCH_START_BIT 4
#define GPIO_53_RISING_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Rising edge detected on GPIO60 ball.
//  Register LatchedInterrupt13 0x0E2C, Bits 5:5, typedef gpio60_rising_latch_event
//   
//  
#define GPIO_60_RISING_LATCH_MASK 0x20U
#define GPIO_60_RISING_LATCH_OFFSET 5
#define GPIO_60_RISING_LATCH_STOP_BIT 5
#define GPIO_60_RISING_LATCH_START_BIT 5
#define GPIO_60_RISING_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Rising edge detected on IEXTCHRFBN ball.
//  Register LatchedInterrupt13 0x0E2C, Bits 6:6, typedef 
//  rising_edge_external_charger_event
//   
//  
#define RISING_EDGE_EXTERNAL_CHARGER_LATCH_MASK 0x40U
#define RISING_EDGE_EXTERNAL_CHARGER_LATCH_OFFSET 6
#define RISING_EDGE_EXTERNAL_CHARGER_LATCH_STOP_BIT 6
#define RISING_EDGE_EXTERNAL_CHARGER_LATCH_START_BIT 6
#define RISING_EDGE_EXTERNAL_CHARGER_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Falling edge detected on GPIO50 ball.
//  Register LatchedInterrupt14 0x0E2D, Bits 1:1, typedef gpio50_falling_latch_event
//   
//  
#define GPIO_50_FALLING_LATCH_MASK 0x2U
#define GPIO_50_FALLING_LATCH_OFFSET 1
#define GPIO_50_FALLING_LATCH_STOP_BIT 1
#define GPIO_50_FALLING_LATCH_START_BIT 1
#define GPIO_50_FALLING_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_14_REG 0xE2D



//  
//  0: Inactive 
//  1: Falling edge detected on GPIO51 ball.
//  Register LatchedInterrupt14 0x0E2D, Bits 2:2, typedef gpio51_falling_latch_event
//   
//  
#define GPIO_51_FALLING_LATCH_MASK 0x4U
#define GPIO_51_FALLING_LATCH_OFFSET 2
#define GPIO_51_FALLING_LATCH_STOP_BIT 2
#define GPIO_51_FALLING_LATCH_START_BIT 2
#define GPIO_51_FALLING_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Falling edge detected on GPIO52 ball.
//  Register LatchedInterrupt14 0x0E2D, Bits 3:3, typedef gpio52_falling_latch_event
//   
//  
#define GPIO_52_FALLING_LATCH_MASK 0x8U
#define GPIO_52_FALLING_LATCH_OFFSET 3
#define GPIO_52_FALLING_LATCH_STOP_BIT 3
#define GPIO_52_FALLING_LATCH_START_BIT 3
#define GPIO_52_FALLING_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Falling edge detected on GPIO53 ball.
//  Register LatchedInterrupt14 0x0E2D, Bits 4:4, typedef gpio53_falling_latch_event
//   
//  
#define GPIO_53_FALLING_LATCH_MASK 0x10U
#define GPIO_53_FALLING_LATCH_OFFSET 4
#define GPIO_53_FALLING_LATCH_STOP_BIT 4
#define GPIO_53_FALLING_LATCH_START_BIT 4
#define GPIO_53_FALLING_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Falling edge detected on GPIO60 ball.
//  Register LatchedInterrupt14 0x0E2D, Bits 5:5, typedef gpio60_falling_latch_event
//   
//  
#define GPIO_60_FALLING_LATCH_MASK 0x20U
#define GPIO_60_FALLING_LATCH_OFFSET 5
#define GPIO_60_FALLING_LATCH_STOP_BIT 5
#define GPIO_60_FALLING_LATCH_START_BIT 5
#define GPIO_60_FALLING_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: Rising edge detected on IEXTCHRFBN ball.
//  Register LatchedInterrupt14 0x0E2D, Bits 6:6, typedef 
//  falling_edge_external_charger_event
//   
//  
#define FALLING_EDGE_EXTERNAL_CHARGER_LATCH_MASK 0x40U
#define FALLING_EDGE_EXTERNAL_CHARGER_LATCH_OFFSET 6
#define FALLING_EDGE_EXTERNAL_CHARGER_LATCH_STOP_BIT 6
#define FALLING_EDGE_EXTERNAL_CHARGER_LATCH_START_BIT 6
#define FALLING_EDGE_EXTERNAL_CHARGER_LATCH_WIDTH 1




//  
//  0: Btemp greater BtempLow 
//  1: BtempLow greater Btemp.
//  Register LatchedSource19 0x0E32, Bits 0:0, typedef battery_temperature_low_latch
//   
//  
#define BATTERY_TEMPERATURE_LOW_LATCH_MASK 0x1U
#define BATTERY_TEMPERATURE_LOW_LATCH_OFFSET 0
#define BATTERY_TEMPERATURE_LOW_LATCH_STOP_BIT 0
#define BATTERY_TEMPERATURE_LOW_LATCH_START_BIT 0
#define BATTERY_TEMPERATURE_LOW_LATCH_WIDTH 1

#define LATCHED_SOURCE_19_REG 0xE32



//  
//  0: Btemp lower BtempLow or Btemp greater BtempMedium 
//  1: BtempMedium greater Btemp greater BtempLow.
//  Register LatchedSource19 0x0E32, Bits 1:1, typedef 
//  battery_temperature_low_medium_latch
//   
//  
#define BATTERY_TEMPERATURE_LOW_MEDIUM_LATCH_MASK 0x2U
#define BATTERY_TEMPERATURE_LOW_MEDIUM_LATCH_OFFSET 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_LATCH_STOP_BIT 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_LATCH_START_BIT 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_LATCH_WIDTH 1




//  
//  0: Btemp lower BtempMedium or Btemp greater BtempHigh 
//  1: BtempHigh greater Btemp greater BtempMedium.
//  Register LatchedSource19 0x0E32, Bits 2:2, typedef 
//  battery_temperature_medium_high_latch
//   
//  
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_LATCH_MASK 0x4U
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_LATCH_OFFSET 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_LATCH_STOP_BIT 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_LATCH_START_BIT 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_LATCH_WIDTH 1




//  
//  0: Btemp lower BtempHigh 
//  1: Btemp greater BtempHigh.
//  Register LatchedSource19 0x0E32, Bits 3:3, typedef battery_temperature_high_latch
//   
//  
#define BATTERY_TEMPERATURE_HIGH_LATCH_MASK 0x8U
#define BATTERY_TEMPERATURE_HIGH_LATCH_OFFSET 3
#define BATTERY_TEMPERATURE_HIGH_LATCH_STOP_BIT 3
#define BATTERY_TEMPERATURE_HIGH_LATCH_START_BIT 3
#define BATTERY_TEMPERATURE_HIGH_LATCH_WIDTH 1




//  
//  ID resistance greater than 220k detected.
//  Register LatchedInterrupt20 0x0E33, Bits 7:7, typedef id_detection_greater220k_latch
//   
//  
#define ID_DETECTION_GREATER_22_0K_LATCH_MASK 0x80U
#define ID_DETECTION_GREATER_22_0K_LATCH_OFFSET 7
#define ID_DETECTION_GREATER_22_0K_LATCH_STOP_BIT 7
#define ID_DETECTION_GREATER_22_0K_LATCH_START_BIT 7
#define ID_DETECTION_GREATER_22_0K_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_20_REG 0xE33



//  
//  ID resistance greater than 125k detected.
//  Register LatchedInterrupt20 0x0E33, Bits 6:6, typedef id_detection_greater125k_latch
//   
//  
#define ID_DETECTION_GREATER_12_5K_LATCH_MASK 0x40U
#define ID_DETECTION_GREATER_12_5K_LATCH_OFFSET 6
#define ID_DETECTION_GREATER_12_5K_LATCH_STOP_BIT 6
#define ID_DETECTION_GREATER_12_5K_LATCH_START_BIT 6
#define ID_DETECTION_GREATER_12_5K_LATCH_WIDTH 1




//  
//  ID resistance greater than 69k detected.
//  Register LatchedInterrupt20 0x0E33, Bits 5:5, typedef id_detection_greater69k_latch
//   
//  
#define ID_DETECTION_GREATER_6_9K_LATCH_MASK 0x20U
#define ID_DETECTION_GREATER_6_9K_LATCH_OFFSET 5
#define ID_DETECTION_GREATER_6_9K_LATCH_STOP_BIT 5
#define ID_DETECTION_GREATER_6_9K_LATCH_START_BIT 5
#define ID_DETECTION_GREATER_6_9K_LATCH_WIDTH 1




//  
//  ID resistance greater than 37k detected.
//  Register LatchedInterrupt20 0x0E33, Bits 4:4, typedef id_detection_greater37k_latch
//   
//  
#define ID_DETECTION_GREATER_3_7K_LATCH_MASK 0x10U
#define ID_DETECTION_GREATER_3_7K_LATCH_OFFSET 4
#define ID_DETECTION_GREATER_3_7K_LATCH_STOP_BIT 4
#define ID_DETECTION_GREATER_3_7K_LATCH_START_BIT 4
#define ID_DETECTION_GREATER_3_7K_LATCH_WIDTH 1




//  
//  Rising edge detected on ID.
//  Register LatchedInterrupt20 0x0E33, Bits 2:2, typedef id_up_rising_event_latch
//   
//  
#define ID_UP_RISING_EVENT_LATCH_MASK 0x4U
#define ID_UP_RISING_EVENT_LATCH_OFFSET 2
#define ID_UP_RISING_EVENT_LATCH_STOP_BIT 2
#define ID_UP_RISING_EVENT_LATCH_START_BIT 2
#define ID_UP_RISING_EVENT_LATCH_WIDTH 1




//  
//  not allowed USB charger detected on Vbus ball.
//  Register LatchedInterrupt20 0x0E33, Bits 1:1, typedef usb_bad_charger_plug_latch
//   
//  
#define USB_CHARGER_PLUG_LATCH_MASK 0x2U
#define USB_CHARGER_PLUG_LATCH_OFFSET 1
#define USB_CHARGER_PLUG_LATCH_STOP_BIT 1
#define USB_CHARGER_PLUG_LATCH_START_BIT 1
#define USB_CHARGER_PLUG_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: SRP detected.
//  Register LatchedInterrupt20 0x0E33, Bits 0:0, typedef srpd_dected_latch
//   
//  
#define SRPD_DECTED_MASK 0x1U
#define SRPD_DECTED_OFFSET 0
#define SRPD_DECTED_STOP_BIT 0
#define SRPD_DECTED_START_BIT 0
#define SRPD_DECTED_WIDTH 1




//  
//  ID resistance lower than 220k detected.
//  Register LatchedInterrupt21 0x0E34, Bits 5:5, typedef id_detection220k_latch
//   
//  
#define ID_DETECTION_22_0K_LATCH_MASK 0x20U
#define ID_DETECTION_22_0K_LATCH_OFFSET 5
#define ID_DETECTION_22_0K_LATCH_STOP_BIT 5
#define ID_DETECTION_22_0K_LATCH_START_BIT 5
#define ID_DETECTION_22_0K_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_21_REG 0xE34



//  
//  ID resistance lower than 125k detected.
//  Register LatchedInterrupt21 0x0E34, Bits 4:4, typedef id_detection125k_latch
//   
//  
#define ID_DETECTION_12_5K_LATCH_MASK 0x10U
#define ID_DETECTION_12_5K_LATCH_OFFSET 4
#define ID_DETECTION_12_5K_LATCH_STOP_BIT 4
#define ID_DETECTION_12_5K_LATCH_START_BIT 4
#define ID_DETECTION_12_5K_LATCH_WIDTH 1




//  
//  ID resistance lower than 69k detected.
//  Register LatchedInterrupt21 0x0E34, Bits 3:3, typedef id_detection69k_latch
//   
//  
#define ID_DETECTION_6_9K_LATCH_MASK 0x8U
#define ID_DETECTION_6_9K_LATCH_OFFSET 3
#define ID_DETECTION_6_9K_LATCH_STOP_BIT 3
#define ID_DETECTION_6_9K_LATCH_START_BIT 3
#define ID_DETECTION_6_9K_LATCH_WIDTH 1




//  
//  ID resistance lower than 37k detected.
//  Register LatchedInterrupt21 0x0E34, Bits 2:2, typedef id_detection37k_latch
//   
//  
#define ID_DETECTION_3_7K_LATCH_MASK 0x4U
#define ID_DETECTION_3_7K_LATCH_OFFSET 2
#define ID_DETECTION_3_7K_LATCH_STOP_BIT 2
#define ID_DETECTION_3_7K_LATCH_START_BIT 2
#define ID_DETECTION_3_7K_LATCH_WIDTH 1




//  
//  Falling edge detected on ID.
//  Register LatchedInterrupt21 0x0E34, Bits 0:0, typedef id_wake_up_falling_event_latch
//   
//  
#define ID_WAKE_UP_FALLING_LATCH_MASK 0x1U
#define ID_WAKE_UP_FALLING_LATCH_OFFSET 0
#define ID_WAKE_UP_FALLING_LATCH_STOP_BIT 0
#define ID_WAKE_UP_FALLING_LATCH_START_BIT 0
#define ID_WAKE_UP_FALLING_LATCH_WIDTH 1




//  
//  0: Die temperature is above than usb charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than usb charger thermal protection threshold (charge 
//  stopped).
//  Register LatchedInterrupt22 0x0E35, Bits 0:0, typedef usb_charger_thermal_below
//   
//  
#define USB_CHARGER_THERMAL_BELOW_LATCH_MASK 0x1U
#define USB_CHARGER_THERMAL_BELOW_LATCH_OFFSET 0
#define USB_CHARGER_THERMAL_BELOW_LATCH_STOP_BIT 0
#define USB_CHARGER_THERMAL_BELOW_LATCH_START_BIT 0
#define USB_CHARGER_THERMAL_BELOW_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_22_REG 0xE35



//  
//  0: Die temperature is above than usb charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than usb charger thermal protection threshold (charge 
//  stopped).
//  Register LatchedInterrupt22 0x0E35, Bits 1:1, typedef usb_charger_thermal_above
//   
//  
#define USB_CHARGER_THERMAL_ABOVE_LATCH_MASK 0x2U
#define USB_CHARGER_THERMAL_ABOVE_LATCH_OFFSET 1
#define USB_CHARGER_THERMAL_ABOVE_LATCH_STOP_BIT 1
#define USB_CHARGER_THERMAL_ABOVE_LATCH_START_BIT 1
#define USB_CHARGER_THERMAL_ABOVE_LATCH_WIDTH 1




//  
//  0: inactive 
//  1: Charging current is no more limited (USB PHY is not in HS or Chirp 
//  modes).
//  Register LatchedInterrupt22 0x0E35, Bits 5:5, typedef 
//  charging_current_no_more_limited_latch
//   
//  
#define CHARGING_CURRENT_NO_MORE_LIMITED_LATCH_MASK 0x20U
#define CHARGING_CURRENT_NO_MORE_LIMITED_LATCH_OFFSET 5
#define CHARGING_CURRENT_NO_MORE_LIMITED_LATCH_STOP_BIT 5
#define CHARGING_CURRENT_NO_MORE_LIMITED_LATCH_START_BIT 5
#define CHARGING_CURRENT_NO_MORE_LIMITED_LATCH_WIDTH 1




//  
//  0: Inactive 
//  1: In USB HS or Chirp modes, charging current is limited to a setting defined in 
//  registers..
//  Register LatchedInterrupt22 0x0E35, Bits 6:6, typedef 
//  charging_current_limited_h_s_chirp_latch
//   
//  
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_LATCH_MASK 0x40U
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_LATCH_OFFSET 6
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_LATCH_STOP_BIT 6
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_LATCH_START_BIT 6
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_LATCH_WIDTH 1




//  
//  .
//  Register LatchedInterrupt25 0x0E38, Bits 0:0, typedef key_stuck_event_latch
//   
//  
#define KEY_STUCK_LATCH_MASK 0x1U
#define KEY_STUCK_LATCH_OFFSET 0
#define KEY_STUCK_LATCH_STOP_BIT 0
#define KEY_STUCK_LATCH_START_BIT 0
#define KEY_STUCK_LATCH_WIDTH 1

#define LATCHED_INTERRUPT_25_REG 0xE38



//  
//  .
//  Register LatchedInterrupt25 0x0E38, Bits 1:1, typedef lpr_vdddig_event_latch
//   
//  
#define LPR_VDDDIG_LATCH_MASK 0x2U
#define LPR_VDDDIG_LATCH_OFFSET 1
#define LPR_VDDDIG_LATCH_STOP_BIT 1
#define LPR_VDDDIG_LATCH_START_BIT 1
#define LPR_VDDDIG_LATCH_WIDTH 1




//  
//  .
//  Register LatchedInterrupt25 0x0E38, Bits 2:2, typedef l_kp_vdddig_event_latch
//   
//  
#define L_KP_VDDDIG_LATCH_MASK 0x4U
#define L_KP_VDDDIG_LATCH_OFFSET 2
#define L_KP_VDDDIG_LATCH_STOP_BIT 2
#define L_KP_VDDDIG_LATCH_START_BIT 2
#define L_KP_VDDDIG_LATCH_WIDTH 1




//  
//  .
//  Register LatchedInterrupt25 0x0E38, Bits 3:3, typedef kp_vdddig_latch
//   
//  
#define KP_VDDDIG_LATCH_MASK 0x8U
#define KP_VDDDIG_LATCH_OFFSET 3
#define KP_VDDDIG_LATCH_STOP_BIT 3
#define KP_VDDDIG_LATCH_START_BIT 3
#define KP_VDDDIG_LATCH_WIDTH 1




//  
//  .
//  Register LatchedInterrupt25 0x0E38, Bits 4:4, typedef key_deglitch_latch
//   
//  
#define KEY_DEGLITCH_LATCH_MASK 0x10U
#define KEY_DEGLITCH_LATCH_OFFSET 4
#define KEY_DEGLITCH_LATCH_STOP_BIT 4
#define KEY_DEGLITCH_LATCH_START_BIT 4
#define KEY_DEGLITCH_LATCH_WIDTH 1




//  
//  .
//  Register LatchedInterrupt25 0x0E38, Bits 6:7, typedef modem_power_status_latch
//   
//  
#define MODEM_POWER_STATUS_LATCH_MASK 0xC0U
#define MODEM_POWER_STATUS_LATCH_OFFSET 7
#define MODEM_POWER_STATUS_LATCH_STOP_BIT 7
#define MODEM_POWER_STATUS_LATCH_START_BIT 6
#define MODEM_POWER_STATUS_LATCH_WIDTH 2



#endif
