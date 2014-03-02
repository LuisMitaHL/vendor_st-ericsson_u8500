/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>

#include "alsactrl.h"
#include "alsactrl_audiocodec.h"
#include "alsactrl_alsa.h"
#include "alsactrl_fm.h"
#include "alsactrl_debug.h"
#include "alsactrl_tinyalsa_extn.h"
#include <tinyalsa/asoundlib.h>

#include "alsactrl_hwh.h"
#include "alsactrl_hwh_Int.h"

// AB850x fsbitclk
static int fsbitclkActive = 0;

static AUDIO_HAL_TYPE fmrx_type = AUDIO_HAL_DIGITAL;

static const char *bool_str(bool value)
{
	return (value) ? "True" : "False";
}

static const char *state_str(enum alsactrl_channel_state channel_state)
{
	return (channel_state == ALSACTRL_CHANNEL_STATE_OPEN) ? "Open" : "Close";
}

// Playback channels

static int alsactrl_channel_state_earpiece(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n", state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("Earpiece", 0, 1);
		audio_hal_alsa_set_control("Earpiece Playback Switch", 0, 1);
		audio_hal_alsa_set_control("Earpiece Analog Gain Playback Volume", 0, 6);
		audio_hal_alsa_set_control("Headset Master Gain Playback Volume", 0, 40);
		audio_hal_alsa_set_control("Headset Master Gain Playback Volume", 1, 40);
	}
	else {
		audio_hal_alsa_set_control("Earpiece", 0, 0);
		audio_hal_alsa_set_control("Earpiece Playback Switch", 0, 0);
	}

	return 0;
}

static int alsactrl_channel_state_hsetout(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n", state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("Headset Left", 0, 1);
		audio_hal_alsa_set_control("Headset Right", 0, 1);
		audio_hal_alsa_set_control("Headset Digital Gain Playback Volume", 0, 9);
		audio_hal_alsa_set_control("Headset Digital Gain Playback Volume", 1, 9);
		audio_hal_alsa_set_control("Headset Master Gain Playback Volume", 0, 40);
		audio_hal_alsa_set_control("Headset Master Gain Playback Volume", 1, 40);
	} else {
		audio_hal_alsa_set_control("Headset Left", 0, 0);
		audio_hal_alsa_set_control("Headset Right", 0, 0);
	}

	return 0;
}

static int alsactrl_channel_state_speaker(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n",state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("IHF Mode Playback Switch", 0, 0);
		audio_hal_alsa_set_control("IHF Left", 0, 1);
		audio_hal_alsa_set_control("IHF Right", 0, 1);
		audio_hal_alsa_set_control("IHF High Volume Playback Switch", 0, 1);
		audio_hal_alsa_set_control("IHF High Volume Playback Switch", 1, 1);
		audio_hal_alsa_set_control("IHF Left Source Playback Route", 0, 0);
		audio_hal_alsa_set_control("IHF Right Source Playback Route", 0, 0);
		audio_hal_alsa_set_control("IHF Master Gain Playback Volume", 0, 40);
		audio_hal_alsa_set_control("IHF Digital Gain Playback Volume", 0, 20);
		audio_hal_alsa_set_control("Digital Interface DA 3 From Slot Map", 0, 10);
		audio_hal_alsa_set_control("Digital Interface DA 4 From Slot Map", 0, 11);
		audio_hal_alsa_set_control("IHF Master Gain Playback Volume", 1, 40);
	} else {
		audio_hal_alsa_set_control("IHF Left", 0, 0);
		audio_hal_alsa_set_control("IHF Right", 0, 0);
	}

	return 0;
}

static int alsactrl_channel_state_pdmout(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n",state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("IHF Mode Playback Switch", 0, 0);
		audio_hal_alsa_set_control("PDM 1", 0, 1);
		audio_hal_alsa_set_control("PDM 2", 0, 1);
		audio_hal_alsa_set_control("IHF Left", 0, 1);
		audio_hal_alsa_set_control("IHF Right", 0, 1);
		audio_hal_alsa_set_control("IHF Left Source Playback Route", 0, 0);
		audio_hal_alsa_set_control("IHF Right Source Playback Route", 0, 0);
		audio_hal_alsa_set_control("IHF Master Gain Playback Volume", 0, 40);
		audio_hal_alsa_set_control("Digital Interface DA 3 From Slot Map", 0, 10);
		audio_hal_alsa_set_control("Digital Interface DA 4 From Slot Map", 0, 11);
		audio_hal_alsa_set_control("IHF Master Gain Playback Volume", 0, 40);
		audio_hal_alsa_set_control("IHF Master Gain Playback Volume", 1, 40);
		audio_hal_alsa_set_control("EPWM 2 GPIO Signal Source", 0, 0);
	} else {
		audio_hal_alsa_set_control("PDM 1", 0, 0);
		audio_hal_alsa_set_control("PDM 2", 0, 0);
		audio_hal_alsa_set_control("IHF Left", 0, 0);
		audio_hal_alsa_set_control("IHF Right", 0, 0);
	}

	return 0;
}

static int alsactrl_channel_state_vibral(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n", state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("Vibra Master Gain Playback Volume", 0, 63);
		audio_hal_alsa_set_control("Digital Interface DA 5 From Slot Map", 0, 12);
		audio_hal_alsa_set_control("Vibra 1 Playback Route", 0, 0);
		audio_hal_alsa_set_control("Vibra 1", 0, 1);
	} else
		audio_hal_alsa_set_control("Vibra 1", 0, 0);

	return 0;
}

static int alsactrl_channel_state_vibrar(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n", state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("Vibra Master Gain Playback Volume", 1, 63);
		audio_hal_alsa_set_control("Digital Interface DA 6 From Slot Map", 0, 13);
		audio_hal_alsa_set_control("Vibra 2 Playback Route", 0, 0);
		audio_hal_alsa_set_control("Vibra 2", 0, 1);
	} else
		audio_hal_alsa_set_control("Vibra 2", 0, 0);

	return 0;
}

static int alsactrl_channel_state_hsetin(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n", state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("Mic Master Gain Capture Volume", 0, 50);
		audio_hal_alsa_set_control("Mic Master Gain Capture Volum", 1, 50);
		audio_hal_alsa_set_control("Mic 1 Capture Volume", 0, 28);
		audio_hal_alsa_set_control("Mic 1", 0, 1);
		audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 0);
		audio_hal_alsa_set_control("Digital Interface AD To Slot 0 Map", 0, 2);
		audio_hal_alsa_set_control("Mic 1A or 1B Select Capture Route", 0, 1);
	} else
		audio_hal_alsa_set_control("Mic 1", 0, 0);

	return 0;
}

static int alsactrl_channel_state_mic(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n", state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("Mic Master Gain Capture Volume", 0, 50);
		audio_hal_alsa_set_control("Mic Master Gain Capture Volume", 1, 50);
		audio_hal_alsa_set_control("LineIn Master Gain Capture Volume", 1, 50);
		audio_hal_alsa_set_control("Mic 1 Capture Volume", 0, 28);
		audio_hal_alsa_set_control("Mic 2 Capture Volume", 0, 28);
		audio_hal_alsa_set_control("Mic 1", 0, 1);
		audio_hal_alsa_set_control("Mic 2", 0, 1);
		audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 0);
		audio_hal_alsa_set_control("AD 2 Select Capture Route", 0, 0);
		audio_hal_alsa_set_control("Mic 2 or LINR Select Capture Route", 0, 0);
		audio_hal_alsa_set_control("Digital Interface AD To Slot 0 Map", 0, 2);
		audio_hal_alsa_set_control("Digital Interface AD To Slot 1 Map", 0, 1);
		audio_hal_alsa_set_control("Mic 1A or 1B Select Capture Route", 0, 0);
	} else {
		audio_hal_alsa_set_control("Mic 1", 0, 0);
		audio_hal_alsa_set_control("Mic 2", 0, 0);
	}

	return 0;
}

static int alsactrl_channel_state_digmic12(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n",state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("Digital Interface AD To Slot 4 Map", 0, 0);
		audio_hal_alsa_set_control("Digital Interface AD To Slot 5 Map", 0, 1);
		audio_hal_alsa_set_control("AD 1 Select Capture Route", 0, 1);
		audio_hal_alsa_set_control("AD 2 Select Capture Route", 0, 1);
		audio_hal_alsa_set_control("LineIn Master Gain Capture Volume", 0, 32);
		audio_hal_alsa_set_control("LineIn Master Gain Capture Volume", 1, 32);
		audio_hal_alsa_set_control("DMic 1 and 2 Frequency", 0, 2);
		audio_hal_alsa_set_control("DMic 1", 0, 1);
		audio_hal_alsa_set_control("DMic 2", 0, 1);
	} else {
		audio_hal_alsa_set_control("DMic 1", 0, 0);
		audio_hal_alsa_set_control("DMic 2", 0, 0);
	}

	return 0;
}

static int alsactrl_channel_state_digmic34(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n",state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("Digital Interface AD To Slot 4 Map", 0, 2);
		audio_hal_alsa_set_control("Digital Interface AD To Slot 5 Map", 0, 3);
		audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 1);
		audio_hal_alsa_set_control("Mic Master Gain Capture Volume", 0, 32);
		audio_hal_alsa_set_control("Mic Master Gain Capture Volume", 1, 32);
		audio_hal_alsa_set_control("DMic 3 and 4 Frequency", 0, 2);
		audio_hal_alsa_set_control("DMic 3", 0, 1);
		audio_hal_alsa_set_control("DMic 4", 0, 1);
	} else {
		audio_hal_alsa_set_control("DMic 3", 0, 0);
		audio_hal_alsa_set_control("DMic 4", 0, 0);
	}

	return 0;
}

static int alsactrl_channel_state_digmic56(enum alsactrl_channel_state state)
{
	LOG_I("Enter (state = %s).\n",state_str(state));

	if (state == ALSACTRL_CHANNEL_STATE_OPEN) {
		audio_hal_alsa_set_control("Digital Interface AD To Slot 4 Map", 0, 6);
		audio_hal_alsa_set_control("Digital Interface AD To Slot 5 Map", 0, 7);
		audio_hal_alsa_set_control("AD 5 Select Capture Route", 0, 1);
		audio_hal_alsa_set_control("AD 6 Select Capture Route", 0, 1);
		audio_hal_alsa_set_control("HD Mic Master Gain Capture Volume", 0, 32);
		audio_hal_alsa_set_control("HD Mic Master Gain Capture Volume", 1, 32);
		audio_hal_alsa_set_control("DMic 5 and 6 Frequency", 0, 2);
		audio_hal_alsa_set_control("DMic 5", 0, 1);
		audio_hal_alsa_set_control("DMic 6", 0, 1);
	} else {
		audio_hal_alsa_set_control("DMic 5", 0, 0);
		audio_hal_alsa_set_control("DMic 6", 0, 0);
	}

	return 0;
}

static int alsactrl_channel_state_fmrx(enum alsactrl_channel_state state)
{
	UNUSED_PAR(state);

	LOG_I("Enter (state = %s).\n", state_str(state));

	return 0;
}

static int alsactrl_channel_state_fmtx(enum alsactrl_channel_state state)
{
	UNUSED_PAR(state);

	LOG_I("Enter (state = %s).\n", state_str(state));

	return 0;
}

static int alsactrl_channel_state_btin(enum alsactrl_channel_state state)
{
	UNUSED_PAR(state);

	LOG_I("Enter (state = %s).\n", state_str(state));

	return 0;
}

static int alsactrl_channel_state_btout(enum alsactrl_channel_state state)
{
	UNUSED_PAR(state);

	LOG_I("Enter (state = %s).\n", state_str(state));

	return 0;
}

static int alsactrl_channel_state(audio_hal_channel channel,
				enum alsactrl_channel_state state)
{
	int ret;

	Alsactrl_Hwh_OpenControls();

	// Write ALSA-controls for specific channel
	switch (channel) {
	case AUDIO_HAL_CHANNEL_EARPIECE:
		ret = alsactrl_channel_state_earpiece(state);
		break;
	case AUDIO_HAL_CHANNEL_HSETOUT:
		ret = alsactrl_channel_state_hsetout(state);
		break;
	case AUDIO_HAL_CHANNEL_SPEAKER:
		ret = alsactrl_channel_state_speaker(state);
		break;
	case AUDIO_HAL_CHANNEL_VIBRAL:
		ret = alsactrl_channel_state_vibral(state);
		break;
	case AUDIO_HAL_CHANNEL_VIBRAR:
		ret = alsactrl_channel_state_vibrar(state);
		break;
	case AUDIO_HAL_CHANNEL_HSETIN:
		ret = alsactrl_channel_state_hsetin(state);
		break;
	case AUDIO_HAL_CHANNEL_MIC:
		ret = alsactrl_channel_state_mic(state);
		break;
	case AUDIO_HAL_CHANNEL_FMTX:
		ret = alsactrl_channel_state_fmtx(state);
		break;
	case AUDIO_HAL_CHANNEL_FMRX:
		ret = alsactrl_channel_state_fmrx(state);
		break;
	case AUDIO_HAL_CHANNEL_BTIN:
		ret = alsactrl_channel_state_btin(state);
		break;
	case AUDIO_HAL_CHANNEL_BTOUT:
		ret = alsactrl_channel_state_btout(state);
		break;
	case AUDIO_HAL_CHANNEL_PDMOUT:
		ret = alsactrl_channel_state_pdmout(state);
		break;
	case AUDIO_HAL_CHANNEL_DIGMIC12:
		ret = alsactrl_channel_state_digmic12(state);
		break;
	case AUDIO_HAL_CHANNEL_DIGMIC34:
		ret = alsactrl_channel_state_digmic34(state);
		break;
	case AUDIO_HAL_CHANNEL_DIGMIC56:
		ret = alsactrl_channel_state_digmic56(state);
		break;
	default:
		LOG_E("ERROR: Unknown channel type (%d)!", channel);
		return ERR_INVPAR;
	}

	Alsactrl_Hwh_CloseControls();

	return (ret < 0) ? ERR_GENERIC : 0;
}

static int alsactrl_set_channel_state(alsactrl_dev_info_t* dev, enum alsactrl_channel_state channel_state, bool matchPeer)
{
	int ret;

	alsactrl_dev_info_t* dev_peer = Alsactrl_Hwh_GetDevInfo_Peer(dev);

	if (channel_state == ALSACTRL_CHANNEL_STATE_OPEN) {
		if (!dev->opened) {
			ret = Alsactrl_Hwh_OpenAlsaDev(dev);
			if (ret < 0) {
				LOG_E("ERROR: Failed to open alsa device '%s' (%d)!", dev->name, ret);
				return ret;
			}
		}
		if (matchPeer)
			if (!dev_peer->opened) {
				ret = Alsactrl_Hwh_OpenAlsaDev(dev_peer);
				if (ret < 0) {
					LOG_E("ERROR: Failed to open alsa device '%s' (%d)!", dev_peer->name, ret);
					return ret;
				}
			}
	} else {
		if (matchPeer) {
			if (dev->active == 0 && dev_peer->active == 0) {
				Alsactrl_Hwh_CloseAlsaDev(dev);
				Alsactrl_Hwh_CloseAlsaDev(dev_peer);
			}
		} else {
			if (dev->active == 0)
				Alsactrl_Hwh_CloseAlsaDev(dev);
		}
	}

	return 0;
}

static int alsactrl_change_active_count(alsactrl_dev_info_t* dev_info_p, enum alsactrl_channel_state channel_state)
{
	int ret, valueOld;
	alsactrl_dev_info_t* dev_info_peer_p = Alsactrl_Hwh_GetDevInfo_Peer(dev_info_p);

	valueOld = dev_info_p->active;
	if (channel_state == ALSACTRL_CHANNEL_STATE_OPEN)
		dev_info_p->active++;
	else
		dev_info_p->active--;

	LOG_I("Active count for '%s' changed from %d to %d\n", dev_info_p->name, valueOld, dev_info_p->active);

	if (dev_info_p->alsa_dev == ALSACTRL_ALSA_DEVICE_AB850x || dev_info_p->alsa_dev == ALSACTRL_ALSA_DEVICE_AB8540) {
		ret = alsactrl_set_channel_state(dev_info_p, channel_state, true); // Match peer-device
		if (dev_info_p->active == 0 && dev_info_peer_p->active == 0 && channel_state == ALSACTRL_CHANNEL_STATE_CLOSE && fsbitclkActive) {
			LOG_I("Calling audio_hal_stop_fsbitclk().\n");
			audio_hal_stop_fsbitclk();
		}
	} else if (dev_info_p->alsa_dev == ALSACTRL_ALSA_DEVICE_CG29XX)
		ret = alsactrl_set_channel_state(dev_info_p, channel_state, true); // Match peer-device
	else
		ret = alsactrl_set_channel_state(dev_info_p, channel_state, false);

	if (ret < 0)
		dev_info_p->active = valueOld;

	return ret;
}

static alsactrl_dev_info_t* audio_hal_get_device_info(audio_hal_channel channel)
{
	enum audio_hal_chip_id_t chip_id = Alsactrl_Hwh_ChipID();

	switch (channel) {
	case AUDIO_HAL_CHANNEL_BTIN:
		return Alsactrl_Hwh_GetDevInfo(ALSACTRL_ALSA_DEVICE_CG29XX, PCM_IN);
	case AUDIO_HAL_CHANNEL_BTOUT:
		return Alsactrl_Hwh_GetDevInfo(ALSACTRL_ALSA_DEVICE_CG29XX, PCM_OUT);
	case AUDIO_HAL_CHANNEL_HSETIN:
	case AUDIO_HAL_CHANNEL_MIC:
	case AUDIO_HAL_CHANNEL_FMRX:
	case AUDIO_HAL_CHANNEL_DIGMIC12:
	case AUDIO_HAL_CHANNEL_DIGMIC34:
	case AUDIO_HAL_CHANNEL_DIGMIC56:
		switch (chip_id) {
		case CHIP_ID_AB8500:
		case CHIP_ID_AB8505_V1:
		case CHIP_ID_AB8505_V2:
		case CHIP_ID_AB8505_V3:
		case CHIP_ID_AB9540_V1:
		case CHIP_ID_AB9540_V2:
			return Alsactrl_Hwh_GetDevInfo(ALSACTRL_ALSA_DEVICE_AB850x, PCM_IN);
		case CHIP_ID_AB8540_V1:
		case CHIP_ID_AB8540_V2:
			return Alsactrl_Hwh_GetDevInfo(ALSACTRL_ALSA_DEVICE_AB8540, PCM_IN);
		default:
			LOG_E("%s: ERROR: Unknown codec chip-ID '%s'!", __func__, audio_hal_alsa_get_chip_id_str(chip_id));
			return NULL;
		};
	case AUDIO_HAL_CHANNEL_EARPIECE:
	case AUDIO_HAL_CHANNEL_HSETOUT:
	case AUDIO_HAL_CHANNEL_SPEAKER:
	case AUDIO_HAL_CHANNEL_FMTX:
	case AUDIO_HAL_CHANNEL_VIBRAL:
	case AUDIO_HAL_CHANNEL_VIBRAR:
	case AUDIO_HAL_CHANNEL_PDMOUT:
		switch (chip_id) {
		case CHIP_ID_AB8500:
		case CHIP_ID_AB8505_V1:
		case CHIP_ID_AB8505_V2:
		case CHIP_ID_AB8505_V3:
		case CHIP_ID_AB9540_V1:
		case CHIP_ID_AB9540_V2:
			return Alsactrl_Hwh_GetDevInfo(ALSACTRL_ALSA_DEVICE_AB850x, PCM_OUT);
		case CHIP_ID_AB8540_V1:
		case CHIP_ID_AB8540_V2:
			return Alsactrl_Hwh_GetDevInfo(ALSACTRL_ALSA_DEVICE_AB8540, PCM_OUT);
		default:
			LOG_E("%s: ERROR: Unknown codec chip-ID '%s'!", __func__, audio_hal_alsa_get_chip_id_str(chip_id));
			return NULL;
		};

		return 0;
	default:
		LOG_E("ERROR: No device defined on channel (%d)!", channel);
		return NULL;
	}
}

static AUDIO_HAL_STATUS audio_hal_change_channel(audio_hal_channel channel, enum alsactrl_channel_state channel_state)
{
	alsactrl_dev_info_t* dev_info_p;
	int ret;

	LOG_I("Enter (channel = %d, channel_state = %s).\n", channel, state_str(channel_state));

	dev_info_p = audio_hal_get_device_info(channel);
	if (dev_info_p == NULL) {
		LOG_E("ERROR: Unable to get device info for device on channel %d!", channel);
		return AUDIO_HAL_STATUS_UNSUPPORTED;
	}
	LOG_I("ALSA-device: %s\n", dev_info_p->dev_name);
	LOG_I("Opened: %s", bool_str(dev_info_p->opened));
	LOG_I("Active-count: %d", dev_info_p->active);

	if (audio_hal_alsa_get_ignore_defaults()) {
		LOG_I("ignore_defaults = true. Will not set default values...\n");
	} else {
		LOG_I("ignore_defaults = false. Setting default values...\n");
		ret = alsactrl_channel_state(channel, channel_state);
		if (ret < 0) {
			LOG_I("WARN: Unable to set channel state (ret = %d)!", ret);
		}
	}

	ret = alsactrl_change_active_count(dev_info_p, channel_state);

	return (ret < 0) ? AUDIO_HAL_STATUS_ERROR : AUDIO_HAL_STATUS_OK;
}

// Audio HAL-interface (Audiocodec)

AUDIO_HAL_STATUS audio_hal_open_channel(audio_hal_channel channel)
{
	return audio_hal_change_channel(channel, ALSACTRL_CHANNEL_STATE_OPEN);
}

AUDIO_HAL_STATUS audio_hal_close_channel(audio_hal_channel channel)
{
	return audio_hal_change_channel(channel, ALSACTRL_CHANNEL_STATE_CLOSE);
}

AUDIO_HAL_STATUS audio_hal_set_power(uint32 channel_index, AUDIO_HAL_STATE power_control, audio_hal_channel channel)
{
	UNUSED_PAR(channel_index);

	LOG_I("Enter (channel = %d).\n", channel);

	if (channel == AUDIO_HAL_CHANNEL_FMRX) {
		if (power_control == AUDIO_HAL_STATE_ON) {
			if (fmrx_type == AUDIO_HAL_DIGITAL) {
				setup_fm_rx_i2s();
			} else {
				int err = setup_fm_analog_out();
				LOG_I("Setup FM chip analog out : %d\n", err);
			}
		}
		else {
			if (fmrx_type == AUDIO_HAL_DIGITAL) {
				teardown_fm_i2s();
			}
		}
	} else if (channel == AUDIO_HAL_CHANNEL_FMTX) {
		if (power_control == AUDIO_HAL_STATE_ON)
			setup_i2s_fm_tx();
		else
			teardown_fm_i2s();
	}

	return AUDIO_HAL_STATUS_OK;
}

AUDIO_HAL_STATE audio_hal_get_power(uint32 channel_index, audio_hal_channel channel)
{
	UNUSED_PAR(channel_index);

	LOG_I("Enter (channel = %d).\n", channel);

	return AUDIO_HAL_STATE_ON;
}

int audio_hal_digital_mute(bool mute)
{
	int ret;

	LOG_I("Enter (mute = %d).\n", (int)mute);

	Alsactrl_Hwh_OpenControls();
	ret = audio_hal_alsa_set_control("Digital Interface Mute", 0, (int)!mute);
	Alsactrl_Hwh_CloseControls();

	return ret;
}

AUDIO_HAL_STATUS audio_hal_configure_channel(audio_hal_channel channel, void *param)
{
	configure_fm_t *config_fm = (configure_fm_t*)param;

	LOG_I("Enter (channel = %d).\n", channel);

	if (!param) {
		return AUDIO_HAL_STATUS_UNSUPPORTED;
	}

	switch (channel) {
	case AUDIO_HAL_CHANNEL_FMRX:
		break;
	default:
		return AUDIO_HAL_STATUS_UNSUPPORTED;
	}

	fmrx_type = config_fm->type;
	LOG_I("Config FMRX as %s.\n", fmrx_type == AUDIO_HAL_DIGITAL ? "DIGITAL" : "ANALOG");

	return AUDIO_HAL_STATUS_OK;
}

void audio_hal_start_fsbitclk(void)
{
	LOG_I("Enter.\n");

	fsbitclkActive++;
	LOG_I("fsbitclkActive changed from %d to %d\n", fsbitclkActive-1, fsbitclkActive);

	if (fsbitclkActive == 1) {
		Alsactrl_Hwh_OpenControls();
		audio_hal_alsa_set_control("Digital Interface Master Generator Switch",0,0);
		audio_hal_alsa_set_control("Burst FIFO Interface Mode", 0, 1); // Master
		audio_hal_alsa_set_control("Burst FIFO Interface Switch", 0, 1); // Enabled
		audio_hal_alsa_set_control("Burst FIFO Threshold", 0, 27);
		audio_hal_alsa_set_control("Burst FIFO Length", 0, 192);
		audio_hal_alsa_set_control("Burst FIFO FS Extra Bit-clocks", 0, 28); // 28 extra clocks
		audio_hal_alsa_set_control("Burst FIFO Wake Up Delay", 0, 112); // 112*26.7 = 2990.4 us
		audio_hal_alsa_set_control("Burst FIFO Switch Frame Number", 0, 0);
		audio_hal_alsa_set_control("Digital Interface 0 FIFO Enable Switch",0,0);
		audio_hal_alsa_set_control("Digital Interface 0 Bit-clock Switch", 0, 1);
		audio_hal_alsa_set_control("Digital Interface 1 Bit-clock Switch", 0, 1);
		audio_hal_alsa_set_control("Digital Interface Master Generator Switch",0,1);
		audio_hal_alsa_set_control("IHF and Headset Swap Playback Switch", 0, 0);
		Alsactrl_Hwh_CloseControls();
	}
}

void audio_hal_stop_fsbitclk(void)
{
	LOG_I("Enter.\n");

	if (fsbitclkActive == 0) {
		LOG_I("fsbitclk already stopped! Returning.\n");
		return;
	}

	fsbitclkActive--;
	LOG_I("fsbitclkActive changed from %d to %d\n", fsbitclkActive+1, fsbitclkActive);

	if (fsbitclkActive == 0) {
		Alsactrl_Hwh_OpenControls();
		audio_hal_alsa_set_control("Digital Interface 0 Bit-clock Switch", 0, 0);
		audio_hal_alsa_set_control("Digital Interface 1 Bit-clock Switch", 0, 0);
		Alsactrl_Hwh_CloseControls();
	}
}

int audio_hal_switch_to_burst(unsigned int framecount)
{
	int ret;
	LOG_I("Enter (framecount = %d).\n", framecount);

	Alsactrl_Hwh_OpenControls();
	ret = (audio_hal_alsa_set_control("Burst FIFO Switch Frame Number", 0, framecount) != 0) ||
		(audio_hal_alsa_set_control("Digital Interface 0 FIFO Enable Switch", 0, 1) != 0);
	Alsactrl_Hwh_CloseControls();

	return ret;
}

int audio_hal_switch_to_normal(void)
{
	int ret;
	LOG_I("Enter.\n");

	Alsactrl_Hwh_OpenControls();
	ret = audio_hal_alsa_set_control("Digital Interface 0 FIFO Enable Switch", 0, 0);
	Alsactrl_Hwh_CloseControls();

	return ret;
}

int audio_hal_set_burst_device(audio_hal_channel channel)
{
	int ret;
	LOG_I("Enter (channel = %d).\n", channel);
	Alsactrl_Hwh_OpenControls();
	switch (channel) {
		case AUDIO_HAL_CHANNEL_PDMOUT:
		case AUDIO_HAL_CHANNEL_SPEAKER:
			ret = audio_hal_alsa_set_control("IHF and Headset Swap Playback Switch", 0, 1);
			break;
		case AUDIO_HAL_CHANNEL_HSETOUT:
			ret = audio_hal_alsa_set_control("IHF and Headset Swap Playback Switch", 0, 0);
			break;
		default:
			ret = -1;
			break;
	}
	Alsactrl_Hwh_CloseControls();
	return ret;
}

