/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : tc_mal_basic.h
 * Description     : basic mal connectivity tests
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#ifndef TC_MAL_CC_H_
#define TC_MAL_CC_H_
#include "common.h"

psccd_test_state_type_t tc_cc_allow(int state);
psccd_test_state_type_t tc_hcmp_dcmp_ON(int state);

psccd_test_state_type_t tc_cc_not_allow(int state);
psccd_test_state_type_t tc_cc_allow_mod(int state);
psccd_test_state_type_t tc_cc_denied_by_gpds(int state);

//psccd_test_state_type_t tc_fdc(int state);


#endif //TC_MAL_CC_H_
