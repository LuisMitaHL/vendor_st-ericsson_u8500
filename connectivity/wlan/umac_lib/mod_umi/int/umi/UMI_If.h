/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup Upper_MAC_Interface
 * \brief
 *
 */
/**
 * \file UMI_If.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_If.h
 * \brief
 * Header for UMAC Interface module.
 * \ingroup Upper_MAC_Interface
 * \date 05/01/09 14:42
 */

#ifndef _WUM_IF_H
#define _WUM_IF_H

#include "WUM_Api.h"

#pragma pack(push, 4)

typedef struct WUM_802_3_FRAME_S	/* 4 byte aligned */
{
	uint8 DestinationAddress[WUM_MAC_ADDRESS_SIZE];
	uint8 SourceAddress[WUM_MAC_ADDRESS_SIZE];
	uint16 EthernetType;
	uint8 Data[WUM_MAX_ETHERNET_FRAME_SIZE];
	uint16 Padding;
} WUM_802_3_FRAME;		/* TBD : How to allign this? total size right now is 1514, it needs to be 1516
				   to be alligned for 32 bit boundary
				 */

#pragma pack(pop)		/* release pragma */

#endif
