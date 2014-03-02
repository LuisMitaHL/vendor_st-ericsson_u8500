/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup Upper_MAC_Core
 * \brief
 * This is main layer which will maintain the UMAC state machine. This will
 * accept the request received from UMI layer and convert it into the
 * corresponding MIBs/Requests of WSM firmware. This will handle all the
 * management frames and data frames. This will send the entire MIBs, requests,
 * management frames and data frames to the DIL layer.
 */
/**
 * \file umac_hi.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_hi.c
 * \brief
 * Implementation of UMAC HI module. This file implements the interface between
 * HI module and UMAC module.
 * \ingroup Upper_MAC_Core
 * \date 13/02/08 14:05
 */

/******************************************************************************
				INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "umac_hi.h"
#include "umac_messages.h"
#include "umac_sm.h"
#include "umac_dev_list.h"
#include "umac_frames.h"
#include "umac_utils.h"
#include "umac_mib.h"
#include "umac_internal.h"

#include "umac_ll_if.h"

/******************************************************************************
			LOCAL DECLARATIONS
******************************************************************************/

/******************************************************************************
		      EXTERNAL DATA REFERENCES
 *****************************************************************************/

extern UMAC_FSM_HANDLER UMAC_StateEventTable[UMAC_MAX_STATE][UMAC_MAX_EVT];

#ifdef UMAC_SM_DBG_ENABLE
extern char *UMAC_StateEventTable_StateName[UMAC_MAX_STATE];
extern char *UMAC_StateEventTable_EventName[UMAC_MAX_EVT];
#endif	/*UMAC_SM_DBG_ENABLE */

/******************************************************************************
			  EXTERNAL FUNCTIONS
******************************************************************************/

/******************************************************************************
			LOCAL FUNCTIONS
******************************************************************************/

/******************************************************************************
 * NAME:	WFM_HI_Proc_TransmitReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the transmit request.
 * \param UmacHandle - Handle to the UMAC Instance.
 * \param *pTxReq - Tx Request from upper layer.
 * \returns uint16 Appropriate WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_TransmitReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *pTxReq)
{
	uint32 LmacStatus;
	UMAC_TX_DATA *pTxDataReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_EVENT(DBG_WFM_HI, "WFM_HI_Proc_TransmitReq()\n");

	OS_ASSERT(UmacHandle);

	OS_ASSERT(pTxReq);
	pTxDataReq = (UMAC_TX_DATA *) pTxReq;
	if (pTxDataReq->pUlHdr->MessageLength) {
		if (((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
		     && (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED)) || (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED)
#if P2P_MINIAP_SUPPORT
		    || ((pUmacInstance->UmacPreviousState == UMAC_BSS_STARTED)
			&& (pUmacInstance->UmacCurrentState == UMAC_PROCESSING)) || (pUmacInstance->UmacCurrentState == UMAC_BSS_STARTED)
		    || ((pUmacInstance->UmacPreviousState == UMAC_GROUP_FORMED)
			&& (pUmacInstance->UmacCurrentState == UMAC_PROCESSING)) || (pUmacInstance->UmacCurrentState == UMAC_GROUP_FORMED)
#endif	/*P2P_MINIAP_SUPPORT */
		    ) {
			LmacStatus = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_DATA_TX, pTxDataReq);
		} else {
			LmacStatus = WFM_STATUS_REQ_REJECTED;
		}

#if P2P_MINIAP_SUPPORT
		if (WFM_STATUS_PENDING == LmacStatus) {
			/*bit 15 is used to indicate pending HI requests */
			pTxDataReq->MsgId = (pTxDataReq->MsgId | 0x1000);
			UMAC_QUEUE_PENDING_HI_MSG(UmacHandle, UMAC_DATA_TX, pTxDataReq);
		}	/*if( WFM_STATUS_PENDING == LmacStatus ) */
#endif	/*P2P_MINIAP_SUPPORT */

	}	/*if( pTxDataReq->pUlHdr->MessageLength ) */
	else
		LmacStatus = WFM_STATUS_BAD_PARAM;

	return (uint16) LmacStatus;
}				/*end WFM_HI_Proc_TransmitReq() */

/******************************************************************************
 * NAME:	WFM_HI_Proc_GetParamReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Getparameter request.
 * \param UmacHandle   - Handle to the UMAC Instance.
 * \param *hi_msg      - Message from host
 * \returns uint16       Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_GetParamReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg)
{
	WFM_GET_PARAM_REQ *get_param_req = NULL;
	WFM_GET_PARAM_CNF *get_param_cnf = NULL;
	uint16 umac_event = UMAC_MAX_EVT;
	uint32 DevCount = 0;
	WFM_DEV_ELEMENT *pDevElement = NULL;
	WFM_BSS_CACHE_INFO_IND *pBssInd = NULL;
	WFM_STATUS_CODE status = WFM_STATUS_SUCCESS;
	uint8 linkId = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_EVENT(DBG_WFM_HI, "WFM_HI_Proc_GetParamReq()\n");

	OS_ASSERT(pUmacInstance);

#if P2P_MINIAP_SUPPORT
	linkId = (uint8) ((hi_msg->MessageId & UMAC_HI_MSG_LINK_ID) >> 6);
#endif	/* P2P_MINIAP_SUPPORT */

	if (hi_msg->MessageLength) {

		get_param_req = &hi_msg->PayLoad.wfm_get_param_req;
		get_param_cnf = &hi_msg->PayLoad.wfm_get_param_cnf;

		LOG_DATA(DBG_WFM_HI | DBG_WFM_OID, "WFM_HI_Proc_GetParamReq(): Oid=0x%x\n", get_param_req->ParameterId);

		/* Need to set a default length */
		get_param_cnf->ParameterLength = 0;

		switch (get_param_req->ParameterId) {
		case WFM_DEVICE_OID_802_11_STATION_ID:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_STATION_ID\n");

				OS_MemoryCopy(&get_param_cnf->ParameterData.dot11StationId, pUmacInstance->MacAddress, WFM_MAC_ADDRESS_SIZE);

				get_param_cnf->ParameterLength = WFM_MAC_ADDRESS_SIZE;

				break;
			}
		case WFM_DEVICE_OID_802_11_COUNTRY_STRING:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_COUNTRY_STRING\n");
				if (pUmacInstance->gVars.p.Enable11dFeature) {
					if (pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PHY_BAND_2G] == TRUE || pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PHY_BAND_5G] == TRUE) {
						OS_MemoryCopy(get_param_cnf->ParameterData.CountryStringInfo.dot11CountryString, pUmacInstance->gVars.p.InfoHost.CountryStringInfo->countryString, WFM_DOT11_COUNTRY_STRING_SIZE);
						get_param_cnf->ParameterLength = sizeof(WFM_DOT11_COUNTRY_STRING) - sizeof(uint8);
					}
				} else {
					if (pUmacInstance->gVars.p.InfoHost.countryStringAvl) {
						OS_MemoryCopy(get_param_cnf->ParameterData.CountryStringInfo.dot11CountryString, pUmacInstance->gVars.p.InfoHost.CountryStringInfo->countryString, WFM_DOT11_COUNTRY_STRING_SIZE);
						get_param_cnf->ParameterLength = sizeof(WFM_DOT11_COUNTRY_STRING) - sizeof(uint8);
					}
				}
				break;
			}
		case WFM_DEVICE_OID_802_11_BSSID_LIST:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_BSSID_LIST\n");

				/* Number of devices in the cache */
				DevCount = WFM_UMAC_DEV_GetListSize(pUmacInstance);

				if (DevCount) {
					/*
					   Right now we are sending onlly one entry to host.
					   TBD: To be enhanced after discussing section 4.4
					   WFM HI MEssages.doc with Hong

					 */
					pDevElement = WFM_UMAC_DEV_GetNextElement(pUmacInstance);

					if (pDevElement) {
						pBssInd = (WFM_BSS_CACHE_INFO_IND *) pDevElement->DeviceElement;

						OS_MemoryCopy(&get_param_cnf->ParameterData.bss_cache_info_ind, pBssInd, pDevElement->ElementSize);

						get_param_cnf->ParameterLength = (uint16) pDevElement->ElementSize;

					}	/*if ( pDevElement ) */

				} else {
					get_param_cnf->ParameterLength = 0;

				}

				break;
			}
		case WFM_DEVICE_OID_802_11_BSSID:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_BSSID\n");
				/* Get bssInfo from current instance */
				if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
											     && (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {
					OS_MemoryReset(&get_param_cnf->ParameterData.BssidOid, sizeof(get_param_cnf->ParameterData.BssidOid));
					OS_MemoryCopy((void *)get_param_cnf->ParameterData.BssidOid.Bssid, pUmacInstance->RemoteMacAdd, WFM_MAC_ADDRESS_SIZE);
					get_param_cnf->ParameterLength = WFM_MAC_ADDRESS_SIZE;
				} else {
					get_param_cnf->ParameterLength = 0;
					status = WFM_STATUS_FAILURE;
				}

				LOG_DATA(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_BSSID : %d\n", get_param_cnf->ParameterLength);

				break;
			}
		case WFM_DEVICE_OID_802_11_AUTHENTICATION_MODE:
			{
				get_param_cnf->ParameterData.dot11AuthenticationMode = (uint8)
				    pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost;
				get_param_cnf->ParameterLength = sizeof(get_param_cnf->ParameterData.dot11AuthenticationMode);

				LOG_DATA(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ENCRYPTION, "GET_OID: WFM_DEVICE_OID_802_11_AUTHENTICATION_MODE = %d\n", get_param_cnf->ParameterData.dot11AuthenticationMode);

				break;
			}
		case WFM_DEVICE_OID_802_11_ASSOCIATION_INFORMATION:
			{
				WFM_OID_802_11_ASSOCIATION_INFORMATION *pAssocInfo;
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_ASSOCIATION_INFORMATION\n");

				pAssocInfo = &get_param_cnf->ParameterData.dot11AssocInfo;

				if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
					pAssocInfo->Capabilities_Req = pUmacInstance->sta[linkId].capabilities;
					pAssocInfo->Listen_Interval = pUmacInstance->sta[linkId].listenInterval;

					OS_MemoryCopy((void *)&pAssocInfo->Current_AP_Address[0],
						(void *)&pUmacInstance->sta[linkId].currentAPAddress,
						WFM_MAC_ADDRESS_SIZE);

					pAssocInfo->Capabilities_Resp = pUmacInstance->sta[linkId].capabilityResp;
					pAssocInfo->Status_Code = pUmacInstance->sta[linkId].AssocRespStatusCode;
					pAssocInfo->Association_Id = pUmacInstance->sta[linkId].linkId;	/* 802.11 frame endianness (little endian) */
					pAssocInfo->VariableIELenReq = pUmacInstance->sta[linkId].AssocReqIELen;
					pAssocInfo->VariableIELenRsp = pUmacInstance->sta[linkId].AssocRespIELen;

					if (pAssocInfo->VariableIELenReq)
						OS_MemoryCopy((void *)((uint8 *) pAssocInfo + sizeof(WFM_OID_802_11_ASSOCIATION_INFORMATION)),
						&pUmacInstance->sta[linkId].AssocReqIE[0],
						pUmacInstance->sta[linkId].AssocReqIELen);

					if (pAssocInfo->VariableIELenRsp)
						OS_MemoryCopy((void *)((uint8 *) pAssocInfo + sizeof(WFM_OID_802_11_ASSOCIATION_INFORMATION) + pAssocInfo->VariableIELenReq),
						&pUmacInstance->sta[linkId].AssocRspIE[0],
						pUmacInstance->sta[linkId].AssocRespIELen);

					get_param_cnf->ParameterLength = sizeof(WFM_OID_802_11_ASSOCIATION_INFORMATION) +
						pUmacInstance->sta[linkId].AssocReqIELen +
						pUmacInstance->sta[linkId].AssocRespIELen;


				} else {
					if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED)
						|| ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
						&& (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {


						pAssocInfo->Capabilities_Req = pUmacInstance->CapabilitiesReq;	/* 802.11 frame endianness (little endian) */

						pAssocInfo->Listen_Interval = pUmacInstance->AssocListenInterval;	/* 802.11 frame endianness (little endian) */

						OS_MemoryCopy((void *)&pAssocInfo->Current_AP_Address[0], (void *)&pUmacInstance->RemoteMacAdd[0], WFM_MAC_ADDRESS_SIZE);

						pAssocInfo->Capabilities_Resp = pUmacInstance->CapabilitiesRsp;
						pAssocInfo->Status_Code = pUmacInstance->AssocStatusCode;
						pAssocInfo->Association_Id = pUmacInstance->AssociationId;	/* 802.11 frame endianness (little endian) */
						pAssocInfo->VariableIELenReq = pUmacInstance->AssocReqIELen;
						pAssocInfo->VariableIELenRsp = pUmacInstance->AssocRespIELen;

						if (pUmacInstance->AssocReqIELen)
							OS_MemoryCopy((void *)((uint8 *) pAssocInfo + sizeof(WFM_OID_802_11_ASSOCIATION_INFORMATION)), pUmacInstance->AssocReqIE, pUmacInstance->AssocReqIELen);

						if (pUmacInstance->AssocRespIELen)
							OS_MemoryCopy((void *)((uint8 *) pAssocInfo + sizeof(WFM_OID_802_11_ASSOCIATION_INFORMATION) + pUmacInstance->AssocReqIELen), pUmacInstance->AssocRspIE, pUmacInstance->AssocRespIELen);

						get_param_cnf->ParameterLength = sizeof(WFM_OID_802_11_ASSOCIATION_INFORMATION) + pUmacInstance->AssocReqIELen + pUmacInstance->AssocRespIELen;
					} else
						get_param_cnf->ParameterLength = 0;
				}

				break;
			}
		case WFM_DEVICE_OID_802_11_RSSI_RCPI:
			{
				/*-10 through -200 dBm for windows. pac returns rssi in Q8.0 format (sint8) */
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_RSSI_RCPI\n");
				LOG_DATA(DBG_WFM_HI | DBG_WFM_OID, "GETOID: RSSI=%d ******\n", pUmacInstance->RxRSSI);
				get_param_cnf->ParameterData.RCPI_Value = pUmacInstance->RxRcpi;
				get_param_cnf->ParameterLength = sizeof(get_param_cnf->ParameterData.RCPI_Value);
				break;
			}
		case WFM_DEVICE_OID_802_11_RSSI_TRIGGER:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "IGNORED: GET_OID: WFM_DEVICE_OID_802_11_RSSI_TRIGGER\n");

				break;
			}
		case WFM_DEVICE_OID_802_11_MEDIA_STREAM_MODE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_MEDIA_STREAM_MODE\n");

				get_param_cnf->ParameterData.Streaming_Mode = WFM_802_MediaStreamOff;
				get_param_cnf->ParameterLength = sizeof(get_param_cnf->ParameterData.Streaming_Mode);

				break;
			}
		case WFM_DEVICE_OID_802_11_TX_ANTENNA_SELECTED:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "IGNORED: GET_OID: WFM_DEVICE_OID_802_11_TX_ANTENNA_SELECTED\n");

				break;
			}
		case WFM_DEVICE_OID_802_11_RX_ANTENNA_SELECTED:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "IGNORED: GET_OID: WFM_DEVICE_OID_802_11_RX_ANTENNA_SELECTED\n");

				break;
			}
		case WFM_DEVICE_OID_802_11_SUPPORTED_DATA_RATES:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "IGNORED: GET_OID: WFM_DEVICE_OID_802_11_SUPPORTED_DATA_RATES\n");

				break;
			}
		case WFM_DEVICE_OID_802_11_TX_POWER_LEVEL:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "IGNORED: GET_OID: WFM_DEVICE_OID_802_11_TX_POWER_LEVEL\n");
				get_param_cnf->ParameterData.TxPowerLevel = pUmacInstance->TxPowerLevel;
				get_param_cnf->ParameterLength = sizeof(sint32);
				break;
			}
		case WFM_DEVICE_OID_802_11_NETWORK_TYPE_IN_USE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_NETWORK_TYPE_IN_USE\n");

				get_param_cnf->ParameterData.dot11NetworkTypeInUse = WFM_802_11OFDM24;
				get_param_cnf->ParameterLength = 4;

				break;
			}
		case WFM_DEVICE_OID_802_11_INFRASTRUCTURE_MODE:
			{
				if (pUmacInstance->dot11InfraStructureMode < WFM_802_INFRASTRUCTURE_MODE_MAX_VALUE) {
					get_param_cnf->ParameterData.dot11InfraStructureMode = pUmacInstance->dot11InfraStructureMode;
					get_param_cnf->ParameterLength = 4;

				} else {
					get_param_cnf->ParameterLength = 0;
					status = WFM_STATUS_FAILURE;
				}

				LOG_DATA2(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_INFRASTRUCTURE_MODE: len=%d, mode=%d\n", get_param_cnf->ParameterLength, get_param_cnf->ParameterData.dot11InfraStructureMode);

				break;
			}
		case WFM_DEVICE_OID_802_11_SSID:
			{

				if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
											     && (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {
					if (pUmacInstance->SsidLength)
						OS_MemoryCopy((void *)get_param_cnf->ParameterData.ssid.Ssid, pUmacInstance->Ssid, pUmacInstance->SsidLength);
					else
						LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_SSID - Len is 0 ERROR?\n");
					get_param_cnf->ParameterData.ssid.SsidLength = pUmacInstance->SsidLength;
				} else {
					get_param_cnf->ParameterData.ssid.SsidLength = 0;
				}
				get_param_cnf->ParameterLength = (uint16) get_param_cnf->ParameterData.ssid.SsidLength + sizeof(get_param_cnf->ParameterData.ssid.SsidLength);

				LOG_DATA(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_SSID : ssidlen=%d\n", get_param_cnf->ParameterData.ssid.SsidLength);

				LOG_STRING(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "GET_OID: WFM_DEVICE_OID_802_11_SSID : ssid=\n", get_param_cnf->ParameterData.ssid.Ssid, get_param_cnf->ParameterData.ssid.SsidLength, get_param_cnf->ParameterData.ssid.SsidLength);

				break;
			}

		case WFM_DEVICE_OID_802_11_ENCRYPTION_STATUS:
			{
				status = (WFM_STATUS_CODE) WFM_UMAC_GetOid_EncryptionStatus(UmacHandle, &(get_param_cnf->ParameterData.Encryption_Status)
				    );
				get_param_cnf->ParameterLength = sizeof(get_param_cnf->ParameterData.Encryption_Status);

				LOG_DATA(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ENCRYPTION, "GET_OID: WFM_DEVICE_OID_802_11_ENCRYPTION_STATUS = 0x%x\n", get_param_cnf->ParameterData.Encryption_Status);

				break;
			}

		case WFM_DEVICE_OID_802_11_PRIVACY_FILTER:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "IGNORED: GET_OID: WFM_DEVICE_OID_802_11_PRIVACY_FILTER\n");

				break;
			}

		case WFM_DEVICE_OID_802_11_STATISTICS:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "IGNORED: GET_OID: WFM_DEVICE_OID_802_11_STATISTICS\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}

		case WFM_DEVICE_OID_802_11_POWER_MODE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_POWER_MODE\n");

				get_param_cnf->ParameterData.psMode.PmMode = pUmacInstance->gVars.p.PsMode.PmMode;
				get_param_cnf->ParameterData.psMode.FastPsmIdlePeriod = pUmacInstance->gVars.p.PsMode.FastPsmIdlePeriod;
				get_param_cnf->ParameterData.psMode.ApPsmChangePeriod = pUmacInstance->gVars.p.PsMode.ApPsmChangePeriod;
				get_param_cnf->ParameterLength = sizeof(get_param_cnf->ParameterData.psMode);

				break;
			}
		case WFM_DEVICE_OID_802_11_BLOCK_ACK_POLICY:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_BLOCK_ACK_POLICY\n");

				get_param_cnf->ParameterData.block_ack_policy.BlockAckTxTidPolicy = (uint8) pUmacInstance->UmacTxblkackpolicy;
				get_param_cnf->ParameterData.block_ack_policy.BlockAckRxTidPolicy = (uint8) pUmacInstance->UmacRxblkackpolicy;
				get_param_cnf->ParameterLength = sizeof(get_param_cnf->ParameterData.block_ack_policy);

				break;
			}
		case WFM_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE\n");

				get_param_cnf->ParameterData.OprPwrMode = (uint32) pUmacInstance->gVars.p.OprPwrMode;
				get_param_cnf->ParameterLength = sizeof(get_param_cnf->ParameterData.OprPwrMode);

				break;
			}
		case WFM_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD\n");
				OS_MemoryCopy(&get_param_cnf->ParameterData.RcpiRssiThreshold, &pUmacInstance->gVars.p.RcpiRssiThreshold, sizeof(WFM_RCPI_RSSI_THRESHOLD));
				get_param_cnf->ParameterLength = sizeof(WFM_RCPI_RSSI_THRESHOLD);
				break;

			}
		case WFM_DEVICE_OID_802_11_SET_PMKID_BKID:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_SET_PMKID\n");
				get_param_cnf->ParameterData.PmkidBkidInfo.bssidInfoCount = pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfoCount;
				OS_MemoryCopy(&get_param_cnf->ParameterData.PmkidBkidInfo.bssidInfo[0], &pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfo[0], (get_param_cnf->ParameterData.PmkidBkidInfo.bssidInfoCount * sizeof(WFM_BSSID_INFO)));
				get_param_cnf->ParameterLength = (uint16) (sizeof(uint32) + (get_param_cnf->ParameterData.PmkidBkidInfo.bssidInfoCount * sizeof(WFM_BSSID_INFO)));
				break;
			}
		case WFM_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT\n");
				OS_MemoryCopy(&get_param_cnf->ParameterData.BeaconLostCount.BeaconLostCount, &pUmacInstance->gVars.p.BeaconLostCount.BeaconLostCount, sizeof(uint8));
				get_param_cnf->ParameterLength = sizeof(WFM_BEACON_LOST_COUNT);
				break;
			}
		case WFM_DEVICE_OID_802_11_TX_FAILURE_THRESHOLD_COUNT:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_TX_FAILURE_THRESHOLD_COUNT\n");
				OS_MemoryCopy(&get_param_cnf->ParameterData.TxFailureCount.TransmitFailureThresholdCount, &pUmacInstance->gVars.p.TxFailureCount.TransmitFailureThresholdCount, sizeof(WFM_TX_FAILURE_THRESHOLD_COUNT));
				get_param_cnf->ParameterLength = sizeof(WFM_TX_FAILURE_THRESHOLD_COUNT);
				break;
			}

#if MGMT_FRAME_PROTECTION
		case WFM_DEVICE_OID_802_11_PROTECTED_MGMT_POLICY:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: WFM_DEVICE_OID_802_11_PROTECTED_MGMT_POLICY\n");
				get_param_cnf->ParameterData.procMgmtFrmPolicy = pUmacInstance->hostMfpPolicy;
				get_param_cnf->ParameterLength = sizeof(pUmacInstance->hostMfpPolicy);
				break;
			}
#endif	/*MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
		case UMAC_DEVICE_OID_802_11_USE_P2P:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: UMAC_DEVICE_OID_802_11_USE_P2P\n");
				get_param_cnf->ParameterData.useP2P.useP2P = pUmacInstance->gVars.p.useP2P;
				get_param_cnf->ParameterLength = sizeof(UMAC_OID_USE_P2P);
				break;
			}

		case UMAC_DEVICE_OID_802_11_GET_GROUP_TSC:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: UMAC_DEVICE_OID_802_11_GET_GROUP_TSC\n");
				umac_event = UMAC_EXT_REQ_TO_WSM;
				break;
			}
#endif	/*P2P_MINIAP_SUPPORT */
		case UMAC_DEVICE_OID_802_11_GET_DEVICE_MAC_ADDRESS:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "IGNORED: GET_OID: UMAC_DEVICE_OID_802_11_GET_DEVICE_MAC_ADDRESS\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case UMAC_DEVICE_OID_802_11_GET_LINK_SPEED:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "IGNORED: GET_OID: UMAC_DEVICE_OID_802_11_GET_LINK_SPEED\n");

#if P2P_MINIAP_SUPPORT
				if (linkId)
					get_param_cnf->ParameterData.CurrentRateIndex = pUmacInstance->sta[linkId].currentRateIndex;
				else
#endif	/* P2P_MINIAP_SUPPORT */
					get_param_cnf->ParameterData.CurrentRateIndex = pUmacInstance->CurrentRateIndex;

				get_param_cnf->ParameterLength = sizeof(get_param_cnf->ParameterData.CurrentRateIndex);
				break;
			}
		case UMAC_DEVICE_OID_802_11_P2P_PS_MODE:
			{
				OS_MemoryCopy(&get_param_cnf->ParameterData.p2pPSMode,
					&pUmacInstance->gVars.p.p2pPsMode,
					sizeof(UMAC_P2P_PS_MODE));
				get_param_cnf->ParameterLength = sizeof(UMAC_P2P_PS_MODE);
				break;

			}
		case UMAC_DEVICE_OID_802_11_GET_LOCAL_AMP_INFO:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: UMAC_DEVICE_OID_802_11_GET_LOCAL_AMP_INFO\n");
				get_param_cnf->ParameterLength = sizeof(UMAC_LOCAL_AMP_INFO);
				get_param_cnf->ParameterData.LocalAmpInfo.TotalBandwidth = 30000;
				get_param_cnf->ParameterData.LocalAmpInfo.MaxGuaranteedBandwidth = 30000;
				get_param_cnf->ParameterData.LocalAmpInfo.MinLatency = 0;
				get_param_cnf->ParameterData.LocalAmpInfo.MaxPduSize = 1500;
				get_param_cnf->ParameterData.LocalAmpInfo.PalCapabilities = 0x01;
				get_param_cnf->ParameterData.LocalAmpInfo.MaxAmpAssocLength = 1500;
				break;
			}
		case UMAC_DEVICE_OID_802_11_GET_AMP_ASSOC_INFO:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "GET_OID: UMAC_DEVICE_OID_802_11_GET_AMP_ASSOC_INFO\n");
				get_param_cnf->ParameterLength = sizeof(UMAC_AMP_ASSOC_INFO);
				OS_MemoryCopy(&get_param_cnf->ParameterData.AmpAssocInfo.MacAddress[0], &pUmacInstance->MacAddress[0], WFM_MAC_ADDRESS_SIZE);
				get_param_cnf->ParameterData.AmpAssocInfo.PalCapabilities = 1;
				if ((pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PHY_BAND_2G] == TRUE)
					|| (pUmacInstance->gVars.p.InfoHost.countryStringAvl == TRUE)) {
						if (!WFM_UMAC_FetchPreferredChannelFromTable(&pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString[0],
							&get_param_cnf->ParameterData.AmpAssocInfo.PreferredChannel.FirstChannel,
							&get_param_cnf->ParameterData.AmpAssocInfo.PreferredChannel.MaxTxPower,
							&get_param_cnf->ParameterData.AmpAssocInfo.PreferredChannel.NoOfChannel)) {
								OS_MemoryCopy(&get_param_cnf->ParameterData.AmpAssocInfo.PreferredChannel.CountryString.dot11CountryString[0],
									  &pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString[0],
									  WFM_DOT11_COUNTRY_STRING_SIZE);
								get_param_cnf->ParameterData.AmpAssocInfo.PreferredChannel.CoverageClass = UMAC_DEFAULT_COVERAGE_CLASS;
								get_param_cnf->ParameterData.AmpAssocInfo.PreferredChannel.RegulatoryClass = 1;
								get_param_cnf->ParameterData.AmpAssocInfo.PreferredChannel.RegulatoryExtension = 201;
						} else {
							get_param_cnf->ParameterLength = 0;
							status = WFM_STATUS_FAILURE;
						}
				} else {
					get_param_cnf->ParameterLength = 0;
					status = WFM_STATUS_FAILURE;
				}
				break;
			}
#if TX_PER_SUPPORT
		case UMAC_DEVICE_OID_802_11_GET_TX_STATS:
			{
				OS_MemoryCopy(&get_param_cnf->ParameterData.TxStats,
					&pUmacInstance->TxStats,
					sizeof(pUmacInstance->TxStats));
				get_param_cnf->ParameterLength = sizeof(pUmacInstance->TxStats);
				OS_MemoryReset(&pUmacInstance->TxStats, sizeof(pUmacInstance->TxStats));
				break;
			}
#endif
		default:
			{
				LOG_DATA(DBG_WFM_ERROR, "ERROR: GETOID : Bad OID 0x%x\n", get_param_req->ParameterId);
				status = WFM_STATUS_BAD_PARAM;
				break;
			}
		} /* end switch(get_param_req->ParameterId) */

		if ((umac_event != UMAC_MAX_EVT))
			status = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, umac_event, hi_msg);

		if (WFM_STATUS_PENDING == status) {
			hi_msg->MessageId = (hi_msg->MessageId | 0x1000);
			UMAC_QUEUE_PENDING_HI_MSG(UmacHandle, umac_event, hi_msg);
			goto exit_handler;
		} /*if( WFM_STATUS_PENDING == status ) */
	} else {
		status = WFM_STATUS_BAD_PARAM;
		LOG_EVENT(DBG_WFM_ERROR | DBG_WFM_OID, "ERROR: GET_OID with HiMessageLength 0\n");
		goto exit_handler;
	} /*if(hi_msg->MessageLength) */


	if ((WFM_STATUS_PENDING != status) && (hi_msg->MessageId & 0x1000)) {
		get_param_cnf->ParameterId = get_param_req->ParameterId;
		get_param_cnf->Reference = get_param_req->Reference;
		get_param_cnf->Status = status;

		hi_msg->MessageId = WFM_GET_PARAM_CNF_ID;
		hi_msg->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + WFM_GET_PARAM_CNF_HDR_SIZE + get_param_cnf->ParameterLength;

		/* Sending Get Param Cnf to Host */
		pUmacInstance->gVars.p.UpperLayerCb(
						pUmacInstance->gVars.p.ulHandle,
						hi_msg
						);
	} /*if( (WFM_STATUS_PENDING != status) && (hi_msg->MessageId &0x0100))*/
	else {

		get_param_cnf->ParameterId = get_param_req->ParameterId;
		get_param_cnf->Reference = get_param_req->Reference;
		get_param_cnf->Status = status;

		hi_msg->MessageId = WFM_GET_PARAM_CNF_ID;
		hi_msg->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + WFM_GET_PARAM_CNF_HDR_SIZE + get_param_cnf->ParameterLength;
	}

 exit_handler:
	return status;
} /*end WFM_HI_Proc_GetParamReq() */

/******************************************************************************
 * NAME:	WFM_HI_Proc_SetParamReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the SetParameter request.
 * \param UmacHandle   - Handle to the UMAC Instance.
 * \param *hi_msg      - Message from host
 * \returns uint16       Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_SetParamReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg)
{
	WFM_SET_PARAM_CNF *set_param_cnf = NULL;
	WFM_SET_PARAM_REQ *set_param_req = NULL;
	uint32 LmacStatus = WFM_STATUS_SUCCESS;
	uint16 umac_event = UMAC_MAX_EVT;
	void *pMsg = (void *)hi_msg;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
#if P2P_MINIAP_SUPPORT
	uint8 linkId = (uint8) ((hi_msg->MessageId & UMAC_HI_MSG_LINK_ID) >> 6);
#endif	/* P2P_MINIAP_SUPPORT */


	LOG_EVENT(DBG_WFM_HI, "WFM_HI_Proc_SetParamReq()\n");

	OS_ASSERT(pUmacInstance);

	if (hi_msg->MessageLength) {
		set_param_req = &hi_msg->PayLoad.wfm_set_param_req;
		set_param_cnf = &hi_msg->PayLoad.wfm_set_param_cnf;

		LOG_DATA(DBG_WFM_HI | DBG_WFM_OID, "WFM_HI_Proc_SetParamReq(): Oid=0x%x\n", set_param_req->ParameterId);

		switch (set_param_req->ParameterId) {

		case WFM_DEVICE_OID_802_11_COUNTRY_STRING:
			{
				if (pUmacInstance->gVars.p.Enable11dFeature) {
					/*Here we are checking the country string that driver has sent with the country string that is already stored
					   in the pUmacInstance.
					   There are 2 cases:
					   1) If country string that driver sends is different from what is available in pUmacInstance then UMAC sends
					   WFM_STATUS_COUNTRY_INFO_MISMATCH.Now if driver again sends the same country string request then UMAC
					   updates the country string info in pUmacInstance.
					   2) If country string that driver has sent is same as what is stored in pUmacInstance then UMAC returns WFM_STATUS_SUCCESS
					   to driver.We need not to update this country info in pUmacInstance as it is already available in pUmacInstance.
					   3) If driver sends the country string that is not available in UMAC table then it sends WFM_STATUS_COUNTRY_NOT_FOUND_IN_TABLE */

					uint8 PhyBand = PHY_BAND_2G;
					uint32 CountryStrCompare = FALSE;
					LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_COUNTRY_STRING\n");
					CountryStrCompare = OS_MemoryEqual(&set_param_req->ParameterData.CountryStringInfo.dot11CountryString[0],
									   (uint32) WFM_DOT11_COUNTRY_STRING_SIZE, &pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PhyBand].countryString[0], (uint32) WFM_DOT11_COUNTRY_STRING_SIZE);

					if (pUmacInstance->gVars.p.CountryStrReqFromHost == 0) {
						if (pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PhyBand] == TRUE) {
							pUmacInstance->gVars.p.CountryStrReqFromHost++;
							if (!CountryStrCompare) {
								LmacStatus = WFM_STATUS_COUNTRY_INFO_MISMATCH;
								goto exit_handler;
							} else {
								LmacStatus = WFM_STATUS_SUCCESS;
								goto exit_handler;
							}
						} else
							pUmacInstance->gVars.p.CountryStrReqFromHost++;
					}
					if (pUmacInstance->gVars.p.CountryStrReqFromHost) {
						if (pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PhyBand] == TRUE) {
							/*We are updating the string info based on the country string given by driver */
							/*So reset the previous content of Country String Info in pUmacInstance */
							OS_MemoryReset(&pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PhyBand], sizeof(WFM_COUNTRY_STRING_INFO));
							pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PhyBand] = FALSE;
						}
						/*Update the channels and transmit Power for 2.4 Ghz */
						LmacStatus = WFM_UMAC_FetchCountryStringInfoFromTable(UmacHandle, &set_param_req->ParameterData.CountryStringInfo.dot11CountryString[0], PhyBand);
						if (LmacStatus == WFM_STATUS_SUCCESS)
							pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PhyBand] = TRUE;
						else {
							/*Country not found in table */
							LmacStatus = WFM_STATUS_COUNTRY_NOT_FOUND_IN_TABLE;
						}
						/*Update the channels for 5 Ghz */
						PhyBand = PHY_BAND_5G;
						if (pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PhyBand] == TRUE) {
							OS_MemoryReset(&pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PhyBand], sizeof(WFM_COUNTRY_STRING_INFO));
							pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PhyBand] = FALSE;
						}
						LmacStatus = WFM_UMAC_FetchCountryStringInfoFromTable(UmacHandle, &set_param_req->ParameterData.CountryStringInfo.dot11CountryString[0], PhyBand);
						if (LmacStatus == WFM_STATUS_SUCCESS)
							pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PhyBand] = TRUE;
						else {
							/*Country not found in table */
							LmacStatus = WFM_STATUS_COUNTRY_NOT_FOUND_IN_TABLE;
						}
						pUmacInstance->gVars.p.CountryStrReqFromHost = 0;
					}
				} else {
					/*If 11d feature is disabled then we are just upating country String to get the regulatory
					   class table */
					if (!pUmacInstance->gVars.p.InfoHost.countryStringAvl) {
						OS_MemoryCopy((uint8 *) pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString, &set_param_req->ParameterData.CountryStringInfo.dot11CountryString[0], WFM_DOT11_COUNTRY_STRING_SIZE);
						OS_MemoryCopy((uint8 *) pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_5G].countryString, &set_param_req->ParameterData.CountryStringInfo.dot11CountryString[0], WFM_DOT11_COUNTRY_STRING_SIZE);
						pUmacInstance->gVars.p.InfoHost.countryStringAvl = TRUE;
					}

				}
				break;
			}


		case WFM_DEVICE_OID_802_11_BSSID_LIST_SCAN:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_BSSID_LIST_SCAN\n");

				if (set_param_req->ParameterLength) {

					if (set_param_req->ParameterData.bssListScan.Flags)
						umac_event = UMAC_START_SCAN;
					else
						umac_event = UMAC_STOP_SCAN;

					pMsg = (void *)&set_param_req->ParameterData.bssListScan;

				}	/*if( set_param_req->ParameterLength ) */
				else
					LmacStatus = WFM_STATUS_BAD_PARAM;

				break;
			}
		case WFM_DEVICE_OID_802_11_BSSID:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ASSOC | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_BSSID\n");

				if (set_param_req->ParameterLength) {
					umac_event = UMAC_START_JOIN;
					pMsg = (void *)&set_param_req->ParameterData.BssInfo;
				}

				else if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode) {
					umac_event = UMAC_START_JOIN;
					LOG_EVENT(DBG_WFM_HI, "WFM_HI_Proc_SetParamReq(): IBSS\n");
				} else
					LmacStatus = WFM_STATUS_BAD_PARAM;
				/*Update roaming Enabled parameter */
				pUmacInstance->RoamingEnabled = set_param_req->ParameterData.BssInfo.RoamingEnabled;
				break;
			}
		case WFM_DEVICE_OID_802_11_ADD_KEY:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ENCRYPTION | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_ADD_KEY\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case WFM_DEVICE_OID_802_11_AUTHENTICATION_MODE:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ENCRYPTION | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_AUTHENTICATION_MODE\n");

				LmacStatus = WFM_UMAC_SetOid_AuthenticationMode(UmacHandle, set_param_req->ParameterData.dot11AuthenticationMode);

				break;
			}
		case WFM_DEVICE_OID_802_11_REMOVE_KEY:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ENCRYPTION | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_REMOVE_KEY\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case WFM_DEVICE_OID_802_11_DISASSOCIATE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_DISASSOCIATE\n");
#if P2P_MINIAP_SUPPORT
				if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP) && (linkId > 0)) {
					if ((pUmacInstance->sta[linkId].state != STA_STATE_DEAUTHENTICATING)
						&& (pUmacInstance->sta[linkId].state != STA_STATE_NONE))
						umac_event = UMAC_DE_ASSOC;
				} else
#endif /* P2P_MINIAP_SUPPORT */
					umac_event = UMAC_DE_ASSOC;
				break;
			}
		case WFM_DEVICE_OID_802_11_RSSI_TRIGGER:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "IGNORED: SET_OID: WFM_DEVICE_OID_802_11_RSSI_TRIGGER\n");

				break;
			}
		case WFM_DEVICE_OID_802_11_MEDIA_STREAM_MODE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "IGNORED: SET_OID: WFM_DEVICE_OID_802_11_MEDIA_STREAM_MODE\n");

				break;
			}
		case WFM_DEVICE_OID_802_11_TX_ANTENNA_SELECTED:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "IGNORED: SET_OID: WFM_DEVICE_OID_802_11_TX_ANTENNA_SELECTED\n");

				break;
			}
		case WFM_DEVICE_OID_802_11_RX_ANTENNA_SELECTED:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "IGNORED: SET_OID: WFM_DEVICE_OID_802_11_RX_ANTENNA_SELECTED\n");

				break;
			}
		case WFM_DEVICE_OID_802_11_TX_POWER_LEVEL:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "IGNORED: SET_OID: WFM_DEVICE_OID_802_11_TX_POWER_LEVEL\n");
				OS_MemoryCopy(&pUmacInstance->TxPowerLevel, &set_param_req->ParameterData.TxPowerLevel, sizeof(sint32));
				break;
			}

		case WFM_DEVICE_OID_802_11_INFRASTRUCTURE_MODE:
			{
				LOG_DATA(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ENCRYPTION | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_INFRASTRUCTURE_MODE = %d\n", set_param_req->ParameterData.dot11InfraStructureMode);

				LmacStatus = WFM_UMAC_SetOid_InfrastructureMode(UmacHandle, set_param_req->ParameterData.dot11InfraStructureMode);

				break;
			}

		case WFM_DEVICE_OID_802_11_SSID:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ASSOC | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SSID\n");
				LOG_DATA(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "ssidlen=%d\n", ((WFM_BSS_INFO *)&set_param_req->ParameterData.BssInfo)->SsidLength);

				LOG_STRING(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "ssid=", ((WFM_BSS_INFO *)&set_param_req->ParameterData.BssInfo)->Ssid, ((WFM_BSS_INFO *)&set_param_req->ParameterData.BssInfo)->SsidLength, 24	/* print max 24 chars of ssid */
				    );

				if (set_param_req->ParameterLength) {
					umac_event = UMAC_START_JOIN;
					pMsg = (void *)&set_param_req->ParameterData.BssInfo;
					if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode) {
						pUmacInstance->SsidLength = ((WFM_BSS_INFO *) &set_param_req->ParameterData.BssInfo)->SsidLength;

						/*set_param_req->ParameterData.BssInfo.Capabilities = WFM_UMAC_SUPPORTED_CAPABILITIES ; */

						OS_MemoryCopy(pUmacInstance->Ssid, ((WFM_BSS_INFO *) &set_param_req->ParameterData.BssInfo)->Ssid, pUmacInstance->SsidLength);
					}
				} else
					LmacStatus = WFM_STATUS_BAD_PARAM;

				break;
			}

		case WFM_DEVICE_OID_802_11_ENCRYPTION_STATUS:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ENCRYPTION | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_ENCRYPTION_STATUS\n");

				pUmacInstance->enableWEP = (uint8) (set_param_req->ParameterData.Encryption_Status & WFM_UMAC_WEP_ENC_SUPPORTED);

#if P2P_MINIAP_SUPPORT
				pUmacInstance->dot11BssConf.vdrSpecificCapInfo &= WFM_BSS_INFO_FLAGS_BSS_WMM_CAP;
				pUmacInstance->dot11BssConf.vdrSpecificCapInfo |= (uint8) (set_param_req->ParameterData.Encryption_Status << 5);

#endif	/*P2P_MINIAP_SUPPORT */
				LmacStatus = WFM_UMAC_SetOid_EncryptionStatus(UmacHandle, set_param_req->ParameterData.Encryption_Status);

				break;
			}

		case WFM_DEVICE_OID_802_11_PRIVACY_FILTER:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "IGNORED: SET_OID: WFM_DEVICE_OID_802_11_PRIVACY_FILTER\n");

				break;
			}

		case WFM_DEVICE_OID_802_11_POWER_MODE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_POWER_MODE\n");

				LOG_DATA(DBG_WFM_CURRENT_GM, "PMMODE = %d\n", set_param_req->ParameterData.psMode);

				if ((set_param_req->ParameterData.psMode.PmMode <= WFM_PS_MODE_ENABLED)
					|| (set_param_req->ParameterData.psMode.PmMode == (WFM_PS_MODE_FLAG_FAST_PSM_ENABLE | WFM_PS_MODE_ENABLED))) {
						if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
							&& (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {
								/*If UMAC is Associated then check whether BSS Param set in firmware */
								if (pUmacInstance->IsFirmwareBssParamSet == TRUE)
									umac_event = UMAC_EXT_REQ_TO_WSM;
								else
									LmacStatus = WFM_STATUS_PENDING;
						} else
							LmacStatus = WFM_STATUS_FAILURE;
				} else {
					LOG_EVENT(DBG_WFM_ERROR, "Bad parameter for set PS mode, over riding it with default value\n");
					LmacStatus = WFM_STATUS_BAD_PARAM;
				}

				break;
			}
		case WFM_DEVICE_OID_802_11_BLOCK_ACK_POLICY:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_BLOCK_ACK_POLICY\n");
				if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
					&& (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {
						umac_event = UMAC_EXT_REQ_TO_WSM;
				} else {
					pUmacInstance->UmacTxblkackpolicy = set_param_req->ParameterData.block_ack_policy.BlockAckTxTidPolicy;
					pUmacInstance->UmacRxblkackpolicy = set_param_req->ParameterData.block_ack_policy.BlockAckRxTidPolicy;
				}

				break;
			}

		case WFM_DEVICE_OID_802_11_UPDATE_EPTA_CONFIG_DATA:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_UPDATE_EPTA_CONFIG_DATA\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;

			}

		case WFM_DEVICE_OID_802_11_SET_AUTO_CALIBRATION_MODE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "SET_OID: WFM_DEVICE_OID_802_11_SET_AUTO_CALIBRATION_MODE\n");

				pUmacInstance->setAutoCalibrationMode = set_param_req->ParameterData.setAutoCalibrationMode;

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}

		case WFM_DEVICE_OID_802_11_CONFIGURE_IBSS:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "SET_OID: WFM_DEVICE_OID_802_11_CONFIGURE_IBSS\n");

				pUmacInstance->Channelnumber = set_param_req->ParameterData.configureIBSS.channelNum;
				pUmacInstance->enableWMM = set_param_req->ParameterData.configureIBSS.enableWMM;
				pUmacInstance->enableWEP = set_param_req->ParameterData.configureIBSS.enableWEP;
				pUmacInstance->atimWinSize = set_param_req->ParameterData.configureIBSS.atimWinSize;
				pUmacInstance->beaconInterval = set_param_req->ParameterData.configureIBSS.beaconInterval;
				pUmacInstance->networkTypeInUse = set_param_req->ParameterData.configureIBSS.networkTypeInUse;
				break;
			}

#if P2P_MINIAP_SUPPORT
		case UMAC_DEVICE_OID_802_11_START_AP:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "SET_OID: WFM_DEVICE_OID_802_11_START_AP\n");
				OS_MemoryCopy(&pUmacInstance->dot11BssConf,
					(UMAC_OID_802_11_START_AP *) &set_param_req->ParameterData.dot11StartAp,
					sizeof(UMAC_OID_802_11_START_AP)
					);

				pMsg = (void *)&set_param_req->ParameterData.dot11StartAp;
				umac_event = UMAC_START_AP;
				break;
			}

		case UMAC_DEVICE_OID_802_11_USE_P2P:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "SET_OID: UMAC_DEVICE_OID_802_11_USE_P2P\n");

			pUmacInstance->gVars.p.useP2P = set_param_req->ParameterData.useP2P.useP2P;
			if (pUmacInstance->gVars.p.useP2P == UMAC_P2P_ENABLED) {
				/*Remove the 11n rates from OurAllSupportedRates_Abgn and OurAllBasicSupportedRates_Abgn*/
				pUmacInstance->OurAllSupportedRates_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED ;
				pUmacInstance->OurAllBasicSupportedRates_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED;
				pUmacInstance->AllSupportedRates_WithAP_Abgn = WFM_ERP_OFDM_RATES_SUPPORTED;
			} else {
				pUmacInstance->OurAllSupportedRates_Abgn = UMAC_SUPPORTED_RATES_OURS_DEFAULT;
				pUmacInstance->OurAllBasicSupportedRates_Abgn = UMAC_BASIC_SUPPORTED_RATES_OURS_DEFAULT;
			}

			break;


		case UMAC_DEVICE_OID_802_11_ALLOW_ACCESS:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "SET_OID: UMAC_DEVICE_OID_802_11_ALLOW_ACCESS\n");
			pUmacInstance->gVars.p.userallowaccess = set_param_req->ParameterData.usrAllowAccess.allow_access;
			break;


		case UMAC_DEVICE_OID_802_11_MAC_ADDR_UPDATE:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, "SET_OID: UMAC_DEVICE_OID_802_11_MAC_ADDR_UPDATE\n");
			LmacStatus = UMAC_AddDeleteUserAllowedPeer(UmacHandle, &set_param_req->ParameterData.databasereq);
			break;

		case UMAC_DEVICE_OID_802_11_DEVICE_DISCOVERY:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, ("SET_OID: UMAC_DEVICE_OID_802_11_DEVICE_DISCOVERY\n"));

			OS_MemoryCopy(&pUmacInstance->p2pDevDiscvry,
				(UMAC_OID_P2P_DEVICE_DISCOVERY *) &set_param_req->ParameterData.p2pDevDiscvry,
				sizeof(UMAC_OID_P2P_DEVICE_DISCOVERY)
				);

			if (set_param_req->ParameterData.p2pDevDiscvry.bssListScan.Flags)
				umac_event = UMAC_START_DEVICE_DISCOVERY;
			else {
				if (pUmacInstance->UmacScanState == UMAC_SCANNING)
					umac_event = UMAC_STOP_SCAN;
				else if (pUmacInstance->UmacFindState == UMAC_FINDING)
					umac_event = UMAC_STOP_FIND;
			}

			break;

		case UMAC_DEVICE_OID_802_11_GROUP_FORMATION:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, ("SET_OID: UMAC_DEVICE_OID_802_11_GROUP_FORMATION\n"));

			OS_MemoryCopy(&pUmacInstance->p2pGrpFormtn,
				(UMAC_OID_P2P_GROUP_FORMATION *) &set_param_req->ParameterData.dot11StartAp,
				sizeof(UMAC_OID_P2P_GROUP_FORMATION)
				);
			pUmacInstance->operatingMode = OPERATING_MODE_P2P_DEVICE;
			/*umac_event = UMAC_START_GO_NEGO; */

			break;

		case UMAC_DEVICE_OID_802_11_INVITATION:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, ("SET_OID: UMAC_DEVICE_OID_802_11_INVITATION\n"));

			OS_MemoryCopy(&pUmacInstance->p2pInvtn,
				(UMAC_OID_P2P_INVITATION *) &set_param_req->ParameterData.dot11StartAp,
				sizeof(UMAC_OID_P2P_INVITATION)
				);

			break;

		case UMAC_DEVICE_OID_802_11_CONFIGURE_WMM_PARAMS:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, ("SET_OID: UMAC_DEVICE_OID_802_11_CONFIGURE_WMM_PARAMS\n"));
			pUmacInstance->dot11BssConf.vdrSpecificCapInfo |= WFM_BSS_INFO_FLAGS_BSS_WMM_CAP;
			pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION;
			OS_MemoryCopy(&pUmacInstance->gVars.WmmParamElement.QoSInfo,
				(UMAC_OID_802_11_CONFIG_WMM_PARAMS *) &set_param_req->ParameterData.dot11ConfigWMMparams.QoSInfo,
				sizeof(WFM_WMM_AC_PARAM_PACKED) * WFM_NUM_ACS + 2
				);

			break;

		case UMAC_DEVICE_OID_802_11_START_GROUP_FORMATION:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, ("SET_OID: UMAC_DEVICE_OID_802_11_START_GROUP_FORMATION\n"));

			OS_MemoryCopy(&pUmacInstance->p2pStartGrpFormatn,
				(UMAC_OID_P2P_START_GROUP_FORMATION *) &set_param_req->ParameterData.p2pStartGrpFormatn,
				sizeof(UMAC_OID_P2P_START_GROUP_FORMATION)
				);

			UMAC_SET_STATE(UmacHandle, UMAC_GROUP_FORMING);
			umac_event = UMAC_START_FIND;
			break;

		case UMAC_DEVICE_OID_802_11_SOCIAL_CHANNELS:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID, ("SET_OID: UMAC_DEVICE_OID_802_11_SOCIAL_CHANNELS\n"));
			OS_MemoryCopy(&pUmacInstance->socialChannels,
				(UMAC_OID_P2P_SOCIAL_CHANNEL *) &set_param_req->ParameterData.socialChannels,
				sizeof(UMAC_OID_P2P_SOCIAL_CHANNEL)
				);

			break;

#endif	/*P2P_MINIAP_SUPPORT */

		case WFM_DEVICE_OID_802_11_SET_UAPSD:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_UAPSD\n");
				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case WFM_DEVICE_OID_802_11_MAX_RX_AMSDU_SIZE:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_MAX_RX_AMSDU_SIZE\n");

				pUmacInstance->gVars.p.maxRxAmsduSize = set_param_req->ParameterData.maxRxAmsduSize;

				break;
			}
#if FT_SUPPORT
		case WFM_DEVICE_OID_802_11_MDIE:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_MDIE\n");
				if (!OS_MemoryEqual(&pUmacInstance->gVars.p.mdie[2], 2, &set_param_req->ParameterData.mdie.mdie[2], 2)) {
					pUmacInstance->FtFlags &= ~UMAC_FT_FLAG_INIT_MD_ASSOC_DONE;
					OS_MemoryReset(&pUmacInstance->FtTargetBss, sizeof(pUmacInstance->FtTargetBss));
					OS_MemoryReset(&pUmacInstance->gVars.p.FtAuthenticatedAPs[0], MAX_FT_AP * sizeof(pUmacInstance->gVars.p.FtAuthenticatedAPs[0]));
				}

				OS_MemoryCopy(&pUmacInstance->gVars.p.mdie[0], &set_param_req->ParameterData.mdie.mdie[0], 5);
				break;
			}
		case WFM_DEVICE_OID_802_11_FTIE:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_FTIE\n");

				pUmacInstance->gVars.ftie_len = set_param_req->ParameterData.ftie.ftie_len;
				OS_MemoryCopy(&pUmacInstance->gVars.ftie[0], &set_param_req->ParameterData.ftie.ftie[0], set_param_req->ParameterData.ftie.ftie_len);

				break;
			}

		case WFM_DEVICE_OID_802_11_FT_AUTHENTICATE:
			{
				uint8 *pMdid;
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_ASSOC | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_BSSID\n");

				if (set_param_req->ParameterLength) {
					umac_event = UMAC_START_FT_AUTH;
					pMsg = (void *)&set_param_req->ParameterData.BssInfo;
				} /* if( set_param_req->ParameterLength ) */
				else
					LmacStatus = WFM_STATUS_BAD_PARAM;
				if (pUmacInstance->FtFlags != UMAC_FT_FLAG_INIT_MD_ASSOC_DONE)
					LmacStatus = WFM_STATUS_BAD_PARAM;
				pMdid = &set_param_req->ParameterData.BssInfo.MobilityDomainElement.mdid[0];
				if ((pMdid[0] != pUmacInstance->gVars.p.mdie[2])
				    || (pMdid[1] != pUmacInstance->gVars.p.mdie[3])) {
					LmacStatus = WFM_STATUS_BAD_PARAM;
				}
				break;
			}
		case WFM_DEVICE_OID_802_11_DELETE_FT_AUTHENTICATION:
			{
				uint8 *pBssid;
				int i;
				uint8 BROADCAST_ADDR[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
				pBssid = &set_param_req->ParameterData.BssidOid.Bssid[0];
				if (D0_ADDR_EQ(pBssid, BROADCAST_ADDR)) {
					for (i = 0; i < MAX_FT_AP; i++)
						OS_MemoryReset(&pUmacInstance->gVars.p.FtAuthenticatedAPs[i], sizeof(pUmacInstance->gVars.p.FtAuthenticatedAPs[0]));
				} else {
					for (i = 0; i < MAX_FT_AP; i++) {
						if (D0_ADDR_EQ(pBssid, &pUmacInstance->gVars.p.FtAuthenticatedAPs[i].Bssid[0])) {
							OS_MemoryReset(&pUmacInstance->gVars.p.FtAuthenticatedAPs[i], sizeof(pUmacInstance->gVars.p.FtAuthenticatedAPs[0]));
							break;
						}
					}
				}
				LmacStatus = WFM_STATUS_SUCCESS;
				break;
			}
#endif	/* FT_SUPPORT */
		case WFM_DEVICE_OID_802_11_TXOP_LIMIT:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_TXOP_LIMIT\n");

				pUmacInstance->gVars.p.txopLimit = set_param_req->ParameterData.txopLimit.txop;

				break;
			}

		case WFM_DEVICE_OID_802_11_SET_LISTEN_INTERVAL:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_LISTEN_INTERVAL\n");

				if (set_param_req->ParameterData.listenInterval.dtimSkippingCount > 1)
					pUmacInstance->dtimSkippingCount = set_param_req->ParameterData.listenInterval.dtimSkippingCount;
				else
					pUmacInstance->dtimSkippingCount = 1;

				if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
					&& (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {
						/*If UMAC is Associated then check whether BSS Param set in firmware */
						if (pUmacInstance->IsFirmwareBssParamSet == TRUE)
							umac_event = UMAC_EXT_REQ_TO_WSM;
						else
							LmacStatus = WFM_STATUS_PENDING;
				}
				break;
			}

		case WFM_DEVICE_OID_802_11_DISABLE_BG_SCAN:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_DISABLE_BG_SCAN\n");

				pUmacInstance->gVars.p.InfoHost.disableBGScan = set_param_req->ParameterData.disableBGScan;

				break;
			}

		case WFM_DEVICE_OID_802_11_BLACK_LIST_ADDR:
			{
				uint32 status;

				status = UMAC_ManageBlackListPeer(UmacHandle, &set_param_req->ParameterData.BlackListAddr[0]
				    );

				if (status == UMAC_BLKLST_STATUS_ADDED) {
					if (OS_MemoryEqual(&set_param_req->ParameterData.BlackListAddr[0], 6, &pUmacInstance->RemoteMacAdd[0], 6)
					    ) {
						/*If STA is connected with this peer, DISCONNECT */
						umac_event = UMAC_DE_ASSOC;
					}

				} else if (status == UMAC_BLKLST_STATUS_ERROR) {
					LOG_EVENT(DBG_WFM_ERROR, "SET_OID: WFM_DEVICE_OID_802_11_BLACK_LIST_ADDR\n");
				}

				break;
			}

		case WFM_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE\n");

				LOG_DATA(DBG_WFM_CURRENT_GM, "OprPwrMode = %d\n", set_param_req->ParameterData.OprPwrMode);

				if (set_param_req->ParameterData.OprPwrMode <= WFM_OPR_PWR_MODE_QUIESCENT)
					umac_event = UMAC_EXT_REQ_TO_WSM;
				else {
					LOG_EVENT(DBG_WFM_ERROR, "Bad parameter for set for Operational Power Mode, over riding it with default value\n");
					LmacStatus = WFM_STATUS_BAD_PARAM;
				}
				break;
			}
		case WFM_DEVICE_OID_802_11_SET_HOST_MSG_TYPE_FILTER:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_HOST_MSG_TYPE_FILTER\n");

				LOG_DATA(DBG_WFM_CURRENT_GM, "hostMsgTypeFilter = %d\n", set_param_req->ParameterData.hostMsgTypeFilter);

				pUmacInstance->gVars.p.hostMsgTypeFilter = set_param_req->ParameterData.hostMsgTypeFilter;

				break;
			}
		case WFM_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD\n");
				pUmacInstance->gVars.p.RcpiRssiThresholdRcvd = 1;
				OS_MemoryCopy(&pUmacInstance->gVars.p.RcpiRssiThreshold, &set_param_req->ParameterData.RcpiRssiThreshold, sizeof(WFM_RCPI_RSSI_THRESHOLD));
				umac_event = UMAC_EXT_REQ_TO_WSM;
				break;
			}
		case WFM_DEVICE_OID_802_11_SET_PMKID_BKID:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_PMKID_BKID\n");
				pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfoCount = set_param_req->ParameterData.PmkidBkidInfo.bssidInfoCount;
				if (!set_param_req->ParameterData.PmkidBkidInfo.bssidInfoCount) {
					pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfoCount = 0;
					OS_MemoryReset(&pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfo[0], (WFM_MAX_BSSID_INFO_ENTRIES * sizeof(WFM_BSSID_INFO)));
				} else {
					OS_MemoryCopy(&pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfo[0], &set_param_req->ParameterData.PmkidBkidInfo.bssidInfo[0], (pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfoCount * sizeof(WFM_BSSID_INFO)));
				}
				break;
			}
		case WFM_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT\n");

				OS_MemoryCopy(&pUmacInstance->gVars.p.BeaconLostCount.BeaconLostCount, &set_param_req->ParameterData.BeaconLostCount.BeaconLostCount, sizeof(uint8));
				/*If current state is associated we need to again to WSM firmware */
				if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
											     && (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED))) {
					umac_event = UMAC_EXT_REQ_TO_WSM;
				}
				break;
			}
		case WFM_DEVICE_OID_802_11_TX_FAILURE_THRESHOLD_COUNT:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_TX_FAILURE_THRESHOLD_COUNT\n");
				OS_MemoryCopy(&pUmacInstance->gVars.p.TxFailureCount.TransmitFailureThresholdCount, &set_param_req->ParameterData.TxFailureCount.TransmitFailureThresholdCount, sizeof(WFM_TX_FAILURE_THRESHOLD_COUNT));
				break;
			}
		case WFM_DEVICE_OID_802_11_SET_ETHER_TYPE_FILTER:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_ETHER_TYPE_FILTER\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case WFM_DEVICE_OID_802_11_SET_UDP_PORT_FILTER:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_UDP_PORT_FILTER\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case WFM_DEVICE_OID_802_11_SET_MAGIC_FRAME_FILTER:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_MAGIC_FRAME_FILTER\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case WFM_DEVICE_OID_802_11_ENABLE_11D_FEATURE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_ENABLE_11D_FEATURE\n");
				OS_MemoryCopy(&pUmacInstance->gVars.p.Enable11dFeature, &set_param_req->ParameterData.Enable11dFeature, sizeof(uint8));
				break;
			}
		case WFM_DEVICE_OID_802_11_UPDATE_VENDOR_IE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_UPDATE_VENDOR_IE\n");
				if (set_param_req->ParameterData.vendorIe.probeReqIElength) {
					if ((pUmacInstance->VendorIe.pProbeReqIE) &&
					(pUmacInstance->VendorIe.probeReqIElength)) {
						OS_Free(pUmacInstance->VendorIe.pProbeReqIE);
						pUmacInstance->VendorIe.pProbeReqIE = NULL;
						pUmacInstance->VendorIe.probeReqIElength = 0;

					}

					pUmacInstance->VendorIe.probeReqIElength = set_param_req->ParameterData.vendorIe.probeReqIElength;
					pUmacInstance->VendorIe.pProbeReqIE = (uint8 *) OS_Allocate(pUmacInstance->VendorIe.probeReqIElength);
					if (NULL == pUmacInstance->VendorIe.pProbeReqIE) {
						LOG_EVENT(DBG_WFM_ERROR | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "ERROR: Failed to allocate memory for pProbeReqIE");
						LmacStatus = WFM_STATUS_OUT_OF_RESOURCES;
						break;
					}
					OS_MemoryCopy(
						(uint8 *) pUmacInstance->VendorIe.pProbeReqIE,
						(uint8 *) (set_param_req->ParameterData.vendorIe.pProbeReqIE),
						pUmacInstance->VendorIe.probeReqIElength
						);
				}
				if (set_param_req->ParameterData.vendorIe.probeRespIElength) {
					if ((pUmacInstance->VendorIe.pProbeRespIE) &&
					(pUmacInstance->VendorIe.probeRespIElength)) {
						OS_Free(pUmacInstance->VendorIe.pProbeRespIE);
						pUmacInstance->VendorIe.pProbeRespIE = NULL;
						pUmacInstance->VendorIe.probeRespIElength = 0;
					}

					pUmacInstance->VendorIe.probeRespIElength = set_param_req->ParameterData.vendorIe.probeRespIElength;
					pUmacInstance->VendorIe.pProbeRespIE = (uint8 *) OS_Allocate(pUmacInstance->VendorIe.probeRespIElength);
					if (NULL == pUmacInstance->VendorIe.pProbeRespIE) {
						LOG_EVENT(DBG_WFM_ERROR | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "ERROR: Failed to allocate memory for pProbeRespIE");
						LmacStatus = WFM_STATUS_OUT_OF_RESOURCES;
						break;
					}
					OS_MemoryCopy(
						(uint8 *) pUmacInstance->VendorIe.pProbeRespIE,
						(uint8 *) (set_param_req->ParameterData.vendorIe.pProbeRespIE),
						pUmacInstance->VendorIe.probeRespIElength
						);
				}
				if (set_param_req->ParameterData.vendorIe.beaconIElength) {
					if ((pUmacInstance->VendorIe.pBeaconIE) &&
					(pUmacInstance->VendorIe.beaconIElength)) {
						OS_Free(pUmacInstance->VendorIe.pBeaconIE);
						pUmacInstance->VendorIe.pBeaconIE = NULL;
						pUmacInstance->VendorIe.beaconIElength = 0;
					}

					pUmacInstance->VendorIe.beaconIElength = set_param_req->ParameterData.vendorIe.beaconIElength;
					pUmacInstance->VendorIe.pBeaconIE = (uint8 *) OS_Allocate(pUmacInstance->VendorIe.beaconIElength);
					if (NULL == pUmacInstance->VendorIe.pBeaconIE) {
						LOG_EVENT(DBG_WFM_ERROR | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "ERROR: Failed to allocate memory for pBeaconIE");
						LmacStatus = WFM_STATUS_OUT_OF_RESOURCES;
						break;
					}
					OS_MemoryCopy(
						(uint8 *) pUmacInstance->VendorIe.pBeaconIE,
						(uint8 *) (set_param_req->ParameterData.vendorIe.pBeaconIE),
						pUmacInstance->VendorIe.beaconIElength
						);
				}
				if (set_param_req->ParameterData.vendorIe.assocReqIElength) {
					if ((pUmacInstance->VendorIe.pAssocReqIE) &&
					(pUmacInstance->VendorIe.assocReqIElength)) {
						OS_Free(pUmacInstance->VendorIe.pAssocReqIE);
						pUmacInstance->VendorIe.pAssocReqIE = NULL;
						pUmacInstance->VendorIe.assocReqIElength = 0;

					}

					pUmacInstance->VendorIe.assocReqIElength = set_param_req->ParameterData.vendorIe.assocReqIElength;
					pUmacInstance->VendorIe.pAssocReqIE = (uint8 *) OS_Allocate(pUmacInstance->VendorIe.assocReqIElength);
					if (NULL == pUmacInstance->VendorIe.pAssocReqIE) {
						LOG_EVENT(DBG_WFM_ERROR | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "ERROR: Failed to allocate memory for pAssocReqIE");
						LmacStatus = WFM_STATUS_OUT_OF_RESOURCES;
						break;
					}
					OS_MemoryCopy(
						(uint8 *) pUmacInstance->VendorIe.pAssocReqIE,
						(uint8 *) (set_param_req->ParameterData.vendorIe.pAssocReqIE),
						pUmacInstance->VendorIe.assocReqIElength
						);
				}
				if (set_param_req->ParameterData.vendorIe.assocRespIElength) {
					if ((pUmacInstance->VendorIe.pAssocRespIE) &&
					(pUmacInstance->VendorIe.assocRespIElength)) {
						OS_Free(pUmacInstance->VendorIe.pAssocRespIE);
						pUmacInstance->VendorIe.pAssocRespIE = NULL;
						pUmacInstance->VendorIe.assocRespIElength = 0;
					}

					pUmacInstance->VendorIe.assocRespIElength = set_param_req->ParameterData.vendorIe.assocRespIElength;
					pUmacInstance->VendorIe.pAssocRespIE = (uint8 *) OS_Allocate(pUmacInstance->VendorIe.assocRespIElength);
					if (NULL == pUmacInstance->VendorIe.pAssocRespIE) {
						LOG_EVENT(DBG_WFM_ERROR | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "ERROR: Failed to allocate memory for pAssocRespIE");
						LmacStatus = WFM_STATUS_OUT_OF_RESOURCES;
						break;
					}
					OS_MemoryCopy(
						(uint8 *) pUmacInstance->VendorIe.pAssocRespIE,
						(uint8 *) (set_param_req->ParameterData.vendorIe.pAssocRespIE),
						pUmacInstance->VendorIe.assocRespIElength
						);
				}

				umac_event = UMAC_EXT_REQ_TO_WSM;
				break;
			}

		case WFM_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case UMAC_DEVICE_OID_802_11_SET_ARP_KEEP_ALIVE_PERIOD:
			{

				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMAC_DEVICE_OID_802_11_SET_ARP_KEEP_ALIVE_PERIOD\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case WFM_DEVICE_OID_802_11_PTA_LISTEN_INTERVAL:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_PTA_LISTEN_INTERVAL\n");

				pUmacInstance->gVars.p.PtaListenInterval.PtaListenInterval = set_param_req->ParameterData.PtaListenInterval.PtaListenInterval;

				break;
			}
#if DOT11K_SUPPORT
		case WFM_DEVICE_OID_802_11_MEASUREMENT_CAPABILITY:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_MEASUREMENT_CAPABILITY\n");
				pUmacInstance->gVars.p.measurementCapability = set_param_req->ParameterData.measurementCapability;
				if (pUmacInstance->gVars.p.measurementCapability)
					pUmacInstance->gVars.p.InfoHost.MeasurementCapSupported = TRUE;
				break;
			}
		case WFM_DEVICE_OID_802_11_ENABLE_MEASUREMENTS:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_ENABLE_MEASUREMENTS\n");
				pUmacInstance->gVars.p.enableMeasurements = set_param_req->ParameterData.enableMeasurements;
				break;
			}
		case WFM_DEVICE_OID_802_11_NEIGHBOR_REPORT:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_NEIGHBOR_REPORT\n");
				if ((pUmacInstance->gVars.p.measurementCapability & (1 << 2)) && (pUmacInstance->gVars.p.enableMeasurements & (1 << 2)) && (pUmacInstance->gVars.p.InfoHost.MeasurementCapSupported == TRUE)) {
					pMsg = (void *)&set_param_req->ParameterData.NeighborReportSsid;
					umac_event = UMAC_SEND_NEIGHBOR_REPORT_REQ;
				} else
					LmacStatus = WFM_STATUS_REQ_REJECTED;
				break;
			}
		case WFM_DEVICE_OID_802_11_LCI_INFORMATION:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_LCI_INFORMATION\n");
				OS_MemoryCopy(&pUmacInstance->gVars.p.LciInfo, &set_param_req->ParameterData.lciInfo, sizeof(UMAC_802_11_LCI_INFORMATION));
				pUmacInstance->gVars.p.lciInfoAvl = 1;
				break;
			}
#endif	/* DOT11K_SUPPORT */

		case WFM_DEVICE_OID_802_11_CONFIGURE_REGULATORY_DOMAIN:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_CONFIGURE_REGULATORY_DOMAIN\n");
				LmacStatus = WFM_UMAC_UpdateRegulatoryDomain(UmacHandle, &set_param_req->ParameterData.regDomainTable, set_param_req->ParameterLength);

				break;
			}

#if MGMT_FRAME_PROTECTION
		case WFM_DEVICE_OID_802_11_PROTECTED_MGMT_POLICY:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_PROTECTED_MGMT_POLICY\n");
				pUmacInstance->hostMfpPolicy = set_param_req->ParameterData.procMgmtFrmPolicy;
				break;
			}
		case WFM_DEVICE_OID_802_11_SA_QUERY_INTERVAL:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_SA_QUERY_INTERVAL\n");
				OS_MemoryCopy(&pUmacInstance->saQueryInterval, &set_param_req->ParameterData.saQueryInterval, sizeof(UMAC_SA_QUERY_INTERVAL)
				    );
				break;
			}
#endif	/*MGMT_FRAME_PROTECTION */

		case WFM_DEVICE_OID_802_11_MULTI_TX_CONFIRMATION:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: WFM_DEVICE_OID_802_11_MULTI_TX_CONFIRMATION\n");

				pUmacInstance->gVars.p.UseMultiTxCnfMsg = set_param_req->ParameterData.useMultiTxConfMsg;
				break;
			}
		case UMAC_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMAC_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case UMAC_DEVICE_OID_802_11_KEEP_ALIVE_PERIOD:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMAC_DEVICE_OID_802_11_KEEP_ALIVE_PERIOD\n");

				OS_MemoryCopy(&pUmacInstance->gVars.p.keepAlivePeriod, &set_param_req->ParameterData.keepAlivePeriod, sizeof(UMAC_KEEP_ALIVE_PERIOD));
				break;
			}
		case UMAC_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMAC_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID\n");

				umac_event = UMAC_EXT_REQ_TO_WSM;

				break;
			}
		case UMAC_DEVICE_OID_802_11_DISABLE_ACTIVE_JOIN:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMAC_DEVICE_OID_802_11_DISABLE_ACTIVE_JOIN\n");

				OS_MemoryCopy(&pUmacInstance->gVars.p.DisableActiveJoin, &set_param_req->ParameterData.DisableActiveJoin, sizeof(pUmacInstance->gVars.p.DisableActiveJoin));
				break;
			}
		case UMAC_DEVICE_OID_802_11_INTRA_BSS_BRIDGING:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMAC_DEVICE_OID_802_11_DISABLE_ACTIVE_JOIN\n");

				OS_MemoryCopy(&pUmacInstance->gVars.p.IntraBssBridging, &set_param_req->ParameterData.IntraBssBridging, sizeof(pUmacInstance->gVars.p.IntraBssBridging));
				break;
			}
		case UMAC_DEVICE_OID_802_11_STOP_AP:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMAC_DEVICE_OID_802_11_STOP_AP\n");
				umac_event = UMAC_STOP_AP;
				/* pUmacInstance->gVars.p.stopAPFlag = 1; */
				pUmacInstance->gVars.p.sendStopAPEvt = 1;
				break;
			}
		case UMAC_DEVICE_OID_802_11_P2P_PS_MODE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMAC_DEVICE_OID_802_11_P2P_PS_MODE\n");
				if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) ||
					((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
				&& (pUmacInstance->UmacPreviousState == UMAC_ASSOCIATED)) ||

				(pUmacInstance->UmacCurrentState == UMAC_BSS_STARTED) ||
				((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
				&& (pUmacInstance->UmacPreviousState == UMAC_BSS_STARTED)) ||

				(pUmacInstance->UmacCurrentState == UMAC_GROUP_FORMED) ||
				((pUmacInstance->UmacCurrentState == UMAC_PROCESSING)
				&& (pUmacInstance->UmacPreviousState == UMAC_GROUP_FORMED))
				) {
					umac_event = UMAC_EXT_REQ_TO_WSM;
				} else
					LmacStatus = WFM_STATUS_FAILURE;
				break;
			}
		case UMAC_DEVICE_OID_802_11_CHANGE_CHANNEL:
			LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMAC_DEVICE_OID_802_11_CHANGE_CHANNEL\n");
			umac_event = UMAC_EXT_REQ_TO_WSM;
#if 0
			if ((pUmacInstance->UmacCurrentState == UMAC_INITIALIZED)
			    || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING) && (pUmacInstance->UmacPreviousState == UMAC_INITIALIZED))) {
				umac_event = UMAC_EXT_REQ_TO_WSM;
			} else if ((pUmacInstance->UmacCurrentState == UMAC_GROUP_FORMED)
			    || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING) && (pUmacInstance->UmacPreviousState == UMAC_GROUP_FORMED))) {
				if (set_param_req->ParameterData.ChangeChannel.Channel != pUmacInstance->Channelnumber)
					LmacStatus = WFM_STATUS_REQ_REJECTED;
			} else {
				LmacStatus = WFM_STATUS_REQ_REJECTED;
			}
#endif

			break;
		case UMAC_DEVICE_OID_802_11_RESTORE_CHANNEL:
#if 0
			if ((pUmacInstance->UmacCurrentState == UMAC_INITIALIZED)
			    || ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING) && (pUmacInstance->UmacPreviousState == UMAC_INITIALIZED))) {
				if (pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]) {

					/*There are outstanding Tx buffer, so Scheduling
					  restore channel request for future */
					OS_START_TIMER(
						pUmacInstance->gVars.p.UmacTimers.pCancelRemainOnChTimer,
						WFM_UMAC_CANCEL_REMAIN_ON_CH_TIMEOUT,
						UmacHandle
						);
					LmacStatus = WFM_STATUS_PENDING;
				} else
					umac_event = UMAC_EXT_REQ_TO_WSM;
			} else
				LmacStatus = WFM_STATUS_REQ_REJECTED;
#endif
			umac_event = UMAC_EXT_REQ_TO_WSM;

			break;
		case UMAC_DEVICE_OID_802_11_REGISTER_MGMT_FRAME_RX:
			break;
		case UMAC_DEVICE_OID_802_11_SET_NUM_CLIENTS:
			if ((pUmacInstance->UmacCurrentState == UMAC_INITIALIZED)
				|| ((pUmacInstance->UmacCurrentState == UMAC_PROCESSING) && (pUmacInstance->UmacPreviousState == UMAC_INITIALIZED))) {
					set_param_req->ParameterData.NumOfClients.NumOfClients++;
					if (set_param_req->ParameterData.NumOfClients.NumOfClients == 1)
						LmacStatus = WFM_STATUS_BAD_PARAM;
					else if (set_param_req->ParameterData.NumOfClients.NumOfClients <= pUmacInstance->gVars.p.FirmwareClientSupport)
						pUmacInstance->gVars.p.maxNumClientSupport = set_param_req->ParameterData.NumOfClients.NumOfClients;
					else
						LmacStatus = WFM_STATUS_BAD_PARAM;
			} else
				LmacStatus = WFM_STATUS_REQ_REJECTED;
			break;
		case UMI_DEVICE_OID_802_11_SET_HIDDEN_AP_MODE:
			{
				LOG_EVENT(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: UMI_DEVICE_OID_802_11_SET_HIDDEN_AP_MODE\n");
				OS_MemoryCopy(&pUmacInstance->gVars.p.HiddenAPFlag, &set_param_req->ParameterData.HiddenAPMode.flag, sizeof(pUmacInstance->gVars.p.HiddenAPFlag));
				break;
			}
		default:
			{
				LOG_DATA(DBG_WFM_HI | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "SET_OID: Bad OID - 0x%x\n", set_param_req->ParameterId);
				LmacStatus = WFM_STATUS_BAD_PARAM;
				break;
			}
		} /* end switch(set_param_req->ParameterId) */

	} else {
		LOG_EVENT(DBG_WFM_ERROR | DBG_WFM_OID | DBG_WFM_CURRENT_GM, "ERROR: SET_OID with HiMessageLength 0");

		LmacStatus = WFM_STATUS_BAD_PARAM;
		goto exit_handler;
	} /*if(hi_msg->MessageLength) */

	if ((umac_event != UMAC_MAX_EVT))
		LmacStatus = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, umac_event, pMsg);

	if (WFM_STATUS_PENDING == LmacStatus) {
		/*bit 15 is used to indicate pening HI requests as b8 is used
		for linkId */
		hi_msg->MessageId = (hi_msg->MessageId | 0x1000);

		UMAC_QUEUE_PENDING_HI_MSG(UmacHandle, umac_event, hi_msg);
		goto exit_handler;
	} /*if( WFM_STATUS_PENDING == LmacStatus ) */

	if ((WFM_STATUS_PENDING != LmacStatus) && (hi_msg->MessageId & 0x1000)) {
		set_param_cnf->Status = (uint16) LmacStatus;
		hi_msg->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + sizeof(WFM_SET_PARAM_CNF);
		hi_msg->MessageId = WFM_SET_PARAM_CNF_ID;

		/*Sending Set Param Cnf to Host */
		pUmacInstance->gVars.p.UpperLayerCb(
						pUmacInstance->gVars.p.ulHandle,
						hi_msg
						);
	}

 exit_handler:
	return (uint16) LmacStatus;

} /*end WFM_HI_Proc_SetParamReq() */

/******************************************************************************
 * NAME:	WFM_HI_DefaultMsgHandler
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the default request.
 * \param UmacHandle   - Handle to the UMAC Instance.
 * \param *hi_msg      - Message from host
 * \returns uint16       Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_DefaultMsgHandler(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_EVENT(DBG_WFM_HI, "WFM_HI_DefaultMsgHandler()\n");

	hi_msg->MessageId = hi_msg->MessageId | WFM_CNF_BASE;

	/*Sending Cnf to Host */
	pUmacInstance->gVars.p.UpperLayerCb(
						pUmacInstance->gVars.p.ulHandle,
						hi_msg
						);

	return WFM_STATUS_FAILURE;

} /*end WFM_HI_DefaultMsgHandler() */

/******************************************************************************
 * NAME:	WFM_HI_Proc_MemoryReadReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Memory Read Request.
 * \param UmacHandle - Handle to the UMAC Instance.
 * \param *hi_msg    - Pointer to Hi Message.
 * \returns uint16     Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_MemoryReadReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg)
{
	uint32 Status;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_EVENT(DBG_WFM_HI, "WFM_HI_Proc_MemoryReadReq()\n");

	OS_ASSERT(UmacHandle);

	OS_ASSERT(hi_msg);

	Status = WFM_UMAC_EXEC_STATE_MACHINE(
						UmacHandle,
						UMAC_EXT_REQ_TO_WSM,
						hi_msg
						);

	if (WFM_STATUS_PENDING == Status) {
		UMAC_QUEUE_PENDING_HI_MSG(UmacHandle, UMAC_DATA_TX, hi_msg);
		goto exit_handler;
	} /*if( WFM_STATUS_PENDING == Status ) */

	if (WFM_STATUS_PENDING != Status) {
		hi_msg->MessageId = WFM_MEMORY_READ_CNF_ID;
		hi_msg->MessageLength = sizeof(WFM_MEM_READ_CNF);
		pUmacInstance->gVars.p.UpperLayerCb(
						pUmacInstance->gVars.p.ulHandle,
						(WFM_HI_MESSAGE *) hi_msg
						);
	} /*if( WFM_STATUS_SUCCESS != Status ) */

 exit_handler:
	return (uint16) Status;

} /*end WFM_HI_Proc_MemoryReadReq() */

/******************************************************************************
 * NAME:	WFM_HI_Proc_MemoryWriteReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Memory Write Request.
 * \param UmacHandle - Handle to the UMAC Instance.
 * \param *hi_msg    - Pointer to Hi Message.
 * \returns uint16     Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_MemoryWriteReq(UMAC_HANDLE UmacHandle,
				  WFM_HI_MESSAGE *hi_msg)
{
	uint32 Status;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_EVENT(DBG_WFM_HI, "WFM_HI_Proc_MemoryWriteReq()\n");

	OS_ASSERT(UmacHandle);

	OS_ASSERT(hi_msg);

	Status = WFM_UMAC_EXEC_STATE_MACHINE(
						UmacHandle,
						UMAC_EXT_REQ_TO_WSM,
						hi_msg
						);

	if (WFM_STATUS_PENDING == Status) {
		UMAC_QUEUE_PENDING_HI_MSG(UmacHandle, UMAC_DATA_TX, hi_msg);
		goto exit_handler;
	} /*if( WFM_STATUS_PENDING == Status ) */

	if (WFM_STATUS_PENDING != Status) {
		hi_msg->MessageId = WFM_MEMORY_WRITE_CNF_ID;
		hi_msg->MessageLength = sizeof(WFM_MEM_WRITE_CNF);
		pUmacInstance->gVars.p.UpperLayerCb(
						pUmacInstance->gVars.p.ulHandle,
						(WFM_HI_MESSAGE *) hi_msg
						);
	} /*if( WFM_STATUS_SUCCESS != Status ) */

 exit_handler:
	return (uint16) Status;
} /*end WFM_HI_Proc_MemoryWriteReq() */

/******************************************************************************
 * NAME:	WFM_HI_Proc_ConfigReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Configuration Request.
 * \param UmacHandle - Handle to the UMAC Instance.
 * \param *hi_msg    - Pointer to Hi Message.
 * \returns uint16     Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_ConfigReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg)
{
	WFM_CONFIG_CNF *pConfigCnf = NULL;
	uint32 Status;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	CHECK_POINTER_IS_VALID(hi_msg);

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_HI, "WFM_HI_Proc_ConfigReq()\n");

	pConfigCnf = (WFM_CONFIG_CNF *) &hi_msg->PayLoad.wfm_config_cnf;

	Status = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_EXT_REQ_TO_WSM, hi_msg);

	if (WFM_STATUS_PENDING == Status) {
		UMAC_QUEUE_PENDING_HI_MSG(UmacHandle, UMAC_EXT_REQ_TO_WSM, hi_msg);
		goto exit_handler;
	} /*if( WFM_STATUS_PENDING == Status ) */

	if ((WFM_STATUS_PENDING != Status)) {
		hi_msg->MessageLength = WFM_HI_MESSAGE_HDR_SIZE + sizeof(WFM_CONFIG_CNF);
		hi_msg->MessageId = WFM_CONFIG_CNF_ID;

		/*Sending Set Param Cnf to Host */
		pUmacInstance->gVars.p.UpperLayerCb(
						   pUmacInstance->gVars.p.ulHandle,
						   hi_msg
						   );

		if (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED)
			WFM_SCHEDULE_LMAC_TO_UMAC(UmacHandle, UMAC_DE_ASSOC, NULL);

	} /*if( (WFM_STATUS_PENDING != LmacStatus) ) */


 exit_handler:
	return (uint16) Status;

} /*end WFM_HI_Proc_ConfigReq() */

/******************************************************************************
 * NAME:	WFM_HI_Proc_GenericReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Generic Request.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \param  *hi_msg    - Pointer to Hi Message.
 * \returns uint16      Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_GenericReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	LOG_EVENT(DBG_WFM_HI, "WFM_HI_Proc_GenericReq()\n");

	hi_msg->MessageId = hi_msg->MessageId | WFM_CNF_BASE;
	/*Sending Cnf to Host */
	pUmacInstance->gVars.p.UpperLayerCb(
						pUmacInstance->gVars.p.ulHandle,
						hi_msg
						);

	return WFM_STATUS_SUCCESS;
} /*end WFM_HI_Proc_GenericReq() */


/******************************************************************************
 * NAME:	WFM_HI_Proc_TransmitMgmtReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the transmit Managment request.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \param *pMgmtTxReq - Tx Managment Request from upper layer.
 * \returns uint16 Appropriate WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_TransmitMgmtReq(UMAC_HANDLE UmacHandle,
				   UMAC_MGMT_TRANSMIT_REQ *pMgmtTxReq)
{
	uint32 LmacStatus;

	LOG_EVENT(DBG_WFM_HI, "WFM_HI_Proc_TransmitMgmtReq()\n");

	OS_ASSERT(UmacHandle);

	OS_ASSERT(pMgmtTxReq);

	if (pMgmtTxReq->FrameLength)
		LmacStatus = WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, UMAC_TX_MGMT_FRAME, pMgmtTxReq);
	else
		LmacStatus = WFM_STATUS_BAD_PARAM;

	return (uint16) LmacStatus;
} /* end WFM_HI_Proc_TransmitMgmtReq() */
