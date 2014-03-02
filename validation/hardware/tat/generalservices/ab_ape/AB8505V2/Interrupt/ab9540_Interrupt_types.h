/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Interrupt/Linux/ab9540_Interrupt_types.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_INTERRUPT_LINUX_AB9540_INTERRUPT_TYPES_H
#define C__LISP_PRODUCTION_AB9540_AB9540_INTERRUPT_LINUX_AB9540_INTERRUPT_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


//  
//  LevelChange on PonKey1 ball detected.
//  Register InterruptSource1 0x0E00, Bits 7:7, typedef rising_pon_key1_interrupt
//   
//  
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_MASK 0x80U
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_OFFSET 7
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_STOP_BIT 7
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_START_BIT 7
#define LEVEL_CHANGE_PON_KEY_1_INTERRUPT_WIDTH 1

#define INTERRUPT_SOURCE_1_REG 0xE00



//  
//  LevelChange on PonKey2 ball detected.
//  Register InterruptSource1 0x0E00, Bits 5:5, typedef rising_pon_key2_interrupt
//   
//  
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_MASK 0x20U
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_OFFSET 5
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_STOP_BIT 5
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_START_BIT 5
#define LEVEL_CHANGE_PON_KEY_2_INTERRUPT_WIDTH 1




//  
//  Thermal warning.
//  Register InterruptSource1 0x0E00, Bits 3:3, typedef thermal_warning_interrupt
//   
//  
#define THERMAL_WARNING_INTERRUPT_MASK 0x8U
#define THERMAL_WARNING_INTERRUPT_OFFSET 3
#define THERMAL_WARNING_INTERRUPT_STOP_BIT 3
#define THERMAL_WARNING_INTERRUPT_START_BIT 3
#define THERMAL_WARNING_INTERRUPT_WIDTH 1




//  
//  Vbus ball level change detected.
//  Register InterruptSource2 0x0E01, Bits 7:7, typedef vbus_edge_interrupt
//   
//  
#define VBUS_LEVEL_INTERRUPT_MASK 0x80U
#define VBUS_LEVEL_INTERRUPT_OFFSET 7
#define VBUS_LEVEL_INTERRUPT_STOP_BIT 7
#define VBUS_LEVEL_INTERRUPT_START_BIT 7
#define VBUS_LEVEL_INTERRUPT_WIDTH 1

#define INTERRUPT_SOURCE_2_REG 0xE01




//  
//  VbatA ball goes upper over voltage threshold (charge stopped).
//  Register InterruptSource2 0x0E01, Bits 0:0, typedef battery_over_voltage_interrupt
//   
//  
#define BATTERY_OVER_VOLTAGE_INTERRUPT_MASK 0x1U
#define BATTERY_OVER_VOLTAGE_INTERRUPT_OFFSET 0
#define BATTERY_OVER_VOLTAGE_INTERRUPT_STOP_BIT 0
#define BATTERY_OVER_VOLTAGE_INTERRUPT_START_BIT 0
#define BATTERY_OVER_VOLTAGE_INTERRUPT_WIDTH 1




//  
//  Overvoltage on Vbus ball detected.
//  Register InterruptSource3 0x0E02, Bits 6:6, typedef overvoltage_on_vbus_interrupt
//   
//  
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_MASK 0x40U
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_OFFSET 6
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_STOP_BIT 6
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_START_BIT 6
#define OVERVOLTAGE_ON_VBUS_INTERRUPT_WIDTH 1





//  
//  Battery removal.
//  Register InterruptSource3 0x0E02, Bits 4:4, typedef battery_removal_interrupt
//   
//  
#define BATTERY_REMOVAL_INTERRUPT_MASK 0x10U
#define BATTERY_REMOVAL_INTERRUPT_OFFSET 4
#define BATTERY_REMOVAL_INTERRUPT_STOP_BIT 4
#define BATTERY_REMOVAL_INTERRUPT_START_BIT 4
#define BATTERY_REMOVAL_INTERRUPT_WIDTH 1




//  
//  RTC timer reaches alarm time.
//  Register InterruptSource3 0x0E02, Bits 2:2, typedef reach_alarm_time
//   
//  
#define RTC_ALARM_MASK 0x4U
#define RTC_ALARM_OFFSET 2
#define RTC_ALARM_STOP_BIT 2
#define RTC_ALARM_START_BIT 2
#define RTC_ALARM_WIDTH 1




//  
//  BackUpBat ball voltage goes above RtcBackupChg register.
//  Register InterruptSource4 0x0E03, Bits 7:7, typedef rtc_battery_high_interrupt
//   
//  
#define RTC_BATTERY_HIGH_INTERRUPT_MASK 0x80U
#define RTC_BATTERY_HIGH_INTERRUPT_OFFSET 7
#define RTC_BATTERY_HIGH_INTERRUPT_STOP_BIT 7
#define RTC_BATTERY_HIGH_INTERRUPT_START_BIT 7
#define RTC_BATTERY_HIGH_INTERRUPT_WIDTH 1

#define INTERRUPT_SOURCE_4_REG 0xE03



//  
//  VbatA voltage reaches LowBat register programmed threshold.
//  Register InterruptSource4 0x0E03, Bits 5:5, typedef high_battery_interrupt
//   
//  
#define HIGH_BATTERY_INTERRUPT_MASK 0x20U
#define HIGH_BATTERY_INTERRUPT_OFFSET 5
#define HIGH_BATTERY_INTERRUPT_STOP_BIT 5
#define HIGH_BATTERY_INTERRUPT_START_BIT 5
#define HIGH_BATTERY_INTERRUPT_WIDTH 1



//  
//  Audio interrupt form audio digital part detected.
//  Register InterruptSource4 0x0E03, Bits 1:1, typedef audio_interrupt
//   
//  
#define AUDIO_INTERRUPT_MASK 0x2U
#define AUDIO_INTERRUPT_OFFSET 1
#define AUDIO_INTERRUPT_STOP_BIT 1
#define AUDIO_INTERRUPT_START_BIT 1
#define AUDIO_INTERRUPT_WIDTH 1


#define INTERRUPT_SOURCE_5_REG 0xE04

//  
//  AccDetect2 ball voltage level reaches AccDetect21Th.
//  Register InterruptSource5 0x0E04, Bits 6:6, typedef accessory21_high_voltage
//   
//  
#define ACCESSORY_21_HIGH_VOLTAGE_MASK 0x40U
#define ACCESSORY_21_HIGH_VOLTAGE_OFFSET 6
#define ACCESSORY_21_HIGH_VOLTAGE_STOP_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_START_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_WIDTH 1




//  
//  AccDetect2 ball voltage level reaches AccDetect22Th.
//  Register InterruptSource5 0x0E04, Bits 4:4, typedef accessory22_high_voltage
//   
//  
#define ACCESSORY_22_HIGH_VOLTAGE_MASK 0x10U
#define ACCESSORY_22_HIGH_VOLTAGE_OFFSET 4
#define ACCESSORY_22_HIGH_VOLTAGE_STOP_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_START_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_WIDTH 1




//  
//  AccDetect1 ball voltage level is below AccDetect1Th.
//  Register InterruptSource5 0x0E04, Bits 2:2, typedef accessory1_low_voltage
//   
//  
#define ACCESSORY_1_LOW_VOLTAGE_MASK 0x2U
#define ACCESSORY_1_LOW_VOLTAGE_OFFSET 1
#define ACCESSORY_1_LOW_VOLTAGE_STOP_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_START_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_WIDTH 1



#define INTERRUPT_SOURCE_6_REG 0xE05

//  
//  Edge detected on GPIO6 ball.
//  Register InterruptSource7 0x0E06, Bits 0:0, typedef gpio6_rising_event
//   
//  
#define GPIO_6_RISING_MASK 0x1U
#define GPIO_6_RISING_OFFSET 0
#define GPIO_6_RISING_STOP_BIT 0
#define GPIO_6_RISING_START_BIT 0
#define GPIO_6_RISING_WIDTH 1

#define INTERRUPT_SOURCE_7_REG 0xE06



//  
//  Edge detected on GPIO7 ball.
//  Register InterruptSource7 0x0E06, Bits 1:1, typedef gpio7_rising_event
//   
//  
#define GPIO_7_RISING_MASK 0x2U
#define GPIO_7_RISING_OFFSET 1
#define GPIO_7_RISING_STOP_BIT 1
#define GPIO_7_RISING_START_BIT 1
#define GPIO_7_RISING_WIDTH 1




//  
//  Edge detected on GPIO8 ball.
//  Register InterruptSource7 0x0E06, Bits 2:2, typedef gpio8_rising_event
//   
//  
#define GPIO_8_RISING_MASK 0x4U
#define GPIO_8_RISING_OFFSET 2
#define GPIO_8_RISING_STOP_BIT 2
#define GPIO_8_RISING_START_BIT 2
#define GPIO_8_RISING_WIDTH 1




//  
//  Edge detected on GPIO9 ball.
//  Register InterruptSource7 0x0E06, Bits 3:3, typedef gpio9_rising_event
//   
//  
#define GPIO_9_RISING_MASK 0x8U
#define GPIO_9_RISING_OFFSET 3
#define GPIO_9_RISING_STOP_BIT 3
#define GPIO_9_RISING_START_BIT 3
#define GPIO_9_RISING_WIDTH 1




//  
//  Edge detected on GPIO10 ball.
//  Register InterruptSource7 0x0E06, Bits 4:4, typedef gpio10_rising_event
//   
//  
#define GPIO_10_RISING_MASK 0x10U
#define GPIO_10_RISING_OFFSET 4
#define GPIO_10_RISING_STOP_BIT 4
#define GPIO_10_RISING_START_BIT 4
#define GPIO_10_RISING_WIDTH 1




//  
//  Edge detected on GPIO11 ball.
//  Register InterruptSource7 0x0E06, Bits 5:5, typedef gpio11_rising_event
//   
//  
#define GPIO_11_RISING_MASK 0x20U
#define GPIO_11_RISING_OFFSET 5
#define GPIO_11_RISING_STOP_BIT 5
#define GPIO_11_RISING_START_BIT 5
#define GPIO_11_RISING_WIDTH 1




//  
//  Edge detected on GPIO12 ball.
//  Register InterruptSource7 0x0E06, Bits 6:6, typedef gpio12_rising_event
//   
//  
#define GPIO_12_RISING_MASK 0x40U
#define GPIO_12_RISING_OFFSET 6
#define GPIO_12_RISING_STOP_BIT 6
#define GPIO_12_RISING_START_BIT 6
#define GPIO_12_RISING_WIDTH 1




//  
//  Edge detected on GPIO13 ball.
//  Register InterruptSource7 0x0E06, Bits 7:7, typedef gpio13_rising_event
//   
//  
#define GPIO_13_RISING_MASK 0x80U
#define GPIO_13_RISING_OFFSET 7
#define GPIO_13_RISING_STOP_BIT 7
#define GPIO_13_RISING_START_BIT 7
#define GPIO_13_RISING_WIDTH 1




//  
//  Edge detected on GPIO24 ball.
//  Register InterruptSource8 0x0E07, Bits 0:0, typedef gpio24_rising_event
//   
//  
#define GPIO_24_RISING_MASK 0x1U
#define GPIO_24_RISING_OFFSET 0
#define GPIO_24_RISING_STOP_BIT 0
#define GPIO_24_RISING_START_BIT 0
#define GPIO_24_RISING_WIDTH 1

#define INTERRUPT_SOURCE_8_REG 0xE07



//  
//  Edge detected on GPIO25 ball.
//  Register InterruptSource8 0x0E07, Bits 1:1, typedef gpio25_rising_event
//   
//  
#define GPIO_25_RISING_MASK 0x2U
#define GPIO_25_RISING_OFFSET 1
#define GPIO_25_RISING_STOP_BIT 1
#define GPIO_25_RISING_START_BIT 1
#define GPIO_25_RISING_WIDTH 1




//  
//  Edge detected on GPIO36 ball.
//  Register InterruptSource8 0x0E07, Bits 2:2, typedef gpio36_rising_event
//   
//  
#define GPIO_36_RISING_MASK 0x4U
#define GPIO_36_RISING_OFFSET 2
#define GPIO_36_RISING_STOP_BIT 2
#define GPIO_36_RISING_START_BIT 2
#define GPIO_36_RISING_WIDTH 1




//  
//  Edge detected on GPIO37 ball.
//  Register InterruptSource8 0x0E07, Bits 3:3, typedef gpio37_rising_event
//   
//  
#define GPIO_37_RISING_MASK 0x8U
#define GPIO_37_RISING_OFFSET 3
#define GPIO_37_RISING_STOP_BIT 3
#define GPIO_37_RISING_START_BIT 3
#define GPIO_37_RISING_WIDTH 1




//  
//  Edge detected on GPIO38 ball.
//  Register InterruptSource8 0x0E07, Bits 4:4, typedef gpio38_rising_event
//   
//  
#define GPIO_38_RISING_MASK 0x10U
#define GPIO_38_RISING_OFFSET 4
#define GPIO_38_RISING_STOP_BIT 4
#define GPIO_38_RISING_START_BIT 4
#define GPIO_38_RISING_WIDTH 1




//  
//  Edge detected on GPIO39 ball.
//  Register InterruptSource8 0x0E07, Bits 5:5, typedef gpio39_rising_event
//   
//  
#define GPIO_39_RISING_MASK 0x20U
#define GPIO_39_RISING_OFFSET 5
#define GPIO_39_RISING_STOP_BIT 5
#define GPIO_39_RISING_START_BIT 5
#define GPIO_39_RISING_WIDTH 1




//  
//  Edge detected on GPIO40 ball.
//  Register InterruptSource8 0x0E07, Bits 6:6, typedef gpio40_rising_event
//   
//  
#define GPIO_40_RISING_MASK 0x40U
#define GPIO_40_RISING_OFFSET 6
#define GPIO_40_RISING_STOP_BIT 6
#define GPIO_40_RISING_START_BIT 6
#define GPIO_40_RISING_WIDTH 1




//  
//  Edge detected on GPIO41 ball.
//  Register InterruptSource8 0x0E07, Bits 7:7, typedef gpio41_rising_event
//   
//  
#define GPIO_41_RISING_MASK 0x80U
#define GPIO_41_RISING_OFFSET 7
#define GPIO_41_RISING_STOP_BIT 7
#define GPIO_41_RISING_START_BIT 7
#define GPIO_41_RISING_WIDTH 1




//  
//  0: inactive 
//  1: no detection after inactive no detection after 2046*32Khz clock cycles (about 
//  62ms).
//  Register InterruptSource12 0x0E0B, Bits 0:0, typedef adp_source_error
//   
//  
#define ADP_SOURCE_ERROR_MASK 0x1U
#define ADP_SOURCE_ERROR_OFFSET 0
#define ADP_SOURCE_ERROR_STOP_BIT 0
#define ADP_SOURCE_ERROR_START_BIT 0
#define ADP_SOURCE_ERROR_WIDTH 1

#define INTERRUPT_SOURCE_12_REG 0xE0B



//  
//  0: inactive 
//  1: no detection after 2046*32Khz clock cycles (about 62ms).
//  Register InterruptSource12 0x0E0B, Bits 1:1, typedef adp_sink_error
//   
//  
#define ADP_SINK_ERROR_MASK 0x2U
#define ADP_SINK_ERROR_OFFSET 1
#define ADP_SINK_ERROR_STOP_BIT 1
#define ADP_SINK_ERROR_START_BIT 1
#define ADP_SINK_ERROR_WIDTH 1




//  
//  0: previous state 
//  1: accessory plug on Vbus.
//  Register InterruptSource12 0x0E0B, Bits 2:2, typedef adp_probe_plug
//   
//  
#define ADP_PROBE_PLUG_MASK 0x4U
#define ADP_PROBE_PLUG_OFFSET 2
#define ADP_PROBE_PLUG_STOP_BIT 2
#define ADP_PROBE_PLUG_START_BIT 2
#define ADP_PROBE_PLUG_WIDTH 1




//  
//  0: previous state 
//  1: accessory unplug on Vbus.
//  Register InterruptSource12 0x0E0B, Bits 3:3, typedef adp_probe_un_plug
//   
//  
#define ADP_PROBE_UN_PLUG_MASK 0x8U
#define ADP_PROBE_UN_PLUG_OFFSET 3
#define ADP_PROBE_UN_PLUG_STOP_BIT 3
#define ADP_PROBE_UN_PLUG_START_BIT 3
#define ADP_PROBE_UN_PLUG_WIDTH 1




//  
//  0: Current source Off 
//  1: Current source On.
//  Register InterruptSource12 0x0E0B, Bits 4:4, typedef current_source_on
//   
//  
#define ADP_SENSE_OFF_MASK 0x10U
#define ADP_SENSE_OFF_OFFSET 4
#define ADP_SENSE_OFF_STOP_BIT 4
#define ADP_SENSE_OFF_START_BIT 4
#define ADP_SENSE_OFF_WIDTH 1




//  
//  0: UsbLink status unchanged 
//  1: UsbLink status changed.
//  Register InterruptSource12 0x0E0B, Bits 7:7, typedef usb_link_status_changed
//   
//  
#define USB_LINK_STATUS_CHANGED_MASK 0x80U
#define USB_LINK_STATUS_CHANGED_OFFSET 7
#define USB_LINK_STATUS_CHANGED_STOP_BIT 7
#define USB_LINK_STATUS_CHANGED_START_BIT 7
#define USB_LINK_STATUS_CHANGED_WIDTH 1




//  
//  .
//  Register InterruptSource13 0x0E0C, Bits 3:3, typedef kp_vdddig_event
//   
//  
#define KP_VDDDIG_MASK 0x8U
#define KP_VDDDIG_OFFSET 3
#define KP_VDDDIG_STOP_BIT 3
#define KP_VDDDIG_START_BIT 3
#define KP_VDDDIG_WIDTH 1

#define INTERRUPT_SOURCE_13_REG 0xE0C



//  
//  0: Inactive 
//  1: Falling edge detected on GPIO50 ball.
//  Register InterruptSource13 0x0E0C, Bits 1:1, typedef gpio50_high_level_event
//   
//  
#define GPIO_50_HIGH_LEVEL_MASK 0x2U
#define GPIO_50_HIGH_LEVEL_OFFSET 1
#define GPIO_50_HIGH_LEVEL_STOP_BIT 1
#define GPIO_50_HIGH_LEVEL_START_BIT 1
#define GPIO_50_HIGH_LEVEL_WIDTH 1




//  
//  0: Inactive 
//  1: Falling edge detected on GPIO51 ball.
//  Register InterruptSource13 0x0E0C, Bits 2:2, typedef gpio51_high_level_event
//   
//  
#define GPIO_51_HIGH_LEVEL_MASK 0x4U
#define GPIO_51_HIGH_LEVEL_OFFSET 2
#define GPIO_51_HIGH_LEVEL_STOP_BIT 2
#define GPIO_51_HIGH_LEVEL_START_BIT 2
#define GPIO_51_HIGH_LEVEL_WIDTH 1




//  
//  0: Inactive 
//  1: Falling edge detected on GPIO52 ball.
//  Register InterruptSource13 0x0E0C, Bits 3:3, typedef gpio52_high_level_event
//   
//  
#define GPIO_52_HIGH_LEVEL_MASK 0x8U
#define GPIO_52_HIGH_LEVEL_OFFSET 3
#define GPIO_52_HIGH_LEVEL_STOP_BIT 3
#define GPIO_52_HIGH_LEVEL_START_BIT 3
#define GPIO_52_HIGH_LEVEL_WIDTH 1




//  
//  0: Inactive 
//  1: Falling edge detected on GPIO53 ball.
//  Register InterruptSource13 0x0E0C, Bits 4:4, typedef gpio53_high_level_event
//   
//  
#define GPIO_53_HIGH_LEVEL_MASK 0x10U
#define GPIO_53_HIGH_LEVEL_OFFSET 4
#define GPIO_53_HIGH_LEVEL_STOP_BIT 4
#define GPIO_53_HIGH_LEVEL_START_BIT 4
#define GPIO_53_HIGH_LEVEL_WIDTH 1




//  
//  0: Inactive 
//  1: Falling edge detected on GPIO60 ball.
//  Register InterruptSource13 0x0E0C, Bits 5:5, typedef gpio60_high_level_event
//   
//  
#define GPIO_60_HIGH_LEVEL_MASK 0x20U
#define GPIO_60_HIGH_LEVEL_OFFSET 5
#define GPIO_60_HIGH_LEVEL_STOP_BIT 5
#define GPIO_60_HIGH_LEVEL_START_BIT 5
#define GPIO_60_HIGH_LEVEL_WIDTH 1




//  
//  0: Inactive 
//  1: Rising edge detected on IEXTCHRFBN ball.
//  Register InterruptSource13 0x0E0C, Bits 6:6, typedef 
//  external_charger_high_level_event
//   
//  
#define HIGH_LEVEL_EXTERNAL_CHARGER_MASK 0x40U
#define HIGH_LEVEL_EXTERNAL_CHARGER_OFFSET 6
#define HIGH_LEVEL_EXTERNAL_CHARGER_STOP_BIT 6
#define HIGH_LEVEL_EXTERNAL_CHARGER_START_BIT 6
#define HIGH_LEVEL_EXTERNAL_CHARGER_WIDTH 1



//  
//  ID resistance greater than 220k detected.
//  Register InterruptSource20 0x0E13, Bits 7:7, typedef id_detection_greater330k
//   
//  
#define ID_DETECTION_R4_MASK 0x80U
#define ID_DETECTION_R4_OFFSET 7
#define ID_DETECTION_R4_STOP_BIT 7
#define ID_DETECTION_R4_START_BIT 7
#define ID_DETECTION_R4_WIDTH 1

#define INTERRUPT_SOURCE_20_REG 0xE13



//  
//  ID resistance greater than 125k detected.
//  Register InterruptSource20 0x0E13, Bits 6:6, typedef id_detection_greater220k
//   
//  
#define ID_DETECTION_R3_MASK 0x40U
#define ID_DETECTION_R3_OFFSET 6
#define ID_DETECTION_R3_STOP_BIT 6
#define ID_DETECTION_R3_START_BIT 6
#define ID_DETECTION_R3_WIDTH 1




//  
//  ID resistance greater than 69k detected.
//  Register InterruptSource20 0x0E13, Bits 5:5, typedef id_detection_greater140k
//   
//  
#define ID_DETECTION_R2_MASK 0x20U
#define ID_DETECTION_R2_OFFSET 5
#define ID_DETECTION_R2_STOP_BIT 5
#define ID_DETECTION_R2_START_BIT 5
#define ID_DETECTION_R2_WIDTH 1




//  
//  ID resistance greater than 37k detected.
//  Register InterruptSource20 0x0E13, Bits 4:4, typedef id_detection_greater80k
//   
//  
#define ID_DETECTION_R1_MASK 0x10U
#define ID_DETECTION_R1_OFFSET 4
#define ID_DETECTION_R1_STOP_BIT 4
#define ID_DETECTION_R1_START_BIT 4
#define ID_DETECTION_R1_WIDTH 1




//  
//  edge detected on ID.
//  Register InterruptSource20 0x0E13, Bits 2:2, typedef id_wake_up_rising_event
//   
//  
#define ID_WAKE_UP_RISING_MASK 0x4U
#define ID_WAKE_UP_RISING_OFFSET 2
#define ID_WAKE_UP_RISING_STOP_BIT 2
#define ID_WAKE_UP_RISING_START_BIT 2
#define ID_WAKE_UP_RISING_WIDTH 1




//  
//  Not allowed USB charger detected.
//  Register InterruptSource20 0x0E13, Bits 1:1, typedef usb_bad_charger_plug_event
//   
//  
#define USB_BAD_CHARGER_PLUG_MASK 0x2U
#define USB_BAD_CHARGER_PLUG_OFFSET 1
#define USB_BAD_CHARGER_PLUG_STOP_BIT 1
#define USB_BAD_CHARGER_PLUG_START_BIT 1
#define USB_BAD_CHARGER_PLUG_WIDTH 1




//  
//  0: Inactive 
//  1:SRP detected.
//  Register InterruptSource20 0x0E13, Bits 0:0, typedef srp_detected
//   
//  
#define SRP_P_DETECT_MASK 0x1U
#define SRP_P_DETECT_OFFSET 0
#define SRP_P_DETECT_STOP_BIT 0
#define SRP_P_DETECT_START_BIT 0
#define SRP_P_DETECT_WIDTH 1



//  
//  0: Die temperature is not upper than usb charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than usb charger thermal protection threshold (charge 
//  stopped).
//  Register InterruptSource22 0x0E15, Bits 1:1, typedef usb_charger_thermal_protection
//   
//  
#define USB_CHARGER_THERMAL_PROTECTION_MASK 0x2U
#define USB_CHARGER_THERMAL_PROTECTION_OFFSET 1
#define USB_CHARGER_THERMAL_PROTECTION_STOP_BIT 1
#define USB_CHARGER_THERMAL_PROTECTION_START_BIT 1
#define USB_CHARGER_THERMAL_PROTECTION_WIDTH 1

#define INTERRUPT_SOURCE_22_REG 0xE15



//  
//  0: Die temperature is not upper than main charger thermal protection threshold 
//  (charge stopped) 
//  1: Die temperature is upper than main charger thermal protection threshold (charge 
//  stopped).
//  Register InterruptSource22 0x0E15, Bits 3:3, typedef main_charger_thermal_protection
//   
//  
#define MAIN_CHARGER_THERMAL_PROTECTION_MASK 0x8U
#define MAIN_CHARGER_THERMAL_PROTECTION_OFFSET 3
#define MAIN_CHARGER_THERMAL_PROTECTION_STOP_BIT 3
#define MAIN_CHARGER_THERMAL_PROTECTION_START_BIT 3
#define MAIN_CHARGER_THERMAL_PROTECTION_WIDTH 1




//  
//  0: inactive 
//  1: charging current is limited in HS or Chirp modes..
//  Register InterruptSource22 0x0E15, Bits 6:6, typedef 
//  charging_current_limited_hs_chirp_enable
//   
//  
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK 0x40U
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_OFFSET 6
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_STOP_BIT 6
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_START_BIT 6
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_WIDTH 1


typedef enum charging_current_limited_hs_chirp_enable {
    CHARGING_CURRENT_LIMITED_HS_CHIRP_DISABLE_E,
    CHARGING_CURRENT_LIMITED_HS_CHIRP_ENABLE_E
} CHARGING_CURRENT_LIMITED_HS_CHIRP_ENABLE_T ;
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_PARAM_MASK  0x40



//  
//  0: XTAL starting time is more than 250ms (125ms in MPW05/06) and XTAL clock is not 
//  selected as internal clock. Or Xtal failure during run time. 
//  1: XTAL is running and starting time is lower than 250ms (125ms in 
//  MPW05/06)..
//  Register InterruptSource22 0x0E15, Bits 7:7, typedef xtal32_ok
//   
//  
#define XTAL_32_OK_MASK 0x80U
#define XTAL_32_OK_OFFSET 7
#define XTAL_32_OK_STOP_BIT 7
#define XTAL_32_OK_START_BIT 7
#define XTAL_32_OK_WIDTH 1


typedef enum xtal32_ok {
    XTAL_CLOCK_IS_NOT_SELECTED_E,
    XTAL_IS_RUNNING_E
} XTAL_32_OK_T ;
#define XTAL_32_OK_PARAM_MASK  0x80



//  
//  .
//  Register InterruptSource25 0x0E18, Bits 0:0, typedef key_stuck_event
//   
//  
#define KEY_STUCK_MASK 0x1U
#define KEY_STUCK_OFFSET 0
#define KEY_STUCK_STOP_BIT 0
#define KEY_STUCK_START_BIT 0
#define KEY_STUCK_WIDTH 1

#define INTERRUPT_SOURCE_25_REG 0xE18



//  
//  .
//  Register InterruptSource25 0x0E18, Bits 1:1, typedef lpr_vdddig_event
//   
//  
#define LPR_VDDDIG_MASK 0x2U
#define LPR_VDDDIG_OFFSET 1
#define LPR_VDDDIG_STOP_BIT 1
#define LPR_VDDDIG_START_BIT 1
#define LPR_VDDDIG_WIDTH 1




//  
//  .
//  Register InterruptSource25 0x0E18, Bits 2:2, typedef l_kp_vdddig_event
//   
//  
#define L_KP_VDDDIG_MASK 0x4U
#define L_KP_VDDDIG_OFFSET 2
#define L_KP_VDDDIG_STOP_BIT 2
#define L_KP_VDDDIG_START_BIT 2
#define L_KP_VDDDIG_WIDTH 1




//  
//  .
//  Register InterruptSource25 0x0E18, Bits 4:4, typedef key_deglitch_event
//   
//  
#define KEY_DEGLITCH_MASK 0x10U
#define KEY_DEGLITCH_OFFSET 4
#define KEY_DEGLITCH_STOP_BIT 4
#define KEY_DEGLITCH_START_BIT 4
#define KEY_DEGLITCH_WIDTH 1




//  
//  .
//  Register InterruptSource25 0x0E18, Bits 6:7, typedef modem_power_status_event
//   
//  
#define MODEM_POWER_STATUS_MASK 0xC0U
#define MODEM_POWER_STATUS_OFFSET 7
#define MODEM_POWER_STATUS_STOP_BIT 7
#define MODEM_POWER_STATUS_START_BIT 6
#define MODEM_POWER_STATUS_WIDTH 2



#endif
