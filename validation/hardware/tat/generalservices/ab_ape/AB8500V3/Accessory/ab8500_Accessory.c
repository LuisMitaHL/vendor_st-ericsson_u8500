/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Accessory/Linux/ab8500_Accessory.c
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
#include "ab8500_Accessory.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_acc_detect1_th
 *
 * IN   : param, a value to write to the regiter AccDetect1
 * OUT  : 
 *
 * RET  : Return the value of register AccDetect1
 *
 * Notes : From register 0x0880, bits 6:3
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
  
    value =  old_value & ~DETECTION_THRESHOLD_PARAM_MASK;


     value |= ( param << 0x3);  
    I2CWrite(ACC_DETECT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect1_th
 *
 * RET  : Return the value of register AccDetect1
 *
 * Notes : From register 0x0880, bits 6:3
 *
 **************************************************************************/
unsigned char get_acc_detect1_th()
  {
    unsigned char value;


    /* 
     * Accessory Detection threshold on AccDetect1 ball 
     */ 
    value = (I2CRead(ACC_DETECT_1_REG) & DETECTION_THRESHOLD_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect_db
 *
 * IN   : param, a value to write to the regiter AccDetect1
 * OUT  : 
 *
 * RET  : Return the value of register AccDetect1
 *
 * Notes : From register 0x0880, bits 2:0
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
  

     value =  old_value & ~ACC_DETECT_DB_PARAM_MASK ;

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
 * RET  : Return the value of register AccDetect1
 *
 * Notes : From register 0x0880, bits 2:0
 *
 **************************************************************************/
unsigned char get_acc_detect_db()
  {
    unsigned char value;


    /* 
     * Accessory Detection threshold on AccDetect1 ball 
     */ 
    value = (I2CRead(ACC_DETECT_1_REG) & DETECTION_THRESHOLD_2_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect21_th
 *
 * IN   : param, a value to write to the regiter AccDetect2
 * OUT  : 
 *
 * RET  : Return the value of register AccDetect2
 *
 * Notes : From register 0x0881, bits 7:4
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
  
    value =  old_value & ~ACC_DETECT_21_TH_PARAM_MASK;


     value |= ( param << 0x4);  
    I2CWrite(ACC_DETECT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect21_th
 *
 * RET  : Return the value of register AccDetect2
 *
 * Notes : From register 0x0881, bits 7:4
 *
 **************************************************************************/
unsigned char get_acc_detect21_th()
  {
    unsigned char value;


    /* 
     * Accessory Detection threshold 2 on AccDetect1 ball 
     */ 
    value = (I2CRead(ACC_DETECT_2_REG) & ACC_DETECT_21_TH_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect22_db
 *
 * IN   : param, a value to write to the regiter AccDetect2
 * OUT  : 
 *
 * RET  : Return the value of register AccDetect2
 *
 * Notes : From register 0x0881, bits 3:0
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
  
    value =  old_value & ~ACC_DETECT_22_DB_PARAM_MASK;


     value |=  param ;  
    I2CWrite(ACC_DETECT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect22_db
 *
 * RET  : Return the value of register AccDetect2
 *
 * Notes : From register 0x0881, bits 3:0
 *
 **************************************************************************/
unsigned char get_acc_detect22_db()
  {
    unsigned char value;


    /* 
     * Accessory Detection threshold 2 on AccDetect1 ball 
     */ 
    value = (I2CRead(ACC_DETECT_2_REG) & ACC_DETECT_22_DB_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect22_comparator
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 5:5
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
           value = old_value | COMPARATOR_22_ENABLE_PARAM_MASK; 
           break;
        case COMPARATOR_22_DISABLE_E: 
           value = old_value & ~ COMPARATOR_22_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect22_comparator
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 5:5
 *
 **************************************************************************/
unsigned char get_acc_detect22_comparator()
  {
    unsigned char value;


    /* 
     * 0: disable AccDetect22 comparator 
     * 1: enable AccDetect22 comparator 
     */ 
    value = (I2CRead(ACC_DETECT_3_REG) & COMPARATOR_22_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect21_comparator
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 4:4
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
           value = old_value | COMPARATOR_21_ENABLE_PARAM_MASK; 
           break;
        case COMPARATOR_21_DISABLE_E: 
           value = old_value & ~ COMPARATOR_21_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect21_comparator
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 4:4
 *
 **************************************************************************/
unsigned char get_acc_detect21_comparator()
  {
    unsigned char value;


    /* 
     * 0: disable AccDetect21 comparator 
     * 1: enable AccDetect21 comparator 
     */ 
    value = (I2CRead(ACC_DETECT_3_REG) & COMPARATOR_21_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect_pullup
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 3:3
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
           value = old_value | ACC_DETECT_PULLUP_ENABLE_PARAM_MASK; 
           break;
        case ACC_DETECT_PULLUP_DISABLE_E: 
           value = old_value & ~ ACC_DETECT_PULLUP_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect_pullup
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 3:3
 *
 **************************************************************************/
unsigned char get_acc_detect_pullup()
  {
    unsigned char value;


    /* 
     * 0: disable AccDetect2 Pull-up 
     * 1: enable AccDetect2 Pull-up 
     */ 
    value = (I2CRead(ACC_DETECT_3_REG) & ACC_DETECT_PULLUP_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect_pullup_supply
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 1:1
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
           value = old_value | V21_SUPPLY_ENABLE_PARAM_MASK; 
           break;
        case V21_SUPPLY_DISABLE_E: 
           value = old_value & ~ V21_SUPPLY_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect_pullup_supply
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 1:1
 *
 **************************************************************************/
unsigned char get_acc_detect_pullup_supply()
  {
    unsigned char value;


    /* 
     * 0: disable AccDetect2 Pull-up 2.1v supply 
     * 1: enable AccDetect2 Pull-up 2.1v supply 
     */ 
    value = (I2CRead(ACC_DETECT_3_REG) & V21_SUPPLY_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_acc_detect1_comparator
 *
 * IN   : param, a value to write to the regiter AccDetect3
 * OUT  : 
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 0:0
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
           value = old_value | DETECT_1_COMPARATOR_ENABLE_PARAM_MASK; 
           break;
        case DETECT_1_COMPARATOR_DISABLE_E: 
           value = old_value & ~ DETECT_1_COMPARATOR_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ACC_DETECT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_acc_detect1_comparator
 *
 * RET  : Return the value of register AccDetect3
 *
 * Notes : From register 0x0882, bits 0:0
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
    value = (I2CRead(ACC_DETECT_3_REG) & DETECT_1_COMPARATOR_ENABLE_PARAM_MASK);
    return value;
  }


