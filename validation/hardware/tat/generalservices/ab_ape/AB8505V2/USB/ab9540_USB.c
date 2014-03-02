/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_USB/Linux/ab9540_USB.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_USB_LINUX_AB9540_USB_C
#define C__LISP_PRODUCTION_AB9540_AB9540_USB_LINUX_AB9540_USB_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_USB.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_dplus_cmp_out
 *
 * RET  : Return the value of register 0xUsbLineStatus
 *
 * Notes : From register 0x580, bits 0:0
 *
 **************************************************************************/
unsigned char get_dplus_cmp_out()
  {
    unsigned char value;


    /* 
     * 0: DM Single Ended Receiver output is low 
     * 1: DMSingle Ended Receiver output is high 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & DPLUS_CMP_OUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_dminus_cmp_out
 *
 * RET  : Return the value of register 0xUsbLineStatus
 *
 * Notes : From register 0x580, bits 1:1
 *
 **************************************************************************/
unsigned char get_dminus_cmp_out()
  {
    unsigned char value;


    /* 
     * 0: DM Single Ended Receiver output is low 
     * 1: DMSingle Ended Receiver output is high 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & DMINUS_CMP_OUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_host_charger_detected
 *
 * RET  : Return the value of register 0xUsbLineStatus
 *
 * Notes : From register 0x580, bits 2:2
 *
 **************************************************************************/
unsigned char get_host_charger_detected()
  {
    unsigned char value;


    /* 
     * 0: No Host charger detected 
     * 1: Host Charger detected 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & HOST_CHARGER_DETECTED_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_charger_type
 *
 * RET  : Return the value of register 0xUsbLineStatus
 *
 * Notes : From register 0x580, bits 6:3
 *
 **************************************************************************/
unsigned char get_charger_type()
  {
    unsigned char value;


    /* 
     * 0000: Not configured 
     * 0001: Standard Host, not charging 
     * 0010: Standard Host, charging, not suspended 
     * 0011: Standard Host, charging, suspended 
     * 0100: Host charger, normal mode 
     * 0101: Host charger, HS mode 
     * 0110: Host charger, HS Chirp mode 
     * 0111: Dedicated USx charger 
     * 1000: ACA RID_A configuration 
     * 1001: ACA RID_B configuration 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & CHARGER_TYPE_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_uart_mode
 *
 * RET  : Return the value of register 0xUsbLineStatus
 *
 * Notes : From register 0x580, bits 7:7
 *
 **************************************************************************/
unsigned char get_uart_mode()
  {
    unsigned char value;


    /* 
     * 0: Uart mode is not active 
     * 1: Uart mode is active 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & UART_MODE_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_pull_up
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 0:0
 *
 **************************************************************************/
unsigned char set_charger_pull_up( enum id_detection_thru_adc_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: disable Id detection thru ADC 
     * 1: enable Id detection thru ADC 
     */ 
    switch( param ){
        case ID_DETECTION_THRU_ADC_ENABLE_E: 
           value = old_value | CHARGER_PULL_UP_MASK; 
           break;
        case ID_DETECTION_THRU_ADC_DISABLE_E: 
           value = old_value & ~ CHARGER_PULL_UP_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_pull_up
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 0:0
 *
 **************************************************************************/
unsigned char get_charger_pull_up()
  {
    unsigned char value;


    /* 
     * 0: disable Id detection thru ADC 
     * 1: enable Id detection thru ADC 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & CHARGER_PULL_UP_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_switch_ctrl
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 1:1
 *
 **************************************************************************/
unsigned char set_usb_switch_ctrl( enum usb_switch param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by SPI register 
     * 1: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by ULPI register 
     */ 
    switch( param ){
        case CONTROLED_BY_ULPI_E: 
           value = old_value | USB_SWITCH_CTRL_MASK; 
           break;
        case CONTROLED_BY_SPI_E: 
           value = old_value & ~ USB_SWITCH_CTRL_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_switch_ctrl
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_switch_ctrl()
  {
    unsigned char value;


    /* 
     * 0: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by SPI register 
     * 1: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by ULPI register 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & USB_SWITCH_CTRL_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_phy_reset
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 2:2
 *
 **************************************************************************/
unsigned char set_phy_reset( enum reset_active_low param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: PHY is reset (active low) 
     * 1: PHY is not reset 
     */ 
    switch( param ){
        case PHY_IS_NOT_RESET_E: 
           value = old_value | PHY_RESET_MASK; 
           break;
        case PHY_IS_RESET_E: 
           value = old_value & ~ PHY_RESET_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_phy_reset
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 2:2
 *
 **************************************************************************/
unsigned char get_phy_reset()
  {
    unsigned char value;


    /* 
     * 0: PHY is reset (active low) 
     * 1: PHY is not reset 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & PHY_RESET_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dp_pull_up
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 3:3
 *
 **************************************************************************/
unsigned char set_dp_pull_up( enum dp_pull_up_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DpPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    switch( param ){
        case DP_PULLUP_DISABLE_E: 
           value = old_value | DP_PULL_UP_MASK; 
           break;
        case DP_PULLUP_ENABLE_E: 
           value = old_value & ~ DP_PULL_UP_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dp_pull_up
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 3:3
 *
 **************************************************************************/
unsigned char get_dp_pull_up()
  {
    unsigned char value;


    /* 
     * 0: DpPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & DP_PULL_UP_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dm_pull_down
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 4:4
 *
 **************************************************************************/
unsigned char set_dm_pull_down( enum dm_pull_down_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull down enable 
     */ 
    switch( param ){
        case DM1_PULLDOWN_DISABLE_E: 
           value = old_value | DM_PULL_DOWN_MASK; 
           break;
        case DM1_PULLDOWN_ENABLE_E: 
           value = old_value & ~ DM_PULL_DOWN_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dm_pull_down
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 4:4
 *
 **************************************************************************/
unsigned char get_dm_pull_down()
  {
    unsigned char value;


    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull down enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & DM_PULL_DOWN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dp_pull_down
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 5:5
 *
 **************************************************************************/
unsigned char set_dp_pull_down( enum dp_pull_down_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DpMPull Down disable 
     * 1: DpM Pulldown enable 
     */ 
    switch( param ){
        case DP_PULLDOWN_DISABLE_E: 
           value = old_value | DP_PULL_DOWN_MASK; 
           break;
        case DP_PULLDOWN_ENABLE_E: 
           value = old_value & ~ DP_PULL_DOWN_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dp_pull_down
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 5:5
 *
 **************************************************************************/
unsigned char get_dp_pull_down()
  {
    unsigned char value;


    /* 
     * 0: DpMPull Down disable 
     * 1: DpM Pulldown enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & DP_PULL_DOWN_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dm_pull_up
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 6:6
 *
 **************************************************************************/
unsigned char set_dm_pull_up( enum dm_pull_up_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    switch( param ){
        case DM_PULLUP_DISABLE_E: 
           value = old_value | DM_PULL_UP_MASK; 
           break;
        case DM_PULLUP_ENABLE_E: 
           value = old_value & ~ DM_PULL_UP_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dm_pull_up
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement
 *
 * Notes : From register 0x581, bits 6:6
 *
 **************************************************************************/
unsigned char get_dm_pull_up()
  {
    unsigned char value;


    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & DM_PULL_UP_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_detection
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 0:0
 *
 **************************************************************************/
unsigned char set_usb_charger_detection( enum usb_charger_detection_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: disable USB charger detection 
     * 1: enable USB charger detection 
     */ 
    switch( param ){
        case USB_CHARGER_DETECTION_ENABLE_E: 
           value = old_value | USB_CHARGER_DETECTION_MASK; 
           break;
        case USB_CHARGER_DETECTION_DISABLE_E: 
           value = old_value & ~ USB_CHARGER_DETECTION_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_detection
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 0:0
 *
 **************************************************************************/
unsigned char get_usb_charger_detection()
  {
    unsigned char value;


    /* 
     * 0: disable USB charger detection 
     * 1: enable USB charger detection 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & USB_CHARGER_DETECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gate_sys_ulp_clock
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 4:4
 *
 **************************************************************************/
unsigned char set_gate_sys_ulp_clock( enum gate_ulp_clock param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: clock not gated 
     * 1: clock gated, sent only to audio digital 
     */ 
    switch( param ){
        case ULPCLOCK_GATED_E: 
           value = old_value | GATE_SYS_ULP_CLOCK_MASK; 
           break;
        case ULPCLOCK_NOT_GATED_E: 
           value = old_value & ~ GATE_SYS_ULP_CLOCK_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gate_sys_ulp_clock
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 4:4
 *
 **************************************************************************/
unsigned char get_gate_sys_ulp_clock()
  {
    unsigned char value;


    /* 
     * 0: clock not gated 
     * 1: clock gated, sent only to audio digital 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & GATE_SYS_ULP_CLOCK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_uart_low_power_mode
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 5:5
 *
 **************************************************************************/
unsigned char set_uart_low_power_mode( enum uart_low_power_mode_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: inactive 
     * 1: Uart low power mode is enabled 
     */ 
    switch( param ){
        case UART_LOW_POWER_MODE_ENABLE_E: 
           value = old_value | UART_LOW_POWER_MODE_MASK; 
           break;
        case UART_LOW_POWER_MODE_DISABLE_E: 
           value = old_value & ~ UART_LOW_POWER_MODE_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_uart_low_power_mode
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 5:5
 *
 **************************************************************************/
unsigned char get_uart_low_power_mode()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Uart low power mode is enabled 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & UART_LOW_POWER_MODE_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_mux_ctrl
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 6:6
 *
 **************************************************************************/
unsigned char set_charger_mux_ctrl( enum charger_mux_ctrl param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by ULPI 
     * 1: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by I2C for charger 
     */ 
    switch( param ){
        case I2C_E: 
           value = old_value | CHARGER_MUX_CTRL_MASK; 
           break;
        case UPLI_E: 
           value = old_value & ~ CHARGER_MUX_CTRL_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_mux_ctrl
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 6:6
 *
 **************************************************************************/
unsigned char get_charger_mux_ctrl()
  {
    unsigned char value;


    /* 
     * 0: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by ULPI 
     * 1: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by I2C for charger 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & CHARGER_MUX_CTRL_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dm_pull_down_disable
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 7:7
 *
 **************************************************************************/
unsigned char set_dm_pull_down_disable( enum dm_pull param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: Disable DM 19.25KOhms pull down 
     * 1: Enable DM 19.25kOhms pull down 
     */ 
  

     value =  old_value & ~DM_PULL_DOWN_DISABLE_MASK ;

    switch(  param ){

           case DM_PULLDOWN_ENABLE_E:
                value  = value  | (DM_PULLDOWN_ENABLE << 0x7);
                break;
           case DM_PULLDOWN_DISABLE_E:
                value  = value  | (DM_PULLDOWN_DISABLE << 0x7);
                break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dm_pull_down_disable
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement2
 *
 * Notes : From register 0x582, bits 7:7
 *
 **************************************************************************/
unsigned char get_dm_pull_down_disable()
  {
    unsigned char value;


    /* 
     * 0: Disable DM 19.25KOhms pull down 
     * 1: Enable DM 19.25kOhms pull down 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & DM_PULL_DOWN_DISABLE_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_idat_sink
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 0:0
 *
 **************************************************************************/
unsigned char set_idat_sink( enum idat_sink_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are disabled 
     * 1: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are enabled 
     */ 
    switch( param ){
        case IDAT_SINK_ENABLE_E: 
           value = old_value | IDAT_SINK_MASK; 
           break;
        case IDAT_SINK_DISABLE_E: 
           value = old_value & ~ IDAT_SINK_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_idat_sink
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 0:0
 *
 **************************************************************************/
unsigned char get_idat_sink()
  {
    unsigned char value;


    /* 
     * 0: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are disabled 
     * 1: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are enabled 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & IDAT_SINK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vdat_source
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 1:1
 *
 **************************************************************************/
unsigned char set_vdat_source( enum vdat_source_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference are disabled 
     * 1: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference comparators are enabled 
     */ 
    switch( param ){
        case VDAT_SOURCE_ENABLE_E: 
           value = old_value | VDAT_SOURCE_MASK; 
           break;
        case VDAT_SOURCE_DISABLE_E: 
           value = old_value & ~ VDAT_SOURCE_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vdat_source
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 1:1
 *
 **************************************************************************/
unsigned char get_vdat_source()
  {
    unsigned char value;


    /* 
     * 0: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference are disabled 
     * 1: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference comparators are enabled 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & VDAT_SOURCE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sdp_src_switch
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 2:2
 *
 **************************************************************************/
unsigned char set_sdp_src_switch( enum sdp_src_switch_opened param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDP_SRC USB switch is opened 
     * 1: SDP_SRC USB switch is closed 
     */ 
    switch( param ){
        case SDP_SRC_SWITCH_OPEN_E: 
           value = old_value | SDP_SRC_SWITCH_MASK; 
           break;
        case SDP_SRC_SWITCH_CLOSE_E: 
           value = old_value & ~ SDP_SRC_SWITCH_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sdp_src_switch
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 2:2
 *
 **************************************************************************/
unsigned char get_sdp_src_switch()
  {
    unsigned char value;


    /* 
     * 0: SDP_SRC USB switch is opened 
     * 1: SDP_SRC USB switch is closed 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & SDP_SRC_SWITCH_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sdm_src_switch
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 3:3
 *
 **************************************************************************/
unsigned char set_sdm_src_switch( enum sdm_src_switch_opened param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDM_SRC USB switch is opened 
     * 1: SDM_SRC USB switch is closed 
     */ 
    switch( param ){
        case SDM_SRC_SWITCH_OPEN_E: 
           value = old_value | SDM_SRC_SWITCH_MASK; 
           break;
        case SDM_SRC_SWITCH_CLOSE_E: 
           value = old_value & ~ SDM_SRC_SWITCH_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sdm_src_switch
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 3:3
 *
 **************************************************************************/
unsigned char get_sdm_src_switch()
  {
    unsigned char value;


    /* 
     * 0: SDM_SRC USB switch is opened 
     * 1: SDM_SRC USB switch is closed 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & SDM_SRC_SWITCH_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sdp_snk_switch
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 4:4
 *
 **************************************************************************/
unsigned char set_sdp_snk_switch( enum sdp_snk_switch_opened param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDP_SNK USB switch is opened 
     * 1: SDP_SNK USB switch is closed 
     */ 
    switch( param ){
        case SDP_SNK_SWITCH_OPEN_E: 
           value = old_value | SDP_SNK_SWITCH_MASK; 
           break;
        case SDP_SNK_SWITCH_CLOSE_E: 
           value = old_value & ~ SDP_SNK_SWITCH_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sdp_snk_switch
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 4:4
 *
 **************************************************************************/
unsigned char get_sdp_snk_switch()
  {
    unsigned char value;


    /* 
     * 0: SDP_SNK USB switch is opened 
     * 1: SDP_SNK USB switch is closed 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & SDP_SNK_SWITCH_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sdm_snk_switch
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 5:5
 *
 **************************************************************************/
unsigned char set_sdm_snk_switch( enum sdm_snk_switch_opened param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDM_SNK USB switch is opened 
     * 1: SDM_SNK USB switch is closed 
     */ 
    switch( param ){
        case SDM_SNK_SWITCH_OPEN_E: 
           value = old_value | SDM_SNK_SWITCH_MASK; 
           break;
        case SDM_SNK_SWITCH_CLOSE_E: 
           value = old_value & ~ SDM_SNK_SWITCH_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sdm_snk_switch
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 5:5
 *
 **************************************************************************/
unsigned char get_sdm_snk_switch()
  {
    unsigned char value;


    /* 
     * 0: SDM_SNK USB switch is opened 
     * 1: SDM_SNK USB switch is closed 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & SDM_SNK_SWITCH_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_data_connect_source
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 6:6
 *
 **************************************************************************/
unsigned char set_data_connect_source( enum data_connect_source_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: Data Connect source is disabled 
     * 1:Data Connect source is enable 
     */ 
    switch( param ){
        case DATA_CONNECT_SOURCE_ENABLE_E: 
           value = old_value | DATA_CONNECT_SOURCE_MASK; 
           break;
        case DATA_CONNECT_SOURCE_DISABLE_E: 
           value = old_value & ~ DATA_CONNECT_SOURCE_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_data_connect_source
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 6:6
 *
 **************************************************************************/
unsigned char get_data_connect_source()
  {
    unsigned char value;


    /* 
     * 0: Data Connect source is disabled 
     * 1:Data Connect source is enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & DATA_CONNECT_SOURCE_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_data_connect_detection
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 7:7
 *
 **************************************************************************/
unsigned char set_data_connect_detection( enum data_connect_detection_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: inactive 
     * 1: force 60 Mhz PLL on 
     */ 
    switch( param ){
        case DATA_CONNECT_DETECTION_ENABLE_E: 
           value = old_value | DATA_CONNECT_DETECTION_MASK; 
           break;
        case DATA_CONNECT_DETECTION_DISABLE_E: 
           value = old_value & ~ DATA_CONNECT_DETECTION_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_data_connect_detection
 *
 * RET  : Return the value of register 0xUsbPhyMmanagement3
 *
 * Notes : From register 0x583, bits 7:7
 *
 **************************************************************************/
unsigned char get_data_connect_detection()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: force 60 Mhz PLL on 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & DATA_CONNECT_DETECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_device_detection
 *
 * IN   : param, a value to write to the regiter UsbPhyAdaptation
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyAdaptation
 *
 * Notes : From register 0x587, bits 0:0
 *
 **************************************************************************/
unsigned char set_device_detection( enum detection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable ID Device detection 
     * 1: Enable ID Device detection(default) 
     */ 
  

     value =  old_value & ~DEVICE_DETECTION_MASK ;

    switch(  param ){

           case ENABLE_DETECTION_E:
                value =  value | ENABLE_DETECTION ;
                break;
           case DISABLE_DETECTION_E:
                value =  value | DISABLE_DETECTION ;
                break;
    }
  

    I2CWrite(USB_PHY_ADAPTATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_device_detection
 *
 * RET  : Return the value of register 0xUsbPhyAdaptation
 *
 * Notes : From register 0x587, bits 0:0
 *
 **************************************************************************/
unsigned char get_device_detection()
  {
    unsigned char value;


    /* 
     * 0: Disable ID Device detection 
     * 1: Enable ID Device detection(default) 
     */ 
    value = (I2CRead(USB_PHY_ADAPTATION_REG) & DEVICE_DETECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_host_detection
 *
 * IN   : param, a value to write to the regiter UsbPhyAdaptation
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyAdaptation
 *
 * Notes : From register 0x587, bits 1:1
 *
 **************************************************************************/
unsigned char set_id_host_detection( enum detection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable ID Host detection 
     * 1: Enable ID Host detection 
     */ 
  

     value =  old_value & ~ID_HOST_DETECTION_MASK ;

    switch(  param ){

           case ENABLE_DETECTION_E:
                value  = value  | (ENABLE_DETECTION << 0x1);
                break;
           case DISABLE_DETECTION_E:
                value  = value  | (DISABLE_DETECTION << 0x1);
                break;
    }
  

    I2CWrite(USB_PHY_ADAPTATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_host_detection
 *
 * RET  : Return the value of register 0xUsbPhyAdaptation
 *
 * Notes : From register 0x587, bits 1:1
 *
 **************************************************************************/
unsigned char get_id_host_detection()
  {
    unsigned char value;


    /* 
     * 0: Disable ID Host detection 
     * 1: Enable ID Host detection 
     */ 
    value = (I2CRead(USB_PHY_ADAPTATION_REG) & ID_HOST_DETECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_valid
 *
 * IN   : param, a value to write to the regiter UsbPhyAdaptation
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyAdaptation
 *
 * Notes : From register 0x587, bits 5:5
 *
 **************************************************************************/
unsigned char set_vbus_valid( enum valid_compatrator_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable VBUS Valid comparator 
     * 1: Enable VBUS Valid comparator 
     */ 
    switch( param ){
        case VALID_COMPATRATOR_ENABLE_E: 
           value = old_value | VBUS_VALID_MASK; 
           break;
        case VALID_COMPATRATOR_DISABLE_E: 
           value = old_value & ~ VBUS_VALID_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_ADAPTATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_valid
 *
 * RET  : Return the value of register 0xUsbPhyAdaptation
 *
 * Notes : From register 0x587, bits 5:5
 *
 **************************************************************************/
unsigned char get_vbus_valid()
  {
    unsigned char value;


    /* 
     * 0: Disable VBUS Valid comparator 
     * 1: Enable VBUS Valid comparator 
     */ 
    value = (I2CRead(USB_PHY_ADAPTATION_REG) & VBUS_VALID_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_wakeup
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 0:0
 *
 **************************************************************************/
unsigned char set_id_wakeup( enum resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is floating (no micro-A plug detected) 
     * 1: ID resistance is grounded (micro-A plug detected)k 
     */ 
    switch( param ){
        case GROUNDED_E: 
           value = old_value | ID_WAKEUP_MASK; 
           break;
        case FLOATING_E: 
           value = old_value & ~ ID_WAKEUP_MASK;
           break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_wakeup
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 0:0
 *
 **************************************************************************/
unsigned char get_id_wakeup()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is floating (no micro-A plug detected) 
     * 1: ID resistance is grounded (micro-A plug detected)k 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & ID_WAKEUP_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det_r1
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 1:1
 *
 **************************************************************************/
unsigned char set_id_det_r1( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is above R1 
     * 1: ID resistance is below R1 
     */ 
  

     value =  old_value & ~ID_DET_R1_MASK ;

    switch(  param ){

           case BELOW_E:
                value  = value  | (BELOW << 0x1);
                break;
           case ABOVE_E:
                value  = value  | (ABOVE << 0x1);
                break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det_r1
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 1:1
 *
 **************************************************************************/
unsigned char get_id_det_r1()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is above R1 
     * 1: ID resistance is below R1 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & ID_DET_R1_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det_r2
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 2:2
 *
 **************************************************************************/
unsigned char set_id_det_r2( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is above R2 
     * 1: ID resistance is below R2 
     */ 
  

     value =  old_value & ~ID_DET_R2_MASK ;

    switch(  param ){

           case BELOW_E:
                value  = value  | (BELOW << 0x2);
                break;
           case ABOVE_E:
                value  = value  | (ABOVE << 0x2);
                break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det_r2
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 2:2
 *
 **************************************************************************/
unsigned char get_id_det_r2()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is above R2 
     * 1: ID resistance is below R2 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & ID_DET_R2_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det_r3
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 3:3
 *
 **************************************************************************/
unsigned char set_id_det_r3( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is above R3 
     * 1: ID resistance is below R3 
     */ 
  

     value =  old_value & ~ID_DET_R3_MASK ;

    switch(  param ){

           case BELOW_E:
                value  = value  | (BELOW << 0x3);
                break;
           case ABOVE_E:
                value  = value  | (ABOVE << 0x3);
                break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det_r3
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 3:3
 *
 **************************************************************************/
unsigned char get_id_det_r3()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is above R3 
     * 1: ID resistance is below R3 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & ID_DET_R3_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det_r4
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 4:4
 *
 **************************************************************************/
unsigned char set_id_det_r4( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is above R4 
     * 1: ID resistance is below R4 
     */ 
  

     value =  old_value & ~ID_DET_R4_MASK ;

    switch(  param ){

           case BELOW_E:
                value  = value  | (BELOW << 0x4);
                break;
           case ABOVE_E:
                value  = value  | (ABOVE << 0x4);
                break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det_r4
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 4:4
 *
 **************************************************************************/
unsigned char get_id_det_r4()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is above R4 
     * 1: ID resistance is below R4 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & ID_DET_R4_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_drive_vbus
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 5:5
 *
 **************************************************************************/
unsigned char set_drive_vbus( enum vbus_generation_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: 5V generation is disabled 
     * 1: 5V generation is enabled 
     */ 
    switch( param ){
        case VBUS_GENERATION_ENABLE_E: 
           value = old_value | DRIVE_VBUS_MASK; 
           break;
        case VBUS_GENERATION_DISABLE_E: 
           value = old_value & ~ DRIVE_VBUS_MASK;
           break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_drive_vbus
 *
 * RET  : Return the value of register 0xOtgIdResistance
 *
 * Notes : From register 0x588, bits 5:5
 *
 **************************************************************************/
unsigned char get_drive_vbus()
  {
    unsigned char value;


    /* 
     * 0: 5V generation is disabled 
     * 1: 5V generation is enabled 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & DRIVE_VBUS_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_suspend_mode
 *
 * RET  : Return the value of register 0xUsbPhyStatus
 *
 * Notes : From register 0x589, bits 0:0
 *
 **************************************************************************/
unsigned char get_suspend_mode()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(USB_PHY_STATUS_REG) & SUSPEND_MODE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_chirp_mode
 *
 * RET  : Return the value of register 0xUsbPhyStatus
 *
 * Notes : From register 0x589, bits 1:1
 *
 **************************************************************************/
unsigned char get_chirp_mode()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(USB_PHY_STATUS_REG) & CHIRP_MODE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_high_speed_mode
 *
 * RET  : Return the value of register 0xUsbPhyStatus
 *
 * Notes : From register 0x589, bits 2:2
 *
 **************************************************************************/
unsigned char get_high_speed_mode()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(USB_PHY_STATUS_REG) & HIGH_SPEED_MODE_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_host_mode
 *
 * IN   : param, a value to write to the regiter UsbPhyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyControl
 *
 * Notes : From register 0x58A, bits 0:0
 *
 **************************************************************************/
unsigned char set_usb_host_mode( enum usb_host_mode_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_CONTROL_REG);

    /* 
     * 0: doesn't turn on USB PHY 
     * 1: turn on USB PHY in host mode 
     */ 
    switch( param ){
        case USB_HOST_MODE_ENABLE_E: 
           value = old_value | USB_HOST_MODE_MASK; 
           break;
        case USB_HOST_MODE_DISABLE_E: 
           value = old_value & ~ USB_HOST_MODE_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_host_mode
 *
 * RET  : Return the value of register 0xUsbPhyControl
 *
 * Notes : From register 0x58A, bits 0:0
 *
 **************************************************************************/
unsigned char get_usb_host_mode()
  {
    unsigned char value;


    /* 
     * 0: doesn't turn on USB PHY 
     * 1: turn on USB PHY in host mode 
     */ 
    value = (I2CRead(USB_PHY_CONTROL_REG) & USB_HOST_MODE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_device_mode
 *
 * IN   : param, a value to write to the regiter UsbPhyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbPhyControl
 *
 * Notes : From register 0x58A, bits 1:1
 *
 **************************************************************************/
unsigned char set_usb_device_mode( enum usb_device_mode_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_CONTROL_REG);

    /* 
     * 0: doesn't turn on USB PHY 
     * 1: turn on USB PHY in Device mode (turn on sequence of 
     * USB PHY with no Vbus generation) 
     */ 
    switch( param ){
        case USB_DEVICE_MODE_ENABLE_E: 
           value = old_value | USB_DEVICE_MODE_MASK; 
           break;
        case USB_DEVICE_MODE_DISABLE_E: 
           value = old_value & ~ USB_DEVICE_MODE_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_device_mode
 *
 * RET  : Return the value of register 0xUsbPhyControl
 *
 * Notes : From register 0x58A, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_device_mode()
  {
    unsigned char value;


    /* 
     * 0: doesn't turn on USB PHY 
     * 1: turn on USB PHY in Device mode (turn on sequence of 
     * USB PHY with no Vbus generation) 
     */ 
    value = (I2CRead(USB_PHY_CONTROL_REG) & USB_DEVICE_MODE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_time_two_thresholds_lsb
 *
 * RET  : Return the value of register 0xTimeTwoThresholdsLsb
 *
 * Notes : From register 0x591, bits 7:0
 *
 **************************************************************************/
unsigned char get_time_two_thresholds_lsb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(TIME_TWO_THRESHOLDS_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_time_two_thresholds_lsb
 *
 * IN   : param, a value to write to the regiter TimeTwoThresholdsLsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xTimeTwoThresholdsLsb
 *
 * Notes : From register 0x591, bits 7:0
 *
 **************************************************************************/
unsigned char set_time_two_thresholds_lsb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(TIME_TWO_THRESHOLDS_LSB_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(TIME_TWO_THRESHOLDS_LSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_usb_adp_probe_time_current
 *
 * IN   : param, a value to write to the regiter TimeTwoThresholdsMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xTimeTwoThresholdsMsb
 *
 * Notes : From register 0x592, bits 0:0
 *
 **************************************************************************/
unsigned char set_usb_adp_probe_time_current( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TIME_TWO_THRESHOLDS_MSB_REG);

    /* 
     * Time between two thresholds: UsbAdpTimeCurrent[10:0] x 
     * Clk32kHz period (max about 63.9ms) 
     */ 
  
    value =  old_value & ~USB_ADP_PROBE_TIME_CURRENT_MASK;


    value |=  param ;  

    I2CWrite(TIME_TWO_THRESHOLDS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_adp_probe_time_current
 *
 * RET  : Return the value of register 0xTimeTwoThresholdsMsb
 *
 * Notes : From register 0x592, bits 0:0
 *
 **************************************************************************/
unsigned char get_usb_adp_probe_time_current()
  {
    unsigned char value;


    /* 
     * Time between two thresholds: UsbAdpTimeCurrent[10:0] x 
     * Clk32kHz period (max about 63.9ms) 
     */ 
    value = (I2CRead(TIME_TWO_THRESHOLDS_MSB_REG) & USB_ADP_PROBE_TIME_CURRENT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_adp_probe_time
 *
 * IN   : param, a value to write to the regiter UsbAdpProbe
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbAdpProbe
 *
 * Notes : From register 0x593, bits 3:1
 *
 **************************************************************************/
unsigned char set_adp_probe_time( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_ADP_PROBE_REG);

    /* 
     * Threshold tolerance of AdpProbeTimeCurrent[10:0] 
     * measurement 
     */ 
  
    value =  old_value & ~ADP_PROBE_TIME_MASK;


    value |= ( param << 0x1);  

    I2CWrite(USB_ADP_PROBE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adp_probe_time
 *
 * RET  : Return the value of register 0xUsbAdpProbe
 *
 * Notes : From register 0x593, bits 3:1
 *
 **************************************************************************/
unsigned char get_adp_probe_time()
  {
    unsigned char value;


    /* 
     * Threshold tolerance of AdpProbeTimeCurrent[10:0] 
     * measurement 
     */ 
    value = (I2CRead(USB_ADP_PROBE_REG) & ADP_PROBE_TIME_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adp
 *
 * IN   : param, a value to write to the regiter UsbAdpProbe
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbAdpProbe
 *
 * Notes : From register 0x593, bits 0:0
 *
 **************************************************************************/
unsigned char set_adp( enum adp_enabled param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_ADP_PROBE_REG);

    /* 
     * 0: inactive 
     * 1: ADP is enabled 
     */ 
    switch( param ){
        case ADP_ENABLE_E: 
           value = old_value | ADP_MASK; 
           break;
        case ADP_DISABLE_E: 
           value = old_value & ~ ADP_MASK;
           break;
    }
  

    I2CWrite(USB_ADP_PROBE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adp
 *
 * RET  : Return the value of register 0xUsbAdpProbe
 *
 * Notes : From register 0x593, bits 0:0
 *
 **************************************************************************/
unsigned char get_adp()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: ADP is enabled 
     */ 
    value = (I2CRead(USB_ADP_PROBE_REG) & ADP_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_link_status
 *
 * RET  : Return the value of register 0xUsbLinkStatus
 *
 * Notes : From register 0x594, bits 7:3
 *
 **************************************************************************/
unsigned char get_usb_link_status()
  {
    unsigned char value;


    /* 
     * LinkStatus 
     */ 
    value = (I2CRead(USB_LINK_STATUS_REG) & USB_LINK_STATUS_MASK) >> 3;
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
