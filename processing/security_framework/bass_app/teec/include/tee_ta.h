/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef TEE_TA_H
#define TEE_TA_H

#define TEE_TA_HASH_SIZE 20

struct ta_signed_header {
    uint32_t magic;
    uint16_t size_of_signed_header;
    uint16_t size_of_signature;
    uint32_t sign_hash_type; /* see t_hash_type */
    uint32_t signature_type; /* see t_signature_type */
    uint32_t hash_type;      /* see t_hash_type */
    uint32_t payload_type;   /* see enum ta_payload_type */
    uint32_t flags;          /* reserved */
    uint32_t size_of_payload;
    uint32_t sw_vers_nbr;
    uint32_t load_address;
    uint32_t startup_address;
    uint32_t spare;          /* reserved */
#if 0
    uint8_t     hash[get_hash_length(this.hash_type)];
    uint8_t     signature[size_of_signature];
#endif

};

/* Trusted Application header */
struct tee_ta_head {
    TEEC_UUID uuid;
    uint32_t nbr_func;
    uint32_t ro_size;
    uint32_t rw_size;
    uint32_t zi_size;
    uint32_t got_size;
    uint32_t hash_type;
};

struct tee_ta_func_head {
    uint32_t cmd_id;
    void *start;
    uint32_t size;
    uint8_t hash[TEE_TA_HASH_SIZE];
};

#endif

