/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Header file for DIF Panel Driver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _DIF_PANEL_C3_TPO_H_
#define _DIF_PANEL_C3_TPO_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "hcl_defs.h"
//#include "dif.h"
#include "dif_panel_c3_tpo.h"

/*---------------------------------------------------------------------------
 * Enums
 *---------------------------------------------------------------------------*/
/* general */
typedef enum
{
    DIF_INTERNAL_EVENT              = HCL_INTERNAL_EVENT,
    DIF_REMAINING_PENDING_EVENTS    = HCL_REMAINING_PENDING_EVENTS,
    DIF_NO_MORE_PENDING_EVENT       = HCL_NO_MORE_PENDING_EVENT,
    DIF_NO_MORE_FILTER_PENDING_EVENT= HCL_NO_MORE_FILTER_PENDING_EVENT,
    DIF_NO_PENDING_EVENT_ERROR      = HCL_NO_PENDING_EVENT_ERROR,
    DIF_OK                          = HCL_OK,
    DIF_ERROR                       = HCL_ERROR,
    DIF_UNSUPPORTED_HW              = HCL_UNSUPPORTED_HW,
    DIF_UNSUPPORTED_FEATURE         = HCL_UNSUPPORTED_FEATURE,
    DIF_INVALID_PARAMETER           = HCL_INVALID_PARAMETER,
    DIF_REQUEST_NOT_APPLICABLE      = HCL_REQUEST_NOT_APPLICABLE,
    DIF_REQUEST_PENDING             = HCL_REQUEST_PENDING,
    DIF_NOT_CONFIGURED              = HCL_NOT_CONFIGURED,
    DIF_INTERNAL_ERROR              = HCL_INTERNAL_ERROR,
    DIF_BAD_ID                      = -10,
    DIF_FRAME_DESC_BUFFER_FULL      = -11,
    DIF_FRAME_DESC_BUFFER_EMPTY     = -12
} t_dif_error;

typedef enum
{
    DIF_PANEL_C3_TPO_BIT_DISABLE= 0x0,
    DIF_PANEL_C3_TPO_BIT_ENABLE
} t_dif_panel_c3_tpo_state;

typedef enum
{
    DIF_PANEL_C3_TPO_SCROLL_SCREEN1 = 0x0,
    DIF_PANEL_C3_TPO_SCROLL_SCREEN2
} t_dif_panel_c3_tpo_scroll_screen;

/* Oscillation Setting*/
typedef enum
{
    DIF_PANEL_C3_TPO_OSC_OFF,
    DIF_PANEL_C3_TPO_OSC_ON
} t_dif_panel_c3_tpo_oscillation;

/* Driver output control setting */
typedef enum
{
    /*DIF_PANEL_C3_TPO_DISP_INVALID_SIZE =0x0,*/
    DIF_PANEL_C3_TPO_DISP_256_16_SIZE   = 0x1,
    DIF_PANEL_C3_TPO_DISP_256_24_SIZE   = 0x2,
    DIF_PANEL_C3_TPO_DISP_256_32_SIZE   = 0x3,
    DIF_PANEL_C3_TPO_DISP_256_40_SIZE   = 0x4,
    DIF_PANEL_C3_TPO_DISP_256_48_SIZE   = 0x5,
    DIF_PANEL_C3_TPO_DISP_256_56_SIZE   = 0x6,
    DIF_PANEL_C3_TPO_DISP_256_64_SIZE   = 0x7,
    DIF_PANEL_C3_TPO_DISP_256_72_SIZE   = 0x8,
    DIF_PANEL_C3_TPO_DISP_256_80_SIZE   = 0x9,
    DIF_PANEL_C3_TPO_DISP_256_88_SIZE   = 0xA,
    DIF_PANEL_C3_TPO_DISP_256_96_SIZE   = 0xB,
    DIF_PANEL_C3_TPO_DISP_256_104_SIZE  = 0xC,
    DIF_PANEL_C3_TPO_DISP_256_112_SIZE  = 0xD,
    DIF_PANEL_C3_TPO_DISP_256_120_SIZE  = 0xE,
    DIF_PANEL_C3_TPO_DISP_256_128_SIZE  = 0xF,
    DIF_PANEL_C3_TPO_DISP_256_136_SIZE  = 0x10,
    DIF_PANEL_C3_TPO_DISP_256_144_SIZE  = 0x11,
    DIF_PANEL_C3_TPO_DISP_256_152_SIZE  = 0x12,
    DIF_PANEL_C3_TPO_DISP_256_160_SIZE  = 0x13,
    DIF_PANEL_C3_TPO_DISP_256_168_SIZE  = 0x14,
    DIF_PANEL_C3_TPO_DISP_256_176_SIZE  = 0x15,
    DIF_PANEL_C3_TPO_DISP_256_184_SIZE  = 0x16,
    DIF_PANEL_C3_TPO_DISP_256_192_SIZE  = 0x17,
    DIF_PANEL_C3_TPO_DISP_256_200_SIZE  = 0x18,
    DIF_PANEL_C3_TPO_DISP_256_208_SIZE  = 0x19,
    DIF_PANEL_C3_TPO_DISP_256_216_SIZE  = 0x1A,
    DIF_PANEL_C3_TPO_DISP_256_224_SIZE  = 0x1B,
    DIF_PANEL_C3_TPO_DISP_256_232_SIZE  = 0x1C,
    DIF_PANEL_C3_TPO_DISP_256_240_SIZE  = 0x1D,
    DIF_PANEL_C3_TPO_DISP_256_248_SIZE  = 0x1E,
    DIF_PANEL_C3_TPO_DISP_256_256_SIZE  = 0x1F,
    DIF_PANEL_C3_TPO_DISP_256_264_SIZE  = 0x20,
    DIF_PANEL_C3_TPO_DISP_256_272_SIZE  = 0x21,
    DIF_PANEL_C3_TPO_DISP_256_280_SIZE  = 0x22,
    DIF_PANEL_C3_TPO_DISP_256_288_SIZE  = 0x23,
    DIF_PANEL_C3_TPO_DISP_256_296_SIZE  = 0x24,
    DIF_PANEL_C3_TPO_DISP_256_304_SIZE  = 0x25,
    DIF_PANEL_C3_TPO_DISP_256_312_SIZE  = 0x26,
    DIF_PANEL_C3_TPO_DISP_256_320_SIZE  = 0x27,

    /*
  When OSD Command is off,TPO can use OSD RAM area for graphic display.
  So dispaly area size expands 256*320 to 256*352 dots.
  */
    DIF_PANEL_C3_TPO_DISP_256_328_SIZE  = 0x28,
    DIF_PANEL_C3_TPO_DISP_256_336_SIZE  = 0x29,
    DIF_PANEL_C3_TPO_DISP_256_344_SIZE  = 0x2A,
    DIF_PANEL_C3_TPO_DISP_256_352_SIZE  = 0x2B,

    /*DIF_PANEL_C3_TPO_DISP_MAX_INVALID_SIZE*/
} t_dif_panel_c3_tpo_disp_size;

/* LCD driving signal setting */
typedef enum
{
    DIF_PANEL_C3_TPO_FRAME_AC_WAVEFORM  = 0x0,
    DIF_PANEL_C3_TPO_LINE_AC_WAVEFORM   = 0x1
} t_dif_panel_c3_tpo_lcd_drv_signal;

/* Display Mode */
/* Display Mode 1 */
typedef enum
{
    DIF_PANEL_C3_TPO_262K_COLORS        = 0x0,
    DIF_PANEL_C3_TPO_65K_COLORS         = 0x1,
    DIF_PANEL_C3_TPO_8_COLORS           = 0x3,
} t_dif_panel_c3_tpo_disp_color_mode;

/* VLE2 VLE1*/
typedef enum
{
    DIF_PANEL_C3_TPO_FIXED_DISP         = 0x0,
    DIF_PANEL_C3_TPO_SCROLL_DISP
} t_dif_panel_c3_tpo_scroll_feature;

/* Display Mode 3 */
typedef enum
{
    DIF_PANEL_C3_TPO_MAGN_FACTOR_2      = 0x0,
    DIF_PANEL_C3_TPO_MAGN_FACTOR_4,
    DIF_PANEL_C3_TPO_MAGN_FACTOR_8,
    DIF_PANEL_C3_TPO_MAGN_FACTOR_16
} t_dif_panel_c3_tpo_magn_factor;

/* Display Mode 4 */
typedef enum
{
    DIF_PANEL_C3_TPO_XY_EXP_DISABLED    = 0x0,
    DIF_PANEL_C3_TPO_Y_DIR_EXP,
    DIF_PANEL_C3_TPO_X_DIR_EXP,
    DIF_PANEL_C3_TPO_BOTH_DIR_EXP
} t_dif_panel_c3_tpo_xy_expansion;

/* External display signal setting */
/* External display signal setting 1 */
typedef enum
{
    DIF_PANEL_C3_TPO_MPU_OR_VSYNC       = 0x0,
    DIF_PANEL_C3_TPO_RGB
} t_dif_panel_c3_tpo_ram_access_interface;

typedef enum
{
    DIF_PANEL_C3_TPO_INTERNAL_CLK_OPER  = 0x0,
    DIF_PANEL_C3_TPO_RGB_INTERFACE,
    DIF_PANEL_C3_TPO_VSYNC_INTERFACE
} t_dif_panel_c3_tpo_sync_signal;

typedef enum
{
    DIF_PANEL_C3_TPO_18_BIT             = 0x0,
    DIF_PANEL_C3_TPO_16_BIT,
    DIF_PANEL_C3_TPO_6_BIT
} t_dif_panel_c3_tpo_interface_width;

/* External display signal setting 2 */
typedef enum
{
    /* DIF_PANEL_C3_TPO_DOTCLK_DIV_2 =0x0,*/
    DIF_PANEL_C3_TPO_DOTCLK_DIV_4       = 0x1,
    DIF_PANEL_C3_TPO_DOTCLK_DIV_8,
    DIF_PANEL_C3_TPO_DOTCLK_DIV_16
} t_dif_panel_c3_tpo_dotclk_divisor;

/* FR period adjustment setting */
typedef enum
{
    DIF_PANEL_C3_TPO_ONCHIP_DOTCLK_DIV_1= 0x0,
    DIF_PANEL_C3_TPO_ONCHIP_DOTCLK_DIV_2,
    DIF_PANEL_C3_TPO_ONCHIP_DOTCLK_DIV_4,
    DIF_PANEL_C3_TPO_ONCHIP_DOTCLK_DIV_8
} t_dif_panel_c3_tpo_onchip_osc_divisor;

/* LTPS Control Settings */
/* LTPS Control Settings 1 & 2 & 4 & 8 */
typedef enum
{
    DIF_PANEL_C3_TPO_POS_ZERO_CLK           = 0x0,
    DIF_PANEL_C3_TPO_POS_HALF_CLK,
    DIF_PANEL_C3_TPO_POS_ONE_CLK,
    DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_POS_TWO_CLK,
    DIF_PANEL_C3_TPO_POS_TWO_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_POS_THREE_CLK,
    DIF_PANEL_C3_TPO_POS_THREE_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_POS_FOUR_CLK,
    DIF_PANEL_C3_TPO_POS_FOUR_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_POS_FIVE_CLK,
    DIF_PANEL_C3_TPO_POS_FIVE_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_POS_SIX_CLK,
    DIF_PANEL_C3_TPO_POS_SIX_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_POS_SEVEN_CLK,
    DIF_PANEL_C3_TPO_POS_SEVEN_AND_HALF_CLK
} t_dif_panel_c3_tpo_clk_pos;

/* LTPS Control Settings 3& 7  */
typedef enum
{
    DIF_PANEL_C3_TPO_HALF_CLK               = 0x0,
    DIF_PANEL_C3_TPO_ONE_CLK,
    DIF_PANEL_C3_TPO_ONE_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_TWO_CLK,
    DIF_PANEL_C3_TPO_TWO_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_THREE_CLK,
    DIF_PANEL_C3_TPO_THREE_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_FOUR_CLK,
    DIF_PANEL_C3_TPO_FOUR_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_FIVE_CLK,
    DIF_PANEL_C3_TPO_FIVE_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_SIX_CLK,
    DIF_PANEL_C3_TPO_SIX_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_SEVEN_CLK,
    DIF_PANEL_C3_TPO_SEVEN_AND_HALF_CLK,
    DIF_PANEL_C3_TPO_EIGHT_CLK,
} t_dif_panel_c3_tpo_edge_hold_time;

/* Amplifier capability setting */
typedef enum
{
    DIF_PANEL_C3_TPO_MAX                    = 0x0,
    DIF_PANEL_C3_TPO_OFF,   /* Bias /AMP off mode */
    DIF_PANEL_C3_TPO_OPTIMUM,
    DIF_PANEL_C3_TPO_MINIMUM
} t_dif_panel_c3_tpo_amp_cap_set;

typedef enum
{
    /* DIF_PANEL_C3_TPO_INVALID_TIME=0x0,*/
    DIF_PANEL_C3_TPO_2H_TIME                = 0x1,
    DIF_PANEL_C3_TPO_3H_TIME,
    DIF_PANEL_C3_TPO_4H_TIME,
    DIF_PANEL_C3_TPO_5H_TIME,
    DIF_PANEL_C3_TPO_6H_TIME,
    DIF_PANEL_C3_TPO_7H_TIME,
    DIF_PANEL_C3_TPO_8H_TIME,
    DIF_PANEL_C3_TPO_9H_TIME,
    DIF_PANEL_C3_TPO_10H_TIME,
    DIF_PANEL_C3_TPO_11H_TIME,
    DIF_PANEL_C3_TPO_12H_TIME,
    DIF_PANEL_C3_TPO_13H_TIME,
    DIF_PANEL_C3_TPO_14H_TIME,
    DIF_PANEL_C3_TPO_15H_TIME,
    DIF_PANEL_C3_TPO_16H_TIME,
} t_dif_panel_c3_tpo_power_offline_set;

/* Dispaly Control */
typedef enum
{
    DIF_PANEL_C3_TPO_FIXED_VSS_0            = 0x0,
    DIF_PANEL_C3_TPO_FIXED_VSS_1            = 0x1,

    /* TBD */
    DIF_PANEL_C3_TPO_FIXED_VDD              = 0x2,
    DIF_PANEL_C3_TPO_NORMAL_DRIVE           = 0x3
} t_dif_panel_c3_tpo_asw_level;

typedef enum
{
    DIF_PANEL_C3_TPO_OP_VSS                 = 0x1,
    DIF_PANEL_C3_TPO_OP_WHITE_DATA,
    DIF_PANEL_C3_TPO_OP_NORMAL_DRIVE
} t_dif_panel_c3_tpo_output_pin_state;

/* power supply control */
/* power supply control 1*/
typedef enum
{
    DIF_PANEL_C3_TPO_VCOMD_1_4V             = 0x0,
    DIF_PANEL_C3_TPO_VCOMD_1_5V,
    DIF_PANEL_C3_TPO_VCOMD_1_6V,
    DIF_PANEL_C3_TPO_VCOMD_1_7V,
    DIF_PANEL_C3_TPO_VCOMD_1_8V,
    DIF_PANEL_C3_TPO_VCOMD_1_9V,
    DIF_PANEL_C3_TPO_VCOMD_2_0V,
    DIF_PANEL_C3_TPO_VCOMD_2_1V,
    DIF_PANEL_C3_TPO_VCOMD_2_2V,
    DIF_PANEL_C3_TPO_VCOMD_2_3V,
    DIF_PANEL_C3_TPO_VCOMD_2_4V,
    DIF_PANEL_C3_TPO_VCOMD_2_5V,
    DIF_PANEL_C3_TPO_VCOMD_2_6V,
    DIF_PANEL_C3_TPO_VCOMD_2_7V,
    DIF_PANEL_C3_TPO_VCOMD_2_8V,
    DIF_PANEL_C3_TPO_VCOMD_2_9V
} t_dif_panel_c3_tpo_vcond_power;

typedef enum
{
    DIF_PANEL_C3_TPO_VCS_3_0V               = 0x0,
    DIF_PANEL_C3_TPO_VCS_3_1V,
    DIF_PANEL_C3_TPO_VCS_3_2V,
    DIF_PANEL_C3_TPO_VCS_3_3V,
    DIF_PANEL_C3_TPO_VCS_3_4V,
    DIF_PANEL_C3_TPO_VCS_3_5V,
    DIF_PANEL_C3_TPO_VCS_3_6V,
    DIF_PANEL_C3_TPO_VCS_3_7V,
    DIF_PANEL_C3_TPO_VCS_3_8V,
    DIF_PANEL_C3_TPO_VCS_3_9V,
    DIF_PANEL_C3_TPO_VCS_4_0V,
    DIF_PANEL_C3_TPO_VCS_4_1V,
    DIF_PANEL_C3_TPO_VCS_4_2V,
    DIF_PANEL_C3_TPO_VCS_4_3V,
    DIF_PANEL_C3_TPO_VCS_4_4V,
    DIF_PANEL_C3_TPO_VCS_4_5V
} t_dif_panel_c3_tpo_vcs_power;

typedef enum
{
    DIF_PANEL_C3_TPO_VGM_3_7V               = 0x0,
    DIF_PANEL_C3_TPO_VGM_3_8V,
    DIF_PANEL_C3_TPO_VGM_3_9V,
    DIF_PANEL_C3_TPO_VGM_4_0V,
    DIF_PANEL_C3_TPO_VGM_4_1V,
    DIF_PANEL_C3_TPO_VGM_4_2V,
    DIF_PANEL_C3_TPO_VGM_4_3V,
    DIF_PANEL_C3_TPO_VGM_4_4V,
    DIF_PANEL_C3_TPO_VGM_4_5V,
    DIF_PANEL_C3_TPO_VGM_4_6V,
    DIF_PANEL_C3_TPO_VGM_4_7V,
    DIF_PANEL_C3_TPO_VGM_4_8V,
    DIF_PANEL_C3_TPO_VGM_4_9V,
    DIF_PANEL_C3_TPO_VGM_5_0V,
    DIF_PANEL_C3_TPO_VGM_5_1V,
    DIF_PANEL_C3_TPO_VGM_5_2V
} t_dif_panel_c3_tpo_vgm_power;

/* Display supply Control 2 */
typedef enum
{
    DIF_PANEL_C3_TPO_DUAL_MODE              = 0x0,
    DIF_PANEL_C3_TPO_SINGLE_MODE,
} t_dif_panel_c3_tpo_boost_clk_mode;

typedef enum
{
    DIF_PANEL_C3_TPO_XVDD_4_7V              = 0x0,
    DIF_PANEL_C3_TPO_XVDD_4_8V,
    DIF_PANEL_C3_TPO_XVDD_4_9V,
    DIF_PANEL_C3_TPO_XVDD_5_0V,
    DIF_PANEL_C3_TPO_XVDD_5_1V,
    DIF_PANEL_C3_TPO_XVDD_5_2V,
    DIF_PANEL_C3_TPO_XVDD_5_3V,
    DIF_PANEL_C3_TPO_XVDD_5_4V
} t_dif_panel_c3_tpo_xvdd_power;

/* Display supply Control 3 */
typedef enum
{
    DIF_PANEL_C3_TPO_X2_MODE                = 0x0,
    DIF_PANEL_C3_TPO_X3_MODE,
} t_dif_panel_c3_tpo_vol_boost_step;

/* Display supply Control 4 */
typedef enum
{
    DIF_PANEL_C3_TPO_REGULATE_HALF          = 0x0,
    DIF_PANEL_C3_TPO_REGULATE_ONE,
    DIF_PANEL_C3_TPO_REGULATE_TWO,
    DIF_PANEL_C3_TPO_REGULATE_FOUR
} t_dif_panel_c3_tpo_regulate_clk_div;

typedef enum
{
    DIF_PANEL_C3_TPO_HALF                   = 0x0,
    DIF_PANEL_C3_TPO_ONE,
    DIF_PANEL_C3_TPO_ONE_HALF,
    DIF_PANEL_C3_TPO_TWO
} t_dif_panel_c3_tpo_dcclk_mask;

/* Gray scale offset amounts */
typedef enum
{
    DIF_PANEL_C3_TPO_0_0V                   = 0x0,
    DIF_PANEL_C3_TPO_0_1V,
    DIF_PANEL_C3_TPO_0_2V,
    DIF_PANEL_C3_TPO_0_3V,
    DIF_PANEL_C3_TPO_0_4V,
    DIF_PANEL_C3_TPO_0_5V,
    DIF_PANEL_C3_TPO_0_6V,
    DIF_PANEL_C3_TPO_0_7V
} t_dif_panel_c3_tpo_gray_scale_offset;

/*----------------------------------------------------------------------------
*       STRUCTURES
*----------------------------------------------------------------------------*/

/* Entry Mode */
typedef struct
{
    t_dif_panel_c3_tpo_state    tri_status;
    t_dif_panel_c3_tpo_state    bgr_status;
    t_dif_panel_c3_tpo_state    if18_status;
    t_dif_panel_c3_tpo_state    hwm_status;
    t_dif_panel_c3_tpo_state    am_status;
    t_dif_panel_c3_tpo_state    id0_status;
    t_dif_panel_c3_tpo_state    id1_status;
    t_dif_panel_c3_tpo_state    dfm0_status;
    t_dif_panel_c3_tpo_state    dfm1_status;
} t_dif_panel_c3_tpo_entry_mode_config;

/* Horizontal valid width settings */

/*typedef struct
{

	t_uint8 horizontal_width;
	t_uint8 horizontal_bach_porch;
	
}t_dif_panel_c3_tpo_hor_set;*/
typedef struct
{
    t_uint8                         horizontal_width;   /*Horizontal valid width settings*/
    t_uint8                         horizontal_back_porch;  /*Horizontal valid width settings*/
    t_dif_panel_c3_tpo_disp_size    disp_size;              /*Driver Output Control Setting */
    t_dif_panel_c3_tpo_state        driver_shift;           /*Driver Output Control Setting */
} t_dif_panel_c3_tpo_resolution_set;

/* Display Mode  */
typedef struct
{
    t_dif_panel_c3_tpo_disp_color_mode  color_mode;
    t_dif_panel_c3_tpo_magn_factor      magn_factor;            /* magnification factor for
                                                                      number of refresh fields
                                                                      (Display Mode 3) */
    t_dif_panel_c3_tpo_xy_expansion     xy_expansion;           /* Display Mode 4 */
    t_dif_panel_c3_tpo_scroll_feature   screen1;                /* Display Mode 1 linked with VLE2 */
    t_dif_panel_c3_tpo_scroll_feature   screen2;                /* Display Mode 1 linked with VLE1 */

    t_dif_panel_c3_tpo_state            xy_expansion_state;     /* Display Mode 4 */
    t_dif_panel_c3_tpo_state            sip_stat;               /*  SIP =0 SuperImPosition 
                                                                           Disabled Display Mode 4 */

    t_dif_panel_c3_tpo_state            split_screen_status;    /* SPT=1 LCD is driven as two 
                                                                          screen */
    t_dif_panel_c3_tpo_state            nbw_status;             /* Display Mode 1 */
    t_dif_panel_c3_tpo_state            rev_status;             /* Display Mode 1 */
    t_dif_panel_c3_tpo_state            pt0_status;             /* Display Mode 1 */
    t_dif_panel_c3_tpo_state            pt1_status;             /* Display Mode 1 */
    t_dif_panel_c3_tpo_state            refresh_state;          /* RSE=1 Refreshed (Display Mode 3 )*/

    /* Display Mode 2 */
    t_uint8                             vertical_back_porch;    /* 2-14 lines*/
    t_uint8                             vertical_front_porch;   /* 2 -254 lines */

    /* Display Mode 3 */
    t_uint16                            refresh_fields;         /* 2 to 16 fields */
} t_dif_panel_c3_tpo_disp_mode_config;

/* External Display Mode  */
typedef struct
{
    t_dif_panel_c3_tpo_sync_signal          sync_signal;
    t_dif_panel_c3_tpo_interface_width      interface_width;
    t_dif_panel_c3_tpo_dotclk_divisor       dotclk_divisor;
    t_dif_panel_c3_tpo_ram_access_interface ram_access_interface;

    /* External display signal setting 2 */
    t_uint8                                 line_period;    /* 16 to 255 */

    /* External display signal setting 3 */
    t_dif_panel_c3_tpo_state                vsync_polarity;
    t_dif_panel_c3_tpo_state                hsync_polarity;
    t_dif_panel_c3_tpo_state                vld_polarity;
    t_dif_panel_c3_tpo_state                enable_polarity;
    t_dif_panel_c3_tpo_state                dotclk_polarity;
} t_dif_panel_c3_tpo_ext_disp_config;

/* FR period adjustment setting */
typedef struct
{
    t_dif_panel_c3_tpo_onchip_osc_divisor   onchip_osc_divisor;
    t_uint8                                 lines_per_1h;
} t_dif_panel_c3_tpo_fr_period_config;

/* LTPS */
typedef struct
{
    t_dif_panel_c3_tpo_clk_pos          ltps_asw_rise_pos;              /* LTPS Control Settings 1 */

    t_dif_panel_c3_tpo_clk_pos          ltps_oe_rise_pos;               /* LTPS Control Settings 2 */
    t_dif_panel_c3_tpo_clk_pos          ltps_oe_fall_pos;               /* LTPS Control Settings 2 */

    t_dif_panel_c3_tpo_clk_pos          ltps_ckv_rise_pos;              /* LTPS Control Settings 4 */
    t_dif_panel_c3_tpo_clk_pos          ltps_ckv_fall_pos;              /* LTPS Control Settings 4 */

    t_dif_panel_c3_tpo_clk_pos          ltps_ext_asw_rise_pos;          /* LTPS Control Settings 5 */

    t_dif_panel_c3_tpo_clk_pos          ltps_ext_oe_rise_pos;           /* LTPS Control Settings 6 */
    t_dif_panel_c3_tpo_clk_pos          ltps_ext_oe_fall_pos;           /* LTPS Control Settings 6 */

    t_dif_panel_c3_tpo_clk_pos          ltps_ext_ckv_rise_pos;          /* LTPS Control Settings 8  */
    t_dif_panel_c3_tpo_clk_pos          ltps_ext_ckv_fall_pos;          /* LTPS Control Settings 8  */

    t_dif_panel_c3_tpo_edge_hold_time   ltps_asw_fall_hold_time;        /* LTPS Control Settings 3 */
    t_dif_panel_c3_tpo_edge_hold_time   ltps_ext_asw_fall_hold_time;    /* LTPS Control Settings 7 */

    t_uint8                             ltps_asw_high_width;            /* LTPS Control Settings 1 */
    t_uint8                             ltps_ext_asw_high_width;        /* LTPS Control Settings 5 */
} t_dif_panel_c3_tpo_ltps_ctrl_set;

/* Display Control  */
typedef struct
{
    t_dif_panel_c3_tpo_asw_level        asw_level;
    t_dif_panel_c3_tpo_output_pin_state output_pin_state;
    t_dif_panel_c3_tpo_state            po_status;
    t_dif_panel_c3_tpo_state            cont_status;
    t_dif_panel_c3_tpo_state            pev_status;
    t_dif_panel_c3_tpo_state            dcev_status;    /*turns on/off DCCK,/DCCK and DCEV signals */
    t_dif_panel_c3_tpo_state            ud_scan;
    t_dif_panel_c3_tpo_state            con_status;
    t_dif_panel_c3_tpo_state            oev_status;
    t_dif_panel_c3_tpo_state            vcs_status;
    t_dif_panel_c3_tpo_state            fr_status;
    t_dif_panel_c3_tpo_state            fdon_status;
    t_dif_panel_c3_tpo_state            vgam_status;
    t_dif_panel_c3_tpo_state            dcg_status;
} t_dif_panel_c3_tpo_disp_config;

/* Power supply Control  */
typedef struct
{
    t_dif_panel_c3_tpo_vcond_power      vcond_output_voltage;
    t_dif_panel_c3_tpo_vcs_power        vcs_output_voltage;
    t_dif_panel_c3_tpo_vgm_power        vgm_output_voltage;
    t_dif_panel_c3_tpo_xvdd_power       xvdd_output_voltage;
    t_dif_panel_c3_tpo_regulate_clk_div avdd_regulate_ckt;
    t_dif_panel_c3_tpo_regulate_clk_div xvdd_regulate_ckt;
    t_dif_panel_c3_tpo_regulate_clk_div ext_regulate_ckt;
    t_dif_panel_c3_tpo_dcclk_mask       dcclk_msk_period;
    t_dif_panel_c3_tpo_boost_clk_mode   wsel_avdd;
    t_dif_panel_c3_tpo_boost_clk_mode   wsel_xvdd;
    t_dif_panel_c3_tpo_vol_boost_step   bav_avdd;
    t_dif_panel_c3_tpo_vol_boost_step   bxv_xvdd;
} t_dif_panel_c3_tpo_power_config;

/* EXT polarity control */
typedef struct
{
    t_dif_panel_c3_tpo_state    extc1_status;
    t_dif_panel_c3_tpo_state    extc2_status;
} t_dif_panel_c3_tpo_ext_polarity_ctrl;

/* Address settings */
typedef struct
{
    /* RAM address settings */
    t_uint16    x_addr;
    t_uint16    y_addr;

    /* Horizontal vertical RAM address location */
    t_uint16    hor_start_addr;
    t_uint16    hor_end_addr;
    t_uint16    ver_start_addr;
    t_uint16    ver_end_addr;

    /* First Second screen drive position */
    t_uint16    screen1_start_addr;
    t_uint16    screen1_end_addr;
    t_uint16    screen2_start_addr;
    t_uint16    screen2_end_addr;

    /* OSD first/second screen start addresses */
    t_uint16    osd_screen1_start_addr;
    t_uint16    osd_screen2_start_addr;
} t_dif_panel_c3_tpo_addr_config;

/* Gray scale offset amounts */
typedef struct
{
    t_dif_panel_c3_tpo_gray_scale_offset    bofs_amount;
    t_dif_panel_c3_tpo_gray_scale_offset    bup_amount;
    t_dif_panel_c3_tpo_state                blon_status;
} t_dif_panel_c3_tpo_gray_scale_amt_config;

/* vertical scroll control */
typedef struct
{
    t_uint16    screen1_lines;
    t_uint16    screen2_lines;
} t_dif_panel_c3_tpo_vert_scroll;

/* Gray Scale Settings */
typedef struct
{
    t_uint8 pk0_config;
    t_uint8 pk1_config;
    t_uint8 pk2_config;
    t_uint8 pk3_config;
    t_uint8 pk4_config;
    t_uint8 pk5_config;
    t_uint8 pr0_config;
    t_uint8 pr1_config;
    t_uint8 vr0_config;
    t_uint8 vr1_config;
} t_dif_panel_c3_tpo_gray_scale;

/*  OSD Settings */
typedef struct
{
    t_dif_panel_c3_tpo_state    feature;
    t_dif_panel_c3_tpo_state    addr_mode;
} t_dif_panel_c3_tpo_osd_set;

/*  Auto Sequence Control Settings */
typedef struct
{
    t_dif_panel_c3_tpo_state    ctrl;
} t_dif_panel_c3_tpo_autoseq_set;

/*---------------------------------------------------------------------------
 *  Functions Prototype                                                   
 *---------------------------------------------------------------------------*/
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_ConfigEntryMode(t_dif_panel_c3_tpo_entry_mode_config);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_SetResolution(t_dif_panel_c3_tpo_resolution_set resolution_set);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_ConfigDisplayMode(t_dif_panel_c3_tpo_disp_mode_config);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_ConfigExtDisplayMode(t_dif_panel_c3_tpo_ext_disp_config);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_SetFR_Period(t_dif_panel_c3_tpo_fr_period_config);

PUBLIC t_dif_error  DIF_PANEL_C3_TPO_LTPS_Config(t_dif_panel_c3_tpo_ltps_ctrl_set);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_Display_Config(t_dif_panel_c3_tpo_disp_config);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_Power_Config(t_dif_panel_c3_tpo_power_config);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_Power_OffLine_Setting(t_dif_panel_c3_tpo_power_offline_set);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_ExtPolarityConfig(t_dif_panel_c3_tpo_ext_polarity_ctrl);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_AddrConfig(t_dif_panel_c3_tpo_addr_config);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_GrayScaleAmtConfig(t_dif_panel_c3_tpo_gray_scale_amt_config);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_CtrlVertScroll(t_dif_panel_c3_tpo_vert_scroll);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_AutoSeqCtrl(t_dif_panel_c3_tpo_autoseq_set);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_OscillatorSetting(t_dif_panel_c3_tpo_state);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_LCD_DrvSetting(t_dif_panel_c3_tpo_lcd_drv_signal);
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_AmpCapSetting(t_dif_panel_c3_tpo_amp_cap_set);

/* Mode Setting */
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_StandBy(t_dif_panel_c3_tpo_state, t_dif_panel_c3_tpo_state);

/* RAM data write */
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_RAM_DataWrite(t_uint16);

/* Graphic Operations */
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_DisplayRAM_Mask(t_uint8, t_uint8, t_uint8);

/* Gray scale settings */
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_GrayScaleConfig(t_dif_panel_c3_tpo_gray_scale);

/* OSD function On/Off */
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_ConfigOSD(t_dif_panel_c3_tpo_osd_set);

/* NOP function*/
PUBLIC t_dif_error  DIF_PANEL_C3_TPO_Nop(void);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _DIF_PANEL_C3_TPO_H_ */

