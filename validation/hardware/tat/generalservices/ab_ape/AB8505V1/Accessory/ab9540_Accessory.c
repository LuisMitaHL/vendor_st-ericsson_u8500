/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Accessory/Linux/ab9540_Accessory.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_ACCESSORY_LINUX_AB9540_ACCESSORY_C
#define C__LISP_PRODUCTION_AB9540_AB9540_ACCESSORY_LINUX_AB9540_ACCESSORY_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Accessory.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_acc_detect_db
 *
 * IN   : param, a value to write to the regiter AccDetect1
 * OUT  : 
 *
 * RET  : Return the value of register 0xAccDetect1
 *
 * Notes : From register 0x880, bits 2:0
 *
 **************************************************************************/
unsigned char set_acc_detect_db( enum detection_threshold2 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ACC_DETECT_1_REG);

    /* 
     * Accessory Detection threshold on AccDetect1 ball 
     */ 
  

     value =  old_value & ~ACC_DETECT_DB_MASK ;

    switch(  param ){

           case DETECTION_THRESHOLD_150MV_E:
                value =  value | DETECTION_THRESHOLD_150MV ;
                break;
           case DETECTION_THRESHOLD_250MV_E:
                value =  value | DETECTION_THRESHOLD_250MV ;
                break;
           case DETECTION_THRESHOLD_350MV_E:
                value =  value | DETECTION_THRESHOLD_350MV ;
                break;
           case DETECTION_THRESHOLD_550MV_E:
                value =  value | DETECTION_THRESHOLD_550MV ;
                break;
           case DETECTION_THRESHOLD_750MV_E:
                value =  value | DETECTION_THRESHOLD_750MV ;
                break;
           case DETECTION_THRESHOLD_1050MV_E:
                value =  value | DETECTION_THRESHOLD_1050MV ;
                break;
           case DETECTION_THRESHOLD_1500MV_E:
                value =  value | DETECTION_THRESHOLD_1500MV ;
                break;
    }
  

    I2CWrite(ACC_DETECT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect_db
 *
 * RET  : Return the value of register 0xAccDetect1
 *
 * Notes : From register 0x880, bits 2:0
 *
 **************************************************************************/
unsigned char get_acc_detect_db()
  {
    unsigned char value;


    /* 
     * Accessory Detection threshold on AccDetect1 ball 
     */ 
    value = (I2CRead(ACC_DETECT_1_REG) & ACC_DETECT_DB_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect1_th
 *
 * IN   : param, a value to write to the regiter AccDetect1
 * OUT  : 
 *
 * RET  : Return the value of register 0xAccDetect1
 *
 * Notes : From register 0x880, bits 6:3
 *
 **************************************************************************/
unsigned char set_acc_detect1_th( enum detection_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ACC_DETECT_1_REG);

    /* 
     * Accessory Detection threshold on AccDetect1 ball 
     */ 
  
    value =  old_value & ~ACC_DETECT_1_TH_MASK;


    value |= ( param << 0x3);  
    I2CWrite(ACC_DETECT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect1_th
 *
 * RET  : Return the value of register 0xAccDetect1
 *
 * Notes : From register 0x880, bits 6:3
 *
 **************************************************************************/
unsigned char get_acc_detect1_th()
  {
    unsigned char value;


    /* 
     * Accessory Detection threshold on AccDetect1 ball 
     */ 
    value = (I2CRead(ACC_DETECT_1_REG) & ACC_DETECT_1_TH_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect22_db
 *
 * IN   : param, a value to write to the regiter AccDetect2
 * OUT  : 
 *
 * RET  : Return the value of register 0xAccDetect2
 *
 * Notes : From register 0x881, bits 3:0
 *
 **************************************************************************/
unsigned char set_acc_detect22_db( enum detection_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ACC_DETECT_2_REG);

    /* 
     * Accessory Detection threshold 2 on AccDetect1 ball 
     */ 
  
    value =  old_value & ~ACC_DETECT_22_DB_MASK;


    value |=  param ;  
    I2CWrite(ACC_DETECT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect22_db
 *
 * RET  : Return the value of register 0xAccDetect2
 *
 * Notes : From register 0x881, bits 3:0
 *
 **************************************************************************/
unsigned char get_acc_detect22_db()
  {
    unsigned char value;


    /* 
     * Accessory Detection threshold 2 on AccDetect1 ball 
     */ 
    value = (I2CRead(ACC_DETECT_2_REG) & ACC_DETECT_22_DB_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect21_th
 *
 * IN   : param, a value to write to the regiter AccDetect2
 * OUT  : 
 *
 * RET  : Return the value of register 0xAccDetect2
 *
 * Notes : From register 0x881, bits 7:4
 *
 **************************************************************************/
unsigned char set_acc_detect21_th( enum detection_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ACC_DETECT_2_REG);

    /* 
     * Accessory Detection threshold 2 on AccDetect1 ball 
     */ 
  
    value =  old_value & ~ACC_DETECT_21_TH_MASK;


    value |= ( param << 0x4);  
    I2CWrite(ACC_DETECT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect21_th
 *
 * RET  : Return the value of register 0xAccDetect2
 *
 * Notes : From register 0x881, bits 7:4
 *
 **************************************************************************/
unsigned char get_acc_detect21_th()
  {
    unsigned char value;


    /* 
     * Accessory Detection threshold 2 on AccDetect1 ball 
     */ 
    value = (I2CRead(ACC_DETECT_2_REG) & ACC_DETECT_21_TH_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect1_comparator
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 0:0
 *
 **************************************************************************/
unsigned char set_acc_detect1_comparator( enum detect1_comparator_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ACC_DETECT_3_REG);

    /* 
     * Note: doesn't turn On internal pull-up 
     * 0: disable AccDetect1 comparator 
     * 1: enable AccDetect1 comparator 
     */ 
    switch( param ){
        case DETECT_1_COMPARATOR_ENABLE_E: 
           value = old_value | ACC_DETECT_1_COMPARATOR_MASK; 
           break;
        case DETECT_1_COMPARATOR_DISABLE_E: 
           value = old_value & ~ ACC_DETECT_1_COMPARATOR_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect1_comparator
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 0:0
 *
 **************************************************************************/
unsigned char get_acc_detect1_comparator()
  {
    unsigned char value;


    /* 
     * Note: doesn't turn On internal pull-up 
     * 0: disable AccDetect1 comparator 
     * 1: enable AccDetect1 comparator 
     */ 
    value = (I2CRead(ACC_DETECT_3_REG) & ACC_DETECT_1_COMPARATOR_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect_pullup_supply
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 1:1
 *
 **************************************************************************/
unsigned char set_acc_detect_pullup_supply( enum v21_supply_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ACC_DETECT_3_REG);

    /* 
     * 0: disable AccDetect2 Pull-up 2.1v supply 
     * 1: enable AccDetect2 Pull-up 2.1v supply 
     */ 
    switch( param ){
        case V21_SUPPLY_ENABLE_E: 
           value = old_value | ACC_DETECT_PULLUP_SUPPLY_MASK; 
           break;
        case V21_SUPPLY_DISABLE_E: 
           value = old_value & ~ ACC_DETECT_PULLUP_SUPPLY_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect_pullup_supply
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 1:1
 *
 **************************************************************************/
unsigned char get_acc_detect_pullup_supply()
  {
    unsigned char value;


    /* 
     * 0: disable AccDetect2 Pull-up 2.1v supply 
     * 1: enable AccDetect2 Pull-up 2.1v supply 
     */ 
    value = (I2CRead(ACC_DETECT_3_REG) & ACC_DETECT_PULLUP_SUPPLY_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect_pullup
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 3:3
 *
 **************************************************************************/
unsigned char set_acc_detect_pullup( enum acc_detect_pullup_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ACC_DETECT_3_REG);

    /* 
     * 0: disable AccDetect2 Pull-up 
     * 1: enable AccDetect2 Pull-up 
     */ 
    switch( param ){
        case ACC_DETECT_PULLUP_ENABLE_E: 
           value = old_value | ACC_DETECT_PULLUP_MASK; 
           break;
        case ACC_DETECT_PULLUP_DISABLE_E: 
           value = old_value & ~ ACC_DETECT_PULLUP_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect_pullup
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 3:3
 *
 **************************************************************************/
unsigned char get_acc_detect_pullup()
  {
    unsigned char value;


    /* 
     * 0: disable AccDetect2 Pull-up 
     * 1: enable AccDetect2 Pull-up 
     */ 
    value = (I2CRead(ACC_DETECT_3_REG) & ACC_DETECT_PULLUP_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect21_comparator
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 4:4
 *
 **************************************************************************/
unsigned char set_acc_detect21_comparator( enum comparator21_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ACC_DETECT_3_REG);

    /* 
     * 0: disable AccDetect21 comparator 
     * 1: enable AccDetect21 comparator 
     */ 
    switch( param ){
        case COMPARATOR_21_ENABLE_E: 
           value = old_value | ACC_DETECT_21_COMPARATOR_MASK; 
           break;
        case COMPARATOR_21_DISABLE_E: 
           value = old_value & ~ ACC_DETECT_21_COMPARATOR_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect21_comparator
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 4:4
 *
 **************************************************************************/
unsigned char get_acc_detect21_comparator()
  {
    unsigned char value;


    /* 
     * 0: disable AccDetect21 comparator 
     * 1: enable AccDetect21 comparator 
     */ 
    value = (I2CRead(ACC_DETECT_3_REG) & ACC_DETECT_21_COMPARATOR_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect22_comparator
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 5:5
 *
 **************************************************************************/
unsigned char set_acc_detect22_comparator( enum comparator22_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ACC_DETECT_3_REG);

    /* 
     * 0: disable AccDetect22 comparator 
     * 1: enable AccDetect22 comparator 
     */ 
    switch( param ){
        case COMPARATOR_22_ENABLE_E: 
           value = old_value | ACC_DETECT_22_COMPARATOR_MASK; 
           break;
        case COMPARATOR_22_DISABLE_E: 
           value = old_value & ~ ACC_DETECT_22_COMPARATOR_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect22_comparator
 *
 * RET  : Return the value of register 0xAccDetect3
 *
 * Notes : From register 0x882, bits 5:5
 *
 **************************************************************************/
unsigned char get_acc_detect22_comparator()
  {
    unsigned char value;


    /* 
     * 0: disable AccDetect22 comparator 
     * 1: enable AccDetect22 comparator 
     */ 
    value = (I2CRead(ACC_DETECT_3_REG) & ACC_DETECT_22_COMPARATOR_MASK) >> 5;
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
