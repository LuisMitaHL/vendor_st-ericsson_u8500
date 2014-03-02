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
#include "alsactrl_audiocodec.h"
#include "alsactrl_alsa.h"

#include "alsactrl_hwh.h"
#include "alsactrl_hwh_Int.h"
#include "alsactrl_hwh_db.h"
#include "alsactrl_hwh_u85xx.h"
#include "alsactrl_hwh_u8540_dev.h"

/*
 * Device-handlers
 */

// Out

static int HandleDevFmtx(u85xx_device_t *dev, sqlite3* db_p)
{
	UNUSED_PAR(dev);
	UNUSED_PAR(db_p);

	audio_hal_alsa_set_control("Digital Interface AD 5 Loopback Switch", 0, 1);
	audio_hal_alsa_set_control("Digital Interface AD 6 Loopback Switch", 0, 1);
	audio_hal_alsa_set_control("Digital Interface AD To Slot 16 Map", 0, 4); // REG_ADSLOTSELX_AD_OUT5_TO_SLOT_EVEN
	audio_hal_alsa_set_control("Digital Interface AD To Slot 17 Map", 0, 5); // REG_ADSLOTSELX_AD_OUT6_TO_SLOT_ODD
	audio_hal_alsa_set_control("Digital Interface DA 7 From Slot Map", 0, 14); // Slot 14 -> DA_IN7
	audio_hal_alsa_set_control("Digital Interface DA 8 From Slot Map", 0, 15); // Slot 15 -> DA_IN8

	return 0;
}

// In

static int HandleDevMic(u85xx_device_t *dev, sqlite3* db_p)
{
	int ADSel_slot0, ADSel_slot1;
	int ret;
	mic_config_t mic_config;

	LOG_I("Enter.");

	ret = Alsactrl_DB_GetMicConfig_Generic(db_p, dev->name, &mic_config);
	if (ret) {
		LOG_I("ERROR: No mic-config found for dev '%s'!\n", dev->name);
		return ret;
	}

	if (mic_config.type == MICROPHONE_TYPE_DIGITAL) {
		LOG_I("Device MIC actice (mic_type = MICROPHONE_TYPE_DIGITAL, nch = %d).", mic_config.n_mics);
		ADSel_slot0 = 0; // AD_OUT2 (DMic 1)
		ADSel_slot1 = 1; // AD_OUT2 (DMic 2)
		audio_hal_alsa_set_control("AD 1 Select Capture Route", 0, 1); // DMic 1
		if (mic_config.n_mics > 1)
			audio_hal_alsa_set_control("AD 2 Select Capture Route", 0, 1); // DMic 2
	} else {
		LOG_I("Device MIC active (mic_type = MICROPHONE_TYPE_ANALOG, nch = %d).", mic_config.n_mics);

		// Left channel
		switch (mic_config.mics[0]) {
		case MIC1A:
			ADSel_slot0 = 2; // AD_OUT3 (Mic 1)
			audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 0); // Mic 1
			audio_hal_alsa_set_control("Mic 1A or 1B Select Capture Route", 0, 0); // Mic 1A
			break;
		case MIC1B:
			ADSel_slot0 = 2; // AD_OUT3 (Mic 1)
			audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 0); // Mic 1
			audio_hal_alsa_set_control("Mic 1A or 1B Select Capture Route", 0, 1); // Mic 1B
			break;
		case MIC2:
			ADSel_slot0 = 1; // AD_OUT2 (Mic 2)
			audio_hal_alsa_set_control("AD 2 Select Capture Route", 0, 0); // LineIn (Mic 2)
			audio_hal_alsa_set_control("Mic or LIN Select Capture Route", 0, 0); // Mic 2
			break;
		default:
			return ERR_GENERIC;
		}
		LOG_I("Left channel: %s", Alsactrl_Hwh_GetMicStr(mic_config.mics[0]));

		// Right channel
		if (mic_config.n_mics > 1) {
			switch (mic_config.mics[1]) {
			case MIC1A:
				ADSel_slot1 = 2; // AD_OUT3 (Mic 1)
				audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 0); // Mic 1
				audio_hal_alsa_set_control("Mic 1A or 1B Select Capture Route", 0, 0); // Mic 1A
				break;
			case MIC1B:
				ADSel_slot1 = 2; // AD_OUT3 (Mic 1)
				audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 0); // Mic 1
				audio_hal_alsa_set_control("Mic 1A or 1B Select Capture Route", 0, 1); // Mic 1B
				break;
			case MIC2:
				ADSel_slot1 = 1; // AD_OUT2 (Mic 2)
				audio_hal_alsa_set_control("AD 2 Select Capture Route", 0, 0); // LineIn (Mic 2)
				audio_hal_alsa_set_control("Mic or LIN Select Capture Route", 0, 0); // Mic 2
				break;
			default:
				return ERR_GENERIC;
			}
			LOG_I("Right channel: %s", Alsactrl_Hwh_GetMicStr(mic_config.mics[1]));
		} else
			ADSel_slot1 = ADSel_slot0;
	}

	audio_hal_alsa_set_control("Digital Interface AD To Slot 0 Map", 0, ADSel_slot0);
	audio_hal_alsa_set_control("Digital Interface AD To Slot 1 Map", 0, ADSel_slot1);

	return 0;
}

static int HandleDevHSetIn(u85xx_device_t *dev, sqlite3* db_p)
{
	int ADSel_slot0;
	int ret;
	mic_config_t mic_config;

	LOG_I("Enter.");

	ret = Alsactrl_DB_GetMicConfig_Generic(db_p, dev->name, &mic_config);
	if (ret) {
		LOG_I("ERROR: No mic-config found for dev '%s'!\n", dev->name);
		return ret;
	}

	if (mic_config.n_mics == 0)
	{
		LOG_I("ERROR: Unable to get mic-config for '%s'!\n", ALSACTRL_DEVSTR_HSIN);
		return ERR_GENERIC;
	}

	// Left channel
	switch (mic_config.mics[0]) {
	case MIC1A:
		ADSel_slot0 = 2; // AD_OUT3 (Mic 1)
		audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 0); // Mic 1
		audio_hal_alsa_set_control("Mic 1A or 1B Select Capture Route", 0, 0); // Mic 1A
		break;
	case MIC1B:
		ADSel_slot0 = 2; // AD_OUT3 (Mic 1)
		audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 0); // Mic 1
		audio_hal_alsa_set_control("Mic 1A or 1B Select Capture Route", 0, 1); // Mic 1B
		break;
	default:
		return ERR_GENERIC;
	}
	LOG_I("Left channel: %s", Alsactrl_Hwh_GetMicStr(mic_config.mics[0]));

	audio_hal_alsa_set_control("Digital Interface AD To Slot 0 Map", 0, ADSel_slot0);

	return 0;
}

static int HandleDevFmrx(u85xx_device_t *dev, sqlite3* db_p)
{
	UNUSED_PAR(dev);
	UNUSED_PAR(db_p);
	configure_fm_t config;

	LOG_I("Enter.");

	if (FMRX_TYPE == FM_TYPE_DIGITAL) { // FM is connected to digtal IF1 on AB8540
		LOG_I("FMRx digital active! Setting DA-from-slot and AD-to-slot mapping...");

		config.type = AUDIO_HAL_DIGITAL;

		audio_hal_alsa_set_control("Digital Interface AD 5 Loopback Switch", 0, 1);
		audio_hal_alsa_set_control("Digital Interface AD 6 Loopback Switch", 0, 1);
		audio_hal_alsa_set_control("Digital Interface AD To Slot 6 Map", 0, 4); // REG_ADSLOTSELX_AD_OUT5_TO_SLOT_EVEN
		audio_hal_alsa_set_control("Digital Interface AD To Slot 7 Map", 0, 5); // REG_ADSLOTSELX_AD_OUT6_TO_SLOT_ODD
		audio_hal_alsa_set_control("Digital Interface DA 7 From Slot Map", 0, 24); // Slot 24 -> DA_IN7
		audio_hal_alsa_set_control("Digital Interface DA 8 From Slot Map", 0, 25); // Slot 25 -> DA_IN8
	} else { // FM is connected to LineIn analog input on AB8540
		LOG_I("FMRx analog active! FM is setup as a D2D-device.");

		config.type = AUDIO_HAL_ANALOG;

		audio_hal_alsa_set_control("Digital Interface AD 5 Loopback Switch", 0, 0);
		audio_hal_alsa_set_control("Digital Interface AD 6 Loopback Switch", 0, 0);
	}
	audio_hal_configure_channel(AUDIO_HAL_CHANNEL_FMRX, &config);

	return 0;
}

static int HandleDevLinein(u85xx_device_t *dev, sqlite3* db_p)
{
	UNUSED_PAR(dev);
	UNUSED_PAR(db_p);

	LOG_I("Enter.");

	audio_hal_alsa_set_control("AD 1 Voice RX1 Select Capture Route", 0, 0); // AD1
	audio_hal_alsa_set_control("AD 4 Voice RX1 Select Capture Route", 0, 0); // AD4

	return 0;
}

static int HandleDevModemDL(u85xx_device_t *dev, sqlite3* db_p)
{
	UNUSED_PAR(dev);
	UNUSED_PAR(db_p);

	LOG_I("Enter.");

	audio_hal_alsa_set_control("Voice Interface RX1 From Slot Map", 0, 8); // RX1 -> Slot 40

	audio_hal_alsa_set_control("AD 1 Voice RX1 Select Capture Route", 0, 1); // RX1
	audio_hal_alsa_set_control("AD 4 Voice RX1 Select Capture Route", 0, 1); // RX1

	return 0;
}

static int HandleDevModemUL(u85xx_device_t *dev, sqlite3* db_p)
{
	UNUSED_PAR(dev);
	UNUSED_PAR(db_p);

	LOG_I("Enter.");

	audio_hal_alsa_set_control("Voice Interface TX To Slot 32 Map", 0, 0); // Slot 32 -> TX1
	audio_hal_alsa_set_control("Voice Interface TX To Slot 33 Map", 0, 1); // Slot 33 -> TX2
	audio_hal_alsa_set_control("Voice Interface TX To Slot 34 Map", 0, 2); // Slot 34 -> TX3
	audio_hal_alsa_set_control("Voice Interface TX To Slot 35 Map", 0, 3); // Slot 35 -> TX4

	return 0;
}

static bool ChipHasDev(enum audio_hal_chip_id_t chip_id, enum u8540_device_idx dev_id)
{
	switch (chip_id) {
	case CHIP_ID_AB8540_V1:
	case CHIP_ID_AB8540_V2:
		return ((dev_id == U8540_DEV_HSOUT) ||
			(dev_id == U8540_DEV_SPEAKER) ||
			(dev_id == U8540_DEV_EARP) ||
			(dev_id == U8540_DEV_HSIN) ||
			(dev_id == U8540_DEV_MIC) ||
			(dev_id == U8540_DEV_LINEIN) ||
			(dev_id == U8540_DEV_FMTX) ||
			(dev_id == U8540_DEV_FMRX) ||
			(dev_id == U8540_DEV_VIBL) ||
			(dev_id == U8540_DEV_LINEOUT) ||
			(dev_id == U8540_DEV_CARKITOUT) ||
			(dev_id == U8540_DEV_AUSBOUT) ||
			(dev_id == U8540_DEV_EPWM1) ||
			(dev_id == U8540_DEV_EPWM2) ||
			(dev_id == U8540_DEV_PDM1) ||
			(dev_id == U8540_DEV_PDM2) ||
			(dev_id == U8540_DEV_MODEMDL) ||
			(dev_id == U8540_DEV_MODEMUL));
	default:
		return false;
	}
}

static bool ChipHasSwitch(enum audio_hal_chip_id_t chip_id, enum u8540_switch_idx sw_id)
{
	switch (chip_id) {
	case CHIP_ID_AB8540_V1:
	case CHIP_ID_AB8540_V2:
		return 	((sw_id == U8540_SW_HEADSET_L) ||
			(sw_id == U8540_SW_HEADSET_R) ||
/*
 * To be enabled when implementation exists in kernel driver
 *			(sw_id == U8540_SW_IHF_L) ||
 *			(sw_id == U8540_SW_IHF_R) ||
 */
			(sw_id == U8540_SW_EARPIECE) ||
			(sw_id == U8540_SW_LINEOUT_L) ||
			(sw_id == U8540_SW_LINEOUT_R) ||
			(sw_id == U8540_SW_AMIC_1) ||
			(sw_id == U8540_SW_AMIC_2) ||
			(sw_id == U8540_SW_LINEIN_L) ||
			(sw_id == U8540_SW_LINEIN_R) ||
/*
 * To be enabled when implementation exists in kernel driver
 *			(sw_id == U8540_SW_VIBRA_1) ||
 *			(sw_id == U8540_SW_CARKIT_L) ||
 *			(sw_id == U8540_SW_CARKIT_R) ||
 *			(sw_id == U8540_SW_AUSB_L) ||
 *			(sw_id == U8540_SW_AUSB_R) ||
 *			(sw_id == U8540_SW_EPWM_1) ||
 *			(sw_id == U8540_SW_EPWM_2) ||
 *			(sw_id == U8540_SW_PDM_1) ||
 *			(sw_id == U8540_SW_PDM_2) ||
*/
			(sw_id == U8540_SW_VOICE_BCLK) ||
			(sw_id == U8540_SW_VOICE_MAST_GEN) ||
			(sw_id == U8540_SW_VOICE_MODEMDL) ||
			(sw_id == U8540_SW_VOICE_MODEMUL));
	default:
		return false;
	}
}

static u85xx_device_t u8540_devices[] = {
	{
		.id = U8540_DEV_HSOUT,
		.name = ALSACTRL_DEVSTR_HSOUT,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_SPEAKER,
		.name = ALSACTRL_DEVSTR_SPEAKER,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_EARP,
		.name = ALSACTRL_DEVSTR_EARP,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_HSIN,
		.name = ALSACTRL_DEVSTR_HSIN,
		.direction = DIRECTION_IN,
		.device_handler = HandleDevHSetIn,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_MIC,
		.name = ALSACTRL_DEVSTR_MIC,
		.direction = DIRECTION_IN,
		.device_handler = HandleDevMic,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_LINEIN,
		.name = ALSACTRL_DEVSTR_LINEIN,
		.direction = DIRECTION_IN,
		.device_handler = HandleDevLinein,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_FMTX,
		.name = ALSACTRL_DEVSTR_FMTX,
		.direction = DIRECTION_OUT,
		.device_handler = HandleDevFmtx,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_FMRX,
		.name = ALSACTRL_DEVSTR_FMRX,
		.direction = DIRECTION_IN,
		.device_handler = HandleDevFmrx,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_LINEOUT,
		.name = ALSACTRL_DEVSTR_LINEOUT,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_CARKITOUT,
		.name = ALSACTRL_DEVSTR_CARKITOUT,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_AUSBOUT,
		.name = ALSACTRL_DEVSTR_AUSBOUT,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_EPWM1,
		.name = ALSACTRL_DEVSTR_EPWM1,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_EPWM2,
		.name = ALSACTRL_DEVSTR_EPWM2,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_PDM1,
		.name = ALSACTRL_DEVSTR_PDM1,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_PDM2,
		.name = ALSACTRL_DEVSTR_PDM2,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
		},
	{
		.id = U8540_DEV_VIBL,
		.name = ALSACTRL_DEVSTR_VIBL,
		.direction = DIRECTION_OUT,
		.device_handler = NULL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_MODEMDL,
		.name = ALSACTRL_DEVSTR_MODEMDL,
		.direction = DIRECTION_IN,
		.device_handler = HandleDevModemDL,
		.set_state = NULL,
	},
	{
		.id = U8540_DEV_MODEMUL,
		.name = ALSACTRL_DEVSTR_MODEMUL,
		.direction = DIRECTION_OUT,
		.device_handler = HandleDevModemUL,
		.set_state = NULL,
	},
};

static int HSetOut_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_HSOUT].active);
}

static int IHF_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_SPEAKER].active);
}

static int Earpiece_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_EARP].active);
}

static int LineOut_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_LINEOUT].active);
}

static int Carkit_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_CARKITOUT].active);
}

static int AUSB_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_AUSBOUT].active);
}

static int EPWM1_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_EPWM1].active);
}

static int EPWM2_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_EPWM2].active);
}

static int PDM1_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_PDM1].active);
}

static int PDM2_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_PDM2].active);
}

static int Vibra1_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_VIBL].active);
}

static int Mic1_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	unsigned int i;
	bool enable = false;
	mic_config_t mic_config;
	int ret;

	if (devices[U8540_DEV_HSIN].active) {
		ret = Alsactrl_DB_GetMicConfig_Generic(db_p, devices[U8540_DEV_MIC].name, &mic_config);
		if (ret < 0) {
			LOG_E("Unable to get mic-config for '%s'!", devices[U8540_DEV_MIC].name);
			return ret;
		}
		if ((mic_config.mics[0] == MIC1A) || (mic_config.mics[0] == MIC1B))
			enable = true;
	}

	if (devices[U8540_DEV_MIC].active) {
		ret = Alsactrl_DB_GetMicConfig_Generic(db_p, devices[U8540_DEV_HSIN].name, &mic_config);
		if (ret < 0) {
			LOG_E("Unable to get mic-config for '%s'!", devices[U8540_DEV_HSIN].name);
			return ret;
		}
		for (i = 0; i < mic_config.n_mics; i++)
			if ((mic_config.mics[i] == MIC1A) || (mic_config.mics[i] == MIC1B))
				enable = true;
	}

	return SetSwitch(sw, enable);
}

static int Mic2_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	unsigned int i;
	bool enable = false;
	mic_config_t mic_config;
	int ret;

	if (devices[U8540_DEV_HSIN].active) {
		ret = Alsactrl_DB_GetMicConfig_Generic(db_p, devices[U8540_DEV_MIC].name, &mic_config);
		if (ret < 0) {
			LOG_E("Unable to get mic-config for '%s'!", devices[U8540_DEV_MIC].name);
			return ret;
		}
		if (mic_config.mics[0] == MIC2)
			enable = true;
	}

	if (devices[U8540_DEV_MIC].active) {
		ret = Alsactrl_DB_GetMicConfig_Generic(db_p, devices[U8540_DEV_HSIN].name, &mic_config);
		if (ret < 0) {
			LOG_E("Unable to get mic-config for '%s'!", devices[U8540_DEV_HSIN].name);
			return ret;
		}
		for (i = 0; i < mic_config.n_mics; i++)
			if (mic_config.mics[i] == MIC2)
				enable = true;
	}

	return SetSwitch(sw, enable);
}

static int LineIn_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_LINEIN].active);
}

static int VoiceBitClock_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_MODEMDL].active || devices[U8540_DEV_MODEMUL].active);
}

static int VoiceMasterGenerator_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(db_p);

	return SetSwitch(sw, devices[U8540_DEV_MODEMDL].active || devices[U8540_DEV_MODEMUL].active);
}

static int VoiceModemDL_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(sw);
	UNUSED_PAR(db_p);
	alsactrl_dev_info_t* alsa_dev;
	int ret;

	LOG_I("Enter.");

	alsa_dev = Alsactrl_Hwh_GetDevInfo(ALSACTRL_ALSA_DEVICE_AB8540_VC, PCM_IN);
	if (alsa_dev == NULL) {
		LOG_E("ERROR: Failed to get alsa device info for ab8540 voice capture!");
		return -1;
	}

	if (devices[U8540_DEV_MODEMDL].active && !alsa_dev->opened) {
		ret = Alsactrl_Hwh_OpenAlsaDev(alsa_dev);
		if (ret < 0) {
			LOG_E("ERROR: Failed to open alsa device '%s' (%d)!", alsa_dev->dev_name, ret);
			return ret;
		}
	} else if (!devices[U8540_DEV_MODEMDL].active && alsa_dev->opened) {
		Alsactrl_Hwh_CloseAlsaDev(alsa_dev);
	}

	return 0;
}

static int VoiceModemUL_SetEnabled(u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p)
{
	UNUSED_PAR(sw);
	UNUSED_PAR(db_p);
	alsactrl_dev_info_t* alsa_dev;
	int ret;

	LOG_I("Enter.");

	alsa_dev = Alsactrl_Hwh_GetDevInfo(ALSACTRL_ALSA_DEVICE_AB8540_VC, PCM_OUT);
	if (alsa_dev == NULL) {
		LOG_E("ERROR: Failed to get alsa device info for ab8540 voice playback!");
		return -1;
	}

	if (devices[U8540_DEV_MODEMUL].active && !alsa_dev->opened) {
		ret = Alsactrl_Hwh_OpenAlsaDev(alsa_dev);
		if (ret < 0) {
			LOG_E("ERROR: Failed to open alsa device '%s' (%d)!", alsa_dev->dev_name, ret);
			return ret;
		}
	} else if (!devices[U8540_DEV_MODEMUL].active && alsa_dev->opened) {
		Alsactrl_Hwh_CloseAlsaDev(alsa_dev);
	}

	return 0;
}

static u85xx_switch_t u8540_switches[] = {
	{
		.id = U8540_SW_HEADSET_L,
		.name = ALSACTRL_SWSTR_HEADSET_L,
		.set_enabled = HSetOut_SetEnabled,
	},
	{
		.id = U8540_SW_HEADSET_R,
		.name = ALSACTRL_SWSTR_HEADSET_R,
		.set_enabled = HSetOut_SetEnabled,
	},
	{
		.id = U8540_SW_IHF_L,
		.name = ALSACTRL_SWSTR_IHF_L,
		.set_enabled = IHF_SetEnabled,
	},
	{
		.id = U8540_SW_IHF_R,
		.name = ALSACTRL_SWSTR_IHF_R,
		.set_enabled = IHF_SetEnabled,
	},
	{
		.id = U8540_SW_EARPIECE,
		.name = ALSACTRL_SWSTR_EARPIECE,
		.set_enabled = Earpiece_SetEnabled,
	},
	{
		.id = U8540_SW_LINEOUT_L,
		.name = ALSACTRL_SWSTR_LINEOUT_L,
		.set_enabled = LineOut_SetEnabled,
	},
	{
		.id = U8540_SW_LINEOUT_R,
		.name = ALSACTRL_SWSTR_LINEOUT_R,
		.set_enabled = LineOut_SetEnabled,
	},
	{
		.id = U8540_SW_AMIC_1,
		.name = ALSACTRL_SWSTR_AMIC_1,
		.set_enabled = Mic1_SetEnabled,
	},
	{
		.id = U8540_SW_AMIC_2,
		.name = ALSACTRL_SWSTR_AMIC_2,
		.set_enabled = Mic2_SetEnabled,
	},
	{
		.id = U8540_SW_LINEIN_L,
		.name = ALSACTRL_SWSTR_LINEIN_L,
		.set_enabled = LineIn_SetEnabled,
	},
	{
		.id = U8540_SW_LINEIN_R,
		.name = ALSACTRL_SWSTR_LINEIN_R,
		.set_enabled = LineIn_SetEnabled,
	},
	{
		.id = U8540_SW_CARKIT_L,
		.name = ALSACTRL_SWSTR_CARKIT_L,
		.set_enabled = Carkit_SetEnabled,
	},
	{
		.id = U8540_SW_CARKIT_R,
		.name = ALSACTRL_SWSTR_CARKIT_R,
		.set_enabled = Carkit_SetEnabled,
	},
	{
		.id = U8540_SW_AUSB_L,
		.name = ALSACTRL_SWSTR_AUSB_L,
		.set_enabled = AUSB_SetEnabled,
	},
	{
		.id = U8540_SW_AUSB_R,
		.name = ALSACTRL_SWSTR_AUSB_R,
		.set_enabled = AUSB_SetEnabled,
	},
	{
		.id = U8540_SW_EPWM_1,
		.name = ALSACTRL_SWSTR_EPWM_1,
		.set_enabled = EPWM1_SetEnabled,
	},
	{
		.id = U8540_SW_EPWM_2,
		.name = ALSACTRL_SWSTR_EPWM_2,
		.set_enabled = EPWM2_SetEnabled,
	},
	{
		.id = U8540_SW_PDM_1,
		.name = ALSACTRL_SWSTR_PDM_1,
		.set_enabled = PDM1_SetEnabled,
	},
	{
		.id = U8540_SW_PDM_2,
		.name = ALSACTRL_SWSTR_PDM_2,
		.set_enabled = PDM2_SetEnabled,
	},
	{
		.id = U8540_SW_VIBRA_1,
		.name = ALSACTRL_SWSTR_VIBRA_1,
		.set_enabled = Vibra1_SetEnabled,
	},
	{
		.id = U8540_SW_VOICE_BCLK,
		.name = ALSACTRL_SWSTR_VOICE_BCLK,
		.set_enabled = VoiceBitClock_SetEnabled,
	},
	{
		.id = U8540_SW_VOICE_MAST_GEN,
		.name = ALSACTRL_SWSTR_VOICE_MAST_GEN,
		.set_enabled = VoiceMasterGenerator_SetEnabled,
	},
	{
		.id = U8540_SW_VOICE_MODEMDL,
		.name = ALSACTRL_SWSTR_VOICE_MODEMDL,
		.set_enabled = VoiceModemDL_SetEnabled,
	},
	{
		.id = U8540_SW_VOICE_MODEMUL,
		.name = ALSACTRL_SWSTR_VOICE_MODEMUL,
		.set_enabled = VoiceModemUL_SetEnabled,
	},
};

static void UpdateActiveDev(hwh_dev_next_t dev_next)
{
	const char* name = NULL;
	unsigned int i;

	while (dev_next(&name) == 0) {
		for (i = 0; i < ARRAY_SIZE(u8540_devices); i++) {
			if (strcmp(name, u8540_devices[i].name) == 0) {
				SetActive(&u8540_devices[i], true);
				break;
			}
		}
	}
}

static void ClearActiveDevs(void)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(u8540_devices); i++)
		u8540_devices[i].active = false;
}

// Public interface

int Alsactrl_Hwh_U8540_Dev_UpdateSwitches(sqlite3* db_p, enum audio_hal_chip_id_t chip_id)
{
	int ret = 0;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(u8540_switches); i++)
		if ((u8540_switches[i].set_enabled) && ChipHasSwitch(chip_id, u8540_switches[i].id))
			ret |= u8540_switches[i].set_enabled(&u8540_switches[i], u8540_devices, db_p);

	return ret;
}

u85xx_device_t* Alsactrl_Hwh_U8540_Dev_GetDevs(void)
{
	return u8540_devices;
}

int Alsactrl_Hwh_U8540_Dev(sqlite3* db_p, hwh_dev_next_t dev_next, enum audio_hal_chip_id_t chip_id)
{
	int ret;
	unsigned int i;

	LOG_I("Enter.");

	ClearActiveDevs();

	// Update active-flags
	UpdateActiveDev(dev_next);

	/* Call devices-handlers for active devices */
	for (i = 0; i < ARRAY_SIZE(u8540_devices); i++)
		if (ChipHasDev(chip_id, u8540_devices[i].id) && (u8540_devices[i].device_handler) && (u8540_devices[i].active)) {
			LOG_I("Calling device-handler (dev = '%s').", u8540_devices[i].name);
			ret = u8540_devices[i].device_handler(&u8540_devices[i], db_p);
			if (ret < 0) {
				LOG_E("Error in device-handler (dev = '%s')!", u8540_devices[i].name);
				return ret;
			}
		}

	LOG_I("Setting DA from Slot mapping...");
	audio_hal_alsa_set_control("Digital Interface DA 1 From Slot Map", 0, 8); // Slot 8 -> DA_IN1
	audio_hal_alsa_set_control("Digital Interface DA 2 From Slot Map", 0, 9); // Slot 9 -> DA_IN2
	audio_hal_alsa_set_control("Digital Interface DA 3 From Slot Map", 0, 10); // Slot 10 -> DA_IN3
	audio_hal_alsa_set_control("Digital Interface DA 4 From Slot Map", 0, 11); // Slot 11 -> DA_IN4
	audio_hal_alsa_set_control("Digital Interface DA 5 From Slot Map", 0, 12); // Slot 12 -> DA_IN5
	audio_hal_alsa_set_control("Digital Interface DA 6 From Slot Map", 0, 13); // Slot 13 -> DA_IN6

	LOG_I("Setting Voice TX to Slot mapping...");
	audio_hal_alsa_set_control("Voice Interface TX To Slot 36 Map", 0, 12); // Slot 36 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 37 Map", 0, 12); // Slot 37 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 38 Map", 0, 12); // Slot 38 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 39 Map", 0, 12); // Slot 39 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 40 Map", 0, 12); // Slot 40 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 41 Map", 0, 12); // Slot 41 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 42 Map", 0, 12); // Slot 42 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 43 Map", 0, 12); // Slot 43 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 44 Map", 0, 12); // Slot 44 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 45 Map", 0, 12); // Slot 45 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 46 Map", 0, 12); // Slot 46 -> tristate
	audio_hal_alsa_set_control("Voice Interface TX To Slot 47 Map", 0, 12); // Slot 47 -> tristate

	return 0;
}
