/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Miscellanous/Linux/ab8500_Miscellanous.h
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

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_MISCELLANOUS_LINUX_AB8500_MISCELLANOUS_H
#define C__LISP_PRODUCTION_AB8500_AB8500_MISCELLANOUS_LINUX_AB8500_MISCELLANOUS_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_Miscellanous_types.h"

unsigned char set_ape_i2c_data_pullup( enum ape_i2c_data_pullup_enable param );
unsigned char get_ape_i2c_data_pullup();
unsigned char set_ape_i2c_clock_pullup( enum ape_i2c_clock_pullup_enable param );
unsigned char get_ape_i2c_clock_pullup();
unsigned char set_modem_i2c_data_pullup( enum modem_i2c_data_pullup_enable param );
unsigned char get_modem_i2c_data_pullup();
unsigned char set_modem_i2c_clock_pullup( enum modem_i2c_pullup_enable param );
unsigned char get_modem_i2c_clock_pullup();
unsigned char get_full_mask_set_revision();
unsigned char get_metal_fix_revision();


#ifdef __cplusplus
}
#endif

#endif
