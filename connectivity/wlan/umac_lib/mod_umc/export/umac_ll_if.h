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
 * \file umac_ll_if.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_ll_if.h
 * \brief
 * This header file defines the interface between core UMAC
 * and Lower layers.
 * \ingroup Upper_MAC_Core
 * \date 22/01/09 11:43
 */

#ifndef _UMAC_LL_IF_H
#define _UMAC_LL_IF_H

/******************************************************************************
				INCLUDE FILES
******************************************************************************/
#include "lmac_if.h"
#include "umac_if.h"
#include "UMI_UmacDil.h"
#include "umac_mem.h"

/******************************************************************************
		    DATA STRUCTURES AND PREPROCESSORS
******************************************************************************/

typedef struct UMAC_TX_DESC_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 PacketId;
	uint8 MaxTxRate;
	uint8 QueueId;
	uint8 More;
	uint8 Flags;
	uint32 Reserved;
	uint32 ExpireTime;
	uint32 HtTxParameters;
} UMAC_TX_DESC;

typedef struct UMAC_RX_DESC_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
	uint16 ChannelNumber;
	uint8 RxedRate;
	uint8 Rcpi;
	uint32 Flags;
} UMAC_RX_DESC;

typedef struct UMAC_TX_CNF_DESC_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 PacketId;
	uint32 Status;
	uint8 TxedRate;
	uint8 AckFailures;
	uint16 Flags;
	uint32 MediaDelay;
	uint32 TxQueueDelay;
} UMAC_TX_CNF_DESC;

typedef struct UMAC_MULTI_TX_CNF_DESC_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 NumTxConfs;
	uint32 TxConfPayload;	/* Variable length */
} UMAC_MULTI_TX_CNF_DESC;

typedef struct UMAC_DEVIF_TX_DATA_S {
	uint32 HeaderLength;	/*Length of the info. in pTxDescriptor */
	UMAC_TX_DESC *pTxDescriptor;	/*Start of the packet to be written to
					  the device HI+WSM+Dot11 HDR */
	uint8 *pDot11Frame;	/*Remaining Dot11 Header- 14 bytes */
	uint8 *UlInfo;		/*Information from upper layer */
} UMAC_DEVIF_TX_DATA;

#define TX_DESC_GET_DOT11FRAME(pTxDesc)	 \
	(((uint8 *)pTxDesc)+sizeof(UMAC_TX_DESC))

#define RX_DESC_GET_DOT11FRAME(pRxDesc)	 \
	(((uint8 *)pRxDesc)+sizeof(UMAC_RX_DESC))

#define ASSIGN_FRAME_TO_TX_DESC(pTxDesc, pTxFrame)		       \
	{								   \
	uint8   *ptr  = (uint8 *)pTxDesc + sizeof(UMAC_TX_DESC);	\
	ptr   = (uint8 *)pTxFrame  ;				    \
	}

#define ASSIGN_FRAME_LENGTH(pTxDesc, Len)		\
	(pTxDesc->MsgLen = ((Len)+(sizeof(UMAC_TX_DESC))))

#define GET_TX_DESC_FRAME_LENGTH(pTxDesc)   (pTxDesc->MsgLen)

#define GET_RX_DESC_FRAME_LENGTH(pRxDesc)       \
	(pRxDesc->MsgLen - sizeof(UMAC_RX_DESC))

#define	ASSIGN_TX_PTA_PRIORITY(_pTxDesc, _Priority)			\
	do {								\
		(_pTxDesc)->Flags |= (((_Priority) & 0x7) << 1);	\
	} while (0)

#define PTA_PRIORITY_DEFAULT		4 /* default pta priority */
#define PTA_PRIORITY_DATA		4 /* use for normal data */
#define PTA_PRIORITY_CONNECT		5 /* use for connect/disconnect */
#define PTA_PRIORITY_RECONNECT		5 /* use for roaming */
#define PTA_PRIORITY_MGT		5 /* use for normal mgt frame */
#define PTA_PRIORITY_VIDEO		5 /* use for AC_VI data */
#define PTA_PRIORITY_VOICE		6 /* use for AC_VO data */
#define PTA_PRIORITY_EAPOL		7 /* use for EAPOL exchange */

/* macros for assigning MaxTxRate to Tx packets */
#define ASSIGN_MGT_FRAME_TX_RATE(_pTxDesc, _pUmacInstance) \
	do {\
		(_pTxDesc)->MaxTxRate = (_pUmacInstance)->MgtTxRateIndex;\
	} while (0)


/******************************************************************************
				LMAC FUNCTIONS
******************************************************************************/
#define UMAC_LL_INIT(UmacHandle)		\
	DIL_Init(UmacHandle)

#define UMAC_LL_DEINIT(UmacHandle)	      \
	DIL_DeInit(UmacHandle)

#define UMAC_LL_REQ_DEV_CONF(UmacHandle, pConfData, hi_msg)       \
	DIL_ReqDevConf(UmacHandle, hi_msg)

#define UMAC_LL_INITIATE_SCAN(UmacHandle, pScanParams)   \
	DIL_InitiateScan(UmacHandle, pScanParams)

#define UMAC_LL_STOP_SCAN(UmacHandle)	   \
	DIL_StopScan(UmacHandle)

#define UMAC_LL_REQ_JOIN(UmacHandle, pJoinParams)	\
	DIL_ReqJoin(UmacHandle, pJoinParams)

#if DOT11K_SUPPORT
#define UMAC_LL_REQ_MEASUEREMENT(UmacHandle, pMeasurementReqParams)	\
	DIL_ReqMeasurement(UmacHandle, pMeasurementReqParams)
#endif	/* DOT11K_SUPPORT */

#define UMAC_LL_REQ_TX_FRAME(UmacHandle, priority, pDevIfTxReq, linkId)   \
	DIL_ReqTxFrame(UmacHandle, priority, pDevIfTxReq, linkId)

#define UMAC_LL_REQ_RESET_JOIN(UmacHandle, Flags, linkId)	\
	DIL_ReqResetJoin(UmacHandle, Flags, linkId)

#define UMAC_LL_REQ_ADD_KEY_ENTRY(UmacHandle, KeyData, linkId)   \
	DIL_ReqAddKeyEntry(UmacHandle, KeyData, linkId)

#define UMAC_LL_REQ_REMOVE_KEY_ENTRY(UmacHandle, KeyEntryIndex, linkId)  \
	DIL_ReqRemoveKeyEntry(UmacHandle, KeyEntryIndex, linkId)

#define UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams)    \
	DIL_ReqWriteMib(UmacHandle, pMibParams)

#define UMAC_LL_REQ_READ_MIB(UmacHandle, pMibParams)     \
	DIL_ReqReadMib(UmacHandle, pMibParams)

#define UMAC_LL_REQ_TX_EDCA_PARAMS(UmacHandle, pEdcaParams)      \
	DIL_ReqTxEdcaParams(UmacHandle, pEdcaParams)

#if P2P_MINIAP_SUPPORT
#define UMAC_LL_REQ_MAP_LINK(UmacHandle, pMapLink, linkId)      \
	DIL_ReqMapLink(UmacHandle, pMapLink, linkId)

#define UMAC_LL_REQ_START(UmacHandle, pStartReq)	\
	DIL_ReqStart(UmacHandle, pStartReq)

#define UMAC_LL_REQ_BCN_TX_REQ(UmacHandle, pBcnTxReq)	\
	DIL_ReqBcnTxReq(UmacHandle, pBcnTxReq)

#define UMAC_LL_REQ_START_FIND(UmacHandle)	      \
	DIL_ReqStartFind(UmacHandle)

#define UMAC_LL_REQ_STOP_FIND(UmacHandle)	      \
	DIL_ReqStopFind(UmacHandle)

#define UMAC_LL_REQ_UPDATE_IE(UmacHandle, pUpdateIE)    \
	DIL_ReqUpdateIE(UmacHandle, pUpdateIE)
#endif	/* P2P_MINIAP_SUPPORT */

#define UMAC_LL_REQ_SET_TEMPLATE_FRAME(UmacHandle, pLmacTemplateFrame,   \
				       pMibParams)		      \
	DIL_ReqWriteMib(UmacHandle, pMibParams)

#define UMAC_LL_REQ_SET_TX_QUEUE_PARAMS(UmacHandle, pQueueParams)	\
	DIL_ReqSetTxQueueParams(UmacHandle, pQueueParams)

#define UMAC_LL_REQ_SYN_BSS_PARAMS(UmacHandle, pBssParameters)   \
	DIL_ReqSynBssParams(UmacHandle, pBssParameters)

#define UMAC_LL_REQ_PS_POWER_MGMT_MODE(UmacHandle, pPsMode)     \
	DIL_ReqPSPowerMgmtMode(UmacHandle, pPsMode)

#define UMAC_LL_REQ_SWITCH_CHANNEL(UmacHandle, pSwitchChannel)   \
	DIL_ReqSwitchChannel(UmacHandle, pSwitchChannel)

#define UMAC_LL_REQ_SCHEDULE_TX(UmacHandle)   \
	DIL_ScheduleTx(UmacHandle)

/*****************************************************************************
			       [LMAC] MEMORY FUNCTIONS
******************************************************************************/

/*Find the position to start WSM HI TX REq*/
#define UMAC_LL_ALLOCATE_TX_DESC(UmacHandle, pData)      \
	((uint8 *)pData - sizeof(UMAC_TX_DESC))

#define UMAC_LL_RELEASE_TX_DESC(UmacHandle, pTxDesc)

#define UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle)	\
	umac_allocate_txbuf((void *)UmacHandle)

#define UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pTxDesc) \
	umac_release_txbuf(UmacHandle, pTxDesc)

#define UMAC_LL_RELEASE_RX_BUFFER_TYPE(UmacHandle, pRxFrame, Flags)       \
	DIL_RxComplete(UmacHandle, pRxFrame, NULL)

#define UMAC_LL_RELEASE_RXBUFF(UmacHandle, RxBuffDesc)   \
	DIL_RxComplete(UmacHandle, RxBuffDesc, NULL)

#define UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle, RxBuffDesc, pDriverInfo)	\
	DIL_RxComplete(UmacHandle, RxBuffDesc, pDriverInfo)
#define UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor)

#define UMAC_RELEASE_TX_CNF_FRAME(UmacHandle, TxDescCnf) \
	DIL_RxComplete(UmacHandle, TxDescCnf, NULL)


/******************************************************************************
			  PLATFORM [HI] FUNCTIONS
******************************************************************************/
#define UMAC_LL_READ_MEMORY(UmacHandle, pHiMsg)  \
	DIL_ReqReadMemory(UmacHandle, pHiMsg)

#define UMAC_LL_WRITE_MEMORY(UmacHandle, pHiMsg) \
	DIL_ReqWriteMemory(UmacHandle, pHiMsg)

#endif	/* _UMAC_LL_IF_H */
