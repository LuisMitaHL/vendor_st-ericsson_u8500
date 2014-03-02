/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
// debugging, unit tests etc for database

#include <stdio.h>
#include <ctype.h>
#include <unistd.h> // unlink
#include <stdlib.h>
#include <string.h>

#include "ste_adm_db.h"
#include "ste_adm_dbg.h"

///////////////////////////////////////////////////////////////////////////////
//
// hexdump
//
///////////////////////////////////////////////////////////////////////////////
static void hexdump(const char* buf, unsigned int bytes)
{
    unsigned int cur=0;
    while (cur < bytes)
    {
        if (cur%16 == 0) printf("[MT]    %08X: ",cur);
        cur++;
        printf("%02X", 0xFF & *buf++);
        if (cur%16 == 0) printf("\n");
        else if (cur%4  == 0) printf(" ");
    }
    if (cur%16 != 0) printf("\n");
}

///////////////////////////////////////////////////////////////////////////////
//
// stmt_foreignkeys_text
//
///////////////////////////////////////////////////////////////////////////////
//static const char* stmt_foreignkeys_text = "PRAGMA foreign_keys = ON";
//static sqlite3_stmt* stmt_foreignkeys = NULL;

ste_adm_res_t dbg_dumpchain(const char* dev, ste_adm_effect_chain_type_t type)
{
    adm_db_comp_iter_t iter;
    if (adm_db_comp_iter_create(dev, NULL, type, 0, &iter) != STE_ADM_RES_OK) {
        printf("[MT]  dbg_dumpchain(%s) - adm_db_comp_iter_create failed\n", dev);
        return STE_ADM_RES_DB_FATAL;
    }



    const char* il_name;
    adm_db_cfg_iter_t cfg_iter;
    ste_adm_res_t res;

    memset(&cfg_iter, 0, sizeof(adm_db_cfg_iter_t));

    while ((res=adm_db_comp_iter_get_next(&iter, &il_name,
            &cfg_iter)) == STE_ADM_RES_OK) {
        printf("[MT]   IL: %s\n", il_name);


        int is_param;
        const char* index_name;
        const void* config_data;
        while ((res=adm_db_cfg_iter_get_next(&cfg_iter, &is_param,
                &index_name, &config_data)) == STE_ADM_RES_OK) {
            printf("[MT]   IsParam: %u IndexName: '%s'\n",
                    is_param, index_name);
               hexdump(config_data, *((const unsigned long*) config_data));
        }
        adm_db_cfg_iter_destroy(&cfg_iter);

        if (res != STE_ADM_RES_NO_MORE) {
            printf("[MT]  FAILURE\n");
            adm_db_comp_iter_destroy(&iter);
            return STE_ADM_RES_DB_FATAL;
        }
    }

    adm_db_comp_iter_destroy(&iter);

    if (res != STE_ADM_RES_NO_MORE) {
        printf("[MT]  FAILURE\n");
        return STE_ADM_RES_DB_FATAL;
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t dbg_dumpio(const char* dev)
{
    char* il_name = NULL;
    adm_db_cfg_iter_t cfg_iter;
    ste_adm_res_t res;
    int is_param;
    const char* index_name;
    const void* config_data;

    memset(&cfg_iter, 0, sizeof(adm_db_cfg_iter_t));

    res = adm_db_io_cfg_iter_create(dev, &il_name, &cfg_iter);
    if (STE_ADM_RES_OK != res) {
        res = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    }
    printf("[MT]   IL: %s\n", il_name);

    while ((res=adm_db_cfg_iter_get_next(&cfg_iter, &is_param,
            &index_name, &config_data)) == STE_ADM_RES_OK) {

        printf("[MT]   IsParam: %u IndexName: '%s'\n",
                is_param, index_name);
        hexdump(config_data, *((const unsigned long*) config_data));
    }
    adm_db_cfg_iter_destroy(&cfg_iter);

    if (res != STE_ADM_RES_NO_MORE) {
        printf("[MT]  FAILURE\n");
        res = STE_ADM_RES_DB_FATAL;
    } else {
        res = STE_ADM_RES_OK;
    }
cleanup:
    free(il_name);
    return res;
}


ste_adm_res_t dbg_dumpdev(const char* dev)
{
    printf("[MT]  ----------------------------------------\n");

    int is_input = 99;
    if (adm_db_io_info(dev, &is_input) != STE_ADM_RES_OK) {
        printf("[MT]  dbg_dumpdev(%s) - dbg_dumpdev failed\n", dev);
        return STE_ADM_RES_DB_FATAL;
    }

    printf("[MT]  Dump of device: %s   is_input:%u\n", dev, is_input);

    printf("[MT]  APP Chain:\n");
    if (dbg_dumpchain(dev, STE_ADM_APPLICATION_CHAIN) != STE_ADM_RES_OK) {
        printf("[MT]  dbg_dumpdev(%s) - dbg_dumpchain APP failed\n", dev);
        return STE_ADM_RES_DB_FATAL;
    }

    printf("[MT]  COMMON Chain:\n");
    if (dbg_dumpchain(dev, STE_ADM_COMMON_CHAIN) != STE_ADM_RES_OK) {
        printf("[MT]  dbg_dumpdev(%s) - dbg_dumpchain COMMON failed\n", dev);
        return STE_ADM_RES_DB_FATAL;
    }

    printf("[MT]  IO Chain:\n");
    if (dbg_dumpio(dev) != STE_ADM_RES_OK) {
        printf("[MT]  dbg_dumpdev(%s) - dbg_dumpio failed\n", dev);
        return STE_ADM_RES_DB_FATAL;
    }


    return STE_ADM_RES_OK;
}

void dbg_db_test()
{
    dbg_dumpdev("Speaker");
    dbg_dumpdev("Earpiece");
    dbg_dumpdev("HSetOut");
    dbg_dumpdev("Mic");
    dbg_dumpdev("HSetIn");
    dbg_dumpdev("A2DP");
/*    dbg_dumpdev("devA");
    dbg_dumpdev("devB");
    dbg_dumpdev("devC");
    dbg_dumpdev("devD");
    dbg_dumpdev("devE");
    dbg_dumpdev("devF");
    dbg_dumpdev("devNonexistant");
    dbg_dumpdev("onlySource");
    dbg_dumpdev("onlySink");

    dbg_dumpdev("dev_test1");
    dbg_dumpdev("dev_test2");
    dbg_dumpdev("dev_test3");
    dbg_dumpdev("dev_test4");
    dbg_dumpdev("dev_test5");
    dbg_dumpdev("dev_test6");
    dbg_dumpdev("dev_test7");
    dbg_dumpdev("dev_test8");
    dbg_dumpdev("dev_test9");
    dbg_dumpdev("dev_test10");
    dbg_dumpdev("dev_test11");
    dbg_dumpdev("dev_test12");
    dbg_dumpdev("dev_test13");
    dbg_dumpdev("dev_test14");
    dbg_dumpdev("dev_test15");
    dbg_dumpdev("dev_test16");
    dbg_dumpdev("dev_test17");

    dbg_dumpdev("dev_test20");  */
}



