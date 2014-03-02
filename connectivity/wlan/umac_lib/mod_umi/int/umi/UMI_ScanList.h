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
 * \file UMI_ScanList.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_ScanList.h
 * \brief
 * This file maintains the scan list recieved on the Host.
 * \ingroup Upper_MAC_Interface
 * \date 23/01/08 18:50
 */

#ifndef _UMI_SCAN_LIST_H
#define _UMI_SCAN_LIST_H

/*----------------------------------------------------------------------------*
 * Include Files							      *
 *---------------------------------------------------------------------------*/
#include "umac_messages.h"
#include "UMI_Queue.h"
#include "UMI_Api.h"

#define UMI_ENC_NO_ENCRYPTION    0
#define UMI_ENC_WEP_SUPPORTED    1
#define UMI_ENC_TKIP_SUPPORTED   2
#define UMI_ENC_AES_SUPPORTED    4
#define UMI_ENC_WAPI_SUPPORTED   8
/*
  Maximum number of entries that can be there in the Host scan list at a time
*/
#define UMI_MAX_NUM_DEV_ENTRIES_SUPPORTED 256

/******************************************************************************
 * NAME:	UMI_InitScanList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the UMAC Scan list.This function should be called
 * once during startup.
 * \returns UMI_Q     Pointer to the queue.
 *****************************************************************************/
UMI_Q *UMI_InitScanList(void);

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
 *                                    element, NULL otherwise.
 *****************************************************************************/
UMI_BSS_CACHE_INFO_IND *UMI_GetBssInfoMacAddr(UMI_Q *pScanList,
					      uint8 *pMacAddress);

/******************************************************************************
 * NAME:	UMI_GetBssInfoSsid
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function Returns the UMI_BSS_CACHE_INFO_IND element corresponding to
 * the given SSID
 * \param  pScanList   - Pointer to Scan list queue.
 * \param  ssidLength  - Length of the SSID.
 * \param  pSsid       - pointer to SSID.
 * \param  encStatus   - Encryption Status of the AP to be searched
 * \returns UMI_BSS_CACHE_INFO_IND*   A pointer to UMI_BSS_CACHE_INFO_IND
 *                                    element, returns NULL if fails.
 *****************************************************************************/
UMI_BSS_CACHE_INFO_IND *UMI_GetBssInfoSsid(UMI_Q *pScanList,
					   uint8 ssidLength,
					   uint8 *pSsid,
					   uint32 encStatus);

/******************************************************************************
 * NAME:	UMI_ClearScanList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function clears the entire BSS list.
 * \param  pScanList   - Pointer to Scan list queue.
 * \returns NULL.
 *****************************************************************************/
void UMI_ClearScanList(UMI_Q *pScanList);

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
 *			        Supported Rates information.
 * \returns NULL.
 *****************************************************************************/
void UMI_InsertBssElement(UMI_Q *pScanList,
			  WFM_BSS_INFO *pBssElement,
			  uint16 numberOfExtraBytes);

/******************************************************************************
 * NAME:	UMI_DeInitScanList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function De-initializes the UMI Scan list.This function should be
 * called from System Deinit.
 * \param  pScanList   - The queue on which the operation needs to be done.
 * \returns NULL.
 *****************************************************************************/
void UMI_DeInitScanList(UMI_Q *pScanList);

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
uint8 UMI_GetMacAddrIndex(UMI_Q *pScanList, uint8 *pMacAddress);

#endif	/*_UMI_SCAN_LIST_H */
