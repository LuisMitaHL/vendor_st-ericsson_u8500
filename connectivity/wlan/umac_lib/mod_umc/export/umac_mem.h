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
 * \file umac_mem.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_mem.h
 * \brief
 * Memory management module for UMAC.
 * \ingroup Upper_MAC_Core
 * \date 06/11/08 15:02
 */

#ifndef _UMAC_MEM_H
#define _UMAC_MEM_H

/******************************************************************************
			      INCLUDE FILES
******************************************************************************/

#include "stddefs.h"
#include "os_if.h"

#define WFM_MAX_NUM_TX_BUFFS     16
#define WFM_MAX_NUM_IND_BUFFS    4

#ifdef ALLOC_SMALL_MEM_CHUNK
typedef struct UMAC_TX_BUFF_POOL_S {
	uint32 Get;
	uint32 Put;
	void *pMem[WFM_MAX_NUM_TX_BUFFS];
	void *pMemList[WFM_MAX_NUM_TX_BUFFS];
	OS_LOCK_TYPE Lock;
} UMAC_TX_BUFF_POOL;

typedef struct UMAC_IND_BUFF_POOL_S {
	uint32 Get;
	uint32 Put;
	void *pMem[WFM_MAX_NUM_IND_BUFFS];
	void *pMemList[WFM_MAX_NUM_TX_BUFFS];
	OS_LOCK_TYPE Lock;
} UMAC_IND_BUFF_POOL;
#else
typedef struct UMAC_TX_BUFF_POOL_S {
	uint8 *pBaseMem;
	uint32 Get;
	uint32 Put;
	void *pMem[WFM_MAX_NUM_TX_BUFFS];
	OS_LOCK_TYPE Lock;
} UMAC_TX_BUFF_POOL;

typedef struct UMAC_IND_BUFF_POOL_S {
	uint32 Get;
	uint32 Put;
	void *pMem[WFM_MAX_NUM_IND_BUFFS];
	OS_LOCK_TYPE Lock;

} UMAC_IND_BUFF_POOL;
#endif
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
void umac_mem_init(UMAC_TX_BUFF_POOL *pTxBuffPool,
		   UMAC_IND_BUFF_POOL *pIndBuffPool);

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
void umac_mem_deInit(UMAC_TX_BUFF_POOL *pTxBuffPool,
		     UMAC_IND_BUFF_POOL *pIndBuffPool);

/******************************************************************************
 * NAME:	umac_allocate_txbuf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function allocates an internal tx buffer.
 * \param *UmacHandle - Pointer to UMAC handle.
 * \returns none.
 *****************************************************************************/
void *umac_allocate_txbuf(void *UmacHandle);

/******************************************************************************
 * NAME:	umac_release_txbuf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function releases an allocated tx buffer.
 * \param *UmacHandle - Pointer to UMAC Handle.
 * \param pBuff       - Pointer to the buffer to be released.
 * \returns uint32    1 on success 0 on failure
 *****************************************************************************/
uint32 umac_release_txbuf(void *UmacHandle, void *pBuff);

/******************************************************************************
 * NAME:	umac_allocate_indbuf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function releases an allocated tx buffer.
 * \param *UmacHandle - Pointer to UMAC Handle.
 * \param Size        - Size of buffer to be allocated.
 * \returns void*       Pointer to the buffer allocated, NULL if no buffer is
 *                      available.
 *****************************************************************************/
void *umac_allocate_indbuf(void *UmacHandle, unsigned Size);

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
uint32 umac_release_indbuf(void *UmacHandle, void *pBuff);

#define UMAC_GET_IND_BUF(UmacHandle, Size)	\
	umac_allocate_indbuf(UmacHandle, Size)

#endif	/* _UMAC_MEM_H */
