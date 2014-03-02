#ifndef NUMERICS_H
#define NUMERICS_H

/*----------------------------------------------------------------------------*
 *      STEricsson, Incorporated. All rights reserved.                        *
 *                                                                            *
 *        STEricsson, INCORPORATED PROPRIETARY INFORMATION                    *
 * This software is supplied under the terms of a license agreement or non-   *
 * disclosure agreement with STEricsson and may not be copied or disclosed    *
 * except in accordance with the terms of that agreement.                     *
 *----------------------------------------------------------------------------*
 * System      : STxP70 Processor                                             *
 * File Name   : numerics.h                                                	  *
 * Purpose     : typedef definition											  *
 * Author(s)   : Didier MOULIN                                                *
 *               Helder DA SILVA VALENTE                                      *
 * History     : 2009/10/26 - First draft implementation. (Helder)            *
 *----------------------------------------------------------------------------*/

// general purpose On/Off states 
typedef enum {
  OffStat = 0,
  OnStat  = 1
} t_OnOffSt;


// general purpose On/Off/Retention states
typedef enum {
  OffSt = 0,
  OnSt  = 1,
  RetSt = 2
} t_OnOffRetSt;

  
// 8500 Ap power states
typedef enum {
  NoPwrSt       = 0x00, 
  ApBoot        = 0x01, 
  ApExecute     = 0x02,
  ApDeepSleep   = 0x03,
  ApSleep       = 0x04,
  ApIdle        = 0x05,
  ApReset       = 0x06 
} t_ApPwrSt;

  
// STM register
typedef enum {
  /* Debug Val for action ok */
//empty field = 0x00
  PLLARMLOCKP_OK			= 0x01,
  PLLDDRLOCKP_OK			= 0x02,
  PLLSOC0LOCKP_OK			= 0x03,
  PLLSOC1LOCKP_OK			= 0x04,
  ARMWFI_OK					= 0x05,
  SYSCLK_OK					= 0x06,
  PLL32KLOCKP_OK			= 0x07,
  DDRDEEPSLEEP_OK			= 0x08,
/* Debug Val for IT13 process */
  ANCH_IT13					= 0x09,
  SIDETONEH_IT13			= 0x0A,
  DEFAULT_IT13				= 0x0B,
/* Debug Val for timing_loop_fct */
  TMLFCT_IN					= 0x0C,
  TMLFCT_TIMER				= 0x0D,
  TMLFCT_OUT				= 0x0E,
/* Debug Val for IT10 process */
  PWRSTTRH_IT10				= 0x0F,
  CFGWKUPSHEXE_IT10			= 0x10,
  RDWKUPACKH_IT10			= 0x11,
  CFGWKUPSHSLEEP_IT10		= 0x12,
/* Debug Val for sxammdsp_fct */
  FIFO4500IT_SVA_ACTIVE		= 0x13,
  FIFO4500IT_SIA_ACTIVE		= 0x14,
/* actived signal detection for _it5_4500_fifo */
  GOEXE_IT10				= 0x15,
  NOGOEXE_IT10				= 0x16,
  GODS_IT10					= 0x17,
  NOGODS_IT10				= 0x18,
  GOSL_IT10					= 0x19,
  NOGOSL_IT10				= 0x1A,
  GOID_IT10					= 0x1B,
  NOGOID_IT10				= 0x1C,
  NOPWRTR_IT10				= 0x1D,
/* Debug Val for It3 subprogram */
  IT3_XP70_APE_WAKEUP_IN	= 0x1E,
  IT3_XP70_APE_WAKEUP_OUT	= 0x1F,
/* Debug Val for It4 subprogram */
  IT4_MODEM_DDR_IN			= 0x20,
  IT4_MODEM_DDR_OUT			= 0x21,
/* Debug Val for It5 subprogram */
  IT5_4500_FIFO_IN			= 0x22,
  IT5_4500_FIFO_OUT			= 0x23,
/* Debug Val for It6 subprogram */
  IT6_A9WD_SFTRST_IN		= 0x24,
  IT6_A9WD_SFTRST_OUT		= 0x25,
/* Debug Val for It7 subprogram */
  IT7_GPIO_IN				= 0x26,
  IT7_GPIO_OUT				= 0x27,
/* Debug Val for It10 subprogram */
  IT10_PWRSTATE_MGT_IN		= 0x28,
  IT10_PWRSTATE_MGT_OUT		= 0x29,
/* Debug Val for It11 subprogram */
  IT11_DVFS_IN				= 0x2A,
  IT11_DVFS_OUT				= 0x2B,
/* Debug Val for It12 subprogram */
  IT12_DPS_IN				= 0x2C,
  IT12_DPS_OUT				= 0x2D,
/* Debug Val for It15 subprogram */
  IT14_8500DBG_IN			= 0x2E,
  IT14_8500DBG_OUT			= 0x2F,
/* /Debug Val for It15 subprogram */
  IT15_ARM_IT_MGT_IN		= 0x30,
  IT15_ARM_IT_MGT_OUT		= 0x31,
/* Debug Val for It15 subprogram */
  IT18_SVAMMDSP_IN			= 0x32,
  IT18_SVAMMDSP_OUT			= 0x33,
/* Debug Val for It15 subprogram */
  IT19_SIAMMDSP_IN			= 0x34,
  IT19_SIAMMDSP_OUT			= 0x35,
/* Debug Val for It8 subprogram */
  IT8_DDR_CONFIG_IN			= 0x36,
  IT8_DDR_CONFIG_OUT		= 0x37,
/* Debug Val for It16 subprogram */
  IT16_DDR_SERVICE_IN		= 0x38,
  IT16_DDR_SERVICE_OUT		= 0x39,
/* Debug Val for It17 subprogram */
  IT17_GENI2C_IN			= 0x3A,
  IT17_GENI2C_OUT			= 0x3B,
/* Debug Val for It13 subprogram */
  IT13_AUDIOFILTER_IN		= 0x3C,
  IT13_AUDIOFILTER_OUT		= 0x3D,
/* Debug Val for It25 subprogram */
  IT25_XP70_MODEM_POWER_IN	= 0x3E,
  IT25_XP70_MODEM_POWER_OUT	= 0x3F,
/* Debug Val for polling_fct */
  POLFCT_IN					= 0x40,
  POLFCT_TIMER				= 0x41,
  POLFCT_ERR 			    = 0x42,
/* Debug Val for pollingon romcode */
  XP70RST_POLDRFCT_TIMER	= 0x43,
  XP70RST_POLDRFCT_ERR		= 0x44,
/* Debug Val for polling_a9wfi_fct */
  POLFCT_A9WFI_IN 			= 0x45,
  POLFCT_A9WFI_TIMER		= 0x46,
  POLFCT_A9WFI_ERR			= 0x47,
/* Debug Val for IT25 process */
  I2C_RD_IT25				= 0x48,
  MODSYSCLKREQ_IT25			= 0x49,
  MODSYSCLKREQ_RIS_IT25  	= 0x4A,
  MODSYSCLKREQ_FAL_IT25		= 0x4B,
  MODPWRENA_IT25			= 0x4C,
  MODPWRENA_RISING_IT25		= 0x4D,
  MODPWRENA_FALLING_IT25	= 0x4E,
  MODSTATE_IT25				= 0x4F,
  MODSTATE_VMODSEL2_IT25	= 0x50,
  MODSTATE_VMODSEL1_IT25	= 0x51,
  MODSTATE_VMODSEL_IT25		= 0x52,
  MODAUXCLKREQ_IT25			= 0x53,
  MODAUXCLKREQ_RIS_IT25		= 0x54,
  MODAUXCLKREQ_FAL_IT25		= 0x55,
/* Debug Val for hw_evt_fct */
  HW_EVT_IN					= 0x56,
  INVRDP_PONG				= 0x57,
  INVRDP_PING				= 0x58,
  WKUP_ST_RDWR				= 0x59,
  WKUP_ST_RD				= 0x5A,
  WKUP_ST_RDWR_ERR			= 0x5B,
  WKUP4500_INIT				= 0x5C,
  WKUP_ST_WR 				= 0x5D,
/* Debug Val for i2c_acess_fct */
  I2C_IN					= 0x5E,
  I2C_POLFCT_TIMER			= 0x5F,
  I2C_POLFCT_ERR 			= 0x60,
  I2C_POLFCT_ABORT 			= 0x61,
  I2C_OUT					= 0x62,
/* Debug Val for Geni2c_acess_fct */
  GENI2C_IN					= 0x63,
  GENI2C_POLFCT_TIMER 		= 0x64,
  GENI2C_POLFCT_ERR			= 0x65,
  GENI2C_ABORT 				= 0x66,
  GENI2C_OUT				= 0x67,
/* Debug Val for IT16 process */
  DDRINITH_IT16				= 0x68,
  MEMSTH_IT16				= 0x69,
  AUTOREFH_IT16				= 0x6A,
  AUTOPWRH_IT16				= 0x6B,
  PSREFCS0H_IT16			= 0x6C,
  PSREFCS1H_IT16			= 0x6D,
  CSPWRMGTH_IT16			= 0x6E,
  SYSCLK_IT16				= 0x6F,
  SYSCLKON_IT16				= 0x70,
//empty field = 0x71,
  SYSCLKOFF_IT16			= 0x72,
  MODSYSCLKOFF1_IT16		= 0x73,
  DEFAULT_IT16				= 0x74,
/* Debug Val for romcode polling on execute_to_deepsleep */
  ROMCODE_READY_TIMER		= 0x75,
  ROMCODE_READY_ERR			= 0x76,
  ROMCODE_READY_TO_DS		= 0x77,
  ROMCODE_NOREADY_TO_DS		= 0x78,
/* Debug Val for _init_fct */
  INIT_IN					= 0x79,
  INIT_I2C_SECURE			= 0x7A,
  INIT_I2C_NOSECURE			= 0x7B,
/* Debug Val for sysclk_mgt_fct */
  SYSCLK_MGT_IN				= 0x7C,
  SYSCLKVALID_I2C			= 0x7D,
/* Debug Val for stn4500_init_sequency_fct */
  INITSEQ_IN				= 0x7E,
  INITSEQ_OUT				= 0x7F,
/* Debug Val for wake_up_fct */
  WUFCT_IN					= 0x80,
  WUFCT_DS					= 0x81,
  WUFCT_SL					= 0x82,
  WUFCT_ID					= 0x83,
  WUFCT_DEFAULT				= 0x84,
/* Debug Val for IT3 process */
  CAWAKE_FALLING			= 0x85,
  WAKEUP_IT3				= 0x86,
  APEINT4500_1				= 0x87,
  APEINT4500_2				= 0x88,
  APEINT4500_3				= 0x89,
/* Debug Val for hwacc_fct */
  HWACFCT_IN_ESRAM12		= 0x8A,
  HWACFCT_IN_ESRAM34		= 0x8B,
  HWACFCT_IN_SVADSP			= 0x8C,
  HWACFCT_IN_SVAPIPE		= 0x8D,
  HWACFCT_IN_SIADSP			= 0x8E,
  HWACFCT_IN_SIAPIPE		= 0x8F,
  HWACFCT_IN_SGA 			= 0x90,
  HWACFCT_HWON				= 0x91,
  HWACFCT_HWOFF				= 0x92,
  HWACFCT_HWOFFRAMRET		= 0x93,
  HWACFCT_IN_APERAMS 		= 0x94,
  HWACFCT_IN_B2R2MCDE		= 0x95,
  HWACFCT_HWONCLKOFF		= 0x97,
  HWACC_VOK					= 0x98,
/* Debug Val for polling on PRCM_MOD_SWRESET_ACK */
  MOD_SWRESET_ACK_OK		= 0x96,
/* Debug Val for IT4 process */
  MOD_MEM_REQ_FALLING1		= 0x99,
  MOD_MEM_REQ_FALLING0		= 0x9A,
  HOSTACCESS_IN				= 0x9B,
  MOD_MEM_REQ_IN			= 0x9C,
  MOD_MEM_REQ_RISING		= 0x9D,
  MOD_MEM_REQ_FALLING		= 0x9E,
/* Debug Val for IT23 process */
  TIMER3_0_IT23				= 0x9F,
  TIMER3_1_IT23				= 0xA0,
/* Debug Val for IT24 process */
  APE_MEM_ACK_1_IT24		= 0xA1,
/* Debug Val for exe_to_DS/SL/ID  */
/* when wake up evt has been happened before */
  RDYTODS_RETURNTOEXE		= 0xA2,
  RDYTOSL_RETURNTOEXE		= 0xA3,
  RDYTOID_RETURNTOEXE		= 0xA4,
/* Debug Val for a9_dvfs_fct */
  DVFSFCT_A9_IN				= 0xA5,
  DVFSFCT_A9_100OPP			= 0xA6,
  DVFSFCT_A9_50OPP			= 0xA7,
  DVFSFCT_A9_EXT			= 0xA8,
  DVFSFCT_A9_DEFAULT		= 0xA9,
/* Debug Val for clkarm_fct */
  CLK_A9_BOOT				= 0xAA,
  CLK_A9_OPT1				= 0xAB,
  CLK_A9_OPT2				= 0xAC,
  CLK_A9_EXT				= 0xAD,
  CLK_A9_DEFAULT			= 0xAE,
/*  */
//empty field = 0xAF,
//empty field = 0xB0,
//empty field = 0xB1,
/* Debug Val for clkscalable_fct */
  CLK_APES_BOOT				= 0xB2,
  CLK_APES_OPT1				= 0xB3,
  CLK_APES_DEFAULT			= 0xB4,
/* Debug Val for clkfixed_fct */
  CLK_APEF_BOOT				= 0xB5,
  CLK_APEF_OPT1				= 0xB6,
  CLK_APEF_DEFAULT			= 0xB7,
/* Debug Val for arm_clk_mgt_execute_to_sleepmode_fct */
  CLK_A9_MGT				= 0xB8,
  CLK_A9_MGT_NODBG			= 0xB9,
/* Debug Val for ape_clk_mgt_execute_to_sleepmode_fct */
  CLK_APE_MGT				= 0xBA,
  CLK_APE_MGT_NODBG			= 0xBB,
/* Debug Val for power_domain_fct */
  PWR_DOMAIN				= 0xBC,
  PWR_DOMAIN_NODBG			= 0xBD,
/* Debug Val for arm_clk_mgt_sleepmode_to_execute_fct */
  CLK_A9_MGT_EXE			= 0xBE,
  CLK_A9_MGT_PLLARM_EN		= 0xBF,
/* Debug Val for IT17 process */
  I2C_RD_OK_IT17			= 0xC0,
/* Debug Val for DDR SUBSYSTEM configuration polling */
  DDR_CFG_TIMER 			= 0xC1,
/* Debug Val for ape_clk_mgt_sleepmode_to_execute_fct */
  CLK_APE_MGT_EXE			= 0xC2,
  CLK_APE_MGT_PLLSOC0_EN	= 0xC3,
  CLK_APE_MGT_PLLSOC1_EN	= 0xC4,
/* Debug Val for a9_reset_fct */
  A9_RESET					= 0xC5,
  A9_RESET_NODBG			= 0xC6,
  A9_RESET_DBG				= 0xC7,
/* Debug Val for ddr_config_fct */
  DDR_CFG					= 0xC8,
  DDR_CFG0					= 0xC9,
  DDR_CFG0_A				= 0xCA,
  DDR_CFG0_B				= 0xCB,
  DDR_CFG1					= 0xCC,
  DDR_CFG1_FIRST			= 0xCD,
/* Debug Val for pll32k_enable_fct */
  PLL32K_ENABLE				= 0xCE,
  PLL32K_STATE				= 0xCF,
/* Debug Val for It23 subprogram */
  IT23_TIMER3_IN			= 0xD0,
  IT23_TIMER3_OUT			= 0xD1,
/* Debug Val for It24 subprogram */
  IT24_APE_MEM_ACK_IN		= 0xD2,
  IT24_APE_MEM_ACK_OUT		= 0xD3,
/* Debug Val for pll32k_start_fct */
  PLL32K_START				= 0xD4,
  PLL32K_STATE1				= 0xD5,
  PLLDDR_START				= 0xD6,
/* Debug Val for pll32k_disable_fct */
  PLL32K_DISABLE			= 0xD7,
/* Debug Val for pll32k_stop_fct */
  PLL32K_STOP				= 0xD8,
/* Debug Val for ddr_state_sleep_idle_fct */
  DDR_STATE					= 0xD9,
  DDR_STATE_OFFLOWLAT		= 0xDA,
  DDR_STATE_OFFHIGHLAT		= 0xDB,
  DDR_STATE_ON				= 0xDC,
  DDR_STATE_DEFAULT			= 0xDD,
/* Debug Val for self reset */
  SELF_RESET				= 0xDE,
/* Debug Val for jump */
  JUMP						= 0xDF,
/* Debug Val for ape_dvfs_fct */
  DVFSFCT_APE_IN			= 0xE0,
  DVFSFCT_APE_100OPP		= 0xE1,
  DVFSFCT_APE_50OPP			= 0xE2,
  DVFSFCT_APE_DEFAULT		= 0xE3,
/* Debug Val for hwacc_fct */
  HWACFCT_AUTOPWRH			= 0xE4,
  HWACFCTH_DEFAULT			= 0xE5,
/* Debug val for SXAMMDSP/SXAPIPE into IT18/IT19 process */
  DSPOFF_HWPOFF_ST 			= 0xE6,
  DSPOFFRAMRET_HWPOFF_ST	= 0xE7,
  DSPCLKOFF_HWPOFF_ST		= 0xE8,
  DSPCLKOFF_HWPCLKOFF_ST	= 0xE9,
  DEFAULT_MMDSP_PIPE_OFF	= 0xEA,
  MMDSP_PIPE_OFF			= 0xEB,
/* wakeup event configuration for MMDSP */
  SVAMMDSP_WUP				= 0xEC,
  SIAMMDSP_WUP				= 0xED,
  HSEM_SEL	 				= 0xEE,
  FIFO4500IT_SEL			= 0xEF,
  HSEM_FIFO4500IT_SEL		= 0xF0,
  NO_MMDSP_WUP				= 0xF1,
/* actived signal detection for _it6_a9wd_sftrst */
  SOFT_RESET_ACTIVE 		= 0xF2,
  HSEM_SVA_IT_ACTIVE		= 0xF3,
  HSEM_SIA_IT_ACTIVE		= 0xF4,
/* Debug val for context SAVING/RESTORING process */
  READ_MMDSP				= 0xF5,
  WRITE_MMDSP				= 0xF6,
  CONTEXT_SAVING			= 0xF7,
  CONTEXT_RESTORING			= 0xF8,
  MMDSP_START 				= 0xF9,
/* debug val when modem run standalone */
  GOEXE_MODEMSTANDALONE 	= 0xFA,
/* ESRAM0 cfg */
  ESRAM0_VOK				= 0xFB,
  ESRAM0_IN					= 0xFC,
  ESRAM0_ON					= 0xFD,
  ESRAM0_OFF				= 0xFE,
  ESRAM0_RET				= 0xFF
} t_STM;


//---------------------------- Header for ReqMb0
typedef enum {
  PwrStTrH    		= 0,
  CfgWkUpsHExe		= 1,
  WkUpHExe       	= 2,
  RdWkUpAckH  		= 3,
  CfgWkUpsHSleep	= 4,
  WkUpHSleep       	= 5
} t_ReqMb0H;
  

// power state transitions
//2do:DM: values changed
typedef enum {
  NoTransition           			= 0,
  ApExecuteToApSleep     			= 1,
  ApIdleToApSleep        			= 2,
  ApBootToApExecute      			= 3, 
  ApExecuteToApDeepSleep 			= 4,
  ApExecuteToApIdle      			= 5,
  ApBootToApExecuteModemStandAlone	= 6 
} t_ApPwrStTr;


// ddr power states
typedef enum {
  UnChanged     = 0x00,
  On            = 0x01,
  OffLowLat     = 0x02,
  OffHighLat    = 0x03
} t_DdrPwrSt;

// implementation issue: the values are chosen the way that
// we can change from ping to pong (resp. pong to ping) by
// simply using the 'not' operator in C, e.g. toggling operation:
// t_PingPong p = ping; p = ~p;
typedef enum {
  ping = 0,
  pong = 1
} t_PingPong;

// Defines the states of the hardware events (resp. wake-up) reason fields (WRF).
// WRF will be set in line with the mask configured via ReqMb0
typedef enum {
  // WRF has been written but neither sent nor read by the arm
  EvtWr = 1,
  // WRF has been written and sent, but not yet read by the arm
  EvtSt = 2,
  // WRF has been written, sent and read by the arm
  EvtRd = 0
} t_WkUpReasonFdSt; // Wake-up reason Field State

//---------------------------- Header for ReqMb1
typedef enum {
  ArmApeOppH           = 0x00,
  ArmApeRestore100OppH = 0x01
} t_ReqMb1H;


// ARM OPP STATES
typedef enum {
  ArmOppInit= 0x00, 
  ArmNoChg	= 0x01,
  Arm100Opp	= 0x02,
  Arm50Opp	= 0x03,
  ArmExtClk	= 0x07
} t_ArmOpp;


// APE OPP STATES
typedef enum {
  ApeOppInit= 0x00, 
  ApeNoChg	= 0x01,
  Ape100Opp	= 0x02,
  Ape50Opp	= 0x03
} t_ApeOpp;


// Hw Accelerator states
typedef enum {
  HwNoChgt     = 0x00,
  HwOff        = 0x01,
  HwOffRamRet  = 0x02, 
  HwOnClkOff   = 0x03, // hw on and clock gated
  HwOn         = 0x04  // hw on and clocked
} t_HwAccSt;
  

//---------------------------- Header for ReqMb2 
typedef enum {
  dpsH     = 0x00,
  autoPwrH = 0x01
} t_ReqMb2H;

// auto power OFF policy
typedef enum {
  NOCHGT       		  = 0x0, 
  DSPOFF_HWPOFF       = 0x1, 
  DSPOFFRAMRET_HWPOFF = 0x2,
  DSPCLKOFF_HWPOFF    = 0x3,
  DSPCLKOFF_HWPCLKOFF = 0x4
} t_SiaSvaPwrPolicy;


//---------------------------- Header for ReqMb3 
typedef enum {
  AncH      = 0x00,
  SidetoneH = 0x01
} t_ReqMb3H;


//---------------------------- Header for ReqMb4
typedef enum {
  ddrInitH         = 0x00,
  memStH           = 0x01, 
  autoSrefreshH    = 0x02,  
  autoPwrDnH       = 0x03,
  partialSrefreshCS0H = 0x04, 
  partialSrefreshCS1H = 0x05, 
  csPwrMgtH        = 0x06,
  sysClkH          = 0x0A,
  workAround2H     = 0x0F
} t_ReqMb4H;

  
//---------------------------- status/Error
typedef enum {
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
  CfgWkUpsHExeOk	 	 = 0xEA,  
  CfgWkUpsHSleepOk 		 = 0xE9,  
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
  I2c_NAck_Data0123_Er          = 0x1B, /* PRCM_HWI2C_SR(4:2)="110", PRCM_HWI2C_SR(1:0)="11" */ 
  I2c_NAck_Addr_Er              = 0x1F, /* PRCM_HWI2C_SR(4:2)="111", PRCM_HWI2C_SR(1:0)="11" */ 
  CurApPwrStIsNotBoot           = 0x20,
  CurApPwrStIsNotExecute        = 0x21,
  CurApPwrStIsNotSleepMode      = 0x22,
  CurApPwrStIsNotCorrectForIT10 = 0x23,          
  Fifo4500WuIsNotWupEvent       = 0x24,          
  Pll32kLockp_Er                = 0x29, 
  DDRDeepSleepOk_Er				= 0x2A, 
  RomcodeReady_Er               = 0x50,
  WupBeforeDs                   = 0x51,
  DdrConfig_Er                  = 0x52,
  WupBeforeSleep                = 0x53,
  WupBeforeIdle                 = 0x54,
  WkUpR_st_RdWr_Err             = 0x55,
  epod_vok0_Er            		= 0x56,
  epod_vok1_Er            		= 0x57,
  esram0_epod_vok0_Er			= 0x58,
  Mod_SwReset_Ack_Er			= 0x60
} t_ApPwrStTrStatus;
  
  
typedef enum {
  PwrDwnCS0 = 0,
  WkUpCS0   = 1,
  PwrDwnCS1 = 2,
  WkUpCS1   = 3
} t_csPwrMgt;
    

typedef enum {
  I2CWrite = 0,
  I2CRead  = 1
} t_I2CopType;
  
  
typedef enum {
  MMDSPOn		= 0x02,
  MMDSPOff		= 0x01,
  MMDSPInit		= 0x00
} t_MmdspStatus;
 
  
typedef enum {
  AckMb3Init  = 0,
  ANC_Ok      = 1,
  sidetone_Ok = 2,
  ANC_Er      = 11,
  sidetone_Er = 12
} t_AckMb3Status;


  typedef enum {
  AckMb4Init  = 0,
  SysClkOn_Ok = 1,
  DDROn_Ok = 2
} t_AckMb4Status;

  
typedef enum {
  AckMb5Init  = 0x00,
  I2C_Wr_Ok   = 0x01,
  I2C_Rd_Ok   = 0x02,
  SysClk_Ok   = 0x03,
  I2C_TimeOut = 0x11,
  SysClk_Er   = 0x12,
  // Error Status resent by PRCM_HWI2C_SR
  I2cWr_NAck_Data_Er			= 0x07, /* PRCM_HWI2C_SR(4:2)="001", PRCM_HWI2C_SR(1:0)="11" */
  I2cWr_NAck_Reg_Addr_Er		= 0x0B, /* PRCM_HWI2C_SR(4:2)="010", PRCM_HWI2C_SR(1:0)="11" */ 
  I2cRdWr_NAck_Data0123_Er		= 0x1B, /* PRCM_HWI2C_SR(4:2)="110", PRCM_HWI2C_SR(1:0)="11" */ 
  I2cWr_NAck_Addr_Er			= 0x1F, /* PRCM_HWI2C_SR(4:2)="111", PRCM_HWI2C_SR(1:0)="11" */ 
  I2cRd_NAck_Addr_Init_Er		= 0x0F, /* PRCM_HWI2C_SR(4:2)="011", PRCM_HWI2C_SR(1:0)="11" */
  I2cRd_NAck_Reg_Addr_Init_Er	= 0x13, /* PRCM_HWI2C_SR(4:2)="100", PRCM_HWI2C_SR(1:0)="11" */ 
  I2cRd_NAck_Addr_Er			= 0x17 /* PRCM_HWI2C_SR(4:2)="101", PRCM_HWI2C_SR(1:0)="11" */ 
} t_AckMb5Status;
  
  
/////////////////////////////
// Messages for communication with romcode 
/////////////////////////////
typedef enum {
  Init   		= 0x00,
  FsToDs 		= 0x0A,
  EndDs	 		= 0x0B,
  DsToFs 		= 0x0C,
  EndFs	 		= 0x0D,
  Swr    		= 0x0E,
  EndSwr 		= 0x0F,
  ReadyToXp70Rst= 0x10,
  GoWFI 		= 0x40,
  NoGoWFI 		= 0x41,
  ReadyToDs 	= 0x80,
  NoReadyToDs_Er1	= 0x81,
  NoReadyToDs_Er2	= 0x82,
  NoReadyToDs_Er3	= 0x83,
  NoReadyToDs_Er4	= 0x84,
  NoReadyToDs_Er5	= 0x85,
  NoReadyToDs_Er6	= 0x86,
  NoReadyToDs_Er7	= 0x87,
  NoReadyToDs_Er8	= 0x88,
  NoReadyToDs_Er9	= 0x89,
  NoReadyToDs_ErA	= 0x8A,
  NoReadyToDs_ErB	= 0x8B,
  NoReadyToDs_ErC	= 0x8C,
  NoReadyToDs_ErD	= 0x8D,
  NoReadyToDs_ErE	= 0x8E,
  NoReadyToDs_ErF	= 0x8F  
} t_RomcodeMb;


typedef enum {
  ModSwResetReq = 0x03,
  CaSleepReq    = 0x02,
  HostPortAck   = 0x01,
  AckMb7Init    = 0x00
} t_AckMb7Status;


typedef enum {
  DVFSOk			= 0xA,
  DVFSReqMb1Err		= 0xB,
//  A9DVFSOk			= 0x0A,
//  A9DVFSReqMb1Err	= 0x01,
//  APEDVFSOk			= 0xA0,
//  APEDVFSReqMb1Err	= 0x10,
  InitDvfsStatus	= 0x0
} t_DvfsStatus;


typedef enum {
  HwAccPwrStGo			= 0xFF,
  HwAccPwrStOk			= 0xFE,
  InitHwAccPwrStatus	= 0x00
} t_DpsStatus;


// (true/false) STATE DEFINITION
typedef enum {
  tfalse = 0x00,
  ttrue  = 0x01
} t_TState;


// A9 CLOCK SCHEMES
typedef enum {
  A9Off, 
  A9Boot, 
  A9OpPt1, 
  A9OpPt2, 
  A9ExtClk 
} t_ClkArm;


// GEN#0/GEN#1 CLOCK SCHEMES
typedef enum {
  GenOff, 
  GenBoot, 
  GenOpPt1, 
} t_ClkGen;


// timer3_state
typedef enum {
  TOff= 0x00, 
  TModSysClkReq, 
  TModPwrEna
} t_Timer3;


typedef struct s_MMDSP {
 unsigned int EMUL_UDATA[8];	/* @0x80 to @0x9C */
 unsigned int EMUL_UADDRL; 		/* @0xA0 */
 unsigned int EMUL_UADDRM; 		/* @0xA4 */
 unsigned int EMUL_UCMD; 		/* @0xA8 */
 unsigned int EMUL_UBKCMD; 		/* @0xAC */
 unsigned int fill1[11];		/* @0xB0 to @0xD4*/
 unsigned int EMUL_UADDRH; 		/* @0xD8 */
 unsigned int fill2[4];			/* @0xDC to @0xE4*/
 unsigned int EMUL_CLKCMD; 		/* @0xE8 */
}s_MMDSP;


typedef struct{
  unsigned char val[8];
} t_hwacc_state;


#endif /* !defined(NUMERICS_H) */
