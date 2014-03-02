/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdint.h>
#include <stddef.h>
#include <sqlite3.h>

#include "alsactrl.h"
#include "alsactrl_debug.h"

#include "alsactrl_hwh.h"
#include "alsactrl_hwh_u85xx.h"
#include "alsactrl_hwh_db.h"

int Alsactrl_DB_GetDevChannelCount(sqlite3* db_p, const char* dev)
{
	int rc = SQLITE_OK;
	sqlite3_stmt *stmt = NULL;
	const char* dev_top = NULL;
	char* command = malloc(1024 * sizeof(char));
	int nch = -1;
	int res;

	res = Alsactrl_Hwh_GetToplevelMapping(dev, &dev_top);
    if (res != 0) {
        LOG_E("ERROR: Unable to get top-level device for %s!", dev);
        goto cleanup;
    }

    memset((void*)command, 0, 1024);
    strcat(command, "SELECT Value FROM StructData "
                    "WHERE Name = ( "
                        "SELECT Template_Config.StructData FROM Template_Config "
                        "WHERE Name = ( "
                            "SELECT Device.IOTemplateName FROM Device "
                            "WHERE Device.Name = '");
    strcat(command,             dev_top);
    strcat(command,         "' ) "
                        ")"
                    "AND Param = 'nChannels'");
    LOG_I("Query: %s", command);

    rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        LOG_E("ERROR: Unable to prepare SQL-statement!");
        goto cleanup;
    }

	if (sqlite3_step(stmt) != SQLITE_ROW) {
		LOG_E("ERROR: No value found in DB!\n");
		goto cleanup;
	}

	nch = sqlite3_column_int(stmt, 0);
	if (nch <= 0) {
		LOG_E("ERROR: Illegal number of channels (nch = %d)!\n", nch);
		goto cleanup;
	}

	LOG_I("nch = %d.\n", nch);

cleanup:
	if (command != NULL) free(command);
	if (stmt != NULL) {
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	return nch;
}

int Alsactrl_DB_GetMicConfig_Specific(sqlite3* db_p, const char *actual_dev, mic_config_t *mic_config)
{
	unsigned int i, j;
	int rc = SQLITE_OK;
	int ret = ERR_GENERIC;
	sqlite3_stmt *stmt = NULL;
	char* command = malloc(1024 * sizeof(char));
	const char* amic_str_db = NULL;

	if (mic_config == NULL) {
		LOG_E("ERROR: mic_config struct not allocated!");
		return ERR_INVPAR;
	}

	memset((void*)command, 0, 1024);
	strcat(command, "SELECT * FROM HW_Settings_Mics "
			"WHERE Dev = '");
	strcat(command,     actual_dev);
	strcat(command,  "'");
	LOG_I("Query: %s", command);

	rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		LOG_E("ERROR: Unable to prepare SQL-statement!");
		goto cleanup;
	}

	if (sqlite3_step(stmt) != SQLITE_ROW)
		goto cleanup;

	LOG_I("Found matching mic-config for device '%s'.", actual_dev);

	mic_config->type = sqlite3_column_int(stmt, 1);
	mic_config->n_mics = sqlite3_column_int(stmt, 2);
	for (i = 0; i < mic_config->n_mics; i++) {
		amic_str_db = (const char*)sqlite3_column_text(stmt, 3+i);
		for (j = 0; j < AMIC_COUNT; j++) 
			if (strcmp(amic_str_db, Alsactrl_Hwh_GetMicStr(j)) == 0)
				break;
		if (j == AMIC_COUNT) {
			LOG_E("ERROR: Unknown amic '%s' selected!", amic_str_db);
			goto cleanup;
		}
		mic_config->mics[i] = (enum amic)j;
	}

	ret = 0;

cleanup:
	if (command != NULL) free(command);
	if (stmt != NULL) {
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	return ret;
}

int Alsactrl_DB_GetMicConfig_Generic(sqlite3* db_p, const char *toplevel_dev, mic_config_t *mic_config)
{
	int ret;
	const char* actual_dev = NULL;

	if (mic_config == NULL) {
		LOG_E("ERROR: mic_config struct not allocated!");
		return ERR_INVPAR;
	}

	ret = Alsactrl_Hwh_GetToplevelMapping(toplevel_dev, &actual_dev);
	if (ret != 0) {
		LOG_E("ERROR: Failed to get toplevel-mapping (ret = %d)!", ret);
		return ret;
	}
	LOG_I("'%s' maps to '%s'\n", toplevel_dev, actual_dev);

	ret = Alsactrl_DB_GetMicConfig_Specific(db_p, actual_dev, mic_config);
	if (ret < 0) {
		LOG_E("ERROR: Failed to get mic-config for '%s'!", actual_dev);
		return ret;
	}

	return 0;
}

int Alsactrl_DB_GetDeviceDataIndex(sqlite3* db_p, const char* dev, bool use_sdev)
{
	int rc = SQLITE_OK;
	sqlite3_stmt *stmt = NULL;
	int idx_data = -1;
	char* command = malloc(1024 * sizeof(char));

	memset((void*)command, 0, 1024);
	if (use_sdev)
		strcat(command, "SELECT Idx_Data FROM HW_Settings_SDev WHERE Device = '");
	else
		strcat(command, "SELECT Idx_Data FROM HW_Settings_GDev WHERE Device = '");
	strcat(command, dev);
	strcat(command, "'");
	LOG_I("Query: %s", command);

	rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		LOG_E("ERROR: Unable to prepare SQL-statement!");
		goto cleanup;
	}

	if (sqlite3_step(stmt) != SQLITE_ROW) {
		LOG_E("ERROR: sqlite3_step failed to evaluate the statement");
		goto cleanup;
	}

	LOG_I("Found matching HW-settings for device '%s'.", dev);

	idx_data = sqlite3_column_int(stmt, 0);

cleanup:
	if (command != NULL) free(command);
	if (stmt != NULL) {
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	return idx_data;
}

static int GetComboDataIndex(sqlite3* db_p, char* command)
{
	int rc = SQLITE_OK;
	sqlite3_stmt *stmt = NULL;
	int idx_data = -1;

	LOG_I("Query: %s", command);

		rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		LOG_E("ERROR: Unable to prepare SQL-statement!");
		goto cleanup;
	}

	if (sqlite3_step(stmt) != SQLITE_ROW)
		goto cleanup;

	LOG_I("Found matching HW-settings: devices = ('%s', '%s', '%s', '%s', '%s', '%s').",
		sqlite3_column_text(stmt, 1),
		sqlite3_column_text(stmt, 2),
		sqlite3_column_text(stmt, 3),
		sqlite3_column_text(stmt, 4),
		sqlite3_column_text(stmt, 5),
		sqlite3_column_text(stmt, 6));

	idx_data = sqlite3_column_int(stmt, 7);

cleanup:
	if (stmt != NULL) {
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	return idx_data;
}

char* Alsactrl_DB_GetData(sqlite3* db_p, int idx_data)
{
	int rc = SQLITE_OK;
	sqlite3_stmt *stmt = NULL;
	char* command = malloc(1024 * sizeof(char));
	const unsigned char* data = NULL;
	char* data_ret = NULL;

	memset((void*)command, 0, 1024);
	sprintf(command, "SELECT Data FROM HW_Settings_Data WHERE Idx = %u", idx_data);
	LOG_I("Query: %s", command);

	rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		LOG_E("ERROR: Unable to prepare SQL-statement!");
		goto cleanup;
	}

	if (sqlite3_step(stmt) != SQLITE_ROW)
		goto cleanup;

	data = sqlite3_column_text(stmt, 0);
	if (data == NULL) {
		LOG_E("ERROR: Data not found (idx_data = %d)!\n", idx_data);
		goto cleanup;
	}

	data_ret = strdup((const char*)data);
	if (!data_ret) {
		LOG_E("ERROR: strdup() failed\n");
	}

cleanup:
	if (command != NULL) free(command);
	if (stmt != NULL) {
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	return data_ret;
}

int Alsactrl_DB_WriteDefaultData(sqlite3* db_p)
{
	char* data = NULL;
	int ret = 0;

	data = Alsactrl_DB_GetData(db_p, ALSACTRL_IDX_DEFAULT_STATE);
	if (data == NULL) {
		LOG_E("ERROR: Failed to get default data!");
		return ERR_GENERIC;
	}

	LOG_I("Write default HW-settings.");
	ret = audio_hal_alsa_memctrl_init_default((const char*)data);
	if (ret < 0) {
		LOG_E("ERROR: Failed to write default HW-settings (ret = %d)!", ret);
		return ERR_GENERIC;
	}

	return 0;
}

int Alsactrl_DB_WriteComboData(sqlite3* db_p, hwh_dev_next_t dev_next)
{
	char *command = NULL;
	const char* name = NULL;
	char* data = NULL;
	int idx_data;
	int n_dev_combo = 0;
	int ret;
	int ret_func = 0;

	command = malloc(1024 * sizeof(char));
	memset(command, 0, 1024);

	strcat(command, "SELECT * FROM HW_Settings_Combo WHERE");
	while (dev_next(&name) == 0) {
		if (IsComboDevice(name)) {
			n_dev_combo++;
			if (n_dev_combo > 1)
				strcat(command, " AND");
			strcat(command, " ('");
			strcat(command, name);
			strcat(command, "' IN (Dev1, Dev2, Dev3, Dev4, Dev5, Dev6)) ");
		}
	}
	if (n_dev_combo <= 1) {
		LOG_I("No of devices less than 2 (n_dev_combo = %d). Returning...", n_dev_combo);
		goto cleanup;
	}

	strcat(command, " ORDER BY rowid LIMIT 1");

	idx_data = GetComboDataIndex(db_p, command);
	if (idx_data == -1) {
		LOG_I("WARNING: Data-index not found (idx_data = %d)!", idx_data);
		goto cleanup;
	}

	data = Alsactrl_DB_GetData(db_p, idx_data);
	if (data == NULL) {
		LOG_E("ERROR: Failed to get data with index = %d!", idx_data);
		ret_func = ERR_GENERIC;
		goto cleanup;
	}

	LOG_I("Write HW-settings from file to ALSA-interface.");
	ret = audio_hal_alsa_memctrl_set((const char*)data);
	if (ret < 0) {
		LOG_E("ERROR: Failed to write HW-settings (ret = %d)!", ret);
		ret_func = ERR_GENERIC;
		goto cleanup;
	}

cleanup:
	if (command != NULL) free(command);

	return ret_func;
}

int Alsactrl_DB_WriteDevData(sqlite3* db_p, hwh_dev_next_t dev_next, bool use_sdev)
{
	const char* gdev = NULL;
	const char* sdev = NULL;
	const char* dev = NULL;
	char* data = NULL;
	int idx_data;

	int ret = 0;

	/* Device-data */
	while (dev_next(&gdev) == 0) {
		if (strlen(gdev) == 0)
			continue;

		if (use_sdev) {
			ret = Alsactrl_Hwh_GetToplevelMapping(gdev, &sdev);
			if (ret < 0) {
				LOG_E("ERROR: Unable to find specific device for '%s'!\n", gdev);
				ret = ERR_GENERIC;
				goto interrupted_loop;
			}
			LOG_I("'%s' maps to '%s'\n", gdev, sdev);
			dev = sdev;
		} else
			dev = gdev;

		LOG_I("Getting data-index for %s device '%s'.", use_sdev ? "specific" : "generic", dev);
		idx_data = Alsactrl_DB_GetDeviceDataIndex(db_p, dev, use_sdev);
		if (idx_data == -1) {
			LOG_I("No data exists for device '%s'!", dev);
			continue;
		}

		data = Alsactrl_DB_GetData(db_p, idx_data);
		if (data == NULL) {
			LOG_E("ERROR: Failed to get data with index = %d!", idx_data);
			ret = ERR_GENERIC;
			goto interrupted_loop;
		}

		LOG_I("Write HW-settings for device '%s'.", dev);
		ret = audio_hal_alsa_memctrl_set((const char*)data);
		if (ret < 0) {
			LOG_E("ERROR: Failed to write HW-settings (ret = %d)!", ret);
			ret = ERR_GENERIC;
			goto interrupted_loop;
		}
	};

	return 0;

interrupted_loop:
	while (dev_next(&gdev) == 0) {};

	return ret;
}
