/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef BASS_APP_TEST_H
#define BASS_APP_TEST_H

#include <bass_app.h>
#include <stdint.h>

/**
 * Test functions for general bass app tests.
 */
void bass_app_test_all(void);
uint32_t bass_app_test_calc_digest(void);
uint32_t bass_app_test_check_payload_hash(void);
uint32_t bass_app_test_get_product_config(void);
uint32_t bass_app_test_get_productid(void);
uint32_t bass_app_test_smcl(void);
uint32_t bass_app_test_verify_signedheader(void);
uint32_t bass_app_test_get_arb_table_info(void);
uint32_t bass_app_test_drm_key(void);
uint32_t bass_app_stress_test(void);
uint32_t bass_app_static_uuid_test(void);
uint32_t bass_app_sha_aes_test(void);

#endif
