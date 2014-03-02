#ifndef INCLUSION_GUARD_PARAM_DEFINE_H
#define INCLUSION_GUARD_PARAM_DEFINE_H

/*----------------------------------------------------------------------------*
 *      Copyright 2000-2005, STMicroelectronics, Incorporated.                *
 *      All rights reserved.                                                  *
 *                                                                            *
 *        STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION            *
 * This software is supplied under the terms of a license agreement or nondis-*
 * closure agreement with STMicroelectronics and may not be copied or disclo- *
 * sed except in accordance with the terms of that agreement.                 *
 *----------------------------------------------------------------------------*
 * System           : STxP70 Processor                                        *
 * Project Component: System Library                                          *
 * File Name        : param_define.h                                         *
 * Purpose          : parameters definitions used by prcmufw.c file           *
 * History          : 2009/09/21 - First implementation.                      *
 *----------------------------------------------------------------------------*/

//////////////////////
/// SIMULATION SETTING
//////////////////////
#ifdef MAPSIMU_CODE
  // shorter timing loops to accelerate simulation
  #define SIMULATION
/*
  // bypass read/write actions on I2C because palladium simulation does not have 4500 model
  #define SIMULATION_ROMCODE
*/
  // Put here code that is meant to be executed on the VHDL platform (map)
  #undef _IO_
  #define _IO_(x)
  #define _PROBE_INT_(y)
  #define _BOOTSTATUS_(x) share_var.boot_status_debug_var = x;
  #define _DEBUG_(x) PRCMUREG->PRCM_STM_DATA = x;
#endif

//////////////////////////////////
/// DEVELOPMENT VERSION
//////////////////////////////////
#ifdef DEV_CODE
/* Put here code that is meant to be executed on the xp70 toolset (dev) */
#undef _IO_
#define _IO_(x) x
#define _PROBE_INT_(y) printf("Value of <" #y "> = %u (%#x)\n", y, y);
#define _DEBUG_(x) share_var.boot_status_debug_var = x; printf("boot_status_debug_var = %u\n", x);
#endif

//////////////////////////////////
/// MAPPING REAL VERSION
//////////////////////////////////
#ifdef MAP_CODE

  /* Put here code that is meant to be executed on the VHDL platform (map) */
  #undef _IO_
  #define _IO_(x)
  #define _PROBE_INT_(y)
  #define _BOOTSTATUS_(x) share_var.boot_status_debug_var = x;
  #define _DEBUG_(x) PRCMUREG->PRCM_STM_DATA = x;
  // allow to correct I2C hardware bug replacing by software I2C
  #define I2C_WORKAROUND
#endif

///////////////////////////////////
// DEFINE for ReqMb0
///////////////////////////////////
#define NB_HWEVT4500 	20	/* number of mailbox 8bit to write wake up reason coming from ST4500 */


///////////////////////////////////
// DEFINE for DENALI CONTROLER
///////////////////////////////////
#define NB_MASK					5	/* number of word 32b to mask the DENALI controler regiters that are not to configure */
#define DENALI_CTL_MAX_VALUE 	144+1 /* last DENALI_CTL register to load +1 */
#define NB_BIT					32
#define LAST_LOOP_INDICE		DENALI_CTL_MAX_VALUE-((NB_MASK-1)*NB_BIT)


//////////////////////////////////
/// DEFINE VARIABLE DESCRIPTION
//////////////////////////////////

#define PRCMUREG_I2C_SECURE ((volatile struct s_PRCMU *) 0x00808000)
#define PRCMUREG	((volatile struct s_PRCMU *) 0x00807000)
#define DBAHN 		((volatile struct s_DBAHN *) 0x00806000)
#define MSP1REG     ((volatile struct s_MSP1 *)  0x00809000)
#if 0
#define TCDM_BEG  ((volatile struct s_initTCDM *) 0x00000000)
#define SVAMMDSPREG  ((volatile struct s_MMDSP *) 0x00807680)
#define SIAMMDSPREG  ((volatile struct s_MMDSP *) 0x00807A80)
#endif
// MMDSP STATUS
#define MMDSP_UCMD_CTRL_STATUS_ACCESS	0x10
#define MMDSP_UCMD_WRITE				0
#define MMDSP_UCMD_READ					4
#define MMDSP_UBKCMD_EXT_CODE_MEM_ACCESS_ENABLE    (1 << 3)
#define MMDSP_CLOCKCMD_START_CLOCK        0



// variable definition for power_state
#define VARM_RET		0x09
#define VARM_OFF		0x00
#define POWER_STATE_OFF	0x00
#define POWER_STATE_RET	0x01


///////////////////////////////////
// DEFINE for I2C
///////////////////////////////////
#define BANK2_I2C	0x02
#define BANK4_I2C	0x04
#define BANKE_I2C	0x0E


///////////////////////////////////
// DEFINE for BOOTSTATUS
///////////////////////////////////
#define MAINBEG		0xFE
#define MAINEND		0xFF


//////////////////////////
// APPLICATION PARAMETERS
//////////////////////////

/* Vsupply used from STn4500 */
#define VARMHIGHSPEED	1.125
#define VARMLOWSPEED	1.0375
#define VARMRET			0.75
#define VAPEHIGHSPEED	1.1375
#define VAPELOWSPEED	1.0375
#define VSAFEHP			1.1625
#define VMODSEL1		1.1625
#define VMODSEL2		1.1625  /* VMODSEL2 MUST BE > VMODSEL1 */
#define VTIMESLOPE1		2.4 	/* Slope from 2.4V/ms for STn4500 de  0V a 1.1V */
#define VTIMESLOPE2		10 		/* Slope from 10V/ms for STn4500 de 1.1V a 1.2V */

/* CLK definition */
#define CLK32_VAL		   32 	/* CLK32 value(32KHz) */
#define SYSCLK_VAL		38400 	/* SYSCLK value(38.4Mhz) */
#define ULPCLK_VAL		38400	/* ULPCLK value(38.4Mhz) */
#define FREERUNOSC_VAL	38400	/* FREE RUNNING OSCILLATOR value(38.4Mhz) */

/* Timing definition */
#define TIME_ULPCLKRDY_IDLE  7.8 	/* Time used to have ULPCK ready for power state changing from idle to execute (7.8 ms) */
#define TIME_ULPCLKRDY_SLEEP 7.5 	/* Time used to have ULPCK ready for power state changing from sleep/deepsleep to execute (7.5 ms) */
#define TIME_PLL_ENABLE		 0.013	/* Time used to have PLLs available after enable='1' (13.3 us) */
#define TIME_PLLSOC_LOCKP	 0.2133	/* Time used to Lock PLLSOC0/PLLSOC1(213.3 us) */
#define TIME_PLLARM_LOCKP	 0.2133 /* Time used to Lock PLLARM (213.3 us) */
#define TIME_PLLDDR_LOCKP	 0.2133 /* Time used to Lock PLLARM (213.3 us) */
#define TIME_PLL32K_LOCKP	 8 		/* Time used to Lock PLL32K (8 ms) */
#define TIME_SYSCLKOK		 2		/* Time used by STn4500 to have SYSCLK available (2 ms) */
#define TIME_4500RST		 13		/*  Time used by STn4500 for reset (13 ms) */
#define TIME_I2C			 0.008	/* Timeout definition for enabling I2C access(4us), but two access followed (8us)*/
#define TIME_ARMWFI			 55		/* Timeout definition for Arm in Wfi mode(55 ms) */
#define TIME_BOOT			 1		/* Timeout definition when polling on romcode_mb=ReadyToXp70Rst(1 ms) */
#define TIME_TO_VMOD_STABLE  0.5    /* Time to have VMOD ON when modem power mgt is requested */
/* TIME definition used by DDR to go in deepsleep mode before receiving mod_mem_req='0' and mod_mem_latency='1' */
/* TIME_DDRDEEPSLEEP_VAL= (TIME_DDRDEEPSLEEP*ULPCLK_VAL)= (0.5*38400)= 0d16 */
#define TIME_DDRDEEPSLEEP	 0.5	/* Time used by DDR to go in deepsleep mode before receiving mod_mem_req='0' and mod_mem_latency='1' */

/* Timing loop definition */
/* Value to put into timer ref of timing_loop_fct (take into account time to go from VApeHighSpeed to VApeLowSpeed) */
/* TIME_VAPEHIGHSPEED_VAPELOWSPEED=	(((VAPEHIGHSPEED-VAPELOWSPEED)/VTIMESLOPE)*ULPCLK_VAL)= (((1.1375-1.0375)/2.4)*38400)= 0d1600 */
#define TIME_VAPEHIGHSPEED_VAPELOWSPEED	0x00000640
/* Value to put into timer ref of timing_loop_fct (take into account time to go from VArmHighSpeed to VArmLowSpeed) */
/* TIME_VARMHIGHSPEED_VARMLOWSPEED=	(((VARMHIGHSPEED-VARMLOWSPEED)/VTIMESLOPE)*ULPCLK_VAL)= (((1.125-1.0375)/2.4)*38400)= 0d1400 */
#define TIME_VARMHIGHSPEED_VARMLOWSPEED	0x00000580
/* Value to put into timer ref of timing_loop_fct  (take into account time to go from VArmHighSpeed to VArmRetention) */
/* TIME_VARMHIGHSPEED_VARMRET= (((VARMHIGHSPEED-VARMRET)/VTIMESLOPE)*ULPCLK_VAL)= (((1.125-0.75)/2.4)*38400)= 0d6000 */
#define TIME_VARMHIGHSPEED_VARMRET		0x00001780
/* Value to put into timer ref of timing_loop_fct (take into account time to go from VMAX(VSAFEHP in this case) to V=0) */
/* TIME_VMAX_V0= (((VSAFEHP-0)/VTIMESLOPE)*ULPCLK_VAL)= (((1.1625-0)/2.4)*38400)= 0d18600 */
#define TIME_VMAX_V0						0x000048B0
/* Time of timing loop used to have ULPCLK available when going from deepslee/sleep/idle power state to excecute */
/* TIME_ULPCKREADY= (TIME_ULPCLKRDY_SLEEP*FREERUNOSC_VAL)= (7.5*38400)= 0d288000 */
#define TIME_ULPCKREADY					0x00046510
/* Timeout definition for polling registers on SYSCLK available by STn4500*/
/* TIME_SYSCLKOK_VAL= (TIME_SYSCLKOK*ULPCLK_VAL)= (4.2*52000)= 218400 = 0x35520  (assuming that 52MHz is the highest PRCMU CLK FREQ even for free running */
#define TIME_SYSCLKOK_VAL					0x00035520
#define TIME_SYSCLKOK_VAL_US                (4200)
/* Time of timing loop needs for STn4500 reset */
/* TIME_4500RST_VAL= (TIME_4500RST*SYSCLK_VAL)= (13*38400)= 0d499200 */
#define TIME_4500RST_VAL					0x00079E01
#define TIME_4500RST_VAL_US                 (13000)
/* Timeout definition for polling registers on ARM in WFI */
/* TIME_ARMWFI_VAL= (TIME_ARMWFI*ULPCLK_VAL)= (55*38400)= 0d2112000 */
#define TIME_ARMWFI_VAL					0x00203A00
/* Timeout definition to go from starting ULPCLK to ModSysClkReq1Valid for modem power management */
/* (8ms-.5ms-2ms-0.1875ms-2ms)= 3.3125ms => (0.0033125*32000)=0d106 */
#define TIME_TO_ACTIV_MODSYSCLKREQ1VALID  0x0000006A
/* Value to put into timer ref of timing_loop_fct (take into account time to go from VMODSEL1 to VMODSEL2) */
/* TIME_VMODSEL1_VMODSEL2=	(((VMODSEL2-VMODSEL1)/VTIMESLOPE2)*ULPCLK_VAL)= (((1.1625-1.0625)/10)*38400)= 0d384 */
#define TIME_VMODSEL1_VMODSEL2 			0x00000180
// timing used to Supply is OK for hardware accelerator. TO BE DEFINED
#define VOK_POOLING_VAL 					0x00001FFF
#define VOK_POOLING_VAL_US                  (7)
// timing used to Supply is off for hardware accelerator. TO BE DEFINED
#define SUPPLY_ON_OFF 					0x000000FF
// Timeout definition for memory retention stabilization (bias generator)
// 0.015 ms * 52000 = 780 = 30c
#define MEM_RET_STABILIZATION            0x0000030c
/* Timeout definition to put Hostaccess_req to '0', after that MOD_MEM_REQ goes from '0' to '1' */
/* (10s*32000)=0d320000 */
#define TIME_TO_HOSTACCESS_REQ_0		    0x0004E200

/* Time used from PLL configuration to enable PLL */
/* TIME_PLL_ENABLE_VAL= (TIME_PLL_ENABLE*SYSCLK_VAL)= (0.013*26000)= 338,=0x152 */
#define TIME_PLL_ENABLE_VAL					0x00000152
#define TIME_PLL_ENABLE_VAL_US              (200)
/* Timeout definition for polling registers */
/* Time value used for PLL 32K is locked */
/* TIME_PLL32KLOCKP= (TIME_PLL32K_LOCKP*SYSCLK_VAL)= (8*38400)= 0d307200 */
#define TIME_PLL32KLOCKP					0x0004B000
/* Time value used for PLL SOC0/SOC1 is locked */
/* TIME_PLLSOCLOCKP= (TIME_PLLSOC_LOCKP*SYSCLK_VAL)= (0.2133*38400)= 0d8191 */
#define TIME_PLLSOCLOCKP					0x00001FFF
/* Time value used for PLL ARM is locked */
/* #define TIME_PLLARMLOCKP= (TIME_PLLARM_LOCKP*SYSCLK_VAL)= (0.2133*38400)= 0d8191 */
#define TIME_PLLARMLOCKP					0x00001FFF
/* Time value used for PLL DDR is locked */
/* #define TIME_PLLDDRLOCKP= (TIME_PLLDDR_LOCKP*SYSCLK_VAL)= (0.2133*38400)= 0d8191 */
#define TIME_PLLDDRLOCKP					0x00001FFF
/* Time definition for enabling I2C access */
/* define TIME_I2C_VAL= (TIME_I2C*SYSCLK_VAL)= (0.008*38400)= 0d307 */
#define TIME_I2C_VAL						0x00000140
/* Time definition for romcode_mb=ReadyToXp70Rst */
/* TIME_BOOT_VAL= (TIME_BOOT*SYSCLK_VAL)= (1*38400)= 0d38400 */
#define TIME_BOOT_VAL						0x20000000	 /* used for romcode simulation to be sure to not trigger timeout */
/* Time definition for romcode_mb=ReadyToXp70Rst */
#define TIME_ROMCODE_READY_VAL				0x20000001	 /* VALUE TO BE DEFINED BY ROMCODE TEAM */
/* SYSCLKOK_DELAY definition used by modem power management */
#define SYSCLKOK_DELAY_FOR_MODEM_PWR_MGT    0x00000006
/* TIME definition used by modem power management to have VMOD "ON" */
/* TIME_TO_VMOD_STABLE_VAL= (TIME_TO_VMOD_STABLE*CLK32_VAL)= (0.5*32)= 0d16 */
#define TIME_TO_VMOD_STABLE_VAL				0x00000010
/* TIME definition used by DDR to go in deepsleep mode */
/* TIME_DDRDEEPSLEEP_VAL= (TIME_DDRDEEPSLEEP*ULPCLK_VAL)= (0.5*38400)= 0d19200 */
#define TIME_DDRDEEPSLEEP_VAL				0x00004B00

#define NO_MODE_DEBUG_A9        0x0
#define NO_MODE_DEBUG_A9_MMDSP  0x0
#define NO_TRANSFER_DATA123     0x0
#define RESET_NO_EXIST          0x0
#define RETENTION_NO_EXIST      0x0
#define NO_EPOD_C_L_ON          0x0
#define NO_LS_CLAMP             0x0
#define NOPOWER_STATE           0x0
#define ENABLE_NO_EXIST         0x0
#define ENABLE_MASK             0x00000100

/* ---------------------------------------------- *
 * ABB                                            *
 * ---------------------------------------------- */

#define ABB_SLAVE_ADDRESS       0x0E 
#define ABB_LATCH_REG_FIRST     0x40
#define ABB_LATCH_REG_LAST      0x56


/* end of file */
#endif  // INCLUSION_GUARD_PARAM_DEFINE_H
