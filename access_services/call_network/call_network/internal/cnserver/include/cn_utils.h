/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_utils_h__
#define __cn_utils_h__ (1)

#include <stdbool.h>

#include "cn_data_types.h"
#include "cn_log.h"

/* Number of seconds the result of the latest network search is considered valid
 * and may be used to search for an operators MCC+MNC using the long or short name.
 */
#define CN_NETWORK_SEARCH_VALID_S   (600)       /* 10 minutes */

void cn_util_set_latest_network_search(cn_manual_network_search_data_t *network_search_data_p);
int cn_util_search_latest_network_search(const char *spn_p, const char *mnn_p, const uint8_t act, char *mcc_mnc_p);
void cn_util_clear_latest_network_search(void);
cn_neighbour_rat_type_t cn_util_map_rat_to_neigh_rat(cn_rat_type_t rat);


#endif /* __cn_utils_h__ */
