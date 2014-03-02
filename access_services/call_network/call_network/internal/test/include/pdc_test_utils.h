/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef PDC_TEST_UTILS_H_
#define PDC_TEST_UTILS_H_

#include "sim.h"
/***********************************************/
/* Utilities for basic (method-based) tests:   */
/***********************************************/

/* Allocates objects and sets up environment for basic pdc testing. */
cn_pdc_t* cn_pdc_setup_test_environment();

/* Cleans up resources allocated by test environment setup. */
void cleanup_test_environment(cn_pdc_t* cn_pdc_p);


/***************************************************/
/* Utilities for use-case (interface-based) tests: */
/***************************************************/
ste_uicc_sim_fdn_response_t* cn_pdc_make_sim_fdn_response(char* sim_fdn_numbers[], int number_of_fdn_numbers, ste_uicc_status_code_t status);
void free_EST_response(ste_uicc_sim_fdn_response_t** response_pp);
ste_uicc_sim_ecc_response_t * cn_pdc_make_sim_ecc_response();
void free_sim_ecc_response(ste_uicc_sim_ecc_response_t * response_p);
ste_cat_call_control_response_t* cn_pdc_make_sim_sat_cc_response(char* number_p, int length);
void free_sim_sat_response(ste_cat_call_control_response_t** p_pp);





#endif /* PDC_TEST_UTILS_H_ */
