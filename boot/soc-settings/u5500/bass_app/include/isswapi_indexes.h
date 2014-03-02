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

/*!
 * Indexes of ISSW services
 * indexes are such as > 0x10000000
 */
#define  ISSWAPI_VA2PA_INDEX                    0x10000000
#define  ISSWAPI_INVALIDATE_MMU_TLB_INDEX       0x10000001
#define  ISSWAPI_GSTATE_ALLOC_INDEX             0x10000002
#define  ISSWAPI_GSTATE_FREE_INDEX              0x10000003
#define  ISSWAPI_GSTATE_GET_INDEX               0x10000004
#define  ISSWAPI_VERIFY_SIGNED_HEADER_INDEX     0x10000005
#define  ISSWAPI_PK_LOAD_KEY_INDEX              0x10000006
#define  ISSWAPI_PK_UNLOAD_KEY_INDEX            0x10000007
#define  ISSWAPI_TEE_MAKE_CURRENT_SESSION_RESIDENT_INDEX 0x10000008
#define  ISSWAPI_TEE_UNLOCK_CURRENT_SESSION_INDEX 0x10000009
#define  ISSWAPI_IS_PRODCHIP_INDEX              0x1000000A
#define  ISSWAPI_GET_CUTID_INDEX                0x1000000B
#define  ISSWAPI_GET_RT_FLAGS_INDEX             0x1000000C
#define  ISSWAPI_ENABLE_SECWD_INDEX             0x1000000D


#endif
