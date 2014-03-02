/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_USB/Linux/ab8500_USB.h
 * 
 *
 * Generated on the 23/09/2009 15:21 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_USB_LINUX_AB8500_USB_H
#define C__LISP_PRODUCTION_AB8500_AB8500_USB_LINUX_AB8500_USB_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_USB_types.h"

unsigned char get_charger_type();
unsigned char get_host_charger_detected();
unsigned char get_dminus_cmp_out();
unsigned char get_dplus_cmp_out();
unsigned char set_dm_pull_up( enum dm_pull_up_enable param );
unsigned char get_dm_pull_up();
unsigned char set_dp_pull_down( enum dm_pull param );
unsigned char get_dp_pull_down();
unsigned char set_dm_pull_down( enum dm_pull_down_enable param );
unsigned char get_dm_pull_down();
unsigned char set_dp_pull_up( enum dp_pull_up_enable param );
unsigned char get_dp_pull_up();
unsigned char set_usb_switch_ctrl( enum usb_switch param );
unsigned char get_usb_switch_ctrl();
unsigned char set_charger_pull_up( enum charger_pull_up_close param );
unsigned char get_charger_pull_up();
unsigned char set_dm_pull_down_disable( enum dm_pull param );
unsigned char get_dm_pull_down_disable();
unsigned char set_charger_mux_ctrl( enum charger_mux_ctrl param );
unsigned char get_charger_mux_ctrl();
unsigned char set_usb_charger_detection( enum usb_charger_detection_enable param );
unsigned char get_usb_charger_detection();
unsigned char set_data_connect_detection_enable( enum data_connect_detection_enable param );
unsigned char get_data_connect_detection_enable();
unsigned char set_data_connect_source_enable( enum data_connect_source_enable param );
unsigned char get_data_connect_source_enable();
unsigned char set_sdm_snk_switch( enum sdm_snk_switch_opened param );
unsigned char get_sdm_snk_switch();
unsigned char set_sdp_snk_switch( enum sdp_snk_switch_opened param );
unsigned char get_sdp_snk_switch();
unsigned char set_sdm_src_switch( enum sdm_src_switch_opened param );
unsigned char get_sdm_src_switch();
unsigned char set_sdp_src_switch( enum sdp_src_switch_opened param );
unsigned char get_sdp_src_switch();
unsigned char set_vdat_source( enum vdat_source_enable param );
unsigned char get_vdat_source();
unsigned char set_idat_sink( enum idat_sink_enable param );
unsigned char get_idat_sink();
unsigned char set_vbus_comp( enum compatrator_enable param );
unsigned char get_vbus_comp();
unsigned char set_vbus_valid( enum valid_compatrator_enable param );
unsigned char get_vbus_valid();
unsigned char set_id_host_detection( enum detection param );
unsigned char get_id_host_detection();
unsigned char set_device_detection( enum detection param );
unsigned char get_device_detection();
unsigned char set_id_det330k( enum id_resitance param );
unsigned char get_id_det330k();
unsigned char set_id_det220k( enum id_resitance param );
unsigned char get_id_det220k();
unsigned char set_id_det140k( enum id_resitance param );
unsigned char get_id_det140k();
unsigned char set_id_det80k( enum id_resitance param );
unsigned char get_id_det80k();
unsigned char set_id_wakeup( enum resitance param );
unsigned char get_id_wakeup();
unsigned char get_high_speed_mode();
unsigned char get_chirp_mode();
unsigned char get_suspend_mode();
unsigned char set_usb_device_mode_enable( enum usb_device_mode_enable param );
unsigned char get_usb_device_mode_enable();
unsigned char set_usb_host_mode_enable( enum usb_host_mode_enable param );
unsigned char get_usb_host_mode_enable();


#ifdef __cplusplus
}
#endif

#endif
