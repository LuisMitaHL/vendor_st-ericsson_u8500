/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_RTC/Linux/ab9540_RTC.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_RTC_LINUX_AB9540_RTC_H
#define C__LISP_PRODUCTION_AB9540_AB9540_RTC_LINUX_AB9540_RTC_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_RTC_types.h"

unsigned char set_backup_voltage_select( enum backup_voltage_select param );
unsigned char get_backup_voltage_select();
unsigned char set_backup_current_select( enum backup_current_select param );
unsigned char get_backup_current_select();
unsigned char get_switch_cmd_off();
unsigned char get_rtc_thermal_protection();
unsigned char get_battery_very_low();
unsigned char get_wathgod_expired();
unsigned char get_no_signal_c_lock32();
unsigned char get_low_battery();
unsigned char get_long_pressure();
unsigned char get_rtc_thermal_shutdown();
unsigned char set_rtc_read_request( enum rtc_read_enable param );
unsigned char get_rtc_read_request();
unsigned char set_rtc_write_request( enum rtc_write_enable param );
unsigned char get_rtc_write_request();
unsigned char set_rtc_status_data( enum rtc_status_data param );
unsigned char get_rtc_status_data();
unsigned char set_rtc_alarm( enum rtc_alarm_enable param );
unsigned char get_rtc_alarm();
unsigned char set_backup_present( enum backup_present_enable param );
unsigned char get_backup_present();
unsigned char set_backup_charger( enum backup_charger_enable param );
unsigned char get_backup_charger();
unsigned char set_charge_backup_battery( enum charge_backup_battery_enable param );
unsigned char get_charge_backup_battery();
unsigned char set_backup_force( enum backup_force param );
unsigned char get_backup_force();
unsigned char set_coulomb_counter_power_off( enum coulomb_counter_power_off_enable param );
unsigned char get_coulomb_counter_power_off();
unsigned char get_clock32_status();
unsigned char get_oscillator32k_status();


#ifdef __cplusplus
}
#endif

#endif
