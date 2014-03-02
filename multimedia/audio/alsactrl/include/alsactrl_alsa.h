/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef _ALSACTRL_INTERFACE_ALSA_H_
#define _ALSACTRL_INTERFACE_ALSA_H_

#include <stdbool.h>
#include <tinyalsa/asoundlib.h>

#ifndef AUDIO_HAL_EXPORT
  #define AUDIO_HAL_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum audio_hal_chip_id_t {
       CHIP_ID_UNKNOWN = 0,
       CHIP_ID_AB8500 = 1,
       CHIP_ID_AB9540_V1 = 2,
       CHIP_ID_AB9540_V2 = 3,
       CHIP_ID_AB9540_V3 = 4,
       CHIP_ID_AB8505_V1 = 5,
       CHIP_ID_AB8505_V2 = 6,
       CHIP_ID_AB8505_V3 = 7,
       CHIP_ID_AB8540_V1 = 8,
       CHIP_ID_AB8540_V2 = 9
};

// Audio HAL-interface (ALSA)

int audio_hal_alsa_get_card_and_device_idx(const char* dev_name, unsigned int stream_dir, int* idx_card_p, int* idx_dev_p);

int audio_hal_alsa_get_device_idx(const char* dev_name, unsigned int stream_dir);

int audio_hal_alsa_open_controls(const char* card_name);

int audio_hal_alsa_open_controls_cardno(int cardno_open);

void audio_hal_alsa_close_controls(void);

int audio_hal_alsa_get_control_values(const char* name, long **values);

int audio_hal_alsa_set_control_values(const char* name, long *values);

int audio_hal_alsa_set_control(const char* name, unsigned int idx, long value);

void audio_hal_alsa_memctrl_reset();

int audio_hal_alsa_memctrl_init_default(const char* data);

int audio_hal_alsa_memctrl_set_default();

int audio_hal_alsa_memctrl_set(const char* data);

int audio_hal_alsa_memctrl_write();

enum audio_hal_chip_id_t audio_hal_alsa_get_chip_id(void);
const char* audio_hal_alsa_get_chip_id_str(enum audio_hal_chip_id_t chip_id);

void audio_hal_alsa_set_ignore_defaults(bool ignore);

bool audio_hal_alsa_get_ignore_defaults(void);

#ifdef __cplusplus
}
#endif

#endif // _ALSACTRL_INTERFACE_ALSA_H_
