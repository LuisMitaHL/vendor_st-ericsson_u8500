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
 * \file umac_if.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_if.h
 * \brief
 * This header file defines the interface to core UMAC Component.
 * \ingroup Upper_MAC_Core
 * \date 11/01/09 14:15
 */

#ifndef _UMAC_IF_H
#define _UMAC_IF_H

/******************************************************************************
				INCLUDE FILES
******************************************************************************/

#include "umac_interface.h"
#include "umac_messages.h"

/******************************************************************************
				DATA STRUCTURES
******************************************************************************/

/*DefInition for UMAC_HANDLE*/
typedef void *UMAC_HANDLE;

typedef struct UMAC_TX_DATA_CNF_S UMAC_TX_DATA_CNF;
typedef struct UMAC_TX_MGMT_DATA_CNF_S UMAC_TX_MGMT_DATA_CNF;

/*
  Type of the function pointer the upper layer need to pass down to UMAC.
  All the messages to upper layer will be passed through this call.
  Returns : This function should return 1 for success and 0 for failure.
*/
typedef uint8(*UMAC_2UL_CB) (void *ulHandle, WFM_HI_MESSAGE *pMsg);

/*
  Callback for passing Rx frames to the upper layer.
  Returns: 1 for success and 0 for failure.
*/
typedef uint8(*UMAC_2UL_RX_FRM_CB) (void *ulHandle,
				    uint8 statusCode,
				    void *pFrame,
				    uint16 Length,
				    void *pDriverInfo,
				    void *pFrameStart,
				    uint32 Flags);

#if DOT11K_SUPPORT
typedef void *(*UMAC_2UL_BEACON_INFO_CB) (void *umiHandle,
					  uint8 *pBssid,
					  uint8 bssidIndex,
					  uint32 *pBssidCount);
#endif	/* DOT11K_SUPPORT */
/*
  Callback for passing Tx confirmation to the upper layer.
  Returns: 1 for success and 0 for failure.
*/
typedef uint8(*UMAC_2UL_TX_CNF_CB) (void *ulHandle, UMAC_TX_DATA_CNF *pTxCnf);

/*
  Callback for passing Tx Managment Frame confirmation to the upper layer.
  Returns: 1 for success and 0 for failure.
*/
typedef uint8(*UMAC_2UL_TX_MGMT_CNF_CB) (void *ulHandle, UMAC_TX_MGMT_DATA_CNF *pTxMgmtCnf);

/*Schedule Callback to be registered with umac*/
typedef void (*UMAC_2UL_SCHEDULE_CB) (void *ulHandle);

typedef void *DIL_HANDLE;

typedef uint8(*SchedTx) (DIL_HANDLE LowerHandle);
typedef void (*RecvComplete) (DIL_HANDLE LowerHandle, void *pFrame, void *pDriverInfo);

typedef struct UMAC_TO_LL_CALLS_S {
	SchedTx LL_TX_FN;
	RecvComplete LL_RX_COMPLETE;
} UMAC_TO_LL_CALLS;

typedef struct LL_INFO_S {
	DIL_HANDLE LowerLayerHandle;
	void *pLLPrivate;
	uint8 AllowCtrlQueueId;
	uint8 Reserved[3];
	UMAC_TO_LL_CALLS LL_FnCalls;
} LL_INFO;

typedef struct UMAC_TX_REQ_HDR_S {
	uint16 MessageLength;
	uint16 Reserved;
	uint32 Reference;
	uint32 Priority;
	uint32 Flags;
} UMAC_TX_REQ_HDR;

typedef struct UMAC_TX_CNF_HDR_S {
	uint16 MessageLength;
	uint16 Reserved;
	uint32 Reference;
	uint32 Status;
} UMAC_TX_CNF_HDR;

typedef struct UMAC_TX_DATA_S {
	uint16 Reserved;
	uint16 MsgId;
	UMAC_TX_REQ_HDR *pUlHdr;
	uint8 *pEtherHeader;
	uint8 *pEtherPayLoad;
	uint8 *UlInfo;
} UMAC_TX_DATA;

struct UMAC_TX_DATA_CNF_S {
	uint16 Reserved;
	uint16 MsgId;
	UMAC_TX_CNF_HDR *pUlHdr;
	uint8 *pEherHeader;
	uint8 *pEthrPayLoad;
	uint8 *UlInfo;
};

typedef struct UMAC_MGMT_TRANSMIT_REQ_S {
	uint32 linkId;
	uint32 FrameLength;
	uint8 *pTxMgmtData;
} UMAC_MGMT_TRANSMIT_REQ;

struct UMAC_TX_MGMT_DATA_CNF_S {
	uint32 Status;
	uint32 FrameLength;
	uint32 *pFrame;
};

/******************************************************************************
				EXTERNAL DEPENDENCIES
******************************************************************************/

extern void wfm_umac_task(UMAC_HANDLE UmacHandle);
extern uint16 WFM_HI_Proc_GetParamReq(UMAC_HANDLE UmacHandle,
				      WFM_HI_MESSAGE *hi_msg);
extern uint16 WFM_HI_Proc_SetParamReq(UMAC_HANDLE UmacHandle,
				      WFM_HI_MESSAGE *hi_msg);
extern uint16 WFM_HI_Proc_TransmitReq(UMAC_HANDLE UmacHandle,
				      WFM_HI_MESSAGE *pTxReq);
extern uint16 WFM_HI_Proc_ConfigReq(UMAC_HANDLE UmacHandle,
				    WFM_HI_MESSAGE *hi_msg);
extern uint16 WFM_HI_Proc_MemoryReadReq(UMAC_HANDLE UmacHandle,
					WFM_HI_MESSAGE *hi_msg);
extern uint16 WFM_HI_Proc_MemoryWriteReq(UMAC_HANDLE UmacHandle,
					 WFM_HI_MESSAGE *hi_msg);
extern uint16 WFM_HI_Proc_TransmitMgmtReq(UMAC_HANDLE UmacHandle,
					  UMAC_MGMT_TRANSMIT_REQ *pMgmtTxReq);

/******************************************************************************
				INTERFACE DEFINITION
******************************************************************************/

/******************************************************************************
 * NAME:	UMAC_Create
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for creating a new UMAC Instance.All the
 * resource allocation should happen here.
 * \param *ulHandle    -  Handle to UMAC Instance.
 * \param *pLlCb       - Function pointers to Lower layer.
 * \param *pMem	- Memory required for UMAC to operate.
 * \returns UMAC_HANDLE if success, otherwise NULL..
 *****************************************************************************/
UMAC_HANDLE UMAC_Create(void *ulHandle, UMAC_TO_LL_CALLS *pLlCb, void *pMem);

/******************************************************************************
 * NAME:	UMAC_Init
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for starting/initializing a UMAC Instance.
 * UmacHandle should be created with UMAC_Create() before calling this
 * function.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \param *llHandle       - Handle to Lower layer.
 * \param UlCb		  - Callback function pointer
 *			    for upper layer to recieve event data
 * \param UpperLayerRxCb  - Callback function for upper layer to recive
 *			    data frames.
 * \param UpperLayerTxcnf - Callback function for Tx confirmation.
 * \param ScheduleCb      - Callback to request the upper layer for context.
 * \param *ScheduleHndle  - The handle needs to be passed to the
 *			    ScheduleCb inorder to request a context.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
#if DOT11K_SUPPORT
WFM_STATUS_CODE UMAC_Init(UMAC_HANDLE UmacHandle,
			  void *llHandle,
			  UMAC_2UL_CB UlCb,
			  UMAC_2UL_RX_FRM_CB UpperLayerRxCb,
			  UMAC_2UL_BEACON_INFO_CB UpperLayerBeaconInfoCb,
			  UMAC_2UL_TX_CNF_CB UpperLayerTxcnf,
			  UMAC_2UL_TX_MGMT_CNF_CB UpperLayerTxMgmtcnf,
			  UMAC_2UL_SCHEDULE_CB ScheduleCb,
			  void *ScheduleHndle);
#else	/* DOT11K_SUPPORT */
WFM_STATUS_CODE UMAC_Init(UMAC_HANDLE UmacHandle,
			  void *llHandle,
			  UMAC_2UL_CB UlCb,
			  UMAC_2UL_RX_FRM_CB UpperLayerRxCb,
			  UMAC_2UL_TX_CNF_CB UpperLayerTxcnf,
			  UMAC_2UL_TX_MGMT_CNF_CB UpperLayerTxMgmtcnf,
			  UMAC_2UL_SCHEDULE_CB ScheduleCb,
			  void *ScheduleHndle);
#endif	/* DOT11K_SUPPORT */

/******************************************************************************
 * NAME:	UMAC_Start
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for starting the UMAC instance which is
 * already created. UmacHandle should be created with UMAC_Create() before
 * calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
WFM_STATUS_CODE UMAC_Start(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_Stop
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for Stopping a UMAC Instance.UmacHandle should
 * be created with UMAC_Create() and started using UMAC_Start()before calling
 * this  function. UmacHandle should be created with UMAC_Create() before
 * calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
WFM_STATUS_CODE UMAC_Stop(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_Distroy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for destroying a UMAC Instance.UmacHandle
 * should be created with UMAC_Create() before calling this  fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
WFM_STATUS_CODE UMAC_Distroy(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_CleanAllTimers
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for clearing all the timers.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns void
 *****************************************************************************/
void UMAC_CleanAllTimers(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_ReleaseMemory
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Function to be called to release memory recieved from UMAC. UmacHandle
 * should be created with UMAC_Create() before calling this  fucntion.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param pMsg       - Pointer to the buffer to be released.
 * \returns void       none
 *****************************************************************************/
void UMAC_ReleaseMemory(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *pMsg);


#define UMAC_RUN_TASK(UmacHandle)  wfm_umac_task(UmacHandle)

#define UMAC_IF_GET_PARAM_REQ(UmacHandle, pHiMsg)	\
	WFM_HI_Proc_GetParamReq(UmacHandle, pHiMsg)

#define UMAC_IF_SET_PARAM_REQ(UmacHandle, pHiMsg)	\
	WFM_HI_Proc_SetParamReq(UmacHandle, pHiMsg)

#define UMAC_IF_TX_REQ(UmacHandle, pTxReq)       \
	WFM_HI_Proc_TransmitReq(UmacHandle, pTxReq)

#define UMAC_IF_CONFIG_REQ(UmacHandle, pHiMsg)   \
	WFM_HI_Proc_ConfigReq(UmacHandle, pHiMsg)

#define UMAC_MEMORY_READ_REQ(UmacHandle, pHiMsg)	 \
	WFM_HI_Proc_MemoryReadReq(UmacHandle, pHiMsg)

#define UMAC_MEMORY_WRITE_REQ(UmacHandle, pHiMsg)	\
	WFM_HI_Proc_MemoryWriteReq(UmacHandle, pHiMsg)

#define UMAC_IF_MGMT_TX_REQ(UmacHandle, pMgmtTxReq)       \
	WFM_HI_Proc_TransmitMgmtReq(UmacHandle, pMgmtTxReq)

#endif	/* _UMAC_IF_H */
