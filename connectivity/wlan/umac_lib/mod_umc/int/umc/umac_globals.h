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
 * \file umac_globals.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_globals.h
 * \brief
 * UMAC globals are defined here.
 * \ingroup Upper_MAC_Core
 * \date 17/01/09 15:48
 */

#ifndef _UMAC_GLOBALS_H
#define _UMAC_GLOBALS_H

/******************************************************************************
			    INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "os_if.h"
#include "sysconfig.h"
#include "umac_dev_conf.h"
#include "umac_dev_list.h"
#include "umac_internal.h"
#include "umac_dbg.h"
#include "umac_2_ul.h"
#include "umac_task.h"
#include "umac_mem.h"
#include "umac_ll_if.h"
#include "umac_if.h"

#if P2P_MINIAP_SUPPORT
#define UMAC_CONFIG_COUNTRY_REGULATORY_TABLE_SIZE 12	/* bytes */
#endif	/* P2P_MINIAP_SUPPORT */

#define WFM_CONFIG_COUNTRY_CODE_LENGTH		3
#define WFM_CONFIG_MAX_COUNTRY			128
#define WFM_CONFIG_COUNTRY_REGION_TABLE_SIZE	512	/*bytes */
#define WFM_MAX_TRIPLET_COUNT			15
#define WFM_REGION_CONFIG_TABLE_SIZE		255
#define WFM_CHANNEL_SUPPORT_LIST_SIZE		64
#define WFM_MAX_NUM_OF_REGION			11

#if DOT11K_SUPPORT
#define UMAC_DEAFULT_RC_TO_POWERLEVEL_TABLE_SIZE	255
#endif	/* DOT11K_SUPPORT */

typedef struct _CountryStr_RegionCode_Mapping {
	uint8 CountryStr[WFM_CONFIG_COUNTRY_CODE_LENGTH];
	uint8 RegionCode;
} CountryStr_RegionCode_Mapping;

static const uint16 Arr_Channel_Offset[9] = { 0x000, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800 };

/** @brief Mapping between Region String and Region Code */
/*
 *  {"US ", 0x10}  US FCC
 *  {"CA ", 0x20}  IC Canada
 *  {"EU ", 0x30}  ETSI
 *  {"ES ", 0x31}  Spain
 *  {"FR ", 0x32}  France
 *  {"JP ", 0x40}  Japan
 *  {"CN ", 0x50}  China
 *  {"IN ", 0x00}  India
 *  {"AX ", 0x30}  Aland Islands
 *  {"AL ", 0x30}  Albania, Republic of
 *  {"AD ", 0x30}  Andorra, Principality of
 *  {"AT ", 0x30}  Austria, Republic of
 *  {"BY ", 0x30}  Belarus
 *  {"BE ", 0x30}  Belgium, Kingdom of
 *  {"BA ", 0x30}  Bosnia and Herzegovina
 *  {"BG ", 0x30}  Bulgaria, People's Republic of
 *  {"HR ", 0x30}  Croatia, Republic of
 *  {"CZ ", 0x30}  Czech Republic
 *  {"DK ", 0x30}  Denmark, Kingdom of
 *  {"EE ", 0x30}  Estonia
 *  {"FO ", 0x30}  Faroe Islands
 *  {"FI ", 0x30}  Finland, Republic of
 *  {"FR ", 0x32}  France, French Republic
 *  {"DE ", 0x30}  Germany
 *  {"GI ", 0x30}  Gibraltar
 *  {"GR ", 0x30}  Greece, Hellenic Republic
 *  {"GG ", 0x30}  Guernsey, Bailiwick of
 *  {"VA ", 0x30}  Holy See (Vatican City State)
 *  {"HU ", 0x30}  Hungary, Hungarian People's Republic
 *  {"IS ", 0x30}  Iceland, Republic of
 *  {"IE ", 0x30}  Ireland
 *  {"IM ", 0x30}  Isle of Man
 *  {"IT ", 0x30}  Italy, Italian Republic
 *  {"JE ", 0x30}  Jersey, Bailiwick of
 *  {"LV ", 0x30}  Latvia
 *  {"LI ", 0x30}  Liechtenstein, Principality of
 *  {"LT ", 0x30}  Lithuania
 *  {"LU ", 0x30}  Luxembourg, Grand Duchy of
 *  {"MK ", 0x30}  Macedonia, the former Yugoslav Republic of
 *  {"MT ", 0x30}  Malta, Republic of
 *  {"MD ", 0x30}  Moldova, Republic of
 *  {"MC ", 0x30}  Monaco, Principality of
 *  {"ME ", 0x30}  Montenegro, Republic of
 *  {"NL ", 0x30}  Netherlands, Kingdom of the
 *  {"NO ", 0x30}  Norway, Kingdom of
 *  {"PL ", 0x30}  Poland, Polish People's Republic
 *  {"PT ", 0x30}  Portugal, Portuguese Republic
 *  {"RO ", 0x30}  Romania, Socialist Republic of
 *  {"RU ", 0x30}  Russian Federation
 *  {"SM ", 0x30}  San Marino, Republic of
 *  {"RS ", 0x30}  Serbia, Republic of
 *  {"SK ", 0x30}  Slovakia (Slovak Republic)
 *  {"SI ", 0x30}  Slovenia
 *  {"ES ", 0x31}  Spain, Spanish State
 *  {"SJ ", 0x30}  Svalbard & Jan Mayen Islands
 *  {"SE ", 0x30}  Sweden, Kingdom of
 *  {"CH ", 0x30}  Switzerland, Swiss Confederation
 *  {"UA ", 0x30}  Ukraine
 *  {"GB ", 0x30}  United Kingdom of Great Britain & N. Ireland
 */

static const uint8 DefaultCountryRegionTableBuffer[WFM_CONFIG_COUNTRY_REGION_TABLE_SIZE] = {
	0x55, 0x53, 0x20, 0x10, 0x43, 0x41, 0x20, 0x20, 0x45, 0x55, 0x20, 0x30, 0x45, 0x53, 0x20,
	0x31, 0x46, 0x52, 0x20, 0x32, 0x4a, 0x50, 0x20, 0x40, 0x43, 0x4e, 0x20, 0x50, 0x49, 0x4E,
	0x20, 0x00, 0x41, 0x58, 0x20, 0x30, 0x41, 0x4c, 0x20, 0x30, 0x41, 0x44, 0x20, 0x30, 0x41,
	0x54, 0x20, 0x30, 0x42, 0x59, 0x20, 0x30, 0x42, 0x45, 0x20, 0x30, 0x42, 0x41, 0x20, 0x30,
	0x42, 0x47, 0x20, 0x30, 0x48, 0x52, 0x20, 0x30, 0x43, 0x5a, 0x20, 0x30, 0x44, 0x4b, 0x20,
	0x30, 0x45, 0x45, 0x20, 0x30, 0x46, 0x4f, 0x20, 0x30, 0x46, 0x49, 0x20, 0x30, 0x46, 0x52,
	0x20, 0x30, 0x44, 0x45, 0x20, 0x30, 0x47, 0x49, 0x20, 0x30, 0x47, 0x52, 0x20, 0x30, 0x47,
	0x47, 0x20, 0x30, 0x56, 0x41, 0x20, 0x30, 0x48, 0x55, 0x20, 0x30, 0x49, 0x53, 0x20, 0x30,
	0x49, 0x45, 0x20, 0x30, 0x49, 0x4d, 0x20, 0x30, 0x49, 0x54, 0x20, 0x30, 0x4a, 0x45, 0x20,
	0x30, 0x4c, 0x56, 0x20, 0x30, 0x4c, 0x49, 0x20, 0x30, 0x4c, 0x54, 0x20, 0x30, 0x4c, 0x55,
	0x20, 0x30, 0x4d, 0x4b, 0x20, 0x30, 0x4d, 0x54, 0x20, 0x30, 0x4d, 0x44, 0x20, 0x30, 0x4d,
	0x43, 0x20, 0x30, 0x4d, 0x45, 0x20, 0x30, 0x4e, 0x4c, 0x20, 0x30, 0x4e, 0x4f, 0x20, 0x30,
	0x50, 0x4c, 0x20, 0x30, 0x50, 0x54, 0x20, 0x30, 0x52, 0x4f, 0x20, 0x30, 0x52, 0x55, 0x20,
	0x30, 0x53, 0x4d, 0x20, 0x30, 0x52, 0x53, 0x20, 0x30, 0x53, 0x4b, 0x20, 0x30, 0x53, 0x49,
	0x20, 0x30, 0x45, 0x53, 0x20, 0x30, 0x53, 0x4a, 0x20, 0x30, 0x53, 0x45, 0x20, 0x30, 0x43,
	0x48, 0x20, 0x30, 0x55, 0x41, 0x20, 0x30, 0x47, 0x42, 0x20, 0x30,
	/* Margin */
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static const uint8 DefaultConfigRegionTable_2_4G[WFM_REGION_CONFIG_TABLE_SIZE] = {
	/*CountryCode,NoOfTriplets,First Channel,NoOfchannels,Tx PowerChannel Offset Index */
	16, 1, 1, 11, 20, 0,
	32, 1, 1, 11, 20, 0,
	48, 1, 1, 13, 20, 0,
	49, 1, 10, 2, 20, 0,
	50, 1, 10, 4, 20, 0,
	64, 1, 1, 14, 16, 0,
	80, 1, 1, 13, 20, 0,
	0, 1, 1, 13, 20, 0,
	240, 1, 3, 8, 20, 0,
	241, 1, 10, 2, 20, 0,
	242, 1, 10, 2, 10, 0,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255
};

static const uint8 DefaultConfigRegionTable_5G[WFM_REGION_CONFIG_TABLE_SIZE] = {
	/*CountryCode,NoOfTriplets,First Channel,NoOfchannels,Tx Power,Channel Offset Index */
	 /*US*/ 16, 5,
	36, 4, 17, 0,
	52, 4, 24, 0,
	149, 4, 30, 0,
	100, 11, 24, 0,
	165, 1, 30, 0,
	/*Europe */
	48, 2,
	36, 8, 24, 0,
	100, 11, 30, 0,
	/*Japan */
	64, 3,
	34, 4, 22, 0,
	8, 3, 24, 0,
	184, 4, 24, 2,
	/*Spain */
	49, 2,
	36, 8, 23, 0,
	100, 11, 30, 0,
	/*Canada */
	32, 3,
	36, 4, 23, 0,
	52, 4, 24, 0,
	149, 5, 30, 0,
	/*France */
	50, 1,
	36, 8, 23, 0,
	/*China */
	80, 1,
	149, 5, 27, 0,
	/*India */
	0, 2,
	36, 8, 23, 0,
	149, 5, 30, 0,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

static const uint8 ChannelSupportList[WFM_CHANNEL_SUPPORT_LIST_SIZE] = {
	/*Format Channel Index,Channel Number,Channel Support */
	6, 1, 6, 2, 6, 3, 6, 4, 6, 5, 6, 6, 6, 7, 6, 8, 6, 9, 6, 10,
	5, 11, 5, 13, 5, 15, 5, 17, 5, 19,
	8, 6, 8, 7, 8, 8, 8, 9, 8, 10, 8, 11,
	7, 182, 7, 183, 7, 184, 7, 185, 7, 186, 7, 187, 7, 188, 7, 189
};

#if P2P_MINIAP_SUPPORT
/** @brief Mapping between Region String and regulatory class */
/*
 * {"EU ", 0x04}  Europe
   {"US ", 0x0B}   United States
   {"JP ", 0x1E}   Japan
*/
static const uint8 DefaultCountryRegulatoryTableBuffer[UMAC_CONFIG_COUNTRY_REGULATORY_TABLE_SIZE] = {
	0x45, 0x55, 0x20, 0x04, 0x55, 0x53, 0x20, 0x0B, 0x4a, 0x50, 0x20, 0x1E
};

typedef struct COUNTRY_REGCLASS_MAPPING_S {
	uint8 ctryStr[WFM_CONFIG_COUNTRY_CODE_LENGTH];
	uint8 RegClass;
} COUNTRY_REGCLASS_MAPPING;
#endif	/* P2P_MINIAP_SUPPORT */

typedef struct CHANNEL_SUPPORT_LIST_S {
	uint8 channel_index;
	uint16 channel_no;
	uint8 IsChannelSupported;
} CHANNEL_SUPPORT_LIST;

static const UMAC_REGULATORY_DOMAIN_TABLE DefaultRegulatoryDomainTable = {

	/*Country Regulatory Details */
	/*US*/ {
		  {"US "}, 12,
		  {
		   {160, 0, 1, 1, {{36, 4, 4} } },
		   {230, 0, 2, 1, {{52, 4, 4} } },
		   {290, 0, 3, 1, {{149, 4, 4} } },
		   {230, 0, 4, 1, {{100, 11, 4} } },
		   {300, 0, 5, 1, {{149, 5, 4} } },	/*Taken from 11k standard */
		   {139, 0x0600, 6, 1, {{1, 10, 1} } },
		   {269, 0x0600, 7, 1, {{1, 10, 1} } },
		   {169, 0x0500, 8, 1, {{11, 5, 2} } },
		   {300, 0x0500, 9, 1, {{11, 5, 2} } },
		   {200, 0x0400, 10, 1, {{21, 2, 4} } },
		   {330, 0x0400, 11, 1, {{21, 2, 4} } },
		   {300, 0, 12, 1, {{1, 11, 1} } }
		   }
		  }
};

#if DOT11K_SUPPORT
static const uint16 DefaultRCToPowerLevelTable[UMAC_DEAFULT_RC_TO_POWERLEVEL_TABLE_SIZE] = {
	/*Format Regulatory class and Tx Power level in dBM * WFM_UMAC_POWER_LEVEL_FACTOR */
	5, 300,			/*US + 5Ghz */
	12, 300,		/*US + 2.4 Ghz */
	4, 200,			/*Europe + 2.4 Ghz */
	30, 136,		/*Japan + 2.407 Ghz */
	31, 136,		/*Japan + 2.414 Ghz */
	32, 124,		/*Japan + 5Ghz */
	0, 128,			/*Default Power WFM_UMAC_DEFAULT_POWER_LEVEL */

	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255
};

#endif	/* DOT11K_SUPPORT */
/*
  This structure holds the Tx packet's reference number along with
  TxDescriptor, so that UMAC can identify the packet corresponding
  to a particular Tx_Complete().This helps UMAC to free TxDescpritors
  and also to form a proper HiTxCnf message for uploading to HOST.
*/

typedef struct UMAC_TX_PKT_S {
	UMAC_TX_DESC *pTxDesc;
	uint32 reference;
	void *pMsgPtr;
	void *pDevIfTxReq;
	uint8 linkId;
	uint8 reserved[3];
} UMAC_TX_PKT;

typedef enum UMAC_FORCE_START_MODE_E {
	UMAC_FORCE_START_OFF = 0,
	UMAC_FORCE_START_ON
} UMAC_FORCE_START_MODE;

typedef enum UMAC_FORCE_STOP_MODE_E {
	UMAC_FORCE_STOP_OFF = 0,
	UMAC_FORCE_STOP_ON,
	UMAC_FORCE_STOP_RESET
} UMAC_FORCE_STOP_MODE;

typedef struct WFM_COUNTRY_STRING_INFO_S {
	uint8 countryString[WFM_DOT11_COUNTRY_STRING_SIZE];
	uint8 totalNumOfCh;
	uint16 channels[24];
	sint32 powerLevel[24];
} WFM_COUNTRY_STRING_INFO;

typedef struct WFM_UMAC_ENC_KEY_S {
	WFM_OID_802_11_KEY UmacDot11AddKey;
	uint8 linkId;
	uint8 InUse;
} WFM_UMAC_ENC_KEY;

typedef struct UMAC_INFO_SET_FROM_HOST_S {
	/* Authentication mode set by host */
	uint32 AuthenticationModeHost;
	/*Encryption Status set by host */
	uint32 EncryptionStatusHost;
	/*Encryption Type */
	uint16 EncryptionType;
	/* Authentication mode = shared/open */
	uint8 AuthenticationMode;
	uint8 WepKeyMap;
	uint8 GroupKeyInstalled;
	uint8 WepDefaultKeyId;
	uint8 EncKeyAvailable[MAX_NUM_CLIENTS_SUPPORTED];
	uint8 WepPairwiseKey[MAX_NUM_CLIENTS_SUPPORTED];
	uint8 Reserved1;
	/* Info about keys programmed into WSM */
	WFM_UMAC_ENC_KEY EncKey[MAX_KEY_ENTRIES];
	/* UMI_DEVICE_OID_802_11_DISABLE_BG_SCAN */
	uint32 disableBGScan;
	uint8 ScanPhyBand;
	uint8 countryStringAvl;
	uint8 CountryInfoAvl[2];
	/* To Store Country String Information */
	WFM_COUNTRY_STRING_INFO CountryStringInfo[2];
#if DOT11K_SUPPORT
	uint8 MeasurementCapSupported;
	uint8 Reserved[3];
#endif	/* DOT11K_SUPPORT */
} UMAC_INFO_SET_FROM_HOST;

typedef struct UMAC_TIMERS_S {
	/*
	  Pointers used to hold timer objects , UMAC will be using these
	  timer pointers for operation rather than using the timer objects
	 */
	TIMER_HANDLE *pUMACTimer;
	TIMER_HANDLE *pRateAdapTimer;
	TIMER_HANDLE *pUnJoinTimer;
	TIMER_HANDLE *pUnExtendedJoinTimer;
	TIMER_HANDLE *pCheckForConnectionTimer;
	TIMER_HANDLE *pBssLostTimer;
	TIMER_HANDLE *pCancelRemainOnChTimer;

#if DOT11K_SUPPORT
	TIMER_HANDLE *pMeasPauseTimer;
	TIMER_HANDLE *pTxStreamTimer;
#endif	/* DOT11K_SUPPORT */

#if MGMT_FRAME_PROTECTION
	TIMER_HANDLE *pSaQueryRetryTimer;
	TIMER_HANDLE *ReAssocTimer;
#endif	/* MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
	TIMER_HANDLE *pUmacClientConnectionTimer[MAX_NUM_CLIENTS_SUPPORTED];
	TIMER_HANDLE *pUmacGroupFormationTimer;
	TIMER_HANDLE *pUmacInvitationTimer;
	TIMER_HANDLE *pUMACInactivityTimer[MAX_NUM_CLIENTS_SUPPORTED];
	TIMER_HANDLE *pUMACSTAUnJoinTimer[MAX_NUM_CLIENTS_SUPPORTED];
	TIMER_HANDLE *pUMACCleanupTimer;
#endif	/* P2P_MINIAP_SUPPORT */
} UMAC_TIMERS;

typedef struct TX_POWER_RANGE_S {
	sint32 minPowerLevel;
	sint32 maxPowerLevel;
} TX_POWER_RANGE;

#if FT_SUPPORT
typedef enum UMAC_FT_AP_STATE_E {
	UMAC_FT_NONE,
	UMAC_FT_INIT,
	UMAC_FT_AUTHENTICATING_DS,
	UMAC_FT_AUTHENTICATING_AIR,
	UMAC_FT_AUTHENTICATED
} UMAC_FT_AP_STATE;

#define MAX_FT_AP	   4

typedef struct UMAC_FT_AP_DESC_S {
	UMAC_FT_AP_STATE State;
	uint8 Rsvd1;
	uint8 Bssid[6];
} UMAC_FT_AP_DESC;
#endif	/* FT_SUPPORT */

#if P2P_MINIAP_SUPPORT
typedef struct UMAC_MGMT_FRM_FILTER_S {
	uint8 NumFilter;
	uint16 TypeSubType;
} UMAC_MGMT_FRM_FILTER;
#endif
typedef struct UMAC_PERMENENT_DATA_S {
	LL_INFO LlInfo;
	void *ULHandle;
	UMAC_2UL_CB UpperLayerCb;
	void *ulHandle;
	UMAC_2UL_SCHEDULE_CB ScheduleCb;
	void *ScheduleParam;
	UMAC_2UL_RX_FRM_CB UpperLayerRxFrmCb;

#if DOT11K_SUPPORT
	UMAC_2UL_BEACON_INFO_CB UpperLayerBeaconInfoCb;
#endif	/* DOT11K_SUPPORT */

	UMAC_2UL_TX_CNF_CB UpperLayerTxcnf;
	UMAC_2UL_TX_MGMT_CNF_CB UpperLayerTxMgmtcnf;
	UMAC_TX_BUFF_POOL TxBuffPool;
	UMAC_IND_BUFF_POOL IndBuffPool;
	uint32 UmacAsyncMgmtReqStatus;
	/*
	  Asynchronous Management Request status
	  is stored in this variable
	*/
	SMGMT_REQ_PARAMS SerialMgmtInfo;

	OS_LOCK_TYPE TxDescStoreLock;
	OS_LOCK_TYPE PacketIdPoolLock;

#if P2P_MINIAP_SUPPORT
	/* Used in AP mode */
	OS_LOCK_TYPE ApPsTxBuffPoolLock[MAX_NUM_CLIENTS_SUPPORTED];
	OS_LOCK_TYPE ApPacketIdPoolLock;
	OS_LOCK_TYPE StaDataBase;
#endif	/* P2P_MINIAP_SUPPORT */

	OS_LOCK_TYPE TxBufCountLock;
	OS_LOCK_TYPE RxBufCountLock;
	OS_LOCK_TYPE TxBuffRequeuePoolLock;
	OS_LOCK_TYPE UpdateTimIeLock;
	OS_LOCK_TYPE UpdateERPIeLock;

	/*
	  Flag which indicates whether UMAC is forced to be initialized
	  by an external module
	*/
	uint16 ForceStart;

	/*
	   Following is a flag to indicate whether umac is forced by
	   upper layer to stop.  UMAC_FORCE_STOP_MODE
	*/

	uint16 ForceStopMode;

	/*
	   Pointers used to hold timer objects , UMAC will be using these
	   timer pointers for operation rather than using the timer objects
	*/
	UMAC_TIMERS UmacTimers;
	UMAC_TASK_CONTROL_BLOCK tcb;
	uint32 OprPwrMode;
	uint32 dot11RTSThreshold;
	UMAC_POWER_MODE PsMode;
	uint32 hostMsgTypeFilter;
	WFM_OID_802_11_SET_UAPSD setUapsdInfo;
	UMAC_INFO_SET_FROM_HOST InfoHost;
	TX_POWER_RANGE TxPowerRange[2];
	MAC_ADDR BlackList[NUM_BLACK_LIST_ENTRIES];
	WFM_RCPI_RSSI_THRESHOLD RcpiRssiThreshold;
	uint8 RcpiRssiThresholdRcvd;
	uint8 CountryStrReqFromHost;
	uint8 Enable11dFeature;
	uint8 stopAPFlag;
	uint8 sendStopAPEvt;
	uint8 Reserved;
	uint16 txopLimit;
	WFM_BEACON_LOST_COUNT BeaconLostCount;
	WFM_TX_FAILURE_THRESHOLD_COUNT TxFailureCount;
	WFM_BSSID_PMKID_BKID BssidPmkidBkid;
	uint32 maxRxAmsduSize;

#if FT_SUPPORT
	uint8 mdie[5];
	uint8 resvd[3];
	UMAC_FT_AP_DESC FtAuthenticatedAPs[4];
#endif	/* FT_SUPPORT */

	UMAC_PTA_LISTEN_INTERVAL PtaListenInterval;

#if DOT11K_SUPPORT
	uint16 measurementCapability;
	uint16 enableMeasurements;
	UMAC_802_11_LCI_INFORMATION LciInfo;
	uint8 lciInfoAvl;
	uint8 Reserved1[3];
#endif	/* DOT11K_SUPPORT */

	UMAC_REGULATORY_DOMAIN_TABLE RegDomainTable;
	uint8 BtActive;
	uint8 DisableActiveJoin;
#if P2P_MINIAP_SUPPORT
	uint8 IntraBssBridging;
	uint8 useP2P;
	UMAC_P2P_PS_MODE p2pPsMode;
#else
	uint8 Reserved2[2];
#endif
	uint32 UseMultiTxCnfMsg;

#if P2P_MINIAP_SUPPORT
	uint8 tieBrkr;
	uint16 maxNumClientSupport;
	uint16 FirmwareClientSupport;
	uint8 reserved3[3];
	uint8 userallowaccess;
#endif	/* P2P_MINIAP_SUPPORT */

	UMAC_KEEP_ALIVE_PERIOD keepAlivePeriod;
	uint8 HiddenAPFlag;
} UMAC_PERMENENT_DATA;

/*
  The structure below holds the global variables umac uses
*/
typedef struct UMAC_GLOBALS_S {
	UMAC_PERMENENT_DATA p;
	/*
	   These variables stores the Tx buffer pointer to keep track of the
	   buffers allocated by UMAC for Scan and Join
	 */
	UMAC_TX_DESC *pScanTxDesc;
	UMAC_TX_DESC *pJoinTxDesc;

#if DOT11K_SUPPORT
	UMAC_TX_DESC *pMeasurementTxDesc;
#endif	/* DOT11K_SUPPORT */

#if P2P_MINIAP_SUPPORT
	UMAC_TX_DESC *pStartReq;
	UMAC_TX_DESC *pBcnTxReq;
	UMAC_TX_DESC *pMapLinkReq;
	UMAC_TX_DESC *pUpdateIeReq;
	UMAC_TX_DESC *pUpdateErpIeReq;
#endif	/* P2P_MINIAP_SUPPORT */

	WFM_WMM_PARAM_ELEMENT WmmParamElement;
	WFM_WPA_PARAM_ELEMENT WpaParamElement;
	WFM_WPA2_PARAM_ELEMENT Wpa2ParamElement;

#if WAPI_SUPPORT
	WFM_WAPI_PARAM_ELEMENT WapiParamElement;
#endif	/* WAPI_SUPPORT */

	/*we are keeping a pool of Id's that can be re-used */
	uint32 PacketIdPool[CFG_HI_NUM_REQS];
	uint32 PktIdGet;
	uint32 PktIdPut;

#if P2P_MINIAP_SUPPORT
	/*Pool of Id's which is used in frame forwarding in AP mode */
	uint32 ApPacketIdPool[UMAC_NUM_AP_INTERNAL_TX_BUFFERS];
	uint32 ApPktIdGet;
	uint32 ApPktIdPut;
	UMAC_TX_PKT_INT ApUmacTxDesc[UMAC_NUM_AP_INTERNAL_TX_BUFFERS];
#endif	/* P2P_MINIAP_SUPPORT */
	/*
	   We should keep CFG_HI_NUM_REQS number of Tx Descriptors, as thats the
	   maximum numner of packets LMAC can keep in its queue.
	 */

	UMAC_TX_PKT TxDescStore[CFG_HI_NUM_REQS];
	UMAC_TX_PKT_INT umacTxDescInt[UMAC_NUM_INTERNAL_TX_BUFFERS];

	/*WFM_UMAC_2_HOST */
	UMAC_EVT_PENDING event_queue;

	/*WFM_UMAC_DEV_LIST */
	/*The device Queue */
	WFM_DEV_LIST DeviceQueue;
	uint8 *pWfmDevEntry;
#if FT_SUPPORT
	uint16 ftie_len;
	uint8 ftie[258];
#endif	/* FT_SUPPORT */
} UMAC_GLOBALS;

#endif	/* _UMAC_GLOBALS_H */
