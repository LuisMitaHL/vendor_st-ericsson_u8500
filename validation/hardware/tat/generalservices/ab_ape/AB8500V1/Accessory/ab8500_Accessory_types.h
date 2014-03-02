/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Accessory/Linux/ab8500_Accessory_types.h
 * 
 *
 * Generated on the 26/02/2010 09:06 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_ACCESSORY_LINUX_AB8500_ACCESSORY_TYPES_H
#define C__LISP_PRODUCTION_AB8500_AB8500_ACCESSORY_LINUX_AB8500_ACCESSORY_TYPES_H



#define  DETECTION_THRESHOLD_150MV 0
#define  DETECTION_THRESHOLD_250MV 1
#define  DETECTION_THRESHOLD_350MV 2
#define  DETECTION_THRESHOLD_550MV 3
#define  DETECTION_THRESHOLD_750MV 4
#define  DETECTION_THRESHOLD_1050MV 5
#define  DETECTION_THRESHOLD_1500MV 6



typedef enum detection_threshold2 {
    DETECTION_THRESHOLD_150MV_E,
    DETECTION_THRESHOLD_250MV_E,
    DETECTION_THRESHOLD_350MV_E,
    DETECTION_THRESHOLD_550MV_E,
    DETECTION_THRESHOLD_750MV_E,
    DETECTION_THRESHOLD_1050MV_E,
    DETECTION_THRESHOLD_1500MV_E
} DETECTION_THRESHOLD_2_T ;



typedef enum detection_threshold {
    DETECTION_THRESHOLD_300_MV = 0x0,
    DETECTION_THRESHOLD_400_MV = 0x1,
    DETECTION_THRESHOLD_500_MV = 0x2,
    DETECTION_THRESHOLD_600_MV = 0x3,
    DETECTION_THRESHOLD_700_MV = 0x4,
    DETECTION_THRESHOLD_800_MV = 0x5,
    DETECTION_THRESHOLD_900_MV = 0x6,
    DETECTION_THRESHOLD_1000_MV = 0x7,
    DETECTION_THRESHOLD_1100_MV = 0x8,
    DETECTION_THRESHOLD_1200_MV = 0x9,
    DETECTION_THRESHOLD_1300_MV = 0xA,
    DETECTION_THRESHOLD_1400_MV = 0xB,
    DETECTION_THRESHOLD_1500_MV = 0xC,
    DETECTION_THRESHOLD_1600_MV = 0xD,
    DETECTION_THRESHOLD_1700_MV = 0xE,
    DETECTION_THRESHOLD_1800_MV = 0xF
} DETECTION_THRESHOLD_T ;



//  
//  Accessory Detection threshold on AccDetect1 ball.
//  Register AccDetect1 0x0880, Bits 3:6, typedef detection_threshold
//   
//  
#define ACC_DETECT_1_TH_MASK 0x78
#define ACC_DETECT_1_TH_OFFSET 6
#define ACC_DETECT_1_TH_STOP_BIT 6
#define ACC_DETECT_1_TH_START_BIT 3
#define ACC_DETECT_1_TH_WIDTH 4

#define ACC_DETECT_1_REG 0x880
#define ACC_DETECT_1_TH_PARAM_MASK  0x78
#define DETECTION_THRESHOLD_PARAM_MASK  0x78

//  
//  Accessory Detection threshold on AccDetect1 ball.
//  Register AccDetect1 0x0880, Bits 0:2, typedef detection_threshold2
//   
//  
#define ACC_DETECT_DB_MASK 0x7
#define ACC_DETECT_DB_OFFSET 2
#define ACC_DETECT_DB_STOP_BIT 2
#define ACC_DETECT_DB_START_BIT 0
#define ACC_DETECT_DB_WIDTH 3

#define ACC_DETECT_DB_PARAM_MASK  0x7
#define DETECTION_THRESHOLD_2_PARAM_MASK  0x7

//  
//  Accessory Detection threshold 2 on AccDetect1 ball.
//  Register AccDetect2 0x0881, Bits 4:7, typedef detection_threshold
//   
//  
#define ACC_DETECT_21_TH_MASK 0xF0
#define ACC_DETECT_21_TH_OFFSET 7
#define ACC_DETECT_21_TH_STOP_BIT 7
#define ACC_DETECT_21_TH_START_BIT 4
#define ACC_DETECT_21_TH_WIDTH 4

#define ACC_DETECT_2_REG 0x881
#define ACC_DETECT_21_TH_PARAM_MASK  0xF0

//  
//  Accessory Detection threshold 2 on AccDetect1 ball.
//  Register AccDetect2 0x0881, Bits 0:3, typedef detection_threshold
//   
//  
#define ACC_DETECT_22_DB_MASK 0xF
#define ACC_DETECT_22_DB_OFFSET 3
#define ACC_DETECT_22_DB_STOP_BIT 3
#define ACC_DETECT_22_DB_START_BIT 0
#define ACC_DETECT_22_DB_WIDTH 4

#define ACC_DETECT_22_DB_PARAM_MASK  0xF

//  
//  0: disable AccDetect22 comparator 
//  1: enable AccDetect22 comparator.
//  Register AccDetect3 0x0882, Bits 5:5, typedef comparator22_enable
//   
//  
#define ACC_DETECT_22_COMPARATOR_MASK 0x20
#define ACC_DETECT_22_COMPARATOR_OFFSET 5
#define ACC_DETECT_22_COMPARATOR_STOP_BIT 5
#define ACC_DETECT_22_COMPARATOR_START_BIT 5
#define ACC_DETECT_22_COMPARATOR_WIDTH 1

#define ACC_DETECT_3_REG 0x882

typedef enum comparator22_enable {
    COMPARATOR_22_DISABLE_E,
    COMPARATOR_22_ENABLE_E
} COMPARATOR_22_ENABLE_T ;

#define COMPARATOR_22_ENABLE_PARAM_MASK  0x20

//  
//  0: disable AccDetect21 comparator 
//  1: enable AccDetect21 comparator.
//  Register AccDetect3 0x0882, Bits 4:4, typedef comparator21_enable
//   
//  
#define ACC_DETECT_21_COMPARATOR_MASK 0x10
#define ACC_DETECT_21_COMPARATOR_OFFSET 4
#define ACC_DETECT_21_COMPARATOR_STOP_BIT 4
#define ACC_DETECT_21_COMPARATOR_START_BIT 4
#define ACC_DETECT_21_COMPARATOR_WIDTH 1


typedef enum comparator21_enable {
    COMPARATOR_21_DISABLE_E,
    COMPARATOR_21_ENABLE_E
} COMPARATOR_21_ENABLE_T ;

#define COMPARATOR_21_ENABLE_PARAM_MASK  0x10

//  
//  0: disable AccDetect2 Pull-up 
//  1: enable AccDetect2 Pull-up.
//  Register AccDetect3 0x0882, Bits 3:3, typedef acc_detect_pullup_enable
//   
//  
#define ACC_DETECT_PULLUP_MASK 0x8
#define ACC_DETECT_PULLUP_OFFSET 3
#define ACC_DETECT_PULLUP_STOP_BIT 3
#define ACC_DETECT_PULLUP_START_BIT 3
#define ACC_DETECT_PULLUP_WIDTH 1


typedef enum acc_detect_pullup_enable {
    ACC_DETECT_PULLUP_DISABLE_E,
    ACC_DETECT_PULLUP_ENABLE_E
} ACC_DETECT_PULLUP_ENABLE_T ;

#define ACC_DETECT_PULLUP_ENABLE_PARAM_MASK  0x8

//  
//  0: disable AccDetect2 Pull-up 2.1v supply 
//  1: enable AccDetect2 Pull-up 2.1v supply.
//  Register AccDetect3 0x0882, Bits 1:1, typedef v21_supply_enable
//   
//  
#define ACC_DETECT_PULLUP_SUPPLY_MASK 0x2
#define ACC_DETECT_PULLUP_SUPPLY_OFFSET 1
#define ACC_DETECT_PULLUP_SUPPLY_STOP_BIT 1
#define ACC_DETECT_PULLUP_SUPPLY_START_BIT 1
#define ACC_DETECT_PULLUP_SUPPLY_WIDTH 1


typedef enum v21_supply_enable {
    V21_SUPPLY_DISABLE_E,
    V21_SUPPLY_ENABLE_E
} V21_SUPPLY_ENABLE_T ;

#define V21_SUPPLY_ENABLE_PARAM_MASK  0x2

//  
//  Note: doesn't turn On internal pull-up 
//  0: disable AccDetect1 comparator 
//  1: enable AccDetect1 comparator.
//  Register AccDetect3 0x0882, Bits 0:0, typedef detect1_comparator_enable
//   
//  
#define ACC_DETECT_1_COMPARATOR_MASK 0x1
#define ACC_DETECT_1_COMPARATOR_OFFSET 0
#define ACC_DETECT_1_COMPARATOR_STOP_BIT 0
#define ACC_DETECT_1_COMPARATOR_START_BIT 0
#define ACC_DETECT_1_COMPARATOR_WIDTH 1


typedef enum detect1_comparator_enable {
    DETECT_1_COMPARATOR_DISABLE_E,
    DETECT_1_COMPARATOR_ENABLE_E
} DETECT_1_COMPARATOR_ENABLE_T ;

#define DETECT_1_COMPARATOR_ENABLE_PARAM_MASK  0x1
#endif
