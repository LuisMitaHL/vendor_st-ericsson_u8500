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
 * \file umac_task.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_task.h
 * \brief
 * The UMAC task, to process events to UMAC from HI and LMAC.
 * \ingroup Upper_MAC_Core
 * \date 24/06/08 11:07
 */

#ifndef _UMAC_TASK_H
#define _UMAC_TASK_H

/******************************************************************************
			    INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "umac_dbg.h"
#include "umac_if.h"

#define FEATURE_UMAC_QUEUE_PENDING_REQUESTS   1

#define WFM_UMAC_MAX_NO_OUTSTANDING_FRAMES     32

#define WFM_UMAC_HI_MESSAGE_PACKET_TYPE	0
#define WFM_UMAC_LMAC_MESSAGE_PACKET_TYPE      1
#define WFM_UMAC_SYSTEM_MESSAGE_PACKET_TYPE    2

typedef void (*WFM_UMAC_TASK_MSG_HANDLER) (UMAC_HANDLE UmacHandle, void *pMsg);

typedef struct WFM_UMAC_PKT_Q_S {
	uint16 Type;		/*HI or LMAC */
	uint16 Event;		/*Event to UMAC */
	void *pMsg;
} WFM_UMAC_PKT_Q;

typedef struct UMAC_TASK_CONTROL_BLOCK_S {
	WFM_UMAC_PKT_Q UmacPktQ[WFM_UMAC_MAX_NO_OUTSTANDING_FRAMES];
	uint32 UmacTaskQGet;
	uint32 UmacTaskQPut;
	OS_LOCK_TYPE Lock;
	WFM_UMAC_PKT_Q PendingPktQ[WFM_UMAC_MAX_NO_OUTSTANDING_FRAMES];
	uint32 PendingQGet;
	uint32 PendingQPut;
	OS_LOCK_TYPE PendingQLock;
} UMAC_TASK_CONTROL_BLOCK;

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
void wfm_umac_task(UMAC_HANDLE UmacHandle);


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
		    void *pMsg);

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
			void *pMsg);

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
void umac_move_pending_msgs_to_active_q(UMAC_HANDLE UmacHandle);

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
uint16 umac_find_idle_channel(UMAC_HANDLE UmacHandle);

/*
  Following Macro is defined to make the code more simple
*/
#define WFM_SCHEDULE_TO_SELF(UmacHandle, Event, pMsg)		     \
	wfm_umac_q_msg(UmacHandle, WFM_UMAC_SYSTEM_MESSAGE_PACKET_TYPE, Event, pMsg)

#define WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, Event, pMsg)		\
	wfm_umac_q_msg(UmacHandle, WFM_UMAC_LMAC_MESSAGE_PACKET_TYPE, Event, pMsg)

#define WFM_SCHEDULE_HI_MSG_TO_UMAC(UmacHandle, Event, pMsg)	      \
	wfm_umac_q_msg(UmacHandle, (uint32)WFM_UMAC_HI_MESSAGE_PACKET_TYPE,  \
	Event, (void *)pMsg)

#define UMAC_QUEUE_PENDING_HI_MSG(UmacHandle, Event, pMsg)		\
	umac_q_pending_msg(UmacHandle, (uint32)WFM_UMAC_HI_MESSAGE_PACKET_TYPE, \
	Event, (void *)pMsg)

#define UMAC_QUEUE_PENDING_LMAC_MSG(UmacHandle, Event, pMsg)	      \
	umac_q_pending_msg(UmacHandle, (uint32)WFM_UMAC_LMAC_MESSAGE_PACKET_TYPE, \
	Event, (void *)pMsg)

#define UMAC_QUEUE_PENDING_TO_SELF_MSG(UmacHandle, Event, pMsg)	   \
	umac_q_pending_msg(UmacHandle, (uint32)WFM_UMAC_SYSTEM_MESSAGE_PACKET_TYPE, \
	Event, (void *)pMsg)

#define UMAC_MOVE_PENDING_MSGS_TO_ACTIVE_Q(x)   \
	umac_move_pending_msgs_to_active_q(x)

#define UMAC_FIND_IDLE_CHANNEL(x)	\
	umac_find_idle_channel(x)

#endif	/* _UMAC_TASK_H */
