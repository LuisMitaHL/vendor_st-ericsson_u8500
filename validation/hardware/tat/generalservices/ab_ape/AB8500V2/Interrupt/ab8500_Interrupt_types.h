/****************************************************************************
 * File : c:/Lisp/Production/ab8500_V2/ab8500_Interrupt/Linux/ab8500_Interrupt_types.h
 * 
 *
 * Generated on the 09/09/2011 09:23 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - STEricsson 
 * with the help of : 
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : UM0836 User manual, AB8500_V2 Mixed signal multimedia and power management Rev 1
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_V2_AB8500_INTERRUPT_LINUX_AB8500_INTERRUPT_TYPES_H
#define C__LISP_PRODUCTION_AB8500_V2_AB8500_INTERRUPT_LINUX_AB8500_INTERRUPT_TYPES_H



#define INTERRUPT_SOURCE_1_REG 0xE00

//  
//  Main charger connected is not an allowed one.
//  Register InterruptSource1 0x0E00, Bits 0:0, typedef main_charger_allowed_interrupt
//   
//  
#define MAIN_CHARGER_ALLOWED_MASK 0x1
#define MAIN_CHARGER_ALLOWED_OFFSET 0
#define MAIN_CHARGER_ALLOWED_STOP_BIT 0
#define MAIN_CHARGER_ALLOWED_START_BIT 0
#define MAIN_CHARGER_ALLOWED_WIDTH 1

#define MAIN_CHARGER_ALLOWED_INTERRUPT_PARAM_MASK  0x1

//  
//  Tvset (75 Ohms) connection/disconnection on CVBS ball detected.
//  Register InterruptSource1 0x0E00, Bits 2:2, typedef plug_tv_interrupt
//   
//  
#define PLUG_TV_INTERRUPT_MASK 0x4
#define PLUG_TV_INTERRUPT_OFFSET 2
#define PLUG_TV_INTERRUPT_STOP_BIT 2
#define PLUG_TV_INTERRUPT_START_BIT 2
#define PLUG_TV_INTERRUPT_WIDTH 1

#define PLUG_TV_INTERRUPT_PARAM_MASK  0x4

//  
//  Thermal warning.
//  Register InterruptSource1 0x0E00, Bits 3:3, typedef thermal_warning_interrupt
//   
//  
#define THERMAL_WARNING_INTERRUPT_MASK 0x8
#define THERMAL_WARNING_INTERRUPT_OFFSET 3
#define THERMAL_WARNING_INTERRUPT_STOP_BIT 3
#define THERMAL_WARNING_INTERRUPT_START_BIT 3
#define THERMAL_WARNING_INTERRUPT_WIDTH 1

#define THERMAL_WARNING_INTERRUPT_PARAM_MASK  0x8

//  
//  LevelChange on PonKey2 ball detected.
//  Register InterruptSource1 0x0E00, Bits 5:5, typedef rising_pon_key2_interrupt
//   
//  
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_MASK 0x20
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_OFFSET 5
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_STOP_BIT 5
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_START_BIT 5
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_WIDTH 1

#define RISING_PON_KEY_2_INTERRUPT_PARAM_MASK  0x20

//  
//  LevelChange on PonKey1 ball detected.
//  Register InterruptSource1 0x0E00, Bits 7:7, typedef rising_pon_key1_interrupt
//   
//  
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_MASK 0x80
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_OFFSET 7
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_STOP_BIT 7
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_START_BIT 7
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_WIDTH 1

#define RISING_PON_KEY_1_INTERRUPT_PARAM_MASK  0x80
#define INTERRUPT_SOURCE_2_REG 0xE01

//  
//  VbatA ball goes upper over voltage threshold (charge stopped).
//  Register InterruptSource2 0x0E01, Bits 0:0, typedef battery_over_voltage_interrupt
//   
//  
#define BATTERY_OVER_VOLTAGE_INTERRUPT_MASK 0x1
#define BATTERY_OVER_VOLTAGE_INTERRUPT_OFFSET 0
#define BATTERY_OVER_VOLTAGE_INTERRUPT_STOP_BIT 0
#define BATTERY_OVER_VOLTAGE_INTERRUPT_START_BIT 0
#define BATTERY_OVER_VOLTAGE_INTERRUPT_WIDTH 1

#define BATTERY_OVER_VOLTAGE_INTERRUPT_PARAM_MASK  0x1

//  
//  Main charger plug/unplug detected.
//  Register InterruptSource2 0x0E01, Bits 3:3, typedef main_charger_plug_interrupt
//   
//  
#define MAIN_CHARGER_PLUG_INTERRUPT_MASK 0x8
#define MAIN_CHARGER_PLUG_INTERRUPT_OFFSET 3
#define MAIN_CHARGER_PLUG_INTERRUPT_STOP_BIT 3
#define MAIN_CHARGER_PLUG_INTERRUPT_START_BIT 3
#define MAIN_CHARGER_PLUG_INTERRUPT_WIDTH 1

#define MAIN_CHARGER_PLUG_INTERRUPT_PARAM_MASK  0x8

//  
//  Vbus ball level change detected.
//  Register InterruptSource2 0x0E01, Bits 7:7, typedef vbus_edge_interrupt
//   
//  
#define VBUS_LEVEL_INTERRUPT_MASK 0x80
#define VBUS_LEVEL_INTERRUPT_OFFSET 7
#define VBUS_LEVEL_INTERRUPT_STOP_BIT 7
#define VBUS_LEVEL_INTERRUPT_START_BIT 7
#define VBUS_LEVEL_INTERRUPT_WIDTH 1

#define VBUS_EDGE_INTERRUPT_PARAM_MASK  0x80
#define INTERRUPT_SOURCE_3_REG 0xE02

//  
//  Usb charger was in dropout and internal input current loop update current regulation 
//  to avoid usb charger to drop.
//  Register InterruptSource3 0x0E02, Bits 0:0, typedef usb_drop_out
//   
//  
#define USB_CHARGER_DROPOUT_MASK 0x1
#define USB_CHARGER_DROPOUT_OFFSET 0
#define USB_CHARGER_DROPOUT_STOP_BIT 0
#define USB_CHARGER_DROPOUT_START_BIT 0
#define USB_CHARGER_DROPOUT_WIDTH 1

#define USB_DROP_OUT_PARAM_MASK  0x1

//  
//  RTC timer reaches a 60s period.
//  Register InterruptSource3 0x0E02, Bits 1:1, typedef period60s
//   
//  
#define RTC_6_0S_MASK 0x2
#define RTC_6_0S_OFFSET 1
#define RTC_6_0S_STOP_BIT 1
#define RTC_6_0S_START_BIT 1
#define RTC_6_0S_WIDTH 1

#define PERIOD_6_0S_PARAM_MASK  0x2

//  
//  RTC timer reaches alarm time.
//  Register InterruptSource3 0x0E02, Bits 2:2, typedef reach_alarm_time
//   
//  
#define RTC_ALARM_MASK 0x4
#define RTC_ALARM_OFFSET 2
#define RTC_ALARM_STOP_BIT 2
#define RTC_ALARM_START_BIT 2
#define RTC_ALARM_WIDTH 1

#define REACH_ALARM_TIME_PARAM_MASK  0x4

//  
//  Battery removal.
//  Register InterruptSource3 0x0E02, Bits 4:4, typedef battery_removal_interrupt
//   
//  
#define BATTERY_REMOVAL_INTERRUPT_MASK 0x10
#define BATTERY_REMOVAL_INTERRUPT_OFFSET 4
#define BATTERY_REMOVAL_INTERRUPT_STOP_BIT 4
#define BATTERY_REMOVAL_INTERRUPT_START_BIT 4
#define BATTERY_REMOVAL_INTERRUPT_WIDTH 1

#define BATTERY_REMOVAL_INTERRUPT_PARAM_MASK  0x10

//  
//  Watchdog charger expiration detected.
//  Register InterruptSource3 0x0E02, Bits 5:5, typedef 
//  watchdog_charger_expiration_interrupt
//   
//  
#define WATCHDOG_CHARGER_EXPIRATION_INTERRUPT_MASK 0x20
#define WATCHDOG_CHARGER_EXPIRATION_INTERRUPT_OFFSET 5
#define WATCHDOG_CHARGER_EXPIRATION_INTERRUPT_STOP_BIT 5
#define WATCHDOG_CHARGER_EXPIRATION_INTERRUPT_START_BIT 5
#define WATCHDOG_CHARGER_EXPIRATION_INTERRUPT_WIDTH 1

#define WATCHDOG_CHARGER_EXPIRATION_INTERRUPT_PARAM_MASK  0x20

//  
//  Overvoltage on Vbus ball detected.
//  Register InterruptSource3 0x0E02, Bits 6:6, typedef overvoltage_on_vbus_interrupt
//   
//  
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_MASK 0x40
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_OFFSET 6
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_STOP_BIT 6
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_START_BIT 6
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_WIDTH 1

#define OVERVOLTAGE_ON_VBUS_INTERRUPT_PARAM_MASK  0x40

//  
//  Icharge > Icharge max programmed detected.
//  Register InterruptSource3 0x0E02, Bits 7:7, typedef 
//  main_charger_over_current_interrupt
//   
//  
#define MAIN_CHARGER_OVER_CURRENT_INTERRUPT_MASK 0x80
#define MAIN_CHARGER_OVER_CURRENT_INTERRUPT_OFFSET 7
#define MAIN_CHARGER_OVER_CURRENT_INTERRUPT_STOP_BIT 7
#define MAIN_CHARGER_OVER_CURRENT_INTERRUPT_START_BIT 7
#define MAIN_CHARGER_OVER_CURRENT_INTERRUPT_WIDTH 1

#define MAIN_CHARGER_OVER_CURRENT_INTERRUPT_PARAM_MASK  0x80
#define INTERRUPT_SOURCE_4_REG 0xE03

//  
//  Accumulation of N sample conversion is reached.
//  Register InterruptSource4 0x0E03, Bits 0:0, typedef accumulation_sample_conversion
//   
//  
#define ACCUMULATION_SAMPLE_CONVERSION_MASK 0x1
#define ACCUMULATION_SAMPLE_CONVERSION_OFFSET 0
#define ACCUMULATION_SAMPLE_CONVERSION_STOP_BIT 0
#define ACCUMULATION_SAMPLE_CONVERSION_START_BIT 0
#define ACCUMULATION_SAMPLE_CONVERSION_WIDTH 1

#define ACCUMULATION_SAMPLE_CONVERSION_PARAM_MASK  0x1

//  
//  Audio interrupt form audio digital part detected.
//  Register InterruptSource4 0x0E03, Bits 1:1, typedef audio_interrupt
//   
//  
#define AUDIO_INTERRUPT_MASK 0x2
#define AUDIO_INTERRUPT_OFFSET 1
#define AUDIO_INTERRUPT_STOP_BIT 1
#define AUDIO_INTERRUPT_START_BIT 1
#define AUDIO_INTERRUPT_WIDTH 1

#define AUDIO_INTERRUPT_PARAM_MASK  0x2

//  
//  Coulomb Counter has ended data convertion.
//  Register InterruptSource4 0x0E03, Bits 2:2, typedef coulomb_counter_ended_interrupt
//   
//  
#define COULOMB_ENDED_INTERRUPT_MASK 0x4
#define COULOMB_ENDED_INTERRUPT_OFFSET 2
#define COULOMB_ENDED_INTERRUPT_STOP_BIT 2
#define COULOMB_ENDED_INTERRUPT_START_BIT 2
#define COULOMB_ENDED_INTERRUPT_WIDTH 1

#define COULOMB_COUNTER_ENDED_INTERRUPT_PARAM_MASK  0x4

//  
//  Coulomb Counter has ended its calibration.
//  Register InterruptSource4 0x0E03, Bits 3:3, typedef 
//  coulomb_calibration_endedd_interrupt
//   
//  
#define COULOMB_CALIBRATION_ENDEDD_INTERRUPT_MASK 0x8
#define COULOMB_CALIBRATION_ENDEDD_INTERRUPT_OFFSET 3
#define COULOMB_CALIBRATION_ENDEDD_INTERRUPT_STOP_BIT 3
#define COULOMB_CALIBRATION_ENDEDD_INTERRUPT_START_BIT 3
#define COULOMB_CALIBRATION_ENDEDD_INTERRUPT_WIDTH 1

#define COULOMB_CALIBRATION_ENDEDD_INTERRUPT_PARAM_MASK  0x8

//  
//  VbatA voltage reaches LowBat register programmed threshold.
//  Register InterruptSource4 0x0E03, Bits 5:5, typedef high_battery_interrupt
//   
//  
#define HIGH_BATTERY_INTERRUPT_MASK 0x20
#define HIGH_BATTERY_INTERRUPT_OFFSET 5
#define HIGH_BATTERY_INTERRUPT_STOP_BIT 5
#define HIGH_BATTERY_INTERRUPT_START_BIT 5
#define HIGH_BATTERY_INTERRUPT_WIDTH 1

#define HIGH_BATTERY_INTERRUPT_PARAM_MASK  0x20

//  
//  BackUpBat ball voltage goes above RtcBackupChg register.
//  Register InterruptSource4 0x0E03, Bits 7:7, typedef rtc_battery_high_interrupt
//   
//  
#define RTC_BATTERY_HIGH_INTERRUPT_MASK 0x80
#define RTC_BATTERY_HIGH_INTERRUPT_OFFSET 7
#define RTC_BATTERY_HIGH_INTERRUPT_STOP_BIT 7
#define RTC_BATTERY_HIGH_INTERRUPT_START_BIT 7
#define RTC_BATTERY_HIGH_INTERRUPT_WIDTH 1

#define RTC_BATTERY_HIGH_INTERRUPT_PARAM_MASK  0x80
#define INTERRUPT_SOURCE_5_REG 0xE04

//  
//  ADC DAta Available . Software condition.
//  Register InterruptSource5 0x0E04, Bits 0:0, typedef adc_data_software_int
//   
//  
#define ADC_DATA_SOFTWARE_INT_MASK 0x1
#define ADC_DATA_SOFTWARE_INT_OFFSET 0
#define ADC_DATA_SOFTWARE_INT_STOP_BIT 0
#define ADC_DATA_SOFTWARE_INT_START_BIT 0
#define ADC_DATA_SOFTWARE_INT_WIDTH 1

#define ADC_DATA_SOFTWARE_INT_PARAM_MASK  0x1

//  
//  AccDetect1 ball voltage level goes below AccDetect1Th.
//  Register InterruptSource5 0x0E04, Bits 1:1, typedef accessory1_low_voltage
//   
//  
#define ACCESSORY_1_LOW_VOLTAGE_MASK 0x2
#define ACCESSORY_1_LOW_VOLTAGE_OFFSET 1
#define ACCESSORY_1_LOW_VOLTAGE_STOP_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_START_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_WIDTH 1

#define ACCESSORY_1_LOW_VOLTAGE_PARAM_MASK  0x2

//  
//  AccDetect2 ball voltage level reaches AccDetect22Th.
//  Register InterruptSource5 0x0E04, Bits 4:4, typedef accessory22_high_voltage
//   
//  
#define ACCESSORY_22_HIGH_VOLTAGE_MASK 0x10
#define ACCESSORY_22_HIGH_VOLTAGE_OFFSET 4
#define ACCESSORY_22_HIGH_VOLTAGE_STOP_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_START_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_WIDTH 1

#define ACCESSORY_22_HIGH_VOLTAGE_PARAM_MASK  0x10

//  
//  AccDetect2 ball voltage level reaches AccDetect21Th.
//  Register InterruptSource5 0x0E04, Bits 6:6, typedef accessory21_high_voltage
//   
//  
#define ACCESSORY_21_HIGH_VOLTAGE_MASK 0x40
#define ACCESSORY_21_HIGH_VOLTAGE_OFFSET 6
#define ACCESSORY_21_HIGH_VOLTAGE_STOP_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_START_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_WIDTH 1

#define ACCESSORY_21_HIGH_VOLTAGE_PARAM_MASK  0x40

//  
//  ADC DAta Available ..
//  Register InterruptSource5 0x0E04, Bits 7:7, typedef adc_data_available
//   
//  
#define ADC_DATA_HARDWARE_INT_MASK 0x80
#define ADC_DATA_HARDWARE_INT_OFFSET 7
#define ADC_DATA_HARDWARE_INT_STOP_BIT 7
#define ADC_DATA_HARDWARE_INT_START_BIT 7
#define ADC_DATA_HARDWARE_INT_WIDTH 1

#define ADC_DATA_AVAILABLE_PARAM_MASK  0x80
#define INTERRUPT_SOURCE_7_REG 0xE06

//  
//  Edge detected on GPIO13 ball.
//  Register InterruptSource7 0x0E06, Bits 7:7, typedef gpio13_rising_event
//   
//  
#define GPIO_13_RISING_MASK 0x80
#define GPIO_13_RISING_OFFSET 7
#define GPIO_13_RISING_STOP_BIT 7
#define GPIO_13_RISING_START_BIT 7
#define GPIO_13_RISING_WIDTH 1

#define GPIO_13_RISING_EVENT_PARAM_MASK  0x80

//  
//  Edge detected on GPIO12 ball.
//  Register InterruptSource7 0x0E06, Bits 6:6, typedef gpio12_rising_event
//   
//  
#define GPIO_12_RISING_MASK 0x40
#define GPIO_12_RISING_OFFSET 6
#define GPIO_12_RISING_STOP_BIT 6
#define GPIO_12_RISING_START_BIT 6
#define GPIO_12_RISING_WIDTH 1

#define GPIO_12_RISING_EVENT_PARAM_MASK  0x40

//  
//  Edge detected on GPIO11 ball.
//  Register InterruptSource7 0x0E06, Bits 5:5, typedef gpio11_rising_event
//   
//  
#define GPIO_11_RISING_MASK 0x20
#define GPIO_11_RISING_OFFSET 5
#define GPIO_11_RISING_STOP_BIT 5
#define GPIO_11_RISING_START_BIT 5
#define GPIO_11_RISING_WIDTH 1

#define GPIO_11_RISING_EVENT_PARAM_MASK  0x20

//  
//  Edge detected on GPIO10 ball.
//  Register InterruptSource7 0x0E06, Bits 4:4, typedef gpio10_rising_event
//   
//  
#define GPIO_10_RISING_MASK 0x10
#define GPIO_10_RISING_OFFSET 4
#define GPIO_10_RISING_STOP_BIT 4
#define GPIO_10_RISING_START_BIT 4
#define GPIO_10_RISING_WIDTH 1

#define GPIO_10_RISING_EVENT_PARAM_MASK  0x10

//  
//  Edge detected on GPIO9 ball.
//  Register InterruptSource7 0x0E06, Bits 3:3, typedef gpio9_rising_event
//   
//  
#define GPIO_9_RISING_MASK 0x8
#define GPIO_9_RISING_OFFSET 3
#define GPIO_9_RISING_STOP_BIT 3
#define GPIO_9_RISING_START_BIT 3
#define GPIO_9_RISING_WIDTH 1

#define GPIO_9_RISING_EVENT_PARAM_MASK  0x8

//  
//  Edge detected on GPIO8 ball.
//  Register InterruptSource7 0x0E06, Bits 2:2, typedef gpio8_rising_event
//   
//  
#define GPIO_8_RISING_MASK 0x4
#define GPIO_8_RISING_OFFSET 2
#define GPIO_8_RISING_STOP_BIT 2
#define GPIO_8_RISING_START_BIT 2
#define GPIO_8_RISING_WIDTH 1

#define GPIO_8_RISING_EVENT_PARAM_MASK  0x4

//  
//  Edge detected on GPIO7 ball.
//  Register InterruptSource7 0x0E06, Bits 1:1, typedef gpio7_rising_event
//   
//  
#define GPIO_7_RISING_MASK 0x2
#define GPIO_7_RISING_OFFSET 1
#define GPIO_7_RISING_STOP_BIT 1
#define GPIO_7_RISING_START_BIT 1
#define GPIO_7_RISING_WIDTH 1

#define GPIO_7_RISING_EVENT_PARAM_MASK  0x2

//  
//  Edge detected on GPIO6 ball.
//  Register InterruptSource7 0x0E06, Bits 0:0, typedef gpio6_rising_event
//   
//  
#define GPIO_6_RISING_MASK 0x1
#define GPIO_6_RISING_OFFSET 0
#define GPIO_6_RISING_STOP_BIT 0
#define GPIO_6_RISING_START_BIT 0
#define GPIO_6_RISING_WIDTH 1

#define GPIO_6_RISING_EVENT_PARAM_MASK  0x1
#define INTERRUPT_SOURCE_8_REG 0xE07

//  
//  Edge detected on GPIO41 ball.
//  Register InterruptSource8 0x0E07, Bits 7:7, typedef gpio41_rising_event
//   
//  
#define GPIO_41_RISING_MASK 0x80
#define GPIO_41_RISING_OFFSET 7
#define GPIO_41_RISING_STOP_BIT 7
#define GPIO_41_RISING_START_BIT 7
#define GPIO_41_RISING_WIDTH 1

#define GPIO_41_RISING_EVENT_PARAM_MASK  0x80

//  
//  Edge detected on GPIO40 ball.
//  Register InterruptSource8 0x0E07, Bits 6:6, typedef gpio40_rising_event
//   
//  
#define GPIO_40_RISING_MASK 0x40
#define GPIO_40_RISING_OFFSET 6
#define GPIO_40_RISING_STOP_BIT 6
#define GPIO_40_RISING_START_BIT 6
#define GPIO_40_RISING_WIDTH 1

#define GPIO_40_RISING_EVENT_PARAM_MASK  0x40

//  
//  Edge detected on GPIO39 ball.
//  Register InterruptSource8 0x0E07, Bits 5:5, typedef gpio39_rising_event
//   
//  
#define GPIO_39_RISING_MASK 0x20
#define GPIO_39_RISING_OFFSET 5
#define GPIO_39_RISING_STOP_BIT 5
#define GPIO_39_RISING_START_BIT 5
#define GPIO_39_RISING_WIDTH 1

#define GPIO_39_RISING_EVENT_PARAM_MASK  0x20

//  
//  Edge detected on GPIO38 ball.
//  Register InterruptSource8 0x0E07, Bits 4:4, typedef gpio38_rising_event
//   
//  
#define GPIO_38_RISING_MASK 0x10
#define GPIO_38_RISING_OFFSET 4
#define GPIO_38_RISING_STOP_BIT 4
#define GPIO_38_RISING_START_BIT 4
#define GPIO_38_RISING_WIDTH 1

#define GPIO_38_RISING_EVENT_PARAM_MASK  0x10

//  
//  Edge detected on GPIO37 ball.
//  Register InterruptSource8 0x0E07, Bits 3:3, typedef gpio37_rising_event
//   
//  
#define GPIO_37_RISING_MASK 0x8
#define GPIO_37_RISING_OFFSET 3
#define GPIO_37_RISING_STOP_BIT 3
#define GPIO_37_RISING_START_BIT 3
#define GPIO_37_RISING_WIDTH 1

#define GPIO_37_RISING_EVENT_PARAM_MASK  0x8

//  
//  Edge detected on GPIO36 ball.
//  Register InterruptSource8 0x0E07, Bits 2:2, typedef gpio36_rising_event
//   
//  
#define GPIO_36_RISING_MASK 0x4
#define GPIO_36_RISING_OFFSET 2
#define GPIO_36_RISING_STOP_BIT 2
#define GPIO_36_RISING_START_BIT 2
#define GPIO_36_RISING_WIDTH 1

#define GPIO_36_RISING_EVENT_PARAM_MASK  0x4

//  
//  Edge detected on GPIO25 ball.
//  Register InterruptSource8 0x0E07, Bits 1:1, typedef gpio25_rising_event
//   
//  
#define GPIO_25_RISING_MASK 0x2
#define GPIO_25_RISING_OFFSET 1
#define GPIO_25_RISING_STOP_BIT 1
#define GPIO_25_RISING_START_BIT 1
#define GPIO_25_RISING_WIDTH 1

#define GPIO_25_RISING_EVENT_PARAM_MASK  0x2

//  
//  Edge detected on GPIO24 ball.
//  Register InterruptSource8 0x0E07, Bits 0:0, typedef gpio24_rising_event
//   
//  
#define GPIO_24_RISING_MASK 0x1
#define GPIO_24_RISING_OFFSET 0
#define GPIO_24_RISING_STOP_BIT 0
#define GPIO_24_RISING_START_BIT 0
#define GPIO_24_RISING_WIDTH 1

#define GPIO_24_RISING_EVENT_PARAM_MASK  0x1
#define INTERRUPT_SOURCE_12_REG 0xE09

//  
//  0: UsbLink status unchanged 
//  1: UsbLink status changed.
//  Register InterruptSource12 0x0E09, Bits 7:7, typedef usb_link_status_changed
//   
//  
#define USB_LINK_STATUS_MASK 0x80
#define USB_LINK_STATUS_OFFSET 7
#define USB_LINK_STATUS_STOP_BIT 7
#define USB_LINK_STATUS_START_BIT 7
#define USB_LINK_STATUS_WIDTH 1

#define USB_LINK_STATUS_CHANGED_PARAM_MASK  0x80

//  
//  0: Current source Off 
//  1: Current source On.
//  Register InterruptSource12 0x0E09, Bits 4:4, typedef current_source_on
//   
//  
#define ADP_SENSE_OFF_MASK 0x10
#define ADP_SENSE_OFF_OFFSET 4
#define ADP_SENSE_OFF_STOP_BIT 4
#define ADP_SENSE_OFF_START_BIT 4
#define ADP_SENSE_OFF_WIDTH 1

#define CURRENT_SOURCE_ON_PARAM_MASK  0x10

//  
//  0: previous state 
//  1: accessory unplug on Vbus.
//  Register InterruptSource12 0x0E09, Bits 3:3, typedef adp_probe_un_plug
//   
//  
#define ADP_PROBE_UN_PLUG_MASK 0x8
#define ADP_PROBE_UN_PLUG_OFFSET 3
#define ADP_PROBE_UN_PLUG_STOP_BIT 3
#define ADP_PROBE_UN_PLUG_START_BIT 3
#define ADP_PROBE_UN_PLUG_WIDTH 1

#define ADP_PROBE_UN_PLUG_PARAM_MASK  0x8

//  
//  0: previous state 
//  1: accessory plug on Vbus.
//  Register InterruptSource12 0x0E09, Bits 2:2, typedef adp_probe_plug
//   
//  
#define ADP_PROBE_PLUG_MASK 0x4
#define ADP_PROBE_PLUG_OFFSET 2
#define ADP_PROBE_PLUG_STOP_BIT 2
#define ADP_PROBE_PLUG_START_BIT 2
#define ADP_PROBE_PLUG_WIDTH 1

#define ADP_PROBE_PLUG_PARAM_MASK  0x4

//  
//  0: inactive 
//  1: no detection after 2046*32Khz clock cycles (about 62ms).
//  Register InterruptSource12 0x0E09, Bits 1:1, typedef adp_sink_error
//   
//  
#define ADP_SINK_ERROR_MASK 0x2
#define ADP_SINK_ERROR_OFFSET 1
#define ADP_SINK_ERROR_STOP_BIT 1
#define ADP_SINK_ERROR_START_BIT 1
#define ADP_SINK_ERROR_WIDTH 1

#define ADP_SINK_ERROR_PARAM_MASK  0x2

//  
//  0: inactive 
//  1: no detection after inactive no detection after 2046*32Khz clock cycles (about 
//  62ms).
//  Register InterruptSource12 0x0E09, Bits 0:0, typedef adp_source_error
//   
//  
#define ADP_SOURCE_ERROR_MASK 0x1
#define ADP_SOURCE_ERROR_OFFSET 0
#define ADP_SOURCE_ERROR_STOP_BIT 0
#define ADP_SOURCE_ERROR_START_BIT 0
#define ADP_SOURCE_ERROR_WIDTH 1

#define ADP_SOURCE_ERROR_PARAM_MASK  0x1
#define INTERRUPT_SOURCE_19_REG 0xE12

//  
//  0: Btemp < BtempHigh 
//  1: Btemp > BtempHigh.
//  Register InterruptSource19 0x0E12, Bits 3:3, typedef 
//  battery_temperature_high_interrupt
//   
//  
#define BATTERY_TEMPERATURE_HIGH_MASK 0x8
#define BATTERY_TEMPERATURE_HIGH_OFFSET 3
#define BATTERY_TEMPERATURE_HIGH_STOP_BIT 3
#define BATTERY_TEMPERATURE_HIGH_START_BIT 3
#define BATTERY_TEMPERATURE_HIGH_WIDTH 1

#define BATTERY_TEMPERATURE_HIGH_INTERRUPT_PARAM_MASK  0x8

//  
//  0: Btemp < BtempMedium or Btemp > BtempHigh 
//  1: BtempHigh > Btemp > BtempMedium.
//  Register InterruptSource19 0x0E12, Bits 2:2, typedef 
//  battery_temperature_medium_high_interrupt
//   
//  
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK 0x4
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_OFFSET 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_STOP_BIT 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_START_BIT 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_WIDTH 1

#define BATTERY_TEMPERATURE_MEDIUM_HIGH_INTERRUPT_PARAM_MASK  0x4

//  
//  0: Btemp < BtempLow or Btemp > BtempMedium 
//  1: BtempMedium > Btemp > BtempLow.
//  Register InterruptSource19 0x0E12, Bits 1:1, typedef 
//  battery_temperature_low_medium_interrupt
//   
//  
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK 0x2
#define BATTERY_TEMPERATURE_LOW_MEDIUM_OFFSET 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_STOP_BIT 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_START_BIT 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_WIDTH 1

#define BATTERY_TEMPERATURE_LOW_MEDIUM_INTERRUPT_PARAM_MASK  0x2

//  
//  0: Btemp > BtempLow 
//  1: BtempLow > Btemp.
//  Register InterruptSource19 0x0E12, Bits 0:0, typedef 
//  battery_temperature_low_interrupt
//   
//  
#define BATTERY_TEMPERATURE_LOW_MASK 0x1
#define BATTERY_TEMPERATURE_LOW_OFFSET 0
#define BATTERY_TEMPERATURE_LOW_STOP_BIT 0
#define BATTERY_TEMPERATURE_LOW_START_BIT 0
#define BATTERY_TEMPERATURE_LOW_WIDTH 1

#define BATTERY_TEMPERATURE_LOW_INTERRUPT_PARAM_MASK  0x1

//  
//  IntSource20 Status.
//  Register InterruptSource20 0x0E13, Bits 0:7, typedef interrupt_source20
//   
//  
#define INTERRUPT_SOURCE_20_MASK 0xFF
#define INTERRUPT_SOURCE_20_OFFSET 7
#define INTERRUPT_SOURCE_20_STOP_BIT 7
#define INTERRUPT_SOURCE_20_START_BIT 0
#define INTERRUPT_SOURCE_20_WIDTH 8

#define INTERRUPT_SOURCE_20_REG 0xE13
#define INTERRUPT_SOURCE_20_PARAM_MASK  0xFF

//  
//  0: Inactive 
//  1:SRP detected.
//  Register InterruptSource20 0x0E13, Bits 0:0, typedef srp_detected
//   
//  
#define SRP_P_DETECT_MASK 0x1
#define SRP_P_DETECT_OFFSET 0
#define SRP_P_DETECT_STOP_BIT 0
#define SRP_P_DETECT_START_BIT 0
#define SRP_P_DETECT_WIDTH 1

#define SRP_DETECTED_PARAM_MASK  0x1

//  
//  Not allowed USB charger detected.
//  Register InterruptSource20 0x0E13, Bits 1:1, typedef usb_bad_charger_plug_event
//   
//  
#define USB_BAD_CHARGER_PLUG_MASK 0x2
#define USB_BAD_CHARGER_PLUG_OFFSET 1
#define USB_BAD_CHARGER_PLUG_STOP_BIT 1
#define USB_BAD_CHARGER_PLUG_START_BIT 1
#define USB_BAD_CHARGER_PLUG_WIDTH 1

#define USB_BAD_CHARGER_PLUG_EVENT_PARAM_MASK  0x2

//  
//  edge detected on ID.
//  Register InterruptSource20 0x0E13, Bits 2:2, typedef id_wake_up_rising_event
//   
//  
#define ID_WAKE_UP_RISING_MASK 0x4
#define ID_WAKE_UP_RISING_OFFSET 2
#define ID_WAKE_UP_RISING_STOP_BIT 2
#define ID_WAKE_UP_RISING_START_BIT 2
#define ID_WAKE_UP_RISING_WIDTH 1

#define ID_WAKE_UP_RISING_EVENT_PARAM_MASK  0x4

//  
//  ID resistance greater than 37k detected.
//  Register InterruptSource20 0x0E13, Bits 4:4, typedef id_detection_greater80k
//   
//  
#define ID_DETECTION_R1_MASK 0x10
#define ID_DETECTION_R1_OFFSET 4
#define ID_DETECTION_R1_STOP_BIT 4
#define ID_DETECTION_R1_START_BIT 4
#define ID_DETECTION_R1_WIDTH 1

#define ID_DETECTION_GREATER_8_0K_PARAM_MASK  0x10

//  
//  ID resistance greater than 69k detected.
//  Register InterruptSource20 0x0E13, Bits 5:5, typedef id_detection_greater140k
//   
//  
#define ID_DETECTION_R2_MASK 0x20
#define ID_DETECTION_R2_OFFSET 5
#define ID_DETECTION_R2_STOP_BIT 5
#define ID_DETECTION_R2_START_BIT 5
#define ID_DETECTION_R2_WIDTH 1

#define ID_DETECTION_GREATER_14_0K_PARAM_MASK  0x20

//  
//  ID resistance greater than 125k detected.
//  Register InterruptSource20 0x0E13, Bits 6:6, typedef id_detection_greater220k
//   
//  
#define ID_DETECTION_R3_MASK 0x40
#define ID_DETECTION_R3_OFFSET 6
#define ID_DETECTION_R3_STOP_BIT 6
#define ID_DETECTION_R3_START_BIT 6
#define ID_DETECTION_R3_WIDTH 1

#define ID_DETECTION_GREATER_22_0K_PARAM_MASK  0x40

//  
//  ID resistance greater than 220k detected.
//  Register InterruptSource20 0x0E13, Bits 7:7, typedef id_detection_greater330k
//   
//  
#define ID_DETECTION_R4_MASK 0x80
#define ID_DETECTION_R4_OFFSET 7
#define ID_DETECTION_R4_STOP_BIT 7
#define ID_DETECTION_R4_START_BIT 7
#define ID_DETECTION_R4_WIDTH 1

#define ID_DETECTION_GREATER_33_0K_PARAM_MASK  0x80

//  
//  IntSource21 Status.
//  Register InterruptSource21 0x0E14, Bits 0:7, typedef interrupt_source21
//   
//  
#define INTERRUPT_SOURCE_21_MASK 0xFF
#define INTERRUPT_SOURCE_21_OFFSET 7
#define INTERRUPT_SOURCE_21_STOP_BIT 7
#define INTERRUPT_SOURCE_21_START_BIT 0
#define INTERRUPT_SOURCE_21_WIDTH 8

#define INTERRUPT_SOURCE_21_REG 0xE14
#define INTERRUPT_SOURCE_21_PARAM_MASK  0xFF

//  
//  USB charger detected.
//  Register InterruptSource21 0x0E14, Bits 6:6, typedef usb_charger_plug
//   
//  
#define USB_CHARGER_PLUG_MASK 0x40
#define USB_CHARGER_PLUG_OFFSET 6
#define USB_CHARGER_PLUG_STOP_BIT 6
#define USB_CHARGER_PLUG_START_BIT 6
#define USB_CHARGER_PLUG_WIDTH 1

#define USB_CHARGER_PLUG_PARAM_MASK  0x40

//  
//  IntSource22 Status.
//  Register InterruptSource22 0x0E15, Bits 0:7, typedef interrupt_source22
//   
//  
#define INTERRUPT_SOURCE_22_MASK 0xFF
#define INTERRUPT_SOURCE_22_OFFSET 7
#define INTERRUPT_SOURCE_22_STOP_BIT 7
#define INTERRUPT_SOURCE_22_START_BIT 0
#define INTERRUPT_SOURCE_22_WIDTH 8

#define INTERRUPT_SOURCE_22_REG 0xE15
#define INTERRUPT_SOURCE_22_PARAM_MASK  0xFF

//  
//  0: Die temperature is not upper than main charger thermal protection threshold 
//  (charge stopped) 
//  1: Die temperature is upper than main charger thermal protection threshold (charge 
//  stopped).
//  Register InterruptSource22 0x0E15, Bits 3:3, typedef main_charger_thermal_protection
//   
//  
#define MAIN_CHARGER_THERMAL_PROTECTION_MASK 0x8
#define MAIN_CHARGER_THERMAL_PROTECTION_OFFSET 3
#define MAIN_CHARGER_THERMAL_PROTECTION_STOP_BIT 3
#define MAIN_CHARGER_THERMAL_PROTECTION_START_BIT 3
#define MAIN_CHARGER_THERMAL_PROTECTION_WIDTH 1

#define MAIN_CHARGER_THERMAL_PROTECTION_PARAM_MASK  0x8

//  
//  0: Die temperature is not upper than usb charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than usb charger thermal protection threshold (charge 
//  stopped).
//  Register InterruptSource22 0x0E15, Bits 1:1, typedef usb_charger_thermal_protection
//   
//  
#define USB_CHARGER_THERMAL_PROTECTION_MASK 0x2
#define USB_CHARGER_THERMAL_PROTECTION_OFFSET 1
#define USB_CHARGER_THERMAL_PROTECTION_STOP_BIT 1
#define USB_CHARGER_THERMAL_PROTECTION_START_BIT 1
#define USB_CHARGER_THERMAL_PROTECTION_WIDTH 1

#define USB_CHARGER_THERMAL_PROTECTION_PARAM_MASK  0x2
#endif
