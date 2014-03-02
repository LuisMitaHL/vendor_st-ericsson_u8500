/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __plmn_main_h__
#define __plmn_main_h__ (1)

#include <string.h>
#include "str_convert.h"

int plmn_init(void);
int plmn_sim_init(void);
int plmn_shutdown(void);
int plmn_add_nitz_name(const char *mcc_mnc_p,
                       const char *spn_p, const size_t spn_length, const size_t spn_spare, const string_encoding_t spn_encoding,
                       const char *mnn_p, const size_t mnn_length, const size_t mnn_spare, const string_encoding_t mnn_encoding);
int plmn_get_operator_name(const char *mcc_mnc_p, uint16_t lac, const char **spn_pp, const char **mnn_pp);
int plmn_get_operator_mcc_mnc(const char *spn_p, const char *mnn_p, char *mcc_mnc_p);

#endif /* __plmn_main_h__ */
