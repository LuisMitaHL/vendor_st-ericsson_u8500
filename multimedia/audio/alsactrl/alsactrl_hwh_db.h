/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef ALSACTRL_HWH_DB_H
#define ALSACTRL_HWH_DB_H

#include <sqlite3.h>

#include "alsactrl_hwh.h"

int Alsactrl_DB_GetDevChannelCount(sqlite3* db_p, const char* dev);
int Alsactrl_DB_GetMicConfig_Generic(sqlite3* db_p, const char *toplevel_dev, mic_config_t *mic_config);
int Alsactrl_DB_GetMicConfig_Specific(sqlite3* db_p, const char *actual_dev, mic_config_t *mic_config);
int Alsactrl_DB_GetDeviceDataIndex(sqlite3* db_p, const char* dev, bool use_sdev);
int Alsactrl_DB_WriteDefaultData(sqlite3* db_p);
int Alsactrl_DB_WriteComboData(sqlite3* db_p, hwh_dev_next_t dev_next);
int Alsactrl_DB_WriteDevData(sqlite3* db_p, hwh_dev_next_t dev_next, bool use_sdev);

#endif /* ALSACTRL_HWH_Alsactrl_DB_H */
