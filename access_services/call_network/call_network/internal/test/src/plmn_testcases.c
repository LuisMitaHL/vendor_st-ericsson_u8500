/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "testcases.h"
#include "module_test_main.h"
#include "cn_client.h"
#include "cn_log.h"

#include "plmn_main.h"
#include "plmn_list.h"
#include "plmn_file.h"

#include "sim_client.h"

#include "mal_net.h"


/* MAL callback prototypes, defined in mal_client.c */
extern void net_event_callback(int mal_event_id, void *data_p, mal_net_error_type error_code, void *client_tag_p);

/* SIM stub variables */
extern uintptr_t g_sim_client_tag_p;
extern int8_t g_ste_uicc_get_app_info__int;
extern sim_service_type_t g_ste_uicc_get_service_availability__sim_service_type;
extern int g_ste_uicc_sim_file_read_record__file_id;
extern int g_ste_uicc_sim_file_read_record__record_id;
extern int g_ste_uicc_sim_file_read_record__length;
extern char *g_ste_uicc_sim_file_read_record__file_path_p;


/* Note: During Call&Network startup there has already been attempts made to
 * load PLMN lists from the filesystem. Some of these may actually have been
 * successful. But to provide a known environment these lists will be
 * nuked by the tests below and substituted with content generated here.
 */

/* Keep constants below synchronized with plmn_main.c */
static const char *plmn_static_operator_list_p = "./plmn.operator.list";
static const char *plmn_nitz_operator_list_p = "./plmn.operator.nitz.list";
static const char *plmn_sim_operator_list_p = "./plmn.operator.sim.list";

/* Keep constants below synchronized with plmn_sim.c */
#define APP_GSM         "3F007F20"
#define APP_USIM        "3F007FFF"

#define EF_PNN          (0x6FC5)
#define EF_OPL          (0x6FC6)


#define OKI_DOKI        "Oki Doki"
#define CINGULAR_EXTEND "Cingular Extend"
#define OTHER_CARRIER   "Other Carrier"
#define LAC_ONLY        "LAC Only"


static const struct {
    const plmn_list_source_t source;
    const uint16_t  mcc;
    const uint16_t  mnc;
    const uint16_t  lac1;
    const uint16_t  lac2;
    const char     *spn_p;
    const char     *mnn_p;
} const plmn_operator_list[] = {

    // Abkhazia
    {PLMN_FILE, 0x289, 0x67 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Aquafon"           , "Aqua"},
    {PLMN_FILE, 0x289, 0x88 , PLMN_LAC_MIN, PLMN_LAC_MAX, "A-Mobile"          , "A-Mob"},

    // Afghanistan
    {PLMN_FILE, 0x412, 0x01 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Afghan Wireless"   , "AWCC"},
    {PLMN_FILE, 0x412, 0x20 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Telecom Afghan"    , "TDCA"},
    {PLMN_FILE, 0x412, 0x40 , PLMN_LAC_MIN, PLMN_LAC_MAX, "MTN Group"         , "MTN"},
    {PLMN_FILE, 0x412, 0x50 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Etisalat Afghan"   , "Etisalat"},

    // Albania
    {PLMN_FILE, 0x276, 0x01 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Albanian Mobile"   , "AlbaMob"},
    {PLMN_FILE, 0x276, 0x02 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Vodafone Albania"  , "Vodafone"},
    {PLMN_FILE, 0x276, 0x03 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Eagle Mobile"      , "Eagle"},
    {PLMN_FILE, 0x276, 0x04 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Plus Communication", "Plus"},

    // Algeria
    {PLMN_FILE, 0x603, 0x01 , PLMN_LAC_MIN, PLMN_LAC_MAX, "ATM Mobilis"       , "Mobilis"},
    {PLMN_FILE, 0x603, 0x02 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Orascom Telecom"   , "Djezzy"},
    {PLMN_FILE, 0x603, 0x03 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Wataniya Telecom"  , "Nedjma"},

    // Andorra
    {PLMN_FILE, 0x213, 0x03 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Servei Andorra"    , "Mobiland"},

    // Angola
    {PLMN_FILE, 0x631, 0x02 , PLMN_LAC_MIN, PLMN_LAC_MAX, "UNITEL S.a.r.l"    , "UNITEL"},

    // Anguilla
    {PLMN_FILE, 0x365, 0x010, PLMN_LAC_MIN, PLMN_LAC_MAX, "Weblinks Ltd"      , "Weblinks"},
    {PLMN_FILE, 0x365, 0x840, PLMN_LAC_MIN, PLMN_LAC_MAX, "Cable&Wireless"    , "C&W"},

    // Antigua and Barbuda
    {PLMN_FILE, 0x344, 0x030, PLMN_LAC_MIN, PLMN_LAC_MAX, "Antigua Public"    , "APUA"},
    {PLMN_FILE, 0x344, 0x920, PLMN_LAC_MIN, PLMN_LAC_MAX, "Cable&Wireless"    , "C&W"},
    {PLMN_FILE, 0x338, 0x050, PLMN_LAC_MIN, PLMN_LAC_MAX, "Antigua Wireless"  , "Digicel"},

    // Argentina
    {PLMN_FILE, 0x722, 0x010, PLMN_LAC_MIN, PLMN_LAC_MAX, "Telefonica Móviles", "Movistar"},
    {PLMN_FILE, 0x722, 0x020, PLMN_LAC_MIN, PLMN_LAC_MAX, "NII Holdings"      , "Nextel"},
    {PLMN_FILE, 0x722, 0x070, PLMN_LAC_MIN, PLMN_LAC_MAX, "Telefonica Móviles", "Movistar"},
    {PLMN_FILE, 0x722, 0x310, PLMN_LAC_MIN, PLMN_LAC_MAX, "AMX Argentina"     , "Claro"},
    {PLMN_FILE, 0x722, 0x320, PLMN_LAC_MIN, PLMN_LAC_MAX, "AMX Argentina"     , "Claro"},
    {PLMN_FILE, 0x722, 0x330, PLMN_LAC_MIN, PLMN_LAC_MAX, "AMX Argentina"     , "Claro"},
    {PLMN_FILE, 0x722, 0x034, PLMN_LAC_MIN, PLMN_LAC_MAX, "Telecom Personal"  , "Personal"},
    {PLMN_FILE, 0x722, 0x341, PLMN_LAC_MIN, PLMN_LAC_MAX, "Telecom Personal"  , "Personal"},
    {PLMN_FILE, 0x722, 0x350, PLMN_LAC_MIN, PLMN_LAC_MAX, "Telecom Personal"  , "Personal"},
    {PLMN_FILE, 0x722, 0x036, PLMN_LAC_MIN, PLMN_LAC_MAX, "Telecom Personal"  , "Personal"},

    // Sweden
    {PLMN_FILE, 0x240, 0x01 , PLMN_LAC_MIN, PLMN_LAC_MAX, "TeliaSonera"       , "Telia"},
    {PLMN_FILE, 0x240, 0x02 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Tre"               , "3"},
    {PLMN_FILE, 0x240, 0x03 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Nordisk Mobil"     , "Ice.net"},
    {PLMN_FILE, 0x240, 0x04 , PLMN_LAC_MIN, PLMN_LAC_MAX, "3G Infrastructure" , "3GIS"},
    {PLMN_FILE, 0x240, 0x05 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Sweden3G"          , "S3G"},
    {PLMN_FILE, 0x240, 0x06 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Telenor Sweden"    , "Telenor"},
    {PLMN_FILE, 0x240, 0x07 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Tele2"             , "Tele2"},
    {PLMN_FILE, 0x240, 0x08 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Telenor Sweden"    , "Telenor"},
    {PLMN_FILE, 0x240, 0x09 , PLMN_LAC_MIN, PLMN_LAC_MAX, "djuice"            , "djuice"},
    {PLMN_FILE, 0x240, 0x10 , PLMN_LAC_MIN, PLMN_LAC_MAX, "SpringMobil"       , "Spring"},
    {PLMN_FILE, 0x240, 0x11 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Lindholmen"        , "LSP"},
    {PLMN_FILE, 0x240, 0x12 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Barablu Mobile"    , "Barablu"},
    {PLMN_FILE, 0x240, 0x13 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Ventelo"           , "Ventelo"},
    {PLMN_FILE, 0x240, 0x14 , PLMN_LAC_MIN, PLMN_LAC_MAX, "TDC Mobil"         , "TDC"},
    {PLMN_FILE, 0x240, 0x15 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Wireless MN"       , "WMN"},
    {PLMN_FILE, 0x240, 0x16 , PLMN_LAC_MIN, PLMN_LAC_MAX, "42IT"              , "42IT"},
    {PLMN_FILE, 0x240, 0x17 , PLMN_LAC_MIN, PLMN_LAC_MAX, "TeliaSonera"       , "Telia"},
    {PLMN_FILE, 0x240, 0x20 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Wireless MMS"      , "WMMS"},
    {PLMN_FILE, 0x240, 0x21 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Trafikverket"      , "MobiSir"},
    {PLMN_FILE, 0x240, 0x25 , PLMN_LAC_MIN, PLMN_LAC_MAX, "DigiTelMobile"     , "DigiTel"},
    {PLMN_FILE, 0x240, 0x26 , PLMN_LAC_MIN, PLMN_LAC_MAX, "Beepsend"          , "Beep"},

    // USA
    {PLMN_FILE, 0x310, 0x012, PLMN_LAC_MIN, PLMN_LAC_MAX, "Verizon"           , "Verizon"},
    {PLMN_FILE, 0x310, 0x020, PLMN_LAC_MIN, PLMN_LAC_MAX, "Union Telephone"   , "Union"},
    {PLMN_FILE, 0x310, 0x026, PLMN_LAC_MIN, PLMN_LAC_MAX, "T-Mobile"          , "T-MO"},
    {PLMN_FILE, 0x310, 0x030, PLMN_LAC_MIN, PLMN_LAC_MAX, "Centennial"        , "Cen10ial"},
    {PLMN_FILE, 0x310, 0x032, PLMN_LAC_MIN, PLMN_LAC_MAX, "IT&E"              , "IT&E"},
    {PLMN_FILE, 0x310, 0x034, PLMN_LAC_MIN, PLMN_LAC_MAX, "Airpeak"           , "Airpeak"},
    {PLMN_FILE, 0x310, 0x070, PLMN_LAC_MIN, PLMN_LAC_MAX, "AT&T Mobility"     , "AT&T"},
    {PLMN_FILE, 0x310, 0x090, PLMN_LAC_MIN, PLMN_LAC_MAX, "AT&T Mobility"     , "AT&T"},
    {PLMN_FILE, 0x310, 0x130, PLMN_LAC_MIN, PLMN_LAC_MAX, "Cellular One"      , "Cellular"},
    {PLMN_FILE, 0x310, 0x140, PLMN_LAC_MIN, PLMN_LAC_MAX, "Pulse Mobile"      , "Pulse"},
    {PLMN_FILE, 0x310, 0x190, PLMN_LAC_MIN, PLMN_LAC_MAX, "Alaska Telecom"    , "Alaska"},
    {PLMN_FILE, 0x310, 0x311, PLMN_LAC_MIN, PLMN_LAC_MAX, "AT&T Mobility"     , "AT&T"},

    {PLMN_FILE, 0x311, 0x030, PLMN_LAC_MIN, PLMN_LAC_MAX, "Indigo Wireless"   , "Indigo"},
    {PLMN_FILE, 0x311, 0x070, PLMN_LAC_MIN, PLMN_LAC_MAX, "AT&T Extend"       , "AT&T"},
    {PLMN_FILE, 0x311, 0x130, PLMN_LAC_MIN, PLMN_LAC_MAX, "Alltel"            , "Alltel"},
    {PLMN_FILE, 0x311, 0x48D, PLMN_LAC_MIN, PLMN_LAC_MAX, "Verizon"           , "Verizon"},   // Note wildcard in MNC

    // AT&T wildcards entries
    {PLMN_FILE, 0x311, 0xDDD, PLMN_LAC_MIN, PLMN_LAC_MAX, "AT&T"               , "AT&T"},
    {PLMN_FILE, 0x312, 0xDDD, PLMN_LAC_MIN, PLMN_LAC_MAX, "AT&T"               , "AT&T"},
    {PLMN_FILE, 0x313, 0xDDD, PLMN_LAC_MIN, PLMN_LAC_MAX, "AT&T"               , "AT&T"},
    {PLMN_FILE, 0x314, 0xDDD, PLMN_LAC_MIN, PLMN_LAC_MAX, "AT&T"               , "AT&T"},
    {PLMN_FILE, 0x315, 0xDDD, PLMN_LAC_MIN, PLMN_LAC_MAX, "AT&T"               , "AT&T"},

    {PLMN_FILE, 0x901, 0x018, PLMN_LAC_MIN, PLMN_LAC_MAX, "Cellular@SEA"       , "AT&T"},

    // Lac range test entries
    {PLMN_SIM_FILE, 0x310, 0x027, 0x2500      , 0x2700      , CINGULAR_EXTEND      , "CingEx"},
    {PLMN_SIM_FILE, 0x310, 0x027, PLMN_LAC_MIN, PLMN_LAC_MAX, OTHER_CARRIER        , "Other"},
    {PLMN_SIM_FILE, 0x310, 0x180, PLMN_LAC_MIN, PLMN_LAC_MAX, OKI_DOKI             , "Oki"},
    {PLMN_SIM_FILE, 0x310, 0x170, PLMN_LAC_MIN, PLMN_LAC_MAX, OKI_DOKI             , "Oki"},
    {PLMN_SIM_FILE, 0x310, 0x150, PLMN_LAC_MIN, PLMN_LAC_MAX, OKI_DOKI             , "Oki"},
    {PLMN_SIM_FILE, 0x310, 0x210, 0x1250      , 0x7500      , OKI_DOKI             , "Oki"},
    {PLMN_SIM_FILE, 0x310, 0x210, 0x8000      , 0x9000      , CINGULAR_EXTEND      , "CingEx"},
    {PLMN_SIM_FILE, 0x310, 0x210, 0x1200      , 0x1200      , "Cingular Nation"    , "CingNat"},
    {PLMN_SIM_FILE, 0x310, 0x210, PLMN_LAC_MIN, PLMN_LAC_MAX, "Whole World"        , "World"},
    {PLMN_SIM_FILE, 0x310, 0x270, 0x2200      , 0x2400      , OKI_DOKI             , "Oki"},
    {PLMN_SIM_FILE, 0x310, 0x271, PLMN_LAC_MIN, PLMN_LAC_MAX, LAC_ONLY             , "LAC"},
    {PLMN_SIM_FILE, 0x310, 0x410, 0x2000      , 0x3000      , OKI_DOKI             , "Oki"},
    {PLMN_SIM_FILE, 0x310, 0x410, 0x4000      , 0x5000      , LAC_ONLY             , "LAC"},
    {PLMN_SIM_FILE, 0x310, 0x410, 0x6000      , 0x7000      , OTHER_CARRIER        , "Other"},
    {PLMN_SIM_FILE, 0x310, 0x410, PLMN_LAC_MIN, PLMN_LAC_MAX, CINGULAR_EXTEND      , "CingEx"},
    {PLMN_SIM_FILE, 0x302, 0x370, PLMN_LAC_MIN, PLMN_LAC_MAX, ""                   , ""},
    {PLMN_SIM_FILE, 0x310, 0xDDD, PLMN_LAC_MIN, PLMN_LAC_MAX, "Cingular Nation"    , "CingNat"},
    {PLMN_SIM_FILE, 0xDDD, 0xDDD, PLMN_LAC_MIN, PLMN_LAC_MAX, "Whole World"        , "World"},

    // End of list
    {PLMN_FILE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, "EOL", "EOL"},
};


static const struct {
    const int       length;
    const uint8_t   data[8];
} const ef_opl_record[] = {
    {8, {0x13, 0x01, 0x71, 0x00, 0x00, 0xFF, 0xFE, 0x02}}, /* 1 */
    {8, {0x13, 0xD0, 0xDD, 0x00, 0x00, 0xFF, 0xFE, 0x01}}, /* 2 */
    {8, {0x13, 0xD1, 0xDD, 0x00, 0x00, 0xFF, 0xFE, 0x01}}, /* 3 */
    {8, {0x13, 0xD2, 0xDD, 0x00, 0x00, 0xFF, 0xFE, 0x01}}, /* 4 */
    {8, {0x13, 0xD3, 0xDD, 0x00, 0x00, 0xFF, 0xFE, 0x01}}, /* 5 */
    {8, {0x13, 0xD4, 0xDD, 0x00, 0x00, 0xFF, 0xFE, 0x01}}, /* 6 */
    {8, {0x13, 0xD5, 0xDD, 0x00, 0x00, 0xFF, 0xFE, 0x01}}, /* 7 */
    {8, {0x09, 0xF1, 0x81, 0x00, 0x00, 0xFF, 0xFE, 0x04}}, /* 8 */
    {8, {0x13, 0x00, 0x51, 0x00, 0x00, 0xFF, 0xFE, 0x06}}, /* 9 */
    {8, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}, /* 10 */
};

#define EF_OPL_RECORDS  (10)


static const struct {
    const int       length;
    const uint8_t   data[24];
} const ef_pnn_record[] = {
    {24, {0x43, 0x05, 0x84, 0x41, 0xAA, 0x89, 0x0A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {24, {0x43, 0x0B, 0x83, 0x41, 0xAA, 0x89, 0x0A, 0x2A, 0xE2, 0xE9, 0x65, 0x37, 0x19, 0x45, 0x05, 0x84, 0x41, 0xAA, 0x89, 0x0A, 0xFF, 0xFF, 0xFF, 0xFF}},
    { 0, {0x00}},
    {24, {0x43, 0x0F, 0x87, 0xE3, 0x32, 0x9B, 0x5D, 0x67, 0x87, 0xE5, 0xA0, 0x30, 0x1D, 0x34, 0x2D, 0x06, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    { 0, {0x00}},
    {24, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
};

#define EF_PNN_RECORDS  (6)


tc_result_t plmn_test_reset()
{
    int result;
    int errnum;

    /* Nuke any existing PLMN lists on the filesystem */
    result = unlink(plmn_static_operator_list_p);

    if (result < 0) {
        if (ENOENT != errno) {
            errnum = errno;
            CN_LOG_E("unlink(\"%s\") error %d (%s)",
                     plmn_static_operator_list_p,
                     errnum,
                     strerror(errnum));
            return TC_RESULT_FAILED;
        }
    } else {
        CN_LOG_I("Deleted \"%s\"", plmn_static_operator_list_p);
    }

    result = unlink(plmn_nitz_operator_list_p);

    if (result < 0) {
        if (ENOENT != errno) {
            errnum = errno;
            CN_LOG_E("unlink(\"%s\") error %d (%s)",
                     plmn_nitz_operator_list_p,
                     errnum,
                     strerror(errnum));
            return TC_RESULT_FAILED;
        }
    } else {
        CN_LOG_I("Deleted \"%s\"", plmn_nitz_operator_list_p);
    }

    /* Reset the PLMN list.
     * Function only returns how many entries that were deleted. */
    result = plmn_list_empty(PLMN_ANY_SOURCE);
    CN_LOG_I("PLMN list emptied of all contents, %d entries", result);

    return TC_RESULT_OK;
}


/* PLMN list population */
tc_result_t plmn_test_list_populate()
{
    int added = 0, updated = 0, entries = 0;
    int result;
    int i;

    /* Populate list */
    for (i = 0; plmn_operator_list[i].mcc != 0xFFFF ; i++) {
        result = plmn_list_add_entry(plmn_operator_list[i].source,
                                     plmn_operator_list[i].mcc,
                                     plmn_operator_list[i].mnc,
                                     plmn_operator_list[i].lac1,
                                     plmn_operator_list[i].lac2,
                                     plmn_operator_list[i].spn_p,
                                     plmn_operator_list[i].mnn_p);

        if (result > 0) {
            added++;

        } else if (result < 0) {
            ;
        } else {
            CN_LOG_E("Updated!! source=%d, mcc=0x%X, mnc=0x%X, lac=0x%X-0x%X, spn=\"%s\", mnn=\"%s\"",
                     plmn_operator_list[i].source,
                     plmn_operator_list[i].mcc,
                     plmn_operator_list[i].mnc,
                     plmn_operator_list[i].lac1,
                     plmn_operator_list[i].lac2,
                     plmn_operator_list[i].spn_p,
                     plmn_operator_list[i].mnn_p);

            updated++;
        }

        entries++;
    }

    if (added != entries) {
        CN_LOG_E("Added %d, updated %d to a total of %d entries in PLMN list", added, updated, entries);
        return TC_RESULT_FAILED;
    } else {
        CN_LOG_I("Added %d to a total of %d entries in PLMN list", added, entries);
    }

    return TC_RESULT_OK;
}


/* PLMN list population */
tc_result_t plmn_test_list_repopulate()
{
    int added = 0, updated = 0, entries = 0;
    int result;
    int i;

    /* Populate list */
    for (i = 0; plmn_operator_list[i].mcc != 0xFFFF ; i++) {
        result = plmn_list_delete_entry(plmn_operator_list[i].source,
                                        plmn_operator_list[i].mcc,
                                        plmn_operator_list[i].mnc,
                                        plmn_operator_list[i].lac1,
                                        plmn_operator_list[i].lac2);

        if (result > 0) {
            added++;

        } else if (result < 0) {
            ;
        } else {
            CN_LOG_E("Updated!! source=%d, mcc=0x%X, mnc=0x%X, lac=0x%X-0x%X, spn=\"%s\", mnn=\"%s\"",
                     plmn_operator_list[i].source,
                     plmn_operator_list[i].mcc,
                     plmn_operator_list[i].mnc,
                     plmn_operator_list[i].lac1,
                     plmn_operator_list[i].lac2);

            updated++;
        }

        entries++;
    }

    if (added != entries) {
        CN_LOG_E("Added %d, updated %d to a total of %d entries in PLMN list", added, updated, entries);
        return TC_RESULT_FAILED;
    } else {
        CN_LOG_I("Added %d to a total of %d entries in PLMN list", added, entries);
    }

    return TC_RESULT_OK;
}


tc_result_t plmn_test_list_populate_negative()
{
    int added = 0, updated = 0, entries = 0;
    int result;
    int i;

    /* Populate with same list a second time */
    for (i = 0; plmn_operator_list[i].mcc != 0xFFFF ; i++) {
        /* The plmn_list_add_entry function will return the number of
         * entries added. Normally this will be one, but if an identical
         * entry exist zero will be returned to indicate that an entry
         * was updated. A negative value is returned only added if memory
         * could not be allocated.
         */
        /* This also tests the list search function since it is
         * used to test if an entry already exists or not.
         */
        result = plmn_list_add_entry(plmn_operator_list[i].source,
                                     plmn_operator_list[i].mcc,
                                     plmn_operator_list[i].mnc,
                                     plmn_operator_list[i].lac1,
                                     plmn_operator_list[i].lac2,
                                     plmn_operator_list[i].spn_p,
                                     plmn_operator_list[i].mnn_p);

        if (result > 0) {
            CN_LOG_E("Added!! source=%d, mcc=0x%X, mnc=0x%X, lac=0x%X-0x%X, spn=\"%s\", mnn=\"%s\"",
                     plmn_operator_list[i].source,
                     plmn_operator_list[i].mcc,
                     plmn_operator_list[i].mnc,
                     plmn_operator_list[i].lac1,
                     plmn_operator_list[i].lac2,
                     plmn_operator_list[i].spn_p,
                     plmn_operator_list[i].mnn_p);
            added++;

        } else if (result < 0) {
            ;
        } else {
            updated++;
        }

        entries++;
    }

    if (updated != entries) {
        CN_LOG_E("Added %d, updated %d from a total of %d entries in PLMN list", added, updated, entries);
        return TC_RESULT_FAILED;
    } else {
        CN_LOG_I("Updated %d from a total of %d entries in PLMN list", updated, entries);
    }

    return TC_RESULT_OK;
}


/* PLMN file handling */
tc_result_t plmn_test_file_read_negative()
{
    int result;

    /* Read PLMN list from filesystem
     * When this function is invoked the PLMN list files
     * should have been removed from the filesystem. So any
     * attempt to load any of them should fail.
     */
    result = plmn_file_load(PLMN_FILE, plmn_static_operator_list_p);

    if (result < 0) {
        CN_LOG_I("Expectedly failed loading PLMN list from file \"%s\"", plmn_static_operator_list_p);
    } else {
        CN_LOG_E("Unexpectedly succeeded in opening \"%s\"!", plmn_static_operator_list_p);
        return TC_RESULT_FAILED;
    }

    result = plmn_file_load(PLMN_NITZ_NAME, plmn_nitz_operator_list_p);

    if (result < 0) {
        CN_LOG_I("Expectedly failed loading PLMN list from file \"%s\"", plmn_nitz_operator_list_p);
    } else {
        CN_LOG_E("Unexpectedly succeeded in opening \"%s\"!", plmn_nitz_operator_list_p);
        return TC_RESULT_FAILED;
    }

    result = plmn_file_load(PLMN_SIM_FILE, plmn_sim_operator_list_p);

    if (result < 0) {
        CN_LOG_I("Expectedly failed loading PLMN list from file \"%s\"", plmn_sim_operator_list_p);
    } else {
        return TC_RESULT_FAILED;
    }

    return TC_RESULT_OK;
}


tc_result_t plmn_test_file_dump()
{
    int result;

    /* Dump list in memory to file on filesystem */
    result = plmn_file_dump(PLMN_FILE, plmn_static_operator_list_p);

    if (result < 0) {
        CN_LOG_E("Failed to dump PLMN list to \"%s\"", plmn_static_operator_list_p);
        return TC_RESULT_FAILED;
    }

    CN_LOG_I("Dumped %d entries in PLMN list to file \"%s\"",
             result, plmn_static_operator_list_p);

    return TC_RESULT_OK;
}


tc_result_t plmn_test_file_read()
{
    int deleted;
    int loaded;
    int count;
    int i;

    /* Reset the PLMN list.
     * Empty of PLMN_FILE originating entries, so we won't get a ton of
     * warnings about duplicate entries when loading them from file next.
     * Function only returns how many entries that were deleted. */
    deleted = plmn_list_empty(PLMN_FILE);
    CN_LOG_I("PLMN list emptied of PLMN_FILE contents, %d entries", deleted);

    /* Read PLMN list from filesystem
     * When this function is invoked the PLMN static list
     * file should have been dumped on the filesystem. So
     * any attempt to load it should succeed.
     */
    loaded = plmn_file_load(PLMN_FILE, plmn_static_operator_list_p);

    if (loaded < 0) {
        CN_LOG_E("Failed loading PLMN list from file \"%s\"!", plmn_static_operator_list_p);
        return TC_RESULT_FAILED;
    }

    /* Count number of PLMN_FILE entries in operator table and
     * compare with number of entries loaded from file */
    for (i = count = 0; plmn_operator_list[i].mcc != 0xFFFF ; i++) {
        if (PLMN_FILE == plmn_operator_list[i].source) {
            count++;
        }
    }

    if (loaded != count) {
        CN_LOG_E("Loaded %d entries from file, differs from %d in operator table!", loaded, count);
        return TC_RESULT_FAILED;
    }

    CN_LOG_I("Loaded %d entries into PLMN list from file \"%s\"",
             loaded, plmn_static_operator_list_p);

    return TC_RESULT_OK;
}


/* PLMN NITZ update */
tc_result_t plmn_test_nitz_update()
{
    mal_net_name_info mal_name_info;
    cn_error_code_t cn_result;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    char mcc_mnc[] = "12345";
    char spn_utf8[] = "plmn_test_nitz_update test";
    char mnn_utf8[] = "plmn test";
    uint8_t spn_ucs2[80];
    uint8_t mnn_ucs2[40];
    const char *spn_p;
    const char *mnn_p;
    size_t length;
    int result;

    /* Simulate NITZ network name update from network */
    mal_name_info.mcc_mnc_string      = (uint8_t *) mcc_mnc;

    length = sizeof(spn_ucs2) - 1;
    result = str_convert_string(CHARSET_UTF_8,
                                sizeof(spn_utf8) - 1, (uint8_t *)spn_utf8, /*spare */ 0,
                                CHARSET_UCS_2, &length, spn_ucs2);
    mal_name_info.full_operator_name.text_p = (char *)spn_ucs2;
    mal_name_info.full_operator_name.length = (uint8_t)length;
    mal_name_info.full_operator_name.add_ci = (uint8_t)0;
    mal_name_info.full_operator_name.dcs = 0x01; /* UCS-2 (iconv doesn't support GSM deafult character set) */
    mal_name_info.full_operator_name.spare = (uint8_t)0;

    length = sizeof(mnn_ucs2) - 1;
    result = str_convert_string(CHARSET_UTF_8,
                                sizeof(mnn_utf8) - 1, (uint8_t *)mnn_utf8, /*spare */ 0,
                                CHARSET_UCS_2, &length, mnn_ucs2);
    mal_name_info.short_operator_name.text_p = (char *)mnn_ucs2;
    mal_name_info.short_operator_name.length = (uint8_t)length;
    mal_name_info.short_operator_name.add_ci = (uint8_t)0;
    mal_name_info.short_operator_name.dcs = 0x01; /* UCS-2 (iconv doesn't support GSM deafult character set) */
    mal_name_info.short_operator_name.spare = (uint8_t)0;

    net_event_callback(MAL_NET_NITZ_NAME_IND, &mal_name_info, MAL_NET_SUCCESS, NULL);

    /* Receive CN_EVENT_NETWORK_INFO */
    cn_result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != cn_result) {
        CN_LOG_E("cn_message_receive failed!");
        return TC_RESULT_FAILED;
    } else {
        free(msg_p);
    }

    /* Verify that the NITZ network name is in the list. Note the
     * LAC. The NITZ name update has no LAC so the entry should have been
     * added to the list with the widest possible LAC range. */
    result = plmn_get_operator_name(mcc_mnc, 0x678 /* LAC */, &spn_p, &mnn_p);

    if (result == 1) {
        if (strcmp(spn_utf8, spn_p) != 0) {
            CN_LOG_E("Found NITZ update in list, but SPN name do not match \"%s\" != \"%s\"!", spn_utf8, spn_p);
            return TC_RESULT_FAILED;
        }

        if (strcmp(mnn_utf8, mnn_p) != 0) {
            CN_LOG_E("Found NITZ update in list, but MNN name do not match \"%s\" != \"%s\"!", mnn_utf8, mnn_p);
            return TC_RESULT_FAILED;
        }
    } else {
        CN_LOG_E("Failed to find NITZ update in list!");
        return TC_RESULT_FAILED;
    }

    /* Empty PLMN list of all NITZ names */
    result = plmn_list_empty(PLMN_NITZ_NAME);
    CN_LOG_I("PLMN list emptied of all NITZ names, %d entries", result);

    /* Read PLMN NITZ list from the filesystem */
    result = plmn_file_load(PLMN_NITZ_NAME, plmn_nitz_operator_list_p);

    if (result < 0) {
        CN_LOG_E("Failed loading PLMN NITZ list from file \"%s\"!", plmn_nitz_operator_list_p);
        return TC_RESULT_FAILED;
    } else {
        CN_LOG_I("PLMN NITZ list restored from file, %d entries", result);
    }

    /* Verify again that the NITZ network name is in the list. Note the
     * LAC. The NITZ name update has no LAC so the entry should have been
     * added to the list with the widest possible LAC range. */
    result = plmn_get_operator_name(mcc_mnc, 0x789 /* LAC */, &spn_p, &mnn_p);

    if (result == 1) {
        if (strcmp(spn_utf8, spn_p) != 0) {
            CN_LOG_E("Found NITZ update in list, but SPN name do not match \"%s\" != \"%s\"!", spn_utf8, spn_p);
            return TC_RESULT_FAILED;
        }

        if (strcmp(mnn_utf8, mnn_p) != 0) {
            CN_LOG_E("Found NITZ update in list, but MNN name do not match \"%s\" != \"%s\"!", mnn_utf8, mnn_p);
            return TC_RESULT_FAILED;
        }
    } else {
        CN_LOG_E("Failed to find NITZ update in list!");
        return TC_RESULT_FAILED;
    }

    CN_LOG_I("Successfully received NITZ name update, dumped to filesystem, read it back, and located it in the list");

    return TC_RESULT_OK;
}


/* PLMN SIM file reading
 * Also exercises string character set conversion
 */
tc_result_t plmn_test_sim_read()
{
    tc_result_t tc_result = TC_RESULT_OK;

    /* Signal already sent by calling function */
    TC_ASSERT(1 == g_ste_uicc_get_app_info__int);

    /* Generate callback from ste_uicc_get_sim_app_info */
    {
        ste_uicc_sim_app_info_response_t sim_app_info_response = {
            .status = STE_UICC_STATUS_CODE_OK,
            .app_type = SIM_APP_USIM,
        };
        sim_client_callback(STE_UICC_CAUSE_REQ_GET_APP_INFO, g_sim_client_tag_p, &sim_app_info_response, NULL);
    }

    /* Check that ste_uicc_get_service_availability has been called */
    TC_ASSERT(SIM_SERVICE_TYPE_PNN == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability for PNN */
    {
        ste_uicc_get_service_availability_response_t service_availability_response = {
            .service_availability = STE_UICC_SERVICE_AVAILABLE,
            .uicc_status_code = STE_UICC_STATUS_CODE_OK,
        };
        sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, g_sim_client_tag_p, &service_availability_response, NULL);
    }

    /* Check that ste_uicc_get_service_availability has been called */
    TC_ASSERT(SIM_SERVICE_TYPE_OPL == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability for OPL */
    {
        ste_uicc_get_service_availability_response_t service_availability_response = {
            .service_availability = STE_UICC_SERVICE_AVAILABLE,
            .uicc_status_code = STE_UICC_STATUS_CODE_OK,
        };
        sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, g_sim_client_tag_p, &service_availability_response, NULL);
    }

read_ef_opl_record:

    /* Check that ste_uicc_sim_file_read_record has been called for EF-OPL */
    CN_LOG_I("file_path=\"%s\", file_id=0x%X, record_id=%d",
             g_ste_uicc_sim_file_read_record__file_path_p,
             g_ste_uicc_sim_file_read_record__file_id,
             g_ste_uicc_sim_file_read_record__record_id);
    TC_ASSERT(0 == strcmp(g_ste_uicc_sim_file_read_record__file_path_p, APP_USIM) &&
              EF_OPL == g_ste_uicc_sim_file_read_record__file_id &&
              g_ste_uicc_sim_file_read_record__record_id > 0 &&
              g_ste_uicc_sim_file_read_record__record_id <= EF_OPL_RECORDS);

    /* Generate callback from ste_uicc_sim_file_read_record for EF-OPL */
    {
        ste_uicc_sim_file_read_record_response_t sim_file_read_record_response = {
            .uicc_status_code = STE_UICC_STATUS_CODE_OK,
            .uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
            .status_word = { 0, 0 },
            .data = (uint8_t *)ef_opl_record[g_ste_uicc_sim_file_read_record__record_id - 1].data,
            .length = ef_opl_record[g_ste_uicc_sim_file_read_record__record_id - 1].length,
        };
        sim_client_callback(STE_UICC_CAUSE_REQ_SIM_FILE_READ_GENERIC, g_sim_client_tag_p, &sim_file_read_record_response, NULL);
    }

    if (EF_OPL_RECORDS == g_ste_uicc_sim_file_read_record__record_id) {
        /* Last record read from EF-OPL by plmn_sim.c */
        goto exit;
    }

    /* Check that ste_uicc_sim_file_read_record has been called for EF-PNN */
    CN_LOG_I("file_path=\"%s\", file_id=0x%X, record_id=%d",
             g_ste_uicc_sim_file_read_record__file_path_p,
             g_ste_uicc_sim_file_read_record__file_id,
             g_ste_uicc_sim_file_read_record__record_id);
    TC_ASSERT(0 == strcmp(g_ste_uicc_sim_file_read_record__file_path_p, APP_USIM) &&
              EF_PNN == g_ste_uicc_sim_file_read_record__file_id &&
              0 < g_ste_uicc_sim_file_read_record__record_id &&
              EF_PNN_RECORDS >= g_ste_uicc_sim_file_read_record__record_id);

    /* Generate callback from ste_uicc_sim_file_read_record for EF-PNN */
    {
        ste_uicc_sim_file_read_record_response_t sim_file_read_record_response = {
            .uicc_status_code = STE_UICC_STATUS_CODE_OK,
            .uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
            .status_word = { 0, 0 },
            .data = (uint8_t *)ef_pnn_record[g_ste_uicc_sim_file_read_record__record_id - 1].data,
            .length = ef_pnn_record[g_ste_uicc_sim_file_read_record__record_id - 1].length,
        };
        sim_client_callback(STE_UICC_CAUSE_REQ_SIM_FILE_READ_GENERIC, g_sim_client_tag_p, &sim_file_read_record_response, NULL);
    }

    goto read_ef_opl_record;

exit:
    return tc_result;
}


tc_result_t plmn_test_sim_state_ready()
{
    tc_result_t tc_result = TC_RESULT_OK;

    /* Send signal that SIM is ready to kick-off reading of PLMN info from SIM */
    ste_uicc_sim_state_changed_t uicc_sim_state_changed = { .state = SIM_STATE_READY };
    sim_client_callback(STE_UICC_CAUSE_SIM_STATE_CHANGED, (uintptr_t)NULL, &uicc_sim_state_changed, NULL);

    tc_result = plmn_test_sim_read();

    return tc_result;
}


tc_result_t plmn_test_sim_change()
{
    tc_result_t tc_result = TC_RESULT_OK;

    /* Send signal that SIM has been changed due to proactive command */
    sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_IND, (uintptr_t)NULL, NULL, NULL);

    tc_result = plmn_test_sim_read();

    return tc_result;
}


tc_result_t plmn_test_sim_file_change()
{
    tc_result_t tc_result = TC_RESULT_OK;

    /* Send signal that SIM has been changed due to proactive command */
    ste_cat_pc_refresh_file_ind_t cat_pc_refresh_file_ind = { 0, {6, {0xAA, 0xBB, 0xCC, 0xDD, 0x6F, 0xC5}}};
    sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_FILE_IND, (uintptr_t)NULL, &cat_pc_refresh_file_ind, NULL);

    tc_result = plmn_test_sim_read();

    return tc_result;
}


tc_result_t plmn_test_sim_file_change_negative()
{
    tc_result_t tc_result = TC_RESULT_OK;

    /* Send signal that SIM has been changed due to proactive command */
    /* Negative test with byte order reversed to test for endian error */
    ste_cat_pc_refresh_file_ind_t cat_pc_refresh_file_ind = { 0, {6, {0xAA, 0xBB, 0xCC, 0xDD, 0xC5, 0x6F}}};
    sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_FILE_IND, (uintptr_t)NULL, &cat_pc_refresh_file_ind, NULL);

    tc_result = plmn_test_sim_read() == TC_RESULT_FAILED ? TC_RESULT_OK : TC_RESULT_FAILED;

    return tc_result;
}


/* PLMN list search by MCC+MNC
 * This tests both the ability to find existing entries,
 * not finding entries that do not exist or are unreachable (wildcard entries). */
tc_result_t plmn_test_mcc_mnc_search()
{
    int entries = 0, one_found = 0, not_found = 0, expected_not_found = 0, multiple_found = 0, expected_multiple_found = 0;
    int result;
    uint16_t lac;
    const char *spn_p;
    const char *mnn_p;
    char mcc_mnc[16];
    size_t length, pos;
    int i;

    /* Empty and repopulate list */
    result = plmn_list_empty(PLMN_ANY_SOURCE);

    for (i = 0; plmn_operator_list[i].mcc != 0xFFFF ; i++) {
        result = plmn_list_add_entry(plmn_operator_list[i].source,
                                     plmn_operator_list[i].mcc,
                                     plmn_operator_list[i].mnc,
                                     plmn_operator_list[i].lac1,
                                     plmn_operator_list[i].lac2,
                                     plmn_operator_list[i].spn_p,
                                     plmn_operator_list[i].mnn_p);
    }

    /* Search list */
    for (i = 0; plmn_operator_list[i].mcc != 0xFFFF ; i++) {
        sprintf(mcc_mnc, "%03X%02X",
                plmn_operator_list[i].mcc,
                plmn_operator_list[i].mnc);

        if (PLMN_LAC_MIN != plmn_operator_list[i].lac1 || PLMN_LAC_MAX != plmn_operator_list[i].lac2) {
            lac = plmn_operator_list[i].lac1 + i + 1;
        } else {
            lac = i + 1;
        }

        result = plmn_get_operator_name(mcc_mnc, lac, &spn_p, &mnn_p);

        if (result > 0) {
            if (result > 1) {
                /* One reason for finding multiple entries is that there are several
                 * entries with overlapping LAC ranges in the PLMN list. In these
                 * cases multiple matches are expected.
                 *
                 * Another reason is that there one or more matching wildcard entries
                 * in the PLMN list. If there is just one wildcard entry there must also
                 * be another non-wildcard entry found that match. In these cases the
                 * non-wildcard entry has priority and will be the one the search will
                 * return the name(s) for.
                 */

                /* At the moment there is no way to tell the reason for why a multiple
                 * match was made. So we count 'em all as expected.
                 */

                expected_multiple_found++;
                multiple_found++;
            } else {
                one_found++;
            }
        } else {
            /* No match found, why ? */
            bool wildcard = false;

            /* One reason for not being able to find an entry might be because we are
             * trying to search for a wildcard entry. That will not work with this
             * type of search. These are expected search failures.
             */
            length = strlen(mcc_mnc);

            for (pos = 0; pos < length; pos++) {
                if ('D' == mcc_mnc[pos]) {
                    wildcard = true;
                    break;
                }
            }

            if (wildcard) {
                /* Wildcard(s) found in entry, expected failure to find matching entry */
                expected_not_found++;
            } else {
                CN_LOG_E("Can't find mcc=%03X, mnc=%02X, lac=%04X !",
                         plmn_operator_list[i].mcc,
                         plmn_operator_list[i].mnc,
                         lac);
            }

            not_found++;
        }

        entries++;
    }

    /* Verify test results */
    if ((entries - (expected_not_found + expected_multiple_found)) != one_found ||
            expected_multiple_found != multiple_found ||
            expected_not_found != not_found) {
        CN_LOG_E("Expected/Got: single matches %d/%d, multiple matches %d/%d, non-matches %d/%d, in list with %d entries",
                 (entries - (expected_not_found + expected_multiple_found)), one_found,
                 expected_multiple_found, multiple_found,
                 expected_not_found, not_found,
                 entries);
        return TC_RESULT_FAILED;
    }

    CN_LOG_I("Found %d single matches, %d multiple matches, and %d non-matches in list with %d entries",
             one_found, multiple_found, not_found, entries);

    return TC_RESULT_OK;
}


tc_result_t plmn_test_lac_range_search()
{
    int failed = 0, found = 0;
    int result;
    const char *spn_p;
    const char *mnn_p;
    uint16_t mcc, mnc, lac;
    char mcc_mnc[16];
    char spn[32];

    /* Search using LAC ranges */
    mcc = 0x310, mnc = 0x270, lac = 0x2300, spn_p = NULL;
    sprintf(mcc_mnc, "%03X%02X", mcc, mnc);
    strcpy(spn, OKI_DOKI);
    result = plmn_get_operator_name(mcc_mnc, lac, &spn_p, &mnn_p);

    if (result > 0 && strcmp(spn_p, spn) == 0) {
        found++;
    } else {
        CN_LOG_E("Wrong operator found for mcc=%03x, mnc=%02X, lac=%04X, expected=\"%s\", got=\"%s\"",
                 mcc, mnc, lac, spn, spn_p);
        failed++;
    }

    mcc = 0x310, mnc = 0x27, lac = 0x2300, spn_p = NULL;
    sprintf(mcc_mnc, "%03X%02X", mcc, mnc);
    strcpy(spn, OKI_DOKI);
    result = plmn_get_operator_name(mcc_mnc, lac, &spn_p, &mnn_p);

    if (result > 0 && strcmp(spn_p, spn) == 0) {
        found++;
    } else {
        CN_LOG_E("Wrong operator found for mcc=%03x, mnc=%02X, lac=%04X, expected=\"%s\", got=\"%s\"",
                 mcc, mnc, lac, spn, spn_p);
        failed++;
    }

    mcc = 0x310, mnc = 0x270, lac = 0x2600, spn_p = NULL;
    sprintf(mcc_mnc, "%03X%02X", mcc, mnc);
    strcpy(spn, CINGULAR_EXTEND);
    result = plmn_get_operator_name(mcc_mnc, lac, &spn_p, &mnn_p);

    if (result > 0 && strcmp(spn_p, spn) == 0) {
        found++;
    } else {
        CN_LOG_E("Wrong operator found for mcc=%03x, mnc=%02X, lac=%04X, expected=\"%s\", got=\"%s\"",
                 mcc, mnc, lac, spn, spn_p);
        failed++;
    }

    mcc = 0x310, mnc = 0x27, lac = 0x2600, spn_p = NULL;
    sprintf(mcc_mnc, "%03X%02X", mcc, mnc);
    strcpy(spn, CINGULAR_EXTEND);
    result = plmn_get_operator_name(mcc_mnc, lac, &spn_p, &mnn_p);

    if (result > 0 && strcmp(spn_p, spn) == 0) {
        found++;
    } else {
        CN_LOG_E("Wrong operator found for mcc=%03x, mnc=%02X, lac=%04X, expected=\"%s\", got=\"%s\"",
                 mcc, mnc, lac, spn, spn_p);
        failed++;
    }

    mcc = 0x310, mnc = 0x270, lac = 0x2900, spn_p = NULL;
    sprintf(mcc_mnc, "%03X%02X", mcc, mnc);
    strcpy(spn, OTHER_CARRIER);
    result = plmn_get_operator_name(mcc_mnc, lac, &spn_p, &mnn_p);

    if (result > 0 && strcmp(spn_p, spn) == 0) {
        found++;
    } else {
        CN_LOG_E("Wrong operator found for mcc=%03x, mnc=%02X, lac=%04X, expected=\"%s\", got=\"%s\"",
                 mcc, mnc, lac, spn, spn_p);
        failed++;
    }

    mcc = 0x310, mnc = 0x27, lac = 0x2900, spn_p = NULL;
    sprintf(mcc_mnc, "%03X%02X", mcc, mnc);
    strcpy(spn, OTHER_CARRIER);
    result = plmn_get_operator_name(mcc_mnc, lac, &spn_p, &mnn_p);

    if (result > 0 && strcmp(spn_p, spn) == 0) {
        found++;
    } else {
        CN_LOG_E("Wrong operator found for mcc=%03x, mnc=%02X, lac=%04X, expected=\"%s\", got=\"%s\"",
                 mcc, mnc, lac, spn, spn_p);
        failed++;
    }

    mcc = 0x310, mnc = 0x271, lac = 0x2200, spn_p = NULL;
    sprintf(mcc_mnc, "%03X%02X", mcc, mnc);
    strcpy(spn, LAC_ONLY);
    result = plmn_get_operator_name(mcc_mnc, lac, &spn_p, &mnn_p);

    if (result > 0 && strcmp(spn_p, spn) == 0) {
        found++;
    } else {
        CN_LOG_E("Wrong operator found for mcc=%03x, mnc=%02X, lac=%04X, expected=\"%s\", got=\"%s\"",
                 mcc, mnc, lac, spn, spn_p);
        failed++;
    }

    if (failed) {
        CN_LOG_E("Found the correct operator %d times, failed %d times using LAC range!", found, failed);
        return TC_RESULT_FAILED;
    }

    CN_LOG_I("Found the correct operator %d out %d times using LAC range", found, found);

    return TC_RESULT_OK;
}


/* PLMN list search by name
 * This tests both the ability to find existing entries,
 * not finding entries that do not exist. */
tc_result_t plmn_test_name_search()
{
    int entries = 0, spn_found = 0, spn_expected_found = 0, mnn_found = 0, mnn_expected_found = 0;
    int result;
    const char *spn_p;
    const char *mnn_p;
    char mcc_mnc[16];
    char spn_mcc_mnc[16];
    char mnn_mcc_mnc[16];
    int i;

    /* NOTE: This test is designed to run with the PLMN list already populated. */

    /* Search list */
    for (i = 0; plmn_operator_list[i].mcc != 0xFFFF ; i++) {

        /* No wildcard entries */
        if ((plmn_operator_list[i].mcc & 0x00F) == 0x00D ||
                (plmn_operator_list[i].mcc & 0x0F0) == 0x0D0 ||
                (plmn_operator_list[i].mcc & 0xF00) == 0xD00) {
            continue;
        }

        if ((plmn_operator_list[i].mnc & 0x00F) == 0x00D ||
                (plmn_operator_list[i].mnc & 0x0F0) == 0x0D0 ||
                (plmn_operator_list[i].mnc & 0xF00) == 0xD00) {
            continue;
        }

        sprintf(mcc_mnc, "%03X%02X",
                plmn_operator_list[i].mcc,
                plmn_operator_list[i].mnc);

        spn_p = plmn_operator_list[i].spn_p;

        if (0 < strlen(spn_p)) {
            spn_expected_found++;

            result = plmn_get_operator_mcc_mnc(spn_p, NULL, &spn_mcc_mnc[0]);

            if (0 < result) {
                spn_found++;

                if (0 != strcmp(mcc_mnc, spn_mcc_mnc)) {
                    CN_LOG_E("Unexpected match: %s = %s !",
                             spn_mcc_mnc,
                             mcc_mnc);
                }
            }
        }

        mnn_p = plmn_operator_list[i].mnn_p;

        if (0 < strlen(mnn_p)) {
            mnn_expected_found++;

            result = plmn_get_operator_mcc_mnc(NULL, mnn_p, &mnn_mcc_mnc[0]);

            if (0 < result) {
                mnn_found++;

                if (0 != strcmp(mcc_mnc, mnn_mcc_mnc)) {
                    CN_LOG_E("Unexpected match: %s = %s !",
                             mnn_mcc_mnc,
                             mcc_mnc);
                }
            }
        }

        entries++;
    }

    /* Verify test results */
    /* All entries have both SPN and MNN name so there should be 1:1 match */
    if (spn_expected_found != spn_found ||
            mnn_expected_found != mnn_found) {
        CN_LOG_E("Expected/Got: SPN found %d/%d, MNN found %d/%d, in list with %d entries",
                 spn_found, spn_expected_found,
                 mnn_found, mnn_expected_found,
                 entries);
        return TC_RESULT_FAILED;
    }

    CN_LOG_I("Found %d SPN matches, and %d MNN matches in list with %d entries",
             spn_found, mnn_found, entries);

    return TC_RESULT_OK;
}
