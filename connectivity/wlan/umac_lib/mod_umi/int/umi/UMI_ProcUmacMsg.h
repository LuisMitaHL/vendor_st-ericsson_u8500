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
 * \file UMI_ProcUmacMsg.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_ProcUmacMsg.h
 * \brief
 * Header for UMI Process UMAC Message.
 * \ingroup Upper_MAC_Interface
 * \date 05/01/09 14:42
 */

#ifndef _UMI_PROC_UMAC_MSG_H
#define _UMI_PROC_UMAC_MSG_H

#include "UMI_Api.h"
#include "umac_if.h"
#include "umac_messages.h"

/******************************************************************************
 * NAME:	UMI_ProcessMessages
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the incoming messages from core UMAC to UMI module.
 * \param umiHandle   -   Handle for this UMI instance.
 * \param pHiMessage  -   The incoming message.
 * \returns none.
 *****************************************************************************/
uint8 UMI_ProcessMessages(UMI_HANDLE umiHandle, WFM_HI_MESSAGE *pHiMessage);

/******************************************************************************
 * NAME:	UMI_ScheduleUmac
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function calls upper layer to schedule UMAC.
 * \param umiHandle - Handle for this UMI instance.
 * \returns none.
 *****************************************************************************/
void UMI_ScheduleUmac(UMI_HANDLE umiHandle);

/******************************************************************************
 * NAME:	TransmitCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the Transmit Confirmation to the upper layer driver.
 * \param umiHandle - Handle for this UMI instance.
 * \param pTxCnf - Confirmation message.
 * \returns Status     1 : SUCCESS, 0 : FAILURE.
 *****************************************************************************/
uint8 TransmitCnf(UMI_HANDLE umiHandle, UMAC_TX_DATA_CNF *pTxCnf);

/******************************************************************************
 * NAME:	TxMgmtFrmCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the Transmit Confirmation for Mangment frame to the
 * upper layer driver.
 * \param umiHandle - Handle for this UMI instance.
 * \param pTxCnf - Confirmation message.
 * \returns Status     1 : SUCCESS, 0 : FAILURE.
 *****************************************************************************/
uint8 TxMgmtFrmCnf(UMI_HANDLE umiHandle, UMAC_TX_MGMT_DATA_CNF *pTxMgmtDataCnf);

/******************************************************************************
 * NAME:    UMI_ProcessRxFrames
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the received 802.3 frames to the upper layer driver.
 * \param umiHandle - Handle for this UMI instance.
 * \param Length - Length of the 802.3 frame.
 * \param pFrame - Received 802.3 frame.
 * \param pDriverInfo - Pointer to driver information.
 * \param pFrameStart - Start of the frame buffer.
 * \param flags - misc flags.
 * \returns Status     1 : SUCCESS, 0 : FAILURE.
 *****************************************************************************/
uint8 UMI_ProcessRxFrames(UMI_HANDLE umiHandle,
			  uint8 statusCode,
			  void *pFrame,
			  uint16 Length,
			  void *pDriverInfo,
			  void *pFrameStart,
			  uint32 flags);

#if DOT11K_SUPPORT
/******************************************************************************
 * NAME:    UMI_BeaconInfo
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the beacon cache info that is requested based on bssid
 * \param umiHandle - Handle for this UMI instance.
 * \param pBssid - Pointer to BSSID.
 * \returns pointer to UMI_BSS_CACHE_INFO_IND structure.
 *****************************************************************************/
void *UMI_BeaconInfo(UMI_HANDLE umiHandle,
		     uint8 *pBssid,
		     uint8 bssidIndex,
		     uint32 *pBssidCount);

#endif	/*DOT11K_SUPPORT */

#endif
