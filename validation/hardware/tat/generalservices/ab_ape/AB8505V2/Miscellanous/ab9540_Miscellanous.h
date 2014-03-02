/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Miscellanous/Linux/ab9540_Miscellanous.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_MISCELLANOUS_LINUX_AB9540_MISCELLANOUS_H
#define C__LISP_PRODUCTION_AB9540_AB9540_MISCELLANOUS_LINUX_AB9540_MISCELLANOUS_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Miscellanous_types.h"

unsigned char get_metal_fix_revision();
unsigned char get_full_mask_set_revision();
unsigned char set_modem_i2c_clock_pullup( enum modem_i2c_pullup_enable param );
unsigned char get_modem_i2c_clock_pullup();
unsigned char set_modem_i2c_data_pullup( enum modem_i2c_data_pullup_enable param );
unsigned char get_modem_i2c_data_pullup();
unsigned char set_ape_i2c_clock_pullup( enum ape_i2c_clock_pullup_enable param );
unsigned char get_ape_i2c_clock_pullup();
unsigned char set_ape_i2c_data_pullup( enum ape_i2c_data_pullup_enable param );
unsigned char get_ape_i2c_data_pullup();


#ifdef __cplusplus
}
#endif

#endif
