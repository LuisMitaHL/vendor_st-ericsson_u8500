/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_routing_u9540.c
    \brief Implementation of routing for U9540 platform.

    @todo Describe
*/

#define ADM_LOG_FILENAME "hwh_u9540"

#include <sys/time.h>

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sqlite3.h>

#include "ste_adm_client.h"
#include "ste_adm_db.h"
#include "ste_adm_dbg.h"
#include "ste_adm_hw_handler.h"
#include "ste_adm_srv.h"

#include "audio_hwctrl_interface.h"
#include "audio_hwctrl_interface_alsa.h"

static const char* codec_name_ab9540_p = "AB9540";
//static const char* codec_name_cg92xx_p = "CG29XX";
static const char* card_name_p = NULL;


enum microphone_type {
    MICROPHONE_TYPE_ANALOG = 0,
    MICROPHONE_TYPE_DIGITAL = 1
};

enum fm_type {
    FM_TYPE_ANALOG = 0,
    FM_TYPE_DIGITAL = 1
};

enum amic_regulator {
    REGULATOR_AMIC1 = 0,
    REGULATOR_AMIC2 = 1
};

static enum microphone_type mic_type = MICROPHONE_TYPE_ANALOG;

static enum fm_type fmrx_type = FM_TYPE_DIGITAL;

static enum amic_regulator mic_1a_regulator = REGULATOR_AMIC1;
static enum amic_regulator mic_1b_regulator = REGULATOR_AMIC1;
static enum amic_regulator mic_2_regulator = REGULATOR_AMIC2;

static int is_asoc_device(const char* name)
{
    static const char *asoc_devs[] = {
        STE_ADM_DEVICE_STRING_MIC,
        STE_ADM_DEVICE_STRING_EARP,
        STE_ADM_DEVICE_STRING_SPEAKER,
        STE_ADM_DEVICE_STRING_HSIN,
        STE_ADM_DEVICE_STRING_HSOUT,
        STE_ADM_DEVICE_STRING_VIBL,
        STE_ADM_DEVICE_STRING_VIBR,
        STE_ADM_DEVICE_STRING_TTYIN,
        STE_ADM_DEVICE_STRING_TTYOUT,
        NULL
    };

    const char** cur = asoc_devs;
    while(*cur) {
        if (strcmp(*cur, name) == 0) {
            return 1;
        }
        cur++;
    }
    return 0;
}

static int GetMICChannelCount(sqlite3* db_p, const char* dev)
{
    int rc = SQLITE_OK;
    sqlite3_stmt *stmt = NULL;
    const char* dev_top = NULL;
    char* command = malloc(1024 * sizeof(char));
    int nChannels = -1;
    ste_adm_res_t res;

    res = adm_db_toplevel_mapping_get(dev, &dev_top);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("%s: ERROR: Unable to get top-level device for %s!", __func__, dev);
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
    ALOG_INFO("Query: %s", command);

    rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("%s: ERROR: Unable to prepare SQL-statement!", __func__);
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto cleanup;

    nChannels = sqlite3_column_int(stmt, 0);
    if (nChannels <= 0) {
        ALOG_ERR("%s: ERROR: Illegal number of channels (nChannels = %d)!\n", __func__, nChannels);
        goto cleanup;
    }

cleanup:
    if (command != NULL) free(command);
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    return nChannels;
}

static int GetComboDataIndex(sqlite3* db_p, char* command)
{
    int rc = SQLITE_OK;
    sqlite3_stmt *stmt = NULL;
    int idx_data = -1;

    ALOG_INFO("Query: %s", command);

    rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("%s: ERROR: Unable to prepare SQL-statement!", __func__);
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto cleanup;

    ALOG_INFO("%s: Found matching HW-settings: devices = ('%s', '%s', '%s', '%s', '%s', '%s').",
        __func__,
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

static int GetDeviceDataIndex(sqlite3* db_p, const char* dev_top)
{
    int rc = SQLITE_OK;
    sqlite3_stmt *stmt = NULL;
    int idx_data = -1;
    char* command = malloc(1024 * sizeof(char));

    memset((void*)command, 0, 1024);
    strcat(command, "SELECT Idx_Data FROM HW_Settings_Device "
                    "WHERE Device = '");
    strcat(command,     dev_top);
    strcat(command,  "'");
    ALOG_INFO("Query: %s", command);

    rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("%s: ERROR: Unable to prepare SQL-statement!", __func__);
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto cleanup;

    ALOG_INFO("%s: Found matching HW-settings for device '%s'.", __func__, dev_top);

    idx_data = sqlite3_column_int(stmt, 0);

cleanup:
    if (command != NULL) free(command);
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    return idx_data;
}

static char* GetData(sqlite3* db_p, int idx_data)
{
    int rc = SQLITE_OK;
    sqlite3_stmt *stmt = NULL;
    char* command = malloc(1024 * sizeof(char));
    const unsigned char* data = NULL;
    char* data_ret = NULL;

    memset((void*)command, 0, 1024);
    sprintf(command, "SELECT Data FROM HW_Settings_Data WHERE Idx = %u", idx_data);
    ALOG_INFO("Query: %s", command);

    rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("%s: ERROR: Unable to prepare SQL-statement!", __func__);
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto cleanup;

    data = sqlite3_column_text(stmt, 0);
    if (data == NULL) {
        ALOG_ERR("%s: ERROR: Data not found (idx_data = %d)!\n", __func__, idx_data);
        goto cleanup;
    }

    data_ret = strdup((const char*)data);
    if (!data_ret) {
        ALOG_ERR("%s: ERROR: strdup() failed\n", __func__);
    }

cleanup:
    if (command != NULL) free(command);
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    return data_ret;
}

static int FMRX_HSETOUT_FLAG = 0;
static int FMRX_SPEAKER_FLAG = 0;
static int SPEAKER_FLAG = 0;
static int HSETOUT_FLAG = 0;

static void ClearFlags()
{
    FMRX_HSETOUT_FLAG = 0;
    FMRX_SPEAKER_FLAG = 0;
    SPEAKER_FLAG = 0;
    HSETOUT_FLAG = 0;
}

static void UpdateFlags(const char* name)
{
    if(strcmp(name, "HSetOut") == 0 || strcmp(name, "TTYOut") == 0){
        HSETOUT_FLAG = 1;
    }
    if(strcmp(name, "Speaker") == 0){
        SPEAKER_FLAG = 1;
    }
}

static void UpdateD2DFlags(const char* src_name, const char* dst_name)
{
    if(strcmp(src_name, "FMRX") == 0 && strcmp(dst_name, "HSetOut") == 0){
        FMRX_HSETOUT_FLAG = 1;
        HSETOUT_FLAG = 1;
    }
    if(strcmp(src_name, "FMRX") == 0 && strcmp(dst_name, "Speaker") == 0){
        FMRX_SPEAKER_FLAG = 1;
        SPEAKER_FLAG = 1;
    }
}

static void ExecuteFlags()
{
    audio_hal_alsa_set_control("LineIn Left to Headset Left", 0, FMRX_HSETOUT_FLAG);
    audio_hal_alsa_set_control("LineIn Right to Headset Right", 0, FMRX_HSETOUT_FLAG);
    audio_hal_alsa_set_control("AD1 to IHF Left", 0, FMRX_SPEAKER_FLAG);
    audio_hal_alsa_set_control("AD2 to IHF Right", 0, FMRX_SPEAKER_FLAG);
    audio_hal_alsa_set_control("IHF Left", 0, SPEAKER_FLAG);
    audio_hal_alsa_set_control("IHF Right", 0, SPEAKER_FLAG);
    audio_hal_alsa_set_control("Headset Left", 0, HSETOUT_FLAG);
    audio_hal_alsa_set_control("Headset Right", 0, HSETOUT_FLAG);
}

static void AppendD2D(sqlite3* db_p, hw_handler_dev_to_dev_next_fp_t dev_next_d2d_fp)
{
    int rc = SQLITE_OK;
    sqlite3_stmt *stmt = NULL;
    const char* src_name = NULL;
    const char* dst_name = NULL;
    char *command = NULL;
    const unsigned char* data = NULL;
    int ret;

    ALOG_INFO("%s ENTER", __func__);

    command = malloc(1024 * sizeof(char));
    while(dev_next_d2d_fp(&src_name, &dst_name) == 0) {
        UpdateD2DFlags(src_name, dst_name);
        memset(command, 0, 1024);
        sprintf(command, "SELECT Data FROM HW_Settings_Data_D2D WHERE Idx=(\
                    SELECT Idx_Data FROM HW_Settings_Combo_D2D WHERE (Codec = '%s') AND (Src='%s') AND (Dst='%s'))",
                    codec_name_ab9540_p, src_name, dst_name);

        rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("%s: ERROR: Unable to prepare SQL-statement!", __func__);
            goto cleanup;
        }

        if (sqlite3_step(stmt) != SQLITE_ROW)
            goto cleanup;

        data = sqlite3_column_text(stmt, 0);
        if (data == NULL) {
            ALOG_ERR("%s: ERROR: Data not found !\n", __func__);
            goto cleanup;
        }
        ret = audio_hal_alsa_set_controls_cfg((const char*)data);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Failed to write HW-settings! ste_adm_hw_handler_alsa_set_controls returned %d.", __func__, ret);
            goto cleanup;
        }
        ALOG_INFO("%s: found Match src: %s, dst :%s \n %s\n", __func__, src_name, dst_name, data);
    }

cleanup:
    if (command != NULL) free(command);
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }
}

int ste_adm_hw_handler_u9540(sqlite3* db_p, hw_handler_dev_next_fp_t dev_next_fp, hw_handler_dev_to_dev_next_fp_t dev_next_d2d_fp, fadeSpeed_t fadeSpeed)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    const char* name = NULL;
    char* dev_names = NULL;
    int ret = 0;
    int ret_func = STE_ADM_RES_ALSA_ERROR;
    char *command = NULL;
    char* data = NULL;
    int is_input, nDev_combo, idx_data;
    bool activeFMRx = false;
    bool activeFMTx = false;
    bool activeOutputDev = false;
    bool activeInputDev = false;
    bool activeInputDev_MIC = false;
    const char* dev_top = NULL;
    bool controls_open = false;

    ALOG_INFO("%s: Enter.", __func__);

    // Make sure that the settings that is written from ADM is not overwritten
    // by any sink or source
    audio_hal_alsa_set_ignore_defaults(true);

    // Create SQL-statement
    ClearFlags();
    command = malloc(1024 * sizeof(char));
    memset(command, 0, 1024);

    nDev_combo = 0;
    strcat(command, "SELECT * FROM HW_Settings_Combo WHERE (Codec = '");
    strcat(command, codec_name_ab9540_p);
    strcat(command, "')");
    while (dev_next_fp(&name) == 0) {
        if (strcmp(name, STE_ADM_DEVICE_STRING_FMRX) == 0) {
            activeFMRx = true;
        } else if (strcmp(name, STE_ADM_DEVICE_STRING_FMTX) == 0) {
            activeFMTx = true;
        } else if (is_asoc_device(name)) {
            if (adm_db_io_info(name, &is_input) != STE_ADM_RES_OK) {
                ALOG_INFO("%s: Warning: Device direction not found for device %s!", __func__, name);
            } else {
                if (is_input == 1) {
                    activeInputDev = true;
                    if (strcmp(name, STE_ADM_DEVICE_STRING_MIC) == 0) {
                        activeInputDev_MIC = true;
                    }
                } else {
                    activeOutputDev = true;
                }
            }
            UpdateFlags(name);
            nDev_combo++;
            strcat(command, " AND ('");
            strcat(command, name);
            strcat(command, "' IN (Dev1, Dev2, Dev3, Dev4, Dev5, Dev6)) ");
        }
    }

    // Become worker thread before opening ALSA controls (to avoid having the
    // "normal-thread" lock at the same time as the ALSA lock)
    srv_become_worker_thread();

    if (audio_hal_alsa_open_controls(card_name_p) < 0) {
        ALOG_ERR("%s: ERROR: Unable to open ALSA-card '%s'!\n", __func__, card_name_p);
        goto cleanup_worker;
    }
    controls_open = true;

    if (nDev_combo == 0) {
        strcat(command, " AND (Dev1 ='')");
    }
    strcat(command, " ORDER BY rowid LIMIT 1");

    /* Combo-data */
    idx_data = GetComboDataIndex(db_p, command);
    if (idx_data == -1) {
        ALOG_INFO("%s: WARNING: Data-index not found (idx_data = %d)!", __func__, idx_data);
        ret_func = STE_ADM_RES_OK;
        goto cleanup_worker;
    }

    data = GetData(db_p, idx_data);
    if (data == NULL) {
        ALOG_ERR("%s: ERROR: Failed to get data with index = %d!", __func__, idx_data);
        goto cleanup_worker;
    }

    ALOG_INFO("%s: Write HW-settings from file to ALSA-interface.", __func__);
    ret = audio_hal_alsa_set_controls_cfg((const char*)data);
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Failed to write HW-settings! ste_adm_hw_handler_alsa_set_controls returned %d.", __func__, ret);
        goto cleanup_worker;
    }

    if (data != NULL) {
        free(data);
        data = NULL;
    }

    /* Device-data */
    while (dev_next_fp(&name) == 0) {
        if (strlen(name) == 0)
            continue;

        ret = adm_db_toplevel_mapping_get(name, &dev_top);
        if (ret < 0) {
            ALOG_INFO("%s: ERROR: Unable to find top-level device for '%s'!\n", __func__, name);
            goto cleanup_worker;
        }
        ALOG_INFO("%s: '%s' maps to '%s'\n", __func__, name, dev_top);

        idx_data = GetDeviceDataIndex(db_p, dev_top);
        if (idx_data == -1) {
            ALOG_INFO("%s: No top-level device-specific data exists for '%s'!", __func__, dev_top);
            continue;
        }

        if (data != NULL) {
            free(data);
            data = NULL;
        }
        data = GetData(db_p, idx_data);
        if (data == NULL) {
            ALOG_ERR("%s: ERROR: Failed to get data with index = %d!", __func__, idx_data);
            goto cleanup_worker;
        }

        // Set Digital Gain Fade Speed Switch
        audio_hal_alsa_set_control("Digital Gain Fade Speed Switch", 0, fadeSpeed);

        ALOG_INFO("%s: Write HW-settings for device '%s'.", __func__, dev_top);
        ret = audio_hal_alsa_set_controls_cfg((const char*)data);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Failed to write HW-settings! ste_adm_hw_handler_alsa_set_controls returned %d.", __func__, ret);
            goto cleanup_worker;
        }
    };

    /* Appending device to device connections */
    AppendD2D(db_p, dev_next_d2d_fp);
    ExecuteFlags();

    /* FM-devices */
    if (activeFMRx || activeFMTx) {
        ALOG_INFO("%s: FM-device active!", __func__);

        if (activeFMRx) {
            configure_fm_t config;

            if (FM_TYPE_DIGITAL == fmrx_type) {
                ALOG_INFO("%s: FMRx digital active! Setting DA-from-slot and AD-to-slot mapping...", __func__);

                config.type = AUDIO_HAL_DIGITAL;

                audio_hal_alsa_set_control("Digital Interface AD 5 Loopback Switch", 0, 1);
                audio_hal_alsa_set_control("Digital Interface AD 6 Loopback Switch", 0, 1);
                audio_hal_alsa_set_control("Digital Interface AD To Slot 6 Map", 0, 4); // REG_ADSLOTSELX_AD_OUT5_TO_SLOT_EVEN
                audio_hal_alsa_set_control("Digital Interface AD To Slot 7 Map", 0, 5); // REG_ADSLOTSELX_AD_OUT6_TO_SLOT_ODD
                audio_hal_alsa_set_control("Digital Interface DA 7 From Slot Map", 0, 24); // Slot 24 -> DA_IN7
                audio_hal_alsa_set_control("Digital Interface DA 8 From Slot Map", 0, 25); // Slot 25 -> DA_IN8
            } else {
                ALOG_INFO("%s: FMRx analog active! Setting DA-from-slot and AD-to-slot mapping...", __func__);

                config.type = AUDIO_HAL_ANALOG;

                audio_hal_alsa_set_control("Digital Interface AD 5 Loopback Switch", 0, 0);
                audio_hal_alsa_set_control("Digital Interface AD 6 Loopback Switch", 0, 0);
                audio_hal_alsa_set_control("Digital Interface AD To Slot 6 Map", 0, 0);
                audio_hal_alsa_set_control("Digital Interface AD To Slot 7 Map", 0, 1);
                audio_hal_alsa_set_control("AD 1 Select Capture Route", 0, 0);
                audio_hal_alsa_set_control("AD 2 Select Capture Route", 0, 0);
                audio_hal_alsa_set_control("Mic 2 or LINR Select Capture Route", 0, 1);
                audio_hal_alsa_set_control("LineIn Left", 0, 1);
                audio_hal_alsa_set_control("LineIn Right", 0, 1);
            }
            audio_hal_configure_channel(AUDIO_HAL_CHANNEL_FMRX, &config);

        } else if (activeFMTx) {
            ALOG_INFO("%s: FMTx active! Setting DA-from-slot and AD-to-slot mapping...", __func__);
            audio_hal_alsa_set_control("Digital Interface AD 5 Loopback Switch", 0, 1);
            audio_hal_alsa_set_control("Digital Interface AD 6 Loopback Switch", 0, 1);
            audio_hal_alsa_set_control("Digital Interface AD To Slot 16 Map", 0, 4); // REG_ADSLOTSELX_AD_OUT5_TO_SLOT_EVEN
            audio_hal_alsa_set_control("Digital Interface AD To Slot 17 Map", 0, 5); // REG_ADSLOTSELX_AD_OUT6_TO_SLOT_ODD
            audio_hal_alsa_set_control("Digital Interface DA 7 From Slot Map", 0, 14); // Slot 14 -> DA_IN7
            audio_hal_alsa_set_control("Digital Interface DA 8 From Slot Map", 0, 15); // Slot 15 -> DA_IN8
        }
    }

    /* Input-devices */
    if (activeInputDev) {
        int ADSel_slot0 = 2; // REG_ADSLOTSELX_AD_OUT3_TO_SLOT_EVEN
        int ADSel_slot1 = 1; // REG_ADSLOTSELX_AD_OUT2_TO_SLOT_ODD
        ALOG_INFO("%s: Input-device active!", __func__);
        if (activeInputDev_MIC) {
            int nChannels = GetMICChannelCount(db_p, STE_ADM_DEVICE_STRING_MIC);
            if (mic_type == MICROPHONE_TYPE_DIGITAL) {
                ALOG_INFO("%s: Device MIC actice (mic_type = MICROPHONE_TYPE_DIGITAL, nChannels = %d).", __func__, nChannels);
                ADSel_slot0 = 0;
                ADSel_slot1 = 1;
                audio_hal_alsa_set_control("AD 1 Select Capture Route", 0, 1); // DMic 1
                if (nChannels > 1)
                    audio_hal_alsa_set_control("AD 2 Select Capture Route", 0, 1); // DMic 2
            } else {
                ALOG_INFO("%s: Device MIC active (mic_type = MICROPHONE_TYPE_ANALOG, nChannels = %d).", __func__, nChannels);
                audio_hal_alsa_set_control("AD 3 Select Capture Route", 0, 0); // Mic 1
                if (nChannels > 1) {
                    audio_hal_alsa_set_control("AD 2 Select Capture Route", 0, 0); // LineIn (Mic 2)
                    audio_hal_alsa_set_control("Mic 2 or LINR Select Capture Route", 0, 0); // Mic 2
                } else {
                    // Force dual mono
                    ADSel_slot0 = 2; // REG_ADSLOTSELX_AD_OUT3_TO_SLOT_EVEN
                    ADSel_slot1 = 2; // REG_ADSLOTSELX_AD_OUT3_TO_SLOT_ODD
                }
            }
            audio_hal_alsa_set_control("DMic 1", 0, mic_type == MICROPHONE_TYPE_DIGITAL);
            audio_hal_alsa_set_control("DMic 2", 0, (mic_type == MICROPHONE_TYPE_DIGITAL) && (nChannels > 1));
            audio_hal_alsa_set_control("Mic 1", 0, mic_type != MICROPHONE_TYPE_DIGITAL);
            audio_hal_alsa_set_control("Mic 2", 0, (mic_type != MICROPHONE_TYPE_DIGITAL) && (nChannels > 1));
        }
        audio_hal_alsa_set_control("Digital Interface AD To Slot 0 Map", 0, ADSel_slot0);
        audio_hal_alsa_set_control("Digital Interface AD To Slot 1 Map", 0, ADSel_slot1);
    }

    /* Output-devices */
    if (activeOutputDev) {
        ALOG_INFO("%s: Output-device active! Setting DA from Slot mapping...", __func__);
        audio_hal_alsa_set_control("Digital Interface DA 1 From Slot Map", 0, 8); // Slot 8 -> DA_IN1
        audio_hal_alsa_set_control("Digital Interface DA 2 From Slot Map", 0, 9); // Slot 9 -> DA_IN2
        audio_hal_alsa_set_control("Digital Interface DA 3 From Slot Map", 0, 10); // Slot 10 -> DA_IN3
        audio_hal_alsa_set_control("Digital Interface DA 4 From Slot Map", 0, 11); // Slot 11 -> DA_IN4
        audio_hal_alsa_set_control("Digital Interface DA 5 From Slot Map", 0, 12); // Slot 12 -> DA_IN5
        audio_hal_alsa_set_control("Digital Interface DA 6 From Slot Map", 0, 13); // Slot 13 -> DA_IN6
    }

    ret_func = STE_ADM_RES_OK;

cleanup_worker:
    if (controls_open) {
      audio_hal_alsa_close_controls();
    }
    srv_become_normal_thread();

    if (command != NULL) free(command);
    if (dev_names != NULL) free(dev_names);
    if (data != NULL) free(data);

    ALOG_INFO("%s: Exit (%s).", __func__, (ret_func == STE_ADM_RES_OK) ? "OK" : "ERROR");

    return ret_func;
}

static int set_sidetone(sqlite3_stmt *stmt)
{
    const unsigned int NR_OF_SIDETONE_FIR_COEFFS = 128;
    long sid_fir_coefficients[NR_OF_SIDETONE_FIR_COEFFS];
    unsigned int i;
    int value, ret;
    long gain[2];

    value = sqlite3_column_int(stmt, 1);
    gain[0] = value;
    gain[1] = value;
    ALOG_INFO("%s: Configuring sidetone gain = %d,%d\n", __func__, gain[0], gain[1]);
    ret = audio_hal_alsa_set_control_values("Sidetone Digital Gain Playback Volume", gain);
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to set sidetone gain! (ret = %d)\n", __func__, ret);
        return ret;
    }
    if (gain[0] == 0 && gain[1] == 0) {
        ALOG_INFO("%s: No need to set sidetone coefficients with zero gain.\n", __func__);
        ret = audio_hal_alsa_set_control("Sidetone Left", 0, 0);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Unable to disable sidetone left! (ret = %d)\n", __func__, ret);
            return ret;
        }
        ret = audio_hal_alsa_set_control("Sidetone Right", 0, 0);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Unable to disable sidetone right! (ret = %d)\n", __func__, ret);
            return ret;
        }
    }

    // 128 unsigned 16bit FIR-coefficients
    ALOG_INFO("%s: Configuring sidetone FIR-coefficients.\n", __func__);
    for (i = 0; i < NR_OF_SIDETONE_FIR_COEFFS; i++)
        sid_fir_coefficients[i] = sqlite3_column_int(stmt, i + 2);
    ret = audio_hal_alsa_set_control_values("Sidetone FIR Coefficients", sid_fir_coefficients);
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to set Sidetone FIR-coefficients!\n", __func__);
        return ret;
    }

    for (i = 0; i < MAX_TRIES_SID; i++) {
        ret = audio_hal_alsa_set_control("Sidetone FIR Apply Coefficients", 0, 1);
        if ((ret == 0) || ((ret < 0) && (ret != -EBUSY)))
            break;
        ALOG_INFO("%s: Unable to apply sidetone coefficients (try %d)!\n", __func__, i);
        usleep(5 * 1000);
    }
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to apply sidetone coefficients (MAX_TRIES_SID = %d)! (ret = %d)\n", __func__, MAX_TRIES_SID, ret);
        return ret;
    }

    ret = audio_hal_alsa_set_control("Sidetone Left Source Playback Route", 0, 2); // 'Mic 1'
    ret |= audio_hal_alsa_set_control("Sidetone Right Source Playback Route", 0, 1); // 'Mic 1'
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to set sidetone source to 'Mic 1'! (ret = %d)\n", __func__, ret);
        return ret;
    }

    ret = audio_hal_alsa_set_control("Sidetone Left", 0, 1);
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to enable sidetone left path! (ret = %d)\n", __func__, ret);
        return ret;
    }

    ret = audio_hal_alsa_set_control("Sidetone Right", 0, 1);
    if (ret < 0)
        ALOG_ERR("%s: ERROR: Unable to enable sidetone right path! (ret = %d)\n", __func__, ret);

    return ret;
}

static int set_anc(sqlite3_stmt *stmt)
{
    const unsigned int NR_OF_ANC_FIR_COEFFS = 15;
    const unsigned int NR_OF_ANC_IIR_COEFFS = 24;
    long anc_fir_coefficients[NR_OF_ANC_FIR_COEFFS];
    long anc_iir_coefficients[NR_OF_ANC_IIR_COEFFS];
    unsigned int i;
    int value, ret;

    // 15 signed 16bit FIR-coefficients
    ALOG_INFO("%s: Configuring ANC FIR-coefficients.\n", __func__);
    for (i = 0; i < NR_OF_ANC_FIR_COEFFS; i++)
        anc_fir_coefficients[i] = sqlite3_column_int(stmt, i + 5);
    ret = audio_hal_alsa_set_control_values("ANC FIR Coefficients", anc_fir_coefficients);
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to set ANC FIR-coefficients!\n", __func__);
        return ret;
    }

    // 1bit double delay + 8 signed 5bit internal IIR shift + 20 signed 24bit IIR-coefficients
    ALOG_INFO("%s: Configuring ANC IIR-shift and IIR-coefficients.\n", __func__);
    for (i = 0; i < NR_OF_ANC_IIR_COEFFS; i++)
        anc_iir_coefficients[i] = sqlite3_column_int(stmt, i + 20);
    ret = audio_hal_alsa_set_control_values("ANC IIR Coefficients", anc_iir_coefficients);
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to set ANC IIR-coefficients!\n", __func__);
        return ret;
    }

    // 3 signed 5bit external shift parameters and 1 unsigned 16bit warp delay parameter
    ALOG_INFO("%s: Configuring ANC Warp and Shifts.\n", __func__);
    for (i = 0; i < 4; i++) {
        value = sqlite3_column_int(stmt, i + 1);
        if (i == 0)
            ret = audio_hal_alsa_set_control("ANC Warp Delay Shift", 0, value);
        else if (i == 1)
            ret = audio_hal_alsa_set_control("ANC FIR Output Shift", 0, value);
        else if (i == 2)
            ret = audio_hal_alsa_set_control("ANC IIR Output Shift", 0, value);
        else if (i == 3)
            ret = audio_hal_alsa_set_control("ANC Warp Delay", 0, value);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Unable to set ANC IIR Warp or Shift!\n", __func__);
            return ret;
        }
    }

    // configure FIR-/IIR-coefficients into ab9540 ANC block
    ret = audio_hal_alsa_set_control("ANC Status", 0, 1);
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to apply ANC FIR+IIR-coefficients!\n", __func__);
        return ret;
    }

    ret = audio_hal_alsa_set_control("ANC Playback Switch", 0, 1);
    if (ret < 0)
        ALOG_ERR("%s: ERROR: Unable to enable ANC! (ret = %d)\n", __func__, ret);

    return ret;
}

static int set_filters(sqlite3* db_p, const char* indev_p, const char* outdev_p, int fs)
{
    int ret_func = STE_ADM_RES_ALSA_ERROR;
    char *command = NULL;
    char *c_criteria = NULL;
    const char* preset = NULL;
    sqlite3_stmt *stmt = NULL;
    int ret;
    char str_fs[6];

    ALOG_INFO("%s: Enter.", __func__);

	sprintf(str_fs, "%d", fs);
    c_criteria = malloc(192 * sizeof(char));
    memset(c_criteria, 0, 192 * sizeof(char));
    strcat(c_criteria, " AND SpeechConfig.InDev = '");
    strcat(c_criteria, indev_p);
    strcat(c_criteria, "' AND SpeechConfig.OutDev = '");
    strcat(c_criteria, outdev_p);
    strcat(c_criteria, "' AND SpeechConfig.SampleRate = '");
    strcat(c_criteria, str_fs);
    strcat(c_criteria, "'");

    command = malloc(512 * sizeof(char));
    memset(command, 0, 512 * sizeof(char));
    strcat(command, "SELECT HW_Settings_FIR.* FROM SpeechConfig, HW_Settings_FIR WHERE SpeechConfig.SidetonePreset = HW_Settings_FIR.Preset");
    strcat(command, c_criteria);

    ret = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
    if (ret != SQLITE_OK) {
        ALOG_ERR("%s: Unable to prepare SQL-statement! \"%s\" (ret = %d)\n", __func__, command, ret);
        ret = -1;
        goto cleanup;
    }
    ret = sqlite3_step(stmt);
    if (ret != SQLITE_ROW) {
        ALOG_WARN("%s: No sidetone preset found in DB for indev = '%s', outdev = '%s'!", __func__, indev_p, outdev_p);
        ret = 0;
        goto sidetone_done;
    }

    preset = (const char*)sqlite3_column_text(stmt, 0);
    ALOG_INFO("%s: Found sidetone preset = '%s' matching indev = '%s', outdev = '%s'.", __func__, preset, indev_p, outdev_p);

    ALOG_INFO("%s: Configuring sidetone.", __func__);
    ret = set_sidetone(stmt);
    if (ret < 0) {
        ALOG_ERR("%s: Failed to configure sidetone! (ret = %d)\n", __func__, ret);
        goto cleanup;
    } else {
        ALOG_INFO("%s: Sidetone configured.\n", __func__);
    }

sidetone_done:
    if (stmt != NULL) {
        ret = sqlite3_finalize(stmt);
        stmt = NULL;
        if (ret != SQLITE_OK)
            ALOG_WARN("%s: Failed to destroy prepared statement after use! (ret = %d)", __func__, ret);
    }

    memset(command, 0, 512 * sizeof(char));
    strcat(command, "SELECT HW_Settings_ANC.* FROM SpeechConfig, HW_Settings_ANC WHERE SpeechConfig.ANCPreset = HW_Settings_ANC.Preset");
    strcat(command, c_criteria);
    ret = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
    if (ret != SQLITE_OK) {
        ALOG_ERR("%s: Unable to prepare SQL-statement! \"%s\" (ret = %d)\n", __func__, command, ret);
        ret = -1;
        goto cleanup;
    }
    ret = sqlite3_step(stmt);
    if (ret != SQLITE_ROW) {
        ALOG_WARN("%s: No ANC preset found in DB for indev = '%s', outdev = '%s'.", __func__, indev_p, outdev_p);
        ret = 0;
        goto anc_done;
    }
    preset = (const char*)sqlite3_column_text(stmt, 0);
    ALOG_INFO("%s: Found ANC-preset = '%s' matching indev = '%s', outdev = '%s'.", __func__, preset, indev_p, outdev_p);

    ALOG_INFO("%s: Configuring ANC.", __func__);
    ret = set_anc(stmt);
    if (ret < 0) {
        ALOG_ERR("%s: Failed to configure ANC! (ret = %d)\n", __func__, ret);
        goto cleanup;
    } else {
        ALOG_INFO("%s: ANC configured.\n", __func__);
    }

anc_done:
    ret_func = STE_ADM_RES_OK;

cleanup:
    if (stmt != NULL) {
        ret = sqlite3_finalize(stmt);
        stmt = NULL;
        if (ret != SQLITE_OK)
            ALOG_WARN("%s: Failed to destroy prepared statement after use! (ret = %d)", __func__, ret);
    }

    if (c_criteria != NULL)
        free(c_criteria);
    if (command != NULL)
        free(command);

    ALOG_INFO("%s: Exit (%s).", __func__, (ret_func == STE_ADM_RES_OK) ? "OK" : "ERROR");

    return ret_func;
}

int ste_adm_hw_handler_vc_u9540(sqlite3* db_p, const char* indev, const char* outdev, int fs)
{
    bool ret, vc_on;
    int ret_func = STE_ADM_RES_ALSA_ERROR;
    const char* dev_top = NULL;
    char* outdev_top = NULL;
    char* indev_top = NULL;

    ALOG_INFO("%s: Enter.", __func__);

    vc_on = (indev != NULL) && (outdev != NULL);
    if (vc_on)
        ALOG_INFO("%s: VC on (indev = '%s', outdev = '%s').", __func__, indev, outdev);
    else
        ALOG_INFO("%s: VC off.", __func__);

    // Become worker thread before opening ALSA controls (to avoid having the
    // "normal-thread" lock at the same time as the ALSA lock)
    srv_become_worker_thread();

    audio_hal_alsa_open_controls(card_name_p);

    ret = audio_hal_alsa_set_control("Master Clock Select", 0, vc_on ? 0 : 1); // VC -> SYSCLK,  Non-VC -> ULPCLK
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to set audio master-clock!\n", __func__, card_name_p);
        goto cleanup;
    }

    if (vc_on) {
        ret = adm_db_toplevel_mapping_get(indev, &dev_top);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Unable to find top-level device for '%s'!\n", __func__, indev);
            goto cleanup;
        }
        indev_top = strdup(dev_top);
        if (!indev_top) {
            ALOG_ERR("%s: ERROR: strdup() failed\n", __func__);
            goto cleanup;
        }

        ret = adm_db_toplevel_mapping_get(outdev, &dev_top);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Unable to find top-level device for '%s'!\n", __func__, outdev);
            goto cleanup;
        }
        outdev_top = strdup(dev_top);
        if (!outdev_top) {
            ALOG_ERR("%s: ERROR: strdup() failed\n", __func__);
            goto cleanup;
        }

        ALOG_INFO("%s: '%s','%s' maps to '%s','%s' (fs = %d)\n", __func__, indev, outdev, indev_top, outdev_top, fs);

        ret = set_filters(db_p, indev_top, outdev_top, fs);
        if (ret < 0) {
            ALOG_ERR("%s: Failed to configure filters! (ret = %d)\n", __func__, ret);
            goto cleanup;
        }

    } else {
        ret = audio_hal_alsa_set_control("Sidetone Left", 0, 0);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Unable to disable sidetone left path! (ret = %d)\n", __func__, ret);
            goto cleanup;
        }
        ret = audio_hal_alsa_set_control("Sidetone Right", 0, 0);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Unable to disable sidetone right path! (ret = %d)\n", __func__, ret);
            goto cleanup;
        }
        ret = audio_hal_alsa_set_control("ANC Playback Switch", 0, 0);
        if (ret < 0) {
            ALOG_ERR("%s: ERROR: Unable to disable ANC! (ret = %d)\n", __func__, ret);
            goto cleanup;
        }
    }

    ret_func = STE_ADM_RES_OK;

cleanup:
    if (outdev_top != NULL) free(outdev_top);
    if (indev_top != NULL) free(indev_top);

    audio_hal_alsa_close_controls();
    srv_become_normal_thread();

    return ret_func;
}

int ste_adm_hw_handler_init_u9540(const char* card_name)
{
    int ret = 0;
    int ret_func = STE_ADM_RES_ALSA_ERROR;

    ALOG_INFO("%s: Enter (card_name = '%s').", __func__, card_name);

    card_name_p = card_name;

    ret = audio_hal_alsa_open_controls(card_name_p);
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to open ALSA-card '%s'!\n", __func__, card_name_p);
        goto cleanup;
    }

    if (mic_type == MICROPHONE_TYPE_ANALOG) {
       audio_hal_alsa_set_control("Mic 1A Regulator", 0, mic_1a_regulator);
       audio_hal_alsa_set_control("Mic 1B Regulator", 0, mic_1b_regulator);
       audio_hal_alsa_set_control("Mic 2 Regulator", 0, mic_2_regulator);
    }

    ret = audio_hal_alsa_set_control("Master Clock Select", 0, 1); // Default -> ULPCLK
    if (ret < 0) {
        ALOG_ERR("%s: ERROR: Unable to set master clock select! (ret = %d)\n", __func__, ret);
        goto cleanup;
    }

    ret_func = STE_ADM_RES_OK;

cleanup:
    audio_hal_alsa_close_controls();
    return ret_func;
}
