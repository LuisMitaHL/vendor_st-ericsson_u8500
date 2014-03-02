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
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "alsactrl.h"
#include "alsactrl_alsa.h"
#include "alsactrl_debug.h"
#include "alsactrl_statefile_parser.h"
#include "alsactrl_tinyalsa_extn.h"
#include <tinyalsa/asoundlib.h>
#include <sound/asound.h>

static struct control_config_t *main_cfg_list = NULL;
static struct control_config_t *default_cfg_list = NULL;
static struct mixer *mixer_p = NULL;
static pthread_mutex_t ctl_open_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool ignore_defaults = false;

static int alsactrl_alsa_set_control(struct mixer_ctl *ctl, long *values)
{
	enum mixer_ctl_type ctl_type;
	const char *str;
	int ret = -1, value_idx, value_count;
	LOG_I("Enter");
	ctl_type = mixer_ctl_get_type(ctl);
	value_count = mixer_ctl_get_num_values(ctl);
	switch (ctl_type) {
	case MIXER_CTL_TYPE_BOOL:
	case MIXER_CTL_TYPE_INT:
		for (value_idx = 0; value_idx < value_count; value_idx++) {
			long min, max;
			min = mixer_ctl_get_range_min(ctl);
			max = mixer_ctl_get_range_max(ctl);
			values[value_idx] = (values[value_idx] > max ? max : (values[value_idx] < min ? min : values[value_idx]));
			ret = mixer_ctl_set_value(ctl, value_idx, values[value_idx]);
			if (ret < 0) {
				LOG_E("Error: Unable to set mixer control %s for type BOOL/INT",
						mixer_ctl_get_name(ctl));
				break;
			}
		}
		break;
	case MIXER_CTL_TYPE_ENUM:
		for (value_idx = 0; value_idx < value_count; value_idx++) {
			str = mixer_ctl_get_enum_string(ctl, values[value_idx]);
			if (str) {
				ret = mixer_ctl_set_enum_by_string(ctl, str);
				if (ret < 0) {
					LOG_E("Error: Unable to set mixer control %s for type ENUM",
							mixer_ctl_get_name(ctl));
					break;
				}
			} else {
				LOG_E("Error: Unable to set value %d to ENUM mixer control %s since"
						"this value is out of bounds", values[value_idx], mixer_ctl_get_name(ctl));
			}
		}
		break;
	default:
		return ret;
	}
	return ret;
}

static int alsactrl_set_control(struct mixer_ctl *ctl, unsigned int idx, long value)
{
	/*enum mixer_ctl_type ctl_type;*/
	const char *name;
	int ret = -1;
	struct snd_ctl_elem_value ev;
	LOG_I("Enter");
	name = mixer_ctl_get_name(ctl);
	ret = mixer_ctrl_read_elem_value(ctl, &ev);
	if (ret < 0) {
		LOG_E("Unable to get the control value !!");
		return ret;
	}
	ret = mixer_ctrl_modify_elem_value(ctl, &ev, value, idx);
	if (ret < 0) {
		LOG_E("Unable to set BOOL/INT value for mixer control = %s idx = %d value = %d",
				name, idx, value);
		return ret;
	}
	ret = mixer_ctrl_write_elem_value(ctl, &ev);
	if (ret < 0) {
		LOG_E("Unable to set the control..Control not found !! ");
	}
	LOG_I("Exit");
	return ret;
}

static int alsactrl_alsa_get_card_idx(const char* card_name)
{
	int cardno_open;

	LOG_I("Enter (name = '%s'.\n", card_name);

	cardno_open = get_card_index(card_name);
	if (cardno_open < 0) {
		LOG_E("ERROR: Card %s not found!", card_name);
		return -1;
	}

	return cardno_open;
}

static void alsactrl_alsa_close_card(void)
{
	LOG_I("Enter");
	if (mixer_p != NULL)
		mixer_close(mixer_p);
	mixer_p = NULL;
}

int audio_hal_alsa_get_card_and_device_idx(const char* dev_name, unsigned int stream_dir,
											int* idx_card_p, int* idx_dev_p)
{
	if (stream_dir == PCM_IN) {
		stream_dir = 1;
	}
	int ret, idx_dev, len, idx_card, fd;
	struct snd_pcm_info *pcminfo;
	const char* dev_name_read;
	for (idx_card = 0; idx_card < MAX_SND_CARD_NBR; idx_card++) {
		fd = scan_for_next_card(idx_card, NULL, NULL);
		LOG_I("the value of fd = %d", fd);
		if (fd < 0) {
			continue;
		}
		idx_dev = -1;
		for (;;) {
			ret = scan_for_next_device(fd, &idx_dev);
			LOG_I("Device name detected = %d for card no = %d and fd = %d", idx_dev, idx_card, fd);
			if (ret < 0) {
				LOG_I("Device not found");
				break;
			}
			if (idx_dev == -1)
				break;
			LOG_I("idx_card = %d, Device  = %d found", idx_card, idx_dev);
			pcminfo = (struct snd_pcm_info *)malloc(sizeof(struct snd_pcm_info));
			pcminfo->device = idx_dev;
			pcminfo->subdevice = 0;
			pcminfo->stream = stream_dir;
			LOG_I("fd for the pcm info function : %d", fd);
			ret = get_pcm_info(fd, pcminfo);
			LOG_I("Device = %d, subdevice = %d, Stream = %d, card = %d, id = %s,name = %s,subname = %s,subdevices_count= %d,subdevices_avail = %d", pcminfo->device, pcminfo->subdevice, pcminfo->stream, pcminfo->card, pcminfo->id, pcminfo->name, pcminfo->subname, pcminfo->subdevices_count, pcminfo->subdevices_avail);
			if (ret < 0)
				continue;
			dev_name_read = get_device_id(pcminfo);
			LOG_I("The Device name read = %s dev_name = %s", dev_name_read, dev_name);
			len = strchr(dev_name_read, ' ') - dev_name_read;
			if (strncmp(dev_name, dev_name_read, len) == 0)
				break;
		}
		close(fd);
		if (idx_dev >= 0) {
			*idx_card_p = idx_card;
			*idx_dev_p = idx_dev;
			free(pcminfo);
			return 0;
		}
	}
	LOG_E("device name %s not found", dev_name);
	return -1;
}

void audio_hal_alsa_close_controls()
{
	LOG_I("Enter");
	alsactrl_alsa_close_card();
	pthread_mutex_unlock(&ctl_open_mutex);
}

int audio_hal_alsa_open_controls_cardno(int cardno_open)
{
	pthread_mutex_lock(&ctl_open_mutex);
	mixer_p = mixer_open(cardno_open);
	if (!mixer_p) {
		LOG_E("ERROR: Unable to open mixer");
		pthread_mutex_unlock(&ctl_open_mutex);
		return -1;
	}
	LOG_I("Mixer for Card no. %d opened successfully", cardno_open);
	return 0;
}

int audio_hal_alsa_open_controls(const char* card_name)
{
	LOG_I("Enter (card_name = '%s').", card_name);
	return audio_hal_alsa_open_controls_cardno(alsactrl_alsa_get_card_idx(card_name));
}

int audio_hal_alsa_set_control_values(const char* name, long *values)
{
	struct mixer_ctl *ctrl_p = NULL;
	ctrl_p = mixer_get_ctl_by_name(mixer_p, name);
	if (ctrl_p == NULL) {
		LOG_E("ERROR: Controls not opened!\n");
		audio_hal_alsa_close_controls();
		return -1;
	}
	return alsactrl_alsa_set_control(ctrl_p, values);
}

int audio_hal_alsa_set_control(const char* name, unsigned int idx, long value)
{
	struct mixer_ctl *ctrl_p = NULL;
	ctrl_p = mixer_get_ctl_by_name(mixer_p, name);
	if (ctrl_p == NULL) {
		LOG_E("ERROR: Controls not opened!\n");
		audio_hal_alsa_close_controls();
		return -1;
	}
	return alsactrl_set_control(ctrl_p, idx, value);
}

static int is_control_in_ignore_list(char *name)
{
	if ((strcmp(name, "Digital Interface Master Generator Switch") == 0) ||
		(strcmp(name, "Digital Interface 0 Bit-clock Switch") == 0) ||
		(strcmp(name, "Digital Interface 1 Bit-clock Switch") == 0) ||
		(strcmp(name, "Digital Interface AD 1 Loopback Switch") == 0) ||
		(strcmp(name, "Digital Interface AD 2 Loopback Switch") == 0) ||
		(strcmp(name, "Digital Interface AD 3 Loopback Switch") == 0) ||
		(strcmp(name, "Digital Interface AD 4 Loopback Switch") == 0) ||
		(strcmp(name, "Digital Interface AD 5 Loopback Switch") == 0) ||
		(strcmp(name, "Digital Interface AD 6 Loopback Switch") == 0) ||
		(strcmp(name, "Digital Interface AD 7 Loopback Switch") == 0) ||
		(strcmp(name, "Digital Interface AD 8 Loopback Switch") == 0) ||
		(strcmp(name, "Sidetone FIR Coefficient Index") == 0) ||
		(strcmp(name, "Sidetone FIR Coefficient Value") == 0) ||
		(strcmp(name, "Sidetone FIR Apply Coefficients") == 0) ||
		(strcmp(name, "Sidetone Left Source Playback Route") == 0) ||
		(strcmp(name, "Sidetone Right Source Playback Route") == 0) ||
		(strcmp(name, "Sidetone Digital Gain Playback Volume") == 0) ||
		(strcmp(name, "Sidetone Playback Switch") == 0) ||
		(strcmp(name, "ANC Warp Delay Shift") == 0) ||
		(strcmp(name, "ANC FIR Output Shift") == 0) ||
		(strcmp(name, "ANC IIR Output Shift") == 0) ||
		(strcmp(name, "ANC Warp Delay") == 0) ||
		(strcmp(name, "ANC FIR Coefficients") == 0) ||
		(strcmp(name, "ANC IIR Coefficients") == 0) ||
		(strcmp(name, "ANC Status") == 0) ||
		(strcmp(name, "Digital Interface AD To Slot 0 Map") == 0) ||
		(strcmp(name, "Digital Interface AD To Slot 1 Map") == 0) ||
		(strcmp(name, "Digital Interface AD To Slot 2 Map") == 0) ||
		(strcmp(name, "Digital Interface AD To Slot 3 Map") == 0) ||
		(strcmp(name, "Digital Interface AD To Slot 4 Map") == 0) ||
		(strcmp(name, "Digital Interface AD To Slot 5 Map") == 0) ||
		(strcmp(name, "Digital Interface AD To Slot 6 Map") == 0) ||
		(strcmp(name, "Digital Interface AD To Slot 7 Map") == 0) ||
		(strcmp(name, "Mic 1A Regulator") == 0) ||
		(strcmp(name, "Mic 1B Regulator") == 0) ||
		(strcmp(name, "Mic 2 Regulator") == 0) ||
		(strcmp(name, "IHF and Headset Swap Playback Switch") == 0) ||
		(strcmp(name, "Burst FIFO Interface Mode") == 0) ||
		(strcmp(name, "Burst FIFO Interface Switch") == 0) ||
		(strcmp(name, "Burst FIFO Threshold") == 0) ||
		(strcmp(name, "Burst FIFO Length") == 0) ||
		(strcmp(name, "Burst FIFO FS Extra Bit-clocks") == 0) ||
		(strcmp(name, "Burst FIFO Wake Up Delay") == 0) ||
		(strcmp(name, "Burst FIFO Switch Frame Number") == 0) ||
		(strcmp(name, "Digital Interface 0 FIFO Enable Switch")  == 0))
	{
		LOG_I("Ignoring control '%s'!", name);
		return 1;
	}
	return 0;
}

static int audio_hal_alsa_set_control_cfg(struct control_config_t *control_p)
{
	int ret = -1;
	int value_idx, value_count;
	struct mixer_ctl *ctrl_p;
	enum mixer_ctl_type ctl_type;
	struct snd_ctl_elem_value ev;
	LOG_I("Enter");
	if (!control_p) {
		LOG_E("ERROR: Invalid control node!\n");
		return -1;
	}
	ctrl_p = mixer_get_ctl_by_name(mixer_p, control_p->name);
	if (ctrl_p == NULL) {
		LOG_E("Error: Mixer Control name = %s not found", control_p->name);
		return -1;
	}
	ret = mixer_ctrl_read_elem_value(ctrl_p, &ev);
	if (ret < 0) {
		LOG_E("Unable to get the control..Control not found !!");
		return -1;
	}
	value_count = mixer_ctl_get_num_values(ctrl_p);
	if (value_count <= 0) {
		LOG_E("Error: Nothing to set for mixer control = %s", control_p->name);
		return -1;
	}
	ctl_type = mixer_ctl_get_type(ctrl_p);
	switch (ctl_type) {
	case MIXER_CTL_TYPE_BOOL:
	case MIXER_CTL_TYPE_INT:
		for (value_idx = 0; value_idx < value_count; value_idx++) {
			ret = mixer_ctrl_modify_elem_value(ctrl_p, &ev, control_p->value[value_idx].integer, value_idx);
			if (ret < 0) {
				LOG_E("Error: Unable to set BOOL/INT value for mixer control = %s", control_p->name);
				break;
			}
			LOG_I("Successfully set BOOL/INT value for mixer control = %s idx = %d value = %d", control_p->name, value_idx, control_p->value[value_idx].integer);
		}
		break;
	case MIXER_CTL_TYPE_ENUM:
		for (value_idx = 0; value_idx < value_count; value_idx++) {
			ret = mixer_ctrl_modify_elem_enum_string(ctrl_p, &ev,
								control_p->value[value_idx].enum_val_str, value_idx);
			if (ret < 0) {
				LOG_E("Error: Unable to set ENUM value for mixer control = %s", control_p->name);
				break;
			}
			LOG_I("Successfully set ENUM value for mixer control = %s idx = %d value = %s", control_p->name, value_idx, control_p->value[value_idx].enum_val_str);
		}
		break;
	default:
		LOG_E("Error: Unknown type found for mixer control = %s", control_p->name);
	}
	ret = mixer_ctrl_write_elem_value(ctrl_p, &ev);
	if (ret < 0) {
		LOG_E("Unable to set the control..Control not found !! ");
	}
	LOG_I("Exit");
	return ret;
}

int audio_hal_alsa_memctrl_init_default(const char* data)
{
	int ret = -1;
	LOG_I("Enter.\n");
	if (default_cfg_list != NULL) {
		delete_control_list(default_cfg_list);
		LOG_I("Deleted default configuration list");
		default_cfg_list = NULL;
	}
	if (main_cfg_list != NULL) {
		delete_control_list(main_cfg_list);
		LOG_I("Deleted main configuration list");
		main_cfg_list = NULL;
	}
	ret = audio_hal_alsa_memctrl_set(data);/* this will parse data and populate main_cfg_list*/
	if (ret < 0) {
		LOG_E("Error: ALSA Failed to set memctrl");
		goto cleanup;
	}
	ret = copy_control_list(&default_cfg_list, main_cfg_list);
	if (ret < 0)
		LOG_E("Error: Failed to copy current cfg to default cfg error: %d\n", ret);

cleanup:
	return ret;
}

int audio_hal_alsa_memctrl_set_default()
{
	int ret = -1;
	LOG_I("Enter.\n");
	if (default_cfg_list == NULL) {
		LOG_E("Error: Cant copy default cfg, default cfg == NULL");
		return -1;
	}
	if (main_cfg_list != NULL) {
		delete_control_list(main_cfg_list);
		LOG_I("main_cfg_list deleted");
		main_cfg_list = NULL;
	}
	ret = copy_control_list(&main_cfg_list, default_cfg_list);
	if (ret < 0)
		LOG_E("Error: Failed to copy default cfg to current cfg error: %d\n", ret);
	return ret;
}

void audio_hal_alsa_memctrl_reset()
{
	LOG_I("Enter.\n");
	if (default_cfg_list != NULL) {
		delete_control_list(default_cfg_list);
		default_cfg_list = NULL;
	}
	if (main_cfg_list != NULL) {
		delete_control_list(main_cfg_list);
		main_cfg_list = NULL;
	}
}

int audio_hal_alsa_memctrl_set(const char* data)
{
	int ret = 0;
	struct parser_context *statefile_parser = NULL;
	struct control_config_t *override_cfg_list = NULL;
	LOG_I("Enter");
	ret = init_parser(&statefile_parser, data);
	if (ret) {
		LOG_E("Error: Statefile Parser init failed");
		return ret;
	}
	if (!main_cfg_list) {
		ret = populate_control_list(statefile_parser, &main_cfg_list);
		if (ret) {
			LOG_E("Error: Statefile Parset unable to parse for main cfg list");
			goto cleanup;
		}
	} else {
		ret = populate_control_list(statefile_parser, &override_cfg_list);
		if (ret) {
			LOG_E("Error: Statefile Parset unable to parse for override list");
			goto cleanup;
		}
		ret = override_control_list(main_cfg_list, override_cfg_list);
		LOG_I("Override control list %s" , (ret) ? "Fails" : "Success");
		if (!ret)
			delete_control_list(override_cfg_list);
	}

cleanup:
	exit_parser(statefile_parser);
	return ret;
}

int audio_hal_alsa_memctrl_write()
{
	int ret = -1, nCtrl = 0, nCtrl_notOK = 0;
	struct control_config_t *node = NULL;
	LOG_I("Enter.\n");
	if (main_cfg_list == NULL) {
		LOG_E("ERROR: main_cfg_list is NULL nothing to commit!\n");
		goto cleanup;
	}
	node = main_cfg_list;
	while (node) {
		LOG_I("Name = %s, Count= %d, Type = %d",node->name, node->count, node->type);
		nCtrl++;
		if ((strcmp(node->name, "") == 0) || is_control_in_ignore_list(node->name)) {
			node = node->next;
			continue;
		}
		if (audio_hal_alsa_set_control_cfg(node)!= 0)
			nCtrl_notOK++;
		node = node->next;
	}
	LOG_I("%d/%d controls correctly set.", nCtrl-nCtrl_notOK, nCtrl);
	return 0;
cleanup:
	audio_hal_alsa_close_controls();
	if (main_cfg_list != NULL) {
		delete_control_list(main_cfg_list);
		main_cfg_list = NULL;
	}
	return ret;
}

enum audio_hal_chip_id_t audio_hal_alsa_get_chip_id(void)
{
	int ret;
	struct mixer_ctl *ctl = NULL;
	ret = audio_hal_alsa_open_controls_cardno(0);
	if (ret < 0) {
		LOG_E("ERROR: audio_hal_alsa_open_controls_cardno failed (ret = %d)!", ret);
		return CHIP_ID_UNKNOWN;
	}
	ctl =  mixer_get_ctl_by_name(mixer_p, "ChipId");
	if (ctl == NULL) {
		LOG_E("ERROR: Control 'ChipId' not found!\n");
		audio_hal_alsa_close_controls();
		return CHIP_ID_UNKNOWN;
	}
	ret = mixer_ctl_get_value(ctl, 0);
	if (ret < 0) {
		LOG_E("ERROR: Unable to get codec chip-ID (ret = %d)!", ret);
		return CHIP_ID_UNKNOWN;
	}
	audio_hal_alsa_close_controls();
	return (enum audio_hal_chip_id_t)ret;
}

static const char *bool_str1(bool value)
{
	return (value) ? "True" : "False";
}

const char* audio_hal_alsa_get_chip_id_str(enum audio_hal_chip_id_t chip_id)
{
	switch (chip_id) {
	case CHIP_ID_UNKNOWN:
		return "CHIP_ID_UNKNOWN";
	case CHIP_ID_AB8500:
		return "CHIP_ID_AB8500";
	case CHIP_ID_AB9540_V1:
		return "CHIP_ID_AB9540_V1";
	case CHIP_ID_AB9540_V2:
		return "CHIP_ID_AB9540_V2";
	case CHIP_ID_AB9540_V3:
		return "CHIP_ID_AB9540_V3";
	case CHIP_ID_AB8505_V1:
		return "CHIP_ID_AB8505_V1";
	case CHIP_ID_AB8505_V2:
		return "CHIP_ID_AB8505_V2";
	case CHIP_ID_AB8505_V3:
		return "CHIP_ID_AB8505_V3";
	case CHIP_ID_AB8540_V1:
		return "CHIP_ID_AB8540_V1";
	case CHIP_ID_AB8540_V2:
		return "CHIP_ID_AB8540_V2";
	default:
		return "";
	}
}

void audio_hal_alsa_set_ignore_defaults(bool ignore)
{
	LOG_I("ignore_defaults changed to '%s'\n", bool_str1(ignore));
	ignore_defaults = ignore;
}

bool audio_hal_alsa_get_ignore_defaults(void)
{
	return ignore_defaults;
}

