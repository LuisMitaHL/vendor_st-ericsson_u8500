/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef ALSACTRL_HWH_U8500_VC_H
#define ALSACTRL_HWH_U8500_VC_H

#include <sqlite3.h>

// Public interface

int Alsactrl_Hwh_U8500_VC_Init(const char* card_name_in);
int Alsactrl_Hwh_U8500_VC(sqlite3* db_p, const char* indev, const char* outdev, int fs);

#endif
