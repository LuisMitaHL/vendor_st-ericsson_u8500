/*****************************************************************************/
/**
*  Copyright ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage power
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HATS_POWER_H
#define _HATS_POWER_H

#include <stdint.h>			/* uint32_t ... */
#include <string.h>
#include <stdio.h>

typedef enum {
#ifdef PWS_9540
	AP_NORMAL,
	AP_EXECUTE,
	AP_RUNNING,
	AP_IDLE,
	AP_SLEEP,
	AP_DEEP_IDLE,
	AP_DEEP_SLEEP
#else
	AP_NORMAL,
    AP_EXECUTE,
	AP_RUNNING,
    AP_IDLE,
    AP_SLEEP,
    AP_DEEP_SLEEP_ESRAM0_RET,
    AP_DEEP_SLEEP_ESRAM0_OFF
#endif
} ap_mode_t;

typedef enum {
	DDR_OFFHIGHLAT,
	DDR_OFFLOWLAT,
	DDR_ON
} ap_ddr_mode_t;

typedef enum {
	DDR_CTRL0,
	DDR_CTRL1
} ap_ddr_ctrl_t;

typedef enum {
	DDR_IDLE,
	DDR_DEEPIDLE,
	DDR_SLEEP
} ap_power_state_t;

typedef enum {
	AP_ULP_TO_OFF,
	AP_ULP_TO_ON
} ap_ulpclk_t;

typedef enum {
	HWACC_ON,
    HWACC_OFF,
    HWACC_RETENTION
} hwacc_mode_t;

typedef enum {
	MODEM_LP_DISABLED,
    MODEM_LP_ENABLED
} modem_mode_t;

typedef enum {
	ARM_OPP30,
	ARM_OPP50,
    ARM_OPP100,
    ARM_OPP125
} arm_opp_t;

typedef enum {
	APE_OPP50,
    APE_OPP100
} ape_opp_t;

typedef enum {
	MODEM_OPP50,
    MODEM_OPP100
} modem_opp_t;

typedef enum {
	DDR_OPP25,
	DDR_OPP50,
    DDR_OPP100
} ddr_opp_t;

typedef enum {
	SW_APE_RESET,
	SW_ARM_RESET,
	WD_ARM_RESET,
	AB8500_WD_RESET
} ap_reset_t;

typedef enum {
	MODEM_SW_RESET,
	MODEM_WD_RESET
} modem_reset_t;

typedef enum {
	INIT_VAL,
    ERR_FILE_ERROR,
    ERR_UNKNOWN_MODE,
    ERR_UNKNOWN_OPP,
    ERR_UNKNOWN_RESSOURCE,

	AP_NORMAL_OK,
    ERR_AP_NORMAL_KO,
    AP_EXECUTE_OK,
    ERR_AP_EXECUTE_KO,
    AP_RUNNING_OK,
    ERR_AP_RUNNING_KO,
    AP_IDLE_OK,
    ERR_AP_IDLE_KO,
    AP_SLEEP_OK,
    ERR_AP_SLEEP_KO,
#ifdef PWS_9540
    AP_DEEP_IDLE_OK,
    ERR_AP_DEEP_IDLE_KO,
    AP_DEEP_SLEEP_OK,
    ERR_AP_DEEP_SLEEP_KO,
#else
    AP_DEEPSLEEP_ESRAM0_RET_OK,
    ERR_AP_DEEPSLEEP_ESRAM0_RET_KO,
    AP_DEEPSLEEP_ESRAM0_OFF_OK,
    ERR_AP_DEEPSLEEP_ESRAM0_OFF_KO,
#endif
    MODEM_LP_DISABLED_OK,
    ERR_MODEM_LP_DISABLED_KO,
    MODEM_LP_ENABLED_OK,
    ERR_MODEM_LP_ENABLED_KO,

    HWACC_MODE_OK,
    ERR_HWACC_MODE_KO,

	ARM_OPP_OK,
    ERR_ARM_OPP_KO,
    APE_OPP_OK,
    ERR_APE_OPP_KO,
    DDR_OPP_OK,
    ERR_DDR_OPP_KO,
    MODEM_OPP_OK,
    ERR_MODEM_OPP_KO,
#ifdef PWS_9540
    ARM_FREQ_OK,
    ERR_ARM_FREQ_KO,
#endif
    MODEM_RESET_OK,
    ERR_MODEM_RESET_KO,
    AP_RESET_OK,
	ERR_AP_RESET_KO
} status_t;

typedef enum {
#ifdef PWS_9540
#ifdef PWS_8540
	HVAG1,
	SGX,
	SIA,
	DSS,
	HVAG1MEM,
	SGXMEM,
	SIAMEM,
	SIAMMDSPMEM,
	DSSMEM,
	USBPKASTMMEM,
	ESRAM10,
	ESRAM2,
	ESRAM3,
	ESRAM45,
#endif
	SIAMMDSP,
	SIAPIPE,
	SVAMMDSP,
	SVAPIPE,
	SGA,
	ESRAM12,
	ESRAM34,
	B2R2MCDE,
	HWACCMAX
#else
	SIAMMDSP,
	SIAPIPE,
	SVAMMDSP,
	SVAPIPE,
	SGA,
	ESRAM1,
	ESRAM2,
	ESRAM3,
	ESRAM4,
	B2R2MCDE
#endif
} hwacc_type_t;

#ifdef PWS_9540
static const char *hwacc_name[HWACCMAX] = {
#ifdef PWS_8540
	[HVAG1]		= "dbx540-hva-g1",
	[SGX]		= "dbx540-sgx",
	[SIA]		= "dbx540-sia",
	[DSS]		= "dbx540-dss",
	[HVAG1MEM]	= "dbx540-hva-g1-mem",
	[SGXMEM]	= "dbx540-sgx-mem",
	[SIAMEM]	= "dbx540-sia-mem",
	[SIAMMDSPMEM]	= "dbx540-sia-mmdsp-mem",
	[DSSMEM]	= "dbx540-dss-mem",
	[USBPKASTMMEM]	= "dbx540-usb-pka-stm-mem",
	[ESRAM10]	= "dbx540-eram10",
	[ESRAM2]	= "dbx540-eram2",
	[ESRAM3]	= "dbx540-eram3",
	[ESRAM45]	= "dbx540-eram45",
#endif
	[SVAMMDSP]	= "db8500-sva-mmdsp",
	[SVAPIPE]	= "db8500-sva-pipe",
	[SIAMMDSP]	= "db8500-sia-mmdsp",
	[SIAPIPE]	= "db8500-sia-pipe",
	[SGA]		= "db8500-sga",
	[B2R2MCDE]	= "db8500-b2r2-mcde",
	[ESRAM12]	= "db8500-esram12",
	[ESRAM34]	= "db8500-esram34"
};
#endif

#ifdef PWS_9540
static const char *hwacc_ret_name[HWACCMAX] = {
#ifdef PWS_8540
	[ESRAM10]	= "dbx540-eram10-ret",
	[ESRAM2]	= "dbx540-eram2-ret",
	[ESRAM3]	= "dbx540-eram3-ret",
	[ESRAM45]	= "dbx540-eram45-ret",
#endif
	[SVAMMDSP]	= "db8500-sva-mmdsp-ret",
	[SIAMMDSP]	= "db8500-sia-mmdsp-ret",
	[ESRAM12]	= "db8500-esram12-ret",
	[ESRAM34]	= "db8500-esram34-ret"
};
#endif

typedef enum
{
	AP_EXIT_BY_GPIO,
	AP_EXIT_BY_TIMER
} ap_exit_t;

typedef enum
{
	AP_CPU1_TO_ON,
	AP_CPU1_TO_OFF
} ap_hot_plug_t;

#ifdef PWS_9540
status_t pwm_set_ap_mode(ap_mode_t vp_mode, ap_exit_t vp_exit_type, ap_ulpclk_t vp_ulpclk);
status_t pwm_set_ddr_start(ap_ddr_mode_t __attribute__ ((unused)) vp_ddr_mode, ap_ddr_ctrl_t __attribute__ ((unused)) vp_ddr_ctrl,ap_power_state_t __attribute__ ((unused)) vp_power_state);
#else
status_t pwm_set_ap_mode(ap_mode_t vp_mode, ap_exit_t vp_exit_type);
#endif
status_t pwm_set_hwacc_mode(hwacc_type_t hwacc_name, hwacc_mode_t vp_mode);
status_t pwm_set_modem_mode(modem_mode_t vp_mode);
status_t pwm_set_ape_opp(ape_opp_t vp_mode);
#ifdef PWS_9540
status_t pwm_get_number_of_arm_freq(uint32_t *p_number);
status_t pwm_get_arm_freq_from_opp(arm_opp_t v_opp, uint32_t *p_freq);
status_t pwm_set_cpu1_mode(ap_hot_plug_t vp_hot_plug);
#endif
status_t pwm_set_arm_opp(arm_opp_t vp_mode);
status_t pwm_set_ddr_opp(ddr_opp_t vp_mode);
int32_t pwm_set_modem_opp(modem_opp_t opp);
status_t pwm_reset_modem(modem_reset_t vp_reset);
status_t pwm_reset_ap(ap_reset_t vp_reset);

#endif /* _HATS_POWER_H */
