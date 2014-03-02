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
 * \file umac_dev_list.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_dev_list.h
 * \brief
 *This file implements a list which stores scan result/beacon Info.
 * \ingroup Upper_MAC_Core
 * \date 19/02/08 14:52
 */

#ifndef _UMAC_DEV_LIST_H
#define _UMAC_DEV_LIST_H

/******************************************************************************
			    INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "umac_dev_conf.h"

/*
  The following macro's value should be same as
  that of WFM_UMAC_MAX_FRAME_BODY_SIZE defined
  in umac_frames.h
*/

#define WFM_UMAC_DEV_MAX_SIZE_OF_ENTRY  2312

/*
  Maximum number of beacon frames/ Prob responses
  to be stored in the device
*/
#define WFM_UMAC_DEV_LIST_MAX_SIZE 1

typedef enum WFM_DEV_LIST_STATUS_E {
	WFM_DEV_LIST_FAILURE = 0,
	WFM_DEV_LIST_SUCCESS
} WFM_DEV_LIST_STATUS;

typedef struct WFM_DEV_ELEMENT_S {
	uint32 ElementSize;	/*Actual size of the stored element */
	uint8 DeviceElement[WFM_UMAC_DEV_MAX_SIZE_OF_ENTRY];
} WFM_DEV_ELEMENT;

typedef struct WFM_DEV_LIST_S {
	uint16 NumberOfElements;
	uint16 IsInitialized;
	uint16 HeadElement;	/* Oldest Element in the queue */
	uint16 TailElement;	/* Oldest Element in the queue */
	WFM_DEV_ELEMENT DeviceStore[WFM_UMAC_DEV_LIST_MAX_SIZE];
} WFM_DEV_LIST;

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_InitList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API initializes the Device list.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns WFM_DEV_LIST_STATUS.
 *****************************************************************************/
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_InitList(void *pCurrentInstance);

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
WFM_DEV_ELEMENT *WFM_UMAC_DEV_GetMemoryElement(void *pCurrentInstance);

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
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_GiveBackMemoryElement(void *pCurrentInstance);

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
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_InsertToList(void *pCurrentInstance);

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_GetListSize
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns the number of entries in the list.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns uint32   Number of elements in the list..
 *****************************************************************************/
uint32 WFM_UMAC_DEV_GetListSize(void *pCurrentInstance);

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
WFM_DEV_ELEMENT *WFM_UMAC_DEV_GetNextElement(void *pCurrentInstance);

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
						  uint8 *pBssid);

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_DeInitList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API De-Initializes the Device list.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns WFM_DEV_LIST_STATUS.
 *****************************************************************************/
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_DeInitList(void *pCurrentInstance);

/******************************************************************************
 * NAME:	WFM_UMAC_DEV_ClearList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API empties the Device list.
 * \param *pCurrentInstance   - Current UMAC Instance.
 * \returns WFM_DEV_LIST_STATUS.
 *****************************************************************************/
WFM_DEV_LIST_STATUS WFM_UMAC_DEV_ClearList(void *pCurrentInstance);

#endif	/* _UMAC_DEV_LIST_H */
