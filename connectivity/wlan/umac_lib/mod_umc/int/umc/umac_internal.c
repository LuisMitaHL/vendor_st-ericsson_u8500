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
 * \file umac_internal.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_internal.c
 * \brief
 * This module handles the Internal Implementation functions for UMAC.
 * \ingroup Upper_MAC_Core
 * \date 05/11/08 08:41
 */

/******************************************************************************
		      INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "wsm_api.h"
#include "umac_dev_conf.h"
#include "umac_internal.h"
#include "umac_sm.h"
#include "umac_messages.h"
#include "umac_frames.h"
#include "umac_2_lmac.h"
#include "umac_task.h"
#include "umac_mib.h"
#include "umac_mem.h"
#include "umac_ll_if.h"
#include "umac_if.h"
#include "umac_data_handler.h"

/******************************************************************************
 * NAME:	WFM_UMAC_Initialize_TxQueueParams_All
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets Tx Queue parameters and rates to a default value.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Initialize_TxQueueParams_All(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	int i;

	OS_ASSERT(pUmacInstance);

	for (i = 0; i < WFM_UMAC_NUM_TX_QUEUES; i++) {

		/* normal ACK */
		pUmacInstance->TxQueueParam[i].AckPolicy = WSM_ACKPLCY_NORMAL;

		pUmacInstance->TxQueueParam[i].MaxTransmitLifetime = D11_MAX_TRANSMIT_MSDU_LIFETIME;

		pUmacInstance->TxQueueParam[i].SupportedRateSet = pUmacInstance->AllSupportedRates_WithAP_Abgn;
	}
} /* end WFM_UMAC_Initialize_TxQueueParams_All() */

/******************************************************************************
 * NAME:	UMAC_Initialize_TxQueueParams_Rates_All
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets rates params of all queues of Tx Queue parameters.
 * \param UmacHandle       - Handle to UMAC Instance.
 * \param SupportedRates   - Rates for data frames in wsm/wfm format.
 * \returns none.
 *****************************************************************************/
void UMAC_Initialize_TxQueueParams_Rates_All(UMAC_HANDLE UmacHandle,
					     uint32 SupportedRates)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	int i;

	OS_ASSERT(pUmacInstance);

	for (i = 0; i < WFM_UMAC_NUM_TX_QUEUES; i++)
		pUmacInstance->TxQueueParam[i].SupportedRateSet = SupportedRates;

} /* end UMAC_Initialize_TxQueueParams_Rates_All() */

/******************************************************************************
 * NAME:	WFM_UMAC_Initialize_Encryption
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets Encryption parameters to default (open/no encryption)
 * \param UmacHandle       - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Initialize_Encryption(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	int i;

	OS_ASSERT(pUmacInstance);

	pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost = WFM_AUTH_MODE_OPEN;

	pUmacInstance->gVars.p.InfoHost.AuthenticationMode = UMAC_OPEN_SYSTEM_AUTH;

	pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost = WFM_ENC_STATUS_NO_ENCRYPTION;
	pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_NO_ENC_SUPPORTED;

	pUmacInstance->gVars.p.InfoHost.WepKeyMap = 0;
	pUmacInstance->gVars.p.InfoHost.GroupKeyInstalled = 0;

	for (i = 0; i < UMAC_MAX_KEY_ENTRIES; i++)
		pUmacInstance->gVars.p.InfoHost.EncKey[i].InUse = FALSE;

} /* end WFM_UMAC_Initialize_Encryption() */

/******************************************************************************
 * NAME:	WFM_UMAC_GetOid_EncryptionStatus
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function gets the present encryption status of the device.
 * \param UmacHandle       - Handle to UMAC Instance.
 * \param pEncryption_Status - Encryption Type in use
 *			     b0:1 => WEP is being used
 *			     b1:1 => TKIP (WPA) is being used
 *			     b2:1 => AES (WPA2) is being used
 *			     All b0/b1/b2 = 0 => No encryption is being used
 *			     b31:1 => Tx key available for that encryption mode
 * \returns uint32 WFM_STATUS_SUCCESS when successful.
 *****************************************************************************/
uint32 WFM_UMAC_GetOid_EncryptionStatus(UMAC_HANDLE UmacHandle,
					uint32 *pEncryption_Status)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 i;
	WFM_OID_802_11_KEY *pKey;
	uint8 iskeypresent = 0;

	OS_ASSERT(pUmacInstance);

	*pEncryption_Status = pUmacInstance->gVars.p.InfoHost.EncryptionType;

	if (pUmacInstance->gVars.p.InfoHost.EncryptionType == WFM_UMAC_NO_ENC_SUPPORTED) {
		/*No encryption supported, so no point in searching for key */
		goto exit_handler;
	}

	/* Look for a Tx key for the encryption type */
	for (i = 0; i < UMAC_MAX_KEY_ENTRIES; i++) {
		if (pUmacInstance->gVars.p.InfoHost.EncKey[i].InUse) {
			pKey = (WFM_OID_802_11_KEY *) &pUmacInstance->gVars.p.InfoHost.EncKey[i].UmacDot11AddKey;

			switch (pKey->Type) {
			case eEncWEPDEFAULT:
			case eEncWEPPAIRWISE:

				if (pUmacInstance->gVars.p.InfoHost.EncryptionType == WFM_UMAC_WEP_ENC_SUPPORTED)
					iskeypresent = 1;

				break;
			case eEncTKIPGROUP:
			case eEncTKIPPAIRWISE:

				if (pUmacInstance->gVars.p.InfoHost.EncryptionType == WFM_UMAC_TKIP_ENC_SUPPORTED)
					iskeypresent = 1;

				break;
			case eEncAESGROUP:
			case eEncAESPAIRWISE:

				if (pUmacInstance->gVars.p.InfoHost.EncryptionType == WFM_UMAC_AES_ENC_SUPPORTED)
					iskeypresent = 1;

				break;
#if WAPI_SUPPORT
			case eEncWAPIGROUP:
			case eEncWAPIPAIRWISE:

				if (pUmacInstance->gVars.p.InfoHost.EncryptionType == WFM_UMAC_WAPI_ENC_SUPPORTED)
					iskeypresent = 1;

				break;
#endif	/* WAPI_SUPPORT */
			case eEncNONE:
			default:
				OS_ASSERT(0);

			} /* switch(pKey->Type) */

		} /* if(pUmacInstance->gVars.p.InfoHost.EncKey[i].InUse) */

		if (iskeypresent) {
			*pEncryption_Status |= WFM_ENC_STATUS_MASK_KEY_AVAIL;
			break;
		} /* if( iskeypresent ) */
	} /* for(i=0; i<UMAC_MAX_KEY_ENTRIES; i++)*/

 exit_handler:
	return Status;
} /* end - WFM_UMAC_GetOid_EncryptionStatus() */

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_AuthenticationMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets authentication mode to open/shared.
 * \param UmacHandle	      - Handle to UMAC Instance.
 * \param dot11AuthenticationMode - Authentication mode to set(Open/Shared)
 * \returns WFM_STATUS_SUCCESS  -  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_AuthenticationMode(UMAC_HANDLE UmacHandle,
					  uint8 dot11AuthenticationMode)
{
	uint32 Status = WFM_STATUS_SUCCESS;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_DATA(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ENCRYPTION, "WFM_UMAC_SetOid_AuthenticationMode(): AuthMode = %d\n", dot11AuthenticationMode);

	/*set encryption type */
	Status = WFM_UMAC_SetEncryptionType(UmacHandle, dot11AuthenticationMode);

	if (Status == WFM_STATUS_SUCCESS) {
#if FT_SUPPORT
		if ((pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT)
		    || (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT_PSK)) {
			if (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost != dot11AuthenticationMode) {
				OS_MemoryReset(&pUmacInstance->gVars.p.FtAuthenticatedAPs[0], MAX_FT_AP * sizeof(pUmacInstance->gVars.p.FtAuthenticatedAPs[0]));
				pUmacInstance->FtFlags = 0;
				OS_MemoryReset(&pUmacInstance->FtTargetBss, sizeof(pUmacInstance->FtTargetBss));
			}
		}
#endif	/* FT_SUPPORT */

		/*change authentication mode only when its a valid value */
		pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost = dot11AuthenticationMode;

		switch (dot11AuthenticationMode) {
		case WFM_AUTH_MODE_SHARED:
			pUmacInstance->gVars.p.InfoHost.AuthenticationMode = UMAC_SHARED_KEY_AUTH;
			break;
		case WFM_AUTH_MODE_OPEN:
		case WFM_AUTH_MODE_WPA:
		case WFM_AUTH_MODE_WPA_PSK:
		case WFM_AUTH_MODE_WPA_NONE:
		case WFM_AUTH_MODE_WPA_2:
		case WFM_AUTH_MODE_WPA_2_PSK:
		case WFM_AUTH_MODE_WAPI:
#if FT_SUPPORT
		case WFM_AUTH_MODE_WPA_2_FT:
		case WFM_AUTH_MODE_WPA_2_FT_PSK:
#endif	/* FT_SUPPORT */
#if MGMT_FRAME_PROTECTION
		case WFM_AUTH_MODE_WPA_2_SH256:
		case WFM_AUTH_MODE_WPA_2_SH256_PSK:
#endif	/* MGMT_FRAME_PROTECTION */
			pUmacInstance->gVars.p.InfoHost.AuthenticationMode = UMAC_OPEN_SYSTEM_AUTH;
			break;
		}
	}

	return Status;
} /* end - WFM_UMAC_SetOid_AuthenticationMode() */

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_EncryptionStatus
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets encryption type to be used by the device.
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param Encryption_Status - B0:1 => WEP can be used
 *			    B1:1 => TKIP (WPA) can be used
 *			    B2:1 => AES (WPA2) can be used
 *			    All 0 => No encryption can be used
 * \returns WFM_STATUS_SUCCESS  -  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_EncryptionStatus(UMAC_HANDLE UmacHandle,
					uint32 Encryption_Status)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_DATA(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ENCRYPTION, "WFM_UMAC_SetOid_EncryptionStatus(): EncStatus = 0x%x\n", Encryption_Status);

	pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost = Encryption_Status;

	/*check encryption type to be used */
	WFM_UMAC_SetEncryptionType(UmacHandle, pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost);

	return Status;
} /* end - WFM_UMAC_SetOid_EncryptionStatus() */

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_InfrastructureMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets infrastructure mode to IBSS/Infrastructure.
 * It also causes disassociation with the presently associated AP.
 * It also deletes all the Encryption Keys.
 * \param UmacHandle		  - Handle to UMAC Instance.
 * \param dot11InfrastructureMode - Infrastructure mode to
 *				    set (IBSS/Infrastructure)
 * \returns WFM_STATUS_SUCCESS    -  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_InfrastructureMode(UMAC_HANDLE UmacHandle,
					  uint32 dot11InfrastructureMode)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	if (pUmacInstance->dot11InfraStructureMode != dot11InfrastructureMode) {
		pUmacInstance->dot11InfraStructureMode = dot11InfrastructureMode;
		/*disassociate from AP if associated */
		if (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED)
			WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_DE_ASSOC, NULL);

	}

	return Status;
} /* end - WFM_UMAC_SetOid_InfrastructureMode() */

/******************************************************************************
 * NAME:	WFM_UMAC_SetEncryptionType
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets encryption type to be used by the wfm.It uses
 * pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost and
 * AuthenticationModeHost to set the
 * pUmacInstance->gVars.p.InfoHost.EncryptionType.
 * \param UmacHandle	         - Handle to UMAC Instance.
 * \param AuthenticationModeHost - Authentication mode sent by host
 * \returns WFM_STATUS_SUCCESS   -  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetEncryptionType(UMAC_HANDLE UmacHandle,
				  uint32 AuthenticationModeHost)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	switch (AuthenticationModeHost) {
	case WFM_AUTH_MODE_OPEN:
	case WFM_AUTH_MODE_SHARED:
		if (pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost == WFM_ENC_STATUS_NO_ENCRYPTION)
			pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_NO_ENC_SUPPORTED;
		else if (pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost & WFM_ENC_STATUS_WEP_SUPPORTED)
			pUmacInstance->gVars.p.InfoHost.EncryptionType = WFM_UMAC_WEP_ENC_SUPPORTED;
		break;

	case WFM_AUTH_MODE_WPA:
	case WFM_AUTH_MODE_WPA_PSK:
	case WFM_AUTH_MODE_WPA_2:
	case WFM_AUTH_MODE_WPA_2_PSK:
#if FT_SUPPORT
	case WFM_AUTH_MODE_WPA_2_FT:
	case WFM_AUTH_MODE_WPA_2_FT_PSK:
#endif	/* FT_SUPPORT */
#if MGMT_FRAME_PROTECTION
	case WFM_AUTH_MODE_WPA_2_SH256:
	case WFM_AUTH_MODE_WPA_2_SH256_PSK:
		break;
#endif	/* MGMT_FRAME_PROTECTION */
		break;

#if WAPI_SUPPORT
	case WFM_AUTH_MODE_WAPI:
		break;
#endif	/* WAPI_SUPPORT */

	case WFM_AUTH_MODE_WPA_NONE:
	default:
		LOG_DATA(DBG_WFM_CURRENT_GM, "Wrong value for AuthenticationModeHost = %d\n", AuthenticationModeHost);
		/* unsupported */
		Status = WFM_STATUS_BAD_PARAM;
	} /* end-switch( AuthenticationModeHost ) */

	return Status;

} /* end WFM_UMAC_SetEncryptionType() */

/******************************************************************************
 * NAME:	WFM_UMAC_UseGreenfieldMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets whether to use mixed mode or greenfield mode.
 * \param UmacHandle	      - Handle to UMAC Instance.
 * \param u8UseGreenfieldMode - TRUE -> use greenfield mode,
 *			        FALSE -> use mixed mode.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_UseGreenfieldMode(UMAC_HANDLE UmacHandle,
				  uint8 u8UseGreenfieldMode)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_FAILURE;

	OS_ASSERT(pUmacInstance);

	if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP) {
		if (u8UseGreenfieldMode == TRUE) {
			/* use greenfield mode */
			if ((WFM_HT_CAP_GREEN_FIELD & WFM_UMAC_HT_CAP_OURS)
				&& !(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_HT_DISABLE_GREENFIELD_MODE)
				&& (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_HT_GREENFIELD_MODE_SUPPORTED_BY_AP)
				) {
					/*both AP and we support Green Field mode, so tx in green field mode */
					pUmacInstance->WfmHtTxParameters &= (~WSM_TX_HT_FF_MIXED);
					pUmacInstance->WfmHtTxParameters |= WSM_TX_HT_FF_GREENFIELD;
					WFM_UMAC_WriteMib_UpdateJoinedMode_MixedOrGreenfieldMode(UmacHandle, WSM_SAM_MIXED_MODE);
					status = WFM_STATUS_SUCCESS;
					LOG_EVENT(DBG_WFM_ASSOC, "11n: Using GreenField Mode\n");
			} else {
				/*This is the case when Green field is not supported by AP */
				pUmacInstance->WfmHtTxParameters &= (~WSM_TX_HT_FF_GREENFIELD);
				pUmacInstance->WfmHtTxParameters |= WSM_TX_HT_FF_MIXED;
				WFM_UMAC_WriteMib_UpdateJoinedMode_MixedOrGreenfieldMode(UmacHandle, WSM_SAM_MIXED_MODE);
				status = WFM_STATUS_SUCCESS;
			}
		} else {
			/*Non-GreenField = 1 */

			pUmacInstance->WfmHtTxParameters &= (~WSM_TX_HT_FF_GREENFIELD);
			pUmacInstance->WfmHtTxParameters |= WSM_TX_HT_FF_MIXED;
			WFM_UMAC_WriteMib_UpdateJoinedMode_MixedOrGreenfieldMode(UmacHandle, WSM_SAM_MIXED_MODE);
			status = WFM_STATUS_SUCCESS;

		} /*end - if( u8UseGreenfieldMode == TRUE ) */
	}

	return status;
} /*end - WFM_UMAC_UseGreenfieldMode() */

/******************************************************************************
 * NAME:	WFM_UMAC_UseErpProtection
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets whether to use CTS/RTS protection when sending OFDM
 * frames.
 * \param UmacHandle	      - Handle to UMAC Instance.
 * \param u8UseErpProtection  - TRUE -> use protection,
 *			        FALSE -> don't use protection.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_UseErpProtection(UMAC_HANDLE UmacHandle,
				 uint8 u8UseErpProtection)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_FAILURE;

	OS_ASSERT(pUmacInstance);

	if (u8UseErpProtection == TRUE) {
		/* enable protection if not already enabled */
		if (!(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_CTSRTS_PROTECTION)) {
			/*protection was not enabled, so enable it now */
			pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_CTSRTS_PROTECTION;

			/*program wsm to enable protection */
			WFM_UMAC_WriteMib_NonErpProtection(UmacHandle, TRUE);
			status = WFM_STATUS_SUCCESS;
		}
	} else {
		/*disable protection if not already disabled */
		if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_CTSRTS_PROTECTION) {
			/*protection was enabled, so disabling it */
			pUmacInstance->Flags &= ~WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_CTSRTS_PROTECTION;

			/*program wsm to disable protection */
			WFM_UMAC_WriteMib_NonErpProtection(UmacHandle, FALSE);
			status = WFM_STATUS_SUCCESS;
		}
	}

	return status;
} /* end - WFM_UMAC_UseErpProtection() */

/******************************************************************************
 * NAME:	WFM_UMAC_DeleteAllKeysOneByOne
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function deletes all keys. Encryption is disabled till a new key is set.
 * When a new key is added, encryption will be automatically enabled.This
 * function should be called repetedly until its returning zero.
 * \param UmacHandle	  - Handle to UMAC Instance.
 * \returns zero when all the keys are deleted, otherwise 1
 *****************************************************************************/
uint32 WFM_UMAC_DeleteAllKeysOneByOne(UMAC_HANDLE UmacHandle)
{
	uint8 u8KeyEntryIndex;
	uint32 status = 1;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_HI | DBG_WFM_ENCRYPTION, "WFM_UMAC_DeleteAllKeysOneByOne()\n");

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	u8KeyEntryIndex = pSmgmtReq->operation.deassoc_req_params.KeyEntryIndex;

	do {
		/*Max arry index is N-1 */
		u8KeyEntryIndex--;

		/* Delete all keys from WSM */
		if (pUmacInstance->gVars.p.InfoHost.EncKey[u8KeyEntryIndex].InUse == TRUE) {
			UMAC_LL_REQ_REMOVE_KEY_ENTRY(
				UmacHandle,
				pUmacInstance->gVars.p.InfoHost.EncKey[u8KeyEntryIndex].UmacDot11AddKey.EntryIndex,
				pUmacInstance->linkId
				);

			pUmacInstance->gVars.p.InfoHost.EncKey[u8KeyEntryIndex].InUse = FALSE;

			status = 0;
			break;

		}

	} while (u8KeyEntryIndex);

	pSmgmtReq->operation.deassoc_req_params.KeyEntryIndex = u8KeyEntryIndex;

	if (!u8KeyEntryIndex) {
		uint8 i;
		pUmacInstance->gVars.p.InfoHost.WepKeyMap = 0;
		pUmacInstance->gVars.p.InfoHost.GroupKeyInstalled = 0;
		for (i = 0; i < pUmacInstance->gVars.p.maxNumClientSupport; i++)
			pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[i] = FALSE;
	} /* if( !u8KeyEntryIndex ) */

	return status;
} /* end WFM_UMAC_DeleteAllKeysOneByOne() */

/******************************************************************************
 * NAME:	UMAC_HandleRates
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function derives the different UMAC rate sets from the host connect
 * request.
 * \param UmacHandle     - Handle to UMAC Instance.
 * \param *pBssInfo      - Information about the AP to be connected with.
 * \returns zero when all the keys are deleted, otherwise 1
 *****************************************************************************/
void UMAC_HandleRates(UMAC_HANDLE UmacHandle, WFM_BSS_INFO *pBssInfo)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 DotNSuppRatesMask = 0;
	uint32 DotNBasicRatesMask = 0;

	/*Converting the Basic Supported Rates to WSM Bitmask */
	WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate(pBssInfo->NumRates, (uint8 *) pBssInfo->SupportedRates, &pUmacInstance->SupportedRates_WithAP_Abg, &pUmacInstance->BasicSupportedRates_WithAP_Abg);

	pUmacInstance->SupportedRates_WithAP_Abg &= pUmacInstance->OurAllSupportedRates_Abgn;
	pUmacInstance->BasicSupportedRates_WithAP_Abg &= pUmacInstance->OurAllBasicSupportedRates_Abgn;

	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP)) {
		/*a. Converting the 11n Supported Rates to WSM Bitmask */
		WFM_UMAC_Cvt_80211nFormat_Rate_To_WfmFormat_Rate(&pBssInfo->HtCapabilities.SupportedMCSSet, &DotNSuppRatesMask);

		/*b. Converting the 11n Basic Supported Rates to WSM Bitmask */
		WFM_UMAC_Cvt_80211nFormat_Rate_To_WfmFormat_Rate(&pBssInfo->HtOperationIe.BasicMCSSet, &DotNBasicRatesMask);

		/* c. green field mode check */
		if (pBssInfo->HtCapabilities.CapabilitiesInfo & WFM_HT_CAP_GREEN_FIELD & WFM_UMAC_HT_CAP_OURS) {
			/*set it into greenfield mode by default first. then check if it was the right thing to do. */
			/*the function WFM_UMAC_Process_HtOperationIe() below will revert to mixed mode */
			/*if it was not right to put it into greenfield mode now. */

			LOG_EVENT(DBG_WFM_ASSOC, "11n: Using GreenField Mode\n");
		} else {
			/*either AP or we don't support Green Field mode, so tx in mixed mode */
			LOG_EVENT(DBG_WFM_ASSOC, "11n: Using Mixed Mode\n");
		}
	} else {
		/* non 11n mode */
		pUmacInstance->WfmHtTxParameters = WSM_TX_HT_FF_NON_HT;
		LOG_EVENT(DBG_WFM_ASSOC, "11abg mode\n");
	}


	pUmacInstance->AllSupportedRates_WithAP_Abgn = pUmacInstance->SupportedRates_WithAP_Abg | DotNSuppRatesMask;
	pUmacInstance->AllBasicSupportedRates_WithAP_Abgn = pUmacInstance->BasicSupportedRates_WithAP_Abg | DotNBasicRatesMask;
	pUmacInstance->CurrentModeOpRates = pUmacInstance->AllBasicSupportedRates_WithAP_Abgn;	/*we have to send mgt packets, so choose this */

	/* Finding the rates supported in different modulation */
	pUmacInstance->ErpDssCckRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_DSSS_CCK_RATES_SUPPORTED);
	pUmacInstance->ErpOfdmRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_OFDM_RATES_SUPPORTED);
	pUmacInstance->HtOfdmRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_HT_OFDM_RATES_SUPPORTED);

	if (pUmacInstance->HtOfdmRates)
		pUmacInstance->CurrentModeOpRates = pUmacInstance->HtOfdmRates;
	else if (pUmacInstance->ErpOfdmRates)
		pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpOfdmRates;
	else if (pUmacInstance->ErpDssCckRates)
		pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpDssCckRates;
	UMAC_UpdateTxRatesBitmap(pUmacInstance, DEFAULT_LINK_ID);

	pUmacInstance->CurrentRateIndex = WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);

	if (pUmacInstance->ErpOfdmRates) {
		if (pBssInfo->Flags & WFM_BSS_INFO_FLAGS_PROT_ENABLED)
			pUmacInstance->MgtTxRateIndex = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(
				pUmacInstance->AllBasicSupportedRates_WithAP_Abgn
				);
		else
			pUmacInstance->MgtTxRateIndex = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(
				pUmacInstance->ErpOfdmRates
				);
	} else {
		pUmacInstance->MgtTxRateIndex = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(
			pUmacInstance->AllBasicSupportedRates_WithAP_Abgn
			);
	}

	return;
} /* end UMAC_HandleRates() */

/******************************************************************************
 * NAME:	UMAC_UpdateTxRatesBitmap
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates the Tx rates bitmap which is used in rate fallback
 * and rate recovery.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \returns WFM_STATUS_SUCCESS  when successful
 *****************************************************************************/
void UMAC_UpdateTxRatesBitmap(UMAC_HANDLE UmacHandle, uint8 linkId)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 currentHThighest;
	uint8 currentOFDMhighest;
	uint8 currentCCKhighest;
	uint8 currentRateIndex;
	uint8 prevRateIndex;
	uint32 rate_count;
	uint8 SupportedRateTbl[] = WFM_UMAC_80211_FORMAT_SUPPORTED_RATESET_ABGN;
	uint32 currentMode;
	currentHThighest = (uint8) WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->HtOfdmRates);
	currentOFDMhighest = (uint8) WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->ErpOfdmRates);
	currentCCKhighest = (uint8) WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->ErpDssCckRates);

	if (SupportedRateTbl[currentHThighest] > SupportedRateTbl[currentOFDMhighest])
		pUmacInstance->CurrentModeOpRates = pUmacInstance->HtOfdmRates;
	else
		pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpOfdmRates;

	currentRateIndex = (uint8) WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->CurrentModeOpRates);

	if (SupportedRateTbl[currentCCKhighest] > SupportedRateTbl[currentRateIndex])
		pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpDssCckRates;

	if (pUmacInstance->CurrentModeOpRates == pUmacInstance->ErpDssCckRates)
		pUmacInstance->TxRatesBitmap[linkId] |= UMAC_RATE_1;

	currentMode = pUmacInstance->CurrentModeOpRates;
	currentRateIndex = (uint8) WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->CurrentModeOpRates);
	while (currentRateIndex) {
		pUmacInstance->TxRatesBitmap[linkId] |= 1 << currentRateIndex;
		prevRateIndex = currentRateIndex;
		currentRateIndex = (uint8) WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(pUmacInstance->AllSupportedRates_WithAP_Abgn, currentRateIndex);
		if (prevRateIndex == currentRateIndex)
			break;

		if (currentMode == pUmacInstance->HtOfdmRates) {
			if (SupportedRateTbl[currentRateIndex] - SupportedRateTbl[currentOFDMhighest] < RATE_DIFFERENCE) {
				currentMode = pUmacInstance->ErpOfdmRates;
				currentRateIndex = (uint8) WFM_UMAC_GET_HIGHEST_RATE_INDEX(currentMode);
			} else if (SupportedRateTbl[currentRateIndex] - SupportedRateTbl[currentCCKhighest] < RATE_DIFFERENCE) {
				currentMode = pUmacInstance->ErpDssCckRates;
				currentRateIndex = (uint8) WFM_UMAC_GET_HIGHEST_RATE_INDEX(currentMode);
				pUmacInstance->TxRatesBitmap[linkId] |= UMAC_RATE_1;
			}
		} else if (currentMode == pUmacInstance->ErpOfdmRates) {
			if (SupportedRateTbl[currentRateIndex] - SupportedRateTbl[currentCCKhighest] < RATE_DIFFERENCE) {
				currentMode = pUmacInstance->ErpDssCckRates;
				currentRateIndex = (uint8) WFM_UMAC_GET_HIGHEST_RATE_INDEX(currentMode);
				pUmacInstance->TxRatesBitmap[linkId] |= UMAC_RATE_1;
			}
		}

	} /* while(currentRateIndex) */

	pUmacInstance->TxRetryRateBitMap[linkId] = pUmacInstance->TxRatesBitmap[linkId];
	if ((pUmacInstance->UmacTxblkackpolicy || pUmacInstance->UmacRxblkackpolicy)
	&& ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATING) || ((pUmacInstance->UmacPreviousState == UMAC_ASSOCIATING) && (pUmacInstance->UmacCurrentState == UMAC_PROCESSING))
	|| (pUmacInstance->UmacCurrentState == UMAC_REASSOCIATING) || ((pUmacInstance->UmacPreviousState == UMAC_REASSOCIATING) && (pUmacInstance->UmacCurrentState == UMAC_PROCESSING)))
	&& pUmacInstance->HtOfdmRates) {
		/* Use HT rates only if aggregation is turned on */
		uint32	TxBitMap = 0;
		uint32	currRateIndex;
		uint32	highestCckIndex;

		/* Fill up HT rates >= 39Mbps */
		currRateIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->HtOfdmRates);
		while (SupportedRateTbl[currRateIndex] >= WFM_802_11_HT_RATE_39_MBPS) {
			TxBitMap |= (1 << currRateIndex);
			prevRateIndex = (uint8)currRateIndex;
			currRateIndex = WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(pUmacInstance->HtOfdmRates, currRateIndex);
			if (prevRateIndex == currRateIndex)
				break;
		}

		/* Find out the highest ERP OFDM rate < 39Mbps */
		currRateIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->ErpOfdmRates);
		while (currRateIndex) {
			if (SupportedRateTbl[currRateIndex] < WFM_802_11_HT_RATE_39_MBPS)
				break;
			prevRateIndex = (uint8)currRateIndex;
			currRateIndex = WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(pUmacInstance->ErpOfdmRates, currRateIndex);
			if (prevRateIndex == currRateIndex)
				break;
		}

		/* Fill up ERP OFDM rates < 39Mpbs and > (11 + 5) Mbps */
		highestCckIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->ErpDssCckRates);

		while (SupportedRateTbl[currRateIndex] > SupportedRateTbl[highestCckIndex] + RATE_DIFFERENCE) {
			TxBitMap |= (1 << currRateIndex);
			prevRateIndex = (uint8)currRateIndex;
			currRateIndex = WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(pUmacInstance->ErpOfdmRates, currRateIndex);
			if (prevRateIndex == currRateIndex)
				break;
		}

		TxBitMap |= pUmacInstance->ErpDssCckRates;
		pUmacInstance->TxRatesBitmap[linkId] = TxBitMap;

		/* Now calculate the retry policy rate map */

		/*
		 * rate retry policy 1 is for frames with MaxTxRate = non MCS rate.
		 * Includes only non MCS rates < 36Mbps.
		 */
		TxBitMap &= (pUmacInstance->ErpOfdmRates|pUmacInstance->ErpDssCckRates);
		TxBitMap &= ~(1 << RATE_INDEX_A_36M);
		pUmacInstance->TxRetryRateBitMap[1] = TxBitMap;

		/*
		 * rate retry policy 0 is for frame with MaxTxRate = MCS rate.
		 * include all MCS rates + 5.5, 2, 1mbps.
		 */
		TxBitMap = pUmacInstance->HtOfdmRates|pUmacInstance->ErpDssCckRates;
		TxBitMap &= ~(1 << RATE_INDEX_B_11M);
		pUmacInstance->TxRetryRateBitMap[0] = TxBitMap;
	}

	rate_count = WFM_UMAC_GET_NUM_OF_RATES(pUmacInstance->TxRatesBitmap[linkId]);
	if (rate_count > WFM_DEFAULT_SHORT_RETRY_COUNT) {
		uint32 next, lowest, skip_count, skip_gap;
		uint32 TxRateMap;
		uint8 j = 0;

		TxRateMap = pUmacInstance->TxRetryRateBitMap[linkId];
		skip_count = rate_count - WFM_DEFAULT_SHORT_RETRY_COUNT;
		skip_gap = rate_count/skip_count;

		lowest = WFM_UMAC_GET_LOWEST_RATE_INDEX(TxRateMap);

		j = 1;
		next = WFM_UMAC_GET_HIGHEST_RATE_INDEX(TxRateMap);
		TxRateMap &= ~(1 << next);
		skip_count--;
		while (skip_count && (next > lowest)) {
			if (j == skip_gap) {
				TxRateMap &= ~(1 << next);
				skip_count--;
				j = 0;
			}
			prevRateIndex = (uint8)next;
			next = WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(TxRateMap, next);
			if (prevRateIndex == next)
				break;
			j++;
		}

		pUmacInstance->TxRetryRateBitMap[linkId] = TxRateMap;
	}
} /* end UMAC_UpdateTxRatesBitmap() */

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_AddKey
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function add a key. Adding a key will not start encryption unless it
 * was enabled by the host using other OIDs.
 * \param UmacHandle	     - Handle to UMAC Instance.
 * \param *pDot11AddKey      - Pointer to WFM_OID_802_11_KEY.
 * \returns WFM_STATUS_SUCCESS  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_AddKey(UMAC_HANDLE UmacHandle,
			      WFM_OID_802_11_KEY *pDot11AddKey)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	LMC_KEY_DATA *pKeyData;	/* Key inside PrivacyParams */

	PRIVACY_KEY_DATA *pPrivacyParams;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	OS_ASSERT(pUmacInstance);

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;
	pPrivacyParams = (PRIVACY_KEY_DATA *) pDot11AddKey;

	if (pPrivacyParams->KeyEntryIndex >= MAX_KEY_ENTRIES) {
		/*Invalid value */
		LOG_DATA(DBG_WFM_ERROR, "WFM_UMAC_SetOid_AddKey(): Invalid KeyIndex (%d)!\n", pPrivacyParams->KeyEntryIndex);
		Status = WFM_STATUS_BAD_PARAM;
		goto exit_handler;
	} /* if( pPrivacyParams->KeyEntryIndex >= MAX_KEY_ENTRIES ) */

	pKeyData = (LMC_KEY_DATA *) pPrivacyParams->Key;

	switch (pPrivacyParams->KeyType) {
	case eEncWEPDEFAULT:
		pUmacInstance->gVars.p.InfoHost.WepKeyMap |= (1 << pDot11AddKey->Key.WepGroupKey.KeyId);
		if (pDot11AddKey->Key.WepGroupKey.KeyId == pUmacInstance->gVars.p.InfoHost.WepDefaultKeyId) {
			uint8 i;
			for (i = 0; i < pUmacInstance->gVars.p.maxNumClientSupport; i++)
				pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[i] = 1;
			pUmacInstance->gVars.p.InfoHost.GroupKeyInstalled = 1;
		}
		break;
	case eEncWEPPAIRWISE:
		pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[pUmacInstance->linkId] = 1;
		pUmacInstance->gVars.p.InfoHost.WepPairwiseKey[pUmacInstance->linkId] = 1;
		break;
	case eEncTKIPGROUP:
	case eEncAESGROUP:
#if WAPI_SUPPORT
	case eEncWAPIGROUP:
#endif
		pUmacInstance->gVars.p.InfoHost.GroupKeyInstalled = 1;
		break;
	case eEncTKIPPAIRWISE:
	case eEncAESPAIRWISE:
#if WAPI_SUPPORT
	case eEncWAPIPAIRWISE:
#endif
		pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[pUmacInstance->linkId] = 1;
		break;
#if MGMT_FRAME_PROTECTION
	case eEncIGTKGROUP:
#endif
		break;
	default:
		LOG_EVENT(DBG_WFM_CURRENT_GM | DBG_WFM_ALWAYS, "ADD_KEY: eEncNONE\n");
		Status = WFM_STATUS_BAD_PARAM;
		goto exit_handler;

	} /* switch(pPrivacyParams->KeyType) */

	/*////////////PART-2////////////////// */
	/*Copy entry in wfm table */

	pUmacInstance->gVars.p.InfoHost.EncKey[pPrivacyParams->KeyEntryIndex].InUse = TRUE;

	pUmacInstance->gVars.p.InfoHost.EncKey[pPrivacyParams->KeyEntryIndex].linkId = pUmacInstance->linkId;

	OS_MemoryCopy(
		&pUmacInstance->gVars.p.InfoHost.EncKey[pPrivacyParams->KeyEntryIndex].UmacDot11AddKey,
		pDot11AddKey,
		sizeof(WFM_OID_802_11_KEY)
		);

	pSmgmtReq->operation.ul_triggered_params.pPrivacyParams = (uint8 *) pPrivacyParams;

 exit_handler:
	return Status;
} /* end - WFM_UMAC_SetOid_AddKey() */

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_DeleteKey
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function deletes a key. Note that deleting a key will not stop
 * encryption unless all keys were deleted. Encryption will be enabled again
 * when a key is added by the host.
 * \param UmacHandle       - Handle to UMAC Instance.
 * \param *pDot11RemoveKey - Pointer to WFM_OID_802_11_REMOVE_KEY.
 * \returns WFM_STATUS_SUCCESS  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_DeleteKey(UMAC_HANDLE UmacHandle,
				 WFM_OID_802_11_REMOVE_KEY *pDot11RemoveKey)
{
	uint32 Status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WFM_UMAC_ENC_KEY *pUmacKey;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(pDot11RemoveKey);

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	if (pDot11RemoveKey->EntryIndex >= MAX_KEY_ENTRIES) {
		Status = WFM_STATUS_BAD_PARAM;
		goto exit_handler;
	}

	pUmacKey = (WFM_UMAC_ENC_KEY *) &pUmacInstance->gVars.p.InfoHost.EncKey[pDot11RemoveKey->EntryIndex];

	OS_ASSERT(pUmacKey);

	if (!pUmacKey->InUse) {
		Status = WFM_STATUS_REQ_REJECTED;
		goto exit_handler;
	}

	switch (pUmacKey->UmacDot11AddKey.Type) {
	case eEncWEPDEFAULT:
		pUmacInstance->gVars.p.InfoHost.WepKeyMap &= ~(1<<pUmacKey->UmacDot11AddKey.Key.WepGroupKey.KeyId);
		if (!(pUmacInstance->gVars.p.InfoHost.WepKeyMap & (1 << pUmacInstance->gVars.p.InfoHost.WepDefaultKeyId))) {
			uint8 i;
			for (i = 0; i < pUmacInstance->gVars.p.maxNumClientSupport; i++) {
				if (pUmacInstance->gVars.p.InfoHost.WepPairwiseKey[i] == 0)
					pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[i] = 0;
			}
			pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[0] = 0;
			pUmacInstance->gVars.p.InfoHost.GroupKeyInstalled = 0;
		}
		break;
	case eEncWEPPAIRWISE:
		pUmacInstance->gVars.p.InfoHost.WepPairwiseKey[pUmacInstance->linkId] = 0;
		if (!(pUmacInstance->gVars.p.InfoHost.WepKeyMap & (1 << pUmacInstance->gVars.p.InfoHost.WepDefaultKeyId)))
			pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[pUmacInstance->linkId] = 0;

		break;

	case eEncTKIPGROUP:
	case eEncAESGROUP:
#if WAPI_SUPPORT
	case eEncWAPIGROUP:
#endif
		pUmacInstance->gVars.p.InfoHost.GroupKeyInstalled = 0;
		break;
	case eEncTKIPPAIRWISE:
	case eEncAESPAIRWISE:
#if WAPI_SUPPORT
	case eEncWAPIPAIRWISE:
#endif
		pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[pUmacKey->linkId] = 0;
		break;
#if MGMT_FRAME_PROTECTION
	case eEncIGTKGROUP:
#endif
		break;
	default:
		Status = WFM_STATUS_BAD_PARAM;
		goto exit_handler;

	} /* switch(pUmacKey->UmacDot11AddKey.Type) */

	if (pUmacKey->UmacDot11AddKey.EntryIndex != pDot11RemoveKey->EntryIndex) {
		/*Wrong Key entry index */
		Status = WFM_STATUS_BAD_PARAM;
		goto exit_handler;
	}

	pUmacKey->InUse = FALSE;

	pSmgmtReq->operation.ul_triggered_params.RemoveKeyIndex = pDot11RemoveKey->EntryIndex;

 exit_handler:
	return Status;
} /* end - WFM_UMAC_SetOid_DeleteKey() */

/******************************************************************************
 * NAME:	UMAC_Initialize_Internal_TxDescList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the internal TxDescriptor list kept by umac.
 * \param UmacHandle - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void UMAC_Initialize_Internal_TxDescList(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	int Count;

	OS_ASSERT(pUmacInstance);

	/*Initializing the TxDesc Store */
	for (Count = 0; Count < UMAC_NUM_INTERNAL_TX_BUFFERS; Count++) {
		pUmacInstance->gVars.umacTxDescInt[Count].pTxDesc = NULL;
		pUmacInstance->gVars.umacTxDescInt[Count].PacketId = 0;

	}
} /* end UMAC_Initialize_Internal_TxDescList() */

/******************************************************************************
 * NAME:	UMAC_DeInit_Internal_TxDescList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function De-initializes the internal TxDescriptor list kept by umac.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param linkId     - Link Id of the STA
 * \returns none.
 *****************************************************************************/
void UMAC_DeInit_Internal_TxDescList(UMAC_HANDLE UmacHandle, uint8 linkId)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	int Count;
	uint32 memstatus;

	OS_ASSERT(pUmacInstance);

	/*Initializing the TxDesc Store */
	for (Count = 0; Count < UMAC_NUM_INTERNAL_TX_BUFFERS; Count++) {
		if (pUmacInstance->gVars.umacTxDescInt[Count].pTxDesc != NULL) {
			memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
				UmacHandle,
				pUmacInstance->gVars.umacTxDescInt[Count].pTxDesc
				);

			OS_ASSERT(memstatus);

			UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.umacTxDescInt[Count].pTxDesc);

			pUmacInstance->gVars.umacTxDescInt[Count].pTxDesc = NULL;
			pUmacInstance->gVars.umacTxDescInt[Count].PacketId = 0;

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		}
	} /* for(Count=0 ;Count<UMAC_NUM_INTERNAL_TX_BUFFERS ; Count++)*/
} /* end UMAC_DeInit_Internal_TxDescList() */

/******************************************************************************
 * NAME:	UMAC_Store_Internal_TxDesc
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function stores an entry the internal TxDescriptor list kept by umac.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pTxDesc     -  TxDescpritor to be stored
 * \param umacTxDesc[] - Array of UMAC Tx Descriptors
 * \param numBuffs     - Number of buffers to be searched
 * \returns none.
 *****************************************************************************/
void UMAC_Store_Internal_TxDesc(UMAC_HANDLE UmacHandle,
				UMAC_TX_DESC *pTxDesc,
				UMAC_TX_PKT_INT umacTxDesc[],
				uint8 numBuffs,
				uint8 headerOffset,
				void *pDriverInfo)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	int Count;

	OS_ASSERT(pUmacInstance);

	/*Initializing the TxDesc Store */
	for (Count = 0; Count < numBuffs; Count++) {
		if (umacTxDesc[Count].pTxDesc == NULL) {
			umacTxDesc[Count].pTxDesc = pTxDesc;
			umacTxDesc[Count].PacketId = pTxDesc->PacketId;
			umacTxDesc[Count].headerOffset = headerOffset ;
			umacTxDesc[Count].pDriverInfo = pDriverInfo;
			break;
		}
	} /* for(Count=0 ;Count<UMAC_NUM_INTERNAL_TX_BUFFERS ; Count++)*/
} /* end UMAC_Store_Internal_TxDesc() */

/******************************************************************************
 * NAME:	UMAC_Release_Internal_TxDesc
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function releases an entry the internal TxDescriptor list kept by umac.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pTxDesc      - TxDescpritor to be relased.
 * \param umacTxDesc[]  - Array of UMAC Tx Descriptors
 * \param numBuffs      - Number of buffers to be searched
 * \returns UMAC_TX_DESC* pointer to the stored descriptor, NULL if no
 *                        descriptor found.
 *****************************************************************************/
UMAC_TX_DESC *UMAC_Release_Internal_TxDesc(UMAC_HANDLE UmacHandle,
					   UMAC_TX_DESC *pTxDesc,
					   UMAC_TX_PKT_INT umacTxDesc[],
					   uint8 numBuffs,
					   void **pDriverInfo)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pStoredTxDesc = NULL;
	int Count;

	OS_ASSERT(pUmacInstance);

	/*Initializing the TxDesc Store */
	for (Count = 0; Count < numBuffs; Count++) {
		if (umacTxDesc[Count].PacketId == (pTxDesc->PacketId & ~(UMAC_PACKETID_PS_BUFFRED_FRAME))) {
			pStoredTxDesc = umacTxDesc[Count].pTxDesc;
			if (umacTxDesc[Count].headerOffset)
				pStoredTxDesc =
				(UMAC_TX_DESC *)((uint8 *)pStoredTxDesc -
				umacTxDesc[Count].headerOffset);
			umacTxDesc[Count].pTxDesc = NULL;
			umacTxDesc[Count].PacketId = CFG_HI_NUM_REQS;
			umacTxDesc[Count].headerOffset = 0;
			*pDriverInfo = umacTxDesc[Count].pDriverInfo;
			umacTxDesc[Count].pDriverInfo = NULL;
			break;

		}
	} /* for(Count=0 ;Count<UMAC_NUM_INTERNAL_TX_BUFFERS ; Count++)*/

	return pStoredTxDesc;
} /* end UMAC_Release_Internal_TxDesc() */

/******************************************************************************
 * NAME:	UMAC_Get_Internal_TxDesc
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function Get an entry the internal TxDescriptor list kept by umac.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pTxDesc      - TxDescpritor to be relased.
 * \param umacTxDesc[]  - Array of UMAC Tx Descriptors
 * \param numBuffs      - Number of buffers to be searched
 * \returns UMAC_TX_DESC* pointer to the stored descriptor, NULL if no
 *                        descriptor found.
 *****************************************************************************/
UMAC_TX_DESC *UMAC_Get_Internal_TxDesc(UMAC_HANDLE UmacHandle,
					   UMAC_TX_DESC *pTxDesc,
					   UMAC_TX_PKT_INT umacTxDesc[],
					   uint8 numBuffs)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pStoredTxDesc = NULL;
	int Count;

	OS_ASSERT(pUmacInstance);

	/*Initializing the TxDesc Store */
	for (Count = 0; Count < numBuffs; Count++) {
		if (umacTxDesc[Count].PacketId == (pTxDesc->PacketId & ~(UMAC_PACKETID_PS_BUFFRED_FRAME))) {
			pStoredTxDesc = umacTxDesc[Count].pTxDesc;
			/*if (umacTxDesc[Count].headerOffset)
				pStoredTxDesc =
				(UMAC_TX_DESC *)((uint8 *)pStoredTxDesc -
				umacTxDesc[Count].headerOffset);*/
			break;
		}
	} /* for(Count=0 ;Count<UMAC_NUM_INTERNAL_TX_BUFFERS ; Count++)*/

	return pStoredTxDesc;
} /* end UMAC_Get_Internal_TxDesc() */

/******************************************************************************
 * NAME:	UMAC_Send_Beacons_ProbRsp_To_UpperLayer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends a Beacon or prob responce to the Upper Layer/Host
 * \param UmacHandle   - Handle to UMAC Instance.
 * \returns WFM_STATUS_SUCCESS when successful.
 *****************************************************************************/
uint32 UMAC_Send_Beacons_ProbRsp_To_UpperLayer(UMAC_HANDLE UmacHandle)
{
	uint32 ListSize = 0;
	WFM_BSS_CACHE_INFO_IND *DevInfo = NULL;
	WFM_DEV_ELEMENT *pDevContainer = NULL;
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	ListSize = WFM_UMAC_DEV_GetListSize(pUmacInstance);

	if (ListSize >= WFM_UMAC_DEV_LIST_MAX_SIZE) {
		/*
		   List is full, so get the oldest element and send it to the
		   host, to make some space for the current element
		 */
		pDevContainer = (WFM_DEV_ELEMENT *) WFM_UMAC_DEV_GetNextElement(pUmacInstance);

		if (pDevContainer) {

			DevInfo = (WFM_BSS_CACHE_INFO_IND *) UMAC_GET_IND_BUF(UmacHandle, pDevContainer->ElementSize);

			if (DevInfo) {
				DevInfo = (WFM_BSS_CACHE_INFO_IND *) ((uint8 *) DevInfo + WFM_HI_MESSAGE_HDR_SIZE);
				OS_MemoryCopy(DevInfo, &pDevContainer->DeviceElement, pDevContainer->ElementSize);

				LOG_MACADDR(DBG_WFM_SCAN, "WFM:Scan-BSSID of AP=", DevInfo->BssId);

				/* 2. Send it to the host */
				WFM_UMAC_2_UL_SendDevInfo(
					UmacHandle,
					(uint8 *) DevInfo,
					WFM_BSS_CACHE_INFO_IND_ID,
					(uint16) pDevContainer->ElementSize
					);

			} else {
				LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "Discarding Beacon? ERROR?\n\n");
				Result = WFM_STATUS_OUT_OF_RESOURCES;
			}
		} else
			Result = WFM_STATUS_OUT_OF_RESOURCES;

	} /* if ( ListSize >= WFM_UMAC_DEV_LIST_MAX_SIZE ) */

	return Result;
} /* end UMAC_Send_Beacons_ProbRsp_To_UpperLayer() */

/******************************************************************************
 * NAME:	UMAC_ProcessPendingTxRequests
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns Tx confirmation for the pending Tx requests buffered
 * in UMAC.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param linkId       - Link Id of the assoicated STA
 * \returns uint32 Corresponding UMAC status code
 *****************************************************************************/
uint32 UMAC_ProcessPendingTxRequests(UMAC_HANDLE UmacHandle, uint8 linkId)
{
	UMAC_TX_PKT *pUmacTxPkt;
	uint16 Count;
	UMAC_TX_DATA_CNF *pTxDataCnf;
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_HI_MESSAGE *pHiMessage = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	void *pDriverInfo = NULL;
	CHECK_POINTER_IS_VALID(pHiMessage);

	/*Clearing the TxDes list, if there's any outstanding one */
	for (Count = 0; Count < CFG_HI_NUM_REQS; Count++) {
		pUmacTxPkt = (UMAC_TX_PKT *) &pUmacInstance->gVars.TxDescStore[Count];

#if P2P_MINIAP_SUPPORT
		if (pUmacTxPkt->linkId == linkId) {
#endif	/* P2P_MINIAP_SUPPORT */

			if (pUmacTxPkt->pTxDesc != NULL) {
				OS_LOCK(pUmacInstance->gVars.p.TxDescStoreLock);

				pTxDataCnf = (UMAC_TX_DATA_CNF *) pUmacTxPkt->pMsgPtr;

				OS_ASSERT(pTxDataCnf);

				pUmacTxPkt->pMsgPtr = NULL;

				pHiMessage = (WFM_HI_MESSAGE *) pTxDataCnf->pUlHdr;

				OS_ASSERT(pHiMessage);

				pHiMessage->PayLoad.wfm_transmit_cnf.Reference = pUmacTxPkt->reference;

				pUmacTxPkt->reference = 0;

				WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacTxPkt->pTxDesc);


#if P2P_MINIAP_SUPPORT
				UMAC_Release_Internal_TxDesc(
					UmacHandle,
					pUmacTxPkt->pTxDesc,
					pUmacInstance->gVars.ApUmacTxDesc,
					UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
					&pDriverInfo
					);
#endif	/* P2P_MINIAP_SUPPORT */

				UMAC_RELEASE_PACKET_ID(UmacHandle, pUmacTxPkt->pTxDesc->PacketId);
				/*Releasing the Tx Descriptor */
				UMAC_LL_RELEASE_TX_DESC(
							UmacHandle,
							pUmacTxPkt->pTxDesc
							);

				pUmacTxPkt->pTxDesc = NULL;

				OS_UNLOCK(pUmacInstance->gVars.p.TxDescStoreLock);

				pHiMessage->PayLoad.wfm_transmit_cnf.Status = WFM_STATUS_REQ_REJECTED;

				OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
				pUmacInstance->NumTxBufPending[linkId]--;
				OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

				pHiMessage->MessageId = WFM_TRANSMIT_CNF_ID;
				pHiMessage->MessageLength = (WFM_HI_MESSAGE_HDR_SIZE + sizeof(WFM_TRANSMIT_CNF));

				OS_ASSERT(pUmacInstance->gVars.p.UpperLayerTxcnf);

				pUmacInstance->gVars.p.UpperLayerTxcnf(
					pUmacInstance->gVars.p.ulHandle,
					pTxDataCnf
					);

			} /* if( pUmacTxPkt->pTxDesc  !=  NULL ) */
#if P2P_MINIAP_SUPPORT
		} /* if(pUmacTxPkt->linkId == linkId) */
#endif	/* P2P_MINIAP_SUPPORT */

	} /* for( Count=0 ; Count < CFG_HI_NUM_REQS ; Count++)*/

	if (Count >= CFG_HI_NUM_REQS)
		goto exit_handler;

 exit_handler:
	return Result;

} /* end UMAC_ProcessPendingTxRequests() */

/******************************************************************************
 * NAME:	UMAC_ProcessPendingPsBuffTxRequests
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns Tx confirmation for the pending PS Buffer Tx requests
 * buffered in UMAC.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param linkId       - Link Id of the assoicated STA
 * \param pTxDescriptor     - Pointer to TxDescriptor of Frame.
 * \returns uint32 Corresponding UMAC status code
 *****************************************************************************/
uint32 UMAC_ProcessPendingPsBuffTxRequests(UMAC_HANDLE UmacHandle,
					   uint8 linkId,
					   UMAC_TX_DESC *pTxDescriptor)
{
	UMAC_TX_PKT *pUmacTxPkt;
	UMAC_TX_DATA_CNF *pTxDataCnf;
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_HI_MESSAGE *pHiMessage = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	void *pDriverInfo = NULL;
	UMAC_TX_DESC *pStoredTxDesc = NULL;
	uint32 packetId = 0;

	OS_ASSERT(pTxDescriptor);

	if (pTxDescriptor->PacketId & UMAC_PACKETID_FLAG_EXT_MGMT) {
		pStoredTxDesc = UMAC_Release_Internal_TxDesc(UmacHandle, pTxDescriptor, pUmacInstance->gVars.ApUmacTxDesc, UMAC_NUM_AP_INTERNAL_TX_BUFFERS, &pDriverInfo);
		if (pStoredTxDesc) {
			UMAC_RELEASE_PACKET_ID(UmacHandle, pTxDescriptor->PacketId);
			UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
				UmacHandle,
				MOVE_TO_START_OF_DEV_IF_TX_REQ(pStoredTxDesc)
				);

			UMAC_MEM_REMOVE_STAMP(
				MOVE_TO_START_OF_DEV_IF_TX_REQ(pStoredTxDesc)
				);

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

			goto  exit_handler;
		}
	}

	packetId = pTxDescriptor->PacketId;

	if (packetId >= CFG_HI_NUM_REQS)
		packetId &= (CFG_HI_NUM_REQS - 1);

	/*Clearing the TxDes list, if there's any outstanding one */
	pUmacTxPkt = (UMAC_TX_PKT *) &pUmacInstance->gVars.TxDescStore[packetId];

	if ((pUmacTxPkt->pTxDesc != NULL) && (pUmacTxPkt->linkId == linkId)) {
		OS_LOCK(pUmacInstance->gVars.p.TxDescStoreLock);

		pTxDataCnf = (UMAC_TX_DATA_CNF *) pUmacTxPkt->pMsgPtr;

		OS_ASSERT(pTxDataCnf);

		pUmacTxPkt->pMsgPtr = NULL;

		pHiMessage = (WFM_HI_MESSAGE *) pTxDataCnf->pUlHdr;

		OS_ASSERT(pHiMessage);

		pHiMessage->PayLoad.wfm_transmit_cnf.Reference = pUmacTxPkt->reference;

		pUmacTxPkt->reference = 0;

		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacTxPkt->pTxDesc);

		if (pUmacTxPkt->pTxDesc->PacketId & UMAC_PACKETID_PS_BUFFRED_FRAME)
			pUmacTxPkt->pTxDesc->PacketId &= ~(UMAC_PACKETID_PS_BUFFRED_FRAME);

#if P2P_MINIAP_SUPPORT
		UMAC_Release_Internal_TxDesc(
			UmacHandle,
			pUmacTxPkt->pTxDesc,
			pUmacInstance->gVars.ApUmacTxDesc,
			UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
			&pDriverInfo
			);
#endif	/* P2P_MINIAP_SUPPORT */

		UMAC_RELEASE_PACKET_ID(UmacHandle, pUmacTxPkt->pTxDesc->PacketId);

		/*Releasing the Tx Descriptor */
		UMAC_LL_RELEASE_TX_DESC(
					UmacHandle,
					pUmacTxPkt->pTxDesc
					);

		pUmacTxPkt->pTxDesc = NULL;

		OS_UNLOCK(pUmacInstance->gVars.p.TxDescStoreLock);

		pHiMessage->PayLoad.wfm_transmit_cnf.Status = WFM_STATUS_REQ_REJECTED;

		OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
		pUmacInstance->NumTxBufPending[linkId]--;
		OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		pHiMessage->MessageId = WFM_TRANSMIT_CNF_ID;
		pHiMessage->MessageLength = (WFM_HI_MESSAGE_HDR_SIZE + sizeof(WFM_TRANSMIT_CNF));

		OS_ASSERT(pUmacInstance->gVars.p.UpperLayerTxcnf);

		pUmacInstance->gVars.p.UpperLayerTxcnf(
			pUmacInstance->gVars.p.ulHandle,
			pTxDataCnf
			);

	} /* if( pUmacTxPkt->pTxDesc  !=  NULL ) */

 exit_handler:
	return Result;

} /* end UMAC_ProcessPendingPsBuffTxRequests() */

/******************************************************************************
 * NAME:	UMAC_ProcessSetMibRequests
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the direct setparameter request issued by upper
 * layers to WSM
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pMsg	-  Message to be processed
 * \returns One of the following values
 *	  UMAC_INT_STATUS_SUCCESS
 *	  UMAC_INT_STATUS_DUPLICATE_REQUEST
 *	  UMAC_INT_STATUS_BAD_PARAM
 *	  UMAC_INT_STATUS_VALUE_STORED
 *****************************************************************************/
uint32 UMAC_ProcessSetMibRequests(UMAC_HANDLE UmacHandle,
				  WFM_SET_PARAM_REQ *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	uint32 Status = UMAC_INT_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	switch (pMsg->ParameterId) {
	case WFM_DEVICE_OID_802_11_ADD_KEY:

		Result = WFM_UMAC_SetOid_AddKey(
					UmacHandle,
					&(pMsg->ParameterData.dot11AddKey)
					);

		break;
	case WFM_DEVICE_OID_802_11_REMOVE_KEY:
		Result = WFM_UMAC_SetOid_DeleteKey(
					UmacHandle,
					&(pMsg->ParameterData.dot11RemoveKey)
					);

		break;
	case WFM_DEVICE_OID_802_11_POWER_MODE:

		OS_MemoryReset(&pUmacInstance->gVars.p.PsMode,
				sizeof(pUmacInstance->gVars.p.PsMode));
		pUmacInstance->gVars.p.PsMode.PmMode = pMsg->ParameterData.psMode.PmMode;
		pUmacInstance->gVars.p.PsMode.FastPsmIdlePeriod = pMsg->ParameterData.psMode.FastPsmIdlePeriod;
		pUmacInstance->gVars.p.PsMode.ApPsmChangePeriod = pMsg->ParameterData.psMode.ApPsmChangePeriod;

		break;
	case WFM_DEVICE_OID_802_11_SET_UAPSD:
		/* Reject the request if uapsdFlags are modified post
		association */
		if (UMAC_ASSOCIATED != pUmacInstance->UmacCurrentState) {
			OS_MemoryCopy(
				&pUmacInstance->gVars.p.setUapsdInfo,
				&pMsg->ParameterData.setUAPSD,
				sizeof(WFM_OID_802_11_SET_UAPSD)
				);

			Status = UMAC_INT_STATUS_VALUE_STORED;
		} else {
			if (pUmacInstance->gVars.p.setUapsdInfo.uapsdFlags != pMsg->ParameterData.setUAPSD.uapsdFlags) {
				Status = UMAC_INT_STATUS_BAD_PARAM;
			} else {
				OS_MemoryCopy(
					&pUmacInstance->gVars.p.setUapsdInfo,
					&pMsg->ParameterData.setUAPSD,
					sizeof(WFM_OID_802_11_SET_UAPSD)
					);
			}
		}

		break;
	case WFM_DEVICE_OID_802_11_BLOCK_ACK_POLICY:
		{
			uint8 IsUpdated = 0;

			if (pUmacInstance->UmacTxblkackpolicy != pMsg->ParameterData.block_ack_policy.BlockAckTxTidPolicy) {
				pUmacInstance->UmacTxblkackpolicy = (uint8) pMsg->ParameterData.block_ack_policy.BlockAckTxTidPolicy;
				IsUpdated = 1;
			}

			if (pUmacInstance->UmacRxblkackpolicy != pMsg->ParameterData.block_ack_policy.BlockAckRxTidPolicy) {
				pUmacInstance->UmacRxblkackpolicy = (uint8) pMsg->ParameterData.block_ack_policy.BlockAckRxTidPolicy;
				IsUpdated = 1;
			}

			if (IsUpdated == 0) {
				/*Its a duplicate request */
				Status = UMAC_INT_STATUS_DUPLICATE_REQUEST;
			}

			break;
		}
	case WFM_DEVICE_OID_802_11_UPDATE_EPTA_CONFIG_DATA:
		pUmacInstance->gVars.p.SerialMgmtInfo.operation.ul_triggered_params.pEptaConfigInfo = (uint8 *) &pMsg->ParameterData.update_EPTA_config_data;

		break;
	case WFM_DEVICE_OID_802_11_SET_AUTO_CALIBRATION_MODE:

		break;
	case WFM_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE:

		if (pUmacInstance->gVars.p.OprPwrMode != pMsg->ParameterData.OprPwrMode)
			pUmacInstance->gVars.p.OprPwrMode = pMsg->ParameterData.OprPwrMode;
		else {
			/*Its a duplicate request */
			Status = UMAC_INT_STATUS_DUPLICATE_REQUEST;
		}

		break;
	case WFM_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD:
		break;
	case WFM_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT:
		break;
	case WFM_DEVICE_OID_802_11_SET_ETHER_TYPE_FILTER:
		pUmacInstance->gVars.p.SerialMgmtInfo.operation.ul_triggered_params.pMsg = (uint8 *) &pMsg->ParameterData.setEtherTypeFilter;

		break;
	case WFM_DEVICE_OID_802_11_SET_UDP_PORT_FILTER:
		pUmacInstance->gVars.p.SerialMgmtInfo.operation.ul_triggered_params.pMsg = (uint8 *) &pMsg->ParameterData.setUdpPortFilter;

		break;
	case WFM_DEVICE_OID_802_11_SET_MAGIC_FRAME_FILTER:
		pUmacInstance->gVars.p.SerialMgmtInfo.operation.ul_triggered_params.pMsg = (uint8 *) &pMsg->ParameterData.setMagicFrameFilter;

		break;
	case WFM_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER:
		pUmacInstance->gVars.p.SerialMgmtInfo.operation.ul_triggered_params.pMsg = (uint8 *) &pMsg->ParameterData.ArpFilter;
		if (pMsg->ParameterData.ArpFilter.filterMode == 1) {
			pUmacInstance->isArpFilterEnabled = 1;
		}
		else {
			pUmacInstance->isArpFilterEnabled = 0;
		}

		break;
	case UMAC_DEVICE_OID_802_11_SET_ARP_KEEP_ALIVE_PERIOD:
		pUmacInstance->gVars.p.SerialMgmtInfo.operation.ul_triggered_params.pMsg = (uint8 *) &pMsg->ParameterData.ArpKeepAlivePeriod;

		break;
	case UMAC_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER:
		pUmacInstance->gVars.p.SerialMgmtInfo.operation.ul_triggered_params.pMsg = (uint8 *) &pMsg->ParameterData.multicastAddrFilter;

		break;
	case UMAC_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID:
		pUmacInstance->gVars.p.SerialMgmtInfo.operation.ul_triggered_params.pMsg = (uint8 *) &pMsg->ParameterData.wepDefaultKeyId;
		pUmacInstance->gVars.p.InfoHost.WepDefaultKeyId = pMsg->ParameterData.wepDefaultKeyId.wepDefaultKeyId;
		if (pUmacInstance->gVars.p.InfoHost.WepKeyMap) {
			if (pUmacInstance->gVars.p.InfoHost.WepKeyMap & (1 << pUmacInstance->gVars.p.InfoHost.WepDefaultKeyId)) {
				uint8 i;
				for (i = 0; i < pUmacInstance->gVars.p.maxNumClientSupport; i++)
					pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[i] = 1;
			} else {
				uint8 i;
				for (i = 0; i < pUmacInstance->gVars.p.maxNumClientSupport; i++)
					pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[i] = 0;
			}
		}

		break;
	case UMAC_DEVICE_OID_802_11_CHANGE_CHANNEL:
		if (pUmacInstance->ChannelChange.ChannelState == UMAC_CHANNEL_STATE_NONE) {
			pUmacInstance->ChannelChange.ChannelState = UMAC_CHANNEL_STATE_CHANGING;
			pUmacInstance->ChannelChange.PreviosChannel = (uint8)pUmacInstance->Channelnumber;
			pUmacInstance->ChannelChange.ChangedChannel = pMsg->ParameterData.ChangeChannel.Channel;
			pUmacInstance->ChannelChange.PreviousState = (uint8)pUmacInstance->UmacCurrentState;

			pSmgmtReq->operation.ul_triggered_params.SwitchChReq.NewChannelNumber = pMsg->ParameterData.ChangeChannel.Channel;
			pSmgmtReq->operation.ul_triggered_params.SwitchChReq.Band = pMsg->ParameterData.ChangeChannel.Band;
			pSmgmtReq->operation.ul_triggered_params.SwitchChReq.DurationInKus = pMsg->ParameterData.ChangeChannel.DurationInKus;
			pSmgmtReq->operation.ul_triggered_params.SwitchChReq.ChannelMode = UMAC_SC_CHANNELMODE_ENHANCED;
			pSmgmtReq->operation.ul_triggered_params.SwitchChReq.NumTxSlotsReserved = UMAC_SC_RESV_TX_SLOT;

			pUmacInstance->SwitchChannelReq = UMAC_SC_REQ_EXTERNAL;

			/* Updating rates during p2p find to prevent fallback on CCK rates */
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
			pSmgmtReq->operation.ul_triggered_params.SwitchChReq.BasicRateSet = pUmacInstance->CurrentModeOpRates;

		} else {
			Status = UMAC_INT_STATUS_BAD_PARAM;
		}
		break;
	case UMAC_DEVICE_OID_802_11_RESTORE_CHANNEL:
		if (pUmacInstance->ChannelChange.ChannelState == UMAC_CHANNEL_STATE_CHANGED) {
			pSmgmtReq->operation.ul_triggered_params.SwitchChReq.ChannelMode = UMAC_SC_CHANNELMODE_ENHANCED;
			pSmgmtReq->operation.ul_triggered_params.SwitchChReq.Flags = UMAC_SC_F_ABORT;
			pUmacInstance->ChannelChange.ChannelState = UMAC_CHANNEL_STATE_RESTORING;
			pUmacInstance->SwitchChannelReq = UMAC_SC_REQ_EXTERNAL;
		} else {
			Status = UMAC_INT_STATUS_BAD_PARAM;
		}
		break;
	case WFM_DEVICE_OID_802_11_UPDATE_VENDOR_IE:
		break;
	case WFM_DEVICE_OID_802_11_SET_LISTEN_INTERVAL:
		pUmacInstance->gVars.p.SerialMgmtInfo.operation.ul_triggered_params.NumBeaconPeriods = pUmacInstance->DTIMPeriod * pUmacInstance->dtimSkippingCount;
		break;
#if P2P_MINIAP_SUPPORT
	case UMAC_DEVICE_OID_802_11_P2P_PS_MODE:
		OS_MemoryCopy((void *)&pUmacInstance->gVars.p.p2pPsMode,
			(void *)&pMsg->ParameterData.p2pPSMode,
			sizeof(UMAC_P2P_PS_MODE));
		break;
#endif
	default:
		Status = UMAC_INT_STATUS_BAD_PARAM;
	} /* switch(pMsg->ParameterId) */

	if (Result != WFM_STATUS_SUCCESS)
		Status = umac_convert_to_internal_status(Result);

	return Status;
} /* end UMAC_ProcessSetMibRequests() */

/******************************************************************************
 * NAME:	UMAC_ProcessGetMibCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the direct getparameter request issued by upper
 * layers to WSM
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pMsg	-  Message to be processed
 * \returns Corresponding UMAC status code
 *****************************************************************************/
uint32 UMAC_ProcessGetMibCnf(UMAC_HANDLE UmacHandle, WFM_GET_PARAM_CNF *pMsg)
{
	uint32 Status = UMAC_INT_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	switch (pMsg->ParameterId) {
	case WFM_DEVICE_OID_802_11_STATISTICS:
		OS_MemoryCopy(
			&pMsg->ParameterData.dot11Statistics,
			&pUmacInstance->statisticsInfo,
			sizeof(WFM_OID_802_11_STATISTICS)
			);
		pMsg->ParameterLength = sizeof(WFM_OID_802_11_STATISTICS);
		break;

#if P2P_MINIAP_SUPPORT
	case UMAC_DEVICE_OID_802_11_GET_GROUP_TSC:
		OS_MemoryCopy(
			&pMsg->ParameterData.dot11GroupTSC,
			&pUmacInstance->groupTSC,
			sizeof(UMAC_OID_802_11_GET_GROUP_TSC)
			);
		pMsg->ParameterLength = sizeof(UMAC_OID_802_11_GET_GROUP_TSC);
		break;
#endif	/*P2P_MINIAP_SUPPORT */

	case UMAC_DEVICE_OID_802_11_GET_DEVICE_MAC_ADDRESS:
		OS_MemoryCopy(
			&pMsg->ParameterData.deviceMacAddress,
			&pUmacInstance->MacAddress[0],
			6
			);
		pMsg->ParameterLength = sizeof(UMAC_DEVICE_MAC_ADDRESS);
		break;

	default:
		Status = UMAC_INT_STATUS_BAD_PARAM;
	} /* switch(pMsg->ParameterId) */

	return Status;
} /* end UMAC_ProcessGetMibCnf() */

/******************************************************************************
 * NAME:	UMAC_ProcessSetMibCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the direct setparameter request issued by upper
 * layers to WSM
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pMsg		-  Message to be processed
 * \returns Corresponding UMAC status code
 *****************************************************************************/
uint32 UMAC_ProcessSetMibCnf(UMAC_HANDLE UmacHandle, WFM_SET_PARAM_CNF *pMsg)
{
	uint32 Status = UMAC_INT_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	switch (pMsg->ParameterId) {
	case WFM_DEVICE_OID_802_11_POWER_MODE:

		if (TRUE == pUmacInstance->sendDisassoc) {
			pUmacInstance->sendDisassoc = FALSE;
			Status = WFM_STATUS_FAILURE;
		}

		break;
	case UMAC_DEVICE_OID_802_11_CHANGE_CHANNEL:
		/* pUmacInstance->ChannelChange.ChannelState = UMAC_CHANNEL_STATE_CHANGED; */
		break;
	case UMAC_DEVICE_OID_802_11_RESTORE_CHANNEL:
		/* pUmacInstance->ChannelChange.ChannelState = UMAC_CHANNEL_STATE_NONE; */
		break;
	case UMAC_DEVICE_OID_802_11_P2P_PS_MODE:
		Status = pUmacInstance->gVars.p.UmacAsyncMgmtReqStatus;
		break;
	default:
		Status = WFM_STATUS_SUCCESS;
	} /* switch(pMsg->ParameterId) */

	return Status;
} /* end UMAC_ProcessGetMibCnf() */

/******************************************************************************
 * NAME:	UMAC_ManageBlackListPeer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function manages the blacklist of peer address kept by UMAC.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - MAC Address to be added/removed from black list.
 * \returns Corresponding UMAC black list status code
 *****************************************************************************/
uint32 UMAC_ManageBlackListPeer(UMAC_HANDLE UmacHandle, uint8 *pAddr)
{
	uint32 Result = UMAC_BLKLST_STATUS_ERROR;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 count;

	for (count = 0; count < NUM_BLACK_LIST_ENTRIES; count++) {
		if (OS_MemoryEqual(pAddr, WFM_MAC_ADDRESS_SIZE, (uint8 *) &pUmacInstance->gVars.p.BlackList[count], WFM_MAC_ADDRESS_SIZE)) {
			OS_MemoryReset(
				&pUmacInstance->gVars.p.BlackList[count],
				6
				);

			Result = UMAC_BLKLST_STATUS_CLEARED;
			goto exit_handler;
		}
	} /* for(count=0;count<NUM_BLACK_LIST_ENTRIES; count++)*/

	for (count = 0; count < NUM_BLACK_LIST_ENTRIES; count++) {

		if (IS_NLL_MAC_ADDRESS(pUmacInstance->gVars.p.BlackList[count])) {
			OS_MemoryCopy(
				&pUmacInstance->gVars.p.BlackList[count],
				pAddr,
				6
				);

			Result = UMAC_BLKLST_STATUS_ADDED;
			goto exit_handler;
		}
	} /* for(count=0;count<NUM_BLACK_LIST_ENTRIES; count++)*/

	OS_ASSERT(0);

 exit_handler:
	return Result;

} /* end UMAC_ManageBlackListPeer() */

/******************************************************************************
 * NAME:	UMAC_AddDeleteUserAllowedPeer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function regulates the peer access to MiniAP
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - Mac Address + Action required.
 * \returns Corresponding status code
 *****************************************************************************/
uint32 UMAC_AddDeleteUserAllowedPeer(UMAC_HANDLE UmacHandle,  UMAC_OID_ALLOWED_DATABASE *macaddr_db)
{
	uint32 Result = UMAC_INT_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 count;

	/* Checking if the new whitelist or blacklist entry can be added */
	if ((macaddr_db->actionType == WFM_BLACKLIST && pUmacInstance->miniap_blacklistcount == MAX_BLACKLIST_ENTRIES) ||
		(macaddr_db->actionType == WFM_WHITELIST && pUmacInstance->miniap_whitelistcount == MAX_WHITELIST_ENTRIES)) {

		Result = UMAC_DATABASE_FULL;
		goto exit_handler;
	}

	switch (macaddr_db->actionType) {
	case WFM_BLACKLIST:
	case WFM_WHITELIST:

		for (count = 0; count < MAX_DB_ENTRIES; count++) {
			if (!pUmacInstance->peerAllowDB[count].blacklist_status &&  !pUmacInstance->peerAllowDB[count].whitelist_status) {
				OS_MemoryCopy(
					pUmacInstance->peerAllowDB[count].stamacaddr,
					macaddr_db->mac_addr,
					WFM_MAC_ADDRESS_SIZE
					);

				if (macaddr_db->actionType == WFM_BLACKLIST) {
					pUmacInstance->peerAllowDB[count].blacklist_status = 1;
					pUmacInstance->miniap_blacklistcount++;
				} else {
					pUmacInstance->peerAllowDB[count].whitelist_status = 1;
					pUmacInstance->miniap_whitelistcount++;
				}
				break;
			}
		}
		break;

	case UMI_DELETE:
		for (count = 0; count < MAX_DB_ENTRIES; count++) {
			if (OS_MemoryEqual(macaddr_db->mac_addr, WFM_MAC_ADDRESS_SIZE, (uint8 *) pUmacInstance->peerAllowDB[count].stamacaddr, WFM_MAC_ADDRESS_SIZE)) {
				OS_MemoryReset(
					pUmacInstance->peerAllowDB[count].stamacaddr,
					WFM_MAC_ADDRESS_SIZE
					);

				if (pUmacInstance->peerAllowDB[count].whitelist_status) {
					pUmacInstance->peerAllowDB[count].whitelist_status = 0;
					pUmacInstance->miniap_whitelistcount--;
				} else {
					pUmacInstance->peerAllowDB[count].blacklist_status = 0;
					pUmacInstance->miniap_blacklistcount--;
				}
				break;
			}
		}
		break;

	}
 exit_handler:
	return Result;

} /* end UMAC_AddDeleteUserAllowedPeer() */

/******************************************************************************
 * NAME:	UMAC_SearchAllowedPeer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function searches the new connection sta address in internal table
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - Mac Address.
 * \returns corresponding action code
 *****************************************************************************/
WFM_MAC_ADDR_ACTION UMAC_SearchAllowedPeer(UMAC_HANDLE UmacHandle,  uint8 *pAddr)
{
	WFM_MAC_ADDR_ACTION Result = WFM_NOTAVAILABLE;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 count;

	if (!pUmacInstance->miniap_blacklistcount && !pUmacInstance->miniap_whitelistcount)
		goto exit_handler;


	for (count = 0; count < MAX_DB_ENTRIES; count++) {
		if (OS_MemoryEqual(pAddr, WFM_MAC_ADDRESS_SIZE, (uint8 *) pUmacInstance->peerAllowDB[count].stamacaddr, WFM_MAC_ADDRESS_SIZE)) {
				if (pUmacInstance->peerAllowDB[count].whitelist_status)
					Result = WFM_WHITELIST;
				else
					Result = WFM_BLACKLIST;
				break;
			}
	}
 exit_handler:
	return Result;
} /* end UMAC_SearchAllowedPeer() */
/******************************************************************************
 * NAME:	UMAC_IsBlackListedPeer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function checks whether the given Address is a black listed one.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - MAC Address to be to be verified.
 * \returns Corresponding UMAC black list status code
 *****************************************************************************/
uint32 UMAC_IsBlackListedPeer(UMAC_HANDLE UmacHandle, uint8 *pAddr)
{
	uint32 Result = UMAC_BLKLST_NOT_LISTED;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 count;

	for (count = 0; count < NUM_BLACK_LIST_ENTRIES; count++) {
		if (OS_MemoryEqual(pAddr, WFM_MAC_ADDRESS_SIZE, (uint8 *) &pUmacInstance->gVars.p.BlackList[count], WFM_MAC_ADDRESS_SIZE)) {

			Result = UMAC_BLKLST_LISTED;
			break;
		}
	} /* for(count=0;count<NUM_BLACK_LIST_ENTRIES; count++)*/

	return Result;
} /* end UMAC_IsBlackListedPeer() */

/******************************************************************************
 * NAME:	UMAC_ClearBlackList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function clears the black list.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void UMAC_ClearBlackList(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_MemoryReset(
			&pUmacInstance->gVars.p.BlackList[0],
			(WFM_MAC_ADDRESS_SIZE * NUM_BLACK_LIST_ENTRIES)
			);

} /* end UMAC_ClearBlackList() */

/******************************************************************************
 * NAME:UMAC_ReleaseTxResources
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function releases all the resources UMAC has allocated for a Tx
 * operation.
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pTxDesc	- Pointer to TxDescriptor.
 * \param linkId	- Link Id of the STA
 * \returns none.
 *****************************************************************************/
void UMAC_ReleaseTxResources(UMAC_HANDLE UmacHandle,
			     UMAC_TX_DESC *pTxDesc,
			     uint8 linkId)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	void *pDriverInfo = NULL;

	LOG_EVENT(DBG_WFM_ERROR, "Tx Failure\n");

	if (pTxDesc->PacketId & UMAC_PACKETID_PS_BUFFRED_FRAME)
		pTxDesc->PacketId &= ~(UMAC_PACKETID_PS_BUFFRED_FRAME);

	UMAC_MEM_REMOVE_STAMP(pDevIfTxReq);

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

	OS_STOP_TIMER(pUmacInstance->gVars.p.UmacTimers.pUMACTimer);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[linkId]--;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

} /* end UMAC_ReleaseTxResources() */

/******************************************************************************
 * NAME:UMAC_ManageAsynMgmtOperations
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function manages the set of Management operations to be carried out in
 * a sequential fasion.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \returns Corresponding UMAC status code.
 *****************************************************************************/
uint32 UMAC_ManageAsynMgmtOperations(UMAC_HANDLE UmacHandle)
{
	uint32 status = WFM_STATUS_PENDING;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtInfo;
	uint32 NextIteration = 0;
	UMAC_POWER_MODE psModeEnabled[1] = { {WFM_PS_MODE_ENABLED, 0, 0, 0} };

	OS_ASSERT(pUmacInstance);

	if (pUmacInstance->UmacCurrentState != UMAC_PROCESSING)
		UMAC_SET_STATE_PROCESSING(UmacHandle);

	pSmgmtInfo = &pUmacInstance->gVars.p.SerialMgmtInfo;
	pSmgmtInfo->status  = WFM_STATUS_PENDING;

	do {

		NextIteration = 0;
		if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_SCAN_REQ) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				NextIteration = WFM_UMAC_WriteMib_TemplateFrame_ProbeReq(
					UmacHandle,
					pSmgmtInfo->operation.scan_mib_params.phy_band,
					pSmgmtInfo->operation.scan_mib_params.type
					);
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;

			} /* switch(pSmgmtInfo->ReqNum) */

		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_SCAN_REQ) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_PSMODE_UPDATE) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				NextIteration = WFM_UMAC_SendReqToWsm_SetPsMode(UmacHandle, &pUmacInstance->gVars.p.PsMode);
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
			}
		}
#if P2P_MINIAP_SUPPORT
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_FIND_REQ) {
			switch (pSmgmtInfo->ReqNum) {

			case 0:
				NextIteration = WFM_UMAC_WriteMib_FindInfo(UmacHandle);

				pSmgmtInfo->ReqNum = 1;
				break;

			case 1:
				NextIteration = WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UmacHandle, WSM_TMPLT_PRBRES);

				pSmgmtInfo->ReqNum = 2;
				break;

				/*
				  Use Update-IE Request to update the probe
				  request template for P2P WildCard SSID
				*/

			case 2:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;

			} /* switch(pSmgmtInfo->ReqNum) */

		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_FIND_REQ) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_SEND_DEASSOC) {

			switch (pSmgmtInfo->ReqNum) {
			case 0:
				NextIteration = WFM_UMAC_WriteMib_P2P_PSMode(UmacHandle);
				pSmgmtInfo->ReqNum = 1;
				break;

			case 1:
				NextIteration = WFM_UMAC_SendReqToWsm_SetPsMode(UmacHandle, &pUmacInstance->gVars.p.PsMode);
				pSmgmtInfo->ReqNum = 2;
				break;

			case 2:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
			}
		} /* if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_SEND_DEASSOC) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_GO_NEGOTIATION) {
			JOIN_PARAMETERS *pJoinParams = NULL;
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				NextIteration = UMAC_LL_REQ_STOP_FIND(UmacHandle);

				pSmgmtInfo->ReqNum = 1;
				break;

			case 1:
				UMAC_Configure_Join_Req(
							UmacHandle,
							&pJoinParams
							);
				NextIteration = UMAC_LL_REQ_JOIN(
								UmacHandle,
								pJoinParams
								);

				pSmgmtInfo->ReqNum = 2;
				break;

			case 2:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;

			} /* switch(pSmgmtInfo->ReqNum) */

		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_GO_NEGOTIATION) */
#endif	/* P2P_MINIAP_SUPPORT */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_JOIN_REQ) {
			uint8 probeband;
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				if (pUmacInstance->ChannelChange.ChannelState == UMAC_CHANNEL_STATE_CHANGED) {
					pUmacInstance->ChannelChange.ChannelState = UMAC_CHANNEL_STATE_NONE;
					NextIteration = UMAC_SendReqToWsm_Reset(UmacHandle, 0, 0);
				} else {
					NextIteration = 1;
				}

				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				if (pSmgmtInfo->operation.join_mib_params.WriteBlkAckPolicy)
					/* Set Block Ack Policy */
					NextIteration = WFM_UMAC_WriteMib_BlockAckPolicy(UmacHandle, pUmacInstance->UmacTxblkackpolicy, pUmacInstance->UmacRxblkackpolicy);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				NextIteration = WFM_UMAC_WriteMib_SlotTime(UmacHandle, pSmgmtInfo->operation.join_mib_params.SloteTime);

				pSmgmtInfo->ReqNum = 3;

				break;
			case 3:
				/* use cts/rts protection */
				NextIteration = WFM_UMAC_WriteMib_NonErpProtection(UmacHandle, pSmgmtInfo->operation.join_mib_params.UseNonProtection);
				pSmgmtInfo->ReqNum = 4;
				break;
			case 4:

				NextIteration = WFM_UMAC_UseErpProtection(UmacHandle, pSmgmtInfo->operation.join_mib_params.HtOperationIe.ErpProtection);
				pSmgmtInfo->ReqNum = 5;
				break;
			case 5:
				NextIteration = WFM_UMAC_UseGreenfieldMode(UmacHandle, pSmgmtInfo->operation.join_mib_params.HtOperationIe.GreenFieldMode);
				pSmgmtInfo->ReqNum = 6;
				break;

			case 6:
				if (pSmgmtInfo->operation.join_mib_params.HtOperationIe.WriteHtProtection)
					NextIteration = WFM_UMAC_WriteMib_SetHTProtection(UmacHandle, pSmgmtInfo->operation.join_mib_params.HtOperationIe.SetHtProtection);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 7;
				break;
			case 7:
				/*clear bcn filter table */
				NextIteration = WFM_UMAC_WriteMib_BeaconFilterTable(UmacHandle, FALSE);
				pSmgmtInfo->ReqNum = 8;
				break;
			case 8:
				/*now fill it */
				NextIteration = WFM_UMAC_WriteMib_BeaconFilterTable(UmacHandle, TRUE);
				pSmgmtInfo->ReqNum = 9;
				break;
			case 9:
				/*enable bcn filter */
				NextIteration = WFM_UMAC_WriteMib_BeaconFilterEnable(UmacHandle, TRUE, 0);
				pSmgmtInfo->ReqNum = 10;
				break;
			case 10:
				if (pSmgmtInfo->operation.join_mib_params.hostMsgTypeFilter)
					NextIteration = WFM_UMAC_WriteMib_HostMsgTypeFilter(UmacHandle);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 11;
				break;
			case 11:
				NextIteration = WFM_UMAC_WriteMib_PowerLevel(UmacHandle,
#if USE_DEFAULT_POWER
						WFM_UMAC_DEFAULT_POWER_LEVEL
#else
						pSmgmtInfo->operation.join_mib_params.powerLevel
#endif
						);
				pSmgmtInfo->ReqNum = 12;

				break;
			case 12:
				if (pUmacInstance->Band == PHY_BAND_2G)
					probeband = WFM_BSS_LIST_SCAN_2_4G_BAND;
				else
					probeband = WFM_BSS_LIST_SCAN_5G_BAND;

				NextIteration =
					WFM_UMAC_WriteMib_TemplateFrame_ProbeReq
						(
							UmacHandle,
							probeband,
							0
							);
				pSmgmtInfo->ReqNum = 13;
				break;

			case 13:
				if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode)
					NextIteration = WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UmacHandle, WSM_TMPLT_BCN);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 14;
				break;

			case 14:
				if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode)
					NextIteration = WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UmacHandle, WSM_TMPLT_PRBRES);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 15;
				break;
			case 15:
				if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode)
					NextIteration = WFM_UMAC_WriteMib_OverrideInternalTxRate(UmacHandle);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 16;
				break;
			case 16:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;

			} /* switch(pSmgmtInfo->ReqNum) */

		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_JOIN_REQ) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_JOIN_SUCCESS) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:

				NextIteration = WFM_UMAC_WriteMib_RtsThreshold(UmacHandle, pSmgmtInfo->operation.join_success_params.RtsThreashold);
				pSmgmtInfo->ReqNum = 1;
				break;

			case 1:
				NextIteration = WFM_UMAC_WriteMib_TxRetryRatePolicy(UmacHandle, 0	/* Policy Index */
						);
				pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				NextIteration = wfm_umac_set_default_ac_parameters(UmacHandle, pUmacInstance->AllSupportedRates_WithAP_Abgn, pUmacInstance->Band);

				pSmgmtInfo->ReqNum = 3;
				break;
			case 3:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			} /* switch(pSmgmtInfo->ReqNum) */

		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_JOIN_SUCCESS) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_ASSOC_SUCCESS) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				if (pSmgmtInfo->operation.assoc_success_params.WriteJoinModeBasicRateSet)
					NextIteration = WFM_UMAC_WriteMib_UpdateJoinedMode_BasicRateSet(UmacHandle, pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				NextIteration = WFM_UMAC_SendReqToWsm_TxQueueParams(UmacHandle, (uint8) pSmgmtInfo->operation.assoc_success_params.QueueId);

				pSmgmtInfo->operation.assoc_success_params.QueueId++;
				if (pSmgmtInfo->operation.assoc_success_params.QueueId >= WFM_UMAC_NUM_TX_QUEUES)
					pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				if (pSmgmtInfo->operation.assoc_success_params.WriteEdcaParam)
					NextIteration = wfm_umac_set_edca_parameters(UmacHandle, &pSmgmtInfo->operation.assoc_success_params.EdcaParam);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 3;
				break;
			case 3:
				NextIteration = WFM_UMAC_WriteMib_OverrideInternalTxRate(UmacHandle);
				pSmgmtInfo->ReqNum = 4;
				break;
			case 4:
				/*Set Beacon Wakeup Period */
				NextIteration = WFM_UMAC_WriteMib_BeaconWakeUpPeriod(UmacHandle, pSmgmtInfo->operation.assoc_success_params.NumBeaconPeriods);
				pSmgmtInfo->ReqNum = 5;
				break;
			case 5:
				/* Send NullData Template frame */
				NextIteration = WFM_UMAC_WriteMib_TemplateFrame_NullData(UmacHandle, pSmgmtInfo->operation.assoc_success_params.IsQosLink);
				pSmgmtInfo->ReqNum = 6;
				break;
			case 6:
				/* If link is QoS enabled, write Non-QoS null frame template also */
				if (pSmgmtInfo->operation.assoc_success_params.IsQosLink)
					NextIteration = WFM_UMAC_WriteMib_TemplateFrame_NullData(UmacHandle, 0);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 7;

				break;
			case 7:
				/* Send PsPoll Template frame */
				NextIteration = WFM_UMAC_WriteMib_TemplateFrame_PsPoll(UmacHandle);
				pSmgmtInfo->ReqNum = 8;
				break;
			case 8:
				NextIteration = WFM_UMAC_WriteMib_TemplateFrame_ArpReply(UmacHandle);
				pSmgmtInfo->ReqNum = 9;
				break;
			case 9:
				NextIteration = WFM_UMAC_SendReqToWsm_SetBssParams(UmacHandle, UMAC_SET_BSS_F_AID);
				pSmgmtInfo->ReqNum = 10;
				break;
			case 10:
				NextIteration = WFM_UMAC_WriteMib_TxRetryRatePolicy(UmacHandle, 0	/* Policy Index */);
				pSmgmtInfo->ReqNum = 11;
				break;
			case 11:
				if (pUmacInstance->TxRetryRateBitMap[1])
					NextIteration = WFM_UMAC_WriteMib_TxRetryRatePolicy(UmacHandle, 1);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 12;
				break;
			case 12:
				if (pUmacInstance->gVars.WmmParamElement.QoSInfo)
					NextIteration = WFM_UMAC_WriteMib_SetUAPSDInformation(UmacHandle);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 13;
				break;
			case 13:
				if (pUmacInstance->gVars.p.RcpiRssiThresholdRcvd)
					NextIteration = WFM_UMAC_WriteMib_SetRcpiRssiThreshold(UmacHandle);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 14;
				break;
			case 14:
#if MGMT_FRAME_PROTECTION
				if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_RSN_IE_SUPPORTED_BY_AP)
					NextIteration = WFM_UMAC_WriteMib_ProtectedMgmtPolicy(UmacHandle);
				else
#endif	/* MGMT_FRAME_PROTECTION */
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 15;
				break;
			case 15:
				NextIteration = WFM_UMAC_WriteMib_UseMultiTxCnfMsg(UmacHandle);
				pSmgmtInfo->ReqNum = 16;
				break;
			case 16:
				NextIteration = WFM_UMAC_WriteMib_KeepAlivePeriod(UmacHandle);
				pSmgmtInfo->ReqNum = 17;
				break;
			case 17:
				NextIteration = WFM_UMAC_WriteMib_P2P_PSMode(UmacHandle);
				pSmgmtInfo->ReqNum = 18;
				break;
			case 18:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			} /* switch(pSmgmtInfo->ReqNum) */

		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_ASSOC_SUCCESS) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_DEL_ALL_KEYS) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				if (pSmgmtInfo->operation.deassoc_req_params.KeyEntryIndex)
					NextIteration = WFM_UMAC_DeleteAllKeysOneByOne(UmacHandle);

				if ((!pSmgmtInfo->operation.deassoc_req_params.KeyEntryIndex))
					pSmgmtInfo->ReqNum = 1;

				break;
			case 1:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			} /* switch(pSmgmtInfo->ReqNum) */

		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_DEL_ALL_KEYS) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_DYNAMIC_UPDATIONS) {
			UMAC_RUNTIME *pRunTime;
			pRunTime = (UMAC_RUNTIME *) &pSmgmtInfo->run_time;
			switch (pSmgmtInfo->ReqNum) {
			case 0:

				if (pRunTime->WriteSloteTime)
					NextIteration = WFM_UMAC_WriteMib_SlotTime(UmacHandle, pRunTime->SlotTime);
				else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				if (pRunTime->WriteUpdateJoinMode)
					NextIteration = WFM_UMAC_WriteMib_UpdateJoinedMode_PreambleType(UmacHandle, pRunTime->PreambleType);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				if (pRunTime->HtOperationIe.WriteGreenFieldMode)
					NextIteration = WFM_UMAC_UseGreenfieldMode(UmacHandle, pRunTime->HtOperationIe.GreenFieldMode);
				else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 3;
				break;
			case 3:
				if (pRunTime->HtOperationIe.WriteErpProtection)
					NextIteration = WFM_UMAC_UseErpProtection(UmacHandle, pRunTime->HtOperationIe.ErpProtection);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 4;
				break;
			case 4:
				if (pRunTime->WriteEdcaParam)
					NextIteration = wfm_umac_set_edca_parameters(UmacHandle, &pRunTime->EdcaParam);
				else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 5;
				break;
			case 5:
				if (pRunTime->IsChSwitchAnnounce) {
					NextIteration = WFM_UMAC_SendReqToWsm_SwitchChReq(UmacHandle, (uint8 *)&pRunTime->SwitchChReq);
					pRunTime->IsChSwitchAnnounce = FALSE;
				} else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 6;
				break;
			case 6:
				if (pRunTime->BeaconWakeupPeriod) {
					NextIteration = WFM_UMAC_WriteMib_BeaconWakeUpPeriod(UmacHandle, pRunTime->BeaconWakeupPeriod);
					pRunTime->BeaconWakeupPeriod = 0;
				} else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 7;

				break;
			case 7:
				if (pRunTime->HtOperationIe.WriteHtProtection)
					NextIteration = WFM_UMAC_WriteMib_SetHTProtection(UmacHandle, pRunTime->HtOperationIe.SetHtProtection);
				else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 8;

				break;
			case 8:
				if (pRunTime->UpdatePsMode)
					NextIteration = WFM_UMAC_SendReqToWsm_SetPsMode(UmacHandle, &pUmacInstance->gVars.p.PsMode);
				else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 9;

				break;
			case 9:
				if (pRunTime->updateNoaAttrib)
					NextIteration = WFM_UMAC_WriteMib_P2P_PSMode(UmacHandle);
				else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 10;

				break;
			case 10:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			} /* switch(pSmgmtInfo->ReqNum) */
		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_DYNAMIC_UPDATIONS)*/
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_INIT) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				NextIteration = WFM_UMAC_ReadMib_MacAddress(UmacHandle, pUmacInstance->MacAddress);
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				/*
				* As we are initialized, so set the operational
				* Power mode with host configured value. The
				* host may over ride this by setting the OID
				* UMI_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE
				*/
				NextIteration = WFM_UMAC_WriteMib_OperationalPwrMode(UmacHandle, pUmacInstance->gVars.p.OprPwrMode);
				pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				/* Update EDCA parmeters for default mode (i.e. B Mode) */
				NextIteration = wfm_umac_set_default_ac_parameters(UmacHandle, 0x0F, 0);
				pSmgmtInfo->ReqNum = 3;
				break;
			case 3:
				NextIteration = WFM_UMAC_WriteMib_SetRcpiRssiThreshold(UmacHandle);
				pSmgmtInfo->ReqNum = 4;
				break;
			case 4:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				pSmgmtInfo->status  = WFM_STATUS_SUCCESS;
				break;
			} /* switch(pSmgmtInfo->ReqNum) */
		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_INIT) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_HOST_UL_TRIGGERED) {
			if (pSmgmtInfo->operation.ul_triggered_params.Category == WFM_SET_PARAM_REQ_ID) {
				/* uint8 Channel; */
				switch (pSmgmtInfo->operation.ul_triggered_params.Activity) {
				case UMAC_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_WepDefaultKeyId(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.pMsg);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */
					break;
				case WFM_DEVICE_OID_802_11_ADD_KEY:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = UMAC_LL_REQ_ADD_KEY_ENTRY(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.pPrivacyParams, pUmacInstance->linkId);
						pSmgmtInfo->ReqNum = 1;

						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;
				case WFM_DEVICE_OID_802_11_REMOVE_KEY:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = UMAC_LL_REQ_REMOVE_KEY_ENTRY(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.RemoveKeyIndex, pUmacInstance->linkId);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;
				case WFM_DEVICE_OID_802_11_POWER_MODE:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_SendReqToWsm_SetPsMode(UmacHandle, &pUmacInstance->gVars.p.PsMode);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;
				case WFM_DEVICE_OID_802_11_BLOCK_ACK_POLICY:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_BlockAckPolicy(UmacHandle, pUmacInstance->UmacTxblkackpolicy, pUmacInstance->UmacRxblkackpolicy);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;
				case WFM_DEVICE_OID_802_11_UPDATE_EPTA_CONFIG_DATA:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_EPTAConfigData(UmacHandle, (WFM_UPDATE_EPTA_CONFIG_DATA *) pSmgmtInfo->operation.ul_triggered_params.pEptaConfigInfo);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;
				case WFM_DEVICE_OID_802_11_SET_AUTO_CALIBRATION_MODE:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						WFM_UMAC_WriteMib_SetAutoCalibrationMode(UmacHandle, pUmacInstance->setAutoCalibrationMode);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */
					break;
				case WFM_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_OperationalPwrMode(UmacHandle, pUmacInstance->gVars.p.OprPwrMode);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */
					break;
				case WFM_DEVICE_OID_802_11_SET_UAPSD:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						if (pUmacInstance->gVars.WmmParamElement.QoSInfo)
							NextIteration = WFM_UMAC_WriteMib_SetUAPSDInformation(UmacHandle);
						else
							NextIteration = 1;

						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;
				case WFM_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_SetRcpiRssiThreshold(UmacHandle);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					}
					break;
				case WFM_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_SendReqToWsm_SetBssParams(
											UmacHandle,
											UMAC_SET_BSS_F_BEACON_LOST_COUNT_ONLY
											);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;

					}

					break;
				case WFM_DEVICE_OID_802_11_SET_ETHER_TYPE_FILTER:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_EtherTypeDataFrameFilter(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.pMsg);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */
					break;
				case WFM_DEVICE_OID_802_11_SET_UDP_PORT_FILTER:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_UdpPortDataFrameFilter(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.pMsg);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */
					break;
				case WFM_DEVICE_OID_802_11_SET_MAGIC_FRAME_FILTER:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_MagicDataFrameFilter(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.pMsg);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */
					break;
				case WFM_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_ArpIpAddrFilter(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.pMsg);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;
				case UMAC_DEVICE_OID_802_11_SET_ARP_KEEP_ALIVE_PERIOD:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_ArpKeepAlivePeriod(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.pMsg);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;
				case UMAC_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_MulticastAddrFilter(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.pMsg);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;
				case UMAC_DEVICE_OID_802_11_CHANGE_CHANNEL:
					/* Channel = pSmgmtInfo->operation.ul_triggered_params.SwitchChReq->NewChannelNumber; */
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_WriteMib_TxRetryRatePolicy(UmacHandle, 0	/* Policy Index */);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						NextIteration = WFM_UMAC_SendReqToWsm_SwitchChReq(UmacHandle, (uint8 *)&pSmgmtInfo->operation.ul_triggered_params.SwitchChReq);
						pSmgmtInfo->ReqNum = 2;
						break;
					case 2:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					}

					break;
				case UMAC_DEVICE_OID_802_11_RESTORE_CHANNEL:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_SendReqToWsm_SwitchChReq(UmacHandle, (uint8 *)&pSmgmtInfo->operation.ul_triggered_params.SwitchChReq);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					}

					break;
				case WFM_DEVICE_OID_802_11_UPDATE_VENDOR_IE:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						if ((pUmacInstance->VendorIe.pProbeReqIE) &&
						(pUmacInstance->VendorIe.probeReqIElength)) {
							NextIteration = WFM_UMAC_WriteMib_TemplateFrame_ProbeReq(
								UmacHandle,
								WFM_BSS_LIST_SCAN_2_4G_BAND,
								WSM_TMPLT_PRBREQ
								);
						} else
							NextIteration = 1;
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						if ((pUmacInstance->VendorIe.pProbeRespIE) &&
						(pUmacInstance->VendorIe.probeRespIElength)) {
							NextIteration =
								WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UmacHandle, WSM_TMPLT_PRBRES);
						} else
							NextIteration = 1;
						pSmgmtInfo->ReqNum = 2;
						break;
					case 2:
						if ((pUmacInstance->VendorIe.pBeaconIE) &&
						(pUmacInstance->VendorIe.beaconIElength)) {
							NextIteration =
								WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UmacHandle, WSM_TMPLT_BCN);
						} else
							NextIteration = 1;
						pSmgmtInfo->ReqNum = 3;
						break;
					case 3:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					}

					break;
				case WFM_DEVICE_OID_802_11_SET_LISTEN_INTERVAL:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						/*Set Beacon Wakeup Period */
						NextIteration = WFM_UMAC_WriteMib_BeaconWakeUpPeriod(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.NumBeaconPeriods);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					}
					break;
				case UMAC_DEVICE_OID_802_11_P2P_PS_MODE:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
							NextIteration = WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UmacHandle,
								WSM_TMPLT_BCN);
						else
							NextIteration = 1;
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
							NextIteration = WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UmacHandle,
								WSM_TMPLT_PRBRES);
						else
							NextIteration = 1;
						pSmgmtInfo->ReqNum = 2;
						break;
					case 2:
						/*If P2P client then clear the beacon filter table*/
						if (pUmacInstance->operatingMode < OPERATING_MODE_AP)
							NextIteration = WFM_UMAC_WriteMib_BeaconFilterTable(UmacHandle, FALSE);
						else
							NextIteration = 1;
						pSmgmtInfo->ReqNum = 3;
						break;
					case 3:
						/*Fill the beacon table*/
						if (pUmacInstance->operatingMode < OPERATING_MODE_AP)
							NextIteration = WFM_UMAC_WriteMib_BeaconFilterTable(UmacHandle, TRUE);
						else
							NextIteration = 1;
						pSmgmtInfo->ReqNum = 4;
						break;
					case 4:
						/*Enable Beacon filtering*/
						if (pUmacInstance->operatingMode < OPERATING_MODE_AP)
							NextIteration = WFM_UMAC_WriteMib_BeaconFilterEnable(UmacHandle, TRUE, 0);
						else
							NextIteration = 1;
						pSmgmtInfo->ReqNum = 5;
						break;
					case 5:
						NextIteration = WFM_UMAC_WriteMib_P2P_PSMode(UmacHandle);
						pSmgmtInfo->ReqNum = 6;
						break;
					case 6:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */
					break;
				default:
					LOG_EVENT(DBG_WFM_ERROR, "\nERROR: Bad MIB req from Upper layer\n");
				} /* switch(ul_triggered_params.Activity) */

			} /* if(Category == WFM_SET_PARAM_REQ_ID) */
			else if (pSmgmtInfo->operation.ul_triggered_params.Category == WFM_GET_PARAM_REQ_ID) {
				switch (pSmgmtInfo->operation.ul_triggered_params.Activity) {
				case WFM_DEVICE_OID_802_11_STATISTICS:
					switch (pSmgmtInfo->ReqNum) {
					case 0:

						NextIteration = WFM_UMAC_ReadMib_CounterTable(UmacHandle);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */
					break;
#if P2P_MINIAP_SUPPORT
				case UMAC_DEVICE_OID_802_11_GET_GROUP_TSC:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_Get_Group_TSC(UmacHandle);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */
					break;
#endif	/* P2P_MINIAP_SUPPORT */
				case UMAC_DEVICE_OID_802_11_GET_DEVICE_MAC_ADDRESS:
					switch (pSmgmtInfo->ReqNum) {
					case 0:
						NextIteration = WFM_UMAC_ReadMib_MacAddress(UmacHandle, pUmacInstance->MacAddress);
						pSmgmtInfo->ReqNum = 1;
						break;
					case 1:
						UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
						status = WFM_STATUS_SUCCESS;
						break;
					} /* switch(pSmgmtInfo->ReqNum) */

					break;

				default:
					LOG_EVENT(DBG_WFM_ERROR, "\nERROR: Bad MIB req from Upper layer\n");
				} /* switch(ul_triggered_params.Activity) */
			} /* if(Category == WFM_GET_PARAM_REQ_ID) */
			else if (pSmgmtInfo->operation.ul_triggered_params.Category == WFM_MEMORY_WRITE_REQ_ID) {
				switch (pSmgmtInfo->ReqNum) {
				case 0:
					NextIteration = UMAC_LL_WRITE_MEMORY(UmacHandle, (HI_MSG_HDR *) pSmgmtInfo->operation.ul_triggered_params.pMsg);
					pSmgmtInfo->ReqNum = 1;
					break;
				case 1:
					UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
					status = WFM_STATUS_SUCCESS;
					break;
				} /* switch(pSmgmtInfo->ReqNum) */
			} else if (pSmgmtInfo->operation.ul_triggered_params.Category == WFM_MEMORY_READ_REQ_ID) {
				switch (pSmgmtInfo->ReqNum) {
				case 0:
					NextIteration = UMAC_LL_READ_MEMORY(UmacHandle, (HI_MSG_HDR *) pSmgmtInfo->operation.ul_triggered_params.pMsg);
					pSmgmtInfo->ReqNum = 1;
					break;
				case 1:
					UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
					status = WFM_STATUS_SUCCESS;
					break;
				} /* switch(pSmgmtInfo->ReqNum) */
			} else if (pSmgmtInfo->operation.ul_triggered_params.Category == WFM_CONFIG_REQ_ID) {
				uint16 ConfigStatus;
				switch (pSmgmtInfo->ReqNum) {
				case 0:
					pSmgmtInfo->ReqNum = 1;
					ConfigStatus = UMAC_LL_REQ_DEV_CONF(UmacHandle, pSmgmtInfo->operation.ul_triggered_params.pConfigReq, pSmgmtInfo->operation.ul_triggered_params.pConfigHiMsg);
					if (ConfigStatus == WFM_STATUS_FAILURE) {
						status = ConfigStatus;
						NextIteration = 1;
					}

					break;
				case 1:
					UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
					status = WFM_STATUS_SUCCESS;
					break;
				} /* switch(pSmgmtInfo->ReqNum) */

			} /* if(Category == WFM_CONFIG_REQ_ID) */

		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_HOST_UL_TRIGGERED)*/
#if DOT11K_SUPPORT
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_MEASUREMENT_REQ) {
			switch (pSmgmtInfo->operation.measurement_req.measurementReqType) {
			case UMAC_MEASUREMENT_REQ_TYPE_CHANNEL_LOAD:
				switch (pSmgmtInfo->ReqNum) {
				case 0:
					NextIteration = UMAC_SendReqToWsm_ChannelLoadReq(UmacHandle);
					pSmgmtInfo->ReqNum = 1;
					break;
				case 1:
					UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
					status = WFM_STATUS_SUCCESS;
					break;
				}
				break;
			case UMAC_MEASUREMENT_REQ_TYPE_NOISE_HISTOGRAM:
				switch (pSmgmtInfo->ReqNum) {
				case 0:
					NextIteration = UMAC_SendReqToWsm_NoiseHistogramReq(UmacHandle);
					pSmgmtInfo->ReqNum = 1;
					break;
				case 1:
					UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
					status = WFM_STATUS_SUCCESS;
					break;
				}
				break;
			case UMAC_MEASUREMENT_REQ_TYPE_BEACON:
				switch (pSmgmtInfo->ReqNum) {
				case 0:
					NextIteration = UMAC_SendReqToWsm_BeaconReq(UmacHandle);
					pSmgmtInfo->ReqNum = 1;
					break;
				case 1:
					UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
					status = WFM_STATUS_SUCCESS;
					break;
				}
				break;
			case UMAC_MEASUREMENT_REQ_TYPE_STA_STATISTICS:
				switch (pSmgmtInfo->ReqNum) {
				case 0:
					NextIteration = UMAC_SendReqToWsm_StatisticsReq(UmacHandle);
					pSmgmtInfo->ReqNum = 1;
					break;
				case 1:
					UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
					status = WFM_STATUS_SUCCESS;
					break;
				}
				break;
			case UMAC_MEASUREMENT_REQ_TYPE_LINK_MEAS:
				switch (pSmgmtInfo->ReqNum) {
				case 0:
					NextIteration = UMAC_SendReqToWsm_LinkMeasReq(UmacHandle);
					pSmgmtInfo->ReqNum = 1;
					break;
				case 1:
					UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
					status = WFM_STATUS_SUCCESS;
					break;
				}
				break;
			case UMAC_MEASUREMENT_REQ_TYPE_TRANSMIT_STREAM:
				switch (pSmgmtInfo->ReqNum) {
				case 0:
					NextIteration = UMAC_Read_TSF_Timer(UmacHandle);
					pSmgmtInfo->ReqNum = 1;
					break;
				case 1:
					UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
					status = WFM_STATUS_SUCCESS;
					break;
				}
				break;
			} /* End of Switch Measurement Type */
		} /* End of Else IF pSmgmtInfo->Type */
#endif	/* DOT11K_SUPPORT */

#if FT_SUPPORT
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_FT_AUTH_JOIN) {
			WFM_BSS_INFO *pBssInfo = &pUmacInstance->FtTargetBss;
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				if (pUmacInstance->gVars.p.PsMode.PmMode == WFM_PS_MODE_DISABLED)
					NextIteration = WFM_UMAC_SendReqToWsm_SetPsMode(UmacHandle, psModeEnabled);
				else
					NextIteration = WFM_UMAC_WriteMib_OperationalPwrMode(UmacHandle, WFM_OPR_PWR_MODE_ACTIVE);
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				NextIteration = WFM_UMAC_SendReqToWsm_JoinReq(UmacHandle, pBssInfo);
				pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_JOIN_SUCCESS, &pUmacInstance->JoinConfirmResult);
			}
		} else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_FT_AUTH_JOINBACK) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				NextIteration = WFM_UMAC_SendReqToWsm_JoinReq(UmacHandle, NULL);
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				NextIteration = WFM_UMAC_SendReqToWsm_SetBssParams(UmacHandle, UMAC_SET_BSS_F_AID);
				pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				if (pUmacInstance->gVars.p.PsMode.PmMode != WFM_PS_MODE_ENABLED)
					NextIteration = WFM_UMAC_SendReqToWsm_SetPsMode(UmacHandle, &pUmacInstance->gVars.p.PsMode);
				else
					NextIteration = WFM_UMAC_WriteMib_OperationalPwrMode(UmacHandle, WFM_OPR_PWR_MODE_DOZE);
				pSmgmtInfo->ReqNum = 3;
				break;
			case 3:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_JOIN_SUCCESS, &pUmacInstance->JoinConfirmResult);
			}
		}
#endif	/* FT_SUPPORT */

#if P2P_MINIAP_SUPPORT
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_SEND_AUTH_RESP) {
			WSM_HI_MAP_LINK_REQ *pMapLinkReq = NULL;
			switch (pSmgmtInfo->ReqNum) {
			case 0:

				pMapLinkReq = (WSM_HI_MAP_LINK_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pMapLinkReq);
				NextIteration = UMAC_LL_REQ_MAP_LINK(UmacHandle, pMapLinkReq, pUmacInstance->linkId);
				pUmacInstance->dot11BssConf.numSTAsConnected++;
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			}

		} /*if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_SEND_AUTH_SUCCESS) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_RESET_SEQ_NUM_COUNTER) {
			WSM_HI_MAP_LINK_REQ *pMapLinkReq = NULL;
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				pMapLinkReq = (WSM_HI_MAP_LINK_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pMapLinkReq);
				NextIteration = UMAC_LL_REQ_MAP_LINK(
								UmacHandle,
								pMapLinkReq,
								pSmgmtInfo->operation.reset_seq_num_counter_params.linkId
								);
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				NextIteration = UMAC_LL_REQ_RESET_JOIN(
								UmacHandle,
								0x00,
								pSmgmtInfo->operation.reset_seq_num_counter_params.linkId
								);
				pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			}
		} /*if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_RESET_SEQ_NUM_COUNTER) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_START_AP) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				NextIteration = WFM_UMAC_WriteMib_TemplateFrame_ProbeReq(UmacHandle, WFM_BSS_LIST_SCAN_2_4G_BAND,	/* Support for 2.4G band only */ 0);
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				if (pUmacInstance->autoChanSel)
					NextIteration = UMAC_ScanReqACS(UmacHandle, &pUmacInstance->InternalScanMsg[0]);
					/* Scan Request */
				else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				NextIteration = WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UmacHandle, WSM_TMPLT_BCN);
				pSmgmtInfo->ReqNum = 3;
				break;
			case 3:
				NextIteration = WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UmacHandle, WSM_TMPLT_PRBRES);
				pSmgmtInfo->ReqNum = 4;
				break;
			case 4:
				if (pUmacInstance->dot11BssConf.vdrSpecificCapInfo & WFM_BSS_INFO_FLAGS_BSS_WMM_CAP) {
					LOG_EVENT(DBG_WFM_ALWAYS, "EDCA Paramaters in Qos Mode\n");
					NextIteration = wfm_umac_set_edca_parameters(UmacHandle, (WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED *) &pUmacInstance->gVars.WmmParamElement.QoSInfo);
				} else {
					wfm_umac_set_default_edca_parameters(UmacHandle, pUmacInstance->Channelnumber, pUmacInstance->AllSupportedRates_WithAP_Abgn, pUmacInstance->Band);
				}
				pSmgmtInfo->ReqNum = 5;
				break;
			case 5:
				if (pUmacInstance->gVars.p.hostMsgTypeFilter)
					NextIteration = WFM_UMAC_WriteMib_HostMsgTypeFilter(UmacHandle);
				else
					NextIteration = 1;
				pSmgmtInfo->ReqNum = 6;
				break;
			case 6:
				NextIteration = WFM_UMAC_WriteMib_OverrideInternalTxRate(UmacHandle);
				pSmgmtInfo->ReqNum = 7;
				break;
			case 7:
				if (pUmacInstance->gVars.p.useP2P == UMAC_P2P_ENABLED)
					NextIteration = WFM_UMAC_WriteMib_TxRetryRatePolicy(UmacHandle, 0);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 8;
				break;
			case 8:
				NextIteration = WFM_UMAC_WriteMib_TemplateFrame_ArpReply(UmacHandle);
				pSmgmtInfo->ReqNum = 9;
				break;
			case 9:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			}

		} /* if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_START_AP) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_UNLINK_STA) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:	/*fallthrough*/
			case 1:	/*fallthrough*/
			case 2:	/*fallthrough*/
			case 3:	/*fallthrough*/
			case 4:	/*fallthrough*/
			case 5:	/*fallthrough*/
			case 6:	/*fallthrough*/
			case 7:	/*fallthrough*/
			case 8:
				NextIteration = UMAC_LL_REQ_RESET_JOIN(UmacHandle, 0x00, pSmgmtInfo->ReqNum);
				pSmgmtInfo->ReqNum = 9;
				break;
			case 9:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			}
		} /* else if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_UNLINK_STA) */

		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_UPDATE_IE) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				NextIteration = UMAC_LL_REQ_UPDATE_IE(UmacHandle, (void *)TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pUpdateIeReq));
				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			}
		} /* else if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_UPDATE_IE) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_UPDATE_ERP_IE) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				if (pUmacInstance->ErpIeToBeUpdated) {
					pUmacInstance->ErpInfoPrevious = pUmacInstance->ErpInformation;
					NextIteration = UMAC_LL_REQ_UPDATE_IE(UmacHandle, (void *)TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pUpdateErpIeReq));
				} else {
					NextIteration = 1;
				}

				pSmgmtInfo->ReqNum = 1;
				break;
			case 1:
				/* use cts/rts protection */
				if (pUmacInstance->ErpIeToBeUpdated)
					NextIteration = WFM_UMAC_WriteMib_NonErpProtection(UmacHandle, pUmacInstance->ErpUseCtsToSelf);
				else
					NextIteration = 1;

				pSmgmtInfo->ReqNum = 2;
				break;
			case 2:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			}
		} /* else if(pSmgmtInfo->Type == SMGMT_REQ_TYPE_UPDATE_ERP_IE) */
		else if (pSmgmtInfo->Type == SMGMT_REQ_TYPE_SEND_ASSOC_RESP) {
			switch (pSmgmtInfo->ReqNum) {
			case 0:
				NextIteration = WFM_UMAC_WriteMib_TxRetryRatePolicy(UmacHandle, pUmacInstance->linkId);
				/*ReqNum 1 is reset request for the failed association request*/
				pSmgmtInfo->ReqNum = 2;
				break;
			case 1:
				/* Association is failed so reset the link */
				NextIteration = UMAC_LL_REQ_RESET_JOIN(UmacHandle, 0x00, pUmacInstance->linkId);
				pSmgmtInfo->ReqNum = 2;
				break;

			case 2:
				UMAC_RESTORE_PREVIOUS_STATE(UmacHandle);
				status = WFM_STATUS_SUCCESS;
				break;
			}
		} /* (pSmgmtInfo->Type == SMGMT_REQ_TYPE_SEND_ASSOC_RESP) */
#endif	/* P2P_MINIAP_SUPPORT */
	} while (NextIteration);

	return status;
} /* end UMAC_ManageAsynMgmtOperations() */

/******************************************************************************
 * NAME:	umac_interpret_wsm_status
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts WSM status codes to UMAC status codes.
 * \param wsm_status_code  - The WSM status code.
 * \returns uint32	   Corresponding wsm_status_code.
 *****************************************************************************/
uint32 umac_interpret_wsm_status(uint32 wsm_status_code)
{
	uint32 umac_status;

	switch (wsm_status_code) {

	case WSM_STATUS_SUCCESS:

		umac_status = WFM_STATUS_SUCCESS;
		break;
	case WSM_STATUS_FAILURE:
		umac_status = WFM_STATUS_FAILURE;
		break;
	case WSM_INVALID_PARAMETER:
		umac_status = WFM_STATUS_BAD_PARAM;
		break;
	case WSM_ACCESS_DENIED:
		umac_status = WFM_STATUS_REQ_REJECTED;
		break;
	case WSM_STATUS_DECRYPTFAILURE:
		umac_status = WFM_STATUS_FAILURE;
		break;
	case WSM_STATUS_MICFAILURE:
		umac_status = WFM_STATUS_FAILURE;
		break;
	case WSM_STATUS_RETRY_EXCEEDED:
		umac_status = WFM_STATUS_RETRY_EXCEEDED;
		break;
	case WSM_STATUS_TX_LIFETIME_EXCEEDED:
		umac_status = WFM_STATUS_TX_LIFETIME_EXCEEDED;
		break;
	case WSM_STATUS_LINK_LOST:
		umac_status = WFM_STATUS_LINK_LOST;
		break;
	case WSM_STATUS_NO_KEY_FOUND:
		umac_status = WFM_STATUS_REQ_REJECTED;
		break;
	case WSM_STATUS_JAMMER_DETECTED:
		umac_status = WFM_STATUS_FAILURE;
		break;
	default:
		umac_status = WFM_STATUS_FAILURE;
	} /* switch(wsm_status_code) */

	return umac_status;
} /* end umac_interpret_wsm_status() */

/******************************************************************************
 * NAME:	umac_interpret_int_status
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts UMAC internal status codes to UMAC's exported
 * status codes,
 * \param int_status_code  - The internal status code.
 * \returns uint32	   Corresponding UMAC status code.
 *****************************************************************************/
uint32 umac_interpret_int_status(uint32 int_status_code)
{
	uint32 umac_status_code;

	switch (int_status_code) {

	case UMAC_INT_STATUS_SUCCESS:

		umac_status_code = WFM_STATUS_SUCCESS;
		break;
	case UMAC_INT_STATUS_DUPLICATE_REQUEST:

		umac_status_code = WFM_STATUS_SUCCESS;
		break;
	case UMAC_INT_STATUS_BAD_PARAM:

		umac_status_code = WFM_STATUS_BAD_PARAM;
		break;
	case UMAC_INT_STATUS_VALUE_STORED:

		umac_status_code = WFM_STATUS_SUCCESS;
		break;
	default:
		umac_status_code = WFM_STATUS_BAD_PARAM;

	} /* switch( int_status_code  ) */

	return umac_status_code;
} /* end umac_interpret_int_status() */

/******************************************************************************
 * NAME:	umac_convert_to_internal_status
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts UMAC's exported status code UMAC internal
 * status code.
 * \param status_code  - The UMAC status code.
 * \returns uint32	   Corresponding UMAC Internal status code.
 *****************************************************************************/
uint32 umac_convert_to_internal_status(uint32 status_code)
{
	uint32 int_status_code;

	switch (status_code) {

	case WFM_STATUS_SUCCESS:

		int_status_code = UMAC_INT_STATUS_SUCCESS;
		break;
	case WFM_STATUS_BAD_PARAM:

		int_status_code = UMAC_INT_STATUS_BAD_PARAM;
		break;
	default:
		int_status_code = UMAC_INT_STATUS_BAD_PARAM;

	} /* switch( status_code  ) */

	return int_status_code;
} /* end umac_convert_to_internal_status() */

/******************************************************************************
 * NAME:	UMAC_IBSS_GetRandomBSSID
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function generates the random BSSID while creating the IBSS.
 * \param *addr - Address
 * \returns none.
 *****************************************************************************/
void UMAC_IBSS_GetRandomBSSID(uint8 *addr)
{
	uint8 i;
	uint32 random;
	addr[0] = 0x00;
	addr[1] = 0x80;
	addr[2] = 0xE1;

	random = OS_GetTime();

	for (i = 0; i < 3; i++)
		addr[3 + i] = (uint8) (random >> (8 * i));
}

#if DOT11K_SUPPORT
/******************************************************************************
 * NAME:	UMAC_ProcessRRMCapabilities
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will update the RRMcapablities as per the capabilities set by
 * upper layer.
 * \param UmacHandle		- Handle to the UMAC Instance
 * \param BeaconRRMCapabilities - RRM Capabilities in Beacon
 * \returns none.
 *****************************************************************************/
void UMAC_ProcessRRMCapabilities(UMAC_HANDLE UmacHandle,
				 uint64 BeaconRRMCapabilities)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 measurementCapability = 0;
	uint16 MC = 0;
	uint16 bitMask = 1;
	uint16 maskIndex = 0;

	/*
	  Set the pUmacInstance->dot11k_Meas_Params.RRMCapability to
	  default value
	*/
	pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_SUPPORTED_DEFAULT_CAPABILITES;
	measurementCapability = pUmacInstance->gVars.p.measurementCapability;
	while (measurementCapability) {
		MC = measurementCapability;
		MC &= (bitMask << maskIndex);
		switch (MC) {
		case 2:
			pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_RRMCAPABILITY_LINK_MEASUREMENT;
		break;

		case 4:
			pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_RRMCAPABILITY_NEIGHBOR_REPORT;
		break;

		case 8:
			pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_RRMCAPABILITY_CHANNEL_LOAD;
		break;

		case 16:
			pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_RRMCAPABILITY_NOISE_HISTOGRAM;
		break;

		case 32:
			pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_SUPPORTED_BEACON_CAPABILITIES;
		break;

		case 64:
			pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_RRMCAPABILITY_FRAME_MEASUREMENT;
		break;

		case 128:
			pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_RRMCAPABILITY_STATISTICS;
		break;

		case 256:
			pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_SUPPORTED_LCI_CAPABILITIES;
		break;

		case 512:
			pUmacInstance->dot11k_Meas_Params.RRMCapability |= UMAC_RRMCAPABILITY_TRANSMIT_STREAM;
		break;

		default:

		break;

		} /*end of switch */
		/*clear the measurementCapability bit and update maskIndex */
		measurementCapability &= ~(bitMask << maskIndex);
		maskIndex++;
	}
}

/******************************************************************************
 * NAME:	UMAC_ProcessRRMCapabilities
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will parse all the measurement request elements and stores
 * them in pUmacInstance
 * \param UmacHandle	 - Handle to the UMAC Instance
 * \param pFrame	 - Pointer to start of measurement request elements
 * \param reqElementsLen - Measurement request elements length
 * \returns none.
 *****************************************************************************/
void UMAC_ProcMeasurementRequestElements(UMAC_HANDLE UmacHandle,
					 uint8 *pFrame,
					 uint16 reqElementsLen)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	IeElement IEelement;
	uint8 iIndex = 0;
	uint8 ReqSupported = 0;
	uint16 MeasReqElemLen = 0;
	uint8 measurementType, measurementReqMode;

	while (reqElementsLen) {
		if ((WFM_UMAC_ProcIeElements(pFrame,
					reqElementsLen,
					UMAC_IE_ELEMENT_MEASUREMENT_REQ,
					&IEelement) == TRUE) && (IEelement.size)) {
			ReqSupported = 0;
			measurementReqMode = *(IEelement.info + 1);
			measurementType = *(IEelement.info + 2);
			/*It indicates that Enable bit is 1 and req and rep bit is 0 then don't add the request
			   as we will not send any report for this request */
			/*TBC - Whether to Include the condition of En=1,Req=1,Report = 0 - Pg 40 11k Standard */
			if ((measurementReqMode & 0x0E) != 2) {
				if (measurementType != UMAC_MEASUREMENT_REQ_TYPE_MEASUREMENT_PAUSE) {
					pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[iIndex].measurementToken = *(IEelement.info);
					pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[iIndex].measurementRequestMode = measurementReqMode;
					pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[iIndex].measurementType = measurementType;
					OS_MemoryCopy(
						(uint8 *) &pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[iIndex].measurementElementReq[0],
						((uint8 *) IEelement.info + 3),
						(IEelement.size - 3));
					ReqSupported = 1;
				} else {
					MeasReqElemLen = ((IEelement.size - 3) <= UMAC_DEFAULT_MEAS_PAUSE_LEN) ? (IEelement.size - 3) : UMAC_DEFAULT_MEAS_PAUSE_LEN;
					OS_MemoryCopy((uint8 *) &pUmacInstance->dot11k_Meas_Params.MeasPauseTime, ((uint8 *) IEelement.info + 3), MeasReqElemLen);
					pUmacInstance->dot11k_Meas_Params.MeasPauseIndex = iIndex;
					pUmacInstance->dot11k_Meas_Params.MeasPauseReq = TRUE;
					ReqSupported = 0;
				}

				if (ReqSupported) {
					UMAC_fnUpdateTotalReq(UmacHandle, iIndex);
					iIndex++;
				}
			}
			pFrame += (IEelement.size + 2);
			reqElementsLen -= (IEelement.size + 2);
		}		/*End of IE */
	}
	/*Update the total Array Index elements and */
	pUmacInstance->dot11k_Meas_Params.totalArrayIndex = iIndex;
	if (pUmacInstance->dot11k_Meas_Params.MeasPauseReq) {
		/*Check if the measurement pause was the last measurement request */
		if (pUmacInstance->dot11k_Meas_Params.MeasPauseIndex == (pUmacInstance->dot11k_Meas_Params.totalArrayIndex))
			pUmacInstance->dot11k_Meas_Params.MeasPauseIndex = 0;
		/*Case when Measuremet Pause is the first request */
		if (!pUmacInstance->dot11k_Meas_Params.MeasPauseIndex)
			pUmacInstance->dot11k_Meas_Params.MeasPauseIndex = 1;
	}
	/*Reset the current Array Index to 0 as new measurement request comes in */
	pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex = 0;
}

/******************************************************************************
 * NAME:	UMAC_fnUpdateTotalReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will update the total req Index and check if regulatory class
 * is configured.
 * \param UmacHandle - Handle to the UMAC Instance
 * \param iIndex - Index of Request Element
 * \returns none.
 *****************************************************************************/
void UMAC_fnUpdateTotalReq(UMAC_HANDLE UmacHandle, uint8 iIndex)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	uint8 measurementReqMode = 0, measurementType = 0;

	measurementType = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[iIndex].measurementType ;
	measurementReqMode = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[iIndex].measurementRequestMode;
	if (!(pUmacInstance->gVars.p.measurementCapability & (1 << measurementType))) {
		pUmacInstance->dot11k_Meas_Params.totalRequests += 1;
	} else if (!(pUmacInstance->gVars.p.enableMeasurements & (1 << measurementType))) {
		pUmacInstance->dot11k_Meas_Params.totalRequests += 1;
	} else if (measurementReqMode & UMAC_ENABLE_MASK) {
		pUmacInstance->dot11k_Meas_Params.totalRequests += 1;
	} else {
		if ((measurementType == UMAC_MEASUREMENT_REQ_TYPE_LCI) && (!pUmacInstance->gVars.p.lciInfoAvl)) {
			pUmacInstance->dot11k_Meas_Params.totalRequests += 1;
		} else if ((measurementType == UMAC_MEASUREMENT_REPORT_TYPE_CHANNEL_LOAD) ||
			   (measurementType == UMAC_MEASUREMENT_REPORT_TYPE_NOISE_HISTOGRAM) ||
			   ((measurementType == UMAC_MEASUREMENT_REPORT_TYPE_BEACON) &&
			    (pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq[6] != UMAC_MEAS_MODE_BEACON_TABLE))
		    ) {

			uint16 channelOffset;
			uint32 txPowerLevel;
			uint8 nResult = FALSE, RegClass = 0, channelNum = 0;

			RegClass = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[iIndex].measurementElementReq[0];
			channelNum = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[iIndex].measurementElementReq[1];
			nResult = UMAC_CheckRegClassAndChannelNum(UmacHandle, RegClass, channelNum, &channelOffset, &txPowerLevel);
			if (!nResult)
				pUmacInstance->dot11k_Meas_Params.totalRequests += 1;
			else
				pUmacInstance->dot11k_Meas_Params.totalRequests += (pUmacInstance->dot11k_Meas_Params.noOfRep + 1);

		} else {
			pUmacInstance->dot11k_Meas_Params.totalRequests += (pUmacInstance->dot11k_Meas_Params.noOfRep + 1);
		}
	}
}

#endif /* DOT11K_SUPPORT */

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:	UMAC_Buffer_Frame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function buffers the frame for particular LinkID.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param linkId - linkID
 * \param pTxDesc - Pointer to pTxDesc
 * \param headerOffset - headerOffset
 * \returns TRUE if frame is buffered else FALSE.
 *****************************************************************************/
uint8 UMAC_Buffer_Frame(UMAC_HANDLE UmacHandle,
			uint8 linkId,
			UMAC_TX_DESC *pTxDesc,
			uint8 headerOffset,
			void *pDriverInfo)
{
	uint8 result = TRUE;
	uint8 uapsdFlag, buffType, index = 0;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	uint32 put, get;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uapsdFlag = pUmacInstance->sta[linkId].qosInfo & UMAC_QOS_INFO_STA_UAPSD_MASK;
	buffType = AP_BUFF_TYPES_LEGACY;
	switch (pTxDesc->QueueId) {
	case WFM_ACI_BE:
		if (uapsdFlag & UMAC_QOS_INFO_STA_UAPSD_AC_BE)
			buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
		break;
	case WFM_ACI_BK:
		if (uapsdFlag & UMAC_QOS_INFO_STA_UAPSD_AC_BK)
			buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
		break;
	case WFM_ACI_VI:
		if (uapsdFlag & UMAC_QOS_INFO_STA_UAPSD_AC_VI)
			buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
		break;
	case WFM_ACI_VO:
		if (uapsdFlag & UMAC_QOS_INFO_STA_UAPSD_AC_VO)
			buffType = AP_BUFF_TYPES_DLVRY_ENABLED;
		break;
	}		/*switch(pTxDescriptor->QueueId) */

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)MOVE_TO_START_OF_DEV_IF_TX_REQ(pTxDesc);
	if (linkId && ((buffType == AP_BUFF_TYPES_LEGACY) || (uapsdFlag == UMAC_QOS_INFO_STA_UAPSD_MASK))
	    ) {
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
		pTxDesc->MsgId = linkId << 6;
	}		/*if(linkId && ((buffType == AP_BUFF_TYPES_LEGACY)...) */

	OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
	put = pUmacInstance->sta[linkId].put[buffType];
	get = pUmacInstance->sta[linkId].get[buffType];
	if ((put - pUmacInstance->sta[linkId].get[buffType]) < AP_NUM_BUFFERS) {
		index = (uint8) (put & (AP_NUM_BUFFERS - 1));
		pUmacInstance->sta[linkId].buffer[buffType][index].buffPtr = (uint8 *) pDevIfTxReq;
		pUmacInstance->sta[linkId].buffer[buffType][index].timeStamp = (OS_GetTime() + (pUmacInstance->sta[linkId].listenInterval + 4) * pUmacInstance->dot11BssConf.beaconInterval);
		put++;
		pUmacInstance->sta[linkId].put[buffType] = put;
		pUmacInstance->sta[linkId].numPendingFrames[buffType] = (uint8) (put - pUmacInstance->sta[linkId].get[buffType]);
		UMAC_Store_Internal_TxDesc(
			UmacHandle,
			pTxDesc,
			pUmacInstance->gVars.ApUmacTxDesc,
			UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
			headerOffset,
			pDriverInfo);

		/* Remove from the internal Tx bufferes */
		/*UMAC_Release_Internal_TxDesc(UmacHandle, pTxDesc, pUmacInstance->gVars.umacTxDescInt, UMAC_NUM_INTERNAL_TX_BUFFERS);*/
	} /*if((put - pUmacInstance->sta[linkId].get) < AP_NUM_BUFFERS ) */
	else
		result = FALSE;
	OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
	return result;
}

/******************************************************************************
 * NAME:	UMAC_GetACFromPriority
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function gives the AC from the priority
 * \param priority - priority
 * \returns AC
 *****************************************************************************/
uint8 UMAC_GetACFromPriority(uint8 priority)
{
	uint8 AccessCategory = 0;
	switch (priority) {
	case WFM_802_1D_UP_BK:
	case WFM_802_1D_UP_RESERVED:
		AccessCategory = WFM_ACI_BK;
		break;
	case WFM_802_1D_UP_BE:
	case WFM_802_1D_UP_EE:
		AccessCategory = WFM_ACI_BE;
		break;
	case WFM_802_1D_UP_CL:
	case WFM_802_1D_UP_VI:
		AccessCategory = WFM_ACI_VI;
		break;
	case WFM_802_1D_UP_VO:
	case WFM_802_1D_UP_NC:
		AccessCategory = WFM_ACI_VO;
		break;
	default:
		AccessCategory = WFM_ACI_BE;
	} /*switch(Priority) */
	return AccessCategory;
}

/******************************************************************************
 * NAME:	UMAC_CheckAllSTA_WMM_Capable
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function checks if all STA's are WMM capable
 * \param UmacHandle - Handle to UMAC Instance.
 * \returns TRUE if all STS'a are WMM capable else FALSE.
 *****************************************************************************/
uint8 UMAC_CheckAllSTA_WMM_Capable(UMAC_HANDLE UmacHandle)
{
	uint8 nResult = TRUE;
	uint8 i = 1;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	for (i = 1 ; i < pUmacInstance->gVars.p.maxNumClientSupport; i++) {
		if ((!(pUmacInstance->sta[i].isWMMEnabled)) && (pUmacInstance->sta[i].state != STA_STATE_NONE)) {
			nResult = FALSE;
			break;
		}
	}
	return nResult;
}

/******************************************************************************
 * NAME:	UMAC_Initialize_Internal_ApTxDescList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the internal TxDescriptor list kept by umac when
 * it is in AP mode.
 * \param UmacHandle - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void UMAC_Initialize_Internal_ApTxDescList(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	int Count;

	OS_ASSERT(pUmacInstance);

	/*Initializing the TxDesc Store */
	for (Count = 0; Count < UMAC_NUM_AP_INTERNAL_TX_BUFFERS; Count++) {
		pUmacInstance->gVars.ApUmacTxDesc[Count].pTxDesc = NULL;
		pUmacInstance->gVars.ApUmacTxDesc[Count].PacketId = 0;

	} /* for(Count=0 ;Count<UMAC_NUM_INTERNAL_TX_BUFFERS ; Count++)*/
} /* end UMAC_Initialize_Internal_TxDescList() */

/******************************************************************************
 * NAME:	UMAC_DeInit_Internal_ApTxDescList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function De-initializes the internal TxDescriptor list kept by umac.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param linkId     - Link Id of the STA
 * \returns none.
 *****************************************************************************/
void UMAC_DeInit_Internal_ApTxDescList(UMAC_HANDLE UmacHandle, uint8 linkId)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	int Count;
	uint32 memstatus;

	OS_ASSERT(pUmacInstance);

	/*Initializing the TxDesc Store */
	for (Count = 0; Count < UMAC_NUM_AP_INTERNAL_TX_BUFFERS; Count++) {
		if (pUmacInstance->gVars.ApUmacTxDesc[Count].pTxDesc != NULL) {
			memstatus = UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pUmacInstance->gVars.ApUmacTxDesc[Count].pTxDesc);

			OS_ASSERT(memstatus);

			UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.ApUmacTxDesc[Count].pTxDesc);

			pUmacInstance->gVars.ApUmacTxDesc[Count].pTxDesc = NULL;
			pUmacInstance->gVars.ApUmacTxDesc[Count].PacketId = 0;

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

		}
	} /* for(Count=0 ;Count<UMAC_NUM_INTERNAL_TX_BUFFERS ; Count++)*/
} /* end UMAC_DeInit_Internal_TxDescList() */

/******************************************************************************
 * NAME:UMAC_DB_Insert_STA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function insert the new STA MAC address in STA Database.
 * \param UmacHandle	- Handle to UMAC instance.
 * \param *staAddr	- MAC Address of the STA
 * \returns Location index where the STA is inserted.
 *****************************************************************************/
uint8 UMAC_DB_Insert_STA(UMAC_HANDLE UmacHandle, uint8 *staAddr)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 i;

	if (IS_NLL_MAC_ADDRESS(staAddr)) {
		i = (uint8) pUmacInstance->gVars.p.maxNumClientSupport;
		goto exit_handler;
	} else {
		for (i = 1; i < pUmacInstance->gVars.p.maxNumClientSupport; i++) {
			if (STA_STATE_NONE == pUmacInstance->sta[i].state) {
				pUmacInstance->sta[i].state = STA_STATE_AUTHENTICATING;
				OS_MemoryCopy(
						pUmacInstance->sta[i].macAddr,
						staAddr,
						WFM_MAC_ADDRESS_SIZE
						);

				pUmacInstance->sta[i].linkId = i;
				break;
			}
		}
	}

 exit_handler:
	return i;
} /* end UMAC_DB_Insert_STA */

/******************************************************************************
 * NAME:UMAC_DB_Search_STA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function search the link ID for the provided STA MAC address.
 * \param UmacHandle	- Handle to UMAC instance.
 * \param *staAddr	- MAC Address of the STA
 * \returns Location index where the STA is inserted.
 *****************************************************************************/
uint8 UMAC_DB_Search_STA(UMAC_HANDLE UmacHandle, uint8 *staAddr)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 i;
	uint32 Status;

	if (IS_NLL_MAC_ADDRESS(staAddr)) {
		i = (uint8) pUmacInstance->gVars.p.maxNumClientSupport;
		goto exit_handler;
	} else {
		for (i = 1; i < pUmacInstance->gVars.p.maxNumClientSupport; i++) {
			if (STA_STATE_NONE != pUmacInstance->sta[i].state) {
				Status = OS_MemoryEqual(
						pUmacInstance->sta[i].macAddr,
						WFM_MAC_ADDRESS_SIZE,
						staAddr,
						WFM_MAC_ADDRESS_SIZE
						);

				if (Status == TRUE)
					break;
			}
		}
	}

 exit_handler:
	return i;
} /* end UMAC_DB_Search_STA */

/******************************************************************************
 * NAME:UMAC_Configure_Join_Req
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function configures the Join request to be sent prior to GO
 * negotiation request.
 * \param  UmacHandle      - Handle to UMAC instance.
 * \param  pJoinParams     - Pointer to the Join Param structure
 * \returns none
 *****************************************************************************/
void UMAC_Configure_Join_Req(UMAC_HANDLE UmacHandle,
			     JOIN_PARAMETERS **pJoinParams)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	OS_ASSERT(!pUmacInstance->gVars.pJoinTxDesc);

	pUmacInstance->gVars.pJoinTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pJoinTxDesc);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pUmacInstance->gVars.pJoinTxDesc, UMAC_JOIN_REQ);

	WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

	*pJoinParams = (JOIN_PARAMETERS *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pJoinTxDesc);
	CHECK_POINTER_IS_VALID(*pJoinParams);

	(*pJoinParams)->Mode = WFM_802_INFRASTRUCTURE;
	(*pJoinParams)->Band = PHY_BAND_2G;
	(*pJoinParams)->ChannelNumber = pUmacInstance->Channelnumber;

	OS_MemoryCopy(
		(*pJoinParams)->BSSID,
		pUmacInstance->MacAddress,
		WFM_MAC_ADDRESS_SIZE
		);

	(*pJoinParams)->AtimWindow = 0;
	(*pJoinParams)->PreambleType = 1; /* Short preamble, Long for 1Mbps */
	(*pJoinParams)->ProbeForJoin = 1;
	(*pJoinParams)->DTIMPeriod = 1;
	(*pJoinParams)->SSIDLength = pUmacInstance->p2pGrpFormtn.SSIDLength;

	OS_MemoryCopy(
		(*pJoinParams)->SSID,
		pUmacInstance->p2pGrpFormtn.ssid,
		pUmacInstance->p2pGrpFormtn.SSIDLength
		);

	(*pJoinParams)->BeaconInterval = WFM_DEFAULT_BEACON_INTERVAL;
	(*pJoinParams)->BasicRateSet = WFM_ERP_OFDM_RATES_SUPPORTED;
} /* end UMAC_Configure_Join_Req */

/******************************************************************************
 * NAME:    UMAC_FreePendingMgmtRequests
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function free the memory allocated for pending mgmt frames when the
 * STA is being unlinked.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param linkId       - Link Id of the assoicated STA
 * \returns none
 *****************************************************************************/
void UMAC_FreePendingMgmtRequests(UMAC_HANDLE UmacHandle, uint8 linkId)
{
	uint8 count;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 storedLinkId = (uint8)pUmacInstance->gVars.p.maxNumClientSupport;
	UMAC_TX_DESC *pStoredTxDesc = NULL;
	for (count = 0; count < UMAC_NUM_AP_INTERNAL_TX_BUFFERS; count++) {
		if (pUmacInstance->gVars.ApUmacTxDesc[count].pTxDesc) {
			UMAC_RETRIEVE_LINK_ID(
			pUmacInstance->gVars.ApUmacTxDesc[count].pTxDesc->MsgId,
			storedLinkId
			);
		}

		if (storedLinkId == linkId) {
			pStoredTxDesc = pUmacInstance->gVars.ApUmacTxDesc[count].pTxDesc;
			pUmacInstance->gVars.ApUmacTxDesc[count].pTxDesc = NULL;
			pUmacInstance->gVars.ApUmacTxDesc[count].PacketId = CFG_HI_NUM_REQS;

			UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
				UmacHandle,
				MOVE_TO_START_OF_DEV_IF_TX_REQ(pStoredTxDesc)
				);

			UMAC_MEM_REMOVE_STAMP(
				MOVE_TO_START_OF_DEV_IF_TX_REQ(pStoredTxDesc)
				);

			OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
			pUmacInstance->NumTxBufPending[linkId]--;
			OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

			storedLinkId = (uint8)pUmacInstance->gVars.p.maxNumClientSupport;
		}
	} /* for(Count=0 ;Count<UMAC_NUM_INTERNAL_TX_BUFFERS ; Count++)*/
}

/******************************************************************************
 * NAME:UMAC_Any_Client_Dozing
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function checks whether any of the associated station is dozing.
 * \param UmacHandle - Handle to UMAC instance.
 * \returns TRUE if any STA is dozing, otherwise FALSE.
 *****************************************************************************/
uint8 UMAC_Any_Client_Dozing(UMAC_HANDLE UmacHandle)
{
	uint8 result = FALSE;
	uint8 i;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	for (i = 1; i < pUmacInstance->gVars.p.maxNumClientSupport; i++) {
		if ((pUmacInstance->sta[i].PsState == STA_PS_STATE_DOZE) && (pUmacInstance->sta[i].state == STA_STATE_ASSOCIATED)
		    ) {
			result = TRUE;
			break;
		}
	}
	return result;
} /* end UMAC_Any_Client_Dozing() */

/******************************************************************************
 * NAME:UMAC_FlushSTA_Buffers
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function flushes the buffer of an associated STA who is going to be
 * disassociated.
 * \param UmacHandle - Handle to UMAC instance.
 * \param linkId     - Link Id of the associated STA.
 * \returns none
 *****************************************************************************/
void UMAC_FlushSTA_Buffers(UMAC_HANDLE UmacHandle, uint8 linkId)
{
	uint32 get;
	UMAC_TX_DESC *pStoredTxDesc;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 index;
	uint8 buffType;
	void *pDriverInfo = NULL;

	for (buffType = 0; buffType < AP_BUFF_TYPES; buffType++) {
		get = pUmacInstance->sta[linkId].get[buffType];
		while (pUmacInstance->sta[linkId].numPendingFrames[buffType]) {
			UMAC_DEVIF_TX_DATA *pDevIfTxRequest;
			UMAC_TX_DESC *pTxDescriptor = NULL;
			index = (uint8) (get & (AP_NUM_BUFFERS - 1));
			OS_LOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);
			pDevIfTxRequest = (UMAC_DEVIF_TX_DATA *) pUmacInstance->sta[linkId].buffer[buffType][index].buffPtr;
			pTxDescriptor = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxRequest + sizeof(UMAC_DEVIF_TX_DATA));
			get++;
			pUmacInstance->sta[linkId].get[buffType] = get;
			OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]);

			if (pTxDescriptor->PacketId & UMAC_FWDED_FRAME) {
				pStoredTxDesc = (UMAC_TX_DESC *) ((uint8 *) pDevIfTxRequest + (sizeof(UMAC_TX_DESC)
											       - sizeof(UMAC_RX_DESC))
				    );

				UMAC_LL_RELEASE_FWD_RX_BUFF(
							UmacHandle,
							pStoredTxDesc,
							pDriverInfo);

				UMAC_Release_Internal_TxDesc(
					UmacHandle,
					pTxDescriptor,
					pUmacInstance->gVars.ApUmacTxDesc,
					UMAC_NUM_AP_INTERNAL_TX_BUFFERS,
					&pDriverInfo
					);
				UMAC_RELEASE_PACKET_ID_AP(UmacHandle, pTxDescriptor->PacketId);
			}
			/* Release the internally allocated memory */
			UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pDevIfTxRequest);
			pUmacInstance->sta[linkId].numPendingFrames[buffType] = (uint8) (pUmacInstance->sta[linkId].put[buffType] - get);
		} /* while(pUmacInstance->sta[linkId].numPendingFrames) */
	} /* for(buffType=0; buffType<AP_BUFF_TYPES; buffType++)*/
	UMAC_ProcessPendingTxRequests(pUmacInstance, linkId);
	if (pUmacInstance->NumTxBufPending[linkId])
		UMAC_FreePendingMgmtRequests(pUmacInstance, linkId);

} /* end UMAC_FlushSTA_Buffers() */
#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:	UMAC_GetLinkId
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns the linkid corresponding to a given MAC address.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param RemoteMac    - MAC address of the remote STA/AP.
 * \returns uint8.
 *****************************************************************************/
uint8	UMAC_GetLinkId(UMAC_HANDLE UmacHandle, uint8 *RemoteMac)
{
	uint8 linkId = 0;
#if P2P_MINIAP_SUPPORT
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *)UmacHandle;

	if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		&& !IS_MULTICAST_MAC_ADDRESS(RemoteMac)) {
		for (linkId = 1; linkId < pUmacInstance->gVars.p.maxNumClientSupport; linkId++) {
			if (D0_ADDR_EQ(&pUmacInstance->sta[linkId], RemoteMac))
				break;
		}
	}
#endif
	return linkId;
}

/******************************************************************************
 * NAME:	UMAC_ProtectionRequired
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function determines if a given frame requires encryption.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param linkId       - Link Id corresponding to the give STA/AP.
 * \param Type         - Type of the frame.
 * \param SubType      - SubType of the frame.
 * \returns uint8.
 *****************************************************************************/
uint8	UMAC_ProtectionRequired(UMAC_HANDLE UmacHandle, uint8 linkId, uint8 Type, uint8 SubType)
{
	uint8 ProtRequired = FALSE;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *)UmacHandle;
#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode <= OPERATING_MODE_P2P_CLIENT) {
#endif
		if (Type == WFM_UMAC_DATA) {
			if (pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[0])
				ProtRequired = TRUE;
		} else {
#if MGMT_FRAME_PROTECTION
			if (pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm
				&& ((SubType == WFM_UMAC_MGMT_ACTION)
				|| (SubType == WFM_UMAC_MGMT_DISASSOC)
				|| (SubType == WFM_UMAC_MGMT_DEAUTH)))
					ProtRequired = TRUE;
#endif
		}
#if P2P_MINIAP_SUPPORT
	} else {
		if (Type == WFM_UMAC_DATA) {
			if (linkId && pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId])
				ProtRequired = TRUE;
			else if ((linkId == 0) && pUmacInstance->gVars.p.InfoHost.GroupKeyInstalled)
				ProtRequired = TRUE;
		}
	}
#endif
	return ProtRequired;
}

/******************************************************************************
 * NAME:	UMAC_FindIndexInNeighborBssDB
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function searches the new connection sta address in internal table
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - Mac Address.
 * \returns index of neighboring BSS if already in DB or of free space if not
 * \ already in DB
 *****************************************************************************/
uint32 UMAC_FindIndexInNeighborBssDB(UMAC_HANDLE UmacHandle,  uint8 *pAddr)
{
	uint32 index = MAXIMUM_NUMBER_OF_NEIGHBORING_BSS;
	uint8 BssidNull[WFM_MAC_ADDRESS_SIZE] = {0, 0, 0, 0, 0, 0};
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 i = 0;

	for (i = 0; i < MAXIMUM_NUMBER_OF_NEIGHBORING_BSS; i++) {
		if (OS_MemoryEqual(&pUmacInstance->neighborBssDB[i].Bssid[0], WFM_MAC_ADDRESS_SIZE, pAddr, WFM_MAC_ADDRESS_SIZE))
			break;
	}

	if (i == MAXIMUM_NUMBER_OF_NEIGHBORING_BSS) {
		for (i = 0; i < MAXIMUM_NUMBER_OF_NEIGHBORING_BSS; i++) {
			if (OS_MemoryEqual(&pUmacInstance->neighborBssDB[i].Bssid[0], WFM_MAC_ADDRESS_SIZE, &BssidNull[0], WFM_MAC_ADDRESS_SIZE)) {
				OS_MemoryCopy(
						&pUmacInstance->neighborBssDB[i].Bssid[0],
						pAddr,
						WFM_MAC_ADDRESS_SIZE
						);
				break;
			}
		}
	}

	index = i;

	return index;
}
