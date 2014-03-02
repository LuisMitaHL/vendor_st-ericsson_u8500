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
 * \file lmac_callbacks.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: lmac_callbacks.h
 * \brief
 * LMAC Callbacks to UMAC.
 * \ingroup Upper_MAC_Core
 * \date 31-Aug-2007
 */

#ifndef _LMAC_CALLBACKS_H
#define _LMAC_CALLBACKS_H

/******************************************************************************
		       INCLUDE FILES
******************************************************************************/
#include "umac_ll_if.h"
#include "umac_if.h"
#include "hi_api.h"
#include "wsm_api.h"

/******************************************************************************
			    PUBLIC FUNCTIONS
******************************************************************************/


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
void LLtoUMAC_ScanComplete(UMAC_HANDLE UmacHandle,
			   SCAN_COMPLETE *pScanComplete);

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
				  WSM_HI_MEASURE_CMPL_IND *pMeasurementComplete);

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
				SET_PM_MODE_COMPLETE *pSetPsModeComplete);

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
			  WSM_HI_SWITCH_CHANNEL_IND *pSwitchChInd);

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
			   void *pDriverInfo);

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
				   UMAC_TX_CNF_DESC *pTxDescriptor);

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
			   LMAC_RESET_CONFIRMATION *pResetCnf);

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
void LLtoUMAC_JoinConfirm(UMAC_HANDLE UmacHandle,
			  JOIN_CONFIRM *pJoinConfirm);

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
			      EVENT_INDICATION *pEventIndication);

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
			      BLOCK_ACK_TIMEOUT *pBlockAckTimeout);

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
void LLtoUMAC_ConfigConfirmation(UMAC_HANDLE UmacHandle,
				 HI_MSG_HDR *pMsg);

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
void LLtoUMAC_ReadMIBConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
void LLtoUMAC_WriteMIBConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
void LLtoUMAC_MemWriteConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
void LLtoUMAC_MemReadConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
void LLtoUMAC_StartScanConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
void LLtoUMAC_StopScanConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
void LLtoUMAC_AddKeyConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
void LLtoUMAC_RemoveKeyConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
					   HI_MSG_HDR *pMsg);

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
					  HI_MSG_HDR *pMsg);

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
void LLtoUMAC_SetPmModeConfirmation(UMAC_HANDLE UmacHandle,
				    HI_MSG_HDR *pMsg);

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
void LLtoUMAC_SetBssParamConfirmation(UMAC_HANDLE UmacHandle,
				      HI_MSG_HDR *pMsg);

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
					HI_MSG_HDR *pMsg);

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
					  HI_MSG_HDR *pMsg);

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
void LLtoUMAC_SetEdcaParamConfirmation(UMAC_HANDLE UmacHandle,
				       HI_MSG_HDR *pMsg);

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
					  HI_MSG_HDR *pMsg);

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
void LLtoUMAC_StartUpIndication(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
					    HI_MSG_HDR *pMsg);

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
void LLtoUMAC_GenericIndication(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
void LLtoUMAC_ExceptionIndication(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:LLtoUMAC_STARTConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the START confirmation to UMAC.
 * \param  UmacHandle   -  UMAC handle
 * \param  pStartCnf    -  Pointer to START confirmation.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_STARTConfirmation(UMAC_HANDLE UmacHandle,
				WSM_HI_START_CNF *pStartCnf);

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
				WSM_HI_BEACON_TRANSMIT_CNF *pBcnTxCnf);

/******************************************************************************
 * NAME:LLtoUMAC_MapLinkConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It processes the Map link confirmation
 * \param  UmacHandle   -  UMAC handle
 * \param  pMapLinkCnf  -  Pointer to Map link confirmation.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_MapLinkConfirmation(UMAC_HANDLE UmacHandle,
				  WSM_HI_MAP_LINK_CNF *pMapLinkCnf);

/******************************************************************************
 * NAME:	LLtoUMAC_StartFindConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the start find confirmation to UMAC.
 * \param UmacHandle    - UMAC handle
 * \param pMsg		- Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_StartFindConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

/******************************************************************************
 * NAME:	LLtoUMAC_FindComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the Find Complete Indication to UMAC.
 * \param UmacHandle       - UMAC handle
 * \param pFindComplete    - Pointer to WSM_HI_FIND_CMPL_IND structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_FindComplete(UMAC_HANDLE UmacHandle,
			   WSM_HI_FIND_CMPL_IND *pFindComplete);

/******************************************************************************
 * NAME:	LLtoUMAC_StopFindConfirmation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the stop find confirmation to UMAC.
 * \param UmacHandle    - UMAC handle
 * \param pMsg		- Pointer to HI_MSG_HDR structure.
 * \returns void
 *****************************************************************************/
void LLtoUMAC_StopFindConfirmation(UMAC_HANDLE UmacHandle, HI_MSG_HDR *pMsg);

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
				   WSM_HI_UPDATE_IE_CNF *pUpdateIeCnf);

/******************************************************************************
 * NAME:LLtoUMAC_SuspendResumeTxInd
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * It sends the suspend resume Tx Indication to UMAC.
 * \param UmacHandle    - UMAC handle
 * \param pSRTxInd	- Pointer to the Suspend Resume Tx Ind structure
 * \returns void
 *****************************************************************************/
void LLtoUMAC_SuspendResumeTxInd(UMAC_HANDLE UmacHandle,
				 WSM_HI_SUSPEND_RESUME_TX_IND *pSRTxInd);

#endif /* P2P_MINIAP_SUPPORT */

#endif	/* _LMAC_CALLBACKS_H */
