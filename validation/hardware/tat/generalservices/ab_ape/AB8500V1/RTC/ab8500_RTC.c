/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_RTC/Linux/ab8500_RTC.c
 * 
 *
 * Generated on the 26/02/2010 08:57 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/
#include "ab8500_RTC.h"

extern unsigned char SPIRead( unsigned short register_address);
extern void SPIWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_long_pressure
 *
 * RET  : Return the value of register SwitchOffStatus
 *
 * Notes : From register 0x0F00, bits 6:6
 *
 **************************************************************************/
unsigned char get_long_pressure()
  {
    unsigned char value;


    /* 
     * STw4500 has switched off with a PonKey1 pressure longer 
     * than 10 s. 
     */ 
    value = (SPIRead(SWITCH_OFF_STATUS_REG) & LONG_PRESSURE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_low_battery
 *
 * RET  : Return the value of register SwitchOffStatus
 *
 * Notes : From register 0x0F00, bits 5:5
 *
 **************************************************************************/
unsigned char get_low_battery()
  {
    unsigned char value;


    /* 
     * STw4500 has switched off due to a battery level lower 
     * than PornVbat threshold. 
     */ 
    value = (SPIRead(SWITCH_OFF_STATUS_REG) & LOW_BATTERY_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_no_signal_c_lock32
 *
 * RET  : Return the value of register SwitchOffStatus
 *
 * Notes : From register 0x0F00, bits 4:4
 *
 **************************************************************************/
unsigned char get_no_signal_c_lock32()
  {
    unsigned char value;


    /* 
     * STw4500 has switched off due to the non presence of 32 
     * Khz clock. 
     */ 
    value = (SPIRead(SWITCH_OFF_STATUS_REG) & NO_SIGNAL_C_LOCK_32_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_wathgod_expired
 *
 * RET  : Return the value of register SwitchOffStatus
 *
 * Notes : From register 0x0F00, bits 3:3
 *
 **************************************************************************/
unsigned char get_wathgod_expired()
  {
    unsigned char value;


    /* 
     * STw4500 has switched off due to primary watchdog has 
     * expired. 
     */ 
    value = (SPIRead(SWITCH_OFF_STATUS_REG) & WATHGOD_EXPIRED_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_battery_very_low
 *
 * RET  : Return the value of register SwitchOffStatus
 *
 * Notes : From register 0x0F00, bits 2:2
 *
 **************************************************************************/
unsigned char get_battery_very_low()
  {
    unsigned char value;


    /* 
     * STw4500 has switched off due to a VbatA ball level lower 
     * than BattOk falling threshold. 
     */ 
    value = (SPIRead(SWITCH_OFF_STATUS_REG) & BATTERY_VERY_LOW_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_rtc_thermal_protection
 *
 * RET  : Return the value of register SwitchOffStatus
 *
 * Notes : From register 0x0F00, bits 1:1
 *
 **************************************************************************/
unsigned char get_rtc_thermal_protection()
  {
    unsigned char value;


    /* 
     * STw4500 has switched off due to Thermal protection 
     * activation. 
     */ 
    value = (SPIRead(SWITCH_OFF_STATUS_REG) & THERMAL_PROTECTION_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_switch_cmd_off
 *
 * RET  : Return the value of register SwitchOffStatus
 *
 * Notes : From register 0x0F00, bits 0:0
 *
 **************************************************************************/
unsigned char get_switch_cmd_off()
  {
    unsigned char value;


    /* 
     * STw4500 has switched off with ‘Swoff’.bit of STw4500Ctrl1 
     * register. 
     */ 
    value = (SPIRead(SWITCH_OFF_STATUS_REG) & SWITCH_CMD_OFF_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_low_power
 *
 * IN   : param, a value to write to the regiter CoulombCounter
 * OUT  : 
 *
 * RET  : Return the value of register CoulombCounter
 *
 * Notes : From register 0x0F01, bits 1:1
 *
 **************************************************************************/
unsigned char set_coulomb_counter_low_power( enum coulomb_counter_low_power_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(COULOMB_COUNTER_REG);

    /* 
     * 0: allow to disable automatically CoulombCounter in 
     * STw4500 deep sleep mode 
     * 1: allow to keep CoulombCounter enable in STw4500 deep 
     * sleep mode 
     */ 
    switch( param )
      {
        case COULOMB_COUNTER_LOW_POWER_UP_E: 
           value = old_value | COULOMB_COUNTER_LOW_POWER_ENABLE_PARAM_MASK; 
           break;
        case COULOMB_COUNTER_LOW_POWER_DOWN_E: 
           value = old_value & ~ COULOMB_COUNTER_LOW_POWER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(COULOMB_COUNTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_low_power
 *
 * RET  : Return the value of register CoulombCounter
 *
 * Notes : From register 0x0F01, bits 1:1
 *
 **************************************************************************/
unsigned char get_coulomb_counter_low_power()
  {
    unsigned char value;


    /* 
     * 0: allow to disable automatically CoulombCounter in 
     * STw4500 deep sleep mode 
     * 1: allow to keep CoulombCounter enable in STw4500 deep 
     * sleep mode 
     */ 
    value = (SPIRead(COULOMB_COUNTER_REG) & COULOMB_COUNTER_LOW_POWER_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_power_off
 *
 * IN   : param, a value to write to the regiter CoulombCounter
 * OUT  : 
 *
 * RET  : Return the value of register CoulombCounter
 *
 * Notes : From register 0x0F01, bits 0:0
 *
 **************************************************************************/
unsigned char set_coulomb_counter_power_off( enum coulomb_counter_power_off_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(COULOMB_COUNTER_REG);

    /* 
     * Note: Coulomb Counter always disable in PwrOff mode. 
     * 0: disable Coulomb Counter 
     * 1: enable Coulomb Counter 
     */ 
    switch( param )
      {
        case COULOMB_COUNTER_POWER_UP_E: 
           value = old_value | COULOMB_COUNTER_POWER_OFF_ENABLE_PARAM_MASK; 
           break;
        case COULOMB_COUNTER_POWER_DOWN_E: 
           value = old_value & ~ COULOMB_COUNTER_POWER_OFF_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(COULOMB_COUNTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_power_off
 *
 * RET  : Return the value of register CoulombCounter
 *
 * Notes : From register 0x0F01, bits 0:0
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
    value = (SPIRead(COULOMB_COUNTER_REG) & COULOMB_COUNTER_POWER_OFF_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_write_request
 *
 * IN   : param, a value to write to the regiter RTCReadRequest
 * OUT  : 
 *
 * RET  : Return the value of register RTCReadRequest
 *
 * Notes : From register 0x0F02, bits 1:1
 *
 **************************************************************************/
unsigned char set_rtc_write_request( enum rtc_write_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_READ_REQUEST_REG);

    /* 
     * RTC data write transfer is required. 
     * Cleared upon data transfert in watchtim register is done 
     */ 
    switch( param )
      {
        case RTC_WRITE_ENABLE_E: 
           value = old_value | RTC_WRITE_ENABLE_PARAM_MASK; 
           break;
        case RTC_WRITE_DISABLE_E: 
           value = old_value & ~ RTC_WRITE_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(RTC_READ_REQUEST_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_write_request
 *
 * RET  : Return the value of register RTCReadRequest
 *
 * Notes : From register 0x0F02, bits 1:1
 *
 **************************************************************************/
unsigned char get_rtc_write_request()
  {
    unsigned char value;


    /* 
     * RTC data write transfer is required. 
     * Cleared upon data transfert in watchtim register is done 
     */ 
    value = (SPIRead(RTC_READ_REQUEST_REG) & RTC_WRITE_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_read_request
 *
 * IN   : param, a value to write to the regiter RTCReadRequest
 * OUT  : 
 *
 * RET  : Return the value of register RTCReadRequest
 *
 * Notes : From register 0x0F02, bits 0:0
 *
 **************************************************************************/
unsigned char set_rtc_read_request( enum rtc_read_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_READ_REQUEST_REG);

    /* 
     * RTC data read transfer is required 
     * Cleared upon data transfert in watchtim register is done 
     */ 
    switch( param )
      {
        case RTC_READ_ENABLE_E: 
           value = old_value | RTC_READ_ENABLE_PARAM_MASK; 
           break;
        case RTC_READ_DISABLE_E: 
           value = old_value & ~ RTC_READ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(RTC_READ_REQUEST_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_read_request
 *
 * RET  : Return the value of register RTCReadRequest
 *
 * Notes : From register 0x0F02, bits 0:0
 *
 **************************************************************************/
unsigned char get_rtc_read_request()
  {
    unsigned char value;


    /* 
     * RTC data read transfer is required 
     * Cleared upon data transfert in watchtim register is done 
     */ 
    value = (SPIRead(RTC_READ_REQUEST_REG) & RTC_READ_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_sec_low
 *
 * IN   : param, a value to write to the regiter WatchTimeSecLow
 * OUT  : 
 *
 * RET  : Return the value of register WatchTimeSecLow
 *
 * Notes : From register 0x0F03, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_sec_low( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(WATCH_TIME_SEC_LOW_REG);

    /* 
     * Bits [7:0] for seconds of WatchTime data 
     */ 
    SPIWrite(WATCH_TIME_SEC_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_watch_time_sec_low
 *
 * RET  : Return the value of register WatchTimeSecLow
 *
 * Notes : From register 0x0F03, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_sec_low()
  {
    unsigned char value;


    /* 
     * Bits [7:0] for seconds of WatchTime data 
     */ 
    value = SPIRead(WATCH_TIME_SEC_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_sec_high
 *
 * IN   : param, a value to write to the regiter WatchTimeSecHigh
 * OUT  : 
 *
 * RET  : Return the value of register WatchTimeSecHigh
 *
 * Notes : From register 0x0F04, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_sec_high( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(WATCH_TIME_SEC_HIGH_REG);

    /* 
     * Bits [15:8] for seconds of WatchTime data 
     */ 
    SPIWrite(WATCH_TIME_SEC_HIGH_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_watch_time_sec_high
 *
 * RET  : Return the value of register WatchTimeSecHigh
 *
 * Notes : From register 0x0F04, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_sec_high()
  {
    unsigned char value;


    /* 
     * Bits [15:8] for seconds of WatchTime data 
     */ 
    value = SPIRead(WATCH_TIME_SEC_HIGH_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_min_low
 *
 * IN   : param, a value to write to the regiter WatchTimeMinLow
 * OUT  : 
 *
 * RET  : Return the value of register WatchTimeMinLow
 *
 * Notes : From register 0x0F05, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_min_low( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(WATCH_TIME_MIN_LOW_REG);

    /* 
     * Bits [7:0] for minutes of WatchTime data 
     */ 
    SPIWrite(WATCH_TIME_MIN_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_watch_time_min_low
 *
 * RET  : Return the value of register WatchTimeMinLow
 *
 * Notes : From register 0x0F05, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_min_low()
  {
    unsigned char value;


    /* 
     * Bits [7:0] for minutes of WatchTime data 
     */ 
    value = SPIRead(WATCH_TIME_MIN_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_min_mid
 *
 * IN   : param, a value to write to the regiter WatchTimeMinMid
 * OUT  : 
 *
 * RET  : Return the value of register WatchTimeMinMid
 *
 * Notes : From register 0x0F06, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_min_mid( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(WATCH_TIME_MIN_MID_REG);

    /* 
     * Bits [15:8] for minutes of WatchTime data 
     */ 
    SPIWrite(WATCH_TIME_MIN_MID_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_watch_time_min_mid
 *
 * RET  : Return the value of register WatchTimeMinMid
 *
 * Notes : From register 0x0F06, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_min_mid()
  {
    unsigned char value;


    /* 
     * Bits [15:8] for minutes of WatchTime data 
     */ 
    value = SPIRead(WATCH_TIME_MIN_MID_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watch_time_min_high
 *
 * IN   : param, a value to write to the regiter WatchTimeMinHigh
 * OUT  : 
 *
 * RET  : Return the value of register WatchTimeMinHigh
 *
 * Notes : From register 0x0F07, bits 7:0
 *
 **************************************************************************/
unsigned char set_watch_time_min_high( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(WATCH_TIME_MIN_HIGH_REG);

    /* 
     * Bits [23:16] for minutes of WatchTime data 
     */ 
    SPIWrite(WATCH_TIME_MIN_HIGH_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_watch_time_min_high
 *
 * RET  : Return the value of register WatchTimeMinHigh
 *
 * Notes : From register 0x0F07, bits 7:0
 *
 **************************************************************************/
unsigned char get_watch_time_min_high()
  {
    unsigned char value;


    /* 
     * Bits [23:16] for minutes of WatchTime data 
     */ 
    value = SPIRead(WATCH_TIME_MIN_HIGH_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_alarm_min_low
 *
 * IN   : param, a value to write to the regiter AlarmMinLow
 * OUT  : 
 *
 * RET  : Return the value of register AlarmMinLow
 *
 * Notes : From register 0x0F08, bits 7:0
 *
 **************************************************************************/
unsigned char set_alarm_min_low( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(ALARM_MIN_LOW_REG);

    /* 
     * Bits [7:0] for minutes of Alarm data 
     */ 
    SPIWrite(ALARM_MIN_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_alarm_min_low
 *
 * RET  : Return the value of register AlarmMinLow
 *
 * Notes : From register 0x0F08, bits 7:0
 *
 **************************************************************************/
unsigned char get_alarm_min_low()
  {
    unsigned char value;


    /* 
     * Bits [7:0] for minutes of Alarm data 
     */ 
    value = SPIRead(ALARM_MIN_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_alarm_min_mid
 *
 * IN   : param, a value to write to the regiter AlarmMinMid
 * OUT  : 
 *
 * RET  : Return the value of register AlarmMinMid
 *
 * Notes : From register 0x0F09, bits 7:0
 *
 **************************************************************************/
unsigned char set_alarm_min_mid( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(ALARM_MIN_MID_REG);

    /* 
     * Bits [15:8] for minutes of Alarm data 
     */ 
    SPIWrite(ALARM_MIN_MID_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_alarm_min_mid
 *
 * RET  : Return the value of register AlarmMinMid
 *
 * Notes : From register 0x0F09, bits 7:0
 *
 **************************************************************************/
unsigned char get_alarm_min_mid()
  {
    unsigned char value;


    /* 
     * Bits [15:8] for minutes of Alarm data 
     */ 
    value = SPIRead(ALARM_MIN_MID_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_alarm_min_high
 *
 * IN   : param, a value to write to the regiter AlarmMinHigh
 * OUT  : 
 *
 * RET  : Return the value of register AlarmMinHigh
 *
 * Notes : From register 0x0F0A, bits 7:0
 *
 **************************************************************************/
unsigned char set_alarm_min_high( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(ALARM_MIN_HIGH_REG);

    /* 
     * Bits [23:16] for minutes of Alarm data 
     */ 
    SPIWrite(ALARM_MIN_HIGH_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_alarm_min_high
 *
 * RET  : Return the value of register AlarmMinHigh
 *
 * Notes : From register 0x0F0A, bits 7:0
 *
 **************************************************************************/
unsigned char get_alarm_min_high()
  {
    unsigned char value;


    /* 
     * Bits [23:16] for minutes of Alarm data 
     */ 
    value = SPIRead(ALARM_MIN_HIGH_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charge_backup_battery
 *
 * IN   : param, a value to write to the regiter RTCStatus
 * OUT  : 
 *
 * RET  : Return the value of register RTCStatus
 *
 * Notes : From register 0x0F0B, bits 5:5
 *
 **************************************************************************/
unsigned char set_charge_backup_battery( enum charge_backup_battery_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_STATUS_REG);

    /* 
     * 0: Current source to charge back up battery disabled in 
     * OFF mode 
     * 1: Current source to charge back up battery enabled in 
     * OFF mode 
     */ 
    switch( param )
      {
        case CHARGE_BACKUP_BATTERY_ENABLE_E: 
           value = old_value | CHARGE_BACKUP_BATTERY_ENABLE_PARAM_MASK; 
           break;
        case CHARGE_BACKUP_BATTERY_DISABLE_E: 
           value = old_value & ~ CHARGE_BACKUP_BATTERY_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charge_backup_battery
 *
 * RET  : Return the value of register RTCStatus
 *
 * Notes : From register 0x0F0B, bits 5:5
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
    value = (SPIRead(RTC_STATUS_REG) & CHARGE_BACKUP_BATTERY_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_charger_enable
 *
 * IN   : param, a value to write to the regiter RtcStatus
 * OUT  : 
 *
 * RET  : Return the value of register RtcStatus
 *
 * Notes : From register 0x0F0B, bits 4:4
 *
 **************************************************************************/
unsigned char set_backup_charger_enable( enum backup_charger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_STATUS_REG);

    /* 
     * Enable/Disable Backup charger 
     */ 
    switch( param )
      {
        case BACKUP_CHARGER_ENABLE_E: 
           value = old_value | BACKUP_CHARGER_ENABLE_PARAM_MASK; 
           break;
        case BACKUP_CHARGER_DISABLE_E: 
           value = old_value & ~ BACKUP_CHARGER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_charger_enable
 *
 * RET  : Return the value of register RtcStatus
 *
 * Notes : From register 0x0F0B, bits 4:4
 *
 **************************************************************************/
unsigned char get_backup_charger_enable()
  {
    unsigned char value;


    /* 
     * Enable/Disable Backup charger 
     */ 
    value = (SPIRead(RTC_STATUS_REG) & BACKUP_CHARGER_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_present_enable
 *
 * IN   : param, a value to write to the regiter RtcStatus
 * OUT  : 
 *
 * RET  : Return the value of register RtcStatus
 *
 * Notes : From register 0x0F0B, bits 3:3
 *
 **************************************************************************/
unsigned char set_backup_present_enable( enum backup_present_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_STATUS_REG);

    /* 
     * Enable/Disable Backup Present 
     */ 
    switch( param )
      {
        case BACKUP_PRESENT_ENABLE_E: 
           value = old_value | BACKUP_PRESENT_ENABLE_PARAM_MASK; 
           break;
        case BACKUP_PRESENT_DISABLE_E: 
           value = old_value & ~ BACKUP_PRESENT_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_present_enable
 *
 * RET  : Return the value of register RtcStatus
 *
 * Notes : From register 0x0F0B, bits 3:3
 *
 **************************************************************************/
unsigned char get_backup_present_enable()
  {
    unsigned char value;


    /* 
     * Enable/Disable Backup Present 
     */ 
    value = (SPIRead(RTC_STATUS_REG) & BACKUP_PRESENT_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_alarm_enable
 *
 * IN   : param, a value to write to the regiter RtcStatus
 * OUT  : 
 *
 * RET  : Return the value of register RtcStatus
 *
 * Notes : From register 0x0F0B, bits 2:2
 *
 **************************************************************************/
unsigned char set_rtc_alarm_enable( enum rtc_alarm_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_STATUS_REG);

    /* 
     * Enable/Disable RTC Alarm 
     */ 
    switch( param )
      {
        case RTC_ALARM_ENABLE_E: 
           value = old_value | RTC_ALARM_ENABLE_PARAM_MASK; 
           break;
        case RTC_ALARM_DISABLE_E: 
           value = old_value & ~ RTC_ALARM_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_alarm_enable
 *
 * RET  : Return the value of register RtcStatus
 *
 * Notes : From register 0x0F0B, bits 2:2
 *
 **************************************************************************/
unsigned char get_rtc_alarm_enable()
  {
    unsigned char value;


    /* 
     * Enable/Disable RTC Alarm 
     */ 
    value = (SPIRead(RTC_STATUS_REG) & RTC_ALARM_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_status_data
 *
 * IN   : param, a value to write to the regiter RtcStatus
 * OUT  : 
 *
 * RET  : Return the value of register RtcStatus
 *
 * Notes : From register 0x0F0B, bits 0:0
 *
 **************************************************************************/
unsigned char set_rtc_status_data( enum rtc_status_data param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_STATUS_REG);

    /* 
     * Set at 1 by Software and then reset if RTC supply fail 
     */ 
    switch( param )
      {
        case DO_NOT_USE_E: 
           value = old_value | RTC_STATUS_DATA_PARAM_MASK; 
           break;
        case SET_ONE_E: 
           value = old_value & ~ RTC_STATUS_DATA_PARAM_MASK;
           break;
      }
  

    SPIWrite(RTC_STATUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_status_data
 *
 * RET  : Return the value of register RtcStatus
 *
 * Notes : From register 0x0F0B, bits 0:0
 *
 **************************************************************************/
unsigned char get_rtc_status_data()
  {
    unsigned char value;


    /* 
     * Set at 1 by Software and then reset if RTC supply fail 
     */ 
    value = (SPIRead(RTC_STATUS_REG) & RTC_STATUS_DATA_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_current_select
 *
 * IN   : param, a value to write to the regiter RTCBackupCharger
 * OUT  : 
 *
 * RET  : Return the value of register RTCBackupCharger
 *
 * Notes : From register 0x0F0C, bits 3:2
 *
 **************************************************************************/
unsigned char set_backup_current_select( enum backup_current_select param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_BACKUP_CHARGER_REG);

    /* 
     * Backup charger current intensity selection 
     */ 
    SPIWrite(RTC_BACKUP_CHARGER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_current_select
 *
 * RET  : Return the value of register RTCBackupCharger
 *
 * Notes : From register 0x0F0C, bits 3:2
 *
 **************************************************************************/
unsigned char get_backup_current_select()
  {
    unsigned char value;


    /* 
     * Backup charger current intensity selection 
     */ 
    value = (SPIRead(RTC_BACKUP_CHARGER_REG) & BACKUP_CURRENT_SELECT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_voltage_select
 *
 * IN   : param, a value to write to the regiter RTCBackupCharger
 * OUT  : 
 *
 * RET  : Return the value of register RTCBackupCharger
 *
 * Notes : From register 0x0F0C, bits 1:0
 *
 **************************************************************************/
unsigned char set_backup_voltage_select( enum backup_voltage_select param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_BACKUP_CHARGER_REG);

    /* 
     * Backup charger voltage selection 
     */ 
    SPIWrite(RTC_BACKUP_CHARGER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_voltage_select
 *
 * RET  : Return the value of register RTCBackupCharger
 *
 * Notes : From register 0x0F0C, bits 1:0
 *
 **************************************************************************/
unsigned char get_backup_voltage_select()
  {
    unsigned char value;


    /* 
     * Backup charger voltage selection 
     */ 
    value = (SPIRead(RTC_BACKUP_CHARGER_REG) & BACKUP_VOLTAGE_SELECT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_backup_force
 *
 * IN   : param, a value to write to the regiter RtcCForceBackup
 * OUT  : 
 *
 * RET  : Return the value of register RtcCForceBackup
 *
 * Notes : From register 0x0F0D, bits 0:0
 *
 **************************************************************************/
unsigned char set_backup_force( enum backup_force param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(RTC_C_FORCE_BACKUP_REG);

    /* 
     * Force backup battery as supply for RTC block 
     */ 
    switch( param )
      {
        case RTC_BACKUP_INACTIVE_E: 
           value = old_value | BACKUP_FORCE_PARAM_MASK; 
           break;
        case RTC_FORCE_BACKUP_E: 
           value = old_value & ~ BACKUP_FORCE_PARAM_MASK;
           break;
      }
  

    SPIWrite(RTC_C_FORCE_BACKUP_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_backup_force
 *
 * RET  : Return the value of register RtcCForceBackup
 *
 * Notes : From register 0x0F0D, bits 0:0
 *
 **************************************************************************/
unsigned char get_backup_force()
  {
    unsigned char value;


    /* 
     * Force backup battery as supply for RTC block 
     */ 
    value = (SPIRead(RTC_C_FORCE_BACKUP_REG) & BACKUP_FORCE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_derivation
 *
 * IN   : param, a value to write to the regiter RtcDerivation
 * OUT  : 
 *
 * RET  : Return the value of register RtcDerivation
 *
 * Notes : From register 0x0F0E, bits 7:0
 *
 **************************************************************************/
unsigned char set_rtc_derivation( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(RTC_DERIVATION_REG);

    /* 
     * Watchtime calibration register 
     * 1 LSB = 30.5 usec which corresponds to 0.5 ppm of 32kHz 
     * clock 
     */ 
    SPIWrite(RTC_DERIVATION_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_rtc_derivation
 *
 * RET  : Return the value of register RtcDerivation
 *
 * Notes : From register 0x0F0E, bits 7:0
 *
 **************************************************************************/
unsigned char get_rtc_derivation()
  {
    unsigned char value;


    /* 
     * Watchtime calibration register 
     * 1 LSB = 30.5 usec which corresponds to 0.5 ppm of 32kHz 
     * clock 
     */ 
    value = SPIRead(RTC_DERIVATION_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_clock32_status
 *
 * RET  : Return the value of register RtcDerivation
 *
 * Notes : From register 0x0F0F, bits 1:0
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
    value = (SPIRead(RTC_DERIVATION_REG) & CLOCK_32_STATUS_PARAM_MASK);
    return value;
  }


