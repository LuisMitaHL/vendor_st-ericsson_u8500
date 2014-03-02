#ifndef _alsactrl_tinyalsa_extn_
#define _alsactrl_tinyalsa_extn_

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/ioctl.h>
#include <sound/asound.h>
#include <tinyalsa/asoundlib.h>

#define MAX_SND_CARD_NBR 32
#define CARD_ID_LEN 25
/*
** PCM Interface Extenssion Apis
*/
int pcm_prepare(struct pcm *pcm);

/*
** Mixer Control Interface Extenssion Apis
*/
int mixer_ctrl_read_elem_value(struct mixer_ctl *ctl, struct snd_ctl_elem_value *ev);
int mixer_ctrl_modify_elem_value(struct mixer_ctl *ctl, struct snd_ctl_elem_value *ev,
															int value, unsigned int idx);
int mixer_ctrl_modify_elem_enum_string(struct mixer_ctl *ctl, struct snd_ctl_elem_value *ev,
																char *string, unsigned int idx);
int mixer_ctrl_write_elem_value(struct mixer_ctl *ctl, struct snd_ctl_elem_value *ev);
int scan_for_next_card(int cardno, char *card_id, int size);
int scan_for_next_device(int card_fd,int *device);
int get_card_info(int card_fd,struct snd_ctl_card_info *cardinfo);
int get_pcm_info(int pcm_fd,struct snd_pcm_info *pcminfo);
const char *get_device_id(const struct snd_pcm_info *obj);
int get_card_index(char *cardname);


#endif /* _alsactrl_tinyalsa_extn_ */