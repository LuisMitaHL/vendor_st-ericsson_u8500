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
 * \file UMI_Utility.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_Utility.c
 * \brief
 * This file contains some general utility function used by the UMI
 * \ingroup Upper_MAC_Interface
 * \date 29/01/09 19:17
 */

/*****************************************************************************/
/********		       Include Files			      ********/
/*****************************************************************************/
#include "UMI_Utility.h"
#include "UMI_DebugTrace.h"
#include "umac_messages.h"
#include "UMI_ApiInternal.h"

/*****************************************************************************/
/********		   Internally Visible Functions		      ********/
/*****************************************************************************/

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
			     UMI_BSS_CACHE_INFO_IND *pUmiBssCacheInfoInd)
{
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;
	uint16 sizeOfBssElement = sizeof(WFM_BSS_INFO);
	IE_ELEMENT ieElement = { 0 };
	P2P_IE_ELEMENT p2pIeElement = {0};
	WFM_BSS_INFO *pBssElement = NULL;
	uint8 *pSupportedRates = NULL;
	uint16 numSupportedRates = 0;
	uint16 numExtSupportedRates = 0;
	UMI_BEACON_CAPABILITY umiBeaconCapability = { 0 };
	UMI_OUI_INFO OuiInfo = {
			WMM_OUI,
			WMM_IE_ELEMENT_TYPE,
			UMI_IE_PARAMETER_ELEMENT_SUB_TYPE
		};
	UMI_OUI_INFO OuiInfo1 = {
			WPA_OUI,
			0x01,
			WPA_IE_PARAMETER_ELEMENT_SUB_TYPE
		};

	UMI_TRACE(UMI_Utility, ("---> UMI_ProcessDeviceCacheInfo()\n"));

	if (pUmiBssCacheInfoInd == NULL) {
		UMI_ERROR(UMI_Utility, ("UMI_ProcessDeviceCacheInfo: Unexpected NULL pUmiBssCacheInfoInd\n"));
		goto Exit_Func;
	}

	UMI_ProcIeElements((uint8 *) &pUmiBssCacheInfoInd->ieElements[0], pUmiBssCacheInfoInd->ieLength, IE_SUPPORTED_RATES_ID, &ieElement);

	numSupportedRates = ieElement.size;

	if (numSupportedRates) {
		/*
		   Keeping the pointer to Supported rates, so that we can copy
		   it later when the  buffer is allocated
		 */
		pSupportedRates = ieElement.pInfo;

	}	/* if (numSupportedRates) */

	/*Getting the extended supported rates */
	UMI_ProcIeElements(
			(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
			pUmiBssCacheInfoInd->ieLength,
			IE_EXTENDED_SUPPORTED_RATES_ID,
			&ieElement
			);

	numExtSupportedRates = ieElement.size;

	sizeOfBssElement = (uint16) (sizeOfBssElement + numSupportedRates + numExtSupportedRates - WFM_BSS_INFO_SUPPORTED_RATE_SIZE);

	/*Calculating the length of supported rates for allocating
	memory for it */
	pBssElement = (WFM_BSS_INFO *) UMI_Allocate(sizeOfBssElement);

	if (!pBssElement) {
		UMI_ERROR(UMI_Utility, ("UMI_ProcessDeviceCacheInfo: No memory available [IE_EXTENDED_SUPPORTED_RATES_ID]"));
		goto Exit_Func;
	}

	UMI_MemoryReset(pBssElement, sizeOfBssElement);

	pBssElement->NumRates = numSupportedRates + numExtSupportedRates;

	if (numSupportedRates) {
		UMI_MemoryCopy(
			(void *)(uint8 *) &pBssElement->SupportedRates[0],
			pSupportedRates,
			numSupportedRates
			);
	}	/* if (pBssElement->NumRates) */

	if (numExtSupportedRates) {
		UMI_MemoryCopy(
			&pBssElement->SupportedRates[numSupportedRates],
			ieElement.pInfo,
			numExtSupportedRates
			);
	}	/* if (pBssElement->NumExtendedRates) */

	UMI_MemoryCopy(
		pBssElement->MacAddress,
		pUmiBssCacheInfoInd->bssId,
		UMI_MAC_ADDRESS_SIZE
		);

	pBssElement->RCPI = pUmiBssCacheInfoInd->rcpi;
	pBssElement->TSF = pUmiBssCacheInfoInd->timeStamp;
	pBssElement->Rssi = pUmiBssCacheInfoInd->rssi;
	pBssElement->BeaconPeriod = pUmiBssCacheInfoInd->beaconInterval;

	/*
	   If the beacon doesnt have a DS parameter Set IE element, use the
	   channel on which we recieved this beacon, as the channel of
	   operation of the AP.
	 */
	pBssElement->ChannelNumber = pUmiBssCacheInfoInd->channelNumber;

	/*
	   The channel number WFM firmware is sending up to the HOST is
	   in WSM encoded format. It has the band-channel spacing information
	   masked in it as per Section 2.7 of WSM API document specification.
	   Since the operating system / application above AFM API is not
	   interested in the WSM portion of channel number, we are masking
	   it off here. Please note that the WSM masks are in the MSB.
	 */

	pUmiBssCacheInfoInd->channelNumber &= 0x00FF;

	pBssElement->Capabilities = pUmiBssCacheInfoInd->capability;

	*((uint16 *) &umiBeaconCapability) = pUmiBssCacheInfoInd->capability;
	if ((umiBeaconCapability.ess == 1)
	    && (umiBeaconCapability.ibss == 0)
	    ) {
		/* This is an ACCESS POINT */
		pBssElement->InfrastructureMode = UMI_802_INFRASTRUCTURE;
	} else if ((umiBeaconCapability.ess == 0)
		   && (umiBeaconCapability.ibss == 1)
	    ) {
		/* This is an Adhoc network */
		pBssElement->InfrastructureMode = UMI_802_IBSS;
	} else {
		UMI_ERROR(UMI_Utility, ("UMI_ProcessDeviceCacheInfo: Unexpected WfmBeaconCapability\n"));
		pBssElement->InfrastructureMode = UMI_802_AUTO_UNKNOWN;
		goto Exit_Func;
	}

	UMI_ProcIeElements((uint8 *) &pUmiBssCacheInfoInd->ieElements[0], pUmiBssCacheInfoInd->ieLength, IE_SSID_ID, &ieElement);

	if (ieElement.size) {
		UMI_MemoryCopy(pBssElement->Ssid, ieElement.pInfo, ieElement.size);
		pBssElement->SsidLength = ieElement.size;
	}	/* if (ieElement.size) */

	pBssElement->Flags = 0;	/* initializing it */

	UMI_ProcIeElements((uint8 *) &pUmiBssCacheInfoInd->ieElements[0], pUmiBssCacheInfoInd->ieLength, IE_ERP_INFO_ID, &ieElement);

	if (ieElement.size) {
		/*
		   As per the current standard, ERP element has only one byte
		   information in it. The following code should be changed when
		   there is a change to the information element.
		 */
		uint8 erpInfo = (uint8) *ieElement.pInfo;

		if (erpInfo & 0x01) {
			/*Non-ERP bit set */
			pBssElement->Flags |= WFM_BSS_INFO_FLAGS_NON_ERP_AP;
		}	/* if (erpInfo & 0x01) */
		if (erpInfo & 0x02) {
			/*Protection enabled */
			pBssElement->Flags |= WFM_BSS_INFO_FLAGS_PROT_ENABLED;
		}	/*if (erpInfo & 0x02) */
		if (!(erpInfo & 0x04)) {
			/*Short Preamble supported */
			pBssElement->Flags |= WFM_BSS_INFO_FLAGS_SUPP_SHORT_PREAMBLE;
		}	/* if (!(erpInfo & 0x04)) */
	}	/* if (ieElement.size) */

	/*Process TIM information element from beacon IE */
	UMI_ProcIeElements(
		(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
		pUmiBssCacheInfoInd->ieLength,
		IE_TIM_ID,
		&ieElement
		);

	if (ieElement.size)
		pBssElement->DTIMPeriod = (uint8) ieElement.pInfo[1];

	UMI_ProcIeElements(
		(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
		pUmiBssCacheInfoInd->ieLength,
		IE_DS_PARAM_SET_ID,
		&ieElement
		);

	if (ieElement.size) {
		pBssElement->DSConfig = (uint8) (*ieElement.pInfo);

		/*
		   If the beacon has DS paramter set in it, extract channel
		   information from here and use it.
		 */
		pBssElement->ChannelNumber = pBssElement->DSConfig;

		pBssElement->DSConfig = UMI_CONVERT_CHANNEL_NO_TO_FREQ_KHZ(pBssElement->DSConfig);
	}	/* if (ieElement.size) */

	if (pUmiCon->UseP2P == UMI_P2P_DISABLE) {
		/*
		   Importent Note:
		   The following IE's  (HT Capabilities and HT Information ) needs to be handled
		   carefully. Each of the members needs to be copied explicitly as the structue
		   is slightly different from the spec; for handling byte padding.
		 */

		UMI_ProcIeElements((uint8 *) &pUmiBssCacheInfoInd->ieElements[0], pUmiBssCacheInfoInd->ieLength, IE_HT_CAPABILITIES_ID, &ieElement);

		if (ieElement.size) {
			/*
			   AP support 11n rates, so setting the flags for 11n mode
			   support and 11n rate support.
			 */

			/*Hard coding for the time being */
			pBssElement->Flags |= WFM_BSS_INFO_FLAGS_11N_CAPABLE | WFM_BSS_INFO_FLAGS_SUPP_11N_RATES;

			UMI_MemoryCopy(
				(void *)&pBssElement->HtCapabilities.CapabilitiesInfo,
				ieElement.pInfo,
				2
				);

			pBssElement->HtCapabilities.AMPDU_Params = *(ieElement.pInfo + 2);

			UMI_MemoryCopy(
				(void *)&pBssElement->HtCapabilities.SupportedMCSSet,
				(ieElement.pInfo + 3),
				16
				);

			UMI_MemoryCopy(
				(void *)&pBssElement->HtCapabilities.ExtendedCapabilities,
				(ieElement.pInfo + 18),
				2
				);

			UMI_MemoryCopy(
				(void *)&pBssElement->HtCapabilities.TxBeamFormingCapabilities,
				(ieElement.pInfo + 20),
				4
				);

			pBssElement->HtCapabilities.ASEL_Capabilities = (uint8) *(ieElement.pInfo + 24);

		}	/* if (ieElement.size) */

		UMI_ProcIeElements(
			(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
			pUmiBssCacheInfoInd->ieLength,
			IE_HT_INFORMATION_ID,
			&ieElement
			);

		if (ieElement.size) {
			pBssElement->HtOperationIe.PrimaryChannel = (uint8) *(ieElement.pInfo);

			UMI_MemoryCopy(
				(void *)&pBssElement->HtOperationIe.HTInfoParam1,
				(ieElement.pInfo + 1),
				sizeof(pBssElement->HtOperationIe.HTInfoParam1)
				);

			UMI_MemoryCopy(
				(void *)&pBssElement->HtOperationIe.HTInfoParam2,
				(ieElement.pInfo + 2),
				sizeof(pBssElement->HtOperationIe.HTInfoParam2)
				);

			UMI_MemoryCopy(
				(void *)&pBssElement->HtOperationIe.HTInfoParam3,
				(ieElement.pInfo + 4),
				sizeof(pBssElement->HtOperationIe.HTInfoParam3)
				);

			UMI_MemoryCopy(
				(void *)&pBssElement->HtOperationIe.BasicMCSSet,
				(ieElement.pInfo + 6),
				sizeof(pBssElement->HtOperationIe.BasicMCSSet)
				);

		}	/* if (ieElement.size) */
	}


	UMI_ProcVendorSpecificIeElements(
		(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
		pUmiBssCacheInfoInd->ieLength,
		&OuiInfo,
		&ieElement
		);

	/* Additional check for WMM */
	if (!ieElement.size) {
		OuiInfo.ouiSubType = 0x0;
		UMI_ProcVendorSpecificIeElements(
			(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
			pUmiBssCacheInfoInd->ieLength,
			&OuiInfo,
			&ieElement
			);
	}

	if (ieElement.size) {
		/*
		   This Device is WMM capable, If the Host is not interested in
		   WMM, Host should reset this bit.
		 */
		pBssElement->Flags |= WFM_BSS_INFO_FLAGS_BSS_WMM_CAP;

		UMI_MemoryCopy(
			(void *)&pBssElement->WmmParamElement,
			ieElement.pInfo,
			ieElement.size
			);

	}	/* if (ieElement.size) */


	/*
	   Both WPA IE and RSN IE wont be present together
	 */

	UMI_ProcIeElements(
		(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
		pUmiBssCacheInfoInd->ieLength,
		IE_RSN_ID,
		&ieElement
		);

	if (ieElement.size) {

		UMI_MemoryCopy(
			(void *)&pBssElement->Wpa2ParamElement.ParamInfo[0],
			ieElement.pInfo,
			ieElement.size
			);

		pBssElement->Wpa2ParamElement.Length = ieElement.size;

		pBssElement->Flags |= WFM_BSS_INFO_FLAGS_SUPP_WPA2;

	}	/* if (ieElement.size) */

	UMI_ProcVendorSpecificIeElementsWPA(
		(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
		pUmiBssCacheInfoInd->ieLength,
		&OuiInfo1,
		&ieElement
		);

	if (ieElement.size) {

		UMI_MemoryCopy(
			(void *)&pBssElement->WpaParamElement.ParamInfo[0],
			ieElement.pInfo,
			ieElement.size
			);

		pBssElement->WpaParamElement.Length = ieElement.size;

		pBssElement->Flags |= WFM_BSS_INFO_FLAGS_SUPP_WPA;
	}	/* if (ieElement.size) */

#if WAPI_SUPPORT
	UMI_ProcIeElements((uint8 *) &pUmiBssCacheInfoInd->ieElements[0], pUmiBssCacheInfoInd->ieLength, IE_WAPI_ID, &ieElement);

	if (ieElement.size) {

		UMI_MemoryCopy(
			(void *)&pBssElement->WapiParamElement.ParamInfo[0],
			ieElement.pInfo,
			ieElement.size
			);

		pBssElement->WapiParamElement.Length = ieElement.size;

		pBssElement->Flags |= WFM_BSS_INFO_FLAGS_SUPP_WAPI;

	}	/* if (ieElement.size) */
#endif	/* WAPI_SUPPORT */

#if FT_SUPPORT
	{
		/*
		 * Process FT specific IEs.
		 */
		UMI_ProcIeElements(
			(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
			pUmiBssCacheInfoInd->ieLength,
			IE_MOBILITY_DOMAIN_ID,
			&ieElement
			);

		if (ieElement.size == 3) {

			UMI_MemoryCopy(
				(void *)&pBssElement->MobilityDomainElement.mdid[0],
				ieElement.pInfo,
				ieElement.size
				);
			pBssElement->Flags |= WFM_BSS_INFO_FLAGS_SUPP_FT;

		}	/* if (ieElement.size == 3) */
	}
#endif	/* FT_SUPPORT */

#if DOT11K_SUPPORT
	UMI_ProcIeElements(
		(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
		pUmiBssCacheInfoInd->ieLength,
		IE_RRM_ENABLED_CAPABILITIES_ID,
		&ieElement
		);

	if (ieElement.size) {
		UMI_MemoryCopy(
			(void *)&pBssElement->RRMMeasurementCapabilities,
			ieElement.pInfo,
			ieElement.size
			);

		pBssElement->Flags |= WFM_BSS_INFO_FLAGS_RRM_MEASUREMENT_CAPABLE;
	}
#endif


#if P2P_MINIAP_SUPPORT
	UMI_ProcVendorSpecificIeElementsP2P(
		(uint8 *) &pUmiBssCacheInfoInd->ieElements[0],
		pUmiBssCacheInfoInd->ieLength,
		IE_P2P_ATTRIB_NOA,
		&p2pIeElement
		);

	if (p2pIeElement.size) {
		UMI_MemoryCopy(
			(void *)&pBssElement->noaAttrib.index,
			p2pIeElement.pInfo,
			p2pIeElement.size
			);
	}
#endif	/* P2P_MINIAP_SUPPORT */

 Exit_Func:;

	UMI_TRACE(UMI_Utility, ("<--- UMI_ProcessDeviceCacheInfo()\n"));

	return pBssElement;
}	/* End UMI_GetBssInfo() */

/******************************************************************************
 * NAME:	UMI_ProcIeElements
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given IE element from the given beacon/prob
 * response.
 * \param    pIeElementStart	-   Start of IE lements in the beacon or
 *				    probe response.
 * \param    ieLength		-   Total length of the IE elements.
 * \param    elementToRetrive   -   The IE element to be retrieved.
 * \param    pIeElement		-   memory to which IE elements to be copied.
 * \returns none.
 *****************************************************************************/
void UMI_ProcIeElements(IN uint8 *pIeElementStart,
			IN uint16 ieLength,
			IN uint32 elementToRetrive,
			IN_OUT IE_ELEMENT *pIeElement)
{

	/*IeLen has to be signed int, otherwise this code may fail */
	sint16 ieLen = ieLength;
	uint8 ieElementId = 0;
	uint8 ieElementSize = 0;

	UMI_DEFAULT(UMI_Utility, ("UMI_ProcIeElements(): UMI_ProcIeElements\n"));

	UMI_OS_ASSERT(pIeElementStart);

	UMI_MemoryReset(pIeElement, sizeof(IE_ELEMENT));

	do {

		ieElementId = (uint8) *pIeElementStart;
		ieElementSize = (uint8) *(pIeElementStart + 1);

		pIeElementStart = (pIeElementStart + ieElementSize + 2);
		ieLen = (ieLen - (ieElementSize + 2));

	} while ((elementToRetrive != ieElementId) && (ieLen > 0));

	if (elementToRetrive == ieElementId) {
		pIeElement->id = ieElementId;
		pIeElement->size = ieElementSize;
		pIeElement->pInfo = (uint8 *) (pIeElementStart - ieElementSize);
	}	/* if (elementToRetrive == IeElementId) */

}	/* End UMI_ProcIeElements() */

/******************************************************************************
 * NAME:	UMI_ProcVendorSpecificIeElements
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given IE element corresponding to the specified
 * OUI information from the given beacon/prob response.
 * \param  pIeElementStart     -   Start of IE lements in the beacon or
 *				 probe response.
 * \param  ieLength	    -   Total length of the IE elements.
 * \param  pOuiInfo	    -   The OUI info. for the element to be retrived
 * \param  pIeElement	  -   Memory to which IE elements to be copied.
 * \returns none.
 *****************************************************************************/
void UMI_ProcVendorSpecificIeElements(uint8 *pIeElementStart,
				      uint16 ieLength,
				      UMI_OUI_INFO *pOuiInfo,
				      IE_ELEMENT *pIeElement)
{

	int ieLen = ieLength;
	uint8 ieElementId = 0;
	uint8 ieElementSize = 0;

	UMI_TRACE(UMI_Utility, ("---> UMI_ProcVendorSpecificIeElements()\n"));

	if (pIeElementStart == NULL) {
		UMI_TRACE(UMI_Utility, ("UMI_ProcVendorSpecificIeElements: Unexpected NULL IeElementStart\n"));
		goto Exit_Func;
	}

	UMI_MemoryReset(pIeElement, sizeof(IE_ELEMENT));

	do {
		ieElementId = (uint8)*pIeElementStart;
		ieElementSize = (uint8) *(pIeElementStart + 1);

		if (ieElementId == IE_VENDOR_SPECIFIC_ID) {
			if (UMI_MemoryCompare((pIeElementStart + 2), 5, (uint8 *) pOuiInfo, 5)) {
				pIeElement->id = ieElementId;
				pIeElement->size = ieElementSize;
				pIeElement->pInfo = (uint8 *) (pIeElementStart + 2);

				/*Got what we were interested in,
				so break here */
				break;
			}
		}

		pIeElementStart = (pIeElementStart + ieElementSize + 2);
		ieLen = (ieLen - (ieElementSize + 2));

	} while ((ieLen > 0));

	UMI_TRACE(UMI_Utility, ("<--- UMI_ProcVendorSpecificIeElements()\n"));

 Exit_Func:;
	return;
}	/* End UMI_ProcVendorSpecificIeElements() */

/******************************************************************************
 * NAME:	UMI_ProcVendorSpecificIeElementsWPA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given IE element corresponding to the specified
 * OUI information from the given beacon/probe response for WPA.
 * \param  pIeElementStart	-   Start of IE lements in the beacon or
 *				    prob response.
 * \param  ieLength		-   Total length of the IE elements.
 * \param  pOuiInfo		-   The OUI info. for the element to be retrived
 * \param  pIeElement		-   Memory to which IE elements to be copied.
 * \returns none.
 *****************************************************************************/
void UMI_ProcVendorSpecificIeElementsWPA(uint8 *pIeElementStart,
					 uint16 ieLength,
					 UMI_OUI_INFO *pOuiInfo,
					 IE_ELEMENT *pIeElement)
{
	int ieLen = ieLength;
	uint8 ieElementId = 0;
	uint8 ieElementSize = 0;

	UMI_TRACE(UMI_Utility, ("--> UMI_ProcVendorSpecificIeElements\n"));

	if (pIeElementStart == NULL)
		goto Exit_Func;

	UMI_OS_ASSERT(pIeElementStart);

	UMI_MemoryReset(pIeElement, sizeof(IE_ELEMENT));

	do {
		ieElementId = (uint8)*pIeElementStart;
		ieElementSize = (uint8) *(pIeElementStart + 1);

		if (ieElementId == IE_VENDOR_SPECIFIC_ID) {
			if (UMI_MemoryCompare((pIeElementStart + 2), 4, (uint8 *) pOuiInfo, 4)) {
				pIeElement->id = ieElementId;
				pIeElement->size = ieElementSize;
				pIeElement->pInfo = (uint8 *) (pIeElementStart + 2);

				/* Got what we were interested in,
				so break here */
				break;
			}
		}

		pIeElementStart = (pIeElementStart + ieElementSize + 2);
		ieLen = (ieLen - (ieElementSize + 2));

	} while ((ieLen > 0));

 Exit_Func:;
	return;

}	/* End UMI_ProcVendorSpecificIeElements() */



#if P2P_MINIAP_SUPPORT
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
					 P2P_IE_ELEMENT *pIeElement)
{
	int ieLen = ieLength;
	uint8 ieElementId = 0;
	uint8 ieElementSize = 0;
	uint16 p2pAttributesLen = 0;
	uint8 pOuiInfo[4] = {0x50, 0x6F, 0x9A, 0x09};
	uint8 *p2pAttributesID = NULL;
	uint16 attribLen = 0;
	uint8 atrribFound = FALSE;
	UMI_TRACE(UMI_Utility, ("--> UMI_ProcVendorSpecificIeElements\n"));

	if (pIeElementStart == NULL)
		goto Exit_Func;

	UMI_OS_ASSERT(pIeElementStart);

	UMI_MemoryReset(pIeElement, sizeof(P2P_IE_ELEMENT));
	do {
		ieElementId = (uint8)*pIeElementStart;
		ieElementSize = (uint8) *(pIeElementStart + 1);

		if (ieElementId == IE_VENDOR_SPECIFIC_ID) {
			if (UMI_MemoryCompare((pIeElementStart + 2), 4, (uint8 *) pOuiInfo, 4)) {
				p2pAttributesID = (pIeElementStart + 6);
				p2pAttributesLen = ieElementSize - 4;
				attribLen = *(uint16 *)(p2pAttributesID + 1);
				/*Check if NoA is present*/
				while (p2pAttributesLen) {
					if (*p2pAttributesID == p2pAttribID) {
						pIeElement->id = ieElementId;
						pIeElement->size = *(uint16 *)(p2pAttributesID + 1);
						pIeElement->pInfo = (uint8 *) (p2pAttributesID + 3);
						/* Got what we were interested in,
						so break here */
						atrribFound = TRUE;
						break;
					}
					p2pAttributesID += (attribLen + 3);
					p2pAttributesLen -= (attribLen + 3);
					attribLen = *(uint16 *)(p2pAttributesID + 1);
				}
			}
		}
		pIeElementStart = (pIeElementStart + ieElementSize + 2);
		ieLen = (ieLen - (ieElementSize + 2));
		if (atrribFound == TRUE)
			break;
	} while ((ieLen > 0));

Exit_Func:
	return;
}
#endif	/* P2P_MINIAP_SUPPORT */
