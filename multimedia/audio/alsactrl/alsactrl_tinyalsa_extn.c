
#include "alsactrl_tinyalsa_extn.h"
#include "alsactrl_debug.h"
#include <fcntl.h>
#include <assert.h>
#include <string.h>

/*
** PCM Interface Extenssion Apis
*/

int pcm_prepare(struct pcm *pcm)
{
	int ret = -1;
	ret = ioctl(pcm->fd, SNDRV_PCM_IOCTL_PREPARE);
	if (ret)
		LOG_E("Cannot prepare channel, error = %s", strerror(errno));
	return ret;
}

/*
** Mixer Control Interface Extenssion Apis
*/

int mixer_ctrl_read_elem_value(struct mixer_ctl *ctl, struct snd_ctl_elem_value *ev)
{
	if (!ctl)
		return -EINVAL;
	memset(ev, 0, sizeof(struct snd_ctl_elem_value));
	ev->id.numid = ctl->info->id.numid;
	return ioctl(ctl->mixer->fd, SNDRV_CTL_IOCTL_ELEM_READ, ev);
}

int mixer_ctrl_modify_elem_value(struct mixer_ctl *ctl, struct snd_ctl_elem_value *ev,
									int value, unsigned int idx)
{
	long min, max;
	int no_items;
	if (!ctl || (idx >= ctl->info->count))
		return -EINVAL;
	switch (ctl->info->type) {
	case SNDRV_CTL_ELEM_TYPE_BOOLEAN:
		ev->value.integer.value[idx] = !!value;
		break;
	case SNDRV_CTL_ELEM_TYPE_INTEGER:
		min = mixer_ctl_get_range_min(ctl);
		max = mixer_ctl_get_range_max(ctl);
		value = (value > max ? max : (value < min ? min : value));
		ev->value.integer.value[idx] = value;
		break;
	case SNDRV_CTL_ELEM_TYPE_BYTES:
		ev->value.bytes.data[idx] = value;
		break;
	case SNDRV_CTL_ELEM_TYPE_ENUMERATED:
		no_items = mixer_ctl_get_num_enums(ctl);
		if (value > no_items - 1) {
			LOG_E("ERROR: Enum-index %d outside bounds!\n", (int)value);
			return -EINVAL;
		}
		ev->value.enumerated.item[idx] = value;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

int mixer_ctrl_modify_elem_enum_string(struct mixer_ctl *ctl, struct snd_ctl_elem_value *ev,
																char *string, unsigned int idx)
{
	unsigned int i, num_enums;

	if (!ctl || (idx >= ctl->info->count))
		return -EINVAL;
	if (ctl->info->type != SNDRV_CTL_ELEM_TYPE_ENUMERATED)
		return -EINVAL;
	num_enums = ctl->info->value.enumerated.items;
	for (i = 0; i < num_enums; i++) {
		if (!strcmp(string, ctl->ename[i])) {
			ev->value.enumerated.item[idx] = i;
			return 0;
		}
	}
	return -EINVAL;
}

int mixer_ctrl_write_elem_value(struct mixer_ctl *ctl, struct snd_ctl_elem_value *ev)
{
	if (!ctl)
		return -EINVAL;
	ev->id.numid = ctl->info->id.numid;
	return ioctl(ctl->mixer->fd, SNDRV_CTL_IOCTL_ELEM_WRITE, ev);
}

/*to extract the card number*/

int scan_for_next_card(int cardno, char *card_id, int size)
{
	LOG_W("Enter.");
	struct snd_ctl_card_info *cardinfo;
	cardinfo = (struct snd_ctl_card_info *)malloc(sizeof(struct snd_ctl_card_info));
	if (!cardinfo) {
		LOG_E("Error: Malloc failed unable to allocate memory to cardinfo");
		return -1;
	}
	int fd, ret;
	char control[30];
	sprintf(control, "/dev/snd/controlC%i", cardno);
	LOG_W("Trying file name = %s with cardno = %d", control, cardno);
	fd = open(control, O_RDONLY);
	if (fd > 0) {
		ret = get_card_info(fd, cardinfo);
		LOG_W("cardno = %d,cardid = %s", cardno, cardinfo->id);
		if (ret < 0)
			close(fd);
		if (card_id)
			strncpy(card_id, cardinfo->id, size);
	}
	free(cardinfo);
	return fd;
}

/*to extract the device_number*/

int scan_for_next_device(int card_fd, int *device)
{
	LOG_I("Enter.");
	assert(device);
	if (ioctl(card_fd, SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE, device) < 0) {
		LOG_E("SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE failed err = %s", strerror(errno));
		return -errno;
	}
	return 0;
}

/* to get the card info*/

int get_card_info(int card_fd, struct snd_ctl_card_info *cardinfo)
{
	LOG_I("Enter.");
	if (ioctl(card_fd, SNDRV_CTL_IOCTL_CARD_INFO, cardinfo) < 0) {
		LOG_E("SNDRV_CTL_IOCTL_CARD_INFO failed with err = %s", strerror(errno));
		return -errno;
	}
	return 0;
}

/* to get the pcm info*/

int get_pcm_info(int pcm_fd, struct snd_pcm_info *pcminfo)
{
	LOG_I("Enter.");
	if (ioctl(pcm_fd, SNDRV_CTL_IOCTL_PCM_INFO, pcminfo) < 0) {
		LOG_E("SNDRV_CTL_IOCTL_PCM_INFO failed with err = %s", strerror(errno));
		return -errno;
	}
	return 0;
}

/*get device id*/

const char *get_device_id(const struct snd_pcm_info *pcminfo)
{
	LOG_I("Enter.");
	assert(pcminfo);
	return (const char *)pcminfo->id;
}

/*get card index*/

int get_card_index(char *cardname)
{
	int ret, cardno;
	char id[CARD_ID_LEN];
	for (cardno = 0; cardno < MAX_SND_CARD_NBR; cardno++) {
		ret = scan_for_next_card(cardno, id, CARD_ID_LEN);
		LOG_W("scan_for_next_card fd:%d, id:%s, cardname:%s\n", ret, id, cardname);
		if (ret > 0) {
			LOG_W("Close fd:%d\n",ret);
			close(ret);
			if (!strcmp(id, cardname)){
				return cardno;
			}
		}
	}
	return -ENODEV;
}