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
#include <errno.h>

#include "alsactrl.h"
#include "alsactrl_debug.h"
#include "alsactrl_alsa.h"

#include "alsactrl_hwh_db.h"
#include "alsactrl_hwh_u85xx.h"

int IsComboDevice(const char* name)
{
	const char *devices_combo[] = {
		ALSACTRL_DEVSTR_HSIN,
		ALSACTRL_DEVSTR_MIC,
		ALSACTRL_DEVSTR_HSOUT,
		ALSACTRL_DEVSTR_SPEAKER,
		ALSACTRL_DEVSTR_EARP,
		ALSACTRL_DEVSTR_VIBL,
		ALSACTRL_DEVSTR_VIBR,
		ALSACTRL_DEVSTR_TTYIN,
		ALSACTRL_DEVSTR_TTYOUT,
		NULL
	};
	const char** cur = devices_combo;

	while (*cur) {
		if (strcmp(*cur, name) == 0) {
			return 1;
		}
		cur++;
	}
	return 0;
}

void SetActive(u85xx_device_t *dev, bool active)
{
	dev->active = active;
	LOG_I("'%s' set to %s.\n", dev->name, dev->active ? "active" : "inactive");
}

int SetSwitch(u85xx_switch_t *sw, bool enable)
{
	int ret;

	ret = audio_hal_alsa_set_control(sw->name, 0, enable);
	if (ret < 0) {
		LOG_E("ERROR: Failed to %s '%s'!\n", enable ? "enable" : "disable", sw->name);
	} else {
		LOG_I("'%s' %s.\n", sw->name, enable ? "enabled" : "disabled");
	}

	return ret;
}

int SetSidetoneRoute(sqlite3* db_p, const char *indev_p, bool *sidetone_left, bool *sidetone_right)
{
	int ret;
	mic_config_t mic_config;

	*sidetone_left = false;
	*sidetone_right = false;

	ret = Alsactrl_DB_GetMicConfig_Specific(db_p, indev_p, &mic_config);
	if (ret < 0) {
		LOG_E("ERROR: Unable to get mic-config for '%s'!", indev_p);
		return ERR_GENERIC;
	}
	if (mic_config.n_mics > 2) {
		LOG_E("ERROR: No of mics is larger than 2 (n_mics = %d)!", mic_config.n_mics);
		return ERR_INVPAR;
	}

	switch (mic_config.mics[0]) {
	case MIC1A:
		LOG_I("%s: Sidetone left selects %s", __func__, Alsactrl_Hwh_GetMicStr(mic_config.mics[0]));
		ret = audio_hal_alsa_set_control("Sidetone Left Source Playback Route", 0, 2); // 'Mic 1A'
		*sidetone_left = true;
		break;
	case MIC1B:
		LOG_I("%s: Sidetone left selects %s", __func__, Alsactrl_Hwh_GetMicStr(mic_config.mics[1]));
		ret = audio_hal_alsa_set_control("Sidetone Left Source Playback Route", 0, 2); // 'Mic 1B'
		*sidetone_left = true;
		break;
	case MIC2:
		LOG_I("%s: Sidetone left selects %s", __func__, Alsactrl_Hwh_GetMicStr(mic_config.mics[2]));
		ret = audio_hal_alsa_set_control("Sidetone Left Source Playback Route", 0, 1); // 'Mic 2'
		*sidetone_left = true;
		break;
	default:
		break;
	}

	switch (mic_config.mics[1]) {
	case MIC1A:
		LOG_I("%s: Sidetone right selects %s", __func__, Alsactrl_Hwh_GetMicStr(mic_config.mics[0]));
		ret = audio_hal_alsa_set_control("Sidetone Right Source Playback Route", 0, 1); // 'Mic 1A'
		*sidetone_right = true;
		break;
	case MIC1B:
		LOG_I("%s: Sidetone right selects %s", __func__, Alsactrl_Hwh_GetMicStr(mic_config.mics[1]));
		ret = audio_hal_alsa_set_control("Sidetone Right Source Playback Route", 0, 1); // 'Mic 1B'
		*sidetone_right = true;
		break;
	case MIC2:
		LOG_I("%s: Sidetone right selects %s", __func__, Alsactrl_Hwh_GetMicStr(mic_config.mics[2]));
		ret = audio_hal_alsa_set_control("Sidetone Right Source Playback Route", 0, 0); // 'Mic 2'
		*sidetone_right = true;
		break;
	default:
		break;
	}
	if (ret < 0)
		LOG_E("%s: ERROR: Unable to set sidetone source (ret = %d)\n", __func__, ret);

	return 0;
}

int SetSidetoneEnable(bool sidetone_left, bool sidetone_right)
{
	int ret;
	ret = audio_hal_alsa_set_control("Sidetone Left", 0, sidetone_left);
	if (ret < 0) {
		LOG_E("%s: ERROR: Unable to set sidetone left! (ret = %d)\n", __func__, ret);
		return ret;
	}
	ret = audio_hal_alsa_set_control("Sidetone Right", 0, sidetone_right);
	if (ret < 0) {
		LOG_E("%s: ERROR: Unable to set sidetone right! (ret = %d)\n", __func__, ret);
		return ret;
	}

	return 0;
}

int SetSidetoneGain(sqlite3_stmt *stmt, int *sidetone_gain)
{
	int ret;
	long gain[2];

	*sidetone_gain = sqlite3_column_int(stmt, 1);
	gain[0] = *sidetone_gain;
	gain[1] = *sidetone_gain;

	LOG_I("%s: Configuring sidetone gain = %ld,%ld\n", __func__, gain[0], gain[1]);
	ret = audio_hal_alsa_set_control_values("Sidetone Digital Gain Playback Volume", gain);
	if (ret < 0) {
		LOG_E("%s: ERROR: Unable to set sidetone gain! (ret = %d)\n", __func__, ret);
		return ret;
	}

	return 0;
}

int SetSidetoneCoeffs(sqlite3_stmt *stmt)
{
	const unsigned int NR_OF_SIDETONE_FIR_COEFFS = 128;
	long sid_fir_coefficients[NR_OF_SIDETONE_FIR_COEFFS];
	unsigned int i;
	int ret;

	// 128 unsigned 16bit FIR-coefficients
	LOG_I("%s: Configuring sidetone FIR-coefficients.\n", __func__);
	for (i = 0; i < NR_OF_SIDETONE_FIR_COEFFS; i++)
		sid_fir_coefficients[i] = sqlite3_column_int(stmt, i + 2);
	ret = audio_hal_alsa_set_control_values("Sidetone FIR Coefficients", sid_fir_coefficients);
	if (ret < 0) {
		LOG_E("%s: ERROR: Unable to set Sidetone FIR-coefficients!\n", __func__);
		return ret;
	}

	for (i = 0; i < MAX_TRIES_SID; i++) {
		ret = audio_hal_alsa_set_control("Sidetone FIR Apply Coefficients", 0, 1);
		if ((ret == 0) || ((ret < 0) && (ret != -EBUSY)))
			break;
		LOG_I("%s: Unable to apply sidetone coefficients (try %d)!\n", __func__, i);
		usleep(5 * 1000);
	}

	if (ret < 0) {
		LOG_E("%s: ERROR: Unable to apply sidetone coefficients (MAX_TRIES_SID = %d)! (ret = %d)\n", __func__, MAX_TRIES_SID, ret);
		return ret;
	}

	return ret;
}

int SetANC(sqlite3_stmt *stmt)
{
	const unsigned int NR_OF_ANC_FIR_COEFFS = 15;
	const unsigned int NR_OF_ANC_IIR_COEFFS = 24;
	long anc_fir_coefficients[NR_OF_ANC_FIR_COEFFS];
	long anc_iir_coefficients[NR_OF_ANC_IIR_COEFFS];
	unsigned int i;
	int value, ret;

	// 15 signed 16bit FIR-coefficients
	LOG_I("%s: Configuring ANC FIR-coefficients.\n", __func__);
	for (i = 0; i < NR_OF_ANC_FIR_COEFFS; i++)
		anc_fir_coefficients[i] = sqlite3_column_int(stmt, i + 5);
	ret = audio_hal_alsa_set_control_values("ANC FIR Coefficients", anc_fir_coefficients);
	if (ret < 0) {
		LOG_E("%s: ERROR: Unable to set ANC FIR-coefficients!\n", __func__);
		return ret;
	}

	// 1bit double delay + 8 signed 5bit internal IIR shift + 20 signed 24bit IIR-coefficients
	LOG_I("%s: Configuring ANC IIR-shift and IIR-coefficients.\n", __func__);
	for (i = 0; i < NR_OF_ANC_IIR_COEFFS; i++)
		anc_iir_coefficients[i] = sqlite3_column_int(stmt, i + 20);
	ret = audio_hal_alsa_set_control_values("ANC IIR Coefficients", anc_iir_coefficients);
	if (ret < 0) {
		LOG_E("%s: ERROR: Unable to set ANC IIR-coefficients!\n", __func__);
		return ret;
	}

	// 3 signed 5bit external shift parameters and 1 unsigned 16bit warp delay parameter
	LOG_I("%s: Configuring ANC Warp and Shifts.\n", __func__);
	for (i = 0; i < 4; i++) {
		value = sqlite3_column_int(stmt, i + 1);
		if (i == 0)
			ret = audio_hal_alsa_set_control("ANC Warp Delay Shift", 0, value);
		else if (i == 1)
			ret = audio_hal_alsa_set_control("ANC FIR Output Shift", 0, value);
		else if (i == 2)
			ret = audio_hal_alsa_set_control("ANC IIR Output Shift", 0, value);
		else if (i == 3)
			ret = audio_hal_alsa_set_control("ANC Warp Delay", 0, value);
		if (ret < 0) {
			LOG_E("%s: ERROR: Unable to set ANC IIR Warp or Shift!\n", __func__);
			return ret;
		}
	}

	// configure FIR-/IIR-coefficients into ab8500 ANC block
	ret = audio_hal_alsa_set_control("ANC Status", 0, 1);
	if (ret < 0) {
		LOG_E("%s: ERROR: Unable to apply ANC FIR+IIR-coefficients!\n", __func__);
		return ret;
	}

	ret = audio_hal_alsa_set_control("ANC Playback Switch", 0, 1);
	if (ret < 0)
		LOG_E("%s: ERROR: Unable to enable ANC! (ret = %d)\n", __func__, ret);

	return ret;
}

int SetFilters(sqlite3* db_p, const char* indev_p, const char* outdev_p, int fs)
{
	int ret_func = ERR_GENERIC;
	char *command = NULL;
	char *c_criteria = NULL;
	const char* preset = NULL;
	sqlite3_stmt *stmt = NULL;
	int sidetone_gain;
	bool sidetone_left, sidetone_right;
	int ret;
	char str_fs[6];

	LOG_I("%s: Enter.", __func__);

	sprintf(str_fs, "%d", fs);
	c_criteria = malloc(192 * sizeof(char));
	memset(c_criteria, 0, 192 * sizeof(char));
	strcat(c_criteria, " AND SpeechConfig.InDev = '");
	strcat(c_criteria, indev_p);
	strcat(c_criteria, "' AND SpeechConfig.OutDev = '");
	strcat(c_criteria, outdev_p);
	strcat(c_criteria, "' AND SpeechConfig.SampleRate = '");
	strcat(c_criteria, str_fs);
	strcat(c_criteria, "'");

	command = malloc(512 * sizeof(char));
	memset(command, 0, 512 * sizeof(char));
	strcat(command, "SELECT HW_Settings_FIR.* FROM SpeechConfig, HW_Settings_FIR WHERE SpeechConfig.SidetonePreset = HW_Settings_FIR.Preset");
	strcat(command, c_criteria);

	ret = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOG_E("%s: Unable to prepare SQL-statement! \"%s\" (ret = %d)\n", __func__, command, ret);
		ret = -1;
		goto cleanup;
	}
	ret = sqlite3_step(stmt);
	if (ret != SQLITE_ROW) {
		LOG_I("%s: No sidetone preset found in DB for indev = '%s', outdev = '%s'!", __func__, indev_p, outdev_p);
		ret = 0;
		goto sidetone_done;
	}
	preset = (const char*)sqlite3_column_text(stmt, 0);

	LOG_I("%s: Found sidetone preset = '%s' matching indev = '%s', outdev = '%s'.", __func__, preset, indev_p, outdev_p);

	LOG_I("%s: Configuring sidetone.", __func__);

	ret = SetSidetoneGain(stmt, &sidetone_gain);
	if (ret < 0) {
		LOG_E("%s: Failed to configure sidetone gain! (ret = %d)\n", __func__, ret);
		goto cleanup;
	}

	if (sidetone_gain == 0) {
		LOG_I("%s: No need to set sidetone coefficients with zero gain.\n", __func__);
		SetSidetoneEnable(false, false);
		goto sidetone_done;
	}

	ret = SetSidetoneRoute(db_p, indev_p, &sidetone_left, &sidetone_right);
	if (ret < 0) {
		LOG_E("%s: Failed to configure sidetone mic! (ret = %d)\n", __func__, ret);
		goto cleanup;
	}

	ret = SetSidetoneCoeffs(stmt);
	if (ret < 0) {
		LOG_E("%s: Failed to configure sidetone! (ret = %d)\n", __func__, ret);
		goto cleanup;
	}

	ret = SetSidetoneEnable(sidetone_left, sidetone_right);
	if (ret < 0) {
		LOG_E("%s: Failed to configure sidetone playback switches! (ret = %d)\n", __func__, ret);
		goto cleanup;
	}

	LOG_I("%s: Sidetone configured.\n", __func__);

sidetone_done:
	if (stmt != NULL) {
		ret = sqlite3_finalize(stmt);
		stmt = NULL;
		if (ret != SQLITE_OK) {
			LOG_I("%s: Failed to destroy prepared statement after use! (ret = %d)", __func__, ret);
		}
	}

	memset(command, 0, 512 * sizeof(char));
	strcat(command, "SELECT HW_Settings_ANC.* FROM SpeechConfig, HW_Settings_ANC WHERE SpeechConfig.ANCPreset = HW_Settings_ANC.Preset");
	strcat(command, c_criteria);
	ret = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOG_E("%s: Unable to prepare SQL-statement! \"%s\" (ret = %d)\n", __func__, command, ret);
		ret = -1;
		goto cleanup;
	}
	ret = sqlite3_step(stmt);
	if (ret != SQLITE_ROW) {
		LOG_I("%s: No ANC preset found in DB for indev = '%s', outdev = '%s'.", __func__, indev_p, outdev_p);
		ret = 0;
		goto anc_done;
	}
	preset = (const char*)sqlite3_column_text(stmt, 0);
	LOG_I("%s: Found ANC-preset = '%s' matching indev = '%s', outdev = '%s'.", __func__, preset, indev_p, outdev_p);

	LOG_I("%s: Configuring ANC.", __func__);
	ret = SetANC(stmt);
	if (ret < 0) {
		LOG_E("%s: Failed to configure ANC! (ret = %d)\n", __func__, ret);
		goto cleanup;
	} else {
		LOG_I("%s: ANC configured.\n", __func__);
	}

anc_done:
	ret_func = 0;

cleanup:
	if (stmt != NULL) {
		ret = sqlite3_finalize(stmt);
		stmt = NULL;
		if (ret != SQLITE_OK) {
			LOG_I("%s: Failed to destroy prepared statement after use! (ret = %d)", __func__, ret);
		}
	}

	if (c_criteria != NULL)
		free(c_criteria);
	if (command != NULL)
		free(command);

	LOG_I("%s: Exit (%s).", __func__, (ret_func == 0) ? "OK" : "ERROR");

	return ret_func;
}
