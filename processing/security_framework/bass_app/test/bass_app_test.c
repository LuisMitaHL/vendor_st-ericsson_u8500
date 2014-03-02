/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <bass_app_test.h>
#include <debug.h>

#ifdef OS_FREE
void bass_app_test_all(void)
#else
int main(void)
#endif
{
    bool passed = true;
    passed &= (bass_app_test_verify_signedheader() == 0);
    passed &= (bass_app_test_calc_digest() == 0);
    passed &= (bass_app_test_get_productid() == 0);
    passed &= (bass_app_test_get_product_config() == 0);
    passed &= (bass_app_test_check_payload_hash() == 0);
#ifndef LBP
    passed &= (bass_app_test_drm_key() == 0);
#endif
    passed &= (bass_app_stress_test() == 0);
    passed &= (bass_app_static_uuid_test() == 0);
    passed &= (bass_app_sha_aes_test() == 0);
#if 0
    /*
     * Only to be used as reference code.
     * Need ARB to enabled and initialized.
     * (Only supported at glacier setup, eg. h12).
     * To test on u8500:
     * - Enable below testcase.
     * - Disable (uncomment cops_check_arb_update() in cops_main.c
     * - Use eg. glacier h12 ISSW, signed with model ID.
     * - Update sw_type_map_hz3.txt to eg. "ipl ipl <arb_sw_version>"
     * - Flash command to initialize arb table: -a<model ID>
     * - Boot and run bass_app_testsuite.
     */
    printf("\nbass_app_test_get_arb_table_info\n");
    bass_app_test_get_arb_table_info();
#endif
    if (passed) {
        dprintf(ALWAYS, "All bass_app_testsuite tests PASSED!\n");
    } else {
        dprintf(ERROR, "Some tests in bass_app_testsuite FAILED (scroll up and "
                "review the log)!\n");
    }
#ifndef OS_FREE
    return 0;
#endif
}
