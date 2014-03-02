/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_routing_u8500.c
    \brief Implementation of routing for x86.

    @todo Describe
*/

#define ADM_LOG_FILENAME "hwh_x86"

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <stdbool.h>

#include "ste_adm_db.h"
#include "ste_adm_dbg.h"
#include "ste_adm_hw_handler.h"
//#include "ste_adm_hw_handler_alsa.h"
#include "ste_adm_client.h"
#include "ste_adm_srv.h"

const char* platform_name_p = "ALC262 Analog";
const char* card_name_p = "NVidia";

static const char* codec_name_ab8500_p = "AB8500";
static const char* codec_name_cg92xx_p = "CG29XX";


static int is_asoc_device(const char* name)
{
    static const char *asoc_devs[] = {
        STE_ADM_DEVICE_STRING_MIC,
        STE_ADM_DEVICE_STRING_EARP,
        STE_ADM_DEVICE_STRING_SPEAKER,
        STE_ADM_DEVICE_STRING_HSIN,
        STE_ADM_DEVICE_STRING_HSOUT,
        STE_ADM_DEVICE_STRING_BTIN ,
        STE_ADM_DEVICE_STRING_BTOUT,
        STE_ADM_DEVICE_STRING_VIBL,
        STE_ADM_DEVICE_STRING_VIBR,
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



void init_db_string(char** name_string_p) {
    *name_string_p = malloc(512 * sizeof(char));
    strcpy(*name_string_p, "''");
}

void add_name_to_db_string(char* name_string, const char* name)
{
    strcat(name_string, ", '");
    strcat(name_string, name);
    strcat(name_string, "'");
}

ste_adm_res_t ste_adm_hw_handler_x86(sqlite3* db_p, hw_handler_dev_next_fp_t dev_next_fp, hw_handler_dev_to_dev_next_fp_t dev_next_d2d_fp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    const char* name = NULL;
    char* dev_names = NULL;
    char* d2d_names = NULL;
    int ret = 0;
    int ret_func = STE_ADM_RES_ALSA_ERROR;
    int rc = SQLITE_OK;
    sqlite3_stmt *stmt = NULL;
    char *command = NULL;
    const unsigned char* data = NULL;
    int i, is_input, nDev_combo;
    bool activeFMRx = false;
    bool activeFMTx = false;
    bool activeOutputDev = false;
    bool activeInputDev = false;

    ALOG_INFO("%s: Enter.", __func__);

    // Make sure that the settings that is written from ADM is not overwritten
    // by any sink or source
   // audio_hal_alsa_set_ignore_defaults(true);

    // Go through the device-list and create a device-string
    ret = dev_next_fp(&name);
    if (ret != 0) {
        ALOG_INFO("%s: No connected devices! Returning...", __func__);
        ret_func = STE_ADM_RES_OK;
        goto cleanup;
    }

    // Create SQL-statement
    command = malloc(1024 * sizeof(char));
    for (i = 0; i < 1024; i++)
        command[i] = (char)0;

    nDev_combo = 0;
    strcat(command, "SELECT * FROM HW_Settings WHERE (Platform = '");
    strcat(command, codec_name_ab8500_p);
    strcat(command, "')");
    do {
        if (strcmp(name, STE_ADM_DEVICE_STRING_FMRX) == 0) {
            activeFMRx = true;
        }
        else if (strcmp(name, STE_ADM_DEVICE_STRING_FMTX) == 0) {
            activeFMTx = true;
        } else if (is_asoc_device(name)) {
            if (adm_db_io_info(name, &is_input) != STE_ADM_RES_OK) {
                ALOG_INFO("%s: Warning: Device direction not found for device %s!", __func__, name);
            } else {
                if (is_input == 1)
                    activeInputDev = true;
                else
                    activeOutputDev = true;
            }
            nDev_combo++;
            strcat(command, " AND ('");
            strcat(command, name);
            strcat(command, "' IN (Dev1, Dev2, Dev3, Dev4, Dev5, Dev6))");
        }
    } while (dev_next_fp(&name) == 0);

    if (nDev_combo == 0) {
        strcat(command, " AND (Dev1 ='')");
    }

  /*  if (audio_hal_alsa_open_controls(card_name_p) < 0) {
        ALOG_ERR("%s: Unable to open ALSA-card '%s'!\n", __func__, card_name_p);
        goto cleanup;
    }
*/


    srv_become_worker_thread();

    ALOG_INFO("Query: %s", command);

    rc = sqlite3_prepare_v2(db_p, command, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("%s: Unable to prepare SQL-statement!", __func__);
        goto cleanup_worker;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        ALOG_ERR("%s: WARNING: No matching HW-settings found in db! Returning...", __func__);
        ret_func = STE_ADM_RES_OK;
        goto cleanup_worker;
    }

    ALOG_INFO("%s: Found matching HW-settings: devices = ('%s', '%s', '%s', '%s', '%s', '%s'), d2d-connections = ('%s', '%s', '%s', '%s').",
        __func__,
        sqlite3_column_text(stmt, 1),
        sqlite3_column_text(stmt, 2),
        sqlite3_column_text(stmt, 3),
        sqlite3_column_text(stmt, 4),
        sqlite3_column_text(stmt, 5),
        sqlite3_column_text(stmt, 6),
        sqlite3_column_text(stmt, 7),
        sqlite3_column_text(stmt, 8),
        sqlite3_column_text(stmt, 9),
        sqlite3_column_text(stmt, 10));

    data = sqlite3_column_text(stmt, 11);
    if (data == NULL) {
    ALOG_INFO("%s: Empty HW-settings. Returning...", __func__);
        ret_func = STE_ADM_RES_OK;
        goto cleanup_worker;
    }

    ALOG_INFO("%s: Write HW-settings from file to ALSA-interface.", __func__);
   /* ret = audio_hal_alsa_set_controls_cfg((const char*)data);
    if (ret < 0) {
        ALOG_ERR("%s: Failed to write HW-settings! ste_adm_hw_handler_alsa_set_controls returned %d.", __func__, ret);
        goto cleanup_worker;
    }*/

    ret = sqlite3_finalize(stmt);
    if (ret != SQLITE_OK)
        ALOG_WARN("%s: Failed to destroy prepared statement after use (sqlite3_finalize returned %d)!", __func__, ret);
    stmt = NULL;

cleanup_worker:
    srv_become_normal_thread();


    /* FM-devices */
    if (activeFMRx || activeFMTx) {
        ALOG_INFO("%s: FM-device active! Activating digital loopback...", __func__);
      //  audio_hal_alsa_set_control("Digital Interface AD 5 Loopback Switch", 0, 1);
        //audio_hal_alsa_set_control("Digital Interface AD 6 Loopback Switch", 0, 1);

        if (activeFMRx) {
            ALOG_INFO("%s: FMRx active! Setting DA-from-slot and AD-to-slot mapping...", __func__);
          //  audio_hal_alsa_set_control("Digital Interface AD To Slot 6 Map", 0, 4); // REG_ADSLOTSELX_AD_OUT5_TO_SLOT_EVEN
          //  audio_hal_alsa_set_control("Digital Interface AD To Slot 7 Map", 0, 5); // REG_ADSLOTSELX_AD_OUT6_TO_SLOT_ODD
          //  audio_hal_alsa_set_control("Digital Interface DA 7 From Slot Map", 0, 24); // Slot 24 -> DA_IN7
          //  audio_hal_alsa_set_control("Digital Interface DA 8 From Slot Map", 0, 25); // Slot 25 -> DA_IN8
        } else if (activeFMTx) {
            ALOG_INFO("%s: FMTx active! Setting DA-from-slot and AD-to-slot mapping...", __func__);
           // audio_hal_alsa_set_control("Digital Interface AD To Slot 16 Map", 0, 4); // REG_ADSLOTSELX_AD_OUT5_TO_SLOT_EVEN
           // audio_hal_alsa_set_control("Digital Interface AD To Slot 17 Map", 0, 5); // REG_ADSLOTSELX_AD_OUT6_TO_SLOT_ODD
           // audio_hal_alsa_set_control("Digital Interface DA 7 From Slot Map", 0, 14); // Slot 14 -> DA_IN7
           // audio_hal_alsa_set_control("Digital Interface DA 8 From Slot Map", 0, 15); // Slot 15 -> DA_IN8
        }
    }

    /* Input-devices */
    if (activeInputDev) {
        ALOG_INFO("%s: Input-device active! Setting AD to Slot mapping...", __func__);
     //   audio_hal_alsa_set_control("Digital Interface AD To Slot 0 Map", 0, 2); // REG_ADSLOTSELX_AD_OUT3_TO_SLOT_EVEN
     //   audio_hal_alsa_set_control("Digital Interface AD To Slot 1 Map", 0, 1); // REG_ADSLOTSELX_AD_OUT2_TO_SLOT_ODD
    }

    /* Output-devices */
    if (activeOutputDev) {
        ALOG_INFO("%s: Output-device active! Setting DA from Slot mapping...", __func__);
     //   audio_hal_alsa_set_control("Digital Interface DA 1 From Slot Map", 0, 8); // Slot 8 -> DA_IN1
     //   audio_hal_alsa_set_control("Digital Interface DA 2 From Slot Map", 0, 9); // Slot 9 -> DA_IN2
     //   audio_hal_alsa_set_control("Digital Interface DA 3 From Slot Map", 0, 10); // Slot 10 -> DA_IN3
    //    audio_hal_alsa_set_control("Digital Interface DA 4 From Slot Map", 0, 11); // Slot 11 -> DA_IN4
    //    audio_hal_alsa_set_control("Digital Interface DA 5 From Slot Map", 0, 12); // Slot 12 -> DA_IN5
    //    audio_hal_alsa_set_control("Digital Interface DA 6 From Slot Map", 0, 13); // Slot 13 -> DA_IN6
    }

    ret_func = STE_ADM_RES_OK;

cleanup:
    if (command != NULL) free(command);
    if (dev_names != NULL) free(dev_names);
    if (d2d_names != NULL) free(d2d_names);
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

 //   audio_hal_alsa_close_controls();

    ALOG_INFO("%s: Exit (%s).",
        __func__,
        (ret_func == STE_ADM_RES_OK) ? "OK" : "ERROR");

    return ret_func;
}

ste_adm_res_t ste_adm_hw_handler_vc_x86(const char* indev, const char* outdev)
{
    return STE_ADM_RES_OK;
}

ste_adm_res_t ste_adm_hw_handler_init_x86()
{
    return STE_ADM_RES_OK;
}
