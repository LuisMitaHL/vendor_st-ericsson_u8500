/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header for DIF Panel Driver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _DIF_PANEL_C3_TPO_P_H_
#define _DIF_PANEL_C3_TPO_P_H_

/* Register Indexes for TPO */
#define DIF_PANEL_C3_TPO_OSCILLATION_SET            0x000
#define DIF_PANEL_C3_TPO_DRV_OP_CTRL_SET            0x001
#define DIF_PANEL_C3_TPO_LCD_DRV_SIGNAL_SET         0x002
#define DIF_PANEL_C3_TPO_ENTRY_MODE                 0x003
#define DIF_PANEL_C3_TPO_HOR_VALID_SET              0x006
#define DIF_PANEL_C3_TPO_DISP_MODE_1                0x007
#define DIF_PANEL_C3_TPO_DISP_MODE_2                0x008
#define DIF_PANEL_C3_TPO_DISP_MODE_3                0x009
#define DIF_PANEL_C3_TPO_DISP_MODE_4                0x00B
#define DIF_PANEL_C3_TPO_EXT_DISP_SIGNAL_SET_1      0x00C
#define DIF_PANEL_C3_TPO_FR_FREQ_ADJ_SET            0x00D
#define DIF_PANEL_C3_TPO_EXT_DISP_SIGNAL_SET_2      0x00E
#define DIF_PANEL_C3_TPO_EXT_DISP_SIGNAL_SET_3      0x00F

#define DIF_PANEL_C3_TPO_LTPS_CTRL_SET_1            0x012
#define DIF_PANEL_C3_TPO_LTPS_CTRL_SET_2            0x013
#define DIF_PANEL_C3_TPO_LTPS_CTRL_SET_3            0x014
#define DIF_PANEL_C3_TPO_LTPS_CTRL_SET_4            0x015
#define DIF_PANEL_C3_TPO_LTPS_CTRL_SET_5            0x018
#define DIF_PANEL_C3_TPO_LTPS_CTRL_SET_6            0x019
#define DIF_PANEL_C3_TPO_LTPS_CTRL_SET_7            0x01A
#define DIF_PANEL_C3_TPO_LTPS_CTRL_SET_8            0x01B
#define DIF_PANEL_C3_TPO_AMP_CAP_SET                0x01C
#define DIF_PANEL_C3_TPO_MODE_SET                   0x01D
#define DIF_PANEL_C3_TPO_POW_OFF_LINE_COUNT_SET     0x01E

#define DIF_PANEL_C3_TPO_DISP_CTRL                  0x100
#define DIF_PANEL_C3_TPO_AUTO_SEQ_CTRL              0x101

#define DIF_PANEL_C3_TPO_POW_SUPPLY_CTRL_1          0x102
#define DIF_PANEL_C3_TPO_POW_SUPPLY_CTRL_2          0x103
#define DIF_PANEL_C3_TPO_POW_SUPPLY_CTRL_3          0x104
#define DIF_PANEL_C3_TPO_POW_SUPPLY_CTRL_4          0x105
#define DIF_PANEL_C3_TPO_EXT_POLARITY_CTRL          0x108

#define DIF_PANEL_C3_TPO_RAM_ADD_SET_1              0x200
#define DIF_PANEL_C3_TPO_RAM_ADD_SET_2              0x201

#define DIF_PANEL_C3_TPO_RAM_DATA_WRITE             0x202

#define DIF_PANEL_C3_TPO_GRAPHIC_OP_1               0x203
#define DIF_PANEL_C3_TPO_GRAPHIC_OP_2               0x204

#define DIF_PANEL_C3_TPO_GRAY_SCALE_SET_1           0x300
#define DIF_PANEL_C3_TPO_GRAY_SCALE_SET_2           0x301
#define DIF_PANEL_C3_TPO_GRAY_SCALE_SET_3           0x302
#define DIF_PANEL_C3_TPO_GRAY_SCALE_SET_4           0x303
#define DIF_PANEL_C3_TPO_GRAY_SCALE_SET_5           0x304
#define DIF_PANEL_C3_TPO_GRAY_SCALE_OFFSET_SET      0x305

#define DIF_PANEL_C3_TPO_VER_SCROLL_CTRL_1          0x400
#define DIF_PANEL_C3_TPO_VER_SCROLL_CTRL_2          0x401
#define DIF_PANEL_C3_TPO_FIRST_SCREEN_START_ADDR    0x402
#define DIF_PANEL_C3_TPO_FIRST_SCREEN_END_ADDR      0x403
#define DIF_PANEL_C3_TPO_SECOND_SCREEN_START_ADDR   0x404
#define DIF_PANEL_C3_TPO_SECOND_SCREEN_END_ADDR     0x405
#define DIF_PANEL_C3_TPO_HOR_RAM_START_ADDR         0x406
#define DIF_PANEL_C3_TPO_HOR_RAM_END_ADDR           0x407
#define DIF_PANEL_C3_TPO_VER_RAM_START_ADDR         0x408
#define DIF_PANEL_C3_TPO_VER_RAM_END_ADDR           0x409

#define DIF_PANEL_C3_TPO_OSD_ON_OFF                 0x500
#define DIF_PANEL_C3_TPO_OSD_SCREEN_1_START_ADD     0x504
#define DIF_PANEL_C3_TPO_OSD_SCREEN_2_START_ADD     0x505
#define DIF_PANEL_C3_TPO_NOP                        0x5FF

#define DIF_PANEL_PUT_BITS(reg_name, val, shift, mask) \
        ((reg_name) = (((reg_name) &~(mask)) | (((t_uint32) (val) << shift) & (mask))))

/*-----------------------------------------------------------------
   Register Mask and bit Position 

 ------------------------------------------------------------------*/
/* Entry Mode Settings */
/* AM*/
#define DIF_PANEL_AM_POS    3
#define DIF_PANEL_ID0_POS   4
#define DIF_PANEL_ID1_POS   5
#define DIF_PANEL_HWM_POS   8
#define DIF_PANEL_IF18_POS  11
#define DIF_PANEL_BGR_POS   12
#define DIF_PANEL_DFM0_POS  13
#define DIF_PANEL_DFM1_POS  14
#define DIF_PANEL_TRI_POS   15

/* Horizontal valid width settings , For Resolution*/
#define DIF_PANEL_HBP_POS   0
#define DIF_PANEL_HBP_MASK  (0x1F)
#define DIF_PANEL_HWS_POS   8
#define DIF_PANEL_HWS_MASK  (0xFF)

/* Driver Output Control Setting , For Resolution*/
#define DIF_PANEL_SS_POS    8
#define DIF_PANEL_NL_POS    0
#define DIF_PANEL_NL_MASK   (0x3F)

/* OSD ON/OFF  Settings */
#define DIF_PANEL_OSDON_POS 0
#define DIF_PANEL_OSDW_POS  8

/* FR period adjustment setting */
#define DIF_PANEL_DIVI_POS  8
#define DIF_PANEL_DIVI_MASK (MASK_BIT9 | MASK_BIT8)
#define DIF_PANEL_RTNI_POS  0
#define DIF_PANEL_RTNI_MASK (0x1F)

/* Display Mode 1 Settings */
#define DIF_PANEL_REV_POS   2
#define DIF_PANEL_NBW_POS   4
#define DIF_PANEL_PT0_POS   6
#define DIF_PANEL_PT1_POS   7
#define DIF_PANEL_SPT_POS   8
#define DIF_PANEL_VLE1_POS  9
#define DIF_PANEL_VLE2_POS  10
#define DIF_PANEL_COL_POS   14
#define DIF_PANEL_COL_MASK  (MASK_BIT15 | MASK_BIT14)

/* Display Mode 2 Settings */
#define DIF_PANEL_BP_POS    0
#define DIF_PANEL_BP_MASK   (MASK_BIT3 | MASK_BIT2 | MASK_BIT1 | MASK_BIT0)
#define DIF_PANEL_FP_POS    8
#define DIF_PANEL_FP_MASK   (0xFF)

/* Display Mode 3 Settings */
#define DIF_PANEL_RSE_POS   7
#define DIF_PANEL_RSB_POS   4
#define DIF_PANEL_RSB_MASK  (MASK_BIT5 | MASK_BIT4)
#define DIF_PANEL_RSH_POS   0
#define DIF_PANEL_RSH_MASK  (MASK_BIT3 | MASK_BIT2 | MASK_BIT1 | MASK_BIT0)

/* Display Mode 4 Settings */
#define DIF_PANEL_SIP_POS   0
#define DIF_PANEL_X2Y2_POS  5
#define DIF_PANEL_X2Y2_MASK (MASK_BIT6 | MASK_BIT5)
#define DIF_PANEL_XYON_POS  7

/* External Display Signal Setting(1) */
#define DIF_PANEL_RM_POS    8
#define DIF_PANEL_RIM_POS   0
#define DIF_PANEL_RIM_MASK  (MASK_BIT9 | MASK_BIT8)
#define DIF_PANEL_DM_POS    4
#define DIF_PANEL_DM_MASK   (MASK_BIT5 | MASK_BIT4)

/* External Display Signal Setting(2) */
#define DIF_PANEL_DIVE_POS  8
#define DIF_PANEL_DIVE_MASK (MASK_BIT1 | MASK_BIT0)
#define DIF_PANEL_RTNE_POS  0
#define DIF_PANEL_RTNE_MASK (0xFF)

/* External Display Signal Setting(3) */
#define DIF_PANEL_DPL_POS   0
#define DIF_PANEL_EPL_POS   1
#define DIF_PANEL_VPL_POS   2
#define DIF_PANEL_HSPL_POS  3
#define DIF_PANEL_VSPL_POS  4

/* LTPS Control Settings 1 */
/* CLTI*/
#define DIF_PANEL_CLTI_POS  0
#define DIF_PANEL_CLTI_MASK (MASK_BIT1 | MASK_BIT0)

/* CLWI*/
#define DIF_PANEL_CLWI_POS  8
#define DIF_PANEL_CLWI_MASK (MASK_BIT8 | MASK_BIT9 | MASK_BIT10 | MASK_BIT11)

/* LTPS Control Settings 2 */
/* OEVFI */
#define DIF_PANEL_OEVFI_POS     0
#define DIF_PANEL_OEVFI_MASK    (MASK_BIT1 | MASK_BIT0)

/* OEVBI*/
#define DIF_PANEL_OEVBI_POS     8
#define DIF_PANEL_OEVBI_MASK    (MASK_BIT8 | MASK_BIT9)

/* LTPS Control Settings 3 */
#define DIF_PANEL_SHI_POS   0
#define DIF_PANEL_SHI_MASK  (MASK_BIT1 | MASK_BIT0)

/* LTPS Control Settings 4 */
/* CKBI */
#define DIF_PANEL_CKBI_POS  12
#define DIF_PANEL_CKBI_MASK (MASK_BIT12 | MASK_BIT13)

/* CKFI*/
#define DIF_PANEL_CKFI_POS  8
#define DIF_PANEL_CKFI_MASK (MASK_BIT8 | MASK_BIT9)

/* LTPS Control Settings 5 */
/* CLTE */
#define DIF_PANEL_CLTE_POS  0
#define DIF_PANEL_CLTE_MASK (MASK_BIT0 | MASK_BIT1 | MASK_BIT2 | MASK_BIT3)

/* CLWE*/
#define DIF_PANEL_CLWE_POS  8
#define DIF_PANEL_CLWE_MASK (MASK_BIT8 | MASK_BIT9 | MASK_BIT10 | MASK_BIT11 | MASK_BIT12 | MASK_BIT13)

/* LTPS Control Settings 6 */
/* OEVBE */
#define DIF_PANEL_OEVBE_POS     8
#define DIF_PANEL_OEVBE_MASK    (MASK_BIT8 | MASK_BIT9 | MASK_BIT10 | MASK_BIT11)

/* OEVFE */
#define DIF_PANEL_OEVFE_POS     0
#define DIF_PANEL_OEVFE_MASK    (MASK_BIT0 | MASK_BIT1 | MASK_BIT2 | MASK_BIT3)

/* LTPS Control Settings 7 */
/* SHE */
#define DIF_PANEL_SHE_POS   0
#define DIF_PANEL_SHE_MASK  (MASK_BIT0 | MASK_BIT1 | MASK_BIT2 | MASK_BIT3)

/* LTPS Control Settings 8 */
/* CKBE */
#define DIF_PANEL_CKBE_POS  12
#define DIF_PANEL_CKBE_MASK (MASK_BIT12 | MASK_BIT13 | MASK_BIT14 | MASK_BIT15)

/* CKFE */
#define DIF_PANEL_CKFE_POS  8
#define DIF_PANEL_CKFE_MASK (MASK_BIT8 | MASK_BIT9 | MASK_BIT10 | MASK_BIT11)

/*DISPLAY Control */
/* ASW1-0*/
#define DIF_PANEL_ASW_POS   6
#define DIF_PANEL_ASW_MASK  (MASK_BIT6 | MASK_BIT7)

/*Auto Sequence Control*/
#define DIF_PANEL_AUTO_POS  0

/*Oscillation Setting*/
#define DIF_PANEL_OSC_POS   0

/*LCD Driving Signal*/
#define DIF_PANEL_LCD_DRV_POS   9

/*Amplifier Capability Setting*/
#define DIF_PANEL_LCD_AMP_POS   0
#define DIF_PANEL_LCD_AMP_MASK  (MASK_BIT1 | MASK_BIT0)

/*D1-0*/
#define DIF_PANEL_OP_PIN_POS    4
#define DIF_PANEL_OP_PIN_MASK   (MASK_BIT4 | MASK_BIT5)
#define DIF_PANEL_PO_PIN_POS    15
#define DIF_PANEL_CONT_POS      14
#define DIF_PANEL_PEV_POS       13
#define DIF_PANEL_DCEV_POS      12
#define DIF_PANEL_UD_POS        11
#define DIF_PANEL_CON_POS       10
#define DIF_PANEL_OEV_POS       9
#define DIF_PANEL_VCS_POS       8
#define DIF_PANEL_FR_POS        3
#define DIF_PANEL_FDON_POS      2
#define DIF_PANEL_VGAM_POS      1
#define DIF_PANEL_DCG_POS       0

/* Power off line settings */
#define DIF_PANEL_POFH_POS  0
#define DIF_PANEL_POFH_MASK (MASK_BIT0 | MASK_BIT1 | MASK_BIT2 | MASK_BIT3)

/* Power Supply*/
/* Power Supply control (1) */
#define DIF_PANEL_VCOND_POS     8
#define DIF_PANEL_VCOND_MASK    (MASK_BIT8 | MASK_BIT9 | MASK_BIT10 | MASK_BIT11)
#define DIF_PANEL_VCS_OP_POS    4
#define DIF_PANEL_VCS_OP_MASK   (MASK_BIT4 | MASK_BIT5 | MASK_BIT6 | MASK_BIT7)
#define DIF_PANEL_VGM_POS       0
#define DIF_PANEL_VGM_MASK      (MASK_BIT0 | MASK_BIT1 | MASK_BIT2 | MASK_BIT3)

/* Power supply control (2) */
#define DIF_PANEL_XVDD_POS  0
#define DIF_PANEL_XVDD_MASK (MASK_BIT0 | MASK_BIT1 | MASK_BIT2)
#define DIF_PANEL_WSEL1_POS 12
#define DIF_PANEL_WSEL2_POS 13

/* Power supply control (3) */
#define DIF_PANEL_BAV_POS   0
#define DIF_PANEL_BXV_POS   1

/* Power supply control (4) */
#define DIF_PANEL_AVDD_REG_POS  0
#define DIF_PANEL_AVDD_REG_MASK (MASK_BIT0 | MASK_BIT1)
#define DIF_PANEL_XVDD_REG_POS  2
#define DIF_PANEL_XVDD_REG_MASK (MASK_BIT2 | MASK_BIT3)
#define DIF_PANEL_EXT_REG_POS   4
#define DIF_PANEL_EXT_REG_MASK  (MASK_BIT4 | MASK_BIT5)
#define DIF_PANEL_DCW_POS       8
#define DIF_PANEL_DCW_MASK      (MASK_BIT8 | MASK_BIT9)

/*Ext Polarity control */
#define DIF_PANEL_EXTC1_POS 0
#define DIF_PANEL_EXTC2_POS 1

/* Setting for gray scale offset */
#define DIF_PANEL_BLON_POS  8
#define DIF_PANEL_BUP_POS   4
#define DIF_PANEL_BUP_MASK  (MASK_BIT4 | MASK_BIT5 | MASK_BIT6)
#define DIF_PANEL_BOFS_POS  0
#define DIF_PANEL_BOFS_MASK (MASK_BIT0 | MASK_BIT1 | MASK_BIT2)

/* Graphic operation (1)(2)*/
#define DIF_PANEL_BLUE_POS      0
#define DIF_PANEL_BLUE_MASK     (MASK_BIT0 | MASK_BIT1 | MASK_BIT2 | MASK_BIT3 | MASK_BIT4 | MASK_BIT5)
#define DIF_PANEL_GREEN_POS     8
#define DIF_PANEL_GREEN_MASK    (MASK_BIT8 | MASK_BIT9 | MASK_BIT10 | MASK_BIT11 | MASK_BIT12 | MASK_BIT13)

/*Mode setting */
#define DIF_PANEL_DSTB_POS  2
#define DIF_PANEL_STB_POS   0

/* Gray scale setting */
/* Gray scale setting (1)*/
#define DIF_PANEL_PK0_POS   0
#define DIF_PANEL_PK0_MASK  (MASK_BIT0 | MASK_BIT1 | MASK_BIT2)
#define DIF_PANEL_PK1_POS   8
#define DIF_PANEL_PK1_MASK  (MASK_BIT8 | MASK_BIT9 | MASK_BIT10)

/* Gray scale setting (2)*/
#define DIF_PANEL_PK2_POS   0
#define DIF_PANEL_PK2_MASK  (MASK_BIT0 | MASK_BIT1 | MASK_BIT2)
#define DIF_PANEL_PK3_POS   8
#define DIF_PANEL_PK3_MASK  (MASK_BIT8 | MASK_BIT9 | MASK_BIT10)

/* Gray scale setting (3)*/
#define DIF_PANEL_PK4_POS   0
#define DIF_PANEL_PK4_MASK  (MASK_BIT0 | MASK_BIT1 | MASK_BIT2)
#define DIF_PANEL_PK5_POS   8
#define DIF_PANEL_PK5_MASK  (MASK_BIT8 | MASK_BIT9 | MASK_BIT10)

/* Gray scale setting (4)*/
#define DIF_PANEL_PR0_POS   0
#define DIF_PANEL_PR0_MASK  (MASK_BIT0 | MASK_BIT1 | MASK_BIT2)
#define DIF_PANEL_PR1_POS   8
#define DIF_PANEL_PR1_MASK  (MASK_BIT8 | MASK_BIT9 | MASK_BIT10)

/* Gray scale setting (5)*/
#define DIF_PANEL_VR0_POS   0
#define DIF_PANEL_VR0_MASK  (MASK_BIT0 | MASK_BIT1 | MASK_BIT2)
#define DIF_PANEL_VR1_POS   8
#define DIF_PANEL_VR1_MASK  (MASK_BIT8 | MASK_BIT9 | MASK_BIT10)
#endif /*_DIF_PANEL_C3_TPO_P_H_*/

