/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sqlite3.h>

#include "alsactrl.h"
#include "alsactrl_alsa.h"
#include "alsactrl_debug.h"

#include "alsactrl_hwh.h"
#include "alsactrl_hwh_db.h"
#include "alsactrl_hwh_u85xx.h"
#include "alsactrl_hwh_u8500.h"
#include "alsactrl_hwh_u8500_dev.h"
#include "alsactrl_hwh_u8500_d2d.h"

static enum audio_hal_chip_id_t chip_id = CHIP_ID_UNKNOWN;

static int CheckIncompabilities(sqlite3* db_p, u85xx_device_t* u8500_devs, u85xx_d2d_t* u8500_d2ds)
{
	int dev_mic_nch = Alsactrl_DB_GetDevChannelCount(db_p, ALSACTRL_DEVSTR_MIC);
	UNUSED_PAR(u8500_d2ds);

	LOG_I("Enter.");

	if ((u8500_devs[U8500_DEV_HSIN].active) &&
		(dev_mic_nch == 2) &&
		(u8500_devs[U8500_DEV_LINEIN].active)) {
		LOG_E("ERROR: Not possible to have MIC (2-channel) at the same time as LINEIN (right)!");
		return ERR_GENERIC; 
	}
	if ((u8500_devs[U8500_DEV_MIC].active) &&
		(dev_mic_nch == 2) &&
		(u8500_devs[U8500_DEV_FMRX].active) &&
		(FMRX_TYPE == FM_TYPE_ANALOG)) {
		LOG_E("ERROR: Not possible to have MIC (2-channel) at the same time as analog FMRX!");
		return ERR_GENERIC; 
	}

	return 0;
}

int Alsactrl_Hwh_U8500(sqlite3* db_p, hwh_dev_next_t dev_next, hwh_d2d_next_t dev_next_d2d, fadeSpeed_t fadeSpeed)
{
	char* dev_names = NULL;
	int ret_func = ERR_GENERIC;
	int ret;

	alsactrl_dbg_set_logs_from_properties();

	LOG_I("Enter.");

	// Make sure that the settings that is written from ADM is not overwritten
	// by any sink or source
	audio_hal_alsa_set_ignore_defaults(true);

	ret = Alsactrl_Hwh_OpenControls();
	if (ret < 0) {
		LOG_E("ERROR: Unable to open ALSA-card '%s' error: %d!\n", Alsactrl_Hwh_CardName(), ret);
		goto cleanup;
	}

	// Load default data to control-array
	ret = audio_hal_alsa_memctrl_set_default();
	if (ret < 0) {
		LOG_E("ERROR: load default cfg failed error: %d!", ret);
		goto cleanup_close;
	}

	// Write state-files for a generic device to control-array
	Alsactrl_DB_WriteDevData(db_p, dev_next, false);

	// Write state-files for a specific device to control-array
	Alsactrl_DB_WriteDevData(db_p, dev_next, true);

	// Write state-files for a combo of devices to control-array
	Alsactrl_DB_WriteComboData(db_p, dev_next);

	// Set Digital Gain Fade Speed Switch
	audio_hal_alsa_set_control("Digital Gain Fade Speed Switch", 0, fadeSpeed);

	// Execute device-handlers for generic devices
	ret = Alsactrl_Hwh_U8500_Dev(db_p, dev_next, chip_id);

	// Execute device-handlers for d2d-connections
	ret = Alsactrl_Hwh_U8500_D2D(db_p, dev_next_d2d);

	// Detect settings not possible in the HW
	ret = CheckIncompabilities(db_p, Alsactrl_Hwh_U8500_Dev_GetDevs(),  Alsactrl_Hwh_U8500_D2D_GetD2Ds());
	if (ret < 0) {
		LOG_E("ERROR: Incompabilities of HW-settings detected!");
		goto cleanup_close;
	}

	// Write control-array to HW
	ret = audio_hal_alsa_memctrl_write();
	if (ret < 0) {
		LOG_E("ERROR: commit control configuration failed!");
		goto cleanup_close;
	}

	// Update all on/off-switches for playback/recording/loopback devices
	ret = Alsactrl_Hwh_U8500_Dev_UpdateSwitches(db_p, chip_id);

	// Update all on/off-switches for d2d-paths
	ret = Alsactrl_Hwh_U8500_D2D_UpdateSwitches();

	ret_func = 0;

cleanup_close:
	Alsactrl_Hwh_CloseControls();

cleanup:
	if (dev_names != NULL) free(dev_names);

	LOG_I("Exit (%s).", (ret_func == 0) ? "OK" : "ERROR");

	return ret_func;
}

int Alsactrl_Hwh_U8500_Init(enum audio_hal_chip_id_t chip_id_in, sqlite3* db_p)
{
	int ret_func = ERR_GENERIC;
	int ret;

	LOG_I("Enter.");

	chip_id = chip_id_in;

	ret = Alsactrl_Hwh_OpenControls();
	if (ret < 0) {
		LOG_E("ERROR: Unable to open ALSA-card '%s' error: %d!\n", Alsactrl_Hwh_CardName(), ret);
		goto cleanup;
 	}

	audio_hal_alsa_set_control("Mic 1A Regulator", 0, MIC_1A_REGULATOR);
	audio_hal_alsa_set_control("Mic 1B Regulator", 0, MIC_1B_REGULATOR);
	audio_hal_alsa_set_control("Mic 2 Regulator", 0, MIC_2_REGULATOR);

	ret = audio_hal_alsa_set_control("Master Clock Select", 0, 1); // Default -> ULPCLK
	if (ret < 0) {
		LOG_E("ERROR: Unable to set master clock select! (ret = %d)\n", ret);
		goto cleanup;
	}

	ret = Alsactrl_DB_WriteDefaultData(db_p);
	if (ret < 0) {
		LOG_E("ERROR: Unable to write default data! (ret = %d)\n", ret);
		goto cleanup;
	}

	ret_func = 0;

cleanup:
	Alsactrl_Hwh_CloseControls();
	return ret_func;
}

