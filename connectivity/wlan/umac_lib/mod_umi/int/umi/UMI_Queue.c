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
 * \file UMI_Queue.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_Queue.c
 * \brief
 * This file implements a simple queue.
 * \ingroup Upper_MAC_Interface
 * \date 24/01/09 09:18
 */

/*****************************************************************************/
/********		       Include Files			      ********/
/*****************************************************************************/
#include "UMI_Queue.h"
#include "UMI_DebugTrace.h"
#include "UMI_Api.h"
#include "UMI_ScanList.h"

/******************************************************************************
 * NAME:	UMI_QueueInit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the queue and returns a pointer to it.
 * \returns UMI_Q     Pointer to the queue.
 *****************************************************************************/
UMI_Q *UMI_QueueInit()
{
	UMI_Q *pQueue = NULL;

	UMI_TRACE(UMI_Queue, ("---> UMI_QueueInit()\n"));

	pQueue = UMI_Allocate(sizeof(UMI_Q));

	if (pQueue) {
		pQueue->Head = NULL;
		pQueue->Tail = NULL;
		pQueue->NumElements = 0;
	}	/* if (pQueue) */
	UMI_TRACE(UMI_Queue, ("<--- UMI_QueueInit()\n"));

	return pQueue;
}	/* End UMI_QueueInit() */

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
UMI_Q_STATUS UMI_QueueInsert(UMI_Q *pQueue, void *pPkt)
{
	UMI_Q_ITEM *pCurrentTail = NULL;
	UMI_Q_ITEM *pNewElement = NULL;

	UMI_TRACE(UMI_Queue, ("---> UMI_QueueInsert()\n"));

	if (pQueue == NULL) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueInsert: Unexpected NULL queue\n"));
		return UMI_Q_FAILURE;
	}

	if (pPkt == NULL) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueInsert: Unexpected NULL pkt\n"));
		return UMI_Q_FAILURE;
	}

	pNewElement = (UMI_Q_ITEM *) UMI_Allocate(sizeof(UMI_Q_ITEM));

	if (!pNewElement)
		return UMI_Q_FAILURE;

	pNewElement->Pkt = pPkt;
	pNewElement->Next = NULL;

	pCurrentTail = (UMI_Q_ITEM *) pQueue->Tail;

	if (pCurrentTail) {
		/* Queue is not empty */
		pCurrentTail->Next = pNewElement;
		pNewElement->back = pCurrentTail;
		pQueue->Tail = pNewElement;

	} else {
		/* Its the first element of this queue */
		pQueue->Tail = pNewElement;
		pQueue->Head = pQueue->Tail;
		pNewElement->back = NULL;

	}

	/* updating the element count */
	pQueue->NumElements++;

	UMI_TRACE(UMI_Queue, ("<--- UMI_QueueInsert()\n"));

	return UMI_Q_SUCCESS;
}	/* End WFM_QueueInsert() */

/******************************************************************************
 * NAME:	UMI_QueueGetSize
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns the number of elements in the queue.
 * \param pQueue - The queue on which the operation needs to be done.
 * \returns uint32     Number of elements in the queue.
 *****************************************************************************/
uint32 UMI_QueueGetSize(UMI_Q *pQueue)
{
	UMI_TRACE(UMI_Queue, ("---> UMI_QueueGetSize()\n"));
	if (pQueue == NULL) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueGetSize: Unexpected NULL queue\n"));
		return 0;
	}

	UMI_TRACE(UMI_Queue, ("<--- UMI_QueueGetSize()\n"));

	return pQueue->NumElements;

}	/* End UMI_QueueGetSize() */

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
void *UMI_QueueReturnElementAtIndex(UMI_Q *pQueue, uint32 index)
{
	uint32 count = 0;
	UMI_Q_ITEM *pCurrentElement = NULL;

	UMI_TRACE(UMI_Queue, ("---> UMI_QueueReturnElementAtIndex()\n"));
	if (pQueue == NULL) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueReturnElementAtIndex: Unexpected NULL queue\n"));
		return NULL;
	}

	if (index >= pQueue->NumElements) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueReturnElementAtIndex: Invalid Index (%d)\n", index));
		return NULL;
	}

	/*
	   Now traverse the list. The element at the head is considered
	   to have index zero.
	 */
	pCurrentElement = (UMI_Q_ITEM *) pQueue->Head;

	while ((count < index) && (pCurrentElement != NULL)) {
		pCurrentElement = pCurrentElement->Next;
		count++;
	}	/* while ((count < index) && (pCurrentElement != NULL)) */

	UMI_TRACE(UMI_Queue, ("<--- UMI_QueueReturnElementAtIndex()\n"));
	return (pCurrentElement == NULL)
	    ? NULL : (void *)pCurrentElement->Pkt;

}	/* End WFM_QueueReturnElementAtIndex() */

/******************************************************************************
 * NAME:	UMI_QueueReturnNextElement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API returns and removes the element at the head.
 * \param   pQueue - The queue on which the operation needs to be done.
 * \returns void*    A pointer to the next element.
 *****************************************************************************/
void *UMI_QueueReturnNextElement(UMI_Q *pQueue)
{

	UMI_Q_ITEM *pElement = NULL;
	void *pPkt = NULL;

	UMI_TRACE(UMI_Queue, ("---> UMI_QueueReturnNextElement()\n"));

	if (pQueue == NULL) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueReturnNextElement: Unexpected NULL queue\n"));
		return NULL;
	}

	pElement = (UMI_Q_ITEM *) pQueue->Head;

	if (!pElement)
		return NULL;

	/* Packet to return. */
	pPkt = pElement->Pkt;

	/* Updating the Head */
	pQueue->Head = pElement->Next;

	if (!pQueue->Head) {
		/*
		   It was the last element in the queue, so assigning Tail to
		   NULL
		 */
		pQueue->Tail = pQueue->Head;
	} else
		pQueue->Head->back = NULL;

	UMI_Free(pElement);

	/* Decrement the element count */
	pQueue->NumElements--;

	UMI_TRACE(UMI_Queue, ("<--- UMI_QueueReturnNextElement()\n"));

	return pPkt;
}	/* End UMI_QueueReturnNextElement() */

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
UMI_Q_STATUS UMI_QueueRemoveElement(UMI_Q *pQueue, void *pPkt)
{
	UMI_Q_ITEM *pElement = NULL;
	UMI_Q_ITEM *pPrevElement = NULL;
	UMI_Q_ITEM *pNextElement = NULL;

	UMI_TRACE(UMI_Queue, ("---> UMI_QueueRemoveElement()\n"));

	if (pQueue == NULL) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueRemoveElement: Unexpected NULL queue\n"));
		return UMI_Q_FAILURE;
	}

	if (pPkt == NULL) {
		UMI_ERROR(UMI_Queue, ("WFM_QueueRemoveElement: Unexpected NULL packet\n"));
		return UMI_Q_FAILURE;
	}

	/*
	   1] If first element, update the head
	   2] If last element, update the tail
	   3] Free the memory allocated for queue element container.
	 */

	pElement = (UMI_Q_ITEM *) pQueue->Head;

	while (pElement) {
		/*
		   Just comparing the pointers would suffice
		   as queue is not alocating memory for the stored elements.
		 */

		if (pElement->Pkt == pPkt)
			break;

		pPrevElement = pElement;
		pElement = (UMI_Q_ITEM *) pElement->Next;

	}	/* while (pElement) */

	if (!pElement)
		return UMI_Q_FAILURE;

	/*
	   We have identified the Queue element to be removed here.
	   Now adjust the pointers properly and free the memory allocated
	   for this queue element.
	 */

	pNextElement = pElement->Next;

	if (!pPrevElement) {
		/* The first element is the one to be removed */
		pQueue->Head = pNextElement;
		if (pQueue->Head)
			pQueue->Head->back = NULL;
		else
			pQueue->Tail = pQueue->Head;

	} else {
		pPrevElement->Next = pNextElement;

		if (pNextElement)
			pNextElement->back = pPrevElement;
		else
			pQueue->Tail = pPrevElement;
	}

	UMI_Free((void *)pElement);

	/* Decrement the element count */
	pQueue->NumElements--;

	UMI_TRACE(UMI_Queue, ("<--- UMI_QueueRemoveElement()\n"));

	return UMI_Q_SUCCESS;
}	/* End WFM_QueueRetriveElement() */

/******************************************************************************
 * NAME:	UMI_QueueFlush
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function cleans up the queue.
 * \param pQueue - The queue on which the operation needs to be done.
 * \returns UMI_Q_STATUS
 *****************************************************************************/
UMI_Q_STATUS UMI_QueueFlush(UMI_Q *pQueue)
{
	uint32 numOfElements = 0;
	void *pPkt = NULL;

	UMI_TRACE(UMI_Queue, ("---> UMI_QueueFlush()\n"));

	if (pQueue == NULL) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueFlush: Unexpected NULL queue\n"));
		return UMI_Q_FAILURE;
	}

	numOfElements = UMI_QueueGetSize(pQueue);
	while (numOfElements) {
		pPkt = UMI_QueueReturnNextElement(pQueue);
		UMI_Free(pPkt);
		numOfElements--;
	}
	pQueue->NumElements = numOfElements;
	UMI_TRACE(UMI_Queue, ("<--- UMI_QueueFlush()\n"));

	return UMI_Q_SUCCESS;

}	/* End WFM_QueueFlush() */

/******************************************************************************
 * NAME:	UMI_QueueDinit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function De-initializes the queue.
 * \param pQueue - The queue on which the operation needs to be done.
 * \returns UMI_Q_STATUS
 *****************************************************************************/
UMI_Q_STATUS UMI_QueueDinit(UMI_Q *pQueue)
{
	UMI_TRACE(UMI_Queue, ("---> UMI_QueueDinit()\n"));

	if (pQueue == NULL) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueDinit: Unexpected NULL queue\n"));
		return UMI_Q_FAILURE;
	}

	UMI_Free(pQueue);

	UMI_TRACE(UMI_Queue, ("<--- UMI_QueueDinit()\n"));

	return UMI_Q_SUCCESS;
}	/* End UMI_QueueDinit() */

/******************************************************************************
 * NAME:	UMI_QueueInsertElementAtIndex
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API inserts the element at the given index
 * \param   pQueue - The queue on which the operation needs to be done.
 * \param   pElem - Pointer to the element which need to be inserted
 * \param   index - Index at which element to be inserted
 * \returns UMI_Q_STATUS
 *****************************************************************************/
UMI_Q_STATUS UMI_QueueInsertElementAtIndex(UMI_Q *pQueue,
					   void *pElem,
					   uint32 index)
{
	uint32 count = 0;
	UMI_Q_ITEM *pCurrentElement = NULL;

	UMI_TRACE(UMI_Queue, ("---> UMI_QueueInsertElementAtIndex()\n"));
	if (NULL == pQueue) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueInsertElementAtIndex: Unexpected NULL queue\n"));
		return UMI_Q_FAILURE;
	}

	if (NULL == pElem) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueInsertElementAtIndex: Unexpected NULL element\n"));
		return UMI_Q_FAILURE;
	}

	if (index >= pQueue->NumElements) {
		UMI_ERROR(UMI_Queue, ("UMI_QueueInsertElementAtIndex: Invalid Index (%d)\n", index));
		return UMI_Q_FAILURE;
	}

	/*
	   Now traverse the list. The element at the head is considered
	   to have index zero.
	 */
	pCurrentElement = (UMI_Q_ITEM *) pQueue->Head;

	while ((count < index) && (pCurrentElement != NULL)) {
		pCurrentElement = pCurrentElement->Next;
		count++;
	}	/* while ((count < index) && (pCurrentElement != NULL)) */

	UMI_OS_ASSERT(pCurrentElement != NULL);
	/* Insert the element */
	UMI_Free(pCurrentElement->Pkt);
	pCurrentElement->Pkt = pElem;

	UMI_TRACE(UMI_Queue, ("<--- UMI_QueueInsertElementAtIndex()\n"));
	return UMI_Q_SUCCESS;

}	/* End UMI_QueueInsertElementAtIndex() */

/******************************************************************************
 * NAME:	UMI_CopyQueue
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function copies the contents of pQueueSrc to pQueueDest
 * \param  pQueueDest  - Pointer to destination queue.
 * \param  pQueueSrc   - Pointer to source queue.
 * \returns NULL.
 *****************************************************************************/
void UMI_CopyQueue(UMI_Q *pQueueDest, UMI_Q *pQueueSrc)
{
	uint32 count;
	UMI_Q_ITEM *pCurrentSrcElement = NULL;
	UMI_Q_ITEM *pCurrentDestTail = NULL;
	UMI_Q_ITEM *pNewElement = NULL;

	UMI_TRACE(UMI_Queue, ("---> UMI_ClearScanList()\n"));
	if (NULL == pQueueDest)
		UMI_OS_ASSERT(0);
	if (NULL == pQueueSrc)
		UMI_OS_ASSERT(0);
	pCurrentSrcElement = (UMI_Q_ITEM *) pQueueSrc->Head;
	pCurrentDestTail = (UMI_Q_ITEM *) pQueueDest->Tail;

	for (count = 0; count < pQueueSrc->NumElements; count++) {
		pNewElement = (UMI_Q_ITEM *) UMI_Allocate(sizeof(UMI_Q_ITEM));
		if (!pNewElement)
			UMI_ERROR(UMI_Queue, ("UMI_ClearScanList: Unexpected NULL element\n"));
		pNewElement->Pkt = pCurrentSrcElement->Pkt;
		pNewElement->Next = NULL;
		pCurrentSrcElement = pCurrentSrcElement->Next;
		if (pQueueDest->Tail) {
			/* Dest Queue is not empty */
			pCurrentDestTail->Next = pNewElement;
			pNewElement->back = pCurrentDestTail;
			pQueueDest->Tail = pNewElement;
			pCurrentDestTail = pCurrentDestTail->Next;

		} else {
			/* Its the first element of this queue */
			pQueueDest->Tail = pNewElement;
			pQueueDest->Head = pNewElement;
			pCurrentDestTail = pNewElement;
			pNewElement->back = NULL;

		}
	}	/*for(count = 0; count < pQueueSrc->NumElements; count++)*/
	pQueueDest->NumElements = count;
}

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
void UMI_MergeQueues(UMI_Q *pOldQueue, UMI_Q *pNewQueue)
{
	uint32 NumElementsNewQ = 0;
	uint32 NumElementsOldQ = 0;
	uint32 indexNewQueue;
	uint32 index;
	UMI_BSS_CACHE_INFO_IND *pBssCacheInfoInd = NULL;
	if (NULL == pOldQueue)
		UMI_OS_ASSERT(0);
	if (NULL == pNewQueue)
		UMI_OS_ASSERT(0);
	NumElementsNewQ = pNewQueue->NumElements;
	NumElementsOldQ = pOldQueue->NumElements;

	for (indexNewQueue = 0; indexNewQueue < NumElementsNewQ; indexNewQueue++) {
		pBssCacheInfoInd = UMI_QueueReturnElementAtIndex(pNewQueue, indexNewQueue);
		if (NULL == pBssCacheInfoInd)
			UMI_OS_ASSERT(0);
		index = UMI_GetMacAddrIndex(pOldQueue, pBssCacheInfoInd->bssId);
		if (index >= NumElementsOldQ) {
			/* The beacon is not found in old list, so insert the
			newly arrived beacon */
			UMI_QueueInsert(pOldQueue, pBssCacheInfoInd);
		}
	}
}
