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
 * \file umac_proc_lmac_frames.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_proc_lmac_frames.c
 * \brief
 * This module processes frames from lmac.
 * \ingroup Upper_MAC_Core
 * \date 05/01/08 17:19
 */

/******************************************************************************
	      INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "d0_defs.h"

#include "umac_dev_conf.h"
#include "umac_proc_lmac_frames.h"
#include "umac_frames.h"
#include "umac_sm.h"
#include "umac_task.h"

#include "umac_ll_if.h"
#include "umac_if.h"
#if P2P_MINIAP_SUPPORT
#include "umac_data_handler.h"
#endif	/*P2P_MINIAP_SUPPORT */

extern UMAC_FSM_HANDLER UMAC_StateEventTable[UMAC_MAX_STATE][UMAC_MAX_EVT];

static void WFM_UMAC_ProcessMgmtFrames(UMAC_HANDLE UmacHandle,
				       uint8 SubType,
				       UMAC_RX_DESC *pRxDescriptor);

static void WFM_UMAC_ProcessCntrlFrames(UMAC_HANDLE UmacHandle,
					uint8 SubType,
					UMAC_RX_DESC *pRxDescriptor);

static void WFM_UMAC_ProcessDataFrames(UMAC_HANDLE UmacHandle,
				       uint8 SubType,
				       UMAC_RX_DESC *pRxDescriptor);

UMAC_FRAME_HANDLER UMAC_InComing_Frame_Hndlr[] = {
	WFM_UMAC_ProcessMgmtFrames,
	WFM_UMAC_ProcessCntrlFrames,
	WFM_UMAC_ProcessDataFrames
};

/******************************************************************************
	PROCESSING OF INCOMING FRAMES FROM LMAC
******************************************************************************/

static void WFM_UMAC_ProcessMgmtFrames(UMAC_HANDLE UmacHandle, uint8 SubType, UMAC_RX_DESC *pRxDescriptor)
{
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	WFM_UMAC_ASSOC_RSP_BODY *pAssocRsp = NULL;
	WFM_UMAC_AUTH_FRAME_BODY *pAuthFrame = NULL;
	UMAC_ACTION_FRAME_BODY *pActionFrame = NULL;
	uint16 WfmEvent = UMAC_MAX_EVT;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;


#if MGMT_FRAME_PROTECTION
	uint16 PayloadLength = 0;
	IeElement Ie = { 0 };
#endif	/* MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
	uint8 staAddr[6];
#endif	/* P2P_MINIAP_SUPPORT */

	uint8 linkId = 0;
#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
#endif	/* P2P_MINIAP_SUPPORT */

	LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_ProcessMgmtFrames()\n");

	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);

#if MGMT_FRAME_PROTECTION
	PayloadLength = pRxDescriptor->MsgLen - sizeof(UMAC_RX_DESC) - sizeof(WFM_UMAC_FRAME_HDR);
#endif	/* MGMT_FRAME_PROTECTION */

	LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_ProcessMgmtFrames()\n");

	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);

	switch (SubType) {
	case WFM_UMAC_MGMT_ASSOC_REQ:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_ASSOC_REQ\n");

		if (!pUmacInstance->gVars.p.stopAPFlag) {

#if P2P_MINIAP_SUPPORT
			if (pUmacInstance->operatingMode >= OPERATING_MODE_AP && linkId > 0)
				WfmEvent = UMAC_ASSOC_REQ;
#endif	/* P2P_MINIAP_SUPPORT */
		}
		break;

	case WFM_UMAC_MGMT_ASSOC_RSP:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_ASSOC_RSP\n");

		if (pUmacInstance->UmacCurrentState == UMAC_PROCESSING) {
			if (pUmacInstance->UmacPreviousState != UMAC_ASSOCIATING)
				break;
		} else if (pUmacInstance->UmacCurrentState != UMAC_ASSOCIATING) {
			break;
		}
		pAssocRsp = (WFM_UMAC_ASSOC_RSP_BODY *) pUmacMgmt->FrameBody;

		if (UMAC_STATUS_CODE_0 == pAssocRsp->StatusCode) {
			WfmEvent = WFM_EVT_CONNECTED;
			UMAC_ProcPostAssocIE(UmacHandle, pRxDescriptor);
			UMAC_SET_STATE(UmacHandle, UMAC_ASSOCIATED);
			UMAC_SET_STATE_PROCESSING(UmacHandle);
			/*Send the connected event from here */
			WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WfmEvent,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
			WfmEvent = UMAC_ASSOC_SUCCESS;
		} else {
			WfmEvent = UMAC_ASSOC_FAILED;
		}

		break;

	case WFM_UMAC_MGMT_REASSOC_REQ:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_REASSOC_REQ\n");
#if P2P_MINIAP_SUPPORT
		if (pUmacInstance->operatingMode >= OPERATING_MODE_AP && linkId > 0)
			WfmEvent = UMAC_ASSOC_REQ;
#endif	/* P2P_MINIAP_SUPPORT */
		break;

	case WFM_UMAC_MGMT_REASSOC_RSP:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_REASSOC_RSP\n");
		if (pUmacInstance->UmacCurrentState == UMAC_PROCESSING) {
			if (pUmacInstance->UmacPreviousState != UMAC_REASSOCIATING)
				break;
		} else if (pUmacInstance->UmacCurrentState != UMAC_REASSOCIATING) {
			break;
		}

		pAssocRsp = (WFM_UMAC_ASSOC_RSP_BODY *) pUmacMgmt->FrameBody;

		if (UMAC_STATUS_CODE_0 == pAssocRsp->StatusCode) {
			uint8 PMKIDSent = 0;
			/*Send the reconnected event from here */
			WfmEvent = WFM_EVT_RECONNECTED;
			UMAC_ProcPostAssocIE(UmacHandle, pRxDescriptor);
			UMAC_SET_STATE(UmacHandle, UMAC_ASSOCIATED);
			UMAC_SET_STATE_PROCESSING(UmacHandle);
			if (pUmacInstance->PMKIDSent) {
				PMKIDSent = 1;
				pUmacInstance->PMKIDSent = 0;
			}
			/* Inform the host that we are associated now */
			WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WfmEvent,
					1,
					&PMKIDSent,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
			WfmEvent = UMAC_REASSOC_SUCCESS;
		}
#if MGMT_FRAME_PROTECTION
		else if ((UMAC_STATUS_CODE_30 == pAssocRsp->StatusCode)
			 && (pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE)) {
			/*Check Timeout Interval Information Element */
			if ((WFM_UMAC_ProcIeElements(pAssocRsp->IeData, (PayloadLength - 6), UMAC_IE_ELEMENT_TIMEOUT, &Ie) == TRUE)
			    && (Ie.size)) {
				/*Get Timeout Interval in TUs */
				uint32 timeOutInterval = 0;
				timeOutInterval = (uint32) *((uint8 *)Ie.info + 1);

				/*
				  Start a Timer to initiate ReAssociation
				  again after timeout interval
				*/
				OS_START_TIMER(
					pUmacInstance->gVars.p.UmacTimers.ReAssocTimer,
					timeOutInterval,
					UmacHandle
					);

				/*
				  Change State from ReAssociating to Associated
				*/
				UMAC_SET_STATE(UmacHandle, UMAC_ASSOCIATED);
			}
		}
#endif	/* MGMT_FRAME_PROTECTION */
		else
			WfmEvent = UMAC_REASSOC_FAILED;

		break;

	case WFM_UMAC_MGMT_PROB_REQ:
		{
			LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_PROB_REQ\n");
			/*EVT_DATA_RX_MGMT_FRAME RxMgmtFrame;
			uint16 FrameLen;
			FrameLen = pRxDescriptor->MsgLen - sizeof(UMAC_RX_DESC);
			RxMgmtFrame.FrmLen = FrameLen;
			OS_MemoryCopy(RxMgmtFrame.FrmBuf, pUmacMgmt, FrameLen);*/

			if (pUmacInstance->gVars.p.useP2P) {
				if (!UMAC_ProcProbeReqCheckCCKRates(UmacHandle, pRxDescriptor)) {
					WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_RX_MGMT_FRAME,
						(pRxDescriptor->MsgLen - sizeof(UMAC_RX_DESC)),
						(uint8 *) pUmacMgmt,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);
				}
			} else {
				if ((pUmacInstance->operatingMode < OPERATING_MODE_AP) &&
					OS_MemoryEqual(pUmacInstance->MacAddress, WFM_MAC_ADDRESS_SIZE, pUmacMgmt->dot11hdr.Address2, WFM_MAC_ADDRESS_SIZE))
					break;
				else
					WFM_UMAC_2_UL_SendEvent(
						UmacHandle,
						WFM_EVT_RX_MGMT_FRAME,
						(pRxDescriptor->MsgLen - sizeof(UMAC_RX_DESC)),
						(uint8 *) pUmacMgmt,
						WFM_UMAC_EVT_NOT_RETRANSMIT
						);
			}
			break;
		}

	case WFM_UMAC_MGMT_PROB_RSP:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_PROB_RSP\n");

		if (!(IS_BROADCAST_MAC_ADDRESS(pUmacMgmt->dot11hdr.Address1))) {
#if SEND_PROBE
			if (pUmacInstance->isProbeReqFrame) {
				if (OS_MemoryEqual(pUmacInstance->RemoteMacAdd, WFM_MAC_ADDRESS_SIZE, pUmacMgmt->dot11hdr.Address2, WFM_MAC_ADDRESS_SIZE))
					pUmacInstance->isProbeRespRcvd = TRUE;
			}
#endif
			WfmEvent = UMAC_PROC_SCAN_RSP;
		}

		break;

	case WFM_UMAC_MGMT_BEACON:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_BEACON\n");

		WfmEvent = UMAC_IN_BEACON;

		break;

	case WFM_UMAC_MGMT_ATIM:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_ATIM\n");

		break;

	case WFM_UMAC_MGMT_DISASSOC:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_DISASSOC\n");
#if P2P_MINIAP_SUPPORT
		if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
			if (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_DOZE) {
				UMAC_CHANGE_STA_PS_STATE(
							UmacHandle,
							linkId,
							STA_PS_STATE_AWAKE
							);
			}

			if (pUmacInstance->sta[linkId].state == STA_STATE_ASSOCIATED)
				WfmEvent = UMAC_DE_ASSOC_REQ;
			else {
				/*UMAC_LL_RELEASE_RXBUFF(
							UmacHandle,
							pRxDescriptor
							);

				UMAC_LL_RELEASE_RXDESC(
							UmacHandle,
							pRxDescriptor
							);*/
			}
		} else {
			WfmEvent = UMAC_DE_ASSOC_REQ;
#else
		WfmEvent = UMAC_DE_ASSOC_REQ;
#endif	/* P2P_MINIAP_SUPPORT */
#if P2P_MINIAP_SUPPORT
		}
#endif	/* P2P_MINIAP_SUPPORT */

	break;

	case WFM_UMAC_MGMT_AUTH:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_AUTH\n");


		if (!pUmacInstance->gVars.p.stopAPFlag) {
			pAuthFrame = (WFM_UMAC_AUTH_FRAME_BODY *) pUmacMgmt->FrameBody;

#if P2P_MINIAP_SUPPORT
			/* Copy transmitter's mac address */
			OS_MemoryCopy(
					staAddr,
					pUmacMgmt->dot11hdr.Address2,
					WFM_MAC_ADDRESS_SIZE
					);
			if ((1 == pAuthFrame->TransactionSeqNumber) && (pUmacInstance->operatingMode >= OPERATING_MODE_AP)) {
				uint8 index;
				LOG_EVENT(DBG_WFM_FRAMES, "Incoming Auth Req\n");

				index = UMAC_DB_Search_STA(UmacHandle, staAddr);
				if (index != pUmacInstance->gVars.p.maxNumClientSupport) {
					if ((STA_STATE_DEAUTHENTICATING != pUmacInstance->sta[index].state)
					    && (STA_STATE_AUTHENTICATING != pUmacInstance->sta[index].state)) {
						UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
						UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

						OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
						pUmacInstance->NumRxBufPending[linkId]--;
						OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
						if (index != 0)
							OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[index]);
						/* b0-b5 bits are set to 1 when unlink STA is called from host side
						   or from call backtimer or UMAC Internally */
						pUmacInstance->sta[index].hiMsgHdr.MsgId = index << 6;
						pUmacInstance->sta[index].state = STA_STATE_DEAUTHENTICATING;
						pRxDescriptor = (UMAC_RX_DESC *) &(pUmacInstance->sta[index].hiMsgHdr);
						WfmEvent = UMAC_UNLINK_STA;
					}
				} else {
					if (pUmacInstance->gVars.p.userallowaccess) {
						WFM_MAC_ADDR_ACTION peerverdict;
						/* Check if the MAC address is in whitelist/Blacklist */
						peerverdict = UMAC_SearchAllowedPeer(UmacHandle,  staAddr);
						if (peerverdict == WFM_NOTAVAILABLE) {
							WFM_UMAC_2_UL_SendEvent(
								UmacHandle,
								WFM_EVT_MACADDR_ACTION,
								6,
								staAddr,
								WFM_UMAC_EVT_NOT_RETRANSMIT
								);
							WfmEvent =  UMAC_AUTH_RESP_FAILED;
							break;
						} else if (peerverdict == WFM_BLACKLIST) {
							WfmEvent =  UMAC_AUTH_RESP_FAILED;
							break;
						}
					}
					index = UMAC_DB_Insert_STA(UmacHandle, staAddr);
					if (index < (pUmacInstance->gVars.p.maxNumClientSupport)) {
						UMAC_CHANGE_STA_PS_STATE(UmacHandle, index, STA_PS_STATE_AWAKE);
						WfmEvent = UMAC_AUTH_RESP_SUCCESS;
						pRxDescriptor->MsgId |= index << 6;
					} else {
						WfmEvent = UMAC_AUTH_RESP_FAILED;
					}
				}
			} else
#endif	/* P2P_MINIAP_SUPPORT */
			if ((pAuthFrame->TransactionSeqNumber == 2)
				   && (pAuthFrame->AuthAlgorithm != 2)) {
				if (pAuthFrame->StatusCode == UMAC_STATUS_CODE_0) {
					if (pAuthFrame->AuthAlgorithm != pUmacInstance->gVars.p.InfoHost.AuthenticationMode)
						WfmEvent = UMAC_AUTH_FAILED;
					else if (pUmacInstance->gVars.p.InfoHost.AuthenticationMode != UMAC_SHARED_KEY_AUTH)
						WfmEvent = UMAC_AUTH_SUCCESS;
					else
						WfmEvent = UMAC_START_AUTH;
				} else {
					WfmEvent = UMAC_AUTH_FAILED;
				}
			} else if ((pAuthFrame->AuthAlgorithm == 2)
				   && (pAuthFrame->TransactionSeqNumber == 2)) {
				WfmEvent = UMAC_FT_AUTH_COMPLT;
			} else if (pAuthFrame->TransactionSeqNumber == 4) {
				if (pAuthFrame->StatusCode == UMAC_STATUS_CODE_0)
					WfmEvent = UMAC_AUTH_SUCCESS;
				else
					WfmEvent = UMAC_AUTH_FAILED;
			}
		}
		if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATING) || (pUmacInstance->UmacCurrentState == UMAC_AUTHENTICATED)
			|| (pUmacInstance->UmacCurrentState == UMAC_REASSOCIATING)) {
			WfmEvent = UMAC_MAX_EVT;
		}
	break;
	case WFM_UMAC_MGMT_DEAUTH:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_DEAUTH\n");

#if P2P_MINIAP_SUPPORT
		if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
			if (pUmacInstance->sta[linkId].PsState == STA_PS_STATE_DOZE)
				UMAC_CHANGE_STA_PS_STATE(UmacHandle, linkId, STA_PS_STATE_AWAKE);
			if (pUmacInstance->sta[linkId].state == STA_STATE_ASSOCIATED)
				WfmEvent = UMAC_DE_AUTH_REQ;
			else {
				/*UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
				UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);*/
			}
		}			/*if(pUmacInstance->operatingMode >= OPERATING_MODE_AP) */
		else {
			WfmEvent = UMAC_DE_AUTH_REQ;
#else
		WfmEvent = UMAC_DE_AUTH_REQ;
#endif	/* P2P_MINIAP_SUPPORT */
#if P2P_MINIAP_SUPPORT
		}
#endif	/* P2P_MINIAP_SUPPORT */

	break;
	case WFM_UMAC_MGMT_ACTION:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_MGMT_ACTION\n");

#if MGMT_FRAME_PROTECTION
		if (pRxDescriptor->Status == WSM_STATUS_SUCCESS) {
			if (pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE) {
				pActionFrame = (UMAC_ACTION_FRAME_BODY *) pUmacMgmt->FrameBody + 4;
			} else {
#endif	/* MGMT_FRAME_PROTECTION */
				pActionFrame = (UMAC_ACTION_FRAME_BODY *) pUmacMgmt->FrameBody;
#if MGMT_FRAME_PROTECTION
			}
#endif	/* MGMT_FRAME_PROTECTION */
			switch (pActionFrame->category) {
#if DOT11K_SUPPORT
			case UMAC_CATEGORY_RADIO_MEASUREMENT:
				switch (pActionFrame->actionField) {
				case UMAC_ACTION_TYPE_RADIO_MEASUREMENT_REQ:
					WfmEvent = UMAC_RADIO_MEASUREMENT;
					break;
				case UMAC_ACTION_TYPE_LINK_MEASUREMENT_REQ:
					WfmEvent = UMAC_LINK_MEASUREMENT;
					break;
				case UMAC_ACTION_TYPE_NEIGHBOR_REPORT_RESP:
					WfmEvent = UMAC_SEND_NEIGHBOR_REP_RESP;
					break;
				}
				break;
#endif	/* DOT11K_SUPPORT */

#if FT_SUPPORT
			case UMAC_CATEGORY_FT_AUTH:
				if (pActionFrame->actionField == UMAC_ACTION_TYPE_FT_AUTH_RESP)
					WfmEvent = UMAC_FT_AUTH_COMPLT;
			break;
#endif /* FT_SUPPORT */

#if MGMT_FRAME_PROTECTION
			case UMAC_CATEGORY_SA_QUERY:
				if (pActionFrame->actionField == UMAC_ACTION_TYPE_SA_QUERY_REQ)
					WfmEvent = UMAC_SAQUERY_REQ;
				else if (pActionFrame->actionField == UMAC_ACTION_TYPE_SA_QUERY_RESP)
					WfmEvent = UMAC_SAQUERY_SUCCESS;
			break;
#endif	/* MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
			case UMAC_PUBLIC_ACTN_FRM_CTGRY:
			case UMAC_GENERAL_ACTN_FRM_CTGRY:
				{
					EVT_DATA_ACTION_FRAME EvtActionFrame;
					LOG_EVENT(DBG_WFM_FRAMES, "UMAC_PUBLIC_ACTN_FRM_CTGRY/UMAC_GENERAL_ACTN_FRM_CTGRY\n");
					if (pActionFrame->OuiInfo.OuiSubType == D11_P2P_ACTION_TYPE_DEV_DISCVRBLTY_REQ
						&& (pUmacInstance->operatingMode >= OPERATING_MODE_AP)) {
							uint8 index;
							/* Copy transmitter's mac address */
							OS_MemoryCopy(
								staAddr,
								pUmacMgmt->dot11hdr.Address2,
								WFM_MAC_ADDRESS_SIZE
								);
							index = UMAC_DB_Search_STA(UmacHandle, staAddr);
							if (index == pUmacInstance->gVars.p.maxNumClientSupport) {
								index = UMAC_DB_Insert_STA(UmacHandle, staAddr);
								if (index < pUmacInstance->gVars.p.maxNumClientSupport) {
									pRxDescriptor->MsgId |= index << 6;
									WfmEvent = UMAC_RESET_SEQ_NUM_COUNTER;
								}
							}
					}
					/*EVT_DATA_RX_MGMT_FRAME RxMgmtFrame;
					uint16 FrameLen;
					FrameLen = pRxDescriptor->MsgLen - sizeof(UMAC_RX_DESC);
					RxMgmtFrame.FrmLen = FrameLen;
					OS_MemoryCopy(RxMgmtFrame.FrmBuf, pUmacMgmt, FrameLen); */
					EvtActionFrame.Channel = pRxDescriptor->ChannelNumber;
					EvtActionFrame.FrameLength = pRxDescriptor->MsgLen - sizeof(UMAC_RX_DESC);
					EvtActionFrame.pFrame = (uint32 *) pUmacMgmt;
#if 0
					WFM_UMAC_2_UL_SendEvent(
								UmacHandle,
								WFM_EVT_RX_ACTION_FRAME,
								(pRxDescriptor->MsgLen - sizeof(UMAC_RX_DESC)),
								(uint8 *) pUmacMgmt,
								WFM_UMAC_EVT_NOT_RETRANSMIT
								);
#endif
					WFM_UMAC_2_UL_SendEvent(
								UmacHandle,
								WFM_EVT_RX_ACTION_FRAME,
								sizeof(EVT_DATA_ACTION_FRAME),
								(uint8 *) &EvtActionFrame,
								WFM_UMAC_EVT_NOT_RETRANSMIT
								);

					break;
				}
#if 0
				pActionFrame = (UMAC_ACTION_FRAME_BODY *) pUmacMgmt->FrameBody;
				switch (pActionFrame->OuiInfo.OuiSubType) {
				case D11_P2P_ACTION_TYPE_GO_NEG_REQ:
					WfmEvent = UMAC_GO_NEGO_REQ;
					break;

				case D11_P2P_ACTION_TYPE_GO_NEG_RESP:
					WfmEvent = UMAC_GO_NEGO_RESP;
					break;

				case D11_P2P_ACTION_TYPE_GO_NEG_CNF:
					LOG_EVENT(DBG_WFM_FRAMES, "D11_P2P_ACTION_TYPE_GO_NEG_CNF\n");
					WfmEvent = UMAC_GO_NEGO_REQ;
					break;

				case D11_P2P_ACTION_TYPE_P2P_INVTN_REQ:
					LOG_EVENT(DBG_WFM_FRAMES, "D11_P2P_ACTION_TYPE_P2P_INVTN_REQ\n");
					WfmEvent = UMAC_INVTN_REQ;
					break;

				case D11_P2P_ACTION_TYPE_P2P_INVTN_RESP:
					LOG_EVENT(DBG_WFM_FRAMES, "D11_P2P_ACTION_TYPE_P2P_INVTN_RESP\n");
					WfmEvent = UMAC_INVTN_RESP;
					break;

				case D11_P2P_ACTION_TYPE_DEV_DISCVRBLTY_REQ:
					LOG_EVENT(DBG_WFM_FRAMES, "D11_P2P_ACTION_TYPE_DEV_DISCVRBLTY_REQ\n");
					break;

				case D11_P2P_ACTION_TYPE_DEV_DISCVRBLTY_RESP:
					LOG_EVENT(DBG_WFM_FRAMES, "D11_P2P_ACTION_TYPE_DEV_DISCVRBLTY_RESP\n");
					break;

				case D11_P2P_ACTION_TYPE_PROVSN_DISCOVER_REQ:
					LOG_EVENT(DBG_WFM_FRAMES, "D11_P2P_ACTION_TYPE_PROVSN_DISCOVER_REQ\n");
					break;

				case D11_P2P_ACTION_TYPE_PROVSN_DISCOVER_RESP:
					LOG_EVENT(DBG_WFM_FRAMES, "D11_P2P_ACTION_TYPE_PROVSN_DISCOVER_RESP\n");
					break;
				} /* switch (pActionFrame->OuiInfo.OuiSubType) */
			break;
#endif
#endif	/* P2P_MINIAP_SUPPORT */
			default:
				break;
			} /* switch (pActionFrame->category) */
#if MGMT_FRAME_PROTECTION
		}
#endif	/* MGMT_FRAME_PROTECTION */

	} /* switch(SubType) */

#if 0	/* P2P_MINIAP_SUPPORT */
	if ((WfmEvent != UMAC_MAX_EVT) && (WfmEvent != UMAC_AUTH_RESP_SUCCESS))
		WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, WfmEvent, (void *)pRxDescriptor);
#else
	if (WfmEvent != UMAC_MAX_EVT)
		WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, WfmEvent, (void *)pRxDescriptor);
#endif	/* P2P_MINIAP_SUPPORT */
	else {
		WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
		/*
		   We are not interested in this packet, so no point in
		   scheduling UMAC task. Release the RxDescriptor and
		   Buffer here.
		 */
		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
		UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

		OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
		pUmacInstance->NumRxBufPending[linkId]--;
		OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
	} /*  if( WfmEvent ) */

} /* end WFM_UMAC_ProcessMgmtFrames() */

static void WFM_UMAC_ProcessCntrlFrames(UMAC_HANDLE UmacHandle,
					uint8 SubType,
					UMAC_RX_DESC *pRxDescriptor)
{
	/*WFM_UMAC_CTRL_PS_POLL_FRAME *pPsPoll = NULL;*/
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 umacEvent = UMAC_MAX_EVT;
	uint8 linkId;
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);

	LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_ProcessCntrlFrames()\n");

	switch (SubType) {

	case WFM_UMAC_CNTRL_BLK_ACK_REQ:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_CNTRL_BLK_ACK_REQ\n");

		break;
	case WFM_UMAC_CNTRL_BLK_ACK:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_CNTRL_BLK_ACK\n");

		break;
#if P2P_MINIAP_SUPPORT
	case WFM_UMAC_CNTRL_PS_POLL:
		/* The AID value always has its two MSBs each set to 1 */
		/*pPsPoll = (WFM_UMAC_CTRL_PS_POLL_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
		linkId = pPsPoll->Aid & 0x3FFF;*/
		if (linkId > 0) {
			if (pUmacInstance->sta[linkId].state != STA_STATE_ASSOCIATED) {
				/*
				   Deauth STA as we have received a PS poll from
				   a non-associated STA
				*/
				WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_DISCONNECT_STA, (void *)pRxDescriptor);
				goto exit_handler;
			}

			pUmacInstance->sta[linkId].UmacConHandle.linkId = linkId;
			pUmacInstance->sta[linkId].UmacConHandle.UmacHandle = UmacHandle;

			OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId]);
			OS_START_TIMER(
				pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[linkId],
				UMAC_CLIENT_INACTIVITY_TIMEOUT,
				(UMAC_CONNECTION_HANDLE *) &(pUmacInstance->sta[linkId].UmacConHandle)
				);

			if (pUmacInstance->sta[linkId].numPendingFrames[AP_BUFF_TYPES_LEGACY]) {
				umacEvent = UMAC_PS_DLVR_LEGACY_DATA;
				/* b12-b15 are used for Host interface */
				/*
				  Use b15 to indicate that it is triggered by
				  PS-Poll frame
				*/
				pRxDescriptor->MsgId |= 0x8000;
			} else
				umacEvent = UMAC_SEND_NULL_DATA;

			WFM_UMAC_EXEC_STATE_MACHINE(
						UmacHandle,
						umacEvent,
						(void *)pRxDescriptor
						);
			LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_CNTRL_PS_POLL\n");
		}

		break;
#endif	/* P2P_MINIAP_SUPPORT */
	case WFM_UMAC_CNTRL_RTS:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_CNTRL_RTS\n");

		break;
	case WFM_UMAC_CNTRL_CTS:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_CNTRL_CTS\n");

		break;
	case WFM_UMAC_CNTRL_ACK:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_CNTRL_ACK\n");

		break;
	case WFM_UMAC_CNTRL_CF_END:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_CNTRL_CF_END\n");

		break;
	case WFM_UMAC_CNTRL_CF_END_CF_ACK:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_CNTRL_CF_END_CF_ACK\n");

		break;
	default:
		LOG_EVENT(DBG_WFM_FRAMES, "Unknown Control Frame\n");

	} /* switch( SubType ) */
#if 0
	if (umacEvent != UMAC_MAX_EVT) {
		WFM_SCHEDULE_LMAC_TO_UMAC(
					UmacHandle,
					umacEvent,
					(void *)pRxDescriptor
					);
	} else {
		/*
		   We are not interested in this packet, so
		   release the RxDescriptor and Buffer here.
		 */
		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
		UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

		OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
		pUmacInstance->NumRxBufPending[linkId]--;
		OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
	}
#endif
	if (umacEvent == UMAC_MAX_EVT) {
		/*
		   We are not interested in this packet, so
		   release the RxDescriptor and Buffer here.
		*/
		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
		UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

		OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
		pUmacInstance->NumRxBufPending[linkId]--;
		OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
	}
#if P2P_MINIAP_SUPPORT
 exit_handler:
#endif	/* P2P_MINIAP_SUPPORT */
	return;

} /* end WFM_UMAC_ProcessCntrlFrames() */

static void WFM_UMAC_ProcessDataFrames(UMAC_HANDLE UmacHandle,
				       uint8 SubType,
				       UMAC_RX_DESC *pRxDescriptor)
{
	uint8 i = 0;
	uint32 status;
	uint8 rxLinkId = 0;
	uint8 PSIndicate = 0;
#if P2P_MINIAP_SUPPORT
	uint8 txLinkId;
	uint8 anyClientDozing;
	WFM_UMAC_FRAME_HDR *pDot11hdr;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_CONNECTION_HANDLE UmacConHandle;

	uint8 priority;
	uint8 uapsdFlag = 0;
	rxLinkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
	pDot11hdr = (WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);

	UmacConHandle.UmacHandle = UmacHandle;
	UmacConHandle.linkId = rxLinkId;

	if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP) &&
		OS_MemoryEqual(&pDot11hdr->Address1[0], WFM_UMAC_MAC_ADDRESS_SIZE, &pUmacInstance->MacAddress[0], WFM_UMAC_MAC_ADDRESS_SIZE)) {
		for (i = 0; i < MAX_NUM_CLIENTS_SUPPORTED; i++) {
			if (OS_MemoryEqual(&pDot11hdr->Address2[0], WFM_UMAC_MAC_ADDRESS_SIZE, &pUmacInstance->sta[i].macAddr[0], WFM_UMAC_MAC_ADDRESS_SIZE) &&
				(pUmacInstance->sta[i].state == STA_STATE_ASSOCIATED))
				break;
			}

		if (i == MAX_NUM_CLIENTS_SUPPORTED) {
			/* Send the unicast De-auth frame to this STA */
			WFM_UMAC_DeAuthSTAUnicast(UmacHandle, &pDot11hdr->Address2[0]);
			}
	}

	if (pUmacInstance->sta[rxLinkId].isWMMEnabled) {
		priority = (uint8) *((uint8 *)pDot11hdr + sizeof(WFM_UMAC_FRAME_HDR)) & 0x0F;
		/*
		  Priority to UAPSD flag mapping as it comes in
		  association request
		*/
		switch (priority) {
		case WFM_802_1D_UP_BK:
		case WFM_802_1D_UP_RESERVED:
			uapsdFlag = 0x04;
			break;
		case WFM_802_1D_UP_BE:
		case WFM_802_1D_UP_EE:
			uapsdFlag = 0x08;
			break;
		case WFM_802_1D_UP_CL:
		case WFM_802_1D_UP_VI:
			uapsdFlag = 0x02;
			break;
		case WFM_802_1D_UP_VO:
		case WFM_802_1D_UP_NC:
			uapsdFlag = 0x01;
			break;
		} /* switch(priority). */
	} /* if(pUmacInstance->sta[rxLinkId].isWMMEnabled) */

	if (rxLinkId) {
		pUmacInstance->sta[rxLinkId].UmacConHandle.linkId = rxLinkId;
		pUmacInstance->sta[rxLinkId].UmacConHandle.UmacHandle = UmacHandle;
		OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[rxLinkId]);
		OS_START_TIMER(
			pUmacInstance->gVars.p.UmacTimers.pUMACInactivityTimer[rxLinkId],
			UMAC_CLIENT_INACTIVITY_TIMEOUT,
			(UMAC_CONNECTION_HANDLE *) &(pUmacInstance->sta[rxLinkId].UmacConHandle)
			);
	}

#endif	/*P2P_MINIAP_SUPPORT */
	LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_ProcessDataFrames()\n");
	switch (SubType) {
#if P2P_MINIAP_SUPPORT
	case WFM_UMAC_SUB_DATA_NULL:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_SUB_DATA_NULL\n");
		pDot11hdr = (WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
		if (pDot11hdr->FrameControl & UMAC_FC_PWR_MGMT) {
			UMAC_CHANGE_STA_PS_STATE(UmacHandle, rxLinkId, STA_PS_STATE_DOZE);
			UMAC_CHANGE_STA_PS_STATE(UmacHandle, DEFAULT_LINK_ID, STA_PS_STATE_DOZE);

			OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
			pUmacInstance->NumRxBufPending[rxLinkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
			UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
		} else {
			UMAC_CHANGE_STA_PS_STATE(UmacHandle, rxLinkId, STA_PS_STATE_AWAKE);
			anyClientDozing = UMAC_Any_Client_Dozing(UmacHandle);
			if (FALSE == anyClientDozing)
				UMAC_CHANGE_STA_PS_STATE(UmacHandle, DEFAULT_LINK_ID, STA_PS_STATE_AWAKE);
			if (pUmacInstance->sta[rxLinkId].numPendingFrames[AP_BUFF_TYPES_LEGACY]) {
				/*
				  Schedule delivery of buffered frames
				*/
				WFM_SCHEDULE_LMAC_TO_UMAC(
					UmacHandle,
					UMAC_PS_DLVR_LEGACY_DATA,
					(void *)pRxDescriptor
					);
			} else {
				OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
				pUmacInstance->NumRxBufPending[rxLinkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
				UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
			}

		}

		break;

	case WFM_UMAC_SUB_NULL_QOS:
		LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_SUB_NULL_QOS\n");
		pDot11hdr = (WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
		if (pDot11hdr->FrameControl & UMAC_FC_PWR_MGMT) {

			if (pUmacInstance->sta[rxLinkId].PsState == STA_PS_STATE_AWAKE)
				PSIndicate = 1;

			UMAC_CHANGE_STA_PS_STATE(
						UmacHandle,
						rxLinkId,
						STA_PS_STATE_DOZE
						);

			UMAC_CHANGE_STA_PS_STATE(
						UmacHandle,
						DEFAULT_LINK_ID,
						STA_PS_STATE_DOZE
						);
		} else {
			UMAC_CHANGE_STA_PS_STATE(
						UmacHandle,
						rxLinkId,
						STA_PS_STATE_AWAKE
						);
			anyClientDozing = UMAC_Any_Client_Dozing(UmacHandle);
			if (FALSE == anyClientDozing) {
				UMAC_CHANGE_STA_PS_STATE(
						UmacHandle,
						DEFAULT_LINK_ID,
						STA_PS_STATE_AWAKE
						);
			}
			PSIndicate = 1;

		}

		if (pUmacInstance->sta[rxLinkId].qosInfo & uapsdFlag) {
			if (pUmacInstance->sta[rxLinkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED]) {
				/* Schedule delivery of buffered frames */
				WFM_SCHEDULE_LMAC_TO_UMAC(
					UmacHandle,
					UMAC_PS_DLVR_WMM_DATA,
					(void *)pRxDescriptor
					);
			} else if (!PSIndicate) {
				if (rxLinkId) {
					WFM_UMAC_EXEC_STATE_MACHINE(
						UmacHandle,
						UMAC_SEND_NULL_DATA,
						(void *)pRxDescriptor
						);
				}
			} else {
				OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
				pUmacInstance->NumRxBufPending[rxLinkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
				UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
			}
		} else if (pUmacInstance->sta[rxLinkId].qosInfo == 0) {
			if (pUmacInstance->sta[rxLinkId].numPendingFrames[AP_BUFF_TYPES_LEGACY]) {
				/* Schedule delivery of buffered frames */
				WFM_SCHEDULE_LMAC_TO_UMAC(
					UmacHandle,
					UMAC_PS_DLVR_LEGACY_DATA,
					(void *)pRxDescriptor
					);
			} else if (!PSIndicate) {
				if (rxLinkId) {
					WFM_UMAC_EXEC_STATE_MACHINE(
						UmacHandle,
						UMAC_SEND_NULL_DATA,
						(void *)pRxDescriptor
						);
				}
			} else {
				OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
				pUmacInstance->NumRxBufPending[rxLinkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
				UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
			}
		} else {
			OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
			pUmacInstance->NumRxBufPending[rxLinkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
			UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
		}
	break;
#endif	/* P2P_MINIAP_SUPPORT */

	default:
		/*
		   In the firmware version, Rx packet should be scheduled to
		   be processed for future.But in the driver version, Rx
		   packet should be processed immediatly.
		 */
#if P2P_MINIAP_SUPPORT
		/*
		  Data frame destined for another STA would be received at
		  Non-Zero linkId
		*/
		if (rxLinkId) {
			OS_MemoryReset(((uint8 *)pRxDescriptor - 4), 4);
			OS_MemoryCopy(((uint8 *)pRxDescriptor - 4), (uint8 *)&pUmacInstance->pDriverInfo, 4);

			if ((pDot11hdr->FrameControl & UMAC_FC_PWR_MGMT)	/* &&
										   (pUmacInstance->sta[rxLinkId].PsState == STA_PS_STATE_AWAKE) */
			    ) {
				UMAC_CHANGE_STA_PS_STATE(UmacHandle, rxLinkId, STA_PS_STATE_DOZE);
				UMAC_CHANGE_STA_PS_STATE(UmacHandle, DEFAULT_LINK_ID, STA_PS_STATE_DOZE);
				if (pUmacInstance->sta[rxLinkId].isWMMEnabled && (pUmacInstance->sta[rxLinkId].qosInfo & uapsdFlag)) {
					if (pUmacInstance->sta[rxLinkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED]) {
						pUmacInstance->sta[rxLinkId].hiMsgHdr.MsgId = rxLinkId<<6;
						/* Schedule delivery of buffered frames */
						/* WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_PS_DLVR_WMM_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr)); */
						WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_PS_DLVR_WMM_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr));
					} else {
						pUmacInstance->sta[rxLinkId].hiMsgHdr.MsgId = rxLinkId<<6;
						/* WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle,	UMAC_SEND_NULL_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr)); */
						WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle,	UMAC_SEND_NULL_DATA, (void *)pRxDescriptor);
						/* WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_SEND_NULL_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr)); */
					}
				}	/*if(pUmacInstance->sta[rxLinkId].isWMMEnabled) */
			} else if (!(pDot11hdr->FrameControl & UMAC_FC_PWR_MGMT) && (pUmacInstance->sta[rxLinkId].PsState == STA_PS_STATE_DOZE)
			    ) {
				UMAC_CHANGE_STA_PS_STATE(UmacHandle, rxLinkId, STA_PS_STATE_AWAKE);
				anyClientDozing = UMAC_Any_Client_Dozing(UmacHandle);
				if (FALSE == anyClientDozing)
					UMAC_CHANGE_STA_PS_STATE(UmacHandle, DEFAULT_LINK_ID, STA_PS_STATE_AWAKE);
				/* Schedule delivery of buffered frames */
				if (pUmacInstance->sta[rxLinkId].isWMMEnabled) {
					if (pUmacInstance->sta[rxLinkId].numPendingFrames[AP_BUFF_TYPES_DLVRY_ENABLED] && (pUmacInstance->sta[rxLinkId].qosInfo & uapsdFlag)
						) {
						pUmacInstance->sta[rxLinkId].hiMsgHdr.MsgId = rxLinkId<<6;
						/* Schedule delivery of buffered frames */
						/* WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_PS_DLVR_WMM_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr)); */
						WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_PS_DLVR_WMM_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr));
					}
					if (pUmacInstance->sta[rxLinkId].numPendingFrames[AP_BUFF_TYPES_LEGACY]) {
						pUmacInstance->sta[rxLinkId].hiMsgHdr.MsgId = rxLinkId<<6;
						/* Schedule delivery of buffered frames */
						/* WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_PS_DLVR_LEGACY_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr)); */
						WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_PS_DLVR_LEGACY_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr));
					}

				} else {
					if (pUmacInstance->sta[rxLinkId].numPendingFrames[AP_BUFF_TYPES_LEGACY]) {
						pUmacInstance->sta[rxLinkId].hiMsgHdr.MsgId = rxLinkId<<6;
						/* WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_PS_DLVR_LEGACY_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr)); */
						WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_PS_DLVR_LEGACY_DATA, (void *)&(pUmacInstance->sta[rxLinkId].hiMsgHdr));
					}
				}
			}

			if ((OS_MemoryEqual((void *)&pDot11hdr->Address1[0], WFM_MAC_ADDRESS_SIZE, (void *)&pDot11hdr->Address3[0], WFM_MAC_ADDRESS_SIZE))
				|| (!pUmacInstance->gVars.p.IntraBssBridging && IS_MULTICAST_MAC_ADDRESS(pDot11hdr->Address3))
				) {
				/* The packet has to be sent to stack */
				status = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_DATA_RX, pRxDescriptor);

				if (WFM_STATUS_FAILURE == status) {
					/*UMAC is not in a position to process this frame, so drop it */
					WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
					/* Release RxBuffer and RxDescriptor for these frames */
					WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

					/*UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);*/
					UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle, pRxDescriptor, pUmacInstance->pDriverInfo);
					UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

					OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
					pUmacInstance->NumRxBufPending[rxLinkId]--;
					OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

				} /* if( WFM_STATUS_PENDING ==  status) */
			} else {
				UMAC_GET_LINK_ID(UmacHandle, pDot11hdr->Address3, txLinkId);
				if (txLinkId) {
					if (pRxDescriptor->Status == WFM_STATUS_SUCCESS) {
						pRxDescriptor->MsgId &= ~UMAC_HI_MSG_LINK_ID;
						/* We can use the MsgId to communciate txLinkId */
						pRxDescriptor->MsgId |= txLinkId << 6;
						/* The STA to which the frame is to be Txed, is present in our STA Database */
						WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_FWD_FRM, (void *)pRxDescriptor);
						OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
						pUmacInstance->NumRxBufPending[rxLinkId]--;
						OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
					} else {
						WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_DATA_RX, (void *)pRxDescriptor);
					}

				} else if (IS_BROADCAST_MAC_ADDRESS(pDot11hdr->Address3) || IS_MULTICAST_MAC_ADDRESS(pDot11hdr->Address3)) {
					uint8 buffStore[BC_MC_BUFFER_SIZE];
					/* Broadcast/Multicast data needs to be forwarded and
					   also needs to be sent to network stack as well */
					if (pRxDescriptor->Status == WFM_STATUS_SUCCESS) {
						OS_MemoryCopy(buffStore, pRxDescriptor, BC_MC_BUFFER_SIZE);

						/* The BC/MC data memory is freed by UMAC and not by host driver */
						WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_DATA_RX, pRxDescriptor);

						OS_MemoryCopy(pRxDescriptor, buffStore, BC_MC_BUFFER_SIZE);

						pRxDescriptor->MsgId &= ~UMAC_HI_MSG_LINK_ID;

						/* The STA to which the frame is to be Txed, is present in our STA Database */
						WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_FWD_FRM, (void *)pRxDescriptor);
					} else {
						/*UMAC is not in a position to process this frame, so drop it */
						WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
						if (pRxDescriptor->Status == WSM_STATUS_MICFAILURE)
							LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ALWAYS, "WFM_UMAC_ProcessDataFrames(): Received WSM_STATUS_MICFAILURE\n");
						else if (pRxDescriptor->Status == WSM_STATUS_DECRYPTFAILURE)
							LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ALWAYS, "WFM_UMAC_ProcessDataFrames(): Received WSM_STATUS_DECRYPTFAILURE\n");
						else if (pRxDescriptor->Status == WSM_STATUS_NO_KEY_FOUND)
							LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ALWAYS, "WFM_UMAC_ProcessDataFrames(): Received WSM_STATUS_NO_KEY_FOUND\n");
						else
							LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ALWAYS, "WFM_UMAC_ProcessDataFrames(): General Failure\n");

						UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle, pRxDescriptor, pUmacInstance->pDriverInfo);

						OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
						pUmacInstance->NumRxBufPending[rxLinkId]--;
						OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
					}


					/*Else part is commented as the data will go twice to the stack*/
					/*else
						WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_DATA_RX, (void *)pRxDescriptor);*/

				} else {
					/* The STA is not connected to us */
					/* Pass this frame to host so that host
					   will take care to release buffer*/
					/* WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

					UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
					UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

					OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
					pUmacInstance->NumRxBufPending[rxLinkId]--;
					OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock); */

					WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_DATA_RX, pRxDescriptor);
				}


			}
		} /* if(rxLinkId) */
		else {
#endif	/* P2P_MINIAP_SUPPORT */
			status = WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_DATA_RX,
							pRxDescriptor
							);

			if (WFM_STATUS_PENDING == status) {
				/*
				  UMAC is not in a position to process this
				  frame, so drop it
				*/
				WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
				/*
				  Release RxBuffer and RxDescriptor for
				  these frames
				*/
				WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

				UMAC_LL_RELEASE_RXBUFF(
							UmacHandle,
							pRxDescriptor
							);

				UMAC_LL_RELEASE_RXDESC(
							UmacHandle,
							pRxDescriptor
							);

				OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
				pUmacInstance->NumRxBufPending[rxLinkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

			} /* if( WFM_STATUS_PENDING ==  status) */
#if P2P_MINIAP_SUPPORT
		} /* if(rxLinkId) */
#endif	/* P2P_MINIAP_SUPPORT */

	} /* switch( SubType ) */
	return;

} /* end WFM_UMAC_ProcessDataFrames() */
