/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Gauge/Linux/ab9540_Gauge.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_GAUGE_LINUX_AB9540_GAUGE_C
#define C__LISP_PRODUCTION_AB9540_AB9540_GAUGE_LINUX_AB9540_GAUGE_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Gauge.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_coulomb_counter_read
 *
 * IN   : param, a value to write to the regiter CoulombCounterControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 0:0
 *
 **************************************************************************/
unsigned char set_coulomb_counter_read( enum read_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(COULOMB_COUNTER_CONTROL_REG);

    /* 
     * (auto-cleared) 
     * 0: inactive 
     * 1: allows to transfer the accumulator data, 
     * SampleConversion and SampleCounter data to the associated 
     * registers 
     */ 
    switch( param ){
        case READ_ENABLE_E: 
           value = old_value | COULOMB_COUNTER_READ_MASK; 
           break;
        case READ_DISABLE_E: 
           value = old_value & ~ COULOMB_COUNTER_READ_MASK;
           break;
    }
  

    I2CWrite(COULOMB_COUNTER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_read
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 0:0
 *
 **************************************************************************/
unsigned char get_coulomb_counter_read()
  {
    unsigned char value;


    /* 
     * (auto-cleared) 
     * 0: inactive 
     * 1: allows to transfer the accumulator data, 
     * SampleConversion and SampleCounter data to the associated 
     * registers 
     */ 
    value = (I2CRead(COULOMB_COUNTER_CONTROL_REG) & COULOMB_COUNTER_READ_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_reset_accumlator
 *
 * IN   : param, a value to write to the regiter CoulombCounterControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 1:1
 *
 **************************************************************************/
unsigned char set_coulomb_counter_reset_accumlator( enum reset_accumlator_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(COULOMB_COUNTER_CONTROL_REG);

    /* 
     * (auto-cleared) 
     * 0: inactive 
     * 1: allow to reset Accumulator and SampleConversion 
     * counters when a reading is done.r 
     */ 
    switch( param ){
        case RESET_ACCUMLATOR_E: 
           value = old_value | COULOMB_COUNTER_RESET_ACCUMLATOR_MASK; 
           break;
        case ACCUMLATOR_ENABLE_E: 
           value = old_value & ~ COULOMB_COUNTER_RESET_ACCUMLATOR_MASK;
           break;
    }
  

    I2CWrite(COULOMB_COUNTER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_reset_accumlator
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 1:1
 *
 **************************************************************************/
unsigned char get_coulomb_counter_reset_accumlator()
  {
    unsigned char value;


    /* 
     * (auto-cleared) 
     * 0: inactive 
     * 1: allow to reset Accumulator and SampleConversion 
     * counters when a reading is done.r 
     */ 
    value = (I2CRead(COULOMB_COUNTER_CONTROL_REG) & COULOMB_COUNTER_RESET_ACCUMLATOR_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_calibration_mode
 *
 * IN   : param, a value to write to the regiter CoulombCounterControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 3:3
 *
 **************************************************************************/
unsigned char set_coulomb_counter_calibration_mode( enum calibration_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(COULOMB_COUNTER_CONTROL_REG);

    /* 
     * 0: doesn't set ADconverter in calibration mode. 
     * 1: set ADconverter in calibration mode 
     */ 
    switch( param ){
        case CALIBRATION_ENABLE_E: 
           value = old_value | COULOMB_COUNTER_CALIBRATION_MODE_MASK; 
           break;
        case CALIBRATION_DISABLE_E: 
           value = old_value & ~ COULOMB_COUNTER_CALIBRATION_MODE_MASK;
           break;
    }
  

    I2CWrite(COULOMB_COUNTER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_calibration_mode
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 3:3
 *
 **************************************************************************/
unsigned char get_coulomb_counter_calibration_mode()
  {
    unsigned char value;


    /* 
     * 0: doesn't set ADconverter in calibration mode. 
     * 1: set ADconverter in calibration mode 
     */ 
    value = (I2CRead(COULOMB_COUNTER_CONTROL_REG) & COULOMB_COUNTER_CALIBRATION_MODE_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_offset_computation
 *
 * IN   : param, a value to write to the regiter CoulombCounterControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 4:4
 *
 **************************************************************************/
unsigned char set_coulomb_counter_offset_computation( enum computation_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(COULOMB_COUNTER_CONTROL_REG);

    /* 
     * (auto-cleared) 
     * 0: disable offset average computation. 
     * 1: enable offset average computation 
     */ 
    switch( param ){
        case COMPUTATION_ENABLE_E: 
           value = old_value | COULOMB_COUNTER_OFFSET_COMPUTATION_MASK; 
           break;
        case COMPUTATION_DISABLE_E: 
           value = old_value & ~ COULOMB_COUNTER_OFFSET_COMPUTATION_MASK;
           break;
    }
  

    I2CWrite(COULOMB_COUNTER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_offset_computation
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 4:4
 *
 **************************************************************************/
unsigned char get_coulomb_counter_offset_computation()
  {
    unsigned char value;


    /* 
     * (auto-cleared) 
     * 0: disable offset average computation. 
     * 1: enable offset average computation 
     */ 
    value = (I2CRead(COULOMB_COUNTER_CONTROL_REG) & COULOMB_COUNTER_OFFSET_COMPUTATION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_number_of_convertion
 *
 * IN   : param, a value to write to the regiter CoulombCounterControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 6:5
 *
 **************************************************************************/
unsigned char set_coulomb_counter_number_of_convertion( enum number_convertion param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(COULOMB_COUNTER_CONTROL_REG);

    /* 
     * 00: 4 
     * 01: 8 
     * 10: 16 
     */ 
  

     value =  old_value & ~COULOMB_COUNTER_NUMBER_OF_CONVERTION_MASK ;

    switch(  param ){

           case NUMBER_CONVERTION_4_E:
                value  = value  | (NUMBER_CONVERTION_4 << 0x5);
                break;
           case NUMBER_CONVERTION_8_E:
                value  = value  | (NUMBER_CONVERTION_8 << 0x5);
                break;
           case NUMBER_CONVERTION_16_E:
                value  = value  | (NUMBER_CONVERTION_16 << 0x5);
                break;
    }
  

    I2CWrite(COULOMB_COUNTER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_number_of_convertion
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 6:5
 *
 **************************************************************************/
unsigned char get_coulomb_counter_number_of_convertion()
  {
    unsigned char value;


    /* 
     * 00: 4 
     * 01: 8 
     * 10: 16 
     */ 
    value = (I2CRead(COULOMB_COUNTER_CONTROL_REG) & COULOMB_COUNTER_NUMBER_OF_CONVERTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_mux_offset
 *
 * IN   : param, a value to write to the regiter CoulombCounterControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 7:7
 *
 **************************************************************************/
unsigned char set_coulomb_counter_mux_offset( enum select param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(COULOMB_COUNTER_CONTROL_REG);

    /* 
     * 0: select offset from CC_Offset register. 
     * 1: select internal average offset 
     */ 
    switch( param ){
        case SELECT_INTERNAL_E: 
           value = old_value | COULOMB_COUNTER_MUX_OFFSET_MASK; 
           break;
        case SELECT_FROM_OFFSET_REGISTER_E: 
           value = old_value & ~ COULOMB_COUNTER_MUX_OFFSET_MASK;
           break;
    }
  

    I2CWrite(COULOMB_COUNTER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_mux_offset
 *
 * RET  : Return the value of register 0xCoulombCounterControl
 *
 * Notes : From register 0xC00, bits 7:7
 *
 **************************************************************************/
unsigned char get_coulomb_counter_mux_offset()
  {
    unsigned char value;


    /* 
     * 0: select offset from CC_Offset register. 
     * 1: select internal average offset 
     */ 
    value = (I2CRead(COULOMB_COUNTER_CONTROL_REG) & COULOMB_COUNTER_MUX_OFFSET_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_low
 *
 * RET  : Return the value of register 0xCoulombCounterLow
 *
 * Notes : From register 0xC07, bits 7:0
 *
 **************************************************************************/
unsigned char get_coulomb_counter_low()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(COULOMB_COUNTER_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_low
 *
 * IN   : param, a value to write to the regiter CoulombCounterLow
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombCounterLow
 *
 * Notes : From register 0xC07, bits 7:0
 *
 **************************************************************************/
unsigned char set_coulomb_counter_low( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(COULOMB_COUNTER_LOW_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(COULOMB_COUNTER_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_coulomb_counter_mid
 *
 * RET  : Return the value of register 0xCoulombCounterMid
 *
 * Notes : From register 0xC08, bits 7:0
 *
 **************************************************************************/
unsigned char get_coulomb_counter_mid()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(COULOMB_COUNTER_MID_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_counter_mid
 *
 * IN   : param, a value to write to the regiter CoulombCounterMid
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombCounterMid
 *
 * Notes : From register 0xC08, bits 7:0
 *
 **************************************************************************/
unsigned char set_coulomb_counter_mid( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(COULOMB_COUNTER_MID_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(COULOMB_COUNTER_MID_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_coulomb_sample_counter_average_offset
 *
 * RET  : Return the value of register 0xCoulombSampleCounterAverageOffset
 *
 * Notes : From register 0xC09, bits 7:0
 *
 **************************************************************************/
unsigned char get_coulomb_sample_counter_average_offset()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(COULOMB_SAMPLE_COUNTER_AVERAGE_OFFSET_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_sample_counter_average_offset
 *
 * IN   : param, a value to write to the regiter CoulombSampleCounterAverageOffset
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombSampleCounterAverageOffset
 *
 * Notes : From register 0xC09, bits 7:0
 *
 **************************************************************************/
unsigned char set_coulomb_sample_counter_average_offset( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(COULOMB_SAMPLE_COUNTER_AVERAGE_OFFSET_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(COULOMB_SAMPLE_COUNTER_AVERAGE_OFFSET_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_coulomb_sample_counter_offset
 *
 * RET  : Return the value of register 0xCoulombSampleCounterOffset
 *
 * Notes : From register 0xC0A, bits 7:0
 *
 **************************************************************************/
unsigned char get_coulomb_sample_counter_offset()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(COULOMB_SAMPLE_COUNTER_OFFSET_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_sample_counter_offset
 *
 * IN   : param, a value to write to the regiter CoulombSampleCounterOffset
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombSampleCounterOffset
 *
 * Notes : From register 0xC0A, bits 7:0
 *
 **************************************************************************/
unsigned char set_coulomb_sample_counter_offset( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(COULOMB_SAMPLE_COUNTER_OFFSET_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(COULOMB_SAMPLE_COUNTER_OFFSET_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_coulomb_sample_number_conversion
 *
 * RET  : Return the value of register 0xCoulombSampleNumberConversion
 *
 * Notes : From register 0xC10, bits 7:0
 *
 **************************************************************************/
unsigned char get_coulomb_sample_number_conversion()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(COULOMB_SAMPLE_NUMBER_CONVERSION_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_sample_number_conversion
 *
 * IN   : param, a value to write to the regiter CoulombSampleNumberConversion
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombSampleNumberConversion
 *
 * Notes : From register 0xC10, bits 7:0
 *
 **************************************************************************/
unsigned char set_coulomb_sample_number_conversion( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(COULOMB_SAMPLE_NUMBER_CONVERSION_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(COULOMB_SAMPLE_NUMBER_CONVERSION_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_comlomb_reset_bit_data
 *
 * IN   : param, a value to write to the regiter CoulombDataManagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombDataManagement
 *
 * Notes : From register 0xC11, bits 0:0
 *
 **************************************************************************/
unsigned char set_comlomb_reset_bit_data( enum bit_data_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(COULOMB_DATA_MANAGEMENT_REG);

    /* 
     * allows to reset the 21bits accumulator data 
     */ 
    switch( param ){
        case BIT_DATA_RESET_E: 
           value = old_value | COMLOMB_RESET_BIT_DATA_MASK; 
           break;
        case BIT_DATA_UN_RESET_E: 
           value = old_value & ~ COMLOMB_RESET_BIT_DATA_MASK;
           break;
    }
  

    I2CWrite(COULOMB_DATA_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_comlomb_reset_bit_data
 *
 * RET  : Return the value of register 0xCoulombDataManagement
 *
 * Notes : From register 0xC11, bits 0:0
 *
 **************************************************************************/
unsigned char get_comlomb_reset_bit_data()
  {
    unsigned char value;


    /* 
     * allows to reset the 21bits accumulator data 
     */ 
    value = (I2CRead(COULOMB_DATA_MANAGEMENT_REG) & COMLOMB_RESET_BIT_DATA_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_comlomb_data_transfert
 *
 * IN   : param, a value to write to the regiter CoulombDataManagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombDataManagement
 *
 * Notes : From register 0xC11, bits 1:1
 *
 **************************************************************************/
unsigned char set_comlomb_data_transfert( enum data_transfert param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(COULOMB_DATA_MANAGEMENT_REG);

    /* 
     * allows to transfer the accumulator data to the NconAccu 
     * registers 
     */ 
    switch( param ){
        case TRANSFERT_TO_NCONACCU_E: 
           value = old_value | COMLOMB_DATA_TRANSFERT_MASK; 
           break;
        case NOT_TRANSFERT_TO_NCONACCU_E: 
           value = old_value & ~ COMLOMB_DATA_TRANSFERT_MASK;
           break;
    }
  

    I2CWrite(COULOMB_DATA_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_comlomb_data_transfert
 *
 * RET  : Return the value of register 0xCoulombDataManagement
 *
 * Notes : From register 0xC11, bits 1:1
 *
 **************************************************************************/
unsigned char get_comlomb_data_transfert()
  {
    unsigned char value;


    /* 
     * allows to transfer the accumulator data to the NconAccu 
     * registers 
     */ 
    value = (I2CRead(COULOMB_DATA_MANAGEMENT_REG) & COMLOMB_DATA_TRANSFERT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_sample_number_conversion_low
 *
 * RET  : Return the value of register 0xCoulombSampleNumberConversionLow
 *
 * Notes : From register 0xC12, bits 7:0
 *
 **************************************************************************/
unsigned char get_coulomb_sample_number_conversion_low()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(COULOMB_SAMPLE_NUMBER_CONVERSION_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_sample_number_conversion_low
 *
 * IN   : param, a value to write to the regiter CoulombSampleNumberConversionLow
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombSampleNumberConversionLow
 *
 * Notes : From register 0xC12, bits 7:0
 *
 **************************************************************************/
unsigned char set_coulomb_sample_number_conversion_low( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(COULOMB_SAMPLE_NUMBER_CONVERSION_LOW_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(COULOMB_SAMPLE_NUMBER_CONVERSION_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_coulomb_sample_number_conversion_mid
 *
 * RET  : Return the value of register 0xCoulombSampleNumberConversionMid
 *
 * Notes : From register 0xC13, bits 7:0
 *
 **************************************************************************/
unsigned char get_coulomb_sample_number_conversion_mid()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(COULOMB_SAMPLE_NUMBER_CONVERSION_MID_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_sample_number_conversion_mid
 *
 * IN   : param, a value to write to the regiter CoulombSampleNumberConversionMid
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombSampleNumberConversionMid
 *
 * Notes : From register 0xC13, bits 7:0
 *
 **************************************************************************/
unsigned char set_coulomb_sample_number_conversion_mid( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(COULOMB_SAMPLE_NUMBER_CONVERSION_MID_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(COULOMB_SAMPLE_NUMBER_CONVERSION_MID_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_coulomb_sample_number_conversion_high
 *
 * RET  : Return the value of register 0xCoulombSampleNumberConversionHigh
 *
 * Notes : From register 0xC14, bits 7:0
 *
 **************************************************************************/
unsigned char get_coulomb_sample_number_conversion_high()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(COULOMB_SAMPLE_NUMBER_CONVERSION_HIGH_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_sample_number_conversion_high
 *
 * IN   : param, a value to write to the regiter CoulombSampleNumberConversionHigh
 * OUT  : 
 *
 * RET  : Return the value of register 0xCoulombSampleNumberConversionHigh
 *
 * Notes : From register 0xC14, bits 7:0
 *
 **************************************************************************/
unsigned char set_coulomb_sample_number_conversion_high( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(COULOMB_SAMPLE_NUMBER_CONVERSION_HIGH_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(COULOMB_SAMPLE_NUMBER_CONVERSION_HIGH_REG, param);
    return value;

  }




#ifdef __cplusplus
}
#endif

#endif
