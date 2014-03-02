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
 * \file UMI_ProcUmacMsg.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_ProcUmacMsg.c
 * \brief
 * Implementation of API which are exposed to UMAC.
 * \ingroup Upper_MAC_Interface
 * \date 21/01/09 17:02
 */

/*----------------------------------------------------------------------------*
 *Include Files								      *
 *---------------------------------------------------------------------------*/
#include "UMI_ProcUmacMsg.h"
#include "UMI_ApiInternal.h"
#include "UMI_DebugTrace.h"
#include "UMI_ScanList.h"
#include "UMI_Utility.h"
#include "UMI_OsIf.h"
#include "umac_ll_if.h"

/******************************************************************************
 * NAME:	UMI_ProcessMessages
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the incoming messages from core UMAC to UMI module.
 * \param umiHandle   -   Handle for this UMI instance.
 * \param pHiMessage  -   The incoming message.
 * \returns none.
 *****************************************************************************/

uint8 UMI_ProcessMessages(UMI_HANDLE umiHandle, WFM_HI_MESSAGE *pHiMessage)
{

	UMI_CONNECTION *pUmiCon = NULL;
	UMI_STATUS_CODE umiStatus = UMI_STATUS_SUCCESS;
	WFM_TRANSMIT_CNF *wfm_transmit_cnf = NULL;
	WFM_GET_PARAM_CNF *wfm_get_param_cnf = NULL;
	WFM_SET_PARAM_CNF *wfm_set_param_cnf = NULL;
	WFM_EVENT_IND *wfm_event_ind = NULL;
	UMI_BSS_CACHE_INFO_IND *umi_bss_cache_info_ind = NULL;
	WFM_BSS_CACHE_INFO_IND *wfm_bss_cache_info_ind = NULL;
	UMI_TX_DATA *pTxData = NULL;
	uint32 reference = 0;
	uint32 status = UMI_STATUS_SUCCESS;
	uint32 expMsgLen = 0;

	UMI_TRACE(UMI_ProcUmacMsg, ("---> UMI_ProcessMessages()\n"));

	if (umiHandle == NULL) {
		UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): Unexpected NULL umiHandle\n"));
		goto Exit_Func;

	}	/* if (umiHandle == NULL) */
	if (pHiMessage == NULL) {
		UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): Unexpected NULL\n"));
		goto Exit_Func;
	}	/* if (pHiMessage == NULL) */
	pUmiCon = (UMI_CONNECTION *) umiHandle;

	switch (pHiMessage->MessageId) {
	case WFM_TRANSMIT_CNF_ID:
		{
			UMI_DEFAULT(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): UMI_TRANSMIT_CNF_ID\n"));

			wfm_transmit_cnf = (WFM_TRANSMIT_CNF *) &pHiMessage->PayLoad.wfm_transmit_cnf;

			reference = wfm_transmit_cnf->Reference;

			if (pUmiCon->inParams.ulCallbacks.txComplete_Cb != NULL)
				pUmiCon->inParams.ulCallbacks.txComplete_Cb(pUmiCon->inParams.ulHandle, wfm_transmit_cnf->Status, pTxData);
			else
				UMI_ERROR(UMI_ProcUmacMsg, ("txComplete_Cb Not defined\n"));
		}
		break;

	case WFM_GET_PARAM_CNF_ID:
		{
			expMsgLen = sizeof(WFM_HI_MESSAGE)
			    - sizeof(WFM_HI_PAYLOAD)
			    + 80
			    + sizeof(uint16)	/* type of ParameterLength */
			    ;

			UMI_DEFAULT(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): UMI_GET_PARAM_CNF_ID\n"));

			if (pHiMessage->MessageLength < expMsgLen) {
				UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): Unexpected UMI_GET_PARAM_CNF_ID Length [%d < %d]\n", pHiMessage->MessageLength, expMsgLen)
				    );
			}	/* if (pHiMessage->MessageLength < expMsgLen) */

			wfm_get_param_cnf = (WFM_GET_PARAM_CNF *) &pHiMessage->PayLoad.wfm_get_param_cnf;


			if (pUmiCon->inParams.ulCallbacks.getParameterComplete_Cb != NULL)
				pUmiCon->inParams.ulCallbacks.getParameterComplete_Cb(pUmiCon->inParams.ulHandle, wfm_get_param_cnf->ParameterId, wfm_get_param_cnf->Status, wfm_get_param_cnf->ParameterLength, (void *)&wfm_get_param_cnf->ParameterData);
			else
				UMI_ERROR(UMI_ProcUmacMsg, ("getParameterComplete_Cb Not defined\n"));

			UMI_Free(pHiMessage);
		}

		break;

	case WFM_SET_PARAM_CNF_ID:
		{
			UMI_DEFAULT(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): UMI_SET_PARAM_CNF_ID\n"));

			/* UMI_SET_PARAM_CNF */
			wfm_set_param_cnf = (WFM_SET_PARAM_CNF *) &pHiMessage->PayLoad.wfm_set_param_cnf;

			reference = wfm_set_param_cnf->Reference;

			if ((wfm_set_param_cnf->ParameterId == UMI_DEVICE_OID_802_11_BSSID_LIST_SCAN)
			    && (wfm_set_param_cnf->Status == UMI_STATUS_SUCCESS)
			    ) {
				uint8 i;
				UMI_Q_ITEM *pCurrentElement = NULL;
				UMI_Q *pQueue = pUmiCon->pScanListQueue;
				/*Backup the current Scan list and clear it */
				UMI_CopyQueue(pUmiCon->pScanListOldQueue, pUmiCon->pScanListQueue);

				if (pQueue->NumElements) {
					for (i = 0; i < pQueue->NumElements - 1; i++) {
						pCurrentElement = pQueue->Head;
						pQueue->Head = pCurrentElement->Next;
						UMI_Free(pCurrentElement);
					}
					UMI_Free(pQueue->Head);
					pQueue->Head = NULL;
					pQueue->Tail = NULL;
					pQueue->NumElements = 0;
				}
			}

			if (pUmiCon->inParams.ulCallbacks.setParameterComplete_Cb != NULL)
				pUmiCon->inParams.ulCallbacks.setParameterComplete_Cb(pUmiCon->inParams.ulHandle, wfm_set_param_cnf->ParameterId, wfm_set_param_cnf->Status);
			else
				UMI_ERROR(UMI_ProcUmacMsg, ("setParameterComplete_Cb Not defined\n"));
			UMI_Free(pHiMessage);
		}
		break;

	case WFM_MEMORY_READ_CNF_ID:
		UMI_DEFAULT(UMI_ProcUmacMsg, ("WFM_MEMORY_READ_CNF_ID\n"));

		if (pUmiCon->inParams.ulCallbacks.memoryReadReqComplete_Cb != NULL)
			pUmiCon->inParams.ulCallbacks.memoryReadReqComplete_Cb(pUmiCon->inParams.ulHandle, (UMI_MEM_READ_CNF *) &pHiMessage->PayLoad.wfm_mem_read_cnf);
		else
			UMI_ERROR(UMI_ProcUmacMsg, ("memoryReadReqComplete_Cb Not defined\n"));

		UMI_Free(((uint8 *) pHiMessage - sizeof(UMAC_DEVIF_TX_DATA)));

		break;
	case WFM_MEMORY_WRITE_CNF_ID:
		UMI_DEFAULT(UMI_ProcUmacMsg, ("WFM_MEMORY_WRITE_CNF_ID\n"));

		if (pUmiCon->inParams.ulCallbacks.memoryWriteReqComplete_Cb != NULL)
			pUmiCon->inParams.ulCallbacks.memoryWriteReqComplete_Cb(pUmiCon->inParams.ulHandle, pHiMessage->PayLoad.wfm_mem_write_cnf.Result);
		else
			UMI_ERROR(UMI_ProcUmacMsg, ("memoryWriteReqComplete_Cb Not defined\n"));

		UMI_Free(((uint8 *) pHiMessage - sizeof(UMAC_DEVIF_TX_DATA)));

		break;

	case WFM_CONFIG_CNF_ID:
		UMI_DEFAULT(UMI_ProcUmacMsg, ("UMI_CONFIG_CNF_ID\n"));

		if (pUmiCon->inParams.ulCallbacks.configReqComplete_Cb != NULL)
			pUmiCon->inParams.ulCallbacks.configReqComplete_Cb(pUmiCon->inParams.ulHandle, (UMI_CONFIG_CNF *) &pHiMessage->PayLoad.wfm_config_cnf);
		else
			UMI_ERROR(UMI_ProcUmacMsg, ("configReqComplete_Cb Not defined\n"));
		UMI_Free(((uint8 *) pHiMessage - sizeof(UMAC_DEVIF_TX_DATA)));

		break;

	case WFM_GENERIC_CNF_ID:
		UMI_DEFAULT(UMI_ProcUmacMsg, ("WFM_GENERIC_CNF_ID\n"));

		UMI_Free(pHiMessage);

		break;
	case WFM_EXCEPTION_IND_ID:
		UMI_DEFAULT(UMI_ProcUmacMsg, ("WFM_EXCEPTION_IND_ID\n"));

		UMAC_ReleaseMemory(pUmiCon->umacHandle, pHiMessage);

		break;
	case WFM_STARTUP_IND_ID:
		UMI_DEFAULT(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): WFM_STARTUP_IND_ID\n"));

		/* Update UMI State Machine to UMI_STATE_STARTED */
		SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_STARTED);
		break;
	case WFM_TRACE_IND_ID:
		UMI_DEFAULT(UMI_ProcUmacMsg, ("WFM_TRACE_IND_ID\n"));

		UMAC_ReleaseMemory(pUmiCon->umacHandle, pHiMessage);

		break;
	case WFM_GENERIC_IND_ID:
		UMI_DEFAULT(UMI_ProcUmacMsg, ("WFM_GENERIC_IND_ID\n"));

		UMAC_ReleaseMemory(pUmiCon->umacHandle, pHiMessage);

		break;
	case WFM_EVENT_IND_ID:
		{
			UMI_DEFAULT(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): UMI_EVENT_IND_ID\n"));

			/* UMI_EVENT_IND */
			wfm_event_ind = (WFM_EVENT_IND *) &pHiMessage->PayLoad.wfm_event_ind;

			if (wfm_event_ind->EventId == WFM_EVT_STOP_COMPLETED) {
				umiStatus = pUmiCon->inParams.llCallbacks.stop_Cb(pUmiCon->llHandle);

				if (umiStatus == UMI_STATUS_SUCCESS)
					SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_STOPPED);
			}
			if (wfm_event_ind->EventId == WFM_EVT_SCAN_COMPLETED)
				UMI_ClearScanList(pUmiCon->pScanListOldQueue);

			if (pUmiCon->inParams.ulCallbacks.indicateEvent_Cb != NULL)
				pUmiCon->inParams.ulCallbacks.indicateEvent_Cb(pUmiCon->inParams.ulHandle, UMI_STATUS_SUCCESS,	/*Hard coding success now */
									       wfm_event_ind->EventId, wfm_event_ind->EventDataLength, (void *)wfm_event_ind->EventData);
			else
				UMI_ERROR(UMI_ProcUmacMsg, ("indicateEvent_Cb Not defined\n"));

                     if (wfm_event_ind->EventId != WFM_EVT_STOP_COMPLETED)
				UMAC_ReleaseMemory(pUmiCon->umacHandle, pHiMessage);
		}
		break;

	case WFM_BSS_CACHE_INFO_IND_ID:
		{
			uint32 cacheInfoLen = 0;
			uint8 len = 0;
			uint8 skipLen = 0;
			uint8 skipLen1 = 0;
			UMI_BSS_CACHE_INFO_IND *pBssInfo = NULL;
			uint32 index = 0;

			UMI_DEFAULT(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): UMI_BSS_CACHE_INFO_IND_ID\n"));

			/* UMI_BSS_CACHE_INFO_IND */
			wfm_bss_cache_info_ind = (WFM_BSS_CACHE_INFO_IND *) &pHiMessage->PayLoad.wfm_bss_cache_info_ind;

			pBssInfo = UMI_GetBssInfoMacAddr(pUmiCon->pScanListQueue, wfm_bss_cache_info_ind->BssId);

			if (pBssInfo == NULL) {
				/* Check ScanList Queue */
				if (UMI_MAX_NUM_DEV_ENTRIES_SUPPORTED < UMI_QueueGetSize(pUmiCon->pScanListQueue)) {
					/* List is full, so discard recevied beacon */
					goto Free_Beacon;
				}

				cacheInfoLen = sizeof(UMI_BSS_CACHE_INFO_IND)
				    + wfm_bss_cache_info_ind->IELength - sizeof(wfm_bss_cache_info_ind->IEElements);

				umi_bss_cache_info_ind = (UMI_BSS_CACHE_INFO_IND *) UMI_Allocate(cacheInfoLen);

				if (!umi_bss_cache_info_ind)
					UMI_OS_ASSERT(0);

				UMI_MemoryCopy(umi_bss_cache_info_ind, wfm_bss_cache_info_ind, cacheInfoLen);

				/* Insert Beacon/Prob response in ScanList Queue */
				status = UMI_QueueInsert(pUmiCon->pScanListQueue, umi_bss_cache_info_ind);
			} else {

				/*
				  Check SSID information element in the new
				  entry, if not present then replace with the
				  old entry
				*/
				if ((wfm_bss_cache_info_ind->IEElements[0] == IE_SSID_ID)
					&& (wfm_bss_cache_info_ind->IEElements[1] <= 1)
					&& (pBssInfo->ieElements[0] == IE_SSID_ID)
					&& (pBssInfo->ieLength > 1)) {
						/* Get extra SSID length */
						len =  pBssInfo->ieElements[1] - wfm_bss_cache_info_ind->IEElements[1];
						/*
						  Get complete length for BSS
						  Info.
						*/
						cacheInfoLen = sizeof(UMI_BSS_CACHE_INFO_IND)
								+ wfm_bss_cache_info_ind->IELength
								+ len
								- sizeof(wfm_bss_cache_info_ind->IEElements);

						umi_bss_cache_info_ind = (UMI_BSS_CACHE_INFO_IND *) UMI_Allocate(cacheInfoLen);

						if (!umi_bss_cache_info_ind)
							UMI_OS_ASSERT(0);

						UMI_MemoryCopy(
							umi_bss_cache_info_ind,
							wfm_bss_cache_info_ind,
							sizeof(UMI_BSS_CACHE_INFO_IND)
							);

						/*
						   Update IE Length with extra
						   SSID length
						*/
						umi_bss_cache_info_ind->ieLength = umi_bss_cache_info_ind->ieLength + len;
						skipLen = sizeof(UMI_BSS_CACHE_INFO_IND)
							- sizeof(umi_bss_cache_info_ind->ieElements);
						/*
						  Copy SSID present in the old
						  entry into new entry.
						*/
						UMI_MemoryCopy(
							((uint8 *)umi_bss_cache_info_ind + skipLen),
							((uint8 *)pBssInfo + skipLen),
							(pBssInfo->ieElements[1] + 2)
							);
						skipLen = sizeof(UMI_BSS_CACHE_INFO_IND)
							+ pBssInfo->ieElements[1] + 2
							- sizeof(umi_bss_cache_info_ind->ieElements);
						skipLen1 = sizeof(UMI_BSS_CACHE_INFO_IND)
							+ wfm_bss_cache_info_ind->IEElements[1] + 2
							- sizeof(umi_bss_cache_info_ind->ieElements);
						/*
						  Copy rest of the IE elements
						*/
						UMI_MemoryCopy(
							((uint8 *)umi_bss_cache_info_ind + skipLen),
							((uint8 *)wfm_bss_cache_info_ind + skipLen1),
							(wfm_bss_cache_info_ind->IELength - wfm_bss_cache_info_ind->IEElements[1] - 2)
							);
				} else {
					cacheInfoLen = sizeof(UMI_BSS_CACHE_INFO_IND)
					    + wfm_bss_cache_info_ind->IELength - sizeof(wfm_bss_cache_info_ind->IEElements);

					umi_bss_cache_info_ind = (UMI_BSS_CACHE_INFO_IND *) UMI_Allocate(cacheInfoLen);

					if (!umi_bss_cache_info_ind)
						UMI_OS_ASSERT(0);

					UMI_MemoryCopy(umi_bss_cache_info_ind, wfm_bss_cache_info_ind, cacheInfoLen);
				}

				/*
				   Replace the already present beacon with the
				   newly received one as it might be different
				   from the old one
				*/
				index = UMI_GetMacAddrIndex(pUmiCon->pScanListQueue, umi_bss_cache_info_ind->bssId);
				UMI_QueueInsertElementAtIndex(pUmiCon->pScanListQueue, umi_bss_cache_info_ind, index);
			}

			if (pUmiCon->inParams.ulCallbacks.scanInfo_Cb != NULL)
				pUmiCon->inParams.ulCallbacks.scanInfo_Cb(pUmiCon->inParams.ulHandle, cacheInfoLen, umi_bss_cache_info_ind);
			else
				UMI_ERROR(UMI_ProcUmacMsg, ("scanInfo_Cb Not defined\n"));
 Free_Beacon:		;
			UMAC_ReleaseMemory(pUmiCon->umacHandle, pHiMessage);

		}
		break;

	default:
		{
			/*
			* If an invalid MessageId is received then indicate a
			* General Failure Event as the UMI Client module
			* (i.e. The Miniport Driver) is expecting a response.
			*/
			UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ProcessMessages(): Invalid Event (pHiMessage->MessageId = 0x%04x)\n", pHiMessage->MessageId));

			if (pUmiCon->inParams.ulCallbacks.indicateEvent_Cb != NULL)
				pUmiCon->inParams.ulCallbacks.indicateEvent_Cb(pUmiCon->inParams.ulHandle, UMI_STATUS_FAILURE,	/* Should really have a UMI_STATUS_BAD_MESSAGE */
									       UMI_EVT_GENERAL_FAILURE, 0, NULL);
			else
				UMI_ERROR(UMI_ProcUmacMsg, ("indicateEvent_Cb Not defined\n"));
			break;
		}
	}	/* switch(pHiMessage->MessageId) */

 Exit_Func:;
	UMI_TRACE(UMI_ProcUmacMsg, ("<--- UMI_ProcessMessages()\n"));

	return 1;
}	/* End UMI_ProcessMessages() */

/******************************************************************************
 * NAME:	UMI_ScheduleUmac
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function calls upper layer to schedule UMAC.
 * \param umiHandle - Handle for this UMI instance.
 * \returns none.
 *****************************************************************************/
void UMI_ScheduleUmac(UMI_HANDLE umiHandle)
{
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;

	UMI_TRACE(UMI_ProcUmacMsg, ("---> UMI_ScheduleUmac()\n"));

	if (umiHandle == NULL) {
		UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ScheduleUmac(): Unexpected NULL umiHandle\n"));
		goto Exit_Func;

	}
	/* if (umiHandle == NULL) */
	if (NULL != pUmiCon->inParams.ulCallbacks.schedule_Cb)
		pUmiCon->inParams.ulCallbacks.schedule_Cb(pUmiCon->inParams.ulHandle);
	else
		UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ScheduleUmac(): Unexpected NULL Upper Layer Schedule Callback\n"));

 Exit_Func:;
	UMI_TRACE(UMI_ProcUmacMsg, ("<--- UMI_ScheduleUmac()\n"));
}

/******************************************************************************
 * NAME:	TransmitCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the Transmit Confirmation to the upper layer driver.
 * \param umiHandle - Handle for this UMI instance.
 * \param pTxCnf - Confirmation message.
 * \returns Status     1 : SUCCESS, 0 : FAILURE.
 *****************************************************************************/
uint8 TransmitCnf(UMI_HANDLE umiHandle, UMAC_TX_DATA_CNF *pTxCnf)
{
	UMI_CONNECTION *pUmiCon = NULL;
	UMI_TX_DATA *pTxData = NULL;

	pUmiCon = (UMI_CONNECTION *) umiHandle;

	pTxData = (UMI_TX_DATA *) pTxCnf->pUlHdr->Reference;

	/*Send Confirmation to Host Driver */
	pUmiCon->inParams.ulCallbacks.txComplete_Cb(
					pUmiCon->inParams.ulHandle,
					pTxCnf->pUlHdr->Status,
					pTxData
					);

	return 1;
}

/******************************************************************************
 * NAME:	TxMgmtFrmCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the Transmit Confirmation for Mangment frame to the
 * upper layer driver.
 * \param umiHandle - Handle for this UMI instance.
 * \param pTxCnf - Confirmation message.
 * \returns Status     1 : SUCCESS, 0 : FAILURE.
 *****************************************************************************/
uint8 TxMgmtFrmCnf(UMI_HANDLE umiHandle, UMAC_TX_MGMT_DATA_CNF *pTxMgmtDataCnf)
{
	UMI_CONNECTION *pUmiCon = NULL;
	UMI_TX_MGMT_FRAME_COMPLETED *pTxMgmtFrmCnf = NULL;

	pUmiCon = (UMI_CONNECTION *) umiHandle;

	pTxMgmtFrmCnf = (UMI_TX_MGMT_FRAME_COMPLETED *) pTxMgmtDataCnf;

	/*Send Confirmation to Host Driver */
	if (pUmiCon->inParams.ulCallbacks.txMgmtFrmComplete_Cb != NULL) {
		pUmiCon->inParams.ulCallbacks.txMgmtFrmComplete_Cb(
					pUmiCon->inParams.ulHandle,
					pTxMgmtFrmCnf
					);
	} else
		UMI_ERROR(UMI_ProcUmacMsg, ("txMgmtFrmComplete_Cb Not defined\n"));

	return 1;
}

/******************************************************************************
 * NAME:    UMI_ProcessRxFrames
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the received 802.3 frames to the upper layer driver.
 * \param umiHandle - Handle for this UMI instance.
 * \param Length - Length of the 802.3 frame.
 * \param pFrame - Received 802.3 frame.
 * \param pDriverInfo - Pointer to driver information.
 * \param pFrameStart - Start of the frame buffer.
 * \param flags - misc flags.
 * \returns Status     1 : SUCCESS, 0 : FAILURE.
 *****************************************************************************/
uint8 UMI_ProcessRxFrames(UMI_HANDLE umiHandle,
			  uint8 statusCode,
			  void *pFrame,
			  uint16 Length,
			  void *pDriverInfo,
			  void *pFrameStart,
			  uint32 flags)
{
	UMI_CONNECTION *pUmiCon = NULL;

	UMI_TRACE(UMI_ProcUmacMsg, ("---> UMI_ProcessRxFrames()\n"));

	if (umiHandle == NULL) {
		UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ProcessRxFrames(): Unexpected NULL umiHandle\n"));
		goto Exit_Func;

	}	/* if (umiHandle == NULL) */
	if ((pFrame == NULL) || (pFrameStart == NULL)) {
		UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ProcessRxFrames(): Unexpected NULL\n"));
		goto Exit_Func;
	}

	if (Length < 14) {
		UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ProcessRxFrames(): Frame too small\n"));
		goto Exit_Func;
	}

	if ((flags & 0x7fff) == 0) {
		UMI_ERROR(UMI_ProcUmacMsg, ("UMI_ProcessRxFrames(): Msdu count 0\n"));
		goto Exit_Func;
	}

	pUmiCon = (UMI_CONNECTION *) umiHandle;

	if (pUmiCon->inParams.ulCallbacks.dataReceived_Cb != NULL) {
		pUmiCon->inParams.ulCallbacks.dataReceived_Cb(
						pUmiCon->inParams.ulHandle,
						statusCode,
						Length,
						pFrame,
						pDriverInfo,
						pFrameStart,
						flags
						);
	} else {
		/*
		 * This condition shouldn't arise. If the callback disappaers
		 * in the middle of an A-MSDU delivery we face trouble.
		 */

		if ((flags & (1 << 15)) == 0)
			UMAC_ReleaseMemory(pUmiCon->umacHandle, pFrameStart);
	}

 Exit_Func:;
	UMI_TRACE(UMI_ProcUmacMsg, ("<--- UMI_ProcessRxFrames()\n"));
	return 1;
}

#if DOT11K_SUPPORT
/******************************************************************************
 * NAME:    UMI_BeaconInfo
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the beacon cache info that is requested based on bssid
 * \param umiHandle - Handle for this UMI instance.
 * \param pBssid - Pointer to BSSID.
 * \returns pointer to UMI_BSS_CACHE_INFO_IND structure.
 *****************************************************************************/
void *UMI_BeaconInfo(UMI_HANDLE umiHandle,
		     uint8 *pBssid,
		     uint8 bssidIndex,
		     uint32 *pBssidCount)
{
	uint8 buff[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	UMI_BSS_CACHE_INFO_IND *pBssCacheInfo = NULL;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;
	if (!UMI_MemoryCompare(pBssid, 6, &buff[0], 6)) {
		pBssCacheInfo = UMI_GetBssInfoMacAddr(
						pUmiCon->pScanListQueue,
						pBssid
						);
		*pBssidCount = 0;
	} else {
		/* Its a broadcast request.So return total number of elements
		and element at bssidIndex. If it is the first req then send the
		total no of beacons also */
		if (((*pBssidCount) == 0) && (bssidIndex == 0))
			*pBssidCount = UMI_QueueGetSize(
							pUmiCon->pScanListQueue
							);
		pBssCacheInfo = UMI_QueueReturnElementAtIndex(
						pUmiCon->pScanListQueue,
						bssidIndex
						);
	}
	return (void *)pBssCacheInfo;
}
#endif		/*DOT11K_SUPPORT */

