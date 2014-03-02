/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of AB8500(STw4500) Core module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __AB8500_CORE_P_H__
#define __AB8500_CORE_P_H__

#ifdef __cplusplus
extern "C"
{
#endif
/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

#define AB8500_CORE_PONKEY1_MAX_PRESS_DURATION       10000       /* in msecs */

/*------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------BLOCKS----------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
#define AB8500_CORE_SYSTEM_CTRL1_BANK                0x01
#define AB8500_CORE_SYSTEM_CTRL2_BANK                0x02
#define AB8500_CORE_REGULATOR_CTRL1_BANK             0x03
#define AB8500_CORE_REG_CTRL2_SIM_BANK               0x04
#define AB8500_CORE_USB_BANK                         0x05
#define AB8500_CORE_TVOUT_BANK                       0x06
#define AB8500_CORE_DBI_BANK                         0x07
#define AB8500_CORE_ECI_AV_ACC_DET_BANK              0x08
#define AB8500_CORE_GPADC_BANK                       0x0A
#define AB8500_CORE_CHARGER_BANK                     0x0B
#define AB8500_CORE_GAS_GAUGE_BANK                   0x0C
#define AB8500_CORE_AUDIO_BANK                       0x0D
#define AB8500_CORE_INTERRUPT_BANK                   0x0E
#define AB8500_CORE_RTC_BANK                         0x0F

#define AB8500_CORE_CONTROL_REG_MIN_OFFSET           0x03
#define AB8500_CORE_CONTROL_REG_MAX_OFFSET           0x18
#define AB8500_CORE_PMU_REG_MIN_OFFSET               0x20
#define AB8500_CORE_PMU_REG_MAX_OFFSET               0x39

#define ONE_BYTE                                     0x01
#define MASK_ONE_BYTE                                0xFF

/*------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------AB8500 CORE REGISTER OFFSETS------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*---------------------------------AB8500_CORE_SYSTEM_CTRL1_BANK(BANK1)---------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
#define AB8500_CORE_TURN_ON_STATUS_REG               0x00
#define AB8500_CORE_PON_KEYPRESS_STA_REG             0x02

#define AB8500_CORE_CTRL1_REG                        0x80
#define AB8500_CORE_CTRL2_REG                        0x81


/*-------------------------- OFFSETS IN TURN-ON STATUS REGISTER FIELDS--------------*/
#define AB8500_CORE_START_WITH_PORnVBAT              0x01
#define AB8500_CORE_START_WITH_PONKEY1dbF            0x02
#define AB8500_CORE_START_WITH_PONKEY2dbF            0x04
#define AB8500_CORE_START_WITH_RTCALARM              0x08
#define AB8500_CORE_START_WITH_MAIN_CHRG_DET         0x10
#define AB8500_CORE_START_WITH_VBUS_DETECT           0x20
#define AB8500_CORE_START_WITH_VUSB_ID_DETECT        0x40


/*-------------------------- OFFSETS IN STw4500 CTRL1 REGISTER FIELDS--------------*/
#define AB8500_CORE_SWITCHOFF_CMD                    0x01
#define AB8500_CORE_RESETn_4500_REG                  0x02




/*------------------------------------------------------------------------------------------------------------*/
/*---------------------------------AB8500_CORE_SYSTEM_CTRL2_BANK(BANK2)---------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
#define AB8500_CORE_CTRL3_REG                        0x00
#define AB8500_CORE_MAIN_WDOG_CTRL_REG               0x01
#define AB8500_CORE_MAIN_WDOG_TMR_REG                0x02
#define AB8500_CORE_LOW_BATT_REG                     0x03
#define AB8500_CORE_BATT_OK_REG                      0x04
#define AB8500_CORE_SYSCLK_TMR_REG                   0x05
#define AB8500_CORE_SMPSCLK_CTRL_REG                 0x06
#define AB8500_CORE_SMPSCLK_SEL1_REG                 0x07
#define AB8500_CORE_SMPSCLK_SEL2_REG                 0x08
#define AB8500_CORE_SMPSCLK_SEL3_REG                 0x09
#define AB8500_CORE_SYSULPCLK_CONF_REG               0x0A
#define AB8500_CORE_SYSULPCLK_CTRL1_REG              0x0B
#define AB8500_CORE_SYSCLKCTRL_REG                   0x0C
#define AB8500_CORE_SYSCLKREQ1_VALID_REG             0x0D
#define AB8500_CORE_SYSCTRL_SPARE_REG                0x0F

#define AB8500_CORE_SYSCLKREQ1_RFCLKBUF_REG          0x10
#define AB8500_CORE_SYSCLKREQ2_RFCLKBUF_REG          0x11
#define AB8500_CORE_SYSCLKREQ3_RFCLKBUF_REG          0x12
#define AB8500_CORE_SYSCLKREQ4_RFCLKBUF_REG          0x13
#define AB8500_CORE_SYSCLKREQ5_RFCLKBUF_REG          0x14
#define AB8500_CORE_SYSCLKREQ6_RFCLKBUF_REG          0x15
#define AB8500_CORE_SYSCLKREQ7_RFCLKBUF_REG          0x16
#define AB8500_CORE_SYSCLKREQ8_RFCLKBUF_REG          0x17
#define AB8500_CORE_SWAT_CTRL_REG                    0x30
#define AB8500_CORE_HIQCLK_CTRL                      0x32
#define AB8500_CORE_VSIMSYSCLK_CTRL                  0x33

/*-------------------------- OFFSETS IN AB8500_CORE_MAIN_WDOG_CTRL_REG FIELDS--------------*/
#define AB8500_CORE_WDEXP_TURNONVALID                0x10	
#define AB8500_CORE_MAINWDWRITE                      0x08
#define AB8500_CORE_MAINWDKICK                       0x02
#define AB8500_CORE_MAINWDENABLE                     0x01
	
#define AB8500_CORE_MAIN_WDTMR_MAX_VAL               127	
/*-------------------------- OFFSETS IN STw4500 CTRL3 REGISTER FIELDS--------------*/
#define AB8500_CORE_CLK32_OUT2_DIS                   0x01
#define sbAB8500_CORE_CLK32_OUT2_DIS                 0x00

#define AB8500_CORE_RESETn_AUDIO                     0x02
#define AB8500_CORE_RESETn_DENC                      0x04

/*---------- OFFSETS IN AB8500_CORE_SYSULPCLK_CONF_REG REGISTER FIELDS------------------*/
#define AB8500_CORE_ULPCLK_CONF                      0x03
#define sbAB8500_CORE_ULPCLK_CONF                    0x00

#define AB8500_CORE_CLK_27MHZ_STRENGTH               0x04  
#define sbAB8500_CORE_CLK_27MHZ_STRENGTH             0x02

#define AB8500_CORE_TVOUT_CLK_DELAY                  0x08
#define sbAB8500_CORE_TVOUT_CLK_DELAY                0x03

#define AB8500_CORE_TVOUT_CLK_INV                    0x10
#define sbAB8500_CORE_TVOUT_CLK_INV                  0x04

#define AB8500_CORE_ULPCLK_STRENGTH                  0x20  
#define sbAB8500_CORE_ULPCLK_STRENGTH                0x05
 
#define AB8500_CORE_CLK27MHZ_BUF_ENA                 0x40
#define sbAB8500_CORE_CLK27MHZ_BUF_ENA               0x06

#define AB8500_CORE_CLK27MHZ_PD_ENA                  0x80
#define sbAB8500_CORE_CLK27MHZ_PD_ENA                0x07

/*---------- OFFSETS IN STw4500 AB8500_CORE_SYSULPCLK_CTRL1_REG FIELDS--------------*/
#define AB8500_CORE_SYSULPCLK_INTSEL                 0x03
#define sbAB8500_CORE_SYSULPCLK_INTSEL               0x00

#define AB8500_CORE_ULPCLK_REQ                       0x04
#define sbAB8500_CORE_ULPCLK_REQ                     0x02

#define AB8500_CORE_SYSCLK_REQ                       0x08
#define sbAB8500_CORE_SYSCLK_REQ                     0x03

#define AB8500_CORE_AUDIO_CLK_ENABLE                 0x10


/*---------- OFFSETS IN AB8500_CORE_SYSCLKCTRL_REG REGISTER FIELDS------------------*/
#define AB8500_CORE_TVOUT_PLL_ENA                    0x01
#define sbAB8500_CORE_TVOUT_PLL_ENA                  0x00

#define AB8500_CORE_TVOUT_CLK_ENA                    0x02
#define AB8500_CORE_USB_CLK_ENA                      0x01

/*---------- OFFSETS IN AB8500_CORE_SYSCLKREQ1_VALID_REG REGISTER FIELDS------------------*/
#define AB8500_CORE_SYSCLKREQ1_VALID                 0x01
#define sbAB8500_CORE_SYSCLKREQ1_VALID               0x00

#define AB8500_CORE_ULPCLKREQ1_VALID                 0x02
#define sbAB8500_CORE_ULPCLKREQ1_VALID               0x01

/*---------- OFFSETS IN AB8500_CORE_SYSCLKREQ*_RFCLKBUF_REG REGISTER FIELDS------------------*/
#define AB8500_CORE_SYSCLKREQ_RFCLKBUF_2             0x04
#define sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_2           0x02

#define AB8500_CORE_SYSCLKREQ_RFCLKBUF_3             0x08
#define sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_3           0x03

#define AB8500_CORE_SYSCLKREQ_RFCLKBUF_4             0x10
#define sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_4           0x04

/*---------- OFFSETS IN AB8500_CORE_SWAT_CTRL_REG REGISTER FIELDS------------------*/
#define AB8500_CORE_LEVI_DIESEL_SEL                  0x01
#define sbAB8500_CORE_LEVI_DIESEL_SEL                0x00

#define AB8500_CORE_SWAT_ENABLE                      0x02
#define sbAB8500_CORE_SWAT_ENABLE                    0x01

#define AB8500_CORE_RF_OFF_TMR                       0x1C
#define sbAB8500_CORE_RF_OFF_TMR                     0x02

/*---------- OFFSETS IN AB8500_CORE_SYSCTRL_SPARE_REG REGISTER FIELDS------------------*/
#define AB8500_CORE_INT8500N                         0x10
#define sbAB8500_CORE_INT8500N                       0x04

/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------AB8500_CORE_REGULATOR_CTRL1_BANK(BANK3)---------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
#define AB8500_CORE_REGU_SERIALCTRL1_REG             0x00
#define AB8500_CORE_REGU_SERIALCTRL2_REG             0x01
#define AB8500_CORE_REGU_SERIALCTRL3_REG             0x02
#define AB8500_CORE_REGU_REQUESTCTRL1_REG            0x03
#define AB8500_CORE_REGU_REQUESTCTRL2_REG            0x04
#define AB8500_CORE_REGU_REQUESTCTRL3_REG            0x05
#define AB8500_CORE_REGU_REQUESTCTRL4_REG            0x06
#define AB8500_CORE_REGU_SYSCLKREQ1_HPVALID1_REG     0x07
#define AB8500_CORE_REGU_SYSCLKREQ1_HPVALID2_REG     0x08
#define AB8500_CORE_REGU_HWHPREQ1_VALID1_REG         0x09
#define AB8500_CORE_REGU_HWHPREQ1_VALID2_REG         0x0A
#define AB8500_CORE_REGU_HWHPREQ2_VALID1_REG         0x0B
#define AB8500_CORE_REGU_HWHPREQ2_VALID2_REG         0x0C
#define AB8500_CORE_REGU_SWHPREQ_VALID1_REG          0x0D
#define AB8500_CORE_REGU_SWHPREQ_VALID2_REG          0x0E
#define AB8500_CORE_REGU_SYSCLKREQ1_VALID_REG        0x0F
#define AB8500_CORE_REGU_SYSCLKREQ2_VALID_REG        0x10

#define AB8500_CORE_REGU_MISC1_REG                   0x80
#define AB8500_CORE_OTG_SUPPLY_CTRL_REG              0x81
#define AB8500_CORE_VUSB_CTRL_REG                    0x82
#define AB8500_CORE_VAUDIO_SUPPLY_REG                0x83
#define AB8500_CORE_REGCTRL1_SPARE_REG               0x84

/*-------OFFSETS IN STw4500 AB8500_CORE_REGU_MISC1_REG FIELDS--------------*/
#define AB8500_CORE_VTVOUT_ENA                       0x02
#define sbAB8500_CORE_VTVOUT_ENA                     0x01

#define AB8500_CORE_VINT_VCORE12_ENA                 0x04
#define sbAB8500_CORE_VINT_VCORE12_ENA               0x02

#define AB8500_CORE_VINTCORE12_SEL                   0x38
#define sbAB8500_CORE_VINTCORE12_SEL                 0x03

#define AB8500_CORE_VINTCORE12_LP                    0x40
#define sbAB8500_CORE_VINTCORE12_LP                  0x06


/*-------OFFSETS IN STw4500 AB8500_CORE_OTG_SUPPLY_CTRL_REG FIELDS--------------*/
#define AB8500_CORE_OTG_SUPPLY_ENA                   0x01
#define sbAB8500_CORE_OTG_SUPPLY_ENA                 0x00

/*------ OFFSETS IN STw4500 AB8500_CORE_VAUDIO_SUPPLY_REG FIELDS-----------*/
#define AB8500_CORE_VAUDIO_ENA                       0x02
#define sbAB8500_CORE_VAUDIO_ENA                     0x01

#define AB8500_CORE_VDMIC_ENA                        0x04
#define sbAB8500_CORE_VDMIC_ENA                      0x02

#define AB8500_CORE_VAMIC1_ENA                       0x08
#define sbAB8500_CORE_VAMIC1_ENA                     0x03

#define AB8500_CORE_VAMIC2_ENA                       0x10
#define sbAB8500_CORE_VAMIC2_ENA                     0x04

/*------ OFFSETS IN STw4500 AB8500_CORE_REGCTRL1_SPARE_REG FIELDS-----------*/
#define AB8500_CORE_VAMIC1_DZOUT                     0x01
#define sbAB8500_CORE_VAMIC1_DZOUT                   0x00

#define AB8500_CORE_VAMIC2_DZOUT                     0x02
#define sbAB8500_CORE_VAMIC2_DZOUT                   0x01

/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------AB8500_CORE_REG_CTRL2_SIM_BANK(BANK4)-----------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
#define AB8500_CORE_ARM_REGU1_REG                    0x00
#define AB8500_CORE_ARM_REGU2_REG                    0x01
#define AB8500_CORE_VAPE_REGU_REG                    0x02
#define AB8500_CORE_VSMPS1_REGU_REG                  0x03
#define AB8500_CORE_VSMPS2_REGU_REG                  0x04
#define AB8500_CORE_VSMPS3_REGU_REG                  0x05
#define AB8500_CORE_VPLL_VANA_REGU_REG               0x06
#define AB8500_CORE_VREF_DDR_REG                     0x07
#define AB8500_CORE_EXT_SUPPLY_REGU_REG              0x08
#define AB8500_CORE_VAUX12_REGU_REG                  0x09
#define AB8500_CORE_RF1_VAUX3_REGU_REG               0x0A
#define AB8500_CORE_VARM_SEL1_REG                    0x0B
#define AB8500_CORE_VARM_SEL2_REG                    0x0C
#define AB8500_CORE_VARM_SEL3_REG                    0x0D
#define AB8500_CORE_VAPE_SEL1_REG                    0x0E
#define AB8500_CORE_VAPE_SEL2_REG                    0x0F
#define AB8500_CORE_VAPE_SEL3_REG                    0x10
#define AB8500_CORE_VBB_SEL1_REG                     0x11
#define AB8500_CORE_VBB_SEL2_REG                     0x12
#define AB8500_CORE_VSMPS1_SEL1_REG                  0x13
#define AB8500_CORE_VSMPS1_SEL2_REG                  0x14
#define AB8500_CORE_VSMPS1_SEL3_REG                  0x15
#define AB8500_CORE_VSMPS2_SEL1_REG                  0x17
#define AB8500_CORE_VSMPS2_SEL2_REG                  0x18
#define AB8500_CORE_VSMPS2_SEL3_REG                  0x19
#define AB8500_CORE_VSMPS3_SEL1_REG                  0x1B
#define AB8500_CORE_VSMPS3_SEL2_REG                  0x1C
#define AB8500_CORE_VSMPS3_SEL3_REG                  0x1D
#define AB8500_CORE_VAUX1_SEL_REG                    0x1F
#define AB8500_CORE_VAUX2_SEL_REG                    0x20
#define AB8500_CORE_VRF1_VAUX3_SEL_REG               0x21
#define AB8500_CORE_REGU_CTRL2_SPARE_REG             0x22

#define AB8500_CORE_VMOD_REGU_REG                    0x40
#define AB8500_CORE_VMOD_SEL1_REG                    0x41
#define AB8500_CORE_VMOD_SEL2_REG                    0x42
#define AB8500_CORE_REGU_CTRL_DISCH_REG              0x43
#define AB8500_CORE_REGU_CTRL_DISCH2_REG             0x44

/*------ OFFSETS IN STw4500 AB8500_CORE_ARM_REGU1_REG FIELDS-----------*/
#define AB8500_CORE_VARM_REGU                        0x03
#define sbAB8500_CORE_VARM_REGU                      0x00

#define AB8500_CORE_VARM_CTRL_SEL                    0x0C
#define sbAB8500_CORE_VARM_CTRL_SEL                  0x02
   
#define AB8500_CORE_VBBP_SEL_CTRL                    0x10
#define sbAB8500_CORE_VBBP_SEL_CTRL                  0x04

#define AB8500_CORE_VBBN_SEL_CTRL                    0x20
#define sbAB8500_CORE_VBBN_SEL_CTRL                  0x05

#define AB8500_CORE_VARM_AUTO_MODE                   0x40
#define sbAB8500_CORE_VARM_AUTO_MODE                 0x06

#define AB8500_CORE_VARM_PWM_MODE                    0x80
#define sbAB8500_CORE_VARM_PWM_MODE                  0x07

/*------ OFFSETS IN STw4500 AB8500_CORE_ARM_REGU2_REG FIELDS-----------*/
#define AB8500_CORE_VBBP_REGU                        0x03
#define sbAB8500_CORE_VBBP_REGU                      0x00

#define AB8500_CORE_VBBN_REGU                        0x0C
#define sbAB8500_CORE_VBBN_REGU                      0x02


/*------ OFFSETS IN STw4500 AB8500_CORE_VAPE_REGU_REG FIELDS-----------*/
#define AB8500_CORE_VAPE_REGU                        0x03
#define sbAB8500_CORE_VAPE_REGU                      0x00

#define AB8500_CORE_VAPE_CTRL_SEL                    0x04
#define sbAB8500_CORE_VAPE_CTRL_SEL                  0x02

#define AB8500_CORE_VAPE_AUTO_MODE                   0x08
#define sbAB8500_CORE_VAPE_AUTO_MODE                 0x03

#define AB8500_CORE_VAPE_PWM_MODE                    0x10
#define sbAB8500_CORE_VAPE_PWM_MODE                  0x04

#define AB8500_CORE_VAPE3_CTRL_SEL                   0x20
/*------ OFFSETS IN STw4500 AB8500_CORE_VSMPS*_REGU_REG FIELDS-----------*/
#define AB8500_CORE_VSMPS_REGU                       0x03
#define sbAB8500_CORE_VSMPS_REGU                     0x00

#define AB8500_CORE_VSMPS_CTRL_SEL                   0x0C
#define sbAB8500_CORE_VSMPS_CTRL_SEL                 0x02

#define AB8500_CORE_VSMPS_AUTO_MODE                  0x10
#define sbAB8500_CORE_VSMPS_AUTO_MODE                0x04

#define AB8500_CORE_VSMPS_PWM_MODE                   0x20
#define sbAB8500_CORE_VSMPS_PWM_MODE                 0x05

/*------ OFFSETS IN STw4500 AB8500_CORE_VPLL_VANA_REGU_REG FIELDS-----------*/
#define AB8500_CORE_VPLL_REGU                        0x03
#define sbAB8500_CORE_VPLL_REGU                      0x00

#define AB8500_CORE_VANA_REGU                        0x0C
#define sbAB8500_CORE_VANA_REGU                      0x02

/*------ OFFSETS IN STw4500 AB8500_CORE_VREF_DDR_REG FIELDS-----------*/
#define AB8500_CORE_VREF_DDR_ENA                     0x01
#define sbAB8500_CORE_VREF_DDR_ENA                   0x00

#define AB8500_CORE_VREF_DDR_SLEEPMODE               0x02
#define sbAB8500_CORE_VREF_DDR_SLEEPMODE             0x01


/*------ OFFSETS IN STw4500 AB8500_CORE_EXT_SUPPLY_REGU_REG FIELDS-----------*/
#define AB8500_CORE_VEXT_SUPPLY_1_REGU               0x03
#define sbAB8500_CORE_VEXT_SUPPLY_1_REGU             0x00

#define AB8500_CORE_VEXT_SUPPLY_2_REGU               0x0C
#define sbAB8500_CORE_VEXT_SUPPLY_2_REGU             0x02

#define AB8500_CORE_VEXT_SUPPLY_3_REGU               0x30
#define sbAB8500_CORE_VEXT_SUPPLY_3_REGU             0x04


/*------ OFFSETS IN STw4500 AB8500_CORE_VAUX12_REGU_REG FIELDS-----------*/
#define AB8500_CORE_VAUX_1_REGU                      0x03
#define sbAB8500_CORE_VAUX_1_REGU                    0x00

#define AB8500_CORE_VAUX_2_REGU                      0x0C
#define sbAB8500_CORE_VAUX_2_REGU                    0x02

/*------ OFFSETS IN STw4500 AB8500_CORE_RF1_VAUX3_REGU_REG FIELDS-----------*/
#define AB8500_CORE_VAUX_3_REGU                      0x03
#define sbAB8500_CORE_VAUX_3_REGU                    0x00

#define AB8500_CORE_VRF_1_REGU                       0x0C
#define sbAB8500_CORE_VRF_1_REGU                     0x02

/*------ OFFSETS IN STw4500 AB8500_CORE_VARM_SEL_REG FIELDS-----------*/
#define AB8500_CORE_VARM_APE_SMPS_SEL                0x3F
#define sbAB8500_CORE_VARM_APE_SMPS_SEL              0x00

/*------ OFFSETS IN STw4500 AB8500_CORE_VBB_SEL*_REG FIELDS-----------*/
#define AB8500_CORE_VBBN_SEL                         0x0F
#define sbAB8500_CORE_VBBN_SEL                       0x00

#define AB8500_CORE_VBBP_SEL                         0xF0
#define sbAB8500_CORE_VBBP_SEL                       0x04

/*------ OFFSETS IN STw4500 AB8500_CORE_VAUX*_SEL_REG FIELDS-----------*/
#define AB8500_CORE_VAUX_SEL                         0x0F
#define sbAB8500_CORE_VAUX_SEL                       0x00

/*------ OFFSETS IN STw4500 AB8500_CORE_VRF1_VAUX3_SEL_REG FIELDS-----------*/
#define AB8500_CORE_VRF1_SEL                         0x30
#define sbAB8500_CORE_VRF1_SEL                       0x04


/*------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------AB8500_CORE_USB_BANK(BANK5)-----------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
#define AB8500_CORE_USB_OTG_CTRL_REG                 0x87


/*------ OFFSETS IN STw4500 AB8500_CORE_USB_OTG_CTRL_REG FIELDS-----------*/
#define AB8500_CORE_ID_DEV_ENA                       0x01
#define sbAB8500_CORE_ID_DEV_ENA                     0x00

#define AB8500_CORE_ID_HOST_ENA                      0x02
#define sbAB8500_CORE_ID_HOST_ENA                    0x01

/*------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------AB8500_CORE_TVOUT_BANK(BANK6)-----------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
#define AB8500_CORE_TVOUT_CTRL_REG                   0x80

/*------ OFFSETS IN STw4500 AB8500_CORE_TVOUT_CTRL_REG FIELDS-----------*/
#define AB8500_CORE_TVOUT_DACCTRL_1_0                0x03
#define sbAB8500_CORE_TVOUT_DACCTRL_1_0              0x00

#define AB8500_CORE_PLUG_TVON                        0x04
#define sbAB8500_CORE_PLUG_TVON                      0x02

#define AB8500_CORE_PLUG_TV_TIME                     0x38
#define sbAB8500_CORE_PLUG_TV_TIME                   0x03

#define AB8500_CORE_TVLOAD_RC                        0x40
#define sbAB8500_CORE_TVLOAD_RC                      0x06

/*------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------AB8500_CORE_DBI_BANK(BANK7)-------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
#define AB8500_CORE_DBI_REG1                         0x00
#define AB8500_CORE_DBI_REG2                         0x01

/*------------------------------------------------------------------------------------------------------------*/
/*---------------------------------AB8500_CORE_ECI_AV_ACC_DET_BANK(BANK8)-------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
#define AB8500_CORE_ECI_CTRL_REG                     0x00
#define AB8500_CORE_ECI_HOOK_LEVEL_REG               0x01
#define AB8500_CORE_ECI_DATAOUT_REG                  0x02
#define AB8500_CORE_ECI_DATAIN_REG                   0x03







/*------------------------------------------------------------------------
 *  Private Functions declaration
 *----------------------------------------------------------------------*/
PRIVATE t_ab8500_core_error Ab8500_CORE_InterpretTurnOnCause(IN t_uint8 , OUT t_ab8500_core_turnon_cause * );
/*------------------------------------------------------------------------
 * Structures
 *----------------------------------------------------------------------*/


#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __AB8500_CORE_P_H__ */

/* End of file - ab8500_core_p.h */


