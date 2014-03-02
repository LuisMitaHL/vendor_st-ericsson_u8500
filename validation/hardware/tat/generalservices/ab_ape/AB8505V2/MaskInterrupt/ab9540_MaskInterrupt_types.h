/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_MaskInterrupt/Linux/ab9540_MaskInterrupt_types.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_MASKINTERRUPT_LINUX_AB9540_MASKINTERRUPT_TYPES_H
#define C__LISP_PRODUCTION_AB9540_AB9540_MASKINTERRUPT_LINUX_AB9540_MASKINTERRUPT_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


//  
//  Rising edge on PonKey1 ball masked.
//  Register MaskedInterrupt1 0x0E40, Bits 7:7, typedef rising_pon_key1_mask_set
//   
//  
#define RISING_PON_KEY_1_MASKED_INTERRUPT_MASK 0x80U
#define RISING_PON_KEY_1_MASKED_INTERRUPT_OFFSET 7
#define RISING_PON_KEY_1_MASKED_INTERRUPT_STOP_BIT 7
#define RISING_PON_KEY_1_MASKED_INTERRUPT_START_BIT 7
#define RISING_PON_KEY_1_MASKED_INTERRUPT_WIDTH 1

#define MASKED_INTERRUPT_1_REG 0xE40

typedef enum rising_pon_key1_mask_set {
    RISING_PON_KEY_1_MASK_NO_ACTION_E,
    RISING_PON_KEY_1_MASK_ENABLE_E
} RISING_PON_KEY_1_MASK_SET_T ;
#define RISING_PON_KEY_1_MASKED_INTERRUPT_PARAM_MASK  0x80



//  
//  Falling edge on PonKey1 ball masked.
//  Register MaskedInterrupt1 0x0E40, Bits 6:6, typedef falling_pon_key1_mask_set
//   
//  
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_MASK 0x40U
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_OFFSET 6
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_STOP_BIT 6
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_START_BIT 6
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_WIDTH 1


typedef enum falling_pon_key1_mask_set {
    FALLING_PON_KEY_1_MASK_NO_ACTION_E,
    FALLING_PON_KEY_1_MASK_ENABLE_E
} FALLING_PON_KEY_1_MASK_SET_T ;
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_PARAM_MASK  0x40



//  
//  Rising edge on PonKey2 ball masked.
//  Register MaskedInterrupt1 0x0E40, Bits 5:5, typedef rising_pon_key2_mask_set
//   
//  
#define RISING_PON_KEY_2_MASKED_INTERRUPT_MASK 0x20U
#define RISING_PON_KEY_2_MASKED_INTERRUPT_OFFSET 5
#define RISING_PON_KEY_2_MASKED_INTERRUPT_STOP_BIT 5
#define RISING_PON_KEY_2_MASKED_INTERRUPT_START_BIT 5
#define RISING_PON_KEY_2_MASKED_INTERRUPT_WIDTH 1


typedef enum rising_pon_key2_mask_set {
    RISING_PON_KEY_2_MASK_NO_ACTION_E,
    RISING_PON_KEY_2_MASK_ENABLE_E
} RISING_PON_KEY_2_MASK_SET_T ;
#define RISING_PON_KEY_2_MASKED_INTERRUPT_PARAM_MASK  0x20



//  
//  Falling edge on PonKey2 ball masked.
//  Register MaskedInterrupt1 0x0E40, Bits 4:4, typedef falling_pon_key2_mask_set
//   
//  
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_MASK 0x10U
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_OFFSET 4
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_STOP_BIT 4
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_START_BIT 4
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_WIDTH 1


typedef enum falling_pon_key2_mask_set {
    FALLING_PON_KEY_2_MASK_NO_ACTION_E,
    FALLING_PON_KEY_2_MASK_ENABLE_E
} FALLING_PON_KEY_2_MASK_SET_T ;
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_PARAM_MASK  0x10



//  
//  Thermal warning masked.
//  Register MaskedInterrupt1 0x0E40, Bits 3:3, typedef thermal_warning_mask_set
//   
//  
#define THERMAL_WARNING_MASKED_INTERRUPT_MASK 0x8U
#define THERMAL_WARNING_MASKED_INTERRUPT_OFFSET 3
#define THERMAL_WARNING_MASKED_INTERRUPT_STOP_BIT 3
#define THERMAL_WARNING_MASKED_INTERRUPT_START_BIT 3
#define THERMAL_WARNING_MASKED_INTERRUPT_WIDTH 1


typedef enum thermal_warning_mask_set {
    THERMAL_WARNING_MASK_NO_ACTION_E,
    THERMAL_WARNING_MASK_ENABLE_E
} THERMAL_WARNING_MASK_SET_T ;
#define THERMAL_WARNING_MASKED_INTERRUPT_PARAM_MASK  0x8



//  
//  Tvset (75 Ohms) connection on CVBS ball detected masked.
//  Register MaskedInterrupt1 0x0E40, Bits 2:2, typedef plug_tv_mask_set
//   
//  
#define PLUG_TV_MASKED_INTERRUPT_MASK 0x4U
#define PLUG_TV_MASKED_INTERRUPT_OFFSET 2
#define PLUG_TV_MASKED_INTERRUPT_STOP_BIT 2
#define PLUG_TV_MASKED_INTERRUPT_START_BIT 2
#define PLUG_TV_MASKED_INTERRUPT_WIDTH 1


typedef enum plug_tv_mask_set {
    PLUG_TV_MASK_NO_ACTION_E,
    PLUG_TV_MASK_ENABLE_E
} PLUG_TV_MASK_SET_T ;
#define PLUG_TV_MASKED_INTERRUPT_PARAM_MASK  0x4



//  
//  Tvset (75 Ohms) disconnection on CVBS ball detected masked.
//  Register MaskedInterrupt1 0x0E40, Bits 1:1, typedef un_plug_tv_mask_set
//   
//  
#define UN_PLUG_TV_MASKED_INTERRUPT_MASK 0x2U
#define UN_PLUG_TV_MASKED_INTERRUPT_OFFSET 1
#define UN_PLUG_TV_MASKED_INTERRUPT_STOP_BIT 1
#define UN_PLUG_TV_MASKED_INTERRUPT_START_BIT 1
#define UN_PLUG_TV_MASKED_INTERRUPT_WIDTH 1


typedef enum un_plug_tv_mask_set {
    UN_PLUG_TV_MASK_NO_ACTION_E,
    UN_PLUG_TV_MASK_ENABLE_E
} UN_PLUG_TV_MASK_SET_T ;
#define UN_PLUG_TV_MASKED_INTERRUPT_PARAM_MASK  0x2



//  
//  Main charger connected is not an allowed Interupt mask.
//  Register MaskedInterrupt1 0x0E40, Bits 0:0, typedef main_charger_allowed_mask_set
//   
//  
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_MASK 0x1U
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_OFFSET 0
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_STOP_BIT 0
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_START_BIT 0
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_WIDTH 1


typedef enum main_charger_allowed_mask_set {
    MAIN_CHARGER_ALLOWED_MASK_NO_ACTION_E,
    MAIN_CHARGER_ALLOWED_MASK_ENABLE_E
} MAIN_CHARGER_ALLOWED_MASK_SET_T ;
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_PARAM_MASK  0x1



//  
//  Rising edge on Vbus ball masked.
//  Register MaskedInterrupt2 0x0E41, Bits 7:7, typedef vbus_rising_edge_mask_set
//   
//  
#define VBUS_RISING_EDGE_MASK_MASK 0x80U
#define VBUS_RISING_EDGE_MASK_OFFSET 7
#define VBUS_RISING_EDGE_MASK_STOP_BIT 7
#define VBUS_RISING_EDGE_MASK_START_BIT 7
#define VBUS_RISING_EDGE_MASK_WIDTH 1

#define MASKED_INTERRUPT_2_REG 0xE41

typedef enum vbus_rising_edge_mask_set {
    VBUS_RISING_EDGE_MASK_NO_ACTION_E,
    VBUS_RISING_EDGE_MASK_ENABLE_E
} VBUS_RISING_EDGE_MASK_SET_T ;
#define VBUS_RISING_EDGE_MASK_PARAM_MASK  0x80



//  
//  Falling edge on Vbus ball masked.
//  Register MaskedInterrupt2 0x0E41, Bits 6:6, typedef vbus_falling_edge_mask_set
//   
//  
#define VBUS_FALLING_EDGE_MASK_MASK 0x40U
#define VBUS_FALLING_EDGE_MASK_OFFSET 6
#define VBUS_FALLING_EDGE_MASK_STOP_BIT 6
#define VBUS_FALLING_EDGE_MASK_START_BIT 6
#define VBUS_FALLING_EDGE_MASK_WIDTH 1


typedef enum vbus_falling_edge_mask_set {
    VBUS_FALLING_EDGE_MASK_NO_ACTION_E,
    VBUS_FALLING_EDGE_MASK_ENABLE_E
} VBUS_FALLING_EDGE_MASK_SET_T ;
#define VBUS_FALLING_EDGE_MASK_PARAM_MASK  0x40



//  
//  TVbatA ball goes upper over voltage threshold masked.
//  Register MaskedInterrupt2 0x0E41, Bits 0:0, typedef battery_over_voltage_mask_set
//   
//  
#define BATTERY_OVER_VOLTAGE_MASK_MASK 0x1U
#define BATTERY_OVER_VOLTAGE_MASK_OFFSET 0
#define BATTERY_OVER_VOLTAGE_MASK_STOP_BIT 0
#define BATTERY_OVER_VOLTAGE_MASK_START_BIT 0
#define BATTERY_OVER_VOLTAGE_MASK_WIDTH 1


typedef enum battery_over_voltage_mask_set {
    BATTERY_OVER_VOLTAGE_MASK_NO_ACTION_E,
    BATTERY_OVER_VOLTAGE_MASK_ENABLE_E
} BATTERY_OVER_VOLTAGE_MASK_SET_T ;
#define BATTERY_OVER_VOLTAGE_MASK_PARAM_MASK  0x1



//  
//  Icharge greater Icharge max programmed detected.
//  Register MaskedInterrupt3 0x0E42, Bits 7:7, typedef 
//  cain_charger_over_current_mask_set
//   
//  
#define MAIN_CHARGER_OVER_CURRENT_MASK_MASK 0x80U
#define MAIN_CHARGER_OVER_CURRENT_MASK_OFFSET 7
#define MAIN_CHARGER_OVER_CURRENT_MASK_STOP_BIT 7
#define MAIN_CHARGER_OVER_CURRENT_MASK_START_BIT 7
#define MAIN_CHARGER_OVER_CURRENT_MASK_WIDTH 1

#define MASKED_INTERRUPT_3_REG 0xE42

typedef enum cain_charger_over_current_mask_set {
    CAIN_CHARGER_OVER_CURRENT_MASK_NO_ACTION_E,
    CAIN_CHARGER_OVER_CURRENT_MASK_ENABLE_E
} CAIN_CHARGER_OVER_CURRENT_MASK_SET_T ;
#define MAIN_CHARGER_OVER_CURRENT_MASK_PARAM_MASK  0x80



//  
//  Overvoltage on Vbus ball detected.
//  Register MaskedInterrupt3 0x0E42, Bits 6:6, typedef overvoltage_on_vbus_l_mask_set
//   
//  
#define OVERVOLTAGE_ON_VBUS_MASK_MASK 0x40U
#define OVERVOLTAGE_ON_VBUS_MASK_OFFSET 6
#define OVERVOLTAGE_ON_VBUS_MASK_STOP_BIT 6
#define OVERVOLTAGE_ON_VBUS_MASK_START_BIT 6
#define OVERVOLTAGE_ON_VBUS_MASK_WIDTH 1


typedef enum overvoltage_on_vbus_l_mask_set {
    OVERVOLTAGE_OFF_E,
    OVERVOLTAGE_ON_E
} OVERVOLTAGE_ON_VBUS_L_MASK_SET_T ;
#define OVERVOLTAGE_ON_VBUS_MASK_PARAM_MASK  0x40



//  
//  Watchdog charger expiration detected.
//  Register MaskedInterrupt3 0x0E42, Bits 5:5, typedef 
//  watchdog_charger_expiration_mask_set
//   
//  
#define WATCHDOG_CHARGER_EXPIRATION_MASK_MASK 0x20U
#define WATCHDOG_CHARGER_EXPIRATION_MASK_OFFSET 5
#define WATCHDOG_CHARGER_EXPIRATION_MASK_STOP_BIT 5
#define WATCHDOG_CHARGER_EXPIRATION_MASK_START_BIT 5
#define WATCHDOG_CHARGER_EXPIRATION_MASK_WIDTH 1


typedef enum watchdog_charger_expiration_mask_set {
    WATCHDOG_CHARGER_EXPIRATION_MASK_NO_ACTION_E,
    WATCHDOG_CHARGER_EXPIRATION_MASK_ENABLE_E
} WATCHDOG_CHARGER_EXPIRATION_MASK_SET_T ;
#define WATCHDOG_CHARGER_EXPIRATION_MASK_PARAM_MASK  0x20



//  
//  Battery removal.
//  Register MaskedInterrupt3 0x0E42, Bits 4:4, typedef battery_removal_mask_set
//   
//  
#define BATTERY_REMOVAL_MASK_MASK 0x10U
#define BATTERY_REMOVAL_MASK_OFFSET 4
#define BATTERY_REMOVAL_MASK_STOP_BIT 4
#define BATTERY_REMOVAL_MASK_START_BIT 4
#define BATTERY_REMOVAL_MASK_WIDTH 1


typedef enum battery_removal_mask_set {
    BATTERY_REMOVAL_MASK_NO_ACTION_E,
    BATTERY_REMOVAL_MASK_ENABLE_E
} BATTERY_REMOVAL_MASK_SET_T ;
#define BATTERY_REMOVAL_MASK_PARAM_MASK  0x10



//  
//  RTC timer reaches alarm time.
//  Register MaskedInterrupt3 0x0E42, Bits 2:2, typedef reach_alarm_time_mask_set
//   
//  
#define RTC_ALARM_MASK_MASK 0x4U
#define RTC_ALARM_MASK_OFFSET 2
#define RTC_ALARM_MASK_STOP_BIT 2
#define RTC_ALARM_MASK_START_BIT 2
#define RTC_ALARM_MASK_WIDTH 1




//  
//  RTC timer reaches a 60s period.
//  Register MaskedInterrupt3 0x0E42, Bits 1:1, typedef period60s_mask_set
//   
//  
#define RTC_6_0S_MASK_MASK 0x2U
#define RTC_6_0S_MASK_OFFSET 1
#define RTC_6_0S_MASK_STOP_BIT 1
#define RTC_6_0S_MASK_START_BIT 1
#define RTC_6_0S_MASK_WIDTH 1




//  
//  Usb charger was in dropout and internal input current loop update current regulation 
//  to avoid usb charger to drop.
//  Register MaskedInterrupt3 0x0E42, Bits 0:0, typedef usb_drop_out_mask_set
//   
//  
#define USB_CHARGER_DROPOUT_MASK_MASK 0x1U
#define USB_CHARGER_DROPOUT_MASK_OFFSET 0
#define USB_CHARGER_DROPOUT_MASK_STOP_BIT 0
#define USB_CHARGER_DROPOUT_MASK_START_BIT 0
#define USB_CHARGER_DROPOUT_MASK_WIDTH 1




//  
//  BackUpBat ball voltage goes above RtcBackupChg register.
//  Register MaskedInterrupt4 0x0E43, Bits 7:7, typedef rtc_battery_high_mask_set
//   
//  
#define RTC_BATTERY_HIGH_MASK_MASK 0x80U
#define RTC_BATTERY_HIGH_MASK_OFFSET 7
#define RTC_BATTERY_HIGH_MASK_STOP_BIT 7
#define RTC_BATTERY_HIGH_MASK_START_BIT 7
#define RTC_BATTERY_HIGH_MASK_WIDTH 1

#define MASKED_INTERRUPT_4_REG 0xE43

typedef enum rtc_battery_high_mask_set {
    RTC_BATTERY_HIGH_MASK_NO_ACTION_E,
    RTC_BATTERY_HIGH_MASK_ENABLE_E
} RTC_BATTERY_HIGH_MASK_SET_T ;
#define RTC_BATTERY_HIGH_MASK_PARAM_MASK  0x80



//  
//  BackUpBat ball voltage goes below RtcBackupChg register.
//  Register MaskedInterrupt4 0x0E43, Bits 6:6, typedef rtc_battery_low_mask_set
//   
//  
#define RTC_BATTERY_LOW_MASK_MASK 0x40U
#define RTC_BATTERY_LOW_MASK_OFFSET 6
#define RTC_BATTERY_LOW_MASK_STOP_BIT 6
#define RTC_BATTERY_LOW_MASK_START_BIT 6
#define RTC_BATTERY_LOW_MASK_WIDTH 1


typedef enum rtc_battery_low_mask_set {
    RTC_BATTERY_LOW_MASK_NO_ACTION_E,
    RTC_BATTERY_LOW_MASK_ENABLE_E
} RTC_BATTERY_LOW_MASK_SET_T ;
#define RTC_BATTERY_LOW_MASK_PARAM_MASK  0x40



//  
//  VbatA voltage goes above LowBat register programmed threshold.
//  Register MaskedInterrupt4 0x0E43, Bits 5:5, typedef high_battery_mask_set
//   
//  
#define HIGH_BATTERY_MASK_MASK 0x20U
#define HIGH_BATTERY_MASK_OFFSET 5
#define HIGH_BATTERY_MASK_STOP_BIT 5
#define HIGH_BATTERY_MASK_START_BIT 5
#define HIGH_BATTERY_MASK_WIDTH 1


typedef enum high_battery_mask_set {
    HIGH_BATTERY_MASK_NO_ACTION_E,
    HIGH_BATTERY_MASK_ENABLE_E
} HIGH_BATTERY_MASK_SET_T ;
#define HIGH_BATTERY_MASK_PARAM_MASK  0x20



//  
//  VbatA voltage goes below LowBat register programmed threshold.
//  Register MaskedInterrupt4 0x0E43, Bits 4:4, typedef low_battery_mask_set
//   
//  
#define LOW_BATTERY_MASK_MASK 0x10U
#define LOW_BATTERY_MASK_OFFSET 4
#define LOW_BATTERY_MASK_STOP_BIT 4
#define LOW_BATTERY_MASK_START_BIT 4
#define LOW_BATTERY_MASK_WIDTH 1


typedef enum low_battery_mask_set {
    LOW_BATTERY_MASK_NO_ACTION_E,
    LOW_BATTERY_MASK_ENABLE_E
} LOW_BATTERY_MASK_SET_T ;
#define LOW_BATTERY_MASK_PARAM_MASK  0x10



//  
//  Coulomb Counter has ended its calibration.
//  Register MaskedInterrupt4 0x0E43, Bits 3:3, typedef 
//  coulomb_calibration_endedd_mask_set
//   
//  
#define COULOMB_CALIBRATION_ENDEDD_MASK_MASK 0x8U
#define COULOMB_CALIBRATION_ENDEDD_MASK_OFFSET 3
#define COULOMB_CALIBRATION_ENDEDD_MASK_STOP_BIT 3
#define COULOMB_CALIBRATION_ENDEDD_MASK_START_BIT 3
#define COULOMB_CALIBRATION_ENDEDD_MASK_WIDTH 1


typedef enum coulomb_calibration_endedd_mask_set {
    COULOMB_CALIBRATION_ENDEDD_MASK_NO_ACTION_E,
    COULOMB_CALIBRATION_ENDEDD_MASK_ENABLE_E
} COULOMB_CALIBRATION_ENDEDD_MASK_SET_T ;
#define COULOMB_CALIBRATION_ENDEDD_MASK_PARAM_MASK  0x8



//  
//  Coulomb Counter has ended data convertion.
//  Register MaskedInterrupt4 0x0E43, Bits 2:2, typedef coulomb_counter_ended_mask_set
//   
//  
#define COULOMB_ENDED_MASK_MASK 0x4U
#define COULOMB_ENDED_MASK_OFFSET 2
#define COULOMB_ENDED_MASK_STOP_BIT 2
#define COULOMB_ENDED_MASK_START_BIT 2
#define COULOMB_ENDED_MASK_WIDTH 1


typedef enum coulomb_counter_ended_mask_set {
    COULOMB_COUNTER_ENDED_MASK_NO_ACTION_E,
    COULOMB_COUNTER_ENDED_MASK_ENABLE_E
} COULOMB_COUNTER_ENDED_MASK_SET_T ;
#define COULOMB_ENDED_MASK_PARAM_MASK  0x4



//  
//  Audio interrupt form audio digital part detected.
//  Register MaskedInterrupt4 0x0E43, Bits 1:1, typedef audio_mask_set
//   
//  
#define AUDIO_MASK_MASK 0x2U
#define AUDIO_MASK_OFFSET 1
#define AUDIO_MASK_STOP_BIT 1
#define AUDIO_MASK_START_BIT 1
#define AUDIO_MASK_WIDTH 1


typedef enum audio_mask_set {
    AUDIO_MASK_NO_ACTION_E,
    AUDIO_MASK_ENABLE_E
} AUDIO_MASK_SET_T ;
#define AUDIO_MASK_PARAM_MASK  0x2



//  
//  Accumulation of N sample conversion is detected.
//  Register MaskedInterrupt4 0x0E43, Bits 0:0, typedef accumulation_conversion_mask_set
//   
//  
#define ACCUMULATION_CONVERSION_MASK_MASK 0x1U
#define ACCUMULATION_CONVERSION_MASK_OFFSET 0
#define ACCUMULATION_CONVERSION_MASK_STOP_BIT 0
#define ACCUMULATION_CONVERSION_MASK_START_BIT 0
#define ACCUMULATION_CONVERSION_MASK_WIDTH 1


typedef enum accumulation_conversion_mask_set {
    ACCUMULATION_CONVERSION_MASK_NO_ACTION_E,
    ACCUMULATION_CONVERSION_MASK_ENABLE_E
} ACCUMULATION_CONVERSION_MASK_SET_T ;
#define ACCUMULATION_CONVERSION_MASK_PARAM_MASK  0x1



//  
//  GP ADC conversion requested thru Hardware control.
//  Register MaskedInterrupt5 0x0E44, Bits 7:7, typedef adc_hardware_control_mask_set
//   
//  
#define ADC_HARDWARE_CONTROL_MASK_MASK 0x80U
#define ADC_HARDWARE_CONTROL_MASK_OFFSET 7
#define ADC_HARDWARE_CONTROL_MASK_STOP_BIT 7
#define ADC_HARDWARE_CONTROL_MASK_START_BIT 7
#define ADC_HARDWARE_CONTROL_MASK_WIDTH 1

#define MASKED_INTERRUPT_5_REG 0xE44

typedef enum adc_hardware_control_mask_set {
    ADC_HARDWARE_CONTROL_MASK_NO_ACTION_E,
    ADC_HARDWARE_CONTROL_MASK_ENABLE_E
} ADC_HARDWARE_CONTROL_MASK_SET_T ;
#define ADC_HARDWARE_CONTROL_MASK_PARAM_MASK  0x80



//  
//  AccDetect2 ball voltage level goes above AccDetect21Th.
//  Register MaskedInterrupt5 0x0E44, Bits 6:6, typedef 
//  accessory21_high_voltage_mask_set
//   
//  
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_MASK 0x40U
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_OFFSET 6
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_STOP_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_START_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_WIDTH 1


typedef enum accessory21_high_voltage_mask_set {
    ACCESSORY_21_HIGH_VOLTAGE_MASK_NO_ACTION_E,
    ACCESSORY_21_HIGH_VOLTAGE_MASK_ENABLE_E
} ACCESSORY_21_HIGH_VOLTAGE_MASK_SET_T ;
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_PARAM_MASK  0x40



//  
//  AccDetect2 ball voltage level goes below AccDetect21Th.
//  Register MaskedInterrupt5 0x0E44, Bits 5:5, typedef accessory21_low_voltage_mask_set
//   
//  
#define ACCESSORY_21_LOW_VOLTAGE_MASK_MASK 0x20U
#define ACCESSORY_21_LOW_VOLTAGE_MASK_OFFSET 5
#define ACCESSORY_21_LOW_VOLTAGE_MASK_STOP_BIT 5
#define ACCESSORY_21_LOW_VOLTAGE_MASK_START_BIT 5
#define ACCESSORY_21_LOW_VOLTAGE_MASK_WIDTH 1


typedef enum accessory21_low_voltage_mask_set {
    ACCESSORY_21_LOW_VOLTAGE_MASK_NO_ACTION_E,
    ACCESSORY_21_LOW_VOLTAGE_MASK_ENABLE_E
} ACCESSORY_21_LOW_VOLTAGE_MASK_SET_T ;
#define ACCESSORY_21_LOW_VOLTAGE_MASK_PARAM_MASK  0x20



//  
//  AccDetect2 ball voltage level goes above AccDetect22Th.
//  Register MaskedInterrupt5 0x0E44, Bits 4:4, typedef 
//  accessory22_high_voltage_mask_set
//   
//  
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_MASK 0x10U
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_OFFSET 4
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_STOP_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_START_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_WIDTH 1


typedef enum accessory22_high_voltage_mask_set {
    ACCESSORY_22_HIGH_VOLTAGE_MASK_NO_ACTION_E,
    ACCESSORY_22_HIGH_VOLTAGE_MASK_ENABLE_E
} ACCESSORY_22_HIGH_VOLTAGE_MASK_SET_T ;
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_PARAM_MASK  0x10



//  
//  AccDetect2 ball voltage level goes below AccDetect22Th.
//  Register MaskedInterrupt5 0x0E44, Bits 3:3, typedef accessory22_low_voltage_mask_set
//   
//  
#define ACCESSORY_22_LOW_VOLTAGE_MASK_MASK 0x8U
#define ACCESSORY_22_LOW_VOLTAGE_MASK_OFFSET 3
#define ACCESSORY_22_LOW_VOLTAGE_MASK_STOP_BIT 3
#define ACCESSORY_22_LOW_VOLTAGE_MASK_START_BIT 3
#define ACCESSORY_22_LOW_VOLTAGE_MASK_WIDTH 1


typedef enum accessory22_low_voltage_mask_set {
    ACCESSORY_22_LOW_VOLTAGE_MASK_NO_ACTION_E,
    ACCESSORY_22_LOW_VOLTAGE_MASK_ENABLE_E
} ACCESSORY_22_LOW_VOLTAGE_MASK_SET_T ;
#define ACCESSORY_22_LOW_VOLTAGE_MASK_PARAM_MASK  0x8



//  
//  AccDetect1 ball voltage level goes above AccDetect1Th.
//  Register MaskedInterrupt5 0x0E44, Bits 2:2, typedef accessory1_high_voltage_mask_set
//   
//  
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_MASK 0x4U
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_OFFSET 2
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_STOP_BIT 2
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_START_BIT 2
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_WIDTH 1


typedef enum accessory1_high_voltage_mask_set {
    ACCESSORY_1_HIGH_VOLTAGE_MASK_NO_ACTION_E,
    ACCESSORY_1_HIGH_VOLTAGE_MASK_ENABLE_E
} ACCESSORY_1_HIGH_VOLTAGE_MASK_SET_T ;
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_PARAM_MASK  0x4



//  
//  AccDetect1 ball voltage level goes below AccDetect1Th.
//  Register MaskedInterrupt5 0x0E44, Bits 1:1, typedef accessory1_low_voltage_mask_set
//   
//  
#define ACCESSORY_1_LOW_VOLTAGE_MASK_MASK 0x2U
#define ACCESSORY_1_LOW_VOLTAGE_MASK_OFFSET 1
#define ACCESSORY_1_LOW_VOLTAGE_MASK_STOP_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_MASK_START_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_MASK_WIDTH 1


typedef enum accessory1_low_voltage_mask_set {
    ACCESSORY_1_LOW_VOLTAGE_MASK_NO_ACTION_E,
    ACCESSORY_1_LOW_VOLTAGE_MASK_ENABLE_E
} ACCESSORY_1_LOW_VOLTAGE_MASK_SET_T ;
#define ACCESSORY_1_LOW_VOLTAGE_MASK_PARAM_MASK  0x2



//  
//  GP ADC conversion requested thru software control is ended (data are 
//  available).
//  Register MaskedInterrupt5 0x0E44, Bits 0:0, typedef adc_software_control_mask_set
//   
//  
#define ADC_SOFTWARE_CONTROL_MASK_MASK 0x1U
#define ADC_SOFTWARE_CONTROL_MASK_OFFSET 0
#define ADC_SOFTWARE_CONTROL_MASK_STOP_BIT 0
#define ADC_SOFTWARE_CONTROL_MASK_START_BIT 0
#define ADC_SOFTWARE_CONTROL_MASK_WIDTH 1


typedef enum adc_software_control_mask_set {
    ADC_SOFTWARE_CONTROL_MASK_NO_ACTION_E,
    ADC_SOFTWARE_CONTROL_MASK_ENABLE_E
} ADC_SOFTWARE_CONTROL_MASK_SET_T ;
#define ADC_SOFTWARE_CONTROL_MASK_PARAM_MASK  0x1



//  
//  Rising edge Interrupt masked on GPIO6 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 0:0, typedef gpio6_rising_mask_set
//   
//  
#define GPIO_6_RISING_MASK_MASK 0x1U
#define GPIO_6_RISING_MASK_OFFSET 0
#define GPIO_6_RISING_MASK_STOP_BIT 0
#define GPIO_6_RISING_MASK_START_BIT 0
#define GPIO_6_RISING_MASK_WIDTH 1

#define MASKED_INTERRUPT_7_REG 0xE46

typedef enum gpio6_rising_mask_set {
    GPIO_6_RISING_MASK_NO_ACTION_E,
    GPIO_6_RISING_MASK_ENABLE_E
} GPIO_6_RISING_MASK_SET_T ;
#define GPIO_6_RISING_MASK_PARAM_MASK  0x1



//  
//  Rising edge Interrupt masked on GPIO7 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 1:1, typedef gpio7_rising_mask_set
//   
//  
#define GPIO_7_RISING_MASK_MASK 0x2U
#define GPIO_7_RISING_MASK_OFFSET 1
#define GPIO_7_RISING_MASK_STOP_BIT 1
#define GPIO_7_RISING_MASK_START_BIT 1
#define GPIO_7_RISING_MASK_WIDTH 1


typedef enum gpio7_rising_mask_set {
    GPIO_7_RISING_MASK_NO_ACTION_E,
    GPIO_7_RISING_MASK_ENABLE_E
} GPIO_7_RISING_MASK_SET_T ;
#define GPIO_7_RISING_MASK_PARAM_MASK  0x2



//  
//  Rising edge Interrupt masked on GPIO8 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 2:2, typedef gpio8_rising_mask_set
//   
//  
#define GPIO_8_RISING_MASK_MASK 0x4U
#define GPIO_8_RISING_MASK_OFFSET 2
#define GPIO_8_RISING_MASK_STOP_BIT 2
#define GPIO_8_RISING_MASK_START_BIT 2
#define GPIO_8_RISING_MASK_WIDTH 1


typedef enum gpio8_rising_mask_set {
    GPIO_8_RISING_MASK_NO_ACTION_E,
    GPIO_8_RISING_MASK_ENABLE_E
} GPIO_8_RISING_MASK_SET_T ;
#define GPIO_8_RISING_MASK_PARAM_MASK  0x4



//  
//  Rising edge Interrupt masked on GPIO9 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 3:3, typedef gpio9_rising_mask_set
//   
//  
#define GPIO_9_RISING_MASK_MASK 0x8U
#define GPIO_9_RISING_MASK_OFFSET 3
#define GPIO_9_RISING_MASK_STOP_BIT 3
#define GPIO_9_RISING_MASK_START_BIT 3
#define GPIO_9_RISING_MASK_WIDTH 1


typedef enum gpio9_rising_mask_set {
    GPIO_9_RISING_MASK_NO_ACTION_E,
    GPIO_9_RISING_MASK_ENABLE_E
} GPIO_9_RISING_MASK_SET_T ;
#define GPIO_9_RISING_MASK_PARAM_MASK  0x8



//  
//  Rising edge Interrupt masked on GPIO10 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 4:4, typedef gpio10_rising_mask_set
//   
//  
#define GPIO_10_RISING_MASK_MASK 0x10U
#define GPIO_10_RISING_MASK_OFFSET 4
#define GPIO_10_RISING_MASK_STOP_BIT 4
#define GPIO_10_RISING_MASK_START_BIT 4
#define GPIO_10_RISING_MASK_WIDTH 1


typedef enum gpio10_rising_mask_set {
    GPIO_10_RISING_MASK_NO_ACTION_E,
    GPIO_10_RISING_MASK_ENABLE_E
} GPIO_10_RISING_MASK_SET_T ;
#define GPIO_10_RISING_MASK_PARAM_MASK  0x10



//  
//  Rising edge Interrupt masked on GPIO11 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 5:5, typedef gpio11_rising_mask_set
//   
//  
#define GPIO_11_RISING_MASK_MASK 0x20U
#define GPIO_11_RISING_MASK_OFFSET 5
#define GPIO_11_RISING_MASK_STOP_BIT 5
#define GPIO_11_RISING_MASK_START_BIT 5
#define GPIO_11_RISING_MASK_WIDTH 1


typedef enum gpio11_rising_mask_set {
    GPIO_11_RISING_MASK_NO_ACTION_E,
    GPIO_11_RISING_MASK_ENABLE_E
} GPIO_11_RISING_MASK_SET_T ;
#define GPIO_11_RISING_MASK_PARAM_MASK  0x20



//  
//  Rising edge Interrupt masked on GPIO12 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 6:6, typedef gpio12_rising_mask_set
//   
//  
#define GPIO_12_RISING_MASK_MASK 0x40U
#define GPIO_12_RISING_MASK_OFFSET 6
#define GPIO_12_RISING_MASK_STOP_BIT 6
#define GPIO_12_RISING_MASK_START_BIT 6
#define GPIO_12_RISING_MASK_WIDTH 1


typedef enum gpio12_rising_mask_set {
    GPIO_12_RISING_MASK_NO_ACTION_E,
    GPIO_12_RISING_MASK_ENABLE_E
} GPIO_12_RISING_MASK_SET_T ;
#define GPIO_12_RISING_MASK_PARAM_MASK  0x40



//  
//  Rising edge Interrupt masked on GPIO13 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 7:7, typedef gpio13_rising_mask_set
//   
//  
#define GPIO_13_RISING_MASK_MASK 0x80U
#define GPIO_13_RISING_MASK_OFFSET 7
#define GPIO_13_RISING_MASK_STOP_BIT 7
#define GPIO_13_RISING_MASK_START_BIT 7
#define GPIO_13_RISING_MASK_WIDTH 1


typedef enum gpio13_rising_mask_set {
    GPIO_13_RISING_MASK_NO_ACTION_E,
    GPIO_13_RISING_MASK_ENABLE_E
} GPIO_13_RISING_MASK_SET_T ;
#define GPIO_13_RISING_MASK_PARAM_MASK  0x80



//  
//  Rising edge Interrupt masked on GPIO24 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 0:0, typedef gpio24_rising_mask_set
//   
//  
#define GPIO_24_RISING_MASK_MASK 0x1U
#define GPIO_24_RISING_MASK_OFFSET 0
#define GPIO_24_RISING_MASK_STOP_BIT 0
#define GPIO_24_RISING_MASK_START_BIT 0
#define GPIO_24_RISING_MASK_WIDTH 1

#define MASKED_INTERRUPT_8_REG 0xE47

typedef enum gpio24_rising_mask_set {
    GPIO_24_RISING_MASK_NO_ACTION_E,
    GPIO_24_RISING_MASK_ENABLE_E
} GPIO_24_RISING_MASK_SET_T ;
#define GPIO_24_RISING_MASK_PARAM_MASK  0x1



//  
//  Rising edge Interrupt masked on GPIO25 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 1:1, typedef gpio25_rising_mask_set
//   
//  
#define GPIO_25_RISING_MASK_MASK 0x2U
#define GPIO_25_RISING_MASK_OFFSET 1
#define GPIO_25_RISING_MASK_STOP_BIT 1
#define GPIO_25_RISING_MASK_START_BIT 1
#define GPIO_25_RISING_MASK_WIDTH 1


typedef enum gpio25_rising_mask_set {
    GPIO_25_RISING_MASK_NO_ACTION_E,
    GPIO_25_RISING_MASK_ENABLE_E
} GPIO_25_RISING_MASK_SET_T ;
#define GPIO_25_RISING_MASK_PARAM_MASK  0x2



//  
//  Rising edge Interrupt masked on GPIO36 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 2:2, typedef gpio36_rising_mask_set
//   
//  
#define GPIO_36_RISING_MASK_MASK 0x4U
#define GPIO_36_RISING_MASK_OFFSET 2
#define GPIO_36_RISING_MASK_STOP_BIT 2
#define GPIO_36_RISING_MASK_START_BIT 2
#define GPIO_36_RISING_MASK_WIDTH 1


typedef enum gpio36_rising_mask_set {
    GPIO_36_RISING_MASK_NO_ACTION_E,
    GPIO_36_RISING_MASK_ENABLE_E
} GPIO_36_RISING_MASK_SET_T ;
#define GPIO_36_RISING_MASK_PARAM_MASK  0x4



//  
//  Rising edge Interrupt masked on GPIO37 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 3:3, typedef gpio37_rising_mask_set
//   
//  
#define GPIO_37_RISING_MASK_MASK 0x8U
#define GPIO_37_RISING_MASK_OFFSET 3
#define GPIO_37_RISING_MASK_STOP_BIT 3
#define GPIO_37_RISING_MASK_START_BIT 3
#define GPIO_37_RISING_MASK_WIDTH 1


typedef enum gpio37_rising_mask_set {
    GPIO_37_RISING_MASK_NO_ACTION_E,
    GPIO_37_RISING_MASK_ENABLE_E
} GPIO_37_RISING_MASK_SET_T ;
#define GPIO_37_RISING_MASK_PARAM_MASK  0x8



//  
//  Rising edge Interrupt masked on GPIO38 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 4:4, typedef gpio38_rising_mask_set
//   
//  
#define GPIO_38_RISING_MASK_MASK 0x10U
#define GPIO_38_RISING_MASK_OFFSET 4
#define GPIO_38_RISING_MASK_STOP_BIT 4
#define GPIO_38_RISING_MASK_START_BIT 4
#define GPIO_38_RISING_MASK_WIDTH 1


typedef enum gpio38_rising_mask_set {
    GPIO_38_RISING_MASK_NO_ACTION_E,
    GPIO_38_RISING_MASK_ENABLE_E
} GPIO_38_RISING_MASK_SET_T ;
#define GPIO_38_RISING_MASK_PARAM_MASK  0x10



//  
//  Rising edge Interrupt masked on GPIO39 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 5:5, typedef gpio39_rising_mask_set
//   
//  
#define GPIO_39_RISING_MASK_MASK 0x20U
#define GPIO_39_RISING_MASK_OFFSET 5
#define GPIO_39_RISING_MASK_STOP_BIT 5
#define GPIO_39_RISING_MASK_START_BIT 5
#define GPIO_39_RISING_MASK_WIDTH 1


typedef enum gpio39_rising_mask_set {
    GPIO_39_RISING_MASK_NO_ACTION_E,
    GPIO_39_RISING_MASK_ENABLE_E
} GPIO_39_RISING_MASK_SET_T ;
#define GPIO_39_RISING_MASK_PARAM_MASK  0x20



//  
//  Rising edge Interrupt masked on GPIO40 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 6:6, typedef gpio40_rising_mask_set
//   
//  
#define GPIO_40_RISING_MASK_MASK 0x40U
#define GPIO_40_RISING_MASK_OFFSET 6
#define GPIO_40_RISING_MASK_STOP_BIT 6
#define GPIO_40_RISING_MASK_START_BIT 6
#define GPIO_40_RISING_MASK_WIDTH 1


typedef enum gpio40_rising_mask_set {
    GPIO_40_RISING_MASK_NO_ACTION_E,
    GPIO_40_RISING_MASK_ENABLE_E
} GPIO_40_RISING_MASK_SET_T ;
#define GPIO_40_RISING_MASK_PARAM_MASK  0x40



//  
//  Rising edge Interrupt masked on GPIO41 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 7:7, typedef gpio41_rising_mask_set
//   
//  
#define GPIO_41_RISING_MASK_MASK 0x80U
#define GPIO_41_RISING_MASK_OFFSET 7
#define GPIO_41_RISING_MASK_STOP_BIT 7
#define GPIO_41_RISING_MASK_START_BIT 7
#define GPIO_41_RISING_MASK_WIDTH 1


typedef enum gpio41_rising_mask_set {
    GPIO_41_RISING_MASK_NO_ACTION_E,
    GPIO_41_RISING_MASK_ENABLE_E
} GPIO_41_RISING_MASK_SET_T ;
#define GPIO_41_RISING_MASK_PARAM_MASK  0x80



//  
//  Falling edge Masked on GPIO6 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 0:0, typedef gpio6_falling_mask_set
//   
//  
#define GPIO_6_FALLING_MASK_MASK 0x1U
#define GPIO_6_FALLING_MASK_OFFSET 0
#define GPIO_6_FALLING_MASK_STOP_BIT 0
#define GPIO_6_FALLING_MASK_START_BIT 0
#define GPIO_6_FALLING_MASK_WIDTH 1

#define MASKED_INTERRUPT_9_REG 0xE48

typedef enum gpio6_falling_mask_set {
    GPIO_6_FALLING_MASK_NO_ACTION_E,
    GPIO_6_FALLING_MASK_ENABLE_E
} GPIO_6_FALLING_MASK_SET_T ;
#define GPIO_6_FALLING_MASK_PARAM_MASK  0x1



//  
//  Falling edge Masked on GPIO7 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 1:1, typedef gpio7_falling_mask_set
//   
//  
#define GPIO_7_FALLING_MASK_MASK 0x2U
#define GPIO_7_FALLING_MASK_OFFSET 1
#define GPIO_7_FALLING_MASK_STOP_BIT 1
#define GPIO_7_FALLING_MASK_START_BIT 1
#define GPIO_7_FALLING_MASK_WIDTH 1


typedef enum gpio7_falling_mask_set {
    GPIO_7_FALLING_MASK_NO_ACTION_E,
    GPIO_7_FALLING_MASK_ENABLE_E
} GPIO_7_FALLING_MASK_SET_T ;
#define GPIO_7_FALLING_MASK_PARAM_MASK  0x2



//  
//  Falling edge Masked on GPIO8 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 2:2, typedef gpio8_falling_mask_set
//   
//  
#define GPIO_8_FALLING_MASK_MASK 0x4U
#define GPIO_8_FALLING_MASK_OFFSET 2
#define GPIO_8_FALLING_MASK_STOP_BIT 2
#define GPIO_8_FALLING_MASK_START_BIT 2
#define GPIO_8_FALLING_MASK_WIDTH 1


typedef enum gpio8_falling_mask_set {
    GPIO_8_FALLING_MASK_NO_ACTION_E,
    GPIO_8_FALLING_MASK_ENABLE_E
} GPIO_8_FALLING_MASK_SET_T ;
#define GPIO_8_FALLING_MASK_PARAM_MASK  0x4



//  
//  Falling edge Masked on GPIO9 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 3:3, typedef gpio9_falling_mask_set
//   
//  
#define GPIO_9_FALLING_MASK_MASK 0x8U
#define GPIO_9_FALLING_MASK_OFFSET 3
#define GPIO_9_FALLING_MASK_STOP_BIT 3
#define GPIO_9_FALLING_MASK_START_BIT 3
#define GPIO_9_FALLING_MASK_WIDTH 1


typedef enum gpio9_falling_mask_set {
    GPIO_9_FALLING_MASK_NO_ACTION_E,
    GPIO_9_FALLING_MASK_ENABLE_E
} GPIO_9_FALLING_MASK_SET_T ;
#define GPIO_9_FALLING_MASK_PARAM_MASK  0x8



//  
//  Falling edge Masked on GPIO10 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 4:4, typedef gpio10_falling_mask_set
//   
//  
#define GPIO_10_FALLING_MASK_MASK 0x10U
#define GPIO_10_FALLING_MASK_OFFSET 4
#define GPIO_10_FALLING_MASK_STOP_BIT 4
#define GPIO_10_FALLING_MASK_START_BIT 4
#define GPIO_10_FALLING_MASK_WIDTH 1


typedef enum gpio10_falling_mask_set {
    GPIO_10_FALLING_MASK_NO_ACTION_E,
    GPIO_10_FALLING_MASK_ENABLE_E
} GPIO_10_FALLING_MASK_SET_T ;
#define GPIO_10_FALLING_MASK_PARAM_MASK  0x10



//  
//  Falling edge Masked on GPIO11 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 5:5, typedef gpio11_falling_mask_set
//   
//  
#define GPIO_11_FALLING_MASK_MASK 0x20U
#define GPIO_11_FALLING_MASK_OFFSET 5
#define GPIO_11_FALLING_MASK_STOP_BIT 5
#define GPIO_11_FALLING_MASK_START_BIT 5
#define GPIO_11_FALLING_MASK_WIDTH 1


typedef enum gpio11_falling_mask_set {
    GPIO_11_FALLING_MASK_NO_ACTION_E,
    GPIO_11_FALLING_MASK_ENABLE_E
} GPIO_11_FALLING_MASK_SET_T ;
#define GPIO_11_FALLING_MASK_PARAM_MASK  0x20



//  
//  Falling edge Masked on GPIO12 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 6:6, typedef gpio12_falling_mask_set
//   
//  
#define GPIO_12_FALLING_MASK_MASK 0x40U
#define GPIO_12_FALLING_MASK_OFFSET 6
#define GPIO_12_FALLING_MASK_STOP_BIT 6
#define GPIO_12_FALLING_MASK_START_BIT 6
#define GPIO_12_FALLING_MASK_WIDTH 1


typedef enum gpio12_falling_mask_set {
    GPIO_12_FALLING_MASK_NO_ACTION_E,
    GPIO_12_FALLING_MASK_ENABLE_E
} GPIO_12_FALLING_MASK_SET_T ;
#define GPIO_12_FALLING_MASK_PARAM_MASK  0x40



//  
//  Falling edge Masked on GPIO13 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 7:7, typedef gpio13_falling_mask_set
//   
//  
#define GPIO_13_FALLING_MASK_MASK 0x80U
#define GPIO_13_FALLING_MASK_OFFSET 7
#define GPIO_13_FALLING_MASK_STOP_BIT 7
#define GPIO_13_FALLING_MASK_START_BIT 7
#define GPIO_13_FALLING_MASK_WIDTH 1


typedef enum gpio13_falling_mask_set {
    GPIO_13_FALLING_MASK_NO_ACTION_E,
    GPIO_13_FALLING_MASK_ENABLE_E
} GPIO_13_FALLING_MASK_SET_T ;
#define GPIO_13_FALLING_MASK_PARAM_MASK  0x80



//  
//  Falling edge Masked on GPIO24 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 0:0, typedef gpio24_falling_mask_set
//   
//  
#define GPIO_24_FALLING_MASK_MASK 0x1U
#define GPIO_24_FALLING_MASK_OFFSET 0
#define GPIO_24_FALLING_MASK_STOP_BIT 0
#define GPIO_24_FALLING_MASK_START_BIT 0
#define GPIO_24_FALLING_MASK_WIDTH 1

#define MASKED_INTERRUPT_10_REG 0xE49

typedef enum gpio24_falling_mask_set {
    GPIO_24_FALLING_MASK_NO_ACTION_E,
    GPIO_24_FALLING_MASK_ENABLE_E
} GPIO_24_FALLING_MASK_SET_T ;
#define GPIO_24_FALLING_MASK_PARAM_MASK  0x1



//  
//  Falling edge Masked on GPIO25 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 1:1, typedef gpio25_falling_mask_set
//   
//  
#define GPIO_25_FALLING_MASK_MASK 0x2U
#define GPIO_25_FALLING_MASK_OFFSET 1
#define GPIO_25_FALLING_MASK_STOP_BIT 1
#define GPIO_25_FALLING_MASK_START_BIT 1
#define GPIO_25_FALLING_MASK_WIDTH 1


typedef enum gpio25_falling_mask_set {
    GPIO_25_FALLING_MASK_NO_ACTION_E,
    GPIO_25_FALLING_MASK_ENABLE_E
} GPIO_25_FALLING_MASK_SET_T ;
#define GPIO_25_FALLING_MASK_PARAM_MASK  0x2



//  
//  Falling edge Masked on GPIO36 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 2:2, typedef gpio36_falling_mask_set
//   
//  
#define GPIO_36_FALLING_MASK_MASK 0x4U
#define GPIO_36_FALLING_MASK_OFFSET 2
#define GPIO_36_FALLING_MASK_STOP_BIT 2
#define GPIO_36_FALLING_MASK_START_BIT 2
#define GPIO_36_FALLING_MASK_WIDTH 1


typedef enum gpio36_falling_mask_set {
    GPIO_36_FALLING_MASK_NO_ACTION_E,
    GPIO_36_FALLING_MASK_ENABLE_E
} GPIO_36_FALLING_MASK_SET_T ;
#define GPIO_36_FALLING_MASK_PARAM_MASK  0x4



//  
//  Falling edge Masked on GPIO37 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 3:3, typedef gpio37_falling_mask_set
//   
//  
#define GPIO_37_FALLING_MASK_MASK 0x8U
#define GPIO_37_FALLING_MASK_OFFSET 3
#define GPIO_37_FALLING_MASK_STOP_BIT 3
#define GPIO_37_FALLING_MASK_START_BIT 3
#define GPIO_37_FALLING_MASK_WIDTH 1


typedef enum gpio37_falling_mask_set {
    GPIO_37_FALLING_MASK_NO_ACTION_E,
    GPIO_37_FALLING_MASK_ENABLE_E
} GPIO_37_FALLING_MASK_SET_T ;
#define GPIO_37_FALLING_MASK_PARAM_MASK  0x8



//  
//  Falling edge Masked on GPIO38 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 4:4, typedef gpio38_falling_mask_set
//   
//  
#define GPIO_38_FALLING_MASK_MASK 0x10U
#define GPIO_38_FALLING_MASK_OFFSET 4
#define GPIO_38_FALLING_MASK_STOP_BIT 4
#define GPIO_38_FALLING_MASK_START_BIT 4
#define GPIO_38_FALLING_MASK_WIDTH 1


typedef enum gpio38_falling_mask_set {
    GPIO_38_FALLING_MASK_NO_ACTION_E,
    GPIO_38_FALLING_MASK_ENABLE_E
} GPIO_38_FALLING_MASK_SET_T ;
#define GPIO_38_FALLING_MASK_PARAM_MASK  0x10



//  
//  Falling edge Masked on GPIO39 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 5:5, typedef gpio39_falling_mask_set
//   
//  
#define GPIO_39_FALLING_MASK_MASK 0x20U
#define GPIO_39_FALLING_MASK_OFFSET 5
#define GPIO_39_FALLING_MASK_STOP_BIT 5
#define GPIO_39_FALLING_MASK_START_BIT 5
#define GPIO_39_FALLING_MASK_WIDTH 1


typedef enum gpio39_falling_mask_set {
    GPIO_39_FALLING_MASK_NO_ACTION_E,
    GPIO_39_FALLING_MASK_ENABLE_E
} GPIO_39_FALLING_MASK_SET_T ;
#define GPIO_39_FALLING_MASK_PARAM_MASK  0x20



//  
//  Falling edge Masked on GPIO40 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 6:6, typedef gpio40_falling_mask_set
//   
//  
#define GPIO_40_FALLING_MASK_MASK 0x40U
#define GPIO_40_FALLING_MASK_OFFSET 6
#define GPIO_40_FALLING_MASK_STOP_BIT 6
#define GPIO_40_FALLING_MASK_START_BIT 6
#define GPIO_40_FALLING_MASK_WIDTH 1


typedef enum gpio40_falling_mask_set {
    GPIO_40_FALLING_MASK_NO_ACTION_E,
    GPIO_40_FALLING_MASK_ENABLE_E
} GPIO_40_FALLING_MASK_SET_T ;
#define GPIO_40_FALLING_MASK_PARAM_MASK  0x40



//  
//  Falling edge Masked on GPIO41 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 7:7, typedef gpio41_falling_mask_set
//   
//  
#define GPIO_41_FALLING_MASK_MASK 0x80U
#define GPIO_41_FALLING_MASK_OFFSET 7
#define GPIO_41_FALLING_MASK_STOP_BIT 7
#define GPIO_41_FALLING_MASK_START_BIT 7
#define GPIO_41_FALLING_MASK_WIDTH 1


typedef enum gpio41_falling_mask_set {
    GPIO_41_FALLING_MASK_NO_ACTION_E,
    GPIO_41_FALLING_MASK_ENABLE_E
} GPIO_41_FALLING_MASK_SET_T ;
#define GPIO_41_FALLING_MASK_PARAM_MASK  0x80



//  
//  0: inactive 
//  1: no detection after inactive no detection after 2046*32Khzclock cycles.
//  Register MaskedInterrupt12 0x0E4B, Bits 0:0, typedef adp_source_error_mask
//   
//  
#define ADP_SOURCE_ERROR_MASK_MASK 0x1U
#define ADP_SOURCE_ERROR_MASK_OFFSET 0
#define ADP_SOURCE_ERROR_MASK_STOP_BIT 0
#define ADP_SOURCE_ERROR_MASK_START_BIT 0
#define ADP_SOURCE_ERROR_MASK_WIDTH 1

#define MASKED_INTERRUPT_12_REG 0xE4B



//  
//  0: inactive 
//  1: no detection after 2046*32Khzclock cycles.
//  Register MaskedInterrupt12 0x0E4B, Bits 1:1, typedef adp_sink_error_mask
//   
//  
#define ADP_SINK_ERROR_MASK_MASK 0x2U
#define ADP_SINK_ERROR_MASK_OFFSET 1
#define ADP_SINK_ERROR_MASK_STOP_BIT 1
#define ADP_SINK_ERROR_MASK_START_BIT 1
#define ADP_SINK_ERROR_MASK_WIDTH 1




//  
//  0: previous state 
//  1: accessory plug on Vbus.
//  Register MaskedInterrupt12 0x0E4B, Bits 2:2, typedef adp_probe_plug_mask
//   
//  
#define ADP_PROBE_PLUG_MASK_MASK 0x4U
#define ADP_PROBE_PLUG_MASK_OFFSET 2
#define ADP_PROBE_PLUG_MASK_STOP_BIT 2
#define ADP_PROBE_PLUG_MASK_START_BIT 2
#define ADP_PROBE_PLUG_MASK_WIDTH 1




//  
//  0: previous state 
//  1: accessory unplug on Vbus.
//  Register MaskedInterrupt12 0x0E4B, Bits 3:3, typedef adp_probe_un_plug_mask
//   
//  
#define ADP_PROBE_UN_PLUG_MASK_MASK 0x8U
#define ADP_PROBE_UN_PLUG_MASK_OFFSET 3
#define ADP_PROBE_UN_PLUG_MASK_STOP_BIT 3
#define ADP_PROBE_UN_PLUG_MASK_START_BIT 3
#define ADP_PROBE_UN_PLUG_MASK_WIDTH 1




//  
//  0: Current source Off 
//  1: Current source On.
//  Register MaskedInterrupt12 0x0E4B, Bits 4:4, typedef current_source_on_mask
//   
//  
#define ADP_SENSE_OFF_MASK_MASK 0x10U
#define ADP_SENSE_OFF_MASK_OFFSET 4
#define ADP_SENSE_OFF_MASK_STOP_BIT 4
#define ADP_SENSE_OFF_MASK_START_BIT 4
#define ADP_SENSE_OFF_MASK_WIDTH 1




//  
//  0: inactive 
//  1: USB PHY has been reset due to a power loss.
//  Register MaskedInterrupt12 0x0E4B, Bits 6:6, typedef usb_phy_error_latch_mask
//   
//  
#define USB_PHY_ERROR_MASK_MASK 0x40U
#define USB_PHY_ERROR_MASK_OFFSET 6
#define USB_PHY_ERROR_MASK_STOP_BIT 6
#define USB_PHY_ERROR_MASK_START_BIT 6
#define USB_PHY_ERROR_MASK_WIDTH 1




//  
//  0: UsbLink status unchanged 
//  1: UsbLink status changed.
//  Register MaskedInterrupt12 0x0E4B, Bits 7:7, typedef usb_link_status_changed_mask
//   
//  
#define USB_LINK_STATUS_MASK_MASK 0x80U
#define USB_LINK_STATUS_MASK_OFFSET 7
#define USB_LINK_STATUS_MASK_STOP_BIT 7
#define USB_LINK_STATUS_MASK_START_BIT 7
#define USB_LINK_STATUS_MASK_WIDTH 1




//  
//  0: Inactive 
//  1: Rising edge Masked on GPIO50 ball.
//  Register MaskedInterrupt13 0x0E4C, Bits 1:1, typedef gpio50_rising_masked
//   
//  
#define GPIO_50_RISING_MASK_MASK 0x2U
#define GPIO_50_RISING_MASK_OFFSET 1
#define GPIO_50_RISING_MASK_STOP_BIT 1
#define GPIO_50_RISING_MASK_START_BIT 1
#define GPIO_50_RISING_MASK_WIDTH 1

#define MASKED_INTERRUPT_13_REG 0xE4C

typedef enum gpio50_rising_masked {
    GPIO_50_RISING_UN_MASKED_E,
    GPIO_50_RISING_MASKED_E
} GPIO_50_RISING_MASKED_T ;
#define GPIO_50_RISING_MASK_PARAM_MASK  0x2



//  
//  0: Inactive 
//  1: Rising edge Masked on GPIO51 ball.
//  Register MaskedInterrupt13 0x0E4C, Bits 2:2, typedef gpio51_rising_masked
//   
//  
#define GPIO_51_RISING_MASK_MASK 0x4U
#define GPIO_51_RISING_MASK_OFFSET 2
#define GPIO_51_RISING_MASK_STOP_BIT 2
#define GPIO_51_RISING_MASK_START_BIT 2
#define GPIO_51_RISING_MASK_WIDTH 1


typedef enum gpio51_rising_masked {
    GPIO_51_RISING_UN_MASKED_E,
    GPIO_51_RISING_MASKED_E
} GPIO_51_RISING_MASKED_T ;
#define GPIO_51_RISING_MASK_PARAM_MASK  0x4



//  
//  0: Inactive 
//  1: Rising edge Masked on GPIO52 ball.
//  Register MaskedInterrupt13 0x0E4C, Bits 3:3, typedef gpio52_rising_masked
//   
//  
#define GPIO_52_RISING_MASK_MASK 0x8U
#define GPIO_52_RISING_MASK_OFFSET 3
#define GPIO_52_RISING_MASK_STOP_BIT 3
#define GPIO_52_RISING_MASK_START_BIT 3
#define GPIO_52_RISING_MASK_WIDTH 1


typedef enum gpio52_rising_masked {
    GPIO_52_RISING_UN_MASKED_E,
    GPIO_52_RISING_MASKED_E
} GPIO_52_RISING_MASKED_T ;
#define GPIO_52_RISING_MASK_PARAM_MASK  0x8



//  
//  0: Inactive 
//  1: Rising edge Masked on GPIO53 ball.
//  Register MaskedInterrupt13 0x0E4C, Bits 4:4, typedef gpio53_rising_masked
//   
//  
#define GPIO_53_RISING_MASK_MASK 0x10U
#define GPIO_53_RISING_MASK_OFFSET 4
#define GPIO_53_RISING_MASK_STOP_BIT 4
#define GPIO_53_RISING_MASK_START_BIT 4
#define GPIO_53_RISING_MASK_WIDTH 1


typedef enum gpio53_rising_masked {
    GPIO_53_RISING_UN_MASKED_E,
    GPIO_53_RISING_MASKED_E
} GPIO_53_RISING_MASKED_T ;
#define GPIO_53_RISING_MASK_PARAM_MASK  0x10



//  
//  0: Inactive 
//  1: Rising edge Masked on GPIO60 ball.
//  Register MaskedInterrupt13 0x0E4C, Bits 5:5, typedef gpio60_rising_masked
//   
//  
#define GPIO_60_RISING_MASK_MASK 0x20U
#define GPIO_60_RISING_MASK_OFFSET 5
#define GPIO_60_RISING_MASK_STOP_BIT 5
#define GPIO_60_RISING_MASK_START_BIT 5
#define GPIO_60_RISING_MASK_WIDTH 1


typedef enum gpio60_rising_masked {
    GPIO_60_RISING_UN_MASKED_E,
    GPIO_60_RISING_MASKED_E
} GPIO_60_RISING_MASKED_T ;
#define GPIO_60_RISING_MASK_PARAM_MASK  0x20



//  
//  0: Inactive 
//  1: Rising edge Masked on IEXTCHRFBN ball.
//  Register MaskedInterrupt13 0x0E4C, Bits 6:6, typedef 
//  external_charger_rising_mask_set
//   
//  
#define EXTERNAL_CHARGER_RISING_MASK_MASK 0x40U
#define EXTERNAL_CHARGER_RISING_MASK_OFFSET 6
#define EXTERNAL_CHARGER_RISING_MASK_STOP_BIT 6
#define EXTERNAL_CHARGER_RISING_MASK_START_BIT 6
#define EXTERNAL_CHARGER_RISING_MASK_WIDTH 1


typedef enum external_charger_rising_mask_set {
    EXTERNAL_CHARGER_RISING_MASK_NO_ACTION_E,
    EXTERNAL_CHARGER_RISING_MASK_ENABLE_E
} EXTERNAL_CHARGER_RISING_MASK_SET_T ;
#define EXTERNAL_CHARGER_RISING_MASK_PARAM_MASK  0x40



//  
//  0: Inactive 
//  1: Falling edge Masked on GPIO50 ball.
//  Register MaskedInterrupt14 0x0E4D, Bits 1:1, typedef gpio50_falling_masked
//   
//  
#define GPIO_50_FALLING_MASK_MASK 0x2U
#define GPIO_50_FALLING_MASK_OFFSET 1
#define GPIO_50_FALLING_MASK_STOP_BIT 1
#define GPIO_50_FALLING_MASK_START_BIT 1
#define GPIO_50_FALLING_MASK_WIDTH 1

#define MASKED_INTERRUPT_14_REG 0xE4D

typedef enum gpio50_falling_masked {
    GPIO_50_FALLING_UN_MASKED_E,
    GPIO_50_FALLING_MASKED_E
} GPIO_50_FALLING_MASKED_T ;
#define GPIO_50_FALLING_MASK_PARAM_MASK  0x2



//  
//  0: Inactive 
//  1: Falling edge Masked on GPIO51 ball.
//  Register MaskedInterrupt14 0x0E4D, Bits 2:2, typedef gpio51_falling_masked
//   
//  
#define GPIO_51_FALLING_MASK_MASK 0x4U
#define GPIO_51_FALLING_MASK_OFFSET 2
#define GPIO_51_FALLING_MASK_STOP_BIT 2
#define GPIO_51_FALLING_MASK_START_BIT 2
#define GPIO_51_FALLING_MASK_WIDTH 1


typedef enum gpio51_falling_masked {
    GPIO_51_FALLING_UN_MASKED_E,
    GPIO_51_FALLING_MASKED_E
} GPIO_51_FALLING_MASKED_T ;
#define GPIO_51_FALLING_MASK_PARAM_MASK  0x4



//  
//  0: Inactive 
//  1: Falling edge Masked on GPIO52 ball.
//  Register MaskedInterrupt14 0x0E4D, Bits 3:3, typedef gpio52_falling_masked
//   
//  
#define GPIO_52_FALLING_MASK_MASK 0x8U
#define GPIO_52_FALLING_MASK_OFFSET 3
#define GPIO_52_FALLING_MASK_STOP_BIT 3
#define GPIO_52_FALLING_MASK_START_BIT 3
#define GPIO_52_FALLING_MASK_WIDTH 1


typedef enum gpio52_falling_masked {
    GPIO_52_FALLING_UN_MASKED_E,
    GPIO_52_FALLING_MASKED_E
} GPIO_52_FALLING_MASKED_T ;
#define GPIO_52_FALLING_MASK_PARAM_MASK  0x8



//  
//  0: Inactive 
//  1: Falling edge Masked on GPIO53 ball.
//  Register MaskedInterrupt14 0x0E4D, Bits 4:4, typedef gpio53_falling_masked
//   
//  
#define GPIO_53_FALLING_MASK_MASK 0x10U
#define GPIO_53_FALLING_MASK_OFFSET 4
#define GPIO_53_FALLING_MASK_STOP_BIT 4
#define GPIO_53_FALLING_MASK_START_BIT 4
#define GPIO_53_FALLING_MASK_WIDTH 1


typedef enum gpio53_falling_masked {
    GPIO_53_FALLING_UN_MASKED_E,
    GPIO_53_FALLING_MASKED_E
} GPIO_53_FALLING_MASKED_T ;
#define GPIO_53_FALLING_MASK_PARAM_MASK  0x10



//  
//  0: Inactive 
//  1: Falling edge Masked on GPIO60 ball.
//  Register MaskedInterrupt14 0x0E4D, Bits 5:5, typedef gpio60_falling_masked
//   
//  
#define GPIO_60_FALLING_MASK_MASK 0x20U
#define GPIO_60_FALLING_MASK_OFFSET 5
#define GPIO_60_FALLING_MASK_STOP_BIT 5
#define GPIO_60_FALLING_MASK_START_BIT 5
#define GPIO_60_FALLING_MASK_WIDTH 1


typedef enum gpio60_falling_masked {
    GPIO_60_FALLING_UN_MASKED_E,
    GPIO_60_FALLING_MASKED_E
} GPIO_60_FALLING_MASKED_T ;
#define GPIO_60_FALLING_MASK_PARAM_MASK  0x20



//  
//  0: Inactive 
//  1: Falling edge Masked on IEXTCHRFBN ball.
//  Register MaskedInterrupt14 0x0E4D, Bits 6:6, typedef external_charger_falling_masked
//   
//  
#define EXTERNAL_CHARGER_FALLING_MASK_MASK 0x40U
#define EXTERNAL_CHARGER_FALLING_MASK_OFFSET 6
#define EXTERNAL_CHARGER_FALLING_MASK_STOP_BIT 6
#define EXTERNAL_CHARGER_FALLING_MASK_START_BIT 6
#define EXTERNAL_CHARGER_FALLING_MASK_WIDTH 1


typedef enum external_charger_falling_masked {
    EXTERNAL_CHARGER_FALLING_UN_MASKED_E,
    EXTERNAL_CHARGER_FALLING_MASKED_E
} EXTERNAL_CHARGER_FALLING_MASKED_T ;
#define EXTERNAL_CHARGER_FALLING_MASK_PARAM_MASK  0x40



//  
//  0: Inactive 
//  1:Interrupt 'Btemp lower BtempLow' masked.
//  Register MaskedInterrupt19 0x0E52, Bits 0:0, typedef 
//  battery_temperature_low_mask_set
//   
//  
#define BATTERY_TEMPERATURE_LOW_MASK_MASK 0x1U
#define BATTERY_TEMPERATURE_LOW_MASK_OFFSET 0
#define BATTERY_TEMPERATURE_LOW_MASK_STOP_BIT 0
#define BATTERY_TEMPERATURE_LOW_MASK_START_BIT 0
#define BATTERY_TEMPERATURE_LOW_MASK_WIDTH 1

#define MASKED_INTERRUPT_19_REG 0xE52

typedef enum battery_temperature_low_mask_set {
    BATTERY_TEMPERATURE_LOW_MASK_NO_ACTION_E,
    BATTERY_TEMPERATURE_LOW_MASK_ENABLE_E
} BATTERY_TEMPERATURE_LOW_MASK_SET_T ;
#define BATTERY_TEMPERATURE_LOW_MASK_PARAM_MASK  0x1



//  
//  0: Inactive 
//  1: Interrupt 'BtempMedium greater Btemp greater BtempLow' masked.
//  Register MaskedInterrupt19 0x0E52, Bits 1:1, typedef 
//  battery_temperature_low_medium_mask_set
//   
//  
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_MASK 0x2U
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_OFFSET 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_STOP_BIT 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_START_BIT 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_WIDTH 1


typedef enum battery_temperature_low_medium_mask_set {
    BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_NO_ACTION_E,
    BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_ENABLE_E
} BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_SET_T ;
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_PARAM_MASK  0x2



//  
//  0: Inactive 
//  1: Interrupt 'BtempHigh greater Btemp greater BtempMedium' masked.
//  Register MaskedInterrupt19 0x0E52, Bits 2:2, typedef 
//  battery_temperature_medium_high_mask_set
//   
//  
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_MASK 0x4U
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_OFFSET 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_STOP_BIT 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_START_BIT 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_WIDTH 1


typedef enum battery_temperature_medium_high_mask_set {
    BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_NO_ACTION_E,
    BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_ENABLE_E
} BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_SET_T ;
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_PARAM_MASK  0x4



//  
//  0: Inactive 
//  1: Interrupt 'Btemp greater BtempHigh' masked.
//  Register MaskedInterrupt19 0x0E52, Bits 3:3, typedef 
//  battery_temperature_high_mask_set
//   
//  
#define BATTERY_TEMPERATURE_HIGH_MASK_MASK 0x8U
#define BATTERY_TEMPERATURE_HIGH_MASK_OFFSET 3
#define BATTERY_TEMPERATURE_HIGH_MASK_STOP_BIT 3
#define BATTERY_TEMPERATURE_HIGH_MASK_START_BIT 3
#define BATTERY_TEMPERATURE_HIGH_MASK_WIDTH 1


typedef enum battery_temperature_high_mask_set {
    BATTERY_TEMPERATURE_HIGH_MASK_NO_ACTION_E,
    BATTERY_TEMPERATURE_HIGH_MASK_ENABLE_E
} BATTERY_TEMPERATURE_HIGH_MASK_SET_T ;
#define BATTERY_TEMPERATURE_HIGH_MASK_PARAM_MASK  0x8



//  
//  ID resistance greater than 220k detected.
//  Register MaskedInterrupt20 0x0E53, Bits 7:7, typedef 
//  id_detection_greater220k_mask_set
//   
//  
#define ID_DETECTION_GREATER_22_0K_MASK_MASK 0x80U
#define ID_DETECTION_GREATER_22_0K_MASK_OFFSET 7
#define ID_DETECTION_GREATER_22_0K_MASK_STOP_BIT 7
#define ID_DETECTION_GREATER_22_0K_MASK_START_BIT 7
#define ID_DETECTION_GREATER_22_0K_MASK_WIDTH 1

#define MASKED_INTERRUPT_20_REG 0xE53

typedef enum id_detection_greater220k_mask_set {
    ID_DETECTION_GREATER_22_0K_MASK_NO_ACTION_E,
    ID_DETECTION_GREATER_22_0K_MASK_ENABLE_E
} ID_DETECTION_GREATER_22_0K_MASK_SET_T ;
#define ID_DETECTION_GREATER_22_0K_MASK_PARAM_MASK  0x80



//  
//  ID resistance greater than 125k detected.
//  Register MaskedInterrupt20 0x0E53, Bits 6:6, typedef 
//  id_detection_greater125k_mask_set
//   
//  
#define ID_DETECTION_GREATER_15_0K_MASK_MASK 0x40U
#define ID_DETECTION_GREATER_15_0K_MASK_OFFSET 6
#define ID_DETECTION_GREATER_15_0K_MASK_STOP_BIT 6
#define ID_DETECTION_GREATER_15_0K_MASK_START_BIT 6
#define ID_DETECTION_GREATER_15_0K_MASK_WIDTH 1


typedef enum id_detection_greater125k_mask_set {
    ID_DETECTION_GREATER_12_5K_MASK_NO_ACTION_E,
    ID_DETECTION_GREATER_12_5K_MASK_ENABLE_E
} ID_DETECTION_GREATER_12_5K_MASK_SET_T ;
#define ID_DETECTION_GREATER_15_0K_MASK_PARAM_MASK  0x40



//  
//  ID resistance greater than 69k detected.
//  Register MaskedInterrupt20 0x0E53, Bits 5:5, typedef 
//  id_detection_greater69k_mask_set
//   
//  
#define ID_DETECTION_GREATER_6_9K_MASK_MASK 0x20U
#define ID_DETECTION_GREATER_6_9K_MASK_OFFSET 5
#define ID_DETECTION_GREATER_6_9K_MASK_STOP_BIT 5
#define ID_DETECTION_GREATER_6_9K_MASK_START_BIT 5
#define ID_DETECTION_GREATER_6_9K_MASK_WIDTH 1


typedef enum id_detection_greater69k_mask_set {
    ID_DETECTION_GREATER_6_9K_MASK_NO_ACTION_E,
    ID_DETECTION_GREATER_6_9K_MASK_ENABLE_E
} ID_DETECTION_GREATER_6_9K_MASK_SET_T ;
#define ID_DETECTION_GREATER_6_9K_MASK_PARAM_MASK  0x20



//  
//  ID resistance greater than 37k detected.
//  Register MaskedInterrupt20 0x0E53, Bits 4:4, typedef 
//  id_detection_greater37k_mask_set
//   
//  
#define ID_DETECTION_GREATER_3_7K_MASK_MASK 0x10U
#define ID_DETECTION_GREATER_3_7K_MASK_OFFSET 4
#define ID_DETECTION_GREATER_3_7K_MASK_STOP_BIT 4
#define ID_DETECTION_GREATER_3_7K_MASK_START_BIT 4
#define ID_DETECTION_GREATER_3_7K_MASK_WIDTH 1


typedef enum id_detection_greater37k_mask_set {
    ID_DETECTION_GREATER_3_7K_MASK_NO_ACTION_E,
    ID_DETECTION_GREATER_3_7K_MASK_ENABLE_E
} ID_DETECTION_GREATER_3_7K_MASK_SET_T ;
#define ID_DETECTION_GREATER_3_7K_MASK_PARAM_MASK  0x10



//  
//  Rising edge detected on WakeUp.
//  Register MaskedInterrupt20 0x0E53, Bits 2:2, typedef id_wake_up_rising_mask_set
//   
//  
#define ID_WAKE_UP_MASK_MASK 0x4U
#define ID_WAKE_UP_MASK_OFFSET 2
#define ID_WAKE_UP_MASK_STOP_BIT 2
#define ID_WAKE_UP_MASK_START_BIT 2
#define ID_WAKE_UP_MASK_WIDTH 1


typedef enum id_wake_up_rising_mask_set {
    ID_WAKE_UP_RISING_MASK_NO_ACTION_E,
    ID_WAKE_UP_RISING_MASK_ENABLE_E
} ID_WAKE_UP_RISING_MASK_SET_T ;
#define ID_WAKE_UP_MASK_PARAM_MASK  0x4



//  
//  0: Inactive 
//  1: not allowed USB charger detection on Vbus interrupt masked.
//  Register MaskedInterrupt20 0x0E53, Bits 1:1, typedef usb_charger_plug_mask_set
//   
//  
#define USB_CHARGER_PLUG_MASK_MASK 0x2U
#define USB_CHARGER_PLUG_MASK_OFFSET 1
#define USB_CHARGER_PLUG_MASK_STOP_BIT 1
#define USB_CHARGER_PLUG_MASK_START_BIT 1
#define USB_CHARGER_PLUG_MASK_WIDTH 1


typedef enum usb_charger_plug_mask_set {
    USB_CHARGER_PLUG_MASK_NO_ACTION_E,
    USB_CHARGER_PLUG_MASK_ENABLE_E
} USB_CHARGER_PLUG_MASK_SET_T ;
#define USB_CHARGER_PLUG_MASK_PARAM_MASK  0x2



//  
//  0: Inactive 
//  1: SRP detect Interrupt masked.
//  Register MaskedInterrupt20 0x0E53, Bits 0:0, typedef dplus_rising_mask_set
//   
//  
#define SRP_DECTECT_MASK_MASK 0x1U
#define SRP_DECTECT_MASK_OFFSET 0
#define SRP_DECTECT_MASK_STOP_BIT 0
#define SRP_DECTECT_MASK_START_BIT 0
#define SRP_DECTECT_MASK_WIDTH 1


typedef enum dplus_rising_mask_set {
    DPLUS_RISING_MASK_NO_ACTION_E,
    DPLUS_RISING_MASK_ENABLE_E
} DPLUS_RISING_MASK_SET_T ;
#define SRP_DECTECT_MASK_PARAM_MASK  0x1



//  
//  0: Inactive 
//  1: Charging is stopped by security reason interrupt masked.
//  Register MaskedInterrupt21 0x0E54, Bits 7:7, typedef charger_sttoped_mask_set
//   
//  
#define CH_STOP_BY_SEC_MASK 0x80U
#define CH_STOP_BY_SEC_OFFSET 7
#define CH_STOP_BY_SEC_STOP_BIT 7
#define CH_STOP_BY_SEC_START_BIT 7
#define CH_STOP_BY_SEC_WIDTH 1

#define MASKED_INTERRUPT_21_REG 0xE54

typedef enum charger_sttoped_mask_set {
    CHARGER_STTOPED_MASK_NO_ACTION_E,
    CHARGER_STTOPED_MASK_ENABLE_E
} CHARGER_STTOPED_MASK_SET_T ;
#define CH_STOP_BY_SEC_PARAM_MASK  0x80



//  
//  USB charger detected.
//  Register MaskedInterrupt21 0x0E54, Bits 6:6, typedef usb_charger_mask_set
//   
//  
#define USB_CHARGER_MASK_MASK 0x40U
#define USB_CHARGER_MASK_OFFSET 6
#define USB_CHARGER_MASK_STOP_BIT 6
#define USB_CHARGER_MASK_START_BIT 6
#define USB_CHARGER_MASK_WIDTH 1


typedef enum usb_charger_mask_set {
    USB_CHARGER_MASK_NO_ACTION_E,
    USB_CHARGER_MASK_ENABLE_E
} USB_CHARGER_MASK_SET_T ;
#define USB_CHARGER_MASK_PARAM_MASK  0x40



//  
//  ID resistance lower than 220k detected.
//  Register MaskedInterrupt21 0x0E54, Bits 5:5, typedef id_detection220k_mask_set
//   
//  
#define ID_DETECTION_22_0K_MASK_MASK 0x20U
#define ID_DETECTION_22_0K_MASK_OFFSET 5
#define ID_DETECTION_22_0K_MASK_STOP_BIT 5
#define ID_DETECTION_22_0K_MASK_START_BIT 5
#define ID_DETECTION_22_0K_MASK_WIDTH 1


typedef enum id_detection220k_mask_set {
    ID_DETECTION_22_0K_MASK_NO_ACTION_E,
    ID_DETECTION_22_0K_MASK_ENABLE_E
} ID_DETECTION_22_0K_MASK_SET_T ;
#define ID_DETECTION_22_0K_MASK_PARAM_MASK  0x20



//  
//  ID resistance lower than 125k detected.
//  Register MaskedInterrupt21 0x0E54, Bits 4:4, typedef id_detection125k_mask_set
//   
//  
#define ID_DETECTION_15_0K_MASK_MASK 0x10U
#define ID_DETECTION_15_0K_MASK_OFFSET 4
#define ID_DETECTION_15_0K_MASK_STOP_BIT 4
#define ID_DETECTION_15_0K_MASK_START_BIT 4
#define ID_DETECTION_15_0K_MASK_WIDTH 1


typedef enum id_detection125k_mask_set {
    ID_DETECTION_12_5K_MASK_NO_ACTION_E,
    ID_DETECTION_12_5K_MASK_ENABLE_E
} ID_DETECTION_12_5K_MASK_SET_T ;
#define ID_DETECTION_15_0K_MASK_PARAM_MASK  0x10



//  
//  ID resistance lower than 69k detected.
//  Register MaskedInterrupt21 0x0E54, Bits 3:3, typedef id_detection69k_mask_set
//   
//  
#define ID_DETECTION_6_9K_MASK_MASK 0x8U
#define ID_DETECTION_6_9K_MASK_OFFSET 3
#define ID_DETECTION_6_9K_MASK_STOP_BIT 3
#define ID_DETECTION_6_9K_MASK_START_BIT 3
#define ID_DETECTION_6_9K_MASK_WIDTH 1


typedef enum id_detection69k_mask_set {
    ID_DETECTION_6_9K_MASK_NO_ACTION_E,
    ID_DETECTION_6_9K_MASK_ENABLE_E
} ID_DETECTION_6_9K_MASK_SET_T ;
#define ID_DETECTION_6_9K_MASK_PARAM_MASK  0x8



//  
//  ID resistance lower than 37k detected.
//  Register MaskedInterrupt21 0x0E54, Bits 2:2, typedef id_detection37k_mask_set
//   
//  
#define ID_DETECTION_3_7K_MASK_MASK 0x4U
#define ID_DETECTION_3_7K_MASK_OFFSET 2
#define ID_DETECTION_3_7K_MASK_STOP_BIT 2
#define ID_DETECTION_3_7K_MASK_START_BIT 2
#define ID_DETECTION_3_7K_MASK_WIDTH 1


typedef enum id_detection37k_mask_set {
    ID_DETECTION_3_7K_MASK_NO_ACTION_E,
    ID_DETECTION_3_7K_MASK_ENABLE_E
} ID_DETECTION_3_7K_MASK_SET_T ;
#define ID_DETECTION_3_7K_MASK_PARAM_MASK  0x4



//  
//  Falling edge on ID Interrupt masked.
//  Register MaskedInterrupt21 0x0E54, Bits 0:0, typedef id_wake_up_falling_mask_set
//   
//  
#define ID_WAKE_UP_FALLING_MASK_MASK 0x1U
#define ID_WAKE_UP_FALLING_MASK_OFFSET 0
#define ID_WAKE_UP_FALLING_MASK_STOP_BIT 0
#define ID_WAKE_UP_FALLING_MASK_START_BIT 0
#define ID_WAKE_UP_FALLING_MASK_WIDTH 1


typedef enum id_wake_up_falling_mask_set {
    ID_WAKE_UP_FALLING_MASK_NO_ACTION_E,
    ID_WAKE_UP_FALLING_MASK_ENABLE_E
} ID_WAKE_UP_FALLING_MASK_SET_T ;
#define ID_WAKE_UP_FALLING_MASK_PARAM_MASK  0x1



//  
//  0: Die temperature is above than usb charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than usb charger thermal protection threshold (charge 
//  stopped).
//  Register MaskedInterrupt22 0x0E55, Bits 0:0, typedef 
//  usb_charger_thermal_below_mask_set
//   
//  
#define USB_CHARGER_THERMAL_BELOW_MASK_MASK 0x1U
#define USB_CHARGER_THERMAL_BELOW_MASK_OFFSET 0
#define USB_CHARGER_THERMAL_BELOW_MASK_STOP_BIT 0
#define USB_CHARGER_THERMAL_BELOW_MASK_START_BIT 0
#define USB_CHARGER_THERMAL_BELOW_MASK_WIDTH 1

#define MASKED_INTERRUPT_22_REG 0xE55

typedef enum usb_charger_thermal_below_mask_set {
    USB_CHARGER_THERMAL_BELOW_MASK_NO_ACTION_E,
    USB_CHARGER_THERMAL_BELOW_MASK_ENABLE_E
} USB_CHARGER_THERMAL_BELOW_MASK_SET_T ;
#define USB_CHARGER_THERMAL_BELOW_MASK_PARAM_MASK  0x1



//  
//  0: Die temperature is above than usb charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than usb charger thermal protection threshold (charge 
//  stopped).
//  Register MaskedInterrupt22 0x0E55, Bits 1:1, typedef 
//  usb_charger_thermal_above_mask_set
//   
//  
#define USB_CHARGER_THERMAL_ABOVE_MASK_MASK 0x2U
#define USB_CHARGER_THERMAL_ABOVE_MASK_OFFSET 1
#define USB_CHARGER_THERMAL_ABOVE_MASK_STOP_BIT 1
#define USB_CHARGER_THERMAL_ABOVE_MASK_START_BIT 1
#define USB_CHARGER_THERMAL_ABOVE_MASK_WIDTH 1


typedef enum usb_charger_thermal_above_mask_set {
    USB_CHARGER_THERMAL_ABOVE_MASK_NO_ACTION_E,
    USB_CHARGER_THERMAL_ABOVE_MASK_ENABLE_E
} USB_CHARGER_THERMAL_ABOVE_MASK_SET_T ;
#define USB_CHARGER_THERMAL_ABOVE_MASK_PARAM_MASK  0x2



//  
//  0: Die temperature is below than main charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than main charger thermal protection threshold (charge 
//  stopped).
//  Register MaskedInterrupt22 0x0E55, Bits 2:2, typedef 
//  main_charger_thermal_below_mask_set
//   
//  
#define MAIN_CHARGER_THERMAL_BELOW_MASK_MASK 0x4U
#define MAIN_CHARGER_THERMAL_BELOW_MASK_OFFSET 2
#define MAIN_CHARGER_THERMAL_BELOW_MASK_STOP_BIT 2
#define MAIN_CHARGER_THERMAL_BELOW_MASK_START_BIT 2
#define MAIN_CHARGER_THERMAL_BELOW_MASK_WIDTH 1


typedef enum main_charger_thermal_below_mask_set {
    MAIN_CHARGER_THERMAL_BELOW_MASK_NO_ACTION_E,
    MAIN_CHARGER_THERMAL_BELOW_MASK_ENABLE_E
} MAIN_CHARGER_THERMAL_BELOW_MASK_SET_T ;
#define MAIN_CHARGER_THERMAL_BELOW_MASK_PARAM_MASK  0x4



//  
//  0: Die temperature is above than main charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than main charger thermal protection threshold (charge 
//  stopped).
//  Register MaskedInterrupt22 0x0E55, Bits 3:3, typedef 
//  main_charger_thermal_above_mask_set
//   
//  
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_MASK 0x8U
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_OFFSET 3
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_STOP_BIT 3
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_START_BIT 3
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_WIDTH 1


typedef enum main_charger_thermal_above_mask_set {
    MAIN_CHARGER_THERMAL_ABOVE_MASK_NO_ACTION_E,
    MAIN_CHARGER_THERMAL_ABOVE_MASK_ENABLE_E
} MAIN_CHARGER_THERMAL_ABOVE_MASK_SET_T ;
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_PARAM_MASK  0x8



//  
//  0: IT unmasked 
//  1: IT_ChargingCurrentNoMoreLimited masked.
//  Register MaskedInterrupt22 0x0E55, Bits 5:5, typedef 
//  charging_current_no_more_limited_mask_set
//   
//  
#define CHARGING_CURRENT_NO_MORE_LIMITED_MASK_MASK 0x20U
#define CHARGING_CURRENT_NO_MORE_LIMITED_MASK_OFFSET 5
#define CHARGING_CURRENT_NO_MORE_LIMITED_MASK_STOP_BIT 5
#define CHARGING_CURRENT_NO_MORE_LIMITED_MASK_START_BIT 5
#define CHARGING_CURRENT_NO_MORE_LIMITED_MASK_WIDTH 1


typedef enum charging_current_no_more_limited_mask_set {
    CHARGING_CURRENT_NO_MORE_LIMITED_MASK_NO_ACTION_E,
    CHARGING_CURRENT_NO_MORE_LIMITED_MASK_ENABLE_E
} CHARGING_CURRENT_NO_MORE_LIMITED_MASK_SET_T ;
#define CHARGING_CURRENT_NO_MORE_LIMITED_MASK_PARAM_MASK  0x20



//  
//  0: IT unmasked 
//  1: IT_ChargingCurrentLimitedHSChirp masked.
//  Register MaskedInterrupt22 0x0E55, Bits 6:6, typedef 
//  charging_current_limited_h_s_chirp_mask_set
//   
//  
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_MASK 0x40U
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_OFFSET 6
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_STOP_BIT 6
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_START_BIT 6
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_WIDTH 1


typedef enum charging_current_limited_h_s_chirp_mask_set {
    CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_NO_ACTION_E,
    CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_ENABLE_E
} CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_SET_T ;
#define CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_PARAM_MASK  0x40



//  
//  0: IT unmasked 
//  1: XTAL startup time failure masked.
//  Register MaskedInterrupt22 0x0E55, Bits 7:7, typedef xtal32_ok_mask_set
//   
//  
#define XTAL_32_OK_MASK_MASK 0x80U
#define XTAL_32_OK_MASK_OFFSET 7
#define XTAL_32_OK_MASK_STOP_BIT 7
#define XTAL_32_OK_MASK_START_BIT 7
#define XTAL_32_OK_MASK_WIDTH 1


typedef enum xtal32_ok_mask_set {
    XTAL_32_OK_MASK_NO_ACTION_E,
    XTAL_32_OK_MASK_ENABLE_E
} XTAL_32_OK_MASK_SET_T ;
#define XTAL_32_OK_MASK_PARAM_MASK  0x80



//  
//  .
//  Register MaskInterrupt25 0x0E58, Bits 0:0, typedef key_stuck_event_masked
//   
//  
#define KEY_STUCK_MASK_MASK 0x1U
#define KEY_STUCK_MASK_OFFSET 0
#define KEY_STUCK_MASK_STOP_BIT 0
#define KEY_STUCK_MASK_START_BIT 0
#define KEY_STUCK_MASK_WIDTH 1

#define MASK_INTERRUPT_25_REG 0xE58

typedef enum key_stuck_event_masked {
    KEY_STUCK_EVENT_UN_MASKED_E,
    KEY_STUCK_EVENT_MASKED_E
} KEY_STUCK_EVENT_MASKED_T ;
#define KEY_STUCK_MASK_PARAM_MASK  0x1



//  
//  .
//  Register MaskInterrupt25 0x0E58, Bits 1:1, typedef lpr_vdddig_event_masked
//   
//  
#define LPR_VDDDIG_MASK_MASK 0x2U
#define LPR_VDDDIG_MASK_OFFSET 1
#define LPR_VDDDIG_MASK_STOP_BIT 1
#define LPR_VDDDIG_MASK_START_BIT 1
#define LPR_VDDDIG_MASK_WIDTH 1


typedef enum lpr_vdddig_event_masked {
    LPR_VDDDIG_EVENT_UN_MASKED_E,
    LPR_VDDDIG_EVENT_MASKED_E
} LPR_VDDDIG_EVENT_MASKED_T ;
#define LPR_VDDDIG_MASK_PARAM_MASK  0x2



//  
//  .
//  Register MaskInterrupt25 0x0E58, Bits 2:2, typedef l_kp_vdddig_event_masked
//   
//  
#define L_KP_VDDDIG_MASK_MASK 0x4U
#define L_KP_VDDDIG_MASK_OFFSET 2
#define L_KP_VDDDIG_MASK_STOP_BIT 2
#define L_KP_VDDDIG_MASK_START_BIT 2
#define L_KP_VDDDIG_MASK_WIDTH 1


typedef enum l_kp_vdddig_event_masked {
    L_KP_VDDDIG_EVENT_UN_MASKED_E,
    L_KP_VDDDIG_EVENT_MASKED_E
} L_KP_VDDDIG_EVENT_MASKED_T ;
#define L_KP_VDDDIG_MASK_PARAM_MASK  0x4



//  
//  .
//  Register MaskInterrupt25 0x0E58, Bits 3:3, typedef kp_vdddig_masked
//   
//  
#define KP_VDDDIG_MASK_MASK 0x8U
#define KP_VDDDIG_MASK_OFFSET 3
#define KP_VDDDIG_MASK_STOP_BIT 3
#define KP_VDDDIG_MASK_START_BIT 3
#define KP_VDDDIG_MASK_WIDTH 1


typedef enum kp_vdddig_masked {
    KP_VDDDIG_UN_MASKED_E,
    KP_VDDDIG_MASKED_E
} KP_VDDDIG_MASKED_T ;
#define KP_VDDDIG_MASK_PARAM_MASK  0x8



//  
//  .
//  Register MaskInterrupt25 0x0E58, Bits 4:4, typedef key_deglitch_masked
//   
//  
#define KEY_DEGLITCH_MASK_MASK 0x10U
#define KEY_DEGLITCH_MASK_OFFSET 4
#define KEY_DEGLITCH_MASK_STOP_BIT 4
#define KEY_DEGLITCH_MASK_START_BIT 4
#define KEY_DEGLITCH_MASK_WIDTH 1


typedef enum key_deglitch_masked {
    KEY_DEGLITCH_UN_MASKED_E,
    KEY_DEGLITCH_MASKED_E
} KEY_DEGLITCH_MASKED_T ;
#define KEY_DEGLITCH_MASK_PARAM_MASK  0x10



//  
//  .
//  Register MaskInterrupt25 0x0E58, Bits 6:7, typedef modem_power_status_masked
//   
//  
#define MODEM_POWER_STATUS_MASK_MASK 0xC0U
#define MODEM_POWER_STATUS_MASK_OFFSET 7
#define MODEM_POWER_STATUS_MASK_STOP_BIT 7
#define MODEM_POWER_STATUS_MASK_START_BIT 6
#define MODEM_POWER_STATUS_MASK_WIDTH 2


typedef enum modem_power_status_masked {
    MODEM_POWER_DOWN_E,
    MODEM_POWER_UP_E
} MODEM_POWER_STATUS_MASKED_T ;
#define MODEM_POWER_STATUS_MASK_PARAM_MASK  0xC0



//  
//  At least one interrupt appends in ITLatch1 register.
//  Register ITLatchHier1 0x0E60, Bits 0:0, typedef it_from_latch1
//   
//  
#define IT_FROM_LATCH_1_MASK 0x1U
#define IT_FROM_LATCH_1_OFFSET 0
#define IT_FROM_LATCH_1_STOP_BIT 0
#define IT_FROM_LATCH_1_START_BIT 0
#define IT_FROM_LATCH_1_WIDTH 1

#define IT_LATCH_HIER_1_REG 0xE60



//  
//  At least one interrupt appends in ITLatch2 register.
//  Register ITLatchHier1 0x0E60, Bits 1:1, typedef it_from_latch2
//   
//  
#define IT_FROM_LATCH_2_MASK 0x2U
#define IT_FROM_LATCH_2_OFFSET 1
#define IT_FROM_LATCH_2_STOP_BIT 1
#define IT_FROM_LATCH_2_START_BIT 1
#define IT_FROM_LATCH_2_WIDTH 1




//  
//  At least one interrupt appends in ITLatch3 register.
//  Register ITLatchHier1 0x0E60, Bits 2:2, typedef it_from_latch3
//   
//  
#define IT_FROM_LATCH_3_MASK 0x4U
#define IT_FROM_LATCH_3_OFFSET 2
#define IT_FROM_LATCH_3_STOP_BIT 2
#define IT_FROM_LATCH_3_START_BIT 2
#define IT_FROM_LATCH_3_WIDTH 1




//  
//  At least one interrupt appends in ITLatch4 register.
//  Register ITLatchHier1 0x0E60, Bits 3:3, typedef it_from_latch4
//   
//  
#define IT_FROM_LATCH_4_MASK 0x8U
#define IT_FROM_LATCH_4_OFFSET 3
#define IT_FROM_LATCH_4_STOP_BIT 3
#define IT_FROM_LATCH_4_START_BIT 3
#define IT_FROM_LATCH_4_WIDTH 1




//  
//  At least one interrupt appends in ITLatch5 register.
//  Register ITLatchHier1 0x0E60, Bits 4:4, typedef it_from_latch5
//   
//  
#define IT_FROM_LATCH_5_MASK 0x10U
#define IT_FROM_LATCH_5_OFFSET 4
#define IT_FROM_LATCH_5_STOP_BIT 4
#define IT_FROM_LATCH_5_START_BIT 4
#define IT_FROM_LATCH_5_WIDTH 1




//  
//  At least one interrupt appends in ITLatch7 register.
//  Register ITLatchHier1 0x0E60, Bits 5:5, typedef it_from_latch7
//   
//  
#define IT_FROM_LATCH_7_MASK 0x20U
#define IT_FROM_LATCH_7_OFFSET 5
#define IT_FROM_LATCH_7_STOP_BIT 5
#define IT_FROM_LATCH_7_START_BIT 5
#define IT_FROM_LATCH_7_WIDTH 1




//  
//  At least one interrupt appends in ITLatch8 register.
//  Register ITLatchHier1 0x0E60, Bits 7:7, typedef it_from_latch8
//   
//  
#define IT_FROM_LATCH_8_MASK 0x80U
#define IT_FROM_LATCH_8_OFFSET 7
#define IT_FROM_LATCH_8_STOP_BIT 7
#define IT_FROM_LATCH_8_START_BIT 7
#define IT_FROM_LATCH_8_WIDTH 1




//  
//  At least one interrupt appends in ITLatch9 register.
//  Register ITLatchHier2 0x0E61, Bits 0:0, typedef it_from_latch9
//   
//  
#define IT_FROM_LATCH_9_MASK 0x1U
#define IT_FROM_LATCH_9_OFFSET 0
#define IT_FROM_LATCH_9_STOP_BIT 0
#define IT_FROM_LATCH_9_START_BIT 0
#define IT_FROM_LATCH_9_WIDTH 1

#define IT_LATCH_HIER_2_REG 0xE61



//  
//  At least one interrupt appends in ITLatch10 register.
//  Register ITLatchHier2 0x0E61, Bits 1:1, typedef it_from_latch10
//   
//  
#define IT_FROM_LATCH_10_MASK 0x2U
#define IT_FROM_LATCH_10_OFFSET 1
#define IT_FROM_LATCH_10_STOP_BIT 1
#define IT_FROM_LATCH_10_START_BIT 1
#define IT_FROM_LATCH_10_WIDTH 1




//  
//  At least one interrupt appends in ITLatch12 register.
//  Register ITLatchHier2 0x0E61, Bits 3:3, typedef it_from_latch12
//   
//  
#define IT_FROM_LATCH_12_MASK 0x8U
#define IT_FROM_LATCH_12_OFFSET 3
#define IT_FROM_LATCH_12_STOP_BIT 3
#define IT_FROM_LATCH_12_START_BIT 3
#define IT_FROM_LATCH_12_WIDTH 1




//  
//  At least one interrupt appends in ITLatch19 register.
//  Register ITLatchHier3 0x0E62, Bits 2:2, typedef it_from_latch19
//   
//  
#define IT_FROM_LATCH_19_MASK 0x4U
#define IT_FROM_LATCH_19_OFFSET 2
#define IT_FROM_LATCH_19_STOP_BIT 2
#define IT_FROM_LATCH_19_START_BIT 2
#define IT_FROM_LATCH_19_WIDTH 1

#define IT_LATCH_HIER_3_REG 0xE62



//  
//  At least one interrupt appends in ITLatch20 register.
//  Register ITLatchHier3 0x0E62, Bits 3:3, typedef it_from_latch20
//   
//  
#define IT_FROM_LATCH_20_MASK 0x8U
#define IT_FROM_LATCH_20_OFFSET 3
#define IT_FROM_LATCH_20_STOP_BIT 3
#define IT_FROM_LATCH_20_START_BIT 3
#define IT_FROM_LATCH_20_WIDTH 1




//  
//  At least one interrupt appends in ITLatch21 register.
//  Register ITLatchHier3 0x0E62, Bits 4:4, typedef it_from_latch21
//   
//  
#define IT_FROM_LATCH_21_MASK 0x10U
#define IT_FROM_LATCH_21_OFFSET 4
#define IT_FROM_LATCH_21_STOP_BIT 4
#define IT_FROM_LATCH_21_START_BIT 4
#define IT_FROM_LATCH_21_WIDTH 1




//  
//  At least one interrupt appends in ITLatch22 register.
//  Register ITLatchHier3 0x0E62, Bits 5:5, typedef it_from_latch22
//   
//  
#define IT_FROM_LATCH_22_MASK 0x20U
#define IT_FROM_LATCH_22_OFFSET 5
#define IT_FROM_LATCH_22_STOP_BIT 5
#define IT_FROM_LATCH_22_START_BIT 5
#define IT_FROM_LATCH_22_WIDTH 1



#endif
