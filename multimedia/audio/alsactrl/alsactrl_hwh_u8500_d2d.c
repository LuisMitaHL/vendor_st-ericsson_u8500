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
#include "alsactrl_debug.h"
#include "alsactrl_alsa.h"
#include "alsactrl_fm.h"

#include "alsactrl_hwh.h"
#include "alsactrl_hwh_Int.h"
#include "alsactrl_hwh_u85xx.h"
#include "alsactrl_hwh_u8500_dev.h"
#include "alsactrl_hwh_u8500_d2d.h"

static u85xx_d2d_t u8500_d2ds[] = {
	{
		.name_src = ALSACTRL_DEVSTR_FMRX,
		.name_dst = ALSACTRL_DEVSTR_HSOUT,
		.d2d_handler = NULL,
	},
	{
		.name_src = ALSACTRL_DEVSTR_FMRX,
		.name_dst = ALSACTRL_DEVSTR_SPEAKER,
		.d2d_handler = NULL,
	},
};

static void UpdateActiveD2D(hwh_d2d_next_t dev_next_d2d)
{
	const char* src_name = NULL;
	const char* dst_name = NULL;
	u85xx_device_t* u8500_devices = Alsactrl_Hwh_U8500_Dev_GetDevs();

	while (dev_next_d2d(&src_name, &dst_name) == 0) {
		if (strcmp(src_name, ALSACTRL_DEVSTR_FMRX) == 0) {
			if (strcmp(dst_name, ALSACTRL_DEVSTR_HSOUT) == 0) {
				u8500_devices[U8500_DEV_FMRX].active = true;
				u8500_devices[U8500_DEV_HSOUT].active = true;
				u8500_d2ds[U8500_D2D_FMRX_HSOUT].active = true;
			} else if (strcmp(dst_name, ALSACTRL_DEVSTR_SPEAKER) == 0) {
				u8500_devices[U8500_DEV_FMRX].active = true;
				u8500_devices[U8500_DEV_SPEAKER].active = true;
				u8500_d2ds[U8500_D2D_FMRX_SPEAKER].active = true;
			}
			(void)setup_fm_analog_out();
			LOG_I("%s: found src:%s  dst:%s\n", __func__, src_name, dst_name);
		}
	}
}

static void ClearActiveD2Ds(void)
{
	unsigned int i;
	LOG_I("%s: Enter.", __func__);
	for (i = 0; i < ARRAY_SIZE(u8500_d2ds); i++) {
		u8500_d2ds[i].active = false;
	}
}

// Public interface

int Alsactrl_Hwh_U8500_D2D_UpdateSwitches(void)
{
	LOG_I("%s: Enter.", __func__);

	LOG_I("%s: u8500_d2ds[U8500_D2D_FMRX_HSOUT].active: %d", __func__, u8500_d2ds[U8500_D2D_FMRX_HSOUT].active);
	audio_hal_alsa_set_control("LineIn Left to Headset Left", 0, u8500_d2ds[U8500_D2D_FMRX_HSOUT].active);
	audio_hal_alsa_set_control("LineIn Right to Headset Right", 0, u8500_d2ds[U8500_D2D_FMRX_HSOUT].active);
	audio_hal_alsa_set_control("AD1 to IHF Left", 0, u8500_d2ds[U8500_D2D_FMRX_SPEAKER].active);
	audio_hal_alsa_set_control("AD2 to IHF Right", 0, u8500_d2ds[U8500_D2D_FMRX_SPEAKER].active);

	return 0;
}

u85xx_d2d_t* Alsactrl_Hwh_U8500_D2D_GetD2Ds(void)
{
	return u8500_d2ds;
}

int Alsactrl_Hwh_U8500_D2D(sqlite3* db_p, hwh_d2d_next_t dev_next_d2d)
{
	int rc = SQLITE_OK;
	sqlite3_stmt *stmt = NULL;
	const char* src_name = NULL;
	const char* dst_name = NULL;
	char *command = NULL;
	const unsigned char* data = NULL;
	int ret = 0;
	unsigned int i;

	LOG_I("%s: Enter.", __func__);

	ClearActiveD2Ds();

	// Update active-flags
	UpdateActiveD2D(dev_next_d2d);

	command = malloc(1024 * sizeof(char));

	while (dev_next_d2d(&src_name, &dst_name) == 0) {
		memset(command, 0, 1024);
		sprintf(command, "SELECT Data FROM HW_Settings_Data_D2D WHERE Idx=(\
				SELECT Idx_Data FROM HW_Settings_Combo_D2D WHERE (Src='%s') AND (Dst='%s'))",
				src_name, dst_name);

		LOG_I("%s: Query: %s", __func__, command);

		rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
		if (rc != SQLITE_OK) {
			LOG_E("%s: ERROR: Unable to prepare SQL-statement!", __func__);
			goto cleanup;
		}

		if (sqlite3_step(stmt) != SQLITE_ROW)
			goto cleanup;

		data = sqlite3_column_text(stmt, 0);
		if (data == NULL) {
			LOG_E("%s: ERROR: Data not found!\n", __func__);
			goto cleanup;
		}

		ret = audio_hal_alsa_memctrl_set((const char*)data);
		if (ret < 0) {
			LOG_E("%s: ERROR: audio_hal_alsa_memctrl_set failed (ret = %d)!", __func__, ret);
			goto cleanup;
		}

		LOG_I("%s: Found match! (src: %s, dst :%s)\n %s\n", __func__, src_name, dst_name, data);
	}

	/* Call d2d-handlers for active devices */
	for (i = 0; i < ARRAY_SIZE(u8500_d2ds); i++) {
		if ((u8500_d2ds[i].d2d_handler) && (u8500_d2ds[i].active)) {
			LOG_I("%s: Calling d2d-handler (src = '%s' dst = '%s').", __func__,
					u8500_d2ds[i].name_src, u8500_d2ds[i].name_dst);
			ret = u8500_d2ds[i].d2d_handler(&u8500_d2ds[i]);
			if (ret < 0) {
				LOG_E("%s: Error in d2d-handler (src = '%s', dst = '%s')!", __func__,
						u8500_d2ds[i].name_src, u8500_d2ds[i].name_dst);
				goto cleanup;
			}
		}
	}

cleanup:
	if (command != NULL) free(command);
	if (stmt != NULL) {
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	return ret;
}
