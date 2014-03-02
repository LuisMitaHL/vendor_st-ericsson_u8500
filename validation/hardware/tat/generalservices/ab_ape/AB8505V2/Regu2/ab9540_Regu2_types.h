/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Regu2/Linux/ab9540_Regu2_types.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_REGU2_LINUX_AB9540_REGU2_TYPES_H
#define C__LISP_PRODUCTION_AB9540_AB9540_REGU2_LINUX_AB9540_REGU2_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


//  
//  00: force Varm disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VarmRequest signal) to control Varm supply if 
//  VarmRequestCtrl[1:0]=0x. Forced in HP mode if VarmRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register ArmVoltageManagement 0x0400, Bits 0:1, typedef regulation
//   
//  
#define ARM_OUPUT_REGULATION_MASK 0x3U
#define ARM_OUPUT_REGULATION_OFFSET 1
#define ARM_OUPUT_REGULATION_STOP_BIT 1
#define ARM_OUPUT_REGULATION_START_BIT 0
#define ARM_OUPUT_REGULATION_WIDTH 2

#define ARM_VOLTAGE_MANAGEMENT_REG 0x400
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
#define ARM_OUPUT_REGULATION_PARAM_MASK  0x3



//  
//  00: Varm output voltage is defined by VarmSel1 register 
//  01: Varm output voltage is defined by VarmSel2 register 
//  1x: Varm output voltage is defined by VarmSel3 register.
//  Register ArmVoltageManagement 0x0400, Bits 2:3, typedef selection
//   
//  
#define ARM_OUPUT_VOLAGE_SELECT_MASK 0xCU
#define ARM_OUPUT_VOLAGE_SELECT_OFFSET 3
#define ARM_OUPUT_VOLAGE_SELECT_STOP_BIT 3
#define ARM_OUPUT_VOLAGE_SELECT_START_BIT 2
#define ARM_OUPUT_VOLAGE_SELECT_WIDTH 2

#define  SEL1_REGISTER 0
#define  SEL2_REGISTER 1
#define  SEL3_REGISTER 2

typedef enum selection {
    SEL1_REGISTER_E,
    SEL2_REGISTER_E,
    SEL3_REGISTER_E
} SELECTION_T ;
#define ARM_OUPUT_VOLAGE_SELECT_PARAM_MASK  0xC



//  
//  0: VBBP output voltage is defined by VBBNSel1 register 
//  1: VBBP output voltage is defined by VBBNSel2 register.
//  Register ArmVoltageManagement 0x0400, Bits 4:4, typedef selection1
//   
//  
#define VBBP_SELECTION_CONTROL_MASK 0x10U
#define VBBP_SELECTION_CONTROL_OFFSET 4
#define VBBP_SELECTION_CONTROL_STOP_BIT 4
#define VBBP_SELECTION_CONTROL_START_BIT 4
#define VBBP_SELECTION_CONTROL_WIDTH 1


typedef enum selection1 {
    VBBNSEL1_REGISTER_E,
    VBBNSEL2_REGISTER_E
} SELECTION_1_T ;
#define VBBP_SELECTION_CONTROL_PARAM_MASK  0x10



//  
//  0: VBBN output voltage is defined by VBBNSel1 register 
//  1: VBBN output voltage is defined by VBBNSel2 register.
//  Register ArmVoltageManagement 0x0400, Bits 5:5, typedef selection1
//   
//  
#define VBBN_SELECTION_CONTROL_MASK 0x20U
#define VBBN_SELECTION_CONTROL_OFFSET 5
#define VBBN_SELECTION_CONTROL_STOP_BIT 5
#define VBBN_SELECTION_CONTROL_START_BIT 5
#define VBBN_SELECTION_CONTROL_WIDTH 1


#define VBBN_SELECTION_CONTROL_PARAM_MASK  0x20



//  
//  0: Varm is not in AutoMode 
//  1: Varm is in AutoMode.
//  Register ArmVoltageManagement 0x0400, Bits 6:6, typedef auto_mode_enable
//   
//  
#define ARM_VOLTAGE_AUTO_MODE_MASK 0x40U
#define ARM_VOLTAGE_AUTO_MODE_OFFSET 6
#define ARM_VOLTAGE_AUTO_MODE_STOP_BIT 6
#define ARM_VOLTAGE_AUTO_MODE_START_BIT 6
#define ARM_VOLTAGE_AUTO_MODE_WIDTH 1


typedef enum auto_mode_enable {
    AUTO_MODE_DISABLE_E,
    AUTO_MODE_ENABLE_E
} AUTO_MODE_ENABLE_T ;
#define ARM_VOLTAGE_AUTO_MODE_PARAM_MASK  0x40



//  
//  0: inactive (Varm can be in PWM or PFM mode) 
//  1: Varm is forced in PWM mode.
//  Register ArmVoltageManagement 0x0400, Bits 7:7, typedef forced
//   
//  
#define ARM_VOLTAGE_PWM_MODE_MASK 0x80U
#define ARM_VOLTAGE_PWM_MODE_OFFSET 7
#define ARM_VOLTAGE_PWM_MODE_STOP_BIT 7
#define ARM_VOLTAGE_PWM_MODE_START_BIT 7
#define ARM_VOLTAGE_PWM_MODE_WIDTH 1


typedef enum forced {
    INACTIVE_E,
    FORCED_E
} FORCED_T ;
#define ARM_VOLTAGE_PWM_MODE_PARAM_MASK  0x80



//  
//  00: force Vbbp disabled (switch between Varm and ground closed) 
//  01: force enabled in HP mode 
//  10: allows HW (internal VbbpRequest signal) to control Vbbp supply if 
//  VbbpRequestCtrl[1:0]=0x. Forced in HP mode if VbbpRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register ArmVoltageBackManagement 0x0401, Bits 0:1, typedef regulation
//   
//  
#define VBBP_REGULATION_MASK 0x3U
#define VBBP_REGULATION_OFFSET 1
#define VBBP_REGULATION_STOP_BIT 1
#define VBBP_REGULATION_START_BIT 0
#define VBBP_REGULATION_WIDTH 2

#define ARM_VOLTAGE_BACK_MANAGEMENT_REG 0x401
#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VBBP_REGULATION_PARAM_MASK  0x3



//  
//  00: force Vbbn disabled (switch between VBBN and ground closed) 
//  01: force enabled in HP mode 
//  10: allows HW (internal VbbnRequest signal) to control Vbbn supply if 
//  VbbnRequestCtrl[1:0]=0x. Forced in HP mode if VbbnRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register ArmVoltageBackManagement 0x0401, Bits 2:3, typedef regulation
//   
//  
#define VBBN_REGULATION_MASK 0xCU
#define VBBN_REGULATION_OFFSET 3
#define VBBN_REGULATION_STOP_BIT 3
#define VBBN_REGULATION_START_BIT 2
#define VBBN_REGULATION_WIDTH 2

#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VBBN_REGULATION_PARAM_MASK  0xC



//  
//  0: Vaux3 output voltage is fixed by 'Vaux3Sel[2:0]' bits (VRF1Vaux3Sel register) 
//  1: Vaux3 output voltage is equal to 3.05V (if Vaux3 previously enabled).
//  Register ArmVoltageBackManagement 0x0401, Bits 4:4, typedef vaux3_output_voltage_on
//   
//  
#define VAUX_3_OUTPUT_VOLTAGE_MASK 0x10U
#define VAUX_3_OUTPUT_VOLTAGE_OFFSET 4
#define VAUX_3_OUTPUT_VOLTAGE_STOP_BIT 4
#define VAUX_3_OUTPUT_VOLTAGE_START_BIT 4
#define VAUX_3_OUTPUT_VOLTAGE_WIDTH 1


typedef enum vaux3_output_voltage_on {
    VAUX_3_OUTPUT_VOLTAGE_OFF_E,
    VAUX_3_OUTPUT_VOLTAGE_ON_E
} VAUX_3_OUTPUT_VOLTAGE_ON_T ;
#define VAUX_3_OUTPUT_VOLTAGE_PARAM_MASK  0x10



//  
//  0: disable pull-down (switch) between VBBN and gnd 
//  1: enable pull-down (switch) between VBBN and gnd].
//  Register ArmVoltageBackManagement 0x0401, Bits 5:5, typedef vbbp_pulldonw_enable
//   
//  
#define VBBP_PULLDONW_MASK 0x20U
#define VBBP_PULLDONW_OFFSET 5
#define VBBP_PULLDONW_STOP_BIT 5
#define VBBP_PULLDONW_START_BIT 5
#define VBBP_PULLDONW_WIDTH 1


typedef enum vbbp_pulldonw_enable {
    VBBP_PULLDONW_DISABLE_E,
    VBBP_PULLDONW_ENABLE_E
} VBBP_PULLDONW_ENABLE_T ;
#define VBBP_PULLDONW_PARAM_MASK  0x20



//  
//  1: inactive 
//  0: Force no voltage step between Vbbp and Varm whatever is VbbpSelx[3:0].
//  Register ArmVoltageBackManagement 0x0401, Bits 6:6, typedef force_no_step
//   
//  
#define FORCE_NO_STEP_MASK 0x40U
#define FORCE_NO_STEP_OFFSET 6
#define FORCE_NO_STEP_STOP_BIT 6
#define FORCE_NO_STEP_START_BIT 6
#define FORCE_NO_STEP_WIDTH 1


typedef enum force_no_step {
    FORCE_NO_VOLTAGE_FORCED_E,
    FORCE_NO_VOLTAGE_INACTIVE_E
} FORCE_NO_STEP_T ;
#define FORCE_NO_STEP_PARAM_MASK  0x40



//  
//  00: force Vape disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VapeRequest signal) to control Vape supply if 
//  VapeRequestCtrl[1:0]=0x. Forced in HP mode if VapeRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VapeVoltageManagement 0x0402, Bits 0:1, typedef regulation
//   
//  
#define VAPE_REGULATION_MASK 0x3U
#define VAPE_REGULATION_OFFSET 1
#define VAPE_REGULATION_STOP_BIT 1
#define VAPE_REGULATION_START_BIT 0
#define VAPE_REGULATION_WIDTH 2

#define VAPE_VOLTAGE_MANAGEMENT_REG 0x402
#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VAPE_REGULATION_PARAM_MASK  0x3



//  
//  0: Vape output voltage is defined by VapeSel1 register 
//  1: Vape output voltage is defined by VapeSel2 register.
//  Register VapeVoltageManagement 0x0402, Bits 2:2, typedef ape_selection1
//   
//  
#define VAPE_SELECTION_CONTROL_1_MASK 0x4U
#define VAPE_SELECTION_CONTROL_1_OFFSET 2
#define VAPE_SELECTION_CONTROL_1_STOP_BIT 2
#define VAPE_SELECTION_CONTROL_1_START_BIT 2
#define VAPE_SELECTION_CONTROL_1_WIDTH 1


typedef enum ape_selection1 {
    V_APE_SEL1_REGISTER_E,
    V_APE_SEL2_REGISTER_E
} APE_SELECTION_1_T ;
#define VAPE_SELECTION_CONTROL_1_PARAM_MASK  0x4



//  
//  0: Vape is not in AutoMode 
//  1: Vape is in AutoMode.
//  Register VapeVoltageManagement 0x0402, Bits 3:3, typedef auto_mode
//   
//  
#define VAPE_AUTO_MODE_MASK 0x8U
#define VAPE_AUTO_MODE_OFFSET 3
#define VAPE_AUTO_MODE_STOP_BIT 3
#define VAPE_AUTO_MODE_START_BIT 3
#define VAPE_AUTO_MODE_WIDTH 1


typedef enum auto_mode {
    AUTO_MODE_NOT_ACTIVE_E,
    AUTO_MODE_ACTIVE_E
} AUTO_MODE_T ;
#define VAPE_AUTO_MODE_PARAM_MASK  0x8



//  
//  0: inactive (Vape can be in PWM or PFM mode) 
//  1: Vape is forced in PWM mode.
//  Register VapeVoltageManagement 0x0402, Bits 4:4, typedef forced
//   
//  
#define VAPE_PWM_MODE_FORCED_MASK 0x10U
#define VAPE_PWM_MODE_FORCED_OFFSET 4
#define VAPE_PWM_MODE_FORCED_STOP_BIT 4
#define VAPE_PWM_MODE_FORCED_START_BIT 4
#define VAPE_PWM_MODE_FORCED_WIDTH 1


#define VAPE_PWM_MODE_FORCED_PARAM_MASK  0x10



//  
//  0: 'VapeSelCtrl1' bit select Vape output voltage 
//  1: VapeSel3 register selects Vape output voltage.
//  Register VapeVoltageManagement 0x0402, Bits 5:5, typedef ape_selection2
//   
//  
#define VAPE_SELECTION_CONTROL_2_MASK 0x20U
#define VAPE_SELECTION_CONTROL_2_OFFSET 5
#define VAPE_SELECTION_CONTROL_2_STOP_BIT 5
#define VAPE_SELECTION_CONTROL_2_START_BIT 5
#define VAPE_SELECTION_CONTROL_2_WIDTH 1


typedef enum ape_selection2 {
    VAPE_SELECTED_BY_CTRL1_E,
    VAPE_SELECTED_BY_SEL3_E
} APE_SELECTION_2_T ;
#define VAPE_SELECTION_CONTROL_2_PARAM_MASK  0x20



//  
//  00: force Vsmps1 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vsmps1Request signal) to control Vsmps1 supply if 
//  Vsmps1RequestCtrl[1:0]=0x. Forced in HP mode if Vsmps1RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vsmps1VoltageManagement 0x0403, Bits 0:1, typedef regulation
//   
//  
#define VSMPS_1_REGULATION_MASK 0x3U
#define VSMPS_1_REGULATION_OFFSET 1
#define VSMPS_1_REGULATION_STOP_BIT 1
#define VSMPS_1_REGULATION_START_BIT 0
#define VSMPS_1_REGULATION_WIDTH 2

#define VSMPS_1_VOLTAGE_MANAGEMENT_REG 0x403
#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VSMPS_1_REGULATION_PARAM_MASK  0x3



//  
//  00: Vsmps1 output voltage is defined by Vsmps1Sel1 register 
//  01: Vsmps1 output voltage is defined by Vsmps1Sel2 register 
//  1x: Vsmps1 output voltage is defined by Vsmps1Sel3 register.
//  Register Vsmps1VoltageManagement 0x0403, Bits 2:3, typedef selection
//   
//  
#define VSMPS_1_OUPUT_VOLAGE_SELECT_MASK 0xCU
#define VSMPS_1_OUPUT_VOLAGE_SELECT_OFFSET 3
#define VSMPS_1_OUPUT_VOLAGE_SELECT_STOP_BIT 3
#define VSMPS_1_OUPUT_VOLAGE_SELECT_START_BIT 2
#define VSMPS_1_OUPUT_VOLAGE_SELECT_WIDTH 2

#define  SEL1_REGISTER 0
#define  SEL2_REGISTER 1
#define  SEL3_REGISTER 2

#define VSMPS_1_OUPUT_VOLAGE_SELECT_PARAM_MASK  0xC



//  
//  0: Vape is not in AutoMode 
//  1: Vape is in AutoMode.
//  Register Vsmps1VoltageManagement 0x0403, Bits 4:4, typedef auto_mode
//   
//  
#define VSMPS_1_AUTO_MODE_MASK 0x10U
#define VSMPS_1_AUTO_MODE_OFFSET 4
#define VSMPS_1_AUTO_MODE_STOP_BIT 4
#define VSMPS_1_AUTO_MODE_START_BIT 4
#define VSMPS_1_AUTO_MODE_WIDTH 1


#define VSMPS_1_AUTO_MODE_PARAM_MASK  0x10



//  
//  0: inactive (Vsmps1 can be in PWM or PFM mode) 
//  1: Vsmps1 is forced in PWM mode.
//  Register Vsmps1VoltageManagement 0x0403, Bits 5:5, typedef forced
//   
//  
#define VSMPS_1_VOLTAGE_PWM_MODE_MASK 0x20U
#define VSMPS_1_VOLTAGE_PWM_MODE_OFFSET 5
#define VSMPS_1_VOLTAGE_PWM_MODE_STOP_BIT 5
#define VSMPS_1_VOLTAGE_PWM_MODE_START_BIT 5
#define VSMPS_1_VOLTAGE_PWM_MODE_WIDTH 1


#define VSMPS_1_VOLTAGE_PWM_MODE_PARAM_MASK  0x20



//  
//  00: force Vsmps2 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vsmps2Request signal) to control Vsmps2 supply if 
//  Vsmps2RequestCtrl[1:0]=0x. Forced in HP mode if Vsmps2RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vsmps2VoltageManagement 0x0404, Bits 0:1, typedef regulation
//   
//  
#define VSMPS_2_REGULATION_MASK 0x3U
#define VSMPS_2_REGULATION_OFFSET 1
#define VSMPS_2_REGULATION_STOP_BIT 1
#define VSMPS_2_REGULATION_START_BIT 0
#define VSMPS_2_REGULATION_WIDTH 2

#define VSMPS_2_VOLTAGE_MANAGEMENT_REG 0x404
#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VSMPS_2_REGULATION_PARAM_MASK  0x3



//  
//  00: Vsmps2 output voltage is defined by Vsmps2Sel1 register 
//  01: Vsmps2 output voltage is defined by Vsmps2Sel2 register 
//  1x: Vsmps2 output voltage is defined by Vsmps2Sel3 register.
//  Register Vsmps2VoltageManagement 0x0404, Bits 2:3, typedef selection
//   
//  
#define VSMPS_2_OUPUT_VOLAGE_SELECT_MASK 0xCU
#define VSMPS_2_OUPUT_VOLAGE_SELECT_OFFSET 3
#define VSMPS_2_OUPUT_VOLAGE_SELECT_STOP_BIT 3
#define VSMPS_2_OUPUT_VOLAGE_SELECT_START_BIT 2
#define VSMPS_2_OUPUT_VOLAGE_SELECT_WIDTH 2

#define  SEL1_REGISTER 0
#define  SEL2_REGISTER 1
#define  SEL3_REGISTER 2

#define VSMPS_2_OUPUT_VOLAGE_SELECT_PARAM_MASK  0xC



//  
//  0: Vape is not in AutoMode 
//  1: Vape is in AutoMode.
//  Register Vsmps2VoltageManagement 0x0404, Bits 4:4, typedef auto_mode
//   
//  
#define VSMPS_2_AUTO_MODE_MASK 0x10U
#define VSMPS_2_AUTO_MODE_OFFSET 4
#define VSMPS_2_AUTO_MODE_STOP_BIT 4
#define VSMPS_2_AUTO_MODE_START_BIT 4
#define VSMPS_2_AUTO_MODE_WIDTH 1


#define VSMPS_2_AUTO_MODE_PARAM_MASK  0x10



//  
//  0: inactive (Vsmps2 can be in PWM or PFM mode) 
//  1: Vsmps2 is forced in PWM mode.
//  Register Vsmps2VoltageManagement 0x0404, Bits 5:5, typedef forced
//   
//  
#define VSMPS_2_VOLTAGE_PWM_MODE_MASK 0x20U
#define VSMPS_2_VOLTAGE_PWM_MODE_OFFSET 5
#define VSMPS_2_VOLTAGE_PWM_MODE_STOP_BIT 5
#define VSMPS_2_VOLTAGE_PWM_MODE_START_BIT 5
#define VSMPS_2_VOLTAGE_PWM_MODE_WIDTH 1


#define VSMPS_2_VOLTAGE_PWM_MODE_PARAM_MASK  0x20



//  
//  00: force Vsmps3 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vsmps3Request signal) to control Vsmps3 supply if 
//  Vsmps3RequestCtrl[1:0]=0x. Forced in HP mode if Vsmps3RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vsmps3VoltageManagement 0x0405, Bits 0:1, typedef regulation
//   
//  
#define VSMPS_3_REGULATION_MASK 0x3U
#define VSMPS_3_REGULATION_OFFSET 1
#define VSMPS_3_REGULATION_STOP_BIT 1
#define VSMPS_3_REGULATION_START_BIT 0
#define VSMPS_3_REGULATION_WIDTH 2

#define VSMPS_3_VOLTAGE_MANAGEMENT_REG 0x405
#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VSMPS_3_REGULATION_PARAM_MASK  0x3



//  
//  00: Vsmps3 output voltage is defined by Vsmps3Sel1 register 
//  01: Vsmps3 output voltage is defined by Vsmps3Sel2 register 
//  1x: Vsmps3 output voltage is defined by Vsmps3Sel3 register.
//  Register Vsmps3VoltageManagement 0x0405, Bits 2:3, typedef selection
//   
//  
#define VSMPS_3_OUPUT_VOLAGE_SELECT_MASK 0xCU
#define VSMPS_3_OUPUT_VOLAGE_SELECT_OFFSET 3
#define VSMPS_3_OUPUT_VOLAGE_SELECT_STOP_BIT 3
#define VSMPS_3_OUPUT_VOLAGE_SELECT_START_BIT 2
#define VSMPS_3_OUPUT_VOLAGE_SELECT_WIDTH 2

#define  SEL1_REGISTER 0
#define  SEL2_REGISTER 1
#define  SEL3_REGISTER 2

#define VSMPS_3_OUPUT_VOLAGE_SELECT_PARAM_MASK  0xC



//  
//  0: Vape is not in AutoMode 
//  1: Vape is in AutoMode.
//  Register Vsmps3VoltageManagement 0x0405, Bits 4:4, typedef auto_mode
//   
//  
#define VSMPS_3_AUTO_MODE_MASK 0x10U
#define VSMPS_3_AUTO_MODE_OFFSET 4
#define VSMPS_3_AUTO_MODE_STOP_BIT 4
#define VSMPS_3_AUTO_MODE_START_BIT 4
#define VSMPS_3_AUTO_MODE_WIDTH 1


#define VSMPS_3_AUTO_MODE_PARAM_MASK  0x10



//  
//  0: inactive (Vsmps3 can be in PWM or PFM mode) 
//  1: Vsmps3 is forced in PWM mode.
//  Register Vsmps3VoltageManagement 0x0405, Bits 5:5, typedef forced
//   
//  
#define VSMPS_3_VOLTAGE_PWM_MODE_MASK 0x20U
#define VSMPS_3_VOLTAGE_PWM_MODE_OFFSET 5
#define VSMPS_3_VOLTAGE_PWM_MODE_STOP_BIT 5
#define VSMPS_3_VOLTAGE_PWM_MODE_START_BIT 5
#define VSMPS_3_VOLTAGE_PWM_MODE_WIDTH 1


#define VSMPS_3_VOLTAGE_PWM_MODE_PARAM_MASK  0x20



//  
//  00: force Vpll disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VpllRequest signal) to control Vpll supply if 
//  VpllRequestCtrl[1:0]=0x. Forced in HP mode if VpllRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VanaVoltageManagement 0x0406, Bits 0:1, typedef regulation
//   
//  
#define VPLL_REGULATION_MASK 0x3U
#define VPLL_REGULATION_OFFSET 1
#define VPLL_REGULATION_STOP_BIT 1
#define VPLL_REGULATION_START_BIT 0
#define VPLL_REGULATION_WIDTH 2

#define VANA_VOLTAGE_MANAGEMENT_REG 0x406
#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VPLL_REGULATION_PARAM_MASK  0x3



//  
//  00: force Vana disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VanaRequest signal) to control Vana supply if 
//  VanaRequestCtrl[1:0]=0x. Forced in HP mode if VanaRequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VanaVoltageManagement 0x0406, Bits 2:3, typedef regulation
//   
//  
#define VANA_REGULATION_MASK 0xCU
#define VANA_REGULATION_OFFSET 3
#define VANA_REGULATION_STOP_BIT 3
#define VANA_REGULATION_START_BIT 2
#define VANA_REGULATION_WIDTH 2

#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VANA_REGULATION_PARAM_MASK  0xC



//  
//  0: disable VrefDDR (output grounded) 
//  1: enable VrefDDR.
//  Register VrefDdrVoltageManagement 0x0407, Bits 0:0, typedef vref_ddr_enable
//   
//  
#define VREF_DDR_MASK 0x1U
#define VREF_DDR_OFFSET 0
#define VREF_DDR_STOP_BIT 0
#define VREF_DDR_START_BIT 0
#define VREF_DDR_WIDTH 1

#define VREF_DDR_VOLTAGE_MANAGEMENT_REG 0x407

typedef enum vref_ddr_enable {
    VREF_DDR_DISABLE_E,
    VREF_DDR_ENABLE_E
} VREF_DDR_ENABLE_T ;
#define VREF_DDR_PARAM_MASK  0x1



//  
//  0: inactive 
//  1: enable 50KOhms pulldown.
//  Register VrefDdrVoltageManagement 0x0407, Bits 1:1, typedef sleep_mode
//   
//  
#define VREF_DDR_SLEEP_MODE_MASK 0x2U
#define VREF_DDR_SLEEP_MODE_OFFSET 1
#define VREF_DDR_SLEEP_MODE_STOP_BIT 1
#define VREF_DDR_SLEEP_MODE_START_BIT 1
#define VREF_DDR_SLEEP_MODE_WIDTH 1


typedef enum sleep_mode {
    SLEEP_MODE_INACTIVE_E,
    SLEEP_MODE_ENABLE_E
} SLEEP_MODE_T ;
#define VREF_DDR_SLEEP_MODE_PARAM_MASK  0x2



//  
//  00: force VextSuppply1 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VextSuppply1Request signal) to control VextSuppply1 supply if 
//  VextSuppply1RequestCtrl[1:0]=0x. Forced in HP mode if VextSuppply1RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 0:1, typedef regulation
//   
//  
#define VEXT_SUPPPLY_1_REGULATION_MASK 0x3U
#define VEXT_SUPPPLY_1_REGULATION_OFFSET 1
#define VEXT_SUPPPLY_1_REGULATION_STOP_BIT 1
#define VEXT_SUPPPLY_1_REGULATION_START_BIT 0
#define VEXT_SUPPPLY_1_REGULATION_WIDTH 2

#define VEXT_SUPPPLY_1_VOLTAGE_MANAGEMENT_REG 0x408
#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VEXT_SUPPPLY_1_REGULATION_PARAM_MASK  0x3



//  
//  00: force VextSuppply2 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VextSuppply2Request signal) to control VextSuppply2 supply if 
//  VextSuppply2RequestCtrl[1:0]=0x. Forced in HP mode if VextSuppply2RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 2:3, typedef regulation
//   
//  
#define VEXT_SUPPPLY_2_REGULATION_MASK 0xCU
#define VEXT_SUPPPLY_2_REGULATION_OFFSET 3
#define VEXT_SUPPPLY_2_REGULATION_STOP_BIT 3
#define VEXT_SUPPPLY_2_REGULATION_START_BIT 2
#define VEXT_SUPPPLY_2_REGULATION_WIDTH 2

#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VEXT_SUPPPLY_2_REGULATION_PARAM_MASK  0xC



//  
//  00: force VextSuppply3 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal VextSuppply3Request signal) to control VextSuppply3 supply if 
//  VextSuppply3RequestCtrl[1:0]=0x. Forced in HP mode if VextSuppply3RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 4:5, typedef regulation
//   
//  
#define VEXT_SUPPPLY_3_REGULATION_MASK 0x30U
#define VEXT_SUPPPLY_3_REGULATION_OFFSET 5
#define VEXT_SUPPPLY_3_REGULATION_STOP_BIT 5
#define VEXT_SUPPPLY_3_REGULATION_START_BIT 4
#define VEXT_SUPPPLY_3_REGULATION_WIDTH 2

#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VEXT_SUPPPLY_3_REGULATION_PARAM_MASK  0x30



//  
//  External Smps is with ByPass mode ExtSupply2.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 6:6, typedef by_pass
//   
//  
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_MASK 0x40U
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_OFFSET 6
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_STOP_BIT 6
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_START_BIT 6
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_WIDTH 1


typedef enum by_pass {
    WITHOUT_BYPASS_E,
    WITH_BYPASS_E
} BY_PASS_T ;
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_2_PARAM_MASK  0x40



//  
//  External Smps is with ByPass mode ExtSupply3.
//  Register VextSuppply1VoltageManagement 0x0408, Bits 7:7, typedef by_pass
//   
//  
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_MASK 0x80U
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_OFFSET 7
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_STOP_BIT 7
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_START_BIT 7
#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_WIDTH 1


#define EXTERNAL_SMPS_BY_PASS_EXT_SUPPLY_3_PARAM_MASK  0x80



//  
//  00: force Vaux1 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vaux1Request signal) to control Vaux1 supply if 
//  Vaux1RequestCtrl[1:0]=0x. Forced in HP mode if Vaux1RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VauxVoltageManagement 0x0409, Bits 0:1, typedef regulation
//   
//  
#define VAUX_1_REGULATION_MASK 0x3U
#define VAUX_1_REGULATION_OFFSET 1
#define VAUX_1_REGULATION_STOP_BIT 1
#define VAUX_1_REGULATION_START_BIT 0
#define VAUX_1_REGULATION_WIDTH 2

#define VAUX_VOLTAGE_MANAGEMENT_REG 0x409
#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VAUX_1_REGULATION_PARAM_MASK  0x3



//  
//  00: force Vaux2 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vaux2Request signal) to control Vaux2 supply if 
//  Vaux2RequestCtrl[1:0]=0x. Forced in HP mode if Vaux2RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register VauxVoltageManagement 0x0409, Bits 2:3, typedef regulation
//   
//  
#define VAUX_2_REGULATION_MASK 0xCU
#define VAUX_2_REGULATION_OFFSET 3
#define VAUX_2_REGULATION_STOP_BIT 3
#define VAUX_2_REGULATION_START_BIT 2
#define VAUX_2_REGULATION_WIDTH 2

#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VAUX_2_REGULATION_PARAM_MASK  0xC



//  
//  00: force Vaux3 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vaux3Request signal) to control Vaux3 supply if 
//  Vaux3RequestCtrl[1:0]=0x. Forced in HP mode if Vaux3RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vaux3VoltageManagement 0x040A, Bits 0:1, typedef regulation
//   
//  
#define VAUX_3_REGULATION_MASK 0x3U
#define VAUX_3_REGULATION_OFFSET 1
#define VAUX_3_REGULATION_STOP_BIT 1
#define VAUX_3_REGULATION_START_BIT 0
#define VAUX_3_REGULATION_WIDTH 2

#define VAUX_3_VOLTAGE_MANAGEMENT_REG 0x40A
#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VAUX_3_REGULATION_PARAM_MASK  0x3



//  
//  00: force Vrf1 disabled 
//  01: force enabled in HP mode 
//  10: allows HW (internal Vrf1Request signal) to control Vrf1 supply if 
//  Vrf1RequestCtrl[1:0]=0x. Forced in HP mode if Vrf1RequestCtrl[1:0]=1x 
//  11: force enable in LP mode.
//  Register Vaux3VoltageManagement 0x040A, Bits 2:3, typedef regulation
//   
//  
#define VRF_1_REGULATION_MASK 0xCU
#define VRF_1_REGULATION_OFFSET 3
#define VRF_1_REGULATION_STOP_BIT 3
#define VRF_1_REGULATION_START_BIT 2
#define VRF_1_REGULATION_WIDTH 2

#define  REGULATION_DISABLE 0
#define  REGULATION_FORCE_HP_MODE 1
#define  REGULATION_ALLOW_HW 2
#define  REGULATION_FORCE_LOW_POWER 3

#define VRF_1_REGULATION_PARAM_MASK  0xC


#define VARM_OUTPUT_SEL_1_PARAM_MASK  0x7F
#define VARM_OUTPUT_SEL_1_MASK 0x7FU
#define VARM_OUTPUT_SEL_1_OFFSET 6
#define VARM_OUTPUT_SEL_1_STOP_BIT 6
#define VARM_OUTPUT_SEL_1_START_BIT 0
#define VARM_OUTPUT_SEL_1_WIDTH 7

typedef enum output_supply {
    OUTPUT_SUPPLY_VOLTAGE_0_70_V = 0x0,
    OUTPUT_SUPPLY_VOLTAGE_0_71_V = 0x1,
    OUTPUT_SUPPLY_VOLTAGE_0_72_V = 0x2,
    OUTPUT_SUPPLY_VOLTAGE_0_74_V = 0x3,
    OUTPUT_SUPPLY_VOLTAGE_0_75_V = 0x4,
    OUTPUT_SUPPLY_VOLTAGE_0_76_V = 0x5,
    OUTPUT_SUPPLY_VOLTAGE_0_77_V = 0x6,
    OUTPUT_SUPPLY_VOLTAGE_0_79_V = 0x7,
    OUTPUT_SUPPLY_VOLTAGE_0_80_V = 0x8,
    OUTPUT_SUPPLY_VOLTAGE_0_81_V = 0x9,
    OUTPUT_SUPPLY_VOLTAGE_0_82_V = 0xA,
    OUTPUT_SUPPLY_VOLTAGE_0_84_V = 0xB,
    OUTPUT_SUPPLY_VOLTAGE_0_85_V = 0xC,
    OUTPUT_SUPPLY_VOLTAGE_0_86_V = 0xD,
    OUTPUT_SUPPLY_VOLTAGE_0_87_V = 0xE,
    OUTPUT_SUPPLY_VOLTAGE_0_89_V = 0xF,
    OUTPUT_SUPPLY_VOLTAGE_0_90_V = 0x10,
    OUTPUT_SUPPLY_VOLTAGE_0_91_V = 0x11,
    OUTPUT_SUPPLY_VOLTAGE_0_92_V = 0x12,
    OUTPUT_SUPPLY_VOLTAGE_0_94_V = 0x13,
    OUTPUT_SUPPLY_VOLTAGE_0_95_V = 0x14,
    OUTPUT_SUPPLY_VOLTAGE_0_96_V = 0x15,
    OUTPUT_SUPPLY_VOLTAGE_0_97_V = 0x16,
    OUTPUT_SUPPLY_VOLTAGE_0_99_V = 0x17,
    OUTPUT_SUPPLY_VOLTAGE_1_V = 0x18,
    OUTPUT_SUPPLY_VOLTAGE_1_01_V = 0x19,
    OUTPUT_SUPPLY_VOLTAGE_1_02_V = 0x1A,
    OUTPUT_SUPPLY_VOLTAGE_1_04_V = 0x1B,
    OUTPUT_SUPPLY_VOLTAGE_1_05_V = 0x1C,
    OUTPUT_SUPPLY_VOLTAGE_1_06_V = 0x1D,
    OUTPUT_SUPPLY_VOLTAGE_1_07_V = 0x1E,
    OUTPUT_SUPPLY_VOLTAGE_1_09_V = 0x1F,
    OUTPUT_SUPPLY_VOLTAGE_1_10_V = 0x20,
    OUTPUT_SUPPLY_VOLTAGE_1_11_V = 0x21,
    OUTPUT_SUPPLY_VOLTAGE_1_13_V = 0x22,
    OUTPUT_SUPPLY_VOLTAGE_1_14_V = 0x23,
    OUTPUT_SUPPLY_VOLTAGE_1_15_V = 0x24,
    OUTPUT_SUPPLY_VOLTAGE_1_16_V = 0x25,
    OUTPUT_SUPPLY_VOLTAGE_1_18_V = 0x26,
    OUTPUT_SUPPLY_VOLTAGE_1_19_V = 0x27,
    OUTPUT_SUPPLY_VOLTAGE_1_20_V = 0x28,
    OUTPUT_SUPPLY_VOLTAGE_1_21_V = 0x29,
    OUTPUT_SUPPLY_VOLTAGE_1_23_V = 0x2A,
    OUTPUT_SUPPLY_VOLTAGE_1_24_V = 0x2B,
    OUTPUT_SUPPLY_VOLTAGE_1_25_V = 0x2C,
    OUTPUT_SUPPLY_VOLTAGE_1_26_V = 0x2D,
    OUTPUT_SUPPLY_VOLTAGE_1_28_V = 0x2E,
    OUTPUT_SUPPLY_VOLTAGE_1_29_V = 0x2F,
    OUTPUT_SUPPLY_VOLTAGE_1_30_V = 0x30,
    OUTPUT_SUPPLY_VOLTAGE_1_31_V = 0x31,
    OUTPUT_SUPPLY_VOLTAGE_1_33_V = 0x32,
    OUTPUT_SUPPLY_VOLTAGE_1_34_V = 0x33,
    OUTPUT_SUPPLY_VOLTAGE_1_35_V = 0x34,
    OUTPUT_SUPPLY_VOLTAGE_1_36_V = 0x35,
    OUTPUT_SUPPLY_VOLTAGE_1_38_V = 0x36,
    OUTPUT_SUPPLY_VOLTAGE_1_39_V = 0x37,
    OUTPUT_SUPPLY_VOLTAGE_1_40_V = 0x38
} OUTPUT_SUPPLY_T ;
#define VARM_OUTPUT_SEL_1_REG 0x40B
#define VARM_OUTPUT_SEL_2_PARAM_MASK  0x7F
#define VARM_OUTPUT_SEL_2_MASK 0x7FU
#define VARM_OUTPUT_SEL_2_OFFSET 6
#define VARM_OUTPUT_SEL_2_STOP_BIT 6
#define VARM_OUTPUT_SEL_2_START_BIT 0
#define VARM_OUTPUT_SEL_2_WIDTH 7

#define VARM_OUTPUT_SEL_2_REG 0x40C
#define VARM_OUTPUT_SEL_3_PARAM_MASK  0x7F
#define VARM_OUTPUT_SEL_3_MASK 0x7FU
#define VARM_OUTPUT_SEL_3_OFFSET 6
#define VARM_OUTPUT_SEL_3_STOP_BIT 6
#define VARM_OUTPUT_SEL_3_START_BIT 0
#define VARM_OUTPUT_SEL_3_WIDTH 7

#define VARM_OUTPUT_SEL_3_REG 0x40D
#define VAPE_OUTPUT_SEL_1_PARAM_MASK  0x3F
#define VAPE_OUTPUT_SEL_1_MASK 0x3FU
#define VAPE_OUTPUT_SEL_1_OFFSET 5
#define VAPE_OUTPUT_SEL_1_STOP_BIT 5
#define VAPE_OUTPUT_SEL_1_START_BIT 0
#define VAPE_OUTPUT_SEL_1_WIDTH 6

#define VAPE_OUTPUT_SEL_1_REG 0x40E
#define VAPE_OUTPUT_SEL_2_PARAM_MASK  0x7F
#define VAPE_OUTPUT_SEL_2_MASK 0x7FU
#define VAPE_OUTPUT_SEL_2_OFFSET 6
#define VAPE_OUTPUT_SEL_2_STOP_BIT 6
#define VAPE_OUTPUT_SEL_2_START_BIT 0
#define VAPE_OUTPUT_SEL_2_WIDTH 7

#define VAPE_OUTPUT_SEL_2_REG 0x40F
#define VAPE_OUTPUT_SEL_3_PARAM_MASK  0x7F
#define VAPE_OUTPUT_SEL_3_MASK 0x7FU
#define VAPE_OUTPUT_SEL_3_OFFSET 6
#define VAPE_OUTPUT_SEL_3_STOP_BIT 6
#define VAPE_OUTPUT_SEL_3_START_BIT 0
#define VAPE_OUTPUT_SEL_3_WIDTH 7

#define VAPE_OUTPUT_SEL_3_REG 0x410

//  
//  VBBN output supply value management.
//  Register VbbOutputSupply1 0x0411, Bits 4:7, typedef negative_voltage_selection
//   
//  
#define VBB_NEGATIVE_SELECTION_1_MASK 0xF0U
#define VBB_NEGATIVE_SELECTION_1_OFFSET 7
#define VBB_NEGATIVE_SELECTION_1_STOP_BIT 7
#define VBB_NEGATIVE_SELECTION_1_START_BIT 4
#define VBB_NEGATIVE_SELECTION_1_WIDTH 4

#define VBB_OUTPUT_SUPPLY_1_REG 0x411
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
#define VBB_NEGATIVE_SELECTION_1_PARAM_MASK  0xF0



//  
//  VBBP output supply value management.
//  Register VbbOutputSupply1 0x0411, Bits 0:3, typedef positive_voltage_selection
//   
//  
#define VBB_POSITIVE_SELECTION_1_MASK 0xFU
#define VBB_POSITIVE_SELECTION_1_OFFSET 3
#define VBB_POSITIVE_SELECTION_1_STOP_BIT 3
#define VBB_POSITIVE_SELECTION_1_START_BIT 0
#define VBB_POSITIVE_SELECTION_1_WIDTH 4

#define  VBBPFB 0
#define  VBBPFB_01V 1
#define  VBBPFB_02V 2
#define  VBBPFB_03V 3
#define  VBBPFB_04V 4
#define  VBBPFB_MINUS_04V 8
#define  VBBPFB_MINUS_03V 13
#define  VBBPFB_MINUS_02V 14
#define  VBBPFB_MINUS_01V 15

typedef enum positive_voltage_selection {
    VBBPFB_E,
    VBBPFB_01V_E,
    VBBPFB_02V_E,
    VBBPFB_03V_E,
    VBBPFB_04V_E,
    VBBPFB_MINUS_04V_E,
    VBBPFB_MINUS_03V_E,
    VBBPFB_MINUS_02V_E,
    VBBPFB_MINUS_01V_E
} POSITIVE_VOLTAGE_SELECTION_T ;
#define VBB_POSITIVE_SELECTION_1_PARAM_MASK  0xF



//  
//  VBBN output supply value management.
//  Register VbbOutputSupply2 0x0412, Bits 4:7, typedef negative_voltage_selection
//   
//  
#define VBB_NEGATIVE_SELECTION_2_MASK 0xF0U
#define VBB_NEGATIVE_SELECTION_2_OFFSET 7
#define VBB_NEGATIVE_SELECTION_2_STOP_BIT 7
#define VBB_NEGATIVE_SELECTION_2_START_BIT 4
#define VBB_NEGATIVE_SELECTION_2_WIDTH 4

#define VBB_OUTPUT_SUPPLY_2_REG 0x412
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

#define VBB_NEGATIVE_SELECTION_2_PARAM_MASK  0xF0



//  
//  VBBP output supply value management.
//  Register VbbOutputSupply2 0x0412, Bits 0:3, typedef positive_voltage_selection
//   
//  
#define VBB_POSITIVE_SELECTION_2_MASK 0xFU
#define VBB_POSITIVE_SELECTION_2_OFFSET 3
#define VBB_POSITIVE_SELECTION_2_STOP_BIT 3
#define VBB_POSITIVE_SELECTION_2_START_BIT 0
#define VBB_POSITIVE_SELECTION_2_WIDTH 4

#define  VBBPFB 0
#define  VBBPFB_01V 1
#define  VBBPFB_02V 2
#define  VBBPFB_03V 3
#define  VBBPFB_04V 4
#define  VBBPFB_MINUS_04V 8
#define  VBBPFB_MINUS_03V 13
#define  VBBPFB_MINUS_02V 14
#define  VBBPFB_MINUS_01V 15

#define VBB_POSITIVE_SELECTION_2_PARAM_MASK  0xF


#define VSMPS_1_SEL_1_OUTPUT_PARAM_MASK  0x3F
#define VSMPS_1_SEL_1_OUTPUT_MASK 0x3FU
#define VSMPS_1_SEL_1_OUTPUT_OFFSET 5
#define VSMPS_1_SEL_1_OUTPUT_STOP_BIT 5
#define VSMPS_1_SEL_1_OUTPUT_START_BIT 0
#define VSMPS_1_SEL_1_OUTPUT_WIDTH 6

typedef enum output_supply_med {
    OUTPUT_SUPPLY_MED_VOLTAGE_1_10_V = 0x0,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_11_V = 0x1,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_13_V = 0x2,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_14_V = 0x3,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_15_V = 0x4,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_16_V = 0x5,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_18_V = 0x6,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_19_V = 0x7,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_20_V = 0x8,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_21_V = 0x9,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_23_V = 0xA,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_24_V = 0xB,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_25_V = 0xC,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_26_V = 0xD,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_28_V = 0xE,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_29_V = 0xF,
    OUTPUT_SUPPLY_MED_VOLTAGE_1_30_V = 0x10
} OUTPUT_SUPPLY_MED_T ;
#define VSMPS_1_SEL_1_OUTPUT_REG 0x413
#define VSMPS_1_SEL_2_OUTPUT_PARAM_MASK  0x3F
#define VSMPS_1_SEL_2_OUTPUT_MASK 0x3FU
#define VSMPS_1_SEL_2_OUTPUT_OFFSET 5
#define VSMPS_1_SEL_2_OUTPUT_STOP_BIT 5
#define VSMPS_1_SEL_2_OUTPUT_START_BIT 0
#define VSMPS_1_SEL_2_OUTPUT_WIDTH 6

#define VSMPS_1_SEL_2_OUTPUT_REG 0x414
#define VSMPS_1_SEL_3_OUTPUT_PARAM_MASK  0x3F
#define VSMPS_1_SEL_3_OUTPUT_MASK 0x3FU
#define VSMPS_1_SEL_3_OUTPUT_OFFSET 5
#define VSMPS_1_SEL_3_OUTPUT_STOP_BIT 5
#define VSMPS_1_SEL_3_OUTPUT_START_BIT 0
#define VSMPS_1_SEL_3_OUTPUT_WIDTH 6

#define VSMPS_1_SEL_3_OUTPUT_REG 0x415
#define VSMPS_2_SEL_1_OUTPUT_PARAM_MASK  0x3F
#define VSMPS_2_SEL_1_OUTPUT_MASK 0x3FU
#define VSMPS_2_SEL_1_OUTPUT_OFFSET 5
#define VSMPS_2_SEL_1_OUTPUT_STOP_BIT 5
#define VSMPS_2_SEL_1_OUTPUT_START_BIT 0
#define VSMPS_2_SEL_1_OUTPUT_WIDTH 6

typedef enum output_supply_large {
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_70_V = 0x0,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_71_V = 0x1,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_72_V = 0x2,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_74_V = 0x3,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_75_V = 0x4,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_76_V = 0x5,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_77_V = 0x6,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_79_V = 0x7,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_80_V = 0x8,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_81_V = 0x9,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_82_V = 0xA,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_84_V = 0xB,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_85_V = 0xC,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_86_V = 0xD,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_87_V = 0xE,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_89_V = 0xF,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_90_V = 0x10,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_91_V = 0x11,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_92_V = 0x12,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_94_V = 0x13,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_95_V = 0x14,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_96_V = 0x15,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_97_V = 0x16,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_0_99_V = 0x17,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_V = 0x18,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_01_V = 0x19,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_02_V = 0x1A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_04_V = 0x1B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_05_V = 0x1C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_06_V = 0x1D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_07_V = 0x1E,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_09_V = 0x1F,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_10_V = 0x20,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_11_V = 0x21,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_13_V = 0x22,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_14_V = 0x23,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_15_V = 0x24,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_16_V = 0x25,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_18_V = 0x26,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_19_V = 0x27,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_20_V = 0x28,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_21_V = 0x29,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_23_V = 0x2A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_24_V = 0x2B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_25_V = 0x2C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_26_V = 0x2D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_28_V = 0x2E,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_29_V = 0x2F,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_30_V = 0x30,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_31_V = 0x31,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_33_V = 0x32,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_34_V = 0x33,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_35_V = 0x34,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_36_V = 0x35,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_38_V = 0x36,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_39_V = 0x37,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_40_V = 0x38,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_41_V = 0x39,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_43_V = 0x3A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_44_V = 0x3B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_45_V = 0x3C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_46_V = 0x3D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_48_V = 0x3E,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_49_V = 0x3F,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_50_V = 0x40,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_51_V = 0x41,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_53_V = 0x42,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_54_V = 0x43,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_55_V = 0x44,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_56_V = 0x45,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_58_V = 0x46,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_59_V = 0x47,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_60_V = 0x48,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_61_V = 0x49,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_63_V = 0x4A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_64_V = 0x4B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_65_V = 0x4C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_66_V = 0x4D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_68_V = 0x4E,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_69_V = 0x4F,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_70_V = 0x50,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_71_V = 0x51,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_73_V = 0x52,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_74_V = 0x53,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_75_V = 0x54,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_76_V = 0x55,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_78_V = 0x56,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_79_V = 0x57,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_80_V = 0x58,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_81_V = 0x59,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_83_V = 0x5A,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_84_V = 0x5B,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_85_V = 0x5C,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_86_V = 0x5D,
    OUTPUT_SUPPLY_LARGE_VOLTAGE_1_88_V = 0x5E
} OUTPUT_SUPPLY_LARGE_T ;
#define VSMPS_2_SEL_1_OUTPUT_REG 0x417
#define VSMPS_2_SEL_2_OUTPUT_PARAM_MASK  0x3F
#define VSMPS_2_SEL_2_OUTPUT_MASK 0x3FU
#define VSMPS_2_SEL_2_OUTPUT_OFFSET 5
#define VSMPS_2_SEL_2_OUTPUT_STOP_BIT 5
#define VSMPS_2_SEL_2_OUTPUT_START_BIT 0
#define VSMPS_2_SEL_2_OUTPUT_WIDTH 6

#define VSMPS_2_SEL_2_OUTPUT_REG 0x418
#define VSMPS_2_SEL_3_OUTPUT_PARAM_MASK  0x3F
#define VSMPS_2_SEL_3_OUTPUT_MASK 0x3FU
#define VSMPS_2_SEL_3_OUTPUT_OFFSET 5
#define VSMPS_2_SEL_3_OUTPUT_STOP_BIT 5
#define VSMPS_2_SEL_3_OUTPUT_START_BIT 0
#define VSMPS_2_SEL_3_OUTPUT_WIDTH 6

#define VSMPS_2_SEL_3_OUTPUT_REG 0x419
#define VSMPS_3_SEL_1_OUTPUT_PARAM_MASK  0x7F
#define VSMPS_3_SEL_1_OUTPUT_MASK 0x7FU
#define VSMPS_3_SEL_1_OUTPUT_OFFSET 6
#define VSMPS_3_SEL_1_OUTPUT_STOP_BIT 6
#define VSMPS_3_SEL_1_OUTPUT_START_BIT 0
#define VSMPS_3_SEL_1_OUTPUT_WIDTH 7

typedef enum output_supply3 {
    OUTPUT_VOLTAGE_VSMPS3_0_70_V = 0x0,
    OUTPUT_VOLTAGE_VSMPS3_0_71_V = 0x1,
    OUTPUT_VOLTAGE_VSMPS3_0_72_V = 0x2,
    OUTPUT_VOLTAGE_VSMPS3_0_74_V = 0x3,
    OUTPUT_VOLTAGE_VSMPS3_0_75_V = 0x4,
    OUTPUT_VOLTAGE_VSMPS3_0_76_V = 0x5,
    OUTPUT_VOLTAGE_VSMPS3_0_77_V = 0x6,
    OUTPUT_VOLTAGE_VSMPS3_0_79_V = 0x7,
    OUTPUT_VOLTAGE_VSMPS3_0_80_V = 0x8,
    OUTPUT_VOLTAGE_VSMPS3_0_81_V = 0x9,
    OUTPUT_VOLTAGE_VSMPS3_0_82_V = 0xA,
    OUTPUT_VOLTAGE_VSMPS3_0_84_V = 0xB,
    OUTPUT_VOLTAGE_VSMPS3_0_85_V = 0xC,
    OUTPUT_VOLTAGE_VSMPS3_0_86_V = 0xD,
    OUTPUT_VOLTAGE_VSMPS3_0_87_V = 0xE,
    OUTPUT_VOLTAGE_VSMPS3_0_89_V = 0xF,
    OUTPUT_VOLTAGE_VSMPS3_0_90_V = 0x10,
    OUTPUT_VOLTAGE_VSMPS3_0_91_V = 0x11,
    OUTPUT_VOLTAGE_VSMPS3_0_92_V = 0x12,
    OUTPUT_VOLTAGE_VSMPS3_0_94_V = 0x13,
    OUTPUT_VOLTAGE_VSMPS3_0_95_V = 0x14,
    OUTPUT_VOLTAGE_VSMPS3_0_96_V = 0x15,
    OUTPUT_VOLTAGE_VSMPS3_0_97_V = 0x16,
    OUTPUT_VOLTAGE_VSMPS3_0_99_V = 0x17,
    OUTPUT_VOLTAGE_VSMPS3_1_V = 0x18,
    OUTPUT_VOLTAGE_VSMPS3_1_01_V = 0x19,
    OUTPUT_VOLTAGE_VSMPS3_1_02_V = 0x1A,
    OUTPUT_VOLTAGE_VSMPS3_1_04_V = 0x1B,
    OUTPUT_VOLTAGE_VSMPS3_1_05_V = 0x1C,
    OUTPUT_VOLTAGE_VSMPS3_1_06_V = 0x1D,
    OUTPUT_VOLTAGE_VSMPS3_1_07_V = 0x1E,
    OUTPUT_VOLTAGE_VSMPS3_1_09_V = 0x1F,
    OUTPUT_VOLTAGE_VSMPS3_1_10_V = 0x20,
    OUTPUT_VOLTAGE_VSMPS3_1_11_V = 0x21,
    OUTPUT_VOLTAGE_VSMPS3_1_13_V = 0x22,
    OUTPUT_VOLTAGE_VSMPS3_1_14_V = 0x23,
    OUTPUT_VOLTAGE_VSMPS3_1_15_V = 0x24,
    OUTPUT_VOLTAGE_VSMPS3_1_16_V = 0x25,
    OUTPUT_VOLTAGE_VSMPS3_1_18_V = 0x26,
    OUTPUT_VOLTAGE_VSMPS3_1_19_V = 0x27,
    OUTPUT_VOLTAGE_VSMPS3_1_20_V = 0x28,
    OUTPUT_VOLTAGE_VSMPS3_1_21_V = 0x29,
    OUTPUT_VOLTAGE_VSMPS3_1_23_V = 0x2A,
    OUTPUT_VOLTAGE_VSMPS3_1_24_V = 0x2B,
    OUTPUT_VOLTAGE_VSMPS3_1_25_V = 0x2C,
    OUTPUT_VOLTAGE_VSMPS3_1_26_V = 0x2D,
    OUTPUT_VOLTAGE_VSMPS3_1_28_V = 0x2E,
    OUTPUT_VOLTAGE_VSMPS3_1_29_V = 0x2F,
    OUTPUT_VOLTAGE_VSMPS3_1_30_V = 0x30,
    OUTPUT_VOLTAGE_VSMPS3_1_31_V = 0x31,
    OUTPUT_VOLTAGE_VSMPS3_1_33_V = 0x32,
    OUTPUT_VOLTAGE_VSMPS3_1_34_V = 0x33,
    OUTPUT_VOLTAGE_VSMPS3_1_35_V = 0x34,
    OUTPUT_VOLTAGE_VSMPS3_1_36_V = 0x35
} OUTPUT_SUPPLY_3_T ;
#define VSMPS_3_SEL_1_OUTPUT_REG 0x41B
#define VSMPS_3_SEL_2_OUTPUT_PARAM_MASK  0x7F
#define VSMPS_3_SEL_2_OUTPUT_MASK 0x7FU
#define VSMPS_3_SEL_2_OUTPUT_OFFSET 6
#define VSMPS_3_SEL_2_OUTPUT_STOP_BIT 6
#define VSMPS_3_SEL_2_OUTPUT_START_BIT 0
#define VSMPS_3_SEL_2_OUTPUT_WIDTH 7

#define VSMPS_3_SEL_2_OUTPUT_REG 0x41C
#define VSMPS_3_SEL_3_OUTPUT_PARAM_MASK  0x7F
#define VSMPS_3_SEL_3_OUTPUT_MASK 0x7FU
#define VSMPS_3_SEL_3_OUTPUT_OFFSET 6
#define VSMPS_3_SEL_3_OUTPUT_STOP_BIT 6
#define VSMPS_3_SEL_3_OUTPUT_START_BIT 0
#define VSMPS_3_SEL_3_OUTPUT_WIDTH 7

#define VSMPS_3_SEL_3_OUTPUT_REG 0x41D

//  
//  Vaux1 output supply value management.
//  Register Vaux1Sel1Output 0x041F, Bits 0:3, typedef vaux_output
//   
//  
#define VAUX_1_SEL_OUTPUT_MASK 0xFU
#define VAUX_1_SEL_OUTPUT_OFFSET 3
#define VAUX_1_SEL_OUTPUT_STOP_BIT 3
#define VAUX_1_SEL_OUTPUT_START_BIT 0
#define VAUX_1_SEL_OUTPUT_WIDTH 4

#define VAUX_1_SEL_1_OUTPUT_REG 0x41F
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
#define VAUX_1_SEL_OUTPUT_PARAM_MASK  0xF



//  
//  Vaux2 output supply value management.
//  Register Vaux2Sel1Output 0x0420, Bits 0:3, typedef vaux_output
//   
//  
#define VAUX_2_SEL_OUTPUT_MASK 0xFU
#define VAUX_2_SEL_OUTPUT_OFFSET 3
#define VAUX_2_SEL_OUTPUT_STOP_BIT 3
#define VAUX_2_SEL_OUTPUT_START_BIT 0
#define VAUX_2_SEL_OUTPUT_WIDTH 4

#define VAUX_2_SEL_1_OUTPUT_REG 0x420
#define  VAUX_1200MV 0
#define  VAUX_1500MV 1
#define  VAUX_1800MV 2
#define  VAUX_2100MV 3
#define  VAUX_2500MV 4
#define  VAUX_2750MV 5
#define  VAUX_2790MV 6
#define  VAUX_2910MV 7

#define VAUX_2_SEL_OUTPUT_PARAM_MASK  0xF



//  
//  Vrf1 output supply value management.
//  Register Vaux3Sel1Output 0x0421, Bits 4:5, typedef vrf1_output
//   
//  
#define VRF_1_SEL_OUTPUT_MASK 0x30U
#define VRF_1_SEL_OUTPUT_OFFSET 5
#define VRF_1_SEL_OUTPUT_STOP_BIT 5
#define VRF_1_SEL_OUTPUT_START_BIT 4
#define VRF_1_SEL_OUTPUT_WIDTH 2

#define VAUX_3_SEL_1_OUTPUT_REG 0x421
#define  VRF1_1800_MV 0
#define  VRF1_2000_MV 1
#define  VRF1_2150_MV 2
#define  VRF1_2500_MV 3

typedef enum vrf1_output {
    VRF1_1800_MV_E,
    VRF1_2000_MV_E,
    VRF1_2150_MV_E,
    VRF1_2500_MV_E
} VRF_1_OUTPUT_T ;
#define VRF_1_SEL_OUTPUT_PARAM_MASK  0x30



//  
//  Vaux2 output supply value management.
//  Register Vaux3Sel1Output 0x0421, Bits 0:2, typedef vaux_output
//   
//  
#define VAUX_3_SEL_OUTPUT_MASK 0x7U
#define VAUX_3_SEL_OUTPUT_OFFSET 2
#define VAUX_3_SEL_OUTPUT_STOP_BIT 2
#define VAUX_3_SEL_OUTPUT_START_BIT 0
#define VAUX_3_SEL_OUTPUT_WIDTH 3

#define  VAUX_1200MV 0
#define  VAUX_1500MV 1
#define  VAUX_1800MV 2
#define  VAUX_2100MV 3
#define  VAUX_2500MV 4
#define  VAUX_2750MV 5
#define  VAUX_2790MV 6
#define  VAUX_2910MV 7

#define VAUX_3_SEL_OUTPUT_PARAM_MASK  0x7



//  
//  0: inactive 
//  1: Force ExtSupply12LPn = Low when 'ExtSup12LPnClkSel[1:0]' = 00 
//  (ExtSup12LPn/ExtSupply12Clk pad functionality = ExtSupply12LPn).
//  Register ExternalSupplyControl 0x0422, Bits 0:0, typedef ext_supply_force_low
//   
//  
#define EXT_SUPPLY_FORCE_LOW_MASK 0x1U
#define EXT_SUPPLY_FORCE_LOW_OFFSET 0
#define EXT_SUPPLY_FORCE_LOW_STOP_BIT 0
#define EXT_SUPPLY_FORCE_LOW_START_BIT 0
#define EXT_SUPPLY_FORCE_LOW_WIDTH 1

#define EXTERNAL_SUPPLY_CONTROL_REG 0x422

typedef enum ext_supply_force_low {
    INNACTIVE_E,
    FORCE_E
} EXT_SUPPLY_FORCE_LOW_T ;
#define EXT_SUPPLY_FORCE_LOW_PARAM_MASK  0x1


#define MODEM_CONTROL_2_REG 0x423
#define VARM_SELECTION_4_REG 0x424
#define VARM_SELECTION_5_REG 0x425
#define VARM_SELECTION_6_REG 0x426
#define VARM_SELECTION_7_REG 0x427
#define VARM_SEL_4567_CTRL_REG 0x428
#define VANA_VPLL_SEL_REG 0x429

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
#define VMOD_REGULATION_MASK 0x3U
#define VMOD_REGULATION_OFFSET 1
#define VMOD_REGULATION_STOP_BIT 1
#define VMOD_REGULATION_START_BIT 0
#define VMOD_REGULATION_WIDTH 2

#define VMOD_SUPPLY_MANAGEMENT_REG 0x440
#define  FORCE_VMOD_DISABLED 0
#define  FORCE_ENABLED_IN_HP_MODE 1
#define  ALLOWS_HW_CONTROL 2
#define  FORCE_ENABLE_IN_LP_MODE 3

typedef enum vmod_regulation {
    FORCE_VMOD_DISABLED_E,
    FORCE_ENABLED_IN_HP_MODE_E,
    ALLOWS_HW_CONTROL_E,
    FORCE_ENABLE_IN_LP_MODE_E
} VMOD_REGULATION_T ;
#define VMOD_REGULATION_PARAM_MASK  0x3



//  
//  0: Vmod output voltage is defined by VmodSel1 register 
//  1: Vmod output voltage is defined by VmodSel2 register.
//  Register VmodSupplyManagement 0x0440, Bits 2:2, typedef vmod_selection
//   
//  
#define VMOD_SELECTION_CONTROL_MASK 0x4U
#define VMOD_SELECTION_CONTROL_OFFSET 2
#define VMOD_SELECTION_CONTROL_STOP_BIT 2
#define VMOD_SELECTION_CONTROL_START_BIT 2
#define VMOD_SELECTION_CONTROL_WIDTH 1


typedef enum vmod_selection {
    VMOD_SELECT1_E,
    VMOD_SELECT2_E
} VMOD_SELECTION_T ;
#define VMOD_SELECTION_CONTROL_PARAM_MASK  0x4



//  
//  0: Vmod is not in AutoMode 
//  1: Vmod is in AutoMode.
//  Register VmodSupplyManagement 0x0440, Bits 3:3, typedef vmod_auto_mode_enable
//   
//  
#define VMOD_AUTO_MODE_MASK 0x8U
#define VMOD_AUTO_MODE_OFFSET 3
#define VMOD_AUTO_MODE_STOP_BIT 3
#define VMOD_AUTO_MODE_START_BIT 3
#define VMOD_AUTO_MODE_WIDTH 1


typedef enum vmod_auto_mode_enable {
    VMOD_AUTO_MODE_DISABLE_E,
    VMOD_AUTO_MODE_ENABLE_E
} VMOD_AUTO_MODE_ENABLE_T ;
#define VMOD_AUTO_MODE_PARAM_MASK  0x8



//  
//  0: inactive (Vmod can be in PWM or PFM mode) 
//  1: Vmod is forced in PWM mode when output voltage is programm to a lower value 
//  (energy sent to the battery.
//  Register VmodSupplyManagement 0x0440, Bits 4:4, typedef vmod_forced_enable
//   
//  
#define VMOD_PWM_MODE_MASK 0x10U
#define VMOD_PWM_MODE_OFFSET 4
#define VMOD_PWM_MODE_STOP_BIT 4
#define VMOD_PWM_MODE_START_BIT 4
#define VMOD_PWM_MODE_WIDTH 1


typedef enum vmod_forced_enable {
    VMOD_FORCED_DISABLE_E,
    VMOD_FORCED_ENABLE_E
} VMOD_FORCED_ENABLE_T ;
#define VMOD_PWM_MODE_PARAM_MASK  0x10



//  
//  0: doesn't valid SysClkReq1 input to request system clock 
//  1: Valid SysClkReq1 input to request system clock.
//  Register VmodSupplyManagement 0x0440, Bits 5:5, typedef 
//  modem_system_clock_request_valid
//   
//  
#define MOD_SYS_CLK_REQ_1_VALID_MASK 0x20U
#define MOD_SYS_CLK_REQ_1_VALID_OFFSET 5
#define MOD_SYS_CLK_REQ_1_VALID_STOP_BIT 5
#define MOD_SYS_CLK_REQ_1_VALID_START_BIT 5
#define MOD_SYS_CLK_REQ_1_VALID_WIDTH 1


typedef enum modem_system_clock_request_valid {
    DOESNOT_VALID_SYSTEM_CLOCK_E,
    VALID_SYSTEM_CLOCK_E
} MODEM_SYSTEM_CLOCK_REQUEST_VALID_T ;
#define MOD_SYS_CLK_REQ_1_VALID_PARAM_MASK  0x20



//  
//  00: allows to set Vmod in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vmod in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vmod in HP whatever VmodRequest signal value.
//  Register VmodSupplyManagement 0x0440, Bits 6:7, typedef vmod_resquet_control
//   
//  
#define VMOD_RESQUET_CONTROL_MASK 0xC0U
#define VMOD_RESQUET_CONTROL_OFFSET 7
#define VMOD_RESQUET_CONTROL_STOP_BIT 7
#define VMOD_RESQUET_CONTROL_START_BIT 6
#define VMOD_RESQUET_CONTROL_WIDTH 2

#define  ALLOWS_TO_SET_HP_LP_MODE 0
#define  ALLOWS_TO_SET_HP_OFF_MODE 1
#define  FORCE_VMOD_IN_HP 2

typedef enum vmod_resquet_control {
    ALLOWS_TO_SET_HP_LP_MODE_E,
    ALLOWS_TO_SET_HP_OFF_MODE_E,
    FORCE_VMOD_IN_HP_E
} VMOD_RESQUET_CONTROL_T ;
#define VMOD_RESQUET_CONTROL_PARAM_MASK  0xC0


#define VMOD_SEL_1_OUTPUT_PARAM_MASK  0x3F
#define VMOD_SEL_1_OUTPUT_MASK 0x3FU
#define VMOD_SEL_1_OUTPUT_OFFSET 5
#define VMOD_SEL_1_OUTPUT_STOP_BIT 5
#define VMOD_SEL_1_OUTPUT_START_BIT 0
#define VMOD_SEL_1_OUTPUT_WIDTH 6

#define VMOD_SEL_1_OUTPUT_REG 0x441
#define VMOD_SEL_2_OUTPUT_PARAM_MASK  0x3F
#define VMOD_SEL_2_OUTPUT_MASK 0x3FU
#define VMOD_SEL_2_OUTPUT_OFFSET 5
#define VMOD_SEL_2_OUTPUT_STOP_BIT 5
#define VMOD_SEL_2_OUTPUT_START_BIT 0
#define VMOD_SEL_2_OUTPUT_WIDTH 6

#define VMOD_SEL_2_OUTPUT_REG 0x442

//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 0:0, typedef supply_discharge_time
//   
//  
#define VPLL_DISCHARGE_TIME_MASK 0x1U
#define VPLL_DISCHARGE_TIME_OFFSET 0
#define VPLL_DISCHARGE_TIME_STOP_BIT 0
#define VPLL_DISCHARGE_TIME_START_BIT 0
#define VPLL_DISCHARGE_TIME_WIDTH 1

#define SUPPLY_DISCHARGE_TIME_REG 0x443

typedef enum supply_discharge_time {
    LONG_DISCHARGE_TIME_E,
    SHORT_DISCHARGE_TIME_E
} SUPPLY_DISCHARGE_TIME_T ;
#define VPLL_DISCHARGE_TIME_PARAM_MASK  0x1



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 1:1, typedef supply_discharge_time
//   
//  
#define VRF_1_DISCHARGE_TIME_MASK 0x2U
#define VRF_1_DISCHARGE_TIME_OFFSET 1
#define VRF_1_DISCHARGE_TIME_STOP_BIT 1
#define VRF_1_DISCHARGE_TIME_START_BIT 1
#define VRF_1_DISCHARGE_TIME_WIDTH 1


#define VRF_1_DISCHARGE_TIME_PARAM_MASK  0x2



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 2:2, typedef supply_discharge_time
//   
//  
#define VAUX_1_DISCHARGE_TIME_MASK 0x4U
#define VAUX_1_DISCHARGE_TIME_OFFSET 2
#define VAUX_1_DISCHARGE_TIME_STOP_BIT 2
#define VAUX_1_DISCHARGE_TIME_START_BIT 2
#define VAUX_1_DISCHARGE_TIME_WIDTH 1


#define VAUX_1_DISCHARGE_TIME_PARAM_MASK  0x4



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 3:3, typedef supply_discharge_time
//   
//  
#define VAUX_2_DISCHARGE_TIME_MASK 0x8U
#define VAUX_2_DISCHARGE_TIME_OFFSET 3
#define VAUX_2_DISCHARGE_TIME_STOP_BIT 3
#define VAUX_2_DISCHARGE_TIME_START_BIT 3
#define VAUX_2_DISCHARGE_TIME_WIDTH 1


#define VAUX_2_DISCHARGE_TIME_PARAM_MASK  0x8



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 5:5, typedef supply_discharge_time
//   
//  
#define VINT_CORE_12_DISCHARGE_TIME_MASK 0x20U
#define VINT_CORE_12_DISCHARGE_TIME_OFFSET 5
#define VINT_CORE_12_DISCHARGE_TIME_STOP_BIT 5
#define VINT_CORE_12_DISCHARGE_TIME_START_BIT 5
#define VINT_CORE_12_DISCHARGE_TIME_WIDTH 1


#define VINT_CORE_12_DISCHARGE_TIME_PARAM_MASK  0x20



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 6:6, typedef supply_discharge_time
//   
//  
#define VTV_OUT_DISCHARGE_TIME_MASK 0x40U
#define VTV_OUT_DISCHARGE_TIME_OFFSET 6
#define VTV_OUT_DISCHARGE_TIME_STOP_BIT 6
#define VTV_OUT_DISCHARGE_TIME_START_BIT 6
#define VTV_OUT_DISCHARGE_TIME_WIDTH 1


#define VTV_OUT_DISCHARGE_TIME_PARAM_MASK  0x40



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime 0x0443, Bits 7:7, typedef supply_discharge_time
//   
//  
#define VAUDIO_DISCHARGE_TIME_MASK 0x80U
#define VAUDIO_DISCHARGE_TIME_OFFSET 7
#define VAUDIO_DISCHARGE_TIME_STOP_BIT 7
#define VAUDIO_DISCHARGE_TIME_START_BIT 7
#define VAUDIO_DISCHARGE_TIME_WIDTH 1


#define VAUDIO_DISCHARGE_TIME_PARAM_MASK  0x80



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime2 0x0444, Bits 0:0, typedef supply_discharge_time
//   
//  
#define VSIM_DISCHARGE_TIME_MASK 0x1U
#define VSIM_DISCHARGE_TIME_OFFSET 0
#define VSIM_DISCHARGE_TIME_STOP_BIT 0
#define VSIM_DISCHARGE_TIME_START_BIT 0
#define VSIM_DISCHARGE_TIME_WIDTH 1

#define SUPPLY_DISCHARGE_TIME_2_REG 0x444

#define VSIM_DISCHARGE_TIME_PARAM_MASK  0x1



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime2 0x0444, Bits 1:1, typedef supply_discharge_time
//   
//  
#define VANA_DISCHARGE_TIME_MASK 0x2U
#define VANA_DISCHARGE_TIME_OFFSET 1
#define VANA_DISCHARGE_TIME_STOP_BIT 1
#define VANA_DISCHARGE_TIME_START_BIT 1
#define VANA_DISCHARGE_TIME_WIDTH 1


#define VANA_DISCHARGE_TIME_PARAM_MASK  0x2



//  
//  0: When Vdmic is disabled, pull down is disabled 
//  1: When Vdmic is disabled, pull down is enabled.
//  Register SupplyDischargeTime2 0x0444, Bits 2:2, typedef vdmic_pull_down_enable
//   
//  
#define VDMIC_PULL_DOWN_MASK 0x4U
#define VDMIC_PULL_DOWN_OFFSET 2
#define VDMIC_PULL_DOWN_STOP_BIT 2
#define VDMIC_PULL_DOWN_START_BIT 2
#define VDMIC_PULL_DOWN_WIDTH 1


typedef enum vdmic_pull_down_enable {
    VDMIC_PULL_DOWN_DISABLE_E,
    VDMIC_PULL_DOWN_ENABLE_E
} VDMIC_PULL_DOWN_ENABLE_T ;
#define VDMIC_PULL_DOWN_PARAM_MASK  0x4



//  
//  0: When Vpll is disabled, pull down is disabled 
//  1: When Vpll is disabled, pull down is enabled.
//  Register SupplyDischargeTime2 0x0444, Bits 3:3, typedef vpll_pull_down_enable
//   
//  
#define VPLL_PULL_DOWN_MASK 0x8U
#define VPLL_PULL_DOWN_OFFSET 3
#define VPLL_PULL_DOWN_STOP_BIT 3
#define VPLL_PULL_DOWN_START_BIT 3
#define VPLL_PULL_DOWN_WIDTH 1


typedef enum vpll_pull_down_enable {
    VPLL_PULL_DOWN_DISABLE_E,
    VPLL_PULL_DOWN_ENABLE_E
} VPLL_PULL_DOWN_ENABLE_T ;
#define VPLL_PULL_DOWN_PARAM_MASK  0x8



//  
//  0: short discharge time when disabled 
//  1: long discharge time when disabled.
//  Register SupplyDischargeTime2 0x0444, Bits 4:4, typedef supply_discharge_time
//   
//  
#define VDMIC_DISCHARGE_TIME_MASK 0x10U
#define VDMIC_DISCHARGE_TIME_OFFSET 4
#define VDMIC_DISCHARGE_TIME_STOP_BIT 4
#define VDMIC_DISCHARGE_TIME_START_BIT 4
#define VDMIC_DISCHARGE_TIME_WIDTH 1


#define VDMIC_DISCHARGE_TIME_PARAM_MASK  0x10


#endif
