/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Accessory/Linux/ab9540_Accessory.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_ACCESSORY_LINUX_AB9540_ACCESSORY_H
#define C__LISP_PRODUCTION_AB9540_AB9540_ACCESSORY_LINUX_AB9540_ACCESSORY_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Accessory_types.h"

unsigned char set_acc_detect1_comparator( enum detect1_comparator_enable param );
unsigned char get_acc_detect1_comparator();
unsigned char set_acc_detect_pullup_supply( enum v21_supply_enable param );
unsigned char get_acc_detect_pullup_supply();
unsigned char set_acc_detect_pullup( enum acc_detect_pullup_enable param );
unsigned char get_acc_detect_pullup();
unsigned char set_acc_detect21_comparator( enum comparator21_enable param );
unsigned char get_acc_detect21_comparator();
unsigned char set_acc_detect22_comparator( enum comparator22_enable param );
unsigned char get_acc_detect22_comparator();
unsigned char set_acc_detect22_db( enum detection_threshold param );
unsigned char get_acc_detect22_db();
unsigned char set_acc_detect21_th( enum detection_threshold param );
unsigned char get_acc_detect21_th();
unsigned char set_acc_detect_db( enum detection_threshold2 param );
unsigned char get_acc_detect_db();
unsigned char set_acc_detect1_th( enum detection_threshold param );
unsigned char get_acc_detect1_th();


#ifdef __cplusplus
}
#endif

#endif
