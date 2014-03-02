/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef ALSACTRL_HWH_U8540_DEV_H
#define ALSACTRL_HWH_U8540_DEV_H

#include "alsactrl_hwh_u85xx.h"

enum u8540_device_idx {
	U8540_DEV_HSOUT,
	U8540_DEV_SPEAKER,
	U8540_DEV_EARP,
	U8540_DEV_HSIN,
	U8540_DEV_MIC,
	U8540_DEV_LINEIN,
	U8540_DEV_FMTX,
	U8540_DEV_FMRX,
	U8540_DEV_LINEOUT,
	U8540_DEV_CARKITOUT,
	U8540_DEV_AUSBOUT,
	U8540_DEV_EPWM1,
	U8540_DEV_EPWM2,
	U8540_DEV_PDM1,
	U8540_DEV_PDM2,
	U8540_DEV_VIBL,
	U8540_DEV_MODEMDL,
	U8540_DEV_MODEMUL,
};

enum u8540_switch_idx {
	U8540_SW_HEADSET_L,
	U8540_SW_HEADSET_R,
	U8540_SW_IHF_L,
	U8540_SW_IHF_R,
	U8540_SW_EARPIECE,
	U8540_SW_LINEOUT_L,
	U8540_SW_LINEOUT_R,
	U8540_SW_AMIC_1,
	U8540_SW_AMIC_2,
	U8540_SW_LINEIN_L,
	U8540_SW_LINEIN_R,
	U8540_SW_CARKIT_L,
	U8540_SW_CARKIT_R,
	U8540_SW_AUSB_L,
	U8540_SW_AUSB_R,
	U8540_SW_EPWM_1,
	U8540_SW_EPWM_2,
	U8540_SW_PDM_1,
	U8540_SW_PDM_2,
	U8540_SW_VIBRA_1,
	U8540_SW_VOICE_BCLK,
	U8540_SW_VOICE_MAST_GEN,
	U8540_SW_VOICE_MODEMDL,
	U8540_SW_VOICE_MODEMUL,
};

// Public interface

u85xx_device_t* Alsactrl_Hwh_U8540_Dev_GetDevs(void);
int Alsactrl_Hwh_U8540_Dev(sqlite3* db_p, hwh_dev_next_t dev_next, enum audio_hal_chip_id_t chip_id);
int Alsactrl_Hwh_U8540_Dev_UpdateSwitches(sqlite3* db_p, enum audio_hal_chip_id_t chip_id);

#endif
