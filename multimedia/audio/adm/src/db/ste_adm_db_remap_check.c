/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define ADM_LOG_FILENAME "db_check"

#include "ste_adm_db_remap_check.h"
#include "ste_adm_db.h"
#include "ste_adm_dbg.h"
#include <string.h>


static sqlite3_stmt* stmt_devicesummary_new = NULL;
static sqlite3_stmt* stmt_devicesummary_cur = NULL;
static sqlite3_stmt* stmt_devicesummary_2dev_new = NULL;
static sqlite3_stmt* stmt_devicesummary_2dev_cur = NULL;

// INPUT: parameter IDs for stmt_globalsettings
#define PID_DEVICESUMMARY_DEVICE 1
#define PID_DEVICESUMMARY_IN_DEVICE 2
#define PID_DEVICESUMMARY_OUT_DEVICE 3
#define PID_DEVICESUMMARY_SAMPLERATE 4
#define PID_DEVICESUMMARY_DEVICE2 5


// OUTPUT: column IDs for stmt_dev_extname
#define CID_DEVICESUMMARY_CHAIN          0
#define CID_DEVICESUMMARY_CHAIN_DEF_NAME 1
#define CID_DEVICESUMMARY_POSITION       2
#define CID_DEVICESUMMARY_CONFIG_TYPE    3
#define CID_DEVICESUMMARY_STRUCT_DATA    4
#define CID_DEVICESUMMARY_INDEX_NAME     5
#define CID_DEVICESUMMARY_OMX_NAME       6


ste_adm_res_t adm_db_remap_init()
{
    static const char stmt_devicesummary_text[] =
"SELECT 'common' AS Chain, EffectChainDef.Name as ChainDefName, "
" EffectChainDef.Position as Position,"
" Template_Config.ConfigType, Template_Config.StructData, Template_Config.IndexName,"
" Template_OMXName.OMXName "
"FROM Device, Template_Config, EffectChainDef, Template_OMXName "
"WHERE Device.Name = ?1"
" AND Device.CommonEffectChainName = EffectChainDef.Name"
" AND Template_Config.Name = EffectChainDef.TemplateName"
" AND Template_OMXName.Name = EffectChainDef.TemplateName"
""
" UNION ALL "
""
"SELECT 'app' AS Chain, EffectChainDef.Name as ChainDefName, "
" EffectChainDef.Position as Position,"
" Template_Config.ConfigType, Template_Config.StructData, Template_Config.IndexName,"
" Template_OMXName.OMXName "
"FROM Device, Template_Config, EffectChainDef, Template_OMXName "
"WHERE Device.Name = ?1"
" AND Device.AppEffectChainName = EffectChainDef.Name"
" AND Template_Config.Name = EffectChainDef.TemplateName"
" AND Template_OMXName.Name = EffectChainDef.TemplateName"
""
" UNION ALL "
""
"SELECT 'vc_input' AS Chain, EffectChainDef.Name as ChainDefName, "
" EffectChainDef.Position as Position,"
" Template_Config.ConfigType, Template_Config.StructData, Template_Config.IndexName,"
" Template_OMXName.OMXName "
"FROM SpeechConfig, Template_Config, EffectChainDef, Template_OMXName "
"WHERE SpeechConfig.InDev = ?2"
" AND SpeechConfig.OutDev = ?3"
" AND SampleRate = ?4"
" AND SpeechConfig.InDevEffectChainName = EffectChainDef.Name"
" AND Template_Config.Name = EffectChainDef.TemplateName"
" AND Template_OMXName.Name = EffectChainDef.TemplateName"
""
" UNION ALL "
""
"SELECT 'vc_output' AS Chain, EffectChainDef.Name as ChainDefName, "
" EffectChainDef.Position as Position,"
" Template_Config.ConfigType, Template_Config.StructData, Template_Config.IndexName,"
" Template_OMXName.OMXName "
"FROM SpeechConfig, Template_Config, EffectChainDef, Template_OMXName "
"WHERE SpeechConfig.InDev = ?2"
" AND SpeechConfig.OutDev = ?3"
" AND SampleRate = ?4"
" AND SpeechConfig.OutDevEffectChainName = EffectChainDef.Name"
" AND Template_Config.Name = EffectChainDef.TemplateName"
" AND Template_OMXName.Name = EffectChainDef.TemplateName"
""
" UNION ALL "
""
"SELECT 'io' AS Chain, 'io_chain' as ChainDefName, "
" '1' as Position,"
" Template_Config.ConfigType, Template_Config.StructData, Template_Config.IndexName,"
" Template_OMXName.OMXName "
"FROM Device, Template_Config, Template_OMXName "
"WHERE Device.Name = ?1"
" AND Device.IOTemplateName = Template_Config.Name"
" AND Device.IOTemplateName = Template_OMXName.Name"
""
" ORDER BY "
"  Chain,"
"  Position,"
"  Template_Config.IndexName,"
"  Template_Config.ConfigType,"
"  Template_Config.StructData"
";";

    sqlite3* adm_db_handle;
    adm_db_get_handle(&adm_db_handle);

    if (stmt_devicesummary_new == NULL){
        int rc = sqlite3_prepare_v2(adm_db_handle, stmt_devicesummary_text,
            -1, &stmt_devicesummary_new, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    if (stmt_devicesummary_cur == NULL){
        int rc = sqlite3_prepare_v2(adm_db_handle, stmt_devicesummary_text,
            -1, &stmt_devicesummary_cur, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }


    //
    //
    // Query for verifying two devices at same time
    //
    //

    const char stmt_dev2_pretext[] =
"SELECT 'common2' AS Chain, EffectChainDef.Name as ChainDefName, "
" EffectChainDef.Position as Position,"
" Template_Config.ConfigType, Template_Config.StructData, Template_Config.IndexName,"
" Template_OMXName.OMXName "
"FROM Device, Template_Config, EffectChainDef, Template_OMXName "
"WHERE Device.Name = ?5"
" AND Device.CommonEffectChainName = EffectChainDef.Name"
" AND Template_Config.Name = EffectChainDef.TemplateName"
" AND Template_OMXName.Name = EffectChainDef.TemplateName"
""
" UNION ALL "
""
"SELECT 'app2' AS Chain, EffectChainDef.Name as ChainDefName, "
" EffectChainDef.Position as Position,"
" Template_Config.ConfigType, Template_Config.StructData, Template_Config.IndexName,"
" Template_OMXName.OMXName "
"FROM Device, Template_Config, EffectChainDef, Template_OMXName "
"WHERE Device.Name = ?5"
" AND Device.AppEffectChainName = EffectChainDef.Name"
" AND Template_Config.Name = EffectChainDef.TemplateName"
" AND Template_OMXName.Name = EffectChainDef.TemplateName"
""
" UNION ALL "
""
"SELECT 'io2' AS Chain, 'io_chain' as ChainDefName, "
" '1' as Position,"
" Template_Config.ConfigType, Template_Config.StructData, Template_Config.IndexName,"
" Template_OMXName.OMXName "
"FROM Device, Template_Config, Template_OMXName "
"WHERE Device.Name = ?5"
" AND Device.IOTemplateName = Template_Config.Name"
" AND Device.IOTemplateName = Template_OMXName.Name"
" UNION ALL "
"";

    char stmt_2dev_text[sizeof(stmt_dev2_pretext)-1+sizeof(stmt_devicesummary_text)];
    strcpy(stmt_2dev_text, stmt_dev2_pretext);
    strcat(stmt_2dev_text, stmt_devicesummary_text);


    if (stmt_devicesummary_2dev_new == NULL){
        int rc = sqlite3_prepare_v2(adm_db_handle, stmt_2dev_text,
            -1, &stmt_devicesummary_2dev_new, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    if (stmt_devicesummary_2dev_cur == NULL){
        int rc = sqlite3_prepare_v2(adm_db_handle, stmt_2dev_text,
            -1, &stmt_devicesummary_2dev_cur, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n",
                sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    return STE_ADM_RES_OK;
}

void adm_db_remap_destroy()
{
    sqlite3* adm_db_handle;
    (void) adm_db_get_handle(&adm_db_handle);

    if (stmt_devicesummary_new) {
        int rc = sqlite3_finalize(stmt_devicesummary_new);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_remap_destroy\n",
                sqlite3_errmsg(adm_db_handle));
        }
    }

    if (stmt_devicesummary_cur) {
        int rc = sqlite3_finalize(stmt_devicesummary_cur);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_remap_destroy\n",
                sqlite3_errmsg(adm_db_handle));
        }
    }

    if (stmt_devicesummary_2dev_new) {
        int rc = sqlite3_finalize(stmt_devicesummary_2dev_new);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_remap_destroy\n",
                sqlite3_errmsg(adm_db_handle));
        }
    }

    if (stmt_devicesummary_2dev_cur) {
        int rc = sqlite3_finalize(stmt_devicesummary_2dev_cur);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_finalize failed with error %s in adm_db_remap_destroy\n",
                sqlite3_errmsg(adm_db_handle));
        }
    }


    stmt_devicesummary_new = NULL;
    stmt_devicesummary_cur = NULL;
    stmt_devicesummary_2dev_new = NULL;
    stmt_devicesummary_2dev_cur = NULL;
}


#define DB_RETURN_ON_ERR(__rc) \
    if (__rc != SQLITE_OK) { \
        ALOG_ERR("%s failed, error %s\n", #__rc, sqlite3_errmsg(adm_db_handle)); \
        return STE_ADM_RES_UNKNOWN_DATABASE_ERROR; \
    }

ste_adm_res_t adm_db_remap_verify(
    int is_input,
    const char* cur_vc_in_device,
    const char* cur_vc_out_device,
    int vc_samplerate,
    const char* cur_device,
    const char* new_device,
    const char* cur_device2,
    const char* new_device2)
{
    sqlite3* adm_db_handle;
    adm_db_get_handle(&adm_db_handle);


    sqlite3_stmt* stmt_cur;
    sqlite3_stmt* stmt_new;
    const char* new_vc_in_device  = cur_vc_in_device;
    const char* new_vc_out_device = cur_vc_out_device;

    if (cur_device2[0]) {
        stmt_cur = stmt_devicesummary_2dev_cur;
        stmt_new = stmt_devicesummary_2dev_new;

        if (cur_vc_in_device[0]) {
            if (is_input) {
                new_vc_in_device  = new_device;
                new_vc_out_device = new_device2;
            } else {
                new_vc_in_device  = new_device2;
                new_vc_out_device = new_device;
            }
        }
    } else {
        stmt_cur = stmt_devicesummary_cur;
        stmt_new = stmt_devicesummary_new;

        if (cur_vc_in_device[0]) {
            if (is_input) {
                new_vc_in_device  = new_device;
            } else {
                new_vc_out_device = new_device;
            }
        }
    }

    DB_RETURN_ON_ERR(sqlite3_reset(stmt_cur));
    DB_RETURN_ON_ERR(sqlite3_reset(stmt_new));


    ALOG_INFO("------------------------------------------------------------------------------------------------------------------------------\n");
    ALOG_INFO("Comparing cur_device=%-20s cur_vc_device=[%s,%s] SR=%d\n", cur_device, cur_vc_in_device, cur_vc_out_device, vc_samplerate);
    ALOG_INFO("     with new_device=%-20s new_vc_device=[%s,%s] SR=%d\n", new_device, new_vc_in_device, new_vc_out_device, vc_samplerate);
    if (cur_device2[0]) {
        ALOG_INFO("     as well as cur_device2=%-20s --> new_device2=%-20s\n", cur_device2, new_device2);
    }
    ALOG_INFO("------------------------------------------------------------------------------------------------------------------------------\n");

    DB_RETURN_ON_ERR(sqlite3_bind_text(stmt_new, PID_DEVICESUMMARY_DEVICE,     new_device,        -1, SQLITE_STATIC));
    DB_RETURN_ON_ERR(sqlite3_bind_text(stmt_new, PID_DEVICESUMMARY_IN_DEVICE,  new_vc_in_device,  -1, SQLITE_STATIC));
    DB_RETURN_ON_ERR(sqlite3_bind_text(stmt_new, PID_DEVICESUMMARY_OUT_DEVICE, new_vc_out_device, -1, SQLITE_STATIC));
    DB_RETURN_ON_ERR(sqlite3_bind_int(stmt_new,  PID_DEVICESUMMARY_SAMPLERATE, vc_samplerate));

    DB_RETURN_ON_ERR(sqlite3_bind_text(stmt_cur, PID_DEVICESUMMARY_DEVICE,     cur_device,        -1, SQLITE_STATIC));
    DB_RETURN_ON_ERR(sqlite3_bind_text(stmt_cur, PID_DEVICESUMMARY_IN_DEVICE,  cur_vc_in_device,  -1, SQLITE_STATIC));
    DB_RETURN_ON_ERR(sqlite3_bind_text(stmt_cur, PID_DEVICESUMMARY_OUT_DEVICE, cur_vc_out_device, -1, SQLITE_STATIC));
    DB_RETURN_ON_ERR(sqlite3_bind_int(stmt_cur,  PID_DEVICESUMMARY_SAMPLERATE, vc_samplerate));

    if (cur_device2[0]) {
        DB_RETURN_ON_ERR(sqlite3_bind_text(stmt_new, PID_DEVICESUMMARY_DEVICE2, new_device2, -1, SQLITE_STATIC));
        DB_RETURN_ON_ERR(sqlite3_bind_text(stmt_cur, PID_DEVICESUMMARY_DEVICE2, cur_device2, -1, SQLITE_STATIC));
    }


    // If something incorrect is spotted, set is_bad, but keep analysing
    // all entries to it easier to understand from the logs what went wrong.
    int is_bad = 0;
    int rc_cur = SQLITE_OK;
    int rc_new = SQLITE_OK;
    while(1)
    {
        if (rc_cur != SQLITE_DONE) {
            rc_cur = sqlite3_step(stmt_cur);
        }

        if (rc_cur != SQLITE_DONE && rc_cur != SQLITE_ROW) {
            ALOG_ERR("sqlite3_step() failed for cur stmt; %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_UNKNOWN_DATABASE_ERROR;
        }

        if (rc_new != SQLITE_DONE) {
            rc_new = sqlite3_step(stmt_new);
        }

        if (rc_new != SQLITE_DONE && rc_new != SQLITE_ROW) {
            ALOG_ERR("sqlite3_step() failed for new stmt; %s\n", sqlite3_errmsg(adm_db_handle));
            return STE_ADM_RES_UNKNOWN_DATABASE_ERROR;
        }

        if (!is_bad && rc_cur != rc_new) {
            if (rc_cur == SQLITE_DONE) {
                ALOG_ERR("Old list ended prematurely\n");
                is_bad = 1;
            } else {
                ALOG_ERR("New list ended prematurely\n");
                is_bad = 1;
            }
        }

        if (rc_cur == SQLITE_DONE) {
            if (!is_bad) {
                ALOG_INFO("All compared OK\n");
                return STE_ADM_RES_OK;
            }
            ALOG_INFO("No more entries. Error causing incompatibility encountered previously.\n");
            return STE_ADM_RES_SETTINGS_NOT_COMPATIBLE;
        }


        const char* old_chain_type = (const char*) sqlite3_column_text(stmt_cur, CID_DEVICESUMMARY_CHAIN);
        const char* new_chain_type = (const char*) sqlite3_column_text(stmt_new, CID_DEVICESUMMARY_CHAIN);

        const char* old_chain_def = (const char*) sqlite3_column_text(stmt_cur, CID_DEVICESUMMARY_CHAIN_DEF_NAME);
        const char* new_chain_def = (const char*) sqlite3_column_text(stmt_new, CID_DEVICESUMMARY_CHAIN_DEF_NAME);

        const char* old_chain_pos = (const char*) sqlite3_column_text(stmt_cur, CID_DEVICESUMMARY_POSITION);
        const char* new_chain_pos = (const char*) sqlite3_column_text(stmt_new, CID_DEVICESUMMARY_POSITION);

        const char* old_config_type = (const char*) sqlite3_column_text(stmt_cur, CID_DEVICESUMMARY_CONFIG_TYPE);
        const char* new_config_type = (const char*) sqlite3_column_text(stmt_new, CID_DEVICESUMMARY_CONFIG_TYPE);

        const char* old_config_data = (const char*) sqlite3_column_text(stmt_cur, CID_DEVICESUMMARY_STRUCT_DATA);
        const char* new_config_data = (const char*) sqlite3_column_text(stmt_new, CID_DEVICESUMMARY_STRUCT_DATA);

        const char* old_config_index = (const char*) sqlite3_column_text(stmt_cur, CID_DEVICESUMMARY_INDEX_NAME);
        const char* new_config_index = (const char*) sqlite3_column_text(stmt_new, CID_DEVICESUMMARY_INDEX_NAME);

        const char* old_omxname = (const char*) sqlite3_column_text(stmt_cur, CID_DEVICESUMMARY_OMX_NAME);
        const char* new_omxname = (const char*) sqlite3_column_text(stmt_new, CID_DEVICESUMMARY_OMX_NAME);

        if (old_chain_type) {
            ALOG_INFO("Cur %-10s %-20s %s %-6s %-28s %-25s %s\n",
                 old_chain_type,old_chain_def,old_chain_pos,old_config_type,old_config_index,old_config_data,old_omxname);
        }

        if (new_chain_type) {
            ALOG_INFO("New %-10s %-20s %s %-6s %-28s %-25s %s\n",
                 new_chain_type,new_chain_def,new_chain_pos,new_config_type,new_config_index,new_config_data,new_omxname);
        }

        if (!is_bad) {
            if (strcmp(old_chain_type, new_chain_type)     != 0 ||
                strcmp(old_chain_pos,  new_chain_pos)      != 0 ||
                strcmp(old_config_type, new_config_type)   != 0 ||
                (strcmp(old_config_type, "Config") != 0 &&  strcmp(old_config_data, new_config_data) != 0) ||
                strcmp(old_config_index, new_config_index) != 0 ||
                strcmp(old_omxname, new_omxname) != 0)
            {
                ALOG_INFO("DB mismatch\n");
                is_bad = 1;
            }
        }

        ALOG_INFO("------------------------------------------------------------------------------------------------------------------------------\n");
    }
}
