/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_RTC/Linux/ab9540_RTC_types.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_RTC_LINUX_AB9540_RTC_TYPES_H
#define C__LISP_PRODUCTION_AB9540_AB9540_RTC_LINUX_AB9540_RTC_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


//  
//  STw4500 has switched off with 'Swoff' bit of STw4500Ctrl1 register..
//  Register SwitchOffStatus 0x0F00, Bits 0:0, typedef switch_cmd_off
//   
//  
#define SWITCH_CMD_OFF_MASK 0x1U
#define SWITCH_CMD_OFF_OFFSET 0
#define SWITCH_CMD_OFF_STOP_BIT 0
#define SWITCH_CMD_OFF_START_BIT 0
#define SWITCH_CMD_OFF_WIDTH 1

#define SWITCH_OFF_STATUS_REG 0xF00



//  
//  STw4500 has switched off due to Thermal protection activation..
//  Register SwitchOffStatus 0x0F00, Bits 1:1, typedef thermal_protection
//   
//  
#define RTC_THERMAL_PROTECTION_MASK 0x2U
#define RTC_THERMAL_PROTECTION_OFFSET 1
#define RTC_THERMAL_PROTECTION_STOP_BIT 1
#define RTC_THERMAL_PROTECTION_START_BIT 1
#define RTC_THERMAL_PROTECTION_WIDTH 1




//  
//  STw4500 has switched off due to a VbatA ball level lower than BattOk falling 
//  threshold..
//  Register SwitchOffStatus 0x0F00, Bits 2:2, typedef battery_very_low
//   
//  
#define BATTERY_VERY_LOW_MASK 0x4U
#define BATTERY_VERY_LOW_OFFSET 2
#define BATTERY_VERY_LOW_STOP_BIT 2
#define BATTERY_VERY_LOW_START_BIT 2
#define BATTERY_VERY_LOW_WIDTH 1




//  
//  STw4500 has switched off due to primary watchdog has expired..
//  Register SwitchOffStatus 0x0F00, Bits 3:3, typedef wathgod_expired
//   
//  
#define WATHGOD_EXPIRED_MASK 0x8U
#define WATHGOD_EXPIRED_OFFSET 3
#define WATHGOD_EXPIRED_STOP_BIT 3
#define WATHGOD_EXPIRED_START_BIT 3
#define WATHGOD_EXPIRED_WIDTH 1




//  
//  STw4500 has switched off due to the non presence of 32 Khz clock..
//  Register SwitchOffStatus 0x0F00, Bits 4:4, typedef no_signal_c_lock32
//   
//  
#define NO_SIGNAL_C_LOCK_32_MASK 0x10U
#define NO_SIGNAL_C_LOCK_32_OFFSET 4
#define NO_SIGNAL_C_LOCK_32_STOP_BIT 4
#define NO_SIGNAL_C_LOCK_32_START_BIT 4
#define NO_SIGNAL_C_LOCK_32_WIDTH 1




//  
//  STw4500 has switched off due to a battery level lower than PornVbat 
//  threshold..
//  Register SwitchOffStatus 0x0F00, Bits 5:5, typedef low_battery
//   
//  
#define LOW_BATTERY_MASK 0x20U
#define LOW_BATTERY_OFFSET 5
#define LOW_BATTERY_STOP_BIT 5
#define LOW_BATTERY_START_BIT 5
#define LOW_BATTERY_WIDTH 1




//  
//  STw4500 has switched off with a PonKey1 pressure longer than 10 s..
//  Register SwitchOffStatus 0x0F00, Bits 6:6, typedef long_pressure
//   
//  
#define LONG_PRESSURE_MASK 0x40U
#define LONG_PRESSURE_OFFSET 6
#define LONG_PRESSURE_STOP_BIT 6
#define LONG_PRESSURE_START_BIT 6
#define LONG_PRESSURE_WIDTH 1




//  
//  0:AB8500 has not switch off due to DB8500 thermal shutdown 
//  1:AB8500 has switch off due to DB8500 thermal shutdown.
//  Register SwitchOffStatus 0x0F00, Bits 7:7, typedef thermal_shutdown
//   
//  
#define RTC_THERMAL_SHUTDOWN_MASK 0x80U
#define RTC_THERMAL_SHUTDOWN_OFFSET 7
#define RTC_THERMAL_SHUTDOWN_STOP_BIT 7
#define RTC_THERMAL_SHUTDOWN_START_BIT 7
#define RTC_THERMAL_SHUTDOWN_WIDTH 1


typedef enum thermal_shutdown {
    AB8500_SWITCH_OFF_DUE_OTHER_RAISON_E,
    AB8500_SWITCH_OFF_DUE_APE_THERMAL_SHUTDOWN_E
} THERMAL_SHUTDOWN_T ;
#define RTC_THERMAL_SHUTDOWN_PARAM_MASK  0x80



//  
//  Note: Coulomb Counter always disable in PwrOff mode. 
//  0: disable Coulomb Counter 
//  1: enable Coulomb Counter.
//  Register CoulombCounter 0x0F01, Bits 0:0, typedef coulomb_counter_power_off_enable
//   
//  
#define COULOMB_COUNTER_POWER_OFF_MASK 0x1U
#define COULOMB_COUNTER_POWER_OFF_OFFSET 0
#define COULOMB_COUNTER_POWER_OFF_STOP_BIT 0
#define COULOMB_COUNTER_POWER_OFF_START_BIT 0
#define COULOMB_COUNTER_POWER_OFF_WIDTH 1

#define COULOMB_COUNTER_REG 0xF01

typedef enum coulomb_counter_power_off_enable {
    COULOMB_COUNTER_POWER_OFF_DISABLE_E,
    COULOMB_COUNTER_POWER_OFF_ENABLE_E
} COULOMB_COUNTER_POWER_OFF_ENABLE_T ;
#define COULOMB_COUNTER_POWER_OFF_PARAM_MASK  0x1



//  
//  RTC data read transfer is required 
//  Cleared upon data transfert in watchtim register is done.
//  Register RTCReadRequest 0x0F02, Bits 0:0, typedef rtc_read_enable
//   
//  
#define RTC_READ_REQUEST_MASK 0x1U
#define RTC_READ_REQUEST_OFFSET 0
#define RTC_READ_REQUEST_STOP_BIT 0
#define RTC_READ_REQUEST_START_BIT 0
#define RTC_READ_REQUEST_WIDTH 1

#define RTC_READ_REQUEST_REG 0xF02

typedef enum rtc_read_enable {
    RTC_READ_DISABLE_E,
    RTC_READ_ENABLE_E
} RTC_READ_ENABLE_T ;
#define RTC_READ_REQUEST_PARAM_MASK  0x1



//  
//  RTC data write transfer is required. 
//  Cleared upon data transfert in watchtim register is done.
//  Register RTCReadRequest 0x0F02, Bits 1:1, typedef rtc_write_enable
//   
//  
#define RTC_WRITE_REQUEST_MASK 0x2U
#define RTC_WRITE_REQUEST_OFFSET 1
#define RTC_WRITE_REQUEST_STOP_BIT 1
#define RTC_WRITE_REQUEST_START_BIT 1
#define RTC_WRITE_REQUEST_WIDTH 1


typedef enum rtc_write_enable {
    RTC_WRITE_DISABLE_E,
    RTC_WRITE_ENABLE_E
} RTC_WRITE_ENABLE_T ;
#define RTC_WRITE_REQUEST_PARAM_MASK  0x2


#define WATCH_TIME_SEC_LOW_REG 0xF03
#define WATCH_TIME_SEC_HIGH_REG 0xF04
#define WATCH_TIME_MIN_LOW_REG 0xF05
#define WATCH_TIME_MIN_MID_REG 0xF06
#define WATCH_TIME_MIN_HIGH_REG 0xF07
#define ALARM_MIN_LOW_REG 0xF08
#define ALARM_MIN_MID_REG 0xF09
#define ALARM_MIN_HIGH_REG 0xF0A

//  
//  Set at 1 by Software and then reset if RTC supply fail.
//  Register RtcStatus 0x0F0B, Bits 0:0, typedef rtc_status_data
//   
//  
#define RTC_STATUS_DATA_MASK 0x1U
#define RTC_STATUS_DATA_OFFSET 0
#define RTC_STATUS_DATA_STOP_BIT 0
#define RTC_STATUS_DATA_START_BIT 0
#define RTC_STATUS_DATA_WIDTH 1

#define RTC_STATUS_REG 0xF0B

typedef enum rtc_status_data {
    DO_NOT_USE_E,
    SET_ONE_E
} RTC_STATUS_DATA_T ;
#define RTC_STATUS_DATA_PARAM_MASK  0x1



//  
//  Enable/Disable RTC Alarm.
//  Register RtcStatus 0x0F0B, Bits 2:2, typedef rtc_alarm_enable
//   
//  
#define RTC_ALARM_MASK 0x4U
#define RTC_ALARM_OFFSET 2
#define RTC_ALARM_STOP_BIT 2
#define RTC_ALARM_START_BIT 2
#define RTC_ALARM_WIDTH 1


typedef enum rtc_alarm_enable {
    RTC_ALARM_DISABLE_E,
    RTC_ALARM_ENABLE_E
} RTC_ALARM_ENABLE_T ;
#define RTC_ALARM_PARAM_MASK  0x4



//  
//  Enable/Disable Backup Present.
//  Register RtcStatus 0x0F0B, Bits 3:3, typedef backup_present_enable
//   
//  
#define BACKUP_PRESENT_MASK 0x8U
#define BACKUP_PRESENT_OFFSET 3
#define BACKUP_PRESENT_STOP_BIT 3
#define BACKUP_PRESENT_START_BIT 3
#define BACKUP_PRESENT_WIDTH 1


typedef enum backup_present_enable {
    BACKUP_PRESENT_DISABLE_E,
    BACKUP_PRESENT_ENABLE_E
} BACKUP_PRESENT_ENABLE_T ;
#define BACKUP_PRESENT_PARAM_MASK  0x8



//  
//  Enable/Disable Backup charger.
//  Register RtcStatus 0x0F0B, Bits 4:4, typedef backup_charger_enable
//   
//  
#define BACKUP_CHARGER_MASK 0x10U
#define BACKUP_CHARGER_OFFSET 4
#define BACKUP_CHARGER_STOP_BIT 4
#define BACKUP_CHARGER_START_BIT 4
#define BACKUP_CHARGER_WIDTH 1


typedef enum backup_charger_enable {
    BACKUP_CHARGER_DISABLE_E,
    BACKUP_CHARGER_ENABLE_E
} BACKUP_CHARGER_ENABLE_T ;
#define BACKUP_CHARGER_PARAM_MASK  0x10



//  
//  0: Current source to charge back up battery disabled in OFF mode 
//  1: Current source to charge back up battery enabled in OFF mode.
//  Register RtcStatus 0x0F0B, Bits 5:5, typedef charge_backup_battery_enable
//   
//  
#define CHARGE_BACKUP_BATTERY_MASK 0x20U
#define CHARGE_BACKUP_BATTERY_OFFSET 5
#define CHARGE_BACKUP_BATTERY_STOP_BIT 5
#define CHARGE_BACKUP_BATTERY_START_BIT 5
#define CHARGE_BACKUP_BATTERY_WIDTH 1


typedef enum charge_backup_battery_enable {
    CHARGE_BACKUP_BATTERY_DISABLE_E,
    CHARGE_BACKUP_BATTERY_ENABLE_E
} CHARGE_BACKUP_BATTERY_ENABLE_T ;
#define CHARGE_BACKUP_BATTERY_PARAM_MASK  0x20



//  
//  Backup charger voltage selection.
//  Register RTCBackupCharger 0x0F0C, Bits 0:1, typedef backup_voltage_select
//   
//  
#define BACKUP_VOLTAGE_SELECT_MASK 0x3U
#define BACKUP_VOLTAGE_SELECT_OFFSET 1
#define BACKUP_VOLTAGE_SELECT_STOP_BIT 1
#define BACKUP_VOLTAGE_SELECT_START_BIT 0
#define BACKUP_VOLTAGE_SELECT_WIDTH 2

#define RTC_BACKUP_CHARGER_REG 0xF0C
#define  BACKUP_CHARGER_2V5 0
#define  BACKUP_CHARGER_2V6 1
#define  BACKUP_CHARGER_2V8 2
#define  BACKUP_CHARGER_3V1 3

typedef enum backup_voltage_select {
    BACKUP_CHARGER_2V5_E,
    BACKUP_CHARGER_2V6_E,
    BACKUP_CHARGER_2V8_E,
    BACKUP_CHARGER_3V1_E
} BACKUP_VOLTAGE_SELECT_T ;
#define BACKUP_VOLTAGE_SELECT_PARAM_MASK  0x3



//  
//  Backup charger current intensity selection.
//  Register RTCBackupCharger 0x0F0C, Bits 2:3, typedef backup_current_select
//   
//  
#define BACKUP_CURRENT_SELECT_MASK 0xCU
#define BACKUP_CURRENT_SELECT_OFFSET 3
#define BACKUP_CURRENT_SELECT_STOP_BIT 3
#define BACKUP_CURRENT_SELECT_START_BIT 2
#define BACKUP_CURRENT_SELECT_WIDTH 2

#define  BACKUP_CHARGER_50_UA 0
#define  BACKUP_CHARGER_150_UA 1
#define  BACKUP_CHARGER_300_UA 2
#define  BACKUP_CHARGER_700UA 3

typedef enum backup_current_select {
    BACKUP_CHARGER_50_UA_E,
    BACKUP_CHARGER_150_UA_E,
    BACKUP_CHARGER_300_UA_E,
    BACKUP_CHARGER_700UA_E
} BACKUP_CURRENT_SELECT_T ;
#define BACKUP_CURRENT_SELECT_PARAM_MASK  0xC



//  
//  Force backup battery as supply for RTC block.
//  Register RtcCForceBackup 0x0F0D, Bits 0:0, typedef backup_force
//   
//  
#define BACKUP_FORCE_MASK 0x1U
#define BACKUP_FORCE_OFFSET 0
#define BACKUP_FORCE_STOP_BIT 0
#define BACKUP_FORCE_START_BIT 0
#define BACKUP_FORCE_WIDTH 1

#define RTC_C_FORCE_BACKUP_REG 0xF0D

typedef enum backup_force {
    RTC_BACKUP_INACTIVE_E,
    RTC_FORCE_BACKUP_E
} BACKUP_FORCE_T ;
#define BACKUP_FORCE_PARAM_MASK  0x1


#define RTC_DERIVATION_REG 0xF0E

//  
//  00: no clock available (only possible in test mode) 
//  01: internal 32Khz RC oscillator selected 
//  10: internal 32khz Xtal oscillator selected (when Xtal selected, internal RC 
//  oscillator is turnedoff).
//  Register RtcClockStatus 0x0F0F, Bits 0:1, typedef clock32_status
//   
//  
#define CLOCK_32_STATUS_MASK 0x3U
#define CLOCK_32_STATUS_OFFSET 1
#define CLOCK_32_STATUS_STOP_BIT 1
#define CLOCK_32_STATUS_START_BIT 0
#define CLOCK_32_STATUS_WIDTH 2

#define RTC_CLOCK_STATUS_REG 0xF0F
#define  NO_CLOCK_AVAILABLE 0
#define  INTERNAL_RC_OSCILLASTOR 1
#define  INTERNAL_XTAL_OSCILLASTOR 2

typedef enum clock32_status {
    NO_CLOCK_AVAILABLE_E,
    INTERNAL_RC_OSCILLASTOR_E,
    INTERNAL_XTAL_OSCILLASTOR_E
} CLOCK_32_STATUS_T ;
#define CLOCK_32_STATUS_PARAM_MASK  0x3



//  
//  0: Xtal 32khz oscillator is stopped 
//  1: Xtal 32khz oscillator is running.
//  Register RtcClockStatus 0x0F0F, Bits 2:2, typedef oscillator32k_status
//   
//  
#define OSCILLATOR_3_2K_STATUS_MASK 0x4U
#define OSCILLATOR_3_2K_STATUS_OFFSET 2
#define OSCILLATOR_3_2K_STATUS_STOP_BIT 2
#define OSCILLATOR_3_2K_STATUS_START_BIT 2
#define OSCILLATOR_3_2K_STATUS_WIDTH 1


typedef enum oscillator32k_status {
    OSCILLATOR32K_STOPPED_E,
    OSCILLATOR32K_RUNNING_E
} OSCILLATOR_3_2K_STATUS_T ;
#define OSCILLATOR_3_2K_STATUS_PARAM_MASK  0x4


#endif
