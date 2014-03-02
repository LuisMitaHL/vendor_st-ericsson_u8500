/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_TVOut/Linux/ab8500_TVOut.h
 * 
 *
 * Generated on the 04/04/2011 14:09 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : STw4500 Registers Specification Rev 2.61 5 January 2011
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_TVOUT_LINUX_AB8500_TVOUT_H
#define C__LISP_PRODUCTION_AB8500_AB8500_TVOUT_LINUX_AB8500_TVOUT_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_TVOut_types.h"

unsigned char set_tvout_standard();
unsigned char get_tvout_standard();
unsigned char set_tvout_synchro_mode();
unsigned char get_tvout_synchro_mode();
unsigned char set_soft_reset( enum soft_reset param );
unsigned char get_soft_reset();
unsigned char set_jump_enable( enum jump_enable param );
unsigned char get_jump_enable();
unsigned char set_dec_ninc_enable( enum dec_ninc_enable param );
unsigned char get_dec_ninc_enable();
unsigned char set_free_jump_enable( enum free_jump_enable param );
unsigned char get_free_jump_enable();
unsigned char set_cfc( enum cfc param );
unsigned char get_cfc();
unsigned char set_max_dyn_enable( enum max_dyn_enable param );
unsigned char get_max_dyn_enable();
unsigned char set_phase_reset_mode( enum phase_reset_mode param );
unsigned char get_phase_reset_mode();
unsigned char set_val422_mux( enum val422_mux_set param );
unsigned char get_val422_mux();
unsigned char set_blanking_all_enable( enum blanking_all_enable param );
unsigned char get_blanking_all_enable();
unsigned char set_tv_load_r_c( enum tv_load_r_c_set param );
unsigned char get_tv_load_r_c();
unsigned char set_plug_tv_time( enum plug_tv_time param );
unsigned char get_plug_tv_time();
unsigned char set_plug_tv_detect_enable( enum plug_tv_detect_enable param );
unsigned char get_plug_tv_detect_enable();
unsigned char set_tvout_dac_ctrl( enum tvout_dac_enable param );
unsigned char get_tvout_dac_ctrl();
unsigned char set_tv_dual_data_mode( enum clock_mode param );
unsigned char get_tv_dual_data_mode();
unsigned char set_tv_double_data_rate( enum double_data_rate param );
unsigned char get_tv_double_data_rate();


#ifdef __cplusplus
}
#endif

#endif
