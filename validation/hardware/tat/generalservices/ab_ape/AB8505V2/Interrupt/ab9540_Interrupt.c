/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Interrupt/Linux/ab9540_Interrupt.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_INTERRUPT_LINUX_AB9540_INTERRUPT_C
#define C__LISP_PRODUCTION_AB9540_AB9540_INTERRUPT_LINUX_AB9540_INTERRUPT_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Interrupt.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_interrupt_source6
 *
 * RET  : Return the value of register 0xInterruptSource6
 *
 * Notes : From register 0xE05, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source6()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(INTERRUPT_SOURCE_6_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_charging_current_limited_hs_chirp
 *
 * RET  : Return the value of register 0xInterruptSource22
 *
 * Notes : From register 0xE15, bits 6:6
 *
 **************************************************************************/
unsigned char get_charging_current_limited_hs_chirp()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: charging current is limited in HS or Chirp modes. 
     */ 
    value = (I2CRead(INTERRUPT_SOURCE_22_REG) & CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_xtal32_ok
 *
 * RET  : Return the value of register 0xInterruptSource22
 *
 * Notes : From register 0xE15, bits 7:7
 *
 **************************************************************************/
unsigned char get_xtal32_ok()
  {
    unsigned char value;


    /* 
     * 0: XTAL starting time is more than 250ms (125ms in 
     * MPW05/06) and XTAL clock is not selected as internal 
     * clock. Or Xtal failure during run time. 
     * 1: XTAL is running and starting time is lower than 250ms 
     * (125ms in MPW05/06). 
     */ 
    value = (I2CRead(INTERRUPT_SOURCE_22_REG) & XTAL_32_OK_MASK) >> 7;
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
