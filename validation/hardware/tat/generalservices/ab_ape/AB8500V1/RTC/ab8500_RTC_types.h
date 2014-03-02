/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_RTC/Linux/ab8500_RTC_types.h
 * 
 *
 * Generated on the 26/02/2010 09:06 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/

/****************************************************************************
 * Modifications:
 *  CAP_948_001: 22 March 2010 by Karine Boclaud
 *
 ***************************************************************************/

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_RTC_LINUX_AB8500_RTC_TYPES_H
#define C__LISP_PRODUCTION_AB8500_AB8500_RTC_LINUX_AB8500_RTC_TYPES_H




//  
//  STw4500 has switched off with a PonKey1 pressure longer than 10 s..
//  Register SwitchOffStatus 0x0F00, Bits 6:6, typedef long_pressure
//   
//  
#define LONG_PRESSURE_MASK 0x40
#define LONG_PRESSURE_OFFSET 6
#define LONG_PRESSURE_STOP_BIT 6
#define LONG_PRESSURE_START_BIT 6
#define LONG_PRESSURE_WIDTH 1

#define SWITCH_OFF_STATUS_REG 0xF00
#define LONG_PRESSURE_PARAM_MASK  0x40

//  
//  STw4500 has switched off due to a battery level lower than PornVbat 
//  threshold..
//  Register SwitchOffStatus 0x0F00, Bits 5:5, typedef low_battery
//   
//  
#define LOW_BATTERY_MASK 0x20
#define LOW_BATTERY_OFFSET 5
#define LOW_BATTERY_STOP_BIT 5
#define LOW_BATTERY_START_BIT 5
#define LOW_BATTERY_WIDTH 1

#define LOW_BATTERY_PARAM_MASK  0x20

//  
//  STw4500 has switched off due to the non presence of 32 Khz clock..
//  Register SwitchOffStatus 0x0F00, Bits 4:4, typedef no_signal_c_lock32
//   
//  
#define NO_SIGNAL_C_LOCK_32_MASK 0x10
#define NO_SIGNAL_C_LOCK_32_OFFSET 4
#define NO_SIGNAL_C_LOCK_32_STOP_BIT 4
#define NO_SIGNAL_C_LOCK_32_START_BIT 4
#define NO_SIGNAL_C_LOCK_32_WIDTH 1

#define NO_SIGNAL_C_LOCK_32_PARAM_MASK  0x10

//  
//  STw4500 has switched off due to primary watchdog has expired..
//  Register SwitchOffStatus 0x0F00, Bits 3:3, typedef wathgod_expired
//   
//  
#define WATHGOD_EXPIRED_MASK 0x8
#define WATHGOD_EXPIRED_OFFSET 3
#define WATHGOD_EXPIRED_STOP_BIT 3
#define WATHGOD_EXPIRED_START_BIT 3
#define WATHGOD_EXPIRED_WIDTH 1

#define WATHGOD_EXPIRED_PARAM_MASK  0x8

//  
//  STw4500 has switched off due to a VbatA ball level lower than BattOk falling 
//  threshold..
//  Register SwitchOffStatus 0x0F00, Bits 2:2, typedef battery_very_low
//   
//  
#define BATTERY_VERY_LOW_MASK 0x4
#define BATTERY_VERY_LOW_OFFSET 2
#define BATTERY_VERY_LOW_STOP_BIT 2
#define BATTERY_VERY_LOW_START_BIT 2
#define BATTERY_VERY_LOW_WIDTH 1

#define BATTERY_VERY_LOW_PARAM_MASK  0x4

//  
//  STw4500 has switched off due to Thermal protection activation..
//  Register SwitchOffStatus 0x0F00, Bits 1:1, typedef thermal_protection
//   
//  
#define RTC_THERMAL_PROTECTION_MASK 0x2
#define RTC_THERMAL_PROTECTION_OFFSET 1
#define RTC_THERMAL_PROTECTION_STOP_BIT 1
#define RTC_THERMAL_PROTECTION_START_BIT 1
#define RTC_THERMAL_PROTECTION_WIDTH 1

#define THERMAL_PROTECTION_PARAM_MASK  0x2

//  
//  STw4500 has switched off with ‘Swoff’.bit of STw4500Ctrl1 register..
//  Register SwitchOffStatus 0x0F00, Bits 0:0, typedef switch_cmd_off
//   
//  
#define SWITCH_CMD_OFF_MASK 0x1
#define SWITCH_CMD_OFF_OFFSET 0
#define SWITCH_CMD_OFF_STOP_BIT 0
#define SWITCH_CMD_OFF_START_BIT 0
#define SWITCH_CMD_OFF_WIDTH 1

#define SWITCH_CMD_OFF_PARAM_MASK  0x1

//  
//  0: allow to disable automatically CoulombCounter in STw4500 deep sleep mode 
//  1: allow to keep CoulombCounter enable in STw4500 deep sleep mode.
//  Register CoulombCounter 0x0F01, Bits 1:1, typedef coulomb_counter_low_power_enable
//   
//  
#define COULOMB_COUNTER_LOW_POWER_MASK 0x2
#define COULOMB_COUNTER_LOW_POWER_OFFSET 1
#define COULOMB_COUNTER_LOW_POWER_STOP_BIT 1
#define COULOMB_COUNTER_LOW_POWER_START_BIT 1
#define COULOMB_COUNTER_LOW_POWER_WIDTH 1

#define COULOMB_COUNTER_REG 0xF01

typedef enum coulomb_counter_low_power_enable {
    COULOMB_COUNTER_LOW_POWER_DOWN_E,
    COULOMB_COUNTER_LOW_POWER_UP_E
} COULOMB_COUNTER_LOW_POWER_ENABLE_T ;

#define COULOMB_COUNTER_LOW_POWER_ENABLE_PARAM_MASK  0x2

//  
//  Note: Coulomb Counter always disable in PwrOff mode. 
//  0: disable Coulomb Counter 
//  1: enable Coulomb Counter.
//  Register CoulombCounter 0x0F01, Bits 0:0, typedef coulomb_counter_power_off_enable
//   
//  
#define COULOMB_COUNTER_POWER_OFF_MASK 0x1
#define COULOMB_COUNTER_POWER_OFF_OFFSET 0
#define COULOMB_COUNTER_POWER_OFF_STOP_BIT 0
#define COULOMB_COUNTER_POWER_OFF_START_BIT 0
#define COULOMB_COUNTER_POWER_OFF_WIDTH 1


typedef enum coulomb_counter_power_off_enable {
    COULOMB_COUNTER_POWER_DOWN_E,
    COULOMB_COUNTER_POWER_UP_E
} COULOMB_COUNTER_POWER_OFF_ENABLE_T ;

#define COULOMB_COUNTER_POWER_OFF_ENABLE_PARAM_MASK  0x1

//  
//  RTC data write transfer is required. 
//  Cleared upon data transfert in watchtim register is done.
//  Register RTCReadRequest 0x0F02, Bits 1:1, typedef rtc_write_enable
//   
//  
#define RTC_WRITE_REQUEST_MASK 0x2
#define RTC_WRITE_REQUEST_OFFSET 1
#define RTC_WRITE_REQUEST_STOP_BIT 1
#define RTC_WRITE_REQUEST_START_BIT 1
#define RTC_WRITE_REQUEST_WIDTH 1

#define RTC_READ_REQUEST_REG 0xF02

typedef enum rtc_write_enable {
    RTC_WRITE_DISABLE_E,
    RTC_WRITE_ENABLE_E
} RTC_WRITE_ENABLE_T ;

#define RTC_WRITE_ENABLE_PARAM_MASK  0x2

//  
//  RTC data read transfer is required 
//  Cleared upon data transfert in watchtim register is done.
//  Register RTCReadRequest 0x0F02, Bits 0:0, typedef rtc_read_enable
//   
//  
#define RTC_READ_REQUEST_MASK 0x1
#define RTC_READ_REQUEST_OFFSET 0
#define RTC_READ_REQUEST_STOP_BIT 0
#define RTC_READ_REQUEST_START_BIT 0
#define RTC_READ_REQUEST_WIDTH 1


typedef enum rtc_read_enable {
    RTC_READ_DISABLE_E,
    RTC_READ_ENABLE_E
} RTC_READ_ENABLE_T ;

#define RTC_READ_ENABLE_PARAM_MASK  0x1
#define WATCH_TIME_SEC_LOW_REG 0xF03
#define WATCH_TIME_SEC_HIGH_REG 0xF04
#define WATCH_TIME_MIN_LOW_REG 0xF05
#define WATCH_TIME_MIN_MID_REG 0xF06
#define WATCH_TIME_MIN_HIGH_REG 0xF07
#define ALARM_MIN_LOW_REG 0xF08
#define ALARM_MIN_MID_REG 0xF09
#define ALARM_MIN_HIGH_REG 0xF0A

//  
//  0: Current source to charge back up battery disabled in OFF mode 
//  1: Current source to charge back up battery enabled in OFF mode.
//  Register RTCStatus 0x0F0B, Bits 5:5, typedef charge_backup_battery_enable
//   
//  
#define CHARGE_BACKUP_BATTERY_MASK 0x20
#define CHARGE_BACKUP_BATTERY_OFFSET 5
#define CHARGE_BACKUP_BATTERY_STOP_BIT 5
#define CHARGE_BACKUP_BATTERY_START_BIT 5
#define CHARGE_BACKUP_BATTERY_WIDTH 1

#define RTC_STATUS_REG 0xF0B

typedef enum charge_backup_battery_enable {
    CHARGE_BACKUP_BATTERY_DISABLE_E,
    CHARGE_BACKUP_BATTERY_ENABLE_E
} CHARGE_BACKUP_BATTERY_ENABLE_T ;

#define CHARGE_BACKUP_BATTERY_ENABLE_PARAM_MASK  0x20

//  
//  Enable/Disable Backup charger.
//  Register RtcStatus 0x0F0B, Bits 4:4, typedef backup_charger_enable
//   
//  
#define BACKUP_CHARGER_ENABLE_MASK 0x10
#define BACKUP_CHARGER_ENABLE_OFFSET 4
#define BACKUP_CHARGER_ENABLE_STOP_BIT 4
#define BACKUP_CHARGER_ENABLE_START_BIT 4
#define BACKUP_CHARGER_ENABLE_WIDTH 1


typedef enum backup_charger_enable {
    BACKUP_CHARGER_DISABLE_E,
    BACKUP_CHARGER_ENABLE_E
} BACKUP_CHARGER_ENABLE_T ;

#define BACKUP_CHARGER_ENABLE_PARAM_MASK  0x10

//  
//  Enable/Disable Backup Present.
//  Register RtcStatus 0x0F0B, Bits 3:3, typedef backup_present_enable
//   
//  
#define BACKUP_PRESENT_ENABLE_MASK 0x8
#define BACKUP_PRESENT_ENABLE_OFFSET 3
#define BACKUP_PRESENT_ENABLE_STOP_BIT 3
#define BACKUP_PRESENT_ENABLE_START_BIT 3
#define BACKUP_PRESENT_ENABLE_WIDTH 1


typedef enum backup_present_enable {
    BACKUP_PRESENT_DISABLE_E,
    BACKUP_PRESENT_ENABLE_E
} BACKUP_PRESENT_ENABLE_T ;

#define BACKUP_PRESENT_ENABLE_PARAM_MASK  0x8

//  
//  Enable/Disable RTC Alarm.
//  Register RtcStatus 0x0F0B, Bits 2:2, typedef rtc_alarm_enable
//   
//  
#define RTC_ALARM_ENABLE_MASK 0x4
#define RTC_ALARM_ENABLE_OFFSET 2
#define RTC_ALARM_ENABLE_STOP_BIT 2
#define RTC_ALARM_ENABLE_START_BIT 2
#define RTC_ALARM_ENABLE_WIDTH 1


typedef enum rtc_alarm_enable {
    RTC_ALARM_DISABLE_E,
    RTC_ALARM_ENABLE_E
} RTC_ALARM_ENABLE_T ;

#define RTC_ALARM_ENABLE_PARAM_MASK  0x4

//  
//  Set at 1 by Software and then reset if RTC supply fail.
//  Register RtcStatus 0x0F0B, Bits 0:0, typedef rtc_status_data
//   
//  
#define RTC_STATUS_DATA_MASK 0x1
#define RTC_STATUS_DATA_OFFSET 0
#define RTC_STATUS_DATA_STOP_BIT 0
#define RTC_STATUS_DATA_START_BIT 0
#define RTC_STATUS_DATA_WIDTH 1

#define  RTC_STATUS_DATA 1

/*+CAP_948_001*/
 typedef enum rtc_status_data {
     DO_NOT_USE_E,
	 SET_ONE_E
} RTC_STATUS_DATA_T ;
/*-CAP_948_001*/
#define RTC_STATUS_DATA_PARAM_MASK  0x1



//  
//  Backup charger current intensity selection.
//  Register RTCBackupCharger 0x0F0C, Bits 2:3, typedef backup_current_select
//   
//  
#define BACKUP_CURRENT_SELECT_MASK 0xC
#define BACKUP_CURRENT_SELECT_OFFSET 3
#define BACKUP_CURRENT_SELECT_STOP_BIT 3
#define BACKUP_CURRENT_SELECT_START_BIT 2
#define BACKUP_CURRENT_SELECT_WIDTH 2

#define RTC_BACKUP_CHARGER_REG 0xF0C
#define  BACKUP_CHARGER_50_UA 0
#define  BACKUP_CHARGER_150_UA 1
#define  BACKUP_CHARGER_300_UA 2
#define  BACKUP_CHARGER_700UA 3
#define BACKUP_CURRENT_SELECT_PARAM_MASK  0xC



typedef enum backup_current_select {
    BACKUP_CHARGER_50_UA_E,
    BACKUP_CHARGER_150_UA_E,
    BACKUP_CHARGER_300_UA_E,
    BACKUP_CHARGER_700UA_E
} BACKUP_CURRENT_SELECT_T ;




//  
//  Backup charger voltage selection.
//  Register RTCBackupCharger 0x0F0C, Bits 0:1, typedef backup_voltage_select
//   
//  
#define BACKUP_VOLTAGE_SELECT_MASK 0x3
#define BACKUP_VOLTAGE_SELECT_OFFSET 1
#define BACKUP_VOLTAGE_SELECT_STOP_BIT 1
#define BACKUP_VOLTAGE_SELECT_START_BIT 0
#define BACKUP_VOLTAGE_SELECT_WIDTH 2

#define  BACKUP_CHARGER_2V5 0
#define  BACKUP_CHARGER_2V6 1
#define  BACKUP_CHARGER_2V8 2
#define  BACKUP_CHARGER_3V1 3
#define BACKUP_VOLTAGE_SELECT_PARAM_MASK  0x3



typedef enum backup_voltage_select {
    BACKUP_CHARGER_2V5_E,
    BACKUP_CHARGER_2V6_E,
    BACKUP_CHARGER_2V8_E,
    BACKUP_CHARGER_3V1_E
} BACKUP_VOLTAGE_SELECT_T ;




//  
//  Force backup battery as supply for RTC block.
//  Register RtcCForceBackup 0x0F0D, Bits 0:0, typedef backup_force
//   
//  
#define BACKUP_FORCE_MASK 0x1
#define BACKUP_FORCE_OFFSET 0
#define BACKUP_FORCE_STOP_BIT 0
#define BACKUP_FORCE_START_BIT 0
#define BACKUP_FORCE_WIDTH 1

#define RTC_C_FORCE_BACKUP_REG 0xF0D
#define  BACKUP_FORCE 1

/*+CAP_948_001*/
typedef enum backup_force {
    RTC_BACKUP_INACTIVE_E,
    RTC_FORCE_BACKUP_E
} BACKUP_FORCE_T ;
/*-CAP_948_001*/
#define BACKUP_FORCE_PARAM_MASK  0x1


#define RTC_DERIVATION_REG 0xF0E

//  
//  00: no clock available (only possible in test mode) 
//  01: internal 32Khz RC oscillator selected 
//  10: internal 32khz Xtal oscillator selected (when Xtal selected, internal RC 
//  oscillator is turnedoff).
//  Register RtcDerivation 0x0F0F, Bits 0:1, typedef clock32_status
//   
//  
#define CLOCK_32_STATUS_MASK 0x3
#define CLOCK_32_STATUS_OFFSET 1
#define CLOCK_32_STATUS_STOP_BIT 1
#define CLOCK_32_STATUS_START_BIT 0
#define CLOCK_32_STATUS_WIDTH 2

#define  NO_CLOCK_AVAILABLE 0
#define  INTERNAL_RC_OSCILLASTOR 1
#define  INTERNAL_XTAL_OSCILLASTOR 2
#define CLOCK_32_STATUS_PARAM_MASK  0x3



typedef enum clock32_status {
    NO_CLOCK_AVAILABLE_E,
    INTERNAL_RC_OSCILLASTOR_E,
    INTERNAL_XTAL_OSCILLASTOR_E
} CLOCK_32_STATUS_T ;



#endif
