/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \mainpage WLAN HOST UMAC
 * \image html umac.png
 * \brief
    - <STRONG>Upper MAC Interface (UMI)</STRONG>: This is the interface layer
 * between host driver and Core Upper MAC. This will take the request from the
 * host driver and provide the information to Core Upper MAC. Similarly this
 * will pass the received information from Core Upper MAC to host driver.
    - <STRONG>Core Upper MAC (UMC)</STRONG>: This is main layer which will
 * maintain the UMAC state machine. This will accept the request received from
 * UMI layer and convert it into the corresponding MIBs/Requests of WSM
 * firmware. This will handle all the management frames and data frames. This
 * will send the entire MIBs, requests, management frames and data frames to
 * the DIL layer.
    - <STRONG>Device Interface Layer (DIL)</STRONG>: The responsibility of this
 * layer is to collect all the MIBs, requests, management frames and data
 * frames in to a single queue and inform to the lower layer (i.e. WDEV) for
 * frame availability to send it to WSM firmware. This will also receive all
 * the MIBs confirmation, requests confirmation, management responses, data
 * frames and events from WSM firmware and will provide this to Core Upper MAC
 * for processing.
 */
/**
 * \addtogroup Upper_MAC_Interface
 * \brief
 *
 */
/**
 * \file UMI_Api.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_Api.c
 * \brief
 * Implementation of UMI API module. This file implements the  interface as
 * described in
 * WLAN_Host_UMAC_API.doc
 * \ingroup Upper_MAC_Interface
 * \date 19/12/08 11:37
 */

/*****************************************************************************/
/********		       Include Files			      ********/
/*****************************************************************************/

#include "UMI_Api.h"
#include "UMI_ApiInternal.h"
#include "UMI_DebugTrace.h"
#include "UMI_Queue.h"
#include "umac_messages.h"
#include "umac_if.h"
#include "UMI_ProcUmacMsg.h"
#include "UMI_Utility.h"
#include "UMI_UmacDil.h"
#include "WHUM_Version.h"
/*#include "umac_dbg.h" */

#define UMI_HDR_START_OFFSET  (EXTRA_MEMORY_SIZE - sizeof(UMAC_TX_REQ_HDR))

/*****************************************************************************/
/********	  Internally Visible Static Data		      ********/
/*****************************************************************************/

/*****************************************************************************/
/********	   Internally Visible Functions			      ********/
/*****************************************************************************/

/******************************************************************************
 * NAME:	UMI_Create
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the first API that needs to be called by the Host device.It creates
 * a UMI object and initializes the UMI module.
 * \param pIn     -   A structure that contains all the input parameters to
 *                    this function.
 * \param pOut    -   A structure that contains all the output parameters to
 *                    this function.
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/

UMI_STATUS_CODE UMI_Create(UMI_CREATE_IN *pIn, UMI_CREATE_OUT *pOut)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	UMI_CONNECTION *pUmiCon = NULL;
	UMAC_TO_LL_CALLS llCallBacks = { 0 };

	/*Initlize Debug Settings */
	UMI_DebugInit();

	UMI_TRACE(UMI_Api, ("---> UMI_Create()\n"));

	UMI_DbgPrint("%s, %s, %s\n", WHUM_SwTitle, __DATE__, __TIME__);

	/* Verify paramaters */
	if (pIn == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_Create(): Unexpected NULL pIn\n"));
		statusCode = UMI_STATUS_BAD_PARAM;

		UMI_TRACE(UMI_Api, ("<--- UMI_Create() (0x%08x)\n", statusCode));
		return statusCode;
	}

	if (pOut == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_Create(): Unexpected NULL pOut\n"));
		statusCode = UMI_STATUS_BAD_PARAM;

		UMI_TRACE(UMI_Api, ("<--- UMI_Create() (0x%08x)\n", statusCode));
		return statusCode;
	}

	UMI_DEFAULT(UMI_Api, ("UMI_Create(): pIn->ApiVersion = %d\n", pIn->apiVersion));
	UMI_DEFAULT(UMI_Api, ("UMI_Create(): pIn->Flags      = %d\n", pIn->flags));

	pUmiCon = (UMI_CONNECTION *) UMI_Allocate(sizeof(UMI_CONNECTION));

	if (pUmiCon) {
		UMI_MemoryReset(pUmiCon, sizeof(UMI_CONNECTION));

		/* Stored input paramaters received from host driver
		in UMI object */
		UMI_MemoryCopy(&pUmiCon->inParams, pIn, sizeof(UMI_CREATE_IN));

		/* Indicates that the basic state has been initi alized */
		INITIALIZE_UMI_STATE(pUmiCon);

		/* Initializing the scan lists */
		pUmiCon->pScanListQueue = UMI_InitScanList();

		pUmiCon->pScanListOldQueue = UMI_InitScanList();
		/* Registerd lower layer callbacks */
		llCallBacks.LL_RX_COMPLETE = pUmiCon->inParams.llCallbacks.rxComplete_Cb;
		llCallBacks.LL_TX_FN = pUmiCon->inParams.llCallbacks.scheduleTx_Cb;

		/* Create UMAC Handle */
		pUmiCon->umacHandle = UMAC_Create(pUmiCon, &llCallBacks, NULL);

		if (pUmiCon->umacHandle == NULL) {
			statusCode = UMI_STATUS_FAILURE;
			UMI_Free(pUmiCon);
			pOut->umiHandle = NULL;
			pOut->mtu = 0;
			return statusCode;
		}

		/* Create Lower layer driver instance */
		pUmiCon->llHandle = pUmiCon->inParams.llCallbacks.create_Cb(
						pUmiCon->umacHandle,
						pUmiCon->inParams.ulHandle
						);

		/* Verify handle */
		if (pUmiCon->llHandle == NULL) {
			statusCode = UMI_STATUS_FAILURE;
			UMI_Free(pUmiCon);
			pOut->umiHandle = NULL;
			pOut->mtu = 0;
			return statusCode;
		}

		pUmiCon->mtu = ETH_MAX_PACKET_SIZE;

		/* Fill output paramaters */
		pOut->umiHandle = (void *)pUmiCon;
		pOut->mtu = pUmiCon->mtu;
		pOut->llHandle = pUmiCon->llHandle;

		pUmiCon->infraMode = UMI_802_AUTO_UNKNOWN;

	} /* if(pUmiCon) */
	else
		statusCode = UMI_STATUS_OUT_OF_RESOURCES;

	UMI_TRACE(UMI_Api, ("<--- UMI_Create() (0x%08x)\n", statusCode));

	return statusCode;

}	/* End UMI_Create() */

/******************************************************************************
 * NAME:	UMI_Start
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This needs to be called by the Host device.This function starts the UMI
 * module. The caller should wait for UL_CB::IndicateEvent() to return an event
 * that indicates the start operation is completed.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param pStart      -   Pointer to the start-request parameters.
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Start(UMI_HANDLE umiHandle, UMI_START_T *pStart)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_FAILURE;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;

	UMI_TRACE(UMI_Api, ("---> UMI_Start()\n"));

	/* Verify Paramaters */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_Start(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		UMI_TRACE(UMI_Api, ("<--- UMI_Start() (0x%08x)\n", statusCode));
		return statusCode;
	}	/* if (umiHandle == NULL) */
	if (pStart == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_Start(): Unexpected NULL pStart\n"));
		statusCode = UMI_STATUS_BAD_PARAM;

		if (statusCode != UMI_STATUS_SUCCESS) {
			UMI_ERROR(UMI_Api, ("UMI_START(): Start Failed\n"));
			SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_START_FAILED);
		}	/* if (statusCode != UMI_STATUS_SUCCESS) */
		UMI_TRACE(UMI_Api, ("<--- UMI_Start() (0x%08x)\n", statusCode));

		return statusCode;
	}	/* if (pStart == NULL) */
	UMI_DEFAULT(UMI_Api, ("UMI_START(): pUmiCon->InParams.ApiVersion = %d\n", pUmiCon->inParams.apiVersion));

	/* Verify UMI State Machine */
	if (!(IS_UMI_STATE(pUmiCon, UMI_STATE_INITIALIZED)
	      || IS_UMI_STATE(pUmiCon, UMI_STATE_STOPPED))
	    ) {
		UMI_ERROR(UMI_Api, ("UMI_START(): UMI Unexpected State (0x%08x)\n", pUmiCon->devUmiState));
		statusCode = UMI_STATUS_FAILURE;

		if (statusCode != UMI_STATUS_SUCCESS) {
			UMI_ERROR(UMI_Api, ("UMI_START(): Start Failed\n"));
			SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_START_FAILED);
		}

		UMI_TRACE(UMI_Api, ("<--- UMI_Start() (0x%08x)\n", statusCode));

		return statusCode;
	}

	SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_STARTING);

	/* Start UMAC */
	statusCode = UMAC_Init(
				pUmiCon->umacHandle,
				pUmiCon->llHandle,
				UMI_ProcessMessages,
				UMI_ProcessRxFrames,
#if DOT11K_SUPPORT
				UMI_BeaconInfo,
#endif
				TransmitCnf,
				TxMgmtFrmCnf,
				pUmiCon->inParams.ulCallbacks.schedule_Cb,
				pUmiCon->inParams.ulHandle
				);

	if (statusCode != UMI_STATUS_SUCCESS) {
		UMI_ERROR(UMI_Api, ("UMI_Start(): UMAC Initlization Failed\n"));
		UMI_ERROR(UMI_Api, ("UMI_START(): Start Failed\n"));
		SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_START_FAILED);

		UMI_TRACE(UMI_Api, ("<--- UMI_Start() (0x%08x)\n", statusCode));

		return statusCode;
	}	/* if (statusCode != UMI_STATUS_SUCCESS) */

	/* Start Lower Layer Driver */
	statusCode = pUmiCon->inParams.llCallbacks.start_Cb(
							pUmiCon->llHandle,
							pStart->fmLength,
							pStart->pFirmware
							);

	if (statusCode != UMI_STATUS_SUCCESS) {
		UMI_ERROR(UMI_Api, ("UMI_START(): Start Failed\n"));
		SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_START_FAILED);

		UMI_TRACE(UMI_Api, ("<--- UMI_Start() (0x%08x)\n", statusCode));

		return statusCode;
	}	/* if (statusCode != UMI_STATUS_SUCCESS) */
	UMI_TRACE(UMI_Api, ("<--- UMI_Start() (0x%08x)\n", statusCode));

	return statusCode;
}	/* End UMI_Start() */

/******************************************************************************
 * NAME:	UMI_Destroy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function deletes a UMI object and free all resources owned by this
 * object. The caller should call UMI_Stop() and wait for UMI returning a
 * stop-event before removing the UMI object.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Destroy(UMI_HANDLE umiHandle)
{
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;
	UMI_STATUS_CODE umiStatus = UMI_STATUS_FAILURE;

	UMI_TRACE(UMI_Api, ("---> UMI_Destroy()\n"));

	/* Verify Paramaters */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_Destroy(): Unexpected NULL umiHandle\n"));
		goto Exit_Func;
	}	/* if (UMIHandle == NULL ) */
	switch (pUmiCon->devUmiState) {
	case UMI_STATE_INITIALIZED:
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_INITIALIZED\n"));
		goto Remove_UMI;

	case UMI_STATE_STARTING:
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_STARTING\n"));

		if (IS_UMI_STATE(pUmiCon, UMI_STATE_STARTED))
			goto Case_Started;

		if (IS_UMI_STATE(pUmiCon, UMI_STATE_START_FAILED))
			goto Case_Start_Failed;

		goto Remove_UMI;

	case UMI_STATE_STARTED:
 Case_Started:	;
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_STARTED - Stopping\n"));

		umiStatus = UMI_Stop(umiHandle);
		if (UMI_STATUS_PENDING == umiStatus)
			goto Case_Stop_Pending;

		if (IS_UMI_STATE(pUmiCon, UMI_STATE_STOPPED))
			goto Case_Stopped;

		goto Remove_UMI;

	case UMI_STATE_START_FAILED:
 Case_Start_Failed:;
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_START_FAILED - Destroying\n"));

		goto Remove_UMI;

	case UMI_STATE_STOP_PENDING:
 Case_Stop_Pending:;
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_STOP_PENDING - Stopping\n"));

		goto Case_Stopped;

	case UMI_STATE_STOPPED:
 Case_Stopped:	;
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_STOPPED - Removing\n"));

		goto Remove_UMI;

	case UMI_STATE_SUSPENDED:
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_SUSPENDED - Removing\n"));

		goto Remove_UMI;

	case UMI_STATE_SURPRISE_REMOVE:
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_SUPRISE_REMOVE - Removing\n"));

		goto Remove_UMI;

	case UMI_STATE_REMOVE_PENDING:
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_REMOVE_PENDING - (IGNORING)\n"));

		umiStatus = UMI_STATUS_PENDING;
		goto Exit_Func;

	case UMI_STATE_REMOVED:
		UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): UMI_STATE_REMOVED\n"));

		umiStatus = UMI_STATUS_SUCCESS;
		goto Exit_Func;

	default:
		UMI_ERROR(UMI_Api, ("UMI_Destroy(): Unknown state (0x%08x)\n", pUmiCon->devUmiState));
		goto Exit_Func;
	}	/* switch (pUMICon->devUmiState) */

 Remove_UMI:;
	UMI_DEFAULT(UMI_Api, ("UMI_Destroy(): Removing UMI\n"));

	SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_REMOVE_PENDING);

	/* Destroy UMAC */
	umiStatus = pUmiCon->inParams.llCallbacks.destroy_Cb(pUmiCon->llHandle);
	if (umiStatus != UMI_STATUS_SUCCESS)
		UMI_ERROR(UMI_Api, ("UMI_Destroy(): Lower Layer Driver Destroy() Failed\n"));

	umiStatus = UMAC_Distroy(pUmiCon->umacHandle);
	if (umiStatus != UMI_STATUS_SUCCESS) {
		UMI_ERROR(UMI_Api, ("UMI_Destroy(): UMAC Destroy() Failed\n"));
		goto Exit_Func;
	}	/* if (umiStatus != UMI_STATUS_SUCCESS) */

	/* De-Initializing the scan Lists */
	UMI_DeInitScanList(pUmiCon->pScanListQueue);

	UMI_DeInitScanList(pUmiCon->pScanListOldQueue);

	SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_REMOVED);

	if (UMI_STATUS_SUCCESS == umiStatus)
		UMI_Free((void *)pUmiCon);
	else {
		/* This is a non-existing connection */
		UMI_ERROR(UMI_Api, ("UMI_Destroy(): Failed to Destroy UMI\n"));
		goto Exit_Func;
	}	/* if (UMI_STATUS_SUCCESS == umiStatus) */

	umiStatus = UMI_STATUS_SUCCESS;

 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_Destroy() (0x%08x)\n", umiStatus));

	return umiStatus;
}	/* end UMI_Destroy() */

/******************************************************************************
 * NAME:	UMI_Stop
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to stop the UMI module. The caller should wait for
 * UMI_CB::IndicateEvent() to return an event that indicates the stop operation
 * is completed.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Stop(UMI_HANDLE umiHandle)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;

	UMI_TRACE(UMI_Api, ("---> UMI_Stop()\n"));

	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_Stop(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}	/* if (umiHandle == NULL) */
	if (!IS_UMI_STATE(pUmiCon, UMI_STATE_STARTED)) {
		UMI_ERROR(UMI_Api, ("UMI_Stop(): Unexpected State (0x%08x)\n", pUmiCon->devUmiState));
		statusCode = UMI_STATUS_FAILURE;
		goto Exit_Func;
	}	/* if (!IS_UMI_STATE( pUmiCon, UMI_STATE_STARTED)) */
	SET_NEW_UMI_STATE(pUmiCon, UMI_STATE_STOP_PENDING);

	/*Stop UMAC */
	UMI_ClearScanList(pUmiCon->pScanListQueue);
	pUmiCon->encryptionStatus = 0;
	statusCode = UMAC_Stop(pUmiCon->umacHandle);

	if (statusCode != UMI_STATUS_SUCCESS)
		UMI_ERROR(UMI_Api, ("UMI_Stop(): Failed to Stop UMAC (0x%08x)\n", statusCode));

	statusCode = UMI_STATUS_PENDING;

 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_Stop() (0x%08x)\n", statusCode));

	return statusCode;

}	/* End UMI_Stop() */

/******************************************************************************
 * NAME:	UMI_Transmit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function indicates to the UMI module the driver has 802.3 frames to be
 * sent to the device.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param priority    -   Priority of the frame.
 * \param flags       -   b0-b3 : Link ID
 *			  b4-b30: Reserved
 *			  b31   :set indicates more frames waiting to be
 *                               serviced
 * \param pTxData     -   pointer to the structure which
 *			 include frame to transmit.
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Transmit(UMI_HANDLE umiHandle,
			     uint8 priority,
			     uint32 flags,
			     UMI_TX_DATA *pTxData)
{
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;
	UMI_STATUS_CODE statusCode;
	UMAC_TX_DATA *pUmacTxData;

	UMI_TRACE(UMI_Api, ("---> UMI_Transmit()\n"));

	/* Verify Paramaters */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_Transmit(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}	/* if (umiHandle == NULL) */

	pUmacTxData = (UMAC_TX_DATA *) pTxData->pExtraBuffer;

	pUmacTxData->MsgId = 0x04;

	pUmacTxData->pEtherHeader = pTxData->pEthHeader;
	pUmacTxData->pEtherPayLoad = pTxData->pEthPayloadStart;
	pUmacTxData->UlInfo = pTxData->pDriverInfo;

	/* Add UMI HDR on last 12 bytes of Extra Buffer */
	pUmacTxData->pUlHdr = (UMAC_TX_REQ_HDR *) ((uint8 *) (pTxData->pExtraBuffer)
						   + UMI_HDR_START_OFFSET);

	/* Size of UMAC_TX_REQ_HDR + size of Ethernet HDR
	+ Ethernet Payload Length */
	pUmacTxData->pUlHdr->MessageLength = sizeof(UMAC_TX_REQ_HDR)
	    + 14 + (uint16) pTxData->ethPayloadLen;

	pUmacTxData->pUlHdr->Priority = (uint32) priority;

	pUmacTxData->pUlHdr->Flags = (uint32) flags;

	pUmacTxData->pUlHdr->Reference = (uint32) pTxData;

	/* Transmit frame to UMAC module */
	statusCode = UMAC_IF_TX_REQ(
				pUmiCon->umacHandle,
				(WFM_HI_MESSAGE *) pUmacTxData
				);
 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_Transmit() (0x%08x)\n", statusCode));

	return statusCode;
}	/* End UMI_Transmit() */

/******************************************************************************
 * NAME:	UMI_GetParameter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function request to get a parameter from the UMI module or the WLAN
 * device. The caller should wait for UL_CB::GetParameterComplete() to retrieve
 * the data.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param oid	 -   The OID number that is associated with the parameter.
 * \param flag	-   Flag field (b0-b7: Link Id)
 * \returns UMI_GET_PARAM_STATUS If the Status filled in UMI_GET_PARAM_STATUS
 *		       structure is set to UMI_STATUS_SUCCESS than caller
 *		       will get parameter data in pValue filed. If Status
 *		       filled is set to UMI_STATUS_PENDING than caller
 *		       should wait for UL_CB::GetParameterComplete() to
 *		       retrieve the data.
 *****************************************************************************/
UMI_GET_PARAM_STATUS *UMI_GetParameter(UMI_HANDLE umiHandle,
				       uint16 oid,
				       uint16 flag)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	WFM_HI_MESSAGE *pHiMessage = NULL;
	WFM_HI_PAYLOAD *pHiPayload = NULL;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;
	UMI_GET_PARAM_STATUS *pGetParamStatus = NULL;
	UMI_BSS_CACHE_INFO_IND_LIST *pBssCacheInfoList = NULL;
	UMI_BSS_CACHE_INFO_IND *pBssCacheInfoInd = NULL;
	UMI_BSS_CACHE_INFO_IND *pBssIndList = NULL;
	uint32 count = 0;
	uint32 scanListQueueSize = 0;
	uint32 numberOfItems = 0;
	uint32 size = 0;
	UMI_Q *pScanList = NULL;

	UMI_TRACE(UMI_Api, ("---> UMI_GetParameter()\n"));
	UMI_TRACE(UMI_Api, ("<--- UMI_GetParameter() OID: (0x%08x)\n", oid));

	/* Verify UMI Handle */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_GetParameter(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* Verify Current State */
	if (!IS_UMI_STATE(pUmiCon, UMI_STATE_STARTED)) {
		UMI_ERROR(UMI_Api, ("UMI_GetParameter(): Unexpected STATE (0x%08x)\n", pUmiCon->devUmiState));
		statusCode = UMI_STATUS_FAILURE;
		goto Exit_Func;
	}

	pGetParamStatus = (UMI_GET_PARAM_STATUS *) UMI_Allocate(sizeof(UMI_GET_PARAM_STATUS));

	if (!pGetParamStatus) {
		UMI_ERROR(UMI_Api, ("UMI_GetParameter(): Failed to allocate memory for Get Paramater Status"));
		goto Exit_Func;
	}

	UMI_MemoryReset(pGetParamStatus, sizeof(UMI_GET_PARAM_STATUS));

	switch (oid) {
	case UMI_DEVICE_OID_802_11_BSSID_LIST:
		{
			UMI_Q_ITEM *pCurrentElement;
			pGetParamStatus->oid = UMI_DEVICE_OID_802_11_BSSID_LIST;
			pGetParamStatus->status = UMI_STATUS_SUCCESS;

			/* Store size of pBssCacheInfoList->numberOfItems */
			scanListQueueSize = sizeof(uint32);

			numberOfItems = UMI_QueueGetSize(pUmiCon->pScanListOldQueue);
			if (numberOfItems > 0) {
				/*Scan completed event has not yet been
				recieved. So merge the queues */
				UMI_MergeQueues(pUmiCon->pScanListOldQueue, pUmiCon->pScanListQueue);
				pScanList = pUmiCon->pScanListOldQueue;
			} else
				pScanList = pUmiCon->pScanListQueue;

			numberOfItems = UMI_QueueGetSize(pScanList);
			/*If scan list is not empty */
			if (numberOfItems > 0) {
				pCurrentElement = pScanList->Head;
				while ((count < numberOfItems) && (pCurrentElement != NULL)) {
					pBssCacheInfoInd = pCurrentElement->Pkt;

					if (!pBssCacheInfoInd) {
						count++;
						pCurrentElement = pCurrentElement->Next;
						continue;
					}

					scanListQueueSize += sizeof(UMI_BSS_CACHE_INFO_IND)
					    - sizeof(pBssCacheInfoInd->ieElements)
					    + pBssCacheInfoInd->ieLength;

					count++;
					pCurrentElement = pCurrentElement->Next;
				}

				/* Allocate pBssCacheInfoList with complete
				queue size */
				pBssCacheInfoList = (UMI_BSS_CACHE_INFO_IND_LIST *) UMI_Allocate(scanListQueueSize);

				if (!pBssCacheInfoList) {
					UMI_ERROR(UMI_Api, ("UMI_GetParameter(): Failed to allocate memory for BSS Cache Info Indication List"));
					UMI_Free(pGetParamStatus);
					pGetParamStatus = NULL;
					goto Exit_Func;
				}

				pBssCacheInfoList->numberOfItems = numberOfItems;

				pBssIndList = pBssCacheInfoList->bssCacheInfoInd;

				/*Store Beacon/Probe Resp from queue to
				pBssCacheInfoList for host */
				for (count = 0; count < numberOfItems; count++) {
					pBssCacheInfoInd = (UMI_BSS_CACHE_INFO_IND *) UMI_QueueReturnElementAtIndex(pScanList, count);

					size = sizeof(UMI_BSS_CACHE_INFO_IND) - sizeof(pBssCacheInfoInd->ieElements) + pBssCacheInfoInd->ieLength;

					UMI_MemoryCopy(pBssIndList, pBssCacheInfoInd, size);

					pBssIndList = ((UMI_BSS_CACHE_INFO_IND *) (((uint8 *) pBssIndList) + size));
				}

				pGetParamStatus->length = (uint16) scanListQueueSize;
				pGetParamStatus->pValue = pBssCacheInfoList;
			} else {
				pGetParamStatus->length = 0;
				pGetParamStatus->pValue = NULL;
			}
		}

		goto Exit_Func;
	}

	pHiMessage = (WFM_HI_MESSAGE *) UMI_Allocate(sizeof(WFM_HI_MESSAGE));
	if (!pHiMessage) {
		UMI_ERROR(UMI_Api, ("UMI_GetParameter(): Failed to allocate memory for HI Message"));
		UMI_Free(pGetParamStatus);
		pGetParamStatus = NULL;
		goto Exit_Func;
	}

	UMI_MemoryReset(pHiMessage, sizeof(WFM_HI_MESSAGE));

	/* Message length here is the length of HI Message Header
	   + size of the total Get Parameter Req Structure */

	pHiMessage->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + WFM_GET_PARAM_REQ_HDR_SIZE;

	pHiMessage->MessageId = WFM_GET_PARAM_REQ_ID;

#if P2P_MINIAP_SUPPORT
	pHiMessage->MessageId |= (flag & 0x0F) << 6;
#endif	/* P2P_MINIAP_SUPPORT */

	pHiPayload = &pHiMessage->PayLoad;

	pHiPayload->wfm_get_param_req.ParameterId = (UMI_DEVICE_OID) oid;

	/*
	   This field is not relevent now,Its been introduced to keep
	   memory alligned to 32 bit boundary. So assigning Zero.
	 */
	pHiPayload->wfm_get_param_req.ElementIndex = 0;

	/* Sending HiMessage to UMAC module */
	statusCode = UMAC_IF_GET_PARAM_REQ(pUmiCon->umacHandle, pHiMessage);

	if (statusCode != UMI_STATUS_PENDING) {
		pGetParamStatus->length = pHiMessage->PayLoad.wfm_get_param_cnf.ParameterLength;
		pGetParamStatus->oid = pHiMessage->PayLoad.wfm_get_param_cnf.ParameterId;
		pGetParamStatus->status = pHiMessage->PayLoad.wfm_get_param_cnf.Status;
		if (pGetParamStatus->length) {
			pGetParamStatus->pValue = (void *)UMI_Allocate(pHiMessage->PayLoad.wfm_get_param_cnf.ParameterLength);
			if (!(pGetParamStatus->pValue)) {
				UMI_ERROR(UMI_Api, ("UMI_GetParameter(): Failed to allocate memory for Paramater Value"));
				UMI_Free(pGetParamStatus);
				pGetParamStatus = NULL;
				goto Exit_Func;
			}
				UMI_MemoryCopy(pGetParamStatus->pValue, &pHiMessage->PayLoad.wfm_get_param_cnf.ParameterData, pHiMessage->PayLoad.wfm_get_param_cnf.ParameterLength);
		}
		UMI_Free(pHiMessage);
	} else if (statusCode == UMI_STATUS_PENDING) {
		pGetParamStatus->oid = pHiPayload->wfm_get_param_req.ParameterId;
		pGetParamStatus->status = UMI_STATUS_PENDING;
	}

 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_GetParameter() (0x%08x)\n", statusCode));

	return pGetParamStatus;
}	/* End UMI_GetParameter() */

/******************************************************************************
 * NAME:	UMI_SetParameter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function request to set a parameter to the UMI module or the WLAN
 * device. This API is used to issue WLAN commands to the device.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param oid	 -   The OID number that is associated with the parameter.
 * \param length      -   Number of bytes in the data.
 * \param flag	 -   Flag field (b0-b7: Link Id).
 * \param pValue      -   The parameter data
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_SetParameter(UMI_HANDLE umiHandle,
				 uint16 oid,
				 uint16 length,
				 uint16 flag,
				 void *pValue)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	WFM_BSS_INFO *pBssInfo = NULL;
	UMI_BSS_CACHE_INFO_IND *pBssCacheInfoInd = NULL;
	WFM_WLAN_PARAMETER_INFO *pParamInfo = NULL;
	WFM_HI_MESSAGE *pHiMessage = NULL;
	WFM_HI_PAYLOAD *pHiPayload = NULL;
	uint16 paramDataLen = 0;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;
	UMI_802_11_SSID *pSsid = NULL;
	UMI_BLOCK_ACK_POLICY *pBlockAckPolicy = NULL;
	UMI_DOT11_COUNTRY_STRING *pCountryInfo = NULL;
	UMI_UPDATE_EPTA_CONFIG_DATA *pEptaConfigData = NULL;
	UMI_OID_802_11_CONFIGURE_IBSS *pConfigureIBSS = NULL;
	UMI_OID_802_11_SET_UAPSD *pSetUAPSD = NULL;
	UMI_RCPI_RSSI_THRESHOLD *pRcpiRssiThreshold = NULL;
	UMI_BEACON_LOST_COUNT *pBeaconLostCount = NULL;
	UMI_TX_FAILURE_THRESHOLD_COUNT *pTxFailureCount = NULL;
	UMI_BSSID_PMKID_BKID *pBssidPmkidBkid = NULL;
	UMI_SET_ETHER_TYPE_FILTER *pSetEtherTypeFilter = NULL;
	UMI_SET_UDP_PORT_FILTER *pSetUdpPortFilter = NULL;
	UMI_SET_MAGIC_FRAME_FILTER *pSetMagicFrameFilter = NULL;
#if P2P_MINIAP_SUPPORT
	UMI_OID_P2P_SOCIAL_CHANNEL *pSocialChannels = NULL;
	UMI_OID_P2P_DEVICE_DISCOVERY *pDeviceDiscovery = NULL;
	UMI_OID_802_11_P2P_PS_MODE *p2pPsMode = NULL;
#endif	/*P2P_MINIAP_SUPPORT */

	UMI_TRACE(UMI_Api, ("---> UMI_SetParameter()\n"));

	/* Verify UMI Handle */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_SetParameter(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* Check Current State of UMI module */
	if (!IS_UMI_STATE(pUmiCon, UMI_STATE_STARTED)) {
		UMI_ERROR(UMI_Api, ("UMI_SetParameter(): Unexpected STATE (0x%08x)\n", pUmiCon->devUmiState));
		statusCode = UMI_STATUS_FAILURE;
		goto Exit_Func;
	}

	pHiMessage = (WFM_HI_MESSAGE *) UMI_Allocate(sizeof(WFM_HI_MESSAGE));
	if (pHiMessage == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_SetParameter(): Failed to allocate memory for HI Message\n"));
		statusCode = UMI_STATUS_OUT_OF_RESOURCES;
		goto Exit_Func;
	}

	UMI_MemoryReset(pHiMessage, sizeof(WFM_HI_MESSAGE));

	/* Message length is the size of HI HDR+ SET_PARAM HDR + Payload size */
	pHiMessage->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + WFM_SET_PARAM_REQ_HDR_SIZE + length;

	pHiMessage->MessageId = WFM_SET_PARAM_REQ_ID;

	pHiPayload = &pHiMessage->PayLoad;

	pHiPayload->wfm_set_param_req.ParameterId = (UMI_DEVICE_OID) oid;

	pParamInfo = &pHiPayload->wfm_set_param_req.ParameterData;

	/* Only the OID's on which set operation is valid, is listed here */
	switch (oid) {
	case UMI_DEVICE_OID_802_11_COUNTRY_STRING:
		{
			paramDataLen = sizeof(UMI_DOT11_COUNTRY_STRING);
			if (pValue) {
				pCountryInfo = (UMI_DOT11_COUNTRY_STRING *) pValue;
				OS_MemoryCopy((void *)&pParamInfo->CountryStringInfo, (void *)pValue, paramDataLen);
			}
		}
		break;
	case UMI_DEVICE_OID_802_11_BSSID_LIST_SCAN:
		{
			UMI_BSS_LIST_SCAN *pBssListScan = NULL;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_BSSID_LIST_SCAN\n"));

			/*
			* As per Windows specification No data is associated
			* with this OID But giving an option for driver to give
			* the structure WFM_BSS_LIST_SCAN as input, for more
			* flexibility.
			*/

			if (pValue) {
				pBssListScan = (UMI_BSS_LIST_SCAN *) ((uint8 *) pValue);
				UMI_MemoryCopy((void *)&pParamInfo->bssListScan, (void *)pBssListScan, sizeof(UMI_BSS_LIST_SCAN) - sizeof(UMI_CHANNELS) + (pBssListScan->NumberOfChannels_2_4Ghz + pBssListScan->NumberOfChannels_5Ghz) * sizeof(UMI_CHANNELS));
				paramDataLen = sizeof(UMI_BSS_LIST_SCAN) - sizeof(UMI_CHANNELS) + (pBssListScan->NumberOfChannels_2_4Ghz + pBssListScan->NumberOfChannels_5Ghz) * sizeof(UMI_CHANNELS);
			} else {
				/* Following values are hard coded for the
				time being */
				pParamInfo->bssListScan.Flags = WFM_BSS_LIST_SCAN_2_4G_BAND;

				pParamInfo->bssListScan.Flags = (pParamInfo->bssListScan.Flags | WFM_BSS_LIST_NO_ACTIVE_SCAN);

				/*In this version, we are using broadcast prop requests */
				pParamInfo->bssListScan.NumOfProbeRequests = 0;
				pParamInfo->bssListScan.NumOfSSIDs = 0;
				OS_MemoryReset(&pParamInfo->bssListScan.Ssids, sizeof(UMI_802_11_SSID));
				pParamInfo->bssListScan.NumberOfChannels_2_4Ghz = 0;
				pParamInfo->bssListScan.NumberOfChannels_5Ghz = 0;
				paramDataLen = sizeof(UMI_BSS_LIST_SCAN);
			}	/* if (pValue) */
		}
		break;

	case UMI_DEVICE_OID_802_11_BSSID:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_BSSID\n"));

		/*
		   Search the stored BSS_LIST and load the UMI_BSS_INFO
		   structure corresponding  to this MAC Address.
		   In this case, pValue contains the MAC address as per
		   the Windows DDK documentation.
		 */
		pBssCacheInfoInd = UMI_GetBssInfoMacAddr(pUmiCon->pScanListQueue, (uint8 *) pValue);

		/* Convert UMI_BSS_CACHE_INFO_IND to UMI_BSS_INFO */
		pBssInfo = UMI_GetBssInfo(umiHandle, pBssCacheInfoInd);

		if (pBssInfo) {

			/*
			   Need to add the length of number of bytes here. 4
			   bytes are reserved for Supported rates already,
			   so have to substract 4 and add the number of rates.
			 */
			pBssInfo->RoamingEnabled = ((UMI_BSSID_OID *) (pValue))->RoamingEnable;
			paramDataLen = (sizeof(WFM_BSS_INFO) - WFM_BSS_INFO_SUPPORTED_RATE_SIZE)
			    + pBssInfo->NumRates;

			UMI_MemoryCopy(&pParamInfo->BssInfo, pBssInfo, paramDataLen);
			UMI_Free(pBssInfo);
		} /* if (pBssInfo) */
		else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): No stored device with the given MAC Address\n"));
			statusCode = UMI_STATUS_BAD_PARAM;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		} /* if (pBssInfo) */

		break;

	case UMI_DEVICE_OID_802_11_ADD_KEY:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_ADD_KEY\n"));

		if (pValue) {
			OS_MemoryCopy((void *)&pParamInfo->dot11AddKey, (void *)pValue, sizeof(WFM_OID_802_11_KEY)
			    );
			paramDataLen = sizeof(WFM_OID_802_11_KEY);
#if P2P_MINIAP_SUPPORT
			pHiMessage->MessageId |= (flag & 0x0F) << 6;
#endif	/*P2P_MINIAP_SUPPORT */

		}

		break;

	case UMI_DEVICE_OID_802_11_AUTHENTICATION_MODE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_AUTHENTICATION_MODE\n"));

		if (pValue) {
			pParamInfo->dot11AuthenticationMode = (*((uint8 *) pValue));

			/* Authentication mode consumes only 1 byte */
			paramDataLen = 1;
#if P2P_MINIAP_SUPPORT
			pHiMessage->MessageId |= (flag & 0x0F) << 6;
#endif	/*P2P_MINIAP_SUPPORT */
		}

		break;

	case UMI_DEVICE_OID_802_11_INFRASTRUCTURE_MODE:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_INFRASTRUCTURE_MODE\n"));

			if (pValue != NULL) {
				pParamInfo->dot11InfraStructureMode = (*((uint32 *) pValue));
				/* Infrastructure mode consumes only 4 bytes */
				paramDataLen = sizeof(pParamInfo->dot11InfraStructureMode);
				pUmiCon->infraMode = pParamInfo->dot11InfraStructureMode;
			} /* if (pValue != NULL) */
			else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
				statusCode = UMI_STATUS_FAILURE;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			} /* if (pValue != NULL) */

			break;
		}

	case UMI_DEVICE_OID_802_11_REMOVE_KEY:
		{
			if (pValue) {
				UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_REMOVE_KEY\n"));
				OS_MemoryCopy((void *)&pParamInfo->dot11RemoveKey, (void *)pValue, sizeof(WFM_OID_802_11_REMOVE_KEY)
				    );

				paramDataLen = sizeof(WFM_OID_802_11_REMOVE_KEY);
#if P2P_MINIAP_SUPPORT
				pHiMessage->MessageId |= (flag & 0x0F) << 6;
#endif	/*P2P_MINIAP_SUPPORT */

			}
		}
		break;

	case UMI_DEVICE_OID_802_11_DISASSOCIATE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_DISASSOCIATE\n"));
		pHiMessage->MessageId |= (flag & 0x0F) << 6;
		break;

	case UMI_DEVICE_OID_802_11_RSSI_TRIGGER:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_RSSI_TRIGGER\n"));
		break;

	case UMI_DEVICE_OID_802_11_MEDIA_STREAM_MODE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_MEDIA_STREAM_MODE\n"));
		break;

	case UMI_DEVICE_OID_802_11_TX_ANTENNA_SELECTED:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_TX_ANTENNA_SELECTED\n"));
		break;

	case UMI_DEVICE_OID_802_11_RX_ANTENNA_SELECTED:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_RX_ANTENNA_SELECTED\n"));
		break;

	case UMI_DEVICE_OID_802_11_TX_POWER_LEVEL:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_TX_POWER_LEVEL\n"));
		if (pValue) {
			UMI_MemoryCopy(&pParamInfo->TxPowerLevel, (uint8 *) pValue, sizeof(sint32));
			paramDataLen = sizeof(sint32);
		}
		break;

	case UMI_DEVICE_OID_802_11_SSID:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SSID\n"));

		if (pValue) {
			/*
			   Search the stored BSS_LIST and load the UMI_BSS_INFO
			   structure corresponding  to this SSID.
			   In this case, pValue contains the SSID as per
			   the Windows DDK documentation.
			 */
			pSsid = (UMI_802_11_SSID *) pValue;

			pBssCacheInfoInd = UMI_GetBssInfoSsid(pUmiCon->pScanListQueue, (uint8) pSsid->ssidLength, pSsid->ssid, pUmiCon->encryptionStatus);

			/* Convert UMI_BSS_CACHE_INFO_IND to UMI_BSS_INFO */
			pBssInfo = UMI_GetBssInfo(umiHandle, pBssCacheInfoInd);

			if (pBssInfo) {
				/*
				   Need to add the length of number of bytes
				   here. 4 bytes are reserved for Supported
				   rates already, so have to substract 4 and
				   add the number of rates.
				 */
				paramDataLen = (sizeof(WFM_BSS_INFO) - WFM_BSS_INFO_SUPPORTED_RATE_SIZE)
				    + pBssInfo->NumRates;

				UMI_MemoryCopy(&pParamInfo->BssInfo, pBssInfo, paramDataLen);
				UMI_Free(pBssInfo);
			} else if (UMI_802_IBSS == pUmiCon->infraMode) {

				/*pParamInfo->BssInfo.Capabilities = WFM_UMAC_SUPPORTED_CAPABILITIES; */
				pParamInfo->BssInfo.InfrastructureMode = WFM_802_IBSS;
				paramDataLen = sizeof(WFM_BSS_INFO);
				pSsid = (UMI_802_11_SSID *) pValue;

				pParamInfo->BssInfo.SsidLength = pSsid->ssidLength;
				OS_MemoryCopy((void *)&pParamInfo->BssInfo.Ssid[0], (void *)&pSsid->ssid[0], pSsid->ssidLength);
			} else {
				paramDataLen = sizeof(WFM_BSS_INFO);

				pParamInfo->BssInfo.SsidLength = pSsid->ssidLength;

				OS_MemoryCopy((void *)&pParamInfo->BssInfo.Ssid[0], (void *)&pSsid->ssid[0], pSsid->ssidLength);

				/* Flags Bit 10-12 indicates encryption status */
				pParamInfo->BssInfo.Flags = (uint16) (pUmiCon->encryptionStatus << 10);
			}	/* if (pBssnfo) */
		}	/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_ENCRYPTION_STATUS:
		if (pValue) {
			pParamInfo->Encryption_Status = *(uint32 *) pValue;
			pUmiCon->encryptionStatus = *(uint32 *) pValue;
			paramDataLen = sizeof(pParamInfo->Encryption_Status);
		}	/* if (pValue) */
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_ENCRYPTION_STATUS\n"));
		break;

	case UMI_DEVICE_OID_802_11_PRIVACY_FILTER:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_PRIVACY_FILTER\n"));
		break;

	case UMI_DEVICE_OID_802_11_BLOCK_ACK_POLICY:
		if (pValue) {
			pBlockAckPolicy = (UMI_BLOCK_ACK_POLICY *) pValue;
			UMI_MemoryCopy((void *)&pParamInfo->block_ack_policy, (void *)pBlockAckPolicy, sizeof(UMI_BLOCK_ACK_POLICY));
			paramDataLen = sizeof(UMI_BLOCK_ACK_POLICY);
		}
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_BLOCK_ACK_POLICY\n"));
		break;

	case UMI_DEVICE_OID_802_11_UPDATE_EPTA_CONFIG_DATA:
		if (pValue) {
			pEptaConfigData = (UMI_UPDATE_EPTA_CONFIG_DATA *) pValue;

			pParamInfo->update_EPTA_config_data.enablePta = pEptaConfigData->enablePta;
			pParamInfo->update_EPTA_config_data.bluetoothQuality = pEptaConfigData->bluetoothQuality;
			pParamInfo->update_EPTA_config_data.subscribeBtEvent = 1;
			pParamInfo->update_EPTA_config_data.PtaDebugCommand = pEptaConfigData->PtaDebugCommand;
			pParamInfo->update_EPTA_config_data.reserved = 0;

			paramDataLen = sizeof(WFM_UPDATE_EPTA_CONFIG_DATA);
		}
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_UPDATE_EPTA_CONFIG_DATA\n"));
		break;

	case UMI_DEVICE_OID_802_11_POWER_MODE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_POWER_MODE\n"));
		if (pValue) {
			UMI_MemoryCopy(&pParamInfo->psMode, pValue, sizeof(pParamInfo->psMode));
			paramDataLen = sizeof(pParamInfo->psMode);
		}	/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_DISABLE_BG_SCAN:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_POWER_MODE\n"));
		if (pValue) {
			pParamInfo->disableBGScan = *(uint32 *) pValue;
			paramDataLen = sizeof(pParamInfo->disableBGScan);
		}	/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_SET_AUTO_CALIBRATION_MODE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_AUTO_CALIBRATION_MODE\n"));
		if (pValue) {
			pParamInfo->setAutoCalibrationMode = *(uint32 *) pValue;
			paramDataLen = sizeof(pParamInfo->setAutoCalibrationMode);
		}	/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_BLACK_LIST_ADDR:
		UMI_DEFAULT(UMI_Api, ("WFM_SetParameter(): UMI_DEVICE_OID_802_11_BLACK_LIST_ADDR\n"));

		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->BlackListAddr[0], (void *)pValue, 6);

			paramDataLen = 6;
		}	/*if(pValue) */

		break;

	case UMI_DEVICE_OID_802_11_CONFIGURE_IBSS:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_CONFIGURE_IBSS\n"));
		if (UMI_802_IBSS == pUmiCon->infraMode) {
			if (pValue) {
				pConfigureIBSS = (UMI_OID_802_11_CONFIGURE_IBSS *) pValue;
				UMI_MemoryCopy((void *)&pParamInfo->configureIBSS, (void *)pConfigureIBSS, sizeof(UMI_OID_802_11_CONFIGURE_IBSS));
				paramDataLen = sizeof(UMI_OID_802_11_CONFIGURE_IBSS);
			}
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: Invalid Infra Mode\n"));
			statusCode = UMI_STATUS_REQ_REJECTED;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}
		break;

	case UMI_DEVICE_OID_802_11_SET_UAPSD:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_UAPSD\n"));
		if (pValue) {
			pSetUAPSD = (UMI_OID_802_11_SET_UAPSD *) pValue;
			/* No need to send parameters if all the UAPSD flags are 0 */
			if (0 == pSetUAPSD->uapsdFlags) {
				statusCode = UMI_STATUS_REQ_REJECTED;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			UMI_MemoryCopy((void *)&pParamInfo->setUAPSD, (void *)pSetUAPSD, sizeof(UMI_OID_802_11_SET_UAPSD));
			paramDataLen = sizeof(UMI_OID_802_11_SET_UAPSD);
		}
		break;

	case UMI_DEVICE_OID_802_11_MAX_RX_AMSDU_SIZE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_MAX_RX_AMSDU_SIZE\n"));
		if (pValue) {
			uint32 maxRxAmsduSize = *(uint32 *) pValue;

			if (maxRxAmsduSize <= 1) {
				pParamInfo->maxRxAmsduSize = *(uint32 *) pValue;
				paramDataLen = sizeof(pParamInfo->maxRxAmsduSize);
			} else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: UMI_DEVICE_OID_802_11_MAX_RX_AMSDU_SIZE invalid value\n"));
				statusCode = UMI_STATUS_BAD_PARAM;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_FAILURE;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}	/* if (pValue != NULL) */

		break;
#if FT_SUPPORT
	case UMI_DEVICE_OID_802_11_MDIE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_MDIE\n"));
		if (pValue) {
			UMAC_MDIE *mdie = (UMAC_MDIE *) pValue;
			if (mdie->mdie_len && ((mdie->mdie_len != 5)
					       || (mdie->mdie[0] != 54) || (mdie->mdie[1] != 3))) {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: UMI_DEVICE_OID_802_11_MDIE invalid value\n"));
				statusCode = UMI_STATUS_BAD_PARAM;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			pParamInfo->mdie.mdie_len = mdie->mdie_len;
			if (mdie->mdie_len == 0)
				UMI_MemoryReset(&pParamInfo->mdie.mdie[0], 5);
			else
				UMI_MemoryCopy(&pParamInfo->mdie.mdie[0], &mdie->mdie[0], 5);
			paramDataLen = sizeof(pParamInfo->mdie);
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_FAILURE;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}	/* if (pValue != NULL) */

		break;

	case UMI_DEVICE_OID_802_11_FTIE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_FTIE\n"));
		if (pValue) {
			UMAC_FTIE *ftie = (UMAC_FTIE *) pValue;
			if (ftie->ftie_len && ((ftie->ftie_len > 258)
					       || (ftie->ftie[0] != 55)
					       || (ftie->ftie[1] != (ftie->ftie_len - 2)))) {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: UMI_DEVICE_OID_802_11_FTIE invalid value\n"));
				statusCode = UMI_STATUS_BAD_PARAM;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			pParamInfo->ftie.ftie_len = ftie->ftie_len;
			if (ftie->ftie_len == 0)
				UMI_MemoryReset(&pParamInfo->ftie.ftie[0], sizeof(pParamInfo->ftie.ftie));
			else
				UMI_MemoryCopy(&pParamInfo->ftie.ftie[0], &ftie->ftie[0], ftie->ftie_len);
			paramDataLen = sizeof(pParamInfo->ftie);
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_FAILURE;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}	/* if (pValue != NULL) */

		break;

	case UMI_DEVICE_OID_802_11_FT_AUTHENTICATE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_FT_AUTHENTICATE\n"));
		if (pValue) {
			/*
			 * At this point -
			 * 1. STA must be in associated satte.
			 * 2. Initial Mobility Domain Association is completed.
			 * 3. PMKR0Name, MDIE and FTIE are already updated by
			 * upper layer.
			 */
			UMI_FT_AUTHENTICATE *pUmiFtAuth = (UMI_FT_AUTHENTICATE *) pValue;

			pBssCacheInfoInd = UMI_GetBssInfoMacAddr(pUmiCon->pScanListQueue, pUmiFtAuth->Bssid);
			pBssInfo = UMI_GetBssInfo(umiHandle, pBssCacheInfoInd);

			if (pBssInfo) {
				pBssInfo->FtAuthFlags = pUmiFtAuth->flags;
				paramDataLen = sizeof(*pBssInfo) - WFM_BSS_INFO_SUPPORTED_RATE_SIZE + pBssInfo->NumRates;

				UMI_MemoryCopy((void *)(&pParamInfo->BssInfo), pBssInfo, paramDataLen);
				UMI_Free(pBssInfo);
			} else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: No such BSSID\n"));
				statusCode = UMI_STATUS_FAILURE;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_FAILURE;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}	/* if (pValue != NULL) */
		break;
	case UMI_DEVICE_OID_802_11_DELETE_FT_AUTHENTICATION:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_DELETE_FT_AUTHENTICATION\n"));
		if (pValue) {
			UMI_BSSID_OID *pUmiBssid;
			pUmiBssid = (UMI_BSSID_OID *) pValue;
			UMI_MemoryCopy(&pParamInfo->BssidOid, pUmiBssid, sizeof(*pUmiBssid));
			paramDataLen = sizeof(*pUmiBssid);
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_BAD_PARAM;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}
		break;
#endif	/* FT_SUPPORT */
	case UMI_DEVICE_OID_802_11_TXOP_LIMIT:
		{
			UMI_TXOP_LIMIT *pTxopLimit = NULL;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_TXOP_LIMIT\n"));
			if (pValue) {
				pTxopLimit = (UMI_TXOP_LIMIT *) pValue;
				UMI_MemoryCopy(&pParamInfo->txopLimit, pTxopLimit, sizeof(UMI_TXOP_LIMIT));
				paramDataLen = sizeof(UMI_TXOP_LIMIT);
			}	/* if (pValue) */
			break;
		}

	case UMI_DEVICE_OID_802_11_SET_LISTEN_INTERVAL:
		{
			UMI_LISTEN_INTERVAL *pListenInterval = NULL;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_LISTEN_INTERVAL\n"));
			if (pValue) {
				pListenInterval = (UMI_LISTEN_INTERVAL *) pValue;
				UMI_MemoryCopy(&pParamInfo->listenInterval, pListenInterval, sizeof(UMI_LISTEN_INTERVAL));
				paramDataLen = sizeof(UMI_LISTEN_INTERVAL);
			}	/* if (pValue) */
			break;
		}

	case UMI_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE\n"));
		if (pValue) {
			pParamInfo->OprPwrMode = *(uint32 *) pValue;
			paramDataLen = sizeof(pParamInfo->OprPwrMode);
		}	/* if (pValue) */
		break;
	case UMI_DEVICE_OID_802_11_SET_HOST_MSG_TYPE_FILTER:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_HOST_MSG_TYPE_FILTER\n"));
		if (pValue) {
			pParamInfo->hostMsgTypeFilter = *(uint32 *) pValue;
			paramDataLen = sizeof(pParamInfo->hostMsgTypeFilter);
		}	/* if (pValue) */
		break;
	case UMI_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD\n"));
			if (pValue) {
				pRcpiRssiThreshold = (UMI_RCPI_RSSI_THRESHOLD *) pValue;
				UMI_MemoryCopy(&pParamInfo->RcpiRssiThreshold, pRcpiRssiThreshold, sizeof(UMI_RCPI_RSSI_THRESHOLD));
				paramDataLen = sizeof(UMI_RCPI_RSSI_THRESHOLD);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_SET_PMKID_BKID:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_PMKID_BKID\n"));
			if (pValue) {
				pBssidPmkidBkid = (UMI_BSSID_PMKID_BKID *) pValue;
				pParamInfo->PmkidBkidInfo.bssidInfoCount = pBssidPmkidBkid->bssidInfoCount;
				UMI_MemoryCopy(&pParamInfo->PmkidBkidInfo.bssidInfo, &pBssidPmkidBkid->bssidInfo, pBssidPmkidBkid->bssidInfoCount * sizeof(UMI_BSSID_INFO));
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT\n"));
			if (pValue) {
				pBeaconLostCount = (UMI_BEACON_LOST_COUNT *) pValue;
				UMI_MemoryCopy(&pParamInfo->BeaconLostCount.BeaconLostCount, &pBeaconLostCount->BeaconLostCount, sizeof(UMI_BEACON_LOST_COUNT));
				paramDataLen = sizeof(UMI_BEACON_LOST_COUNT);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_TX_FAILURE_THRESHOLD_COUNT:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_TX_FAILURE_THRESHOLD_COUNT\n"));
			if (pValue) {
				pTxFailureCount = (UMI_TX_FAILURE_THRESHOLD_COUNT *) pValue;
				UMI_MemoryCopy(&pParamInfo->TxFailureCount.TransmitFailureThresholdCount, &pTxFailureCount->TransmitFailureThresholdCount, sizeof(UMI_TX_FAILURE_THRESHOLD_COUNT));
				paramDataLen = sizeof(UMI_TX_FAILURE_THRESHOLD_COUNT);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_SET_ETHER_TYPE_FILTER:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_ETHER_TYPE_FILTER\n"));
			if (pValue) {
				pSetEtherTypeFilter = (UMI_SET_ETHER_TYPE_FILTER *) pValue;
				UMI_MemoryCopy(&pParamInfo->setEtherTypeFilter, pSetEtherTypeFilter, sizeof(UMI_SET_ETHER_TYPE_FILTER));
				paramDataLen = sizeof(UMI_SET_ETHER_TYPE_FILTER);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_SET_UDP_PORT_FILTER:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_UDP_PORT_FILTER\n"));
			if (pValue) {
				pSetUdpPortFilter = (UMI_SET_UDP_PORT_FILTER *) pValue;
				UMI_MemoryCopy(&pParamInfo->setUdpPortFilter, pSetUdpPortFilter, sizeof(UMI_SET_UDP_PORT_FILTER));
				paramDataLen = sizeof(UMI_SET_UDP_PORT_FILTER);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_SET_MAGIC_FRAME_FILTER:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_MAGIC_FRAME_FILTER\n"));
			if (pValue) {
				pSetMagicFrameFilter = (UMI_SET_MAGIC_FRAME_FILTER *) pValue;
				UMI_MemoryCopy(&pParamInfo->setMagicFrameFilter, pSetMagicFrameFilter, sizeof(UMI_SET_MAGIC_FRAME_FILTER));
				paramDataLen = sizeof(UMI_SET_MAGIC_FRAME_FILTER);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_ENABLE_11D_FEATURE:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_ENABLE_11D_FEATURE\n"));
			if (pValue) {
				UMI_MemoryCopy(&pParamInfo->Enable11dFeature, (uint8 *) pValue, sizeof(uint8));
				paramDataLen = sizeof(uint8);
			}
			break;
		}

#if P2P_MINIAP_SUPPORT
	case UMI_DEVICE_OID_802_11_START_AP:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_START_AP\n"));
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->dot11StartAp, (void *)pValue, sizeof(UMI_OID_802_11_START_AP));
			paramDataLen = sizeof(UMI_OID_802_11_START_AP);
		}		/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_USE_P2P:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_USE_P2P\n"));
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->useP2P, (void *)pValue, sizeof(UMI_OID_USE_P2P));
			pUmiCon->UseP2P = pParamInfo->useP2P.useP2P;
			paramDataLen = sizeof(UMI_OID_USE_P2P);
		}		/* if (pValue) */
		break;


	case UMI_DEVICE_OID_802_11_ALLOW_ACCESS:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_ALLOW_ACCESS\n"));
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->usrAllowAccess, (void *)pValue, sizeof(UMI_OID_ALLOWED_ACCESS));
			paramDataLen = sizeof(UMI_OID_ALLOWED_ACCESS);
		}		/* if (pValue) */
		break;


	case UMI_DEVICE_OID_802_11_MAC_ADDR_UPDATE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_MAC_ADDR_UPDATE\n"));
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->databasereq, (void *)pValue, sizeof(UMI_OID_ALLOWED_DATABASE));
			paramDataLen = sizeof(UMI_OID_ALLOWED_DATABASE);
		}		/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_DEVICE_DISCOVERY:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_DEVICE_DISCOVERY\n"));
		pDeviceDiscovery = (UMI_OID_P2P_DEVICE_DISCOVERY *) pValue;
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->p2pDevDiscvry, (void *)pValue, sizeof(UMI_OID_P2P_DEVICE_DISCOVERY) - (sizeof(UMI_CHANNELS) + (pDeviceDiscovery->bssListScan.NumberOfChannels_2_4Ghz + pDeviceDiscovery->bssListScan.NumberOfChannels_5Ghz) * sizeof(UMI_CHANNELS)));

			paramDataLen = sizeof(UMI_OID_P2P_DEVICE_DISCOVERY) - (sizeof(UMI_CHANNELS) + (pDeviceDiscovery->bssListScan.NumberOfChannels_2_4Ghz + pDeviceDiscovery->bssListScan.NumberOfChannels_5Ghz) * sizeof(UMI_CHANNELS));
		}		/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_CONFIG_GROUP_FORMATION:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_CONFIG_GROUP_FORMATION\n"));
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->p2pGrpFormtn, (void *)pValue, sizeof(UMI_OID_P2P_GROUP_FORMATION));
			paramDataLen = sizeof(UMI_OID_P2P_GROUP_FORMATION);
		}		/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_INVITATION:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_INVITATION\n"));
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->p2pInvtn, (void *)pValue, sizeof(UMI_OID_P2P_INVITATION));
			paramDataLen = sizeof(UMI_OID_P2P_INVITATION);
		}		/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_CONFIGURE_WMM_PARAMS:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_CONFIGURE_WMM_PARAMS\n"));
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->dot11ConfigWMMparams.QoSInfo, (void *)pValue, sizeof(UMI_OID_802_11_CONFIG_WMM_PARAMS));
			paramDataLen = sizeof(UMI_OID_802_11_CONFIG_WMM_PARAMS);
		}		/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_START_GROUP_FORMATION:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_START_GROUP_FORMATION\n"));
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->p2pStartGrpFormatn.devNameLen, (void *)pValue, sizeof(UMI_OID_P2P_START_GROUP_FORMATION));
			paramDataLen = sizeof(UMI_OID_P2P_START_GROUP_FORMATION);
		}		/* if (pValue) */
		break;

	case UMI_DEVICE_OID_802_11_SOCIAL_CHANNELS:
		pSocialChannels = (UMI_OID_P2P_SOCIAL_CHANNEL *) pValue;
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SOCIAL_CHANNELS\n"));
		if (pValue) {
			UMI_MemoryCopy((void *)&pParamInfo->socialChannels, (void *)pValue, sizeof(UMI_OID_P2P_SOCIAL_CHANNEL) - sizeof(UMI_CHANNELS)
				       + pSocialChannels->numSocChannels * sizeof(UMI_CHANNELS));

			paramDataLen = sizeof(UMI_OID_P2P_SOCIAL_CHANNEL) - sizeof(UMI_CHANNELS)
			    + pSocialChannels->numSocChannels * sizeof(UMI_CHANNELS);
		}		/* if (pValue) */
		break;
#endif	/*P2P_MINIAP_SUPPORT */
	case UMI_DEVICE_OID_802_11_UPDATE_VENDOR_IE:
		{
			UMI_UPDATE_VENDOR_IE *pVdrIE = (UMI_UPDATE_VENDOR_IE *) pValue;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_UPDATE_VENDOR_IE\n"));
			if (pValue) {
				pParamInfo->vendorIe.probeReqIElength = pVdrIE->probeReqIElength;
				pParamInfo->vendorIe.pProbeReqIE = pVdrIE->pProbeReqIE;
				pParamInfo->vendorIe.probeRespIElength = pVdrIE->probeRespIElength;
				pParamInfo->vendorIe.pProbeRespIE = pVdrIE->pProbeRespIE;
				pParamInfo->vendorIe.beaconIElength = pVdrIE->beaconIElength;
				pParamInfo->vendorIe.pBeaconIE = pVdrIE->pBeaconIE;
				pParamInfo->vendorIe.assocReqIElength = pVdrIE->assocReqIElength;
				pParamInfo->vendorIe.pAssocReqIE = pVdrIE->pAssocReqIE;
				pParamInfo->vendorIe.assocRespIElength = pVdrIE->assocRespIElength;
				pParamInfo->vendorIe.pAssocRespIE = pVdrIE->pAssocRespIE;
				paramDataLen = sizeof(UMI_UPDATE_VENDOR_IE);
			} else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
				statusCode = UMI_STATUS_FAILURE;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER:
		{
			UMI_SET_ARP_IP_ADDR_FILTER *pArpFilter = (UMI_SET_ARP_IP_ADDR_FILTER *) pValue;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER\n"));
			if (pValue) {
				UMI_MemoryCopy(&pParamInfo->ArpFilter, pArpFilter, sizeof(UMI_SET_ARP_IP_ADDR_FILTER));
				paramDataLen = sizeof(UMI_SET_ARP_IP_ADDR_FILTER);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_SET_ARP_KEEP_ALIVE_PERIOD:
		{
			UMI_OID_802_11_ARP_KEEP_ALIVE_PERIOD *pArpKeepAlivePeriod = (UMI_OID_802_11_ARP_KEEP_ALIVE_PERIOD *) pValue;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_ARP_KEEP_ALIVE_PERIOD\n"));
			if (pValue) {
				UMI_MemoryCopy(&pParamInfo->ArpKeepAlivePeriod, pArpKeepAlivePeriod, sizeof(UMI_OID_802_11_ARP_KEEP_ALIVE_PERIOD));
				paramDataLen = sizeof(UMI_OID_802_11_ARP_KEEP_ALIVE_PERIOD);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_PTA_LISTEN_INTERVAL:
		{
			UMI_PTA_LISTEN_INTERVAL *pPtaListenInterval = (UMI_PTA_LISTEN_INTERVAL *) pValue;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_PTA_LISTEN_INTERVAL\n"));
			if (pPtaListenInterval && pPtaListenInterval->PtaListenInterval < 20) {
				UMI_MemoryCopy(&pParamInfo->PtaListenInterval, pPtaListenInterval, sizeof(UMI_PTA_LISTEN_INTERVAL));
				paramDataLen = sizeof(UMI_PTA_LISTEN_INTERVAL);
			} /* if (pValue) */
			else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): bad parameter\n"));
				statusCode = UMI_STATUS_BAD_PARAM;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			break;
		}
#if DOT11K_SUPPORT
	case UMI_DEVICE_OID_802_11_MEASUREMENT_CAPABILITY:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_MEASUREMENT_CAPABILITY\n"));
			if (pValue) {
				pParamInfo->measurementCapability = *(uint16 *) pValue;
				paramDataLen = sizeof(uint16);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_ENABLE_MEASUREMENTS:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_ENABLE_MEASUREMENTS\n"));

			if (pValue) {
				pParamInfo->enableMeasurements = *(uint16 *) pValue;
				paramDataLen = sizeof(uint16);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_NEIGHBOR_REPORT:
		{
			UMI_802_11_SSID *pNeighborRep = NULL;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_NEIGHBOR_REPORT\n"));
			if (pValue) {
				pNeighborRep = (UMI_802_11_SSID *) pValue;
				pParamInfo->NeighborReportSsid.SsidLength = pNeighborRep->ssidLength;
				UMI_MemoryCopy(&pParamInfo->NeighborReportSsid.Ssid, pNeighborRep->ssid, pNeighborRep->ssidLength);
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_LCI_INFORMATION:
		{
			UMI_802_11_LCI_INFORMATION *pLciInfo = NULL;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_LCI_INFORMATION\n"));
			if (pValue) {
				pLciInfo = (UMI_802_11_LCI_INFORMATION *) pValue;
				UMI_MemoryCopy(&pParamInfo->lciInfo, pLciInfo, sizeof(UMI_802_11_LCI_INFORMATION));
				paramDataLen = sizeof(UMI_802_11_LCI_INFORMATION);
			}
			break;
		}
#endif

	case UMI_DEVICE_OID_802_11_CONFIGURE_REGULATORY_DOMAIN:
		{
			UMI_REGULATORY_DOMAIN_TABLE *pRegDomainTable = NULL;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_CONFIGURE_REGULATORY_DOMAIN\n"));
			if (pValue) {
				pRegDomainTable = (UMI_REGULATORY_DOMAIN_TABLE *) pValue;
				UMI_MemoryCopy(&pParamInfo->regDomainTable, pRegDomainTable,
					sizeof(UMI_REGULATORY_DOMAIN_TABLE));
			}
			paramDataLen = (uint16) sizeof(UMI_REGULATORY_DOMAIN_TABLE);
			break;
		}

#if MGMT_FRAME_PROTECTION
	case UMI_DEVICE_OID_802_11_PROTECTED_MGMT_POLICY:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_PROTECTED_MGMT_POLICY\n"));
			if (pValue) {
				pParamInfo->procMgmtFrmPolicy = *(uint32 *) pValue;
				paramDataLen = sizeof(pParamInfo->procMgmtFrmPolicy);
			} else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
				statusCode = UMI_STATUS_FAILURE;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_SA_QUERY_INTERVAL:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SA_QUERY_INTERVAL\n"));
			if (pValue) {
				UMI_SA_QUERY_INTERVAL *pSaQueryInterval = (UMI_SA_QUERY_INTERVAL *) pValue;
				UMI_MemoryCopy(&pParamInfo->saQueryInterval, pSaQueryInterval, sizeof(UMI_SA_QUERY_INTERVAL)
				    );
				paramDataLen = sizeof(UMI_SA_QUERY_INTERVAL);
			} else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
				statusCode = UMI_STATUS_FAILURE;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			break;
		}
#endif	/*MGMT_FRAME_PROTECTION */
	case UMI_DEVICE_OID_802_11_MULTI_TX_CONFIRMATION:
		{
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_MULTI_TX_CONFIRMATION\n"));
			if (pValue) {
				pParamInfo->useMultiTxConfMsg = *(uint32 *) pValue;
				paramDataLen = sizeof(uint32);
			} else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
				statusCode = UMI_STATUS_BAD_PARAM;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER:
		{
			UMI_OID_802_11_MULTICAST_ADDR_FILTER *pMulticastAddrFilter = (UMI_OID_802_11_MULTICAST_ADDR_FILTER *) pValue;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER\n"));
			if (pValue) {
				paramDataLen = (uint16) (8 + (pMulticastAddrFilter->numOfAddresses * sizeof(UMI_MAC_ADDRESS)));
				UMI_MemoryCopy(&pParamInfo->multicastAddrFilter, pMulticastAddrFilter, paramDataLen);
			} else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
				statusCode = UMI_STATUS_BAD_PARAM;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_KEEP_ALIVE_PERIOD:
		{
			UMI_OID_802_11_KEEP_ALIVE_PERIOD *pKeepAlivePeriod = (UMI_OID_802_11_KEEP_ALIVE_PERIOD *) pValue;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_KEEP_ALIVE_PERIOD\n"));
			if (pValue) {
				paramDataLen = sizeof(UMI_OID_802_11_KEEP_ALIVE_PERIOD);
				UMI_MemoryCopy(&pParamInfo->keepAlivePeriod, pKeepAlivePeriod, paramDataLen);
			} else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
				statusCode = UMI_STATUS_BAD_PARAM;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID:
		{
			UMI_OID_802_11_WEP_DEFAULT_KEY_ID *pWepDefaultKeyId = (UMI_OID_802_11_WEP_DEFAULT_KEY_ID *) pValue;
			UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID\n"));
			if (pValue) {
				paramDataLen = sizeof(UMI_OID_802_11_WEP_DEFAULT_KEY_ID);
				UMI_MemoryCopy(&pParamInfo->wepDefaultKeyId, pWepDefaultKeyId, paramDataLen);
			} else {
				UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
				statusCode = UMI_STATUS_BAD_PARAM;
				UMI_Free(pHiMessage);
				goto Exit_Func;
			}
			break;
		}
	case UMI_DEVICE_OID_802_11_DISABLE_ACTIVE_JOIN:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_DISABLE_ACTIVE_JOIN\n"));
		if (pValue) {
			pParamInfo->DisableActiveJoin = *(uint32 *) pValue;
			paramDataLen = sizeof(uint32);
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_BAD_PARAM;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}
		break;
#if P2P_MINIAP_SUPPORT
	case UMI_DEVICE_OID_802_11_INTRA_BSS_BRIDGING:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_DISABLE_AP_MODE_BRIDGING\n"));
		if (pValue) {
			pParamInfo->IntraBssBridging = *(uint32 *) pValue;
			paramDataLen = sizeof(uint32);
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_BAD_PARAM;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}
		break;
	case UMI_DEVICE_OID_802_11_STOP_AP:
		break;

	case UMI_DEVICE_OID_802_11_CHANGE_CHANNEL:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_CHANGE_CHANNEL\n"));
		if (pValue) {
			paramDataLen = sizeof(pParamInfo->ChangeChannel);
			OS_MemoryCopy(&pParamInfo->ChangeChannel, pValue, sizeof(pParamInfo->ChangeChannel));
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_BAD_PARAM;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}
		break;
	case UMI_DEVICE_OID_802_11_RESTORE_CHANNEL:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_RESTORE_CHANNEL\n"));
		break;
	case UMI_DEVICE_OID_802_11_REGISTER_MGMT_FRAME_RX:
		break;
	case UMI_DEVICE_OID_802_11_P2P_PS_MODE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_P2P_PS_MODE\n"));
		p2pPsMode = (UMI_OID_802_11_P2P_PS_MODE *)pValue;
		if (pValue) {
			OS_MemoryCopy((void *)&pParamInfo->p2pPSMode,
				(void *)p2pPsMode,
				sizeof(UMI_OID_802_11_P2P_PS_MODE));
			paramDataLen = sizeof(UMI_OID_802_11_P2P_PS_MODE);
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_BAD_PARAM;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}
		break;
	case UMI_DEVICE_OID_802_11_SET_NUM_CLIENTS:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_NUM_CLIENTS\n"));
		if (pValue) {
			OS_MemoryCopy((void *)&pParamInfo->NumOfClients,
						  (void *)pValue,
						  sizeof(UMI_OID_802_11_SET_NUM_CLIENTS));
			paramDataLen = sizeof(UMI_OID_802_11_SET_NUM_CLIENTS);
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_BAD_PARAM;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}
		break;
	case UMI_DEVICE_OID_802_11_SET_HIDDEN_AP_MODE:
		UMI_DEFAULT(UMI_Api, ("UMI_SetParameter(): UMI_DEVICE_OID_802_11_SET_HIDDEN_AP_MODE\n"));
		if (pValue) {
			OS_MemoryCopy((void *)&pParamInfo->HiddenAPMode,
					(void *)pValue,
					sizeof(UMI_OID_802_11_SET_HIDDEN_AP_MODE));
			paramDataLen = sizeof(UMI_OID_802_11_SET_HIDDEN_AP_MODE);
		} else {
			UMI_ERROR(UMI_Api, ("UMI_SetParameter(): ERROR: pValue is NULL\n"));
			statusCode = UMI_STATUS_BAD_PARAM;
			UMI_Free(pHiMessage);
			goto Exit_Func;
		}
		break;
#endif

	default:
		UMI_ERROR(UMI_Api, ("UMI_SetParameter(): Invalid UMI_DEVICE_OID (0x%08x)\n", oid));
		statusCode = UMI_STATUS_FAILURE;
		UMI_Free(pHiMessage);
		goto Exit_Func;
	}	/* end switch(Oid) */

	if (UMI_STATUS_SUCCESS == statusCode) {
		/* Message length is the size of HI HDR+ SET_PARAM HDR
		+ Payload size */
		pHiMessage->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + WFM_SET_PARAM_REQ_HDR_SIZE + paramDataLen;

		/* Filling in the length of the parameters in this packet. */
		pHiPayload->wfm_set_param_req.ParameterLength = cpu_to_le16(paramDataLen);

		/* Send HiMessage to UMAC Module */
		statusCode = UMAC_IF_SET_PARAM_REQ(pUmiCon->umacHandle, pHiMessage);

		if (UMI_STATUS_PENDING != statusCode)
			UMI_Free(pHiMessage);
	}	/* if (UMI_STATUS_SUCCESS == StatusCode) */
 Exit_Func:;

	UMI_TRACE(UMI_Api, ("<--- UMI_SetParameter() (0x%08x)\n", statusCode));

	return statusCode;
}	/* End UMI_SetParameter() */

/******************************************************************************
 * NAME:	UMI_Worker
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function executes the UMI state machine in a exclusive context scheduled
 * by the driver software.The function UL_CB::Schedule() indicate driver that
 * UMAC need context to process imminent events. If driver is free, than it will
 * call this function.This function shall return as soon as it has processed all
 * imminent events. It is not a thread function.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Worker(UMI_HANDLE umiHandle)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;

	UMI_TRACE(UMI_Api, ("---> UMI_Worker()\n"));

	/* Verify UMI Handle */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_Worker(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* UMAC function to process imminent events */
	UMAC_RUN_TASK(pUmiCon->umacHandle);

 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_Worker() (0x%08x)\n", statusCode));

	return statusCode;
}	/* End UMI_Worker() */

/******************************************************************************
 * NAME:	UMI_ConfigurationRequest
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will format and send a Configuration request down to the
 * Device. Before calling this function the UMI module has to be initialized
 * and started.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param length      -   Number of bytes in the data.
 * \param pValue      -   The parameter data.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_ConfigurationRequest(UMI_HANDLE umiHandle,
					 uint16 length,
					 UMI_CONFIG_REQ *pValue)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	WFM_HI_MESSAGE *pHiMessage = NULL;
	WFM_CONFIG_REQ *pHiConfigReq = NULL;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;
	uint16 configSize = 0;

	UMI_TRACE(UMI_Api, ("---> UMI_ConfigurationRequest()"));

	/* Verify UMI Handle */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_ConfigurationRequest(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* Verify paramaters */
	if (pValue == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_ConfigurationRequest(): Unexpected NULL pValue\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	if (!length) {
		UMI_ERROR(UMI_Api, ("UMI_ConfigurationRequest(): Unexpected Length\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* Check Current State of UMI module */
	if (!IS_UMI_STATE(pUmiCon, UMI_STATE_STARTED)) {
		UMI_ERROR(UMI_Api, ("UMI_ConfigurationRequest(): Unexpected STATE (0x%08x)\n", pUmiCon->devUmiState));
		statusCode = UMI_STATUS_FAILURE;
		goto Exit_Func;
	}

	pHiMessage = (WFM_HI_MESSAGE *) UMI_Allocate(sizeof(UMAC_DEVIF_TX_DATA) + sizeof(WFM_HI_MESSAGE) + length);

	if (!pHiMessage) {
		UMI_ERROR(UMI_Api, ("UMI_ConfigurationRequest: Failed to allocate memory for HI Message"));
		statusCode = UMI_STATUS_OUT_OF_RESOURCES;
		goto Exit_Func;
	}	/* if (!pHiMessage) */
	UMI_MemoryReset(pHiMessage, (sizeof(UMAC_DEVIF_TX_DATA) + sizeof(WFM_HI_MESSAGE)));

	pHiMessage = (WFM_HI_MESSAGE *) ((uint8 *) pHiMessage + sizeof(UMAC_DEVIF_TX_DATA));

	pHiMessage->MessageId = WFM_CONFIG_REQ_ID;

	pHiConfigReq = (WFM_CONFIG_REQ *) &pHiMessage->PayLoad.wfm_config_req;

	configSize = length;

	UMI_MemoryCopy(pHiConfigReq, pValue, configSize);

	pHiMessage->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + configSize;

	/* Send HiMessage to UMAC Module */
	statusCode = UMAC_IF_CONFIG_REQ(pUmiCon->umacHandle, pHiMessage);

	if (statusCode != UMI_STATUS_PENDING)
		UMI_Free(((uint8 *) pHiMessage - sizeof(UMAC_DEVIF_TX_DATA)));

 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_ConfigurationRequest(): 0x%08x\n", statusCode));

	return statusCode;

}	/* End UMI_ConfigurationRequest() */

/******************************************************************************
 * NAME:	UMI_MemoryReadRequest
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will format and send a memory read request down to the Device.
 * \param umiHandle - A UMI instance returned by UMI_Create().
 * \param length - Number of bytes in the data.
 * \param pValue - The parameter data.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_MemoryReadRequest(UMI_HANDLE umiHandle,
				      uint16 length,
				      UMI_MEM_READ_REQ *pValue)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	WFM_HI_MESSAGE *pHiMessage = NULL;
	WFM_MEM_READ_REQ *pHiMemReadReq = NULL;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;

	UMI_TRACE(UMI_Api, ("---> UMI_MemoryReadRequest()"));

	/* Verify UMI Handle */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryReadRequest(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* Verify paramaters */
	if (pValue == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryReadRequest(): Unexpected NULL pValue\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	if (!length) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryReadRequest(): Unexpected Length\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* Check Current State of UMI module */
	if (!IS_UMI_STATE(pUmiCon, UMI_STATE_STARTED)) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryReadRequest(): Unexpected STATE (0x%08x)\n", pUmiCon->devUmiState));
		statusCode = UMI_STATUS_FAILURE;
		goto Exit_Func;
	}

	pHiMessage = (WFM_HI_MESSAGE *) UMI_Allocate(sizeof(UMAC_DEVIF_TX_DATA) + sizeof(WFM_HI_MESSAGE) + length);

	if (!pHiMessage) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryReadRequest: Failed to allocate memory for HI Message"));
		statusCode = UMI_STATUS_OUT_OF_RESOURCES;
		goto Exit_Func;
	}	/* if (!pHiMessage) */
	UMI_MemoryReset(pHiMessage, (sizeof(UMAC_DEVIF_TX_DATA) + sizeof(WFM_HI_MESSAGE)));

	pHiMessage = (WFM_HI_MESSAGE *) ((uint8 *) pHiMessage + sizeof(UMAC_DEVIF_TX_DATA));

	pHiMessage->MessageId = WFM_MEMORY_READ_REQ_ID;

	pHiMemReadReq = (WFM_MEM_READ_REQ *) &pHiMessage->PayLoad.wfm_mem_read_req;

	UMI_MemoryCopy(pHiMemReadReq, pValue, length);

	pHiMessage->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + length;

	/* Send HiMessage to UMAC Module */
	statusCode = UMAC_MEMORY_READ_REQ(pUmiCon->umacHandle, pHiMessage);

	if (statusCode != UMI_STATUS_PENDING)
		UMI_Free(((uint8 *) pHiMessage - sizeof(UMAC_DEVIF_TX_DATA)));

 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_MemoryReadRequest(): 0x%08x\n", statusCode));

	return statusCode;
}

/******************************************************************************
 * NAME:	UMI_MemoryWriteRequest
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will format and send a memory write request down to the Device.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param length      -   Number of bytes in the data.
 * \param pValue - The parameter data.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_MemoryWriteRequest(UMI_HANDLE umiHandle,
				       uint16 length,
				       UMI_MEM_WRITE_REQ *pValue)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	WFM_HI_MESSAGE *pHiMessage = NULL;
	WFM_MEM_WRITE_REQ *pHiMemWriteReq = NULL;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;

	UMI_TRACE(UMI_Api, ("---> UMI_MemoryWriteRequest()"));

	/* Verify UMI Handle */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryWriteRequest(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* Verify paramaters */
	if (pValue == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryWriteRequest(): Unexpected NULL pValue\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	if (!length) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryWriteRequest(): Unexpected Length\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* Check Current State of UMI module */
	if (!IS_UMI_STATE(pUmiCon, UMI_STATE_STARTED)) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryWriteRequest(): Unexpected STATE (0x%08x)\n", pUmiCon->devUmiState));
		statusCode = UMI_STATUS_FAILURE;
		goto Exit_Func;
	}

	pHiMessage = (WFM_HI_MESSAGE *) UMI_Allocate(sizeof(UMAC_DEVIF_TX_DATA) + sizeof(WFM_HI_MESSAGE) + length);

	if (!pHiMessage) {
		UMI_ERROR(UMI_Api, ("UMI_MemoryWriteRequest: Failed to allocate memory for HI Message"));
		statusCode = UMI_STATUS_OUT_OF_RESOURCES;
		goto Exit_Func;
	}	/* if (!pHiMessage) */

	UMI_MemoryReset(pHiMessage, (sizeof(UMAC_DEVIF_TX_DATA) + sizeof(WFM_HI_MESSAGE)));

	pHiMessage = (WFM_HI_MESSAGE *) ((uint8 *) pHiMessage + sizeof(UMAC_DEVIF_TX_DATA));

	pHiMessage->MessageId = WFM_MEMORY_WRITE_REQ_ID;

	pHiMemWriteReq = (WFM_MEM_WRITE_REQ *) &pHiMessage->PayLoad.wfm_mem_write_req;

	UMI_MemoryCopy(pHiMemWriteReq, pValue, length);

	pHiMessage->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + length;

	/* Send HiMessage to UMAC Module */
	statusCode = UMAC_MEMORY_WRITE_REQ(pUmiCon->umacHandle, pHiMessage);

	if (statusCode != UMI_STATUS_PENDING)
		UMI_Free(((uint8 *) pHiMessage - sizeof(UMAC_DEVIF_TX_DATA)));

 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_MemoryWriteRequest(): 0x%08x\n", statusCode));

	return statusCode;
}

/******************************************************************************
 * NAME:	UMI_RegisterEvents
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Through this API upper layer will register event of their interest.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param eventMask   -   Bit mask of registered events.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_RegisterEvents(UMI_HANDLE umiHandle, uint32 eventMask)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;

	UMI_TRACE(UMI_Api, ("---> UMI_RegisterEvents()"));

	/* Verify UMI Handle */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_RegisterEvents(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	}

	/* if (umiHandle == NULL) */
	/* Check Current State of UMI module */
	if (!IS_UMI_STATE(pUmiCon, UMI_STATE_STARTED)) {
		UMI_ERROR(UMI_Api, ("UMI_RegisterEvents(): Unexpected STATE (0x%08x)\n", pUmiCon->devUmiState));
		statusCode = UMI_STATUS_FAILURE;
		goto Exit_Func;
	}

 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_RegisterEvents(): 0x%08x\n", statusCode));

	return statusCode;
}	/* End UMI_RegisterEvents() */

/******************************************************************************
 * NAME:UMI_MgmtTxFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function indicates to the UMI module the driver has 802.11 Managment or
 * Action frame to be sent to the device.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param flags       -   b0-b7:Link Id
			  b8-b30: Reserved
 * \param pTxMgmtData -   pointer to the structure which
 *			  include frame to transmit.
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_MgmtTxFrame(UMI_HANDLE umiHandle,
			     uint32 flags,
			     uint32 FrameLength,
			     uint8  *pTxMgmtData)
{
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;
	UMI_STATUS_CODE statusCode;
	UMAC_MGMT_TRANSMIT_REQ pMgmtTxReq = {0};

	UMI_TRACE(UMI_Api, ("---> UMI_MgmtTxFrame()\n"));

	/* Verify Paramaters */
	if (umiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_MgmtTxFrame(): Unexpected NULL umiHandle\n"));
		statusCode = UMI_STATUS_BAD_PARAM;
		goto Exit_Func;
	} /* if (umiHandle == NULL) */


	pMgmtTxReq.FrameLength = FrameLength;
	pMgmtTxReq.linkId = flags;
	pMgmtTxReq.pTxMgmtData = pTxMgmtData;

	/* Transmit frame to UMAC module */
	statusCode = UMAC_IF_MGMT_TX_REQ(
				pUmiCon->umacHandle,
				&pMgmtTxReq
				);
 Exit_Func:;
	UMI_TRACE(UMI_Api, ("<--- UMI_MgmtTxFrame() (0x%08x)\n", statusCode));

	return statusCode;
} /* End UMI_MgmtTxFrame() */

/*-----------------------------------------------------------------------------
 *		 Device Interface Layer
 ----------------------------------------------------------------------------*/
/******************************************************************************
 * NAME:	UMI_GetTxFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Pulls the frame queued in the Device Interface Layer.
 * \param UmiHandle   -   A UMI instance returned by UMI_Create().
 * \param pTxData     -   A structure that contains all the input-output
 *                        parameters to this function.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_GetTxFrame(UMI_HANDLE UmiHandle, UMI_GET_TX_DATA **pTxData)
{
	sint8 iCount = 0;
	uint32 getIndex;
	DIL_UMAC_HI_MSG_Q *dilUmacHiMsgQ;
	UMI_STATUS_CODE StatusCode = UMI_STATUS_SUCCESS;
	LL_INFO *pLowerLayerInfo = (LL_INFO *) UmiHandle;

	LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	if (UmiHandle == NULL) {
		UMI_ERROR(UMI_Api, ("UMI_GetTxFrame(): Unexpected NULL UmiHandle\n"));
		StatusCode = UMI_STATUS_FAILURE;
		return StatusCode;
	}

	if (pTxData == NULL) {

		UMI_ERROR(UMI_Api, ("UMI_GetTxFrame(): Unexpected NULL pTxData\n"));
		StatusCode = UMI_STATUS_FAILURE;
		return StatusCode;
	}

	UMI_AcquireLock(llPvt->txQueueLock);

	if (pLowerLayerInfo->AllowCtrlQueueId == TRUE) {
		if (((DIL_UMAC_HI_MSG_Q *) llPvt->pQueue[QueueId_CTRL])->Put != ((DIL_UMAC_HI_MSG_Q *) (llPvt->pQueue[QueueId_CTRL]))->Get)
			iCount = QueueId_CTRL;
		else
			iCount--;
	} else {
		for (iCount = NUM_PRIO_QUEUE - 1; iCount >= 0; iCount--) {
			if (((DIL_UMAC_HI_MSG_Q *) llPvt->pQueue[iCount])->Put != ((DIL_UMAC_HI_MSG_Q *) (llPvt->pQueue[iCount]))->Get)
				break;
		}
	}

	if (iCount < 0) {
		UMI_ReleaseLock(llPvt->txQueueLock);
		StatusCode = UMI_STATUS_NO_MORE_DATA;
		goto exit_handler;
	} else {
		dilUmacHiMsgQ = (DIL_UMAC_HI_MSG_Q *) llPvt->pQueue[iCount];
	}

	getIndex = dilUmacHiMsgQ->Get;

	if (dilUmacHiMsgQ->Put != getIndex) {
		*pTxData = (UMI_GET_TX_DATA *) dilUmacHiMsgQ->UmacHiBuff[getIndex];
		getIndex++;
		dilUmacHiMsgQ->Get = (getIndex & (PRIO_QUEUE_SIZE - 1));
	}	/*if(dilUmacHiMsgQ->Put != getIndex ) */
	else
		StatusCode = UMI_STATUS_NO_MORE_DATA;

	UMI_ReleaseLock(llPvt->txQueueLock);

 exit_handler:
	UMI_TRACE(UMI_Api, ("<--- UMI_GetTxFrame() (0x%08x)\n", StatusCode));

	return StatusCode;

}	/* End UMI_GetTxFrame() */

/******************************************************************************
 * NAME:	UMI_ReceiveFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will receive frame buffer from lower layer driver.
 * \param umiHandle       -   A UMI instance returned by UMI_Create().
 * \param pFrameBuffer    -   The frame buffer form the Device.
 * \param pDriverInfo     -   Pointer to the Driver Info.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_ReceiveFrame(UMI_HANDLE umiHandle,
				 void *pFrameBuffer,
				 void *pDriverInfo)
{
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;

	UMI_TRACE(UMI_Api, ("---> UMI_ReceiveFrame()\n"));

	DIL_HiMsgHandler(umiHandle, pFrameBuffer, pDriverInfo);

	UMI_TRACE(UMI_Api, ("<--- UMI_ReceiveFrame() (0x%08x)\n", statusCode));

	return statusCode;
}	/* End UMI_ReceiveFrame() */

/******************************************************************************
 * NAME:	UMI_RxFrmComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is called when the receiving of a frame is complete by the
 * upper layer driver so that the memory associated with the frame can be freed.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param pFrame      -   Pointer to the received frame.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_RxFrmComplete(UMI_HANDLE umiHandle, void *pFrame)
{
	UMI_CONNECTION *pUmiCon = (UMI_CONNECTION *) umiHandle;

	UMI_OS_ASSERT(pUmiCon);
	UMI_OS_ASSERT(pFrame);

	UMAC_ReleaseMemory(pUmiCon->umacHandle, (WFM_HI_MESSAGE *) pFrame);

	return UMI_STATUS_SUCCESS;
}
