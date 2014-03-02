/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef ALSACTRL_HWH_H
#define ALSACTRL_HWH_H

#include <sqlite3.h>

#include "alsactrl_alsa.h"

#include "alsactrl_hwh.h"

// AB8500-devices (out)
#define ALSACTRL_DEVSTR_HSOUT "HSetOut"
#define ALSACTRL_DEVSTR_SPEAKER "Speaker"
#define ALSACTRL_DEVSTR_EARP "EarPiece"
#define ALSACTRL_DEVSTR_LINEOUT "LineOut"
#define ALSACTRL_DEVSTR_CARKITOUT "CarKitOut"
#define ALSACTRL_DEVSTR_AUSBOUT "AUSBOut"
#define ALSACTRL_DEVSTR_VIBL "VibraL"
#define ALSACTRL_DEVSTR_VIBR "VibraR"
#define ALSACTRL_DEVSTR_EPWM1 "EPWM1"
#define ALSACTRL_DEVSTR_EPWM2 "EPWM2"
#define ALSACTRL_DEVSTR_PDM1 "PDM1"
#define ALSACTRL_DEVSTR_PDM2 "PDM2"
// AB8540-devices (out)
#define ALSACTRL_DEVSTR_MODEMUL "MODEM_UL"
// AB8100-devices (out)
#define ALSACTRL_DEVSTR_HDMI "HDMI"
// CG29xx-devices (out)
#define ALSACTRL_DEVSTR_BTOUT "BTOut"
#define ALSACTRL_DEVSTR_BTWBOUT "BTWBOut"
#define ALSACTRL_DEVSTR_A2DP "A2DPOut"
#define ALSACTRL_DEVSTR_FMTX "FMTX"
// Misc-devices (out)
#define ALSACTRL_DEVSTR_TTYOUT "TTYOut"
#define ALSACTRL_DEVSTR_DICT_PLAY "x-ste-adm-dict-play"
#define ALSACTRL_DEVSTR_VOIPOUT "VoIPOut"
#define ALSACTRL_DEVSTR_USBOUT "USBOut"

// AB8500-devices (in)
#define ALSACTRL_DEVSTR_HSIN "HSetIn"
#define ALSACTRL_DEVSTR_MIC "Mic"
#define ALSACTRL_DEVSTR_LINEIN "LineIn"
// AB8540-devices (in)
#define ALSACTRL_DEVSTR_MODEMDL "MODEM_DL"
// CG29xx-devices (in)
#define ALSACTRL_DEVSTR_BTIN "BTIn"
#define ALSACTRL_DEVSTR_BTWBIN "BTWBIn"
#define ALSACTRL_DEVSTR_FMRX "FMRX"
// Misc-devices (in)
#define ALSACTRL_DEVSTR_TTYIN "TTYIn"
#define ALSACTRL_DEVSTR_DICT_REC "x-ste-adm-dict-rec"
#define ALSACTRL_DEVSTR_VOIPIN "VoIPIn"
#define ALSACTRL_DEVSTR_USBIN "USBIn"

// AB8500-switches (out)
#define ALSACTRL_SWSTR_HEADSET_L "Headset Left"
#define ALSACTRL_SWSTR_HEADSET_R "Headset Right"
#define ALSACTRL_SWSTR_IHF_L "IHF Left"
#define ALSACTRL_SWSTR_IHF_R "IHF Right"
#define ALSACTRL_SWSTR_EARPIECE "Earpiece"
#define ALSACTRL_SWSTR_LINEOUT_L "LineOut Left"
#define ALSACTRL_SWSTR_LINEOUT_R "LineOut Right"
#define ALSACTRL_SWSTR_CARKIT_L "Carkit Left"
#define ALSACTRL_SWSTR_CARKIT_R "Carkit Right"
#define ALSACTRL_SWSTR_AUSB_L "USB Headset Left"
#define ALSACTRL_SWSTR_AUSB_R "USB Headset Right"
#define ALSACTRL_SWSTR_VIBRA_1 "Vibra 1"
#define ALSACTRL_SWSTR_VIBRA_2 "Vibra 2"
#define ALSACTRL_SWSTR_EPWM_1 "EPWM 1"
#define ALSACTRL_SWSTR_EPWM_2 "EPWM 2"
#define ALSACTRL_SWSTR_PDM_1 "PDM 1"
#define ALSACTRL_SWSTR_PDM_2 "PDM 2"
// AB8500-switches (in)
#define ALSACTRL_SWSTR_AMIC_1 "Mic 1"
#define ALSACTRL_SWSTR_AMIC_2 "Mic 2"
#define ALSACTRL_SWSTR_LINEIN_L "LineIn Left"
#define ALSACTRL_SWSTR_LINEIN_R "LineIn Right"

// AB8540-switches
#define ALSACTRL_SWSTR_VOICE_BCLK "Voice Interface Bit-clock Switch"
#define ALSACTRL_SWSTR_VOICE_MAST_GEN "Voice Interface Master Generator Switch"
#define ALSACTRL_SWSTR_VOICE_MODEMDL "Voice ModemDL"
#define ALSACTRL_SWSTR_VOICE_MODEMUL "Voice ModemUL"

/**
* Represent the speed fade register values
* FADESPEED_FAST = 0 --> 1ms
* FADESPEED_SLOW = 3 --> 16ms
*/
typedef enum {
	FADESPEED_FAST = 0, /* 1ms */
	FADESPEED_SLOW = 3 /* 16ms */
} fadeSpeed_t;

enum amic {
	MIC1A = 0,
	MIC1B = 1,
	MIC2 = 2,
	AMIC_COUNT
};

enum microphone_type {
	MICROPHONE_TYPE_ANALOG = 0,
	MICROPHONE_TYPE_DIGITAL = 1
};

typedef struct {
	enum microphone_type type;
	unsigned int n_mics;
	enum amic mics[6];
} mic_config_t;

typedef int (*hwh_dev_next_t)(const char** dev_name_pp);
typedef int (*hwh_d2d_next_t)(const char** src_dev_name_pp, const char** dst_dev_name_pp);

typedef int (*alsactrl_hwh_init_t)(enum audio_hal_chip_id_t chip_id, sqlite3* db_p);
typedef int (*alsactrl_hwh_t)(sqlite3* db_p, hwh_dev_next_t dev_next, hwh_d2d_next_t dev_next_d2d, fadeSpeed_t fadeSpeed);
typedef int (*alsactrl_hwh_vc_t)(sqlite3* db_p, const char* indev, const char* outdev, int fs);

typedef struct {
	enum audio_hal_chip_id_t chip_id;
	const char* card_name;
	alsactrl_hwh_init_t Hwh_Init;
	alsactrl_hwh_t Hwh;
	alsactrl_hwh_vc_t Hwh_VC;
} hwh_t;

enum alsactrl_alsa_device {
	ALSACTRL_ALSA_DEVICE_AB850x = 0,
	ALSACTRL_ALSA_DEVICE_CG29XX = 1,
	ALSACTRL_ALSA_DEVICE_AB8540 = 2,
	ALSACTRL_ALSA_DEVICE_AB8540_VC= 3,
};

enum alsactrl_channel_state {
	ALSACTRL_CHANNEL_STATE_CLOSE = 0,
	ALSACTRL_CHANNEL_STATE_OPEN = 1,
};

typedef struct {
	const char* name;
	enum alsactrl_alsa_device alsa_dev;
	const char* dev_name;
	unsigned int stream_dir;
	int channels;
	int bits;
	int rate;
	struct pcm *pcm;
	int active;
	bool opened;
} alsactrl_dev_info_t;

// Public interface

typedef int (*alsactrl_hwh_GetToplevelMapping_t)(const char* toplevel_dev, const char** actual_dev);
int Alsactrl_Hwh_GetToplevelMapping(const char* toplevel_dev, const char** actual_dev);

const char *Alsactrl_Hwh_GetMicStr(enum amic am);
enum audio_hal_chip_id_t Alsactrl_Hwh_SelectHW(void);
int Alsactrl_Hwh_Init(sqlite3* db_p, alsactrl_hwh_GetToplevelMapping_t get_top_level_mapping_fp);
int Alsactrl_Hwh(sqlite3* db_p, hwh_dev_next_t dev_next, hwh_d2d_next_t dev_next_d2d, fadeSpeed_t fadeSpeed);
int Alsactrl_Hwh_VC(sqlite3* db_p, const char* indev, const char* outdev, int fs);

int Alsactrl_Hwh_OpenControls(void);
void Alsactrl_Hwh_CloseControls(void);
const char* Alsactrl_Hwh_CardName(void);
enum audio_hal_chip_id_t Alsactrl_Hwh_ChipID(void);

#endif /* ALSACTRL_HWH_H */

