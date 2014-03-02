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
 * \file umac_2_ul.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_2_ul.h
 * \brief
 * This file implements the functions necessary to send information to the
 * upper layer.
 * \ingroup Upper_MAC_Core
 * \date 24/02/08 18:44
 */

#ifndef _UMAC_2_UL_H
#define _UMAC_2_UL_H

/******************************************************************************
			    INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "umac_dev_conf.h"
#include "umac_if.h"

/******************************************************************************
				  CONSTANTS
*****************************************************************************/
#define  WFM_UMAC_EVT_RETRANSMIT	0
#define  WFM_UMAC_EVT_NOT_RETRANSMIT    1

/******************************************************************************
			LOCAL DECLARATIONS
******************************************************************************/
typedef struct UMAC_EVT_PENDING_S {
	uint32 NumPendingEvents;
	uint16 Evt[WFM_UMAC_MAX_SIZE_EVT_QUEUE];
} UMAC_EVT_PENDING;

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
 *********************************************************************************/
void WFM_UMAC_2_UL_SendEvent(UMAC_HANDLE UmacHandle,
			     uint16 Event,
			     uint16 EventDataLength,
			     uint8 *EventData,
			     uint8 IsRetry);

/******************************************************************************
 * NAME:	WFM_UMAC_2_UL_SendDevInfo
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates a WFM event for the host and uploads it to the
 * host.
 * \param UmacHandle    - Handle to the UMAC Instance
 * \param *pMessage	- Pointer to the message to be uploaded to host
 * \param Messagetype	- Type of the message
 * \param PayloadLength - Length Of Hi Message Payload.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_2_UL_SendDevInfo(UMAC_HANDLE UmacHandle,
			       uint8 *pMessage,
			       uint16 Messagetype,
			       uint16 PayloadLength);

/******************************************************************************
 * NAME:	WFM_UMAC_2_UL_Evt_Q_Init
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the UMAC event queue
 * \param UmacHandle - Handle to the UMAC Instance
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_2_UL_Evt_Q_Init(UMAC_HANDLE UmacHandle);

#endif	/* _UMAC_2_UL_H */
