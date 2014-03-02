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
 * \file umac_frames.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_frames.c
 * \brief
 * This module handles the frame creation and conversion.
 * \ingroup Upper_MAC_Core
 * \date 15/02/08 16:58
 */

/******************************************************************************
	      INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "d0_defs.h"

#include "umac_dev_conf.h"
#include "umac_frames.h"
#include "umac_2_lmac.h"
#include "umac_sm.h"
#include "umac_dev_list.h"
#include "umac_messages.h"
#include "umac_utils.h"
#include "umac_mib.h"
#include "umac_internal.h"

#include "umac_ll_if.h"
#include "umac_if.h"
#include "wsm_api.h"
#include "umac_data_handler.h"

/******************************************************************************
			    LOCAL  FUNCTIONS
*****************************************************************************/
static void WFM_ProcessDeviceCacheInfo(WFM_BSS_CACHE_INFO_IND *wfm_bss_cache_info_ind,
				       WFM_BSS_INFO *pBssElement,
				       IeElement *pIelement);

/******************************************************************************
			    EXTERNAL DATA REFERENCES
*****************************************************************************/

/******************************************************************************
			EXTERNAL FUNCTIONS
******************************************************************************/

/******************************************************************************
 * NAME:	WFM_UMAC_CreateDot11FrameHdr
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is for inserting dot11 header information to the packet.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param Type		- Type of the packet.
 * \param SubType       - Sub type of the packet.
 * \param *dot11Hdr     - Pointer to the buffer where header
 *			  info. needs to be inserted.
 * \param RemoteMacAddr	- MAC Address /BSSID of the remote device.
 * \param *LocalMacAddr	- MAC address / BSSID of the local device.
 * \param EtherType     - Ether Type of frame.
 * \param Encrypt	- Encryption required or not.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_CreateDot11FrameHdr(UMAC_HANDLE UmacHandle,
				  uint8 Type,
				  uint8 SubType,
				  WFM_UMAC_FRAME_HDR *dot11Hdr,
				  uint8 *RemoteMacAddr,
				  uint8 *LocalMacAddr,
				  uint16 EtherType,
				  uint8  Encrypt
				  )
{
	uint16 FCtrl;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	/*
	   UMAC is not supposed to fill the following fields in Frame control
	   1] More fragments
	   2] Retransmission
	   3] Power management
	   4] More data
	 */

	FCtrl = ((Type & 0x0003) << 2);	/*Type */
	FCtrl |= ((SubType & 0x000F) << 4);	/*SubType */

	if (Type != WFM_UMAC_MGMT) {
		if ((WFM_802_INFRASTRUCTURE == pUmacInstance->dot11InfraStructureMode)
#if P2P_MINIAP_SUPPORT
			&& (pUmacInstance->operatingMode <= OPERATING_MODE_P2P_CLIENT)
#endif	/* P2P_MINIAP_SUPPORT */
			)
			FCtrl |= (1 << 8);	/* Setting the ToDS bit */
#if P2P_MINIAP_SUPPORT
		else if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
			FCtrl |= (1 << 9);	/* Setting the FromDS bit */
#endif	/* P2P_MINIAP_SUPPORT */
	} /* if (Type != WFM_UMAC_MGMT ) */

	if (Encrypt)
		FCtrl |= (1 << 14);

	/* cvt little endian to 802.11 frame endianness (little endian) */
	dot11Hdr->FrameControl = umc_cpu_to_le16(FCtrl);

	/*
	   UMAC is not supposed to fill the following fields
	   1] Duration/ID
	   2] Sequence control
	   3] WEP IV/ICV
	   4] TKIP Michael
	   5] FCS
	 */
/*
  -------------------------------------------------------------
  | ToDS | FromDS | Address1 | Address2 | Address3 | Address4 |
  -------------------------------------------------------------
  |  0   |  0     | DA       | SA       | BSSID    | N/A      |
  -------------------------------------------------------------
  |  0   |  1     | DA       | BSSID    | SA       | N/A      |
  -------------------------------------------------------------
  |  1   |  0     | BSSID    | SA       | DA       | N/A      |
  -------------------------------------------------------------
  |  1   |  1     | RA       | TA       | DA       | SA       |
  -------------------------------------------------------------
*/

	/*
	   In our case ToDs = 1 and FromDS = 0
	 */
	if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode) {
		D0_ADDR_COPY(dot11Hdr->Address1, RemoteMacAddr);
		D0_ADDR_COPY(dot11Hdr->Address3, pUmacInstance->RemoteMacAdd);
	} else {
#if P2P_MINIAP_SUPPORT
		if (pUmacInstance->operatingMode >= OPERATING_MODE_AP) {
			D0_ADDR_COPY(
					dot11Hdr->Address1,
					RemoteMacAddr
					);
			D0_ADDR_COPY(
					dot11Hdr->Address3,
					pUmacInstance->RemoteMacAdd
					);
		} else {
#endif	/*P2P_MINIAP_SUPPORT */
			D0_ADDR_COPY(
					dot11Hdr->Address1,
					pUmacInstance->RemoteMacAdd
					);
			D0_ADDR_COPY(
					dot11Hdr->Address3,
					RemoteMacAddr
					);
#if P2P_MINIAP_SUPPORT
		}
#endif	/*P2P_MINIAP_SUPPORT */
	}

D0_ADDR_COPY(dot11Hdr->Address2, pUmacInstance->MacAddress);

} /* end WFM_UMAC_CreateDot11FrameHdr() */

/******************************************************************************
 * NAME:	WFM_UMAC_CreateDot11FrameHdr_MoreGeneral
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is for inserting dot11 header information to the packet.
 * \param *dot11Hdr		- Pointer to dot11 frame header.
 * \param Type			- Type of the packet.
 * \param SubType		- Sub type of the packet.
 * \param ToDs			- Value of ToDs
 * \param EncryptedPacket	- Value 0 for encrypted packet else 1
 * \param *ToDsBssid_Da_A1	- BSSID.
 * \param Sa_A2			- Sa_A2
 * \param *ToDsDa_Bssid_A3	-DA Bssid
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_CreateDot11FrameHdr_MoreGeneral(WFM_UMAC_FRAME_HDR *dot11Hdr,
					      uint8 Type,
					      uint8 SubType,
					      uint8 ToDs,
					      uint8 EncryptedPacket,
					      uint8 *ToDsBssid_Da_A1,
					      uint8 *Sa_A2,
					      uint8 *ToDsDa_Bssid_A3)
{
	/*
	   UMAC is not supposed to fill the following fields in Frame control
	   1] More fragments
	   2] Retransmission
	   3] Power management
	   4] More data
	 */

	/* from DS is always 0 */
	/* retry is 0 */
	/* pwr mgt is 0 */
	/* more data is 0 */
	/* cvt little endian to 802.11 frame endianness (little endian) */
	dot11Hdr->FrameControl = ((Type & 0x3) << 2) | ((SubType & 0xf) << 4) | ((ToDs & 0x1) << 8) | ((EncryptedPacket & 0x1) << 14);

	/*
	   UMAC is not supposed to fill the following fields
	   1] Duration/ID
	   2] Sequence control
	   3] WEP IV/ICV
	   4] TKIP Michael
	   5] FCS
	 */
/*
  -------------------------------------------------------------
  | ToDS | FromDS | Address1 | Address2 | Address3 | Address4 |
  -------------------------------------------------------------
  |  0   |  0     | DA       | SA       | BSSID    | N/A      |
  -------------------------------------------------------------
  |  1   |  0     | BSSID    | SA       | DA       | N/A      |
  -------------------------------------------------------------
*/

	/*
	  fill in address fields only if ptrs are not null,
	  otherwise dont fill it
	*/
	if (ToDsBssid_Da_A1 != NULL)
		D0_ADDR_COPY(dot11Hdr->Address1, ToDsBssid_Da_A1);

	if (Sa_A2 != NULL)
		D0_ADDR_COPY(dot11Hdr->Address2, Sa_A2);

	if (ToDsDa_Bssid_A3 != NULL)
		D0_ADDR_COPY(dot11Hdr->Address3, ToDsDa_Bssid_A3);

} /* end WFM_UMAC_CreateDot11FrameHdr_MoreGeneral() */

/******************************************************************************
 * NAME:	WFM_UMAC_CreateAuthFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an authentication request frame body.
 * \param UmacHandle		- Handle to UMAC Instance.
 * \param *pFrameBody		- a pointer to the buffer where
 *				  authentication frame body
 *			          needs to be constructed.
 * \param AuthAlgo		- Authentication Algorithm to be used.
 * \param TransactionNo		- Authentication Transaction Number.
 * \param pRxAuthFrame		- Received Auth frame.
 * \param status		- Status to be filled in auth frame
 * \returns uint16		-  Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateAuthFrame(UMAC_HANDLE UmacHandle,
				uint8 *pFrameBody,
				uint16 AuthAlgo,
				uint16 TransactionNo,
				WFM_UMAC_AUTH_FRAME_BODY *pRxAuthFrame,
				uint16 status)
{
	uint16 FrameBodyLen = 0;
	WFM_UMAC_AUTH_FRAME_BODY *pAuthFrame = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	IeElement *pIe = NULL;
	uint16 ChallengeTextIeSize = 0;

	OS_ASSERT(pUmacInstance);

	pAuthFrame = (WFM_UMAC_AUTH_FRAME_BODY *) pFrameBody;

	OS_ASSERT(pAuthFrame);

	pAuthFrame->AuthAlgorithm = AuthAlgo;

	pAuthFrame->TransactionSeqNumber = TransactionNo;

	/*
	   One of the old AP's was not accepting value zero in status code for
	   Authentication request as its reserved.
	 */
	pAuthFrame->StatusCode = 0x00;

#if FT_SUPPORT
	if ((pAuthFrame->AuthAlgorithm == UMAC_FT_AUTH)
	    && (TransactionNo == 1)) {
		uint8 nIndex = 0;
		uint8 nResult = FALSE;
		uint8 *pos = (uint8 *) (&pAuthFrame->IEs[0]);

		nResult = WFM_GetPMKIDIndex(
					UmacHandle,
					pUmacInstance->FtTargetBss.MacAddress,
					&nIndex
					);

		*pos = UMAC_IE_ELEMENT_RSN;

		pos++;

		/*Length + PMKID len + PMKID */
		*pos = pUmacInstance->gVars.Wpa2ParamElement.Length + 18;

		pos++;

		OS_MemoryCopy(
			pos,
			(void *)pUmacInstance->gVars.Wpa2ParamElement.ParamInfo,
			pUmacInstance->gVars.Wpa2ParamElement.Length
			);

		pos = pos + pUmacInstance->gVars.Wpa2ParamElement.Length;

		/*Add PMKID in case of WPA2 */
		if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_RSN_IE_SUPPORTED_BY_AP)
		    && ((pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT)
			|| (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT_PSK))) {
			if ((nResult)) {
				*(uint16 *) pos = 0x0001;
				pos += 2;

				OS_MemoryCopy(
					pos,
					&pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfo[nIndex].pmkidBkid[0],
					WFM_PMKID_BKID_SIZE
					);

				pos += WFM_PMKID_BKID_SIZE;
			}
		}

		if (pUmacInstance->gVars.p.mdie[0] == UMAC_IE_ELEMENT_MOBILITY_DOMAIN) {
			OS_MemoryCopy(pos, &pUmacInstance->gVars.p.mdie[0], 5);
			pos += 5;
		}

		if (pUmacInstance->gVars.ftie_len != 0) {
			OS_MemoryCopy(pos, &pUmacInstance->gVars.ftie[0], pUmacInstance->gVars.ftie_len);
			pos += pUmacInstance->gVars.ftie_len;
		}

		FrameBodyLen = sizeof(WFM_UMAC_AUTH_FRAME_BODY) - 2 + (pos - &pAuthFrame->IEs[0]);

		return FrameBodyLen;
	}
#endif	/* FT_SUPPORT */

	if (pRxAuthFrame) {
		/* Update Auth request frame with challenge text for Seq No 3 */
		pIe = (IeElement *) pRxAuthFrame->IEs;

		/* 2 : size of Element Id and Length */
		ChallengeTextIeSize = pIe->size + 2;
		if (pIe->Id == UMAC_IE_ELEMENT_CHALLENGE_TEXT)
			OS_MemoryCopy(pAuthFrame->IEs, pIe, ChallengeTextIeSize);
		FrameBodyLen = sizeof(WFM_UMAC_AUTH_FRAME_BODY) + ChallengeTextIeSize - 2;
	} else {
		/*
		  Update Length of Auth request frame for Seq No. 1 The
		  length of the Padding bytes should be ommited in the length
		*/
		FrameBodyLen = sizeof(WFM_UMAC_AUTH_FRAME_BODY) - (uint16) 2;
	}

#if P2P_MINIAP_SUPPORT
	/* Sending Auth response frame */
	if (TransactionNo == 2)
		pAuthFrame->StatusCode = status;
#endif	/*P2P_MINIAP_SUPPORT */

	return FrameBodyLen;
} /* end WFM_UMAC_CreateAuthFrame() */

#if FT_SUPPORT
/******************************************************************************
 * NAME:	WFM_UMAC_CreateFtActionReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an association request frame body.
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pFrameBody	- a pointer to the buffer where association frame body
 *			  needs to be constructed.
 * \param *TargetApMac	- points to 6 octet MAC address of target AP.
 * \returns uint16	Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateFtActionReq(UMAC_HANDLE UmacHandle,
				  uint8 *pFrameBody,
				  uint8 *TargetApMac)
{
	uint8 *pos, nResult, nIndex;
	WFM_UMAC_FT_REQ_ACTION_BODY *pFtReqAction;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);
	OS_ASSERT(pFrameBody);
	OS_ASSERT(TargetApMac);

	pFtReqAction = (WFM_UMAC_FT_REQ_ACTION_BODY *) pFrameBody;

	pFtReqAction->Category = 6;
	pFtReqAction->Action = 1;
	OS_MemoryCopy(&pFtReqAction->StaMac[0], pUmacInstance->MacAddress, 6);
	OS_MemoryCopy(&pFtReqAction->TargetAPMac[0], TargetApMac, 6);

	pos = &pFtReqAction->IEs[0];

	/*
	 * Insert RSN IE.
	 * Assume RSN IE of target AP is same as current AP.
	 */
	nResult = WFM_GetPMKIDIndex(UmacHandle, TargetApMac, &nIndex);

	*pos = UMAC_IE_ELEMENT_RSN;
	pos++;

	/*Length + PMKID len + PMKID */
	*pos = pUmacInstance->gVars.Wpa2ParamElement.Length + 18;
	pos++;

	OS_MemoryCopy(
		pos,
		(void *)pUmacInstance->gVars.Wpa2ParamElement.ParamInfo,
		pUmacInstance->gVars.Wpa2ParamElement.Length
		);

	pos = pos + pUmacInstance->gVars.Wpa2ParamElement.Length;

	/* Add PMKR0Name */
	*(uint16 *) pos = 0x0001;
	pos += 2;

	OS_MemoryCopy(
		pos,
		&pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfo[nIndex].pmkidBkid[0],
		WFM_PMKID_BKID_SIZE
		);

	pos += WFM_PMKID_BKID_SIZE;

	/*
	 * Insert MDIE
	 */
	if (pUmacInstance->gVars.p.mdie[0] == UMAC_IE_ELEMENT_MOBILITY_DOMAIN) {
		OS_MemoryCopy(pos, &pUmacInstance->gVars.p.mdie[0], 5);
		pos += 5;
	}
	/*
	 * Insert FTIE
	 */
	if (pUmacInstance->gVars.ftie_len) {
		OS_MemoryCopy(pos, &pUmacInstance->gVars.ftie[0], pUmacInstance->gVars.ftie_len);
		pos += pUmacInstance->gVars.ftie_len;
	}

	return (uint16)(pos - pFrameBody);
}
#endif	/* FT_SUPPORT */

/******************************************************************************
 * NAME:	WFM_UMAC_CreateAssocReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an association request frame body.
 * \param UmacHandle		  - Handle to UMAC Instance.
 * \param *pFrameBody		  - a pointer to the buffer where association
 *				    frame body needs to be constructed.
 * \param SsidLength		  - SSID length
 * \param *Ssid			  - SSID of the device/ network to be
 *				    connected with.
 * \param *ReAssoc_MacAddressOfAP - Mac Address of AP in case of reassociation
 *				    (null otherwise)
 * \returns uint16		    Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateAssocReq(UMAC_HANDLE UmacHandle,
			       uint8 *pFrameBody,
			       uint32 SsidLength,
			       uint8 *Ssid,
			       uint8 *ReAssoc_MacAddressOfAP)
{
	uint8 *pos;
	WFM_UMAC_ASSOC_REQ_BODY *pAssocReq;

	/* default is ext rate not present */
	uint32 u32RemainingSupportedRateSet = 0;

	uint32 u32RemainingBasicRateSet;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(pFrameBody);

	pAssocReq = (WFM_UMAC_ASSOC_REQ_BODY *) pFrameBody;

	/*
	   ----------------------------
	   |  Capability	       |
	   ----------------------------
	   |  Listen interval	  |
	   ----------------------------
	   |  *ReAssoc:MacAddressOfAP* |
	   ----------------------------
	   |  SSID		     |
	   ----------------------------
	   |  Supported rates	  |
	   ----------------------------
	   |  Extended Supported Rates |
	   ----------------------------
	   |  Power Capability	 |
	   ----------------------------
	   |  Supported Channels       |
	   ----------------------------
	   |  RSN		      |
	   ----------------------------
	   |  WAPI		     |
	   ----------------------------
	   |  QoS Capability	   |
	   ----------------------------
	   |  Vendor Specific	  |
	   ----------------------------
	 */

    /*************************************************************************
			      1. CAPABILITY
    *************************************************************************/

	/*Specifies the requested operational capabilities to the AP */
	pAssocReq->Capability = pUmacInstance->CapabilitiesReq;

    /*************************************************************************
			     2. Listen Intervel
    *************************************************************************/
	/* stored as 802.11 frame endianness (little endian) */
	pAssocReq->ListenInterval = (pUmacInstance->DTIMPeriod & ~WFM_DTIM_PERIOD_FLAG_UNKNOWN) * pUmacInstance->dtimSkippingCount;

	if ((pUmacInstance->DTIMPeriod & WFM_DTIM_PERIOD_FLAG_UNKNOWN)) {
		if (pAssocReq->ListenInterval > 20)
			pAssocReq->ListenInterval = 20;
		else if (pAssocReq->ListenInterval < 5)
			pAssocReq->ListenInterval = 5;
	}

	if (pAssocReq->ListenInterval < pUmacInstance->PtaInterval)
		pAssocReq->ListenInterval = pUmacInstance->PtaInterval;

	/*Keeping a copy of this value */
	pUmacInstance->AssocListenInterval = pAssocReq->ListenInterval;

    /*************************************************************************
	      pos - ptr to where remaining fields need to be put
    ************************************************************************/

	/*Getting the position of next element */
	pos = (uint8 *) (pAssocReq + 1);

    /*************************************************************************
	     (3R) ** Mac Address of AP - only for Reassoc **
    ************************************************************************/
	if (ReAssoc_MacAddressOfAP != NULL) {
		/* we want to reassociate */
		OS_MemoryCopy(
				pos,
				ReAssoc_MacAddressOfAP,
				WFM_UMAC_MAC_ADDRESS_SIZE
				);
		pos += WFM_UMAC_MAC_ADDRESS_SIZE;
	} /* if( ReAssoc_MacAddressOfAP != NULL) */

    /*************************************************************************
				3. SSID Element
    ************************************************************************/
	if (SsidLength) {

		*pos = UMAC_IE_ELEMENT_SSID;

		pos++;

		*pos = (uint8) SsidLength;

		pos++;

		OS_MemoryCopy(pos, Ssid, SsidLength);

		pos = pos + SsidLength;

	} /* if ( SsidLength ) */

    /*************************************************************************
		4. SUPPORTED RATES.
    *************************************************************************/
	if (pUmacInstance->SupportedRates_WithAP_Abg) {

		*pos = UMAC_IE_ELEMENT_SUPPORTED_RATES;

		pos++;

		pos = pos + 1 + WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate(pUmacInstance->SupportedRates_WithAP_Abg,	/* IN  // Use "pUmacInstance->SupportedRates_WithAP_Abg" if AP rates are to be considered */
										   0,	/* IN // was: pUmacInstance->BasicSupportedRates,  ivan explained that it should be just supported rates there in assoc req. also, cisco behaves that way. */
										   WFM_MAX_ELEMENTS_IN_SUPPORTED_RATES,	/* IN */
										   pos + 1,	/*pu8_80211FormatRate - OUT   // IE_Element Length */
										   pos,	/* pu8NumRatesPut - OUT    // IE_Element Data */
										   &u32RemainingSupportedRateSet,	/*OUT */
										   &u32RemainingBasicRateSet	/* OUT */
		    );
	} /* if ( pUmacInstance->SupportedRates ) */

    /*************************************************************************
		5. EXTENDED SUPPORTED RATES.
    *************************************************************************/

	if (u32RemainingSupportedRateSet) {
		/* non zero means extended rates are present */
		/* IE */
		*pos = UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES;
		pos++;

		pos = pos + 1 + WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate(u32RemainingSupportedRateSet,	/* IN */
										   0,	/* IN */
										   WFM_MAX_ELEMENTS_IN_EXTENDED_SUPPORTED_RATES,	/* IN */
										   pos + 1,	/*pu8_80211FormatRate - OUT   // IE_Element Length */
										   pos,	/* pu8NumRatesPut - OUT    // IE_Element Data */
										   &u32RemainingSupportedRateSet,	/*OUT - will be ignored */
										   &u32RemainingBasicRateSet	/* OUT - will be ignored */
		    );

	} /* if ( pUmacInstance->ExtendedRates ) */

    /*************************************************************************
		6. POWER CAPABILITY   in case of 11K
    *************************************************************************/

#if DOT11K_SUPPORT
	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_RRM_MEASUREMENT_SUPPORTED_BY_AP)
	    && (pUmacInstance->gVars.p.InfoHost.MeasurementCapSupported == TRUE)) {
		*pos = UMAC_IE_ELEMENT_POWER_CAPABILITY;
		pos++;

		*pos = 2;
		pos++;

		*pos = WFM_UMAC_DEFAULT_POWER_LEVEL;
		pos++;

		*pos = WFM_UMAC_DEFAULT_POWER_LEVEL;
		pos++;
	}
#endif /* DOT11K_SUPPORT */

    /*************************************************************************
		7. SUPPORTED CHANNELS
    *************************************************************************/

    /*************************************************************************
		8. RSN
    *************************************************************************/

	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_RSN_IE_SUPPORTED_BY_AP)
	    && ((pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2)
		|| (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_PSK)
#if FT_SUPPORT
		|| (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT)
		|| (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT_PSK)
#endif	/* FT_SUPPORT */
#if MGMT_FRAME_PROTECTION
		|| (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_SH256)
		|| (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_SH256_PSK)
#endif	/*MGMT_FRAME_PROTECTION */
	    )) {
		uint8 nIndex = 0;
		uint8 nResult = FALSE;
		nResult = WFM_GetPMKIDIndex(UmacHandle, pUmacInstance->RemoteMacAdd, &nIndex);
		*pos = UMAC_IE_ELEMENT_RSN;

		pos++;
		if ((pUmacInstance->RoamingEnabled) && (nResult)) {
			/*Length + PMKID len + PMKID */
			*pos = pUmacInstance->gVars.Wpa2ParamElement.Length + 18;
		} else
			*pos = pUmacInstance->gVars.Wpa2ParamElement.Length;

		pos++;

		OS_MemoryCopy(
			pos,
			(void *)pUmacInstance->gVars.Wpa2ParamElement.ParamInfo,
			pUmacInstance->gVars.Wpa2ParamElement.Length
			);

		pos = pos + pUmacInstance->gVars.Wpa2ParamElement.Length;

		/*Add PMKID in case of WPA2 */
		if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_RSN_IE_SUPPORTED_BY_AP)
		    && ((pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2)
#if FT_SUPPORT
			|| (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT)
			|| (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2_FT_PSK)
#endif	/* FT_SUPPORT */
			)) {
			/*Add PMKID only in case of roaming */
			if ((pUmacInstance->RoamingEnabled) && (nResult)) {
				pUmacInstance->PMKIDSent = 1;
				*(uint16 *) pos = 0x0001;
				pos += 2;

				OS_MemoryCopy(
					pos,
					&pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfo[nIndex].pmkidBkid[0],
					WFM_PMKID_BKID_SIZE
					);

				pos += WFM_PMKID_BKID_SIZE;
			}
		}
	}
#if WAPI_SUPPORT
    /*************************************************************************
		9. WAPI
    *************************************************************************/
	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WAPI_IE_SUPPORTED_BY_AP)
	    && (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WAPI)) {
		uint8 nIndex = 0;
		uint8 nResult = FALSE;
		nResult = WFM_GetPMKIDIndex(UmacHandle, pUmacInstance->RemoteMacAdd, &nIndex);
		*pos = UMAC_IE_ELEMENT_WAPI;

		pos++;

		if ((pUmacInstance->RoamingEnabled) && (nResult)) {
			/*Length + BKID len + BKID */
			*pos = pUmacInstance->gVars.WapiParamElement.Length + 18;
		} else
			*pos = pUmacInstance->gVars.WapiParamElement.Length + 2;

		pos++;

		OS_MemoryCopy(
			pos,
			(void *)pUmacInstance->gVars.WapiParamElement.ParamInfo,
			pUmacInstance->gVars.WapiParamElement.Length
			);

		pos = pos + pUmacInstance->gVars.WapiParamElement.Length;

		/*Add BKID */
		if ((pUmacInstance->RoamingEnabled) && (nResult)) {
			pUmacInstance->PMKIDSent = 1;
			*(uint16 *) pos = 0x0001;
			pos += 2;

			OS_MemoryCopy(
				pos,
				&pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfo[nIndex].pmkidBkid[0],
				WFM_PMKID_BKID_SIZE
				);

			pos += WFM_PMKID_BKID_SIZE;
		} else {
			/*BKID Count */
			*(uint16 *) pos = 0x0000;
			pos += 2;
		}

	}
#endif	/* WAPI_SUPPORT */
   /*************************************************************************
		10. RRMEnabledCapabilities
    *************************************************************************/
#if DOT11K_SUPPORT
	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_RRM_MEASUREMENT_SUPPORTED_BY_AP)
		&& (pUmacInstance->gVars.p.InfoHost.MeasurementCapSupported == TRUE)) {
		/*Support by Host */
		*pos = UMAC_IE_ELEMENT_RRM_ENABLED_CAPABILITIES;

		pos++;
		*pos = UMAC_RMM_CAPABILITIES_LEN;

		pos++;

		OS_MemoryCopy(
			pos,
			(void *)&pUmacInstance->dot11k_Meas_Params.RRMCapability,
			(UMAC_RMM_CAPABILITIES_LEN)
			);

		pos = pos + UMAC_RMM_CAPABILITIES_LEN;
	}
#endif /* DOT11K_SUPPORT */

    /*************************************************************************
		8.1 WPA IE (Vendor Specific)
    *************************************************************************/

	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WPA_IE_SUPPORTED_BY_AP)
	    && ((pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA)
		|| (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_PSK))) {
		/*
		   Hard coding some WPA related information elemements
		   .Has to be removed and implemented properly.
		 */

		*pos = UMAC_IE_ELEMENT_VENDOR_SPECIFIC;

		pos++;
		*pos = pUmacInstance->gVars.WpaParamElement.Length;

		pos++;

		OS_MemoryCopy(
			pos,
			(void *)pUmacInstance->gVars.WpaParamElement.ParamInfo,
			pUmacInstance->gVars.WpaParamElement.Length
			);

		pos = pos + pUmacInstance->gVars.WpaParamElement.Length;

	}

    /*************************************************************************
		14. HT Capabilities (11n)
    *************************************************************************/

	/*Parameters for 11n */
	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP)) {
		/*
		   11N is suported, so include the 11N paramters to the request
		 */

		if (pUmacInstance->OurAllSupportedRates_Abgn & WFM_HT_OFDM_RATES_SUPPORTED) {
			pos = WFM_UMAC_Put_HTCapabilitiesIE(
							pos,
							pUmacInstance->OurAllSupportedRates_Abgn,
							(uint8) pUmacInstance->gVars.p.maxRxAmsduSize,
							(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_HT_DISABLE_GREENFIELD_MODE) ? 0 : 1
							);
		}

	/**********************************************************************
		15. 20/40 BSS Coexistence (11n) tbd
	**********************************************************************/

	/**********************************************************************
		16. Extended Capabilities (11n) tbd
	**********************************************************************/

	}

    /*************************************************************************
		x. VENDOR SPECIFIC (Contd). - WMM
    *************************************************************************/

	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_SUPPORTED_BY_AP)) {
		WFM_WMM_INFO_ELEMENT_IN_FRAME *pWmmInfoElement;

		*pos = UMAC_IE_ELEMENT_VENDOR_SPECIFIC;

		pos++;
		/*
		   Inserting WMM IE element. not that the host is sending WMM
		   Parameter element which is a super set of WMM Information
		   element. so we are extracting WMM IE from WMM Parameter IE
		 */
		*pos = sizeof(WFM_WMM_INFO_ELEMENT_IN_FRAME);

		pos++;
		OS_MemoryCopy(
				pos,
				&pUmacInstance->gVars.WmmParamElement,
				sizeof(WFM_WMM_INFO_ELEMENT_IN_FRAME)
				);

		pWmmInfoElement = (WFM_WMM_INFO_ELEMENT_IN_FRAME *) pos;

		/*
		   Since we are creating WMM Info element from WMM Parameter
		   element,the OUI-SubType Needs to be modified
		 */
		pWmmInfoElement->OUISubType = 0;

		/* Adding the size of IE header too */
		pos = pos + sizeof(WFM_WMM_INFO_ELEMENT_IN_FRAME);
	}

#if FT_SUPPORT
	if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_FT_SUPPORTED_BY_AP) {
		/*
		 * Add MDIE
		 */
		if (pUmacInstance->gVars.p.mdie[0] == UMAC_IE_ELEMENT_MOBILITY_DOMAIN) {
			OS_MemoryCopy(pos, &pUmacInstance->gVars.p.mdie[0], 5);
			pos += 5;
		}
		/*
		 * Add FTIE.
		 * In case of initial mobility domain association, Reassoc
		 * request should not contain FTIE. Upper layer must make
		 * sure FTIE is reset.
		 */
		if (pUmacInstance->gVars.ftie_len != 0) {

			OS_MemoryCopy(
				pos,
				&pUmacInstance->gVars.ftie[0],
				pUmacInstance->gVars.ftie_len
				);

			pos += pUmacInstance->gVars.ftie_len;
		}
	}
#endif	/* FT_SUPPORT */
    /*************************************************************************

    *************************************************************************
		11. VENDOR SPECIFIC IEs ?
    *************************************************************************/
	if ((pUmacInstance->VendorIe.assocReqIElength) && (pUmacInstance->VendorIe.pAssocReqIE)) {

		OS_MemoryCopy(
				pos,
				pUmacInstance->VendorIe.pAssocReqIE,
				pUmacInstance->VendorIe.assocReqIElength
				);

		pos += pUmacInstance->VendorIe.assocReqIElength;
	}
#if 0
	if ((pUmacInstance->VendorIe.reqIElength) && (pUmacInstance->VendorIe.pReqIE)) {

		OS_MemoryCopy(
				pos,
				pUmacInstance->VendorIe.pReqIE,
				pUmacInstance->VendorIe.reqIElength
				);

		pos += pUmacInstance->VendorIe.reqIElength;
	}
#endif
	return (uint16) (pos - pFrameBody);
} /* end WFM_UMAC_CreateAssocReq() */

#if P2P_MINIAP_SUPPORT

/******************************************************************************
 * NAME:WFM_UMAC_CreateAssocRsp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an association request frame body.
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pFrameBody	- A pointer to the buffer where association response
 *			  frame body needs to be constructed.
 * \param   AID		- Association ID of the STA
 * \param status	- Status field of the association response
 * \returns uint16	Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateAssocRsp(UMAC_HANDLE UmacHandle,
			       uint8 *pFrameBody,
			       uint8 AID,
			       uint16 status)
{
	uint8 *pos, *posRef;
	WFM_UMAC_ASSOC_RSP_BODY *pAssocResp;

	/* default is ext rate not present */
	uint32 u32RemainingSupportedRateSet = 0;

	uint32 u32RemainingBasicRateSet;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(pFrameBody);

	pAssocResp = (WFM_UMAC_ASSOC_RSP_BODY *) pFrameBody;

/*
	  ----------------------------
	  |  Capability		    |
	  ----------------------------
	  |  Status code	    |
	  ----------------------------
	  |  AID		    |
	  ----------------------------
	  |  Supported rates	    |
	  ----------------------------
	  |Extended Supported Rates |
	  ----------------------------
	  |  EDCA Parameter Set	    |
	  ----------------------------
	  |  Vendor Specific	    |
	  ----------------------------
*/

	pAssocResp->Capability = pUmacInstance->dot11BssConf.capabilityInfo;
	pUmacInstance->sta[AID].capabilityResp = pAssocResp->Capability;

	pAssocResp->StatusCode = status;
	pUmacInstance->sta[AID].AssocRespStatusCode = pAssocResp->StatusCode;

	if (WFM_STATUS_SUCCESS == status)
		pAssocResp->AID = AID;
	else
		pAssocResp->AID = 0;

	/*Getting the position of next element */
	pos = ((uint8 *) pAssocResp + sizeof(pAssocResp->Capability)
	       + sizeof(pAssocResp->StatusCode)
	       + sizeof(pAssocResp->AID));
	posRef = pos;

    /*************************************************************************
		 SUPPORTED RATES.
    *************************************************************************/
	if (pUmacInstance->AllSupportedRates_WithAP_Abgn) {

		*pos = UMAC_IE_ELEMENT_SUPPORTED_RATES;

		pos++;

		pos = pos + 1 + WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate(pUmacInstance->AllSupportedRates_WithAP_Abgn, pUmacInstance->AllBasicSupportedRates_WithAP_Abgn, WFM_MAX_ELEMENTS_IN_SUPPORTED_RATES, pos + 1, pos, &u32RemainingSupportedRateSet, &u32RemainingBasicRateSet);
	} /* if ( pUmacInstance->dot11StartAp.supportedRates ) */

    /*************************************************************************
		EXTENDED SUPPORTED RATES.
    *************************************************************************/

	if (u32RemainingSupportedRateSet) {

		*pos = UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES;
		pos++;

		pos = pos + 1 + WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate(u32RemainingSupportedRateSet, u32RemainingBasicRateSet, WFM_MAX_ELEMENTS_IN_EXTENDED_SUPPORTED_RATES, pos + 1, pos, &u32RemainingSupportedRateSet,	/* will be ignored */
										   &u32RemainingBasicRateSet	/*  will be ignored */
		    );

	} /* if ( u32RemainingSupportedRateSet ) */

    /*************************************************************************
		One or more Vendor Specific IEs
    *************************************************************************/
    /*************************************************************************
					WMM
    *************************************************************************/
	if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		&& (pUmacInstance->dot11BssConf.vdrSpecificCapInfo & WFM_BSS_INFO_FLAGS_BSS_WMM_CAP)) {
		*pos++ = UMAC_IE_ELEMENT_VENDOR_SPECIFIC;

		*pos++ = WMM_PARAM_ELT_LENGTH;

		/* OUI */
		*pos++ = 0x00;
		*pos++ = 0x50;
		*pos++ = 0xF2;

		/* OUI  Type */
		*pos++ = WMM_IE_ELEMENT_TYPE;

		/* OUI  Subype */
		*pos++ = WFM_IE_PARAMETER_ELEMENT_SUB_TYPE;

		*pos++ = WMM_PARAM_ELT_VER;

		*pos++ = pUmacInstance->gVars.WmmParamElement.QoSInfo;

		*pos++ = 0;	/* Reserved */

		OS_MemoryCopy(
			pos,
			&pUmacInstance->gVars.WmmParamElement.Ac_BestEffort,
			sizeof(WFM_WMM_AC_PARAM_PACKED) * WFM_NUM_ACS
			);

		pos += sizeof(WFM_WMM_AC_PARAM_PACKED) * WFM_NUM_ACS;
	}

    /*************************************************************************
					WPA
    *************************************************************************/
	if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		&& (pUmacInstance->dot11BssConf.vdrSpecificCapInfo & WFM_BSS_INFO_FLAGS_SUPP_WPA)) {
		*pos++ = UMAC_IE_ELEMENT_VENDOR_SPECIFIC;

		*pos++ = 24;	/* Length */

		*pos++ = 0x00;	/* VENDOR_OUI */
		*pos++ = 0x50;
		*pos++ = 0xF2;

		*pos++ = WPA_IE_PARAMETER_ELEMENT_SUB_TYPE;

		*pos++ = 0x01;	/* Version */
		*pos++ = 0x00;

		*pos++ = 0x00;	/* Group Cipher */
		*pos++ = 0x50;
		*pos++ = 0xF2;
		*pos++ = CIPHER_TYPE_TKIP;

		*pos++ = 0x01;	/* Pairwise cipher count */
		*pos++ = 0x00;

		*pos++ = 0x00;	/* Pairwise Cipher TKIP list */
		*pos++ = 0x50;
		*pos++ = 0xF2;
		*pos++ = CIPHER_TYPE_TKIP;

		*pos++ = 0x01;	/* AKM Suite count */
		*pos++ = 0x00;

		*pos++ = 0x00;	/* AKM suite TKIP list */
		*pos++ = 0x50;
		*pos++ = 0xF2;
		if (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA)
			*pos++ = 0x01;
		else {
			/*WPA-PSK */
			*pos++ = 0x02;
		}

		*pos++ = 0x18;	/* RSN Capabilities */
		*pos++ = 0x00;
	}


  /*************************************************************************
		 VENDOR SPECIFIC IEs
    *************************************************************************/
	if ((pUmacInstance->VendorIe.assocRespIElength) && (pUmacInstance->VendorIe.pAssocRespIE)) {
		OS_MemoryCopy(
				pos,
				pUmacInstance->VendorIe.pAssocRespIE,
				pUmacInstance->VendorIe.assocRespIElength
				);

		pos += pUmacInstance->VendorIe.assocRespIElength;
	}
#if 0
	if ((pUmacInstance->VendorIe.respIElength) && (pUmacInstance->VendorIe.pRespIE)) {
		OS_MemoryCopy(
				pos,
				pUmacInstance->VendorIe.pRespIE,
				pUmacInstance->VendorIe.respIElength
				);

		pos += pUmacInstance->VendorIe.respIElength;
	}

    /*************************************************************************
		 P2P IE
    *************************************************************************/
	if (pUmacInstance->operatingMode == OPERATING_MODE_GO) {
		if (WFM_STATUS_SUCCESS != status) {
			UMAC_INSERT_ATTRIB(
						pos,
						UMAC_P2P_IE_ATTRIB_STATUS,
						UMAC_P2P_ATTRIB_STATUS_LENGTH
						);

			*pos++ = P2P_STATUS_FAILED_INCOMPAT_PARAMS;
		}
	}
#endif

	pUmacInstance->sta[AID].AssocRespIELen = (pos - posRef);
	OS_MemoryCopy(&pUmacInstance->sta[AID].AssocRspIE[0],
			posRef,
			pUmacInstance->sta[AID].AssocRespIELen);
	return (uint16) (pos - pFrameBody);
} /* end of WFM_UMAC_CreateAssocRsp */

/******************************************************************************
* NAME:	 UMAC_PutVendorIE
******************************************************************************/
/**
* \brief
* This function puts the vendor IE in Beacon/probeResponse.
* \param UmacHandle - Handle to the UMAC Instance
* \param ptr - Pointer where IE is put
* \param subType - Subtype
* \return pointer to the end of vendor IE
******************************************************************************/
uint8 *UMAC_PutVendorIE(UMAC_HANDLE UmacHandle, uint8 *ptr,
			uint8 subType)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 totalIElen = 0;
	uint8 oui[3] = {0};
	uint8 ouiType = 0;
	uint8 IELen = 0;
	uint8 P2P_Oui[3] = {0x50, 0x6F, 0x9A};
	uint32 offset = 0;
	uint8 *pTemp = NULL;
	if (subType == WFM_UMAC_MGMT_BEACON) {
		totalIElen = pUmacInstance->VendorIe.beaconIElength;
		pTemp = (uint8 *)pUmacInstance->VendorIe.pBeaconIE;
	} else if (subType == WFM_UMAC_MGMT_PROB_RSP) {
		totalIElen = pUmacInstance->VendorIe.probeRespIElength;
		pTemp = (uint8 *)pUmacInstance->VendorIe.pProbeRespIE;
	}

	while (totalIElen) {
		IELen = *(uint8 *)((uint8 *)pTemp + 1 + offset);
		OS_MemoryCopy((void *)&oui,
			(void *)((uint8 *)pTemp + 2 + offset),
			3);
		ouiType = *(uint8 *)((uint8 *)pTemp + 5 + offset);
		if ((OS_MemoryEqual(oui, 3, P2P_Oui, 3)) && (ouiType == 0x09)) {
			ptr = UMAC_AddNoAAttribute(UmacHandle,
				ptr,
				(uint8 *)(pTemp + offset));
		} else {
			OS_MemoryCopy((uint8 *)ptr,
				(uint8 *)(pTemp + offset),
				IELen + 2);
			ptr += IELen + 2;
		}
		offset = IELen + 2;
		totalIElen -= offset;
	}
	return ptr;
}
/******************************************************************************
* NAME:	 UMAC_AddNoAAttribute
******************************************************************************/
/**
* \brief
* This function puts the vendor IE and NoA in Beacon.
* \param UmacHandle - Handle to the UMAC Instance
* \param ptr - Pointer where IE is put
* \param p2pIE - Pointer to p2pIE
* \return pointer to the end of vendor IE
******************************************************************************/
uint8 *UMAC_AddNoAAttribute(UMAC_HANDLE UmacHandle, uint8 *ptr, uint8 *p2pIE)
{
	uint8 *pAttributeID = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 p2pIELen = *(uint8 *)((uint8 *)p2pIE + 1);
	uint8 p2pAttributesLen = p2pIELen - 4;
	uint16 attribLen = 0;
	uint8 nResult = FALSE;
	*ptr++ = *p2pIE; /*Element ID*/

	nResult = UMAC_NoAPresent(UmacHandle);
#if 0
	if (nResult && pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow)
		*ptr++ = *(p2pIE + 1) + 18; /*Length with the NoA attribute*/
	else if (nResult &&
		(!pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow))
		*ptr++ = *(p2pIE + 1) + 18; /*Length with the NoA attribute*/
	else if ((!nResult) &&
		(!pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow))
		*ptr++ = *(p2pIE + 1); /*Length with the NoA attribute*/
	else
		*ptr++ = *(p2pIE + 1) + 5; /*Length with the NoA attribute for CTWindow only*/
#endif
	if (nResult) {
		*ptr++ = *(p2pIE + 1) + 18; /*Length with the NoA attribute*/
	} else {
		if (pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow)
			*ptr++ = *(p2pIE + 1) + 5;
		else
			*ptr++ = *(p2pIE + 1);


	}
	OS_MemoryCopy(ptr, (p2pIE + 2), 3); /*OUI*/
	ptr += 3;

	*ptr++ = *(p2pIE + 5); /*OUI Type*/

	/*Processing Attributes ID*/
	pAttributeID = (uint8 *)((uint8 *)p2pIE + 6);


	while (((*pAttributeID) < 12) && (p2pAttributesLen)) {
		attribLen = *(uint16 *)(pAttributeID + 1);
		OS_MemoryCopy(ptr, pAttributeID,
			attribLen + 3);
		ptr += attribLen + 3;
		pAttributeID += attribLen + 3;
		p2pAttributesLen -= attribLen + 3;
	}
	if ((nResult) || (pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow)) {
		/*Add noA here*/
		*ptr++ = 12; /*noA attribute ID*/

		if (nResult) {
			*(uint16 *)ptr = (uint16) 0x000F; /*Length includes Opps and NoA attributes*/
		} else {
			if (pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow)
				*(uint16 *)ptr = (uint16) 0x0002;
		}
		ptr += 2;

		*ptr++ = 1; /*Index*/
		*ptr++ = pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow; /*Opps CT Window*/

		if (nResult) {
			*ptr++ = pUmacInstance->gVars.p.p2pPsMode.count;
			OS_MemoryCopy(ptr,
				&pUmacInstance->gVars.p.p2pPsMode.duration,
				12);
			ptr += 12;
		}
	}
	while (((*pAttributeID) > 12) && (p2pAttributesLen)) {
		attribLen = *(uint16 *)(pAttributeID + 1);
		OS_MemoryCopy(ptr, pAttributeID,
			attribLen + 3);
		ptr += attribLen + 3;
		pAttributeID += attribLen + 3;
		p2pAttributesLen -= attribLen + 3;
	}
	return ptr;
}
/******************************************************************************
* NAME:	 UMAC_NoAPresent
******************************************************************************/
/**
* \brief
* This function puts the vendor IE and NoA in Beacon.
* \param UmacHandle - Handle to the UMAC Instance
* \return TRUE if non-zero value of NoA is present
******************************************************************************/
uint8 UMAC_NoAPresent(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	if ((pUmacInstance->gVars.p.p2pPsMode.count) ||
		(pUmacInstance->gVars.p.p2pPsMode.duration) ||
		(pUmacInstance->gVars.p.p2pPsMode.interval) ||
		(pUmacInstance->gVars.p.p2pPsMode.startTime))
		return TRUE;
	else
		return FALSE;
}

/******************************************************************************
 * NAME:WFM_UMAC_CreateGoNegoReqResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates GO Negotiation request/response body
 * \param UmacHandle	 - Handle to UMAC Instance.
 * \param *pFrameBody	 - A pointer to the buffer where GO negotiation
 *			   confirmation frame body needs to be constructed
 * \param  isReq	 - 1: Request, 0: Response
 * \param  status	 - Status to be filled in Go Negotiation response frame.
 *		           Ignored in case of creating Go negotiation request.
 * \returns uint16	 Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateGoNegoReqResp(UMAC_HANDLE UmacHandle,
				    uint8 *pFrameBody,
				    uint8 isReq,
				    uint8 status)
{
	uint8 *pos;
	UMAC_ACTION_FRAME_BODY *pGoNego;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 wifiOUI[3] = WIFI_OUI;
	uint8 channelList[] = FCC_2G_CHANNELS;
	uint8 *length;
	uint8 i;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(pFrameBody);

	pGoNego = (UMAC_ACTION_FRAME_BODY *) pFrameBody;

	pGoNego->category = UMAC_PUBLIC_ACTN_FRM_CTGRY;
	pGoNego->actionField = D11_WIFI_ELT_ID;
	OS_MemoryCopy(pGoNego->OuiInfo.Oui, wifiOUI, sizeof(wifiOUI));
	pGoNego->OuiInfo.OuiType = UMAC_P2P_OUI_TYPE;
	if (isReq)
		pGoNego->OuiInfo.OuiSubType = D11_P2P_ACTION_TYPE_GO_NEG_REQ;
	else
		pGoNego->OuiInfo.OuiSubType = D11_P2P_ACTION_TYPE_GO_NEG_RESP;

	pGoNego->dialogToken = pUmacInstance->dialogToken;
	/*TBD*/ pos = (uint8 *) (pGoNego + 1);

	/* P2P IE */
	*pos++ = UMAC_P2P_IE_ELEMENT_ID;
	length = pos++;
	OS_MemoryCopy(pos, wifiOUI, sizeof(wifiOUI));
	pos += sizeof(wifiOUI);
	*pos++ = UMAC_P2P_OUI_TYPE;

	/* P2P Attributes */
	/* Status Attribute, if Go Neg response */
	if (!isReq) {
		UMAC_INSERT_ATTRIB(
					pos,
					UMAC_P2P_IE_ATTRIB_STATUS,
					UMAC_P2P_ATTRIB_STATUS_LENGTH
					);

		*pos++ = status;
	}

	/* Capability Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_CAPABILITY,
				UMAC_P2P_ATTRIB_CAPABILITY_LENGTH
				);

	*pos++ = UMAC_DC_CLIENT_DISCVRBLTY | UMAC_DC_DEVICE_LIMIT | UMAC_DC_INVTN_PROCEDURE;
	*pos++ = (pUmacInstance->p2pGrpFormtn.persistentGroup) << 1 | UMAC_GC_INTRA_BSS_DIST;

	/* GO Intent Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_GO_INTENT,
				UMAC_P2P_ATTRIB_GO_INTENT_LENGTH
				);

	*pos++ = (pUmacInstance->p2pGrpFormtn.intentValue) << 1 | pUmacInstance->gVars.p.tieBrkr;

	/* Toggle the tie breaker bit */
	pUmacInstance->gVars.p.tieBrkr = pUmacInstance->gVars.p.tieBrkr ? 0 : 1;

	/* Configuration Timeout Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_CONF_TIMEOUT,
				UMAC_P2P_ATTRIB_CONF_TO_LENGTH
				);

	*pos++ = pUmacInstance->p2pGrpFormtn.configTimeGO;
	*pos++ = pUmacInstance->p2pGrpFormtn.ConfigTimeClient;

	/* Listen Channel Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_LISTEN_CHANNEL,
				UMAC_P2P_ATTRIB_CHANNEL_LENGTH
				);

	*pos++ = (uint8) pUmacInstance->p2pDevDiscvry.listenChannel;
	*pos++ = 0;

	/* Extended Listen Timing Attirbute not supported */

	/* Intended P2P Interface Address Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_INTND_IF_ADDR,
				UMAC_P2P_ATTRIB_INTD_IF_ADDR_LENGTH
				);

	OS_MemoryCopy(pos, pUmacInstance->MacAddress, WFM_MAC_ADDRESS_SIZE);
	pos += WFM_MAC_ADDRESS_SIZE;

	/* Channel List Attribute */
	*pos++ = UMAC_P2P_IE_ATTRIB_CHANNEL_LIST;
	*pos++ = WFM_DOT11_COUNTRY_STRING_SIZE + 1 /*Regulatory Class */  +
	    1 /*Number of Channels */  +
	    sizeof(channelList);
	*pos++ = 0;

	OS_MemoryCopy(
		pos,
		pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString,
		WFM_DOT11_COUNTRY_STRING_SIZE
		);

	pos += WFM_DOT11_COUNTRY_STRING_SIZE;
	*pos++ = UMAC_REG_CLASS_FCC_2G;	/* Regulatory class: FCC */
	OS_MemoryCopy(pos, channelList, sizeof(channelList));
	pos += sizeof(channelList);

	/* Device Info Attribute */
	*pos++ = UMAC_P2P_IE_ATTRIB_DEVICE_INFO;
	*pos++ = 0;		/*Length: TBD */
	*pos++ = 0;
	OS_MemoryCopy(pos, pUmacInstance->MacAddress, WFM_MAC_ADDRESS_SIZE);
	pos += WFM_MAC_ADDRESS_SIZE;
	*pos++ = 0;		/*Config Methods: TBD */
	*pos++ = 0;

	UMAC_INSERT_WSC_IE(
				pos,
				UMAC_WSC_PRIM_DEV_TYPE,
				UMAC_P2P_ATTRIB_PRIM_DEV_TYP_LENGTH
				);

	*pos++ = UMAC_WSC_PRIM_DEV_CAT_ID;
	*pos++ = 0x00;
	*pos++ = 0x50;
	*pos++ = 0xF2;
	*pos++ = 0x04;
	*pos++ = UMAC_WSC_PRIM_DEV_SUB_CAT_ID;
	*pos++ = 0;		/* Number of secondary device types */

	/*Friendly device name */
	OS_MemoryCopy(
			pos,
			pUmacInstance->wscIE.devName,
			pUmacInstance->wscIE.devNameLength
			);

	pos += pUmacInstance->wscIE.devNameLength;

	/* Operating Channel Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_OP_CHANNEL,
				UMAC_P2P_ATTRIB_CHANNEL_LENGTH
				);

	*pos++ = pUmacInstance->p2pGrpFormtn.opChannel;
	*pos++ = 0;

	*length = (uint8) (pos - length - 1);

	/* WSC IE */
	*pos++ = UMAC_IE_ELEMENT_VENDOR_SPECIFIC;
	length = pos++;
	*pos++ = 0x00;
	*pos++ = 0x50;
	*pos++ = 0xF2;
	*pos++ = 0x04;

	/* Version IE */
	UMAC_INSERT_WSC_IE(pos, UMAC_WSC_VER, UMAC_WSC_VER_IE_LENGTH);
	*pos++ = pUmacInstance->wscIE.version;

	/* Device Password IE */
	UMAC_INSERT_WSC_IE(
				pos,
				UMAC_WSC_DEV_PASSWD,
				UMAC_WSC_DEV_PASSWD_IE_LENGTH
				);

	for (i = 0; i < UMAC_WSC_DEV_PASSWD_IE_LENGTH; i++)
		*pos++ = pUmacInstance->wscIE.devPasswd[i];

	*length = (uint8) (pos - length - 1);

	return (uint16) (pos - pFrameBody);
} /* end of WFM_UMAC_CreateGoNegoReqResp() */

/******************************************************************************
 * NAME:WFM_UMAC_CreateGoNegoCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an GO negotiation confirmation frame body.
 * \param UmacHandle	  - Handle to UMAC Instance.
 * \param *pFrameBody	  - A pointer to the buffer where
 *			    association response frame body
 *			    needs to be constructed.
 * \param status	  - Status field of the association response
 * \returns uint16	  -  Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateGoNegoCnf(UMAC_HANDLE UmacHandle,
				uint8 *pFrameBody,
				uint8 status)
{
	uint8 *pos;
	uint8 *length;
	UMAC_ACTION_FRAME_BODY *pGoNegoCnf;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 wifiOUI[3] = WIFI_OUI;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(pFrameBody);

	pGoNegoCnf = (UMAC_ACTION_FRAME_BODY *) pFrameBody;

	pGoNegoCnf->category = UMAC_PUBLIC_ACTN_FRM_CTGRY;
	pGoNegoCnf->actionField = D11_WIFI_ELT_ID;
	OS_MemoryCopy(pGoNegoCnf->OuiInfo.Oui, wifiOUI, sizeof(wifiOUI));
	pGoNegoCnf->OuiInfo.OuiType = UMAC_P2P_OUI_TYPE;
	pGoNegoCnf->OuiInfo.OuiSubType = D11_P2P_ACTION_TYPE_GO_NEG_CNF;
	pGoNegoCnf->dialogToken = pUmacInstance->dialogToken;
	/*TBD*/ pos = (uint8 *) (pGoNegoCnf + 1);

	/* P2P IE */
	*pos++ = UMAC_P2P_IE_ELEMENT_ID;
	length = pos++;		/* Length filled at end */
	OS_MemoryCopy(pos, wifiOUI, sizeof(wifiOUI));
	pos += sizeof(wifiOUI);
	*pos++ = UMAC_P2P_OUI_TYPE;

	/* P2P Attributes */
	/* Status Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_STATUS,
				UMAC_P2P_ATTRIB_STATUS_LENGTH
				);
	*pos++ = status;

	/* Capability Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_CAPABILITY,
				UMAC_P2P_ATTRIB_CAPABILITY_LENGTH
				);

	*pos++ = 0;	/* Device capability, reserved in GO Nego Cnf */

	if (pUmacInstance->operatingMode == OPERATING_MODE_P2P_CLIENT)
		*pos++ = 0;
	else if (pUmacInstance->operatingMode == OPERATING_MODE_GO)
		*pos++ = UMAC_GC_GO | UMAC_GC_INTRA_BSS_DIST;

	/* Operating Channel Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_OP_CHANNEL,
				UMAC_P2P_ATTRIB_CHANNEL_LENGTH
				);

	if (pUmacInstance->operatingMode == OPERATING_MODE_P2P_CLIENT) {
		*pos++ = pUmacInstance->p2pGroupConfig.regClass;
		*pos++ = pUmacInstance->p2pGroupConfig.opChannel;
	} else {
		*pos++ = UMAC_REG_CLASS_FCC_2G;	/* Regulatory class: FCC */
		*pos++ = pUmacInstance->p2pGrpFormtn.opChannel;
	}

	/* Channel list Attribute */
	*pos++ = UMAC_P2P_IE_ATTRIB_CHANNEL_LIST;
	*pos++ = WFM_DOT11_COUNTRY_STRING_SIZE + 1 /*Regulatory Class */  +
	    1 /*Number of Channels */  +
	    pUmacInstance->dot11BssConf.numOfChannels;
	*pos++ = 0;

	OS_MemoryCopy(
			pos,
			pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString,
			WFM_DOT11_COUNTRY_STRING_SIZE
			);

	pos += WFM_DOT11_COUNTRY_STRING_SIZE;
	*pos++ = UMAC_REG_CLASS_FCC_2G;	/* Regulatory class: FCC */

	OS_MemoryCopy(
			pos,
			pUmacInstance->dot11BssConf.channels,
			pUmacInstance->dot11BssConf.numOfChannels
			);

	pos += pUmacInstance->dot11BssConf.numOfChannels;

	*length = (uint8) (pos - length - 1);
	return (uint16) (pos - pFrameBody);

} /* end of WFM_UMAC_CreateGoNegoCnf */

/******************************************************************************
 * NAME:WFM_UMAC_CreateInvtnReqResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates Invitation request/response body
 * \param UmacHandle	 - Handle to UMAC Instance.
 * \param *pFrameBody	 - A pointer to the buffer where frame body needs
 *			   to be constructed
 * \param  isReq	 - 1: Request, 0: Response
 * \param  status	 - Status to be filled in Invitation response frame.
 *			   Ignored in case of creating Invitation request.
 * \returns uint16       Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateInvtnReqResp(UMAC_HANDLE UmacHandle,
				   uint8 *pFrameBody,
				   uint8 isReq,
				   uint8 status)
{
	uint8 *pos;
	uint8 *length;
	UMAC_ACTION_FRAME_BODY *pInvtnReqResp;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 wifiOUI[3] = WIFI_OUI;
	uint8 channelList[] = FCC_2G_CHANNELS;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(pFrameBody);

	pInvtnReqResp = (UMAC_ACTION_FRAME_BODY *) pFrameBody;

	pInvtnReqResp->category = UMAC_PUBLIC_ACTN_FRM_CTGRY;
	pInvtnReqResp->actionField = D11_WIFI_ELT_ID;
	OS_MemoryCopy(pInvtnReqResp->OuiInfo.Oui, wifiOUI, sizeof(wifiOUI));
	pInvtnReqResp->OuiInfo.OuiType = UMAC_P2P_OUI_TYPE;
	if (isReq)
		pInvtnReqResp->OuiInfo.OuiSubType = D11_P2P_ACTION_TYPE_P2P_INVTN_REQ;
	else
		pInvtnReqResp->OuiInfo.OuiSubType = D11_P2P_ACTION_TYPE_P2P_INVTN_RESP;
	pInvtnReqResp->dialogToken = pUmacInstance->dialogToken;
	/*TBD*/ pos = (uint8 *) (pInvtnReqResp + 1);

	/* P2P IE */
	*pos++ = UMAC_P2P_IE_ELEMENT_ID;
	length = pos++;		/* Length filled at end */
	OS_MemoryCopy(pos, wifiOUI, sizeof(wifiOUI));
	pos += sizeof(wifiOUI);
	*pos++ = UMAC_P2P_OUI_TYPE;

	/* P2P Attributes */
	/* Status Attribute, only in case of Invitation response */
	if (!isReq) {
		UMAC_INSERT_ATTRIB(
					pos,
					UMAC_P2P_IE_ATTRIB_STATUS,
					UMAC_P2P_ATTRIB_STATUS_LENGTH
					);

		*pos++ = status;
	}

	/* Configuration Timeout Attribute */
	UMAC_INSERT_ATTRIB(
				pos,
				UMAC_P2P_IE_ATTRIB_CONF_TIMEOUT,
				UMAC_P2P_ATTRIB_CONF_TO_LENGTH
				);

	if (UMAC_ROLE_P2P_GO == pUmacInstance->p2pInvtn.role) {
		if (OPERATING_MODE_GO == pUmacInstance->operatingMode) {
			*pos++ = 0;
		} else {	/* It is the GO of a persistent group */

			*pos++ = pUmacInstance->p2pInvtn.GOConfigTime;
		}
		*pos++ = 0;
	} else {
		*pos++ = 0;
		if (OPERATING_MODE_P2P_CLIENT == pUmacInstance->operatingMode)
			*pos++ = 0;
		else
			*pos++ = pUmacInstance->p2pInvtn.clientConfigTime;
	}

	if (isReq || (OPERATING_MODE_GO >= pUmacInstance->operatingMode)) {
		/* Operating Channel Attribute */
		UMAC_INSERT_ATTRIB(
					pos,
					UMAC_P2P_IE_ATTRIB_OP_CHANNEL,
					UMAC_P2P_ATTRIB_CHANNEL_LENGTH\
					);

		*pos++ = pUmacInstance->dot11BssConf.regClass;
		if (OPERATING_MODE_P2P_DEVICE == pUmacInstance->operatingMode)
			*pos++ = pUmacInstance->p2pInvtn.channel;
		else
			*pos++ = (uint8) pUmacInstance->dot11BssConf.channelNum;

		/* Group BSSID Attribute */
		UMAC_INSERT_ATTRIB(
					pos,
					UMAC_P2P_IE_ATTRIB_GROUP_BSSID,
					WFM_MAC_ADDRESS_SIZE
					);

		OS_MemoryCopy(
				pos,
				pUmacInstance->p2pInvtn.p2pGroupBssid,
				WFM_MAC_ADDRESS_SIZE
				);

		pos += WFM_MAC_ADDRESS_SIZE;
	}

	/* Channel list Attribute */
	*pos++ = UMAC_P2P_IE_ATTRIB_CHANNEL_LIST;
	*pos++ = WFM_DOT11_COUNTRY_STRING_SIZE + 1 /*Regulatory Class */  +
	    1 /*Number of Channels */  +
	    sizeof(channelList);
	*pos++ = 0;

	OS_MemoryCopy(
			pos,
			pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString,
			WFM_DOT11_COUNTRY_STRING_SIZE
			);

	pos += WFM_DOT11_COUNTRY_STRING_SIZE;
	*pos++ = UMAC_REG_CLASS_FCC_2G;	/* Regulatory class: FCC */
	OS_MemoryCopy(pos, channelList, sizeof(channelList));
	pos += sizeof(channelList);

	/*
	  Group ID Attribute and Invitation Flags, only in case of
	  Invitation Request
	*/
	if (isReq) {
		UMAC_INSERT_ATTRIB(
			pos,
			UMAC_P2P_IE_ATTRIB_GROUP_ID,
			(WFM_MAC_ADDRESS_SIZE + pUmacInstance->p2pInvtn.ssidLength)
			);

		OS_MemoryCopy(
				pos,
				pUmacInstance->p2pInvtn.p2pGroupIdAddress,
				WFM_MAC_ADDRESS_SIZE
				);

		pos += WFM_MAC_ADDRESS_SIZE;
		OS_MemoryCopy(pos, pUmacInstance->p2pInvtn.p2pGroupIdSsid, pUmacInstance->p2pInvtn.ssidLength);
		pos += pUmacInstance->p2pInvtn.ssidLength;

		UMAC_INSERT_ATTRIB(
					pos,
					UMAC_P2P_IE_ATTRIB_INVTN_FLAGS,
					UMAC_P2P_ATTRIB_INVTN_FLAG_LENGTH
					);

		 /*TBD*/ *pos++ = 0;	/* Invitation Flags bitmap */
	}
	*length = (uint8) (pos - length - 1);
	return (uint16) (pos - pFrameBody);

} /* end of WFM_UMAC_CreateInvtnReqResp() */

#endif	/*P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:WFM_UMAC_CreateDisAssocReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an DisAssociation request frame body.
 * \param UmacHandle  - Handle to UMAC Instance.
 * \param *pFrameBody - Pointer to the buffer where the frame body
			needs to be constructed.
 * \returns uint16    - Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateDisAssocReq(
					 UMAC_HANDLE UmacHandle,
					 uint8 *pFrameBody)
{
#if MGMT_FRAME_PROTECTION
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
#endif	/*MGMT_FRAME_PROTECTION */
	uint16 FrameBodyLen = 0;
	WFM_UMAC_DISASSOC_REQ_BODY *pDisAssocFrame = NULL;
#if MGMT_FRAME_PROTECTION
	uint8 EncHdrsize = 8;
	uint8 EncTagSize = 8;
	uint8 linkId = 0;
#endif	/*MGMT_FRAME_PROTECTION */

	OS_ASSERT(pFrameBody);

#if MGMT_FRAME_PROTECTION
	if ((pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId] == TRUE)
	    && (pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE)) {
		OS_MemoryReset(pFrameBody, EncHdrsize);
		FrameBodyLen = FrameBodyLen + EncHdrsize;
	}
#endif	/*MGMT_FRAME_PROTECTION */

	pDisAssocFrame = (WFM_UMAC_DISASSOC_REQ_BODY *) ((uint8 *) pFrameBody + FrameBodyLen);

	pDisAssocFrame->ReasonCode = UMAC_REASON_CODE_1;

	FrameBodyLen = FrameBodyLen + 2;

#if MGMT_FRAME_PROTECTION
	if ((pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId] == TRUE)
	    && (pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE)) {
		OS_MemoryReset(((uint8 *) pFrameBody + FrameBodyLen), EncTagSize);
		FrameBodyLen = FrameBodyLen + EncTagSize;
	}
#endif	/*MGMT_FRAME_PROTECTION */

	return FrameBodyLen;

} /*end WFM_UMAC_CreateDisAssocReq() */

/******************************************************************************
 * NAME:	WFM_UMAC_CreateDeAuthFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an DeAuthentication request frame body.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pFrameBody    - a pointer to the buffer where
			  authentication frame body needs to be constructed.
 * \returns uint16      - Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateDeAuthFrame(
					 UMAC_HANDLE UmacHandle,
					 uint8 *pFrameBody)
{
#if MGMT_FRAME_PROTECTION
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
#endif	/*MGMT_FRAME_PROTECTION */
	uint16 FrameBodyLen = 0;
	WFM_UMAC_DEAUTH_FRAME_BODY *pDeAuthFrame = NULL;
#if MGMT_FRAME_PROTECTION
	uint8 EncHdrsize = 8;
	uint8 EncTagSize = 8;
	uint8 linkId = 0;
#endif	/*MGMT_FRAME_PROTECTION */

	OS_ASSERT(pFrameBody);

#if MGMT_FRAME_PROTECTION
	if ((pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId] == TRUE)
	    && (pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE)) {
		OS_MemoryReset(pFrameBody, EncHdrsize);
		FrameBodyLen = FrameBodyLen + EncHdrsize;
	}
#endif	/*MGMT_FRAME_PROTECTION */

	pDeAuthFrame = (WFM_UMAC_DEAUTH_FRAME_BODY *) ((uint8 *) pFrameBody + FrameBodyLen);

	/*Hard coding as unspecified reason now */
	pDeAuthFrame->ReasonCode = UMAC_REASON_CODE_1;

	FrameBodyLen = FrameBodyLen + 2;

#if MGMT_FRAME_PROTECTION
	if ((pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId] == TRUE)
	    && (pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE)) {
		OS_MemoryReset(
				((uint8 *) pFrameBody + FrameBodyLen),
				EncTagSize
				);
		FrameBodyLen = FrameBodyLen + EncTagSize;
	}
#endif	/*MGMT_FRAME_PROTECTION */

	return FrameBodyLen;
} /*end WFM_UMAC_CreateDeAuthFrame() */

/******************************************************************************
 * NAME:	WFM_UMAC_CreateProbeReqTemplateFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates a Probe Request Template Frame.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pFrameBody   - a pointer to the buffer where
 *			  authentication frame body needs to be constructed.
 * \param SsidLength    - Authentication Transaction Number.
 * \param *Ssid		- SSID of the device/ network to be connected with.
 * \param PhyBand       - Phy Band.
 * \param Type		- 0 : SCAN and 1 : FIND
 * \returns uint16	Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateProbeReqTemplateFrame(UMAC_HANDLE UmacHandle,
					    uint8 *pFrameBody,
					    uint8 SsidLength,
					    uint8 *Ssid,
					    uint8 PhyBand,
					    uint8 Type)
{
	uint8 *pos;
	uint32 u32OurAllBasicSupportedRatesAbg;

	/* extended rates not present by default */
	uint32 u32OurAllSupportedRatesAbg = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(pFrameBody);

	/*
	   --------------------------------
	   |  1. SSID		          |
	   --------------------------------
	   |  2. Supported rates	  |
	   --------------------------------
	   |  3. Request information      |
	   --------------------------------
	   |  4. Extended Supported Rates |
	   --------------------------------
	   |  5-6. Vendor Specific	  |
	   --------------------------------
	   |  7. HT Capabilities	  |
	   --------------------------------
	   |  8. 20/40 BSS Coexistence    |
	   --------------------------------
	   |  9. Extended Capabilities    |
	   --------------------------------
	   | 10. P2P IE(Capablities, Channel) |
	   ------------------------------------
	 */

    /*************************************************************************
		pos - ptr to where remaining fields need to be put
    ************************************************************************/

	/*Getting the position of next element */
	pos = pFrameBody;

    /*************************************************************************
				1. SSID Element
    ************************************************************************/
#if P2P_MINIAP_SUPPORT
	if ((Type == 1) && (pUmacInstance->gVars.p.useP2P == UMAC_P2P_ENABLED)) {
		/*Include P2P Wlidcard SSID, This is under TBD in standard */
	} else
#endif	/* P2P_MINIAP_SUPPORT */
		/* if ( SsidLength )  //let it be there always. */
	{
		/*
		   As per the spec SSID element should be there even if SSID
		   length is zero. Zero length SSID is interpreted as wildcard
		   SSID as per section 7.3.2.1 of spec.
		 */
		*pos = UMAC_IE_ELEMENT_SSID;

		pos++;

		*pos = SsidLength;

		pos++;

		if (SsidLength) {
			OS_MemoryCopy(pos, Ssid, SsidLength);
			pos = pos + SsidLength;
		} /* if(SsidLength) */

	}

    /*************************************************************************
		2. SUPPORTED RATES.
    *************************************************************************/

	if (PhyBand & (WFM_BSS_LIST_SCAN_2_4G_BAND | WFM_BSS_LIST_SCAN_AUTO_BAND)) {	/* 2.4 ghz or autoband */
		u32OurAllSupportedRatesAbg = pUmacInstance->OurAllSupportedRates_Abgn	/* all our supported rates */
		    & (WFM_ERP_DSSS_CCK_RATES_SUPPORTED | WFM_ERP_OFDM_RATES_SUPPORTED)	/* only abg rates (not 11n) */
		    & UMAC_PHY_BAND_2_4G_SUPPORTED_RATES;	/* rates for 2.4ghz band */

		u32OurAllBasicSupportedRatesAbg = pUmacInstance->OurAllBasicSupportedRates_Abgn	/* all our basic supported rates */
		    & (WFM_ERP_DSSS_CCK_RATES_SUPPORTED | WFM_ERP_OFDM_RATES_SUPPORTED)	/* only abg rates (not 11n) */
		    & UMAC_PHY_BAND_2_4G_SUPPORTED_RATES;	/* rates for 2.4ghz band */
	} else {
		u32OurAllSupportedRatesAbg = pUmacInstance->OurAllSupportedRates_Abgn	/* all our supported rates */
		    & (WFM_ERP_DSSS_CCK_RATES_SUPPORTED | WFM_ERP_OFDM_RATES_SUPPORTED)	/* only abg rates (not 11n) */
		    & UMAC_PHY_BAND_5G_SUPPORTED_RATES;	/* rates for 5ghz band */

		u32OurAllBasicSupportedRatesAbg = pUmacInstance->OurAllBasicSupportedRates_Abgn	/* all our basic supported rates */
		    & (WFM_ERP_DSSS_CCK_RATES_SUPPORTED | WFM_ERP_OFDM_RATES_SUPPORTED)	/* only abg rates (not 11n) */
		    & UMAC_PHY_BAND_5G_SUPPORTED_RATES;	/* rates for 5ghz band */

	}

#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->gVars.p.useP2P == UMAC_P2P_ENABLED) {
		u32OurAllSupportedRatesAbg &= WFM_ERP_OFDM_RATES_SUPPORTED;
		u32OurAllBasicSupportedRatesAbg &= WFM_ERP_OFDM_RATES_SUPPORTED;
	}
#endif	/*P2P_MINIAP_SUPPORT */

	if (u32OurAllSupportedRatesAbg > 0) {	/* we support atleast 1 rate */
		*pos = UMAC_IE_ELEMENT_SUPPORTED_RATES;

		pos++;

		pos = pos + 1 + WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate(u32OurAllSupportedRatesAbg,	/* IN */
										   0,
										   /*u32OurAllBasicSupportedRatesAbg,  // IN */
										   WFM_MAX_ELEMENTS_IN_SUPPORTED_RATES,	/* IN */
										   pos + 1,	/*pu8_80211FormatRate - OUT   // IE_Element Length */
										   pos,	/* pu8NumRatesPut - OUT    // IE_Element Data */
										   &u32OurAllSupportedRatesAbg,	/*OUT */
										   &u32OurAllBasicSupportedRatesAbg	/* OUT */
		    );

	}

    /*************************************************************************
		3. REQUEST INFORMATION.  tbd
    *************************************************************************/

    /*************************************************************************
		4. EXTENDED SUPPORTED RATES.
    *************************************************************************/

	if (u32OurAllSupportedRatesAbg)	{ /* non-zero if extended rates present */
		/* IE */
		*pos = UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES;
		pos++;

		pos = pos + 1 + WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate(u32OurAllSupportedRatesAbg,	/* IN */
										   u32OurAllBasicSupportedRatesAbg,	/* IN */
										   WFM_MAX_ELEMENTS_IN_EXTENDED_SUPPORTED_RATES,	/* IN */
										   pos + 1,	/*pu8_80211FormatRate - OUT   // IE_Element Length */
										   pos,	/* pu8NumRatesPut - OUT    // IE_Element Data */
										   &u32OurAllSupportedRatesAbg,	/*OUT- value ignored */
										   &u32OurAllBasicSupportedRatesAbg	/* OUT - value ignored */
		    );
	} /* if ( u32OurAllSupportedRatesAbg ) */

    /*************************************************************************
		5. In case of 11K - DS Parameter Set

#if DOT11K_SUPPORT
    if( PhyBand & (WFM_BSS_LIST_SCAN_2_4G_BAND ))
    {
	*pos  = UMAC_IE_ELEMENT_DS_PARAM ;
	pos++ ;
	*pos  = 1 ;
	pos++ ;
	*pos = 0;
	pos++;
    }
#endif
    *************************************************************************/

	/*Parameters for 11n */
	if (pUmacInstance->OurAllSupportedRates_Abgn & WFM_HT_OFDM_RATES_SUPPORTED) {
		/*
		   11N is suported, so include the 11N paramters to the request
		 */
		pos = WFM_UMAC_Put_HTCapabilitiesIE(
				pos,
				pUmacInstance->OurAllSupportedRates_Abgn,
				(uint8) pUmacInstance->gVars.p.maxRxAmsduSize,
				1
				);

	/**********************************************************************
		6. 20/40 BSS Coexistence (11n)
	**********************************************************************/

	/**********************************************************************
		7. Extended Capabilities (11n)
	**********************************************************************/

	}
#if 0
#if P2P_MINIAP_SUPPORT

    /*************************************************************************
		9. WSC IE (Device, Channel)
    *************************************************************************/
	if (pUmacInstance->useP2P == UMAC_P2P_ENABLED) {
		uint8 *length;
		UMAC_P2P_OUI_INFO p2pOuiInfo = {
			UMAC_P2P_OUI,
			UMAC_P2P_OUI_TYPE
		};

		/* Device Name Attribute */
		UMAC_INSERT_WSC_IE(
					pos,
					UMAC_WSC_DEV_NAME,
					pUmacInstance->wscIE.devNameLength
					);

    /*************************************************************************
		10. P2P IEs (Capabilities, Channel)
    *************************************************************************/

		/* P2P OUI Information */
		*pos++ = UMAC_IE_ELEMENT_VENDOR_SPECIFIC;

		length = pos++;

		OS_MemoryCopy(pos, &p2pOuiInfo, sizeof(UMAC_P2P_OUI_INFO));
		pos = pos + sizeof(UMAC_P2P_OUI_INFO);

		/* P2P Capability Attribute */
		UMAC_INSERT_ATTRIB(
					pos,
					UMAC_P2P_IE_ATTRIB_CAPABILITY,
					UMAC_P2P_ATTRIB_CAPABILITY_LENGTH
					);

		*pos++ = pUmacInstance->dot11BssConf.deviceCapability;

		*pos++ = pUmacInstance->dot11BssConf.groupCapability;

		/* P2P Listen Channel Attribute */
		UMAC_INSERT_ATTRIB(
					pos,
					UMAC_P2P_IE_ATTRIB_LISTEN_CHANNEL,
					UMAC_P2P_ATTRIB_CHANNEL_LENGTH
					);

		/* Extended Listen timing Attribute: May be present */

		/* P2P Operating Channel Attribute present only if it is GO */

		/* Default regulatory class */
		*pos++ = pUmacInstance->dot11BssConf.regClass;
		*pos++ = (uint8) pUmacInstance->p2pDevDiscvry.listenChannel;

		*length = (uint8) (pos - length - 1);

	} /* if(pUmacInstance->useP2P == UMAC_P2P_ENABLED) */
#endif	/*P2P_MINIAP_SUPPORT */
#endif
    /*************************************************************************
		11. VENDOR SPECIFIC IEs ?
    *************************************************************************/

	if ((pUmacInstance->VendorIe.probeReqIElength) && (pUmacInstance->VendorIe.pProbeReqIE)) {
		OS_MemoryCopy(
				pos,
				pUmacInstance->VendorIe.pProbeReqIE,
				pUmacInstance->VendorIe.probeReqIElength
				);
		pos += pUmacInstance->VendorIe.probeReqIElength;
	}

#if 0
	if ((pUmacInstance->VendorIe.reqIElength) && (pUmacInstance->VendorIe.pReqIE)) {
		OS_MemoryCopy(
				pos,
				pUmacInstance->VendorIe.pReqIE,
				pUmacInstance->VendorIe.reqIElength
				);
		pos += pUmacInstance->VendorIe.reqIElength;
	}
#endif
	return (uint16) (pos - pFrameBody);

} /* end WFM_UMAC_CreateProbeReqTemplateFrame() */

#if MGMT_FRAME_PROTECTION
/******************************************************************************
 * NAME:	WFM_UMAC_CreateSaQueryFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an SA Query request/response frame body.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pFrameBody    - a pointer to the buffer where
 *			  SA Query frame body needs to be constructed.
 * \param SaQueryAction - value to indicate SA Query Request or Response frame.
 * \param TransactionId - Identifer to specify in SA Query frame.
 * \returns uint16      - Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateSaQueryFrame(UMAC_HANDLE UmacHandle,
				   uint8 *pFrameBody,
				   uint8 SaQueryAction,
				   uint16 TransactionId)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 FrameBodyLen = 0;
	UMAC_SA_QUERY_FRAME_BODY *pSaQueryFrame = NULL;
	uint8 EncHdrsize = 8;
	uint8 EncTagSize = 8;
	uint8 linkId = 0;

	OS_ASSERT(pFrameBody);

	if ((pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId] == TRUE)
	    && (pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE)) {
		OS_MemoryReset(pFrameBody, EncHdrsize);
		FrameBodyLen = FrameBodyLen + EncHdrsize;
	}

	pSaQueryFrame = (UMAC_SA_QUERY_FRAME_BODY *) ((uint8 *) pFrameBody + FrameBodyLen);

	pSaQueryFrame->Action = SaQueryAction;
	pSaQueryFrame->Category = UMAC_CATEGORY_SA_QUERY;
	pSaQueryFrame->TransactionId = TransactionId;

	FrameBodyLen = FrameBodyLen + sizeof(UMAC_SA_QUERY_FRAME_BODY);

	if ((pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId] == TRUE)
	    && (pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm == TRUE)) {
		OS_MemoryReset(
				((uint8 *) pFrameBody + FrameBodyLen),
				EncTagSize
				);
		FrameBodyLen = FrameBodyLen + EncTagSize;
	}

	return FrameBodyLen;
} /* end WFM_UMAC_CreateSaQueryFrame() */
#endif	/*MGMT_FRAME_PROTECTION */

/******************************************************************************
 * NAME:	WFM_UMAC_ProcessBeaconOrProbRspFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This processes incoming beacon and prob response frames.
 * \param UmacHandle	     - Handle to UMAC Instance.
 * \param *pLmacRxDescriptor - Incoming RxDescriptor from LMAC.
 * \param IsBeacon	     - Whether the incoming frame is a beacon or
 *                             Prob Response
 * \returns uint16
 * WFM_BEACON_PROBRSP_LIST_UPDATED	       - If its a new beacon and its
 *						 inserted to list
 * WFM_BEACON_PROBRSP_ERROR		       - Some error occured
 * WFM_BEACON_PROBRSP_INTERNAL_SCAN_ONGOING    - Internal Scan was not able to
 *					         find the device.
 * WFM_BEACON_PROBRSP_INTERNAL_SCAN_GOT_RESULT - Internal Scan finished.
 *****************************************************************************/
uint16 WFM_UMAC_ProcessBeaconOrProbRspFrame(UMAC_HANDLE UmacHandle,
					    void *pLmacRxDescriptor,
					    uint8 IsBeacon)
{
	WFM_BSS_CACHE_INFO_IND *DevInfo = NULL;
	WFM_BSS_CACHE_INFO_IND *pStoredElement = NULL;
	WFM_UMAC_MGMT_FRAME *umacMgmt = NULL;
	WFM_UMAC_BEACON_FRAME_BODY *pBeaconFrameBody = NULL;
	uint16 LengthOfIEElements = 0;
	uint16 FrameBodyLength = 0;
	WFM_DEV_LIST_STATUS DevListStatus = WFM_DEV_LIST_FAILURE;
	uint16 SizeOfMem = 0;
	UMAC_RX_DESC *pRxDescriptor = NULL;
	WFM_DEV_ELEMENT *pDevContainer = NULL;
	uint16 status = WFM_BEACON_PROBRSP_LIST_UPDATED;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	IeElement Ie = { 0 };
	WFM_STATUS_CODE csiStatus = WFM_STATUS_FAILURE;

	OS_ASSERT(pUmacInstance);

	/*
	   Extract the information required, allocate memory and store it.
	   Then return memory to LMAC
	 */

	CHECK_POINTER_IS_VALID(pLmacRxDescriptor);
	pRxDescriptor = (UMAC_RX_DESC *) pLmacRxDescriptor;
	umacMgmt = (WFM_UMAC_MGMT_FRAME *)
	    RX_DESC_GET_DOT11FRAME(pRxDescriptor);
	CHECK_POINTER_IS_VALID(umacMgmt);
	pBeaconFrameBody = (WFM_UMAC_BEACON_FRAME_BODY *) umacMgmt->FrameBody;
	CHECK_POINTER_IS_VALID(pBeaconFrameBody);

	FrameBodyLength = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor) - sizeof(WFM_UMAC_FRAME_HDR));

	/*
	   WFM_UMAC_BEACON_FRAME_BODY contains an arry called IEElements,
	   so we will have to  decrement 4 from size of beacon body
	 */

	LengthOfIEElements = FrameBodyLength - (sizeof(WFM_UMAC_BEACON_FRAME_BODY) - 4);

	/*If this Device is already in the list, we can ignore this beacon now */
	if ((!pStoredElement)) {

		SizeOfMem = (sizeof(WFM_BSS_CACHE_INFO_IND) - 2) + LengthOfIEElements;
		pDevContainer = WFM_UMAC_DEV_GetMemoryElement(pUmacInstance);

		if (!pDevContainer) {
			LOG_EVENT(DBG_WFM_FRAMES, "Device List Full\n");
			status = WFM_BEACON_PROBRSP_ERROR;
			return status;
		}

		DevInfo = (WFM_BSS_CACHE_INFO_IND *) &pDevContainer->DeviceElement;
		CHECK_POINTER_IS_VALID(DevInfo);
		pDevContainer->ElementSize = SizeOfMem;

		/*
		   Copying information available for the LMAC header
		 */

		if (pUmacInstance->gVars.p.RcpiRssiThreshold.RssiRcpiMode & 0x02) {
			/*RSSI case */
			DevInfo->RCPI = 2 * (pRxDescriptor->Rcpi + 110);
			DevInfo->RSSI = pRxDescriptor->Rcpi;
		} else {
			/*RCPI case */
			DevInfo->RCPI = pRxDescriptor->Rcpi;
			DevInfo->RSSI = (sint8) ((pRxDescriptor->Rcpi / 2) - 110);
		}


		DevInfo->IELength = LengthOfIEElements;	/* Little Endian */
		if (DevInfo->IELength) {
			/*Copying the information elements */
			OS_MemoryCopy(
					(void *)DevInfo->IEElements,
					pBeaconFrameBody->IEElements,
					LengthOfIEElements
					);
		}

		if ((WFM_UMAC_ProcIeElements(DevInfo->IEElements, LengthOfIEElements, UMAC_IE_ELEMENT_DS_PARAM, &Ie) == TRUE)
		    && (Ie.size)) {
			DevInfo->ChannelNumber = *(uint8 *) Ie.info;
		} else {
			DevInfo->ChannelNumber = pRxDescriptor->ChannelNumber;
		}
		/* Count of unique BSS on each channel for ACS in MiniAP Mode */
		/* ACS is only supported in 2.4GHz Band */
		if (pUmacInstance->autoChanSel == TRUE)
			pUmacInstance->bss_count[DevInfo->ChannelNumber]++;
		/*Copy the frequency */
		DevInfo->freqKhz = WFM_CONVERT_CHANNEL_NO_TO_FREQ_KHZ(DevInfo->ChannelNumber);

		/*
		   Address2 is the Transmitter Address[TA] of the beacon
		 */

		D0_ADDR_COPY(DevInfo->BssId, umacMgmt->dot11hdr.Address3);

		/* Copying the information available form beacon body */
		DevInfo->TimeStamp = pBeaconFrameBody->TimeStamp;
		DevInfo->BeaconInterval = pBeaconFrameBody->BeaconInterval;
		DevInfo->Capability = pBeaconFrameBody->Capability;

		/* Traverse beacon, and look for Country IE only if 11D OID is enabled */
		if (pUmacInstance->gVars.p.Enable11dFeature) {
			if ((WFM_UMAC_ProcIeElements(DevInfo->IEElements, LengthOfIEElements, UMAC_IE_ELEMENT_COUNTRY, &Ie) == TRUE)
			    && (Ie.size)) {
				/* if Country element was found. */
				/* If CountryInfoAvl is not present then we find the channels and transmit power from the table */
				if ((pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PHY_BAND_2G] != TRUE) || (pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PHY_BAND_5G] != TRUE)) {
					OS_MemoryReset(&pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G], sizeof(WFM_COUNTRY_STRING_INFO));
					OS_MemoryReset(&pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_5G], sizeof(WFM_COUNTRY_STRING_INFO));
					csiStatus = WFM_UMAC_Process_CountryIe(UmacHandle, &Ie);
					if (csiStatus == WFM_STATUS_SUCCESS) {
						pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PHY_BAND_2G] = TRUE;
						pUmacInstance->gVars.p.InfoHost.CountryInfoAvl[PHY_BAND_5G] = TRUE;

					}
				}
			}
		} else {
			/*Get the country string.This will be required for 11k */
			if ((WFM_UMAC_ProcIeElements(DevInfo->IEElements, LengthOfIEElements, UMAC_IE_ELEMENT_COUNTRY, &Ie) == TRUE)
			    && (Ie.size)) {
				/* Store Country String */
				/*We will not update the countryInfoAvl to TRUE as this is not the case of active scanning
				   we are just storing the countryString for 11k Measurement regulatory class */
				if (!pUmacInstance->gVars.p.InfoHost.countryStringAvl) {
					OS_MemoryCopy((uint8 *) pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString, (uint8 *) Ie.info, WFM_DOT11_COUNTRY_STRING_SIZE);
					OS_MemoryCopy((uint8 *) pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_5G].countryString, (uint8 *) Ie.info, WFM_DOT11_COUNTRY_STRING_SIZE);
					pUmacInstance->gVars.p.InfoHost.countryStringAvl = TRUE;
				}

			}
		}

		if (IsBeacon) {
			/*Its a beacon */
			DevInfo->Flags = WFM_BSS_CACHE_INFO_FLAGS_BEACON;

			/*
			   If we are associated, decode beacon and do the necessary actions
			   like
			   1] Check for ERP element change and update the device settings
			   accordinly
			   2] Check for change in EDCA parameter element and act accordingly
			   etc..
			 */
			if (pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) {
				if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode && !OS_MemoryEqual(DevInfo->BssId, WFM_UMAC_MAC_ADDRESS_SIZE, pUmacInstance->RemoteMacAdd, WFM_UMAC_MAC_ADDRESS_SIZE)
				    /* Check whether there is any unsupported capability in the beacon */
				    && (DevInfo->Capability | WFM_UMAC_SUPPORTED_CAPABILITIES) <= (WFM_UMAC_SUPPORTED_CAPABILITIES | WFM_UMAC_CAPABILITY_IBSS)
				    ) {
					WFM_BSS_INFO *pBssElement = (WFM_BSS_INFO *) &pUmacInstance->bssInfo[0];
					pUmacInstance->isCoalescing = TRUE;

					if (WFM_UMAC_ProcIeElements(DevInfo->IEElements, DevInfo->IELength, UMAC_IE_ELEMENT_SSID, &Ie)) {
						WFM_ProcessDeviceCacheInfo(DevInfo, pBssElement, &Ie);
						OS_MemoryCopy(pUmacInstance->RemoteMacAdd, pBssElement->MacAddress, WFM_MAC_ADDRESS_SIZE);

						UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);
						WFM_SCHEDULE_TO_SELF(UmacHandle, UMAC_START_JOIN, pBssElement);
						status = 4;	/*Irrelevant */
						goto exit_handler;
					}
				} else if (OS_MemoryEqual(DevInfo->BssId, WFM_UMAC_MAC_ADDRESS_SIZE, pUmacInstance->RemoteMacAdd, WFM_UMAC_MAC_ADDRESS_SIZE)) {
					WFM_UMAC_ScanIEListForUpdations(UmacHandle, pBeaconFrameBody->Capability, pBeaconFrameBody->IEElements, LengthOfIEElements);
					pUmacInstance->RxRcpi = DevInfo->RCPI;
					pUmacInstance->RxRSSI = DevInfo->RSSI;
				}

			}
		} else {
			/*Its a ProbRsp */
			if ((pUmacInstance->UmacCurrentState == UMAC_ASSOCIATED) &&
				OS_MemoryEqual(DevInfo->BssId, WFM_UMAC_MAC_ADDRESS_SIZE, pUmacInstance->RemoteMacAdd, WFM_UMAC_MAC_ADDRESS_SIZE)) {
				pUmacInstance->RxRcpi = DevInfo->RCPI;
				pUmacInstance->RxRSSI = DevInfo->RSSI;
			}
			DevInfo->Flags = WFM_BSS_CACHE_INFO_FLAGS_PROBE_RESP;
		}

		/*Check for RSN IE for preauthentication */
		if ((WFM_UMAC_ProcIeElements(DevInfo->IEElements, LengthOfIEElements, UMAC_IE_ELEMENT_RSN, &Ie) == TRUE)
		    && (Ie.size)) {

			uint16  PWCipherCount = 0;
			uint16  AKMSuiteCount = 0;
			uint16  RSNCapability = 0;

			if (Ie.size > 6)
				PWCipherCount = (uint16) ((*((uint8 *) Ie.info + 6)) | (*((uint8 *) Ie.info + 7) << 8));

			if (Ie.size > (6+2+(PWCipherCount*4))) {
				AKMSuiteCount = (uint16) ((*((uint8 *) Ie.info + 6 + 2 + (4 * PWCipherCount)))
											| (*((uint8 *) Ie.info + 6 + 2 + (4 * PWCipherCount) + 1) << 8));
			}

			if (Ie.size > (6+2+(PWCipherCount*4)+2+(AKMSuiteCount*4))) {
				RSNCapability = (uint16) ((*((uint8 *) Ie.info + 6 + 2 + (4 * PWCipherCount) + 2 + (4 * AKMSuiteCount)))
											| (*((uint8 *) Ie.info + 6 + 2 + (4 * PWCipherCount) + 2 + (4 * AKMSuiteCount) + 1) << 8));

				if (RSNCapability & 0x0001)
					DevInfo->Flags = (DevInfo->Flags | WFM_PRE_AUTH_MASK);
			}
		}

		if (pUmacInstance->IsInternalScan) {

			status = WFM_UMAC_ProcInternalScanRsp(
								UmacHandle,
								DevInfo
								);

			/*
			   Return the memory we have allocated for the
			   beacon internally
			*/
			if (WFM_DEV_LIST_SUCCESS != WFM_UMAC_DEV_GiveBackMemoryElement(pUmacInstance))
				LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_DEV_GiveBackMemoryElement failed\n");

			return status;
		} /* if( pUmacInstance->IsInternalScan ) */

		/*
		   1] Check number of entries in the device list and update
		      cacheline
		   2] insert to list
		   3] Return RsDescriptor to LMAC
		 */

		/*
		   Cache line is the position where the next element
		   can be inserted.
		*/
		DevInfo->CacheLine = (uint8) WFM_UMAC_DEV_GetListSize(pUmacInstance);
		DevListStatus = WFM_UMAC_DEV_InsertToList(pUmacInstance);

		if (WFM_DEV_LIST_FAILURE == DevListStatus)
			LOG_EVENT(DBG_WFM_FRAMES, "Failed to Update Device List\n");

	} /* if ( pStoredElement ) */

exit_handler:

	return status;

} /* end WFM_UMAC_ProcessBeaconOrProbRspFrame() */

/******************************************************************************
 * NAME:	WFM_UMAC_ProcVendorSpecificIeElements
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the IE element corresponding to the specified OUI
 * information from the given beacon/prob response.
 * \param *IeElementStart   - Start of IE lements in the beacon or prob rsp.
 * \param IELength	    - Total length of the IE elements.
 * \param *pOuiInfo	    - The OUI info. for the element to be retrived.
 * \param *pIelement	    - memory in which IE elements information will be
 *			      available
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_ProcVendorSpecificIeElements(uint8 *IeElementStart,
					   uint16 IELength,
					   WFM_OUI_INFO *pOuiInfo,
					   IeElement *pIelement)
{

	sint32 IeLen = IELength;
	uint8 IeElementId = 0;
	uint8 IeElementSize = 0;

	LOG_EVENT(DBG_WFM_FRAMES, "WFM_ProcVendorSpecificIeElements\n");

	CHECK_POINTER_IS_VALID(IeElementStart);
	CHECK_POINTER_IS_VALID(pOuiInfo);
	CHECK_POINTER_IS_VALID(pIelement);

	OS_MemoryReset(pIelement, sizeof(IeElement));

	do {

		IeElementId = *IeElementStart;
		IeElementSize = *(IeElementStart + 1);

		if (IeElementId == UMAC_IE_ELEMENT_VENDOR_SPECIFIC) {
			if (OS_MemoryEqual((uint8 *) (IeElementStart + 2), 5, (uint8 *) pOuiInfo, 5)) {
				pIelement->Id = IeElementId;
				pIelement->size = IeElementSize;

				if (pIelement->size) {
					/*
					  Point the  IE element's information
					  part
					*/
					pIelement->info = IeElementStart + 2;
				} /* if(pIelement->size) */

				/*
				  Got what we were interested in, so break here
				*/
				break;
			}
		} /* if(IeElementId  ==  IE_VENDOR_SPECIFIC_ID) */

		IeElementStart = (IeElementStart + IeElementSize + 2);
		IeLen = (IeLen - (IeElementSize + 2));

	} while ((IeLen > 0));

} /* end WFM_UMAC_ProcVendorSpecificIeElements() */


#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:	WFM_UMAC_ProcVendorSpecificIeElementsP2P
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given IE element corresponding to the specified
 * OUI information from the given beacon/probe response for P2P.
 * \param  pIeElementStart	-   Start of IE lements in the beacon or
 *				    prob response.
 * \param  ieLength		-   Total length of the IE elements.
 * \param  p2pAttribID		-   The P2P Attribute ID to be retrived
 * \param  pIeElement		-   Memory to which IE elements to be copied.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_ProcVendorSpecificIeElementsP2P(uint8 *pIeElementStart,
					      uint16 ieLength,
					      uint8 p2pAttribID,
					      P2P_IE_ELEMENT *pIeElement)
{
	int ieLen = ieLength;
	uint8 ieElementId = 0;
	uint8 ieElementSize = 0;
	uint16 p2pAttributesLen = 0;
	uint8 pOuiInfo[4] = {0x50, 0x6F, 0x9A, 0x09};
	uint8 *p2pAttributesID = NULL;
	uint16 attribLen = 0;
	uint8 atrribFound = FALSE;

	LOG_EVENT(DBG_WFM_FRAMES, "WFM_UMAC_ProcVendorSpecificIeElementsP2P\n");

	if (pIeElementStart == NULL)
		goto Exit_Func;

	OS_MemoryReset(pIeElement, sizeof(P2P_IE_ELEMENT));
	do {
		ieElementId = (uint8)*pIeElementStart;
		ieElementSize = (uint8) *(pIeElementStart + 1);

		if (ieElementId == UMAC_IE_ELEMENT_VENDOR_SPECIFIC) {
			if (OS_MemoryEqual((pIeElementStart + 2), 4, (uint8 *) pOuiInfo, 4)) {
				p2pAttributesID = (pIeElementStart + 6);
				p2pAttributesLen = ieElementSize - 4;
				attribLen = *(uint16 *)(p2pAttributesID + 1);
				/*Check if NoA is present*/
				while (p2pAttributesLen) {
					if (*p2pAttributesID == p2pAttribID) {
						pIeElement->id = ieElementId;
						pIeElement->size = *(uint16 *)(p2pAttributesID + 1);
						pIeElement->pInfo = (uint8 *) (p2pAttributesID + 3);
						/* Got what we were interested in,
						so break here */
						atrribFound = TRUE;
						break;
					}
					p2pAttributesID += (attribLen + 3);
					p2pAttributesLen -= (attribLen + 3);
					attribLen = *(uint16 *)(p2pAttributesID + 1);
				}
			}
		}
		pIeElementStart = (pIeElementStart + ieElementSize + 2);
		ieLen = (ieLen - (ieElementSize + 2));
		if (atrribFound == TRUE)
			break;
	} while ((ieLen > 0));

Exit_Func:
	return;
}
#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:	WFM_UMAC_AssocResponse_ProcessRates
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the IE element corresponding to the supported rates,
 * extended supported rates, ht capabilities, ht operation and update umac
 * if some rate has been changed since association was requested.
 * \param UmacHandle	    - Handle to UMAC Instance.
 * \param *IeElementStart   - Start of IE lements in the assoc resp.
 * \param IELength	    - Total length of the IE elements.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_AssocResponse_ProcessRates(UMAC_HANDLE UmacHandle,
					 uint8 *IeElementStart,
					 uint16 IELength)
{
	uint32 OldSuppAbgnRate;
	uint32 OldBasicSuppAbgnRate;
	uint32 current_rate, lowest_rate;
	uint32 prevRateIndex;
	uint8 SupportedRateTbl[] = WFM_UMAC_80211_FORMAT_SUPPORTED_RATESET_ABGN;
	SMGMT_REQ_PARAMS *pSmgmtReq;

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	/*
	  1. Store present rates
	*/
	OldSuppAbgnRate = pUmacInstance->AllSupportedRates_WithAP_Abgn;
	OldBasicSuppAbgnRate = pUmacInstance->AllBasicSupportedRates_WithAP_Abgn;

	/*
	  2. Check supported rates/basic supported rates again from assoc resp
	*/
	WFM_UMAC_CvtAllRatesFromIeToWfmFormat(
			UmacHandle,
			IeElementStart,
			IELength,
			&pUmacInstance->SupportedRates_WithAP_Abg,
			&pUmacInstance->BasicSupportedRates_WithAP_Abg,
			&pUmacInstance->AllSupportedRates_WithAP_Abgn,
			&pUmacInstance->AllBasicSupportedRates_WithAP_Abgn
			);

	/*
	  3. Take intersection with our rates
	*/
	pUmacInstance->SupportedRates_WithAP_Abg &= pUmacInstance->OurAllSupportedRates_Abgn;
	pUmacInstance->BasicSupportedRates_WithAP_Abg &= pUmacInstance->OurAllBasicSupportedRates_Abgn;
	pUmacInstance->AllSupportedRates_WithAP_Abgn &= pUmacInstance->OurAllSupportedRates_Abgn;
	pUmacInstance->AllBasicSupportedRates_WithAP_Abgn &= pUmacInstance->OurAllBasicSupportedRates_Abgn;

	/*
	  4. Program WSM to use new supported rates for retry
	  (if rates have changed)
	*/
	if (OldSuppAbgnRate != pUmacInstance->AllSupportedRates_WithAP_Abgn) {
		LOG_DATA2(DBG_WFM_ASSOC, "Supported Rates changed after association: Old=0x%x, New=0x%x\n", OldSuppAbgnRate, pUmacInstance->AllSupportedRates_WithAP_Abgn);
		LOG_EVENT(DBG_WFM_ASSOC | DBG_WFM_ERROR, "ERROR: Wfm don't support programming of supported rates after join - CQ2649");
	}

	/*
	  5. Program WSM to use new basic supported rates for retry
	  (if rates have changed)
	*/
	if (OldBasicSuppAbgnRate != pUmacInstance->AllBasicSupportedRates_WithAP_Abgn) {
		LOG_DATA2(DBG_WFM_ASSOC, "Basic Supported Rates changed after association: Old=0x%x, New=0x%x\n", OldBasicSuppAbgnRate, pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);
		pUmacInstance->WriteJoinModeBasicRateSet = 1;
	}

	/*
	  6. Finding the rates supported in different modulation
	*/
	pUmacInstance->ErpDssCckRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_DSSS_CCK_RATES_SUPPORTED);
	pUmacInstance->ErpOfdmRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_OFDM_RATES_SUPPORTED);
	pUmacInstance->HtOfdmRates = (uint32) (pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_HT_OFDM_RATES_SUPPORTED);

	/*
	  7. Select the operational Rate
	*/
	if (pUmacInstance->HtOfdmRates)
		pUmacInstance->CurrentModeOpRates = pUmacInstance->HtOfdmRates;
	else if (pUmacInstance->ErpOfdmRates)
		pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpOfdmRates;
	else if (pUmacInstance->ErpDssCckRates)
		pUmacInstance->CurrentModeOpRates = pUmacInstance->ErpDssCckRates;
	else
		pUmacInstance->CurrentModeOpRates = pUmacInstance->AllSupportedRates_WithAP_Abgn;

	UMAC_UpdateTxRatesBitmap(pUmacInstance, DEFAULT_LINK_ID);
	/*
	  8. Program transmit rate
	*/
	current_rate = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->TxRatesBitmap[DEFAULT_LINK_ID]);
	lowest_rate  = WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->TxRatesBitmap[DEFAULT_LINK_ID]);

	while (current_rate > lowest_rate) {
		if (SupportedRateTbl[current_rate] <= WFM_802_11_HT_RATE_26_MBPS)
			break;
		prevRateIndex = current_rate;
		current_rate = WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(pUmacInstance->TxRatesBitmap[DEFAULT_LINK_ID], current_rate);
		if (prevRateIndex == current_rate)
			break;
	}

	pUmacInstance->CurrentRateIndex = current_rate;

	/*
	  9. Set Tx Queue params with operational (supported) rates for data
	  frames
	*/
	/* a. store in umac */
	UMAC_Initialize_TxQueueParams_Rates_All(
				UmacHandle,
				pUmacInstance->AllSupportedRates_WithAP_Abgn
				);

} /* end WFM_UMAC_AssocResponse_ProcessRates() */

/******************************************************************************
 * NAME:	WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts 802.11 formatted rate into WFM format rate.
 * It returns basic/supported rates in WFM format.
 * \param NumRates	         - Number of rates in pu8SupportedRatesRemote.
 * \param *pu8_80211FormatRate   - Supported Rates in 802.11 formate
 *                                 (in 500Kbps) with msb set for basic rate.
 * \param *pu32SupportedRateSet  - Supported Rate in WFM format(supported by us)
 * \param *pu32BasicRateSet	 - Basic Supported Rate in WFM format.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate(uint32 NumRates,
							uint8 *pu8_80211FormatRate,
							uint32 *pu32SupportedRateSet,
							uint32 *pu32BasicRateSet)
{
	uint32 i, j;
	uint8 RateFound;
	uint8 SupportedRateTbl[] = WFM_UMAC_80211_FORMAT_SUPPORTED_RATESET_ABG;
	uint32 BasicRateSet = 0;
	uint32 SupportedRateSet = 0;

	/* 802.11: 1 Mbps = 2, 2Mbps = 4, 5.5 Mbps = 1, etc  */
	/* Wsm:   1 Mbps = bit0 set, 2Mbps = bit1 set */
	for (i = 0; i < NumRates; i++) {
		RateFound = 0;
		for (j = 0; j < sizeof(SupportedRateTbl); j++) {
			/*
			  MSB needs to be Masked. Reffer 802.11 std -2007
			  .section 7.3.2.2
			*/
			if (SupportedRateTbl[j] == (pu8_80211FormatRate[i] & 0x7F)) {
				RateFound = 1;
				break;
			}
		} /* for(j=0;j<sizeof(SupportedRateTbl); j++)*/

		if (RateFound) {
			SupportedRateSet = (SupportedRateSet | (0x01 << j));

			if (pu8_80211FormatRate[i] & 0x80)
				BasicRateSet = (BasicRateSet | (0x01 << j));

		} /* if( RateFound ) */

	} /* for( i = 0 ; i < pBssInfo->NumRates  ; i++ ) */

	*pu32SupportedRateSet = SupportedRateSet;
	*pu32BasicRateSet = BasicRateSet;

} /* end WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate() */

/******************************************************************************
 * NAME:	WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts WFM format rate into 802.11 formatted rate.
 * It returns basic/supported rates in WFM format.
 * \param u32SupportedRateSet	    - IN -  supported rate in wfm format.
 * \param u32BasicRateSet	    - IN -  basic supported rate in wfm format.
 * \param MaxNumRates		    - IN -  Max number of rates to fill in
 *					    pu8_80211FormatRate.
 * \param *pu8_80211FormatRate	    - OUT - Array to fill in supported rate.
 * \param *pu8NumRatesPut	    - OUT - Num Rates Put
 * \param *pu32RemainingSupportedRateSet - OUT - Remaining supported rates
 *					   (bits already in pu8_80211FormatRate
 *                                         are cleared)
 * \param *pu32RemainingBasicRateSet     - OUT - Remaining basic supported
 *                                         rates (bits already in
 *                                         pu8_80211FormatRate are cleared)
 * \returns uint32.
 *****************************************************************************/
uint32 WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate(uint32 u32SupportedRateSet,
							  uint32 u32BasicRateSet,
							  uint32 MaxNumRates,
							  uint8 *pu8_80211FormatRate,
							  uint8 *pu8NumRatesPut,
							  uint32 *pu32RemainingSupportedRateSet,
							  uint32 *pu32RemainingBasicRateSet)
{
	uint8 SupportedRateTbl[] = WFM_UMAC_80211_FORMAT_SUPPORTED_RATESET_ABG;
	uint32 NumSuppRates = 0;
	uint32 CurRate;

	/* 802.11: 1 Mbps = 2, 2Mbps = 4, 5.5 Mbps = 11 , etc  */
	/* Wsm:   1 Mbps = bit0 set, 2Mbps = bit1 set */

	while ((u32SupportedRateSet != 0) && (NumSuppRates < MaxNumRates)) {

		CurRate = WFM_UMAC_GET_LOWEST_RATE_INDEX(u32SupportedRateSet);

		pu8_80211FormatRate[NumSuppRates] = SupportedRateTbl[CurRate];

		if (u32BasicRateSet) {
			if (u32BasicRateSet >> CurRate & 1)
				pu8_80211FormatRate[NumSuppRates] = BASIC_RATE(pu8_80211FormatRate[NumSuppRates]);
		}

		NumSuppRates++;

		u32SupportedRateSet &= ~(1 << CurRate);
	}

	*pu8NumRatesPut = (uint8) NumSuppRates;
	*pu32RemainingSupportedRateSet = u32SupportedRateSet;
	*pu32RemainingBasicRateSet = u32BasicRateSet & u32SupportedRateSet;

	return NumSuppRates;
} /* end WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate() */

/******************************************************************************
 * NAME: WFM_UMAC_Cvt_80211_Rate_To_Wfm_Rate_OnlyOurSupportedRates_Obsolete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts WFM format rate into 802.11 formatted rate.
 * It returns basic/supported rates in WFM format.
 * \param NumRates		   - Number of rates in pu8SupportedRatesRemote.
 * \param *pu8SupportedRatesRemote - Supported Rates in 802.11 formate
 *                                   (in 500Kbps) with msb set for basic rate
 * \param *pu32SupportedRateSet    - Supported Rate in WFM format
 *                                   (supported by us)
 * \param *pu32BasicRateSet	   - Basic Supported Rate in WFM format
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Cvt_80211_Rate_To_Wfm_Rate_OnlyOurSupportedRates_Obsolete(uint32 NumRates,
									uint8 *pu8SupportedRatesRemote,
									uint32 *pu32SupportedRateSet,
									uint32 *pu32BasicRateSet)
{
	uint32 i, j;
	uint8 RateFound;
	uint8 SupportedRateTbl[] = WFM_UMAC_80211_FORMAT_SUPPORTED_RATESET_ABG;
	uint32 BasicRateSet = 0;
	uint32 SupportedRateSet = 0;

	for (i = 0; i < NumRates; i++) {
		RateFound = 0;
		for (j = 0; j < sizeof(SupportedRateTbl); j++) {
			/*MSB needs to be Masked. Reffer 802.11 std -2007 .section 7.3.2.2 */
			if ((SupportedRateTbl[j] & 0x7F) == (pu8SupportedRatesRemote[i] & 0x7F)) {

				RateFound = 1;
				break;
			}
		} /* for(j=0;j<sizeof(SupportedRateTbl); j++)*/

		if (RateFound) {
			SupportedRateSet = (SupportedRateSet | (0x01 << j));

			if (pu8SupportedRatesRemote[i] & 0x80 & SupportedRateTbl[j])
				BasicRateSet = (BasicRateSet | (0x01 << j));
		} /* if( RateFound ) */
	} /* for( i = 0 ; i < pBssInfo->NumRates  ;  i++ )*/

	*pu32SupportedRateSet = SupportedRateSet;
	*pu32BasicRateSet = BasicRateSet;

} /*end WFM_UMAC_Cvt_80211_Rate_To_Wfm_Rate_OnlyOurSupportedRates_Obsolete() */

/******************************************************************************
 * NAME:	WFM_UMAC_Cvt_80211nFormat_Rate_To_WfmFormat_Rate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts 802.11n formatted rate into WFM format rate.
 * \param *p802_11nFormatRate - (IN) 802.11n format 11n rate.
 * \param *pu32WfmFormatRate  - (OUT) Wfm format rate.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Cvt_80211nFormat_Rate_To_WfmFormat_Rate(WFM_HT_SUPPORTED_MCS_SET_PACKED *p802_11nFormatRate,
						      uint32 *pu32WfmFormatRate)
{
	/* note: presently, we support just 8 11n rates */
	*pu32WfmFormatRate = p802_11nFormatRate->s.RxMCSBitmap0 << UMAC_HT_LOWEST_RATE_BIT_POS;
}

/******************************************************************************
 * NAME: WFM_UMAC_Cvt_WfmFormat_Rate_To_80211nFormat_Rate_OnlyOurSupportedRates
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts 802.11n formatted rate into WFM format rate.
 * \param u32WfmFormatRate  - (OUT) Wfm format rate.
 * \param *p802_11nFormatRate - (IN) 802.11n format 11n rate.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Cvt_WfmFormat_Rate_To_80211nFormat_Rate_OnlyOurSupportedRates(uint32 u32WfmFormatRate,
									    WFM_HT_SUPPORTED_MCS_SET_PACKED *p802_11nFormatRate)
{
	p802_11nFormatRate->s.RxMCSBitmap0 = ((u32WfmFormatRate >> UMAC_HT_LOWEST_RATE_BIT_POS) & 0xFF)
	    & WFM_SUP_MCS_RX_MCS_BMAP_0;
	p802_11nFormatRate->s.RxMCSBitmap1 = WFM_SUP_MCS_RX_MCS_BMAP_1;
	p802_11nFormatRate->s.RxMCSBitmap2 = WFM_SUP_MCS_RX_MCS_BMAP_2;
	p802_11nFormatRate->s.RxMCSBitmap3 = WFM_SUP_MCS_RX_MCS_BMAP_3;
	p802_11nFormatRate->s.RxMCSBitmap4 = WFM_SUP_MCS_RX_MCS_BMAP_4;
	p802_11nFormatRate->s.RxMCSBitmap5 = WFM_SUP_MCS_RX_MCS_BMAP_5;
	p802_11nFormatRate->s.RxMCSBitmap6 = WFM_SUP_MCS_RX_MCS_BMAP_6;
	p802_11nFormatRate->s.RxMCSBitmap7 = WFM_SUP_MCS_RX_MCS_BMAP_7;
	p802_11nFormatRate->s.RxMCSBitmap8 = WFM_SUP_MCS_RX_MCS_BMAP_8;
	p802_11nFormatRate->s.RxMCSBitmap9 = WFM_SUP_MCS_RX_MCS_BMAP_9;
}

/******************************************************************************
 * NAME:	WFM_UMAC_CvtAllRatesFromIeToWfmFormat
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes supported rates/basic supported rates, HT rates
 * from the IE.
 * \param *IeElementStart	  -  Start of IE lements
 * \param IELength		  -  Total length of the IE elements
 * \param *pWfmSuppRate_abg       -  802.11 abg Supported rates present
 * \param *pWfmBasicSuppRate_abg  -  802.11 abg Basic Supported rates present
 * \param *pWfmSuppRate_abgn      -  802.11 abgn Supported rates present
 * \param *pWfmBasicSuppRate_abgn -  802.11 abgn Basic Supported rates present
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_CvtAllRatesFromIeToWfmFormat(UMAC_HANDLE UmacHandle,
					   uint8 *IeElementStart,
					   uint16 IELength,
					   uint32 *pWfmSuppRate_abg,
					   uint32 *pWfmBasicSuppRate_abg,
					   uint32 *pWfmSuppRate_abgn,
					   uint32 *pWfmBasicSuppRate_abgn)
{
	IeElement Ie;
	uint32 SuppRates = 0; /* initialization needed */
	uint32 BasicSuppRates = 0; /*initialization needed */

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *)UmacHandle;

	/* 1. initialize all to zero (needed) */
	*pWfmSuppRate_abg = 0;
	*pWfmBasicSuppRate_abg = 0;
	*pWfmSuppRate_abgn = 0;
	*pWfmBasicSuppRate_abgn = 0;

	/* 2. traverse IEs and look for supported rates IE */
	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_IE_ELEMENT_SUPPORTED_RATES, &Ie) == TRUE) {
		/* supported rate IE found */
		if (Ie.size != 0) {
			/*Converting the Basic Supported Rates to WSM Bitmask */
			WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate(Ie.size,	/* num of rates */
									   Ie.info,	/* supported rates */
									   pWfmSuppRate_abg, pWfmBasicSuppRate_abg);
		}		/*if(Ie.size != 0) */
	}			/*if( WFM_UMAC_ProcIeElements - UMAC_IE_ELEMENT_SUPPORTED_RATES */

	/* 3. traverse IEs and look for extended supported rates IE */
	if (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES, &Ie) == TRUE) {
		/* extended supported rate IE found */
		if (Ie.size != 0) {
			/*Converting the Extended Supported Rates to WSM Bitmask */
			WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate(Ie.size,	/* num of rates */
									   Ie.info,	/* supported rates */
									   &SuppRates, &BasicSuppRates);
		}		/*if(Ie.size != 0) */
	}			/*if( WFM_UMAC_ProcIeElements - UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES */

	/* 4. Add it to info recd from supported rate IE */
	*pWfmSuppRate_abg |= SuppRates;
	*pWfmBasicSuppRate_abg |= BasicSuppRates;

	/* 5. look for HT rates now in HT Capabilities (supported 11n rates) */
	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP)
		&& (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_IE_ELEMENT_HT_CAPABILITIES, &Ie) == TRUE)) {
		/* IE found */
		if (Ie.size != 0) {
			WFM_HT_CAPABILITIES_IE_IN_FRAME_PACKED *pHtCapabilities;

			pHtCapabilities = (WFM_HT_CAPABILITIES_IE_IN_FRAME_PACKED *) Ie.info;

			/*Converting the 11n Supported Rates to WSM Bitmask */
			WFM_UMAC_Cvt_80211nFormat_Rate_To_WfmFormat_Rate(&pHtCapabilities->SupportedMCSSet, pWfmSuppRate_abgn);

		}		/*if(Ie.size != 0) */
	}			/*if( WFM_UMAC_ProcIeElements - UMAC_IE_ELEMENT_HT_CAPABILITIES */

	/* 6. look for HT basic rates in HT Operation IE (basic supported 11n rates) */
	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP)
		&& (WFM_UMAC_ProcIeElements(IeElementStart, IELength, UMAC_IE_ELEMENT_HT_OPERATION, &Ie) == TRUE)) {
		/* IE found */
		if (Ie.size != 0) {
			WFM_HT_OPERATION_IE_IN_FRAME_PACKED *pHtOperation;

			pHtOperation = (WFM_HT_OPERATION_IE_IN_FRAME_PACKED *) Ie.info;

			/*Converting the 11n Supported Rates to WSM Bitmask */
			WFM_UMAC_Cvt_80211nFormat_Rate_To_WfmFormat_Rate(&pHtOperation->BasicMCSSet, pWfmBasicSuppRate_abgn);

		}		/*if(Ie.size != 0) */
	}			/*if( WFM_UMAC_ProcIeElements - UMAC_IE_ELEMENT_HT_OPERATION */

	/* 7. update abgn with abg rates */
	*pWfmSuppRate_abgn |= *pWfmSuppRate_abg;
	*pWfmBasicSuppRate_abgn |= *pWfmBasicSuppRate_abg;

}

/******************************************************************************
 * NAME:	WFM_UMAC_Put_HTCapabilitiesIE
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function puts HT Capabilities IE.
 * \param *pos		    -  IN/OUT) Put IE here
 * \param u32WfmFormatRate  -  (IN) Wfm format rate
 * \param large_rxamsdu     -  (IN) Max rx AMSDU size 1=>7935 octet 0=>3839
 *                             octet
 * \param use_greenfield    -  (IN) device can receive 11n greenfield preamble
 * \returns uint8* New position for pos.
 *****************************************************************************/
uint8 *WFM_UMAC_Put_HTCapabilitiesIE(uint8 *pos,
				     uint32 u32WfmFormatRate,
				     uint8 large_rx_amsdu,
				     uint8 use_greenfield)
{
	uint16 caps = WFM_UMAC_HT_CAP_OURS;
	WFM_HT_CAPABILITIES_IE_IN_FRAME_PACKED HtCapabilities = { 0 };

	*pos = UMAC_IE_ELEMENT_HT_CAPABILITIES;
	pos++;

	*pos = sizeof(WFM_HT_CAPABILITIES_IE_IN_FRAME_PACKED);
	pos++;

	if (large_rx_amsdu)
		caps |= WFM_HT_CAP_MAX_A_MSDU_LENGTH_7935;
	if (use_greenfield)
		caps |= WFM_HT_CAP_GREEN_FIELD;
	else
		caps &= ~WFM_HT_CAP_GREEN_FIELD;

	/* Fill in our capabilities */
	HtCapabilities.CapabilitiesInfo = cpu_to_le16(caps);

	HtCapabilities.AMPDU_Params = WFM_AMPDU_PARAM_OURS;

	/* fill in RxMCSBitmap0 to RxMCSBitmap9 */
	WFM_UMAC_Cvt_WfmFormat_Rate_To_80211nFormat_Rate_OnlyOurSupportedRates(u32WfmFormatRate, &HtCapabilities.SupportedMCSSet);

	HtCapabilities.SupportedMCSSet.s.Reserved1 = 0;
	HtCapabilities.SupportedMCSSet.s.RxHighestSupportedDataRate = WFM_SUP_MCS_RX_HIGHEST_SUP_DATA_RATE;
	HtCapabilities.SupportedMCSSet.s.Reserved2 = 0;
	HtCapabilities.SupportedMCSSet.s.TxMCSSetDefined = WFM_SUP_MCS_TX_MCS_SET_DEFINED;
	HtCapabilities.SupportedMCSSet.s.TxRxMCSSetNotEqual = WFM_SUP_MCS_TXRX_MCS_SET_NOT_EQUAL;
	HtCapabilities.SupportedMCSSet.s.TxMaxNumSpatialStreamsSup = WFM_SUP_MCS_TX_MAX_NUM_SPATIAL_STRAMS_SUP;
	HtCapabilities.SupportedMCSSet.s.TxUnequalModulationSup = WFM_SUP_MCS_TX_UNEQUAL_MODULATION_SUP;
	HtCapabilities.SupportedMCSSet.s.Reserved3 = 0;
	HtCapabilities.TxBeamFormingCapabilities = cpu_to_le16(WFM_TX_BEAM_FORM_CAP_OURS);
	HtCapabilities.HTExtendedCapabilities.All = cpu_to_le16(WFM_HT_EXTEND_CAP_OURS);
	HtCapabilities.ASEL_Capabilities = WFM_ASEL_OURS;

	OS_MemoryCopy(pos, (uint8 *) &HtCapabilities, sizeof(WFM_HT_CAPABILITIES_IE_IN_FRAME_PACKED)
	    );

	return (uint8 *)((uint8 *) ((WFM_HT_CAPABILITIES_IE_IN_FRAME_PACKED *) pos + 1));

} /* end WFM_UMAC_Put_HTCapabilitiesIE() */

/******************************************************************************
 * NAME:	WFM_UMAC_ProcIeElements
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given IE element from the given beacon/prob
 * response.
 * \param *IeElementStart   -  Start of IE lements in the beacon or prob rsp.
 * \param IELength	    -  Total length of the IE elements.
 * \param ElementToRetrive  -  The IE element to be retrived.
 * \param *pIelement	    -  memory to which IE elements to be copied.
 * \returns uint8 TRUE (IE found), FALSE (IE not found).
 *****************************************************************************/
uint8 WFM_UMAC_ProcIeElements(uint8 *IeElementStart,
			      uint16 IELength,
			      uint32 ElementToRetrive,
			      IeElement *pIelement)
{

	uint8 IeElementId;
	uint8 IeElementSize;
	uint8 Found = FALSE;	/* Not found */

	LOG_EVENT(DBG_WFM_UMAC, "WFM_ProcIeElements");

	OS_ASSERT(IeElementStart);

	/*0 generally means not found */
	/*but it is still possible that element is found and size is 0 */
	pIelement->size = 0;

	/* an IE with IE_length 0 might be there */
	/* if IELength < 2, it means we are in pad bytes */
	while (IELength >= 2) {

		IeElementId = (uint8) *IeElementStart;
		IeElementSize = (uint8) *(IeElementStart + 1);

		/*check if the ie element length is valid */
		if (IeElementSize > (IELength - 2)) {
			/*
			  2 bytes subtracted for IeElementId and IeElementSize
			*/
			/* it means length is not valid */
			LOG_DATA2(DBG_WFM_ERROR, "WFM_UMAC_ProcIeElements(): Error: IeElementSize (%d) not correct (should be < %d)!\n", IeElementSize, (IELength - 2));

			/*
			  we should not continue now, as it may be pad bytes
			  at the end of the frame, so exit
			*/
			break;
		}

		if (ElementToRetrive == IeElementId) {
			pIelement->Id = IeElementId;
			pIelement->size = IeElementSize;
			pIelement->info = IeElementStart + 2;

			Found = TRUE;

			break;
		} /* if ( ElementToRetrive == IeElementId ) */

		/* not found yet, so continue searching */
		IeElementStart += (IeElementSize + 2);
		IELength -= (IeElementSize + 2);

	} /* while(IELength >= 2) */

	if (IELength == 1)
		LOG_EVENT(DBG_WFM_ERROR, "WFM_UMAC_ProcIeElements(): Error: IE is padded!");

	return Found;

} /* end WFM_ProcIeElements() */

/******************************************************************************
 * NAME:   UMAC_ProcProbeReqCheckCCKRates
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function cecks the presence of OFDM rates in probe req IE, which will
 * be generated on the reception of probe request from the remote device with
 * status success.
 * -# Releases the Rx buffer and Rxdescriptor for probe Req frame
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pMsg		- Message to be Processed.
 * \returns uint32.
 *****************************************************************************/
uint32 UMAC_ProcProbeReqCheckCCKRates(UMAC_HANDLE UmacHandle, void *pMsg)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	UMAC_RX_DESC *pRxDescriptor = (UMAC_RX_DESC *) pMsg;
	WFM_UMAC_MGMT_FRAME *pUmacMgmt = NULL;
	uint16 ProbeReqIELength = 0;
	uint8 *pProbeReqIE = NULL;
	uint8 i, supportedrate, numofsupprates = 0;
	IeElement ie = {0};


	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "UMAC_ProcProbeReqCheckCCKRates()\n");

	OS_ASSERT(pMsg);

	CHECK_POINTER_IS_VALID(pRxDescriptor);

	pUmacMgmt = (WFM_UMAC_MGMT_FRAME *) RX_DESC_GET_DOT11FRAME(pRxDescriptor);
	CHECK_POINTER_IS_VALID(pUmacMgmt);

	/* CPU - Little Endian */
	ProbeReqIELength = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor) - sizeof(WFM_UMAC_FRAME_HDR));	/* 6 is added for sizeof cap+statuscode+aid */

	if (ProbeReqIELength > 0) {
		pProbeReqIE =  (uint8 *)pUmacMgmt	+  sizeof(WFM_UMAC_FRAME_HDR);
		if (WFM_UMAC_ProcIeElements(pProbeReqIE, ProbeReqIELength, UMAC_IE_ELEMENT_SUPPORTED_RATES, &ie)) {
			numofsupprates = ie.size;
			for (i = 0; i < numofsupprates; i++) {
				supportedrate = ie.info[i] & ~(1<<7);
				switch (supportedrate) {
				case 0x0c:
				case 0x12:
				case 0x18:
				case 0x24:
				case 0x30:
				case 0x48:
				case 0x60:
				case 0x6c:
					Result = WFM_STATUS_SUCCESS;
					break;
				default:
					Result = WFM_STATUS_FAILURE;
					break;
				}

				if (Result == WFM_STATUS_SUCCESS)
					break;
			}
		}
	} else {
		OS_ASSERT(ProbeReqIELength);
	}

	return Result;
} /* end UMAC_ProcProbeReqCheckCCKRates() */




/******************************************************************************
 * NAME:WFM_UMAC_ProcWPSIE
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given attirbute from the WPS IE
 * response.
 * \param *IeElementStart   -  Start of WPS IE element.
 * \param IELength	    -  Total length of the WPS IE element
 * \param ElementToRetrive  -  The attribute to be retrived.
 * \param *pAttrib	    -  memory to which attribute to be copied.
 * \returns uint8 TRUE (attribute found), FALSE (attribute not found).
 *****************************************************************************/
uint8 WFM_UMAC_ProcWPSIE(uint8 *IeElementStart,
			 uint32 IELength,
			 uint32 ElementToRetrive,
			 WPS_IeElement *pAttrib)
{
	uint8 IeElementId;
	uint8 IeElementSize;
	uint8 Found = FALSE;	/* Not found */

	LOG_EVENT(DBG_WFM_UMAC, "WFM_UMAC_ProcWPSIE");

	OS_ASSERT(IeElementStart);

	/*0 generally means not found */
	/*but it is still possible that element is found and size is 0 */
	pAttrib->size = 0;

	/* an IE with IE_length 0 might be there */
	/* if IELength < 4, it means we are in pad bytes */
	while (IELength >= 4) {

		IeElementId = (uint8) *IeElementStart;
		IeElementSize = (uint8) *(IeElementStart + 2);

		/*check if the ie element length is valid */
		if (IeElementSize > (IELength - 4)) {
			/*
			  4 bytes subtracted for IeElementId and IeElementSize
			*/

			/*
			  it means length is not valid or WPS IE is not present
			*/

			/*
			  we should not continue now, as it may be pad bytes
			  at the end of the frame, so exit
			*/
			break;
		}

		if (ElementToRetrive == IeElementId) {
			pAttrib->Id = IeElementId;
			pAttrib->size = IeElementSize;
			pAttrib->info = IeElementStart + 4;

			Found = TRUE;

			break;
		} /* if ( ElementToRetrive == IeElementId ) */

		/* not found yet, so continue searching */
		IeElementStart += (IeElementSize + 4);
		IELength -= (IeElementSize + 4);

	} /* while(IELength >= 4) */

	if (IELength == 1)
		LOG_EVENT(DBG_WFM_ERROR, "WFM_UMAC_ProcWPSIE(): Error: IE is padded!");

	return Found;
} /* end WFM_UMAC_ProcWPSIE() */

/******************************************************************************
 * NAME:	WFM_UMAC_Process_ErpIe
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Erp Ie and enables/disables protection.
 * \param UmacHandle	-  Handle to UMAC Instance.
 * \param ErpInfo	-  Erp Info.
 * \param pUmacRuntime  -  Pointer to UMAC_RUNTIME structure.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Process_ErpIe(UMAC_HANDLE UmacHandle,
			    uint8 ErpInfo,
			    UMAC_RUNTIME *pUmacRuntime)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	pUmacRuntime->WriteUpdateJoinMode = FALSE;

	/*
	   As per the current standard, ERP element has only one byte
	   information in it. The following code should be changed when there
	   is a change to the information element.
	 */

	/* Store it as it will be used in
	   WFM_UMAC_Process_HtOperationIe_AllParams() to decide whether to use
	   protection for GF mode or not
	*/
	pUmacInstance->ErpInfo = ErpInfo;

	/* 2. Use CTS/RTS Protection */
	if (ErpInfo & WFM_UMAC_ERP_IE_USE_PROTECTION) {
		/* UseProtection Bit */
		/*Protection enabled */
		/*
		  Means, >=1 stations, which cannot understand ERP-OFDM rates,
		  which are associated with the AP we are communicating with.
		*/

		/* enable protection if not already enabled */
		pUmacRuntime->HtOperationIe.WriteErpProtection = TRUE;
		pUmacRuntime->HtOperationIe.ErpProtection = TRUE;
	} /* end - if ( ErpInfo & WFM_UMAC_ERP_IE_USE_PROTECTION ) */
	else {
		/*Protection disabled */
		/*disable protection if not already disabled */
		pUmacRuntime->HtOperationIe.WriteErpProtection = TRUE;
		pUmacRuntime->HtOperationIe.ErpProtection = FALSE;
	} /* end - else - if ( ErpInfo & WFM_UMAC_ERP_IE_USE_PROTECTION ) */

	/* 3. Short Preamble supported */
	if (ErpInfo & WFM_UMAC_ERP_IE_BARKER_PREAMBLE_MODE) {
		/* BarkerPreambleMode Bit */
		/* set => All stations do not support short preamble */
		if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_BARKER_SHORT_PREAMBLE) {
			/*
			  was using short preamble earlier. so program wsm to
			  change it
			*/
			pUmacInstance->Flags &= (~WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_BARKER_SHORT_PREAMBLE);
			pUmacRuntime->WriteUpdateJoinMode = TRUE;
			pUmacRuntime->PreambleType = WSM_PREAMBLE_LONG;
		}
	} else {
		/* clear All stations connected to AP support short preamble */
		if (!(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_BARKER_SHORT_PREAMBLE)) {
			/*
			  was using long preamble earlier. so program wsm to
			  change it
			*/
			pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_BARKER_SHORT_PREAMBLE;
			pUmacRuntime->WriteUpdateJoinMode = TRUE;
			pUmacRuntime->PreambleType = WSM_PREAMBLE_SHORT;
		}
	} /* if (ErpInfo & WFM_UMAC_ERP_IE_BARKER_PREAMBLE_MODE)) */

	return;
} /* end - WFM_UMAC_Process_ErpIe() */

/******************************************************************************
 * NAME:	WFM_UMAC_Process_HtOperationIe_AllParams
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes HT Operation Ie and enables/disables HT protection.
 * Use it only after calling Process_ErpIe
 * \param UmacHandle	    - Handle to UMAC Insance.
 * \param *pHtOperationInfo - HT Operation IE.
 * \param *pUmacRunTime     - pUmacRunTime
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Process_HtOperationIe_AllParams(UMAC_HANDLE UmacHandle,
					      WFM_HT_OPERATION_IE_IN_FRAME_PACKED *pHtOperationInfo,
					      UMAC_RUNTIME *pUmacRunTime)
{
	WFM_UMAC_Process_HtOperationIe_Param2(
					UmacHandle,
					&(pHtOperationInfo->HTInfoParam2),
					&pUmacRunTime->HtOperationIe
					);

	WFM_UMAC_Process_HtOperationIe_Param3(
					UmacHandle,
					&(pHtOperationInfo->HTInfoParam3),
					&pUmacRunTime->HtOperationIe
					);
}

/******************************************************************************
 * NAME:	WFM_UMAC_Process_HtOperationIe_Param2
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Param2 of HT Operation Ie and enables/disables
 * HT protection.
 * Use it only after calling Process_ErpIe.
 * \param UmacHandle	  -  Handle to UMAC Insance.
 * \param *pHTInfoParam2  -  HT Operation IE param 2.
 * \param *pHtOpIe	  -  HT operational Element.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Process_HtOperationIe_Param2(UMAC_HANDLE UmacHandle,
					   WFM_HT_OPERATION_IE_PARAM2_PACKED *pHTInfoParam2,
					   HT_OPERATION_IE *pHtOpIe)
{

/*
  First few rows shows what is said by 802.11n standard, and last rows
  shows what is implemented by WFM.

  HT_Protection    ERP_UseProtection    Non_GF_Present       Standard		 In Wbf
  ------------------------------------------------------------------------------------------------------------------------
  According to standard:
  ----------------------
  0 or 2	   don't care	   0		    No Protection	    Use GF
  0 or 2	   don't care	   1		    Use prot for GF	  Use Mixed mode instead of GF
  1		0		    don't care	   Prot for all HT	  Use CTS/RTS
  1		0		    1		    Prot only for HT GF      Use Mixed mode instead of GF
  1		1		    don't care	   TABLE 9.6? (Prot HT)     Use CTS/RTS
  3		0		    don't care	   Prot only for HT GF      Use Mixed mode instead of GF
  3		1		    don't care	   Prot for all HT. Type    Use CTS/RTS
  of protection depends
  on non-HT STA

  Implemented in WFM:
  -------------------
  0 or 2	   don't care	   0		    No prot		  Don't use CTS/RTS. Use GF.
  0 or 2	   don't care	   1		    Prot GF		  Don't use CTS/RTS. Use Mixed mode.
  1		don't care	   don't care	   Prot all HT	      Use CTS/RTS. Use GF.
  3		0		    don't care	   Prot GF		  Don't use CTS/RTS. Use Mixed mode.
  3		1		    don't care	   Prot all HT	      Use CTS/RTS. Use GF.

*/
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	OS_ASSERT(pUmacInstance);

	/* only go in if we are associated in 11n mode */
	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP)) {
		/*Reset SetHtprotection variable as it wll be set again now */
		pHtOpIe->SetHtProtection = 0;
		pHtOpIe->WriteHtProtection = TRUE;

		/* 1. Use HT Protection */
		switch (pHTInfoParam2->s.HTProtection) {
		case 0:	/* no protection mode */
		case 2:	/* 20 MHz protection mode */
			{
				/*a. don't use CTS/RTS Protection */
				pHtOpIe->ErpProtection = FALSE;
				pHtOpIe->WriteErpProtection = TRUE;
				/*SetHtProtection Mode */
				if (pHTInfoParam2->s.HTProtection == 0)
					pHtOpIe->SetHtProtection |= UMAC_NO_PROT_MODE;
				else
					pHtOpIe->SetHtProtection |= UMAC_20_MHZ_PROT_MODE;
				break;
			}

		case 1:	/* non-member protection mode */
			{
				/*a. use CTS/RTS Protection always */
				if ((pUmacInstance->ErpInfo & WFM_UMAC_ERP_IE_USE_PROTECTION) == 0) {
					/*a. Don't use CTS/RTS Protection */
					pHtOpIe->ErpProtection = FALSE;
				} else {
					/*a. use CTS/RTS Protection always */
					pHtOpIe->ErpProtection = TRUE;
				}
				pHtOpIe->WriteErpProtection = TRUE;
				/*SetHTProtection Mode */
				pHtOpIe->SetHtProtection |= UMAC_NON_MEMBER_PROT_MODE;
				break;
			}

		case 3:	/* non-HT mixed mode */
			{
				if ((pUmacInstance->ErpInfo & WFM_UMAC_ERP_IE_USE_PROTECTION) == 0) {
					/*a. Don't use CTS/RTS Protection */
					pHtOpIe->ErpProtection = FALSE;
				} else {
					/*a. use CTS/RTS Protection always */
					pHtOpIe->ErpProtection = TRUE;
				}
				pHtOpIe->WriteErpProtection = TRUE;
				/*SetHTProtection Mode */
				pHtOpIe->SetHtProtection |= UMAC_NON_HT_MIXED_PROT_MODE;
				break;
			}
		} /* end - switch( pHTInfoParam2->s.HTProtection) */

		/* 2. green field mode check. 0 => greenfield, 1 => mixed */
		if (pHTInfoParam2->s.NonGreenfieldHTSTAsPresent == 0) {
			/*
			  Set to 0 if all HT STAs that are associated are
			  HT Greenfield capable
			*/
			pHtOpIe->GreenFieldMode = TRUE;
			pHtOpIe->WriteGreenFieldMode = TRUE;
		} else {
			/*
			  Set to 1 if one or more HT STAs that are not
			  HT Greenfield
			*/
			pHtOpIe->GreenFieldMode = FALSE;
			pHtOpIe->WriteGreenFieldMode = TRUE;
			/*SetHTProtection -> Non GreenGield Present */
			pHtOpIe->SetHtProtection |= UMAC_SET_NON_GREEN_FIELD_CAPABLE_STA;
		}
	}

	return;
}

/******************************************************************************
 * NAME:	WFM_UMAC_Process_HtOperationIe_Param3
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Param3 of HT Operation Ie and set/reset
 * SetHtProtection.
 * \param UmacHandle	  -  Handle to UMAC Insance.
 * \param *pHTInfoParam3  -  HT Operation IE param 3.
 * \param *pHtOpIe	  -  HT operational Element.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Process_HtOperationIe_Param3(UMAC_HANDLE UmacHandle,
					   WFM_HT_OPERATION_IE_PARAM3_PACKED *pHTInfoParam3,
					   HT_OPERATION_IE *pHtOpIe)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	OS_ASSERT(pUmacInstance);

	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP)) {

		if (pHTInfoParam3->s.DualCTSProtection)
			pHtOpIe->SetHtProtection |= UMAC_SET_DUAL_CTS_PROTECTION;
		if (pHTInfoParam3->s.LSIGTXOPProtectionFullSupport)
			pHtOpIe->SetHtProtection |= UMAC_SET_LSIG_TXOP_PROTECTION;
	}
}

/******************************************************************************
 * NAME:	WFM_UMAC_ScanIEListForUpdations
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes Param2 of HT Operation Ie and enables/disables
 * HT protection. Use it only after calling Process_ErpIe.
 * \param UmacHandle	     -  Handle to UMAC Insance
 * \param Capability	     -  Capability of AP.
 * \param *pIeElements       -  Pointer to the start of IE elements
 * \param LengthOfIEElements -  Total size of all IE elements
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_ScanIEListForUpdations(UMAC_HANDLE UmacHandle,
				     uint16 Capability,
				     uint8 *pIeElements,
				     uint16 LengthOfIEElements)
{

	IeElement Ie;
	P2P_IE_ELEMENT p2pIeElement;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	SMGMT_REQ_PARAMS *pSmgmtReq;
	UMAC_RUNTIME *pUmacRunTime;
	UMAC_NOA_ATTRIBUTE noaAttribute = { 0 };

	OS_ASSERT(pUmacInstance);

	pSmgmtReq = (SMGMT_REQ_PARAMS *) &pUmacInstance->gVars.p.SerialMgmtInfo;

	pSmgmtReq->Type = SMGMT_REQ_TYPE_DYNAMIC_UPDATIONS;
	pSmgmtReq->ReqNum = 0;
	pUmacRunTime = (UMAC_RUNTIME *) &pSmgmtReq->run_time;
	if (pUmacInstance->Band == PHY_BAND_2G) {
		if (Capability & WFM_UMAC_SUPPORTED_CAPABILITIES & WFM_UMAC_CAPABILITY_SHORT_SLOT_TIME) {
			if (!(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_SHORT_SLOT)) {
				/*Set Short Slot Time */
				pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_SHORT_SLOT;
				pUmacRunTime->WriteSloteTime = TRUE;
				pUmacRunTime->SlotTime = WFM_SLOT_TIME_SHORT_SLOT;
			}
		} else {
			if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_SHORT_SLOT) {
				/*Set Long Slot Time */
				pUmacInstance->Flags &= (~WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_SHORT_SLOT);
				pUmacRunTime->WriteSloteTime = TRUE;
				pUmacRunTime->SlotTime = WFM_SLOT_TIME_LONG_SLOT;
			}
		}
	}
	/*a. traverse beacon, and look for erp IE */
	if (!pUmacInstance->gVars.p.useP2P) {
		if ((WFM_UMAC_ProcIeElements(pIeElements, LengthOfIEElements, UMAC_IE_ELEMENT_ERP_INFORMATION, &Ie) == TRUE)
			&& (Ie.size)) {
			/*if ERP element was found, check if it has got changed */
			WFM_UMAC_Process_ErpIe(UmacHandle, *(uint8 *) Ie.info, pUmacRunTime);
		}
	}

	if (WFM_UMAC_ProcIeElements(pIeElements, LengthOfIEElements, UMAC_IE_ELEMENT_HT_CAPABILITIES, &Ie) == TRUE) {
		/*if ERP element was found, check if it has got changed */
		if (Ie.size) {
			uint16 HtCapInfo;
			OS_MemoryCopy((void *)&HtCapInfo, Ie.info, 2);
			if (HtCapInfo & (WFM_HT_CAP_GREEN_FIELD)) {
				if (!(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_HT_GREENFIELD_MODE_SUPPORTED_BY_AP))
					pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_HT_GREENFIELD_MODE_SUPPORTED_BY_AP;
			} else {
				if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_HT_GREENFIELD_MODE_SUPPORTED_BY_AP)
					pUmacInstance->Flags &= ~(WFM_UMAC_INSTANCE_FLAGS_HT_GREENFIELD_MODE_SUPPORTED_BY_AP);
			}
		}
	}
	/*b. traverse beacon, and look for HT OPERATION IE */
	if (WFM_UMAC_ProcIeElements(pIeElements, LengthOfIEElements, UMAC_IE_ELEMENT_HT_OPERATION, &Ie) == TRUE) {
		/*if ERP element was found, check if it has got changed */
		if (Ie.size)
			WFM_UMAC_Process_HtOperationIe_AllParams(UmacHandle, (WFM_HT_OPERATION_IE_IN_FRAME_PACKED *) Ie.info, pUmacRunTime);
	} /* if( WFM_UMAC_ProcIeElements */

	if (pUmacInstance->DTIMPeriod & WFM_DTIM_PERIOD_FLAG_UNKNOWN) {
		if ((WFM_UMAC_ProcIeElements(pIeElements, LengthOfIEElements, UMAC_IE_ELEMENT_TIM, &Ie) == TRUE)
		    && (Ie.size)) {
			uint8 WakeUpInterval;

			pUmacInstance->DTIMPeriod = (uint8) Ie.info[1];

			WakeUpInterval = pUmacInstance->DTIMPeriod * pUmacInstance->dtimSkippingCount;

			pUmacRunTime->BeaconWakeupPeriod = WakeUpInterval;
		}
	}
	/*
	   Look for change in EDCA parameter updations
	 */
	if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION) {

		if ((WFM_UMAC_ProcIeElements(pIeElements, LengthOfIEElements, UMAC_IE_ELEMENT_EDCA_PARAMETERS, &Ie) == TRUE)
		    && (Ie.size)) {
			WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED *pEdcaParam = (WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED *) Ie.info;

			if ((pEdcaParam->QoSInfo & 0x0F) != pUmacInstance->EdcaParamSetUpdateCount) {
				/*EDCA Parameter has changed, so update it with LMAC */
				pUmacRunTime->WriteEdcaParam = TRUE;
				OS_MemoryCopy(
					&pUmacRunTime->EdcaParam,
					pEdcaParam,
					sizeof(WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED)
					);

			}
		} /* if( WFM_UMAC_ProcIeElements */
	} else if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) {
		WFM_OUI_INFO OuiInfo = {
			WMM_OUI,
			WMM_IE_ELEMENT_TYPE,
			WFM_IE_PARAMETER_ELEMENT_SUB_TYPE
		};

		WFM_UMAC_ProcVendorSpecificIeElements(
							pIeElements,
							LengthOfIEElements,
							&OuiInfo,
							&Ie
							);

		if (Ie.size) {
			WFM_WMM_PARAM_ELEMENT *pwmmParamElement = (WFM_WMM_PARAM_ELEMENT *) Ie.info;

			if ((pwmmParamElement->QoSInfo & 0x0F) != pUmacInstance->EdcaParamSetUpdateCount) {
				pUmacRunTime->WriteEdcaParam = TRUE;
				OS_MemoryCopy(
					&pUmacRunTime->EdcaParam,
					&pwmmParamElement->QoSInfo,
					sizeof(WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED)
					);
			}
		} /* if( Ie.size ) */
	}

	if ((WFM_UMAC_ProcIeElements(pIeElements, LengthOfIEElements, UMAC_IE_ELEMENT_CHANNEL_SWITCH, &Ie) == TRUE)
	    && (Ie.size)) {
		/*
		   Getting the all values from Channel Switch info element.
		*/
		pUmacRunTime->SwitchChReq.ChannelMode  = (uint8) Ie.info[0];
		pUmacRunTime->SwitchChReq.NewChannelNumber  = (uint8) Ie.info[1];
		pUmacRunTime->SwitchChReq.ChannelSwitchCount  = (uint8) Ie.info[2];
		pUmacRunTime->IsChSwitchAnnounce = TRUE;
		pUmacInstance->SwitchChannelReq = UMAC_SC_REQ_INTERNAL;
	} /* if( WFM_UMAC_ProcIeElements */


#if P2P_MINIAP_SUPPORT
	WFM_UMAC_ProcVendorSpecificIeElementsP2P(
		pIeElements,
		LengthOfIEElements,
		UMAC_P2P_IE_ATTRIB_NOA,
		&p2pIeElement
		);
	if (p2pIeElement.size) {
		/* Copy NoA attributes from beacon */
		OS_MemoryCopy(
			(void *)&noaAttribute.index,
			p2pIeElement.pInfo,
			p2pIeElement.size
			);
		if ((pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow != noaAttribute.oppsCtWindow)
			|| (pUmacInstance->gVars.p.p2pPsMode.count != noaAttribute.count)
			|| (pUmacInstance->gVars.p.p2pPsMode.duration != noaAttribute.duration)
			|| (pUmacInstance->gVars.p.p2pPsMode.interval != noaAttribute.interval)
			|| (pUmacInstance->gVars.p.p2pPsMode.startTime != noaAttribute.startTime)
			) {
				/*Parametes of NoA Attribute in beacon chnaged*/
				/*Update our own parmaters from beacon and configure firmware*/
				pUmacRunTime->updateNoaAttrib = TRUE;
				pUmacInstance->gVars.p.p2pPsMode.oppPsCTWindow = noaAttribute.oppsCtWindow;
				pUmacInstance->gVars.p.p2pPsMode.count = noaAttribute.count;
				pUmacInstance->gVars.p.p2pPsMode.DtimCount = 0;
				pUmacInstance->gVars.p.p2pPsMode.duration = noaAttribute.duration;
				pUmacInstance->gVars.p.p2pPsMode.interval = noaAttribute.interval;
				pUmacInstance->gVars.p.p2pPsMode.startTime = noaAttribute.startTime;
				pUmacInstance->gVars.p.p2pPsMode.reserved = 0;
				pUmacInstance->p2pPsSet = TRUE;
		} else
			pUmacRunTime->updateNoaAttrib = FALSE;
	} /* if (ieElement.size) */
#endif	/* P2P_MINIAP_SUPPORT */

	/*Since these MIB updations can happen independent of the current
	   handler we are in, we can skip the return status of the
	   following function here
	 */
	UMAC_ManageAsynMgmtOperations(UmacHandle);

} /* end WFM_UMAC_ScanIEListForUpdations() */

/******************************************************************************
 * NAME:	WFM_UMAC_ProcInternalScanRsp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes a prob rsp/beacon recieved by an internal scan
 * issued by the device.
 * On getting the information corresponding to the SSID its scanning for, this
 * function will terminate the ongoing scan and will re-initiate the join
 * procedure with the latest information available.
 * \param UmacHandle		- Handle to UMAC Insance.
 * \param *pBeaconFrameBody	- Beacon /prob response to be processed.
 * \returns uint16
 * WFM_BEACON_PROBRSP_INTERNAL_SCAN_ONGOING	- Internal scan, didnt find
 *						matching Device.
 * WFM_BEACON_PROBRSP_INTERNAL_SCAN_GOT_RESULT	- Internal Scan found the
 *						Matching Device
 *****************************************************************************/
uint16 WFM_UMAC_ProcInternalScanRsp(UMAC_HANDLE UmacHandle,
				    WFM_BSS_CACHE_INFO_IND *pBeaconFrameBody)
{
	IeElement Ie;
	IeElement SsidIe;
	WFM_BSS_LIST_SCAN *pbssListScan;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 encStatus;
	uint8 encStatusAp = UMAC_ENC_NO_ENCRYPTION;

	OS_ASSERT(pUmacInstance);

	pbssListScan = (WFM_BSS_LIST_SCAN *) &pUmacInstance->InternalScanMsg[0];
	encStatus = (uint8) (pbssListScan->Flags >> 10);

	if ((WFM_UMAC_ProcIeElements(pBeaconFrameBody->IEElements, pBeaconFrameBody->IELength, UMAC_IE_ELEMENT_SSID, &SsidIe) == TRUE)
		) {
		if (OS_MemoryEqual((void *)&pbssListScan->Ssids[0].Ssid, pbssListScan->Ssids[0].SsidLength, (void *)SsidIe.info, SsidIe.size)
		    ) {
			/* We found the AP we were looking for */
			WFM_BSS_INFO *pBssElement = (WFM_BSS_INFO *) &pUmacInstance->bssInfo[0];

			if (SsidIe.size) {
				if (!(pBeaconFrameBody->Capability & 0x0010))
					encStatusAp = UMAC_ENC_NO_ENCRYPTION;
				else {
					WFM_OUI_INFO OuiInfo = {
						WPA_OUI,
						0x01,
						WPA_IE_PARAMETER_ELEMENT_SUB_TYPE
					};
					WFM_UMAC_ProcVendorSpecificIeElements((uint8 *) &pBeaconFrameBody->IEElements[0], pBeaconFrameBody->IELength, &OuiInfo, &Ie);
					/*
					   Both WPA IE and RSN IE wont be present together
					 */
					if (Ie.size)
						encStatusAp = UMAC_ENC_TKIP_SUPPORTED;
					else {
						if (WFM_UMAC_ProcIeElements((uint8 *) &pBeaconFrameBody->IEElements[0], pBeaconFrameBody->IELength, UMAC_IE_ELEMENT_RSN, &Ie) == TRUE) {
							if (Ie.size)
								encStatusAp = UMAC_ENC_AES_SUPPORTED;
						}
							else
								encStatusAp = UMAC_ENC_WEP_SUPPORTED;
					}
				}

				if ((encStatusAp & encStatus) || (encStatusAp == encStatus)) {
					pUmacInstance->IsInternalScanSuccess = TRUE;
					pUmacInstance->StartScanInNextBand = FALSE;

					WFM_ProcessDeviceCacheInfo(pBeaconFrameBody, pBssElement, &SsidIe);

					return WFM_BEACON_PROBRSP_INTERNAL_SCAN_GOT_RESULT;
				}	/*if(encStatusAp == encStatus) */
				else
					return WFM_BEACON_PROBRSP_INTERNAL_SCAN_ONGOING;

			} /* if( Ie.size ) */
		}
	}

	return WFM_BEACON_PROBRSP_INTERNAL_SCAN_ONGOING;
} /* end WFM_UMAC_ProcInternalScanRsp() */

/******************************************************************************
 * NAME:	WFM_ProcessDeviceCacheInfo
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the beacons/ prob responses and updates them in the
 * WFM cache.
 * \param *wfm_bss_cache_info_ind   - Beacon /prob response to be processed.
 * \param *pBssElement		    - The WFM_BSS_INFO structre where the beacon
 *				      information should be copied
 * \param *pIelement		    - Pointer to the information element
 *                                    containing SSID info.
 * \returns none.
 *****************************************************************************/
void WFM_ProcessDeviceCacheInfo(WFM_BSS_CACHE_INFO_IND *wfm_bss_cache_info_ind,
				WFM_BSS_INFO *pBssElement,
				IeElement *pIelement)
{
	uint16 sizeofBssElement = sizeof(WFM_BSS_INFO);
	IeElement Ielement = { 0 };
	uint8 *pSupportedRates = NULL;
	uint16 NumSupportedRates = 0;
	uint16 NumExtSupportedRates = 0;
	WFM_BEACON_CAPABILITY WfmBeaconCapability;

	OS_ASSERT(wfm_bss_cache_info_ind);

	if (WFM_UMAC_ProcIeElements((uint8 *) &wfm_bss_cache_info_ind->IEElements[0], wfm_bss_cache_info_ind->IELength, UMAC_IE_ELEMENT_SUPPORTED_RATES, &Ielement)) {

		NumSupportedRates = Ielement.size;

		if (NumSupportedRates)
			/*
			   Keeping the pointer to Supported rates, so that
			   we can copy it later when the  buffer is allocated
			 */
			pSupportedRates = Ielement.info;
	}

	/*Getting the extended supported rates */
	if (WFM_UMAC_ProcIeElements((uint8 *) &wfm_bss_cache_info_ind->IEElements[0], wfm_bss_cache_info_ind->IELength, UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES, &Ielement)) {

		NumExtSupportedRates = Ielement.size;

		sizeofBssElement = (uint16) (sizeofBssElement + NumSupportedRates + (NumExtSupportedRates - WFM_BSS_INFO_SUPPORTED_RATE_SIZE));

		OS_MemoryReset(pBssElement, sizeofBssElement);

		pBssElement->NumRates = NumSupportedRates + NumExtSupportedRates;

		if (NumSupportedRates) {
			OS_MemoryCopy(
			(void *)(uint8 *) &pBssElement->SupportedRates[0],
			pSupportedRates,
			NumSupportedRates
			);
		}

		if (NumExtSupportedRates) {
			OS_MemoryCopy(
				&pBssElement->SupportedRates[NumSupportedRates],
				Ielement.info,
				NumExtSupportedRates
				);
		}
	} else if (NumSupportedRates) {
		pBssElement->NumRates  =  NumSupportedRates;
		OS_MemoryCopy(
			&pBssElement->SupportedRates[0],
			Ielement.info,
			NumSupportedRates
			);
	}

	OS_MemoryCopy(
		pBssElement->MacAddress,
		wfm_bss_cache_info_ind->BssId,
		WFM_MAC_ADDRESS_SIZE
		);

	pBssElement->RCPI = wfm_bss_cache_info_ind->RCPI;
	pBssElement->TSF = wfm_bss_cache_info_ind->TimeStamp;
	pBssElement->Rssi = wfm_bss_cache_info_ind->RSSI;
	pBssElement->BeaconPeriod = wfm_bss_cache_info_ind->BeaconInterval;
	/*
	   If the beacon doesnt have a DS parameter Set IE element, use the
	   channel on which we recieved this beacon, as the channel of
	   operation of the AP.
	 */
	pBssElement->ChannelNumber = wfm_bss_cache_info_ind->ChannelNumber;

	pBssElement->Capabilities = wfm_bss_cache_info_ind->Capability;

	*((uint16 *) &WfmBeaconCapability) = wfm_bss_cache_info_ind->Capability;
	if ((WfmBeaconCapability.ESS == 1)
	    && (WfmBeaconCapability.IBSS == 0)
	    ) {
		/* This is an ACCESS POINT */
		pBssElement->InfrastructureMode = WFM_802_INFRASTRUCTURE;
	} else if ((WfmBeaconCapability.ESS == 0)
		   && (WfmBeaconCapability.IBSS == 1)
	    ) {
		/* This is an Adhoc network */
		pBssElement->InfrastructureMode = WFM_802_IBSS;
	} else {

		pBssElement->InfrastructureMode = WFM_802_AUTO_UNKNOWN;
		return;
	}

	if (pIelement->size) {
		OS_MemoryCopy(pBssElement->Ssid, pIelement->info, pIelement->size);

		pBssElement->SsidLength = pIelement->size;

	} /* if( pIelement->size ) */

	pBssElement->Flags = 0;	/* initializing it */

	if (WFM_UMAC_ProcIeElements((uint8 *) &wfm_bss_cache_info_ind->IEElements[0], wfm_bss_cache_info_ind->IELength, UMAC_IE_ELEMENT_ERP_INFORMATION, &Ielement)) {
		if (Ielement.size) {
			/*
			   As per the current standard, ERP element has only
			   one byte information in it. The following code
			   should be changed when there is a change to the
			   information element.
			 */
			uint8 ErpInfo = (uint8) *Ielement.info;

			if (ErpInfo & 0x01)
				/*Non-ERP bit set */
				pBssElement->Flags |= WFM_BSS_INFO_FLAGS_NON_ERP_AP;

			if (ErpInfo & 0x02)
				/*Protection enabled */
				pBssElement->Flags |= WFM_BSS_INFO_FLAGS_PROT_ENABLED;

			if (!(ErpInfo & 0x04)) {
				/*Short Preamble supported */
				pBssElement->Flags |= WFM_BSS_INFO_FLAGS_SUPP_SHORT_PREAMBLE;
			}	/*if ( ErpInfo & 0x04 ) */

		}		/*if( Ielement.size ) */
	}


	if ((WFM_UMAC_ProcIeElements((uint8 *) &wfm_bss_cache_info_ind->IEElements[0], wfm_bss_cache_info_ind->IELength, UMAC_IE_ELEMENT_TIM, &Ielement) == TRUE)
	    && (Ielement.size)) {
		pBssElement->DTIMPeriod = (uint8) Ielement.info[1];
	} /* if( WFM_UMAC_ProcIeElements */

	if (WFM_UMAC_ProcIeElements((uint8 *) &wfm_bss_cache_info_ind->IEElements[0], wfm_bss_cache_info_ind->IELength, UMAC_IE_ELEMENT_DS_PARAM, &Ielement)) {
		if (Ielement.size) {
			pBssElement->DSConfig = (uint8) (*Ielement.info);

			/*
			   If the beacon has DS paramter set in it, extract
			   channel information from here and use it.
			 */
			pBssElement->ChannelNumber = pBssElement->DSConfig;
		} /* if( Ielement.size ) */
	}

	/*
	   Importent Note:
	   The following IE's (HT Capabilities and HT Information ) needs to
	   be handled carefully. Each of the members needs to be copied
	   explicitly as the structue is slightly different from the spec;
	   for handling byte padding.
	 */

	if (WFM_UMAC_ProcIeElements((uint8 *) &wfm_bss_cache_info_ind->IEElements[0], wfm_bss_cache_info_ind->IELength, UMAC_IE_ELEMENT_HT_CAPABILITIES, &Ielement)) {

		if (Ielement.size) {

			/*
			   AP support 11n rates, so setting the flags for 11n
			   mode support and 11n rate support.
			 */
			pBssElement->Flags |= WFM_BSS_INFO_FLAGS_11N_CAPABLE | WFM_BSS_INFO_FLAGS_SUPP_11N_RATES;	/*Hard coding for the time being */

			OS_MemoryCopy((void *)&pBssElement->HtCapabilities.CapabilitiesInfo, Ielement.info, 2);

			pBssElement->HtCapabilities.AMPDU_Params = *(Ielement.info + 2);

			OS_MemoryCopy((void *)&pBssElement->HtCapabilities.SupportedMCSSet, (Ielement.info + 3), 16);

			OS_MemoryCopy((void *)&pBssElement->HtCapabilities.ExtendedCapabilities, (Ielement.info + 18), 2);

			OS_MemoryCopy((void *)&pBssElement->HtCapabilities.TxBeamFormingCapabilities, (Ielement.info + 20), 4);

			pBssElement->HtCapabilities.ASEL_Capabilities = (uint8) *(Ielement.info + 24);

		} /* if( Ielement.size ) */
	}


	{

		WFM_OUI_INFO OuiInfo = {
			WMM_OUI,
			WMM_IE_ELEMENT_TYPE,
			WFM_IE_PARAMETER_ELEMENT_SUB_TYPE
		};

		WFM_UMAC_ProcVendorSpecificIeElements(
			(uint8 *) &wfm_bss_cache_info_ind->IEElements[0],
			wfm_bss_cache_info_ind->IELength,
			&OuiInfo,
			&Ielement
			);

		if (Ielement.size) {
			/*
			   This Device is WMM capable, If the Host is not
			   interested in WMM, Host should reset this bit.
			 */
			pBssElement->Flags |= WFM_BSS_INFO_FLAGS_BSS_WMM_CAP;

			OS_MemoryCopy((void *)&pBssElement->WmmParamElement, Ielement.info, Ielement.size);

		} /* if( Ielement.size ) */
	}


	{

		WFM_OUI_INFO OuiInfo = {
			WPA_OUI,
			0x01,
			WPA_IE_PARAMETER_ELEMENT_SUB_TYPE
		};

		/*
		   Both WPA IE and RSN IE wont be present together
		 */

		if (WFM_UMAC_ProcIeElements((uint8 *) &wfm_bss_cache_info_ind->IEElements[0], wfm_bss_cache_info_ind->IELength, UMAC_IE_ELEMENT_RSN, &Ielement)) {

			if (Ielement.size) {

				OS_MemoryCopy((void *)&pBssElement->Wpa2ParamElement.ParamInfo[0], Ielement.info, Ielement.size);

				pBssElement->Wpa2ParamElement.Length = Ielement.size;

				pBssElement->Flags |= WFM_BSS_INFO_FLAGS_SUPP_WPA2;

			}	/*if( Ielement.size ) */
		}

		WFM_ProcVendorSpecificIeElementsWPA(
			(uint8 *) &wfm_bss_cache_info_ind->IEElements[0],
			wfm_bss_cache_info_ind->IELength,
			&OuiInfo,
			&Ielement
			);

		if (Ielement.size) {

			OS_MemoryCopy(
				(void *)&pBssElement->WpaParamElement.ParamInfo[0],
				Ielement.info,
				Ielement.size
				);

			pBssElement->WpaParamElement.Length = Ielement.size;

			pBssElement->Flags |= WFM_BSS_INFO_FLAGS_SUPP_WPA;

		} /* if( Ielement.size ) */
	}

#if WAPI_SUPPORT
	if (WFM_UMAC_ProcIeElements((uint8 *) &wfm_bss_cache_info_ind->IEElements[0], wfm_bss_cache_info_ind->IELength, UMAC_IE_ELEMENT_WAPI, &Ielement)) {

		if (Ielement.size) {

			OS_MemoryCopy(
				(void *)&pBssElement->WapiParamElement.ParamInfo[0],
				Ielement.info,
				Ielement.size
				);

			pBssElement->WapiParamElement.Length = Ielement.size;

			pBssElement->Flags |= WFM_BSS_INFO_FLAGS_SUPP_WAPI;

		} /* if( Ielement.size ) */
	}
#endif	/* WAPI_SUPPORT */

	return;
} /* end WFM_ProcessDeviceCacheInfo() */

void WFM_ProcVendorSpecificIeElementsWPA(uint8 *IeElementStart,
						uint16 IELength,
						WFM_OUI_INFO *pOuiInfo,
						IeElement *pIelement)
{
	int IeLen = IELength;
	uint8 IeElementId = 0;
	uint8 IeElementSize = 0;

	OS_ASSERT(IeElementStart);

	OS_MemoryReset(pIelement, sizeof(IeElement));

	do {

		IeElementId = (uint8) *IeElementStart;
		IeElementSize = (uint8) *(IeElementStart + 1);

		if (IeElementId == UMAC_IE_ELEMENT_VENDOR_SPECIFIC) {
			if (OS_MemoryEqual((IeElementStart + 2), 4, (uint8 *) pOuiInfo, 4)) {
				pIelement->Id = IeElementId;
				pIelement->size = IeElementSize;
				pIelement->info = (uint8 *) (IeElementStart + 2);

				/*
				  Got what we were interested in, so break here
				*/
				break;
			}
		} /* if(IeElementId  ==  UMAC_IE_ELEMENT_VENDOR_SPECIFIC) */

		IeElementStart = (IeElementStart + IeElementSize + 2);
		IeLen = (IeLen - (IeElementSize + 2));

	} while ((IeLen > 0));

	return;
} /* end WFM_ProcVendorSpecificIeElements() */


/******************************************************************************
* NAME:	 WFM_GetPMKIDIndex
******************************************************************************/
/**
* \brief
* It checks whether the PMKID is available for a given BSSID
* \param UmacHandle	- Handle to UMAC Instance
* \param pBSSID		- Pointer to BSSID
* \param pIndex		- Pointer to index value
* \return True if PMKID is available else FALSE.
******************************************************************************/
uint8 WFM_GetPMKIDIndex(UMAC_HANDLE UmacHandle, uint8 *pBSSID, uint8 *pIndex)
{
	uint8 i = 0;
	uint8 nResult = FALSE;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	for (i = 0; i < WFM_MAX_BSSID_INFO_ENTRIES; i++) {
		if (OS_MemoryEqual(pBSSID, WFM_MAC_ADDRESS_SIZE, pUmacInstance->gVars.p.BssidPmkidBkid.bssidInfo[i].bssid, WFM_MAC_ADDRESS_SIZE)) {
			*pIndex = i;
			nResult = TRUE;
			break;
		}
	}
	return nResult;
}

/******************************************************************************
* NAME:	 WFM_GetChannelIndexInTable
******************************************************************************/
/**
* \brief
* It checks whether the given channel is available in Country String Info table
* maintained in pUmacInstance
* \param UmacHandle	- Handle to UMAC Instance
* \param ChannelNum	- Channel Number to check the availability
* \param PhyBand	- Phy Band
* \PowerLevel		- Pointer to Power Level
* \return True if channel is available in Table and its power level.
******************************************************************************/
uint8 WFM_GetChannelIndexInTable(UMAC_HANDLE UmacHandle,
				 uint32 ChannelNum,
				 uint8 PhyBand,
				 sint32 *PowerLevel)
{
	uint8 ChannelAvail = FALSE;
	uint8 TotalChannels = 0;
	uint8 count = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	TotalChannels = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->gVars.p.InfoHost.ScanPhyBand].totalNumOfCh;
	for (count = 0; count < TotalChannels; count++) {
		if (ChannelNum == pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->gVars.p.InfoHost.ScanPhyBand].channels[count]) {
			/*Channel found in the table */
			ChannelAvail = TRUE;
			*PowerLevel = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[pUmacInstance->gVars.p.InfoHost.ScanPhyBand].powerLevel[count];
			break;
		}
	}
	return ChannelAvail;
}

/******************************************************************************
* NAME:	 WM_UMAC_GetCountryCode
*----------------------------------------------------------------------------*/
/**
* \brief
* It gets the region code corresponding to Country String
* \param pCountryStr - Pointer to Country String.
* \param pRegionCode - Pointer to region code.
* \return uint8 - Region Code
******************************************************************************/
uint8 WFM_UMAC_ConvertCountryStrToRegionCode(uint8 *pCountryStr,
					     uint8 *pRegionCode)
{
	int ii = 0;
	uint8 bRegionCodeAvl = FALSE;
	CountryStr_RegionCode_Mapping *pCountryRegionTable = NULL;
	if (!pCountryStr) {
		/*Return Country Code as 0 */
		OS_ASSERT(pCountryStr);
		return 0;
	}
	pCountryRegionTable = (CountryStr_RegionCode_Mapping *) &DefaultCountryRegionTableBuffer[0];
	for (ii = 0; pCountryStr[ii] && ii < WFM_CONFIG_COUNTRY_CODE_LENGTH; ii++) {
		if (pCountryStr[ii] >= 'a' && pCountryStr[ii] <= 'z') {	/* toupper */
			pCountryStr[ii] += (uint8) ('A' - 'a');
		}
	}
	for (ii = 0; ii < WFM_CONFIG_MAX_COUNTRY /*128 */ ; ii++) {
		if (OS_MemoryEqual(pCountryStr, (uint32) WFM_CONFIG_COUNTRY_CODE_LENGTH, pCountryRegionTable[ii].CountryStr, (uint32) WFM_CONFIG_COUNTRY_CODE_LENGTH)) {
			bRegionCodeAvl = TRUE;
			*pRegionCode = pCountryRegionTable[ii].RegionCode;
			break;
		}
	}
	/* default is Other, region code is 0 */
	return bRegionCodeAvl;
}

/******************************************************************************
* NAME:	 WFM_IsChannelNotSupported
******************************************************************************/
/**
* \brief
* It checks whether the given channel is supported or not by our device.
* \param ChannelNo	- Channel Number
* \param ChannelIndex	- Channel Index
* \return TRUE if channel is not supported else FALSE
******************************************************************************/
uint8 WFM_IsChannelNotSupported(uint8 ChannelNo, uint8 ChannelIndex)
{
	uint8 StatusCode = FALSE;
	uint8 *pChannelList = NULL;
	uint8 i = 0;
	pChannelList = (uint8 *) &ChannelSupportList[0];
	while (i < WFM_CHANNEL_SUPPORT_LIST_SIZE) {
		if ((pChannelList[i] == ChannelIndex) && (pChannelList[i + 1] == ChannelNo)) {
			StatusCode = TRUE;
			break;
		}
		i += 2;
	}
	return StatusCode;
}

/******************************************************************************
* NAME:	 UMAC_PutCountryIE
******************************************************************************/
/**
* \brief
* This function puts the country IE.
* \param UmacHandle - Handle to the UMAC Instance
* \param ptr - Pointer where IE is put
* \return WFM_STATUS_CODE
******************************************************************************/
uint8 *UMAC_PutCountryIE(UMAC_HANDLE UmacHandle, uint8 *ptr)
{
	uint8 RegionCode = 0;
	uint8 *pTemp = NULL;
	uint8 TripletCount;
	uint8 phyBand;
	uint8 CountryString[4];
	uint8 bRegionCodeAvl = 0, i = 0, j = 0;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	phyBand  = pUmacInstance->Band;
	if (phyBand == PHY_BAND_2G)
		pTemp = (uint8 *) &DefaultConfigRegionTable_2_4G[0];
	else
		pTemp = (uint8 *) &DefaultConfigRegionTable_5G[0];
	OS_MemoryCopy(&CountryString[0],
		pUmacInstance->gVars.p.InfoHost.CountryStringInfo[phyBand].countryString,
		WFM_DOT11_COUNTRY_STRING_SIZE);
	CountryString[2] = ' ';
	CountryString[3] = '\0';
	bRegionCodeAvl =
		WFM_UMAC_ConvertCountryStrToRegionCode(&CountryString[0],
		&RegionCode);
	if (!bRegionCodeAvl)
		goto exit_handler;

	while (i < WFM_REGION_CONFIG_TABLE_SIZE - 1) {
		if (pTemp[i] == RegionCode) {
			i++;
			TripletCount = pTemp[i];
			*ptr++ = UMAC_IE_ELEMENT_COUNTRY;
			*ptr++ = WFM_DOT11_COUNTRY_STRING_SIZE +
				(TripletCount * 3);
			OS_MemoryCopy(ptr, &CountryString[0], WFM_DOT11_COUNTRY_STRING_SIZE);
			ptr += 3;
			for (j = 0; j < TripletCount && i < WFM_REGION_CONFIG_TABLE_SIZE - 4; j++) {
				i++;
				*ptr++ = pTemp[i];
				i++;
				*ptr++ = pTemp[i];
				i++;
				*ptr++ = pTemp[i];
				i++;
			}
			break;
		} else {
			i++;
			TripletCount = pTemp[i];
			i += (4 * TripletCount) + 1;
		}
	}
exit_handler:
	return ptr;

}

/******************************************************************************
* NAME:	 WFM_UMAC_FetchCountryStringInfoFromTable
******************************************************************************/
/**
* \brief
* It updates the country string Info from the table based on Country string.
* \param UmacHandle - Handle to the UMAC Instance
* \param pdot11CountryString - Pointer to Country String
* \param PhyBand	     - 2.4Ghz or 5Ghz
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE WFM_UMAC_FetchCountryStringInfoFromTable(UMAC_HANDLE UmacHandle,
							 uint8 *pdot11CountryString,
							 uint8 PhyBand)
{
	WFM_STATUS_CODE StatusCode = WFM_STATUS_SUCCESS;
	uint8 IsChannelNotSupported = FALSE;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 *pTemp = NULL;
	uint8 TripletCount;
	uint8 i = 0, j = 0;
	uint8 FirstChannelNumber = 0, numOfChannels = 0;
	uint8 MaxTransmitPower = 0, startIndex = 0;
	uint16 ChannelOffset = 0;
	uint8 bRegionCodeAvl = FALSE;
	uint8 ChannelNotSupported = 0;
	uint16 count = 0;
	uint8 iIndex = 0;
	uint8 RegionCode = 0;
	/*Find the region code */
	bRegionCodeAvl = WFM_UMAC_ConvertCountryStrToRegionCode(pdot11CountryString, &RegionCode);
	if (!bRegionCodeAvl) {
		StatusCode = WFM_STATUS_COUNTRY_NOT_FOUND_IN_TABLE;
		goto exit_handler;
	}
	if (PhyBand == PHY_BAND_2G) {
		pTemp = (uint8 *) &DefaultConfigRegionTable_2_4G[0];
	} else if (PhyBand == PHY_BAND_5G) {
		/*Here assign the pointer to 5Ghz table TBC */
		pTemp = (uint8 *) &DefaultConfigRegionTable_5G[0];
	} else {
		StatusCode = WFM_STATUS_BAD_PARAM;
		goto exit_handler;
	}
	/* Store Country String */
	OS_MemoryCopy(
		pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PhyBand].countryString,
		pdot11CountryString,
		WFM_DOT11_COUNTRY_STRING_SIZE
		);

	/*
	  Parse the table for the region code and update the triplets in
	  global structure
	*/
	while (i < WFM_REGION_CONFIG_TABLE_SIZE - 1) {
		if (pTemp[i] == RegionCode) {
			i++;
			TripletCount = pTemp[i];
			for (j = 0; j < TripletCount && i < WFM_REGION_CONFIG_TABLE_SIZE - 4; j++) {
				i++;
				FirstChannelNumber = pTemp[i];
				i++;
				numOfChannels = pTemp[i];
				i++;
				MaxTransmitPower = pTemp[i];
				i++;
				ChannelOffset = Arr_Channel_Offset[pTemp[i]];
				startIndex = pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PhyBand].totalNumOfCh;
				for (count = startIndex; count < (numOfChannels + startIndex); count++) {
					/* Update channel numbers */
					IsChannelNotSupported = WFM_IsChannelNotSupported(FirstChannelNumber + iIndex, pTemp[i]);
					if (!IsChannelNotSupported) {
						pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PhyBand].channels[count] = FirstChannelNumber + iIndex + ChannelOffset;
						pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PhyBand].powerLevel[count] = MaxTransmitPower * WFM_UMAC_POWER_LEVEL_FACTOR;
					} else {
						ChannelNotSupported++;
					}
					if (PhyBand == PHY_BAND_2G)
						iIndex = iIndex + 1;
					else
						iIndex = iIndex + 4;
				}
				/*Update total channel number */
				pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PhyBand].totalNumOfCh += numOfChannels;
				iIndex = 0;
			}
			pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PhyBand].totalNumOfCh -= ChannelNotSupported;
			break;
		} else {
			i++;
			TripletCount = pTemp[i];
			i += (4 * TripletCount) + 1;
		}
	}
 exit_handler:
	return StatusCode;
}

/******************************************************************************
* NAME:	 WFM_UMAC_Process_CountryIe
******************************************************************************/
/**
* \brief
* This function processes Country Ie.
* \param UmacHandle	- Handle to the UMAC Instance
* \param pCountryInfoIe - Pointer to Start of Country information element.
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE WFM_UMAC_Process_CountryIe(UMAC_HANDLE UmacHandle,
					   IeElement *pCountryInfoIe)
{
	uint8 *pInfo = NULL;
	uint8 PhyBand = PHY_BAND_2G;
	uint8 CountryString[4];
	WFM_STATUS_CODE StatusCode = WFM_STATUS_SUCCESS;
	pInfo = pCountryInfoIe->info;
	OS_MemoryCopy(CountryString, pInfo, 3);
	CountryString[2] = ' ';
	CountryString[3] = '\0';
	/*
	  Update the channels and transmit power based on country string from
	  the Table
	*/
	StatusCode = WFM_UMAC_FetchCountryStringInfoFromTable(UmacHandle, CountryString, PhyBand);
	PhyBand = PHY_BAND_5G;
	StatusCode = WFM_UMAC_FetchCountryStringInfoFromTable(UmacHandle, CountryString, PhyBand);
	return StatusCode;
}

/******************************************************************************
* NAME:	 WFM_UMAC_UpdateRegulatoryDomain
******************************************************************************/
/**
* \brief
* It updates the regulatory domain table available in pUmacInstance
* \param UmacHandle	 - Handle to the UMAC Instance
* \param pRegDomainTable - Pointer to regDomainTable
* \param regDomainLen	 - Regulatory Domain length
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE WFM_UMAC_UpdateRegulatoryDomain(UMAC_HANDLE UmacHandle,
						UMAC_REGULATORY_DOMAIN_TABLE *pRegDomainTable,
						uint16 regDomainLen)
{
	WFM_STATUS_CODE StatusCode = WFM_STATUS_SUCCESS;
	/*Update the pUmacinstance accordingly */
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 i = 0;
	UMAC_REGULATORY_DOMAIN_TABLE *pRegDT = &pUmacInstance->gVars.p.RegDomainTable;
	OS_MemoryReset(pRegDT, sizeof(UMAC_REGULATORY_DOMAIN_TABLE));
	OS_MemoryCopy(&pRegDT->countryRegDetails.countryString[0],
		(uint8 *) &pRegDomainTable->countryRegDetails.countryString[0],
		WFM_DOT11_COUNTRY_STRING_SIZE);
	pRegDT->countryRegDetails.noOfRegClass =
		pRegDomainTable->countryRegDetails.noOfRegClass;
	for (i = 0 ; i < pRegDT->countryRegDetails.noOfRegClass ; i++) {
		pRegDT->countryRegDetails.regClassMap[i].txPowerLevel =
			pRegDomainTable->countryRegDetails.regClassMap[i].txPowerLevel;
		pRegDT->countryRegDetails.regClassMap[i].channelOffset =
			pRegDomainTable->countryRegDetails.regClassMap[i].channelOffset;
		pRegDT->countryRegDetails.regClassMap[i].RegClass =
			pRegDomainTable->countryRegDetails.regClassMap[i].RegClass;
		pRegDT->countryRegDetails.regClassMap[i].noOfTriplets =
			pRegDomainTable->countryRegDetails.regClassMap[i].noOfTriplets;
		OS_MemoryCopy(&pRegDT->countryRegDetails.regClassMap[i].channelTriplet[0],
			&pRegDomainTable->countryRegDetails.regClassMap[i].channelTriplet[0],
			(pRegDT->countryRegDetails.regClassMap[i].noOfTriplets * sizeof(UMAC_CHANNEL_TRIPLET)));
	}
	return StatusCode;
}

/******************************************************************************
* NAME:	 UMAC_CheckRegClassAndChannelNum
******************************************************************************/
/**
* \brief
* It checks the channel number and regulatory class
* \param UmacHandle	- Handle to the UMAC Instance
* \param RegClass	- Regulatory Class
* \param ChannelNum	- channel number
* \param channelOffset	- Pointer to Channel Offset
* \param txPowerLevel	- POinter to txPower level
* \return TRUE if channel and regulatory class is found successfully else FALSE
******************************************************************************/
uint8 UMAC_CheckRegClassAndChannelNum(UMAC_HANDLE UmacHandle,
				      uint8 RegClass,
				      uint8 ChannelNum,
				      uint16 *channelOffset,
				      uint32 *txPowerLevel)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 bCountryFound = FALSE;


	if (OS_MemoryEqual(pUmacInstance->gVars.p.InfoHost.CountryStringInfo[PHY_BAND_2G].countryString,
		WFM_DOT11_COUNTRY_STRING_SIZE,
		pUmacInstance->gVars.p.RegDomainTable.countryRegDetails.countryString,
		WFM_DOT11_COUNTRY_STRING_SIZE)) {
		bCountryFound = TRUE;
	}

	if (bCountryFound) {
		uint8 noOfRegClass = 0, j = 0;
		noOfRegClass = pUmacInstance->gVars.p.RegDomainTable.countryRegDetails.noOfRegClass;
		for (j = 0; j < noOfRegClass; j++) {
			if (RegClass == pUmacInstance->gVars.p.RegDomainTable.countryRegDetails.regClassMap[j].RegClass) {
				/*Check if the channel number is correct */
				uint8 k = 0;
				UMAC_REG_CLASS_MAP *pRegClassMap = &pUmacInstance->gVars.p.RegDomainTable.countryRegDetails.regClassMap[j];

				uint16 currentChannelOffset = pRegClassMap->channelOffset;
				for (k = 0; k < pRegClassMap->noOfTriplets; k++) {
					uint16 firstChannel = pRegClassMap->channelTriplet[k].firstChannel;
					uint16 noOfChannels = pRegClassMap->channelTriplet[k].noOfChannels;
					uint32 channelSpacing = pRegClassMap->channelTriplet[k].channelSpacing;
					uint8 count = 0;
					for (count = 0; count < noOfChannels; count++) {
						if (ChannelNum == (firstChannel + (channelSpacing * count))) {
							/*Channel found successfully */
							*channelOffset = currentChannelOffset;
							*txPowerLevel = pRegClassMap->txPowerLevel;
							return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}

#if DOT11K_SUPPORT
/******************************************************************************
* NAME:	 UMAC_SendChannelReport
******************************************************************************/
/**
* \brief
* This function sends the channel report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendChannelReport(UMAC_HANDLE UmacHandle)
{
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq = NULL;
	WFM_UMAC_MGMT_FRAME *pMgmtActionFrame = NULL;
	uint8 *pos = NULL;
	uint8 *posRef = NULL;
	UMAC_RADIO_MEAS_REPORT_BODY *pRadioMeasReport = NULL;
	/*UMAC_MEAS_CHANNEL_LOAD_RESULTS *pChannelLoadResult = NULL; */
	WSM_MEAS_CHANNEL_LOAD_RESULTS *pChannelLoadResult = NULL;
	CHANNEL_LOAD_REQ *pChannelLoadReq = NULL;

	uint16 FrameBodyLen = 0;
	uint8 Encrypt;

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
	WFM_UMAC_CreateDot11FrameHdr(UmacHandle, WFM_UMAC_MGMT, WFM_UMAC_MGMT_ACTION, &pMgmtActionFrame->dot11hdr, pUmacInstance->RemoteMacAdd, pUmacInstance->MacAddress, 0, Encrypt);
	pChannelLoadResult = (WSM_MEAS_CHANNEL_LOAD_RESULTS *) &pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.ChannelLoadResults;
	pChannelLoadReq = (CHANNEL_LOAD_REQ *) &pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;
	/*Now fill the frame body and send it to DIL layer */
	pRadioMeasReport = (UMAC_RADIO_MEAS_REPORT_BODY *) pMgmtActionFrame->FrameBody;
	/*Radio Measurement Report */
	pRadioMeasReport->category = UMAC_CATEGORY_RADIO_MEASUREMENT;
	pRadioMeasReport->action = UMAC_ACTION_TYPE_RADIO_MEASUREMENT_REPORT;
	pRadioMeasReport->dialogToken = pUmacInstance->dot11k_Meas_Params.dialogToken;
	/*Measurement Report Element */
	pRadioMeasReport->ElementID = UMAC_IE_ELEMENT_MEASUREMENT_REPORT;
	/*Meas Token + ReportMode + Type + ReportLength */
	pRadioMeasReport->length = 3;
	pRadioMeasReport->measurementToken = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementToken;
	pRadioMeasReport->measurementType = UMAC_MEASUREMENT_REPORT_TYPE_CHANNEL_LOAD;
	if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == NORMAL) {
		pRadioMeasReport->measurementReportMode = 0;

		/*Fill the report only in case of normal option */
		pos = (uint8 *) (pRadioMeasReport + 1);
		posRef = pos;
		/*Regulatory class */
		*pos++ = pChannelLoadReq->regulatoryClass;
		/*Channel Number */
		*pos++ = pChannelLoadReq->channelNum;
		/*Measurement Start Time */
		OS_MemoryCopy(pos, (uint8 *) &pChannelLoadResult->ActualMeasurementStartTime, 8);
		pos += 8;
		/*Measurement Duration */
		*(uint16 *) pos = pChannelLoadResult->MeasurementDuration;
		pos += 2;
		/*Channel Load */
		*pos++ = pChannelLoadResult->ChannelLoad;
		/*Upate length */
		pRadioMeasReport->length += (pos - posRef);
		/*Optional Subelements */
		FrameBodyLen = pos - pMgmtActionFrame->FrameBody;
	} else if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == INCAPABLE) {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_INCAPABLE;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	} else {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_REFUSED;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	}
	/*Fill pTxDesc->MsgLen */
	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));
	/*Get Packet ID */
	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendChannelReport() : Transmit Queue Full\n");
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

	UMAC_Store_Internal_TxDesc(UmacHandle, pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL);


	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);


	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*Sending the management frame to LMAC for transmission */
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
}

/******************************************************************************
* NAME:	 UMAC_SendNoiseHistReport
******************************************************************************/
/**
* \brief
* This function sends the noise histogram report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendNoiseHistReport(UMAC_HANDLE UmacHandle)
{
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq = NULL;
	WFM_UMAC_MGMT_FRAME *pMgmtActionFrame = NULL;
	uint8 *pos = NULL;
	uint8 *posRef = NULL;
	UMAC_RADIO_MEAS_REPORT_BODY *pRadioMeasReport = NULL;
	WSM_MEAS_NOISE_HISTOGRAM_RESULTS *pNoiseHistogramResult = NULL;
	NOISE_HISTOGRAM_REQ *pNoiseHistogrameReq = NULL;
	uint16 FrameBodyLen = 0;
	uint8 Encrypt;

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
	WFM_UMAC_CreateDot11FrameHdr(UmacHandle, WFM_UMAC_MGMT, WFM_UMAC_MGMT_ACTION, &pMgmtActionFrame->dot11hdr, pUmacInstance->RemoteMacAdd, pUmacInstance->MacAddress, 0, Encrypt);
	pNoiseHistogramResult = (WSM_MEAS_NOISE_HISTOGRAM_RESULTS *) &pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.NoiseHistogramResults;
	pNoiseHistogrameReq = (NOISE_HISTOGRAM_REQ *) &pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;

	/*Now fill the frame body and send it to DIL layer */
	pRadioMeasReport = (UMAC_RADIO_MEAS_REPORT_BODY *) pMgmtActionFrame->FrameBody;
	/*Radio Measurement Report */
	pRadioMeasReport->category = UMAC_CATEGORY_RADIO_MEASUREMENT;
	pRadioMeasReport->action = UMAC_ACTION_TYPE_RADIO_MEASUREMENT_REPORT;
	pRadioMeasReport->dialogToken = pUmacInstance->dot11k_Meas_Params.dialogToken;
	/*Measurement Report Element */
	pRadioMeasReport->ElementID = UMAC_IE_ELEMENT_MEASUREMENT_REPORT;
	/*Meas Token + ReportMode + Type + ReportLength */
	pRadioMeasReport->length = 3;
	pRadioMeasReport->measurementToken = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementToken;
	pRadioMeasReport->measurementType = UMAC_MEASUREMENT_REPORT_TYPE_NOISE_HISTOGRAM;
	if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == NORMAL) {
		uint8 anpi = 0;
		pRadioMeasReport->measurementReportMode = 0;
		/*Fill the report only in case of normal option */
		pos = (uint8 *) (pRadioMeasReport + 1);
		posRef = pos;
		/*Regulatory class */
		*pos++ = pNoiseHistogrameReq->regulatoryClass;
		/*Channel Number */
		*pos++ = pNoiseHistogrameReq->channelNum;
		/*Measurement Start Time */
		OS_MemoryCopy(pos, (uint8 *) &pNoiseHistogramResult->ActualMeasurementStartTime, 8);
		pos += 8;
		/*Measurement Duration */
		*(uint16 *) pos = pNoiseHistogramResult->MeasurementDuration;
		pos += 2;
		/*Antenna ID */
		*pos++ = pNoiseHistogramResult->AntennaID;
		/*Calculate ANPI from IPI densities */
		anpi = (uint8) ((pNoiseHistogramResult->PI_0_Density + pNoiseHistogramResult->PI_1_Density +
				 pNoiseHistogramResult->PI_2_Density + pNoiseHistogramResult->PI_3_Density +
				 pNoiseHistogramResult->PI_4_Density + pNoiseHistogramResult->PI_5_Density +
				 pNoiseHistogramResult->PI_6_Density + pNoiseHistogramResult->PI_7_Density + pNoiseHistogramResult->PI_8_Density + pNoiseHistogramResult->PI_9_Density + pNoiseHistogramResult->PI_10_Density) / 11);
		*pos = anpi;
		pos++;
		/*Copy all 11 densities */
		OS_MemoryCopy(pos, (uint8 *) &pNoiseHistogramResult->PI_0_Density, 11);
		pos += 11;
		/*Update the length */
		pRadioMeasReport->length += (pos - posRef);
		/*Optional Subelements */
		FrameBodyLen = pos - pMgmtActionFrame->FrameBody;
	} else if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == INCAPABLE) {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_INCAPABLE;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	} else {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_REFUSED;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	}
	/*Fill pTxDesc->MsgLen */
	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));
	/*Get Packet ID */
	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);
	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendNoiseHistReport() : Transmit Queue Full\n");
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

	UMAC_Store_Internal_TxDesc(UmacHandle,
				pTxDesc,
				pUmacInstance->gVars.umacTxDescInt,
				UMAC_NUM_INTERNAL_TX_BUFFERS,
				0,
				NULL);


	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);


	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*Sending the management frame to LMAC for transmission */
	Result = UMAC_LL_REQ_TX_FRAME(UmacHandle, pTxDesc->QueueId, pDevIfTxReq, DEFAULT_LINK_ID);
	if (Result != WFM_STATUS_SUCCESS)
		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);

exit_handler:
	return Result;
}

/******************************************************************************
* NAME:	 UMAC_SendBeaconReport
******************************************************************************/
/**
* \brief
* This function sends the beacon report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendBeaconReport(UMAC_HANDLE UmacHandle)
{
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq = NULL;
	WFM_UMAC_MGMT_FRAME *pMgmtActionFrame = NULL;
	uint8 *pos = NULL;
	uint8 *posRef = NULL;
	UMAC_RADIO_MEAS_REPORT_BODY *pRadioMeasReport = NULL;
	WSM_MEAS_BEACON_RESULTS *pBeaconResult = NULL;
	uint16 FrameBodyLen = 0;
	uint8 Encrypt;

	BEACON_REQ *pBeaconReq = NULL;

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
	WFM_UMAC_CreateDot11FrameHdr(UmacHandle, WFM_UMAC_MGMT, WFM_UMAC_MGMT_ACTION, &pMgmtActionFrame->dot11hdr, pUmacInstance->RemoteMacAdd, pUmacInstance->MacAddress, 0, Encrypt);

	pBeaconReq = (BEACON_REQ *) &pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;
	pBeaconResult = (WSM_MEAS_BEACON_RESULTS *) &pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.BeaconResults;
	/*Now fill the frame body and send it to DIL layer */
	pRadioMeasReport = (UMAC_RADIO_MEAS_REPORT_BODY *) pMgmtActionFrame->FrameBody;
	/*Radio Measurement Report */
	pRadioMeasReport->category = UMAC_CATEGORY_RADIO_MEASUREMENT;
	pRadioMeasReport->action = UMAC_ACTION_TYPE_RADIO_MEASUREMENT_REPORT;
	pRadioMeasReport->dialogToken = pUmacInstance->dot11k_Meas_Params.dialogToken;
	/*Measurement Report Element */
	pRadioMeasReport->ElementID = UMAC_IE_ELEMENT_MEASUREMENT_REPORT;
	/*Meas Token + ReportMode + Type + ReportLength */
	pRadioMeasReport->length = 3;	/*By default + pUmacInstance->measurementComplInd.MeasurementReportLength*//*TBC*/
	pRadioMeasReport->measurementToken = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementToken;
	pRadioMeasReport->measurementType = UMAC_MEASUREMENT_REPORT_TYPE_BEACON;
	if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == NORMAL) {
		uint32 Temp = 0;
		pRadioMeasReport->measurementReportMode = 0;
		/*Fill the report only in case of normal option */
		pos = (uint8 *) (pRadioMeasReport + 1);
		posRef = pos;
		/*Regulatory class */
		*pos++ = pBeaconReq->regulatoryClass;
		/*Channel Number Copying 1 byte only without offset. */
		*pos++ = (uint8)pBeaconReq->channelNum;
		/*Measurement Start Time */
		OS_MemoryCopy(pos, (uint8 *) &pBeaconResult->StartTsf, 8);
		pos += 8;
		/*Measurement Duration */
		*(uint16 *) pos = pBeaconResult->MeasurementDuration;
		pos += 2;
		/*Reported Frame information */
		*pos++ = 0; /*TBD*/
		/*Copy Rcpi,Rsni,Bssid,AntennaID */
		*pos++ = 0; /*TBD Get it from Beacon that is received*/
		*pos++ = 0;/*TBD Get it from beacon that is received*/
		OS_MemoryCopy(pos, (uint8 *)&pBeaconReq->bssid[0], 6);
		pos += 6;
		*pos++ = 0;/*TBD Get it from beacon that is received*/
		/*Copy Parent TSF */
		/*TBD Change this value from beacon*/
		OS_MemoryCopy(pos, (uint8 *) &Temp, 4);
		pos += 4;
		/*Upate the length */
		pRadioMeasReport->length += (pos - posRef);
		/*Optional Subelements */
		FrameBodyLen = pos - pMgmtActionFrame->FrameBody;
	} else if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == BEACON_TABLE) {
		WFM_BSS_CACHE_INFO_IND *pBssCacheInfo = NULL;

		pRadioMeasReport->measurementReportMode = 0;
		/*Call the UMi callback and then fill the beacon report */
		if (pUmacInstance->gVars.p.UpperLayerBeaconInfoCb)
			pBssCacheInfo = (WFM_BSS_CACHE_INFO_IND *) pUmacInstance->gVars.p.UpperLayerBeaconInfoCb(pUmacInstance->gVars.p.ulHandle, pBeaconReq->bssid, pUmacInstance->dot11k_Meas_Params.bssidIndex, &pUmacInstance->dot11k_Meas_Params.bssidCount);
		/*Fill the report */
		if (pBssCacheInfo) {
			/*Fill the report only in case of normal option */
			pos = (uint8 *) (pRadioMeasReport + 1);
			posRef = pos;
			/*Regulatory class */
			*pos++ = (uint8) 0xFF;
			/*Channel Number Copying 1 byte only without offset */
			*pos++ = (uint8) pBssCacheInfo->ChannelNumber;
			/*Measurement Start Time */
			OS_MemoryReset((uint8 *) pos, 8);
			pos += 8;
			/*Measurement Duration */
			*(uint16 *) pos = 0;
			pos += 2;
			/*Reported Frame information */
			*pos++ = 0xFF;
			/*Rcpi */
			*pos++ = pBssCacheInfo->RCPI;
			/*Rsni */
			*pos++ = pBssCacheInfo->RSSI;
			/*Bssid */
			OS_MemoryCopy(pos, (uint8 *) &pBssCacheInfo->BssId, 6);
			pos += 6;
			/*AntennaID */
			*pos++ = 0;
			/*Copy Parent TSF */
			*(uint32 *) pos = 0;
			pos += 4;
			/*Upate the length */
			pRadioMeasReport->length += (pos - posRef);
			/*Optional Subelements */
			FrameBodyLen = pos - pMgmtActionFrame->FrameBody;
		} else {
			/*Send Empty Report */ /*TBC*/
			    FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
		}
		/*We have got 1st beacon.Decrement the bssid count */
		if (pUmacInstance->dot11k_Meas_Params.bssidCount) {
			pUmacInstance->dot11k_Meas_Params.bssidCount--;
			pUmacInstance->dot11k_Meas_Params.bssidIndex++;
		} else {
			pUmacInstance->dot11k_Meas_Params.bssidIndex = 0;
		}
	} else if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == INCAPABLE) {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_INCAPABLE;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	} else if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == REFUSED) {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_REFUSED;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	} else {
		/*UMI_DbgPrint("Wrong CurrentReportOp Set\n"); */
	}
	/*Fill pTxDesc->MsgLen */
	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));
	/*Get Packet ID */
	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);
	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendBeaconReport() : Transmit Queue Full\n");
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
				NULL);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*Sending the management frame to LMAC for transmission */
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
}

/******************************************************************************
* NAME:	 UMAC_SendStatisticsReport
******************************************************************************/
/**
* \brief
* This function sends the Statistics report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendStatisticsReport(UMAC_HANDLE UmacHandle)
{
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq = NULL;
	WFM_UMAC_MGMT_FRAME *pMgmtActionFrame = NULL;
	uint8 *pos = NULL;
	uint8 *posRef = NULL;
	UMAC_RADIO_MEAS_REPORT_BODY *pRadioMeasReport = NULL;
	WSM_MEAS_STA_STATS_RESULTS *pStatsResult = NULL;
	uint16 FrameBodyLen = 0;
	uint8 Encrypt;

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

	pStatsResult = (WSM_MEAS_STA_STATS_RESULTS *) &pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.StaStatsResults;
	/*Now fill the frame body and send it to DIL layer */
	pRadioMeasReport = (UMAC_RADIO_MEAS_REPORT_BODY *) pMgmtActionFrame->FrameBody;
	/*Radio Measurement Report */
	pRadioMeasReport->category = UMAC_CATEGORY_RADIO_MEASUREMENT;
	pRadioMeasReport->action = UMAC_ACTION_TYPE_RADIO_MEASUREMENT_REPORT;
	pRadioMeasReport->dialogToken = pUmacInstance->dot11k_Meas_Params.dialogToken;
	/*Measurement Report Element */
	pRadioMeasReport->ElementID = UMAC_IE_ELEMENT_MEASUREMENT_REPORT;
	/*Meas Token + ReportMode + Type + ReportLength */
	pRadioMeasReport->length = 3;
	pRadioMeasReport->measurementToken = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementToken;
	pRadioMeasReport->measurementType = UMAC_MEASUREMENT_REPORT_TYPE_STA_STATISTICS;
	if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == NORMAL) {
		pRadioMeasReport->measurementReportMode = 0;
		/*Fill the report only in case of normal option */
		pos = (uint8 *) (pRadioMeasReport + 1);
		posRef = pos;
		/*Measurement Duration */
		*(uint16 *) pos = pStatsResult->MeasurementDuration;
		pos += 2;
		/*Group ID */
		*pos++ = pStatsResult->GroupId;
		/*Group Data */
		OS_MemoryCopy(pos, (uint8 *) pStatsResult->StatisticsGroupData, pStatsResult->StatisticsGroupDataLength);
		pos += pStatsResult->StatisticsGroupDataLength;
		/*Upate the length */
		pRadioMeasReport->length += (pos - posRef);
		/*Optional Subelements */
		FrameBodyLen = pos - pMgmtActionFrame->FrameBody;
	} else if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == INCAPABLE) {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_INCAPABLE;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	} else {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_REFUSED;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	}
	/*Fill pTxDesc->MsgLen */
	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));
	/*Get Packet ID */
	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendStatisticsReport() : Transmit Queue Full\n");
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pDevIfTxReq
					);
		Result = WFM_STATUS_OUT_OF_RESOURCES;
		goto exit_handler;
	}

	/*Fill the pTxDesc parameters */
	pTxDesc->MaxTxRate = (uint8) pUmacInstance->CurrentRateIndex;
	pTxDesc->QueueId = 0;
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
				NULL);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*Sending the management frame to LMAC for transmission */
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
}

/******************************************************************************
* NAME:	 UMAC_SendLCIReport
******************************************************************************/
/**
* \brief
* This function sends the LCI report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendLCIReport(UMAC_HANDLE UmacHandle)
{
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq = NULL;
	WFM_UMAC_MGMT_FRAME *pMgmtActionFrame = NULL;
	uint8 *pos = NULL;
	uint8 *posRef = NULL;
	UMAC_RADIO_MEAS_REPORT_BODY *pRadioMeasReport = NULL;
	uint16 FrameBodyLen = 0;
	uint8 Encrypt;

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

	/*Now fill the frame body and send it to DIL layer */
	pRadioMeasReport = (UMAC_RADIO_MEAS_REPORT_BODY *) pMgmtActionFrame->FrameBody;
	/*Radio Measurement Report */
	pRadioMeasReport->category = UMAC_CATEGORY_RADIO_MEASUREMENT;
	pRadioMeasReport->action = UMAC_ACTION_TYPE_RADIO_MEASUREMENT_REPORT;
	pRadioMeasReport->dialogToken = pUmacInstance->dot11k_Meas_Params.dialogToken;
	/*Measurement Report Element */
	pRadioMeasReport->ElementID = UMAC_IE_ELEMENT_MEASUREMENT_REPORT;
	/*Meas Token + ReportMode + Type + ReportLength */
	pRadioMeasReport->length = 3;
	pRadioMeasReport->measurementToken = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementToken;
	pRadioMeasReport->measurementType = UMAC_MEASUREMENT_REPORT_TYPE_LCI;
	if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == NORMAL) {
		pRadioMeasReport->measurementReportMode = 0;
		/*Fill the report only in case of normal option */
		pos = (uint8 *) (pRadioMeasReport + 1);
		posRef = pos;
		/*Element ID */
		OS_MemoryCopy(pos, &pUmacInstance->gVars.p.LciInfo.elementID, 1);
		pos += 1;
		/*Length */
		*pos = 16;
		pos++;
		/*latitude */
		OS_MemoryCopy(pos, &pUmacInstance->gVars.p.LciInfo.latitiude, 5);
		pos += 5;
		/*longitude */
		OS_MemoryCopy(pos, &pUmacInstance->gVars.p.LciInfo.longitude, 5);
		pos += 5;
		/*Altitude */
		OS_MemoryCopy(pos, &pUmacInstance->gVars.p.LciInfo.altitude, 5);
		pos += 5;
		OS_MemoryCopy(pos, &pUmacInstance->gVars.p.LciInfo.datum, 1);
		pos++;
		/*Upate the length */
		pRadioMeasReport->length += (pos - posRef);
		/*Optional Subelements */
		FrameBodyLen = pos - pMgmtActionFrame->FrameBody;

	} else if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == INCAPABLE) {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_INCAPABLE;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	} else {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_REFUSED;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	}
	/*Fill pTxDesc->MsgLen */
	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));
	/*Get Packet ID */
	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendLCIReport() : Transmit Queue Full\n");
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
				NULL);


	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*Sending the management frame to LMAC for transmission */
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
}

/******************************************************************************
* NAME:	 UMAC_SendTxStreamReport
******************************************************************************/
/**
* \brief
* This function sends the Link Measurement report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendTxStreamReport(UMAC_HANDLE UmacHandle)
{
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq = NULL;
	WFM_UMAC_MGMT_FRAME *pMgmtActionFrame = NULL;
	uint8 *pos = NULL;
	uint8 *posRef = NULL;
	UMAC_RADIO_MEAS_REPORT_BODY *pRadioMeasReport = NULL;
	uint16 FrameBodyLen = 0;
	TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ *pTxStreamReq = NULL;
	uint8 Encrypt;

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
	WFM_UMAC_CreateDot11FrameHdr(UmacHandle,
				WFM_UMAC_MGMT,
				WFM_UMAC_MGMT_ACTION,
				&pMgmtActionFrame->dot11hdr,
				pUmacInstance->RemoteMacAdd,
				pUmacInstance->MacAddress,
				0,
				Encrypt);
	pTxStreamReq = (TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ *) &pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;
	/*Now fill the frame body and send it to DIL layer */
	pRadioMeasReport = (UMAC_RADIO_MEAS_REPORT_BODY *) pMgmtActionFrame->FrameBody;
	/*Radio Measurement Report */
	pRadioMeasReport->category = UMAC_CATEGORY_RADIO_MEASUREMENT;
	pRadioMeasReport->action = UMAC_ACTION_TYPE_RADIO_MEASUREMENT_REPORT;
	pRadioMeasReport->dialogToken = pUmacInstance->dot11k_Meas_Params.dialogToken;
	/*Measurement Report Element */
	pRadioMeasReport->ElementID = UMAC_IE_ELEMENT_MEASUREMENT_REPORT;
	/*Meas Token + ReportMode + Type + ReportLength */
	pRadioMeasReport->length = 3;
	pRadioMeasReport->measurementToken = pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementToken;
	pRadioMeasReport->measurementType = UMAC_MEASUREMENT_REPORT_TYPE_TRANSMIT_STREAM;
	if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == NORMAL) {
		pRadioMeasReport->measurementReportMode = 0;
		/*Fill the report only in case of normal option */
		pos = (uint8 *) (pRadioMeasReport + 1);
		posRef = pos;
		/*Actual Measurement Start Time TBC */
		OS_MemoryCopy(pos, &pUmacInstance->dot11k_Meas_Params.txStreamResults.tsfTimer, 8);
		pos += 8;
		/*Measurement Duration */
		OS_MemoryCopy(pos, &pTxStreamReq->measurementDuration, 2);
		pos += 2;
		/*Peer STA address */
		OS_MemoryCopy(pos, &pTxStreamReq->peerSTAAddress, 6);
		pos += 6;
		 /*TID*/ *pos = pTxStreamReq->trafficIdentifier;
		pos++;
		/*Reporting Reason */
		*pos = 0;
		pos++;
		/*Transmitted MSDU Count */
		OS_MemoryCopy(pos, &pUmacInstance->dot11k_Meas_Params.txStreamResults.txMsduCount, 4);
		pos += 4;
		/*MSDU Discarded count */
		OS_MemoryCopy(pos, &pUmacInstance->dot11k_Meas_Params.txStreamResults.msduDiscardedCount, 4);
		pos += 4;
		/*MSDU Failed Count */
		OS_MemoryCopy(pos, &pUmacInstance->dot11k_Meas_Params.txStreamResults.msduFailedCount, 4);
		pos += 4;
		/*MSDU Multiple Retry Count */
		OS_MemoryCopy(pos, &pUmacInstance->dot11k_Meas_Params.txStreamResults.msduMultipleRetryCount, 4);
		pos += 4;
		/*Qos CF-Polls */
		*(uint32 *) pos = 0;
		pos += 4;
		/*Average Queue delay in TU */
		if (pUmacInstance->dot11k_Meas_Params.txStreamResults.totalPackets)
			*(uint32 *) pos = UMAC_Div_64(&pUmacInstance->dot11k_Meas_Params.txStreamResults.queueDelay, pUmacInstance->dot11k_Meas_Params.txStreamResults.totalPackets);
		else
			*(uint32 *) pos = 0;
		pos += 4;
		/*Average Transmit Delay in TU */
		if (pUmacInstance->dot11k_Meas_Params.txStreamResults.txMsduCount)
			*(uint32 *) pos = UMAC_Div_64(&pUmacInstance->dot11k_Meas_Params.txStreamResults.transmitDelay, pUmacInstance->dot11k_Meas_Params.txStreamResults.txMsduCount);
		else
			*(uint32 *) pos = 0;
		pos += 4;
		/*Bin 0 Reange */
		*pos = pTxStreamReq->binRange;
		pos++;
		/*Bin0 Range */
		*(uint32 *) pos = pUmacInstance->dot11k_Meas_Params.txStreamResults.bin0;
		pos += 4;
		/*Bin1 Range */
		*(uint32 *) pos = pUmacInstance->dot11k_Meas_Params.txStreamResults.bin1;
		pos += 4;
		/*Bin2 Range */
		*(uint32 *) pos = pUmacInstance->dot11k_Meas_Params.txStreamResults.bin2;
		pos += 4;
		/*Bin3 Range */
		*(uint32 *) pos = pUmacInstance->dot11k_Meas_Params.txStreamResults.bin3;
		pos += 4;
		/*Bin4 Range */
		*(uint32 *) pos = pUmacInstance->dot11k_Meas_Params.txStreamResults.bin4;
		pos += 4;
		/*Bin5 Range */
		*(uint32 *) pos = pUmacInstance->dot11k_Meas_Params.txStreamResults.bin5;
		pos += 4;
		/*Optional Subelements */
		/*Upate the length */
		pRadioMeasReport->length += (pos - posRef);
		FrameBodyLen = pos - pMgmtActionFrame->FrameBody;
	} else if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == INCAPABLE) {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_INCAPABLE;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	} else {
		pRadioMeasReport->measurementReportMode = MEAS_REPORT_MODE_REFUSED;
		FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	}
	/*Fill pTxDesc->MsgLen */
	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));
	/*Get Packet ID */
	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendTxStreamReport() : Transmit Queue Full\n");
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
				NULL);

	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*Sending the management frame to LMAC for transmission */
	Result = UMAC_LL_REQ_TX_FRAME(
				UmacHandle,
				pTxDesc->QueueId,
				pDevIfTxReq,
				DEFAULT_LINK_ID
				);

	if (Result != WFM_STATUS_SUCCESS)
		UMAC_ReleaseTxResources(UmacHandle, pTxDesc, DEFAULT_LINK_ID);
	/*As we have sent the request now we can reset the TxStream Results */
	OS_MemoryReset(
		&pUmacInstance->dot11k_Meas_Params.txStreamResults,
		sizeof(TANSMIT_STREAM_RESULTS)
		);

exit_handler:
	return Result;
}

/******************************************************************************
* NAME:	 UMAC_SendLinkMeasurementReport
******************************************************************************/
/**
* \brief
* This function sends the Link Measurement report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendLinkMeasurementReport(UMAC_HANDLE UmacHandle)
{
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq = NULL;
	WFM_UMAC_MGMT_FRAME *pMgmtActionFrame = NULL;
	uint8 *pos = NULL;
	WSM_MEAS_LINK_MEASUREMENT_RESULTS *pLinkMeasResult = NULL;
	UMAC_LINK_MEAS_REPORT_BODY *pLinkMeasReport = NULL;
	uint16 FrameBodyLen = 0;
	uint8 Encrypt;

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

	pLinkMeasResult = (WSM_MEAS_LINK_MEASUREMENT_RESULTS *) &pUmacInstance->dot11k_Meas_Params.measurementComplInd.MeasurementReport.LinkMeasurementResults;
	pLinkMeasReport = (UMAC_LINK_MEAS_REPORT_BODY *) pMgmtActionFrame->FrameBody;
	pos = (uint8 *) pLinkMeasReport;
	pLinkMeasReport->category = UMAC_CATEGORY_RADIO_MEASUREMENT;
	pLinkMeasReport->action = UMAC_ACTION_TYPE_LINK_MEASUREMENT_REPORT;
	pLinkMeasReport->dialogToken = pUmacInstance->dot11k_Meas_Params.LinkMeasuremetReq.dialogToken;

	/*Increment pos to 3 to feed the rest of values in report */
	pos += 3;
	/*TPC Report Element */
	*pos++ = UMAC_IE_TPC_REPORT;
	/*Length */
	*pos++ = 2;
	/*Transmit Power */
	*pos++ = (uint8) pLinkMeasResult->TransmitPower;
	/*Link Margin */
	*pos++ = 0; /*TBD we needto modify it after we know sensitivity value */
	/*Rx Antenna ID */
	*pos++ = pLinkMeasResult->RxAntennaID;
	/*Tx Antenna ID */
	*pos++ = pLinkMeasResult->TxAntennaID;
	 /*RCPI*/ *pos++ = pUmacInstance->dot11k_Meas_Params.linkMeasReqRCPI;
	 /*RSNI*/ *pos++ = 0xFF;	/*TBD calculating RSNI */
	if (pUmacInstance->dot11k_Meas_Params.CurrentReportOp == NORMAL)
		FrameBodyLen = pos - pMgmtActionFrame->FrameBody;
	/*
	  Changes required in 11k standard to send the incapable and refused
	  bit set report
	*/
	/* else if(pUmacInstance->dot11k_Meas_Params.CurrentReportOp == INCAPABLE)
	   {
	   FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	   }
	   else
	   {
	   FrameBodyLen = sizeof(UMAC_RADIO_MEAS_REPORT_BODY);
	   } */
	/*Fill pTxDesc->MsgLen */
	ASSIGN_FRAME_LENGTH(pTxDesc, FrameBodyLen + sizeof(WFM_UMAC_FRAME_HDR));
	/*Get Packet ID */
	UMAC_GET_PACKET_ID(UmacHandle, pTxDesc->PacketId);

	if (pTxDesc->PacketId == CFG_HI_NUM_REQS) {
		LOG_EVENT(DBG_WFM_ERROR, " UMAC_SendLinkMeasurementReport() : Transmit Queue Full\n");
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
				NULL);


	OS_LOCK(pUmacInstance->gVars.p.TxBufCountLock);
	pUmacInstance->NumTxBufPending[DEFAULT_LINK_ID]++;
	OS_UNLOCK(pUmacInstance->gVars.p.TxBufCountLock);

	ASSIGN_INTERNAL_DEV_IF_TX_REQ(pDevIfTxReq, pTxDesc);

	/*Sending the management frame to LMAC for transmission */
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
}

/******************************************************************************
* NAME:	 UMAC_Div_64
******************************************************************************/
/**
* \brief
* This function is used for 64bit divison.
* \param *pVal - Pointer to 64bit divison
* \param base - Value of divisor
* \return Quotient of divison
******************************************************************************/
uint32 UMAC_Div_64(uint64 *pVal, uint32 base)
{
	uint64 rem = *pVal;
	uint64 b = base;
	uint64 res, d = 1;
	uint32 high = (uint32) (rem >> 32);

	/* Reduce the thing a bit first */
	res = 0;
	if (high >= base) {
		high /= base;
		res = (uint64) high << 32;
		rem -= (uint64) (high * base) << 32;
	}

	while ((uint32) b > 0 && b < rem) {
		b = b + b;
		d = d + d;
	}

	do {
		if (rem >= b) {
			rem -= b;
			res += d;
		}
		b >>= 1;
		d >>= 1;
	} while (d);
	return (uint32) res;
}
#endif /* DOT11K_SUPPORT */
/******************************************************************************
* NAME:	 WFM_UMAC_FetchPreferredChannelFromTable
******************************************************************************/
/**
* \brief
* It finds the preferred channel info from the table based on Country string.
* \param pdot11CountryString - Pointer to Country String
* \param pFirstChannel	     - Pointer to first channel
* \param pMaxTransmitPower	 - Pointer to maximum transmit power
* \param pNumOfChannels	     - Pointer to number of channels
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE WFM_UMAC_FetchPreferredChannelFromTable(uint8 *pdot11CountryString,
														uint32 *pFirstChannel,
														sint32 *pMaxTransmitPower,
														uint32 *pNumOfChannels)
{
	WFM_STATUS_CODE StatusCode = WFM_STATUS_COUNTRY_NOT_FOUND_IN_TABLE;
	uint8 i = 0;
	uint8 bRegionCodeAvl = FALSE;
	uint8 RegionCode = 0;
	UMAC_PREFERRED_CHANNEL_INFO *pPreferredChannelInfo = NULL;
	/*Find the region code */
	bRegionCodeAvl = WFM_UMAC_ConvertCountryStrToRegionCode(pdot11CountryString, &RegionCode);
	if (!bRegionCodeAvl) {
		StatusCode = WFM_STATUS_COUNTRY_NOT_FOUND_IN_TABLE;
		goto exit_handler;
	}

	pPreferredChannelInfo = (UMAC_PREFERRED_CHANNEL_INFO *)&DefaultConfigRegionTable_2_4G[0];

	for (i = 0; i < WFM_REGION_CONFIG_TABLE_SIZE; i += sizeof(UMAC_PREFERRED_CHANNEL_INFO)) {
		if (RegionCode == pPreferredChannelInfo->CountryCode) {
			*pFirstChannel = (uint32)pPreferredChannelInfo->FirstChannel;
			*pMaxTransmitPower = (sint32)pPreferredChannelInfo->TxPower;
			*pNumOfChannels = (uint32)pPreferredChannelInfo->NoOfChannels;
			StatusCode = WFM_STATUS_SUCCESS;
			break;
		}
		pPreferredChannelInfo++;
	}
exit_handler:
	return StatusCode;
}

/******************************************************************************
 * NAME:	WFM_UMAC_ProcessErpProtection
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This processes incoming beacon and prob response frames.
 * \param UmacHandle	     - Handle to UMAC Instance.
 * \param *pLmacRxDescriptor - Incoming RxDescriptor from LMAC.
 * \param IsBeacon	     - Whether the incoming frame is a beacon or
 *                             Prob Response
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
WFM_STATUS_CODE WFM_UMAC_ProcessErpProtection(UMAC_HANDLE UmacHandle,
				     void *pLmacRxDescriptor,
				     uint8 IsBeacon)
{
	WFM_UMAC_MGMT_FRAME *umacMgmt = NULL;
	WFM_UMAC_BEACON_FRAME_BODY *pBeaconFrameBody = NULL;
	uint16 LengthOfIEElements = 0;
	uint16 FrameBodyLength = 0;
	uint16 SizeOfMem = 0;
	UMAC_RX_DESC *pRxDescriptor = NULL;
	WFM_STATUS_CODE Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	IeElement Ie = { 0 };

	uint32 basicRates = 0;
	uint32 supportedRates = 0;
	uint32 i = 0;
	uint8 ErpInfo = 0;
	uint8 Bssid[WFM_MAC_ADDRESS_SIZE] = {0, 0, 0, 0, 0, 0};
	uint8 BssidNull[WFM_MAC_ADDRESS_SIZE] = {0, 0, 0, 0, 0, 0};
	uint32 BssIndex = MAXIMUM_NUMBER_OF_NEIGHBORING_BSS;
	WSM_HI_UPDATE_IE_REQ *pUpdateIeReq = NULL;
	WFM_UMAC_ERP_IE *pUpdateERP = NULL;
	uint8 Is11bUserPresent = FALSE;

	OS_ASSERT(pUmacInstance);

	/*
	   Extract the information required, allocate memory and store it.
	   Then return memory to LMAC
	 */

	CHECK_POINTER_IS_VALID(pLmacRxDescriptor);
	pRxDescriptor = (UMAC_RX_DESC *) pLmacRxDescriptor;
	umacMgmt = (WFM_UMAC_MGMT_FRAME *)
	    RX_DESC_GET_DOT11FRAME(pRxDescriptor);
	CHECK_POINTER_IS_VALID(umacMgmt);
	OS_MemoryCopy(
			&Bssid[0],
			&umacMgmt->dot11hdr.Address3[0],
			WFM_MAC_ADDRESS_SIZE
			);
	pBeaconFrameBody = (WFM_UMAC_BEACON_FRAME_BODY *) umacMgmt->FrameBody;
	CHECK_POINTER_IS_VALID(pBeaconFrameBody);

	FrameBodyLength = (uint16) (GET_RX_DESC_FRAME_LENGTH(pRxDescriptor) - sizeof(WFM_UMAC_FRAME_HDR));

	/*
	   WFM_UMAC_BEACON_FRAME_BODY contains an arry called IEElements,
	   so we will have to  decrement 4 from size of beacon body
	 */

	LengthOfIEElements = FrameBodyLength - (sizeof(WFM_UMAC_BEACON_FRAME_BODY) - 4);

	BssIndex = UMAC_FindIndexInNeighborBssDB(UmacHandle, &Bssid[0]);
	if ((WFM_UMAC_ProcIeElements(pBeaconFrameBody->IEElements, LengthOfIEElements, UMAC_IE_ELEMENT_ERP_INFORMATION, &Ie) == TRUE)
		&& (Ie.size)) {
			ErpInfo = *(uint8 *) Ie.info;
			if (BssIndex != MAXIMUM_NUMBER_OF_NEIGHBORING_BSS) {
				if ((ErpInfo & WFM_UMAC_ERP_IE_NON_ERP_PRESENT) == WFM_UMAC_ERP_IE_NON_ERP_PRESENT) {
					pUmacInstance->ErpOLBC = TRUE;
					pUmacInstance->neighborBssDB[BssIndex].IsNonErpStaPresent = TRUE;
				} else {
					pUmacInstance->neighborBssDB[BssIndex].IsNonErpStaPresent = FALSE;
				}
			}
	}

	if ((WFM_UMAC_ProcIeElements(pBeaconFrameBody->IEElements, LengthOfIEElements, UMAC_IE_ELEMENT_SUPPORTED_RATES, &Ie) == TRUE)
		&& (Ie.size)) {
			if (BssIndex != MAXIMUM_NUMBER_OF_NEIGHBORING_BSS) {
				/*Converting the Basic Supported Rates to WSM Bitmask */
				WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate(Ie.size, Ie.info, &supportedRates, &basicRates);

				if (!(supportedRates & (~(UMAC_RATE_11B)))) {
					pUmacInstance->ErpOLBC = TRUE;
					pUmacInstance->neighborBssDB[BssIndex].Is11bOnly = TRUE;
				} else {
					pUmacInstance->neighborBssDB[BssIndex].Is11bOnly = FALSE;
				}
				/* A timer should also be started for timeout */
				/* clearing of 11b only flag should be done in that timeout call back */
			}
	}

	/* Here check all neighboring BSSs and set ErpOLBC accordinlgy */
	/* if 11b only and NonErpPresent both are FALSE then remove from data base */
	/* calculate Erp information and schedule */

	for (i = 0; i < MAXIMUM_NUMBER_OF_NEIGHBORING_BSS; i++) {
		if ((pUmacInstance->neighborBssDB[i].IsNonErpStaPresent == TRUE) ||
			(pUmacInstance->neighborBssDB[i].Is11bOnly == TRUE)) {
				pUmacInstance->ErpOLBC = TRUE;
				break;
		}
	}

	if (i == MAXIMUM_NUMBER_OF_NEIGHBORING_BSS) {
		pUmacInstance->ErpOLBC = FALSE;
		for (i = 1; i < MAX_NUM_CLIENTS_SUPPORTED; i++) {
			if (pUmacInstance->sta[i].Is11bUser) {
				Is11bUserPresent = TRUE;
				break;
			}
		}
	}

	for (i = 0; i < MAXIMUM_NUMBER_OF_NEIGHBORING_BSS; i++) {
		if ((pUmacInstance->neighborBssDB[i].IsNonErpStaPresent == FALSE) &&
			(pUmacInstance->neighborBssDB[i].Is11bOnly == FALSE)) {
				OS_MemoryCopy(
						&pUmacInstance->neighborBssDB[i].Bssid[0],
						&BssidNull[0],
						WFM_MAC_ADDRESS_SIZE
						);
		}
	}

	if (pUmacInstance->ErpOLBC == TRUE) {
		pUmacInstance->ErpInformation |= WFM_UMAC_ERP_IE_USE_PROTECTION;
		pUmacInstance->ErpUseCtsToSelf = TRUE;
	} else if (!Is11bUserPresent) {
		pUmacInstance->ErpInformation &= ~WFM_UMAC_ERP_IE_USE_PROTECTION;
		pUmacInstance->ErpUseCtsToSelf = FALSE;
	}

	if (pUmacInstance->ErpInformation != pUmacInstance->ErpInfoPrevious) {
		pUmacInstance->ErpIeToBeUpdated = TRUE;
		if ((pUmacInstance->ErpInformation & WFM_UMAC_ERP_IE_USE_PROTECTION) == WFM_UMAC_ERP_IE_USE_PROTECTION)
			pUmacInstance->ErpUseCtsToSelf = TRUE;
	} else {
		pUmacInstance->ErpIeToBeUpdated = FALSE;
	}
	return Result;

} /* end WFM_UMAC_ProcessErpProtection() */

/******************************************************************************
 * NAME:WFM_UMAC_DeAuthSTAUnicast
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the unicast de-authentication to an STA
 * This function performs the following things
 * -# Allocates memory for DeAuth frame and forms Authentication frame and
 *    queues it to LMAC for transmission.
 * \param UmacHandle		- Handle to UMAC Instance.
 * \param *StaMacAddress	- STA mac address.
 * \returns uint32.
 *****************************************************************************/
uint32 WFM_UMAC_DeAuthSTAUnicast(UMAC_HANDLE UmacHandle, uint8 *StaMacAddress)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_MGMT_FRAME *pDeAuthFrame = NULL;
	uint16 FrameBodyLen = 0;
	UMAC_TX_DESC *pTxDesc = NULL;
	UMAC_DEVIF_TX_DATA *pDevIfTxReq;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 broadcastAddr[WFM_MAC_ADDRESS_SIZE] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	uint8 remoteAddr[WFM_MAC_ADDRESS_SIZE];

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_ASSOC, "WFM_UMAC_DeAuthSTAUnicast()\n");

	WFM_UMAC_DBG_PRINT_TX_DESC();

	pDevIfTxReq = (UMAC_DEVIF_TX_DATA *)
	    UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pDevIfTxReq);

	pTxDesc = (UMAC_TX_DESC *) MOVE_TO_END_OF_DEV_IF_TX_REQ(pDevIfTxReq);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pDevIfTxReq, UMAC_DEAUTH_REQ);

	LOG_DATA(DBG_WFM_UMAC, "WFM_UMAC_DeAuthSTAUnicast pTxDesc=0x%x)\n", pTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pTxDesc);

	pDeAuthFrame = (WFM_UMAC_MGMT_FRAME *) TX_DESC_GET_DOT11FRAME(pTxDesc);
	CHECK_POINTER_IS_VALID(pDeAuthFrame);

	/*
	   Creating the WLAN Header
	*/

	OS_MemoryCopy(
			remoteAddr,
			StaMacAddress,
			WFM_MAC_ADDRESS_SIZE
			);

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
		LOG_EVENT(DBG_WFM_ERROR, " WFM_UMAC_DeAuthSTAUnicast() : Transmit Queue Full\n");
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
	return Result;
} /* end WFM_UMAC_DeAuthSTAUnicast() */
