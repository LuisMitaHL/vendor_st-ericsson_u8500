/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cn_log.h"
#include "cn_utils.h"


/* The CN server runs in a single thread context. No practical issue with static keyword */
static cn_manual_network_search_data_t *latest_network_search_data_p = NULL;
static time_t latest_network_search_time = 0;


/*
 * Manage storage and handling of the latest network search result. This information will
 * be used if and when an attempt to manually register to a network using the long or short
 * operator name is made.
 */
void cn_util_set_latest_network_search(cn_manual_network_search_data_t *network_search_data_p)
{
    struct timespec now;

    cn_util_clear_latest_network_search();
    latest_network_search_data_p = network_search_data_p;

    if (0 > clock_gettime(CLOCK_MONOTONIC, &now)) {
        CN_LOG_E("Unable to get current time using clock_gettime()!");
        latest_network_search_time = 0;
    } else {
        latest_network_search_time = now.tv_sec;
    }
}


int cn_util_search_latest_network_search(const char *spn_p, const char *mnn_p, const uint8_t act, char *mcc_mnc_p)
{
    bool spn_search = false;
    bool mnn_search = false;
    struct timespec now;
    int matches = -1;
    int i;

    if (NULL == latest_network_search_data_p) {
        /* No latest network search to search */
        goto exit_function;
    }

    if (0 > clock_gettime(CLOCK_MONOTONIC, &now)) {
        /* Unable to retrieve time */
        CN_LOG_E("Unable to get current time using clock_gettime()!");
        goto exit_function;
    }

    if (latest_network_search_time > now.tv_sec ||
            CN_NETWORK_SEARCH_VALID_S < (now.tv_sec - latest_network_search_time)) {
        /* Best before date has expired */
        goto exit_function;
    }

    /* Parameter validation */
    if (NULL != spn_p && 0 < strlen(spn_p)) {
        spn_search = true;
    } else if (NULL != mnn_p && 0 < strlen(mnn_p)) {
        mnn_search = true;
    } else {
        CN_LOG_E("No SPN or MNN names provided. Search not possible!");
        goto exit_function;
    }

    /* Search list for matching entry. Exit when matching entry found. */
    for (matches = i = 0;
            0 == matches && i < latest_network_search_data_p->num_of_networks;
            i++) {
        /* Check for matching radio-technology
         * 0 = GSM
         * 1 = GSM Compact (not supported)
         * 2 = UTRAN
         * 255 = Unknown (not specified, any will do)
         */
        if ((0 == act && /* Want GSM, but 'band_info == 2' means no GSM on this cell */
                2 == latest_network_search_data_p->cn_network_info[i].band_info) ||
                (2 == act && /* Want UTRAN, but 'umts_available == 0' means no UMTS/UTRAN on this cell */
                 0 == latest_network_search_data_p->cn_network_info[i].umts_available)) {
            /* Wanted radio-technology is not available on this cell. Keep looking */
            continue;
        }

        /* Check matching long or short operator name */
        if ((spn_search &&
                0 == strcmp(spn_p, latest_network_search_data_p->cn_network_info[i].long_op_name)) ||
                (mnn_search &&
                 0 == strcmp(mnn_p, latest_network_search_data_p->cn_network_info[i].short_op_name))) {
            /* Create MCC+MNC compound string. Note BCD format. */
            sprintf(mcc_mnc_p, "%03X%02X",
                    latest_network_search_data_p->cn_network_info[i].country_code,
                    latest_network_search_data_p->cn_network_info[i].network_code);
            CN_LOG_D("Converted name into MCC+MNC compund string: %s", mcc_mnc_p);
            matches++;
        }
    }

exit_function:
    return matches;
}


void cn_util_clear_latest_network_search()
{
    free(latest_network_search_data_p);
    latest_network_search_data_p = NULL;
}

cn_neighbour_rat_type_t cn_util_map_rat_to_neigh_rat(cn_rat_type_t rat)
{
    CN_LOG_D("%s: Current rat:%d", __FUNCTION__, rat);
    cn_neighbour_rat_type_t neighbour_rat = CN_NEIGHBOUR_RAT_TYPE_UNKNOWN;
    switch (rat) {
        case CN_RAT_TYPE_GSM:
        case CN_RAT_TYPE_GPRS:
        case CN_RAT_TYPE_EDGE_DTM:
        case CN_RAT_TYPE_GPRS_DTM:
        case CN_RAT_TYPE_EDGE: {
            neighbour_rat = CN_NEIGHBOUR_RAT_TYPE_2G;
            break;
        }
        case CN_RAT_TYPE_UMTS:
        case CN_RAT_TYPE_HSDPA:
        case CN_RAT_TYPE_HSUPA:
        case CN_RAT_TYPE_HSPA: {
            neighbour_rat = CN_NEIGHBOUR_RAT_TYPE_3G;
            break;
        }
        case CN_RAT_TYPE_UNKNOWN:
        case CN_RAT_TYPE_IS95A:
        case CN_RAT_TYPE_IS95B:
        case CN_RAT_TYPE_1xRTT:
        case CN_RAT_TYPE_EVDO_REV_0:
        case CN_RAT_TYPE_EVDO_REV_A:
        default:
            CN_LOG_E("%s: unexpected rat:%d, neighbour_rat %d", __FUNCTION__, rat,neighbour_rat);
            break;
    }
    return neighbour_rat;
}
