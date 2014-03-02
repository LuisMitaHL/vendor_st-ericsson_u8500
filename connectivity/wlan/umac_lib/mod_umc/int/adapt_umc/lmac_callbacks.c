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
 * \file lmac_callbacks.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: lmac_callbacks.c
 * \brief
 * The file contains the APIs to send the messages from LMAC to UMAC core.
 * \ingroup Upper_MAC_Core
 * \date 31-Aug-2007
 */

/******************************************************************************
		       INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "lmac_callbacks.h"
#include "umac_sm.h"

#include "umac_proc_lmac_frames.h"
#include "umac_internal.h"
#include "umac_frames.h"
#include "umac_task.h"
#include "umac_dbg.h"

#include "umac_ll_if.h"
#include "umac_if.h"
#include "umac_mib.h"

/******************************************************************************
			    EXTERNAL DATA REFERENCES
******************************************************************************/

extern UMAC_FRAME_HANDLER UMAC_InComing_Frame_Hndlr[];

extern UMAC_FSM_HANDLER UMAC_StateEventTable[UMAC_MAX_STATE][UMAC_MAX_EVT];

/******************************************************************************
 * NAME:	LLtoUMAC_ScanComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the Scan Complete Indication to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pScanComplete Pointer to SCAN_COMPLETE structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_ScanComplete(UMAC_HANDLE UmacHandle, SCAN_COMPLETE *pScanComplete)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_SCAN, "FRM-LMAC: LLtoUMAC_ScanComplete()\n");

	/*copying this information as memory of pScanComplete will be invalid */
	/*after this function returns */
	OS_MemoryCopy(&pUmacInstance->ScanCompleteResult, pScanComplete, sizeof(pUmacInstance->ScanCompleteResult));

	if (pUmacInstance->autoChanSel) {
		/* Select the Best Channel */
		pUmacInstance->Channelnumber = UMAC_FIND_IDLE_CHANNEL(UmacHandle);

		UMI_DbgPrint("No. of BSS in Channel 1=%d\n", pUmacInstance->bss_count[1]);
		UMI_DbgPrint("No. of BSS in Channel 6=%d\n", pUmacInstance->bss_count[6]);
		UMI_DbgPrint("No. of BSS in Channel 11=%d\n", pUmacInstance->bss_count[11]);
		UMI_DbgPrint("Least Congested Channel =%d\n", pUmacInstance->Channelnumber);

		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
	} else
	WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_SCAN_COMPLETED, &pUmacInstance->ScanCompleteResult);

} /*end LLtoUMAC_ScanComplete() */

/******************************************************************************
 * NAME:	LLtoUMAC_MeasurementComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the Scan Complete Indication to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMeasurementComplete Pointer to MEASUREMENT_COMPLETE structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_MeasurementComplete(UMAC_HANDLE UmacHandle,
				  WSM_HI_MEASURE_CMPL_IND *pMeasurementComplete)
{
#if DOT11K_SUPPORT
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 UmacEvent = UMAC_MEASUREMENT_REPORT;

	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LMActoUMAC_MeasurementComplete()\n");

	/*Copy the Measurement Complete structure here */
	OS_MemoryCopy(&pUmacInstance->dot11k_Meas_Params.measurementComplInd,
		pMeasurementComplete,
		(sizeof(WSM_HI_MEASURE_CMPL_IND) - sizeof(WSM_MEAS_REPORT))
		);

	switch (pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementType) {
	case WSM_MEASURE_TYPE_CHANNEL_LOAD:
		{
			OS_MemoryCopy((uint8 *)&pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.ChannelLoadResults, (uint8 *)&pMeasurementComplete->MeasurementReport.ChannelLoadResults, pMeasurementComplete->MeasurementReportLength);
			break;
		}
	case WSM_MEASURE_TYPE_NOISE_HISTOGRAM:
		{
			OS_MemoryCopy((uint8 *)&pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.NoiseHistogramResults, (uint8 *)&pMeasurementComplete->MeasurementReport.NoiseHistogramResults, pMeasurementComplete->MeasurementReportLength);
			break;
		}
	case WSM_MEASURE_TYPE_BEACON:
		{
			OS_MemoryCopy((uint8 *)&pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.BeaconResults, (uint8 *)&pMeasurementComplete->MeasurementReport.NoiseHistogramResults, pMeasurementComplete->MeasurementReportLength);
			break;
		}
	case WSM_MEASURE_TYPE_STAT_STATISTICS:
		{
			OS_MemoryCopy((uint8 *)&pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.StaStatsResults, (uint8 *)&pMeasurementComplete->MeasurementReport.StaStatsResults, pMeasurementComplete->MeasurementReportLength);
			break;
		}
	case WSM_MEASURE_TYPE_LINK_MEASUREMENT:
		{
			OS_MemoryCopy((uint8 *)&pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.LinkMeasurementResults, (uint8 *)&pMeasurementComplete->MeasurementReport.LinkMeasurementResults, pMeasurementComplete->MeasurementReportLength);
			UmacEvent = UMAC_SEND_LINK_MSRMT_REPORT;
			break;
		}
	}

	WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UmacEvent,
			&pUmacInstance->dot11k_Meas_Params.measurementComplInd
			);

#endif /* DOT11K_SUPPORT */
} /*end LLtoUMAC_MeasurementComplete() */

/******************************************************************************
 * NAME:	LLtoUMAC_SetPmModeComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends Set PSMode Complete Indication to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pSetPsModeComplete Pointer to SET_PS_MODE_COMPLETE structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SetPmModeComplete(UMAC_HANDLE UmacHandle,
				SET_PM_MODE_COMPLETE *pSetPsModeComplete)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	OS_ASSERT(pUmacInstance);
	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_SetPsModeComplete()\n");
	LOG_DATA2(DBG_WFM_IN_OUT_MSGS, "PS_MODE = %d, RESULT = %d  ", pSetPsModeComplete->PmMode, pSetPsModeComplete->Result);

	pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = pSetPsModeComplete->Result;
	pUmacInstance->IsPmModeQueued = FALSE;
	if (WSM_STATUS_FAILURE == pSetPsModeComplete->Result) {
		LOG_EVENT(DBG_WFM_ERROR, "SET PM MODE Failed\n");
		LOG_DATA2(DBG_WFM_ERROR, "PS_MODE = %d, RESULT = %d\n", pSetPsModeComplete->PmMode, pSetPsModeComplete->Result);
		OS_ASSERT(0);
	} /*if(pSetPsModeComplete->Result) */
	else {
		/* If PmMode is Undetermined, try again one more time */
		if (WFM_PM_MODE_STATE_UNDETERMINED == pSetPsModeComplete->PmMode) {
			if (pUmacInstance->writeMibPS) {
				/*We have already sent Set PM mode twice but
				got undetermined status so reset */
				pUmacInstance->writeMibPS = 0;
				pUmacInstance->sendDisassoc = TRUE;
				WFM_SCHEDULE_LMAC_TO_UMAC(
					UmacHandle,
					UMAC_ASYNC_MGMT,
					NULL
					);
			} else {
				pUmacInstance->writeMibPS++;
				WFM_UMAC_SendReqToWsm_SetPsMode(UmacHandle, &pUmacInstance->gVars.p.PsMode);
			}
		} else {
			pUmacInstance->writeMibPS = 0;
			WFM_SCHEDULE_LMAC_TO_UMAC(
				UmacHandle,
				UMAC_ASYNC_MGMT,
				NULL
				);
			/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
		}
	}
} /*end LLtoUMAC_SetPmModeComplete() */

/******************************************************************************
 * NAME:	LLtoUMAC_SwitchChInd
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends Switch Channel Complete Indication to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pSwitchChInd Pointer to WSM_HI_SWITCH_CHANNEL_IND structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SwitchChInd(UMAC_HANDLE UmacHandle,
			  WSM_HI_SWITCH_CHANNEL_IND *pSwitchChInd)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 Event = WFM_EVT_MAX;

	if (pUmacInstance->SwitchChannelReq == UMAC_SC_REQ_EXTERNAL) {
		switch (pSwitchChInd->State) {
		case WSM_HI_SC_STATE_COMPLETED:
		case WSM_HI_SC_STATE_COMPLETED_EARLY:
			pUmacInstance->ChannelChange.ChannelState = UMAC_CHANNEL_STATE_NONE;
			Event = WFM_EVT_CC_STATE_COMPLETED;
			WFM_SCHEDULE_LMAC_TO_UMAC(
						UmacHandle,
						UMAC_DLVR_REQUEUE_DATA,
						NULL
						);
			UMAC_LL_REQ_SCHEDULE_TX(UmacHandle);
			break;
		case WSM_HI_SC_STATE_ABORTED:
			pUmacInstance->ChannelChange.ChannelState = UMAC_CHANNEL_STATE_NONE;
			Event = WFM_EVT_CC_STATE_ABORTED;
			UMAC_LL_REQ_SCHEDULE_TX(UmacHandle);
			WFM_SCHEDULE_LMAC_TO_UMAC(
						UmacHandle,
						UMAC_DLVR_REQUEUE_DATA,
						NULL
						);
			break;
		case WSM_HI_SC_STATE_SWITCHING:
			pUmacInstance->ChannelChange.ChannelState = UMAC_CHANNEL_STATE_CHANGED;
			Event = WFM_EVT_CC_STATE_SWITCHING;
			break;
		}

		if (Event != WFM_EVT_MAX) {
			WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					Event,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		} else {
			LOG_EVENT(DBG_WFM_ERROR, "Rcv incorrect Switch Channel IND\n");
		}
	}
} /*end LLtoUMAC_SwitchChInd() */

/******************************************************************************
 * NAME:	LLtoUMAC_ReceiveFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * All the recieved frames will be passed up to UMAC through
 * this callback function. This callback will perform some basic checkings on
 * the validity of the frame recieved and if it qualifies these checks, queues
 * the frame to UMAC for further processing.
 * \param  UmacHandle UMAC handle
 * \param pRxDescriptor Pointer to UMAC_RX_DESC structure
 * \param pDriverInfo Pointer to driver info.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_ReceiveFrame(UMAC_HANDLE UmacHandle,
			   UMAC_RX_DESC *pRxDescriptor,
			   void *pDriverInfo)
{
	WFM_UMAC_FRAME_HDR *pDot11hdr;
	uint8 Type;
	uint8 SubType;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 linkId = 0;
#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((pRxDescriptor->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);
#endif	/*P2P_MINIAP_SUPPORT */

	LOG_EVENT(DBG_WFM_FRAMES | DBG_WFM_IN_OUT_MSGS | DBG_WFM_RX, "FRM-LMAC: Rx Frame - LLtoUMAC_ReceiveFrame()\n");

	OS_ASSERT(pRxDescriptor);

	WFM_UMAC_DBG_STORE_RX_DESC(pRxDescriptor);

	if (pRxDescriptor->Flags & WFM_UMAC_INSTANCE_FLAGS_BCN_PBRSP_TSF_PRESENT)
		pRxDescriptor->MsgLen = pRxDescriptor->MsgLen - WFM_UMAC_INSTANCE_TSF_TIMESTAMP_SIZE;

	pDot11hdr = (WFM_UMAC_FRAME_HDR *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);

	if (((pDot11hdr->FrameControl & 0x0100)
		|| D0_ADDR_EQ(pDot11hdr->Address2, pUmacInstance->MacAddress))
#if P2P_MINIAP_SUPPORT
		&& (pUmacInstance->operatingMode < OPERATING_MODE_AP)
#endif
	    ) {
		/*
		   1] If TODS bit is set,the packet is for an AP,
		   2] If Source Address is same as own MacAddress, No need to
		   Process the packet
		 */

		/* Release RxBuffer and RxDescriptor for these frames */

		WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

		UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle, pRxDescriptor, pDriverInfo);
		UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

		return;
	}

	if ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING) && (pUmacInstance->UmacPreviousState == UMAC_INITIALIZED)) {
		WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

		UMAC_LL_RELEASE_FWD_RX_BUFF(UmacHandle, pRxDescriptor, pDriverInfo);
		UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

		return;
	}

	OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);

	pUmacInstance->NumRxBufPending[linkId]++;

	OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);

	/* b2 and b3 constitutes Type */
	Type = (uint8) ((pDot11hdr->FrameControl >> 2) & 0x0003);

	/*b4,b5,b6 and b7 forms SubType */
	SubType = (uint8) ((pDot11hdr->FrameControl >> 4) & 0x000F);

	if (Type <= WFM_UMAC_DATA) {
		pUmacInstance->pDriverInfo = pDriverInfo;
		/* OS_MemoryReset(((uint8 *)pRxDescriptor - 4),4);
		OS_MemoryCopy(((uint8 *)pRxDescriptor - 4),(uint8 *)&pDriverInfo, 4); */
		UMAC_InComing_Frame_Hndlr[Type] (UmacHandle, SubType, pRxDescriptor);
	} else {
		LOG_EVENT(DBG_WFM_FRAMES, "Invalid UMAC Frame Type\n");

		WFM_UMAC_DBG_REMOVE_RX_DESC(pRxDescriptor);

		/* Release RxBuffer and RxDescriptor for these frames */
		UMAC_LL_RELEASE_RXBUFF(UmacHandle, pRxDescriptor);
		UMAC_LL_RELEASE_RXDESC(UmacHandle, pRxDescriptor);

		OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);

		pUmacInstance->NumRxBufPending[linkId]--;

		OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);
	}
} /* end LLtoUMAC_ReceiveFrame() */

/******************************************************************************
 * NAME:	LLtoUMAC_TransmitFrameConfirm
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This callback informs UMAC about the status
 * of a frame which has been submitted to LMAC Tx queue by UMAC earlier.
 * The Tx descriptor will be queued to UMAC for further processing.
 * \param  UmacHandle UMAC handle
 * \param pTxDescriptor Pointer to UMAC_TX_CNF_DESC structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_TransmitFrameConfirm(UMAC_HANDLE UmacHandle,
				   UMAC_TX_CNF_DESC *pTxDescriptor)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status;

#if P2P_MINIAP_SUPPORT
	if (pTxDescriptor->PacketId & UMAC_PACKETID_PS_BUFFRED_FRAME) {
		if (pTxDescriptor->Status == WFM_FRAME_REQUEUE)
			pUmacInstance->UpdateNewTIMInfo = FALSE;

		pUmacInstance->PsBuffDataCount--;
		if ((!pUmacInstance->PsBuffDataCount) && (pUmacInstance->UpdateNewTIMInfo == TRUE)) {
			pUmacInstance->UpdateNewTIMInfo = FALSE;
			OS_LOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
			if (pUmacInstance->updateTimIeInProgress == FALSE) {
				pUmacInstance->updateTimIeInProgress = TRUE;
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
				WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_UPDATE_TIM_IE, NULL);
			} else
				OS_UNLOCK(pUmacInstance->gVars.p.UpdateTimIeLock);
		}
	}

	if (pTxDescriptor->PacketId & UMAC_FWDED_FRAME)
		status = WFM_UMAC_EXEC_STATE_MACHINE(
						UmacHandle,
						UMAC_FWD_FRM_TX_CMPL,
						pTxDescriptor
						);
	else {
#endif	/*P2P_MINIAP_SUPPORT */
		status = WFM_UMAC_EXEC_STATE_MACHINE(
						UmacHandle,
						UMAC_TX_COMPLETED,
						pTxDescriptor
						);
#if P2P_MINIAP_SUPPORT
	}
#endif	/*P2P_MINIAP_SUPPORT */

	if (status == WFM_STATUS_REQ_REJECTED) {
		LOG_DATA(DBG_WFM_ERROR, "LLtoUMAC_TransmitFrameConfirm() : PacketId : %x", pTxDescriptor->PacketId);
		UMAC_RELEASE_TX_CNF_FRAME(UmacHandle, pTxDescriptor);
	}

} /*end LLtoUMAC_TransmitFrameConfirm() */

/******************************************************************************
 * NAME:LLtoUMAC_ResetConfirm
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the reset confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pResetCnf: Pointer to reset confirmation
 * \returns void
 *****************************************************************************/
void LLtoUMAC_ResetConfirm(UMAC_HANDLE UmacHandle,
			   LMAC_RESET_CONFIRMATION *pResetCnf)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

#if P2P_MINIAP_SUPPORT
	uint8 linkId;
	linkId = (uint8) ((pResetCnf->MessageId & UMAC_HI_MSG_LINK_ID) >> 6);
#endif	/*P2P_MINIAP_SUPPORT */

	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_ResetConfirm()\n");

	if (pUmacInstance->gVars.p.ForceStopMode == UMAC_FORCE_STOP_RESET) {
		/*Its a forced Reset */
		pUmacInstance->gVars.p.ForceStopMode = UMAC_FORCE_STOP_OFF;

		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_STOP_COMPLETED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);

	} else if (pUmacInstance->ResetReaminch == 1) {
		pUmacInstance->ResetReaminch = 0;
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
	} else {
#if P2P_MINIAP_SUPPORT
		if ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING) && (linkId == DEFAULT_LINK_ID))
			UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
		/* If linkId is non zero, it was reset request for specific STA */
		if (linkId) {
			/*pUmacInstance->sta[linkId].hiMsgHdr.MsgId = linkId<<6;
			WFM_SCHEDULE_LMAC_TO_UMAC(
				UmacHandle,
				UMAC_UNLINK_STA_SUCCESS,
				(void *)&(pUmacInstance->sta[linkId].hiMsgHdr)
				);*/
			WFM_SCHEDULE_LMAC_TO_UMAC(
				UmacHandle,
				UMAC_ASYNC_MGMT,
				NULL
				);
		} else {
			WFM_SCHEDULE_LMAC_TO_UMAC(
						UmacHandle,
						UMAC_UNJOIN_SUCCESS,
						(void *)pResetCnf->Result
						);
#else	/* P2P_MINIAP_SUPPORT */
			/*Its normal Disconnect */
			WFM_SCHEDULE_LMAC_TO_UMAC(
						UmacHandle,
						UMAC_UNJOIN_SUCCESS,
						(void *)pResetCnf->Result
						);
#endif	/*P2P_MINIAP_SUPPORT */

#if P2P_MINIAP_SUPPORT
		}
#endif	/*P2P_MINIAP_SUPPORT */
	}
} /*end LLtoUMAC_ResetComplete() */

/******************************************************************************
 * NAME:	LLtoUMAC_JoinConfirm
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the join confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pJoinConfirm Pointer to JOIN_CONFIRM structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_JoinConfirm(UMAC_HANDLE UmacHandle, JOIN_CONFIRM *pJoinConfirm)
{
	WFM_STATUS_CODE wfmStatus = WFM_STATUS_SUCCESS;
	uint16 UmacEvent = 0;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_DATA(DBG_WFM_IN_OUT_MSGS | DBG_WFM_ASSOC, "FRM-LMAC: LLtoUMAC_JoinConfirm():%d\n", pJoinConfirm->Status);

	/*copying this information as memory of pJoinComplete will be invalid */
	/*after this function returns */
	OS_MemoryCopy(
			&pUmacInstance->JoinConfirmResult,
			pJoinConfirm,
			sizeof(pUmacInstance->JoinConfirmResult)
			);

	if (
#if P2P_ENABLE
		(pUmacInstance->UmacCurrentState == UMAC_GROUP_FORMING)
#else
		0
#endif	/* P2P_ENABLE */
#if FT_SUPPORT
		|| (pUmacInstance->FtFlags & (UMAC_FT_FLAG_AUTH_JOIN_BSS_BIT | UMAC_FT_FLAG_AUTH_JOINBACK_BSS_BIT))
#endif	/* FT_SUPPORT */
		) {
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
	} else {
		wfmStatus = (WFM_STATUS_CODE) umac_interpret_wsm_status(pJoinConfirm->Status);

		if (WFM_STATUS_SUCCESS != wfmStatus)
			UmacEvent = UMAC_JOIN_FAILED;
		else
			UmacEvent = UMAC_JOIN_SUCCESS;

		WFM_SCHEDULE_LMAC_TO_UMAC(
					UmacHandle,
					UmacEvent,
					&pUmacInstance->JoinConfirmResult
					);
	}
} /*end LLtoUMAC_JoinConfirm() */

/******************************************************************************
 * NAME:	LLtoUMAC_EventIndication
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the event indication to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pEventIndication Pointer to EVENT_INDICATION structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_EventIndication(UMAC_HANDLE UmacHandle,
			      EVENT_INDICATION *pEventIndication)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
#if PS_MODE_ERROR_HANDLING
	SMGMT_REQ_PARAMS *pSmgmtReq;
#endif
	uint32 Result = WFM_STATUS_SUCCESS;
#if SEND_PROBE
	uint32 ProbeReqStatus = WFM_STATUS_SUCCESS;
	uint32 UniProbeReqStatus = WFM_STATUS_SUCCESS;
#endif /* SEND_PROBE */

	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_EventIndication()\n");
	LOG_DATA(DBG_WFM_IN_OUT_MSGS, "EventID = %d\n", pEventIndication->EventId);

	switch (pEventIndication->EventId) {
	case LMAC_EVENT_IND_ERROR:
		LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "LMAC_EVENT_IND_ERROR\n");

		break;
	case LMAC_EVENT_IND_BSSLOST:
		LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC | DBG_WFM_ASSOC, "LMAC_EVENT_IND_BSSLOST\n");
		/* Reset the variable. Will be set when the event is sent to the driver */
		pUmacInstance->BssLostEventRcvd = 0;
		pUmacInstance->BssRegainedEventRcvd = 0;
		/* pUmacInstance->BssLostEventRcvd = 1;
		WFM_UMAC_2_UL_SendEvent(
		UmacHandle,
		WFM_EVT_BSS_LOST,
		(sizeof(EVENT_INDICATION) - 4),
		((uint8 *) pEventIndication + 4),
		WFM_UMAC_EVT_NOT_RETRANSMIT
		); */
#if SEND_PROBE
		if (!pUmacInstance->isProbeReqFrame) {
			pUmacInstance->isProbeReqFrame = 1;
			pUmacInstance->eventIndicationData = pEventIndication->EventData;


			UniProbeReqStatus = WFM_UMAC_EXEC_STATE_MACHINE(
					UmacHandle,
					UMAC_SEND_PROBE_REQ,
					NULL
					);
			Result = WFM_UMAC_EXEC_STATE_MACHINE(
					UmacHandle,
					UMAC_SEND_PROBE_REQ,
					NULL
					);

			ProbeReqStatus = WFM_UMAC_EXEC_STATE_MACHINE(
					UmacHandle,
					UMAC_SEND_PROBE_REQ,
					NULL
					);

			/*
			Schedule a time to check for Probe response timeout
			*/

			if (((Result != WFM_STATUS_REQ_REJECTED) && (Result != WFM_STATUS_OUT_OF_RESOURCES)) ||
				((ProbeReqStatus != WFM_STATUS_REQ_REJECTED) && (ProbeReqStatus != WFM_STATUS_OUT_OF_RESOURCES)) ||
				((UniProbeReqStatus != WFM_STATUS_REQ_REJECTED) && (UniProbeReqStatus != WFM_STATUS_OUT_OF_RESOURCES))) {

					OS_START_TIMER(
						pUmacInstance->gVars.p.UmacTimers.pBssLostTimer,
						WFM_UMAC_CHECK_FOR_PRBOE_RESP_TIMEOUT,
						UmacHandle
						);
				} else {
					pUmacInstance->isProbeReqFrame = 0;
				}
		}
#else
		if (!pUmacInstance->isNullDataFrame) {
			pUmacInstance->isNullDataFrame = 1;
			pUmacInstance->nullFrmPktId = 0;
			pUmacInstance->eventIndicationData = pEventIndication->EventData;

			Result = WFM_UMAC_EXEC_STATE_MACHINE(
				UmacHandle,
				UMAC_SEND_NULL_DATA,
				NULL
				);
		/*
		Schedule a time to check for Null data frame
		confirmation.
		*/
			if ((Result != WFM_STATUS_REQ_REJECTED) && (Result != WFM_STATUS_OUT_OF_RESOURCES)) {
				OS_START_TIMER(
					pUmacInstance->gVars.p.UmacTimers.pBssLostTimer,
					WFM_UMAC_CHECK_FOR_NULL_CNF_TIMEOUT,
					UmacHandle
					);
			} else {
				pUmacInstance->isNullDataFrame = 0;
			}
		}
#endif
		break;
	case LMAC_EVENT_IND_BSSREGAINED:
		LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC | DBG_WFM_ASSOC, "LMAC_EVENT_IND_BSSREGAINED\n");
		pUmacInstance->BssRegainedEventRcvd = 1;
		if (pUmacInstance->BssLostEventRcvd)
			WFM_UMAC_2_UL_SendEvent(UmacHandle, WFM_EVT_BSS_REGAINED, (sizeof(EVENT_INDICATION) - 4), ((uint8 *) pEventIndication + 4), WFM_UMAC_EVT_NOT_RETRANSMIT);
		break;
	case LMAC_EVENT_IND_RADAR:
		LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "LMAC_EVENT_IND_RADAR\n");
		break;

	case LMAC_EVENT_IND_RCPI_RSSI:
		LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "LMAC_EVENT_IND_RCPI_RSSI\n");
		pUmacInstance->RssiRcpiThresholdEventRcvd = 1;
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_RCPI_RSSI,
					(sizeof(EVENT_INDICATION) - 4),
					((uint8 *) pEventIndication + 4),
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		break;
	case LMAC_EVENT_IND_BT_INACTIVE:
		LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "LMAC_EVENT_IND_BT_INACTIVE\n");
		pUmacInstance->gVars.p.BtActive = 0;
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_BT_INACTIVE,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		break;
	case LMAC_EVENT_IND_BT_ACTIVE:
		LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "LMAC_EVENT_IND_BT_ACTIVE\n");
		pUmacInstance->gVars.p.BtActive = 1;
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_BT_ACTIVE,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		break;

#if PS_MODE_ERROR_HANDLING
	case WSM_EVENT_IND_PS_MODE_ERROR:
		LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "LMAC_EVENT_IND_PS_MODE_ERROR\n");
		if (pEventIndication->EventData == WSM_PS_ERROR_AP_SENT_UNICAST_IN_DOZE) {
			if (!pUmacInstance->IsPmModeQueued) {
				pSmgmtReq = &pUmacInstance->gVars.p.SerialMgmtInfo;
				pSmgmtReq->Type = SMGMT_REQ_TYPE_PSMODE_UPDATE;
				pSmgmtReq->ReqNum = 0;
				/* Setting the power save mode as legacy */
				OS_MemoryReset(
								&pUmacInstance->gVars.p.PsMode.PmMode,
								sizeof(pUmacInstance->gVars.p.PsMode.PmMode)
							);
				pUmacInstance->gVars.p.PsMode.PmMode = WFM_PS_MODE_ENABLED;
				pUmacInstance->gVars.p.PsMode.FastPsmIdlePeriod = 0;
				pUmacInstance->gVars.p.PsMode.ApPsmChangePeriod = 0;
				pUmacInstance->IsPmModeQueued = TRUE;
				Result = WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_ASYNC_MGMT,
							NULL
							);
			}
		}
		break;
#endif

	default:
		LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "Unknown Event from LMAC\n");

	} /*  switch(pEventIndication->EventId) */
} /*end LLtoUMAC_EventIndication() */

/******************************************************************************
 * NAME:	LLtoUMAC_BlockAckTimeout
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the block timeout to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pBlockAckTimeout Pointer to BLOCK_ACK_TIMEOUT structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_BlockAckTimeout(UMAC_HANDLE UmacHandle,
			      BLOCK_ACK_TIMEOUT *pBlockAckTimeout)
{
	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_BlockAckTimeout()\n");

} /*end LLtoUMAC_BlockAckTimeout() */

/******************************************************************************
 * NAME:	LLtoUMAC_ConfigConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the configuration confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_ConfigConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WSM_HI_CONFIGURATION_CNF *pConfigCnf = (WSM_HI_CONFIGURATION_CNF *) pMsg;

	uint32 *pStatus = (uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));

	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_ConfigConfirmation()\n");

	OS_ASSERT(pConfigCnf);

	/*Convert to WFM type */
	pMsg->MsgId = WFM_CONFIG_CNF_ID;

	OS_MemoryCopy(&pUmacInstance->MacAddress[0], pConfigCnf->dot11StationId, 6);

	/* Power Levels for 2.4GHz */
	OS_MemoryCopy(
			&pUmacInstance->gVars.p.TxPowerRange[PHY_BAND_2G],
			&pConfigCnf->TxPowerRange[PHY_BAND_2G],
			2 * sizeof(sint32)
			);

	/* Power Levels for 5GHz */
	OS_MemoryCopy(
			&pUmacInstance->gVars.p.TxPowerRange[PHY_BAND_5G],
			&pConfigCnf->TxPowerRange[PHY_BAND_5G],
			2 * sizeof(sint32)
			);

	pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = (uint32) *pStatus;
	WFM_SCHEDULE_LMAC_TO_UMAC(
		UmacHandle,
		UMAC_ASYNC_MGMT,
		NULL
		);
	/* UMAC_ManageAsynMgmtOperations(UmacHandle); */

} /*end LLtoUMAC_ConfigConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_ReadMIBConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the read MIB confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_ReadMIBConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	WSM_HI_READ_MIB_CNF *pMibCnf = (WSM_HI_READ_MIB_CNF *) pMsg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	if (!pMibCnf->Status) {
		if (pMibCnf->MibId == WSM_MIB_ID_DOT11_STATION_ID) {
			OS_MemoryCopy(
					&pUmacInstance->MacAddress[0],
					&pMibCnf->MibData.dot11StationId[0],
					6
					);
		} else if (pMibCnf->MibId == WSM_MIB_ID_COUNTERS_TABLE) {
			OS_MemoryCopy(
					&pUmacInstance->statisticsInfo,
					&pMibCnf->MibData.CountTable,
					sizeof(WSM_MIB_COUNT_TABLE)
					);
		}
#if DOT11K_SUPPORT
		else if (pMibCnf->MibId == WSM_MIB_ID_TSF_COUNTER) {
			TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ *pTxStreamReq = (TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ *)
			pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;
			/*Copy the tsf timer value */
			OS_MemoryCopy((uint8 *)&pUmacInstance->dot11k_Meas_Params.txStreamResults.tsfTimer, (uint8 *)&pMibCnf->MibData.TSFCounter.TSFCounterlo, 8);
			pUmacInstance->dot11k_Meas_Params.transmitStreamUnderProcess = 1;
			OS_START_TIMER(pUmacInstance->gVars.p.UmacTimers.pTxStreamTimer,
					((pTxStreamReq->measurementDuration * 1024) / 1000),	/*meas Duration is in TU */
					UmacHandle);
		}
#endif
#if P2P_MINIAP_SUPPORT
		else if (pMibCnf->MibId == WSM_MIB_ID_GRP_SEQ_COUNTER) {
			OS_MemoryCopy(&pUmacInstance->groupTSC, &pMibCnf->MibData.GroupSeqCounter, sizeof(WSM_MIB_GROUP_SEQ_COUNTER)
			    );

		} /* else if ( pMibCnf->MibId == WSM_MIB_ID_GRP_SEQ_COUNTER ) */
#endif	/*P2P_MINIAP_SUPPORT */

		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = pMibCnf->Status;

	} /*if(!pMibCnf->Status) */
	else {
		LOG_EVENT(DBG_WFM_ERROR, "MIB Read Failed\n");
		OS_ASSERT(0);
	} /*if(!pMibCnf->Status) */

} /*end LLtoUMAC_ReadMIBConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_WriteMIBConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the write MIB confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_WriteMIBConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 *pStatus = (uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));

	if (!((uint32) *pStatus)) {
		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = (uint32) *pStatus;
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
	} else {
		LOG_EVENT(DBG_WFM_ERROR, "MIB Write Failed\n");
		OS_ASSERT(0);
	} /*if(!((uint32)*pStatus)) */

} /*end LLtoUMAC_WriteMIBConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_MemWriteConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Memory write confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_MemWriteConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	HI_MEM_WRITE_CNF *pMemWriteCnf = (HI_MEM_WRITE_CNF *) pMsg;

	pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = pMemWriteCnf->Result;

	WFM_SCHEDULE_LMAC_TO_UMAC(
		UmacHandle,
		UMAC_ASYNC_MGMT,
		NULL
		);
	/* UMAC_ManageAsynMgmtOperations(UmacHandle); */

} /*end LLtoUMAC_MemWriteConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_MemReadConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Memory read confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_MemReadConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_MemoryCopy(
			(uint8 *) pUmacInstance->RecvData,
			(uint8 *) pMsg, pMsg->MsgLen
			);

	WFM_SCHEDULE_LMAC_TO_UMAC(
		UmacHandle,
		UMAC_ASYNC_MGMT,
		NULL
		);
	/* UMAC_ManageAsynMgmtOperations(UmacHandle); */

} /*end LLtoUMAC_MemReadConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_StartScanConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the start scan confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_StartScanConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_StartScanConfirmation()\n");

} /*end LLtoUMAC_StartScanConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_StopScanConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the stop scan confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_StopScanConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_StopScanConfirmation()\n");

} /*end LLtoUMAC_StopScanConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_AddKeyConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the add key confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_AddKeyConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 *pStatus = (uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));

	if (!((uint32) *pStatus)) {
		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = (uint32) *pStatus;

		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
	} else {
		LOG_EVENT(DBG_WFM_ERROR, "Add Key Entry Req Failed\n");
		OS_ASSERT(0);
	} /*if(!((uint32)*pStatus)) */

} /*end LLtoUMAC_AddKeyConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_RemoveKeyConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the remove key confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_RemoveKeyConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 *pStatus = (uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));

	if (!((uint32) *pStatus)) {
		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = (uint32) *pStatus;
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
	} else {
		LOG_EVENT(DBG_WFM_ERROR, "Remove Key Entry Req Failed\n");
		OS_ASSERT(0);
	} /*if(!((uint32)*pStatus)) */

} /*end LLtoUMAC_RemoveKeyConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_StartMeasurementConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the start Measurment confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_StartMeasurementConfirmation(UMAC_HANDLE UmacHandle,
					   HI_MSG_HDR *pMsg)
{
#if DOT11K_SUPPORT
	/*Got the start measurement confirmation from the firmware */
	uint32 Result = WFM_STATUS_SUCCESS;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);
	/*Release the pMeasurementTxDesc memory */
	UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pUmacInstance->gVars.pMeasurementTxDesc
					);

	pUmacInstance->gVars.pMeasurementTxDesc = NULL;

	/* The result can either be SUCCESS or failure TBD */
	Result = *(uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));
	/*if (Result == WFM_STATUS_SUCCESS) {
		UMAC_ManageAsynMgmtOperations(UmacHandle);
	} else {
		LOG_EVENT(DBG_WFM_ERROR, "Measurement Confirmation Failed\n");
		OS_ASSERT(0);
	} */
	WFM_SCHEDULE_LMAC_TO_UMAC(
		UmacHandle,
		UMAC_ASYNC_MGMT,
		NULL
		);
	/* UMAC_ManageAsynMgmtOperations(UmacHandle); */

	if (Result != WSM_STATUS_SUCCESS)
		pUmacInstance->dot11k_Meas_Params.CurrentReportOp = REFUSED;
#endif	/* DOT11K_SUPPORT */

} /*end LLtoUMAC_StartMeasurementConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_StopMeasurementConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the stop measurement confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_StopMeasurementConfirmation(UMAC_HANDLE UmacHandle,
					  HI_MSG_HDR *pMsg)
{
	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_StopMeasurementConfirmation()\n");

} /*end LLtoUMAC_StopMeasurementConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_SetPmModeConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Set PS Mode Confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SetPmModeConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_SetPmModeConfirmation()\n");

} /*end LLtoUMAC_SetPmModeConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_SetBssParamConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Set BSS parameter confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SetBssParamConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 *pStatus = (uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));

	if (!(uint32) *pStatus) {
		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = (uint32) *pStatus;
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
	} else {
		LOG_EVENT(DBG_WFM_ERROR, "BSS Parameter Request Failed\n");
		OS_ASSERT(0);
	} /*if(!(uint32)*pStatus) */

} /*end LLtoUMAC_SetBssParamConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_SwitchChannelConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Switch Channel confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SwitchChannelConfirmation(UMAC_HANDLE UmacHandle,
					HI_MSG_HDR *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 *pStatus = (uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));

		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = (uint32) *pStatus;
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);

} /*end LLtoUMAC_SwitchChannelConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_SetTxQueueParamConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Set TX Queue Parameter confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SetTxQueueParamConfirmation(UMAC_HANDLE UmacHandle,
					  HI_MSG_HDR *pMsg)
{
	uint32 *pStatus = (uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	if (!(uint32) *pStatus) {
		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = (uint32) *pStatus;
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
	} else {
		LOG_EVENT(DBG_WFM_ERROR, "Tx Queue Parameter Request Failed\n");
		OS_ASSERT(0);
	} /*if(!(uint32)*pStatus) */

} /*end LLtoUMAC_SetTxQueueParamConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_SetEdcaParamConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Set EDCA Parameter confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SetEdcaParamConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 *pStatus = (uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));

	if (!(uint32) *pStatus) {
		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = (uint32) *pStatus;
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
	} else {
		LOG_EVENT(DBG_WFM_ERROR, "EDCA Parameter Request Failed\n");
		OS_ASSERT(0);
	} /*if(!(uint32)*pStatus) */

} /*end LLtoUMAC_SetEdcaParamConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_StartUpIndication
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Start up Indication to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_StartUpIndication(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_STATUS_CODE status;
	HI_STARTUP_IND *pStartUpInd = (HI_STARTUP_IND *) pMsg;

	OS_ASSERT(pUmacInstance);

#if P2P_MINIAP_SUPPORT
	pUmacInstance->gVars.p.FirmwareClientSupport = ((uint16) pStartUpInd->Configuration[0] + 1);
	pUmacInstance->gVars.p.maxNumClientSupport = ((uint16) pStartUpInd->Configuration[0] + 1);
#endif /* P2P_MINIAP_SUPPORT */

	/* Sending Startup Indication to Host */
	pUmacInstance->gVars.p.UpperLayerCb(
					   pUmacInstance->gVars.p.ulHandle,
					   (WFM_HI_MESSAGE *) pMsg
					   );

	status = UMAC_Start(UmacHandle);

	if (WFM_STATUS_SUCCESS != status)
		OS_ASSERT(0);

} /*end LLtoUMAC_StartUpIndication() */

/******************************************************************************
 * NAME:	LLtoUMAC_SetConfigBlockAckConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Block ACK Confirmation to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SetConfigBlockAckConfirmation(UMAC_HANDLE UmacHandle,
					    HI_MSG_HDR *pMsg)
{
	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_SetConfigBlockAckConfirmation()\n");

} /*end LLtoUMAC_SetConfigBlockAckConfirmation() */

/******************************************************************************
 * NAME:	LLtoUMAC_GenericIndication
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Generic Indication to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_GenericIndication(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_GenericIndication()\n");
} /*end LLtoUMAC_GenericIndication() */


/******************************************************************************
 * NAME:	LLtoUMAC_ExceptionIndication
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the Exception Indication to UMAC.
 * \param  UmacHandle UMAC handle
 * \param pMsg Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_ExceptionIndication(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	HI_EXCEPTION_IND *pExpInd = (HI_EXCEPTION_IND *) pMsg;
	LOG_EVENT(DBG_WFM_ERROR, "EXCEPTION INDICATION\n");
	LOG_DATA(DBG_WFM_ERROR, "REASON = %d\n", pExpInd->Reason);
} /*end LLtoUMAC_ExceptionIndication() */

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:LLtoUMAC_STARTConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the START confirmation to UMAC.
 * \param  UmacHandle   -  UMAC handle
 * \param  pStartCnf      -  Pointer to START confirmation.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_STARTConfirmation(UMAC_HANDLE UmacHandle,
				WSM_HI_START_CNF *pStartCnf)
{
	WFM_STATUS_CODE wfmStatus = WFM_STATUS_SUCCESS;
	uint16 UmacEvent = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *)UmacHandle;

	LOG_DATA(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "FRM-LMAC: LLtoUMAC_STARTConfirmation():%d\n", pStartCnf->Status);

	wfmStatus = (WFM_STATUS_CODE) umac_interpret_wsm_status(pStartCnf->Status);

#if 0
	if (pUmacInstance->ChannelChange.ChannelState == UMAC_CHANNEL_STATE_CHANGING) {
		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = wfmStatus;
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
	} else {
#endif
		if (WFM_STATUS_SUCCESS != wfmStatus)
			UmacEvent = UMAC_START_AP_FAILED;
		else
			UmacEvent = UMAC_START_AP_SUCCESS;

		WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UmacEvent, (void *)wfmStatus);
#if 0
	}
#endif

} /*end LLtoUMAC_STARTConfirmation() */

/******************************************************************************
 * NAME:LLtoUMAC_BcnTxConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the START confirmation to UMAC.
 * \param  UmacHandle   -  UMAC handle
 * \param  pBcnTxCnf    -  Pointer to Beacon transmit confirmation.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_BcnTxConfirmation(UMAC_HANDLE UmacHandle,
				WSM_HI_BEACON_TRANSMIT_CNF *pBcnTxCnf)
{
	WFM_STATUS_CODE wfmStatus = WFM_STATUS_SUCCESS;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	LOG_DATA(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "FRM-LMAC: LLtoUMAC_BcnTxConfirmation():%d\n", pBcnTxCnf->Status);

	wfmStatus = (WFM_STATUS_CODE) umac_interpret_wsm_status(pBcnTxCnf->Status);

	UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
						UmacHandle,
						pUmacInstance->gVars.pBcnTxReq
						);

	pUmacInstance->gVars.pBcnTxReq = NULL;

/*	if(pUmacInstance->stopAPFlag) {
		pUmacInstance->stopAPFlag = 0;
		WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle,UMAC_STOP_AP,NULL);
	} else {*/

	UMAC_MOVE_PENDING_MSGS_TO_ACTIVE_Q(UmacHandle);

	WFM_UMAC_2_UL_SendEvent(
			UmacHandle,
			WFM_EVT_BSS_STARTED,
			0,
			NULL,
			WFM_UMAC_EVT_NOT_RETRANSMIT
			);

} /*end LLtoUMAC_BcnTxConfirmation() */

/******************************************************************************
 * NAME:LLtoUMAC_MapLinkConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It processes the Map link confirmation
 * \param  UmacHandle   -  UMAC handle
 * \param  pMapLinkCnf    -  Pointer to Map link confirmation.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_MapLinkConfirmation(UMAC_HANDLE UmacHandle,
				  WSM_HI_BEACON_TRANSMIT_CNF *pMapLinkCnf)
{
	WFM_STATUS_CODE wfmStatus = WFM_STATUS_SUCCESS;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	LOG_DATA(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "FRM-LMAC: LLtoUMAC_MapLinkConfirmation():%d\n", pMapLinkCnf->Status);

	wfmStatus = (WFM_STATUS_CODE) umac_interpret_wsm_status(pMapLinkCnf->Status);

	pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = wfmStatus;
	WFM_SCHEDULE_LMAC_TO_UMAC(
		UmacHandle,
		UMAC_ASYNC_MGMT,
		NULL
		);
	/* UMAC_ManageAsynMgmtOperations(UmacHandle); */

	UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pMapLinkReq);

} /*end LLtoUMAC_MapLinkConfirmation() */

/******************************************************************************
 * NAME:LLtoUMAC_StartFindConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the start find confirmation to UMAC.
 * \param UmacHandle       - UMAC handle
 * \param pMsg	     - Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_StartFindConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	LOG_EVENT(DBG_WFM_IN_OUT_MSGS, "FRM-LMAC: LLtoUMAC_StartFindConfirmation()\n");

} /*end LLtoUMAC_StartFindConfirmation() */

/******************************************************************************
 * NAME:LLtoUMAC_FindComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the Find Complete Indication to UMAC.
 * \param UmacHandle       - UMAC handle
 * \param pFindComplete    - Pointer to WSM_HI_FIND_CMPL_IND structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_FindComplete(UMAC_HANDLE UmacHandle,
			   WSM_HI_FIND_CMPL_IND *pFindComplete)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_SCAN, "FRM-LMAC: LLtoUMAC_FindComplete()\n");

	pUmacInstance->findCompleteStatus = (uint8) pFindComplete->Status;

	if (pUmacInstance->UmacCurrentState == UMAC_GROUP_FORMING) {
		/* We could not find the peer device for group formation hence group formation failed */
		WFM_UMAC_2_UL_SendEvent(
					UmacHandle,
					WFM_EVT_GROUP_FORMATION_FAILED,
					0,
					NULL,
					WFM_UMAC_EVT_NOT_RETRANSMIT
					);
		pUmacInstance->UmacFindState = UMAC_READY_TO_FIND;
		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
	} else {
		WFM_SCHEDULE_LMAC_TO_UMAC(
					UmacHandle,
					UMAC_FIND_COMPLETED,
					&pUmacInstance->findCompleteStatus
					);
	}

} /*end LLtoUMAC_FindComplete() */

/******************************************************************************
 * NAME:LLtoUMAC_StopFindConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the stop find confirmation to UMAC.
 * \param UmacHandle    - UMAC handle
 * \param pMsg	  - Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_StopFindConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 *pStatus = (uint32 *) ((uint8 *) pMsg + sizeof(HI_MSG_HDR));

	if (!((uint32) *pStatus)) {
		pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = (uint32) *pStatus;

		/* UMAC_ManageAsynMgmtOperations(UmacHandle); */
		WFM_SCHEDULE_LMAC_TO_UMAC(
			UmacHandle,
			UMAC_ASYNC_MGMT,
			NULL
			);
	} else {
		LOG_EVENT(DBG_WFM_ERROR, "Stop Find Req Failed\n");
		OS_ASSERT(0);
	} /*if(!((uint32)*pStatus)) */
} /*end LLtoUMAC_StopFindConfirmation() */

/******************************************************************************
 * NAME:LLtoUMAC_UpdateIEConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the update IE confirmation to UMAC.
 * \param UmacHandle       - UMAC handle
 * \param pUpdateIeCnf     - Pointer to the Update IE confirmation structure
 * \returns void
 *****************************************************************************/
void LLtoUMAC_UpdateIEConfirmation(UMAC_HANDLE UmacHandle,
				   WSM_HI_UPDATE_IE_CNF *pUpdateIeCnf)
{
	WFM_STATUS_CODE wfmStatus = WFM_STATUS_SUCCESS;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	LOG_DATA(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "FRM-LMAC: LLtoUMAC_UpdateIEConfirmation():%d\n", pUpdateIeCnf->Status);

	wfmStatus = (WFM_STATUS_CODE) umac_interpret_wsm_status(pUpdateIeCnf->Status);

	pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus = wfmStatus;

	WFM_SCHEDULE_LMAC_TO_UMAC(
		UmacHandle,
		UMAC_ASYNC_MGMT,
		NULL
		);
	/* UMAC_ManageAsynMgmtOperations(UmacHandle); */

} /*end LLtoUMAC_UpdateIEConfirmation() */

/******************************************************************************
 * NAME:LLtoUMAC_SuspendResumeTxInd
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the suspend resume Tx Indication to UMAC.
 * \param UmacHandle       - UMAC handle
 * \param pSRTxInd     - Pointer to the Suspend Resume Tx Ind structure
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SuspendResumeTxInd(UMAC_HANDLE UmacHandle,
				 WSM_HI_SUSPEND_RESUME_TX_IND *pSRTxInd)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 linkId;
	linkId = (uint8) ((pSRTxInd->MsgId & UMAC_HI_MSG_LINK_ID) >> 6);

	LOG_EVENT(DBG_WFM_IN_OUT_MSGS | DBG_WFM_UMAC, "FRM-LMAC: LLtoUMAC_SuspendResumeTxInd()\n");

	/*b0=Resume Tx, b3=Tx broadcast/multicast data */
	if (((pSRTxInd->Flags & 0x00000009) == 0x00000009)
		&& (pUmacInstance->sta[DEFAULT_LINK_ID].numPendingFrames[AP_BUFF_TYPES_LEGACY])
	    ) {
		/* Resume Tx indication */
		/*OS_LOCK(pUmacInstance->gVars.p.RxBufCountLock);
		pUmacInstance->NumRxBufPending[linkId]++;
		OS_UNLOCK(pUmacInstance->gVars.p.RxBufCountLock);*/
		if (!pUmacInstance->PendingSusResTxInd) {
			pUmacInstance->PendingSusResTxInd = TRUE;
			OS_MemoryCopy(&pUmacInstance->SuspendResumeTxInd, pSRTxInd, sizeof(UMAC_SUSPEND_RESUME_TX_IND));
			WFM_UMAC_EXEC_STATE_MACHINE(
							UmacHandle,
							UMAC_PS_DLVR_LEGACY_DATA,
							(void *)&pUmacInstance->SuspendResumeTxInd
							);
		}
		DIL_RxComplete((LL_INFO *) UmacHandle, pSRTxInd, NULL);
	} else if (pSRTxInd->Flags & 0xFFFFFFFE) {
		/* Suspend Tx indication */
		DIL_RxComplete((LL_INFO *) UmacHandle, pSRTxInd, NULL);
	} else
		DIL_RxComplete((LL_INFO *) UmacHandle, pSRTxInd, NULL);

} /*end LLtoUMAC_SuspendResumeTxInd() */
#endif	/*P2P_MINIAP_SUPPORT */

