/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <bass_app.h>
#include <bass_app_test.h>
#include <debug.h>
#include <tee_client_api.h>

/* true = success and false indicates error. */
static bool run_sha1_test(void)
{
    bool result = true;
    uint8_t hash_length = 20;
    t_bass_hash_type ht = BASS_APP_SHA1_HASH;
    uint8_t data1[] = "abc";
    uint8_t data1_length = 3;
    uint8_t data1_sha1[] = {0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
                            0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
                            0x9c, 0xd0, 0xd8, 0x9d};

    uint8_t data2[] =
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    size_t data2_length = 56;
    uint8_t data2_sha1[] = {0x84, 0x98, 0x3e, 0x44, 0x1c, 0x3b, 0xd2, 0x6e,
                            0xba, 0xae, 0x4a, 0xa1, 0xf9, 0x51, 0x29, 0xe5,
                            0xe5, 0x46, 0x70, 0xf1};
    bass_hash_t hash;

    memset(hash.value, 0, BASS_HASH_LENGTH);

    /* Create a SHA-1 hash for the data using TA. */
    if (BASS_RC_SUCCESS != bass_calcdigest(ht, data1, data1_length, &hash)) {
        dprintf(ERROR, "SHA-1 hash calculation failure\n");
        result = false;
    }

    /* Compare the calculated hash with reference data. */
    if (memcmp(hash.value, data1_sha1, hash_length)) {
        dprintf(ERROR, "TA SHA-1 comparison failure\n");
        result = false;
    } else {
        dprintf(INFO, "TA SHA-1 testcase 1 successful\n");
    }

    /* Test case 2. */
    /* Create a SHA-1 hash for the data using TA. */
    if (BASS_RC_SUCCESS != bass_calcdigest(ht, data2, data2_length, &hash)) {
        dprintf(ERROR, "SHA-1 hash calculation failure\n");
        result = false;
    }

    /* Compare the calculated hash with reference data. */
    if (memcmp(hash.value, data2_sha1, hash_length)) {
        dprintf(ERROR, "TA SHA-1 comparison failure\n");
        result = false;
    } else {
        dprintf(INFO, "TA SHA-1 testcase 2 successful\n");
    }

    return result;
}

static bool run_sha256_test(void)
{
    bool result = true;
    t_bass_hash_type ht = BASS_APP_SHA256_HASH;
    uint8_t hash_length = 32;
    uint8_t data1[] = "abc";
    uint8_t data1_length = 3;
    uint8_t data1_sha256[] = {0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
                              0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
                              0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
                              0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad};

    uint8_t data2[] =
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

    size_t data2_length = 56;
    uint8_t data2_sha256[] = {0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
                              0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
                              0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
                              0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1};
    bass_hash_t hash;
    memset(hash.value, 0, BASS_HASH_LENGTH);

    /* Create a SHA-256 hash for the data using TA. */
    if (BASS_RC_SUCCESS != bass_calcdigest(ht, data1, data1_length, &hash)) {
        dprintf(ERROR, "SHA-256 hash calculation failure\n");
        result = false;
    }

    /* Compare the calculated hash with reference data. */
    if (memcmp(hash.value, data1_sha256, hash_length)) {
        dprintf(ERROR, "TA SHA-256 comparison failure\n");
        result = false;
    } else {
        dprintf(INFO, "TA SHA-256 testcase 1 successful\n");
    }

    /* Test case 2. */
    memset(hash.value, 0, BASS_HASH_LENGTH);

    /* Create a SHA-256 hash for the data using TA. */
    if (BASS_RC_SUCCESS != bass_calcdigest(ht, data2, data2_length, &hash)) {
        dprintf(ERROR, "SHA-256 hash calculation failure\n");
        result = false;
    }

    /* Compare the calculated hash with reference data. */
    if (memcmp(hash.value, data2_sha256, hash_length)) {
        dprintf(ERROR, "TA SHA-256 comparison failure\n");
        result = false;
    } else {
        dprintf(INFO, "TA SHA-256 testcase 2 successful\n");
    }

    return true;
}

/*
 * Returns true if operation was successful. An error is indicated by false in
 * return value.
 */
uint32_t bass_app_test_calc_digest(void)
{
    uint32_t result = 0;
    if (false == run_sha1_test()) {
        dprintf(ERROR, "SHA-1 testcase failure\n");
        result = 1;
    } else {
        dprintf(INFO, "SHA-1 testcase successful\n");
    }

    if (false == run_sha256_test()) {
        dprintf(ERROR, "SHA-256 testcase failure\n");
        result = 1;
    } else {
        dprintf(INFO, "SHA-256 testcase successful\n");
    }
    return result;
}
