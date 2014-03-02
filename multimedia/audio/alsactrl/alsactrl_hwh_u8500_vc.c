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
#include <sys/time.h>

#include "alsactrl.h"
#include "alsactrl_debug.h"
#include "alsactrl_alsa.h"

#include "alsactrl_hwh.h"
#include "alsactrl_hwh_db.h"
#include "alsactrl_hwh_u85xx.h"
#include "alsactrl_hwh_u8500_vc.h"

// Public interface

int Alsactrl_Hwh_U8500_VC(sqlite3* db_p, const char* indev, const char* outdev, int fs)
{
	bool ret = false, vc_on;
	int ret_func = ERR_GENERIC;
	const char* dev_top = NULL;
	char* outdev_top = NULL;
	char* indev_top = NULL;
	UNUSED_PAR(fs);

	LOG_I("%s: Enter.", __func__);

	vc_on = (indev != NULL) && (outdev != NULL);
	if (vc_on)
		LOG_I("%s: VC on (indev = '%s', outdev = '%s', fs = %d).", __func__, indev, outdev, fs);
	else
		LOG_I("%s: VC off.", __func__);

	Alsactrl_Hwh_OpenControls();

	ret = audio_hal_alsa_set_control("Master Clock Select", 0, vc_on ? 0 : 1); // VC -> SYSCLK,  Non-VC -> ULPCLK
	if (ret < 0) {
		LOG_E("%s: ERROR: Unable to set audio master-clock!\n", __func__);
		goto cleanup;
	}

	if (vc_on) {
		ret = Alsactrl_Hwh_GetToplevelMapping(indev, &dev_top);
		if (ret < 0) {
			LOG_E("%s: ERROR: Unable to find top-level device for '%s'!\n", __func__, indev);
			goto cleanup;
		}

		indev_top = strdup(dev_top);
		if (!indev_top) {
			LOG_E("%s: ERROR: strdup() failed\n", __func__);
			goto cleanup;
		}

		ret = Alsactrl_Hwh_GetToplevelMapping(outdev, &dev_top);
		if (ret < 0) {
			LOG_E("%s: ERROR: Unable to find top-level device for '%s'!\n", __func__, outdev);
			goto cleanup;
		}

		outdev_top = strdup(dev_top);
		if (!outdev_top) {
			LOG_E("%s: ERROR: strdup() failed\n", __func__);
			goto cleanup;
		}

		LOG_I("%s: '%s','%s' maps to '%s','%s' (fs = %d)\n", __func__, indev, outdev, indev_top, outdev_top, fs);

		ret = SetFilters(db_p, indev_top, outdev_top, fs);
		if (ret < 0) {
			LOG_E("%s: Failed to configure filters! (ret = %d)\n", __func__, ret);
			goto cleanup;
		}

	} else {
		ret = SetSidetoneEnable(false, false);
		if (ret < 0) {
			LOG_E("%s: ERROR: Failed! (ret = %d)\n", __func__, ret);
			goto cleanup;
		}
		ret = audio_hal_alsa_set_control("ANC Playback Switch", 0, 0);
		if (ret < 0) {
			LOG_E("%s: ERROR: Unable to disable ANC! (ret = %d)\n", __func__, ret);
			goto cleanup;
		}
	}

	ret_func = 0;

cleanup:
	if (outdev_top != NULL) free(outdev_top);
	if (indev_top != NULL) free(indev_top);

	Alsactrl_Hwh_CloseControls();

	return ret_func;
}
