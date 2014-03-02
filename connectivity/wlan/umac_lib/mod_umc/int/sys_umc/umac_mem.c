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
 * \file umac_mem.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_mem.c
 * \brief
 * This file contains all the memory management APIs for UMAC.
 * \ingroup Upper_MAC_Core
 * \date 13/02/08 14:05
 */

/******************************************************************************
			      INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "umac_mem.h"
#include "umac_sm.h"
#include "umac_globals.h"
#include "umac_if.h"
#include "sysconfig.h"

/******************************************************************************
			      EXTERNAL REFERENCES
******************************************************************************/

extern void WFM_UMAC_2_UL_MessageBuffAvailable(UMAC_HANDLE UmacHandle);

/******************************************************************************
			      EXTERNAL DATA
******************************************************************************/

/******************************************************************************
			      INTERNAL FUNCTIONS
******************************************************************************/

/******************************************************************************
			      INTERNAL DATA
******************************************************************************/

#define WFM_MAX_FRAME_SIZE      2048
#define WFM_MAX_IND_BUFF_SIZE   1024

typedef struct UMAC_MEM_S {

	uint32 TxBuff[WFM_MAX_NUM_TX_BUFFS][CONVERT_BYTE_SIZE_TO_WORD_SIZE(WFM_MAX_FRAME_SIZE)];

	uint32 IndBuf[WFM_MAX_NUM_IND_BUFFS][CONVERT_BYTE_SIZE_TO_WORD_SIZE(WFM_MAX_IND_BUFF_SIZE)];

} UMAC_MEM;

/******************************************************************************
			      EXTERNAL FUNCTIONS
******************************************************************************/

/******************************************************************************
 * NAME:	umac_mem_init
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the umac memory management module.Apart from
 * initialization, this function allocates and reserves internal buffers
 * required for umac operation.
 * \param pTxBuffPool    - Pointer to UMAC_TX_BUFF_POOL structure.
 * \param pIndBuffPool   - Pointer to UMAC_IND_BUFF_POOL structure.
 * \returns none.
 *****************************************************************************/
#ifdef ALLOC_SMALL_MEM_CHUNK
void umac_mem_init(UMAC_TX_BUFF_POOL *pTxBuffPool,
		   UMAC_IND_BUFF_POOL *pIndBuffPool)
{
	int i;
	uint8 *pMem;

	OS_ASSERT(pTxBuffPool);
	OS_ASSERT(pIndBuffPool);

	OS_MemoryReset(pTxBuffPool, sizeof(*pTxBuffPool));

	/* Allocate internal Tx buffers */
	for (i = 0; i < WFM_MAX_NUM_TX_BUFFS; i++) {
		pMem = OS_Allocate(WFM_MAX_FRAME_SIZE);
		if (pMem == NULL) {
			/* cannot allocate memory: handle error */
			goto release_mem;
		} else {
			pTxBuffPool->pMem[i] = pMem;
			pTxBuffPool->pMemList[i] = pMem;
		}
	}
	pTxBuffPool->Get = 0;
	pTxBuffPool->Put = WFM_MAX_NUM_TX_BUFFS;
	pTxBuffPool->Lock = OS_CREATE_LOCK();

	OS_MemoryReset(pIndBuffPool, sizeof(*pIndBuffPool));
	/* Allocate indication buffers */
	for (i = 0; i < WFM_MAX_NUM_IND_BUFFS; i++) {
		pMem = OS_Allocate(WFM_MAX_IND_BUFF_SIZE);
		if (pMem == NULL) {
			/* cannot allocate memory: handle error */
			goto release_mem;
		} else {
			pIndBuffPool->pMem[i] = pMem;
			pIndBuffPool->pMemList[i] = pMem;
		}
	}
	pIndBuffPool->Get = 0;
	pIndBuffPool->Put = WFM_MAX_NUM_IND_BUFFS;
	pIndBuffPool->Lock = OS_CREATE_LOCK();

	return;

release_mem:
	LOG_EVENT(DBG_WFM_ERROR, "Failed to Allocate resources\n");
	umac_mem_deInit(pTxBuffPool, pIndBuffPool);
	return;

} /*end umac_mem_init() */
#else
void umac_mem_init(UMAC_TX_BUFF_POOL *pTxBuffPool,
		   UMAC_IND_BUFF_POOL *pIndBuffPool)
{
	int i;
	UMAC_MEM *pUmacMem;

	OS_ASSERT(pTxBuffPool);
	OS_ASSERT(pIndBuffPool);

	pUmacMem = (UMAC_MEM *) OS_Allocate(sizeof(UMAC_MEM));

	if (NULL == pUmacMem) {
		LOG_EVENT(DBG_WFM_ERROR, "Failed to Allocate resources\n");
		OS_ASSERT(0);
	}

	/*
	   Storing the base pointer, this will help us to release this
	   memory later
	 */
	pTxBuffPool->pBaseMem = (uint8 *) pUmacMem;

	OS_MemoryReset(pUmacMem, sizeof(UMAC_MEM));

	pTxBuffPool->Get = 0;

	for (i = 0; i < WFM_MAX_NUM_TX_BUFFS; i++)
		pTxBuffPool->pMem[i] = (void *)pUmacMem->TxBuff[i];

	pTxBuffPool->Put = i;

	pTxBuffPool->Lock = OS_CREATE_LOCK();

	pIndBuffPool->Get = 0;

	for (i = 0; i < WFM_MAX_NUM_IND_BUFFS; i++)
		pIndBuffPool->pMem[i] = (void *)pUmacMem->IndBuf[i];

	pIndBuffPool->Put = i;
	pIndBuffPool->Lock = OS_CREATE_LOCK();

} /*end umac_mem_init() */
#endif

/******************************************************************************
 * NAME:	umac_mem_deInit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function deinitializes the umac memory management module.Apart from
 * deinitialization, this function releases the internal buffers reserved for
 * umac operation.
 * \param pTxBuffPool    - Pointer to UMAC_TX_BUFF_POOL structure.
 * \param pIndBuffPool   - Pointer to UMAC_IND_BUFF_POOL structure.
 * \returns none.
 *****************************************************************************/
#ifdef ALLOC_SMALL_MEM_CHUNK
void umac_mem_deInit(UMAC_TX_BUFF_POOL *pTxBuffPool,
		     UMAC_IND_BUFF_POOL *pIndBuffPool)
{
	int i;

	/* Free allocated internal tx buffers */
	for (i = 0; i < WFM_MAX_NUM_TX_BUFFS; i++) {
		if (pTxBuffPool->pMemList[i])
			OS_Free(pTxBuffPool->pMemList[i]);
	}

	if (pTxBuffPool->Lock)
		OS_DESTROY_LOCK(pTxBuffPool->Lock);

	OS_MemoryReset(pTxBuffPool, sizeof(*pTxBuffPool));

	/* Free allocated indication buffers */
	for (i = 0; i < WFM_MAX_NUM_IND_BUFFS; i++) {
		if (pIndBuffPool->pMemList[i])
			OS_Free(pIndBuffPool->pMemList[i]);
	}

	if (pIndBuffPool->Lock)
		OS_DESTROY_LOCK(pIndBuffPool->Lock);

	OS_MemoryReset(pIndBuffPool, sizeof(*pIndBuffPool));

	return;

} /*end umac_mem_deInit() */
#else
void umac_mem_deInit(UMAC_TX_BUFF_POOL *pTxBuffPool,
		     UMAC_IND_BUFF_POOL *pIndBuffPool)
{
	UMAC_MEM *pUmacMem;

	OS_ASSERT(pTxBuffPool);
	OS_ASSERT(pIndBuffPool);

	pUmacMem = (UMAC_MEM *) pTxBuffPool->pBaseMem;

	OS_ASSERT(pUmacMem);

	OS_DESTROY_LOCK(pTxBuffPool->Lock);
	OS_DESTROY_LOCK(pIndBuffPool->Lock);

	OS_Free(pUmacMem);

} /*end umac_mem_deInit() */
#endif

/******************************************************************************
 * NAME:	umac_allocate_txbuf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function allocates an internal tx buffer.
 * \param *UmacHandle - Pointer to UMAC handle.
 * \returns none.
 *****************************************************************************/
void *umac_allocate_txbuf(void *UmacHandle)
{
	void *pBuff;
	uint32 Get;
	uint8 Index;
	UMAC_TX_BUFF_POOL *pTxBuffPool;
	WFM_UMAC_INSTANCE *pUmacInstance;

	OS_ASSERT(UmacHandle);

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	pTxBuffPool = &pUmacInstance->gVars.p.TxBuffPool;

	OS_ASSERT(pTxBuffPool);

	OS_LOCK(pTxBuffPool->Lock);

	Get = pTxBuffPool->Get;

	if ((uint32) (pTxBuffPool->Put - Get) > 0) {
		Index = (uint8) (Get & (WFM_MAX_NUM_TX_BUFFS - 1));
		pBuff = pTxBuffPool->pMem[Index];

		OS_ASSERT(pBuff);

		pTxBuffPool->pMem[Index] = NULL;
		Get++;
		pTxBuffPool->Get = Get;
	} else
		pBuff = NULL;

	OS_UNLOCK(pTxBuffPool->Lock);

	return pBuff;

} /*end umac_allocate_txbuf() */

/******************************************************************************
 * NAME:	umac_release_txbuf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function releases an allocated tx buffer.
 * \param *UmacHandle - Pointer to UMAC Handle.
 * \param pBuff      - Pointer to the buffer to be released.
 * \returns uint32    1 on success 0 on failure
 *****************************************************************************/
uint32 umac_release_txbuf(void *UmacHandle, void *pBuff)
{
	uint32 Put;
	uint32 Index;
	UMAC_TX_BUFF_POOL *pTxBuffPool;
	WFM_UMAC_INSTANCE *pUmacInstance;
	uint32 status;

	OS_ASSERT(pBuff);

	OS_ASSERT(UmacHandle);

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	pTxBuffPool = &pUmacInstance->gVars.p.TxBuffPool;

	OS_ASSERT(pTxBuffPool);

	OS_LOCK(pTxBuffPool->Lock);
	Put = pTxBuffPool->Put;

	OS_ASSERT((Put - pTxBuffPool->Get) <= WFM_MAX_NUM_TX_BUFFS);

	Index = (uint32) (Put & (WFM_MAX_NUM_TX_BUFFS - 1));

	if (pTxBuffPool->pMem[Index] != NULL) {
		status = 0;
		goto exit_handler;
	} /*if(pTxBuffPool->pMem[Index]!= NULL) */

	pTxBuffPool->pMem[Index] = pBuff;

	Put++;

	pTxBuffPool->Put = Put;

	status = 1;

 exit_handler:
	OS_UNLOCK(pTxBuffPool->Lock);
	return status;

} /*end umac_release_txbuf() */

/******************************************************************************
 * NAME:	umac_allocate_indbuf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function releases an allocated tx buffer.
 * \param *UmacHandle - Pointer to UMAC Handle.
 * \param Size       - Size of buffer to be allocated.
 * \returns void*      Pointer to the buffer allocated, NULL if no buffer is
 *                     available.
 *****************************************************************************/
void *umac_allocate_indbuf(void *UmacHandle, unsigned Size)
{
	void *pBuff;
	uint32 Get;
	uint8 Index;
	UMAC_IND_BUFF_POOL *pIndBuffPool;
	WFM_UMAC_INSTANCE *pUmacInstance;

	OS_ASSERT(Size <= WFM_MAX_IND_BUFF_SIZE);

	OS_ASSERT(UmacHandle);

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	pIndBuffPool = &pUmacInstance->gVars.p.IndBuffPool;

	OS_ASSERT(pIndBuffPool);

	OS_LOCK(pIndBuffPool->Lock);

	Get = pIndBuffPool->Get;

	if ((uint32) (pIndBuffPool->Put - Get) > 0) {
		Index = (uint8) (Get & (WFM_MAX_NUM_IND_BUFFS - 1));
		pBuff = pIndBuffPool->pMem[Index];

		OS_ASSERT(pBuff);

		pIndBuffPool->pMem[Index] = NULL;
		Get++;
		pIndBuffPool->Get = Get;
	} else
		pBuff = NULL;

	OS_UNLOCK(pIndBuffPool->Lock);

	return pBuff;

} /*end umac_allocate_indbuf() */

/******************************************************************************
 * NAME:	umac_release_indbuf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function releases an allocated indication buffer.
 * \param *UmacHandle - Pointer to UMAC Handle.
 * \param  pBuff      - Pointer to the buffer to be released.
 * \returns uint32      1 on success 0 on failure
 *****************************************************************************/
uint32 umac_release_indbuf(void *UmacHandle, void *pBuff)
{
	uint32 Put;
	uint32 Index;
	UMAC_IND_BUFF_POOL *pIndBuffPool;
	WFM_UMAC_INSTANCE *pUmacInstance;
	uint32 status;

	OS_ASSERT(pBuff);

	OS_ASSERT(UmacHandle);

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	pIndBuffPool = &pUmacInstance->gVars.p.IndBuffPool;

	OS_ASSERT(pIndBuffPool);

	OS_LOCK(pIndBuffPool->Lock);

	Put = pIndBuffPool->Put;

	OS_ASSERT((Put - pIndBuffPool->Get) <= WFM_MAX_NUM_IND_BUFFS);

	Index = (uint32) (Put & (WFM_MAX_NUM_IND_BUFFS - 1));

	if (pIndBuffPool->pMem[Index] != NULL) {
		status = 0;
		goto exit_handler;
	} /*if(pIndBuffPool->pMem[Index]!= NULL) */

	pIndBuffPool->pMem[Index] = pBuff;

	Put++;

	pIndBuffPool->Put = Put;

	status = 1;

 exit_handler:
	OS_UNLOCK(pIndBuffPool->Lock);
	return status;
} /*end umac_release_indbuf() */

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
void UMAC_ReleaseMemory(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *pMsg)
{
	HI_MSG_HDR *pHiMsg;
	WSM_HI_RX_IND *pWsmHiRxInd;
	WFM_UMAC_INSTANCE *pUmacInstance;
	uint32 status;

	OS_ASSERT(pMsg);

	OS_ASSERT(UmacHandle);

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	pHiMsg = (HI_MSG_HDR *) pMsg;

	/*   LOG_DATA(DBG_WFM_HI,"HI: %08X\n",(uint32)pMsg); */

	if ((pHiMsg->MsgId & 0x0C3F) == WSM_HI_RX_IND_ID) {
		pWsmHiRxInd = (WSM_HI_RX_IND *) pHiMsg;

		UMAC_LL_RELEASE_RX_BUFFER_TYPE(
				UmacHandle,
				(uint8 *) pMsg,
				(pWsmHiRxInd->Flags & WSM_RI_FLAGS_DEFRAG)
				);

		return;
	}

	/* Check for trace buffer */
	if ((pHiMsg->MsgId & 0x0FFF) == HI_TRACE_IND_ID)
		OS_ASSERT(0);

	/*
	   If control reaches here, its an indication message, release it
	*/
	status = umac_release_indbuf(UmacHandle, pMsg);

	OS_ASSERT(status);

	/*
	   If there are pending UMAC events to be transmitted to the Host,
	   send it now, as buffers are available now.
	*/

	WFM_UMAC_2_UL_MessageBuffAvailable(UmacHandle);

} /*end UMAC_ReleaseMemory() */
