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
 * \file umac_2_ul.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_2_ul.c
 * \brief
 * This file implements the functions necessary to send information to the
 * upper layer.
 * \ingroup Upper_MAC_Core
 * \date 24/02/08 18:44
 */

/******************************************************************************
	      INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "umac_messages.h"
#include "umac_2_ul.h"
#include "umac_dev_conf.h"

#include "umac_mem.h"

#include "umac_dbg.h"
#include "umac_sm.h"

#include "umac_if.h"

/******************************************************************************
			  EXTERNAL FUNCTIONS
******************************************************************************/

void WFM_UMAC_2_UL_MessageBuffAvailable(UMAC_HANDLE UmacHandle);

/******************************************************************************
			  LOCAL DATA
******************************************************************************/

static void WFM_UMAC_2_UL_QueueEvents(UMAC_HANDLE UmacHandle, uint16 Event);

/******************************************************************************
 * NAME:	WFM_UMAC_2_UL_SendEvent
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates a WFM event for the host and uploads it to the
 * host.
 * \param UmacHandle      - Handle to the UMAC Instance
 * \param Event		  - The event to be uplaoded to the host
 * \param EventDataLength - Length of Data/info associated with this event
 * \param *EventData      - Information associated with this event.
 * \param IsRetry	  - Zero if its is retry,1  otherwise.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_2_UL_SendEvent(UMAC_HANDLE UmacHandle,
			     uint16 Event,
			     uint16 EventDataLength,
			     uint8 *EventData,
			     uint8 IsRetry)
{
	WFM_HI_MESSAGE *pHi_Msg = NULL;
	WFM_EVENT_IND *pEventInd = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 TotalLen = EventDataLength + WFM_HI_MESSAGE_HDR_SIZE + sizeof(WFM_EVENT_IND);

	OS_ASSERT(pUmacInstance);

	if (EventDataLength > 4) {
		/*
		  A default buffer of 4 bytes has already been allocated
		  for event data.
		*/
		TotalLen = TotalLen - 4;
	}

	pHi_Msg = (WFM_HI_MESSAGE *) UMAC_GET_IND_BUF(UmacHandle, TotalLen);

	if (pHi_Msg) {

		pHi_Msg->MessageId = WFM_EVENT_IND_ID;
		pHi_Msg->MessageLength = TotalLen;

		pEventInd = (WFM_EVENT_IND *) &pHi_Msg->PayLoad.wfm_event_ind;
		pEventInd->EventId = Event;
		pEventInd->EventDataLength = EventDataLength;

		if (EventDataLength) {

			if (EventData) {
				OS_MemoryCopy(
						(void *)pEventInd->EventData,
						EventData,
						EventDataLength
						);
			}

		} /*if( EventDataLength ) */

		if (!(pUmacInstance->gVars.p.UpperLayerCb(
					 pUmacInstance->gVars.p.ulHandle,
					 pHi_Msg))
		    ) {

			/* No buffer available, so queue it now */
			WFM_UMAC_2_UL_QueueEvents(UmacHandle, Event);

			/*
			   Hi Send failed, so no pointing in keeping this buffer
			   So release it.
			 */
			UMAC_ReleaseMemory(
						UmacHandle,
						(WFM_HI_MESSAGE *) pHi_Msg
						);
		}
	} /*if ( pHi_Msg ) */
	else {
		/*
		   If this is a retry and no buffer is available,
		   If IsRetry == 0 here] something is wrong  so Assert.
		 */
		OS_ASSERT(IsRetry);

		/* No buffer available, so queue it now */
		WFM_UMAC_2_UL_QueueEvents(UmacHandle, Event);
	}

	return;
} /* end WFM_UMAC_2_UL_SendEvent() */

/******************************************************************************
 * NAME:	WFM_UMAC_2_UL_SendDevInfo
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates a WFM event for the host and uploads it to the
 * host.
 * \param UmacHandle    - Handle to the UMAC Instance
 * \param *pMessage		- Pointer to the message to be uploaded to host
 * \param Messagetype	- Type of the message
 * \param PayloadLength - Length Of Hi Message Payload.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_2_UL_SendDevInfo(UMAC_HANDLE UmacHandle,
			       uint8 *pMessage,
			       uint16 Messagetype,
			       uint16 PayloadLength)
{
	WFM_HI_MESSAGE *pMsg = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	if (pMessage) {

		/*Finding space for Hi header */
		pMsg = (WFM_HI_MESSAGE *) ((uint8 *) pMessage - WFM_HI_MESSAGE_HDR_SIZE);
		pMsg->MessageId = Messagetype;
		pMsg->MessageLength = PayloadLength + WFM_HI_MESSAGE_HDR_SIZE;

		/* Send Receive Packet Indication to HOST */
		if (!(pUmacInstance->gVars.p.UpperLayerCb(
					 pUmacInstance->gVars.p.ulHandle,
					 pMsg))
		    ) {
			/*
			   If pUmacInstance->gVars.p.UpperLayerCb fails,
			   just droping the beacon and freeing the indication
			   buffer.
			 */
			UMAC_ReleaseMemory(UmacHandle, (WFM_HI_MESSAGE *) pMsg);

		}
	} /*if ( pMessage ) */
} /* end WFM_UMAC_2_UL_SendDevInfo() */

/******************************************************************************
 * NAME:	WFM_UMAC_2_UL_MessageBuffAvailable
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the callback fucntion which is called by Hi module when an indication
 * buffer is freed.
 * \param UmacHandle - Handle to the UMAC Instance
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_2_UL_MessageBuffAvailable(UMAC_HANDLE UmacHandle)
{
	uint8 i = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	if (pUmacInstance->gVars.event_queue.NumPendingEvents) {
		for (i = 0; i < pUmacInstance->gVars.event_queue.NumPendingEvents; i++) {
			if (pUmacInstance->gVars.event_queue.Evt[i] != WFM_EVT_MAX) {
				/*
				   Since a buffer is available now, so the
				   following function should be successful.
				 */
				LOG_EVENT(DBG_WFM_UMAC, "WFM_UMAC_2_UL_MessageBuffAvailable() *****\n");
				WFM_UMAC_2_UL_SendEvent(UmacHandle, pUmacInstance->gVars.event_queue.Evt[i], 0, NULL, WFM_UMAC_EVT_RETRANSMIT);

				pUmacInstance->gVars.event_queue.Evt[i] = WFM_EVT_MAX;

				if (pUmacInstance->gVars.event_queue.NumPendingEvents)
					pUmacInstance->gVars.event_queue.NumPendingEvents--;

				break;
			}
		}
	} /* if ( pUmacInstance->gVars.event_queue.NumPendingEvents ) */

} /* end WFM_UMAC_2_UL_MessageBuffAvailable() */

/******************************************************************************
 * NAME:	WFM_UMAC_2_UL_QueueEvents
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function queues UMAC events to the host.
 * \param UmacHandle - Handle to the UMAC Instance
 * \param Event		 - Event to be sent to HOST
 * \returns none.
 *****************************************************************************/
static void WFM_UMAC_2_UL_QueueEvents(UMAC_HANDLE UmacHandle, uint16 Event)
{
	uint16 i = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	if (pUmacInstance->gVars.event_queue.NumPendingEvents <= WFM_UMAC_MAX_SIZE_EVT_QUEUE) {
		for (i = 0; i < WFM_UMAC_MAX_SIZE_EVT_QUEUE; i++) {
			if (pUmacInstance->gVars.event_queue.Evt[i] == WFM_EVT_MAX) {
				pUmacInstance->gVars.event_queue.Evt[i] = Event;
				pUmacInstance->gVars.event_queue.NumPendingEvents++;
				break;
			}
		} /* for( i = 0; i < WFM_UMAC_MAX_SIZE_EVT_QUEUE;  i++ ) */
	} else
		LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ERROR, "ERROR? Event Queue Over Flow\n");

} /*end WFM_UMAC_2_UL_QueueEvents() */

/******************************************************************************
 * NAME:	WFM_UMAC_2_UL_Evt_Q_Init
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the UMAC event queue
 * \param UmacHandle - Handle to the UMAC Instance
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_2_UL_Evt_Q_Init(UMAC_HANDLE UmacHandle)
{
	uint8 Count = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	/*Initializing the Pending event queue */
	for (Count = 0; Count < WFM_UMAC_MAX_SIZE_EVT_QUEUE; Count++)
		pUmacInstance->gVars.event_queue.Evt[Count] = WFM_EVT_MAX;

} /* end WFM_UMAC_2_UL_Evt_Q_Init() */

