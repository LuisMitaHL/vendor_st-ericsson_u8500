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
 * \file umac_task.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_task.c
 * \brief
 * The UMAC task, to process events to UMAC from HI and LMAC.
 * \ingroup Upper_MAC_Core
 * \date 24/06/08 11:07
 */

/******************************************************************************
			    INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "hi_api.h"

#include "umac_task.h"
#include "umac_hi.h"
#include "umac_sm.h"

/******************************************************************************
			    LOCAL FUNCTIONS
******************************************************************************/
void wfm_umac_hi_msg_handler(UMAC_HANDLE UmacHandle, void *pMsg);
void wfm_umac_lmac_msg_handler(UMAC_HANDLE UmacHandle, void *pMsg);
void wfm_umac_system_msg_handler(UMAC_HANDLE UmacHandle, void *pMsg);

/******************************************************************************
			    LOCAL DATA
******************************************************************************/

/******************************************************************************
		      EXTERNAL DATA REFERENCES
******************************************************************************/

extern UMAC_FSM_HANDLER UMAC_StateEventTable[UMAC_MAX_STATE][UMAC_MAX_EVT];

/******************************************************************************
			    HANDLER TABLES
******************************************************************************/
const WFM_UMAC_HI_MSG_HANDLER WfmHiMsg_DispatchTable[] = {
	WFM_HI_Proc_MemoryReadReq,
	WFM_HI_Proc_MemoryWriteReq,
	WFM_HI_Proc_ConfigReq,
	WFM_HI_Proc_GenericReq,
	WFM_HI_Proc_TransmitReq,
	WFM_HI_Proc_GetParamReq,
	WFM_HI_Proc_SetParamReq,
	WFM_HI_DefaultMsgHandler
};

const WFM_UMAC_TASK_MSG_HANDLER WfmUmacMsgDispatchTbl[] = {
	wfm_umac_hi_msg_handler,
	wfm_umac_lmac_msg_handler,
	wfm_umac_system_msg_handler
};

/******************************************************************************
			    LOCAL DEFINITIONS
******************************************************************************/
#define IS_MORE_PENDING_MESSAGES(x)					\
	((uint8)(((WFM_UMAC_INSTANCE *)x)->gVars.p.tcb.PendingQPut -	\
	((WFM_UMAC_INSTANCE *)x)->gVars.p.tcb.PendingQGet) > 0)


/******************************************************************************
 * NAME:	wfm_umac_task
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * UMAC Task handler. This function will be triggered on the system event
 * SC_UMAC_TASK_MESSAGE
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \returns none.
 *****************************************************************************/
void wfm_umac_task(UMAC_HANDLE UmacHandle)
{
	uint8 index;
	WFM_UMAC_INSTANCE *pUmacInstance;
	WFM_UMAC_PKT_Q *pPktQ;
	uint32 Get;
	uint32 Put;

	OS_ASSERT(UmacHandle);

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

       OS_LOCK(pUmacInstance->gVars.p.tcb.Lock);
	Get = pUmacInstance->gVars.p.tcb.UmacTaskQGet;
	Put = pUmacInstance->gVars.p.tcb.UmacTaskQPut;

	if ((uint8) (Put - Get) > 0) {
		WFM_UMAC_PKT_Q pktQ = { 0 };

		index = (uint8) (Get & (WFM_UMAC_MAX_NO_OUTSTANDING_FRAMES - 1));

		pPktQ = &pUmacInstance->gVars.p.tcb.UmacPktQ[index];
		pktQ = *pPktQ;

		Get++;

		pUmacInstance->gVars.p.tcb.UmacTaskQGet = Get;
		pPktQ->pMsg = NULL;

		OS_UNLOCK(pUmacInstance->gVars.p.tcb.Lock);

		WfmUmacMsgDispatchTbl[pktQ.Type]
		(UmacHandle, &pktQ);

		if ((uint8) (Put - Get) != 0) {
			/*There are more packets to be processed */
			/*Request for Contact- Schedule for future */
			pUmacInstance->gVars.p.ScheduleCb(
				pUmacInstance->gVars.p.ScheduleParam
				);
		} /* if((uint8)(Put - Get) != 0) */
	} else {
	       OS_UNLOCK(pUmacInstance->gVars.p.tcb.Lock);
		LOG_EVENT(DBG_WFM_ERROR | DBG_WFM_STATE_MC, "BAD UMAC TASK SET EVENT*******\n");
	}

} /* end wfm_umac_task() */

/******************************************************************************
 * NAME:	wfm_umac_hi_msg_handler
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Handler to process Hi messages->UMAC.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \param *pMsg	      - Message to be processed.
 * \returns none.
 *****************************************************************************/
void wfm_umac_hi_msg_handler(UMAC_HANDLE UmacHandle, void *pMsg)
{
	WFM_UMAC_PKT_Q *pUmacPkt = (WFM_UMAC_PKT_Q *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 MsgId = NULL;

	if (!(pUmacPkt->pMsg)) {
		UMI_DbgPrint("wfm_umac_hi_msg_handler: message pointer is NULL\n");
		OS_ASSERT(pUmacPkt->pMsg);
		return;
        }
	MsgId = (((HI_MSG_HDR *) pUmacPkt->pMsg)->MsgId & UMAC_HI_MSG_ID);

	OS_ASSERT(pUmacInstance);

	if (MsgId >= (sizeof(WfmHiMsg_DispatchTable) / sizeof(WFM_UMAC_HI_MSG_HANDLER)))
		MsgId = (sizeof(WfmHiMsg_DispatchTable) / sizeof(WFM_UMAC_HI_MSG_HANDLER)) - 1;


	(*WfmHiMsg_DispatchTable[MsgId])
	    (UmacHandle, (WFM_HI_MESSAGE *) pUmacPkt->pMsg);

} /* end wfm_umac_hi_msg_handler() */

/******************************************************************************
 * NAME:	wfm_umac_lmac_msg_handler
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Handler to process LMAC->UMAC events/messages.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \param *pMsg	      - Message to be processed.
 * \returns none.
 *****************************************************************************/
void wfm_umac_lmac_msg_handler(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status;
	WFM_UMAC_PKT_Q *pUmacPkt = (WFM_UMAC_PKT_Q *) pMsg;

	CHECK_POINTER_IS_VALID(pMsg);
	OS_ASSERT(UmacHandle);

	if ((!(pUmacPkt->pMsg)) &&
		 (pUmacPkt->Event == UMAC_PROC_SCAN_RSP || pUmacPkt->Event == UMAC_IN_BEACON)) {
		  UMI_DbgPrint("wfm_umac_lmac_msg_handler: message pointer is NULL");
		  OS_ASSERT(pUmacPkt->pMsg);
		  return;
	}
	status = WFM_UMAC_EXEC_STATE_MACHINE(
					UmacHandle,
					pUmacPkt->Event,
					pUmacPkt->pMsg
					);

	if (WFM_STATUS_PENDING == status) {
		UMAC_QUEUE_PENDING_LMAC_MSG(
					UmacHandle,
					pUmacPkt->Event,
					pUmacPkt->pMsg
					);
	}

exit_handler:
	return;
} /* end wfm_umac_lmac_msg_handler() */

/******************************************************************************
 * NAME:	wfm_umac_system_msg_handler
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Handler to process events/messages from own context
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \param *pMsg		 - Message to be processed.
 * \returns none.
 *****************************************************************************/
void wfm_umac_system_msg_handler(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status;
	WFM_UMAC_PKT_Q *pUmacPkt = (WFM_UMAC_PKT_Q *) pMsg;

	CHECK_POINTER_IS_VALID(pMsg);

	OS_ASSERT(UmacHandle);

	status = WFM_UMAC_EXEC_STATE_MACHINE(
					UmacHandle,
					pUmacPkt->Event,
					pUmacPkt->pMsg
					);

	if (WFM_STATUS_PENDING == status)
		UMAC_QUEUE_PENDING_TO_SELF_MSG(
					UmacHandle,
					pUmacPkt->Event,
					pUmacPkt->pMsg
					);

} /* end wfm_umac_system_msg_handler() */

/******************************************************************************
 * NAME:	wfm_umac_q_msg
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Queues an Event or message to UMAC.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \param MessageType - Message to be processed.
 * \param EventType   - Event to be processed.
 * \param *pMsg	      - Message to be processed.
 * \returns none.
 *****************************************************************************/
void wfm_umac_q_msg(UMAC_HANDLE UmacHandle,
		    uint16 MessageType,
		    uint16 EventType,
		    void *pMsg)
{
	uint8 index;
	WFM_UMAC_INSTANCE *pUmacInstance;
	WFM_UMAC_PKT_Q *pPktQ;
	uint32 Put;

	OS_ASSERT(UmacHandle);

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_LOCK(pUmacInstance->gVars.p.tcb.Lock);

	Put = pUmacInstance->gVars.p.tcb.UmacTaskQPut;

	if ((int)(Put - pUmacInstance->gVars.p.tcb.UmacTaskQGet) < WFM_UMAC_MAX_NO_OUTSTANDING_FRAMES) {

		index = (uint8) (Put & (WFM_UMAC_MAX_NO_OUTSTANDING_FRAMES - 1));

		pPktQ = (WFM_UMAC_PKT_Q *) &pUmacInstance->gVars.p.tcb.UmacPktQ[index];

		OS_ASSERT(!(pPktQ->pMsg));

		Put++;

		pUmacInstance->gVars.p.tcb.UmacTaskQPut = Put;

		pPktQ->Type = MessageType;
		pPktQ->Event = EventType;
		pPktQ->pMsg = pMsg;

		OS_UNLOCK(pUmacInstance->gVars.p.tcb.Lock);

		pUmacInstance->gVars.p.ScheduleCb(
					pUmacInstance->gVars.p.ScheduleParam
					);
	} else {
		OS_UNLOCK(pUmacInstance->gVars.p.tcb.Lock);
		/*Task Queue over-flow */
		OS_ASSERT(0);
	}

} /* end wfm_umac_q_msg() */

/******************************************************************************
 * NAME:	umac_q_pending_msg
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Queues an Event or message to UMAC.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \param MessageType - Message to be processed.
 * \param EventType   - Event to be processed.
 * \param *pMsg	      - Message to be processed.
 * \returns none.
 *****************************************************************************/
void umac_q_pending_msg(UMAC_HANDLE UmacHandle,
			uint16 MessageType,
			uint16 EventType,
			void *pMsg)
{
	uint8 index;
	WFM_UMAC_INSTANCE *pUmacInstance;
	WFM_UMAC_PKT_Q *pPktQ;
	uint32 Put;

	OS_ASSERT(UmacHandle);

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_LOCK(pUmacInstance->gVars.p.tcb.PendingQLock);

	Put = pUmacInstance->gVars.p.tcb.PendingQPut;

	OS_ASSERT((int)(Put - pUmacInstance->gVars.p.tcb.PendingQGet) < WFM_UMAC_MAX_NO_OUTSTANDING_FRAMES);

	index = (uint8) (Put & (WFM_UMAC_MAX_NO_OUTSTANDING_FRAMES - 1));

	pPktQ = (WFM_UMAC_PKT_Q *) &pUmacInstance->gVars.p.tcb.PendingPktQ[index];

	OS_ASSERT(!(pPktQ->pMsg));

	Put++;

	pUmacInstance->gVars.p.tcb.PendingQPut = Put;

	pPktQ->Type = MessageType;
	pPktQ->Event = EventType;
	pPktQ->pMsg = pMsg;

	OS_UNLOCK(pUmacInstance->gVars.p.tcb.PendingQLock);

} /* end umac_q_pending_msg() */

/******************************************************************************
 * NAME:	umac_move_pending_msgs_to_active_q
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function moves the messages queued in the pending message queue to the
 * main task queue.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \returns none.
 *****************************************************************************/
void umac_move_pending_msgs_to_active_q(UMAC_HANDLE UmacHandle)
{
	uint8 index;
	WFM_UMAC_INSTANCE *pUmacInstance;
	WFM_UMAC_PKT_Q *pPktQ;

	OS_ASSERT(UmacHandle);

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	while (IS_MORE_PENDING_MESSAGES(UmacHandle)) {

		OS_LOCK(pUmacInstance->gVars.p.tcb.PendingQLock);

		index = (uint8) (pUmacInstance->gVars.p.tcb.PendingQGet & (WFM_UMAC_MAX_NO_OUTSTANDING_FRAMES - 1)
		    );

		pPktQ = (WFM_UMAC_PKT_Q *) &pUmacInstance->gVars.p.tcb.PendingPktQ[index];

		pUmacInstance->gVars.p.tcb.PendingQGet++;

		OS_UNLOCK(pUmacInstance->gVars.p.tcb.PendingQLock);

		wfm_umac_q_msg(
				UmacHandle,
				pPktQ->Type,
				pPktQ->Event,
				pPktQ->pMsg
				);

		pPktQ->Type = 0;
		pPktQ->Event = 0;
		pPktQ->pMsg = NULL;

	} /* while(IS_MORE_PENDING_MESSAGES(UmacHandle)) */

} /* end umac_move_pending_msgs_to_active_q */
/******************************************************************************
 * NAME:	umac_find_idle_channel
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function finds an ideal channel for MiniAP operation when Auto Channel
 * selection mode is enabled.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \returns channel number.
 *****************************************************************************/
uint16 umac_find_idle_channel(UMAC_HANDLE UmacHandle)
{
	uint8 index = 1;
	uint8 ideal = 0xFF;
	uint16 channel = 0;
	WFM_UMAC_INSTANCE *pUmacInstance;
	OS_ASSERT(UmacHandle);
	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	for (index = 1; index < 14; index += 5) {
		if (ideal > pUmacInstance->bss_count[index]) {
			ideal = pUmacInstance->bss_count[index];
			channel = index;
		}
	}
	return channel;
} /* end umac_find_idle_channel */

