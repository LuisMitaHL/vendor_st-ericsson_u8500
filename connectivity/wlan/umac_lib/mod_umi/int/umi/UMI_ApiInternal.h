/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup Upper_MAC_Interface
 * \brief
 * This layer is the direct interface to the network driver and it converts the
 * UMI API calls made by the network driver to UMAC understandable data
 * structures. Similarly this layer converts the UMAC data structures to UMI
 * data structures. This layer receives the messages from UMC in HI message
 * format and converts those HI messages into network driver function calls.
 *
 */
/**
 * \file UMI_ApiInternal.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_ApiInternal.h
 * \brief
 *  Header for UMI module.
 * \ingroup Upper_MAC_Interface
 * \date 29/12/08 15:08
 */

#ifndef _UMI_API_INTERNAL_H
#define _UMI_API_INTERNAL_H

#include "UMI_OsIf.h"
#include "UMI_ScanList.h"

#define UMI_P2P_ENABLED		1
#define UMI_P2P_DISABLE		0

typedef struct UMI_CONNECTION_S UMI_CONNECTION;
typedef void *UMC_HANDLE;

/*---------------------------------------------------------------------------*
 *			External Data Structure				     *
 *---------------------------------------------------------------------------*/
typedef enum UMI_STATE_E {
	UMI_STATE_INITIALIZED = 0,
	UMI_STATE_STARTING,
	UMI_STATE_STARTED,
	UMI_STATE_DISCONNECTED,
	UMI_STATE_CONNECTED,
	UMI_STATE_START_FAILED,
	UMI_STATE_STOP_PENDING,
	UMI_STATE_STOPPED,
	UMI_STATE_SUSPENDED,
	UMI_STATE_SURPRISE_REMOVE,
	UMI_STATE_REMOVE_PENDING,
	UMI_STATE_REMOVED
} UMI_STATE;

/*This structure stores the state for a particular connection*/
struct UMI_CONNECTION_S {
	UMI_CREATE_IN inParams;
	uint32 mtu;
	LL_HANDLE llHandle;	/* Handle for WDEV Module */
	UMC_HANDLE umacHandle;	/* Handle for UMAC Module */
	UMI_STATE devUmiState;	/* Current  State of WFM Module */
	UMI_STATE preUmiState;	/* Previous State of WFM Module */
	UMI_Q *pScanListQueue;	/* Scan List Queue */
	UMI_Q *pScanListOldQueue;	/* Old Scan List Queue */
	UMI_CONNECTION *nextConn;	/* Pointer to the next element */
	uint32 infraMode;
	uint32 encryptionStatus;
	uint32 UseP2P;
};

#define INITIALIZE_UMI_STATE(_Data_)		    \
	do {					\
		(_Data_)->devUmiState = UMI_STATE_INITIALIZED  ;    \
		(_Data_)->preUmiState = UMI_STATE_INITIALIZED  ;    \
	} while (0)

#define IS_UMI_STATE(_Data_, _State_)	 \
	((_Data_)->devUmiState == (_State_))

#define SET_NEW_UMI_STATE(_Data_, _State_)	\
	do {						\
		(_Data_)->preUmiState = (_Data_)->devUmiState  ;    \
		(_Data_)->devUmiState = _State_		;           \
	} while (0)

#define RESTORE_PREVIOUS_UMI_STATE(_Data_)	      \
	(_Data_)->devUmiState = (_Data_)->preUmiState  ;

#endif	/* _UMI_API_INTERNAL_H */
