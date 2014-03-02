/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Control/Linux/ab8500_Control.h
 * 
 *
 * Generated on the 23/06/2010 14:50 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2010
 *
 *  Reference document : AB8500-Registers-cut1_1-cut2_0-rev1.xls
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_CONTROL_LINUX_AB8500_CONTROL_H
#define C__LISP_PRODUCTION_AB8500_AB8500_CONTROL_LINUX_AB8500_CONTROL_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_Control_types.h"

unsigned char set_usb_id_detect( enum usb_detect_on param );
unsigned char get_usb_id_detect();
unsigned char set_vbus_dectect( enum vbus_detect_on param );
unsigned char get_vbus_dectect();
unsigned char set_main_ch_detect( enum main_ch_detect_on param );
unsigned char get_main_ch_detect();
unsigned char set_rtc_alarm_detect( enum rtc_alarm_detect_on param );
unsigned char get_rtc_alarm_detect();
unsigned char set_power_on_key2_detect( enum key_detected param );
unsigned char get_power_on_key2_detect();
unsigned char set_power_on_key1_detect( enum key_detected param );
unsigned char get_power_on_key1_detect();
unsigned char set_battery_detect( enum battery_detect_on param );
unsigned char get_battery_detect();
unsigned char get_software_reset_flag();
unsigned char get_reset4500();
unsigned char set_power_on_key1_press_time( unsigned char param );
unsigned char get_power_on_key1_press_time();
unsigned char set_sys_clk_req( unsigned char param );
unsigned char get_sys_clk_req();
unsigned char set_thermal_software_off( enum thermal_software_off param );
unsigned char get_thermal_software_off();
unsigned char set_software_reset( enum software_reset param );
unsigned char get_software_reset();
unsigned char set_software_off( enum software_off param );
unsigned char get_software_off();
unsigned char set_reset_vsmps1_valid( enum reset_valid param );
unsigned char get_reset_vsmps1_valid();
unsigned char set_reset_vext_supply3_valid( enum reset_valid param );
unsigned char get_reset_vext_supply3_valid();
unsigned char set_reset_vext_supply2_valid( enum reset_valid param );
unsigned char get_reset_vext_supply2_valid();
unsigned char set_reset_vext_supply_valid( enum reset_valid param );
unsigned char get_reset_vext_supply_valid();
unsigned char set_reset_vmod_valid( enum reset_valid param );
unsigned char get_reset_vmod_valid();
unsigned char set_reset_vaux3_valid( enum reset_valid param );
unsigned char get_reset_vaux3_valid();
unsigned char set_reset_vaux2_valid( enum reset_valid param );
unsigned char get_reset_vaux2_valid();
unsigned char set_reset_vaux1_valid( enum reset_valid param );
unsigned char get_reset_vaux1_valid();
unsigned char set_thermal_shutdown( enum thermal_shutdown_enable param );
unsigned char get_thermal_shutdown();
unsigned char set_video_denc_reset( enum video_denc_reset param );
unsigned char get_video_denc_reset();
unsigned char set_audio_reset( enum audio_reset param );
unsigned char get_audio_reset();
unsigned char set_clock32( enum clock32_output_buffer_enable param );
unsigned char get_clock32();
unsigned char set_turn_on_valid_on_expire( enum main_watch_dog_write_enable param );
unsigned char get_turn_on_valid_on_expire();
unsigned char set_main_watchdog_kick( enum kick_main_watchdog_enable param );
unsigned char get_main_watchdog_kick();
unsigned char set_main_watchdog_enable( enum main_watchdog_enable param );
unsigned char get_main_watchdog_enable();
unsigned char set_main_watchdog_timer( enum main_watchdog_timer param );
unsigned char get_main_watchdog_timer();
unsigned char set_low_bat_threshold( enum low_bat_threshold param );
unsigned char get_low_bat_threshold();
unsigned char set_low_battery_enable( enum low_battery_enable param );
unsigned char get_low_battery_enable();
unsigned char set_battery_ok2_threshold( enum battery_ok2_threshold param );
unsigned char get_battery_ok2_threshold();
unsigned char set_battery_ok1_threshold( enum battery_ok1_threshold param );
unsigned char get_battery_ok1_threshold();
unsigned char set_system_clock_timer_adjustement( enum timer_adjustement param );
unsigned char get_system_clock_timer_adjustement();
unsigned char set_system_clock_timer( enum clock_timer param );
unsigned char get_system_clock_timer();
unsigned char set_force_smps_clock( enum force_smps_clock_on param );
unsigned char get_force_smps_clock();
unsigned char set_smps_clock_selection( enum smps_clock_selection param );
unsigned char get_smps_clock_selection();
unsigned char set_vape_clock_sel( enum smps_ckock_phase param );
unsigned char get_vape_clock_sel();
unsigned char set_varm_clock_sel( enum smps_ckock_phase param );
unsigned char get_varm_clock_sel();
unsigned char set_vsmps1_clock_sel( enum smps_ckock_phase param );
unsigned char get_vsmps1_clock_sel();
unsigned char set_vsmod_clock_sel( enum smps_ckock_phase param );
unsigned char get_vsmod_clock_sel();
unsigned char set_vsmps2_clock_sel( enum smps_ckock_phase param );
unsigned char get_vsmps2_clock_sel();
unsigned char set_vsmps3_clock_sel( enum smps_ckock_phase param );
unsigned char get_vsmps3_clock_sel();
unsigned char set_clock27_ouput_buffer_d_p( enum ouput_buffer_p_d_enable param );
unsigned char get_clock27_ouput_buffer_d_p();
unsigned char set_clock27_ouput_buffer( enum ouput_buffer_enable param );
unsigned char get_clock27_ouput_buffer();
unsigned char set_ulta_low_power_clock_strength( enum strength param );
unsigned char get_ulta_low_power_clock_strength();
unsigned char set_tvout_clock_inverted( enum tvout_clock_inverted param );
unsigned char get_tvout_clock_inverted();
unsigned char set_tvout_clock_input( enum tvout_clock_input_enable param );
unsigned char get_tvout_clock_input();
unsigned char set_clock27_strength( enum strength param );
unsigned char get_clock27_strength();
unsigned char set_ulta_low_power_pad( enum ulta_low_power_padonnf param );
unsigned char get_ulta_low_power_pad();
unsigned char set_sys_clk_buf4_req( enum activate_sys_clock_buffer4 param );
unsigned char get_sys_clk_buf4_req();
unsigned char set_sys_clk_buf3_req( enum activate_sys_clock_buffer3 param );
unsigned char get_sys_clk_buf3_req();
unsigned char set_sys_clk_buf2_req( enum activate_sys_clock_buffer2 param );
unsigned char get_sys_clk_buf2_req();
unsigned char set_audio_clock( enum audio_clock_enable param );
unsigned char get_audio_clock();
unsigned char set_system_clock_request( enum system_clock_request_enable param );
unsigned char get_system_clock_request();
unsigned char set_ulp_request( enum ulp_request_enable param );
unsigned char get_ulp_request();
unsigned char set_sys_ultra_low_power_selection( enum sys_ultra_low_power_selection param );
unsigned char get_sys_ultra_low_power_selection();
unsigned char set_usb_clock_enable( enum usb_clock_enable param );
unsigned char get_usb_clock_enable();
unsigned char set_t_vout_clock_enable( enum t_vout_clk_enable param );
unsigned char get_t_vout_clock_enable();
unsigned char set_tv_out_pll_enable( enum tv_out_pll_enable param );
unsigned char get_tv_out_pll_enable();
unsigned char set_ultra_low_power_request1_valid( enum ultra_low_power_request1_valid param );
unsigned char get_ultra_low_power_request1_valid();
unsigned char set_sys_clock_request1_valid( enum sys_clock_request1_valid param );
unsigned char get_sys_clock_request1_valid();
unsigned char set_interrupt8500_open_drain( enum ball_configuration param );
unsigned char get_interrupt8500_open_drain();
unsigned char set_ext_sup3_l_pn_clk_sel( enum configuration3 param );
unsigned char get_ext_sup3_l_pn_clk_sel();
unsigned char set_ext_sup12_l_pn_clk_sel( enum configuration12 param );
unsigned char get_ext_sup12_l_pn_clk_sel();
unsigned char set_sys_clock_buf4_req1( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req1();
unsigned char set_sys_clock_buf3_req1( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req1();
unsigned char set_sys_clock_buf2_req1( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req1();
unsigned char set_sys_clock_buf4_req2( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req2();
unsigned char set_sys_clock_buf3_req2( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req2();
unsigned char set_sys_clock_buf2_req2( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req2();
unsigned char set_sys_clock_buf4_req3( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req3();
unsigned char set_sys_clock_buf3_req3( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req3();
unsigned char set_sys_clock_buf2_req3( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req3();
unsigned char set_sys_clock_buf4_req4( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req4();
unsigned char set_sys_clock_buf3_req4( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req4();
unsigned char set_sys_clock_buf2_req4( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req4();
unsigned char set_sys_clock_buf4_req5( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req5();
unsigned char set_sys_clock_buf3_req5( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req5();
unsigned char set_sys_clock_buf2_req5( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req5();
unsigned char set_sys_clock_buf4_req6( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req6();
unsigned char set_sys_clock_buf3_req6( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req6();
unsigned char set_sys_clock_buf2_req6( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req6();
unsigned char set_sys_clock_buf4_req7( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req7();
unsigned char set_sys_clock_buf3_req7( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req7();
unsigned char set_sys_clock_buf2_req7( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req7();
unsigned char set_sys_clock_buf4_req8( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf4_req8();
unsigned char set_sys_clock_buf3_req8( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf3_req8();
unsigned char set_sys_clock_buf2_req8( enum sys_clock_resquest param );
unsigned char get_sys_clock_buf2_req8();
unsigned char set_dithering_delay( enum dithering_delay param );
unsigned char get_dithering_delay();
unsigned char set_vape_dither( enum vape_dither_enable param );
unsigned char get_vape_dither();
unsigned char set_vmod_dither( enum vmod_dither_enable param );
unsigned char get_vmod_dither();
unsigned char set_vsmps2_dither( enum vsmps2_dither_enable param );
unsigned char get_vsmps2_dither();
unsigned char set_vsmps1_dither( enum vsmps1_dither_enable param );
unsigned char get_vsmps1_dither();
unsigned char set_vsmps3_dither( enum vsmps3_dither_enable param );
unsigned char get_vsmps3_dither();
unsigned char set_varm_dither( enum varm_dither_enable param );
unsigned char get_varm_dither();
unsigned char set_swat_bit5_set( enum swat_bit5_set param );
unsigned char get_swat_bit5_set();
unsigned char set_rf_off_timer( enum off_timer param );
unsigned char get_rf_off_timer();
unsigned char set_swat_enable( enum swat_enable param );
unsigned char get_swat_enable();
unsigned char set_levi_or_yllas( enum levi_or_yllas param );
unsigned char get_levi_or_yllas();
unsigned char set_sys_clk_req8_hiq_enable( enum sys_clk_req8_hiq_enable param );
unsigned char get_sys_clk_req8_hiq_enable();
unsigned char set_sys_clk_req7_hiq_enable( enum sys_clk_req7_hiq_enable param );
unsigned char get_sys_clk_req7_hiq_enable();
unsigned char set_sys_clk_req6_hiq_enable( enum sys_clk_req6_hiq_enable param );
unsigned char get_sys_clk_req6_hiq_enable();
unsigned char set_sys_clk_req5_hiq_enable( enum sys_clk_req5_hiq_enable param );
unsigned char get_sys_clk_req5_hiq_enable();
unsigned char set_sys_clk_req4_hiq_enable( enum sys_clk_req4_hiq_enable param );
unsigned char get_sys_clk_req4_hiq_enable();
unsigned char set_sys_clk_req3_hiq_enable( enum sys_clk_req3_hiq_enable param );
unsigned char get_sys_clk_req3_hiq_enable();
unsigned char set_sys_clk_req2_hiq_enable( enum sys_clk_req2_hiq_enable param );
unsigned char get_sys_clk_req2_hiq_enable();
unsigned char set_sys_clk_req1_hiq_enable( enum sys_clk_req1_hiq_enable param );
unsigned char get_sys_clk_req1_hiq_enable();
unsigned char set_vsim_sys_clk_req8_hiq_enable( enum vsim_sys_clk_req8_hiq_enable param );
unsigned char get_vsim_sys_clk_req8_hiq_enable();
unsigned char set_vsim_sys_clk_req7_hiq_enable( enum vsim_sys_clk_req7_hiq_enable param );
unsigned char get_vsim_sys_clk_req7_hiq_enable();
unsigned char set_vsim_sys_clk_req6_hiq_enable( enum vsim_sys_clk_req6_hiq_enable param );
unsigned char get_vsim_sys_clk_req6_hiq_enable();
unsigned char set_vsim_sys_clk_req5_hiq_enable( enum vsim_sys_clk_req5_hiq_enable param );
unsigned char get_vsim_sys_clk_req5_hiq_enable();
unsigned char set_vsim_sys_clk_req4_hiq_enable( enum vsim_sys_clk_req4_hiq_enable param );
unsigned char get_vsim_sys_clk_req4_hiq_enable();
unsigned char set_vsim_sys_clk_req3_hiq_enable( enum vsim_sys_clk_req3_hiq_enable param );
unsigned char get_vsim_sys_clk_req3_hiq_enable();
unsigned char set_vsim_sys_clk_req2_hiq_enable( enum vsim_sys_clk_req2_hiq_enable param );
unsigned char get_vsim_sys_clk_req2_hiq_enable();
unsigned char set_vsim_sys_clk_req1_hiq_enable( enum vsim_sys_clk_req1_hiq_enable param );
unsigned char get_vsim_sys_clk_req1_hiq_enable();


#ifdef __cplusplus
}
#endif

#endif
