/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef ALSACTRL_HWH_U8500_DEV_H
#define ALSACTRL_HWH_U8500_DEV_H

#include "alsactrl_hwh_u85xx.h"

enum u8500_device_idx {
	U8500_DEV_HSOUT,
	U8500_DEV_SPEAKER,
	U8500_DEV_EARP,
	U8500_DEV_HSIN,
	U8500_DEV_MIC,
	U8500_DEV_LINEIN,
	U8500_DEV_FMTX,
	U8500_DEV_FMRX,
	U8500_DEV_LINEOUT,
	U8500_DEV_CARKITOUT,
	U8500_DEV_AUSBOUT,
	U8500_DEV_EPWM1,
	U8500_DEV_EPWM2,
	U8500_DEV_PDM1,
	U8500_DEV_PDM2,
	U8500_DEV_VIBL,
	U8500_DEV_VIBR,
};

enum u8500_switch_idx {
	U8500_SW_HEADSET_L,
	U8500_SW_HEADSET_R,
	U8500_SW_IHF_L,
	U8500_SW_IHF_R,
	U8500_SW_EARPIECE,
	U8500_SW_LINEOUT_L,
	U8500_SW_LINEOUT_R,
	U8500_SW_AMIC_1,
	U8500_SW_AMIC_2,
	U8500_SW_LINEIN_L,
	U8500_SW_LINEIN_R,
	U8500_SW_CARKIT_L,
	U8500_SW_CARKIT_R,
	U8500_SW_AUSB_L,
	U8500_SW_AUSB_R,
	U8500_SW_EPWM_1,
	U8500_SW_EPWM_2,
	U8500_SW_PDM_1,
	U8500_SW_PDM_2,
	U8500_SW_VIBRA_1,
	U8500_SW_VIBRA_2,
};

// Public interface

u85xx_device_t* Alsactrl_Hwh_U8500_Dev_GetDevs(void);
int Alsactrl_Hwh_U8500_Dev(sqlite3* db_p, hwh_dev_next_t dev_next, enum audio_hal_chip_id_t chip_id);
int Alsactrl_Hwh_U8500_Dev_UpdateSwitches(sqlite3* db_p, enum audio_hal_chip_id_t chip_id);

#endif
