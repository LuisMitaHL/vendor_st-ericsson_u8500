/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_RTC/Linux/ab9540_RTC.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_RTC_LINUX_AB9540_RTC_C
#define C__LISP_PRODUCTION_AB9540_AB9540_RTC_LINUX_AB9540_RTC_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_RTC.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_rtc_thermal_shutdown
 *
 * RET  : Return the value of register 0xSwitchOffStatus
 *
 * Notes : From register 0xF00, bits 7:7
 *
 **************************************************************************/
unsigned char get_rtc_thermal_shutdown()
  {
    unsigned char value;


    /* 
     * 0:AB8500 has not switch off due to DB8500 thermal 
     * shutdown 
     * 1:AB8500 has switch off due to DB8500 thermal shutdown 
     */ 
    value = (I2CRead(SWITCH_OFF_STATUS_REG) & RTC_THERMAL_SHUTDOWN_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_power_off
 *
 * IN   : param, a value to write to the regiter CoulombCounter
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombCounter
 *
 * Notes : From register 0xF01, bits 0:0
 *
 **************************************************************************/
unsigned char set_coulomb_counter_power_off( enum coulomb_counter_power_off_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(COULOMB_COUNTER_REG);

    /* 
     * Note: Coulomb Counter always disable in PwrOff mode. 
     * 0: disable Coulomb Counter 
     * 1: enable Coulomb Counter 
     */ 
    switch( param ){
        case COULOMB_COUNTER_POWER_OFF_ENABLE_E: 
           value = old_value | COULOMB_COUNTER_POWER_OFF_MASK; 
           break;
        case COULOMB_COUNTER_POWER_OFF_DISABLE_E: 
           value = old_value & ~ COULOMB_COUNTER_POWER_OFF_MASK;
           break;
    }
  

    I2CWrite(COULOMB_COUNTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_power_off
 *
 * RET  : Return the value of register 0xCoulombCounter
 *
 * Notes : From register 0xF01, bits 0:0
 *
 **************************************************************************/
unsigned char get_coulomb_counter_power_off()
  {
    unsigned char value;


    /* 
     * Note: Coulomb Counter always disable in PwrOff mode. 
     * 0: disable Coulomb Counter 
     * 1: enable Coulomb Counter 
     */ 
    value = (I2CRead(COULOMB_COUNTER_REG) & COULOMB_COUNTER_POWER_OFF_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_read_request
 *
 * IN   : param, a value to write to the regiter RTCReadRequest
 * OUT  : 
 *
 * RET  : Return the value of register 0xRTCReadRequest
 *
 * Notes : From register 0xF02, bits 0:0
 *
 **************************************************************************/
unsigned char set_rtc_read_request( enum rtc_read_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_READ_REQUEST_REG);

    /* 
     * RTC data read transfer is required 
     * Cleared upon data transfert in watchtim register is done 
     */ 
    switch( param ){
        case RTC_READ_ENABLE_E: 
           value = old_value | RTC_READ_REQUEST_MASK; 
           break;
        case RTC_READ_DISABLE_E: 
           value = old_value & ~ RTC_READ_REQUEST_MASK;
           break;
    }
  

    I2CWrite(RTC_READ_REQUEST_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_read_request
 *
 * RET  : Return the value of register 0xRTCReadRequest
 *
 * Notes : From register 0xF02, bits 0:0
 *
 **************************************************************************/
unsigned char get_rtc_read_request()
  {
    unsigned char value;


    /* 
     * RTC data read transfer is required 
     * Cleared upon data transfert in watchtim register is done 
     */ 
    value = (I2CRead(RTC_READ_REQUEST_REG) & RTC_READ_REQUEST_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_write_request
 *
 * IN   : param, a value to write to the regiter RTCReadRequest
 * OUT  : 
 *
 * RET  : Return the value of register 0xRTCReadRequest
 *
 * Notes : From register 0xF02, bits 1:1
 *
 **************************************************************************/
unsigned char set_rtc_write_request( enum rtc_write_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_READ_REQUEST_REG);

    /* 
     * RTC data write transfer is required. 
     * Cleared upon data transfert in watchtim register is done 
     */ 
    switch( param ){
        case RTC_WRITE_ENABLE_E: 
           value = old_value | RTC_WRITE_REQUEST_MASK; 
           break;
        case RTC_WRITE_DISABLE_E: 
           value = old_value & ~ RTC_WRITE_REQUEST_MASK;
           break;
    }
  

    I2CWrite(RTC_READ_REQUEST_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_write_request
 *
 * RET  : Return the value of register 0xRTCReadRequest
 *
 * Notes : From register 0xF02, bits 1:1
 *
 **************************************************************************/
unsigned char get_rtc_write_request()
  {
    unsigned char value;


    /* 
     * RTC data write transfer is required. 
     * Cleared upon data transfert in watchtim register is done 
     */ 
    value = (I2CRead(RTC_READ_REQUEST_REG) & RTC_WRITE_REQUEST_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_watch_time_sec_low
 *
 * RET  : Return the value of register 0xWatchTimeSecLow
 *
 * Notes : From register 0xF03, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_sec_low()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(WATCH_TIME_SEC_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_sec_low
 *
 * IN   : param, a value to write to the regiter WatchTimeSecLow
 * OUT  : 
 *
 * RET  : Return the value of register 0xWatchTimeSecLow
 *
 * Notes : From register 0xF03, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_sec_low( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(WATCH_TIME_SEC_LOW_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(WATCH_TIME_SEC_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_watch_time_sec_high
 *
 * RET  : Return the value of register 0xWatchTimeSecHigh
 *
 * Notes : From register 0xF04, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_sec_high()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(WATCH_TIME_SEC_HIGH_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_sec_high
 *
 * IN   : param, a value to write to the regiter WatchTimeSecHigh
 * OUT  : 
 *
 * RET  : Return the value of register 0xWatchTimeSecHigh
 *
 * Notes : From register 0xF04, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_sec_high( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(WATCH_TIME_SEC_HIGH_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(WATCH_TIME_SEC_HIGH_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_watch_time_min_low
 *
 * RET  : Return the value of register 0xWatchTimeMinLow
 *
 * Notes : From register 0xF05, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_min_low()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(WATCH_TIME_MIN_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_min_low
 *
 * IN   : param, a value to write to the regiter WatchTimeMinLow
 * OUT  : 
 *
 * RET  : Return the value of register 0xWatchTimeMinLow
 *
 * Notes : From register 0xF05, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_min_low( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(WATCH_TIME_MIN_LOW_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(WATCH_TIME_MIN_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_watch_time_min_mid
 *
 * RET  : Return the value of register 0xWatchTimeMinMid
 *
 * Notes : From register 0xF06, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_min_mid()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(WATCH_TIME_MIN_MID_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_min_mid
 *
 * IN   : param, a value to write to the regiter WatchTimeMinMid
 * OUT  : 
 *
 * RET  : Return the value of register 0xWatchTimeMinMid
 *
 * Notes : From register 0xF06, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_min_mid( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(WATCH_TIME_MIN_MID_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(WATCH_TIME_MIN_MID_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_watch_time_min_high
 *
 * RET  : Return the value of register 0xWatchTimeMinHigh
 *
 * Notes : From register 0xF07, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_min_high()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(WATCH_TIME_MIN_HIGH_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_min_high
 *
 * IN   : param, a value to write to the regiter WatchTimeMinHigh
 * OUT  : 
 *
 * RET  : Return the value of register 0xWatchTimeMinHigh
 *
 * Notes : From register 0xF07, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_min_high( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(WATCH_TIME_MIN_HIGH_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(WATCH_TIME_MIN_HIGH_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_alarm_min_low
 *
 * RET  : Return the value of register 0xAlarmMinLow
 *
 * Notes : From register 0xF08, bits 7:0
 *
 **************************************************************************/
unsigned char get_alarm_min_low()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ALARM_MIN_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_alarm_min_low
 *
 * IN   : param, a value to write to the regiter AlarmMinLow
 * OUT  : 
 *
 * RET  : Return the value of register 0xAlarmMinLow
 *
 * Notes : From register 0xF08, bits 7:0
 *
 **************************************************************************/
unsigned char set_alarm_min_low( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ALARM_MIN_LOW_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(ALARM_MIN_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_alarm_min_mid
 *
 * RET  : Return the value of register 0xAlarmMinMid
 *
 * Notes : From register 0xF09, bits 7:0
 *
 **************************************************************************/
unsigned char get_alarm_min_mid()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ALARM_MIN_MID_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_alarm_min_mid
 *
 * IN   : param, a value to write to the regiter AlarmMinMid
 * OUT  : 
 *
 * RET  : Return the value of register 0xAlarmMinMid
 *
 * Notes : From register 0xF09, bits 7:0
 *
 **************************************************************************/
unsigned char set_alarm_min_mid( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ALARM_MIN_MID_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(ALARM_MIN_MID_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_alarm_min_high
 *
 * RET  : Return the value of register 0xAlarmMinHigh
 *
 * Notes : From register 0xF0A, bits 7:0
 *
 **************************************************************************/
unsigned char get_alarm_min_high()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ALARM_MIN_HIGH_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_alarm_min_high
 *
 * IN   : param, a value to write to the regiter AlarmMinHigh
 * OUT  : 
 *
 * RET  : Return the value of register 0xAlarmMinHigh
 *
 * Notes : From register 0xF0A, bits 7:0
 *
 **************************************************************************/
unsigned char set_alarm_min_high( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ALARM_MIN_HIGH_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(ALARM_MIN_HIGH_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_rtc_status_data
 *
 * IN   : param, a value to write to the regiter RtcStatus
 * OUT  : 
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 0:0
 *
 **************************************************************************/
unsigned char set_rtc_status_data( enum rtc_status_data param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_STATUS_REG);

    /* 
     * Set at 1 by Software and then reset if RTC supply fail 
     */ 
    switch( param ){
        case SET_ONE_E: 
           value = old_value | RTC_STATUS_DATA_MASK; 
           break;
        case DO_NOT_USE_E: 
           value = old_value & ~ RTC_STATUS_DATA_MASK;
           break;
    }
  

    I2CWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_status_data
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 0:0
 *
 **************************************************************************/
unsigned char get_rtc_status_data()
  {
    unsigned char value;


    /* 
     * Set at 1 by Software and then reset if RTC supply fail 
     */ 
    value = (I2CRead(RTC_STATUS_REG) & RTC_STATUS_DATA_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_alarm
 *
 * IN   : param, a value to write to the regiter RtcStatus
 * OUT  : 
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 2:2
 *
 **************************************************************************/
unsigned char set_rtc_alarm( enum rtc_alarm_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_STATUS_REG);

    /* 
     * Enable/Disable RTC Alarm 
     */ 
    switch( param ){
        case RTC_ALARM_ENABLE_E: 
           value = old_value | RTC_ALARM_MASK; 
           break;
        case RTC_ALARM_DISABLE_E: 
           value = old_value & ~ RTC_ALARM_MASK;
           break;
    }
  

    I2CWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_alarm
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 2:2
 *
 **************************************************************************/
unsigned char get_rtc_alarm()
  {
    unsigned char value;


    /* 
     * Enable/Disable RTC Alarm 
     */ 
    value = (I2CRead(RTC_STATUS_REG) & RTC_ALARM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_present
 *
 * IN   : param, a value to write to the regiter RtcStatus
 * OUT  : 
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 3:3
 *
 **************************************************************************/
unsigned char set_backup_present( enum backup_present_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_STATUS_REG);

    /* 
     * Enable/Disable Backup Present 
     */ 
    switch( param ){
        case BACKUP_PRESENT_ENABLE_E: 
           value = old_value | BACKUP_PRESENT_MASK; 
           break;
        case BACKUP_PRESENT_DISABLE_E: 
           value = old_value & ~ BACKUP_PRESENT_MASK;
           break;
    }
  

    I2CWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_present
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 3:3
 *
 **************************************************************************/
unsigned char get_backup_present()
  {
    unsigned char value;


    /* 
     * Enable/Disable Backup Present 
     */ 
    value = (I2CRead(RTC_STATUS_REG) & BACKUP_PRESENT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_charger
 *
 * IN   : param, a value to write to the regiter RtcStatus
 * OUT  : 
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 4:4
 *
 **************************************************************************/
unsigned char set_backup_charger( enum backup_charger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_STATUS_REG);

    /* 
     * Enable/Disable Backup charger 
     */ 
    switch( param ){
        case BACKUP_CHARGER_ENABLE_E: 
           value = old_value | BACKUP_CHARGER_MASK; 
           break;
        case BACKUP_CHARGER_DISABLE_E: 
           value = old_value & ~ BACKUP_CHARGER_MASK;
           break;
    }
  

    I2CWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_charger
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 4:4
 *
 **************************************************************************/
unsigned char get_backup_charger()
  {
    unsigned char value;


    /* 
     * Enable/Disable Backup charger 
     */ 
    value = (I2CRead(RTC_STATUS_REG) & BACKUP_CHARGER_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charge_backup_battery
 *
 * IN   : param, a value to write to the regiter RtcStatus
 * OUT  : 
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 5:5
 *
 **************************************************************************/
unsigned char set_charge_backup_battery( enum charge_backup_battery_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_STATUS_REG);

    /* 
     * 0: Current source to charge back up battery disabled in 
     * OFF mode 
     * 1: Current source to charge back up battery enabled in 
     * OFF mode 
     */ 
    switch( param ){
        case CHARGE_BACKUP_BATTERY_ENABLE_E: 
           value = old_value | CHARGE_BACKUP_BATTERY_MASK; 
           break;
        case CHARGE_BACKUP_BATTERY_DISABLE_E: 
           value = old_value & ~ CHARGE_BACKUP_BATTERY_MASK;
           break;
    }
  

    I2CWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charge_backup_battery
 *
 * RET  : Return the value of register 0xRtcStatus
 *
 * Notes : From register 0xF0B, bits 5:5
 *
 **************************************************************************/
unsigned char get_charge_backup_battery()
  {
    unsigned char value;


    /* 
     * 0: Current source to charge back up battery disabled in 
     * OFF mode 
     * 1: Current source to charge back up battery enabled in 
     * OFF mode 
     */ 
    value = (I2CRead(RTC_STATUS_REG) & CHARGE_BACKUP_BATTERY_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_voltage_select
 *
 * IN   : param, a value to write to the regiter RTCBackupCharger
 * OUT  : 
 *
 * RET  : Return the value of register 0xRTCBackupCharger
 *
 * Notes : From register 0xF0C, bits 1:0
 *
 **************************************************************************/
unsigned char set_backup_voltage_select( enum backup_voltage_select param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_BACKUP_CHARGER_REG);

    /* 
     * Backup charger voltage selection 
     */ 
  

     value =  old_value & ~BACKUP_VOLTAGE_SELECT_MASK ;

    switch(  param ){

           case BACKUP_CHARGER_2V5_E:
                value =  value | BACKUP_CHARGER_2V5 ;
                break;
           case BACKUP_CHARGER_2V6_E:
                value =  value | BACKUP_CHARGER_2V6 ;
                break;
           case BACKUP_CHARGER_2V8_E:
                value =  value | BACKUP_CHARGER_2V8 ;
                break;
           case BACKUP_CHARGER_3V1_E:
                value =  value | BACKUP_CHARGER_3V1 ;
                break;
    }
  

    I2CWrite(RTC_BACKUP_CHARGER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_voltage_select
 *
 * RET  : Return the value of register 0xRTCBackupCharger
 *
 * Notes : From register 0xF0C, bits 1:0
 *
 **************************************************************************/
unsigned char get_backup_voltage_select()
  {
    unsigned char value;


    /* 
     * Backup charger voltage selection 
     */ 
    value = (I2CRead(RTC_BACKUP_CHARGER_REG) & BACKUP_VOLTAGE_SELECT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_current_select
 *
 * IN   : param, a value to write to the regiter RTCBackupCharger
 * OUT  : 
 *
 * RET  : Return the value of register 0xRTCBackupCharger
 *
 * Notes : From register 0xF0C, bits 3:2
 *
 **************************************************************************/
unsigned char set_backup_current_select( enum backup_current_select param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_BACKUP_CHARGER_REG);

    /* 
     * Backup charger current intensity selection 
     */ 
  

     value =  old_value & ~BACKUP_CURRENT_SELECT_MASK ;

    switch(  param ){

           case BACKUP_CHARGER_50_UA_E:
                value  = value  | (BACKUP_CHARGER_50_UA << 0x2);
                break;
           case BACKUP_CHARGER_150_UA_E:
                value  = value  | (BACKUP_CHARGER_150_UA << 0x2);
                break;
           case BACKUP_CHARGER_300_UA_E:
                value  = value  | (BACKUP_CHARGER_300_UA << 0x2);
                break;
           case BACKUP_CHARGER_700UA_E:
                value  = value  | (BACKUP_CHARGER_700UA << 0x2);
                break;
    }
  

    I2CWrite(RTC_BACKUP_CHARGER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_current_select
 *
 * RET  : Return the value of register 0xRTCBackupCharger
 *
 * Notes : From register 0xF0C, bits 3:2
 *
 **************************************************************************/
unsigned char get_backup_current_select()
  {
    unsigned char value;


    /* 
     * Backup charger current intensity selection 
     */ 
    value = (I2CRead(RTC_BACKUP_CHARGER_REG) & BACKUP_CURRENT_SELECT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_force
 *
 * IN   : param, a value to write to the regiter RtcCForceBackup
 * OUT  : 
 *
 * RET  : Return the value of register 0xRtcCForceBackup
 *
 * Notes : From register 0xF0D, bits 0:0
 *
 **************************************************************************/
unsigned char set_backup_force( enum backup_force param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RTC_C_FORCE_BACKUP_REG);

    /* 
     * Force backup battery as supply for RTC block 
     */ 
    switch( param ){
        case RTC_FORCE_BACKUP_E: 
           value = old_value | BACKUP_FORCE_MASK; 
           break;
        case RTC_BACKUP_INACTIVE_E: 
           value = old_value & ~ BACKUP_FORCE_MASK;
           break;
    }
  

    I2CWrite(RTC_C_FORCE_BACKUP_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_force
 *
 * RET  : Return the value of register 0xRtcCForceBackup
 *
 * Notes : From register 0xF0D, bits 0:0
 *
 **************************************************************************/
unsigned char get_backup_force()
  {
    unsigned char value;


    /* 
     * Force backup battery as supply for RTC block 
     */ 
    value = (I2CRead(RTC_C_FORCE_BACKUP_REG) & BACKUP_FORCE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_rtc_derivation
 *
 * RET  : Return the value of register 0xRtcDerivation
 *
 * Notes : From register 0xF0E, bits 7:0
 *
 **************************************************************************/
unsigned char get_rtc_derivation()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(RTC_DERIVATION_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_derivation
 *
 * IN   : param, a value to write to the regiter RtcDerivation
 * OUT  : 
 *
 * RET  : Return the value of register 0xRtcDerivation
 *
 * Notes : From register 0xF0E, bits 7:0
 *
 **************************************************************************/
unsigned char set_rtc_derivation( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(RTC_DERIVATION_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(RTC_DERIVATION_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_clock32_status
 *
 * RET  : Return the value of register 0xRtcClockStatus
 *
 * Notes : From register 0xF0F, bits 1:0
 *
 **************************************************************************/
unsigned char get_clock32_status()
  {
    unsigned char value;


    /* 
     * 00: no clock available (only possible in test mode) 
     * 01: internal 32Khz RC oscillator selected 
     * 10: internal 32khz Xtal oscillator selected (when Xtal 
     * selected, internal RC oscillator is turnedoff) 
     */ 
    value = (I2CRead(RTC_CLOCK_STATUS_REG) & CLOCK_32_STATUS_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_oscillator32k_status
 *
 * RET  : Return the value of register 0xRtcClockStatus
 *
 * Notes : From register 0xF0F, bits 2:2
 *
 **************************************************************************/
unsigned char get_oscillator32k_status()
  {
    unsigned char value;


    /* 
     * 0: Xtal 32khz oscillator is stopped 
     * 1: Xtal 32khz oscillator is running 
     */ 
    value = (I2CRead(RTC_CLOCK_STATUS_REG) & OSCILLATOR_3_2K_STATUS_MASK) >> 2;
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
