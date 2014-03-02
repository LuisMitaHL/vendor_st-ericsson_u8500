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
 * \file UMI_UmacDil.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_UmacDil.h
 * \brief
 * This file implements the communication between UMAC core and Lower Layer
 * Driver.
 * \ingroup Upper_MAC_Interface
 * \date 20/01/09 14:42
 */

/*-----------------------------------------------------------------------------
			     INCLUDE FILES
-----------------------------------------------------------------------------*/
#ifndef _UMAC_DIL_H
#define _UMAC_DIL_H

#include "umac_ll_if.h"
#include "umac_if.h"
#include "UMI_Api.h"
#include "sysconfig.h"

#define DIL_UMAC_MAX_NUM_RX_BUFF 16
#define MAX_OUTSTANDING_REQUESTS 8
#define QueueId_BE 0
#define QueueId_BK 1
#define QueueId_VI 2
#define QueueId_VO 3
#define QueueId_CTRL 4

#define UMAC_HI_MSG_ID		   0x003F
#define UMAC_HI_MSG_LINK_ID	      0x03C0
#define UMAC_HI_MSG_TYPE		 0x0C00

#define NUM_PRIO_QUEUE  5
#define PRIO_QUEUE_SIZE 128

typedef struct DIL_UMAC_HI_MSG_Q_S {
	uint32 Put;
	uint32 Get;
	UMI_GET_TX_DATA *UmacHiBuff[PRIO_QUEUE_SIZE];
} DIL_UMAC_HI_MSG_Q;

/* Pool of buffers for management requests(set/get OIDs)*/
typedef struct DIL_BUFF_POOL_S {
	uint8 Get;
	uint8 Put;
	void *pMem[DIL_UMAC_MAX_NUM_RX_BUFF];
} DIL_BUFF_POOL;

/*Required to free the variables which remain throughout the life of UMAC*/
typedef struct LL_PRIVATE_S {
	void *pQueue[NUM_PRIO_QUEUE];
	void *pDILReq;
	void *pBuffPool;
	void *pWriteReq;
	void *pReadReq;
	void *pConfigReq;
	UMI_WAIT_HANDLE TimedWaitHandle;	/*Handle to be used to
						   wait for an event */
	LOCK_HANDLE txQueueLock;
} LL_PRIVATE;

typedef struct DIL_Req_Q_S {
	uint8 Put;
	uint8 Get;
	void *request[MAX_OUTSTANDING_REQUESTS];
} DIL_Req_Q;

#define UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle)	\
	umac_allocate_txbuf((void *)UmacHandle)
#define UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pTxDesc) \
	umac_release_txbuf(UmacHandle, pTxDesc)

/******************************************************************************
 * NAME:	DIL_HiMsgHandler
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the HI Messages received from LMAC.
 * \param  UmacHandle      - Pointer to the lower driver instance
 *			     for UMI callback.
 * \param  pMsg	           - Pointer to the HI Msg.
 * \param  pDriverInfo     - Pointer to the Driver Info.
 * \returns None
 *****************************************************************************/
void DIL_HiMsgHandler(UMAC_HANDLE UMACHandle, void *pMsg, void *pDriverInfo);

/******************************************************************************
 * NAME:	DIL_ReqTxFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function queues the frame to be transmitted.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  queueId	   - QueueId of the frame to be transmitted.
 * \param  pData	   - Pointer to TX data frame.
 * \param  linkId	   - Link Id of the STA.
 * \returns none.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqTxFrame(LL_INFO *pLowerLayerInfo,
			       uint8 queueId,
			       void *pData,
			       uint8 linkId);

/******************************************************************************
 * NAME:	DIL_ReqDevConf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets the Configuration Data.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pMsg	           - Pointer to Configuration structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqDevConf(LL_INFO *pLowerLayerInfo, void *pHiMsg);

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
				 void *pScanParameters);

/******************************************************************************
 * NAME:	DIL_StopScan
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to stop an in-progress scan operation .
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_StopScan(LL_INFO *pLowerLayerInfo);

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
UMI_STATUS_CODE DIL_ReqJoin(LL_INFO *pLowerLayerInfo, void *pJoinParameters);

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
				   void *pMeasurementReqParams);
#endif

/******************************************************************************
 * NAME:	DIL_ReqResetJoin
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to LMAC to return to its initial state.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  Flags	   - Flags for join Reset.
 * \param  linkId	   - Link Id of the STA.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqResetJoin(LL_INFO *pLowerLayerInfo,
				 uint32 Flags,
				 uint8 linkId);

/******************************************************************************
 * NAME:	DIL_ReqAddKeyEntry
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function adds the key entry.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pKeyData	   - Pointer to Key Data Structure.
 * \param  linkId	   - LinkId for add key request
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqAddKeyEntry(LL_INFO *pLowerLayerInfo,
				   void *pKeyData,
				   uint8 linkId);

/******************************************************************************
 * NAME:	DIL_ReqRemoveKeyEntry
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function removes the key entry.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  keyEntryIndex   - Index of the key to be removed.
 * \param  linkId	   - LinkId for add key request
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqRemoveKeyEntry(LL_INFO *pLowerLayerInfo,
				      uint8 keyEntryIndex,
				      uint8 linkId);

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
UMI_STATUS_CODE DIL_ReqWriteMib(LL_INFO *pLowerLayerInfo, void *pMibParameters);

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
UMI_STATUS_CODE DIL_ReqReadMib(LL_INFO *pLowerLayerInfo, void *pMibParameters);

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
UMI_STATUS_CODE DIL_ReqTxEdcaParams(LL_INFO *pLowerLayerInfo,
				    void *pEdcaParams);

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
					void *pQueueParams);

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
				    void *pBssParameters);

/******************************************************************************
 * NAME:	DIL_ReqPSPowerMgmtMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests the LMAC to enable/disable the power save mode.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pPmMode	   - Pointer to UMI Power Mgmt Mode structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqPSPowerMgmtMode(LL_INFO *pLowerLayerInfo, void *pPmMode);

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
				     void *pSwitchChannel);

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
UMI_STATUS_CODE DIL_ReqReadMemory(LL_INFO *pLowerLayerInfo, void *pHiMsg);

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
UMI_STATUS_CODE DIL_ReqWriteMemory(LL_INFO *pLowerLayerInfo, void *pHiMsg);

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:DIL_ReqMapLink
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends request to LMAC to accept new incoming link.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pMapLink	   - Pointer to map link request structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqMapLink(LL_INFO *pLowerLayerInfo,
			       void *pMapLink,
			       uint8 linkId);

/******************************************************************************
 * NAME:	DIL_ReqStart
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to configure the device in AP like mode.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \param  pStartReq       - Pointer to start request structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqStart(LL_INFO *pLowerLayerInfo, void *pStartReq);

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
UMI_STATUS_CODE DIL_ReqBcnTxReq(LL_INFO *pLowerLayerInfo, void *pBcnTxReq);

/******************************************************************************
 * NAME:DIL_ReqStartFind
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to start the FIND phase.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqStartFind(LL_INFO *pLowerLayerInfo);

/******************************************************************************
 * NAME:DIL_ReqStopFind
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to stop the FIND phase.
 * \param  pLowerLayerInfo - Pointer to LL_INFO structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_ReqStopFind(LL_INFO *pLowerLayerInfo);

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
UMI_STATUS_CODE DIL_ReqUpdateIE(LL_INFO *pLowerLayerInfo, void *pUpdateIE);

#endif	/*P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:	DIL_InsertQ
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Inserts the HI Msg into the queue.
 * \param  pLowerLayerInfo - Pointer to the lower driver instance
 *			     for UMI callback.
 * \param  pMsg	           - Pointer to the HI Msg to be inserted.
 * \param  queueId	   - Access category queue number.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_InsertQ(LL_INFO *pLowerLayerInfo,
			    void *pMsg,
			    uint8 queueId);

/******************************************************************************
 * NAME:	DIL_RxComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Through this API upper layer indicate receiving is finished.
 * \param  pLowerLayerInfo - Pointer to the lower driver instance
 *			     for UMI callback.
 * \param  pFrame	   - Pointer to the received frame.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
void DIL_RxComplete(LL_INFO *pLowerLayerInfo, void *pFrame, void *pDriverInfo);

/******************************************************************************
 * NAME:	DIL_Init
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the Device Interface Layer.
 * \param  pLowerLayerInfo - Pointer to the Lower Layer Info structure.
 * \returns none
 *****************************************************************************/
void DIL_Init(LL_INFO *pLLInfo);

/******************************************************************************
 * NAME:	DIL_DeInit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Through this API, memory for private data structures of DIL is deinitialized.
 * \param  pLowerLayerInfo     - Pointer to the LL_INFO structure.
 * \returns UMI_STATUS_CODE.
 *****************************************************************************/
UMI_STATUS_CODE DIL_DeInit(LL_INFO *pLowerLayerInfo);

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
UMI_STATUS_CODE DIL_ScheduleTx(LL_INFO *pLowerLayerInfo);

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
UMI_STATUS_CODE DIL_PutQueue(void *queue, void *insertPtr);

/******************************************************************************
 * NAME:	DIL_GetQueue
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrieves the head of the queue.
 * \param  queue - Pointer to the queue in which head is to be retrieved.
 * \returns Element at the head of the queue.
 *****************************************************************************/
void *DIL_GetQueue(void *queue);

#endif	/* _UMAC_DIL_H */
