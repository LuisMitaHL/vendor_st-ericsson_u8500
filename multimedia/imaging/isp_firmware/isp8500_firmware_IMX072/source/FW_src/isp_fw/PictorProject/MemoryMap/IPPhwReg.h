/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef _IPPhwReg_
#define _IPPhwReg_

#include "Platform.h"



//IPP_DPHY_TOP_IF_EN
/*Description: Bit register to enable/disable the SoC top-level DPHYs, both in CSI and CCP2 SubLVDS modes.

When high, the DPHYs are allowed to generate input sensor/camera data and clocks to the SIA module.

The DPHYs must be gated i.e. IPP_DPHY_TOP_IF_EN=0 when any IPP static register (IPP_XX_STATIC_XX) is written, unless the IPP is in DDR load memory mode only.

This register is used only by the SoC top-level DPHYs, but never by the SIA internally. Note it is under the responsability of the DPHYs to safely de-assert the enabled DPHYs from this asynchronous signal.

Hard reset value is 0, i.e. disabled DPHYs..
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t DPHY_TOP_IF_EN :1;
    }IPP_DPHY_TOP_IF_EN_ts;

}IPP_DPHY_TOP_IF_EN_tu;



//IPP_STATIC_TOP_IF_SEL
/*Description: Static register to select the camera data interface.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.

This register is used both by the external DPHYs and by the IPP modules CCP_IF, CRM, SDG, and CDG.

Hard reset is 0, i.e. selection of CSI0 primary interface.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_TOP_IF_SEL :2;
        /* 
        1x: CCP (secondary interface)*/
    }IPP_STATIC_TOP_IF_SEL_ts;

}IPP_STATIC_TOP_IF_SEL_tu;



//IPP_STATIC_CCP_IF
/*Description: Static register to control external Sublvds rx pads, clock/data recovery i.e. DS-decoders which are instanciated nearby pads at Soc level inside the DPHYs. But IPP_CCP_IF is used also internally by CCP_IF, SDG, and CDG to configure accordingly IPP.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.

Defaut value is 0x800, ie RAW8, data/stobe mode.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CCP_DSCLK :1;
        /* data/strobe*/
        uint8_t reserved0 :7;
        uint8_t STATIC_CCP_BPP :4;
        /* 
        valid values are 6,7,8,10,12*/
    }IPP_STATIC_CCP_IF_ts;

}IPP_STATIC_CCP_IF_tu;



//IPP_INTERNAL_EN_CLK_CCP
/*Description: Register to gate internally to the IPP core, inside the CCP_IF and CRM modules, the CCP sensor clock source both to the sensor datapath and the camera datapath. This ensures no glitch on the DPHY clock is propagated to SDG during the transition from 'streaming' to 'SW standby' of the CCP camera module.

On a sensor power-up sequence, the FW must :

On a sensor power-down sequence, the FW must:

Defaut value is 0x0, ie disabled ccp clock..
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t INTERNAL_EN_CLK_CCP :1;
        /* disabled*/
    }IPP_INTERNAL_EN_CLK_CCP_ts;

}IPP_INTERNAL_EN_CLK_CCP_tu;



//IPP_DPHY_COMPENSATION_CTRL
/*Description: This register is common for all DPHYs, wheter it is for MIPI CSI2 channels CSI#0 or CSI#1, or for SMIA CCP2.

It is used to start the DPHY compensation, either in a self-calibration mode when calib_write_en=0, or with an explicit FW written calibration value calib_write_code when calb_write_en=1.

Default value is 0, i.e. compensation not started.

After reset, FW should first write the register field CLK_1_5M_DPHY_COMPENSATION_EN=1 to enable the clock needed by the compensation clock. This clock should be present in the SoC top-level, with a frequence value between 1 and 5 MHz.

Then compensation DPHY should be started by a FW setting of the register field dphy_compensation_start=1.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CLK_1_5M_COMPENSATION_EN :1;
        uint8_t DPHY_COMPENSATION_START :1;
        uint8_t DPHY_COMPENSATION_CALIB_WRITE_EN :1;
        uint8_t DPHY_COMPENSATION_CALIB_WRITE_CODE :3;
    }IPP_DPHY_COMPENSATION_CTRL_ts;

}IPP_DPHY_COMPENSATION_CTRL_tu;



//IPP_DPHY_COMPENSATION_STATUS
/*Description: This register is common for all DPHYs, wheter it is for MIPI CSI2 channels CSI#0 or CSI#1, or for SMIA CCP2. Once compensation is started, FW should poll this register until that it indicates that compensation is completed (COMPENSATION_OK=1).

Then used calibration codes are available for read if needed.

Then FW should gate the Soc top-level clock between 1 and 5 MHz by setting the field CLK_1_5M_DPHY_COMPENSATION_EN=0
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t DPHY_COMPENSATION_OK :1;
        uint8_t DPHY_COMPENSATION_CALIB_READ_CODE :3;
        uint8_t DPHY_COMPENSATION_DBG_COMP_FLAG :1;
    }IPP_DPHY_COMPENSATION_STATUS_ts;

}IPP_DPHY_COMPENSATION_STATUS_tu;



//IPP_STATIC_CSI0_DATA_LANES
/*Description: Static register for sensor or camera, to set the IPP configuration for the CSI#0 interface with the relevant data lanes map and number.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.

Default value is 0xD12, i.e. two data lanes, without any swap on data lanes.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI0_DATA_LANES_NB :3;
        /* set the number of data lanes one, two, three or four. This field is used by the CSM/csi2 clock manager to generate a relevant CSI0-driven pixel clock, and by the CSI2RX0 lane merger.*/
        uint8_t reserved0 :1;
        uint16_t STATIC_CSI0_DATA_LANES_MAP :12;
        /* select and map the 4 output physical DPHY data lanes onto the 4 input logical CSI2RX0 data lanes before merging. More explicitely, e.g. the 3bit-lsb value is set to select the DPHY physical lane that will be connected to the input of the CSI2RX0 logical data lane 1, and the 3bit-msb value is set ot select the DPHY physical lane that will be connected to the input of the CSI2RX0 logical data lane 4.
        100_011_010_001: (4,3,2,1) (default map) 100_011_001_010: (4,3,1,2)
        100_010_011_001: (4,2,3,1) 100_010_001_011: (4,2,1,3)
        100_001_011_010: (4,1,3,2) 100_001_010_011: (4,1,2,3)
        011_100_010_001: (3,4,2,1) 011_100_001_010: (3,4,1,2)
        011_010_100_011: (3,2,4,1) 011_010_001_100: (3,2,1,4)
        011_001_100_010: (3,1,4,2) 011_001_010_100: (3,1,2,4)
        010_100_011_001: (2,4,3,1) 010_100_011_001: (2,4,1,3)
        010_011_100_001: (2,3,4,1) 010_011_001_100: (2,3,1,4)
        010_001_100_011: (2,1,4,3) 010_001_011_100: (2,1,3,4)
        001_100_011_010: (1,4,3,2) 001_100_010_011: (1,4,2,3)
        001_011_100_010: (1,3,4,2) 001_011_010_100: (1,3,2,4)
        001_010_100_011: (1,2,4,3) 001_010_011_100: (1,2,3,4)
        others: reserved
        Note: 3b000 triplet will be interpreted as 3b100 to ensure binary compatibility, ie: 100_011_010_001 is equivalent to 000_011_010_001*/
    }IPP_STATIC_CSI0_DATA_LANES_ts;

}IPP_STATIC_CSI0_DATA_LANES_tu;


//IPP_CSI2_DPHY0_CL_CTRL
/*Description: drives signals to be connected to CSI2 DPHY0 clock lane functional inputs.Defaut value is 0x128 i.e. no impedance tuning, no shift up/down of HS RX termination, no high speed clock lane inversion, no P/N swap pin, MIPI DPHY specification v0.90, and a 100Mbps but discarded sensor data rate setting.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_UI_X4 :6;
        /* set according to the sensor data bitrate, in Mbps. Given N is the Mbps, formula is csi0_ui_x4= round_down(1000/N *4)
        e.g a N=600 Mbps is given by a csi0_ui_x4=round_down(1000/600*4)=round_down(6.67)=6.*/
        uint8_t reserved0 :2;
        uint8_t CSI0_SPECS_90_81B :1;
        /* set the IP mode wrt the MIPI DPHY specification (1: v0.90, 0: v0.81)*/
        uint8_t CSI0_SWAP_PINS_CL :1;
        /* set to invert P and N pins both for high-speed and low power CSI2 modes. In CCP2 SMIA mode, it is set to invert clk/strobe edge fro data sampling. (low: falling edge, high:*/
        uint8_t CSI0_HS_INVERT_CL :1;
        /* set to invert only P and N pins for high speed*/
        uint8_t CSI0_HSRX_TERM_SHIFT_UP_CL :1;
        /* set to shift up the value of HS RX termination*/
        uint8_t CSI0_HSRX_TERM_SHIFT_DOWN_CL :1;
        /* set to shift down the value of HS RX termination*/
        uint8_t CSI0_TEST_RESERVED_1_CL :1;
        /* set for impedance tuning*/
    }IPP_CSI2_DPHY0_CL_CTRL_ts;

}IPP_CSI2_DPHY0_CL_CTRL_tu;



//IPP_CSI2_DPHY0_DL1_CTRL
/*Description: drives signals to be connected to CSI2 DPHY0 data lane 1 functional inputs.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_SWAP_PINS_DL1 :1;
        uint8_t CSI0_HS_INVERT_DL1 :1;
        uint8_t CSI0_FORCE_RX_MODE_DL1 :1;
        uint8_t CSI0_CD_OFF_DL1 :1;
        uint8_t CSI0_EOT_BYPASS_DL1 :1;
        uint8_t CSI0_HSRX_TERM_SHIFT_UP_DL1 :1;
        uint8_t CSI0_HSRX_TERM_SHIFT_DOWN_DL1 :1;
        uint8_t CSI0_TEST_RESERVED_1_DL1 :1;
    }IPP_CSI2_DPHY0_DL1_CTRL_ts;

}IPP_CSI2_DPHY0_DL1_CTRL_tu;



//IPP_CSI2_DPHY0_DL2_CTRL
/*Description: drives signals to be connected to CSI2 DPHY0 data lane 2 functional inputs.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_SWAP_PINS_DL2 :1;
        uint8_t CSI0_HS_INVERT_DL2 :1;
        uint8_t CSI0_FORCE_RX_MODE_DL2 :1;
        uint8_t CSI0_CD_OFF_DL2 :1;
        uint8_t CSI0_EOT_BYPASS_DL2 :1;
        uint8_t CSI0_HSRX_TERM_SHIFT_UP_DL2 :1;
        uint8_t CSI0_HSRX_TERM_SHIFT_DOWN_DL2 :1;
        uint8_t CSI0_TEST_RESERVED_1_DL2 :1;
    }IPP_CSI2_DPHY0_DL2_CTRL_ts;

}IPP_CSI2_DPHY0_DL2_CTRL_tu;



//IPP_CSI2_DPHY0_DL3_CTRL
/*Description: drives signals to be connected to CSI2 DPHY0 data lane 3 functional inputs.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_SWAP_PINS_DL3 :1;
        uint8_t CSI0_HS_INVERT_DL3 :1;
        uint8_t CSI0_FORCE_RX_MODE_DL3 :1;
        uint8_t CSI0_CD_OFF_DL3 :1;
        uint8_t CSI0_EOT_BYPASS_DL3 :1;
        uint8_t CSI0_HSRX_TERM_SHIFT_UP_DL3 :1;
        uint8_t CSI0_HSRX_TERM_SHIFT_DOWN_DL3 :1;
        uint8_t CSI0_TEST_RESERVED_1_DL3 :1;
    }IPP_CSI2_DPHY0_DL3_CTRL_ts;

}IPP_CSI2_DPHY0_DL3_CTRL_tu;



//IPP_CSI2_DPHY0_CL_DBG
/*Description: controls DPHY0 clock lane debug inputs pins.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_HS_RX_OFFSET_CL :3;
        uint8_t CSI0_MIPI_IN_SHORT_CL :1;
        uint8_t reserved0 :4;
        uint8_t CSI0_LP_HS_BYPASS_CL :1;
        uint8_t CSI0_LP_RX_VIL_CL :2;
        uint8_t CSI0_DIRECT_DYN_ACCESS_CL :1;
    }IPP_CSI2_DPHY0_CL_DBG_ts;

}IPP_CSI2_DPHY0_CL_DBG_tu;



//IPP_CSI2_DPHY0_DL1_DBG
/*Description: controls DPHY0 data lane 1 debug inputs pins.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_HS_RX_OFFSET_DL1 :3;
        uint8_t CSI0_MIPI_IN_SHORT_DL1 :1;
        uint8_t CSI0_SKEW_DL1 :3;
        uint8_t CSI0_OVERSAMPLE_BYPASS_DL1 :1;
        uint8_t CSI0_LP_HS_BYPASS_DL1 :1;
        uint8_t CSI0_LP_RX_VIL_DL1 :2;
        uint8_t CSI0_DIRECT_DYN_ACCES_DL1 :1;
    }IPP_CSI2_DPHY0_DL1_DBG_ts;

}IPP_CSI2_DPHY0_DL1_DBG_tu;



//IPP_CSI2_DPHY0_DL2_DBG
/*Description: controls DPHY0 data lane 2 debug inputs pins.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_HS_RX_OFFSET_DL2 :3;
        uint8_t CSI0_MIPI_IN_SHORT_DL2 :1;
        uint8_t CSI0_SKEW_DL2 :3;
        uint8_t CSI0_OVERSAMPLE_BYPASS_DL2 :1;
        uint8_t CSI0_LP_HS_BYPASS_DL2 :1;
        uint8_t CSI0_LP_RX_VIL_DL2 :2;
        uint8_t CSI0_DIRECT_DYN_ACCESS_DL2 :1;
    }IPP_CSI2_DPHY0_DL2_DBG_ts;

}IPP_CSI2_DPHY0_DL2_DBG_tu;



//IPP_CSI2_DPHY0_DL3_DBG
/*Description: controls DPHY0 data lane 3 debug inputs pins.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_HS_RX_OFFSET_DL3 :3;
        uint8_t CSI0_MIPI_IN_SHORT_DL3 :1;
        uint8_t CSI0_SKEW_DL3 :3;
        uint8_t CSI0_OVERSAMPLE_BYPASS_DL3 :1;
        uint8_t CSI0_LP_HS_BYPASS_DL3 :1;
        uint8_t CSI0_LP_RX_VIL_DL3 :2;
        uint8_t CSI0_DIRECT_DYN_ACCESS_DL3 :1;
    }IPP_CSI2_DPHY0_DL3_DBG_ts;

}IPP_CSI2_DPHY0_DL3_DBG_tu;



//IPP_CSI2_DPHY0_XL_DBG0_OBS
/*Description: enables observation of DPHY0 clock and data lanes debug outputs pins.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_OVERSAMPLE_FLAG1_DL1 :1;
        uint8_t CSI0_OVERSAMPLE_FLAG1_DL2 :1;
        uint8_t CSI0_OVERSAMPLE_FLAG1_DL3 :1;
        uint8_t CSI0_OVERSAMPLE_FLAG2_DL1 :1;
        uint8_t CSI0_OVERSAMPLE_FLAG2_DL2 :1;
        uint8_t CSI0_OVERSAMPLE_FLAG2_DL3 :1;
        uint8_t CSI0_HS_RX_ZM_CL :1;
        uint8_t CSI0_HS_RX_ZM_DL1 :1;
        uint8_t CSI0_HS_RX_ZM_DL2 :1;
        uint8_t CSI0_HS_RX_ZM_DL3 :1;
        uint8_t CSI0_RX_INIT_CLK :1;
        uint8_t CSI0_OVERSAMPLE_FLAG1_DL4 :1;
        uint8_t CSI0_OVERSAMPLE_FLAG2_DL4 :1;
        uint8_t CSI0_HS_RX_ZM_DL4 :1;
    }IPP_CSI2_DPHY0_XL_DBG0_OBS_ts;

}IPP_CSI2_DPHY0_XL_DBG0_OBS_tu;



//IPP_CSI2_DPHY0_XL_DBG1_OBS
/*Description: enables observation of DPHY0 clock and data lanes debug outputs pins.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_STOP_STATE_CL :1;
        uint8_t CSI0_STOP_STATE_DL1 :1;
        uint8_t CSI0_STOP_STATE_DL2 :1;
        uint8_t CSI0_STOP_STATE_DL3 :1;
        uint8_t CSI0_ULP_ACTIVE_NOT_CL :1;
        uint8_t CSI0_ULP_ACTIVE_NOT_DL1 :1;
        uint8_t CSI0_ULP_ACTIVE_NOT_DL2 :1;
        uint8_t CSI0_ULP_ACTIVE_NOT_DL3 :1;
        uint8_t CSI0_STOP_STATE_DL4 :1;
        uint8_t CSI0_ULP_ACTIVE_NOT_DL4 :1;
    }IPP_CSI2_DPHY0_XL_DBG1_OBS_ts;

}IPP_CSI2_DPHY0_XL_DBG1_OBS_tu;



//IPP_CSI2_DPHY1_CL_CTRL
/*Description: drives signals to be connected to CSI2 DPHY1 clock lane functional inputs. Defaut value is 0x128 i.e. no impedance tuning, no shift up/down of HS RX termination, MIPI DPHY spec v0.90, no high speed clock lane inversion, no P/N swap pin, and a discarded 100 Mbps sensor data rate.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_UI_X4 :6;
        /* set according to the sensor data bitrate, in Mbps. Given a N is the Mbps, formula is csi0_ui_x4= round_down(1000/N *4).
        For example a N=600 Mbps is given by a csi1_ui_x4=round_down(1000/600*4)=round_down(6.67)=6.*/
        uint8_t reserved0 :2;
        uint8_t CSI1_SPECS_90_81B :1;
        /* set the IP mode wrt the MIPI DPHY specification (1: v0.90, 0: v0.81)*/
        uint8_t CSI1_SWAP_PINS_CL :1;
        /* set to invert P and N pins both for high-speed and low power CSI2 modes. In CCP2 SMIA mode, it is set to invert clk/strobe edge fro data sampling. (low: rising edge, high: falling edge).*/
        uint8_t CSI1_HS_INVERT_CL :1;
        uint8_t CSI1_HSRX_TERM_SHIFT_UP_CL :1;
        /* set to shift up the value of HS RX termination*/
        uint8_t CSI1_HSRX_TERM_SHIFT_DOWN_CL :1;
        /* set to shift down the value of HS RX termination*/
        uint8_t CSI1_TEST_RESERVED_1_CL :1;
        /* set for impedance tuning*/
    }IPP_CSI2_DPHY1_CL_CTRL_ts;

}IPP_CSI2_DPHY1_CL_CTRL_tu;



//IPP_CSI2_DPHY1_DL1_CTRL
/*Description: drives signals to be connected to CSI2 DPHY1 single data lane functional inputs.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_SWAP_PINS_DL1 :1;
        uint8_t CSI1_HS_INVERT_DL1 :1;
        uint8_t CSI1_FORCE_RX_MODE_DL1 :1;
        uint8_t CSI1_CD_OFF_DL1 :1;
        uint8_t CSI1_EOT_BYPASS_DL1 :1;
        uint8_t CSI1_HSRX_TERM_SHIFT_UP_DL1 :1;
        uint8_t CSI1_HSRX_TERM_SHIFT_DOWN_DL1 :1;
        uint8_t CSI1_TEST_RESERVED_1_DL1 :1;
    }IPP_CSI2_DPHY1_DL1_CTRL_ts;

}IPP_CSI2_DPHY1_DL1_CTRL_tu;



//IPP_CSI2_DPHY1_CL_DBG
/*Description: controls DPHY1 clock lane debug inputs pins.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_HS_RX_OFFSET_CL :3;
        uint8_t CSI1_MIPI_IN_SHORT_CL :1;
        uint8_t reserved0 :4;
        uint8_t CSI1_LP_HS_BYPASS_CL :1;
        uint8_t CSI1_LP_RX_VIL_CL :2;
        uint8_t CSI1_DIRECT_DYN_ACCESS_CL :1;
    }IPP_CSI2_DPHY1_CL_DBG_ts;

}IPP_CSI2_DPHY1_CL_DBG_tu;



//IPP_CSI2_DPHY1_DL1_DBG
/*Description: controls DPHY1 single data lane debug inputs pins.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_HS_RX_OFFSET_DL1 :3;
        uint8_t CSI1_MIPI_IN_SHORT_DL1 :1;
        uint8_t CSI1_SKEW_DL1 :3;
        uint8_t CSI1_OVERSAMPLE_BYPASS_DL1 :1;
        uint8_t CSI1_LP_HS_BYPASS_DL1 :1;
        uint8_t CSI1_LP_RX_VIL_DL1 :2;
        uint8_t CSI1_DIRECT_DYN_ACCESS_DL1 :1;
        uint8_t CSI1_DLY_CTRL_SUBL_DL1 :3;
    }IPP_CSI2_DPHY1_DL1_DBG_ts;

}IPP_CSI2_DPHY1_DL1_DBG_tu;



//IPP_CSI2_DPHY1_XL_DBG_OBS
/*Description: enables observation of DPHY1 clock and data lane debug outputs pins.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_OVERSAMPLE_FLAG1_DL1 :1;
        uint8_t CSI1_OVERSAMPLE_FLAG2_DL1 :1;
        uint8_t CSI1_HS_RX_ZM_CL :1;
        uint8_t CSI1_HS_RX_ZM_DL1 :1;
        uint8_t CSI1_RX_INIT_CLK :1;
        uint8_t reserved0 :3;
        uint8_t CSI1_STOP_STATE_CL :1;
        uint8_t CSI1_STOP_STATE_DL1 :1;
        uint8_t CSI1_ULP_ACTIVE_NOT_CL :1;
        uint8_t CSI1_ULP_ACTIVE_NOT_DL1 :1;
    }IPP_CSI2_DPHY1_XL_DBG_OBS_ts;

}IPP_CSI2_DPHY1_XL_DBG_OBS_tu;



//IPP_CSI2_DPHY0_DBG_ITS
/*Description: provides debug information about CSI2 DPHY-level errors, associated to the input channel CSI0.

Provided that the level0 bit-mask IPP_SD_ERROR_ITM[15] or IPP_CD_ERROR_ITM[10] is unmasked and that the corresponding bit of IPP_CSI2_DPHY0_DBG_ITM is unmasked, an interrupt line is generated.

All these potential DPHY-level errors may be handled by the SW application, although they are present only for debug prurpose. Thus error mask is disabled by default.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_ERR_ESC_DL1 :1;
        uint8_t CSI0_ERR_ESC_DL2 :1;
        uint8_t CSI0_ERR_ESC_DL3 :1;
        uint8_t CSI0_ERR_SYNC_ESC_DL1 :1;
        uint8_t CSI0_ERR_SYNC_ESC_DL2 :1;
        uint8_t CSI0_ERR_SYNC_ESC_DL3 :1;
        uint8_t CSI0_ERR_ESC_DL4 :1;
        uint8_t CSI0_ERR_SYNC_ESC_DL4 :1;
    }IPP_CSI2_DPHY0_DBG_ITS_ts;

}IPP_CSI2_DPHY0_DBG_ITS_tu;



//IPP_CSI2_DPHY0_DBG_ITS_BCLR
/*Description: Writing 1 to this register clears the interrupt status IPP_CSI2_DPHY0_DBG_ITS.

Writing 0 has no impact.

But it does not reset the DPHY-level generated error cause. Note only csi_err_control_dl1/2/3 keeps high until a next change in line state, other error causes should be generated as one high pulse at rx_byte_clk_hs.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_DBG_ITS_BCLR :6;
    }IPP_CSI2_DPHY0_DBG_ITS_BCLR_ts;

}IPP_CSI2_DPHY0_DBG_ITS_BCLR_tu;



//IPP_CSI2_DPHY0_DBG_ITS_BSET
/*Description: Writing 1 to this register asserts the interrupt status IPP_CSI2_DPHY0_DBG_ITS.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_DBG_ITS_BSET :6;
    }IPP_CSI2_DPHY0_DBG_ITS_BSET_ts;

}IPP_CSI2_DPHY0_DBG_ITS_BSET_tu;



//IPP_CSI2_DPHY0_DBG_ITM
/*Description: Bit addressable mask read register following the interrupt line ipp_sensor_mode_error_irq or ipp_camera_mode_err_irq when the IPP HW detects a rising edge of the corresponding error status bit of IPP_CSI2_DPHY0_DBG_ITS.

When bit value is 0 the interrupt source is disabled/masked, when bit value is 1 the interrupt source is enabled.

Hard reset is 0, i.e. disabled interrupt line.

Global level0 mask bit IPP_SD_ERROR_ITM[15] or IPP_CD_ERROR_ITM[10] must be unmasked to enable the interrupt line.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_DBG_ITM :6;
    }IPP_CSI2_DPHY0_DBG_ITM_ts;

}IPP_CSI2_DPHY0_DBG_ITM_tu;



//IPP_CSI2_DPHY0_DBG_ITM_BCLR
/*Description: Writing 1 to this bit addressable mask register clears the corresponding bit interrupt mask IPP_CSI2_DPHY0_DBG_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_DBG_ITM_BCLR :6;
    }IPP_CSI2_DPHY0_DBG_ITM_BCLR_ts;

}IPP_CSI2_DPHY0_DBG_ITM_BCLR_tu;



//IPP_CSI2_DPHY0_DBG_ITM_BSET
/*Description: Writing 1 to this bit addressable mask register sets the corresponding bit interrupt mask IPP_CSI2_DPHY0_DBG_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_DBG_ITM_BSET :6;
    }IPP_CSI2_DPHY0_DBG_ITM_BSET_ts;

}IPP_CSI2_DPHY0_DBG_ITM_BSET_tu;



//IPP_CSI2_DPHY1_DBG_ITS
/*Description: provides debug information about CSI2 DPHY-level errors, associated to the input channel CSI1.

Provided that the level0 bit-mask IPP_SD_ERROR_ITM[15] or IPP_CD_ERROR_ITM[10] is unmasked and that the corresponding bit of IPP_CSI2_DPHY0_DBG1_ITM is unmasked, an interrupt ipp_sensor_mode_error_irq (or resp. ipp_camera_mode_error_irq) is generated.

All these potential DPHY-level errors may be handled by the SW application, although they are present only for debug prurpose. Thus error mask is disabled by default.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_ERR_ESC_DL1 :1;
        uint8_t CSI1_ERR_SYNC_ESC_DL1 :1;
    }IPP_CSI2_DPHY1_DBG_ITS_ts;

}IPP_CSI2_DPHY1_DBG_ITS_tu;



//IPP_CSI2_DPHY1_DBG_ITS_BCLR
/*Description: Writing 1 to this register clears the interrupt status IPP_CSI2_DPHY1_DBG_ITS.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_DBG_ITS_BCLR :2;
    }IPP_CSI2_DPHY1_DBG_ITS_BCLR_ts;

}IPP_CSI2_DPHY1_DBG_ITS_BCLR_tu;



//IPP_CSI2_DPHY1_DBG_ITS_BSET
/*Description: Writing 1 to this register asserts the interrupt status IPP_CSI2_DPHY1_DBG_ITS.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_DBG_ITS_BSET :2;
    }IPP_CSI2_DPHY1_DBG_ITS_BSET_ts;

}IPP_CSI2_DPHY1_DBG_ITS_BSET_tu;



//IPP_CSI2_DPHY1_DBG_ITM
/*Description: Bit addressable mask read register following the interrupt line ipp_sensor_mode_error_irq or ipp_camera_mode_err_irq when the IPP HW detects a rising edge of the corresponding error status bit of IPP_CSI2_DPHY1_DBG_ITS.

When bit value is 0 the interrupt source is disabled/masked, when bit value is 1 the interrupt source is enabled.

Hard reset is 0, i.e. disabled interrupt line.

Global level0 mask bit IPP_SD_ERROR_ITM[15] or IPP_CD_ERROR_ITM[10] must be unmasked to enable the interrupt line.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_DBG_ITM :2;
    }IPP_CSI2_DPHY1_DBG_ITM_ts;

}IPP_CSI2_DPHY1_DBG_ITM_tu;



//IPP_CSI2_DPHY1_DBG_ITM_BCLR
/*Description: Writing 1 to this bit addressable mask register clears the corresponding bit interrupt mask IPP_CSI2_DPHY1_DBG_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_DBG_ITM_BCLR :2;
    }IPP_CSI2_DPHY1_DBG_ITM_BCLR_ts;

}IPP_CSI2_DPHY1_DBG_ITM_BCLR_tu;



//IPP_CSI2_DPHY1_DBG_ITM_BSET
/*Description: Writing 1 to this bit addressable mask register sets the corresponding bit interrupt mask IPP_CSI2_DPHY1_DBG_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_DBG_ITM_BSET :2;
    }IPP_CSI2_DPHY1_DBG_ITM_BSET_ts;

}IPP_CSI2_DPHY1_DBG_ITM_BSET_tu;



//IPP_CSI2_PACKET_DBG_ITS
/*Description: provides debug information about a received CSI2 packet header, when a 1-bit error ECC is detected and corrected.

Provided that the level0 bit-mask IPP_SD_ERROR_ITM or IPP_CD_ERROR_ITM[ is unmasked and that the bit of IPP_CSI2_PACKET_DBG_ITM is unmasked, an error interrupt line is generated.

This bit information may be handled by the SW application, although it is present only for debug prurpose. Error mask is disabled by default.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI2_PACKET_ECC_CORRECTED_DBG :1;
        uint8_t reserved0 :7;
        uint8_t CSI2_3D_PACKET_ECC_CORRECTED_DBG :1;
    }IPP_CSI2_PACKET_DBG_ITS_ts;

}IPP_CSI2_PACKET_DBG_ITS_tu;



//IPP_CSI2_PACKET_DBG_ITS_BCLR
/*Description: Writing 1 to this register clears the interrupt status IPP_CSI2_PACKET_DBG_ITS.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI2_PACKET_DBG_ITS_BCLR :1;
        uint8_t reserved0 :7;
        uint8_t CSI2_3D_PACKET_DBG_ITS_BCLR :1;
    }IPP_CSI2_PACKET_DBG_ITS_BCLR_ts;

}IPP_CSI2_PACKET_DBG_ITS_BCLR_tu;



//IPP_CSI2_PACKET_DBG_ITS_BSET
/*Description: Writing 1 to this register clears the interrupt status IPP_CSI2_PACKET_DBG_ITS.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI2_PACKET_DBG_ITS_BSET :1;
        uint8_t reserved0 :7;
        uint8_t CSI2_3D_PACKET_DBG_ITS_BSET :1;
    }IPP_CSI2_PACKET_DBG_ITS_BSET_ts;

}IPP_CSI2_PACKET_DBG_ITS_BSET_tu;



//IPP_CSI2_PACKET_DBG_ITM
/*Description: Bit addressable mask read register following the interrupt line ipp_sensor_mode_error_irq or ipp_camera_mode_err_irq when the IPP HW detects a rising edge of the corresponding error status bit of IPP_CSI2_PACKET_DBG_ITS.

When bit value is 0 the interrupt source is disabled/masked, when bit value is 1 the interrupt source is enabled.

Hard reset is 0, i.e. disabled interrupt line.

Global level0 mask bit IPP_SD_ERROR_ITM] or IPP_CD_ERROR_ITM must be unmasked to enable the interrupt line.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI2_PACKET_DBG_ITM :1;
        uint8_t reserved0 :7;
        uint8_t CSI2_3D_PACKET_DBG_ITM :1;
    }IPP_CSI2_PACKET_DBG_ITM_ts;

}IPP_CSI2_PACKET_DBG_ITM_tu;



//IPP_CSI2_PACKET_DBG_ITM_BCLR
/*Description: Writing 1 to this bit addressable mask register clears the corresponding bit interrupt mask IPP_CSI2_PACKET_DBG_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI2_PACKET_DBG_ITM_BCLR :1;
        uint8_t reserved0 :7;
        uint8_t CSI2_3D_PACKET_DBG_ITM_BCLR :1;
    }IPP_CSI2_PACKET_DBG_ITM_BCLR_ts;

}IPP_CSI2_PACKET_DBG_ITM_BCLR_tu;



//IPP_CSI2_PACKET_DBG_ITM_BSET
/*Description: Writing 1 to this bit addressable mask register sets the corresponding bit interrupt mask IPP_CSI2_PACKET_DBG_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI2_PACKET_DBG_ITM_BSET :1;
        uint8_t reserved0 :7;
        uint8_t CSI2_3D_PACKET_DBG_ITM_BSET :1;
    }IPP_CSI2_PACKET_DBG_ITM_BSET_ts;

}IPP_CSI2_PACKET_DBG_ITM_BSET_tu;



//DEPRECATED_IPP1
/*Description: OLD used to observe the values of the current virtual_channel and the current data_type (image or embedded), given by by the CSI2RX HW decoder module.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}DEPRECATED_IPP1_tu;



//IPP_STATIC_CSI2RX_DATA_TYPE_W
/*Description: Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module with the values of the csi2rx_non_image_data_type and csi2rx_image_data_type.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.

Default value is 0x2A2A (RAW8-RAW8).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_IMAGE_DATA_TYPE :6;
        uint8_t reserved0 :2;
        uint8_t STATIC_CSI2RX_NON_IMAGE_DATA_TYPE :6;
    }IPP_STATIC_CSI2RX_DATA_TYPE_W_ts;

}IPP_STATIC_CSI2RX_DATA_TYPE_W_tu;



//DEPRECATED_IPP3
/*Description: OLD Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module for the virtual channel.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}DEPRECATED_IPP3_tu;



//DEPRECATED_IPP4
/*Description: OLD used to observe the current frame number, extracted by the CSI2RX from the last frame start 'short packet data field'.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}DEPRECATED_IPP4_tu;



//DEPRECATED_IPP5
/*Description: OLD used to observe the current data field, extracted by the CSI2RX from the last line start 'short packet data field'
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}DEPRECATED_IPP5_tu;



//IPP_STATIC_CSI2RX_PIX_WIDTH_W
/*Description: Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module with the values of the embedded data and active data pixel widths, provided that the IPP_CSI2RX_DATA_SEL register field is CSI2RX_DTYPE_SEL=0.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_ACTDATA_PIXWIDTH :4;
        uint8_t STATIC_CSI2RX_EMBDATA_PIXWIDTH :4;
    }IPP_STATIC_CSI2RX_PIX_WIDTH_W_ts;

}IPP_STATIC_CSI2RX_PIX_WIDTH_W_tu;



//IPP_CSI2_DPHY_ERROR_ITS
/*Description: provides information about CSI2 DPHY-level errors, associated to the two input channels CSI0 and CSI1.

Provided that the level0 bit-mask IPP_SD_ERROR_ITM[15] (or resp. IPP_SD_ERROR_ITM[10]) is unmasked and that the corresponding bit of IPP_CSI2_DPHY_ERROR_ITM is unmasked, an interrupt ipp_sensor_mode_error_irq (or resp. ipp_camera_mode_error_irq) is generated.

It is recommended that all these DPHY-level errors are handled by the SW application. MMDSP FW interrupt routine sequence can start by disabling next all possible sources by IPP_XD_ERROR_ITM, and reading the error interrupt status registers to identify the root causes of the error.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_ERR_EOT_SYNC_HS_DL1 :1;
        uint8_t CSI0_ERR_EOT_SYNC_HS_DL2 :1;
        uint8_t CSI0_ERR_EOT_SYNC_HS_DL3 :1;
        uint8_t CSI0_ERR_SOT_HS_DL1 :1;
        uint8_t CSI0_ERR_SOT_HS_DL2 :1;
        uint8_t CSI0_ERR_SOT_HS_DL3 :1;
        uint8_t CSI0_ERR_SOT_SYNC_HS_DL1 :1;
        uint8_t CSI0_ERR_SOT_SYNC_HS_DL2 :1;
        uint8_t CSI0_ERR_SOT_SYNC_HS_DL3 :1;
        uint8_t CSI0_ERR_CONTROL_DL1 :1;
        uint8_t CSI0_ERR_CONTROL_DL2 :1;
        uint8_t CSI0_ERR_CONTROL_DL3 :1;
        uint8_t CSI1_ERR_EOT_SYNC_HS_DL1 :1;
        uint8_t CSI1_ERR_SOT_HS_DL1 :1;
        uint8_t CSI1_ERR_SOT_SYNC_HS_DL1 :1;
        uint8_t CSI1_ERR_CONTROL_DL1 :1;
    }IPP_CSI2_DPHY_ERROR_ITS_ts;

}IPP_CSI2_DPHY_ERROR_ITS_tu;



//IPP_CSI2_DPHY_ERROR_ITS_BCLR
/*Description: Writing 1 to this register clears the interrupt status IPP_CSI2_DPHY_ERROR_ITS.

Writing 0 has no impact.

Writing 1 to any of these bits, clears the corresponding DPHY-level error interrupt status bit. However it does not reset the DPHY-level generated error cause. Note only csi_err_control_dl1/2/3 keeps high until a next change in line state, other error causes should be generated as one high pulse at rx_byte_clk_hs.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CSI_DPHY_ERROR_ITS_BCLR;
    }IPP_CSI2_DPHY_ERROR_ITS_BCLR_ts;

}IPP_CSI2_DPHY_ERROR_ITS_BCLR_tu;



//IPP_CSI2_DPHY_ERROR_ITS_BSET
/*Description: Writing 1 to this register asserts the interrupt status IPP_CSI2_DPHY_ERROR_ITS.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CSI_DPHY_ERROR_ITS_BSET;
    }IPP_CSI2_DPHY_ERROR_ITS_BSET_ts;

}IPP_CSI2_DPHY_ERROR_ITS_BSET_tu;



//IPP_CSI2_DPHY_ERROR_ITM
/*Description: Bit addressable mask register in order to enable the interrupt line ipp_sensor_mode_error_irq or ipp_camera_mode_error_irq when the IPP HW detects a rising edge of the corresponding error status bit of IPP_CSI2_DPHY_ERROR_ITS .

Hard reset is 0x0, i.e. disabled interrupt line for any of the DPHY error sources.

Global level0 mask bit IPP_SD_ERROR_ITM[15] or IPP_CD_ERROR_ITM[10] must be unmasked to enable the interrupt line
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CSI_DPHY_ERROR_ITM;
    }IPP_CSI2_DPHY_ERROR_ITM_ts;

}IPP_CSI2_DPHY_ERROR_ITM_tu;



//IPP_CSI2_DPHY_ERROR_ITM_BCLR
/*Description: Writing 1 to this bit addressable mask register clears the corresponding bit interrupt mask IPP_CSI2_DPHY_ERROR_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CSI_DPHY_ERROR_ITM_BCLR;
    }IPP_CSI2_DPHY_ERROR_ITM_BCLR_ts;

}IPP_CSI2_DPHY_ERROR_ITM_BCLR_tu;



//IPP_CSI2_DPHY_ERROR_ITM_BSET
/*Description: Writing 1 to this bit addressable mask register sets the corresponding bit interrupt mask IPP_CSI2_DPHY_ERROR_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CSI_DPHY_ERROR_ITM_BSET;
    }IPP_CSI2_DPHY_ERROR_ITM_BSET_ts;

}IPP_CSI2_DPHY_ERROR_ITM_BSET_tu;



//IPP_CSI2_PACKET_ERROR_ITS
/*Description: provides information generated by CSI2RX about CSI2 packet-level errors.

Provided that the level0 bit-mask IPP_SD_ERROR_ITM (or resp. IPP_CD_ERROR_ITM) is unmasked and that the corresponding bit of IPP_CSI2_PACKET_ERROR_ITM is unmasked, an interrupt ipp_sensor_mode_error_irq (or resp. ipp_camera_mode_error_irq) is generated.

It is recommended that all these packet level errors are handled by the SW application. MMDSP FW interrupt routine sequence can start by disabling next all possible sources by IPP_XD_ERROR_ITM, and reading the error interrupt status registers to identify the root causes of the error.

A packet header ECC error should occur when at least a two-bit error is detected.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI2_PACKET_PAYLOAD_CRC_ERROR :1;
        uint8_t CSI2_PACKET_HEADER_ECC_ERROR :1;
        uint8_t CSI2_EOF :1;
        uint8_t reserved0 :5;
        uint8_t CSI2_3D_PACKET_PAYLOAD_CRC_ERROR :1;
        uint8_t CSI2_3D_PACKET_HEADER_ECC_ERROR :1;
        uint8_t CSI2_3D_EOF :1;
    }IPP_CSI2_PACKET_ERROR_ITS_ts;

}IPP_CSI2_PACKET_ERROR_ITS_tu;



//IPP_CSI2_PACKET_ERROR_ITS_BCLR
/*Description: Writing 1 to this register clears the interrupt status IPP_CSI2_DPHY_ERROR_ITS.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_PACKET_ERROR_ITS_BCLR :3;
        uint8_t reserved0 :5;
        uint8_t CSI_3D_PACKET_ERROR_ITS_BCLR :3;
    }IPP_CSI2_PACKET_ERROR_ITS_BCLR_ts;

}IPP_CSI2_PACKET_ERROR_ITS_BCLR_tu;



//IPP_CSI2_PACKET_ERROR_ITS_BSET
/*Description: Writing 1 to this register asserts the interrupt status IPP_CSI2_DPHY_ERROR_ITS.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_PACKET_ERROR_ITS_BSET :3;
        uint8_t reserved0 :5;
        uint8_t CSI_3D_PACKET_ERROR_ITS_BSET :3;
    }IPP_CSI2_PACKET_ERROR_ITS_BSET_ts;

}IPP_CSI2_PACKET_ERROR_ITS_BSET_tu;



//IPP_CSI2_PACKET_ERROR_ITM
/*Description: Bit addressable mask register in order to enable the interrupt line ipp_sensor_mode or ipp_camera_mode_error_irq when the IPP HW detects a rising edge of the corresponding error status bit of IPP_CSI2_PACKET_ERROR_ITS.

Hard reset is 0, i.e. enabled interrupt line for any of the protocol packet decoding error sources.

Global level0 mask bit IPP_SD_ERROR_ITM or IPP_CD_ERROR_ITM must be unmasked to enable the interupt line.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_PACKET_ERROR_ITM :3;
        uint8_t reserved0 :5;
        uint8_t CSI_3D_PACKET_ERROR_ITM :3;
    }IPP_CSI2_PACKET_ERROR_ITM_ts;

}IPP_CSI2_PACKET_ERROR_ITM_tu;



//IPP_CSI2_PACKET_ERROR_ITM_BCLR
/*Description: Writing 1 to this bit addressable mask register clears the corresponding bit interrupt mask IPP_CSI2_PACKET_ERROR_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_PACKET_ERROR_ITM_BCLR :3;
        uint8_t reserved0 :5;
        uint8_t CSI_3D_PACKET_ERROR_ITM_BCLR :3;
    }IPP_CSI2_PACKET_ERROR_ITM_BCLR_ts;

}IPP_CSI2_PACKET_ERROR_ITM_BCLR_tu;



//IPP_CSI2_PACKET_ERROR_ITM_BSET
/*Description: Writing 1 to this bit addressable mask register sets the corresponding bit interrupt mask IPP_CSI2_PACKET_ERROR_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_PACKET_ERROR_ITM_BSET :3;
        uint8_t reserved0 :5;
        uint8_t CSI_3D_PACKET_ERROR_ITM_BSET :3;
    }IPP_CSI2_PACKET_ERROR_ITM_BSET_ts;

}IPP_CSI2_PACKET_ERROR_ITM_BSET_tu;



//IPP_STATIC_OPIPE03_SEL
/*Description: Static register to select the output pipe to be sent to the DMA from either the sensor output pipe0 (PRY0) or the camera ouput pipe 3 (YUV/JPEG input pipeline).

This static register must be written only when SIA_CLOCK_ENABLE.enable_clk_stbp=0.

Hard reset is 0, i.e. selection of output pipe0.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_OPIPE03_SEL :1;
    }IPP_STATIC_OPIPE03_SEL_ts;

}IPP_STATIC_OPIPE03_SEL_tu;



//IPP_ISP_ASYNC_RESET
/*Description: Any write to the IPP_ISP_ASYNC_RESET register address issues a local hard reset to the ISP=IRP sub-block (i.e. including xP70 MCU core&memories), whatever is the xbus write data xdw[15:0].

This FW command sets all the internal FFs of the IRP sub-block into the same state as during asynchronous hard reset, i.e. after power-up and before ewarp FW boot.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}IPP_ISP_ASYNC_RESET_tu;



//IPP_SD_RESET
/*Description: Write '1' to the IPP_SD_RESET register issues a soft reset to the all sensor datapath. Writing an even value issues individual reset to blocks, according to bits set to '1'.

A 'all-sensor-datapath' soft reset can be issued when IPP in sensor mode is idle or active, for the latter it is equivalent to a stop command to force IPP in sensor mode to go to a safe idle state. A sensor datapath stop command can be written whenever.
Note: Nevertheless, MMDSP FW first should stop the IPP and then after it should stop the DMA for imaging data processes (in order to avoid an IPP raised interrupt consecutive to a DMA process not ready).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t ALL :1;
        /* reset all sensor datapath (legacy SD_RESET)*/
        uint8_t reserved0 :6;
        uint8_t BRML :1;
        /* reset BML/RML*/
        uint8_t SDI :1;
        /* reset sensor front-end (Sensor Data Interface) only (CSI2RX, CCPRX)*/
    }IPP_SD_RESET_ts;

}IPP_SD_RESET_tu;



//IPP_SD_STATIC_CCP_EN
/*Description: Static enable control register bit for sensor datapath in CCP mode, i.e. CCP_IF and CRM modules.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.

An explicit FW write IPP_SD_STATIC_CCP_EN=1 must be issued to command the IPP/SDG to enable and start for CCP sensor grabbing. A sensor data grabber enable command is equivalent to 'start to stream input video and write output pipes by a frame-based locked step'.

Then, according to the 3 possible IPP outputs, enabled by IPP_SD_PRY0/1_ENABLE_CLR/SET/STATUS and IPP_SD_BMS_ENABLE_CLR/SET/STATUS, one next frame via up to 3 concurrent IRP output pipes is sent or not to the DMA. IPP SDG starts and keeps on grabbing next sensor frame, until an explicit IPP_SD_RESET or by disabling IPP_SD_STATIC_CCP_EN=0.

The DMA issues a memory frame storage completion interrrupt to the MMDSP. Writing IPP_SD_RESET has no impact on the register IPP_SD_STATIC_CCP_EN.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_SD_CCP_EN :1;
        /* disabled*/
    }IPP_SD_STATIC_CCP_EN_ts;

}IPP_SD_STATIC_CCP_EN_tu;



//IPP_SD_STATIC_SDG_CSI_MODE
/*Description: Static configuration register bit for SDG in CSI mode, in order to set SDG CSI mode either into :

0: SMIA over CSI2 mode (default) : embedded data are padded and packed as defined by SMIA specification. The number of bits per pixel is the same for embedded data and for active data. Also a line length is the same both for an embedded lines and an active one within a frame.

1: CSI2 specific mode: embedded data are always 8-bit data. And possibly a line length is not the same between an embedded line and an active one.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0..
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CDG_CSI_MODE :1;
        /* SMIA over CSI2 mode*/
    }IPP_SD_STATIC_SDG_CSI_MODE_ts;

}IPP_SD_STATIC_SDG_CSI_MODE_tu;



//IPP_SD_PRY0_ENABLE_SET
/*Description: For sensor mode, any write at this register address enables the PRY0 for a next frame, whatever is the written data bus.

An explicit FW write must be issued to enable PRY0, i.e. enable one next frame out of the output pipe0 (high resolution pipe), in order to explicit the synchronization between the host buffer allocation and the IPP/DMA HW.

The HW PRY0 automatically deasserts the enabled state at the start of the first active line of each frame. Then each frame out must be repeatedly enabled by the FW. PRY0 activity status can be read via the register IPP_SD_PRY0_ENABLE_STATUS.

Hard reset value is 0, and PRY0 pipe0 disabled.

Writing global reset by IPP_SD_RESET or local reset by IPP_SD_PRY0_ENABLE_CLR disables the PRY0 and deasserts the IPP_SD_PRY0_ENABLE_STATUS provided it was high.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}IPP_SD_PRY0_ENABLE_SET_tu;



//IPP_SD_PRY1_ENABLE_SET
/*Description: For sensor mode, any write at this register address enables the PRY1 for a next frame, whatever is the written data bus.

An explicit FW write must be issued to enable PRY1, i.e. enable one next frame out of the output pipe1 (low resolution pipe), in order to explicit the synchronization between the host buffer allocation and the IPP/DMA HW.

The HW PRY1 automatically deasserts its enabled state at the start of the first active line of each frame. Then each frame out must be repeatedly enabled by the FW. PRY1 activity status can be read via the register IPP_SD_PRY1_ENABLE_STATUS.

Hard reset value is 0, and PRY1 pipe1 disabled.

Writing global reset by IPP_SD_RESET or local reset by IPP_SD_PRY1_ENABLE_CLR disables the PRY1 and deasserts the IPP_SD_PRY1_ENABLE_STATUS provided it was high.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}IPP_SD_PRY1_ENABLE_SET_tu;



//IPP_SD_PRY0_ENABLE_CLR
/*Description: Any write to the IPP_SD_PRY0_ENABLE_CLR register address issues a local soft reset to the output pipe0 packer PRY0, whatever is the xbus write data xdw[15:0].

Then PRY0 is forced to idle state. A next IPP_SD_PRY0_ENABLE_SET must be set to enable a next frame to be grabbed.

This register is to be typically used in the interrupt service routine following an output pipe0 overflow, i.e. IPP_SD_ERROR_ITS[7:5] asserted.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}IPP_SD_PRY0_ENABLE_CLR_tu;



//IPP_SD_PRY1_ENABLE_CLR
/*Description: Any write to the IPP_SD_PRY1_ENABLE_CLR register address issues a local soft reset to the output pipe1 packer PRY1, whatever is the xbus write data xdw[15:0].

Then PRY1 is forced to idle state. A next IPP_SD_PRY1_ENABLE_SET must be set to enable a next frame to be grabbed.

This register is to be typically used in the interrupt service routine following an output pipe1 overflow, i.e. IPP_SD_ERROR_ITS[8] is asserted.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}IPP_SD_PRY1_ENABLE_CLR_tu;



//IPP_SD_PRY0_ENABLE_STATUS
/*Description: This register logs the IPP PRY0 enable status. FW typically makes polling on this register until its value enable_status=0 in order to know that a current frame has started to be entered in the PRY and is being generated to the DMA. From this synchronization point, FW may make a new configuration of the PRY0 via IPP_SD_PRY0_FORMAT and trigger a next frame to be grabbed via a write to IPP_SD_PRY0_ENABLE_SET.

Hard reset value is 0, and PRY0 status disabled.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t PRY0_ENABLE_STATUS :1;
        /* default to 0*/
    }IPP_SD_PRY0_ENABLE_STATUS_ts;

}IPP_SD_PRY0_ENABLE_STATUS_tu;



//IPP_SD_PRY1_ENABLE_STATUS
/*Description: This register logs the IPP PRY1 enable status. FW typically makes polling on this register until its value enable_status=0 in order to know that a current frame has started to be entered in the PRY and is being generated to the DMA. From this synchronization point, FW may make a new configuration of the PRY1 via IPP_SD_PRY1_FORMAT and trigger a next frame to be grabbed via a write to IPP_SD_PRY1_ENABLE_SET.

Hard reset value is 0, and PRY1 status disabled.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t PRY1_ENABLE_STATUS :1;
        /* default to 0*/
    }IPP_SD_PRY1_ENABLE_STATUS_ts;

}IPP_SD_PRY1_ENABLE_STATUS_tu;



//IPP_SD_BMS_ENABLE_SET
/*Description: For sensor mode, any write at this register address enables the BMS for a next frame, whatever is the written data bus.

An explicit FW write must be issued to enable BMS, i.e. enable one next frame coming from the ISP Bayer front datapath to be output to DMA, in order to explicit the synchronization between the host buffer allocation and the IPP/DMA HW.

The HW BMS automatically deasserts its enabled state at the start of the first SOF line of each frame. Then each frame out must be repeatedly enabled by the FW. Hard reset value is 0, and BMS disabled.



Writing global reset by IPP_SD_RESET or local reset by IPP_SD_BMS_ENABLE_CLR disables the BMS and deasserts the IPP_SD_BMS_ENABLE_STATUS provided it was high.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}IPP_SD_BMS_ENABLE_SET_tu;



//IPP_SD_BMS_ENABLE_CLR
/*Description: Any write to the IPP_SD_BMS_ENABLE_CLR register address issues a local soft reset to the BMS, whatever is the xbus write data xdw[15:0].

Then BMS is forced to idle state. A next IPP_BMS_ENABLE_SET must be set to enable a next frame to be grabbed.

When the BMS is reset via IPP_SD_BMS_ENABLE_CLR or IPP_SD_RESET, the BMS is either cancelled from grabbing a next frame or aborted during a current frame generation. In the last case, since the BMS is reset, it can not generate any potential following error on the current frame.

This register is to be typically used in the interrupt service routine following a BMS opipe2 overflow, i.e. IPP_SD_ERROR_ITS[9] asserted.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}IPP_SD_BMS_ENABLE_CLR_tu;



//IPP_SD_BMS_ENABLE_STATUS
/*Description: This register logs the IPP BMS enable status. FW typically makes polling on this register until its value enable_status=0 in order to know that a current frame has started to be entered in the BMS and is being generated to the DMA. From this synchronization point, FW may make a new configuration of the BMS via IPP_SD_BMS_FORMAT and trigger a next frame to be grabbed via a write to IPP_SD_BMS_ENABLE_SET.

Hard reset value is 0, and BMS status disabled.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BMS_ENABLE_STATUS :1;
        /* default to 0*/
    }IPP_SD_BMS_ENABLE_STATUS_ts;

}IPP_SD_BMS_ENABLE_STATUS_tu;



//IPP_SD_PRY0_FORMAT
/*Description: Set the PRY0 format configuration.

This shadow configuration register is internally taken into account after that the PRY0 is enabled, i.e. when FW has written IPP_SD_PRY0_ENABLE_SET.

More precisely, it can be set at any given time but the HW guards internally the re-configuration until an interframe boundary. The next grabbed frame format configuration is the latest FW written value before that an asynchrounous new frame start occurs, provided that PRY0 was enabled before end of vertical blanking.

Default is 0x0600, i.e. YCbCr 4:2:2 1 buffer. In YCbCr 4:2:0 formats, 4:2:2 to 4:2:0 chroma subsampling is done in IPP by simple chroma decimation.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0;
        uint8_t PRY0_FORMAT :4;
        /* 
        all other values are reserved*/
    }IPP_SD_PRY0_FORMAT_ts;

}IPP_SD_PRY0_FORMAT_tu;



//IPP_SD_PRY1_FORMAT
/*Description: Set the PRY1 format configuration.

This shadow configuration register is internally taken into account after that the PRY0 is enabled, i.e. when FW has written IPP_SD_PRY0_ENABLE_SET.

More precisely, it can be set at any given time but the HW guards internally the re-configuration until an interframe boundary. The next grabbed frame format configuration is the latest FW written value before that an asynchrounous new frame start occurs, provided that PRY0 was enabled before end of vertical blanking.

Default is 0x0100, i.e. YCbCr 4:2:2 1 buffer. In RGB 16bpp formats, the four (resp. one) lsbs of pipe1_alpha[7:0] are used to generate A4R4G4B4 (resp. A1R5G5B5).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t PRY1_ALPHA;
        uint8_t PRY1_FORMAT :4;
        /* 
        all other values are reserved*/
    }IPP_SD_PRY1_FORMAT_ts;

}IPP_SD_PRY1_FORMAT_tu;



//IPP_SD_BMS_FORMAT
/*Description: Set the BMS format configuration.

This shadow configuration register is internally taken into account when the BMS is enabled, i.e. when FW writes IPP_SD_BMS_ENABLE_SET.

More precisely, it can be set at any given time but the HW guards internally the re-configuration until an interframe boundary. The next grabbed frame format configuration is the latest FW written value before that an asynchrounous new frame start occurs, provided that BMS was enabled before end of vertical blanking.

Default is 0x0, i.e. RAW8 and Bayer Store 0 selected.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BMS_RAW_FORMAT :1;
        /* set the RAW memory format, either RAW8 or RAW12.*/
        uint8_t BMS_RAW_SEL :2;
        /* Bayer raw store data output at the beginning of the ISP Pictor Bayer domain (before the line blanking elimination aka 'Bayer store 0', in clk_sd_hispeed domain)*/
    }IPP_SD_BMS_FORMAT_ts;

}IPP_SD_BMS_FORMAT_tu;



//IPP_SD_STATIC_MEM_LOAD_EN
/*Description: Static control register bit to enable/disable the memory load datapath (BML for Bayer formats RAW8 and RAW12 or RML from R10G10B10) from the DDR via the STBusplug to the ISP in order to retreive frame buffer(s) according to the static configuration registers IPP_SD_STATIC_LOAD_XXX and inject them into the ISP.

This static register must be written only when CRM_EN_CLK_BML=0.

In memory load mode, an explicit FW write IPP_SD_STATIC_LOAD_MEM_EN=1 must be issued to command the BML or RML to start for frame(s) loading. A sensor data loader enable command is equivalent to 'start to load input frame buffer(s) and write output pipes by a frame-based locked step'.

Then, according to the 3 possible IPP outputs, enabled by IPP_SD_PRY0/1_ENABLE_CLR/SET/STATUS and IPP_SD_BMS_ENABLE_CLR/SET/STATUS, one next frame via up to 3 concurrent ISP output pipes is sent or not to the DMA.

Selected BML or RML starts and keeps on loading next data, until an explicit IPP_SD_RESET or by disabling IPP_SD_STATIC_MEM_LOAD_EN=0.

The DMA issues a memory frame storage completion interrrupt to the MMDSP.

Writing IPP_SD_RESET has no impact on the register IPP_SD_STATIC_MEM_LOAD_EN.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_MEM_LOAD_EN :1;
        /* memory load enable (active high)*/
    }IPP_SD_STATIC_MEM_LOAD_EN_ts;

}IPP_SD_STATIC_MEM_LOAD_EN_tu;



//IPP_SD_STATIC_LOAD_FORMAT
/*Description: Static configuration register for memory load mode. Configure the memory format: either RAW8, RAW12, R10G10B10, R8G8B8 and the ISP entry point.

This static register must be written only when CRM_EN_CLK_BML=0.

Default is 0, i.e. disabled RAW12 in the ISP entry point of the Bayer domain.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_LOAD_FORMAT :2;
        /* RAW12*/
        uint8_t STATIC_LOAD_RAW_SEL :1;
        /* Bayer load data injected at the entry point of the ISP Pictor Bayer domain*/
    }IPP_SD_STATIC_LOAD_FORMAT_ts;

}IPP_SD_STATIC_LOAD_FORMAT_tu;



//IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN
/*Description: Static configuration register for memory load mode. For bayer RAW12, RAW8 or R10G10B10, set the minimum horizontal time between two lines, in cases where the ISP upscaler requires to fix a minimum interline blanking time.

This static register must be written only when CRM_EN_CLK_BML=0.

Time unit is one pixel clock cycle (clk_pipe).

Reset values are 0. The 20-bit width interline_time field register is split into two IPP registers IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN and IPP_SD_STATIC_LOAD_LINE_TIME_MSB.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_INTERLINE_TIME_EN :1;
        /* interline time enable (active high)*/
        uint16_t STATIC_INTERLINE_TIME_LSB :15;
        /* 15-bit LSB interline time*/
    }IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_ts;

}IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_tu;



//IPP_SD_STATIC_LOAD_LINE_TIME_MSB
/*Description: Static configuration register for memory load mode.For bayer RAW12, RAW8 or R10G10B10, set the minimum horizontal time between two lines, in cases where the ISP upscaler requires to fix a minimum interline blanking time.

This static register must be written only when CRM_EN_CLK_BML=0.

Time unit is one pixel clock cycle (clk_pipe).

Reset values are 0. The 20-bit width interline_time field register is split into two IPP registers IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN and IPP_SD_STATIC_LOAD_LINE_TIME_MSB.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_INTERLINE_TIME_MSB :5;
        /* 5-bit MSB interline time*/
    }IPP_SD_STATIC_LOAD_LINE_TIME_MSB_ts;

}IPP_SD_STATIC_LOAD_LINE_TIME_MSB_tu;



//IPP_SD_STATIC_LOAD_WW
/*Description: Static configuration register for memory load mode. Set the frame window width as received from the sensor after ISP/BMS raw capture pass. Unit is one pixel.

This static register must be written only when CRM_EN_CLK_BML=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t STATIC_LOAD_WW :13;
        /* bayer memory store window width.*/
    }IPP_SD_STATIC_LOAD_WW_ts;

}IPP_SD_STATIC_LOAD_WW_tu;



//IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER
/*Description: Static configuration register for memory load mode. For RAW8 or RAW12 Bayer memory load, set the color of the first visible pixel. Hard reset value is 0 (Green in Red).

This static register must be written only when CRM_EN_CLK_BML=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_LOAD_BML_PIXEL_ORDER :2;
        /* [*/
    }IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts;

}IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_tu;



//IPP_SD_STATIC_LOAD_LINETYPE0
/*Description: Static configuration register for memory load mode. For RAW8 or RAW12or R10G10B10 memory load, set the 8 frame format raw descriptors. Each descriptor is described by its line type identification, and the height of that line type.

This static register must be written only when CRM_EN_CLK_BML=0.

A null height of that line type ie field STATIC_LOAD_LINETYPE_HEIGHTX=0 means that this linetype is unused.

Raw descriptors increasing order depicts the frame from the top to the bottom..
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t STATIC_LOAD_LINETYPE_HEIGHT0 :12;
        uint8_t STATIC_LOAD_LINETYPE_ID0 :4;
        /* 
        10xx: DARK 1101: ACTIVE*/
    }IPP_SD_STATIC_LOAD_LINETYPE0_ts;

}IPP_SD_STATIC_LOAD_LINETYPE0_tu;



//IPP_SD_STATIC_LOAD_LINETYPE1
/*Description: Static configuration register for memory load mode. For RAW8 or RAW12 or R10G10B10 memory load, set the 8 frame format raw descriptors. Each descriptor is described by its line type identification, and the height of that line type.

This static register must be written only when CRM_EN_CLK_BML=0.

A null height of that line type ie field STATIC_LOAD_LINETYPE_HEIGHTX=0 means that this linetype is unused.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t STATIC_LOAD_LINETYPE_HEIGHT1 :12;
        uint8_t STATIC_LOAD_LINETYPE_ID1 :4;
        /* 
        10xx: DARK 1101: ACTIVE*/
    }IPP_SD_STATIC_LOAD_LINETYPE1_ts;

}IPP_SD_STATIC_LOAD_LINETYPE1_tu;



//IPP_SD_STATIC_LOAD_LINETYPE2
/*Description: Static configuration register for memory load mode. For RAW8 or RAW12 or R10G10B10 memory load, set the 8 frame format raw descriptors. Each descriptor is described by its line type identification, and the height of that line type.

This static register must be written only when CRM_EN_CLK_BML=0.

A null height of that line type ie field STATIC_LOADLINETYPE_HEIGHTX=0 means that this linetype is unused.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t STATIC_LOAD_LINETYPE_HEIGHT2 :12;
        uint8_t STATIC_LOAD_LINETYPE_ID2 :4;
        /* 
        10xx: DARK 1101: ACTIVE*/
    }IPP_SD_STATIC_LOAD_LINETYPE2_ts;

}IPP_SD_STATIC_LOAD_LINETYPE2_tu;



//IPP_SD_STATIC_LOAD_LINETYPE3
/*Description: Static configuration register for memoryload mode. For RAW8 or RAW12 or R10G10B10 memory load, set the 8 frame format raw descriptors. Each descriptor is described by its line type identification, and the height of that line type.

This static register must be written only when CRM_EN_CLK_BML=0.

A null height of that line type ie field STATIC_LOAD_LINETYPE_HEIGHTX=0 means that this linetype is unused.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t STATIC_LOAD_LINETYPE_HEIGHT3 :12;
        uint8_t STATIC_LOAD_LINETYPE_ID3 :4;
        /* 
        10xx: DARK 1101: ACTIVE*/
    }IPP_SD_STATIC_LOAD_LINETYPE3_ts;

}IPP_SD_STATIC_LOAD_LINETYPE3_tu;



//IPP_SD_STATIC_LOAD_LINETYPE4
/*Description: Static configuration register fomemory load mode. For RAW8 or RAW12 or R10G10B10 memory load, set the 8 frame format raw descriptors. Each descriptor is described by its line type identification, and the height of that line type.

This static register must be written only when CRM_EN_CLK_BML=0.

A null height of that line type ie field STATIC_LOAD_LINETYPE_HEIGHTX=0 means that this linetype is unused.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t STATIC_LOAD_LINETYPE_HEIGHT4 :12;
        uint8_t STATIC_LOAD_LINETYPE_ID4 :4;
        /* 
        10xx: DARK 1101: ACTIVE*/
    }IPP_SD_STATIC_LOAD_LINETYPE4_ts;

}IPP_SD_STATIC_LOAD_LINETYPE4_tu;



//IPP_SD_STATIC_LOAD_LINETYPE5
/*Description: Static configuration register for memory load modeFor RAW8 or RAW12 or R10G10B10 memory load, set the 8 frame format raw descriptors. Each descriptor is described by its line type identification, and the height of that line type.

This static register must be written only when CRM_EN_CLK_BML=0.

A null height of that line type ie field STATIC_LOAD_LINETYPE_HEIGHTX=0 means that this linetype is unused.Raw descriptors increasing order depicts the frame from the top to the bottom.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t STATIC_LOAD_LINETYPE_HEIGHT5 :12;
        uint8_t STATIC_LOAD_LINETYPE_ID5 :4;
        /* 
        10xx: DARK 1101: ACTIVE*/
    }IPP_SD_STATIC_LOAD_LINETYPE5_ts;

}IPP_SD_STATIC_LOAD_LINETYPE5_tu;



//IPP_SD_STATIC_LOAD_LINETYPE6
/*Description: Static configuration register for memory load mode. For RAW8 or RAW12 or R10G10B10 memory load, set the 8 frame format raw descriptors. Each descriptor is described by its line type identification, and the height of that line type.

This static register must be written only when CRM_EN_CLK_BML=0.

A null height of that line type ie field STATIC_LOAD_LINETYPE_HEIGHTX=0 means that this linetype is unused.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t STATIC_LOAD_LINETYPE_HEIGHT6 :12;
        uint8_t STATIC_LOAD_LINETYPE_ID6 :4;
        /* 
        10xx: DARK 1101: ACTIVE*/
    }IPP_SD_STATIC_LOAD_LINETYPE6_ts;

}IPP_SD_STATIC_LOAD_LINETYPE6_tu;



//IPP_SD_STATIC_LOAD_LINETYPE7
/*Description: Static configuration register for memory load mode. For RAW8 or RAW12 or R10G10B10 memory load, set the 8 frame format raw descriptors. Each descriptor is described by its line type identification, and the height of that line type.

This static register must be written only when CRM_EN_CLK_BML=0.

A null height of that line type ie field STATIC_LOAD_LINETYPE_HEIGHTX=0 means that this linetype is unused.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t STATIC_LOAD_LINETYPE_HEIGHT7 :12;
        uint8_t STATIC_LOAD_LINETYPE_ID7 :4;
        /* 
        10xx: DARK 1101: ACTIVE*/
    }IPP_SD_STATIC_LOAD_LINETYPE7_ts;

}IPP_SD_STATIC_LOAD_LINETYPE7_tu;



//IPP_SD_ERROR_ITS
/*Description: When simultaneously one or several first error(s) occur(s), the IPP sensor datapath out of IRP goes immediately in a frozen state and the ipp_sensor_mode_err_irq interrupt line is asserted, provided that the mask IPP_SD_ERROR_ITM was set.Reading the error interrupt status register identifies the root causes of the error, which can be one/several of:
* CCP sensor rx error type: CRC error, false_sync error (false following of synchronization codes), shift_sync (active line not a multiple of 32 bits), pixel_sync error (invalid number of horizontal pixels)
* opipe0 overflow: any one of the 3 data channels
* opipe1 overflow data channel
* opipe2 overflow data channel
* opipe0 channel data error when frame not a multiple of 8 bytes (for more information c.f. IPP_SD_OPIPE0_MULT_ERR_DBG)
* opipe1 channel data error when frame not a multiple of 8 bytes
* sensor csi2 packet error (c.f. IPP_CSI2_PACKET_ERROR_INT_CTRL, IPP_CSI2_PACKET_DBG_INT_CTRL)
* sensor csi2 DPHY error (c.f. IPP_CSI2_DPHY_ERROR_INT_CTRL, IPP_CSI2_DPHY_DBG_INT_CTRL)
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CCP_SHIFT_SYNC :1;
        uint8_t CCP_FALSE_SYNC :1;
        uint8_t CCP_CRC_ERROR :1;
        uint8_t OPIPE0_CH0_OVERFLOW :1;
        uint8_t OPIPE0_CH1_OVERFLOW :1;
        uint8_t OPIPE0_CH2_OVERFLOW :1;
        uint8_t OPIPE1_OVERFLOW :1;
        uint8_t OPIPE2_OVERFLOW :1;
        uint8_t OPIPE0_MULT_ERR :1;
        uint8_t OPIPE1_MULT_ERR :1;
        uint8_t SD_CSI2_PACKET_ERROR :1;
        uint8_t SD_CSI2_DPHY_ERROR :1;
    }IPP_SD_ERROR_ITS_ts;

}IPP_SD_ERROR_ITS_tu;



//IPP_SD_ERROR_ITS_BCLR
/*Description: Writing 1 to any of the bits of the IPP_SD_ERROR_ITS_BCLR clears the corresponding error interrupt status bit.

Writing 0 has no impact.

Note that writing IPP_SD_RESET clears all the potential error sources, except for DPHY-level errors.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t SD_ERROR_ITS_BCLR :12;
    }IPP_SD_ERROR_ITS_BCLR_ts;

}IPP_SD_ERROR_ITS_BCLR_tu;



//IPP_SD_ERROR_ITS_BSET
/*Description: Writing 1 to any of the bits of the IPP_SD_ERROR_ITS_BSET asserts the corresponding error interrupt status bit.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t SD_ERROR_ITS_BSET :12;
    }IPP_SD_ERROR_ITS_BSET_ts;

}IPP_SD_ERROR_ITS_BSET_tu;



//IPP_SD_ERROR_ITM
/*Description: Bit addressable mask read register in order to enable the interrupt line ipp_sensor_mode_error_irq when the IPP HW detects a rising edge of the corresponding error status bit of IPP_SD_ERROR_ITS.

Hard reset is 0x0, i.e. disabled interrupt line for any of the error sources
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t SD_ERROR_ITM :12;
    }IPP_SD_ERROR_ITM_ts;

}IPP_SD_ERROR_ITM_tu;



//IPP_SD_ERROR_ITM_BCLR
/*Description: Writing 1 to this bit addressable mask register clears the corresponding bit interrupt mask IPP_SD_ERROR_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t SD_ERROR_ITM_BCLR :12;
    }IPP_SD_ERROR_ITM_BCLR_ts;

}IPP_SD_ERROR_ITM_BCLR_tu;



//IPP_SD_ERROR_ITM_BSET
/*Description: Writing 1 to this bit addressable mask register sets the corresponding bit interrupt mask IPP_SD_ERROR_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t SD_ERROR_ITM_BSET :12;
    }IPP_SD_ERROR_ITM_BSET_ts;

}IPP_SD_ERROR_ITM_BSET_tu;



//IPP_SD_OPIPE0_MULT_ERR_DBG
/*Description: For debug, provided error information when not a multiple of 8 bytes is sent to the DMA via the output pipe0, whether it is channel0 (luma), channel1 (chroma Cb or Cb/Cr) or channel2 (Cr).This register is updated either by internal HW when an error is detected (in the same time, IPP_SD_ERROR_ITS[8] is HW internally set) or by an IPP_SD_RESET command. Hard reset value is 0 .
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t OPIPE0_CH0_MULT_ERR :1;
        uint8_t OPIPE0_CH1_MULT_ERR :1;
        uint8_t OPIPE0_CH2_MULT_ERR :1;
    }IPP_SD_OPIPE0_MULT_ERR_DBG_ts;

}IPP_SD_OPIPE0_MULT_ERR_DBG_tu;



//IPP_CD_RESET
/*Description: Any write to the IPP_CD_RESET register address issues a soft reset to the camera datapath, whatever is the xbus write data xdw[15:0].

A soft reset can be issued when IPP in camera mode is idle or active, for the latter it is equivalent to a stop command to force IPP in camera mode to go to a safe idle state. A camera datapath stop command can be written whenever.
Note: Nevertheless, MMDSP FW first should stop the IPP and then after it should stop the DMA for imaging data processes (in order to avoid an IPP raised interrupt consecutive to a DMA process not ready).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}IPP_CD_RESET_tu;



//IPP_CD_START
/*Description: Any write to the IPP_CD_START register address command the IPP to start to grab and process one single next camera frame, whatever is the xbus write data xdw[15:0].A camera datapath start command should be issued only after proper IPP camera configuration settings.

A camera start is equivalent to 'start to grab one input frame and write it out'.

When IPP CDG extracts a frame end from a CCP jpeg image, CDG sends the ipp_camera_raw_cdg_eof_irq to the MMDSP FW. Then the FW can ask again the IPP to 'start to grab one input frame and write it out', although the current frame is being written out from the DMA to DDR/eSRAM.

If the CDG extracts a frame end, and if FW did not restart the IPP by IPP_CD_START since the last CDG frame start extraction, IPP goes to idle state, and YUV/JPEG input pipeline finishes to write out to the DMA the last data. The DMA issues an interrrupt to indicate a complete frame storage.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
}IPP_CD_START_tu;



//IPP_CD_STATIC_EN
/*Description: Static register used to statically enable/disable the camera data path, i.e.CCP_IF and CRM modules. In camera mode, an explicit FW write IPP_CD_STATIC_EN=1 must be set to first enable the camera datapth before that a dynamic IPP_CD_START is issued to the IPP/CDG to start for camera grabbing.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.

Then IPP CDG starts grabbing one next sensor frame, until an explicit IPP_CD_RESET.

Writing IPP_CD_RESET has no impact on the register IPP_CD_STATIC_EN
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CD_EN :1;
        /* disabled*/
    }IPP_CD_STATIC_EN_ts;

}IPP_CD_STATIC_EN_tu;



//IPP_CD_STATIC_CDG_CSI_PADD_EN
/*Description: Static register used to statically configure the CDG for 32-bit line padding in the case of CSI (8bit per pixel) mode. SMIA CCP standard specifiies that a line must be a multiple of 32-bit on the contrary of the CSI2 byte protocol.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CDG_CSI_PADD_EN :1;
        /* disabled*/
    }IPP_CD_STATIC_CDG_CSI_PADD_EN_ts;

}IPP_CD_STATIC_CDG_CSI_PADD_EN_tu;



//IPP_CD_STATIC_CCP_FC
/*Description: Static register used only for the CDG in CCP mode.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CCP_FS;
        /* frame start code*/
        uint8_t STATIC_CCP_FE;
        /* frame end code*/
    }IPP_CD_STATIC_CCP_FC_ts;

}IPP_CD_STATIC_CCP_FC_tu;



//IPP_CD_STATIC_CCP_LC
/*Description: Static register used only for the CDG in CCP mode.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CCP_LS;
        /* line start code*/
        uint8_t STATIC_CCP_LE;
        /* line end code*/
    }IPP_CD_STATIC_CCP_LC_ts;

}IPP_CD_STATIC_CCP_LC_tu;



//IPP_CD_RAW
/*Description: Register used only if camera data pipe is set for raw data (IPP_CD_PIPE3_FORMAT[2:0]=3'b100).

When jpeg_padd_en is set for CSI or CCP, the camera datapath should complete the jpeg compressed stream by padding to a multiple of a 64-bit with the repeated value 32'hA5A5A5A5.

When ccp_raw_swap is set for CCP, the camera datapath should swap bits accordingly.

This register is used only for the CCP camera data path. Default value is 0, i.e. none bit swapping, none 64-bit word padding.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t JPEG_PADD_EN :1;
        uint8_t CCP_RAW_SWAP :2;
        /* 
        0x: pure raw*/
    }IPP_CD_RAW_ts;

}IPP_CD_RAW_tu;



//IPP_CD_PIPE3_FORMAT
/*Description: Register for pipe3 (IFP module) format.

In YUV modes, the number of bits per pixel is assumed to be 8. And pixel values can be saturated to comply with BT601standard (luma range is [16:235], chroma range is[16:240]).

In YUV 4:2:0 mode, only chroma decimation is supported inside IPP. (Note that another SIA alternative in order to store a YUV4:2:0 image into memory is to set YUV4:2:2 as output of IPP/pipe3 and then program the HW module VCF( Vertical Chroma Filter) for linear interpolation out of IPP.

In raw data mode (IPP_CD_PIPE3_FORMAT[2]=1), the CDG can trigger an interrupt ipp_camera_raw_cdg_eof_irq to the MMDSP.

Default is 0, i.e. YCbCr 4:2:0, 2 buffers (one for Y, one for Cb/Cr), without BT601 saturation.
Note: YUV 4:2:2 1 buffer output is one particular mode of the raw data.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t PIPE3_FORMAT :3;
        /* 
        all other values are reserved*/
        uint8_t BT601_EN :1;
        /* for BT601 saturation
        (luma in [16-235], chroma in [16-240])*/
    }IPP_CD_PIPE3_FORMAT_ts;

}IPP_CD_PIPE3_FORMAT_tu;



//IPP_CD_CROP_EN
/*Description: Register to enable cropping the input source frame received on the CCP/CSI interface..
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CROP_EN :1;
    }IPP_CD_CROP_EN_ts;

}IPP_CD_CROP_EN_tu;



//IPP_CD_CROP_H0
/*Description: Register to define the horizontal cropping.

Horizontal cropping has a resolution of two pixels (Cb,Y0,Cr,Y1) in order to align horizontal filtering with chrominance pixels. Low significant bit is left aside and CROP_H0[11:1] and CROP_H1[11:1] are sent to the CROP sub-module; CROP_H0 and CROP_H1 range starts from 0.

The maximum crop rectangle in luminance pixel size is 4094x4095.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :1;
        uint16_t CROP_H0 :11;
        /* upper left kept corner horizontal coordinate*/
    }IPP_CD_CROP_H0_ts;

}IPP_CD_CROP_H0_tu;



//IPP_CD_CROP_V0
/*Description: Register to define the vertical cropping. Vertical cropping has a resolution of one column;crop_v0 and crop_v1 range starts from 0.

The maximum crop rectangle in luminance pixel size is 4094x4095.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CROP_V0 :12;
        /* upper left kept corner vertical coordinate*/
    }IPP_CD_CROP_V0_ts;

}IPP_CD_CROP_V0_tu;



//IPP_CD_CROP_H1
/*Description: Register to define the horizontal cropping.

Horizontal cropping has a resolution of two pixels (Cb,Y0,Cr,Y1) in order to align horizontal filtering with chrominance pixels. Low significant bit is left aside and CROP_H0[11:1] and CROP_H1[11:1] are sent to the CROP sub-module; CROP_H0 and CROP_H1 range starts from 0.

The maximum crop rectangle in luminance pixel size is 4094x4095.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :1;
        uint16_t CROP_H1 :11;
        /* lower right first filtered corner horizontal coordinate*/
    }IPP_CD_CROP_H1_ts;

}IPP_CD_CROP_H1_tu;



//IPP_CD_CROP_V1
/*Description: Register to define the vertical cropping.

Vertical cropping has a resolution of one column;crop_v0 and crop_v1 range starts from 0.The maximum crop rectangle in luminance pixel size is 4094x4095.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CROP_V1 :12;
        /* lower right first filtered corner vertical coordinate*/
    }IPP_CD_CROP_V1_ts;

}IPP_CD_CROP_V1_tu;



//IPP_CD_ERROR_ITS
/*Description: The ipp_camera_mode_err_irq interrupt line is asserted when an IPP camera mode error occurs, provided that the corresponding bit mask was enabled by IPP_CD_ERROR_ITM.

Error interrupt status is reported by IPP_CD_ERROR_ITS.

The MMDSP can read the interrupt status register to identify the root cause of the error, which can be one/several of CCP error, raw data error, chroma data error, camera csi2 packet error, camera csi2 DPHY error

Hard reset value is 0.
Note: USE CASE ASSUMPTIONS:
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :1;
        uint8_t CCP_ERROR :1;
        /* a CCP protocol error due to a wrong succesion of synchronization codes ( |IPP_CD_CCP_ERR_DBG[5:3]=1) or a CCP source frame width not a multiple of 32 bits (|IPP_CD_CCP_ERR_DBG[5:3]=0) (e.g 2 pixels CbYCr, Y).*/
        uint8_t RAW_DATA_ERROR_NOT_X8_BYTES :1;
        /* overflow or raw data fifo error due to output window width not multiple of 64-bit words (this should not occur if IPP_CD_RAW_DATA.jpeg_padd_en=1).*/
        uint8_t RAW_DATA_ERROR_OVERFLOW :1;
        /* overflow or raw data fifo error due to output window width not multiple of 64-bit words (this should not occur if IPP_CD_RAW_DATA.jpeg_padd_en=1)*/
        uint8_t LUMA_ERROR_NOT_X8_LUMA_BYTES :1;
        /* luma data error, overflow or output window width not multiple of luma 64-bit words (i.e. 8 Y pixels)*/
        uint8_t LUMA_ERROR_OVERFLOW :1;
        /* luma data error, overflow or output window width not multiple of luma 64-bit words (i.e. 8 Y pixels)*/
        uint8_t CHROMA_ERROR_NOT_X16_BYTES_3BUF :1;
        uint8_t CHROMA_ERROR_NOT_X8BYTES_2BUF :1;
        uint8_t CHROMA_ERROR_OVERFLOW :1;
        /* chroma data error, overflow or output window width not multiple of chroma 64-bit words (i.e. 8 Y pixels) when 2 buffers, or output window width not multiple of chroma blue and red 64-bit words (i.e. 16 Y pixels) when 3 buffers*/
        uint8_t CD_CSI2_PACKET_ERROR :1;
        /* camera csi2 packet error (see IPP_CSI2_PACKET_ERROR_ITS, IPP_CSI2_PACKET_DBG_ITS)*/
        uint8_t CD_CSI2_DPHY_ERROR :1;
        /* camera csi2 DPHY error (see IPP_CSI2_DPHY_ERROR_ITS, IPP_CSI2_DPHY_DBG_ITS)*/
    }IPP_CD_ERROR_ITS_ts;

}IPP_CD_ERROR_ITS_tu;



//IPP_CD_ERROR_ITS_BCLR
/*Description: The ipp_camera_mode_err_irq interrupt line is asserted when an IPP camera mode error occurs, provided that the corresponding bit mask was enabled by IPP_CD_ERROR_ITM.

Error interrupt status is reported by IPP_CD_ERROR_ITS.

Writing 1 to any of the 10 bits of the IPP_CD_ERROR_ITS_BCLR clears the corresponding error interrupt status bit.

Note that writing IPP_CD_RESET clears all the potential error sources, except for DPHY-level errors, but does not reset the error status.

Hard reset value is 0.
Note: USE CASE ASSUMPTIONS:
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :1;
        uint16_t CD_ERROR_ITS_BCLR :10;
    }IPP_CD_ERROR_ITS_BCLR_ts;

}IPP_CD_ERROR_ITS_BCLR_tu;



//IPP_CD_ERROR_ITS_BSET
/*Description: The ipp_camera_mode_err_irq interrupt line is asserted when an IPP camera mode error occurs, provided that the corresponding bit mask was enabled by IPP_CD_ERROR_ITM.

Error interrupt status is reported by IPP_CD_ERROR_ITS.

Writing 1 to any of the 10 bits of the IPP_CD_ERROR_ITS_BSET set the corresponding error interrupt status bit.

Hard reset value is 0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :1;
        uint16_t CD_ERROR_ITS_BSET :10;
    }IPP_CD_ERROR_ITS_BSET_ts;

}IPP_CD_ERROR_ITS_BSET_tu;



//IPP_CD_ERROR_ITM
/*Description: Bit addressable mask read register in order to enable the interrupt line ipp_camera_mode_error_irq when the IPP HW detects a rising edge of the corresponding error status bit of IPPCSD_ERROR_ITS.

Hard reset is 0x0, i.e. disabled interrupt line for any of the error sources.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :1;
        uint16_t CD_ERROR_ITM :10;
    }IPP_CD_ERROR_ITM_ts;

}IPP_CD_ERROR_ITM_tu;



//IPP_CD_ERROR_ITM_BCLR
/*Description: Writing 1 to this bit addressable mask register clears the corresponding bit interrupt mask IPP_CD_ERROR_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :1;
        uint16_t CD_ERROR_ITM_BCLR :10;
    }IPP_CD_ERROR_ITM_BCLR_ts;

}IPP_CD_ERROR_ITM_BCLR_tu;



//IPP_CD_ERROR_ITM_BSET
/*Description: Writing 1 to this bit addressable mask register sets the corresponding bit interrupt mask IPP_CD_ERROR_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :1;
        uint16_t CD_ERROR_ITM_BSET :10;
    }IPP_CD_ERROR_ITM_BSET_ts;

}IPP_CD_ERROR_ITM_BSET_tu;



//IPP_CD_RAW_EOF_ITS
/*Description: Interrupt status for the interrupt line ipp_camera_raw_cdg_eof_irq signalling an end of a grabbed frame, while raw data mode.

Hard reset is 0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CD_RAW_EOF_ITS :1;
        /* [0]: cd raw eof interrupt status*/
    }IPP_CD_RAW_EOF_ITS_ts;

}IPP_CD_RAW_EOF_ITS_tu;



//IPP_CD_RAW_EOF_ITS_BCLR
/*Description: Writing 1 to this register clears the interrupt status IPP_CD_RAW_EOF_ITS.

Writing 0 has no impact.

Hard reset is 0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CD_RAW_EOF_ITS_BCLR :1;
        /* [0]: cd raw eof interrupt status clear*/
    }IPP_CD_RAW_EOF_ITS_BCLR_ts;

}IPP_CD_RAW_EOF_ITS_BCLR_tu;



//IPP_CD_RAW_EOF_ITS_BSET
/*Description: Writing 1 to this register sets the interrupt status IPP_CD_RAW_EOF_ITS.

Writing 0 has no impact.

Hard reset is 0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CD_RAW_EOF_ITS_BSET :1;
        /* [0]: cd raw eof interrupt status clear*/
    }IPP_CD_RAW_EOF_ITS_BSET_ts;

}IPP_CD_RAW_EOF_ITS_BSET_tu;



//IPP_CD_RAW_EOF_ITM
/*Description: Bit mask read register to enable the interrupt line ipp_camera_raw_cdg_eof_irq signalling an end of a grabbed frame, while raw data mode.Hard reset is 0, disabled interrupt.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CD_RAW_EOF_ITM :1;
    }IPP_CD_RAW_EOF_ITM_ts;

}IPP_CD_RAW_EOF_ITM_tu;



//IPP_CD_RAW_EOF_ITM_BCLR
/*Description: Writing 1 to this bit addressable mask register clears the corresponding bit interrupt mask IPP_CD_RAW_EOF_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CD_RAW_EOF_ITM_BCLR :1;
    }IPP_CD_RAW_EOF_ITM_BCLR_ts;

}IPP_CD_RAW_EOF_ITM_BCLR_tu;



//IPP_CD_RAW_EOF_ITM_BSET
/*Description: Writing 1 to this bit addressable mask register sets the corresponding bit interrupt mask IPP_CD_RAW_EOF_ITM.

Writing 0 has no impact.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CD_RAW_EOF_ITM_BSET :1;
    }IPP_CD_RAW_EOF_ITM_BSET_ts;

}IPP_CD_RAW_EOF_ITM_BSET_tu;



//IPP_CD_CCP_ERR_DBG_ITS
/*Description: For debug, provided additional error information from HW camera data grabber (CDG) about the CCP protocol violation.This register is updated either by internal HW when an error is detected (in the same time, one bit of IPP_CD_ERROR_ITS is HW internally set) or by an IPP_SD_RESET command. Hard reset value is 0.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t CD_CCP_ERR_DBG :3;
        /* CCP protocol error:*/
    }IPP_CD_CCP_ERR_DBG_ITS_ts;

}IPP_CD_CCP_ERR_DBG_ITS_tu;



//IPP_ISP_INTERNAL_ITS_L
/*Description: This register is the interrupt status register for ISP internal events.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t USER_IF :1;
        /* interrupt status*/
        uint8_t reserved0 :1;
        uint8_t MASTER_CCI :1;
        /* interrupt status*/
        uint8_t LBE :1;
        /* LINE BLANKING ELIMINATION interrupt status*/
        uint8_t REPIPE :1;
        /* interrupt status*/
        uint8_t ISP_RX :1;
        /* interrupt status*/
        uint8_t VIDEO_COMPLETE_PIPE0 :1;
        /* (OPF0) interrupt status*/
        uint8_t VIDEO_COMPLETE_PIPE1 :1;
        /* (OPF1) interrupt status*/
        uint8_t HISTOGRAM_256_BINS :1;
        /* interrupt status*/
        uint8_t EXPOSURE_6X8 :1;
        /* interrupt status*/
        uint8_t AF_STATS :1;
        /* interrupt status*/
        uint8_t WB_STATS :1;
        /* interrupt status*/
        uint8_t SKIN_TONE_STATS :1;
        /* interrupt status*/
        uint8_t DMCEPIPE :1;
        /* interrupt status*/
        uint8_t ARCTIC :1;
        /* interrupt status*/
        uint8_t ACCWGZONE :1;
        /* interrupt status*/
    }IPP_ISP_INTERNAL_ITS_L_ts;

}IPP_ISP_INTERNAL_ITS_L_tu;



//IPP_ISP_INTERNAL_ITS_BCLR_L
/*Description: This register is the interrupt status clear register for ISP internal events. Writing '1' to any of those bits clears the associated bit in ISP_INTERNAL_ITS_L status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t USER_IF :1;
        /* interrupt status clear*/
        uint8_t reserved0 :1;
        uint8_t MASTER_CCI :1;
        /* interrupt status clear*/
        uint8_t LBE :1;
        /* LINE BLANKING ELIMINATION interrupt status clear*/
        uint8_t REPIPE :1;
        /* interrupt status clear*/
        uint8_t ISP_RX :1;
        /* interrupt status clear*/
        uint8_t VIDEO_COMPLETE_PIPE0 :1;
        /* (OPF0) interrupt status clear*/
        uint8_t VIDEO_COMPLETE_PIPE1 :1;
        /* (OPF1) interrupt status clear*/
        uint8_t HISTOGRAM_256_BINS :1;
        /* interrupt status clear*/
        uint8_t EXPOSURE_6X8 :1;
        /* interrupt status clear*/
        uint8_t AF_STATS :1;
        /* interrupt status clear*/
        uint8_t WB_STATS :1;
        /* interrupt status clear*/
        uint8_t SKIN_TONE_STATS :1;
        /* interrupt status clear*/
        uint8_t DMCEPIPE :1;
        /* interrupt status clear*/
        uint8_t ARCTIC :1;
        /* interrupt status clear*/
        uint8_t ACCWGZONE :1;
        /* interrupt status clear*/
    }IPP_ISP_INTERNAL_ITS_BCLR_L_ts;

}IPP_ISP_INTERNAL_ITS_BCLR_L_tu;



//IPP_ISP_INTERNAL_ITS_BSET_L
/*Description: This register is the interrupt status set register for ISP internal events. Writing '1' to any of those bits sets the associated bit in ISP_INTERNAL_ITS_L status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t USER_IF :1;
        /* interrupt status set*/
        uint8_t reserved0 :1;
        uint8_t MASTER_CCI :1;
        /* interrupt status set*/
        uint8_t LBE :1;
        /* LINE BLANKING ELIMINATION interrupt status set*/
        uint8_t REPIPE :1;
        /* interrupt status set*/
        uint8_t ISP_RX :1;
        /* interrupt status set*/
        uint8_t VIDEO_COMPLETE_PIPE0 :1;
        /* (OPF0) interrupt status set*/
        uint8_t VIDEO_COMPLETE_PIPE1 :1;
        /* (OPF1) interrupt status set*/
        uint8_t HISTOGRAM_256_BINS :1;
        /* interrupt status set*/
        uint8_t EXPOSURE_6X8 :1;
        /* interrupt status set*/
        uint8_t AF_STATS :1;
        /* interrupt status set*/
        uint8_t WB_STATS :1;
        /* interrupt status set*/
        uint8_t SKIN_TONE_STATS :1;
        /* interrupt status set*/
        uint8_t DMCEPIPE :1;
        /* interrupt status set*/
        uint8_t ARCTIC :1;
        /* interrupt status set*/
        uint8_t ACCWGZONE :1;
        /* interrupt status set*/
    }IPP_ISP_INTERNAL_ITS_BSET_L_ts;

}IPP_ISP_INTERNAL_ITS_BSET_L_tu;



//IPP_ISP_INTERNAL_ITM_L
/*Description: This register is the interrupt mask register for ISP internal events.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t USER_IF :1;
        /* interrupt mask*/
        uint8_t reserved0 :1;
        uint8_t MASTER_CCI :1;
        /* interrupt mask*/
        uint8_t LBE :1;
        /* LINE BLANKING ELIMINATION interrupt mask*/
        uint8_t REPIPE :1;
        /* interrupt mask*/
        uint8_t ISP_RX :1;
        /* interrupt mask*/
        uint8_t VIDEO_COMPLETE_PIPE0 :1;
        /* (OPF0) interrupt mask*/
        uint8_t VIDEO_COMPLETE_PIPE1 :1;
        /* (OPF1) interrupt mask*/
        uint8_t HISTOGRAM_256_BINS :1;
        /* interrupt mask*/
        uint8_t EXPOSURE_6X8 :1;
        /* interrupt mask*/
        uint8_t AF_STATS :1;
        /* interrupt mask*/
        uint8_t WB_STATS :1;
        /* interrupt mask*/
        uint8_t SKIN_TONE_STATS :1;
        /* interrupt mask*/
        uint8_t DMCEPIPE :1;
        /* interrupt mask*/
        uint8_t ARCTIC :1;
        /* interrupt mask*/
        uint8_t ACCWGZONE :1;
        /* interrupt mask*/
    }IPP_ISP_INTERNAL_ITM_L_ts;

}IPP_ISP_INTERNAL_ITM_L_tu;



//IPP_ISP_INTERNAL_ITM_BCLR_L
/*Description: This register is the interrupt mask clear register for ISP internal events. Writing '1' to any of those bits clears the associated bit in ISP_INTERNAL_ITM_L mask register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t USER_IF :1;
        /* interrupt mask clear*/
        uint8_t reserved0 :1;
        uint8_t MASTER_CCI :1;
        /* interrupt mask clear*/
        uint8_t LBE :1;
        /* LINE BLANKING ELIMINATION interrupt mask clear*/
        uint8_t REPIPE :1;
        /* interrupt mask clear*/
        uint8_t ISP_RX :1;
        /* interrupt mask clear*/
        uint8_t VIDEO_COMPLETE_PIPE0 :1;
        /* (OPF0) interrupt mask clear*/
        uint8_t VIDEO_COMPLETE_PIPE1 :1;
        /* (OPF1) interrupt mask clear*/
        uint8_t HISTOGRAM_256_BINS :1;
        /* interrupt mask clear*/
        uint8_t EXPOSURE_6X8 :1;
        /* interrupt mask clear*/
        uint8_t AF_STATS :1;
        /* interrupt mask clear*/
        uint8_t WB_STATS :1;
        /* interrupt mask clear*/
        uint8_t SKIN_TONE_STATS :1;
        /* interrupt mask clear*/
        uint8_t DMCEPIPE :1;
        /* interrupt mask clear*/
        uint8_t ARCTIC :1;
        /* interrupt mask clear*/
        uint8_t ACCWGZONE :1;
        /* interrupt mask clear*/
    }IPP_ISP_INTERNAL_ITM_BCLR_L_ts;

}IPP_ISP_INTERNAL_ITM_BCLR_L_tu;



//IPP_ISP_INTERNAL_ITM_BSET_L
/*Description: This register is the interrupt mask set register for ISP internal events. Writing '1' to any of those bits sets the associated bit in ISP_INTERNAL_ITM_L mask register. Writing '0' has no effect
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t USER_IF :1;
        /* interrupt mask set*/
        uint8_t reserved0 :1;
        uint8_t MASTER_CCI :1;
        /* interrupt mask set*/
        uint8_t LBE :1;
        /* LINE BLANKING ELIMINATION interrupt mask set*/
        uint8_t REPIPE :1;
        /* interrupt mask set*/
        uint8_t ISP_RX :1;
        /* interrupt mask set*/
        uint8_t VIDEO_COMPLETE_PIPE0 :1;
        /* (OPF0) interrupt mask set*/
        uint8_t VIDEO_COMPLETE_PIPE1 :1;
        /* (OPF1) interrupt mask set*/
        uint8_t HISTOGRAM_256_BINS :1;
        /* interrupt mask set*/
        uint8_t EXPOSURE_6X8 :1;
        /* interrupt mask set*/
        uint8_t AF_STATS :1;
        /* interrupt mask set*/
        uint8_t WB_STATS :1;
        /* interrupt mask set*/
        uint8_t SKIN_TONE_STATS :1;
        /* interrupt mask set*/
        uint8_t DMCEPIPE :1;
        /* interrupt mask set*/
        uint8_t ARCTIC :1;
        /* interrupt mask set*/
        uint8_t ACCWGZONE :1;
        /* interrupt mask set*/
    }IPP_ISP_INTERNAL_ITM_BSET_L_ts;

}IPP_ISP_INTERNAL_ITM_BSET_L_tu;



//IPP_ISP_INTERNAL_ITS_H
/*Description: This register is the interrupt status register for ISP internal events.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t GLACE :1;
        /* interrupt status*/
        uint8_t GPIO :1;
        /* interrupt status*/
        uint8_t DMCE_LBE :1;
        /* interrupt status*/
        uint8_t SMIARX_3D :1;
        /* interrupt status*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt status*/
        uint8_t DXO_DPP :1;
        /* interrupt status*/
        uint8_t DXO_DOP7 :1;
        /* interrupt status*/
        uint8_t DXO_INT :1;
        /* interrupt status*/
    }IPP_ISP_INTERNAL_ITS_H_ts;

}IPP_ISP_INTERNAL_ITS_H_tu;



//IPP_ISP_INTERNAL_ITS_BCLR_H
/*Description: This register is the interrupt status clear register for ISP internal events. Writing '1' to any of those bits clears the associated bit in ISP_INTERNAL_ITS_H status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t GLACE :1;
        /* interrupt status clear*/
        uint8_t GPIO :1;
        /* interrupt status clear*/
        uint8_t DMCE_LBE :1;
        /* interrupt status clear*/
        uint8_t SMIARX_3D :1;
        /* interrupt status clear*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt status clear*/
        uint8_t DXO_DPP :1;
        /* interrupt status clear*/
        uint8_t DXO_DOP7 :1;
        /* interrupt status clear*/
        uint8_t DXO_INT :1;
        /* interrupt status clear*/
    }IPP_ISP_INTERNAL_ITS_BCLR_H_ts;

}IPP_ISP_INTERNAL_ITS_BCLR_H_tu;



//IPP_ISP_INTERNAL_ITS_BSET_H
/*Description: This register is the interrupt status set register for ISP internal events. Writing '1' to any of those bits sets the associated bit in ISP_INTERNAL_ITS_H status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t GLACE :1;
        /* interrupt status set*/
        uint8_t GPIO :1;
        /* interrupt status set*/
        uint8_t DMCE_LBE :1;
        /* interrupt status set*/
        uint8_t SMIARX_3D :1;
        /* interrupt status set*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt status set*/
        uint8_t DXO_DPP :1;
        /* interrupt status set*/
        uint8_t DXO_DOP7 :1;
        /* interrupt status set*/
        uint8_t DXO_INT :1;
        /* interrupt status set*/
    }IPP_ISP_INTERNAL_ITS_BSET_H_ts;

}IPP_ISP_INTERNAL_ITS_BSET_H_tu;



//IPP_ISP_INTERNAL_ITM_H
/*Description: This register is the interrupt mask register for ISP internal events.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t GLACE :1;
        /* interrupt mask*/
        uint8_t GPIO :1;
        /* interrupt mask*/
        uint8_t DMCE_LBE :1;
        /* interrupt mask*/
        uint8_t SMIARX_3D :1;
        /* interrupt mask*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt mask*/
        uint8_t DXO_DPP :1;
        /* interrupt mask*/
        uint8_t DXO_DOP7 :1;
        /* interrupt mask*/
        uint8_t DXO_INT :1;
        /* interrupt mask*/
    }IPP_ISP_INTERNAL_ITM_H_ts;

}IPP_ISP_INTERNAL_ITM_H_tu;



//IPP_ISP_INTERNAL_ITM_BCLR_H
/*Description: This register is the interrupt mask clear register for ISP internal events. Writing '1' to any of those bits clears the associated bit in ISP_INTERNAL_ITM_H mask register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t GLACE :1;
        /* interrupt mask clear*/
        uint8_t GPIO :1;
        /* interrupt mask clear*/
        uint8_t DMCE_LBE :1;
        /* interrupt mask clear*/
        uint8_t SMIARX_3D :1;
        /* interrupt mask clear*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt mask clear*/
        uint8_t DXO_DPP :1;
        /* interrupt mask clear*/
        uint8_t DXO_DOP7 :1;
        /* interrupt mask clear*/
        uint8_t DXO_INT :1;
        /* interrupt mask clear*/
    }IPP_ISP_INTERNAL_ITM_BCLR_H_ts;

}IPP_ISP_INTERNAL_ITM_BCLR_H_tu;



//IPP_ISP_INTERNAL_ITM_BSET_H
/*Description: This register is the interrupt mask set register for ISP internal events. Writing '1' to any of those bits sets the associated bit in ISP_INTERNAL_ITM_H mask register. Writing '0' has no effect.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t GLACE :1;
        /* interrupt mask set*/
        uint8_t GPIO :1;
        /* interrupt mask set*/
        uint8_t DMCE_LBE :1;
        /* interrupt mask set*/
        uint8_t SMIARX_3D :1;
        /* interrupt mask set*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt mask set*/
        uint8_t DXO_DPP :1;
        /* interrupt mask set*/
        uint8_t DXO_DOP7 :1;
        /* interrupt mask set*/
        uint8_t DXO_INT :1;
        /* interrupt mask set*/
    }IPP_ISP_INTERNAL_ITM_BSET_H_ts;

}IPP_ISP_INTERNAL_ITM_BSET_H_tu;



//IPP_SIA_ITEXT_ITS
/*Description: This register is the interrupt status register for SIA external interrupts to ISP MCU.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t ITEXT0 :1;
        /* interrupt status*/
        uint8_t ITEXT1 :1;
        /* interrupt status*/
        uint8_t ITEXT2 :1;
        /* interrupt status*/
        uint8_t ITEXT3 :1;
        /* interrupt status*/
        uint8_t ITEXT4 :1;
        /* interrupt status*/
        uint8_t ITEXT5 :1;
        /* interrupt status*/
        uint8_t ITEXT6 :1;
        /* interrupt status*/
        uint8_t ITEXT7 :1;
        /* interrupt status*/
        uint8_t ITEXT8 :1;
        /* interrupt status*/
        uint8_t ITEXT9 :1;
        /* interrupt status*/
        uint8_t ITEXT10 :1;
        /* interrupt status*/
        uint8_t ITEXT11 :1;
        /* interrupt status*/
        uint8_t ITEXT12 :1;
        /* interrupt status*/
    }IPP_SIA_ITEXT_ITS_ts;

}IPP_SIA_ITEXT_ITS_tu;



//IPP_SIA_ITEXT_ITS_BCLR
/*Description: This register is the interrupt status clear register for SIA external events. Writing '1' to any of those bits clears the associated bit in SIA_ITEXT_ITS status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t ITEXT0 :1;
        /* interrupt status clear*/
        uint8_t ITEXT1 :1;
        /* interrupt status clear*/
        uint8_t ITEXT2 :1;
        /* interrupt status clear*/
        uint8_t ITEXT3 :1;
        /* interrupt status clear*/
        uint8_t ITEXT4 :1;
        /* interrupt status clear*/
        uint8_t ITEXT5 :1;
        /* interrupt status clear*/
        uint8_t ITEXT6 :1;
        /* interrupt status clear*/
        uint8_t ITEXT7 :1;
        /* interrupt status clear*/
        uint8_t ITEXT8 :1;
        /* interrupt status clear*/
        uint8_t ITEXT9 :1;
        /* interrupt status clear*/
        uint8_t ITEXT10 :1;
        /* interrupt status clear*/
        uint8_t ITEXT11 :1;
        /* interrupt status clear*/
        uint8_t ITEXT12 :1;
        /* interrupt status clear*/
    }IPP_SIA_ITEXT_ITS_BCLR_ts;

}IPP_SIA_ITEXT_ITS_BCLR_tu;



//IPP_SIA_ITEXT_ITS_BSET
/*Description: This register is the interrupt status set register for SIA external events. Writing '1' to any of those bits sets the associated bit in SIA_ITEXT_ITS status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t ITEXT0 :1;
        /* interrupt status set*/
        uint8_t ITEXT1 :1;
        /* interrupt status set*/
        uint8_t ITEXT2 :1;
        /* interrupt status set*/
        uint8_t ITEXT3 :1;
        /* interrupt status set*/
        uint8_t ITEXT4 :1;
        /* interrupt status set*/
        uint8_t ITEXT5 :1;
        /* interrupt status set*/
        uint8_t ITEXT6 :1;
        /* interrupt status set*/
        uint8_t ITEXT7 :1;
        /* interrupt status set*/
        uint8_t ITEXT8 :1;
        /* interrupt status set*/
        uint8_t ITEXT9 :1;
        /* interrupt status set*/
        uint8_t ITEXT10 :1;
        /* interrupt status set*/
        uint8_t ITEXT11 :1;
        /* interrupt status set*/
        uint8_t ITEXT12 :1;
        /* interrupt status set*/
    }IPP_SIA_ITEXT_ITS_BSET_ts;

}IPP_SIA_ITEXT_ITS_BSET_tu;



//IPP_SIA_ITEXT_ITM
/*Description: This register is the interrupt mask register for SIA external events.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t ITEXT0 :1;
        /* interrupt mask*/
        uint8_t ITEXT1 :1;
        /* interrupt mask*/
        uint8_t ITEXT2 :1;
        /* interrupt mask*/
        uint8_t ITEXT3 :1;
        /* interrupt mask*/
        uint8_t ITEXT4 :1;
        /* interrupt mask*/
        uint8_t ITEXT5 :1;
        /* interrupt mask*/
        uint8_t ITEXT6 :1;
        /* interrupt mask*/
        uint8_t ITEXT7 :1;
        /* interrupt mask*/
        uint8_t ITEXT8 :1;
        /* interrupt mask*/
        uint8_t ITEXT9 :1;
        /* interrupt mask*/
        uint8_t ITEXT10 :1;
        /* interrupt mask*/
        uint8_t ITEXT11 :1;
        /* interrupt mask*/
        uint8_t ITEXT12 :1;
        /* interrupt mask*/
    }IPP_SIA_ITEXT_ITM_ts;

}IPP_SIA_ITEXT_ITM_tu;



//IPP_SIA_ITEXT_ITM_BCLR
/*Description: This register is the interrupt mask clear register for SIA external events. Writing '1' to any of those bits clears the associated bit in SIA_ITEXT_ITM mask register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t ITEXT0 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT1 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT2 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT3 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT4 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT5 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT6 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT7 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT8 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT9 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT10 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT11 :1;
        /* interrupt mask clear*/
        uint8_t ITEXT12 :1;
        /* interrupt mask clear*/
    }IPP_SIA_ITEXT_ITM_BCLR_ts;

}IPP_SIA_ITEXT_ITM_BCLR_tu;



//IPP_SIA_ITEXT_ITM_BSET
/*Description: This register is the interrupt mask set register for SIA external events. Writing '1' to any of those bits sets the associated bit in SIA_ITEXT_ITM mask register. Writing '0' has no effect.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t ITEXT0 :1;
        /* interrupt mask set*/
        uint8_t ITEXT1 :1;
        /* interrupt mask set*/
        uint8_t ITEXT2 :1;
        /* interrupt mask set*/
        uint8_t ITEXT3 :1;
        /* interrupt mask set*/
        uint8_t ITEXT4 :1;
        /* interrupt mask set*/
        uint8_t ITEXT5 :1;
        /* interrupt mask set*/
        uint8_t ITEXT6 :1;
        /* interrupt mask set*/
        uint8_t ITEXT7 :1;
        /* interrupt mask set*/
        uint8_t ITEXT8 :1;
        /* interrupt mask set*/
        uint8_t ITEXT9 :1;
        /* interrupt mask set*/
        uint8_t ITEXT10 :1;
        /* interrupt mask set*/
        uint8_t ITEXT11 :1;
        /* interrupt mask set*/
        uint8_t ITEXT12 :1;
        /* interrupt mask set*/
    }IPP_SIA_ITEXT_ITM_BSET_ts;

}IPP_SIA_ITEXT_ITM_BSET_tu;



//IPP_EVT_ITS
/*Description: This register is the interrupt status register for IPP events to ISP MCU.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t SD_ERROR :1;
        /* Camera pipe errors interrupt status*/
        uint8_t CD_ERROR :1;
        /* Camera pipe errors interrupt status*/
        uint8_t CD_RAW_EOF :1;
        /* Camera mode raw-data end-of-frame interrupt status*/
        uint8_t BML_EOF :1;
        /* Bayer Memory Load end-of-frame interrupt status*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt status*/
        uint8_t DXO_DPP :1;
        /* interrupt status*/
        uint8_t DXO_DOP7 :1;
        /* interrupt status*/
        uint8_t DXO_INT :1;
        /* interrupt status*/
    }IPP_EVT_ITS_ts;

}IPP_EVT_ITS_tu;



//IPP_EVT_ITS_BCLR
/*Description: This register is the interrupt status clear register for IPP events to ISP MCU. Writing '1' to any of those bits clears the associated bit in IPP_EVT_ITS status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t SD_ERROR :1;
        /* Camera pipe errors interrupt status clear*/
        uint8_t CD_ERROR :1;
        /* Camera pipe errors interrupt status clear*/
        uint8_t CD_RAW_EOF :1;
        /* Camera mode raw-data end-of-frame interrupt status clear*/
        uint8_t BML_EOF :1;
        /* Bayer Memory Load end-of-frame interrupt status clear*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt status clear*/
        uint8_t DXO_DPP :1;
        /* interrupt status clear*/
        uint8_t DXO_DOP7 :1;
        /* interrupt status clear*/
        uint8_t DXO_INT :1;
        /* interrupt status clear*/
    }IPP_EVT_ITS_BCLR_ts;

}IPP_EVT_ITS_BCLR_tu;



//IPP_EVT_ITS_BSET
/*Description: This register is the interrupt status set register for IPP events for ISP MCU. Writing '1' to any of those bits sets the associated bit in IPP_EVT_ITS status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t SD_ERROR :1;
        /* Camera pipe errors interrupt status set*/
        uint8_t CD_ERROR :1;
        /* Camera pipe errors interrupt status set*/
        uint8_t CD_RAW_EOF :1;
        /* Camera mode raw-data end-of-frame interrupt status set*/
        uint8_t BML_EOF :1;
        /* Bayer Memory Load end-of-frame interrupt status set*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt status set*/
        uint8_t DXO_DPP :1;
        /* interrupt status set*/
        uint8_t DXO_DOP7 :1;
        /* interrupt status set*/
        uint8_t DXO_INT :1;
        /* interrupt status set*/
    }IPP_EVT_ITS_BSET_ts;

}IPP_EVT_ITS_BSET_tu;



//IPP_EVT_ITM
/*Description: This register is the interrupt mask register for IPP events for ISP MCU.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t SD_ERROR :1;
        /* Camera pipe errors interrupt mask*/
        uint8_t CD_ERROR :1;
        /* Camera pipe errors interrupt mask*/
        uint8_t CD_RAW_EOF :1;
        /* Camera mode raw-data end-of-frame interrupt mask*/
        uint8_t BML_EOF :1;
        /* Bayer Memory Load end-of-frame interrupt mask*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt mask*/
        uint8_t DXO_DPP :1;
        /* interrupt mask*/
        uint8_t DXO_DOP7 :1;
        /* interrupt mask*/
        uint8_t DXO_INT :1;
        /* interrupt mask*/
    }IPP_EVT_ITM_ts;

}IPP_EVT_ITM_tu;



//IPP_EVT_ITM_BCLR
/*Description: This register is the interrupt mask clear register for IPP events for ISP MCU. Writing '1' to any of those bits clears the associated bit in IPP_EVT_ITM mask register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t SD_ERROR :1;
        /* Camera pipe errors interrupt mask clear*/
        uint8_t CD_ERROR :1;
        /* Camera pipe errors interrupt mask clear*/
        uint8_t CD_RAW_EOF :1;
        /* Camera mode raw-data end-of-frame interrupt mask clear*/
        uint8_t BML_EOF :1;
        /* Bayer Memory Load end-of-frame interrupt mask clear*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt mask clear*/
        uint8_t DXO_DPP :1;
        /* interrupt mask clear*/
        uint8_t DXO_DOP7 :1;
        /* interrupt mask clear*/
        uint8_t DXO_INT :1;
        /* interrupt mask clear*/
    }IPP_EVT_ITM_BCLR_ts;

}IPP_EVT_ITM_BCLR_tu;



//IPP_EVT_ITM_BSET
/*Description: This register is the interrupt mask set register for IPP events for ISP MCU. Writing '1' to any of those bits sets the associated bit in IPP_EVT_ITM mask register. Writing '0' has no effect.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t SD_ERROR :1;
        /* Camera pipe errors interrupt mask set*/
        uint8_t CD_ERROR :1;
        /* Camera pipe errors interrupt mask set*/
        uint8_t CD_RAW_EOF :1;
        /* Camera mode raw-data end-of-frame interrupt mask set*/
        uint8_t BML_EOF :1;
        /* Bayer Memory Load end-of-frame interrupt mask set*/
        uint8_t reserved0 :4;
        uint8_t DXO_PDP :1;
        /* interrupt mask set*/
        uint8_t DXO_DPP :1;
        /* interrupt mask set*/
        uint8_t DXO_DOP7 :1;
        /* interrupt mask set*/
        uint8_t DXO_INT :1;
        /* interrupt mask set*/
    }IPP_EVT_ITM_BSET_ts;

}IPP_EVT_ITM_BSET_tu;



//IPP_STATIC_TOP_IF_3D_SEL
/*Description: Static register to select which CSI2 camera data interface is connected to the dual camera datapath This static register must be written only when IPP_DPHY_TOP_IF_EN=0. When in 'legacy' use case (i.e. not involving simultaneous captures from 2 cameras), this is required to use the 'regular' path and not the '3D' path.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_TOP_IF_3D_SEL :2;
        /* CSI0 (primary interface)*/
    }IPP_STATIC_TOP_IF_3D_SEL_ts;

}IPP_STATIC_TOP_IF_3D_SEL_tu;



//IPP_CSI2RX_3D_VC_DATA_TYPE_R
/*Description: used to observe the values of the current virtual_channel and the current data_type (image or embedded), given by the CSI2RX HW decoder module of the dual camera datapath.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI2RX_3D_DATA_TYPE :6;
        uint8_t CSI2RX_3D_VIRTUAL_CHANNEL :2;
    }IPP_CSI2RX_3D_VC_DATA_TYPE_R_ts;

}IPP_CSI2RX_3D_VC_DATA_TYPE_R_tu;



//IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W
/*Description: Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module of the dual camera datapath.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0..
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_3D_DATA_TYPE0 :6;
        uint8_t reserved0 :2;
        uint8_t STATIC_CSI2RX_3D_DATA_TYPE1 :6;
    }IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_ts;

}IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_tu;



//IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W
/*Description: Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module of the dual camera datapath N.ote that if only 2 different data types are to be received, the same value should be programmed to 2 of the 3 values. If only one single data type is to be received, the this value should be set for all the three different data types

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.

.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_3D_DATA_TYPE2 :6;
    }IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W_ts;

}IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W_tu;



//IPP_STATIC_CSI2RX_3D_VC_W
/*Description: Static bit register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module for enabling any of up to 4 possible concurrent virtual channel(s).

Hard reset is 1, i.e. only virtual channel 0 enabled.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_3D_VC0_EN :1;
        /* virtual channel 0 is grabbed.*/
        uint8_t STATIC_CSI2RX_3D_VC1_EN :1;
        /* virtual channel 1 is grabbed*/
        uint8_t STATIC_CSI2RX_3D_VC2_EN :1;
        /* virtual channel 2 is grabbed*/
        uint8_t STATIC_CSI2RX_3D_VC3_EN :1;
        /* virtual channel 3 is grabbed*/
    }IPP_STATIC_CSI2RX_3D_VC_W_ts;

}IPP_STATIC_CSI2RX_3D_VC_W_tu;



//IPP_CSI2RX_3D_FRAME_NB_R
/*Description: used to observe the current frame number, extracted by the CSI2RX from the last frame start 'short packet data field' on the dual camera data path.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CSI2RX_3D_FRAME_NB;
    }IPP_CSI2RX_3D_FRAME_NB_R_ts;

}IPP_CSI2RX_3D_FRAME_NB_R_tu;



//IPP_CSI2RX_3D_DATA_FIELD_R
/*Description: used to observe the current data field, extracted by the CSI2RX from the last line start 'short packet data field on the dual camera datapath'
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CSI2RX_3D_DATA_FIELD;
    }IPP_CSI2RX_3D_DATA_FIELD_R_ts;

}IPP_CSI2RX_3D_DATA_FIELD_R_tu;



//IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W
/*Description: Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module of the dual camera datapath for the corresponding 3 possible data types.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_3D_DATATYPE0_PIXWIDTH :4;
        uint8_t STATIC_CSI2RX_3D_DATATYPE1_PIXWIDTH :4;
        uint8_t STATIC_CSI2RX_3D_DATATYPE2_PIXWIDTH :4;
    }IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_ts;

}IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_tu;



//IPP_STATIC_DELAYM_EN
/*Description: Static register used to enable the delay monitoring between the 2 CSI2 camera.This enable must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_DELAYM_EN :1;
    }IPP_STATIC_DELAYM_EN_ts;

}IPP_STATIC_DELAYM_EN_tu;



//IPP_DELAYM_LINE_DELAY
/*Description: Enable obervation of the master camera (first having a vsync rising edge) and the line delay between master and slave sensor.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t LINE_DELAY_COUNTER :13;
        uint8_t reserved0 :2;
        uint8_t MASTER_ID :1;
        /* CSI0 (primary interface)*/
    }IPP_DELAYM_LINE_DELAY_ts;

}IPP_DELAYM_LINE_DELAY_tu;



//IPP_DELAYM_PIXEL_DELAY
/*Description: Enable obervation of the pixel delay between the 2 CSI2.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t PIXEL_DELAY_COUNTER :13;
    }IPP_DELAYM_PIXEL_DELAY_ts;

}IPP_DELAYM_PIXEL_DELAY_tu;



//IPP_SD_STATIC_SDG_3D_CSI_MODE
/*Description: Static configuration register bit for SDG_3D in CSI mode, in order to set SDG CSI mode either into :

0: SMIA over CSI2 mode (default) : embedded data are padded and packed as defined by SMIA specification. The number of bits per pixel is the same for embedded data and for active data. Also a line length is the same both for an embedded lines and an active one within a frame.

1: CSI2 specific mode: embedded data are always 8-bit data. And possibly a line length is not the same between an embedded line and an active one.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0..
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_SDG_3D_CSI_MODE :1;
        /* SMIA over CSI2 mode*/
    }IPP_SD_STATIC_SDG_3D_CSI_MODE_ts;

}IPP_SD_STATIC_SDG_3D_CSI_MODE_tu;



//IPP_CSI2_DPHY0_DL4_CTRL
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_SWAP_PINS_DL4 :1;
        uint8_t CSI0_HS_INVERT_DL4 :1;
        uint8_t CSI0_FORCE_RX_MODE_DL4 :1;
        uint8_t CSI0_CD_OFF_DL4 :1;
        uint8_t CSI0_EOT_BYPASS_DL4 :1;
        uint8_t CSI0_HSRX_TERM_SHIFT_UP_DL4 :1;
        uint8_t CSI0_HSRX_TERM_SHIFT_DOWN_DL4 :1;
        uint8_t CSI0_TEST_RESERVED_1_DL4 :1;
    }IPP_CSI2_DPHY0_DL4_CTRL_ts;

}IPP_CSI2_DPHY0_DL4_CTRL_tu;



//IPP_CSI2_DPHY0_DL4_DBG
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_HS_RX_OFFSET_DL4 :3;
        uint8_t CSI0_MIPI_IN_SHORT_DL4 :1;
        uint8_t CSI0_SKEW_DL4 :3;
        uint8_t CSI0_OVERSAMPLE_BYPASS_DL4 :1;
        uint8_t CSI0_LP_HS_BYPASS_DL4 :1;
        uint8_t reserved0 :2;
        uint8_t CSI0_DIRECT_DYN_ACCES_DL4 :1;
    }IPP_CSI2_DPHY0_DL4_DBG_ts;

}IPP_CSI2_DPHY0_DL4_DBG_tu;



//IPP_CSI2_DPHY0_DBG2
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_THSSETTLE_TIMER :3;
        uint8_t reserved0 :1;
        uint8_t CSI0_TIMER_SELECT_DL1 :1;
        uint8_t CSI0_TIMER_SELECT_DL2 :1;
        uint8_t CSI0_TIMER_SELECT_DL3 :1;
        uint8_t CSI0_TIMER_SELECT_DL4 :1;
    }IPP_CSI2_DPHY0_DBG2_ts;

}IPP_CSI2_DPHY0_DBG2_tu;



//IPP_CSI2_DPHY1_DBG2
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI1_THSSETTLE_TIMER :3;
        uint8_t reserved0 :1;
        uint8_t CSI1_TIMER_SELECT_DL1 :1;
    }IPP_CSI2_DPHY1_DBG2_ts;

}IPP_CSI2_DPHY1_DBG2_tu;



//IPP_CSI2_DPHY0_DL4_ERROR_ITS
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI0_ERR_EOT_SYNC_HS_DL4 :1;
        uint8_t CSI0_ERR_SOT_HS_DL4 :1;
        uint8_t CSI0_ERR_SOT_SYNC_HS_DL4 :1;
        uint8_t CSI0_ERR_CONTROL_DL4 :1;
    }IPP_CSI2_DPHY0_DL4_ERROR_ITS_ts;

}IPP_CSI2_DPHY0_DL4_ERROR_ITS_tu;



//IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_DPHY_ERROR_ITS_BCLR :4;
    }IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR_ts;

}IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR_tu;



//IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_DPHY_ERROR_ITS_BSET :4;
    }IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET_ts;

}IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET_tu;



//IPP_CSI2_DPHY0_DL4_ERROR_ITM
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_DPHY_ERROR_ITM :4;
    }IPP_CSI2_DPHY0_DL4_ERROR_ITM_ts;

}IPP_CSI2_DPHY0_DL4_ERROR_ITM_tu;



//IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_DPHY_ERROR_ITM_BCLR :4;
    }IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR_ts;

}IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR_tu;



//IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI_DPHY_ERROR_ITM_BSET :4;
    }IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET_ts;

}IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET_tu;



//IPP_CSI2RX_VC_DATA_TYPE_R
/*Description: used to observe the values of the current virtual_channel and the current data_type (image or embedded), given by the CSI2RX HW decoder module of the dual camera datapath.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t CSI2RX_DATA_TYPE :6;
        uint8_t CSI2RX_VIRTUAL_CHANNEL :2;
    }IPP_CSI2RX_VC_DATA_TYPE_R_ts;

}IPP_CSI2RX_VC_DATA_TYPE_R_tu;



//IPP_STATIC_CSI2RX_DATA_TYPE_1_W
/*Description: Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module of the dual camera datapath.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0..
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_DATA_TYPE0 :6;
        uint8_t reserved0 :2;
        uint8_t STATIC_CSI2RX_DATA_TYPE1 :6;
    }IPP_STATIC_CSI2RX_DATA_TYPE_1_W_ts;

}IPP_STATIC_CSI2RX_DATA_TYPE_1_W_tu;



//IPP_STATIC_CSI2RX_DATA_TYPE_2_W
/*Description: Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module of the dual camera datapath N.ote that if only 2 different data types are to be received, the same value should be programmed to 2 of the 3 values. If only one single data type is to be received, the this value should be set for all the three different data types

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.

.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_DATA_TYPE2 :6;
    }IPP_STATIC_CSI2RX_DATA_TYPE_2_W_ts;

}IPP_STATIC_CSI2RX_DATA_TYPE_2_W_tu;



//IPP_STATIC_CSI2RX_VC_W
/*Description: Static bit register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module for enabling any of up to 4 possible concurrent virtual channel(s).

Hard reset is 1, i.e. only virtual channel 0 enabled.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_VC0_EN :1;
        /* virtual channel 0 is grabbed.*/
        uint8_t STATIC_CSI2RX_VC1_EN :1;
        /* virtual channel 1 is grabbed*/
        uint8_t STATIC_CSI2RX_VC2_EN :1;
        /* virtual channel 2 is grabbed*/
        uint8_t STATIC_CSI2RX_VC3_EN :1;
        /* virtual channel 3 is grabbed*/
    }IPP_STATIC_CSI2RX_VC_W_ts;

}IPP_STATIC_CSI2RX_VC_W_tu;



//IPP_CSI2RX_FRAME_NB_R
/*Description: used to observe the current frame number, extracted by the CSI2RX from the last frame start 'short packet data field' on the dual camera data path.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CSI2RX_FRAME_NB;
    }IPP_CSI2RX_FRAME_NB_R_ts;

}IPP_CSI2RX_FRAME_NB_R_tu;



//IPP_CSI2RX_DATA_FIELD_R
/*Description: used to observe the current data field, extracted by the CSI2RX from the last line start 'short packet data field on the dual camera datapath'
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CSI2RX_DATA_FIELD;
    }IPP_CSI2RX_DATA_FIELD_R_ts;

}IPP_CSI2RX_DATA_FIELD_R_tu;



//IPP_STATIC_CSI2RX_PIX_WIDTH_W_new
/*Description: Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module of the dual camera datapath for the corresponding 3 possible data types.

This static register must be written only when IPP_DPHY_TOP_IF_EN=0.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t STATIC_CSI2RX_DATATYPE0_PIXWIDTH :4;
        uint8_t STATIC_CSI2RX_DATATYPE1_PIXWIDTH :4;
        uint8_t STATIC_CSI2RX_DATATYPE2_PIXWIDTH :4;
    }IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_ts;

}IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_tu;


//IPP_DXO_DPP_ITM
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DPP_ENDOFBOOT_ITM :1;
        uint8_t IPP_DXO_DPP_ENDOFEXECCMD_ITM :1;
        uint8_t IPP_DXO_DPP_ENDOFPROCESSING_ITM :1;
        uint8_t IPP_DXO_DPP_NEWFRAMECMDACK_ITM :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEERROR_ITM :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM :1;
        uint8_t IPP_DXO_DPP_PULSEOUT0_ITM :1;
    }IPP_DXO_DPP_ITM_ts;

}IPP_DXO_DPP_ITM_tu;



//IPP_BML_ITS
/*Description: BML interrupt status. Logs events generated by the BML.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BML_EOF :1;
        /* BML end-of-task. Notifies when BML has completed sending last pixel to the ISP.*/
    }IPP_BML_ITS_ts;

}IPP_BML_ITS_tu;



//IPP_BML_ITS_BCLR
/*Description: This register is the BML interrupt status clear register. Writing '1' to any of those bits clears the associated bit in IPP_BML_ITS status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BML_EOF :1;
        /* BML end-of-task.*/
    }IPP_BML_ITS_BCLR_ts;

}IPP_BML_ITS_BCLR_tu;



//IPP_BML_ITS_BSET
/*Description: This register is the BML interrupt status set register. Writing '1' to any of those bits sets the associated bit in IPP_BML_ITS status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BML_EOF :1;
        /* BML end-of-task.*/
    }IPP_BML_ITS_BSET_ts;

}IPP_BML_ITS_BSET_tu;



//IPP_BML_ITM
/*Description: BML interrupt mask. Masks events generated by the BML.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BML_EOF :1;
        /* BML end-of-task.*/
    }IPP_BML_ITM_ts;

}IPP_BML_ITM_tu;



//IPP_BML_ITM_BCLR
/*Description: This register is the BML interrupt mask clear register. Writing '1' to any of those bits clears the associated bit in IPP_BML_ITM mask register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BML_EOF :1;
        /* BML end-of-task.*/
    }IPP_BML_ITM_BCLR_ts;

}IPP_BML_ITM_BCLR_tu;



//IPP_BML_ITM_BSET
/*Description: This register is the BML interrupt mask set register. Writing '1' to any of those bits sets the associated bit in IPP_BML_ITM mask register. Writing '0' has no effect.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t BML_EOF :1;
        /* BML end-of-task.*/
    }IPP_BML_ITM_BSET_ts;

}IPP_BML_ITM_BSET_tu;



//IPP_SD_BMS_ISL_TOP_H
/*Description: This register defines the number of lines to be captured as part of the top ISL (Intelligent Status Lines).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t HEIGHT :5;
        /* Height (in lines) of the top ISLs starting from SOF line (included).*/
    }IPP_SD_BMS_ISL_TOP_H_ts;

}IPP_SD_BMS_ISL_TOP_H_tu;



//IPP_SD_BMS_ISL_BOT_H
/*Description: This register defines the number of lines to be captured as part of the bottom ISL (Intelligent Status Lines).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t HEIGHT :5;
        /* Height (in lines) of the bottom ISLs starting from LAST_ACTIVE line (excluded).*/
    }IPP_SD_BMS_ISL_BOT_H_ts;

}IPP_SD_BMS_ISL_BOT_H_tu;



//IPP_SD_BMS_ISL_FILTER
/*Description: This register defines which lines are filtred in/out (Intelligent Status Lines).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t ISL_BOT :1;
        /* ISL_BOT lines enable*/
        uint8_t PIX_DATA :1;
        /* PIX_DATA lines enable*/
        uint8_t ISL_TOP :1;
        /* ISL_TOP lines enable*/
    }IPP_SD_BMS_ISL_FILTER_ts;

}IPP_SD_BMS_ISL_FILTER_tu;



//IPP_SD_BMS_ISL_FMT
/*Description: This register defines the format of the intelligent status lines (or native format of the pixels over the data link, before bayer decompression and depacking) .
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t FORMAT :4;
        /* Format of the ISL*/
    }IPP_SD_BMS_ISL_FMT_ts;

}IPP_SD_BMS_ISL_FMT_tu;



//IPP_SD_DXO_DATAPATH
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t ISP_IN :1;
        /* Source selection of ISP Bayer domain entry point*/
        uint8_t reserved0 :7;
        uint8_t DXO_IN :1;
        /* Source selection of DxO IP input*/
    }IPP_SD_DXO_DATAPATH_ts;

}IPP_SD_DXO_DATAPATH_tu;



//IPP_DXO_PDP_ITS
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_PDP_ENDOFBOOT_ITS :1;
        uint8_t IPP_DXO_PDP_ENDOFEXECCMD_ITS :1;
        uint8_t IPP_DXO_PDP_ENDOFPROCESSING_ITS :1;
        uint8_t IPP_DXO_PDP_NEWFRAMECMDACK_ITS :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEERROR_ITS :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS :1;
        uint8_t IPP_DXO_PDP_PULSEOUT0_ITS :1;
    }IPP_DXO_PDP_ITS_ts;

}IPP_DXO_PDP_ITS_tu;



//IPP_DXO_PDP_ITS_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_PDP_ENDOFBOOT_ITS_BCLR :1;
        uint8_t IPP_DXO_PDP_ENDOFEXECCMD_ITS_BCLR :1;
        uint8_t IPP_DXO_PDP_ENDOFPROCESSING_ITS_BCLR :1;
        uint8_t IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BCLR :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BCLR :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEERROR_ITS_BCLR :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BCLR :1;
        uint8_t IPP_DXO_PDP_PULSEOUT0_ITS_BCLR :1;
    }IPP_DXO_PDP_ITS_BCLR_ts;

}IPP_DXO_PDP_ITS_BCLR_tu;



//IPP_DXO_PDP_ITS_BSET
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_PDP_ENDOFBOOT_ITS_BSET :1;
        uint8_t IPP_DXO_PDP_ENDOFEXECCMD_ITS_BSET :1;
        uint8_t IPP_DXO_PDP_ENDOFPROCESSING_ITS_BSET :1;
        uint8_t IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BSET :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BSET :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEERROR_ITS_BSET :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BSET :1;
        uint8_t IPP_DXO_PDP_PULSEOUT0_ITS_BSET :1;
    }IPP_DXO_PDP_ITS_BSET_ts;

}IPP_DXO_PDP_ITS_BSET_tu;



//IPP_DXO_PDP_ITM
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_PDP_ENDOFBOOT_ITM :1;
        uint8_t IPP_DXO_PDP_ENDOFEXECCMD_ITM :1;
        uint8_t IPP_DXO_PDP_ENDOFPROCESSING_ITM :1;
        uint8_t IPP_DXO_PDP_NEWFRAMECMDACK_ITM :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEERROR_ITM :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM :1;
        uint8_t IPP_DXO_PDP_PULSEOUT0_ITM :1;
    }IPP_DXO_PDP_ITM_ts;

}IPP_DXO_PDP_ITM_tu;



//IPP_DXO_PDP_ITM_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_PDP_ENDOFBOOT_ITM_BCLR :1;
        uint8_t IPP_DXO_PDP_ENDOFEXECCMD_ITM_BCLR :1;
        uint8_t IPP_DXO_PDP_ENDOFPROCESSING_ITM_BCLR :1;
        uint8_t IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BCLR :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BCLR :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEERROR_ITM_BCLR :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BCLR :1;
        uint8_t IPP_DXO_PDP_PULSEOUT0_ITM_BCLR :1;
    }IPP_DXO_PDP_ITM_BCLR_ts;

}IPP_DXO_PDP_ITM_BCLR_tu;



//IPP_DXO_PDP_ITM_BSET
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_PDP_ENDOFBOOT_ITM_BSET :1;
        uint8_t IPP_DXO_PDP_ENDOFEXECCMD_ITM_BSET :1;
        uint8_t IPP_DXO_PDP_ENDOFPROCESSING_ITM_BSET :1;
        uint8_t IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BSET :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BSET :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEERROR_ITM_BSET :1;
        uint8_t IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BSET :1;
        uint8_t IPP_DXO_PDP_PULSEOUT0_ITM_BSET :1;
    }IPP_DXO_PDP_ITM_BSET_ts;

}IPP_DXO_PDP_ITM_BSET_tu;



//IPP_DXO_DPP_ITS
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DPP_ENDOFBOOT_ITS :1;
        uint8_t IPP_DXO_DPP_ENDOFEXECCMD_ITS :1;
        uint8_t IPP_DXO_DPP_ENDOFPROCESSING_ITS :1;
        uint8_t IPP_DXO_DPP_NEWFRAMECMDACK_ITS :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEERROR_ITS :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS :1;
        uint8_t IPP_DXO_DPP_PULSEOUT0_ITS :1;
    }IPP_DXO_DPP_ITS_ts;

}IPP_DXO_DPP_ITS_tu;



//IPP_DXO_DPP_ITS_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DPP_ENDOFBOOT_ITS_BCLR :1;
        uint8_t IPP_DXO_DPP_ENDOFEXECCMD_ITS_BCLR :1;
        uint8_t IPP_DXO_DPP_ENDOFPROCESSING_ITS_BCLR :1;
        uint8_t IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BCLR :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BCLR :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEERROR_ITS_BCLR :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BCLR :1;
        uint8_t IPP_DXO_DPP_PULSEOUT0_ITS_BCLR :1;
    }IPP_DXO_DPP_ITS_BCLR_ts;

}IPP_DXO_DPP_ITS_BCLR_tu;



//IPP_DXO_DPP_ITS_BSET
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DPP_ENDOFBOOT_ITS_BSET :1;
        uint8_t IPP_DXO_DPP_ENDOFEXECCMD_ITS_BSET :1;
        uint8_t IPP_DXO_DPP_ENDOFPROCESSING_ITS_BSET :1;
        uint8_t IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BSET :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BSET :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEERROR_ITS_BSET :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BSET :1;
        uint8_t IPP_DXO_DPP_PULSEOUT0_ITS_BSET :1;
    }IPP_DXO_DPP_ITS_BSET_ts;

}IPP_DXO_DPP_ITS_BSET_tu;



//IPP_DXO_DPP_ITM_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DPP_ENDOFBOOT_ITM_BCLR :1;
        uint8_t IPP_DXO_DPP_ENDOFEXECCMD_ITM_BCLR :1;
        uint8_t IPP_DXO_DPP_ENDOFPROCESSING_ITM_BCLR :1;
        uint8_t IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BCLR :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BCLR :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEERROR_ITM_BCLR :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BCLR :1;
        uint8_t IPP_DXO_DPP_PULSEOUT0_ITM_BCLR :1;
    }IPP_DXO_DPP_ITM_BCLR_ts;

}IPP_DXO_DPP_ITM_BCLR_tu;



//IPP_DXO_DPP_ITM_BSET
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DPP_ENDOFBOOT_ITM_BSET :1;
        uint8_t IPP_DXO_DPP_ENDOFEXECCMD_ITM_BSET :1;
        uint8_t IPP_DXO_DPP_ENDOFPROCESSING_ITM_BSET :1;
        uint8_t IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BSET :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BSET :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEERROR_ITM_BSET :1;
        uint8_t IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BSET :1;
        uint8_t IPP_DXO_DPP_PULSEOUT0_ITM_BSET :1;
    }IPP_DXO_DPP_ITM_BSET_ts;

}IPP_DXO_DPP_ITM_BSET_tu;



//IPP_DXO_DOP7_ITS
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DOP7_ENDOFBOOT_ITS :1;
        uint8_t IPP_DXO_DOP7_ENDOFEXECCMD_ITS :1;
        uint8_t IPP_DXO_DOP7_ENDOFPROCESSING_ITS :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMECMDACK_ITS :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEERROR_ITS :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS :1;
        uint8_t IPP_DXO_DOP7_PULSEOUT0_ITS :1;
    }IPP_DXO_DOP7_ITS_ts;

}IPP_DXO_DOP7_ITS_tu;



//IPP_DXO_DOP7_ITS_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DOP7_ENDOFBOOT_ITS_BCLR :1;
        uint8_t IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BCLR :1;
        uint8_t IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BCLR :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BCLR :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BCLR :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BCLR :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BCLR :1;
        uint8_t IPP_DXO_DOP7_PULSEOUT0_ITS_BCLR :1;
    }IPP_DXO_DOP7_ITS_BCLR_ts;

}IPP_DXO_DOP7_ITS_BCLR_tu;



//IPP_DXO_DOP7_ITS_BSET
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DOP7_ENDOFBOOT_ITS_BSET :1;
        uint8_t IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BSET :1;
        uint8_t IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BSET :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BSET :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BSET :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BSET :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BSET :1;
        uint8_t IPP_DXO_DOP7_PULSEOUT0_ITS_BSET :1;
    }IPP_DXO_DOP7_ITS_BSET_ts;

}IPP_DXO_DOP7_ITS_BSET_tu;



//IPP_DXO_DOP7_ITM
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DOP7_ENDOFBOOT_ITM :1;
        uint8_t IPP_DXO_DOP7_ENDOFEXECCMD_ITM :1;
        uint8_t IPP_DXO_DOP7_ENDOFPROCESSING_ITM :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMECMDACK_ITM :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEERROR_ITM :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM :1;
        uint8_t IPP_DXO_DOP7_PULSEOUT0_ITM :1;
    }IPP_DXO_DOP7_ITM_ts;

}IPP_DXO_DOP7_ITM_tu;



//IPP_DXO_DOP7_ITM_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DOP7_ENDOFBOOT_ITM_BCLR :1;
        uint8_t IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BCLR :1;
        uint8_t IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BCLR :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BCLR :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BCLR :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BCLR :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BCLR :1;
        uint8_t IPP_DXO_DOP7_PULSEOUT0_ITM_BCLR :1;
    }IPP_DXO_DOP7_ITM_BCLR_ts;

}IPP_DXO_DOP7_ITM_BCLR_tu;



//IPP_DXO_DOP7_ITM_BSET
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IPP_DXO_DOP7_ENDOFBOOT_ITM_BSET :1;
        uint8_t IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BSET :1;
        uint8_t IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BSET :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BSET :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BSET :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BSET :1;
        uint8_t IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BSET :1;
        uint8_t IPP_DXO_DOP7_PULSEOUT0_ITM_BSET :1;
    }IPP_DXO_DOP7_ITM_BSET_ts;

}IPP_DXO_DOP7_ITM_BSET_tu;

typedef struct
{
    IPP_DPHY_TOP_IF_EN_tu IPP_DPHY_TOP_IF_EN;
    IPP_STATIC_TOP_IF_SEL_tu IPP_STATIC_TOP_IF_SEL;
    IPP_STATIC_CCP_IF_tu IPP_STATIC_CCP_IF;
    IPP_INTERNAL_EN_CLK_CCP_tu IPP_INTERNAL_EN_CLK_CCP;
    IPP_DPHY_COMPENSATION_CTRL_tu IPP_DPHY_COMPENSATION_CTRL;
    IPP_DPHY_COMPENSATION_STATUS_tu IPP_DPHY_COMPENSATION_STATUS;
    IPP_STATIC_CSI0_DATA_LANES_tu IPP_STATIC_CSI0_DATA_LANES;
    IPP_CSI2_DPHY0_CL_CTRL_tu IPP_CSI2_DPHY0_CL_CTRL;
    IPP_CSI2_DPHY0_DL1_CTRL_tu IPP_CSI2_DPHY0_DL1_CTRL;
    IPP_CSI2_DPHY0_DL2_CTRL_tu IPP_CSI2_DPHY0_DL2_CTRL;
    IPP_CSI2_DPHY0_DL3_CTRL_tu IPP_CSI2_DPHY0_DL3_CTRL;
    IPP_CSI2_DPHY0_CL_DBG_tu IPP_CSI2_DPHY0_CL_DBG;
    IPP_CSI2_DPHY0_DL1_DBG_tu IPP_CSI2_DPHY0_DL1_DBG;
    IPP_CSI2_DPHY0_DL2_DBG_tu IPP_CSI2_DPHY0_DL2_DBG;
    IPP_CSI2_DPHY0_DL3_DBG_tu IPP_CSI2_DPHY0_DL3_DBG;
    IPP_CSI2_DPHY0_XL_DBG0_OBS_tu IPP_CSI2_DPHY0_XL_DBG0_OBS;
    IPP_CSI2_DPHY0_XL_DBG1_OBS_tu IPP_CSI2_DPHY0_XL_DBG1_OBS;
    IPP_CSI2_DPHY1_CL_CTRL_tu IPP_CSI2_DPHY1_CL_CTRL;
    IPP_CSI2_DPHY1_DL1_CTRL_tu IPP_CSI2_DPHY1_DL1_CTRL;
    IPP_CSI2_DPHY1_CL_DBG_tu IPP_CSI2_DPHY1_CL_DBG;
    IPP_CSI2_DPHY1_DL1_DBG_tu IPP_CSI2_DPHY1_DL1_DBG;
    IPP_CSI2_DPHY1_XL_DBG_OBS_tu IPP_CSI2_DPHY1_XL_DBG_OBS;
    IPP_CSI2_DPHY0_DBG_ITS_tu IPP_CSI2_DPHY0_DBG_ITS;
    IPP_CSI2_DPHY0_DBG_ITS_BCLR_tu IPP_CSI2_DPHY0_DBG_ITS_BCLR;
    IPP_CSI2_DPHY0_DBG_ITS_BSET_tu IPP_CSI2_DPHY0_DBG_ITS_BSET;
    IPP_CSI2_DPHY0_DBG_ITM_tu IPP_CSI2_DPHY0_DBG_ITM;
    IPP_CSI2_DPHY0_DBG_ITM_BCLR_tu IPP_CSI2_DPHY0_DBG_ITM_BCLR;
    IPP_CSI2_DPHY0_DBG_ITM_BSET_tu IPP_CSI2_DPHY0_DBG_ITM_BSET;
    IPP_CSI2_DPHY1_DBG_ITS_tu IPP_CSI2_DPHY1_DBG_ITS;
    IPP_CSI2_DPHY1_DBG_ITS_BCLR_tu IPP_CSI2_DPHY1_DBG_ITS_BCLR;
    IPP_CSI2_DPHY1_DBG_ITS_BSET_tu IPP_CSI2_DPHY1_DBG_ITS_BSET;
    IPP_CSI2_DPHY1_DBG_ITM_tu IPP_CSI2_DPHY1_DBG_ITM;
    IPP_CSI2_DPHY1_DBG_ITM_BCLR_tu IPP_CSI2_DPHY1_DBG_ITM_BCLR;
    IPP_CSI2_DPHY1_DBG_ITM_BSET_tu IPP_CSI2_DPHY1_DBG_ITM_BSET;
    IPP_CSI2_PACKET_DBG_ITS_tu IPP_CSI2_PACKET_DBG_ITS;
    IPP_CSI2_PACKET_DBG_ITS_BCLR_tu IPP_CSI2_PACKET_DBG_ITS_BCLR;
    IPP_CSI2_PACKET_DBG_ITS_BSET_tu IPP_CSI2_PACKET_DBG_ITS_BSET;
    IPP_CSI2_PACKET_DBG_ITM_tu IPP_CSI2_PACKET_DBG_ITM;
    IPP_CSI2_PACKET_DBG_ITM_BCLR_tu IPP_CSI2_PACKET_DBG_ITM_BCLR;
    IPP_CSI2_PACKET_DBG_ITM_BSET_tu IPP_CSI2_PACKET_DBG_ITM_BSET;
    DEPRECATED_IPP1_tu DEPRECATED_IPP1;
    IPP_STATIC_CSI2RX_DATA_TYPE_W_tu IPP_STATIC_CSI2RX_DATA_TYPE_W;
    DEPRECATED_IPP3_tu DEPRECATED_IPP3;
    DEPRECATED_IPP4_tu DEPRECATED_IPP4;
    DEPRECATED_IPP5_tu DEPRECATED_IPP5;
    IPP_STATIC_CSI2RX_PIX_WIDTH_W_tu IPP_STATIC_CSI2RX_PIX_WIDTH_W;
    IPP_CSI2_DPHY_ERROR_ITS_tu IPP_CSI2_DPHY_ERROR_ITS;
    IPP_CSI2_DPHY_ERROR_ITS_BCLR_tu IPP_CSI2_DPHY_ERROR_ITS_BCLR;
    IPP_CSI2_DPHY_ERROR_ITS_BSET_tu IPP_CSI2_DPHY_ERROR_ITS_BSET;
    IPP_CSI2_DPHY_ERROR_ITM_tu IPP_CSI2_DPHY_ERROR_ITM;
    IPP_CSI2_DPHY_ERROR_ITM_BCLR_tu IPP_CSI2_DPHY_ERROR_ITM_BCLR;
    IPP_CSI2_DPHY_ERROR_ITM_BSET_tu IPP_CSI2_DPHY_ERROR_ITM_BSET;
    IPP_CSI2_PACKET_ERROR_ITS_tu IPP_CSI2_PACKET_ERROR_ITS;
    IPP_CSI2_PACKET_ERROR_ITS_BCLR_tu IPP_CSI2_PACKET_ERROR_ITS_BCLR;
    IPP_CSI2_PACKET_ERROR_ITS_BSET_tu IPP_CSI2_PACKET_ERROR_ITS_BSET;
    IPP_CSI2_PACKET_ERROR_ITM_tu IPP_CSI2_PACKET_ERROR_ITM;
    IPP_CSI2_PACKET_ERROR_ITM_BCLR_tu IPP_CSI2_PACKET_ERROR_ITM_BCLR;
    IPP_CSI2_PACKET_ERROR_ITM_BSET_tu IPP_CSI2_PACKET_ERROR_ITM_BSET;
    IPP_STATIC_OPIPE03_SEL_tu IPP_STATIC_OPIPE03_SEL;
    uint32_t pad_IPP_ISP_ASYNC_RESET[2];
    IPP_ISP_ASYNC_RESET_tu IPP_ISP_ASYNC_RESET;
    IPP_SD_RESET_tu IPP_SD_RESET;
    IPP_SD_STATIC_CCP_EN_tu IPP_SD_STATIC_CCP_EN;
    IPP_SD_STATIC_SDG_CSI_MODE_tu IPP_SD_STATIC_SDG_CSI_MODE;
    IPP_SD_PRY0_ENABLE_SET_tu IPP_SD_PRY0_ENABLE_SET;
    IPP_SD_PRY1_ENABLE_SET_tu IPP_SD_PRY1_ENABLE_SET;
    IPP_SD_PRY0_ENABLE_CLR_tu IPP_SD_PRY0_ENABLE_CLR;
    IPP_SD_PRY1_ENABLE_CLR_tu IPP_SD_PRY1_ENABLE_CLR;
    IPP_SD_PRY0_ENABLE_STATUS_tu IPP_SD_PRY0_ENABLE_STATUS;
    IPP_SD_PRY1_ENABLE_STATUS_tu IPP_SD_PRY1_ENABLE_STATUS;
    IPP_SD_BMS_ENABLE_SET_tu IPP_SD_BMS_ENABLE_SET;
    IPP_SD_BMS_ENABLE_CLR_tu IPP_SD_BMS_ENABLE_CLR;
    IPP_SD_BMS_ENABLE_STATUS_tu IPP_SD_BMS_ENABLE_STATUS;
    IPP_SD_PRY0_FORMAT_tu IPP_SD_PRY0_FORMAT;
    IPP_SD_PRY1_FORMAT_tu IPP_SD_PRY1_FORMAT;
    IPP_SD_BMS_FORMAT_tu IPP_SD_BMS_FORMAT;
    IPP_SD_STATIC_MEM_LOAD_EN_tu IPP_SD_STATIC_MEM_LOAD_EN;
    IPP_SD_STATIC_LOAD_FORMAT_tu IPP_SD_STATIC_LOAD_FORMAT;
    IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_tu IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN;
    IPP_SD_STATIC_LOAD_LINE_TIME_MSB_tu IPP_SD_STATIC_LOAD_LINE_TIME_MSB;
    IPP_SD_STATIC_LOAD_WW_tu IPP_SD_STATIC_LOAD_WW;
    IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_tu IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER;
    IPP_SD_STATIC_LOAD_LINETYPE0_tu IPP_SD_STATIC_LOAD_LINETYPE0;
    IPP_SD_STATIC_LOAD_LINETYPE1_tu IPP_SD_STATIC_LOAD_LINETYPE1;
    IPP_SD_STATIC_LOAD_LINETYPE2_tu IPP_SD_STATIC_LOAD_LINETYPE2;
    IPP_SD_STATIC_LOAD_LINETYPE3_tu IPP_SD_STATIC_LOAD_LINETYPE3;
    IPP_SD_STATIC_LOAD_LINETYPE4_tu IPP_SD_STATIC_LOAD_LINETYPE4;
    IPP_SD_STATIC_LOAD_LINETYPE5_tu IPP_SD_STATIC_LOAD_LINETYPE5;
    IPP_SD_STATIC_LOAD_LINETYPE6_tu IPP_SD_STATIC_LOAD_LINETYPE6;
    IPP_SD_STATIC_LOAD_LINETYPE7_tu IPP_SD_STATIC_LOAD_LINETYPE7;
    IPP_SD_ERROR_ITS_tu IPP_SD_ERROR_ITS;
    IPP_SD_ERROR_ITS_BCLR_tu IPP_SD_ERROR_ITS_BCLR;
    IPP_SD_ERROR_ITS_BSET_tu IPP_SD_ERROR_ITS_BSET;
    IPP_SD_ERROR_ITM_tu IPP_SD_ERROR_ITM;
    IPP_SD_ERROR_ITM_BCLR_tu IPP_SD_ERROR_ITM_BCLR;
    IPP_SD_ERROR_ITM_BSET_tu IPP_SD_ERROR_ITM_BSET;
    IPP_SD_OPIPE0_MULT_ERR_DBG_tu IPP_SD_OPIPE0_MULT_ERR_DBG;
    uint32_t pad_IPP_CD_RESET;
    IPP_CD_RESET_tu IPP_CD_RESET;
    IPP_CD_START_tu IPP_CD_START;
    IPP_CD_STATIC_EN_tu IPP_CD_STATIC_EN;
    IPP_CD_STATIC_CDG_CSI_PADD_EN_tu IPP_CD_STATIC_CDG_CSI_PADD_EN;
    IPP_CD_STATIC_CCP_FC_tu IPP_CD_STATIC_CCP_FC;
    IPP_CD_STATIC_CCP_LC_tu IPP_CD_STATIC_CCP_LC;
    IPP_CD_RAW_tu IPP_CD_RAW;
    IPP_CD_PIPE3_FORMAT_tu IPP_CD_PIPE3_FORMAT;
    IPP_CD_CROP_EN_tu IPP_CD_CROP_EN;
    IPP_CD_CROP_H0_tu IPP_CD_CROP_H0;
    IPP_CD_CROP_V0_tu IPP_CD_CROP_V0;
    IPP_CD_CROP_H1_tu IPP_CD_CROP_H1;
    IPP_CD_CROP_V1_tu IPP_CD_CROP_V1;
    IPP_CD_ERROR_ITS_tu IPP_CD_ERROR_ITS;
    IPP_CD_ERROR_ITS_BCLR_tu IPP_CD_ERROR_ITS_BCLR;
    IPP_CD_ERROR_ITS_BSET_tu IPP_CD_ERROR_ITS_BSET;
    IPP_CD_ERROR_ITM_tu IPP_CD_ERROR_ITM;
    IPP_CD_ERROR_ITM_BCLR_tu IPP_CD_ERROR_ITM_BCLR;
    IPP_CD_ERROR_ITM_BSET_tu IPP_CD_ERROR_ITM_BSET;
    IPP_CD_RAW_EOF_ITS_tu IPP_CD_RAW_EOF_ITS;
    IPP_CD_RAW_EOF_ITS_BCLR_tu IPP_CD_RAW_EOF_ITS_BCLR;
    IPP_CD_RAW_EOF_ITS_BSET_tu IPP_CD_RAW_EOF_ITS_BSET;
    IPP_CD_RAW_EOF_ITM_tu IPP_CD_RAW_EOF_ITM;
    IPP_CD_RAW_EOF_ITM_BCLR_tu IPP_CD_RAW_EOF_ITM_BCLR;
    IPP_CD_RAW_EOF_ITM_BSET_tu IPP_CD_RAW_EOF_ITM_BSET;
    IPP_CD_CCP_ERR_DBG_ITS_tu IPP_CD_CCP_ERR_DBG_ITS;
    uint32_t pad_IPP_ISP_INTERNAL_ITS_L[6];
    IPP_ISP_INTERNAL_ITS_L_tu IPP_ISP_INTERNAL_ITS_L;
    IPP_ISP_INTERNAL_ITS_BCLR_L_tu IPP_ISP_INTERNAL_ITS_BCLR_L;
    IPP_ISP_INTERNAL_ITS_BSET_L_tu IPP_ISP_INTERNAL_ITS_BSET_L;
    IPP_ISP_INTERNAL_ITM_L_tu IPP_ISP_INTERNAL_ITM_L;
    IPP_ISP_INTERNAL_ITM_BCLR_L_tu IPP_ISP_INTERNAL_ITM_BCLR_L;
    IPP_ISP_INTERNAL_ITM_BSET_L_tu IPP_ISP_INTERNAL_ITM_BSET_L;
    IPP_ISP_INTERNAL_ITS_H_tu IPP_ISP_INTERNAL_ITS_H;
    IPP_ISP_INTERNAL_ITS_BCLR_H_tu IPP_ISP_INTERNAL_ITS_BCLR_H;
    IPP_ISP_INTERNAL_ITS_BSET_H_tu IPP_ISP_INTERNAL_ITS_BSET_H;
    IPP_ISP_INTERNAL_ITM_H_tu IPP_ISP_INTERNAL_ITM_H;
    IPP_ISP_INTERNAL_ITM_BCLR_H_tu IPP_ISP_INTERNAL_ITM_BCLR_H;
    IPP_ISP_INTERNAL_ITM_BSET_H_tu IPP_ISP_INTERNAL_ITM_BSET_H;
    IPP_SIA_ITEXT_ITS_tu IPP_SIA_ITEXT_ITS;
    IPP_SIA_ITEXT_ITS_BCLR_tu IPP_SIA_ITEXT_ITS_BCLR;
    IPP_SIA_ITEXT_ITS_BSET_tu IPP_SIA_ITEXT_ITS_BSET;
    IPP_SIA_ITEXT_ITM_tu IPP_SIA_ITEXT_ITM;
    IPP_SIA_ITEXT_ITM_BCLR_tu IPP_SIA_ITEXT_ITM_BCLR;
    IPP_SIA_ITEXT_ITM_BSET_tu IPP_SIA_ITEXT_ITM_BSET;
    IPP_EVT_ITS_tu IPP_EVT_ITS;
    IPP_EVT_ITS_BCLR_tu IPP_EVT_ITS_BCLR;
    IPP_EVT_ITS_BSET_tu IPP_EVT_ITS_BSET;
    IPP_EVT_ITM_tu IPP_EVT_ITM;
    IPP_EVT_ITM_BCLR_tu IPP_EVT_ITM_BCLR;
    IPP_EVT_ITM_BSET_tu IPP_EVT_ITM_BSET;
    IPP_STATIC_TOP_IF_3D_SEL_tu IPP_STATIC_TOP_IF_3D_SEL;
    IPP_CSI2RX_3D_VC_DATA_TYPE_R_tu IPP_CSI2RX_3D_VC_DATA_TYPE_R;
    IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_tu IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W;
    IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W_tu IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W;
    IPP_STATIC_CSI2RX_3D_VC_W_tu IPP_STATIC_CSI2RX_3D_VC_W;
    IPP_CSI2RX_3D_FRAME_NB_R_tu IPP_CSI2RX_3D_FRAME_NB_R;
    IPP_CSI2RX_3D_DATA_FIELD_R_tu IPP_CSI2RX_3D_DATA_FIELD_R;
    IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_tu IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W;
    IPP_STATIC_DELAYM_EN_tu IPP_STATIC_DELAYM_EN;
    IPP_DELAYM_LINE_DELAY_tu IPP_DELAYM_LINE_DELAY;
    IPP_DELAYM_PIXEL_DELAY_tu IPP_DELAYM_PIXEL_DELAY;
    IPP_SD_STATIC_SDG_3D_CSI_MODE_tu IPP_SD_STATIC_SDG_3D_CSI_MODE;
    uint32_t pad_IPP_CSI2_DPHY0_DL4_CTRL;
    IPP_CSI2_DPHY0_DL4_CTRL_tu IPP_CSI2_DPHY0_DL4_CTRL;
    IPP_CSI2_DPHY0_DL4_DBG_tu IPP_CSI2_DPHY0_DL4_DBG;
    IPP_CSI2_DPHY0_DBG2_tu IPP_CSI2_DPHY0_DBG2;
    IPP_CSI2_DPHY1_DBG2_tu IPP_CSI2_DPHY1_DBG2;
    IPP_CSI2_DPHY0_DL4_ERROR_ITS_tu IPP_CSI2_DPHY0_DL4_ERROR_ITS;
    IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR_tu IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR;
    IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET_tu IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET;
    IPP_CSI2_DPHY0_DL4_ERROR_ITM_tu IPP_CSI2_DPHY0_DL4_ERROR_ITM;
    IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR_tu IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR;
    IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET_tu IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET;
    uint32_t pad_IPP_CSI2RX_VC_DATA_TYPE_R[2];
    IPP_CSI2RX_VC_DATA_TYPE_R_tu IPP_CSI2RX_VC_DATA_TYPE_R;
    IPP_STATIC_CSI2RX_DATA_TYPE_1_W_tu IPP_STATIC_CSI2RX_DATA_TYPE_1_W;
    IPP_STATIC_CSI2RX_DATA_TYPE_2_W_tu IPP_STATIC_CSI2RX_DATA_TYPE_2_W;
    IPP_STATIC_CSI2RX_VC_W_tu IPP_STATIC_CSI2RX_VC_W;
    IPP_CSI2RX_FRAME_NB_R_tu IPP_CSI2RX_FRAME_NB_R;
    IPP_CSI2RX_DATA_FIELD_R_tu IPP_CSI2RX_DATA_FIELD_R;
    IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_tu IPP_STATIC_CSI2RX_PIX_WIDTH_W_new;
    uint32_t pad_IPP_BML_ITS[4];
    IPP_BML_ITS_tu IPP_BML_ITS;
    IPP_BML_ITS_BCLR_tu IPP_BML_ITS_BCLR;
    IPP_BML_ITS_BSET_tu IPP_BML_ITS_BSET;
    IPP_BML_ITM_tu IPP_BML_ITM;
    IPP_BML_ITM_BCLR_tu IPP_BML_ITM_BCLR;
    IPP_BML_ITM_BSET_tu IPP_BML_ITM_BSET;
    IPP_SD_BMS_ISL_TOP_H_tu IPP_SD_BMS_ISL_TOP_H;
    IPP_SD_BMS_ISL_BOT_H_tu IPP_SD_BMS_ISL_BOT_H;
    IPP_SD_BMS_ISL_FILTER_tu IPP_SD_BMS_ISL_FILTER;
    IPP_SD_BMS_ISL_FMT_tu IPP_SD_BMS_ISL_FMT;
    IPP_SD_DXO_DATAPATH_tu IPP_SD_DXO_DATAPATH;
    IPP_DXO_PDP_ITS_tu IPP_DXO_PDP_ITS;
    IPP_DXO_PDP_ITS_BCLR_tu IPP_DXO_PDP_ITS_BCLR;
    IPP_DXO_PDP_ITS_BSET_tu IPP_DXO_PDP_ITS_BSET;
    IPP_DXO_PDP_ITM_tu IPP_DXO_PDP_ITM;
    IPP_DXO_PDP_ITM_BCLR_tu IPP_DXO_PDP_ITM_BCLR;
    IPP_DXO_PDP_ITM_BSET_tu IPP_DXO_PDP_ITM_BSET;
    IPP_DXO_DPP_ITS_tu IPP_DXO_DPP_ITS;
    IPP_DXO_DPP_ITS_BCLR_tu IPP_DXO_DPP_ITS_BCLR;
    IPP_DXO_DPP_ITS_BSET_tu IPP_DXO_DPP_ITS_BSET;
    IPP_DXO_DPP_ITM_BCLR_tu IPP_DXO_DPP_ITM_BCLR;
    IPP_DXO_DPP_ITM_BSET_tu IPP_DXO_DPP_ITM_BSET;
    IPP_DXO_DOP7_ITS_tu IPP_DXO_DOP7_ITS;
    IPP_DXO_DOP7_ITS_BCLR_tu IPP_DXO_DOP7_ITS_BCLR;
    IPP_DXO_DOP7_ITS_BSET_tu IPP_DXO_DOP7_ITS_BSET;
    IPP_DXO_DOP7_ITM_tu IPP_DXO_DOP7_ITM;
    IPP_DXO_DOP7_ITM_BCLR_tu IPP_DXO_DOP7_ITM_BCLR;
    IPP_DXO_DOP7_ITM_BSET_tu IPP_DXO_DOP7_ITM_BSET;
}IPP_IP_ts;




//IPP_DPHY_TOP_IF_EN


#define Get_IPP_IPP_DPHY_TOP_IF_EN_DPHY_TOP_IF_EN() p_IPP_IP->IPP_DPHY_TOP_IF_EN.IPP_DPHY_TOP_IF_EN_ts.DPHY_TOP_IF_EN
#define Set_IPP_IPP_DPHY_TOP_IF_EN_DPHY_TOP_IF_EN(x) (p_IPP_IP->IPP_DPHY_TOP_IF_EN.IPP_DPHY_TOP_IF_EN_ts.DPHY_TOP_IF_EN = x)
#define Set_IPP_IPP_DPHY_TOP_IF_EN(DPHY_TOP_IF_EN) (p_IPP_IP->IPP_DPHY_TOP_IF_EN.word = (uint16_t)DPHY_TOP_IF_EN<<0)
#define Get_IPP_IPP_DPHY_TOP_IF_EN() p_IPP_IP->IPP_DPHY_TOP_IF_EN.word
#define Set_IPP_IPP_DPHY_TOP_IF_EN_word(x) (p_IPP_IP->IPP_DPHY_TOP_IF_EN.word = x)


//IPP_STATIC_TOP_IF_SEL


#define Get_IPP_IPP_STATIC_TOP_IF_SEL_STATIC_TOP_IF_SEL() p_IPP_IP->IPP_STATIC_TOP_IF_SEL.IPP_STATIC_TOP_IF_SEL_ts.STATIC_TOP_IF_SEL
#define Is_IPP_IPP_STATIC_TOP_IF_SEL_STATIC_TOP_IF_SEL_B_0x0() (p_IPP_IP->IPP_STATIC_TOP_IF_SEL.IPP_STATIC_TOP_IF_SEL_ts.STATIC_TOP_IF_SEL == STATIC_TOP_IF_SEL_B_0x0)
#define Set_IPP_IPP_STATIC_TOP_IF_SEL_STATIC_TOP_IF_SEL__B_0x0() (p_IPP_IP->IPP_STATIC_TOP_IF_SEL.IPP_STATIC_TOP_IF_SEL_ts.STATIC_TOP_IF_SEL = STATIC_TOP_IF_SEL_B_0x0)
#define STATIC_TOP_IF_SEL_B_0x0 0x0    //CSI0 (primary interface)
#define Is_IPP_IPP_STATIC_TOP_IF_SEL_STATIC_TOP_IF_SEL_B_0x1() (p_IPP_IP->IPP_STATIC_TOP_IF_SEL.IPP_STATIC_TOP_IF_SEL_ts.STATIC_TOP_IF_SEL == STATIC_TOP_IF_SEL_B_0x1)
#define Set_IPP_IPP_STATIC_TOP_IF_SEL_STATIC_TOP_IF_SEL__B_0x1() (p_IPP_IP->IPP_STATIC_TOP_IF_SEL.IPP_STATIC_TOP_IF_SEL_ts.STATIC_TOP_IF_SEL = STATIC_TOP_IF_SEL_B_0x1)
#define STATIC_TOP_IF_SEL_B_0x1 0x1    //CSI1 (secondary interface)
#define Set_IPP_IPP_STATIC_TOP_IF_SEL(STATIC_TOP_IF_SEL) (p_IPP_IP->IPP_STATIC_TOP_IF_SEL.word = (uint16_t)STATIC_TOP_IF_SEL<<0)
#define Get_IPP_IPP_STATIC_TOP_IF_SEL() p_IPP_IP->IPP_STATIC_TOP_IF_SEL.word
#define Set_IPP_IPP_STATIC_TOP_IF_SEL_word(x) (p_IPP_IP->IPP_STATIC_TOP_IF_SEL.word = x)


//IPP_STATIC_CCP_IF


#define Get_IPP_IPP_STATIC_CCP_IF_STATIC_CCP_DSCLK() p_IPP_IP->IPP_STATIC_CCP_IF.IPP_STATIC_CCP_IF_ts.STATIC_CCP_DSCLK
#define Is_IPP_IPP_STATIC_CCP_IF_STATIC_CCP_DSCLK_B_0x0() (p_IPP_IP->IPP_STATIC_CCP_IF.IPP_STATIC_CCP_IF_ts.STATIC_CCP_DSCLK == STATIC_CCP_DSCLK_B_0x0)
#define Set_IPP_IPP_STATIC_CCP_IF_STATIC_CCP_DSCLK__B_0x0() (p_IPP_IP->IPP_STATIC_CCP_IF.IPP_STATIC_CCP_IF_ts.STATIC_CCP_DSCLK = STATIC_CCP_DSCLK_B_0x0)
#define STATIC_CCP_DSCLK_B_0x0 0x0    //data/strobe
#define Is_IPP_IPP_STATIC_CCP_IF_STATIC_CCP_DSCLK_B_0x1() (p_IPP_IP->IPP_STATIC_CCP_IF.IPP_STATIC_CCP_IF_ts.STATIC_CCP_DSCLK == STATIC_CCP_DSCLK_B_0x1)
#define Set_IPP_IPP_STATIC_CCP_IF_STATIC_CCP_DSCLK__B_0x1() (p_IPP_IP->IPP_STATIC_CCP_IF.IPP_STATIC_CCP_IF_ts.STATIC_CCP_DSCLK = STATIC_CCP_DSCLK_B_0x1)
#define STATIC_CCP_DSCLK_B_0x1 0x1    //data/clock


#define Get_IPP_IPP_STATIC_CCP_IF_STATIC_CCP_BPP() p_IPP_IP->IPP_STATIC_CCP_IF.IPP_STATIC_CCP_IF_ts.STATIC_CCP_BPP
#define Set_IPP_IPP_STATIC_CCP_IF_STATIC_CCP_BPP(x) (p_IPP_IP->IPP_STATIC_CCP_IF.IPP_STATIC_CCP_IF_ts.STATIC_CCP_BPP = x)
#define Set_IPP_IPP_STATIC_CCP_IF(STATIC_CCP_DSCLK,STATIC_CCP_BPP) (p_IPP_IP->IPP_STATIC_CCP_IF.word = (uint16_t)STATIC_CCP_DSCLK<<0 | (uint16_t)STATIC_CCP_BPP<<8)
#define Get_IPP_IPP_STATIC_CCP_IF() p_IPP_IP->IPP_STATIC_CCP_IF.word
#define Set_IPP_IPP_STATIC_CCP_IF_word(x) (p_IPP_IP->IPP_STATIC_CCP_IF.word = x)


//IPP_INTERNAL_EN_CLK_CCP


#define Get_IPP_IPP_INTERNAL_EN_CLK_CCP_INTERNAL_EN_CLK_CCP() p_IPP_IP->IPP_INTERNAL_EN_CLK_CCP.IPP_INTERNAL_EN_CLK_CCP_ts.INTERNAL_EN_CLK_CCP
#define Is_IPP_IPP_INTERNAL_EN_CLK_CCP_INTERNAL_EN_CLK_CCP_B_0x0() (p_IPP_IP->IPP_INTERNAL_EN_CLK_CCP.IPP_INTERNAL_EN_CLK_CCP_ts.INTERNAL_EN_CLK_CCP == INTERNAL_EN_CLK_CCP_B_0x0)
#define Set_IPP_IPP_INTERNAL_EN_CLK_CCP_INTERNAL_EN_CLK_CCP__B_0x0() (p_IPP_IP->IPP_INTERNAL_EN_CLK_CCP.IPP_INTERNAL_EN_CLK_CCP_ts.INTERNAL_EN_CLK_CCP = INTERNAL_EN_CLK_CCP_B_0x0)
#define INTERNAL_EN_CLK_CCP_B_0x0 0x0    //disabled
#define Is_IPP_IPP_INTERNAL_EN_CLK_CCP_INTERNAL_EN_CLK_CCP_B_0x1() (p_IPP_IP->IPP_INTERNAL_EN_CLK_CCP.IPP_INTERNAL_EN_CLK_CCP_ts.INTERNAL_EN_CLK_CCP == INTERNAL_EN_CLK_CCP_B_0x1)
#define Set_IPP_IPP_INTERNAL_EN_CLK_CCP_INTERNAL_EN_CLK_CCP__B_0x1() (p_IPP_IP->IPP_INTERNAL_EN_CLK_CCP.IPP_INTERNAL_EN_CLK_CCP_ts.INTERNAL_EN_CLK_CCP = INTERNAL_EN_CLK_CCP_B_0x1)
#define INTERNAL_EN_CLK_CCP_B_0x1 0x1    //enabled
#define Set_IPP_IPP_INTERNAL_EN_CLK_CCP(INTERNAL_EN_CLK_CCP) (p_IPP_IP->IPP_INTERNAL_EN_CLK_CCP.word = (uint16_t)INTERNAL_EN_CLK_CCP<<0)
#define Get_IPP_IPP_INTERNAL_EN_CLK_CCP() p_IPP_IP->IPP_INTERNAL_EN_CLK_CCP.word
#define Set_IPP_IPP_INTERNAL_EN_CLK_CCP_word(x) (p_IPP_IP->IPP_INTERNAL_EN_CLK_CCP.word = x)


//IPP_DPHY_COMPENSATION_CTRL


#define Get_IPP_IPP_DPHY_COMPENSATION_CTRL_CLK_1_5M_COMPENSATION_EN() p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.IPP_DPHY_COMPENSATION_CTRL_ts.CLK_1_5M_COMPENSATION_EN
#define Set_IPP_IPP_DPHY_COMPENSATION_CTRL_CLK_1_5M_COMPENSATION_EN(x) (p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.IPP_DPHY_COMPENSATION_CTRL_ts.CLK_1_5M_COMPENSATION_EN = x)


#define Get_IPP_IPP_DPHY_COMPENSATION_CTRL_DPHY_COMPENSATION_START() p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.IPP_DPHY_COMPENSATION_CTRL_ts.DPHY_COMPENSATION_START
#define Set_IPP_IPP_DPHY_COMPENSATION_CTRL_DPHY_COMPENSATION_START(x) (p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.IPP_DPHY_COMPENSATION_CTRL_ts.DPHY_COMPENSATION_START = x)


#define Get_IPP_IPP_DPHY_COMPENSATION_CTRL_DPHY_COMPENSATION_CALIB_WRITE_EN() p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.IPP_DPHY_COMPENSATION_CTRL_ts.DPHY_COMPENSATION_CALIB_WRITE_EN
#define Set_IPP_IPP_DPHY_COMPENSATION_CTRL_DPHY_COMPENSATION_CALIB_WRITE_EN(x) (p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.IPP_DPHY_COMPENSATION_CTRL_ts.DPHY_COMPENSATION_CALIB_WRITE_EN = x)


#define Get_IPP_IPP_DPHY_COMPENSATION_CTRL_DPHY_COMPENSATION_CALIB_WRITE_CODE() p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.IPP_DPHY_COMPENSATION_CTRL_ts.DPHY_COMPENSATION_CALIB_WRITE_CODE
#define Set_IPP_IPP_DPHY_COMPENSATION_CTRL_DPHY_COMPENSATION_CALIB_WRITE_CODE(x) (p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.IPP_DPHY_COMPENSATION_CTRL_ts.DPHY_COMPENSATION_CALIB_WRITE_CODE = x)
#define Set_IPP_IPP_DPHY_COMPENSATION_CTRL(CLK_1_5M_COMPENSATION_EN,DPHY_COMPENSATION_START,DPHY_COMPENSATION_CALIB_WRITE_EN,DPHY_COMPENSATION_CALIB_WRITE_CODE) (p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.word = (uint16_t)CLK_1_5M_COMPENSATION_EN<<0 | (uint16_t)DPHY_COMPENSATION_START<<1 | (uint16_t)DPHY_COMPENSATION_CALIB_WRITE_EN<<2 | (uint16_t)DPHY_COMPENSATION_CALIB_WRITE_CODE<<3)
#define Get_IPP_IPP_DPHY_COMPENSATION_CTRL() p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.word
#define Set_IPP_IPP_DPHY_COMPENSATION_CTRL_word(x) (p_IPP_IP->IPP_DPHY_COMPENSATION_CTRL.word = x)


//IPP_DPHY_COMPENSATION_STATUS


#define Get_IPP_IPP_DPHY_COMPENSATION_STATUS_DPHY_COMPENSATION_OK() p_IPP_IP->IPP_DPHY_COMPENSATION_STATUS.IPP_DPHY_COMPENSATION_STATUS_ts.DPHY_COMPENSATION_OK


#define Get_IPP_IPP_DPHY_COMPENSATION_STATUS_DPHY_COMPENSATION_CALIB_READ_CODE() p_IPP_IP->IPP_DPHY_COMPENSATION_STATUS.IPP_DPHY_COMPENSATION_STATUS_ts.DPHY_COMPENSATION_CALIB_READ_CODE


#define Get_IPP_IPP_DPHY_COMPENSATION_STATUS_DPHY_COMPENSATION_DBG_COMP_FLAG() p_IPP_IP->IPP_DPHY_COMPENSATION_STATUS.IPP_DPHY_COMPENSATION_STATUS_ts.DPHY_COMPENSATION_DBG_COMP_FLAG
#define Get_IPP_IPP_DPHY_COMPENSATION_STATUS() p_IPP_IP->IPP_DPHY_COMPENSATION_STATUS.word


//IPP_STATIC_CSI0_DATA_LANES


#define Get_IPP_IPP_STATIC_CSI0_DATA_LANES_STATIC_CSI0_DATA_LANES_NB() p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.IPP_STATIC_CSI0_DATA_LANES_ts.STATIC_CSI0_DATA_LANES_NB
#define Is_IPP_IPP_STATIC_CSI0_DATA_LANES_STATIC_CSI0_DATA_LANES_NB_B_0x1() (p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.IPP_STATIC_CSI0_DATA_LANES_ts.STATIC_CSI0_DATA_LANES_NB == STATIC_CSI0_DATA_LANES_NB_B_0x1)
#define Set_IPP_IPP_STATIC_CSI0_DATA_LANES_STATIC_CSI0_DATA_LANES_NB__B_0x1() (p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.IPP_STATIC_CSI0_DATA_LANES_ts.STATIC_CSI0_DATA_LANES_NB = STATIC_CSI0_DATA_LANES_NB_B_0x1)
#define STATIC_CSI0_DATA_LANES_NB_B_0x1 0x1    //single data lane
#define Is_IPP_IPP_STATIC_CSI0_DATA_LANES_STATIC_CSI0_DATA_LANES_NB_B_0x2() (p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.IPP_STATIC_CSI0_DATA_LANES_ts.STATIC_CSI0_DATA_LANES_NB == STATIC_CSI0_DATA_LANES_NB_B_0x2)
#define Set_IPP_IPP_STATIC_CSI0_DATA_LANES_STATIC_CSI0_DATA_LANES_NB__B_0x2() (p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.IPP_STATIC_CSI0_DATA_LANES_ts.STATIC_CSI0_DATA_LANES_NB = STATIC_CSI0_DATA_LANES_NB_B_0x2)
#define STATIC_CSI0_DATA_LANES_NB_B_0x2 0x2    //double data lane
#define Is_IPP_IPP_STATIC_CSI0_DATA_LANES_STATIC_CSI0_DATA_LANES_NB_B_0x3() (p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.IPP_STATIC_CSI0_DATA_LANES_ts.STATIC_CSI0_DATA_LANES_NB == STATIC_CSI0_DATA_LANES_NB_B_0x3)
#define Set_IPP_IPP_STATIC_CSI0_DATA_LANES_STATIC_CSI0_DATA_LANES_NB__B_0x3() (p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.IPP_STATIC_CSI0_DATA_LANES_ts.STATIC_CSI0_DATA_LANES_NB = STATIC_CSI0_DATA_LANES_NB_B_0x3)
#define STATIC_CSI0_DATA_LANES_NB_B_0x3 0x3    //reserved (for Early Drop)


#define Get_IPP_IPP_STATIC_CSI0_DATA_LANES_STATIC_CSI0_DATA_LANES_MAP() p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.IPP_STATIC_CSI0_DATA_LANES_ts.STATIC_CSI0_DATA_LANES_MAP
#define Set_IPP_IPP_STATIC_CSI0_DATA_LANES_STATIC_CSI0_DATA_LANES_MAP(x) (p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.IPP_STATIC_CSI0_DATA_LANES_ts.STATIC_CSI0_DATA_LANES_MAP = x)
#define Set_IPP_IPP_STATIC_CSI0_DATA_LANES(STATIC_CSI0_DATA_LANES_NB,STATIC_CSI0_DATA_LANES_MAP) (p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.word = (uint16_t)STATIC_CSI0_DATA_LANES_NB<<0 | (uint16_t)STATIC_CSI0_DATA_LANES_MAP<<4)
#define Get_IPP_IPP_STATIC_CSI0_DATA_LANES() p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.word
#define Set_IPP_IPP_STATIC_CSI0_DATA_LANES_word(x) (p_IPP_IP->IPP_STATIC_CSI0_DATA_LANES.word = x)


//IPP_CSI2_DPHY0_CL_CTRL


#define Get_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_UI_X4() p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_UI_X4
#define Set_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_UI_X4(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_UI_X4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_SPECS_90_81B() p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_SPECS_90_81B
#define Set_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_SPECS_90_81B(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_SPECS_90_81B = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_SWAP_PINS_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_SWAP_PINS_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_SWAP_PINS_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_SWAP_PINS_CL = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_HS_INVERT_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_HS_INVERT_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_HS_INVERT_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_HS_INVERT_CL = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_HSRX_TERM_SHIFT_UP_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_HSRX_TERM_SHIFT_UP_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_CL = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_CL = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_TEST_RESERVED_1_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_TEST_RESERVED_1_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_CTRL_CSI0_TEST_RESERVED_1_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.IPP_CSI2_DPHY0_CL_CTRL_ts.CSI0_TEST_RESERVED_1_CL = x)
#define Set_IPP_IPP_CSI2_DPHY0_CL_CTRL(CSI0_UI_X4,CSI0_SPECS_90_81B,CSI0_SWAP_PINS_CL,CSI0_HS_INVERT_CL,CSI0_HSRX_TERM_SHIFT_UP_CL,CSI0_HSRX_TERM_SHIFT_DOWN_CL,CSI0_TEST_RESERVED_1_CL) (p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.word = (uint16_t)CSI0_UI_X4<<0 | (uint16_t)CSI0_SPECS_90_81B<<8 | (uint16_t)CSI0_SWAP_PINS_CL<<9 | (uint16_t)CSI0_HS_INVERT_CL<<10 | (uint16_t)CSI0_HSRX_TERM_SHIFT_UP_CL<<11 | (uint16_t)CSI0_HSRX_TERM_SHIFT_DOWN_CL<<12 | (uint16_t)CSI0_TEST_RESERVED_1_CL<<13)
#define Get_IPP_IPP_CSI2_DPHY0_CL_CTRL() p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.word
#define Set_IPP_IPP_CSI2_DPHY0_CL_CTRL_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_CTRL.word = x)


//IPP_CSI2_DPHY0_DL1_CTRL


#define Get_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_SWAP_PINS_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_SWAP_PINS_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_SWAP_PINS_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_SWAP_PINS_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_HS_INVERT_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_HS_INVERT_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_HS_INVERT_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_HS_INVERT_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_FORCE_RX_MODE_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_FORCE_RX_MODE_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_FORCE_RX_MODE_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_FORCE_RX_MODE_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_CD_OFF_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_CD_OFF_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_CD_OFF_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_CD_OFF_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_EOT_BYPASS_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_EOT_BYPASS_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_EOT_BYPASS_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_EOT_BYPASS_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_HSRX_TERM_SHIFT_UP_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_HSRX_TERM_SHIFT_UP_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_TEST_RESERVED_1_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_TEST_RESERVED_1_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL_CSI0_TEST_RESERVED_1_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.IPP_CSI2_DPHY0_DL1_CTRL_ts.CSI0_TEST_RESERVED_1_DL1 = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL(CSI0_SWAP_PINS_DL1,CSI0_HS_INVERT_DL1,CSI0_FORCE_RX_MODE_DL1,CSI0_CD_OFF_DL1,CSI0_EOT_BYPASS_DL1,CSI0_HSRX_TERM_SHIFT_UP_DL1,CSI0_HSRX_TERM_SHIFT_DOWN_DL1,CSI0_TEST_RESERVED_1_DL1) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.word = (uint16_t)CSI0_SWAP_PINS_DL1<<0 | (uint16_t)CSI0_HS_INVERT_DL1<<1 | (uint16_t)CSI0_FORCE_RX_MODE_DL1<<2 | (uint16_t)CSI0_CD_OFF_DL1<<3 | (uint16_t)CSI0_EOT_BYPASS_DL1<<4 | (uint16_t)CSI0_HSRX_TERM_SHIFT_UP_DL1<<5 | (uint16_t)CSI0_HSRX_TERM_SHIFT_DOWN_DL1<<6 | (uint16_t)CSI0_TEST_RESERVED_1_DL1<<7)
#define Get_IPP_IPP_CSI2_DPHY0_DL1_CTRL() p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.word
#define Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_CTRL.word = x)


//IPP_CSI2_DPHY0_DL2_CTRL


#define Get_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_SWAP_PINS_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_SWAP_PINS_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_SWAP_PINS_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_SWAP_PINS_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_HS_INVERT_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_HS_INVERT_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_HS_INVERT_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_HS_INVERT_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_FORCE_RX_MODE_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_FORCE_RX_MODE_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_FORCE_RX_MODE_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_FORCE_RX_MODE_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_CD_OFF_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_CD_OFF_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_CD_OFF_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_CD_OFF_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_EOT_BYPASS_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_EOT_BYPASS_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_EOT_BYPASS_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_EOT_BYPASS_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_HSRX_TERM_SHIFT_UP_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_HSRX_TERM_SHIFT_UP_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_TEST_RESERVED_1_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_TEST_RESERVED_1_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL_CSI0_TEST_RESERVED_1_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.IPP_CSI2_DPHY0_DL2_CTRL_ts.CSI0_TEST_RESERVED_1_DL2 = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL(CSI0_SWAP_PINS_DL2,CSI0_HS_INVERT_DL2,CSI0_FORCE_RX_MODE_DL2,CSI0_CD_OFF_DL2,CSI0_EOT_BYPASS_DL2,CSI0_HSRX_TERM_SHIFT_UP_DL2,CSI0_HSRX_TERM_SHIFT_DOWN_DL2,CSI0_TEST_RESERVED_1_DL2) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.word = (uint16_t)CSI0_SWAP_PINS_DL2<<0 | (uint16_t)CSI0_HS_INVERT_DL2<<1 | (uint16_t)CSI0_FORCE_RX_MODE_DL2<<2 | (uint16_t)CSI0_CD_OFF_DL2<<3 | (uint16_t)CSI0_EOT_BYPASS_DL2<<4 | (uint16_t)CSI0_HSRX_TERM_SHIFT_UP_DL2<<5 | (uint16_t)CSI0_HSRX_TERM_SHIFT_DOWN_DL2<<6 | (uint16_t)CSI0_TEST_RESERVED_1_DL2<<7)
#define Get_IPP_IPP_CSI2_DPHY0_DL2_CTRL() p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.word
#define Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_CTRL.word = x)


//IPP_CSI2_DPHY0_DL3_CTRL


#define Get_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_SWAP_PINS_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_SWAP_PINS_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_SWAP_PINS_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_SWAP_PINS_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_HS_INVERT_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_HS_INVERT_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_HS_INVERT_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_HS_INVERT_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_FORCE_RX_MODE_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_FORCE_RX_MODE_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_FORCE_RX_MODE_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_FORCE_RX_MODE_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_CD_OFF_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_CD_OFF_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_CD_OFF_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_CD_OFF_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_EOT_BYPASS_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_EOT_BYPASS_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_EOT_BYPASS_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_EOT_BYPASS_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_HSRX_TERM_SHIFT_UP_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_HSRX_TERM_SHIFT_UP_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_TEST_RESERVED_1_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_TEST_RESERVED_1_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL_CSI0_TEST_RESERVED_1_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.IPP_CSI2_DPHY0_DL3_CTRL_ts.CSI0_TEST_RESERVED_1_DL3 = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL(CSI0_SWAP_PINS_DL3,CSI0_HS_INVERT_DL3,CSI0_FORCE_RX_MODE_DL3,CSI0_CD_OFF_DL3,CSI0_EOT_BYPASS_DL3,CSI0_HSRX_TERM_SHIFT_UP_DL3,CSI0_HSRX_TERM_SHIFT_DOWN_DL3,CSI0_TEST_RESERVED_1_DL3) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.word = (uint16_t)CSI0_SWAP_PINS_DL3<<0 | (uint16_t)CSI0_HS_INVERT_DL3<<1 | (uint16_t)CSI0_FORCE_RX_MODE_DL3<<2 | (uint16_t)CSI0_CD_OFF_DL3<<3 | (uint16_t)CSI0_EOT_BYPASS_DL3<<4 | (uint16_t)CSI0_HSRX_TERM_SHIFT_UP_DL3<<5 | (uint16_t)CSI0_HSRX_TERM_SHIFT_DOWN_DL3<<6 | (uint16_t)CSI0_TEST_RESERVED_1_DL3<<7)
#define Get_IPP_IPP_CSI2_DPHY0_DL3_CTRL() p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.word
#define Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_CTRL.word = x)


//IPP_CSI2_DPHY0_CL_DBG


#define Get_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_HS_RX_OFFSET_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_HS_RX_OFFSET_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_HS_RX_OFFSET_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_HS_RX_OFFSET_CL = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_MIPI_IN_SHORT_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_MIPI_IN_SHORT_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_MIPI_IN_SHORT_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_MIPI_IN_SHORT_CL = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_LP_HS_BYPASS_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_LP_HS_BYPASS_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_LP_HS_BYPASS_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_LP_HS_BYPASS_CL = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_LP_RX_VIL_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_LP_RX_VIL_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_LP_RX_VIL_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_LP_RX_VIL_CL = x)


#define Get_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_DIRECT_DYN_ACCESS_CL() p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_DIRECT_DYN_ACCESS_CL
#define Set_IPP_IPP_CSI2_DPHY0_CL_DBG_CSI0_DIRECT_DYN_ACCESS_CL(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.IPP_CSI2_DPHY0_CL_DBG_ts.CSI0_DIRECT_DYN_ACCESS_CL = x)
#define Set_IPP_IPP_CSI2_DPHY0_CL_DBG(CSI0_HS_RX_OFFSET_CL,CSI0_MIPI_IN_SHORT_CL,CSI0_LP_HS_BYPASS_CL,CSI0_LP_RX_VIL_CL,CSI0_DIRECT_DYN_ACCESS_CL) (p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.word = (uint16_t)CSI0_HS_RX_OFFSET_CL<<0 | (uint16_t)CSI0_MIPI_IN_SHORT_CL<<3 | (uint16_t)CSI0_LP_HS_BYPASS_CL<<8 | (uint16_t)CSI0_LP_RX_VIL_CL<<9 | (uint16_t)CSI0_DIRECT_DYN_ACCESS_CL<<11)
#define Get_IPP_IPP_CSI2_DPHY0_CL_DBG() p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.word
#define Set_IPP_IPP_CSI2_DPHY0_CL_DBG_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_CL_DBG.word = x)


//IPP_CSI2_DPHY0_DL1_DBG


#define Get_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_HS_RX_OFFSET_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_HS_RX_OFFSET_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_HS_RX_OFFSET_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_HS_RX_OFFSET_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_MIPI_IN_SHORT_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_MIPI_IN_SHORT_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_MIPI_IN_SHORT_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_MIPI_IN_SHORT_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_SKEW_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_SKEW_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_SKEW_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_SKEW_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_OVERSAMPLE_BYPASS_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_OVERSAMPLE_BYPASS_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_OVERSAMPLE_BYPASS_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_OVERSAMPLE_BYPASS_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_LP_HS_BYPASS_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_LP_HS_BYPASS_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_LP_HS_BYPASS_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_LP_HS_BYPASS_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_LP_RX_VIL_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_LP_RX_VIL_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_LP_RX_VIL_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_LP_RX_VIL_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_DIRECT_DYN_ACCES_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_DIRECT_DYN_ACCES_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DL1_DBG_CSI0_DIRECT_DYN_ACCES_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.IPP_CSI2_DPHY0_DL1_DBG_ts.CSI0_DIRECT_DYN_ACCES_DL1 = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL1_DBG(CSI0_HS_RX_OFFSET_DL1,CSI0_MIPI_IN_SHORT_DL1,CSI0_SKEW_DL1,CSI0_OVERSAMPLE_BYPASS_DL1,CSI0_LP_HS_BYPASS_DL1,CSI0_LP_RX_VIL_DL1,CSI0_DIRECT_DYN_ACCES_DL1) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.word = (uint16_t)CSI0_HS_RX_OFFSET_DL1<<0 | (uint16_t)CSI0_MIPI_IN_SHORT_DL1<<3 | (uint16_t)CSI0_SKEW_DL1<<4 | (uint16_t)CSI0_OVERSAMPLE_BYPASS_DL1<<7 | (uint16_t)CSI0_LP_HS_BYPASS_DL1<<8 | (uint16_t)CSI0_LP_RX_VIL_DL1<<9 | (uint16_t)CSI0_DIRECT_DYN_ACCES_DL1<<11)
#define Get_IPP_IPP_CSI2_DPHY0_DL1_DBG() p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.word
#define Set_IPP_IPP_CSI2_DPHY0_DL1_DBG_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL1_DBG.word = x)


//IPP_CSI2_DPHY0_DL2_DBG


#define Get_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_HS_RX_OFFSET_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_HS_RX_OFFSET_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_HS_RX_OFFSET_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_HS_RX_OFFSET_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_MIPI_IN_SHORT_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_MIPI_IN_SHORT_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_MIPI_IN_SHORT_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_MIPI_IN_SHORT_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_SKEW_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_SKEW_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_SKEW_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_SKEW_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_OVERSAMPLE_BYPASS_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_OVERSAMPLE_BYPASS_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_OVERSAMPLE_BYPASS_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_OVERSAMPLE_BYPASS_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_LP_HS_BYPASS_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_LP_HS_BYPASS_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_LP_HS_BYPASS_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_LP_HS_BYPASS_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_LP_RX_VIL_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_LP_RX_VIL_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_LP_RX_VIL_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_LP_RX_VIL_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_DIRECT_DYN_ACCESS_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_DIRECT_DYN_ACCESS_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DL2_DBG_CSI0_DIRECT_DYN_ACCESS_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.IPP_CSI2_DPHY0_DL2_DBG_ts.CSI0_DIRECT_DYN_ACCESS_DL2 = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL2_DBG(CSI0_HS_RX_OFFSET_DL2,CSI0_MIPI_IN_SHORT_DL2,CSI0_SKEW_DL2,CSI0_OVERSAMPLE_BYPASS_DL2,CSI0_LP_HS_BYPASS_DL2,CSI0_LP_RX_VIL_DL2,CSI0_DIRECT_DYN_ACCESS_DL2) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.word = (uint16_t)CSI0_HS_RX_OFFSET_DL2<<0 | (uint16_t)CSI0_MIPI_IN_SHORT_DL2<<3 | (uint16_t)CSI0_SKEW_DL2<<4 | (uint16_t)CSI0_OVERSAMPLE_BYPASS_DL2<<7 | (uint16_t)CSI0_LP_HS_BYPASS_DL2<<8 | (uint16_t)CSI0_LP_RX_VIL_DL2<<9 | (uint16_t)CSI0_DIRECT_DYN_ACCESS_DL2<<11)
#define Get_IPP_IPP_CSI2_DPHY0_DL2_DBG() p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.word
#define Set_IPP_IPP_CSI2_DPHY0_DL2_DBG_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL2_DBG.word = x)


//IPP_CSI2_DPHY0_DL3_DBG


#define Get_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_HS_RX_OFFSET_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_HS_RX_OFFSET_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_HS_RX_OFFSET_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_HS_RX_OFFSET_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_MIPI_IN_SHORT_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_MIPI_IN_SHORT_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_MIPI_IN_SHORT_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_MIPI_IN_SHORT_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_SKEW_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_SKEW_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_SKEW_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_SKEW_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_OVERSAMPLE_BYPASS_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_OVERSAMPLE_BYPASS_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_OVERSAMPLE_BYPASS_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_OVERSAMPLE_BYPASS_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_LP_HS_BYPASS_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_LP_HS_BYPASS_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_LP_HS_BYPASS_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_LP_HS_BYPASS_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_LP_RX_VIL_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_LP_RX_VIL_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_LP_RX_VIL_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_LP_RX_VIL_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_DIRECT_DYN_ACCESS_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_DIRECT_DYN_ACCESS_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DL3_DBG_CSI0_DIRECT_DYN_ACCESS_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.IPP_CSI2_DPHY0_DL3_DBG_ts.CSI0_DIRECT_DYN_ACCESS_DL3 = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL3_DBG(CSI0_HS_RX_OFFSET_DL3,CSI0_MIPI_IN_SHORT_DL3,CSI0_SKEW_DL3,CSI0_OVERSAMPLE_BYPASS_DL3,CSI0_LP_HS_BYPASS_DL3,CSI0_LP_RX_VIL_DL3,CSI0_DIRECT_DYN_ACCESS_DL3) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.word = (uint16_t)CSI0_HS_RX_OFFSET_DL3<<0 | (uint16_t)CSI0_MIPI_IN_SHORT_DL3<<3 | (uint16_t)CSI0_SKEW_DL3<<4 | (uint16_t)CSI0_OVERSAMPLE_BYPASS_DL3<<7 | (uint16_t)CSI0_LP_HS_BYPASS_DL3<<8 | (uint16_t)CSI0_LP_RX_VIL_DL3<<9 | (uint16_t)CSI0_DIRECT_DYN_ACCESS_DL3<<11)
#define Get_IPP_IPP_CSI2_DPHY0_DL3_DBG() p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.word
#define Set_IPP_IPP_CSI2_DPHY0_DL3_DBG_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL3_DBG.word = x)


//IPP_CSI2_DPHY0_XL_DBG0_OBS


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_OVERSAMPLE_FLAG1_DL1() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_OVERSAMPLE_FLAG1_DL1


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_OVERSAMPLE_FLAG1_DL2() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_OVERSAMPLE_FLAG1_DL2


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_OVERSAMPLE_FLAG1_DL3() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_OVERSAMPLE_FLAG1_DL3


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_OVERSAMPLE_FLAG2_DL1() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_OVERSAMPLE_FLAG2_DL1


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_OVERSAMPLE_FLAG2_DL2() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_OVERSAMPLE_FLAG2_DL2


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_OVERSAMPLE_FLAG2_DL3() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_OVERSAMPLE_FLAG2_DL3


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_HS_RX_ZM_CL() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_HS_RX_ZM_CL


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_HS_RX_ZM_DL1() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_HS_RX_ZM_DL1


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_HS_RX_ZM_DL2() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_HS_RX_ZM_DL2

#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_OVERSAMPLE_FLAG1_DL4() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_OVERSAMPLE_FLAG1_DL4


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_OVERSAMPLE_FLAG2_DL4() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_OVERSAMPLE_FLAG2_DL4


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_HS_RX_ZM_DL4() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_HS_RX_ZM_DL4

#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_HS_RX_ZM_DL3() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_HS_RX_ZM_DL3


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS_CSI0_RX_INIT_CLK() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.IPP_CSI2_DPHY0_XL_DBG0_OBS_ts.CSI0_RX_INIT_CLK
#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG0_OBS() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG0_OBS.word


//IPP_CSI2_DPHY0_XL_DBG1_OBS


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_STOP_STATE_CL() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_STOP_STATE_CL


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_STOP_STATE_DL1() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_STOP_STATE_DL1


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_STOP_STATE_DL2() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_STOP_STATE_DL2


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_STOP_STATE_DL3() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_STOP_STATE_DL3


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_ULP_ACTIVE_NOT_CL() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_ULP_ACTIVE_NOT_CL


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_ULP_ACTIVE_NOT_DL1() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_ULP_ACTIVE_NOT_DL1


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_ULP_ACTIVE_NOT_DL2() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_ULP_ACTIVE_NOT_DL2


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_ULP_ACTIVE_NOT_DL3() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_ULP_ACTIVE_NOT_DL3

#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_ULP_ACTIVE_NOT_DL4() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_ULP_ACTIVE_NOT_DL4


#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS_CSI0_CLK_ACTIVE_HS() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.IPP_CSI2_DPHY0_XL_DBG1_OBS_ts.CSI0_CLK_ACTIVE_HS

#define Get_IPP_IPP_CSI2_DPHY0_XL_DBG1_OBS() p_IPP_IP->IPP_CSI2_DPHY0_XL_DBG1_OBS.word


//IPP_CSI2_DPHY1_CL_CTRL


#define Get_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_UI_X4() p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_UI_X4
#define Set_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_UI_X4(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_UI_X4 = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_SPECS_90_81B() p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_SPECS_90_81B
#define Set_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_SPECS_90_81B(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_SPECS_90_81B = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_SWAP_PINS_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_SWAP_PINS_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_SWAP_PINS_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_SWAP_PINS_CL = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_HS_INVERT_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_HS_INVERT_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_HS_INVERT_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_HS_INVERT_CL = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_HSRX_TERM_SHIFT_UP_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_HSRX_TERM_SHIFT_UP_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_HSRX_TERM_SHIFT_UP_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_HSRX_TERM_SHIFT_UP_CL = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_HSRX_TERM_SHIFT_DOWN_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_HSRX_TERM_SHIFT_DOWN_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_HSRX_TERM_SHIFT_DOWN_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_HSRX_TERM_SHIFT_DOWN_CL = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_TEST_RESERVED_1_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_TEST_RESERVED_1_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_CTRL_CSI1_TEST_RESERVED_1_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.IPP_CSI2_DPHY1_CL_CTRL_ts.CSI1_TEST_RESERVED_1_CL = x)
#define Set_IPP_IPP_CSI2_DPHY1_CL_CTRL(CSI1_UI_X4,CSI1_SPECS_90_81B,CSI1_SWAP_PINS_CL,CSI1_HS_INVERT_CL,CSI1_HSRX_TERM_SHIFT_UP_CL,CSI1_HSRX_TERM_SHIFT_DOWN_CL,CSI1_TEST_RESERVED_1_CL) (p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.word = (uint16_t)CSI1_UI_X4<<0 | (uint16_t)CSI1_SPECS_90_81B<<8 | (uint16_t)CSI1_SWAP_PINS_CL<<9 | (uint16_t)CSI1_HS_INVERT_CL<<10 | (uint16_t)CSI1_HSRX_TERM_SHIFT_UP_CL<<11 | (uint16_t)CSI1_HSRX_TERM_SHIFT_DOWN_CL<<12 | (uint16_t)CSI1_TEST_RESERVED_1_CL<<13)
#define Get_IPP_IPP_CSI2_DPHY1_CL_CTRL() p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.word
#define Set_IPP_IPP_CSI2_DPHY1_CL_CTRL_word(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_CTRL.word = x)


//IPP_CSI2_DPHY1_DL1_CTRL


#define Get_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_SWAP_PINS_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_SWAP_PINS_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_SWAP_PINS_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_SWAP_PINS_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_HS_INVERT_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_HS_INVERT_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_HS_INVERT_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_HS_INVERT_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_FORCE_RX_MODE_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_FORCE_RX_MODE_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_FORCE_RX_MODE_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_FORCE_RX_MODE_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_CD_OFF_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_CD_OFF_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_CD_OFF_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_CD_OFF_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_EOT_BYPASS_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_EOT_BYPASS_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_EOT_BYPASS_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_EOT_BYPASS_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_HSRX_TERM_SHIFT_UP_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_HSRX_TERM_SHIFT_UP_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_HSRX_TERM_SHIFT_UP_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_HSRX_TERM_SHIFT_UP_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_HSRX_TERM_SHIFT_DOWN_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_HSRX_TERM_SHIFT_DOWN_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_HSRX_TERM_SHIFT_DOWN_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_HSRX_TERM_SHIFT_DOWN_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_TEST_RESERVED_1_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_TEST_RESERVED_1_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL_CSI1_TEST_RESERVED_1_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.IPP_CSI2_DPHY1_DL1_CTRL_ts.CSI1_TEST_RESERVED_1_DL1 = x)
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL(CSI1_SWAP_PINS_DL1,CSI1_HS_INVERT_DL1,CSI1_FORCE_RX_MODE_DL1,CSI1_CD_OFF_DL1,CSI1_EOT_BYPASS_DL1,CSI1_HSRX_TERM_SHIFT_UP_DL1,CSI1_HSRX_TERM_SHIFT_DOWN_DL1,CSI1_TEST_RESERVED_1_DL1) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.word = (uint16_t)CSI1_SWAP_PINS_DL1<<0 | (uint16_t)CSI1_HS_INVERT_DL1<<1 | (uint16_t)CSI1_FORCE_RX_MODE_DL1<<2 | (uint16_t)CSI1_CD_OFF_DL1<<3 | (uint16_t)CSI1_EOT_BYPASS_DL1<<4 | (uint16_t)CSI1_HSRX_TERM_SHIFT_UP_DL1<<5 | (uint16_t)CSI1_HSRX_TERM_SHIFT_DOWN_DL1<<6 | (uint16_t)CSI1_TEST_RESERVED_1_DL1<<7)
#define Get_IPP_IPP_CSI2_DPHY1_DL1_CTRL() p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.word
#define Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL_word(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_CTRL.word = x)


//IPP_CSI2_DPHY1_CL_DBG


#define Get_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_HS_RX_OFFSET_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_HS_RX_OFFSET_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_HS_RX_OFFSET_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_HS_RX_OFFSET_CL = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_MIPI_IN_SHORT_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_MIPI_IN_SHORT_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_MIPI_IN_SHORT_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_MIPI_IN_SHORT_CL = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_LP_HS_BYPASS_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_LP_HS_BYPASS_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_LP_HS_BYPASS_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_LP_HS_BYPASS_CL = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_LP_RX_VIL_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_LP_RX_VIL_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_LP_RX_VIL_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_LP_RX_VIL_CL = x)


#define Get_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_DIRECT_DYN_ACCESS_CL() p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_DIRECT_DYN_ACCESS_CL
#define Set_IPP_IPP_CSI2_DPHY1_CL_DBG_CSI1_DIRECT_DYN_ACCESS_CL(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.IPP_CSI2_DPHY1_CL_DBG_ts.CSI1_DIRECT_DYN_ACCESS_CL = x)
#define Set_IPP_IPP_CSI2_DPHY1_CL_DBG(CSI1_HS_RX_OFFSET_CL,CSI1_MIPI_IN_SHORT_CL,CSI1_LP_HS_BYPASS_CL,CSI1_LP_RX_VIL_CL,CSI1_DIRECT_DYN_ACCESS_CL) (p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.word = (uint16_t)CSI1_HS_RX_OFFSET_CL<<0 | (uint16_t)CSI1_MIPI_IN_SHORT_CL<<3 | (uint16_t)CSI1_LP_HS_BYPASS_CL<<8 | (uint16_t)CSI1_LP_RX_VIL_CL<<9 | (uint16_t)CSI1_DIRECT_DYN_ACCESS_CL<<11)
#define Get_IPP_IPP_CSI2_DPHY1_CL_DBG() p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.word
#define Set_IPP_IPP_CSI2_DPHY1_CL_DBG_word(x) (p_IPP_IP->IPP_CSI2_DPHY1_CL_DBG.word = x)


//IPP_CSI2_DPHY1_DL1_DBG


#define Get_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_HS_RX_OFFSET_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_HS_RX_OFFSET_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_HS_RX_OFFSET_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_HS_RX_OFFSET_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_MIPI_IN_SHORT_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_MIPI_IN_SHORT_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_MIPI_IN_SHORT_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_MIPI_IN_SHORT_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_SKEW_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_SKEW_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_SKEW_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_SKEW_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_OVERSAMPLE_BYPASS_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_OVERSAMPLE_BYPASS_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_OVERSAMPLE_BYPASS_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_OVERSAMPLE_BYPASS_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_LP_HS_BYPASS_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_LP_HS_BYPASS_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_LP_HS_BYPASS_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_LP_HS_BYPASS_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_LP_RX_VIL_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_LP_RX_VIL_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_LP_RX_VIL_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_LP_RX_VIL_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_DIRECT_DYN_ACCESS_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_DIRECT_DYN_ACCESS_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DL1_DBG_CSI1_DIRECT_DYN_ACCESS_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.IPP_CSI2_DPHY1_DL1_DBG_ts.CSI1_DIRECT_DYN_ACCESS_DL1 = x)
#define Set_IPP_IPP_CSI2_DPHY1_DL1_DBG(CSI1_HS_RX_OFFSET_DL1,CSI1_MIPI_IN_SHORT_DL1,CSI1_SKEW_DL1,CSI1_OVERSAMPLE_BYPASS_DL1,CSI1_LP_HS_BYPASS_DL1,CSI1_LP_RX_VIL_DL1,CSI1_DIRECT_DYN_ACCESS_DL1) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.word = (uint16_t)CSI1_HS_RX_OFFSET_DL1<<0 | (uint16_t)CSI1_MIPI_IN_SHORT_DL1<<3 | (uint16_t)CSI1_SKEW_DL1<<4 | (uint16_t)CSI1_OVERSAMPLE_BYPASS_DL1<<7 | (uint16_t)CSI1_LP_HS_BYPASS_DL1<<8 | (uint16_t)CSI1_LP_RX_VIL_DL1<<9 | (uint16_t)CSI1_DIRECT_DYN_ACCESS_DL1<<11)
#define Get_IPP_IPP_CSI2_DPHY1_DL1_DBG() p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.word
#define Set_IPP_IPP_CSI2_DPHY1_DL1_DBG_word(x) (p_IPP_IP->IPP_CSI2_DPHY1_DL1_DBG.word = x)


//IPP_CSI2_DPHY1_XL_DBG_OBS


#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS_CSI1_OVERSAMPLE_FLAG1_DL1() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.IPP_CSI2_DPHY1_XL_DBG_OBS_ts.CSI1_OVERSAMPLE_FLAG1_DL1


#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS_CSI1_OVERSAMPLE_FLAG2_DL1() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.IPP_CSI2_DPHY1_XL_DBG_OBS_ts.CSI1_OVERSAMPLE_FLAG2_DL1


#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS_CSI1_HS_RX_ZM_CL() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.IPP_CSI2_DPHY1_XL_DBG_OBS_ts.CSI1_HS_RX_ZM_CL


#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS_CSI1_HS_RX_ZM_DL1() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.IPP_CSI2_DPHY1_XL_DBG_OBS_ts.CSI1_HS_RX_ZM_DL1


#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS_CSI1_RX_INIT_CLK() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.IPP_CSI2_DPHY1_XL_DBG_OBS_ts.CSI1_RX_INIT_CLK


#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS_CSI1_STOP_STATE_CL() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.IPP_CSI2_DPHY1_XL_DBG_OBS_ts.CSI1_STOP_STATE_CL


#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS_CSI1_STOP_STATE_DL1() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.IPP_CSI2_DPHY1_XL_DBG_OBS_ts.CSI1_STOP_STATE_DL1


#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS_CSI1_ULP_ACTIVE_NOT_CL() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.IPP_CSI2_DPHY1_XL_DBG_OBS_ts.CSI1_ULP_ACTIVE_NOT_CL


#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS_CSI1_ULP_ACTIVE_NOT_DL1() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.IPP_CSI2_DPHY1_XL_DBG_OBS_ts.CSI1_ULP_ACTIVE_NOT_DL1
#define Get_IPP_IPP_CSI2_DPHY1_XL_DBG_OBS() p_IPP_IP->IPP_CSI2_DPHY1_XL_DBG_OBS.word


//IPP_CSI2_DPHY0_DBG_ITS


#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITS_CSI0_ERR_ESC_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS.IPP_CSI2_DPHY0_DBG_ITS_ts.CSI0_ERR_ESC_DL1


#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITS_CSI0_ERR_ESC_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS.IPP_CSI2_DPHY0_DBG_ITS_ts.CSI0_ERR_ESC_DL2


#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITS_CSI0_ERR_ESC_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS.IPP_CSI2_DPHY0_DBG_ITS_ts.CSI0_ERR_ESC_DL3


#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITS_CSI0_ERR_SYNC_ESC_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS.IPP_CSI2_DPHY0_DBG_ITS_ts.CSI0_ERR_SYNC_ESC_DL1


#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITS_CSI0_ERR_SYNC_ESC_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS.IPP_CSI2_DPHY0_DBG_ITS_ts.CSI0_ERR_SYNC_ESC_DL2

#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITS_CSI0_ERR_ESC_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS.IPP_CSI2_DPHY0_DBG_ITS_ts.CSI0_ERR_ESC_DL4
#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITS_CSI0_ERR_SYNC_ESC_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS.IPP_CSI2_DPHY0_DBG_ITS_ts.CSI0_ERR_SYNC_ESC_DL4

#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITS_CSI0_ERR_SYNC_ESC_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS.IPP_CSI2_DPHY0_DBG_ITS_ts.CSI0_ERR_SYNC_ESC_DL3
#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITS() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS.word


//IPP_CSI2_DPHY0_DBG_ITS_BCLR


#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITS_BCLR_CSI0_DBG_ITS_BCLR(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS_BCLR.IPP_CSI2_DPHY0_DBG_ITS_BCLR_ts.CSI0_DBG_ITS_BCLR = x)
#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITS_BCLR(CSI0_DBG_ITS_BCLR) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS_BCLR.word = (uint16_t)CSI0_DBG_ITS_BCLR<<0)
#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITS_BCLR_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS_BCLR.word = x)


//IPP_CSI2_DPHY0_DBG_ITS_BSET


#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITS_BSET_CSI0_DBG_ITS_BSET(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS_BSET.IPP_CSI2_DPHY0_DBG_ITS_BSET_ts.CSI0_DBG_ITS_BSET = x)
#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITS_BSET(CSI0_DBG_ITS_BSET) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS_BSET.word = (uint16_t)CSI0_DBG_ITS_BSET<<0)
#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITS_BSET_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITS_BSET.word = x)


//IPP_CSI2_DPHY0_DBG_ITM


#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITM_CSI0_DBG_ITM() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITM.IPP_CSI2_DPHY0_DBG_ITM_ts.CSI0_DBG_ITM
#define Get_IPP_IPP_CSI2_DPHY0_DBG_ITM() p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITM.word


//IPP_CSI2_DPHY0_DBG_ITM_BCLR


#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITM_BCLR_CSI0_DBG_ITM_BCLR(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITM_BCLR.IPP_CSI2_DPHY0_DBG_ITM_BCLR_ts.CSI0_DBG_ITM_BCLR = x)
#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITM_BCLR(CSI0_DBG_ITM_BCLR) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITM_BCLR.word = (uint16_t)CSI0_DBG_ITM_BCLR<<0)
#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITM_BCLR_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITM_BCLR.word = x)


//IPP_CSI2_DPHY0_DBG_ITM_BSET


#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITM_BSET_CSI0_DBG_ITM_BSET(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITM_BSET.IPP_CSI2_DPHY0_DBG_ITM_BSET_ts.CSI0_DBG_ITM_BSET = x)
#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITM_BSET(CSI0_DBG_ITM_BSET) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITM_BSET.word = (uint16_t)CSI0_DBG_ITM_BSET<<0)
#define Set_IPP_IPP_CSI2_DPHY0_DBG_ITM_BSET_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG_ITM_BSET.word = x)


//IPP_CSI2_DPHY1_DBG_ITS


#define Get_IPP_IPP_CSI2_DPHY1_DBG_ITS_CSI1_ERR_ESC_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITS.IPP_CSI2_DPHY1_DBG_ITS_ts.CSI1_ERR_ESC_DL1


#define Get_IPP_IPP_CSI2_DPHY1_DBG_ITS_CSI1_ERR_SYNC_ESC_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITS.IPP_CSI2_DPHY1_DBG_ITS_ts.CSI1_ERR_SYNC_ESC_DL1
#define Get_IPP_IPP_CSI2_DPHY1_DBG_ITS() p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITS.word


//IPP_CSI2_DPHY1_DBG_ITS_BCLR


#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITS_BCLR_CSI1_DBG_ITS_BCLR(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITS_BCLR.IPP_CSI2_DPHY1_DBG_ITS_BCLR_ts.CSI1_DBG_ITS_BCLR = x)
#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITS_BCLR(CSI1_DBG_ITS_BCLR) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITS_BCLR.word = (uint16_t)CSI1_DBG_ITS_BCLR<<0)
#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITS_BCLR_word(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITS_BCLR.word = x)


//IPP_CSI2_DPHY1_DBG_ITS_BSET


#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITS_BSET_CSI1_DBG_ITS_BSET(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITS_BSET.IPP_CSI2_DPHY1_DBG_ITS_BSET_ts.CSI1_DBG_ITS_BSET = x)
#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITS_BSET(CSI1_DBG_ITS_BSET) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITS_BSET.word = (uint16_t)CSI1_DBG_ITS_BSET<<0)
#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITS_BSET_word(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITS_BSET.word = x)


//IPP_CSI2_DPHY1_DBG_ITM


#define Get_IPP_IPP_CSI2_DPHY1_DBG_ITM_CSI1_DBG_ITM() p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITM.IPP_CSI2_DPHY1_DBG_ITM_ts.CSI1_DBG_ITM
#define Get_IPP_IPP_CSI2_DPHY1_DBG_ITM() p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITM.word


//IPP_CSI2_DPHY1_DBG_ITM_BCLR


#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITM_BCLR_CSI1_DBG_ITM_BCLR(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITM_BCLR.IPP_CSI2_DPHY1_DBG_ITM_BCLR_ts.CSI1_DBG_ITM_BCLR = x)
#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITM_BCLR(CSI1_DBG_ITM_BCLR) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITM_BCLR.word = (uint16_t)CSI1_DBG_ITM_BCLR<<0)
#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITM_BCLR_word(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITM_BCLR.word = x)


//IPP_CSI2_DPHY1_DBG_ITM_BSET


#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITM_BSET_CSI1_DBG_ITM_BSET(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITM_BSET.IPP_CSI2_DPHY1_DBG_ITM_BSET_ts.CSI1_DBG_ITM_BSET = x)
#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITM_BSET(CSI1_DBG_ITM_BSET) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITM_BSET.word = (uint16_t)CSI1_DBG_ITM_BSET<<0)
#define Set_IPP_IPP_CSI2_DPHY1_DBG_ITM_BSET_word(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG_ITM_BSET.word = x)


//IPP_CSI2_PACKET_DBG_ITS


#define Get_IPP_IPP_CSI2_PACKET_DBG_ITS_CSI2_PACKET_ECC_CORRECTED_DBG() p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS.IPP_CSI2_PACKET_DBG_ITS_ts.CSI2_PACKET_ECC_CORRECTED_DBG


#define Get_IPP_IPP_CSI2_PACKET_DBG_ITS_CSI2_3D_PACKET_ECC_CORRECTED_DBG() p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS.IPP_CSI2_PACKET_DBG_ITS_ts.CSI2_3D_PACKET_ECC_CORRECTED_DBG
#define Get_IPP_IPP_CSI2_PACKET_DBG_ITS() p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS.word


//IPP_CSI2_PACKET_DBG_ITS_BCLR


#define Set_IPP_IPP_CSI2_PACKET_DBG_ITS_BCLR_CSI2_PACKET_DBG_ITS_BCLR(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS_BCLR.IPP_CSI2_PACKET_DBG_ITS_BCLR_ts.CSI2_PACKET_DBG_ITS_BCLR = x)


#define Set_IPP_IPP_CSI2_PACKET_DBG_ITS_BCLR_CSI2_3D_PACKET_DBG_ITS_BCLR(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS_BCLR.IPP_CSI2_PACKET_DBG_ITS_BCLR_ts.CSI2_3D_PACKET_DBG_ITS_BCLR = x)
#define Set_IPP_IPP_CSI2_PACKET_DBG_ITS_BCLR(CSI2_PACKET_DBG_ITS_BCLR,CSI2_3D_PACKET_DBG_ITS_BCLR) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS_BCLR.word = (uint16_t)CSI2_PACKET_DBG_ITS_BCLR<<0 | (uint16_t)CSI2_3D_PACKET_DBG_ITS_BCLR<<8)
#define Set_IPP_IPP_CSI2_PACKET_DBG_ITS_BCLR_word(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS_BCLR.word = x)


//IPP_CSI2_PACKET_DBG_ITS_BSET


#define Set_IPP_IPP_CSI2_PACKET_DBG_ITS_BSET_CSI2_PACKET_DBG_ITS_BSET(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS_BSET.IPP_CSI2_PACKET_DBG_ITS_BSET_ts.CSI2_PACKET_DBG_ITS_BSET = x)


#define Set_IPP_IPP_CSI2_PACKET_DBG_ITS_BSET_CSI2_3D_PACKET_DBG_ITS_BSET(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS_BSET.IPP_CSI2_PACKET_DBG_ITS_BSET_ts.CSI2_3D_PACKET_DBG_ITS_BSET = x)
#define Set_IPP_IPP_CSI2_PACKET_DBG_ITS_BSET(CSI2_PACKET_DBG_ITS_BSET,CSI2_3D_PACKET_DBG_ITS_BSET) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS_BSET.word = (uint16_t)CSI2_PACKET_DBG_ITS_BSET<<0 | (uint16_t)CSI2_3D_PACKET_DBG_ITS_BSET<<8)
#define Set_IPP_IPP_CSI2_PACKET_DBG_ITS_BSET_word(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITS_BSET.word = x)


//IPP_CSI2_PACKET_DBG_ITM


#define Get_IPP_IPP_CSI2_PACKET_DBG_ITM_CSI2_PACKET_DBG_ITM() p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM.IPP_CSI2_PACKET_DBG_ITM_ts.CSI2_PACKET_DBG_ITM


#define Get_IPP_IPP_CSI2_PACKET_DBG_ITM_CSI2_3D_PACKET_DBG_ITM() p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM.IPP_CSI2_PACKET_DBG_ITM_ts.CSI2_3D_PACKET_DBG_ITM
#define Get_IPP_IPP_CSI2_PACKET_DBG_ITM() p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM.word


//IPP_CSI2_PACKET_DBG_ITM_BCLR


#define Set_IPP_IPP_CSI2_PACKET_DBG_ITM_BCLR_CSI2_PACKET_DBG_ITM_BCLR(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM_BCLR.IPP_CSI2_PACKET_DBG_ITM_BCLR_ts.CSI2_PACKET_DBG_ITM_BCLR = x)


#define Set_IPP_IPP_CSI2_PACKET_DBG_ITM_BCLR_CSI2_3D_PACKET_DBG_ITM_BCLR(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM_BCLR.IPP_CSI2_PACKET_DBG_ITM_BCLR_ts.CSI2_3D_PACKET_DBG_ITM_BCLR = x)
#define Set_IPP_IPP_CSI2_PACKET_DBG_ITM_BCLR(CSI2_PACKET_DBG_ITM_BCLR,CSI2_3D_PACKET_DBG_ITM_BCLR) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM_BCLR.word = (uint16_t)CSI2_PACKET_DBG_ITM_BCLR<<0 | (uint16_t)CSI2_3D_PACKET_DBG_ITM_BCLR<<8)
#define Set_IPP_IPP_CSI2_PACKET_DBG_ITM_BCLR_word(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM_BCLR.word = x)


//IPP_CSI2_PACKET_DBG_ITM_BSET


#define Set_IPP_IPP_CSI2_PACKET_DBG_ITM_BSET_CSI2_PACKET_DBG_ITM_BSET(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM_BSET.IPP_CSI2_PACKET_DBG_ITM_BSET_ts.CSI2_PACKET_DBG_ITM_BSET = x)


#define Set_IPP_IPP_CSI2_PACKET_DBG_ITM_BSET_CSI2_3D_PACKET_DBG_ITM_BSET(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM_BSET.IPP_CSI2_PACKET_DBG_ITM_BSET_ts.CSI2_3D_PACKET_DBG_ITM_BSET = x)
#define Set_IPP_IPP_CSI2_PACKET_DBG_ITM_BSET(CSI2_PACKET_DBG_ITM_BSET,CSI2_3D_PACKET_DBG_ITM_BSET) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM_BSET.word = (uint16_t)CSI2_PACKET_DBG_ITM_BSET<<0 | (uint16_t)CSI2_3D_PACKET_DBG_ITM_BSET<<8)
#define Set_IPP_IPP_CSI2_PACKET_DBG_ITM_BSET_word(x) (p_IPP_IP->IPP_CSI2_PACKET_DBG_ITM_BSET.word = x)


//DEPRECATED_IPP1
#define Get_IPP_DEPRECATED_IPP1() p_IPP_IP->DEPRECATED_IPP1.word


//IPP_STATIC_CSI2RX_DATA_TYPE_W


#define Get_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_W_STATIC_CSI2RX_IMAGE_DATA_TYPE() p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_W.IPP_STATIC_CSI2RX_DATA_TYPE_W_ts.STATIC_CSI2RX_IMAGE_DATA_TYPE
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_W_STATIC_CSI2RX_IMAGE_DATA_TYPE(x) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_W.IPP_STATIC_CSI2RX_DATA_TYPE_W_ts.STATIC_CSI2RX_IMAGE_DATA_TYPE = x)


#define Get_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_W_STATIC_CSI2RX_NON_IMAGE_DATA_TYPE() p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_W.IPP_STATIC_CSI2RX_DATA_TYPE_W_ts.STATIC_CSI2RX_NON_IMAGE_DATA_TYPE
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_W_STATIC_CSI2RX_NON_IMAGE_DATA_TYPE(x) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_W.IPP_STATIC_CSI2RX_DATA_TYPE_W_ts.STATIC_CSI2RX_NON_IMAGE_DATA_TYPE = x)
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_W(STATIC_CSI2RX_IMAGE_DATA_TYPE,STATIC_CSI2RX_NON_IMAGE_DATA_TYPE) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_W.word = (uint16_t)STATIC_CSI2RX_IMAGE_DATA_TYPE<<0 | (uint16_t)STATIC_CSI2RX_NON_IMAGE_DATA_TYPE<<8)
#define Get_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_W() p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_W.word
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_W_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_W.word = x)


//DEPRECATED_IPP3
#define Get_IPP_DEPRECATED_IPP3() p_IPP_IP->DEPRECATED_IPP3.word
#define Set_IPP_DEPRECATED_IPP3_word(x) (p_IPP_IP->DEPRECATED_IPP3.word = x)


//DEPRECATED_IPP4
#define Get_IPP_DEPRECATED_IPP4() p_IPP_IP->DEPRECATED_IPP4.word


//DEPRECATED_IPP5
#define Get_IPP_DEPRECATED_IPP5() p_IPP_IP->DEPRECATED_IPP5.word


//IPP_STATIC_CSI2RX_PIX_WIDTH_W


#define Get_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_STATIC_CSI2RX_ACTDATA_PIXWIDTH() p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W.IPP_STATIC_CSI2RX_PIX_WIDTH_W_ts.STATIC_CSI2RX_ACTDATA_PIXWIDTH
#define Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_STATIC_CSI2RX_ACTDATA_PIXWIDTH(x) (p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W.IPP_STATIC_CSI2RX_PIX_WIDTH_W_ts.STATIC_CSI2RX_ACTDATA_PIXWIDTH = x)


#define Get_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_STATIC_CSI2RX_EMBDATA_PIXWIDTH() p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W.IPP_STATIC_CSI2RX_PIX_WIDTH_W_ts.STATIC_CSI2RX_EMBDATA_PIXWIDTH
#define Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_STATIC_CSI2RX_EMBDATA_PIXWIDTH(x) (p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W.IPP_STATIC_CSI2RX_PIX_WIDTH_W_ts.STATIC_CSI2RX_EMBDATA_PIXWIDTH = x)
#define Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W(STATIC_CSI2RX_ACTDATA_PIXWIDTH,STATIC_CSI2RX_EMBDATA_PIXWIDTH) (p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W.word = (uint16_t)STATIC_CSI2RX_ACTDATA_PIXWIDTH<<0 | (uint16_t)STATIC_CSI2RX_EMBDATA_PIXWIDTH<<4)
#define Get_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W() p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W.word
#define Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W.word = x)


//IPP_CSI2_DPHY_ERROR_ITS


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_EOT_SYNC_HS_DL1() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_EOT_SYNC_HS_DL1


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_EOT_SYNC_HS_DL2() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_EOT_SYNC_HS_DL2


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_EOT_SYNC_HS_DL3() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_EOT_SYNC_HS_DL3


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_SOT_HS_DL1() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_SOT_HS_DL1


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_SOT_HS_DL2() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_SOT_HS_DL2


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_SOT_HS_DL3() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_SOT_HS_DL3


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_SOT_SYNC_HS_DL1() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_SOT_SYNC_HS_DL1


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_SOT_SYNC_HS_DL2() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_SOT_SYNC_HS_DL2


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_SOT_SYNC_HS_DL3() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_SOT_SYNC_HS_DL3


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_CONTROL_DL1() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_CONTROL_DL1


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_CONTROL_DL2() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_CONTROL_DL2


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI0_ERR_CONTROL_DL3() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI0_ERR_CONTROL_DL3


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI1_ERR_EOT_SYNC_HS_DL1() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI1_ERR_EOT_SYNC_HS_DL1


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI1_ERR_SOT_HS_DL1() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI1_ERR_SOT_HS_DL1


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI1_ERR_SOT_SYNC_HS_DL1() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI1_ERR_SOT_SYNC_HS_DL1


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS_CSI1_ERR_CONTROL_DL1() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.IPP_CSI2_DPHY_ERROR_ITS_ts.CSI1_ERR_CONTROL_DL1
#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITS() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS.word


//IPP_CSI2_DPHY_ERROR_ITS_BCLR


#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITS_BCLR_CSI_DPHY_ERROR_ITS_BCLR(x) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS_BCLR.IPP_CSI2_DPHY_ERROR_ITS_BCLR_ts.CSI_DPHY_ERROR_ITS_BCLR = x)
#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITS_BCLR(CSI_DPHY_ERROR_ITS_BCLR) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS_BCLR.word = (uint16_t)CSI_DPHY_ERROR_ITS_BCLR<<0)
#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITS_BCLR_word(x) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS_BCLR.word = x)


//IPP_CSI2_DPHY_ERROR_ITS_BSET


#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITS_BSET_CSI_DPHY_ERROR_ITS_BSET(x) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS_BSET.IPP_CSI2_DPHY_ERROR_ITS_BSET_ts.CSI_DPHY_ERROR_ITS_BSET = x)
#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITS_BSET(CSI_DPHY_ERROR_ITS_BSET) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS_BSET.word = (uint16_t)CSI_DPHY_ERROR_ITS_BSET<<0)
#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITS_BSET_word(x) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITS_BSET.word = x)


//IPP_CSI2_DPHY_ERROR_ITM


#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITM_CSI_DPHY_ERROR_ITM() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITM.IPP_CSI2_DPHY_ERROR_ITM_ts.CSI_DPHY_ERROR_ITM
#define Get_IPP_IPP_CSI2_DPHY_ERROR_ITM() p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITM.word


//IPP_CSI2_DPHY_ERROR_ITM_BCLR


#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITM_BCLR_CSI_DPHY_ERROR_ITM_BCLR(x) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITM_BCLR.IPP_CSI2_DPHY_ERROR_ITM_BCLR_ts.CSI_DPHY_ERROR_ITM_BCLR = x)
#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITM_BCLR(CSI_DPHY_ERROR_ITM_BCLR) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITM_BCLR.word = (uint16_t)CSI_DPHY_ERROR_ITM_BCLR<<0)
#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITM_BCLR_word(x) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITM_BCLR.word = x)


//IPP_CSI2_DPHY_ERROR_ITM_BSET


#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITM_BSET_CSI_DPHY_ERROR_ITM_BSET(x) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITM_BSET.IPP_CSI2_DPHY_ERROR_ITM_BSET_ts.CSI_DPHY_ERROR_ITM_BSET = x)
#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITM_BSET(CSI_DPHY_ERROR_ITM_BSET) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITM_BSET.word = (uint16_t)CSI_DPHY_ERROR_ITM_BSET<<0)
#define Set_IPP_IPP_CSI2_DPHY_ERROR_ITM_BSET_word(x) (p_IPP_IP->IPP_CSI2_DPHY_ERROR_ITM_BSET.word = x)


//IPP_CSI2_PACKET_ERROR_ITS


#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITS_CSI2_PACKET_PAYLOAD_CRC_ERROR() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS.IPP_CSI2_PACKET_ERROR_ITS_ts.CSI2_PACKET_PAYLOAD_CRC_ERROR


#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITS_CSI2_PACKET_HEADER_ECC_ERROR() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS.IPP_CSI2_PACKET_ERROR_ITS_ts.CSI2_PACKET_HEADER_ECC_ERROR


#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITS_CSI2_EOF() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS.IPP_CSI2_PACKET_ERROR_ITS_ts.CSI2_EOF


#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITS_CSI2_3D_PACKET_PAYLOAD_CRC_ERROR() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS.IPP_CSI2_PACKET_ERROR_ITS_ts.CSI2_3D_PACKET_PAYLOAD_CRC_ERROR


#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITS_CSI2_3D_PACKET_HEADER_ECC_ERROR() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS.IPP_CSI2_PACKET_ERROR_ITS_ts.CSI2_3D_PACKET_HEADER_ECC_ERROR


#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITS_CSI2_3D_EOF() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS.IPP_CSI2_PACKET_ERROR_ITS_ts.CSI2_3D_EOF
#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITS() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS.word


//IPP_CSI2_PACKET_ERROR_ITS_BCLR


#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BCLR_CSI_PACKET_ERROR_ITS_BCLR(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS_BCLR.IPP_CSI2_PACKET_ERROR_ITS_BCLR_ts.CSI_PACKET_ERROR_ITS_BCLR = x)


#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BCLR_CSI_3D_PACKET_ERROR_ITS_BCLR(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS_BCLR.IPP_CSI2_PACKET_ERROR_ITS_BCLR_ts.CSI_3D_PACKET_ERROR_ITS_BCLR = x)
#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BCLR(CSI_PACKET_ERROR_ITS_BCLR,CSI_3D_PACKET_ERROR_ITS_BCLR) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS_BCLR.word = (uint16_t)CSI_PACKET_ERROR_ITS_BCLR<<0 | (uint16_t)CSI_3D_PACKET_ERROR_ITS_BCLR<<8)
#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BCLR_word(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS_BCLR.word = x)


//IPP_CSI2_PACKET_ERROR_ITS_BSET


#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BSET_CSI_PACKET_ERROR_ITS_BSET(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS_BSET.IPP_CSI2_PACKET_ERROR_ITS_BSET_ts.CSI_PACKET_ERROR_ITS_BSET = x)


#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BSET_CSI_3D_PACKET_ERROR_ITS_BSET(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS_BSET.IPP_CSI2_PACKET_ERROR_ITS_BSET_ts.CSI_3D_PACKET_ERROR_ITS_BSET = x)
#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BSET(CSI_PACKET_ERROR_ITS_BSET,CSI_3D_PACKET_ERROR_ITS_BSET) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS_BSET.word = (uint16_t)CSI_PACKET_ERROR_ITS_BSET<<0 | (uint16_t)CSI_3D_PACKET_ERROR_ITS_BSET<<8)
#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITS_BSET_word(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITS_BSET.word = x)


//IPP_CSI2_PACKET_ERROR_ITM


#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITM_CSI_PACKET_ERROR_ITM() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM.IPP_CSI2_PACKET_ERROR_ITM_ts.CSI_PACKET_ERROR_ITM


#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITM_CSI_3D_PACKET_ERROR_ITM() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM.IPP_CSI2_PACKET_ERROR_ITM_ts.CSI_3D_PACKET_ERROR_ITM
#define Get_IPP_IPP_CSI2_PACKET_ERROR_ITM() p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM.word


//IPP_CSI2_PACKET_ERROR_ITM_BCLR


#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITM_BCLR_CSI_PACKET_ERROR_ITM_BCLR(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM_BCLR.IPP_CSI2_PACKET_ERROR_ITM_BCLR_ts.CSI_PACKET_ERROR_ITM_BCLR = x)


#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITM_BCLR_CSI_3D_PACKET_ERROR_ITM_BCLR(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM_BCLR.IPP_CSI2_PACKET_ERROR_ITM_BCLR_ts.CSI_3D_PACKET_ERROR_ITM_BCLR = x)
#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITM_BCLR(CSI_PACKET_ERROR_ITM_BCLR,CSI_3D_PACKET_ERROR_ITM_BCLR) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM_BCLR.word = (uint16_t)CSI_PACKET_ERROR_ITM_BCLR<<0 | (uint16_t)CSI_3D_PACKET_ERROR_ITM_BCLR<<8)
#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITM_BCLR_word(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM_BCLR.word = x)


//IPP_CSI2_PACKET_ERROR_ITM_BSET


#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITM_BSET_CSI_PACKET_ERROR_ITM_BSET(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM_BSET.IPP_CSI2_PACKET_ERROR_ITM_BSET_ts.CSI_PACKET_ERROR_ITM_BSET = x)


#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITM_BSET_CSI_3D_PACKET_ERROR_ITM_BSET(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM_BSET.IPP_CSI2_PACKET_ERROR_ITM_BSET_ts.CSI_3D_PACKET_ERROR_ITM_BSET = x)
#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITM_BSET(CSI_PACKET_ERROR_ITM_BSET,CSI_3D_PACKET_ERROR_ITM_BSET) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM_BSET.word = (uint16_t)CSI_PACKET_ERROR_ITM_BSET<<0 | (uint16_t)CSI_3D_PACKET_ERROR_ITM_BSET<<8)
#define Set_IPP_IPP_CSI2_PACKET_ERROR_ITM_BSET_word(x) (p_IPP_IP->IPP_CSI2_PACKET_ERROR_ITM_BSET.word = x)


//IPP_STATIC_OPIPE03_SEL


#define Get_IPP_IPP_STATIC_OPIPE03_SEL_STATIC_OPIPE03_SEL() p_IPP_IP->IPP_STATIC_OPIPE03_SEL.IPP_STATIC_OPIPE03_SEL_ts.STATIC_OPIPE03_SEL
#define Set_IPP_IPP_STATIC_OPIPE03_SEL_STATIC_OPIPE03_SEL(x) (p_IPP_IP->IPP_STATIC_OPIPE03_SEL.IPP_STATIC_OPIPE03_SEL_ts.STATIC_OPIPE03_SEL = x)
#define Set_IPP_IPP_STATIC_OPIPE03_SEL(STATIC_OPIPE03_SEL) (p_IPP_IP->IPP_STATIC_OPIPE03_SEL.word = (uint32_t)STATIC_OPIPE03_SEL<<0)
#define Get_IPP_IPP_STATIC_OPIPE03_SEL() p_IPP_IP->IPP_STATIC_OPIPE03_SEL.word
#define Set_IPP_IPP_STATIC_OPIPE03_SEL_word(x) (p_IPP_IP->IPP_STATIC_OPIPE03_SEL.word = x)


//IPP_ISP_ASYNC_RESET
#define Set_IPP_IPP_ISP_ASYNC_RESET_word(x) (p_IPP_IP->IPP_ISP_ASYNC_RESET.word = x)


//IPP_SD_RESET


#define Set_IPP_IPP_SD_RESET_ALL(x) (p_IPP_IP->IPP_SD_RESET.IPP_SD_RESET_ts.ALL = x)


#define Set_IPP_IPP_SD_RESET_BRML(x) (p_IPP_IP->IPP_SD_RESET.IPP_SD_RESET_ts.BRML = x)


#define Set_IPP_IPP_SD_RESET_SDI(x) (p_IPP_IP->IPP_SD_RESET.IPP_SD_RESET_ts.SDI = x)
#define Set_IPP_IPP_SD_RESET(ALL,BRML,SDI) (p_IPP_IP->IPP_SD_RESET.word = (uint16_t)ALL<<0 | (uint16_t)BRML<<7 | (uint16_t)SDI<<8)
#define Set_IPP_IPP_SD_RESET_word(x) (p_IPP_IP->IPP_SD_RESET.word = x)


//IPP_SD_STATIC_CCP_EN


#define Get_IPP_IPP_SD_STATIC_CCP_EN_STATIC_SD_CCP_EN() p_IPP_IP->IPP_SD_STATIC_CCP_EN.IPP_SD_STATIC_CCP_EN_ts.STATIC_SD_CCP_EN
#define Is_IPP_IPP_SD_STATIC_CCP_EN_STATIC_SD_CCP_EN_B_0x0() (p_IPP_IP->IPP_SD_STATIC_CCP_EN.IPP_SD_STATIC_CCP_EN_ts.STATIC_SD_CCP_EN == STATIC_SD_CCP_EN_B_0x0)
#define Set_IPP_IPP_SD_STATIC_CCP_EN_STATIC_SD_CCP_EN__B_0x0() (p_IPP_IP->IPP_SD_STATIC_CCP_EN.IPP_SD_STATIC_CCP_EN_ts.STATIC_SD_CCP_EN = STATIC_SD_CCP_EN_B_0x0)
#define STATIC_SD_CCP_EN_B_0x0 0x0    //disabled
#define Is_IPP_IPP_SD_STATIC_CCP_EN_STATIC_SD_CCP_EN_B_0x1() (p_IPP_IP->IPP_SD_STATIC_CCP_EN.IPP_SD_STATIC_CCP_EN_ts.STATIC_SD_CCP_EN == STATIC_SD_CCP_EN_B_0x1)
#define Set_IPP_IPP_SD_STATIC_CCP_EN_STATIC_SD_CCP_EN__B_0x1() (p_IPP_IP->IPP_SD_STATIC_CCP_EN.IPP_SD_STATIC_CCP_EN_ts.STATIC_SD_CCP_EN = STATIC_SD_CCP_EN_B_0x1)
#define STATIC_SD_CCP_EN_B_0x1 0x1    //active
#define Set_IPP_IPP_SD_STATIC_CCP_EN(STATIC_SD_CCP_EN) (p_IPP_IP->IPP_SD_STATIC_CCP_EN.word = (uint16_t)STATIC_SD_CCP_EN<<0)
#define Get_IPP_IPP_SD_STATIC_CCP_EN() p_IPP_IP->IPP_SD_STATIC_CCP_EN.word
#define Set_IPP_IPP_SD_STATIC_CCP_EN_word(x) (p_IPP_IP->IPP_SD_STATIC_CCP_EN.word = x)


//IPP_SD_STATIC_SDG_CSI_MODE


#define Get_IPP_IPP_SD_STATIC_SDG_CSI_MODE_STATIC_CDG_CSI_MODE() p_IPP_IP->IPP_SD_STATIC_SDG_CSI_MODE.IPP_SD_STATIC_SDG_CSI_MODE_ts.STATIC_CDG_CSI_MODE
#define Is_IPP_IPP_SD_STATIC_SDG_CSI_MODE_STATIC_CDG_CSI_MODE_B_0x0() (p_IPP_IP->IPP_SD_STATIC_SDG_CSI_MODE.IPP_SD_STATIC_SDG_CSI_MODE_ts.STATIC_CDG_CSI_MODE == STATIC_CDG_CSI_MODE_B_0x0)
#define Set_IPP_IPP_SD_STATIC_SDG_CSI_MODE_STATIC_CDG_CSI_MODE__B_0x0() (p_IPP_IP->IPP_SD_STATIC_SDG_CSI_MODE.IPP_SD_STATIC_SDG_CSI_MODE_ts.STATIC_CDG_CSI_MODE = STATIC_CDG_CSI_MODE_B_0x0)
#define STATIC_CDG_CSI_MODE_B_0x0 0x0    //SMIA over CSI2 mode
#define Is_IPP_IPP_SD_STATIC_SDG_CSI_MODE_STATIC_CDG_CSI_MODE_B_0x1() (p_IPP_IP->IPP_SD_STATIC_SDG_CSI_MODE.IPP_SD_STATIC_SDG_CSI_MODE_ts.STATIC_CDG_CSI_MODE == STATIC_CDG_CSI_MODE_B_0x1)
#define Set_IPP_IPP_SD_STATIC_SDG_CSI_MODE_STATIC_CDG_CSI_MODE__B_0x1() (p_IPP_IP->IPP_SD_STATIC_SDG_CSI_MODE.IPP_SD_STATIC_SDG_CSI_MODE_ts.STATIC_CDG_CSI_MODE = STATIC_CDG_CSI_MODE_B_0x1)
#define STATIC_CDG_CSI_MODE_B_0x1 0x1    //CSI2 specific mode
#define Set_IPP_IPP_SD_STATIC_SDG_CSI_MODE(STATIC_CDG_CSI_MODE) (p_IPP_IP->IPP_SD_STATIC_SDG_CSI_MODE.word = (uint16_t)STATIC_CDG_CSI_MODE<<0)
#define Get_IPP_IPP_SD_STATIC_SDG_CSI_MODE() p_IPP_IP->IPP_SD_STATIC_SDG_CSI_MODE.word
#define Set_IPP_IPP_SD_STATIC_SDG_CSI_MODE_word(x) (p_IPP_IP->IPP_SD_STATIC_SDG_CSI_MODE.word = x)


//IPP_SD_PRY0_ENABLE_SET
#define Set_IPP_IPP_SD_PRY0_ENABLE_SET_word(x) (p_IPP_IP->IPP_SD_PRY0_ENABLE_SET.word = x)


//IPP_SD_PRY1_ENABLE_SET
#define Set_IPP_IPP_SD_PRY1_ENABLE_SET_word(x) (p_IPP_IP->IPP_SD_PRY1_ENABLE_SET.word = x)


//IPP_SD_PRY0_ENABLE_CLR
#define Set_IPP_IPP_SD_PRY0_ENABLE_CLR_word(x) (p_IPP_IP->IPP_SD_PRY0_ENABLE_CLR.word = x)


//IPP_SD_PRY1_ENABLE_CLR
#define Set_IPP_IPP_SD_PRY1_ENABLE_CLR_word(x) (p_IPP_IP->IPP_SD_PRY1_ENABLE_CLR.word = x)


//IPP_SD_PRY0_ENABLE_STATUS


#define Get_IPP_IPP_SD_PRY0_ENABLE_STATUS_PRY0_ENABLE_STATUS() p_IPP_IP->IPP_SD_PRY0_ENABLE_STATUS.IPP_SD_PRY0_ENABLE_STATUS_ts.PRY0_ENABLE_STATUS
#define Get_IPP_IPP_SD_PRY0_ENABLE_STATUS() p_IPP_IP->IPP_SD_PRY0_ENABLE_STATUS.word


//IPP_SD_PRY1_ENABLE_STATUS


#define Get_IPP_IPP_SD_PRY1_ENABLE_STATUS_PRY1_ENABLE_STATUS() p_IPP_IP->IPP_SD_PRY1_ENABLE_STATUS.IPP_SD_PRY1_ENABLE_STATUS_ts.PRY1_ENABLE_STATUS
#define Get_IPP_IPP_SD_PRY1_ENABLE_STATUS() p_IPP_IP->IPP_SD_PRY1_ENABLE_STATUS.word


//IPP_SD_BMS_ENABLE_SET
#define Set_IPP_IPP_SD_BMS_ENABLE_SET_word(x) (p_IPP_IP->IPP_SD_BMS_ENABLE_SET.word = x)


//IPP_SD_BMS_ENABLE_CLR
#define Set_IPP_IPP_SD_BMS_ENABLE_CLR_word(x) (p_IPP_IP->IPP_SD_BMS_ENABLE_CLR.word = x)


//IPP_SD_BMS_ENABLE_STATUS


#define Get_IPP_IPP_SD_BMS_ENABLE_STATUS_BMS_ENABLE_STATUS() p_IPP_IP->IPP_SD_BMS_ENABLE_STATUS.IPP_SD_BMS_ENABLE_STATUS_ts.BMS_ENABLE_STATUS
#define Get_IPP_IPP_SD_BMS_ENABLE_STATUS() p_IPP_IP->IPP_SD_BMS_ENABLE_STATUS.word


//IPP_SD_PRY0_FORMAT


#define Get_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT() p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT
#define Is_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT_B_0x1() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT == PRY0_FORMAT_B_0x1)
#define Set_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT__B_0x1() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT = PRY0_FORMAT_B_0x1)
#define PRY0_FORMAT_B_0x1 0x1    //YUV 4:2:2 1buffer
#define Is_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT_B_0x2() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT == PRY0_FORMAT_B_0x2)
#define Set_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT__B_0x2() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT = PRY0_FORMAT_B_0x2)
#define PRY0_FORMAT_B_0x2 0x2    //YUV 4:2:2 2buffers
#define Is_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT_B_0x6() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT == PRY0_FORMAT_B_0x6)
#define Set_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT__B_0x6() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT = PRY0_FORMAT_B_0x6)
#define PRY0_FORMAT_B_0x6 0x6    //YUV 4:2:0 2buffers
#define Is_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT_B_0x7() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT == PRY0_FORMAT_B_0x7)
#define Set_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT__B_0x7() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT = PRY0_FORMAT_B_0x7)
#define PRY0_FORMAT_B_0x7 0x7    //YUV 4:2:0 3buffers
#define Is_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT_B_0xC() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT == PRY0_FORMAT_B_0xC)
#define Set_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT__B_0xC() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT = PRY0_FORMAT_B_0xC)
#define PRY0_FORMAT_B_0xC 0xC    //RGB888 (24bpp)
#define Is_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT_B_0xE() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT == PRY0_FORMAT_B_0xE)
#define Set_IPP_IPP_SD_PRY0_FORMAT_PRY0_FORMAT__B_0xE() (p_IPP_IP->IPP_SD_PRY0_FORMAT.IPP_SD_PRY0_FORMAT_ts.PRY0_FORMAT = PRY0_FORMAT_B_0xE)
#define PRY0_FORMAT_B_0xE 0xE    //R10G10B10 (30bpp)
#define Set_IPP_IPP_SD_PRY0_FORMAT(PRY0_FORMAT) (p_IPP_IP->IPP_SD_PRY0_FORMAT.word = (uint16_t)PRY0_FORMAT<<8)
#define Get_IPP_IPP_SD_PRY0_FORMAT() p_IPP_IP->IPP_SD_PRY0_FORMAT.word
#define Set_IPP_IPP_SD_PRY0_FORMAT_word(x) (p_IPP_IP->IPP_SD_PRY0_FORMAT.word = x)


//IPP_SD_PRY1_FORMAT


#define Get_IPP_IPP_SD_PRY1_FORMAT_PRY1_ALPHA() p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_ALPHA
#define Set_IPP_IPP_SD_PRY1_FORMAT_PRY1_ALPHA(x) (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_ALPHA = x)


#define Get_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT() p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT
#define Is_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT_B_0x1() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT == PRY1_FORMAT_B_0x1)
#define Set_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT__B_0x1() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT = PRY1_FORMAT_B_0x1)
#define PRY1_FORMAT_B_0x1 0x1    //YUV 4:2:2 1buffer
#define Is_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT_B_0x8() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT == PRY1_FORMAT_B_0x8)
#define Set_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT__B_0x8() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT = PRY1_FORMAT_B_0x8)
#define PRY1_FORMAT_B_0x8 0x8    //A4R4G4B4 (16bpp)
#define Is_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT_B_0x9() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT == PRY1_FORMAT_B_0x9)
#define Set_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT__B_0x9() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT = PRY1_FORMAT_B_0x9)
#define PRY1_FORMAT_B_0x9 0x9    //A1R5G5B5 (16bpp)
#define Is_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT_B_0xA() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT == PRY1_FORMAT_B_0xA)
#define Set_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT__B_0xA() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT = PRY1_FORMAT_B_0xA)
#define PRY1_FORMAT_B_0xA 0xA    //R5G6B5 (16bpp)
#define Is_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT_B_0xC() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT == PRY1_FORMAT_B_0xC)
#define Set_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT__B_0xC() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT = PRY1_FORMAT_B_0xC)
#define PRY1_FORMAT_B_0xC 0xC    //R8G8B8 (24bpp)
#define Is_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT_B_0xD() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT == PRY1_FORMAT_B_0xD)
#define Set_IPP_IPP_SD_PRY1_FORMAT_PRY1_FORMAT__B_0xD() (p_IPP_IP->IPP_SD_PRY1_FORMAT.IPP_SD_PRY1_FORMAT_ts.PRY1_FORMAT = PRY1_FORMAT_B_0xD)
#define PRY1_FORMAT_B_0xD 0xD    //A8R8G8B8 (32bpp)
#define Set_IPP_IPP_SD_PRY1_FORMAT(PRY1_ALPHA,PRY1_FORMAT) (p_IPP_IP->IPP_SD_PRY1_FORMAT.word = (uint16_t)PRY1_ALPHA<<0 | (uint16_t)PRY1_FORMAT<<8)
#define Get_IPP_IPP_SD_PRY1_FORMAT() p_IPP_IP->IPP_SD_PRY1_FORMAT.word
#define Set_IPP_IPP_SD_PRY1_FORMAT_word(x) (p_IPP_IP->IPP_SD_PRY1_FORMAT.word = x)


//IPP_SD_BMS_FORMAT


#define Get_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_FORMAT() p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_FORMAT
#define Is_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_FORMAT_B_0x0() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_FORMAT == BMS_RAW_FORMAT_B_0x0)
#define Set_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_FORMAT__B_0x0() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_FORMAT = BMS_RAW_FORMAT_B_0x0)
#define BMS_RAW_FORMAT_B_0x0 0x0    //RAW12
#define Is_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_FORMAT_B_0x1() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_FORMAT == BMS_RAW_FORMAT_B_0x1)
#define Set_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_FORMAT__B_0x1() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_FORMAT = BMS_RAW_FORMAT_B_0x1)
#define BMS_RAW_FORMAT_B_0x1 0x1    //RAW8


#define Get_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_SEL() p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_SEL
#define Is_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_SEL_B_0x0() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_SEL == BMS_RAW_SEL_B_0x0)
#define Set_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_SEL__B_0x0() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_SEL = BMS_RAW_SEL_B_0x0)
#define BMS_RAW_SEL_B_0x0 0x0    //Bayer raw store data output at the beginning of the ISP Pictor Bayer domain (before the line blanking elimination aka 'Bayer store 0', in clk_sd_hispeed domain)
#define Is_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_SEL_B_0x1() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_SEL == BMS_RAW_SEL_B_0x1)
#define Set_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_SEL__B_0x1() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_SEL = BMS_RAW_SEL_B_0x1)
#define BMS_RAW_SEL_B_0x1 0x1    //Bayer raw store data output at the end of the ISP Pictor Bayer domain (before the demosaic, aka 'Bayer store 2', in clk_pipe domain)
#define Is_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_SEL_B_0x10() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_SEL == BMS_RAW_SEL_B_0x10)
#define Set_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_SEL__B_0x10() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_SEL = BMS_RAW_SEL_B_0x10)
#define BMS_RAW_SEL_B_0x10 0x10    //Bayer raw store data output after digital gain
#define Is_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_SEL_B_0x11() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_SEL == BMS_RAW_SEL_B_0x11)
#define Set_IPP_IPP_SD_BMS_FORMAT_BMS_RAW_SEL__B_0x11() (p_IPP_IP->IPP_SD_BMS_FORMAT.IPP_SD_BMS_FORMAT_ts.BMS_RAW_SEL = BMS_RAW_SEL_B_0x11)
#define BMS_RAW_SEL_B_0x11 0x11    //Bayer raw store data output (dual capture / 3D path)
#define Set_IPP_IPP_SD_BMS_FORMAT(BMS_RAW_FORMAT,BMS_RAW_SEL) (p_IPP_IP->IPP_SD_BMS_FORMAT.word = (uint16_t)BMS_RAW_FORMAT<<0 | (uint16_t)BMS_RAW_SEL<<1)
#define Get_IPP_IPP_SD_BMS_FORMAT() p_IPP_IP->IPP_SD_BMS_FORMAT.word
#define Set_IPP_IPP_SD_BMS_FORMAT_word(x) (p_IPP_IP->IPP_SD_BMS_FORMAT.word = x)


//IPP_SD_STATIC_MEM_LOAD_EN


#define Get_IPP_IPP_SD_STATIC_MEM_LOAD_EN_STATIC_MEM_LOAD_EN() p_IPP_IP->IPP_SD_STATIC_MEM_LOAD_EN.IPP_SD_STATIC_MEM_LOAD_EN_ts.STATIC_MEM_LOAD_EN
#define Set_IPP_IPP_SD_STATIC_MEM_LOAD_EN_STATIC_MEM_LOAD_EN(x) (p_IPP_IP->IPP_SD_STATIC_MEM_LOAD_EN.IPP_SD_STATIC_MEM_LOAD_EN_ts.STATIC_MEM_LOAD_EN = x)
#define Set_IPP_IPP_SD_STATIC_MEM_LOAD_EN(STATIC_MEM_LOAD_EN) (p_IPP_IP->IPP_SD_STATIC_MEM_LOAD_EN.word = (uint16_t)STATIC_MEM_LOAD_EN<<0)
#define Get_IPP_IPP_SD_STATIC_MEM_LOAD_EN() p_IPP_IP->IPP_SD_STATIC_MEM_LOAD_EN.word
#define Set_IPP_IPP_SD_STATIC_MEM_LOAD_EN_word(x) (p_IPP_IP->IPP_SD_STATIC_MEM_LOAD_EN.word = x)


//IPP_SD_STATIC_LOAD_FORMAT


#define Get_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_FORMAT() p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_FORMAT
#define Is_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_FORMAT_B_0x0() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_FORMAT == STATIC_LOAD_FORMAT_B_0x0)
#define Set_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_FORMAT__B_0x0() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_FORMAT = STATIC_LOAD_FORMAT_B_0x0)
#define STATIC_LOAD_FORMAT_B_0x0 0x0    //RAW12
#define Is_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_FORMAT_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_FORMAT == STATIC_LOAD_FORMAT_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_FORMAT__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_FORMAT = STATIC_LOAD_FORMAT_B_0x1)
#define STATIC_LOAD_FORMAT_B_0x1 0x1    //RAW8
#define Is_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_FORMAT_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_FORMAT == STATIC_LOAD_FORMAT_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_FORMAT__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_FORMAT = STATIC_LOAD_FORMAT_B_0x2)
#define STATIC_LOAD_FORMAT_B_0x2 0x2    //R10G10B10
#define Is_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_FORMAT_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_FORMAT == STATIC_LOAD_FORMAT_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_FORMAT__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_FORMAT = STATIC_LOAD_FORMAT_B_0x3)
#define STATIC_LOAD_FORMAT_B_0x3 0x3    //R8G8B8


#define Get_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_RAW_SEL() p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_RAW_SEL
#define Is_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_RAW_SEL_B_0x0() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_RAW_SEL == STATIC_LOAD_RAW_SEL_B_0x0)
#define Set_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_RAW_SEL__B_0x0() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_RAW_SEL = STATIC_LOAD_RAW_SEL_B_0x0)
#define STATIC_LOAD_RAW_SEL_B_0x0 0x0    //Bayer load data injected at the entry point of the ISP Pictor Bayer domain
#define Is_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_RAW_SEL_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_RAW_SEL == STATIC_LOAD_RAW_SEL_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_FORMAT_STATIC_LOAD_RAW_SEL__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.IPP_SD_STATIC_LOAD_FORMAT_ts.STATIC_LOAD_RAW_SEL = STATIC_LOAD_RAW_SEL_B_0x1)
#define STATIC_LOAD_RAW_SEL_B_0x1 0x1    //Bayer load data injected at the end point of the ISP Pictor Bayer domain (before demosaic)
#define Set_IPP_IPP_SD_STATIC_LOAD_FORMAT(STATIC_LOAD_FORMAT,STATIC_LOAD_RAW_SEL) (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.word = (uint16_t)STATIC_LOAD_FORMAT<<0 | (uint16_t)STATIC_LOAD_RAW_SEL<<2)
#define Get_IPP_IPP_SD_STATIC_LOAD_FORMAT() p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.word
#define Set_IPP_IPP_SD_STATIC_LOAD_FORMAT_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_FORMAT.word = x)


//IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN


#define Get_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_STATIC_INTERLINE_TIME_EN() p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN.IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_ts.STATIC_INTERLINE_TIME_EN
#define Set_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_STATIC_INTERLINE_TIME_EN(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN.IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_ts.STATIC_INTERLINE_TIME_EN = x)


#define Get_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_STATIC_INTERLINE_TIME_LSB() p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN.IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_ts.STATIC_INTERLINE_TIME_LSB
#define Set_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_STATIC_INTERLINE_TIME_LSB(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN.IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_ts.STATIC_INTERLINE_TIME_LSB = x)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN(STATIC_INTERLINE_TIME_EN,STATIC_INTERLINE_TIME_LSB) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN.word = (uint16_t)STATIC_INTERLINE_TIME_EN<<0 | (uint16_t)STATIC_INTERLINE_TIME_LSB<<1)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN() p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_LSB_EN.word = x)


//IPP_SD_STATIC_LOAD_LINE_TIME_MSB


#define Get_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_MSB_STATIC_INTERLINE_TIME_MSB() p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_MSB.IPP_SD_STATIC_LOAD_LINE_TIME_MSB_ts.STATIC_INTERLINE_TIME_MSB
#define Set_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_MSB_STATIC_INTERLINE_TIME_MSB(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_MSB.IPP_SD_STATIC_LOAD_LINE_TIME_MSB_ts.STATIC_INTERLINE_TIME_MSB = x)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_MSB(STATIC_INTERLINE_TIME_MSB) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_MSB.word = (uint16_t)STATIC_INTERLINE_TIME_MSB<<0)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_MSB() p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_MSB.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINE_TIME_MSB_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINE_TIME_MSB.word = x)


//IPP_SD_STATIC_LOAD_WW


#define Get_IPP_IPP_SD_STATIC_LOAD_WW_STATIC_LOAD_WW() p_IPP_IP->IPP_SD_STATIC_LOAD_WW.IPP_SD_STATIC_LOAD_WW_ts.STATIC_LOAD_WW
#define Set_IPP_IPP_SD_STATIC_LOAD_WW_STATIC_LOAD_WW(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_WW.IPP_SD_STATIC_LOAD_WW_ts.STATIC_LOAD_WW = x)
#define Set_IPP_IPP_SD_STATIC_LOAD_WW(STATIC_LOAD_WW) (p_IPP_IP->IPP_SD_STATIC_LOAD_WW.word = (uint16_t)STATIC_LOAD_WW<<0)
#define Get_IPP_IPP_SD_STATIC_LOAD_WW() p_IPP_IP->IPP_SD_STATIC_LOAD_WW.word
#define Set_IPP_IPP_SD_STATIC_LOAD_WW_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_WW.word = x)


//IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER


#define Get_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_STATIC_LOAD_BML_PIXEL_ORDER() p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts.STATIC_LOAD_BML_PIXEL_ORDER
#define Is_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_STATIC_LOAD_BML_PIXEL_ORDER_B_0x0() (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts.STATIC_LOAD_BML_PIXEL_ORDER == STATIC_LOAD_BML_PIXEL_ORDER_B_0x0)
#define Set_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_STATIC_LOAD_BML_PIXEL_ORDER__B_0x0() (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts.STATIC_LOAD_BML_PIXEL_ORDER = STATIC_LOAD_BML_PIXEL_ORDER_B_0x0)
#define STATIC_LOAD_BML_PIXEL_ORDER_B_0x0 0x0    //GR
#define Is_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_STATIC_LOAD_BML_PIXEL_ORDER_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts.STATIC_LOAD_BML_PIXEL_ORDER == STATIC_LOAD_BML_PIXEL_ORDER_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_STATIC_LOAD_BML_PIXEL_ORDER__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts.STATIC_LOAD_BML_PIXEL_ORDER = STATIC_LOAD_BML_PIXEL_ORDER_B_0x1)
#define STATIC_LOAD_BML_PIXEL_ORDER_B_0x1 0x1    //RG
#define Is_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_STATIC_LOAD_BML_PIXEL_ORDER_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts.STATIC_LOAD_BML_PIXEL_ORDER == STATIC_LOAD_BML_PIXEL_ORDER_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_STATIC_LOAD_BML_PIXEL_ORDER__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts.STATIC_LOAD_BML_PIXEL_ORDER = STATIC_LOAD_BML_PIXEL_ORDER_B_0x2)
#define STATIC_LOAD_BML_PIXEL_ORDER_B_0x2 0x2    //BG
#define Is_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_STATIC_LOAD_BML_PIXEL_ORDER_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts.STATIC_LOAD_BML_PIXEL_ORDER == STATIC_LOAD_BML_PIXEL_ORDER_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_STATIC_LOAD_BML_PIXEL_ORDER__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_ts.STATIC_LOAD_BML_PIXEL_ORDER = STATIC_LOAD_BML_PIXEL_ORDER_B_0x3)
#define STATIC_LOAD_BML_PIXEL_ORDER_B_0x3 0x3    //GB
#define Set_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER(STATIC_LOAD_BML_PIXEL_ORDER) (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.word = (uint16_t)STATIC_LOAD_BML_PIXEL_ORDER<<0)
#define Get_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER() p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.word
#define Set_IPP_IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_BML_PIXEL_ORDER.word = x)


//IPP_SD_STATIC_LOAD_LINETYPE0


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_HEIGHT0() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_HEIGHT0
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_HEIGHT0(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_HEIGHT0 = x)


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_ID0() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_ID0
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_ID0_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_ID0 == STATIC_LOAD_LINETYPE_ID0_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_ID0__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_ID0 = STATIC_LOAD_LINETYPE_ID0_B_0x1)
#define STATIC_LOAD_LINETYPE_ID0_B_0x1 0x1    //SOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_ID0_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_ID0 == STATIC_LOAD_LINETYPE_ID0_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_ID0__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_ID0 = STATIC_LOAD_LINETYPE_ID0_B_0x2)
#define STATIC_LOAD_LINETYPE_ID0_B_0x2 0x2    //EOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_ID0_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_ID0 == STATIC_LOAD_LINETYPE_ID0_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_ID0__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_ID0 = STATIC_LOAD_LINETYPE_ID0_B_0x3)
#define STATIC_LOAD_LINETYPE_ID0_B_0x3 0x3    //BLANK 01xx: BLACK
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_ID0_B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_ID0 == STATIC_LOAD_LINETYPE_ID0_B_0xE)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_STATIC_LOAD_LINETYPE_ID0__B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.IPP_SD_STATIC_LOAD_LINETYPE0_ts.STATIC_LOAD_LINETYPE_ID0 = STATIC_LOAD_LINETYPE_ID0_B_0xE)
#define STATIC_LOAD_LINETYPE_ID0_B_0xE 0xE    //ACTIVE_LAST
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE0(STATIC_LOAD_LINETYPE_HEIGHT0,STATIC_LOAD_LINETYPE_ID0) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.word = (uint16_t)STATIC_LOAD_LINETYPE_HEIGHT0<<0 | (uint16_t)STATIC_LOAD_LINETYPE_ID0<<12)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE0() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE0_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE0.word = x)


//IPP_SD_STATIC_LOAD_LINETYPE1


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_HEIGHT1() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_HEIGHT1
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_HEIGHT1(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_HEIGHT1 = x)


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_ID1() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_ID1
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_ID1_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_ID1 == STATIC_LOAD_LINETYPE_ID1_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_ID1__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_ID1 = STATIC_LOAD_LINETYPE_ID1_B_0x1)
#define STATIC_LOAD_LINETYPE_ID1_B_0x1 0x1    //SOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_ID1_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_ID1 == STATIC_LOAD_LINETYPE_ID1_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_ID1__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_ID1 = STATIC_LOAD_LINETYPE_ID1_B_0x2)
#define STATIC_LOAD_LINETYPE_ID1_B_0x2 0x2    //EOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_ID1_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_ID1 == STATIC_LOAD_LINETYPE_ID1_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_ID1__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_ID1 = STATIC_LOAD_LINETYPE_ID1_B_0x3)
#define STATIC_LOAD_LINETYPE_ID1_B_0x3 0x3    //BLANK 01xx: BLACK
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_ID1_B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_ID1 == STATIC_LOAD_LINETYPE_ID1_B_0xE)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_STATIC_LOAD_LINETYPE_ID1__B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.IPP_SD_STATIC_LOAD_LINETYPE1_ts.STATIC_LOAD_LINETYPE_ID1 = STATIC_LOAD_LINETYPE_ID1_B_0xE)
#define STATIC_LOAD_LINETYPE_ID1_B_0xE 0xE    //ACTIVE_LAST
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE1(STATIC_LOAD_LINETYPE_HEIGHT1,STATIC_LOAD_LINETYPE_ID1) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.word = (uint16_t)STATIC_LOAD_LINETYPE_HEIGHT1<<0 | (uint16_t)STATIC_LOAD_LINETYPE_ID1<<12)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE1() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE1_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE1.word = x)


//IPP_SD_STATIC_LOAD_LINETYPE2


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_HEIGHT2() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_HEIGHT2
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_HEIGHT2(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_HEIGHT2 = x)


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_ID2() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_ID2
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_ID2_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_ID2 == STATIC_LOAD_LINETYPE_ID2_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_ID2__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_ID2 = STATIC_LOAD_LINETYPE_ID2_B_0x1)
#define STATIC_LOAD_LINETYPE_ID2_B_0x1 0x1    //SOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_ID2_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_ID2 == STATIC_LOAD_LINETYPE_ID2_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_ID2__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_ID2 = STATIC_LOAD_LINETYPE_ID2_B_0x2)
#define STATIC_LOAD_LINETYPE_ID2_B_0x2 0x2    //EOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_ID2_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_ID2 == STATIC_LOAD_LINETYPE_ID2_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_ID2__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_ID2 = STATIC_LOAD_LINETYPE_ID2_B_0x3)
#define STATIC_LOAD_LINETYPE_ID2_B_0x3 0x3    //BLANK 01xx: BLACK
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_ID2_B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_ID2 == STATIC_LOAD_LINETYPE_ID2_B_0xE)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_STATIC_LOAD_LINETYPE_ID2__B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.IPP_SD_STATIC_LOAD_LINETYPE2_ts.STATIC_LOAD_LINETYPE_ID2 = STATIC_LOAD_LINETYPE_ID2_B_0xE)
#define STATIC_LOAD_LINETYPE_ID2_B_0xE 0xE    //ACTIVE_LAST
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE2(STATIC_LOAD_LINETYPE_HEIGHT2,STATIC_LOAD_LINETYPE_ID2) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.word = (uint16_t)STATIC_LOAD_LINETYPE_HEIGHT2<<0 | (uint16_t)STATIC_LOAD_LINETYPE_ID2<<12)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE2() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE2_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE2.word = x)


//IPP_SD_STATIC_LOAD_LINETYPE3


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_HEIGHT3() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_HEIGHT3
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_HEIGHT3(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_HEIGHT3 = x)


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_ID3() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_ID3
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_ID3_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_ID3 == STATIC_LOAD_LINETYPE_ID3_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_ID3__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_ID3 = STATIC_LOAD_LINETYPE_ID3_B_0x1)
#define STATIC_LOAD_LINETYPE_ID3_B_0x1 0x1    //SOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_ID3_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_ID3 == STATIC_LOAD_LINETYPE_ID3_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_ID3__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_ID3 = STATIC_LOAD_LINETYPE_ID3_B_0x2)
#define STATIC_LOAD_LINETYPE_ID3_B_0x2 0x2    //EOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_ID3_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_ID3 == STATIC_LOAD_LINETYPE_ID3_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_ID3__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_ID3 = STATIC_LOAD_LINETYPE_ID3_B_0x3)
#define STATIC_LOAD_LINETYPE_ID3_B_0x3 0x3    //BLANK 01xx: BLACK
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_ID3_B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_ID3 == STATIC_LOAD_LINETYPE_ID3_B_0xE)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_STATIC_LOAD_LINETYPE_ID3__B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.IPP_SD_STATIC_LOAD_LINETYPE3_ts.STATIC_LOAD_LINETYPE_ID3 = STATIC_LOAD_LINETYPE_ID3_B_0xE)
#define STATIC_LOAD_LINETYPE_ID3_B_0xE 0xE    //ACTIVE_LAST
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE3(STATIC_LOAD_LINETYPE_HEIGHT3,STATIC_LOAD_LINETYPE_ID3) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.word = (uint16_t)STATIC_LOAD_LINETYPE_HEIGHT3<<0 | (uint16_t)STATIC_LOAD_LINETYPE_ID3<<12)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE3() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE3_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE3.word = x)


//IPP_SD_STATIC_LOAD_LINETYPE4


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_HEIGHT4() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_HEIGHT4
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_HEIGHT4(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_HEIGHT4 = x)


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_ID4() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_ID4
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_ID4_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_ID4 == STATIC_LOAD_LINETYPE_ID4_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_ID4__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_ID4 = STATIC_LOAD_LINETYPE_ID4_B_0x1)
#define STATIC_LOAD_LINETYPE_ID4_B_0x1 0x1    //SOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_ID4_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_ID4 == STATIC_LOAD_LINETYPE_ID4_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_ID4__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_ID4 = STATIC_LOAD_LINETYPE_ID4_B_0x2)
#define STATIC_LOAD_LINETYPE_ID4_B_0x2 0x2    //EOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_ID4_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_ID4 == STATIC_LOAD_LINETYPE_ID4_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_ID4__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_ID4 = STATIC_LOAD_LINETYPE_ID4_B_0x3)
#define STATIC_LOAD_LINETYPE_ID4_B_0x3 0x3    //BLANK 01xx: BLACK
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_ID4_B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_ID4 == STATIC_LOAD_LINETYPE_ID4_B_0xE)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_STATIC_LOAD_LINETYPE_ID4__B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.IPP_SD_STATIC_LOAD_LINETYPE4_ts.STATIC_LOAD_LINETYPE_ID4 = STATIC_LOAD_LINETYPE_ID4_B_0xE)
#define STATIC_LOAD_LINETYPE_ID4_B_0xE 0xE    //ACTIVE_LAST
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE4(STATIC_LOAD_LINETYPE_HEIGHT4,STATIC_LOAD_LINETYPE_ID4) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.word = (uint16_t)STATIC_LOAD_LINETYPE_HEIGHT4<<0 | (uint16_t)STATIC_LOAD_LINETYPE_ID4<<12)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE4() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE4_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE4.word = x)


//IPP_SD_STATIC_LOAD_LINETYPE5


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_HEIGHT5() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_HEIGHT5
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_HEIGHT5(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_HEIGHT5 = x)


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_ID5() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_ID5
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_ID5_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_ID5 == STATIC_LOAD_LINETYPE_ID5_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_ID5__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_ID5 = STATIC_LOAD_LINETYPE_ID5_B_0x1)
#define STATIC_LOAD_LINETYPE_ID5_B_0x1 0x1    //SOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_ID5_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_ID5 == STATIC_LOAD_LINETYPE_ID5_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_ID5__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_ID5 = STATIC_LOAD_LINETYPE_ID5_B_0x2)
#define STATIC_LOAD_LINETYPE_ID5_B_0x2 0x2    //EOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_ID5_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_ID5 == STATIC_LOAD_LINETYPE_ID5_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_ID5__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_ID5 = STATIC_LOAD_LINETYPE_ID5_B_0x3)
#define STATIC_LOAD_LINETYPE_ID5_B_0x3 0x3    //BLANK 01xx: BLACK
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_ID5_B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_ID5 == STATIC_LOAD_LINETYPE_ID5_B_0xE)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_STATIC_LOAD_LINETYPE_ID5__B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.IPP_SD_STATIC_LOAD_LINETYPE5_ts.STATIC_LOAD_LINETYPE_ID5 = STATIC_LOAD_LINETYPE_ID5_B_0xE)
#define STATIC_LOAD_LINETYPE_ID5_B_0xE 0xE    //ACTIVE_LAST
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE5(STATIC_LOAD_LINETYPE_HEIGHT5,STATIC_LOAD_LINETYPE_ID5) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.word = (uint16_t)STATIC_LOAD_LINETYPE_HEIGHT5<<0 | (uint16_t)STATIC_LOAD_LINETYPE_ID5<<12)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE5() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE5_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE5.word = x)


//IPP_SD_STATIC_LOAD_LINETYPE6


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_HEIGHT6() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_HEIGHT6
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_HEIGHT6(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_HEIGHT6 = x)


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_ID6() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_ID6
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_ID6_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_ID6 == STATIC_LOAD_LINETYPE_ID6_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_ID6__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_ID6 = STATIC_LOAD_LINETYPE_ID6_B_0x1)
#define STATIC_LOAD_LINETYPE_ID6_B_0x1 0x1    //SOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_ID6_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_ID6 == STATIC_LOAD_LINETYPE_ID6_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_ID6__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_ID6 = STATIC_LOAD_LINETYPE_ID6_B_0x2)
#define STATIC_LOAD_LINETYPE_ID6_B_0x2 0x2    //EOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_ID6_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_ID6 == STATIC_LOAD_LINETYPE_ID6_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_ID6__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_ID6 = STATIC_LOAD_LINETYPE_ID6_B_0x3)
#define STATIC_LOAD_LINETYPE_ID6_B_0x3 0x3    //BLANK 01xx: BLACK
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_ID6_B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_ID6 == STATIC_LOAD_LINETYPE_ID6_B_0xE)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_STATIC_LOAD_LINETYPE_ID6__B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.IPP_SD_STATIC_LOAD_LINETYPE6_ts.STATIC_LOAD_LINETYPE_ID6 = STATIC_LOAD_LINETYPE_ID6_B_0xE)
#define STATIC_LOAD_LINETYPE_ID6_B_0xE 0xE    //ACTIVE_LAST
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE6(STATIC_LOAD_LINETYPE_HEIGHT6,STATIC_LOAD_LINETYPE_ID6) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.word = (uint16_t)STATIC_LOAD_LINETYPE_HEIGHT6<<0 | (uint16_t)STATIC_LOAD_LINETYPE_ID6<<12)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE6() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE6_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE6.word = x)


//IPP_SD_STATIC_LOAD_LINETYPE7


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_HEIGHT7() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_HEIGHT7
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_HEIGHT7(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_HEIGHT7 = x)


#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_ID7() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_ID7
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_ID7_B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_ID7 == STATIC_LOAD_LINETYPE_ID7_B_0x1)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_ID7__B_0x1() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_ID7 = STATIC_LOAD_LINETYPE_ID7_B_0x1)
#define STATIC_LOAD_LINETYPE_ID7_B_0x1 0x1    //SOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_ID7_B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_ID7 == STATIC_LOAD_LINETYPE_ID7_B_0x2)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_ID7__B_0x2() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_ID7 = STATIC_LOAD_LINETYPE_ID7_B_0x2)
#define STATIC_LOAD_LINETYPE_ID7_B_0x2 0x2    //EOF
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_ID7_B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_ID7 == STATIC_LOAD_LINETYPE_ID7_B_0x3)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_ID7__B_0x3() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_ID7 = STATIC_LOAD_LINETYPE_ID7_B_0x3)
#define STATIC_LOAD_LINETYPE_ID7_B_0x3 0x3    //BLANK 01xx: BLACK
#define Is_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_ID7_B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_ID7 == STATIC_LOAD_LINETYPE_ID7_B_0xE)
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_STATIC_LOAD_LINETYPE_ID7__B_0xE() (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.IPP_SD_STATIC_LOAD_LINETYPE7_ts.STATIC_LOAD_LINETYPE_ID7 = STATIC_LOAD_LINETYPE_ID7_B_0xE)
#define STATIC_LOAD_LINETYPE_ID7_B_0xE 0xE    //ACTIVE_LAST
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE7(STATIC_LOAD_LINETYPE_HEIGHT7,STATIC_LOAD_LINETYPE_ID7) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.word = (uint16_t)STATIC_LOAD_LINETYPE_HEIGHT7<<0 | (uint16_t)STATIC_LOAD_LINETYPE_ID7<<12)
#define Get_IPP_IPP_SD_STATIC_LOAD_LINETYPE7() p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.word
#define Set_IPP_IPP_SD_STATIC_LOAD_LINETYPE7_word(x) (p_IPP_IP->IPP_SD_STATIC_LOAD_LINETYPE7.word = x)


//IPP_SD_ERROR_ITS


#define Get_IPP_IPP_SD_ERROR_ITS_CCP_SHIFT_SYNC() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.CCP_SHIFT_SYNC


#define Get_IPP_IPP_SD_ERROR_ITS_CCP_FALSE_SYNC() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.CCP_FALSE_SYNC


#define Get_IPP_IPP_SD_ERROR_ITS_CCP_CRC_ERROR() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.CCP_CRC_ERROR


#define Get_IPP_IPP_SD_ERROR_ITS_OPIPE0_CH0_OVERFLOW() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.OPIPE0_CH0_OVERFLOW


#define Get_IPP_IPP_SD_ERROR_ITS_OPIPE0_CH1_OVERFLOW() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.OPIPE0_CH1_OVERFLOW


#define Get_IPP_IPP_SD_ERROR_ITS_OPIPE0_CH2_OVERFLOW() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.OPIPE0_CH2_OVERFLOW


#define Get_IPP_IPP_SD_ERROR_ITS_OPIPE1_OVERFLOW() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.OPIPE1_OVERFLOW


#define Get_IPP_IPP_SD_ERROR_ITS_OPIPE2_OVERFLOW() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.OPIPE2_OVERFLOW


#define Get_IPP_IPP_SD_ERROR_ITS_OPIPE0_MULT_ERR() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.OPIPE0_MULT_ERR


#define Get_IPP_IPP_SD_ERROR_ITS_OPIPE1_MULT_ERR() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.OPIPE1_MULT_ERR


#define Get_IPP_IPP_SD_ERROR_ITS_SD_CSI2_PACKET_ERROR() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.SD_CSI2_PACKET_ERROR


#define Get_IPP_IPP_SD_ERROR_ITS_SD_CSI2_DPHY_ERROR() p_IPP_IP->IPP_SD_ERROR_ITS.IPP_SD_ERROR_ITS_ts.SD_CSI2_DPHY_ERROR
#define Get_IPP_IPP_SD_ERROR_ITS() p_IPP_IP->IPP_SD_ERROR_ITS.word


//IPP_SD_ERROR_ITS_BCLR


#define Set_IPP_IPP_SD_ERROR_ITS_BCLR_SD_ERROR_ITS_BCLR(x) (p_IPP_IP->IPP_SD_ERROR_ITS_BCLR.IPP_SD_ERROR_ITS_BCLR_ts.SD_ERROR_ITS_BCLR = x)
#define Set_IPP_IPP_SD_ERROR_ITS_BCLR(SD_ERROR_ITS_BCLR) (p_IPP_IP->IPP_SD_ERROR_ITS_BCLR.word = (uint16_t)SD_ERROR_ITS_BCLR<<0)
#define Set_IPP_IPP_SD_ERROR_ITS_BCLR_word(x) (p_IPP_IP->IPP_SD_ERROR_ITS_BCLR.word = x)


//IPP_SD_ERROR_ITS_BSET


#define Set_IPP_IPP_SD_ERROR_ITS_BSET_SD_ERROR_ITS_BSET(x) (p_IPP_IP->IPP_SD_ERROR_ITS_BSET.IPP_SD_ERROR_ITS_BSET_ts.SD_ERROR_ITS_BSET = x)
#define Set_IPP_IPP_SD_ERROR_ITS_BSET(SD_ERROR_ITS_BSET) (p_IPP_IP->IPP_SD_ERROR_ITS_BSET.word = (uint16_t)SD_ERROR_ITS_BSET<<0)
#define Set_IPP_IPP_SD_ERROR_ITS_BSET_word(x) (p_IPP_IP->IPP_SD_ERROR_ITS_BSET.word = x)


//IPP_SD_ERROR_ITM


#define Get_IPP_IPP_SD_ERROR_ITM_SD_ERROR_ITM() p_IPP_IP->IPP_SD_ERROR_ITM.IPP_SD_ERROR_ITM_ts.SD_ERROR_ITM
#define Get_IPP_IPP_SD_ERROR_ITM() p_IPP_IP->IPP_SD_ERROR_ITM.word


//IPP_SD_ERROR_ITM_BCLR


#define Set_IPP_IPP_SD_ERROR_ITM_BCLR_SD_ERROR_ITM_BCLR(x) (p_IPP_IP->IPP_SD_ERROR_ITM_BCLR.IPP_SD_ERROR_ITM_BCLR_ts.SD_ERROR_ITM_BCLR = x)
#define Set_IPP_IPP_SD_ERROR_ITM_BCLR(SD_ERROR_ITM_BCLR) (p_IPP_IP->IPP_SD_ERROR_ITM_BCLR.word = (uint16_t)SD_ERROR_ITM_BCLR<<0)
#define Set_IPP_IPP_SD_ERROR_ITM_BCLR_word(x) (p_IPP_IP->IPP_SD_ERROR_ITM_BCLR.word = x)


//IPP_SD_ERROR_ITM_BSET


#define Set_IPP_IPP_SD_ERROR_ITM_BSET_SD_ERROR_ITM_BSET(x) (p_IPP_IP->IPP_SD_ERROR_ITM_BSET.IPP_SD_ERROR_ITM_BSET_ts.SD_ERROR_ITM_BSET = x)
#define Set_IPP_IPP_SD_ERROR_ITM_BSET(SD_ERROR_ITM_BSET) (p_IPP_IP->IPP_SD_ERROR_ITM_BSET.word = (uint16_t)SD_ERROR_ITM_BSET<<0)
#define Set_IPP_IPP_SD_ERROR_ITM_BSET_word(x) (p_IPP_IP->IPP_SD_ERROR_ITM_BSET.word = x)


//IPP_SD_OPIPE0_MULT_ERR_DBG


#define Get_IPP_IPP_SD_OPIPE0_MULT_ERR_DBG_OPIPE0_CH0_MULT_ERR() p_IPP_IP->IPP_SD_OPIPE0_MULT_ERR_DBG.IPP_SD_OPIPE0_MULT_ERR_DBG_ts.OPIPE0_CH0_MULT_ERR


#define Get_IPP_IPP_SD_OPIPE0_MULT_ERR_DBG_OPIPE0_CH1_MULT_ERR() p_IPP_IP->IPP_SD_OPIPE0_MULT_ERR_DBG.IPP_SD_OPIPE0_MULT_ERR_DBG_ts.OPIPE0_CH1_MULT_ERR


#define Get_IPP_IPP_SD_OPIPE0_MULT_ERR_DBG_OPIPE0_CH2_MULT_ERR() p_IPP_IP->IPP_SD_OPIPE0_MULT_ERR_DBG.IPP_SD_OPIPE0_MULT_ERR_DBG_ts.OPIPE0_CH2_MULT_ERR
#define Get_IPP_IPP_SD_OPIPE0_MULT_ERR_DBG() p_IPP_IP->IPP_SD_OPIPE0_MULT_ERR_DBG.word


//IPP_CD_RESET
#define Set_IPP_IPP_CD_RESET_word(x) (p_IPP_IP->IPP_CD_RESET.word = x)


//IPP_CD_START
#define Set_IPP_IPP_CD_START_word(x) (p_IPP_IP->IPP_CD_START.word = x)


//IPP_CD_STATIC_EN


#define Get_IPP_IPP_CD_STATIC_EN_STATIC_CD_EN() p_IPP_IP->IPP_CD_STATIC_EN.IPP_CD_STATIC_EN_ts.STATIC_CD_EN
#define Is_IPP_IPP_CD_STATIC_EN_STATIC_CD_EN_B_0x0() (p_IPP_IP->IPP_CD_STATIC_EN.IPP_CD_STATIC_EN_ts.STATIC_CD_EN == STATIC_CD_EN_B_0x0)
#define Set_IPP_IPP_CD_STATIC_EN_STATIC_CD_EN__B_0x0() (p_IPP_IP->IPP_CD_STATIC_EN.IPP_CD_STATIC_EN_ts.STATIC_CD_EN = STATIC_CD_EN_B_0x0)
#define STATIC_CD_EN_B_0x0 0x0    //disabled
#define Is_IPP_IPP_CD_STATIC_EN_STATIC_CD_EN_B_0x1() (p_IPP_IP->IPP_CD_STATIC_EN.IPP_CD_STATIC_EN_ts.STATIC_CD_EN == STATIC_CD_EN_B_0x1)
#define Set_IPP_IPP_CD_STATIC_EN_STATIC_CD_EN__B_0x1() (p_IPP_IP->IPP_CD_STATIC_EN.IPP_CD_STATIC_EN_ts.STATIC_CD_EN = STATIC_CD_EN_B_0x1)
#define STATIC_CD_EN_B_0x1 0x1    //active)
#define Set_IPP_IPP_CD_STATIC_EN(STATIC_CD_EN) (p_IPP_IP->IPP_CD_STATIC_EN.word = (uint16_t)STATIC_CD_EN<<0)
#define Get_IPP_IPP_CD_STATIC_EN() p_IPP_IP->IPP_CD_STATIC_EN.word
#define Set_IPP_IPP_CD_STATIC_EN_word(x) (p_IPP_IP->IPP_CD_STATIC_EN.word = x)


//IPP_CD_STATIC_CDG_CSI_PADD_EN


#define Get_IPP_IPP_CD_STATIC_CDG_CSI_PADD_EN_STATIC_CDG_CSI_PADD_EN() p_IPP_IP->IPP_CD_STATIC_CDG_CSI_PADD_EN.IPP_CD_STATIC_CDG_CSI_PADD_EN_ts.STATIC_CDG_CSI_PADD_EN
#define Is_IPP_IPP_CD_STATIC_CDG_CSI_PADD_EN_STATIC_CDG_CSI_PADD_EN_B_0x0() (p_IPP_IP->IPP_CD_STATIC_CDG_CSI_PADD_EN.IPP_CD_STATIC_CDG_CSI_PADD_EN_ts.STATIC_CDG_CSI_PADD_EN == STATIC_CDG_CSI_PADD_EN_B_0x0)
#define Set_IPP_IPP_CD_STATIC_CDG_CSI_PADD_EN_STATIC_CDG_CSI_PADD_EN__B_0x0() (p_IPP_IP->IPP_CD_STATIC_CDG_CSI_PADD_EN.IPP_CD_STATIC_CDG_CSI_PADD_EN_ts.STATIC_CDG_CSI_PADD_EN = STATIC_CDG_CSI_PADD_EN_B_0x0)
#define STATIC_CDG_CSI_PADD_EN_B_0x0 0x0    //disabled
#define Is_IPP_IPP_CD_STATIC_CDG_CSI_PADD_EN_STATIC_CDG_CSI_PADD_EN_B_0x1() (p_IPP_IP->IPP_CD_STATIC_CDG_CSI_PADD_EN.IPP_CD_STATIC_CDG_CSI_PADD_EN_ts.STATIC_CDG_CSI_PADD_EN == STATIC_CDG_CSI_PADD_EN_B_0x1)
#define Set_IPP_IPP_CD_STATIC_CDG_CSI_PADD_EN_STATIC_CDG_CSI_PADD_EN__B_0x1() (p_IPP_IP->IPP_CD_STATIC_CDG_CSI_PADD_EN.IPP_CD_STATIC_CDG_CSI_PADD_EN_ts.STATIC_CDG_CSI_PADD_EN = STATIC_CDG_CSI_PADD_EN_B_0x1)
#define STATIC_CDG_CSI_PADD_EN_B_0x1 0x1    //active
#define Set_IPP_IPP_CD_STATIC_CDG_CSI_PADD_EN(STATIC_CDG_CSI_PADD_EN) (p_IPP_IP->IPP_CD_STATIC_CDG_CSI_PADD_EN.word = (uint16_t)STATIC_CDG_CSI_PADD_EN<<0)
#define Get_IPP_IPP_CD_STATIC_CDG_CSI_PADD_EN() p_IPP_IP->IPP_CD_STATIC_CDG_CSI_PADD_EN.word
#define Set_IPP_IPP_CD_STATIC_CDG_CSI_PADD_EN_word(x) (p_IPP_IP->IPP_CD_STATIC_CDG_CSI_PADD_EN.word = x)


//IPP_CD_STATIC_CCP_FC


#define Get_IPP_IPP_CD_STATIC_CCP_FC_STATIC_CCP_FS() p_IPP_IP->IPP_CD_STATIC_CCP_FC.IPP_CD_STATIC_CCP_FC_ts.STATIC_CCP_FS
#define Set_IPP_IPP_CD_STATIC_CCP_FC_STATIC_CCP_FS(x) (p_IPP_IP->IPP_CD_STATIC_CCP_FC.IPP_CD_STATIC_CCP_FC_ts.STATIC_CCP_FS = x)


#define Get_IPP_IPP_CD_STATIC_CCP_FC_STATIC_CCP_FE() p_IPP_IP->IPP_CD_STATIC_CCP_FC.IPP_CD_STATIC_CCP_FC_ts.STATIC_CCP_FE
#define Set_IPP_IPP_CD_STATIC_CCP_FC_STATIC_CCP_FE(x) (p_IPP_IP->IPP_CD_STATIC_CCP_FC.IPP_CD_STATIC_CCP_FC_ts.STATIC_CCP_FE = x)
#define Set_IPP_IPP_CD_STATIC_CCP_FC(STATIC_CCP_FS,STATIC_CCP_FE) (p_IPP_IP->IPP_CD_STATIC_CCP_FC.word = (uint16_t)STATIC_CCP_FS<<0 | (uint16_t)STATIC_CCP_FE<<8)
#define Get_IPP_IPP_CD_STATIC_CCP_FC() p_IPP_IP->IPP_CD_STATIC_CCP_FC.word
#define Set_IPP_IPP_CD_STATIC_CCP_FC_word(x) (p_IPP_IP->IPP_CD_STATIC_CCP_FC.word = x)


//IPP_CD_STATIC_CCP_LC


#define Get_IPP_IPP_CD_STATIC_CCP_LC_STATIC_CCP_LS() p_IPP_IP->IPP_CD_STATIC_CCP_LC.IPP_CD_STATIC_CCP_LC_ts.STATIC_CCP_LS
#define Set_IPP_IPP_CD_STATIC_CCP_LC_STATIC_CCP_LS(x) (p_IPP_IP->IPP_CD_STATIC_CCP_LC.IPP_CD_STATIC_CCP_LC_ts.STATIC_CCP_LS = x)


#define Get_IPP_IPP_CD_STATIC_CCP_LC_STATIC_CCP_LE() p_IPP_IP->IPP_CD_STATIC_CCP_LC.IPP_CD_STATIC_CCP_LC_ts.STATIC_CCP_LE
#define Set_IPP_IPP_CD_STATIC_CCP_LC_STATIC_CCP_LE(x) (p_IPP_IP->IPP_CD_STATIC_CCP_LC.IPP_CD_STATIC_CCP_LC_ts.STATIC_CCP_LE = x)
#define Set_IPP_IPP_CD_STATIC_CCP_LC(STATIC_CCP_LS,STATIC_CCP_LE) (p_IPP_IP->IPP_CD_STATIC_CCP_LC.word = (uint16_t)STATIC_CCP_LS<<0 | (uint16_t)STATIC_CCP_LE<<8)
#define Get_IPP_IPP_CD_STATIC_CCP_LC() p_IPP_IP->IPP_CD_STATIC_CCP_LC.word
#define Set_IPP_IPP_CD_STATIC_CCP_LC_word(x) (p_IPP_IP->IPP_CD_STATIC_CCP_LC.word = x)


//IPP_CD_RAW


#define Get_IPP_IPP_CD_RAW_JPEG_PADD_EN() p_IPP_IP->IPP_CD_RAW.IPP_CD_RAW_ts.JPEG_PADD_EN
#define Set_IPP_IPP_CD_RAW_JPEG_PADD_EN(x) (p_IPP_IP->IPP_CD_RAW.IPP_CD_RAW_ts.JPEG_PADD_EN = x)


#define Get_IPP_IPP_CD_RAW_CCP_RAW_SWAP() p_IPP_IP->IPP_CD_RAW.IPP_CD_RAW_ts.CCP_RAW_SWAP
#define Is_IPP_IPP_CD_RAW_CCP_RAW_SWAP_B_0x2() (p_IPP_IP->IPP_CD_RAW.IPP_CD_RAW_ts.CCP_RAW_SWAP == CCP_RAW_SWAP_B_0x2)
#define Set_IPP_IPP_CD_RAW_CCP_RAW_SWAP__B_0x2() (p_IPP_IP->IPP_CD_RAW.IPP_CD_RAW_ts.CCP_RAW_SWAP = CCP_RAW_SWAP_B_0x2)
#define CCP_RAW_SWAP_B_0x2 0x2    //rgb565
#define Is_IPP_IPP_CD_RAW_CCP_RAW_SWAP_B_0x3() (p_IPP_IP->IPP_CD_RAW.IPP_CD_RAW_ts.CCP_RAW_SWAP == CCP_RAW_SWAP_B_0x3)
#define Set_IPP_IPP_CD_RAW_CCP_RAW_SWAP__B_0x3() (p_IPP_IP->IPP_CD_RAW.IPP_CD_RAW_ts.CCP_RAW_SWAP = CCP_RAW_SWAP_B_0x3)
#define CCP_RAW_SWAP_B_0x3 0x3    //rgb444
#define Set_IPP_IPP_CD_RAW(JPEG_PADD_EN,CCP_RAW_SWAP) (p_IPP_IP->IPP_CD_RAW.word = (uint16_t)JPEG_PADD_EN<<0 | (uint16_t)CCP_RAW_SWAP<<1)
#define Get_IPP_IPP_CD_RAW() p_IPP_IP->IPP_CD_RAW.word
#define Set_IPP_IPP_CD_RAW_word(x) (p_IPP_IP->IPP_CD_RAW.word = x)


//IPP_CD_PIPE3_FORMAT


#define Get_IPP_IPP_CD_PIPE3_FORMAT_PIPE3_FORMAT() p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.PIPE3_FORMAT
#define Is_IPP_IPP_CD_PIPE3_FORMAT_PIPE3_FORMAT_B_0x0() (p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.PIPE3_FORMAT == PIPE3_FORMAT_B_0x0)
#define Set_IPP_IPP_CD_PIPE3_FORMAT_PIPE3_FORMAT__B_0x0() (p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.PIPE3_FORMAT = PIPE3_FORMAT_B_0x0)
#define PIPE3_FORMAT_B_0x0 0x0    //YUV 4:2:0 2buffers
#define Is_IPP_IPP_CD_PIPE3_FORMAT_PIPE3_FORMAT_B_0x1() (p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.PIPE3_FORMAT == PIPE3_FORMAT_B_0x1)
#define Set_IPP_IPP_CD_PIPE3_FORMAT_PIPE3_FORMAT__B_0x1() (p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.PIPE3_FORMAT = PIPE3_FORMAT_B_0x1)
#define PIPE3_FORMAT_B_0x1 0x1    //YUV 4:2:2 2buffers
#define Is_IPP_IPP_CD_PIPE3_FORMAT_PIPE3_FORMAT_B_0x2() (p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.PIPE3_FORMAT == PIPE3_FORMAT_B_0x2)
#define Set_IPP_IPP_CD_PIPE3_FORMAT_PIPE3_FORMAT__B_0x2() (p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.PIPE3_FORMAT = PIPE3_FORMAT_B_0x2)
#define PIPE3_FORMAT_B_0x2 0x2    //YUV 4:2:0 3buffers
#define Is_IPP_IPP_CD_PIPE3_FORMAT_PIPE3_FORMAT_B_0x4() (p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.PIPE3_FORMAT == PIPE3_FORMAT_B_0x4)
#define Set_IPP_IPP_CD_PIPE3_FORMAT_PIPE3_FORMAT__B_0x4() (p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.PIPE3_FORMAT = PIPE3_FORMAT_B_0x4)
#define PIPE3_FORMAT_B_0x4 0x4    //raw data 1 buffer


#define Get_IPP_IPP_CD_PIPE3_FORMAT_BT601_EN() p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.BT601_EN
#define Set_IPP_IPP_CD_PIPE3_FORMAT_BT601_EN(x) (p_IPP_IP->IPP_CD_PIPE3_FORMAT.IPP_CD_PIPE3_FORMAT_ts.BT601_EN = x)
#define Set_IPP_IPP_CD_PIPE3_FORMAT(PIPE3_FORMAT,BT601_EN) (p_IPP_IP->IPP_CD_PIPE3_FORMAT.word = (uint16_t)PIPE3_FORMAT<<0 | (uint16_t)BT601_EN<<3)
#define Get_IPP_IPP_CD_PIPE3_FORMAT() p_IPP_IP->IPP_CD_PIPE3_FORMAT.word
#define Set_IPP_IPP_CD_PIPE3_FORMAT_word(x) (p_IPP_IP->IPP_CD_PIPE3_FORMAT.word = x)


//IPP_CD_CROP_EN


#define Get_IPP_IPP_CD_CROP_EN_CROP_EN() p_IPP_IP->IPP_CD_CROP_EN.IPP_CD_CROP_EN_ts.CROP_EN
#define Set_IPP_IPP_CD_CROP_EN_CROP_EN(x) (p_IPP_IP->IPP_CD_CROP_EN.IPP_CD_CROP_EN_ts.CROP_EN = x)
#define Set_IPP_IPP_CD_CROP_EN(CROP_EN) (p_IPP_IP->IPP_CD_CROP_EN.word = (uint16_t)CROP_EN<<0)
#define Get_IPP_IPP_CD_CROP_EN() p_IPP_IP->IPP_CD_CROP_EN.word
#define Set_IPP_IPP_CD_CROP_EN_word(x) (p_IPP_IP->IPP_CD_CROP_EN.word = x)


//IPP_CD_CROP_H0


#define Get_IPP_IPP_CD_CROP_H0_CROP_H0() p_IPP_IP->IPP_CD_CROP_H0.IPP_CD_CROP_H0_ts.CROP_H0
#define Set_IPP_IPP_CD_CROP_H0_CROP_H0(x) (p_IPP_IP->IPP_CD_CROP_H0.IPP_CD_CROP_H0_ts.CROP_H0 = x)
#define Set_IPP_IPP_CD_CROP_H0(CROP_H0) (p_IPP_IP->IPP_CD_CROP_H0.word = (uint16_t)CROP_H0<<1)
#define Get_IPP_IPP_CD_CROP_H0() p_IPP_IP->IPP_CD_CROP_H0.word
#define Set_IPP_IPP_CD_CROP_H0_word(x) (p_IPP_IP->IPP_CD_CROP_H0.word = x)


//IPP_CD_CROP_V0


#define Get_IPP_IPP_CD_CROP_V0_CROP_V0() p_IPP_IP->IPP_CD_CROP_V0.IPP_CD_CROP_V0_ts.CROP_V0
#define Set_IPP_IPP_CD_CROP_V0_CROP_V0(x) (p_IPP_IP->IPP_CD_CROP_V0.IPP_CD_CROP_V0_ts.CROP_V0 = x)
#define Set_IPP_IPP_CD_CROP_V0(CROP_V0) (p_IPP_IP->IPP_CD_CROP_V0.word = (uint16_t)CROP_V0<<0)
#define Get_IPP_IPP_CD_CROP_V0() p_IPP_IP->IPP_CD_CROP_V0.word
#define Set_IPP_IPP_CD_CROP_V0_word(x) (p_IPP_IP->IPP_CD_CROP_V0.word = x)


//IPP_CD_CROP_H1


#define Get_IPP_IPP_CD_CROP_H1_CROP_H1() p_IPP_IP->IPP_CD_CROP_H1.IPP_CD_CROP_H1_ts.CROP_H1
#define Set_IPP_IPP_CD_CROP_H1_CROP_H1(x) (p_IPP_IP->IPP_CD_CROP_H1.IPP_CD_CROP_H1_ts.CROP_H1 = x)
#define Set_IPP_IPP_CD_CROP_H1(CROP_H1) (p_IPP_IP->IPP_CD_CROP_H1.word = (uint16_t)CROP_H1<<1)
#define Get_IPP_IPP_CD_CROP_H1() p_IPP_IP->IPP_CD_CROP_H1.word
#define Set_IPP_IPP_CD_CROP_H1_word(x) (p_IPP_IP->IPP_CD_CROP_H1.word = x)


//IPP_CD_CROP_V1


#define Get_IPP_IPP_CD_CROP_V1_CROP_V1() p_IPP_IP->IPP_CD_CROP_V1.IPP_CD_CROP_V1_ts.CROP_V1
#define Set_IPP_IPP_CD_CROP_V1_CROP_V1(x) (p_IPP_IP->IPP_CD_CROP_V1.IPP_CD_CROP_V1_ts.CROP_V1 = x)
#define Set_IPP_IPP_CD_CROP_V1(CROP_V1) (p_IPP_IP->IPP_CD_CROP_V1.word = (uint16_t)CROP_V1<<0)
#define Get_IPP_IPP_CD_CROP_V1() p_IPP_IP->IPP_CD_CROP_V1.word
#define Set_IPP_IPP_CD_CROP_V1_word(x) (p_IPP_IP->IPP_CD_CROP_V1.word = x)


//IPP_CD_ERROR_ITS


#define Get_IPP_IPP_CD_ERROR_ITS_CCP_ERROR() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.CCP_ERROR


#define Get_IPP_IPP_CD_ERROR_ITS_RAW_DATA_ERROR_NOT_X8_BYTES() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.RAW_DATA_ERROR_NOT_X8_BYTES


#define Get_IPP_IPP_CD_ERROR_ITS_RAW_DATA_ERROR_OVERFLOW() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.RAW_DATA_ERROR_OVERFLOW


#define Get_IPP_IPP_CD_ERROR_ITS_LUMA_ERROR_NOT_X8_LUMA_BYTES() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.LUMA_ERROR_NOT_X8_LUMA_BYTES


#define Get_IPP_IPP_CD_ERROR_ITS_LUMA_ERROR_OVERFLOW() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.LUMA_ERROR_OVERFLOW


#define Get_IPP_IPP_CD_ERROR_ITS_CHROMA_ERROR_NOT_X16_BYTES_3BUF() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.CHROMA_ERROR_NOT_X16_BYTES_3BUF


#define Get_IPP_IPP_CD_ERROR_ITS_CHROMA_ERROR_NOT_X8BYTES_2BUF() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.CHROMA_ERROR_NOT_X8BYTES_2BUF


#define Get_IPP_IPP_CD_ERROR_ITS_CHROMA_ERROR_OVERFLOW() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.CHROMA_ERROR_OVERFLOW


#define Get_IPP_IPP_CD_ERROR_ITS_CD_CSI2_PACKET_ERROR() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.CD_CSI2_PACKET_ERROR


#define Get_IPP_IPP_CD_ERROR_ITS_CD_CSI2_DPHY_ERROR() p_IPP_IP->IPP_CD_ERROR_ITS.IPP_CD_ERROR_ITS_ts.CD_CSI2_DPHY_ERROR
#define Get_IPP_IPP_CD_ERROR_ITS() p_IPP_IP->IPP_CD_ERROR_ITS.word


//IPP_CD_ERROR_ITS_BCLR


#define Set_IPP_IPP_CD_ERROR_ITS_BCLR_CD_ERROR_ITS_BCLR(x) (p_IPP_IP->IPP_CD_ERROR_ITS_BCLR.IPP_CD_ERROR_ITS_BCLR_ts.CD_ERROR_ITS_BCLR = x)
#define Set_IPP_IPP_CD_ERROR_ITS_BCLR(CD_ERROR_ITS_BCLR) (p_IPP_IP->IPP_CD_ERROR_ITS_BCLR.word = (uint16_t)CD_ERROR_ITS_BCLR<<1)
#define Set_IPP_IPP_CD_ERROR_ITS_BCLR_word(x) (p_IPP_IP->IPP_CD_ERROR_ITS_BCLR.word = x)


//IPP_CD_ERROR_ITS_BSET


#define Set_IPP_IPP_CD_ERROR_ITS_BSET_CD_ERROR_ITS_BSET(x) (p_IPP_IP->IPP_CD_ERROR_ITS_BSET.IPP_CD_ERROR_ITS_BSET_ts.CD_ERROR_ITS_BSET = x)
#define Set_IPP_IPP_CD_ERROR_ITS_BSET(CD_ERROR_ITS_BSET) (p_IPP_IP->IPP_CD_ERROR_ITS_BSET.word = (uint16_t)CD_ERROR_ITS_BSET<<1)
#define Set_IPP_IPP_CD_ERROR_ITS_BSET_word(x) (p_IPP_IP->IPP_CD_ERROR_ITS_BSET.word = x)


//IPP_CD_ERROR_ITM


#define Get_IPP_IPP_CD_ERROR_ITM_CD_ERROR_ITM() p_IPP_IP->IPP_CD_ERROR_ITM.IPP_CD_ERROR_ITM_ts.CD_ERROR_ITM
#define Get_IPP_IPP_CD_ERROR_ITM() p_IPP_IP->IPP_CD_ERROR_ITM.word


//IPP_CD_ERROR_ITM_BCLR


#define Set_IPP_IPP_CD_ERROR_ITM_BCLR_CD_ERROR_ITM_BCLR(x) (p_IPP_IP->IPP_CD_ERROR_ITM_BCLR.IPP_CD_ERROR_ITM_BCLR_ts.CD_ERROR_ITM_BCLR = x)
#define Set_IPP_IPP_CD_ERROR_ITM_BCLR(CD_ERROR_ITM_BCLR) (p_IPP_IP->IPP_CD_ERROR_ITM_BCLR.word = (uint16_t)CD_ERROR_ITM_BCLR<<1)
#define Set_IPP_IPP_CD_ERROR_ITM_BCLR_word(x) (p_IPP_IP->IPP_CD_ERROR_ITM_BCLR.word = x)


//IPP_CD_ERROR_ITM_BSET


#define Set_IPP_IPP_CD_ERROR_ITM_BSET_CD_ERROR_ITM_BSET(x) (p_IPP_IP->IPP_CD_ERROR_ITM_BSET.IPP_CD_ERROR_ITM_BSET_ts.CD_ERROR_ITM_BSET = x)
#define Set_IPP_IPP_CD_ERROR_ITM_BSET(CD_ERROR_ITM_BSET) (p_IPP_IP->IPP_CD_ERROR_ITM_BSET.word = (uint16_t)CD_ERROR_ITM_BSET<<1)
#define Set_IPP_IPP_CD_ERROR_ITM_BSET_word(x) (p_IPP_IP->IPP_CD_ERROR_ITM_BSET.word = x)


//IPP_CD_RAW_EOF_ITS


#define Get_IPP_IPP_CD_RAW_EOF_ITS_CD_RAW_EOF_ITS() p_IPP_IP->IPP_CD_RAW_EOF_ITS.IPP_CD_RAW_EOF_ITS_ts.CD_RAW_EOF_ITS
#define Get_IPP_IPP_CD_RAW_EOF_ITS() p_IPP_IP->IPP_CD_RAW_EOF_ITS.word


//IPP_CD_RAW_EOF_ITS_BCLR


#define Set_IPP_IPP_CD_RAW_EOF_ITS_BCLR_CD_RAW_EOF_ITS_BCLR(x) (p_IPP_IP->IPP_CD_RAW_EOF_ITS_BCLR.IPP_CD_RAW_EOF_ITS_BCLR_ts.CD_RAW_EOF_ITS_BCLR = x)
#define Set_IPP_IPP_CD_RAW_EOF_ITS_BCLR(CD_RAW_EOF_ITS_BCLR) (p_IPP_IP->IPP_CD_RAW_EOF_ITS_BCLR.word = (uint16_t)CD_RAW_EOF_ITS_BCLR<<0)
#define Set_IPP_IPP_CD_RAW_EOF_ITS_BCLR_word(x) (p_IPP_IP->IPP_CD_RAW_EOF_ITS_BCLR.word = x)


//IPP_CD_RAW_EOF_ITS_BSET


#define Set_IPP_IPP_CD_RAW_EOF_ITS_BSET_CD_RAW_EOF_ITS_BSET(x) (p_IPP_IP->IPP_CD_RAW_EOF_ITS_BSET.IPP_CD_RAW_EOF_ITS_BSET_ts.CD_RAW_EOF_ITS_BSET = x)
#define Set_IPP_IPP_CD_RAW_EOF_ITS_BSET(CD_RAW_EOF_ITS_BSET) (p_IPP_IP->IPP_CD_RAW_EOF_ITS_BSET.word = (uint16_t)CD_RAW_EOF_ITS_BSET<<0)
#define Set_IPP_IPP_CD_RAW_EOF_ITS_BSET_word(x) (p_IPP_IP->IPP_CD_RAW_EOF_ITS_BSET.word = x)


//IPP_CD_RAW_EOF_ITM


#define Get_IPP_IPP_CD_RAW_EOF_ITM_CD_RAW_EOF_ITM() p_IPP_IP->IPP_CD_RAW_EOF_ITM.IPP_CD_RAW_EOF_ITM_ts.CD_RAW_EOF_ITM
#define Get_IPP_IPP_CD_RAW_EOF_ITM() p_IPP_IP->IPP_CD_RAW_EOF_ITM.word


//IPP_CD_RAW_EOF_ITM_BCLR


#define Set_IPP_IPP_CD_RAW_EOF_ITM_BCLR_CD_RAW_EOF_ITM_BCLR(x) (p_IPP_IP->IPP_CD_RAW_EOF_ITM_BCLR.IPP_CD_RAW_EOF_ITM_BCLR_ts.CD_RAW_EOF_ITM_BCLR = x)
#define Set_IPP_IPP_CD_RAW_EOF_ITM_BCLR(CD_RAW_EOF_ITM_BCLR) (p_IPP_IP->IPP_CD_RAW_EOF_ITM_BCLR.word = (uint16_t)CD_RAW_EOF_ITM_BCLR<<0)
#define Set_IPP_IPP_CD_RAW_EOF_ITM_BCLR_word(x) (p_IPP_IP->IPP_CD_RAW_EOF_ITM_BCLR.word = x)


//IPP_CD_RAW_EOF_ITM_BSET


#define Set_IPP_IPP_CD_RAW_EOF_ITM_BSET_CD_RAW_EOF_ITM_BSET(x) (p_IPP_IP->IPP_CD_RAW_EOF_ITM_BSET.IPP_CD_RAW_EOF_ITM_BSET_ts.CD_RAW_EOF_ITM_BSET = x)
#define Set_IPP_IPP_CD_RAW_EOF_ITM_BSET(CD_RAW_EOF_ITM_BSET) (p_IPP_IP->IPP_CD_RAW_EOF_ITM_BSET.word = (uint16_t)CD_RAW_EOF_ITM_BSET<<0)
#define Set_IPP_IPP_CD_RAW_EOF_ITM_BSET_word(x) (p_IPP_IP->IPP_CD_RAW_EOF_ITM_BSET.word = x)


//IPP_CD_CCP_ERR_DBG_ITS


#define Get_IPP_IPP_CD_CCP_ERR_DBG_ITS_CD_CCP_ERR_DBG() p_IPP_IP->IPP_CD_CCP_ERR_DBG_ITS.IPP_CD_CCP_ERR_DBG_ITS_ts.CD_CCP_ERR_DBG
#define Is_IPP_IPP_CD_CCP_ERR_DBG_ITS_CD_CCP_ERR_DBG_B_0x1() (p_IPP_IP->IPP_CD_CCP_ERR_DBG_ITS.IPP_CD_CCP_ERR_DBG_ITS_ts.CD_CCP_ERR_DBG == CD_CCP_ERR_DBG_B_0x1)
#define CD_CCP_ERR_DBG_B_0x1 0x1    //fs;fs
#define Is_IPP_IPP_CD_CCP_ERR_DBG_ITS_CD_CCP_ERR_DBG_B_0x2() (p_IPP_IP->IPP_CD_CCP_ERR_DBG_ITS.IPP_CD_CCP_ERR_DBG_ITS_ts.CD_CCP_ERR_DBG == CD_CCP_ERR_DBG_B_0x2)
#define CD_CCP_ERR_DBG_B_0x2 0x2    //fs;ls
#define Is_IPP_IPP_CD_CCP_ERR_DBG_ITS_CD_CCP_ERR_DBG_B_0x3() (p_IPP_IP->IPP_CD_CCP_ERR_DBG_ITS.IPP_CD_CCP_ERR_DBG_ITS_ts.CD_CCP_ERR_DBG == CD_CCP_ERR_DBG_B_0x3)
#define CD_CCP_ERR_DBG_B_0x3 0x3    //le;fs
#define Is_IPP_IPP_CD_CCP_ERR_DBG_ITS_CD_CCP_ERR_DBG_B_0x4() (p_IPP_IP->IPP_CD_CCP_ERR_DBG_ITS.IPP_CD_CCP_ERR_DBG_ITS_ts.CD_CCP_ERR_DBG == CD_CCP_ERR_DBG_B_0x4)
#define CD_CCP_ERR_DBG_B_0x4 0x4    //le;fe
#define Is_IPP_IPP_CD_CCP_ERR_DBG_ITS_CD_CCP_ERR_DBG_B_0x5() (p_IPP_IP->IPP_CD_CCP_ERR_DBG_ITS.IPP_CD_CCP_ERR_DBG_ITS_ts.CD_CCP_ERR_DBG == CD_CCP_ERR_DBG_B_0x5)
#define CD_CCP_ERR_DBG_B_0x5 0x5    //le;le
#define Is_IPP_IPP_CD_CCP_ERR_DBG_ITS_CD_CCP_ERR_DBG_B_0x6() (p_IPP_IP->IPP_CD_CCP_ERR_DBG_ITS.IPP_CD_CCP_ERR_DBG_ITS_ts.CD_CCP_ERR_DBG == CD_CCP_ERR_DBG_B_0x6)
#define CD_CCP_ERR_DBG_B_0x6 0x6    //ls;fs
#define Is_IPP_IPP_CD_CCP_ERR_DBG_ITS_CD_CCP_ERR_DBG_B_0x7() (p_IPP_IP->IPP_CD_CCP_ERR_DBG_ITS.IPP_CD_CCP_ERR_DBG_ITS_ts.CD_CCP_ERR_DBG == CD_CCP_ERR_DBG_B_0x7)
#define CD_CCP_ERR_DBG_B_0x7 0x7    //ls;ls
#define Get_IPP_IPP_CD_CCP_ERR_DBG_ITS() p_IPP_IP->IPP_CD_CCP_ERR_DBG_ITS.word


//IPP_ISP_INTERNAL_ITS_L


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_USER_IF() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.USER_IF
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_USER_IF_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.USER_IF == USER_IF_B_0x0)
#define USER_IF_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_USER_IF_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.USER_IF == USER_IF_B_0x1)
#define USER_IF_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_MASTER_CCI() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.MASTER_CCI
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_MASTER_CCI_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.MASTER_CCI == MASTER_CCI_B_0x0)
#define MASTER_CCI_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_MASTER_CCI_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.MASTER_CCI == MASTER_CCI_B_0x1)
#define MASTER_CCI_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_LBE() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.LBE
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_LBE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.LBE == LBE_B_0x0)
#define LBE_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_LBE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.LBE == LBE_B_0x1)
#define LBE_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_REPIPE() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.REPIPE
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_REPIPE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.REPIPE == REPIPE_B_0x0)
#define REPIPE_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_REPIPE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.REPIPE == REPIPE_B_0x1)
#define REPIPE_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_ISP_RX() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.ISP_RX
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_ISP_RX_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.ISP_RX == ISP_RX_B_0x0)
#define ISP_RX_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_ISP_RX_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.ISP_RX == ISP_RX_B_0x1)
#define ISP_RX_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_VIDEO_COMPLETE_PIPE0() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.VIDEO_COMPLETE_PIPE0
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_VIDEO_COMPLETE_PIPE0_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.VIDEO_COMPLETE_PIPE0 == VIDEO_COMPLETE_PIPE0_B_0x0)
#define VIDEO_COMPLETE_PIPE0_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_VIDEO_COMPLETE_PIPE0_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.VIDEO_COMPLETE_PIPE0 == VIDEO_COMPLETE_PIPE0_B_0x1)
#define VIDEO_COMPLETE_PIPE0_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_VIDEO_COMPLETE_PIPE1() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.VIDEO_COMPLETE_PIPE1
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_VIDEO_COMPLETE_PIPE1_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.VIDEO_COMPLETE_PIPE1 == VIDEO_COMPLETE_PIPE1_B_0x0)
#define VIDEO_COMPLETE_PIPE1_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_VIDEO_COMPLETE_PIPE1_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.VIDEO_COMPLETE_PIPE1 == VIDEO_COMPLETE_PIPE1_B_0x1)
#define VIDEO_COMPLETE_PIPE1_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_HISTOGRAM_256_BINS() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.HISTOGRAM_256_BINS
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_HISTOGRAM_256_BINS_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.HISTOGRAM_256_BINS == HISTOGRAM_256_BINS_B_0x0)
#define HISTOGRAM_256_BINS_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_HISTOGRAM_256_BINS_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.HISTOGRAM_256_BINS == HISTOGRAM_256_BINS_B_0x1)
#define HISTOGRAM_256_BINS_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_EXPOSURE_6X8() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.EXPOSURE_6X8
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_EXPOSURE_6X8_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.EXPOSURE_6X8 == EXPOSURE_6X8_B_0x0)
#define EXPOSURE_6X8_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_EXPOSURE_6X8_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.EXPOSURE_6X8 == EXPOSURE_6X8_B_0x1)
#define EXPOSURE_6X8_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_AF_STATS() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.AF_STATS
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_AF_STATS_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.AF_STATS == AF_STATS_B_0x0)
#define AF_STATS_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_AF_STATS_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.AF_STATS == AF_STATS_B_0x1)
#define AF_STATS_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_WB_STATS() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.WB_STATS
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_WB_STATS_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.WB_STATS == WB_STATS_B_0x0)
#define WB_STATS_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_WB_STATS_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.WB_STATS == WB_STATS_B_0x1)
#define WB_STATS_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_SKIN_TONE_STATS() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.SKIN_TONE_STATS
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_SKIN_TONE_STATS_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.SKIN_TONE_STATS == SKIN_TONE_STATS_B_0x0)
#define SKIN_TONE_STATS_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_SKIN_TONE_STATS_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.SKIN_TONE_STATS == SKIN_TONE_STATS_B_0x1)
#define SKIN_TONE_STATS_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_DMCEPIPE() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.DMCEPIPE
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_DMCEPIPE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.DMCEPIPE == DMCEPIPE_B_0x0)
#define DMCEPIPE_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_DMCEPIPE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.DMCEPIPE == DMCEPIPE_B_0x1)
#define DMCEPIPE_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_ARCTIC() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.ARCTIC
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_ARCTIC_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.ARCTIC == ARCTIC_B_0x0)
#define ARCTIC_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_ARCTIC_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.ARCTIC == ARCTIC_B_0x1)
#define ARCTIC_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_L_ACCWGZONE() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.ACCWGZONE
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_ACCWGZONE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.ACCWGZONE == ACCWGZONE_B_0x0)
#define ACCWGZONE_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_L_ACCWGZONE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.IPP_ISP_INTERNAL_ITS_L_ts.ACCWGZONE == ACCWGZONE_B_0x1)
#define ACCWGZONE_B_0x1 0x1    //event pending
#define Get_IPP_IPP_ISP_INTERNAL_ITS_L() p_IPP_IP->IPP_ISP_INTERNAL_ITS_L.word


//IPP_ISP_INTERNAL_ITS_BCLR_L


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_USER_IF(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.USER_IF = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_MASTER_CCI(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.MASTER_CCI = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_LBE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.LBE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_REPIPE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.REPIPE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_ISP_RX(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.ISP_RX = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_VIDEO_COMPLETE_PIPE0(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.VIDEO_COMPLETE_PIPE0 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_VIDEO_COMPLETE_PIPE1(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.VIDEO_COMPLETE_PIPE1 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_HISTOGRAM_256_BINS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.HISTOGRAM_256_BINS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_EXPOSURE_6X8(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.EXPOSURE_6X8 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_AF_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.AF_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_WB_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.WB_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_SKIN_TONE_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.SKIN_TONE_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_DMCEPIPE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.DMCEPIPE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_ARCTIC(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.ARCTIC = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_ACCWGZONE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.IPP_ISP_INTERNAL_ITS_BCLR_L_ts.ACCWGZONE = x)
#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L(USER_IF,MASTER_CCI,LBE,REPIPE,ISP_RX,VIDEO_COMPLETE_PIPE0,VIDEO_COMPLETE_PIPE1,HISTOGRAM_256_BINS,EXPOSURE_6X8,AF_STATS,WB_STATS,SKIN_TONE_STATS,DMCEPIPE,ARCTIC,ACCWGZONE) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.word = (uint16_t)USER_IF<<0 | (uint16_t)MASTER_CCI<<2 | (uint16_t)LBE<<3 | (uint16_t)REPIPE<<4 | (uint16_t)ISP_RX<<5 | (uint16_t)VIDEO_COMPLETE_PIPE0<<6 | (uint16_t)VIDEO_COMPLETE_PIPE1<<7 | (uint16_t)HISTOGRAM_256_BINS<<8 | (uint16_t)EXPOSURE_6X8<<9 | (uint16_t)AF_STATS<<10 | (uint16_t)WB_STATS<<11 | (uint16_t)SKIN_TONE_STATS<<12 | (uint16_t)DMCEPIPE<<13 | (uint16_t)ARCTIC<<14 | (uint16_t)ACCWGZONE<<15)
#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_L_word(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_L.word = x)


//IPP_ISP_INTERNAL_ITS_BSET_L


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_USER_IF(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.USER_IF = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_MASTER_CCI(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.MASTER_CCI = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_LBE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.LBE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_REPIPE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.REPIPE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_ISP_RX(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.ISP_RX = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_VIDEO_COMPLETE_PIPE0(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.VIDEO_COMPLETE_PIPE0 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_VIDEO_COMPLETE_PIPE1(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.VIDEO_COMPLETE_PIPE1 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_HISTOGRAM_256_BINS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.HISTOGRAM_256_BINS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_EXPOSURE_6X8(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.EXPOSURE_6X8 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_AF_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.AF_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_WB_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.WB_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_SKIN_TONE_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.SKIN_TONE_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_DMCEPIPE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.DMCEPIPE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_ARCTIC(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.ARCTIC = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_ACCWGZONE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.IPP_ISP_INTERNAL_ITS_BSET_L_ts.ACCWGZONE = x)
#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L(USER_IF,MASTER_CCI,LBE,REPIPE,ISP_RX,VIDEO_COMPLETE_PIPE0,VIDEO_COMPLETE_PIPE1,HISTOGRAM_256_BINS,EXPOSURE_6X8,AF_STATS,WB_STATS,SKIN_TONE_STATS,DMCEPIPE,ARCTIC,ACCWGZONE) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.word = (uint16_t)USER_IF<<0 | (uint16_t)MASTER_CCI<<2 | (uint16_t)LBE<<3 | (uint16_t)REPIPE<<4 | (uint16_t)ISP_RX<<5 | (uint16_t)VIDEO_COMPLETE_PIPE0<<6 | (uint16_t)VIDEO_COMPLETE_PIPE1<<7 | (uint16_t)HISTOGRAM_256_BINS<<8 | (uint16_t)EXPOSURE_6X8<<9 | (uint16_t)AF_STATS<<10 | (uint16_t)WB_STATS<<11 | (uint16_t)SKIN_TONE_STATS<<12 | (uint16_t)DMCEPIPE<<13 | (uint16_t)ARCTIC<<14 | (uint16_t)ACCWGZONE<<15)
#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_L_word(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_L.word = x)


//IPP_ISP_INTERNAL_ITM_L


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_USER_IF() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.USER_IF
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_USER_IF_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.USER_IF == USER_IF_B_0x0)
#define USER_IF_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_USER_IF_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.USER_IF == USER_IF_B_0x1)
#define USER_IF_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_MASTER_CCI() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.MASTER_CCI
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_MASTER_CCI_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.MASTER_CCI == MASTER_CCI_B_0x0)
#define MASTER_CCI_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_MASTER_CCI_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.MASTER_CCI == MASTER_CCI_B_0x1)
#define MASTER_CCI_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_LBE() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.LBE
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_LBE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.LBE == LBE_B_0x0)
#define LBE_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_LBE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.LBE == LBE_B_0x1)
#define LBE_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_REPIPE() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.REPIPE
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_REPIPE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.REPIPE == REPIPE_B_0x0)
#define REPIPE_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_REPIPE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.REPIPE == REPIPE_B_0x1)
#define REPIPE_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_ISP_RX() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.ISP_RX
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_ISP_RX_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.ISP_RX == ISP_RX_B_0x0)
#define ISP_RX_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_ISP_RX_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.ISP_RX == ISP_RX_B_0x1)
#define ISP_RX_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_VIDEO_COMPLETE_PIPE0() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.VIDEO_COMPLETE_PIPE0
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_VIDEO_COMPLETE_PIPE0_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.VIDEO_COMPLETE_PIPE0 == VIDEO_COMPLETE_PIPE0_B_0x0)
#define VIDEO_COMPLETE_PIPE0_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_VIDEO_COMPLETE_PIPE0_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.VIDEO_COMPLETE_PIPE0 == VIDEO_COMPLETE_PIPE0_B_0x1)
#define VIDEO_COMPLETE_PIPE0_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_VIDEO_COMPLETE_PIPE1() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.VIDEO_COMPLETE_PIPE1
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_VIDEO_COMPLETE_PIPE1_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.VIDEO_COMPLETE_PIPE1 == VIDEO_COMPLETE_PIPE1_B_0x0)
#define VIDEO_COMPLETE_PIPE1_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_VIDEO_COMPLETE_PIPE1_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.VIDEO_COMPLETE_PIPE1 == VIDEO_COMPLETE_PIPE1_B_0x1)
#define VIDEO_COMPLETE_PIPE1_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_HISTOGRAM_256_BINS() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.HISTOGRAM_256_BINS
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_HISTOGRAM_256_BINS_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.HISTOGRAM_256_BINS == HISTOGRAM_256_BINS_B_0x0)
#define HISTOGRAM_256_BINS_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_HISTOGRAM_256_BINS_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.HISTOGRAM_256_BINS == HISTOGRAM_256_BINS_B_0x1)
#define HISTOGRAM_256_BINS_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_EXPOSURE_6X8() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.EXPOSURE_6X8
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_EXPOSURE_6X8_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.EXPOSURE_6X8 == EXPOSURE_6X8_B_0x0)
#define EXPOSURE_6X8_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_EXPOSURE_6X8_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.EXPOSURE_6X8 == EXPOSURE_6X8_B_0x1)
#define EXPOSURE_6X8_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_AF_STATS() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.AF_STATS
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_AF_STATS_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.AF_STATS == AF_STATS_B_0x0)
#define AF_STATS_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_AF_STATS_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.AF_STATS == AF_STATS_B_0x1)
#define AF_STATS_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_WB_STATS() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.WB_STATS
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_WB_STATS_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.WB_STATS == WB_STATS_B_0x0)
#define WB_STATS_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_WB_STATS_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.WB_STATS == WB_STATS_B_0x1)
#define WB_STATS_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_SKIN_TONE_STATS() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.SKIN_TONE_STATS
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_SKIN_TONE_STATS_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.SKIN_TONE_STATS == SKIN_TONE_STATS_B_0x0)
#define SKIN_TONE_STATS_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_SKIN_TONE_STATS_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.SKIN_TONE_STATS == SKIN_TONE_STATS_B_0x1)
#define SKIN_TONE_STATS_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_DMCEPIPE() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.DMCEPIPE
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_DMCEPIPE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.DMCEPIPE == DMCEPIPE_B_0x0)
#define DMCEPIPE_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_DMCEPIPE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.DMCEPIPE == DMCEPIPE_B_0x1)
#define DMCEPIPE_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_ARCTIC() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.ARCTIC
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_ARCTIC_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.ARCTIC == ARCTIC_B_0x0)
#define ARCTIC_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_ARCTIC_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.ARCTIC == ARCTIC_B_0x1)
#define ARCTIC_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_L_ACCWGZONE() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.ACCWGZONE
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_ACCWGZONE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.ACCWGZONE == ACCWGZONE_B_0x0)
#define ACCWGZONE_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_L_ACCWGZONE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.IPP_ISP_INTERNAL_ITM_L_ts.ACCWGZONE == ACCWGZONE_B_0x1)
#define ACCWGZONE_B_0x1 0x1    //event enabled
#define Get_IPP_IPP_ISP_INTERNAL_ITM_L() p_IPP_IP->IPP_ISP_INTERNAL_ITM_L.word


//IPP_ISP_INTERNAL_ITM_BCLR_L


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_USER_IF(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.USER_IF = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_MASTER_CCI(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.MASTER_CCI = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_LBE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.LBE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_REPIPE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.REPIPE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_ISP_RX(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.ISP_RX = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_VIDEO_COMPLETE_PIPE0(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.VIDEO_COMPLETE_PIPE0 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_VIDEO_COMPLETE_PIPE1(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.VIDEO_COMPLETE_PIPE1 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_HISTOGRAM_256_BINS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.HISTOGRAM_256_BINS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_EXPOSURE_6X8(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.EXPOSURE_6X8 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_AF_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.AF_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_WB_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.WB_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_SKIN_TONE_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.SKIN_TONE_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_DMCEPIPE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.DMCEPIPE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_ARCTIC(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.ARCTIC = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_ACCWGZONE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.IPP_ISP_INTERNAL_ITM_BCLR_L_ts.ACCWGZONE = x)
#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L(USER_IF,MASTER_CCI,LBE,REPIPE,ISP_RX,VIDEO_COMPLETE_PIPE0,VIDEO_COMPLETE_PIPE1,HISTOGRAM_256_BINS,EXPOSURE_6X8,AF_STATS,WB_STATS,SKIN_TONE_STATS,DMCEPIPE,ARCTIC,ACCWGZONE) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.word = (uint16_t)USER_IF<<0 | (uint16_t)MASTER_CCI<<2 | (uint16_t)LBE<<3 | (uint16_t)REPIPE<<4 | (uint16_t)ISP_RX<<5 | (uint16_t)VIDEO_COMPLETE_PIPE0<<6 | (uint16_t)VIDEO_COMPLETE_PIPE1<<7 | (uint16_t)HISTOGRAM_256_BINS<<8 | (uint16_t)EXPOSURE_6X8<<9 | (uint16_t)AF_STATS<<10 | (uint16_t)WB_STATS<<11 | (uint16_t)SKIN_TONE_STATS<<12 | (uint16_t)DMCEPIPE<<13 | (uint16_t)ARCTIC<<14 | (uint16_t)ACCWGZONE<<15)
#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_L_word(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_L.word = x)


//IPP_ISP_INTERNAL_ITM_BSET_L


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_USER_IF(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.USER_IF = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_MASTER_CCI(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.MASTER_CCI = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_LBE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.LBE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_REPIPE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.REPIPE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_ISP_RX(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.ISP_RX = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_VIDEO_COMPLETE_PIPE0(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.VIDEO_COMPLETE_PIPE0 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_VIDEO_COMPLETE_PIPE1(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.VIDEO_COMPLETE_PIPE1 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_HISTOGRAM_256_BINS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.HISTOGRAM_256_BINS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_EXPOSURE_6X8(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.EXPOSURE_6X8 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_AF_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.AF_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_WB_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.WB_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_SKIN_TONE_STATS(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.SKIN_TONE_STATS = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_DMCEPIPE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.DMCEPIPE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_ARCTIC(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.ARCTIC = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_ACCWGZONE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.IPP_ISP_INTERNAL_ITM_BSET_L_ts.ACCWGZONE = x)
#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L(USER_IF,MASTER_CCI,LBE,REPIPE,ISP_RX,VIDEO_COMPLETE_PIPE0,VIDEO_COMPLETE_PIPE1,HISTOGRAM_256_BINS,EXPOSURE_6X8,AF_STATS,WB_STATS,SKIN_TONE_STATS,DMCEPIPE,ARCTIC,ACCWGZONE) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.word = (uint32_t)USER_IF<<0 | (uint32_t)MASTER_CCI<<2 | (uint32_t)LBE<<3 | (uint32_t)REPIPE<<4 | (uint32_t)ISP_RX<<5 | (uint32_t)VIDEO_COMPLETE_PIPE0<<6 | (uint32_t)VIDEO_COMPLETE_PIPE1<<7 | (uint32_t)HISTOGRAM_256_BINS<<8 | (uint32_t)EXPOSURE_6X8<<9 | (uint32_t)AF_STATS<<10 | (uint32_t)WB_STATS<<11 | (uint32_t)SKIN_TONE_STATS<<12 | (uint32_t)DMCEPIPE<<13 | (uint32_t)ARCTIC<<14 | (uint32_t)ACCWGZONE<<15)
#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_L_word(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_L.word = x)


//IPP_ISP_INTERNAL_ITS_H


#define Get_IPP_IPP_ISP_INTERNAL_ITS_H_GLACE() p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.GLACE
#define Is_IPP_IPP_ISP_INTERNAL_ITS_H_GLACE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.GLACE == GLACE_B_0x0)
#define GLACE_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_H_GLACE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.GLACE == GLACE_B_0x1)
#define GLACE_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_H_GPIO() p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.GPIO
#define Is_IPP_IPP_ISP_INTERNAL_ITS_H_GPIO_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.GPIO == GPIO_B_0x0)
#define GPIO_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_H_GPIO_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.GPIO == GPIO_B_0x1)
#define GPIO_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_H_DMCE_LBE() p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DMCE_LBE
#define Is_IPP_IPP_ISP_INTERNAL_ITS_H_DMCE_LBE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DMCE_LBE == DMCE_LBE_B_0x0)
#define DMCE_LBE_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_H_DMCE_LBE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DMCE_LBE == DMCE_LBE_B_0x1)
#define DMCE_LBE_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_H_SMIARX_3D() p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.SMIARX_3D
#define Is_IPP_IPP_ISP_INTERNAL_ITS_H_SMIARX_3D_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.SMIARX_3D == SMIARX_3D_B_0x0)
#define SMIARX_3D_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_ISP_INTERNAL_ITS_H_SMIARX_3D_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.SMIARX_3D == SMIARX_3D_B_0x1)
#define SMIARX_3D_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_H_DXO_PDP() p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_PDP
#define Is__IPP_ISP_INTERNAL_ITS_H_DXO_PDP_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_PDP == DXO_PDP_B_0x0)
#define DXO_PDP_B_0x0 0x0    //no event pending
#define Is__IPP_ISP_INTERNAL_ITS_H_DXO_PDP_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_PDP == DXO_PDP_B_0x1)
#define DXO_PDP_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_H_DXO_DPP() p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_DPP
#define Is__IPP_ISP_INTERNAL_ITS_H_DXO_DPP_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_DPP == DXO_DPP_B_0x0)
#define DXO_DPP_B_0x0 0x0    //no event pending
#define Is__IPP_ISP_INTERNAL_ITS_H_DXO_DPP_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_DPP == DXO_DPP_B_0x1)
#define DXO_DPP_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_H_DXO_DOP7() p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_DOP7
#define Is__IPP_ISP_INTERNAL_ITS_H_DXO_DOP7_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_DOP7 == DXO_DOP7_B_0x0)
#define DXO_DOP7_B_0x0 0x0    //no event pending
#define Is__IPP_ISP_INTERNAL_ITS_H_DXO_DOP7_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_DOP7 == DXO_DOP7_B_0x1)
#define DXO_DOP7_B_0x1 0x1    //event pending


#define Get_IPP_IPP_ISP_INTERNAL_ITS_H_DXO_INT() p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_INT
#define Is__IPP_ISP_INTERNAL_ITS_H_DXO_INT_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_INT == DXO_INT_B_0x0)
#define DXO_INT_B_0x0 0x0    //no event pending
#define Is__IPP_ISP_INTERNAL_ITS_H_DXO_INT_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.IPP_ISP_INTERNAL_ITS_H_ts.DXO_INT == DXO_INT_B_0x1)
#define DXO_INT_B_0x1 0x1    //event pending
#define Get_IPP_IPP_ISP_INTERNAL_ITS_H() p_IPP_IP->IPP_ISP_INTERNAL_ITS_H.word


//IPP_ISP_INTERNAL_ITS_BCLR_H


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H_GLACE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.IPP_ISP_INTERNAL_ITS_BCLR_H_ts.GLACE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H_GPIO(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.IPP_ISP_INTERNAL_ITS_BCLR_H_ts.GPIO = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H_DMCE_LBE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.IPP_ISP_INTERNAL_ITS_BCLR_H_ts.DMCE_LBE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H_SMIARX_3D(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.IPP_ISP_INTERNAL_ITS_BCLR_H_ts.SMIARX_3D = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H_DXO_PDP(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.IPP_ISP_INTERNAL_ITS_BCLR_H_ts.DXO_PDP = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H_DXO_DPP(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.IPP_ISP_INTERNAL_ITS_BCLR_H_ts.DXO_DPP = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H_DXO_DOP7(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.IPP_ISP_INTERNAL_ITS_BCLR_H_ts.DXO_DOP7 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H_DXO_INT(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.IPP_ISP_INTERNAL_ITS_BCLR_H_ts.DXO_INT = x)
#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H(GLACE,GPIO,DMCE_LBE,SMIARX_3D,DXO_PDP,DXO_DPP,DXO_DOP7,DXO_INT) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.word = (uint16_t)GLACE<<0 | (uint16_t)GPIO<<1 | (uint16_t)DMCE_LBE<<2 | (uint16_t)SMIARX_3D<<3 | (uint16_t)DXO_PDP<<8 | (uint16_t)DXO_DPP<<9 | (uint16_t)DXO_DOP7<<10 | (uint16_t)DXO_INT<<11)
#define Set_IPP_IPP_ISP_INTERNAL_ITS_BCLR_H_word(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BCLR_H.word = x)


//IPP_ISP_INTERNAL_ITS_BSET_H


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H_GLACE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.IPP_ISP_INTERNAL_ITS_BSET_H_ts.GLACE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H_GPIO(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.IPP_ISP_INTERNAL_ITS_BSET_H_ts.GPIO = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H_DMCE_LBE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.IPP_ISP_INTERNAL_ITS_BSET_H_ts.DMCE_LBE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H_SMIARX_3D(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.IPP_ISP_INTERNAL_ITS_BSET_H_ts.SMIARX_3D = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H_DXO_PDP(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.IPP_ISP_INTERNAL_ITS_BSET_H_ts.DXO_PDP = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H_DXO_DPP(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.IPP_ISP_INTERNAL_ITS_BSET_H_ts.DXO_DPP = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H_DXO_DOP7(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.IPP_ISP_INTERNAL_ITS_BSET_H_ts.DXO_DOP7 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H_DXO_INT(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.IPP_ISP_INTERNAL_ITS_BSET_H_ts.DXO_INT = x)
#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H(GLACE,GPIO,DMCE_LBE,SMIARX_3D,DXO_PDP,DXO_DPP,DXO_DOP7,DXO_INT) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.word = (uint16_t)GLACE<<0 | (uint16_t)GPIO<<1 | (uint16_t)DMCE_LBE<<2 | (uint16_t)SMIARX_3D<<3 | (uint16_t)DXO_PDP<<8 | (uint16_t)DXO_DPP<<9 | (uint16_t)DXO_DOP7<<10 | (uint16_t)DXO_INT<<11)
#define Set_IPP_IPP_ISP_INTERNAL_ITS_BSET_H_word(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITS_BSET_H.word = x)


//IPP_ISP_INTERNAL_ITM_H


#define Get_IPP_IPP_ISP_INTERNAL_ITM_H_GLACE() p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.GLACE
#define Is_IPP_IPP_ISP_INTERNAL_ITM_H_GLACE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.GLACE == GLACE_B_0x0)
#define GLACE_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_H_GLACE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.GLACE == GLACE_B_0x1)
#define GLACE_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_H_GPIO() p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.GPIO
#define Is_IPP_IPP_ISP_INTERNAL_ITM_H_GPIO_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.GPIO == GPIO_B_0x0)
#define GPIO_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_H_GPIO_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.GPIO == GPIO_B_0x1)
#define GPIO_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_H_DMCE_LBE() p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DMCE_LBE
#define Is_IPP_IPP_ISP_INTERNAL_ITM_H_DMCE_LBE_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DMCE_LBE == DMCE_LBE_B_0x0)
#define DMCE_LBE_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_H_DMCE_LBE_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DMCE_LBE == DMCE_LBE_B_0x1)
#define DMCE_LBE_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_H_SMIARX_3D() p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.SMIARX_3D
#define Is_IPP_IPP_ISP_INTERNAL_ITM_H_SMIARX_3D_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.SMIARX_3D == SMIARX_3D_B_0x0)
#define SMIARX_3D_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_ISP_INTERNAL_ITM_H_SMIARX_3D_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.SMIARX_3D == SMIARX_3D_B_0x1)
#define SMIARX_3D_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_H_DXO_PDP() p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_PDP
#define Is__IPP_ISP_INTERNAL_ITM_H_DXO_PDP_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_PDP == DXO_PDP_B_0x0)
#define DXO_PDP_B_0x0 0x0    //event disabled
#define Is__IPP_ISP_INTERNAL_ITM_H_DXO_PDP_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_PDP == DXO_PDP_B_0x1)
#define DXO_PDP_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_H_DXO_DPP() p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_DPP
#define Is__IPP_ISP_INTERNAL_ITM_H_DXO_DPP_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_DPP == DXO_DPP_B_0x0)
#define DXO_DPP_B_0x0 0x0    //event disabled
#define Is__IPP_ISP_INTERNAL_ITM_H_DXO_DPP_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_DPP == DXO_DPP_B_0x1)
#define DXO_DPP_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_H_DXO_DOP7() p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_DOP7
#define Is__IPP_ISP_INTERNAL_ITM_H_DXO_DOP7_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_DOP7 == DXO_DOP7_B_0x0)
#define DXO_DOP7_B_0x0 0x0    //event disabled
#define Is__IPP_ISP_INTERNAL_ITM_H_DXO_DOP7_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_DOP7 == DXO_DOP7_B_0x1)
#define DXO_DOP7_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_ISP_INTERNAL_ITM_H_DXO_INT() p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_INT
#define Is__IPP_ISP_INTERNAL_ITM_H_DXO_INT_B_0x0() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_INT == DXO_INT_B_0x0)
#define DXO_INT_B_0x0 0x0    //event disabled
#define Is__IPP_ISP_INTERNAL_ITM_H_DXO_INT_B_0x1() (p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.IPP_ISP_INTERNAL_ITM_H_ts.DXO_INT == DXO_INT_B_0x1)
#define DXO_INT_B_0x1 0x1    //event enabled
#define Get_IPP_IPP_ISP_INTERNAL_ITM_H() p_IPP_IP->IPP_ISP_INTERNAL_ITM_H.word


//IPP_ISP_INTERNAL_ITM_BCLR_H


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H_GLACE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.IPP_ISP_INTERNAL_ITM_BCLR_H_ts.GLACE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H_GPIO(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.IPP_ISP_INTERNAL_ITM_BCLR_H_ts.GPIO = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H_DMCE_LBE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.IPP_ISP_INTERNAL_ITM_BCLR_H_ts.DMCE_LBE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H_SMIARX_3D(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.IPP_ISP_INTERNAL_ITM_BCLR_H_ts.SMIARX_3D = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H_DXO_PDP(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.IPP_ISP_INTERNAL_ITM_BCLR_H_ts.DXO_PDP = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H_DXO_DPP(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.IPP_ISP_INTERNAL_ITM_BCLR_H_ts.DXO_DPP = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H_DXO_DOP7(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.IPP_ISP_INTERNAL_ITM_BCLR_H_ts.DXO_DOP7 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H_DXO_INT(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.IPP_ISP_INTERNAL_ITM_BCLR_H_ts.DXO_INT = x)
#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H(GLACE,GPIO,DMCE_LBE,SMIARX_3D,DXO_PDP,DXO_DPP,DXO_DOP7,DXO_INT) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.word = (uint16_t)GLACE<<0 | (uint16_t)GPIO<<1 | (uint16_t)DMCE_LBE<<2 | (uint16_t)SMIARX_3D<<3 | (uint16_t)DXO_PDP<<8 | (uint16_t)DXO_DPP<<9 | (uint16_t)DXO_DOP7<<10 | (uint16_t)DXO_INT<<11)
#define Set_IPP_IPP_ISP_INTERNAL_ITM_BCLR_H_word(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BCLR_H.word = x)


//IPP_ISP_INTERNAL_ITM_BSET_H


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H_GLACE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.IPP_ISP_INTERNAL_ITM_BSET_H_ts.GLACE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H_GPIO(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.IPP_ISP_INTERNAL_ITM_BSET_H_ts.GPIO = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H_DMCE_LBE(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.IPP_ISP_INTERNAL_ITM_BSET_H_ts.DMCE_LBE = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H_SMIARX_3D(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.IPP_ISP_INTERNAL_ITM_BSET_H_ts.SMIARX_3D = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H_DXO_PDP(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.IPP_ISP_INTERNAL_ITM_BSET_H_ts.DXO_PDP = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H_DXO_DPP(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.IPP_ISP_INTERNAL_ITM_BSET_H_ts.DXO_DPP = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H_DXO_DOP7(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.IPP_ISP_INTERNAL_ITM_BSET_H_ts.DXO_DOP7 = x)


#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H_DXO_INT(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.IPP_ISP_INTERNAL_ITM_BSET_H_ts.DXO_INT = x)
#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H(GLACE,GPIO,DMCE_LBE,SMIARX_3D,DXO_PDP,DXO_DPP,DXO_DOP7,DXO_INT) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.word = (uint32_t)GLACE<<0 | (uint32_t)GPIO<<1 | (uint32_t)DMCE_LBE<<2 | (uint32_t)SMIARX_3D<<3 | (uint32_t)DXO_PDP<<8 | (uint32_t)DXO_DPP<<9 | (uint32_t)DXO_DOP7<<10 | (uint32_t)DXO_INT<<11)
#define Set_IPP_IPP_ISP_INTERNAL_ITM_BSET_H_word(x) (p_IPP_IP->IPP_ISP_INTERNAL_ITM_BSET_H.word = x)


//IPP_SIA_ITEXT_ITS


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT0() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT0
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT0_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT0 == ITEXT0_B_0x0)
#define ITEXT0_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT0_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT0 == ITEXT0_B_0x1)
#define ITEXT0_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT1() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT1
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT1_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT1 == ITEXT1_B_0x0)
#define ITEXT1_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT1_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT1 == ITEXT1_B_0x1)
#define ITEXT1_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT2() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT2
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT2_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT2 == ITEXT2_B_0x0)
#define ITEXT2_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT2_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT2 == ITEXT2_B_0x1)
#define ITEXT2_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT3() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT3
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT3_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT3 == ITEXT3_B_0x0)
#define ITEXT3_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT3_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT3 == ITEXT3_B_0x1)
#define ITEXT3_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT4() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT4
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT4_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT4 == ITEXT4_B_0x0)
#define ITEXT4_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT4_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT4 == ITEXT4_B_0x1)
#define ITEXT4_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT5() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT5
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT5_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT5 == ITEXT5_B_0x0)
#define ITEXT5_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT5_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT5 == ITEXT5_B_0x1)
#define ITEXT5_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT6() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT6
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT6_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT6 == ITEXT6_B_0x0)
#define ITEXT6_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT6_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT6 == ITEXT6_B_0x1)
#define ITEXT6_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT7() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT7
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT7_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT7 == ITEXT7_B_0x0)
#define ITEXT7_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT7_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT7 == ITEXT7_B_0x1)
#define ITEXT7_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT8() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT8
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT8_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT8 == ITEXT8_B_0x0)
#define ITEXT8_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT8_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT8 == ITEXT8_B_0x1)
#define ITEXT8_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT9() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT9
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT9_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT9 == ITEXT9_B_0x0)
#define ITEXT9_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT9_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT9 == ITEXT9_B_0x1)
#define ITEXT9_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT10() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT10
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT10_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT10 == ITEXT10_B_0x0)
#define ITEXT10_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT10_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT10 == ITEXT10_B_0x1)
#define ITEXT10_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT11() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT11
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT11_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT11 == ITEXT11_B_0x0)
#define ITEXT11_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT11_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT11 == ITEXT11_B_0x1)
#define ITEXT11_B_0x1 0x1    //event pending


#define Get_IPP_IPP_SIA_ITEXT_ITS_ITEXT12() p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT12
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT12_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT12 == ITEXT12_B_0x0)
#define ITEXT12_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_SIA_ITEXT_ITS_ITEXT12_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITS.IPP_SIA_ITEXT_ITS_ts.ITEXT12 == ITEXT12_B_0x1)
#define ITEXT12_B_0x1 0x1    //event pending
#define Get_IPP_IPP_SIA_ITEXT_ITS() p_IPP_IP->IPP_SIA_ITEXT_ITS.word


//IPP_SIA_ITEXT_ITS_BCLR


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT0(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT0 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT1(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT1 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT2(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT2 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT3(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT3 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT4(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT4 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT5(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT5 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT6(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT6 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT7(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT7 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT8(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT8 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT9(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT9 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT10(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT10 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT11(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT11 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_ITEXT12(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.IPP_SIA_ITEXT_ITS_BCLR_ts.ITEXT12 = x)
#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR(ITEXT0,ITEXT1,ITEXT2,ITEXT3,ITEXT4,ITEXT5,ITEXT6,ITEXT7,ITEXT8,ITEXT9,ITEXT10,ITEXT11,ITEXT12) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.word = (uint16_t)ITEXT0<<0 | (uint16_t)ITEXT1<<1 | (uint16_t)ITEXT2<<2 | (uint16_t)ITEXT3<<3 | (uint16_t)ITEXT4<<4 | (uint16_t)ITEXT5<<5 | (uint16_t)ITEXT6<<6 | (uint16_t)ITEXT7<<7 | (uint16_t)ITEXT8<<8 | (uint16_t)ITEXT9<<9 | (uint16_t)ITEXT10<<10 | (uint16_t)ITEXT11<<11 | (uint16_t)ITEXT12<<12)
#define Set_IPP_IPP_SIA_ITEXT_ITS_BCLR_word(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BCLR.word = x)


//IPP_SIA_ITEXT_ITS_BSET


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT0(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT0 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT1(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT1 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT2(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT2 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT3(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT3 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT4(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT4 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT5(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT5 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT6(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT6 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT7(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT7 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT8(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT8 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT9(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT9 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT10(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT10 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT11(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT11 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_ITEXT12(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.IPP_SIA_ITEXT_ITS_BSET_ts.ITEXT12 = x)
#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET(ITEXT0,ITEXT1,ITEXT2,ITEXT3,ITEXT4,ITEXT5,ITEXT6,ITEXT7,ITEXT8,ITEXT9,ITEXT10,ITEXT11,ITEXT12) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.word = (uint16_t)ITEXT0<<0 | (uint16_t)ITEXT1<<1 | (uint16_t)ITEXT2<<2 | (uint16_t)ITEXT3<<3 | (uint16_t)ITEXT4<<4 | (uint16_t)ITEXT5<<5 | (uint16_t)ITEXT6<<6 | (uint16_t)ITEXT7<<7 | (uint16_t)ITEXT8<<8 | (uint16_t)ITEXT9<<9 | (uint16_t)ITEXT10<<10 | (uint16_t)ITEXT11<<11 | (uint16_t)ITEXT12<<12)
#define Set_IPP_IPP_SIA_ITEXT_ITS_BSET_word(x) (p_IPP_IP->IPP_SIA_ITEXT_ITS_BSET.word = x)


//IPP_SIA_ITEXT_ITM


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT0() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT0
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT0_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT0 == ITEXT0_B_0x0)
#define ITEXT0_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT0_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT0 == ITEXT0_B_0x1)
#define ITEXT0_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT1() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT1
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT1_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT1 == ITEXT1_B_0x0)
#define ITEXT1_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT1_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT1 == ITEXT1_B_0x1)
#define ITEXT1_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT2() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT2
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT2_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT2 == ITEXT2_B_0x0)
#define ITEXT2_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT2_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT2 == ITEXT2_B_0x1)
#define ITEXT2_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT3() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT3
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT3_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT3 == ITEXT3_B_0x0)
#define ITEXT3_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT3_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT3 == ITEXT3_B_0x1)
#define ITEXT3_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT4() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT4
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT4_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT4 == ITEXT4_B_0x0)
#define ITEXT4_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT4_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT4 == ITEXT4_B_0x1)
#define ITEXT4_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT5() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT5
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT5_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT5 == ITEXT5_B_0x0)
#define ITEXT5_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT5_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT5 == ITEXT5_B_0x1)
#define ITEXT5_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT6() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT6
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT6_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT6 == ITEXT6_B_0x0)
#define ITEXT6_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT6_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT6 == ITEXT6_B_0x1)
#define ITEXT6_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT7() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT7
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT7_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT7 == ITEXT7_B_0x0)
#define ITEXT7_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT7_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT7 == ITEXT7_B_0x1)
#define ITEXT7_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT8() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT8
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT8_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT8 == ITEXT8_B_0x0)
#define ITEXT8_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT8_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT8 == ITEXT8_B_0x1)
#define ITEXT8_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT9() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT9
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT9_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT9 == ITEXT9_B_0x0)
#define ITEXT9_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT9_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT9 == ITEXT9_B_0x1)
#define ITEXT9_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT10() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT10
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT10_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT10 == ITEXT10_B_0x0)
#define ITEXT10_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT10_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT10 == ITEXT10_B_0x1)
#define ITEXT10_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT11() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT11
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT11_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT11 == ITEXT11_B_0x0)
#define ITEXT11_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT11_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT11 == ITEXT11_B_0x1)
#define ITEXT11_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_SIA_ITEXT_ITM_ITEXT12() p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT12
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT12_B_0x0() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT12 == ITEXT12_B_0x0)
#define ITEXT12_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_SIA_ITEXT_ITM_ITEXT12_B_0x1() (p_IPP_IP->IPP_SIA_ITEXT_ITM.IPP_SIA_ITEXT_ITM_ts.ITEXT12 == ITEXT12_B_0x1)
#define ITEXT12_B_0x1 0x1    //event enabled
#define Get_IPP_IPP_SIA_ITEXT_ITM() p_IPP_IP->IPP_SIA_ITEXT_ITM.word


//IPP_SIA_ITEXT_ITM_BCLR


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT0(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT0 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT1(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT1 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT2(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT2 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT3(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT3 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT4(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT4 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT5(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT5 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT6(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT6 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT7(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT7 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT8(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT8 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT9(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT9 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT10(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT10 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT11(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT11 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_ITEXT12(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.IPP_SIA_ITEXT_ITM_BCLR_ts.ITEXT12 = x)
#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR(ITEXT0,ITEXT1,ITEXT2,ITEXT3,ITEXT4,ITEXT5,ITEXT6,ITEXT7,ITEXT8,ITEXT9,ITEXT10,ITEXT11,ITEXT12) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.word = (uint16_t)ITEXT0<<0 | (uint16_t)ITEXT1<<1 | (uint16_t)ITEXT2<<2 | (uint16_t)ITEXT3<<3 | (uint16_t)ITEXT4<<4 | (uint16_t)ITEXT5<<5 | (uint16_t)ITEXT6<<6 | (uint16_t)ITEXT7<<7 | (uint16_t)ITEXT8<<8 | (uint16_t)ITEXT9<<9 | (uint16_t)ITEXT10<<10 | (uint16_t)ITEXT11<<11 | (uint16_t)ITEXT12<<12)
#define Set_IPP_IPP_SIA_ITEXT_ITM_BCLR_word(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BCLR.word = x)


//IPP_SIA_ITEXT_ITM_BSET


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT0(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT0 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT1(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT1 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT2(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT2 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT3(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT3 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT4(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT4 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT5(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT5 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT6(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT6 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT7(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT7 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT8(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT8 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT9(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT9 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT10(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT10 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT11(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT11 = x)


#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_ITEXT12(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.IPP_SIA_ITEXT_ITM_BSET_ts.ITEXT12 = x)
#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET(ITEXT0,ITEXT1,ITEXT2,ITEXT3,ITEXT4,ITEXT5,ITEXT6,ITEXT7,ITEXT8,ITEXT9,ITEXT10,ITEXT11,ITEXT12) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.word = (uint32_t)ITEXT0<<0 | (uint32_t)ITEXT1<<1 | (uint32_t)ITEXT2<<2 | (uint32_t)ITEXT3<<3 | (uint32_t)ITEXT4<<4 | (uint32_t)ITEXT5<<5 | (uint32_t)ITEXT6<<6 | (uint32_t)ITEXT7<<7 | (uint32_t)ITEXT8<<8 | (uint32_t)ITEXT9<<9 | (uint32_t)ITEXT10<<10 | (uint32_t)ITEXT11<<11 | (uint32_t)ITEXT12<<12)
#define Set_IPP_IPP_SIA_ITEXT_ITM_BSET_word(x) (p_IPP_IP->IPP_SIA_ITEXT_ITM_BSET.word = x)


//IPP_EVT_ITS


#define Get_IPP_IPP_EVT_ITS_SD_ERROR() p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.SD_ERROR
#define Is_IPP_IPP_EVT_ITS_SD_ERROR_B_0x0() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.SD_ERROR == SD_ERROR_B_0x0)
#define SD_ERROR_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_EVT_ITS_SD_ERROR_B_0x1() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.SD_ERROR == SD_ERROR_B_0x1)
#define SD_ERROR_B_0x1 0x1    //event pending


#define Get_IPP_IPP_EVT_ITS_CD_ERROR() p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.CD_ERROR
#define Is_IPP_IPP_EVT_ITS_CD_ERROR_B_0x0() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.CD_ERROR == CD_ERROR_B_0x0)
#define CD_ERROR_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_EVT_ITS_CD_ERROR_B_0x1() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.CD_ERROR == CD_ERROR_B_0x1)
#define CD_ERROR_B_0x1 0x1    //event pending


#define Get_IPP_IPP_EVT_ITS_CD_RAW_EOF() p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.CD_RAW_EOF
#define Is_IPP_IPP_EVT_ITS_CD_RAW_EOF_B_0x0() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.CD_RAW_EOF == CD_RAW_EOF_B_0x0)
#define CD_RAW_EOF_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_EVT_ITS_CD_RAW_EOF_B_0x1() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.CD_RAW_EOF == CD_RAW_EOF_B_0x1)
#define CD_RAW_EOF_B_0x1 0x1    //event pending


#define Get_IPP_IPP_EVT_ITS_BML_EOF() p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.BML_EOF
#define Is_IPP_IPP_EVT_ITS_BML_EOF_B_0x0() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.BML_EOF == BML_EOF_B_0x0)
#define BML_EOF_B_0x0 0x0    //no event pending
#define Is_IPP_IPP_EVT_ITS_BML_EOF_B_0x1() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.BML_EOF == BML_EOF_B_0x1)
#define BML_EOF_B_0x1 0x1    //event pending


#define Get_IPP_IPP_EVT_ITS_DXO_PDP() p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_PDP
#define Is__IPP_EVT_ITS_DXO_PDP_B_0x0() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_PDP == DXO_PDP_B_0x0)
#define DXO_PDP_B_0x0 0x0    //event disabled
#define Is__IPP_EVT_ITS_DXO_PDP_B_0x1() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_PDP == DXO_PDP_B_0x1)
#define DXO_PDP_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITS_DXO_DPP() p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_DPP
#define Is__IPP_EVT_ITS_DXO_DPP_B_0x0() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_DPP == DXO_DPP_B_0x0)
#define DXO_DPP_B_0x0 0x0    //event disabled
#define Is__IPP_EVT_ITS_DXO_DPP_B_0x1() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_DPP == DXO_DPP_B_0x1)
#define DXO_DPP_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITS_DXO_DOP7() p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_DOP7
#define Is__IPP_EVT_ITS_DXO_DOP7_B_0x0() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_DOP7 == DXO_DOP7_B_0x0)
#define DXO_DOP7_B_0x0 0x0    //event disabled
#define Is__IPP_EVT_ITS_DXO_DOP7_B_0x1() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_DOP7 == DXO_DOP7_B_0x1)
#define DXO_DOP7_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITS_DXO_INT() p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_INT
#define Is__IPP_EVT_ITS_DXO_INT_B_0x0() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_INT == DXO_INT_B_0x0)
#define DXO_INT_B_0x0 0x0    //event disabled
#define Is__IPP_EVT_ITS_DXO_INT_B_0x1() (p_IPP_IP->IPP_EVT_ITS.IPP_EVT_ITS_ts.DXO_INT == DXO_INT_B_0x1)
#define DXO_INT_B_0x1 0x1    //event enabled
#define Get_IPP_IPP_EVT_ITS() p_IPP_IP->IPP_EVT_ITS.word


//IPP_EVT_ITS_BCLR


#define Set_IPP_IPP_EVT_ITS_BCLR_SD_ERROR(x) (p_IPP_IP->IPP_EVT_ITS_BCLR.IPP_EVT_ITS_BCLR_ts.SD_ERROR = x)


#define Set_IPP_IPP_EVT_ITS_BCLR_CD_ERROR(x) (p_IPP_IP->IPP_EVT_ITS_BCLR.IPP_EVT_ITS_BCLR_ts.CD_ERROR = x)


#define Set_IPP_IPP_EVT_ITS_BCLR_CD_RAW_EOF(x) (p_IPP_IP->IPP_EVT_ITS_BCLR.IPP_EVT_ITS_BCLR_ts.CD_RAW_EOF = x)


#define Set_IPP_IPP_EVT_ITS_BCLR_BML_EOF(x) (p_IPP_IP->IPP_EVT_ITS_BCLR.IPP_EVT_ITS_BCLR_ts.BML_EOF = x)


#define Set_IPP_IPP_EVT_ITS_BCLR_DXO_PDP(x) (p_IPP_IP->IPP_EVT_ITS_BCLR.IPP_EVT_ITS_BCLR_ts.DXO_PDP = x)


#define Set_IPP_IPP_EVT_ITS_BCLR_DXO_DPP(x) (p_IPP_IP->IPP_EVT_ITS_BCLR.IPP_EVT_ITS_BCLR_ts.DXO_DPP = x)


#define Set_IPP_IPP_EVT_ITS_BCLR_DXO_DOP7(x) (p_IPP_IP->IPP_EVT_ITS_BCLR.IPP_EVT_ITS_BCLR_ts.DXO_DOP7 = x)


#define Set_IPP_IPP_EVT_ITS_BCLR_DXO_INT(x) (p_IPP_IP->IPP_EVT_ITS_BCLR.IPP_EVT_ITS_BCLR_ts.DXO_INT = x)
#define Set_IPP_IPP_EVT_ITS_BCLR(SD_ERROR,CD_ERROR,CD_RAW_EOF,BML_EOF,DXO_PDP,DXO_DPP,DXO_DOP7,DXO_INT) (p_IPP_IP->IPP_EVT_ITS_BCLR.word = (uint16_t)SD_ERROR<<0 | (uint16_t)CD_ERROR<<1 | (uint16_t)CD_RAW_EOF<<2 | (uint16_t)BML_EOF<<3 | (uint16_t)DXO_PDP<<8 | (uint16_t)DXO_DPP<<9 | (uint16_t)DXO_DOP7<<10 | (uint16_t)DXO_INT<<11)
#define Set_IPP_IPP_EVT_ITS_BCLR_word(x) (p_IPP_IP->IPP_EVT_ITS_BCLR.word = x)


//IPP_EVT_ITS_BSET


#define Set_IPP_IPP_EVT_ITS_BSET_SD_ERROR(x) (p_IPP_IP->IPP_EVT_ITS_BSET.IPP_EVT_ITS_BSET_ts.SD_ERROR = x)


#define Set_IPP_IPP_EVT_ITS_BSET_CD_ERROR(x) (p_IPP_IP->IPP_EVT_ITS_BSET.IPP_EVT_ITS_BSET_ts.CD_ERROR = x)


#define Set_IPP_IPP_EVT_ITS_BSET_CD_RAW_EOF(x) (p_IPP_IP->IPP_EVT_ITS_BSET.IPP_EVT_ITS_BSET_ts.CD_RAW_EOF = x)


#define Set_IPP_IPP_EVT_ITS_BSET_BML_EOF(x) (p_IPP_IP->IPP_EVT_ITS_BSET.IPP_EVT_ITS_BSET_ts.BML_EOF = x)


#define Set_IPP_IPP_EVT_ITS_BSET_DXO_PDP(x) (p_IPP_IP->IPP_EVT_ITS_BSET.IPP_EVT_ITS_BSET_ts.DXO_PDP = x)


#define Set_IPP_IPP_EVT_ITS_BSET_DXO_DPP(x) (p_IPP_IP->IPP_EVT_ITS_BSET.IPP_EVT_ITS_BSET_ts.DXO_DPP = x)


#define Set_IPP_IPP_EVT_ITS_BSET_DXO_DOP7(x) (p_IPP_IP->IPP_EVT_ITS_BSET.IPP_EVT_ITS_BSET_ts.DXO_DOP7 = x)


#define Set_IPP_IPP_EVT_ITS_BSET_DXO_INT(x) (p_IPP_IP->IPP_EVT_ITS_BSET.IPP_EVT_ITS_BSET_ts.DXO_INT = x)
#define Set_IPP_IPP_EVT_ITS_BSET(SD_ERROR,CD_ERROR,CD_RAW_EOF,BML_EOF,DXO_PDP,DXO_DPP,DXO_DOP7,DXO_INT) (p_IPP_IP->IPP_EVT_ITS_BSET.word = (uint16_t)SD_ERROR<<0 | (uint16_t)CD_ERROR<<1 | (uint16_t)CD_RAW_EOF<<2 | (uint16_t)BML_EOF<<3 | (uint16_t)DXO_PDP<<8 | (uint16_t)DXO_DPP<<9 | (uint16_t)DXO_DOP7<<10 | (uint16_t)DXO_INT<<11)
#define Set_IPP_IPP_EVT_ITS_BSET_word(x) (p_IPP_IP->IPP_EVT_ITS_BSET.word = x)


//IPP_EVT_ITM


#define Get_IPP_IPP_EVT_ITM_SD_ERROR() p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.SD_ERROR
#define Is_IPP_IPP_EVT_ITM_SD_ERROR_B_0x0() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.SD_ERROR == SD_ERROR_B_0x0)
#define SD_ERROR_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_EVT_ITM_SD_ERROR_B_0x1() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.SD_ERROR == SD_ERROR_B_0x1)
#define SD_ERROR_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITM_CD_ERROR() p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.CD_ERROR
#define Is_IPP_IPP_EVT_ITM_CD_ERROR_B_0x0() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.CD_ERROR == CD_ERROR_B_0x0)
#define CD_ERROR_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_EVT_ITM_CD_ERROR_B_0x1() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.CD_ERROR == CD_ERROR_B_0x1)
#define CD_ERROR_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITM_CD_RAW_EOF() p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.CD_RAW_EOF
#define Is_IPP_IPP_EVT_ITM_CD_RAW_EOF_B_0x0() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.CD_RAW_EOF == CD_RAW_EOF_B_0x0)
#define CD_RAW_EOF_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_EVT_ITM_CD_RAW_EOF_B_0x1() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.CD_RAW_EOF == CD_RAW_EOF_B_0x1)
#define CD_RAW_EOF_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITM_BML_EOF() p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.BML_EOF
#define Is_IPP_IPP_EVT_ITM_BML_EOF_B_0x0() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.BML_EOF == BML_EOF_B_0x0)
#define BML_EOF_B_0x0 0x0    //event disabled
#define Is_IPP_IPP_EVT_ITM_BML_EOF_B_0x1() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.BML_EOF == BML_EOF_B_0x1)
#define BML_EOF_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITM_DXO_PDP() p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_PDP
#define Is__IPP_EVT_ITM_DXO_PDP_B_0x0() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_PDP == DXO_PDP_B_0x0)
#define DXO_PDP_B_0x0 0x0    //event disabled
#define Is__IPP_EVT_ITM_DXO_PDP_B_0x1() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_PDP == DXO_PDP_B_0x1)
#define DXO_PDP_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITM_DXO_DPP() p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_DPP
#define Is__IPP_EVT_ITM_DXO_DPP_B_0x0() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_DPP == DXO_DPP_B_0x0)
#define DXO_DPP_B_0x0 0x0    //event disabled
#define Is__IPP_EVT_ITM_DXO_DPP_B_0x1() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_DPP == DXO_DPP_B_0x1)
#define DXO_DPP_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITM_DXO_DOP7() p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_DOP7
#define Is__IPP_EVT_ITM_DXO_DOP7_B_0x0() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_DOP7 == DXO_DOP7_B_0x0)
#define DXO_DOP7_B_0x0 0x0    //event disabled
#define Is__IPP_EVT_ITM_DXO_DOP7_B_0x1() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_DOP7 == DXO_DOP7_B_0x1)
#define DXO_DOP7_B_0x1 0x1    //event enabled


#define Get_IPP_IPP_EVT_ITM_DXO_INT() p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_INT
#define Is__IPP_EVT_ITM_DXO_INT_B_0x0() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_INT == DXO_INT_B_0x0)
#define DXO_INT_B_0x0 0x0    //event disabled
#define Is__IPP_EVT_ITM_DXO_INT_B_0x1() (p_IPP_IP->IPP_EVT_ITM.IPP_EVT_ITM_ts.DXO_INT == DXO_INT_B_0x1)
#define DXO_INT_B_0x1 0x1    //event enabled
#define Get_IPP_IPP_EVT_ITM() p_IPP_IP->IPP_EVT_ITM.word


//IPP_EVT_ITM_BCLR


#define Set_IPP_IPP_EVT_ITM_BCLR_SD_ERROR(x) (p_IPP_IP->IPP_EVT_ITM_BCLR.IPP_EVT_ITM_BCLR_ts.SD_ERROR = x)


#define Set_IPP_IPP_EVT_ITM_BCLR_CD_ERROR(x) (p_IPP_IP->IPP_EVT_ITM_BCLR.IPP_EVT_ITM_BCLR_ts.CD_ERROR = x)


#define Set_IPP_IPP_EVT_ITM_BCLR_CD_RAW_EOF(x) (p_IPP_IP->IPP_EVT_ITM_BCLR.IPP_EVT_ITM_BCLR_ts.CD_RAW_EOF = x)


#define Set_IPP_IPP_EVT_ITM_BCLR_BML_EOF(x) (p_IPP_IP->IPP_EVT_ITM_BCLR.IPP_EVT_ITM_BCLR_ts.BML_EOF = x)


#define Set_IPP_IPP_EVT_ITM_BCLR_DXO_PDP(x) (p_IPP_IP->IPP_EVT_ITM_BCLR.IPP_EVT_ITM_BCLR_ts.DXO_PDP = x)


#define Set_IPP_IPP_EVT_ITM_BCLR_DXO_DPP(x) (p_IPP_IP->IPP_EVT_ITM_BCLR.IPP_EVT_ITM_BCLR_ts.DXO_DPP = x)


#define Set_IPP_IPP_EVT_ITM_BCLR_DXO_DOP7(x) (p_IPP_IP->IPP_EVT_ITM_BCLR.IPP_EVT_ITM_BCLR_ts.DXO_DOP7 = x)


#define Set_IPP_IPP_EVT_ITM_BCLR_DXO_INT(x) (p_IPP_IP->IPP_EVT_ITM_BCLR.IPP_EVT_ITM_BCLR_ts.DXO_INT = x)
#define Set_IPP_IPP_EVT_ITM_BCLR(SD_ERROR,CD_ERROR,CD_RAW_EOF,BML_EOF,DXO_PDP,DXO_DPP,DXO_DOP7,DXO_INT) (p_IPP_IP->IPP_EVT_ITM_BCLR.word = (uint16_t)SD_ERROR<<0 | (uint16_t)CD_ERROR<<1 | (uint16_t)CD_RAW_EOF<<2 | (uint16_t)BML_EOF<<3 | (uint16_t)DXO_PDP<<8 | (uint16_t)DXO_DPP<<9 | (uint16_t)DXO_DOP7<<10 | (uint16_t)DXO_INT<<11)
#define Set_IPP_IPP_EVT_ITM_BCLR_word(x) (p_IPP_IP->IPP_EVT_ITM_BCLR.word = x)


//IPP_EVT_ITM_BSET


#define Set_IPP_IPP_EVT_ITM_BSET_SD_ERROR(x) (p_IPP_IP->IPP_EVT_ITM_BSET.IPP_EVT_ITM_BSET_ts.SD_ERROR = x)


#define Set_IPP_IPP_EVT_ITM_BSET_CD_ERROR(x) (p_IPP_IP->IPP_EVT_ITM_BSET.IPP_EVT_ITM_BSET_ts.CD_ERROR = x)


#define Set_IPP_IPP_EVT_ITM_BSET_CD_RAW_EOF(x) (p_IPP_IP->IPP_EVT_ITM_BSET.IPP_EVT_ITM_BSET_ts.CD_RAW_EOF = x)


#define Set_IPP_IPP_EVT_ITM_BSET_BML_EOF(x) (p_IPP_IP->IPP_EVT_ITM_BSET.IPP_EVT_ITM_BSET_ts.BML_EOF = x)


#define Set_IPP_IPP_EVT_ITM_BSET_DXO_PDP(x) (p_IPP_IP->IPP_EVT_ITM_BSET.IPP_EVT_ITM_BSET_ts.DXO_PDP = x)


#define Set_IPP_IPP_EVT_ITM_BSET_DXO_DPP(x) (p_IPP_IP->IPP_EVT_ITM_BSET.IPP_EVT_ITM_BSET_ts.DXO_DPP = x)


#define Set_IPP_IPP_EVT_ITM_BSET_DXO_DOP7(x) (p_IPP_IP->IPP_EVT_ITM_BSET.IPP_EVT_ITM_BSET_ts.DXO_DOP7 = x)


#define Set_IPP_IPP_EVT_ITM_BSET_DXO_INT(x) (p_IPP_IP->IPP_EVT_ITM_BSET.IPP_EVT_ITM_BSET_ts.DXO_INT = x)
#define Set_IPP_IPP_EVT_ITM_BSET(SD_ERROR,CD_ERROR,CD_RAW_EOF,BML_EOF,DXO_PDP,DXO_DPP,DXO_DOP7,DXO_INT) (p_IPP_IP->IPP_EVT_ITM_BSET.word = (uint32_t)SD_ERROR<<0 | (uint32_t)CD_ERROR<<1 | (uint32_t)CD_RAW_EOF<<2 | (uint32_t)BML_EOF<<3 | (uint32_t)DXO_PDP<<8 | (uint32_t)DXO_DPP<<9 | (uint32_t)DXO_DOP7<<10 | (uint32_t)DXO_INT<<11)
#define Set_IPP_IPP_EVT_ITM_BSET_word(x) (p_IPP_IP->IPP_EVT_ITM_BSET.word = x)


//IPP_STATIC_TOP_IF_3D_SEL


#define Get_IPP_IPP_STATIC_TOP_IF_3D_SEL_STATIC_TOP_IF_3D_SEL() p_IPP_IP->IPP_STATIC_TOP_IF_3D_SEL.IPP_STATIC_TOP_IF_3D_SEL_ts.STATIC_TOP_IF_3D_SEL
#define Is_IPP_IPP_STATIC_TOP_IF_3D_SEL_STATIC_TOP_IF_3D_SEL_B_0x0() (p_IPP_IP->IPP_STATIC_TOP_IF_3D_SEL.IPP_STATIC_TOP_IF_3D_SEL_ts.STATIC_TOP_IF_3D_SEL == STATIC_TOP_IF_3D_SEL_B_0x0)
#define Set_IPP_IPP_STATIC_TOP_IF_3D_SEL_STATIC_TOP_IF_3D_SEL__B_0x0() (p_IPP_IP->IPP_STATIC_TOP_IF_3D_SEL.IPP_STATIC_TOP_IF_3D_SEL_ts.STATIC_TOP_IF_3D_SEL = STATIC_TOP_IF_3D_SEL_B_0x0)
#define STATIC_TOP_IF_3D_SEL_B_0x0 0x0    //CSI0 (primary interface)
#define Is_IPP_IPP_STATIC_TOP_IF_3D_SEL_STATIC_TOP_IF_3D_SEL_B_0x1() (p_IPP_IP->IPP_STATIC_TOP_IF_3D_SEL.IPP_STATIC_TOP_IF_3D_SEL_ts.STATIC_TOP_IF_3D_SEL == STATIC_TOP_IF_3D_SEL_B_0x1)
#define Set_IPP_IPP_STATIC_TOP_IF_3D_SEL_STATIC_TOP_IF_3D_SEL__B_0x1() (p_IPP_IP->IPP_STATIC_TOP_IF_3D_SEL.IPP_STATIC_TOP_IF_3D_SEL_ts.STATIC_TOP_IF_3D_SEL = STATIC_TOP_IF_3D_SEL_B_0x1)
#define STATIC_TOP_IF_3D_SEL_B_0x1 0x1    //CSI1 (secondary interface) 1x: no connexion
#define Set_IPP_IPP_STATIC_TOP_IF_3D_SEL(STATIC_TOP_IF_3D_SEL) (p_IPP_IP->IPP_STATIC_TOP_IF_3D_SEL.word = (uint16_t)STATIC_TOP_IF_3D_SEL<<0)
#define Get_IPP_IPP_STATIC_TOP_IF_3D_SEL() p_IPP_IP->IPP_STATIC_TOP_IF_3D_SEL.word
#define Set_IPP_IPP_STATIC_TOP_IF_3D_SEL_word(x) (p_IPP_IP->IPP_STATIC_TOP_IF_3D_SEL.word = x)


//IPP_CSI2RX_3D_VC_DATA_TYPE_R


#define Get_IPP_IPP_CSI2RX_3D_VC_DATA_TYPE_R_CSI2RX_3D_DATA_TYPE() p_IPP_IP->IPP_CSI2RX_3D_VC_DATA_TYPE_R.IPP_CSI2RX_3D_VC_DATA_TYPE_R_ts.CSI2RX_3D_DATA_TYPE


#define Get_IPP_IPP_CSI2RX_3D_VC_DATA_TYPE_R_CSI2RX_3D_VIRTUAL_CHANNEL() p_IPP_IP->IPP_CSI2RX_3D_VC_DATA_TYPE_R.IPP_CSI2RX_3D_VC_DATA_TYPE_R_ts.CSI2RX_3D_VIRTUAL_CHANNEL
#define Get_IPP_IPP_CSI2RX_3D_VC_DATA_TYPE_R() p_IPP_IP->IPP_CSI2RX_3D_VC_DATA_TYPE_R.word


//IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W


#define Get_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_STATIC_CSI2RX_3D_DATA_TYPE0() p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W.IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_ts.STATIC_CSI2RX_3D_DATA_TYPE0
#define Set_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_STATIC_CSI2RX_3D_DATA_TYPE0(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W.IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_ts.STATIC_CSI2RX_3D_DATA_TYPE0 = x)


#define Get_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_STATIC_CSI2RX_3D_DATA_TYPE1() p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W.IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_ts.STATIC_CSI2RX_3D_DATA_TYPE1
#define Set_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_STATIC_CSI2RX_3D_DATA_TYPE1(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W.IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_ts.STATIC_CSI2RX_3D_DATA_TYPE1 = x)
#define Set_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W(STATIC_CSI2RX_3D_DATA_TYPE0,STATIC_CSI2RX_3D_DATA_TYPE1) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W.word = (uint16_t)STATIC_CSI2RX_3D_DATA_TYPE0<<0 | (uint16_t)STATIC_CSI2RX_3D_DATA_TYPE1<<8)
#define Get_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W() p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W.word
#define Set_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_1_W.word = x)


//IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W


#define Get_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W_STATIC_CSI2RX_3D_DATA_TYPE2() p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W.IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W_ts.STATIC_CSI2RX_3D_DATA_TYPE2
#define Set_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W_STATIC_CSI2RX_3D_DATA_TYPE2(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W.IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W_ts.STATIC_CSI2RX_3D_DATA_TYPE2 = x)
#define Set_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W(STATIC_CSI2RX_3D_DATA_TYPE2) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W.word = (uint16_t)STATIC_CSI2RX_3D_DATA_TYPE2<<0)
#define Get_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W() p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W.word
#define Set_IPP_IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_DATA_TYPE_2_W.word = x)


//IPP_STATIC_CSI2RX_3D_VC_W


#define Get_IPP_IPP_STATIC_CSI2RX_3D_VC_W_STATIC_CSI2RX_3D_VC0_EN() p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.IPP_STATIC_CSI2RX_3D_VC_W_ts.STATIC_CSI2RX_3D_VC0_EN
#define Set_IPP_IPP_STATIC_CSI2RX_3D_VC_W_STATIC_CSI2RX_3D_VC0_EN(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.IPP_STATIC_CSI2RX_3D_VC_W_ts.STATIC_CSI2RX_3D_VC0_EN = x)


#define Get_IPP_IPP_STATIC_CSI2RX_3D_VC_W_STATIC_CSI2RX_3D_VC1_EN() p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.IPP_STATIC_CSI2RX_3D_VC_W_ts.STATIC_CSI2RX_3D_VC1_EN
#define Set_IPP_IPP_STATIC_CSI2RX_3D_VC_W_STATIC_CSI2RX_3D_VC1_EN(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.IPP_STATIC_CSI2RX_3D_VC_W_ts.STATIC_CSI2RX_3D_VC1_EN = x)


#define Get_IPP_IPP_STATIC_CSI2RX_3D_VC_W_STATIC_CSI2RX_3D_VC2_EN() p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.IPP_STATIC_CSI2RX_3D_VC_W_ts.STATIC_CSI2RX_3D_VC2_EN
#define Set_IPP_IPP_STATIC_CSI2RX_3D_VC_W_STATIC_CSI2RX_3D_VC2_EN(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.IPP_STATIC_CSI2RX_3D_VC_W_ts.STATIC_CSI2RX_3D_VC2_EN = x)


#define Get_IPP_IPP_STATIC_CSI2RX_3D_VC_W_STATIC_CSI2RX_3D_VC3_EN() p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.IPP_STATIC_CSI2RX_3D_VC_W_ts.STATIC_CSI2RX_3D_VC3_EN
#define Set_IPP_IPP_STATIC_CSI2RX_3D_VC_W_STATIC_CSI2RX_3D_VC3_EN(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.IPP_STATIC_CSI2RX_3D_VC_W_ts.STATIC_CSI2RX_3D_VC3_EN = x)
#define Set_IPP_IPP_STATIC_CSI2RX_3D_VC_W(STATIC_CSI2RX_3D_VC0_EN,STATIC_CSI2RX_3D_VC1_EN,STATIC_CSI2RX_3D_VC2_EN,STATIC_CSI2RX_3D_VC3_EN) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.word = (uint16_t)STATIC_CSI2RX_3D_VC0_EN<<0 | (uint16_t)STATIC_CSI2RX_3D_VC1_EN<<1 | (uint16_t)STATIC_CSI2RX_3D_VC2_EN<<2 | (uint16_t)STATIC_CSI2RX_3D_VC3_EN<<3)
#define Get_IPP_IPP_STATIC_CSI2RX_3D_VC_W() p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.word
#define Set_IPP_IPP_STATIC_CSI2RX_3D_VC_W_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_VC_W.word = x)


//IPP_CSI2RX_3D_FRAME_NB_R


#define Get_IPP_IPP_CSI2RX_3D_FRAME_NB_R_CSI2RX_3D_FRAME_NB() p_IPP_IP->IPP_CSI2RX_3D_FRAME_NB_R.IPP_CSI2RX_3D_FRAME_NB_R_ts.CSI2RX_3D_FRAME_NB
#define Get_IPP_IPP_CSI2RX_3D_FRAME_NB_R() p_IPP_IP->IPP_CSI2RX_3D_FRAME_NB_R.word


//IPP_CSI2RX_3D_DATA_FIELD_R


#define Get_IPP_IPP_CSI2RX_3D_DATA_FIELD_R_CSI2RX_3D_DATA_FIELD() p_IPP_IP->IPP_CSI2RX_3D_DATA_FIELD_R.IPP_CSI2RX_3D_DATA_FIELD_R_ts.CSI2RX_3D_DATA_FIELD
#define Get_IPP_IPP_CSI2RX_3D_DATA_FIELD_R() p_IPP_IP->IPP_CSI2RX_3D_DATA_FIELD_R.word


//IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W


#define Get_IPP_IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_STATIC_CSI2RX_3D_DATATYPE0_PIXWIDTH() p_IPP_IP->IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W.IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_ts.STATIC_CSI2RX_3D_DATATYPE0_PIXWIDTH
#define Set_IPP_IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_STATIC_CSI2RX_3D_DATATYPE0_PIXWIDTH(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W.IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_ts.STATIC_CSI2RX_3D_DATATYPE0_PIXWIDTH = x)


#define Get_IPP_IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_STATIC_CSI2RX_3D_DATATYPE1_PIXWIDTH() p_IPP_IP->IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W.IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_ts.STATIC_CSI2RX_3D_DATATYPE1_PIXWIDTH
#define Set_IPP_IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_STATIC_CSI2RX_3D_DATATYPE1_PIXWIDTH(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W.IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_ts.STATIC_CSI2RX_3D_DATATYPE1_PIXWIDTH = x)


#define Get_IPP_IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_STATIC_CSI2RX_3D_DATATYPE2_PIXWIDTH() p_IPP_IP->IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W.IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_ts.STATIC_CSI2RX_3D_DATATYPE2_PIXWIDTH
#define Set_IPP_IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_STATIC_CSI2RX_3D_DATATYPE2_PIXWIDTH(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W.IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_ts.STATIC_CSI2RX_3D_DATATYPE2_PIXWIDTH = x)
#define Set_IPP_IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W(STATIC_CSI2RX_3D_DATATYPE0_PIXWIDTH,STATIC_CSI2RX_3D_DATATYPE1_PIXWIDTH,STATIC_CSI2RX_3D_DATATYPE2_PIXWIDTH) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W.word = (uint16_t)STATIC_CSI2RX_3D_DATATYPE0_PIXWIDTH<<0 | (uint16_t)STATIC_CSI2RX_3D_DATATYPE1_PIXWIDTH<<4 | (uint16_t)STATIC_CSI2RX_3D_DATATYPE2_PIXWIDTH<<8)
#define Get_IPP_IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W() p_IPP_IP->IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W.word
#define Set_IPP_IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_3D_PIX_WIDTH_W.word = x)


//IPP_STATIC_DELAYM_EN


#define Get_IPP_IPP_STATIC_DELAYM_EN_STATIC_DELAYM_EN() p_IPP_IP->IPP_STATIC_DELAYM_EN.IPP_STATIC_DELAYM_EN_ts.STATIC_DELAYM_EN
#define Set_IPP_IPP_STATIC_DELAYM_EN_STATIC_DELAYM_EN(x) (p_IPP_IP->IPP_STATIC_DELAYM_EN.IPP_STATIC_DELAYM_EN_ts.STATIC_DELAYM_EN = x)
#define Set_IPP_IPP_STATIC_DELAYM_EN(STATIC_DELAYM_EN) (p_IPP_IP->IPP_STATIC_DELAYM_EN.word = (uint16_t)STATIC_DELAYM_EN<<0)
#define Get_IPP_IPP_STATIC_DELAYM_EN() p_IPP_IP->IPP_STATIC_DELAYM_EN.word
#define Set_IPP_IPP_STATIC_DELAYM_EN_word(x) (p_IPP_IP->IPP_STATIC_DELAYM_EN.word = x)


//IPP_DELAYM_LINE_DELAY


#define Get_IPP_IPP_DELAYM_LINE_DELAY_LINE_DELAY_COUNTER() p_IPP_IP->IPP_DELAYM_LINE_DELAY.IPP_DELAYM_LINE_DELAY_ts.LINE_DELAY_COUNTER


#define Get_IPP_IPP_DELAYM_LINE_DELAY_MASTER_ID() p_IPP_IP->IPP_DELAYM_LINE_DELAY.IPP_DELAYM_LINE_DELAY_ts.MASTER_ID
#define Is_IPP_IPP_DELAYM_LINE_DELAY_MASTER_ID_B_0x0() (p_IPP_IP->IPP_DELAYM_LINE_DELAY.IPP_DELAYM_LINE_DELAY_ts.MASTER_ID == MASTER_ID_B_0x0)
#define MASTER_ID_B_0x0 0x0    //CSI0 (primary interface)
#define Is_IPP_IPP_DELAYM_LINE_DELAY_MASTER_ID_B_0x2() (p_IPP_IP->IPP_DELAYM_LINE_DELAY.IPP_DELAYM_LINE_DELAY_ts.MASTER_ID == MASTER_ID_B_0x2)
#define MASTER_ID_B_0x2 0x2    //CSI1 (secondary interface)
#define Get_IPP_IPP_DELAYM_LINE_DELAY() p_IPP_IP->IPP_DELAYM_LINE_DELAY.word


//IPP_DELAYM_PIXEL_DELAY


#define Get_IPP_IPP_DELAYM_PIXEL_DELAY_PIXEL_DELAY_COUNTER() p_IPP_IP->IPP_DELAYM_PIXEL_DELAY.IPP_DELAYM_PIXEL_DELAY_ts.PIXEL_DELAY_COUNTER
#define Get_IPP_IPP_DELAYM_PIXEL_DELAY() p_IPP_IP->IPP_DELAYM_PIXEL_DELAY.word


//IPP_SD_STATIC_SDG_3D_CSI_MODE


#define Get_IPP_IPP_SD_STATIC_SDG_3D_CSI_MODE_STATIC_SDG_3D_CSI_MODE() p_IPP_IP->IPP_SD_STATIC_SDG_3D_CSI_MODE.IPP_SD_STATIC_SDG_3D_CSI_MODE_ts.STATIC_SDG_3D_CSI_MODE
#define Is_IPP_IPP_SD_STATIC_SDG_3D_CSI_MODE_STATIC_SDG_3D_CSI_MODE_B_0x0() (p_IPP_IP->IPP_SD_STATIC_SDG_3D_CSI_MODE.IPP_SD_STATIC_SDG_3D_CSI_MODE_ts.STATIC_SDG_3D_CSI_MODE == STATIC_SDG_3D_CSI_MODE_B_0x0)
#define Set_IPP_IPP_SD_STATIC_SDG_3D_CSI_MODE_STATIC_SDG_3D_CSI_MODE__B_0x0() (p_IPP_IP->IPP_SD_STATIC_SDG_3D_CSI_MODE.IPP_SD_STATIC_SDG_3D_CSI_MODE_ts.STATIC_SDG_3D_CSI_MODE = STATIC_SDG_3D_CSI_MODE_B_0x0)
#define STATIC_SDG_3D_CSI_MODE_B_0x0 0x0    //SMIA over CSI2 mode
#define Is_IPP_IPP_SD_STATIC_SDG_3D_CSI_MODE_STATIC_SDG_3D_CSI_MODE_B_0x1() (p_IPP_IP->IPP_SD_STATIC_SDG_3D_CSI_MODE.IPP_SD_STATIC_SDG_3D_CSI_MODE_ts.STATIC_SDG_3D_CSI_MODE == STATIC_SDG_3D_CSI_MODE_B_0x1)
#define Set_IPP_IPP_SD_STATIC_SDG_3D_CSI_MODE_STATIC_SDG_3D_CSI_MODE__B_0x1() (p_IPP_IP->IPP_SD_STATIC_SDG_3D_CSI_MODE.IPP_SD_STATIC_SDG_3D_CSI_MODE_ts.STATIC_SDG_3D_CSI_MODE = STATIC_SDG_3D_CSI_MODE_B_0x1)
#define STATIC_SDG_3D_CSI_MODE_B_0x1 0x1    //CSI2 specific mode
#define Set_IPP_IPP_SD_STATIC_SDG_3D_CSI_MODE(STATIC_SDG_3D_CSI_MODE) (p_IPP_IP->IPP_SD_STATIC_SDG_3D_CSI_MODE.word = (uint32_t)STATIC_SDG_3D_CSI_MODE<<0)
#define Get_IPP_IPP_SD_STATIC_SDG_3D_CSI_MODE() p_IPP_IP->IPP_SD_STATIC_SDG_3D_CSI_MODE.word
#define Set_IPP_IPP_SD_STATIC_SDG_3D_CSI_MODE_word(x) (p_IPP_IP->IPP_SD_STATIC_SDG_3D_CSI_MODE.word = x)


//IPP_CSI2_DPHY0_DL4_CTRL


#define Get_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_SWAP_PINS_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_SWAP_PINS_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_SWAP_PINS_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_SWAP_PINS_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_HS_INVERT_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_HS_INVERT_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_HS_INVERT_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_HS_INVERT_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_FORCE_RX_MODE_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_FORCE_RX_MODE_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_FORCE_RX_MODE_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_FORCE_RX_MODE_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_CD_OFF_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_CD_OFF_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_CD_OFF_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_CD_OFF_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_EOT_BYPASS_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_EOT_BYPASS_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_EOT_BYPASS_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_EOT_BYPASS_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_HSRX_TERM_SHIFT_UP_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_HSRX_TERM_SHIFT_UP_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_HSRX_TERM_SHIFT_UP_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_HSRX_TERM_SHIFT_DOWN_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_HSRX_TERM_SHIFT_DOWN_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_TEST_RESERVED_1_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_TEST_RESERVED_1_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL_CSI0_TEST_RESERVED_1_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.IPP_CSI2_DPHY0_DL4_CTRL_ts.CSI0_TEST_RESERVED_1_DL4 = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL(CSI0_SWAP_PINS_DL4,CSI0_HS_INVERT_DL4,CSI0_FORCE_RX_MODE_DL4,CSI0_CD_OFF_DL4,CSI0_EOT_BYPASS_DL4,CSI0_HSRX_TERM_SHIFT_UP_DL4,CSI0_HSRX_TERM_SHIFT_DOWN_DL4,CSI0_TEST_RESERVED_1_DL4) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.word = (uint16_t)CSI0_SWAP_PINS_DL4<<0 | (uint16_t)CSI0_HS_INVERT_DL4<<1 | (uint16_t)CSI0_FORCE_RX_MODE_DL4<<2 | (uint16_t)CSI0_CD_OFF_DL4<<3 | (uint16_t)CSI0_EOT_BYPASS_DL4<<4 | (uint16_t)CSI0_HSRX_TERM_SHIFT_UP_DL4<<5 | (uint16_t)CSI0_HSRX_TERM_SHIFT_DOWN_DL4<<6 | (uint16_t)CSI0_TEST_RESERVED_1_DL4<<7)
#define Get_IPP_IPP_CSI2_DPHY0_DL4_CTRL() p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.word
#define Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_CTRL.word = x)


//IPP_CSI2_DPHY0_DL4_DBG


#define Get_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_HS_RX_OFFSET_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_HS_RX_OFFSET_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_HS_RX_OFFSET_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_HS_RX_OFFSET_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_MIPI_IN_SHORT_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_MIPI_IN_SHORT_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_MIPI_IN_SHORT_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_MIPI_IN_SHORT_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_SKEW_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_SKEW_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_SKEW_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_SKEW_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_OVERSAMPLE_BYPASS_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_OVERSAMPLE_BYPASS_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_OVERSAMPLE_BYPASS_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_OVERSAMPLE_BYPASS_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_LP_HS_BYPASS_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_LP_HS_BYPASS_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_LP_HS_BYPASS_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_LP_HS_BYPASS_DL4 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_DIRECT_DYN_ACCES_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_DIRECT_DYN_ACCES_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DL4_DBG_CSI0_DIRECT_DYN_ACCES_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.IPP_CSI2_DPHY0_DL4_DBG_ts.CSI0_DIRECT_DYN_ACCES_DL4 = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_DBG(CSI0_HS_RX_OFFSET_DL4,CSI0_MIPI_IN_SHORT_DL4,CSI0_SKEW_DL4,CSI0_OVERSAMPLE_BYPASS_DL4,CSI0_LP_HS_BYPASS_DL4,CSI0_DIRECT_DYN_ACCES_DL4) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.word = (uint16_t)CSI0_HS_RX_OFFSET_DL4<<0 | (uint16_t)CSI0_MIPI_IN_SHORT_DL4<<3 | (uint16_t)CSI0_SKEW_DL4<<4 | (uint16_t)CSI0_OVERSAMPLE_BYPASS_DL4<<7 | (uint16_t)CSI0_LP_HS_BYPASS_DL4<<8 | (uint16_t)CSI0_DIRECT_DYN_ACCES_DL4<<11)
#define Get_IPP_IPP_CSI2_DPHY0_DL4_DBG() p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.word
#define Set_IPP_IPP_CSI2_DPHY0_DL4_DBG_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_DBG.word = x)


//IPP_CSI2_DPHY0_DBG2


#define Get_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_THSSETTLE_TIMER() p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_THSSETTLE_TIMER
#define Set_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_THSSETTLE_TIMER(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_THSSETTLE_TIMER = x)


#define Get_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_TIMER_SELECT_DL1() p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_TIMER_SELECT_DL1
#define Set_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_TIMER_SELECT_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_TIMER_SELECT_DL1 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_TIMER_SELECT_DL2() p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_TIMER_SELECT_DL2
#define Set_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_TIMER_SELECT_DL2(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_TIMER_SELECT_DL2 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_TIMER_SELECT_DL3() p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_TIMER_SELECT_DL3
#define Set_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_TIMER_SELECT_DL3(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_TIMER_SELECT_DL3 = x)


#define Get_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_TIMER_SELECT_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_TIMER_SELECT_DL4
#define Set_IPP_IPP_CSI2_DPHY0_DBG2_CSI0_TIMER_SELECT_DL4(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG2.IPP_CSI2_DPHY0_DBG2_ts.CSI0_TIMER_SELECT_DL4 = x)
#define Set_IPP_IPP_CSI2_DPHY0_DBG2(CSI0_THSSETTLE_TIMER,CSI0_TIMER_SELECT_DL1,CSI0_TIMER_SELECT_DL2,CSI0_TIMER_SELECT_DL3,CSI0_TIMER_SELECT_DL4) (p_IPP_IP->IPP_CSI2_DPHY0_DBG2.word = (uint16_t)CSI0_THSSETTLE_TIMER<<0 | (uint16_t)CSI0_TIMER_SELECT_DL1<<4 | (uint16_t)CSI0_TIMER_SELECT_DL2<<5 | (uint16_t)CSI0_TIMER_SELECT_DL3<<6 | (uint16_t)CSI0_TIMER_SELECT_DL4<<7)
#define Get_IPP_IPP_CSI2_DPHY0_DBG2() p_IPP_IP->IPP_CSI2_DPHY0_DBG2.word
#define Set_IPP_IPP_CSI2_DPHY0_DBG2_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DBG2.word = x)


//IPP_CSI2_DPHY1_DBG2


#define Get_IPP_IPP_CSI2_DPHY1_DBG2_CSI1_THSSETTLE_TIMER() p_IPP_IP->IPP_CSI2_DPHY1_DBG2.IPP_CSI2_DPHY1_DBG2_ts.CSI1_THSSETTLE_TIMER
#define Set_IPP_IPP_CSI2_DPHY1_DBG2_CSI1_THSSETTLE_TIMER(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG2.IPP_CSI2_DPHY1_DBG2_ts.CSI1_THSSETTLE_TIMER = x)


#define Get_IPP_IPP_CSI2_DPHY1_DBG2_CSI1_TIMER_SELECT_DL1() p_IPP_IP->IPP_CSI2_DPHY1_DBG2.IPP_CSI2_DPHY1_DBG2_ts.CSI1_TIMER_SELECT_DL1
#define Set_IPP_IPP_CSI2_DPHY1_DBG2_CSI1_TIMER_SELECT_DL1(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG2.IPP_CSI2_DPHY1_DBG2_ts.CSI1_TIMER_SELECT_DL1 = x)
#define Set_IPP_IPP_CSI2_DPHY1_DBG2(CSI1_THSSETTLE_TIMER,CSI1_TIMER_SELECT_DL1) (p_IPP_IP->IPP_CSI2_DPHY1_DBG2.word = (uint16_t)CSI1_THSSETTLE_TIMER<<0 | (uint16_t)CSI1_TIMER_SELECT_DL1<<4)
#define Get_IPP_IPP_CSI2_DPHY1_DBG2() p_IPP_IP->IPP_CSI2_DPHY1_DBG2.word
#define Set_IPP_IPP_CSI2_DPHY1_DBG2_word(x) (p_IPP_IP->IPP_CSI2_DPHY1_DBG2.word = x)


//IPP_CSI2_DPHY0_DL4_ERROR_ITS


#define Get_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_CSI0_ERR_EOT_SYNC_HS_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS.IPP_CSI2_DPHY0_DL4_ERROR_ITS_ts.CSI0_ERR_EOT_SYNC_HS_DL4


#define Get_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_CSI0_ERR_SOT_HS_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS.IPP_CSI2_DPHY0_DL4_ERROR_ITS_ts.CSI0_ERR_SOT_HS_DL4


#define Get_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_CSI0_ERR_SOT_SYNC_HS_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS.IPP_CSI2_DPHY0_DL4_ERROR_ITS_ts.CSI0_ERR_SOT_SYNC_HS_DL4


#define Get_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_CSI0_ERR_CONTROL_DL4() p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS.IPP_CSI2_DPHY0_DL4_ERROR_ITS_ts.CSI0_ERR_CONTROL_DL4
#define Get_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS() p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS.word


//IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR


#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR_CSI_DPHY_ERROR_ITS_BCLR(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR.IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR_ts.CSI_DPHY_ERROR_ITS_BCLR = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR(CSI_DPHY_ERROR_ITS_BCLR) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR.word = (uint16_t)CSI_DPHY_ERROR_ITS_BCLR<<0)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS_BCLR.word = x)


//IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET


#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET_CSI_DPHY_ERROR_ITS_BSET(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET.IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET_ts.CSI_DPHY_ERROR_ITS_BSET = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET(CSI_DPHY_ERROR_ITS_BSET) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET.word = (uint16_t)CSI_DPHY_ERROR_ITS_BSET<<0)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITS_BSET.word = x)


//IPP_CSI2_DPHY0_DL4_ERROR_ITM


#define Get_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITM_CSI_DPHY_ERROR_ITM() p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITM.IPP_CSI2_DPHY0_DL4_ERROR_ITM_ts.CSI_DPHY_ERROR_ITM
#define Get_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITM() p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITM.word


//IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR


#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR_CSI_DPHY_ERROR_ITM_BCLR(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR.IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR_ts.CSI_DPHY_ERROR_ITM_BCLR = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR(CSI_DPHY_ERROR_ITM_BCLR) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR.word = (uint16_t)CSI_DPHY_ERROR_ITM_BCLR<<0)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITM_BCLR.word = x)


//IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET


#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET_CSI_DPHY_ERROR_ITM_BSET(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET.IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET_ts.CSI_DPHY_ERROR_ITM_BSET = x)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET(CSI_DPHY_ERROR_ITM_BSET) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET.word = (uint32_t)CSI_DPHY_ERROR_ITM_BSET<<0)
#define Set_IPP_IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET_word(x) (p_IPP_IP->IPP_CSI2_DPHY0_DL4_ERROR_ITM_BSET.word = x)

//IPP_CSI2RX_VC_DATA_TYPE_R


#define Get_IPP_IPP_CSI2RX_VC_DATA_TYPE_R_CSI2RX_DATA_TYPE() p_IPP_IP->IPP_CSI2RX_VC_DATA_TYPE_R.IPP_CSI2RX_VC_DATA_TYPE_R_ts.CSI2RX_DATA_TYPE


#define Get_IPP_IPP_CSI2RX_VC_DATA_TYPE_R_CSI2RX_VIRTUAL_CHANNEL() p_IPP_IP->IPP_CSI2RX_VC_DATA_TYPE_R.IPP_CSI2RX_VC_DATA_TYPE_R_ts.CSI2RX_VIRTUAL_CHANNEL
#define Get_IPP_IPP_CSI2RX_VC_DATA_TYPE_R() p_IPP_IP->IPP_CSI2RX_VC_DATA_TYPE_R.word


//IPP_STATIC_CSI2RX_DATA_TYPE_1_W


#define Get_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_1_W_STATIC_CSI2RX_DATA_TYPE0() p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_1_W.IPP_STATIC_CSI2RX_DATA_TYPE_1_W_ts.STATIC_CSI2RX_DATA_TYPE0
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_1_W_STATIC_CSI2RX_DATA_TYPE0(x) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_1_W.IPP_STATIC_CSI2RX_DATA_TYPE_1_W_ts.STATIC_CSI2RX_DATA_TYPE0 = x)


#define Get_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_1_W_STATIC_CSI2RX_DATA_TYPE1() p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_1_W.IPP_STATIC_CSI2RX_DATA_TYPE_1_W_ts.STATIC_CSI2RX_DATA_TYPE1
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_1_W_STATIC_CSI2RX_DATA_TYPE1(x) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_1_W.IPP_STATIC_CSI2RX_DATA_TYPE_1_W_ts.STATIC_CSI2RX_DATA_TYPE1 = x)
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_1_W(STATIC_CSI2RX_DATA_TYPE0,STATIC_CSI2RX_DATA_TYPE1) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_1_W.word = (uint16_t)STATIC_CSI2RX_DATA_TYPE0<<0 | (uint16_t)STATIC_CSI2RX_DATA_TYPE1<<8)
#define Get_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_1_W() p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_1_W.word
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_1_W_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_1_W.word = x)


//IPP_STATIC_CSI2RX_DATA_TYPE_2_W


#define Get_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_2_W_STATIC_CSI2RX_DATA_TYPE2() p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_2_W.IPP_STATIC_CSI2RX_DATA_TYPE_2_W_ts.STATIC_CSI2RX_DATA_TYPE2
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_2_W_STATIC_CSI2RX_DATA_TYPE2(x) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_2_W.IPP_STATIC_CSI2RX_DATA_TYPE_2_W_ts.STATIC_CSI2RX_DATA_TYPE2 = x)
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_2_W(STATIC_CSI2RX_DATA_TYPE2) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_2_W.word = (uint16_t)STATIC_CSI2RX_DATA_TYPE2<<0)
#define Get_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_2_W() p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_2_W.word
#define Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_2_W_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_DATA_TYPE_2_W.word = x)


//IPP_STATIC_CSI2RX_VC_W


#define Get_IPP_IPP_STATIC_CSI2RX_VC_W_STATIC_CSI2RX_VC0_EN() p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.IPP_STATIC_CSI2RX_VC_W_ts.STATIC_CSI2RX_VC0_EN
#define Set_IPP_IPP_STATIC_CSI2RX_VC_W_STATIC_CSI2RX_VC0_EN(x) (p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.IPP_STATIC_CSI2RX_VC_W_ts.STATIC_CSI2RX_VC0_EN = x)


#define Get_IPP_IPP_STATIC_CSI2RX_VC_W_STATIC_CSI2RX_VC1_EN() p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.IPP_STATIC_CSI2RX_VC_W_ts.STATIC_CSI2RX_VC1_EN
#define Set_IPP_IPP_STATIC_CSI2RX_VC_W_STATIC_CSI2RX_VC1_EN(x) (p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.IPP_STATIC_CSI2RX_VC_W_ts.STATIC_CSI2RX_VC1_EN = x)


#define Get_IPP_IPP_STATIC_CSI2RX_VC_W_STATIC_CSI2RX_VC2_EN() p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.IPP_STATIC_CSI2RX_VC_W_ts.STATIC_CSI2RX_VC2_EN
#define Set_IPP_IPP_STATIC_CSI2RX_VC_W_STATIC_CSI2RX_VC2_EN(x) (p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.IPP_STATIC_CSI2RX_VC_W_ts.STATIC_CSI2RX_VC2_EN = x)


#define Get_IPP_IPP_STATIC_CSI2RX_VC_W_STATIC_CSI2RX_VC3_EN() p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.IPP_STATIC_CSI2RX_VC_W_ts.STATIC_CSI2RX_VC3_EN
#define Set_IPP_IPP_STATIC_CSI2RX_VC_W_STATIC_CSI2RX_VC3_EN(x) (p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.IPP_STATIC_CSI2RX_VC_W_ts.STATIC_CSI2RX_VC3_EN = x)
#define Set_IPP_IPP_STATIC_CSI2RX_VC_W(STATIC_CSI2RX_VC0_EN,STATIC_CSI2RX_VC1_EN,STATIC_CSI2RX_VC2_EN,STATIC_CSI2RX_VC3_EN) (p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.word = (uint16_t)STATIC_CSI2RX_VC0_EN<<0 | (uint16_t)STATIC_CSI2RX_VC1_EN<<1 | (uint16_t)STATIC_CSI2RX_VC2_EN<<2 | (uint16_t)STATIC_CSI2RX_VC3_EN<<3)
#define Get_IPP_IPP_STATIC_CSI2RX_VC_W() p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.word
#define Set_IPP_IPP_STATIC_CSI2RX_VC_W_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_VC_W.word = x)


//IPP_CSI2RX_FRAME_NB_R


#define Get_IPP_IPP_CSI2RX_FRAME_NB_R_CSI2RX_FRAME_NB() p_IPP_IP->IPP_CSI2RX_FRAME_NB_R.IPP_CSI2RX_FRAME_NB_R_ts.CSI2RX_FRAME_NB
#define Get_IPP_IPP_CSI2RX_FRAME_NB_R() p_IPP_IP->IPP_CSI2RX_FRAME_NB_R.word


//IPP_CSI2RX_DATA_FIELD_R


#define Get_IPP_IPP_CSI2RX_DATA_FIELD_R_CSI2RX_DATA_FIELD() p_IPP_IP->IPP_CSI2RX_DATA_FIELD_R.IPP_CSI2RX_DATA_FIELD_R_ts.CSI2RX_DATA_FIELD
#define Get_IPP_IPP_CSI2RX_DATA_FIELD_R() p_IPP_IP->IPP_CSI2RX_DATA_FIELD_R.word


//IPP_STATIC_CSI2RX_PIX_WIDTH_W_new


#define Get_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_STATIC_CSI2RX_DATATYPE0_PIXWIDTH() p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W_new.IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_ts.STATIC_CSI2RX_DATATYPE0_PIXWIDTH
#define Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_STATIC_CSI2RX_DATATYPE0_PIXWIDTH(x) (p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W_new.IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_ts.STATIC_CSI2RX_DATATYPE0_PIXWIDTH = x)


#define Get_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_STATIC_CSI2RX_DATATYPE1_PIXWIDTH() p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W_new.IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_ts.STATIC_CSI2RX_DATATYPE1_PIXWIDTH
#define Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_STATIC_CSI2RX_DATATYPE1_PIXWIDTH(x) (p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W_new.IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_ts.STATIC_CSI2RX_DATATYPE1_PIXWIDTH = x)


#define Get_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_STATIC_CSI2RX_DATATYPE2_PIXWIDTH() p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W_new.IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_ts.STATIC_CSI2RX_DATATYPE2_PIXWIDTH
#define Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_STATIC_CSI2RX_DATATYPE2_PIXWIDTH(x) (p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W_new.IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_ts.STATIC_CSI2RX_DATATYPE2_PIXWIDTH = x)
#define Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new(STATIC_CSI2RX_DATATYPE0_PIXWIDTH,STATIC_CSI2RX_DATATYPE1_PIXWIDTH,STATIC_CSI2RX_DATATYPE2_PIXWIDTH) (p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W_new.word = (uint32_t)STATIC_CSI2RX_DATATYPE0_PIXWIDTH<<0 | (uint32_t)STATIC_CSI2RX_DATATYPE1_PIXWIDTH<<4 | (uint32_t)STATIC_CSI2RX_DATATYPE2_PIXWIDTH<<8)
#define Get_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new() p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W_new.word
#define Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_word(x) (p_IPP_IP->IPP_STATIC_CSI2RX_PIX_WIDTH_W_new.word = x)

//IPP_DXO_DPP_ITM


#define Get_IPP_IPP_DXO_DPP_ITM_IPP_DXO_DPP_ENDOFBOOT_ITM() p_IPP_IP->IPP_DXO_DPP_ITM.IPP_DXO_DPP_ITM_ts.IPP_DXO_DPP_ENDOFBOOT_ITM


#define Get_IPP_IPP_DXO_DPP_ITM_IPP_DXO_DPP_ENDOFEXECCMD_ITM() p_IPP_IP->IPP_DXO_DPP_ITM.IPP_DXO_DPP_ITM_ts.IPP_DXO_DPP_ENDOFEXECCMD_ITM


#define Get_IPP_IPP_DXO_DPP_ITM_IPP_DXO_DPP_ENDOFPROCESSING_ITM() p_IPP_IP->IPP_DXO_DPP_ITM.IPP_DXO_DPP_ITM_ts.IPP_DXO_DPP_ENDOFPROCESSING_ITM


#define Get_IPP_IPP_DXO_DPP_ITM_IPP_DXO_DPP_NEWFRAMECMDACK_ITM() p_IPP_IP->IPP_DXO_DPP_ITM.IPP_DXO_DPP_ITM_ts.IPP_DXO_DPP_NEWFRAMECMDACK_ITM


#define Get_IPP_IPP_DXO_DPP_ITM_IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM() p_IPP_IP->IPP_DXO_DPP_ITM.IPP_DXO_DPP_ITM_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM


#define Get_IPP_IPP_DXO_DPP_ITM_IPP_DXO_DPP_NEWFRAMEERROR_ITM() p_IPP_IP->IPP_DXO_DPP_ITM.IPP_DXO_DPP_ITM_ts.IPP_DXO_DPP_NEWFRAMEERROR_ITM


#define Get_IPP_IPP_DXO_DPP_ITM_IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM() p_IPP_IP->IPP_DXO_DPP_ITM.IPP_DXO_DPP_ITM_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM


#define Get_IPP_IPP_DXO_DPP_ITM_IPP_DXO_DPP_PULSEOUT0_ITM() p_IPP_IP->IPP_DXO_DPP_ITM.IPP_DXO_DPP_ITM_ts.IPP_DXO_DPP_PULSEOUT0_ITM
#define Get_IPP_IPP_DXO_DPP_ITM() p_IPP_IP->IPP_DXO_DPP_ITM.word


//IPP_BML_ITS


#define Get_IPP_IPP_BML_ITS_BML_EOF() p_IPP_IP->IPP_BML_ITS.IPP_BML_ITS_ts.BML_EOF
#define Is_IPP_IPP_BML_ITS_BML_EOF_B_0x0() (p_IPP_IP->IPP_BML_ITS.IPP_BML_ITS_ts.BML_EOF == BML_EOF_B_0x0)
#define BML_EOF_B_0x0 0x0    //no BML EOF pending
#define Is_IPP_IPP_BML_ITS_BML_EOF_B_0x1() (p_IPP_IP->IPP_BML_ITS.IPP_BML_ITS_ts.BML_EOF == BML_EOF_B_0x1)
#define BML_EOF_B_0x1 0x1    //BML EOF pending
#define Get_IPP_IPP_BML_ITS() p_IPP_IP->IPP_BML_ITS.word


//IPP_BML_ITS_BCLR


#define Set_IPP_IPP_BML_ITS_BCLR_BML_EOF__B_0x0() (p_IPP_IP->IPP_BML_ITS_BCLR.IPP_BML_ITS_BCLR_ts.BML_EOF = BML_EOF_B_0x0)
#define BML_EOF_B_0x0 0x0    //no effect
#define Set_IPP_IPP_BML_ITS_BCLR_BML_EOF__B_0x1() (p_IPP_IP->IPP_BML_ITS_BCLR.IPP_BML_ITS_BCLR_ts.BML_EOF = BML_EOF_B_0x1)
#define BML_EOF_B_0x1 0x1    //clear status
#define Set_IPP_IPP_BML_ITS_BCLR(BML_EOF) (p_IPP_IP->IPP_BML_ITS_BCLR.word = (uint16_t)BML_EOF<<0)
#define Set_IPP_IPP_BML_ITS_BCLR_word(x) (p_IPP_IP->IPP_BML_ITS_BCLR.word = x)


//IPP_BML_ITS_BSET


#define Set_IPP_IPP_BML_ITS_BSET_BML_EOF__B_0x0() (p_IPP_IP->IPP_BML_ITS_BSET.IPP_BML_ITS_BSET_ts.BML_EOF = BML_EOF_B_0x0)
#define BML_EOF_B_0x0 0x0    //no effect
#define Set_IPP_IPP_BML_ITS_BSET_BML_EOF__B_0x1() (p_IPP_IP->IPP_BML_ITS_BSET.IPP_BML_ITS_BSET_ts.BML_EOF = BML_EOF_B_0x1)
#define BML_EOF_B_0x1 0x1    //set status
#define Set_IPP_IPP_BML_ITS_BSET(BML_EOF) (p_IPP_IP->IPP_BML_ITS_BSET.word = (uint16_t)BML_EOF<<0)
#define Set_IPP_IPP_BML_ITS_BSET_word(x) (p_IPP_IP->IPP_BML_ITS_BSET.word = x)


//IPP_BML_ITM


#define Get_IPP_IPP_BML_ITM_BML_EOF() p_IPP_IP->IPP_BML_ITM.IPP_BML_ITM_ts.BML_EOF
#define Is_IPP_IPP_BML_ITM_BML_EOF_B_0x0() (p_IPP_IP->IPP_BML_ITM.IPP_BML_ITM_ts.BML_EOF == BML_EOF_B_0x0)
#define BML_EOF_B_0x0 0x0    //BML EOF masked
#define Is_IPP_IPP_BML_ITM_BML_EOF_B_0x1() (p_IPP_IP->IPP_BML_ITM.IPP_BML_ITM_ts.BML_EOF == BML_EOF_B_0x1)
#define BML_EOF_B_0x1 0x1    //BML EOF unmasked
#define Get_IPP_IPP_BML_ITM() p_IPP_IP->IPP_BML_ITM.word


//IPP_BML_ITM_BCLR


#define Set_IPP_IPP_BML_ITM_BCLR_BML_EOF__B_0x0() (p_IPP_IP->IPP_BML_ITM_BCLR.IPP_BML_ITM_BCLR_ts.BML_EOF = BML_EOF_B_0x0)
#define BML_EOF_B_0x0 0x0    //no effect
#define Set_IPP_IPP_BML_ITM_BCLR_BML_EOF__B_0x1() (p_IPP_IP->IPP_BML_ITM_BCLR.IPP_BML_ITM_BCLR_ts.BML_EOF = BML_EOF_B_0x1)
#define BML_EOF_B_0x1 0x1    //clear mask
#define Set_IPP_IPP_BML_ITM_BCLR(BML_EOF) (p_IPP_IP->IPP_BML_ITM_BCLR.word = (uint16_t)BML_EOF<<0)
#define Set_IPP_IPP_BML_ITM_BCLR_word(x) (p_IPP_IP->IPP_BML_ITM_BCLR.word = x)


//IPP_BML_ITM_BSET


#define Set_IPP_IPP_BML_ITM_BSET_BML_EOF__B_0x0() (p_IPP_IP->IPP_BML_ITM_BSET.IPP_BML_ITM_BSET_ts.BML_EOF = BML_EOF_B_0x0)
#define BML_EOF_B_0x0 0x0    //no effect
#define Set_IPP_IPP_BML_ITM_BSET_BML_EOF__B_0x1() (p_IPP_IP->IPP_BML_ITM_BSET.IPP_BML_ITM_BSET_ts.BML_EOF = BML_EOF_B_0x1)
#define BML_EOF_B_0x1 0x1    //set mask
#define Set_IPP_IPP_BML_ITM_BSET(BML_EOF) (p_IPP_IP->IPP_BML_ITM_BSET.word = (uint32_t)BML_EOF<<0)
#define Set_IPP_IPP_BML_ITM_BSET_word(x) (p_IPP_IP->IPP_BML_ITM_BSET.word = x)


//IPP_SD_BMS_ISL_TOP_H


#define Get_IPP_IPP_SD_BMS_ISL_TOP_H_HEIGHT() p_IPP_IP->IPP_SD_BMS_ISL_TOP_H.IPP_SD_BMS_ISL_TOP_H_ts.HEIGHT
#define Set_IPP_IPP_SD_BMS_ISL_TOP_H_HEIGHT(x) (p_IPP_IP->IPP_SD_BMS_ISL_TOP_H.IPP_SD_BMS_ISL_TOP_H_ts.HEIGHT = x)
#define Set_IPP_IPP_SD_BMS_ISL_TOP_H(HEIGHT) (p_IPP_IP->IPP_SD_BMS_ISL_TOP_H.word = (uint16_t)HEIGHT<<0)
#define Get_IPP_IPP_SD_BMS_ISL_TOP_H() p_IPP_IP->IPP_SD_BMS_ISL_TOP_H.word
#define Set_IPP_IPP_SD_BMS_ISL_TOP_H_word(x) (p_IPP_IP->IPP_SD_BMS_ISL_TOP_H.word = x)


//IPP_SD_BMS_ISL_BOT_H


#define Get_IPP_IPP_SD_BMS_ISL_BOT_H_HEIGHT() p_IPP_IP->IPP_SD_BMS_ISL_BOT_H.IPP_SD_BMS_ISL_BOT_H_ts.HEIGHT
#define Set_IPP_IPP_SD_BMS_ISL_BOT_H_HEIGHT(x) (p_IPP_IP->IPP_SD_BMS_ISL_BOT_H.IPP_SD_BMS_ISL_BOT_H_ts.HEIGHT = x)
#define Set_IPP_IPP_SD_BMS_ISL_BOT_H(HEIGHT) (p_IPP_IP->IPP_SD_BMS_ISL_BOT_H.word = (uint16_t)HEIGHT<<0)
#define Get_IPP_IPP_SD_BMS_ISL_BOT_H() p_IPP_IP->IPP_SD_BMS_ISL_BOT_H.word
#define Set_IPP_IPP_SD_BMS_ISL_BOT_H_word(x) (p_IPP_IP->IPP_SD_BMS_ISL_BOT_H.word = x)


//IPP_SD_BMS_ISL_FILTER


#define Get_IPP_IPP_SD_BMS_ISL_FILTER_ISL_BOT() p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_BOT
#define Is_IPP_IPP_SD_BMS_ISL_FILTER_ISL_BOT_B_0x0() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_BOT == ISL_BOT_B_0x0)
#define Set_IPP_IPP_SD_BMS_ISL_FILTER_ISL_BOT__B_0x0() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_BOT = ISL_BOT_B_0x0)
#define ISL_BOT_B_0x0 0x0    //ISL_BOT ignored
#define Is_IPP_IPP_SD_BMS_ISL_FILTER_ISL_BOT_B_0x1() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_BOT == ISL_BOT_B_0x1)
#define Set_IPP_IPP_SD_BMS_ISL_FILTER_ISL_BOT__B_0x1() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_BOT = ISL_BOT_B_0x1)
#define ISL_BOT_B_0x1 0x1    //ISL_BOT captured


#define Get_IPP_IPP_SD_BMS_ISL_FILTER_PIX_DATA() p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.PIX_DATA
#define Is_IPP_IPP_SD_BMS_ISL_FILTER_PIX_DATA_B_0x0() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.PIX_DATA == PIX_DATA_B_0x0)
#define Set_IPP_IPP_SD_BMS_ISL_FILTER_PIX_DATA__B_0x0() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.PIX_DATA = PIX_DATA_B_0x0)
#define PIX_DATA_B_0x0 0x0    //PIX_DATA ignored
#define Is_IPP_IPP_SD_BMS_ISL_FILTER_PIX_DATA_B_0x1() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.PIX_DATA == PIX_DATA_B_0x1)
#define Set_IPP_IPP_SD_BMS_ISL_FILTER_PIX_DATA__B_0x1() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.PIX_DATA = PIX_DATA_B_0x1)
#define PIX_DATA_B_0x1 0x1    //PIX_DATA captured


#define Get_IPP_IPP_SD_BMS_ISL_FILTER_ISL_TOP() p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_TOP
#define Is_IPP_IPP_SD_BMS_ISL_FILTER_ISL_TOP_B_0x0() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_TOP == ISL_TOP_B_0x0)
#define Set_IPP_IPP_SD_BMS_ISL_FILTER_ISL_TOP__B_0x0() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_TOP = ISL_TOP_B_0x0)
#define ISL_TOP_B_0x0 0x0    //ISL_TOP ignored
#define Is_IPP_IPP_SD_BMS_ISL_FILTER_ISL_TOP_B_0x1() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_TOP == ISL_TOP_B_0x1)
#define Set_IPP_IPP_SD_BMS_ISL_FILTER_ISL_TOP__B_0x1() (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.IPP_SD_BMS_ISL_FILTER_ts.ISL_TOP = ISL_TOP_B_0x1)
#define ISL_TOP_B_0x1 0x1    //ISL_TOP captured
#define Set_IPP_IPP_SD_BMS_ISL_FILTER(ISL_BOT,PIX_DATA,ISL_TOP) (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.word = (uint16_t)ISL_BOT<<0 | (uint16_t)PIX_DATA<<1 | (uint16_t)ISL_TOP<<2)
#define Get_IPP_IPP_SD_BMS_ISL_FILTER() p_IPP_IP->IPP_SD_BMS_ISL_FILTER.word
#define Set_IPP_IPP_SD_BMS_ISL_FILTER_word(x) (p_IPP_IP->IPP_SD_BMS_ISL_FILTER.word = x)


//IPP_SD_BMS_ISL_FMT


#define Get_IPP_IPP_SD_BMS_ISL_FMT_FORMAT() p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT
#define Is_IPP_IPP_SD_BMS_ISL_FMT_FORMAT_B_0x6() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT == FORMAT_B_0x6)
#define Set_IPP_IPP_SD_BMS_ISL_FMT_FORMAT__B_0x6() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT = FORMAT_B_0x6)
#define FORMAT_B_0x6 0x6    //RAW6
#define Is_IPP_IPP_SD_BMS_ISL_FMT_FORMAT_B_0x7() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT == FORMAT_B_0x7)
#define Set_IPP_IPP_SD_BMS_ISL_FMT_FORMAT__B_0x7() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT = FORMAT_B_0x7)
#define FORMAT_B_0x7 0x7    //RAW7
#define Is_IPP_IPP_SD_BMS_ISL_FMT_FORMAT_B_0x8() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT == FORMAT_B_0x8)
#define Set_IPP_IPP_SD_BMS_ISL_FMT_FORMAT__B_0x8() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT = FORMAT_B_0x8)
#define FORMAT_B_0x8 0x8    //RAW8
#define Is_IPP_IPP_SD_BMS_ISL_FMT_FORMAT_B_0xA() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT == FORMAT_B_0xA)
#define Set_IPP_IPP_SD_BMS_ISL_FMT_FORMAT__B_0xA() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT = FORMAT_B_0xA)
#define FORMAT_B_0xA 0xA    //RAW10
#define Is_IPP_IPP_SD_BMS_ISL_FMT_FORMAT_B_0xC() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT == FORMAT_B_0xC)
#define Set_IPP_IPP_SD_BMS_ISL_FMT_FORMAT__B_0xC() (p_IPP_IP->IPP_SD_BMS_ISL_FMT.IPP_SD_BMS_ISL_FMT_ts.FORMAT = FORMAT_B_0xC)
#define FORMAT_B_0xC 0xC    //RAW12
#define Set_IPP_IPP_SD_BMS_ISL_FMT(FORMAT) (p_IPP_IP->IPP_SD_BMS_ISL_FMT.word = (uint16_t)FORMAT<<0)
#define Get_IPP_IPP_SD_BMS_ISL_FMT() p_IPP_IP->IPP_SD_BMS_ISL_FMT.word
#define Set_IPP_IPP_SD_BMS_ISL_FMT_word(x) (p_IPP_IP->IPP_SD_BMS_ISL_FMT.word = x)

//IPP_SD_DXO_DATAPATH


#define Get_IPP_IPP_SD_DXO_DATAPATH_ISP_IN() p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.ISP_IN
#define Is__IPP_SD_DXO_DATAPATH_ISP_IN_B_0x0() (p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.ISP_IN == ISP_IN_B_0x0)
#define Set_IPP_IPP_SD_DXO_DATAPATH_ISP_IN__B_0x0() (p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.ISP_IN = ISP_IN_B_0x0)
#define ISP_IN_B_0x0 0x0    //BML
#define Is__IPP_SD_DXO_DATAPATH_ISP_IN_B_0x1() (p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.ISP_IN == ISP_IN_B_0x1)
#define Set_IPP_IPP_SD_DXO_DATAPATH_ISP_IN__B_0x1() (p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.ISP_IN = ISP_IN_B_0x1)
#define ISP_IN_B_0x1 0x1    //DxO IP


#define Get_IPP_IPP_SD_DXO_DATAPATH_DXO_IN() p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.DXO_IN
#define Is__IPP_SD_DXO_DATAPATH_DXO_IN_B_0x0() (p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.DXO_IN == DXO_IN_B_0x0)
#define Set_IPP_IPP_SD_DXO_DATAPATH_DXO_IN__B_0x0() (p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.DXO_IN = DXO_IN_B_0x0)
#define DXO_IN_B_0x0 0x0    //ISP
#define Is__IPP_SD_DXO_DATAPATH_DXO_IN_B_0x1() (p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.DXO_IN == DXO_IN_B_0x1)
#define Set_IPP_IPP_SD_DXO_DATAPATH_DXO_IN__B_0x1() (p_IPP_IP->IPP_SD_DXO_DATAPATH.IPP_SD_DXO_DATAPATH_ts.DXO_IN = DXO_IN_B_0x1)
#define DXO_IN_B_0x1 0x1    //BML
#define Set_IPP_IPP_SD_DXO_DATAPATH(ISP_IN,DXO_IN) (p_IPP_IP->IPP_SD_DXO_DATAPATH.word = (uint16_t)ISP_IN<<0 | (uint16_t)DXO_IN<<8)
#define Get_IPP_IPP_SD_DXO_DATAPATH() p_IPP_IP->IPP_SD_DXO_DATAPATH.word
#define Set_IPP_IPP_SD_DXO_DATAPATH_word(x) (p_IPP_IP->IPP_SD_DXO_DATAPATH.word = x)


//IPP_DXO_PDP_ITS


#define Get_IPP_IPP_DXO_PDP_ITS_IPP_DXO_PDP_ENDOFBOOT_ITS() p_IPP_IP->IPP_DXO_PDP_ITS.IPP_DXO_PDP_ITS_ts.IPP_DXO_PDP_ENDOFBOOT_ITS


#define Get_IPP_IPP_DXO_PDP_ITS_IPP_DXO_PDP_ENDOFEXECCMD_ITS() p_IPP_IP->IPP_DXO_PDP_ITS.IPP_DXO_PDP_ITS_ts.IPP_DXO_PDP_ENDOFEXECCMD_ITS


#define Get_IPP_IPP_DXO_PDP_ITS_IPP_DXO_PDP_ENDOFPROCESSING_ITS() p_IPP_IP->IPP_DXO_PDP_ITS.IPP_DXO_PDP_ITS_ts.IPP_DXO_PDP_ENDOFPROCESSING_ITS


#define Get_IPP_IPP_DXO_PDP_ITS_IPP_DXO_PDP_NEWFRAMECMDACK_ITS() p_IPP_IP->IPP_DXO_PDP_ITS.IPP_DXO_PDP_ITS_ts.IPP_DXO_PDP_NEWFRAMECMDACK_ITS


#define Get_IPP_IPP_DXO_PDP_ITS_IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS() p_IPP_IP->IPP_DXO_PDP_ITS.IPP_DXO_PDP_ITS_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS


#define Get_IPP_IPP_DXO_PDP_ITS_IPP_DXO_PDP_NEWFRAMEERROR_ITS() p_IPP_IP->IPP_DXO_PDP_ITS.IPP_DXO_PDP_ITS_ts.IPP_DXO_PDP_NEWFRAMEERROR_ITS


#define Get_IPP_IPP_DXO_PDP_ITS_IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS() p_IPP_IP->IPP_DXO_PDP_ITS.IPP_DXO_PDP_ITS_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS


#define Get_IPP_IPP_DXO_PDP_ITS_IPP_DXO_PDP_PULSEOUT0_ITS() p_IPP_IP->IPP_DXO_PDP_ITS.IPP_DXO_PDP_ITS_ts.IPP_DXO_PDP_PULSEOUT0_ITS
#define Get_IPP_IPP_DXO_PDP_ITS() p_IPP_IP->IPP_DXO_PDP_ITS.word


//IPP_DXO_PDP_ITS_BCLR


#define Set_IPP_IPP_DXO_PDP_ITS_BCLR_IPP_DXO_PDP_ENDOFBOOT_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.IPP_DXO_PDP_ITS_BCLR_ts.IPP_DXO_PDP_ENDOFBOOT_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BCLR_IPP_DXO_PDP_ENDOFEXECCMD_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.IPP_DXO_PDP_ITS_BCLR_ts.IPP_DXO_PDP_ENDOFEXECCMD_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BCLR_IPP_DXO_PDP_ENDOFPROCESSING_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.IPP_DXO_PDP_ITS_BCLR_ts.IPP_DXO_PDP_ENDOFPROCESSING_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BCLR_IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.IPP_DXO_PDP_ITS_BCLR_ts.IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BCLR_IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.IPP_DXO_PDP_ITS_BCLR_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BCLR_IPP_DXO_PDP_NEWFRAMEERROR_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.IPP_DXO_PDP_ITS_BCLR_ts.IPP_DXO_PDP_NEWFRAMEERROR_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BCLR_IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.IPP_DXO_PDP_ITS_BCLR_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BCLR_IPP_DXO_PDP_PULSEOUT0_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.IPP_DXO_PDP_ITS_BCLR_ts.IPP_DXO_PDP_PULSEOUT0_ITS_BCLR = x)
#define Set_IPP_IPP_DXO_PDP_ITS_BCLR(IPP_DXO_PDP_ENDOFBOOT_ITS_BCLR,IPP_DXO_PDP_ENDOFEXECCMD_ITS_BCLR,IPP_DXO_PDP_ENDOFPROCESSING_ITS_BCLR,IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BCLR,IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BCLR,IPP_DXO_PDP_NEWFRAMEERROR_ITS_BCLR,IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BCLR,IPP_DXO_PDP_PULSEOUT0_ITS_BCLR) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.word = (uint16_t)IPP_DXO_PDP_ENDOFBOOT_ITS_BCLR<<0 | (uint16_t)IPP_DXO_PDP_ENDOFEXECCMD_ITS_BCLR<<1 | (uint16_t)IPP_DXO_PDP_ENDOFPROCESSING_ITS_BCLR<<2 | (uint16_t)IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BCLR<<3 | (uint16_t)IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BCLR<<4 | (uint16_t)IPP_DXO_PDP_NEWFRAMEERROR_ITS_BCLR<<5 | (uint16_t)IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BCLR<<6 | (uint16_t)IPP_DXO_PDP_PULSEOUT0_ITS_BCLR<<7)
#define Set_IPP_IPP_DXO_PDP_ITS_BCLR_word(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BCLR.word = x)


//IPP_DXO_PDP_ITS_BSET


#define Set_IPP_IPP_DXO_PDP_ITS_BSET_IPP_DXO_PDP_ENDOFBOOT_ITS_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.IPP_DXO_PDP_ITS_BSET_ts.IPP_DXO_PDP_ENDOFBOOT_ITS_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BSET_IPP_DXO_PDP_ENDOFEXECCMD_ITS_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.IPP_DXO_PDP_ITS_BSET_ts.IPP_DXO_PDP_ENDOFEXECCMD_ITS_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BSET_IPP_DXO_PDP_ENDOFPROCESSING_ITS_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.IPP_DXO_PDP_ITS_BSET_ts.IPP_DXO_PDP_ENDOFPROCESSING_ITS_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BSET_IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.IPP_DXO_PDP_ITS_BSET_ts.IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BSET_IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.IPP_DXO_PDP_ITS_BSET_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BSET_IPP_DXO_PDP_NEWFRAMEERROR_ITS_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.IPP_DXO_PDP_ITS_BSET_ts.IPP_DXO_PDP_NEWFRAMEERROR_ITS_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BSET_IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.IPP_DXO_PDP_ITS_BSET_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITS_BSET_IPP_DXO_PDP_PULSEOUT0_ITS_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.IPP_DXO_PDP_ITS_BSET_ts.IPP_DXO_PDP_PULSEOUT0_ITS_BSET = x)
#define Set_IPP_IPP_DXO_PDP_ITS_BSET(IPP_DXO_PDP_ENDOFBOOT_ITS_BSET,IPP_DXO_PDP_ENDOFEXECCMD_ITS_BSET,IPP_DXO_PDP_ENDOFPROCESSING_ITS_BSET,IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BSET,IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BSET,IPP_DXO_PDP_NEWFRAMEERROR_ITS_BSET,IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BSET,IPP_DXO_PDP_PULSEOUT0_ITS_BSET) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.word = (uint16_t)IPP_DXO_PDP_ENDOFBOOT_ITS_BSET<<0 | (uint16_t)IPP_DXO_PDP_ENDOFEXECCMD_ITS_BSET<<1 | (uint16_t)IPP_DXO_PDP_ENDOFPROCESSING_ITS_BSET<<2 | (uint16_t)IPP_DXO_PDP_NEWFRAMECMDACK_ITS_BSET<<3 | (uint16_t)IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITS_BSET<<4 | (uint16_t)IPP_DXO_PDP_NEWFRAMEERROR_ITS_BSET<<5 | (uint16_t)IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITS_BSET<<6 | (uint16_t)IPP_DXO_PDP_PULSEOUT0_ITS_BSET<<7)
#define Set_IPP_IPP_DXO_PDP_ITS_BSET_word(x) (p_IPP_IP->IPP_DXO_PDP_ITS_BSET.word = x)


//IPP_DXO_PDP_ITM


#define Get_IPP_IPP_DXO_PDP_ITM_IPP_DXO_PDP_ENDOFBOOT_ITM() p_IPP_IP->IPP_DXO_PDP_ITM.IPP_DXO_PDP_ITM_ts.IPP_DXO_PDP_ENDOFBOOT_ITM


#define Get_IPP_IPP_DXO_PDP_ITM_IPP_DXO_PDP_ENDOFEXECCMD_ITM() p_IPP_IP->IPP_DXO_PDP_ITM.IPP_DXO_PDP_ITM_ts.IPP_DXO_PDP_ENDOFEXECCMD_ITM


#define Get_IPP_IPP_DXO_PDP_ITM_IPP_DXO_PDP_ENDOFPROCESSING_ITM() p_IPP_IP->IPP_DXO_PDP_ITM.IPP_DXO_PDP_ITM_ts.IPP_DXO_PDP_ENDOFPROCESSING_ITM


#define Get_IPP_IPP_DXO_PDP_ITM_IPP_DXO_PDP_NEWFRAMECMDACK_ITM() p_IPP_IP->IPP_DXO_PDP_ITM.IPP_DXO_PDP_ITM_ts.IPP_DXO_PDP_NEWFRAMECMDACK_ITM


#define Get_IPP_IPP_DXO_PDP_ITM_IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM() p_IPP_IP->IPP_DXO_PDP_ITM.IPP_DXO_PDP_ITM_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM


#define Get_IPP_IPP_DXO_PDP_ITM_IPP_DXO_PDP_NEWFRAMEERROR_ITM() p_IPP_IP->IPP_DXO_PDP_ITM.IPP_DXO_PDP_ITM_ts.IPP_DXO_PDP_NEWFRAMEERROR_ITM


#define Get_IPP_IPP_DXO_PDP_ITM_IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM() p_IPP_IP->IPP_DXO_PDP_ITM.IPP_DXO_PDP_ITM_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM


#define Get_IPP_IPP_DXO_PDP_ITM_IPP_DXO_PDP_PULSEOUT0_ITM() p_IPP_IP->IPP_DXO_PDP_ITM.IPP_DXO_PDP_ITM_ts.IPP_DXO_PDP_PULSEOUT0_ITM
#define Get_IPP_IPP_DXO_PDP_ITM() p_IPP_IP->IPP_DXO_PDP_ITM.word


//IPP_DXO_PDP_ITM_BCLR


#define Set_IPP_IPP_DXO_PDP_ITM_BCLR_IPP_DXO_PDP_ENDOFBOOT_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.IPP_DXO_PDP_ITM_BCLR_ts.IPP_DXO_PDP_ENDOFBOOT_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BCLR_IPP_DXO_PDP_ENDOFEXECCMD_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.IPP_DXO_PDP_ITM_BCLR_ts.IPP_DXO_PDP_ENDOFEXECCMD_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BCLR_IPP_DXO_PDP_ENDOFPROCESSING_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.IPP_DXO_PDP_ITM_BCLR_ts.IPP_DXO_PDP_ENDOFPROCESSING_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BCLR_IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.IPP_DXO_PDP_ITM_BCLR_ts.IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BCLR_IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.IPP_DXO_PDP_ITM_BCLR_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BCLR_IPP_DXO_PDP_NEWFRAMEERROR_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.IPP_DXO_PDP_ITM_BCLR_ts.IPP_DXO_PDP_NEWFRAMEERROR_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BCLR_IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.IPP_DXO_PDP_ITM_BCLR_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BCLR_IPP_DXO_PDP_PULSEOUT0_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.IPP_DXO_PDP_ITM_BCLR_ts.IPP_DXO_PDP_PULSEOUT0_ITM_BCLR = x)
#define Set_IPP_IPP_DXO_PDP_ITM_BCLR(IPP_DXO_PDP_ENDOFBOOT_ITM_BCLR,IPP_DXO_PDP_ENDOFEXECCMD_ITM_BCLR,IPP_DXO_PDP_ENDOFPROCESSING_ITM_BCLR,IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BCLR,IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BCLR,IPP_DXO_PDP_NEWFRAMEERROR_ITM_BCLR,IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BCLR,IPP_DXO_PDP_PULSEOUT0_ITM_BCLR) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.word = (uint16_t)IPP_DXO_PDP_ENDOFBOOT_ITM_BCLR<<0 | (uint16_t)IPP_DXO_PDP_ENDOFEXECCMD_ITM_BCLR<<1 | (uint16_t)IPP_DXO_PDP_ENDOFPROCESSING_ITM_BCLR<<2 | (uint16_t)IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BCLR<<3 | (uint16_t)IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BCLR<<4 | (uint16_t)IPP_DXO_PDP_NEWFRAMEERROR_ITM_BCLR<<5 | (uint16_t)IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BCLR<<6 | (uint16_t)IPP_DXO_PDP_PULSEOUT0_ITM_BCLR<<7)
#define Set_IPP_IPP_DXO_PDP_ITM_BCLR_word(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BCLR.word = x)


//IPP_DXO_PDP_ITM_BSET


#define Set_IPP_IPP_DXO_PDP_ITM_BSET_IPP_DXO_PDP_ENDOFBOOT_ITM_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.IPP_DXO_PDP_ITM_BSET_ts.IPP_DXO_PDP_ENDOFBOOT_ITM_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BSET_IPP_DXO_PDP_ENDOFEXECCMD_ITM_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.IPP_DXO_PDP_ITM_BSET_ts.IPP_DXO_PDP_ENDOFEXECCMD_ITM_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BSET_IPP_DXO_PDP_ENDOFPROCESSING_ITM_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.IPP_DXO_PDP_ITM_BSET_ts.IPP_DXO_PDP_ENDOFPROCESSING_ITM_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BSET_IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.IPP_DXO_PDP_ITM_BSET_ts.IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BSET_IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.IPP_DXO_PDP_ITM_BSET_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BSET_IPP_DXO_PDP_NEWFRAMEERROR_ITM_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.IPP_DXO_PDP_ITM_BSET_ts.IPP_DXO_PDP_NEWFRAMEERROR_ITM_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BSET_IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.IPP_DXO_PDP_ITM_BSET_ts.IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BSET = x)


#define Set_IPP_IPP_DXO_PDP_ITM_BSET_IPP_DXO_PDP_PULSEOUT0_ITM_BSET(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.IPP_DXO_PDP_ITM_BSET_ts.IPP_DXO_PDP_PULSEOUT0_ITM_BSET = x)
#define Set_IPP_IPP_DXO_PDP_ITM_BSET(IPP_DXO_PDP_ENDOFBOOT_ITM_BSET,IPP_DXO_PDP_ENDOFEXECCMD_ITM_BSET,IPP_DXO_PDP_ENDOFPROCESSING_ITM_BSET,IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BSET,IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BSET,IPP_DXO_PDP_NEWFRAMEERROR_ITM_BSET,IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BSET,IPP_DXO_PDP_PULSEOUT0_ITM_BSET) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.word = (uint16_t)IPP_DXO_PDP_ENDOFBOOT_ITM_BSET<<0 | (uint16_t)IPP_DXO_PDP_ENDOFEXECCMD_ITM_BSET<<1 | (uint16_t)IPP_DXO_PDP_ENDOFPROCESSING_ITM_BSET<<2 | (uint16_t)IPP_DXO_PDP_NEWFRAMECMDACK_ITM_BSET<<3 | (uint16_t)IPP_DXO_PDP_NEWFRAMEPROCESSINGACK_ITM_BSET<<4 | (uint16_t)IPP_DXO_PDP_NEWFRAMEERROR_ITM_BSET<<5 | (uint16_t)IPP_DXO_PDP_NEWFRAMEPROCESSINGERROR_ITM_BSET<<6 | (uint16_t)IPP_DXO_PDP_PULSEOUT0_ITM_BSET<<7)
#define Set_IPP_IPP_DXO_PDP_ITM_BSET_word(x) (p_IPP_IP->IPP_DXO_PDP_ITM_BSET.word = x)


//IPP_DXO_DPP_ITS


#define Get_IPP_IPP_DXO_DPP_ITS_IPP_DXO_DPP_ENDOFBOOT_ITS() p_IPP_IP->IPP_DXO_DPP_ITS.IPP_DXO_DPP_ITS_ts.IPP_DXO_DPP_ENDOFBOOT_ITS


#define Get_IPP_IPP_DXO_DPP_ITS_IPP_DXO_DPP_ENDOFEXECCMD_ITS() p_IPP_IP->IPP_DXO_DPP_ITS.IPP_DXO_DPP_ITS_ts.IPP_DXO_DPP_ENDOFEXECCMD_ITS


#define Get_IPP_IPP_DXO_DPP_ITS_IPP_DXO_DPP_ENDOFPROCESSING_ITS() p_IPP_IP->IPP_DXO_DPP_ITS.IPP_DXO_DPP_ITS_ts.IPP_DXO_DPP_ENDOFPROCESSING_ITS


#define Get_IPP_IPP_DXO_DPP_ITS_IPP_DXO_DPP_NEWFRAMECMDACK_ITS() p_IPP_IP->IPP_DXO_DPP_ITS.IPP_DXO_DPP_ITS_ts.IPP_DXO_DPP_NEWFRAMECMDACK_ITS


#define Get_IPP_IPP_DXO_DPP_ITS_IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS() p_IPP_IP->IPP_DXO_DPP_ITS.IPP_DXO_DPP_ITS_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS


#define Get_IPP_IPP_DXO_DPP_ITS_IPP_DXO_DPP_NEWFRAMEERROR_ITS() p_IPP_IP->IPP_DXO_DPP_ITS.IPP_DXO_DPP_ITS_ts.IPP_DXO_DPP_NEWFRAMEERROR_ITS


#define Get_IPP_IPP_DXO_DPP_ITS_IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS() p_IPP_IP->IPP_DXO_DPP_ITS.IPP_DXO_DPP_ITS_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS


#define Get_IPP_IPP_DXO_DPP_ITS_IPP_DXO_DPP_PULSEOUT0_ITS() p_IPP_IP->IPP_DXO_DPP_ITS.IPP_DXO_DPP_ITS_ts.IPP_DXO_DPP_PULSEOUT0_ITS
#define Get_IPP_IPP_DXO_DPP_ITS() p_IPP_IP->IPP_DXO_DPP_ITS.word


//IPP_DXO_DPP_ITS_BCLR


#define Set_IPP_IPP_DXO_DPP_ITS_BCLR_IPP_DXO_DPP_ENDOFBOOT_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.IPP_DXO_DPP_ITS_BCLR_ts.IPP_DXO_DPP_ENDOFBOOT_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BCLR_IPP_DXO_DPP_ENDOFEXECCMD_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.IPP_DXO_DPP_ITS_BCLR_ts.IPP_DXO_DPP_ENDOFEXECCMD_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BCLR_IPP_DXO_DPP_ENDOFPROCESSING_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.IPP_DXO_DPP_ITS_BCLR_ts.IPP_DXO_DPP_ENDOFPROCESSING_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BCLR_IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.IPP_DXO_DPP_ITS_BCLR_ts.IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BCLR_IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.IPP_DXO_DPP_ITS_BCLR_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BCLR_IPP_DXO_DPP_NEWFRAMEERROR_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.IPP_DXO_DPP_ITS_BCLR_ts.IPP_DXO_DPP_NEWFRAMEERROR_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BCLR_IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.IPP_DXO_DPP_ITS_BCLR_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BCLR_IPP_DXO_DPP_PULSEOUT0_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.IPP_DXO_DPP_ITS_BCLR_ts.IPP_DXO_DPP_PULSEOUT0_ITS_BCLR = x)
#define Set_IPP_IPP_DXO_DPP_ITS_BCLR(IPP_DXO_DPP_ENDOFBOOT_ITS_BCLR,IPP_DXO_DPP_ENDOFEXECCMD_ITS_BCLR,IPP_DXO_DPP_ENDOFPROCESSING_ITS_BCLR,IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BCLR,IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BCLR,IPP_DXO_DPP_NEWFRAMEERROR_ITS_BCLR,IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BCLR,IPP_DXO_DPP_PULSEOUT0_ITS_BCLR) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.word = (uint16_t)IPP_DXO_DPP_ENDOFBOOT_ITS_BCLR<<0 | (uint16_t)IPP_DXO_DPP_ENDOFEXECCMD_ITS_BCLR<<1 | (uint16_t)IPP_DXO_DPP_ENDOFPROCESSING_ITS_BCLR<<2 | (uint16_t)IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BCLR<<3 | (uint16_t)IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BCLR<<4 | (uint16_t)IPP_DXO_DPP_NEWFRAMEERROR_ITS_BCLR<<5 | (uint16_t)IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BCLR<<6 | (uint16_t)IPP_DXO_DPP_PULSEOUT0_ITS_BCLR<<7)
#define Set_IPP_IPP_DXO_DPP_ITS_BCLR_word(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BCLR.word = x)


//IPP_DXO_DPP_ITS_BSET


#define Set_IPP_IPP_DXO_DPP_ITS_BSET_IPP_DXO_DPP_ENDOFBOOT_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.IPP_DXO_DPP_ITS_BSET_ts.IPP_DXO_DPP_ENDOFBOOT_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BSET_IPP_DXO_DPP_ENDOFEXECCMD_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.IPP_DXO_DPP_ITS_BSET_ts.IPP_DXO_DPP_ENDOFEXECCMD_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BSET_IPP_DXO_DPP_ENDOFPROCESSING_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.IPP_DXO_DPP_ITS_BSET_ts.IPP_DXO_DPP_ENDOFPROCESSING_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BSET_IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.IPP_DXO_DPP_ITS_BSET_ts.IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BSET_IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.IPP_DXO_DPP_ITS_BSET_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BSET_IPP_DXO_DPP_NEWFRAMEERROR_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.IPP_DXO_DPP_ITS_BSET_ts.IPP_DXO_DPP_NEWFRAMEERROR_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BSET_IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.IPP_DXO_DPP_ITS_BSET_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITS_BSET_IPP_DXO_DPP_PULSEOUT0_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.IPP_DXO_DPP_ITS_BSET_ts.IPP_DXO_DPP_PULSEOUT0_ITS_BSET = x)
#define Set_IPP_IPP_DXO_DPP_ITS_BSET(IPP_DXO_DPP_ENDOFBOOT_ITS_BSET,IPP_DXO_DPP_ENDOFEXECCMD_ITS_BSET,IPP_DXO_DPP_ENDOFPROCESSING_ITS_BSET,IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BSET,IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BSET,IPP_DXO_DPP_NEWFRAMEERROR_ITS_BSET,IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BSET,IPP_DXO_DPP_PULSEOUT0_ITS_BSET) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.word = (uint32_t)IPP_DXO_DPP_ENDOFBOOT_ITS_BSET<<0 | (uint32_t)IPP_DXO_DPP_ENDOFEXECCMD_ITS_BSET<<1 | (uint32_t)IPP_DXO_DPP_ENDOFPROCESSING_ITS_BSET<<2 | (uint32_t)IPP_DXO_DPP_NEWFRAMECMDACK_ITS_BSET<<3 | (uint32_t)IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITS_BSET<<4 | (uint32_t)IPP_DXO_DPP_NEWFRAMEERROR_ITS_BSET<<5 | (uint32_t)IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITS_BSET<<6 | (uint32_t)IPP_DXO_DPP_PULSEOUT0_ITS_BSET<<7)
#define Set_IPP_IPP_DXO_DPP_ITS_BSET_word(x) (p_IPP_IP->IPP_DXO_DPP_ITS_BSET.word = x)


//IPP_DXO_DPP_ITM_BCLR


#define Set_IPP_IPP_DXO_DPP_ITM_BCLR_IPP_DXO_DPP_ENDOFBOOT_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.IPP_DXO_DPP_ITM_BCLR_ts.IPP_DXO_DPP_ENDOFBOOT_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BCLR_IPP_DXO_DPP_ENDOFEXECCMD_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.IPP_DXO_DPP_ITM_BCLR_ts.IPP_DXO_DPP_ENDOFEXECCMD_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BCLR_IPP_DXO_DPP_ENDOFPROCESSING_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.IPP_DXO_DPP_ITM_BCLR_ts.IPP_DXO_DPP_ENDOFPROCESSING_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BCLR_IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.IPP_DXO_DPP_ITM_BCLR_ts.IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BCLR_IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.IPP_DXO_DPP_ITM_BCLR_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BCLR_IPP_DXO_DPP_NEWFRAMEERROR_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.IPP_DXO_DPP_ITM_BCLR_ts.IPP_DXO_DPP_NEWFRAMEERROR_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BCLR_IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.IPP_DXO_DPP_ITM_BCLR_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BCLR_IPP_DXO_DPP_PULSEOUT0_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.IPP_DXO_DPP_ITM_BCLR_ts.IPP_DXO_DPP_PULSEOUT0_ITM_BCLR = x)
#define Set_IPP_IPP_DXO_DPP_ITM_BCLR(IPP_DXO_DPP_ENDOFBOOT_ITM_BCLR,IPP_DXO_DPP_ENDOFEXECCMD_ITM_BCLR,IPP_DXO_DPP_ENDOFPROCESSING_ITM_BCLR,IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BCLR,IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BCLR,IPP_DXO_DPP_NEWFRAMEERROR_ITM_BCLR,IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BCLR,IPP_DXO_DPP_PULSEOUT0_ITM_BCLR) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.word = (uint16_t)IPP_DXO_DPP_ENDOFBOOT_ITM_BCLR<<0 | (uint16_t)IPP_DXO_DPP_ENDOFEXECCMD_ITM_BCLR<<1 | (uint16_t)IPP_DXO_DPP_ENDOFPROCESSING_ITM_BCLR<<2 | (uint16_t)IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BCLR<<3 | (uint16_t)IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BCLR<<4 | (uint16_t)IPP_DXO_DPP_NEWFRAMEERROR_ITM_BCLR<<5 | (uint16_t)IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BCLR<<6 | (uint16_t)IPP_DXO_DPP_PULSEOUT0_ITM_BCLR<<7)
#define Set_IPP_IPP_DXO_DPP_ITM_BCLR_word(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BCLR.word = x)


//IPP_DXO_DPP_ITM_BSET


#define Set_IPP_IPP_DXO_DPP_ITM_BSET_IPP_DXO_DPP_ENDOFBOOT_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.IPP_DXO_DPP_ITM_BSET_ts.IPP_DXO_DPP_ENDOFBOOT_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BSET_IPP_DXO_DPP_ENDOFEXECCMD_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.IPP_DXO_DPP_ITM_BSET_ts.IPP_DXO_DPP_ENDOFEXECCMD_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BSET_IPP_DXO_DPP_ENDOFPROCESSING_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.IPP_DXO_DPP_ITM_BSET_ts.IPP_DXO_DPP_ENDOFPROCESSING_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BSET_IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.IPP_DXO_DPP_ITM_BSET_ts.IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BSET_IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.IPP_DXO_DPP_ITM_BSET_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BSET_IPP_DXO_DPP_NEWFRAMEERROR_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.IPP_DXO_DPP_ITM_BSET_ts.IPP_DXO_DPP_NEWFRAMEERROR_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BSET_IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.IPP_DXO_DPP_ITM_BSET_ts.IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DPP_ITM_BSET_IPP_DXO_DPP_PULSEOUT0_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.IPP_DXO_DPP_ITM_BSET_ts.IPP_DXO_DPP_PULSEOUT0_ITM_BSET = x)
#define Set_IPP_IPP_DXO_DPP_ITM_BSET(IPP_DXO_DPP_ENDOFBOOT_ITM_BSET,IPP_DXO_DPP_ENDOFEXECCMD_ITM_BSET,IPP_DXO_DPP_ENDOFPROCESSING_ITM_BSET,IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BSET,IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BSET,IPP_DXO_DPP_NEWFRAMEERROR_ITM_BSET,IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BSET,IPP_DXO_DPP_PULSEOUT0_ITM_BSET) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.word = (uint16_t)IPP_DXO_DPP_ENDOFBOOT_ITM_BSET<<0 | (uint16_t)IPP_DXO_DPP_ENDOFEXECCMD_ITM_BSET<<1 | (uint16_t)IPP_DXO_DPP_ENDOFPROCESSING_ITM_BSET<<2 | (uint16_t)IPP_DXO_DPP_NEWFRAMECMDACK_ITM_BSET<<3 | (uint16_t)IPP_DXO_DPP_NEWFRAMEPROCESSINGACK_ITM_BSET<<4 | (uint16_t)IPP_DXO_DPP_NEWFRAMEERROR_ITM_BSET<<5 | (uint16_t)IPP_DXO_DPP_NEWFRAMEPROCESSINGERROR_ITM_BSET<<6 | (uint16_t)IPP_DXO_DPP_PULSEOUT0_ITM_BSET<<7)
#define Set_IPP_IPP_DXO_DPP_ITM_BSET_word(x) (p_IPP_IP->IPP_DXO_DPP_ITM_BSET.word = x)


//IPP_DXO_DOP7_ITS


#define Get_IPP_IPP_DXO_DOP7_ITS_IPP_DXO_DOP7_ENDOFBOOT_ITS() p_IPP_IP->IPP_DXO_DOP7_ITS.IPP_DXO_DOP7_ITS_ts.IPP_DXO_DOP7_ENDOFBOOT_ITS


#define Get_IPP_IPP_DXO_DOP7_ITS_IPP_DXO_DOP7_ENDOFEXECCMD_ITS() p_IPP_IP->IPP_DXO_DOP7_ITS.IPP_DXO_DOP7_ITS_ts.IPP_DXO_DOP7_ENDOFEXECCMD_ITS


#define Get_IPP_IPP_DXO_DOP7_ITS_IPP_DXO_DOP7_ENDOFPROCESSING_ITS() p_IPP_IP->IPP_DXO_DOP7_ITS.IPP_DXO_DOP7_ITS_ts.IPP_DXO_DOP7_ENDOFPROCESSING_ITS


#define Get_IPP_IPP_DXO_DOP7_ITS_IPP_DXO_DOP7_NEWFRAMECMDACK_ITS() p_IPP_IP->IPP_DXO_DOP7_ITS.IPP_DXO_DOP7_ITS_ts.IPP_DXO_DOP7_NEWFRAMECMDACK_ITS


#define Get_IPP_IPP_DXO_DOP7_ITS_IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS() p_IPP_IP->IPP_DXO_DOP7_ITS.IPP_DXO_DOP7_ITS_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS


#define Get_IPP_IPP_DXO_DOP7_ITS_IPP_DXO_DOP7_NEWFRAMEERROR_ITS() p_IPP_IP->IPP_DXO_DOP7_ITS.IPP_DXO_DOP7_ITS_ts.IPP_DXO_DOP7_NEWFRAMEERROR_ITS


#define Get_IPP_IPP_DXO_DOP7_ITS_IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS() p_IPP_IP->IPP_DXO_DOP7_ITS.IPP_DXO_DOP7_ITS_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS


#define Get_IPP_IPP_DXO_DOP7_ITS_IPP_DXO_DOP7_PULSEOUT0_ITS() p_IPP_IP->IPP_DXO_DOP7_ITS.IPP_DXO_DOP7_ITS_ts.IPP_DXO_DOP7_PULSEOUT0_ITS
#define Get_IPP_IPP_DXO_DOP7_ITS() p_IPP_IP->IPP_DXO_DOP7_ITS.word


//IPP_DXO_DOP7_ITS_BCLR


#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR_IPP_DXO_DOP7_ENDOFBOOT_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.IPP_DXO_DOP7_ITS_BCLR_ts.IPP_DXO_DOP7_ENDOFBOOT_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR_IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.IPP_DXO_DOP7_ITS_BCLR_ts.IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR_IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.IPP_DXO_DOP7_ITS_BCLR_ts.IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR_IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.IPP_DXO_DOP7_ITS_BCLR_ts.IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR_IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.IPP_DXO_DOP7_ITS_BCLR_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR_IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.IPP_DXO_DOP7_ITS_BCLR_ts.IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR_IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.IPP_DXO_DOP7_ITS_BCLR_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR_IPP_DXO_DOP7_PULSEOUT0_ITS_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.IPP_DXO_DOP7_ITS_BCLR_ts.IPP_DXO_DOP7_PULSEOUT0_ITS_BCLR = x)
#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR(IPP_DXO_DOP7_ENDOFBOOT_ITS_BCLR,IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BCLR,IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BCLR,IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BCLR,IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BCLR,IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BCLR,IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BCLR,IPP_DXO_DOP7_PULSEOUT0_ITS_BCLR) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.word = (uint16_t)IPP_DXO_DOP7_ENDOFBOOT_ITS_BCLR<<0 | (uint16_t)IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BCLR<<1 | (uint16_t)IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BCLR<<2 | (uint16_t)IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BCLR<<3 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BCLR<<4 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BCLR<<5 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BCLR<<6 | (uint16_t)IPP_DXO_DOP7_PULSEOUT0_ITS_BCLR<<7)
#define Set_IPP_IPP_DXO_DOP7_ITS_BCLR_word(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BCLR.word = x)


//IPP_DXO_DOP7_ITS_BSET


#define Set_IPP_IPP_DXO_DOP7_ITS_BSET_IPP_DXO_DOP7_ENDOFBOOT_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.IPP_DXO_DOP7_ITS_BSET_ts.IPP_DXO_DOP7_ENDOFBOOT_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BSET_IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.IPP_DXO_DOP7_ITS_BSET_ts.IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BSET_IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.IPP_DXO_DOP7_ITS_BSET_ts.IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BSET_IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.IPP_DXO_DOP7_ITS_BSET_ts.IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BSET_IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.IPP_DXO_DOP7_ITS_BSET_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BSET_IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.IPP_DXO_DOP7_ITS_BSET_ts.IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BSET_IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.IPP_DXO_DOP7_ITS_BSET_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITS_BSET_IPP_DXO_DOP7_PULSEOUT0_ITS_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.IPP_DXO_DOP7_ITS_BSET_ts.IPP_DXO_DOP7_PULSEOUT0_ITS_BSET = x)
#define Set_IPP_IPP_DXO_DOP7_ITS_BSET(IPP_DXO_DOP7_ENDOFBOOT_ITS_BSET,IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BSET,IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BSET,IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BSET,IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BSET,IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BSET,IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BSET,IPP_DXO_DOP7_PULSEOUT0_ITS_BSET) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.word = (uint16_t)IPP_DXO_DOP7_ENDOFBOOT_ITS_BSET<<0 | (uint16_t)IPP_DXO_DOP7_ENDOFEXECCMD_ITS_BSET<<1 | (uint16_t)IPP_DXO_DOP7_ENDOFPROCESSING_ITS_BSET<<2 | (uint16_t)IPP_DXO_DOP7_NEWFRAMECMDACK_ITS_BSET<<3 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITS_BSET<<4 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEERROR_ITS_BSET<<5 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITS_BSET<<6 | (uint16_t)IPP_DXO_DOP7_PULSEOUT0_ITS_BSET<<7)
#define Set_IPP_IPP_DXO_DOP7_ITS_BSET_word(x) (p_IPP_IP->IPP_DXO_DOP7_ITS_BSET.word = x)


//IPP_DXO_DOP7_ITM


#define Get_IPP_IPP_DXO_DOP7_ITM_IPP_DXO_DOP7_ENDOFBOOT_ITM() p_IPP_IP->IPP_DXO_DOP7_ITM.IPP_DXO_DOP7_ITM_ts.IPP_DXO_DOP7_ENDOFBOOT_ITM


#define Get_IPP_IPP_DXO_DOP7_ITM_IPP_DXO_DOP7_ENDOFEXECCMD_ITM() p_IPP_IP->IPP_DXO_DOP7_ITM.IPP_DXO_DOP7_ITM_ts.IPP_DXO_DOP7_ENDOFEXECCMD_ITM


#define Get_IPP_IPP_DXO_DOP7_ITM_IPP_DXO_DOP7_ENDOFPROCESSING_ITM() p_IPP_IP->IPP_DXO_DOP7_ITM.IPP_DXO_DOP7_ITM_ts.IPP_DXO_DOP7_ENDOFPROCESSING_ITM


#define Get_IPP_IPP_DXO_DOP7_ITM_IPP_DXO_DOP7_NEWFRAMECMDACK_ITM() p_IPP_IP->IPP_DXO_DOP7_ITM.IPP_DXO_DOP7_ITM_ts.IPP_DXO_DOP7_NEWFRAMECMDACK_ITM


#define Get_IPP_IPP_DXO_DOP7_ITM_IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM() p_IPP_IP->IPP_DXO_DOP7_ITM.IPP_DXO_DOP7_ITM_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM


#define Get_IPP_IPP_DXO_DOP7_ITM_IPP_DXO_DOP7_NEWFRAMEERROR_ITM() p_IPP_IP->IPP_DXO_DOP7_ITM.IPP_DXO_DOP7_ITM_ts.IPP_DXO_DOP7_NEWFRAMEERROR_ITM


#define Get_IPP_IPP_DXO_DOP7_ITM_IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM() p_IPP_IP->IPP_DXO_DOP7_ITM.IPP_DXO_DOP7_ITM_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM


#define Get_IPP_IPP_DXO_DOP7_ITM_IPP_DXO_DOP7_PULSEOUT0_ITM() p_IPP_IP->IPP_DXO_DOP7_ITM.IPP_DXO_DOP7_ITM_ts.IPP_DXO_DOP7_PULSEOUT0_ITM
#define Get_IPP_IPP_DXO_DOP7_ITM() p_IPP_IP->IPP_DXO_DOP7_ITM.word


//IPP_DXO_DOP7_ITM_BCLR


#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR_IPP_DXO_DOP7_ENDOFBOOT_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.IPP_DXO_DOP7_ITM_BCLR_ts.IPP_DXO_DOP7_ENDOFBOOT_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR_IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.IPP_DXO_DOP7_ITM_BCLR_ts.IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR_IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.IPP_DXO_DOP7_ITM_BCLR_ts.IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR_IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.IPP_DXO_DOP7_ITM_BCLR_ts.IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR_IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.IPP_DXO_DOP7_ITM_BCLR_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR_IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.IPP_DXO_DOP7_ITM_BCLR_ts.IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR_IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.IPP_DXO_DOP7_ITM_BCLR_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BCLR = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR_IPP_DXO_DOP7_PULSEOUT0_ITM_BCLR(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.IPP_DXO_DOP7_ITM_BCLR_ts.IPP_DXO_DOP7_PULSEOUT0_ITM_BCLR = x)
#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR(IPP_DXO_DOP7_ENDOFBOOT_ITM_BCLR,IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BCLR,IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BCLR,IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BCLR,IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BCLR,IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BCLR,IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BCLR,IPP_DXO_DOP7_PULSEOUT0_ITM_BCLR) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.word = (uint16_t)IPP_DXO_DOP7_ENDOFBOOT_ITM_BCLR<<0 | (uint16_t)IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BCLR<<1 | (uint16_t)IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BCLR<<2 | (uint16_t)IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BCLR<<3 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BCLR<<4 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BCLR<<5 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BCLR<<6 | (uint16_t)IPP_DXO_DOP7_PULSEOUT0_ITM_BCLR<<7)
#define Set_IPP_IPP_DXO_DOP7_ITM_BCLR_word(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BCLR.word = x)


//IPP_DXO_DOP7_ITM_BSET


#define Set_IPP_IPP_DXO_DOP7_ITM_BSET_IPP_DXO_DOP7_ENDOFBOOT_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.IPP_DXO_DOP7_ITM_BSET_ts.IPP_DXO_DOP7_ENDOFBOOT_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BSET_IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.IPP_DXO_DOP7_ITM_BSET_ts.IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BSET_IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.IPP_DXO_DOP7_ITM_BSET_ts.IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BSET_IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.IPP_DXO_DOP7_ITM_BSET_ts.IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BSET_IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.IPP_DXO_DOP7_ITM_BSET_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BSET_IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.IPP_DXO_DOP7_ITM_BSET_ts.IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BSET_IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.IPP_DXO_DOP7_ITM_BSET_ts.IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BSET = x)


#define Set_IPP_IPP_DXO_DOP7_ITM_BSET_IPP_DXO_DOP7_PULSEOUT0_ITM_BSET(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.IPP_DXO_DOP7_ITM_BSET_ts.IPP_DXO_DOP7_PULSEOUT0_ITM_BSET = x)
#define Set_IPP_IPP_DXO_DOP7_ITM_BSET(IPP_DXO_DOP7_ENDOFBOOT_ITM_BSET,IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BSET,IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BSET,IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BSET,IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BSET,IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BSET,IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BSET,IPP_DXO_DOP7_PULSEOUT0_ITM_BSET) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.word = (uint16_t)IPP_DXO_DOP7_ENDOFBOOT_ITM_BSET<<0 | (uint16_t)IPP_DXO_DOP7_ENDOFEXECCMD_ITM_BSET<<1 | (uint16_t)IPP_DXO_DOP7_ENDOFPROCESSING_ITM_BSET<<2 | (uint16_t)IPP_DXO_DOP7_NEWFRAMECMDACK_ITM_BSET<<3 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEPROCESSINGACK_ITM_BSET<<4 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEERROR_ITM_BSET<<5 | (uint16_t)IPP_DXO_DOP7_NEWFRAMEPROCESSINGERROR_ITM_BSET<<6 | (uint16_t)IPP_DXO_DOP7_PULSEOUT0_ITM_BSET<<7)
#define Set_IPP_IPP_DXO_DOP7_ITM_BSET_word(x) (p_IPP_IP->IPP_DXO_DOP7_ITM_BSET.word = x)

extern volatile IPP_IP_ts *p_IPP_IP;

#endif 

