/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "alsactrl.h"
#include "alsactrl_alsa.h"
#include "alsactrl_debug.h"

#include "alsactrl_hwh.h"
#include "alsactrl_hwh_Int.h"
#include "alsactrl_hwh_u8500.h"
#include "alsactrl_hwh_u8500_vc.h"
#include "alsactrl_hwh_u8540.h"
#include "alsactrl_hwh_u8540_vc.h"

#include "alsactrl_tinyalsa_extn.h"
#include <tinyalsa/asoundlib.h>

static hwh_t hwh;
static alsactrl_hwh_GetToplevelMapping_t hwh_get_top_level_mapping_fp = NULL;

// Private interface

static const char *stream_str(unsigned int stream_dir)
{
	return (stream_dir == PCM_OUT) ? "Playback" : "Capture";
}

// Public interface

const char *Alsactrl_Hwh_GetMicStr(enum amic am)
{
	switch (am) {
	case MIC1A:
		return "MIC1A";
	case MIC1B:
		return "MIC1B";
	case MIC2:
		return "MIC2";
	default:
		return "UNKNOWN";
	}
};

enum audio_hal_chip_id_t Alsactrl_Hwh_SelectHW(void)
{
	LOG_I("%s: Enter.", __func__);

	hwh.chip_id = audio_hal_alsa_get_chip_id();
	if (hwh.chip_id <= 0) {
		LOG_I("%s: ERROR: Failed to get chip_id (%d)!", __func__, hwh.chip_id);
		return CHIP_ID_UNKNOWN;
	}

	switch (hwh.chip_id) {
	case CHIP_ID_AB8500:
		hwh.card_name = "U85x0card";
		hwh.Hwh_Init = Alsactrl_Hwh_U8500_Init;
		hwh.Hwh = Alsactrl_Hwh_U8500;
		hwh.Hwh_VC = Alsactrl_Hwh_U8500_VC;
		break;
	case CHIP_ID_AB8505_V1:
	case CHIP_ID_AB8505_V2:
	case CHIP_ID_AB8505_V3:
	case CHIP_ID_AB9540_V1:
	case CHIP_ID_AB9540_V2:
		hwh.card_name = "U85x0card";
		//hwh.Hwh_Init = Alsactrl_Hwh_U9540_Init;
		//hwh.Hwh = Alsactrl_Hwh_U9540;
		//hwh.Hwh_VC = Alsactrl_Hwh_U9540_VC;
		hwh.Hwh_Init = Alsactrl_Hwh_U8500_Init;
		hwh.Hwh = Alsactrl_Hwh_U8500;
		hwh.Hwh_VC = Alsactrl_Hwh_U8500_VC;
		break;
	case CHIP_ID_AB9540_V3:
		LOG_E("%s: ERROR: Chip-ID '%s' not supported!", __func__, audio_hal_alsa_get_chip_id_str(hwh.chip_id));
		return CHIP_ID_UNKNOWN;
	case CHIP_ID_AB8540_V1:
	case CHIP_ID_AB8540_V2:
		hwh.card_name = "U8500card";
		hwh.Hwh_Init = Alsactrl_Hwh_U8540_Init;
		hwh.Hwh = Alsactrl_Hwh_U8540;
		hwh.Hwh_VC = Alsactrl_Hwh_U8540_VC;
		break;
	default:
		LOG_E("%s: ERROR: Unknown codec chip-ID '%s'!", __func__, audio_hal_alsa_get_chip_id_str(hwh.chip_id));
		return CHIP_ID_UNKNOWN;
	};

	LOG_I("%s: HW-handler initialized for '%s' (chip_id = %d).", __func__, audio_hal_alsa_get_chip_id_str(hwh.chip_id), hwh.chip_id);

	return hwh.chip_id;
}

int Alsactrl_Hwh_Init(sqlite3* db_p, alsactrl_hwh_GetToplevelMapping_t get_top_level_mapping_fp)
{

	alsactrl_dbg_set_logs_from_properties();

	if (get_top_level_mapping_fp == NULL) {
		LOG_I("%s: ERROR: get_top_level_mapping_fp is NULL!", __func__);
		return ERR_INVPAR;
	}
	hwh_get_top_level_mapping_fp = get_top_level_mapping_fp;

	if (!hwh.Hwh_Init) {
		LOG_I("%s: ERROR: Hwh_Init not registered!", __func__);
		return ERR_GENERIC;
	}
	hwh.Hwh_Init(hwh.chip_id, db_p);


	return 0;
}

int Alsactrl_Hwh(sqlite3* db_p, hwh_dev_next_t dev_next_fp, hwh_d2d_next_t dev_next_d2d_fp, fadeSpeed_t fadeSpeed)
{
	if (!hwh.Hwh) {
		LOG_I("%s: ERROR: hwh not registered!", __func__);
		return ERR_GENERIC;
	}

	hwh.Hwh(db_p, dev_next_fp, dev_next_d2d_fp, fadeSpeed);

	return 0;
}

int Alsactrl_Hwh_VC(sqlite3* db_p, const char* indev, const char* outdev, int fs)
{
	if (!hwh.Hwh_VC) {
		LOG_I("%s: ERROR: hwh_vc not registered!", __func__);
		return ERR_GENERIC;
	}

	hwh.Hwh_VC(db_p, indev, outdev, fs);

	return 0;
}

int Alsactrl_Hwh_GetToplevelMapping(const char* toplevel_dev, const char** actual_dev)
{
	int res;

	if (hwh_get_top_level_mapping_fp == NULL) {
		LOG_I("%s: ERROR: hwh_get_top_level_mapping_fp is NULL!", __func__);
		return ERR_GENERIC;
	}

	res = hwh_get_top_level_mapping_fp(toplevel_dev, actual_dev);
	if (res != 0) {
		LOG_I("%s: ERROR: hwh_get_top_level_mapping_fp failed with error %d!", __func__, res);
		return ERR_GENERIC;
	}

	return 0;
}

int Alsactrl_Hwh_OpenControls(void)
{
	return audio_hal_alsa_open_controls(hwh.card_name);
}

void Alsactrl_Hwh_CloseControls(void)
{
	audio_hal_alsa_close_controls();
}

const char* Alsactrl_Hwh_CardName(void)
{
	return hwh.card_name;
}

enum audio_hal_chip_id_t Alsactrl_Hwh_ChipID(void)
{
	return hwh.chip_id;
}

static alsactrl_dev_info_t devs[] = {
	{
		"AB850x (Playback)",
		ALSACTRL_ALSA_DEVICE_AB850x,
		"ab850x_0",
		PCM_OUT,
		8,
		16,
		48000,
		NULL,
		0,
		false
	},
	{
		"AB850x (Capture)",
		ALSACTRL_ALSA_DEVICE_AB850x,
		"ab850x_1",
		PCM_IN,
		8,
		16,
		48000,
		NULL,
		0,
		false
	},
	{
		"CG29XX (Playback)",
		ALSACTRL_ALSA_DEVICE_CG29XX,
		"cg29xx_0",
		PCM_OUT,
		1,
		16,
		8000,
		NULL,
		0,
		false
	},
	{
		"CG29XX (Capture)",
		ALSACTRL_ALSA_DEVICE_CG29XX,
		"cg29xx_0",
		PCM_IN,
		1,
		16,
		8000,
		NULL,
		0,
		false
	},
	{
		"AB8540 (Playback)",
		ALSACTRL_ALSA_DEVICE_AB8540,
		"ab8540_0",
		PCM_OUT,
		8,
		16,
		48000,
		NULL,
		0,
		false
	},
	{
		"AB8540 (Capture)",
		ALSACTRL_ALSA_DEVICE_AB8540,
		"ab8540_1",
		PCM_IN,
		8,
		16,
		48000,
		NULL,
		0,
		false
	},
	{
		"AB8540 Voice (Playback)",
		ALSACTRL_ALSA_DEVICE_AB8540_VC,
		"ab8540_2_voice",
		PCM_OUT,
		1,
		16,
		16000,
		NULL,
		0,
		false
	},
	{
		"AB8540 Voice (Capture)",
		ALSACTRL_ALSA_DEVICE_AB8540_VC,
		"ab8540_2_voice",
		PCM_IN,
		4,
		16,
		16000,
		NULL,
		0,
		false
	}
};

alsactrl_dev_info_t* Alsactrl_Hwh_GetDevInfo(enum alsactrl_alsa_device alsa_dev, unsigned int stream_dir)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(devs); i++)
		if (devs[i].alsa_dev == alsa_dev && devs[i].stream_dir == stream_dir)
			return &devs[i];

	return NULL;
}

alsactrl_dev_info_t* Alsactrl_Hwh_GetDevInfo_Peer(alsactrl_dev_info_t* dev)
{
	unsigned int stream_dir = (dev->stream_dir == PCM_OUT) ? PCM_IN : PCM_OUT;

	return Alsactrl_Hwh_GetDevInfo(dev->alsa_dev, stream_dir);
}

// Protected interface

int Alsactrl_Hwh_OpenAlsaDev(alsactrl_dev_info_t* dev_info_p)
{
	int ret, idx_card, idx_dev;
	struct pcm_config config;
	unsigned int flags = 0;
	LOG_I("Enter.\n");

	if (dev_info_p->dev_name == NULL) {
		LOG_E("ERROR: No device-name set for stream (%s)!\n", stream_str(dev_info_p->stream_dir));
		return -1;
	}

	if (dev_info_p->opened) {
		LOG_I("ALSA-device %s already opened!\n", dev_info_p->dev_name);
		return 0;
	}

	ret = audio_hal_alsa_get_card_and_device_idx(dev_info_p->dev_name, dev_info_p->stream_dir, &idx_card, &idx_dev);
	if (ret < 0) {
		LOG_E("ERROR: ALSA-device %s not found among %s-devices!\n",
			dev_info_p->dev_name,
			stream_str(dev_info_p->stream_dir));
		return -1;
	}
	LOG_I("Opening ALSA-device (%s).\n", stream_str(dev_info_p->stream_dir));
	flags = dev_info_p->stream_dir;
	memset(&config, 0, sizeof(struct pcm_config));
	config.channels = dev_info_p->channels;
	config.rate = dev_info_p->rate;
	config.period_size = 1024;
	config.period_count = 4;
	config.format = PCM_FORMAT_S16_LE;

	/* Values to use for the ALSA start, stop and silence thresholds.  Setting
	* any one of these values to 0 will cause the default tinyalsa values to be
	* used instead.  Tinyalsa defaults are as follows.
	*
	* start_threshold   : period_count * period_size
	* stop_threshold    : period_count * period_size
	* silence_threshold : 0
	*/
	config.start_threshold = 0;
	config.stop_threshold = 0;
	config.silence_threshold = 0;
	config.avail_min = 0;
	dev_info_p->pcm = pcm_open(idx_card, idx_dev, flags, &config);
	if (!dev_info_p->pcm || !pcm_is_ready(dev_info_p->pcm)) {
		LOG_E("ERROR: Open ALSA-device (%s) failed (ret = %s)!\n",
			stream_str(dev_info_p->stream_dir), pcm_get_error(dev_info_p->pcm));
		return -1;
	}
	LOG_I("hw_params set.\n");
	dev_info_p->opened = true;
	ret = pcm_prepare(dev_info_p->pcm);
	if (ret < 0) {
		LOG_E("ERROR: PCM Start Failed (ret = %s)!", pcm_get_error(dev_info_p->pcm));
	}
	return 0;
}

void Alsactrl_Hwh_CloseAlsaDev(alsactrl_dev_info_t* dev_info_p)
{
	int ret;

	LOG_I("Enter.\n");

	if (dev_info_p->dev_name == NULL) {
		LOG_E("ERROR: No device-name set for stream (%s)!\n", stream_str(dev_info_p->stream_dir));
		return;
	}

	if (!dev_info_p->opened) {
		LOG_E("ALSA-device %s already closed!\n", dev_info_p->dev_name);
		return;
	}
	LOG_I("Closing ALSA-device %s (%s).\n", dev_info_p->dev_name, stream_str(dev_info_p->stream_dir));
	if (dev_info_p->pcm == NULL) {
		LOG_I("Warning: ALSA-device %s is NULL!\n", dev_info_p->dev_name);
	} else {
		ret = pcm_close(dev_info_p->pcm);
		if (ret < 0) {
			LOG_E("Error: PCM close failed, ret = %s", pcm_get_error(dev_info_p->pcm));
		}
		dev_info_p->pcm = NULL;
	}

	dev_info_p->opened = false;
}
