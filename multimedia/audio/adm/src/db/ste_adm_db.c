/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_db.c
*   \brief Implements database connection functionality.
*
*   This file implements functions to connect and disconnect to/from the ADM
*   database.
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define ADM_LOG_FILENAME "db"

#include "ste_adm_db.h"
#include "ste_adm_db_cfg.h"
#include "ste_adm_dbg.h"
#include "ste_adm_client.h"
#include "ste_adm_config.h"
#if defined(ADM_DBG_X86)
#include "ste_adm_hw_handler.h"
#else
#include "alsactrl_hwh.h"
#endif


/**
*   ADM database file name
*/
#ifndef STE_ADM_DB_FILE_NAME
  #ifdef ANDROID
    #define STE_ADM_DB_SYSTEM_NAME "/system/etc/adm.sqlite"
    #define STE_ADM_DB_SYSTEM_NAME_AB8500 "/system/etc/adm.sqlite-u8500_ab8500"
    #define STE_ADM_DB_SYSTEM_NAME_AB8505_V1 "/system/etc/adm.sqlite-u8520_ab8505_v1"
    #define STE_ADM_DB_SYSTEM_NAME_AB8505_V2 "/system/etc/adm.sqlite-u8520_ab8505_v2"
    #define STE_ADM_DB_SYSTEM_NAME_AB8505_V3 "/system/etc/adm.sqlite-u8520_ab8505_v3"
    #define STE_ADM_DB_SYSTEM_NAME_AB9540_V1 "/system/etc/adm.sqlite-u9540_ab9540_v1"
    #define STE_ADM_DB_SYSTEM_NAME_AB9540_V2 "/system/etc/adm.sqlite-u9540_ab9540_v2"
    #define STE_ADM_DB_SYSTEM_NAME_AB9540_V3 "/system/etc/adm.sqlite-u9540_ab9540_v3"
    #define STE_ADM_DB_SYSTEM_NAME_AB8540_V1 "/system/etc/adm.sqlite-l8540_ab8540_v1"
    #define STE_ADM_DB_SYSTEM_NAME_AB8540_V2 "/system/etc/adm.sqlite-l8540_ab8540_v2"
    #define STE_ADM_DB_FILE_NAME "/data/adm.sqlite"
    #define STE_ADM_DB_TMP_FILE_NAME "/data/adm.sqlite_tmp"
  #else
    #define STE_ADM_DB_FILE_NAME "adm.sqlite"
  #endif
#endif


// Even though we just allow a single instance now, assist in keeping the
// client code ready for multiple iterator instances.
#define VALID_COMP_ITERATOR_MAGIC 0x12341122

/** @todo General: The SQLITE error codes needs to be converted somewhere to
                   general negative error codes. Negative error codes should be
                   used since that is the UNIX convention for error codes. */


//static ste_adm_res_t adm_db_comp_iter_create_handle_component(adm_db_comp_iter_t* iter);

// TODO: Naming convention for file scope variables?
static sqlite3* adm_db_handle = NULL;
static char* g_adm_db_system_name = NULL;
static int g_adm_db_tuningmode = 0;

// As of now, we only allow a single instance of each iterator. That way, we
// can use pre-parsed statements for all iterators. The interface is prepared
// for support for multiple instances should it be needed. We want to assert
// on accidental use of multiple instances, though.



static const char* stmt_speech_apf_get_text =
    "SELECT"
    " SpeechConfig.APFPreset"
    " FROM SpeechConfig"
    " WHERE SpeechConfig.InDev = ?1"
    " AND SpeechConfig.OutDev = ?2"
    " AND SpeechConfig.SampleRate = ?3";

static sqlite3_stmt* stmt_speech_apf_get = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// stmt_globalsettings
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_globalsettings
#define PID_GLOBAL_SETTINGS_NAME    1   // The global setting to lookup

// OUTPUT: column IDs for stmt_globalsettings
#define CID_GLOBAL_SETTINGS_DATA    0

static const char* stmt_globalsettings_text =
    "SELECT Data FROM GlobalSettings "
    " WHERE GlobalSettings.Name = ?1";

static sqlite3_stmt* stmt_globalsettings = NULL;


///////////////////////////////////////////////////////////////////////////////
//
// stmt_dev_dir
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_dev_dir
#define PID_DEV_DIR_NAME         1   // The device name to lookup

// OUTPUT: column IDs for stmt_dev_dir
#define CID_DEV_DIR_DIRECTION    0   // The device direction ("Input"/"Output")

static const char* stmt_dev_dir_text =
    "SELECT Direction FROM Device"
    " WHERE Device.Name = ?1";

static sqlite3_stmt* stmt_dev_dir = NULL;



///////////////////////////////////////////////////////////////////////////////
//
// stmt_effectchainname
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_effectchainname
#define PID_EFFECT_CHAIN_NAME_DEVICE_NAME         1   // The device name to lookup
// OUTPUT: column IDs for stmt_effectchainname
#define CID_EFFECT_CHAIN_NAME_APP_CHAIN_NAME      0
#define CID_EFFECT_CHAIN_NAME_COMMON_CHAIN_NAME   1
#define CID_EFFECT_CHAIN_NAME_DIRECTION           2

static const char* stmt_effectchainname_text =
            "SELECT Device.AppEffectChainName, Device.CommonEffectChainName,"
            " Device.Direction FROM Device"
            " WHERE Device.Name = ?1";

static sqlite3_stmt* stmt_effectchainname = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// stmt_ioname
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_ioname
#define PID_IO_NAME_DEVICE_NAME         1   // The device name to lookup
// OUTPUT: column IDs for stmt_ioname
#define CID_IO_NAME_IO_TEMPLATE_NAME    0
#define CID_IO_NAME_OMXNAME             1

static const char* stmt_ioname_text =
            "SELECT Device.IOTemplateName, Template_OMXName.OMXName"
            " FROM Device, Template_OMXName"
            " WHERE Device.Name = ?1"
            " AND Template_OMXName.Name = Device.IOTemplateName";
static sqlite3_stmt* stmt_ioname = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// stmt_pre_effect
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_pre_effect
#define PID_PRE_EFFECT_DEVICE_NAME          1   // The device name to lookup
// OUTPUT: column IDs for stmt_pre_effect
#define CID_PRE_EFFECT_EFFECT_NAME          0
#define CID_PRE_EFFECT_OMX_NAME             1

static const char* stmt_pre_effect_text =
            "SELECT Device.PreEffect, Template_OMXName.OMXName"
            " FROM Device, Template_OMXName"
            " WHERE Device.Name = ?1"
            " AND Template_OMXName.Name = Device.PreEffect";
static sqlite3_stmt* stmt_pre_effect = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// stmt_compiter
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_compiter
#define PID_COMP_ITER_CHAIN_NAME                  1

// OUTPUT: column IDs for stmt_compiter
#define CID_COMP_ITER_TEMPLATE_NAME                  0
#define CID_COMP_ITER_OMXNAME                1

static const char* stmt_compiter_text =
            "SELECT Template_OMXName.Name, Template_OMXName.OMXName"
            " FROM EffectChainDef, Template_OMXName"
            " WHERE Template_OMXName.Name=EffectChainDef.TemplateName"
            " AND EffectChainDef.Name = ?1"
            " ORDER BY EffectChainDef.Position";

static sqlite3_stmt* stmt_compiter              = NULL;
static int           stmt_compiter_busy         = 0;

///////////////////////////////////////////////////////////////////////////////
//
// stmt_speechiter
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_compiter
#define PID_SPEECH_NAME_IN_DEV_NAME             1
#define PID_SPEECH_NAME_OUT_DEV_NAME            2
#define PID_SPEECH_NAME_SAMPLE_RATE             3

// OUTPUT: column IDs for stmt_compiter
#define CID_SPEECH_NAME_IN_EFFECT_CHAIN_NAME    0
#define CID_SPEECH_NAME_OUT_EFFECT_CHAIN_NAME   1
#define CID_SPEECH_NAME_TEMPLATE_NAME           2

static const char* stmt_speechname_text =
            "SELECT"
            " SpeechConfig.InDevEffectChainName,"
            " SpeechConfig.OutDevEffectChainName,"
            " SpeechConfig.SpeechTemplateName"
            " FROM SpeechConfig"
            " WHERE SpeechConfig.InDev = ?1"
            " AND SpeechConfig.OutDev = ?2"
            " AND SpeechConfig.SampleRate = ?3";

static sqlite3_stmt* stmt_speechname              = NULL;


///////////////////////////////////////////////////////////////////////////////
//
// stmt_toplevel_mapping_set
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_compiter
#define PID_TOPLEVEL_MAPPING_SET_TOPLEVEL_NAME              1
#define PID_TOPLEVEL_MAPPING_SET_ACTUAL_NAME                2

static const char* stmt_toplevel_mapping_set_text =
            "UPDATE DeviceMap"
            " SET InternalName = ?2"
            " WHERE Name = ?1";

static sqlite3_stmt* stmt_toplevel_mapping_set = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// stmt_device_name_check
//
///////////////////////////////////////////////////////////////////////////////
#define PID_DEVICE_NAME_CHECK_DEV_NAME             1

static const char* stmt_device_name_check_text =
            "SELECT count(*) from Device where name = ?1";

static sqlite3_stmt* stmt_device_name_check = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// stmt_toplevel_mapping_count
//
///////////////////////////////////////////////////////////////////////////////

// OUTPUT: column IDs for stmt_toplevel_mapping_count
#define CID_TOPLEVEL_MAPPING_COUNT            0

static const char* stmt_toplevel_mapping_count_text =
            "SELECT count(*) from DeviceMap";

static sqlite3_stmt* stmt_toplevel_mapping_count = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// stmt_toplevel_mapping_get_all
//
///////////////////////////////////////////////////////////////////////////////

// OUTPUT: column IDs for stmt_toplevel_mapping_get_all
#define CID_TOPLEVEL_MAPPING_GET_TOPLEVEL_NAME            0
#define CID_TOPLEVEL_MAPPING_GET_INTERNAL_NAME            1
static const char* stmt_toplevel_mapping_get_all_text =
            "SELECT Name, InternalName from DeviceMap";

static sqlite3_stmt* stmt_toplevel_mapping_get_all = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// stmt_external_delay
//
///////////////////////////////////////////////////////////////////////////////

// OUTPUT: column IDs for stmt_external_delay_get
#define CID_EXT_DELAY_LIST_GET_DEVICE_NAME  0
#define CID_EXT_DELAY_LIST_GET_DELAY        1
#define CID_EXT_DELAY_LIST_COUNT            0

static const char* stmt_ext_delay_list_count_text =
            "SELECT count(*) from Device";

static sqlite3_stmt* stmt_ext_delay_list_count = NULL;

static const char* stmt_ext_delay_list_get_text =
    "SELECT Name, ExternalDelay FROM Device";

static sqlite3_stmt* stmt_ext_delay_list_get = NULL;


///////////////////////////////////////////////////////////////////////////////
//
// stmt_foreignkeys_text
//
///////////////////////////////////////////////////////////////////////////////

static const char* stmt_foreignkeys_text =
            "PRAGMA foreign_keys = ON";

static sqlite3_stmt* stmt_foreignkeys              = NULL;

// TODO: void and assert instead? Module-internal error otherwise
int adm_db_get_handle(sqlite3** db_hp)
{
    ADM_ASSERT(db_hp);
    if (adm_db_handle == NULL)
        return -1;

    *db_hp = adm_db_handle;
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// devicemap RAM cache
//
///////////////////////////////////////////////////////////////////////////////


typedef struct
{
    char toplevel_name[ADM_MAX_DEVICE_NAME_LENGTH];
    char internal_name[ADM_MAX_DEVICE_NAME_LENGTH];
} device_map_entry_t;

static device_map_entry_t *toplevel_map = NULL;
static int toplevel_map_size = 0;


///////////////////////////////////////////////////////////////////////////////
//
// external delay RAM cache
//
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
    char device_name[ADM_MAX_DEVICE_NAME_LENGTH];
    int delay;
} ext_delay_list_entry_t;

static ext_delay_list_entry_t *ext_delay_list = NULL;
static int ext_delay_list_size = 0;


static int adm_db_init_toplevel_map() {
    int rc;
    ste_adm_res_t res = STE_ADM_RES_OK;

    if (toplevel_map != NULL) {
        free(toplevel_map);
        toplevel_map_size = 0;
    }

    // Get the size of devicemap

    rc = sqlite3_prepare_v2(adm_db_handle, stmt_toplevel_mapping_count_text,
            -1, &stmt_toplevel_mapping_count, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("adm_db_init_toplevel_map: sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    rc = sqlite3_step(stmt_toplevel_mapping_count);

    if (rc != SQLITE_ROW) {
        ALOG_ERR("adm_db_init_toplevel_map: sqlite3_step failed with error %s\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    toplevel_map_size = sqlite3_column_int(stmt_toplevel_mapping_count, CID_TOPLEVEL_MAPPING_COUNT);


    // Allocate the data structure

    toplevel_map = malloc (toplevel_map_size * sizeof(device_map_entry_t));
    if (toplevel_map == NULL) {
        ALOG_ERR("adm_db_init_toplevel_map: failed to allocate memory");
        goto cleanup;
    }


    // Read the default data

    rc = sqlite3_prepare_v2(adm_db_handle, stmt_toplevel_mapping_get_all_text,
            -1, &stmt_toplevel_mapping_get_all, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    rc = sqlite3_step(stmt_toplevel_mapping_get_all);

    int i = 0;
    while( (rc=sqlite3_step(stmt_toplevel_mapping_get_all)) == SQLITE_ROW && i < toplevel_map_size) {

        const char *tmp = (const char*) sqlite3_column_text(stmt_toplevel_mapping_get_all, CID_TOPLEVEL_MAPPING_GET_TOPLEVEL_NAME);
        strncpy(toplevel_map[i].toplevel_name, tmp, ADM_MAX_DEVICE_NAME_LENGTH);

        tmp = (const char*) sqlite3_column_text(stmt_toplevel_mapping_get_all, CID_TOPLEVEL_MAPPING_GET_INTERNAL_NAME);
        strncpy(toplevel_map[i].internal_name, tmp, ADM_MAX_DEVICE_NAME_LENGTH);

        ALOG_INFO("adm_db_init_toplevel_map: Adding %s = %s", toplevel_map[i].toplevel_name, toplevel_map[i].internal_name);
        i++;
    }

    if (rc != SQLITE_DONE) {
        ALOG_ERR("adm_db_init_toplevel_map: sqlite3_step failed with error %d", rc);
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }


    goto done;

cleanup:
    if (toplevel_map != NULL) {
        free(toplevel_map);
        toplevel_map_size = 0;
    }

done:

    rc = sqlite3_finalize(stmt_toplevel_mapping_count);
    if (rc != SQLITE_OK) {
        ALOG_ERR("adm_db_init_toplevel_map: sqlite3_finalize failed with error %s\n",
            sqlite3_errmsg(adm_db_handle));
    }

    rc = sqlite3_finalize(stmt_toplevel_mapping_get_all);
    if (rc != SQLITE_OK) {
        ALOG_ERR("adm_db_init_toplevel_map: sqlite3_finalize failed with error %s\n",
            sqlite3_errmsg(adm_db_handle));
    }


    return res;
}


static int adm_db_init_ext_delay_list() {
    int rc;
    ste_adm_res_t res = STE_ADM_RES_OK;

    if (ext_delay_list != NULL) {
        free(ext_delay_list);
        ext_delay_list_size = 0;
    }

    // Get the size of device table

    rc = sqlite3_prepare_v2(adm_db_handle, stmt_ext_delay_list_count_text,
            -1, &stmt_ext_delay_list_count, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("adm_db_init_ext_delay_list: sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    rc = sqlite3_step(stmt_ext_delay_list_count);

    if (rc != SQLITE_ROW) {
        ALOG_ERR("adm_db_init_ext_delay_list: sqlite3_step failed with error %s\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    ext_delay_list_size = sqlite3_column_int(stmt_ext_delay_list_count, CID_EXT_DELAY_LIST_COUNT);


    // Allocate the data structure

    ext_delay_list = malloc (ext_delay_list_size * sizeof(ext_delay_list_entry_t));
    if (ext_delay_list == NULL) {
        ALOG_ERR("adm_db_init_ext_delay_list: failed to allocate memory");
        goto cleanup;
    }


    // Read the default data

    rc = sqlite3_prepare_v2(adm_db_handle, stmt_ext_delay_list_get_text,
            -1, &stmt_ext_delay_list_get, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("adm_db_init_ext_delay_list: sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    rc = sqlite3_step(stmt_ext_delay_list_get);

    int i = 0;
    while( (rc=sqlite3_step(stmt_ext_delay_list_get)) == SQLITE_ROW && i < ext_delay_list_size) {

        const char *tmp = (const char*) sqlite3_column_text(stmt_ext_delay_list_get, CID_EXT_DELAY_LIST_GET_DEVICE_NAME);
        strncpy(ext_delay_list[i].device_name, tmp, ADM_MAX_DEVICE_NAME_LENGTH);

        ext_delay_list[i].delay = sqlite3_column_int(stmt_ext_delay_list_get, CID_EXT_DELAY_LIST_GET_DELAY);

        ALOG_INFO("adm_db_init_ext_delay_list: Adding %s, delay = %s", ext_delay_list[i].device_name, ext_delay_list[i].delay);
        i++;
    }

    if (rc != SQLITE_DONE) {
        ALOG_ERR("adm_db_init_ext_delay_list: sqlite3_step failed with error %d", rc);
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }


    goto done;

cleanup:
    if (ext_delay_list != NULL) {
        free(ext_delay_list);
        ext_delay_list_size = 0;
    }

done:

    rc = sqlite3_finalize(stmt_ext_delay_list_count);
    if (rc != SQLITE_OK) {
        ALOG_ERR("adm_db_init_ext_delay_list: sqlite3_finalize failed with error %s\n",
            sqlite3_errmsg(adm_db_handle));
    }

    rc = sqlite3_finalize(stmt_ext_delay_list_get);
    if (rc != SQLITE_OK) {
        ALOG_ERR("adm_db_init_ext_delay_list: sqlite3_finalize failed with error %s\n",
            sqlite3_errmsg(adm_db_handle));
    }


    return res;
}


static int adm_db_connect()
{
    int rc;
    ADM_ASSERT(adm_db_handle == NULL);

    rc = sqlite3_open_v2(STE_ADM_DB_FILE_NAME, &adm_db_handle,
        SQLITE_OPEN_READWRITE, NULL);

    if(rc != SQLITE_OK) {
        // sqlite3_close(NULL) is a valid call
        sqlite3_close(adm_db_handle);
        adm_db_handle = NULL;

#ifdef ANDROID
        /* The db is installed in /system/etc and needs to be copied to /data
         * the first time, and after a reset if /data is cleaned.
         * The copy is made to a temporary file which is renamed if the copy
         * operation is successful. This is done to prevent incomplete copy. */
        ALOG_STATUS("Copying ADM db from %s\n", g_adm_db_system_name);

        FILE *adm_db_system;
        FILE *adm_db_data;
        int ch;
        adm_db_system = fopen(g_adm_db_system_name, "rb");
        if (adm_db_system == NULL) {
            ALOG_ERR("Failed to open file %s\n", g_adm_db_system_name);
            return -1;
        }
        adm_db_data = fopen(STE_ADM_DB_TMP_FILE_NAME, "wb");
        if (adm_db_data == NULL) {
            ALOG_ERR("Failed to open file %s\n", STE_ADM_DB_TMP_FILE_NAME);
            fclose(adm_db_system);
            return -1;
        }

        while (!feof(adm_db_system)) {
            ch = fgetc(adm_db_system);
            if (ferror(adm_db_system)) {
                ALOG_ERR("Failed to read from %s\n", g_adm_db_system_name);
                fclose(adm_db_system);
                fclose(adm_db_data);
                return -1;
            }
            if (!feof(adm_db_system)) {
                fputc(ch, adm_db_data);
                if (ferror(adm_db_data)) {
                    ALOG_ERR("Failed to write to %s\n", STE_ADM_DB_TMP_FILE_NAME);
                    fclose(adm_db_system);
                    fclose(adm_db_data);
                    return -1;
                }
            }
        }
        fclose(adm_db_system);
        fclose(adm_db_data);

        if (rename(STE_ADM_DB_TMP_FILE_NAME, STE_ADM_DB_FILE_NAME) != 0) {
            ALOG_ERR("Failed to rename ADM db from %s to %s\n",
                STE_ADM_DB_TMP_FILE_NAME, STE_ADM_DB_FILE_NAME);
            return -1;
        }

        /* Try to open the db again after coying from /system/etc to /data */
        rc = sqlite3_open_v2(STE_ADM_DB_FILE_NAME, &adm_db_handle,
            SQLITE_OPEN_READWRITE, NULL);

        if(rc != SQLITE_OK) {
            // sqlite3_errmsg(NULL) is a valid call
            const char *errmsg = sqlite3_errmsg(adm_db_handle);
            sqlite3_close(adm_db_handle);
            adm_db_handle = NULL;
            ALOG_ERR("Failed to open database %s: %s\n",
                STE_ADM_DB_FILE_NAME, errmsg);
            return -1;
        }
#else
        // sqlite3_errmsg(NULL) is a valid call
        const char *errmsg = sqlite3_errmsg(adm_db_handle);
        ALOG_ERR("Failed to open database %s: %s\n",
            STE_ADM_DB_FILE_NAME, errmsg);
        return -1;
#endif
    }

    /* Enable foreign key checking */
    ADM_ASSERT(stmt_foreignkeys == NULL);
    rc = sqlite3_prepare_v2(adm_db_handle, stmt_foreignkeys_text,
            -1, &stmt_foreignkeys, NULL);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_prepare_v2 failed with error %s", sqlite3_errmsg(adm_db_handle));
    }

    rc = sqlite3_step(stmt_foreignkeys);
    if (rc != SQLITE_DONE) {
        ALOG_ERR("sqlite3_step failed with error %s", sqlite3_errmsg(adm_db_handle));
    }

    rc = sqlite3_finalize(stmt_foreignkeys);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s", sqlite3_errmsg(adm_db_handle));
    }

    stmt_foreignkeys = NULL;

    return 0;
}

ste_adm_res_t adm_db_disconnect()
{
    ste_adm_res_t res = STE_ADM_RES_OK;

    adm_db_remap_destroy();

    if (adm_db_cfg_close() != STE_ADM_RES_OK)
    {
          ALOG_ERR("sqlite3_finalize failed due to error from adm_db_cfg_close\n");
        res = STE_ADM_RES_DB_FATAL;
    }

    // sqlite3_finalize(NULL) is a valid call
    int rc = sqlite3_finalize(stmt_globalsettings);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_disconnect\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_finalize(stmt_dev_dir);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_disconnect\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_finalize(stmt_effectchainname);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_disconnect\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_finalize(stmt_speechname);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_disconnect\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_finalize(stmt_ioname);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_disconnect\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_finalize(stmt_pre_effect);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_disconnect\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_finalize(stmt_toplevel_mapping_set);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_disconnect\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_finalize(stmt_device_name_check);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_disconnect\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
    }

    // If stmt_compiter exists, assert it is not busy
    ADM_ASSERT(stmt_compiter == NULL || !stmt_compiter_busy);
    rc = sqlite3_finalize(stmt_compiter);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_disconnect\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
    }


    rc = sqlite3_close(adm_db_handle);
    if(rc != SQLITE_OK) {
        // sqlite3_errmsg(NULL) is a valid call
        const char *errmsg = sqlite3_errmsg(adm_db_handle);
        ALOG_ERR("Failed to close database %s: %s\n",
                STE_ADM_DB_FILE_NAME, errmsg);
        res = STE_ADM_RES_DB_FATAL;
    }


    stmt_globalsettings = 0;
    stmt_dev_dir = 0;
    stmt_device_name_check = 0;
    stmt_effectchainname = 0;
    stmt_compiter = 0;
    stmt_speechname = 0;
    stmt_ioname = 0;
    stmt_pre_effect = 0;
    stmt_toplevel_mapping_set = 0;


    ALOG_INFO("DB closed, res = %d\n", rc);
    adm_db_handle = NULL;
    return res;
}


static ste_adm_res_t adm_db_init_statements()
{
    int rc;

    ADM_ASSERT(adm_db_handle != NULL);

    /* Prepare the stmt_globalsettings statement */
    if (stmt_globalsettings == NULL){
        rc = sqlite3_prepare_v2(adm_db_handle, stmt_globalsettings_text,
            -1, &stmt_globalsettings, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    // Prepare the stmt_dev_dir statement
    if (stmt_dev_dir == NULL)
    {
        rc = sqlite3_prepare_v2(adm_db_handle, stmt_dev_dir_text,
            -1, &stmt_dev_dir, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    // Prepare the stmt_effectchainname statement
    if (stmt_effectchainname == NULL)
    {
        rc = sqlite3_prepare_v2(adm_db_handle, stmt_effectchainname_text,
                -1, &stmt_effectchainname, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    // Prepare the stmt_compiter statement
    if (stmt_compiter == NULL)
    {
        rc = sqlite3_prepare_v2(adm_db_handle, stmt_compiter_text,
                -1, &stmt_compiter, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    ADM_ASSERT(!stmt_compiter_busy);

    // Prepare the stmt_speechname statement
    if (stmt_speechname == NULL)
    {
        rc = sqlite3_prepare_v2(adm_db_handle, stmt_speechname_text,
                -1, &stmt_speechname, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    // Prepare the stmt_device_name_check statement
    if (stmt_device_name_check == NULL)
    {
        rc = sqlite3_prepare_v2(adm_db_handle, stmt_device_name_check_text,
                -1, &stmt_device_name_check, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    // Prepare the stmt_ioname statement
    if (stmt_ioname == NULL)
    {
        rc = sqlite3_prepare_v2(adm_db_handle, stmt_ioname_text,
                -1, &stmt_ioname, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    // Prepare the stmt_pre_effect statement
    if (stmt_pre_effect == NULL)
    {
        rc = sqlite3_prepare_v2(adm_db_handle, stmt_pre_effect_text,
                -1, &stmt_pre_effect, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    // Prepare the stmt_toplevel_mapping_set statement
    if (stmt_toplevel_mapping_set == NULL)
    {
        rc = sqlite3_prepare_v2(adm_db_handle, stmt_toplevel_mapping_set_text,
                -1, &stmt_toplevel_mapping_set, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }
    return STE_ADM_RES_OK;
}
#ifdef ANDROID
static char* adm_db_get_system_name()
{
    enum audio_hal_chip_id_t chip_id;
    // Get chip id
    chip_id = Alsactrl_Hwh_SelectHW();
    if (chip_id == CHIP_ID_UNKNOWN){
        ALOG_ERR("Alsactrl_Hwh_SelectHW() failed, CHIP_ID_UNKNOWN!\n");
    }

#ifdef ADM_DB_PROD_SPEC
    return STE_ADM_DB_SYSTEM_NAME;
#else
    switch (chip_id) {
    case CHIP_ID_AB8500: return STE_ADM_DB_SYSTEM_NAME_AB8500;
    case CHIP_ID_AB9540_V1: return STE_ADM_DB_SYSTEM_NAME_AB9540_V1;
    case CHIP_ID_AB9540_V2: return STE_ADM_DB_SYSTEM_NAME_AB9540_V2;
    case CHIP_ID_AB9540_V3: return STE_ADM_DB_SYSTEM_NAME_AB9540_V3;
    case CHIP_ID_AB8505_V1: return STE_ADM_DB_SYSTEM_NAME_AB8505_V1;
    case CHIP_ID_AB8505_V2: return STE_ADM_DB_SYSTEM_NAME_AB8505_V2;
    case CHIP_ID_AB8505_V3: return STE_ADM_DB_SYSTEM_NAME_AB8505_V2;
	case CHIP_ID_AB8540_V1: return STE_ADM_DB_SYSTEM_NAME_AB8540_V1;
	case CHIP_ID_AB8540_V2: return STE_ADM_DB_SYSTEM_NAME_AB8540_V2;
    default: return STE_ADM_DB_SYSTEM_NAME;
    }
#endif
}
#else
static char* adm_db_get_system_name()
{
    return STE_ADM_DB_FILE_NAME;
}
#endif //ANDROID

ste_adm_res_t adm_db_init()
{
#define MAX_TRY_DB_COPY    5    // In case of error during DB initialization, allow to make 5 consecutive DB copy

    ste_adm_res_t  result;
    int error_cnt = 0;

    // Problems with file-scope variables due to dynamic libs?
    ADM_ASSERT(adm_db_handle == 0);
    ADM_ASSERT(stmt_speech_apf_get == 0);
    ADM_ASSERT(stmt_globalsettings == 0);
    ADM_ASSERT(stmt_dev_dir == 0);
    ADM_ASSERT(stmt_device_name_check == 0);
    ADM_ASSERT(stmt_effectchainname == 0);
    ADM_ASSERT(stmt_compiter == 0);
    ADM_ASSERT(stmt_compiter_busy == 0);
    ADM_ASSERT(stmt_speechname == 0);
    ADM_ASSERT(stmt_ioname == 0);
    ADM_ASSERT(stmt_pre_effect == 0);
    ADM_ASSERT(stmt_toplevel_mapping_set == 0);
retry:
    result = STE_ADM_RES_OK;

    // assert() is disabled on target by default (NDEBUG is set)
    adm_db_handle = 0;
    stmt_speech_apf_get = 0;
    stmt_globalsettings = 0;
    stmt_dev_dir = 0;
    stmt_device_name_check = 0;
    stmt_effectchainname = 0;
    stmt_compiter = 0;
    stmt_compiter_busy = 0;
    stmt_speechname = 0;
    stmt_ioname = 0;
    stmt_pre_effect = 0;
    stmt_toplevel_mapping_set = 0;

    g_adm_db_system_name = adm_db_get_system_name();

    if (adm_db_connect() < 0)
    {
      ALOG_ERR("adm_db_connect failed in adm_db_init\n");
      result=STE_ADM_RES_DB_FATAL;
      goto error;
    }
    ADM_ASSERT(adm_db_handle != NULL);

    if (adm_db_cfg_init() != STE_ADM_RES_OK)
    {
      ALOG_ERR("adm_db_cfg_init failed in adm_db_init\n");
      result=STE_ADM_RES_DB_FATAL;
      goto error;
    }

    if (adm_db_init_statements() != STE_ADM_RES_OK)
    {
        ALOG_ERR("adm_db_init_statements failed in adm_db_init\n");
        result=STE_ADM_RES_DB_FATAL;
        goto error;
    }

    if (adm_db_init_toplevel_map() != STE_ADM_RES_OK)
    {
        ALOG_ERR("adm_db_init_toplevel_map failed in adm_db_init\n");
        goto error;
    }

    if (adm_db_init_ext_delay_list() != STE_ADM_RES_OK)
    {
        ALOG_ERR("adm_db_init_ext_delay_list failed in adm_db_init\n");
        goto error;
    }

    if (adm_db_remap_init() != STE_ADM_RES_OK)
    {
        ALOG_ERR("adm_db_remap_init failed in adm_db_init\n");
        result=STE_ADM_RES_DB_FATAL;
        goto error;
    }

#ifdef ADM_DBG_X86 // TODO: Remove this when ADM is more or less finished
    // TODO: We run some tests on the database here on startup...
    extern void dbg_db_test();
    dbg_db_test();
#endif

    return result;

error:
    error_cnt++;

    if(error_cnt >= MAX_TRY_DB_COPY){
        ALOG_ERR("Maximum DB copy reached(%i) in adm_db_init()",MAX_TRY_DB_COPY);
        return result;
    }

    ALOG_ERR("Error during DB initialization -> force copy");

    // sqlite3_close(NULL) is a valid call
    sqlite3_close(adm_db_handle);
    remove(STE_ADM_DB_FILE_NAME);

   // Avoid 100% loading
   sleep(1);
   goto retry;

}



static void adm_db_reinitialize_statement(sqlite3_stmt** statement)
{
    sqlite3_finalize(*statement);
    *statement = NULL;
    if (adm_db_init_statements() != STE_ADM_RES_OK)
    {
        ALOG_ERR("Failed to reinitialize statement\n");
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_extname
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_extname(char* dev)
{
    ADM_ASSERT(dev);
    ADM_ASSERT(toplevel_map);

    int i;

    for (i = 0; i < toplevel_map_size; i++) {
        if (strncmp(dev, toplevel_map[i].toplevel_name, ADM_MAX_DEVICE_NAME_LENGTH) == 0) {
            return STE_ADM_RES_OK;
        }

        if (strncmp(dev, toplevel_map[i].internal_name, ADM_MAX_DEVICE_NAME_LENGTH) == 0) {
            strncpy(dev, toplevel_map[i].toplevel_name, ADM_MAX_DEVICE_NAME_LENGTH);
            return STE_ADM_RES_OK;
        }
    }

    return STE_ADM_RES_NO_SUCH_DEVICE;
}




///////////////////////////////////////////////////////////////////////////////
//
// adm_db_io_info
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_io_info(const char* dev,
                             int* is_input)
{
    ADM_ASSERT(is_input);
    ADM_ASSERT(dev);
    ADM_ASSERT(adm_db_handle);
    ADM_ASSERT(stmt_dev_dir);

    const char *actual_dev = NULL;

    if (adm_db_toplevel_mapping_get(dev, &actual_dev) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }

    if (sqlite3_reset(stmt_dev_dir) != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset failed with error %s in io_info\n", sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    int rc = sqlite3_bind_text(stmt_dev_dir, PID_DEV_DIR_NAME, actual_dev, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s in io_info\n", sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_step(stmt_dev_dir);
    if (rc == SQLITE_DONE) {
        ALOG_WARN("io_info - device '%s' not found in db\n", actual_dev);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }
    if (rc != SQLITE_ROW) {
        ALOG_ERR("sqlite3_step failed with error %s in io_info\n", sqlite3_errmsg(adm_db_handle));
        adm_db_reinitialize_statement(&stmt_dev_dir);
        return STE_ADM_RES_DB_FATAL;
    }

    const char* dir = (const char*) sqlite3_column_text(stmt_dev_dir,
                                        CID_DEV_DIR_DIRECTION);
    if (!dir) {
        ALOG_ERR("get io info: sqlite3_column_text returned NULL\n");
        // out of memory in Linux == address space exhausted ==> fatal
        return STE_ADM_RES_DB_FATAL;
    }
    if (strcmp(dir, "IN") == 0)
        *is_input = 1;
    else if (strcmp(dir, "OUT") == 0)
        *is_input = 0;
    else {
        ALOG_ERR("io_info - inconsistent db entry: dev='%s', dir='%s'\n",dev,dir);
        return STE_ADM_RES_DB_INCONSISTENT;
    }

    return STE_ADM_RES_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_comp_iter_create
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_comp_iter_create(const char* dev, const char* dev_2nd,
                                    ste_adm_effect_chain_type_t type,
                                    int samplerate,
                                    adm_db_comp_iter_t* iter)
{
    sqlite3_stmt* stmt = NULL;
    int rc = SQLITE_OK;
    int cid_chain_name = 0;
    const char* dev_in = NULL;
    const char* dev_out = NULL;

    ADM_ASSERT(adm_db_handle);
    ADM_ASSERT(dev);
    ADM_ASSERT( type == STE_ADM_APPLICATION_CHAIN ||
            type == STE_ADM_VOICE_CHAIN ||
            type == STE_ADM_COMMON_CHAIN);
    ADM_ASSERT(iter);

    ALOG_DB("Enter adm_db_comp_iter_create - dev=%s, dev_2nd=%s, "
            "chain=%d, samplerate=%d", dev, dev_2nd, type, samplerate);

    /* For now, we only allow a single instance of each iterator.
       Not documented, so not an assert */
    if (stmt_compiter_busy) {
        ALOG_ERR("comp_iter_create - currently only one iterator instance is "
            "supported\n");
        return STE_ADM_RES_DB_FATAL;
    }

    const char *actual_dev = NULL;
    const char *actual_dev2 = NULL;

    if (adm_db_toplevel_mapping_get(dev, &actual_dev) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }

    switch (type) {

    case STE_ADM_VOICE_CHAIN:

        if (adm_db_toplevel_mapping_get(dev_2nd, &actual_dev2) != STE_ADM_RES_OK) {
            return STE_ADM_RES_DB_FATAL;
        }

        stmt = stmt_speechname;

        int is_input;
        if (adm_db_io_info(dev, &is_input) != STE_ADM_RES_OK) {
            ALOG_ERR("Failed to get direction of %s from db!\n", dev);
            return STE_ADM_RES_NO_SUCH_DEVICE;
        }
        if (is_input) {
            dev_in  = actual_dev;
            dev_out = actual_dev2;
            cid_chain_name = CID_SPEECH_NAME_IN_EFFECT_CHAIN_NAME;
        } else {
            dev_in  = actual_dev2;
            dev_out = actual_dev;
            cid_chain_name = CID_SPEECH_NAME_OUT_EFFECT_CHAIN_NAME;
        }

        /* Reset the statement we want to use */
        rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_reset() failed with error %s in comp_iter_create\n",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }

        /* Specify the device names and samplerate to filer on */
        rc = sqlite3_bind_text(stmt, PID_SPEECH_NAME_IN_DEV_NAME, dev_in, -1,
            SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_bind_text failed with error %s",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }

        rc = sqlite3_bind_text(stmt, PID_SPEECH_NAME_OUT_DEV_NAME, dev_out, -1,
            SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_bind_text failed with error %s",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }

        rc = sqlite3_bind_int(stmt, PID_SPEECH_NAME_SAMPLE_RATE,
            (int)samplerate);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_bind_int failed with error %s",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
        break;

    case STE_ADM_APPLICATION_CHAIN:
    case STE_ADM_COMMON_CHAIN:
    default:

        stmt = stmt_effectchainname;

        if (type == STE_ADM_APPLICATION_CHAIN) {
            cid_chain_name  = CID_EFFECT_CHAIN_NAME_APP_CHAIN_NAME;
        } else {
            cid_chain_name = CID_EFFECT_CHAIN_NAME_COMMON_CHAIN_NAME;
        }

        /* Reset the statement we want to use */
        rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_reset() failed with error %s in comp_iter_create",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }

        /* Specify the device name to filter on */
        rc = sqlite3_bind_text(stmt, PID_EFFECT_CHAIN_NAME_DEVICE_NAME, actual_dev, -1,
            SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_bind_text failed with error %s in "
                "comp_iter_create\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
        break;
    }

    /* Get a single matching row */
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        if (type == STE_ADM_VOICE_CHAIN) {
            ALOG_ERR("comp_iter_create - device '%s' & '%s' for samplerate=%u "
                "not found in db\n", dev_in, dev_out, samplerate);
        } else {
            ALOG_ERR("comp_iter_create - device '%s' not found in db\n", actual_dev);
        }
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }
    if (rc != SQLITE_ROW) {
        ALOG_ERR("sqlite3_step failed with error %s in comp_iter_create\n",
            sqlite3_errmsg(adm_db_handle));
        adm_db_reinitialize_statement(&stmt);
        return STE_ADM_RES_DB_FATAL;
    }

    /* Get the chain name */
    const char* chain_name =
        (const char*)sqlite3_column_text(stmt, cid_chain_name);
    if (!chain_name) {
        ALOG_ERR("sqlite3_column_text returned NULL\n");
        return STE_ADM_RES_DB_FATAL;
    }
    ALOG_DB("Effect chain name = %s\n", chain_name);

    /* Setup the component iterator */
    rc = sqlite3_reset(stmt_compiter);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset(stmt_compiter) failed with error %s in "
            "comp_iter_create\n", sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }
    /* Add the filter on chain name to the query that produces all effects in
       the current chain. We use SQLITE_TRANSIENT instead of SQLITE_STATIC so
       that we can keep running the component query even if the
       adm_db_comp_iter_create call is repeated. Not really needed since we
       currently block at one instance, but it is difficult to ADM_ASSERT() that
       chain_name stays valid so let's be safe. */
    rc = sqlite3_bind_text(stmt_compiter, PID_COMP_ITER_CHAIN_NAME,
                chain_name, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s in comp_iter_create\n",
            sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    stmt_compiter_busy = 1;
    iter->end_reached = 0;
    iter->magic = VALID_COMP_ITERATOR_MAGIC;
    return STE_ADM_RES_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_comp_iter_get_next
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_comp_iter_get_next(adm_db_comp_iter_t* iter,
                                  const char**      il_name,
                                  adm_db_cfg_iter_t*  cfg_iter)
{
    ADM_ASSERT(iter);
    ADM_ASSERT(il_name);
    ADM_ASSERT(cfg_iter);
    ADM_ASSERT(iter->magic == VALID_COMP_ITERATOR_MAGIC);
    ADM_ASSERT(stmt_compiter_busy);

    ALOG_DB("Enter adm_db_comp_iter_get_next");

    // For now, we only allow a single instance of each iterator.
    // Not documented, so not an assert
    if (adm_db_cfg_is_busy()) {
        ALOG_ERR("comp_iter_get_next - currently only one iterator instance is supported\n");
        return STE_ADM_RES_DB_FATAL;
    }

    int rc = SQLITE_DONE;
    if (!iter->end_reached)
        rc = sqlite3_step(stmt_compiter);

    if (rc == SQLITE_DONE) {
        // sqlite3_step must only return SQLITE_DONE at most once
        iter->end_reached = 1;
        ALOG_DB("\t No more matching entries");
        return STE_ADM_RES_NO_MORE;
    }
    if (rc != SQLITE_ROW) {
        ALOG_ERR("sqlite3_step failed with '%s' (%u) in comp_iter_get_next\n", sqlite3_errmsg(adm_db_handle), rc);
        adm_db_reinitialize_statement(&stmt_compiter);
        return STE_ADM_RES_DB_FATAL;
    }

    *il_name = (const char *) sqlite3_column_text(
                stmt_compiter, CID_COMP_ITER_OMXNAME);
    if (*il_name == NULL) { // out of memory in Linux == exhausted address space ==> fatal
        ALOG_ERR("memory allocation failed in comp_iter_get_next\n");
        return STE_ADM_RES_DB_FATAL;
    }
    ALOG_DB("\t il_name = '%s'", *il_name);

    const char* comp_name = (const char *) sqlite3_column_text(
            stmt_compiter, CID_COMP_ITER_TEMPLATE_NAME); // component name
    if (!comp_name) { // out of memory in Linux == exhausted address space ==> fatal
        ALOG_ERR("memory allocation failed in comp_iter_get_next\n");
        return STE_ADM_RES_DB_FATAL;
    }
    ALOG_DB("\t comp_name = '%s'", comp_name);

    return adm_db_cfg_create_iter(cfg_iter, comp_name);
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_speech_cfg_iter_create
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_speech_cfg_iter_create(const char* dev_in,
                                        const char* dev_out,
                                        int samplerate,
                                        adm_db_cfg_iter_t* iter)
{
    const char* comp_name;
    int rc;

    ALOG_DB("Enter adm_db_speech_cfg_iter_create - "
        "dev_in=%s, dev_out=%s, samplerate=%d", dev_in, dev_out, samplerate);

    const char *actual_dev_in = NULL;
    const char *actual_dev_out = NULL;

    if (adm_db_toplevel_mapping_get(dev_in, &actual_dev_in) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }

    if (adm_db_toplevel_mapping_get(dev_out, &actual_dev_out) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }

    if (sqlite3_reset(stmt_speechname) != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_bind_text(stmt_speechname,
        PID_SPEECH_NAME_IN_DEV_NAME, actual_dev_in, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_bind_text(stmt_speechname,
        PID_SPEECH_NAME_OUT_DEV_NAME, actual_dev_out, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_bind_int(stmt_speechname,
        PID_SPEECH_NAME_SAMPLE_RATE, samplerate);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_int failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_step(stmt_speechname);
    if (rc == SQLITE_DONE) {
        ALOG_WARN("device '%s' & '%s' for samplerate=%d not found in db\n",
            dev_in, dev_out, samplerate);
        return STE_ADM_RES_NO_SUCH_SPEECH_CONFIG;
    }
    if (rc != SQLITE_ROW) {
        ALOG_ERR("sqlite3_step failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        adm_db_reinitialize_statement(&stmt_speechname);
        return STE_ADM_RES_DB_FATAL;
    }
    /* Return a config iterator matching */
    comp_name = (const char *) sqlite3_column_text(
            stmt_speechname, CID_SPEECH_NAME_TEMPLATE_NAME);
    ALOG_DB("returning config iterator for '%s'", comp_name);
    return adm_db_cfg_create_iter(iter, comp_name);
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_io_cfg_iter_create
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_io_cfg_iter_create(const char* dev,
                                    char **name,
                                    adm_db_cfg_iter_t* iter)
{
    int rc;
    const unsigned char *comp_name;
    const unsigned char *tmp_name;
    *name = NULL;

    ALOG_DB_VERBOSE("adm_db_io_cfg_iter_create enter: dev='%s'", dev);


    const char *actual_dev = NULL;

    if (adm_db_toplevel_mapping_get(dev, &actual_dev) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }


    if (sqlite3_reset(stmt_ioname) != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_bind_text(stmt_ioname, PID_IO_NAME_DEVICE_NAME,
                                actual_dev, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_step(stmt_ioname);
    if (rc == SQLITE_DONE) {
        ALOG_WARN("device '%s' not found in db\n", dev);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }
    if (rc != SQLITE_ROW) {
        ALOG_ERR("sqlite3_step failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        adm_db_reinitialize_statement(&stmt_ioname);
        return STE_ADM_RES_DB_FATAL;
    }

    tmp_name = sqlite3_column_text(
            stmt_ioname, CID_IO_NAME_OMXNAME);
    if (!*tmp_name) {
        ALOG_WARN("Error looking up OMXNAME for '%s'", actual_dev);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }
    /* The returned string is only valid until the next sqlite call */
    *name = strdup((const char*)tmp_name);
    if (*name == NULL) {
        ALOG_ERR("Out of memory\n");
        return STE_ADM_RES_ERR_MALLOC;
    }

    /* Return a config iterator matching */
    comp_name = sqlite3_column_text(
            stmt_ioname, CID_IO_NAME_IO_TEMPLATE_NAME);
    ALOG_DB_VERBOSE("returning config iterator for '%s'", comp_name);
    ste_adm_res_t res = adm_db_cfg_create_iter(iter, (const char*)comp_name);
    if (res != STE_ADM_RES_OK) {
        free(*name);
    }
    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_pre_effect_cfg_iter_create
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_pre_effect_cfg_iter_create(const char* dev,
                                            char **name,
                                            adm_db_cfg_iter_t* iter)
{
    int rc;
    const unsigned char *comp_name;
    const unsigned char *tmp_name;
    *name = NULL;

    ALOG_DB_VERBOSE("Enter adm_db_pre_effect_cfg_iter_create - dev='%s'", dev);

    const char *actual_dev = NULL;

    if (adm_db_toplevel_mapping_get(dev, &actual_dev) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }

    if (sqlite3_reset(stmt_pre_effect) != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_bind_text(stmt_pre_effect, PID_PRE_EFFECT_DEVICE_NAME,
                                actual_dev, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_step(stmt_pre_effect);
    if (rc == SQLITE_DONE) {
        ALOG_DB("device '%s' does not have any pre effect in db\n", dev);
        return STE_ADM_RES_OK;
    }
    if (rc != SQLITE_ROW) {
        ALOG_ERR("sqlite3_step failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        adm_db_reinitialize_statement(&stmt_pre_effect);
        return STE_ADM_RES_DB_FATAL;
    }

    tmp_name = sqlite3_column_text(
            stmt_pre_effect, CID_PRE_EFFECT_OMX_NAME);
    if (!*tmp_name) {
        ALOG_WARN("Error looking up OMXNAME for '%s'", dev);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }
    /* The returned string is only valid until the next sqlite call */
    *name = strdup((const char*)tmp_name);

    /* Return a config iterator matching */
    comp_name = sqlite3_column_text(
            stmt_pre_effect, CID_PRE_EFFECT_EFFECT_NAME);
    ALOG_DB_VERBOSE("returning config iterator for '%s'", comp_name);
    return adm_db_cfg_create_iter(iter, (const char*)comp_name);
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_comp_iter_destroy
//
///////////////////////////////////////////////////////////////////////////////
void adm_db_comp_iter_destroy(adm_db_comp_iter_t* iter)
{
    ADM_ASSERT(iter);
    ADM_ASSERT(stmt_compiter_busy);
    ADM_ASSERT(iter->magic == VALID_COMP_ITERATOR_MAGIC);

    stmt_compiter_busy = 0;
    memset(iter,0,sizeof(*iter));
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_global_settings_get
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_global_settings_get(const char* name, const char** data)
{
    int rc = SQLITE_OK;

    ADM_ASSERT(name);
    ADM_ASSERT(data);
    ADM_ASSERT(stmt_globalsettings);

    if (sqlite3_reset(stmt_globalsettings) != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset failed with error %s in global_settings_get\n",
            sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_bind_text(stmt_globalsettings, PID_GLOBAL_SETTINGS_NAME,
        name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s "
            "in global_settings_get\n", sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_step(stmt_globalsettings);
    if (rc == SQLITE_DONE) {
        ALOG_WARN("global_settings_get - setting '%s' not found in db\n", name);
        return STE_ADM_RES_DB_INCONSISTENT;
    }
    if (rc != SQLITE_ROW) {
        ALOG_ERR("sqlite3_step failed with error %s in global_settings_get\n",
            sqlite3_errmsg(adm_db_handle));
        adm_db_reinitialize_statement(&stmt_globalsettings);
        return STE_ADM_RES_DB_FATAL;
    }

    *data = (const char*)sqlite3_column_text(stmt_globalsettings,
        CID_GLOBAL_SETTINGS_DATA);
    if (!*data) {
        ALOG_ERR("sqlite3_column_text returned NULL in global_settings_get\n");
        return STE_ADM_RES_DB_FATAL;
    }

    return STE_ADM_RES_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_toplevel_mapping_set
//
///////////////////////////////////////////////////////////////////////////////
static ste_adm_res_t adm_db_toplevel_mapping_write_to_db(const char* toplevel_dev,
                                        const char* actual_dev)
{
    int rc;
    int nbr_of_changes;
    ste_adm_res_t res;

    if (sqlite3_reset(stmt_toplevel_mapping_set) != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_bind_text(stmt_toplevel_mapping_set,
        PID_TOPLEVEL_MAPPING_SET_TOPLEVEL_NAME, toplevel_dev, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    rc = sqlite3_bind_text(stmt_toplevel_mapping_set,
        PID_TOPLEVEL_MAPPING_SET_ACTUAL_NAME, actual_dev, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    rc = sqlite3_step(stmt_toplevel_mapping_set);
    switch (rc) {
    case SQLITE_DONE:
        break;
    case SQLITE_CONSTRAINT:
        ALOG_DB("Update command violated a constraint('%s', '%s')",
                toplevel_dev, actual_dev);
        res = STE_ADM_RES_UNKNOWN_DATABASE_ERROR;
        adm_db_reinitialize_statement(&stmt_toplevel_mapping_set);
        goto cleanup;
    default:
        ALOG_ERR("sqlite3_step failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        adm_db_reinitialize_statement(&stmt_toplevel_mapping_set);
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }
    nbr_of_changes = sqlite3_changes(adm_db_handle);
    ALOG_DB_VERBOSE("Update command changed %d entires in toplevel",
                    nbr_of_changes);
    switch (nbr_of_changes) {
    case 0:
        ALOG_DB("No such toplevel device: '%s'", toplevel_dev);
        res = STE_ADM_RES_NO_SUCH_DEVICE;
        goto cleanup;
    case 1:
        res = STE_ADM_RES_OK;
        break;
    default:
        ALOG_ERR("Error, toplevel map changed %d fields for '%s'",
                    nbr_of_changes, toplevel_dev);
        res = STE_ADM_RES_DB_FATAL;
        ADM_ASSERT(0);
        goto cleanup;
    }
cleanup:
    return res;
}

ste_adm_res_t adm_db_toplevel_mapping_set(  const char* toplevel_dev,
                                        const char* actual_dev)
{
    ADM_ASSERT(toplevel_dev);
    ADM_ASSERT(actual_dev);
    ADM_ASSERT(toplevel_map);

    int i;
    int rc;

    if (g_adm_db_tuningmode) {
        ALOG_INFO("adm_db_toplevel_mapping_set: Tuning mode enabled, writing setting to DB");
        ste_adm_res_t res = adm_db_toplevel_mapping_write_to_db(toplevel_dev, actual_dev);
        if (res != STE_ADM_RES_OK) {
            return res;
        }
    }

    if (sqlite3_reset(stmt_device_name_check) != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_bind_text(stmt_device_name_check,
        PID_DEVICE_NAME_CHECK_DEV_NAME, actual_dev, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_step(stmt_device_name_check);
    if (rc == SQLITE_DONE) {
        ALOG_ERR("adm_db_toplevel_mapping_set: device '%s' not found device table\n", actual_dev);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }
    if (rc != SQLITE_ROW) {
        ALOG_ERR("adm_db_toplevel_mapping_set: sqlite3_step failed with error %s\n",
            sqlite3_errmsg(adm_db_handle));
        adm_db_reinitialize_statement(&stmt_device_name_check);
        return STE_ADM_RES_DB_FATAL;
    }

    for (i = 0; i < toplevel_map_size; i++) {
        if (strncmp(toplevel_dev, toplevel_map[i].toplevel_name, ADM_MAX_DEVICE_NAME_LENGTH) == 0) {
            strncpy(toplevel_map[i].internal_name, actual_dev, ADM_MAX_DEVICE_NAME_LENGTH);
            ALOG_INFO("adm_db_toplevel_mapping_set: toplevel device '%s' now mapped to '%s'\n", toplevel_dev, actual_dev);
            return STE_ADM_RES_OK;
        }
    }

    ALOG_ERR("adm_db_toplevel_mapping_set: toplevel device '%s' not found devicemap table\n", toplevel_dev);
    return STE_ADM_RES_NO_SUCH_DEVICE;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_toplevel_mapping_get
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_toplevel_mapping_get(const char* toplevel_dev,
                                          const char** actual_dev)
{
    int i;

    ADM_ASSERT(toplevel_map);

    for (i = 0; i < toplevel_map_size; i++) {
        if (strncmp(toplevel_dev, toplevel_map[i].toplevel_name, ADM_MAX_DEVICE_NAME_LENGTH) == 0) {
            *actual_dev = toplevel_map[i].internal_name;
            return STE_ADM_RES_OK;
        }
    }

    ALOG_ERR("adm_db_toplevel_mapping_get - device '%s' not found in db\n", toplevel_dev);
    return STE_ADM_RES_NO_SUCH_DEVICE;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_toplevel_device_get
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_toplevel_device_get(const char* actual_dev,
                                         const char** toplevel_dev)
{
    int i;

    ADM_ASSERT(toplevel_map);

    for (i = 0; i < toplevel_map_size; i++) {
        if (strncmp(actual_dev, toplevel_map[i].internal_name, ADM_MAX_DEVICE_NAME_LENGTH) == 0) {
            *toplevel_dev = toplevel_map[i].toplevel_name;
            return STE_ADM_RES_OK;
        }
    }

    ALOG_ERR("adm_db_toplevel_mapping_get - device '%s' not found in db\n", toplevel_dev);
    return STE_ADM_RES_NO_SUCH_DEVICE;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_external_delay_get
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_external_delay_get(const char* toplevel_dev,
                                    int* external_delay)
{

    ADM_ASSERT(external_delay);
    ADM_ASSERT(toplevel_dev);
    ADM_ASSERT(ext_delay_list);

    int i;

    const char *actual_dev = NULL;

    if (adm_db_toplevel_mapping_get(toplevel_dev, &actual_dev) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }

    for (i = 0; i < ext_delay_list_size; i++) {
        if (strncmp(actual_dev, ext_delay_list[i].device_name, ADM_MAX_DEVICE_NAME_LENGTH) == 0) {
            *external_delay = ext_delay_list[i].delay;
            return STE_ADM_RES_OK;
        }
    }

    ALOG_ERR("adm_db_external_delay_get - device '%s' not found in db\n", actual_dev);
    return STE_ADM_RES_NO_SUCH_DEVICE;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_external_delay_set
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_external_delay_set(const char* dev, int external_delay)
{
    ADM_ASSERT(external_delay);
    ADM_ASSERT(dev);
    ADM_ASSERT(ext_delay_list);

    int i;

    const char *actual_dev = NULL;

    if (adm_db_toplevel_mapping_get(dev, &actual_dev) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }

    for (i = 0; i < ext_delay_list_size; i++) {
        if (strncmp(actual_dev, ext_delay_list[i].device_name, ADM_MAX_DEVICE_NAME_LENGTH) == 0) {
            ext_delay_list[i].delay = external_delay;
            ALOG_INFO("adm_db_external_delay_set: external delay for %s set to %d", actual_dev, external_delay);
            return STE_ADM_RES_OK;
        }
    }

    ALOG_ERR("adm_db_external_delay_set - device '%s' not found in db\n", actual_dev);
    return STE_ADM_RES_NO_SUCH_DEVICE;
}


///////////////////////////////////////////////////////////////////////////////
//
// adm_db_execute_sql
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_execute_sql(const char *sql)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    char *error = NULL;
    ADM_ASSERT(adm_db_handle);

    if (sqlite3_exec(adm_db_handle, sql, NULL, NULL, &error) != SQLITE_OK) {
        ALOG_ERR("adm_db_execute_sql: sqlite3_exec of (%s) failed with error %s\n",
            sql, error);
        res = STE_ADM_RES_DB_FATAL;
        sqlite3_free(error);
    }

    clear_cached_config_data();

    return res;
}
ste_adm_res_t adm_db_speech_apf_get(const char* dev_in,
                                        const char* dev_out,
                                        int samplerate,
                                        const char** apf_file)
{

    ALOG_DB("Enter %s  - dev_in=%s, dev_out=%s, samplerate=%d", __func__, dev_in, dev_out, samplerate);
    ADM_ASSERT(adm_db_handle);
    ADM_ASSERT(stmt_globalsettings);


    int ret;
    ste_adm_res_t res;
    const char *actual_dev_in = NULL;
    const char *actual_dev_out = NULL;

    if (adm_db_toplevel_mapping_get(dev_in, &actual_dev_in) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }

    if (adm_db_toplevel_mapping_get(dev_out, &actual_dev_out) != STE_ADM_RES_OK) {
        return STE_ADM_RES_DB_FATAL;
    }


    if (stmt_speech_apf_get == NULL){
        ret = sqlite3_prepare_v2(adm_db_handle, stmt_speech_apf_get_text,
            -1, &stmt_speech_apf_get, NULL);
        if (ret != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    if (sqlite3_reset(stmt_speech_apf_get) != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    ret = sqlite3_bind_text(stmt_speech_apf_get, 1,
                                actual_dev_in, -1, SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        ALOG_ERR("adm_db_speech_apf_get, sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }


    ret = sqlite3_bind_text(stmt_speech_apf_get, 2,
                                actual_dev_out, -1, SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        ALOG_ERR("adm_db_speech_apf_get, sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }


    ret = sqlite3_bind_int(stmt_speech_apf_get, 3,
                                samplerate);
    if (ret != SQLITE_OK) {
        ALOG_ERR("adm_db_speech_apf_get, sqlite3_bind_text failed with error %s",
                    sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }

    ret = sqlite3_step(stmt_speech_apf_get);
    if (ret == SQLITE_DONE) {
        ALOG_ERR("adm_db_speech_apf_get - device '%s' not found in db\n", dev_in);
        res = STE_ADM_RES_NO_SUCH_DEVICE;
        goto cleanup;
    }
    if (ret != SQLITE_ROW) {
        ALOG_ERR("sqlite3_step failed with error %s in adm_db_speech_apf_get\n",
            sqlite3_errmsg(adm_db_handle));
        res = STE_ADM_RES_DB_FATAL;
        adm_db_reinitialize_statement(&stmt_speech_apf_get);
        goto cleanup;;
    }

    *apf_file = (const char*)sqlite3_column_text(stmt_speech_apf_get, 0);
    ALOG_INFO("%s: Found APF preset = '%s' matching indev = '%s', outdev = '%s' and samplerate=%d.", __func__, *apf_file, dev_in, dev_out, samplerate);

    res = STE_ADM_RES_OK;

cleanup:

    ALOG_INFO("%s: Exit (%s).", __func__, (res == STE_ADM_RES_OK) ? "OK" : "ERROR");

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_set_tuning_mode
//
///////////////////////////////////////////////////////////////////////////////
void adm_db_set_tuning_mode(int enabled)
{
    ALOG_INFO("adm_db_set_tuning_mode mode = %d", enabled);
    g_adm_db_tuningmode = enabled;
}
