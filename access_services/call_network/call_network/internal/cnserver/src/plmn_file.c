/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "cn_log.h"
#include "cn_macros.h"
#include "plmn_main.h"
#include "plmn_list.h"

/* Constants */
static const char *str_header_p = "header:";
static const char *str_mcc_p    = "mcc";
static const char *str_mnc_p    = "mnc";
static const char *str_lac1_p   = "lac1";
static const char *str_lac2_p   = "lac2";
static const char *str_spn_p    = "spn";
static const char *str_mnn_p    = "mnn";

static const int lac_mask       = 0x1 << PLMN_LAC1 |
                                  0x1 << PLMN_LAC2;

static const int mandatory_mask = 0x1 << PLMN_MCC |
                                  0x1 << PLMN_MNC |
                                  0x1 << PLMN_SPN |
                                  0x1 << PLMN_MNN;


/**
 * Populate PLMN list with contents from a file
 */
int plmn_file_load(const plmn_list_source_t source, const char *filepath_p)
{
    int result = 0;
    FILE *file_p = NULL;
    bool header_valid = false;
    int param_order[PLMN_ADD_ENTRY+2] = { PLMN_UNKNOWN };
    int errnum, lineno, mandatory, pos;
    unsigned int param_no;
    long mcc, mnc, lac[2];
    char *spn_p;
    char *mnn_p;
    char line[256];


    CN_LOG_D("Begin PLMN operator list loading, source=%d, filepath=\"%s\"",
             source, filepath_p);

    file_p = fopen(filepath_p, "r");

    if (!file_p) {
        errnum = errno;
        CN_LOG_E("Error %d (%s) when opening \"%s\" for reading!",
                 errnum, strerror(errnum), filepath_p);
        result = -1;
        goto exit;
    }

    lineno = 0;

    while (fgets(line, sizeof(line), file_p) != NULL) {
        lineno++;
        pos = 0;

        /* Handle comment lines (ignore) */
        if (line[0] == '#') {
            continue;
        }

        /* Parse header-line */
        if (strncasecmp(str_header_p, &line[pos], strlen(str_header_p)) == 0) {
            pos += strlen(str_header_p);

            /* Extract column names from header-line */
            header_valid = false;
            mandatory = 0;

            for (param_no = 0;
                    param_no < (sizeof(param_order) / sizeof(int)) && line[pos] != '\0';
                    param_no++) {

                /* Locate beginning of column name */
                while (line[pos] != '\0' && !isalpha(line[pos])) {
                    pos++;
                }

                if (line[pos] == '\0') {
                    /* Add the last instruction to the parameter order array */
                    param_order[param_no] = PLMN_ADD_ENTRY;
                    break;
                } else if (strncasecmp(str_mcc_p, &line[pos], strlen(str_mcc_p)) == 0) {
                    mandatory |= 0x1 << PLMN_MCC;
                    param_order[param_no] = PLMN_MCC;
                    pos += strlen(str_mcc_p);
                } else if (strncasecmp(str_mnc_p, &line[pos], strlen(str_mnc_p)) == 0) {
                    mandatory |= 0x1 << PLMN_MNC;
                    param_order[param_no] = PLMN_MNC;
                    pos += strlen(str_mnc_p);
                } else if (strncasecmp(str_lac1_p, &line[pos], strlen(str_lac1_p)) == 0) {
                    mandatory |= 0x1 << PLMN_LAC1;
                    param_order[param_no] = PLMN_LAC1;
                    pos += strlen(str_lac1_p);
                } else if (strncasecmp(str_lac2_p, &line[pos], strlen(str_lac2_p)) == 0) {
                    mandatory |= 0x1 << PLMN_LAC2;
                    param_order[param_no] = PLMN_LAC2;
                    pos += strlen(str_lac2_p);
                } else if (strncasecmp(str_spn_p, &line[pos], strlen(str_spn_p)) == 0) {
                    mandatory |= 0x1 << PLMN_SPN;
                    param_order[param_no] = PLMN_SPN;
                    pos += strlen(str_spn_p);
                } else if (strncasecmp(str_mnn_p, &line[pos], strlen(str_mnn_p)) == 0) {
                    mandatory |= 0x1 << PLMN_MNN;
                    param_order[param_no] = PLMN_MNN;
                    pos += strlen(str_mnn_p);
                } else {
                    param_order[param_no] = PLMN_UNKNOWN;
                    CN_LOG_W("Unrecognized column name \"%s\" on line %d, pos %d!", &line[pos], lineno, pos);

                    while (line[pos] != '\0' && isalpha(line[pos])) {
                        pos++;
                    }
                }
            }

            /* Check that all mandatory columns are present */
            if ((mandatory & mandatory_mask) != mandatory_mask) {
                CN_LOG_E("Header on line %d, one or more mandatory columns missing! Aborting operation.", lineno);
                result = -1;
                break;
            }

            /* Check that LAC1 & LAC2 are both present, or both absent */
            if ((mandatory & lac_mask) != 0 && (mandatory & lac_mask) != lac_mask) {
                CN_LOG_E("Header on line %d, columns LAC1 and LAC2 must both either be present or absent! Aborting operation.", lineno);
                result = -1;
                break;
            }

            /* Ensure last instruction is present in parameter order array */
            if (param_no < (sizeof(param_order) / sizeof(int)) - 1 &&
                    PLMN_ADD_ENTRY != param_order[param_no]) {
                param_order[param_no+1] = PLMN_ADD_ENTRY;
            }

            header_valid = true;
            continue;
        }

        /* Ignore empty lines */
        while (line[pos] != '\0' && isspace(line[pos])) {
            pos++;
        }

        if (line[pos] == '\0') {
            continue;
        }

        /* Don't try to parse line if there is no valid header */
        if (!header_valid) {
            continue;
        }

        /* Parse operator line */
        errno = errnum = 0;
        mcc = mnc = 0;
        lac[0] = PLMN_LAC_MIN;
        lac[1] = PLMN_LAC_MAX;
        spn_p = mnn_p = NULL;

        for (param_no = 0;
                param_no < sizeof(param_order) / sizeof(int) && !errnum;
                param_no++) {

            /* Locate beginning of next parameter value (digit or double-quote) */
            while (line[pos] != '\0' && line[pos] != '\"' && !isdigit(line[pos])) {
                pos++;
            }

            switch (param_order[param_no]) {
            case PLMN_MCC: {
                /* Country-code */
                char *end_p;
                mcc = strtol(&line[pos], &end_p, 0);
                pos = end_p - line;

                if (!errno && (mcc < PLMN_MCC_MIN || mcc > PLMN_MCC_MAX)) {
                    errno = ERANGE;
                }

                if (errno == EINVAL || errno == ERANGE) {
                    errnum = errno;
                    CN_LOG_E("Line %d, error %d (%s) when converting country-code (MCC)!",
                             lineno, errnum, strerror(errnum));
                }

                break;
            }

            case PLMN_MNC: {
                /* Network-code */
                char *end_p;
                mnc = strtol(&line[pos], &end_p, 0);
                pos = end_p - line;

                if (!errno && (mnc < PLMN_MNC_MIN || mnc > PLMN_MNC_MAX)) {
                    errno = ERANGE;
                }

                if (errno == EINVAL || errno == ERANGE) {
                    errnum = errno;
                    CN_LOG_E("Line %d, error %d (%s) when converting network-code (MNC)!",
                             lineno, errnum, strerror(errnum));
                }

                break;
            }

            case PLMN_LAC1:
            case PLMN_LAC2: {
                /* Location-area-code */
                char *end_p;
                int num = (param_order[param_no] == PLMN_LAC1) ? 0 : 1;
                lac[num] = strtol(&line[pos], &end_p, 0);
                pos = end_p - line;

                if (lac[num] < PLMN_LAC_MIN || lac[num] > PLMN_LAC_MAX) {
                    errno = ERANGE;
                }

                if (errno == EINVAL || errno == ERANGE) {
                    errnum = errno;
                    CN_LOG_E("Line %d, error %d (%s) when converting location_area_code (LAC%d)!",
                             lineno, errnum, strerror(errnum), num);
                }

                break;
            }

            case PLMN_SPN:

                /* Service-operator-name: Look for enclosing double quotes */
                if (line[pos] == '\"') {
                    pos++;
                    spn_p = &line[pos];

                    /* Find end of string and terminate it */
                    while (line[pos] != '\0' && line[pos] != '\"') {
                        pos++;
                    }

                    if (line[pos] == '\"') {
                        line[pos] = '\0';
                        pos++;
                    }
                } else {
                    errnum = EINVAL;
                    CN_LOG_E("Line %d, missing or incorrectly quoted service provider name (SPN)!", lineno);
                }

                break;

            case PLMN_MNN:

                /* Mobile-network-name: Look for enclosing double quotes */
                if (line[pos] == '\"') {
                    pos++;
                    mnn_p = &line[pos];

                    /* Find end of string and terminate it */
                    while (line[pos] != '\0' && line[pos] != '\"') {
                        pos++;
                    }

                    if (line[pos] == '\"') {
                        line[pos] = '\0';
                        pos++;
                    }
                } else {
                    errnum = EINVAL;
                    CN_LOG_E("Line %d, missing or incorrectly quoted mobile network name (MNN)!", lineno);
                }

                break;

            case PLMN_ADD_ENTRY: {
                /* Add entry to list */
                int ret = plmn_list_add_entry(source,
                                              (uint16_t)mcc, (uint16_t)mnc,
                                              (uint16_t)lac[0], (uint16_t)lac[1],
                                              spn_p, mnn_p);

                if (ret < 0) {
                    CN_LOG_E("Line %d, failed adding entry to list!", lineno);
                } else {
                    result += ret;
                }

                /* Fake an error to stop the line parsing */
                errnum = EINVAL;
                break;
            }

            default:
                break;
            }
        }
    }

    fclose(file_p);

exit:
    return result;
}


/**
 * Dump PLMN list contents to file
 */
int plmn_file_dump(const plmn_list_source_t source, const char *filepath_p)
{
    plmn_list_entry_t *curr_entry_p = NULL;
    FILE *file_p = NULL;
    int dumped = 0;
    int dump_lac;
    int errnum;
    int result;


    CN_LOG_D("Dumping source=%d, filepath=\"%s\"",
             source, filepath_p);

    file_p = fopen(filepath_p, "w");

    if (!file_p) {
        errnum = errno;
        CN_LOG_E("Error %d (%s) when opening \"%s\" for reading!",
                 errnum, strerror(errnum), filepath_p);
        dumped = -1;
        goto exit;
    }

    /* Write comment */
    result = fprintf(file_p,
                     "# Normal header-line below. Order and character case not significant.\n"
                     "# Header may occur any number of times in a file.\n#\n"
                     "# Column names:\n"
                     "# MCC  =Mobile Country Code [BCD] (mandatory)\n"
                     "# MNC  =Mobile Network Code [BCD] (mandatory)\n"
                     "# LAC1 =Location Area Code 1 (optional, mandatory if LAC2 present)\n"
                     "# LAC2 =Location Area Code 2 (optional, mandatory if LAC1 present)\n"
                     "# SPN  =Service Provider Name [enclosed by double quotes] (mandatory)\n"
                     "# MNN  =Mobile Network Name [enclosed by double quotes] (mandatory)\n");

    /* Loop over the list twice;
     *      1:st loop, dump non-LAC limited entries
     *      2:nd loop, dump LAC limited entries
     */
    for (dump_lac = 0; dump_lac < 2 && !(result < 0); dump_lac++) {

        /* NITZ sourced names never contain LAC limits, no need to look for them */
        if (PLMN_NITZ_NAME == source && 0 != dump_lac) {
            break;
        }

        /* Write header-line */
        if (0 == dump_lac) {
            result = fprintf(file_p, "#\nHeader:mcc,mnc,spn,mnn\n");
        } else {
            result = fprintf(file_p, "#\nHeader:mcc,mnc,lac1,lac2,spn,mnn\n");
        }

        /* Scan list for suitable entries to dump */
        for (curr_entry_p = plmn_list_get_head();
                curr_entry_p != NULL && !(result < 0);
                curr_entry_p = curr_entry_p->next_p) {

            if (PLMN_ANY_SOURCE != source && curr_entry_p->source != source) {
                continue;
            }

            if (PLMN_LAC_MIN == curr_entry_p->lac1 && PLMN_LAC_MAX == curr_entry_p->lac2) {
                if (0 == dump_lac) {
                    result = fprintf(file_p, "0x%03X\t0x%02X\t\"%s\"\t\"%s\"\n",
                                     curr_entry_p->mcc,
                                     curr_entry_p->mnc,
                                     &curr_entry_p->names[0],
                                     &curr_entry_p->names[curr_entry_p->mnn_offset]);

                    /* Count number of entries successfully written to file */
                    if (!(result < 0)) {
                        dumped++;
                    }
                }
            } else {
                if (0 != dump_lac) {
                    result = fprintf(file_p, "0x%03X\t0x%02X\t0x%04X\t0x%04X\t\"%s\"\t\"%s\"\n",
                                     curr_entry_p->mcc,
                                     curr_entry_p->mnc,
                                     curr_entry_p->lac1,
                                     curr_entry_p->lac2,
                                     &curr_entry_p->names[0],
                                     &curr_entry_p->names[curr_entry_p->mnn_offset]);

                    /* Count number of entries successfully written to file */
                    if (!(result < 0)) {
                        dumped++;
                    }
                }
            }
        }
    }

    /* Check fprintf return code */
    if (result < 0) {
        errnum = errno;
        CN_LOG_E("Error %d (%s) when writing to \"%s\", dump aborted!",
                 errnum, strerror(errnum), filepath_p);
    }

    fclose(file_p);

exit:
    return dumped;
}

