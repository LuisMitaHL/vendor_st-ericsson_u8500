/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
* \file
* \ingroup GLOBAL
*
* \brief  Baisc 802.11 Defintions, shared between the host and the device
*/

#ifndef D11_BASIC_DEFS_H
#define D11_BASIC_DEFS_H

/***********************************************************************/
/***			Data Types and Constants		 ***/
/***********************************************************************/

#ifndef D0_ADDR_SIZE_S

/* D0_ADDR_SIZE and D0_ADDR is also defined in d0_defs.h */

#define D0_ADDR_SIZE_S    6

/* No need to do a packed structure, it is the parent structure that needs to pack
   if it does not define a uint16 after D0_ADDR.
*/
typedef uint16 D0_ADDRESS[D0_ADDR_SIZE_S / 2];

#endif

#define D11_MAX_SSID_LEN			32

#define D11_PHY_NUM_5G_UNII_LOW_CHANNELS	8
#define D11_PHY_NUM_5G_EURO_CHANNELS	    12
#define D11_PHY_NUM_5G_UNII_HIGH_CHANNELS       4
#define D11_PHY_NUM_5G_JAP_CHANNELS	     11

#define D11_PHY_NUM_2G_CHANNELS		 14
#define D11_PHY_NUM_5G_CHANNELS		 (D11_PHY_NUM_5G_UNII_LOW_CHANNELS+\
						D11_PHY_NUM_5G_EURO_CHANNELS+\
						D11_PHY_NUM_5G_UNII_HIGH_CHANNELS+\
						D11_PHY_NUM_5G_JAP_CHANNELS)

#define D11_PHY_NUM_CHANNELS		    (D11_PHY_NUM_2G_CHANNELS+D11_PHY_NUM_5G_CHANNELS)

#define D11_PHY_2G_CHANNELS		(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14)
#define D11_PHY_5G_UNII_LOW_CHANNELS	(36, 40, 44, 48, 52, 56, 60, 64)
#define D11_PHY_5G_EURO_CHANNELS	(100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144)
#define D11_PHY_5G_UNII_HIGH_CHANNELS	(149, 153, 157, 161)
#define D11_PHY_5G_JAP_CHANNELS		(8, 12, 16, 34, 38, 42, 46, 184, 188, 192, 196)

/* These PHY mode constants are STMicro defined constants*/

#define PHY_BAND_2G			     0x00
#define PHY_BAND_5G			     0x01
#define PHY_BAND_MASK			   0x01

#endif
