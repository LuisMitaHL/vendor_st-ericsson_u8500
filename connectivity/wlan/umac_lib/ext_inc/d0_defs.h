/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
* \file
* \ingroup GLOBAL
*
* \brief IEEE 802.0 definitions
*/

#ifndef _D0_DEFS_H
#define _D0_DEFS_H

/***********************************************************************/
/***			Include Files			    ***/
/***********************************************************************/

/***********************************************************************/
/***			Data Types and Constants		 ***/
/***********************************************************************/
#define D0_ADDR_SIZE    6
#define D0_MCAST_BIT    0x1
#define D0_LOCAL_BIT    0x2

typedef PACKED struct D0_ADDR_S {
	PACKED union {
		uint8 Byte[D0_ADDR_SIZE];
		uint16 hWord[D0_ADDR_SIZE / 2];
	} GCC_PACKED u;
} GCC_PACKED D0_ADDR;

#define D0_ADDR_EQ(_a, _b) \
	((*(uint16*)(_a)		     == *(uint16*)(_b)) && \
	(*(((uint16*)(_a))+1) == *(((uint16*)(_b))+1)) && \
	(*(((uint16*)(_a))+2) == *(((uint16*)(_b))+2)))

#define D0_ADDR_NEQ(_a, _b) \
	((*(uint16*)(_a)       != *(uint16*)(_b)) || \
	(*(((uint16*)(_a))+1) != *(((uint16*)(_b))+1)) || \
	(*(((uint16*)(_a))+2) != *(((uint16*)(_b))+2)))

#define D0_IS_BROADCAST(_a) \
	((*(uint16*)(_a)       == 0xFFFF) && \
	(*(((uint16*)(_a))+1) == 0xFFFF) && \
	(*(((uint16*)(_a))+2) == 0xFFFF))

#define D0_IS_MULTICAST(_a) \
	(*((uint8*)(_a)) & ((uint8)0x01))

#define D0_ADDR_COPY(_a, _b) \
	{*((uint16*)(_a)) = *((uint16*)(_b)); \
	*(((uint16*)(_a))+1) = *(((uint16*)(_b))+1); \
	*(((uint16*)(_a))+2) = *(((uint16*)(_b))+2); }

#define D0_SET_BROADCAST(_a) \
	{(*(uint16*)(_a) = 0xFFFF);\
	(*(((uint16*)(_a))+1) = 0xFFFF); \
	(*(((uint16*)(_a))+2) = 0xFFFF); }

#endif				/* _D0_DEFS_H */
