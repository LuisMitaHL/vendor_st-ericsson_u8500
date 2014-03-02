/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup Upper_MAC_Core
 * \brief
 *
 */
/**
 * \file umac_utils.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_utils.h
 * \brief
 * The UMAC utililty file which handles gerneric functions which are used in
 * various files. It also handles endianness conversion.
 * \ingroup Upper_MAC_Core
 * \date 24/06/08 11:07
 */

#ifndef _UMAC_UTILS_H
#define _UMAC_UTILS_H

/******************************************************************************
	       INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "os_if.h"

#define IS_NLL_MAC_ADDRESS(_a)				\
	((*(uint16 *)(_a)       == (uint16)(0)) &&	\
	(*(((uint16 *)(_a))+1) == (((uint16)(0))))   &&	\
	(*(((uint16 *)(_a))+2) == (((uint16)(0)))))

#define IS_BROADCAST_MAC_ADDRESS(_a)		\
	((*(uint16 *)(_a)       == 0xFFFF) &&	\
	(*(((uint16 *)(_a))+1) == 0xFFFF) &&	\
	(*(((uint16 *)(_a))+2) == 0xFFFF))

#define IS_MULTICAST_MAC_ADDRESS(_a) \
	(*((uint8 *)(_a)) & ((uint8)0x01))

#define IS_ACTIVE_SCAN_REQUIRED_BEFORE_JOIN(Channel, MacAddr, SSidLength) \
(					\
	(!(uint32)Channel)       ||				      \
	IS_NLL_MAC_ADDRESS(MacAddr) ||				      \
	(!(uint32)SSidLength)						\
	)

#define IS_INVALID_JOIN_REQUEST(Channel, MacAddr, SSidLength)     \
(							\
	(!(uint32)Channel)       &&			      \
	IS_NLL_MAC_ADDRESS(MacAddr) &&			      \
	(!(uint32)SSidLength)					\
	)

uint32 WFM_UMAC_GetNextHigherRate(uint32 Values, uint32 Pos);
uint32 WFM_UMAC_GetNextLowerRate(uint32 Values, uint32 Pos);
uint32 WFM_UMAC_GetHighestRate(uint32 Values);
uint32 WFM_UMAC_GetLowestRate(uint32 Values);
uint32 WFM_UMAC_GetNumberOfOnes(uint32 Values);
uint32 WFM_UMAC_GetBitPosition(uint32 Value);
uint32 WFM_UMAC_FindMiddleRateIndex(uint32 Mask, uint32 CurrentIndex);

/******************************************************************************
			    MACRO DEFINITIONS
******************************************************************************/

#define WFM_UMAC_INDEX_TO_BITMASK(Index)    (1 << (Index))

#define WFM_UMAC_GET_LOWEST_RATE_INDEX(Mask)  WFM_UMAC_GetBitPosition(WFM_UMAC_GetLowestRate(Mask))

#define WFM_UMAC_GET_HIGHEST_RATE_INDEX(Mask) WFM_UMAC_GetBitPosition(WFM_UMAC_GetHighestRate(Mask))

#define WFM_UMAC_GET_NEXT_HIGHER_RATE_INDEX(Mask, CurrentIndex)	  \
	WFM_UMAC_GetBitPosition(WFM_UMAC_GetNextHigherRate(Mask, WFM_UMAC_INDEX_TO_BITMASK(CurrentIndex)))

#define WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(Mask, CurrentIndex)	   \
	WFM_UMAC_GetBitPosition(WFM_UMAC_GetNextLowerRate(Mask, WFM_UMAC_INDEX_TO_BITMASK(CurrentIndex)))

#define WFM_UMAC_GET_INTERMEDIATE_RATE_INDEX(Mask, CurrentIndex) \
	WFM_UMAC_FindMiddleRateIndex(Mask, CurrentIndex)

#define WFM_UMAC_GET_NUM_OF_RATES(Mask)		\
	WFM_UMAC_GetNumberOfOnes(Mask)

#define TIME_USEC_TO_TGT_UTS(x) ((x)/1000)
#define TIME_MSEC_TO_TGT_UTS(x) ((x))
#define TIME_SEC_TO_TGT_UTS(x)  ((x)*(1000))
#define TIME_MIN_TO_TGT_UTS(x)  ((x)*(1000)*(60))

/*for padding */
#define SIZE_PADDED_TO_MAKE_4_BYTES_ALIGNED(actual_sz)  (((actual_sz) + 3) & (~3))

#define GET_MIN(a, b) (((a) > (b)) ? (b) : (a))

/******************************************************************************
			ENDIAN CONVERSION MACROS
******************************************************************************/

/*  This section defines the endian conversion macros as specified in
    section 5.3 Endian Macros of the document WBF_WLAN_WFM_API.doc [rev 1.2]
*/
#ifdef LITTLE_ENDIAN_PLATFORM

/*le to cpu (both device and packets from air are little endian) */
#define umc_le16_to_cpu(x)  (x)
#define umc_le32_to_cpu(x)  (x)
/*cpu to le */
#define umc_cpu_to_le16(x)  umc_le16_to_cpu(x)
#define umc_cpu_to_le32(x)  umc_le32_to_cpu(x)

/*be to cpu */
#define umc_be16_to_cpu(x)		\
	((((x) << 8) & 0xff00)		\
	| (((x) >> 8) & 0x00ff)		\
	)
#define umc_be32_to_cpu(x)		\
	((((x) << 24) & 0xff000000)	\
	| (((x) <<  8) & 0x00ff0000)	\
	| (((x) >>  8) & 0x0000ff00)	\
	| (((x) >> 24) & 0x000000ff)	\
	)
/*cpu to be */
#define umc_cpu_to_be16(x)  umc_be16_to_cpu(x)
#define umc_cpu_to_be32(x)  umc_be32_to_cpu(x)
#else	/* LITTLE_ENDIAN_PLATFORM */
#define umc_le16_to_cpu(x)		      \
	((((x) << 8) & 0xff00)		   \
	| (((x) >> 8) & 0x00ff)		 \
	)

#define umc_le32_to_cpu(x)		      \
	((((x) << 24) & 0xff000000)	      \
	| (((x) <<  8) & 0x00ff0000)	    \
	| (((x) >>  8) & 0x0000ff00)	    \
	| (((x) >> 24) & 0x000000ff)	    \
	)

/*cpu to le */
#define umc_cpu_to_le16(x)  umc_le16_to_cpu(x)
#define umc_cpu_to_le32(x)  umc_le32_to_cpu(x)

/*be to cpu */
#define umc_be16_to_cpu(x)  (x)

#define umc_be32_to_cpu(x)  (x)
/*cpu to be */
#define umc_cpu_to_be16(x)  (x)
#define umc_cpu_to_be32(x)  (x)

#endif	/* LITTLE_ENDIAN_PLATFORM */

#endif	/* _UMAC_UTILS_H */
