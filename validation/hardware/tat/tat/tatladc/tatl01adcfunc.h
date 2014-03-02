/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATL01ADCFUNC_H_
#define TATL01ADCFUNC_H_

#define BUFFERMAX 255

#define AB_DEBUGFS_PATH "/sys/kernel/debug/ab8500/gpadc/"
#define ACC_DETECT_1	AB_DEBUGFS_PATH"acc_detect1"
#define ACC_DETECT_2	AB_DEBUGFS_PATH"acc_detect2"
#define AUX1			AB_DEBUGFS_PATH"adc_aux1"
#define AUX2			AB_DEBUGFS_PATH"adc_aux2"
#define BK_BAT			AB_DEBUGFS_PATH"bk_bat_v"
#define BATTEMP			AB_DEBUGFS_PATH"btemp_ball"
#define BATCTRL			AB_DEBUGFS_PATH"bat_ctrl"
#define VMAINCHARGE		AB_DEBUGFS_PATH"main_charger_v"
#define V_BUS			AB_DEBUGFS_PATH"vbus_v"
#define IMAINCHARGE		AB_DEBUGFS_PATH"main_charger_c"
#define IUSBCHARGE		AB_DEBUGFS_PATH"usb_charger_c"
#define MAINBATV		AB_DEBUGFS_PATH"main_bat_v"
#define HW_MAINBATV		AB_DEBUGFS_PATH"hw_main_bat_v"
#define SAMPLE          AB_DEBUGFS_PATH"avg_sample"
#define TRIG_EDGE       AB_DEBUGFS_PATH"trig_edge"
#define TRIG_TIMER      AB_DEBUGFS_PATH"trig_timer"

#define BATCTRL_CONV		1.32
#define BATTEMP_CONV		1.32
#define VMAINCHARGE_CONV	19.56
#define ACC_DETECT_1_CONV	1.32
#define ACC_DETECT_2_CONV	2.44
#define AUX1_CONV			1.32
#define AUX2_CONV			1.32
#define MAINBATV_CONV		2.44
#define V_BUS_CONV			19.56
#define IMAINCHARGE_CONV	1.46
#define IUSBCHARGE_CONV		1.46
#define BK_BAT_CONV			3.13
#define DIETEMP_CONV		1

#define SAMPLE_1        1
#define SAMPLE_4        4
#define SAMPLE_8        8
#define SAMPLE_16       16

#define GG_DEBUGFS_PATH				"/sys/kernel/debug/deep_debug/fg/"
#define INIT_CC_1                   GG_DEBUGFS_PATH"fg_algo_enable"
#define INIT_CC_2                   GG_DEBUGFS_PATH"fg_enable"
#define CALAUTO_AVG					GG_DEBUGFS_PATH"internal_calibration/cc_int_n_avg"
#define CALAUTO_OFFSET				GG_DEBUGFS_PATH"internal_calibration/cc_int_offset"
#define CALMANU_SAMPLE				GG_DEBUGFS_PATH"software_calibration/cc_sample_conv"
#define CALMANU_OFFSET				GG_DEBUGFS_PATH"software_calibration/cc_soft_offset"
#define ONE_SAMPLE_RST				GG_DEBUGFS_PATH"cc_one_sample/cc_rst_accu_sample"
#define ONE_SAMPLE_MUX_OFFSET		GG_DEBUGFS_PATH"cc_one_sample/cc_mux_offset"
#define ONE_SAMPLE					GG_DEBUGFS_PATH"cc_one_sample/cc_one_sample"
#define ONE_SAMPLE_NCONV_ACCU       GG_DEBUGFS_PATH"cc_one_sample/cc_nconv_accu"
#define READ_ACCU_RST				GG_DEBUGFS_PATH"read_n_samples/cc_rst_nconv_accu"
#define READ_ACCU_MUX_OFFSET		GG_DEBUGFS_PATH"read_n_samples/cc_mux_offset"
#define READ_ACCU_NB_SAMPLES		GG_DEBUGFS_PATH"read_n_samples/cc_nb_samples_to_average"
#define READ_ACCU					GG_DEBUGFS_PATH"read_n_samples/cc_retrieve_samples"

char *ADC_string[ADC_ELEMENT_COUNT]
#if defined (DTHL01ADCFUNC_C)
={NULL}
#endif
;

int dthl41_00ActADC_exec(struct dth_element *elem);
int dthl41_02ActADCParam_Set(struct dth_element *elem, void *Value);
int dthl41_03ActADCParam_Get(struct dth_element *elem, void *Value);

#endif /* TATL01ADCFUNC_H_*/

