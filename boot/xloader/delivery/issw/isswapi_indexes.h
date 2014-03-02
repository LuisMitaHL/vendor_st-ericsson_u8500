/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
	\file       isswapi_indexes.h
	\brief      This file contains indexes related to the ISSW API.

	 \addtogroup ISSWAPI_INDEXES
	@{
 */
/*---------------------------------------------------------------------------*/
#ifndef ISSW_API_INDEXES_H
#define ISSW_API_INDEXES_H

/*
 * Indexes of ISSW services, >= 0x10000000
 */
#define ISSW_SERV_START_INDEX                  0x10000000
#define ISSWAPI_VA2PA_INDEX                    ISSW_SERV_START_INDEX
#define ISSWAPI_INVALIDATE_MMU_TLB_INDEX       (ISSW_SERV_START_INDEX + 0x01)
#define ISSWAPI_GSTATE_ALLOC_INDEX             (ISSW_SERV_START_INDEX + 0x02)
#define ISSWAPI_GSTATE_FREE_INDEX              (ISSW_SERV_START_INDEX + 0x03)
#define ISSWAPI_GSTATE_GET_INDEX               (ISSW_SERV_START_INDEX + 0x04)
#define ISSWAPI_VERIFY_SIGNED_HEADER_INDEX     (ISSW_SERV_START_INDEX + 0x05)
#define ISSWAPI_PK_LOAD_KEY_INDEX              (ISSW_SERV_START_INDEX + 0x06)
#define ISSWAPI_PK_UNLOAD_KEY_INDEX            (ISSW_SERV_START_INDEX + 0x07)
#define ISSWAPI_TEE_MAKE_CURRENT_SESSION_RESIDENT_INDEX \
                                                (ISSW_SERV_START_INDEX + 0x08)
#define ISSWAPI_TEE_UNLOCK_CURRENT_SESSION_INDEX (ISSW_SERV_START_INDEX + 0x09)
#define ISSWAPI_IS_PRODCHIP_INDEX              (ISSW_SERV_START_INDEX + 0x0A)
#define ISSWAPI_GET_CUTID_INDEX                (ISSW_SERV_START_INDEX + 0x0B)
#define ISSWAPI_GET_RT_FLAGS_INDEX             (ISSW_SERV_START_INDEX + 0x0C)
#define ISSWAPI_ENABLE_SECWD_INDEX             (ISSW_SERV_START_INDEX + 0x0D)
#define ISSWAPI_CLEAR_ENG_STATE_INDEX          (ISSW_SERV_START_INDEX + 0x0E)
#define ISSWAPI_INIT_ARB_TABLE_INDEX           (ISSW_SERV_START_INDEX + 0x0F)
#define ISSWAPI_GET_ARB_TABLE_INDEX            (ISSW_SERV_START_INDEX + 0x10)
#define ISSWAPI_STE_DNT_INIT_DATA_INDEX        (ISSW_SERV_START_INDEX + 0x11)
#define ISSWAPI_STE_DNT_VERIFY_HASH_INDEX      (ISSW_SERV_START_INDEX + 0x12)
#define ISSWAPI_VERIFY_SIGNED_HEADER2_INDEX    (ISSW_SERV_START_INDEX + 0x13)
#define ISSWAPI_GET_CLIENT_ID_INDEX            (ISSW_SERV_START_INDEX + 0x14)

#endif
