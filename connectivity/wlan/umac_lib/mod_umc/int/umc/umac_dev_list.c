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
 * \file umac_dev_list.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_dev_list.c
 * \brief
 * This file implements a list which stores scan result/beacon Info.
 * \ingroup Upper_MAC_Core
 * \date 19/02/08 14:52
 */

/******************************************************************************
	      INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "umac_dev_conf.h"
#include "umac_dev_list.h"
#include "umac_messages.h"
#include "umac_dbg.h"
#include "umac_sm.h"

/******************************************************************************
			  LOCAL DATA
******************************************************************************/

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_InitList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API initializes the Device list.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns WFM_DEV_LIST_STATUS.
 *****************************************************************************/
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_InitList(void *pCurrentInstance)
{
	uint16 Count = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pCurrentInstance;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_InitList()\n");

	if (!pUmacInstance->gVars.DeviceQueue.IsInitialized) {
		pUmacInstance->gVars.DeviceQueue.HeadElement = 0;
		pUmacInstance->gVars.DeviceQueue.TailElement = 0;
		pUmacInstance->gVars.DeviceQueue.NumberOfElements = 0;

		for (Count = 0; Count < WFM_UMAC_DEV_LIST_MAX_SIZE; Count++) {
			OS_MemoryReset(
				pUmacInstance->gVars.DeviceQueue.DeviceStore[Count].DeviceElement,
				WFM_UMAC_DEV_MAX_SIZE_OF_ENTRY
				);

			pUmacInstance->gVars.DeviceQueue.DeviceStore[Count].ElementSize = 0;
		}

		pUmacInstance->gVars.DeviceQueue.IsInitialized = 1;
		pUmacInstance->gVars.pWfmDevEntry = NULL;
	} else
		LOG_EVENT(DBG_WFM_DEVLIST, "List is in initialized state\n");

	return WFM_DEV_LIST_SUCCESS;

} /* end WFM_UMAC_DEV_InitList() */

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_GetMemoryElement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns the next memory location available for storing an element.
 * If the number of entries in the list has reached WFM_UMAC_DEV_LIST_MAX_SIZE,
 * this API will return NULL.
 * WFM_UMAC_DEV_InsertToList() should be called to finish the list updation.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns pointer to the WFM_DEV_ELEMENT if successful, NULL otherwise.
 *****************************************************************************/
WFM_DEV_ELEMENT *WFM_UMAC_DEV_GetMemoryElement(void *pCurrentInstance)
{
	WFM_DEV_ELEMENT *pElement = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pCurrentInstance;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_GetMemoryElement()\n");

	if (pUmacInstance->gVars.DeviceQueue.IsInitialized) {
		if (!pUmacInstance->gVars.pWfmDevEntry) {
			if (pUmacInstance->gVars.DeviceQueue.NumberOfElements < WFM_UMAC_DEV_LIST_MAX_SIZE) {
				pElement = (void *)&pUmacInstance->gVars.DeviceQueue.DeviceStore[pUmacInstance->gVars.DeviceQueue.TailElement];

				OS_ASSERT(pElement)

				    OS_ASSERT(!(pElement->ElementSize > sizeof(WFM_DEV_ELEMENT)));

				/*
				   There will be some values remaining from the
				   previous operation Clearing it.
				 */

				OS_MemoryReset(pElement, pElement->ElementSize);

				pUmacInstance->gVars.pWfmDevEntry = (uint8 *) pElement;
			} else {
				LOG_EVENT(DBG_WFM_DEVLIST, "List is full\n");
				pElement = NULL;
			}
		} else
			LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_GetMemoryElement() is outstanding\n");
	} else
		LOG_EVENT(DBG_WFM_DEVLIST, "List Is not initialized\n");

	return pElement;

} /* end WFM_UMAC_DEV_GetMemoryElement() */

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_GiveBackMemoryElement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns the next memory element given by the Device list back to
 * the pool.
 * WFM_UMAC_DEV_GetMemoryElement() should be called once before this call.
 * otherwise this function will fail.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns WFM_DEV_LIST_STATUS.
 *****************************************************************************/
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_GiveBackMemoryElement(void *pCurrentInstance)
{
	WFM_DEV_LIST_STATUS status = WFM_DEV_LIST_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pCurrentInstance;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_GiveBackMemoryElement()\n");

	if (pUmacInstance->gVars.DeviceQueue.IsInitialized) {
		if (pUmacInstance->gVars.pWfmDevEntry) {
			pUmacInstance->gVars.pWfmDevEntry = NULL;
		} else {
			LOG_EVENT(DBG_WFM_DEVLIST, "Cant Give back un-allocated memory\n");
			status = WFM_DEV_LIST_FAILURE;
		}
	} /* if ( pUmacInstance->gVars.DeviceQueue.IsInitialized ) */

	return status;
} /* end WFM_UMAC_DEV_GiveBackMemoryElement() */

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_InsertToList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API inserts an element to the Device list.
 * The API WFM_UMAC_DEV_GetMemoryElement() needs to be called exactly once
 * before calling this function.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns WFM_DEV_LIST_STATUS.
 *****************************************************************************/
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_InsertToList(void *pCurrentInstance)
{
	WFM_DEV_LIST_STATUS status = WFM_DEV_LIST_FAILURE;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pCurrentInstance;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_InsertToList()\n");

	if (pUmacInstance->gVars.DeviceQueue.IsInitialized) {
		if (pUmacInstance->gVars.pWfmDevEntry) {
			pUmacInstance->gVars.DeviceQueue.NumberOfElements++;

			if (pUmacInstance->gVars.DeviceQueue.NumberOfElements > WFM_UMAC_DEV_LIST_MAX_SIZE)
				pUmacInstance->gVars.DeviceQueue.NumberOfElements = WFM_UMAC_DEV_LIST_MAX_SIZE;

			pUmacInstance->gVars.DeviceQueue.TailElement++;

			if (pUmacInstance->gVars.DeviceQueue.TailElement >= WFM_UMAC_DEV_LIST_MAX_SIZE)
				pUmacInstance->gVars.DeviceQueue.TailElement = 0;	/*Rounding off the Tail */

			pUmacInstance->gVars.pWfmDevEntry = NULL;

			status = WFM_DEV_LIST_SUCCESS;
		} else
			LOG_EVENT(DBG_WFM_DEVLIST, "Calling this function without WFM_UMAC_DEV_GetMemoryElement() is illegal\n");
	} else
		LOG_EVENT(DBG_WFM_DEVLIST, "List Is not initialized\n");

	return status;
} /* end WFM_UMAC_DEV_InsertToList() */

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_GetListSize
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns the number of entries in the list.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns uint32   Number of elements in the list..
 *****************************************************************************/
uint32 WFM_UMAC_DEV_GetListSize(void *pCurrentInstance)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pCurrentInstance;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_GetListSize()\n");

	if (!pUmacInstance->gVars.DeviceQueue.IsInitialized)
		LOG_EVENT(DBG_WFM_DEVLIST, "List Is not initialized\n");

	return pUmacInstance->gVars.DeviceQueue.NumberOfElements;

} /* end WFM_UMAC_DEV_GetListSize() */

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_GetNextElement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns the next device element from the list.The entry will be
 * removed from the list.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns WFM_DEV_ELEMENT*   Pointer to the WFM_DEV_ELEMENT element.
 *****************************************************************************/
WFM_DEV_ELEMENT *WFM_UMAC_DEV_GetNextElement(void *pCurrentInstance)
{
	WFM_DEV_ELEMENT *pElement = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pCurrentInstance;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_GetNextElement()\n");

	if (pUmacInstance->gVars.DeviceQueue.IsInitialized) {
		if (pUmacInstance->gVars.DeviceQueue.NumberOfElements) {
			if (pUmacInstance->gVars.DeviceQueue.HeadElement < WFM_UMAC_DEV_LIST_MAX_SIZE) {
				pElement = (WFM_DEV_ELEMENT *) &pUmacInstance->gVars.DeviceQueue.DeviceStore[pUmacInstance->gVars.DeviceQueue.HeadElement++];

				pUmacInstance->gVars.DeviceQueue.NumberOfElements--;

				if (pUmacInstance->gVars.DeviceQueue.HeadElement >= WFM_UMAC_DEV_LIST_MAX_SIZE)
					pUmacInstance->gVars.DeviceQueue.HeadElement = 0;
			} else
				LOG_EVENT(DBG_WFM_DEVLIST, "Device List Head Out of range\n");
		} /* if( pUmacInstance->gVars.DeviceQueue.NumberOfElements ) */
	} else
		LOG_EVENT(DBG_WFM_DEVLIST, "List Is not initialized\n");

	return pElement;
} /* end WFM_UMAC_DEV_GetNextElement() */

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_GetElementWithBSSID
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns the element with the specified BSSID from the list, if
 * exists.
 * \param *pCurrentInstance  - Current UMAC Instance.
 * \param *pBssid	     - Pointer to Bssid array.
 * \returns WFM_DEV_ELEMENT*   Pointer to the WFM_DEV_ELEMENT element, if exists
 * otherwise NULL.
 *****************************************************************************/
WFM_DEV_ELEMENT *WFM_UMAC_DEV_GetElementWithBSSID(void *pCurrentInstance,
						  uint8 *pBssid)
{
	WFM_BSS_CACHE_INFO_IND *DevInfo = NULL;
	uint8 Count = 0;
	WFM_DEV_ELEMENT *pDevElem = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pCurrentInstance;
	uint8 Index;

	OS_ASSERT(pUmacInstance);

	Index = (uint8) pUmacInstance->gVars.DeviceQueue.HeadElement;

	LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_GetElementWithBSSID()\n");

	if (pUmacInstance->gVars.DeviceQueue.IsInitialized) {
		for (Count = 0; Count < pUmacInstance->gVars.DeviceQueue.NumberOfElements; Count++) {
			pDevElem = (WFM_DEV_ELEMENT *) &pUmacInstance->gVars.DeviceQueue.DeviceStore[Index];
			DevInfo = (WFM_BSS_CACHE_INFO_IND *) &pDevElem->DeviceElement;

			if (OS_MemoryEqual((void *)DevInfo->BssId, WFM_MAC_ADDRESS_SIZE, pBssid, WFM_MAC_ADDRESS_SIZE)
			    ) {
				break;
			}

			Index++;

			if (Index >= WFM_UMAC_DEV_LIST_MAX_SIZE)
				Index = 0; /* Here Tail is less than head */
		}

		if (Count == pUmacInstance->gVars.DeviceQueue.NumberOfElements) {
			DevInfo = NULL;
			pDevElem = NULL;
		}
	} else
		LOG_EVENT(DBG_WFM_DEVLIST, "List Is not initialized\n");

	return pDevElem;
} /* end WFM_UMAC_DEV_GetElementWithBSSID() */

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_DeInitList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API De-Initializes the Device list.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns WFM_DEV_LIST_STATUS.
 *****************************************************************************/
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_DeInitList(void *pCurrentInstance)
{
	WFM_DEV_LIST_STATUS status = WFM_DEV_LIST_FAILURE;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pCurrentInstance;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_DeInitList()\n");

	if (pUmacInstance->gVars.DeviceQueue.IsInitialized) {
		pUmacInstance->gVars.DeviceQueue.IsInitialized = 0;
		status = WFM_DEV_LIST_SUCCESS;
	} /*if ( pUmacInstance->gVars.DeviceQueue.IsInitialized ) */

	return status;
} /* end WFM_UMAC_DEV_DeInitList() ; */

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_ClearList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API empties the Device list.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns WFM_DEV_LIST_STATUS.
 *****************************************************************************/
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_ClearList(void *pCurrentInstance)
{
	WFM_DEV_LIST_STATUS status = WFM_DEV_LIST_FAILURE;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pCurrentInstance;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_DEVLIST, "WFM_UMAC_DEV_ClearList()\n");

	if (pUmacInstance->gVars.DeviceQueue.IsInitialized) {
		pUmacInstance->gVars.DeviceQueue.IsInitialized = 0;
		WFM_UMAC_DEV_InitList(pUmacInstance);
	} /* if ( pUmacInstance->gVars.DeviceQueue.IsInitialized ) */

	return status;

} /* end WFM_UMAC_DEV_ClearList() */
