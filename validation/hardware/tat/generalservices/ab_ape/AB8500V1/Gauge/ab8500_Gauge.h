/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Gauge/Linux/ab8500_Gauge.h
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

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_GAUGE_LINUX_AB8500_GAUGE_H
#define C__LISP_PRODUCTION_AB8500_AB8500_GAUGE_LINUX_AB8500_GAUGE_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_Gauge_types.h"

unsigned char set_coulomb_counter_mux_offset( enum select param );
unsigned char get_coulomb_counter_mux_offset();
unsigned char set_coulomb_counter_number_of_convertion( enum number_convertion param );
unsigned char get_coulomb_counter_number_of_convertion();
unsigned char set_coulomb_counter_offset_computation( enum computation_enable param );
unsigned char get_coulomb_counter_offset_computation();
unsigned char set_coulomb_counter_calibration_mode( enum calibration_enable param );
unsigned char get_coulomb_counter_calibration_mode();
unsigned char set_coulomb_counter_reset_accumlator( enum reset_accumlator_enable param );
unsigned char get_coulomb_counter_reset_accumlator();
unsigned char set_coulomb_counter_read_enable( enum read_enable param );
unsigned char get_coulomb_counter_read_enable();
unsigned char set_coulomb_counter_low( unsigned char param );
unsigned char get_coulomb_counter_low();
unsigned char set_coulomb_counter_mid( unsigned char param );
unsigned char get_coulomb_counter_mid();
unsigned char set_coulomb_sample_counter_average_offset( unsigned char param );
unsigned char get_coulomb_sample_counter_average_offset();
unsigned char set_coulomb_sample_counter_offset( unsigned char param );
unsigned char get_coulomb_sample_counter_offset();
unsigned char set_coulomb_sample_number_conversion( unsigned char param );
unsigned char get_coulomb_sample_number_conversion();
unsigned char set_comlomb_data_transfert( enum data_transfert param );
unsigned char get_comlomb_data_transfert();
unsigned char set_comlomb_reset_bit_data( enum bit_data_reset param );
unsigned char get_comlomb_reset_bit_data();
unsigned char set_coulomb_sample_number_conversion_low( unsigned char param );
unsigned char get_coulomb_sample_number_conversion_low();
unsigned char set_coulomb_sample_number_conversion_mid( unsigned char param );
unsigned char get_coulomb_sample_number_conversion_mid();
unsigned char set_coulomb_sample_number_conversion_high( unsigned char param );
unsigned char get_coulomb_sample_number_conversion_high();


#ifdef __cplusplus
}
#endif

#endif
