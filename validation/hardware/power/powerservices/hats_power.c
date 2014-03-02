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

#include "hats_power.h"
#ifndef PWS_9540
#include <Modem_Lib.h>
#include <isi_driver_lib.h>
#else
#include <signal.h>
#include <time.h>
#include "modemmngt.h"
#endif
#include <unistd.h>
#include "libab.h"
#include "libdb.h"

#ifndef PWS_9540
int modem_dvfs_req_done, modem_sleep_req_done, modem_reset_req_done;

int32_t status_modem_lib_init_all = MLIB_R_GENERAL_ERROR;

static void modem_cb(int32_t mlib_request_id, void __attribute__ ((unused)) *answer, void __attribute__ ((unused)) *userdata)
{
	switch (mlib_request_id) {
	case MLIB_SOL_MTEST_SET_DVFS_MODE:
		modem_dvfs_req_done = 1;
		break;
	case MLIB_SOL_MTEST_SET_SLEEP_MODE:
		modem_sleep_req_done = 1;
		break;
	case MLIB_SOL_MCE_MODEM_RESET:
		modem_reset_req_done = 1;
		break;
	}
}

static void __attribute__ ((unused)) modem_cb_unsollicited(int32_t __attribute__ ((unused)) mlib_unsol_event_id, void __attribute__ ((unused)) *data)
{
}
#endif
#ifdef PWS_9540

status_t pwm_set_cpu1_mode(ap_hot_plug_t __attribute__ ((unused)) vp_hot_plug)
{
	FILE *hot_plug_fp;
	status_t ap_status = INIT_VAL;

	hot_plug_fp=fopen(
			"/sys/devices/system/cpu/cpu1/online","r+");

	if (hot_plug_fp == NULL) {
		printf("File Error hot_plug\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}
	switch( vp_hot_plug ){
		case AP_CPU1_TO_ON:
			fprintf(hot_plug_fp, "1\n");
			fflush(hot_plug_fp);
			break;

		case AP_CPU1_TO_OFF:
			fprintf(hot_plug_fp, "0\n");
			fflush(hot_plug_fp);
			break;
		default:
			ap_status = ERR_UNKNOWN_MODE;
			goto error;

	}
	error:
		fclose(hot_plug_fp);

	return ap_status;
}
#endif
#ifdef PWS_9540

status_t pwm_set_ddr_start(ap_ddr_mode_t __attribute__ ((unused)) vp_ddr_mode, ap_ddr_ctrl_t __attribute__ ((unused)) vp_ddr_ctrl,ap_power_state_t __attribute__ ((unused)) vp_power_state)
{
	FILE *power_state_fp,*ddr_mode_fp,*ddr_ctrl_fp,*ddr_strategy_fp;
	status_t ap_status = INIT_VAL;

	power_state_fp = fopen(
			"/sys/kernel/debug/deep_debug/ddrsleepstrat/ap_state", "r+");

	ddr_mode_fp = fopen(
			"/sys/kernel/debug/deep_debug/ddrsleepstrat/ddr_mode", "r+");

	ddr_ctrl_fp = fopen(
			"/sys/kernel/debug/deep_debug/ddrsleepstrat/ddr_ctrl", "r+");

	ddr_strategy_fp = fopen(
			"/sys/kernel/debug/deep_debug/ddrsleepstrat/ddr_strategy", "r+");

	if (power_state_fp == NULL) {
		printf("File Error /sys/kernel/debug/deep_debug/ddrsleepstrat/ap_state\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}

	if (ddr_mode_fp == NULL) {
		printf("File Error /sys/kernel/debug/deep_debug/ddrsleepstrat/ddr_mode\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}

	if (ddr_ctrl_fp == NULL) {
		printf("File Error /sys/kernel/debug/deep_debug/ddrsleepstrat/ddr_ctrl\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}

	if (ddr_strategy_fp == NULL) {
		printf("File Error /sys/kernel/debug/deep_debug/ddrsleepstrat/ddr_strategy\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}

	switch (vp_power_state)
	{
		case DDR_IDLE:
			fprintf(power_state_fp,"idle\n");
			fflush(power_state_fp);
			break;

		case DDR_DEEPIDLE:
			fprintf(power_state_fp,"deepidle\n");
			fflush(power_state_fp);
			break;

		case DDR_SLEEP:
			fprintf(power_state_fp,"sleep\n");
			fflush(power_state_fp);
			break;

		default:
			ap_status = ERR_UNKNOWN_MODE;
			goto error;
	}

	switch (vp_ddr_ctrl)
	{
		case DDR_CTRL0:
			fprintf(ddr_ctrl_fp,"0\n");
			fflush(ddr_ctrl_fp);
			break;

		case DDR_CTRL1:
			fprintf(ddr_ctrl_fp,"1\n");
			fflush(ddr_ctrl_fp);
			break;

		default:
			ap_status = ERR_UNKNOWN_MODE;
			goto error;
	}
	
	switch (vp_ddr_mode)
	{
		case DDR_OFFHIGHLAT:
			fprintf(ddr_mode_fp,"offhighlat\n");
			fflush(ddr_mode_fp);
			break;

		case DDR_OFFLOWLAT:
			fprintf(ddr_mode_fp,"offlowlat\n");
			fflush(ddr_mode_fp);
			break;

		case DDR_ON:
			fprintf(ddr_mode_fp,"on\n");
			fflush(ddr_mode_fp);
			break;

		default:
			ap_status = ERR_UNKNOWN_MODE;
			goto error;
	}

	fprintf(ddr_strategy_fp,"set\n");
	fflush(ddr_strategy_fp);

error:
	fclose(ddr_strategy_fp);
	fclose(ddr_ctrl_fp);
	fclose(ddr_mode_fp);
	fclose(power_state_fp);
	return ap_status;
}
status_t pwm_set_ap_mode(ap_mode_t __attribute__ ((unused)) vp_mode, ap_exit_t __attribute__ ((unused)) vp_exit_type,ap_ulpclk_t __attribute__ ((unused)) vp_ulpclk)
{

	FILE *deepest_state_fp, *ape_to_suspend_fp, *test_ddr_fp, *gpio_value, *ulpclk_to_suspend_fp, *suspend_test_fp;
	status_t ap_status = INIT_VAL;

	gpio_value = fopen(
			"/sys/class/gpio/gpio129/value", "r+");

	deepest_state_fp = fopen(
			"/sys/kernel/debug/cpuidle/deepest_state", "r+");

	ape_to_suspend_fp = fopen(
			"/sys/kernel/debug/suspend/ape_to_suspend", "r+");

	test_ddr_fp =  fopen(
			"/sys/kernel/debug/deep_debug/ddr/memtest", "r+");

	ulpclk_to_suspend_fp= fopen(
			"/sys/kernel/debug/suspend/ulpclk_to_suspend","r+");

	suspend_test_fp = fopen(
			"/sys/kernel/debug/pwr_test/suspend","r+");

	if (deepest_state_fp == NULL) {
		printf("File Error deepest_state\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}

	if (ape_to_suspend_fp == NULL) {
		printf("File Error ape_to_suspend\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}

	if (test_ddr_fp == NULL) {
		printf("File Error test_ddr\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}

	if (ulpclk_to_suspend_fp == NULL) {
		printf("File Error ulpclk_to_suspend\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}

	if (suspend_test_fp == NULL) {
		printf("File Error suspend_test\n");
		ap_status = ERR_FILE_ERROR;
		goto error;
	}

	if (gpio_value== NULL) {
		FILE* gpio_export,*gpio_direction;
		printf("File Error gpio_value\n");
		gpio_export = fopen("/sys/class/gpio/export","w");
		fprintf(gpio_export, "129\n");
		fflush(gpio_export);
		gpio_direction = fopen("/sys/class/gpio/gpio129/direction","r+");
		fprintf(gpio_direction, "out\n");
		fflush(gpio_direction);
		gpio_value = fopen("/sys/class/gpio/gpio129/value", "r+");
		fclose(gpio_export);
		fclose(gpio_direction);
        }

	switch (vp_ulpclk)
	{
		case AP_ULP_TO_OFF:
			fprintf(ulpclk_to_suspend_fp,"0\n");
			fflush(ulpclk_to_suspend_fp);
			break;

		case AP_ULP_TO_ON:
			fprintf(ulpclk_to_suspend_fp,"1\n");
			fflush(ulpclk_to_suspend_fp);
			break;
	}

	switch( vp_mode )
	{
		case AP_RUNNING:
			fprintf(gpio_value,"0\n");
			fflush(gpio_value);
			/* State 0: AP Running */
			fprintf(deepest_state_fp, "0\n");
			fflush(deepest_state_fp);

			/* Set ape_to_suspend for default */
			fprintf(ape_to_suspend_fp, "1\n");
			fflush(ape_to_suspend_fp);
			popen("/usr/bin/dry2o_30s &","w");
			popen("/usr/bin/dry2o_30s &","w");
			fprintf(test_ddr_fp, "stop\n");
			fflush(test_ddr_fp);
			fprintf(gpio_value,"1\n");
			fflush(gpio_value);
			/* force ulp to ON */
			fprintf(ulpclk_to_suspend_fp,"1\n");	
			fflush(ulpclk_to_suspend_fp);
			/* Cancel suspend test */
			fprintf(suspend_test_fp, "0\n");
			fflush(suspend_test_fp);
			ap_status = AP_RUNNING_OK;
			break;

		case AP_NORMAL:
			fprintf(gpio_value,"0\n");
			fflush(gpio_value);
			/* State 1: WFI */
			fprintf(deepest_state_fp, "1\n");
			fflush(deepest_state_fp);
			/* Set ape_to_suspend for default */
			fprintf(ape_to_suspend_fp, "1\n");
			fflush(ape_to_suspend_fp);
			/* Cancel AP running work */
			fprintf(test_ddr_fp, "stop\n");
			fflush(test_ddr_fp);
			fprintf(gpio_value,"1\n");
			fflush(gpio_value);
			/* force ulp to ON */
			fprintf(ulpclk_to_suspend_fp,"1\n");	
			fflush(ulpclk_to_suspend_fp);
			/* Cancel suspend test */
			fprintf(suspend_test_fp, "0\n");
			fflush(suspend_test_fp);
			ap_status = AP_NORMAL_OK;
			break;

		case AP_SLEEP:
			fprintf(gpio_value, "0\n");
			fflush(gpio_value);
			/* State 3: AP sleep, State 4: AP sleep with UL PLL off */
			if (vp_ulpclk)
				fprintf(deepest_state_fp, "3\n");
			else
				fprintf(deepest_state_fp, "4\n");
			fflush(deepest_state_fp);
			/* Set for suspend mode */
			fprintf(ape_to_suspend_fp, "1\n");
			fflush(ape_to_suspend_fp);
			/* Cancel AP running work */
			fprintf(test_ddr_fp, "stop\n");
			fflush(test_ddr_fp);
			fprintf(gpio_value, "1\n");
			fflush(gpio_value);
			/* Enter suspend test */
			fprintf(suspend_test_fp, "1\n");
			fflush(suspend_test_fp);
			printf("Please remove USB and UART cable for measuring current\n");
			ap_status = AP_SLEEP_OK;
			break;

		case AP_DEEP_IDLE:
			/* Unsupported mode, CONFIG_UX500_CPUIDLE_APDEEPIDLE is not defined in kernel */
			printf("Unsupported mode in kernel\n");
			printf("This mode will be replaced by AP_IDLE\n");

		case AP_IDLE:
			fprintf(gpio_value, "0\n");
			fflush(gpio_value);
			/* State 2: AP idle */
			fprintf(deepest_state_fp, "2\n");
			fflush(deepest_state_fp);
			/* Set for idle mode */
			fprintf(ape_to_suspend_fp, "0\n");
			fflush(ape_to_suspend_fp);
			/* Cancel AP running work */
			fprintf(test_ddr_fp, "stop\n");
			fflush(test_ddr_fp);
			fprintf(gpio_value, "1\n");
			fflush(gpio_value);
			/* force ulp to ON */
			fprintf(ulpclk_to_suspend_fp,"1\n");	
			fflush(ulpclk_to_suspend_fp);
			/* Enter suspend test */
			fprintf(suspend_test_fp, "1\n");
			fflush(suspend_test_fp);
			printf("Please remove USB and UART cable for measuring current\n");
			ap_status = AP_IDLE_OK;
			break;

		case AP_DEEP_SLEEP:
			fprintf(gpio_value, "0\n");
			fflush(gpio_value);
			/* State 5: AP deepsleep */
			fprintf(deepest_state_fp, "5\n");
			fflush(deepest_state_fp);
			/* Set for suspend mode */
			fprintf(ape_to_suspend_fp, "1\n");
			fflush(ape_to_suspend_fp);
			/* Cancel AP running work */
			fprintf(test_ddr_fp, "stop\n");
			fflush(test_ddr_fp);
			fprintf(gpio_value, "1\n");
			fflush(gpio_value);
			/* force ulp to OFF */
			fprintf(ulpclk_to_suspend_fp, "0\n");
			fflush(ulpclk_to_suspend_fp);
			/* Enter suspend test */
			fprintf(suspend_test_fp, "1\n");
			fflush(suspend_test_fp);
			printf("Please remove USB and UART cable for measuring current\n");
			ap_status = AP_DEEP_SLEEP_OK;
			break;

		default:
			ap_status = ERR_UNKNOWN_MODE;
			goto error;

	}
	error:
		fclose(deepest_state_fp);
		fclose(ape_to_suspend_fp);
		fclose(test_ddr_fp);
		fclose(gpio_value);
		fclose(ulpclk_to_suspend_fp);
		fclose(suspend_test_fp);
	return ap_status;

}
#else
status_t pwm_set_ap_mode(ap_mode_t __attribute__ ((unused)) vp_mode, ap_exit_t __attribute__ ((unused)) vp_exit_type)
{
	return INIT_VAL;
}
#endif

status_t pwm_set_hwacc_mode(hwacc_type_t __attribute__ ((unused)) acc_name, hwacc_mode_t __attribute__ ((unused)) vp_mode)
{
#ifdef PWS_9540
	status_t hwacc_status = HWACC_MODE_OK;
	FILE *hwacc_name_fp, *hwacc_state_fp;

	int hwacc_ret_state, hwacc_state;
	char hwacc_name_read[50];
	char hwacc_state_read[50];
	char hwacc_dummy[50];

	hwacc_name_fp = fopen(
			"/sys/kernel/debug/ux500-regulator/reg_name", "r+");
	hwacc_state_fp = fopen(
			"/sys/kernel/debug/ux500-regulator/reg_state", "r+");

	if (hwacc_name_fp == NULL) {
		printf("File Error reg_name\n");
		hwacc_status = ERR_FILE_ERROR;
		goto error;
	}

	if (hwacc_state_fp == NULL) {
		printf("File Error reg_state\n");
		hwacc_status = ERR_FILE_ERROR;
		goto error;
	}

	if(acc_name >= HWACCMAX || hwacc_name[acc_name] == NULL)
	{
		printf("Hw acc ressource not found\n");
		hwacc_status = ERR_UNKNOWN_RESSOURCE;
		goto error;
	}

	if(vp_mode == HWACC_RETENTION && hwacc_ret_name[acc_name] == NULL)
	{
		printf("Hw acc mode mismatch\n");
		hwacc_status = ERR_UNKNOWN_MODE;
		goto error;
	}

	switch(vp_mode)
	{
		case HWACC_OFF:
			hwacc_ret_state = 0;
			hwacc_state = 0;
			break;
		case HWACC_ON:
			hwacc_ret_state = 1;
			hwacc_state = 1;
			break;
		case HWACC_RETENTION:
			hwacc_ret_state = 1;
			hwacc_state = 0;
			break;
		default:
			printf("Hw acc mode not found\n");
			hwacc_status = ERR_UNKNOWN_MODE;
			goto error;
	}
	//printf("-%s-\n",hwacc_name[acc_name]);

	fprintf(hwacc_name_fp, "%s\n", hwacc_name[acc_name]);
	fflush(hwacc_name_fp);
	//printf("-%d-\n",hwacc_state);
	fprintf(hwacc_state_fp, "%d\n", hwacc_state);
	fflush(hwacc_state_fp);

	sleep(1);

	rewind(hwacc_name_fp);
	rewind(hwacc_state_fp);

	fscanf(hwacc_name_fp, "%s", hwacc_name_read);
	fflush(hwacc_name_fp);
	//printf("read-%s-\n",hwacc_name_read);
	fscanf(hwacc_state_fp, "%s : %s",hwacc_dummy, hwacc_state_read);
	fflush(hwacc_state_fp);
	//printf("read-%s-\n",hwacc_state_read);
	if( strncmp(hwacc_name_read, hwacc_name[acc_name], strlen(hwacc_name[acc_name])) != 0)
	{
		printf("Verification ko : %s with state %s\n", hwacc_name_read, hwacc_state_read);
		hwacc_status = ERR_HWACC_MODE_KO;
		goto error;
	}

	if((hwacc_state==1 &&  strncmp(hwacc_state_read, "enabled", strlen(hwacc_state_read)) != 0) || (hwacc_state==0 &&  strncmp(hwacc_state_read, "disabled", strlen(hwacc_state_read)) != 0))
	{
		printf ("erreur state_read %s hwacc_state %d",hwacc_state_read,hwacc_state);
		hwacc_status = ERR_HWACC_MODE_KO;
		goto error;
	}

	if(hwacc_ret_name[acc_name] != NULL)
	{
		rewind(hwacc_name_fp);
		rewind(hwacc_state_fp);
		//printf("-%s-\n",hwacc_ret_name[acc_name]);
		fprintf(hwacc_name_fp, "%s\n", hwacc_ret_name[acc_name]);
		fflush(hwacc_name_fp);
		//printf("-%d-\n",hwacc_ret_state);
		fprintf(hwacc_state_fp, "%d\n", hwacc_ret_state);
		fflush(hwacc_state_fp);

		sleep(1);

		rewind(hwacc_name_fp);
		rewind(hwacc_state_fp);

		fscanf(hwacc_name_fp, "%s", hwacc_name_read);
		fflush(hwacc_name_fp);

		fscanf(hwacc_state_fp, "%s : %s",hwacc_dummy, hwacc_state_read);
		//fscanf(hwacc_state_fp, "%d", &hwacc_state_read);
		fflush(hwacc_state_fp);

		if( (strncmp(hwacc_name_read, hwacc_ret_name[acc_name], strlen(hwacc_ret_name[acc_name])) != 0))
		{
			printf("Verification ret ko : %s with state %s\n", hwacc_name_read, hwacc_state_read);
			hwacc_status = ERR_HWACC_MODE_KO;
			goto error;
		}

		//printf("read ret stat %s .... %d\n",hwacc_state_read,hwacc_ret_state);
		if((hwacc_ret_state==1 &&  strncmp(hwacc_state_read, "enabled", strlen(hwacc_state_read)) != 0) || (hwacc_ret_state==0 &&  strncmp(hwacc_state_read, "disabled", strlen(hwacc_state_read)) != 0))
		{
			printf ("erreur hwacc_ret_stat %s hwacc_ret_state %d",hwacc_state_read,hwacc_ret_state);
			hwacc_status = ERR_HWACC_MODE_KO;
			goto error;
		}


	}

	error:
		fclose(hwacc_name_fp);
		fclose(hwacc_state_fp);

	return hwacc_status;
#else
	return INIT_VAL;
#endif
}

#ifdef PWS_9540
status_t pwm_get_number_of_arm_freq(uint32_t *p_number)
{
	status_t arm_freq_status = ARM_FREQ_OK;
	char charac;
	uint32_t number;
	FILE *scaling_available_frequencies_fp;

	scaling_available_frequencies_fp = fopen(
		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies", "r");

	if (scaling_available_frequencies_fp == NULL) {
		printf("File Error scaling_available_frequencies\n");
		arm_freq_status = ERR_FILE_ERROR;
		goto error;
	}

	number = 0;
	while (!feof(scaling_available_frequencies_fp))
	{
		fscanf(scaling_available_frequencies_fp, "%c", &charac);
		if (charac == ' ')
		number++;
	}

	*p_number = number;
	if( *p_number == 0)
	{
		printf("Bad frequencies number\n");
		arm_freq_status = ERR_ARM_FREQ_KO;
		goto error;
	}

	fflush(scaling_available_frequencies_fp);

	error:
		fclose(scaling_available_frequencies_fp);

	return arm_freq_status;
}

status_t pwm_get_arm_freq_from_opp(arm_opp_t v_opp, uint32_t *p_freq)
{
	status_t arm_freq_status = ARM_FREQ_OK;
	uint32_t count, arm_freq, max_freq;
	FILE *scaling_available_frequencies_fp;

	scaling_available_frequencies_fp = fopen(
		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies", "r");

	if (scaling_available_frequencies_fp == NULL) {
		printf("File Error scaling_available_frequencies\n");
		arm_freq_status = ERR_FILE_ERROR;
		goto error;
	}

	pwm_get_number_of_arm_freq(&max_freq);

	if( v_opp >= max_freq )
	{
		printf("Bad Opp index\n");
		arm_freq_status = ERR_ARM_FREQ_KO;
		goto error;
	}

	count = v_opp;
	fscanf(scaling_available_frequencies_fp, "%d", &arm_freq);

	while (count)
	{
		fscanf(scaling_available_frequencies_fp, "%d", &arm_freq);
		count--;
	}

	*p_freq = arm_freq;
	fflush(scaling_available_frequencies_fp);

	error:
		fclose(scaling_available_frequencies_fp);

	return arm_freq_status;

}
#endif

status_t pwm_set_arm_opp(arm_opp_t vp_opp)
{
	status_t arm_opp_status = INIT_VAL;
	uint32_t previous_arm_freq, updated_arm_freq, requested_arm_freq=0;
#ifdef PWS_9540
	uint32_t arm_freq;
#endif
	char buff[20];

	FILE *scaling_setspeed_fp, *scaling_cur_freq_fp, *scaling_governor_fp,
		*scaling_max_freq_fp, *scaling_min_freq_fp;

	scaling_governor_fp = fopen(
		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "r+");
	scaling_setspeed_fp = fopen(
		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed", "r+");
	scaling_cur_freq_fp = fopen(
		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
	scaling_min_freq_fp = fopen(
		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq", "r+");
	scaling_max_freq_fp = fopen(
		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r+");

	if (scaling_setspeed_fp == NULL) {
		printf("File Error scaling_setspeed\n");
		arm_opp_status = ERR_FILE_ERROR;
		goto error;
	}

	if (scaling_cur_freq_fp == NULL) {
		printf("File Error scaling_cur_freq\n");
		arm_opp_status = ERR_FILE_ERROR;
		goto error;
	}

	if (scaling_min_freq_fp == NULL) {
		printf("File Error scaling_min_freq\n");
		arm_opp_status = ERR_FILE_ERROR;
		goto error;
	}

	if (scaling_max_freq_fp == NULL) {
		printf("File Error scaling_max_freq\n");
		arm_opp_status = ERR_FILE_ERROR;
		goto error;
	}

	if (scaling_governor_fp == NULL) {
		printf("File Error scaling_governor\n");
		arm_opp_status = ERR_FILE_ERROR;
		goto error;
	}

#ifdef PWS_9540
	if( pwm_get_arm_freq_from_opp(vp_opp, &arm_freq) != ARM_FREQ_OK )
	{
		printf("Arm opp to freq conversion error\n");
		arm_opp_status = ERR_ARM_FREQ_KO;
		goto error;
	}
#endif

	/* Save current opp */
	fscanf(scaling_cur_freq_fp, "%d\n", &previous_arm_freq);
	fflush(scaling_cur_freq_fp);

	/* check governor */
	fscanf(scaling_governor_fp, "%s\n", buff);
	fflush(scaling_governor_fp);

    if (strcmp(buff, "ondemand") == 0)
    {
#ifdef PWS_9540
		requested_arm_freq = arm_freq;
		fprintf(scaling_max_freq_fp, "%d\n", arm_freq);
		fprintf(scaling_min_freq_fp, "%d\n", arm_freq);

		fflush(scaling_max_freq_fp);
		fflush(scaling_min_freq_fp);
#else
		switch (vp_opp) {
		case ARM_OPP30:
			fprintf(scaling_max_freq_fp, "200000\n");
			fprintf(scaling_min_freq_fp, "200000\n");
            requested_arm_freq = 200000;
		break;

		case ARM_OPP50:
			fprintf(scaling_max_freq_fp, "400000\n");
			fprintf(scaling_min_freq_fp, "400000\n");
            requested_arm_freq = 400000;
			break;

		case ARM_OPP100:
			fprintf(scaling_max_freq_fp, "800000\n");
			fprintf(scaling_min_freq_fp, "800000\n");
            requested_arm_freq = 800000;
			break;

		case ARM_OPP125:
			fprintf(scaling_max_freq_fp, "1000000\n");
			fprintf(scaling_min_freq_fp, "1000000\n");
            requested_arm_freq = 1000000;
			break;

		default:
			arm_opp_status = ERR_UNKNOWN_MODE;
			goto error;
		}

		fflush(scaling_max_freq_fp);
		fflush(scaling_min_freq_fp);
#endif
    }
    else if (strcmp(buff, "userspace") == 0)
    {
#ifdef PWS_9540
		requested_arm_freq = arm_freq;
		fprintf(scaling_setspeed_fp, "%d\n", arm_freq);

		fflush(scaling_setspeed_fp);
#else
		switch (vp_opp) {
		case ARM_OPP30:
			fprintf(scaling_setspeed_fp, "200000\n");
            requested_arm_freq = 200000;
		break;

		case ARM_OPP50:
			fprintf(scaling_setspeed_fp, "400000\n");
            requested_arm_freq = 400000;
			break;

		case ARM_OPP100:
			fprintf(scaling_setspeed_fp, "800000\n");
            requested_arm_freq = 800000;
			break;

		case ARM_OPP125:
			fprintf(scaling_setspeed_fp, "1000000\n");
            requested_arm_freq = 1000000;
			break;

		default:
			arm_opp_status = ERR_UNKNOWN_MODE;
			goto error;
		}

		fflush(scaling_setspeed_fp);
#endif
    }

	sleep(1);

	/* Read back current frequency to
	 * check if changes are done
	 */
	rewind(scaling_cur_freq_fp);
	fscanf(scaling_cur_freq_fp, "%u\n", &updated_arm_freq);

	printf("\nHATS Power Info : ARM Opp\n");
	printf("previous_arm_freq = %u\n", previous_arm_freq);
	printf("requested_arm_freq = %u\n", requested_arm_freq);
	printf("updated_arm_freq = %u\n", updated_arm_freq);

	if (updated_arm_freq != requested_arm_freq)
		arm_opp_status = ERR_ARM_OPP_KO;
	else
		arm_opp_status = ARM_OPP_OK;

error:
	fclose(scaling_setspeed_fp);
	fclose(scaling_cur_freq_fp);
	fclose(scaling_governor_fp);
	fclose(scaling_min_freq_fp);
	fclose(scaling_max_freq_fp);

	return arm_opp_status;
}

status_t pwm_set_ape_opp(ape_opp_t vp_opp)
{
	status_t ape_status = INIT_VAL;
	FILE *ape_opp_fp;
    int cur_ape_opp, requested_ape_opp, updated_ape_opp, unused;

	ape_opp_fp = fopen("/sys/kernel/debug/prcmu/ape_opp", "r+");

	if (ape_opp_fp == NULL) {
		ape_status = ERR_FILE_ERROR;
		goto error;
	}

	printf("\nHATS Power Info : APE Opp\n");

	/*
	 * Read current ape_opp
	 */
	fscanf(ape_opp_fp, "%d%% (%d)\n", &cur_ape_opp, &unused);
	fflush(ape_opp_fp);

	switch (vp_opp) {
	case APE_OPP50:
		fprintf(ape_opp_fp, "0\n");
		requested_ape_opp = 50;
        break;

	case APE_OPP100:
		fprintf(ape_opp_fp, "1\n");
		requested_ape_opp = 100;
		break;

	default:
		ape_status = ERR_UNKNOWN_MODE;
		goto error;
	}


	fflush(ape_opp_fp);

	sleep(1);

	printf("cur_ape_opp = %d\n", cur_ape_opp);
	//printf("requested_ape_opp = %d\n", requested_ape_opp);

	/*
	 * Read back ape_opp to check
	 */
	rewind(ape_opp_fp);
	fscanf(ape_opp_fp, "%d%% (%d)\n", &updated_ape_opp, &unused);
	fflush(ape_opp_fp);

	//printf("real_ape_opp = %d\n", updated_ape_opp);

	if (updated_ape_opp == 100)
		ape_status = (vp_opp == APE_OPP100) ? APE_OPP_OK:APE_OPP_OK;
	else
		ape_status = (vp_opp == APE_OPP50) ? APE_OPP_OK:APE_OPP_OK;

error:
	fclose(ape_opp_fp);

	return ape_status;
}

status_t pwm_set_ddr_opp(ddr_opp_t vp_opp)
{
	status_t ddr_status = INIT_VAL;
	FILE *ddr_opp_fp;
    int cur_ddr_opp, requested_ddr_opp, updated_ddr_opp, unused;

	ddr_opp_fp = fopen("/sys/kernel/debug/prcmu/ddr_opp", "r+");

	if (ddr_opp_fp == NULL) {
		ddr_status = ERR_FILE_ERROR;
		goto error;
	}

	printf("\nHATS Power Info : DDR Opp\n");

	/*
	 * Read current ddr_opp value
	 */
	fscanf(ddr_opp_fp, "%d%% (%d)\n", &cur_ddr_opp, &unused);
	fflush(ddr_opp_fp);

	switch (vp_opp) {
	case DDR_OPP25:
		fprintf(ddr_opp_fp, "25\n");
        requested_ddr_opp = 25;
		break;

	case DDR_OPP50:
		fprintf(ddr_opp_fp, "50\n");
        requested_ddr_opp = 50;
		break;

	case DDR_OPP100:
		fprintf(ddr_opp_fp, "100\n");
        requested_ddr_opp = 100;
		break;

	default:
		ddr_status = ERR_UNKNOWN_MODE;
		goto error;
	}

	fflush(ddr_opp_fp);

	/* Sleep thread to ensure operation has been done */
	sleep(1);

	printf("cur_ddr_opp = %d\n", cur_ddr_opp);
	printf("requested_ddr_opp = %d\n", requested_ddr_opp);

	/*
	 * Read back ddr_opp value to check
	 */
	rewind(ddr_opp_fp);
	fscanf(ddr_opp_fp, "%d%% (%d)\n", &updated_ddr_opp, &unused);
	fflush(ddr_opp_fp);

	printf("updated_ddr_opp = %d\n", updated_ddr_opp);

	if (updated_ddr_opp == 100)
		ddr_status = (vp_opp == DDR_OPP100) ? DDR_OPP_OK:ERR_DDR_OPP_KO;
	else if (updated_ddr_opp == 50)
		ddr_status = (vp_opp == DDR_OPP50) ? DDR_OPP_OK:ERR_DDR_OPP_KO;
	else
		ddr_status = (vp_opp == DDR_OPP25) ? DDR_OPP_OK:ERR_DDR_OPP_KO;

error:
	fclose(ddr_opp_fp);

	return ddr_status;
}

status_t pwm_set_modem_mode(modem_mode_t vp_mode)
{
#ifndef PWS_9540
	status_t modem_mode_status = INIT_VAL;
	t_MLIB_MT_testControl testControl;
	int32_t status_modem_lib_request = MLIB_R_NOT_INITIALIZED;
	int32_t status_modem_lib_init_all;
	int32_t status_isi_driver_read;

	uint8_t  vl_MsgTab[ISI_DRIVER_MAX_MESSAGE_LENGTH];
	int32_t  vl_RecMsgleng  = 0;

	isi_driver_init();
	status_modem_lib_init_all =
		modem_lib_init_all(modem_cb, modem_cb_unsollicited);

	if (status_modem_lib_init_all != MLIB_R_SUCCESS) {
		modem_mode_status = (vp_mode == MODEM_LP_DISABLED) ?
			ERR_MODEM_LP_DISABLED_KO : ERR_MODEM_LP_ENABLED_KO;
		printf("ERROR status_modem_lib_init_all "
			"= %d\n", status_modem_lib_init_all);
	} else {
		switch (vp_mode) {
		case MODEM_LP_DISABLED:
			testControl.CPU_ID = MLIB_MT_CPU_ALL;
			testControl.forced_mode =
				MLIB_MT_SLEEP_MODE_NO_SLEEP;
			status_modem_lib_request = modem_lib_request(
				MLIB_SOL_MTEST_SET_SLEEP_MODE,
				(void *)&testControl, NULL, NULL);
			break;
		case MODEM_LP_ENABLED:
			testControl.CPU_ID = MLIB_MT_CPU_ALL;
			testControl.forced_mode =
				MLIB_MT_SLEEP_MODE_ALL_ALLOWED;
			status_modem_lib_request = modem_lib_request(
				MLIB_SOL_MTEST_SET_SLEEP_MODE,
				(void *)&testControl, NULL, NULL);
			break;
		default:
			modem_mode_status = ERR_UNKNOWN_MODE;
			printf("ERROR: pwm_set_modem_mode MODE "
				"not implemented\n");
			break;
		}

		if ((status_modem_lib_request != MLIB_R_SUCCESS)
			&& (status_modem_lib_request != MLIB_R_COMPLETED)) {
			modem_mode_status =
				(vp_mode == MODEM_LP_DISABLED) ?
					ERR_MODEM_LP_DISABLED_KO :
					ERR_MODEM_LP_ENABLED_KO;
			printf("ERROR status_modem_lib_request "
				"T= %d %d\n", status_modem_lib_request,
				MLIB_R_SUCCESS);
		} else {
			modem_sleep_req_done = 0;

			/* MLIB init done */
			/* Read isi message from modem to configure MLIB */
			/* Faire 4 isi_driver_read et forwarder les messages */
			/* a MLIB avec modem_lib_treat_ISI_message(i */
			while (!modem_sleep_req_done) {
				status_isi_driver_read =
					isi_driver_read(
						ISI_DRIVER_MAX_MESSAGE_LENGTH,
						(uint8_t *)vl_MsgTab,
						&vl_RecMsgleng);

				if (status_isi_driver_read != ISID_R_SUCCESS)
					printf("ERROR status_isi_driver_read "
						"= %d\n",
						status_isi_driver_read);
				else
					modem_lib_treat_ISI_message(
						vl_MsgTab[3],
						vl_RecMsgleng,
						vl_MsgTab);
			}
			modem_mode_status =
				(vp_mode == MODEM_LP_DISABLED) ?
					MODEM_LP_DISABLED_OK :
					MODEM_LP_ENABLED_OK;
		}
	}

	isi_driver_close();

	return modem_mode_status;
#else
status_t modem_mode_status = INIT_VAL;

switch (vp_mode) {
		case MODEM_LP_DISABLED:

			modemMngt_setModemState (0);

			modem_mode_status = MODEM_LP_DISABLED_OK ;
			break;
		case MODEM_LP_ENABLED:

			modemMngt_setModemState (1);

			modem_mode_status = MODEM_LP_ENABLED_OK;
			break;
		default:
			modem_mode_status = ERR_UNKNOWN_MODE;
			printf("ERROR: pwm_set_modem_mode MODE "
				"not implemented\n");
			break;
		}

	return modem_mode_status;

#endif
}

int32_t pwm_set_modem_opp(modem_opp_t vp_opp)
{
#ifndef PWS_9540
	status_t modem_opp_status = INIT_VAL;
	int32_t status_modem_lib_request = MLIB_R_NOT_INITIALIZED;
	int32_t status_modem_lib_init_all;
	int32_t status_isi_driver_read;
	t_MLIB_MT_DVFStestControl DVFStestControl;

	uint8_t  vl_MsgTab[ISI_DRIVER_MAX_MESSAGE_LENGTH];
	int32_t  vl_RecMsgleng  = 0;

	DVFStestControl.DVFS_control = MLIB_MT_DVFS_CPU_OPP;

	if (vp_opp == MODEM_OPP100)
		DVFStestControl.OPP_percentage = 100;
	else
		DVFStestControl.OPP_percentage = 50;

	isi_driver_init();
	status_modem_lib_init_all = modem_lib_init_all(modem_cb,
		modem_cb_unsollicited);

	if (status_modem_lib_init_all != MLIB_R_SUCCESS) {
		modem_opp_status = ERR_MODEM_OPP_KO;
		printf("ERROR status_modem_lib_init_all = %d\n",
			status_modem_lib_init_all);
	} else {
		status_modem_lib_request =
			modem_lib_request(MLIB_SOL_MTEST_SET_DVFS_MODE,
			(void *)&DVFStestControl, NULL, NULL);

		if ((status_modem_lib_request != MLIB_R_SUCCESS)
			&& (status_modem_lib_request != MLIB_R_COMPLETED)) {
			modem_opp_status = ERR_MODEM_OPP_KO;
		} else {
			modem_dvfs_req_done = 0;

			while (!modem_dvfs_req_done) {
				status_isi_driver_read =
					isi_driver_read(
					ISI_DRIVER_MAX_MESSAGE_LENGTH,
					(uint8_t *)vl_MsgTab, &vl_RecMsgleng);

				if (status_isi_driver_read != ISID_R_SUCCESS)
					printf("ERROR status_isi_driver_read "
						"= %d\n",
						status_isi_driver_read);
				else
					modem_lib_treat_ISI_message(
						vl_MsgTab[3],
						vl_RecMsgleng,
						vl_MsgTab);
			}

			modem_opp_status = MODEM_OPP_OK;
		}
	}

	isi_driver_close();
	return modem_opp_status;
#else
	return ERR_FILE_ERROR;
#endif
}

status_t pwm_reset_modem(modem_reset_t vp_reset)
{
#ifndef PWS_9540
	status_t modem_reset_status = INIT_VAL;
	int32_t status_modem_lib_request = MLIB_R_NOT_INITIALIZED;
	int32_t status_isi_driver_read;

	uint8_t  vl_MsgTab[ISI_DRIVER_MAX_MESSAGE_LENGTH];
	int32_t  vl_RecMsgleng  = 0;

	isi_driver_init();
	if(status_modem_lib_init_all != MLIB_R_SUCCESS)
		status_modem_lib_init_all = modem_lib_init_all(modem_cb, modem_cb_unsollicited);

	switch (vp_reset) {
	case MODEM_SW_RESET:
		if (status_modem_lib_init_all != MLIB_R_SUCCESS) {
			modem_reset_status = ERR_MODEM_RESET_KO;
			printf("ERROR status_modem_lib_init_all = %d\n",
				status_modem_lib_init_all);
		} else {
			status_modem_lib_request =
				modem_lib_request(MLIB_SOL_MCE_MODEM_RESET,
				NULL, NULL, NULL);

			if ((status_modem_lib_request != MLIB_R_SUCCESS)
				&& (status_modem_lib_request !=
				MLIB_R_COMPLETED)) {
				modem_reset_status = ERR_MODEM_RESET_KO;
			} else {
				modem_reset_req_done = 0;

				while (!modem_reset_req_done) {
					status_isi_driver_read =
						isi_driver_read(
						ISI_DRIVER_MAX_MESSAGE_LENGTH,
						(uint8_t *)vl_MsgTab,
						&vl_RecMsgleng);

					if (status_isi_driver_read !=
						ISID_R_SUCCESS)
						printf("ERROR "
							"status_isi_driver_read"
							"= %d\n",
							status_isi_driver_read);
					else
						modem_lib_treat_ISI_message(
							vl_MsgTab[3],
							vl_RecMsgleng,
							vl_MsgTab);
				}

				modem_reset_status = MODEM_RESET_OK;
			}
		}
		break;

	case MODEM_WD_RESET:
	{
		t_MLIB_MTest_RunParameters mtest_data;
		mtest_data.test_group_id = MLIB_MT_GRP_LICENSEE_L23;
		mtest_data.test_handler_id = 0x0010;
		mtest_data.test_case_id = 0x0011;
		mtest_data.data_length = 0;

		if (status_modem_lib_init_all != MLIB_R_SUCCESS)
		{
			modem_reset_status = ERR_MODEM_RESET_KO;
			printf("ERROR status_modem_lib_init_all = %d\n", status_modem_lib_init_all);
		}
		else
		{
			status_modem_lib_request = modem_lib_request(MLIB_SOL_MTEST_RUN_TEST, (void*) &mtest_data, NULL, NULL);

			if( (status_modem_lib_request != MLIB_R_SUCCESS) && (status_modem_lib_request != MLIB_R_COMPLETED) )
			{
				modem_reset_status = ERR_MODEM_RESET_KO;
			}
			else
			{
				printf("Modem WD Reset requested, wait for silent reboot\n");
				modem_reset_status = MODEM_RESET_OK;
			}
		}

		break;
	}

	default:
		break;
	}

    return modem_reset_status;
#else
	return ERR_FILE_ERROR;
#endif
}

status_t pwm_reset_ap(ap_reset_t vp_reset)
{
	status_t reset_status = INIT_VAL;

	regDB_addr_t v_addr = 0;
	regDB_val_t  v_val  = 0;
	uint16_t v_u16_addr = 0;
	uint8_t  v_u8_val   = 0;
	switch (vp_reset)
	{
		case SW_ARM_RESET:
		case SW_APE_RESET:
			v_addr = 0x80157228;
			v_val  = 0x1;
			reset_status = db_u32_write(v_addr, v_val);
			break;

		case WD_ARM_RESET:
			v_addr = 0xA0410620;
			v_val  = 0x100;
			reset_status = db_u32_write(v_addr, v_val);

			v_addr = 0xA0410628;
			v_val  = 0xD;
			reset_status |= db_u32_write(v_addr, v_val);
			break;

		case AB8500_WD_RESET:
			v_u16_addr = 0x0202;
			v_u8_val   = 0x6;
			reset_status = abxxxx_write(v_u16_addr, v_u8_val);

			v_u16_addr = 0x0201;
			v_u8_val   = 0x11;
			reset_status |= abxxxx_write(v_u16_addr, v_u8_val);
			break;

		default:
			break;
	}

	return reset_status;
}
