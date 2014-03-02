/****************************************************************************
 * File : c:/Lisp/Production/ab8500_V2/ab8500_Accessory/Linux/ab8500_Accessory.h
 * 
 *
 * Generated on the 20/04/2011 10:26 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : UM0836 User manual, AB8500_V2 Mixed signal multimedia and power management Rev 1
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_V2_AB8500_ACCESSORY_LINUX_AB8500_ACCESSORY_H
#define C__LISP_PRODUCTION_AB8500_V2_AB8500_ACCESSORY_LINUX_AB8500_ACCESSORY_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_Accessory_types.h"

unsigned char set_acc_detect1_th( enum detection_threshold param );
unsigned char get_acc_detect1_th();
unsigned char set_acc_detect_db( enum detection_threshold2 param );
unsigned char get_acc_detect_db();
unsigned char set_acc_detect21_th( enum detection_threshold param );
unsigned char get_acc_detect21_th();
unsigned char set_acc_detect22_db( enum detection_threshold param );
unsigned char get_acc_detect22_db();
unsigned char set_acc_detect22_comparator( enum comparator22_enable param );
unsigned char get_acc_detect22_comparator();
unsigned char set_acc_detect21_comparator( enum comparator21_enable param );
unsigned char get_acc_detect21_comparator();
unsigned char set_acc_detect_pullup( enum acc_detect_pullup_enable param );
unsigned char get_acc_detect_pullup();
unsigned char set_acc_detect_pullup_supply( enum v21_supply_enable param );
unsigned char get_acc_detect_pullup_supply();
unsigned char set_acc_detect1_comparator( enum detect1_comparator_enable param );
unsigned char get_acc_detect1_comparator();


#ifdef __cplusplus
}
#endif

#endif
