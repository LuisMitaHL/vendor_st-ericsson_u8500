/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup Device_Interface_Layer
 * \brief
 * This layer is an interface to the lower layer driver. It performs two tasks.
 * a)	It converts the function call from the UMC into HI message. It maintains
 * a FIFO queue. Whenever there is a request from the UMC, it is queued in this
 * FIFO queue and the layer notifies the Device Interface Driver about it. So
 * that Device Interface Driver can pull this HI message at its convenience.
 * b)	Whenever it receives HI messages from the Device Interface Driver, it
 * parses its HI message header for message ID. And based upon the message IDs
 * it calls different functions of UMC.
 *
 */
/**
 * \file UMI_UmacDil.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_UmacDil.c
 * \brief
 * This file implements the communication between UMAC core and Lower Layer
 * Driver.
 * \ingroup Device_Interface_Layer
 * \date 20/01/09 14:42
 */

/*****************************************************************************/
/********		       Include Files			      ********/
/*****************************************************************************/
#include "UMI_UmacDil.h"
#include "wsm_api.h"
#include "hi_api.h"
#include "UMI_Queue.h"
#include "UMI_DebugTrace.h"
#include "lmac_callbacks.h"
#include "lmac_if.h"
#include "UMI_OsIf.h"

#define DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(x, y) {		\
		if (y->MsgLen < WFM_ETH_FRAME_HDR_SIZE + 4) {	\
			x->bufferLength = y->MsgLen;		\
		}						\
		else {						\
			x->bufferLength = y->MsgLen - WFM_ETH_FRAME_HDR_SIZE ; \
		}							       \
		x->pTxDesc	= (uint8 *)y ;				\
		x->pDot11Frame    = (uint8 *)y + x->bufferLength ;	\
		x->pDriverInfo    = NULL  ;				\
}

/******************************************************************************
 * NAME:	DIL_Init
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the Device Interface Layer.
 * \param  pLowerLayerInfo - Pointer to the Lower Layer Info structure.
 * \returns none
 *****************************************************************************/
void DIL_Init(LL_INFO *pLowerLayerInfo)
{
	int i = 0;
	LL_PRIVATE *llPvt = NULL;
	DIL_BUFF_POOL *pDilBuffPool = NULL;
	DIL_Req_Q *pDilReadReq = NULL;
	DIL_Req_Q *pDilWriteReq = NULL;
	UMI_TRACE(UMI_UmacDil, ("---> DIL_Init()\n"));

	pDilReadReq = (DIL_Req_Q *) UMI_Allocate(sizeof(DIL_Req_Q));
	if (NULL == pDilReadReq)
		UMI_OS_ASSERT(0);
	UMI_MemoryReset(pDilReadReq, sizeof(DIL_Req_Q));

	pDilWriteReq = (DIL_Req_Q *) UMI_Allocate(sizeof(DIL_Req_Q));
	if (NULL == pDilWriteReq)
		UMI_OS_ASSERT(0);
	UMI_MemoryReset(pDilWriteReq, sizeof(DIL_Req_Q));

	llPvt = (LL_PRIVATE *) UMI_Allocate(sizeof(LL_PRIVATE));
	if (NULL == llPvt)
		UMI_OS_ASSERT(0);
	UMI_MemoryReset(llPvt, sizeof(LL_PRIVATE));
	for (i = 0; i < NUM_PRIO_QUEUE; i++) {
		llPvt->pQueue[i] = (DIL_UMAC_HI_MSG_Q *) UMI_Allocate(sizeof(DIL_UMAC_HI_MSG_Q));
		if (NULL == llPvt->pQueue[i])
			UMI_OS_ASSERT(0);
		UMI_MemoryReset(llPvt->pQueue[i], sizeof(DIL_UMAC_HI_MSG_Q));
	}

	pDilBuffPool = (DIL_BUFF_POOL *) UMI_Allocate(sizeof(DIL_BUFF_POOL));
	if (NULL == pDilBuffPool)
		UMI_OS_ASSERT(0);
	pDilBuffPool->Get = pDilBuffPool->Put = 0;
	/* Allocate memory pool for Get/Set OID requests */
	for (i = 0; i < DIL_UMAC_MAX_NUM_RX_BUFF; i++) {
		pDilBuffPool->pMem[i] = (void *)UMI_Allocate(sizeof(UMI_GET_TX_DATA));
		UMI_MemoryReset(pDilBuffPool->pMem[i], sizeof(UMI_GET_TX_DATA));
	}
	llPvt->pBuffPool = pDilBuffPool;
	llPvt->pReadReq = pDilReadReq;
	llPvt->pWriteReq = pDilWriteReq;

	llPvt->txQueueLock = UMI_AllocateLock();

	pLowerLayerInfo->pLLPrivate = llPvt;

	pLowerLayerInfo->AllowCtrlQueueId = FALSE;
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_Init()\n"));
}

/******************************************************************************
 * NAME:	DIL_PutQueue
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function inserts the value in the provided queue.
 * \param  queue      - Pointer to the queue in which parameter
 *		      pointer is to be inserted.
 * \param  insertPtr  - Pointer to be inserted.
 * \returns UMI_STATUS_CODE
 *****************************************************************************/
UMI_STATUS_CODE DIL_PutQueue(void *queue, void *insertPtr)
{
	DIL_Req_Q *dilRequestQ = NULL;
	uint8 putIndex = 0;
	uint8 getIndex = 0;
	uint32 statusCode = UMI_STATUS_SUCCESS;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_PutQueue()\n"));
	dilRequestQ = (DIL_Req_Q *) queue;

	putIndex = dilRequestQ->Put;
	getIndex = dilRequestQ->Get;

	if (NULL == insertPtr) {
		UMI_ERROR(UMI_UmacDil, ("DIL_InsertReadQueue(): Unexpected NULL pMsg\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	/* Check Queue overflow */

	if (((putIndex + 1) % MAX_OUTSTANDING_REQUESTS) == getIndex) {
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	dilRequestQ->request[putIndex] = insertPtr;
	dilRequestQ->Put++;
	dilRequestQ->Put = dilRequestQ->Put & (MAX_OUTSTANDING_REQUESTS - 1);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_PutQueue()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_GetQueue
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrieves the head of the queue.
 * \param  queue - Pointer to the queue in which head is to be retrieved.
 * \returns Element at the head of the queue.
 *****************************************************************************/
void *DIL_GetQueue(void *queue)
{
	void *head = NULL;
	DIL_Req_Q *dilRequestQ = NULL;
	uint8 index = 0;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_GetQueue()\n"));
	dilRequestQ = (DIL_Req_Q *) queue;

	if (dilRequestQ->Put == dilRequestQ->Get) {
		UMI_ERROR(UMI_UmacDil, ("DIL_GetQueue(): queue Empty\n"));
		return NULL;
	} else {
		index = (dilRequestQ->Get++) % MAX_OUTSTANDING_REQUESTS;
		head = dilRequestQ->request[index];
		dilRequestQ->Get = dilRequestQ->Get & (MAX_OUTSTANDING_REQUESTS - 1);
		UMI_TRACE(UMI_UmacDil, ("<--- DIL_GetQueue()\n"));
		return head;
	}
}

/******************************************************************************
 * NAME:	DIL_ReqTxFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function queues the frame to be transmitted.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  queueId	 - QueueId of the frame to be transmitted.
 * \param  pData	   - Pointer to TX data frame.
 * \param  linkId	  - Link Id of the STA
 * \returns none.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqTxFrame(LL_INFO *pLowerLayerInfo,
			       uint8 queueId,
			       void *pData,
			       uint8 linkId)
{
	uint32 statusCode;

	UMAC_DEVIF_TX_DATA *pDevIfTxData = (UMAC_DEVIF_TX_DATA *) pData;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqTxFrame()\n"));

#if P2P_MINIAP_SUPPORT
	pDevIfTxData->pTxDescriptor->MsgId = WSM_HI_TX_REQ_ID | linkId << 6;
#else
	pDevIfTxData->pTxDescriptor->MsgId = WSM_HI_TX_REQ_ID;
#endif				/*P2P_MINIAP_SUPPORT */

	statusCode = DIL_InsertQ(
				pLowerLayerInfo,
				(UMI_GET_TX_DATA *) pDevIfTxData,
				queueId
				);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqTxFrame()\n"));

	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqDevConf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets the Configuration Data.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pMsg	    - Pointer to Configuration structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqDevConf(LL_INFO *pLowerLayerInfo, void *pMsg)
{
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;
	HI_MSG_HDR *pHiMsg = (HI_MSG_HDR *) pMsg;
	LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqDevConf()\n"));

	if (NULL == pHiMsg) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqDevConf(): Unexpected \
			     NULL pHiMsg\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	pUmiGetTxData = (UMI_GET_TX_DATA *) ((uint8 *) pMsg - sizeof(UMAC_DEVIF_TX_DATA));

	llPvt->pConfigReq = (uint8 *) pHiMsg;

	pHiMsg->MsgId = WSM_HI_CONFIGURATION_REQ_ID;
	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pHiMsg);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqDevConf()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_InitiateScan
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to start the scanning process .
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pScanParameters - Pointer to SCAN_PARAMETERS structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_InitiateScan(LL_INFO *pLowerLayerInfo,
				 void *pScanParameters)
{
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;
	HI_MSG_HDR *pHiMsg;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_InitiateScan()\n"));

	if (NULL == pScanParameters) {
		UMI_ERROR(UMI_UmacDil, ("DIL_InitiateScan(): Unexpected NULL \
			    pScanParameters\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	pHiMsg = (HI_MSG_HDR *) ((uint8 *) pScanParameters - sizeof(HI_MSG_HDR));

	pUmiGetTxData = (UMI_GET_TX_DATA *) ((uint8 *) pHiMsg - sizeof(UMI_GET_TX_DATA));

	pHiMsg->MsgLen = sizeof(SCAN_PARAMETERS) + sizeof(HI_MSG_HDR);
	pHiMsg->MsgId = WSM_HI_START_SCAN_REQ_ID;

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pHiMsg);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_InitiateScan()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_StopScan
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to stop an in-progress scan operation .
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_StopScan(LL_INFO *pLowerLayerInfo)
{
	HI_MSG_HDR *pHiMsg;
	UMI_GET_TX_DATA *pUmiGetTxData;
	uint32 statusCode;

	LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_StopScan()\n"));

	pUmiGetTxData = (UMI_GET_TX_DATA *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	pHiMsg = (HI_MSG_HDR *) ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pHiMsg->MsgId = WSM_HI_STOP_SCAN_REQ_ID;
	pHiMsg->MsgLen = sizeof(HI_MSG_HDR);

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pHiMsg);

	llPvt->pDILReq = pUmiGetTxData;

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_StopScan()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqJoin
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to join a BSS or an IBSS or start an IBSS.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pJoinParameters - Pointer to join parameter structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqJoin(LL_INFO *pLowerLayerInfo, void *pJoinParameters)
{
	HI_MSG_HDR *pHiMsg;
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqJoin()\n"));

	if (NULL == pJoinParameters) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqWriteMib(): Unexpected NULL \
			   DIL_ReqJoin\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	pHiMsg = (HI_MSG_HDR *) ((uint8 *) pJoinParameters - sizeof(HI_MSG_HDR));

	pHiMsg->MsgId = WSM_HI_JOIN_REQ_ID;

	pHiMsg->MsgLen = sizeof(WSM_HI_JOIN_REQ);

	pUmiGetTxData = (UMI_GET_TX_DATA *) ((uint8 *) pHiMsg - sizeof(UMI_GET_TX_DATA));

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pHiMsg);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqJoin()\n"));
	return statusCode;
}

#if DOT11K_SUPPORT
/******************************************************************************
 * NAME:	DIL_ReqMeasurement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the measurement request to WSM.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pMeasurementReqParams - Pointer to Measurement parameter structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqMeasurement(LL_INFO *pLowerLayerInfo,
				   void *pMeasurementReqParams)
{
	HI_MSG_HDR *pHiMsg;
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqJoin()\n"));

	if (NULL == pMeasurementReqParams) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqWriteMib(): Unexpected NULL \
			   DIL_ReqMeasurement\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	pHiMsg = (HI_MSG_HDR *) pMeasurementReqParams;

	pHiMsg->MsgId = WSM_HI_START_MEASUREMENT_REQ_ID;

	pUmiGetTxData = (UMI_GET_TX_DATA *) ((uint8 *) pHiMsg - sizeof(UMI_GET_TX_DATA));

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pHiMsg);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqMeasurement()\n"));
	return statusCode;
}
#endif
/******************************************************************************
 * NAME:	DIL_ReqResetJoin
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to LMAC to return to its initial state.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  Flags	   - Flags for join Reset.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqResetJoin(LL_INFO *pLowerLayerInfo,
				 uint32 Flags,
				 uint8 linkId)
{
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_RESET_REQ *pResetReq;
	LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqResetJoin()\n"));
	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	pResetReq = (WSM_HI_RESET_REQ *) ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pResetReq->MsgId = WSM_HI_RESET_REQ_ID | linkId << 6;
	pResetReq->MsgLen = sizeof(WSM_HI_RESET_REQ);
	pResetReq->Flags = Flags;

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pResetReq);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqResetJoin()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:DIL_ReqAddKeyEntry
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function adds the key entry.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pKeyData	- Pointer to Key Data Structure.
 * \param  linkId	  - LinkId for add key request
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqAddKeyEntry(LL_INFO *pLowerLayerInfo,
				   void *pKeyData,
				   uint8 linkId)
{
	uint32 statusCode = UMI_STATUS_SUCCESS;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_ADD_KEY_REQ *pAddKey = NULL;
	LL_PRIVATE *llPvt;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqAddKeyEntry()\n"));

	if (NULL == pKeyData) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqAddKeyEntry(): Unexpected NULL pKeyData\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	pAddKey = (WSM_HI_ADD_KEY_REQ *) ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pAddKey->MsgId = WSM_HI_ADD_KEY_REQ_ID | linkId << 6;
	pAddKey->MsgLen = sizeof(WSM_HI_ADD_KEY_REQ);

	UMI_MemoryCopy(
		((uint8 *) pAddKey + sizeof(HI_MSG_HDR)),
		pKeyData,
		(sizeof(WSM_HI_ADD_KEY_REQ) - sizeof(HI_MSG_HDR))
		);

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pAddKey);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqAddKeyEntry()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:DIL_ReqRemoveKeyEntry
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function removes the key entry.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  keyEntryIndex   - Index of the key to be removed.
 * \param  linkId	  - LinkId for add key request
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqRemoveKeyEntry(LL_INFO *pLowerLayerInfo,
				      uint8 keyEntryIndex,
				      uint8 linkId)
{
	uint32 statusCode;
	WSM_HI_REMOVE_KEY_REQ *pRemoveKeyReq;
	UMI_GET_TX_DATA *pUmiGetTxData;
	LL_PRIVATE *llPvt;

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqRemoveKeyEntry()\n"));

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	pRemoveKeyReq = (WSM_HI_REMOVE_KEY_REQ *)
	    ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pRemoveKeyReq->MsgId = WSM_HI_REMOVE_KEY_REQ_ID | linkId << 6;
	pRemoveKeyReq->MsgLen = sizeof(WSM_HI_REMOVE_KEY_REQ);
	pRemoveKeyReq->EntryIndex = keyEntryIndex;

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pRemoveKeyReq);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqRemoveKeyEntry()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqWriteMib
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes MIB variable.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pMibParameters  - Pointer to the MIB Parameters.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqWriteMib(LL_INFO *pLowerLayerInfo, void *pMibParameters)
{
	uint32 statusCode;
	MIB_PARAMETERS *pMibParam;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_WRITE_MIB_REQ *pWriteMib;
	LL_PRIVATE *llPvt;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqWriteMib()\n"));

	if (NULL == pMibParameters) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqWriteMib(): Unexpected NULL \
			      pMibParameters\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}	/*if( NULL == pMibParameters ) */

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pMibParam = (MIB_PARAMETERS *) pMibParameters;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	pWriteMib = (WSM_HI_WRITE_MIB_REQ *)
	    ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pWriteMib->MsgId = WSM_HI_WRITE_MIB_REQ_ID;

	pWriteMib->MsgLen = (sizeof(HI_MSG_HDR) + (sizeof(MIB_PARAMETERS) - sizeof(pMibParam->MibData)) + pMibParam->Length);

	UMI_MemoryCopy(
		(uint8 *) pWriteMib + sizeof(HI_MSG_HDR),
		(uint8 *) pMibParameters,
		(pWriteMib->MsgLen - sizeof(HI_MSG_HDR))
		);

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pWriteMib);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqWriteMib()\n"));

	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqReadMib
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function reads MIB variable.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pMibParameters  - Pointer to the MIB Parameters.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqReadMib(LL_INFO *pLowerLayerInfo, void *pMibParameters)
{

	LL_PRIVATE *llPvt;
	WSM_HI_READ_MIB_REQ *pReadMib;
	UMI_GET_TX_DATA *pUmiGetTxData;
	uint32 statusCode;
	MIB_PARAMETERS *pMibParam;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqReadMib()\n"));

	if (NULL == pMibParameters) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqReadMib(): Unexpected NULL \
			   pMibParameters\n"));
		statusCode = UMI_STATUS_FAILURE;

		return statusCode;
	}	/*if( NULL == pMibParameters ) */

	pMibParam = (MIB_PARAMETERS *) pMibParameters;

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	pReadMib = (WSM_HI_READ_MIB_REQ *)
	    ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pReadMib->MsgId = WSM_HI_READ_MIB_REQ_ID;
	pReadMib->MsgLen = (sizeof(pMibParam->MibId) + sizeof(HI_MSG_HDR));

	UMI_MemoryCopy(
		(void *)&pReadMib->MibId,
		pMibParam,
		sizeof(MIB_PARAMETERS)
		);

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pReadMib);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqReadMib()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqTxEdcaParams
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function configures the EDCA parameters in LMAC.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pEdcaParams     - Pointer to EDCA Param structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqTxEdcaParams(LL_INFO *pLowerLayerInfo, void *pEdcaParams)
{
	uint32 statusCode;
	LL_PRIVATE *llPvt;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_EDCA_PARAMS_REQ *pEdcaParam;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqTxEdcaParams()\n"));

	if (NULL == pEdcaParams) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqTxEdcaParams(): Unexpected NULL \
			     pEdcaParams\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}	/*if( NULL == pEdcaParams ) */

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	pEdcaParam = (WSM_HI_EDCA_PARAMS_REQ *)
	    ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pEdcaParam->MsgId = WSM_HI_EDCA_PARAMS_REQ_ID;
	pEdcaParam->MsgLen = sizeof(WSM_HI_EDCA_PARAMS_REQ);

	UMI_MemoryCopy(
		(uint8 *) pEdcaParam + sizeof(HI_MSG_HDR),
		pEdcaParams,
		sizeof(EDCA_PARAMS)
		);

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pEdcaParam);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqTxEdcaParams()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqSetTxQueueParams
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets the transmission queues on the WLAN device.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pQueueParams    - Pointer to queue Param structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqSetTxQueueParams(LL_INFO *pLowerLayerInfo,
					void *pQueueParams)
{
	uint32 statusCode;
	LL_PRIVATE *llPvt;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_TX_QUEUE_PARAMS_REQ *pTxQParamsReq;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqSetTxQueueParams()\n"));

	if (NULL == pQueueParams) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqSetTxQueueParams(): Unexpected NULL pQueueParams\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	pTxQParamsReq = (WSM_HI_TX_QUEUE_PARAMS_REQ *)
	    ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pTxQParamsReq->MsgId = WSM_HI_TX_QUEUE_PARAMS_REQ_ID;
	pTxQParamsReq->MsgLen = sizeof(WSM_HI_TX_QUEUE_PARAMS_REQ);

	UMI_MemoryCopy(
		(void *)((uint8 *) pTxQParamsReq + sizeof(HI_MSG_HDR)),
		(void *)pQueueParams,
		(sizeof(WSM_HI_TX_QUEUE_PARAMS_REQ) - sizeof(HI_MSG_HDR))
		);

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pTxQParamsReq);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqSetTxQueueParams()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqSynBssParams
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Sets the Lower MAC's connection parameters after the initial connection setup
 * and is called prior to putting the WLAN device into power save. Used in
 * infrastructure mode only.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pBssParameters  - Pointer to BSS Parameters.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqSynBssParams(LL_INFO *pLowerLayerInfo,
				    void *pBssParameters)
{
	uint32 statusCode;
	LL_PRIVATE *llPvt;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_SET_BSS_PARAMS_REQ *pSetBssParam;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqSynBssParams()\n"));

	if (NULL == pBssParameters) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqSynBssParams(): Unexpected NULL pBssParameters\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	pSetBssParam = (WSM_HI_SET_BSS_PARAMS_REQ *)
	    ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pSetBssParam->MsgId = WSM_HI_SET_BSS_PARAMS_REQ_ID;
	pSetBssParam->MsgLen = sizeof(WSM_HI_SET_BSS_PARAMS_REQ);

	pSetBssParam->Flags = ((BSS_PARAMETERS *) pBssParameters)->Falgs;

	pSetBssParam->BeaconLostCount = ((BSS_PARAMETERS *)
					 pBssParameters)->BeaconLostCount;

	pSetBssParam->AID = ((BSS_PARAMETERS *) pBssParameters)->AID;

	pSetBssParam->OperationalRateSet = ((BSS_PARAMETERS *) pBssParameters)->OperationalRateSet;

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pSetBssParam);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqSynBssParams()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqPSPowerMgmtMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests the LMAC to enable/disable the power save mode.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pPmMode	  - Pointer to UMI power Management Mode.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqPSPowerMgmtMode(LL_INFO *pLowerLayerInfo, void *pPmMode)
{
	WSM_HI_SET_PM_MODE_REQ *pPmModeReq;
	LL_PRIVATE *llPvt;
	UMI_GET_TX_DATA *pUmiGetTxData;
	uint32 statusCode;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqPSPowerMgmtMode()\n"));

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	pPmModeReq = (WSM_HI_SET_PM_MODE_REQ *)
	    ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pPmModeReq->MsgId = WSM_HI_SET_PM_MODE_REQ_ID;
	pPmModeReq->MsgLen = sizeof(WSM_HI_SET_PM_MODE_REQ);

	UMI_MemoryCopy(
		(uint8 *) pPmModeReq + sizeof(HI_MSG_HDR),
		(uint8 *) pPmMode,
		(pPmModeReq->MsgLen - sizeof(HI_MSG_HDR))
		);

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pPmModeReq);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqPSPowerMgmtMode()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqSwitchChannel
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests the LMAC to switch channel.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pSwitchChannel  - Pointer to switch channel.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqSwitchChannel(LL_INFO *pLowerLayerInfo,
				     void *pSwitchChannel)
{
	uint32 statusCode;
	LL_PRIVATE *llPvt;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_SWITCH_CHANNEL_REQ *pSwitchChReq;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqSwitchChannel()\n"));

	if (NULL == pSwitchChannel) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqSwitchChannel(): Unexpected NULL pSwitchChReq\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	pSwitchChReq = (WSM_HI_SWITCH_CHANNEL_REQ *)
	    ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pSwitchChReq->MsgId = WSM_HI_SWITCH_CHANNEL_REQ_ID;
	pSwitchChReq->MsgLen = sizeof(WSM_HI_SWITCH_CHANNEL_REQ);

	UMI_MemoryCopy(
			(uint8 *)&pSwitchChReq->ChannelMode,
			(uint8 *)pSwitchChannel,
			(sizeof(WSM_HI_SWITCH_CHANNEL_REQ) - 4)
			);
#if 0
	pSwitchChReq->ChannelMode = ((SWITCH_CHANNEL *)
				     pSwitchChannel)->ChannelMode;

	pSwitchChReq->ChannelSwitchCount = ((SWITCH_CHANNEL *)
					    pSwitchChannel)->ChannelSwitchCount;

	pSwitchChReq->NewChannelNumber = ((SWITCH_CHANNEL *)
					  pSwitchChannel)->NewChannelNumber;
#endif
	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pSwitchChReq);

	if (pSwitchChReq->ChannelMode == WSM_HI_SC_CHANNELMODE_ENHANCED) {
		pLowerLayerInfo->AllowCtrlQueueId = TRUE;
		statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_CTRL);
	} else
		statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqSwitchChannel()\n"));
	return statusCode;
}

/******************************************************************************
			   Common HI Requests
******************************************************************************/

/******************************************************************************
 * NAME:	DIL_ReqReadMemory
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Device read memory request.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pMsg	    - Pointer to Read Memory HI Msg.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqReadMemory(LL_INFO *pLowerLayerInfo, void *pMsg)
{
	uint32 statusCode;
	LL_PRIVATE *llPvt;
	UMI_GET_TX_DATA *pUmiGetTxData;

	HI_MEM_READ_REQ *pMemReadReq = (HI_MEM_READ_REQ *) pMsg;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqReadMemory()\n"));

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    ((uint8 *) pMsg - sizeof(UMI_GET_TX_DATA));

	pMemReadReq->MsgId = HI_MEM_READ_REQ_ID;

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pMemReadReq);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqReadMemory()\n"));
	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_ReqWriteMemory
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Device write memory request.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pMsg	    - Pointer to Write Memory HI Msg.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqWriteMemory(LL_INFO *pLowerLayerInfo, void *pMsg)
{
	uint32 statusCode;
	LL_PRIVATE *llPvt;
	UMI_GET_TX_DATA *pUmiGetTxData;
	HI_MEM_WRITE_REQ *pMemWriteReq = (HI_MEM_WRITE_REQ *) pMsg;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqWriteMemory()\n"));

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    ((uint8 *) pMsg - sizeof(UMI_GET_TX_DATA));

	pMemWriteReq->MsgId = HI_MEM_WRITE_REQ_ID;

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pMemWriteReq);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqWriteMemory()\n"));

	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_InsertQ
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Inserts the HI Msg into the queue.
 * \param  pLowerLayerInfo - Pointer to the lower driver instance
 *					for UMI callback.
 * \param  pMsg	    - Pointer to the HI Msg to be inserted.
 * \param  queueId	 - Access category queue number.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_InsertQ(LL_INFO *pLowerLayerInfo, void *pMsg, uint8 queueId)
{
	DIL_UMAC_HI_MSG_Q *dilUmacHiMsgQ;
	LL_PRIVATE *llPvt;
	uint32 putIndex;

	uint32 statusCode = UMI_STATUS_SUCCESS;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_InsertQ()\n"));

	if (NULL == pMsg) {
		UMI_ERROR(UMI_UmacDil, ("DIL_InsertQ(): Unexpected NULL pMsg\n"));

		statusCode = UMI_STATUS_FAILURE;

		return statusCode;
	} /* if ( NULL == pMsg ) */

	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	dilUmacHiMsgQ = (DIL_UMAC_HI_MSG_Q *) llPvt->pQueue[queueId];

	UMI_AcquireLock(llPvt->txQueueLock);

	putIndex = dilUmacHiMsgQ->Put;

	/* Check Queue overflow */

	if (((putIndex + 1) % PRIO_QUEUE_SIZE) == dilUmacHiMsgQ->Get) {
		statusCode = UMI_STATUS_FAILURE;
		UMI_ReleaseLock(llPvt->txQueueLock);
		return statusCode;

	} /*if( ((putIndex + 1) % PRIO_QUEUE_SIZE) == dilUmacHiMsgQ->Get ) */

	dilUmacHiMsgQ->UmacHiBuff[putIndex] = pMsg;
	putIndex++;
	dilUmacHiMsgQ->Put = (putIndex & (PRIO_QUEUE_SIZE - 1));

	UMI_ReleaseLock(llPvt->txQueueLock);

	/*Schedule the TX */
	if ((pLowerLayerInfo->AllowCtrlQueueId == FALSE)
	   || ((pLowerLayerInfo->AllowCtrlQueueId == TRUE) && (queueId == QueueId_CTRL)))
		pLowerLayerInfo->LL_FnCalls.LL_TX_FN(pLowerLayerInfo->LowerLayerHandle);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_InsertQ()\n"));

	return statusCode;
}

/******************************************************************************
 * NAME:	DIL_RxComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Through this API upper layer indicate receiving is finished.
 * \param  pLowerLayerInfo - Pointer to the lower driver instance
 *					for UMI callback.
 * \param  pFrame	  - Pointer to the received frame.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
void DIL_RxComplete(LL_INFO *pLowerLayerInfo, void *pFrame, void *pDriverInfo)
{
	UMI_TRACE(UMI_UmacDil, ("---> RxComplete()\n"));
	if (NULL != pLowerLayerInfo->LL_FnCalls.LL_RX_COMPLETE)
		pLowerLayerInfo->LL_FnCalls.LL_RX_COMPLETE(pLowerLayerInfo->LowerLayerHandle, pFrame, pDriverInfo);
	UMI_TRACE(UMI_UmacDil, ("<--- RxComplete()\n"));
}

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:DIL_ReqMapLink
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends request to LMAC to accept new incoming link.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pMapLink	- Pointer to map link request structure.
 * \param  linkId	  - Link ID to be associated with this request.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqMapLink(LL_INFO *pLowerLayerInfo,
			       void *pMapLink,
			       uint8 linkId)
{
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;
	HI_MSG_HDR *pHiMsg;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqMapLink()\n"));

	if (NULL == pMapLink) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqMapLink(): Unexpected NULL \
			     pMapLink\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}	/*if( NULL == pMapLink ) */

	pHiMsg = (HI_MSG_HDR *) (uint8 *) pMapLink;

	/* Link ID: b6-b9 */
	pHiMsg->MsgId = WSM_HI_MAP_LINK_REQ_ID | (linkId << 6);

	pHiMsg->MsgLen = sizeof(WSM_HI_MAP_LINK_REQ);

	pUmiGetTxData = (UMI_GET_TX_DATA *) ((uint8 *) pHiMsg - sizeof(UMI_GET_TX_DATA));

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pHiMsg);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqMapLink()\n"));
	return statusCode;
}	/*DIL_ReqMapLink() */

/******************************************************************************
 * NAME:DIL_ReqStart
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to configure the device in AP like mode.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pStartReq       - Pointer to start request structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqStart(LL_INFO *pLowerLayerInfo, void *pStartReq)
{
	HI_MSG_HDR *pHiMsg;
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqStart()\n"));

	if (NULL == pStartReq) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqWriteMib(): Unexpected NULL pStartReq\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	pHiMsg = (HI_MSG_HDR *) (uint8 *) pStartReq;

	pHiMsg->MsgId = WSM_HI_START_REQ_ID;

	pHiMsg->MsgLen = sizeof(WSM_HI_START_REQ);

	pUmiGetTxData = (UMI_GET_TX_DATA *) ((uint8 *) pHiMsg - sizeof(UMI_GET_TX_DATA));

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pHiMsg);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqStart()\n"));
	return statusCode;
}	/*DIL_ReqStart() */

/******************************************************************************
 * NAME:DIL_ReqBcnTxReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to configure the device in AP like mode.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pBcnTxReq       - Pointer to start request structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqBcnTxReq(LL_INFO *pLowerLayerInfo, void *pBcnTxReq)
{
	HI_MSG_HDR *pHiMsg;
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqBcnTxReq()\n"));

	if (NULL == pBcnTxReq) {
		UMI_ERROR(UMI_UmacDil, ("DIL_ReqBcnTxReq(): Unexpected NULL pBcnTxReq\n"));
		statusCode = UMI_STATUS_FAILURE;
		return statusCode;
	}

	pHiMsg = (HI_MSG_HDR *) (uint8 *) pBcnTxReq;

	pHiMsg->MsgId = WSM_HI_BEACON_TRANSMIT_REQ_ID;

	pHiMsg->MsgLen = sizeof(WSM_HI_BEACON_TRANSMIT_REQ);

	pUmiGetTxData = (UMI_GET_TX_DATA *) ((uint8 *) pHiMsg - sizeof(UMI_GET_TX_DATA));

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pHiMsg);

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqBcnTxReq()\n"));
	return statusCode;
}	/*DIL_ReqBcnTxReq() */

/******************************************************************************
 * NAME:	DIL_ReqStartFind
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to LMAC to start find.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqStartFind(LL_INFO *pLowerLayerInfo)
{
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_START_FIND_REQ *pStartFindReq;
	LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqStartFind()\n"));
	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	pStartFindReq = (WSM_HI_START_FIND_REQ *) ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pStartFindReq->MsgId = WSM_HI_START_FIND_REQ_ID;
	pStartFindReq->MsgLen = sizeof(WSM_HI_START_FIND_REQ);

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pStartFindReq);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqStartFind()\n"));
	return statusCode;
}	/* End DIL_ReqStartFind() */

/******************************************************************************
 * NAME:DIL_ReqStopFind
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to stop the FIND phase.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqStopFind(LL_INFO *pLowerLayerInfo)
{
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_STOP_FIND_REQ *pStopFindReq;
	LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqStopFind()\n"));
	pUmiGetTxData = (UMI_GET_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(pLowerLayerInfo);

	UMI_OS_ASSERT(pUmiGetTxData);

	pStopFindReq = (WSM_HI_STOP_FIND_REQ *) ((uint8 *) pUmiGetTxData + sizeof(UMI_GET_TX_DATA));

	pStopFindReq->MsgId = WSM_HI_STOP_FIND_REQ_ID;
	pStopFindReq->MsgLen = sizeof(WSM_HI_STOP_FIND_REQ);

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pStopFindReq);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqStopFind()\n"));
	return statusCode;
}	/* End DIL_ReqStopFind() */

/******************************************************************************
 * NAME:DIL_ReqUpdateIE
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to update information element(s) in the firmware.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pUpdateIE       - Pointer to Update IE.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqUpdateIE(LL_INFO *pLowerLayerInfo, void *pUpdateIE)
{
	HI_MSG_HDR *pHiMsg;
	uint32 statusCode;
	UMI_GET_TX_DATA *pUmiGetTxData;
	WSM_HI_UPDATE_IE_REQ *pUpdateIeReq = (WSM_HI_UPDATE_IE_REQ *) pUpdateIE;
	LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ReqUpdateIE()\n"));

	pHiMsg = (HI_MSG_HDR *) pUpdateIE;
	pUmiGetTxData = (UMI_GET_TX_DATA *) ((uint8 *) pHiMsg - sizeof(UMI_GET_TX_DATA));

	UMI_OS_ASSERT(pUmiGetTxData);

	pUpdateIeReq->MsgId = WSM_HI_UPDATE_IE_REQ_ID;

	DIL_PREPARE_INTERNAL_DEV_IF_TX_REQ(pUmiGetTxData, pUpdateIeReq);

	llPvt->pDILReq = (void *)pUmiGetTxData;

	statusCode = DIL_InsertQ(pLowerLayerInfo, pUmiGetTxData, QueueId_VO);
	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ReqUpdateIE()\n"));
	return statusCode;
}
#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:DIL_HiMsgHandler
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the HI Messages received from LMAC.
 * \param  UmacHandle      - Pointer to the lower driver instance
 *					for UMI callback.
 * \param  pMsg	    - Pointer to the HI Msg.
 * \param  pDriverInfo     - Pointer to the Driver Info.
 * \returns None
 *****************************************************************************/
void DIL_HiMsgHandler(UMAC_HANDLE UmacHandle, void *pMsg, void *pDriverInfo)
{

	LL_INFO *pLowerLayerInfo = (LL_INFO *) UmacHandle;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_HiMsgHandler()\n"));

	UMI_OS_ASSERT(pMsg);

	UMI_DEFAULT(UMI_UmacDil, ("DIL_HiMsgHandler(): MsgId  = %0x\n", ((HI_MSG_HDR *) pMsg)->MsgId));

	switch (((HI_MSG_HDR *) pMsg)->MsgId & ~UMAC_HI_MSG_LINK_ID) {
	case WSM_HI_RX_IND_ID:
		{
			LLtoUMAC_ReceiveFrame(UmacHandle, (UMAC_RX_DESC *) pMsg, pDriverInfo);
			break;
		}

	case WSM_HI_EVENT_IND_ID:
		{
			EVENT_INDICATION *pEventIND = (EVENT_INDICATION *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));
			LLtoUMAC_EventIndication(UmacHandle, pEventIND);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_SCAN_CMPL_IND_ID:
		{
			SCAN_COMPLETE *pScanCompInd = (SCAN_COMPLETE *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));
			LLtoUMAC_ScanComplete(UmacHandle, pScanCompInd);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_MEASURE_CMPL_IND_ID:
		{
			LLtoUMAC_MeasurementComplete(UmacHandle, pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_BA_TIMEOUT_IND_ID:
		{
			BLOCK_ACK_TIMEOUT *pBATimeoutInd = (BLOCK_ACK_TIMEOUT *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));
			LLtoUMAC_BlockAckTimeout(UmacHandle, pBATimeoutInd);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_SET_PM_MODE_CMPL_IND_ID:
		{
			SET_PM_MODE_COMPLETE *pSetPmModeCmpl = (SET_PM_MODE_COMPLETE *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));
			LLtoUMAC_SetPmModeComplete(UmacHandle, pSetPmModeCmpl);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_SWITCH_CHANNEL_IND_ID:
		{
			WSM_HI_SWITCH_CHANNEL_IND *pSwitchChInd = (WSM_HI_SWITCH_CHANNEL_IND *) pMsg;
			if (pSwitchChInd->State != WSM_HI_SC_STATE_SWITCHING)
				pLowerLayerInfo->AllowCtrlQueueId = FALSE;
			LLtoUMAC_SwitchChInd(UmacHandle, pSwitchChInd);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}
	case WSM_HI_TX_CNF_ID:
		{
			UMAC_TX_CNF_DESC *pTxDesc = (UMAC_TX_CNF_DESC *) pMsg;
			LLtoUMAC_TransmitFrameConfirm(UmacHandle, pTxDesc);
			break;
		}

	case WSM_HI_MULTI_TX_CNF_ID:
		{
			UMAC_TX_CNF_DESC *pTxDesc = (UMAC_TX_CNF_DESC *) pMsg;
			LLtoUMAC_TransmitFrameConfirm(UmacHandle, pTxDesc);
			break;
		}

	case WSM_HI_READ_MIB_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;
			if (NULL != llPvt)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);

			LLtoUMAC_ReadMIBConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, (void *)pMsg, NULL);
			break;
		}

	case WSM_HI_WRITE_MIB_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

			if (NULL != llPvt)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);

			LLtoUMAC_WriteMIBConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, (void *)pMsg, NULL);
			break;
		}

	case WSM_HI_START_SCAN_CNF_ID:
		{
			LLtoUMAC_StartScanConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_STOP_SCAN_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;
			if (NULL != pLowerLayerInfo->pLLPrivate)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);
			LLtoUMAC_StopScanConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_CONFIGURATION_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;
			if (NULL != pLowerLayerInfo->pLLPrivate)
				UMI_MemoryCopy(llPvt->pConfigReq, pMsg, sizeof(WSM_HI_CONFIGURATION_CNF));
			LLtoUMAC_ConfigConfirmation(UmacHandle, llPvt->pConfigReq);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_RESET_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;
			LMAC_RESET_CONFIRMATION *pResetCnf = (LMAC_RESET_CONFIRMATION *) pMsg;
			if (NULL != pLowerLayerInfo->pLLPrivate)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);
			LLtoUMAC_ResetConfirm(UmacHandle, pResetCnf);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_JOIN_CNF_ID:
		{
			JOIN_CONFIRM *pJoinCnf = (JOIN_CONFIRM *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));
			LLtoUMAC_JoinConfirm(UmacHandle, pJoinCnf);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_ADD_KEY_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

			if (NULL != llPvt)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);

			LLtoUMAC_AddKeyConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_REMOVE_KEY_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

			if (NULL != llPvt)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);

			LLtoUMAC_RemoveKeyConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_START_MEASUREMENT_CNF_ID:
		{
			LLtoUMAC_StartMeasurementConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_STOP_MEASUREMENT_CNF_ID:
		{
			LLtoUMAC_StopMeasurementConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_SET_PM_MODE_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

			if (NULL != llPvt)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);

			LLtoUMAC_SetPmModeConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_SET_BSS_PARAMS_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

			if (NULL != llPvt)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);

			LLtoUMAC_SetBssParamConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, (void *)pMsg, NULL);
			break;
		}

	case WSM_HI_SWITCH_CHANNEL_CNF_ID:
		{
			uint32 *pStatus;
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

			if (NULL != llPvt)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);
			pStatus = (uint32 *)((uint8 *)pMsg + sizeof(HI_MSG_HDR));
			if (*pStatus)
				pLowerLayerInfo->AllowCtrlQueueId = FALSE;
			LLtoUMAC_SwitchChannelConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, (void *)pMsg, NULL);
			break;
		}

	case WSM_HI_TX_QUEUE_PARAMS_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

			if (NULL != llPvt)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);

			LLtoUMAC_SetTxQueueParamConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, (void *)pMsg, NULL);
			break;
		}

	case WSM_HI_EDCA_PARAMS_CNF_ID:
		{
			LL_PRIVATE *llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

			if (NULL != llPvt)
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, llPvt->pDILReq);

			LLtoUMAC_SetEdcaParamConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, (void *)pMsg, NULL);
			break;
		}

	case WSM_HI_CONFIG_BLOCK_ACK_CNF_ID:
		{
			LLtoUMAC_SetConfigBlockAckConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}
	case HI_EXCEPTION_IND_ID:
		{
			LLtoUMAC_ExceptionIndication(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case HI_STARTUP_IND_ID:
		{
			LLtoUMAC_StartUpIndication(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case HI_GENERIC_IND_ID:
		{
			LLtoUMAC_GenericIndication(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}
	case HI_MEM_READ_CNF_ID:
		{
			LLtoUMAC_MemReadConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}
	case HI_MEM_WRITE_CNF_ID:
		{
			LLtoUMAC_MemWriteConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

#if P2P_MINIAP_SUPPORT

	case WSM_HI_START_CNF_ID:
		{
			WSM_HI_START_CNF *pStartCnf = (WSM_HI_START_CNF *) pMsg;
			/*(WSM_HI_START_CNF *)((uint8 *)pMsg + sizeof(HI_MSG_HDR)); */
			LLtoUMAC_STARTConfirmation(UmacHandle, pStartCnf);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_BEACON_TRANSMIT_CNF_ID:
		{
			WSM_HI_BEACON_TRANSMIT_CNF *pBcnTxCnf = (WSM_HI_BEACON_TRANSMIT_CNF *) pMsg;
			/*(WSM_HI_BEACON_TRANSMIT_CNF *)((uint8 *)pMsg + sizeof(HI_MSG_HDR)); */
			LLtoUMAC_BcnTxConfirmation(UmacHandle, pBcnTxCnf);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_MAP_LINK_CNF_ID:
		{
			WSM_HI_MAP_LINK_CNF *pMapLinkCnf = (WSM_HI_BEACON_TRANSMIT_CNF *) pMsg;
			LLtoUMAC_MapLinkConfirmation(UmacHandle, pMapLinkCnf);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_START_FIND_CNF_ID:
		{
			LLtoUMAC_StartFindConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_STOP_FIND_CNF_ID:
		{
			LLtoUMAC_StopFindConfirmation(UmacHandle, (HI_MSG_HDR *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_FIND_CMPL_IND_ID:
		{
			WSM_HI_FIND_CMPL_IND *pFindCompInd = (WSM_HI_FIND_CMPL_IND *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));
			LLtoUMAC_FindComplete(UmacHandle, pFindCompInd);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_UPDATE_IE_CNF_ID:
		{
			LLtoUMAC_UpdateIEConfirmation(UmacHandle, (WSM_HI_UPDATE_IE_CNF *) pMsg);
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			break;
		}

	case WSM_HI_SUSP_RESUME_TX_IND_ID:
		{
			LLtoUMAC_SuspendResumeTxInd(UmacHandle, (WSM_HI_SUSPEND_RESUME_TX_IND *) pMsg);
			/*DIL_RxComplete((LL_INFO *)UmacHandle, pMsg); */
			break;
		}
#endif	/*P2P_MINIAP_SUPPORT */

	default:
		{
			DIL_RxComplete((LL_INFO *) UmacHandle, pMsg, NULL);
			UMI_TRACE(UMI_UmacDil, (" Unknown HI Msg\n"));

		}
	}	/*switch( ((HI_MSG_HDR *)pMsg)->MsgId ) */

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_HiMsgHandler()\n"));

}	/*end DIL_HiMsgHandler() */

/******************************************************************************
 * NAME:	DIL_DeInit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Through this API, memory for private data structures of DIL is deinitialized.
 * \param  pLowerLayerInfo     - Pointer to the LL_INFO structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_DeInit(LL_INFO *pLowerLayerInfo)
{
	uint8 i = 0;
	UMI_STATUS_CODE statusCode = UMI_STATUS_SUCCESS;
	LL_PRIVATE *llPvt;
	DIL_BUFF_POOL *pDilBuffPool;
	llPvt = (LL_PRIVATE *) pLowerLayerInfo->pLLPrivate;

	if (NULL != llPvt) {
		pDilBuffPool = llPvt->pBuffPool;
		for (i = 0; i < DIL_UMAC_MAX_NUM_RX_BUFF; i++)
			UMI_Free(pDilBuffPool->pMem[i]);
		UMI_Free(pDilBuffPool);

		for (i = 0; i < NUM_PRIO_QUEUE; i++)
			UMI_Free(llPvt->pQueue[i]);

		UMI_Free(llPvt->pReadReq);
		UMI_Free(llPvt->pWriteReq);
		UMI_DestroyLock(llPvt->txQueueLock);
		UMI_Free(llPvt);
	}

	return statusCode;
}	/*end DIL_DeInit */

/******************************************************************************
 * NAME:	DIL_ScheduleTx
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This will trigger lower layer driver to get buffred frames from UMAC.
 * \param  pLowerLayerInfo - Pointer to the lower driver instance
 *					for UMI callback.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ScheduleTx(LL_INFO *pLowerLayerInfo)
{
	uint32 statusCode = UMI_STATUS_SUCCESS;

	UMI_TRACE(UMI_UmacDil, ("---> DIL_ScheduleTx()\n"));

	/*Schedule the TX */
	pLowerLayerInfo->LL_FnCalls.LL_TX_FN(pLowerLayerInfo->LowerLayerHandle);

	UMI_TRACE(UMI_UmacDil, ("<--- DIL_ScheduleTx()\n"));

	return statusCode;
}	/*end DIL_ScheduleTx */
