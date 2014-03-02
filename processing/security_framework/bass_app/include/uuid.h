#ifndef UUID_H
#define UUID_H
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * uuid's generated from http://www.itu.int/ITU-T/asn1/cgi-bin/uuid_generate
 */

/* ========================================================================= */
#define STATIC_TA_UUID { 0xBC765EDE, \
    0x6724, \
    0x11DF, \
    {0x8E, 0x12, 0xEC, 0xDB, 0xDF, 0xD7, 0x20, 0x85} }

/* Replicated ID list from tee_ta_static.h. */
#define BASS_APP_VERIFY_SIGNED_HEADER            0
#define BASS_APP_VERIFY_SIGNED_ELF               1
#define BASS_APP_SHA256_INIT                     2
#define BASS_APP_SHA256_UPDATE                   3
#define BASS_APP_SHA256_FINAL                    4
#define BASS_APP_CALC_DIGEST                     5
#define BASS_APP_VERITY_START_MODEM              6
#define BASS_APP_APPLY_SOC_SETTINGS              7
#define BASS_APP_GET_AVS_PARAMETERS              8
#define BASS_APP_GET_PRODUCT_ID                  9
#define BASS_APP_GET_PRODUCT_CONFIG             10
#define BASS_APP_GET_TA_KEY_HASH                12
#define BASS_APP_GET_AVS_FUSES                  13
#define BASS_APP_PRCMU_APPLY_AVS_SETTINGS       14
#define BASS_APP_PRCMU_SET_AP_EXEC              15
#define BASS_APP_PRCMU_DDR_PRE_INIT             16
#define BASS_APP_PRCMU_DDR_INIT                 17
#define BASS_APP_PRCMU_SET_DDR_SPEED            18
#define BASS_APP_PRCMU_ABB_INIT                 19
#define BASS_APP_SET_L2CC_FILTER_REGISTERS      20
#define BASS_APP_SET_POST_BOOT                  21
#define BASS_APP_GET_ENG_STATE                  22
#define BASS_APP_CHECK_ENG_MAGIC                23
#define BASS_APP_CHECK_PAYLOAD_HASH             24
#define BASS_APP_PRCMU_SET_MEMORY_SPEED         25
#define BASS_APP_IMAD_STORE                     26
#define BASS_APP_IMAD_RELEASE                   27
#define BASS_APP_IMAD_EXEC                      28
#define BASS_APP_HANDLE_BOOT_AREA_DATA          29
#define BASS_APP_L2CC_ENABLE                    30
#define BASS_APP_SET_DDR_REGION                 31
#define BASS_APP_PRCMU_SET_THERMAL_SENSORS      32
#define BASS_APP_GET_DIE_ID                     33
#define BASS_APP_CONF_SHARED_MUTEX              34
#define BASS_APP_SYSTEM_RESET                   35
#define BASS_APP_VERIFY_SIGNED_HEADER2          36
#define BASS_APP_GET_ARB_INFO_SIZES             37
#define BASS_APP_GET_ARB_INFO                   38
#define BASS_APP_SET_DRM_STATE                  42
#define BASS_APP_AUTH_DNT_MAGIC                 43

/* ========================================================================= */
#define SMCL_TA_UUID { 0xb6186b40, \
    0xcf93, \
    0x11df, \
    {0x8f, 0x39, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

/*
 * SMCL_MODEM_MEM_UNLOCK, 0:
 * Modem memory unlock for debug purpose (needed for modem core dump generation.
 * Return an error if modem debugging certificate has not been inserted.
 *
 * SMCL_MODEM_RESET_MEM_UNLOCK, 1:
 * Halt Modem CPU and unlock modem memory for modem FW reload.
 *
 * SMCL_MODEM_RESTART_MEM_LOCK, 2:
 * Reload Modem FW, reboot modem and lock modem memory.
 *
 * SMCL_MODEM_GET_MEMORY, 3:
 * Get the TCM memory.
 *
 * SMCL_ENCRYPT_PIN, 4:
 * SMCL taf id for encrypt/decrypt pin.
 */
#define SMCL_MODEM_MEM_UNLOCK                    0
#define SMCL_MODEM_RESET_MEM_UNLOCK              1
#define SMCL_MODEM_RESTART_MEM_LOCK              2
#define SMCL_MODEM_GET_MEMORY                    3
#define SMCL_ENCRYPT_PIN                         4

/* ========================================================================= */
#define STRESS_TA_UUID {0xe20f9200, \
    0x2578, \
    0x11e1, \
    {0xbe, 0x64, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

#define TEST_01_MALLOC_MEMCPY                    0

/* ========================================================================= */
#define TA_SHA_AES_UUID {0x28904980, \
    0x5137, \
    0x11df, \
    {0xbc, 0xcd, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

#define TA_SHA_AES_CMD_SHA224                    3
#define TA_SHA_AES_CMD_SHA256                    7
#define TA_SHA_AES_CMD_AES256ECB_ENC             8
#define TA_SHA_AES_CMD_AES256ECB_DEC             9
#define TA_SHA_AES_CMD_EXPORT_TA_FUNCS          10
#define TA_SHA_AES_CMD_UNEXPORT_TA_FUNCS        11
#define TA_SHA_AES_CMD_TEST_BSS                 12

/* ========================================================================= */
#define TA2TA_UUID { 0xbfd4dda0, \
    0xf705, \
    0x11df, \
    { 0x92, 0x23, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

#define TA_TA2TA_CMD_SHA224                       0
#define TA_TA2TA_CMD_SHA256                       1
#define TA_TA2TA_CMD_AES256ECB_ENC                2
#define TA_TA2TA_CMD_AES256ECB_DEC                3

/* ========================================================================= */
#define DRM_KEY_TA_UUID { 0x2a727de0, \
    0x902d, \
    0x11e1, \
    {0xa0, 0xef, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

/*
 * DRM_KEY_GET_RANDOM, 0:
 * Get random data.
 *
 * DRM_KEY_GET_PUBLIC_ID, 1:
 * Get public ID.
 *
 * DRM_KEY_GET_KEY_DATA, 2:
 * Get the DRM key data.
 *
 * DRM_KEY_ENCRYPT, 3:
 * Encrypt the DRM key.
 */
#define DRM_KEY_GET_RANDOM                  0
#define DRM_KEY_GET_PUBLIC_ID               1
#define DRM_KEY_GET_KEY_DATA                2
#define DRM_KEY_ENCRYPT                     3

/* The uuids below are for testing purpose. */
/* ========================================================================= */
#define INVALID_TA_UUID {0x01010101, \
    0x0101, \
    0x0101, \
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01} }

/* ========================================================================= */
#define UNKNOWN_TA_UUID {0xdeaddead, \
    0xdead, \
    0xdead, \
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xff, 0xff} }

#endif
