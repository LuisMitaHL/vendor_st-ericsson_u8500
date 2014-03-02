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
 * \file umac_sm.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_sm.c
 * - <b>REVISION</b>	: 0.1
 * \brief
 * This file implements the UMAC State Machine.
 * \ingroup Upper_MAC_Core
 * \date 14/02/08 14:57
 * \author George Mathew [george.mathew@stericsson.com]
 * \note
 * Last person to change file : George Mathew
 */

/******************************************************************************
			    INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"
#include "umac_dev_conf.h"
#include "umac_mem.h"
#include "umac_sm.h"
#include "lmac_callbacks.h"
#include "umac_dev_list.h"
#include "umac_frames.h"
#include "umac_2_lmac.h"
#include "umac_2_ul.h"
#include "umac_messages.h"
#include "umac_utils.h"
#include "umac_data_handler.h"
#include "umac_task.h"
#include "umac_dbg.h"
#include "umac_mib.h"
#include "umac_internal.h"
#include "umac_ll_if.h"
#include "umac_if.h"

/******************************************************************************
			EXTERNALLY VISIBLE DATA
******************************************************************************/

/******************************************************************************
		      EXTERNAL DATA REFERENCES
*****************************************************************************/
#ifdef UMAC_SM_DBG_ENABLE
extern char *UMAC_StateEventTable_StateName[UMAC_MAX_STATE];
extern char *UMAC_StateEventTable_EventName[UMAC_MAX_EVT];
#endif				/*UMAC_SM_DBG_ENABLE */

/******************************************************************************
			  EXTERNAL FUNCTIONS
******************************************************************************/

/******************************************************************************
		LOCAL DECLARATIONS - SM HANDLERS
******************************************************************************/

static uint32 UMAC_ProcInit(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcDeInit(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartScan(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStopScan(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcScanRsp(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartJoin(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcJoinSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcJoinFailed(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartAuth(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcAuthSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcAuthFailed(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcDeAuthenticate(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcDeAuthSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartAssoc(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcAssocSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcAssocFailed(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcTxData(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcRxData(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcBeacon(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcDeAssoc(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcDeAssocSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcDeAssocReq(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcDeAuthReq(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcUnJoin(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcUnJoinSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcScanCompleted(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcTxCompleted(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcMgmtTxBuffer(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcRxDescBuff(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcReleaseRxBuff(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcException(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartReAssoc(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcReAssocSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcReAssocFailed(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_DefaultHostReq(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_QueueMessage(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartBGScan(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcReConnect(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcExtMIBReq(UMAC_HANDLE UmacHandle, void *pMsg);

/* FT/802.1r related state machine handler functions */
static uint32 UMAC_ProcStartFtAuth(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcFtAuthComplete(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcFtJoinComplete(UMAC_HANDLE UmacHandle, void *pMsg);

/* 802.11k/radio measurement related state machine handler function */
static uint32 UMAC_ProcRadioMeasurement(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcMsrmtReport(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcLinkMeasurement(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcSendMsrmtReq(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcNeighborReportReq(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcSendLinkMsrmtReq(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcSendLinkMsrmtRep(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcNeighborRepResp(UMAC_HANDLE UmacHandle, void *pMsg);

/* 802.11w/mgmt frame protection related state machine handler function */
static uint32 UMAC_ProcStartSaQuery(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcSaQuerySuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcSaQueryReq(UMAC_HANDLE UmacHandle, void *pMsg);

static uint32 UMAC_SendNullFrame(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_DlvrRequeueBufferedData(UMAC_HANDLE UmacHandle, void *pMsg);
#if P2P_MINIAP_SUPPORT
static uint32 UMAC_ProcFwdedFrmTxCompleted(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_DlvrLegacyBufferedData(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_DlvrWMMBufferedData(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcDeAuthSTA(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcUnLinkSTA(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartDeviceDiscovery(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartFind(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcFindCompleted(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStopFind(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartGoNego(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartGoNegoCnf(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartGoNegoResp(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcGoNegoReq(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcGoNegoResp(UMAC_HANDLE UmacHandle, void *pMsg);
#if 0
static uint32 UMAC_ProcGoNegoCnf(UMAC_HANDLE UmacHandle, void *pMsg);
#endif
static uint32 UMAC_ProcGoNegoSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcGoNegoFailed(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartInvtn(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcInvtnReq(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcInvtnResp(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcInvtnSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcInvtnFailed(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_AuthRespSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_AuthRespFailure(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_SendAuthResp(UMAC_HANDLE UmacHandle, uint16 status, uint8 linkId, void *pMsg);
/*static uint32 UMAC_ProcAuthReq     ( UMAC_HANDLE UmacHandle, void *pMsg ) ; */
static uint32 UMAC_ProcAssocReq(UMAC_HANDLE UmacHandle, void *pMsg);
#if 0
static uint32 UMAC_AssocRespSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_AssocRespFailure(UMAC_HANDLE UmacHandle, void *pMsg);
#endif
static uint32 UMAC_ResetSeqNumCounter(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartAP(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartAPSuccess(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcStartAPFailed(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcFwdFrame(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcUpdateTIMIE(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcDisconnectSTA(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_SendAssocResp(UMAC_HANDLE UmacHandle, uint16 status, uint8 linkId, uint8 SubType);
static uint32 UMAC_SendGoNegoResp(UMAC_HANDLE UmacHandle, uint16 status);
static uint32 UMAC_SendGoNegoCnf(UMAC_HANDLE UmacHandle, uint16 status);
static uint32 UMAC_SendInvtnResp(UMAC_HANDLE UmacHandle, uint16 status);
static uint32 UMAC_ProcStopAP(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_TxMgmtFrame(UMAC_HANDLE UmacHandle, void *pMsg);
static uint32 UMAC_ProcSchAsyncMgmt(UMAC_HANDLE UmacHandle, void *pMsg);
#if SEND_PROBE
static uint32 UMAC_ProcSendProbeRequest(UMAC_HANDLE UmacHandle, void *pMsg);
#endif
static uint32 UMAC_ProcUpdateERPIE(UMAC_HANDLE UmacHandle, void *pMsg);
#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
		  LOCAL STATIC FUNCTIONS
******************************************************************************/

static uint32 UMAC_ProcAssocSuccessWorker(UMAC_HANDLE UmacHandle, void *pMsg, uint16 Event);

static void UMAC_PerformReAssociation(UMAC_HANDLE UmacHandle);
/*static */
void UMAC_ManageRateSelection_TryAllRates(WFM_UMAC_INSTANCE *pUmacInstance, uint32 TxStatus, uint32 RateAtWhichLastTxed, uint32 NumRetries);
/*static */
void UMAC_ManageRateSelection(WFM_UMAC_INSTANCE *pUmacInstance, uint32 TxStatus, uint32 RateAtWhichLastTxed, uint32 NumRetries, uint8 linkId);
static void UMAC_CreateAc2DilQueueMap(UMAC_HANDLE UmacHandle, WFM_WMM_AC_PARAM_PACKED *pAcParam);

static void UMAC_RateAdapTimeOut_Cb(void *Handle);
static void UMAC_CheckForConnectionTimeOut_Cb(void *Handle);
static void UMAC_TimeOut_Cb(void *Handle);
static void UMAC_CheckIfUnJoinCanProceedTimeOut_Cb(void *Handle);
static void UMAC_BssLostTimeout_Cb(void *Handle);
#if 0
static void UMAC_CancelRemainOnChTimeOut_Cb(void *Handle);
#endif

#if DOT11K_SUPPORT
/*static void UMAC_FrameMeasTimeOut_Cb(void *Handle); */
static void UMAC_MeasPauseTimeOut_Cb(void *Handle);
static void UMAC_TxStreamMeasTimeOut_Cb(void *Handle);
#endif	/* DOT11K_SUPPORT */

#if MGMT_FRAME_PROTECTION
static void UMAC_SaQueryRetryTimeOut_Cb(void *Handle);
static void UMAC_ReAssocTimeOutInterval_Cb(void *Handle);
#endif	/* MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
static void UMAC_ClientConnectionTimeout_Cb(void *Handle);
static void UMAC_GroupFormationTimeout_Cb(void *Handle);
static void UMAC_P2P_InvitationTimeout_Cb(void *Handle);
static void UMAC_ConnectionDropTimeout_Cb(void *Handle);
static void UMAC_CleanupTimeout_Cb(void *Handle);
#endif	/* P2P_MINIAP_SUPPORT */


/******************************************************************************
			  STATIC DATA
******************************************************************************/

static const uint8 Channels2G[] = WFM_2_4GHZ_CHANNELS_SUPPORTED;

static const uint8 Channels5GCept[] = WFM_5GHZ_20MHZ_CEPT_CHANNELS_SUPPORTED;
static const uint8 Channels5GFcc[] = WFM_5GHZ_20MHZ_FCC_CHANNELS_SUPPORTED;

static const uint8 Channels5G_20M[] = WFM_5GHZ_20MHZ_CHANNELS_SUPPORTED;
static const uint8 Channels5G_10M[] = WFM_5GHZ_10MHZ_CHANNELS_SUPPORTED;

static const uint8 Channels4_9G_20M[] = WFM_4_9GHZ_20MHZ_CHANNELS_SUPPORTED;
static const uint8 Channels4_9G_10M[] = WFM_4_9GHZ_10MHZ_CHANNELS_SUPPORTED;

static const uint8 AllChannels5G_4_9G[] = WFM_5GHZ_4_9GHZ_ALL_CHANNELS_SUPPORTED;

static const uint8 WpaTkipCipherOui[] = WFM_WPA_TKIP_CIPHER_OUI;
static const uint8 WpaAesCipherOui[] = WFM_WPA_AES_CIPHER_OUI;
static const uint8 Wpa2TkipCipherOui[] = WFM_WPA2_TKIP_CIPHER_OUI;
static const uint8 Wpa2AesCipherOui[] = WFM_WPA2_AES_CIPHER_OUI;
static const uint8 Wpa8021xAkmOui[] = WFM_WPA_8021X_AKM_OUI;
static const uint8 WpaPskAkmOui[] = WFM_WPA_PSK_AKM_OUI;
static const uint8 Wpa28021xAkmOui[] = WFM_WPA2_8021X_AKM_OUI;
static const uint8 Wpa2PskAkmOui[] = WFM_WPA2_PSK_AKM_OUI;
#if FT_SUPPORT
static const uint8 Wpa2FtAkmOui[] = WFM_WPA2_FT_AKM_OUI;
static const uint8 Wpa2FtPskAkmOui[] = WFM_WPA2_FT_PSK_AKM_OUI;
#endif	/* FT_SUPPORT */
#if MGMT_FRAME_PROTECTION
static const uint8 Wpa2GroupMgmtCipherOui[] = WFM_WPA2_BIP_CIPHER_OUI;
static const uint8 Wpa28021xSh256AkmOui[] = WFM_WPA2_8021X_SH256_AKM_OUI;
static const uint8 Wpa2PskSh256AkmOui[] = WFM_WPA2_PSK_SH256_AKM_OUI;
#endif	/* MGMT_FRAME_PROTECTION */

/******************************************************************************
	      UMAC STATE-EVENT TABLE
******************************************************************************/

/*
  Note:
  We may get few Rx Indications even after getting reset complete callback,
  as the LMAC reset statemachine may wait to flush out the tx buffers before
  stopping the hw. So, we have to free the RxDescriptor and RxBuffer even in
  states like UMAC_UNINITIALIZED and UMAC_INITIALIZED. Thats why
  UMAC_ProcRxDescBuff() have been inserted in these states.
  When the system is stable , if there's no possibility to get RxIndications
  after ResetCompleteCB, these calls can be replaced with UMAC_ProcException()
  handler.

  In a similier way, De-Auth Req and De-Assoc Req may be recieved in any state,
  we should free the Rxdescriptors and Rxbuffers associated with them.
*/

#if 0 /* Commentes */
/* All Events - WFM_UMAC_EVENTS */
{
	/*  0 - 3  */ UMAC_INIT, UMAC_DEINIT, UMAC_START_SCAN, UMAC_STOP_SCAN,
	/*  4 - 7  */ UMAC_PROC_SCAN_RSP, UMAC_START_JOIN, UMAC_JOIN_SUCCESS, UMAC_JOIN_FAILED,
	/*  8 - 11 */ UMAC_START_AUTH, UMAC_AUTH_SUCCESS, UMAC_AUTH_FAILED, UMAC_DE_AUTHE,
	/* 12 - 15 */ UMAC_DE_AUTH_SUCCESS, UMAC_START_ASSOC, UMAC_ASSOC_SUCCESS, UMAC_ASSOC_FAILED,
	/* 16 - 19 */ UMAC_DATA_TX, UMAC_DATA_RX, UMAC_IN_BEACON, UMAC_DE_ASSOC,
	/* 20 - 23 */ UMAC_DE_ASSOC_SUCCESS, UMAC_DE_ASSOC_REQ, UMAC_DE_AUTH_REQ, UMAC_UNJOIN,
	/* 24 - 27 */ UMAC_UNJOIN_SUCCESS, UMAC_SCAN_COMPLETED, UMAC_TX_COMPLETED, UMAC_START_REASSOC,
	/* 28 - 29 */ UMAC_REASSOC_SUCCESS, UMAC_REASSOC_FAILED, UMAC_EXT_REQ_TO_WSM
};
#endif /* 0 */

UMAC_FSM_HANDLER UMAC_StateEventTable[UMAC_MAX_STATE][UMAC_MAX_EVT] = {
	/* UMAC_UNINITIALIZED */
	{
	 /*  0 - 3  */ UMAC_ProcInit, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /*  4 - 7  */ UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcReleaseRxBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 48 - 51 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_DefaultHostReq,
	 /* 56 - 59 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq
#endif	/* P2P_MINIAP_SUPPORT */
	},

	/* UMAC_INITIALIZED */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_ProcStartScan, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcStartJoin, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcReleaseRxBuff, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_ProcScanCompleted, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcExtMIBReq, UMAC_ProcStartJoin,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_ProcStartDeviceDiscovery, UMAC_ProcStartFind, UMAC_ProcStopFind, UMAC_ProcFindCompleted,
	 /* 48 - 51 */ UMAC_ProcStartGoNego, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_ProcStartInvtn,
	 /* 56 - 59 */ UMAC_ProcInvtnReq, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcStartAP,
	 /* 64 - 67 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_TxMgmtFrame, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq
#endif	/* P2P_MINIAP_SUPPORT */
	 },

#if P2P_MINIAP_SUPPORT
	/* UMAC_BSS_STARTING */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_QueueMessage,
	 /*  4 - 7  */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 12 - 15 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcReleaseRxBuff, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_DefaultHostReq, UMAC_ProcStartGoNegoCnf, UMAC_ProcStartGoNegoResp, UMAC_DefaultHostReq,
	 /* 52 - 55 */ UMAC_ProcGoNegoResp, UMAC_ProcGoNegoSuccess, UMAC_ProcGoNegoFailed, UMAC_DefaultHostReq,
	 /* 56 - 59 */ UMAC_ProcRxDescBuff, UMAC_ProcInvtnResp, UMAC_ProcInvtnSuccess, UMAC_ProcInvtnFailed,
	 /* 60 - 63 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcStartAPSuccess, UMAC_ProcStartAPFailed, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq
	 },

	/* UMAC_BSS_STARTED */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthenticate,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_ProcTxData, UMAC_ProcRxData, UMAC_ProcBeacon, UMAC_ProcDeAuthSTA,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcDeAssocReq, UMAC_ProcDeAuthReq, UMAC_ProcUnJoin,
	 /* 24 - 27 */ UMAC_ProcUnJoinSuccess, UMAC_DefaultHostReq, UMAC_ProcTxCompleted, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcExtMIBReq, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff,
	 /* 44 - 47 */ UMAC_ProcStartDeviceDiscovery, UMAC_ProcStartFind, UMAC_ProcStopFind, UMAC_ProcFindCompleted,
	 /* 48 - 51 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 56 - 59 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff,
	 /* 60 - 63 */ UMAC_AuthRespSuccess, UMAC_AuthRespFailure, UMAC_ProcAssocReq, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_ProcDeAuthSTA,
	 /* 68 - 71 */ UMAC_ProcUnLinkSTA, UMAC_ProcFwdFrame, UMAC_ProcFwdedFrmTxCompleted, UMAC_DlvrLegacyBufferedData,
	 /* 72 - 75 */ UMAC_DlvrWMMBufferedData, UMAC_SendNullFrame, UMAC_ProcUpdateTIMIE, UMAC_ProcDisconnectSTA,
	 /* 76 - 79 */ UMAC_ProcStopAP, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DlvrRequeueBufferedData,
	 /* 80 - 83 */ UMAC_ProcSendProbeRequest, UMAC_ProcUpdateERPIE
	 },

	/* UMAC_GROUP_FORMING */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcUnJoinSuccess, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_ProcStartFind, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_ProcStartGoNego, UMAC_ProcStartGoNegoCnf, UMAC_ProcStartGoNegoResp, UMAC_ProcGoNegoReq,
	 /* 52 - 55 */ UMAC_ProcGoNegoResp, UMAC_ProcGoNegoSuccess, UMAC_ProcGoNegoFailed, UMAC_DefaultHostReq,
	 /* 56 - 59 */ UMAC_ProcRxDescBuff, UMAC_ProcInvtnResp, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq
	 },

	/* UMAC_GROUP_FORMED */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_ProcStartBGScan, UMAC_DefaultHostReq,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthenticate,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_ProcTxData, UMAC_ProcRxData, UMAC_ProcBeacon, UMAC_ProcDeAuthSTA,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcDeAssocReq, UMAC_ProcDeAuthReq, UMAC_ProcUnJoin,
	 /* 24 - 27 */ UMAC_ProcUnJoinSuccess, UMAC_ProcScanCompleted, UMAC_ProcTxCompleted, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcExtMIBReq, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff,
	 /* 44 - 47 */ UMAC_ProcStartDeviceDiscovery, UMAC_ProcStartFind, UMAC_ProcStopFind, UMAC_ProcFindCompleted,
	 /* 48 - 51 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 56 - 59 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff,
	 /* 60 - 63 */ UMAC_AuthRespSuccess, UMAC_AuthRespFailure, UMAC_ProcAssocReq, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_ProcDeAuthSTA,
	 /* 68 - 71 */ UMAC_ProcUnLinkSTA, UMAC_ProcFwdFrame, UMAC_ProcFwdedFrmTxCompleted, UMAC_DlvrLegacyBufferedData,
	 /* 72 - 75 */ UMAC_DlvrWMMBufferedData, UMAC_SendNullFrame, UMAC_ProcUpdateTIMIE, UMAC_ProcDisconnectSTA,
	 /* 76 - 79 */ UMAC_ProcStopAP, UMAC_TxMgmtFrame, UMAC_ResetSeqNumCounter, UMAC_DlvrRequeueBufferedData,
	 /* 80 - 83 */ UMAC_ProcSendProbeRequest, UMAC_ProcUpdateERPIE
	 },
#endif	/* P2P_MINIAP_SUPPORT */

	/* UMAC_JOINING */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcJoinSuccess, UMAC_ProcJoinFailed,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcReleaseRxBuff, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 48 - 51 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_DefaultHostReq,
	 /* 56 - 59 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_JOINED */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_ProcStartAuth, UMAC_ProcRxDescBuff, UMAC_ProcAuthFailed, UMAC_DefaultHostReq,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcReleaseRxBuff, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcUnJoin,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_DefaultHostReq,
	 /* 56 - 59 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_AuthRespSuccess, UMAC_AuthRespFailure, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_UNJOINING */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcRxData, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcUnJoinSuccess, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_QueueMessage, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_QueueMessage,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_QueueMessage,
	 /* 56 - 59 */ UMAC_QueueMessage, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_AUTHENTICATING */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_ProcRxDescBuff, UMAC_ProcAuthSuccess, UMAC_ProcAuthFailed, UMAC_ProcDeAuthenticate,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcReleaseRxBuff, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthReq, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcFtAuthComplete, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_QueueMessage, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_DefaultHostReq,
	 /* 56 - 59 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_QueueMessage, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_AUTHENTICATED */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthenticate,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_ProcStartAssoc, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcReleaseRxBuff, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthReq, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_ProcStartReAssoc,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_ProcStartGoNego, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_QueueMessage,
	 /* 56 - 59 */ UMAC_ProcInvtnReq, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_QueueMessage, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_DE_AUTHENTICATING */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcRxData, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_QueueMessage, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_QueueMessage,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_QueueMessage,
	 /* 56 - 59 */ UMAC_QueueMessage, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_ASSOCIATING */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthenticate,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcAssocFailed,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcReleaseRxBuff, UMAC_ProcBeacon, UMAC_ProcDeAssoc,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcDeAssocReq, UMAC_ProcDeAuthReq, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_QueueMessage, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_QueueMessage,
	 /* 56 - 59 */ UMAC_QueueMessage, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_QueueMessage, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_ASSOCIATED */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_ProcStartBGScan, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcFtJoinComplete, UMAC_ProcFtJoinComplete,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthenticate,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_ProcAssocSuccess, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_ProcTxData, UMAC_ProcRxData, UMAC_ProcBeacon, UMAC_ProcDeAssoc,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcDeAssocReq, UMAC_ProcDeAuthReq, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_ProcScanCompleted, UMAC_ProcTxCompleted, UMAC_ProcStartReAssoc,
	 /* 28 - 31 */ UMAC_ProcReAssocSuccess, UMAC_ProcRxDescBuff, UMAC_ProcExtMIBReq, UMAC_ProcStartFtAuth,
	 /* 32 - 35 */ UMAC_ProcFtAuthComplete, UMAC_ProcRadioMeasurement, UMAC_ProcMsrmtReport, UMAC_ProcLinkMeasurement,
	 /* 36 - 39 */ UMAC_ProcSendMsrmtReq, UMAC_ProcNeighborReportReq, UMAC_ProcSendLinkMsrmtReq, UMAC_ProcSendLinkMsrmtRep,
	 /* 40 - 43 */ UMAC_ProcNeighborRepResp, UMAC_ProcStartSaQuery, UMAC_ProcSaQuerySuccess, UMAC_ProcSaQueryReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_ProcStartGoNego, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcGoNegoReq,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_ProcStartInvtn,
	 /* 56 - 59 */ UMAC_ProcInvtnReq, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_AuthRespSuccess, UMAC_ProcRxDescBuff, UMAC_ProcAssocReq, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_SendNullFrame, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_TxMgmtFrame, UMAC_ProcRxDescBuff, UMAC_DlvrRequeueBufferedData,
	 /* 80 - 83 */ UMAC_ProcSendProbeRequest, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_DISASSOCIATING */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthenticate,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcRxData, UMAC_ProcBeacon, UMAC_DefaultHostReq,
	 /* 20 - 23 */ UMAC_ProcDeAssocSuccess, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthReq, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_QueueMessage, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_QueueMessage,
	 /* 56 - 59 */ UMAC_QueueMessage, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_ProcRxDescBuff, UMAC_AuthRespFailure, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_REASSOCIATING */
	{
	 /*  0 - 3  */ UMAC_DefaultHostReq, UMAC_ProcDeInit, UMAC_QueueMessage, UMAC_ProcStopScan,
	 /*  4 - 7  */ UMAC_ProcScanRsp, UMAC_ProcReConnect, UMAC_ProcException, UMAC_ProcException,
	 /*  8 - 11 */ UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_ProcDeAuthenticate,
	 /* 12 - 15 */ UMAC_ProcDeAuthSuccess, UMAC_DefaultHostReq, UMAC_ProcAssocSuccess, UMAC_ProcAssocFailed,
	 /* 16 - 19 */ UMAC_DefaultHostReq, UMAC_ProcReleaseRxBuff, UMAC_ProcBeacon, UMAC_ProcDeAssoc,
	 /* 20 - 23 */ UMAC_ProcException, UMAC_ProcDeAssocReq, UMAC_ProcDeAuthReq, UMAC_DefaultHostReq,
	 /* 24 - 27 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcMgmtTxBuffer, UMAC_DefaultHostReq,
	 /* 28 - 31 */ UMAC_DefaultHostReq, UMAC_ProcReAssocFailed, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 32 - 35 */ UMAC_ProcRxDescBuff, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 36 - 39 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
	 /* 40 - 43 */ UMAC_ProcRxDescBuff, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff,
	 /* 48 - 51 */ UMAC_QueueMessage, UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_QueueMessage,
	 /* 52 - 55 */ UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException, UMAC_DefaultHostReq,
	 /* 56 - 59 */ UMAC_QueueMessage, UMAC_ProcRxDescBuff, UMAC_ProcException, UMAC_ProcException,
	 /* 60 - 63 */ UMAC_QueueMessage, UMAC_ProcRxDescBuff, UMAC_QueueMessage, UMAC_DefaultHostReq,
	 /* 64 - 67 */ UMAC_ProcException, UMAC_ProcException, UMAC_ProcSchAsyncMgmt, UMAC_DefaultHostReq,
	 /* 68 - 71 */ UMAC_DefaultHostReq, UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException,
	 /* 72 - 75 */ UMAC_ProcException, UMAC_DefaultHostReq, UMAC_ProcException, UMAC_ProcException,
	 /* 76 - 79 */ UMAC_DefaultHostReq, UMAC_DefaultHostReq, UMAC_ProcRxDescBuff, UMAC_DefaultHostReq,
	 /* 80 - 83 */ UMAC_DefaultHostReq, UMAC_ProcException
#endif	/* P2P_MINIAP_SUPPORT */
	 },

	/* UMAC_PROCESSING */
	{
	 /*  0 - 3  */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /*  4 - 7  */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /*  8 - 11 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 12 - 15 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_ProcAssocSuccess, UMAC_QueueMessage,
	 /* 16 - 19 */ UMAC_ProcTxData, UMAC_ProcRxData, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 20 - 23 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 24 - 27 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_ProcTxCompleted, UMAC_QueueMessage,
	 /* 28 - 31 */ UMAC_ProcReAssocSuccess, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 32 - 35 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 36 - 39 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 40 - 43 */ UMAC_QueueMessage, UMAC_ProcStartSaQuery, UMAC_ProcSaQuerySuccess, UMAC_ProcSaQueryReq,
#if P2P_MINIAP_SUPPORT
	 /* 44 - 47 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 48 - 51 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 52 - 55 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 56 - 59 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 60 - 63 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 64 - 67 */ UMAC_QueueMessage, UMAC_QueueMessage, UMAC_ProcSchAsyncMgmt, UMAC_QueueMessage,
	 /* 68 - 71 */ UMAC_QueueMessage, UMAC_ProcFwdFrame, UMAC_ProcFwdedFrmTxCompleted, UMAC_DlvrLegacyBufferedData,
	 /* 72 - 75 */ UMAC_DlvrWMMBufferedData, UMAC_SendNullFrame, UMAC_QueueMessage, UMAC_QueueMessage,
	 /* 76 - 79 */ UMAC_QueueMessage, UMAC_TxMgmtFrame, UMAC_QueueMessage, UMAC_DlvrRequeueBufferedData,
	 /* 80 - 83 */ UMAC_ProcSendProbeRequest, UMAC_QueueMessage
#endif	/* P2P_MINIAP_SUPPORT */
	 }
};

/******************************************************************************
			    LOCAL FUNCTIONS
******************************************************************************/

/******************************************************************************
 * NAME:	UMAC_ProcInit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the UMAC State machine ,associated lists and
 * variables.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcInit(UMAC_HANDLE UmacHandle, void *pMsg)
{
	UMAC_TX_PKT *pUmacPkt;
	uint32 Result = WFM_STATUS_SUCCESS;
	uint8 Count = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_DEV_LIST_STATUS DevListStatus = WFM_DEV_LIST_FAILURE;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	OS_ASSERT(pUmacInstance);
	LOG_EVENT(DBG_WFM_UMAC, "UMAC_ProcInit()\n");

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	LOG_DATA3(DBG_WFM_UMAC, "UMAC_ProcInit(): sz(WFM_HT_EXTENDED_CAP_IE)=%d,sz\
				(WFM_HT_CAPABILITIES)= %d, sz(WFM_HT_SUPPORTED_MCS_SET_PACKED)=%d\n", sizeof(WFM_HT_EXTENDED_CAP_PACKED), sizeof(WFM_HT_CAPABILITIES), sizeof(WFM_HT_SUPPORTED_MCS_SET_PACKED)
				);

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;

		pSmgmtReq->Type = SMGMT_REQ_TYPE_INIT;

		pSmgmtReq->ReqNum = 0;

		OS_MemoryReset(
		(void *)((uint8 *) pUmacInstance + sizeof(UMAC_PERMENENT_DATA)),
		(sizeof(WFM_UMAC_INSTANCE) - sizeof(UMAC_PERMENENT_DATA))
		);

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);
		Result = pSmgmtReq->status;

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;

	} /* if(!pSmgmtReq->HandleMgmtReq) */

	pSmgmtReq->HandleMgmtReq = 0;

	pUmacInstance->JoinPending = FALSE;

	DevListStatus = WFM_UMAC_DEV_InitList(pUmacInstance);

	if (WFM_DEV_LIST_FAILURE == DevListStatus)
		LOG_EVENT(DBG_WFM_UMAC, "Failed to initialize Device List\n");

	/*Initializing the Pending event queue */
	WFM_UMAC_2_UL_Evt_Q_Init(UmacHandle);

	/*Initializing the UMAC timer */
	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pUMACTimer,
		UMAC_TimeOut_Cb
		);

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer,
		UMAC_RateAdapTimeOut_Cb
		);

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer,
		UMAC_CheckForConnectionTimeOut_Cb
		);

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer,
		UMAC_CheckIfUnJoinCanProceedTimeOut_Cb
		);

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pUnExtendedJoinTimer,
		UMAC_CheckIfForceUnJoinCanProceedTimeOut_Cb
		);

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pBssLostTimer,
		UMAC_BssLostTimeout_Cb
		);

#if 0
	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pCancelRemainOnChTimer,
		UMAC_CancelRemainOnChTimeOut_Cb
		);
#endif

#if DOT11K_SUPPORT
	/*OS_INIT_TIMER
	   (
	   pUmacInstance->gVars.p.UmacTimers.pFrameTimer,
	   UMAC_FrameMeasTimeOut_Cb
	   ) ; */

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer,
		UMAC_MeasPauseTimeOut_Cb
		);

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer,
		UMAC_TxStreamMeasTimeOut_Cb
		);
#endif	/* DOT11K_SUPPORT */

#if MGMT_FRAME_PROTECTION
	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer,
		UMAC_SaQueryRetryTimeOut_Cb
		);

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.ReAssocTimer,
		UMAC_SaQueryRetryTimeOut_Cb
		);

#endif	/* MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
	for (Count = 0; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++) {
		OS_INIT_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[Count],
			UMAC_ClientConnectionTimeout_Cb
			);
	}

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pUmacGroupFormationTimer,
		UMAC_GroupFormationTimeout_Cb
		);

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pUmacInvitationTimer,
		UMAC_P2P_InvitationTimeout_Cb
		);

	for (Count = 0; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++) {
		OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pUMACSTAUnJoinTimer[Count],
		UMAC_CheckIfUnJoinCanProceedTimeOut_Cb
		);
	}

	OS_INIT_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pUMACCleanupTimer,
		UMAC_CleanupTimeout_Cb
		);

	pUmacInstance->dialogToken = 1;

#endif	/* P2P_MINIAP_SUPPORT */

	/*
	   Reading the Local MAC address  and storing it in UMAC Instance
	 */

	if (Result) {
		LOG_EVENT(DBG_WFM_UMAC, "Failed to Read MAC ADDRESS\n");

		/* Inform the host that we failed to intialze */
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_GENERAL_FAILURE,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		goto exit_handler;

	} /* if(Result) */

	pUmacInstance->dot11InfraStructureMode = WFM_802_AUTO_UNKNOWN;

	/*Initializing the TxDesc Store */
	for (Count = 0; Count < CFG_HI_NUM_REQS; Count++) {
		pUmacPkt = (UMAC_TX_PKT *) &pUmacInstance->gVars.TxDescStore[Count];

		pUmacPkt->pTxDesc = NULL;
		pUmacPkt->reference = 0;
		pUmacPkt->pMsgPtr = NULL;

		pUmacInstance->gVars.PacketIdPool[Count] = Count;

	} /* for(Count=0 ;Count<CFG_HI_NUM_REQS ; Count++) */

	pUmacInstance->gVars.PktIdGet = 0;
	pUmacInstance->gVars.PktIdPut = Count;


	/*Initialize the txDesc list for internal frames */
	UMAC_Initialize_Internal_TxDescList(UmacHandle);

#if P2P_MINIAP_SUPPORT
	for (Count = 0; Count < UMAC_NUM_AP_INTERNAL_TX_BUFFERS; Count++)
		pUmacInstance->gVars.ApPacketIdPool[Count] = Count;
	OS_LOCK(pUmacInstance->gVars.p.ApPacketIdPoolLock);
	pUmacInstance->gVars.ApPktIdGet = 0;
	pUmacInstance->gVars.ApPktIdPut = Count;
	OS_UNLOCK(pUmacInstance->gVars.p.ApPacketIdPoolLock);

	/*Initialize the txDesc list for internal frames */
	UMAC_Initialize_Internal_ApTxDescList(UmacHandle);

	if (pUmacInstance->gVars.p.useP2P == UMAC_P2P_ENABLED) {
		/*Remove the 11n rates from OurAllSupportedRates_Abgn and OurAllBasicSupportedRates_Abgn*/
		pUmacInstance->OurAllSupportedRates_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED ;
		pUmacInstance->OurAllBasicSupportedRates_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED;
		pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED;
	} else
#endif	/* P2P_MINIAP_SUPPORT */
	{
		pUmacInstance->OurAllSupportedRates_Abgn = UMAC_SUPPORTED_RATES_OURS_DEFAULT;
		pUmacInstance->OurAllBasicSupportedRates_Abgn = UMAC_BASIC_SUPPORTED_RATES_OURS_DEFAULT;
	}

#if 0
	pUmacInstance->OurAllSupportedRates_Abgn = UMAC_SUPPORTED_RATES_OURS_DEFAULT;
	pUmacInstance->OurAllBasicSupportedRates_Abgn = UMAC_BASIC_SUPPORTED_RATES_OURS_DEFAULT;
#endif

	pUmacInstance->ErpInfo = 0;	/* Initialize it to 0 */

	pUmacInstance->UmacScanState = UMAC_READY_TO_SCAN;

	pUmacInstance->dtimSkippingCount = 1;
	LOG_EVENT(DBG_WFM_UMAC, "SCAN STATE = UMAC_READY_TO_SCAN\n");

	/*Hardcoding to 1(Disabling BG scan) */
	/*pUmacInstance->gVars.p.InfoHost.disableBGScan = 1; */

	/* receiving scan first time by default */
	pUmacInstance->StartScanInNextBand = FALSE;

#if P2P_MINIAP_SUPPORT
	pUmacInstance->UmacFindState = UMAC_READY_TO_FIND;
	LOG_EVENT(DBG_WFM_UMAC, "SCAN STATE = UMAC_READY_TO_FIND\n");
	pUmacInstance->dot11BssConf.regClass = UMAC_REG_CLASS_FCC_2G;
	pUmacInstance->sta[DEFAULT_LINK_ID].PsState = STA_PS_STATE_AWAKE;
#endif	/* P2P_MINIAP_SUPPORT */

	UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");

	pUmacInstance->dot11InfraStructureMode = WFM_802_AUTO_UNKNOWN;
	pUmacInstance->Channelnumber = 1;
	/* WMM disabled */
	pUmacInstance->enableWMM = 0;
	/* WEP disabled */
	pUmacInstance->enableWEP = 0;
	/* Power save not supported */
	pUmacInstance->atimWinSize = 0;
	pUmacInstance->beaconInterval = WFM_DEFAULT_BEACON_INTERVAL;
	/*Default network type: 11b */
	pUmacInstance->networkTypeInUse = NETWORK_TYPE_11B;

	for (Count = 0; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++)
		pUmacInstance->statisticCount[Count] = 0;

#if MGMT_FRAME_PROTECTION
	pUmacInstance->protectMgmtFrame.CheckSaQueryInterval = 0;
	pUmacInstance->protectMgmtFrame.isValidSaQueryResp = FALSE;
	pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm = FALSE;
	pUmacInstance->protectMgmtFrame.tansactionIdentifer = 0;
	pUmacInstance->hostMfpPolicy = 0;
	pUmacInstance->bssInfoMfpPolicy = 0;
	pUmacInstance->saQueryInterval.saQueryMaxTimeout = 1000;
	pUmacInstance->saQueryInterval.saQueryRetryTimeout = 200;
#endif	/* MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
	pUmacInstance->operatingMode = OPERATING_MODE_STA;
	pUmacInstance->networkTypeInUse = NETWORK_TYPE_11BG;
	pUmacInstance->TIMparams.DTIMcount = 0;
	pUmacInstance->TIMparams.DTIMperiod = 1;
	pUmacInstance->TIMparams.bitmapCTRL = 0;
	pUmacInstance->dot11BssConf.DTIMperiod = 1;
	pUmacInstance->dot11BssConf.beaconInterval = 100;
	pUmacInstance->UpdateNewTIMInfo = FALSE;
	pUmacInstance->PsBuffDataCount = 0;
#endif	/* P2P_MINIAP_SUPPORT */

	/*If there are pending request in the queue, service them */
	UMAC_MOVE_PENDING_MSGS_TO_ACTIVE_Q(UmacHandle);

	if (UMAC_FORCE_START_ON == pUmacInstance->gVars.p.ForceStart) {

		pUmacInstance->gVars.p.ForceStart = UMAC_FORCE_START_OFF;

		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_START_COMPLETED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);

	} /* if( UMAC_FORCE_START_ON == pUmacInstance->gVars.p.ForceStart ) */
	else if (pUmacInstance->gVars.p.ForceStopMode == UMAC_FORCE_STOP_ON) {
		/*
		   Its a forced stop, so should send STOP_COMPLETED_EVENT to
		   the upper layer
		 */

		pUmacInstance->gVars.p.ForceStopMode = UMAC_FORCE_STOP_OFF;

		if ((pUmacInstance->gVars.p.stopAPFlag) &&
			(pUmacInstance->gVars.p.sendStopAPEvt)) {
				WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_STOP_AP_COMPLETED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		} else {
			WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_STOP_COMPLETED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		}

		pUmacInstance->gVars.p.stopAPFlag = 0;
		pUmacInstance->gVars.p.sendStopAPEvt = 0;


	} /* if(pUmacInstance->gVars.p.ForceStopMode == UMAC_FORCE_STOP_ON) */

 exit_handler:
	return Result;
} /* end UMAC_ProcInit() */

/******************************************************************************
 * NAME:	UMAC_ProcDeInit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function De-initializes the UMAC State machine ,associated lists and
 * variables. This function releases any allocated buffers which are not
 * released at the point.
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcDeInit(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_DEV_LIST_STATUS DevListStatus = WFM_DEV_LIST_FAILURE;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 memstatus;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC, "UMAC_ProcDeInit()\n");

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer);
	OS_STOP_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer
		);

#if DOT11K_SUPPORT
	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer);
	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer);
	/*Reset all the 11k variables */
	OS_MemoryReset(
			&pUmacInstance->dot11k_Meas_Params,
			sizeof(DOT11K_MEAS_PARAMS)
			);
#endif	/* DOT11K_SUPPORT */

	UMAC_ProcessPendingTxRequests(pUmacInstance, DEFAULT_LINK_ID);

	DevListStatus = WFM_UMAC_DEV_DeInitList(pUmacInstance);

	if (WFM_DEV_LIST_FAILURE == DevListStatus)
		LOG_EVENT(DBG_WFM_UMAC, "Failed to De-initialize Device List\n");


	if (pUmacInstance->gVars.pScanTxDesc) {
		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pScanTxDesc);
		/* Freeing the internal buffer */
		memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pUmacInstance->gVars.pScanTxDesc
					);
		OS_ASSERT(memstatus);

		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pScanTxDesc);

		pUmacInstance->gVars.pScanTxDesc = NULL;
	}

	if (pUmacInstance->gVars.pJoinTxDesc) {
		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);
		/* Freeing the internal buffer */
		memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pJoinTxDesc
						);

		OS_ASSERT(memstatus);

		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pJoinTxDesc);

		pUmacInstance->gVars.pJoinTxDesc = NULL;
	}

	OS_MemoryReset(
			&pUmacInstance->gVars.p.PsMode.PmMode,
			sizeof(pUmacInstance->gVars.p.PsMode.PmMode)
			);
	pUmacInstance->gVars.p.PsMode.PmMode = WFM_PS_MODE_DISABLED;
	pUmacInstance->gVars.p.PsMode.FastPsmIdlePeriod = 0;
	pUmacInstance->gVars.p.PsMode.ApPsmChangePeriod = 0;

#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->gVars.pStartReq) {
		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pStartReq);
		/* Freeing the internal buffer */
		memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pStartReq
						);

		OS_ASSERT(memstatus);

		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pStartReq);

		pUmacInstance->gVars.pStartReq = NULL;
	}
#endif	/* P2P_MINIAP_SUPPORT */

	UMAC_DeInit_Internal_TxDescList(UmacHandle, 0);

	/*Release the Ie Memory */
	if (pUmacInstance->VendorIe.pProbeReqIE) {
		OS_Free(pUmacInstance->VendorIe.pProbeReqIE);
		pUmacInstance->VendorIe.pProbeReqIE = NULL;
	}

	if (pUmacInstance->VendorIe.pProbeRespIE) {
		OS_Free(pUmacInstance->VendorIe.pProbeRespIE);
		pUmacInstance->VendorIe.pProbeRespIE = NULL;
	}

	if (pUmacInstance->VendorIe.pBeaconIE) {
		OS_Free(pUmacInstance->VendorIe.pBeaconIE);
		pUmacInstance->VendorIe.pBeaconIE = NULL;
	}

	if (pUmacInstance->VendorIe.pAssocReqIE) {
		OS_Free(pUmacInstance->VendorIe.pAssocReqIE);
		pUmacInstance->VendorIe.pAssocReqIE = NULL;
	}

	if (pUmacInstance->VendorIe.pAssocRespIE) {
		OS_Free(pUmacInstance->VendorIe.pAssocRespIE);
		pUmacInstance->VendorIe.pAssocRespIE = NULL;
	}
#if 0
	if (pUmacInstance->VendorIe.pReqIE) {
		OS_Free(pUmacInstance->VendorIe.pReqIE);
		pUmacInstance->VendorIe.pReqIE = NULL;
	}

	if (pUmacInstance->VendorIe.pRespIE) {
		OS_Free(pUmacInstance->VendorIe.pRespIE);
		pUmacInstance->VendorIe.pRespIE = NULL;
	}
#endif

	if (pUmacInstance->gVars.p.RcpiRssiThresholdRcvd)
		pUmacInstance->gVars.p.RcpiRssiThresholdRcvd = 0;
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
		if (pUmacInstance->gVars.p.sendStopAPEvt) {
			pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost =
				WFM_ENC_STATUS_NO_ENCRYPTION;
			pUmacInstance->gVars.p.PsMode.PmMode = WFM_PS_MODE_DISABLED;
			pUmacInstance->gVars.p.OprPwrMode = WFM_OPR_PWR_MODE_ACTIVE;
		}
	}
	OS_MemoryReset(
		(void *)((uint8 *) pUmacInstance + sizeof(UMAC_PERMENENT_DATA)),
		(sizeof(WFM_UMAC_INSTANCE) - sizeof(UMAC_PERMENENT_DATA))
		);

	if (UMAC_FORCE_STOP_RESET == pUmacInstance->gVars.p.ForceStopMode)
		Result = UMAC_LL_REQ_RESET_JOIN(UmacHandle, 0x00, 0);


	/*
	   Since UMAC_UNINITIALIZED is zero, the above statement should have
	   done this state change. If the position of UMAC_UNINITIALIZED is
	   changing in future it may cause an issue. Thats why the following
	   state change is there.
	 */

	UMAC_SET_STATE(UmacHandle, UMAC_UNINITIALIZED);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_UNINITIALIZED\n");

	return Result;
} /* end UMAC_ProcDeInit() */

/******************************************************************************
 * NAME:	UMAC_ProcStartScan
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function Initiates a scan based on the parameters provided.If the SM
 * [State-Machine] is already scanning, this function will return immediatly
 * with a rejected status.This funciton scans all the channels in the given
 * band.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartScan(UMAC_HANDLE UmacHandle, void *pMsg)
{
	SCAN_PARAMETERS *pScanParams = NULL;
	WFM_BSS_LIST_SCAN *pWfmListScan = NULL;
	uint32 Result = WFM_STATUS_SUCCESS;
	uint8 Count = 0;
	uint32 PowerLevel = WFM_UMAC_DEFAULT_POWER_LEVEL;
	uint8 *pChann = NULL;
	uint16 *pChannNo = NULL;
	sint32 *pPowerLevel = NULL;
	uint32 u32OurAllBasicSupportedRatesAbgn = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	uint16 ChannelOffset = 0;
	uint16 ChannelsNotSupported = 0;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "UMAC_ProcStartScan()\n");

	OS_ASSERT(pMsg);

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	if (pUmacInstance->UmacScanState == UMAC_READY_TO_SCAN) {

		pUmacInstance->UmacScanState = UMAC_SCANNING;
		LOG_EVENT(DBG_WFM_UMAC, "SCAN STATE = UMAC_SCANNING\n");

		CHECK_POINTER_IS_VALID(pMsg);

		if (pUmacInstance->StartScanInNextBand == FALSE) {
			/* we have just recd scan request */
			pWfmListScan = (WFM_BSS_LIST_SCAN *) pMsg;
			OS_MemoryCopy(
				&pUmacInstance->ScanReqFromHost.ScanReq,
				pWfmListScan,
				sizeof(WFM_BSS_LIST_SCAN) - (sizeof(WFM_CHANNELS) * WFM_MAX_SUPPORTED_CHANNELS) + (pWfmListScan->NumberOfChannels_2_4Ghz + pWfmListScan->NumberOfChannels_5Ghz) * sizeof(WFM_CHANNELS)
				);

			/*
			  we would need to modify it, so do it in the local
			  copy
			*/
			pWfmListScan = &pUmacInstance->ScanReqFromHost.ScanReq;

			/*Clearing the Device cache */
			WFM_UMAC_DEV_ClearList(pUmacInstance);

		} else {
			/*
			  We have completed scan in one band. Now go to next
			  band
			*/
			pWfmListScan = &pUmacInstance->ScanReqFromHost.ScanReq;
		}

		pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;

		if (!pSmgmtReq->HandleMgmtReq) {
			OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
			pSmgmtReq->HandleMgmtReq = 1;

			pSmgmtReq->Type = SMGMT_REQ_TYPE_SCAN_REQ;

			pSmgmtReq->operation.scan_mib_params.phy_band = (uint8)
			    (pWfmListScan->Flags & WFM_BSS_LIST_SCAN_BAND_MASK);

			pSmgmtReq->ReqNum = 0;

			pUmacInstance->UmacScanState = UMAC_READY_TO_SCAN;

			Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

			if (Result != WFM_STATUS_SUCCESS)
				goto exit_handler;

		}	/*if(!pSmgmtReq->HandleMgmtReq) */
		pSmgmtReq->HandleMgmtReq = 0;

		OS_ASSERT(!pUmacInstance->gVars.pScanTxDesc);

		pUmacInstance->gVars.pScanTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

		OS_ASSERT(pUmacInstance->gVars.pScanTxDesc);

		UMAC_MEM_STAMP_INTERNAL_BUFFER(
					pUmacInstance->gVars.pScanTxDesc,
					UMAC_SCAN_REQ
					);

		WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pScanTxDesc);

		pScanParams = (SCAN_PARAMETERS *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pScanTxDesc);

		CHECK_POINTER_IS_VALID(pScanParams);

		/* Scan all bands in auto band */
		if (pWfmListScan->Flags & WFM_BSS_LIST_SCAN_AUTO_BAND) {
			pWfmListScan->Flags |= WFM_BSS_LIST_SCAN_2_4G_BAND | WFM_BSS_LIST_SCAN_4_9G_BAND | WFM_BSS_LIST_SCAN_5G_BAND;

			/* clear auto band bit */
			pWfmListScan->Flags &= ~(WFM_BSS_LIST_SCAN_AUTO_BAND);

		}		/*if ( pWfmListScan->Flags & WFM_BSS_LIST_SCAN_AUTO_BAND ) */

		/* Start from 2.4 G band */
		if (pWfmListScan->Flags & WFM_BSS_LIST_SCAN_2_4G_BAND) {

			LOG_EVENT(DBG_WFM_SCAN | DBG_WFM_CURRENT_AK, "Start Scanning in 2.4G\n");

			/* Clear the band bit */
			pWfmListScan->Flags &= ~WFM_BSS_LIST_SCAN_2_4G_BAND;

			/* Ask WSM to scan in 2.4 G */
			pScanParams->Band = PHY_BAND_2G;
			/*
			   all our basic supported rates
			   & rates for 2.4ghz band
			*/
			u32OurAllBasicSupportedRatesAbgn = pUmacInstance->OurAllBasicSupportedRates_Abgn
			    & UMAC_PHY_BAND_2_4G_SUPPORTED_RATES;

			pUmacInstance->gVars.p.InfoHost.ScanPhyBand = PHY_BAND_2G;

		} /* Then scan in 5 G */
		else if (pWfmListScan->Flags & (WFM_BSS_LIST_SCAN_5G_BAND | WFM_BSS_LIST_SCAN_4_9G_BAND)) {
			LOG_EVENT(DBG_WFM_SCAN | DBG_WFM_CURRENT_AK, "Start Scanning in 4.9/5G\n");

			/* Clear the band bit */
			pWfmListScan->Flags &= ~(WFM_BSS_LIST_SCAN_5G_BAND | WFM_BSS_LIST_SCAN_4_9G_BAND);

			/*4.9 Ghz should be mapped to 5 Ghz */
			pScanParams->Band = PHY_BAND_5G;

			/*
			   all our basic supported rates
			   & rates for 5ghz band
			*/
			u32OurAllBasicSupportedRatesAbgn = pUmacInstance->OurAllBasicSupportedRates_Abgn
			    & UMAC_PHY_BAND_5G_SUPPORTED_RATES;

			pUmacInstance->gVars.p.InfoHost.ScanPhyBand = PHY_BAND_5G;
		}

		pScanParams->AutoScanInterval = 0;
		if (pWfmListScan->RssiThreshold)
			pScanParams->AutoScanInterval |= (pWfmListScan->RssiThreshold << 24);


		if (pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[pUmacInstance->gVars.p.InfoHost.ScanPhyBand] != TRUE) {
			if (pUmacInstance->gVars.p.Enable11dFeature) {
				pScanParams->NumOfProbeRequests = 0;
			} else {
				/*Linux Case */
				pScanParams->NumOfProbeRequests = pWfmListScan->NumOfProbeRequests;
			}
		} else {
			/*Its an active scan */
			/*Check if driver has provided value for this */
			if (pWfmListScan->NumOfProbeRequests)
				pScanParams->NumOfProbeRequests = pWfmListScan->NumOfProbeRequests;
			else
				pScanParams->NumOfProbeRequests = WFM_NUM_PROBE_REQ_FOR_ACTIVE_SCAN;

		}

		if (pWfmListScan->Flags & WFM_BSS_LIST_SCAN_BAND_MASK) {
			/* scan needs to be performed on another band as well */
			pUmacInstance->StartScanInNextBand = TRUE;
			LOG_EVENT(DBG_WFM_SCAN | DBG_WFM_CURRENT_AK, "WFM: First Scan\n");
		} else {
			/*scan in last band */
			pUmacInstance->StartScanInNextBand = FALSE;
			LOG_EVENT(DBG_WFM_SCAN | DBG_WFM_CURRENT_AK, "WFM: Last Scan\n");
		}

		if (pUmacInstance->IsBgScan) {
			/*If we are connected, we should use background scan */
			pScanParams->ScanType = WSM_SCAN_TYPE_BG;

			/*
			  set ScanFlags to use long preamble and 11n mixed mode
			*/
			if ((pUmacInstance->BssLostEventRcvd)
			    || (pUmacInstance->TxFailureEventRcvd)
			    || (pUmacInstance->RssiRcpiThresholdEventRcvd))
				pScanParams->ScanFlags = WSM_SCAN_F_FORCED_BACKGROUND;
			else
				pScanParams->ScanFlags = WSM_SCAN_F_SPLIT_SCAN;

			LOG_EVENT(DBG_WFM_SCAN, "WFM: BKGRND Scan: ERROR ??\n");
		} else {
			pScanParams->ScanType = WSM_SCAN_TYPE_FG;
			/*
			  set ScanFlags to use long preamble and 11n mixed mode
			  (hence 0)
			*/
			pScanParams->ScanFlags = 0;

			LOG_EVENT(DBG_WFM_SCAN, "WFM: FGRND Scan\n");

		}		/*if( pUmacInstance->IsBgScan ) */

		/* Select lowest rate so that most AP/Sta can respond */
		pScanParams->MaxTransmitRate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(u32OurAllBasicSupportedRatesAbgn);

		pScanParams->ProbeDelay = WFM_DEFAULT_TIME_DIFF_BET_PROBE_REQ_IN_SCAN;

		if (pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[pUmacInstance->gVars.p.InfoHost.ScanPhyBand] != TRUE) {
			if (pScanParams->Band == WSM_BAND24) {
				/*2.4 Ghz */
				/*
				  If Channels are provided in Scan request by
				  the driver
				*/
				if (!pWfmListScan->NumberOfChannels_2_4Ghz) {
					pScanParams->NumOfChannels = sizeof(Channels2G);
					pChann = (uint8 *) &Channels2G[0];
					for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
						pScanParams->Channels[Count].ChannelNum = pChann[Count] + ChannelOffset;
						pScanParams->Channels[Count].MinChannelTime = WFM_UMAC_PASSIVE_SCAN_MIN_CHANNEL_TIME;
						pScanParams->Channels[Count].MaxChannelTime = WFM_UMAC_PASSIVE_SCAN_MAX_CHANNEL_TIME;
						pScanParams->Channels[Count].TxPowerLevel = PowerLevel;
					}
				} else {
					/*
					  Driver has given the channels for
					  2.4 Ghz
					*/
					if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
						/*
						 * For P2P GO, we restrict the list of channel to scan to
						 * only 3 channels - 1, 6, 11 (social channels as per P2P spec.
						 * Additionally we fix the maxChannelTime to 32 ms due to
						 * restriction imposed by firmware.
						 */
						pScanParams->NumOfChannels = 3;

						pScanParams->Channels[0].ChannelNum = 1;
						pScanParams->Channels[1].ChannelNum = 6;
						pScanParams->Channels[2].ChannelNum = 11;

						for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
							pScanParams->Channels[Count].MinChannelTime	= 10;
							pScanParams->Channels[Count].MaxChannelTime	= 32;
							pScanParams->Channels[Count].TxPowerLevel	= 128;
						}
					} else {
						pScanParams->NumOfChannels = (uint8) pWfmListScan->NumberOfChannels_2_4Ghz;
						for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
							pScanParams->Channels[Count].ChannelNum = pWfmListScan->Channels[Count].ChannelNum;
							pScanParams->Channels[Count].MinChannelTime = pWfmListScan->Channels[Count].MinChannelTime ? pWfmListScan->Channels[Count].MinChannelTime : WFM_UMAC_PASSIVE_SCAN_MIN_CHANNEL_TIME;
							pScanParams->Channels[Count].MaxChannelTime = pWfmListScan->Channels[Count].MaxChannelTime ? pWfmListScan->Channels[Count].MaxChannelTime : WFM_UMAC_PASSIVE_SCAN_MAX_CHANNEL_TIME;
							pScanParams->Channels[Count].TxPowerLevel = pWfmListScan->Channels[Count].TxPowerLevel ? pWfmListScan->Channels[Count].TxPowerLevel : PowerLevel;
						}
					}
				}

			} else {
				/*5Ghz */
				if (!pWfmListScan->NumberOfChannels_5Ghz) {
					pScanParams->NumOfChannels = sizeof(AllChannels5G_4_9G);
					pChann = (uint8 *) &AllChannels5G_4_9G[0];
					ChannelOffset = 0x0000;
					for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
						pScanParams->Channels[Count].ChannelNum = pChann[Count] + ChannelOffset;
						pScanParams->Channels[Count].MinChannelTime = WFM_UMAC_PASSIVE_SCAN_MIN_CHANNEL_TIME;
						pScanParams->Channels[Count].MaxChannelTime = WFM_UMAC_PASSIVE_SCAN_MAX_CHANNEL_TIME;
						pScanParams->Channels[Count].TxPowerLevel = PowerLevel;

						if (Count == (pScanParams->NumOfChannels - sizeof(Channels4_9G_20M) - 1)) {
							/* The last 4 channels from the list are for MPHPT 7-11 regulatory
							   domain, so change the offset for those channels */
							ChannelOffset = 0x0200;
						}
					}
				} else {
					/*Driver has provided the channels for 5Ghz */
					pScanParams->NumOfChannels = (uint8) pWfmListScan->NumberOfChannels_5Ghz;
					for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
						pScanParams->Channels[Count].ChannelNum = pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].ChannelNum;	/*No need to add channel offset as it is provided by driver */
						pScanParams->Channels[Count].MinChannelTime =
						    pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].MinChannelTime ? pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].MinChannelTime : WFM_UMAC_PASSIVE_SCAN_MIN_CHANNEL_TIME;
						pScanParams->Channels[Count].MaxChannelTime =
						    pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].MaxChannelTime ? pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].MaxChannelTime : WFM_UMAC_PASSIVE_SCAN_MAX_CHANNEL_TIME;
						pScanParams->Channels[Count].TxPowerLevel = pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].TxPowerLevel ? pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].TxPowerLevel : PowerLevel;
					}	/*for(Count=0;  Count<pScanParams->NumOfChannels; Count++) */
				}
			}
		} else {
			/*If country string is available */
			if (pUmacInstance->gVars.p.InfoHost.ScanPhyBand == PHY_BAND_2G) {
				if (!pWfmListScan->NumberOfChannels_2_4Ghz) {
					pScanParams->NumOfChannels = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->gVars.p.InfoHost.ScanPhyBand].totalNumOfCh;
					pChannNo = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->gVars.p.InfoHost.ScanPhyBand].channels;
					pPowerLevel = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->gVars.p.InfoHost.ScanPhyBand].powerLevel;
					for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
						pScanParams->Channels[Count].ChannelNum = pChannNo[Count];	/* We don't require channel offset as it is already updated*//*+ ChannelOffset ; */
						pScanParams->Channels[Count].MinChannelTime = WFM_UMAC_ACTIVE_SCAN_MIN_CHANNEL_TIME;
						pScanParams->Channels[Count].MaxChannelTime = WFM_UMAC_ACTIVE_SCAN_MAX_CHANNEL_TIME;
						pScanParams->Channels[Count].TxPowerLevel = pPowerLevel[Count];
					}
				} else {
					/*
					   Take the channels from Scan structure
					   provided by driver. But check whether
					   the channel is supported by country
					   or not
					*/
					uint8 ChannelAvail = FALSE;
					sint32 PowerLevel;
					pScanParams->NumOfChannels = (uint8) pWfmListScan->NumberOfChannels_2_4Ghz;
					ChannelsNotSupported = 0;
					for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
						ChannelAvail = WFM_GetChannelIndexInTable(UmacHandle, pWfmListScan->Channels[Count].ChannelNum, PHY_BAND_2G, &PowerLevel);
						if (!ChannelAvail) {
							/*means channel is not available in the table */
							ChannelsNotSupported++;
							continue;
						}
						pScanParams->Channels[Count].ChannelNum = pWfmListScan->Channels[Count].ChannelNum;
						pScanParams->Channels[Count].MinChannelTime = pWfmListScan->Channels[Count].MinChannelTime ? pWfmListScan->Channels[Count].MinChannelTime : WFM_UMAC_PASSIVE_SCAN_MIN_CHANNEL_TIME;
						pScanParams->Channels[Count].MaxChannelTime = pWfmListScan->Channels[Count].MaxChannelTime ? pWfmListScan->Channels[Count].MaxChannelTime : WFM_UMAC_PASSIVE_SCAN_MAX_CHANNEL_TIME;
						pScanParams->Channels[Count].TxPowerLevel = PowerLevel;	/*Power level from the table */
					}
					/*Update the Total channels */
					pScanParams->NumOfChannels -= ChannelsNotSupported;
				}

			} else	{
				/*Scan Phy band is 5Ghz */
				if (!pWfmListScan->NumberOfChannels_5Ghz) {
					pScanParams->NumOfChannels = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->gVars.p.InfoHost.ScanPhyBand].totalNumOfCh;
					pChannNo = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->gVars.p.InfoHost.ScanPhyBand].channels;
					pPowerLevel = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->gVars.p.InfoHost.ScanPhyBand].powerLevel;
					for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
						pScanParams->Channels[Count].ChannelNum = pChannNo[Count];	/* We don't require channel offset as it is already updated*//*+ ChannelOffset ; */
						pScanParams->Channels[Count].MinChannelTime = WFM_UMAC_ACTIVE_SCAN_MIN_CHANNEL_TIME;
						pScanParams->Channels[Count].MaxChannelTime = WFM_UMAC_ACTIVE_SCAN_MAX_CHANNEL_TIME;
						pScanParams->Channels[Count].TxPowerLevel = pPowerLevel[Count];
					}	/*for(Count=0;  Count<pScanParams->NumOfChannels; Count++) */
				} else {
					/*Take the channels from Scan structure provided by driver.
					   But check whether the channel is supported by country or not */
					uint8 ChannelAvail = FALSE;
					sint32 PowerLevel;
					ChannelsNotSupported = 0;
					pScanParams->NumOfChannels = (uint8) pWfmListScan->NumberOfChannels_5Ghz;
					for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
						ChannelAvail = WFM_GetChannelIndexInTable(UmacHandle, pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].ChannelNum, PHY_BAND_5G, &PowerLevel);
						if (!ChannelAvail) {
							/*means channel is not available in the table */
							ChannelsNotSupported++;
							continue;
						}
						pScanParams->Channels[Count].ChannelNum = pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].ChannelNum;
						pScanParams->Channels[Count].MinChannelTime =
						    pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].MinChannelTime ? pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].MinChannelTime : WFM_UMAC_PASSIVE_SCAN_MIN_CHANNEL_TIME;
						pScanParams->Channels[Count].MaxChannelTime =
						    pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].MaxChannelTime ? pWfmListScan->Channels[pWfmListScan->NumberOfChannels_2_4Ghz + Count].MaxChannelTime : WFM_UMAC_PASSIVE_SCAN_MAX_CHANNEL_TIME;
						pScanParams->Channels[Count].TxPowerLevel = PowerLevel;	/*Power level from the table */
					}
					/*Update the Total channels */
					pScanParams->NumOfChannels -= ChannelsNotSupported;
				}
			}
		}



		if (pWfmListScan->NumOfSSIDs) {
			uint8 ii = 0;
			uint8 *SsidPtr = NULL;
			LMAC_SSIDS SSIDs[LMAC_MAX_SSIDS] = { {0}
			};
			pScanParams->NumOfSSIDs = pWfmListScan->NumOfSSIDs;
			SsidPtr = (uint8 *) ((uint8 *) &pScanParams->Channels[0] + (pScanParams->NumOfChannels * sizeof(LMAC_CHANNELS)));
			for (ii = 0; ii < pScanParams->NumOfSSIDs; ii++) {
				SSIDs[0].SSIDLength = pWfmListScan->Ssids[ii].SsidLength;
				OS_MemoryCopy((void *)&SSIDs[0].SSID[0], (void *)pWfmListScan->Ssids[ii].Ssid, pWfmListScan->Ssids[ii].SsidLength);

				OS_MemoryCopy((void *)SsidPtr, (void *)&SSIDs[0],
					      sizeof(LMAC_SSIDS)
				    );
				SsidPtr += sizeof(LMAC_SSIDS);
			}
		} else {
			/* Let it be a broadcast prob req now */
			pScanParams->NumOfSSIDs = 0;
		} /* if(pWfmListScan->SsidLength) */

		/*
		   Let LMAC to initiate a scan
		 */

		Result = UMAC_LL_INITIATE_SCAN(UmacHandle, pScanParams);

		if (Result != WFM_STATUS_SUCCESS) {
			/*reverting the scan state */
			pUmacInstance->UmacScanState = UMAC_READY_TO_SCAN;

			WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pScanTxDesc);

			UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pUmacInstance->gVars.pScanTxDesc);

			pUmacInstance->gVars.pScanTxDesc = NULL;

		}		/*if(Result != WFM_STATUS_SUCCESS) */

	} else {
		Result = WFM_STATUS_REQ_REJECTED;
		LOG_EVENT(DBG_WFM_UMAC, "***ERROR: WFM_STATUS_REQ_REJECTED!\n\n");
	}

	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);

 exit_handler:

	return Result;

}				/*end UMAC_ProcStartScan() */

/******************************************************************************
 * NAME:	UMAC_ProcStopScan
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function stops an already progressing scan.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pMsg	- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStopScan(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_BSS_LIST_SCAN *pWfmListScan = NULL;
	/*/WFM_HI_MESSAGE    *hi_msg       = NULL ; */
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "UMAC_ProcStopScan()\n");

	OS_ASSERT(pMsg);
	CHECK_POINTER_IS_VALID(pMsg);

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	if (pUmacInstance->UmacScanState == UMAC_SCANNING) {

		pWfmListScan = (WFM_BSS_LIST_SCAN *) pMsg;

		/*
		   A value ZERO for this field means, its a stop scan
		   request.  We are not doing anything with this value
		   now, as we are considering a scan request the device
		   recives while in scanning state as scan stop request
		 */

		CHECK_POINTER_IS_VALID(pWfmListScan);

		if (!pWfmListScan->Flags)
			LOG_EVENT(DBG_WFM_UMAC, "Its a stop scan request\n");


		UMAC_LL_STOP_SCAN(UmacHandle);

	}			/*if (pUmacInstance->UmacScanState == UMAC_SCANNING) */

	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);

	return Result;
}				/*end UMAC_ProcStopScan() */

/******************************************************************************
 * NAME:	UMAC_ProcScanRsp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the prob responses we are getting as a result of the
 * scan. All the necessory IE's are processed here and the prob response will
 * be stored in the device cache, if there is space available, otherwise the
 * oldest entry in the device cache will be uploaded to host to make space for
 * the current element and will be stored.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pMsg	- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcScanRsp(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 status;
#if P2P_MINIAP_SUPPORT
	WFM_UMAC_PRB_RESP_FRAME_BODY *pPrbRespFrameBody = NULL;
	WFM_UMAC_MGMT_FRAME *umacMgmt = NULL;
	uint16 IELength;
	IeElement Ie;
	WPS_IeElement attrib;
#endif	/*P2P_MINIAP_SUPPORT */

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "UMAC_ProcScanRsp()\n");

	OS_ASSERT(pMsg);

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED)
	    || (pUmacInstance->UmacScanState == UMAC_SCANNING)
#if P2P_MINIAP_SUPPORT
	    || (pUmacInstance->UmacFindState == UMAC_FINDING)
#endif	/* P2P_MINIAP_SUPPORT */
	    ) {

		if (!pUmacInstance->IsInternalScan)
			UMAC_Send_Beacons_ProbRsp_To_UpperLayer(UmacHandle);


		status = WFM_UMAC_ProcessBeaconOrProbRspFrame(UmacHandle, pMsg, 0);

		if (WFM_BEACON_PROBRSP_INTERNAL_SCAN_GOT_RESULT == status) {
			/* We can stop the internal scan now and start the
			   joining procedure
			 */

			WFM_BSS_INFO *pBssElement = (WFM_BSS_INFO *) &pUmacInstance->bssInfo[0];

			WFM_BSS_LIST_SCAN *pbssListScan = (WFM_BSS_LIST_SCAN *) &pUmacInstance->InternalScanMsg[0];

			pbssListScan->Flags = 0;

			/* Schedule to join with the AP */
			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_JOIN, pBssElement);

		} else if (WFM_BEACON_PROBRSP_LIST_UPDATED == status)
			UMAC_Send_Beacons_ProbRsp_To_UpperLayer(UmacHandle);
	}
#if P2P_MINIAP_SUPPORT
	else if (pUmacInstance->UmacCurrentState == UMAC_GROUP_FORMING) {
		UMAC_RX_DESC *pRxDesc = (UMAC_RX_DESC *) pMsg;
		uint8 numSecDeviceTypes;

		umacMgmt = (WFM_UMAC_MGMT_FRAME *)
		    RX_DESC_GET_DOT11FRAME(pRxDesc);

		CHECK_POINTER_IS_VALID(umacMgmt);
		pPrbRespFrameBody = (WFM_UMAC_PRB_RESP_FRAME_BODY *) umacMgmt->FrameBody;
		CHECK_POINTER_IS_VALID(pPrbRespFrameBody);
		IELength = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDesc)
				     - (sizeof(WFM_UMAC_FRAME_HDR)
					+ sizeof(pPrbRespFrameBody->Capability)
					+ sizeof(pPrbRespFrameBody->BeaconInterval)
					+ sizeof(pPrbRespFrameBody->TimeStamp)
				     ));

		if ((WFM_UMAC_ProcIeElements(pPrbRespFrameBody->IEElements, IELength, UMAC_P2P_IE_ELEMENT_ID, &Ie) == TRUE)
		    && (Ie.size)) {
			if ((WFM_UMAC_ProcWPSIE(Ie.info, Ie.size, UMAC_P2P_IE_ATTRIB_DEVICE_INFO, &attrib) == TRUE)
			    && (attrib.size)) {
				uint8 *devName;
				numSecDeviceTypes = *(attrib.info + WFM_MAC_ADDRESS_SIZE + UMAC_P2P_ATTRIB_CONFIG_METHOD_LENGTH + UMAC_P2P_ATTRIB_PRIM_DEV_TYP_LENGTH);
				devName = attrib.info + WFM_MAC_ADDRESS_SIZE + UMAC_P2P_ATTRIB_CONFIG_METHOD_LENGTH + UMAC_P2P_ATTRIB_PRIM_DEV_TYP_LENGTH + numSecDeviceTypes * UMAC_P2P_ATTRIB_SEC_DEV_TYP_LENGTH;
				/* Byte ordering in device name is Big Endian */
				if (OS_MemoryEqual((uint8 *) devName, pUmacInstance->p2pStartGrpFormatn.devNameLen, (uint8 *) pUmacInstance->p2pStartGrpFormatn.devName, pUmacInstance->p2pStartGrpFormatn.devNameLen)) {
					D0_ADDR_COPY(pUmacInstance->RemoteMacAdd, (uint8 *) pMsg + sizeof(UMAC_RX_DESC) + 2 * WFM_MAC_ADDRESS_SIZE);
					/* We have found the P2P device to which we want to send the GO negotiation request */
					WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_GO_NEGO, NULL);
				}
			}

		}

		pUmacInstance->p2pGroupConfig.peerListenChnl = pRxDesc->ChannelNumber;
	}			/*else if(pUmacInstance->UmacCurrentState == UMAC_GROUP_FORMING) */
#endif	/* P2P_MINIAP_SUPPORT */
	else {
		/* Hard-coding to D11_RCODE_INVALID_PARAMS now. Needs to be modified later
		   to give a meaningful status. may be, REJECTED
		 */
		Result = WFM_STATUS_BAD_PARAM;
	}

	WFM_UMAC_DBG_REMOVE_RX_DESC(pMsg);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, (UMAC_RX_DESC *) pMsg);

	if (pUmacInstance->autoChanSel)
		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
	else
	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
	return Result;
}				/*end UMAC_ProcScanRsp() */

/******************************************************************************
 * NAME:	UMAC_ProcStartJoin
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes a connect request issued by the host. This function
 * is backing up some parameters recieved from the host for forming association
 * request frame.
 * Another important functionality done by this function is, conversion of
 * rates to a bit mask as supported by WSM specification.
 * This function selects the lowest rate available as the current Tx rate.
 * UMAC state will be changed to UMAC_JOINING upon successful execution of this
 * function
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pMsg	- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartJoin(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	JOIN_PARAMETERS *pJoinParams = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_BSS_INFO *pBssInfo;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	uint16 CipherCount;
	uint16 PwCipherSuitCount = 0;
	uint16 AkmCount;
	uint32 AuthMode;
	uint32 EncrMode;
	int i;
	uint8 *pCipher;
	uint8 *pPwCipherSuitList;
	uint8 *pAkm;
	uint8 ApCipherSupport = 0;
	uint8 ApPwAESSupport = FALSE;
	uint16 ApAuthSupport = 0;
	uint16 pmkidCount = 0;
	uint8 bssInfoRsnCap = 0;
	uint8 NumOfCh = 0;
	uint8 count = 0;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC, "UMAC_ProcStartJoin()\n");

	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pMsg);

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pBssInfo = (WFM_BSS_INFO *) pMsg;

	CHECK_POINTER_IS_VALID(pBssInfo);

	/*
	  This need to reset in Join beacuse during romaing UMAC is not
	  resetting instance parmaters
	*/
	pUmacInstance->IsFirmwareBssParamSet = FALSE;

	if (WFM_MAX_SSID_SIZE < pBssInfo->SsidLength) {
		/*
		  If SSID size is greater that 32 bytes, parameters are invalid
		*/
		Result = WFM_STATUS_BAD_PARAM;
		goto error_handler;

	}			/*if(WFM_MAX_SSID_SIZE < pBssInfo->SsidLength) */

	if (0 != pBssInfo->NumRates)
		UMAC_HandleRates(UmacHandle, pBssInfo);

	if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode) {
		if (IS_NLL_MAC_ADDRESS(pBssInfo->MacAddress)) {
			/* IBSS create */
			uint8 i;
			uint8 SupportedRateTbl[] = WFM_UMAC_80211_FORMAT_SUPPORTED_RATESET_ABG;

			UMAC_IBSS_GetRandomBSSID(pBssInfo->MacAddress);

			pBssInfo->Capabilities = WFM_UMAC_CAPABILITY_SHORT_PREAMBLE | WFM_UMAC_CAPABILITY_IBSS;

			OS_MemoryCopy(pUmacInstance->RemoteMacAdd, pBssInfo->MacAddress, WFM_MAC_ADDRESS_SIZE);
			pBssInfo->ChannelNumber = pUmacInstance->Channelnumber;
			pBssInfo->ATIMWindow = pUmacInstance->atimWinSize;
			pBssInfo->BeaconPeriod = pUmacInstance->beaconInterval;

			switch (pUmacInstance->networkTypeInUse) {
			case NETWORK_TYPE_11B:
				pBssInfo->NumRates = 4;
				for (i = RATE_INDEX_B_1M; i < pBssInfo->NumRates; i++)
					pBssInfo->SupportedRates[i] = SupportedRateTbl[i];

				pBssInfo->SupportedRates[RATE_INDEX_B_1M] = BASIC_RATE(pBssInfo->SupportedRates[RATE_INDEX_B_1M]);
				pBssInfo->SupportedRates[RATE_INDEX_B_2M] = BASIC_RATE(pBssInfo->SupportedRates[RATE_INDEX_B_2M]);
				break;

			case NETWORK_TYPE_11A:
				pBssInfo->NumRates = 8;
				for (i = 0; i < pBssInfo->NumRates; i++)
					pBssInfo->SupportedRates[i] = SupportedRateTbl[i + RATE_INDEX_A_6M];

				/* Making 6, 12 and 24 Mbps as basic rates */
				pBssInfo->SupportedRates[0] = BASIC_RATE(pBssInfo->SupportedRates[0]);
				pBssInfo->SupportedRates[2] = BASIC_RATE(pBssInfo->SupportedRates[2]);
				pBssInfo->SupportedRates[4] = BASIC_RATE(pBssInfo->SupportedRates[4]);
				break;

			case NETWORK_TYPE_11BG:
				pBssInfo->NumRates = 12;
				pBssInfo->Flags |= WFM_BSS_INFO_FLAGS_PROT_ENABLED;
				pSmgmtReq->operation.join_mib_params.UseNonProtection = TRUE;
				for (i = RATE_INDEX_B_1M; i < 4; i++)
					pBssInfo->SupportedRates[i] = SupportedRateTbl[i];

				for (; i < 12; i++) {
					/* Skipping ERP-PBCC rates */
					pBssInfo->SupportedRates[i] = SupportedRateTbl[i + 2];
				}

				/*
				  Making 1,2, 6, 12 and 24 Mbps as basic rates
				*/
				pBssInfo->SupportedRates[RATE_INDEX_B_1M] = BASIC_RATE(pBssInfo->SupportedRates[RATE_INDEX_B_1M]);
				pBssInfo->SupportedRates[RATE_INDEX_B_2M] = BASIC_RATE(pBssInfo->SupportedRates[RATE_INDEX_B_2M]);
				break;

			case NETWORK_TYPE_11BGN:
				pBssInfo->Flags |= WFM_BSS_INFO_FLAGS_PROT_ENABLED;
				pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_DSSS_CCK_RATES_SUPPORTED | WFM_ERP_OFDM_RATES_SUPPORTED | WFM_HT_OFDM_RATES_SUPPORTED;
				pUmacInstance->AllBasicSupportedRates_WithAP_Abgn = UMAC_RATE_1 | UMAC_RATE_2 | UMAC_RATE_6 | UMAC_RATE_12 | UMAC_RATE_24;
				break;

			case NETWORK_TYPE_11AN:
				pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED | WFM_HT_OFDM_RATES_SUPPORTED;
				pUmacInstance->AllBasicSupportedRates_WithAP_Abgn = UMAC_RATE_6 | UMAC_RATE_12 | UMAC_RATE_24;
				break;
			}
			UMAC_HandleRates(UmacHandle, pBssInfo);
		} else if (IS_NLL_MAC_ADDRESS(pUmacInstance->RemoteMacAdd)) {
			OS_MemoryCopy(pUmacInstance->RemoteMacAdd, pBssInfo->MacAddress, WFM_MAC_ADDRESS_SIZE);
			if (pUmacInstance->ErpDssCckRates) {
				if (pUmacInstance->ErpOfdmRates) {
					if (pUmacInstance->HtOfdmRates)
						pUmacInstance->networkTypeInUse = NETWORK_TYPE_11BGN;
					else
						pUmacInstance->networkTypeInUse = NETWORK_TYPE_11BG;

				} else
					pUmacInstance->networkTypeInUse = NETWORK_TYPE_11B;

			} else if (pUmacInstance->ErpOfdmRates) {
				if (pUmacInstance->HtOfdmRates)
					pUmacInstance->networkTypeInUse = NETWORK_TYPE_11AN;
				else
					pUmacInstance->networkTypeInUse = NETWORK_TYPE_11A;

			}
		}

		/* store as 802.11 frame endianness (little endian) */

		pUmacInstance->CapabilitiesReq = (pBssInfo->Capabilities & (WFM_UMAC_SUPPORTED_CAPABILITIES | WFM_UMAC_CAPABILITY_IBSS))
		    | WFM_UMAC_CAPABILITY_SHORT_PREAMBLE;

	}			/*if(WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode) */


	/*Channel Number, MAC Address and SSID should not be all zeros */
	if (IS_INVALID_JOIN_REQUEST(pBssInfo->ChannelNumber, &pBssInfo->MacAddress[0], pBssInfo->SsidLength)) {

		Result = WFM_STATUS_BAD_PARAM;
		goto error_handler;
	}

	LOG_DATA2(DBG_WFM_ASSOC, "Trying to Join: SSID=%s, Len=%d\n", pBssInfo->Ssid, pBssInfo->SsidLength);
	LOG_MACADDR(DBG_WFM_ASSOC, "WFM:BSSID of AP=", pBssInfo->MacAddress);

	if (pUmacInstance->UmacScanState == UMAC_SCANNING) {
		if (pUmacInstance->JoinPending == FALSE) {
			/* not already pending */
			pUmacInstance->JoinPending = TRUE;

			if (pUmacInstance->IsInternalScan) {
				/*
				  HOST Driver is issuing another Join, so if we
				  are doing an internal scan, we should stop it
				  and service the new join request ASAP.
				 */
				WFM_BSS_LIST_SCAN *pbssListScan = (WFM_BSS_LIST_SCAN *) &pUmacInstance->InternalScanMsg[0];

				pbssListScan->Flags = 0;

				/*
				  Forcing this flag to TRUE to block
				  CONNECT_FAILED event to be sent to HOST,
				  as if HOST is getting a CONNECT FAILED now,
				  it may think that the latest join request
				  is failed.
				 */
				pUmacInstance->IsInternalScanSuccess = TRUE;

				/*
				  Schedule a Stop Scan, as we got the result we
				  were looking for
				*/
				WFM_SCHEDULE_TO_SELF(
							UmacHandle,
							UMAC_STOP_SCAN,
							pbssListScan
							);
			}	/*if( pUmacInstance->IsInternalScan ) */

			Result = WFM_STATUS_PENDING;
			goto error_handler;
		} else {
			LOG_EVENT(DBG_WFM_ASSOC, "Join: Recd two join requests\n");
			/*
			   Got another Join request in between, Queue it for
			   processing later. Host should block unnecessory
			   connect requests from reaching UMAC.
			*/
			Result = WFM_STATUS_PENDING;
			goto error_handler;
		} /* if( pUmacInstance->JoinPending == FALSE ) */
	}			/*if( pUmacInstance->UmacScanState == UMAC_SCANNING ) */

	/*No Join pending as we are processing it now */
	pUmacInstance->JoinPending = FALSE;

	if (IS_ACTIVE_SCAN_REQUIRED_BEFORE_JOIN(pBssInfo->ChannelNumber, &pBssInfo->MacAddress[0], pBssInfo->SsidLength)
	    ) {
		/*
		   This means The host is trying to connect/join with an SSID
		   it is familier with. We have to do an active scan from here
		   to proceed with this request as we dont have the essentionl
		   parameters for this AP.
		 */
		WFM_BSS_LIST_SCAN *pbssListScan = (WFM_BSS_LIST_SCAN *) &pUmacInstance->InternalScanMsg[0];

		pUmacInstance->IsInternalScan = TRUE;

		pbssListScan->Flags = WFM_BSS_LIST_SCAN_2_4G_BAND | WFM_BSS_LIST_SCAN_5G_BAND;

		pbssListScan->NumOfSSIDs = 1;
		pbssListScan->Ssids[0].SsidLength = pBssInfo->SsidLength;
		OS_MemoryCopy((void *)&pbssListScan->Ssids[0].Ssid[0], (void *)&pBssInfo->Ssid[0], pBssInfo->SsidLength);
		pbssListScan->Flags |= pBssInfo->Flags;

		LOG_STRING(DBG_WFM_ASSOC, "SSID : ", pBssInfo->Ssid, pbssListScan->SsidLength, pbssListScan->SsidLength);

		/*
		   Scheduling an active  scan to self
		 */

		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_SCAN, pbssListScan);

		/*
		   As long as this connect request is concerned, UMAC accepted
		   it and started working on it, so no point in returning a
		   PENDING status here.
		 */
		Result = WFM_STATUS_SUCCESS;

		goto error_handler;

	}			/*IS_ACTIVE_SCAN_REQUIRED_BEFORE_JOIN() */

	if (UMAC_BLKLST_LISTED == UMAC_IsBlackListedPeer(UmacHandle, &pBssInfo->MacAddress[0]
	    )

	    ) {

		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_CONNECT_FAILED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);

		Result = WFM_STATUS_SUCCESS;

		goto error_handler;
	}

	if (((pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2)
	     || (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_PSK)
	     || (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT)
	     || (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT_PSK)
	     || (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_SH256)
	     || (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_SH256_PSK))
	    && (pBssInfo->Flags & WFM_BSS_INFO_FLAGS_SUPP_WPA2)) {
		PwCipherSuitCount = (pBssInfo->Wpa2ParamElement.ParamInfo[7] << 8)
		    | pBssInfo->Wpa2ParamElement.ParamInfo[6];
		pPwCipherSuitList = &pBssInfo->Wpa2ParamElement.ParamInfo[8];

		for (i = 0; i < PwCipherSuitCount; i++) {
			if (OS_MemoryEqual(pPwCipherSuitList, CIPHER_OUI_SIZE, (uint8 *) Wpa2AesCipherOui, CIPHER_OUI_SIZE)) {
				ApPwAESSupport = TRUE;
				break;
			}
			pPwCipherSuitList += CIPHER_OUI_SIZE;
		}
	}

	if (((pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA)
	     || (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_PSK)
	     || (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_NONE))
	    && (pBssInfo->Flags & WFM_BSS_INFO_FLAGS_SUPP_WPA)) {
		PwCipherSuitCount = (pBssInfo->WpaParamElement.ParamInfo[11] << 8)
			    | pBssInfo->WpaParamElement.ParamInfo[10];
		pPwCipherSuitList = &pBssInfo->WpaParamElement.ParamInfo[12];

		for (i = 0; i < PwCipherSuitCount; i++) {
			if (OS_MemoryEqual(pPwCipherSuitList, CIPHER_OUI_SIZE, (uint8 *) WpaAesCipherOui, CIPHER_OUI_SIZE)) {
				ApPwAESSupport = TRUE;
				break;
			}
			pPwCipherSuitList += CIPHER_OUI_SIZE;
		}
	}

	/* STA should work in legacy mode if only TKIP is supported on AP side. */
	if ((pBssInfo->Flags & (WFM_BSS_INFO_FLAGS_11N_CAPABLE | WFM_BSS_INFO_FLAGS_SUPP_11N_RATES))
	   && (((ApPwAESSupport) && (pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost & WFM_ENC_STATUS_AES_SUPPORTED))
	      || ((pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_OPEN)
		&& (((pBssInfo->Capabilities & WFM_UMAC_CAPABILITY_PRIVACY) != WFM_UMAC_CAPABILITY_PRIVACY))))
	) {
		pUmacInstance->Flags = (pUmacInstance->Flags | WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP);
	}

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));

		UMAC_Initialize_TxQueueParams_Rates_All(
				UmacHandle,
				pUmacInstance->AllSupportedRates_WithAP_Abgn
				);

		pSmgmtReq->HandleMgmtReq = 1;
		pSmgmtReq->Type = SMGMT_REQ_TYPE_JOIN_REQ;
		pSmgmtReq->ReqNum = 0;

		if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP) {
			if (pBssInfo->Flags & (WFM_BSS_INFO_FLAGS_11N_CAPABLE | WFM_BSS_INFO_FLAGS_SUPP_11N_RATES))
				pSmgmtReq->operation.join_mib_params.WriteBlkAckPolicy = 1;
		}

		if (pBssInfo->ChannelNumber > 14) {
			pSmgmtReq->operation.join_mib_params.SloteTime = WFM_SLOT_TIME_SHORT_SLOT;
		} else {
			/*
			   since ERP IE may not be present in the beacon
			   (very old AP), so first check if AP supports short
			   slot or not
			*/

			/*
			   non ERP stations not present, so use short slot of
			   9 us
			*/
			if (pBssInfo->Capabilities & WFM_UMAC_SUPPORTED_CAPABILITIES & WFM_UMAC_CAPABILITY_SHORT_SLOT_TIME)
				pSmgmtReq->operation.join_mib_params.SloteTime = WFM_SLOT_TIME_SHORT_SLOT;
			else
				pSmgmtReq->operation.join_mib_params.SloteTime = WFM_SLOT_TIME_LONG_SLOT;

		}		/*if(pBssInfo->Flags & WFM_BSS_INFO_FLAGS_NON_ERP_AP) */

		/*
		   call this before 11n stuff - mainly before calling
		   WFM_UMAC_Process_HtOperationIe()
		*/
		if (pBssInfo->Flags & WFM_BSS_INFO_FLAGS_PROT_ENABLED)
			pSmgmtReq->operation.join_mib_params.UseNonProtection = TRUE;
		else
			pSmgmtReq->operation.join_mib_params.UseNonProtection = FALSE;

		/*Update the Green field supported by AP or not */
		if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP) {
			if (pBssInfo->HtCapabilities.CapabilitiesInfo & WFM_HT_CAP_GREEN_FIELD & WFM_UMAC_HT_CAP_OURS) {
				pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_HT_GREENFIELD_MODE_SUPPORTED_BY_AP;
				pUmacInstance->CheckGFProblem = 1;
			}
		}

		WFM_UMAC_Process_HtOperationIe_Param2(UmacHandle, &(pBssInfo->HtOperationIe.HTInfoParam2), &pSmgmtReq->operation.join_mib_params.HtOperationIe);
		WFM_UMAC_Process_HtOperationIe_Param3(UmacHandle, &(pBssInfo->HtOperationIe.HTInfoParam3), &pSmgmtReq->operation.join_mib_params.HtOperationIe);

		pUmacInstance->Channelnumber = (uint16) pBssInfo->ChannelNumber;

		if (pUmacInstance->Channelnumber > 14) {
			/*Its 5G */
			pUmacInstance->Band = PHY_BAND_5G;
		} else {
			/*Coding 2.4 G */
			pUmacInstance->Band = PHY_BAND_2G;
		}

		/* Get Power level for Received country Info */
		if (pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[pUmacInstance->Band] == TRUE) {
			NumOfCh = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->Band].totalNumOfCh;
			for (count = 0; count < NumOfCh; count++) {
				if (pUmacInstance->Channelnumber == pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->Band].channels[count]) {
					pUmacInstance->PowerLevel = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->Band].powerLevel[count] * WFM_POWER_LEVEL_UNIT;
					break;
				}
			}
		} else {
			if (pUmacInstance->TxPowerLevel)
				pUmacInstance->PowerLevel = pUmacInstance->TxPowerLevel;
			else
				pUmacInstance->PowerLevel = WFM_UMAC_DEFAULT_POWER_LEVEL;

		}		/*if(pUmacInstance->gVars.p.CountryInfoAvl == TRUE) */

		pSmgmtReq->operation.join_mib_params.powerLevel = GET_MIN(pUmacInstance->PowerLevel, WFM_UMAC_MAX_DEVICE_POWER);

		if (pSmgmtReq->operation.join_mib_params.powerLevel < WFM_UMAC_MIN_DEVICE_POWER)
			pSmgmtReq->operation.join_mib_params.powerLevel = WFM_UMAC_MIN_DEVICE_POWER;

		pUmacInstance->SsidLength = pBssInfo->SsidLength;

		if (pUmacInstance->SsidLength) {
			OS_MemoryCopy(
				(void *)&pUmacInstance->Ssid[0],
				(void *)&pBssInfo->Ssid[0],
				pBssInfo->SsidLength
				);
		}


		pSmgmtReq->operation.join_mib_params.hostMsgTypeFilter = pUmacInstance->gVars.p.hostMsgTypeFilter;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;
	}			/*if(!pSmgmtReq->HandleMgmtReq) */

	pSmgmtReq->HandleMgmtReq = 0;

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_ProcStartJoin(): Working\n");

	WFM_UMAC_DBG_ASSERT_IF_OUTSTANDING_TX_DESC(pUmacInstance);

	if (pUmacInstance->gVars.p.PtaListenInterval.PtaListenInterval) {
		uint32 PtaListenInterval = pUmacInstance->gVars.p.PtaListenInterval.PtaListenInterval;

		if (((PtaListenInterval * 100) % pBssInfo->BeaconPeriod) == 0)
			pUmacInstance->PtaInterval = (uint8) ((PtaListenInterval * 100) / pBssInfo->BeaconPeriod);
		else
			pUmacInstance->PtaInterval = (uint8) ((PtaListenInterval * 100) / pBssInfo->BeaconPeriod + 1);

	} else
		pUmacInstance->PtaInterval = 0;

	/*
	  Store DTIM period this will be used while configuring the device for
	  PSM
	*/
	if (pBssInfo->BeaconPeriod <= 300) {
		if (pBssInfo->DTIMPeriod) {
			pUmacInstance->DTIMPeriod = pBssInfo->DTIMPeriod;
		} else {
			if (pUmacInstance->PtaInterval)
				pUmacInstance->DTIMPeriod = pUmacInstance->PtaInterval;
			else
				pUmacInstance->DTIMPeriod = DEFAULT_DTIM_PERIOD;

			pUmacInstance->DTIMPeriod |= WFM_DTIM_PERIOD_FLAG_UNKNOWN;
		}
	} else
		pUmacInstance->DTIMPeriod = DTIM_PERIOD_1;

	/*Store Infra mode if it is diffrent in BSS info */
	if (pUmacInstance->dot11InfraStructureMode != pBssInfo->InfrastructureMode)
		pUmacInstance->dot11InfraStructureMode = pBssInfo->InfrastructureMode;

	/*
	   Finding the capabiities which are common for both the devices. This subset
	   will be used for creating Association Request.
	 */

	/* store as 802.11 frame endianness (little endian) */
	/* short slot is our capability, so it needs to be present always */
	/* short preamble is our capability, so it needs to be present always */
	pUmacInstance->CapabilitiesReq = (pBssInfo->Capabilities & WFM_UMAC_SUPPORTED_CAPABILITIES)
		|
	    (WFM_UMAC_CAPABILITY_SHORT_SLOT_TIME & WFM_UMAC_SUPPORTED_CAPABILITIES)
	    |
	    (WFM_UMAC_CAPABILITY_SHORT_PREAMBLE & WFM_UMAC_SUPPORTED_CAPABILITIES);

#if DOT11K_SUPPORT
	/*
	  Check the RRM Capabilities of station and update
	  pUmacInstance->RRMCapability
	*/
	if ((pBssInfo->Flags & WFM_BSS_INFO_FLAGS_RRM_MEASUREMENT_CAPABLE) && (pUmacInstance->gVars.p.InfoHost.MeasurementCapSupported == TRUE)) {
		pUmacInstance->CapabilitiesReq |= WFM_UMAC_CAPABILITY_RADIO_MEASUREMENT;
		UMAC_ProcessRRMCapabilities(UmacHandle, pBssInfo->RRMMeasurementCapabilities);
		pUmacInstance->dot11k_Meas_Params.RRMCapability &= pBssInfo->RRMMeasurementCapabilities;
		pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_RRM_MEASUREMENT_SUPPORTED_BY_AP;
	}
#endif	/* DOT11K_SUPPORT */

	if (pBssInfo->Flags & WFM_BSS_INFO_FLAGS_BSS_WMM_CAP) {
		pUmacInstance->Flags = (pUmacInstance->Flags | WFM_UMAC_INSTANCE_FLAGS_WMM_SUPPORTED_BY_AP);

		OS_MemoryCopy(
			(void *)&pUmacInstance->gVars.WmmParamElement,
			(void *)&pBssInfo->WmmParamElement,
			sizeof(WFM_WMM_PARAM_ELEMENT)
		    );

		if (pBssInfo->WmmParamElement.QoSInfo & D11_UAPSD_SUPPORTED) {
			pUmacInstance->gVars.WmmParamElement.QoSInfo = ((pUmacInstance->gVars.p.setUapsdInfo.uapsdFlags & WSM_QOS_INFO_STA_UAPSD_AC_BK) << 2)
			    | ((pUmacInstance->gVars.p.setUapsdInfo.uapsdFlags & WSM_QOS_INFO_STA_UAPSD_AC_BE) << 2)
			    | ((pUmacInstance->gVars.p.setUapsdInfo.uapsdFlags & WSM_QOS_INFO_STA_UAPSD_AC_VI) >> 1)
			    | ((pUmacInstance->gVars.p.setUapsdInfo.uapsdFlags & WSM_QOS_INFO_STA_UAPSD_AC_VO) >> 3);
			/* Set SP to 2 as WMM AP may deliver a maximum of 2 buffered frames */
			pUmacInstance->gVars.WmmParamElement.QoSInfo |= 0x01 << 5;
		} else {
			/* UAPSD is not supported by AP */
			pUmacInstance->gVars.WmmParamElement.QoSInfo = 0;
		}
	}			/*if( pBssInfo->Flags & WFM_BSS_INFO_FLAGS_BSS_WMM_CAP  ) */

	AuthMode = pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost;
	EncrMode = pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost;

	if ((AuthMode == WFM_AUTH_MODE_WPA)
	    || (AuthMode == WFM_AUTH_MODE_WPA_PSK)) {
		if (pBssInfo->Flags & WFM_BSS_INFO_FLAGS_SUPP_WPA) {
			ApCipherSupport = 0;
			ApAuthSupport = 0;

			pUmacInstance->Flags = (pUmacInstance->Flags | WFM_UMAC_INSTANCE_FLAGS_WPA_IE_SUPPORTED_BY_AP);

			CipherCount = (pBssInfo->WpaParamElement.ParamInfo[11] << 8)
			    | pBssInfo->WpaParamElement.ParamInfo[10];

			pCipher = &pBssInfo->WpaParamElement.ParamInfo[12];

			for (i = 0; i < CipherCount; i++) {
				if (OS_MemoryEqual((uint8 *) WpaAesCipherOui, CIPHER_OUI_SIZE, pCipher, CIPHER_OUI_SIZE))
					ApCipherSupport |= WFM_ENC_STATUS_AES_SUPPORTED;
				else if (OS_MemoryEqual((uint8 *) WpaTkipCipherOui, CIPHER_OUI_SIZE, pCipher, CIPHER_OUI_SIZE))
					ApCipherSupport |= WFM_ENC_STATUS_TKIP_SUPPORTED;

				pCipher += CIPHER_OUI_SIZE;
			}

			AkmCount = (pBssInfo->WpaParamElement.ParamInfo[13 + (CipherCount * CIPHER_OUI_SIZE)] << 8)
			    | pBssInfo->WpaParamElement.ParamInfo[12 + (CipherCount * CIPHER_OUI_SIZE)];

			pAkm = &pBssInfo->WpaParamElement.ParamInfo[14 + (CipherCount * CIPHER_OUI_SIZE)];

			for (i = 0; i < AkmCount; i++) {
				if (OS_MemoryEqual((uint8 *) Wpa8021xAkmOui, AKM_OUI_SIZE, pAkm, AKM_OUI_SIZE))
					ApAuthSupport |= 0x1;
				else if (OS_MemoryEqual((uint8 *) WpaPskAkmOui, AKM_OUI_SIZE, pAkm, AKM_OUI_SIZE))
					ApAuthSupport |= 0x2;

				pAkm += AKM_OUI_SIZE;
			}

			/* 10 -> OUI + Version + Multicast cipher OUI */
			OS_MemoryCopy(
				&pUmacInstance->gVars.WpaParamElement.ParamInfo[0],
				&pBssInfo->WpaParamElement.ParamInfo[0],
				10
			    );
			pUmacInstance->gVars.WpaParamElement.ParamInfo[10] = 1;
			pUmacInstance->gVars.WpaParamElement.ParamInfo[11] = 0;

			if ((EncrMode & WFM_ENC_STATUS_AES_SUPPORTED)
			    && (ApCipherSupport & WFM_ENC_STATUS_AES_SUPPORTED)) {
				OS_MemoryCopy(&pUmacInstance->gVars.WpaParamElement.ParamInfo[12], (void *)WpaAesCipherOui, CIPHER_OUI_SIZE);
				pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_AES_ENC_SUPPORTED;
			} else if ((EncrMode & WFM_ENC_STATUS_TKIP_SUPPORTED)
				   && (ApCipherSupport & WFM_ENC_STATUS_TKIP_SUPPORTED)) {
				OS_MemoryCopy(&pUmacInstance->gVars.WpaParamElement.ParamInfo[12], (void *)WpaTkipCipherOui, CIPHER_OUI_SIZE);
				pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_TKIP_ENC_SUPPORTED;
			} else {
				/* Handle error case */
				goto connect_failure;
			}

			pUmacInstance->gVars.WpaParamElement.ParamInfo[16] = 1;
			pUmacInstance->gVars.WpaParamElement.ParamInfo[17] = 0;

			if ((AuthMode == WFM_AUTH_MODE_WPA)
			    && (ApAuthSupport & 0x01)) {
				OS_MemoryCopy(&pUmacInstance->gVars.WpaParamElement.ParamInfo[18], (void *)Wpa8021xAkmOui, AKM_OUI_SIZE);
			} else if ((AuthMode == WFM_AUTH_MODE_WPA_PSK)
				   && (ApAuthSupport & 0x02)) {
				OS_MemoryCopy(&pUmacInstance->gVars.WpaParamElement.ParamInfo[18], (void *)WpaPskAkmOui, AKM_OUI_SIZE);
			} else {
				/* Handle error case */
				goto connect_failure;
			}
			OS_MemoryCopy(
				&pUmacInstance->gVars.WpaParamElement.ParamInfo[22],
				&pBssInfo->WpaParamElement.ParamInfo[14 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE)],
				(pBssInfo->WpaParamElement.Length - 14 - (CipherCount * CIPHER_OUI_SIZE) - (AkmCount * AKM_OUI_SIZE))
			    );

			pUmacInstance->gVars.WpaParamElement.Length = pBssInfo->WpaParamElement.Length - (((CipherCount - 1) * CIPHER_OUI_SIZE) + ((AkmCount - 1) * AKM_OUI_SIZE));
		} else {
			/* Handle error case */
			goto connect_failure;
		}		/*if( pBssInfo->Flags & WFM_BSS_INFO_FLAGS_SUPP_WPA ) */
	}			/*if( AuthMode == WPA/WPA-PSK ) */

#if FT_SUPPORT
	pUmacInstance->Flags &= ~WFM_UMAC_INSTANCE_FLAGS_FT_SUPPORTED_BY_AP;
#endif	/* FT_SUPPORT */

	if ((AuthMode == WFM_AUTH_MODE_WPA_2)
	    || (AuthMode == WFM_AUTH_MODE_WPA_2_PSK)
#if FT_SUPPORT
	    || (AuthMode == WFM_AUTH_MODE_WPA_2_FT)
	    || (AuthMode == WFM_AUTH_MODE_WPA_2_FT_PSK)
#endif	/* FT_SUPPORT */
#if MGMT_FRAME_PROTECTION
	    || (AuthMode == WFM_AUTH_MODE_WPA_2_SH256)
	    || (AuthMode == WFM_AUTH_MODE_WPA_2_SH256_PSK)
#endif				/*MGMT_FRAME_PROTECTION */
	    ) {
		if (pBssInfo->Flags & WFM_BSS_INFO_FLAGS_SUPP_WPA2) {
			ApCipherSupport = 0;
			ApAuthSupport = 0;

			pUmacInstance->Flags = (pUmacInstance->Flags | WFM_UMAC_INSTANCE_FLAGS_RSN_IE_SUPPORTED_BY_AP);

			CipherCount = (pBssInfo->Wpa2ParamElement.ParamInfo[7] << 8)
			    | pBssInfo->Wpa2ParamElement.ParamInfo[6];

			pCipher = &pBssInfo->Wpa2ParamElement.ParamInfo[8];

#if MGMT_FRAME_PROTECTION
			AkmCount = (pBssInfo->Wpa2ParamElement.ParamInfo[9 + (CipherCount * CIPHER_OUI_SIZE)] << 8)
			    | pBssInfo->Wpa2ParamElement.ParamInfo[8 + (CipherCount * CIPHER_OUI_SIZE)];

			bssInfoRsnCap = pBssInfo->Wpa2ParamElement.ParamInfo[10 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE)];
			/*
			  Get the Managment frame protection policy from
			  BSS Info
			*/
			pUmacInstance->bssInfoMfpPolicy = (bssInfoRsnCap & 0xC0) >> 6;

			if ((pUmacInstance->bssInfoMfpPolicy == UMAC_MFP_REQUIRED)
			    || ((pUmacInstance->hostMfpPolicy & 0x0003) == UMAC_MFP_REQUIRED)) {
				/*Incorrect Managment Frame protection Policy */
				/*Send Connect Failed event to host */
				goto connect_failure;
			} else if ((pUmacInstance->bssInfoMfpPolicy == UMAC_MFP_DISABLE)
				   && ((pUmacInstance->hostMfpPolicy & 0x0003) == UMAC_MFP_CAPABLE_REQUIRED)) {
				/*
				  AP is not capable for Managment Frame
				  protection, but our STA must require
				*/
				/*Send Connect Failed event to host */
				goto connect_failure;
			} else if ((pUmacInstance->bssInfoMfpPolicy == UMAC_MFP_CAPABLE_REQUIRED)
				   && ((pUmacInstance->hostMfpPolicy & 0x0003) == UMAC_MFP_DISABLE)) {
				/*
				  AP must require Managment Frame protection,
				  but our STA is not capable
				*/
				/*Send Connect Failed event to host */
				goto connect_failure;
			}
#endif				/*MGMT_FRAME_PROTECTION */

			for (i = 0; i < CipherCount; i++) {
				if (OS_MemoryEqual(pCipher, CIPHER_OUI_SIZE, (uint8 *) Wpa2AesCipherOui, CIPHER_OUI_SIZE))
					ApCipherSupport |= WFM_ENC_STATUS_AES_SUPPORTED;
				else if (OS_MemoryEqual(pCipher, CIPHER_OUI_SIZE, (uint8 *) Wpa2TkipCipherOui, CIPHER_OUI_SIZE))
					ApCipherSupport |= WFM_ENC_STATUS_TKIP_SUPPORTED;

				pCipher += CIPHER_OUI_SIZE;
			}

#if !MGMT_FRAME_PROTECTION
			AkmCount = (pBssInfo->Wpa2ParamElement.ParamInfo[9 + (CipherCount * CIPHER_OUI_SIZE)] << 8)
			    | pBssInfo->Wpa2ParamElement.ParamInfo[8 + (CipherCount * CIPHER_OUI_SIZE)];
#endif				/*!MGMT_FRAME_PROTECTION */
			pAkm = &pBssInfo->Wpa2ParamElement.ParamInfo[10 + (CipherCount * CIPHER_OUI_SIZE)];

			for (i = 0; i < AkmCount; i++) {
				if (OS_MemoryEqual((uint8 *) Wpa28021xAkmOui, AKM_OUI_SIZE, pAkm, AKM_OUI_SIZE))
					ApAuthSupport |= 0x04;
				else if (OS_MemoryEqual((uint8 *) Wpa2PskAkmOui, AKM_OUI_SIZE, pAkm, AKM_OUI_SIZE))
					ApAuthSupport |= 0x08;
#if FT_SUPPORT
				else if (OS_MemoryEqual((uint8 *) Wpa2FtAkmOui, AKM_OUI_SIZE, pAkm, AKM_OUI_SIZE)) {
					ApAuthSupport |= 0x10;
				} else if (OS_MemoryEqual((uint8 *) Wpa2FtPskAkmOui, AKM_OUI_SIZE, pAkm, AKM_OUI_SIZE)) {
					ApAuthSupport |= 0x20;
				}
#endif	/* FT_SUPPORT */
#if MGMT_FRAME_PROTECTION
				else if (OS_MemoryEqual((uint8 *) Wpa28021xSh256AkmOui, AKM_OUI_SIZE, pAkm, AKM_OUI_SIZE)) {
					ApAuthSupport |= 0x40;
				} else if (OS_MemoryEqual((uint8 *) Wpa2PskSh256AkmOui, AKM_OUI_SIZE, pAkm, AKM_OUI_SIZE)) {
					ApAuthSupport |= 0x80;
				}
#endif				/*MGMT_FRAME_PROTECTION */
				pAkm += AKM_OUI_SIZE;
			}

			OS_MemoryCopy(
				&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[0],
				&pBssInfo->Wpa2ParamElement.ParamInfo[0],
				6
				);
			pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[6] = 1;
			pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[7] = 0;

			if ((EncrMode & WFM_ENC_STATUS_AES_SUPPORTED)
			    && (ApCipherSupport & WFM_ENC_STATUS_AES_SUPPORTED)) {
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[8],
					(void *)&Wpa2AesCipherOui[0],
					CIPHER_OUI_SIZE
					);
				pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_AES_ENC_SUPPORTED;

#if MGMT_FRAME_PROTECTION
				/*Update MFP Policy set by host */
				pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[18] = pBssInfo->Wpa2ParamElement.ParamInfo[10 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE)] & 0x3F;
				pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[18] |= (uint8) ((pUmacInstance->hostMfpPolicy & 0x0003) << 6);
				pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[19] = pBssInfo->Wpa2ParamElement.ParamInfo[10 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE) + 1];
#endif				/*MGMT_FRAME_PROTECTION */

			} else if ((EncrMode & WFM_ENC_STATUS_TKIP_SUPPORTED)
				   && (ApCipherSupport & WFM_ENC_STATUS_TKIP_SUPPORTED)) {
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[8],
					(void *)&Wpa2TkipCipherOui[0],
					CIPHER_OUI_SIZE
					);
				pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_TKIP_ENC_SUPPORTED;
#if MGMT_FRAME_PROTECTION
				/*Update MFP Policy set by host */
				pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[18] = pBssInfo->Wpa2ParamElement.ParamInfo[18];
				pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[19] = pBssInfo->Wpa2ParamElement.ParamInfo[19];
#endif				/*MGMT_FRAME_PROTECTION */
			} else {
				/* Handle error case */
				goto connect_failure;
			}

			pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[12] = 1;
			pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[13] = 0;

			if ((AuthMode == WFM_AUTH_MODE_WPA_2)
			    && (ApAuthSupport & 0x04)) {
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[14],
					(void *)Wpa28021xAkmOui,
					AKM_OUI_SIZE);
			} else if ((AuthMode == WFM_AUTH_MODE_WPA_2_PSK)
				   && (ApAuthSupport & 0x08)) {
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[14],
					(void *)Wpa2PskAkmOui,
					AKM_OUI_SIZE);
			}
#if FT_SUPPORT
			else if ((AuthMode == WFM_AUTH_MODE_WPA_2_FT_PSK)
				 && (ApAuthSupport & 0x20)) {
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[14],
					(void *)Wpa2FtPskAkmOui,
					AKM_OUI_SIZE
					);
				pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_FT_SUPPORTED_BY_AP;
			} else if ((AuthMode == WFM_AUTH_MODE_WPA_2_FT)
				   && (ApAuthSupport & 0x10)) {
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[14],
					(void *)Wpa2FtAkmOui,
					AKM_OUI_SIZE
					);
				pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_FT_SUPPORTED_BY_AP;
			}
#endif	/* FT_SUPPORT */
#if MGMT_FRAME_PROTECTION
			else if ((AuthMode == WFM_AUTH_MODE_WPA_2_SH256)
				 && (ApAuthSupport & 0x40)) {
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[14],
					(void *)Wpa28021xSh256AkmOui,
					AKM_OUI_SIZE
					);
			} else if ((AuthMode == WFM_AUTH_MODE_WPA_2_SH256_PSK)
				   && (ApAuthSupport & 0x80)) {
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[14],
					(void *)Wpa2PskSh256AkmOui,
					AKM_OUI_SIZE
					);
			}
#endif				/*MGMT_FRAME_PROTECTION */
			else {
				/* Handle error */
				goto connect_failure;
			}

#if MGMT_FRAME_PROTECTION
			if (((pUmacInstance->hostMfpPolicy & 0x0003) > UMAC_MFP_REQUIRED)
			    && (EncrMode & WFM_ENC_STATUS_AES_SUPPORTED)) {
				pmkidCount = (uint16) pBssInfo->Wpa2ParamElement.ParamInfo[12 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE)];
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[20],
					&pBssInfo->Wpa2ParamElement.ParamInfo[12 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE)],
					(pmkidCount + 2)
					);
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[22 + pmkidCount],
					(void *)Wpa2GroupMgmtCipherOui,
					CIPHER_OUI_SIZE
					);
				pUmacInstance->gVars.Wpa2ParamElement.Length = 14 + CIPHER_OUI_SIZE + AKM_OUI_SIZE + pmkidCount + CIPHER_OUI_SIZE;
			} else if (((pUmacInstance->bssInfoMfpPolicy & 0x0003) > UMAC_MFP_REQUIRED)
				   && ((pUmacInstance->hostMfpPolicy & 0x0003) == UMAC_MFP_DISABLE)
				   && (EncrMode & WFM_ENC_STATUS_AES_SUPPORTED)) {
				pmkidCount = (uint16) pBssInfo->Wpa2ParamElement.ParamInfo[12 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE)];
				if (pmkidCount) {
					OS_MemoryCopy(
						&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[20],
						&pBssInfo->Wpa2ParamElement.ParamInfo[12 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE)],
						(pmkidCount + 2)
					    );
					pUmacInstance->gVars.Wpa2ParamElement.Length = 12 + CIPHER_OUI_SIZE + AKM_OUI_SIZE + pmkidCount;
				} else
					pUmacInstance->gVars.Wpa2ParamElement.Length = 12 + CIPHER_OUI_SIZE + AKM_OUI_SIZE;
			} else {
				OS_MemoryCopy(
				    &pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[20],
				    &pBssInfo->Wpa2ParamElement.ParamInfo[12 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE)],
				    (pBssInfo->Wpa2ParamElement.Length - 12 - (CipherCount * CIPHER_OUI_SIZE) - (AkmCount * AKM_OUI_SIZE))
				    );
#else	/* MGMT_FRAME_PROTECTION */
				OS_MemoryCopy(
					&pUmacInstance->gVars.Wpa2ParamElement.ParamInfo[18],
					&pBssInfo->Wpa2ParamElement.ParamInfo[10 + (CipherCount * CIPHER_OUI_SIZE) + (AkmCount * AKM_OUI_SIZE)],
					(pBssInfo->Wpa2ParamElement.Length - 10 - (CipherCount * CIPHER_OUI_SIZE) - (AkmCount * AKM_OUI_SIZE))
			    );
#endif				/*MGMT_FRAME_PROTECTION */
				pUmacInstance->gVars.Wpa2ParamElement.Length = pBssInfo->Wpa2ParamElement.Length - (((CipherCount - 1) * CIPHER_OUI_SIZE) + ((AkmCount - 1) * AKM_OUI_SIZE));
#if MGMT_FRAME_PROTECTION
			}
#endif				/*MGMT_FRAME_PROTECTION */
		} else {
			/* Handle error case */
			goto connect_failure;
		}
	}				/*if(pBssInfo->Flags & WFM_BSS_INFO_FLAGS_SUPP_WPA2) */

#if WAPI_SUPPORT
	if (AuthMode == WFM_AUTH_MODE_WAPI) {
		if (pBssInfo->Flags & WFM_BSS_INFO_FLAGS_SUPP_WAPI) {

			pUmacInstance->Flags = (pUmacInstance->Flags | WFM_UMAC_INSTANCE_FLAGS_WAPI_IE_SUPPORTED_BY_AP);

			OS_MemoryCopy(
				&pUmacInstance->gVars.WapiParamElement,
				&pBssInfo->WapiParamElement,
				sizeof(WFM_WAPI_PARAM_ELEMENT)
				);
		} else {
			/* Handle error case */
			goto connect_failure;
		} /* if( pBssInfo->Flags & WFM_BSS_INFO_FLAGS_SUPP_WAPI ) */
		pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_WAPI_ENC_SUPPORTED;
	} /* if (AuthMode == WFM_AUTH_MODE_WAPI) */
#endif				/*WAPI_SUPPORT */

	/* Update P2P Power Save mode */
	pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow = pBssInfo->noaAttrib.oppsCtWindow;
	pUmacInstance->gVars.p.p2pPsMode.count = pBssInfo->noaAttrib.count;
	pUmacInstance->gVars.p.p2pPsMode.DtimCount = 0;
	pUmacInstance->gVars.p.p2pPsMode.duration = pBssInfo->noaAttrib.duration;
	pUmacInstance->gVars.p.p2pPsMode.interval = pBssInfo->noaAttrib.interval;
	pUmacInstance->gVars.p.p2pPsMode.startTime = pBssInfo->noaAttrib.startTime;
	pUmacInstance->gVars.p.p2pPsMode.reserved = 0;

	OS_ASSERT(!pUmacInstance->gVars.pJoinTxDesc);

	pUmacInstance->gVars.pJoinTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pJoinTxDesc);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pUmacInstance->gVars.pJoinTxDesc, UMAC_JOIN_REQ);

	WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

	pJoinParams = (JOIN_PARAMETERS *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pJoinTxDesc);
	CHECK_POINTER_IS_VALID(pJoinParams);

	pJoinParams->Mode = (uint8) pBssInfo->InfrastructureMode;

	pJoinParams->ChannelNumber = (uint16) pBssInfo->ChannelNumber;

	if (pJoinParams->ChannelNumber > 14) {
		/*Its 5G */
		pJoinParams->Band = PHY_BAND_5G;
	} else {
		/*Coding 2.4 G */
		pJoinParams->Band = PHY_BAND_2G;
	}

	pUmacInstance->Band = pJoinParams->Band;

	pUmacInstance->Channelnumber = pJoinParams->ChannelNumber;

	pJoinParams->DTIMPeriod = pUmacInstance->DTIMPeriod & ~WFM_DTIM_PERIOD_FLAG_UNKNOWN;

	pJoinParams->Flags = 0;

	if (pUmacInstance->gVars.p.useP2P == UMAC_P2P_ENABLED)
		pJoinParams->Flags |= (1<<1);

	/*
	   Setting the remote devices BSSID to UMAC Instance
	 */

	if (WFM_802_INFRASTRUCTURE == pUmacInstance->dot11InfraStructureMode) {
		OS_MemoryCopy(
			pUmacInstance->CurrentAPAdd,
			pUmacInstance->RemoteMacAdd,
			WFM_MAC_ADDRESS_SIZE
			);
		OS_MemoryCopy(
			pUmacInstance->RemoteMacAdd,
			(void *)pBssInfo->MacAddress,
			WFM_MAC_ADDRESS_SIZE
			);
	}

	/*
	   Setting the remote devices SSID to UMAC Instance
	 */

	if (pUmacInstance->SsidLength) {
		pJoinParams->SSIDLength = pBssInfo->SsidLength;
		OS_MemoryCopy(
			(void *)&pJoinParams->SSID[0],
			(void *)&pBssInfo->Ssid[0],
			pBssInfo->SsidLength
			);
	}				/*if ( pUmacInstance->SsidLength ) */

	OS_MemoryCopy(
		(void *)pJoinParams->BSSID,
		(void *)pBssInfo->MacAddress,
		WFM_MAC_ADDRESS_SIZE
		);

	/*
	   In WFM HI messages ATIMWindow is of type uint32
	   but in JOIN Paramters, its just uint16
	 */
	pJoinParams->AtimWindow = (uint16) pBssInfo->ATIMWindow;

		if (pBssInfo->ChannelNumber <= 14) {
			if (pBssInfo->Capabilities & WFM_UMAC_SUPPORTED_CAPABILITIES & WFM_UMAC_CAPABILITY_SHORT_SLOT_TIME) {
				/*AP supports short slot */
				pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_SHORT_SLOT;
			} else {
				/*AP does not support short slot */
				pUmacInstance->Flags &= (~WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_SHORT_SLOT);
		}
	}

	/*if AP supports short preamble and we also support short preamble */
	if ((pBssInfo->Flags & WFM_BSS_INFO_FLAGS_SUPP_SHORT_PREAMBLE)
			&&
			(WFM_UMAC_CAPABILITY_SHORT_PREAMBLE & WFM_UMAC_SUPPORTED_CAPABILITIES)
	   ) {
		pJoinParams->PreambleType = WSM_PREAMBLE_SHORT;
		pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_BARKER_SHORT_PREAMBLE;
	} else {
		pJoinParams->PreambleType = WSM_PREAMBLE_LONG;
		pUmacInstance->Flags &= (~WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_BARKER_SHORT_PREAMBLE);
	}

	/*
	   call this before 11n stuff - mainly before calling
	   WFM_UMAC_Process_HtOperationIe()
	*/
	if (pBssInfo->Flags & WFM_BSS_INFO_FLAGS_PROT_ENABLED)
		pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_CTSRTS_PROTECTION;
	else
		pUmacInstance->Flags &= (~WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_CTSRTS_PROTECTION);

	if (pUmacInstance->isCoalescing) {
		pJoinParams->ProbeForJoin = 1;
	} else {
		if (pUmacInstance->dot11InfraStructureMode == WFM_802_IBSS) {
			pJoinParams->ProbeForJoin = 0;
		} else {
			if (pUmacInstance->gVars.p.DisableActiveJoin)
				pJoinParams->ProbeForJoin = 0;
			else
				pJoinParams->ProbeForJoin = 1;
		}
	}

	pJoinParams->BeaconInterval = pBssInfo->BeaconPeriod;

	pJoinParams->BasicRateSet = pUmacInstance->AllBasicSupportedRates_WithAP_Abgn;

	UMAC_SET_STATE(UmacHandle, UMAC_JOINING);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_JOINING\n");

	Result = UMAC_LL_REQ_JOIN(UmacHandle, pJoinParams);

	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pJoinTxDesc);

		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pUmacInstance->gVars.pJoinTxDesc
					);

		pUmacInstance->gVars.pJoinTxDesc = NULL;

		/*
		   Just going back to initialized state here.
		 */
		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");
	}				/*if(Result != WFM_STATUS_SUCCESS) */

exit_handler:
	return Result;

connect_failure:
	WFM_UMAC_2_UL_SendEvent(
				UmacHandle,
				WFM_EVT_CONNECT_FAILED,
				0,
				NULL,
				WFM_UMAC_EVT_NOT_RETRANSMIT
				);

	Result = WFM_STATUS_SUCCESS;
error_handler:
	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
	return Result;
}				/*end UMAC_ProcStartJoin() */

/******************************************************************************
 * NAME:	UMAC_ProcJoinSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the join success event from LMAC. It does the
 * following things.UMAC state will be changed to UMAC_JOINED.The buffer
 * allocated in UMAC_ProcStartJoin() will be released here.This function
 * will trigger the Authentication procedure.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pMsg	- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcJoinSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	uint32 memstatus;

	OS_ASSERT(pUmacInstance);
	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcJoinSuccess()\n");
	UMAC_SET_STATE_PROCESSING(UmacHandle);

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;
		pSmgmtReq->Type = SMGMT_REQ_TYPE_JOIN_SUCCESS;
		pSmgmtReq->ReqNum = 0;

		pSmgmtReq->operation.join_success_params.RtsThreashold = pUmacInstance->gVars.p.dot11RTSThreshold;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;
	}	/*if(!pSmgmtReq->HandleMgmtReq) */

	pSmgmtReq->HandleMgmtReq = 0;

	pUmacInstance->MinPowerLevel = 5;

	OS_ASSERT(pUmacInstance->gVars.pJoinTxDesc);

	WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

	/* Freeing the internal buffer */
	memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pJoinTxDesc
						);

	OS_ASSERT(memstatus);

	UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pJoinTxDesc);

	pUmacInstance->gVars.pJoinTxDesc = NULL;

	if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode) {
		UMAC_SET_STATE(UmacHandle, UMAC_ASSOCIATED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_ASSOCIATED\n");

		/* Select Operational rates */
		if (pUmacInstance->HtOfdmRates)
			pUmacInstance->CurrentModeOpRates = pUmacInstance->HtOfdmRates;
		else if (pUmacInstance->ErpOfdmRates)
			pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpOfdmRates;
		else if (pUmacInstance->ErpDssCckRates)
			pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpDssCckRates;
		else
			pUmacInstance->CurrentModeOpRates = pUmacInstance->AllSupportedRates_WithAP_Abgn;


		pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->CurrentModeOpRates);

		if (!pUmacInstance->isCoalescing) {
			WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_CONNECTED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		} else
			pUmacInstance->isCoalescing = FALSE;
	} else {

		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_CONNECTING,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);

		UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_JOINED\n");
	}

	if (WFM_802_INFRASTRUCTURE == pUmacInstance->dot11InfraStructureMode)
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_AUTH, NULL);

 exit_handler:

	return Result;
}				/*end UMAC_ProcJoinSuccess() */

/******************************************************************************
 * NAME:	UMAC_ProcJoinFailed
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the join failure event from LMAC.UMAC state will be
 * changed to UMAC_INITIALIZED.The buffer allocated in UMAC_ProcStartJoin() will
 * be released.All the state variables will be resetd to initial values.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pMsg	- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcJoinFailed(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 memstatus;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT((DBG_WFM_UMAC | DBG_WFM_ASSOC), "UMAC_ProcJoinFailed()\n\n");

	OS_ASSERT(pUmacInstance->gVars.pJoinTxDesc);

	WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

	/*Freeing the internal buffer */
	memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pJoinTxDesc
						);

	OS_ASSERT(memstatus);

	UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pJoinTxDesc);

	pUmacInstance->gVars.pJoinTxDesc = NULL;

	/*
	   De-Initializing First and then Initializing to bring UMAC to a
	   clean state
	*/
	Result = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_DEINIT, NULL);

	if (WFM_STATUS_PENDING == Result)
		OS_ASSERT(0);

	WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_INIT, NULL);

	WFM_UMAC_2_UL_SendEvent(
				UmacHandle,
				WFM_EVT_CONNECT_FAILED,
				0,
				NULL,
				WFM_UMAC_EVT_NOT_RETRANSMIT
				);

	return Result;
}				/*end UMAC_ProcJoinFailed() */

/******************************************************************************
 * NAME:	UMAC_ProcStartAuth
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initiates the Authentication procedure.
 * -# Memory for Auth frame is allocated
 * -# Authentication frame is formed and queued to LMAC for Tx
 * -# UMAC state will be changed to UMAC_AUTHENTICATING
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartAuth(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_MGMT_FRAME *pAuthFrame = NULL;
	uint16 FrameBodyLen = 0;
	UMAC_TX_DESC *pTxDesc = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	uint16 TransactionSeqNumber = 1;
	uint8 *pFrame = NULL;
	WFM_UMAC_AUTH_FRAME_BODY *pRxAuthFrame = NULL;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	uint16 AuthAlgo;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcStartAuth()\n");

#if FT_SUPPORT
	/*
	 * Check if the AP is in the list of FT pre-authenticated APs.
	 * If, already pre-authenticated, straightway set state to
	 * AUTHENTICATED and trigger event START_ASSOC.
	 */
	if ((pUmacInstance->FtFlags == (UMAC_FT_FLAG_INIT_MD_ASSOC_DONE))
	    && pUmacInstance->RoamingEnabled) {
		uint32 i;
		for (i = 0; i < 4; i++) {
			if (OS_MemoryEqual(pUmacInstance->RemoteMacAdd, WFM_MAC_ADDRESS_SIZE, pUmacInstance->gVars.p.FtAuthenticatedAPs[i].Bssid, WFM_MAC_ADDRESS_SIZE)) {
				if (pUmacInstance->gVars.p.FtAuthenticatedAPs[i].State == UMAC_FT_AUTHENTICATED)
					break;
			}
		}
		if (i < 4) {
			UMAC_SET_STATE(UmacHandle, UMAC_AUTHENTICATED);
			LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_AUTHENTICATED\n");
			/*Start Reassociation procedure from here */
			OS_MemoryReset(
				&pUmacInstance->gVars.p.FtAuthenticatedAPs[i],
				sizeof(pUmacInstance->gVars.p.FtAuthenticatedAPs[i])
				);
			WFM_SCHEDULE_TO_SELF(
						UmacHandle,
						UMAC_START_REASSOC,
						NULL
						);
			return Result;
		}
	}
#endif	/* FT_SUPPORT */

	if (pMsg) {
		pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pMsg);
		pRxAuthFrame = (WFM_UMAC_AUTH_FRAME_BODY *) pUmacMgmt->FrameBody;

		/* The recevied Rx Auth frame is Sequence number 2 frame */
		TransactionSeqNumber = pRxAuthFrame->TransactionSeqNumber + 1;

		/*Cancel the UMAC Timer as we got a valid response */
		if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer))
			LOG_EVENT(DBG_WFM_ERROR, "Failed to Cancel UMAC Timer\n");
	}

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);
	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_AUTH_REQ);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	pAuthFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pAuthFrame);
	/*
	   Creating the WLAN Header
	*/
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_AUTH,
					&pAuthFrame->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					0
					);

	/*
	   Forming the Authentication Request
	 */
	if (TransactionSeqNumber == 3) {
		/* Setting the Protected Frame bit */
		pAuthFrame->dot11hdr.FrameControl |= (1 << 14);
		/* 4: size of IV */
		OS_MemoryReset(pAuthFrame->FrameBody, 4);
		/* Allocate space for IV field */
		pFrame = (pAuthFrame->FrameBody + 4);
	} else
		pFrame = pAuthFrame->FrameBody;

	AuthAlgo = pUmacInstance->gVars.p.InfoHost.AuthenticationMode;
#if FT_SUPPORT
	if ((pUmacInstance->FtFlags & (UMAC_FT_FLAG_INIT_MD_ASSOC_DONE | UMAC_FT_FLAG_AUTH_OVER_AIR_BIT))
	    == (UMAC_FT_FLAG_INIT_MD_ASSOC_DONE | UMAC_FT_FLAG_AUTH_OVER_AIR_BIT)) {
		AuthAlgo = UMAC_FT_AUTH;
	}
#endif	/* FT_SUPPORT */

	FrameBodyLen = WFM_UMAC_CreateAuthFrame(
						UmacHandle,
						pFrame,
						AuthAlgo,
						TransactionSeqNumber,
						pRxAuthFrame,
						0	/* Status */
						);


	if (TransactionSeqNumber == 3) {
		/* Update length with size of IV */
		FrameBodyLen = FrameBodyLen + 4;
		/* 4: size of ICV */
		OS_MemoryReset((pAuthFrame->FrameBody + FrameBodyLen), 4);
		/*pdate length with size of ICV */
		FrameBodyLen = FrameBodyLen + 4;

		OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
		pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
		OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

		/*Release the RxDesc and RxBuffer */
		UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);
	}

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	/*
	   Here we are re-using the buffer from join, so different packet id
	   is not required
	*/

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcStartAuth() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	ASSIGN_MGT_FRAME_TX_RATE(pTxDesc, pUmacInstance);

	/*
	   All the management packets needs to be transmitted in WFM_AC_VO
	   As per spec.  Page 254, section 9.1.3.1 of IEEE Std 802.11 - 2007
	 */
	pTxDesc->QueueId = 3;	/*/WFM_AC_VO ; */
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;
	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL);

	/*Starting the UMAC timer */
	OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pUMACTimer,
			WFM_UMAC_MGMT_FRAME_TIMEOUT,
			UmacHandle
			);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					DEFAULT_LINK_ID
					);

	if (pUmacInstance->gVars.p.InfoHost.AuthenticationMode == UMAC_SHARED_KEY_AUTH && TransactionSeqNumber == 1) {
		UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
	} else if (pUmacInstance->gVars.p.InfoHost.AuthenticationMode == UMAC_SHARED_KEY_AUTH && TransactionSeqNumber == 3) {
		UMAC_SET_STATE(UmacHandle, UMAC_AUTHENTICATING);
		LOG_EVENT(DBG_WFM_UMAC, "STATE   = UMAC_AUTHENTICATING\n");
	}
	if (pUmacInstance->gVars.p.InfoHost.AuthenticationMode != UMAC_SHARED_KEY_AUTH) {
		UMAC_SET_STATE(UmacHandle, UMAC_AUTHENTICATING);
		LOG_EVENT(DBG_WFM_UMAC, "STATE   = UMAC_AUTHENTICATING\n");
	}


	if (Result != WFM_STATUS_SUCCESS) {

		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);

		UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_JOINED\n");

		/*Start Un-Join from here */
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNJOIN, NULL);

	}			/*if(Result != WFM_STATUS_SUCCESS) */

exit_handler:
	return Result;
}				/*end UMAC_ProcStartAuth() */

/******************************************************************************
 * NAME:UMAC_ProcAuthSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Authentication Success event from LMAC.
 * This function performs the following things
 * -# Releases the UMAC_RX_DESC and RX_BUFFER_DESC allocated
 *    for Authentication Response.
 * -# Triggers Association procedure.
 * -# UMAC state will be changed to UMAC_AUTHENTICATED
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcAuthSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcAuthSuccess()\n");

	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pMsg);

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	/*Cancel the UMAC Timer as we got a valid response */
	if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer))
		LOG_EVENT(DBG_WFM_ERROR, "Failed to Cancel UMAC Timer\n");


	WFM_UMAC_DBG_REMOVE_RX_DESC(pMsg);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, (UMAC_RX_DESC *) pMsg);

	UMAC_SET_STATE(UmacHandle, UMAC_AUTHENTICATED);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_AUTHENTICATED\n");

	/*Check for roaming */
	if (pUmacInstance->RoamingEnabled) {
		/*Start Reassociation procedure from here */
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_REASSOC, NULL);
	} else {
		/*Start Association procedure from here */
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_ASSOC, NULL);
	}

	return Result;
}				/*end UMAC_ProcAuthSuccess() */

/******************************************************************************
 * NAME:	UMAC_ProcAuthFailed
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Authentication failure event from LMAC.
 * This function performs the following things
 * -# Releases the UMAC_RX_DESC and RX_BUFFER_DESC allocated
 *    for Authentication Response.
 * -# Triggers the unjoin procedure.
 * -# UMAC state will be changed to UMAC_JOINED
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcAuthFailed(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcAuthFailed()\n");
	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pMsg);

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	/*Cancel the UMAC Timer as we got a valid response */
	if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer))
		LOG_EVENT(DBG_WFM_ERROR, "Failed to Cancel UMAC Timer\n");


	WFM_UMAC_DBG_REMOVE_RX_DESC(pMsg);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, (UMAC_RX_DESC *) pMsg);

	if ((pUmacInstance->gVars.p.InfoHost.AuthenticationMode == UMAC_OPEN_SYSTEM_AUTH)
	   && (pUmacInstance->gVars.p.InfoHost.EncryptionType == WFM_UMAC_WEP_ENC_SUPPORTED)) {
		UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_JOINED\n");

		pUmacInstance->gVars.p.InfoHost.AuthenticationMode = UMAC_SHARED_KEY_AUTH;
		/*Start Auth with shared key from here */
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_AUTH, NULL);
	} else {
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_CONNECT_FAILED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);

		UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_JOINED\n");

		/*Start Un-Join from here */
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNJOIN, NULL);
	}

	return Result;
}				/*end UMAC_ProcAuthFailed() */

/******************************************************************************
 * NAME:	UMAC_ProcDeAuthenticate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initiates the de-authentication procedure
 * This function performs the following things
 * -# Allocates memory for DeAuth frame and forms Authentication frame and
 *    queues it to LMAC for transmission.
 * -# Triggers the unjoin procedure.
 * -# UMAC state will be changed to UMAC_JOINED
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcDeAuthenticate(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_MGMT_FRAME *pDeAuthFrame = NULL;
	uint16 FrameBodyLen = 0;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 Encrypt;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcDeAuthenticate()\n");

	WFM_UMAC_DBG_PRINT_TX_DESC();

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_DEAUTH_REQ);

	LOG_DATA(DBG_WFM_UMAC, "UMAC_ProcDeAuthenticate pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	pDeAuthFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pDeAuthFrame);

	Encrypt = UMAC_ProtectionRequired(pUmacInstance, 0, WFM_UMAC_MGMT, WFM_UMAC_MGMT_DEAUTH);
	/*
	   Creating the WLAN Header
	 */
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_DEAUTH,
					&pDeAuthFrame->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					Encrypt
					);

	FrameBodyLen = WFM_UMAC_CreateDeAuthFrame(
							UmacHandle,
							pDeAuthFrame->FrameBody
						 );

	UMAC_SET_STATE(UmacHandle, UMAC_DE_AUTHENTICATING);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_DE_AUTHENTICATING\n");

	pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcDeAuthenticate() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	ASSIGN_MGT_FRAME_TX_RATE(pTxDesc, pUmacInstance);

	if (((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION) == WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION)
		|| ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) == WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION))
		pTxDesc->QueueId = WFM_AC_VO;	/*WFM_AC_VO ; */
	else
		pTxDesc->QueueId = WFM_AC_VO;	/*WFM_AC_BE ; */

	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;
	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL);

	UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_JOINED\n");

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	 */
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					DEFAULT_LINK_ID
					);


	if (Result != WFM_STATUS_SUCCESS)
		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);

	/*Start Un-Join from here */

	WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNJOIN, NULL);

exit_handler:
	return Result;
}				/*end UMAC_ProcDeAuthenticate() */

/******************************************************************************
 * NAME:	UMAC_ProcDeAuthSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is just to make the state-machine symmetric.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcDeAuthSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcDeAuthSuccess()\n");

	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer);
	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer);

	UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_JOINED\n");

	/*
	  This handler should not be called, so if we are reaching there,its
	  an error
	 */
	OS_ASSERT(0);

	return Result;
}				/*end UMAC_ProcDeAuthSuccess() */

/******************************************************************************
 * NAME:UMAC_ProcStartAssoc
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initiates Association procedure.It
 * -# Allocates memory for Association frame.
 * -# Forms association frame based on the parameters avialable
 *    and queues it to LMAC for transmission.
 * -# UMAC state will be changed to UMAC_ASSOCIATING.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartAssoc(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	WFM_UMAC_MGMT_FRAME *pAssocFrame = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_ProcStartAssoc()\n");

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_ASSO_REQ);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_ProcStartAssoc pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
	WFM_UMAC_DBG_PRINT_TX_DESC();

	pAssocFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pAssocFrame);

	/*
	   Creating the WLAN Header
	 */
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_ASSOC_REQ,
					&pAssocFrame->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					0
					);

	FrameBodyLen = WFM_UMAC_CreateAssocReq(
						UmacHandle,
						pAssocFrame->FrameBody,
						pUmacInstance->SsidLength,
						pUmacInstance->Ssid,
						NULL	/* Assoc Req */
	    );

	pUmacInstance->AssocReqIELen = (FrameBodyLen - 4);

	if (pUmacInstance->AssocReqIELen <= sizeof(pUmacInstance->AssocReqIE)) {
		/*
		  Copy Association Information, from Association Request Frame
		*/
		OS_MemoryCopy(
			(void *)&pUmacInstance->AssocReqIE[0],
			(void *)((uint8 *) &pAssocFrame->FrameBody[0] + 4),
			pUmacInstance->AssocReqIELen
			);
	} else {
		pUmacInstance->AssocReqIELen = 0;
		LOG_EVENT(DBG_WFM_ERROR, "ERROR In Storing Assoc Info.\n");

	}

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcStartAssoc() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	ASSIGN_MGT_FRAME_TX_RATE(pTxDesc, pUmacInstance);

	pTxDesc->QueueId = WFM_AC_VO;	/*/WFM_AC_VO ; */
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;
	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL);

	UMAC_SET_STATE(UmacHandle, UMAC_ASSOCIATING);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_ASSOCIATING\n");

	LOG_BYTES(DBG_WFM_ASSOC, "ASSOC_REQ: ", (uint8 *) pAssocFrame, GET_TX_DESC_FRAME_LENGTH(pTxDesc), GET_TX_DESC_FRAME_LENGTH(pTxDesc));

	/*Starting the UMAC timer */
	OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pUMACTimer,
			WFM_UMAC_MGMT_FRAME_TIMEOUT,
			UmacHandle
			);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					DEFAULT_LINK_ID
					);

	if (Result != WFM_STATUS_SUCCESS) {

		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);

		UMAC_SET_STATE(UmacHandle, UMAC_AUTHENTICATED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_AUTHENTICATED\n");
		/* De-Authenticate */
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_DE_AUTHE, NULL);

	}			/*if(Result != WFM_STATUS_SUCCESS) */

exit_handler:
	return Result;
}				/*end UMAC_ProcStartAssoc() */

/******************************************************************************
 * NAME:	UMAC_ProcAssocSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Association success event, which will be generated
 * on the reception of an association response from the remote device with
 * status success.
 * -# Releases the Rx buffer and Rxdescriptor for Association Rsp frame
 * -# Selects the highest rate available as the current rate.
 * -# UMAC state will be changed to UMAC_ASSOCIATED
 * -# Informs the host that we are in ASSOCIATED/ CONNECTED state now
 *    by sending the event  WFM_WLAN_EVT_CONNECTED.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcAssocSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
#if FT_SUPPORT
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
#endif	/* FT_SUPPORT */

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_ProcAssocSuccess()\n");
#if FT_SUPPORT
	if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_FT_SUPPORTED_BY_AP) {
		if ((pUmacInstance->FtFlags & UMAC_FT_FLAG_INIT_MD_ASSOC_DONE) == 0)
			pUmacInstance->FtFlags = UMAC_FT_FLAG_INIT_MD_ASSOC_DONE;
	}
#endif	/* FT_SUPPORT */
	return UMAC_ProcAssocSuccessWorker(UmacHandle, pMsg, WFM_EVT_CONNECTED);

}				/*end UMAC_ProcAssocSuccess() */

/******************************************************************************
 * NAME:	UMAC_ProcAssocFailed
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Association failure event, which will be generated
 * on the reception of an association response from the remote device with
 * status failure.
 * -# Releases the Rx buffer and Rxdescriptor for Association Rsp frame
 * -# UMAC state will be changed to UMAC_AUTHENTICATED
 * -# Triggers De-Authentication procedure.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcAssocFailed(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	WFM_UMAC_ASSOC_RSP_BODY *pAssocRsp = NULL;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcAssocFailed()\n");

	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pRxDescriptor);

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	/*Cancel the UMAC Timer as we got a valid response */
	if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer))
		LOG_EVENT(DBG_WFM_ERROR, "Failed to Cancel UMAC Timer\n");


	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
	CHECK_POINTER_IS_VALID(pUmacMgmt);
	pAssocRsp = (WFM_UMAC_ASSOC_RSP_BODY *) pUmacMgmt->FrameBody;
	CHECK_POINTER_IS_VALID(pAssocRsp);

	/* 802.11 frame endianness (little endian) */
	pUmacInstance->AssocStatusCode = pAssocRsp->StatusCode;

	/* 802.11 frame endianness (little endian) */
	pUmacInstance->CapabilitiesRsp = pAssocRsp->Capability;

	WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

	/* Send an event to host saying that assoc had failed */
	WFM_UMAC_2_UL_SendEvent(
				UmacHandle,
				WFM_EVT_CONNECT_FAILED,
				0,
				NULL,
				WFM_UMAC_EVT_NOT_RETRANSMIT
				);

	UMAC_SET_STATE(UmacHandle, UMAC_AUTHENTICATED);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_AUTHENTICATED\n");
	/* De-Authenticate */
	WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_DE_AUTHE, NULL);

	return Result;
}				/*end UMAC_ProcAssocFailed() */

/******************************************************************************
 * NAME:	UMAC_ProcStartReAssoc
 *---------------------------------------------------------------------------*/
/**
 * \brief
 *This function initiates ReAssociation procedure.It
 * -# Allocates memory for ReAssociation frame
 * -# Forms association frame based on the parameters avialable
 *    and queues it to LMAC for transmission.
 * -# UMAC state will be changed to UMAC_ASSOCIATING
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartReAssoc(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	WFM_UMAC_MGMT_FRAME *pReAssocFrame = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcStartReAssoc()\n");
	UMAC_SET_STATE_PROCESSING(UmacHandle);
#if 0
	if (TRUE == pUmacInstance->sendDisassoc) {
		/*
		   It means that set PM mode failed and PM state is
		   undetermined. So disassociate. Before that stabilize the
		   UMAC state machine here by brining it back to
		   initialized state.
		 */
		pUmacInstance->sendDisassoc = FALSE;
		UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_DE_ASSOC, NULL);
		goto exit_handler;
	}
#endif

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_REASSOC_REQ);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_ProcStartReAssoc pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	pReAssocFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pReAssocFrame);

	/*
	   Creating the WLAN Header
	 */
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_REASSOC_REQ,
					&pReAssocFrame->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					0
					);

	FrameBodyLen = WFM_UMAC_CreateAssocReq(
						UmacHandle,
						pReAssocFrame->FrameBody,
						pUmacInstance->SsidLength,
						pUmacInstance->Ssid,
						pUmacInstance->CurrentAPAdd
						);

	pUmacInstance->AssocReqIELen = (FrameBodyLen - 4 - WFM_UMAC_MAC_ADDRESS_SIZE);

	if (pUmacInstance->AssocReqIELen <= sizeof(pUmacInstance->AssocReqIE)) {
		/*
		  Copy Association Information, from ReAssociation Request
		  Frame
		*/
		/* WFM_UMAC_MAC_ADDRESS_SIZE added as its a reassoc case */
		OS_MemoryCopy(
			(void *)&pUmacInstance->AssocReqIE[0],
			(void *)((uint8 *) &pReAssocFrame->FrameBody[0] + 4 + WFM_UMAC_MAC_ADDRESS_SIZE),
			pUmacInstance->AssocReqIELen
			);
	} else
		pUmacInstance->AssocReqIELen = 0;



	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcStartReAssoc() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	ASSIGN_MGT_FRAME_TX_RATE(pTxDesc, pUmacInstance);

	pTxDesc->QueueId = WFM_AC_VO;	/*/WFM_AC_VO ; */
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_RECONNECT);
	pTxDesc->Reserved = 0;
	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL
					);

	UMAC_SET_STATE(UmacHandle, UMAC_REASSOCIATING);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_REASSOCIATING\n");

	/*Starting the UMAC timer */
	OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pUMACTimer,
			WFM_UMAC_MGMT_FRAME_TIMEOUT,
			UmacHandle
			);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Reset bRoamIndRcvd if it is true as we are sending Reassoc request
	   from here
	*/
	if (pUmacInstance->RoamingEnabled)
		pUmacInstance->RoamingEnabled = 0;

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					DEFAULT_LINK_ID
					);

	if (Result != WFM_STATUS_SUCCESS)
		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);

 exit_handler:
	return Result;
} /* end UMAC_ProcStartReAssoc() */

/******************************************************************************
 * NAME:	UMAC_ProcReAssocSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes ReAssociation success event, which will be generated
 * on the reception of an association response from the remote device with
 * status success.
 * -# Releases the Rx buffer and Rxdescriptor for ReAssociation Rsp frame
 * -# Selects the highest rate available as the current rate.
 * -# UMAC state will be changed to UMAC_ASSOCIATED
 * -# Informs the host that we are in ASSOCIATED/ CONNECTED state now
 *    by sending the event  WFM_WLAN_EVT_CONNECTED.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcReAssocSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcReAssocSuccess()\n");
	return UMAC_ProcAssocSuccessWorker(UmacHandle, pMsg, WFM_EVT_RECONNECTED);
} /* end UMAC_ProcReAssocSuccess() */

/******************************************************************************
 * NAME:	UMAC_ProcReAssocFailed
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes ReAssociation failure event, which will be generated
 * on the reception of an association response from the remote device with
 * status failure.
 * -# Releases the Rx buffer and Rxdescriptor for ReAssociation Rsp frame
 * -# UMAC state will be changed to UMAC_AUTHENTICATED
 * -# Triggers De-Authentication procedure.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcReAssocFailed(UMAC_HANDLE UmacHandle, void *pMsg)
{
	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcReAssocFailed()\n");

	return UMAC_ProcAssocFailed(UmacHandle, pMsg);

} /* end UMAC_ProcReAssocFailed() */

/******************************************************************************
 * NAME:	UMAC_ProcStartBGScan
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function starts the Back Ground scanning procedure.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartBGScan(UMAC_HANDLE UmacHandle, void *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	/*Setting the flag to indicate that it's a back ground scan */
	if (pUmacInstance->gVars.p.InfoHost.disableBGScan)
		return WFM_STATUS_REQ_REJECTED;
	else {
		pUmacInstance->IsBgScan = 1;
		return UMAC_ProcStartScan(UmacHandle, pMsg);
	}
} /* end UMAC_ProcStartBGScan() */

/******************************************************************************
 * NAME:	UMAC_ProcReConnect
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes CONNECT request from host, when UMAC is already in
 * connected state or in the process of establishing a connection.
 * If UMAC is connected and recieving a request to connect to another SSID,
 * this function triggers a disconnect and Queues connect request.
 * If UMAC is connected and recieves a request to connect to the same SSID,
 * UMAC will do a reassociate now. [should be extented later to do a scan and
 * select an SSID with better RSSI]
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcReConnect(UMAC_HANDLE UmacHandle, void *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_BSS_INFO *pBssInfo;

	OS_ASSERT(pMsg);

	pBssInfo = (WFM_BSS_INFO *) pMsg;

	CHECK_POINTER_IS_VALID(pBssInfo);

	if (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) {
		/*reset the 3 events of roaming */
		pUmacInstance->RssiRcpiThresholdEventRcvd = 0;
		pUmacInstance->TxFailureEventRcvd = 0;
		pUmacInstance->BssLostEventRcvd = 0;
		pUmacInstance->BssRegainedEventRcvd = 0;



		if (pUmacInstance->isProbeReqFrame) {
			OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pBssLostTimer);
			pUmacInstance->isProbeReqFrame = 0;
			pUmacInstance->isProbeRespRcvd = 0;
		}

#if DOT11K_SUPPORT
		/*Stop Timers */
		OS_STOP_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer
			);

		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer);
		/*reset all the variables */
		OS_MemoryReset(
				&pUmacInstance->dot11k_Meas_Params,
				sizeof(DOT11K_MEAS_PARAMS)
				);
#endif	/* DOT11K_SUPPORT */

		if (OS_MemoryEqual((void *)&pBssInfo->Ssid[0], pBssInfo->SsidLength, (void *)&pUmacInstance->Ssid[0], pUmacInstance->SsidLength)
		    && OS_MemoryEqual((void *)&pBssInfo->MacAddress[0], WFM_MAC_ADDRESS_SIZE, (void *)&pUmacInstance->RemoteMacAdd[0], WFM_MAC_ADDRESS_SIZE)
		    ) {
			pUmacInstance->IsFirmwareBssParamSet = FALSE;
			OS_MemoryCopy(
					pUmacInstance->CurrentAPAdd,
					pUmacInstance->RemoteMacAdd,
					WFM_MAC_ADDRESS_SIZE
					);

			/*Its the same SSID, so do a re-associate */
			Result = WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_START_REASSOC,
							NULL
							);

		} else if (OS_MemoryEqual((void *)&pBssInfo->Ssid[0], pBssInfo->SsidLength, (void *)&pUmacInstance->Ssid[0], pUmacInstance->SsidLength)
			   && (!OS_MemoryEqual((void *)&pBssInfo->MacAddress[0], WFM_MAC_ADDRESS_SIZE, (void *)&pUmacInstance->RemoteMacAdd[0], WFM_MAC_ADDRESS_SIZE))
			   && (pUmacInstance->RoamingEnabled)) {
			/*Its a roaming case */
			/*Go to Initailization state */
			UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
			Result = WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_START_JOIN,
							pMsg
							);
		} else {
			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_DE_ASSOC, NULL);
			Result = WFM_STATUS_PENDING;
		}

		goto exit_handler;
	}			/*if( pUmacInstance->UmacCurrentState ==  UMAC_ASSOCIATED ) */
#if FT_SUPPORT
	else if (pUmacInstance->UmacCurrentState == UMAC_AUTHENTICATED) {
		if (OS_MemoryEqual(pUmacInstance->RemoteMacAdd, WFM_MAC_ADDRESS_SIZE, pBssInfo->MacAddress, WFM_MAC_ADDRESS_SIZE)
		    && (pUmacInstance->FtFlags == UMAC_FT_FLAG_INIT_MD_ASSOC_DONE)) {
			/* Implies already joined to the BSS */
			Result = WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_START_REASSOC,
							NULL
							);
		}
	}
#endif	/* FT_SUPPORT */
	else if ((pUmacInstance->UmacCurrentState > UMAC_INITIALIZED) && (pUmacInstance->UmacCurrentState <= UMAC_PROCESSING)
	    ) {
		Result = WFM_STATUS_PENDING;
	} else
		OS_ASSERT(0);

 exit_handler:
	return Result;
} /* end UMAC_ProcConnect() */

/******************************************************************************
 * NAME:	UMAC_ProcStartFtAuth
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will start FT Authentication
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartFtAuth(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
#if FT_SUPPORT
	WFM_BSS_INFO *pBssInfo;
	WFM_UMAC_INSTANCE *pUmacInstance;
	UMAC_FT_AP_DESC *pFtAp;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	UMAC_TX_DESC *pTxDesc = NULL;
	WFM_UMAC_MGMT_FRAME *pFtReqFrame = NULL;
	uint16 FrameBodyLen;
	uint8 ApIndex;

	pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	pBssInfo = (WFM_BSS_INFO *) pMsg;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcStartFtAuth()\n");

	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pMsg);

	if (!OS_MemoryEqual(&pBssInfo->MobilityDomainElement.mdid[0], 2, &pUmacInstance->gVars.p.mdie[2], 2)) {
		EVT_DATA_FT_AUTH_RESPONSE *pFtResp = &pUmacInstance->FtResp;

		D0_ADDR_COPY(&pFtResp->Bssid[0], &pBssInfo->MacAddress[0])
		    pFtResp->Status = 1;
		pFtResp->ies_len = 0;
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_FT_AUTH_RESPONSE,
					sizeof(*pFtResp),
					(uint8 *) pFtResp,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		return Result;
	}

	/* Make sure that the AP is not already in the FT AP list */
	for (ApIndex = 0; ApIndex < MAX_FT_AP; ApIndex++) {
		if (OS_MemoryEqual(pBssInfo->MacAddress, WFM_MAC_ADDRESS_SIZE, pUmacInstance->gVars.p.FtAuthenticatedAPs[ApIndex].Bssid, WFM_MAC_ADDRESS_SIZE))
			break;
	}

	/*
	  Get a free slot from FT AP list and fill it up with
	  target AP
	*/
	if (ApIndex == MAX_FT_AP) {
		uint8 i;
		for (i = 0; i < MAX_FT_AP; i++) {
			if (pUmacInstance->gVars.p.FtAuthenticatedAPs[i].State == UMAC_FT_NONE) {
				ApIndex = i;
				break;
			}
		}
	}

	if (ApIndex >= MAX_FT_AP)
		return Result;	/* Error */

	pFtAp = &pUmacInstance->gVars.p.FtAuthenticatedAPs[ApIndex];

	if (pBssInfo->FtAuthFlags & UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT) {
		if (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) {
			/* When leaving current AP always use over the air */
			pUmacInstance->FtFlags |= UMAC_FT_FLAG_AUTH_OVER_AIR_BIT;
			/*
			   Mark that we are leaving BSS and doing FT auth over
			   air
			*/
			pUmacInstance->FtFlags |= UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT;

			OS_MemoryCopy(
					&pUmacInstance->FtTargetBss,
					pBssInfo,
					sizeof(*pBssInfo)
					);

			D0_ADDR_COPY(
					&pFtAp->Bssid[0],
					&pBssInfo->MacAddress[0]
					);
			pFtAp->State = UMAC_FT_AUTHENTICATING_AIR;

			if (pBssInfo->FtAuthFlags & UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT) {
				pUmacInstance->RssiRcpiThresholdEventRcvd = 0;
				pUmacInstance->TxFailureEventRcvd = 0;
				pUmacInstance->BssLostEventRcvd = 0;
				pUmacInstance->BssRegainedEventRcvd = 0;
			}

			UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
			Result = WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_START_JOIN,
							pMsg
							);
		}
	} else if (pBssInfo->FtAuthFlags & UMAC_FT_FLAG_AUTH_OVER_DS_BIT) {
		/* over the DS */
		/* Need some sort of lock here */
		if (pFtAp->State != UMAC_FT_NONE)
			return WFM_STATUS_BAD_PARAM;	/* Error */

		pUmacInstance->FtFlags |= UMAC_FT_FLAG_AUTH_OVER_DS_BIT;
		OS_MemoryCopy(
				&pUmacInstance->FtTargetBss,
				pBssInfo,
				sizeof(*pBssInfo)
				);

		D0_ADDR_COPY(&pFtAp->Bssid[0], &pBssInfo->MacAddress[0]);
		pFtAp->State = UMAC_FT_INIT;

		UMAC_SET_STATE_PROCESSING(UmacHandle);

		/*
		  Create FT Req Action Frame and send it to LMAC
		 */
		pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
		    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

		OS_ASSERT(pDevIfTxReq);

		pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

		UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_ASSO_REQ);

		LOG_DATA((DBG_WFM_UMAC), "UMAC_ProcStartFtAuth pTxDesc=0x%x)\n", pTxDesc);

		WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
		WFM_UMAC_DBG_PRINT_TX_DESC();

		pFtReqFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
		CHECK_POINTER_IS_VALID(pFtReqFrame);

		WFM_UMAC_CreateDot11FrameHdr(
						UmacHandle,
						WFM_UMAC_MGMT,
						WFM_UMAC_MGMT_ACTION,
						&pFtReqFrame->dot11hdr,
						pUmacInstance->RemoteMacAdd,
						pUmacInstance->MacAddress,
						0,
						0
						);

		FrameBodyLen = WFM_UMAC_CreateFtActionReq(
							UmacHandle,
							pFtReqFrame->FrameBody,
							pFtAp->Bssid
							);
		ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

		UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

		if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
			LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcStartFtAuth() : Transmit Queue Full\n");
			UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pDevIfTxReq
						);
			Result = WFM_STATUS_OUT_OF_RESOURCES;
			goto exit_handler;
		}

		ASSIGN_MGT_FRAME_TX_RATE(pTxDesc, pUmacInstance);

		pTxDesc->QueueId = 3;	/*/WFM_AC_VO ; */
		pTxDesc->More = 0;
		pTxDesc->Flags = 0;
		ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_RECONNECT);
		pTxDesc->Reserved = 0;
		pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

		UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL
					);

		LOG_BYTES(DBG_WFM_ASSOC, "FT_REQ Action: ", (uint8 *) pFtReqFrame, GET_TX_DESC_FRAME_LENGTH(pTxDesc), GET_TX_DESC_FRAME_LENGTH(pTxDesc));
		/*
		  Set FtSate of target AP to FT_AUTHING_DS
		 */

		pFtAp->State = UMAC_FT_AUTHENTICATING_DS;

		/*Starting the UMAC timer */
		OS_START_TIMER(
				pUmacInstance->gVars.p.UmacTimers.pUMACTimer,
				WFM_UMAC_MGMT_FRAME_TIMEOUT,
				UmacHandle
				);

		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

		/*
		  Sending the management frame to LMAC for transmission
		*/

		Result = UMAC_LL_REQ_TX_FRAME(
						UmacHandle,
						pTxDesc->QueueId,
						pDevIfTxReq,
						DEFAULT_LINK_ID
						);
		UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
	} else if (((pBssInfo->FtAuthFlags & UMAC_FT_FLAG_AUTH_OVER_AIR_BIT))
		   && (!(pBssInfo->FtAuthFlags & UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT))) {
		/*
		  Set state to PROCESSING
		 */
		SMGMT_REQ_PARAMS *pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;

		D0_ADDR_COPY(&pFtAp->Bssid[0], &pBssInfo->MacAddress[0]);
		pFtAp->State = UMAC_FT_INIT;

		/*
		  Mark that we are doing an over the air auth
		 */
		pUmacInstance->FtFlags |= UMAC_FT_FLAG_AUTH_OVER_AIR_BIT;
		OS_MemoryCopy(
				&pUmacInstance->FtTargetBss,
				pBssInfo,
				sizeof(*pBssInfo)
				);
			pUmacInstance->FtFlags |= UMAC_FT_FLAG_AUTH_JOIN_BSS_BIT;
			pFtAp->State = UMAC_FT_AUTHENTICATING_AIR;

		if (!pSmgmtReq->HandleMgmtReq) {
			OS_MemoryReset(pSmgmtReq, sizeof(*pSmgmtReq));
			pSmgmtReq->HandleMgmtReq = 1;
			pSmgmtReq->Type = SMGMT_REQ_TYPE_FT_AUTH_JOIN;
			pSmgmtReq->ReqNum = 0;

			Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

			if (Result != WFM_STATUS_SUCCESS)
				goto exit_handler;

		}
		pSmgmtReq->HandleMgmtReq = 0;
	}
 exit_handler:
#endif	/* FT_SUPPORT */
	return Result;
} /* end UMAC_ProcStartFtAuth() */

/******************************************************************************
 * NAME:	UMAC_ProcFtJoinComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * FT Join Complete
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcFtJoinComplete(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
#if FT_SUPPORT
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	JOIN_CONFIRM *pJoinCnf = (JOIN_CONFIRM *) pMsg;
	uint32 memstatus;

	if ((pUmacInstance->FtFlags & (UMAC_FT_FLAG_AUTH_OVER_AIR_BIT | UMAC_FT_FLAG_AUTH_JOIN_BSS_BIT))
	    == (UMAC_FT_FLAG_AUTH_OVER_AIR_BIT | UMAC_FT_FLAG_AUTH_JOIN_BSS_BIT)) {
		if (pJoinCnf->Status == WFM_STATUS_SUCCESS) {
			UMAC_TX_DESC *pTxDesc = NULL;
			UMAC_DEVIF_TX_DATA *pDevIfTxReq;
			WFM_UMAC_MGMT_FRAME *pAuthFrame = NULL;
			uint8 *pFrame;
			uint16 FrameBodyLen;

			pUmacInstance->FtFlags &= ~UMAC_FT_FLAG_AUTH_JOIN_BSS_BIT;

			UMAC_SET_STATE_PROCESSING(UmacHandle);

			/* Create FT Auth Frame. */

			pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
			    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);
			OS_ASSERT(pDevIfTxReq);

			pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

			UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_AUTH_REQ);

			WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

			pAuthFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
			CHECK_POINTER_IS_VALID(pAuthFrame);
			/* Creating the WLAN Header */
			WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_AUTH,
					&pAuthFrame->dot11hdr,
					&pUmacInstance->FtTargetBss.MacAddress[0],
					pUmacInstance->MacAddress,
					0,
					0
					);

			D0_ADDR_COPY(
				&pAuthFrame->dot11hdr.Address1[0],
				&pUmacInstance->FtTargetBss.MacAddress[0]
				);

			pFrame = pAuthFrame->FrameBody;

			FrameBodyLen = WFM_UMAC_CreateAuthFrame(
							UmacHandle,
							pFrame,
							UMAC_FT_AUTH,
							1,
							NULL,
							0
							);

			ASSIGN_FRAME_LENGTH(
				pTxDesc,
				FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR)
				);

			/*
			  Here we are re-using the buffer from join, so
			  different packet id is not required
			*/
			UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

			if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
				LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcFtJoinComplete() : Transmit Queue Full\n");
				UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
							UmacHandle,
							pDevIfTxReq
							);
				Result = WFM_STATUS_OUT_OF_RESOURCES;
				goto exit_handler;
			}
			pTxDesc->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;
			/*
			  All the management packets needs to be transmitted
			  in WFM_AC_VO As per spec. Page 254, section 9.1.3.1
			  of IEEE Std 802.11 - 2007
			*/
			pTxDesc->QueueId = 0;	/*/WFM_AC_VO ; */
			pTxDesc->More = 0;
			pTxDesc->Flags = 0;
			ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_RECONNECT);
			pTxDesc->Reserved = 0;
			pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

			UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL
					);

			/*Starting the UMAC timer */
			OS_START_TIMER(
				pUmacInstance->gVars.p.UmacTimers.pUMACTimer,
				WFM_UMAC_MGMT_FRAME_TIMEOUT,
				UmacHandle
				);

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

			ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

			/* Send FT Auth Frame to firmware. */
			Result = UMAC_LL_REQ_TX_FRAME(
							UmacHandle,
							pTxDesc->QueueId,
							pDevIfTxReq,
							DEFAULT_LINK_ID
							);

			if (Result != WFM_STATUS_SUCCESS) {
				UMAC_ReleaseTxResources(
							UmacHandle,
							pTxDesc,
							DEFAULT_LINK_ID
							);
			}	/*if(Result != WFM_STATUS_SUCCESS) */
			UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
		} else {

			/* Failure case */
			int i;
			UMAC_FT_AP_DESC *pFtAp = NULL;
			EVT_DATA_FT_AUTH_RESPONSE *pFtResp;
			pFtResp = &pUmacInstance->FtResp;

			D0_ADDR_COPY(
				&pFtResp->Bssid[0],
				&pUmacInstance->FtTargetBss.MacAddress[0]
				);
			pFtResp->Status = 1;
			pFtResp->ies_len = 0;

			for (i = 0; i < MAX_FT_AP; i++) {
				if (OS_MemoryEqual(&pUmacInstance->FtTargetBss.MacAddress[0], WFM_MAC_ADDRESS_SIZE, &pUmacInstance->gVars.p.FtAuthenticatedAPs[i].Bssid[0], WFM_MAC_ADDRESS_SIZE)) {
					pFtAp = &pUmacInstance->gVars.p.FtAuthenticatedAPs[i];
					OS_MemoryReset(pFtAp, sizeof(*pFtAp));
					break;
				}
			}
			OS_MemoryReset(
					&pUmacInstance->FtTargetBss,
					sizeof(pUmacInstance->FtTargetBss)
					);

			pUmacInstance->FtFlags &= ~(UMAC_FT_FLAG_AUTH_OVER_AIR_BIT | UMAC_FT_FLAG_AUTH_JOIN_BSS_BIT);
			WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_FT_AUTH_RESPONSE,
						sizeof(*pFtResp),
						(uint8 *) pFtResp,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);
		}
	} else if ((pUmacInstance->FtFlags & (UMAC_FT_FLAG_AUTH_OVER_AIR_BIT | UMAC_FT_FLAG_AUTH_JOINBACK_BSS_BIT))
		   == (UMAC_FT_FLAG_AUTH_OVER_AIR_BIT | UMAC_FT_FLAG_AUTH_JOINBACK_BSS_BIT)) {
		int i;
		UMAC_FT_AP_DESC *pFtAp = NULL;
		EVT_DATA_FT_AUTH_RESPONSE *pFtResp;
		pFtResp = &pUmacInstance->FtResp;
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_FT_AUTH_RESPONSE,
					sizeof(*pFtResp),
					(uint8 *) pFtResp,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		for (i = 0; i < MAX_FT_AP; i++) {
			if (OS_MemoryEqual(&pUmacInstance->FtTargetBss.MacAddress[0], WFM_MAC_ADDRESS_SIZE, &pUmacInstance->gVars.p.FtAuthenticatedAPs[i].Bssid[0], WFM_MAC_ADDRESS_SIZE)) {
				pFtAp = &pUmacInstance->gVars.p.FtAuthenticatedAPs[i];
				if (pFtAp->State == UMAC_FT_NONE)
					OS_MemoryReset(pFtAp, sizeof(*pFtAp));
				break;
			}
		}
		OS_MemoryReset(
				&pUmacInstance->FtTargetBss,
				sizeof(pUmacInstance->FtTargetBss)
				);

		pUmacInstance->FtFlags &= ~(UMAC_FT_FLAG_AUTH_OVER_AIR_BIT | UMAC_FT_FLAG_AUTH_JOINBACK_BSS_BIT);

	}
	OS_ASSERT(pUmacInstance->gVars.pJoinTxDesc);

	WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

	/*Freeing the internal buffer */
	memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pJoinTxDesc
						);

	OS_ASSERT(memstatus);

	UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pJoinTxDesc);

	pUmacInstance->gVars.pJoinTxDesc = NULL;

exit_handler:
#endif	/* FT_SUPPORT */
	return Result;
} /* end UMAC_ProcFtJoinComplete() */

/******************************************************************************
 * NAME:	UMAC_ProcFtAuthComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * FT Auth Complete
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcFtAuthComplete(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
#if FT_SUPPORT
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	WFM_UMAC_FT_RESP_ACTION_BODY *pFtRspAction = NULL;
	UMAC_FT_AP_DESC *pFtAp = NULL;
	UMAC_RX_DESC *pRxDescriptor = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 SubType;
	uint8 *pFtTargetAp;
	EVT_DATA_FT_AUTH_RESPONSE *pFtResp = NULL;
	uint8 FtJoinBack = 0;
	int i;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcFtAuthComplete()\n");

	if (pUmacInstance->FtFlags & (UMAC_FT_FLAG_AUTH_OVER_AIR_BIT | UMAC_FT_FLAG_AUTH_OVER_DS_BIT)) {
		pFtTargetAp = &pUmacInstance->FtTargetBss.MacAddress[0];

		for (i = 0; i < MAX_FT_AP; i++) {
			if (D0_ADDR_EQ(pFtTargetAp, &pUmacInstance->gVars.p.FtAuthenticatedAPs[i].Bssid[0])) {
				pFtAp = &pUmacInstance->gVars.p.FtAuthenticatedAPs[i];
				break;
			}
		}
	}

	if (pFtAp == NULL)
		goto ft_exit;

	pFtResp = &pUmacInstance->FtResp;

	if (pMsg == NULL) {
		pFtResp->Status = 1;
		pFtResp->ies_len = 0;
		D0_ADDR_COPY(&pFtResp->Bssid[0], &pFtAp->Bssid[0]);

		if (pUmacInstance->FtFlags & UMAC_FT_FLAG_AUTH_OVER_AIR_BIT) {
			/* Case of FT over the air auth timeout */

			pFtResp->flags = UMAC_FT_FLAG_AUTH_OVER_AIR_BIT;
			if (pUmacInstance->FtFlags & UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT)
				pFtResp->flags |= UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT;
			else
				FtJoinBack = 1;
		} else {
			pFtResp->flags = UMAC_FT_FLAG_AUTH_OVER_DS_BIT;
			pUmacInstance->FtFlags &= ~UMAC_FT_FLAG_AUTH_OVER_DS_BIT;
		}
		OS_MemoryReset(pFtAp, sizeof(*pFtAp));
	} else {
		pRxDescriptor = (UMAC_RX_DESC *) pMsg;
		pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
		CHECK_POINTER_IS_VALID(pUmacMgmt);
		CHECK_POINTER_IS_VALID(pUmacMgmt->FrameBody);

		/*b4,b5,b6 and b7 forms SubType */
		SubType = (uint8) ((pUmacMgmt->dot11hdr.FrameControl >> 4) & 0x000F);

		if (SubType == WFM_UMAC_MGMT_ACTION) {
			pFtRspAction = (WFM_UMAC_FT_RESP_ACTION_BODY *) pUmacMgmt->FrameBody;
			if (!D0_ADDR_EQ(pFtTargetAp, &pFtRspAction->TargetAPMac[0]))
				goto ft_exit;


			if (pFtAp->State == UMAC_FT_AUTHENTICATING_DS) {
				if (pFtRspAction->Status == 0)
					pFtAp->State = UMAC_FT_AUTHENTICATED;
				else
					OS_MemoryReset(pFtAp, sizeof(*pFtAp));

			}
			/*Cancel the UMAC Timer as we got a valid response */
			if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer))
				LOG_EVENT(DBG_WFM_ERROR, "Failed to Cancel UMAC Timer\n");


			pUmacInstance->FtFlags &= ~UMAC_FT_FLAG_AUTH_OVER_DS_BIT;

			D0_ADDR_COPY(&pFtResp->Bssid[0], &pFtRspAction->TargetAPMac[0]);
			pFtResp->Status = pFtRspAction->Status;
			pFtResp->flags = UMAC_FT_FLAG_AUTH_OVER_DS_BIT;

			if (pFtResp->Status == 0) {
				pFtResp->ies_len = GET_RX_DESC_FRAME_LENGTH(pRxDescriptor)
				    - sizeof(WFM_UMAC_FRAME_HDR) - 14;
				OS_MemoryCopy(&pFtResp->ies[0], &pFtRspAction->IEs[0], pFtResp->ies_len);
			}
		} else if (SubType == WFM_UMAC_MGMT_AUTH) {
			if (pUmacInstance->FtFlags & UMAC_FT_FLAG_AUTH_OVER_AIR_BIT) {
				WFM_UMAC_AUTH_FRAME_BODY *pAuthFrame = (WFM_UMAC_AUTH_FRAME_BODY *) (&pUmacMgmt->FrameBody[0]);

				if (pAuthFrame->StatusCode == 0)
					pFtAp->State = UMAC_FT_AUTHENTICATED;
				else
					pFtAp->State = UMAC_FT_NONE;

				D0_ADDR_COPY(
						&pFtResp->Bssid[0],
						&pUmacMgmt->dot11hdr.Address3[0]
						);

				pFtResp->Status = pAuthFrame->StatusCode;
				pFtResp->flags = UMAC_FT_FLAG_AUTH_OVER_AIR_BIT;

				if (pFtResp->Status == 0) {
					pFtResp->ies_len = GET_RX_DESC_FRAME_LENGTH(pRxDescriptor)
					    - sizeof(WFM_UMAC_FRAME_HDR) - 6;
					OS_MemoryCopy(
							&pFtResp->ies[0],
							&pAuthFrame->IEs[0],
							pFtResp->ies_len
							);
				} else {
					pFtResp->ies_len = 0;
				}

				if (pUmacInstance->FtFlags & UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT) {
					pFtResp->flags |= UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT;

					pUmacInstance->FtFlags &= ~(UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT | UMAC_FT_FLAG_AUTH_OVER_AIR_BIT);

					UMAC_SET_STATE(
							UmacHandle,
							UMAC_AUTHENTICATED
							);
				} else if (pUmacInstance->FtFlags & UMAC_FT_FLAG_AUTH_OVER_AIR_BIT)
					FtJoinBack = 1;

				/*
				  Cancel the UMAC Timer as we got a valid
				  response
				*/
				if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer))
					LOG_EVENT(DBG_WFM_ERROR, "Failed to Cancel UMAC Timer\n");
			}
		}
	}

	if (FtJoinBack) {
		SMGMT_REQ_PARAMS *pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;
		/* Mark that we are doing an over the air auth */
		if (!pSmgmtReq->HandleMgmtReq) {
			pSmgmtReq->HandleMgmtReq = 1;
			pSmgmtReq->Type = SMGMT_REQ_TYPE_FT_AUTH_JOINBACK;
			pSmgmtReq->ReqNum = 0;

			pUmacInstance->FtFlags |= UMAC_FT_FLAG_AUTH_JOINBACK_BSS_BIT;
			Result = UMAC_ManageAsynMgmtOperations(UmacHandle);
			if (Result != WFM_STATUS_SUCCESS)
				goto exit_handler;
		}
		pSmgmtReq->HandleMgmtReq = 0;
	}

 ft_exit:
	if (pRxDescriptor) {
		WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

		OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
		pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
		OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

		/*Release the RxDesc and RxBuffer */
		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
		UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);
	}
	if (!FtJoinBack) {
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_FT_AUTH_RESPONSE,
					sizeof(*pFtResp),
					(uint8 *) pFtResp,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
	}

 exit_handler:
#endif	/* FT_SUPPORT */
	return Result;
} /* end UMAC_ProcFtAuthComplete() */

/******************************************************************************
 * NAME:	UMAC_ProcExtMIBReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the Write/read operations triggered by upper latyer
 * to WSM. Examples of such activities are configuraiton request,Memory Read,
 * Memory Write, ADD Key request ..etc.
 * The purpose of having this handler is to serialize these externaly triggered
 * operations.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg	     - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcExtMIBReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 Result = WFM_STATUS_SUCCESS;
	uint32 SetParamStatus = UMAC_INT_STATUS_SUCCESS;
	WFM_HI_MESSAGE *pHiMsg = (WFM_HI_MESSAGE *) pMsg;
	WFM_CONFIG_CNF *pConfigCnf = NULL;
	SMGMT_REQ_PARAMS *pSmgmtReq;

#if P2P_MINIAP_SUPPORT
	pUmacInstance->linkId = (uint8) ((((UMAC_RX_DESC *) pMsg)->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);

	if (pUmacInstance->operatingMode <= OPERATING_MODE_P2P_CLIENT)
		pUmacInstance->linkId = 0;
#endif				/*P2P_MINIAP_SUPPORT */

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;
		pSmgmtReq->Type = SMGMT_REQ_TYPE_HOST_UL_TRIGGERED;
		pSmgmtReq->ReqNum = 0;

		switch (pHiMsg->MessageId & UMAC_HI_MSG_ID) {
		case WFM_SET_PARAM_REQ_ID:
				pSmgmtReq->operation.ul_triggered_params.Activity = pHiMsg->PayLoad.wfm_set_param_req.ParameterId;
				pSmgmtReq->operation.ul_triggered_params.Category = WFM_SET_PARAM_REQ_ID;
				SetParamStatus = UMAC_ProcessSetMibRequests(UmacHandle, &pHiMsg->PayLoad.wfm_set_param_req);

				break;

		case WFM_GET_PARAM_REQ_ID:
				pSmgmtReq->operation.ul_triggered_params.Activity = pHiMsg->PayLoad.wfm_set_param_req.ParameterId;
				pSmgmtReq->operation.ul_triggered_params.Category = WFM_GET_PARAM_REQ_ID;

				break;

		case WFM_MEMORY_WRITE_REQ_ID:
				pSmgmtReq->operation.ul_triggered_params.Category = WFM_MEMORY_WRITE_REQ_ID;
				pSmgmtReq->operation.ul_triggered_params.pMsg = (uint8 *) pHiMsg;

				break;

		case WFM_MEMORY_READ_REQ_ID:
				pSmgmtReq->operation.ul_triggered_params.Category = WFM_MEMORY_READ_REQ_ID;
				pSmgmtReq->operation.ul_triggered_params.pMsg = (uint8 *) pHiMsg;

				break;

		case WFM_CONFIG_REQ_ID:
			pSmgmtReq->operation.ul_triggered_params.Category = WFM_CONFIG_REQ_ID;

			pSmgmtReq->operation.ul_triggered_params.pConfigReq = (uint8 *) &pHiMsg->PayLoad.wfm_config_req;
			pSmgmtReq->operation.ul_triggered_params.pConfigHiMsg = (uint8 *) pHiMsg;

			pUmacInstance->gVars.p.dot11RTSThreshold = pHiMsg->PayLoad.wfm_config_req.dot11RtsThreshold ? pHiMsg->PayLoad.wfm_config_req.dot11RtsThreshold : WFM_DEFAULT_RTS_THRESHOLD;
			break;

		default:
			LOG_EVENT(DBG_WFM_ERROR, "\n Bad parameters\n");
			Result = WFM_STATUS_BAD_PARAM;
		}		/*switch(pHiMsg->MessageId & UMAC_HI_MSG_ID) */

		if (SetParamStatus == UMAC_INT_STATUS_SUCCESS) {

			Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

			if (Result != WFM_STATUS_SUCCESS)
				goto exit_handler;

		} else
			Result = umac_interpret_int_status(SetParamStatus);


	} else {
		switch (pHiMsg->MessageId & UMAC_HI_MSG_ID) {
		case WFM_SET_PARAM_REQ_ID:
			pHiMsg->PayLoad.wfm_set_param_cnf.ParameterId = pHiMsg->PayLoad.wfm_set_param_req.ParameterId;
			Result = UMAC_ProcessSetMibCnf(UmacHandle, &pHiMsg->PayLoad.wfm_set_param_cnf);
			break;

		case WFM_GET_PARAM_REQ_ID:
				pHiMsg->PayLoad.wfm_get_param_cnf.ParameterId = pHiMsg->PayLoad.wfm_get_param_req.ParameterId;
				UMAC_ProcessGetMibCnf(UmacHandle, &pHiMsg->PayLoad.wfm_get_param_cnf);
				break;

		case WFM_MEMORY_WRITE_REQ_ID:
				pHiMsg->PayLoad.wfm_mem_write_cnf.Result = pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus;
				break;

		case WFM_MEMORY_READ_REQ_ID:
			{
				uint8 Length = sizeof(pHiMsg->PayLoad.wfm_mem_read_cnf.Length);
				WFM_HI_MESSAGE *pMsg = (WFM_HI_MESSAGE *) pUmacInstance->RecvData;
				pHiMsg->PayLoad.wfm_mem_read_cnf.Length = pMsg->MessageLength - sizeof(HI_MSG_HDR)
				    - sizeof(pHiMsg->PayLoad.wfm_mem_read_cnf.Result);

				OS_MemoryCopy(
					((uint8 *) &pHiMsg->PayLoad.wfm_mem_read_cnf + Length),
					((uint8 *) pUmacInstance->RecvData + sizeof(HI_MSG_HDR)),
					(pMsg->MessageLength - sizeof(HI_MSG_HDR))
				    );

				break;
			}

		case WFM_CONFIG_REQ_ID:
				pConfigCnf = (WFM_CONFIG_CNF *) &pHiMsg->PayLoad.wfm_config_cnf;
				pConfigCnf->Result = umac_interpret_wsm_status(pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus);
				OS_MemoryCopy(pConfigCnf->dot11StationId, pUmacInstance->MacAddress, 6);
				break;

		default:
			LOG_EVENT(DBG_WFM_ERROR, "\n Bad parameters\n");
			Result = WFM_STATUS_BAD_PARAM;
		}		/*switch(pHiMsg->MessageId & UMAC_HI_MSG_ID) */
	}

	pSmgmtReq->HandleMgmtReq = 0;

 exit_handler:
	return Result;
} /* end UMAC_ProcExtMIBReq() */

/******************************************************************************
 * NAME:	UMAC_ProcTxData
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes a data packet to be transmitted.
 * It does the following things.
 * -# Converts Ethernet frames to 802.11 format
 * -# Maps packet to the correct Access category based on the priority
 *    information provided.
 * -# Allocated a TxDescriptor to queue this frame for transmission.
 * -# Stores the reference number of the packet , so that it can be given back
 *    to the host with Tx confirmation.
 * -# Leaves space for the Encryption type used , if there is one.
 * -# Queues the 802.11 frame to LMAC for transmission.
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcTxData(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint16 EthFrmLen;
	UMAC_TX_DESC *pTxDescriptor;
	WFM_UMAC_DATA_FRAME *pTxFrame;
	uint32 Reference;
	uint8 PriorityQ;
	uint32 Priority;
	UMAC_TX_DATA *pTxDataReq;
	UMAC_DOT11_FRAME_INFO FrameInfo = {0};
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	UMAC_TX_PKT *pUmacTxPkt;
	uint32 Flags;
	uint16 EtherType;
	uint8 linkId;

	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_TX, "UMAC_ProcTxData()\n");

	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pMsg);

	pTxDataReq = (UMAC_TX_DATA *) pMsg;

	Flags = pTxDataReq->pUlHdr->Flags;

	EtherType = *(uint16 *)(pTxDataReq->pEtherHeader + 12);

#if P2P_MINIAP_SUPPORT
	linkId = (uint8) (Flags & 0x0000000F);
#else
	linkId = 0;
#endif				/*P2P_MINIAP_SUPPORT */

#if P2P_MINIAP_SUPPORT
	if (((pUmacInstance->sta[linkId].state == STA_STATE_DEAUTHENTICATING) || (pUmacInstance->sta[linkId].state == STA_STATE_NONE))
		&& (linkId != 0) && (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		) {
			Result = WFM_STATUS_REQ_REJECTED;
			goto exit_handler;
	}
	if ((linkId == 0) && (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_DOZE)
		&& (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		) {
			OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
			if (pUmacInstance->bitmapCtrl == 0) {
				pUmacInstance->bitmapCtrl = 1;
				if (pUmacInstance->updateTimIeInProgress == FALSE) {
					pUmacInstance->updateTimIeInProgress = TRUE;
					OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
					WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);
				} else
					OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
			} else
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
	}
#endif	/* P2P_MINIAP_SUPPORT */

	EthFrmLen = pTxDataReq->pUlHdr->MessageLength - (WFM_HI_MESSAGE_HDR_SIZE + WFM_TX_REQ_HDR_SIZE);

	Reference = pTxDataReq->pUlHdr->Reference;

	Flags = pTxDataReq->pUlHdr->Flags;

	/*
	   Taking a backup of Priority Info, as we may over write this memory
	   location while performing Ethernet-WLAN packet conversion
	 */
	Priority = (pTxDataReq->pUlHdr->Priority & 0x07);

	/* Downgrade priority if required due to ACM */
	FrameInfo.tid = pUmacInstance->PrioDowngradeMap[Priority];
	Priority = FrameInfo.tid;

	/* converting Ethernet frames to 802.11 format */

	pTxFrame = (WFM_UMAC_DATA_FRAME *) WFM_UMAC_Data_Ethernet_2_Dot11(
								UmacHandle,
								pTxDataReq,
								&FrameInfo,
								linkId
								);

	pTxDescriptor = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_TX_DESC(
								UmacHandle,
								pTxFrame
								);

	if (NULL == pTxDescriptor) {
		LOG_EVENT(DBG_WFM_ERROR, "Failed to Allocate slote in TxDescStore\n");
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	CHECK_POINTER_IS_VALID(pTxDescriptor);

	CHECK_POINTER_FOR_ALIGNMENT(pTxDescriptor);	/* to be removed */

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *) ALLOCATE_UMC_DEV_IF_TX_REQ(pTxDescriptor);

	CHECK_POINTER_IS_VALID(pDevIfTxReq);

	CHECK_POINTER_FOR_ALIGNMENT(pDevIfTxReq);	/* to be removed */

	UMAC_GET_PACKET_ID(UmacHandle, pTxDescriptor->PacketId);

	if (pTxDescriptor->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, "UMAC_ProcTxData() : Transmit Queue Full\n");
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pUmacTxPkt = (UMAC_TX_PKT *)
		&pUmacInstance->gVars.TxDescStore[pTxDescriptor->PacketId];

#if DOT11K_SUPPORT
	/*
	  Here check for the measurement.If under process then set the 10th bit
	  of packet ID and get the current time */
	if (((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) || (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION)) &&
		(pUmacInstance->dot11k_Meas_Params.transmitStreamUnderProcess)) {

		/*Check if the data packet has the requested TID and Peer Address */
		TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ *pTxStreamReq = (TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ *)&pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq[0];
		uint8 BroadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		if ((FrameInfo.tid == pTxStreamReq->trafficIdentifier) && ((OS_MemoryEqual(&pTxFrame->dot11hdr.Address1[0], 6, &pTxStreamReq->peerSTAAddress[0], 6)) || (OS_MemoryEqual(&pTxFrame->dot11hdr.Address1[0], 6, &BroadcastAddr[0], 6)))
		    ) {
			pUmacInstance->dot11k_Meas_Params.txStreamResults.CurrentTime[pTxDescriptor->PacketId] = OS_GetTime();
			pTxDescriptor->PacketId |= UMAC_PACKET_ID_MASK;
		}
	}
#endif	/* DOT11K_SUPPORT */

	if (pUmacTxPkt->pTxDesc == NULL) {
		OS_LOCK(pUmacInstance->gVars.p.TxDescStoreLock);

		pUmacTxPkt->pTxDesc = pTxDescriptor;
		pUmacTxPkt->reference = Reference;
		pUmacTxPkt->pMsgPtr = (void *)pMsg;
		pUmacTxPkt->linkId = linkId;
		pUmacTxPkt->pDevIfTxReq = (void *)pDevIfTxReq;
		OS_UNLOCK(pUmacInstance->gVars.p.TxDescStoreLock);

	} else {
		LOG_EVENT(DBG_WFM_UMAC, "Failed to find a slote in TxDescStore\n");
		/*
		   We failed to get a TxDesc Store, so releasing the Tx
		   Descriptor Allocated.Cant proceed with this Tx now.
		*/
		UMAC_LL_RELEASE_TX_DESC(UmacHandle, pTxDescriptor);

#if DOT11K_SUPPORT
		if (pTxDescriptor->PacketId & UMAC_PACKET_ID_MASK)
			pTxDescriptor->PacketId &= ~(UMAC_PACKET_ID_MASK);
#endif	/* DOT11K_SUPPORT */

		UMAC_RELEASE_PACKET_ID(UmacHandle, pTxDescriptor->PacketId);

		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}


	/* Mapping 802.3 Priority to 802.11 Priority Queues */
	PriorityQ = pUmacInstance->QosPrio2Queue[Priority];

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDescriptor);

	ASSIGN_FRAME_LENGTH(
			pTxDescriptor,
			(EthFrmLen + (uint16) FrameInfo.OffsetLength + (uint16) FrameInfo.EncTagSize)
	    );

	ASSIGN_FRAME_TO_TX_DESC(pTxDescriptor, (uint8 *) pTxFrame);

	/*Check alignment */
	CHECK_POINTER_FOR_ALIGNMENT(TX_DESC_GET_DOT11FRAME(pTxDescriptor));

#if P2P_MINIAP_SUPPORT
	if (linkId)
		pTxDescriptor->MaxTxRate = (uint8) pUmacInstance->sta[linkId].currentRateIndex;
	else if (!linkId  && (pUmacInstance->operatingMode >= OPERATING_MODE_AP))
		pTxDescriptor->MaxTxRate = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);
	else {
#endif	/* P2P_MINIAP_SUPPORT */
		if (pUmacInstance->isArpFilterEnabled)
			pTxDescriptor->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;
		else
			ASSIGN_MGT_FRAME_TX_RATE(pTxDescriptor, pUmacInstance);
	}

	pTxDescriptor->QueueId = PriorityQ;

	pTxDescriptor->More = 0;

	if (FrameInfo.FrmCtrlOffsetFalg == TRUE)
		pTxDescriptor->Flags = UMAC_TX_REQ_FLAG_FRAME_CTRL_OFFSET_ENABLED;
	else
		pTxDescriptor->Flags            = 0x00;

#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		pTxDescriptor->Flags |= linkId<<4;
	else
#endif	/* P2P_MINIAP_SUPPORT */
		if (pUmacInstance->TxRetryRateBitMap[1] && (pTxDescriptor->MaxTxRate < RATE_INDEX_N_6_5M))
			pTxDescriptor->Flags |= (1 << 4);


	if (EtherType == be16_to_cpu(UMAC_EAPOL_ETHER_TYPE))
		ASSIGN_TX_PTA_PRIORITY(pTxDescriptor, PTA_PRIORITY_EAPOL);
	else if (pTxDescriptor->QueueId == WFM_AC_VO)
		ASSIGN_TX_PTA_PRIORITY(pTxDescriptor, PTA_PRIORITY_VOICE);
	else if (pTxDescriptor->QueueId == WFM_AC_VI)
		ASSIGN_TX_PTA_PRIORITY(pTxDescriptor, PTA_PRIORITY_VIDEO);
	else
		ASSIGN_TX_PTA_PRIORITY(pTxDescriptor, PTA_PRIORITY_DATA);

	pTxDescriptor->Reserved = 0;
	pTxDescriptor->HtTxParameters = pUmacInstance->WfmHtTxParameters;
	pTxDescriptor->ExpireTime = WFM_UMAC_TX_EXPIRE_TIME;

	LOG_BYTES(DBG_WFM_TX, "WFM_Tx-Wlan: ", TX_DESC_GET_DOT11FRAME(pTxDescriptor), GET_TX_DESC_FRAME_LENGTH(pTxDescriptor), GET_TX_DESC_FRAME_LENGTH(pTxDescriptor));

	pDevIfTxReq->HeaderLength = FrameInfo.OffsetLength + sizeof(UMAC_TX_DESC);
	pDevIfTxReq->pTxDescriptor = pTxDescriptor;
	pDevIfTxReq->pDot11Frame = pTxDataReq->pEtherHeader;
	pDevIfTxReq->UlInfo = pTxDataReq->UlInfo;

	/* Send Tx Req to LMAC */
#if P2P_MINIAP_SUPPORT
	if ((pUmacInstance->operatingMode == OPERATING_MODE_STA) || (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_AWAKE)) {
#endif	/* P2P_MINIAP_SUPPORT */
		uint8 DilQ;

		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[linkId]++;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		DilQ = pUmacInstance->AC2DilQ[pTxDescriptor->QueueId];
		Result = UMAC_LL_REQ_TX_FRAME(UmacHandle, DilQ, pDevIfTxReq, linkId);

		if (Result != WFM_STATUS_SUCCESS) {
			/*Remove the entry from TxDescStore */
			pUmacTxPkt->pTxDesc = NULL;
			UMAC_LL_RELEASE_TX_DESC(UmacHandle, pTxDescriptor);
#if DOT11K_SUPPORT
			if (pTxDescriptor->PacketId & UMAC_PACKET_ID_MASK)
				pTxDescriptor->PacketId &= ~(UMAC_PACKET_ID_MASK);

#endif	/* DOT11K_SUPPORT */

			UMAC_RELEASE_PACKET_ID(UmacHandle, pTxDescriptor->PacketId);
			WFM_UMAC_DBG_REMOVE_TX_DESC(pTxDescriptor);

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
		} /* if(Result != WFM_STATUS_SUCCESS) */
#if P2P_MINIAP_SUPPORT
	} else {
		/* Buffer the frame, as the destination STA is in PS mode */
		uint8 bResp = FALSE;
		bResp = UMAC_Buffer_Frame(UmacHandle, linkId, pTxDescriptor, 0, NULL);
		if (!bResp) {
			/* Allocated buffer is full for this STA so drop the packet */
			Result = WFM_STATUS_OUT_OF_RESOURCES;
			pUmacTxPkt->pTxDesc = NULL;
			UMAC_LL_RELEASE_TX_DESC(UmacHandle, pTxDescriptor);
			UMAC_RELEASE_PACKET_ID(UmacHandle, pTxDescriptor->PacketId);
		}
	}
#endif	/* P2P_MINIAP_SUPPORT */
 exit_handler:
	return Result;
} /* end UMAC_ProcTxData() */

/******************************************************************************
 * NAME:	UMAC_ProcRxData
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes an incoming data packet.
 * It does the following things.
 * -# Skip the encryption headers.
 * -# Converts 802.11 frames to Ethernet format.
 * -# Releases the RxDescriptor allocated for the frame.
 * -# Add HI headers to the packet and send it up to the host.
 * The Rx buffer will be freed once HI has uploaded the packet to host.
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcRxData(UMAC_HANDLE UmacHandle, void *pMsg)
{
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	uint32 Result = WFM_STATUS_SUCCESS;
	uint32 Flags = 0;
	uint8 EncHdrSize = 0;
	uint8 EncTagSize = 0;
	uint16 NoOfMsdu = 0;
	uint8 AmsduPresent = 0;
	uint8 isBroadMulticastPkt = FALSE;
	uint16 HdrLen, PayLoadLen = 0, Dot3FrmLen, SubFrmLen;
	uint8 *pPayLoad = NULL, *pDot3Frm = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	uint8 linkId;
	uint8 RxedRate;
#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
#else
	linkId = 0;
#endif	/*P2P_MINIAP_SUPPORT */

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC, "UMAC_ProcRxData()\n");

	OS_ASSERT(pMsg);
	CHECK_POINTER_IS_VALID(pMsg);

	pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	RxedRate = pRxDescriptor->RxedRate;

	if (pRxDescriptor->Status == WSM_STATUS_SUCCESS) {
		switch ((pRxDescriptor->Flags & 0x00000007)) {
		case WSM_RI_FLAGS_UNENCRYPTED:
			LOG_DATA2(DBG_WFM_UMAC | DBG_WFM_RX | DBG_WFM_ENCRYPTION, "Normal-RX, EncType=%d, EncKeyAvail=%d\n", pUmacInstance->gVars.p.InfoHost.EncryptionType, pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId]);
			break;

		case WSM_RI_FLAGS_WEP_ENCRYPTED:
			EncHdrSize = 4;
			EncTagSize = 4;
			LOG_DATA2(DBG_WFM_UMAC | DBG_WFM_RX | DBG_WFM_ENCRYPTION, "WEP-RX, EncType=%d, EncKeyAvail=%d\n", pUmacInstance->gVars.p.InfoHost.EncryptionType, pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId]);
			break;

		case WSM_RI_FLAGS_TKIP_ENCRYPTED:
			EncHdrSize = 8;
			EncTagSize = 12;
			LOG_DATA2(DBG_WFM_UMAC | DBG_WFM_RX | DBG_WFM_ENCRYPTION, "TKIP-RX, EncType=%d, EncKeyAvail=%d\n", pUmacInstance->gVars.p.InfoHost.EncryptionType, pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId]);
			break;

		case WSM_RI_FLAGS_AES_ENCRYPTED:
			EncHdrSize = 8;
			EncTagSize = 8;
			LOG_DATA2(DBG_WFM_UMAC | DBG_WFM_RX | DBG_WFM_ENCRYPTION, "AES-RX, EncType=%d, EncKeyAvail=%d\n", pUmacInstance->gVars.p.InfoHost.EncryptionType, pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId]);
			break;

#if WAPI_SUPPORT
		case WSM_RI_FLAGS_WAPI_ENCRYPTED:
			EncHdrSize = 18;
			EncTagSize = 16;
			LOG_DATA2(DBG_WFM_UMAC | DBG_WFM_RX | DBG_WFM_ENCRYPTION, "WAPI-RX, EncType=%d, EncKeyAvail=%d\n", pUmacInstance->gVars.p.InfoHost.EncryptionType, pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId]);
			break;
#endif				/* WAPI_SUPPORT */

		default:
			/*will never reach here */
			;	/* empty statement */
		}		/*switch( (pRxDescriptor->Flags  & 0x00000003) ) */

		LOG_BYTES(DBG_WFM_RX, "WFM_Rx: ", RX_DESC_GET_DOT11FRAME(pRxDescriptor), (uint16) GET_RX_DESC_FRAME_LENGTH(pRxDescriptor), (uint16) GET_RX_DESC_FRAME_LENGTH(pRxDescriptor));

		/*Passing Dot11 frame and its length */
		PayLoadLen = WFM_UMAC_Parse_Dot11_Header(
				RX_DESC_GET_DOT11FRAME(pRxDescriptor),
				GET_RX_DESC_FRAME_LENGTH(pRxDescriptor),
				EncHdrSize,
				EncTagSize,
				&HdrLen,
				&AmsduPresent
				);

		if (IS_MULTICAST_MAC_ADDRESS(((WFM_UMAC_DATA_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor))->dot11hdr.Address1)
			|| IS_BROADCAST_MAC_ADDRESS(((WFM_UMAC_DATA_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor))->dot11hdr.Address1))
			isBroadMulticastPkt = TRUE;


		if (AmsduPresent) {
			uint16 i, DataLen;

			pPayLoad = RX_DESC_GET_DOT11FRAME(pRxDescriptor) + HdrLen;
			DataLen = PayLoadLen;

			NoOfMsdu = WFM_UMAC_Data_Dot11Amsdu_Count_Subfrm(pPayLoad, PayLoadLen);
			Flags = ((0x1 << 15) | NoOfMsdu);	/* MoreData Bit Set */

			for (i = NoOfMsdu; i > 0; i--) {
				pDot3Frm = WFM_UMAC_Data_Dot11AmsduSubFrm_2_Dot3Frm(
								pPayLoad,
								DataLen,
								&Dot3FrmLen,
								&SubFrmLen
								);

				if (i == 1) {
					/*
					   Reset MoreData bit for the last msdu
					 */
					Flags = NoOfMsdu;
				}

				pUmacInstance->gVars.p.UpperLayerRxFrmCb(
						pUmacInstance->gVars.p.ulHandle,
						WFM_STATUS_SUCCESS,
						pDot3Frm,
						Dot3FrmLen,
						pUmacInstance->pDriverInfo,
						pMsg,
						Flags
						);
				pPayLoad += SubFrmLen;
				DataLen -= SubFrmLen;
			}

		} else {
			WFM_UMAC_DATA_FRAME *pUmacDataFrame;

			pUmacDataFrame = (WFM_UMAC_DATA_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
			if (PayLoadLen) {
				Flags = 1;
#if P2P_MINIAP_SUPPORT
				if (pUmacInstance->gVars.p.IntraBssBridging && IS_MULTICAST_MAC_ADDRESS(pUmacDataFrame->dot11hdr.Address3))
					Flags |= UMAC_MC_BC_FRAME;
#endif	/* P2P_MINIAP_SUPPORT */
				pDot3Frm = WFM_UMAC_Data_Dot11_2_Ethernet(
						(uint8 *) pUmacDataFrame,
						(uint16) GET_RX_DESC_FRAME_LENGTH(pRxDescriptor),
						&Dot3FrmLen,
						HdrLen
						);

				Dot3FrmLen -= EncTagSize;
				pUmacInstance->gVars.p.UpperLayerRxFrmCb(
						pUmacInstance->gVars.p.ulHandle,
						WFM_STATUS_SUCCESS,
						pDot3Frm,
						Dot3FrmLen,
						pUmacInstance->pDriverInfo,
						pMsg,
						Flags
						);
			}
		}
		pUmacInstance->CheckForConnectionDataRxCtr++;

		if (pUmacInstance->gVars.p.RcpiRssiThreshold.RssiRcpiMode & 0x02) {
			pUmacInstance->RxRcpi = 2 * (pRxDescriptor->Rcpi + 110);
			pUmacInstance->RxRSSI = pRxDescriptor->Rcpi;
		} else {
			pUmacInstance->RxRcpi = pRxDescriptor->Rcpi;
			pUmacInstance->RxRSSI = (sint8) ((pRxDescriptor->Rcpi / 2) - 110);
		}

		if (pUmacInstance->CheckGFProblem) {
			if (!isBroadMulticastPkt)
				pUmacInstance->RxPacketCnt++;

			if (RxedRate > 3) {
				pUmacInstance->CheckGFProblem = 0;
				pUmacInstance->RxPacketCnt = 0;
			} else if (pUmacInstance->RxPacketCnt >= 20) {
				pUmacInstance->CheckGFProblem = 0;
				pUmacInstance->RxPacketCnt = 0;
				pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_HT_DISABLE_GREENFIELD_MODE;

				UMAC_SET_STATE(UmacHandle, UMAC_AUTHENTICATED);
				WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_REASSOC, NULL);
			}
		}
	} else {
		if (pRxDescriptor->Status == WSM_STATUS_MICFAILURE) {
			EVT_DATA_MIC_FAILURE TcM;
			WFM_UMAC_FRAME_HDR *pDot11hdr;
			uint16 OffsetToIv;
			uint8 SubType;
			uint8 *ptrIv;

			LOG_EVENT(DBG_WFM_ALWAYS, "UMAC_ProcRxData(): WFM_EVT_MICFAILURE\n");

			pUmacInstance->MicFailureCount++;

			pDot11hdr = (WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);

			OffsetToIv = sizeof(WFM_UMAC_FRAME_HDR);
			/*b4,b5,b6 and b7 forms SubType */
			SubType = (uint8) ((pDot11hdr->FrameControl >> 4) & 0x000F);

			if (WFM_UMAC_SUB_DATA_QOS == SubType)
				OffsetToIv = (OffsetToIv + 2);

			ptrIv = (uint8 *) (pDot11hdr + OffsetToIv);

			TcM.Rsc[0] = (uint8) ptrIv[0];
			TcM.Rsc[1] = (uint8) ptrIv[2];

			OS_MemoryCopy(
					(uint8 *) &TcM.Rsc[2],
					(ptrIv + 4),
					sizeof(uint32)
					);

			TcM.MicFailureCount = pUmacInstance->MicFailureCount;

			D0_ADDR_COPY(
					TcM.PeerAddress,
					pUmacInstance->RemoteMacAdd
					);

			if ((pRxDescriptor->Flags >> 19) & 0x00000001)
				TcM.IsPairwiseKey = 0;
			else
				TcM.IsPairwiseKey = 1;


			TcM.KeyIndex = (uint8) ((pRxDescriptor->Flags >> 20) & 0x0000000F);

			/* Inform the host that we received mic failure */
			WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_MICFAILURE,
						sizeof(EVT_DATA_MIC_FAILURE),
						(uint8 *) &TcM,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);

		} else if (pRxDescriptor->Status == WSM_STATUS_DECRYPTFAILURE) {
			LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ALWAYS, "UMAC_ProcRxData(): Received WSM_STATUS_DECRYPTFAILURE\n");
		} else if (pRxDescriptor->Status == WSM_STATUS_NO_KEY_FOUND) {
			LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ALWAYS, "UMAC_ProcRxData(): Received WSM_STATUS_NO_KEY_FOUND\n");
		} else {
			LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ALWAYS, "UMAC_ProcRxData(): General Failure\n");
		}

		HdrLen = GET_RX_DESC_FRAME_LENGTH(pRxDescriptor);
		pDot3Frm = WFM_UMAC_Data_Dot11_2_Ethernet(
				RX_DESC_GET_DOT11FRAME(pRxDescriptor),
				(uint16) GET_RX_DESC_FRAME_LENGTH(pRxDescriptor),
				&Dot3FrmLen,
				HdrLen
				);
		Flags = 1;
		pUmacInstance->gVars.p.UpperLayerRxFrmCb(
					pUmacInstance->gVars.p.ulHandle,
					WFM_STATUS_FAILURE,
					pDot3Frm,
					Dot3FrmLen,
					pUmacInstance->pDriverInfo,
					pMsg,
					Flags
					);
	}

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[linkId]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	return Result;
} /* end UMAC_ProcRxData() */

/******************************************************************************
 * NAME:	UMAC_ProcBeacon
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the beacon frames we are getting as a result of the
 *passive scan. All the necessory IE's are processed here and the prob response
 * will be stored in the device cache, if there is space available, otherwise
 * the oldest entry in the device cache will be uploaded to host to make space
 * for the current element and will be stored.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcBeacon(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 status;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "UMAC_ProcBeacon()\n");

	OS_ASSERT(pMsg);

	/*
	   note: we are using three states: UmacScanState, UmacFindState
	   and UmacCurrentState
	*/
	if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED)
	    || (pUmacInstance->UmacScanState == UMAC_SCANNING)
#if P2P_MINIAP_SUPPORT
	    || (pUmacInstance->UmacFindState == UMAC_FINDING)
#endif	/* P2P_MINIAP_SUPPORT */
	    ) {

		LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "UMAC_ProcBeacon(): New Beacon\n");

		/* 1. Store it in UMAC cache */

		LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "UMAC_ProcBeacon(): Sending beacon to host\n");

		if (!pUmacInstance->IsInternalScan)
			UMAC_Send_Beacons_ProbRsp_To_UpperLayer(UmacHandle);

		/* 3. Parse beacon (Start/Stop ERP protection) */
		status = WFM_UMAC_ProcessBeaconOrProbRspFrame(UmacHandle, pMsg, 1);

		if (WFM_BEACON_PROBRSP_INTERNAL_SCAN_GOT_RESULT == status) {
			/* We can stop the internal scan now and start the
			   joining procedure
			 */

			WFM_BSS_INFO *pBssElement = (WFM_BSS_INFO *) &pUmacInstance->bssInfo[0];
			WFM_BSS_LIST_SCAN *pbssListScan = (WFM_BSS_LIST_SCAN *) &pUmacInstance->InternalScanMsg[0];

			pbssListScan->Flags = 0;

			/* Schedule to join with the AP */
			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_JOIN, pBssElement);

		} else if (WFM_BEACON_PROBRSP_LIST_UPDATED == status)
			UMAC_Send_Beacons_ProbRsp_To_UpperLayer(UmacHandle);
	} else if ((pUmacInstance->UmacCurrentState == UMAC_BSS_STARTED)
		|| (pUmacInstance->UmacCurrentState == UMAC_BSS_STARTING)) {
			OS_LOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
			WFM_UMAC_ProcessErpProtection(UmacHandle, pMsg, TRUE);
			if ((pUmacInstance->updateERPIeInProgress == FALSE)&& (pUmacInstance->ErpIeToBeUpdated == TRUE)) {
				pUmacInstance->updateERPIeInProgress = TRUE;
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
				/* Update the beacon/probe response for ERP protection */
				WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_ERP_IE, NULL);
			} else {
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
			}

	} else {
		LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "UMAC_ProcBeacon(): NOT Sending beacon to host\n");
		Result = WFM_STATUS_BAD_PARAM;
	}

	WFM_UMAC_DBG_REMOVE_RX_DESC(pMsg);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, (UMAC_RX_DESC *) pMsg);

	return Result;
} /* end UMAC_ProcBeacon() */

/******************************************************************************
 * NAME:UMAC_ProcDeAssoc
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initiates the Disassociation procedure.It
 * -# Allocates TxFrameBuffer and forms Disassociation frame based on the
 *    parameters available with UMAC for the current link.
 * -# Selects the lowest rate available for transmission as the current rate.
 * -# Queues the Disassoc frame to LMAC for transmission.
 * -# Changes UMAC state to UMAC_AUTHENTICATED.
 * -# Triggers the De-Authentication procedure.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcDeAssoc(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	/* UMAC_DEVIF_TX_DATA *pDevIfTxReq; */
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	/* uint8 Encrypt; */

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcDeAssoc()\n");

	/*
	  Check if roaming indication is TRUE.If driver has taken decision for
	  disconnect after receving bsslost,rcpirssi or TxFailure threshold
	  count indication then reset bRoamIndRcvd
	*/

	/*Reset the events of roaming */
	pUmacInstance->RssiRcpiThresholdEventRcvd = 0;
	pUmacInstance->BssLostEventRcvd = 0;
	pUmacInstance->TxFailureEventRcvd = 0;
	pUmacInstance->BssRegainedEventRcvd = 0;


	/* Reset P2P PS OID and Power Save Mode as Active */
	if (pUmacInstance->operatingMode < OPERATING_MODE_AP && pUmacInstance->p2pPsSet) {
		pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;
		if (!pSmgmtReq->HandleMgmtReq) {
			pUmacInstance->gVars.p.PsMode.PmMode = WFM_PS_MODE_DISABLED;
			OS_MemoryReset(&pUmacInstance->gVars.p.p2pPsMode, sizeof(UMAC_P2P_PS_MODE));
			OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
			pSmgmtReq->HandleMgmtReq = 1;
			pSmgmtReq->Type = SMGMT_REQ_TYPE_SEND_DEASSOC;
			pSmgmtReq->ReqNum = 0;
			Result = UMAC_ManageAsynMgmtOperations(UmacHandle);
			if (Result != WFM_STATUS_SUCCESS)
				goto exit_handler;
		}
		pSmgmtReq->HandleMgmtReq = 0;
	}

	if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode) {
		UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNJOIN, NULL);
		goto exit_handler;
	}			/*if(WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode) */

#if 0
	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_DEASSOC_REQ);

	LOG_DATA(DBG_WFM_ASSOC, "UMAC_ProcDeAssoc pTxDesc=0x%x)\n", pTxDesc);
#endif
	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer);
	OS_STOP_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer
		);
#if 0
	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	pDeAssocFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pDeAssocFrame);

	Encrypt = UMAC_ProtectionRequired(pUmacInstance, 0, WFM_UMAC_MGMT, WFM_UMAC_MGMT_DISASSOC);
	/* Creating the WLAN Header */
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_DISASSOC,
					&pDeAssocFrame->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					Encrypt
					);

	FrameBodyLen = WFM_UMAC_CreateDisAssocReq(
							UmacHandle,
							pDeAssocFrame->FrameBody
						 );
#endif
	UMAC_SET_STATE(UmacHandle, UMAC_DISASSOCIATING);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_DISASSOCIATING\n");

	/*Reseting association id */
	pUmacInstance->AssociationId = 0;
#if 0
	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	/*For management frames use the lowest rate */
	pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcDeAssoc() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	ASSIGN_MGT_FRAME_TX_RATE(pTxDesc, pUmacInstance);

	if (((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION) == WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION)
		|| ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) == WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION))
		pTxDesc->QueueId = 3;	/*WFM_AC_VO ; */
	else
		pTxDesc->QueueId = 0;	/*WFM_AC_BE ; */
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;
	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL
					);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/* Sending the management frame to LMAC for transmission */
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					DEFAULT_LINK_ID
					);

	if (Result != WFM_STATUS_SUCCESS)
		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);
#endif
	/* Starting De-Auth procedure */
	/* Scheduling De-Authentication */
	WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_DE_AUTHE, NULL);

 exit_handler:
	return Result;
} /* end UMAC_ProcDeAssoc() */

/******************************************************************************
 * NAME:	UMAC_ProcDeAssocSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is implemented to make the state machine symmetric.
 * Since the De-Association is always success, as there are no responces to
 * this frame, This function can be removed later.
 * -# Changes UMAC state to UMAC_AUTHENTICATED.
 * -# Triggers the De-Authentication procedure.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcDeAssocSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	UMAC_SET_STATE(UmacHandle, UMAC_AUTHENTICATED);
	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "STATE = UMAC_AUTHENTICATED\n");

	/* Starting De-Auth procedure */
	WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_DE_AUTHE, NULL);

	return Result;
} /* end UMAC_ProcDeAssocSuccess() */

/******************************************************************************
 * NAME:	UMAC_ProcDeAssocReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the disassociation request which we may receive from
 * the remote device to tear down a link.It
 * -# Changes UMAC state to UMAC_DISASSOCIATING.
 * -# Releases the memory and Rx descriptor allocated for this frame.
 * -# Calls the Disassociation Success handler through the state machine
 *    to bring back the state machine to the INITIALIZED state.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcDeAssocReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_UMAC_DISASSOC_REQ_BODY *pDisAssocFrame = NULL;
	uint8 *pFrameBody = NULL;
#if MGMT_FRAME_PROTECTION
	UMAC_SA_QUERY_FRAME_BODY SaQueryData;
#endif				/*MGMT_FRAME_PROTECTION */

	uint8 linkId;
#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
#else
	linkId = 0;
#endif	/* P2P_MINIAP_SUPPORT */

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcDeAssocReq()\n");

	OS_ASSERT(pMsg);
	CHECK_POINTER_IS_VALID(pMsg);

	pRxDescriptor = (UMAC_RX_DESC *) pMsg;

	WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[linkId]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
		/*UMAC_STA_UPDATE_STATE(
					pUmacInstance,
					linkId,
					STA_STATE_DEAUTHENTICATING
					);*/
		Result = WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_DEAUTH_STA,
							pMsg
							);
	} else {
#endif				/*P2P_MINIAP_SUPPORT */

#if MGMT_FRAME_PROTECTION
		if (pRxDescriptor->Status == WSM_STATUS_SUCCESS) {
#endif				/*MGMT_FRAME_PROTECTION */

#if MGMT_FRAME_PROTECTION
			if ((pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE)
			    && ((pRxDescriptor->Flags & 0x000F) == 0x000B)) {
				pFrameBody = (uint8 *) ((uint8 *) pRxDescriptor + sizeof(UMAC_RX_DESC)
							+ sizeof(WFM_UMAC_FRAME_HDR));
				pDisAssocFrame = (WFM_UMAC_DISASSOC_REQ_BODY *) pFrameBody;
				/*Check if Reason code is 6 or 7 */
				if ((pDisAssocFrame->ReasonCode == UMAC_REASON_CODE_6)
				    || (pDisAssocFrame->ReasonCode == UMAC_REASON_CODE_7)) {
					SaQueryData.Action = UMAC_ACTION_TYPE_SA_QUERY_REQ;
					SaQueryData.Category = UMAC_CATEGORY_SA_QUERY;
					SaQueryData.TransactionId = 0;
					pUmacInstance->protectMgmtFrame.isValidSaQueryResp = FALSE;
					/*Initiate SA Query Procedure */
					Result = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_START_SAQUERY, &SaQueryData);

					OS_START_TIMER(
						pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer,
						pUmacInstance->saQueryInterval.saQueryRetryTimeout,
						UmacHandle
						);

					pUmacInstance->protectMgmtFrame.CheckSaQueryInterval += pUmacInstance->saQueryInterval.saQueryRetryTimeout;
				}
			} else
#endif	/* MGMT_FRAME_PROTECTION */
			{
				/* Reseting association id */
				pUmacInstance->AssociationId = 0;

				UMAC_SET_STATE(UmacHandle, UMAC_DISASSOCIATING);
				LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_DISASSOCIATING\n");

				Result = WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_DE_ASSOC_SUCCESS,
							NULL
							);

				if (WFM_STATUS_PENDING == Result)
					OS_ASSERT(0);
			}
#if MGMT_FRAME_PROTECTION
		}
#endif	/* MGMT_FRAME_PROTECTION */

		WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

		/*Release the RxDesc and RxBuffer */

#if P2P_MINIAP_SUPPORT
	}
#endif	/* P2P_MINIAP_SUPPORT */
	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

	return Result;
} /* end UMAC_ProcDeAssocReq() */

/******************************************************************************
 * NAME:	UMAC_ProcDeAuthReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the De-Authentication request which we may receive
 * from the remote device.It
 * -# Changes UMAC state to UMAC_JOINED.
 * -# Releases the RxDescriptor and buffer allocated for this frame.
 * -# Triggers the UMAC_UNJOIN handler through the state machine
 *    to bring back the state machine to the INITIALIZED state.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcDeAuthReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_UMAC_DEAUTH_FRAME_BODY *pDeAuthFrame = NULL;
	uint8 *pFrameBody = NULL;
#if MGMT_FRAME_PROTECTION
	UMAC_SA_QUERY_FRAME_BODY SaQueryData;
#endif				/*MGMT_FRAME_PROTECTION */
	uint8 linkId;

#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
#else
	linkId = 0;
#endif				/*P2P_MINIAP_SUPPORT */

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcDeAuthReq()\n");

	OS_ASSERT(pMsg);
	CHECK_POINTER_IS_VALID(pMsg);
#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
		/* b0-b5 bits are set to 1 when unlink STA is called from host side
		   or from call backtimer or UMAC Internally */
		if (STA_STATE_DEAUTHENTICATING != pUmacInstance->sta[linkId].state) {
			if (linkId != 0)
				OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId]);
			pUmacInstance->sta[linkId].hiMsgHdr.MsgId = linkId<<6;
			pUmacInstance->sta[linkId].state = STA_STATE_DEAUTHENTICATING;
			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNLINK_STA, &(pUmacInstance->sta[linkId].hiMsgHdr));
		}
	} else {
#endif	/* P2P_MINIAP_SUPPORT */
#if MGMT_FRAME_PROTECTION
		if (pRxDescriptor->Status == WSM_STATUS_SUCCESS) {
#endif	/* MGMT_FRAME_PROTECTION */

#if MGMT_FRAME_PROTECTION
			if ((pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE)
			    && ((pRxDescriptor->Flags & 0x000F) == 0x000B)) {
				pFrameBody = (uint8 *) ((uint8 *) pRxDescriptor + sizeof(UMAC_RX_DESC)
							+ sizeof(WFM_UMAC_FRAME_HDR));
				pDeAuthFrame = (WFM_UMAC_DEAUTH_FRAME_BODY *) pFrameBody;
				/*Check if Reason code is 6 or 7 */
				if ((pDeAuthFrame->ReasonCode == UMAC_REASON_CODE_6)
				    || (pDeAuthFrame->ReasonCode == UMAC_REASON_CODE_7)) {
					SaQueryData.Action = UMAC_ACTION_TYPE_SA_QUERY_REQ;
					SaQueryData.Category = UMAC_CATEGORY_SA_QUERY;
					SaQueryData.TransactionId = 0;
					pUmacInstance->protectMgmtFrame.isValidSaQueryResp = FALSE;
					/*Initiate SA Query Procedure */
					Result = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_START_SAQUERY, &SaQueryData);

					OS_START_TIMER(pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer, pUmacInstance->saQueryInterval.saQueryRetryTimeout, UmacHandle);

					pUmacInstance->protectMgmtFrame.CheckSaQueryInterval += pUmacInstance->saQueryInterval.saQueryRetryTimeout;
				}
			} else
#endif				/*MGMT_FRAME_PROTECTION */
			{
				OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer);
				OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer);

				if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer))
					LOG_EVENT(DBG_WFM_ERROR, "Failed to Cancel UMAC Timer\n");

				UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
				LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_JOINED\n");

				/*Start Un-Join from here */
				WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNJOIN, NULL);
			}
#if MGMT_FRAME_PROTECTION
		}
#endif	/* MGMT_FRAME_PROTECTION */
#if 0
		/*Release the RxDesc and RxBuffer */
		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
		UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);
#endif
#if P2P_MINIAP_SUPPORT
	}
#endif	/* P2P_MINIAP_SUPPORT */

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[linkId]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	return Result;
} /* end UMAC_ProcDeAuthReq() */

/******************************************************************************
 * NAME:UMAC_ProcUnJoin
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function places a request to the LMAC to reset the join state and bring
 * the system back to the initialized state so that a new link establishment
 *procedure can be started in future.It
 * -# Changes UMAC state to UMAC_UNJOINING.
 * -# Triggers the UMAC_UNJOIN_SUCCESS handler through the state machine
 *    to bring back the state machine to the INITIALIZED state.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcUnJoin(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;

#if P2P_MINIAP_SUPPORT
	uint8 buffType;
#endif				/*P2P_MINIAP_SUPPORT */

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcUnJoin()\n");

	UMAC_SET_STATE_PROCESSING(UmacHandle);

#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
		UMAC_DEVIF_TX_DATA *pDevIfTxReq;
		UMAC_TX_DESC *pTxDescriptor;
		UMAC_TX_DESC *pStoredTxDesc;
		uint32 get;
		uint8 index;
		void *pDriverInfo = NULL;

		OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[DEFAULT_LINK_ID]);
		for (buffType = 0; buffType < AP_BUFF_TYPES; buffType++) {
			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID] += pUmacInstance->sta[DEFAULT_LINK_ID].numPendingFrames[buffType];
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
			get = pUmacInstance->sta[DEFAULT_LINK_ID].get[buffType];
			while (pUmacInstance->sta[DEFAULT_LINK_ID].numPendingFrames[buffType]) {
				index = (uint8) (get & (AP_NUM_BUFFERS - 1));
				/*OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[DEFAULT_LINK_ID]);*/
				pDevIfTxReq = (UMAC_DEVIF_TX_DATA *) pUmacInstance->sta[DEFAULT_LINK_ID].buffer[buffType][index].buffPtr;
				pTxDescriptor = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxReq + sizeof(UMAC_DEVIF_TX_DATA));
				get++;
				pUmacInstance->sta[DEFAULT_LINK_ID].get[buffType] = get;
				/*OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[DEFAULT_LINK_ID]);*/

				if (pTxDescriptor->PacketId & UMAC_FWDED_FRAME) {


					UMAC_Release_Internal_TxDesc(UmacHandle, pTxDescriptor, pUmacInstance->gVars.ApUmacTxDesc, UMAC_NUM_AP_INTERNAL_TX_BUFFERS, &pDriverInfo);
					pStoredTxDesc = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxReq + (sizeof(UMAC_TX_DESC)
												   - sizeof(UMAC_RX_DESC))
					    );
					UMAC_RELEASE_PACKET_ID_AP(UmacHandle, pTxDescriptor->PacketId);
					UMAC_LL_RELEASE_FWD_RX_BUFF(
							UmacHandle,
							pStoredTxDesc,
							pDriverInfo);
					pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]--;
				} else {
					if (pTxDescriptor->PacketId & UMAC_PACKETID_PS_BUFFRED_FRAME)
						pTxDescriptor->PacketId &= ~(UMAC_PACKETID_PS_BUFFRED_FRAME);
					UMAC_ProcessPendingPsBuffTxRequests(pUmacInstance, DEFAULT_LINK_ID, pTxDescriptor);
				}

				pUmacInstance->sta[DEFAULT_LINK_ID].numPendingFrames[buffType] = (uint8) (pUmacInstance->sta[DEFAULT_LINK_ID].put[buffType] - get);
			}
		}		/*for(i=0; i<AP_BUFF_TYPES; i++) */
		OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[DEFAULT_LINK_ID]);
		/*UMAC_ProcessPendingTxRequests(pUmacInstance, DEFAULT_LINK_ID);*/
	}
#endif	/* P2P_MINIAP_SUPPORT */

	if (pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID] || pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]) {

		LOG_DATA(DBG_WFM_UMAC | DBG_WFM_ASSOC | DBG_WFM_ERROR, "NumRxBufPending = %d\n", pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]);

		LOG_DATA(DBG_WFM_UMAC | DBG_WFM_ASSOC | DBG_WFM_ERROR, "NumTxBufPending = %d\n", pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]);
		/*There are outstanding Tx or Rx buffers, so
		   Scheduling Unjoin for future */
		pUmacInstance->sta[DEFAULT_LINK_ID].UmacConHandle.linkId = DEFAULT_LINK_ID;
		pUmacInstance->sta[DEFAULT_LINK_ID].UmacConHandle.UmacHandle = UmacHandle;
		OS_START_TIMER(pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer, WFM_UMAC_UNJOIN_TIMEOUT, (UMAC_CONNECTION_HANDLE *)&pUmacInstance->sta[DEFAULT_LINK_ID].UmacConHandle);
		pUmacInstance->UnjoinTimerCount = 0;

		LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcUnJoin() Pending\n");

		Result = WFM_STATUS_SUCCESS;
		goto error_handler;
	}

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;
		pSmgmtReq->Type = SMGMT_REQ_TYPE_DEL_ALL_KEYS;
		pSmgmtReq->ReqNum = 0;
		pSmgmtReq->operation.deassoc_req_params.KeyEntryIndex = MAX_KEY_ENTRIES;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;


	}	/*if(!pSmgmtReq->HandleMgmtReq) */

	pSmgmtReq->HandleMgmtReq = 0;

	/*Reset the PMKID Cache Info */
	if (pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfoCount) {
		pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfoCount = 0;
		OS_MemoryReset(
			&pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfo[0],
			(WFM_MAX_BSSID_INFO_ENTRIES * sizeof(WFM_BSSID_INFO))
			);
	}
#if DOT11K_SUPPORT
	/*reset the LciInfoAvl */
	if (pUmacInstance->gVars.p.lciInfoAvl)
		pUmacInstance->gVars.p.lciInfoAvl = 0;
#endif	/* DOT11K_SUPPORT */


#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode < OPERATING_MODE_AP) {
		UMAC_SET_STATE(UmacHandle, UMAC_UNJOINING);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_UNJOINING\n");
	}
#endif
	Result = UMAC_LL_REQ_RESET_JOIN(UmacHandle, 0x00, DEFAULT_LINK_ID);

#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
#endif				/*P2P_MINIAP_SUPPORT */

 exit_handler:
	return Result;

 error_handler:

	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
	return Result;
} /* end UMAC_ProcUnJoin() */

/******************************************************************************
 * NAME:	UMAC_ProcUnJoinSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called upon the completion of reset-join by LMAC.
 * UMAC will be reseting all the state variables to the initial state here.
 * It
 * -# Changes UMAC state to UMAC_INITIALIZED.
 * -# Sends WFM_WLAN_EVT_DISCONNECTED to the Host to let the host know that
 *    the link has been terminated and the system is ready to accept another
 *    connect request.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcUnJoinSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 ExtendedUnJoinTimer = FALSE;

#if P2P_MINIAP_SUPPORT
	uint8 sendDisconEvent;
	sendDisconEvent = (pUmacInstance->operatingMode < OPERATING_MODE_AP) ? 1 : 0;
#endif				/*P2P_MINIAP_SUPPORT */

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC | DBG_WFM_ALWAYS, "UMAC_ProcUnJoinSuccess()\n");
	/* Stop the unjoin timer as soon as unjoin is successful*/
	if (pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer)
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer);

	ExtendedUnJoinTimer = pUmacInstance->extendedUnJoinTimer;

	/*
	  De-Initializing First and then Initializing to bring UMAC to a
	  clean state
	*/
	Result = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_DEINIT, NULL);

	if (WFM_STATUS_PENDING == Result)
		OS_ASSERT(0);

	if (ExtendedUnJoinTimer == FALSE)
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_INIT, NULL);

	/*Informing the Host that we are DISCONNECTED */

#if P2P_MINIAP_SUPPORT
	if (sendDisconEvent) {
#endif				/*P2P_MINIAP_SUPPORT */
		if (!pUmacInstance->isCoalescing) {
			WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_DISCONNECTED,
						0,
						NULL,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);
		}

#if P2P_MINIAP_SUPPORT
	}
#endif	/* P2P_MINIAP_SUPPORT */

	if (ExtendedUnJoinTimer == TRUE) {
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_DEVICE_UNRESPONSIVE,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
	}

	return Result;
} /* end UMAC_ProcUnJoinSuccess() */

/******************************************************************************
 * NAME:	UMAC_ProcScanCompleted
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called upon the completion of scan by LMAC.
 * UMAC will be reseting all the state variables to the initial state here.
 * It
 * -# Changes UMAC scan sub-state from UMAC_SCANNING to  UMAC_READY_TO_SCAN.
 * -# Sends WFM_EVT_SCAN_COMPLETED to the Host.
 * -# Releases the Internal Buffer and Descriptor allocated for Scan Request.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcScanCompleted(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 memstatus;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN | DBG_WFM_CURRENT_AK, "UMAC_ProcScanCompleted()\n");

	OS_ASSERT(pMsg);

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	if (pUmacInstance->UmacScanState == UMAC_SCANNING) {

		pUmacInstance->UmacScanState = UMAC_READY_TO_SCAN;
		LOG_EVENT(DBG_WFM_UMAC, "SCAN STATE = UMAC_READY_TO_SCAN\n");

		OS_ASSERT(pUmacInstance->gVars.pScanTxDesc)

		    WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pScanTxDesc);

		/* Freeing the internal buffer */
		memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pScanTxDesc
						);

		OS_ASSERT(memstatus);

		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pScanTxDesc);

		pUmacInstance->gVars.pScanTxDesc = NULL;

		if (pUmacInstance->StartScanInNextBand == TRUE) {
			/*
			  scan in one band is completed, now scan in another
			  band
			*/
			WFM_SCHEDULE_TO_SELF(
						UmacHandle,
						UMAC_START_SCAN,
						&pUmacInstance->ScanReqFromHost
						);
		} else {
			/* scan in all the bands is completed */
			pUmacInstance->IsBgScan = 0;

			if (pUmacInstance->IsInternalScan) {
				pUmacInstance->IsInternalScan = FALSE;

				if (!pUmacInstance->IsInternalScanSuccess) {
					/*
					   We didnt get any response for the
					   internal scan. We need to send the
					   Host a Connect Failure event to
					   inform about this.
					 */

					WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_CONNECT_FAILED,
						0,
						NULL,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);

				} else
					pUmacInstance->IsInternalScanSuccess = FALSE;
			}
#if P2P_MINIAP_SUPPORT
			else if (pUmacInstance->deviceDiscovery == TRUE) {
				/*
				  If it is device discovery, send find request
				  after scanning
				*/
				WFM_SCHEDULE_TO_SELF(
							UmacHandle,
							UMAC_START_FIND,
							NULL
							);
			}
#endif	/* P2P_MINIAP_SUPPORT */
			else {
				WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_SCAN_COMPLETED,
						0,
						NULL,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);
			}
			if (pUmacInstance->JoinPending == TRUE) {
				/* join req is pending */
				/*not pending anymore */
				pUmacInstance->JoinPending = FALSE;
			}
		}
	} else {
		LOG_EVENT(DBG_WFM_ERROR, "ERROR: Recieved Scan Complete while not in SCANNING");
		OS_ASSERT(0);
	}

	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);

	return Result;
} /* end UMAC_ProcScanCompleted() */

/******************************************************************************
 * NAME:	UMAC_ProcMgmtTxBuffer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called by LMAC whenever LMAC is completing the
 * transmission of a packet queued by UMAC, if UMAC state is not
 * UMAC_ASSOCIATED. It
 * -# Releases the Internal Buffer and Descriptor allocated
 * -# If its not an internal buffer, calls the TxCnf handler to release
 *    the memory.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcMgmtTxBuffer(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	UMAC_TX_CNF_DESC *TxDescCnf = (UMAC_TX_CNF_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pStoredTxDesc = NULL;
	uint32 memstatus;
	UMAC_MULTI_TX_CNF_DESC *pMultiTxCnfDesc = (UMAC_MULTI_TX_CNF_DESC *) pMsg;
	uint32 NumTxConfs = 0;
	uint8 TxCnfCount = 0;
	uint8 linkId;
	UMAC_TX_MGMT_DATA_CNF DataTxMgmtFrmCnf = {0};
	void *pDriverInfo = NULL;
#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((TxDescCnf->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
#else
	linkId = 0;
#endif	/*P2P_MINIAP_SUPPORT */

	OS_ASSERT(pUmacInstance);

	LOG_DATA(DBG_WFM_UMAC, "UMAC_ProcMgmtTxBuffer (TxComplete) - 0x%x\n", TxDescCnf);

	OS_ASSERT(TxDescCnf);

	LOG_DATA(DBG_WFM_UMAC, "Buffer to Free pMsg=0x%x)\n", TxDescCnf);

	WFM_UMAC_DBG_PRINT_TX_DESC();

	WFM_UMAC_DBG_REMOVE_TX_DESC(TxDescCnf);

	if ((pMultiTxCnfDesc->MsgId & 0x0C3F) == WSM_HI_MULTI_TX_CNF_ID) {
		NumTxConfs = pMultiTxCnfDesc->NumTxConfs;
		TxDescCnf = (UMAC_TX_CNF_DESC *) ((uint8 *) pMultiTxCnfDesc + 4);
		TxDescCnf->MsgId = WSM_HI_TX_CNF_ID;
		TxDescCnf->MsgLen = sizeof(UMAC_TX_CNF_DESC);
	} else if ((TxDescCnf->MsgId & 0x0C3F) == WSM_HI_TX_CNF_ID)
		NumTxConfs = 1;
	else
		OS_ASSERT(0);

	for (TxCnfCount = 1; TxCnfCount <= NumTxConfs; TxCnfCount++) {
		if ((pUmacInstance->nullFrmPktId == TxDescCnf->PacketId)
			&& (pUmacInstance->isNullDataFrame == 1)
			&& TxDescCnf->Status == WFM_STATUS_SUCCESS) {
				pUmacInstance->statusNullDataFrameCnf = 1;
		} else
			pUmacInstance->statusNullDataFrameCnf = 0;

		if ((pUmacInstance->nullFrmPktId == TxDescCnf->PacketId)
			&& (pUmacInstance->PsNullFrameStatus == 1))
			pUmacInstance->PsNullFrameStatus = 0;

		pStoredTxDesc = UMAC_Release_Internal_TxDesc(
					UmacHandle,
					(UMAC_TX_DESC *) TxDescCnf,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					&pDriverInfo
					);

#if P2P_MINIAP_SUPPORT
		if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP) && !pStoredTxDesc) {
			pStoredTxDesc = UMAC_Release_Internal_TxDesc(
						UmacHandle,
						(UMAC_TX_DESC *) TxDescCnf,
						pUmacInstance->gVars.ApUmacTxDesc,
						UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
						&pDriverInfo
						);
		}
#endif		/*P2P_MINIAP_SUPPORT */

		if (TxDescCnf->PacketId & UMAC_PACKETID_FLAG_EXT_MGMT) {
			DataTxMgmtFrmCnf.Status = umac_interpret_wsm_status(TxDescCnf->Status);
			DataTxMgmtFrmCnf.FrameLength = pStoredTxDesc->MsgLen - sizeof(UMAC_TX_DESC);
			DataTxMgmtFrmCnf.pFrame = (uint32 *)((uint8 *)pStoredTxDesc + sizeof(UMAC_TX_DESC));

			pUmacInstance->gVars.p.UpperLayerTxMgmtcnf(
					      pUmacInstance->gVars.p.ulHandle,
					      &DataTxMgmtFrmCnf
					      );
			TxDescCnf->PacketId = TxDescCnf->PacketId & 0x000000FF;
		}

		if (!pStoredTxDesc) {
			if ((TxDescCnf->MsgId & 0x4000) == 0x4000) {
				/*
				  This function called from
				  UMAC_ProcTxCompleted() to check Managment
				  Buffers. So returned status failure as it is
				  failed to find in managment buffer
				*/
				Result = WFM_STATUS_FAILURE;
				goto exit_handler;
			}
			/*
			   Its not an UMAC management frame buffer, so call
			   UMAC_ProcTxCompleted() to handle it properly and
			   free the memory.
			 */

			if (pUmacInstance->MgMtTxCnfmCounter) {
				/*
				   Something went wrong, so better to escape
				   rather than causing stack overflow
				*/
#if P2P_MINIAP_SUPPORT
				if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
					/*
					   It is possible that when UMAC is in
					   mini Ap mode and it wants to deauth
					   an associated STA and that STA is in
					   power save. So the deauth frame is
					   delivered through PS poll and packet
					   ID of deauth frame is freed before
					   Tx CNF for deauth frame arrives at
					   UMAC. In this case the Tx CNF needs
					   to be freed.
					*/
					UMAC_RELEASE_TX_CNF_FRAME(
								UmacHandle,
								TxDescCnf
								);
				}
#endif				/*P2P_MINIAP_SUPPORT */

				goto exit_handler;
			}

			pUmacInstance->MgMtTxCnfmCounter++;

			TxDescCnf->MsgId |= 0x4000;

			Result = UMAC_ProcTxCompleted(
							UmacHandle,
							(void *)TxDescCnf
							);

			TxDescCnf->MsgId &= 0xbFFF;

			pUmacInstance->MgMtTxCnfmCounter--;

			if (WFM_STATUS_SUCCESS != Result) {
				LOG_EVENT(DBG_WFM_ERROR, "UMAC_ProcMgmtTxBuffer() : Failed to find a matching Packet Id in TxDescStore\n");
				OS_ASSERT(0);

			}	/*if( WFM_STATUS_SUCCESS != Result) */

			goto next_step;

		} else {
#if 0
			DataTxMgmtFrm.Status = umac_interpret_wsm_status(TxDescCnf->Status);
			DataTxMgmtFrm.FrameLength = pStoredTxDesc->MsgLen - sizeof(UMAC_TX_DESC);
			DataTxMgmtFrm.pFrame = (uint32 *)((uint8 *)pStoredTxDesc + sizeof(UMAC_TX_DESC));
			if (TxDescCnf->PacketId & UMAC_PACKETID_FLAG_EXT_MGMT) {
				WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_TX_MGMT_FRAME_COMPLETED,
						sizeof(DataTxMgmtFrm),
						(uint8 *)&DataTxMgmtFrm,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);
			}
#endif /* 0 */
			if (TxDescCnf->PacketId & UMAC_PACKETID_PS_BUFFRED_FRAME)
				TxDescCnf->PacketId &= ~(UMAC_PACKETID_PS_BUFFRED_FRAME);

			UMAC_RELEASE_PACKET_ID(UmacHandle, TxDescCnf->PacketId);

			/*Releasing the internally allocated buffer */
			memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					MOVE_TO_START_OF_DEV_IF_TX_REQ(pStoredTxDesc)
					);
			OS_ASSERT(memstatus);

			UMAC_MEM_REMOVE_STAMP(
				MOVE_TO_START_OF_DEV_IF_TX_REQ(pStoredTxDesc)
				);

		}

 next_step:
		if ((TxDescCnf->MsgId & 0x4000) != 0x4000) {
			if (TxCnfCount == NumTxConfs) {
				/*
				   In host umac, we should explicitly release
				   the rx frame
				 */
				TxDescCnf = (UMAC_TX_CNF_DESC *) pMultiTxCnfDesc;
				UMAC_RELEASE_TX_CNF_FRAME(UmacHandle, TxDescCnf);

				OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
				pUmacInstance->NumTxBufPending[linkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
			} else {

				OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
				pUmacInstance->NumTxBufPending[linkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

				TxDescCnf = (UMAC_TX_CNF_DESC *) ((uint8 *) TxDescCnf + 20);
				TxDescCnf->MsgId = WSM_HI_TX_CNF_ID;
				TxDescCnf->MsgLen = sizeof(UMAC_TX_CNF_DESC);
			}
		}
	}

 exit_handler:
	return Result;
} /* end UMAC_ProcMgmtTxBuffer() */

/******************************************************************************
 * NAME:UMAC_ProcTxCompleted
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called by LMAC whenever LMAC is completing the
 * transmission of a packet queued by UMAC, if UMAC state is UMAC_ASSOCIATED.
 * It
 *   -# Releases the Internal Buffer and Descriptor allocated
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcTxCompleted(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_HI_MESSAGE *pHiMessage = NULL;
	UMAC_TX_DATA_CNF *pTxDataCnf = NULL;
	uint32 Status = 0;
	uint8 TxedRate = 0;
	uint8 AckFailures = 0;
	UMAC_TX_CNF_DESC *pTxCnfDesc = (UMAC_TX_CNF_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_PKT *pUmacTxPkt;
	UMAC_MULTI_TX_CNF_DESC *pMultiTxCnfDesc = (UMAC_MULTI_TX_CNF_DESC *) pMsg;
	uint32 NumTxConfs = 0;
	uint8 TxCnfCount = 0;
	uint32 packetId = 0;
	void *pDriverInfo = NULL;
	uint8 linkId;

#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((pTxCnfDesc->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
#else
	linkId = 0;
#endif

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC, "UMAC_ProcTxCompleted()\n");

	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pTxCnfDesc);

	if ((pMultiTxCnfDesc->MsgId & 0x0C3F) == WSM_HI_MULTI_TX_CNF_ID) {
		NumTxConfs = pMultiTxCnfDesc->NumTxConfs;
		pTxCnfDesc = (UMAC_TX_CNF_DESC *) ((uint8 *) pMultiTxCnfDesc + 4);
		pTxCnfDesc->MsgId = WSM_HI_TX_CNF_ID;
		pTxCnfDesc->MsgLen = sizeof(UMAC_TX_CNF_DESC);
	} else if ((pTxCnfDesc->MsgId & 0x0C3F) == WSM_HI_TX_CNF_ID) {
		NumTxConfs = 1;
	} else {
		OS_ASSERT(0);
	}

	for (TxCnfCount = 1; TxCnfCount <= NumTxConfs; TxCnfCount++) {
		packetId =  pTxCnfDesc->PacketId & (CFG_HI_NUM_REQS - 1);

		if ((pTxCnfDesc->Status == WFM_FRAME_REQUEUE) && (packetId == pUmacInstance->nullFrmPktId))
			pUmacInstance->PsNullFrameStatus = 0;

#if P2P_MINIAP_SUPPORT
		if (!(pTxCnfDesc->Flags & UMAC_TX_CNF_REQUEUE)  || (pUmacInstance->operatingMode <= OPERATING_MODE_P2P_CLIENT)) {
#endif				/*P2P_MINIAP_SUPPORT */
			TxedRate = pTxCnfDesc->TxedRate;
			AckFailures = pTxCnfDesc->AckFailures;

			Status = umac_interpret_wsm_status(pTxCnfDesc->Status);

			LOG_DATA2(DBG_WFM_UMAC | DBG_WFM_IN_OUT_MSGS | DBG_WFM_TX, "TxCmpl: Id=0x%x, status=%d\n", pTxCnfDesc->PacketId, Status);
			/*
			  Check the status here and if it status is failure
			  then update the txFailureRecvd in pUmacInstance
			*/
			if (Status == WFM_STATUS_RETRY_EXCEEDED)
				pUmacInstance->txFailureRecvd++;
			else
				pUmacInstance->txFailureRecvd = 0;
			/*
			   Now free the TxDescriptor and send TxConfirmation to
			   the Host
			 */

#if DOT11K_SUPPORT
			/*Check for the transmit stream measurement here */
			if ((pTxCnfDesc->PacketId & UMAC_PACKET_ID_MASK) && (pUmacInstance->dot11k_Meas_Params.transmitStreamUnderProcess)) {
				uint32 TxCnfTime = OS_GetTime();
				pUmacInstance->dot11k_Meas_Params.txStreamResults.totalPackets++;

				if ((Status == WFM_STATUS_SUCCESS) && (!AckFailures))
					pUmacInstance->dot11k_Meas_Params.txStreamResults.txMsduCount++;
				if ((Status == WSM_STATUS_RETRY_EXCEEDED) || (Status == WSM_STATUS_TX_LIFETIME_EXCEEDED))
					pUmacInstance->dot11k_Meas_Params.txStreamResults.msduDiscardedCount++;
				if ((Status == WSM_STATUS_RETRY_EXCEEDED) && (Status != WSM_STATUS_TX_LIFETIME_EXCEEDED))
					pUmacInstance->dot11k_Meas_Params.txStreamResults.msduFailedCount++;
				if ((Status == WFM_STATUS_SUCCESS) && (AckFailures))
					pUmacInstance->dot11k_Meas_Params.txStreamResults.msduMultipleRetryCount++;

				if (TxCnfTime != pUmacInstance->dot11k_Meas_Params.txStreamResults.CurrentTime[packetId])
					pUmacInstance->dot11k_Meas_Params.txStreamResults.queueDelay += (((TxCnfTime - pUmacInstance->dot11k_Meas_Params.txStreamResults.CurrentTime[packetId]) * 1000) - (pTxCnfDesc->MediaDelay - pTxCnfDesc->TxQueueDelay));
				else
					pUmacInstance->dot11k_Meas_Params.txStreamResults.queueDelay += (pTxCnfDesc->MediaDelay - pTxCnfDesc->TxQueueDelay);

				pUmacInstance->dot11k_Meas_Params.txStreamResults.transmitDelay += ((TxCnfTime - pUmacInstance->dot11k_Meas_Params.txStreamResults.CurrentTime[packetId]) * 1000);
				/*Increment the bin as per the value of Bin0 Range */
				if ((Status == WFM_STATUS_SUCCESS) && (!AckFailures)) {
					TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ *pTransmitStreamReq = (TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ *)
						&pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;
					uint8 binRange = pTransmitStreamReq->binRange ;
					uint8 TxDelayInTu = (uint8) (((TxCnfTime - pUmacInstance->dot11k_Meas_Params.txStreamResults.CurrentTime[packetId]) * 1000) / 1024);
					if (TxDelayInTu < binRange)
						pUmacInstance->dot11k_Meas_Params.txStreamResults.bin0++;
					else if ((binRange <= TxDelayInTu) && (TxDelayInTu < (2 * binRange)))
						pUmacInstance->dot11k_Meas_Params.txStreamResults.bin1++;
					else if (((2 * binRange) <= TxDelayInTu) && (TxDelayInTu < (4 * binRange)))
						pUmacInstance->dot11k_Meas_Params.txStreamResults.bin2++;
					else if (((4 * binRange) <= TxDelayInTu) && (TxDelayInTu < (8 * binRange)))
						pUmacInstance->dot11k_Meas_Params.txStreamResults.bin3++;
					else if (((8 * binRange) <= TxDelayInTu) && (TxDelayInTu < (16 * binRange)))
						pUmacInstance->dot11k_Meas_Params.txStreamResults.bin4++;
					else if ((16 * binRange) <= TxDelayInTu)
						pUmacInstance->dot11k_Meas_Params.txStreamResults.bin5++;
				}
			}
#endif	/* DOT11K_SUPPORT */

			WFM_UMAC_DBG_REMOVE_TX_DESC(pMsg);
#if P2P_MINIAP_SUPPORT
		}		/*if(!(pTxCnfDesc->Flags & UMAC_TX_CNF_REQUEUE)) */
#endif	/* P2P_MINIAP_SUPPORT */

		if (pTxCnfDesc->PacketId & UMAC_PACKETID_FLAG_EXT_MGMT) {
			/*This meachanism will not work in 11n mode for SoftAP and Wifi Direct*/
			Result = UMAC_ProcMgmtTxBuffer(UmacHandle, pMsg);
			if (WFM_STATUS_SUCCESS != Result) {
				LOG_EVENT(DBG_WFM_ERROR, "UMAC_ProcTxCompleted() : Failed to find a matching Packet Id in TxDescStore\n");
				OS_ASSERT(0);
			} /* if( WFM_STATUS_SUCCESS != Result) */

			goto exit_handler;
		}

		pUmacTxPkt = (UMAC_TX_PKT *)
			&pUmacInstance->gVars.TxDescStore[packetId];

		if (!(pTxCnfDesc->Flags & UMAC_TX_CNF_REQUEUE)  || (pUmacInstance->operatingMode <= OPERATING_MODE_P2P_CLIENT)) {
			if (pUmacTxPkt->pTxDesc != NULL) {
				if (pUmacTxPkt->pTxDesc->PacketId == pTxCnfDesc->PacketId) {
					OS_LOCK(pUmacInstance->gVars.p.TxDescStoreLock);

					pTxDataCnf = (UMAC_TX_DATA_CNF *) pUmacTxPkt->pMsgPtr;

					OS_ASSERT(pTxDataCnf);

					pUmacTxPkt->pMsgPtr = NULL;

					pHiMessage = (WFM_HI_MESSAGE *) pTxDataCnf->pUlHdr;

					OS_ASSERT(pHiMessage);

					pHiMessage->PayLoad.wfm_transmit_cnf.Reference = pUmacTxPkt->reference;

					pUmacTxPkt->reference = 0;

					/*Releasing the Tx Descriptor */
					UMAC_LL_RELEASE_TX_DESC(UmacHandle, pUmacTxPkt->pTxDesc);

					pUmacTxPkt->pTxDesc = NULL;

					OS_UNLOCK(pUmacInstance->gVars.p.TxDescStoreLock);

					UMAC_RELEASE_PACKET_ID(UmacHandle, packetId);
				}
			} /*if( pUmacTxPkt->pTxDesc  !=  NULL ) */

			if ((!pHiMessage)) {
#if P2P_MINIAP_SUPPORT
				if (pUmacInstance->operatingMode < OPERATING_MODE_AP) {
	#endif				/*P2P_MINIAP_SUPPORT */
					/*
					If there's no matching Tx descriptors, there's
					no point in trying to send confirmation up to
					the Host, as the buffer might be allocated
					internally by UMAC. So freeing the internaly
					buffer here.
					Details:
					There's no guarantee in the order in which LMAC
					will give Tx confirmations,so, sometimes we may
					recieve Tx confirmation for a trasmited Assoc
					req after receiving the Assoc response. Since
					we received the assoc response, UMAC state will
					be ASSOCIATED, which in turn will cause UMAC to
					treat this Tx confirmations as the Tx
					confimration for a Data packet.
					*/
					if ((pTxCnfDesc->MsgId & 0x4000) == 0x4000) {
						/*
						This function called from
						UMAC_ProcMgmtTxBuffer() to check Tx
						Data Buffer. So returned status failure
						as it is failed to find in Tx Data
						Buffer
						*/
						Result = WFM_STATUS_FAILURE;
						goto exit_handler;
					}
#if P2P_MINIAP_SUPPORT
				}
#endif	/* P2P_MINIAP_SUPPORT */
				/*
				Set second MSB to indicate that this function called
				from UMAC_ProcTxCompleted()
				*/
				pTxCnfDesc->MsgId |= 0x4000;
				Result = UMAC_ProcMgmtTxBuffer(UmacHandle, pMsg);
				pTxCnfDesc->MsgId &= 0xbFFF;

				if ((WFM_STATUS_SUCCESS != Result)
#if P2P_MINIAP_SUPPORT
					&& (pUmacInstance->operatingMode < OPERATING_MODE_AP)
#endif	/* P2P_MINIAP_SUPPORT */
				) {
					LOG_EVENT(DBG_WFM_ERROR, "UMAC_ProcTxCompleted() : Failed to find a matching Packet Id in TxDescStore\n");
					OS_ASSERT(0);
				} /* if( WFM_STATUS_SUCCESS != Result) */

				/*goto exit_handler ; */
				goto next_step;
			} else {
				pHiMessage->PayLoad.wfm_transmit_cnf.Status = Status;
			}

		} /* No Requeue */
#if P2P_MINIAP_SUPPORT
		if (!(pTxCnfDesc->Flags & UMAC_TX_CNF_REQUEUE)  || (pUmacInstance->operatingMode <= OPERATING_MODE_P2P_CLIENT)) {
#endif				/*P2P_MINIAP_SUPPORT */
			pHiMessage->MessageId = WFM_TRANSMIT_CNF_ID;
			pHiMessage->MessageLength = (WFM_HI_MESSAGE_HDR_SIZE + sizeof(WFM_TRANSMIT_CNF));

			OS_ASSERT(pUmacInstance->gVars.p.UpperLayerTxcnf);

			pUmacInstance->gVars.p.UpperLayerTxcnf(
						      pUmacInstance->gVars.p.ulHandle,
						      pTxDataCnf
						      );

			/*
			Send the event only if OID is set for Tx failure
			threshold count
			*/
			if (pUmacInstance->gVars.p.TxFailureCount.TransmitFailureThresholdCount) {
				if (pUmacInstance->txFailureRecvd == pUmacInstance->gVars.p.TxFailureCount.TransmitFailureThresholdCount) {
					/*
					Reset the txFailureRecvd as we are
					sending the event to upper layer now
					*/
					pUmacInstance->txFailureRecvd = 0;
					pUmacInstance->TxFailureEventRcvd = 1;
					/*Send the event to upperlayer */
					WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_TX_FAILURE_THRESHOLD_EXCEEDED,
						0,
						NULL,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);
				}
			}

			pUmacInstance->CheckForConnectionDataTxCtr++;

			UMAC_ManageRateSelection(
						pUmacInstance,
						Status,
						TxedRate,
						AckFailures,
						linkId
						);
#if TX_PER_SUPPORT
			if (pUmacInstance->operatingMode < OPERATING_MODE_AP) {
				pUmacInstance->TxStats.TxCount++;
				if (Status == WFM_STATUS_SUCCESS) {
					pUmacInstance->TxStats.TxSuccessCount++;
				} else {
					pUmacInstance->TxStats.TxFailureCount++;
					if (Status == WFM_STATUS_RETRY_EXCEEDED)
						pUmacInstance->TxStats.TxRetryExceedCount++;
				}
				if (AckFailures)
					pUmacInstance->TxStats.TxRetriedCount++;
			}
#endif

 next_step:
#if P2P_MINIAP_SUPPORT
			if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
				UMAC_Release_Internal_TxDesc(UmacHandle,
					(UMAC_TX_DESC *) pTxCnfDesc,
					pUmacInstance->gVars.ApUmacTxDesc,
					UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
					&pDriverInfo);
			} else {
#endif	/* P2P_MINIAP_SUPPORT */
				if ((pTxCnfDesc->MsgId & 0x4000) != 0x4000) {
					if (TxCnfCount == NumTxConfs) {
						/*
						In host umac, we should explicitly
						release the rx frame
						*/
						pTxCnfDesc = (UMAC_TX_CNF_DESC *) pMultiTxCnfDesc;
						UMAC_RELEASE_TX_CNF_FRAME(
								UmacHandle,
								pTxCnfDesc
								);

						OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
						pUmacInstance->NumTxBufPending[linkId]--;
						OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
					} else {
						OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
						pUmacInstance->NumTxBufPending[linkId]--;
						OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

						pTxCnfDesc = (UMAC_TX_CNF_DESC *) ((uint8 *) pTxCnfDesc + 20);
						pTxCnfDesc->MsgId = WSM_HI_TX_CNF_ID;
						pTxCnfDesc->MsgLen = sizeof(UMAC_TX_CNF_DESC);
					}
				} /*if((pTxCnfDesc->MsgId & 0x4000) != 0x4000) */
#if P2P_MINIAP_SUPPORT
			}
#endif	/* P2P_MINIAP_SUPPORT */
#if P2P_MINIAP_SUPPORT
		} else {
			if (pUmacTxPkt->pTxDesc == NULL) {
				UMAC_TX_DESC *pTxDesc;
				void *pDriverInfo = NULL;
				pTxDesc = UMAC_Release_Internal_TxDesc(UmacHandle,
						(UMAC_TX_DESC *) pTxCnfDesc,
						pUmacInstance->gVars.umacTxDescInt,
						UMAC_NUM_INTERNAL_TX_BUFFERS,
						&pDriverInfo);
				if (pTxDesc != NULL) {
					UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						MOVE_TO_START_OF_DEV_IF_TX_REQ(pTxDesc)
						);
					UMAC_RELEASE_PACKET_ID(UmacHandle, pTxCnfDesc->PacketId & 0x7F);
				}
			} else {
				uint32 put;
				uint8 index;
				uint8 buffType = AP_BUFF_TYPES_LEGACY;
				uint8 queueid;
				uint8 uapsdFlag = 0;
				uint8 Queued = 0;
				if ((linkId) && (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_DOZE)) {
					if (linkId)
						uapsdFlag = pUmacInstance->sta[linkId].qosInfo & UMAC_QOS_INFO_STA_UAPSD_MASK;

					queueid = pUmacTxPkt->pTxDesc->QueueId;

					switch (queueid) {
					case 0:
						/* AC_BK */
						if (uapsdFlag & UMAC_QOS_INFO_STA_UAPSD_AC_BK)
							buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
						break;

					case 1:
						/* AC_BE */
						if (uapsdFlag & UMAC_QOS_INFO_STA_UAPSD_AC_BE)
							buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
						break;
					case 2:
						/* AC_VI */
						if (uapsdFlag & UMAC_QOS_INFO_STA_UAPSD_AC_VI)
							buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
						break;
					case 3:
						/* AC_VO */
						if (uapsdFlag & UMAC_QOS_INFO_STA_UAPSD_AC_VO)
							buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
						break;
					} /*switch(accessCategory) */

					OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
					put = pUmacInstance->sta[linkId].put[buffType];
					index = (uint8) (put & (AP_NUM_BUFFERS - 1));
					if ((put - pUmacInstance->sta[linkId].get[buffType]) < AP_NUM_BUFFERS) {
						pUmacInstance->sta[linkId].buffer[buffType][index].buffPtr = (uint8 *) pUmacTxPkt->pDevIfTxReq;
						pUmacInstance->sta[linkId].buffer[buffType][index].timeStamp = (OS_GetTime() + (pUmacInstance->sta[linkId].listenInterval + 4) * pUmacInstance->dot11BssConf.beaconInterval);
						put++;
						/*pUmacInstance->gVars.ApPktIdPut = put;*/
						pUmacInstance->sta[linkId].put[buffType] = put;
						pUmacInstance->sta[linkId].numPendingFrames[buffType] = (uint8) (put - pUmacInstance->sta[linkId].get[buffType]);
						Queued = 1;
					}
					OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
					if (Queued) {
						if ((buffType == AP_BUFF_TYPES_LEGACY) || (uapsdFlag == UMAC_QOS_INFO_STA_UAPSD_MASK)) {
							OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
							if (!(pUmacInstance->virtualBitmap & (1 << linkId))) {
								pUmacInstance->virtualBitmap |= 1 << linkId;
								if (pUmacInstance->updateTimIeInProgress == FALSE) {
									pUmacInstance->updateTimIeInProgress = TRUE;
									OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
									WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);
								} else
									OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
							} else
								OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
						}
					}
				} else {
					OS_LOCK(pUmacInstance->gVars.p.TxBuffRequeuePoolLock);
					put = pUmacInstance->BuffRequeueFrms.put;
					index = (uint8) (put & (NUM_REQUEUE_BUFFERS - 1));
					if ((put - pUmacInstance->BuffRequeueFrms.get) < NUM_REQUEUE_BUFFERS) {
						pUmacInstance->BuffRequeueFrms.frmBuffPtr[index].buffPtr = (uint8 *) pUmacTxPkt->pDevIfTxReq;
						pUmacInstance->BuffRequeueFrms.frmBuffPtr[index].timeStamp = (OS_GetTime() + (pUmacInstance->sta[linkId].listenInterval + 4) * pUmacInstance->dot11BssConf.beaconInterval);
						put++;
						pUmacInstance->BuffRequeueFrms.put = put;
						pUmacInstance->BuffRequeueFrms.numPendingFrames = (uint8) (put - pUmacInstance->BuffRequeueFrms.get);
						Queued = 1;
					}
					OS_UNLOCK(pUmacInstance->gVars.p.TxBuffRequeuePoolLock);
				}

				if (!Queued) {
					if (pUmacTxPkt->pTxDesc->PacketId == pTxCnfDesc->PacketId) {
						OS_LOCK(pUmacInstance->gVars.p.TxDescStoreLock);
						pTxDataCnf = (UMAC_TX_DATA_CNF *) pUmacTxPkt->pMsgPtr;
						OS_ASSERT(pTxDataCnf);
						pUmacTxPkt->pMsgPtr = NULL;

						pHiMessage = (WFM_HI_MESSAGE *) pTxDataCnf->pUlHdr;
						OS_ASSERT(pHiMessage);
						pHiMessage->PayLoad.wfm_transmit_cnf.Reference = pUmacTxPkt->reference;
						pHiMessage->PayLoad.wfm_transmit_cnf.Status = WFM_STATUS_FAILURE;
						pUmacTxPkt->reference = 0;
						pUmacTxPkt->pDevIfTxReq = NULL;
						/*Releasing the Tx Descriptor */
						UMAC_LL_RELEASE_TX_DESC(
									UmacHandle,
									pUmacTxPkt->pTxDesc
									);

						pUmacTxPkt->pTxDesc = NULL;
						OS_UNLOCK(pUmacInstance->gVars.p.TxDescStoreLock);
						UMAC_RELEASE_PACKET_ID(UmacHandle, pTxCnfDesc->PacketId);

						/* Indicate failure to upper layer */
						pHiMessage->MessageId = WFM_TRANSMIT_CNF_ID;
						pHiMessage->MessageLength = (WFM_HI_MESSAGE_HDR_SIZE + sizeof(WFM_TRANSMIT_CNF));

						OS_ASSERT(pUmacInstance->gVars.p.UpperLayerTxcnf);

						pUmacInstance->gVars.p.UpperLayerTxcnf(
								pUmacInstance->gVars.p.ulHandle,
								pTxDataCnf
								);
					} /* PacketId matched */

					UMAC_Release_Internal_TxDesc(UmacHandle,
						(UMAC_TX_DESC *) pTxCnfDesc,
						pUmacInstance->gVars.ApUmacTxDesc,
						UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
						&pDriverInfo);
				} /* Queued */
			} /* pTxDesc != NULL */
		} /* Requeue */
#endif	/* P2P_MINIAP_SUPPORT */
	} /*for(TxCnfCount = 1; TxCnfCount <= NumTxConfs; TxCnfCount++) */
	/*
	   In host umac, we should explicitly release the rx frame
	 */
#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
		if ((pUmacInstance->sta[linkId].state == STA_STATE_DEAUTHENTICATING) && (linkId != 0)
			&& (pUmacInstance->NumTxBufPending[linkId] == 0)) {
				UMAC_RELEASE_TX_CNF_FRAME(UmacHandle, pTxCnfDesc);
		} else {
			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
			UMAC_RELEASE_TX_CNF_FRAME(UmacHandle, pTxCnfDesc);
		}
	}				/*if(pUmacInstance->operatingMode >= OPERATING_MODE_AP) */
#endif	/* P2P_MINIAP_SUPPORT */

exit_handler:
	return Result;

} /* end UMAC_ProcTxCompleted() */

/******************************************************************************
 * NAME:	UMAC_ProcRxDescBuff
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will release the RX Descriptors and RxBuffers LMAC may give to
 * UMAC as RxIndications, when UMAC is not in state to accept those packets.
 * It
 * -# Releases the Rx Buffer and Descriptor allocated
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcRxDescBuff(UMAC_HANDLE UmacHandle, void *pMsg)
{
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	uint8 linkId;
#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
#else
	linkId = 0;
#endif	/* P2P_MINIAP_SUPPORT */

	LOG_EVENT(DBG_WFM_UMAC, "UMAC_ProcRxDescBuff\n");

	OS_ASSERT(pUmacInstance);

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pRxDescriptor = (UMAC_RX_DESC *) pMsg;

	LOG_DATA(DBG_WFM_UMAC, "pUmacInstance->UmacCurrentState  %d\n", pUmacInstance->UmacCurrentState);

	WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[linkId]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);

	return WFM_STATUS_SUCCESS;
} /* end UMAC_ProcRxDescBuff() */

/******************************************************************************
 * NAME:	UMAC_ProcReleaseRxBuff
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will release the RX Descriptors and RxBuffers LMAC may give to
 * UMAC as RxIndications, when UMAC is not in state to accept those packets.
 * It
 * -# Releases the Rx Buffer and Descriptor allocated
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcReleaseRxBuff(UMAC_HANDLE UmacHandle, void *pMsg)
{
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 linkId;
#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
#else
	linkId = 0;
#endif	/* P2P_MINIAP_SUPPORT */

	LOG_EVENT(DBG_WFM_UMAC, "UMAC_ProcReleaseRxBuff\n");

	OS_ASSERT(pUmacInstance);

	/*
	No need to set the state as processing
	UMAC_SET_STATE_PROCESSING(UmacHandle);
	*/

	pRxDescriptor = (UMAC_RX_DESC *) pMsg;

	/*OS_MemoryCopy(&pDriverInfo,((uint8 *)pRxDescriptor - 4), 4);*/

	LOG_DATA(DBG_WFM_UMAC, "pUmacInstance->UmacCurrentState  %d\n", pUmacInstance->UmacCurrentState);

	WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[linkId]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle, pRxDescriptor, pUmacInstance->pDriverInfo);

	/*
	No need to restore the state
	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
	*/

	return WFM_STATUS_SUCCESS;
} /* end UMAC_ProcRxDescBuff() */

/******************************************************************************
 * NAME:	UMAC_ProcException
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This handler is to handle UMAC exceptions.If UMAC is getting some events
 * messages when its not expecting it, which may lead the system to an
 * in-consistent state is referd as UMAC Exception here.
 * For example, if LMAC is givng a data frame before join, it will be treated
 * as a UMAC exception. This function will ASSERT.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcException(UMAC_HANDLE UmacHandle, void *pMsg)
{
	LOG_EVENT(DBG_WFM_ERROR, "\nUMAC_ProcException\n");
	OS_ASSERT(0);

	return WFM_STATUS_SUCCESS;
} /* end UMAC_ProcException() */

/******************************************************************************
 * NAME:	UMAC_DefaultHostReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is a generic handler to process invalid events/ messages from host.
 * This handler will return status WFM_STATUS_REQ_REJECTED.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_DefaultHostReq(UMAC_HANDLE UmacHandle, void *pMsg)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_HI, "UMAC_DefaultHostReq()**\n");
	LOG_DATA(DBG_WFM_UMAC, "pUmacInstance->UmacCurrentState  %d\n", pUmacInstance->UmacCurrentState);

	return WFM_STATUS_REQ_REJECTED;
} /* end UMAC_DefaultHostReq () */


/******************************************************************************
 * NAME:	UMAC_QueueMessage
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This SM handler helps to queue the messages which are getting injected to the
 * SM when SM is busy in processing some other events/messages.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_QueueMessage(UMAC_HANDLE UmacHandle, void *pMsg)
{
	LOG_EVENT(DBG_WFM_HI, "UMAC_QueueMessage()**\n");

	return WFM_STATUS_PENDING;
} /* end UMAC_QueueMessage() */

/******************************************************************************
		      FUNCTIONS NOT IN THE STATE TABLE
******************************************************************************/

/******************************************************************************
 * NAME:	UMAC_ProcAssocSuccessWorker
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Association success event, which will be generated
 * on the reception of an association response from the remote device with
 * status success.
 * -# Releases the Rx buffer and Rxdescriptor for Association Rsp frame.
 * -# Selects the highest rate available as the current rate.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcAssocSuccessWorker(UMAC_HANDLE UmacHandle,
					  void *pMsg,
					  uint16 Event)
{
	uint32 Result;
	uint32 i;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;
		pSmgmtReq->Type = SMGMT_REQ_TYPE_ASSOC_SUCCESS;
		pSmgmtReq->ReqNum = 0;
		pSmgmtReq->operation.assoc_success_params.QueueId = 0;
		if (pUmacInstance->DTIMPeriod & WFM_DTIM_PERIOD_FLAG_UNKNOWN)
			pSmgmtReq->operation.assoc_success_params.NumBeaconPeriods = 1;
		else
			pSmgmtReq->operation.assoc_success_params.NumBeaconPeriods = pUmacInstance->DTIMPeriod * pUmacInstance->dtimSkippingCount;


		pSmgmtReq->operation.assoc_success_params.Result = WFM_STATUS_SUCCESS;

		if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) ||
			   (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION)
		    ) {
			pSmgmtReq->operation.assoc_success_params.IsQosLink = TRUE;
		} else {
			pSmgmtReq->operation.assoc_success_params.IsQosLink = FALSE;
		}
		/*Get the edac parameters from pUmacInstance */
		pSmgmtReq->operation.assoc_success_params.WriteEdcaParam = pUmacInstance->WriteEdcaParam;
		if (pSmgmtReq->operation.assoc_success_params.WriteEdcaParam)
			OS_MemoryCopy(&pSmgmtReq->operation.assoc_success_params.EdcaParam, &pUmacInstance->EdcaParam, sizeof(WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED));
		if (pUmacInstance->WriteJoinModeBasicRateSet)
			pSmgmtReq->operation.assoc_success_params.WriteJoinModeBasicRateSet = pUmacInstance->WriteJoinModeBasicRateSet;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;
	}			/*if(!pSmgmtReq->HandleMgmtReq) */

	pSmgmtReq->HandleMgmtReq = 0;

	Result = pSmgmtReq->operation.assoc_success_params.Result;

	pUmacInstance->IsFirmwareBssParamSet = TRUE;

	if ((WFM_STATUS_SUCCESS == Result) /*&& (pUmacInstance->sendDisassoc == FALSE)*/) {

		/*moved here from UMAC_ProcPostAssocIE since we dont */
		/*want to disturb rate adapation in case of reassociation */

		/* Rate Adap: 1. Reset Rate Adaptation Counters */
		pUmacInstance->RateAdapTxSuccessFailCount = 0;

		for (i = 0; i < WFM_UMAC_NUM_TX_RATES; i++)
			pUmacInstance->RateAdapHistoryTxRate[i] = 0;

		/* Rate Adap: 2. Schedule a timer to clear bad history */
		OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer,
			WFM_UMAC_RATE_ADAP_CLEAR_BAD_HISTORY_TIMEOUT,
			UmacHandle
			);
	} else {
		/*
		   Something is wrong, mostly some MIB operation failed, can be
		   some issue with transport layer below, so we cant move to
		   ASSOCIATED state. We should stabilize the UMAC state machine
		   here by brining it back to initialized state.
		 */
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_DE_ASSOC, NULL);
	}			/*if( Result == WFM_STATUS_SUCCESS ) */

 exit_handler:
	return Result;
} /* end UMAC_ProcAssocSuccessWorker() */


/******************************************************************************
 * NAME:	UMAC_ProcStartSaQuery
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will send SA Query Request or Response to AP (to ensure Secure
 * Association).
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartSaQuery(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
#if MGMT_FRAME_PROTECTION
	UMAC_SA_QUERY_FRAME_BODY *pRxSaQueryData = (UMAC_SA_QUERY_FRAME_BODY *) pMsg;
	WFM_UMAC_MGMT_FRAME *pSaQueryFrame = NULL;
	uint16 FrameBodyLen = 0;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcStartSaQuery()\n");

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	LOG_DATA(DBG_WFM_UMAC, "UMAC_ProcStartSaQuery pTxDesc=0x%x)\n", pTxDesc);

	pSaQueryFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);

	/*
	   Creating the WLAN Header
	 */
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_ACTION,
					&pSaQueryFrame->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					1
					);

	FrameBodyLen = WFM_UMAC_CreateSaQueryFrame(
						UmacHandle,
						pSaQueryFrame->FrameBody,
						pRxSaQueryData->Action,
						pRxSaQueryData->TransactionId
						);

	pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcStartSaQuery() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);

	pTxDesc->QueueId = 0;	/*/WFM_AC_VO ; */
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_MGT);
	pTxDesc->Reserved = 0;
	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;
	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL
					);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management action frame to LMAC for transmission
	 */
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					DEFAULT_LINK_ID
					);

	if (Result != WFM_STATUS_SUCCESS)
		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);

exit_handler:
#endif		/* MGMT_FRAME_PROTECTION */
	return Result;
} /* end UMAC_ProcStartSaQuery() */

/******************************************************************************
 * NAME:	UMAC_ProcSaQuerySuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will stop sending SA Query Request as the valid SA Query
 * Response received from AP.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcSaQuerySuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
#if MGMT_FRAME_PROTECTION
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	/* Valid SA Query Response received */
	pUmacInstance->protectMgmtFrame.isValidSaQueryResp = TRUE;

	pUmacInstance->protectMgmtFrame.CheckSaQueryInterval = 0;

	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer);
#endif			/* MGMT_FRAME_PROTECTION */
	return Result;
} /* end UMAC_ProcSaQuerySuccess() */

/******************************************************************************
 * NAME:	UMAC_ProcSaQueryReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * The AP sent the SA Query Request to ensure sequire Association.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcSaQueryReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
#if MGMT_FRAME_PROTECTION
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_SA_QUERY_FRAME_BODY *pRxSaQueryReqFrm = NULL;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	uint8 EncHdrsize = 8;
	UMAC_SA_QUERY_FRAME_BODY SaQueryData;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(pMsg);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcSaQueryReq()\n");

	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pMsg);
	pRxSaQueryReqFrm = (UMAC_SA_QUERY_FRAME_BODY *) ((uint8 *) pUmacMgmt->FrameBody + EncHdrsize);

	SaQueryData.Action = UMAC_ACTION_TYPE_SA_QUERY_RESP;
	SaQueryData.Category = pRxSaQueryReqFrm->Category;
	SaQueryData.TransactionId = pRxSaQueryReqFrm->TransactionId;

	/*Send SA Query Response */
	Result = WFM_UMAC_EXEC_STATE_MACHINE(
						UmacHandle,
						UMAC_START_SAQUERY,
						&SaQueryData
						);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);
#endif			/* MGMT_FRAME_PROTECTION */
	return Result;
} /* end UMAC_ProcSaQueryReq() */

/******************************************************************************
 * NAME:	UMAC_ProcPostAssocIE
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Association success event, which will be generated
 * on the reception of an association response from the remote device with
 * status success.
 * -# Releases the Rx buffer and Rxdescriptor for Association Rsp frame
 * -# Selects the highest rate available as the current rate.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
uint32 UMAC_ProcPostAssocIE(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	WFM_UMAC_ASSOC_RSP_BODY *pAssocRsp = NULL;
	IeElement Ielement = { 0 };
	WFM_OUI_INFO OuiInfo = {
		WMM_OUI,
		WMM_IE_ELEMENT_TYPE,
		WFM_IE_PARAMETER_ELEMENT_SUB_TYPE
	};

	uint32 prio;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcPostAssocIE()\n");

	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pRxDescriptor);

	/*Cancel the UMAC Timer as we got a valid response */
	if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer))
		LOG_EVENT(DBG_WFM_ERROR, "Failed to Cancel UMAC Timer\n");


	/* Schedule a time to check for association still active */
	OS_START_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer,
		WFM_UMAC_CHECK_FOR_CONNECTION_TIMEOUT,
		UmacHandle
		);

	pUmacInstance->CheckForConnectionDataRxCtr = 0;

	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
	CHECK_POINTER_IS_VALID(pUmacMgmt);
	pAssocRsp = (WFM_UMAC_ASSOC_RSP_BODY *) pUmacMgmt->FrameBody;
	CHECK_POINTER_IS_VALID(pAssocRsp);

	/* Storing the Association id */
	pUmacInstance->AssociationId = pAssocRsp->AID;
	pUmacInstance->AssocStatusCode = pAssocRsp->StatusCode;
	pUmacInstance->CapabilitiesRsp = pAssocRsp->Capability;

	/* CPU - Little Endian */
	pUmacInstance->AssocRespIELen = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor) - (sizeof(WFM_UMAC_FRAME_HDR) + 6));	/* 6 is added for sizeof cap+statuscode+aid */

	/* Store Assoc Resp IE into pUmacInstance */
	if (pUmacInstance->AssocRespIELen <= sizeof(pUmacInstance->AssocRspIE)) {
		OS_MemoryCopy(
				pUmacInstance->AssocRspIE,
				pAssocRsp->IeData,
				pUmacInstance->AssocRespIELen
				);

	} else
		pUmacInstance->AssocRespIELen = 0;


	if (pUmacInstance->AssocRespIELen) {
		/*
		   Check if supported/basic supported rates have changed in
		   assoc response
		*/
		WFM_UMAC_AssocResponse_ProcessRates(
						UmacHandle,
						pAssocRsp->IeData,
						pUmacInstance->AssocRespIELen
						);
	}

	/*
	 * Default is no QOS/WMM. There is only one queue.
	 */
	for (prio = 0; prio < 8; prio++) {
		pUmacInstance->QosPrio2Queue[prio] = 0;
		pUmacInstance->PrioDowngradeMap[prio] = 0;
	}

	pUmacInstance->AC2DilQ[WFM_ACI_BE] = 0;
	pUmacInstance->AC2DilQ[WFM_ACI_BK] = 0;
	pUmacInstance->AC2DilQ[WFM_ACI_VI] = 0;
	pUmacInstance->AC2DilQ[WFM_ACI_VO] = 0;

	if (pUmacInstance->CapabilitiesRsp & WFM_UMAC_CAPABILITY_QOS) {
		uint8 best_nonacm = 1;
		IeElement IEelement;

		/* QoS is supported */
		pUmacInstance->Flags = (pUmacInstance->Flags | WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION);

		if ((WFM_UMAC_ProcIeElements(pAssocRsp->IeData, pUmacInstance->AssocRespIELen, UMAC_IE_ELEMENT_EDCA_PARAMETERS, &IEelement) == TRUE)
		    && (IEelement.size)
		    ) {
			WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED *pEdcaParam;

			pEdcaParam = (WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED *) IEelement.info;

			OS_MemoryCopy(
				&pUmacInstance->EdcaParam,
				pEdcaParam,
				sizeof(WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED)
				);
			pUmacInstance->WriteEdcaParam = 1;

			/* This is the 802.11 priority to AC map -
			 * Prio		AC	AC id
			 * 0,3		BE	0
			 * 1,2		BK	1
			 * 4,5		VI	2
			 * 6,7		VO	3
			 */
			pUmacInstance->QosPrio2Queue[7] = 3;
			pUmacInstance->QosPrio2Queue[6] = 3;
			pUmacInstance->QosPrio2Queue[5] = 2;
			pUmacInstance->QosPrio2Queue[4] = 2;
			pUmacInstance->QosPrio2Queue[3] = 0;
			pUmacInstance->QosPrio2Queue[2] = 1;
			pUmacInstance->QosPrio2Queue[1] = 1;
			pUmacInstance->QosPrio2Queue[0] = 0;

			/*
			 * Following map is used to downgrade 802.11 priority
			 * in case ACM is set for a particular AC. If no ACM
			 * is set, the priority is kept unaltered.
			 */

			/*
			 * BK is the lowest. No downgrade is possible
			 */
			pUmacInstance->PrioDowngradeMap[1] = 1;
			pUmacInstance->PrioDowngradeMap[2] = 2;
			best_nonacm = 1;

			if (pEdcaParam->Ac_BestEffort.AciAifn & 0x10) {
				pUmacInstance->PrioDowngradeMap[0] = best_nonacm;
				pUmacInstance->PrioDowngradeMap[3] = best_nonacm;
			} else {
				pUmacInstance->PrioDowngradeMap[0] = 0;
				pUmacInstance->PrioDowngradeMap[3] = 3;
				best_nonacm = 0;
			}
			if (pEdcaParam->Ac_Video.AciAifn & 0x10) {
				pUmacInstance->PrioDowngradeMap[4] = best_nonacm;
				pUmacInstance->PrioDowngradeMap[5] = best_nonacm;
			} else {
				pUmacInstance->PrioDowngradeMap[4] = 4;
				pUmacInstance->PrioDowngradeMap[5] = 5;
				best_nonacm = 4;
			}
			if (pEdcaParam->Ac_Voice.AciAifn & 0x10) {
				pUmacInstance->PrioDowngradeMap[6] = best_nonacm;
				pUmacInstance->PrioDowngradeMap[7] = best_nonacm;
			} else {
				pUmacInstance->PrioDowngradeMap[6] = 6;
				pUmacInstance->PrioDowngradeMap[7] = 7;
				best_nonacm = 6;
			}

			UMAC_CreateAc2DilQueueMap(
						UmacHandle,
						&pEdcaParam->Ac_BestEffort
						);
		}		/*if( IEelement.size ) */
	}			/*if (pUmacInstance->CapabilitiesRsp & WFM_UMAC_CAPABILITY_QOS ) */

	/*
	   Checking whether the Association Response has a WMM Parameter
	   Element in it. If there is one, this is a WMM Association
	*/

	WFM_UMAC_ProcVendorSpecificIeElements(
						pAssocRsp->IeData,
						pUmacInstance->AssocRespIELen,
						&OuiInfo,
						&Ielement
						);

	if (Ielement.size) {
		uint8 best_nonacm = 0;
		WFM_WMM_PARAM_ELEMENT *pwmmParamElement = (WFM_WMM_PARAM_ELEMENT *) Ielement.info;
		/*
		  WMM Element is there , so AP supports WMM. This is WMM
		  Association
		*/
		pUmacInstance->Flags = (pUmacInstance->Flags | WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION);

		OS_MemoryCopy(
				&pUmacInstance->EdcaParam,
				&pwmmParamElement->QoSInfo,
				sizeof(WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED)
				);
		pUmacInstance->WriteEdcaParam = 1;

		/* This is the 802.11 priority to AC map -
		 * Prio		AC	AC id
		 * 0,3		BE	0
		 * 1,2		BK	1
		 * 4,5		VI	2
		 * 6,7		VO	3
		 */
		pUmacInstance->QosPrio2Queue[7] = 3;
		pUmacInstance->QosPrio2Queue[6] = 3;
		pUmacInstance->QosPrio2Queue[5] = 2;
		pUmacInstance->QosPrio2Queue[4] = 2;
		pUmacInstance->QosPrio2Queue[3] = 0;
		pUmacInstance->QosPrio2Queue[2] = 1;
		pUmacInstance->QosPrio2Queue[1] = 1;
		pUmacInstance->QosPrio2Queue[0] = 0;

		/*
		 * Following map is used to downgrade 802.11 priority
		 * in QOS Control Field, in case ACM is set for a
		 * particular AC. If no ACM is set, priority remains unchanged.
		 */

		/*
		 * BK is the lowest. No downgrade is possible
		 */
		pUmacInstance->PrioDowngradeMap[1] = 1;
		pUmacInstance->PrioDowngradeMap[2] = 2;
		best_nonacm = 1;

		if (pwmmParamElement->Ac_BestEffort.AciAifn & 0x10) {
			pUmacInstance->PrioDowngradeMap[0] = best_nonacm;
			pUmacInstance->PrioDowngradeMap[3] = best_nonacm;
		} else {
			pUmacInstance->PrioDowngradeMap[0] = 0;
			pUmacInstance->PrioDowngradeMap[3] = 3;
			best_nonacm = 0;
		}
		if (pwmmParamElement->Ac_Video.AciAifn & 0x10) {
			pUmacInstance->PrioDowngradeMap[4] = best_nonacm;
			pUmacInstance->PrioDowngradeMap[5] = best_nonacm;
		} else {
			pUmacInstance->PrioDowngradeMap[4] = 4;
			pUmacInstance->PrioDowngradeMap[5] = 5;
			best_nonacm = 4;
		}
		if (pwmmParamElement->Ac_Voice.AciAifn & 0x10) {
			pUmacInstance->PrioDowngradeMap[6] = best_nonacm;
			pUmacInstance->PrioDowngradeMap[7] = best_nonacm;
		} else {
			pUmacInstance->PrioDowngradeMap[6] = 6;
			pUmacInstance->PrioDowngradeMap[7] = 7;
			best_nonacm = 6;
		}

		UMAC_CreateAc2DilQueueMap(
					UmacHandle,
					&pwmmParamElement->Ac_BestEffort
					);
	}			/*if( Ielement.size ) */

	if (!(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) &&
		   !(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION)
	    ) {
		/*
		   It's not QoS Association or WMM Association, so we have to
		   derive the values correspondig to EDCA parameter set.
		 */
		pUmacInstance->WriteEdcaParam = 0;
	}

	WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

	return Result;
} /* end UMAC_ProcPostAssocIE() */

/*static */
void UMAC_ManageRateSelection_TryAllRates(WFM_UMAC_INSTANCE *pUmacInstance,
					  uint32 TxStatus,
					  uint32 RateAtWhichLastTxed,
					  uint32 NumRetries)
{
	static int try_higher;

	/*send in all HT rates */

	if (try_higher == 1) {
		if (pUmacInstance->CurrentRateIndex == WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->CurrentModeOpRates))
			try_higher = 0;
		else
			pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_NEXT_HIGHER_RATE_INDEX(pUmacInstance->CurrentModeOpRates, pUmacInstance->CurrentRateIndex);

	}

	if (try_higher == 0) {
		if (pUmacInstance->CurrentRateIndex == WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->CurrentModeOpRates)) {
			try_higher = 1;
			pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_NEXT_HIGHER_RATE_INDEX(pUmacInstance->CurrentModeOpRates, pUmacInstance->CurrentRateIndex);
		} else {
			pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(pUmacInstance->CurrentModeOpRates, pUmacInstance->CurrentRateIndex);
		}
	}
	LOG_DATA(DBG_WFM_RATE_ADAPTATION, "New Tx Rate = %d\n", pUmacInstance->CurrentRateIndex);

} /* end UMAC_ManageRateSelection_TryAllRates() */

/******************************************************************************
 * NAME:	UMAC_ManageRateSelection
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * #if STATISTICAL_RATE_ADAPTATION
 * This rate adaptation mechanism is based on the TXed rate statistics, computed on certain
 * number of frames.
 * -# If  the number of retries received in Tx CNF is 0, it is considered as success and 256 points
 *     are added into the statistics otherwise it is considered as failure and no points are added.
 * -# After statistics is collected for (WFM_UMAC_RATE_MOVE_UP_AVERAGE - 1) frames, average
 *    statistics is computed .
 * -# If avg statistics is >= 90% (more than 90% frames were transmitted in the first attempt), we
 *    move to the next higher rate available.
 * -# If avg statistics is <= 50% (more than 50% frames were transmitted after retrying atleast once),
 *    we move to the next lower rate available.
 *
 * #endif
 *
 * This function manages the next rate selection based on the
 * transmission status.
 *   -# There is a counter which is incremented if tx is successful,
 *      decremented if unsuccessful
 *   -# There are "history counters" for all possible tx rates. This is incremented
 *      for a rate when we go to a higher rate. It is decremented for a rate when
 *      we go to a lower rate.
 *   -#
 *      -# If at current tx rate, 10 (WFM_UMAC_RATE_ADAP_NUM_SUCCESS_FOR_TX_RATE_CHANGE)
 *	 tx were successful, then move to next higher rate.
 *      -# When going to higher rate, if there is no higher rate in that mode, then
 *	 move to HT mode (if avail) or to Ofdm mode.
 *      -# Check if history at this new rate is good (more than -5
 *	 (WFM_UMAC_RATE_ADAP_THRESHOLD_BAD_HISTORY)), then move to it,
 *	 otherwise keep the orig rate/mode.
 *   -#
 *      -# If at current rate, 10 (WFM_UMAC_RATE_ADAP_NUM_FAILURE_FOR_TX_RATE_CHANGE)
 *	 tx were unsuccessful, then move to next lower rate.
 *      -# And, mark that rate as bad rate in history.
 *      -# When going to lower rate, if there is no lower rate in that mode,
 *	 then move to OFDM mode (if avail) or to DSSS mode (if avail).
 *   -# Clear bad history every 15 mins (WFM_UMAC_RATE_ADAP_CLEAR_BAD_HISTORY_TIMEOUT),
 *      that is, make the history counter for all rates as value
 *      (WFM_UMAC_RATE_ADAP_THRESHOLD_BAD_HISTORY+1) so that wfm can try
 *      transmitting on that rate once.
 * \param pUmacInstance	 - Pointer to UMAC instance
 * \param TxStatus	      - Status of the current transmission.
 * \param RateAtWhichLastTxed   - Last successful Tx rate.
 * \param NumRetries	    - Number of retries for last packet.
 * \returns none.
 *********************************************************************************/
void UMAC_ManageRateSelection(WFM_UMAC_INSTANCE *pUmacInstance,
			      uint32 TxStatus,
			      uint32 RateAtWhichLastTxed,
			      uint32 NumRetries,
			      uint8 linkId)
{
	if (pUmacInstance->statisticCount[linkId] == 0)
		pUmacInstance->statisticAvg[linkId] = 0;


	if (NumRetries == 0)
		pUmacInstance->statisticAvg[linkId] += WFM_UMAC_TX_SUCCESS_CREDIT;

	pUmacInstance->statisticCount[linkId]++;

	if (pUmacInstance->gVars.p.BtActive && ((1 << pUmacInstance->CurrentRateIndex) & WFM_UMAC_EPTA_UNPRFERRED_RATE_SET)
	    && pUmacInstance->statisticCount[linkId] == WFM_UMAC_AGGRESSIVE_RATE_WINDOW) {
		uint32 CreditAvg;
		CreditAvg = (pUmacInstance->statisticAvg[linkId] * 100) / (WFM_UMAC_TX_SUCCESS_CREDIT * WFM_UMAC_AGGRESSIVE_RATE_WINDOW);
		if (CreditAvg >= 70) {
			pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_NEXT_HIGHER_RATE_INDEX(pUmacInstance->TxRatesBitmap[linkId], pUmacInstance->CurrentRateIndex);
			pUmacInstance->statisticCount[linkId] = 0;
		}
	} else if (pUmacInstance->statisticCount[linkId] == WFM_UMAC_RATE_EVALUATION_WINDOW) {
		uint32 CreditAvg;
		CreditAvg = (pUmacInstance->statisticAvg[linkId] * 100) / (WFM_UMAC_TX_SUCCESS_CREDIT * WFM_UMAC_RATE_EVALUATION_WINDOW);
		if (CreditAvg > WFM_UMAC_RATE_MOVE_UP_AVERAGE) {
#if P2P_MINIAP_SUPPORT
			if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
				pUmacInstance->sta[linkId].currentRateIndex = (uint8)WFM_UMAC_GET_NEXT_HIGHER_RATE_INDEX(pUmacInstance->TxRatesBitmap[linkId], pUmacInstance->sta[linkId].currentRateIndex);
			else
#endif	/* P2P_MINIAP_SUPPORT */
				pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_NEXT_HIGHER_RATE_INDEX(pUmacInstance->TxRatesBitmap[linkId], pUmacInstance->CurrentRateIndex);
		} else if (CreditAvg < WFM_UMAC_RATE_FALL_DOWN_AVERAGE) {
#if P2P_MINIAP_SUPPORT
			if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
				pUmacInstance->sta[linkId].currentRateIndex = (uint8)WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(pUmacInstance->TxRatesBitmap[linkId], pUmacInstance->sta[linkId].currentRateIndex);
			else
#endif	/* P2P_MINIAP_SUPPORT */
				pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(pUmacInstance->TxRatesBitmap[linkId], pUmacInstance->CurrentRateIndex);
		}

		pUmacInstance->statisticCount[linkId] = 0;
	}
} /* end UMAC_ManageRateSelection() */

/******************************************************************************
 * NAME:	UMAC_RateAdapTimeOut_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback for rate adaptation. This will go through
 * the history of failing rates, and will try to clear up some bad history.
 * This will only work when Associated.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_RateAdapTimeOut_Cb(void *Handle)
{
	int i;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) Handle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_RATE_ADAPTATION, "UMAC: UMAC_RateAdapTimeOut_Cb - clearing bad history\n");

	if (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) {
		for (i = 0; i < WFM_UMAC_NUM_TX_RATES; i++) {
			if ((pUmacInstance->RateAdapHistoryTxRate[i]) <= (WFM_UMAC_RATE_ADAP_THRESHOLD_BAD_HISTORY)
			    ) {
				/*
				   if the history was bad for this rate, we
				   want to give another chance to this rate
				 */
				pUmacInstance->RateAdapHistoryTxRate[i] = WFM_UMAC_RATE_ADAP_THRESHOLD_BAD_HISTORY + 1;
			}
		}		/*for(i=0; i<WFM_UMAC_NUM_TX_RATES; i++) */

		/*start the timer again */
		OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer,
			WFM_UMAC_RATE_ADAP_CLEAR_BAD_HISTORY_TIMEOUT,
			(UMAC_HANDLE) Handle
			);
	} /* if( pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED ) */
} /* end UMAC_RateAdapTimeOut_Cb() */

/******************************************************************************
 * NAME:	UMAC_PerformReAssociation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Perform ReAssociation.
 * \param UmacHandle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_PerformReAssociation(UMAC_HANDLE UmacHandle)
{
	uint32 Result;
	int Count;
	int CountTx = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	if (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) {
		for (Count = 0; Count < CFG_HI_NUM_REQS; Count++) {
			if (pUmacInstance->gVars.TxDescStore[Count].pTxDesc != NULL) {
				CountTx++;
				break;
			}
		}

		if (CountTx == 0) {
			/*no pending Tx */
			LOG_EVENT(DBG_WFM_ASSOC, "UMAC_PerformReAssociation()\n");
			Result = WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_START_REASSOC,
							NULL
							);

			if (WFM_STATUS_PENDING == Result)
				OS_ASSERT(0);
		}
	} /* if( pUmacInstance->UmacCurrentState  == UMAC_ASSOCIATED) */
} /* end UMAC_PerformReAssociation() */

/******************************************************************************
 * NAME:	UMAC_CheckForConnectionTimeOut_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback. If on data frames received after
 * association, it means that we might be disassociated. Perform ReAssociation
 * to check for that case.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_CheckForConnectionTimeOut_Cb(void *Handle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) Handle;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC, "UMAC: UMAC_CheckForConnectionTimeOut_Cb - checking if AP is here\n");
	if (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) {
		/*
		some tx activity going on, but no reply being recd for a
		long time
		*/
		/* nothing being received and something being transmitted */
		if ((pUmacInstance->CheckForConnectionDataRxCtr == 0)
			&& (pUmacInstance->CheckForConnectionDataTxCtr != 0)
			) {
				/* we wud be scheduled by re-assoc success */
				UMAC_PerformReAssociation((UMAC_HANDLE) Handle);
		} else {
			pUmacInstance->CheckForConnectionDataRxCtr = 0;
			OS_START_TIMER(
				pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer,
				WFM_UMAC_CHECK_FOR_CONNECTION_TIMEOUT,
				(UMAC_HANDLE) Handle
				);
		}
		pUmacInstance->CheckForConnectionDataTxCtr = 0;
	} /* if( pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED ) */
} /* end UMAC_CheckForConnectionTimeOut_Cb() */

/******************************************************************************
 * NAME:	UMAC_TimeOut_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback.If UMAC is waiting for a response from
 * some device and its not getting it for some reason for
 * WFM_UMAC_MGMT_FRAME_TIMEOUT milliseconds, this function will be triggered.
 * All this function does is to change the UMAC state back to UMAC_INITIALIZED.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_TimeOut_Cb(void *Handle)
{
	uint32 Result;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) Handle;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_ASSOC | DBG_WFM_ERROR, "\nUMAC: ERROR: Auth_Resp/[Re]Assoc_Resp not recd! - UMAC_TimeOut_Cb\n\n");
	if (pUmacInstance->UmacCurrentState < UMAC_ASSOCIATED) {
		/*
		   If state is less than associated means, the system was trying to
		   establish a link, and it got timedout
		 */
		WFM_UMAC_2_UL_SendEvent(
					(UMAC_HANDLE) Handle,
					WFM_EVT_CONNECT_FAILED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
	}			/*if( pUmacInstance->UmacCurrentState < UMAC_ASSOCIATED ) */

#if FT_SUPPORT
	if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED)
	    && pUmacInstance->FtFlags & (UMAC_FT_FLAG_AUTH_OVER_AIR_BIT | UMAC_FT_FLAG_AUTH_OVER_DS_BIT)) {
		Result = WFM_UMAC_EXEC_STATE_MACHINE(
							(UMAC_HANDLE) Handle,
							UMAC_FT_AUTH_COMPLT,
							NULL
							);
	} else
#endif	/* FT_SUPPORT */
	if ((pUmacInstance->UmacCurrentState == UMAC_AUTHENTICATED) || (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATING) || (pUmacInstance->UmacCurrentState == UMAC_REASSOCIATING)) {
		LOG_EVENT(DBG_WFM_UMAC, "UMAC_DE_AUTHE\n");
		/*
		   We are already Authenticated, so De-Authenticate
		 */
		Result = WFM_UMAC_EXEC_STATE_MACHINE(
							(UMAC_HANDLE) Handle,
							UMAC_DE_AUTHE,
							NULL
							);

		if (WFM_STATUS_PENDING == Result)
			OS_ASSERT(0);
	} else {
		LOG_EVENT(DBG_WFM_UMAC, "UMAC_UNJOIN\n");

		/*
		   Forcing to UMAC_JOINED state to trigger an unjoin and reset
		   the state machine
		 */

		UMAC_SET_STATE(pUmacInstance, UMAC_JOINED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_JOINED\n");

		/*Start Un-Join from here */

		WFM_SCHEDULE_TO_SELF((UMAC_HANDLE) Handle, UMAC_UNJOIN, NULL);
	}
} /* end UMAC_TimeOut_Cb() */

/******************************************************************************
 * NAME:	UMAC_CheckIfUnJoinCanProceedTimeOut_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Used for scheduling unjoin at a later time.This is useful if some pending tx
 * are present.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_CheckIfUnJoinCanProceedTimeOut_Cb(void *Handle)
{
	UMAC_CONNECTION_HANDLE *pUmacConHandle = (UMAC_CONNECTION_HANDLE *) Handle;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pUmacConHandle->UmacHandle;
	uint8 linkId = pUmacConHandle->linkId;
	LOG_EVENT(DBG_WFM_ASSOC | DBG_WFM_CURRENT_AK, "UMAC: UMAC_CheckIfUnJoinCanProceedTimeOut_Cb-checking \
						      if UnJoin can be called\n");
#if P2P_MINIAP_SUPPORT
	if (pUmacInstance == NULL)
		return;

	pUmacInstance->sta[linkId].hiMsgHdr.MsgId = linkId<<6;
	pUmacInstance->sta[linkId].state = STA_STATE_DEAUTHENTICATING;
	if (linkId > 0) {
		if (pUmacInstance->operatingMode == OPERATING_MODE_GO)
			UMAC_SET_STATE(pUmacConHandle->UmacHandle, UMAC_GROUP_FORMED);
		else
		UMAC_SET_STATE(pUmacConHandle->UmacHandle, UMAC_BSS_STARTED);

		if (linkId != 0)
			OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId]);
		WFM_SCHEDULE_TO_SELF(
					(UMAC_HANDLE) pUmacConHandle->UmacHandle,
					UMAC_UNLINK_STA,
					(void *)&pUmacInstance->sta[linkId].hiMsgHdr
					);
	} else {
#endif	/* P2P_MINIAP_SUPPORT */
		if ((pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID] == 0) && (pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID] == 0)) {
			pUmacInstance->UnjoinTimerCount = 0;
			WFM_SCHEDULE_TO_SELF((UMAC_HANDLE) pUmacConHandle->UmacHandle, UMAC_UNJOIN, NULL);
		} else {
			pUmacInstance->UnjoinTimerCount++;
			if (pUmacInstance->UnjoinTimerCount >= 50) {
				pUmacInstance->UnjoinTimerCount = 0;
				/* Call force Join */
				LOG_EVENT(DBG_WFM_ASSOC | DBG_WFM_CURRENT_AK | DBG_WFM_ERROR, "UMAC: UMAC_CheckIfForceUnJoinCanProceedTimeOut_Cb Called UMAC_UNJOIN_SUCCESS\n");
				pUmacInstance->extendedUnJoinTimer = TRUE;
				UMAC_SET_STATE((UMAC_HANDLE) pUmacConHandle->UmacHandle, UMAC_UNJOINING);
				WFM_SCHEDULE_TO_SELF((UMAC_HANDLE) pUmacConHandle->UmacHandle, UMAC_UNJOIN_SUCCESS, NULL);
			} else {
				LOG_DATA(DBG_WFM_UMAC | DBG_WFM_ASSOC | DBG_WFM_ERROR, "NumRxBufPending = %d\n", pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]);
				LOG_DATA(DBG_WFM_UMAC | DBG_WFM_ASSOC | DBG_WFM_ERROR, "NumTxBufPending = %d\n", pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]);
				/*There are outstanding Tx or Rx buffers, so
				Scheduling Unjoin for future */
				pUmacInstance->sta[DEFAULT_LINK_ID].UmacConHandle.linkId = DEFAULT_LINK_ID;
				pUmacInstance->sta[DEFAULT_LINK_ID].UmacConHandle.UmacHandle = pUmacConHandle->UmacHandle;
				OS_START_TIMER(pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer, WFM_UMAC_UNJOIN_TIMEOUT, (UMAC_CONNECTION_HANDLE *)&pUmacInstance->sta[DEFAULT_LINK_ID].UmacConHandle);
			}
		}
#if P2P_MINIAP_SUPPORT
	}
#endif	/* P2P_MINIAP_SUPPORT */

} /* end UMAC_CheckIfUnJoinCanProceedTimeOut_Cb() */

#if 0
/******************************************************************************
 * NAME:	UMAC_CancelRemainOnChTimeOut_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Used for scheduling Cancel Remain on Channel request at a later time.
 * This is useful if some pending tx are present.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_CancelRemainOnChTimeOut_Cb(void *Handle)
{
	OS_ASSERT(Handle);

	UMAC_MOVE_PENDING_MSGS_TO_ACTIVE_Q(Handle);
} /* end UMAC_CancelRemainOnChTimeOut_Cb() */
#endif

#if DOT11K_SUPPORT
/******************************************************************************
 * NAME:	UMAC_MeasPauseTimeOut_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Used for measurment pause time out
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_MeasPauseTimeOut_Cb(void *Handle)
{
	uint32 Result;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) Handle;
	uint16 UmacEvent = WFM_EVT_MAX;
	UmacEvent = UMAC_SEND_MEASUREMENT_REQ;
	/*Stop the timer here */
	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer);
	/*Call the send measurement request function from here */
	Result = WFM_UMAC_EXEC_STATE_MACHINE(
						(UMAC_HANDLE) Handle,
						UmacEvent,
						NULL
						);
	if (WFM_STATUS_PENDING == Result)
		UMAC_QUEUE_PENDING_TO_SELF_MSG(
						(UMAC_HANDLE) Handle,
						UmacEvent,
						NULL
						);
}

/******************************************************************************
 * NAME:	UMAC_TxStreamMeasTimeOut_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Used for frame measurement Time Out
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_TxStreamMeasTimeOut_Cb(void *Handle)
{
	uint32 Result;
	uint16 UmacEvent = UMAC_MEASUREMENT_REPORT;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) Handle;
	/*Stop the timer here */
	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer);
	pUmacInstance->dot11k_Meas_Params.transmitStreamUnderProcess = 0;
	/*Call the measurement report function */
	Result = WFM_UMAC_EXEC_STATE_MACHINE(
						(UMAC_HANDLE) Handle,
						UmacEvent,
						NULL
						);
	if (WFM_STATUS_PENDING == Result) {
		UMAC_QUEUE_PENDING_TO_SELF_MSG(
						(UMAC_HANDLE) Handle,
						UmacEvent,
						NULL
						);
	}
}

#endif	/* DOT11K_SUPPORT */

#if MGMT_FRAME_PROTECTION
/******************************************************************************
 * NAME:	UMAC_SaQueryRetryTimeOut_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback. This will send the SA query request after
 * every SA Query retry interval if the valid SA query Response is not received.
 * If the valid response recevied then it will not send next SA Query request.
 * If the valid SA Query response not recevied till Max SA Query timeout
 * interval, then it will release all resources allocated for this AP.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_SaQueryRetryTimeOut_Cb(void *Handle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) Handle;
	UMAC_SA_QUERY_FRAME_BODY SaQueryData;
	uint32 Result;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC, "UMAC: UMAC_SaQueryRetryTimeOut_Cb\n");

	if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
								     && (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {
		if (pUmacInstance->protectMgmtFrame.isValidSaQueryResp == FALSE) {
			/*
			  Valid SA Query Response not received so send new SA
			  Query Request
			*/
			SaQueryData.Action = UMAC_ACTION_TYPE_SA_QUERY_REQ;
			SaQueryData.Category = UMAC_CATEGORY_SA_QUERY;
			SaQueryData.TransactionId = pUmacInstance->protectMgmtFrame.tansactionIdentifer;
			/*Send SA Query Request */
			Result = WFM_UMAC_EXEC_STATE_MACHINE(
							(UMAC_HANDLE) Handle,
							UMAC_START_SAQUERY,
							&SaQueryData
							);
			if (pUmacInstance->protectMgmtFrame.CheckSaQueryInterval < pUmacInstance->saQueryInterval.saQueryMaxTimeout) {
				/*Start Timer for SA Query retry interval */
				OS_START_TIMER(
					pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer,
					pUmacInstance->saQueryInterval.saQueryRetryTimeout,
					(UMAC_HANDLE) Handle
					);
				pUmacInstance->protectMgmtFrame.CheckSaQueryInterval += pUmacInstance->saQueryInterval.saQueryRetryTimeout;
				pUmacInstance->protectMgmtFrame.tansactionIdentifer++;
			} else {
				/*
				  If the valid SA Query response not recevied
				  till Max SA Query timeout interval, then
				  release all resources allocated for this AP.
				*/
				pUmacInstance->protectMgmtFrame.CheckSaQueryInterval = 0;
				WFM_SCHEDULE_TO_SELF(
							(UMAC_HANDLE) Handle,
							UMAC_DE_ASSOC,
							NULL
							);
			}
		} else
			pUmacInstance->protectMgmtFrame.tansactionIdentifer = 0;
	} /* if( pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED ) */
} /* end UMAC_SaQueryRetryTimeOut_Cb() */

/******************************************************************************
 * NAME:	UMAC_ReAssocTimeOutInterval_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback. This will send the Re-Association Request.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_ReAssocTimeOutInterval_Cb(void *Handle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) Handle;
	uint32 Result;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC, "UMAC: UMAC_ReAssocTimeOutInterval_Cb\n");

	/*Send ReAssociation Request only when UMAC state is Associated */
	if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
								     && (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {
		Result = WFM_UMAC_EXEC_STATE_MACHINE(
							(UMAC_HANDLE) Handle,
							UMAC_START_REASSOC,
							NULL
							);
	} /* if( pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED ) */
} /* end UMAC_ReAssocTimeOutInterval_Cb() */
#endif				/*MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:	UMAC_ClientConnectionTimeout_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback.If UMAC is waiting for a P2P invitation
 * response from some device and its not getting it for some reason for
 * WFM_UMAC_MGMT_FRAME_TIMEOUT milliseconds, this function will be triggered.
 * This function resets the link on which the STA is associated.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_ClientConnectionTimeout_Cb(void *Handle)
{
	UMAC_CONNECTION_HANDLE *pUmacConHandle = (UMAC_CONNECTION_HANDLE *) Handle;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pUmacConHandle->UmacHandle;
	uint8 linkId = pUmacConHandle->linkId;

	OS_ASSERT(pUmacInstance);

	pUmacInstance->sta[linkId].hiMsgHdr.MsgId = linkId << 6;
	pUmacInstance->sta[linkId].PsState = STA_PS_STATE_AWAKE;
	/* De-Authenticate */
	WFM_SCHEDULE_TO_SELF((UMAC_HANDLE)pUmacInstance, UMAC_DEAUTH_STA, &(pUmacInstance->sta[linkId].hiMsgHdr))  ;

} /* UMAC_ClientConnectionTimeout_Cb() */

/******************************************************************************
 * NAME:	UMAC_P2P_InvitationTimeout_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback.If UMAC is waiting for a P2P invitation
 * response from some device and its not getting it for some reason for
 * WFM_UMAC_MGMT_FRAME_TIMEOUT milliseconds, this function will be triggered.
 * All this function does is to change the UMAC state back to UMAC_INITIALIZED.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_GroupFormationTimeout_Cb(void *Handle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) Handle;

	WFM_UMAC_2_UL_SendEvent(
				(UMAC_HANDLE) Handle,
				WFM_EVT_GROUP_FORMATION_FAILED,
				0,
				NULL,
				WFM_UMAC_EVT_NOT_RETRANSMIT
				);
	pUmacInstance->operatingMode = OPERATING_MODE_P2P_DEVICE;

} /* UMAC_GroupFormationTimeout_Cb() */

/******************************************************************************
 * NAME:	UMAC_P2P_InvitationTimeout_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback.If UMAC in miniAP/GO mode has not received
*  any frame from an associated STA for some reason for MGMT FRAME TIMEOUT
*  milliseconds, this function will be triggered.
 * All this function does is to change the UMAC state back to UMAC_INITIALIZED.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_P2P_InvitationTimeout_Cb(void *Handle)
{

	WFM_UMAC_2_UL_SendEvent(
				(UMAC_HANDLE) Handle,
				WFM_EVT_INVITATION_FAILED,
				0,
				NULL,
				WFM_UMAC_EVT_NOT_RETRANSMIT
				);

} /* end UMAC_P2P_InvitationTimeout_Cb() */

/******************************************************************************
 * NAME:	UMAC_ConnectionDropTimeout_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback.If UMAC in miniAP/GO mode has not received
 * any frame from an associated STA for some reason for CLIENT INACTIVITY
 * TIMEOUT milliseconds, this function will be triggered.
 * This function deauthenticates the associated STA due to inactivity.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_ConnectionDropTimeout_Cb(void *Handle)
{
	UMAC_CONNECTION_HANDLE *pUmacConHandle = (UMAC_CONNECTION_HANDLE *) Handle;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) pUmacConHandle->UmacHandle;
	uint8 linkId = pUmacConHandle->linkId;

	if (pUmacInstance == NULL)
		return;

	OS_LOCK(pUmacInstance->gVars.p.StaDataBase);
	if ((pUmacInstance->sta[linkId].state != STA_STATE_DEAUTHENTICATING)
		&& pUmacInstance->sta[linkId].state != STA_STATE_NONE) {
		pUmacInstance->sta[linkId].Inactive = TRUE;
		pUmacInstance->sta[linkId].hiMsgHdr.MsgId = linkId << 6;
		pUmacInstance->sta[linkId].PsState = STA_PS_STATE_AWAKE;
		OS_UNLOCK(pUmacInstance->gVars.p.StaDataBase);
	} else {
		OS_UNLOCK(pUmacInstance->gVars.p.StaDataBase);
		return;
	}

	WFM_SCHEDULE_TO_SELF(
				(UMAC_HANDLE) pUmacInstance,
				UMAC_DEAUTH_STA,
				&(pUmacInstance->sta[linkId].hiMsgHdr)
				);

} /* end UMAC_ConnectionDropTimeout_Cb */

/******************************************************************************
 * NAME:	UMAC_CleanupTimeout_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC's cleanup timer callback. This callback is called every
 * UMAC_AP_PS_BUFFER_CLEANUP_TIMER interval.It scans the
 * AP_BUFF_TYPES_DLVRY_ENABLED buffer and discards the buffered frames which
 * are present for more than a certain amount of time.
 * \param *UmacHandle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_CleanupTimeout_Cb(void *UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pStoredTxDesc;
	UMAC_TX_PKT *pUmacTxPkt;
	UMAC_TX_DATA_CNF *pTxDataCnf;
	WFM_HI_MESSAGE *pHiMessage = NULL;
	uint8 linkId, index, i, numBufferedFrames, BuffType;
	uint32 currentTime;
	uint32 get;
	void *pDriverInfo = NULL;

	for (linkId = 1; linkId < pUmacInstance->gVars.p.maxNumClientSupport; linkId++) {
		uint8 ClearTim = 0;
		if (pUmacInstance->sta[linkId].state == STA_STATE_NONE)
			continue;
		OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
		for (BuffType = 0; BuffType <= AP_BUFF_TYPES_DLVRY_ENABLED; BuffType++) {
			numBufferedFrames = pUmacInstance->sta[linkId].numPendingFrames[BuffType];
			get = pUmacInstance->sta[linkId].get[BuffType];
			for (i = 0; i < numBufferedFrames; i++) {
				UMAC_DEVIF_TX_DATA *pDevIfTxRequest;
				UMAC_TX_DESC *pTxDescriptor = NULL;
				index = (uint8) (get & (AP_NUM_BUFFERS - 1));
				currentTime = OS_GetTime();

				if (currentTime > pUmacInstance->sta[linkId].buffer[BuffType][index].timeStamp) {
					pDevIfTxRequest = (UMAC_DEVIF_TX_DATA *) pUmacInstance->sta[linkId].buffer[BuffType][index].buffPtr;
					pTxDescriptor = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxRequest + sizeof(UMAC_DEVIF_TX_DATA));
					get++;
					pUmacInstance->sta[linkId].get[BuffType] = get;

					if (pTxDescriptor->PacketId & UMAC_FWDED_FRAME) {
						pStoredTxDesc = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxRequest + (sizeof(UMAC_TX_DESC)
												   - sizeof(UMAC_RX_DESC)));

						UMAC_Release_Internal_TxDesc(UmacHandle, pTxDescriptor, pUmacInstance->gVars.ApUmacTxDesc, UMAC_NUM_AP_INTERNAL_TX_BUFFERS, &pDriverInfo);
						UMAC_RELEASE_PACKET_ID_AP(UmacHandle, pTxDescriptor->PacketId);

						UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle,
								pStoredTxDesc,
								pDriverInfo);
					} else {
						/* The packet came from network stack */
						pUmacTxPkt = (UMAC_TX_PKT *) &pUmacInstance->gVars.TxDescStore[pTxDescriptor->PacketId & (CFG_HI_NUM_REQS - 1)];

						if (pUmacTxPkt->pTxDesc == NULL) {
							/* Release the internally allocated memory */
							UMAC_Release_Internal_TxDesc(UmacHandle,
								pTxDescriptor,
								pUmacInstance->gVars.ApUmacTxDesc,
								UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
								&pDriverInfo
								);
							UMAC_RELEASE_PACKET_ID(UmacHandle, pTxDescriptor->PacketId);
							UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pDevIfTxRequest);
						} else if (pUmacTxPkt->pDevIfTxReq == pDevIfTxRequest) {
							OS_LOCK(pUmacInstance->gVars.p.TxDescStoreLock);

							pTxDataCnf = (UMAC_TX_DATA_CNF *) pUmacTxPkt->pMsgPtr;

							OS_ASSERT(pTxDataCnf);

							pUmacTxPkt->pMsgPtr = NULL;

							pHiMessage = (WFM_HI_MESSAGE *) pTxDataCnf->pUlHdr;

							OS_ASSERT(pHiMessage);

							pHiMessage->PayLoad.wfm_transmit_cnf.Reference = pUmacTxPkt->reference;

							pUmacTxPkt->reference = 0;

							WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacTxPkt->pTxDesc);

							UMAC_Release_Internal_TxDesc(UmacHandle,
								pUmacTxPkt->pTxDesc,
								pUmacInstance->gVars.ApUmacTxDesc,
								UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
								&pDriverInfo
								);
							UMAC_RELEASE_PACKET_ID(UmacHandle, pTxDescriptor->PacketId);
							/*Releasing the Tx Descriptor */
							UMAC_LL_RELEASE_TX_DESC(UmacHandle, pUmacTxPkt->pTxDesc);

							pUmacTxPkt->pTxDesc = NULL;
							OS_UNLOCK(pUmacInstance->gVars.p.TxDescStoreLock);

							pHiMessage->PayLoad.wfm_transmit_cnf.Status = WFM_STATUS_REQ_REJECTED;

							pHiMessage->MessageId = WFM_TRANSMIT_CNF_ID;
							pHiMessage->MessageLength = (WFM_HI_MESSAGE_HDR_SIZE + sizeof(WFM_TRANSMIT_CNF));

							OS_ASSERT(pUmacInstance->gVars.p.UpperLayerTxcnf);

							pUmacInstance->gVars.p.UpperLayerTxcnf(pUmacInstance->gVars.p.ulHandle, pTxDataCnf);
						}
					}

					pUmacInstance->sta[linkId].numPendingFrames[BuffType] = (uint8) (pUmacInstance->sta[linkId].put[BuffType] - get);
				}
			}
		}
		if ((pUmacInstance->sta[linkId].qosInfo & UMAC_QOS_INFO_STA_UAPSD_MASK) == UMAC_QOS_INFO_STA_UAPSD_MASK) {
			if (!pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED])
				ClearTim = 1;
		} else {
			if (!pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_LEGACY])
				ClearTim = 1;
		}
		OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
		if (ClearTim && (pUmacInstance->virtualBitmap & (1 << linkId))) {
			pUmacInstance->virtualBitmap &= (0xFF ^ (1 << linkId));
			if (pUmacInstance->updateTimIeInProgress == FALSE) {
				pUmacInstance->updateTimIeInProgress = TRUE;
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
				WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);
			} else
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
		} else
			OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);

		OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
	}

	OS_START_TIMER(
		pUmacInstance->gVars.p.UmacTimers.pUMACCleanupTimer,
		UMAC_AP_PS_BUFFER_CLEANUP_TIMER,
		(UMAC_HANDLE) UmacHandle
		);
} /* end UMAC_CleanupTimeout_Cb */

#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:	UMAC_CreateAc2DilQueueMap
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Create Access Catogery to DIL queue Mapping.
 * \param UmacHandle	-  Handle to UMAC Instance.
 * \param pAcParam	-  Access Category Parameters.
 * \returns none.
 *****************************************************************************/
static void UMAC_CreateAc2DilQueueMap(UMAC_HANDLE UmacHandle,
				      WFM_WMM_AC_PARAM_PACKED *pAcParam)
{
	/* local data structure */
	struct ac_param {
		uint8 aifs;
		uint8 order;
	} ac_param[4];

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 i, j, last_aifs = 0;
	sint8 DilQ = -1;

	/*
	 * The 1st element in pAcParam is always BE
	 * The 2nd is BK, 3rd is VI and 4th is VO.
	 */
	ac_param[1].aifs = pAcParam[0].AciAifn & 0x0f;	/* BE */
	ac_param[1].order = 0;
	ac_param[0].aifs = pAcParam[1].AciAifn & 0x0f;	/* BK */
	ac_param[0].order = 1;
	ac_param[2].aifs = pAcParam[2].AciAifn & 0x0f;	/* VI */
	ac_param[2].order = 2;
	ac_param[3].aifs = pAcParam[3].AciAifn & 0x0f;	/* VO */
	ac_param[3].order = 3;

	for (j = 0; j < 3; j++) {
		for (i = 0; i < 3 - j; i++) {
			if (ac_param[i].aifs < ac_param[i + 1].aifs) {
				struct ac_param temp;
				temp = ac_param[i];
				ac_param[i] = ac_param[i + 1];
				ac_param[i + 1] = temp;
			}
		}
	}

	for (i = 0; i < 4; i++) {
		/*if( ac_param[i].aifs != last_aifs ) */
		{
			DilQ++;
			last_aifs = ac_param[i].aifs;
		}
		pUmacInstance->AC2DilQ[ac_param[i].order] = DilQ;
	}
}

/******************************************************************************
	      EXTERNAL FUNCTIONS
******************************************************************************/

/******************************************************************************
 * NAME:	UMAC_Create
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for creating a new UMAC Instance.All the
 * resource allocation should happen here.
 * \param *ulHandle	-  Handle to UMAC Instance.
 * \param *pLlCb	- Function pointers to Lower layer.
 * \param *pMem		- Memory required for UMAC to operate.
 * \returns UMAC_HANDLE if success, otherwise NULL..
 *****************************************************************************/
UMAC_HANDLE UMAC_Create(void *ulHandle, UMAC_TO_LL_CALLS *pLlCb, void *pMem)
{

	WFM_UMAC_INSTANCE *pUmacInstance;
	uint8 Count = 0;

	OS_ASSERT(pLlCb);
	pUmacInstance = (WFM_UMAC_INSTANCE *) OS_Allocate(sizeof(WFM_UMAC_INSTANCE));

	if (NULL == pUmacInstance) {
		LOG_EVENT(DBG_WFM_ERROR, "Failed to allocate Memory for UMAC Instance");
		goto exit_handler;
		/* OS_ASSERT(pUmacInstance); */
	}			/*if( NULL == pUmacInstance ) */

	OS_MemoryReset((void *)pUmacInstance, sizeof(WFM_UMAC_INSTANCE));

	/* UMAC State should be UNINITIALIZED immediatly after creation */
	UMAC_SET_STATE(pUmacInstance, UMAC_UNINITIALIZED);

	pUmacInstance->gVars.p.ulHandle = ulHandle;

	/*Allocating Internal Tx and indication buffer pool */
	umac_mem_init(
			&pUmacInstance->gVars.p.TxBuffPool,
			&pUmacInstance->gVars.p.IndBuffPool
			);

	/*Allocating resources for timers */

	pUmacInstance->gVars.p.UmacTimers.pUMACTimer = OS_CREATE_TIMER(pUmacInstance->gVars.UMACTimer, UMAC_TimeOut_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pUMACTimer)
		goto release_exit;


	pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer = OS_CREATE_TIMER(pUmacInstance->gVars.RateAdapTimer, UMAC_RateAdapTimeOut_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer)
		goto release_exit;


	pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer = OS_CREATE_TIMER(pUmacInstance->gVars.CheckForConnectionTimer, UMAC_CheckForConnectionTimeOut_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer)
		goto release_exit;


	pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer = OS_CREATE_TIMER(pUmacInstance->gVars.UnJoinTimer, UMAC_CheckIfUnJoinCanProceedTimeOut_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer)
		goto release_exit;

	pUmacInstance->gVars.p.UmacTimers.pBssLostTimer = OS_CREATE_TIMER(NULL, UMAC_BssLostTimeout_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pBssLostTimer)
		goto release_exit;

#if 0
	pUmacInstance->gVars.p.UmacTimers.pCancelRemainOnChTimer = OS_CREATE_TIMER(NULL, UMAC_CancelRemainOnChTimeOut_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pCancelRemainOnChTimer)
		goto release_exit;
#endif

#if DOT11K_SUPPORT
	pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer = OS_CREATE_TIMER(pUmacInstance->gVars.MeasurementPauseTimer, UMAC_MeasPauseTimeOut_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer)
		goto release_exit;


	pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer = OS_CREATE_TIMER(pUmacInstance->gVars.TxStreamTimer, UMAC_TxStreamMeasTimeOut_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer)
		goto release_exit;

#endif				/*DOT11K_SUPPORT */

#if MGMT_FRAME_PROTECTION
	pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer = OS_CREATE_TIMER(pUmacInstance->gVars.SaQueryTimer, UMAC_SaQueryRetryTimeOut_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer)
		goto release_exit;


	pUmacInstance->gVars.p.UmacTimers.ReAssocTimer = OS_CREATE_TIMER(pUmacInstance->gVars.ReAssocTimer, UMAC_ReAssocTimeOutInterval_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.ReAssocTimer)
		goto release_exit;

#endif				/*MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
	for (Count = 0; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++) {
		pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[Count] = OS_CREATE_TIMER(pUmacInstance->gVars.ClientConnectionTimer, UMAC_ClientConnectionTimeout_Cb);

		if (NULL == pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[Count])
			goto release_exit;
	}


	pUmacInstance->gVars.p.UmacTimers.pUmacGroupFormationTimer = OS_CREATE_TIMER(pUmacInstance->gVars.GroupFormationTimer, UMAC_GroupFormationTimeout_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pUmacGroupFormationTimer)
		goto release_exit;


	pUmacInstance->gVars.p.UmacTimers.pUmacInvitationTimer = OS_CREATE_TIMER(pUmacInstance->gVars.GroupFormationTimer, UMAC_P2P_InvitationTimeout_Cb);

	if (NULL == pUmacInstance->gVars.p.UmacTimers.pUmacInvitationTimer)
		goto release_exit;

	for (Count = 0; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++) {
		pUmacInstance->gVars.p.UmacTimers.pUMACSTAUnJoinTimer[Count] = OS_CREATE_TIMER(pUmacInstance->gVars.pUMACSTAUnJoinTimer, UMAC_CheckIfUnJoinCanProceedTimeOut_Cb);

		if (NULL == pUmacInstance->gVars.p.UmacTimers.pUMACSTAUnJoinTimer[Count])
			goto release_exit;

	}	/*for(Count=0; Count<MAX_NUM_CLIENTS_SUPPORTED; Count++) */

	for (Count = 1; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++) {
		pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[Count] = OS_CREATE_TIMER(pUmacInstance->gVars.ConnDrop, UMAC_ConnectionDropTimeout_Cb);
		if (NULL == pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[Count])
			goto release_exit;

	}	/*for(Count=1; Count<MAX_NUM_CLIENTS_SUPPORTED; Count++) */

	pUmacInstance->gVars.p.UmacTimers.pUMACCleanupTimer = OS_CREATE_TIMER(NULL, UMAC_CleanupTimeout_Cb);
	if (NULL == pUmacInstance->gVars.p.UmacTimers.pUMACCleanupTimer)
		goto release_exit;


	pUmacInstance->gVars.p.ApPacketIdPoolLock = OS_CREATE_LOCK();
	if (NULL == pUmacInstance->gVars.p.ApPacketIdPoolLock)
		goto release_exit;


	for (Count = 0; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++) {
		pUmacInstance->gVars.p.ApPsTxBuffPoolLock[Count] = OS_CREATE_LOCK();
		if (NULL == pUmacInstance->gVars.p.ApPsTxBuffPoolLock[Count])
			goto release_exit;

	}

	pUmacInstance->gVars.p.StaDataBase = OS_CREATE_LOCK();
	if (NULL == pUmacInstance->gVars.p.StaDataBase)
		goto release_exit;

	/* Set the tie breaker bit to 0 or 1 randomly */
	pUmacInstance->gVars.p.tieBrkr = (uint8) (OS_GetTime() & 0x00000001);

#endif				/*P2P_MINIAP_SUPPORT */

	pUmacInstance->gVars.p.tcb.Lock = OS_CREATE_LOCK();

	if (NULL == pUmacInstance->gVars.p.tcb.Lock)
		goto release_exit;


	pUmacInstance->gVars.p.tcb.PendingQLock = OS_CREATE_LOCK();

	if (NULL == pUmacInstance->gVars.p.tcb.PendingQLock)
		goto release_exit;


	pUmacInstance->gVars.p.TxDescStoreLock = OS_CREATE_LOCK();

	if (NULL == pUmacInstance->gVars.p.TxDescStoreLock)
		goto release_exit;


	pUmacInstance->gVars.p.PacketIdPoolLock = OS_CREATE_LOCK();
	if (NULL == pUmacInstance->gVars.p.PacketIdPoolLock)
		goto release_exit;


	pUmacInstance->gVars.p.TxBufCountLock = OS_CREATE_LOCK();

	if (NULL == pUmacInstance->gVars.p.TxBufCountLock)
		goto release_exit;


	pUmacInstance->gVars.p.RxBufCountLock = OS_CREATE_LOCK();

	if (NULL == pUmacInstance->gVars.p.RxBufCountLock)
		goto release_exit;

	pUmacInstance->gVars.p.TxBuffRequeuePoolLock = OS_CREATE_LOCK();

	if (NULL == pUmacInstance->gVars.p.TxBuffRequeuePoolLock)
		goto release_exit;

	pUmacInstance->gVars.p.UpdateTimIeLock = OS_CREATE_LOCK();

	if (NULL == pUmacInstance->gVars.p.UpdateTimIeLock)
		goto release_exit;

	pUmacInstance->gVars.p.UpdateERPIeLock = OS_CREATE_LOCK();

	if (NULL == pUmacInstance->gVars.p.UpdateERPIeLock)
		goto release_exit;

	if (pLlCb) {
		/*Copy the function pointer information to our own memory */
		OS_MemoryCopy(
				&pUmacInstance->gVars.p.LlInfo.LL_FnCalls,
				pLlCb,
				sizeof(UMAC_TO_LL_CALLS)
				);
	}			/*if(pLlCb) */


	/*Initializing the lower Layer */

	UMAC_LL_INIT((UMAC_HANDLE) pUmacInstance);

	return (UMAC_HANDLE) pUmacInstance;

release_exit:
	UMAC_Distroy(pUmacInstance);

exit_handler:
	return NULL;

} /* end UMAC_Create() */

/******************************************************************************
 * NAME:	UMAC_Init
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for starting/initializing a UMAC Instance.
 * UmacHandle should be created with UMAC_Create() before calling this
 * function.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \param *llHandle       - Handle to Lower layer.
 * \param UlCb		  - Callback function pointer
 *			    for upper layer to recieve data
 * \param UpperLayerRxfrm - Callback function for Rx 802.3 frame.
 * \param UpperLayerTxcnf - Callback function for Tx confirmation.
 * \param ScheduleCb      - Callback to request the upper layer for context.
 * \param *ScheduleHndle  - The handle needs to be passed to the
 *			    ScheduleCb inorder to request a context.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
WFM_STATUS_CODE UMAC_Init(UMAC_HANDLE UmacHandle,
			  void *llHandle,
			  UMAC_2UL_CB UlCb,
			  UMAC_2UL_RX_FRM_CB UpperLayerRxfrm,
#if DOT11K_SUPPORT
			  UMAC_2UL_BEACON_INFO_CB UpperLayerBeaconInfoCb,
#endif	/* DOT11K_SUPPORT */
			  UMAC_2UL_TX_CNF_CB UpperLayerTxcnf,
			  UMAC_2UL_TX_MGMT_CNF_CB UpperLayerTxMgmtcnf,
			  UMAC_2UL_SCHEDULE_CB ScheduleCb,
			  void *ScheduleHndle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 Status = WFM_STATUS_SUCCESS;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(UlCb);
	OS_ASSERT(ScheduleCb);
	OS_ASSERT(ScheduleHndle);
	OS_ASSERT(UpperLayerTxcnf);

	pUmacInstance->UmacCurrentState = UMAC_UNINITIALIZED;

	pUmacInstance->dot11InfraStructureMode = WFM_802_AUTO_UNKNOWN;

	pUmacInstance->gVars.p.UpperLayerCb = UlCb;

	pUmacInstance->gVars.p.ScheduleCb = ScheduleCb;

	pUmacInstance->gVars.p.ScheduleParam = ScheduleHndle;

	pUmacInstance->gVars.p.LlInfo.LowerLayerHandle = llHandle;

	pUmacInstance->gVars.p.UpperLayerRxFrmCb = UpperLayerRxfrm;

#if DOT11K_SUPPORT
	pUmacInstance->gVars.p.UpperLayerBeaconInfoCb = UpperLayerBeaconInfoCb;
#endif	/* DOT11K_SUPPORT */

	pUmacInstance->gVars.p.UpperLayerTxcnf = UpperLayerTxcnf;

	pUmacInstance->gVars.p.UpperLayerTxMgmtcnf = UpperLayerTxMgmtcnf;

	/*Hardcoding to 1(Disabling BG scan) */
	pUmacInstance->gVars.p.InfoHost.disableBGScan = 1;
	return (WFM_STATUS_CODE) Status;
} /* end UMAC_Init() */

/******************************************************************************
 * NAME:	UMAC_Start
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for starting the UMAC instance which is already
 * created. UmacHandle should be created with UMAC_Create() before calling this
 * fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
WFM_STATUS_CODE UMAC_Start(UMAC_HANDLE UmacHandle)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	uint8 i = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_REGULATORY_DOMAIN_TABLE *pRegDT = &pUmacInstance->gVars.p.RegDomainTable;
	pUmacInstance->gVars.p.ForceStart = UMAC_FORCE_START_ON;

	WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_INIT, NULL);

	/* set  default values to some wfm parameters */
	WFM_UMAC_Initialize_TxQueueParams_All(pUmacInstance);

	/* Initialize encryption */
	WFM_UMAC_Initialize_Encryption(pUmacInstance);

	/* Initialize PS Mode to a default value of PS enabled */
	pUmacInstance->gVars.p.PsMode.PmMode = WFM_PS_MODE_DISABLED;
	pUmacInstance->gVars.p.PsMode.FastPsmIdlePeriod = 0;
	pUmacInstance->gVars.p.PsMode.ApPsmChangePeriod = 0;
	pUmacInstance->gVars.p.OprPwrMode = WFM_OPR_PWR_MODE_ACTIVE;
	/*
	 * By default max rx AMSDU size is 8k
	 */
	pUmacInstance->gVars.p.maxRxAmsduSize = 1;

	pUmacInstance->gVars.p.txopLimit = 0;
	pUmacInstance->gVars.p.UseMultiTxCnfMsg = 0;
	pUmacInstance->gVars.p.DisableActiveJoin = 0;
	pUmacInstance->isArpFilterEnabled = 0;

	/*Initialize the Regulatory domain table with default */
	OS_MemoryCopy(&pRegDT->countryRegDetails.countryString[0],
		(uint8 *) &DefaultRegulatoryDomainTable.countryRegDetails.countryString[0],
		WFM_DOT11_COUNTRY_STRING_SIZE);
	pRegDT->countryRegDetails.noOfRegClass = DefaultRegulatoryDomainTable.countryRegDetails.noOfRegClass;
	for (i = 0 ; i < pRegDT->countryRegDetails.noOfRegClass ; i++) {
		pRegDT->countryRegDetails.regClassMap[i].txPowerLevel =
			DefaultRegulatoryDomainTable.countryRegDetails.regClassMap[i].txPowerLevel;
		pRegDT->countryRegDetails.regClassMap[i].channelOffset =
			DefaultRegulatoryDomainTable.countryRegDetails.regClassMap[i].channelOffset;
		pRegDT->countryRegDetails.regClassMap[i].RegClass =
			DefaultRegulatoryDomainTable.countryRegDetails.regClassMap[i].RegClass;
		pRegDT->countryRegDetails.regClassMap[i].noOfTriplets =
			DefaultRegulatoryDomainTable.countryRegDetails.regClassMap[i].noOfTriplets;
		OS_MemoryCopy((uint8 *) &pRegDT->countryRegDetails.regClassMap[i].channelTriplet[0],
	      (uint8 *) &DefaultRegulatoryDomainTable.countryRegDetails.regClassMap[i].channelTriplet[0],
	      pRegDT->countryRegDetails.regClassMap[i].noOfTriplets * sizeof(UMAC_CHANNEL_TRIPLET));
	}

	pUmacInstance->gVars.p.keepAlivePeriod.keepAlivePeriod = UMAC_DEFAULT_KEEP_ALIVE_PERIOD;

	return (WFM_STATUS_CODE) Status;
} /* end UMAC_Start() */

/******************************************************************************
 * NAME:	UMAC_Stop
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for Stopping a UMAC Instance.UmacHandle should
 * be created with UMAC_Create() and started using UMAC_Start()before calling
 * this  function. UmacHandle should be created with UMAC_Create() before
 * calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
WFM_STATUS_CODE UMAC_Stop(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 Status = WFM_STATUS_SUCCESS;
	uint16 UmacEvent;
	uint16 StateVar;

	OS_ASSERT(UmacHandle);

	pUmacInstance->gVars.p.ForceStopMode = UMAC_FORCE_STOP_ON;

	if (pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
		StateVar = pUmacInstance->UmacPreviousState;
	else
		StateVar = pUmacInstance->UmacCurrentState;

	if (StateVar > UMAC_AUTHENTICATED)
		UmacEvent = UMAC_DE_ASSOC;
	else if (StateVar > UMAC_JOINED)
		UmacEvent = UMAC_DE_AUTHE;
#if P2P_MINIAP_SUPPORT
	else if (StateVar > UMAC_INITIALIZED) {
		uint8 i;
		if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
			for (i = 1; i < pUmacInstance->gVars.p.maxNumClientSupport; i++) {
				if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[i]))
					LOG_DATA(DBG_WFM_ERROR, "Failed to Cancel pUMACInactivityTimer Timer, linkId = %d\n", i);
			}
			pUmacInstance->gVars.p.stopAPFlag = TRUE;
			/*pUmacInstance->dot11BssConf.numSTAsConnected = 0;*/
			UmacEvent = UMAC_DEAUTH_STA;
		} else {
			UmacEvent = UMAC_UNJOIN;
		}
	}			/*else if( StateVar > UMAC_INITIALIZED ) */

#else	/* P2P_MINIAP_SUPPORT */

	else if (StateVar > UMAC_INITIALIZED)
		UmacEvent = UMAC_UNJOIN;

#endif				/*P2P_MINIAP_SUPPORT */

	else {
		UmacEvent = UMAC_DEINIT;
		pUmacInstance->gVars.p.ForceStopMode = UMAC_FORCE_STOP_RESET;
	}

	/* reset the country info */
	pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[0] = FALSE;
	pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[1] = FALSE;
	pUmacInstance->gVars.p.InfoHost.countryStringAvl = FALSE;
	OS_MemoryReset(
		&pUmacInstance->gVars.p.InfoHost.CountryStringInfo[0],
		(2 * sizeof(WFM_COUNTRY_STRING_INFO))
		);
	if (pUmacInstance->gVars.p.Enable11dFeature)
		pUmacInstance->gVars.p.Enable11dFeature = 0;

	WFM_SCHEDULE_TO_SELF(UmacHandle, UmacEvent, NULL);

	/*Force clearing the black list */
	UMAC_ClearBlackList(UmacHandle);

	return (WFM_STATUS_CODE) Status;
} /* end UMAC_Stop() */

/******************************************************************************
 * NAME:	UMAC_Distroy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for destroying a UMAC Instance.UmacHandle
 * should be created with UMAC_Create() before calling this  fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
WFM_STATUS_CODE UMAC_Distroy(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 Count = 0;

	UMAC_LL_DEINIT(UmacHandle);

	UMAC_CleanAllTimers(UmacHandle);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.tcb.Lock);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.tcb.PendingQLock);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.TxDescStoreLock);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.PacketIdPoolLock);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	OS_DESTROY_LOCK(pUmacInstance->gVars.p.RxBufCountLock);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.ApPacketIdPoolLock);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.TxBuffRequeuePoolLock);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.UpdateERPIeLock);

	for (Count = 0; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++)
		OS_DESTROY_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[Count]);

	OS_DESTROY_LOCK(pUmacInstance->gVars.p.StaDataBase);

	/*Releasing Internal Tx and indication buffer pool */
	umac_mem_deInit(
			&pUmacInstance->gVars.p.TxBuffPool,
			&pUmacInstance->gVars.p.IndBuffPool
			);

	OS_Free((void *)UmacHandle);

	return WFM_STATUS_SUCCESS;

} /* end UMAC_Distroy() */
/******************************************************************************
 * NAME:	UMAC_CleanAllTimers
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for clearing all the timers.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns void
 *****************************************************************************/
void UMAC_CleanAllTimers(UMAC_HANDLE UmacHandle)
{
#if P2P_MINIAP_SUPPORT
	uint8 Count;
#endif	/* P2P_MINIAP_SUPPORT */
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	/*Stop all the timers*/
	if (pUmacInstance->gVars.p.UmacTimers.pUMACTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer);
	}
	if (pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pRateAdapTimer);
	}
	if (pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pCheckForConnectionTimer);
	}
	if (pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pUnJoinTimer);
	}
	if (pUmacInstance->gVars.p.UmacTimers.pBssLostTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pBssLostTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pBssLostTimer);
	}
	if (pUmacInstance->gVars.p.UmacTimers.pCancelRemainOnChTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pCancelRemainOnChTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pCancelRemainOnChTimer);
	}
#if DOT11K_SUPPORT
	if (pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer);
	}
	if (pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer);
	}
#endif
#if MGMT_FRAME_PROTECTION
	if (pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pSaQueryRetryTimer);
	}
	if (pUmacInstance->gVars.p.UmacTimers.ReAssocTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.ReAssocTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.ReAssocTimer);
	}
#endif	/* MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
	for (Count = 0; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++) {
		if (pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[Count]) {
			OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[Count]);
			OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[Count]);
		}
	}
	if (pUmacInstance->gVars.p.UmacTimers.pUmacGroupFormationTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacGroupFormationTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacGroupFormationTimer);
	}
	if (pUmacInstance->gVars.p.UmacTimers.pUmacInvitationTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacInvitationTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacInvitationTimer);
	}

	for (Count = 0; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++) {
		if (pUmacInstance->gVars.p.UmacTimers.pUMACSTAUnJoinTimer[Count]) {
			OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACSTAUnJoinTimer[Count]);
			OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACSTAUnJoinTimer[Count]);
		}
	}
	for (Count = 1; Count < MAX_NUM_CLIENTS_SUPPORTED; Count++) {
		if (pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[Count]) {
			OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[Count]);
			OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[Count]);
		}
	}
	if (pUmacInstance->gVars.p.UmacTimers.pUMACCleanupTimer) {
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACCleanupTimer);
		OS_DESTROY_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACCleanupTimer);
	}

#endif	/* P2P_MINIAP_SUPPORT */
}
/******************************************************************************
 * NAME:	UMAC_ProcRadioMeasurement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the radio measurement request coming from the lower
 * layer. It parses all the measurement request elements,stores it in
 * pUmacInstance and initiates the request to firmware.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcRadioMeasurement(UMAC_HANDLE UmacHandle, void *pMsg)
{
#if DOT11K_SUPPORT
	uint16 UmacEvent = UMAC_SEND_MEASUREMENT_REQ;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	uint8 *pRadioMeasurementFrame = NULL;
	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
	pRadioMeasurementFrame = (uint8 *) pUmacMgmt->FrameBody;

	if (!pUmacInstance->dot11k_Meas_Params.RadioMeasReqUnderProcess) {
		/*Copy Address3 as it is used while sending report */
		OS_MemoryCopy(&pUmacInstance->dot11k_Meas_Params.Addr3, pUmacMgmt->dot11hdr.Address3, 6);
		pUmacInstance->dot11k_Meas_Params.RadioMeasReqUnderProcess = 1;
		/*Update dialog token and no of rep. */
		pUmacInstance->dot11k_Meas_Params.dialogToken = *(pRadioMeasurementFrame + 2);
		/*pUmacInstance->dot11k_Meas_Params.noOfRep = pRadioMeasurementFrame->noOfRep; */
		pUmacInstance->dot11k_Meas_Params.noOfRep = (uint16) *((uint8 *)pRadioMeasurementFrame + 3);
		/*Process the frame to get all the measurement request elements */
		pUmacInstance->dot11k_Meas_Params.MeasurementReqElementsLen = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor) - (sizeof(WFM_UMAC_FRAME_HDR) + 5));	/* 5 is added for sizeof category+Action+DialogToken+NoOfRep */
		UMAC_ProcMeasurementRequestElements(
			UmacHandle,
						    /*(uint8*)pRadioMeasurementFrame->measurementReqElement, */
			(uint8 *) pRadioMeasurementFrame + 5,
			pUmacInstance->dot11k_Meas_Params.MeasurementReqElementsLen
			);

		WFM_UMAC_DBG_REMOVE_RX_DESC(pMsg);

		OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
		pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
		OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

		/*Release the buffer received from lower layer */
		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);

		/*
		  Now all the measurement request elements are parsed so start
		  processing the first request
		*/
		if (pUmacInstance->dot11k_Meas_Params.totalRequests) {
			/*
			  Here schedule the ProcSendMeasurementReq function.
			  We are scheduling this function because when we will
			  get pending status it will be put in pending queue.
			*/
			WFM_SCHEDULE_TO_SELF(UmacHandle, UmacEvent, NULL);
		}
	} else {
		WFM_UMAC_DBG_REMOVE_RX_DESC(pMsg);

		OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
		pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
		OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

		/*Release the buffer received from lower layer */
		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
	}
#endif	/* DOT11K_SUPPORT */
	/*
	  Always send the SUCCESS result so that it does not get queued in
	  pending queue.
	*/
	return WFM_STATUS_SUCCESS;

} /* end UMAC_ProcRadioMeasurement() */

/******************************************************************************
* NAME:	 UMAC_ProcSendMsrmtReq
******************************************************************************/
/**
* \brief
* This function will check the measurement request type and call the
* corresponding function to send the request to firmware.
* \param UmacHandle - Handle to the UMAC Instance
* \param pMsg - Pointer to Msg.
* \return WFM_STATUS_CODE
******************************************************************************/
static uint32 UMAC_ProcSendMsrmtReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
#if DOT11K_SUPPORT
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	uint8 measurementType = 0;
	uint16 umacEvent = UMAC_MAX_EVT;
	UMAC_SET_STATE_PROCESSING(UmacHandle);
	/*Check if there are request pending */
	if (!pUmacInstance->dot11k_Meas_Params.totalRequests) {
		Result = WFM_STATUS_FAILURE;
		goto exit_handler;
	}
	pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;
	if ((!pUmacInstance->dot11k_Meas_Params.reqUnderProcess) && (!pSmgmtReq->HandleMgmtReq)) {
		/*
		  This variable will prevent the code of this function to
		  execute again when it is called by pending queue
		*/
		pUmacInstance->dot11k_Meas_Params.reqUnderProcess = 1;

		/*Check if we are capable for the current request */
		measurementType = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementType;
		if (!(pUmacInstance->gVars.p.measurementCapability & (1 << measurementType))) {
			pUmacInstance->dot11k_Meas_Params.CurrentReportOp = INCAPABLE;
			umacEvent = UMAC_MEASUREMENT_REPORT;
			UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
			WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
			goto exit_handler;
		}
		/*Check if current request is enabled */
		if (!(pUmacInstance->gVars.p.enableMeasurements & (1 << measurementType))) {
			pUmacInstance->dot11k_Meas_Params.CurrentReportOp = REFUSED;
			umacEvent = UMAC_MEASUREMENT_REPORT;
			UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
			WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
			goto exit_handler;
		}
		/*
		  Check if parallel bit is 1 for the current request.If it is 1
		  then send report with refused bit as we are not supporting
		  Parallel operation (Section 11.10.5)
		*/
		if (pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementRequestMode & 0x01) {
			pUmacInstance->dot11k_Meas_Params.CurrentReportOp = INCAPABLE;
			umacEvent = UMAC_MEASUREMENT_REPORT;
			UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
			WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
			goto exit_handler;
		}
		/*
		  Check if correct regulatory class and channel number is
		  provided,else refuse the measurement
		*/
		if ((measurementType == UMAC_MEASUREMENT_REQ_TYPE_CHANNEL_LOAD) ||
		    (measurementType == UMAC_MEASUREMENT_REQ_TYPE_NOISE_HISTOGRAM) ||
		    ((measurementType == UMAC_MEASUREMENT_REQ_TYPE_BEACON) && (pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq[6] != UMAC_MEAS_MODE_BEACON_TABLE))
		    ) {
			uint8 RegClass;
			uint8 channelNum;
			uint16 channelOffset;
			uint32 txPowerLevel;
			uint8 nResult = FALSE;

			RegClass = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq[0];
			channelNum = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq[1];
			nResult = UMAC_CheckRegClassAndChannelNum(UmacHandle, RegClass, channelNum, &channelOffset, &txPowerLevel);
			if (!nResult) {
				/*Channel not found.So refuse the measurement */
				pUmacInstance->dot11k_Meas_Params.CurrentReportOp = REFUSED;
				umacEvent = UMAC_MEASUREMENT_REPORT;
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
				goto exit_handler;
			} else {

				pUmacInstance->dot11k_Meas_Params.txPowerLevel = txPowerLevel;
				pUmacInstance->dot11k_Meas_Params.channelOffset = channelOffset;
			}
		}

		/*
		  Check if the request is for LCI.Send the report with
		  incapable bit as 1
		*/
		if (measurementType == UMAC_MEASUREMENT_REQ_TYPE_LCI) {
			if (pUmacInstance->gVars.p.lciInfoAvl)
				pUmacInstance->dot11k_Meas_Params.CurrentReportOp = NORMAL;
			else
				pUmacInstance->dot11k_Meas_Params.CurrentReportOp = INCAPABLE;

			umacEvent = UMAC_MEASUREMENT_REPORT;
			UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
			WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
			goto exit_handler;
		}
		/*
		  Check if the current request is beacon and type is beacon
		  table
		*/
		if (measurementType == UMAC_MEASUREMENT_REQ_TYPE_BEACON) {
			if (pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq[6] == UMAC_MEAS_MODE_BEACON_TABLE) {
				pUmacInstance->dot11k_Meas_Params.CurrentReportOp = BEACON_TABLE;
				umacEvent = UMAC_MEASUREMENT_REPORT;
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
				goto exit_handler;
			}
		}
		/*Check if the current request if for Transmit Stream */
		if (measurementType == UMAC_MEASUREMENT_REQ_TYPE_TRANSMIT_STREAM) {
			/*
			  Check if the measurement request is for triggered
			  then send report with Incapable bit else start timer
			  for measurement
			*/
			if ((pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementRequestMode & 0x0A) == 0x0A) {
				pUmacInstance->dot11k_Meas_Params.CurrentReportOp = INCAPABLE;
				umacEvent = UMAC_MEASUREMENT_REPORT;
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
				goto exit_handler;
			} else {
					if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION)
					|| (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION)) {
						OS_MemoryReset(&pUmacInstance->dot11k_Meas_Params.txStreamResults, sizeof(TANSMIT_STREAM_RESULTS));
						pUmacInstance->dot11k_Meas_Params.CurrentReportOp = NORMAL;
					} else {
						pUmacInstance->dot11k_Meas_Params.CurrentReportOp = INCAPABLE;
						umacEvent = UMAC_MEASUREMENT_REPORT;
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
						goto exit_handler;
					}

			}
			/*Set the request to 1 and start the timer */
#if 0
			   pUmacInstance->dot11k_Meas_Params.transmitStreamUnderProcess = 1;
			   OS_START_TIMER
			   (
			   pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer,
			   ((pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq.transmitStreamReq.measurementDuration * 1024)/1000),/*meas Duration is in TU */
			   UmacHandle
			   );
			   UMAC_RESTORE_PREVIOUS_STATE(UmacHandle) ;
			   goto exit_handler;
			   }
#endif

		}
		/*
		  All UMAC Req/Resp are handled above.Now the firmware
		  supported Req/Resp are handled below
		*/
		if (!pSmgmtReq->HandleMgmtReq) {
			pUmacInstance->dot11k_Meas_Params.CurrentReportOp = NORMAL;
			OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
			pSmgmtReq->HandleMgmtReq = 1;

			pSmgmtReq->Type = SMGMT_REQ_TYPE_MEASUREMENT_REQ;

			pSmgmtReq->operation.measurement_req.measurementReqType = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementType;
			pSmgmtReq->ReqNum = 0;

			Result = UMAC_ManageAsynMgmtOperations(UmacHandle);
			if (Result != WFM_STATUS_SUCCESS)
				goto exit_handler;


		} /* if(!pSmgmtReq->HandleMgmtReq) */
	}
	pSmgmtReq->HandleMgmtReq = 0;
	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);

 exit_handler:
#endif	/* DOT11K_SUPPORT */
	return Result;
} /* end UMAC_ProcSendMsrmtReq() */

/******************************************************************************
* NAME:	 UMAC_ProcSendLinkMsrmtReq
******************************************************************************/
/**
* \brief
* This function will check the measurement request type and call the
* corresponding function to send the request to firmware.
* \param UmacHandle - Handle to the UMAC Instance
* \param pMsg - Pointer to Msg.
* \return WFM_STATUS_CODE
******************************************************************************/
static uint32 UMAC_ProcSendLinkMsrmtReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
#if DOT11K_SUPPORT
	uint16 umacEvent = UMAC_MAX_EVT;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	uint16 measurementType = 1;	/*Link Measurement */

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;
	if ((!pUmacInstance->dot11k_Meas_Params.reqUnderProcess) && (!pSmgmtReq->HandleMgmtReq)) {
		pUmacInstance->dot11k_Meas_Params.reqUnderProcess = 1;
		/*Check the incapable bit */
		if (!(pUmacInstance->gVars.p.measurementCapability & (1 << measurementType))) {
			pUmacInstance->dot11k_Meas_Params.CurrentReportOp = INCAPABLE;
			umacEvent = UMAC_MEASUREMENT_REPORT;
			UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
			WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
			goto exit_handler;
		}
		/*Check the enable bit */
		if (!(pUmacInstance->gVars.p.enableMeasurements & (1 << measurementType))) {
			pUmacInstance->dot11k_Meas_Params.CurrentReportOp = REFUSED;
			umacEvent = UMAC_MEASUREMENT_REPORT;
			UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
			WFM_SCHEDULE_TO_SELF(UmacHandle, umacEvent, NULL);
			goto exit_handler;
		}
		/*Send the request to the firmware */
		if (!pSmgmtReq->HandleMgmtReq) {
			pUmacInstance->dot11k_Meas_Params.CurrentReportOp = NORMAL;
			OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
			pSmgmtReq->HandleMgmtReq = 1;

			pSmgmtReq->Type = SMGMT_REQ_TYPE_MEASUREMENT_REQ;

			pSmgmtReq->operation.measurement_req.measurementReqType = UMAC_MEASUREMENT_REQ_TYPE_LINK_MEAS;

			pSmgmtReq->ReqNum = 0;

			Result = UMAC_ManageAsynMgmtOperations(UmacHandle);
			if (Result != WFM_STATUS_SUCCESS)
				goto exit_handler;
		} /* if(!pSmgmtReq->HandleMgmtReq) */
	}
	pSmgmtReq->HandleMgmtReq = 0;
	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
 exit_handler:
#endif	/* DOT11K_SUPPORT */
	return Result;
} /* end UMAC_ProcSendLinkMsrmtReq() */

/******************************************************************************
 * NAME:	UMAC_ProcNeighborReportReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends Neighbor report request to AP
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcNeighborReportReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	uint8 Encrypt;
#if DOT11K_SUPPORT

	WFM_UMAC_MGMT_FRAME *pMgmtActionFrame = NULL;
	WFM_802_11_SSID *pNeighborRepSsid = NULL;
	NEIGHBOR_REPORT_REQ *pNeighborReportReq = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq = NULL;
	uint8 *pos = NULL;
	uint16 FrameBodyLen = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	pNeighborRepSsid = (WFM_802_11_SSID *) pMsg;

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);
	OS_ASSERT(pDevIfTxReq);
	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
	WFM_UMAC_DBG_PRINT_TX_DESC();

	pMgmtActionFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pMgmtActionFrame);
	/*
	   Creating the WLAN Header
	 */
	Encrypt = UMAC_ProtectionRequired(pUmacInstance, 0, WFM_UMAC_MGMT, WFM_UMAC_MGMT_ACTION);
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_ACTION,
					&pMgmtActionFrame->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					Encrypt
					);
	pNeighborReportReq = (NEIGHBOR_REPORT_REQ *) pMgmtActionFrame->FrameBody;
	pNeighborReportReq->category = UMAC_CATEGORY_RADIO_MEASUREMENT;
	pNeighborReportReq->action = UMAC_ACTION_TYPE_NEIGHBOR_REPORT_REQ;
	pNeighborReportReq->dialogToken = 1;
	/*TBC*/
	pos = (uint8 *) pMgmtActionFrame->FrameBody + 3;
	/*Subelement ID for SSID */
	*pos = 0;
	pos++;
	/*Subelement ID */
	*pos = (uint8) pNeighborRepSsid->SsidLength;
	pos++;
	/*SSID*/
	OS_MemoryCopy(
			pos,
			pNeighborRepSsid->Ssid,
			pNeighborRepSsid->SsidLength
			);
	pos += pNeighborRepSsid->SsidLength;

	FrameBodyLen = pos - pMgmtActionFrame->FrameBody;
	/*Fill pTxDesc->MsgLen */
	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));
	/*Get Packet ID */
	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcNeighborReportReq() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	/*Fill the pTxDesc parameters */
	pTxDesc->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;
	pTxDesc->QueueId = 0;	/*/WFM_AC_VO ; */
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_MGT);
	pTxDesc->Reserved = 0;

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL
					);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*Sending the management frame to LMAC for transmission */
	Result = UMAC_LL_REQ_TX_FRAME(UmacHandle, pTxDesc->QueueId, pDevIfTxReq, DEFAULT_LINK_ID);
	if (Result != WFM_STATUS_SUCCESS)
		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);

exit_handler:
#endif	/* DOT11K_SUPPORT */
	return Result;
}

/******************************************************************************
 * NAME:	UMAC_ProcNeighborRepResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends Neighbor report response to driver
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcNeighborRepResp(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
#if DOT11K_SUPPORT
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	uint8 *pRadioMeasurementFrame = NULL;
	uint16 EventDataLen = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
	pRadioMeasurementFrame = (uint8 *) pUmacMgmt->FrameBody;
	/*Process the frame to get all the measurement request elements */
	EventDataLen = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor) - (sizeof(WFM_UMAC_FRAME_HDR)));
	WFM_UMAC_2_UL_SendEvent(
				UmacHandle,
				WFM_EVT_NEIGHBOR_REPORT,
				EventDataLen,
				(uint8 *) pRadioMeasurementFrame,
				WFM_UMAC_EVT_NOT_RETRANSMIT
				);
	/*Release the memory */
	WFM_UMAC_DBG_REMOVE_RX_DESC(pMsg);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/*Release the buffer received from lower layer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
#endif	/* DOT11K_SUPPORT */
	return Result;
}

/******************************************************************************
 * NAME:	UMAC_ProcMsrmtReport
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends report to AP for different measurement requests.
 * It checks the measurement report for which measurement complete Indication
 * is received and sends report to firmware,
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg	 - Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcMsrmtReport(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
#if DOT11K_SUPPORT
	WSM_HI_MEASURE_CMPL_IND *pMeasurementComplInd = NULL;
	uint16 UmacEvent = WFM_EVT_MAX;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 measReqMode = 0, reqProcessedOnce = 0;
	uint8 measurementType = 0;
	uint8 nextReqFound = FALSE;
	uint8 currentArrayIndex = pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex;
	pMeasurementComplInd = (WSM_HI_MEASURE_CMPL_IND *) &pUmacInstance->dot11k_Meas_Params.measurementComplInd;
	/*Check for the measurement type and send the report */
	measurementType = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementType;
	switch (measurementType) {
	case UMAC_MEASUREMENT_REPORT_TYPE_CHANNEL_LOAD:
		{
			UMAC_SendChannelReport(UmacHandle);
			break;
		}
	case UMAC_MEASUREMENT_REPORT_TYPE_NOISE_HISTOGRAM:
		{
			UMAC_SendNoiseHistReport(UmacHandle);
			break;
		}
	case UMAC_MEASUREMENT_REPORT_TYPE_BEACON:
		{
			UMAC_SendBeaconReport(UmacHandle);
			break;
		}
	case UMAC_MEASUREMENT_REPORT_TYPE_STA_STATISTICS:
		{
			UMAC_SendStatisticsReport(UmacHandle);
			break;
		}
	case UMAC_MEASUREMENT_REPORT_TYPE_LCI:
		{
			UMAC_SendLCIReport(UmacHandle);
			break;
		}
	case UMAC_MEASUREMENT_REPORT_TYPE_FRAME:
		{
			break;
		}
	case UMAC_MEASUREMENT_REPORT_TYPE_TRANSMIT_STREAM:
		{
			UMAC_SendTxStreamReport(UmacHandle);
			break;
		}
	}
	/*Check if the current request is Beacon and option is beacon table */
	if ((measurementType == UMAC_MEASUREMENT_REQ_TYPE_BEACON) && (pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[currentArrayIndex].measurementElementReq[6] == UMAC_MEAS_MODE_BEACON_TABLE)) {
		if (pUmacInstance->dot11k_Meas_Params.bssidCount) {
			/*Schedule this function again to get the next beacon */
			UmacEvent = UMAC_MEASUREMENT_REPORT;
			WFM_SCHEDULE_TO_SELF(UmacHandle, UmacEvent, NULL);
			goto exit_handler;
		} else {
			/*All the bssid are retrieved.So reset bssidIndex */
			pUmacInstance->dot11k_Meas_Params.bssidIndex = 0;
		}
	}
	/*
	  Check the MoreInd flag.If it is 1 then more measurement report will
	  come from firmware. So don't update for next request
	*/
	/*if (!pUmacInstance->dot11k_Meas_Params.measurementComplInd.MoreInd) {*/
	/*
	   Fill the reqProcessed variable if the current request has
	   enable bit 1,Incapable or refused so that it is not
	   processed again
	*/
	measReqMode = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementRequestMode;
	if ((measReqMode & UMAC_ENABLE_MASK) || (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == INCAPABLE) || (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == REFUSED)
	    ) {
		pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].reqProcessedOnce = 1;
	}
	/*Decrement the Total Request */
	pUmacInstance->dot11k_Meas_Params.totalRequests--;
	/*Increment the CurrentArrayIndex Counter */
	/* 1 R/R is done.Now increment the current index and call the send request */
	if (pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex == (pUmacInstance->dot11k_Meas_Params.totalArrayIndex - 1))
		pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex = 0;
	else
		pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex++;

	/*
	   Check if the current Req has reqProcessedOnce set.If this
	   is the case then update the CurrentArrayIndex
	*/
	if (pUmacInstance->dot11k_Meas_Params.totalRequests) {
		while (!nextReqFound) {
			/*  measReqMode = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementRequestMode; */
			reqProcessedOnce = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].reqProcessedOnce;
			if ((reqProcessedOnce)) {
				/*Update current arrayIndex */
				if (pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex == (pUmacInstance->dot11k_Meas_Params.totalArrayIndex - 1))
					pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex = 0;
				else
					pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex++;

			} else {
				nextReqFound = TRUE;
				/*Check for the measurement pause Index */
				if ((pUmacInstance->dot11k_Meas_Params.MeasPauseReq) && (pUmacInstance->dot11k_Meas_Params.MeasPauseIndex == pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex))
					pUmacInstance->dot11k_Meas_Params.MeasPauseAvl = TRUE;

				break;
			}
		}
	}
	if ((!nextReqFound) && (pUmacInstance->dot11k_Meas_Params.totalRequests))
		goto exit_handler;

	/*Reset pUmacInstance->dot11k_Meas_Params.reqUnderProcess as we can handle next request now */
	if (pUmacInstance->dot11k_Meas_Params.reqUnderProcess)
		pUmacInstance->dot11k_Meas_Params.reqUnderProcess = 0;

	/*If some Request remains then schedule the send request */
	if (pUmacInstance->dot11k_Meas_Params.totalRequests) {
		/*
		   Here check the measurement Pause Request.If it is
		   present then start the timer. If not present then
		   schedule send measurement req from here
		*/
		if (pUmacInstance->dot11k_Meas_Params.MeasPauseAvl) {
			/*Start Timer and exit from here */
			OS_START_TIMER(
				pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer,
				((pUmacInstance->dot11k_Meas_Params.MeasPauseTime * 10 * 1024) / 1000),	/* 1TU = 1024 us */
				UmacHandle
				);
			goto exit_handler;
		} else {
			UmacEvent = UMAC_SEND_MEASUREMENT_REQ;
			WFM_SCHEDULE_TO_SELF(UmacHandle, UmacEvent, NULL);
		}
	} else {
		/*
		  Reset RadioMeasReqUnderProcess so that next radio
		  measurement frame can be accepted
		*/
		pUmacInstance->dot11k_Meas_Params.RadioMeasReqUnderProcess = 0;
		/*Stop Timers */
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pMeasPauseTimer);
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer);
		/*reset all the variables */
		OS_MemoryReset(&pUmacInstance->dot11k_Meas_Params, sizeof(DOT11K_MEAS_PARAMS));
	}
	/*}  End of more Ind */
	/*Reset Measurement Complete Indication. */
	OS_MemoryReset(
			&pUmacInstance->dot11k_Meas_Params.measurementComplInd,
			sizeof(WSM_HI_MEASURE_CMPL_IND)
			);
 exit_handler:
#endif	/* DOT11K_SUPPORT */
	return Result;
}

/******************************************************************************
 * NAME:	UMAC_ProcSendLinkMsrmtRep
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends report to AP for link measurement request.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcSendLinkMsrmtRep(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
#if DOT11K_SUPPORT
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	UMAC_SendLinkMeasurementReport(UmacHandle);

	/*Reset the link measurement results from pUmacInstance */
	OS_MemoryReset(
			&pUmacInstance->dot11k_Meas_Params.LinkMeasuremetReq,
			sizeof(LINK_MEASUREMENT_REQUEST)
			);
	if (pUmacInstance->dot11k_Meas_Params.reqUnderProcess)
		pUmacInstance->dot11k_Meas_Params.reqUnderProcess = 0;
#endif	/* DOT11K_SUPPORT */
	return Result;
}

/******************************************************************************
 * NAME:	UMAC_ProcLinkMeasurement
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the link measurement request and send the request to
 * firmware
 * It checks the measurement report for which measurement complete Indication
 * is received and sends report to firmware,
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcLinkMeasurement(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 UmacEvent = UMAC_MAX_EVT;
#if DOT11K_SUPPORT
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	uint8 *pRadioMeasurementFrame = NULL;
	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);

	pRadioMeasurementFrame = (uint8 *) pUmacMgmt->FrameBody;

	/*Copy the Link measurement request in pUmacInstance */
	OS_MemoryCopy(
			&pUmacInstance->dot11k_Meas_Params.LinkMeasuremetReq,
			pRadioMeasurementFrame,
			5
			);
	pUmacInstance->dot11k_Meas_Params.linkMeasReqRCPI = pRxDescriptor->Rcpi;
	/*Release RxDescriptor buffer */
	WFM_UMAC_DBG_REMOVE_RX_DESC(pMsg);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/*Release the buffer received from lower layer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);

#if 0
	if (!(pUmacInstance->gVars.p.measurementCapability & (1 << 1))) {
		pUmacInstance->dot11k_Meas_Params.CurrentReportOp = INCAPABLE;
		UmacEvent = UMAC_SEND_LINK_MSRMT_REPORT;
		WFM_SCHEDULE_TO_SELF(UmacHandle, UmacEvent, NULL);
		goto exit_handler;
	}
	/*Check if current request is enabled */
	else if (!(pUmacInstance->gVars.p.enableMeasurements & (1 << 1))) {
		pUmacInstance->dot11k_Meas_Params.CurrentReportOp = REFUSED;
		UmacEvent = UMAC_SEND_LINK_MSRMT_REPORT;
		WFM_SCHEDULE_TO_SELF(UmacHandle, UmacEvent, NULL);
		goto exit_handler;
	} else {
#endif
		UmacEvent = UMAC_SEND_LINK_MSRMT_REQ;
		/*Schedule the function to send the Link measurement request to the firmware */
		WFM_SCHEDULE_TO_SELF(UmacHandle, UmacEvent, NULL);
#if 0
	}
#endif

#endif	/* DOT11K_SUPPORT */
	return Result;
}

/******************************************************************************
 * NAME:	UMAC_SendNullFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called when UMAC is in AP mode.And when it wants to
 * send the (QoS)NULL data from to an associated STA to indicate No more
 * buffered data.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_SendNullFrame(UMAC_HANDLE UmacHandle, void *pMsg)
{
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_DATA_FRAME *pNullDataFrm = NULL;
	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	uint8 linkId;
	uint8 SubType;
	void *pDriverInfo = NULL;
#if P2P_MINIAP_SUPPORT
	uint8 FrameType = 0;
	uint8 FrameSubType = 0;
	WFM_UMAC_FRAME_HDR *pDot11hdr;
	uint8  EnableQoS = TRUE;

	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
		pDot11hdr = (WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
		/* b2 and b3 constitutes Type */
		FrameType = (uint8) ((pDot11hdr->FrameControl >> 2) & 0x0003);
		/*b4,b5,b6 and b7 forms SubType */
		FrameSubType = (uint8) ((pDot11hdr->FrameControl >> 4) & 0x000F);
		linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);

		if (pUmacInstance->PsNullFrameStatus == 0)
			pUmacInstance->PsNullFrameStatus = 1;
		else {
			if (FrameType == WFM_UMAC_DATA
			   && ((FrameSubType == WFM_UMAC_SUB_DATA_NULL) || (FrameSubType == WFM_UMAC_SUB_NULL_QOS))
			) {
				UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);

				OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
				pUmacInstance->NumRxBufPending[linkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
			} else if ((FrameType == WFM_UMAC_CTRL) && (FrameSubType == WFM_UMAC_CNTRL_PS_POLL)) {
				UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);

				OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
				pUmacInstance->NumRxBufPending[linkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
			}
			return Result;
		}
	} else
#endif	/* P2P_MINIAP_SUPPORT */
		linkId = 0;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_TX, "UMAC_SendNullFrame()\n");

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);
	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	pNullDataFrm = (WFM_UMAC_DATA_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pNullDataFrm);

	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		EnableQoS = pUmacInstance->sta[linkId].isWMMEnabled;

	if (((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) ||
		(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION))
#if P2P_MINIAP_SUPPORT
		&& (EnableQoS)
#endif	/* P2P_MINIAP_SUPPORT */
	    )
		SubType = WFM_UMAC_SUB_NULL_QOS;
	else
		SubType = WFM_UMAC_SUB_DATA_NULL;

#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {

		WFM_UMAC_CreateDot11FrameHdr(
			UmacHandle,
			WFM_UMAC_DATA,
			SubType,
			&pNullDataFrm->dot11hdr,
			pUmacInstance->sta[linkId].macAddr,
			pUmacInstance->MacAddress,
			0,
			0
			);

		if (EnableQoS) {

			if (FrameSubType == WFM_UMAC_SUB_DATA_NULL || FrameSubType == WFM_UMAC_SUB_NULL_QOS
				|| FrameSubType == WFM_UMAC_SUB_DATA_QOS
				)
				pNullDataFrm->FrameBody[0] = 0x10;
			else
				pNullDataFrm->FrameBody[0] = 0x00;

			/*
			Adding QoS control field
			EOSP = 1
			Ack Policy = 00 (normal)
			AP PS Buffer state =0 (Not specified)
			*/
			pNullDataFrm->FrameBody[1] = 0;
			FrameBodyLen = 2;
		} /* if(EnableQoS) */
	} else {
#endif	/* P2P_MINIAP_SUPPORT */
		WFM_UMAC_CreateDot11FrameHdr(
			UmacHandle,
			WFM_UMAC_DATA,
			SubType,
			&pNullDataFrm->dot11hdr,
			pUmacInstance->RemoteMacAdd,
			pUmacInstance->MacAddress,
			0,
			0
			);
		if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) ||
			(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION)) {
				/*
				Adding QoS control field
				EOSP = 0
				Ack Policy = 00 (normal)
				AP PS Buffer state =0 (Not specified)
				*/
				pNullDataFrm->FrameBody[0] = 0x00;
				pNullDataFrm->FrameBody[1] = 0;
				FrameBodyLen = 2;
		}
#if P2P_MINIAP_SUPPORT
	}
#endif	/* P2P_MINIAP_SUPPORT */

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendNullFrame() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pUmacInstance->nullFrmPktId = pTxDesc->PacketId;

	pTxDesc->MaxTxRate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);
	pTxDesc->QueueId = WFM_AC_VO;
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_VOICE);
	pTxDesc->Reserved = 0;
	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL
					);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					linkId
					);

	if (Result != WFM_STATUS_SUCCESS) {
		WFM_UMAC_DBG_REMOVE_TX_DESC(pTxDesc);

		UMAC_Release_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					&pDriverInfo
					);

		UMAC_RELEASE_PACKET_ID(UmacHandle, pTxDesc->PacketId);

		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					MOVE_TO_START_OF_DEV_IF_TX_REQ(pTxDesc)
					);

		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[linkId]--;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	} /* if(Result != WFM_STATUS_SUCCESS) */

exit_handler:
#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
		if (FrameType == WFM_UMAC_DATA
		   && ((FrameSubType == WFM_UMAC_SUB_DATA_NULL) || (FrameSubType == WFM_UMAC_SUB_NULL_QOS))
		) {
			UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);

			OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
			pUmacInstance->NumRxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
		} else if ((FrameType == WFM_UMAC_CTRL) && (FrameSubType == WFM_UMAC_CNTRL_PS_POLL)) {
			UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);

			OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
			pUmacInstance->NumRxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
		}
	}
#endif	/* P2P_MINIAP_SUPPORT */

	return Result;
} /* end UMAC_SendNullFrame() */

/******************************************************************************
 * NAME:	UMAC_DlvrRequeueBufferedData
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called when UMAC is in AP or STA mode. We have deliver
 * requeue non PS buffered frame.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_DlvrRequeueBufferedData(UMAC_HANDLE UmacHandle, void *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	UMAC_TX_DESC *pTxDescriptor;
	uint32 Result = WFM_STATUS_SUCCESS;
	uint8 linkId;
	void *pDriverInfo = NULL;

	while (pUmacInstance->BuffRequeueFrms.numPendingFrames) {
		uint32 get;
		uint8 index;
		OS_LOCK(pUmacInstance->gVars.p.TxBuffRequeuePoolLock);
		get = pUmacInstance->BuffRequeueFrms.get;
		index = (uint8) (get & (NUM_REQUEUE_BUFFERS - 1));
		pDevIfTxReq = (UMAC_DEVIF_TX_DATA *) pUmacInstance->BuffRequeueFrms.frmBuffPtr[index].buffPtr;
		pTxDescriptor = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxReq + sizeof(UMAC_DEVIF_TX_DATA));
		UMAC_RETRIEVE_LINK_ID(pTxDescriptor->MsgId, linkId);
		get++;
		pUmacInstance->BuffRequeueFrms.get = get;
		pUmacInstance->BuffRequeueFrms.numPendingFrames = (uint8) (pUmacInstance->BuffRequeueFrms.put - get);
		OS_UNLOCK(pUmacInstance->gVars.p.TxBuffRequeuePoolLock);

		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[linkId]++;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		Result = UMAC_LL_REQ_TX_FRAME(
						UmacHandle,
						pTxDescriptor->QueueId,
						pDevIfTxReq,
						linkId
						);

		if (Result != WFM_STATUS_SUCCESS) {
			/*Remove the entry from TxDescStore */
			UMAC_Release_Internal_TxDesc(
					UmacHandle,
					(UMAC_TX_DESC *) pTxDescriptor,
					pUmacInstance->gVars.ApUmacTxDesc,
					UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
					&pDriverInfo
					);

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
		} /* if(Result != WFM_STATUS_SUCCESS) */
	} /* while (pUmacInstance->BuffRequeueFrms.numPendingFrames) */

	return Result;
} /* UMAC_DlvrRequeueBufferedData() */

#if P2P_MINIAP_SUPPORT

/******************************************************************************
 * NAME:	UMAC_ProcFwdedFrmTxCompleted
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called when UMAC is in AP mode. And the Tx Cnf is
 * received for a packet which was not received from the network stack rather
 * it was received from an associated STA.
 * -# Releases the Internal Buffer and Descriptor allocated
 * -# If its not an internal buffer, calls the TxCnf handler to release
 *    the memory.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcFwdedFrmTxCompleted(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	UMAC_TX_CNF_DESC *pTxCnfDesc = (UMAC_TX_CNF_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pStoredTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	void *pDriverInfo = NULL;
	uint8 linkId;
	linkId = (uint8) ((pTxCnfDesc->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);

	OS_ASSERT(pUmacInstance);

	LOG_DATA(DBG_WFM_UMAC, "UMAC_ProcFwdedFrmTxCompleted (TxComplete) - 0x%x\n", pTxCnfDesc);

	OS_ASSERT(pTxCnfDesc);

	LOG_DATA(DBG_WFM_UMAC, "Buffer to Free pMsg=0x%x)\n", pTxCnfDesc);

	WFM_UMAC_DBG_PRINT_TX_DESC();

	if (pTxCnfDesc->PacketId & UMAC_PACKETID_PS_BUFFRED_FRAME)
		pTxCnfDesc->PacketId &= ~(UMAC_PACKETID_PS_BUFFRED_FRAME);

	if (pTxCnfDesc->Status == WSM_REQUEUE) {
		pStoredTxDesc = UMAC_Get_Internal_TxDesc(
						UmacHandle,
						(UMAC_TX_DESC *) pTxCnfDesc,
						pUmacInstance->gVars.ApUmacTxDesc,
						UMAC_NUM_AP_INTERNAL_TX_BUFFERS
						);
	} else {
		pStoredTxDesc = UMAC_Release_Internal_TxDesc(
						UmacHandle,
						(UMAC_TX_DESC *) pTxCnfDesc,
						pUmacInstance->gVars.ApUmacTxDesc,
						UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
						&pDriverInfo
						);
	}

	if (!pStoredTxDesc) {
		/*
		   Problem occurred. Though the MSb of the packet Id is set
		   still the Tx descriptor is not found in store
		 */
		UMAC_RELEASE_TX_CNF_FRAME(UmacHandle, pTxCnfDesc);

		Result = WFM_STATUS_BAD_PARAM;
		goto exit_handler;

	} else {
		if (pTxCnfDesc->Status != WSM_REQUEUE) {
			WFM_UMAC_DBG_REMOVE_TX_DESC(pTxCnfDesc);

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

			UMAC_RELEASE_PACKET_ID_AP(UmacHandle, pTxCnfDesc->PacketId);

			pStoredTxDesc = (UMAC_TX_DESC *) ((uint8 *) pStoredTxDesc + (sizeof(UMAC_TX_DESC)
										     - sizeof(UMAC_RX_DESC))
			    );
			UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle,
							pStoredTxDesc,
							pDriverInfo);
			/* SACHIN
			UMAC_LL_RELEASE_RXBUFF(UmacHandle, pStoredTxDesc);*/

			UMAC_ManageRateSelection(pUmacInstance,		\
			pTxCnfDesc->Status, pTxCnfDesc->TxedRate,	\
			pTxCnfDesc->AckFailures, linkId);
			UMAC_MEM_REMOVE_STAMP(MOVE_TO_START_OF_DEV_IF_TX_REQ(pStoredTxDesc));

		} else {
			uint32 put;
			uint8 index;
			uint8 buffType;
			uint8 uapsdFlag = pUmacInstance->sta[linkId].qosInfo & UMAC_QOS_INFO_STA_UAPSD_MASK;

			if (uapsdFlag)
				buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
			else
				buffType = AP_BUFF_TYPES_LEGACY;

			OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
			put = pUmacInstance->sta[linkId].put[buffType];
			if ((put - pUmacInstance->sta[linkId].get[buffType]) >= AP_NUM_BUFFERS) {
				/*
				  Allocated buffer is full for this STA so drop
				  the packet
				*/

				OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
				pUmacInstance->NumTxBufPending[linkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
				pStoredTxDesc = UMAC_Release_Internal_TxDesc(
						UmacHandle,
						(UMAC_TX_DESC *) pTxCnfDesc,
						pUmacInstance->gVars.ApUmacTxDesc,
						UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
						&pDriverInfo
						);
				pStoredTxDesc = (UMAC_TX_DESC *) ((uint8 *) pStoredTxDesc + (sizeof(UMAC_TX_DESC)
								     - sizeof(UMAC_RX_DESC))
						    );

				UMAC_RELEASE_PACKET_ID_AP(
							UmacHandle,
							pTxCnfDesc->PacketId
							);

				UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle,
							(UMAC_RX_DESC *) pStoredTxDesc,
							pDriverInfo);

			} else {
				index = (uint8) (put & (AP_NUM_BUFFERS - 1));
				pDevIfTxReq = (UMAC_DEVIF_TX_DATA *) ((uint8 *) pStoredTxDesc - sizeof(UMAC_DEVIF_TX_DATA));
				pUmacInstance->sta[linkId].buffer[buffType][index].buffPtr = (uint8 *) pDevIfTxReq;
				pUmacInstance->sta[linkId].buffer[buffType][index].timeStamp = (OS_GetTime() + (pUmacInstance->sta[linkId].listenInterval + 4) * pUmacInstance->dot11BssConf.beaconInterval);
				put++;
				/*pUmacInstance->gVars.ApPktIdPut = put;*/
				pUmacInstance->sta[linkId].put[buffType] = put;
				pUmacInstance->sta[linkId].numPendingFrames[buffType] = (uint8) (put - pUmacInstance->sta[linkId].get[buffType]);
				OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
				pUmacInstance->NumTxBufPending[linkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

				if ((buffType == AP_BUFF_TYPES_LEGACY) || (uapsdFlag == UMAC_QOS_INFO_STA_UAPSD_MASK)) {
					OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
					if (!(pUmacInstance->virtualBitmap & (1 << linkId))) {
						pUmacInstance->virtualBitmap |= 1 << linkId;
						if (pUmacInstance->updateTimIeInProgress == FALSE) {
							pUmacInstance->updateTimIeInProgress = TRUE;
							OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
							WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);
						} else
							OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
					} else
						OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
				}
			}
			OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
		} /* if(pTxCnfDesc->Status != WSM_REQUEUE) */
		UMAC_RELEASE_TX_CNF_FRAME(UmacHandle, pTxCnfDesc);
	}

 exit_handler:
	return Result;
} /* end UMAC_ProcFwdedFrmTxCompleted() */

/******************************************************************************
 * NAME:	UMAC_DlvrLegacyBufferedData
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called when UMAC is in AP mode. And we have received
 * PS poll or NULL frame (indicating PM mode active) from an associated STA
 * for which we have frames buffered with us in legacy(Non WMM) buffer.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_DlvrLegacyBufferedData(UMAC_HANDLE UmacHandle, void *pMsg)
{
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	UMAC_TX_DESC *pTxDescriptor;
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_FRAME_HDR *pDot11Hdr;
	uint8 linkId;
	uint8 freeMsg = TRUE;
	uint8 subType;
	void *pDriverInfo = NULL;
	if (pRxDescriptor) {
		UMAC_RETRIEVE_LINK_ID(pRxDescriptor->MsgId, linkId);
	} else {
		/*
		   We received a data frame with PS bit set to 0 and that data
		   frame has been sent to stack and got freed
		 */
		for (linkId = 1; linkId < pUmacInstance->gVars.p.maxNumClientSupport; linkId++) {
			if ((pUmacInstance->sta[linkId].PsState == STA_PS_STATE_AWAKE) && pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_LEGACY])
				break;
		}
		if (linkId == pUmacInstance->gVars.p.maxNumClientSupport)
			linkId = DEFAULT_LINK_ID;
	}

	OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
	if (pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_LEGACY]) {
		uint32 get;
		uint8 index;

		/* OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]); */
		get = pUmacInstance->sta[linkId].get[AP_BUFF_TYPES_LEGACY];
		index = (uint8) (get & (AP_NUM_BUFFERS - 1));
		pDevIfTxReq = (UMAC_DEVIF_TX_DATA *) pUmacInstance->sta[linkId].buffer[AP_BUFF_TYPES_LEGACY][index].buffPtr;
		pTxDescriptor = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxReq + sizeof(UMAC_DEVIF_TX_DATA));
		pTxDescriptor->PacketId |= UMAC_PACKETID_PS_BUFFRED_FRAME;
		get++;
		pUmacInstance->sta[linkId].get[AP_BUFF_TYPES_LEGACY] = get;
		pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_LEGACY] = (uint8) (pUmacInstance->sta[linkId].put[AP_BUFF_TYPES_LEGACY] - get);
		OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);

		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->PsBuffDataCount++;
		pUmacInstance->NumTxBufPending[linkId]++;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		if (pDevIfTxReq->pTxDescriptor->Flags & UMAC_TX_REQ_FLAG_FRAME_CTRL_OFFSET_ENABLED)
			pDot11Hdr = (WFM_UMAC_FRAME_HDR *) ((uint8 *) pDevIfTxReq->pTxDescriptor + (sizeof(UMAC_TX_DESC) + 2));
		else
			pDot11Hdr = (WFM_UMAC_FRAME_HDR *) ((uint8 *) pDevIfTxReq->pTxDescriptor + sizeof(UMAC_TX_DESC));

		if (pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_LEGACY]) {
			pDot11Hdr->FrameControl |= UMAC_FC_MORE_DATA;
		} else {
			uint8 type, subType;
			OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
			pUmacInstance->virtualBitmap &= (0xFF ^ (1 << linkId));
			if (linkId == 0)
				pUmacInstance->bitmapCtrl = 0;
			OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);

			pUmacInstance->UpdateNewTIMInfo = TRUE;
			/* b2 and b3 constitutes Type */
			type = (uint8) ((pDot11Hdr->FrameControl >> 2) & 0x0003);

			/*b4,b5,b6 and b7 forms SubType */
			subType = (uint8) ((pDot11Hdr->FrameControl >> 4) & 0x000F);

			if ((type == WFM_UMAC_MGMT) && ((subType == WFM_UMAC_MGMT_DISASSOC) || (subType == WFM_UMAC_MGMT_DEAUTH)
			    )) {
				if (STA_STATE_DEAUTHENTICATING != pUmacInstance->sta[linkId].state) {
					if (linkId != 0)
						OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId]);
					pUmacInstance->sta[linkId].hiMsgHdr.MsgId = linkId<<6;
					/*b0-b5 bits are set to 1 when unlink STA is called from host side
					or from call backtimer*/
					pUmacInstance->sta[linkId].state = STA_STATE_DEAUTHENTICATING;
					WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNLINK_STA, &(pUmacInstance->sta[linkId].hiMsgHdr));
				}
			}
		}
			/* WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL); */

		Result = UMAC_LL_REQ_TX_FRAME(
						UmacHandle,
						pTxDescriptor->QueueId,
						pDevIfTxReq,
						linkId
						);

		if (Result != WFM_STATUS_SUCCESS) {
			/*Remove the entry from TxDescStore */
			UMAC_Release_Internal_TxDesc(
					UmacHandle,
					(UMAC_TX_DESC *) pTxDescriptor,
					pUmacInstance->gVars.ApUmacTxDesc,
					UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
					&pDriverInfo
					);

			/* pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_LEGACY]++; */

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
		} /* if(Result != WFM_STATUS_SUCCESS) */

		if (pRxDescriptor && (pRxDescriptor->MsgId & 0x8000)) {
			if (freeMsg) {
				/* PS poll received */
				UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
			}

			OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
			pUmacInstance->NumRxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
		} else {
			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_PS_DLVR_LEGACY_DATA, pMsg);
		}

		/*if (!pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_LEGACY])
			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);*/
	} else {
		OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
		if (pRxDescriptor && (pRxDescriptor->MsgLen)) {
			/*pRxDescriptor is freed but not NULL */
			if ((pRxDescriptor->MsgId & ~UMAC_HI_MSG_LINK_ID) != WSM_HI_SUSP_RESUME_TX_IND_ID)
				subType = ((*((uint8 *) pRxDescriptor + sizeof(UMAC_RX_DESC))) & 0xF0) >> 4;

			if ((pRxDescriptor->MsgId & ~UMAC_HI_MSG_LINK_ID) == WSM_HI_SUSP_RESUME_TX_IND_ID) {
				pUmacInstance->PendingSusResTxInd = FALSE;
			} else if (pRxDescriptor->MsgId & 0x8000) {
				/* PS poll received */
				WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle,	UMAC_SEND_NULL_DATA, pMsg);
				/* WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_SEND_NULL_DATA, pMsg); */
			} else if ((subType == WFM_UMAC_SUB_DATA_NULL) || (subType == WFM_UMAC_SUB_NULL_QOS)) {
				/* NULL data frame received */
				UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);

				OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
				pUmacInstance->NumRxBufPending[linkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
			}
		} /* if(pRxDescriptor) */
	}
	return Result;
} /* UMAC_DlvrLegacyBufferedData() */

/******************************************************************************
 * NAME:	UMAC_DlvrWMMBufferedData
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called when UMAC is in AP mode. And we have received
 * PS poll or trigger frame from an associated WMM enabled STA for which we
 * have frames buffered with us in WMM buffer.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_DlvrWMMBufferedData(UMAC_HANDLE UmacHandle, void *pMsg)
{
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	UMAC_TX_DESC *pTxDescriptor;
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_FRAME_HDR *pDot11Hdr;
	UMAC_TX_DATA_HDR *pUmacTxHdr;
	uint8 linkId;
	uint8 servicePeriodLen;
	uint8 uapsdFlag;
	uint8 rxFrameSubType = 0;
	void *pDriverInfo = NULL;
	if (pRxDescriptor) {
		UMAC_RETRIEVE_LINK_ID(pRxDescriptor->MsgId, linkId);
		pDot11Hdr = (WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
		/*b4,b5,b6 and b7 forms SubType */
		rxFrameSubType = (uint8) ((pDot11Hdr->FrameControl >> 4) & 0x000F);
	} else {
		/*
		   We received a trigger frame with PS bit set to 0 and that
		   data frame has been sent to stack and got freed
		 */
		for (linkId = 1; linkId < pUmacInstance->gVars.p.maxNumClientSupport; linkId++) {
			if ((pUmacInstance->sta[linkId].PsState == STA_PS_STATE_AWAKE) && pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED])
				break;

		}
		if (linkId == pUmacInstance->gVars.p.maxNumClientSupport)
			linkId = DEFAULT_LINK_ID;
	}

	uapsdFlag = pUmacInstance->sta[linkId].qosInfo & UMAC_QOS_INFO_STA_UAPSD_MASK;
	servicePeriodLen = ((pUmacInstance->sta[linkId].qosInfo & UMAC_QOS_INFO_STA_UAPSD_SP_LEN) >> 5) * 2;
	/*
	  Service Period Length 0 means, WMM AP may deliver all buffered frames
	*/
	if ((0 == servicePeriodLen) || (STA_PS_STATE_AWAKE == pUmacInstance->sta[linkId].PsState))
		servicePeriodLen = 0xff;


	OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
	if (pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED]) {
		uint32 get;
		uint8 index;
		uint8 type, subType;
		pUmacInstance->sta[linkId].pendingFrames = pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED];
		OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
		while (pUmacInstance->sta[linkId].pendingFrames && servicePeriodLen) {
			OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
			get = pUmacInstance->sta[linkId].get[AP_BUFF_TYPES_DLVRY_ENABLED];
			index = (uint8) (get & (AP_NUM_BUFFERS - 1));
			pDevIfTxReq = (UMAC_DEVIF_TX_DATA *) pUmacInstance->sta[linkId].buffer[AP_BUFF_TYPES_DLVRY_ENABLED][index].buffPtr;
			pTxDescriptor = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxReq + sizeof(UMAC_DEVIF_TX_DATA));
			get++;
			pTxDescriptor->PacketId |= UMAC_PACKETID_PS_BUFFRED_FRAME;
			pUmacInstance->sta[linkId].get[AP_BUFF_TYPES_DLVRY_ENABLED] = get;
			pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED]
			    = (uint8) (pUmacInstance->sta[linkId].put[AP_BUFF_TYPES_DLVRY_ENABLED] - get);
			pUmacInstance->sta[linkId].pendingFrames = pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED];
			OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);

			servicePeriodLen--;

			if (pDevIfTxReq->pTxDescriptor->Flags & UMAC_TX_REQ_FLAG_FRAME_CTRL_OFFSET_ENABLED)
				pDot11Hdr = (WFM_UMAC_FRAME_HDR *) ((uint8 *) pDevIfTxReq->pTxDescriptor + (sizeof(UMAC_TX_DESC) + 2));
			else
				pDot11Hdr = (WFM_UMAC_FRAME_HDR *) ((uint8 *) pDevIfTxReq->pTxDescriptor + sizeof(UMAC_TX_DESC));

			/* b2 and b3 constitutes Type */
			type = (uint8) ((pDot11Hdr->FrameControl >> 2) & 0x0003);

			/*b4,b5,b6 and b7 forms SubType */
			subType = (uint8) ((pDot11Hdr->FrameControl >> 4) & 0x000F);

			if (((0 == servicePeriodLen) || (0 == pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED]))
			    && ((type == WFM_UMAC_DATA))) {
				uint8 Len = 0;
				uint8 *pQosCntrlStart = NULL;
				uint8 *pEndOfExtraMem = ((uint8 *)pDevIfTxReq->pTxDescriptor + pDevIfTxReq->HeaderLength);
#if 0
				pUmacTxHdr = (UMAC_TX_DATA_HDR *) ((uint8 *) pTxDescriptor + sizeof(UMAC_TX_DESC));
#else
				pUmacTxHdr = (UMAC_TX_DATA_HDR *)pDot11Hdr;
#endif
				pQosCntrlStart = (uint8 *)&pUmacTxHdr->QoSCntrl;
				if (pEndOfExtraMem >= (pQosCntrlStart + sizeof(pUmacTxHdr->QoSCntrl)))
					pUmacTxHdr->QoSCntrl |= UMAC_QOS_INFO_AP_EOSP;
				else {
					Len = pQosCntrlStart - pEndOfExtraMem;
					*((uint16 *)((uint8 *)pDevIfTxReq->pDot11Frame + Len)) |= UMAC_QOS_INFO_AP_EOSP;
				}
			} /* if(0 == servicePeriodLen) */

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->PsBuffDataCount++;
			pUmacInstance->NumTxBufPending[linkId]++;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

			/*if (pDevIfTxReq->pTxDescriptor->Flags & UMAC_TX_REQ_FLAG_FRAME_CTRL_OFFSET_ENABLED)
				pDot11Hdr = (WFM_UMAC_FRAME_HDR *) ((uint8 *) pDevIfTxReq->pTxDescriptor + (sizeof(UMAC_TX_DESC) + 2));
			else
				pDot11Hdr = (WFM_UMAC_FRAME_HDR *) ((uint8 *) pDevIfTxReq->pTxDescriptor + sizeof(UMAC_TX_DESC));*/

			if (pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED])
				pDot11Hdr->FrameControl |= UMAC_FC_MORE_DATA;
			else {
				/* uint8 type, subType; */
				/* b2 and b3 constitutes Type */
				/* type = (uint8) ((pDot11Hdr->FrameControl >> 2) & 0x0003); */

				/*b4,b5,b6 and b7 forms SubType */
				/* subType = (uint8) ((pDot11Hdr->FrameControl >> 4) & 0x000F); */
				OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
				pUmacInstance->virtualBitmap &= (0xFF ^ (1 << linkId));
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
				pUmacInstance->UpdateNewTIMInfo = TRUE;
				if ((type == WFM_UMAC_MGMT) && ((subType == WFM_UMAC_MGMT_DISASSOC) || (subType == WFM_UMAC_MGMT_DEAUTH)
				    )) {
					if (STA_STATE_DEAUTHENTICATING != pUmacInstance->sta[linkId].state) {
						if (linkId != 0)
							OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId]);
						pUmacInstance->sta[linkId].hiMsgHdr.MsgId = linkId<<6;
						/*b0-b5 bits are set to 1 when unlink STA is called from host side
						or from call backtimer*/
						pUmacInstance->sta[linkId].state = STA_STATE_DEAUTHENTICATING;
						WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNLINK_STA, &(pUmacInstance->sta[linkId].hiMsgHdr));
					}
					break;
				}
#if 0
				if (uapsdFlag == UMAC_QOS_INFO_STA_UAPSD_MASK) {
					/*
					  Update TIM only if all the ACs are
					  trigger and delivery enabled
					*/
					OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
					if (pUmacInstance->virtualBitmap & (1 << linkId)) {
						pUmacInstance->virtualBitmap &= (0xFF ^ (1 << linkId));
						if (pUmacInstance->updateTimIeInProgress == FALSE) {
							pUmacInstance->updateTimIeInProgress = TRUE;
							OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
							WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);
						} else
							OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
					} else
						OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
				} /* if(uapsdFlag == UMAC_QOS_INFO_STA_UAPSD_MASK) */
#endif
			}

			Result = UMAC_LL_REQ_TX_FRAME(
							UmacHandle,
							pTxDescriptor->QueueId,
							pDevIfTxReq,
							linkId
							);

			if (Result != WFM_STATUS_SUCCESS) {
				/*Remove the entry from TxDescStore */
				UMAC_Release_Internal_TxDesc(
					UmacHandle,
					(UMAC_TX_DESC *) pTxDescriptor,
					pUmacInstance->gVars.ApUmacTxDesc,
					UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
					&pDriverInfo
					);

				/* pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_LEGACY]++; */

				OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
				pUmacInstance->NumTxBufPending[linkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
			} /* if(Result != WFM_STATUS_SUCCESS) */
#if 0
			if (!pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED]) {
				if (uapsdFlag == UMAC_QOS_INFO_STA_UAPSD_MASK) {
					/*
					  Update TIM only if all the ACs are
					  trigger and delivery enabled
					*/
					OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
					if (pUmacInstance->virtualBitmap & (1 << linkId)) {
						pUmacInstance->virtualBitmap &= (0xFF ^ (1 << linkId));
						if (pUmacInstance->updateTimIeInProgress == FALSE) {
							pUmacInstance->updateTimIeInProgress = TRUE;
							OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
							WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);
						} else
							OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
					} else
						OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
				} /* if(uapsdFlag == UMAC_QOS_INFO_STA_UAPSD_MASK) */
			}
#endif
		} /* while */

		if (WFM_UMAC_SUB_NULL_QOS == rxFrameSubType) {
			UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
			UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

			OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
			pUmacInstance->NumRxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
		} /* if(WFM_UMAC_SUB_NULL_QOS == rxFrameSubType) */
	} else {
		/*
		  Trigger frame received but no buffered data so send QoS NULL
		  frame
		*/

		OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
		WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle,	UMAC_SEND_NULL_DATA, pMsg);
		/*WFM_SCHEDULE_LMAC_TO_UMAC(
						UmacHandle,
						UMAC_SEND_NULL_DATA,
						pMsg
						);*/
	}
	return Result;
} /* end UMAC_DlvrWMMBufferedData() */

/******************************************************************************
 * NAME:UMAC_ProcDeAuthSTA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initiates the de-authentication procedure for a connected STA
 * This function performs the following things
 * -# Allocates memory for DeAuth frame and forms Authentication frame and
 *    queues it to LMAC for transmission.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcDeAuthSTA(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_MGMT_FRAME *pDeAuthFrame = NULL;
	uint16 FrameBodyLen = 0;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 broadcastAddr[WFM_MAC_ADDRESS_SIZE] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	uint8 remoteAddr[WFM_MAC_ADDRESS_SIZE];
	HI_MSG_HDR *pHiMsg = (HI_MSG_HDR *) pMsg;
	uint8 linkId;
	void *pDriverInfo = NULL;
	if (pHiMsg)
		linkId = (uint8) ((pHiMsg->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
	else
		linkId = 0;

	/*
	  linkId will be present only when deauth is initiated by driver for a
	  specific STA
	*/

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcDeAuthSTA()\n");

	WFM_UMAC_DBG_PRINT_TX_DESC();

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_DEAUTH_REQ);

	LOG_DATA(DBG_WFM_UMAC, "UMAC_ProcDeAuthSTA pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	pDeAuthFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pDeAuthFrame);

	/*
	   Creating the WLAN Header
	*/

	if (linkId) {
		OS_MemoryCopy(
				remoteAddr,
				pUmacInstance->sta[linkId].macAddr,
				WFM_MAC_ADDRESS_SIZE
				);
	} else {
		OS_MemoryCopy(
				remoteAddr,
				broadcastAddr,
				WFM_MAC_ADDRESS_SIZE
				);
	}
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_DEAUTH,
					&pDeAuthFrame->dot11hdr,
					remoteAddr,
					pUmacInstance->MacAddress,
					0,
					0
					);

	FrameBodyLen = WFM_UMAC_CreateDeAuthFrame(
							UmacHandle,
							pDeAuthFrame->FrameBody
							);

	pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcDeAuthSTA() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);

	pTxDesc->QueueId = WFM_AC_VO;	/* WFM_AC_VO */
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;
	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	if ((linkId == 0) || (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_AWAKE)) {
		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[linkId]++;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		/*
		   Sending the management frame to LMAC for transmission
		*/

		Result = UMAC_LL_REQ_TX_FRAME(
						UmacHandle,
						pTxDesc->QueueId,
						pDevIfTxReq,
						linkId
						);

		if (Result != WFM_STATUS_SUCCESS)
			UMAC_ReleaseTxResources(UmacHandle, pTxDesc, linkId);

		if (pUmacInstance->gVars.p.stopAPFlag) {
			uint8 i = 1;
			for (i = 1; i < pUmacInstance->gVars.p.maxNumClientSupport; i++) {
				if (pUmacInstance->sta[i].state != STA_STATE_NONE)
					pUmacInstance->sta[i].state = STA_STATE_DEAUTHENTICATING;
			}
			pUmacInstance->gVars.p.ForceStopMode = UMAC_FORCE_STOP_ON;
			if (linkId != 0)
				OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId]);
			WFM_SCHEDULE_TO_SELF((UMAC_HANDLE) UmacHandle, UMAC_UNLINK_STA, NULL);

		} else {
			if (STA_STATE_DEAUTHENTICATING != pUmacInstance->sta[linkId].state) {
				if (linkId) {
					OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId]);
					pUmacInstance->sta[linkId].hiMsgHdr.MsgId = linkId<<6;
					/*b0-b5 bits are set to 1 when unlink STA is called from host side
					or from call backtimer*/
					pUmacInstance->sta[linkId].state = STA_STATE_DEAUTHENTICATING;
					WFM_SCHEDULE_TO_SELF((UMAC_HANDLE) UmacHandle, UMAC_UNLINK_STA, (void *)&(pUmacInstance->sta[linkId].hiMsgHdr));
				}
			}
		}
		/*WFM_SCHEDULE_TO_SELF((UMAC_HANDLE) UmacHandle, UMAC_UNLINK_STA, (void *)&(pUmacInstance->sta[linkId].hiMsgHdr));*/
	} /*if((linkId==0) || (pUmacInstance->sta[linkId].PsState== STA_PS_STATE_AWAKE)) */
	else {
		uint8 bResp = FALSE;
		/* Buffer the frame, as the destination STA is in PS mode */
		bResp = UMAC_Buffer_Frame(UmacHandle, linkId, pTxDesc, 0, NULL);
		if (bResp) {
			/* Remove from the internal Tx bufferes as we have successfully storeed
			the txDesc in APTxDescStore*/
			UMAC_Release_Internal_TxDesc(UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				&pDriverInfo);
		} /*nResult*/
		else {
			uint8 uapsdFlag, buffType;
			uint32 get, put;
			uint8 index = 0;
			UMAC_TX_DESC *pStoredTxDesc;
			uapsdFlag = pUmacInstance->sta[linkId].qosInfo & UMAC_QOS_INFO_STA_UAPSD_MASK;
			if (uapsdFlag)
				buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
			else
				buffType = AP_BUFF_TYPES_LEGACY;
			OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
			get = pUmacInstance->sta[linkId].get[buffType];
			put = pUmacInstance->sta[linkId].put[buffType];
			/* Allocated buffer is full so flush all the frames and queue deauth frame */
			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId] += pUmacInstance->sta[linkId].numPendingFrames[buffType];
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

			while (pUmacInstance->sta[linkId].numPendingFrames[buffType]) {
				UMAC_DEVIF_TX_DATA *pDevIfTxRequest;
				UMAC_TX_DESC *pTxDescriptor = NULL;
				index = (uint8) (pUmacInstance->sta[linkId].get[buffType] & (AP_NUM_BUFFERS - 1));
				/*OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);*/
				pDevIfTxRequest = (UMAC_DEVIF_TX_DATA *) pUmacInstance->sta[linkId].buffer[buffType][index].buffPtr;
				pTxDescriptor = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxRequest + sizeof(UMAC_DEVIF_TX_DATA));
				get++;
				pUmacInstance->sta[linkId].get[buffType] = get;
				/*OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);*/

				if (pTxDescriptor->PacketId & UMAC_FWDED_FRAME) {
					UMAC_RELEASE_PACKET_ID_AP(UmacHandle, pTxDescriptor->PacketId);
					pStoredTxDesc = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxRequest + (sizeof(UMAC_TX_DESC)
												       - sizeof(UMAC_RX_DESC))
					    );

					UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle,
						pStoredTxDesc,
						pDriverInfo);

					OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
					pUmacInstance->NumTxBufPending[linkId]--;
					OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

					UMAC_Release_Internal_TxDesc(UmacHandle, pTxDescriptor, pUmacInstance->gVars.ApUmacTxDesc, UMAC_NUM_AP_INTERNAL_TX_BUFFERS, &pDriverInfo);
				} else {
					if (pTxDescriptor->PacketId & UMAC_PACKETID_PS_BUFFRED_FRAME)
						pTxDescriptor->PacketId &= ~(UMAC_PACKETID_PS_BUFFRED_FRAME);
					UMAC_ProcessPendingPsBuffTxRequests(pUmacInstance, linkId, pTxDescriptor);
				}
				pUmacInstance->sta[linkId].numPendingFrames[buffType] = (uint8) (pUmacInstance->sta[linkId].put[buffType] - get);
			}
			/* OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]); */
			/*UMAC_ProcessPendingTxRequests(pUmacInstance, linkId);
			if (pUmacInstance->NumTxBufPending[linkId])
				UMAC_FreePendingMgmtRequests(pUmacInstance, linkId);*/


			put = 0;
			pUmacInstance->sta[linkId].get[buffType] = 0;
			pUmacInstance->sta[linkId].buffer[buffType][put].buffPtr = (uint8 *) pDevIfTxReq;
			pUmacInstance->sta[linkId].buffer[buffType][index].timeStamp = OS_GetTime();
			put++;
			pUmacInstance->sta[linkId].put[buffType] = put;
			pUmacInstance->sta[linkId].numPendingFrames[buffType] = (uint8) (pUmacInstance->sta[linkId].put[buffType] - pUmacInstance->sta[linkId].get[buffType]);
			OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
			UMAC_Store_Internal_TxDesc(UmacHandle,
						pTxDesc,
						pUmacInstance->gVars.ApUmacTxDesc,
						UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
						0,
						NULL
						);

			/* Remove from the internal Tx bufferes */
			UMAC_Release_Internal_TxDesc(UmacHandle,
						pTxDesc,
						pUmacInstance->gVars.umacTxDescInt,
						UMAC_NUM_INTERNAL_TX_BUFFERS,
						&pDriverInfo);

		}

		if (pMsg) {
			/*Release the RxDesc and RxBuffer */
			/*UMAC_LL_RELEASE_RXBUFF(UmacHandle,pMsg ) ; */
			/*UMAC_LL_RELEASE_RXDESC( UmacHandle,pMsg ) ; */
		}
	}

exit_handler:
	return Result;
} /* end UMAC_ProcDeAuthSTA() */


/******************************************************************************
 * NAME:UMAC_ProcUnLinkSTA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function unlinks a connected STA.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcUnLinkSTA(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	HI_MSG_HDR *pHiMsg = (HI_MSG_HDR *) pMsg;
	uint8 linkId = 0;
	uint8 i;
	UMAC_EVT_DATA_DISCONNECT_INFO disConnectInfo;
	uint8 anyClientDozing = FALSE;
	void *pDriverInfo = NULL;
	uint8 IsOther11bUserPresent = FALSE;
	uint8 IsLongPreambleNeeded = FALSE;
	WSM_HI_UPDATE_IE_REQ *pUpdateIeReq = NULL;
	WFM_UMAC_ERP_IE *pUpdateERP = NULL;
	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcUnLinkSTA()\n");

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	if ((pHiMsg) && (!(pUmacInstance->gVars.p.stopAPFlag))) {
		UMAC_RETRIEVE_LINK_ID(pHiMsg->MsgId, linkId);
	} else {
		for (i = (pUmacInstance->gVars.p.maxNumClientSupport - 1); i >= 1 ; i--) {
			if (pUmacInstance->sta[i].state == STA_STATE_DEAUTHENTICATING) {
				linkId = i;
				break;
			}
		}		/*for(i=1; i<MAX_NUM_CLIENTS_SUPPORTED; i++) */
	}

	if ((linkId == 0) && (pUmacInstance->gVars.p.stopAPFlag)) {
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNJOIN, NULL);
		Result = WFM_STATUS_SUCCESS;
		goto exit_handler;
	}

	if ((pUmacInstance->sta[linkId].ResetReqSent == FALSE) && (pSmgmtReq->HandleMgmtReq == 1)) {
		WFM_SCHEDULE_TO_SELF(
					UmacHandle,
					UMAC_UNLINK_STA,
					pMsg
					);
		Result = WFM_STATUS_SUCCESS;
		goto exit_handler;
	}

	if (!pSmgmtReq->HandleMgmtReq) {
		/* Flush all the buffered frames for the STA as it is disassociating */
		if (linkId) {
			UMAC_DEVIF_TX_DATA *pDevIfTxReq;
			UMAC_TX_DESC *pTxDescriptor;
			UMAC_TX_DESC *pStoredTxDesc;
			uint32 get;
			uint8 index;
			uint8 buffType;

			OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
			for (buffType = 0; buffType < AP_BUFF_TYPES; buffType++) {
				OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
				pUmacInstance->NumTxBufPending[linkId] += pUmacInstance->sta[linkId].numPendingFrames[buffType];
				OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
				get = pUmacInstance->sta[linkId].get[buffType];

				while (pUmacInstance->sta[linkId].numPendingFrames[buffType]) {
					index = (uint8) (get & (AP_NUM_BUFFERS - 1));
					/*OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);*/
					pDevIfTxReq = (UMAC_DEVIF_TX_DATA *) pUmacInstance->sta[linkId].buffer[buffType][index].buffPtr;
					pTxDescriptor = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxReq + sizeof(UMAC_DEVIF_TX_DATA));
					get++;
					pUmacInstance->sta[linkId].get[buffType] = get;
					/*OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);*/

					if (pTxDescriptor->PacketId & UMAC_FWDED_FRAME) {
						UMAC_Release_Internal_TxDesc(UmacHandle, pTxDescriptor, pUmacInstance->gVars.ApUmacTxDesc, UMAC_NUM_AP_INTERNAL_TX_BUFFERS, &pDriverInfo);
						pStoredTxDesc = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxReq + (sizeof(UMAC_TX_DESC)
													   - sizeof(UMAC_RX_DESC))
						    );
						UMAC_RELEASE_PACKET_ID_AP(UmacHandle, pTxDescriptor->PacketId);

						UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle,
									pStoredTxDesc,
									pDriverInfo);

						OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
						pUmacInstance->NumTxBufPending[linkId]--;
						OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
					} else {
						if (pTxDescriptor->PacketId & UMAC_PACKETID_PS_BUFFRED_FRAME)
							pTxDescriptor->PacketId &= ~(UMAC_PACKETID_PS_BUFFRED_FRAME);
						UMAC_ProcessPendingPsBuffTxRequests(pUmacInstance, linkId, pTxDescriptor);
					}
					pUmacInstance->sta[linkId].numPendingFrames[buffType] = (uint8) (pUmacInstance->sta[linkId].put[buffType] - get);
				}
			} /* for(buffType=0; buffType<AP_BUFF_TYPES; buffType++) */
			OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);

			/*UMAC_ProcessPendingTxRequests(pUmacInstance, linkId);*/
		} /* if(linkId) */

		if (pUmacInstance->NumRxBufPending[linkId] || pUmacInstance->NumTxBufPending[linkId]) {
			LOG_DATA2(DBG_WFM_UMAC | DBG_WFM_ASSOC | DBG_WFM_ERROR, "NumRxBufPending[%d] = %d\n", linkId, pUmacInstance->NumRxBufPending[linkId]);
			LOG_DATA2(DBG_WFM_UMAC | DBG_WFM_ASSOC | DBG_WFM_ERROR, "NumTxBufPending[%d] = %d\n", linkId, pUmacInstance->NumTxBufPending[linkId]);
			/* There are outstanding Tx or Rx buffers, so
			Scheduling Unjoin for future */
			pUmacInstance->sta[linkId].UmacConHandle.linkId = linkId;
			pUmacInstance->sta[linkId].UmacConHandle.UmacHandle = UmacHandle;

			OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACSTAUnJoinTimer[linkId]);
			OS_START_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACSTAUnJoinTimer[linkId], WFM_UMAC_UNJOIN_TIMEOUT, (UMAC_CONNECTION_HANDLE *)&pUmacInstance->sta[linkId].UmacConHandle);

			LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcUnJoin() Pending\n");

			Result = WFM_STATUS_SUCCESS;
			goto exit_handler;
		}

		if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACSTAUnJoinTimer[linkId]))
			LOG_DATA(DBG_WFM_ERROR, "Failed to Cancel pUMACSTAUnJoinTimer Timer, linkId = %d\n", linkId);

		OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
		if (pUmacInstance->virtualBitmap & (1 << linkId)) {
			pUmacInstance->virtualBitmap &= (0xFF ^ (1 << linkId));
			if (pUmacInstance->updateTimIeInProgress == FALSE) {
				pUmacInstance->updateTimIeInProgress = TRUE;
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
				WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);
			} else
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
		} else
			OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);

		OS_LOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
		if (pUmacInstance->sta[linkId].Is11bUser == TRUE) {
			for (i = 1; i < MAX_NUM_CLIENTS_SUPPORTED; i++) {
				if ((pUmacInstance->sta[i].Is11bUser) && (i != linkId)) {
					IsOther11bUserPresent = TRUE;
					if (pUmacInstance->sta[i].IsLongPreamble)
						IsLongPreambleNeeded = TRUE;
				}
			}

			if (IsOther11bUserPresent == FALSE) {
				pUmacInstance->ErpInformation &= (~WFM_UMAC_ERP_IE_NON_ERP_PRESENT);
				if (pUmacInstance->ErpOLBC == FALSE)
					pUmacInstance->ErpInformation &= (~WFM_UMAC_ERP_IE_USE_PROTECTION);
			}

			if ((IsLongPreambleNeeded == FALSE) && pUmacInstance->sta[linkId].IsLongPreamble)
				pUmacInstance->ErpInformation &= (~WFM_UMAC_ERP_IE_BARKER_PREAMBLE_MODE);

			if (pUmacInstance->ErpInformation != pUmacInstance->ErpInfoPrevious) {
				pUmacInstance->ErpIeToBeUpdated = TRUE;
				if ((pUmacInstance->ErpInformation & WFM_UMAC_ERP_IE_USE_PROTECTION) == WFM_UMAC_ERP_IE_USE_PROTECTION)
					pUmacInstance->ErpUseCtsToSelf = TRUE;
				else
					pUmacInstance->ErpUseCtsToSelf = FALSE;
			} else {
				pUmacInstance->ErpIeToBeUpdated = FALSE;
			}
			if ((pUmacInstance->updateERPIeInProgress == FALSE) && (pUmacInstance->ErpIeToBeUpdated == TRUE)) {
				pUmacInstance->updateERPIeInProgress = TRUE;
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
				WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_ERP_IE, NULL);
			} else {
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
			}
		} else {
			OS_UNLOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
		}

		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;
		pSmgmtReq->Type = SMGMT_REQ_TYPE_UNLINK_STA;
		pSmgmtReq->ReqNum = linkId;
		pUmacInstance->sta[linkId].ResetReqSent = TRUE;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;	/* if( Result != WFM_STATUS_SUCCESS ) */
	} /* if(!pSmgmtReq->HandleMgmtReq) */

	/*UMAC_STA_UPDATE_STATE(UmacHandle, linkId, STA_STATE_NONE); */
	pSmgmtReq->HandleMgmtReq = 0;
	pUmacInstance->sta[linkId].ResetReqSent = FALSE;
	disConnectInfo.linkId = linkId;
	pUmacInstance->dot11BssConf.numSTAsConnected--;

	for (i = 1; i < MAX_NUM_CLIENTS_SUPPORTED; i++) {
		if ((pUmacInstance->sta[i].state == STA_STATE_ASSOCIATED) &&
			(pUmacInstance->sta[i].isWMMEnabled == FALSE)) {
				pUmacInstance->sta[DEFAULT_LINK_ID].isWMMEnabled = FALSE;
				break;
		}
	}

	if ((i == MAX_NUM_CLIENTS_SUPPORTED) && (pUmacInstance->dot11BssConf.numSTAsConnected >= 1))
		pUmacInstance->sta[DEFAULT_LINK_ID].isWMMEnabled = TRUE;
	else if (pUmacInstance->dot11BssConf.numSTAsConnected == 0)
		pUmacInstance->sta[DEFAULT_LINK_ID].isWMMEnabled = FALSE;

	/*Send the disconnect event from here*/
	if ((STA_STATE_DEAUTHENTICATING == pUmacInstance->sta[linkId].state) && pUmacInstance->sta[linkId].sendDisconnEvent) {
		if (pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[linkId])
			OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[linkId]);
		OS_MemoryReset(&pUmacInstance->sta[linkId], sizeof(UMAC_STA_DB));
		pUmacInstance->sta[linkId].PsState = STA_PS_STATE_AWAKE;
		/* UMAC_STA_UPDATE_STATE(UmacHandle, linkId, STA_STATE_NONE); */
		/* Cancel the Inactivity Timer as we got a valid response */
#if 0
		if (OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId]))
			LOG_DATA(DBG_WFM_ERROR, "Failed to Cancel pUMACInactivityTimer Timer, linkId = %d\n", linkId);
#endif

		if (!pUmacInstance->gVars.p.stopAPFlag) {
			WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_DISCONNECTED,
						sizeof(disConnectInfo),
						(uint8 *) &disConnectInfo,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);
			}
		anyClientDozing = UMAC_Any_Client_Dozing(UmacHandle);
		if (FALSE == anyClientDozing)
			UMAC_CHANGE_STA_PS_STATE(UmacHandle, DEFAULT_LINK_ID, STA_PS_STATE_AWAKE);

	} else if ((STA_STATE_DEAUTHENTICATING == pUmacInstance->sta[linkId].state) && (!pUmacInstance->sta[linkId].sendDisconnEvent)) {
		if (pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[linkId])
			OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[linkId]);
		OS_LOCK(pUmacInstance->gVars.p.StaDataBase);
		OS_MemoryReset(&pUmacInstance->sta[linkId], sizeof(UMAC_STA_DB));
		pUmacInstance->sta[linkId].PsState = STA_PS_STATE_AWAKE;
		OS_UNLOCK(pUmacInstance->gVars.p.StaDataBase);
	}

	if ((pUmacInstance->NumTxBufPending[linkId]) && (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_DOZE))
		UMAC_FreePendingMgmtRequests(pUmacInstance, linkId);

	if (pUmacInstance->gVars.p.stopAPFlag) {
		if (pUmacInstance->dot11BssConf.numSTAsConnected) {
			if (linkId != 0)
				OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId]);
			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNLINK_STA, NULL);
		} else {
			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UNJOIN, NULL);
		}

	}

 exit_handler:
	return Result;

} /* end UMAC_ProcUnLinkSTA() */

/******************************************************************************
 * NAME: UMAC_ProcStopFind
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for Stopping a UMAC Instance.UmacHandle should
 * be created with UMAC_Create() and started using UMAC_Start()before calling
 * this  function. UmacHandle should be created with UMAC_Create() before
 * calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcStopFind(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	if (pUmacInstance->UmacFindState == UMAC_FINDING)
		UMAC_LL_REQ_STOP_FIND(UmacHandle);

	return Status;
}

/******************************************************************************
 * NAME: UMAC_ProcStartGoNego
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for sending the group negotiation request.
 * UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this  function.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the message.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcStartGoNego(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	WFM_UMAC_MGMT_FRAME *pGONegoReq = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	uint8 linkId = 0;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_ProcStartGoNego()\n");

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pUmacInstance->Channelnumber = pUmacInstance->p2pGroupConfig.peerListenChnl;

	pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;
	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;

		pSmgmtReq->Type = SMGMT_REQ_TYPE_GO_NEGOTIATION;

		pSmgmtReq->ReqNum = 0;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;

	} /* if(!pSmgmtReq->HandleMgmtReq) */

	pSmgmtReq->HandleMgmtReq = 0;

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_GO_NEG_REQ);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_ProcStartGoNego pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
	WFM_UMAC_DBG_PRINT_TX_DESC();

	pGONegoReq = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pGONegoReq);

	/*
	   Creating the WLAN Header
	*/
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_ACTION,
					&pGONegoReq->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					0
					);

	FrameBodyLen = WFM_UMAC_CreateGoNegoReqResp(
					UmacHandle,
					pGONegoReq->FrameBody,
					TRUE,
					0 /* Status(ignored in GO neg req) */
					);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcStartGoNego() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;
	pTxDesc->QueueId = 0;	/* WFM_AC_VO ; */
	pTxDesc->More = 0;

	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
				UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL
				);

	LOG_BYTES(DBG_WFM_UMAC, "GO Nego Req: ", (uint8 *) pGONegoReq, GET_TX_DESC_FRAME_LENGTH(pTxDesc), GET_TX_DESC_FRAME_LENGTH(pTxDesc)
	    );

	/*Starting the UMAC timer */
	OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pUMACTimer,
			WFM_UMAC_MGMT_FRAME_TIMEOUT,
			UmacHandle
			);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					linkId
					);

	UMAC_SET_STATE(UmacHandle, UMAC_GROUP_FORMING);
	LOG_EVENT(DBG_WFM_UMAC, "STATE   = UMAC_GROUP_FORMING\n");

	if (Result != WFM_STATUS_SUCCESS) {

		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, linkId);

		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");

	} /* if(Result != WFM_STATUS_SUCCESS) */
 exit_handler:
	return Result;
}

/******************************************************************************
 * NAME: UMAC_ProcStartGoNegoCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for Stopping a UMAC Instance.UmacHandle should
 * be created with UMAC_Create() and started using UMAC_Start()before calling
 * this  function. UmacHandle should be created with UMAC_Create() before
 * calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcStartGoNegoCnf(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	return Status;
}

/******************************************************************************
 * NAME:UMAC_ProcStartGoNegoResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for sending GO negotiation response frame when
 * GO negotiation request is received. UmacHandle should be created with
 * UMAC_Create() and started using UMAC_Start()before calling this  function.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcStartGoNegoResp(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	return Status;
}

/******************************************************************************
 * NAME:UMAC_ProcGoNegoReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for processing the incoming GO negotiation
 * request. UmacHandle should be created with UMAC_Create() before calling this
 * fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcGoNegoReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint16 IELength;
	uint32 Result = WFM_STATUS_SUCCESS;
	IeElement Ie;
	uint8 *IeElementStart;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	UMAC_ACTION_FRAME_BODY *pActionFrame = NULL;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_UMAC_MGMT_FRAME *pGONegoReq = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	uint16 status = WFM_STATUS_SUCCESS;
	uint8 isClient = TRUE;

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pUmacInstance->Channelnumber = pUmacInstance->p2pDevDiscvry.listenChannel;

	pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;
	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;

		pSmgmtReq->Type = SMGMT_REQ_TYPE_GO_NEGOTIATION;

		pSmgmtReq->ReqNum = 0;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;
	} /* if(!pSmgmtReq->HandleMgmtReq) */

	pSmgmtReq->HandleMgmtReq = 0;

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_GO_NEG_REQ);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_ProcGoNegoReq pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
	WFM_UMAC_DBG_PRINT_TX_DESC();

	pGONegoReq = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);

	CHECK_POINTER_IS_VALID(pGONegoReq);
	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pMsg);
	pActionFrame = (UMAC_ACTION_FRAME_BODY *) pUmacMgmt->FrameBody;

	LOG_EVENT(DBG_WFM_FRAMES, "UMAC_ProcGoNegoReq()\n");
	/* Preserve the dialog token */
	pUmacInstance->dialogToken = pActionFrame->dialogToken;
	IELength = pRxDescriptor->MsgLen - (sizeof(UMAC_RX_DESC)
					    + sizeof(UMAC_ACTION_FRAME_BODY)
	    );
	IeElementStart = (uint8 *) &pActionFrame->dialogToken + 1;
	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CAPABILITY, &Ie) == TRUE) {
		uint8 devCapability;
		devCapability = Ie.info[0];
	}

	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_GO_INTENT, &Ie) == TRUE) {
		uint8 GOIntent;
		GOIntent = *(Ie.info) & UMAC_P2P_GO_INTENT;
		if (GOIntent > pUmacInstance->p2pGrpFormtn.intentValue) {
			isClient = TRUE;
			pUmacInstance->operatingMode = OPERATING_MODE_P2P_CLIENT;
		} else if (GOIntent == pUmacInstance->p2pGrpFormtn.intentValue) {
			if (GOIntent == 15) {
				status = P2P_STATUS_FAILED_INTENT_15;
				goto exit_handler;
			} else {
				if (*(Ie.info) & UMAC_P2P_TIE_BREAKER) {
					isClient = TRUE;
					pUmacInstance->operatingMode = OPERATING_MODE_P2P_CLIENT;
				} else {
					isClient = FALSE;
					pUmacInstance->operatingMode = OPERATING_MODE_GO;
				}
			} /* if(GOIntent == 15) */
		} else {
			isClient = FALSE;
			pUmacInstance->operatingMode = OPERATING_MODE_GO;
		}
	}

	/*if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CONF_TIMEOUT, &Ie) == TRUE) */
		/*TBD*/


	/*if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_LISTEN_CHANNEL, &Ie) == TRUE) */
		/* GO negoatiation already started, so ignore listen channel of peer device */


	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_INTND_IF_ADDR, &Ie) == TRUE) {
		if (pUmacInstance->operatingMode == OPERATING_MODE_P2P_CLIENT)
			OS_MemoryCopy(pUmacInstance->dot11BssConf.bssid, Ie.info, WFM_MAC_ADDRESS_SIZE);

	}

	/*if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CHANNEL_LIST, &Ie) == TRUE) */


	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_DEVICE_INFO, &Ie) == TRUE) {
		uint8 *devName;
		uint8 numSecDeviceTypes;
		numSecDeviceTypes = *(Ie.info + WFM_MAC_ADDRESS_SIZE + UMAC_P2P_ATTRIB_CONFIG_METHOD_LENGTH + UMAC_P2P_ATTRIB_PRIM_DEV_TYP_LENGTH);
		devName = Ie.info + WFM_MAC_ADDRESS_SIZE + UMAC_P2P_ATTRIB_CONFIG_METHOD_LENGTH + UMAC_P2P_ATTRIB_PRIM_DEV_TYP_LENGTH + numSecDeviceTypes * UMAC_P2P_ATTRIB_SEC_DEV_TYP_LENGTH;

		/* Device Name is in TLV format */
		if (!OS_MemoryEqual((uint8 *) devName + 4, *(devName + 2), (uint8 *) pUmacInstance->wscIE.devName, *(devName + 2)
		    )) {
			status = P2P_STATUS_FAILED_INCOMPAT_PARAMS;
			goto exit_handler;
		}
	}
	/*UMAC_SET_STATE(UmacHandle,UMAC_GROUP_FORMING) ; */
	/*LOG_EVENT( DBG_WFM_UMAC, "STATE   = UMAC_GROUP_FORMING\n" ) ; */

#if 0
	WFM_UMAC_2_UL_SendEvent(
				UmacHandle,
				WFM_EVT_GROUP_FORMATION_REQ,
				sizeof(grpFormReq),
				(uint8 *) &grpFormReq,
				WFM_UMAC_EVT_NOT_RETRANSMIT
				);
#endif

 exit_handler:

	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pMsg);
	status = (uint8) UMAC_SendGoNegoResp(UmacHandle, status);

	return status;
} /* UMAC_ProcGoNegoReq() */

/******************************************************************************
 * NAME: UMAC_ProcGoNegoResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for processing Go Negotiation response.
 * UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this  function. UmacHandle should be created with
 * UMAC_Create() before calling this fucntion.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the received message.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcGoNegoResp(UMAC_HANDLE UmacHandle, void *pMsg)
{
	IeElement Ie;
	uint8 *IeElementStart;
	UMAC_RX_DESC *pRxDescriptor;
	uint16 IELength;
	uint16 frameBodyLen;
	uint8 confTimeoutGO;
	uint8 confTimeoutClient;
	uint8 GOIntent;
	uint8 isClient = TRUE;
	uint8 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	UMAC_ACTION_FRAME_BODY *pActionFrame = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_EVENT(DBG_WFM_FRAMES, "UMAC_ProcGoNegoResp()\n");

	pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pMsg);
	pActionFrame = (UMAC_ACTION_FRAME_BODY *) pUmacMgmt->FrameBody;
	frameBodyLen = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor) - sizeof(WFM_UMAC_FRAME_HDR));
	IELength = pRxDescriptor->MsgLen - (sizeof(UMAC_RX_DESC)
					    + sizeof(UMAC_ACTION_FRAME_BODY)
	    );
	IeElementStart = (uint8 *) &pActionFrame->dialogToken + 1;

	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_STATUS, &Ie) == TRUE) {
		if (*(Ie.info) == P2P_STATUS_SUCCESS) {
			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CAPABILITY, &Ie) == TRUE) {
				uint8 devCapability;
				uint8 groupCapability;
				devCapability = Ie.info[0];
				groupCapability = Ie.info[1];

				if (devCapability & UMAC_DC_SERVICE_DISCOVERY) {
					status = P2P_STATUS_FAILED_INCOMPAT_PARAMS;
					goto exit_handler;
				} else if (devCapability & UMAC_DC_CONCURRENT_OPERATION) {
					status = P2P_STATUS_FAILED_INCOMPAT_PARAMS;
					goto exit_handler;
				} else if (devCapability & UMAC_DC_INFRA_MANAGED) {
					status = P2P_STATUS_FAILED_INCOMPAT_PARAMS;
					goto exit_handler;
				} else if ((groupCapability & UMAC_GC_PERSISTENT_GROUP) != pUmacInstance->p2pGrpFormtn.persistentGroup) {
					status = P2P_STATUS_FAILED_INCOMPAT_PARAMS;
					goto exit_handler;
				}
			}

			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_GO_INTENT, &Ie) == TRUE) {
				GOIntent = *(Ie.info) & UMAC_P2P_GO_INTENT;
				if (GOIntent > pUmacInstance->p2pGrpFormtn.intentValue) {
					isClient = TRUE;
					pUmacInstance->operatingMode = OPERATING_MODE_P2P_CLIENT;
				} else if (GOIntent == pUmacInstance->p2pGrpFormtn.intentValue) {
					if (GOIntent == 15) {
						status = P2P_STATUS_FAILED_INTENT_15;
						goto exit_handler;
					} else {
						if (*(Ie.info) & UMAC_P2P_TIE_BREAKER) {
							isClient = TRUE;
							pUmacInstance->operatingMode = OPERATING_MODE_P2P_CLIENT;
						} else {
							isClient = FALSE;
							pUmacInstance->operatingMode = OPERATING_MODE_GO;
						}
					} /* if(GOIntent == 15) */
				} else {
					isClient = FALSE;
					pUmacInstance->operatingMode = OPERATING_MODE_GO;
				}
			}
			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CONF_TIMEOUT, &Ie) == TRUE) {
				if (!isClient) {
					confTimeoutGO = Ie.info[0];
					confTimeoutClient = Ie.info[1];
				}
			}

			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_OP_CHANNEL, &Ie) == TRUE) {
				/*
				  Ignore operating Channel attribute if we
				  are GO
				*/
				if (isClient) {
					pUmacInstance->p2pGroupConfig.regClass = Ie.info[0];
					pUmacInstance->p2pGroupConfig.opChannel = Ie.info[1];
				}
			}

			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_INTND_IF_ADDR, &Ie) == TRUE) {
				uint8 macAddr[WFM_MAC_ADDRESS_SIZE];
				OS_MemoryCopy(macAddr, Ie.info, WFM_MAC_ADDRESS_SIZE);
				UMAC_DB_Insert_STA(UmacHandle, macAddr);
				if (pUmacInstance->operatingMode == OPERATING_MODE_P2P_CLIENT)
					OS_MemoryCopy(pUmacInstance->dot11BssConf.bssid, Ie.info, WFM_MAC_ADDRESS_SIZE);

			}

			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CHANNEL_LIST, &Ie) == TRUE) {
				uint8 numChannels;
				uint8 i;
				Ie.info += WFM_CONFIG_COUNTRY_CODE_LENGTH + 1;
				numChannels = *(Ie.info);
				/*
				  We are only supporting 2G channels. And
				  assuming only one regulatory class in 2G.
				  And we support all channels of 2G
				*/
				for (i = 0; i < numChannels; i++)
					pUmacInstance->dot11BssConf.channels[i] = Ie.info[i];

				pUmacInstance->dot11BssConf.numOfChannels = numChannels;
			}

			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_DEVICE_INFO, &Ie) == TRUE) {
				uint8 devAddr[WFM_MAC_ADDRESS_SIZE];
				OS_MemoryCopy(devAddr, Ie.info, WFM_MAC_ADDRESS_SIZE);
				/* TBD: How to use device address */
			}
		} /* if(*(Ie.info) == P2P_STATUS_SUCCESS) */

 exit_handler:

		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pMsg);

		status = (uint8) UMAC_SendGoNegoCnf(UmacHandle, status);
		/*WFM_UMAC_CreateGoNegoCnf(
		   UmacHandle      ,
		   pFrameBody      ,
		   status
		   ); */
	} /* if(WFM_UMAC_ProcIeElements...) */

	return status;
}

#if 0
/******************************************************************************
 * NAME: UMAC_ProcGoNegoCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for processing Go Negotiation Confirmation.
 * UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this  function. UmacHandle should be created with
 * UMAC_Create() before calling this fucntion.
 * \param UmacHandle		- Handle to UMAC Instance.
 * \param pMsg			- Pointer to the received message.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcGoNegoCnf(UMAC_HANDLE UmacHandle, void *pMsg)
{
#if 0
	IeElement Ie;
	uint8 *IeElementStart = NULL;
	UMAC_RX_DESC *pRxDescriptor;
	uint16 IELength;
	uint16 frameBodyLen;
	uint8 isClient = TRUE;
	uint8 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMI_EVT_P2P_DATA_GROUP_FORMATION_INFO grpFormatnInfo;

	LOG_EVENT(DBG_WFM_FRAMES, "UMAC_ProcGoNegoCnf()\n");

	pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	frameBodyLen = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor) - sizeof(WFM_UMAC_FRAME_HDR));
	IELength = frameBodyLen - sizeof(UMAC_ACTION_FRAME_BODY);

	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_STATUS, &Ie) == TRUE) {
		if (*(Ie.info) == P2P_STATUS_SUCCESS) {
			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CAPABILITY, &Ie) == TRUE) {
				uint8 groupCapability;
				groupCapability = Ie.info[1];
				pUmacInstance->dot11BssConf.groupCapability = groupCapability;

			}

			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CHANNEL_LIST, &Ie) == TRUE) {
				uint8 numChannels;
				uint8 i;
				Ie.info += WFM_CONFIG_COUNTRY_CODE_LENGTH + 1 /*Ignoring regulatory class */ ;
				numChannels = *(Ie.info);
				for (i = 0; i < numChannels; i++)
					pUmacInstance->dot11BssConf.channels[i] = Ie.info[i];

				pUmacInstance->dot11BssConf.numOfChannels = numChannels;
			}

			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_OP_CHANNEL, &Ie) == TRUE) {
				pUmacInstance->dot11BssConf.regClass = (uint8) Ie.info[0];
				pUmacInstance->dot11BssConf.channelNum = (uint8) (Ie.info[1]);
			}
			UMAC_SET_STATE(UmacHandle, UMAC_GROUP_FORMED);

			if (pUmacInstance->operatingMode == OPERATING_MODE_GO)
				grpFormatnInfo.role = 1;
			else
				grpFormatnInfo.role = 0;

			OS_MemoryCopy(grpFormatnInfo.p2pGroupBssid, pUmacInstance->dot11BssConf.bssid, WFM_MAC_ADDRESS_SIZE);

			/* Inform the host that GO formation succeeded */
			WFM_UMAC_2_UL_SendEvent(UmacHandle, WFM_EVT_GROUP_FORMATION_SUCCEEDED, sizeof(grpFormatnInfo), (uint8 *) &grpFormatnInfo, WFM_UMAC_EVT_NOT_RETRANSMIT);

		}		/*if(*(Ie.info) == P2P_STATUS_SUCCESS) */
		else {
			UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
			/* Inform the host that GO formation failed */
			WFM_UMAC_2_UL_SendEvent(UmacHandle, WFM_EVT_GROUP_FORMATION_FAILED, 0, NULL, WFM_UMAC_EVT_NOT_RETRANSMIT);
			pUmacInstance->operatingMode = OPERATING_MODE_P2P_DEVICE;
		}

		if (pUmacInstance->operatingMode == OPERATING_MODE_GO) {
			/* Start Beaconing, since we are GO now */
			pUmacInstance->dot11BssConf.band = PHY_BAND_2G;
			pUmacInstance->dot11BssConf.mode = OPERATING_MODE_GO;
			pUmacInstance->dot11BssConf.channelNum = pUmacInstance->p2pGrpFormtn.opChannel;

			OS_MemoryCopy(&pUmacInstance->dot11BssConf.CTWindow, &pUmacInstance->p2pGrpFormtn.CTWindow, 11	/* sizeof(CTWindow + beaconInterval + DTIMPeriod + preambleType + probeDelay) */
			    );
			pUmacInstance->dot11BssConf.basicRates = pUmacInstance->p2pGrpFormtn.basicRateSet;

			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_AP, (void *)&pUmacInstance->dot11BssConf);
		}		/*if(pUmacInstance->operatingMode == OPERATING_MODE_GO) */
		else if (pUmacInstance->operatingMode == OPERATING_MODE_P2P_CLIENT)
			UMAC_SET_STATE(UmacHandle, UMAC_JOINED);


		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pMsg);
	}			/*if(WFM_UMAC_ProcIeElements...) */
#endif
	return (uint32) WFM_STATUS_SUCCESS;

} /* end UMAC_ProcGoNegoCnf() */
#endif

/******************************************************************************
 * NAME: UMAC_ProcGoNegoSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for processing the GO negotiation success.
 * UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this  function. UmacHandle should be created with
 * UMAC_Create() before calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcGoNegoSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	if (pUmacInstance->operatingMode == OPERATING_MODE_GO) {
		UMAC_SET_STATE(UmacHandle, UMAC_GROUP_FORMED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE   = UMAC_GROUP_FORMED\n");
	}

	if (pUmacInstance->operatingMode == OPERATING_MODE_P2P_CLIENT) {
		UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE   = UMAC_JOINED\n");
	}
	return status;
}

/******************************************************************************
 * NAME:UMAC_ProcGoNegoFailed
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for processing the GO negotiation failure.
 * UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this  function. UmacHandle should be created with
 * UMAC_Create() before calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcGoNegoFailed(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	pUmacInstance->operatingMode = OPERATING_MODE_P2P_DEVICE;
	return status;
}

/******************************************************************************
 * NAME: UMAC_ProcStartInvtn
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for starting the invitation procedure.
 * UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this function. UmacHandle should be created with
 * UMAC_Create() before calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcStartInvtn(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	WFM_UMAC_MGMT_FRAME *pInvtnReq = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	uint8 linkId = 0;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_ProcStartInvtn()\n");

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_GO_NEG_REQ);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_ProcStartInvtn pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
	WFM_UMAC_DBG_PRINT_TX_DESC();

	pInvtnReq = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pInvtnReq);

	/*
	   Creating the WLAN Header
	*/
	WFM_UMAC_CreateDot11FrameHdr(
				UmacHandle,
				WFM_UMAC_MGMT,
				WFM_UMAC_MGMT_ACTION,
				&pInvtnReq->dot11hdr,
				pUmacInstance->p2pInvtn.targetStaAddress,
				pUmacInstance->MacAddress,
				0,
				0
				);
	/* Duplication to be removed */
	D0_ADDR_COPY(
			pInvtnReq->dot11hdr.Address1,
			pUmacInstance->p2pInvtn.targetStaAddress
			);
	D0_ADDR_COPY(
			pInvtnReq->dot11hdr.Address3,
			pUmacInstance->dot11BssConf.bssid
			);

	FrameBodyLen = WFM_UMAC_CreateInvtnReqResp(
				UmacHandle,
				pInvtnReq->FrameBody,
				1,
				0 /* Status(ignored in Invitation req) */
				);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcStartInvtn() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;
	pTxDesc->QueueId = 0;	/* WFM_AC_VO ; */
	pTxDesc->More = 0;

	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
				UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL
				);

	UMAC_SET_STATE(UmacHandle, UMAC_BSS_STARTING);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_BSS_STARTING\n");

	LOG_BYTES(DBG_WFM_UMAC, "Invitation Req: ", (uint8 *) pInvtnReq, GET_TX_DESC_FRAME_LENGTH(pTxDesc), GET_TX_DESC_FRAME_LENGTH(pTxDesc));

	/*Starting the UMAC timer */
	OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pUMACTimer,
			WFM_UMAC_MGMT_FRAME_TIMEOUT,
			UmacHandle
			);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					linkId
					);

	if (Result != WFM_STATUS_SUCCESS) {

		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, linkId);

		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");

	} /* if(Result != WFM_STATUS_SUCCESS) */

exit_handler:
	return Result;
}

/******************************************************************************
 * NAME: UMAC_ProcInvtnReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for processing inivitation request. UmacHandle
 * should be created with UMAC_Create() and started using UMAC_Start() before
 * calling this  function. UmacHandle should be created with UMAC_Create()
 * before calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcInvtnReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint16 IELength;
	IeElement Ie;
	uint8 *IeElementStart;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	UMAC_ACTION_FRAME_BODY *pActionFrame = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;

	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pMsg);
	pActionFrame = (UMAC_ACTION_FRAME_BODY *) pUmacMgmt->FrameBody;

	LOG_EVENT(DBG_WFM_FRAMES, "UMAC_ProcInvtnReq()\n");
	/* Preserve the dialog token */
	pUmacInstance->dialogToken = pActionFrame->dialogToken;
	IELength = pRxDescriptor->MsgLen - (sizeof(UMAC_RX_DESC)
					    + sizeof(UMAC_ACTION_FRAME_BODY)
	    );
	IeElementStart = (uint8 *) &pActionFrame->dialogToken + 1;

	/*if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CONF_TIMEOUT, &Ie) == TRUE) */
		/* TBD: How to handle the GO and client conf timeouts */

	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_OP_CHANNEL, &Ie) == TRUE) {
		pUmacInstance->dot11BssConf.regClass = Ie.info[0];
		pUmacInstance->dot11BssConf.channelNum = Ie.info[1];
	}
	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_GROUP_BSSID, &Ie) == TRUE) {
		/* The invitation request has come from a group owner */
		OS_MemoryCopy(
				pUmacInstance->dot11BssConf.bssid,
				Ie.info,
				WFM_MAC_ADDRESS_SIZE
				);
	}
	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CHANNEL_LIST, &Ie) == TRUE) {
		/*Assuming support for only 2.4 G */
		uint8 i;
		OS_MemoryCopy(
			pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString,
			Ie.info,
			WFM_DOT11_COUNTRY_STRING_SIZE
			);
		pUmacInstance->dot11BssConf.numOfChannels = Ie.info[WFM_DOT11_COUNTRY_STRING_SIZE + 1];
		for (i = 0; i < pUmacInstance->dot11BssConf.numOfChannels; i++)
			pUmacInstance->dot11BssConf.channels[i] = Ie.info[WFM_DOT11_COUNTRY_STRING_SIZE + i + 2];
	}
	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_GROUP_ID, &Ie) == TRUE) {
		OS_MemoryCopy(
				pUmacInstance->p2pInvtn.p2pGroupIdAddress,
				Ie.info,
				WFM_MAC_ADDRESS_SIZE
				);

		pUmacInstance->p2pInvtn.ssidLength = Ie.info[WFM_MAC_ADDRESS_SIZE];
		OS_MemoryCopy(
				pUmacInstance->p2pInvtn.p2pGroupIdSsid,
				&Ie.info[WFM_MAC_ADDRESS_SIZE + 1],
				pUmacInstance->p2pInvtn.ssidLength
				);
	}

	/*if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_INVTN_FLAGS, &Ie) == TRUE)*/
		/*TBD*/

	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pMsg);
	/*
	  Status code: The request has been received and passed up to higher
	  layers.
	*/
	status = (uint8) UMAC_SendInvtnResp(
						UmacHandle,
						P2P_STATUS_PASSED_TO_UL
						);

	return status;
}

/******************************************************************************
 * NAME: UMAC_ProcInvtnResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for processing inivitation response. UmacHandle
 * should be created with UMAC_Create() and started using UMAC_Start()before
 * calling this  function. UmacHandle should be created with UMAC_Create()
 * before calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcInvtnResp(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint16 IELength;
	IeElement Ie;
	uint8 *IeElementStart;
	uint32 status = WFM_STATUS_SUCCESS;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	UMAC_ACTION_FRAME_BODY *pActionFrame = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pMsg);
	pActionFrame = (UMAC_ACTION_FRAME_BODY *) pUmacMgmt->FrameBody;

	LOG_EVENT(DBG_WFM_FRAMES, "UMAC_ProcInvtnResp()\n");
	/* Preserve the dialog token */
	pUmacInstance->dialogToken = pActionFrame->dialogToken;

	IELength = pRxDescriptor->MsgLen - (sizeof(UMAC_RX_DESC)
					    + sizeof(UMAC_ACTION_FRAME_BODY)
	    );
	IeElementStart = (uint8 *) &pActionFrame->dialogToken + 1;

	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_STATUS, &Ie) == TRUE) {
		if (WFM_STATUS_SUCCESS == Ie.info[0]) {

			if (UMAC_ROLE_P2P_GO == pUmacInstance->p2pInvtn.role)
				UMAC_DB_Insert_STA(UmacHandle, pUmacInstance->p2pInvtn.targetStaAddress);

			WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_INVITATION_SUCCEEDED,
						0,
						NULL,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);

			/*if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CONF_TIMEOUT, &Ie) == TRUE) */
				/*TBD: How to handle with Conf timeouts */

			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_OP_CHANNEL, &Ie) == TRUE) {
				if (UMAC_ROLE_P2P_CLIENT == pUmacInstance->p2pInvtn.role) {
					pUmacInstance->dot11BssConf.regClass = Ie.info[0];
					pUmacInstance->dot11BssConf.channelNum = Ie.info[1];
				}
			}
			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_GROUP_BSSID, &Ie) == TRUE)
				OS_MemoryCopy(
					pUmacInstance->dot11BssConf.bssid,
					Ie.info,
					WFM_MAC_ADDRESS_SIZE
					);


			if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ATTRIB_CHANNEL_LIST, &Ie) == TRUE) {
				uint8 i;
				OS_MemoryCopy(
					pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString,
					Ie.info,
					WFM_DOT11_COUNTRY_STRING_SIZE
					);
				pUmacInstance->dot11BssConf.numOfChannels = Ie.info[WFM_DOT11_COUNTRY_STRING_SIZE + 1];
				for (i = 0; i < pUmacInstance->dot11BssConf.numOfChannels; i++)
					pUmacInstance->dot11BssConf.channels[i] = Ie.info[WFM_DOT11_COUNTRY_STRING_SIZE + i + 2];
			}
		} else
			WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_INVITATION_FAILED,
						0,
						NULL,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);

	}

	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pMsg);

	return status;
}

/******************************************************************************
 * NAME: UMAC_ProcInvtnSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for Stopping a UMAC Instance.UmacHandle should
 * be created with UMAC_Create() and started using UMAC_Start()before calling
 * this  function. UmacHandle should be created with UMAC_Create() before
 * calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcInvtnSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	return status;
}

/******************************************************************************
 * NAME: UMAC_ProcInvtnFailed
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for Stopping a UMAC Instance.UmacHandle should
 * be created with UMAC_Create() and started using UMAC_Start()before calling
 * this  function. UmacHandle should be created with UMAC_Create() before
 * calling this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcInvtnFailed(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	return status;
}

/******************************************************************************
 * NAME: UMAC_AuthRespSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for sending the authentication response as
 * success. UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this  function.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the received message.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_AuthRespSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WSM_HI_MAP_LINK_REQ *pMapLinkReq = NULL;
	uint8 linkId;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	linkId = (uint8) ((((UMAC_RX_DESC *) pMsg)->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);

	if (NULL == pUmacInstance->gVars.pMapLinkReq) {
		pUmacInstance->gVars.pMapLinkReq = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

		OS_ASSERT(pUmacInstance->gVars.pMapLinkReq);

		UMAC_MEM_STAMP_INTERNAL_BUFFER(
					pUmacInstance->gVars.pMapLinkReq,
					UMAC_START_REQ
					);

		WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pMapLinkReq);

		pMapLinkReq = (WSM_HI_MAP_LINK_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pMapLinkReq);
		CHECK_POINTER_IS_VALID(pMapLinkReq);
		OS_MemoryCopy(
				pMapLinkReq->MacAddr,
				pUmacInstance->sta[linkId].macAddr,
				WFM_MAC_ADDRESS_SIZE
				);
	}

	pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;

		pSmgmtReq->Type = SMGMT_REQ_TYPE_SEND_AUTH_RESP;

		pSmgmtReq->ReqNum = 0;

		pUmacInstance->linkId = linkId;
		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;

	} /* if(!pSmgmtReq->HandleMgmtReq) */
	pSmgmtReq->HandleMgmtReq = 0;

	UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pUmacInstance->gVars.pMapLinkReq
					);
	pUmacInstance->gVars.pMapLinkReq = NULL;

	pUmacInstance->sta[linkId].sendDisconnEvent = FALSE;

	Result = UMAC_SendAuthResp(UmacHandle, WFM_STATUS_SUCCESS, linkId, pMsg);

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, (UMAC_RX_DESC *) pMsg);

 exit_handler:
	return Result;

} /* end UMAC_AuthRespSuccess() */

/******************************************************************************
 * NAME: UMAC_AuthRespFailure
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for sending the authentication response to the
 * peer station UmacHandle should be created with UMAC_Create() before calling
 * this fucntion.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param pMsg	       - Pointer to the received message.
 * \returns uint32
 *********************************************************************************/
static uint32 UMAC_AuthRespFailure(UMAC_HANDLE UmacHandle, void *pMsg)
{

	uint32 Result = WFM_STATUS_SUCCESS;
	uint8 linkId;

	linkId = (uint8) ((((UMAC_RX_DESC *) pMsg)->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
	UMAC_STA_UPDATE_STATE(UmacHandle, linkId, STA_STATE_NONE);

	Result = UMAC_SendAuthResp(UmacHandle, WFM_STATUS_FAILURE, linkId, pMsg);

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, (UMAC_RX_DESC *) pMsg);

	return Result;
} /* end UMAC_AuthRespFailure() */

/******************************************************************************
 * NAME: UMAC_SendAuthResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for sending the authentication response to the
 * peer station UmacHandle should be created with UMAC_Create() before calling
 * this fucntion.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \param status	  - Status to be inserted in the Auth response.
 * \param linkId	  - Link Id of the STA
 * \returns uint32
 ******************************************************************************/
static uint32 UMAC_SendAuthResp(UMAC_HANDLE UmacHandle,
				uint16 status,
				uint8 linkId, void *pMsg)
{

	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_MGMT_FRAME *pAuthFrame = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_MGMT_FRAME *pRcvdAuthFrame;
	UMAC_RX_DESC *pRxDesc;
	uint16 FrameBodyLen = 0;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_SendAuthResp()\n");

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);
	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_AUTH_REQ);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	pAuthFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pAuthFrame);

	OS_ASSERT(linkId < pUmacInstance->gVars.p.maxNumClientSupport);

	pRxDesc = (UMAC_RX_DESC *)pMsg;
	pRcvdAuthFrame = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDesc);

	/*
	   Creating the WLAN Header
	*/
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_AUTH,
					&pAuthFrame->dot11hdr,
					pRcvdAuthFrame->dot11hdr.Address2,
					pUmacInstance->MacAddress,
					0,
					0
					);

#if 0
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_AUTH,
					&pAuthFrame->dot11hdr,
					pUmacInstance->sta[linkId].macAddr,
					pUmacInstance->MacAddress,
					0,
					0
					);
#endif

	/*
	   Forming the Authentication Frame
	*/
	FrameBodyLen = WFM_UMAC_CreateAuthFrame(
						UmacHandle,
						pAuthFrame->FrameBody,
						UMAC_OPEN_SYSTEM_AUTH,
						2, /* Transaction number */
						NULL, /* pRxAuthFrame */
						status
						);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendAuthResp() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->OurAllBasicSupportedRates_Abgn);
	pTxDesc->QueueId = WFM_AC_VO;
	pTxDesc->More = 0;

	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
				UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL
				);


	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					linkId
					);

	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, linkId);

		UMAC_STA_UPDATE_STATE(UmacHandle, linkId, STA_STATE_NONE);

	} else
		UMAC_STA_UPDATE_STATE(
					UmacHandle,
					linkId,
					STA_STATE_AUTHENTICATED
					);

	if (WFM_STATUS_SUCCESS == status) {
		/*Starting the UMAC timer */
		pUmacInstance->sta[linkId].UmacConHandle.linkId = linkId;
		pUmacInstance->sta[linkId].UmacConHandle.UmacHandle = UmacHandle;
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[linkId]);
		OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[linkId],
			UMAC_CLIENT_CONNECTION_TIMEOUT,
			(UMAC_CONNECTION_HANDLE *)&pUmacInstance->sta[linkId].UmacConHandle
			);
	}

exit_handler:
	return Result;
} /* end UMAC_SendAuthResp() */

/******************************************************************************
 * NAME:UMAC_ProcAssocReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for processing the incoming association request.
 * UmacHandle should be created with UMAC_Create() before calling this function.
 * \param UmacHandle     - Handle to UMAC Instance.
 * \param pMsg		 - Pointer to the received message.
 * \returns uint32
******************************************************************************/
static uint32 UMAC_ProcAssocReq(UMAC_HANDLE UmacHandle, void *pMsg)
{
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	WFM_UMAC_ASSOC_REQ_BODY *pAssocReq = NULL;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	IeElement Ie;
	uint32 basicRates = 0;
	uint32 supportedRates = 0;
	uint32 tBasicRates = 0;
	uint32 tSupportedRates = 0;
	uint32 Result;
	uint16 staCapability;
	uint16 IELength;
	uint16 status = WFM_UMAC_SC_SUCCESSFUL;
	uint8 *IeElementStart;
	uint8 linkId, SubType;
	WPS_IeElement attrib;
	IeElement Ielement = { 0 };
	WFM_OUI_INFO OuiInfo = {
		WMM_OUI,
		WMM_IE_ELEMENT_TYPE,
		WFM_IE_INFORMATION_ELEMENT_SUB_TYPE
	};
	SMGMT_REQ_PARAMS *pSmgmtReq;
	WSM_HI_UPDATE_IE_REQ *pUpdateIeReq = NULL;
	WFM_UMAC_ERP_IE *pUpdateERP = NULL;

	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
	UMAC_STA_UPDATE_STATE(UmacHandle, linkId, STA_STATE_ASSOCIATING);
	pAssocReq = (WFM_UMAC_ASSOC_REQ_BODY *) pUmacMgmt->FrameBody;
	staCapability = pAssocReq->Capability;
	/*Check whether the frame is assoc req or reassoc req*/
	SubType = (uint8) ((pUmacMgmt->dot11hdr.FrameControl >> 4) & 0x000F);

	if (SubType == WFM_UMAC_MGMT_REASSOC_REQ) {
		IELength = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor)
			     - (sizeof(WFM_UMAC_FRAME_HDR)
				+ sizeof(pAssocReq->Capability)
				+ sizeof(pAssocReq->ListenInterval)
				+ WFM_UMAC_MAC_ADDRESS_SIZE /*Current AP Address*/
			     ));
		IeElementStart = (uint8 *) &pAssocReq->ListenInterval + sizeof(pAssocReq->ListenInterval) + WFM_UMAC_MAC_ADDRESS_SIZE;
		OS_MemoryCopy(&pUmacInstance->sta[linkId].currentAPAddress[0],
				((uint8 *)pAssocReq + 4),
				WFM_MAC_ADDRESS_SIZE);

	} else {
		IELength = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor)
			     - (sizeof(WFM_UMAC_FRAME_HDR)
				+ sizeof(pAssocReq->Capability)
				+ sizeof(pAssocReq->ListenInterval)
			     ));
		IeElementStart = (uint8 *) &pAssocReq->ListenInterval + sizeof(pAssocReq->ListenInterval);
	}

	/* Traverse IEs and look for supported rates IE */
	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_IE_ELEMENT_SUPPORTED_RATES, &Ie) == TRUE) {
		/* supported rate IE found */
		if (Ie.size != 0) {
			/*Converting the Basic Supported Rates to WSM Bitmask */
			WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate(Ie.size, Ie.info, &supportedRates, &basicRates);
			pUmacInstance->sta[linkId].allSupportedRates = supportedRates;
		} /* if(Ie.size != 0) */
	} /* if( WFM_UMAC_ProcIeElements - UMAC_IE_ELEMENT_SUPPORTED_RATES */

	/* Traverse IEs and look for extended supported rates IE */
	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES, &Ie) == TRUE) {
		/* extended supported rate IE found */
		if (Ie.size != 0) {
			/*
			  Converting the Extended Supported Rates to WSM
			  Bitmask
			*/
			WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate(
							Ie.size,
							Ie.info,
							&tSupportedRates,
							&tBasicRates
							);
			pUmacInstance->sta[linkId].allSupportedRates |= tSupportedRates;

			/* Store device capability of P2P client */

			/* Store device info of P2P client */
		}
	}

	basicRates |= tBasicRates;
	supportedRates |= tSupportedRates;

	if (pUmacInstance->networkTypeInUse == NETWORK_TYPE_11BG) {
		if (!(supportedRates & (~(UMAC_RATE_11B)))) {
			pUmacInstance->sta[linkId].Is11bUser = TRUE;
			pUmacInstance->ErpInformation |= WFM_UMAC_ERP_IE_NON_ERP_PRESENT;
			pUmacInstance->ErpInformation |= WFM_UMAC_ERP_IE_USE_PROTECTION;
			if ((staCapability & WFM_UMAC_CAPABILITY_SHORT_PREAMBLE) != WFM_UMAC_CAPABILITY_SHORT_PREAMBLE) {
				pUmacInstance->sta[linkId].IsLongPreamble = TRUE;
				pUmacInstance->ErpInformation |= WFM_UMAC_ERP_IE_BARKER_PREAMBLE_MODE;
			}
		}
	}

	if ((pUmacInstance->dot11BssConf.basicRates & supportedRates) != pUmacInstance->dot11BssConf.basicRates) {
		status = WFM_UMAC_SC_UNSUPPORTED_BASIC_RATES;
		/*WfmEvent = UMAC_ASSOC_RESP_FAILED; */
	}

	if (pUmacInstance->dot11BssConf.vdrSpecificCapInfo & WFM_BSS_INFO_FLAGS_SUPP_WPA2) {
		if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_IE_ELEMENT_RSN, &Ie) == FALSE)
			status = WFM_UMAC_SC_UNSPECIFIED;
	} else if (pUmacInstance->dot11BssConf.vdrSpecificCapInfo & WFM_BSS_INFO_FLAGS_SUPP_WPA) {
		WFM_OUI_INFO OuiInfo = {
			WPA_OUI,
			0x01,
			WPA_IE_PARAMETER_ELEMENT_SUB_TYPE
		};
		WFM_ProcVendorSpecificIeElementsWPA(IeElementStart, IELength, &OuiInfo, &Ielement);
		if (Ielement.size == 0)
			status = WFM_UMAC_SC_UNSPECIFIED;
	} else if ((pUmacInstance->dot11BssConf.capabilityInfo & WFM_UMAC_CAPABILITY_PRIVACY)
		!= (staCapability & WFM_UMAC_CAPABILITY_PRIVACY)) {
			status = WFM_UMAC_SC_UNSUPPORTED_CAPABILITIES;
			/*WfmEvent = UMAC_ASSOC_RESP_FAILED; */
	}
#if 0
	 else if ((pUmacInstance->dot11BssConf.capabilityInfo & WFM_UMAC_CAPABILITY_PRIVACY)
		   != (staCapability & WFM_UMAC_CAPABILITY_PRIVACY)) {
		status = WFM_UMAC_SC_UNSUPPORTED_CAPABILITIES;
		/*WfmEvent = UMAC_ASSOC_RESP_FAILED; */
	}
#endif
	else
		status = WFM_UMAC_SC_SUCCESSFUL;

	pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;
	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;
		pSmgmtReq->Type = SMGMT_REQ_TYPE_SEND_ASSOC_RESP;
		pSmgmtReq->ReqNum = 0;
		pUmacInstance->linkId = linkId;
		pUmacInstance->ErpDssCckRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_DSSS_CCK_RATES_SUPPORTED);
		pUmacInstance->ErpOfdmRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_OFDM_RATES_SUPPORTED);
		pUmacInstance->HtOfdmRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_HT_OFDM_RATES_SUPPORTED);
		if (pUmacInstance->HtOfdmRates)
			pUmacInstance->CurrentModeOpRates = pUmacInstance->HtOfdmRates;
		else if (pUmacInstance->ErpOfdmRates)
			pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpOfdmRates;
		else if (pUmacInstance->ErpDssCckRates)
			pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpDssCckRates;
		else
			pUmacInstance->CurrentModeOpRates = pUmacInstance->AllSupportedRates_WithAP_Abgn;
		if (status != WFM_UMAC_SC_SUCCESSFUL)
			pSmgmtReq->ReqNum = 1;
		else
			UMAC_UpdateTxRatesBitmap(pUmacInstance, linkId);

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);
		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;
	} /* if(!pSmgmtReq->HandleMgmtReq) */
	pSmgmtReq->HandleMgmtReq = 0;

	OS_LOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
	if (pUmacInstance->ErpInformation != pUmacInstance->ErpInfoPrevious) {
		pUmacInstance->ErpIeToBeUpdated = TRUE;
		if ((pUmacInstance->ErpInformation & WFM_UMAC_ERP_IE_USE_PROTECTION) == WFM_UMAC_ERP_IE_USE_PROTECTION)
			pUmacInstance->ErpUseCtsToSelf = TRUE;
		OS_UNLOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_ERP_IE, NULL);
	} else {
		pUmacInstance->ErpIeToBeUpdated = FALSE;
		OS_UNLOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
	}

	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUmacClientConnectionTimer[linkId]);
	/* pUmacInstance->sta[linkId].listenInterval = pAssocReq->ListenInterval; */
	pUmacInstance->sta[linkId].listenInterval = (pAssocReq->ListenInterval >= 155) ? 155 : pAssocReq->ListenInterval;

	WFM_UMAC_ProcVendorSpecificIeElements(IeElementStart, IELength, &OuiInfo, &Ielement);
	if (Ielement.size) {
		WFM_WMM_PARAM_ELEMENT *pwmmParamElement = (WFM_WMM_PARAM_ELEMENT *) Ielement.info;
		pUmacInstance->sta[linkId].qosInfo = pwmmParamElement->QoSInfo;
		pUmacInstance->sta[linkId].isWMMEnabled = TRUE;
	}			/*if(Ielement.size ) */
	pUmacInstance->sta[linkId].currentRateIndex = (uint8)WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->sta[linkId].allSupportedRates);
	if ((WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_P2P_IE_ELEMENT_ID, &Ie) == TRUE)
		&& (Ie.size)) {
			if ((WFM_UMAC_ProcWPSIE(Ie.info, Ie.size, UMAC_P2P_IE_ATTRIB_CAPABILITY, &attrib) == TRUE)
				&& (attrib.size)) {
					/* For Feature Use */
			}
			if ((WFM_UMAC_ProcWPSIE(Ie.info, Ie.size, UMAC_P2P_IE_ATTRIB_DEVICE_INFO, &attrib) == TRUE)
				&& (attrib.size)) {
					/* For Feature Use */
			}
	}
	Result = UMAC_SendAssocResp(UmacHandle, status, linkId, SubType);

	if (status == WFM_UMAC_SC_SUCCESSFUL) {
		UMAC_EVT_DATA_CONNECT_INFO connectInfo;
		/*Fill the ASSOCIATION INFORMATION parameters*/
		pUmacInstance->sta[linkId].capabilities = pAssocReq->Capability ;
		/* pUmacInstance->sta[linkId].listenInterval = pAssocReq->ListenInterval; */
		pUmacInstance->sta[linkId].listenInterval = (pAssocReq->ListenInterval >= 155) ? 155 : pAssocReq->ListenInterval;
		pUmacInstance->sta[linkId].AssocReqIELen = IELength;
		OS_MemoryCopy(&pUmacInstance->sta[linkId].AssocReqIE[0],
			IeElementStart,
			pUmacInstance->sta[linkId].AssocReqIELen);
		connectInfo.linkId = linkId;
		OS_MemoryCopy(
				connectInfo.stationMacAddr,
				pUmacInstance->sta[linkId].macAddr,
				WFM_MAC_ADDRESS_SIZE
				);
		connectInfo.PWencStatus = (uint8) pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost;

		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_CONNECTED,
					sizeof(connectInfo),
					(uint8 *) &connectInfo,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
	}

	/* Unlink the STA if status is not SUCCESS */
	/*UMAC_ProcUnLinkSTA( UmacHandle,pMsg ); */
	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, (UMAC_RX_DESC *) pMsg);

exit_handler:
	return Result;
} /* end UMAC_ProcAssocReq() */

/******************************************************************************
 * NAME:UMAC_SendAssocResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for sending association response UmacHandle
 * should be created with UMAC_Create() and started using UMAC_Start() before
 * calling this  function. UmacHandle should be created with UMAC_Create()
 * before calling this function.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param status	- Status to be filled into the association response.
 * \param linkId	- Link Id of the STA
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_SendAssocResp(UMAC_HANDLE UmacHandle,
				 uint16 status,
				 uint8 linkId,
				 uint8 SubType)
{

	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	WFM_UMAC_MGMT_FRAME *pAssocRespFrame = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 subType;
	uint8 i = 0;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_SendAssocResp()\n");

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[linkId]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_SendAssocResp() pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
	WFM_UMAC_DBG_PRINT_TX_DESC();

	pAssocRespFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pAssocRespFrame);

	/*
	   Creating the WLAN Header
	*/
	if (SubType == WFM_UMAC_MGMT_REASSOC_REQ)
		subType = WFM_UMAC_MGMT_REASSOC_RSP;
	else
		subType = WFM_UMAC_MGMT_ASSOC_RSP;
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					subType,
					&pAssocRespFrame->dot11hdr,
					pUmacInstance->sta[linkId].macAddr,
					pUmacInstance->MacAddress,
					0,
					0
					);

	FrameBodyLen = WFM_UMAC_CreateAssocRsp(
						UmacHandle,
						pAssocRespFrame->FrameBody,
						linkId,
						status
						);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendAssocResp() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->sta[linkId].allSupportedRates);
	pTxDesc->QueueId = WFM_AC_VO;	/* WFM_AC_VO ; */
	pTxDesc->More = 0;

	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
				UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL
				);

	LOG_BYTES(DBG_WFM_ASSOC, "ASSOC_RESP: ", (uint8 *) pAssocFrame, GET_TX_DESC_FRAME_LENGTH(pTxDesc), GET_TX_DESC_FRAME_LENGTH(pTxDesc)
	    );


	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					linkId
					);

	if (Result != WFM_STATUS_SUCCESS) {
		/* TBD */
		/*UMAC_QUEUE_PENDING_TO_SELF_MSG
		   (
		   UmacHandle      ,
		   UMAC_ASSOC_RESP_SUCCESS,
		   pMsg
		   ) ; */

	} else {
		if (WFM_UMAC_SC_SUCCESSFUL == status) {
			UMAC_STA_UPDATE_STATE(
						UmacHandle,
						linkId,
						STA_STATE_ASSOCIATED
						);
			pUmacInstance->sta[linkId].sendDisconnEvent = TRUE;

			pUmacInstance->sta[linkId].UmacConHandle.linkId = linkId;
			pUmacInstance->sta[linkId].UmacConHandle.UmacHandle = UmacHandle;

			if (pUmacInstance->sta[linkId].isWMMEnabled == FALSE)
				pUmacInstance->sta[DEFAULT_LINK_ID].isWMMEnabled = FALSE;

			/*
			OS_START_TIMER(
				pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId],
				UMAC_CLIENT_INACTIVITY_TIMEOUT,
				(UMAC_HANDLE) &(pUmacInstance->sta[linkId].UmacConHandle)
				);*/
		} else {
			UMAC_STA_UPDATE_STATE(UmacHandle, linkId, STA_STATE_NONE);
			/*WFM_SCHEDULE_TO_SELF(UmacHandle,UMAC_DE_AUTHE, NULL); */
		}
		OS_START_TIMER(
				pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId],
				UMAC_CLIENT_INACTIVITY_TIMEOUT,
				(UMAC_HANDLE) &(pUmacInstance->sta[linkId].UmacConHandle)
				);
	}

exit_handler:
	return Result;
} /* end UMAC_SendAssocResp() */

/******************************************************************************
 * NAME:UMAC_SendGoNegoResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for sending GO negotiation response UmacHandle
 * should be created with UMAC_Create() and started using UMAC_Start() before
 * calling this  function. UmacHandle should be created with UMAC_Create()
 * before calling this function.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param status     - Status to be filled into the GO negotiation confirmation
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_SendGoNegoResp(UMAC_HANDLE UmacHandle, uint16 status)
{

	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	WFM_UMAC_MGMT_FRAME *pGONegoResp = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	uint8 linkId = 0;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_SendGoNegoResp()\n");

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_GO_NEG_RESP);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_SendGoNegoResp pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
	WFM_UMAC_DBG_PRINT_TX_DESC();

	pGONegoResp = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pGONegoResp);

	/* OS_MemoryCopy(
				pUmacInstance->RemoteMacAdd,
				pUmacInstance->p2pGrpFormtn.peerAddr,
				WFM_MAC_ADDRESS_SIZE
				); */

	/*
	   Creating the WLAN Header
	*/
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_ACTION,
					&pGONegoResp->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					0
					);

	FrameBodyLen = WFM_UMAC_CreateGoNegoReqResp(
							UmacHandle,
							pGONegoResp->FrameBody,
							FALSE,
							(uint8) status
							);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendGoNegoResp() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;
	pTxDesc->QueueId = 0;	/* WFM_AC_VO ; */
	pTxDesc->More = 0;

	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
				UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL);

	if (P2P_STATUS_SUCCESS != status) {
		/* Inform the host that GO formation failed */
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_GROUP_FORMATION_FAILED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);

		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");
		pUmacInstance->operatingMode = OPERATING_MODE_P2P_DEVICE;
		goto exit_handler;
	}

	LOG_BYTES(DBG_WFM_UMAC, "UMAC_SendGoNegoResp: ", (uint8 *) pGONegoResp, GET_TX_DESC_FRAME_LENGTH(pTxDesc), GET_TX_DESC_FRAME_LENGTH(pTxDesc));

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					linkId
					);

	/*UMAC_SET_STATE(UmacHandle,UMAC_GROUP_FORMED) ; */
	/*LOG_EVENT( DBG_WFM_UMAC, "STATE   = UMAC_GROUP_FORMED\n" ) ; */

	if (Result != WFM_STATUS_SUCCESS) {

		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, linkId);
		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");

	} /* if(Result != WFM_STATUS_SUCCESS) */

exit_handler:

	return Result;

} /* end UMAC_SendGoNegoResp() */

/******************************************************************************
 * NAME:UMAC_SendGoNegoCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for sending GO negotiation confirmation
 * UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this  function.
 * UmacHandle should be created with UMAC_Create() before calling this function.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param status     - Status to be filled into the GO negotiation confirmation
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_SendGoNegoCnf(UMAC_HANDLE UmacHandle, uint16 status)
{

	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	WFM_UMAC_MGMT_FRAME *pGONegoCnf = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	uint8 linkId = 0;
	UMAC_EVT_P2P_DATA_GROUP_FORMATION_INFO grpFormatnInfo;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_SendGoNegoCnf()\n");

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_GO_NEG_CNF);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_SendGoNegoCnf pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
	WFM_UMAC_DBG_PRINT_TX_DESC();

	pGONegoCnf = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pGONegoCnf);

	/* OS_MemoryCopy(
				pUmacInstance->RemoteMacAdd,
				pUmacInstance->p2pGrpFormtn.peerAddr,
				WFM_MAC_ADDRESS_SIZE
				); */

	/*
	   Creating the WLAN Header
	*/
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_ACTION,
					&pGONegoCnf->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					0
					);

#if 0
	/*
	  Peer station's addr should be used in Addr 1 and Addr 3 for GO nego
	  request and Go Nego cnf frames
	*/
	/* Duplication to be removed */
	D0_ADDR_COPY(
			pGONegoCnf->dot11hdr.Address1,
			pUmacInstance->p2pGrpFormtn.peerAddr
			);
	D0_ADDR_COPY(
			pGONegoCnf->dot11hdr.Address3,
			pUmacInstance->p2pGrpFormtn.peerAddr
			);
#endif

	FrameBodyLen = WFM_UMAC_CreateGoNegoCnf(
						UmacHandle,
						pGONegoCnf->FrameBody,
						(uint8) status
						);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendGoNegoCnf() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;
	pTxDesc->QueueId = 0;	/* WFM_AC_VO ; */
	pTxDesc->More = 0;

	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL
				);

	if (P2P_STATUS_SUCCESS == status) {
		UMAC_SET_STATE(UmacHandle, UMAC_GROUP_FORMED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_GROUP_FORMED\n");
	} else {
		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");
	}

	LOG_BYTES(DBG_WFM_UMAC, "GO Nego Cnf: ", (uint8 *) pGONegoCnf, GET_TX_DESC_FRAME_LENGTH(pTxDesc), GET_TX_DESC_FRAME_LENGTH(pTxDesc));

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					linkId
					);

	UMAC_SET_STATE(UmacHandle, UMAC_GROUP_FORMED);
	LOG_EVENT(DBG_WFM_UMAC, "STATE   = UMAC_GROUP_FORMED\n");

	if (pUmacInstance->operatingMode == OPERATING_MODE_GO)
		grpFormatnInfo.role = 1;
	else
		grpFormatnInfo.role = 0;


	OS_MemoryCopy(
			grpFormatnInfo.p2pGroupBssid,
			pUmacInstance->RemoteMacAdd,
			WFM_MAC_ADDRESS_SIZE
			);

	WFM_UMAC_2_UL_SendEvent(
				UmacHandle,
				WFM_EVT_GROUP_FORMATION_SUCCEEDED,
				sizeof(grpFormatnInfo),
				(uint8 *) &grpFormatnInfo,
				WFM_UMAC_EVT_NOT_RETRANSMIT
				);

	if (pUmacInstance->operatingMode == OPERATING_MODE_GO) {
		/* Start Beaconing, since we are GO now */
		pUmacInstance->dot11BssConf.band = PHY_BAND_2G;
		pUmacInstance->dot11BssConf.mode = OPERATING_MODE_GO;
		pUmacInstance->dot11BssConf.channelNum = pUmacInstance->p2pGrpFormtn.opChannel;

		/*
		   11 -> sizeof(CTWindow + beaconInterval + DTIMPeriod
			+ preambleType + probeDelay)
		*/
		OS_MemoryCopy(
				&pUmacInstance->dot11BssConf.CTWindow,
				&pUmacInstance->p2pGrpFormtn.CTWindow,
				11
				);
		pUmacInstance->dot11BssConf.basicRates = pUmacInstance->p2pGrpFormtn.basicRateSet;

		WFM_SCHEDULE_TO_SELF(
					UmacHandle,
					UMAC_START_AP,
					(void *)&pUmacInstance->dot11BssConf
					);
	} else if (pUmacInstance->operatingMode == OPERATING_MODE_P2P_CLIENT) {
		UMAC_SET_STATE(UmacHandle, UMAC_JOINED);
	}

	if (Result != WFM_STATUS_SUCCESS) {

		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, linkId);
		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");
	} /* if(Result != WFM_STATUS_SUCCESS) */

exit_handler:
	return Result;
} /* end UMAC_SendGoNegoCnf() */

/******************************************************************************
 * NAME:UMAC_SendInvtnResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for sending Invitation response frame
 * UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this  function. UmacHandle should be created with
 * UMAC_Create() before calling this function.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param status     - Status to be filled into the GO negotiation confirmation
  * \returns uint32
 *****************************************************************************/
static uint32 UMAC_SendInvtnResp(UMAC_HANDLE UmacHandle, uint16 status)
{

	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	WFM_UMAC_MGMT_FRAME *pGONegoCnf = NULL;
	UMAC_TX_DESC *pTxDesc = NULL;
	uint8 linkId = 0;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_SendInvtnResp()\n");

	UMAC_SET_STATE_PROCESSING(UmacHandle);

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_GO_INVT_RESP);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_SendInvtnResp pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);
	WFM_UMAC_DBG_PRINT_TX_DESC();

	pGONegoCnf = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pGONegoCnf);

	/*
	   Creating the WLAN Header
	*/
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_ACTION,
					&pGONegoCnf->dot11hdr,
					pUmacInstance->RemoteMacAdd,
					pUmacInstance->MacAddress,
					0,
					0
					);
	/*
	   Peer station's addr should be used in Addr 1 and Addr 3 for
	   GO nego request and Go Nego cnf frames
	*/
	/* Duplication to be removed */
	D0_ADDR_COPY(
			pGONegoCnf->dot11hdr.Address1,
			pUmacInstance->RemoteMacAdd
			);
	D0_ADDR_COPY(
			pGONegoCnf->dot11hdr.Address3,
			pUmacInstance->RemoteMacAdd
			);

	FrameBodyLen = WFM_UMAC_CreateInvtnReqResp(
							UmacHandle,
							pGONegoCnf->FrameBody,
							0, /* isReq is 0 */
							(uint8) status
							);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendInvtnResp() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;
	pTxDesc->QueueId = 0;	/* WFM_AC_VO ; */
	pTxDesc->More = 0;

	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
				UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL
				);

	if (P2P_STATUS_SUCCESS == status) {
		UMAC_SET_STATE(UmacHandle, UMAC_BSS_STARTED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_BSS_STARTED\n");
	} else {
		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");
	}

	LOG_BYTES(DBG_WFM_UMAC, "GO Nego Cnf: ", (uint8 *) pGONegoCnf, GET_TX_DESC_FRAME_LENGTH(pTxDesc), GET_TX_DESC_FRAME_LENGTH(pTxDesc));

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					linkId
					);

	if (Result != WFM_STATUS_SUCCESS) {

		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, linkId);

		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");
	} /* if(Result != WFM_STATUS_SUCCESS) */

exit_handler:
	return Result;
} /* end UMAC_SendInvtnResp() */

/******************************************************************************
 * NAME: UMAC_ResetSeqNumCounter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible to reset sequence number counter for the
 * received frames. This is worarround for Wi-Fi Direct test case 6.1.10, as the
 * paket injector from test engine is missbehaving.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ResetSeqNumCounter(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	WSM_HI_MAP_LINK_REQ *pMapLinkReq = NULL;
	uint8 linkId = 0;

	linkId = (uint8) ((((UMAC_RX_DESC *) pMsg)->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);

	if (NULL == pUmacInstance->gVars.pMapLinkReq) {
		pUmacInstance->gVars.pMapLinkReq = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

		OS_ASSERT(pUmacInstance->gVars.pMapLinkReq);

		pMapLinkReq = (WSM_HI_MAP_LINK_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pMapLinkReq);

		OS_MemoryCopy(
				pMapLinkReq->MacAddr,
				pUmacInstance->sta[linkId].macAddr,
				WFM_MAC_ADDRESS_SIZE
				);
	}

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;

		pSmgmtReq->Type = SMGMT_REQ_TYPE_RESET_SEQ_NUM_COUNTER;
		pSmgmtReq->operation.reset_seq_num_counter_params.linkId = linkId;

		pSmgmtReq->ReqNum = 0;

		status = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (status != WFM_STATUS_SUCCESS)
			goto exit_handler;


	} /* if(!pSmgmtReq->HandleMgmtReq) */
	pSmgmtReq->HandleMgmtReq = 0;

	UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pUmacInstance->gVars.pMapLinkReq
					);
	pUmacInstance->gVars.pMapLinkReq = NULL;

	OS_MemoryReset(&pUmacInstance->sta[linkId], sizeof(UMAC_STA_DB));
	pUmacInstance->sta[linkId].PsState = STA_PS_STATE_AWAKE;

	/*Release the RxDesc and RxBuffer */
	UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[DEFAULT_LINK_ID]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

 exit_handler:
	return status;
}  /* end UMAC_ResetSeqNumCounter() */


/******************************************************************************
 * NAME: UMAC_ProcStartAP
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for configuring the UMAC in miniAP, P2P GO
 * like mode. UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start() before calling this  function.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcStartAP(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	WSM_HI_START_REQ *pStartReq = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_OID_802_11_START_AP *pStartAP;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcStartAP()\n");

	pStartAP = (UMAC_OID_802_11_START_AP *) pMsg;
	CHECK_POINTER_IS_VALID(pStartAP);

	if (pStartAP->beaconInterval != 0) {
		pUmacInstance->dot11BssConf.beaconInterval = (uint16) pStartAP->beaconInterval;
		pUmacInstance->beaconInterval = (uint16) pStartAP->beaconInterval;
	}
	if (pStartAP->DTIMperiod != 0) {
		pUmacInstance->TIMparams.DTIMperiod = pStartAP->DTIMperiod;
		pUmacInstance->dot11BssConf.DTIMperiod = pStartAP->DTIMperiod;
	}

	pUmacInstance->SsidLength = pStartAP->ssidLength;
	OS_MemoryCopy(
			pUmacInstance->Ssid,
			pStartAP->ssid,
			pStartAP->ssidLength
			);
	pUmacInstance->AllBasicSupportedRates_WithAP_Abgn = pStartAP->basicRates;
	pUmacInstance->Band = pStartAP->band;

	if (pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost == WFM_ENC_STATUS_NO_ENCRYPTION)
		pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_NO_ENC_SUPPORTED;
	else if (pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost & WFM_ENC_STATUS_TKIP_SUPPORTED)
		pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_TKIP_ENC_SUPPORTED;
	else if (pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost & WFM_ENC_STATUS_AES_SUPPORTED)
		pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_ENC_STATUS_AES_SUPPORTED;


	switch (pStartAP->networkType) {
	case NETWORK_TYPE_11B:
		pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_DSSS_CCK_RATES_SUPPORTED;
		break;

	case NETWORK_TYPE_11A:
		pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED;
		break;

	case NETWORK_TYPE_11BG:
		pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_DSSS_CCK_RATES_SUPPORTED | WFM_ERP_OFDM_RATES_SUPPORTED;
		break;

	case NETWORK_TYPE_11BGN:
		pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_DSSS_CCK_RATES_SUPPORTED | WFM_ERP_OFDM_RATES_SUPPORTED | WFM_HT_OFDM_RATES_SUPPORTED;
		break;

	case NETWORK_TYPE_11AN:
		pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED | WFM_HT_OFDM_RATES_SUPPORTED;
		break;

	default:
		pStartAP->networkType = NETWORK_TYPE_11BG;
		pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_DSSS_CCK_RATES_SUPPORTED | WFM_ERP_OFDM_RATES_SUPPORTED;
		break;
	} /* switch(pStartAP->networkType) */


	/* Use own MAC address as BSSID */
	OS_MemoryCopy(
			pUmacInstance->RemoteMacAdd,
			pUmacInstance->MacAddress,
			WFM_MAC_ADDRESS_SIZE
			);

	pUmacInstance->networkTypeInUse = pStartAP->networkType;
	pUmacInstance->operatingMode = pStartAP->mode ? OPERATING_MODE_GO : OPERATING_MODE_AP;


	if ((pUmacInstance->gVars.p.useP2P) && (pUmacInstance->operatingMode == OPERATING_MODE_GO)) {
		pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED;
		/* Updating rates for linkID 0 to prevent fallback on CCK rates */
		pUmacInstance->ErpDssCckRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_DSSS_CCK_RATES_SUPPORTED);
		pUmacInstance->ErpOfdmRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_OFDM_RATES_SUPPORTED);
		pUmacInstance->HtOfdmRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_HT_OFDM_RATES_SUPPORTED);
		if (pUmacInstance->HtOfdmRates)
			pUmacInstance->CurrentModeOpRates = pUmacInstance->HtOfdmRates;
		else if (pUmacInstance->ErpOfdmRates)
			pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpOfdmRates;
		else if (pUmacInstance->ErpDssCckRates)
			pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpDssCckRates;
		else
			pUmacInstance->CurrentModeOpRates = pUmacInstance->AllSupportedRates_WithAP_Abgn;
		UMAC_UpdateTxRatesBitmap(pUmacInstance, pUmacInstance->linkId);
		pUmacInstance->sta[0].allSupportedRates = WFM_ERP_OFDM_RATES_SUPPORTED;
	}

	pUmacInstance->CurrentModeOpRates = pUmacInstance->AllSupportedRates_WithAP_Abgn;

	pUmacInstance->QosPrio2Queue[WFM_802_1D_UP_BK] = WFM_ACI_BK;
	pUmacInstance->QosPrio2Queue[WFM_802_1D_UP_RESERVED] = WFM_ACI_BK;

	pUmacInstance->QosPrio2Queue[WFM_802_1D_UP_BE] = WFM_ACI_BE;
	pUmacInstance->QosPrio2Queue[WFM_802_1D_UP_EE] = WFM_ACI_BE;

	pUmacInstance->QosPrio2Queue[WFM_802_1D_UP_CL] = WFM_ACI_VI;
	pUmacInstance->QosPrio2Queue[WFM_802_1D_UP_VI] = WFM_ACI_VI;

	pUmacInstance->QosPrio2Queue[WFM_802_1D_UP_VO] = WFM_ACI_VO;
	pUmacInstance->QosPrio2Queue[WFM_802_1D_UP_NC] = WFM_ACI_VO;

	pUmacInstance->AC2DilQ[WFM_ACI_BE] = 1;
	pUmacInstance->AC2DilQ[WFM_ACI_BK] = 0;
	pUmacInstance->AC2DilQ[WFM_ACI_VI] = 2;
	pUmacInstance->AC2DilQ[WFM_ACI_VO] = 3;
	if (pStartAP->preambleType)
		pUmacInstance->dot11BssConf.capabilityInfo |= WFM_UMAC_CAPABILITY_SHORT_PREAMBLE;

	if (!pStartAP->channelNum)
		pUmacInstance->autoChanSel = TRUE;
	/* Change for Auto Channel Selection */
	if (pUmacInstance->autoChanSel  && !pUmacInstance->IsInternalScan) {
		WFM_BSS_LIST_SCAN *pbssListScan = (WFM_BSS_LIST_SCAN *) &pUmacInstance->InternalScanMsg[0];
		pUmacInstance->IsInternalScan = TRUE;
		if (pUmacInstance->Band == PHY_BAND_2G) {
			uint8 i = 0, channel[3] = {1, 6, 11};
			pbssListScan->Flags = WFM_BSS_LIST_SCAN_2_4G_BAND;
			pbssListScan->NumberOfChannels_2_4Ghz = 3;
			for (i = 0; i < pbssListScan->NumberOfChannels_2_4Ghz; i++) {
				pbssListScan->Channels[i].ChannelNum = channel[i];
				pbssListScan->Channels[i].MinChannelTime = WFM_UMAC_ACTIVE_SCAN_MIN_CHANNEL_TIME;
				pbssListScan->Channels[i].MaxChannelTime =  WFM_UMAC_ACTIVE_SCAN_MAX_CHANNEL_TIME;
				pbssListScan->Channels[i].TxPowerLevel = 0;
			}
		} else
			pbssListScan->Flags = WFM_BSS_LIST_SCAN_5G_BAND;
		pbssListScan->NumOfSSIDs = 0;
		pbssListScan->Ssids[0].SsidLength = 0;
		pbssListScan->NumOfProbeRequests = 2;
	}
	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (!pSmgmtReq->HandleMgmtReq)
		pUmacInstance->Channelnumber = pStartAP->channelNum;
	else if (pUmacInstance->autoChanSel && pSmgmtReq->HandleMgmtReq)
		pStartAP->channelNum = pUmacInstance->Channelnumber;
	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;

		pSmgmtReq->Type = SMGMT_REQ_TYPE_START_AP;

		pSmgmtReq->ReqNum = 0;

		status = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (status != WFM_STATUS_SUCCESS)
			goto exit_handler;


	} /* if(!pSmgmtReq->HandleMgmtReq) */
	pSmgmtReq->HandleMgmtReq = 0;

	/* Free the Scan Tx Desc as ProcScanComplete not called in Auto Channel Select Mode */
	if (pUmacInstance->autoChanSel) {
		if (pUmacInstance->UmacScanState == UMAC_SCANNING) {
			uint32 memstatus = 0;
			pUmacInstance->UmacScanState = UMAC_READY_TO_SCAN;
			pUmacInstance->IsInternalScan = FALSE;
			LOG_EVENT(DBG_WFM_UMAC, "SCAN STATE = UMAC_READY_TO_SCAN\n");
			OS_ASSERT(pUmacInstance->gVars.pScanTxDesc);
		    WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pScanTxDesc);
			/* Freeing the internal buffer */
			memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
							UmacHandle,
							pUmacInstance->gVars.pScanTxDesc
							);
			OS_ASSERT(memstatus);
			UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pScanTxDesc);
			pUmacInstance->gVars.pScanTxDesc = NULL;
		}
		pUmacInstance->autoChanSel = FALSE;
	}
	/*
	   Initialize PS Mode to a default value of PS disabled when working
	   in AP mode
	*/
	pUmacInstance->gVars.p.PsMode.PmMode = WFM_PS_MODE_DISABLED;
	pUmacInstance->gVars.p.OprPwrMode = WFM_OPR_PWR_MODE_ACTIVE;

	OS_ASSERT(!pUmacInstance->gVars.pStartReq);

	pUmacInstance->gVars.pStartReq = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pStartReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(
					pUmacInstance->gVars.pStartReq,
					UMAC_START_REQ
					);

	WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pStartReq);

	pStartReq = (WSM_HI_START_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pStartReq);
	CHECK_POINTER_IS_VALID(pStartReq);

	OS_MemoryCopy(
			(uint8 *) pStartReq + sizeof(HI_MSG_HDR),
			pStartAP,
			sizeof(WSM_HI_START_REQ) - sizeof(HI_MSG_HDR)
			);

	UMAC_SET_STATE(UmacHandle, UMAC_BSS_STARTING);
	LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_BSS_STARTING\n");

	status = UMAC_LL_REQ_START(UmacHandle, pStartReq);

	if (status != WFM_STATUS_SUCCESS) {
		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pStartReq);

		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pStartReq
						);

		pUmacInstance->gVars.pStartReq = NULL;

		/*
		   Just going back to initialized state here.
		*/
		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");
	} /* if(status != WFM_STATUS_SUCCESS) */

 exit_handler:
	return status;
} /* end UMAC_ProcStartAP() */

/*********************************************************************************
 * NAME: UMAC_ProcStopAP
 *-------------------------------------------------------------------------------*/
/**
 * \brief
 * This function will stop the UMAC in miniAP/P2P Go and brings the UMAC to
 * initialize state.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param *pMsg	     - Message to be Processed.
 * \returns uint32
 *********************************************************************************/
static uint32 UMAC_ProcStopAP(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;

	/*if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		&& (pUmacInstance->stopAPFlag)){
		WSM_HI_BEACON_TRANSMIT_REQ *pBcnTxReq = NULL;
		pUmacInstance->gVars.pBcnTxReq = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

		OS_ASSERT(pUmacInstance->gVars.pBcnTxReq);

		pBcnTxReq = (WSM_HI_BEACON_TRANSMIT_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pBcnTxReq);
		CHECK_POINTER_IS_VALID(pBcnTxReq);

		pBcnTxReq->EnableBeaconing = FALSE;

		status = UMAC_LL_REQ_BCN_TX_REQ(UmacHandle, pBcnTxReq);

		if (status != WFM_STATUS_SUCCESS) {
			UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pUmacInstance->gVars.pBcnTxReq);
			pUmacInstance->gVars.pBcnTxReq = NULL;
		}
	} else*/
	UMAC_Stop(UmacHandle);

	return status;
}

/*********************************************************************************
 * NAME: UMAC_ProcStartAPSuccess
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is responsible for processing the START AP event from
 * firmware. UmacHandle should be created with UMAC_Create() and started using
 * UMAC_Start()before calling this  function. UmacHandle should be created with
 * UMAC_Create() before calling this fucntion.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param pMsg	       - Pointer to HI message.
 * \returns uint32
 *****************************************************************************/
static uint32 UMAC_ProcStartAPSuccess(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WSM_HI_BEACON_TRANSMIT_REQ *pBcnTxReq = NULL;
	uint32 memstatus;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT((DBG_WFM_UMAC), "UMAC_ProcStartAPSuccess()\n");

	OS_ASSERT(pUmacInstance->gVars.pStartReq);

	WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pStartReq);

	/*Freeing the internal buffer */
	memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pStartReq
						);

	OS_ASSERT(memstatus);

	UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pStartReq);

	pUmacInstance->gVars.pStartReq = NULL;

	if (pUmacInstance->operatingMode == OPERATING_MODE_GO) {
		UMAC_SET_STATE(UmacHandle, UMAC_GROUP_FORMED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_GROUP_FORMED\n");
	} else {
		UMAC_SET_STATE(UmacHandle, UMAC_BSS_STARTED);
		LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_BSS_STARTED\n");
	}

	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
		OS_ASSERT(!pUmacInstance->gVars.pBcnTxReq);

		pUmacInstance->gVars.pBcnTxReq = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

		OS_ASSERT(pUmacInstance->gVars.pBcnTxReq);

		UMAC_MEM_STAMP_INTERNAL_BUFFER(pUmacInstance->gVars.pBcnTxReq, UMAC_START_REQ);

		WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pBcnTxReq);

		pBcnTxReq = (WSM_HI_BEACON_TRANSMIT_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pBcnTxReq);
		CHECK_POINTER_IS_VALID(pBcnTxReq);

		pBcnTxReq->EnableBeaconing = TRUE;

		pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->CurrentModeOpRates);

		status = UMAC_LL_REQ_BCN_TX_REQ(UmacHandle, pBcnTxReq);

		if (status != WFM_STATUS_SUCCESS) {
			UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pBcnTxReq);

			UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pUmacInstance->gVars.pBcnTxReq);

			pUmacInstance->gVars.pBcnTxReq = NULL;

			/*
			   Just going back to initialized state here.
			*/
			UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
			LOG_EVENT(DBG_WFM_UMAC, "STATE = UMAC_INITIALIZED\n");

		} /* if(status != WFM_STATUS_SUCCESS) */
	} /* if( pUmacInstance->operatingMode>=OPERATING_MODE_AP) */

	OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pUMACCleanupTimer,
			UMAC_AP_PS_BUFFER_CLEANUP_TIMER,
			(UMAC_HANDLE) UmacHandle
			);
	return status;
} /* end UMAC_ProcStartAPSuccess() */

/******************************************************************************
 * NAME:UMAC_ProcStartAPFailed
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the START failure event from LMAC.UMAC state will be
 * changed to UMAC_INITIALIZED.The buffer allocated in UMAC_ProcStartAP() will
 * be released.All the state variables will be resetd to initial values.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pMsg	- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartAPFailed(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 memstatus;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT((DBG_WFM_UMAC), "UMAC_ProcStartAPFailed()\n\n");

	OS_ASSERT(pUmacInstance->gVars.pStartReq);

	WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pStartReq);

	/* Freeing the internal buffer */
	memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pStartReq
						);

	OS_ASSERT(memstatus);

	UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pStartReq);

	pUmacInstance->gVars.pStartReq = NULL;

	/*
	  De-Initializing First and then Initializing to bring UMAC to a
	  clean state
	*/
	Result = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_DEINIT, NULL);

	if (WFM_STATUS_PENDING == Result)
		OS_ASSERT(0);


	WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_INIT, NULL);

	WFM_UMAC_2_UL_SendEvent(
				UmacHandle,
				WFM_EVT_CONNECT_FAILED,
				0,
				NULL,
				WFM_UMAC_EVT_NOT_RETRANSMIT
				);

	return Result;
} /* end UMAC_ProcStartAPFailed() */

/******************************************************************************
 * NAME:UMAC_ProcFwdFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the data coming from a connnected STA and destined
 * for another connected STA.
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
     * ----------------------------------------------
     * |UMAC_DEVIF_TX_DATA  | UMAC_TX_DESC | Payload |
     * ----------------------------------------------
 *****************************************************************************/
static uint32 UMAC_ProcFwdFrame(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint8 linkId;
	uint32 Result = WFM_STATUS_SUCCESS;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_UMAC_FRAME_HDR *pDot11hdr;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	uint8 Type, SubType;
	uint8 AccessCategory;
	uint8 buffer[TX_DESC_QOS_HDR_SIZE];
	uint8 headerOffset = 0;
	void *pDriverInfo = NULL;
	/* We can go back as we have room for 32 bytes at the start of pMsg */
	UMAC_TX_DESC *pTxDescriptor = (UMAC_TX_DESC *) ((uint8 *) pMsg - (sizeof(UMAC_TX_DESC)
									  - sizeof(UMAC_RX_DESC))
	    );
	OS_ASSERT(pUmacInstance);

	OS_MemoryCopy(&pDriverInfo, ((uint8 *)pRxDescriptor - 4), 4);
	LOG_EVENT((DBG_WFM_UMAC), "UMAC_ProcFwdFrame()\n\n");
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);

	if (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_DOZE) {
		OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
		if ((linkId == 0) && (pUmacInstance->bitmapCtrl == 0)) {
			pUmacInstance->bitmapCtrl = 1;
			if (pUmacInstance->updateTimIeInProgress == FALSE) {
				pUmacInstance->updateTimIeInProgress = TRUE;
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
				WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);
			} else
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
		} else
			OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
	}

	UMAC_GET_PACKET_ID_AP(UmacHandle, pTxDescriptor->PacketId);

	if (pTxDescriptor->PacketId == UMAC_NUM_AP_INTERNAL_TX_BUFFERS) {
		/* LOG_EVENT(DBG_WFM_ERROR, "UMAC_ProcFwdFrame() : Transmit Queue Full\n"); */
		UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle,
					pRxDescriptor,
					pDriverInfo);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}


	/* Using default access category as Best Effort */
	/*TBD: We will check if destination station is WMM capable then send with the same AC as
	the frame is received*/
	pDot11hdr = (WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
	/* b2 and b3 constitutes Type */
	Type = (uint8) ((pDot11hdr->FrameControl >> 2) & 0x0003);
	/*b4,b5,b6 and b7 forms SubType */
	SubType = (uint8) ((pDot11hdr->FrameControl >> 4) & 0x000F);

	if ((Type == WFM_UMAC_DATA) && (SubType == WFM_UMAC_SUB_DATA_QOS)) {
		if (linkId) {
			if (pUmacInstance->sta[linkId].isWMMEnabled) {
				uint8 priority = 0;
				headerOffset = 0;
				priority = (uint8) *((uint8 *)pDot11hdr + sizeof(WFM_UMAC_FRAME_HDR)) & 0x0F;
				AccessCategory = UMAC_GetACFromPriority(priority);
			} else {
				headerOffset = 2;
				AccessCategory = WFM_ACI_BE;
			}
		} else {
			/*Broadcast*/
			if (UMAC_CheckAllSTA_WMM_Capable(UmacHandle)) {
				uint8 priority = 0;
				headerOffset = 0;
				priority = (uint8) *((uint8 *)pDot11hdr + sizeof(WFM_UMAC_FRAME_HDR)) & 0x0F;
				AccessCategory = UMAC_GetACFromPriority(priority);
			} else {
				/*All STA's are not WMM capable*/
				headerOffset = 2;
				AccessCategory = WFM_ACI_BE;
			}
		}
	} else {
		/*Non Qos packet*/
		headerOffset = 0;
		AccessCategory = WFM_ACI_BE;
	}

	/*ASSIGN_FRAME_LENGTH(pTxDescriptor, (pRxDescriptor->MsgLen - sizeof(UMAC_RX_DESC)
			    )
	    );*/
	pTxDescriptor->MsgLen = pRxDescriptor->MsgLen - sizeof(UMAC_RX_DESC) +
		sizeof(UMAC_TX_DESC) - headerOffset;


	UMAC_ModifyDot11Header(UmacHandle,
		(WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pRxDescriptor),
		(uint16) GET_RX_DESC_FRAME_LENGTH(pRxDescriptor),
		headerOffset
	    );

	/*Check alignment */
	CHECK_POINTER_FOR_ALIGNMENT(TX_DESC_GET_DOT11FRAME(pTxDescriptor));

	if (linkId)
		pTxDescriptor->MaxTxRate = pUmacInstance->sta[linkId].currentRateIndex;
	else
		pTxDescriptor->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;

	pTxDescriptor->QueueId = AccessCategory;

	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
	pTxDescriptor->More = 0;
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		pTxDescriptor->Flags |= linkId<<4;
	if (pTxDescriptor->QueueId == WFM_AC_VO)
		ASSIGN_TX_PTA_PRIORITY(pTxDescriptor, PTA_PRIORITY_VOICE);
	else if (pTxDescriptor->QueueId == WFM_AC_VI)
		ASSIGN_TX_PTA_PRIORITY(pTxDescriptor, PTA_PRIORITY_VIDEO);
	else
		ASSIGN_TX_PTA_PRIORITY(pTxDescriptor, PTA_PRIORITY_DATA);

	pTxDescriptor->Reserved = 0;
	pTxDescriptor->HtTxParameters = 0;
	pTxDescriptor->ExpireTime = WFM_UMAC_TX_EXPIRE_TIME;

	if (headerOffset) {
		OS_MemoryCopy(&buffer[0], (uint8 *)pTxDescriptor, TX_DESC_QOS_HDR_SIZE);
		OS_MemoryCopy(((uint8 *)pTxDescriptor + 2), &buffer[0], (TX_DESC_QOS_HDR_SIZE - 2));
		pTxDescriptor = (UMAC_TX_DESC *)((uint8 *)pTxDescriptor + 2);
	}

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *) ((uint8 *) pTxDescriptor - sizeof(UMAC_DEVIF_TX_DATA));
	CHECK_POINTER_IS_VALID(pDevIfTxReq);
	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDescriptor);


	if (/*(linkId==0) || */ (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_AWAKE)) {
		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[linkId]++;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		Result = UMAC_LL_REQ_TX_FRAME(UmacHandle, pTxDescriptor->QueueId, pDevIfTxReq, linkId);
		UMAC_Store_Internal_TxDesc(UmacHandle,
					pTxDescriptor,
					pUmacInstance->gVars.ApUmacTxDesc,
					UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
					headerOffset,
					pDriverInfo);

		if (Result != WFM_STATUS_SUCCESS) {
			UMAC_Release_Internal_TxDesc(UmacHandle, pTxDescriptor, pUmacInstance->gVars.ApUmacTxDesc, UMAC_NUM_AP_INTERNAL_TX_BUFFERS, &pDriverInfo);
			UMAC_RELEASE_PACKET_ID_AP(UmacHandle, pTxDescriptor->PacketId);
			UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle,
						pRxDescriptor,
						pDriverInfo);

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		}		/*if(Result != WFM_STATUS_SUCCESS) */
	}			/*if((pUmacInstance->operatingMode == OPERATING_MODE_STA) || (pUmacInstance->sta[linkId].PsState== STA_PS_STATE_AWAKE)) */
	else {
		/* Buffer the frame, as the destination STA is in PS mode */
		uint8 bResp = FALSE;
		/* Buffer the frame, as the destination STA is in PS mode */
		bResp = UMAC_Buffer_Frame(UmacHandle,
					linkId,
					pTxDescriptor,
					headerOffset,
					pDriverInfo
					);
		if (!bResp) {
			/* Allocated buffer is full for this STA so drop the packet */
			UMAC_RELEASE_PACKET_ID_AP(UmacHandle, pTxDescriptor->PacketId);
			/*UMAC_LL_RELEASE_RXBUFF(UmacHandle, (UMAC_RX_DESC *) pMsg);*/
			UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle,
						(UMAC_RX_DESC *) pMsg,
						pDriverInfo);
		}
	}

exit_handler:
	return Result;
} /* end UMAC_ProcFwdFrame() */

/******************************************************************************
 * NAME:UMAC_ProcUpdateTIMIE
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates the TIM IE in the beacon/probe response
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcUpdateTIMIE(UMAC_HANDLE UmacHandle, void *pMsg)
{

	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	WSM_HI_UPDATE_IE_REQ *pUpdateIeReq = NULL;
	UMAC_TIM_IE *pUpdateTIM = NULL;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_TX, "UMAC_ProcUpdateTIMIE()\n");
	UMAC_SET_STATE_PROCESSING(UmacHandle);

#if 0
	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pMsg);

	pTxDataReq = (UMAC_TX_DATA *) pMsg;
#endif

	if (NULL == pUmacInstance->gVars.pUpdateIeReq) {
		pUmacInstance->gVars.pUpdateIeReq = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

		OS_ASSERT(pUmacInstance->gVars.pUpdateIeReq);

		WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pUpdateIeReq);

		OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
		pUpdateIeReq = (WSM_HI_UPDATE_IE_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pUpdateIeReq);
		CHECK_POINTER_IS_VALID(pUpdateIeReq);
		pUpdateIeReq->Flags = UMAC_UPDATE_IE_FLAG_BCN;
		pUpdateIeReq->NumIEs = 1;
		pUpdateTIM = (UMAC_TIM_IE *) ((uint8 *) pUpdateIeReq + sizeof(WSM_HI_UPDATE_IE_REQ));
		pUpdateTIM->elementID = UMAC_IE_ELEMENT_TIM;
		pUpdateTIM->length = 4;	/*DTIM count, period, Bitmap Ctrl and Partial virtual bitmap */
		pUpdateTIM->DTIMperiod = pUmacInstance->dot11BssConf.DTIMperiod;
		pUpdateTIM->bitmapCTRL = pUmacInstance->bitmapCtrl;
		pUpdateTIM->bitmap[0] = pUmacInstance->virtualBitmap;
		pUpdateIeReq->MsgLen = sizeof(WSM_HI_UPDATE_IE_REQ) + pUpdateTIM->length + sizeof(pUpdateTIM->elementID) + sizeof(pUpdateTIM->length);
		pUmacInstance->updateTimIeInProgress = FALSE;
		OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
	} /* if(NULL == pUmacInstance->gVars.pUpdateIeReq) */
	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;

		pSmgmtReq->Type = SMGMT_REQ_TYPE_UPDATE_IE;

		pSmgmtReq->ReqNum = 0;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;

	} /* if(!pSmgmtReq->HandleMgmtReq) */
	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	pSmgmtReq->HandleMgmtReq = 0;
	UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pUmacInstance->gVars.pUpdateIeReq
					);
	pUmacInstance->gVars.pUpdateIeReq = NULL;
	UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pUpdateIeReq);
	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);

 exit_handler:
	return Result;
}

/******************************************************************************
 * NAME:UMAC_ProcDisconnectSTA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function send deauthentication frame when it receives data frame from
 * a non-associated STA.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcDisconnectSTA(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_MGMT_FRAME *pDeAuthFrame = NULL;
	uint16 FrameBodyLen = 0;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 remoteAddr[WFM_MAC_ADDRESS_SIZE];
	HI_MSG_HDR *pHiMsg = (HI_MSG_HDR *) pMsg;
	WFM_UMAC_FRAME_HDR *pDot11hdr;
	uint8	rxLinkId;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcDisconnectSTA()\n");

	WFM_UMAC_DBG_PRINT_TX_DESC();

	rxLinkId = (pHiMsg->MsgId & UMAC_HI_MSG_LINK_ID) >> 6;

	if (pUmacInstance->sta[rxLinkId].state == STA_STATE_DEAUTHENTICATING)
		goto exit_handler;

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_DEAUTH_REQ);

	LOG_DATA(DBG_WFM_UMAC, "UMAC_ProcDisconnectSTA pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	pDeAuthFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pDeAuthFrame);

	/*
	   Creating the WLAN Header
	*/
	pDot11hdr = (WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pHiMsg);

	D0_ADDR_COPY(remoteAddr, pDot11hdr->Address2);

	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					WFM_UMAC_MGMT_DEAUTH,
					&pDeAuthFrame->dot11hdr,
					remoteAddr,
					pUmacInstance->MacAddress,
					0,
					0
					);

	FrameBodyLen = WFM_UMAC_CreateDeAuthFrame(
							UmacHandle,
							pDeAuthFrame->FrameBody
							);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcDisconnectSTA() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);

	pTxDesc->QueueId = WFM_AC_VO;	/* WFM_AC_VO ; */
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_CONNECT);
	pTxDesc->Reserved = 0;

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
				UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL
				);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	/*
	   Sending the management frame to LMAC for transmission
	*/

	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					DEFAULT_LINK_ID
					);

	if (Result != WFM_STATUS_SUCCESS)
		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);

exit_handler:
	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
	pUmacInstance->NumRxBufPending[rxLinkId]--;
	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	UMAC_LL_RELEASE_RXBUFF(UmacHandle, pHiMsg);
	UMAC_LL_RELEASE_RXDESC(UmacHandle, pHiMsg);
	return Result;

} /* end UMAC_ProcDisconnectSTA() */

/******************************************************************************
 * NAME:UMAC_ProcStartFind
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function Initiates a Find request based on the parameters provided. If
 * the SM [State-Machine] is already doing Find, this function will return
 * immediatly with a rejected status.This function scans all the channels in
 * the given band.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartFind(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	/* Start Find */
	if (pUmacInstance->UmacFindState == UMAC_READY_TO_FIND) {

		pUmacInstance->UmacFindState = UMAC_FINDING;
		LOG_EVENT(DBG_WFM_UMAC, "FIND STATE = UMAC_FINDING\n");
	}

	pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;
	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;

		pSmgmtReq->Type = SMGMT_REQ_TYPE_FIND_REQ;

		pSmgmtReq->ReqNum = 0;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;

	} /* if(!pSmgmtReq->HandleMgmtReq) */

	pSmgmtReq->HandleMgmtReq = 0;

	UMAC_LL_REQ_START_FIND(UmacHandle);

 exit_handler:

	return Result;

} /* end UMAC_ProcStartFind() */

/******************************************************************************
 * NAME:UMAC_ProcFindCompleted
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will be called upon the completion of Find by LMAC.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcFindCompleted(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "UMAC_ProcFindCompleted()\n");

	OS_ASSERT(pMsg);

	if (pUmacInstance->deviceDiscovery == TRUE) {
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_DEV_DISCVRY_COMPLETED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);

		pUmacInstance->deviceDiscovery = FALSE;
	}

	return Result;
} /* end UMAC_ProcFindCompleted() */

/******************************************************************************
 * NAME:UMAC_ProcStartDeviceDiscovery
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function Initiates a device discovery based on the parameters provided.
 * If the SM [State-Machine] is already doing device discovery, this function
 * will return immediatly with a rejected status.This function scans all the
 * channels in the given band.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcStartDeviceDiscovery(UMAC_HANDLE UmacHandle, void *pMsg)
{
	/*SCAN_PARAMETERS		  *pScanParams       = NULL  ; */
	/*UMAC_OID_P2P_DEVICE_DISCOVERY    *pDeviceDiscovery  = NULL  ; */
	WFM_BSS_LIST_SCAN *pbssListScan = NULL;
	/*UMAC_OID_P2P_DEVICE_DISCOVERY    *pFindInfo	 = NULL  ; */
	WFM_STATUS_CODE result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	pbssListScan = (WFM_BSS_LIST_SCAN *) &pUmacInstance->p2pDevDiscvry.bssListScan;

	if (pUmacInstance->UmacScanState == UMAC_SCANNING || pUmacInstance->UmacFindState == UMAC_FINDING) {
		result = WFM_STATUS_REQ_REJECTED;
		LOG_EVENT(DBG_WFM_UMAC, "***ERROR: WFM_STATUS_REQ_REJECTED!\n\n");
	} else {

		pUmacInstance->deviceDiscovery = TRUE;

		/* Create Scan request based on the received paramaters */
		/*pbssListScan->Flags		=  pDeviceDiscovery->band ; */
		/*pbssListScan->SsidLength	   =  0 ; */
		/*pbssListScan->PowerLevel	   =  0 ; */

		/* P2P device discovery covers SCAN and Find. */
		/* Get Context from host to start Scan. After Scan Complete send find
		   request to WLAN device */
		WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_SCAN, pbssListScan);
	}

	return result;
} /* end UMAC_ProcStartDeviceDiscovery() */
#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:UMAC_TxMgmtFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function transmit managment frames recevied from host driver.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_TxMgmtFrame(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	UMAC_TX_DESC *pTxDesc = NULL;
	uint8 linkId = 0;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	UMAC_MGMT_TRANSMIT_REQ *pTxMgmtFrame = (UMAC_MGMT_TRANSMIT_REQ *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	void *pDriverInfo = NULL;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_TxMgmtFrame()\n");

#if P2P_MINIAP_SUPPORT
	linkId = (uint8) (pTxMgmtFrame->linkId & 0x0000000F);
#endif

	if ((pUmacInstance->UmacCurrentState != UMAC_GROUP_FORMED)
		&& (pUmacInstance->UmacCurrentState != UMAC_ASSOCIATED)
		&& (pUmacInstance->ChannelChange.ChannelState != UMAC_CHANNEL_STATE_CHANGED)) {
		Result = WFM_STATUS_REQ_REJECTED;
		goto exit_handler;
	}

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	LOG_DATA((DBG_WFM_UMAC), "UMAC_TxMgmtFrame pTxDesc=0x%x)\n", pTxDesc);

	OS_MemoryCopy((void *)TX_DESC_GET_DOT11FRAME(pTxDesc), (void *)pTxMgmtFrame->pTxMgmtData, pTxMgmtFrame->FrameLength);

	ASSIGN_FRAME_LENGTH(pTxDesc, (uint16)pTxMgmtFrame->FrameLength);

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_TxMgmtFrame() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->PacketId |= UMAC_PACKETID_FLAG_EXT_MGMT;

	pTxDesc->MaxTxRate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->OurAllBasicSupportedRates_Abgn);
	pTxDesc->Flags = 0;
	pTxDesc->More = 0;

	if (pUmacInstance->ChannelChange.ChannelState == UMAC_CHANNEL_STATE_CHANGED)
		pTxDesc->QueueId = UMAC_QUEUE_ID_2ND_CHAN_FLAG;
	else
		pTxDesc->QueueId = 0;	/* WFM_AC_VO ; */

	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_MGT);
	pTxDesc->Reserved = 0;
	pTxDesc->ExpireTime = WFM_UMAC_TX_EXPIRE_TIME;

	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(UmacHandle, pTxDesc, pUmacInstance->gVars.umacTxDescInt, UMAC_NUM_INTERNAL_TX_BUFFERS, 0, NULL);

	LOG_BYTES(DBG_WFM_UMAC, "UMAC_TxMgmtFrame: ", (uint8 *) pMgmtFrame, GET_TX_DESC_FRAME_LENGTH(pTxDesc), GET_TX_DESC_FRAME_LENGTH(pTxDesc));

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the management frame to LMAC for transmission
	*/
	if (pUmacInstance->ChannelChange.ChannelState == UMAC_CHANNEL_STATE_CHANGED) {
		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[linkId]++;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);
		Result = UMAC_LL_REQ_TX_FRAME(
						UmacHandle,
						QueueId_CTRL,
						pDevIfTxReq,
						linkId
						);
	}
#if P2P_MINIAP_SUPPORT
	else if ((pUmacInstance->operatingMode == OPERATING_MODE_STA) || (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_AWAKE)
	   ) {
#endif	/* P2P_MINIAP_SUPPORT */
		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[linkId]++;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		Result = UMAC_LL_REQ_TX_FRAME(
						UmacHandle,
						pTxDesc->QueueId,
						pDevIfTxReq,
						linkId
						);
#if P2P_MINIAP_SUPPORT
	} else {
		/* Buffer the frame, as the destination STA is in PS mode */
		uint8 bResp = FALSE;
		bResp = UMAC_Buffer_Frame(UmacHandle, linkId, pTxDesc, 0, NULL);
		/* Remove from the internal Tx bufferes */
		UMAC_Release_Internal_TxDesc(
				UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				&pDriverInfo
				);
		if (!bResp) {
			/* Allocated buffer is full for this STA so drop the packet */
			Result = WFM_STATUS_OUT_OF_RESOURCES;
			pTxDesc->PacketId = pTxDesc->PacketId & 0x000000FF;
			UMAC_RELEASE_PACKET_ID(UmacHandle, pTxDesc->PacketId);
		}
	}
#endif	/* P2P_MINIAP_SUPPORT */

exit_handler:
	return Result;

}

/******************************************************************************
 * NAME:	UMAC_ProcSchAsyncMgmt
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This handler is to handle UMAC exceptions.If UMAC is getting some events
 * messages when its not expecting it, which may lead the system to an
 * in-consistent state is referd as UMAC Exception here.
 * For example, if LMAC is givng a data frame before join, it will be treated
 * as a UMAC exception. This function will ASSERT.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcSchAsyncMgmt(UMAC_HANDLE UmacHandle, void *pMsg)
{
	LOG_EVENT(DBG_WFM_ASSOC, "UMAC_ProcSchAsyncMgmt()\n");

	UMAC_ManageAsynMgmtOperations(UmacHandle);
	return WFM_STATUS_SUCCESS;
} /* end UMAC_ProcSchAsyncMgmt() */

#if SEND_PROBE
/******************************************************************************
 * NAME:	UMAC_ProcSendProbeRequest
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This handler is to handle the case when UMAC receives BSS LOST and it wants
 * to check whether the AP is alive or dead by sending probe request.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcSendProbeRequest(UMAC_HANDLE UmacHandle, void *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_DATA_FRAME *pProbeReqFrame = NULL;
	uint32 Result = WFM_STATUS_SUCCESS;
	uint16 FrameBodyLen = 0;
	uint8 linkId = DEFAULT_LINK_ID;
	void *pDriverInfo = NULL;
	static uint32 sendUniCastProReq = 0;
	uint8 broadcastaddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	static uint8 staAliveCheck = 1;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	UMAC_RUNTIME *pUmacRunTime;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_TX, "UMAC_ProcSendProbeRequest()\n");

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);
	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	pProbeReqFrame = (WFM_UMAC_DATA_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pPrbReqFrm);


	if (sendUniCastProReq % 3) {

		staAliveCheck = 1;
		/* Create header */
		WFM_UMAC_CreateDot11FrameHdr_MoreGeneral(
			&pProbeReqFrame->dot11hdr,
			WFM_UMAC_MGMT,
			WFM_UMAC_MGMT_PROB_REQ,
			WFM_UMAC_NOT_TO_DS_FRAME,
			WFM_UMAC_NOT_PROTECTED_FRAME,
			broadcastaddr, /* A1 is BSSID */
			pUmacInstance->MacAddress, /* A2 is our MAC address (it will be overwritten by WSM) */
			broadcastaddr /* A3 is BSSID */
			);
	} else {
		if (staAliveCheck) {
			staAliveCheck = 0;
			if (pUmacInstance->gVars.p.PsMode.PmMode != WFM_PS_MODE_DISABLED) {
				pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;
				OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
				pSmgmtReq->Type = SMGMT_REQ_TYPE_DYNAMIC_UPDATIONS;
				pSmgmtReq->ReqNum = 0;
				pUmacRunTime = (UMAC_RUNTIME *) &pSmgmtReq->run_time;
				pUmacRunTime->UpdatePsMode = 1;
				pUmacInstance->gVars.p.PsMode.PmMode = WFM_PS_MODE_DISABLED;
				pUmacInstance->gVars.p.PsMode.FastPsmIdlePeriod = 0;
				UMAC_ManageAsynMgmtOperations(UmacHandle);
				pUmacRunTime->UpdatePsMode = 0;
			}
		}

		WFM_UMAC_CreateDot11FrameHdr_MoreGeneral(
			&pProbeReqFrame->dot11hdr,
			WFM_UMAC_MGMT,
			WFM_UMAC_MGMT_PROB_REQ,
			WFM_UMAC_NOT_TO_DS_FRAME,
			WFM_UMAC_NOT_PROTECTED_FRAME,
			pUmacInstance->RemoteMacAdd, /* A1 is BSSID */
			pUmacInstance->MacAddress, /* A2 is our MAC address (it will be overwritten by WSM) */
			pUmacInstance->RemoteMacAdd /* A3 is BSSID */
			);
	}
	sendUniCastProReq++;
	/* Now fill up body */
	FrameBodyLen = WFM_UMAC_CreateProbeReqTemplateFrame(
				UmacHandle,
				pProbeReqFrame->FrameBody,
				(uint8)pUmacInstance->SsidLength,
				pUmacInstance->Ssid,
				WFM_BSS_LIST_SCAN_2_4G_BAND,
				WSM_TMPLT_PRBREQ
				);

	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));

	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_ProcSendProbeRequest() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	pTxDesc->MaxTxRate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);
	pTxDesc->QueueId = WFM_AC_VO;
	pTxDesc->More = 0;
	pTxDesc->Flags = 0;
	ASSIGN_TX_PTA_PRIORITY(pTxDesc, PTA_PRIORITY_VOICE);
	pTxDesc->Reserved = 0;
	pTxDesc->HtTxParameters = pUmacInstance->WfmHtTxParameters;

	UMAC_Store_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					0,
					NULL
					);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*
	   Sending the frame to LMAC for transmission
	*/
	Result = UMAC_LL_REQ_TX_FRAME(
					UmacHandle,
					pTxDesc->QueueId,
					pDevIfTxReq,
					linkId
					);

	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_RELEASE_PACKET_ID(UmacHandle, pTxDesc->PacketId);
		WFM_UMAC_DBG_REMOVE_TX_DESC(pTxDesc);

		UMAC_Release_Internal_TxDesc(
					UmacHandle,
					pTxDesc,
					pUmacInstance->gVars.umacTxDescInt,
					UMAC_NUM_INTERNAL_TX_BUFFERS,
					&pDriverInfo
					);

		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					MOVE_TO_START_OF_DEV_IF_TX_REQ(pTxDesc)
					);

		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[linkId]--;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	} /* if(Result != WFM_STATUS_SUCCESS) */

exit_handler:
	return Result;
} /* end UMAC_ProcSendProbeRequest() */
#endif /* SEND_PROBE */

/******************************************************************************
 * NAME:	UMAC_BssLostTimeout_Cb
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the UMAC timer callback.
 * \param *Handle   -  Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
static void UMAC_BssLostTimeout_Cb(void *Handle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) Handle;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	UMAC_RUNTIME *pUmacRunTime;

	OS_ASSERT(pUmacInstance);

#if SEND_PROBE
	pUmacInstance->isProbeReqFrame = 0;
	if (pUmacInstance->isProbeRespRcvd == 1) {
		pUmacInstance->isProbeRespRcvd = 0;
		if (pUmacInstance->gVars.p.PsMode.PmMode != WFM_PS_MODE_DISABLED) {
				pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;
				OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
				pSmgmtReq->Type = SMGMT_REQ_TYPE_DYNAMIC_UPDATIONS;
				pSmgmtReq->ReqNum = 0;
				pUmacRunTime = (UMAC_RUNTIME *) &pSmgmtReq->run_time;
				pUmacRunTime->UpdatePsMode = 1;
				pUmacInstance->gVars.p.PsMode.PmMode = WFM_PS_MODE_DISABLED;
				pUmacInstance->gVars.p.PsMode.FastPsmIdlePeriod = 0;
				UMAC_ManageAsynMgmtOperations(Handle);
				pUmacRunTime->UpdatePsMode = 0;
		}
	} else {

		pUmacInstance->BssLostEventRcvd = 1;
		if ((!pUmacInstance->BssRegainedEventRcvd) && ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) ||
			(pUmacInstance->UmacCurrentState == UMAC_PROCESSING && pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {
			WFM_UMAC_2_UL_SendEvent(
					Handle,
					WFM_EVT_BSS_LOST,
					sizeof(pUmacInstance->eventIndicationData),
					(uint8 *)&pUmacInstance->eventIndicationData,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		} else {
			pUmacInstance->BssRegainedEventRcvd = 0;
		}
	}
#else
	pUmacInstance->nullFrmPktId = 0;
	pUmacInstance->isNullDataFrame = 0;

	if (pUmacInstance->statusNullDataFrameCnf == 1) {
		pUmacInstance->statusNullDataFrameCnf = 0;
		if (pUmacInstance->gVars.p.PsMode.PmMode != WFM_PS_MODE_DISABLED) {
				pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;
				OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));

				pSmgmtReq->Type = SMGMT_REQ_TYPE_DYNAMIC_UPDATIONS;
				pSmgmtReq->ReqNum = 0;
				pUmacRunTime = (UMAC_RUNTIME *) &pSmgmtReq->run_time;
				pUmacRunTime->UpdatePsMode = 1;
				pUmacInstance->gVars.p.PsMode.PmMode = WFM_PS_MODE_DISABLED;
				pUmacInstance->gVars.p.PsMode.FastPsmIdlePeriod = 0;
				UMAC_ManageAsynMgmtOperations(Handle);
				pUmacRunTime->UpdatePsMode = 0;
		}
	} else {
		pUmacInstance->BssLostEventRcvd = 1;
		if (!pUmacInstance->BssRegainedEventRcvd) {
			WFM_UMAC_2_UL_SendEvent(
					Handle,
					WFM_EVT_BSS_LOST,
					sizeof(pUmacInstance->eventIndicationData),
					(uint8 *)&pUmacInstance->eventIndicationData,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		} else {
			pUmacInstance->BssRegainedEventRcvd = 0;
		}
	}
#endif
}

/******************************************************************************
 * NAME:UMAC_ProcUpdateERPIE
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates the ERP IE in the beacon/probe response
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pMsg		- Pointer to the message to be Processed.
 * \returns uint32.
 *****************************************************************************/
static uint32 UMAC_ProcUpdateERPIE(UMAC_HANDLE UmacHandle, void *pMsg)
{

	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	WSM_HI_UPDATE_IE_REQ *pUpdateErpIeReq = NULL;
	WFM_UMAC_ERP_IE *pUpdateERP = NULL;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_TX, "UMAC_ProcUpdateERPIE()\n");
	UMAC_SET_STATE_PROCESSING(UmacHandle);

	if (NULL == pUmacInstance->gVars.pUpdateErpIeReq) {
		pUmacInstance->gVars.pUpdateErpIeReq = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

		OS_ASSERT(pUmacInstance->gVars.pUpdateErpIeReq);

		WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pUpdateErpIeReq);

		OS_LOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
		pUpdateErpIeReq = (WSM_HI_UPDATE_IE_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pUpdateErpIeReq);
		CHECK_POINTER_IS_VALID(pUpdateErpIeReq);
		pUpdateErpIeReq->Flags = UMAC_UPDATE_IE_FLAG_BCN | UMAC_UPDATE_IE_FLAG_PRB_RESP;
		pUpdateErpIeReq->NumIEs = 1;
		pUpdateERP = (WFM_UMAC_ERP_IE *) ((uint8 *) pUpdateErpIeReq + sizeof(WSM_HI_UPDATE_IE_REQ));
		pUpdateERP->elementID = UMAC_IE_ELEMENT_ERP_INFORMATION;
		pUpdateERP->length = 1;	/*ERP information */
		pUpdateERP->ErpInfo = pUmacInstance->ErpInformation;
		pUpdateErpIeReq->MsgLen = sizeof(WSM_HI_UPDATE_IE_REQ) + pUpdateERP->length + sizeof(pUpdateERP->elementID) + sizeof(pUpdateERP->length);
		pUmacInstance->updateERPIeInProgress = FALSE;
		OS_UNLOCK(pUmacInstance->gVars.p.UpdateERPIeLock);
	} /* if(NULL == pUmacInstance->gVars.pUpdateErpIeReq) */
	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (!pSmgmtReq->HandleMgmtReq) {
		OS_MemoryReset(pSmgmtReq, sizeof(SMGMT_REQ_PARAMS));
		pSmgmtReq->HandleMgmtReq = 1;

		pSmgmtReq->Type = SMGMT_REQ_TYPE_UPDATE_ERP_IE;

		pSmgmtReq->ReqNum = 0;

		Result = UMAC_ManageAsynMgmtOperations(UmacHandle);

		if (Result != WFM_STATUS_SUCCESS)
			goto exit_handler;

	} /* if(!pSmgmtReq->HandleMgmtReq) */
	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	pSmgmtReq->HandleMgmtReq = 0;
	UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pUmacInstance->gVars.pUpdateErpIeReq
					);
	pUmacInstance->gVars.pUpdateErpIeReq = NULL;
	UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pUpdateErpIeReq);
	UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);

 exit_handler:
	return Result;
}

