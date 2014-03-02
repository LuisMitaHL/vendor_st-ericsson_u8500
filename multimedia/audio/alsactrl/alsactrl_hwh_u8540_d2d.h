/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef ALSACTRL_HWH_U8540_D2D_H
#define ALSACTRL_HWH_U8540_D2D_H

#include <sqlite3.h>

#include "alsactrl_hwh_u85xx.h"

enum u8540_d2d_idx {
	U8540_D2D_FMRX_HSOUT = 0,
	U8540_D2D_FMRX_SPEAKER,
	U8540_D2D_MODEMDL_HSOUT,
	U8540_D2D_MODEMDL_EARP,
	U8540_D2D_MODEMDL_SPEAKER,
	U8540_D2D_MODEMUL_HSIN,
	U8540_D2D_MODEMUL_MIC,
};

// Public interface

u85xx_d2d_t* Alsactrl_Hwh_U8540_D2D_GetD2Ds(void);
int Alsactrl_Hwh_U8540_D2D(sqlite3* db_p, hwh_d2d_next_t dev_next_d2d);
int Alsactrl_Hwh_U8540_D2D_UpdateSwitches(void);

#endif
