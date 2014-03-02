/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Gauge/Linux/ab9540_Gauge_types.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_GAUGE_LINUX_AB9540_GAUGE_TYPES_H
#define C__LISP_PRODUCTION_AB9540_AB9540_GAUGE_LINUX_AB9540_GAUGE_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


//  
//  (auto-cleared) 
//  0: inactive 
//  1: allows to transfer the accumulator data, SampleConversion and SampleCounter data 
//  to the associated registers.
//  Register CoulombCounterControl 0x0C00, Bits 0:0, typedef read_enable
//   
//  
#define COULOMB_COUNTER_READ_MASK 0x1U
#define COULOMB_COUNTER_READ_OFFSET 0
#define COULOMB_COUNTER_READ_STOP_BIT 0
#define COULOMB_COUNTER_READ_START_BIT 0
#define COULOMB_COUNTER_READ_WIDTH 1

#define COULOMB_COUNTER_CONTROL_REG 0xC00

typedef enum read_enable {
    READ_DISABLE_E,
    READ_ENABLE_E
} READ_ENABLE_T ;
#define COULOMB_COUNTER_READ_PARAM_MASK  0x1



//  
//  (auto-cleared) 
//  0: inactive 
//  1: allow to reset Accumulator and SampleConversion counters when a reading is 
//  done.r.
//  Register CoulombCounterControl 0x0C00, Bits 1:1, typedef reset_accumlator_enable
//   
//  
#define COULOMB_COUNTER_RESET_ACCUMLATOR_MASK 0x2U
#define COULOMB_COUNTER_RESET_ACCUMLATOR_OFFSET 1
#define COULOMB_COUNTER_RESET_ACCUMLATOR_STOP_BIT 1
#define COULOMB_COUNTER_RESET_ACCUMLATOR_START_BIT 1
#define COULOMB_COUNTER_RESET_ACCUMLATOR_WIDTH 1


typedef enum reset_accumlator_enable {
    ACCUMLATOR_ENABLE_E,
    RESET_ACCUMLATOR_E
} RESET_ACCUMLATOR_ENABLE_T ;
#define COULOMB_COUNTER_RESET_ACCUMLATOR_PARAM_MASK  0x2



//  
//  0: doesn't set ADconverter in calibration mode. 
//  1: set ADconverter in calibration mode.
//  Register CoulombCounterControl 0x0C00, Bits 3:3, typedef calibration_enable
//   
//  
#define COULOMB_COUNTER_CALIBRATION_MODE_MASK 0x8U
#define COULOMB_COUNTER_CALIBRATION_MODE_OFFSET 3
#define COULOMB_COUNTER_CALIBRATION_MODE_STOP_BIT 3
#define COULOMB_COUNTER_CALIBRATION_MODE_START_BIT 3
#define COULOMB_COUNTER_CALIBRATION_MODE_WIDTH 1


typedef enum calibration_enable {
    CALIBRATION_DISABLE_E,
    CALIBRATION_ENABLE_E
} CALIBRATION_ENABLE_T ;
#define COULOMB_COUNTER_CALIBRATION_MODE_PARAM_MASK  0x8



//  
//  (auto-cleared) 
//  0: disable offset average computation. 
//  1: enable offset average computation.
//  Register CoulombCounterControl 0x0C00, Bits 4:4, typedef computation_enable
//   
//  
#define COULOMB_COUNTER_OFFSET_COMPUTATION_MASK 0x10U
#define COULOMB_COUNTER_OFFSET_COMPUTATION_OFFSET 4
#define COULOMB_COUNTER_OFFSET_COMPUTATION_STOP_BIT 4
#define COULOMB_COUNTER_OFFSET_COMPUTATION_START_BIT 4
#define COULOMB_COUNTER_OFFSET_COMPUTATION_WIDTH 1


typedef enum computation_enable {
    COMPUTATION_DISABLE_E,
    COMPUTATION_ENABLE_E
} COMPUTATION_ENABLE_T ;
#define COULOMB_COUNTER_OFFSET_COMPUTATION_PARAM_MASK  0x10



//  
//  00: 4 
//  01: 8 
//  10: 16.
//  Register CoulombCounterControl 0x0C00, Bits 5:6, typedef number_convertion
//   
//  
#define COULOMB_COUNTER_NUMBER_OF_CONVERTION_MASK 0x60U
#define COULOMB_COUNTER_NUMBER_OF_CONVERTION_OFFSET 6
#define COULOMB_COUNTER_NUMBER_OF_CONVERTION_STOP_BIT 6
#define COULOMB_COUNTER_NUMBER_OF_CONVERTION_START_BIT 5
#define COULOMB_COUNTER_NUMBER_OF_CONVERTION_WIDTH 2

#define  NUMBER_CONVERTION_4 0
#define  NUMBER_CONVERTION_8 1
#define  NUMBER_CONVERTION_16 2

typedef enum number_convertion {
    NUMBER_CONVERTION_4_E,
    NUMBER_CONVERTION_8_E,
    NUMBER_CONVERTION_16_E
} NUMBER_CONVERTION_T ;
#define COULOMB_COUNTER_NUMBER_OF_CONVERTION_PARAM_MASK  0x60



//  
//  0: select offset from CC_Offset register. 
//  1: select internal average offset.
//  Register CoulombCounterControl 0x0C00, Bits 7:7, typedef select
//   
//  
#define COULOMB_COUNTER_MUX_OFFSET_MASK 0x80U
#define COULOMB_COUNTER_MUX_OFFSET_OFFSET 7
#define COULOMB_COUNTER_MUX_OFFSET_STOP_BIT 7
#define COULOMB_COUNTER_MUX_OFFSET_START_BIT 7
#define COULOMB_COUNTER_MUX_OFFSET_WIDTH 1


typedef enum select {
    SELECT_FROM_OFFSET_REGISTER_E,
    SELECT_INTERNAL_E
} SELECT_T ;
#define COULOMB_COUNTER_MUX_OFFSET_PARAM_MASK  0x80


#define COULOMB_COUNTER_LOW_REG 0xC07
#define COULOMB_COUNTER_MID_REG 0xC08
#define COULOMB_SAMPLE_COUNTER_AVERAGE_OFFSET_REG 0xC09
#define COULOMB_SAMPLE_COUNTER_OFFSET_REG 0xC0A
#define COULOMB_SAMPLE_NUMBER_CONVERSION_REG 0xC10

//  
//  allows to reset the 21bits accumulator data.
//  Register CoulombDataManagement 0x0C11, Bits 0:0, typedef bit_data_reset
//   
//  
#define COMLOMB_RESET_BIT_DATA_MASK 0x1U
#define COMLOMB_RESET_BIT_DATA_OFFSET 0
#define COMLOMB_RESET_BIT_DATA_STOP_BIT 0
#define COMLOMB_RESET_BIT_DATA_START_BIT 0
#define COMLOMB_RESET_BIT_DATA_WIDTH 1

#define COULOMB_DATA_MANAGEMENT_REG 0xC11

typedef enum bit_data_reset {
    BIT_DATA_UN_RESET_E,
    BIT_DATA_RESET_E
} BIT_DATA_RESET_T ;
#define COMLOMB_RESET_BIT_DATA_PARAM_MASK  0x1



//  
//  allows to transfer the accumulator data to the NconAccu registers.
//  Register CoulombDataManagement 0x0C11, Bits 1:1, typedef data_transfert
//   
//  
#define COMLOMB_DATA_TRANSFERT_MASK 0x2U
#define COMLOMB_DATA_TRANSFERT_OFFSET 1
#define COMLOMB_DATA_TRANSFERT_STOP_BIT 1
#define COMLOMB_DATA_TRANSFERT_START_BIT 1
#define COMLOMB_DATA_TRANSFERT_WIDTH 1


typedef enum data_transfert {
    NOT_TRANSFERT_TO_NCONACCU_E,
    TRANSFERT_TO_NCONACCU_E
} DATA_TRANSFERT_T ;
#define COMLOMB_DATA_TRANSFERT_PARAM_MASK  0x2


#define COULOMB_SAMPLE_NUMBER_CONVERSION_LOW_REG 0xC12
#define COULOMB_SAMPLE_NUMBER_CONVERSION_MID_REG 0xC13
#define COULOMB_SAMPLE_NUMBER_CONVERSION_HIGH_REG 0xC14
#endif
