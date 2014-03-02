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
 * \file UMI_Queue.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_Queue.h
 * \brief
 * This file implements a simple queue.
 * \ingroup Upper_MAC_Interface
 * \date 24/01/09 12:14
 */

#ifndef _UMI_QUEUE_H
#define _UMI_QUEUE_H

#include "UMI_OsIf.h"

typedef enum UMI_Q_STATUS_E {
	UMI_Q_SUCCESS = 0x00,
	UMI_Q_FAILURE = 0x01
} UMI_Q_STATUS;

typedef struct UMI_Q_ITEM_S UMI_Q_ITEM;

struct UMI_Q_ITEM_S {
	void *Pkt;
	UMI_Q_ITEM *Next;
	UMI_Q_ITEM *back;
};

struct UMI_Q_S {
	uint32 NumElements;
	UMI_Q_ITEM *Head;
	UMI_Q_ITEM *Tail;
};

typedef struct UMI_Q_S UMI_Q;

/******************************************************************************
 * NAME:	UMI_QueueInit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the queue and returns a pointer to it.
 * \returns UMI_Q     Pointer to the queue.
 *****************************************************************************/
UMI_Q *UMI_QueueInit(void);

/******************************************************************************
 * NAME:	UMI_QueueInsert
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function inserts the given element in to the queue.
 * \param   pQueue - The queue on which the operation needs to be done.
 * \param   pPkt - The packet to be inserted in to the queue.
 * \returns UMI_Q_STATUS     Queue Status.
 *****************************************************************************/
UMI_Q_STATUS UMI_QueueInsert(UMI_Q *pQueue, void *pPkt);

/******************************************************************************
 * NAME:	UMI_QueueGetSize
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns the number of elements in the queue.
 * \param pQueue - The queue on which the operation needs to be done.
 * \returns uint32     Number of elements in the queue.
 *****************************************************************************/
uint32 UMI_QueueGetSize(UMI_Q *pQueue);

/******************************************************************************
 * NAME:	UMI_QueueReturnElementAtIndex
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns the number of elements in the queue.
 * \param   pQueue - The queue on which the operation needs to be done.
 * \param   index -  Index number from which elements to be returned.
 * \returns void*    Element present in the queue at the given index.
 *****************************************************************************/
void *UMI_QueueReturnElementAtIndex(UMI_Q *pQueue, uint32 index);

/******************************************************************************
 * NAME:	UMI_QueueReturnNextElement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns and removes the element at the head.
 * \param   pQueue - The queue on which the operation needs to be done.
 * \returns void*    A pointer to the next element.
 *****************************************************************************/
void *UMI_QueueReturnNextElement(UMI_Q *pQueue);

/******************************************************************************
 * NAME:	UMI_QueueRemoveElement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function removes the given element from the specified queue.
 * \param   pQueue - The queue on which the operation needs to be done.
 * \param   pPkt - The element to be removed.
 * \returns UMI_Q_STATUS
 *****************************************************************************/
UMI_Q_STATUS UMI_QueueRemoveElement(UMI_Q *pQueue, void *pPkt);

/******************************************************************************
 * NAME:	UMI_QueueFlush
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function cleans up the queue.
 * \param pQueue - The queue on which the operation needs to be done.
 * \returns UMI_Q_STATUS
 *****************************************************************************/
UMI_Q_STATUS UMI_QueueFlush(UMI_Q *pQueue);

/******************************************************************************
 * NAME:	UMI_QueueDinit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function De-initializes the queue.
 * \param pQueue - The queue on which the operation needs to be done.
 * \returns UMI_Q_STATUS
 *****************************************************************************/
UMI_Q_STATUS UMI_QueueDinit(UMI_Q *pQueue);

/******************************************************************************
 * NAME:	UMI_QueueReturnNextElement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns and removes the element at the head.
 * \param   pQueue - The queue on which the operation needs to be done.
 * \returns void*    A pointer to the next element.
 *****************************************************************************/
UMI_Q_STATUS UMI_QueueInsertElementAtIndex(UMI_Q *pQueue,
					   void *pElem,
					   uint32 index);

/******************************************************************************
 * NAME:	UMI_CopyQueue
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function copies the contents of pQueueSrc to pQueueDest
 * \param  pQueueDest  - Pointer to source queue.
 * \param  pQueueSrc   - Pointer to destination queue.
 * \returns NULL.
 *****************************************************************************/
void UMI_CopyQueue(UMI_Q *pQueueDest, UMI_Q *pQueueSrc);

/******************************************************************************
 * NAME:	UMI_MergeQueues
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function merges the contents of pNewQueue to pOldQueue
 * \param  pOldQueue  - Pointer to old queue.
 * \param  pNewQueue  - Pointer to new queue.
 * \returns NULL.
 *****************************************************************************/
void UMI_MergeQueues(UMI_Q *pOldQueue, UMI_Q *pNewQueue);

#endif	/*_WFM_QUEUE_H */
