/****************************************************************************
 * File : c:/Lisp/Production/ab8500_V2/ab8500_Regu2/Linux/ab8500_Regu2_types.h
 * 
 *
 * Generated on the 25/05/2011 07:54 by the 'gen-I2C' code generator 
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

#ifndef C__LISP_PRODUCTION_AB8500_V2_AB8500_REGU2_LINUX_AB8500_REGU2_TYPES_H
#define C__LISP_PRODUCTION_AB8500_V2_AB8500_REGU2_LINUX_AB8500_REGU2_TYPES_H




typedef enum selection1 {
    VBBNSEL1_REGISTER_E,
    VBBNSEL2_REGISTER_E
} SELECTION_1_T ;


#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3



typedef enum regulation {
    REGULATION_DISABLE_E,
    REGULATION_FORCE_HP_MODE_E,
    REGULATION_ALLOW_HW_E,
    REGULATION_FORCE_LOW_POWER_E
} REGULATION_T ;




typedef enum forced {
    INACTIVE_E,
    FORCED_E
} FORCED_T ;



typedef enum auto_mode {
    AUTO_MODE_NOT_ACTIVE_E,
    AUTO_MODE_ACTIVE_E
} AUTO_MODE_T ;


#define  SEL1_REGISTER 0
#define  SEL2_REGISTER 1
#define  SEL3_REGISTER 2



typedef enum selection {
    SEL1_REGISTER_E,
    SEL2_REGISTER_E,
    SEL3_REGISTER_E
} SELECTION_T ;




//  
//  0: inactive (Varm can be in PWM or PFM mode) 
//  1: Varm is forced in PWM mode.
//  Register ArmVoltageManagement 0x0400, Bits 7:7, typedef forced
//   
//  
#define ARM_VOLTAGE_PWM_MODE_MASK 0x80
#define ARM_VOLTAGE_PWM_MODE_OFFSET 7
#define ARM_VOLTAGE_PWM_MODE_STOP_BIT 7
#define ARM_VOLTAGE_PWM_MODE_START_BIT 7
#define ARM_VOLTAGE_PWM_MODE_WIDTH 1

#define ARM_VOLTAGE_MANAGEMENT_REG 0x400
#define ARM_VOLTAGE_PWM_MODE_PARAM_MASK  0x80
#define FORCED_PARAM_MASK  0x80

//  
//  0: Varm is not in AutoMode 
//  1: Varm is in AutoMode.
//  Register ArmVoltageManagement 0x0400, Bits 6:6, typedef auto_mode_enable
//   
//  
#define ARM_VOLTAGE_AUTO_MODE_MASK 0x40
#define ARM_VOLTAGE_AUTO_MODE_OFFSET 6
#define ARM_VOLTAGE_AUTO_MODE_STOP_BIT 6
#define ARM_VOLTAGE_AUTO_MODE_START_BIT 6
#define ARM_VOLTAGE_AUTO_MODE_WIDTH 1


typedef enum auto_mode_enable {
    AUTO_MODE_DISABLE_E,
    AUTO_MODE_ENABLE_E
} AUTO_MODE_ENABLE_T ;

#define AUTO_MODE_ENABLE_PARAM_MASK  0x40

//  
//  0: VBBN output voltage is defined by VBBNSel1 register 
//  1: VBBN output voltage is defined by VBBNSel2 register.
//  Register ArmVoltageManagement 0x0400, Bits 5:5, typedef selection1
//   
//  
#define VBBN_SELECTION_CONTROL_MASK 0x20
#define VBBN_SELECTION_CONTROL_OFFSET 5
#define VBBN_SELECTION_CONTROL_STOP_BIT 5
#define VBBN_SELECTION_CONTROL_START_BIT 5
#define VBBN_SELECTION_CONTROL_WIDTH 1

#define VBBN_SELECTION_CONTROL_PARAM_MASK  0x20
#define SELECTION_1_PARAM_MASK  0x20

//  
//  0: VBBP output voltage is defined by VBBNSel1 register 
//  1: VBBP output voltage is defined by VBBNSel2 register.
//  Register ArmVoltageManagement 0x0400, Bits 4:4, typedef selection1
//   
//  
#define VBBP_SELECTION_CONTROL_MASK 0x10
#define VBBP_SELECTION_CONTROL_OFFSET 4
#define VBBP_SELECTION_CONTROL_STOP_BIT 4
#define VBBP_SELECTION_CONTROL_START_BIT 4
#define VBBP_SELECTION_CONTROL_WIDTH 1

#define VBBP_SELECTION_CONTROL_PARAM_MASK  0x10

//  
//  00: Varm output voltage is defined by VarmSel1 register 
//  01: Varm output voltage is defined by VarmSel2 register 
//  1x: Varm output voltage is defined by VarmSel3 register.
//  Register ArmVoltageManagement 0x0400, Bits 2:3, typedef selection
//   
//  
#define ARM_OUPUT_VOLAGE_SELECT_MASK 0xC
#define ARM_OUPUT_VOLAGE_SELECT_OFFSET 3
#define ARM_OUPUT_VOLAGE_SELECT_STOP_BIT 3
#define ARM_OUPUT_VOLAGE_SELECT_START_BIT 2
#define ARM_OUPUT_VOLAGE_SELECT_WIDTH 2

#define ARM_OUPUT_VOLAGE_SELECT_PARAM_MASK  0xC
#define SELECTION_PARAM_MASK  0xC

//  
//  00: force Varm disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VarmRequest signal) to control Varm supply if 
//  VarmRequestCtrl[1:0]=0x. Forced in HP mode if VarmRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register ArmVoltageManagement 0x0400, Bits 0:1, typedef regulation
//   
//  
#define ARM_OUPUT_REGULATION_MASK 0x3
#define ARM_OUPUT_REGULATION_OFFSET 1
#define ARM_OUPUT_REGULATION_STOP_BIT 1
#define ARM_OUPUT_REGULATION_START_BIT 0
#define ARM_OUPUT_REGULATION_WIDTH 2

#define ARM_OUPUT_REGULATION_PARAM_MASK  0x3
#define REGULATION_PARAM_MASK  0x3

//  
//  1: inactive 
//  0: Force no voltage step between Vbbp and Varm whatever is VbbpSelx[3:0].
//  Register ArmVoltageBackManagement 0x0401, Bits 6:6, typedef force_no_step
//   
//  
#define FORCE_NO_STEP_MASK 0x40
#define FORCE_NO_STEP_OFFSET 6
#define FORCE_NO_STEP_STOP_BIT 6
#define FORCE_NO_STEP_START_BIT 6
#define FORCE_NO_STEP_WIDTH 1

#define ARM_VOLTAGE_BACK_MANAGEMENT_REG 0x401
#define  FORCE_NO_STEP 64

typedef enum force_no_step {
    FORCE_NO_VOLTAGE_FORCED_E,
    FORCE_NO_VOLTAGE_INACTIVE_E
} FORCE_NO_STEP_T ;
#define FORCE_NO_STEP_PARAM_MASK  0x40



//  
//  0: disable pull-down (switch) between VBBN and gnd 
//  1: enable pull-down (switch) between VBBN and gnd].
//  Register ArmVoltageBackManagement 0x0401, Bits 5:5, typedef vbbp_pulldonw_enable
//   
//  
#define VBBP_PULLDONW_ENABLE_MASK 0x20
#define VBBP_PULLDONW_ENABLE_OFFSET 5
#define VBBP_PULLDONW_ENABLE_STOP_BIT 5
#define VBBP_PULLDONW_ENABLE_START_BIT 5
#define VBBP_PULLDONW_ENABLE_WIDTH 1


typedef enum vbbp_pulldonw_enable {
    VBBP_PULLDONW_DISABLE_E,
    VBBP_PULLDONW_ENABLE_E
} VBBP_PULLDONW_ENABLE_T ;

#define VBBP_PULLDONW_ENABLE_PARAM_MASK  0x20

//  
//  00: force Vbbn disabled (switch between VBBN and ground closed) 
//  01: force enabled in HP mode 
//  10: allows HW (internal VbbnRequest signal) to control Vbbn supply if 
//  VbbnRequestCtrl[1:0]=0x. Forced in HP mode if VbbnRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register ArmVoltageBackManagement 0x0401, Bits 2:3, typedef regulation
//   
//  
#define VBBN_REGULATION_MASK 0xC
#define VBBN_REGULATION_OFFSET 3
#define VBBN_REGULATION_STOP_BIT 3
#define VBBN_REGULATION_START_BIT 2
#define VBBN_REGULATION_WIDTH 2

#define VBBN_REGULATION_PARAM_MASK  0xC

//  
//  00: force Vbbp disabled (switch between Varm and ground closed) 
//  01: force enabled in HP mode 
//  10: allows HW (internal VbbpRequest signal) to control Vbbp supply if 
//  VbbpRequestCtrl[1:0]=0x. Forced in HP mode if VbbpRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register ArmVoltageBackManagement 0x0401, Bits 0:1, typedef regulation
//   
//  
#define VBBP_REGULATION_MASK 0x3
#define VBBP_REGULATION_OFFSET 1
#define VBBP_REGULATION_STOP_BIT 1
#define VBBP_REGULATION_START_BIT 0
#define VBBP_REGULATION_WIDTH 2

#define VBBP_REGULATION_PARAM_MASK  0x3

//  
//  0: inactive (Vape can be in PWM or PFM mode) 
//  1: Vape is forced in PWM mode.
//  Register VapeVoltageManagement 0x0402, Bits 4:4, typedef forced
//   
//  
#define VAPE_PWM_MODE_FORCED_MASK 0x10
#define VAPE_PWM_MODE_FORCED_OFFSET 4
#define VAPE_PWM_MODE_FORCED_STOP_BIT 4
#define VAPE_PWM_MODE_FORCED_START_BIT 4
#define VAPE_PWM_MODE_FORCED_WIDTH 1

#define VAPE_VOLTAGE_MANAGEMENT_REG 0x402
#define VAPE_PWM_MODE_FORCED_PARAM_MASK  0x10

//  
//  0: Vape is not in AutoMode 
//  1: Vape is in AutoMode.
//  Register VapeVoltageManagement 0x0402, Bits 3:3, typedef auto_mode
//   
//  
#define VAPE_AUTO_MODE_MASK 0x8
#define VAPE_AUTO_MODE_OFFSET 3
#define VAPE_AUTO_MODE_STOP_BIT 3
#define VAPE_AUTO_MODE_START_BIT 3
#define VAPE_AUTO_MODE_WIDTH 1

#define VAPE_AUTO_MODE_PARAM_MASK  0x8
#define AUTO_MODE_PARAM_MASK  0x8

//  
//  0: Vape output voltage is defined by VapeSel1 register 
//  1: Vape output voltage is defined by VapeSel2 register.
//  Register VapeVoltageManagement 0x0402, Bits 2:2, typedef ape_selection
//   
//  
#define VAPE_SELECTION_CONTROL_MASK 0x4
#define VAPE_SELECTION_CONTROL_OFFSET 2
#define VAPE_SELECTION_CONTROL_STOP_BIT 2
#define VAPE_SELECTION_CONTROL_START_BIT 2
#define VAPE_SELECTION_CONTROL_WIDTH 1

#define  APE_SELECTION 4

typedef enum ape_selection {
    V_APE_SEL1_REGISTER_E,
    V_APE_SEL2_REGISTER_E
} APE_SELECTION_T ;
#define APE_SELECTION_PARAM_MASK  0x4



//  
//  00: force Vape disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VapeRequest signal) to control Vape supply if 
//  VapeRequestCtrl[1:0]=0x. Forced in HP mode if VapeRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VapeVoltageManagement 0x0402, Bits 0:1, typedef regulation
//   
//  
#define VAPE_REGULATION_MASK 0x3
#define VAPE_REGULATION_OFFSET 1
#define VAPE_REGULATION_STOP_BIT 1
#define VAPE_REGULATION_START_BIT 0
#define VAPE_REGULATION_WIDTH 2

#define VAPE_REGULATION_PARAM_MASK  0x3

//  
//  0: inactive (Vsmps1 can be in PWM or PFM mode) 
//  1: Vsmps1 is forced in PWM mode.
//  Register Vsmps1VoltageManagement 0x0403, Bits 5:5, typedef forced
//   
//  
#define VSMPS_1_VOLTAGE_PWM_MODE_MASK 0x20
#define VSMPS_1_VOLTAGE_PWM_MODE_OFFSET 5
#define VSMPS_1_VOLTAGE_PWM_MODE_STOP_BIT 5
#define VSMPS_1_VOLTAGE_PWM_MODE_START_BIT 5
#define VSMPS_1_VOLTAGE_PWM_MODE_WIDTH 1

#define VSMPS_1_VOLTAGE_MANAGEMENT_REG 0x403
#define VSMPS_1_VOLTAGE_PWM_MODE_PARAM_MASK  0x20

//  
//  0: Vape is not in AutoMode 
//  1: Vape is in AutoMode.
//  Register Vsmps1VoltageManagement 0x0403, Bits 4:4, typedef auto_mode
//   
//  
#define VSMPS_1_AUTO_MODE_MASK 0x10
#define VSMPS_1_AUTO_MODE_OFFSET 4
#define VSMPS_1_AUTO_MODE_STOP_BIT 4
#define VSMPS_1_AUTO_MODE_START_BIT 4
#define VSMPS_1_AUTO_MODE_WIDTH 1

#define VSMPS_1_AUTO_MODE_PARAM_MASK  0x10

//  
//  00: Vsmps1 output voltage is defined by Vsmps1Sel1 register 
//  01: Vsmps1 output voltage is defined by Vsmps1Sel2 register 
//  1x: Vsmps1 output voltage is defined by Vsmps1Sel3 register.
//  Register Vsmps1VoltageManagement 0x0403, Bits 2:3, typedef selection
//   
//  
#define VSMPS_1_OUPUT_VOLAGE_SELECT_MASK 0xC
#define VSMPS_1_OUPUT_VOLAGE_SELECT_OFFSET 3
#define VSMPS_1_OUPUT_VOLAGE_SELECT_STOP_BIT 3
#define VSMPS_1_OUPUT_VOLAGE_SELECT_START_BIT 2
#define VSMPS_1_OUPUT_VOLAGE_SELECT_WIDTH 2

#define VSMPS_1_OUPUT_VOLAGE_SELECT_PARAM_MASK  0xC

//  
//  00: force Vsmps1 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vsmps1Request signal) to control Vsmps1 supply if 
//  Vsmps1RequestCtrl[1:0]=0x. Forced in HP mode if Vsmps1RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vsmps1VoltageManagement 0x0403, Bits 0:1, typedef regulation
//   
//  
#define VSMPS_1_REGULATION_MASK 0x3
#define VSMPS_1_REGULATION_OFFSET 1
#define VSMPS_1_REGULATION_STOP_BIT 1
#define VSMPS_1_REGULATION_START_BIT 0
#define VSMPS_1_REGULATION_WIDTH 2

#define VSMPS_1_REGULATION_PARAM_MASK  0x3

//  
//  0: inactive (Vsmps2 can be in PWM or PFM mode) 
//  1: Vsmps2 is forced in PWM mode.
//  Register Vsmps2VoltageManagement 0x0404, Bits 5:5, typedef forced
//   
//  
#define VSMPS_2_VOLTAGE_PWM_MODE_MASK 0x20
#define VSMPS_2_VOLTAGE_PWM_MODE_OFFSET 5
#define VSMPS_2_VOLTAGE_PWM_MODE_STOP_BIT 5
#define VSMPS_2_VOLTAGE_PWM_MODE_START_BIT 5
#define VSMPS_2_VOLTAGE_PWM_MODE_WIDTH 1

#define VSMPS_2_VOLTAGE_MANAGEMENT_REG 0x404
#define VSMPS_2_VOLTAGE_PWM_MODE_PARAM_MASK  0x20

//  
//  0: Vape is not in AutoMode 
//  1: Vape is in AutoMode.
//  Register Vsmps2VoltageManagement 0x0404, Bits 4:4, typedef auto_mode
//   
//  
#define VSMPS_2_AUTO_MODE_MASK 0x10
#define VSMPS_2_AUTO_MODE_OFFSET 4
#define VSMPS_2_AUTO_MODE_STOP_BIT 4
#define VSMPS_2_AUTO_MODE_START_BIT 4
#define VSMPS_2_AUTO_MODE_WIDTH 1

#define VSMPS_2_AUTO_MODE_PARAM_MASK  0x10

//  
//  00: Vsmps2 output voltage is defined by Vsmps2Sel1 register 
//  01: Vsmps2 output voltage is defined by Vsmps2Sel2 register 
//  1x: Vsmps2 output voltage is defined by Vsmps2Sel3 register.
//  Register Vsmps2VoltageManagement 0x0404, Bits 2:3, typedef selection
//   
//  
#define VSMPS_2_OUPUT_VOLAGE_SELECT_MASK 0xC
#define VSMPS_2_OUPUT_VOLAGE_SELECT_OFFSET 3
#define VSMPS_2_OUPUT_VOLAGE_SELECT_STOP_BIT 3
#define VSMPS_2_OUPUT_VOLAGE_SELECT_START_BIT 2
#define VSMPS_2_OUPUT_VOLAGE_SELECT_WIDTH 2

#define VSMPS_2_OUPUT_VOLAGE_SELECT_PARAM_MASK  0xC

//  
//  00: force Vsmps2 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vsmps2Request signal) to control Vsmps2 supply if 
//  Vsmps2RequestCtrl[1:0]=0x. Forced in HP mode if Vsmps2RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vsmps2VoltageManagement 0x0404, Bits 0:1, typedef regulation
//   
//  
#define VSMPS_2_REGULATION_MASK 0x3
#define VSMPS_2_REGULATION_OFFSET 1
#define VSMPS_2_REGULATION_STOP_BIT 1
#define VSMPS_2_REGULATION_START_BIT 0
#define VSMPS_2_REGULATION_WIDTH 2

#define VSMPS_2_REGULATION_PARAM_MASK  0x3

//  
//  0: inactive (Vsmps3 can be in PWM or PFM mode) 
//  1: Vsmps3 is forced in PWM mode.
//  Register Vsmps3VoltageManagement 0x0405, Bits 5:5, typedef forced
//   
//  
#define VSMPS_3_VOLTAGE_PWM_MODE_MASK 0x20
#define VSMPS_3_VOLTAGE_PWM_MODE_OFFSET 5
#define VSMPS_3_VOLTAGE_PWM_MODE_STOP_BIT 5
#define VSMPS_3_VOLTAGE_PWM_MODE_START_BIT 5
#define VSMPS_3_VOLTAGE_PWM_MODE_WIDTH 1

#define VSMPS_3_VOLTAGE_MANAGEMENT_REG 0x405
#define VSMPS_3_VOLTAGE_PWM_MODE_PARAM_MASK  0x20

//  
//  0: Vape is not in AutoMode 
//  1: Vape is in AutoMode.
//  Register Vsmps3VoltageManagement 0x0405, Bits 4:4, typedef auto_mode
//   
//  
#define VSMPS_3_AUTO_MODE_MASK 0x10
#define VSMPS_3_AUTO_MODE_OFFSET 4
#define VSMPS_3_AUTO_MODE_STOP_BIT 4
#define VSMPS_3_AUTO_MODE_START_BIT 4
#define VSMPS_3_AUTO_MODE_WIDTH 1

#define VSMPS_3_AUTO_MODE_PARAM_MASK  0x10

//  
//  00: Vsmps3 output voltage is defined by Vsmps3Sel1 register 
//  01: Vsmps3 output voltage is defined by Vsmps3Sel2 register 
//  1x: Vsmps3 output voltage is defined by Vsmps3Sel3 register.
//  Register Vsmps3VoltageManagement 0x0405, Bits 2:3, typedef selection
//   
//  
#define VSMPS_3_OUPUT_VOLAGE_SELECT_MASK 0xC
#define VSMPS_3_OUPUT_VOLAGE_SELECT_OFFSET 3
#define VSMPS_3_OUPUT_VOLAGE_SELECT_STOP_BIT 3
#define VSMPS_3_OUPUT_VOLAGE_SELECT_START_BIT 2
#define VSMPS_3_OUPUT_VOLAGE_SELECT_WIDTH 2

#define VSMPS_3_OUPUT_VOLAGE_SELECT_PARAM_MASK  0xC

//  
//  00: force Vsmps3 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vsmps3Request signal) to control Vsmps3 supply if 
//  Vsmps3RequestCtrl[1:0]=0x. Forced in HP mode if Vsmps3RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vsmps3VoltageManagement 0x0405, Bits 0:1, typedef regulation
//   
//  
#define VSMPS_3_REGULATION_MASK 0x3
#define VSMPS_3_REGULATION_OFFSET 1
#define VSMPS_3_REGULATION_STOP_BIT 1
#define VSMPS_3_REGULATION_START_BIT 0
#define VSMPS_3_REGULATION_WIDTH 2

#define VSMPS_3_REGULATION_PARAM_MASK  0x3

//  
//  00: force Vana disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VanaRequest signal) to control Vana supply if 
//  VanaRequestCtrl[1:0]=0x. Forced in HP mode if VanaRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VanaVoltageManagement 0x0406, Bits 2:3, typedef regulation
//   
//  
#define VANA_REGULATION_MASK 0xC
#define VANA_REGULATION_OFFSET 3
#define VANA_REGULATION_STOP_BIT 3
#define VANA_REGULATION_START_BIT 2
#define VANA_REGULATION_WIDTH 2

#define VANA_VOLTAGE_MANAGEMENT_REG 0x406
#define VANA_REGULATION_PARAM_MASK  0xC

//  
//  00: force Vpll disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VpllRequest signal) to control Vpll supply if 
//  VpllRequestCtrl[1:0]=0x. Forced in HP mode if VpllRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VanaVoltageManagement 0x0406, Bits 0:1, typedef regulation
//   
//  
#define VPLL_REGULATION_MASK 0x3
#define VPLL_REGULATION_OFFSET 1
#define VPLL_REGULATION_STOP_BIT 1
#define VPLL_REGULATION_START_BIT 0
#define VPLL_REGULATION_WIDTH 2

#define VPLL_REGULATION_PARAM_MASK  0x3

//  
//  0: inactive 
//  1: enable 50KOhms pulldown.
//  Register VrefDdrVoltageManagement 0x0407, Bits 1:1, typedef sleep_mode
//   
//  
#define VREF_DDR_SLEEP_MODE_MASK 0x2
#define VREF_DDR_SLEEP_MODE_OFFSET 1
#define VREF_DDR_SLEEP_MODE_STOP_BIT 1
#define VREF_DDR_SLEEP_MODE_START_BIT 1
#define VREF_DDR_SLEEP_MODE_WIDTH 1

#define VREF_DDR_VOLTAGE_MANAGEMENT_REG 0x407
#define  SLEEP_MODE 2

typedef enum sleep_mode {
    SLEEP_MODE_INACTIVE_E,
    SLEEP_MODE_ENABLE_E
} SLEEP_MODE_T ;
#define SLEEP_MODE_PARAM_MASK  0x2



//  
//  0: disable VrefDDR (output grounded) 
//  1: enable VrefDDR.
//  Register VrefDdrVoltageManagement 0x0407, Bits 0:0, typedef vref_ddr_enable
//   
//  
#define VREF_DDR_ENABLE_MASK 0x1
#define VREF_DDR_ENABLE_OFFSET 0
#define VREF_DDR_ENABLE_STOP_BIT 0
#define VREF_DDR_ENABLE_START_BIT 0
#define VREF_DDR_ENABLE_WIDTH 1


typedef enum vref_ddr_enable {
    VREF_DDR_DISABLE_E,
    VREF_DDR_ENABLE_E
} VREF_DDR_ENABLE_T ;

#define VREF_DDR_ENABLE_PARAM_MASK  0x1

typedef enum by_pass {
    WITHOUT_BYPASS_E,
    WITH_BYPASS_E
} BY_PASS_T ;



//  
//  External Smps is with ByPass mode ExtSupply3.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 7:7, typedef by_pass
//   
//  
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_MASK 0x80
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_OFFSET 7
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_STOP_BIT 7
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_START_BIT 7
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_WIDTH 1

#define VEXT_SUPPPLY_1_VOLTAGE_MANAGEMENT_REG 0x408
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_PARAM_MASK  0x80
#define BY_PASS_PARAM_MASK  0x80

//  
//  External Smps is with ByPass mode ExtSupply2.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 6:6, typedef by_pass
//   
//  
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_MASK 0x40
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_OFFSET 6
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_STOP_BIT 6
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_START_BIT 6
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_WIDTH 1

#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_PARAM_MASK  0x40

//  
//  00: force VextSuppply3 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VextSuppply3Request signal) to control VextSuppply3 supply if 
//  VextSuppply3RequestCtrl[1:0]=0x. Forced in HP mode if VextSuppply3RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 4:5, typedef regulation
//   
//  
#define VEXT_SUPPPLY_3_REGULATION_MASK 0x30
#define VEXT_SUPPPLY_3_REGULATION_OFFSET 5
#define VEXT_SUPPPLY_3_REGULATION_STOP_BIT 5
#define VEXT_SUPPPLY_3_REGULATION_START_BIT 4
#define VEXT_SUPPPLY_3_REGULATION_WIDTH 2

#define VEXT_SUPPPLY_3_REGULATION_PARAM_MASK  0x30

//  
//  00: force VextSuppply2 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VextSuppply2Request signal) to control VextSuppply2 supply if 
//  VextSuppply2RequestCtrl[1:0]=0x. Forced in HP mode if VextSuppply2RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 2:3, typedef regulation
//   
//  
#define VEXT_SUPPPLY_2_REGULATION_MASK 0xC
#define VEXT_SUPPPLY_2_REGULATION_OFFSET 3
#define VEXT_SUPPPLY_2_REGULATION_STOP_BIT 3
#define VEXT_SUPPPLY_2_REGULATION_START_BIT 2
#define VEXT_SUPPPLY_2_REGULATION_WIDTH 2

#define VEXT_SUPPPLY_2_REGULATION_PARAM_MASK  0xC

//  
//  00: force VextSuppply1 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VextSuppply1Request signal) to control VextSuppply1 supply if 
//  VextSuppply1RequestCtrl[1:0]=0x. Forced in HP mode if VextSuppply1RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 0:1, typedef regulation
//   
//  
#define VEXT_SUPPPLY_1_REGULATION_MASK 0x3
#define VEXT_SUPPPLY_1_REGULATION_OFFSET 1
#define VEXT_SUPPPLY_1_REGULATION_STOP_BIT 1
#define VEXT_SUPPPLY_1_REGULATION_START_BIT 0
#define VEXT_SUPPPLY_1_REGULATION_WIDTH 2

#define VEXT_SUPPPLY_1_REGULATION_PARAM_MASK  0x3

//  
//  00: force Vaux2 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vaux2Request signal) to control Vaux2 supply if 
//  Vaux2RequestCtrl[1:0]=0x. Forced in HP mode if Vaux2RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VauxVoltageManagement 0x0409, Bits 2:3, typedef regulation
//   
//  
#define VAUX_2_REGULATION_MASK 0xC
#define VAUX_2_REGULATION_OFFSET 3
#define VAUX_2_REGULATION_STOP_BIT 3
#define VAUX_2_REGULATION_START_BIT 2
#define VAUX_2_REGULATION_WIDTH 2

#define VAUX_VOLTAGE_MANAGEMENT_REG 0x409
#define VAUX_2_REGULATION_PARAM_MASK  0xC

//  
//  00: force Vaux1 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vaux1Request signal) to control Vaux1 supply if 
//  Vaux1RequestCtrl[1:0]=0x. Forced in HP mode if Vaux1RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VauxVoltageManagement 0x0409, Bits 0:1, typedef regulation
//   
//  
#define VAUX_1_REGULATION_MASK 0x3
#define VAUX_1_REGULATION_OFFSET 1
#define VAUX_1_REGULATION_STOP_BIT 1
#define VAUX_1_REGULATION_START_BIT 0
#define VAUX_1_REGULATION_WIDTH 2

#define VAUX_1_REGULATION_PARAM_MASK  0x3

//  
//  00: force Vrf1 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vrf1Request signal) to control Vrf1 supply if 
//  Vrf1RequestCtrl[1:0]=0x. Forced in HP mode if Vrf1RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vaux3VoltageManagement 0x040A, Bits 2:3, typedef regulation
//   
//  
#define VRF_1_REGULATION_MASK 0xC
#define VRF_1_REGULATION_OFFSET 3
#define VRF_1_REGULATION_STOP_BIT 3
#define VRF_1_REGULATION_START_BIT 2
#define VRF_1_REGULATION_WIDTH 2

#define VAUX_3_VOLTAGE_MANAGEMENT_REG 0x40A
#define VRF_1_REGULATION_PARAM_MASK  0xC

//  
//  00: force Vaux3 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vaux3Request signal) to control Vaux3 supply if 
//  Vaux3RequestCtrl[1:0]=0x. Forced in HP mode if Vaux3RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vaux3VoltageManagement 0x040A, Bits 0:1, typedef regulation
//   
//  
#define VAUX_3_REGULATION_MASK 0x3
#define VAUX_3_REGULATION_OFFSET 1
#define VAUX_3_REGULATION_STOP_BIT 1
#define VAUX_3_REGULATION_START_BIT 0
#define VAUX_3_REGULATION_WIDTH 2

#define VAUX_3_REGULATION_PARAM_MASK  0x3
typedef enum output_supply_high {
    OUTPUT_VOLTAGE_1_POINT_8000_V = 0x0,
    OUTPUT_VOLTAGE_1_POINT_8125_V = 0x1,
    OUTPUT_VOLTAGE_1_POINT_8250_V = 0x2,
    OUTPUT_VOLTAGE_1_POINT_8375_V = 0x3,
    OUTPUT_VOLTAGE_1_POINT_8500_V = 0x4,
    OUTPUT_VOLTAGE_1_POINT_8625_V = 0x5,
    OUTPUT_VOLTAGE_1_POINT_8750_V = 0x6
} OUTPUT_SUPPLY_HIGH_T ;


typedef enum output_supply {
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_7000_V = 0x0,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_7125_V = 0x1,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_7250_V = 0x2,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_7375_V = 0x3,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_7500_V = 0x4,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_7625_V = 0x5,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_7750_V = 0x6,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_7875_V = 0x7,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_8000_V = 0x8,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_8125_V = 0x9,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_8250_V = 0xA,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_8375_V = 0xB,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_8500_V = 0xC,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_8625_V = 0xD,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_8750_V = 0xE,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_8875_V = 0xF,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_9000_V = 0x10,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_9125_V = 0x11,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_9250_V = 0x12,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_9375_V = 0x13,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_9500_V = 0x14,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_9625_V = 0x15,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_9750_V = 0x16,
    OUTPUT_SUPPLY_VOLTAGE_0_POINT_9875_V = 0x17,
    OUTPUT_SUPPLY_VOLTAGE_1_V = 0x18,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_0125_V = 0x19,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_0250_V = 0x1A,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_0375_V = 0x1B,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_0500_V = 0x1C,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_0625_V = 0x1D,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_0750_V = 0x1E,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_0875_V = 0x1F,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_1000_V = 0x20,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_1125_V = 0x21,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_1250_V = 0x22,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_1375_V = 0x23,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_1500_V = 0x24,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_1625_V = 0x25,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_1750_V = 0x26,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_1875_V = 0x27,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_2000_V = 0x28,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_2125_V = 0x29,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_2250_V = 0x2A,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_2375_V = 0x2B,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_2500_V = 0x2C,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_2625_V = 0x2D,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_2750_V = 0x2E,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_2875_V = 0x2F,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_3000_V = 0x30,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_3125_V = 0x31,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_3250_V = 0x32,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_3375_V = 0x33,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_3500_V = 0x34,
    OUTPUT_SUPPLY_VOLTAGE_1_POINT_3625_V = 0x35
} OUTPUT_SUPPLY_T ;


typedef enum output_supply_med {
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_1000_V = 0x20,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_1125_V = 0x21,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_1250_V = 0x22,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_1375_V = 0x23,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_1500_V = 0x24,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_1625_V = 0x25,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_1750_V = 0x26,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_1875_V = 0x27,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_2000_V = 0x28,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_2125_V = 0x29,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_2250_V = 0x2A,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_2375_V = 0x2B,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_2500_V = 0x2C,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_2625_V = 0x2D,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_2750_V = 0x2E,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_2875_V = 0x2F,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_POINT_3000_V = 0x30
} OUTPUT_SUPPLY_MED_T ;


typedef enum output_supply_large {
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_7000_V = 0x0,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_7125_V = 0x1,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_7250_V = 0x2,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_7375_V = 0x3,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_7500_V = 0x4,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_7625_V = 0x5,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_7750_V = 0x6,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_7875_V = 0x7,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_8000_V = 0x8,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_8125_V = 0x9,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_8250_V = 0xA,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_8375_V = 0xB,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_8500_V = 0xC,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_8625_V = 0xD,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_8750_V = 0xE,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_8875_V = 0xF,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_9000_V = 0x10,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_9125_V = 0x11,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_9250_V = 0x12,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_9375_V = 0x13,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_9500_V = 0x14,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_9625_V = 0x15,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_9750_V = 0x16,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_POINT_9875_V = 0x17,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_V = 0x18,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_0125_V = 0x19,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_0250_V = 0x1A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_0375_V = 0x1B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_0500_V = 0x1C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_0625_V = 0x1D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_0750_V = 0x1E,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_0875_V = 0x1F,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_1000_V = 0x20,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_1125_V = 0x21,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_1250_V = 0x22,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_1375_V = 0x23,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_1500_V = 0x24,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_1625_V = 0x25,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_1750_V = 0x26,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_1875_V = 0x27,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_2000_V = 0x28,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_2125_V = 0x29,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_2250_V = 0x2A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_2375_V = 0x2B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_2500_V = 0x2C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_2625_V = 0x2D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_2750_V = 0x2E,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_2875_V = 0x2F,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_3000_V = 0x30,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_3125_V = 0x31,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_3250_V = 0x32,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_3375_V = 0x33,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_3500_V = 0x34,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_3625_V = 0x35,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_3750_V = 0x36,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_3875_V = 0x37,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_4000_V = 0x38,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_4125_V = 0x39,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_4250_V = 0x3A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_4375_V = 0x3B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_4500_V = 0x3C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_4625_V = 0x3D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_4750_V = 0x3E,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_4875_V = 0x3F,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_5000_V = 0x40,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_5125_V = 0x41,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_5250_V = 0x42,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_5375_V = 0x43,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_5500_V = 0x44,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_5625_V = 0x45,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_5750_V = 0x46,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_5875_V = 0x47,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_6000_V = 0x48,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_6125_V = 0x49,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_6250_V = 0x4A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_6375_V = 0x4B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_6500_V = 0x4C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_6625_V = 0x4D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_6750_V = 0x4E,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_6875_V = 0x4F,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_7000_V = 0x50,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_7125_V = 0x51,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_7250_V = 0x52,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_7375_V = 0x53,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_7500_V = 0x54,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_7625_V = 0x55,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_7750_V = 0x56,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_7875_V = 0x57,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_8000_V = 0x58,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_8125_V = 0x59,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_8250_V = 0x5A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_8375_V = 0x5B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_8500_V = 0x5C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_8625_V = 0x5D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_POINT_8750_V = 0x5E
} OUTPUT_SUPPLY_LARGE_T ;



//  
//  Defined Varm output voltage from 0.7V to 1.3625V by 12.5mV step and from 1.8 to 1.875 
//  by 12.5 mV step.
//  Register VarmOutputSel1 0x040B, Bits 0:5, typedef output_supply
//   
//  
#define VARM_OUTPUT_SEL_1_MASK 0x3F
#define VARM_OUTPUT_SEL_1_OFFSET 5
#define VARM_OUTPUT_SEL_1_STOP_BIT 5
#define VARM_OUTPUT_SEL_1_START_BIT 0
#define VARM_OUTPUT_SEL_1_WIDTH 6

#define VARM_OUTPUT_SEL_1_REG 0x40B
#define VARM_OUTPUT_SEL_1_PARAM_MASK  0x3F
#define OUTPUT_SUPPLY_PARAM_MASK  0x3F

//  
//  Defined Varm output voltage from 0.7V to 1.3625V by 12.5mV step and from 1.8 to 1.875 
//  by 12.5 mV step.
//  Register VarmOutputSel2 0x040C, Bits 0:5, typedef output_supply
//   
//  
#define VARM_OUTPUT_SEL_2_MASK 0x3F
#define VARM_OUTPUT_SEL_2_OFFSET 5
#define VARM_OUTPUT_SEL_2_STOP_BIT 5
#define VARM_OUTPUT_SEL_2_START_BIT 0
#define VARM_OUTPUT_SEL_2_WIDTH 6

#define VARM_OUTPUT_SEL_2_REG 0x40C
#define VARM_OUTPUT_SEL_2_PARAM_MASK  0x3F

//  
//  Defined Varm output voltage from 0.7V to 1.3625V by 12.5mV step and from 1.8 to 1.875 
//  by 12.5 mV step.
//  Register VarmOutputSel3 0x040D, Bits 0:5, typedef output_supply
//   
//  
#define VARM_OUTPUT_SEL_3_MASK 0x3F
#define VARM_OUTPUT_SEL_3_OFFSET 5
#define VARM_OUTPUT_SEL_3_STOP_BIT 5
#define VARM_OUTPUT_SEL_3_START_BIT 0
#define VARM_OUTPUT_SEL_3_WIDTH 6

#define VARM_OUTPUT_SEL_3_REG 0x40D
#define VARM_OUTPUT_SEL_3_PARAM_MASK  0x3F

//  
//  Defined Vape output voltage from 0.7V to 1.3625V by 12.5mV step and from 1.8 to 1.875 
//  by 12.5 mV step.
//  Register VapeOutputSel1 0x040E, Bits 0:5, typedef output_supply
//   
//  
#define VAPE_OUTPUT_SEL_1_MASK 0x3F
#define VAPE_OUTPUT_SEL_1_OFFSET 5
#define VAPE_OUTPUT_SEL_1_STOP_BIT 5
#define VAPE_OUTPUT_SEL_1_START_BIT 0
#define VAPE_OUTPUT_SEL_1_WIDTH 6

#define VAPE_OUTPUT_SEL_1_REG 0x40E
#define VAPE_OUTPUT_SEL_1_PARAM_MASK  0x3F

//  
//  Defined Vape output voltage from 0.7V to 1.3625V by 12.5mV step and from 1.8 to 1.875 
//  by 12.5 mV step.
//  Register VapeOutputSel2 0x040F, Bits 0:5, typedef output_supply
//   
//  
#define VAPE_OUTPUT_SEL_2_MASK 0x3F
#define VAPE_OUTPUT_SEL_2_OFFSET 5
#define VAPE_OUTPUT_SEL_2_STOP_BIT 5
#define VAPE_OUTPUT_SEL_2_START_BIT 0
#define VAPE_OUTPUT_SEL_2_WIDTH 6

#define VAPE_OUTPUT_SEL_2_REG 0x40F
#define VAPE_OUTPUT_SEL_2_PARAM_MASK  0x3F

//  
//  Defined Vape output voltage from 0.7V to 1.3625V by 12.5mV step and from 1.8 to 1.875 
//  by 12.5 mV step.
//  Register VapeOutputSel3 0x0410, Bits 0:5, typedef output_supply
//   
//  
#define VAPE_OUTPUT_SEL_3_MASK 0x3F
#define VAPE_OUTPUT_SEL_3_OFFSET 5
#define VAPE_OUTPUT_SEL_3_STOP_BIT 5
#define VAPE_OUTPUT_SEL_3_START_BIT 0
#define VAPE_OUTPUT_SEL_3_WIDTH 6

#define VAPE_OUTPUT_SEL_3_REG 0x410
#define VAPE_OUTPUT_SEL_3_PARAM_MASK  0x3F
#define  VBBPFB 0
#define  VBBPFB_01V 1
#define  VBBPFB_02V 2
#define  VBBPFB_03V 3
#define  VBBPFB_04V 4
#define  VBBPFB_MINUS_04V 8
#define  VBBPFB_MINUS_03V 13
#define  VBBPFB_MINUS_02V 14
#define  VBBPFB_MINUS_01V 15



typedef enum positivie_voltage_selection {
    VBBPFB_E,
    VBBPFB_01V_E,
    VBBPFB_02V_E,
    VBBPFB_03V_E,
    VBBPFB_04V_E,
    VBBPFB_MINUS_04V_E,
    VBBPFB_MINUS_03V_E,
    VBBPFB_MINUS_02V_E,
    VBBPFB_MINUS_01V_E
} POSITIVIE_VOLTAGE_SELECTION_T ;



#define  VBBN_0V 0
#define  VBBN_MINUS_01V 1
#define  VBBN_MINUS_02V 2
#define  VBBN_MINUS_03V 3
#define  VBBN_MINUS_04V 4
#define  VBBN_MINUS_0V 8
#define  VBBN_01V 9
#define  VBBN_02V 10
#define  VBBN_03V 11
#define  VBBN_04V 12



typedef enum negative_voltage_selection {
    VBBN_0V_E,
    VBBN_MINUS_01V_E,
    VBBN_MINUS_02V_E,
    VBBN_MINUS_03V_E,
    VBBN_MINUS_04V_E,
    VBBN_MINUS_0V_E,
    VBBN_01V_E,
    VBBN_02V_E,
    VBBN_03V_E,
    VBBN_04V_E
} NEGATIVE_VOLTAGE_SELECTION_T ;




//  
//  VBBP output supply value management.
//  Register VbbOutputSupply1 0x0411, Bits 0:3, typedef positivie_voltage_selection
//   
//  
#define VBB_POSITIVE_SELECTION_1_MASK 0xF
#define VBB_POSITIVE_SELECTION_1_OFFSET 3
#define VBB_POSITIVE_SELECTION_1_STOP_BIT 3
#define VBB_POSITIVE_SELECTION_1_START_BIT 0
#define VBB_POSITIVE_SELECTION_1_WIDTH 4

#define VBB_OUTPUT_SUPPLY_1_REG 0x411
#define VBB_POSITIVE_SELECTION_1_PARAM_MASK  0xF
#define POSITIVIE_VOLTAGE_SELECTION_PARAM_MASK  0xF

//  
//  VBBN output supply value management.
//  Register VbbOutputSupply1 0x0411, Bits 4:7, typedef negative_voltage_selection
//   
//  
#define VBB_NEGATIVE_SELECTION_1_MASK 0xF0
#define VBB_NEGATIVE_SELECTION_1_OFFSET 7
#define VBB_NEGATIVE_SELECTION_1_STOP_BIT 7
#define VBB_NEGATIVE_SELECTION_1_START_BIT 4
#define VBB_NEGATIVE_SELECTION_1_WIDTH 4

#define VBB_NEGATIVE_SELECTION_1_PARAM_MASK  0xF0
#define NEGATIVE_VOLTAGE_SELECTION_PARAM_MASK  0xF0

//  
//  VBBP output supply value management.
//  Register VbbOutputSupply2 0x0412, Bits 0:3, typedef positivie_voltage_selection
//   
//  
#define VBB_POSITIVE_SELECTION_2_MASK 0xF
#define VBB_POSITIVE_SELECTION_2_OFFSET 3
#define VBB_POSITIVE_SELECTION_2_STOP_BIT 3
#define VBB_POSITIVE_SELECTION_2_START_BIT 0
#define VBB_POSITIVE_SELECTION_2_WIDTH 4

#define VBB_OUTPUT_SUPPLY_2_REG 0x412
#define VBB_POSITIVE_SELECTION_2_PARAM_MASK  0xF

//  
//  VBBN output supply value management.
//  Register VbbOutputSupply2 0x0412, Bits 4:7, typedef negative_voltage_selection
//   
//  
#define VBB_NEGATIVE_SELECTION_2_MASK 0xF0
#define VBB_NEGATIVE_SELECTION_2_OFFSET 7
#define VBB_NEGATIVE_SELECTION_2_STOP_BIT 7
#define VBB_NEGATIVE_SELECTION_2_START_BIT 4
#define VBB_NEGATIVE_SELECTION_2_WIDTH 4

#define VBB_NEGATIVE_SELECTION_2_PARAM_MASK  0xF0

//  
//  Defined Vsmps1Sel1 output voltage from 0.7V to 1.3625V by 12.5mV step.
//  Register Vsmps1Sel1Output 0x0413, Bits 0:5, typedef output_supply_med
//   
//  
#define VSMPS_1_SEL_1_OUTPUT_MASK 0x3F
#define VSMPS_1_SEL_1_OUTPUT_OFFSET 5
#define VSMPS_1_SEL_1_OUTPUT_STOP_BIT 5
#define VSMPS_1_SEL_1_OUTPUT_START_BIT 0
#define VSMPS_1_SEL_1_OUTPUT_WIDTH 6

#define VSMPS_1_SEL_1_OUTPUT_REG 0x413
#define VSMPS_1_SEL_1_OUTPUT_PARAM_MASK  0x3F
#define OUTPUT_SUPPLY_MED_PARAM_MASK  0x3F

//  
//  Defined Vsmps1Sel2 output voltage from 0.7V to 1.3625V by 12.5mV step.
//  Register Vsmps1Sel2Output 0x0414, Bits 0:5, typedef output_supply_med
//   
//  
#define VSMPS_1_SEL_2_OUTPUT_MASK 0x3F
#define VSMPS_1_SEL_2_OUTPUT_OFFSET 5
#define VSMPS_1_SEL_2_OUTPUT_STOP_BIT 5
#define VSMPS_1_SEL_2_OUTPUT_START_BIT 0
#define VSMPS_1_SEL_2_OUTPUT_WIDTH 6

#define VSMPS_1_SEL_2_OUTPUT_REG 0x414
#define VSMPS_1_SEL_2_OUTPUT_PARAM_MASK  0x3F

//  
//  Defined Vsmps1Sel3 output voltage from 0.7V to 1.3625V by 12.5mV step.
//  Register Vsmps1Sel3Output 0x0415, Bits 0:5, typedef output_supply_med
//   
//  
#define VSMPS_1_SEL_3_OUTPUT_MASK 0x3F
#define VSMPS_1_SEL_3_OUTPUT_OFFSET 5
#define VSMPS_1_SEL_3_OUTPUT_STOP_BIT 5
#define VSMPS_1_SEL_3_OUTPUT_START_BIT 0
#define VSMPS_1_SEL_3_OUTPUT_WIDTH 6

#define VSMPS_1_SEL_3_OUTPUT_REG 0x415
#define VSMPS_1_SEL_3_OUTPUT_PARAM_MASK  0x3F

//  
//  Defined Vsmps2Sel1 output voltage from 1.8 to 1.875 by 12.5 mV step.
//  Register Vsmps2Sel1Output 0x0417, Bits 0:5, typedef output_supply_large
//   
//  
#define VSMPS_2_SEL_1_OUTPUT_MASK 0x3F
#define VSMPS_2_SEL_1_OUTPUT_OFFSET 5
#define VSMPS_2_SEL_1_OUTPUT_STOP_BIT 5
#define VSMPS_2_SEL_1_OUTPUT_START_BIT 0
#define VSMPS_2_SEL_1_OUTPUT_WIDTH 6

#define VSMPS_2_SEL_1_OUTPUT_REG 0x417
#define VSMPS_2_SEL_1_OUTPUT_PARAM_MASK  0x3F
#define OUTPUT_SUPPLY_LARGE_PARAM_MASK  0x3F

//  
//  Defined Vsmps2Sel2 output voltage from 1.8V to 1.875V by 12.5mV step.
//  Register Vsmps2Sel2Output 0x0418, Bits 0:5, typedef output_supply_large
//   
//  
#define VSMPS_2_SEL_2_OUTPUT_MASK 0x3F
#define VSMPS_2_SEL_2_OUTPUT_OFFSET 5
#define VSMPS_2_SEL_2_OUTPUT_STOP_BIT 5
#define VSMPS_2_SEL_2_OUTPUT_START_BIT 0
#define VSMPS_2_SEL_2_OUTPUT_WIDTH 6

#define VSMPS_2_SEL_2_OUTPUT_REG 0x418
#define VSMPS_2_SEL_2_OUTPUT_PARAM_MASK  0x3F

//  
//  Defined Vsmps2Sel3 output voltage from 1.8 to 1.875 by 12.5 mV step.
//  Register Vsmps2Sel3Output 0x0419, Bits 0:5, typedef output_supply_large
//   
//  
#define VSMPS_2_SEL_3_OUTPUT_MASK 0x3F
#define VSMPS_2_SEL_3_OUTPUT_OFFSET 5
#define VSMPS_2_SEL_3_OUTPUT_STOP_BIT 5
#define VSMPS_2_SEL_3_OUTPUT_START_BIT 0
#define VSMPS_2_SEL_3_OUTPUT_WIDTH 6

#define VSMPS_2_SEL_3_OUTPUT_REG 0x419
#define VSMPS_2_SEL_3_OUTPUT_PARAM_MASK  0x3F
typedef enum output_supply3 {
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_70_V = 0x0,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_71_V = 0x1,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_72_V = 0x2,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_74_V = 0x3,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_75_V = 0x4,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_76_V = 0x5,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_77_V = 0x6,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_79_V = 0x7,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_80_V = 0x8,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_81_V = 0x9,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_82_V = 0xA,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_84_V = 0xB,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_85_V = 0xC,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_86_V = 0xD,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_87_V = 0xE,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_89_V = 0xF,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_90_V = 0x10,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_91_V = 0x11,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_92_V = 0x12,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_94_V = 0x13,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_95_V = 0x14,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_96_V = 0x15,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_97_V = 0x16,
    OUTPUT_VOLTAGE_VSMPS3_0_POINT_99_V = 0x17,
    OUTPUT_VOLTAGE_VSMPS3_1_V = 0x18,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_01_V = 0x19,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_02_V = 0x1A,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_04_V = 0x1B,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_05_V = 0x1C,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_06_V = 0x1D,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_07_V = 0x1E,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_09_V = 0x1F,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_10_V = 0x20,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_11_V = 0x21,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_13_V = 0x22,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_14_V = 0x23,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_15_V = 0x24,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_16_V = 0x25,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_18_V = 0x26,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_19_V = 0x27,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_20_V = 0x28,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_21_V = 0x29,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_23_V = 0x2A,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_24_V = 0x2B,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_25_V = 0x2C,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_26_V = 0x2D,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_28_V = 0x2E,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_29_V = 0x2F,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_30_V = 0x30,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_31_V = 0x31,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_33_V = 0x32,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_34_V = 0x33,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_35_V = 0x34,
    OUTPUT_VOLTAGE_VSMPS3_1_POINT_36_V = 0x35
} OUTPUT_SUPPLY_3_T ;



//  
//  Defined Vsmps3 output voltage from 0.7V to 1.3625 by 12.5mV steps.
//  Register Vsmps3Sel1Output 0x041B, Bits 0:6, typedef output_supply3
//   
//  
#define VSMPS_3_SEL_1_OUTPUT_MASK 0x7F
#define VSMPS_3_SEL_1_OUTPUT_OFFSET 6
#define VSMPS_3_SEL_1_OUTPUT_STOP_BIT 6
#define VSMPS_3_SEL_1_OUTPUT_START_BIT 0
#define VSMPS_3_SEL_1_OUTPUT_WIDTH 7

#define VSMPS_3_SEL_1_OUTPUT_REG 0x41B
#define VSMPS_3_SEL_1_OUTPUT_PARAM_MASK  0x7F
#define OUTPUT_SUPPLY_3_PARAM_MASK  0x7F

//  
//  Defined Vsmps3 output voltage from 0.7V to 1.3625 by 12.5mV steps.
//  Register Vsmps3Sel2Output 0x041C, Bits 0:6, typedef output_supply3
//   
//  
#define VSMPS_3_SEL_2_OUTPUT_MASK 0x7F
#define VSMPS_3_SEL_2_OUTPUT_OFFSET 6
#define VSMPS_3_SEL_2_OUTPUT_STOP_BIT 6
#define VSMPS_3_SEL_2_OUTPUT_START_BIT 0
#define VSMPS_3_SEL_2_OUTPUT_WIDTH 7

#define VSMPS_3_SEL_2_OUTPUT_REG 0x41C
#define VSMPS_3_SEL_2_OUTPUT_PARAM_MASK  0x7F

//  
//  Defined Vsmps3 output voltage from 0.7V to 1.3625 by 12.5mV steps.
//  Register Vsmps3Sel3Output 0x041D, Bits 0:6, typedef output_supply3
//   
//  
#define VSMPS_3_SEL_3_OUTPUT_MASK 0x7F
#define VSMPS_3_SEL_3_OUTPUT_OFFSET 6
#define VSMPS_3_SEL_3_OUTPUT_STOP_BIT 6
#define VSMPS_3_SEL_3_OUTPUT_START_BIT 0
#define VSMPS_3_SEL_3_OUTPUT_WIDTH 7

#define VSMPS_3_SEL_3_OUTPUT_REG 0x41D
#define VSMPS_3_SEL_3_OUTPUT_PARAM_MASK  0x7F
#define  VAUX_1200MV 0
#define  VAUX_1500MV 1
#define  VAUX_1800MV 2
#define  VAUX_2100MV 3
#define  VAUX_2500MV 4
#define  VAUX_2750MV 5
#define  VAUX_2790MV 6
#define  VAUX_2910MV 7



typedef enum vaux_output {
    VAUX_1200MV_E,
    VAUX_1500MV_E,
    VAUX_1800MV_E,
    VAUX_2100MV_E,
    VAUX_2500MV_E,
    VAUX_2750MV_E,
    VAUX_2790MV_E,
    VAUX_2910MV_E
} VAUX_OUTPUT_T ;




//  
//  Vaux1 output supply value management.
//  Register Vaux1Sel1Output 0x041F, Bits 0:3, typedef vaux_output
//   
//  
#define VAUX_1_SEL_OUTPUT_MASK 0xF
#define VAUX_1_SEL_OUTPUT_OFFSET 3
#define VAUX_1_SEL_OUTPUT_STOP_BIT 3
#define VAUX_1_SEL_OUTPUT_START_BIT 0
#define VAUX_1_SEL_OUTPUT_WIDTH 4

#define VAUX_1_SEL_1_OUTPUT_REG 0x41F
#define VAUX_1_SEL_OUTPUT_PARAM_MASK  0xF
#define VAUX_OUTPUT_PARAM_MASK  0xF

//  
//  Vaux2 output supply value management.
//  Register Vaux2Sel1Output 0x0420, Bits 0:3, typedef vaux_output
//   
//  
#define VAUX_2_SEL_OUTPUT_MASK 0xF
#define VAUX_2_SEL_OUTPUT_OFFSET 3
#define VAUX_2_SEL_OUTPUT_STOP_BIT 3
#define VAUX_2_SEL_OUTPUT_START_BIT 0
#define VAUX_2_SEL_OUTPUT_WIDTH 4

#define VAUX_2_SEL_1_OUTPUT_REG 0x420
#define VAUX_2_SEL_OUTPUT_PARAM_MASK  0xF

//  
//  Vaux2 output supply value management.
//  Register Vaux3Sel1Output 0x0421, Bits 0:2, typedef vaux_output
//   
//  
#define VAUX_3_SEL_OUTPUT_MASK 0x7
#define VAUX_3_SEL_OUTPUT_OFFSET 2
#define VAUX_3_SEL_OUTPUT_STOP_BIT 2
#define VAUX_3_SEL_OUTPUT_START_BIT 0
#define VAUX_3_SEL_OUTPUT_WIDTH 3

#define VAUX_3_SEL_1_OUTPUT_REG 0x421
#define VAUX_3_SEL_OUTPUT_PARAM_MASK  0x7

//  
//  Vrf1 output supply value management.
//  Register Vaux3Sel1Output 0x0421, Bits 4:5, typedef vrf1_output
//   
//  
#define VRF_1_SEL_OUTPUT_MASK 0x30
#define VRF_1_SEL_OUTPUT_OFFSET 5
#define VRF_1_SEL_OUTPUT_STOP_BIT 5
#define VRF_1_SEL_OUTPUT_START_BIT 4
#define VRF_1_SEL_OUTPUT_WIDTH 2

#define  VRF1_1800_MV 0
#define  VRF1_2000_MV 1
#define  VRF1_2150_MV 2
#define  VRF1_2500_MV 3
#define VRF_1_OUTPUT_PARAM_MASK  0x30



typedef enum vrf1_output {
    VRF1_1800_MV_E,
    VRF1_2000_MV_E,
    VRF1_2150_MV_E,
    VRF1_2500_MV_E
} VRF_1_OUTPUT_T ;




//  
//  0: inactive 
//  1: Force ExtSupply12LPn = Low when "ExtSup12LPnClkSel[1:0]" = 00 
//  (ExtSup12LPn/ExtSupply12Clk pad functionality = ExtSupply12LPn).
//  Register ExternalSupplyControl 0x0422, Bits 0:0, typedef ext_supply_force_low
//   
//  
#define EXT_SUPPLY_FORCE_LOW_MASK 0x1
#define EXT_SUPPLY_FORCE_LOW_OFFSET 0
#define EXT_SUPPLY_FORCE_LOW_STOP_BIT 0
#define EXT_SUPPLY_FORCE_LOW_START_BIT 0
#define EXT_SUPPLY_FORCE_LOW_WIDTH 1

#define EXTERNAL_SUPPLY_CONTROL_REG 0x422
#define  EXT_SUPPLY_FORCE_LOW 1

typedef enum ext_supply_force_low {
    INNACTIVE_E,
    FORCE_E
} EXT_SUPPLY_FORCE_LOW_T ;
#define EXT_SUPPLY_FORCE_LOW_PARAM_MASK  0x1



//  
//  00: allows to set Vmod in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vmod in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vmod in HP whatever VmodRequest signal value.
//  Register VmodSupplyManagement 0x0440, Bits 6:7, typedef vmod_resquet_control
//   
//  
#define VMOD_RESQUET_CONTROL_MASK 0xC0
#define VMOD_RESQUET_CONTROL_OFFSET 7
#define VMOD_RESQUET_CONTROL_STOP_BIT 7
#define VMOD_RESQUET_CONTROL_START_BIT 6
#define VMOD_RESQUET_CONTROL_WIDTH 2

#define VMOD_SUPPLY_MANAGEMENT_REG 0x440
#define  ALLOWS_TO_SET_HP_LP_MODE 0
#define  ALLOWS_TO_SET_HP_OFF_MODE 1
#define  FORCE_VMOD_IN_HP 2
#define VMOD_RESQUET_CONTROL_PARAM_MASK  0xC0



typedef enum vmod_resquet_control {
    ALLOWS_TO_SET_HP_LP_MODE_E,
    ALLOWS_TO_SET_HP_OFF_MODE_E,
    FORCE_VMOD_IN_HP_E
} VMOD_RESQUET_CONTROL_T ;




//  
//  0: doesn't valid SysClkReq1 input to request system clock 
//  1: Valid SysClkReq1 input to request system clock.
//  Register VmodSupplyManagement 0x0440, Bits 5:5, typedef 
//  modem_system_clock_request_valid
//   
//  
#define MOD_SYS_CLK_REQ_1_VALID_MASK 0x20
#define MOD_SYS_CLK_REQ_1_VALID_OFFSET 5
#define MOD_SYS_CLK_REQ_1_VALID_STOP_BIT 5
#define MOD_SYS_CLK_REQ_1_VALID_START_BIT 5
#define MOD_SYS_CLK_REQ_1_VALID_WIDTH 1

#define  MODEM_SYSTEM_CLOCK_REQUEST_VALID 32

typedef enum modem_system_clock_request_valid {
    DOESNOT_VALID_SYSTEM_CLOCK_E,
    VALID_SYSTEM_CLOCK_E
} MODEM_SYSTEM_CLOCK_REQUEST_VALID_T ;
#define MODEM_SYSTEM_CLOCK_REQUEST_VALID_PARAM_MASK  0x20



//  
//  0: inactive (Vmod can be in PWM or PFM mode) 
//  1: Vmod is forced in PWM mode when output voltage is programm to a lower value 
//  (energy sent to the battery.
//  Register VmodSupplyManagement 0x0440, Bits 4:4, typedef vmod_forced_enable
//   
//  
#define VMOD_PWM_MODE_MASK 0x10
#define VMOD_PWM_MODE_OFFSET 4
#define VMOD_PWM_MODE_STOP_BIT 4
#define VMOD_PWM_MODE_START_BIT 4
#define VMOD_PWM_MODE_WIDTH 1


typedef enum vmod_forced_enable {
    VMOD_FORCED_DISABLE_E,
    VMOD_FORCED_ENABLE_E
} VMOD_FORCED_ENABLE_T ;

#define VMOD_FORCED_ENABLE_PARAM_MASK  0x10

//  
//  0: Vmod is not in AutoMode 
//  1: Vmod is in AutoMode.
//  Register VmodSupplyManagement 0x0440, Bits 3:3, typedef vmod_auto_mode_enable
//   
//  
#define VMOD_AUTO_MODE_MASK 0x8
#define VMOD_AUTO_MODE_OFFSET 3
#define VMOD_AUTO_MODE_STOP_BIT 3
#define VMOD_AUTO_MODE_START_BIT 3
#define VMOD_AUTO_MODE_WIDTH 1


typedef enum vmod_auto_mode_enable {
    VMOD_AUTO_MODE_DISABLE_E,
    VMOD_AUTO_MODE_ENABLE_E
} VMOD_AUTO_MODE_ENABLE_T ;

#define VMOD_AUTO_MODE_ENABLE_PARAM_MASK  0x8

//  
//  0: Vmod output voltage is defined by VmodSel1 register 
//  1: Vmod output voltage is defined by VmodSel2 register.
//  Register VmodSupplyManagement 0x0440, Bits 2:2, typedef vmod_selection
//   
//  
#define VMOD_SELECTION_CONTROL_MASK 0x4
#define VMOD_SELECTION_CONTROL_OFFSET 2
#define VMOD_SELECTION_CONTROL_STOP_BIT 2
#define VMOD_SELECTION_CONTROL_START_BIT 2
#define VMOD_SELECTION_CONTROL_WIDTH 1

#define  VMOD_SELECTION 4

typedef enum vmod_selection {
    VMOD_SELECT1_E,
    VMOD_SELECT2_E
} VMOD_SELECTION_T ;
#define VMOD_SELECTION_PARAM_MASK  0x4



//  
//  00: force Vmod disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vmod signal) to control Vmod supply if 
//  VmodRequestCtrl[1:0]=0x. 
//  Forced in HP mode if VmodRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VmodSupplyManagement 0x0440, Bits 0:1, typedef vmod_regulation
//   
//  
#define VMOD_REGULATION_MASK 0x3
#define VMOD_REGULATION_OFFSET 1
#define VMOD_REGULATION_STOP_BIT 1
#define VMOD_REGULATION_START_BIT 0
#define VMOD_REGULATION_WIDTH 2

#define  FORCE_VMOD_DISABLED 0
#define  FORCE_ENABLED_IN_HP_MODE 1
#define  ALLOWS_HW_CONTROL 2
#define  FORCE_ENABLE_IN_LP_MODE 3
#define VMOD_REGULATION_PARAM_MASK  0x3



typedef enum vmod_regulation {
    FORCE_VMOD_DISABLED_E,
    FORCE_ENABLED_IN_HP_MODE_E,
    ALLOWS_HW_CONTROL_E,
    FORCE_ENABLE_IN_LP_MODE_E
} VMOD_REGULATION_T ;




//  
//  Defined VmodSel1 output voltage from 0.7V to 1.3625V by 12.5mV step and from 1.8 to 
//  1.875 by 12.5 mV step.
//  Register VmodSel1Output 0x0441, Bits 0:5, typedef output_supply
//   
//  
#define VMOD_SEL_1_OUTPUT_MASK 0x3F
#define VMOD_SEL_1_OUTPUT_OFFSET 5
#define VMOD_SEL_1_OUTPUT_STOP_BIT 5
#define VMOD_SEL_1_OUTPUT_START_BIT 0
#define VMOD_SEL_1_OUTPUT_WIDTH 6

#define VMOD_SEL_1_OUTPUT_REG 0x441
#define VMOD_SEL_1_OUTPUT_PARAM_MASK  0x3F

//  
//  Defined VmodSel2 output voltage from 0.7V to 1.3625V by 12.5mV step and from 1.8 to 
//  1.875 by 12.5 mV step.
//  Register VmodSel2Output 0x0442, Bits 0:5, typedef output_supply
//   
//  
#define VMOD_SEL_2_OUTPUT_MASK 0x3F
#define VMOD_SEL_2_OUTPUT_OFFSET 5
#define VMOD_SEL_2_OUTPUT_STOP_BIT 5
#define VMOD_SEL_2_OUTPUT_START_BIT 0
#define VMOD_SEL_2_OUTPUT_WIDTH 6

#define VMOD_SEL_2_OUTPUT_REG 0x442
#define VMOD_SEL_2_OUTPUT_PARAM_MASK  0x3F

typedef enum supply_discharge_time {
    LONG_DISCHARGE_TIME_E,
    SHORT_DISCHARGE_TIME_E
} SUPPLY_DISCHARGE_TIME_T ;



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 7:7, typedef supply_discharge_time
//   
//  
#define VAUDIO_DISCHARGE_TIME_MASK 0x80
#define VAUDIO_DISCHARGE_TIME_OFFSET 7
#define VAUDIO_DISCHARGE_TIME_STOP_BIT 7
#define VAUDIO_DISCHARGE_TIME_START_BIT 7
#define VAUDIO_DISCHARGE_TIME_WIDTH 1

#define SUPPLY_DISCHARGE_TIME_REG 0x443
#define VAUDIO_DISCHARGE_TIME_PARAM_MASK  0x80
#define SUPPLY_DISCHARGE_TIME_PARAM_MASK  0x80

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 6:6, typedef supply_discharge_time
//   
//  
#define VTV_OUT_DISCHARGE_TIME_MASK 0x40
#define VTV_OUT_DISCHARGE_TIME_OFFSET 6
#define VTV_OUT_DISCHARGE_TIME_STOP_BIT 6
#define VTV_OUT_DISCHARGE_TIME_START_BIT 6
#define VTV_OUT_DISCHARGE_TIME_WIDTH 1

#define VTV_OUT_DISCHARGE_TIME_PARAM_MASK  0x40

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 5:5, typedef supply_discharge_time
//   
//  
#define VINT_CORE_12_DISCHARGE_TIME_MASK 0x20
#define VINT_CORE_12_DISCHARGE_TIME_OFFSET 5
#define VINT_CORE_12_DISCHARGE_TIME_STOP_BIT 5
#define VINT_CORE_12_DISCHARGE_TIME_START_BIT 5
#define VINT_CORE_12_DISCHARGE_TIME_WIDTH 1

#define VINT_CORE_12_DISCHARGE_TIME_PARAM_MASK  0x20

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 4:4, typedef supply_discharge_time
//   
//  
#define VAUX_3_DISCHARGE_TIME_MASK 0x10
#define VAUX_3_DISCHARGE_TIME_OFFSET 4
#define VAUX_3_DISCHARGE_TIME_STOP_BIT 4
#define VAUX_3_DISCHARGE_TIME_START_BIT 4
#define VAUX_3_DISCHARGE_TIME_WIDTH 1

#define VAUX_3_DISCHARGE_TIME_PARAM_MASK  0x10

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 3:3, typedef supply_discharge_time
//   
//  
#define VAUX_2_DISCHARGE_TIME_MASK 0x8
#define VAUX_2_DISCHARGE_TIME_OFFSET 3
#define VAUX_2_DISCHARGE_TIME_STOP_BIT 3
#define VAUX_2_DISCHARGE_TIME_START_BIT 3
#define VAUX_2_DISCHARGE_TIME_WIDTH 1

#define VAUX_2_DISCHARGE_TIME_PARAM_MASK  0x8

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 2:2, typedef supply_discharge_time
//   
//  
#define VAUX_1_DISCHARGE_TIME_MASK 0x4
#define VAUX_1_DISCHARGE_TIME_OFFSET 2
#define VAUX_1_DISCHARGE_TIME_STOP_BIT 2
#define VAUX_1_DISCHARGE_TIME_START_BIT 2
#define VAUX_1_DISCHARGE_TIME_WIDTH 1

#define VAUX_1_DISCHARGE_TIME_PARAM_MASK  0x4

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 1:1, typedef supply_discharge_time
//   
//  
#define VRF_1_DISCHARGE_TIME_MASK 0x2
#define VRF_1_DISCHARGE_TIME_OFFSET 1
#define VRF_1_DISCHARGE_TIME_STOP_BIT 1
#define VRF_1_DISCHARGE_TIME_START_BIT 1
#define VRF_1_DISCHARGE_TIME_WIDTH 1

#define VRF_1_DISCHARGE_TIME_PARAM_MASK  0x2

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 0:0, typedef supply_discharge_time
//   
//  
#define VPLL_DISCHARGE_TIME_MASK 0x1
#define VPLL_DISCHARGE_TIME_OFFSET 0
#define VPLL_DISCHARGE_TIME_STOP_BIT 0
#define VPLL_DISCHARGE_TIME_START_BIT 0
#define VPLL_DISCHARGE_TIME_WIDTH 1

#define VPLL_DISCHARGE_TIME_PARAM_MASK  0x1

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime2 0x0444, Bits 4:4, typedef supply_discharge_time
//   
//  
#define VDMIC_DISCHARGE_TIME_MASK 0x10
#define VDMIC_DISCHARGE_TIME_OFFSET 4
#define VDMIC_DISCHARGE_TIME_STOP_BIT 4
#define VDMIC_DISCHARGE_TIME_START_BIT 4
#define VDMIC_DISCHARGE_TIME_WIDTH 1

#define SUPPLY_DISCHARGE_TIME_2_REG 0x444
#define VDMIC_DISCHARGE_TIME_PARAM_MASK  0x10

//  
//  0: When Vpll is disabled, pull down is disabled 
//  1: When Vpll is disabled, pull down is enabled.
//  Register SupplyDischargeTime2 0x0444, Bits 3:3, typedef vpll_pull_down_enable
//   
//  
#define VPLL_PULL_DOWN_ENABLE_MASK 0x8
#define VPLL_PULL_DOWN_ENABLE_OFFSET 3
#define VPLL_PULL_DOWN_ENABLE_STOP_BIT 3
#define VPLL_PULL_DOWN_ENABLE_START_BIT 3
#define VPLL_PULL_DOWN_ENABLE_WIDTH 1


typedef enum vpll_pull_down_enable {
    VPLL_PULL_DOWN_DISABLE_E,
    VPLL_PULL_DOWN_ENABLE_E
} VPLL_PULL_DOWN_ENABLE_T ;

#define VPLL_PULL_DOWN_ENABLE_PARAM_MASK  0x8

//  
//  0: When Vdmic is disabled, pull down is disabled 
//  1: When Vdmic is disabled, pull down is enabled.
//  Register SupplyDischargeTime2 0x0444, Bits 2:2, typedef vdmic_pull_down_enable
//   
//  
#define VDMIC_PULL_DOWN_ENABLE_MASK 0x4
#define VDMIC_PULL_DOWN_ENABLE_OFFSET 2
#define VDMIC_PULL_DOWN_ENABLE_STOP_BIT 2
#define VDMIC_PULL_DOWN_ENABLE_START_BIT 2
#define VDMIC_PULL_DOWN_ENABLE_WIDTH 1


typedef enum vdmic_pull_down_enable {
    VDMIC_PULL_DOWN_DISABLE_E,
    VDMIC_PULL_DOWN_ENABLE_E
} VDMIC_PULL_DOWN_ENABLE_T ;

#define VDMIC_PULL_DOWN_ENABLE_PARAM_MASK  0x4

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime2 0x0444, Bits 1:1, typedef supply_discharge_time
//   
//  
#define VANA_DISCHARGE_TIME_MASK 0x2
#define VANA_DISCHARGE_TIME_OFFSET 1
#define VANA_DISCHARGE_TIME_STOP_BIT 1
#define VANA_DISCHARGE_TIME_START_BIT 1
#define VANA_DISCHARGE_TIME_WIDTH 1

#define VANA_DISCHARGE_TIME_PARAM_MASK  0x2

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime2 0x0444, Bits 0:0, typedef supply_discharge_time
//   
//  
#define VSIM_DISCHARGE_TIME_MASK 0x1
#define VSIM_DISCHARGE_TIME_OFFSET 0
#define VSIM_DISCHARGE_TIME_STOP_BIT 0
#define VSIM_DISCHARGE_TIME_START_BIT 0
#define VSIM_DISCHARGE_TIME_WIDTH 1

#define VSIM_DISCHARGE_TIME_PARAM_MASK  0x1
#endif
