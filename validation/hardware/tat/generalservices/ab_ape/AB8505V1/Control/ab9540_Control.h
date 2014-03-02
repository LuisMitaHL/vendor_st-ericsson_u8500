/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Control/Linux/ab9540_Control.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_CONTROL_LINUX_AB9540_CONTROL_H
#define C__LISP_PRODUCTION_AB9540_AB9540_CONTROL_LINUX_AB9540_CONTROL_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Control_types.h"

unsigned char set_ab_reset_pull_down( enum ab_reset_pull_down param );
unsigned char get_ab_reset_pull_down();
unsigned char set_iso_uicc_data_pull_down( enum iso_uicc_data_pull_down param );
unsigned char get_iso_uicc_data_pull_down();
unsigned char set_iso_uicc_clk_pull_down( enum iso_uicc_clock_pull_down param );
unsigned char get_iso_uicc_clk_pull_down();
unsigned char set_iso_uicc_io_control( enum iso_uicc_io_control_set param );
unsigned char get_iso_uicc_io_control();
unsigned char set_vaudio_selection( unsigned char param );
unsigned char get_vaudio_selection();
unsigned char set_debug_for_internal_use( unsigned char param );
unsigned char get_debug_for_internal_use();
unsigned char get_reset4500();
unsigned char get_software_reset_flag();
unsigned char get_hardware_reset_flag();
unsigned char get_mid_pression_exp();
unsigned char get_main_watch_dog();
unsigned char get_long_pression_exp();
unsigned char set_pull_down_usbu_icc( enum pull_down_usbu_icc param );
unsigned char get_pull_down_usbu_icc();
unsigned char set_varm_dither( enum varm_dither_enable param );
unsigned char get_varm_dither();
unsigned char set_vsmps3_dither( enum vsmps3_dither_enable param );
unsigned char get_vsmps3_dither();
unsigned char set_vsmps1_dither( enum vsmps1_dither_enable param );
unsigned char get_vsmps1_dither();
unsigned char set_vsmps2_dither( enum vsmps2_dither_enable param );
unsigned char get_vsmps2_dither();
unsigned char set_vmod_dither( enum vmod_dither_enable param );
unsigned char get_vmod_dither();
unsigned char set_vape_dither( enum vape_dither_enable param );
unsigned char get_vape_dither();
unsigned char set_dithering_delay( enum dithering_delay param );
unsigned char get_dithering_delay();
unsigned char set_low_battery_function( enum low_battery_function_enable param );
unsigned char get_low_battery_function();
unsigned char set_low_bat_threshold( enum low_bat_threshold param );
unsigned char get_low_bat_threshold();
unsigned char set_sysclk12_buf1( enum sysclk12_buf1_enable param );
unsigned char get_sysclk12_buf1();
unsigned char set_sysclk12_buf2( enum sysclk12_buf2_enable param );
unsigned char get_sysclk12_buf2();
unsigned char set_sysclk12_buf3( enum sysclk12_buf3_enable param );
unsigned char get_sysclk12_buf3();
unsigned char set_sysclk12_buf4( enum sysclk12_buf4_enable param );
unsigned char get_sysclk12_buf4();
unsigned char set_sys_clk12_buf_x_stre0( enum buffer_state param );
unsigned char get_sys_clk12_buf_x_stre0();
unsigned char set_sys_clk12_buf_x_stre1( enum buffer_state param );
unsigned char get_sys_clk12_buf_x_stre1();
unsigned char set_sys_clk12_buf_x_stre2( enum buffer_state param );
unsigned char get_sys_clk12_buf_x_stre2();
unsigned char set_sys_clk12_buf_x_stre3( enum buffer_state param );
unsigned char get_sys_clk12_buf_x_stre3();
unsigned char get_power_on_key1_press_time();
unsigned char set_reset_vaux1_valid( enum reset_valid param );
unsigned char get_reset_vaux1_valid();
unsigned char set_reset_vaux2_valid( enum reset_valid param );
unsigned char get_reset_vaux2_valid();
unsigned char set_reset_vaux3_valid( enum reset_valid param );
unsigned char get_reset_vaux3_valid();
unsigned char set_reset_vmod_valid( enum reset_valid param );
unsigned char get_reset_vmod_valid();
unsigned char set_reset_vext_supply_valid( enum reset_valid param );
unsigned char get_reset_vext_supply_valid();
unsigned char set_reset_vext_supply2_valid( enum reset_valid param );
unsigned char get_reset_vext_supply2_valid();
unsigned char set_reset_vext_supply3_valid( enum reset_valid param );
unsigned char get_reset_vext_supply3_valid();
unsigned char set_reset_vsmps1_valid( enum reset_valid param );
unsigned char get_reset_vsmps1_valid();
unsigned char set_sys_clk_req1_hiq( enum sys_clk_req1_hiq_enable param );
unsigned char get_sys_clk_req1_hiq();
unsigned char set_sys_clk_req2_hiq( enum sys_clk_req2_hiq_enable param );
unsigned char get_sys_clk_req2_hiq();
unsigned char set_sys_clk_req3_hiq( enum sys_clk_req3_hiq_enable param );
unsigned char get_sys_clk_req3_hiq();
unsigned char set_sys_clk_req4_hiq( enum sys_clk_req4_hiq_enable param );
unsigned char get_sys_clk_req4_hiq();
unsigned char set_sys_clk_req5_hiq( enum sys_clk_req5_hiq_enable param );
unsigned char get_sys_clk_req5_hiq();
unsigned char set_sys_clk_req6_hiq( enum sys_clk_req6_hiq_enable param );
unsigned char get_sys_clk_req6_hiq();
unsigned char set_sys_clk_req7_hiq( enum sys_clk_req7_hiq_enable param );
unsigned char get_sys_clk_req7_hiq();
unsigned char set_sys_clk_req8_hiq( enum sys_clk_req8_hiq_enable param );
unsigned char get_sys_clk_req8_hiq();
unsigned char set_power_on_key1_press_time2( unsigned char param );
unsigned char get_power_on_key1_press_time2();
unsigned char set_clock32( enum clock32_output_buffer_enable param );
unsigned char get_clock32();
unsigned char set_audio_reset( enum audio_reset param );
unsigned char get_audio_reset();
unsigned char set_thermal_shutdown( enum thermal_shutdown_enable param );
unsigned char get_thermal_shutdown();
unsigned char set_sys_clock_buf2_req8( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req8();
unsigned char set_sys_clock_buf3_req8( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req8();
unsigned char set_sys_clock_buf4_req8( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req8();
unsigned char set_ext_sup12_l_pn_clk_sel( enum configuration12 param );
unsigned char get_ext_sup12_l_pn_clk_sel();
unsigned char set_ext_sup3_l_pn_clk_sel( enum configuration3 param );
unsigned char get_ext_sup3_l_pn_clk_sel();
unsigned char set_interrupt8500_open_drain( enum ball_configuration param );
unsigned char get_interrupt8500_open_drain();
unsigned char get_battery_detect();
unsigned char get_power_on_key1_detect();
unsigned char get_power_on_key2_detect();
unsigned char get_rtc_alarm_detect();
unsigned char get_vbus_dectect();
unsigned char get_usb_id_detect();
unsigned char get_uart_factory_mode();
unsigned char set_system_clock_timer( enum clock_timer param );
unsigned char get_system_clock_timer();
unsigned char set_system_clock_timer_adjustement( enum timer_adjustement param );
unsigned char get_system_clock_timer_adjustement();
unsigned char set_battery_ok1_threshold( enum battery_ok1_threshold param );
unsigned char get_battery_ok1_threshold();
unsigned char set_battery_ok2_threshold( enum battery_ok2_threshold param );
unsigned char get_battery_ok2_threshold();
unsigned char set_vsmod_clock_sel( enum smps_ckock_phase param );
unsigned char get_vsmod_clock_sel();
unsigned char set_vsmps1_clock_sel( enum smps_ckock_phase param );
unsigned char get_vsmps1_clock_sel();
unsigned char set_vsmps2_clock_sel( enum smps_ckock_phase param );
unsigned char get_vsmps2_clock_sel();
unsigned char set_vsmps3_clock_sel( enum smps_ckock_phase param );
unsigned char get_vsmps3_clock_sel();
unsigned char set_levi_or_yllas( enum levi_or_yllas param );
unsigned char get_levi_or_yllas();
unsigned char set_swat( enum swat_enable param );
unsigned char get_swat();
unsigned char set_rf_off_timer( enum off_timer param );
unsigned char get_rf_off_timer();
unsigned char set_swat_bit5_set( enum swat_bit5_set param );
unsigned char get_swat_bit5_set();
unsigned char set_sys_ultra_low_power_selection( enum sys_ultra_low_power_selection param );
unsigned char get_sys_ultra_low_power_selection();
unsigned char set_ulp_request( enum ulp_request_enable param );
unsigned char get_ulp_request();
unsigned char set_system_clock_request( enum system_clock_request_enable param );
unsigned char get_system_clock_request();
unsigned char set_audio_clock( enum audio_clock_enable param );
unsigned char get_audio_clock();
unsigned char set_sys_clk_buf2_req( enum activate_sys_clock_buffer2 param );
unsigned char get_sys_clk_buf2_req();
unsigned char set_sys_clk_buf3_req( enum activate_sys_clock_buffer3 param );
unsigned char get_sys_clk_buf3_req();
unsigned char set_sys_clk_buf4_req( enum activate_sys_clock_buffer4 param );
unsigned char get_sys_clk_buf4_req();
unsigned char set_varm_clock_sel( enum smps_ckock_phase param );
unsigned char get_varm_clock_sel();
unsigned char set_vape_clock_sel( enum smps_ckock_phase param );
unsigned char get_vape_clock_sel();
unsigned char set_sys_clock_buf2_req2( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req2();
unsigned char set_sys_clock_buf3_req2( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req2();
unsigned char set_sys_clock_buf4_req2( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req2();
unsigned char set_main_watchdog_timer( enum main_watchdog_timer param );
unsigned char get_main_watchdog_timer();
unsigned char set_sys_clock_buf2_req3( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req3();
unsigned char set_sys_clock_buf3_req3( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req3();
unsigned char set_sys_clock_buf4_req3( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req3();
unsigned char set_force_mode_power_on( enum valid_reset param );
unsigned char get_force_mode_power_on();
unsigned char set_force_mode_power_status( enum valid_reset param );
unsigned char get_force_mode_power_status();
unsigned char set_software_dbb_reset( enum valid_reset param );
unsigned char get_software_dbb_reset();
unsigned char set_reset_vaux4_valid( enum valid_reset param );
unsigned char get_reset_vaux4_valid();
unsigned char set_sys_clock_buf2_req4( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req4();
unsigned char set_sys_clock_buf3_req4( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req4();
unsigned char set_sys_clock_buf4_req4( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req4();
unsigned char set_sys_clock_buf2_req5( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req5();
unsigned char set_sys_clock_buf3_req5( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req5();
unsigned char set_sys_clock_buf4_req5( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req5();
unsigned char set_sys_clock_request1_valid( enum sys_clock_request1_valid param );
unsigned char get_sys_clock_request1_valid();
unsigned char set_ultra_low_power_request1_valid( enum ultra_low_power_request1_enable param );
unsigned char get_ultra_low_power_request1_valid();
unsigned char set_usb_sys_clockr_request1_valid( enum usb_sys_clockr_request1_enable param );
unsigned char get_usb_sys_clockr_request1_valid();
unsigned char set_sys_clock_buf2_req1( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req1();
unsigned char set_sys_clock_buf3_req1( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req1();
unsigned char set_sys_clock_buf4_req1( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req1();
unsigned char set_vsim_sys_clk_req1_hiq( enum vsim_sys_clk_req1_hiq_enable param );
unsigned char get_vsim_sys_clk_req1_hiq();
unsigned char set_vsim_sys_clk_req2_hiq( enum vsim_sys_clk_req2_hiq_enable param );
unsigned char get_vsim_sys_clk_req2_hiq();
unsigned char set_vsim_sys_clk_req3_hiq( enum vsim_sys_clk_req3_hiq_enable param );
unsigned char get_vsim_sys_clk_req3_hiq();
unsigned char set_vsim_sys_clk_req4_hiq( enum vsim_sys_clk_req4_hiq_enable param );
unsigned char get_vsim_sys_clk_req4_hiq();
unsigned char set_vsim_sys_clk_req5_hiq( enum vsim_sys_clk_req5_hiq_enable param );
unsigned char get_vsim_sys_clk_req5_hiq();
unsigned char set_vsim_sys_clk_req6_hiq( enum vsim_sys_clk_req6_hiq_enable param );
unsigned char get_vsim_sys_clk_req6_hiq();
unsigned char set_vsim_sys_clk_req7_hiq( enum vsim_sys_clk_req7_hiq_enable param );
unsigned char get_vsim_sys_clk_req7_hiq();
unsigned char set_vsim_sys_clk_req8_hiq( enum vsim_sys_clk_req8_hiq_enable param );
unsigned char get_vsim_sys_clk_req8_hiq();
unsigned char set_sys_clock_buf2_req6( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req6();
unsigned char set_sys_clock_buf3_req6( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req6();
unsigned char set_sys_clock_buf4_req6( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req6();
unsigned char set_sys_clock_buf2_req7( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req7();
unsigned char set_sys_clock_buf3_req7( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req7();
unsigned char set_sys_clock_buf4_req7( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req7();
unsigned char set_ulta_low_power_pad( enum ulta_low_power_pad param );
unsigned char get_ulta_low_power_pad();
unsigned char set_ulta_low_power_clock_strength( enum strength param );
unsigned char get_ulta_low_power_clock_strength();
unsigned char set_main_watchdog( enum main_watchdog_enable param );
unsigned char get_main_watchdog();
unsigned char set_main_watchdog_kick( enum kick_main_watchdog_enable param );
unsigned char get_main_watchdog_kick();
unsigned char set_turn_on_valid_on_expire( enum main_watch_dog_write_enable param );
unsigned char get_turn_on_valid_on_expire();
unsigned char set_usb_clock( enum usb_clock_enable param );
unsigned char get_usb_clock();
unsigned char set_vaux5_selection( unsigned char param );
unsigned char get_vaux5_selection();
unsigned char set_vaux5_lowper( enum vaux5_lowper_set param );
unsigned char get_vaux5_lowper();
unsigned char set_vaux5( enum vaux5_enable_set param );
unsigned char get_vaux5();
unsigned char set_vaux5_discharge( enum vaux5_discharge_set param );
unsigned char get_vaux5_discharge();
unsigned char set_vaux5_dis_sfst( enum vaux5_dis_sfst_set param );
unsigned char get_vaux5_dis_sfst();
unsigned char set_vaux5_dis_pull_down( enum vaux5_dis_pull_down param );
unsigned char get_vaux5_dis_pull_down();
unsigned char set_software_off( enum software_off param );
unsigned char get_software_off();
unsigned char set_software_reset( enum software_reset param );
unsigned char get_software_reset();
unsigned char set_thermal_software_off( enum thermal_software_off param );
unsigned char get_thermal_software_off();
unsigned char set_smps_clock_selection( enum smps_clock_selection param );
unsigned char get_smps_clock_selection();
unsigned char set_force_smps_clock( enum force_smps_clock_on param );
unsigned char get_force_smps_clock();
unsigned char set_vaux6_selection( unsigned char param );
unsigned char get_vaux6_selection();
unsigned char set_vaux6_lowper( enum vaux6_lowper_set param );
unsigned char get_vaux6_lowper();
unsigned char set_vaux6( enum vaux6_enable_set param );
unsigned char get_vaux6();
unsigned char set_vaux6_discharge( enum vaux6_discharge_set param );
unsigned char get_vaux6_discharge();
unsigned char set_vaux6_dis_sfst( enum vaux6_dis_sfst_set param );
unsigned char get_vaux6_dis_sfst();
unsigned char set_vaux6_dis_pull_down( enum vaux6_dis_pull_down param );
unsigned char get_vaux6_dis_pull_down();


#ifdef __cplusplus
}
#endif

#endif
