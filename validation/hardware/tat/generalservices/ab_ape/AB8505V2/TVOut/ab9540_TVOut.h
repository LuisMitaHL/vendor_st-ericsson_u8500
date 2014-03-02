/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_TVOut/Linux/ab9540_TVOut.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_TVOUT_LINUX_AB9540_TVOUT_H
#define C__LISP_PRODUCTION_AB9540_AB9540_TVOUT_LINUX_AB9540_TVOUT_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_TVOut_types.h"

unsigned char set_max_dyn( enum max_dyn_enable param );
unsigned char get_max_dyn();
unsigned char set_free_jump( enum free_jump_enable param );
unsigned char get_free_jump();
unsigned char set_dec_ninc( enum dec_ninc_enable param );
unsigned char get_dec_ninc();
unsigned char set_jump( enum jump_enable param );
unsigned char get_jump();
unsigned char set_soft_reset( enum soft_reset param );
unsigned char get_soft_reset();
unsigned char set_vps_encoding( enum vps_enable param );
unsigned char get_vps_encoding();
unsigned char set_tvout_synchro_mode( enum tvout_synchro_mode param );
unsigned char get_tvout_synchro_mode();
unsigned char set_tvout_standard( enum tvout_standard param );
unsigned char get_tvout_standard();
unsigned char set_blanking_all( enum blanking_all_enable param );
unsigned char get_blanking_all();
unsigned char set_val422_mux( enum val422_mux_set param );
unsigned char get_val422_mux();
unsigned char set_phase_reset_mode( enum phase_reset_mode param );
unsigned char get_phase_reset_mode();
unsigned char set_tvout_dac_ctrl( enum tvout_dac_enable param );
unsigned char get_tvout_dac_ctrl();
unsigned char set_plug_tv_detect( enum plug_tv_detect_enable param );
unsigned char get_plug_tv_detect();
unsigned char set_plug_tv_time( enum plug_tv_time param );
unsigned char get_plug_tv_time();
unsigned char set_tv_load_r_c( enum tv_load_r_c_set param );
unsigned char get_tv_load_r_c();
unsigned char set_tv_double_data_rate( enum double_data_rate param );
unsigned char get_tv_double_data_rate();
unsigned char set_tv_dual_data_mode( enum clock_mode param );
unsigned char get_tv_dual_data_mode();


#ifdef __cplusplus
}
#endif

#endif
