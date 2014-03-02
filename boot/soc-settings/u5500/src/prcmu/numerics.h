#ifndef INCLUSION_GUARD_NUMERICS_H
#define INCLUSION_GUARD_NUMERICS_H

/*----------------------------------------------------------------------------*
 *      STEricsson, Incorporated. All rights reserved.                        *
 *                                                                            *
 *        STEricsson, INCORPORATED PROPRIETARY INFORMATION                    *
 * This software is supplied under the terms of a license agreement or non-   *
 * disclosure agreement with STEricsson and may not be copied or disclosed    *
 * except in accordance with the terms of that agreement.                     *
 *----------------------------------------------------------------------------*
 * System      : STxP70 Processor                                             *
 * File Name   : numerics.h                                                   *
 * Purpose     : typedef definitio                                            *
 * Author(s)   : Per-Inge TALLBERG                                            *
 *               Ulf MORLAND                                                  *
 * History     : 2010/10/26 - First draft implementation.                     *
 *----------------------------------------------------------------------------*/

#include "t_basicdefinitions.h"

// STM register
enum debug {
  /* Debug Val for action ok */
//empty field = 0x00
  INIT                      = 0x0,
  PLLARMLOCKP_OK            = 0x01,
  PLLDDRLOCKP_OK            = 0x02,
  PLLSOC0LOCKP_OK            = 0x03,
  PLLSOC1LOCKP_OK            = 0x04,
  ARMWFI_OK                    = 0x05,
  SYSCLK_OK                    = 0x06,
  PLL32KLOCKP_OK            = 0x07,
  DDRDEEPSLEEP_OK            = 0x08,
/* Debug Val for IT13 process */
  ANCH_IT13                    = 0x09,
  SIDETONEH_IT13            = 0x0A,
  DEFAULT_IT13                = 0x0B,
/* Debug Val for timing_loop_fct */
  TMLFCT_IN                    = 0x0C,
  TMLFCT_TIMER                = 0x0D,
  TMLFCT_OUT                = 0x0E,
/* Debug Val for IT10 process */
  PWRSTTRH_IT10                = 0x0F,
  WKUPCFGH_IT10                = 0x10,
  RDWKUPACKH_IT10            = 0x11,
  DEFAULT_IT10                = 0x12,
/* Debug Val for sxammdsp_fct */
  FIFO4500IT_SVA_ACTIVE        = 0x13,
  FIFO4500IT_SIA_ACTIVE        = 0x14,
/* actived signal detection for _it5_4500_fifo */
  GOEXE_IT10                = 0x15,
  NOGOEXE_IT10                = 0x16,
  GODS_IT10                    = 0x17,
  NOGODS_IT10                = 0x18,
  GOSL_IT10                    = 0x19,
  NOGOSL_IT10                = 0x1A,
  GOID_IT10                    = 0x1B,
  NOGOID_IT10                = 0x1C,
  NOPWRTR_IT10                = 0x1D,
/* Debug Val for It3 subprogram */
  IT3_XP70_APE_WAKEUP_IN    = 0x1E,
  IT3_XP70_APE_WAKEUP_OUT    = 0x1F,
/* Debug Val for It4 subprogram */
  IT4_MODEM_DDR_IN            = 0x20,
  IT4_MODEM_DDR_OUT            = 0x21,
/* Debug Val for It5 subprogram */
  IT5_4500_FIFO_IN            = 0x22,
  IT5_4500_FIFO_OUT            = 0x23,
/* Debug Val for It6 subprogram */
  IT6_A9WD_SFTRST_IN        = 0x24,
  IT6_A9WD_SFTRST_OUT        = 0x25,
/* Debug Val for It7 subprogram */
  IT7_GPIO_IN                = 0x26,
  IT7_GPIO_OUT                = 0x27,
/* Debug Val for It10 subprogram */
  IT10_PWRSTATE_MGT_IN        = 0x28,
  IT10_PWRSTATE_MGT_OUT        = 0x29,
/* Debug Val for It11 subprogram */
  IT11_DVFS_IN                = 0x2A,
  IT11_DVFS_OUT                = 0x2B,
/* Debug Val for It12 subprogram */
  IT12_DPS_IN                = 0x2C,
  IT12_DPS_OUT                = 0x2D,
/* Debug Val for It15 subprogram */
  IT14_DBG_IN               = 0x2E,
  IT14_DBG_OUT              = 0x2F,
/* /Debug Val for It15 subprogram */
  IT15_ARM_IT_MGT_IN        = 0x30,
  IT15_ARM_IT_MGT_OUT        = 0x31,
/* Debug Val for It15 subprogram */

/* Debug Val for It15 subprogram */

/* Debug Val for It8 subprogram */
  IT8_DDR_CONFIG_IN            = 0x36,
  IT8_DDR_CONFIG_OUT        = 0x37,
/* Debug Val for It16 subprogram */
  IT16_DDR_SERVICE_IN        = 0x38,
  IT16_DDR_SERVICE_OUT        = 0x39,
/* Debug Val for It17 subprogram */
  IT17_GENI2C_IN            = 0x3A,
  IT17_GENI2C_OUT            = 0x3B,
/* Debug Val for It13 subprogram */

/* Debug Val for It25 subprogram */
  IT25_XP70_MODEM_POWER_IN    = 0x3E,
  IT25_XP70_MODEM_POWER_OUT    = 0x3F,
/* Debug Val for polling_fct */
  POLFCT_IN                    = 0x40,
  POLFCT_TIMER                = 0x41,
  POLFCT_ERR                 = 0x42,
/* Debug Val for pollingon romcode */
  XP70RST_POLDRFCT_TIMER    = 0x43,
  XP70RST_POLDRFCT_ERR        = 0x44,
/* Debug Val for polling_a9wfi_fct */
  POLFCT_A9WFI_IN             = 0x45,
  POLFCT_A9WFI_TIMER        = 0x46,
  POLFCT_A9WFI_ERR            = 0x47,
/* Debug Val for IT25 process */
  I2C_RD_IT25                = 0x48,
  MODSYSCLKREQ_IT25            = 0x49,
  MODSYSCLKREQ_RIS_IT25      = 0x4A,
  MODSYSCLKREQ_FAL_IT25        = 0x4B,
  MODPWRENA_IT25            = 0x4C,
  MODPWRENA_RISING_IT25        = 0x4D,
  MODPWRENA_FALLING_IT25    = 0x4E,
  MODSTATE_IT25                = 0x4F,
/* Debug Val for hw_evt_fct */
  HW_EVT_IN                    = 0x56,
  INVRDP_PONG                = 0x57,
  INVRDP_PING                = 0x58,
  WKUP_ST_RDWR                = 0x59,
  WKUP_ST_RD                = 0x5A,
  WKUP_ST_RDWR_ERR            = 0x5B,
  WKUPIRQN_INIT                = 0x5C,
  WKUP_ST_WR                 = 0x5D,
/* Debug Val for i2c_acess_fct */
  I2C_IN                    = 0x5E,
  I2C_POLFCT_TIMER            = 0x5F,
  I2C_POLFCT_ERR             = 0x60,
  I2C_POLFCT_ABORT             = 0x61,
  I2C_OUT                    = 0x62,
/* Debug Val for Geni2c_acess_fct */
  GENI2C_IN                    = 0x63,
  GENI2C_POLFCT_TIMER         = 0x64,
  GENI2C_POLFCT_ERR            = 0x65,
  GENI2C_ABORT                 = 0x66,
  GENI2C_OUT                = 0x67,
/* Debug Val for IT16 process */
  DDRINITH_IT16                = 0x68,
  MEMSTH_IT16                = 0x69,
  AUTOREFH_IT16                = 0x6A,
  AUTOPWRH_IT16                = 0x6B,
  PSREFCS0H_IT16            = 0x6C,
  PSREFCS1H_IT16            = 0x6D,
  CSPWRMGTH_IT16            = 0x6E,
  SYSCLK_IT16                = 0x6F,
  SYSCLKON_IT16                = 0x70,
  MODSYSCLKOFF0_IT16        = 0x71,
  SYSCLKOFF_IT16            = 0x72,
  MODSYSCLKOFF1_IT16        = 0x73,
  DEFAULT_IT16                = 0x74,
/* Debug Val for romcode polling on execute_to_deepsleep */
  ROMCODE_READY_TIMER        = 0x75,
  ROMCODE_READY_ERR            = 0x76,
  ROMCODE_READY_TO_DS        = 0x77,
  ROMCODE_NOREADY_TO_DS        = 0x78,
/* Debug Val for _init_fct */
  INIT_IN                    = 0x79,
  INIT_I2C_SECURE            = 0x7A,
  INIT_I2C_NOSECURE            = 0x7B,
/* Debug Val for sysclk_mgt_fct */
  SYSCLK_MGT_IN                = 0x7C,
  SYSCLKVALID_I2C            = 0x7D,
/* Debug Val for stn4500_init_sequency_fct */
  INITSEQ_IN                = 0x7E,
  INITSEQ_OUT                = 0x7F,
/* Debug Val for wake_up_fct */
  WUFCT_IN                    = 0x80,
  WUFCT_DS                    = 0x81,
  WUFCT_SL                    = 0x82,
  WUFCT_ID                    = 0x83,
  WUFCT_DEFAULT                = 0x84,
/* Debug Val for IT3 process */
  CAWAKE_FALLING            = 0x85,
  WAKEUP_IT3                = 0x86,
  APEINT4500_1                = 0x87,
  APEINT4500_2                = 0x88,
  APEINT4500_3                = 0x89,
/* Debug Val for hwacc_fct */
//  ESRAM12_IN                = 0x8A,
//  ESRAM12_VOK               = 0x8B,

  HWACFCT_IN_SVAPIPE        = 0x8D,

  HWACFCT_IN_SIAPIPE        = 0x8F,
  HWACFCT_IN_SGA             = 0x90,
  HWACFCT_HWON                = 0x91,
  HWACFCT_HWOFF                = 0x92,
  HWACFCT_HWOFFRAMRET        = 0x93,
  HWACFCT_IN_APERAMS         = 0x94,
  HWACFCT_IN_B2R2            = 0x95,
  HWACFCT_IN_MCDE            = 0x96,
  HWACFCT_HWONCLKOFF        = 0x97,
  HWACC_VOK                    = 0x98,
/* Debug Val for IT4 process */
  MOD_MEM_REQ_FALLING1        = 0x99,
  MOD_MEM_REQ_FALLING0        = 0x9A,
  HOSTACCESS_IN                = 0x9B,
  MOD_MEM_REQ_IN            = 0x9C,
  MOD_MEM_REQ_RISING        = 0x9D,
  MOD_MEM_REQ_FALLING        = 0x9E,
/* Debug Val for IT23 process */
  TIMER3_0_IT23                = 0x9F,
  TIMER3_1_IT23                = 0xA0,
/* Debug Val for IT24 process */
  APE_MEM_ACK_1_IT24        = 0xA1,
/* Debug Val for exe_to_DS/SL/ID  */
/* when wake up evt has been happened before */
  RDYTODS_RETURNTOEXE        = 0xA2,
  RDYTOSL_RETURNTOEXE        = 0xA3,
  RDYTOID_RETURNTOEXE        = 0xA4,
/* Debug Val for a9_dvfs_fct */
  DVFSFCT_A9_IN                = 0xA5,
  DVFSFCT_A9_100OPP            = 0xA6,
  DVFSFCT_A9_50OPP            = 0xA7,
  DVFSFCT_A9_EXT            = 0xA8,
  DVFSFCT_A9_DEFAULT        = 0xA9,
/* Debug Val for clkarm_fct */
  CLK_A9_BOOT                = 0xAA,
  CLK_A9_OPT1                = 0xAB,
  CLK_A9_OPT2                = 0xAC,
  CLK_A9_EXT                = 0xAD,
  CLK_A9_DEFAULT            = 0xAE,
/* Debug Val for sel_event_mask_fct */
  SEL_EVENT                    = 0xAF,
  SEL_EVENT_ARM_IT_MGT        = 0xB0,
  SEL_EVENT_NOARM_IT_MGT    = 0xB1,
/* Debug Val for clkscalable_fct */
  CLK_APES_BOOT                = 0xB2,
  CLK_APES_OPT1                = 0xB3,
  CLK_APES_DEFAULT            = 0xB4,
/* Debug Val for clkfixed_fct */
  CLK_APEF_BOOT                = 0xB5,
  CLK_APEF_OPT1                = 0xB6,
  CLK_APEF_DEFAULT            = 0xB7,
/* Debug Val for arm_clk_mgt_execute_to_sleepmode_fct */
  CLK_A9_MGT                = 0xB8,
  CLK_A9_MGT_NODBG            = 0xB9,
/* Debug Val for ape_clk_mgt_execute_to_sleepmode_fct */
  CLK_APE_MGT                = 0xBA,
  CLK_APE_MGT_NODBG            = 0xBB,
/* Debug Val for power_domain_fct */
  PWR_DOMAIN                = 0xBC,
  PWR_DOMAIN_NODBG            = 0xBD,
/* Debug Val for arm_clk_mgt_sleepmode_to_execute_fct */
  CLK_A9_MGT_EXE            = 0xBE,
  CLK_A9_MGT_PLLARM_EN        = 0xBF,
/* Debug Val for IT17 process */
  I2C_RD_OK_IT17            = 0xC0,
/* Debug Val for DDR SUBSYSTEM configuration polling */
  DDR_CFG_TIMER             = 0xC1,
/* Debug Val for ape_clk_mgt_sleepmode_to_execute_fct */
  CLK_APE_MGT_EXE            = 0xC2,
  CLK_APE_MGT_PLLSOC0_EN    = 0xC3,
  CLK_APE_MGT_PLLSOC1_EN    = 0xC4,
/* Debug Val for a9_reset_fct */
  A9_RESET                    = 0xC5,
  A9_RESET_NODBG            = 0xC6,
  A9_RESET_DBG                = 0xC7,
/* Debug Val for ddr_config_fct */
  DDR_CFG                    = 0xC8,
  DDR_CFG0                    = 0xC9,
  DDR_CFG0_A                = 0xCA,
  DDR_CFG0_B                = 0xCB,
  DDR_CFG1                    = 0xCC,
  DDR_CFG1_FIRST            = 0xCD,
/* Debug Val for pll32k_enable_fct */
  PLL32K_ENABLE                = 0xCE,
  PLL32K_STATE                = 0xCF,
/* Debug Val for It23 subprogram */
  IT23_TIMER3_IN            = 0xD0,
  IT23_TIMER3_OUT            = 0xD1,
/* Debug Val for It24 subprogram */
  IT24_APE_MEM_ACK_IN        = 0xD2,
  IT24_APE_MEM_ACK_OUT        = 0xD3,
/* Debug Val for pll32k_start_fct */
  PLL32K_START                = 0xD4,
  PLL32K_STATE1                = 0xD5,
  PLLDDR_START                = 0xD6,
/* Debug Val for pll32k_disable_fct */
  PLL32K_DISABLE            = 0xD7,
/* Debug Val for pll32k_stop_fct */
  PLL32K_STOP                = 0xD8,
/* Debug Val for ddr_state_sleep_idle_fct */
  DDR_STATE                    = 0xD9,
  DDR_STATE_OFFLOWLAT        = 0xDA,
  DDR_STATE_OFFHIGHLAT        = 0xDB,
  DDR_STATE_ON                = 0xDC,
  DDR_STATE_DEFAULT            = 0xDD,
/* Debug Val for self reset */
  SELF_RESET                = 0xDE,
/* Debug Val for jump */
  JUMP                        = 0xDF,
/* Debug Val for ape_dvfs_fct */
  DVFSFCT_APE_IN            = 0xE0,
  DVFSFCT_APE_100OPP        = 0xE1,
  DVFSFCT_APE_50OPP            = 0xE2,
  DVFSFCT_APE_DEFAULT        = 0xE3,
/* Debug Val for hwacc_fct */
  HWACFCT_AUTOPWRH            = 0xE4,
  HWACFCTH_DEFAULT            = 0xE5,
/* Debug val for SXAMMDSP/SXAPIPE into IT18/IT19 process */
  ESRAM12_IN                = 0xE6,
  ESRAM12_VOK               = 0xE7,
  ESRAM12_ON                = 0xE8,
  ESRAM12_RET               = 0xE9,
  ESRAM12_OFF               = 0xEA,

/* wakeup event configuration for MMDSP */
  HSEM_SEL                     = 0xEE,
  FIFO4500IT_SEL            = 0xEF,
  HSEM_FIFO4500IT_SEL        = 0xF0,
/* actived signal detection for _it6_a9wd_sftrst */
  SOFT_RESET_ACTIVE         = 0xF2,

/* Debug val for context SAVING/RESTORING process */
  ULPCLK_SWITCH             = 0xF5,
  MODECLK_2                 = 0xF6,
  DEBUG_F7                  = 0xF7,
  DEBUG_F8                  = 0xF8,
  DEBUG_F9                  = 0xF9,
/* debug val when modem run standalone */
  GOEXE_MODEMSTANDALONE     = 0xFA,
/* ESRAM0 cfg */
  ESRAM0_VOK                = 0xFB,
  ESRAM0_IN                 = 0xFC,
  ESRAM0_ON                 = 0xFD,
  ESRAM0_OFF                = 0xFE,
  ESRAM0_RET                = 0xFF
};


// general purpose On/Off states
enum on_off_st {
    OOS_OFF,
    OOS_ON
};


// general purpose On/Off/Retention states
enum on_off_retention_st {
    OORS_OFF,
    OORS_RETENTION,
    OORS_ON
};

// general purpose Off/Retention states
enum off_retention_st {
    ORS_OFF,
    ORS_RETENTION
};


// Ap power states
enum ap_pwr_st {
    APS_IGNORE,
    APS_BOOT,
    APS_EXECUTE,
    APS_DEEP_SLEEP,
    APS_SLEEP,
    APS_IDLE,
    APS_RESET
};

enum dbb_event {
    //event from hold_evt, line registers
    RTC             = 0x00000001,
    RTT0            = 0x00000002,
    RTT1            = 0x00000004,
    CD_IRQ_N        = 0x00000008,
    SRP_TIMER       = 0x00000010,
    APE_RQ          = 0x00000020,  // not needed for A-CPU
    USB_WKUP        = 0x00000040,
    IRQ_N           = 0x00000080,
    MODEM_REQ_ACK   = 0x00000800, // Modem request acknowledge
    MOD_WDOG_BARK_N = 0x00100000,
    GPIO_0          = 0x00800000,
    GPIO_1          = 0x01000000,
    GPIO_2          = 0x02000000,
    GPIO_3          = 0x04000000,
    GPIO_4          = 0x08000000,
    GPIO_5          = 0x10000000,
    GPIO_6          = 0x20000000,
    GPIO_7          = 0x40000000,
    //event not present in hold_evt, line registers
    LOW_PWR_AUDIO   = 0x00000100,
    TEMP_SENSOR_LOW = 0x00000200,
    ARM_IT          = 0x00000400, // not really needed as wakeup cause since, since it is a mirror of the GIC
    TEMP_SENSOR_HIGH= 0x00001000,

    MODEM_IN_RESET  = 0x00200000,
    MODEM_REL_ACK   = 0x80000000, // Modem release acknowledge

    DBB_PHY_EVENT   = (RTC | RTT0 | RTT1 | CD_IRQ_N | SRP_TIMER | USB_WKUP | IRQ_N | MODEM_REQ_ACK | MOD_WDOG_BARK_N |
                      GPIO_0 | GPIO_1 | GPIO_2 | GPIO_3 | GPIO_4 | GPIO_5 | GPIO_6 | GPIO_7),
    DBB_LOG_EVENT   = (LOW_PWR_AUDIO | TEMP_SENSOR_LOW | TEMP_SENSOR_HIGH | ARM_IT |
                       MODEM_REL_ACK | MODEM_IN_RESET  )
};


// List of the ABB interrupt source register.
enum abb_event_reg {
    ABB_EVENT_REG_0  = 0x00000001,
    ABB_EVENT_REG_1  = 0x00000002,
    ABB_EVENT_REG_2  = 0x00000004,
    ABB_EVENT_REG_3  = 0x00000008,
    ABB_EVENT_REG_4  = 0x00000010,
    ABB_EVENT_REG_5  = 0x00000020,
    ABB_EVENT_REG_6  = 0x00000040,
    ABB_EVENT_REG_7  = 0x00000080,
    ABB_EVENT_REG_8  = 0x00000100,
    ABB_EVENT_REG_9  = 0x00000200,
    ABB_EVENT_REG_10 = 0x00000400,
    ABB_EVENT_REG_11 = 0x00000800,
    ABB_EVENT_REG_12 = 0x00001000,
    ABB_EVENT_REG_13 = 0x00002000,
    ABB_EVENT_REG_14 = 0x00004000,
    ABB_EVENT_REG_15 = 0x00008000,
    ABB_EVENT_REG_16 = 0x00010000,
    ABB_EVENT_REG_17 = 0x00020000,
    ABB_EVENT_REG_18 = 0x00040000,
    ABB_EVENT_REG_19 = 0x00080000,
    ABB_EVENT_REG_20 = 0x00100000,
    ABB_EVENT_REG_21 = 0x00200000,
    ABB_EVENT_REG_22 = 0x00400000,
    ABB_EVENT_REG_23 = 0x00800000,
    ABB_EVENT_REG_24 = 0x01000000,
    ABB_EVENT_REG_25 = 0x02000000
};


//---------------------------- Header for ReqMb0
enum req_mb_0_h {
    RMB0H_PWR_STATE_TRANS        = 1,
    RMB0H_WAKE_UP_CFG,
    RMB0H_RD_WAKE_UP_ACK
};

//---------------------------- Header for AckMb0
enum ack_mb_0_h {
    AMB0H_WAKE_UP
};



// power state transitions
//
enum ap_pwr_st_tr {
    APPST_NO_TRANSITION,
    APPST_BOOT_TO_EXECUTE,
    APPST_EXECUTE_TO_SLEEP,
    APPST_EXECUTE_TO_DEEP_SLEEP,
    APPST_EXECUTE_TO_IDLE,
    APPST_EXECUTE_TO_SHUTDOWN
};


// ddr power states
enum ddr_pwr_st {
    DDRPS_UNCHANGED,
    DDRPS_ON,
    DDRPS_OFF_LOW_LATENCY,
    DDRPS_OFF_HIGH_LATENCY
};

// implementation issue: the values are chosen the way that
// we can change from ping to pong (resp. pong to ping) by
// simply using the 'not' operator in C, e.g. toggling operation:
// PingPong p = ping; p = ~p;
enum ping_pong {
    PP_PING,
    PP_PONG
};


//---------------------------- Header for ReqMb1 and AckMb1
enum req_mb_1_h {
    MB1H_ARM_OPP                     = 1,
    MB1H_APE_OPP,
    MB1H_ARM_APE_OPP,
};



// ARM OPP STATES
enum arm_opp {
    AO_ARM_INIT,
    AO_ARM_100_OPP,
    AO_ARM_50_OPP,
    AO_ARM_EXT_OPP
};


// APE OPP STATES
enum ape_opp {
    AO_APE_INIT,
    AO_APE_MAX_OPP,
    AO_APE_133_OPP,
    AO_APE_100_OPP,
};


enum return_code {
    RC_SUCCESS,
    RC_FAIL
};


enum pwr_client {
    SGA,
    HVA,
    SIA,
    DISP,
    DSS = DISP,
    ESRAM12 = 6     // 6 to keep backwards compatibility
};

enum clk_client {
    MSP1,
    CD,
    IRDA,
    TVCLK
};

enum pll {
    PLLSOC0,
    PLLSOC1,
    PLLDDR
};


//---------------------------- Header for ReqMb2 AND AckMb2
enum mb_2_h {
    MB2H_PWR_REQ                = 1,
    MB2H_CLK_REQ,
    MB2H_PLL_REQ,
    MB2H_PWR_STATUS,
    MB2H_CLK_STATUS,
    MB2H_PLL_STATUS
};


//---------------------------- Header for ReqMb3 and AckMb3
enum mb_3_h {
    MB3H_REF_CLK                = 1,
};


//---------------------------- Header for ReqMb4
enum req_mb_4_h {
    RMB4H_DDR_INIT              = 1,
    RMB4H_CFG_AUTO_SR_WATERMARK,
    RMB4H_CFG_AUTO_LP_WATERMARK,
    RMB4H_CFG_PASR_CS1,
    RMB4H_CFG_DPD_CS1,
    RMB4H_CFG_QOS_PORT,
    RMB4H_CFG_HOTDOG,
    RMB4H_CFG_HOTMON,
    RMB4H_ABB_INIT,
    RMB4H_CFG_HOTPERIOD,
    RMB4H_DDR_PRE_INIT,
    RMB4H_AVS_ACTIVATE,
    RMB4H_MODEM_RESET_REQUEST,
    RMB4H_WDOG_ENABLE_WITH_PREBARK,
    RMB4H_WDOG_ENABLE_WITHOUT_PREBARK,
    RMB4H_WDOG_DISABLE
};

enum ddr_dpd_st {
  DDS_DDR_DPD,
  DDS_DDR_ACTIVE,
};


//---------------------------- Header for AckMb4
enum ack_mb_4_h {
    AMB4H_DDRINIT  = 1,
    AMB4H_PASR_CS1,
    AMB4H_DPD_CS1,
    AMB4H_QOS_PORT,
    AMB4H_CFG_HOTDOG,
    AMB4H_CFG_HOTMON,
    AMB4H_ABB_INIT,
    AMB4H_CFG_HOTPERIOD,
    AMB4H_DDR_PRE_INIT,
    AMB4H_AVS_ACTIVATE,
    AMB4H_MODEM_RESET_REQUEST,
    AMB4H_WDOG_ENABLE_WITH_PREBARK,
    AMB4H_WDOG_ENABLE_WITHOUT_PREBARK,
    AMB4H_WDOG_DISABLE
};


//---------------------------- Header for ReqMb5 and AckMb5
enum req_mb_5_h {
    RMB5H_I2C_WRITE             = 1,
    RMB5H_I2C_READ
};

//---------------------------- Header for ReqMb6
enum req_mb_6_h {
    RMB6H_LPA_GET_BUF           = 1,

    RMB6H_LPA_DO_START,
    RMB6H_LPA_DO_STOP,
    RMB6H_LPA_DO_ABORT,
};


//---------------------------- Header for AckMb6
enum ack_mb_6_h {
    AMB6H_LPA_GET_BUF           = 1,
    AMB6H_LPA_DO_CMD
};



//
enum lpa_status {
    IDLE            = 0x0001,   // Nothing more to do
    RUNNING         = 0x0002,   // After DO_START cmd, we are running
    STOPPING        = 0x0004,   // After DO_STOP cmd, we are stopping
    ABORTING        = 0x0008,   // After DO_ABORT cmd, we are aborting
    NEED_MORE_DATA  = 0x0100,   // When watermark is exceeded, we need more data
    ERROR           = 0x1000,   // Some sort of error
    UNDERRUN        = 0x2000,   // Underrun condition
};



enum clk_out_source {
    COS_RAW_REF_CLK             = 0x0001,
    COS_RTC_CLK                 = 0x0002,
    COS_RAW_ULP_CLK             = 0x0004,
    COS_STATIC_0                = 0x0018,
    COS_REF_CLK_DIVIDED         = 0x0028,
    COS_ULP_CLK_DIVIDED         = 0x0048,
    COS_ARM_CLK_DIVIDED         = 0x0088,
    COS_SYS_ACC_0_CLK_DIVIDED   = 0x0108,
    COS_SOC_0_PLL_CLK_DIVIDED   = 0x0208,
    COS_SOC_1_PLL_CLK_DIVIDED   = 0x0408,
    COS_DDR_PLL_CLK_DIVIDED     = 0x0808,
    COS_TV_CLK_DIVIDED          = 0x1008,
    COS_IRDA_CLK_DIVIDED        = 0x2008
};

enum clk_out_oe {
    COO_OUTPUT_DISABLE,
    COO_OUTPUT_ENABLE,
};

enum clk_out_div {
    COD_DIV_2   = 0,
    COD_DIV_4   = 1,
    COD_DIV_8   = 2,
    COD_DIV_16  = 3,
    COD_DIV_32  = 4,
    COD_DIV_64  = 5
};


//---------------------------- Header for ReqMb7 and AckMb7
enum req_mb_7_h {
    CLK_OUT_CONFIG_SW_EN             = 1,
    CLK_OUT_CONFIG_REQ_EN,
    CLK_OUT_CONFIG_SW_DIS,
    CLK_OUT_CONFIG_REQ_DIS,
    CLK_OUT_CONFIG_CLKOUT_REQ_ACTIVE_HIGH,
    CLK_OUT_CONFIG_CLKOUT_REQ_ACTIVE_LOW
};





//---------------------------- status/Error
enum ap_pwr_st_tr_status /* ApPwrStTrStatus */ {
  // success messages
  BootToExecuteOk        = 0xFF,
  DeepSleepOk            = 0xFE,
  SleepOk                = 0xFD,
  IdleOk                 = 0xFC,
  SoftResetOk            = 0xFB,
  SoftResetGo            = 0xFA,
  BootToExecute          = 0xF9,
  ExecuteToDeepSleep     = 0xF8,
  DeepSleepToExecute     = 0xF7,
  DeepSleepToExecuteOk   = 0xF6,
  ExecuteToSleep         = 0xF5,
  SleepToExecute         = 0xF4,
  SleepToExecuteOk       = 0xF3,
  ExecuteToIdle          = 0xF2,
  IdleToExecute          = 0xF1,
  IdleToExecuteOk        = 0xF0,
  RdyToDs_ReturnToExe    = 0xEF,
  NoRdyToDs_ReturnToExe  = 0xEE,
  ExeToSleep_ReturnToExe = 0xED,
  ExeToIdle_ReturnToExe  = 0xEC,
  WkUpCfgOk                 = 0xEA,
  InitStatus             = 0x00,

  // error messages
  InitError                     = 0xEB,
  PllArmLockp_Er                = 0x01,
  PllDdrLockp_Er                = 0x02,
  PllSoc0Lockp_Er               = 0x03,
  PllSoc1Lockp_Er               = 0x04,
  ArmWfi_Er                     = 0x05,
  SysClkOk_Er                   = 0x06,
  I2c_NAck_Data_Er              = 0x07, /* PRCM_HWI2C_SR(4:2)="001", PRCM_HWI2C_SR(1:0)="11" */
  Boot_Er                       = 0x08,
  I2c_Status_Always_1           = 0x0A,
  I2c_NAck_Reg_Addr_Er          = 0x0B, /* PRCM_HWI2C_SR(4:2)="010", PRCM_HWI2C_SR(1:0)="11" */
  I2c_Error                     = 0x0C, /* Error */
  I2c_NAck_Data0123_Er          = 0x1B, /* PRCM_HWI2C_SR(4:2)="110", PRCM_HWI2C_SR(1:0)="11" */
  I2c_NAck_Addr_Er              = 0x1F, /* PRCM_HWI2C_SR(4:2)="111", PRCM_HWI2C_SR(1:0)="11" */
  CurApPwrStIsNotBoot           = 0x20,
  CurApPwrStIsNotExecute        = 0x21,
  CurApPwrStIsNotSleepMode      = 0x22,
  CurApPwrStIsNotCorrectForIT10 = 0x23,
  Fifo4500WuIsNotWupEvent       = 0x24,
  Pll32kLockp_Er                = 0x29,
  DDRDeepSleepOk_Er                = 0x2A,
  PendingReq_Er                 = 0x2B,
  RomcodeReady_Er               = 0x50,
  WupBeforeDs                   = 0x51,
  DdrConfig_Er                  = 0x52,
  WupBeforeSleep                = 0x53,
  WupBeforeIdle                 = 0x54,
  WkUpR_st_RdWr_Err             = 0x55,
  epod_vok_m_Er                 = 0x56,
  epod_vok_l_Er                 = 0x57,
  esram0_epod_vok0_Er           = 0x58,
  esram12_epod_vok_Er           = 0x59,
  Mod_SwReset_Ack_Er			= 0x60, // error message
  NoPwrChgt_DmaOnGoing			= 0x61, // error message
};

// (true/false) STATE DEFINITION


// A9 CLOCK SCHEMES
enum clk_arm {
  A9Off,
  A9Boot,
  A9OpPt1,
  A9OpPt2,
  A9ExtClk
};


// GEN#0/GEN#1 CLOCK SCHEMES
enum clk_gen{
  GEN_OFF,
  GEN_BOOT,
  GEN_OPT1,
};

typedef struct{
  unsigned char val[8];
} hwacc_state;

enum ddr_frequency {
    DDRF_200MHZ = 200,
    DDRF_266MHZ = 266,
    DDRF_333MHZ = 333
};

enum max_cpu_frequecny {
    MCPUF_650MHZ = 650,
    MCPUF_800MHZ = 800,
    MCPUF_1000MHZ = 1000
};

enum max_aclk_frequecny {
    MACLKF_200MHZ = 200,
    MACLKF_266MHZ = 266,
    MACLKF_MAXMHZ = 333
};

enum max_sga_frequecny {
    MSGAF_200MHZ = 200,
    MSGAF_266MHZ = 266,
    MSGAF_MAXMHZ = 333
};

enum avs_varm_opp {
	AVS_VARM_OPP_100,
	AVS_VARM_OPP_50,
	AVS_VARM_OPP_RET
};

enum avs_vape_opp {
    AVS_VAPE_OPP_MAX,
    AVS_VAPE_OPP_133,
    AVS_VAPE_OPP_100
};

enum avs_vsafe_opp {
    AVS_VSAFE_OPP_100
};

#define SW_RESET_WD_DEEP_SLEEP              (0xD0D0)
#define SW_RESET_WD_CACHE_CLEAN_UNCONFIRMED (0xD0D1)
#define SW_RESET_WD_CACHE_CLEAN_CONFIRMED   (0xD0D2)
#define SW_RESET_ABB_XTAL_FAILURE           (0xD0D4)

void reset_has_occured(void);
#endif /* ifndef INCLUSION_GUARD_NUMERICS_H */
