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
 * \file umac_dbg.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_dbg.c
 * \brief
 * This file implements UMAC Debug functionalities.
 * \ingroup Upper_MAC_Core
 * \date 31/07/08 11:34
 */

/******************************************************************************
				INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "umac_dev_conf.h"
#include "umac_dbg.h"
#include "umac_sm.h"

/******************************************************************************
			    EXTERNAL DATA REFERENCES
*****************************************************************************/

#ifdef UMAC_SM_DBG_ENABLE
char *UMAC_StateEventTable_StateName[UMAC_MAX_STATE] = {
	"UMAC_UNINITIALIZED",	/* 0 */
	"UMAC_INITIALIZED",	/* 1 */
	"UMAC_JOINING",		/* 2 */
	"UMAC_JOINED",		/* 3 */
	"UMAC_UNJOINING",	/* 4 */
	"UMAC_AUTHENTICATING",	/* 5 */
	"UMAC_AUTHENTICATED",	/* 6 */
	"UMAC_DE_AUTHENTICATING",	/* 7 */
	"UMAC_ASSOCIATING",	/* 8 */
	"UMAC_ASSOCIATED",	/* 9 */
	"UMAC_DISASSOCIATING",	/* 10 */
	"UMAC_REASSOCIATING"	/* 11 */
};

char *UMAC_StateEventTable_EventName[UMAC_MAX_EVT + 1] = {
	"UMAC_INIT	    ",	/*  0 */
	"UMAC_DEINIT	  ",	/*  1 */
	"UMAC_START_SCAN      ",	/*  2 */
	"UMAC_STOP_SCAN       ",	/*  3 */
	"UMAC_PROC_SCAN_RSP   ",	/*  4 */
	"UMAC_START_JOIN      ",	/*  5 */
	"UMAC_JOIN_SUCCESS    ",	/*  6 */
	"UMAC_JOIN_FAILED     ",	/*  7 */
	"UMAC_START_AUTH      ",	/*  8 */
	"UMAC_AUTH_SUCCESS    ",	/*  9 */
	"UMAC_AUTH_FAILED     ",	/* 10 */
	"UMAC_DE_AUTHE	",	/* 11 */
	"UMAC_DE_AUTH_SUCCESS ",	/* 12 */
	"UMAC_START_ASSOC     ",	/* 13 */
	"UMAC_ASSOC_SUCCESS   ",	/* 14 */
	"UMAC_ASSOC_FAILED    ",	/* 15 */
	"UMAC_DATA_TX	 ",	/* 16 */
	"UMAC_DATA_RX	 ",	/* 17 */
	"UMAC_IN_BEACON       ",	/* 18 */
	"UMAC_DE_ASSOC	",	/* 19 */
	"UMAC_DE_ASSOC_SUCCESS",	/* 20 */
	"UMAC_DE_ASSOC_REQ    ",	/* 21 */
	"UMAC_DE_AUTH_REQ     ",	/* 22 */
	"UMAC_UNJOIN	  ",	/* 23 */
	"UMAC_UNJOIN_SUCCESS  ",	/* 24 */
	"UMAC_SCAN_COMPLETED  ",	/* 25 */
	"UMAC_TX_COMPLETED    ",	/* 26 */
	"UMAC_START_REASSOC   ",	/* 27 */
	"UMAC_REASSOC_SUCCESS ",	/* 28 */
	"UMAC_REASSOC_FAILED  ",	/* 29 */
	"UMAC_MAX_EVENT--ERROR"	/* 30 */
};
#endif				/*UMAC_SM_DBG_ENABLE */

#ifdef UMAC_DBG_STAMP_TX_BUFFS

uint8 *umac_mem_users_name[UMAC_MEM_USR_MAX] = {
	"UMAC_SCAN_REQ......",
	"UMAC_JOIN_REQ......",
	"UMAC_AUTH_REQ......",
	"UMAC_DEAUTH_REQ....",
	"UMAC_ASSO_REQ......",
	"UMAC_REASSOC_REQ...",
	"UMAC_DEASSOC_REQ...",
	"DIL_CONFIG_REQ.....",
	"DIL_SCAN_REQ.......",
	"DIL_STOPSCAN_REQ...",
	"DIL_JOIN_REQ.......",
	"DIL_RESETJOIN_REQ..",
	"DIL_ADDKEY_REQ.....",
	"DIL_REMOVEKEY_REQ..",
	"DIL_WRITEMIB_REQ...",
	"DIL_READMIB_REQ....",
	"DIL_EDCAPARAMS_REQ.",
	"DIL_QTXPRAMS_REQ...",
	"DIL_BSSPARAMS_REQ..",
	"DIL_POWMGMTMODE_REQ"
};

#endif				/*UMAC_DBG_STAMP_TX_BUFFS */

/******************************************************************************
			      FUNCTION DEFINITIONS
******************************************************************************/

#ifdef CHECK_UMAC_TXDESC

/*********************************************************************************
 * NAME:	wfm_umac_dbg_check_for_outstanding_tx_desc
 *-------------------------------------------------------------------------------*/
/**
 * \brief
 * This function checks the TxDesc array kept by UMAC. If there's any txDesc to
 * be freed, this function will ASSERT.
 * \param *pInstance   - UmacInstans pointer to be debugged.
 * \returns none.
 *********************************************************************************/
void wfm_umac_dbg_check_for_outstanding_tx_desc(void *pInstance)
{
	uint32 umactxdeccnt = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pInstance;

	for (umactxdeccnt = 0; umactxdeccnt < WFM_UMAC_TX_ALLOC_MAX_NUM; umactxdeccnt++) {
		if (pUmacInstance->gVars.UmacTxDescBuff[umactxdeccnt] != 0x00) {
			LOG_EVENT(DBG_WFM_UMAC, "wfm_umac_dbg_check_for_outstanding_tx_desc\n");
			OS_ASSERT(0);

		}		/* if( pUmacInstance->gVars.UmacTxDescBuff[umactxdeccnt]!= 0x00 ) */

	}			/*for(umactxdeccnt=0;umactxdeccnt<WFM_UMAC_TX_ALLOC_MAX_NUM; */umactxdeccnt++)

}				/*wfm_umac_dbg_check_for_outstanding_tx_desc() */

#endif				/*CHECK_UMAC_TXDESC */

#if DBG_ENABLE_UMC_LOG_EXT

#define DBG_MAX_BUFF_SIZE 100

void umac_dbg_display_bytes(uint8 *Str, uint8 *bytes, uint32 Length, uint32 NumBytesToDisplay)
{

	uint16 i;

	LOG_EVENT(DBG_WFM_ALWAYS, Str);	/*Display Label */
	for (i = 0; i < Length; i++) {
		LOG_DATA(DBG_WFM_ALWAYS, "0x%x  ", bytes[i]);
#if 0
		if ((i % 16) == 0)
			LOG_EVENT(DBG_WFM_ALWAYS, "\n");
#endif				/*0 */
		if (i == NumBytesToDisplay) {
			/*break from here */
			i = (uint16) Length;
			continue;
		}
	}			/*  for(i=0; i<Length; */ i++)

}				/*end umac_dbg_display_bytes() */

void umac_dbg_display_string(uint8 *Str, uint8 *bytes, uint32 Length)
{

	uint8 Buff[DBG_MAX_BUFF_SIZE];
	uint16 strLen = (uint16) strlen(Str);

	if (strLen >= DBG_MAX_BUFF_SIZE)
		strLen = DBG_MAX_BUFF_SIZE - 2;

	if ((Length + strLen) > (DBG_MAX_BUFF_SIZE - 2))
		Length = (DBG_MAX_BUFF_SIZE - 2) - strLen;

	OS_MemoryCopy((void *)&Buff[0], Str, strLen);
	OS_MemoryCopy((void *)&Buff[strLen], bytes, Length);
	Buff[Length + strLen] = '\n';
	Buff[Length + strLen + 1] = '\0';

	LOG_EVENT(DBG_WFM_ALWAYS, Buff);

}				/*end umac_dbg_display_string() */

#endif				/*DBG_ENABLE_UMC_LOG_EXT */
