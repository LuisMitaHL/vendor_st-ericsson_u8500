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
 * \file UMI_ScanList.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_ScanList.c
 * \brief
 * This file maintains the scan list recieved on the Host.
 * \ingroup Upper_MAC_Interface
 * \date 23/01/09 17:02
 */

/*****************************************************************************/
/********		       Include Files			      ********/
/*****************************************************************************/
#include "UMI_ScanList.h"
#include "UMI_OsIf.h"
#include "UMI_DebugTrace.h"
#include "UMI_Utility.h"

/******************************************************************************
 * NAME:	UMI_InitScanList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the UMAC Scan list.This function should be called
 * once during startup.
 * \returns UMI_Q     Pointer to the queue.
 *****************************************************************************/
UMI_Q *UMI_InitScanList()
{
	UMI_Q *pScanList = NULL;

	pScanList = UMI_QueueInit();

	UMI_OS_ASSERT(pScanList);

	return pScanList;
}	/* End UMI_InitScanList() */

/******************************************************************************
 * NAME:	UMI_GetBssInfoMacAddr
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the WFM Scan list.This function should be called
 * during startup once.
 * \param  pScanList   - Pointer to Scan list queue.
 * \param  pMacAddress - Pointer to MAC Address.
 * \returns UMI_BSS_CACHE_INFO_IND*   A pointer to UMI_BSS_CACHE_INFO_IND
 *				      element, NULL otherwise.
 *****************************************************************************/
UMI_BSS_CACHE_INFO_IND *UMI_GetBssInfoMacAddr(UMI_Q *pScanList,
					      uint8 *pMacAddress)
{
	uint32 i = 0;
	UMI_BSS_CACHE_INFO_IND *pBssCacheInfoInd = NULL;
	uint32 currentListSize = 0;
	uint8 flag = FALSE;
	UMI_Q_ITEM *pCurrentElement = pScanList->Head;

	UMI_TRACE(UMI_ScanList, ("---> UMI_GetBssInfoMacAddr()\n"));

	currentListSize = UMI_QueueGetSize(pScanList);

	while ((i < currentListSize) && (pCurrentElement != NULL)) {
		pBssCacheInfoInd = pCurrentElement->Pkt;

		if (!pBssCacheInfoInd) {
			i++;
			pCurrentElement = pCurrentElement->Next;
			continue;
		}

		if (UMI_MemoryCompare(pBssCacheInfoInd->bssId, UMI_MAC_ADDRESS_SIZE, pMacAddress, UMI_MAC_ADDRESS_SIZE)
		    ) {
			flag = TRUE;
			break;
		}

		i++;
		pCurrentElement = pCurrentElement->Next;
	}	/*while ((i < currentListSize) && (pCurrentElement != NULL)) */

	if (!flag)
		pBssCacheInfoInd = NULL;

	UMI_TRACE(UMI_ScanList, ("<--- UMI_GetBssInfoMacAddr()\n"));

	return pBssCacheInfoInd;

}	/* End UMI_GetBssInfoMacAddr() */

/******************************************************************************
 * NAME:	UMI_GetBssInfoSsid
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function Returns the UMI_BSS_CACHE_INFO_IND element corresponding to the
 * given SSID
 * \param  pScanList   - Pointer to Scan list queue.
 * \param  ssidLength  - Length of the SSID.
 * \param  pSsid       - pointer to SSID.
 * \param  encStatus   - Encryption Status of the AP to be searched
 * \returns UMI_BSS_CACHE_INFO_IND*   A pointer to UMI_BSS_CACHE_INFO_IND
 *				      element, returns NULL if fails.
 *****************************************************************************/
UMI_BSS_CACHE_INFO_IND *UMI_GetBssInfoSsid(UMI_Q *pScanList,
					   uint8 ssidLength,
					   uint8 *pSsid,
					   uint32 encStatus)
{
	UMI_BSS_CACHE_INFO_IND *pBssCacheInfoInd = NULL;
	UMI_BSS_CACHE_INFO_IND *pBssInfoHighestRssi = NULL;
	IE_ELEMENT ieElement = { 0 };
	uint32 i = 0;
	uint32 currentListSize = 0;
	UMI_Q_ITEM *pCurrentElement = pScanList->Head;
	uint32 encStatusAp = 0;

	UMI_TRACE(UMI_ScanList, ("---> UMI_GetBssInfoSsid()\n"));

	if (UMI_MAX_SSID_SIZE < ssidLength) {
		UMI_ERROR(UMI_ScanList, ("UMI_GetBssInfoSsid: Invalid SSID Size (%d)\n", ssidLength));
		return NULL;
	}

	currentListSize = UMI_QueueGetSize(pScanList);

	if (ssidLength == 0) {
		/* Can associated with ANY available if 0 */
		/* connect with first one in the list */
		/* if some AP is present, then bss_info_ptr_highest_rssi will
		be non-null */
		pBssInfoHighestRssi = (UMI_BSS_CACHE_INFO_IND *) UMI_QueueReturnElementAtIndex(pScanList, 0);

	} else {

		while ((i < currentListSize) && (pCurrentElement != NULL)) {
			pBssCacheInfoInd = pCurrentElement->Pkt;

			if (!pBssCacheInfoInd) {
				i++;
				pCurrentElement = pCurrentElement->Next;
				continue;
			}

			UMI_ProcIeElements(
				(uint8 *) &pBssCacheInfoInd->ieElements[0],
				pBssCacheInfoInd->ieLength,
				IE_SSID_ID,
				&ieElement
				);

			if (UMI_MemoryCompare(ieElement.pInfo, ieElement.size, pSsid, ssidLength)) {
				if (!(pBssCacheInfoInd->capability & 0x0010))
					encStatusAp = UMI_ENC_NO_ENCRYPTION;
				else {
					UMI_OUI_INFO OuiInfo = {
						WPA_OUI,
						0x01,
						WPA_IE_PARAMETER_ELEMENT_SUB_TYPE
					};
					UMI_ProcVendorSpecificIeElements((uint8 *) &pBssCacheInfoInd->ieElements[0], pBssCacheInfoInd->ieLength, &OuiInfo, &ieElement);
					/* Both WPA IE and RSN IE wont be
					present together */
					if (ieElement.size)
						encStatusAp = UMI_ENC_TKIP_SUPPORTED;
					else {
						UMI_ProcIeElements((uint8 *) &pBssCacheInfoInd->ieElements[0], pBssCacheInfoInd->ieLength, IE_RSN_ID, &ieElement);
						if (ieElement.size)
							encStatusAp = UMI_ENC_AES_SUPPORTED;
						else {
							UMI_ProcIeElements((uint8 *) &pBssCacheInfoInd->ieElements[0], pBssCacheInfoInd->ieLength, IE_WAPI_ID, &ieElement);

							if (ieElement.size)
								encStatusAp = UMI_ENC_WAPI_SUPPORTED;
							else
								encStatusAp = UMI_ENC_WEP_SUPPORTED;
						}
					}
				}

				/* Connect only if the encryption status
				matches */
				if ((encStatusAp & encStatus) || (encStatusAp == encStatus)) {
					if (pBssInfoHighestRssi == NULL) {
						/* its the first found AP (with matching SSID) in the list */
						pBssInfoHighestRssi = pBssCacheInfoInd;

					} /* if (pBssInfoHighestRssi == NULL) */
					else {

						/* more than 1 APs are present with same SSID */
						if (pBssCacheInfoInd->rssi > pBssInfoHighestRssi->rssi) {
							/* store it if it has higher signal strength */
							pBssInfoHighestRssi = pBssCacheInfoInd;

						} /* if (pBssInfoPtr->Rssi > pBssInfoHighestRssi->Rssi) */
					} /* if (pBssInfoHighestRssi == NULL) */
				} /*if(encStatusAp == encStatus) */
			}

			i++;
			pCurrentElement = pCurrentElement->Next;
		}

	} /* if (ssidLength == 0) */

	UMI_TRACE(UMI_ScanList, ("<--- UMI_GetBssInfoSsid()\n"));

	return pBssInfoHighestRssi;
}	/* end UMI_GetBssInfoSsid() */

/******************************************************************************
 * NAME:	UMI_ClearScanList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function clears the entire BSS list.
 * \param  pScanList   - Pointer to Scan list queue.
 * \returns NULL.
 *****************************************************************************/
void UMI_ClearScanList(UMI_Q *pScanList)
{
	UMI_Q_STATUS status = UMI_Q_SUCCESS;

	UMI_TRACE(UMI_ScanList, ("---> UMI_ClearScanList()\n"));

	status = UMI_QueueFlush(pScanList);

	if (UMI_Q_SUCCESS != status)
		UMI_ERROR(UMI_ScanList, ("UMI_ClearScanList: Failed to flush scan List\n"));
	UMI_TRACE(UMI_ScanList, ("<--- UMI_ClearScanList()\n"));

}	/* end UMI_ClearScanList() */

/******************************************************************************
 * NAME:	UMI_InsertBssElement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function inserts a new element to the scan list.This function will
 * allocate memory for the element.If an element is removed from the list its
 * memory will be deallocated. If the list is full, the oldest memeber will be
 * replaced with the new element.If not full, the new element will be put in
 * the avilable rack.
 * \param  pScanList   - Pointer to Scan list queue.
 * \param  pBssElement - Pointer to the BSS element to be inserted.
 * \param  numberOfExtraBytes - Extra Bytes to be allocated to accomodate
 *			  Supported Rates information.
 * \returns NULL.
 *****************************************************************************/
void UMI_InsertBssElement(UMI_Q *pScanList,
			  WFM_BSS_INFO *pBssElement,
			  uint16 numberOfExtraBytes)
{
	UMI_Q_STATUS status = UMI_Q_SUCCESS;
	WFM_BSS_INFO *pBssInfo = NULL;

	UMI_TRACE(UMI_ScanList, ("---> UMI_InsertBssElement()\n"));

	if (pBssElement == NULL) {
		UMI_ERROR(UMI_ScanList, ("UMI_InsertBssElement: Unexpected NULL BssElement\n"));
		return;
	}

	if (UMI_GetBssInfoMacAddr(pScanList, pBssElement->MacAddress) == NULL) {
		pBssInfo = (WFM_BSS_INFO *) UMI_Allocate(sizeof(WFM_BSS_INFO) + numberOfExtraBytes - WFM_BSS_INFO_SUPPORTED_RATE_SIZE);

		if (pBssInfo) {
			UMI_MemoryCopy(pBssInfo, pBssElement, (sizeof(WFM_BSS_INFO) + numberOfExtraBytes - WFM_BSS_INFO_SUPPORTED_RATE_SIZE));
			if (UMI_MAX_NUM_DEV_ENTRIES_SUPPORTED < UMI_QueueGetSize(pScanList)) {
				/* List is full, so discard recevied beacon */
				UMI_Free(pBssInfo);
				goto Exit_Func;
			}

			status = UMI_QueueInsert(pScanList, pBssInfo);

			if (UMI_Q_SUCCESS != status)
				UMI_ERROR(UMI_ScanList, ("UMI_InsertBssElement: Failed to Insert an element to scan List\n"));
		} /* if(pBssInfo) */
		else
			UMI_ERROR(UMI_ScanList, ("UMI_InsertBssElement: Failed to allocate Memory for Scan Element\n"));

		UMI_DEFAULT(UMI_ScanList, ("UMI_InsertBssElement: scan_list NumberOfDevices (%d)\n", UMI_QueueGetSize(pScanList)));

	} /* if (UMI_GetBssInfoMacAddr(pBssElement->MacAddress) == NULL) */
 Exit_Func:;

	UMI_TRACE(UMI_ScanList, ("<--- UMI_InsertBssElement()\n"));

	return;
}	/* end UMI_InsertBssElement() */

/******************************************************************************
 * NAME:	UMI_DeInitScanList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function De-initializes the UMI Scan list.This function should be called
 * from System Deinit.
 * \param  pScanList   - The queue on which the operation needs to be done.
 * \returns NULL.
 *****************************************************************************/
void UMI_DeInitScanList(UMI_Q *pScanList)
{
	UMI_Q_STATUS status = UMI_Q_SUCCESS;

	UMI_TRACE(UMI_ScanList, ("---> UMI_DeInitScanList()\n"));

	status = UMI_QueueFlush(pScanList);

	if (UMI_Q_SUCCESS != status)
		UMI_ERROR(UMI_ScanList, ("UMI_DeInitScanList: Failed to flush the Scan List\n"));

	status = UMI_QueueDinit(pScanList);

	if (UMI_Q_SUCCESS != status)
		UMI_ERROR(UMI_ScanList, ("UMI_DeInitScanList: Failed to DeInit the Scan List\n"));

	UMI_TRACE(UMI_ScanList, ("<--- UMI_DeInitScanList()\n"));

}	/* End UMI_DeInitScanList() */

/******************************************************************************
 * NAME:	UMI_GetMacAddrIndex
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns the queue index of the UMI_BSS_INFO element
 * corresponding to the given MAC address
 * \param  pScanList   - Pointer to Scan list queue.
 * \param  pMacAddress - Pointer to the MAC Address.
 * \returns uint8 Index of the element.
 *****************************************************************************/
uint8 UMI_GetMacAddrIndex(UMI_Q *pScanList, uint8 *pMacAddress)
{
	uint32 i = 0;
	UMI_BSS_CACHE_INFO_IND *pBssCacheInfoInd = NULL;
	uint32 currentListSize = 0;
	UMI_Q_ITEM *pCurrentElement = (UMI_Q_ITEM *) pScanList->Head;

	UMI_TRACE(UMI_ScanList, ("---> UMI_GetMacAddrIndex()\n"));

	currentListSize = UMI_QueueGetSize(pScanList);

	while ((i < currentListSize) && (pCurrentElement != NULL)) {
		pBssCacheInfoInd = pCurrentElement->Pkt;

		if (!pBssCacheInfoInd) {
			i++;
			pCurrentElement = pCurrentElement->Next;
			continue;
		}

		if (UMI_MemoryCompare(pBssCacheInfoInd->bssId, UMI_MAC_ADDRESS_SIZE, pMacAddress, UMI_MAC_ADDRESS_SIZE)
		    ) {
			break;
		}

		i++;
		pCurrentElement = pCurrentElement->Next;
	}	/*while ((i < currentListSize) && (pCurrentElement != NULL)) */

	return (uint8) i;

}	/* End UMI_GetMacAddrIndex() */
