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
 * \file UMI_Utility.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_Utility.h
 * \brief
 * This file contains some general utility function used by the UMI
 * \ingroup Upper_MAC_Interface
 * \date 29/01/09 21:45
 */

#ifndef _UMI_UTIL_H
#define _UMI_UTIL_H

#include "umac_messages.h"
#include "UMI_Api.h"

#define IE_SSID_ID			    0
#define IE_SUPPORTED_RATES_ID		 1
#define IE_FH_PARAM_SET_ID		    2
#define IE_DS_PARAM_SET_ID		    3
#define IE_CD_PARAM_SET_ID		    4
#define IE_TIM_ID			     5
#define IE_IBSS_PARAM_SET_ID		  6
#define IE_COUNTRY_ID			 7
#define IE_HOP_PATERN_PARAM_ID		8
#define IE_HOP_PATERN_TABLE_ID		9
#define IE_REQUEST_ID			 10
#define IE_BSS_LOAD_ID			11
#define IE_ECDA_PARAM_SET_ID		  12
#define IE_TSPEC_ID			   13
#define IE_TCLAS_ID			   14
#define IE_SCHEDULE_ID			15
#define IE_CHALLENGE_TAXT_ID		  16
#define IE_POWER_CONSTRAINT_ID		32
#define IE_POWER_CAPABILITY_ID		33
#define IE_TPC_REQ_ID			 34
#define IE_TPC_REPORT_ID		      35
#define IE_SUPPORTED_CHANNELS_ID	      36
#define IE_CHANNEL_SWITCH_ANNOUNCEMENT_ID     37
#define IE_MEASUREMENT_REQUEST_ID	     38
#define IE_MEASUREMENT_REPORTT_ID	     39
#define IE_QUIET_ID			   40
#define IE_IBSS_DFS_ID			41
#define IE_ERP_INFO_ID			42
#define IE_TS_DELAY_ID			43
#define IE_TCLAS_PROCESSING_ID		44
#define IE_HT_CAPABILITIES_ID		 45
#define IE_QOS_CAPABILITY_ID		  46
#define IE_RSN_ID			     48
#define IE_EXTENDED_SUPPORTED_RATES_ID	50
#define IE_MOBILITY_DOMAIN_ID		 54
#define IE_FAST_BSS_TRANSITION_ID	     55
#define IE_TIMEOUT_INTERVAL_ID		56
#define IE_RIC_DATA_ID			57
#define IE_HT_INFORMATION_ID		  61
#define IE_SECONDARY_CHANNEL_OFFSET_ID	62
#define IE_WAPI_ID			    68

#if DOT11K_SUPPORT
#define IE_RRM_ENABLED_CAPABILITIES_ID	70
#endif	/* DOT11K_SUPPORT */

#define IE_EXTENDED_CAPABILITIES_ID	   127
#define IE_VENDOR_SPECIFIC_ID		 221

#if P2P_MINIAP_SUPPORT
#define IE_P2P_ATTRIB_NOA		12
#endif	/* P2P_MINIAP_SUPPORT */

#define WMM_OUI			       {0x00, 0x50, 0xF2}

#define WMM_IE_ELEMENT_TYPE		   0x02

/*WMM Sub-Types*/
#define UMI_IE_INFORMATION_ELEMENT_SUB_TYPE   0x00
#define UMI_IE_PARAMETER_ELEMENT_SUB_TYPE     0x01
#define UMI_IE_TSPEC_ELEMENT_SUB_TYPE	 0x02

#define WPA_OUI			       {0x00, 0x50, 0xF2}

/*WPA Sub-Types*/
#define WPA_IE_PARAMETER_ELEMENT_SUB_TYPE     0x01

#define UMI_CONVERT_FREQ_MHZ_TO_CHANNEL_NO(Freq)  ((Freq - 2407)/5)

#define UMI_CONVERT_CHANNEL_NO_TO_FREQ_KHZ(ChannelNo) ((2407 + (ChannelNo * 5))*1000)

typedef struct IE_ELEMENT_S {
	uint8 id;
	uint8 size;
	uint8 *pInfo;
} IE_ELEMENT;

typedef struct P2P_IE_ELEMENT_S {
	uint8 id;
	uint16 size;
	uint8 *pInfo;
} P2P_IE_ELEMENT;

typedef struct UMI_OUI_INFO_S {
	uint8 oui[3];
	uint8 ouiType;
	uint8 ouiSubType;
} UMI_OUI_INFO;

/******************************************************************************
 * NAME:	UMI_GetBssInfo
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the UMI_BSS_CACHE_INFO_IND and convert it in to
 * UMI_BSS_INFO.
 * \param  umiHandle - UMI Handle.
 * \param  pUmiBssCacheInfoInd - The Beacon/Prob Response
 *				      packet.
 * \returns UMI_BSS_INFO*pointer to UMI_BSS_INFO structure.
 *****************************************************************************/
WFM_BSS_INFO *UMI_GetBssInfo(UMI_HANDLE umiHandle,
			     UMI_BSS_CACHE_INFO_IND *pUmiBssCacheInfoInd);

/******************************************************************************
 * NAME:	UMI_ProcIeElements
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given IE element from the given beacon/prob
 * response.
 * \param    pIeElementStart  -   Start of IE lements in the beacon or
 *			          probe response.
 * \param    ieLength	      -   Total length of the IE elements.
 * \param    elementToRetrive -   The IE element to be retrieved.
 * \param    pIeElement	      -   memory to which IE elements to be copied.
 * \returns none.
 *****************************************************************************/
void UMI_ProcIeElements(uint8 *pIeElementStart,
			uint16 ieLength,
			uint32 elementToRetrive,
			IE_ELEMENT *pIeElement);

/******************************************************************************
 * NAME:	UMI_ProcVendorSpecificIeElements
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given IE element corresponding to the specified
 * OUI information from the given beacon/prob response.
 * \param  pIeElementStart  -   Start of IE lements in the beacon or
 *			        probe response.
 * \param  ieLength	    -   Total length of the IE elements.
 * \param  pOuiInfo	    -   The OUI info. for the element to be retrived
 * \param  pIeElement	    -   Memory to which IE elements to be copied.
 * \returns none.
 *****************************************************************************/
void UMI_ProcVendorSpecificIeElements(uint8 *pIeElementStart,
				      uint16 ieLength,
				      UMI_OUI_INFO *pOuiInfo,
				      IE_ELEMENT *pIeElement);

/******************************************************************************
 * NAME:	UMI_ProcVendorSpecificIeElementsWPA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given IE element corresponding to the specified
 * OUI information from the given beacon/probe response for WPA.
 * \param  pIeElementStart  -   Start of IE lements in the beacon or
 *			        prob response.
 * \param  ieLength	    -   Total length of the IE elements.
 * \param  pOuiInfo	    -   The OUI info. for the element to be retrived
 * \param  pIeElement	    -   Memory to which IE elements to be copied.
 * \returns none.
 *****************************************************************************/
void UMI_ProcVendorSpecificIeElementsWPA(uint8 *pIeElementStart,
					 uint16 ieLength,
					 UMI_OUI_INFO *pOuiInfo,
					 IE_ELEMENT *pIeElement);

/******************************************************************************
 * NAME:	UMI_ProcVendorSpecificIeElementsP2P
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given IE element corresponding to the specified
 * OUI information from the given beacon/probe response for P2P.
 * \param  pIeElementStart	-   Start of IE lements in the beacon or
 *				    prob response.
 * \param  ieLength		-   Total length of the IE elements.
 * \param  p2pAttribID		-   The P2P Attribute ID to be retrived
 * \param  pIeElement		-   Memory to which IE elements to be copied.
 * \returns none.
 *****************************************************************************/
void UMI_ProcVendorSpecificIeElementsP2P(uint8 *pIeElementStart,
					 uint16 ieLength,
					 uint8 p2pAttribID,
					 P2P_IE_ELEMENT *pIeElement);
#endif	/* _UMI_UTIL_H */
