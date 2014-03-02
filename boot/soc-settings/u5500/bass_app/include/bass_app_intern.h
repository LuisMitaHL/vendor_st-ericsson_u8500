/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef BASS_APP_INTERN_H
#define BASS_APP_INTERN_H


#define BASS_APP_VERIFY_SIGNED_HEADER_ID	0
#define BASS_APP_VERIFY_SIGNED_ELF_ID		1
#define BASS_APP_SHA256_INIT_ID			2
#define BASS_APP_SHA256_UPDATE_ID		3
#define BASS_APP_SHA256_FINAL_ID		4
#define BASS_APP_CALC_DIGEST_ID			5
#define BASS_APP_VERITY_START_MODEM		6
#define BASS_APP_APPLY_SOC_SETTINGS		7
#define BASS_APP_GET_AVS_PARAMETERS		8
#define BASS_APP_GET_AVS_FUSES                  13
#define BASS_APP_PRCMU_APPLY_AVS_SETTINGS       14
#define BASS_APP_PRCMU_SET_AP_EXEC              15
#define BASS_APP_PRCMU_DDR_PRE_INIT             16
#define BASS_APP_PRCMU_DDR_INIT                 17
#define BASS_APP_PRCMU_SET_DDR_SPEED            18
#define BASS_APP_PRCMU_ABB_INIT                 19
#define BASS_APP_SET_L2CC_FILTER_REGISTERS      20
#define BASS_APP_PRCMU_SET_MEMORY_SPEED		25
#define BASS_APP_PRCMU_SET_THERMAL_SENSORS      32



typedef struct bass_app_signed_header {
	uint32_t    magic;
	uint16_t    size_of_signed_header;
	uint16_t    size_of_signature;
	uint32_t    sign_hash_type;	/* see t_hash_type */
	uint32_t    signature_type;	/* see t_signature_type */
	uint32_t    hash_type;		/* see t_hash_type */
	uint32_t    payload_type;	/* see enum issw_payload_type */
	uint32_t    flags;		/* reserved */
	uint32_t    size_of_payload;
	uint32_t    sw_vers_nbr;
	uint32_t    load_address;
	uint32_t    startup_address;
	uint32_t    spare;		/* reserved */
#if 0
	uint8_t     hash[get_hash_length(this.hash_type)];
	uint8_t     signature[size_of_signature];
#endif
} bass_app_signed_header_t;

#endif
