/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef _ALSACTRL_INTERFACE_H_
#define _ALSACTRL_INTERFACE_H_

#ifndef AUDIO_HAL_EXPORT
  #define AUDIO_HAL_EXPORT __attribute__((visibility("default")))
#endif

#include <stdbool.h>

/** 16/32 bit word aligned typedefs */
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned long uint32;
typedef signed long int32;

/* channel ID denoting special case of all channels being selected where possible */
#define AUDIO_HAL_ALL 0x7FFFFFFF

typedef enum AUDIO_HAL_STATE {
	AUDIO_HAL_STATE_OFF = 0,
	AUDIO_HAL_STATE_ON = 1
} AUDIO_HAL_STATE;

typedef enum AUDIO_HAL_STATUS {
	AUDIO_HAL_STATUS_OK = 0,
	AUDIO_HAL_STATUS_ERROR = 1,
	AUDIO_HAL_STATUS_UNSUPPORTED = 2
} AUDIO_HAL_STATUS;

typedef enum AUDIO_HAL_TYPE {
	AUDIO_HAL_DIGITAL = 0,
	AUDIO_HAL_ANALOG = 1
} AUDIO_HAL_TYPE;

typedef enum audio_hal_channel {
	AUDIO_HAL_CHANNEL_SPEAKER = 0,
	AUDIO_HAL_CHANNEL_EARPIECE,
	AUDIO_HAL_CHANNEL_HSETOUT,
	AUDIO_HAL_CHANNEL_HSETIN,
	AUDIO_HAL_CHANNEL_MIC,
	AUDIO_HAL_CHANNEL_FMTX,
	AUDIO_HAL_CHANNEL_FMRX,
	AUDIO_HAL_CHANNEL_BTOUT,
	AUDIO_HAL_CHANNEL_BTIN,
	AUDIO_HAL_CHANNEL_A2DPOUT, // Not used
	AUDIO_HAL_CHANNEL_VIBRAL,
	AUDIO_HAL_CHANNEL_VIBRAR,
	AUDIO_HAL_CHANNEL_HDMI, // Not used
	AUDIO_HAL_CHANNEL_TTYOUT, // Not used
	AUDIO_HAL_CHANNEL_TTYIN, // Not used
	AUDIO_HAL_CHANNEL_VOICE_DL, // Modem direct connection DownLink
	AUDIO_HAL_CHANNEL_VOICE_UL, // Modem direct connection UpLink
	AUDIO_HAL_CHANNEL_PDMOUT,
	AUDIO_HAL_CHANNEL_DIGMIC12,
	AUDIO_HAL_CHANNEL_DIGMIC34,
	AUDIO_HAL_CHANNEL_DIGMIC56,
} audio_hal_channel;

typedef struct {
	AUDIO_HAL_TYPE type;
} configure_fm_t;

typedef void (*audioChipsetApiStatusCB)(AUDIO_HAL_STATUS status);

#ifdef __cplusplus
extern "C" {
#endif

// Audio HAL-interface

AUDIO_HAL_STATUS audio_hal_open_channel(audio_hal_channel channel);

AUDIO_HAL_STATUS audio_hal_close_channel(audio_hal_channel channel);

AUDIO_HAL_STATUS audio_hal_set_power(uint32 channel_index, AUDIO_HAL_STATE power_control, audio_hal_channel channel);

AUDIO_HAL_STATE audio_hal_get_power(uint32 channel_index, audio_hal_channel channel);

AUDIO_HAL_STATUS audio_hal_configure_channel(audio_hal_channel channel, void *param);

int audio_hal_digital_mute(bool mute);

void audio_hal_start_fsbitclk(void);

void audio_hal_stop_fsbitclk(void);

int audio_hal_set_burst_device(audio_hal_channel channel);

int audio_hal_switch_to_burst(unsigned int framecount);

int audio_hal_switch_to_normal(void);

#ifdef __cplusplus
}
#endif

#endif // _AUDIO_INTERFACE_COMMON_H_
