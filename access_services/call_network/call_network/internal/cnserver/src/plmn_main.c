/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "cn_log.h"
#include "cn_macros.h"
#include "plmn_main.h"
#include "plmn_list.h"
#include "plmn_file.h"
#include "plmn_sim.h"
#include "str_convert.h"

/* Constants */
#ifndef ENABLE_MODULE_TEST
static const char *plmn_static_operator_list_p = "/system/etc/plmn.operator.list";
static const char *plmn_nitz_operator_list_p = "/data/misc/plmn.operator.nitz.list";
#else
static const char *plmn_static_operator_list_p = "./plmn.operator.list";
static const char *plmn_nitz_operator_list_p = "./plmn.operator.nitz.list";
#endif

static int plmn_convert_mcc_mnc(const char *mcc_mnc_p, uint16_t *mcc_p, uint16_t *mnc_p);


int plmn_init(void)
{
    int ret;


    /* Load static operator list from file system */
    ret = plmn_file_load(PLMN_FILE, plmn_static_operator_list_p);

    if (ret < 0) {
        CN_LOG_E("Failed loading PLMN operator list from file!");
    } else {
        CN_LOG_D("Read %d entries into PLMN operator list.", ret);
    }

    /* Load network updated operator list from file system */
    ret = plmn_file_load(PLMN_NITZ_NAME, plmn_nitz_operator_list_p);

    if (ret < 0) {
        CN_LOG_D("No cached NITZ operator names.");
    } else {
        CN_LOG_D("Read %d cached NITZ entries into PLMN operator list.", ret);
    }

    return 0;
}


int plmn_sim_init()
{
    int ret;
    int result = 0;


    /*
     * Ready signal from SIM received, or file change on SIM has been detected.
     * 1. Check that SIM load isn't ongoing already
     * 2. Empty the PLMN operator list of all old SIM entries.
     * 3. Begin reading from SIM and populate the PLMN operator list.
     */

    if (plmn_sim_load_active()) {
        CN_LOG_W("Reading of (U)SIM to populate PLMN operator list already ongoing.");
        result = 0;
        goto exit;
    }

    plmn_list_empty(PLMN_SIM_FILE);

    ret = plmn_sim_load();

    if (ret < 0) {
        CN_LOG_E("Failed loading PLMN operator list from (U)SIM!");
    } else {
        CN_LOG_D("Begin reading (U)SIM entries into PLMN operator list.", ret);
    }

    /* Note: Reading has only been initiated, not completed */
exit:
    return result;
}


int plmn_shutdown()
{
    return plmn_list_empty(PLMN_ANY_SOURCE);
}


int plmn_add_nitz_name(const char *mcc_mnc_p,
                       const char *spn_p, const size_t spn_length, const size_t spn_spare, const string_encoding_t spn_encoding,
                       const char *mnn_p, const size_t mnn_length, const size_t mnn_spare, const string_encoding_t mnn_encoding)
{
    uint16_t mcc, mnc;
    size_t out_length;
    char spn[64];
    char mnn[32];
    int res;
    int result = 0;

    memset(spn,'\0',sizeof(spn));
    memset(mnn,'\0',sizeof(mnn));

    /* Extract MCC and MNC from compound string */
    if (plmn_convert_mcc_mnc(mcc_mnc_p, &mcc, &mnc) < 0) {
        CN_LOG_E("Invalid/unusable MCC & MNC field \"%s\"!", mcc_mnc_p);
        result = -1;
        goto exit;
    }

    /* Convert SPN to UTF-8 */
    if (0 < spn_length) {
        out_length = sizeof(spn) - 1;
        res = str_convert_string(spn_encoding, spn_length, (uint8_t *)spn_p, spn_spare,
                                 CHARSET_UTF_8, &out_length, (uint8_t *)spn);

        if (0 > res) {
            CN_LOG_E("Failed converting SPN to UTF-8!");
            result = -1;
            goto exit;
        }
    } else {
        spn[0] = '\0';
    }

    /* Convert MNN to UTF-8 */
    if (0 < mnn_length) {
        out_length = sizeof(mnn) - 1;
        res = str_convert_string(mnn_encoding, mnn_length, (uint8_t *)mnn_p, mnn_spare,
                                 CHARSET_UTF_8, &out_length, (uint8_t *)mnn);

        if (0 > res) {
            CN_LOG_E("Failed converting MNN to UTF-8!");
            result = -1;
            goto exit;
        }
    } else {
        mnn[0] = '\0';
    }

    /* Update operator list */
    if (plmn_list_add_entry(PLMN_NITZ_NAME, mcc, mnc, PLMN_LAC_MIN, PLMN_LAC_MAX, spn, mnn) < 0) {
        CN_LOG_E("Failed adding MCC=0x%03X, MNC=0x%02X, SPN=\"%s\", MNN=\"%s\" to operator list!",
                 mcc, mnc, spn, mnn);
        result = -1;
        goto exit;
    }

    /* Dump NITZ names to file system */
    if (plmn_file_dump(PLMN_NITZ_NAME, plmn_nitz_operator_list_p) < 0) {
        CN_LOG_E("Failed to dump NITZ names to \"%s\"",
                 plmn_nitz_operator_list_p);
        result = -1;
    }

exit:
    return result;
}


int plmn_get_operator_name(const char *mcc_mnc_p, uint16_t lac, const char **spn_pp, const char **mnn_pp)
{
    uint16_t mcc, mnc;
    int matches = 0;


    /* Silently reject all '0' or all 'F' MCC+MNC strings */
    if (strncmp(mcc_mnc_p, "00000", 5) == 0 || strncmp(mcc_mnc_p, "FFFFF", 5) == 0) {
        matches = -1;
        goto exit;
    }

    /* Extract MCC and MNC from compound string */
    if (plmn_convert_mcc_mnc(mcc_mnc_p, &mcc, &mnc) < 0) {
        CN_LOG_E("Invalid/unusable MCC & MNC field \"%s\"!", mcc_mnc_p);
        matches = -1;
        goto exit;
    }

    /*
     * NOTE: In North America the 3GPP TS 23.122 V10.0.0, Annex A, HPLMN Matching Criteria applies.
     * North America MCC range 310-316.
     *
     * In North America ?
     */
    if (0x310 > mcc || 0x316 < mcc) {

        /* Not in North America */
        matches = plmn_list_find_operator(PLMN_ANY_SOURCE, mcc, mnc, lac, spn_pp, mnn_pp);

    } else {

        /* In North America - 3GPP TS 23.122 V10.0.0, Annex A, HPLMN Matching Criteria - applies */

        /* Two-digit MNC ? */
        if (0x100 > mnc) {

            /* Two-digit, use the (2), (4) and (5) rules. Try to find three-digit MNC. */
            matches = plmn_list_find_operator(PLMN_SIM_FILE, mcc, mnc * 0x10, lac, spn_pp, mnn_pp);

        } else {

            /* Three-digit MNC */
            matches = plmn_list_find_operator(PLMN_SIM_FILE, mcc, mnc, lac, spn_pp, mnn_pp);

            /* Nothing found, & '0' in 3rd position ? */
            if (1 > matches && (0x0 == mnc % 0x10)) {

                /* Nothing found & '0' in 3rd, use the (2) and (6) rules. Try to find two-digit MNC. */
                matches = plmn_list_find_operator(PLMN_SIM_FILE, mcc, mnc / 0x10, lac, spn_pp, mnn_pp);
            }
        }

        if (1 > matches) {
            /* Search entire PLMN list, not only SIM entries. */
            matches = plmn_list_find_operator(PLMN_ANY_SOURCE, mcc, mnc, lac, spn_pp, mnn_pp);
        }
    }

    if (matches < 1) {
        CN_LOG_E("Unknown operator MCC=0x%03X, MNC=0x%02X, LAC=0x%X!", mcc, mnc, lac);
    }

exit:

    /* Number of matching entries */
    return matches;
}


int plmn_get_operator_mcc_mnc(const char *spn_p, const char *mnn_p, char *mcc_mnc_p)
{
    uint16_t mcc, mnc;
    int matches = 0;

    if (NULL == mcc_mnc_p) {
        CN_LOG_E("Illegal argument; mcc_mnc_p!");
        goto exit;
    }

    if (NULL != spn_p && 0 < strlen(spn_p)) {
        matches = plmn_list_find_operator_by_name(PLMN_ANY_SOURCE, PLMN_SPN, spn_p, &mcc, &mnc);

        if (matches < 1) {
            CN_LOG_E("Unknown operator SPN=\"%s\"!", spn_p);
        }
    } else if (NULL != mnn_p && 0 < strlen(mnn_p)) {
        matches = plmn_list_find_operator_by_name(PLMN_ANY_SOURCE, PLMN_MNN, mnn_p, &mcc, &mnc);

        if (matches < 1) {
            CN_LOG_E("Unknown operator MNN=\"%s\"!", mnn_p);
        }
    } else {
        CN_LOG_E("No SPN or MNN provided!");
    }

    /* Create MCC+MNC compound string. Note BCD. */
    if (matches > 0) {
        sprintf(mcc_mnc_p, "%03X%02X", mcc, mnc);
        CN_LOG_D("Converted name into MCC+MNC compund string: %s", mcc_mnc_p);
    }

exit:

    /* Number of matching entries */
    return matches;
}


int plmn_convert_mcc_mnc(const char *mcc_mnc_p, uint16_t *mcc_p, uint16_t *mnc_p)
{
    long mcc, mnc;
    char hexstr[8];
    int errnum;
    int result = 0;
    size_t length, pos;


    /* The MCC+MNC compound string is 5 or 6 characters long */
    length = strlen(mcc_mnc_p);

    if (5 > length || length > 6) {
        CN_LOG_E("Invalid length (%d) of MCC+MNC compound string \"%s\"!", length, mcc_mnc_p);
        result = -1;
        goto exit;
    }

    /* Validate MCC+MNC string contents. Only BCD ('0'-'9') plus 'D' (wildcard) are allowed */
    for (pos = 0; pos < length; pos++) {
        if (('0' > mcc_mnc_p[pos] || mcc_mnc_p[pos] > '9') &&
                'D' != mcc_mnc_p[pos]) {
            CN_LOG_E("Invalid content at position %d in MCC+MNC compound string \"%s\"!", pos + 1, mcc_mnc_p);
            result = -1;
            goto exit;
        }
    }

    /* Extract MCC from compound string */
    strcpy(hexstr, "0x");
    strncat(hexstr, mcc_mnc_p, 3);

    /* Convert from string */
    errno = 0;
    mcc = strtol(hexstr, NULL, 0);

    if (mcc < PLMN_MCC_MIN || mcc > PLMN_MCC_MAX) {
        errno = ERANGE;
    }

    if (errno == EINVAL || errno == ERANGE) {
        errnum = errno;
        CN_LOG_E("Error %d (%s) when extracting MCC from \"%s\"!",
                 errnum, strerror(errnum), mcc_mnc_p);
        result = -1;
        goto exit;
    }

    /* Extract MNC from compound string */
    strcpy(hexstr, "0x");
    strncat(hexstr, mcc_mnc_p + 3, 3);

    /* Convert from string */
    errno = 0;
    mnc = strtol(hexstr, NULL, 0);

    if (mnc < PLMN_MNC_MIN || mnc > PLMN_MNC_MAX) {
        errno = ERANGE;
    }

    if (errno == EINVAL || errno == ERANGE) {
        errnum = errno;
        CN_LOG_E("Error %d (%s) when extracting MNC from \"%s\"!",
                 errnum, strerror(errnum), mcc_mnc_p);
        result = -1;
        goto exit;
    }

    *mcc_p = mcc;
    *mnc_p = mnc;

exit:
    return 0;
}

