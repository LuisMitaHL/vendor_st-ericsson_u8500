/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ALSACTRL_HWH_U8500_H
#define ALSACTRL_HWH_U8500_H

#include <sqlite3.h>

#include "alsactrl_alsa.h"

#include "alsactrl_hwh.h"

// Public interface

int Alsactrl_Hwh_U8500_Init(enum audio_hal_chip_id_t chip_id, sqlite3* db_p);
int Alsactrl_Hwh_U8500(sqlite3* db_p, hwh_dev_next_t dev_next, hwh_d2d_next_t dev_next_d2d, fadeSpeed_t fadeSpeed);
int Alsactrl_Hwh_U8500_VC(sqlite3* db_p, const char* indev, const char* outdev, int fs);

#endif
