/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Gauge/Linux/ab9540_Gauge.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_GAUGE_LINUX_AB9540_GAUGE_H
#define C__LISP_PRODUCTION_AB9540_AB9540_GAUGE_LINUX_AB9540_GAUGE_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Gauge_types.h"

unsigned char set_coulomb_counter_read( enum read_enable param );
unsigned char get_coulomb_counter_read();
unsigned char set_coulomb_counter_reset_accumlator( enum reset_accumlator_enable param );
unsigned char get_coulomb_counter_reset_accumlator();
unsigned char set_coulomb_counter_calibration_mode( enum calibration_enable param );
unsigned char get_coulomb_counter_calibration_mode();
unsigned char set_coulomb_counter_offset_computation( enum computation_enable param );
unsigned char get_coulomb_counter_offset_computation();
unsigned char set_coulomb_counter_number_of_convertion( enum number_convertion param );
unsigned char get_coulomb_counter_number_of_convertion();
unsigned char set_coulomb_counter_mux_offset( enum select param );
unsigned char get_coulomb_counter_mux_offset();
unsigned char set_comlomb_reset_bit_data( enum bit_data_reset param );
unsigned char get_comlomb_reset_bit_data();
unsigned char set_comlomb_data_transfert( enum data_transfert param );
unsigned char get_comlomb_data_transfert();


#ifdef __cplusplus
}
#endif

#endif
